
import chessmoves
import floyd as engine
import sys

depth = 6

def parseEPD(rawLine):

        # 4-field FEN
        line = rawLine.strip().split(' ', 4)
        pos = ' '.join(line[0:4])

        # EPD fields
        operations = [op for op in line[4].split(';') if len(op) > 0]
        operations = [op.strip().split(' ', 1) for op in operations]
        operations = dict(operations)

        return pos, operations

passed = 0
nrTests = 0

for rawLine in sys.stdin:
        nrTests += 1
        pos, operations = parseEPD(rawLine)
        print (pos, operations)
        expected = [chessmoves.move(pos, bm, notation='uci')[0] for bm in operations['bm'].split()]
        print 'Expected:', expected
        score, pv = engine.search(pos, depth)
        print score, pv
        if pv[0] in expected:
                print 'OK'
                passed += 1
        else:
                print 'FAILED'
        print '%d/%d' % (passed, nrTests)
        print

