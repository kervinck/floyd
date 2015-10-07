
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
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "cplus.h"

#if defined(_WIN32)
 #include <windows.h>
 #include <process.h>
 #include <sys/timeb.h>
#endif

#if defined(POSIX)
 #include <pthread.h>
 #include <unistd.h>
#endif

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

int xExitMain(err_t err)
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

void xAbort(err_t err)
{
        (void) xExitMain(err);
        abort();
}

void systemFailure(const char *function, int r)
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
        if (r != 0)
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
 |                                                                      |
 +----------------------------------------------------------------------*/

#if defined(_WIN32)

static unsigned int __stdcall alarmThreadEntry(void *argsPointer)
{
        struct alarm *args = argsPointer;
        DWORD millis = ceil(args->alarmTime * 1e3);
        Sleep(millis);
        args->alarmFunction(args->alarmData);
        return 0;
}

xthread_t setAlarm(struct alarm *alarm)
{
        if (!alarm)
                return null;

        HANDLE threadHandle = (HANDLE)_beginthreadex(
                null,
                0,
                alarmThreadEntry,
                (void*)alarm,
                0,
                null);
        return (xthread_t)threadHandle;
}

void clearAlarm(xthread_t alarm)
{
        if (alarm) {
                HANDLE threadHandle = (HANDLE)alarm;
                TerminateThread(threadHandle, 0);
                WaitForSingleObject(threadHandle, INFINITE);
                CloseHandle(threadHandle);
        }
}

static unsigned int __stdcall threadEntry(void *argsPointer)
{
        struct thread *args = argsPointer;
        args->threadFunction(args->threadData);
        return 0;
}

xthread_t createThread(struct thread *thread)
{
        HANDLE threadHandle = (HANDLE)_beginthreadex(
                null,
                0,
                threadEntry,
                thread,
                0,
                null);
        return threadHandle;
}

void joinThread(xthread_t thread)
{
        HANDLE threadHandle = (HANDLE)thread;

        WaitForSingleObject(threadHandle, INFINITE);
        CloseHandle(threadHandle);
}

#endif

#if defined(POSIX)

static void *alarmThreadEntry(void *argsPointer)
{
        struct alarm *args = argsPointer;
        int r = usleep((useconds_t) (args->alarmTime * 1e6));
        if (r != 0)
                systemFailure("usleep", r);
        args->alarmFunction(args->alarmData);
        return null;
}

xthread_t setAlarm(struct alarm *alarm)
{
        if (!alarm)
                return null;

        pthread_t threadHandle;
        int r = pthread_create(&threadHandle, null, alarmThreadEntry, alarm);
        if (r != 0)
                systemFailure("pthread_create", r);

        return (xthread_t) threadHandle;
}

void clearAlarm(xthread_t alarm)
{
        if (alarm) {
                pthread_t threadHandle = (pthread_t) alarm;
                int r = pthread_join(threadHandle, null);
                if (r != 0)
                        systemFailure("pthread_join", r);
        }
}

static void *threadEntry(void *argsPointer)
{
        struct thread *args = argsPointer;
        args->threadFunction(args->threadData);
        return null;
}

xthread_t createThread(struct thread *thread)
{
        pthread_t threadHandle;
        int r = pthread_create(&threadHandle, null, threadEntry, thread);
        if (r != 0)
                systemFailure("pthread_create", r);
        return (xthread_t) threadHandle;
}

void joinThread(xthread_t thread)
{
        pthread_t threadHandle = (pthread_t) thread;
        int r = pthread_join(threadHandle, null);
        if (r != 0)
                systemFailure("pthread_join", r);
}

#endif

/*----------------------------------------------------------------------+
 |                                                                      |
 +----------------------------------------------------------------------*/

