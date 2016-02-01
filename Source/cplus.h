
/*----------------------------------------------------------------------+
 |                                                                      |
 |      cplus.h -- a loose collection of small C extensions             |
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
 |      Definitions                                                     |
 +----------------------------------------------------------------------*/

// Safe definition of statement-like macros
#define Statement(...) do{ __VA_ARGS__ }while(0)
#define pass Statement()

#define quote(arg) #arg
#define quote2(arg) quote(arg)

#define arrayLen(a) ((int) (sizeof(a) / sizeof((a)[0])))

#if !defined(max)
 #define max(a, b) ((a) >= (b) ? (a) : (b))
#endif

#if !defined(min)
 #define min(a, b) ((a) <= (b) ? (a) : (b))
#endif

#define null      ((void*) 0)
#define constNull ((const void*) 0)

#define maxInt ((int)(~0U >> 1))
#define minInt (-maxInt - 1)

#define maxLong ((long)(~0UL >> 1))
#define minLong (-maxLong - 1L)

#define maxLongLong ((long long)(~0ULL >> 1))
#define minLongLong (-maxLongLong - 1LL)

#define ones(n) (~(~0ULL << (n)))
#define bit(i) (1ULL << (i))
#define bitTest(w,i) (((w) >> (i)) & 1)
#define bitIndex8(w) (((w) & 0xf0 ? 4 : 0) + ((w) & 0xcc ? 2 : 0) + ((w) & 0xaa ? 1 : 0))

#define unused(a) ((void)(a))

/*----------------------------------------------------------------------+
 |      Exceptions                                                      |
 +----------------------------------------------------------------------*/

struct xError {
        const char *format;
        const char *file;
        const char *function;
        int line;
        int argc;
        // struct xValue argv[];
};
typedef struct xError *err_t;

#define OK ((err_t) 0)
#define check(err) Statement( if ((err) != OK) goto cleanup; )

#define xRaise(msg) Statement(                  \
        static struct xError _static_err = {    \
                .format = (msg),                \
                .file = __FILE__,               \
                .function = __func__,           \
                .line = __LINE__,               \
                .argc = -1                      \
        };                                      \
        err = &_static_err;                     \
        goto cleanup;                           \
)
err_t freeErr(err_t err);

/*----------------------------------------------------------------------+
 |      Tuples and pairs                                                |
 +----------------------------------------------------------------------*/

#define Tuple(type, n) struct { type v[n]; }

#define Pair(type)              Tuple(type, 2)
typedef Pair(int)               intPair;

/*----------------------------------------------------------------------+
 |      Lists                                                           |
 +----------------------------------------------------------------------*/

#define List(type)\
struct {\
        type *v;\
        int len;\
        int maxLen;\
}

#define emptyList { null, 0, 0 }

typedef List(uint8_t)   uByteList;
typedef List(int8_t)    sByteList;
typedef List(char)      charList;
typedef List(short)     shortList;
typedef List(int)       intList;
typedef List(long)      longList;
typedef List(uint64_t)  uint64List;
typedef List(void)      voidList;

#define fastPushList(list, value)\
        ((list).v[(list).len++] = (value))

#define popList(list)\
        ((list).v[--(list).len])

#define pushList(list, value) Statement(\
        preparePushList(list, 1);                               \
        fastPushList(list, value);                              \
)

#define preparePushList(list, nrItems) Statement(\
        if ((list).len + (nrItems) > (list).maxLen) {           \
                err_t _err = listEnsureMaxLen(                  \
                        (voidList*)&(list), sizeof((list).v[0]),\
                        (list).len + (nrItems), 128);           \
                if (_err != OK)                                 \
                        errAbort(_err);                         \
        })

#define freeList(list) Statement(\
        if ((list).v) {                                         \
                free((list).v);                                 \
                (list).v = null;                                \
                (list).len = 0;                                 \
                (list).maxLen = 0;                              \
        })

err_t listEnsureMaxLen(voidList *list, int itemSize, int minLen, int minSize);

/*----------------------------------------------------------------------+
 |      Functions                                                       |
 +----------------------------------------------------------------------*/

double xTime(void);
char *stringCopy(char *s, const char *t);
int compareInt(const void *ap, const void *bp);
int readLine(void *fp, charList *lineBuffer);
uint64_t xorshift64star(uint64_t x);

/*----------------------------------------------------------------------+
 |      Main support                                                    |
 +----------------------------------------------------------------------*/

int errExitMain(err_t err);
void errAbort(err_t err);
void xAbort(int r, const char *function);

// Conditional abort
static inline void cAbort(int r, const char *function)
{
        if (r != 0) xAbort(r, function);
}

/*----------------------------------------------------------------------+
 |      Threads and alarms                                              |
 +----------------------------------------------------------------------*/

/*
 *  An xThread_t is either a Windows thread or a POSIX thread. To avoid
 *  exposing these systems' entire APIs, use a typedef. This works because
 *  on both systems it is safe to cast the handle to a pointer. To provide
 *  some type safety and prevent mishaps, use a dummy struct* instead of
 *  void* as handle. In the implementation cast to and from the actual type.
 */
typedef struct threadHandle *xThread_t;

typedef void thread_fn(void *data);
xThread_t createThread(thread_fn *function, void *data);
void joinThread(xThread_t thread);

/*
 *  An alarm is a thread that runs its main function with a delay,
 *  and which can be safely aborted while it is waiting to run.
 */
typedef struct alarmHandle *xAlarm_t;
xAlarm_t setAlarm(double delay, thread_fn *function, void *data);
void clearAlarm(xAlarm_t alarm);

/*----------------------------------------------------------------------+
 |                                                                      |
 +----------------------------------------------------------------------*/

