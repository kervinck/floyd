
/*----------------------------------------------------------------------+
 |                                                                      |
 |      uci.c                                                           |
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
#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// C extension
#include "cplus.h"

// Own interface
#include "Board.h"
#include "Engine.h"
#include "uci.h"

/*----------------------------------------------------------------------+
 |      Definitions                                                     |
 +----------------------------------------------------------------------*/

struct options {
        long Hash;
        bool ClearHash;
};
#define maxHash ((sizeof(size_t) > 4) ? 64 * 1024L : 1024L)

#define ms (1e-3)
#define MiB (1ULL << 20)

/*----------------------------------------------------------------------+
 |      Data                                                            |
 +----------------------------------------------------------------------*/

static const char helpMessage[] =
 #define X "\n"
 "This engine uses the Universal Chess Interface (UCI) protocol."
X"See https://marcelk.net/chess/uci.html for details."
X"Supported UCI commands are:"
X"  uci"
X"        Confirm UCI mode, show engine details and options."
X"  debug [ on | off ]"
X"        Enable/disable debug mode and show its status."
X"  setoption name <optionName> [ value <optionValue> ]"
X"        Set option. The new value becomes active with the next `isready'."
X"  isready"
X"        Activate any changed options and reply `readyok' when done."
X"  ucinewgame"
X"        A new game has started. (ignored)"
X"  position [ startpos | fen <fenField> ... ] [ moves <move> ... ]"
X"        Setup the position on the internal board and play out the sequence"
X"        of moves. In debug mode also show the resulting FEN and board."
X"  go [ <option> ... ]"
X"        Start searching from the current position within the constraints"
X"        given by the options, or until the `stop' command is received."
X"        Always show a final result using `bestmove'. (But: see `infinite')"
X"        Command options are:"
X"          searchmoves <move> ...  Only search these moves"
X"          ponder                  Start search in ponder mode"
X"          wtime <millis>          Time remaining on White's clock"
X"          btime <millis>          Time remaining on Black's clock"
X"          winc <millis>           White's increment after each move"
X"          binc <millis>           Black's increment after each move"
X"          movestogo <nrMoves>     Moves to go until next time control"
X"          depth <ply>             Search no deeper than <ply> halfmoves"
X"          nodes <nrNodes>         Search no more than <nrNodes> nodes"
X"          mate <nrMoves>          Search for a mate in <nrMoves> moves or less"
X"          movetime <millis>       Search no longer than this time"
X"          infinite                Postpone `bestmove' result until `stop'"
X"         (Note: In Floyd `ponder' and `infinite' behave the same.)"
X"  ponderhit"
X"        Opponent has played the ponder move. Continue searching in own time."
X"  stop"
X"        Immediately stop any active `go' command and show its `bestmove' result."
X"  quit"
X"        Terminate engine."
X
X"Extra commands:"
X"  help"
X"        Show this list of commands."
X"  eval"
X"        Show evaluation."
X"  bench [ movetime <millis> ] [ bestof <repeat> ]"
X"        Speed test using 40 standard positions. Default: movetime 333 bestof 3"
X"  moves [ depth <ply> ]"
X"        Move generation test. Default: depth 1"
X
X"Unknown commands and options are silently ignored, except in debug mode."
X;

/*----------------------------------------------------------------------+
 |      Functions                                                       |
 +----------------------------------------------------------------------*/

static xThread_t stopSearch(Engine_t self, xThread_t searchThread);
static xThread_t startSearch(Engine_t self);
static void uciBestMove(Engine_t self);

static void updateOptions(Engine_t self,
        struct options *options, const struct options *newOptions);

/*----------------------------------------------------------------------+
 |      _scanToken                                                      |
 +----------------------------------------------------------------------*/

// Token and optional value scanner
static int _scanToken(char **line, const char *format, void *value)
{
        char nextChar = 0;
        int n = 0;
        if (value)
                sscanf(*line, format, value, &nextChar, &n);
        else
                sscanf(*line, format, &nextChar, &n);
        if (!isspace(nextChar)) // expect space or newline to follow
                n = 0;
        *line += n;
        return n;
}
#define scanValue(tokens, value) _scanToken(&line, " " tokens "%c %n", value)
#define scan(tokens) scanValue(tokens, null)

// For skipping unknown commands or options
#define skipOneToken(type) Statement(\
        while (isspace(*line)) line++;        \
        int _n=0; char *_s=line;              \
        _scanToken(&line, "%*s%n%c %n", &_n); \
        if (_n && debug) printf("info string %s ignored (%.*s)\n", type, _n, _s); \
)
#define skipOtherTokens() Statement( while(*line != '\0') skipOneToken("Option"); )

/*----------------------------------------------------------------------+
 |      uciMain                                                         |
 +----------------------------------------------------------------------*/

void uciMain(Engine_t self)
{
        charList lineBuffer = emptyList;
        bool debug = false;
        struct options oldOptions = { .Hash = -1 };
        struct options newOptions = { .Hash = 128 };

        // Prepare threading
        xThread_t searchThread = null;

        // Process commands
        while (readLine(stdin, &lineBuffer) != 0) {
                char *line = lineBuffer.v;
                if (debug) printf("info string input %s", line);

                if (scan("uci"))
                        printf("id name Floyd "quote2(floydVersion)"\n"
                               "id author Marcel van Kervinck\n"
                               "option name Hash type spin default %ld min 0 max %ld\n"
                               "option name Clear Hash type button\n"
                               "option name Ponder type check default true\n"
                               "uciok\n",
                                newOptions.Hash, maxHash);

                else if (scan("debug")) {
                        if (scan("on")) debug = true;
                        else if (scan("off")) debug = false;
                        printf("debug %s\n", debug ? "on" : "off");
                }
                else if (scan("setoption")) {
                        if (scanValue("name %*[Hh]ash value %ld", &newOptions.Hash)) pass;
                        else if (scan("name %*[Pp]onder value true")) pass;
                        else if (scan("name %*[Pp]onder value false")) pass; // just ignore it
                        else if (scan("name %*[Cc]lear %*[Hh]ash")) newOptions.ClearHash = !oldOptions.ClearHash;
                }
                else if (scan("isready")) {
                        updateOptions(self, &oldOptions, &newOptions);
                        printf("readyok\n");
                }
                else if (scan("ucinewgame"))
                        pass;
                else if (scan("position")) {
                        searchThread = stopSearch(self, searchThread);

                        if (scan("startpos"))
                                setupBoard(board(self), startpos);
                        else if (scan("fen")) {
                                int n = setupBoard(board(self), line);
                                if (debug && n == 0) printf("info string Invalid position\n");
                                line += n;
                        }

                        if (scan("moves"))
                                for (int n=1; n>0; line+=n) {
                                        int moves[maxMoves], move;
                                        int nrMoves = generateMoves(board(self), moves);
                                        n = parseUciMove(board(self), line, moves, nrMoves, &move);
                                        if (n > 0 && move > 0) makeMove(board(self), move);
                                        else if (n > 0) { skipOneToken("Illegal move"); break; }
                                }

                        if (debug) { // dump FEN and board
                                char fen[maxFenSize];
                                boardToFen(board(self), fen);
                                printf("info string fen %s", fen);
                                for (int ix=0, next='/', rank=8; next!=' '; next=fen[ix++])
                                        if (next == '/')
                                                printf("\ninfo string %d", rank--);
                                        else if (isdigit(next))
                                                while (next-- > '0') printf("  .");
                                        else
                                                printf("  %c", next);
                                printf("\ninfo string    a  b  c  d  e  f  g  h\n");
                        }
                }
                else if (scan("go")) {
                        searchThread = stopSearch(self, searchThread);
                        updateOptions(self, &oldOptions, &newOptions);

                        self->infoFunction = uciSearchInfo;
                        self->infoData = self;
                        self->pondering = false;

                        self->target.depth = maxDepth;
                        self->target.nodeCount = maxLongLong;
                        self->searchMoves.len = 0;
                        long time = 0, inc = 0, btime = 0, binc = 0;
                        int movestogo = 0;
                        int mate = 0;
                        long movetime = 0;

                        while (*line != '\0')
                                if ((scan("ponder") && (self->pondering = true))
                                 || scanValue("wtime %ld", &time)  || scanValue("winc %ld", &inc)
                                 || scanValue("btime %ld", &btime) || scanValue("binc %ld", &binc)
                                 || scanValue("movestogo %d", &movestogo)
                                 || scanValue("depth %d",     &self->target.depth)
                                 || scanValue("nodes %lld",   &self->target.nodeCount)
                                 || scanValue("mate %d",      &mate)
                                 || scanValue("movetime %ld", &movetime)
                                 || (scan("infinite") && (self->pondering = true))) // as ponder
                                        pass;
                                else if (scan("searchmoves")) {
                                        int moves[maxMoves], move;
                                        int nrMoves = generateMoves(board(self), moves);
                                        int n = parseUciMove(board(self), line, moves, nrMoves, &move);
                                        while (n > 0) {
                                                if (move > 0) { pushList(self->searchMoves, move); line += n; }
                                                else skipOneToken("Illegal move");
                                                n = parseUciMove(board(self), line, moves, nrMoves, &move);
                                        }
                                } else skipOneToken("Option");

                        if (sideToMove(board(self)) == black)
                                time = btime, inc = binc;
                        setTimeTargets(self, time * ms, inc * ms, movestogo, movetime * ms);
                        self->target.scores.v[0] = minMate - 2 * min(0, mate); // for "mate -n"
                        self->target.scores.v[1] = maxMate - 2 * max(0, mate); // for "mate n"
                        searchThread = startSearch(self);
                }
                else if (scan("stop")) {
                        self->pondering = false;
                        searchThread = stopSearch(self, searchThread);
                }
                else if (scan("ponderhit")) {
                        if (self->pondering)
                                self->alarmHandle = setAlarm(self->target.maxTime, abortSearch, self);
                        self->pondering = false;
                }
                else if (scan("quit")) {
                        skipOtherTokens();
                        break; // leaving the readline loop
                }

                /*
                 *  Extra commands
                 */
                else if (scan("help"))
                        fputs(helpMessage, stdout);

                else if (scan("eval")) {
                        int score = evaluate(board(self));
                        printf("info score cp %.0f string intern %+d\n", round(score / 10.0), score);
                }
                else if (scan("bench")) {
                        updateOptions(self, &oldOptions, &newOptions);
                        int movetime = 333, bestof = 3;
                        scanValue("movetime %d", &movetime);
                        scanValue("bestof %d", &bestof);
                        uciBenchmark(self, movetime * ms, bestof);
                }
                else if (scan("moves")) {
                        int depth = 1;
                        scanValue("depth %d", &depth);
                        uciMoves(board(self), depth);
                }
                else
                        skipOneToken("Command");

                skipOtherTokens(); // For info when debugging
                fflush(stdout);
        }

        searchThread = stopSearch(self, searchThread);
        freeList(lineBuffer);
}

/*----------------------------------------------------------------------+
 |      updateOptions                                                   |
 +----------------------------------------------------------------------*/

static void updateOptions(Engine_t self,
        struct options *oldOptions, const struct options *newOptions)
{
        if (newOptions->Hash < 0 || newOptions->Hash != oldOptions->Hash)
                ttSetSize(self, max(0, newOptions->Hash) * MiB);
        if (newOptions->ClearHash != oldOptions->ClearHash)
                ttClearFast(self);
        *oldOptions = *newOptions;
}

/*----------------------------------------------------------------------+
 |      uciSearchInfo                                                   |
 +----------------------------------------------------------------------*/

void uciSearchInfo(void *uciInfoData)
{
        Engine_t self = uciInfoData;
        charList infoLine = emptyList; // Construct the info line in a thread-safe manner

        long milliSeconds = round(self->seconds / ms);
        listPrintf(&infoLine, "info time %ld", milliSeconds);

        if (self->pv.len > 0 || self->depth == 0) {
                listPrintf(&infoLine, " depth %d score ", self->depth);
                if (isMateScore(self->score))
                        listPrintf(&infoLine, "mate %d",
                                (self->score < 0) ? (minMate - self->score    ) / 2
                                                  : (maxMate - self->score + 1) / 2);
                else
                        listPrintf(&infoLine, "cp %.0f", round(self->score / 10.0));
        }

        double nps = (self->seconds > 0.0) ? self->nodeCount / self->seconds : 0.0;
        listPrintf(&infoLine, " nodes %lld nps %.0f", self->nodeCount, nps);

        double ttLoad = ttCalcLoad(self);
        listPrintf(&infoLine, " hashfull %d", (int) round(ttLoad * 1000.0));

        for (int i=0; i<self->pv.len; i++) {
                char moveString[maxMoveSize];
                moveToUci(moveString, self->pv.v[i]);
                listPrintf(&infoLine, "%s %s", (i == 0) ? " pv" : "", moveString);
        }

        puts(infoLine.v); // Should be atomic and adds a newline
        if (self->seconds >= 0.1)
                fflush(stdout);
        freeList(infoLine);
}

/*----------------------------------------------------------------------+
 |      uciBestMove                                                     |
 +----------------------------------------------------------------------*/

static void uciBestMove(Engine_t self)
{
        char moveString[maxMoveSize];

        if (self->bestMove) {
                moveToUci(moveString, self->bestMove);
                printf("bestmove %s", moveString);
        } else
                printf("bestmove 0000"); // When in doubt, do as Shredder

        if (self->ponderMove) {
                moveToUci(moveString, self->ponderMove);
                printf(" ponder %s", moveString);
        }
        putchar('\n');
        fflush(stdout);
}

/*----------------------------------------------------------------------+
 |      startSearch / stopSearch                                        |
 +----------------------------------------------------------------------*/

static void searchThreadStart(void *args)
{
        Engine_t self = args;
        rootSearch(self);
        while (self->pondering)
                pass; // TODO: change into a semaphore one day
        uciBestMove(self);
}

static xThread_t startSearch(Engine_t args)
{
        return createThread(searchThreadStart, args);
}

static xThread_t stopSearch(Engine_t self, xThread_t searchThread)
{
        if (searchThread != null) {
                self->pondering = false;
                abortSearch(self);
                joinThread(searchThread);
        }
        return null;
}

/*----------------------------------------------------------------------+
 |                                                                      |
 +----------------------------------------------------------------------*/

