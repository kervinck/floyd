
/*----------------------------------------------------------------------+
 |                                                                      |
 |      Board.h                                                         |
 |                                                                      |
 +----------------------------------------------------------------------*/

/*
 *  Copyright (C) 2015-2016, Marcel van Kervinck
 *  All rights reserved
 *
 *  Please read the enclosed file `LICENSE' or retrieve this document
 *  from https://marcelk.net/floyd/LICENSE for terms and conditions.
 */

/*----------------------------------------------------------------------+
 |      Definitions                                                     |
 +----------------------------------------------------------------------*/

#include "geometry.h"

static const int rankStep = rank2 - rank1;
static const int fileStep = fileB - fileA;

typedef struct Board *Board_t;

struct side {
        unsigned char attacks[boardSize];
        int king;
};

/*
 *      +-----+-----+-----+-----+-----+-----+-----+-----+
 *      |   Pawns   |   Minors  |   Rooks   |Queen|King |
 *      +-----+-----+-----+-----+-----+-----+-----+-----+
 *           7..6        5..4        3..2      1     0
 */

enum {
        attackKing   = 1,
        attackQueen  = 2,
        attackRook   = 4,
        attackMinor  = 16,
        attackPawn   = 64,
};

#define maxMoves 256
#define maxMoveSize sizeof("a7-a8=N+")
#define maxFenSize 128

struct Board {
        signed char squares[boardSize];
        signed char castleFlags;
        signed char enPassantPawn;
        signed char halfmoveClock;

        int plyNumber; // holds both side to move and full move number

        uint64_t hash;
        uint64List hashHistory;

        int eloDiff;

        /*
         *  Side data
         */
        struct side sides[2];
        int sideInfoPlyNumber; // for auto-update

        /*
         *  Move undo administration
         */
        sByteList undoStack;

        int *movePtr; // Used only during move generation
};

/*
 *  Chess pieces
 */

enum piece {
        empty,
        whiteKing, whiteQueen, whiteRook, whiteBishop, whiteKnight, whitePawn,
        blackKing, blackQueen, blackRook, blackBishop, blackKnight, blackPawn
};

enum pieceColor { white = 0, black = 1 };

#define pieceColor(piece) ((piece) >= blackKing) // piece must not be 'empty'

/*
 *  Game state
 */

enum castleFlag {
        castleFlagWhiteKside = 1 << 0,
        castleFlagBlackKside = 1 << 1,
        castleFlagWhiteQside = 1 << 2,
        castleFlagBlackQside = 1 << 3
};

#define sideToMove(board) ((board)->plyNumber & 1)
#define other(side) ((side) ^ 1)

/*
 *  Moves
 */

/*
 *  Move integer bits are as follows:
 *  0-5         to square
 *  6-11        from square
 *  12          special flag (castling, promotion, en passant capture, double pawn push)
 *  13-14       promotion: Q=0, R=1, B=2, N=3
 */

#define boardBits  6

#define move(from, to)          (((from) << boardBits) | (to))
enum moveFlags {
        specialMoveFlag         = 1 << (2*boardBits),
        promotionBits           = 2*boardBits + 1,
        queenPromotionFlags     = 0 << promotionBits,
        rookPromotionFlags      = 1 << promotionBits,
        bishopPromotionFlags    = 2 << promotionBits,
        knightPromotionFlags    = 3 << promotionBits
};
#define specialMove(from, to)   (specialMoveFlag | move(from, to))

#define from(move)              (((move) >> boardBits) & ~(~0<<boardBits))
#define to(move)                ( (move)               & ~(~0<<boardBits))

/*----------------------------------------------------------------------+
 |      Data                                                            |
 +----------------------------------------------------------------------*/

extern const char startpos[];

// Expose these for use in evaluation (king safety)
extern const unsigned char kingDirections[];
extern const signed char kingStep[];

/*----------------------------------------------------------------------+
 |      Functions                                                       |
 +----------------------------------------------------------------------*/

/*
 *  Setup chess board from position description in FEN notation
 *  Return the length of the FEN on success, or 0 on failure.
 */
int setupBoard(Board_t self, const char *fen);

/*
 *  Update attack tables and king locations. To be used after
 *  setupBoard or makeMove. Used by generateMoves, inCheck.
 *  Can be invalidated by moveToStandardAlgebraic,
 *  getCheckMark, isLegalMove, normalizeEnPassantStatus.
 */
void updateSideInfo(Board_t self);

/*
 *  Convert the current position to FEN
 */
void boardToFen(Board_t self, char *fen);

/*
 *  Compute a 64-bit hash for the current position using Polyglot-Zobrist hashing
 */
uint64_t hash(Board_t self);

/*
 *  Generate all pseudo-legal moves for the position and return the move count
 */
int generateMoves(Board_t self, int moveList[maxMoves]);

/*
 *  Make the move on the board
 */
void makeMove(Board_t self, int move);

/*
 *  Check if last pseudo move was indeed legal
 */
static inline bool wasLegalMove(Board_t self)
{
        updateSideInfo(self);
        int side = sideToMove(self);
        int xking = self->sides[other(side)].king;
        return self->sides[side].attacks[xking] == 0;
}

/*
 *  Retract the last move and restore the previous position
 *  All moves can be undone all the way back to the setup position
 */
void undoMove(Board_t self);

/*
 *  Null moves
 */
void makeNullMove(Board_t self);

/*
 *  Convert move to computer notation (UCI)
 */
char *moveToUci(Board_t self, char moveString[maxMoveSize], int move);

/*
 *  Parse move input, disambiguate abbreviated notations
 *  A movelist must be prepared by the caller for disambiguation.
 *  Return the length of the move on success, or 0 on failure
 *  (invalid move syntax or not a legal move)
 */
extern int parseUciMove(Board_t self, const char *line, int xmoves[maxMoves], int xlen, int *move);

// Clear the ep flag if there are not legal moves
extern void normalizeEnPassantStatus(Board_t self);

// Side to move in check?
extern int inCheck(Board_t self);

// Is move legal? Move must come from generateMoves, so be safe to make.
extern bool isLegalMove(Board_t self, int move);

// Is the move a pawn promotion?
extern bool isPromotion(Board_t self, int from, int to);

// Search tree to fixed depth for correctness testing
extern long long moveTest(Board_t self, int depth);

// Target square in case the last move was a capture, or -1 otherwise
extern int recaptureSquare(Board_t self);

/*----------------------------------------------------------------------+
 |                                                                      |
 +----------------------------------------------------------------------*/

