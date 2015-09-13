
import floyd

for pos, depth in [
        ('rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKB1R w KQkq -', 0),
        ('rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKB1R w KQkq -', 1),
        ('rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKB1R w KQkq -', 2),
        ('rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKB1R w KQkq -', 3),
        ('rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKB1R w KQkq -', 4),
        ('rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKB1R w KQkq -', 5),
        ('rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKB1R w KQkq -', 6),
        ('2b1r3/5pbk/1R4pp/p2P4/4q3/4BN1P/P2Q1KP1/8 w - -', 0),
]:
        print pos, depth, floyd.evaluate(pos), floyd.search(pos, depth)
