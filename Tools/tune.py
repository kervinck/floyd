#!/usr/bin/env python
#-----------------------------------------------------------------------
#
#       tune.py -- hill climbing of the evaluation vector
#
#-----------------------------------------------------------------------

import floyd as engine
import json
import math
import multiprocessing
import sys

#-----------------------------------------------------------------------
#       Options
#-----------------------------------------------------------------------

# Number of workers to launch (-n)
cpu = 4

# Search depth for evaluations. 0 means qSearch only (-d)
depth = 0

# Number of probes around best known value before narrowing the window (-s)
#nrSteps = 6 # 1.....2.....3..X..4.....5.....6
#nrSteps = 4 # 1.........2....X....3.........4
nrSteps = 2  # 1..............X..............2

# Number of active positions to find before entering quick and dirty mode (-m)
minActive = float('+Inf') # "dirty mode"

# Quit after calculating initial residual (-q)
quit = False

#-----------------------------------------------------------------------
#       readTestsFromEpd
#-----------------------------------------------------------------------

def readTestsFromEpd(input):
        """Read test postions from EPD
        Each line must contain a 4-field FEN followed by a 'result' opcode and operand,
        optionally followed by 'white' and 'black' opcodes for the players' ratings."""

        tests = []
        target = { '1-0' : 1.0, '0-1' : 0.0, '1/2-1/2' : 0.5 }

        for line in input:
                line = line.strip().split()
                pos = ' '.join(line[0:4])
                result = line[5].rstrip(';')
                if line[6] == 'white' and line[8] == 'black':
                        whiteElo = int(line[7].rstrip(';'))
                        blackElo = int(line[9].rstrip(';'))
                        pos = '%s 0 1 %d' % (pos, whiteElo - blackElo)
                tests.append((pos, target[result] if line[1] == 'w' else 1.0 - target[result]))

        return tests

#-----------------------------------------------------------------------
#       getVector
#-----------------------------------------------------------------------

def getVector():
        vector, names = [], []
        try:
                coef = 0
                while True:
                        value, id = engine.setCoefficient(coef, 0)
                        engine.setCoefficient(coef, value) # TODO: HACK
                        vector.append(value)
                        names.append(id)
                        coef += 1
        except IndexError:
                pass
        return vector, names

#-----------------------------------------------------------------------
#       evaluateVector
#-----------------------------------------------------------------------

def evaluateVector(tests, passive, useCache):
        """
        tests    : list of (fen, result) tuples
        passive  : (in/out) dict of positions whose score has stayed constant
        useCache : flag to indicate when cached values can be used
        """

        sumSquaredErrors = 0.0
        scores = []
        active = 0
        for pos, target in tests:
                if not useCache or pos not in passive:
                        score, move = engine.search(pos, depth) # slow
                        if pos in passive:
                                if  score != passive[pos]:
                                        del passive[pos]
                                        active += 1
                        else:
                                active += 1
                else:
                        score = passive[pos] # fast
                scores.append(score)
                p = scoreToP(score)
                sumSquaredErrors += (p - target) * (p - target) 

        return sumSquaredErrors, scores, active

#-----------------------------------------------------------------------
#       scoreToP
#-----------------------------------------------------------------------

def scoreToP(score):
        """Convert a score in pawns to a winning probability (0..1)"""
        return 1 / (1 + 10 ** (-score/4.0))

#-----------------------------------------------------------------------
#       tuneSingle
#-----------------------------------------------------------------------

def tuneSingle(coef, tests, initialValue, initialResidual):
        """Tune a single coefficient using robust a form of hill-climbing"""

        print 'evaluate id %s residual %.9f value %d' % (names[coef], initialResidual, initialValue)

        cache = { initialValue: initialResidual } # value -> residual

        bestValue, bestResidual = initialValue, initialResidual

        # Initial window scales with the magnitude of the initial value
        window = calcWindow(initialValue)

        # For switching to quick mode
        positions = [item[0] for item in tests]
        xNext()
        lastActive, streak = None, 0
        quick, dirty = False, False

        # Loop until an improvement is found or the search is exhausted
        exhausted = False
        while not exhausted:

                # Center the window around the current best value
                center = bestValue
                minValue, maxValue = center - window/2, center + window/2
                stepSize = window / (nrSteps - 1)

                # Walk through the range in equal steps. Always complete all steps
                exhausted = True
                for step in range(nrSteps):
                        nextValue = int(round(minValue + step * stepSize))
                        if nextValue in cache:
                                continue
                        exhausted = False

                        xSetCoefficient(coef, nextValue)
                        nextResidual, active = xEvaluateVector(tests, quick or dirty)
                        cache[nextValue] = nextResidual

                        print 'evaluate id %s residual %.9f' % (names[coef], nextResidual),
                        if not quick and not dirty:
                                print 'active %d' % active,
                        print 'value %d' % nextValue,

                        # Determine if the result is an improvement
                        if (nextResidual, abs(nextValue)) < (bestResidual, abs(bestValue)):
                                bestValue, bestResidual = nextValue, nextResidual
                                print 'best',
                                xUpdate()
                        print # newline

                        # Track changes in number of active positions for this parameter
                        if active != lastActive:
                                streak = 0
                        lastActive = active
                        streak += 1

                # Shrink the window if the best value is near the center
                if abs(bestValue - center) < window / 4:
                        if bestValue != initialValue:
                                break # Early termination (go to next parameter)
                        window /= 2
                        quick = streak >= nrSteps
                else:
                        if min(bestValue - minValue, maxValue - bestValue) < window / 8:
                                window *= 1.25 # Slight increase when at the edge

                dirty = lastActive >= minActive

        # Try a quadratic fit on the 5 nearest points
        X, Y = zip(*sorted(cache.items(), key=lambda item: abs(item[0] - bestValue)))
        nextValue = int(round(quadFit(X[:5], Y[:5])))
        if nextValue not in cache:
                xSetCoefficient(coef, nextValue)
                nextResidual, active = xEvaluateVector(tests, quick or dirty)

                print 'evaluate id %s residual %.9f' % (names[coef], nextResidual),
                if not quick and not dirty:
                        print 'active %d' % active,
                print 'value %d' % nextValue,

                if (nextResidual, abs(nextValue)) < (bestResidual, abs(bestValue)):
                        bestValue, bestResidual = nextValue, nextResidual
                        print 'best',
                        xUpdate()
                print # newline

        # Update vector
        xSetCoefficient(coef, bestValue)

        return bestValue, bestResidual, active

def quadFit(X, Y):
        """The extreme of a quadratic least square fit"""

        SumX   = sum(X)
        SumY   = sum(Y)
        SumX2  = sum([x * x for x in X])
        SumXY  = sum([x * y for x, y in zip(X, Y)])
        SumX3  = sum([x ** 3 for x in X])
        SumX2Y = sum([x * x * y for x, y in zip(X, Y)])
        SumX4  = sum([x ** 4 for x in X])

        n = float(len(X))
        Sxx   = SumX2  - SumX * SumX / n
        Sxy   = SumXY  - SumX * SumY / n
        Sxx2  = SumX3  - SumX * SumX2 / n
        Sx2y  = SumX2Y - SumX2 * SumY / n
        Sx2x2 = SumX4  - SumX2 * SumX2 / n

        a = Sx2y * Sxx   - Sxy  * Sxx2
        b = Sxy  * Sx2x2 - Sx2y * Sxx2

        if a != 0.0:
                return -b / (2 * a)
        else:
                return SumX / n

#-----------------------------------------------------------------------
#       calcWindow
#-----------------------------------------------------------------------

def calcWindow(value):
        sigmoid = 1.0 / (1.0 + math.exp(-value * 1e-3))
        slope = sigmoid * (1.0 - sigmoid)
        window = 0.02 / max(slope, 0.01) * 1e3 # clip when below 1% (queens, rooks)
        return window

#-----------------------------------------------------------------------
#       writeVector
#-----------------------------------------------------------------------

def writeVector(vector, deltas, filename):
        with open(filename, 'w') as fp:
                asList = sorted(zip(names, vector, deltas),  key=lambda x: x[2])
                json.dump(asList, fp, indent=1, separators=(',', ':'))
                fp.write('\n')

#-----------------------------------------------------------------------
#       Workers
#-----------------------------------------------------------------------

def startWorkers(count, tests, vector):
        if count == 0:
                return None

        N = len(tests)
        offsets = range(0, N, N//count)[:count] + [N]

        pipes = [multiprocessing.Pipe() for x in range(count)] # these are Python Connection objects

        workers = {}
        for x in range(count):
                pipe = pipes[x]
                i, j = offsets[x], offsets[x+1]
                process = multiprocessing.Process(target=runWorker, args=(pipe[1], tests[i:j], vector))
                workers[process] = pipe[0]

        for process in workers:
                process.start()

        return workers

def runWorker(pipe, tests, vector):
        for coef in range(len(vector)):
                engine.setCoefficient(coef, vector[coef])
        passive = {}
        positions = [item[0] for item in tests]
        bestScores = None
        message = pipe.recv()
        while message != None:
                if message[0] == 'update':
                        bestScores = lastScores
                elif message[0] == 'next':
                        passive = dict(zip(positions, bestScores))
                elif message[0] == 'setCoefficient':
                        (command, coef, value) = message
                        engine.setCoefficient(coef, value)
                elif message[0] == 'evaluateVector':
                        (command, useCache) = message
                        sumSquaredErrors, lastScores, active = evaluateVector(tests, passive, useCache)
                        pipe.send((sumSquaredErrors, len(tests), active))
                message = pipe.recv()

def stopWorkers(workers):
        for process, pipe in workers.items():
                pipe.send(None)
                process.join()
                del workers[process]

def xSetCoefficient(coef, value):
        for process, pipe in workers.items():
                pipe.send(('setCoefficient', coef, value))

def xUpdate():
        for process, pipe in workers.items():
                pipe.send(('update',))

def xNext():
        for process, pipe in workers.items():
                pipe.send(('next',))

def xEvaluateVector(tests, useCache):
        for process, pipe in workers.items():
                pipe.send(('evaluateVector', useCache))
        sumSquaredErrors, nrTests, nrActive = 0.0, 0, 0
        for process, pipe in workers.items():
                (subSumSquaredErrors, subNrTests, subNrActive) = pipe.recv()
                sumSquaredErrors += subSumSquaredErrors
                nrTests          += subNrTests
                nrActive         += subNrActive
        assert(nrTests == len(tests))
        return math.sqrt(sumSquaredErrors / nrTests), nrActive

#-----------------------------------------------------------------------
#       main
#-----------------------------------------------------------------------

if __name__ == '__main__':

        # -- Step 0: Get vector definition from module

        vector, names = getVector()
        deltas = [-1.0] * len(vector) # priority for new items

        # -- Step 1: Parse command line arguments

        if len(sys.argv) == 1:
                print 'Usage: python tune.py [ <option> ... ] <vector> [ <parameter> ... ]'
                print 'Arguments:'
                print '    vector         - JSON file to tune (input/output)'
                print '    parameter ...  - names of parameter(s) to tune (empty means all)'
                print 'Options:'
                print '    -n <cpu>       - number of parallel processes to use (default 4)'
                print '    -d <depth>     - search depth per position (default 0)'
                print '    -s <steps>     - probe steps before window shrink (default 2)'
                print '    -m <count>     - active positions before short-cut evaluation (default +Inf)'
                print '    -q             - print the current residual and quit'
                sys.exit(0)

        argi = 1

        while len(sys.argv) >= argi and sys.argv[argi][0] == '-':

                if sys.argv[argi] == '-n':
                        cpu = int(sys.argv[argi+1])
                        argi += 2
                        continue

                if sys.argv[argi] == '-d':
                        depth = int(sys.argv[argi+1])
                        argi += 2
                        continue

                if sys.argv[argi] == '-s':
                        nrSteps = int(sys.argv[argi+1])
                        argi += 2
                        continue

                if sys.argv[argi] == '-m':
                        minActive = int(sys.argv[argi+1])
                        argi += 2
                        continue

                if sys.argv[argi] == '-q':
                        quit = True
                        argi += 1
                        continue

                print 'No such option:', sys.argv[argi]
                sys.exit(1)

        # Read vector from file, if any
        filename = sys.argv[argi]
        argi += 1
        try:
                with open(filename, 'r') as fp:
                        values = dict(zip(names, vector))
                        for i, (name, value, delta) in enumerate(json.load(fp)):
                                try:
                                        coef = names.index(name)
                                        values[name] = value
                                        deltas[coef] = delta
                                except ValueError:
                                        print 'warning invalid id', name
                        vector = [values[name] for name in names]
        except IOError as err:
                print err
                print 'continue'

        coefList = range(len(vector))
        if len(sys.argv) > argi:
                coefList = [coef for coef in coefList if names[coef] in sys.argv[argi:]]

        # -- Step 2: Read positions from stdin

        tests = readTestsFromEpd(sys.stdin)
        workers = startWorkers(cpu, tests, vector)

        # -- Step 3: Prepare. Calculate initial scores and residual

        bestResidual, nrActive = xEvaluateVector(tests, False)

        print 'vector filename %s residual %.9f positions %d depth %d' % (repr(filename), bestResidual, len(tests), depth)
        print

        if quit:
                stopWorkers(workers)
                sys.exit(0)
        xUpdate()

        # -- Step 4: Tune all, half the set and repeat tuning until no more halving

        nrRounds = 0
        exitValue = 1
        exhausted = False
        coefList = sorted(coefList, key=lambda x:deltas[x])
        while len(coefList) > 0 and not exhausted:
                nrRounds += 1
                print 'round %d count %d' % (nrRounds, len(coefList))
                print

                exhausted = True
                for coef in coefList:
                        oldValue = vector[coef]
                        newValue, newResidual, active = tuneSingle(coef, tests, oldValue, bestResidual)

                        deltaResidual = newResidual - bestResidual
                        deltas[coef] = deltaResidual

                        if newValue != oldValue:
                                print 'update id %s residual %.9f delta %.3e active %d oldValue %d newValue %d' % (
                                        names[coef], newResidual, deltaResidual, active, oldValue, newValue)
                                vector[coef] = newValue
                                exitValue = 0
                                bestResidual = newResidual
                                exhausted = False
                        writeVector(vector, deltas, filename)
                        print

                # Keep the most volatile half for the next round
                coefList = sorted(coefList, key=lambda x:deltas[x])
                newLen = len(coefList) // 2
                coefList = coefList[:newLen]

        # -- Step 5: Report result and exit

        print 'vector filename %s residual %.9f positions %d depth %d' % (repr(filename), bestResidual, len(tests), depth)
        print

        stopWorkers(workers)
        sys.exit(exitValue)

#-----------------------------------------------------------------------
#
#-----------------------------------------------------------------------

