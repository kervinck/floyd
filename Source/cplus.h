
/*----------------------------------------------------------------------+
 |                                                                      |
 |      cplus.h -- a loose collection of small C extensions             |
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

// Safe definition of statement-like macros
#define Statement(...) do{ __VA_ARGS__ }while(0)
#define pass Statement()

#define quote(arg) #arg
#define quote2(arg) quote(arg)

#define arrayLen(a) (sizeof(a) / sizeof((a)[0]))

#if !defined(max)
 #define max(a, b) ((a) >= (b) ? (a) : (b))
#endif

#if !defined(min)
 #define min(a, b) ((a) <= (b) ? (a) : (b))
#endif

#define setMax(a, b) Statement( if ((a) < (b)) (a) = (b); )
#define setMin(a, b) Statement( if ((a) > (b)) (a) = (b); )

#define null      ((void*) 0)
#define constNull ((const void*) 0)

#define maxInt ((int)(~0U >> 1))
#define minInt (-maxInt - 1)

#define maxLong ((long)(~0UL >> 1))
#define minLong (-maxLong - 1L)

#define maxLongLong ((long long)(~0ULL >> 1))
#define minLongLong (-maxLongLong - 1LL)

#define ones(n) (~(~0ULL << (n)))

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

#define check(err) Statement(\
        if ((err) != OK)      \
                goto cleanup; \
)

err_t err_free(err_t err);

#define xRaise(msg) Statement(\
        static struct xError _static_err = {\
                .format = (msg),\
                .file = __FILE__,\
                .function = __func__,\
                .line = __LINE__,\
                .argc = -1\
        };\
        err = &_static_err;\
        goto cleanup;\
)

#define xAssert(cond) Statement(\
        if (!(cond))\
                xRaise("Assertion (" #cond ") failed");\
)

/*----------------------------------------------------------------------+
 |      Pairs / Tuples                                                  |
 +----------------------------------------------------------------------*/

#define Tuple(type, n)\
struct {\
        type v[n];\
}

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

double xTime(void);
char *stringCopy(char *s, const char *t);
int readLine(void *fp, char **pLine, int *pSize);

/*----------------------------------------------------------------------+
 |      Main support                                                    |
 +----------------------------------------------------------------------*/

int errExitMain(err_t err);
void errAbort(err_t err);
void xAbort(int r, const char *function);

// Conditional abort
static inline void cAbort(int r, const char *function)
{
        if (r != 0)
                xAbort(r, function);
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

