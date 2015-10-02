
#include <stdbool.h>
#include <setjmp.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "cplus.h"

#include "Board.h"
#include "Engine.h"

#include "uci.h"

static struct engine engine;


int main(void)
{
        printf("\nFloyd Chess Program - Version " quote2(floydVersion) "\n"
               "Copyright (C)1998-2015 by Marcel van Kervinck\n"
               "All rights reserved\n"
               "\n"
               "Type \"help\" for more information, or \"quit\" to leave.\n\n");

        setupBoard(&engine.board, startpos); // be nice and allow `go' without `position'

        uciMain(&engine);

        // TODO: move this to a destructor
        freeList(engine.board.hashHistory);
        freeList(engine.searchMoves);
        freeList(engine.pv);

        return 0;
}

