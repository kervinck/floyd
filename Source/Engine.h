
/*----------------------------------------------------------------------+
 |                                                                      |
 |      Engine.h                                                        |
 |                                                                      |
 +----------------------------------------------------------------------*/

/*----------------------------------------------------------------------+
 |      Definitions                                                     |
 +----------------------------------------------------------------------*/

#define maxDepth 120

typedef struct Engine *Engine_t;

/*
 *  Transposition table
 */

#define ttDepthBits 8
#define ttDateBits 12

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
                        unsigned isHardBound    : 1; // Game theoretical result, regardless of depth
                        unsigned isWinLossScore : 1; // DTZ or mate
                };
                uint64_t data; // For lockless hashing
        };
};

struct ttable {
        struct ttSlot *slots;
        size_t size;
        size_t mask;
        unsigned int now;
        uint64_t baseHash; // For fast clearing
};

/*
 *  Chess engine
 */
struct Engine {
        struct Board board;

        struct ttable tt;

        int rootPlyNumber;

        intList searchMoves; // root moves to search, empty means all

        volatile bool stopFlag;

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

        jmp_buf abortEnv;
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

