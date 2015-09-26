
/*----------------------------------------------------------------------+
 |                                                                      |
 |      kpk.c -- pretty fast KPK endgame table generator                |
 |                                                                      |
 +----------------------------------------------------------------------*/

/*
 *  Copyright (C) 2015, Marcel van Kervinck
 *  All rights reserved
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *  1. Redistributions of source code must retain the above copyright
 *  notice, this list of conditions and the following disclaimer.
 *
 *  2. Redistributions in binary form must reproduce the above copyright
 *  notice, this list of conditions and the following disclaimer in the
 *  documentation and/or other materials provided with the distribution.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/*----------------------------------------------------------------------+
 |      Includes                                                        |
 +----------------------------------------------------------------------*/

// C standard
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

// Own interface
#include "Board.h"
#include "kpk.h"

/*----------------------------------------------------------------------+
 |      Definitions                                                     |
 +----------------------------------------------------------------------*/

enum { N = a2-a1, S = -N, E = b1-a1, W = -E }; // Derived geometry

#define inPawnZone(square) (rank(square)!=rank1 && rank(square)!=rank8)
#define conflict(wKing,wPawn,bKing) (wKing==wPawn || wPawn==bKing || wKing==bKing)

#define dist(a,b) (abs(file(a)-file(b)) | abs(rank(a)-rank(b))) // logical-or as max()
#define wInCheck(wKing,wPawn,bKing) (dist(wKing,bKing)==1)
#define bInCheck(wKing,wPawn,bKing) (dist(wKing,bKing)==1                     \
                                  || (file(wPawn)!=fileA && wPawn+N+W==bKing) \
                                  || (file(wPawn)!=fileH && wPawn+N+E==bKing))

// Square set macros (no need to adapt these to the specific geometry)
#define bit(i) (1ULL << (i))
#define mask 0x0101010101010101ULL
#define allW(set) ((set) >> 8)
#define allE(set) ((set) << 8)
#define allS(set) (((set) & ~mask) >> 1)
#define allN(set) (((set) << 1) & ~mask)
#define allKing(set) (allW(allN(set)) | allN(set) | allE(allN(set)) \
                    | allW(set)                   | allE(set)       \
                    | allW(allS(set)) | allS(set) | allE(allS(set)))

#define arrayLen(a) (sizeof(a) / sizeof((a)[0]))
//enum { white, black };
#define wKingSquare(ix) ((ix)>>5)
#define wPawnSquare(ix) square(((ix)>>3)&3, (ix)&7)

/*----------------------------------------------------------------------+
 |      Data                                                            |
 +----------------------------------------------------------------------*/

static uint64_t kpkTable[2][64*32];
static const int kingSteps[] = { N+W, N, N+E, W, E, S+W, S, S+E };

/*----------------------------------------------------------------------+
 |      Functions                                                       |
 +----------------------------------------------------------------------*/

int kpkProbe(int side, int wKing, int wPawn, int bKing)
{
        if (!kpkTable[0][1]) kpkGenerate();

        if (file(wPawn) >= 4) {
                wKing = square(7 - file(wKing), rank(wKing));
                wPawn = square(7 - file(wPawn), rank(wPawn));
                bKing = square(7 - file(bKing), rank(bKing));
        }
        int ix = (wKing << 5) + (file(wPawn) << 3) + rank(wPawn);
        int bit = (kpkTable[side][ix] >> bKing) & 1;
        return (side == white) ? bit : -bit;
}

int kpkGenerate(void)
{
        uint64_t valid[ arrayLen(kpkTable[0]) ];

        for (int ix=0; ix<arrayLen(kpkTable[0]); ix++) {
                int wKing = wKingSquare(ix), wPawn = wPawnSquare(ix);

                // Positions after winning pawn promotion (we can ignore stalemate here)
                if (rank(wPawn) == rank8 && wKing != wPawn) {
                        uint64_t lost = ~allKing(bit(wKing)) & ~bit(wKing) & ~bit(wPawn);
                        if (dist(wKing, wPawn) > 1)
                                lost &= ~allKing(bit(wPawn));
                        kpkTable[black][ix] = lost;
                }

                // Valid positions after black move, pawn capture allowed
                valid[ix] = ~allKing(bit(wKing));
                if (file(wPawn) != fileA) valid[ix] &= ~bit(wPawn+N+W);
                if (file(wPawn) != fileH) valid[ix] &= ~bit(wPawn+N+E);
        }

        int changed;
        do {
                for (int ix=0; ix<arrayLen(kpkTable[0]); ix++) {
                        int wKing = wKingSquare(ix), wPawn = wPawnSquare(ix);
                        if (!inPawnZone(wPawn))
                                continue;

                        // White king moves
                        uint64_t won = 0;
                        for (int i=0; i<arrayLen(kingSteps); i++) {
                                int to = wKing + kingSteps[i];
                                int jx = ix + (kingSteps[i] << 5);
                                if (dist(wKing, to & 63) == 1 && to != wPawn)
                                        won |= kpkTable[black][jx] & ~allKing(bit(to));
                        }
                        // White pawn moves
                        if (wPawn+N != wKing) {
                                won |= kpkTable[black][ix+rank2-rank1] & ~bit(wPawn+N);
                                if (rank(wPawn) == rank2 && wPawn+N+N != wKing)
                                        won |= kpkTable[black][ix+rank4-rank2]
                                            & ~bit(wPawn+N) & ~bit(wPawn+N+N);
                        }
                        kpkTable[white][ix] = won & ~bit(wPawn);
                }

                changed = 0;
                for (int ix=0; ix<arrayLen(kpkTable[0]); ix++) {
                        if (!inPawnZone(wPawnSquare(ix)))
                                continue;

                        // Black king moves
                        uint64_t isBad = kpkTable[white][ix] | ~valid[ix];
                        uint64_t canDraw = allKing(~isBad);
                        uint64_t hasMoves = allKing(valid[ix]);
                        uint64_t lost = hasMoves & ~canDraw;

                        changed += (kpkTable[black][ix] != lost);
                        kpkTable[black][ix] = lost;
                }
        } while (changed);

        return sizeof kpkTable;
}

int kpkSelfCheck(void)
{
        int counts[] = {                // As given by Steven J. Edwards (1996):
                163328 / 2, 168024 / 2, // - Legal positions per side
                124960 / 2, 97604  / 2  // - Non-draw positions per side
        };
        for (int ix=0; ix<arrayLen(kpkTable[0]); ix++) {
                int wKing = wKingSquare(ix), wPawn = wPawnSquare(ix);
                for (int bKing=0; bKing<boardSize; bKing++) {
                        if (!inPawnZone(wPawn) || conflict(wKing, wPawn, bKing))
                                continue;
                        counts[0] -= !bInCheck(wKing, wPawn, bKing);
                        counts[1] -= !wInCheck(wKing, wPawn, bKing);
                        counts[2] -= !bInCheck(wKing, wPawn, bKing) && ((kpkTable[white][ix] >> bKing) & 1);
                        counts[3] -= !wInCheck(wKing, wPawn, bKing) && ((kpkTable[black][ix] >> bKing) & 1);
                }
        }
        return !counts[0] && !counts[1] && !counts[2] && !counts[3];
}

/*----------------------------------------------------------------------+
 |                                                                      |
 +----------------------------------------------------------------------*/

