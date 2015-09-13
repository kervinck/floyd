
/*----------------------------------------------------------------------+
 |                                                                      |
 |      search.c                                                        |
 |                                                                      |
 +----------------------------------------------------------------------*/

/*----------------------------------------------------------------------+
 |      Includes                                                        |
 +----------------------------------------------------------------------*/

// C standard includes
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

// C extension include
#include "cplus.h"

#include "Board.h"

#include "Engine.h"

#include "evaluate.h"

/*----------------------------------------------------------------------+
 |      Definitions                                                     |
 +----------------------------------------------------------------------*/

/*----------------------------------------------------------------------+
 |      Data                                                            |
 +----------------------------------------------------------------------*/

static const int pieceValue[] = {
        [empty] = -1,
        [whiteKing]   = 27, [whiteQueen]  = 9, [whiteRook] = 5,
        [whiteBishop] = 3,  [whiteKnight] = 3, [whitePawn] = 1,
        [blackKing]   = 27, [blackQueen]  = 9, [blackRook] = 5,
        [blackBishop] = 3,  [blackKnight] = 3, [blackPawn] = 1,
};

static const int promotionValue[] = { 9, 5, 3, 3 };

/*----------------------------------------------------------------------+
 |      Functions                                                       |
 +----------------------------------------------------------------------*/

static int pvSearch(Board_t self, int depth, int alpha, int beta);
static int scout(Board_t self, int depth, int alpha);
static int qSearch(Board_t self, int alpha);
static int exchange(Board_t self, int move);
static int filterAndSort(Board_t self, int moveList[maxMoves], int nrMoves, int moveFilter);

/*----------------------------------------------------------------------+
 |      rootSearch                                                      |
 +----------------------------------------------------------------------*/

int rootSearch(Board_t self, int depth, searchInfo_fn *infoFunction, void *infoData)
{
        int score;
        bool stop = false;

        for (int iteration=0; iteration<=depth && !stop; iteration++) {
                score = pvSearch(self, iteration, -maxInt, maxInt);
                if (infoFunction != null)
                        stop = infoFunction(infoData, iteration, score, 0);
        }

        return score;
}

/*----------------------------------------------------------------------+
 |      ttWrite                                                         |
 +----------------------------------------------------------------------*/

static int ttWrite(Board_t self, int depth, int alpha, int beta, int score)
{
        // DUMMY
        return score;
}

/*----------------------------------------------------------------------+
 |      pvSearch                                                        |
 +----------------------------------------------------------------------*/

static int pvSearch(Board_t self, int depth, int alpha, int beta)
{
        int check = inCheck(self);
        int moveFilter = minInt;
        int bestScore = minInt;

        if (depth == 0 && !check) {
                bestScore = evaluate(self, null, null);
                if (bestScore >= beta)
                        return ttWrite(self, depth, alpha, beta, bestScore);
                moveFilter = 0;
        }

        int moveList[maxMoves];
        int nrMoves = generateMoves(self, moveList);
        nrMoves = filterAndSort(self, moveList, nrMoves, moveFilter);

        // It is easiest to work with legal moves in PVS
        int j = 0;
        for (int i=0; i<nrMoves; i++) {
                makeMove(self, moveList[i]);
                if (wasLegalMove(self))
                        moveList[j++] = moveList[i];
                undoMove(self);
        }
        nrMoves = j;

        int newDepth = max(0, depth - 1 + check);

        if (nrMoves > 0) {
                makeMove(self, moveList[0]);
                int newAlpha = max(alpha, bestScore);
                int score = -pvSearch(self, newDepth, -beta, -newAlpha);
                bestScore = max(bestScore, score);
                undoMove(self);
        }

        for (int i=1; i<nrMoves && bestScore<beta; i++) {
                makeMove(self, moveList[i]);
                int newAlpha = max(alpha, bestScore);
                int score = -scout(self, newDepth, -newAlpha-1);
                if (score > bestScore) {
                        score = -pvSearch(self, newDepth, -beta, -newAlpha);
                        bestScore = max(bestScore, score);
                }
                undoMove(self);
        }

        if (bestScore == minInt)
                bestScore = check ? -32000 + self->plyNumber : 0;

        return ttWrite(self, depth, alpha, beta, bestScore);
}

/*----------------------------------------------------------------------+
 |      scout                                                           |
 +----------------------------------------------------------------------*/

static int scout(Board_t self, int depth, int alpha)
{
        if (depth == 0)
                return qSearch(self, alpha);

        int check = inCheck(self);
        int bestScore = minInt;

        int moveList[maxMoves];
        int nrMoves = generateMoves(self, moveList);
        nrMoves = filterAndSort(self, moveList, nrMoves, minInt);

        for (int i=0; i<nrMoves && bestScore<=alpha; i++) {
                makeMove(self, moveList[i]);
                if (wasLegalMove(self)) {
                        int newDepth = max(0, depth - 1 + check);
                        int score = -scout(self, newDepth, -(alpha+1));
                        bestScore = max(bestScore, score);
                }
                undoMove(self);
        }

        if (bestScore == minInt)
                bestScore = check ? -32000 + self->plyNumber : 0;

        return ttWrite(self, depth, alpha, alpha+1, bestScore);
}

/*----------------------------------------------------------------------+
 |      qSearch                                                         |
 +----------------------------------------------------------------------*/

static int qSearch(Board_t self, int alpha)
{
        int check = inCheck(self);
        int bestScore = check ? minInt : evaluate(self, null, null);

        if (bestScore > alpha)
                return ttWrite(self, 0, alpha, alpha+1, bestScore);

        int moveList[maxMoves];
        int nrMoves = generateMoves(self, moveList);
        nrMoves = filterAndSort(self, moveList, nrMoves, check ? minInt : 0);

        for (int i=0; i<nrMoves && bestScore<=alpha; i++) {
                makeMove(self, moveList[i]);
                if (wasLegalMove(self)) {
                        int score = -qSearch(self, -(alpha+1));
                        bestScore = max(bestScore, score);
                }
                undoMove(self);
        }

        if (bestScore == minInt)
                bestScore = check ? -32000 + self->plyNumber : 0;

        return ttWrite(self, 0, alpha, alpha+1, bestScore);
}

/*----------------------------------------------------------------------+
 |      exchange                                                        |
 +----------------------------------------------------------------------*/

static int exchange(Board_t self, int move)
{
        int from = from(move);
        int to = to(move);

        int victim = self->squares[to];
        int score = pieceValue[victim];

        if (self->xside->attacks[to] != 0) {
                int piece = self->squares[from];
                assert(piece != empty);
                score -= pieceValue[piece];
        } else {
                if (isPromotion(self, from, to))
                        score += promotionValue[move >> promotionBits] - 1;
        }
        return score;
}

/*----------------------------------------------------------------------+
 |      filterAndSort                                                   |
 +----------------------------------------------------------------------*/

// For qsort
static int compareMoves(const void *ap, const void *bp)
{
        int a = ((const intPair *)ap)->v[0];
        int b = ((const intPair *)bp)->v[1];
        return (a < b) - (a > b);
}

static int filterAndSort(Board_t self, int moveList[maxMoves], int nrMoves, int moveFilter)
{
        intPair sortList[maxMoves];

        int n = 0;
        for (int i=0; i<nrMoves; i++) {
                int moveScore = exchange(self, moveList[i]);
                if (moveScore >= moveFilter)
                        sortList[n++] = (intPair) {{ moveScore, moveList[i] }};
        }

        qsort(sortList, n, sizeof(sortList[0]), compareMoves);

        for (int i=0; i<n; i++)
                moveList[i] = sortList[i].v[1];
        return n;
}

/*----------------------------------------------------------------------+
 |                                                                      |
 +----------------------------------------------------------------------*/

