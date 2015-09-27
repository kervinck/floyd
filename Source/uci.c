
/*----------------------------------------------------------------------+
 |                                                                      |
 |      uci.c                                                           |
 |                                                                      |
 +----------------------------------------------------------------------*/

/*----------------------------------------------------------------------+
 |      Includes                                                        |
 +----------------------------------------------------------------------*/

// C standard
#include <assert.h>
#include <math.h>
#include <setjmp.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

// C extension
#include "cplus.h"

// Own interface
#include "Board.h"
#include "Engine.h"
#include "uci.h"

/*----------------------------------------------------------------------+
 |      Definitions                                                     |
 +----------------------------------------------------------------------*/

/*----------------------------------------------------------------------+
 |      Data                                                            |
 +----------------------------------------------------------------------*/

/*----------------------------------------------------------------------+
 |      Functions                                                       |
 +----------------------------------------------------------------------*/

/*----------------------------------------------------------------------+
 |      uciInfo                                                         |
 +----------------------------------------------------------------------*/

bool uciInfo(void *uciInfoData)
{
        Engine_t self = uciInfoData;

        int milliSeconds = round(self->seconds * 1e3);

        char scoreString[16];
        if (abs(self->score < 31000))
                sprintf(scoreString, "cp %d", (int) round(self->score / 10.0));
        else
                sprintf(scoreString, "mate %d",
                        ((self->score < 0) ? 32000 + self->score : 32000 - self->score + 1) / 2);

        printf("info time %d depth %d score %s nodes %lld nps %.f",
                milliSeconds,
                self->depth,
                scoreString,
                self->nodeCount,
                (self->seconds > 0.0) ? self->nodeCount / self->seconds : 0.0);

        for (int i=0; i<self->pv.len; i++) {
                char moveString[maxMoveSize];
                int move = self->pv.v[i];
                assert(move != 0);
                moveToUci(board(self), moveString, move);
                printf("%s %s", (i == 0) ? " pv" : "", moveString);
                makeMove(board(self), move); // TODO: uci notation shouldn't need this
        }

        for (int i=0; i<self->pv.len; i++)
                undoMove(board(self));

        putchar('\n');

        if (milliSeconds >= 1000)
                fflush(stdout);

        return false;
}

/*----------------------------------------------------------------------+
 |                                                                      |
 +----------------------------------------------------------------------*/

