
import chessmoves
import floyd as engine
import sys

def parseEpd(rawLine):

        # 4-field FEN
        line = rawLine.strip().split(' ', 4)
        pos = ' '.join(line[0:4])

        # EPD fields
        operations = [op for op in line[4].split(';') if len(op) > 0]
        operations = [op.strip().split(' ', 1) for op in operations]
        operations = dict(operations)

        return pos, operations

nrPassed = 0
nrTests = 0

movetime = float(sys.argv[1])

for rawLine in sys.stdin:
        print rawLine,
        nrTests += 1
        pos, operations = parseEpd(rawLine)
        score, move = engine.search(pos, movetime=movetime, info='uci')
        print 'bestmove', move
        expected = [chessmoves.move(pos, bm, notation='uci')[0] for bm in operations['bm'].split()]
        print 'test',
        if move in expected:
                print 'result OK',
                nrPassed += 1
        else:
                print 'result FAILED',
        print 'passed %d total %d' % (nrPassed, nrTests)
        print

