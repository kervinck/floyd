
/*----------------------------------------------------------------------+
 |                                                                      |
 |      evaluate.c                                                      |
 |                                                                      |
 +----------------------------------------------------------------------*/

/*----------------------------------------------------------------------+
 |      Includes                                                        |
 +----------------------------------------------------------------------*/

#define _XOPEN_SOURCE

// C standard includes
#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>

// Generic C extensions
#include "cplus.h"

// Other includes
#include "Board.h"
#include "kpk.h"

// Own interface
#include "evaluate.h"

/*----------------------------------------------------------------------+
 |      Definitions                                                     |
 +----------------------------------------------------------------------*/

#define other(side) ((side) ^ 1)
#define flip(fileOrRank) ((fileOrRank) ^ 7)

#define isKingFlankFile(file) (((file) ^ fileD) >> 2)

enum vector {
        #define X(id,value) id
        #include "vector.h"
        #undef X
};

/*----------------------------------------------------------------------+
 |      Data                                                            |
 +----------------------------------------------------------------------*/

const char * const vectorLabels[] = {
        #define X(id,value) #id
        #include "vector.h"
        #undef X
};

const int vectorLen = arrayLen(vectorLabels);

// TODO: import the json file (at compile time)
int globalVector[] = {
        #define X(id,value) [id] = (value)
        #include "vector.h"
        #undef X
};

/*----------------------------------------------------------------------+
 |      Functions                                                       |
 +----------------------------------------------------------------------*/

static double sigmoid(double x);
static double logit(double p);
static int squareOf(Board_t self, int piece);
static int evaluatePawn(const int v[vectorLen],
                        const char minRank[][2], const char maxRank[][2], int R,
                        int file, int side,
                        int king, int xking);

/*----------------------------------------------------------------------+
 |      evaluate                                                        |
 +----------------------------------------------------------------------*/

int evaluate(Board_t self, const int v[vectorLen], struct evaluation *Cc)
{
        struct evaluation e;
        memset(&e, 0, sizeof e);

        if (v == null)
                v = globalVector;

        /*--------------------------------------------------------------+
         |      Feature extraction                                      |
         +--------------------------------------------------------------*/

        // Update attack tables and king locations
        updateSideInfo(self);

        /*
         *  First and last pawn rank (per file), to extract passers,
         *  open files, doubled pawns, etc
         */
        char minPawnRank[10][2];
        char maxPawnRank[10][2];
        memset(minPawnRank, 7, sizeof minPawnRank);
        memset(maxPawnRank, 0, sizeof maxPawnRank);

        /*
         *  Outmost files with pawns, to extract pawn span and center
         */
        char minPawnFile[2] = { 7, 7};
        char maxPawnFile[2] = { 0, 0};
#if 0
        char pawnSpan[2]; // 0 .. 8
        char pawnCenter[2]; // 0 .. 14  target for kings?
#endif

        // Scan board for pieces
        for (int square=0; square<boardSize; square++) {

                int piece = self->squares[square];
                if (piece == empty) continue;

                int side = pieceColor(piece);

                int file = file(square);
                int rank = rank(square);

                // abstract "even" or "odd" square color
                int squareColor = (file ^ rank) & 1;

                switch (piece) {
                case whiteKing:
                case blackKing:
                        e.nrKings[side]++;
                        break;

                case whiteQueen:
                case blackQueen:
                        e.nrQueens[side]++;
                        break;

                case whiteRook:
                case blackRook:
                        e.nrRooks[side]++;
                        break;

                case whiteBishop:
                case blackBishop:
                        e.nrBishops[side]++;
                        e.nrBishopsX[side] += squareColor;
                        break;

                case whiteKnight:
                case blackKnight:
                        e.nrKnights[side]++;
                        break;

                case whitePawn:
                case blackPawn:
                        e.nrPawns[side]++;
                        e.nrPawnsX[side] += squareColor;

                        setMin(minPawnRank[1+file][side], rank ^ rank1);
                        setMax(maxPawnRank[1+file][side], rank ^ rank1);

                        setMin(minPawnFile[side], file);
                        setMax(maxPawnFile[side], file);
                        break;
                }
        }

        // Useful piece counters
        int nrKings   = e.nrKings[white]   + e.nrKings[black];
        int nrQueens  = e.nrQueens[white]  + e.nrQueens[black];
        int nrRooks   = e.nrRooks[white]   + e.nrRooks[black];
        int nrBishops = e.nrBishops[white] + e.nrBishops[black];
        int nrKnights = e.nrKnights[white] + e.nrKnights[black];
        int nrPawns   = e.nrPawns[white]   + e.nrPawns[black];

        assert(nrKings == 2);

        // Lump together the minors for convenience
        e.nrMinors[white] = e.nrBishops[white] + e.nrKnights[white];
        e.nrMinors[black] = e.nrBishops[black] + e.nrKnights[black];

        /*--------------------------------------------------------------+
         |      Material                                                |
         +--------------------------------------------------------------*/

        for (int side=white; side<=black; side++) {
                int xside = other(side);

                e.material[side] =
                        e.nrQueens[side] * (
                               + v[queenValue]
                               + v[queenAndQueen]   * (e.nrQueens[side] - 1)
                               + v[queenAndRook]    * e.nrRooks[side]
                               + v[queenAndBishop]  * e.nrBishops[side]
                               + v[queenAndKnight]  * e.nrKnights[side]
                               + v[queenAndPawn_1]  * e.nrPawns[side]
                               + v[queenAndPawn_2]  * e.nrPawns[side] * e.nrPawns[side] /8
                               + v[queenVsRook]     * e.nrRooks[xside]
                               + v[queenVsBishop]   * e.nrBishops[xside]
                               + v[queenVsKnight]   * e.nrKnights[xside]
                               + v[queenVsPawn_1]   * e.nrPawns[xside]
                               + v[queenVsPawn_2]   * e.nrPawns[xside] * e.nrPawns[xside] /8
                        ) + e.nrRooks[side] * (
                               + v[rookValue]
                               + v[rookAndRook]     * (e.nrRooks[side] - 1)
                               + v[rookAndBishop]   * e.nrBishops[side]
                               + v[rookAndKnight]   * e.nrKnights[side]
                               + v[rookAndPawn_1]   * e.nrPawns[side]
                               + v[rookAndPawn_2]   * e.nrPawns[side] * e.nrPawns[side] /8
                               + v[rookVsBishop]    * e.nrBishops[xside]
                               + v[rookVsKnight]    * e.nrKnights[xside]
                               + v[rookVsPawn_1]    * e.nrPawns[xside]
                               + v[rookVsPawn_2]    * e.nrPawns[xside] * e.nrPawns[xside] /8
                        ) + e.nrBishops[side] * (
                               + v[bishopValue]
                               + v[bishopAndBishop] * (e.nrBishops[side] - 1)
                               + v[bishopAndKnight] * e.nrKnights[side]
                               + v[bishopAndPawn_1] * e.nrPawns[side]
                               + v[bishopAndPawn_2] * e.nrPawns[side] * e.nrPawns[side] /8
                               + v[bishopVsKnight]  * e.nrKnights[xside]
                               + v[bishopVsPawn_1]  * e.nrPawns[xside]
                               + v[bishopVsPawn_2]  * e.nrPawns[xside] * e.nrPawns[xside] /8
                        ) + e.nrKnights[side] * (
                               + v[knightValue]
                               + v[knightAndKnight] * (e.nrKnights[side] - 1)
                               + v[knightAndPawn_1] * e.nrPawns[side]
                               + v[knightAndPawn_2] * e.nrPawns[side] * e.nrPawns[side] /8
                               + v[knightVsPawn_1]  * e.nrPawns[xside]
                               + v[knightVsPawn_2]  * e.nrPawns[xside] * e.nrPawns[xside] /8
                        );

                for (int i=0; i<e.nrPawns[side]; i++)
                        e.material[side] += v[pawnValue1+i];
        }

        /*--------------------------------------------------------------+
         |      Board control                                           |
         +--------------------------------------------------------------*/

        #define isCenter(sq)\
                  ((sq)==d4 || (sq)==d5 \
                || (sq)==e4 || (sq)==e5)

        #define isExtendedCenter(sq)\
                  ((sq)==c3 || (sq)==c4 || (sq)==c5 || (sq)==c6 \
                || (sq)==d3 ||                         (sq)==d6 \
                || (sq)==e3 ||                         (sq)==e6 \
                || (sq)==f3 || (sq)==f4 || (sq)==f5 || (sq)==f6)

        for (int square=0; square<boardSize; square++) {
                int wAttack = self->whiteSide.attacks[square];
                int bAttack = self->blackSide.attacks[square];

                int controlValue;
                if (isCenter(square))
                        controlValue = v[controlCenter];
                else if (isExtendedCenter(square))
                        controlValue = v[controlExtendedCenter];
                else
                        controlValue = v[controlOutside];

                if (wAttack > bAttack)
                        e.control[white] += controlValue;

                if (wAttack < bAttack)
                        e.control[black] += controlValue;
        }

        /*--------------------------------------------------------------+
         |      Pawns                                                   |
         +--------------------------------------------------------------*/

        for (int file=fileA; file!=fileH+fileStep; file+=fileStep) {
                e.pawns[white] += evaluatePawn(v,
                                              &minPawnRank[1+file],
                                              &maxPawnRank[1+file], 0,
                                              file,
                                              white,
                                              self->whiteSide.king,
                                              self->blackSide.king);
                e.pawns[black] += evaluatePawn(v,
                                              &maxPawnRank[1+file], // Note: reverse min/max
                                              &minPawnRank[1+file], 7,
                                              file,
                                              black,
                                              self->blackSide.king,
                                              self->whiteSide.king);
        }

        /*--------------------------------------------------------------+
         |      Draw rate prediction ("draw")                           |
         +--------------------------------------------------------------*/

        int drawScore = v[drawOffset]
                + nrQueens  * v[drawQueen]
                + nrRooks   * v[drawRook]
                + nrBishops * v[drawBishop]
                + nrKnights * v[drawKnight]
                + nrPawns   * v[drawPawn];

        if (nrQueens > 0 && nrRooks + nrBishops + nrKnights == 0)
                drawScore += v[drawQueenEnding];

        if (nrRooks > 0 && nrQueens + nrBishops + nrKnights == 0)
                drawScore += v[drawRookEnding];

        if (nrBishops > 0 && nrQueens + nrRooks + nrKnights == 0)
                drawScore += v[drawBishopEnding];

        if (nrKnights > 0 && nrQueens + nrRooks + nrBishops == 0)
                drawScore += v[drawKnightEnding];

        if (nrPawns > 0 && nrQueens + nrRooks + nrBishops + nrKnights == 0)
                drawScore += v[drawPawnEnding];

        if (nrPawns == 0)
                drawScore += v[drawPawnless];

        int wUnlike0 = e.nrBishops[white] > e.nrBishopsX[white];
        int wUnlike1 = e.nrBishopsX[white] > 0;
        int bUnlike0 = e.nrBishops[black] > e.nrBishopsX[black];
        int bUnlike1 = e.nrBishopsX[black] > 0;
        if (wUnlike0 != wUnlike1 && bUnlike0 != bUnlike1
         && wUnlike0 != bUnlike0 && wUnlike1 != bUnlike1) {
                if (nrQueens > 0)
                        drawScore += v[drawUnlikeBishopsAndQueens];
                else if (nrRooks > 0)
                        drawScore += v[drawUnlikeBishopsAndRooks];
                else if (nrKnights > 0)
                        drawScore += v[drawUnlikeBishopsAndKnights];
                else
                        drawScore += v[drawUnlikeBishops];
        }

        // Each side has a piece the other doesn't have (don't count pawns)
        int deltaQueens = e.nrQueens[white] - e.nrQueens[black];
        int deltaRooks  = e.nrRooks[white]  - e.nrRooks[black];
        int deltaMinors = e.nrMinors[white] - e.nrMinors[black];
        if (min(min(deltaQueens, deltaRooks), deltaMinors) < 0
         && max(max(deltaQueens, deltaRooks), deltaMinors) > 0) {
                if (deltaQueens) drawScore += v[drawQueenImbalance];
                if (deltaRooks)  drawScore += v[drawRookImbalance];
                if (deltaMinors) drawScore += v[drawMinorImbalance];
        }

        /*--------------------------------------------------------------+
         |      Subtotal                                                |
         +--------------------------------------------------------------*/

        int side = sideToMove(self);
        int xside = other(side);

        #define partial(side) (\
                  e.material[side] \
                + e.safety[side] \
                + e.passers[side] \
                + e.control[side] \
                + e.mobility[side] \
                + e.kings[side] \
                + e.queens[side] \
                + e.rooks[side] \
                + e.bishops[side] \
                + e.knights[side] \
                + e.pawns[side])

        int wiloScore = partial(side) - partial(xside);

        wiloScore += v[tempo]; // side to move bonus

        /*--------------------------------------------------------------+
         |      Contempt                                                |
         +--------------------------------------------------------------*/

        if (side == white)
                wiloScore += self->eloDiff * v[eloDiff] / 10;
        else
                wiloScore -= self->eloDiff * v[eloDiff] / 10;

        /*--------------------------------------------------------------+
         |      Special endgames                                        |
         +--------------------------------------------------------------*/

        // Ignore multiple same-side bishops on the same square color
        #define nrEffectiveBishops(side) (                     \
                (e.nrBishops[side] - e.nrBishopsX[side] > 0) + \
                (e.nrBishopsX[side] > 0))

        int nrEffectiveBishops = nrEffectiveBishops(white) + nrEffectiveBishops(black);
        int nrEffectivePieces = nrKings + nrQueens + nrRooks + nrEffectiveBishops + nrKnights + nrPawns;

        if (nrEffectivePieces == 2)
                wiloScore = 0;

        if (nrEffectivePieces == 3) {
                if (e.nrQueens[side] + e.nrRooks[side] > 0)
                        wiloScore += v[winBonus];

                if (e.nrQueens[xside] + e.nrRooks[xside] > 0)
                        wiloScore -= v[winBonus];

                if (nrBishops + nrKnights > 0)
                        wiloScore = 0;

                if (nrPawns > 0) {
                        int egtSide, wKing, wPawn, bKing;

                        if (e.nrPawns[white] == 1) {
                                egtSide = side;
                                wKing = self->whiteSide.king;
                                wPawn = squareOf(self, whitePawn);
                                bKing = self->blackSide.king;
                        } else {
                                egtSide = xside;
                                wKing = square(0, 7) ^ self->blackSide.king;
                                wPawn = square(0, 7) ^ squareOf(self, blackPawn);
                                bKing = square(0, 7) ^ self->whiteSide.king;
                        }

                        int egtScore = kpkProbe(egtSide, wKing, wPawn, bKing);
                        if (egtScore == 0)
                                wiloScore = 0;
                        else
                                wiloScore += egtScore * v[winBonus];
                }
        }

        /*--------------------------------------------------------------+
         |      Total                                                   |
         +--------------------------------------------------------------*/

        e.wiloScore = wiloScore;
        e.drawScore = drawScore;

        // Combine wiloScore and drawScore into an expected game result
        double Wp = sigmoid(wiloScore * 1e-3);
        double D = sigmoid(drawScore * 1e-3);
        e.P = 0.5 * D + Wp - D * Wp;

        static const double Ci = 4.0 / M_LN10;
        e.score = round(Ci * logit(e.P) * 1e+3);

        /*--------------------------------------------------------------+
         |      Return                                                  |
         +--------------------------------------------------------------*/

        // Keep copy of the calculation if so requested
        if (Cc != null)
                *Cc = e;

        return e.score;
}

/*----------------------------------------------------------------------+
 |      evaluatePawn                                                    |
 +----------------------------------------------------------------------*/

/*
 *  TODO: `file' is given as 0 for fileA, regardless of the definition of fileA
 */

static int evaluatePawn(const int v[vectorLen],
                        const char minRank[][2], const char maxRank[][2], int R, // For flipping ranks
                        int file, int side,
                        int king, int xking)
{
        int first = maxRank[0][side] ^ R; // Most advanced pawn, if any
        int last  = minRank[0][side] ^ R; // Most backward pawn, if any

        if (last > first) // No pawn
                return 0;

        int pawnScore = 0;
        int F = isKingFlankFile(file(king)) ? fileA : fileH; // For flipping files TODO: flip 'file'
        int xside = other(side);

#if 0
        // TODO: one file and one rank must be neutral
        // OR: make the average neutral, which is much nicer
        pawnScore = v[pawnA+(file^F)] + v[pawn2+(first^R)-1]; // Location
#endif

        /*
         *  Pawn rank and file
         *
         *  Correct down for average to avoid tuning problems,
         *  effectively creating only 7+5 = 12 degrees of freedom
         *  for what is effectively a static pawn PST.
         */

        int pawnCommon = v[pawnA] + v[pawnB] + v[pawnC] + v[pawnD] +
                         v[pawnE] + v[pawnF] + v[pawnG] + v[pawnH];

        pawnScore += v[pawnA+(file^F)] - pawnCommon/8;

        pawnCommon = v[pawn2] + v[pawn3] + v[pawn4] + v[pawn5] + v[pawn6] + v[pawn7];

        pawnScore += v[pawn2+(first^F)-1] - pawnCommon/6;

        /*
         *  Doubled pawn
         */

        if (last < first)
                pawnScore += v[doubledPawnA+(file^F)];

        /*
         *  Backward pawn
         *
         *  "A backward pawn is a pawn no longer defensible by own pawns and
         *   whose stop square lacks pawn protection but is controlled by a
         *   sentry."
         *
         *  TODO: (wiki)
         *  "If two opposing pawns on adjacent files in knight distance are
         *   mutually backward, the more advanced is not considered backward."
         *
         *  TODO: (wiki)
         *  "A backward pawn is worse, or even a real Straggler, if on a
         *   half-open file as suitable target of opponent rooks."
         *
         *  TODO: (Stockfish)
         *  "if it can capture an enemy pawn it cannot be backward either"
         */

        if ((minRank[-1][side] ^ R) > first
         && (minRank[+1][side] ^ R) > first
         && first < 5
         && ((minRank[-1][xside] ^ R) == first + 2
          || (maxRank[-1][xside] ^ R) == first + 2
          || (minRank[+1][xside] ^ R) == first + 2
          || (maxRank[+1][xside] ^ R) == first + 2))
                pawnScore += v[backwardPawnA+(file^F)];

#if 0
        /*
         *  Isolated
         */

        /*
         *  Half connected
         */

        /*
         *  Full connected
         */
#endif

        /*
         *  Passer
         */

        if ((maxRank[-1][xside] ^ R) <= first
         && (maxRank[ 0][xside] ^ R) <= first
         && (maxRank[+1][xside] ^ R) <= first)                  // Passer
                pawnScore += v[passerA_0+(file^F)]
                           + v[passerA_1+(file^F)] * (first - 1)
                           + v[passerA_2+(file^F)] * (first - 1) * (first - 2) / 4;

        return pawnScore;
}

/*----------------------------------------------------------------------+
 |      sigmoid and logit                                               |
 +----------------------------------------------------------------------*/

static
double sigmoid(double x)
{
        return 1.0 / (1.0 + exp(-x));
}

static
double logit(double p)
{
        return log(p / (1.0 - p));
}

/*----------------------------------------------------------------------+
 |      squareOf                                                        |
 +----------------------------------------------------------------------*/

static int squareOf(Board_t self, int piece)
{
        for (int square=0; square<boardSize; square++)
                if (self->squares[square] == piece)
                        return square;
        assert(false);
}

/*----------------------------------------------------------------------+
 |                                                                      |
 +----------------------------------------------------------------------*/

