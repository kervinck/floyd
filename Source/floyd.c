
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
               "All rights reserved\n\n");

        setupBoard(&engine.board, startpos); // allow `go' without `position'

        uciMain(&engine);

        return 0;
}

