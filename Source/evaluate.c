
/*----------------------------------------------------------------------+
 |                                                                      |
 |      evaluate.c                                                      |
 |                                                                      |
 +----------------------------------------------------------------------*/

/*
 *  Copyright (C) 2015, Marcel van Kervinck
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
#include <string.h>

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

#define isOnKingSide(square) ((file(square) ^ fileA) >> 2)

enum vector {
        #define P(id, value) id
        #include "vector.h"
        #undef P
};

struct evaluation {
        /*
         *  Maximum distance of pawn to rank1, for each file and both sides.
         *  And then the same from rank8 point of view.
         *  Used to detect open files, doubled pawns, passers, etc.
         */
        int maxPawnFromRank1[10][2];
        int maxPawnFromRank8[10][2];

        /*
         *  Outmost files with pawns, to extract pawn span and center.
         *  We don't calculate 'min' directly for easier initialization.
         */
        int maxPawnFromFileA[2];
        int maxPawnFromFileH[2];

        /*
         *  Accumulators
         */
        int material[2];
        int safety[2];
        int passers[2];  // passers, not scaled
        int control[2];  // control of each square
        int mobility[2]; // use extended attack maps
        int kings[2];    // PST, distance to weak pawns, distance to passers (w+b)
        int queens[2];
        int rooks[2];    // PST, doubled, strong squares
        int bishops[2];  // PST, trapped, strong squares, can engage enemy pawns, not blocked by own pawns, pawn span is good
        int knights[2];  // PST, strong squares, pawn span is bad
        int pawns[2];    // PST, doubled, grouped, mobile
        int others[2];

        // TODO: move to material table
        double passerScaling[2];
        double safetyScaling[2];
};

#define nrPawns(side)      (int)(((materialKey) >> (((side) << 2) + 0)) & 15)
#define nrKnights(side)    (int)(((materialKey) >> (((side) << 2) + 8)) & 15)
#define nrBishops(side)    (int)(((materialKey) >> (((side) << 2) + 16)) & 15)
#define nrRooks(side)      (int)(((materialKey) >> (((side) << 2) + 24)) & 15)
#define nrQueens(side)     (int)(((materialKey) >> (((side) << 2) + 32)) & 15)
#define nrBishopsD(side)   (int)(((materialKey) >> (((side) << 2) + 40)) & 15)
#define nrBishopsL(side)   (nrBishops(side) - nrBishopsD(side))
#define materialHash(side) (((materialKey) >> 48) & 0xffff)
#define nrMinors(side)     (nrKnights(side) + nrBishops(side))
#define nrMajors(side)     (nrRooks(side)   + nrQueens(side))

/*----------------------------------------------------------------------+
 |      Data                                                            |
 +----------------------------------------------------------------------*/

static const unsigned long long materialKeys[][2] = {
        [empty]       = { 0, 0 },
        [whitePawn]   = { 0x514e000000000001ull, 0x514e000000000001ull },
        [blackPawn]   = { 0x696d000000000010ull, 0x696d000000000010ull },
        [whiteKnight] = { 0x6ab5000000000100ull, 0x6ab5000000000100ull },
        [blackKnight] = { 0xd903000000001000ull, 0xd903000000001000ull },
        [whiteBishop] = { 0x2081000000010000ull, 0x2081000000010000ull + 0xb589010000000000ull },
        [blackBishop] = { 0x3d15000000100000ull, 0x3d15000000100000ull + 0x67f5100000000000ull },
        [whiteRook]   = { 0xae45000001000000ull, 0xae45000001000000ull },
        [blackRook]   = { 0x7de9000010000000ull, 0x7de9000010000000ull },
        [whiteQueen]  = { 0x9ac3000100000000ull, 0x9ac3000100000000ull },
        [blackQueen]  = { 0xa96f001000000000ull, 0xa96f001000000000ull },
        [whiteKing]   = { 0, 0 },
        [blackKing]   = { 0, 0 },
};

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

static const int firstRank[] = { rank1, rank8 };

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

/*----------------------------------------------------------------------+
 |      Functions                                                       |
 +----------------------------------------------------------------------*/

static double sigmoid(double x);
static double logit(double p);

static int squareOf(Board_t self, int piece);

static int evaluatePawnFile(const int v[vectorLen],
                            int fileIndex, int side, bool oppKings,
                            const int minRank[][2],
                            const int maxRank[][2],
                            const double passerScaling[2]);
static int evaluateKnight(const int v[vectorLen], int fileIndex, int rankIndex, bool oppKings);
static int evaluateBishop(const int v[vectorLen], int fileIndex, int rankIndex, bool oppKings);
static int evaluateRook  (const int v[vectorLen], int fileIndex, int rankIndex, bool oppKings);
static int evaluateQueen (const int v[vectorLen], int fileIndex, int rankIndex, bool oppKings);
static int evaluateKing  (const int v[vectorLen], int fileIndex, int rankIndex);

static int shelterPenalty(const int v[vectorLen], int side, int file, const int maxPawnFromLastRank[][2]);

/*----------------------------------------------------------------------+
 |      evaluate                                                        |
 +----------------------------------------------------------------------*/

// TODO:
// pawnBlocks
// rooks behind passers (w+b)
// rooks before passers (w+b)
// distance from enemy king file
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
        struct evaluation e;
        memset(&e, 0, sizeof e);

        const int *v = globalVector;

        /*--------------------------------------------------------------+
         |      Feature extraction                                      |
         +--------------------------------------------------------------*/

        // Update attack tables and king locations
        updateSideInfo(self);

        bool kingSide[2]; // true if respective king on E-H
        kingSide[white] = isOnKingSide(self->sides[white].king);
        kingSide[black] = isOnKingSide(self->sides[black].king);
        bool oppKings = (kingSide[white] != kingSide[black]);

#if 0
        char pawnSpan[2]; // 0 .. 8
        char pawnCenter[2]; // 0 .. 14  target for kings?
#endif

        unsigned long long materialKey = 0; // TODO: calculate incrementally in moves.c

        // Scan board for pieces
        for (int square=0; square<boardSize; square++) {
                int piece = self->squares[square];
                if (piece == empty)
                        continue;

                int side = pieceColor(piece);

                // `file' and `rank' are fileA..H and rank1..8
                int file = file(square);
                int rank = rank(square);

                /*
                 *  `fileIndex' and `rankIndex' are normalized as if the own king
                 *  is White and located on the king side, meaning:
                 *  - If the king is on file E..H, fileIndex 0 means `file A'
                 *  - If the king is on file A..D, fileIndex 0 means `file H'
                 *  - rankIndex 0 means `rank 1' for White and `rank 8' for Black.
                 *  This scheme is independent of the definitions in geometry.h!
                 */
                int fileIndex = file ^ (kingSide[side] ? fileA : fileH);
                int rankIndex = rank ^ (side == white  ? rank1 : rank8);

                // 0 = light (h1) or 1 = dark squares
                int squareColor = (file ^ rank ^ fileH ^ rank1) & 1;

                materialKey += materialKeys[piece][squareColor];

                switch (piece) {
                case whiteKing: case blackKing:
                        e.kings[side] += evaluateKing(v, fileIndex, rankIndex);
                        break;
                case whiteQueen: case blackQueen:
                        e.queens[side] += evaluateQueen(v, fileIndex, rankIndex, oppKings);
                        break;
                case whiteRook: case blackRook:
                        e.rooks[side] += evaluateRook(v, fileIndex, rankIndex, oppKings);
                        break;
                case whiteBishop: case blackBishop:
                        e.bishops[side] += evaluateBishop(v, fileIndex, rankIndex, oppKings);
                        break;
                case whiteKnight: case blackKnight:
                        // TODO: separate feature extraction from evaluation
                        // (for example, pawn structure is not established here yet)
                        e.knights[side] += evaluateKnight(v, fileIndex, rankIndex, oppKings);
                        break;
                case whitePawn: case blackPawn: {
                        int absFileIndex = file ^ fileA;
                        #define setMax(a, b) if ((a) < (b)) (a) = (b);
                        setMax(e.maxPawnFromRank1[1+ absFileIndex][side], rank ^ rank1);
                        setMax(e.maxPawnFromRank8[1+ absFileIndex][side], rank ^ rank8);
                        setMax(e.maxPawnFromFileA[side], file ^ fileA);
                        setMax(e.maxPawnFromFileH[side], file ^ fileH);
                        break;
                        }
                }
        }

        // Useful piece counters
        int nrQueens  = nrQueens(white)  + nrQueens(black);
        int nrRooks   = nrRooks(white)   + nrRooks(black);
        int nrBishops = nrBishops(white) + nrBishops(black);
        int nrKnights = nrKnights(white) + nrKnights(black);
        int nrPawns   = nrPawns(white)   + nrPawns(black);

        /*--------------------------------------------------------------+
         |      Material                                                |
         +--------------------------------------------------------------*/

        for (int side=white; side<=black; side++) {
                int xside = other(side);

                e.material[side] =
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
                        e.material[side] += v[pawnValue1 + nrPawns(side) - 1];

                e.passerScaling[side] = sigmoid(1e-3 * (v[passerScalingOffset]
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

                e.safetyScaling[side] = sigmoid(1e-3 * (v[safetyScalingOffset]
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
                        e.control[white] += controlValue;

                if (wAttack < bAttack)
                        e.control[black] += controlValue;
        }

        /*--------------------------------------------------------------+
         |      Pawns                                                   |
         +--------------------------------------------------------------*/

        for (int absFileIndex=0; absFileIndex<8; absFileIndex++) {

                e.pawns[white] += evaluatePawnFile(v,
                        absFileIndex ^ (kingSide[white] ? 0 : 7), white, oppKings,
                        // C won't allow an implicit const cast of the 2D array here,
                        // see also http://stackoverflow.com/questions/28062095/
                        (const int(*)[2]) &e.maxPawnFromRank1[1+absFileIndex],
                        (const int(*)[2]) &e.maxPawnFromRank8[1+absFileIndex],
                        e.passerScaling);

                e.pawns[black] += evaluatePawnFile(v,
                        absFileIndex ^ (kingSide[black] ? 0 : 7), black, oppKings,
                        (const int(*)[2]) &e.maxPawnFromRank8[1+absFileIndex],
                        (const int(*)[2]) &e.maxPawnFromRank1[1+absFileIndex],
                        e.passerScaling);
        }

        /*--------------------------------------------------------------+
         |      King safety                                             |
         +--------------------------------------------------------------*/

        for (int side=white; side<=black; side++) {
                int xside = other(side);
                int king = self->sides[side].king;
                int target = kingZoneCenter[king];
                int canCastleK = self->castleFlags & (castleFlagWhiteKside << side);
                int canCastleQ = self->castleFlags & (castleFlagWhiteQside << side);

                // King shelter penalty
                const int (*maxPawnFromLastRank)[2] = (const int(*)[2])
                        ((side == white) ? e.maxPawnFromRank8 : e.maxPawnFromRank1);
                int shelter = shelterPenalty(v, side, file(target), maxPawnFromLastRank);
                int castled = shelter; // king can always stay where it is
                if (canCastleK) {
                        int kingSide = shelterPenalty(v, side, fileG, maxPawnFromLastRank);
                        castled = min(castled, kingSide); // maybe the king-side shelter is better
                }
                if (canCastleQ) {
                        int queenSide = shelterPenalty(v, side, fileC, maxPawnFromLastRank);
                        castled = min(castled, queenSide); // or the queen-side shelter
                }
                // (1.0 - w)*A + w*B == A - w*(A - B)
                shelter -= (v[shelterCastled] * (shelter - castled)) >> 8;
                if (rank(king) != firstRank[side])
                        shelter += v[shelterWalkingKing];

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
                e.safety[side] = -trunc(e.safetyScaling[side] * (shelter + attack));

                // Castling capability intrinsic value
                if (canCastleK | canCastleQ)
                        e.kings[side] += !canCastleQ ? v[castleK] :
                                         !canCastleK ? v[castleQ] : v[castleKQ];
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

        // Unlike bishops
        int wUnlikeL = nrBishopsL(white) > 0;
        int wUnlikeD = nrBishopsD(white) > 0;
        int bUnlikeL = nrBishopsL(black) > 0;
        int bUnlikeD = nrBishopsD(black) > 0;
        if (wUnlikeL != wUnlikeD && bUnlikeL != bUnlikeD
         && wUnlikeL != bUnlikeL && wUnlikeD != bUnlikeD) {
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
                + e.pawns[side] \
                + e.others[side])

        e.others[side] += v[tempo]; // side to move bonus
        e.others[white] += self->eloDiff * v[eloDiff] / 10; // contempt

        int wiloScore = partial(side) - partial(xside);

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
                        wiloScore += v[winBonus]; // KQK, KRK
                        drawScore -= v[winBonus];
                }

                if (nrQueens(xside) + nrRooks(xside) > 0) {
                        wiloScore -= v[winBonus]; // KKQ, KKR
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
                                wiloScore += egtScore * v[winBonus];
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
        double Wp = sigmoid(wiloScore * 1e-3);
        double D = sigmoid(drawScore * 1e-3);
        double P = 0.5 * D + Wp - D * Wp;

        static const double Ci = 4.0 / M_LN10;
        int score = round(Ci * logit(P) * 1e+3);

        if (score == 0) score++; // Reserve 0 exclusively for draws
        score = min(score,  maxEval);
        score = max(score, -maxEval);

        //printf("%s wilo %d Wp %.3f draw %d D %.1f P %.3f score %d\n",
                //__func__, wiloScore, Wp, drawScore, D, P, score);

        /*--------------------------------------------------------------+
         |      Return                                                  |
         +--------------------------------------------------------------*/

        return score;
}

/*----------------------------------------------------------------------+
 |      evaluatePawn                                                    |
 +----------------------------------------------------------------------*/

static int evaluatePawnFile(const int v[vectorLen],
                            int fileIndex, int side, bool oppKings,
                            const int maxPawnFromRank1[][2],
                            const int maxPawnFromRank8[][2],
                            const double passerScaling[2])
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
         *  After this don't evaluate the back pawn in more detail
         */
        if (backPawn < frontPawn)
                pawnScore += v[doubledPawnA + fileIndex];

        /*
         *  File and rank dependent scoring (7+5=12 degrees of freedom)
         */

        int offset = oppKings ? pawnByFile_0x : pawnByFile_0;
        if (fileIndex > 0)
                pawnScore -= v[offset + fileIndex - 1];
        if (fileIndex < 7)
                pawnScore += v[offset + fileIndex];

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

        /*
         *  Isolated, middle or end of group
         */
        int neighbours = (maxRank(-1, side) > 0) + (maxRank(+1, side) > 0);
        int openFile = (maxRank(0, xside) <= frontPawn);
        static const int offsets[3][2] = {
                { isolatedPawnClosedA, isolatedPawnOpenA },
                { sidePawnClosedA,     sidePawnOpenA     },
                { middlePawnClosedA,   middlePawnOpenA   } };
        pawnScore += v[offsets[neighbours][openFile] + fileIndex];

        /*
         *  Duo
         */
        if (frontPawn == maxRank(-1, side) || frontPawn == maxRank(+1, side))
                pawnScore += v[duoPawnA + fileIndex];

        /*
         *  Passer
         */
        if (maxRank(-1, xside) <= frontPawn
         && maxRank( 0, xside) <= frontPawn
         && maxRank(+1, xside) <= frontPawn) {
                int nominal = v[passerA_0 + fileIndex]
                            + v[passerA_1 + fileIndex] * (frontPawn - 1)
                            + v[passerA_2 + fileIndex] * (frontPawn - 1) * (frontPawn - 2) / 4;
                pawnScore += round(passerScaling[side] * nominal);
        }

        return pawnScore;
}

/*----------------------------------------------------------------------+
 |      evaluateKnight                                                  |
 +----------------------------------------------------------------------*/

static int evaluateKnight(const int v[vectorLen], int fileIndex, int rankIndex, bool oppKings)
{
        int knightScore = 0;

        /*
         *  File and rank dependent scoring (7+7=14 degrees of freedom)
         */

        int offset = oppKings ? knightByFile_0x : knightByFile_0;
        if (fileIndex > 0)
                knightScore -= v[offset + fileIndex - 1];
        if (fileIndex < 7)
                knightScore += v[offset + fileIndex];
        if (rankIndex > 0)
                knightScore -= v[knightByRank_0 + rankIndex - 1];
        if (rankIndex < 7)
                knightScore += v[knightByRank_0 + rankIndex];

        // TODO: strong squares

        return knightScore;
}

/*----------------------------------------------------------------------+
 |      evaluateBishop                                                  |
 +----------------------------------------------------------------------*/

static int evaluateBishop(const int v[vectorLen], int fileIndex, int rankIndex, bool oppKings)
{
        int offset = oppKings ? bishopBySquare_0x : bishopBySquare_0;
        return v[offset + square(fileIndex, rankIndex)];
}

#if 0
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
#endif

/*----------------------------------------------------------------------+
 |      evaluateRook                                                    |
 +----------------------------------------------------------------------*/

static int evaluateRook(const int v[vectorLen], int fileIndex, int rankIndex, bool oppKings)
{
        int rookScore = 0;

        /*
         *  File and rank dependent scoring (7+7=14 degrees of freedom)
         */

        int offset = oppKings ? rookByFile_0x : rookByFile_0;
        if (fileIndex > 0)
                rookScore -= v[offset + fileIndex - 1];
        if (fileIndex < 7)
                rookScore += v[offset + fileIndex];
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

static int evaluateQueen(const int v[vectorLen], int fileIndex, int rankIndex, bool oppKings)
{
        int queenScore = 0;

        /*
         *  File and rank dependent scoring (7+7=14 degrees of freedom)
         */

        int offset = oppKings ? queenByFile_0x : queenByFile_0;
        if (fileIndex > 0)
                queenScore -= v[offset + fileIndex - 1];
        if (fileIndex < 7)
                queenScore += v[offset + fileIndex];
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

static int shelterPenalty(const int v[vectorLen], int side, int file, const int maxPawnFromLastRank[][2])
{
        int sum = 0;
        int absFileIndex = file ^ fileA;

        for (int i=0; i<3; i++) {
                int j = maxPawnFromLastRank[absFileIndex+i][side];
                if (j < 6) sum += v[shelterPawn_5-j];
        }

        if (absFileIndex > 3) absFileIndex ^= 7;
        if (absFileIndex > 0) sum -= v[shelterKing_0 + absFileIndex - 1];
        if (absFileIndex < 3) sum += v[shelterKing_0 + absFileIndex];

        return sum;
}

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

