
/*----------------------------------------------------------------------+
 |                                                                      |
 |      ttable.c - Hash table for caching search results                |
 |                                                                      |
 +----------------------------------------------------------------------*/

/*----------------------------------------------------------------------+
 |      Includes                                                        |
 +----------------------------------------------------------------------*/

// C standard
#include <assert.h>
#include <errno.h>
#include <setjmp.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <stdio.h>

// C extension
#include "cplus.h"

// Own interface
#include "Board.h"
#include "Engine.h"

/*----------------------------------------------------------------------+
 |      Definitions                                                     |
 +----------------------------------------------------------------------*/

// Needed for lockless hashing, plus a sanity check for proper bitfield packing.
_Static_assert(sizeof(struct ttSlot) == 2 * sizeof(uint64_t), "Unexpected size of struct ttSlot");

#define bucketLen 4 // must be power of 2

/*----------------------------------------------------------------------+
 |      Functions                                                       |
 +----------------------------------------------------------------------*/

static inline int prio(Engine_t self, int ix);

/*----------------------------------------------------------------------+
 |      ttSetSize                                                       |
 +----------------------------------------------------------------------*/

// Change table size. Size is given as bytes.
void ttSetSize(Engine_t self, size_t size)
{
        // Calculate largest new size (and mask) not exceeding the requested size
        size_t newSize = bucketLen * sizeof(struct ttSlot);
        size = max(size, newSize); // but allow no smaller than this
        size_t newMask = 0;
        for (; newSize<=size-newSize; newSize+=newSize)
                newMask = (newMask << 1) + bucketLen;

        // Shrink table contents
        if (newSize < self->tt.size)
                for (size_t i=0; i<self->tt.mask+bucketLen; i++)
                        if (prio(self, i & (newMask+bucketLen-1)) < prio(self, i))
                                self->tt.slots[i&(newMask+bucketLen-1)] = self->tt.slots[i];

        // (Re-)allocate memory, retry with smaller sizes until success
        struct ttSlot *newSlots = realloc(self->tt.slots, newSize);
        while (!newSlots && newMask > 0) {
                newSize >>= 1;
                newMask = (newMask & ~bucketLen) >> 1;
                newSlots = realloc(self->tt.slots, newSize);
        }
        if (!newSlots)
                systemFailure("realloc", errno);

        if (!self->tt.slots) // initial allocation is uninitialized
                memset(newSlots, 0, bucketLen * sizeof newSlots[0]);

        // Expand table contents
        if (newSize > self->tt.size)
                for (size_t i=0; i<newMask+bucketLen; i++)
                        newSlots[i] = newSlots[i&(self->tt.mask+bucketLen-1)];

        // Update
        self->tt.slots = newSlots;
        self->tt.size = newSize;
        self->tt.mask = newMask;
}

/*----------------------------------------------------------------------+
 |      ttWrite                                                         |
 +----------------------------------------------------------------------*/

int ttWrite(Engine_t self, struct ttSlot slot, int depth, int score, int alpha, int beta)
{
        /*
         *  In some cases, let the older result prevail to avoid information loss
         */

        if (slot.isHardBound)
                if ((slot.isLowerBound && score <= slot.score)
                 || (slot.isUpperBound && score >= slot.score))
                        return slot.score;

        /*
         *  Set fields
         */

        slot.score = score;
        slot.depth = depth;
        slot.date = self->tt.now & ones(ttDateBits);
        slot.isUpperBound = score <= alpha;
        slot.isLowerBound = score >= beta;
        slot.isHardBound = false;
        slot.isWinLossScore = false;

        /*
         *  Apply corrections for DTZ and mate scores
         */

        if (score >= 30000) {
                if (score > 30000) {
                        /*
                         *  Don't store DTZ values when halfmoveClock is 0, because such
                         *  entries wreck progress later in the game (after zeroing).
                         */
                        if (board(self)->halfmoveClock == 0 && score <= 31000)
                                return score;
                        slot.score += board(self)->plyNumber - self->rootPlyNumber;
                        slot.isWinLossScore = 1;
                        assert(slot.score < 32000);
                }
                slot.isHardBound = slot.isLowerBound;
        }

        if (score <= -30000) {
                if (score < -30000) {
                        if (board(self)->halfmoveClock == 0 && score >= -31000)
                                return score;
                        slot.score -= board(self)->plyNumber - self->rootPlyNumber;
                        slot.isWinLossScore = 1;
                        assert(slot.score >= -32000);
                }
                slot.isHardBound = slot.isUpperBound;
        }

        /*
         *  Find best slot `bucket+i' to store the search result in, either
         *   - the slot with the lowest (-age, depth)-priority, or
         *   - the last used slot, if still present.
         */

        size_t bucket = slot.key & self->tt.mask;
        int i = -1, iPrio = maxInt;
        for (int j=0; j<bucketLen; j++) {
                struct ttSlot local = self->tt.slots[bucket+j];
                if ((local.key ^ local.data) == slot.key) {
                        i = j;
                        break;
                }
                int jPrio = prio(self, bucket + j);
                if (jPrio < iPrio)
                        i = j, iPrio = jPrio;
        }
        assert(i >= 0);

        /*
         *  Write into table
         */

        slot.key ^= slot.data;
        self->tt.slots[bucket+i] = slot;

        return score;
}

/*----------------------------------------------------------------------+
 |      ttRead                                                          |
 +----------------------------------------------------------------------*/

struct ttSlot ttRead(Engine_t self)
{
        uint64_t hash = board(self)->hash ^ self->tt.baseHash;
        size_t bucket = hash & self->tt.mask;

        for (int i=0; i<bucketLen; i++) {
                struct ttSlot local = self->tt.slots[bucket+i];
                local.key ^= local.data;
                if (local.key == hash) { // Found
                        if (local.isWinLossScore) {
                                int rootDistance = board(self)->plyNumber - self->rootPlyNumber;
                                local.score += local.score >= 0 ? -rootDistance : rootDistance;
                        }
                        return local;
                }
        }
        // Not found
        return (struct ttSlot) { .key = hash, .data = 0 };
}

/*----------------------------------------------------------------------+
 |      ttCalcLoad                                                      |
 +----------------------------------------------------------------------*/

double ttCalcLoad(Engine_t self)
{
        int n = 0;
        int m = min(10000, self->tt.mask + bucketLen);
        int now = self->tt.now & ones(ttDateBits);

        for (int i=0; i<m; i++)
                if (self->tt.slots[i].date == now)
                        n++;
        return (double) n / (double) m;
}

/*----------------------------------------------------------------------+
 |      ttClearFast                                                     |
 +----------------------------------------------------------------------*/

/*
 *  Instead of going through the whole memory to invalidate all entries,
 *  modify the hash function to the same effect. (This only has an effect
 *  on the interface between search and ttable, not in Board.)
 */
void ttClearFast(Engine_t self)
{
        uint64_t x = ~self->tt.baseHash;

        // xorshift64star
        x ^= x >> 12;
        x ^= x << 25;
        x ^= x >> 27;
        x *= 2685821657736338717ULL;

        self->tt.baseHash = ~x;
}

/*----------------------------------------------------------------------+
 |      prio                                                            |
 +----------------------------------------------------------------------*/

// Priority for replacement scheme: (-age, depth). Higher is more important.
static inline int prio(Engine_t self, int ix)
{
        struct ttSlot *slot = &self->tt.slots[ix];
        int age = (self->tt.now - slot->date) & ones(ttDateBits);
        return (-age << ttDepthBits) + slot->depth;
}

/*----------------------------------------------------------------------+
 |                                                                      |
 +----------------------------------------------------------------------*/

