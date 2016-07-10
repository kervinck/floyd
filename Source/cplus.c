
/*----------------------------------------------------------------------+
 |                                                                      |
 |      cplus.c -- a loose collection of small C extensions             |
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

#define _XOPEN_SOURCE 600
#include <assert.h>
#include <errno.h>
#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#if defined(_WIN32)
 #include <windows.h>
 #include <process.h>
 #include <sys/timeb.h>
#elif defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__))
 #include <pthread.h>
 #include <sys/time.h>
 #include <unistd.h>
 #define POSIX
#endif

#include "cplus.h"

/*----------------------------------------------------------------------+
 |      Exceptions                                                      |
 +----------------------------------------------------------------------*/

err_t freeErr(err_t err)
{
        if (err->argc >= 0) {
                //for (int i=0; i<=err->argc; i++)
                        //xUnlink(err->argv[i]);
                free(err);
        }
        return OK;
}

/*----------------------------------------------------------------------+
 |      Lists                                                           |
 +----------------------------------------------------------------------*/

/*
 *  Ensure a mimimum capacity for a list before it needs resizing.
 *  The resulting size will be rounded up in exponential manner to
 *  amortize the costs of repeatedly adding single items.
 *      list:      Pointer to list object
 *      itemSize:  Item size in bytes
 *      minLen:    Minimum number of required items. Must be at least list->len
 *      minSize:   Minimum size in bytes of a non-empty list
 */
err_t listEnsureMaxLen(voidList *list, int itemSize, int minLen, int minSize)
{
        err_t err = OK;
        assert(minLen >= list->len);
        assert(minLen > 0);

        if ((list->maxLen == 0) && (list->v != null))
                xRaise("Invalid operation on fixed-length list");

        int newMax = max(1, (minSize + itemSize - 1) / itemSize);
        while (newMax < minLen)
                newMax *= 2; // TODO: make this robust for huge lists (overflows etc)

        if (newMax != list->maxLen) {
                void *v = realloc(list->v, newMax * itemSize);
                if (v == null) xRaise("Out of memory");
                list->v = v;
                list->maxLen = newMax;
        }
cleanup:
        return err;
}

/*
 *  Formatted printing into a char list.
 *  A trailing zero is written but not counted as part of list->len
 */
void listPrintf(charList *list, const char *format, ...)
{
        va_list args;
        va_start(args, format);
        int len = vsnprintf(null, 0, format, args);
        va_end(args);
        if (len == -1) xAbort(errno, "vsnprintf");
        preparePushList(*list, len+1); // include trailing '\0'
        va_start(args, format); // MUST redo this!
        vsprintf(&list->v[list->len], format, args);
        va_end(args);
        list->len += len;
}

/*----------------------------------------------------------------------+
 |      xTime                                                           |
 +----------------------------------------------------------------------*/

/*
 *  Get wall time in seconds and subseconds
 */

#if defined(WIN32)
double xTime(void)
{
        struct _timeb t;
        _ftime(&t);
        return t.time + t.millitm * 1e-3;
}
#endif

#if defined(POSIX)
double xTime(void)
{
        struct timeval tv;
        int r = gettimeofday(&tv, null);
        if (r == -1) xAbort(errno, "gettimeofday");
        return tv.tv_sec + tv.tv_usec * 1e-6;
}
#endif

/*----------------------------------------------------------------------+
 |      stringCopy                                                      |
 +----------------------------------------------------------------------*/

char *stringCopy(char *s, const char *t)
{
        while ((*s = *t))
                s++, t++;
        return s; // give pointer to terminating zero for easy concatenation
}

/*----------------------------------------------------------------------+
 |      compareInt                                                      |
 +----------------------------------------------------------------------*/

int compareInt(const void *ap, const void *bp)
{
        const int *a = ap, *b = bp;
        if (*a > *b) return 1;
        if (*a < *b) return -1;
        return 0;
}

/*----------------------------------------------------------------------+
 |      readLine                                                        |
 +----------------------------------------------------------------------*/

int readLine(void *fpPointer, charList *lineBuffer)
{
        FILE *fp = fpPointer;
        lineBuffer->len = 0;
        int c;

        do {
                c = getc(fp);
                if (c != EOF)        pushList(*lineBuffer, c);
                else if (ferror(fp)) xAbort(errno, "getc");
                else                 break;
        } while (c != '\n');

        pushList(*lineBuffer, '\0');
        return lineBuffer->len-1;
}

/*----------------------------------------------------------------------+
 |      xorshift64star                                                  |
 +----------------------------------------------------------------------*/

uint64_t xorshift64star(uint64_t x)
{
        x ^= x >> 12;
        x ^= x << 25;
        x ^= x >> 27;
        return x * 2685821657736338717ULL;
}

/*----------------------------------------------------------------------+
 |      Main support                                                    |
 +----------------------------------------------------------------------*/

int errExitMain(err_t err)
{
        if (err != OK) {
                fprintf(stderr, "Error: %s, file %s, function %s, line %d.\n",
                        err->format, err->file, err->function, err->line);
                freeErr(err);
                return EXIT_FAILURE;
        } else
                return 0;
}

void errAbort(err_t err)
{
        errExitMain(err);
        abort();
}

void xAbort(int r, const char *function)
{
        fprintf(stderr, "System error: %s failed (%s)\n", function, strerror(r));
        abort();
}

/*----------------------------------------------------------------------+
 |      Threads (Windows)                                               |
 +----------------------------------------------------------------------*/
#if defined(_WIN32)

struct threadClosure {
        thread_fn *function;
        void *data;
};

static unsigned int __stdcall threadStart(void *args)
{
        struct threadClosure closure = *(struct threadClosure*)args;
        free(args);
        closure.function(closure.data);
        return 0;
}

xThread_t createThread(thread_fn *function, void *data)
{
        // We can't pass these on the stack due to the race-condition
        struct threadClosure *closure = malloc(sizeof(*closure));
        if (!closure)
                xAbort(errno, "malloc");
        closure->function = function;
        closure->data = data;

        HANDLE threadHandle = (HANDLE) _beginthreadex(
                null, 0, threadStart, closure, 0, null);

        return threadHandle;
}

void joinThread(xThread_t thread)
{
        HANDLE threadHandle = (HANDLE) thread;
        WaitForSingleObject(threadHandle, INFINITE);
        CloseHandle(threadHandle);
}
#endif

/*----------------------------------------------------------------------+
 |      Threads (POSIX)                                                 |
 +----------------------------------------------------------------------*/
#if defined(POSIX)

struct threadClosure {
        thread_fn *function;
        void *data;
};

static void *threadStart(void *args)
{
        struct threadClosure closure = *(struct threadClosure*)args;
        free(args);
        closure.function(closure.data);
        return null;
}

xThread_t createThread(thread_fn *function, void *data)
{
        // We can't pass these on the stack due to the race-condition
        struct threadClosure *closure = malloc(sizeof(*closure));
        if (!closure)
                xAbort(errno, "malloc");
        closure->function = function;
        closure->data = data;

        pthread_t threadHandle;
        int r = pthread_create(&threadHandle, null, threadStart, closure);
        cAbort(r, "pthread_create");

        return (xThread_t) threadHandle;
}

void joinThread(xThread_t thread)
{
        pthread_t threadHandle = (pthread_t) thread;
        int r = pthread_join(threadHandle, null);
        cAbort(r, "pthread_join");
}
#endif

/*----------------------------------------------------------------------+
 |      Alarms (Windows)                                                |
 +----------------------------------------------------------------------*/
#if defined(_WIN32)

// TODO: check and abort on all error conditions of win32 functions

struct alarmHandle {
        double delay;
        thread_fn *function;
        void *data;
        HANDLE event;
        HANDLE thread;
};

static unsigned int __stdcall alarmThreadStart(void *args)
{
        struct alarmHandle *alarm = args;
        DWORD millis = ceil(alarm->delay * 1e3);
        DWORD r = WaitForSingleObject(alarm->event, millis);
        if (r == WAIT_TIMEOUT)
                alarm->function(alarm->data);
        return 0;
}

xAlarm_t setAlarm(double delay, thread_fn *function, void *data)
{
        struct alarmHandle *alarm = malloc(sizeof(*alarm));
        if (!alarm) xAbort(errno, "malloc");

        alarm->delay = delay;
        alarm->function = function;
        alarm->data = data;
        alarm->event = CreateEvent(null, true, false, null);
        alarm->thread = (HANDLE) _beginthreadex(
                null, 0, alarmThreadStart, alarm, 0, null);

        return alarm;
}

void clearAlarm(xAlarm_t alarm)
{
        if (alarm) {
                SetEvent(alarm->event);
                WaitForSingleObject(alarm->thread, INFINITE);
                CloseHandle(alarm->thread);
                CloseHandle(alarm->event);
                free(alarm);
        }
}
#endif

/*----------------------------------------------------------------------+
 |      Alarms (POSIX)                                                  |
 +----------------------------------------------------------------------*/
#if defined(POSIX)

struct alarmHandle {
        pthread_mutex_t mutex;
        pthread_cond_t cond;
        struct timespec abstime;
        bool abort;
        thread_fn *function;
        void *data;
        pthread_t thread;
};

static void *alarmThreadStart(void *args)
{
        struct alarmHandle *alarm = args;
        int r;

        r = pthread_mutex_lock(&alarm->mutex);
        cAbort(r, "pthread_mutex_lock");

        while (!alarm->abort) {
                r = pthread_cond_timedwait(&alarm->cond, &alarm->mutex, &alarm->abstime);
                if (r == ETIMEDOUT)
                        break;
                cAbort(r, "pthread_cond_timedwait");
        }

        r = pthread_mutex_unlock(&alarm->mutex);
        cAbort(r, "pthread_mutex_unlock");

        if (!alarm->abort)
                alarm->function(alarm->data);

        return null;
}

xAlarm_t setAlarm(double delay, thread_fn *function, void *data)
{
        struct alarmHandle *alarm = malloc(sizeof(*alarm));
        if (!alarm) xAbort(errno, "malloc");

        int r = pthread_mutex_init(&alarm->mutex, null);
        cAbort(r, "pthread_mutex_init");

        r = pthread_cond_init(&alarm->cond, null);
        cAbort(r, "pthread_cond_init");

        double fabstime = xTime() + delay;
        alarm->abstime.tv_sec = fabstime;
        alarm->abstime.tv_nsec = fmod(fabstime, 1.0) * 1e9;

        alarm->abort = false;

        alarm->function = function;
        alarm->data = data;

        r = pthread_create(&alarm->thread, null, alarmThreadStart, alarm);
        cAbort(r, "pthread_create");

        return alarm;
}

void clearAlarm(xAlarm_t alarm)
{
        if (alarm == null)
                return;

        /*
         *  Stop alarm thread if it is still waiting
         */

        int r = pthread_mutex_lock(&alarm->mutex);
        cAbort(r, "pthread_mutex_lock");

        alarm->abort = true;
        r = pthread_cond_signal(&alarm->cond);
        cAbort(r, "pthread_cond_signal");

        r = pthread_mutex_unlock(&alarm->mutex);
        cAbort(r, "pthread_mutex_unlock");

        /*
         *  Free resources
         */

        r = pthread_join(alarm->thread, null);
        cAbort(r, "pthread_join");

        r = pthread_mutex_destroy(&alarm->mutex);
        cAbort(r, "pthread_mutex_destroy");

        r = pthread_cond_destroy(&alarm->cond);
        cAbort(r, "pthread_cond_destroy");

        free(alarm);
}
#endif

/*----------------------------------------------------------------------+
 |                                                                      |
 +----------------------------------------------------------------------*/

