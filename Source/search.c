
/*----------------------------------------------------------------------+
 |                                                                      |
 |      search.c                                                        |
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

#define moveMask ((int) ones(15))
#define moveScore(longMove) ((longMove) >> 26) // Extract score from move list entry
#define historyBits 11 // 15 for a move and 6 for SEE leaves 11 for history
#define historyIndex(move) ((int) ((move) & ones(12)))

/*----------------------------------------------------------------------+
 |      Functions                                                       |
 +----------------------------------------------------------------------*/

static int pvSearch(Engine_t self, int depth, int alpha, int beta, int pvIndex);
static int scout(Engine_t self, int depth, int alpha, int nodeType);
static int qSearch(Engine_t self, int alpha);

static int updateBestAndPonderMove(Engine_t self);
static int staticMoveScore(Board_t self, int move);
static int filterAndSort(Engine_t self, int moveList[], int nrMoves, int moveFilter);
static int filterLegalMoves(Board_t self, int moveList[], int nrMoves);
static bool moveToFront(int moveList[], int nrMoves, int move);
static bool repetition(Engine_t self);
static bool allowNullMove(Board_t self);

static void killersToFront(Engine_t self, int ply, int moveList[], int nrMoves);
static void updateKillers(Engine_t self, int ply, int move);
static void updateHistory(short historyCounts[], int index, int depth);

static int makeFirstMove(Engine_t self, struct Node *node);
static int makeNextMove(Engine_t self, struct Node *node);

/*----------------------------------------------------------------------+
 |      rootSearch                                                      |
 +----------------------------------------------------------------------*/

void abortSearch(void *engine)
{
        Engine_t self = engine;
        self->target.nodeCount = 0;
}

// TODO: aspiration search
void rootSearch(Engine_t self)
{
        double startTime = xTime();
        self->nodeCount = 0;
        self->rootPlyNumber = board(self)->plyNumber;

        assert(board(self)->hash == hash(board(self)));
        if (hash(board(self)) != self->lastSearched) {
                self->lastSearched = hash(board(self));
                self->pv.len = 0;
                self->killers.len = 0;
                self->bestMove = self->ponderMove = 0;
                self->tt.now = (self->tt.now + 1) & ones(ttDateBits);
                memset(self->historyCounts, 0, sizeof self->historyCounts);
        }

        if (self->target.maxTime > 0.0 && !self->pondering)
                self->alarmHandle = setAlarm(self->target.maxTime, abortSearch, self);

        // Prepare abort possibility
        jmp_buf here;
        self->abortTarget = &here;

        if (setjmp(here) == 0) { // try search
                for (int iteration=0; iteration<=self->target.depth; iteration++) {
                        self->mateStop = true;
                        self->depth = iteration;
                        self->score = pvSearch(self, iteration, -maxInt, maxInt, 0);
                        self->seconds = xTime() - startTime;
                        self->infoFunction(self->infoData);
                        updateBestAndPonderMove(self);
                        bool moveReady = self->bestMove && (self->target.time > 0.0)
                                      && (self->seconds >= 0.5 * self->target.time);
                        if (self->score <= self->target.scores.v[0]
                         || self->score >= self->target.scores.v[1]
                         || (isMateScore(self->score) && self->mateStop && self->depth > 0)
                         || (moveReady && !self->pondering))
                                break;
                }
        } else { // except abort
                self->seconds = xTime() - startTime;
                while (ply(self) > 0)
                        undoMove(board(self));
                int pvCut = updateBestAndPonderMove(self);
                self->pv.len = min(self->pv.len, pvCut);
                self->infoFunction(self->infoData);
        }

        clearAlarm(self->alarmHandle);
        self->alarmHandle = null;
}

/*----------------------------------------------------------------------+
 |      gameOverScore / drawScore                                       |
 +----------------------------------------------------------------------*/

static inline int gameOverScore(Engine_t self, bool inCheck)
{
        return inCheck ? minMate + ply(self) : 0;
}

static inline int drawScore(Engine_t self)
{
        unused(self);
        return 0; // TODO: heuristic draws
}

/*----------------------------------------------------------------------+
 |      pvSearch                                                        |
 +----------------------------------------------------------------------*/

// TODO: killers
// TODO: end game extension
// TODO: singular extension
static int pvSearch(Engine_t self, int depth, int alpha, int beta, int pvIndex)
{
        self->nodeCount++;
        bool inRoot = (ply(self) == 0);
        #define cutPv() (self->pv.len = pvIndex)
        int eval = evaluate(board(self));

        if (!inRoot && (eval == 0 || repetition(self)))
                return cutPv(), drawScore(self);

        // Transposition table pruning
        struct ttSlot slot = ttRead(self);
        if ((slot.depth >= depth || slot.isHardBound) && !inRoot)
                if ((slot.isUpperBound && slot.score <= alpha)
                 || (slot.isLowerBound && slot.score >= beta)
                 || (slot.isUpperBound && slot.isLowerBound && alpha < slot.score && slot.score < beta))
                        return cutPv(), slot.score;

        int inCheck = isInCheck(board(self));
        int moveFilter = minInt; // All moves
        int bestScore = minInt;

        // Quiescence search
        if (depth == 0 && !inCheck) {
                bestScore = eval;
                if (bestScore >= beta)
                        return cutPv(), ttWrite(self, slot, depth, bestScore, alpha, beta);
                moveFilter = 0; // Only good captures
        }

        // Generate moves, or use the `searchmoves' list when specified
        int moveList[maxMoves];
        int nrMoves = generateMoves(board(self), moveList);
        if (inRoot && self->searchMoves.len > 0) {
                nrMoves = self->searchMoves.len;
                memcpy(moveList, self->searchMoves.v, nrMoves * sizeof(int));
        }
        nrMoves = filterAndSort(self, moveList, nrMoves, moveFilter);
        nrMoves = filterLegalMoves(board(self), moveList, nrMoves); // Easier for PVS
        moveToFront(moveList, nrMoves, slot.move);

        // Search the first move with open alpha-beta window
        if (nrMoves > 0) {
                if (pvIndex < self->pv.len)
                        moveToFront(moveList, nrMoves, self->pv.v[pvIndex]); // Follow the PV
                else
                        pushList(self->pv, moveList[0]); // Expand the PV
                int move = moveList[0];
                bool recapture = moveScore(move) > 0 && to(move) == recaptureSquare(board(self));
                makeMove(board(self), move);
                int extension = (inCheck || recapture) + (nrMoves == 1 && (depth > 0));
                int newDepth = max(0, depth - 1 + extension);
                int newAlpha = max(alpha, bestScore);
                int score = -pvSearch(self, newDepth, -beta, -newAlpha, pvIndex + 1);
                if (score > bestScore) {
                        bestScore = score;
                        slot.move = move & moveMask;
                } else
                        cutPv(); // Quiescence (standing pat)
                undoMove(board(self));
        } else
                cutPv(); // Game end or leaf node (horizon)

        // Try the others with zero window and reductions, research if needed
        int reduction = min(2, depth / 5);
        for (int i=1; i<nrMoves && bestScore<beta; i++) {
                int move = moveList[i];
                bool recapture = moveScore(move) > 0 && to(move) == recaptureSquare(board(self));
                makeMove(board(self), move);
                int extension = (inCheck || recapture);
                int newDepth = max(0, depth - 1 + extension - reduction);
                int newAlpha = max(alpha, bestScore);
                int score = -scout(self, newDepth, -(newAlpha+1), 1);
                if (!isMateScore(score) && !isDrawScore(score))
                        self->mateStop = false; // Shortest mate not yet proven
                if (score > bestScore) {
                        pushList(self->pv, 0); // Separator
                        int pvLen = self->pv.len;
                        pushList(self->pv, move);
                        int researchDepth = max(0, depth - 1 + extension);
                        score = -pvSearch(self, researchDepth, -beta, -newAlpha, pvLen + 1);
                        if (score > bestScore) {
                                bestScore = score;
                                slot.move = move & moveMask;
                                for (int j=0; pvLen+j<self->pv.len; j++)
                                        self->pv.v[pvIndex+j] = self->pv.v[pvLen+j];
                                self->pv.len -= pvLen - pvIndex;
                        } else
                                self->pv.len = pvLen - 1; // The research failed, it happens
                }
                undoMove(board(self));
        }

        if (bestScore == minInt) // No legal moves
                bestScore = gameOverScore(self, inCheck);

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
        if (depth == 0) return qSearch(self, alpha);
        if (self->nodeCount >= self->target.nodeCount)
                longjmp(self->abortTarget, 1); // Raise abort

        // Mate distance pruning
        int mateBound = maxMate - ply(self) - 2;
        if (alpha >= mateBound) return mateBound;

        // Transposition table pruning
        struct Node node;
        node.slot = ttRead(self);
        if (node.slot.depth >= depth || node.slot.isHardBound)
                if ((node.slot.isUpperBound && node.slot.score <= alpha)
                 || (node.slot.isLowerBound && node.slot.score > alpha))
                        return node.slot.score;

        // Null move pruning
        int inCheck = isInCheck(board(self));
        if (depth >= 2 && minEval <= alpha && alpha < maxEval
         && !inCheck && allowNullMove(board(self))) {
                makeNullMove(board(self));
                int reduction = min((depth + 1) / 2, 3);
                int score = -scout(self, max(0, depth - reduction - 1), -(alpha+1), nodeType+1);
                undoMove(board(self));
                if (score > alpha)
                        return ttWrite(self, node.slot, depth, score, alpha, alpha+1);
        }

        // Internal iterative deepening
        if (depth >= 3 && isCutNode(nodeType) && !node.slot.move) {
                scout(self, depth - 2, alpha, nodeType);
                node.slot = ttRead(self);
        }

        // Recursively search all other moves until exhausted or one fails high
        int extension = inCheck;
        int bestScore = minInt;
        for (int move=makeFirstMove(self,&node), j=0; move; move=makeNextMove(self,&node), j++) {
                int newDepth = max(0, depth - 1 + extension);
                int reduction = (depth >= 4) && (j >= 1) && (move < 0);
                int reducedDepth = max(0, newDepth - reduction);
                int score = -scout(self, reducedDepth, -(alpha+1), nodeType+1);
                if (score > alpha && reducedDepth < newDepth)
                        score = -scout(self, newDepth, -(alpha+1), nodeType+1);
                undoMove(board(self));
                bestScore = max(bestScore, score);
                if (score > alpha) { // Fail high
                        node.slot.move = move & moveMask;
                        if (j > 0) {
                                updateKillers(self, ply(self), move);
                                updateHistory(self->historyCounts, historyIndex(move), depth);
                        }
                        break;
                }
        }

        if (bestScore == minInt) // No legal moves
                bestScore = gameOverScore(self, inCheck);

        return ttWrite(self, node.slot, depth, bestScore, alpha, alpha+1);
}

/*----------------------------------------------------------------------+
 |      qSearch                                                         |
 +----------------------------------------------------------------------*/

static int qSearch(Engine_t self, int alpha)
{
        // Transposition table pruning
        struct ttSlot slot = ttRead(self);
        if ((slot.isUpperBound && slot.score <= alpha)
         || (slot.isLowerBound && slot.score > alpha))
                return slot.score;

        // Stand pat if evaluation is good and not in check
        int inCheck = isInCheck(board(self));
        int bestScore = inCheck ? minInt : evaluate(board(self));
        if (bestScore > alpha)
                return ttWrite(self, slot, 0, bestScore, alpha, alpha+1);

        // Generate good captures, or all escapes when in check
        int moveList[maxMoves];
        int nrMoves = generateMoves(board(self), moveList);
        nrMoves = filterAndSort(self, moveList, nrMoves, inCheck ? minInt : 0);
        moveToFront(moveList, nrMoves, slot.move);

        // Try if any generated move can improve the result
        for (int i=0; i<nrMoves && bestScore<=alpha; i++) {
                if (!inCheck) {
                        // Regular delta pruning
                        assert(moveList[i] >= 0);
                        int maxDelta = (moveList[i] >> 26) * 1200 + 1200;
                        if (maxDelta <= alpha - bestScore)
                                return ttWrite(self, slot, 0, bestScore + maxDelta, alpha, alpha+1);
                }

                // Search deeper
                makeMove(board(self), moveList[i]);
                if (wasLegalMove(board(self))) {
                        self->nodeCount++;
                        int score = -qSearch(self, -(alpha+1));
                        bestScore = max(bestScore, score);
                        if (score > alpha)
                                slot.move = moveList[i] & moveMask;
                }
                undoMove(board(self));
        }

        if (bestScore == minInt) // No legal moves
                bestScore = gameOverScore(self, inCheck);

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
                undoMove(board(self));
        }
        return makeNextMove(self, node);
}

static int makeNextMove(Engine_t self, struct Node *node)
{
        if (node->phase == 0) {
                int ttMove = node->moveList[0];
                node->nrMoves = generateMoves(board(self), node->moveList);
                node->nrMoves = filterAndSort(self, node->moveList, node->nrMoves, minInt);
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
 |      staticMoveScore                                                 |
 +----------------------------------------------------------------------*/

// Static Exchange Evaluation (SEE)
static int see(int next, int attackers, int defenders, int prom)
{
        if (!attackers) return 0;
        else if (attackers >= attackPawn)
                             next += prom - see(1 + prom, defenders, attackers - attackPawn,  prom);
        else if (attackers >= attackMinor) next -= see(3, defenders, attackers - attackMinor, prom);
        else if (attackers >= attackRook)  next -= see(5, defenders, attackers - attackRook,  prom);
        else if (attackers >= attackQueen) next -= see(9, defenders, attackers - attackQueen, prom);
        else /* attackers >= attackKing */ if (defenders) return 0;
        return max(0, next);
}

static int staticMoveScore(Board_t self, int move)
{
        static const int pieceValue[] = {
                [empty] = -1, // rank non-captures behind neutral exchange sequences
                [whiteKing]   = 27, [whiteQueen]  = 9, [whiteRook] = 5,
                [whiteBishop] = 3,  [whiteKnight] = 3, [whitePawn] = 1,
                [blackKing]   = 27, [blackQueen]  = 9, [blackRook] = 5,
                [blackBishop] = 3,  [blackKnight] = 3, [blackPawn] = 1,
        };
        static const int pieceAttack[] = {
                [27] = attackKing, [9] = attackQueen, [5] = attackRook, [3] = attackMinor
        };
        static const int promotionValue[] = { 9, 5, 3, 3 };

        int to = to(move);
        int victim = self->squares[to]; // may be empty
        int score = pieceValue[victim];

        int from = from(move);
        int piece = self->squares[from];
        int next = pieceValue[piece];

        int side = sideToMove(self);
        int attackers = self->sides[side].attacks[to] - pieceAttack[next];
        if (next == 1 && score >= 0) attackers -= attackPawn; // TODO: en passant?

        bool lastRank = (rank(to) == rank1 || rank(to) == rank8);
        if (next == 1 && lastRank) { // pawn promotion
                next = promotionValue[(move>>promotionBits)&3];
                score += next - 1;
        }

        int defenders = self->sides[other(side)].attacks[to];
        if (defenders) score -= see(next, defenders, attackers, lastRank ? 8 : 0);
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

static int filterAndSort(Engine_t self, int moveList[], int nrMoves, int moveFilter)
{
        int j = 0;
        for (int i=0; i<nrMoves; i++) {
                int moveScore = staticMoveScore(board(self), moveList[i]);
                if (moveScore >= moveFilter)
                        moveList[j++] = (moveScore << 26)
                                      + (self->historyCounts[historyIndex(moveList[i])] << 15)
                                      + (moveList[i] & moveMask);
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

        int j = 0; // Find insertion place: after any (very) good captures
        while (j < nrMoves && moveScore(moveList[j]) >= 3) j++;

        for (int i=nrKillers-1; i>=0; i--) // Bring killers forward, one by one in reverse order
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

static void updateHistory(short historyCounts[], int index, int depth)
{
        historyCounts[index] += min(64, depth * depth); // Rookie v1
        if (historyCounts[index] >= (1 << historyBits))
                for (int i=0; i <= historyIndex(~0); i++)
                        historyCounts[i] >>= 1;
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
                int longMove = moveList[i];
                if ((longMove & moveMask) == move) {
                        memmove(&moveList[1], &moveList[0], i * sizeof(moveList[0]));
                        moveList[0] = longMove;
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
        static const int table[] = { // 1=slider, 2=white piece, 4=black piece
                [empty] = 0,
                [whiteKing]   = 0,   [whiteQueen]  = 1+2, [whiteRook] = 1+2,
                [whiteBishop] = 1+2, [whiteKnight] = 2,   [whitePawn] = 0,
                [blackKing]   = 0,   [blackQueen]  = 1+4, [blackRook] = 1+4,
                [blackBishop] = 1+4, [blackKnight] = 4,   [blackPawn] = 0,
        };
        int bits = 0;
        for (int i=0; i<boardSize; i++)
                bits |= table[self->squares[i]];
        return bits == 7;
}

/*----------------------------------------------------------------------+
 |      updateBestAndPonderMove                                         |
 +----------------------------------------------------------------------*/

// Safe update from a possibly aborted PV
static int updateBestAndPonderMove(Engine_t self)
{
        if (self->pv.len > 0 && self->pv.v[0] != self->bestMove)
                self->ponderMove = 0;
        if (self->pv.len >= 3) // At least 3 to avoid pondering at game-end
                self->ponderMove = self->pv.v[1];

        if (self->pv.len > 0)
                self->bestMove = self->pv.v[0];

        return !self->bestMove + !self->ponderMove; // 0, 1 or 2 halfmoves
}

/*----------------------------------------------------------------------+
 |      setTimeTargets                                                  |
 +----------------------------------------------------------------------*/

static double target(double time, double inc, int movestogo)
{
        double safety = (inc < 0.05) ? 20.0 : 2.5;
        double target = (time + (movestogo - 1) * inc - safety) / movestogo;
        return max(target, 0.05);
}

void setTimeTargets(Engine_t self, double time, double inc, int movestogo, double movetime)
{
        if (time > 0.0 || inc > 0.0) {
                if (!movestogo) // Default time allocation horizon
                        movestogo = 25;
                switch (board(self)->halfmoveClock / 2) { // Some "mild panics" when no progress
                case 15: movestogo = min(movestogo, 5); break;
                case 25: movestogo = min(movestogo, 4); break;
                case 35: movestogo = min(movestogo, 3); break;
                case 45: movestogo = min(movestogo, 2); break;
                }
                int mintogo = max(1, movestogo / 5); // Upto 5 times the target
                self->target.time = target(time, inc, movestogo);
                double panicTime = target(time, inc, mintogo);
                double flagTime = target(time, inc, 1);
                self->target.maxTime  = min(panicTime, flagTime);
        } else
                self->target.time = self->target.maxTime = 0.0;
        if (movetime > 0.0)
                self->target.maxTime = movetime;
}

/*----------------------------------------------------------------------+
 |      noInfoFunction                                                  |
 +----------------------------------------------------------------------*/

void noInfoFunction(void *infoData)
{
        unused(infoData);
}

/*----------------------------------------------------------------------+
 |                                                                      |
 +----------------------------------------------------------------------*/

