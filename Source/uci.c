
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

// System
#include <pthread.h>

// Own interface
#include "Board.h"
#include "Engine.h"
#include "uci.h"

// Other modules
#include "evaluate.h"

/*----------------------------------------------------------------------+
 |      Definitions                                                     |
 +----------------------------------------------------------------------*/

#define ms (1e-3)

struct searchArgs {
        Engine_t self;
        int depth;
        double targetTime;
        double alarmTime;
        searchInfo_fn *infoFunction;
        void *infoData;
        bool ponder;
        bool infinite;
};

/*----------------------------------------------------------------------+
 |      Data                                                            |
 +----------------------------------------------------------------------*/

#define X "\n"

static const char helpMessage[] =
 "This engine uses the Universal Chess Interface (UCI) protocol."
X"See http://download.shredderchess.com/div/uci.zip for details."
X
X"Supported UCI commands are:"
X"  uci"
X"        Confirm UCI mode, show engine details and options."
X"  debug [ on | off ]"
X"        Enable/disable debug mode and show its status."
X"  setoption name <optionName> [ value <optionValue> ]"
X"        Set option. The new value becomes active with the next `isready'."
X"  isready"
X"        Activate any changed options and reply 'isready' when done."
X"  ucinewgame"
X"        A new game has started (ignored)."
X"  position [ startpos | fen <fenField> ... ] [ moves <move> ... ]"
X"        Setup the position on the internal board and play out the sequence"
X"        of moves. In debug mode also show the resulting FEN and board."
X"  go [ <option> ... ]"
X"        Start searching from the current position within the constraints"
X"        given by the options, or until the `stop' command is received."
X"        Always show a final result using `bestmove'. (But: see `infinite')"
X"        Command options are:"
X"          searchmoves <move> ...  Only search these moves"
X"          ponder                  Start search in ponder mode" // TODO: not implemented
X"          wtime <millis>          Time remaining on White's clock"
X"          btime <millis>          Time remaining on Black's clock"
X"          winc <millis>           White's increment after each move"
X"          binc <millis>           Black's increment after each move"
X"          movestogo <nrMoves>     Moves to go until next time control"
X"          depth <ply>             Search no deeper than <ply> halfmoves"
X"          nodes <nrNodes>         Search no more than <nrNodes> nodes" // TODO: not implemented
X"          mate <nrMoves>          Search for a mate in <nrMoves> moves" // TODO: not implemented
X"          movetime <millis>       Search no longer than this time"
X"          infinite                Postpone `bestmove' result until `stop'"
X"  ponderhit"
X"        Opponent has played the ponder move. Continue searching in own time."
X"  stop"
X"        Stop any search. In `infinite' mode also show the `bestmove' result."
X"  quit"
X"        Terminate engine."
X
X"Extra commands:"
X"  help"
X"        Show this list of commands."
X"  eval"
X"        Show evaluation."
X"  bench"
X"        Run a standardized speed test." // TODO: not implemented
X"  moves [ depth <ply> ]"
X"        Run a move generation test." // TODO: not implemented
X
X"Unknown commands and options are silently ignored, except in debug mode."
X;

/*----------------------------------------------------------------------+
 |      Functions                                                       |
 +----------------------------------------------------------------------*/

// Calculate target time (or alarm time) for thinking on game clock
static double target(Engine_t self, double time, double inc, int movestogo);

static pthread_t startSearch(struct searchArgs *args);
static pthread_t stopSearch(pthread_t searchThread, struct searchArgs *args);

static void uciBestMove(Engine_t self);

static void systemFailure(const char *function, int r);


/*----------------------------------------------------------------------+
 |      uciMain                                                         |
 +----------------------------------------------------------------------*/

void uciMain(Engine_t self)
{
        char *line = null;
        int size = 0;
        bool debug = false;

        // Prepare threading
        pthread_t searchThread = null;
        struct searchArgs args;

        // Process commands from stdin
        while (fflush(stdout), readLine(stdin, &line, &size) > 0) {
                char command[15+1];
                int n, m = 0;
                char dummy;

                if (debug)
                        printf("info string input: %s", line);

                if (sscanf(line, " %15s %n", command, &n) != 1)
                        continue;

                if (strcmp(command, "uci") == 0) {
                        printf("id name Floyd " quote2(floydVersion) "\n"
                               "id author Marcel van Kervinck\n"
                               "option name Hash type spin default 0 min 0 max 0\n"
                               "option name Clear Hash type button\n"
                               //"option name Threads type spin default 1 min 1 max 1\n"
                               //"option name Ponder type check default false\n"
                               //"option name MultiPV type spin default 1 min 1 max 1\n"
                               //"option name UCI_Chess960 type check default false\n"
                               //"option name Contempt type spin default 0 min -100 max 100\n"
                               "uciok\n");
                        continue;
                }

                if (strcmp(command, "debug") == 0) {
                        if (sscanf(line+n, "on%c",  &dummy) == 1 && isspace(dummy)) debug = true;
                        if (sscanf(line+n, "off%c", &dummy) == 1 && isspace(dummy)) debug = false;
                        printf("debug %s\n", debug ? "on" : "off");
                        continue;
                }

                if (strcmp(command, "setoption") == 0)
                        continue;

                if (strcmp(command, "isready") == 0) {
                        printf("readyok\n");
                        continue;
                }

                if (strcmp(command, "ucinewgame") == 0)
                        continue;

                if (strcmp(command, "position") == 0) {
                        searchThread = stopSearch(searchThread, &args);

                        if (sscanf(line+n, "startpos%c %n", &dummy, &m) == 1 && isspace(dummy))
                                setupBoard(board(self), startpos);
                        else if (sscanf(line+n, "fen%c %n", &dummy, &m) == 1 && isspace(dummy)) {
                                n += m;
                                m = setupBoard(board(self), line+n);
                                if (debug && m == 0)
                                        printf("info string Invalid position\n");
                        }
                        n += m;

                        if (sscanf(line+n, " moves%c %n", &dummy, &m) == 1 && isspace(dummy)) {
                                while (m > 0) {
                                        n += m;
                                        int moves[maxMoves], move;
                                        int nrMoves = generateMoves(board(self), moves);
                                        m = parseMove(board(self), line+n, moves, nrMoves, &move);
                                        if (m > 0) makeMove(board(self), move);
                                        if (debug && m == -1) printf("info string Illegal move\n");
                                        if (debug && m == -2) printf("info string Ambiguous move\n");
                                }
                        }

                        if (debug) { // dump FEN and board
                                char fen[maxFenSize];
                                boardToFen(board(self), fen);
                                printf("info string %s", fen);
                                for (int ix=0, next='/', rank=8; next!=' '; next=fen[ix++])
                                        if (next == '/')
                                                printf("\ninfo string %d", rank--);
                                        else if (isdigit(next))
                                                while (next-- > '0') printf("  .");
                                        else
                                                printf("  %c", next);
                                printf("\ninfo string    a  b  c  d  e  f  g  h\n");
                        }
                        continue;
                }

                if (strcmp(command, "go") == 0) {
                        searchThread = stopSearch(searchThread, &args);

                        args = (struct searchArgs) {
                                .self = self,
                                .depth = maxDepth,
                                .targetTime = 0.0,
                                .alarmTime = 0.0,
                                .infoFunction = uciSearchInfo,
                                .infoData = self,
                                .ponder = false,
                                .infinite = false,
                        };

                        self->searchMoves.len = 0; // TODO: not implemented
                        long time = 0;
                        long btime = 0;
                        long inc = 0;
                        long binc = 0;
                        int movestogo = 25; // TODO: migrate game timing logic out of uci.c
                        long long nodes = maxLongLong;
                        int mate = 0; // TODO: not implemented
                        long movetime = 0;

                        while (line[n] != '\0') {
                                if (sscanf(line+n, "searchmoves%c %n", &dummy, &m) == 1 && isspace(dummy))
                                        while (m > 0) {
                                                n += m;
                                                int moves[maxMoves], move;
                                                int nrMoves = generateMoves(board(self), moves);
                                                m = parseMove(board(self), line+n, moves, nrMoves, &move);
                                                if (m > 0) pushList(self->searchMoves, move);
                                                if (debug && m == -1) printf("info string Illegal move\n");
                                                if (debug && m == -2) printf("info string Ambiguous move\n");
                                        }
                                if ((sscanf(line+n, "ponder%c     %n", &dummy,    &m) == 1 && isspace(dummy) && (args.ponder = true))
                                 ||  sscanf(line+n, "wtime %ld    %n", &time,     &m) == 1
                                 ||  sscanf(line+n, "btime %ld    %n", &btime,    &m) == 1
                                 ||  sscanf(line+n, "winc %ld     %n", &inc,      &m) == 1
                                 ||  sscanf(line+n, "binc %ld     %n", &binc,     &m) == 1
                                 ||  sscanf(line+n, "movestogo %d %n", &movestogo, &m) == 1
                                 ||  sscanf(line+n, "depth %d     %n", &args.depth, &m) == 1
                                 ||  sscanf(line+n, "nodes %lld   %n", &nodes,    &m) == 1
                                 ||  sscanf(line+n, "mate %d      %n", &mate,     &m) == 1
                                 ||  sscanf(line+n, "movetime %ld %n", &movetime, &m) == 1
                                 || (sscanf(line+n, "infinite%c   %n", &dummy,    &m) == 1 && isspace(dummy) && (args.infinite = true))
                                 || (sscanf(line+n, "%*s%c        %n", &dummy,    &m) == 1 && isspace(dummy))) // unknowns
                                        n += m;
                        }

                        if (movetime > 0)
                                args.alarmTime = movetime * ms;
                        else if (time + inc + btime + binc > 0) {
                                if (sideToMove(board(self)) == black)
                                        time = btime, inc = binc;
                                int alarmtogo = min(movestogo, (board(self)->halfmoveClock <= 70) ? 3 : 1);
                                args.targetTime = target(self, time * ms, inc * ms, movestogo);
                                args.alarmTime  = target(self, time * ms, inc * ms, alarmtogo);
                        }

                        printf("info string targetTime %.3f alarmTime %.3f\n", args.targetTime, args.alarmTime); // TODO: remove once branching factor is ~2

                        searchThread = startSearch(&args);
                        continue;
                }

                if (strcmp(command, "stop") == 0) {
                        searchThread = stopSearch(searchThread, &args);
                        if (args.infinite)
                                uciBestMove(self);
                        continue;
                }

                if (strcmp(command, "ponderhit") == 0) // TODO: implement ponder
                        continue;

                if (strcmp(command, "quit") == 0)
                        break;

                /*
                 *  Extra commands
                 */

                if (strcmp(command, "help") == 0) {
                        fputs(helpMessage, stdout);
                        continue;
                }

                if (strcmp(command, "eval") == 0) {
                        int score = evaluate(board(self));
                        printf("info score cp %.0f string intern %+d\n", round(score / 10.0), score);
                        continue;
                }

                if (debug)
                        printf("info string No such command (%s)\n", command);
        }

        searchThread = stopSearch(searchThread, &args);

        free(line);
        line = null;
}

/*----------------------------------------------------------------------+
 |      target                                                          |
 +----------------------------------------------------------------------*/

static double target(Engine_t self, double time, double inc, int movestogo)
{
        switch (board(self)->halfmoveClock) {
        case 29: case 30: movestogo = min(movestogo, 5); break;
        case 49: case 50: movestogo = min(movestogo, 4); break;
        case 69: case 70: movestogo = min(movestogo, 3); break;
        case 89: case 90: movestogo = min(movestogo, 2); break;
        }

        double safety = 5.0;
        double target = (time + (movestogo - 1) * inc - safety) / movestogo;
        target = max(target, 0.05);

        return target;
}

/*----------------------------------------------------------------------+
 |      uciSearchInfo                                                   |
 +----------------------------------------------------------------------*/

bool uciSearchInfo(void *uciInfoData)
{
        Engine_t self = uciInfoData;

        long milliSeconds = round(self->seconds / ms);

        printf("info time %ld depth %d", milliSeconds, self->depth);

        if (self->pv.len > 0 || self->depth == 0) {
                char scoreString[16];
                if (abs(self->score < 31000))
                        sprintf(scoreString, "cp %.0f", round(self->score / 10.0));
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
 |      uciBestMove                                                     |
 +----------------------------------------------------------------------*/

static void uciBestMove(Engine_t self)
{
        char bestMoveString[maxMoveSize];
        if (self->bestMove)
                moveToUci(board(self), bestMoveString, self->bestMove);
        else
                strcpy(bestMoveString, "0000"); // When in doubt, do as Shredder
        if (self->ponderMove) {
                char ponderMoveString[maxMoveSize];
                moveToUci(board(self), ponderMoveString, self->ponderMove);
                printf("bestmove %s ponder %s\n", bestMoveString, ponderMoveString);
        } else
                printf("bestmove %s\n", bestMoveString);
}

/*----------------------------------------------------------------------+
 |      startSearch                                                     |
 +----------------------------------------------------------------------*/

// Helper for startSearch
static void *searchThreadEntry(void *argsPointer)
{
        struct searchArgs *args = argsPointer;

        rootSearch(args->self,
                   args->depth,
                   args->targetTime, args->alarmTime,
                   args->infoFunction, args->infoData);

        if (!args->infinite) {
                uciBestMove(args->self);
                fflush(stdout);
        }
        return null;
}

static pthread_t startSearch(struct searchArgs *args)
{
        args->self->stopFlag = false;

        pthread_t searchThread;
        int r = pthread_create(&searchThread, null, searchThreadEntry, args);
        if (r != 0)
                systemFailure("pthread_create", r);

        return searchThread;
}

/*----------------------------------------------------------------------+
 |      stopSearch                                                      |
 +----------------------------------------------------------------------*/

static pthread_t stopSearch(pthread_t searchThread, struct searchArgs *args)
{
        if (searchThread == null)
                return null;

        args->self->stopFlag = true;

        int r = pthread_join(searchThread, null);
        if (r != 0)
                systemFailure("pthread_join", r);

        return null;
}

/*----------------------------------------------------------------------+
 |      systemFailure                                                   |
 +----------------------------------------------------------------------*/

// TODO: move to cplus.c
static void systemFailure(const char *function, int r)
{
        fprintf(stderr, "*** System error: %s failed (%s)\n", function, strerror(r));
        exit(EXIT_FAILURE);
}

/*----------------------------------------------------------------------+
 |                                                                      |
 +----------------------------------------------------------------------*/

