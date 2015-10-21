
import floyd as engine
import sys

depth = int(sys.argv[1])
for rawLine in sys.stdin:
        print rawLine,
        score, move = engine.search(rawLine, depth=depth, info='uci')
        print

