
/*----------------------------------------------------------------------+
 |                                                                      |
 |      cplus.c - a loose collection of small C extensions              |
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

#include <errno.h>
#include <math.h>
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

err_t err_free(err_t err)
{
        if (err->argc >= 0) {
#if 0
                for (int i=0; i<=err->argc; i++) {
                        xUnlink(err->argv[i]);
                }
#endif
                free(err);
        }
        return OK;
}

/*----------------------------------------------------------------------+
 |      Main support                                                    |
 +----------------------------------------------------------------------*/

int errExitMain(err_t err)
{
        if (err == OK) {
                return 0;
        } else {
                (void) fprintf(stderr, "[%s:%s:%d] Error: %s\n",
                        err->file,
                        err->function,
                        err->line,
                        err->format);

                (void) err_free(err);

                return EXIT_FAILURE;
        }
}

void errAbort(err_t err)
{
        (void) errExitMain(err);
        abort();
}

void xAbort(int r, const char *function)
{
        fprintf(stderr, "*** System error: %s failed (%s)\n", function, strerror(r));
        abort();
}

/*----------------------------------------------------------------------+
 |      Lists                                                           |
 +----------------------------------------------------------------------*/

/*
 *  @Assumption: Type** can be cast to void** and then dereferenced
 *
 *  -- `newLen' should prefarably to a compile-time constant
 *  *v       the list data
 *  *maxLen  the current allocated length (list elements, not bytes)
 *  minLen   the requested minimum length (list elements, not bytes)
 *  unit     size of a list elements
 *  newLen   starting size for buffer (list elements, not bytes)
 */
err_t list_ensure_len(void **v, int *maxLen, int minLen, int unit, int newLen)
{
        err_t err = OK;

        if ((*maxLen == 0) && (*v != null)) {
                xRaise("Invalid operation on fixed-length list");
        }

        while (newLen < minLen) {
                newLen *= 2;
        }
        if (newLen != *maxLen) {
                void *newv = null;
                if (newLen > 0) {
                        newv = realloc(*v, newLen * unit);
                        if (newv == null) xRaise("Out of memory");
                } else {
                        free(*v);
                        newv = null;
                }
                *v = newv;
                *maxLen = newLen;
        }
cleanup:
        return err;
}

/*----------------------------------------------------------------------+
 |      xclock                                                          |
 +----------------------------------------------------------------------*/

/*
 *  Get clock in seconds
 */
double xclock(void)
{
#if defined(WIN32)
        struct _timeb t;
        _ftime(&t);
        return t.time + t.millitm * 1e-3;
#endif
#if defined(POSIX)
        // TODO: why are we using the CPU time and not wall time...
        struct rusage ru;

        int r = getrusage(RUSAGE_SELF, &ru);
        if (r)
                return -1.0;

        double clock =
                (ru.ru_utime.tv_usec + ru.ru_stime.tv_usec) / 1.0e6 +
                (ru.ru_utime.tv_sec  + ru.ru_stime.tv_sec);

        return clock;
#endif
}

/*----------------------------------------------------------------------+
 |      stringCopy                                                      |
 +----------------------------------------------------------------------*/

char *stringCopy(char *s, const char *t)
{
        while ((*s = *t)) {
                s++;
                t++;
        }
        return s; // give pointer to terminating zero for easy concatenation
}

/*----------------------------------------------------------------------+
 |      readline                                                        |
 +----------------------------------------------------------------------*/

int readLine(void *fpPointer, char **pLine, int *pSize)
{
        FILE *fp = fpPointer;
        char *line = *pLine;
        int size = *pSize;
        int len = 0;

        for (;;) {
                /*
                 *  Ensure there is enough space for the next character and a terminator
                 */
                if (len + 1 >= size) {
                        int newsize = (size > 0) ? (2 * size) : 128;
                        char *newline = realloc(line, newsize);

                        if (newline == NULL) {
                                fprintf(stderr, "*** Error: %s\n", strerror(errno));
                                exit(EXIT_FAILURE);
                        }

                        line = newline;
                        size = newsize;
                }

                /*
                 *  Process next character from file
                 */
                int c = getc(fp);
                if (c == EOF) {
                        if (ferror(fp)) {
                                fprintf(stderr, "*** Error: %s\n", strerror(errno));
                                exit(EXIT_FAILURE);
                        } else {
                                break;
                        }
                }
                line[len++] = c;

                if (c == '\n') break; // End of line found
        }

        line[len] = '\0';
        *pLine = line;
        *pSize = size;

        return len;
}

/*----------------------------------------------------------------------+
 |      Windows implementation of threads and alarms                    |
 +----------------------------------------------------------------------*/
#if defined(_WIN32)

/*
 *  Threads
 */

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
        struct threadClosure *closure = malloc(sizeof(closure));
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

/*
 *  Alarms
 */

struct alarmHandle {
        double delay;
        thread_fn *function;
        void *data;
        HANDLE thread;
};

static unsigned int __stdcall alarmThreadStart(void *argsPointer)
{
        struct alarmHandle *args = argsPointer;
        DWORD millis = ceil(args->delay * 1e3);
        Sleep(millis);
        args->function(args->data);
        return 0;
}

xAlarm_t setAlarm(double delay, thread_fn *function, void *data)
{
        struct alarmHandle *alarm = malloc(sizeof(*alarm));
        if (!alarm) xAbort(errno, "malloc");

        alarm->delay = delay;
        alarm->function = function;
        alarm->data = data;

        alarm->thread = (HANDLE) _beginthreadex(
                null, 0, alarmThreadStart, alarm, 0, null);

        return alarm;
}

void clearAlarm(xAlarm_t alarm)
{
        TerminateThread(alarm->thread, 0);
        WaitForSingleObject(alarm->thread, INFINITE);
        CloseHandle(alarm->thread);
        free(alarm);
}
#endif

/*----------------------------------------------------------------------+
 |      POSIX implementation of threads and alarms                      |
 +----------------------------------------------------------------------*/
#if defined(POSIX)

/*
 *  Threads
 */

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
        struct threadClosure *closure = malloc(sizeof(closure));
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

/*
 *  Alarms
 */

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

        struct timeval now;
        r = gettimeofday(&now, null); // Mac doesn't have clock_gettime
        cAbort(r, "gettimeofday");

        const long giga = 1e9;
        long nsec = now.tv_usec * 1000 + (long) round(fmod(delay, 1.0) * giga);
        long sec = now.tv_sec + (long) delay;
        alarm->abstime.tv_nsec = nsec % giga;
        alarm->abstime.tv_sec = sec + (nsec / giga);

        alarm->abort = false;

        alarm->function = function;
        alarm->data = data;

        r = pthread_create(&alarm->thread, null, alarmThreadStart, alarm);
        cAbort(r, "pthread_create");

        return alarm;
}

void clearAlarm(xAlarm_t alarm)
{
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

