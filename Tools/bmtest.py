
import chessmoves # Source: https://github.com/kervinck/floyd.git
import floyd as engine
import sys

def parseEpd(rawLine):

        # 4-field FEN
        line = rawLine.strip().split(' ', 4)
        pos = ' '.join(line[0:4])

        # EPD fields
        operations = {'bm': '', 'am': ''}
        fields = [op for op in line[4].split(';') if len(op) > 0]
        fields = [op.strip().split(' ', 1) for op in fields]
        operations.update(dict(fields))

        return pos, operations

nrPassed = 0
nrTests = 0

movetime = float(sys.argv[1])

for rawLine in sys.stdin:
        print rawLine,
        nrTests += 1
        pos, operations = parseEpd(rawLine)
        bm = [chessmoves.move(pos, bm, notation='uci')[0] for bm in operations['bm'].split()] # best move
        am = [chessmoves.move(pos, am, notation='uci')[0] for am in operations['am'].split()] # avoid move
        score, move = engine.search(pos, movetime=movetime, info='uci')
        print 'bestmove', move
        print 'test',
        if (len(bm) == 0 or move in bm) and\
           (len(am) == 0 or move not in am):
                print 'result OK',
                nrPassed += 1
        else:
                print 'result FAILED',
        print 'passed %d total %d' % (nrPassed, nrTests)
        print

