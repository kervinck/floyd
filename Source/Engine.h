
/*----------------------------------------------------------------------+
 |                                                                      |
 |      Engine.h                                                        |
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
 |      Definitions                                                     |
 +----------------------------------------------------------------------*/

#define maxDepth 120
#define nrKillers 5
#define newKillerIndex 2

typedef struct Engine *Engine_t;

/*
 *  Transposition table
 */

#define ttDepthBits 8
#define ttDateBits 12

enum {
        minMate = -32000, minEval = -29999, minDtz  = -31000,
        maxMate =  32000, maxEval =  29999, maxDtz  =  31000,
};

struct ttSlot {
        uint64_t key;
        union {
                struct {
                        short move;
                        short score;
                        unsigned depth          : ttDepthBits;
                        unsigned date           : ttDateBits;
                        unsigned isUpperBound   : 1;
                        unsigned isLowerBound   : 1;
                        unsigned isHardBound    : 1; // Game theoretical value, can ignore depth
                        unsigned isWinLossScore : 1; // DTZ or mate
                };
                uint64_t data; // For lockless hashing
        };
};

typedef Tuple(int, nrKillers) killersTuple;

#define ply(self) (board(self)->plyNumber - (self)->rootPlyNumber)

/*
 *  Chess engine
 */
struct Engine {
        struct Board board;

        int rootPlyNumber;
        intList searchMoves; // root moves to search, empty means all
        volatile bool stopFlag;

        // transposition table
        struct {
                struct ttSlot *slots;
                size_t size;
                size_t mask;
                unsigned int now;  // incremented when root changes
                uint64_t baseHash; // For fast clearing
        } tt;

        List(killersTuple) killers;

        // last search result
        struct {
                uint64_t lastSearched;
                int score;
                int depth;
                int bestMove;
                int ponderMove;
                intList pv;
                double seconds;
                long long nodeCount;
        };

        void *abortTarget;
};

/*
 *  Workaround to hide some of the ugliness, at least until ISO C supports
 *  seamless access to members of the base struct (like `-fms-extensions'
 *  or `kenc'). Note that this is a `nop' (just a type conversion) because
 *  `board' is the first element of `struct Engine'.
 */
#define board(engine) (&(engine)->board)

// Callback interface for handling of search progress
typedef bool searchInfo_fn(void *infoData);

/*----------------------------------------------------------------------+
 |      Functions                                                       |
 +----------------------------------------------------------------------*/

/*
 *  Search
 */

void rootSearch(Engine_t self,
                int depth,
                double targetTime, double alarmTime,
                searchInfo_fn *infoFunction, void *infoData);
void abortSearch(Engine_t self);

/*
 *  Transposition table
 */

void ttSetSize(Engine_t self, size_t size);
int ttWrite(Engine_t self, struct ttSlot slot, int depth, int score, int alpha, int beta);
struct ttSlot ttRead(Engine_t self);
void ttClearFast(Engine_t self);
double ttCalcLoad(Engine_t self);

/*----------------------------------------------------------------------+
 |                                                                      |
 +----------------------------------------------------------------------*/

