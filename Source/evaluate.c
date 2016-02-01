
/*----------------------------------------------------------------------+
 |                                                                      |
 |      evaluate.c                                                      |
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
 |      Includes                                                        |
 +----------------------------------------------------------------------*/

// C standard
#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

// C extension
#include "cplus.h"

// Own interface
#include "Board.h"
#include "Engine.h"

// Other modules
#include "kpk.h"

/*----------------------------------------------------------------------+
 |      Definitions                                                     |
 +----------------------------------------------------------------------*/

#ifndef M_LN10
#define M_LN10 2.30258509299404568401799145468436421 // log(10)
#endif

#define flip(fileOrRank) ((fileOrRank) ^ 7)
#define kingDistance(a, b) max(abs(file(a) - file(b)), abs(rank(a) - rank(b)))
#define fileIndex(board, file_, side) ((file_) ^ fileA ^ ((file((board)->sides[side].king) ^ fileA) >> 2 ? 0 : 7))
#define oppKings(board) ((file((board)->sides[white].king) ^ file((board)->sides[black].king)) >> 2)

enum vector {
        #define P(id, value) id
        #include "vector.h"
        #undef P
};

/* struct material {
        uint64_t materialKey;
        int wiloScore, drawScore;
        double passerScaling[2];
        double safetyScaling[2];
}; */

struct pkSlot {
        uint64_t pawnKingHash;
        short wiloScore[2];
        short drawScore;
        short passerScore[2];
        short shelter[2];
        short bishopWilo[2][2]; // [bishopColor][squareColor]
        // TODO: something to boost drawness when bishops don't interact with enemy pawns
        unsigned char pawnOnFile[2]; // open, half-open
        unsigned char passerOnFile[2];
        unsigned char weakPawnOnFile[2];
        unsigned char span[2]; // 0..4
        //unsigned char center[2]; // 0..4
};

#define pawnOnFile(side, file) bitTest(pawns->pawnOnFile[side], file)

#define nrPawns(side)      (int)((self->materialKey >> (((side) << 2) + 0)) & 15)
#define nrKnights(side)    (int)((self->materialKey >> (((side) << 2) + 8)) & 15)
#define nrBishops(side)    (int)((self->materialKey >> (((side) << 2) + 16)) & 15)
#define nrRooks(side)      (int)((self->materialKey >> (((side) << 2) + 24)) & 15)
#define nrQueens(side)     (int)((self->materialKey >> (((side) << 2) + 32)) & 15)
#define nrBishopsD(side)   (int)((self->materialKey >> (((side) << 2) + 40)) & 15)
#define nrBishopsL(side)   (nrBishops(side) - nrBishopsD(side))
#define materialHash(side) (((materialKey) >> 48) & 0xffff)

#define nrMinors(side)     (nrKnights(side) + nrBishops(side))
#define nrMajors(side)     (nrRooks(side)   + nrQueens(side))

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
uint64_t globalVectorBaseHash; // TODO: move to Python engine object

static const int firstRank[] = { rank1, rank8 }; // white, black
static const int pawnStep[] = { a3 - a2, a6 - a7 };

/*
 *  Center of the 3x3 square attack zone around the king
 *  (so shifted for borders and corners: e1->e2, h1->g2, f2->f2, etc.)
 */
#define Z(sq) [sq] = square(file(sq)==fileA ? fileB : file(sq)==fileH ? fileG : file(sq),\
                            rank(sq)==rank1 ? rank2 : rank(sq)==rank8 ? rank7 : rank(sq))
static const int kingZoneCenter[] = {
        Z(a1), Z(a2), Z(a3), Z(a4), Z(a5), Z(a6), Z(a7), Z(a8),
        Z(b1), Z(b2), Z(b3), Z(b4), Z(b5), Z(b6), Z(b7), Z(b8),
        Z(c1), Z(c2), Z(c3), Z(c4), Z(c5), Z(c6), Z(c7), Z(c8),
        Z(d1), Z(d2), Z(d3), Z(d4), Z(d5), Z(d6), Z(d7), Z(d8),
        Z(e1), Z(e2), Z(e3), Z(e4), Z(e5), Z(e6), Z(e7), Z(e8),
        Z(f1), Z(f2), Z(f3), Z(f4), Z(f5), Z(f6), Z(f7), Z(f8),
        Z(g1), Z(g2), Z(g3), Z(g4), Z(g5), Z(g6), Z(g7), Z(g8),
        Z(h1), Z(h2), Z(h3), Z(h4), Z(h5), Z(h6), Z(h7), Z(h8),
};

#define pawnKingLen (1L << 17) // must be power of 2
static struct pkSlot pawnKingTable[pawnKingLen];

/*----------------------------------------------------------------------+
 |      Functions                                                       |
 +----------------------------------------------------------------------*/

static void extractPawnStructure(Board_t self, const int v[vectorLen], struct pkSlot *pawns);
static void evaluatePawnFile(Board_t self, const int v[vectorLen], struct pkSlot *pawns, int file, int side,
                             int maxPawnFromFirst[2][10][2]);
static int evaluatePasser(Board_t self, const int v[vectorLen], int fileFlag, int side);
static int evaluateKnight(Board_t self, const int v[vectorLen], const struct pkSlot *pawns, int square, int side);
static int evaluateBishop(Board_t self, const int v[vectorLen], const struct pkSlot *pawns, int square, int side);
static int evaluateRook  (Board_t self, const int v[vectorLen], const struct pkSlot *pawns, int square, int side, double safetyScaling[2]);
static int evaluateQueen (Board_t self, const int v[vectorLen],                             int square, int side);
static int evaluateKing  (              const int v[vectorLen],                             int square, int side);

static int shelterPenalty(const int v[vectorLen], int side, int file, int maxPawnFromFirst[2][10][2]);

static double sigmoid(double x);
static double logit(double p);
static int squareOf(Board_t self, int piece);

/*----------------------------------------------------------------------+
 |      evaluate                                                        |
 +----------------------------------------------------------------------*/

// TODO:
// rooks behind passers (w+b)
// rooks before passers (w+b)
// occupancy of front square
// occupancy of second square
// occupancy of any  square
// control of front square
// control of second square
// control of any square
// supported passer
// connected passers

int evaluate(Board_t self)
{
        const int *v = globalVector;

        int wiloScore[2]; // Accumulators
        double passerScaling[2];
        double safetyScaling[2];

        /*--------------------------------------------------------------+
         |      Feature extraction                                      |
         +--------------------------------------------------------------*/

        // Update attack tables and king locations
        updateSideInfo(self);

        /*--------------------------------------------------------------+
         |      Material balance                                        |
         +--------------------------------------------------------------*/

        // Useful piece counters
        int nrQueens  = nrQueens(white)  + nrQueens(black);
        int nrRooks   = nrRooks(white)   + nrRooks(black);
        int nrBishops = nrBishops(white) + nrBishops(black);
        int nrKnights = nrKnights(white) + nrKnights(black);
        int nrPawns   = nrPawns(white)   + nrPawns(black);

        for (int side=white; side<=black; side++) {
                int xside = other(side);

                wiloScore[side] =
                        nrQueens(side) * (
                                + v[queenValue]
                                + v[queenAndQueen]  * (nrQueens(side) - 1)
                                + v[queenAndPawn_1] * nrPawns(side)
                                + v[queenAndPawn_2] * nrPawns(side) * nrPawns(side) / 8
                                + v[queenVsPawn_1]  * nrPawns(xside)
                                + v[queenVsPawn_2]  * nrPawns(xside) * nrPawns(xside) / 8)

                        + v[queenAndRook]   * min(nrQueens(side), nrRooks(side))
                        + v[queenAndBishop] * min(nrQueens(side), nrBishops(side))
                        + v[queenAndKnight] * min(nrQueens(side), nrKnights(side))
                        + v[queenVsRook]    * min(nrQueens(side), nrRooks(xside))
                        + v[queenVsBishop]  * min(nrQueens(side), nrBishops(xside))
                        + v[queenVsKnight]  * min(nrQueens(side), nrKnights(xside))

                        + nrRooks(side) * (
                                + v[rookValue]
                                + v[rookAndRook]   * (nrRooks(side) - 1)
                                + v[rookAndPawn_1] * nrPawns(side)
                                + v[rookAndPawn_2] * nrPawns(side) * nrPawns(side) / 8
                                + v[rookVsPawn_1]  * nrPawns(xside)
                                + v[rookVsPawn_2]  * nrPawns(xside) * nrPawns(xside) / 8)

                        + v[rookAndBishop] * min(nrRooks(side), nrBishops(side))
                        + v[rookAndKnight] * min(nrRooks(side), nrKnights(side))
                        + v[rookVsBishop]  * min(nrRooks(side), nrBishops(xside))
                        + v[rookVsKnight]  * min(nrRooks(side), nrKnights(xside))

                        + nrBishops(side) * (
                                + v[bishopValue]
                                + v[bishopAndBishop] * (nrBishops(side) - 1)
                                + v[bishopAndPawn_1] * nrPawns(side)
                                + v[bishopAndPawn_2] * nrPawns(side) * nrPawns(side) / 8
                                + v[bishopVsPawn_1]  * nrPawns(xside)
                                + v[bishopVsPawn_2]  * nrPawns(xside) * nrPawns(xside) / 8)

                        + v[bishopAndKnight] * min(nrBishops(side), nrKnights(side))
                        + v[bishopVsKnight]  * min(nrBishops(side), nrKnights(xside))

                        + nrKnights(side) * (
                                + v[knightValue]
                                + v[knightAndKnight] * (nrKnights(side) - 1)
                                + v[knightAndPawn_1] * nrPawns(side)
                                + v[knightAndPawn_2] * nrPawns(side) * nrPawns(side) / 8
                                + v[knightVsPawn_1]  * nrPawns(xside)
                                + v[knightVsPawn_2]  * nrPawns(xside) * nrPawns(xside) / 8);

                if (nrPawns(side) > 0)
                        wiloScore[side] += v[pawnValue1 + nrPawns(side) - 1];

                passerScaling[side] = sigmoid(1e-3 * (v[passerScalingOffset]
                        + nrQueens(side)   * v[passerAndQueen]
                        + nrRooks(side)    * v[passerAndRook]
                        + nrBishops(side)  * v[passerAndBishop]
                        + nrKnights(side)  * v[passerAndKnight]
                        + nrPawns(side)    * v[passerAndPawn]
                        + nrQueens(xside)  * v[passerVsQueen]
                        + nrRooks(xside)   * v[passerVsRook]
                        + nrBishops(xside) * v[passerVsBishop]
                        + nrKnights(xside) * v[passerVsKnight]
                        + nrPawns(xside)   * v[passerVsPawn]));

                safetyScaling[side] = sigmoid(1e-3 * (v[safetyScalingOffset]
                        + nrQueens(side)   * v[safetyAndQueen]
                        + nrRooks(side)    * v[safetyAndRook]
                        + nrBishops(side)  * v[safetyAndBishop]
                        + nrKnights(side)  * v[safetyAndKnight]
                        + nrPawns(side)    * v[safetyAndPawn]
                        + nrQueens(xside)  * v[safetyVsQueen]
                        + nrRooks(xside)   * v[safetyVsRook]
                        + nrBishops(xside) * v[safetyVsBishop]
                        + nrKnights(xside) * v[safetyVsKnight]
                        + nrPawns(xside)   * v[safetyVsPawn]));
        }

        /*--------------------------------------------------------------+
         |      Pawn structure                                          |
         +--------------------------------------------------------------*/

        long pkIndex = (self->pawnKingHash ^ globalVectorBaseHash) & (pawnKingLen - 1);
        struct pkSlot *pawns = &pawnKingTable[pkIndex];
        if (pawns->pawnKingHash != self->pawnKingHash)
                extractPawnStructure(self, v, pawns);

        for (int side=white; side<=black; side++) {
                wiloScore[side] += pawns->wiloScore[side];

                wiloScore[side] += round(pawns->passerScore[side] * passerScaling[side]);
                for (int files=pawns->passerOnFile[side]; files; files&=files-1)
                        wiloScore[side] += evaluatePasser(self, v, files & -files, side);
        }

        /*--------------------------------------------------------------+
         |      Piece placement                                         |
         +--------------------------------------------------------------*/

        for (int side=white; side<=black; side++)
                wiloScore[side] += evaluateKing(v, self->sides[side].king, side);

        // Scan board for pieces
        for (int square=0; square<boardSize; square++) {
                int piece = self->squares[square];
                int side;
                switch (piece) {
                case whiteQueen: case blackQueen:
                        side = pieceColor(piece);
                        wiloScore[side] += evaluateQueen(self, v, square, side);
                        break;
                case whiteRook: case blackRook:
                        side = pieceColor(piece);
                        wiloScore[side] += evaluateRook(self, v, pawns, square, side, safetyScaling);
                        break;
                case whiteBishop: case blackBishop:
                        side = pieceColor(piece);
                        wiloScore[side] += evaluateBishop(self, v, pawns, square, side);
                        break;
                case whiteKnight: case blackKnight:
                        side = pieceColor(piece);
                        wiloScore[side] += evaluateKnight(self, v, pawns, square, side);
                        break;
                }
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
                int wAttack = self->sides[white].attacks[square];
                int bAttack = self->sides[black].attacks[square];

                int controlValue;
                if (isCenter(square))
                        controlValue = v[controlCenter];
                else if (isExtendedCenter(square))
                        controlValue = v[controlExtendedCenter];
                else
                        controlValue = v[controlOutside];

                if (wAttack > bAttack)
                        wiloScore[white] += controlValue;

                if (wAttack < bAttack)
                        wiloScore[black] += controlValue;
        }

        /*--------------------------------------------------------------+
         |      King safety                                             |
         +--------------------------------------------------------------*/

        for (int side=white; side<=black; side++) {
                int xside = other(side);
                int king = self->sides[side].king;
                int target = kingZoneCenter[king];

                // Number of attacked king zone squares
                unsigned char *attacks = &self->sides[xside].attacks[target];
                int nrAttackedSquares = (attacks[-9] > 0) + (attacks[-8] > 0) + (attacks[-7] > 0)
                                      + (attacks[-1] > 0) + (attacks[ 0] > 0) + (attacks[ 1] > 0)
                                      + (attacks[ 7] > 0) + (attacks[ 8] > 0) + (attacks[ 9] > 0);
                nrAttackedSquares = min(nrAttackedSquares, 6); // clip 7..9 at 6

                // A measure of distinct pieces attacking the king zone
                int attackers = attacks[-9] | attacks[-8] | attacks[-7]
                              | attacks[-1] | attacks[ 0] | attacks[ 1]
                              | attacks[ 7] | attacks[ 8] | attacks[ 9];
                int nrAttackKings  = attackers & 1;
                int nrAttackQueens = (attackers >> 1) & 1;
                int nrAttackRooks  = (attackers >> 2) & 3;
                int nrAttackMinors = (attackers >> 4) & 3;
                int nrAttackPawns  = attackers >> 6;
                // TODO: non-linearity on number of attackers

                // Safe squares for the king (0 or 1 is not good)
                // TODO: should this be a part of safety scaling at all?
                int kingMobility = 0;
                int dirs = kingDirections[king];
                int dir = 0;
                do {
                        dir = (dir - dirs) & dirs; // pick next
                        int to = king + kingStep[dir];
                        if ((self->squares[to] == empty || pieceColor(self->squares[to]) != side)
                         && (self->sides[xside].attacks[to] == 0)
                         && (++kingMobility >= 2))
                                break;
                } while (dirs -= dir); // remove and go to next

                // Lump it together in an attack score
                int attack = 0;
                if (nrAttackedSquares > 0) attack -= v[attackSquares_0 + nrAttackedSquares-1];
                if (nrAttackedSquares < 6) attack += v[attackSquares_0 + nrAttackedSquares];
                if (nrAttackKings  > 0) attack += v[attackByKing];
                if (nrAttackQueens > 0) attack += v[attackByQueen];
                if (nrAttackRooks  > 0) attack -= v[attackByRook_0  + nrAttackRooks-1];
                if (nrAttackRooks  < 3) attack += v[attackByRook_0  + nrAttackRooks];
                if (nrAttackMinors > 0) attack -= v[attackByMinor_0 + nrAttackMinors-1];
                if (nrAttackMinors < 3) attack += v[attackByMinor_0 + nrAttackMinors];
                if (nrAttackPawns  > 0) attack -= v[attackByPawn_0  + nrAttackPawns-1];
                if (nrAttackPawns  < 3) attack += v[attackByPawn_0  + nrAttackPawns];
                if (kingMobility < 2) attack += v[mobilityKing_0  + kingMobility];

                // King safety is the (negative) scaled shelter and attack score
                // TODO: maybe it is better to scale shelter and attack independently:
                // - Small attacks should scale the same as shelter
                // - Heavy attacks are always dangerous
                int shelter = pawns->shelter[side];
                if (rank(king) != firstRank[side])
                        shelter += v[shelterWalkingKing];
                wiloScore[xside] += trunc(safetyScaling[side] * (shelter + attack));
        }

        /*--------------------------------------------------------------+
         |      Draw rate (drawScore)                                   |
         +--------------------------------------------------------------*/

        int drawScore = v[drawOffset]
                + nrQueens  * v[drawQueen]
                + nrRooks   * v[drawRook]
                + nrBishops * v[drawBishop]
                + nrKnights * v[drawKnight]
                + nrPawns   * v[drawPawn]
                + pawns->drawScore;

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

        // Unlike bishops
        int wUnlikeL = nrBishopsL(white) > 0;
        int wUnlikeD = nrBishopsD(white) > 0;
        int bUnlikeL = nrBishopsL(black) > 0;
        int bUnlikeD = nrBishopsD(black) > 0;
        if (abs(wUnlikeL + bUnlikeD - wUnlikeD - bUnlikeL) == 2) {
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
        int dQ = nrQueens(white) - nrQueens(black);
        int dR = nrRooks(white)  - nrRooks(black);
        int dM = nrMinors(white) - nrMinors(black);
        if (min(min(dQ, dR), dM) < 0
         && max(max(dQ, dR), dM) > 0) {
                if (dQ) drawScore += v[drawQueenImbalance];
                if (dR) drawScore += v[drawRookImbalance];
                if (dM) drawScore += v[drawMinorImbalance];
        }

        for (int side=white; side<=black; side++)
                if ((self->materialKey & materialMaskPiecesAndPawns[side]) == 0) { // bare king
                        wiloScore[other(side)] += v[winBonus];
                        drawScore -= v[winBonus];
                }

        /*--------------------------------------------------------------+
         |      Subtotal                                                |
         +--------------------------------------------------------------*/

        int side = sideToMove(self);
        int xside = other(side);

        wiloScore[side] += v[tempo]; // side to move bonus
        wiloScore[white] += self->eloDiff * v[eloDiff] / 10; // contempt

        int wiloSum = wiloScore[side] - wiloScore[xside];

        /*--------------------------------------------------------------+
         |      Special endgames                                        |
         +--------------------------------------------------------------*/

        // Ignore multiple same-side bishops on the same square color
        #define nrEffectiveBishops(side) (\
                (nrBishopsL(side) > 0) + (nrBishopsD(side) > 0))

        int nrEffectiveBishops = nrEffectiveBishops(white) + nrEffectiveBishops(black);
        int nrEffectivePieces = 2 + nrQueens + nrRooks + nrEffectiveBishops + nrKnights + nrPawns;

        if (nrEffectivePieces == 2)
                return 0; // Insufficient mating material (KK)

        if (nrEffectivePieces == 3) {
                if (nrQueens(side) + nrRooks(side) > 0) {
                        wiloSum += v[winBonus]; // KQK, KRK
                        drawScore -= v[winBonus];
                }

                if (nrQueens(xside) + nrRooks(xside) > 0) {
                        wiloSum -= v[winBonus]; // KKQ, KKR
                        drawScore -= v[winBonus];
                }

                if (nrBishops + nrKnights > 0)
                        return 0; // Insufficient mating material (KNK, KB...K of like-bishops)

                if (nrPawns > 0) { // KPK
                        int egtSide, wKing, wPawn, bKing;

                        if (nrPawns(white) == 1) {
                                egtSide = side;
                                wKing = self->sides[white].king;
                                wPawn = squareOf(self, whitePawn);
                                bKing = self->sides[black].king;
                        } else {
                                egtSide = xside;
                                wKing = square(0, 7) ^ self->sides[black].king;
                                wPawn = square(0, 7) ^ squareOf(self, blackPawn);
                                bKing = square(0, 7) ^ self->sides[white].king;
                        }

                        int egtScore = kpkProbe(egtSide, wKing, wPawn, bKing);
                        if (egtScore == 0)
                                return 0;
                        else {
                                wiloSum += egtScore * v[winBonus];
                                drawScore -= v[winBonus];
                        }
                }
        }
#if 0
        if (nrEffectivePieces == 4) {
                if (nrKnights(xside) == 2)
                        return 0; // KKNN, draw if lone king is to move

                if (nrEffectiveBishops(side) == 1 && nrKnights(xside) == 1)
                        return 0; // KBKN, draw if bishop side is to move
        }
#endif

        /*--------------------------------------------------------------+
         |      Total                                                   |
         +--------------------------------------------------------------*/

        // Combine wiloScore and drawScore into an expected game result
        double Wp = sigmoid(wiloSum * 1e-3);
        double D = sigmoid(drawScore * 1e-3);
        double P = 0.5 * D + Wp - D * Wp;

        //printf("wiloSum %+d drawScore %+d P %f\n", wiloSum, drawScore, P);

        static const double Ci = 4.0 / M_LN10;
        int score = round(Ci * logit(P) * 1e+3);

        if (score == 0) score++; // Reserve 0 exclusively for draws
        score = min(score,  maxEval);
        score = max(score, -maxEval);

        /*--------------------------------------------------------------+
         |      Return                                                  |
         +--------------------------------------------------------------*/

        return score;
}

/*----------------------------------------------------------------------+
 |      extractPawnStructure                                            |
 +----------------------------------------------------------------------*/

static void extractPawnStructure(Board_t self, const int v[vectorLen], struct pkSlot *pawns)
{
        *pawns = (struct pkSlot) { .pawnKingHash = self->pawnKingHash };

        /*
         *  Maximum distance of pawn to first rank, for each file and both sides.
         *  Used to detect open files, doubled pawns, passers, etc.
         */
        int maxPawnFromFirst[2][10][2] = {{{0}}}; // [perspective][1+file][pawnColor]

        /*
         *  Outmost files with pawns, to extract pawn span and center.
         *  We don't calculate 'min' directly for easier initialization.
         */
        int maxPawnFromFileA[2] = {0};
        int maxPawnFromFileH[2] = {0};

        int bySquareColor[2][2] = {{0}}; // [pawnColor][squareColor]
        int rammedBySquareColor[2][2] = {{0}};

        // Pawn location scan
        for (int square=0; square<boardSize; square++) {
                int piece = self->squares[square];
                if (piece == whitePawn || piece == blackPawn) {
                        int file = file(square), rank = rank(square);
                        int pawnColor = pieceColor(piece);
                        #define setMax(a, b) if ((a) < (b)) (a) = (b)

                        setMax(maxPawnFromFirst[white][1+file][pawnColor], rank ^ rank1);
                        setMax(maxPawnFromFirst[black][1+file][pawnColor], rank ^ rank8);

                        setMax(maxPawnFromFileA[pawnColor], file ^ fileA);
                        setMax(maxPawnFromFileH[pawnColor], file ^ fileH);

                        int squareColor = squareColor(square);
                        bySquareColor[pawnColor][squareColor] += 1;

                        int inFront = self->squares[square+pawnStep[pawnColor]];
                        if (inFront == whitePawn || inFront == blackPawn) {
                                rammedBySquareColor[pawnColor][squareColor] += 1;

                                int fileType = min(file, flip(file));
                                pawns->drawScore += v[drawRammed_0 + fileType];
                        }
                }
        }

        // Bishop and pawn square color scoring
        for (int side=white; side<=black; side++) {
                int xside = other(side);
                for (int squareColor=white; squareColor<=black; squareColor++)
                        pawns->bishopWilo[side][squareColor] =
                                v[bishopAndLikePawn_1]     * bySquareColor[side][squareColor]
                              + v[bishopAndLikePawn_2]     * bySquareColor[side][squareColor] * bySquareColor[side][squareColor]
                              + v[bishopAndLikeRammedPawn] * rammedBySquareColor[side][squareColor]
                              + v[bishopVsLikePawn_1]      * bySquareColor[xside][squareColor]
                              + v[bishopVsLikePawn_2]      * bySquareColor[xside][squareColor] * bySquareColor[xside][squareColor]
                              + v[bishopVsLikeRammedPawn]  * rammedBySquareColor[xside][squareColor];
        }

        for (int side=white; side<=black; side++) {
                //pawns->center[side] = (maxPawnFromFileA[side] + (7 - maxPawnFromFileH[side])) / 3;
                //assert(0 <= pawns->center[side] && pawns->center[side] <= 4);

                // Center [0..14]/3 -> [0..4] and span [0, 2..9]/2 -> [0..4]
                // +---+---+---+---+---+---+---+---+
                // | 0 0 0 1 1 1 2 2 2 3 3 3 4 4 4 | = center = (minPawnFile + maxPawnFile) / 3 : [0..4]
                // +---+---+---+---+---+---+---+---+
                //   a   b   c   d   e   f   g   h
                pawns->span[side] = max(0, maxPawnFromFileA[side] + maxPawnFromFileH[side] - 5) / 2;
        }

        // Pawn features
        for (int file=0; file<8; file++) {
                evaluatePawnFile(self, v, pawns, file, white, maxPawnFromFirst);
                evaluatePawnFile(self, v, pawns, file, black, maxPawnFromFirst);
        }

        // Pawn shelter for king safety
        for (int side=white; side<=black; side++) {
                int king = self->sides[side].king;
                int target = kingZoneCenter[king];
                int kFlag = self->castleFlags & (castleFlagWhiteKside << side);
                int qFlag = self->castleFlags & (castleFlagWhiteQside << side);
                int shelter = shelterPenalty(v, side, file(target), maxPawnFromFirst);
                int kShelter = kFlag ? shelterPenalty(v, side, fileG, maxPawnFromFirst) : shelter;
                int qShelter = qFlag ? shelterPenalty(v, side, fileB, maxPawnFromFirst) : shelter;
                int best = min(shelter, min(kShelter, qShelter));
                shelter -= (v[shelterCastled] * (shelter - best)) >> 8; // (1-w)*S + w*B == S - w*(S-B)
                pawns->shelter[side] = shelter;
                if (kFlag | qFlag) // Intrinsic value of castling capability
                        pawns->wiloScore[side] += !qFlag ? v[castleK] :
                                                  !kFlag ? v[castleQ] : v[castleKQ];
        }
}

/*----------------------------------------------------------------------+
 |      evaluatePawnFile                                                |
 +----------------------------------------------------------------------*/

// Helper for extractPawnStructure
static void evaluatePawnFile(Board_t self, const int v[vectorLen], struct pkSlot *pawns, int file, int side,
                             int maxPawnFromFirst[2][10][2])
{
        #define maxRank(i, j)     ( maxPawnFromFirst[side ][1+file+(i)][j] )
        #define minRank(i, j) flip( maxPawnFromFirst[xside][1+file+(i)][j] )

        int xside = other(side);
        int frontPawn = maxRank(0, side);
        int backPawn  = minRank(0, side);

        if (backPawn > frontPawn)
                return; // No pawn on this file

        pawns->pawnOnFile[side] |= bit(file);

        // Feature extraction
        int fileIndex = fileIndex(self, file, side);
        int firstHelper = min(minRank(-1, side),  minRank(+1, side));
        int lastSentry  = max(maxRank(-1, xside), maxRank(+1, xside));
        int firstSentry = min(minRank(-1, xside), minRank(+1, xside));
        int pawnZone = ones(8) & ~bit(7) & ~bit(0);
        int helperW  = ((1 << minRank(-1, side))  | (1 << maxRank(-1, side)))  & pawnZone;
        int helperE  = ((1 << minRank(+1, side))  | (1 << maxRank(+1, side)))  & pawnZone;
        int sentryW  = ((1 << minRank(-1, xside)) | (1 << maxRank(-1, xside))) & pawnZone;
        int sentryE  = ((1 << minRank(+1, xside)) | (1 << maxRank(+1, xside))) & pawnZone;
        int xPawns   = ((1 << minRank( 0, xside)) | (1 << maxRank( 0, xside))) & pawnZone;
        int frontSpan = (~0 << (frontPawn + 1)) & ones(8);
        int helperControl = (helperW | helperE) << 1;
        int sentryControl = (sentryW | sentryE) >> 1;
        bool openFile = !(xPawns & frontSpan);
        bool isRammed = bitTest(xPawns,        frontPawn + 1);
        bool isDuo    = bitTest(helperControl, frontPawn + 1);
        bool stopSquareAttacked = bitTest(sentryControl, frontPawn + 1);
        bool isDefended = bitTest(helperControl, frontPawn);
        bool canCapture = bitTest(sentryControl, frontPawn); // == isAttacked
        bool isTrailing = firstHelper > frontPawn;
        bool isDoubled = backPawn < frontPawn;
        int neighbours = (maxRank(-1, side) > 0) + (maxRank(+1, side) > 0);
        int firstXpawn = minRank(0, xside);

        // Scoring
        int pawnScore = 0;

        // First order
        if (openFile)           pawnScore += v[openFilePawn_0 + frontPawn - 1];
        if (isRammed)           pawnScore += v[rammedPawn_0   + frontPawn - 1];
        if (canCapture)         pawnScore += v[capturePawn_0  + frontPawn - 1];
        if (isDefended)         pawnScore += v[defendedPawn_0 + frontPawn - 1];
        if (stopSquareAttacked) pawnScore += v[stoppedPawn_0  + frontPawn - 1];
        if (isDuo)              pawnScore += v[duoPawn_0      + frontPawn - 1];
        if (isTrailing)         pawnScore += v[trailingPawn_0 + frontPawn - 1];
        if (isDoubled)          pawnScore += v[doubledPawnA   + fileIndex];
        if (isDuo)              pawnScore += v[duoPawnA       + fileIndex];

        // File and rank dependent scoring
        int offset = oppKings(self) ? pawnByFile_0x : pawnByFile_0;
        if (fileIndex > 0) pawnScore -= v[offset + fileIndex - 1];
        if (fileIndex < 7) pawnScore += v[offset + fileIndex];
        if (frontPawn > 1) pawnScore -= v[pawnByRank_0 + frontPawn - 2];
        if (frontPawn < 6) pawnScore += v[pawnByRank_0 + frontPawn - 1];

        // Pawn mobility
        if (!isRammed && !stopSquareAttacked)
                pawnScore += v[mobilePawn_0 + frontPawn - 1];

        // Backward
        if (isTrailing && stopSquareAttacked && !canCapture) {
                pawnScore += v[(openFile ? backwardPawnOpenA : backwardPawnClosedA) + fileIndex];
                if (openFile) {
                        if (frontPawn > 2) pawnScore -= v[backwardPawnOpenByRank_0 + frontPawn - 2];
                        if (frontPawn < 6) pawnScore += v[backwardPawnOpenByRank_0 + frontPawn - 1];
                        pawns->weakPawnOnFile[side] |= bit(file);
                } else {
                        if (frontPawn > 2) pawnScore -= v[backwardPawnClosedByRank_0 + frontPawn - 2];
                        if (frontPawn < 6) pawnScore += v[backwardPawnClosedByRank_0 + frontPawn - 1];
                }
        }

        // Rammed and weak
        if (isTrailing && isRammed && !canCapture)
                pawnScore += v[rammedWeakPawnA + fileIndex];

        // Isolated, middle or end of group
        static const int offsets[3][2] = {
                { isolatedPawnClosedA, isolatedPawnOpenA },
                { sidePawnClosedA,     sidePawnOpenA     },
                { middlePawnClosedA,   middlePawnOpenA   } };
        pawnScore += v[offsets[neighbours][openFile] + fileIndex];

        // Passer
        if (openFile && lastSentry <= frontPawn) {
                int nominal = v[passerA_0 + fileIndex]
                            + v[passerA_1 + fileIndex] * (frontPawn - 1)
                            + v[passerA_2 + fileIndex] * (frontPawn - 1) * (frontPawn - 2) / 4;
                pawns->passerScore[side] += nominal;
                pawns->passerOnFile[side] |= bit(file);
        }
        // TODO: scoring by square (no polynomials)
        // TODO: passer is doubled penalty

        // Candidate
        if (openFile && lastSentry > frontPawn) {
                int oneSentry = (sentryW ^ sentryE) >> 1;
                int twoSentry = (sentryW & sentryE) >> 1;
                int anyHelper = (helperW | helperE) << 1;
                int twoHelper = (helperW & helperE) << 1;
                int stoppedByOne = frontSpan & oneSentry & ~anyHelper;
                int stoppedByTwo = frontSpan & twoSentry & ~twoHelper;
                // TODO: scoring by square (no polynomials)
                // TODO: scale by number of sentries to overcome (1,2,3-4)

                if (!stoppedByOne && !stoppedByTwo) {
                        assert(frontPawn < 6);
                        int nominal = v[candidateByRank_0 + frontPawn - 1]
                                    + v[candidateA + fileIndex];
                        pawns->passerScore[side] += nominal;
                }
        }

#if 0
        // Sneaker
        if (isRammed && lastSentry <= frontPawn) {
                // - - -   - - -
                // - * -   - * -
                // - O -   x O -
                // - - -   x - -
                // - - O   O - -
                // - - -   x - -
                // - - -   o - -
                // - - -   x - -
                int blockers = frontSpan & xPawns;
                int blocker = blockers & -blockers;
                if (blocker == blockers) { // only one blocker
                        blocker -= 1; // ranks where we want to find a helper

                        if (helperW & blocker)
                        if (helperE & blocker)
                }
        }
#endif

        // Can make a lever
        // TODO: use bitwise logic
        if (frontPawn + 1 < firstSentry && firstSentry <= firstXpawn && firstXpawn < 7)
                pawnScore += v[pawnLever_0 + firstSentry - frontPawn];

#if 0
        // Potential open file (good for rooks)
        if (openFile && lastSentry > frontPawn && !isDoubled) {
                #define firstBit(w) ((w) & -(w))
                int rank = firstBit(sentryControl & frontSpan);
                assert(rank != 0);
                if ((((sentryW ^ sentryE) >> 1) | helperControl) & rank)
                        pawnFlags |= pawnFlagPotentialOpen;
        }

        pawns->pawnFlags[side][pawns->nrPawns[side]++] = pawnFlags;
        assert(pawns->nrPawns[side] < 8);
#endif

        pawns->wiloScore[side] += pawnScore;
}

/*----------------------------------------------------------------------+
 |      evaluatePasser                                                  |
 +----------------------------------------------------------------------*/

static int evaluatePasser(Board_t self, const int v[vectorLen], int fileFlag, int side)
{
        int file = bitIndex8(fileFlag);
        assert(1 << file == fileFlag);

        // Scan backwards from promotion square
        int xside = other(side);
        int freeSquares = 6; // Free until proven otherwise
        int square = square(file, firstRank[xside]);
        for (;;) {
                int piece = self->squares[square];
                if (piece == whitePawn || piece == blackPawn)
                        break;
                if (piece == empty)
                        freeSquares++;
                else
                        freeSquares = 0;
                square -= pawnStep[side];
                assert(0 <= square && square < boardSize);
                if (square & ~(boardSize - 1))
                        return 0; // Safety net for pawn hash collisions
        }

        assert(0 <= square && square < boardSize);
        assert(file(square) == file);
        assert(pieceColor(self->squares[square]) == side);

        int passerScore = 0;

        if (freeSquares < 6) { // There is a blocker
                if (freeSquares > 0) passerScore -= v[blocker_0 + freeSquares - 1];
                if (freeSquares < 5) passerScore += v[blocker_0 + freeSquares];
        } else {
                passerScore += v[freePasser]; // No blocker

                if ((self->materialKey & materialMaskPiecesNoPawns[xside]) == 0) {
                        // NOW's heuristic (http://talkchess.com/forum/viewtopic.php?p=125794)
                        int rankFlip = (side == white) ?  0 : square(0, 7);
                        int king  = rankFlip ^ self->sides[side].king;
                        int xking = rankFlip ^ self->sides[xside].king;
                        int pawn  = rankFlip ^ square;
                        if (kpkProbe(white, king, pawn, xking) > 0
                         && kpkProbe(black, king, pawn, xking) < 0)
                                passerScore += v[unstoppablePasser];
                }
        }

        return passerScore;
}

/*----------------------------------------------------------------------+
 |      evaluateKnight                                                  |
 +----------------------------------------------------------------------*/

static int evaluateKnight(Board_t self, const int v[vectorLen], const struct pkSlot *pawns, int square, int side)
{
        int knightScore = 0;

        int fileIndex = fileIndex(self, file(square), side);
        int rankIndex = rank(square) ^ firstRank[side]; // Relative to own first rank

        // File and rank dependent scoring
        int offset = oppKings(self) ? knightByFile_0x : knightByFile_0;
        if (fileIndex > 0) knightScore -= v[offset + fileIndex - 1];
        if (fileIndex < 7) knightScore += v[offset + fileIndex];
        if (rankIndex > 0) knightScore -= v[knightByRank_0 + rankIndex - 1];
        if (rankIndex < 7) knightScore += v[knightByRank_0 + rankIndex];

        int span = pawns->span[side];
        if (span > 0) knightScore -= v[knightAndSpan_0 + span - 1];
        if (span < 4) knightScore += v[knightAndSpan_0 + span];

        int xspan = pawns->span[other(side)];
        if (xspan > 0) knightScore -= v[knightVsSpan_0 + xspan - 1];
        if (xspan < 4) knightScore += v[knightVsSpan_0 + xspan];

        // Distance to kings
        knightScore += v[knightToOwnKing] * kingDistance(square, self->sides[side].king)
                     + v[knightToKing]    * kingDistance(square, self->sides[other(side)].king);

        // TODO: strong squares / outposts

        return knightScore;
}

/*----------------------------------------------------------------------+
 |      evaluateBishop                                                  |
 +----------------------------------------------------------------------*/

static int evaluateBishop(Board_t self, const int v[vectorLen], const struct pkSlot *pawns, int square, int side)
{
        int bishopScore = 0;

        int fileIndex = fileIndex(self, file(square), side);
        int rankIndex = rank(square) ^ firstRank[side]; // Relative to own first rank

        int offset = oppKings(self) ? bishopBySquare_0x : bishopBySquare_0;
        bishopScore += v[offset + square(fileIndex, rankIndex)];

        int span = pawns->span[side];
        if (span > 0) bishopScore -= v[bishopAndSpan_0 + span - 1];
        if (span < 4) bishopScore += v[bishopAndSpan_0 + span];

        int xspan = pawns->span[other(side)];
        if (xspan > 0) bishopScore -= v[bishopVsSpan_0 + xspan - 1];
        if (xspan < 4) bishopScore += v[bishopVsSpan_0 + xspan];

        int squareColor = squareColor(square);
        bishopScore += pawns->bishopWilo[side][squareColor]; // TODO: fold this into pkSlot

        // Distance to kings
        bishopScore += v[bishopToOwnKing] * kingDistance(square, self->sides[side].king)
                     + v[bishopToKing]    * kingDistance(square, self->sides[other(side)].king);

        return bishopScore;
}

/*----------------------------------------------------------------------+
 |      evaluateRook                                                    |
 +----------------------------------------------------------------------*/

static int evaluateRook(Board_t self, const int v[vectorLen], const struct pkSlot *pawns, int square, int side, double safetyScaling[2])
{
        int rookScore = 0;

        int file = file(square);
        int fileType = min(file, flip(file)); // Relative to nearest rim
        int rankIndex = rank(square) ^ firstRank[side]; // Relative to own first rank

        // Location dependent scoring
        if (fileType > 0) rookScore -= v[rookByFile_0 + fileType - 1];
        if (fileType < 3) rookScore += v[rookByFile_0 + fileType];
        if (rankIndex > 0) rookScore -= v[rookByRank_0 + rankIndex - 1];
        if (rankIndex < 7) rookScore += v[rookByRank_0 + rankIndex];

        // Distance to kings
        rookScore += v[rookToOwnKing] * kingDistance(square, self->sides[side].king)
                   + v[rookToKing]    * kingDistance(square, self->sides[other(side)].king);

        // Rook on open or half-open file
        if (!pawnOnFile(side, file)) {
                if (pawnOnFile(other(side), file)) {
                        if (bitTest(pawns->weakPawnOnFile[other(side)], file))
                                rookScore += v[rookToWeakPawn_0 + fileType];
                        else
                                rookScore += v[rookOnHalfOpen_0 + fileType];
                } else
                        rookScore += v[rookOnOpen_0 + fileType];

                int xside = other(side);
                int deltaFile = abs(file(square) - file(self->sides[xside].king));
                rookScore += trunc(safetyScaling[xside] * v[rookToKing_0 + deltaFile]);
        }

        return rookScore;
}

// TODO: relation to opponent king zone (normalized to reduce pressure on hash table)
// TODO: connecting (side by side), connecting on 7th
// TODO: controlling squares on an open file (not just by rook)
// TODO: supporting/blocking passers
// TODO: attacking backward pawns

/*----------------------------------------------------------------------+
 |      evaluateQueen                                                   |
 +----------------------------------------------------------------------*/

static int evaluateQueen(Board_t self, const int v[vectorLen], int square, int side)
{
        int queenScore = 0;

        int file = file(square);
        int fileType = min(file, flip(file));
        int rankIndex = rank(square) ^ firstRank[side]; // Relative to own first rank

        // File and rank dependent scoring
        if (fileType > 0) queenScore -= v[queenByFile_0 + fileType - 1];
        if (fileType < 3) queenScore += v[queenByFile_0 + fileType];
        if (rankIndex > 0) queenScore -= v[queenByRank_0 + rankIndex - 1];
        if (rankIndex < 7) queenScore += v[queenByRank_0 + rankIndex];

        // Distance to kings
        queenScore += v[queenToOwnKing] * kingDistance(square, self->sides[side].king)
                    + v[queenToKing]    * kingDistance(square, self->sides[other(side)].king);

        return queenScore;
}
// TODO: attacking backward pawns

/*----------------------------------------------------------------------+
 |      evaluateKing                                                    |
 +----------------------------------------------------------------------*/

static int shelterPenalty(const int v[vectorLen], int side, int file, int maxPawnFromFirst[2][10][2])
{
        int sum = 0;

        for (int i=-1; i<=1; i++) {
                int j = maxPawnFromFirst[other(side)][1+file+i][side];
                if (j < 6) sum += v[shelterPawn_5-j];
        }

        int fileType = min(file, flip(file));
        if (fileType > 0) sum -= v[shelterKing_0 + fileType - 1];
        if (fileType < 3) sum += v[shelterKing_0 + fileType];

        return sum;
}

static int evaluateKing(const int v[vectorLen], int square, int side)
{
        int kingScore = 0;

        int file = file(square);
        int fileType = min(file, flip(file));
        int rankIndex = rank(square) ^ firstRank[side];

        // File and rank dependent scoring
        if (fileType > 0) kingScore -= v[kingByFile_0 + fileType - 1];
        if (fileType < 3) kingScore += v[kingByFile_0 + fileType];
        if (rankIndex > 0) kingScore -= v[kingByRank_0 + rankIndex - 1];
        if (rankIndex < 7) kingScore += v[kingByRank_0 + rankIndex];

        return kingScore;
}

/*----------------------------------------------------------------------+
 |      sigmoid and logit                                               |
 +----------------------------------------------------------------------*/

static double sigmoid(double x)
{
        return 1.0 / (1.0 + exp(-x));
}

static double logit(double p)
{
        return log(p / (1.0 - p));
}

/*----------------------------------------------------------------------+
 |      squareOf                                                        |
 +----------------------------------------------------------------------*/

static int squareOf(Board_t self, int piece)
{
        int square;
        for (square=0; square<boardSize; square++)
                if (self->squares[square] == piece)
                        break;
        assert(square < boardSize);
        return square;
}

/*----------------------------------------------------------------------+
 |                                                                      |
 +----------------------------------------------------------------------*/

