
import chessmoves
import floyd as engine
import sys

def parseEPD(rawLine):

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

for rawLine in sys.stdin:
        print rawLine,
        nrTests += 1
        pos, operations = parseEPD(rawLine)
        score, move = engine.search(pos, movetime=1.0, info='uci')
        print 'bestmove', move
        expected = [chessmoves.move(pos, bm, notation='uci')[0] for bm in operations['bm'].split()]
        print operations['id'],
        if move in expected:
                print 'OK',
                nrPassed += 1
        else:
                print 'FAILED',
        print '%d/%d' % (nrPassed, nrTests)
        print

