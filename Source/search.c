
/*----------------------------------------------------------------------+
 |                                                                      |
 |      search.c                                                        |
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
#include <assert.h>
#include <math.h>
#include <setjmp.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// C extension
#include "cplus.h"

// Own interface
#include "Board.h"
#include "Engine.h"

/*----------------------------------------------------------------------+
 |      Definitions                                                     |
 +----------------------------------------------------------------------*/

struct Node {
        struct ttSlot slot;
        int phase; // Lazy move generation
        int nrMoves, i;
        int moveList[maxMoves];
};

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

static const int allowNullMoveTable[] = { // 1=slider, 2=white piece, 4=black piece
        [empty] = 0,
        [whiteKing]   = 0, [whiteQueen]  = 3, [whiteRook] = 3,
        [whiteBishop] = 3, [whiteKnight] = 2, [whitePawn] = 0,
        [blackKing]   = 0, [blackQueen]  = 5, [blackRook] = 5,
        [blackBishop] = 5, [blackKnight] = 4, [blackPawn] = 0,
};

/*----------------------------------------------------------------------+
 |      Functions                                                       |
 +----------------------------------------------------------------------*/

static int pvSearch(Engine_t self, int depth, int alpha, int beta, int pvIndex);
static int scout(Engine_t self, int depth, int alpha, int nodeType);
static int qSearch(Engine_t self, int alpha);

static int updateBestAndPonderMove(Engine_t self);
static int exchange(Board_t self, int move);
static int filterAndSort(Board_t self, int moveList[], int nrMoves, int moveFilter);
static int filterLegalMoves(Board_t self, int moveList[], int nrMoves);
static bool moveToFront(int moveList[], int nrMoves, int move);
static bool repetition(Engine_t self);
static bool allowNullMove(Board_t self);

static void killersToFront(Engine_t self, int ply, int moveList[], int nrMoves);
static void updateKillers(Engine_t self, int ply, int move);

static int makeFirstMove(Engine_t self, struct Node *node);
static int makeNextMove(Engine_t self, struct Node *node);

/*----------------------------------------------------------------------+
 |      rootSearch                                                      |
 +----------------------------------------------------------------------*/

static void stopSearch(void *data)
{
        Engine_t self = data;
        self->stopFlag = true;
}

// TODO: all timing decisions from search.c
// TODO: aspiration search
void rootSearch(Engine_t self,
        int depth,
        double targetTime,
        double alarmTime,
        searchInfo_fn *infoFunction, void *infoData)
{
        double startTime = xTime();
        self->nodeCount = 0;
        self->rootPlyNumber = board(self)->plyNumber;

        self->infoFunction = infoFunction;
        self->infoData = infoData;

        assert(board(self)->hash == hash(board(self)));
        if (hash(board(self)) != self->lastSearched) {
                self->lastSearched = hash(board(self));
                self->pv.len = 0;
                self->killers.len = 0;
                updateBestAndPonderMove(self);
                self->tt.now = (self->tt.now + 1) & ones(ttDateBits);
        }

        self->stopFlag = false;
        xAlarm_t alarmHandle = null;
        if (alarmTime > 0.0)
                alarmHandle = setAlarm(alarmTime, stopSearch, self);

        // Prepare abort possibility
        jmp_buf here;
        self->abortTarget = &here;

        if (setjmp(here) == 0) { // try search
                bool stop = false;
                for (int iteration=0; iteration<=depth && !stop; iteration++) {
                        self->mateStop = true;
                        self->depth = iteration;
                        self->score = pvSearch(self, iteration, -maxInt, maxInt, 0);
                        self->seconds = xTime() - startTime;
                        updateBestAndPonderMove(self);
                        stop = infoFunction(infoData, null)
                            || (isMateScore(self->score) && self->mateStop && self->depth > 0)
                            || (targetTime > 0.0 && self->seconds >= 0.5 * targetTime); // TODO: remove
                }
        } else { // except abort
                self->seconds = xTime() - startTime;
                while (ply(self) > 0)
                        undoMove(board(self));
                int pvCut = updateBestAndPonderMove(self);
                self->pv.len = min(self->pv.len, pvCut);
                (void) infoFunction(infoData, null);
        }

        clearAlarm(alarmHandle);
}

/*----------------------------------------------------------------------+
 |      endScore / drawScore                                            |
 +----------------------------------------------------------------------*/

// TODO: move to evaluate.h
static inline int endScore(Engine_t self, bool inCheck)
{
        return inCheck ? minMate + ply(self) : 0;
}

static inline int drawScore(Engine_t self)
{
        return 0; // TODO: heuristic draws
}

/*----------------------------------------------------------------------+
 |      pvSearch                                                        |
 +----------------------------------------------------------------------*/

// TODO: single-reply extensions
// TODO: killers
static int pvSearch(Engine_t self, int depth, int alpha, int beta, int pvIndex)
{
        self->nodeCount++;
        bool inRoot = (ply(self) == 0);
        #define cutPv() (self->pv.len = pvIndex)

        if (repetition(self) && !inRoot)
                return cutPv(), drawScore(self);

        struct ttSlot slot = ttRead(self);
        if ((slot.depth >= depth || slot.isHardBound) && !inRoot)
                if ((slot.isUpperBound && slot.score <= alpha)
                 || (slot.isLowerBound && slot.score >= beta)
                 || (slot.isUpperBound && slot.isLowerBound && alpha < slot.score && slot.score < beta))
                        return cutPv(), slot.score;

        int check = inCheck(board(self));
        int extension = check;
        int moveFilter = minInt;
        int bestScore = minInt;

        if (depth == 0 && !check) {
                bestScore = evaluate(board(self));
                if (bestScore >= beta) {
                        self->pv.len = pvIndex;
                        return ttWrite(self, slot, depth, bestScore, alpha, beta);
                }
                moveFilter = 0;
        }

        int moveList[maxMoves];
        int nrMoves = generateMoves(board(self), moveList);
        nrMoves = filterAndSort(board(self), moveList, nrMoves, moveFilter);
        nrMoves = filterLegalMoves(board(self), moveList, nrMoves); // easier for PVS
        moveToFront(moveList, nrMoves, slot.move);

        // Search the first move with open alpha-beta window
        if (nrMoves > 0) {
                if (pvIndex < self->pv.len)
                        moveToFront(moveList, nrMoves, self->pv.v[pvIndex]); // follow the pv
                else
                        pushList(self->pv, moveList[0]); // expand the pv
                makeMove(board(self), moveList[0]);
                int newDepth = max(0, depth - 1 + extension);
                int newAlpha = max(alpha, bestScore);
                int score = -pvSearch(self, newDepth, -beta, -newAlpha, pvIndex + 1);
                if (score > bestScore) {
                        bestScore = score;
                        slot.move = moveList[0];
                } else
                        cutPv(); // quiescence
                undoMove(board(self));
        } else
                cutPv(); // game end or leaf node (horizon)

        // Search the others with zero window and reductions, research if needed
        int reduction = 2;
        for (int i=1; i<nrMoves && bestScore<beta; i++) {
                makeMove(board(self), moveList[i]);
                int newDepth = max(0, depth - 1 + extension - reduction);
                int newAlpha = max(alpha, bestScore);
                int score = -scout(self, newDepth, -(newAlpha+1), 1);
                if (isMateWinScore(bestScore) && !isMateScore(score) && !isDrawScore(score))
                        self->mateStop = false; // shortest mate not yet proven
                if (score > bestScore) {
                        int pvLen = self->pv.len;
                        pushList(self->pv, moveList[i]);
                        int researchDepth = max(0, depth - 1 + extension);
                        score = -pvSearch(self, researchDepth, -beta, -newAlpha, pvLen + 1);
                        if (score > bestScore) {
                                bestScore = score;
                                slot.move = moveList[i];
                                for (int j=0; pvLen+j<self->pv.len; j++)
                                        self->pv.v[pvIndex+j] = self->pv.v[pvLen+j];
                                self->pv.len -= pvLen - pvIndex;
                        } else
                                self->pv.len = pvLen; // research failed, it happens
                }
                undoMove(board(self));
        }

        if (bestScore == minInt)
                bestScore = endScore(self, check);

        return ttWrite(self, slot, depth, bestScore, alpha, beta);
}

/*----------------------------------------------------------------------+
 |      scout                                                           |
 +----------------------------------------------------------------------*/

#define isCutNode(nodeType) (( nodeType) & 1)
#define isAllNode(nodeType) ((~nodeType) & 1)

// TODO: futility
static int scout(Engine_t self, int depth, int alpha, int nodeType)
{
        self->nodeCount++;
        if (repetition(self)) return drawScore(self);
        if (depth == 0)       return qSearch(self, alpha);
        if (self->stopFlag)   longjmp(self->abortTarget, 1); // raise abort

        // Mate distance pruning
        int mateBound = maxMate - ply(self) - 2;
        if (mateBound <= alpha) return mateBound;

        int check = inCheck(board(self));
        struct Node node;
        node.slot = ttRead(self);

        // Internal iterative deepening
        if (depth >= 3 && isCutNode(nodeType) && !node.slot.move) {
                scout(self, depth - 2, alpha, nodeType);
                node.slot = ttRead(self);
        }

        // Transposition table pruning
        if (node.slot.depth >= depth || node.slot.isHardBound)
                if ((node.slot.isUpperBound && node.slot.score <= alpha)
                 || (node.slot.isLowerBound && node.slot.score > alpha))
                        return node.slot.score;

        // Null move pruning
        if (depth >= 2 && isCutNode(nodeType) && minEval <= alpha && alpha < maxEval && !check && allowNullMove(board(self))) {
                makeNullMove(board(self));
                int score = -scout(self, max(0, depth - 2 - 1), -(alpha+1), nodeType+1);
                undoMove(board(self));
                if (score > alpha)
                        return ttWrite(self, node.slot, depth, score, alpha, alpha+1);
        }

        // Search deeper until all moves exhausted or one fails high
        int extension = check;
        int bestScore = minInt;
        for (int j=0, move=makeFirstMove(self,&node); move; j++, move=makeNextMove(self,&node)) {
                int newDepth = max(0, depth - 1 + extension);
                int score = -scout(self, newDepth, -(alpha+1), nodeType+1);
                undoMove(board(self));
                bestScore = max(bestScore, score);
                if (score > alpha) {
                        node.slot.move = move;
                        if (j > 0) updateKillers(self, ply(self), move);
                        break;
                }
        }

        if (bestScore == minInt)
                bestScore = endScore(self, check);

        return ttWrite(self, node.slot, depth, bestScore, alpha, alpha+1);
}

/*----------------------------------------------------------------------+
 |      qSearch                                                         |
 +----------------------------------------------------------------------*/

static int qSearch(Engine_t self, int alpha)
{
        struct ttSlot slot = ttRead(self);
        if ((slot.isUpperBound && slot.score <= alpha)
         || (slot.isLowerBound && slot.score > alpha))
                return slot.score;

        int check = inCheck(board(self));
        int bestScore = check ? minInt : evaluate(board(self));
        if (bestScore > alpha)
                return ttWrite(self, slot, 0, bestScore, alpha, alpha+1);

        int moveList[maxMoves];
        int nrMoves = generateMoves(board(self), moveList);
        nrMoves = filterAndSort(board(self), moveList, nrMoves, check ? minInt : 0);
        moveToFront(moveList, nrMoves, slot.move);

        for (int i=0; i<nrMoves && bestScore<=alpha; i++) {
                makeMove(board(self), moveList[i]);
                if (wasLegalMove(board(self))) {
                        self->nodeCount++;
                        int score = -qSearch(self, -(alpha+1));
                        bestScore = max(bestScore, score);
                        if (score > alpha)
                                slot.move = moveList[i];
                }
                undoMove(board(self));
        }

        if (bestScore == minInt)
                bestScore = endScore(self, check);

        return ttWrite(self, slot, 0, bestScore, alpha, alpha+1);
}

/*----------------------------------------------------------------------+
 |      repetition                                                      |
 +----------------------------------------------------------------------*/

// Search for a simple repetition upto root, or for threefold before root
static bool repetition(Engine_t self)
{
        Board_t board = board(self);
        if (board->halfmoveClock < 4)
                return false;
        int ix = board->hashHistory.len;
        int lastZeroing = max(0, ix - board->halfmoveClock);
        int searchRoot = ix - ply(self);
        int count = 1;
        for (ix=ix-4; ix>=lastZeroing; ix-=2)
                if (board(self)->hashHistory.v[ix] == board->hash)
                        if (++count >= 3 || ix >= searchRoot)
                                return true;
        return false;
}

/*----------------------------------------------------------------------+
 |      Lazy move generator                                             |
 +----------------------------------------------------------------------*/

static int makeFirstMove(Engine_t self, struct Node *node)
{
        node->phase = 0;
        int ttMove = node->moveList[0] = node->slot.move;
        if (ttMove) {
                makeMove(board(self), ttMove);
                if (wasLegalMove(board(self)))
                        return ttMove;
                self->infoFunction(self->infoData, "Illegal hash table move");
                undoMove(board(self));
        }
        return makeNextMove(self, node);
}

static int makeNextMove(Engine_t self, struct Node *node)
{
        if (node->phase == 0) {
                int ttMove = node->moveList[0];
                node->nrMoves = generateMoves(board(self), node->moveList);
                node->nrMoves = filterAndSort(board(self), node->moveList, node->nrMoves, minInt);
                killersToFront(self, ply(self), node->moveList, node->nrMoves);
                node->i = moveToFront(node->moveList, node->nrMoves, ttMove); // skip if already emitted
                node->phase = 1;
        }
        if (node->phase == 1)
                while (node->i < node->nrMoves) {
                        int move = node->moveList[node->i++];
                        makeMove(board(self), move);
                        if (wasLegalMove(board(self)))
                                return move;
                        undoMove(board(self));
                }
        return 0;
}

/*----------------------------------------------------------------------+
 |      exchange (not really "SEE" yet)                                 |
 +----------------------------------------------------------------------*/

static int exchange(Board_t self, int move)
{
        int from = from(move);
        int to = to(move);

        int piece  = self->squares[from];
        int victim = self->squares[to]; // can be empty
        int score  = pieceValue[victim];

        if (self->xside->attacks[to] != 0)
                score -= pieceValue[piece];
        else if (isPromotion(self, from, to))
                score += promotionValue[(move>>promotionBits)&3] - pieceValue[piece];
        return score;
}

/*----------------------------------------------------------------------+
 |      filterAndSort                                                   |
 +----------------------------------------------------------------------*/

// Comparator for qsort: descending order of prescore
static int compareMoves(const void *ap, const void *bp)
{
        int a = *(const int*)ap;
        int b = *(const int*)bp;
        return (a < b) - (a > b);
}

static int filterAndSort(Board_t self, int moveList[], int nrMoves, int moveFilter)
{
        int j = 0;
        for (int i=0; i<nrMoves; i++) {
                int moveScore = exchange(self, moveList[i]);
                if (moveScore >= moveFilter)
                        moveList[j++] = (moveScore << 16) + (moveList[i] & moveMask);
        }
        qsort(moveList, j, sizeof(moveList[0]), compareMoves);
        return j;
}

/*----------------------------------------------------------------------+
 |      filterLegalMoves                                                |
 +----------------------------------------------------------------------*/

static int filterLegalMoves(Board_t self, int moveList[], int nrMoves)
{
        int j = 0;
        for (int i=0; i<nrMoves; i++) {
                makeMove(self, moveList[i]);
                if (wasLegalMove(self))
                        moveList[j++] = moveList[i];
                undoMove(self);
        }
        return j;
}

/*----------------------------------------------------------------------+
 |      killers                                                         |
 +----------------------------------------------------------------------*/

static void killersToFront(Engine_t self, int ply, int moveList[], int nrMoves)
{
        while (self->killers.len <= ply) // Expand table when needed
                pushList(self->killers, (killersTuple) {.v={0}});

        int j=0;
        while (j < nrMoves && moveList[j] >= (3 << 16)) j++;

        for (int i=nrKillers-1; i>=0; i--)
                moveToFront(moveList+j, nrMoves-j, self->killers.v[ply].v[i]);
}

static void updateKillers(Engine_t self, int ply, int move)
{
        killersTuple *killers = &self->killers.v[ply];
        int i = nrKillers-1;
        while (i >= 0 && killers->v[i] != (move & moveMask))
                i--;

        if (i < 0) {
                // Insert new killer and shift the others down (demote)
                if (killers->v[newKillerIndex] != 0)
                        for (int i=nrKillers-1; i>newKillerIndex; i--)
                                killers->v[i] = killers->v[i-1];
                killers->v[newKillerIndex] = move & moveMask;
        } else if (i > 0) {
                // Shift up or promote a pre-existing killer
                killers->v[i] = killers->v[i-1];
                killers->v[i-1] = move & moveMask;
        }
}

/*----------------------------------------------------------------------+
 |      moveToFront                                                     |
 +----------------------------------------------------------------------*/

static bool moveToFront(int moveList[], int nrMoves, int move)
{
        move &= moveMask;
        if (move == 0)
                return false;

        for (int i=0; i<nrMoves; i++) {
                if ((moveList[i] & moveMask) == move) {
                        memmove(&moveList[1], &moveList[0], i * sizeof(moveList[0]));
                        moveList[0] = move;
                        return true;
                }
        }
        return false;
}

/*----------------------------------------------------------------------+
 |      allowNullMove                                                   |
 +----------------------------------------------------------------------*/

// Both sides must have pieces and there must be a slider
static bool allowNullMove(Board_t self)
{
        int bits = 0;
        for (int i=0; i<boardSize; i++)
                bits |= allowNullMoveTable[self->squares[i]];
        return bits == 7;
}

/*----------------------------------------------------------------------+
 |      updateBestAndPonderMove                                         |
 +----------------------------------------------------------------------*/

// Safe update from a possibly aborted PV
static int updateBestAndPonderMove(Engine_t self)
{
        if (self->pv.len >= 1 && self->pv.v[0] != self->bestMove)
                self->ponderMove = 0;
        if (self->pv.len >= 2)
                self->ponderMove = self->pv.v[1];

        if (self->pv.len >= 1)
                self->bestMove = self->pv.v[0];

        return !self->bestMove + !self->ponderMove; // 0, 1 or 2 halfmoves
}

/*----------------------------------------------------------------------+
 |                                                                      |
 +----------------------------------------------------------------------*/

