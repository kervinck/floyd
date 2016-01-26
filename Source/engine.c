
/*----------------------------------------------------------------------+
 |                                                                      |
 |      engine.c - Engine object chores                                 |
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

// C standard
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// C extension
#include "cplus.h"

// Own interface
#include "Board.h"
#include "Engine.h"

/*----------------------------------------------------------------------+
 |      Functions                                                       |
 +----------------------------------------------------------------------*/

void initEngine(Engine_t self)
{
        memset(self, 0, sizeof(struct Engine));
}

void cleanupEngine(Engine_t self)
{
        freeList(self->board.hashHistory);
        freeList(self->board.pkHashHistory);
        freeList(self->board.undoStack);
        freeList(self->searchMoves);
        freeList(self->pv);
        freeList(self->killers);
        free(self->tt.slots);
}

/*----------------------------------------------------------------------+
 |                                                                      |
 +----------------------------------------------------------------------*/

