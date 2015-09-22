
/*----------------------------------------------------------------------+
 |                                                                      |
 |      Engine.h                                                        |
 |                                                                      |
 +----------------------------------------------------------------------*/

/*----------------------------------------------------------------------+
 |      Definitions                                                     |
 +----------------------------------------------------------------------*/

typedef struct engine *Engine_t;

/*
 *  Transposition table
 */

struct ttSlot {
        uint64_t hash;
        short loScore, hiScore;
        short move;
        unsigned short prio;
};

#define ttPrio(now, depth) ((now << 8) + depth)
#define ttDepth(prio) ((prio) & 0xff)

struct ttable {
        struct ttSlot *slots;
        size_t allocSize;
        size_t mask;
};

/*
 *  Chess engine
 */

struct engine {
        struct board board;

        struct ttable tt;

        // last search result
        int score;
        int depth;
        intList pv;

        long milliSeconds;
        long long nodeCount;
};

/*----------------------------------------------------------------------+
 |      Functions                                                       |
 +----------------------------------------------------------------------*/

// callback interface for handling search progress
typedef bool searchInfo_fn(void *infoData, int depth, long long nodes, int score);

int rootSearch(Board_t self, int depth, searchInfo_fn *infoFunction, void *infoData);

/*----------------------------------------------------------------------+
 |                                                                      |
 +----------------------------------------------------------------------*/

