
import chessmoves # Source: https://github.com/kervinck/chessmoves.git
import floyd as engine
import multiprocessing
import sys

def parseEpd(rawLine):
        # 4-field FEN
        line = rawLine.strip().split(' ', 4)
        pos = ' '.join(line[0:4])

        # EPD fields
        operations = {'bm': '', 'am': '', 'dm': ''}
        fields = [op for op in line[4].split(';') if len(op) > 0]
        fields = [op.strip().split(' ', 1) for op in fields]
        operations.update(dict(fields))

        return pos, operations

def startWorkers(cpu, lines, moveTime):
        pipes = [multiprocessing.Pipe() for x in range(cpu)] # Python Connection objects
        N = len(lines)
        offsets = range(0, N, N//cpu)[:cpu] + [N]
        workers = {}
        for x in range(cpu):
                pipe = pipes[x]
                i, j = offsets[x], offsets[x+1]
                process = multiprocessing.Process(target=runWorker, args=(pipe[1], lines[i:j], i, moveTime))
                workers[process] = pipe[0]
        for process in workers:
                process.start()
        return workers

def runWorker(pipe, lines, i, moveTime):
        nrPassed = 0
        for rawLine in lines:
                i += 1
                pos, operations = parseEpd(rawLine)
                bm = [chessmoves.move(pos, bm, notation='uci')[0] for bm in operations['bm'].split()] # best move
                am = [chessmoves.move(pos, am, notation='uci')[0] for am in operations['am'].split()] # avoid move
                dm = [int(dm) for dm in operations['dm'].split()] # mate distance
                score, move = engine.search(pos, movetime=moveTime, info=None)
                mate = None
                if score >=  31.0: mate =  32.0 - score
                if score <= -31.0: mate = -32.0 - score
                if mate is not None:
                        mate = (int(round(mate * 1000.0)) + 1) // 2
                if (len(bm) == 0 or move in bm) and\
                   (len(am) == 0 or move not in am) and\
                   (len(dm) == 0 or mate in dm):
                        print '%5d OK  bestmove %-5s score %+7.3f mate %-4s epd %s' % (i, move, score, mate, rawLine),
                        nrPassed += 1
                else:
                        print '%5d NOK bestmove %-5s score %+7.3f mate %-4s epd %s' % (i, move, score, mate, rawLine),
        pipe.send((nrPassed, len(lines)))

def stopWorkers(workers):
        nrPassed, nrTests = 0, 0
        for process, pipe in workers.items():
                message = pipe.recv()
                nrPassed += message[0]
                nrTests += message[1]
                process.join()
                del workers[process]
        print 'passed %d total %d' % (nrPassed, nrTests)

if __name__ == '__main__':
        argi = 1
        if sys.argv[argi] == '-n':
                cpu = int(sys.argv[argi+1])
                argi += 2
        else:
                cpu = multiprocessing.cpu_count()
                cpu = cpu // 2 if cpu > 1 else 1
        moveTime = float(sys.argv[argi])
        lines = sys.stdin.readlines()
        workers = startWorkers(min(cpu, len(lines)), lines, moveTime)
        stopWorkers(workers)
