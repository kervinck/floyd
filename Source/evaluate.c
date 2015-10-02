
/*----------------------------------------------------------------------+
 |                                                                      |
 |      evaluate.c                                                      |
 |                                                                      |
 +----------------------------------------------------------------------*/

/*----------------------------------------------------------------------+
 |      Includes                                                        |
 +----------------------------------------------------------------------*/

// C standard
#include <assert.h>
#define _XOPEN_SOURCE
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

// C extension
#include "cplus.h"

// Own interface
#include "Board.h"
#include "evaluate.h"

// Other modules
#include "kpk.h"

/*----------------------------------------------------------------------+
 |      Definitions                                                     |
 +----------------------------------------------------------------------*/

#define other(side) ((side) ^ 1)
#define flip(fileOrRank) ((fileOrRank) ^ 7)

#define isKingFlankFile(file) (((file) ^ fileD) >> 2)

enum vector {
        #define P(id, value) id
        #include "vector.h"
        #undef P
};

/*----------------------------------------------------------------------+
 |      Data                                                            |
 +----------------------------------------------------------------------*/

const char * const vectorLabels[] = {
        #define P(id, value) #id
        #include "vector.h"
        #undef P
};

const int vectorLen = arrayLen(vectorLabels);

// TODO: should become default and const
int globalVector[] = {
        #define P(id, value) [id] = (value)
        #include "vector.h"
        #undef P
};

/*----------------------------------------------------------------------+
 |      Functions                                                       |
 +----------------------------------------------------------------------*/

static double sigmoid(double x);

static double logit(double p);

static int squareOf(Board_t self, int piece);

static int evaluatePawn(const int v[vectorLen],
                        const int minRank[][2],
                        const int maxRank[][2],
                        int fileIndex, int side,
                        int king, int xking);

static int evaluateKnight(const int v[vectorLen], int fileIndex, int rankIndex);
static int evaluateBishop(const int v[vectorLen], int fileIndex, int rankIndex);
static int evaluateRook(const int v[vectorLen], int fileIndex, int rankIndex);
static int evaluateQueen(const int v[vectorLen], int fileIndex, int rankIndex);
static int evaluateKing(const int v[vectorLen], int fileIndex, int rankIndex);

static int evaluateCastleFlags(const int v[vectorLen], int kSideFlag, int qSideFlag);

/*----------------------------------------------------------------------+
 |      evaluate                                                        |
 +----------------------------------------------------------------------*/

int evaluate(Board_t self)
{
        struct evaluation e;
        memset(&e, 0, sizeof e);

        const int *v = globalVector;

        /*--------------------------------------------------------------+
         |      Feature extraction                                      |
         +--------------------------------------------------------------*/

        // Update attack tables and king locations
        updateSideInfo(self);

#if 0
        char pawnSpan[2]; // 0 .. 8
        char pawnCenter[2]; // 0 .. 14  target for kings?
#endif

        // Scan board for pieces
        for (int square=0; square<boardSize; square++) {

                int piece = self->squares[square];
                if (piece == empty)
                        continue;

                int side = pieceColor(piece);

                // `file' and `rank' are fileA..H and rank1..8
                int file = file(square);
                int rank = rank(square);

                // `fileIndex' and `rankIndex' are relative to the own king
                int fileIndex, rankIndex;

                // TODO: we should be able to write 'self->side[side].king' here
                int king = (side == white)
                        ? self->whiteSide.king
                        : self->blackSide.king;

                if (isKingFlankFile(file(king)))
                        fileIndex = file ^ fileA;
                else
                        fileIndex = file ^ fileH;

                if (side == white)
                        rankIndex = rank ^ rank1;
                else
                        rankIndex = rank ^ rank8;

                // "even" or "odd" square color
                int squareColor = (file ^ rank) & 1;

                switch (piece) {
                case whiteKing:
                case blackKing:
                        e.nrKings[side]++;
                        e.kings[side] += evaluateKing(v, fileIndex, rankIndex);
                        break;

                case whiteQueen:
                case blackQueen:
                        e.nrQueens[side]++;
                        e.queens[side] += evaluateQueen(v, fileIndex, rankIndex);
                        break;

                case whiteRook:
                case blackRook:
                        e.nrRooks[side]++;
                        e.rooks[side] += evaluateRook(v, fileIndex, rankIndex);
                        break;

                case whiteBishop:
                case blackBishop:
                        e.nrBishops[side]++;
                        e.nrBishopsX[side] += squareColor;
                        e.bishops[side] += evaluateBishop(v, fileIndex, rankIndex);
                        break;

                case whiteKnight:
                case blackKnight:
                        e.nrKnights[side]++;
                        // TODO: separate feature extraction from evaluation
                        // (for example, pawn structure is not established here yet)
                        e.knights[side] += evaluateKnight(v, fileIndex, rankIndex);
                        break;

                case whitePawn:
                case blackPawn:
                        e.nrPawns[side]++;
                        e.nrPawnsX[side] += squareColor;

                        int fileIndex = file ^ fileA;
                        setMax(e.maxPawnFromRank1[1+fileIndex][side], rank ^ rank1);
                        setMax(e.maxPawnFromRank8[1+fileIndex][side], rank ^ rank8);

                        setMin(e.maxPawnFromFileA[side], file ^ fileA);
                        setMax(e.maxPawnFromFileH[side], file ^ fileH);

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
                                + v[queenAndQueen]  * (e.nrQueens[side] - 1)
                                + v[queenAndPawn_1] * e.nrPawns[side]
                                + v[queenAndPawn_2] * e.nrPawns[side] * e.nrPawns[side] / 8
                                + v[queenVsPawn_1]  * e.nrPawns[xside]
                                + v[queenVsPawn_2]  * e.nrPawns[xside] * e.nrPawns[xside] / 8)

                        + v[queenAndRook]   * min(e.nrQueens[side], e.nrRooks[side])
                        + v[queenAndBishop] * min(e.nrQueens[side], e.nrBishops[side])
                        + v[queenAndKnight] * min(e.nrQueens[side], e.nrKnights[side])
                        + v[queenVsRook]    * min(e.nrQueens[side], e.nrRooks[xside])
                        + v[queenVsBishop]  * min(e.nrQueens[side], e.nrBishops[xside])
                        + v[queenVsKnight]  * min(e.nrQueens[side], e.nrKnights[xside])

                        + e.nrRooks[side] * (
                                + v[rookValue]
                                + v[rookAndRook]   * (e.nrRooks[side] - 1)
                                + v[rookAndPawn_1] * e.nrPawns[side]
                                + v[rookAndPawn_2] * e.nrPawns[side] * e.nrPawns[side] / 8
                                + v[rookVsPawn_1]  * e.nrPawns[xside]
                                + v[rookVsPawn_2]  * e.nrPawns[xside] * e.nrPawns[xside] / 8)

                        + v[rookAndBishop] * min(e.nrRooks[side], e.nrBishops[side])
                        + v[rookAndKnight] * min(e.nrRooks[side], e.nrKnights[side])
                        + v[rookVsBishop]  * min(e.nrRooks[side], e.nrBishops[xside])
                        + v[rookVsKnight]  * min(e.nrRooks[side], e.nrKnights[xside])

                        + e.nrBishops[side] * (
                                + v[bishopValue]
                                + v[bishopAndBishop] * (e.nrBishops[side] - 1)
                                + v[bishopAndPawn_1] * e.nrPawns[side]
                                + v[bishopAndPawn_2] * e.nrPawns[side] * e.nrPawns[side] / 8
                                + v[bishopVsPawn_1]  * e.nrPawns[xside]
                                + v[bishopVsPawn_2]  * e.nrPawns[xside] * e.nrPawns[xside] / 8)

                        + v[bishopAndKnight] * min(e.nrBishops[side], e.nrKnights[side])
                        + v[bishopVsKnight]  * min(e.nrBishops[side], e.nrKnights[xside])

                        + e.nrKnights[side] * (
                                + v[knightValue]
                                + v[knightAndKnight] * (e.nrKnights[side] - 1)
                                + v[knightAndPawn_1] * e.nrPawns[side]
                                + v[knightAndPawn_2] * e.nrPawns[side] * e.nrPawns[side] / 8
                                + v[knightVsPawn_1]  * e.nrPawns[xside]
                                + v[knightVsPawn_2]  * e.nrPawns[xside] * e.nrPawns[xside] / 8);

                if (e.nrPawns[side] > 0)
                        e.material[side] += v[pawnValue1 + e.nrPawns[side] - 1];
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

        for (int fileIndex=0; fileIndex<8; fileIndex++) {

                // TODO: flip file based on king location
                // TODO: recognize opposite castled kings

                e.pawns[white] += evaluatePawn(v,
                                              &e.maxPawnFromRank1[1+fileIndex],
                                              &e.maxPawnFromRank8[1+fileIndex],
                                              fileIndex,
                                              white,
                                              self->whiteSide.king,
                                              self->blackSide.king);

                e.pawns[black] += evaluatePawn(v,
                                              &e.maxPawnFromRank8[1+fileIndex],
                                              &e.maxPawnFromRank1[1+fileIndex],
                                              fileIndex,
                                              black,
                                              square(0, 7) ^ self->blackSide.king,
                                              square(0, 7) ^ self->whiteSide.king);
        }

        /*--------------------------------------------------------------+
         |      Kings                                                   |
         +--------------------------------------------------------------*/

        e.kings[white] += evaluateCastleFlags(v,
                self->castleFlags & castleFlagWhiteKside,
                self->castleFlags & castleFlagWhiteQside);

        e.kings[black] += evaluateCastleFlags(v,
                self->castleFlags & castleFlagBlackKside,
                self->castleFlags & castleFlagBlackQside);

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

        // In an imbalance each side has a piece the other doesn't have
        int dQ = e.nrQueens[white] - e.nrQueens[black];
        int dR = e.nrRooks[white]  - e.nrRooks[black];
        int dM = e.nrMinors[white] - e.nrMinors[black];
        if (min(min(dQ, dR), dM) < 0
         && max(max(dQ, dR), dM) > 0) {
                if (dQ) drawScore += v[drawQueenImbalance];
                if (dR) drawScore += v[drawRookImbalance];
                if (dM) drawScore += v[drawMinorImbalance];
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
                wiloScore = 0; // KK

        if (nrEffectivePieces == 3) {
                if (e.nrQueens[side] + e.nrRooks[side] > 0)
                        wiloScore += v[winBonus]; // KQK, KRK

                if (e.nrQueens[xside] + e.nrRooks[xside] > 0)
                        wiloScore -= v[winBonus]; // KKQ, KKR

                if (nrBishops + nrKnights > 0)
                        wiloScore = 0; // KBK, KNK (and KB+K of like-bishops)

                if (nrPawns > 0) { // KPK
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

#if 0
        if (nrEffectivePieces == 4) {
                if (e.nrKnights[xside] == 2)
                        wiloScore = 0; // KKNN, draw if lone king is to move

                if (nrEffectiveBishops(side) == 1 && e.nrKnights[xside] == 1)
                        wiloScore = 0; // KBKN, draw if bishop side is to move
        }
#endif

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

        //printf("%s wilo %d Wp %.3f draw %d D %.1f P %.3f score %d\n",
                //__func__, e.wiloScore, Wp, e.drawScore, D, e.P, e.score);

        /*--------------------------------------------------------------+
         |      Return                                                  |
         +--------------------------------------------------------------*/

        return e.score;
}

/*----------------------------------------------------------------------+
 |      evaluatePawn                                                    |
 +----------------------------------------------------------------------*/

static int evaluatePawn(const int v[vectorLen],
                        const int maxPawnFromRank1[][2],
                        const int maxPawnFromRank8[][2],
                        int fileIndex,
                        int side,
                        int king, int xking)
{
        #define maxRank(i, j)  maxPawnFromRank1[i][j]
        #define minRank(i, j) (maxPawnFromRank8[i][j] ^ 7)

        int frontPawn = maxRank(0, side);
        int backPawn  = minRank(0, side);

        if (backPawn > frontPawn) // No pawn
                return 0;

        int pawnScore = 0;

        /*
         *  Doubled pawn
         *
         *  After this don't evaluate the back pawn in more detail
         */

        if (backPawn < frontPawn)
                pawnScore += v[doubledPawnA + fileIndex];

        /*
         *  File and rank dependent scoring (7+5=12 degrees of freedom)
         */

        if (fileIndex > 0)
                pawnScore -= v[pawnByFile_0 + fileIndex - 1];
        if (fileIndex < 7)
                pawnScore += v[pawnByFile_0 + fileIndex];

        if (frontPawn > 1)
                pawnScore -= v[pawnByRank_0 + frontPawn - 2];
        if (frontPawn < 6)
                pawnScore += v[pawnByRank_0 + frontPawn - 1];

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

        int xside = other(side);

        if (minRank(-1, side) > frontPawn
         && minRank(+1, side) > frontPawn
         && frontPawn < 7 - 2
         && (minRank(-1, xside) == frontPawn + 2
          || maxRank(-1, xside) == frontPawn + 2
          || minRank(+1, xside) == frontPawn + 2
          || maxRank(+1, xside) == frontPawn + 2))
                pawnScore += v[backwardPawnA + fileIndex];

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

        if (maxRank(-1, xside) <= frontPawn
         && maxRank( 0, xside) <= frontPawn
         && maxRank(+1, xside) <= frontPawn)
                pawnScore += v[passerA_0 + fileIndex]
                           + v[passerA_1 + fileIndex] * (frontPawn - 1)
                           + v[passerA_2 + fileIndex] * (frontPawn - 1) * (frontPawn - 2) / 4;

        return pawnScore;
}

/*----------------------------------------------------------------------+
 |      evaluateKnight                                                  |
 +----------------------------------------------------------------------*/

static int evaluateKnight(const int v[vectorLen], int fileIndex, int rankIndex)
{
        int knightScore = 0;

        /*
         *  File and rank dependent scoring (7+7=14 degrees of freedom)
         */

        if (fileIndex > 0)
                knightScore -= v[knightByFile_0 + fileIndex - 1];
        if (fileIndex < 7)
                knightScore += v[knightByFile_0 + fileIndex];
        if (rankIndex > 0)
                knightScore -= v[knightByRank_0 + rankIndex - 1];
        if (rankIndex < 7)
                knightScore += v[knightByRank_0 + rankIndex];

        // TODO: relation to both kings
        // TODO: strong squares

        return knightScore;
}

/*----------------------------------------------------------------------+
 |      evaluateBishop                                                  |
 +----------------------------------------------------------------------*/

static int evaluateBishop(const int v[vectorLen], int fileIndex, int rankIndex)
{
        int bishopScore = 0;

        if (rankIndex - fileIndex == 0)
                bishopScore += v[bishopOnLong_0];
        if (rankIndex + fileIndex == 7)
                bishopScore += v[bishopOnLong_1];

        /*
         *  File and rank dependent scoring (7+7=14 degrees of freedom)
         */

        if (fileIndex > 0)
                bishopScore -= v[bishopByFile_0 + fileIndex - 1];
        if (fileIndex < 7)
                bishopScore += v[bishopByFile_0 + fileIndex];
        if (rankIndex > 0)
                bishopScore -= v[bishopByRank_0 + rankIndex - 1];
        if (rankIndex < 7)
                bishopScore += v[bishopByRank_0 + rankIndex];

        // TODO: relation to both kings
        // TODO: strong squares

        return bishopScore;
}

/*----------------------------------------------------------------------+
 |      evaluateRook                                                    |
 +----------------------------------------------------------------------*/

static int evaluateRook(const int v[vectorLen], int fileIndex, int rankIndex)
{
        int rookScore = 0;

        /*
         *  File and rank dependent scoring (7+7=14 degrees of freedom)
         */

        if (fileIndex > 0)
                rookScore -= v[rookByFile_0 + fileIndex - 1];
        if (fileIndex < 7)
                rookScore += v[rookByFile_0 + fileIndex];
        if (rankIndex > 0)
                rookScore -= v[rookByRank_0 + rankIndex - 1];
        if (rankIndex < 7)
                rookScore += v[rookByRank_0 + rankIndex];

        // TODO: doubling
        // TODO: open files
        // TODO: supporting/blocking passers
        // TODO: attacking backward pawns
        // TODO: relation to both kings
        // TODO: strong squares

        return rookScore;
}

/*----------------------------------------------------------------------+
 |      evaluateQueen                                                   |
 +----------------------------------------------------------------------*/

static int evaluateQueen(const int v[vectorLen], int fileIndex, int rankIndex)
{
        int queenScore = 0;

        /*
         *  File and rank dependent scoring (7+7=14 degrees of freedom)
         */

        if (fileIndex > 0)
                queenScore -= v[queenByFile_0 + fileIndex - 1];
        if (fileIndex < 7)
                queenScore += v[queenByFile_0 + fileIndex];
        if (rankIndex > 0)
                queenScore -= v[queenByRank_0 + rankIndex - 1];
        if (rankIndex < 7)
                queenScore += v[queenByRank_0 + rankIndex];

        // TODO: attacking backward pawns
        // TODO: relation to both kings
        // TODO: strong squares

        return queenScore;
}

/*----------------------------------------------------------------------+
 |      evaluateKing                                                    |
 +----------------------------------------------------------------------*/

static int evaluateKing(const int v[vectorLen], int fileIndex, int rankIndex)
{
        int kingScore = 0;

        /*
         *  File and rank dependent scoring (3+7=10 degrees of freedom)
         */

        assert(fileIndex >= 4);
        if (fileIndex > 4)
                kingScore -= v[kingByFile_0 + fileIndex - 4 - 1];
        if (fileIndex < 7)
                kingScore += v[kingByFile_0 + fileIndex - 4];
        if (rankIndex > 0)
                kingScore -= v[kingByRank_0 + rankIndex - 1];
        if (rankIndex < 7)
                kingScore += v[kingByRank_0 + rankIndex];

        return kingScore;
}

static int evaluateCastleFlags(const int v[vectorLen], int kSideFlag, int qSideFlag)
{
        if (kSideFlag && qSideFlag)
                return v[castleKQ];
        if (kSideFlag)
                return v[castleK];
        if (qSideFlag)
                return v[castleQ];
        return 0;
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

