
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
#include <ctype.h>
#include <math.h>
#include <setjmp.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// C extension
#include "cplus.h"

// Own interface
#include "Board.h"
#include "Engine.h"
#include "uci.h"

/*----------------------------------------------------------------------+
 |      Functions                                                       |
 +----------------------------------------------------------------------*/

/*----------------------------------------------------------------------+
 |      uciMain                                                         |
 +----------------------------------------------------------------------*/

void uciMain(Engine_t engine)
{
        char *line = null;
        int size = 0;

        while (fflush(stdout), readLine(stdin, &line, &size) > 0) {
                char command[15+1];
                int n;

                if (sscanf(line, " %15s %n", command, &n) != 1)
                        continue;

                if (!strcmp(command, "uci")) {
                        printf("id name Floyd " quote2(floydVersion) "\n"
                               "id author Marcel van Kervinck\n"
                               "option name Hash type spin default 0 min 0 max 0\n"
                               //"option name Clear Hash type button\n"
                               //"option name Contempt type spin default 0 min -100 max 100\n"
                               "uciok\n"
                        );
                        continue;
                }

                if (!strcmp(command, "isready")) {
                        printf("readyok\n");
                        continue;
                }

                if (!strcmp(command, "setoption"))
                        continue;

                if (!strcmp(command, "ucinewgame"))
                        continue;

                if (!strcmp(command, "position")) {
                        char dummy;
                        int m = 0;

                        if (sscanf(line + n, "startpos%c%n", &dummy, &m) == 1 && isspace(dummy)) {
                                setupBoard(board(engine), startpos);
                        }
                        else if (sscanf(line + n, "fen %c%n", &dummy, &m) == 1) {
                                n += m;
                                m = setupBoard(board(engine), line + n);
                        }

                        if (m == 0) {
                                printf("info string Invalid position\n");
                                continue;
                        }
                        n += m;

                        if (sscanf(line + n, " moves %n%c", &m, &dummy) <= 0)
                                continue;
                        n += m;

                        while (sscanf(line + n, " %c", &dummy) == 1) { // game moves after 'moves'
                                int moves[maxMoves];
                                int nrMoves = generateMoves(board(engine), moves);
                                int move;
                                m = parseMove(board(engine), line + n, moves, nrMoves, &move);
                                switch (m) {
                                case 0:
                                        printf("info string Invalid move syntax\n");
                                        break;
                                case -1:
                                        printf("info string Illegal move\n");
                                        break;
                                case -2:
                                        printf("info string Ambiguous move\n");
                                        break;
                                default:
                                        makeMove(board(engine), move);
                                        n += m;
                                        continue; // next move
                                }
                                break;
                        }
                        continue;
                }

                if (!strcmp(command, "go")) {
                        int depth = maxDepth;
                        double movetime = 1.0;
                        rootSearch(engine, depth, movetime, uciInfo, engine);
                        char moveString[maxMoveSize];
                        moveToUci(board(engine), moveString, engine->bestMove);
                        printf("bestmove %s\n", moveString);
                        continue;
                }

                if (!strcmp(command, "quit"))
                        break;

                printf("info string No such command (%s)\n", command);
        }

        free(line);
        line = null;
}

/*----------------------------------------------------------------------+
 |      uciInfo                                                         |
 +----------------------------------------------------------------------*/

bool uciInfo(void *uciInfoData)
{
        Engine_t self = uciInfoData;

        long milliSeconds = round(self->seconds * 1e3);

        printf("info time %ld depth %d", milliSeconds, self->depth);

        if (self->pv.len > 0 || self->depth == 0) {
                char scoreString[16];
                if (abs(self->score < 31000))
                        sprintf(scoreString, "cp %d", (int) round(self->score / 10.0));
                else
                        sprintf(scoreString, "mate %d",
                                ((self->score < 0) ? 32000 + self->score : 32000 - self->score + 1) / 2);
                printf(" score %s", scoreString);
        }

        printf(" nodes %lld nps %.f",
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

        if (milliSeconds >= 100)
                fflush(stdout);

        return false;
}

/*----------------------------------------------------------------------+
 |                                                                      |
 +----------------------------------------------------------------------*/

