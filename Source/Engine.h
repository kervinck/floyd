
/*----------------------------------------------------------------------+
 |                                                                      |
 |      Engine.h                                                        |
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
 |      Definitions                                                     |
 +----------------------------------------------------------------------*/

#define maxDepth 120
#define nrKillers 5
#define newKillerIndex 2

typedef struct Engine *Engine_t;

// Callback interface for handling of search progress
typedef bool searchInfo_fn(void *infoData, const char *string, ...);

/*
 *  Transposition table
 */

#define ttDepthBits 8
#define ttDateBits 12

enum {
        minMate = -32000, minEval = -29999, minDtz  = -31000,
        maxMate =  32000, maxEval =  29999, maxDtz  =  31000,
};
#define isDrawScore(score)     ((score) == 0)
#define isWinScore(score)      ((score) > maxEval)
#define isLossScore(score)     ((score) < minEval)
#define isMateWinScore(score)  ((score) >= maxDtz)
#define isMateLossScore(score) ((score) <= minDtz)
#define isMateScore(score)     (abs(score) >= maxDtz)

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
        intList searchMoves;    // root moves to search, empty means all
        volatile bool stopFlag; // aborts search when alarm time exceeded
        bool mateStop;          // stops the search once the shortest mate is found

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

        double targetTime;
        double abortTime;
        int targetDepth;

        searchInfo_fn *infoFunction;
        void *infoData;

        void *abortTarget;
};

/*
 *  Workaround to hide some of the ugliness, at least until ISO C supports
 *  seamless access to members of the base struct (like `-fms-extensions'
 *  or `kenc'). Note that this is a `nop' (just a type conversion) because
 *  `board' is the first element of `struct Engine'.
 */
#define board(engine) (&(engine)->board)

/*----------------------------------------------------------------------+
 |      Data                                                            |
 +----------------------------------------------------------------------*/

extern int globalVector[];
extern const int vectorLen;
extern const char * const vectorLabels[];

/*----------------------------------------------------------------------+
 |      Functions                                                       |
 +----------------------------------------------------------------------*/

/*
 *  Search
 */
void rootSearch(Engine_t self);
searchInfo_fn noInfoFunction;

/*
 *  Evaluate
 */
int evaluate(Board_t self);

/*
 *  Transposition table
 */

void ttSetSize(Engine_t self, size_t size);
int ttWrite(Engine_t self, struct ttSlot slot, int depth, int score, int alpha, int beta);
struct ttSlot ttRead(Engine_t self);
void ttClearFast(Engine_t self);
double ttCalcLoad(Engine_t self);

/*
 *  Time control
 */
void setTimeTargets(Engine_t self, double time, double inc, int movestogo, double movetime);

/*----------------------------------------------------------------------+
 |                                                                      |
 +----------------------------------------------------------------------*/

