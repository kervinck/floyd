
/*----------------------------------------------------------------------+
 |                                                                      |
 |      test.c                                                          |
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
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// C extension
#include "cplus.h"

// Own interface
#include "Board.h"
#include "Engine.h"
#include "uci.h"

// Other modules
#include "kpk.h"

/*----------------------------------------------------------------------+
 |      Data                                                            |
 +----------------------------------------------------------------------*/

static const char *positions[] = {
        "r1bqk1nr/pp3ppp/2nb4/1B1p4/8/1N3N2/PPP2PPP/R1BQ1RK1 b kq -",
        "4rrk1/ppqb1ppn/3p2np/2pP4/2P1P3/2PBN1B1/P5PP/1R1Q1RK1 w - -",
        "3rk2r/1pq1bpp1/p1n1p1n1/3pP3/3P2P1/1PPQNNB1/5P2/R3R1K1 b k -",
        "7k/8/1p1Q2pp/1p3b2/3p3P/P3q3/BP4PK/8 b - -",
        "2r1rnk1/4qppp/p3b3/1p4P1/4p2P/2N1PN2/PP1Q1P2/1KR4R w - -",
        "2rk3r/p2n1pp1/bqn1p2p/1p1pP3/3P1N1P/bP1BBN2/P3QPP1/2R1R1K1 w - -",
        "1nr2rk1/p3Qppp/1p2p1n1/8/3P4/2N2N2/PP3PPP/2R2RK1 b - -",
        "3nr1k1/1p1r3p/pR1pNpp1/3P4/4P3/3P3P/6P1/1R5K b - -",
        "4r1k1/pp3ppp/3n4/2n1r3/2P2NP1/4KB2/PP2P2P/R5R1 w - -",
        "8/p5pk/1p6/5R1p/P7/5PPK/5r2/8 b - -",
        "8/2Q2p2/5qpk/4p3/8/5P1P/6K1/8 w - -",
        "5rk1/1pr2bbp/pNp1n1p1/2P1p3/1PBpP3/1N5P/1P3PP1/3RR1K1 b - -",
        "8/1b3pk1/rp5p/6p1/6B1/1R4P1/4P2P/4K3 w - -",
        "r1bqk2r/pp2ppbp/3p1np1/8/2PQP3/2N5/PP2BPPP/R1B1K2R w KQkq -",
        "8/1b4pk/7q/p3pP2/1p2P3/1Pp5/2P1Q3/5BK1 b - -",
        "r2qkb1r/2p2ppp/p1p5/3pP3/4n1b1/3Q1N2/PPP2PPP/RNB2RK1 w kq -",
        "4r1k1/pp2pp1n/3p2p1/6Bp/2rN1P1P/q1P5/2PQ2P1/1K1RR3 b - -",
        "rnbq1rk1/pp3pbp/2pp1np1/4p3/2PPP3/2N2N2/PP2BPPP/R1BQ1RK1 w - -",
        "8/8/p3k1p1/Bb5p/1P5P/5P2/1r4P1/3R2K1 b - -",
        "4k3/q4p2/2n1p1p1/p1Np4/3P1QP1/PP3P2/6K1/8 b - -",
        "3bn3/4k3/2p3p1/ppN2P2/P5P1/1P1KB3/8/8 b - -",
        "r1b1r1k1/5pbp/pp1p1np1/2qPn3/P1p1P3/2N1BPN1/1P1QB1PP/R1R4K b - -",
        "r1q1rnk1/p1p2pp1/bp2p2p/n2pP3/Q1PP1N1B/P1PB1P2/6PP/R4RK1 w - d6",
        "r3r1k1/1bpnqppp/1p1p4/p2Pp3/1PP1P3/P5P1/1BQ2PBP/1R1R2K1 b - -",
        "1r6/4Pk2/1p1p1P2/p7/8/1P1K2p1/P7/5R2 b - -",
        "1rbqk2r/2pp1ppp/p1n2n2/1pb1p3/P3P3/1B3N2/1PPP1PPP/RNBQ1RK1 w k -",
        "r4bk1/6pp/1B2n3/4P3/pp6/1P2N1P1/P5KP/3R4 w - -",
        "r1bqkb1r/pp1n1ppp/2n5/2ppP3/3P1P2/5N2/PP4PP/RNBQKB1R w KQkq -",
        "r1qr2k1/2p2pPp/p3n3/1p6/8/4BQ2/PP3PPP/R2R2K1 w - -",
        "4r3/1p4k1/p1p1P1pp/3p4/1P1N4/P6B/7P/4K3 w - -",
        "8/R4pk1/4pbq1/8/2P4p/5Q2/8/5K2 b - -",
        "4k3/R4pp1/P7/4P2p/5P2/3p2P1/r7/4K3 w - -",
        "2r3k1/1p4p1/p3bp2/8/N3Pb1p/2R4P/1PP1Q1P1/2q3BK b - -",
        "rnbqkb1r/ppp2ppp/4pn2/3p4/2PP4/2N5/PP2PPPP/R1BQKBNR w KQkq -",
        "8/6N1/8/1k4K1/3n4/8/5P2/8 b - -",
        "rnbqkbnr/ppp1pppp/3p4/8/4P3/8/PPPP1PPP/RNBQKBNR w KQkq -",
        "8/PRn5/3k4/r4p2/5PB1/8/5P2/6K1 w - -",
        "r5k1/4Rpp1/3p3p/2pP4/p1P3P1/1P6/P4PP1/5K2 w - -",
        "5n2/2k1KB2/7P/8/3n1P2/8/p1N5/8 w - -",
        "rnbqkbnr/pp1ppppp/8/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq -",
};

/*----------------------------------------------------------------------+
 |      Functions                                                       |
 +----------------------------------------------------------------------*/

/*----------------------------------------------------------------------+
 |      uciBenchmark                                                    |
 +----------------------------------------------------------------------*/

void uciBenchmark(Engine_t self, double time)
{
        char oldPosition[maxFenSize]; // TODO: clone engine and then share tt instead
        boardToFen(board(self), oldPosition);

        kpkGenerate(); // Initialize before measuring speed
        printf("egt class KPK check %s\n", kpkSelfCheck() ? "OK" : "FAILED");

        long long totalNodes = 0;
        double totalSeconds = 0.0;

        for (int i=0; i<arrayLen(positions); i++) {
                setupBoard(board(self), positions[i]);
                self->targetTime = 0.0;
                self->abortTime = time;
                self->targetDepth = maxDepth;
                self->infoFunction = noInfoFunction;
                rootSearch(self);
                totalNodes += self->nodeCount;
                double s = self->seconds;
                totalSeconds += s;
                double nps = (s > 0.0) ? self->nodeCount / s : 0.0;
                printf("time %.f nps %.f fen %s\n", s * 1e3, nps, positions[i]);
        }

        printf("result nps %.0f\n", (double) totalNodes / totalSeconds);

        setupBoard(board(self), oldPosition);
}

/*----------------------------------------------------------------------+
 |      uciMoves                                                        |
 +----------------------------------------------------------------------*/

void uciMoves(Board_t self, int depth)
{
        int moveList[maxMoves];
        int nrMoves = generateMoves(self, moveList);
        qsort(moveList, nrMoves, sizeof(moveList[0]), compareInt);
        long long totalCount = 0;
        for (int i=0; i<nrMoves; i++) {
                char moveString[maxMoveSize];
                moveToUci(self, moveString, moveList[i]);
                makeMove(self, moveList[i]);
                if (wasLegalMove(self)) {
                        long long count = moveTest(self, depth - 1);
                        printf("move %s count %lld\n", moveString, count);
                        totalCount += count;
                }
                undoMove(self);
        }
        printf("result count %lld\n", totalCount);
}

/*----------------------------------------------------------------------+
 |                                                                      |
 +----------------------------------------------------------------------*/

