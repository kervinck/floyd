
import floyd

for pos, depth in [
        ('rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -', 0),    # startPosition
        ('rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq -', 0),  # 1. e4

        ('rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBN1 w Qkq -', 0),     # without Rh1
        ('rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/1NBQKBNR w Kkq -', 0),     # without Ra1
        ('rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/1NBQKBN1 w kq -', 0),      # without both rooks

        ('rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKB1R w KQkq -', 0),    # without Ng1
        ('rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/R1BQKBNR w KQkq -', 0),    # without Nb1
        ('rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/R1BQKB1R w KQkq -', 0),    # without Nb1 Ng1

        ('rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQK1NR w KQkq -', 0),    # without Bf1
        ('rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RN1QKBNR w KQkq -', 0),    # without Bc1
        ('rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RN1QK1NR w KQkq -', 0),    # without Bc1 Bf1

        ('rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNB1KBNR w KQkq -', 0),

        ('rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKB1R w KQkq -', 7),
        ('2b1r3/5pbk/1R4pp/p2P4/4q3/4BN1P/P2Q1KP1/8 w - -', 0),
]:
        print pos, depth, floyd.evaluate(pos)
        print floyd.search(pos, depth, info='uci')
        print
