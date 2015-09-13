
/*----------------------------------------------------------------------+
 |                                                                      |
 |      cplus.c                                                         |
 |                                                                      |
 +----------------------------------------------------------------------*/

#include <stdlib.h>
#include <stdio.h>

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
 |                                                                      |
 +----------------------------------------------------------------------*/

