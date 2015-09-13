
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

#define isKingSide(file) (((file) ^ fileD) >> 2)

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

/*----------------------------------------------------------------------+
 |      evaluate                                                        |
 +----------------------------------------------------------------------*/

int evaluate(Board_t self, const int v[vectorLen], struct evaluation *Cc)
{
        struct evaluation e;
        memset(&e, 0, sizeof e);

        if (!v) v = globalVector;

        //updateSideInfo(self);

        /*
         *  Scan board for pieces
         */
        for (int square=0; square<boardSize; square++) {

                int piece = self->squares[square];
                if (piece == empty) continue;

                int side = pieceColor(piece);
                int xside = other(side);

                int file = file(square);
                int rank = rank(square);

                int flank = isKingSide(file);
                int squareColor = (file ^ rank) & 1; // abstract "even" or "odd", don't know/care if it is light or dark

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

                        setMax(e.maxPawnRank[file][side], rank);
                        setMax(e.maxPawnRankX[file][side], flip(rank));

                        setMax(e.maxPawnFile[side], file);
                        setMax(e.maxPawnFileX[side], flip(file));
                        break;
                }
        }

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

        /*
         *  Material imbalances
         */

        for (int side=white; side<=black; side++) {
                int xside = other(side);

                e.material[side] =
                        e.nrQueens[side] * (
                               + v[queenValue]
                               + v[queenAndQueen]   * (e.nrQueens[side] - 1)
                               + v[queenAndRook]    * e.nrRooks[side]
                               + v[queenAndBishop]  * e.nrBishops[side]
                               + v[queenAndKnight]  * e.nrKnights[side]
                               + v[queenAndPawn]    * e.nrPawns[side]
                               + ((v[queenAndPawn2] * e.nrPawns[side] * e.nrPawns[side]) >> 3)
                               + v[queenVsRook]     * e.nrRooks[side]
                               + v[queenVsBishop]   * e.nrBishops[side]
                               + v[queenVsKnight]   * e.nrKnights[side]
                               + v[queenVsPawn]     * e.nrPawns[xside]
                               + ((v[queenVsPawn2]  * e.nrPawns[xside] * e.nrPawns[xside]) >> 3)
                        )
                        + e.nrRooks[side] * (
                               + v[rookValue]
                               + v[rookAndRook]     * (e.nrRooks[side] - 1)
                               + v[rookAndBishop]   * e.nrBishops[side]
                               + v[rookAndKnight]   * e.nrKnights[side]
                               + v[rookAndPawn]     * e.nrPawns[side]
                               + ((v[rookAndPawn2]  * e.nrPawns[side] * e.nrPawns[side]) >> 3)
                               + v[rookVsBishop]    * e.nrBishops[xside]
                               + v[rookVsKnight]    * e.nrKnights[xside]
                               + v[rookVsPawn]      * e.nrPawns[xside]
                               + ((v[rookVsPawn2]   * e.nrPawns[xside] * e.nrPawns[xside]) >> 3)
                        )
                        + e.nrBishops[side] * (
                               + v[bishopValue]
                               + v[bishopAndBishop] * (e.nrBishops[side] - 1)
                               + v[bishopAndKnight] * e.nrKnights[side]
                               + v[bishopAndPawn]   * e.nrPawns[side]   
                               + ((v[bishopAndPawn2]* e.nrPawns[side] * e.nrPawns[side]) >> 3)
                               + v[bishopVsKnight]  * e.nrKnights[side]
                               + v[bishopVsPawn]    * e.nrPawns[xside]
                               + ((v[bishopVsPawn2] * e.nrPawns[xside] * e.nrPawns[xside]) >> 3)
                        )
                        + e.nrKnights[side] * (
                               + v[knightValue]
                               + v[knightAndKnight] * (e.nrKnights[side] - 1)
                               + v[knightAndPawn]   * e.nrPawns[side]
                               + ((v[knightAndPawn2]* e.nrPawns[side] * e.nrPawns[side]) >> 3)
                               + v[knightVsPawn]    * e.nrPawns[xside]
                               + ((v[knightVsPawn2] * e.nrPawns[xside] * e.nrPawns[xside]) >> 3)
                        );

                for (int i=0; i<e.nrPawns[side]; i++)
                        e.material[side] += v[pawnValue1+i];
        }

        /*
         *  Pawn evaluation
         */

        for (int file=fileA; file!=fileH+fileStep; file+=fileStep)
                for (int side=white; side<=black; side++) {
                        
                        //e.isOpen[side][file] = (minRank[side][file] + maxRank[side][file] == 0);
                }


        /*
         *  Draw rate prediction ("draw")
         */

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
                drawScore += v[drawUnlikeBishops];
                if (nrQueens)  drawScore += v[drawUnlikeBishopsAndQueens];
                if (nrRooks)   drawScore += v[drawUnlikeBishopsAndRooks];
                if (nrKnights) drawScore += v[drawUnlikeBishopsAndKnights];
        }

        // Each side has a piece the other doesn't have (don't count pawns)
        int deltaQueens = e.nrQueens[white] - e.nrQueens[black];
        int deltaRooks = e.nrRooks[white] - e.nrRooks[black];
        int deltaMinors = e.nrMinors[white] - e.nrMinors[black];
        if (min(min(deltaQueens, deltaRooks), deltaMinors) < 0
         && max(max(deltaQueens, deltaRooks), deltaMinors) > 0) {
                if (deltaQueens) drawScore += v[drawQueenImbalance];
                if (deltaRooks)  drawScore += v[drawRookImbalance];
                if (deltaMinors) drawScore += v[drawMinorImbalance];
        }

        /*
         *  Sum everything
         */

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
                + e.knights[side] )

        int wiloScore = partial(side) - partial(xside);

        wiloScore += v[sideToMoveBonus];

        /*
         *  Contempt
         */
        if (side == white)
                wiloScore += self->eloDiff * v[eloDiff];
        else
                wiloScore -= self->eloDiff * v[eloDiff];

        /*
         *  Special endgames
         */

        // Ignore multiple same-side bishops on the same square color
        #define nrEffectiveBishops(side) (                     \
                (e.nrBishops[side] - e.nrBishopsX[side] > 0) + \
                (e.nrBishopsX[side] > 0))

        int nrEffectiveBishops = nrEffectiveBishops(white) + nrEffectiveBishops(black);
        int nrEffectivePieces = nrKings + nrQueens + nrRooks + nrEffectiveBishops + nrKnights + nrPawns;

        if (nrEffectivePieces == 2)
                wiloScore = 0;

        if (nrEffectivePieces == 3) {
                int deltaMajors = e.nrQueens[side] - e.nrQueens[xside]
                                + e.nrRooks[side] - e.nrRooks[xside];
                wiloScore += deltaMajors * v[winBonus];

                if (nrBishops > 0)
                        wiloScore = 0;

                if (e.nrKnights[xside] > 0)
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


        e.wiloScore = wiloScore;
        e.drawScore = drawScore;

        // Combine wiloScore and drawScore into an expected game result
        double Wp = sigmoid(wiloScore * 1e-3);
        double D = sigmoid(drawScore * 1e-3);
        e.P = 0.5 * D + Wp - D * Wp;

        static const double Ci = 4.0 / M_LN10;
        e.score = round(Ci * logit(e.P) * 1e+3);

        // Keep copy of the calculation if so requested
        if (Cc != null)
                *Cc = e;

        return e.score;
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

