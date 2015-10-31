
/*----------------------------------------------------------------------+
 |                                                                      |
 |      uci.c                                                           |
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

// Other modules
#include "evaluate.h"

/*----------------------------------------------------------------------+
 |      Definitions                                                     |
 +----------------------------------------------------------------------*/

struct searchArgs {
        Engine_t self;
        bool ponder;
        bool infinite;
};

struct options {
        long Hash;
        bool Ponder;
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
X"        Activate any changed options and reply 'isready' when done."
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
X"          searchmoves <move> ...  Only search these moves // TODO: not implemented"
X"          ponder                  Start search in ponder mode // TODO: not implemented"
X"          wtime <millis>          Time remaining on White's clock"
X"          btime <millis>          Time remaining on Black's clock"
X"          winc <millis>           White's increment after each move"
X"          binc <millis>           Black's increment after each move"
X"          movestogo <nrMoves>     Moves to go until next time control"
X"          depth <ply>             Search no deeper than <ply> halfmoves"
X"          nodes <nrNodes>         Search no more than <nrNodes> nodes // TODO: not implemented"
X"          mate <nrMoves>          Search for a mate in <nrMoves> moves // TODO: not implemented"
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
X"  bench [ movetime <millis> ]"
X"        Speed test of 40 standard positions. Default is `movetime 1000'."
X"  moves [ depth <ply> ] // TODO: not implemented"
X"        Move generation test. Default `depth' is 1."
X
X"Unknown commands and options are silently ignored, except in debug mode."
X;

/*----------------------------------------------------------------------+
 |      Functions                                                       |
 +----------------------------------------------------------------------*/

static xThread_t stopSearch(Engine_t self, xThread_t searchThread);
static xThread_t startSearch(struct searchArgs *args);

static void uciBestMove(Engine_t self);

static void updateOptions(Engine_t self,
        struct options *options, const struct options *newOptions);

static searchInfo_fn benchInfoFunction;

/*----------------------------------------------------------------------+
 |      _scanToken                                                      |
 +----------------------------------------------------------------------*/

// Token and (optional) value scanner
static int _scanToken(char **line, const char *format, void *value)
{
        char next = 0;
        int n = 0;
        if (value)
                sscanf(*line, format, value, &next, &n);
        else
                sscanf(*line, format, &next, &n);
        if (!isspace(next)) // space or newline
                n = 0;
        *line += n;
        return n;
}
#define scan(tokens) _scanToken(&line, " " tokens "%c %n", null)
#define scanValue(tokens, value) _scanToken(&line, " " tokens "%c %n", value)

// For skipping unknown commands or options
#define ignoreOneToken(type) Statement(\
        while (isspace(*line)) line++;\
        int _n=0; char *_s=line;\
        _scanToken(&line, "%*s%n%c %n", &_n);\
        if (_n && debug) printf("info string %s ignored (%.*s)\n", type, _n, _s);\
)
#define ignoreOtherTokens() Statement( while(*line != '\0') ignoreOneToken("Option"); )

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
        struct searchArgs args;

        // Process commands from stdin
        while (fflush(stdout), readLine(stdin, &lineBuffer) > 0) {
                char *line = lineBuffer.v;
                if (debug) printf("info string input %s", line);

                if (scan("uci")) {
                        ignoreOtherTokens();
                        printf("id name Floyd "quote2(floydVersion)"\n"
                                "id author Marcel van Kervinck\n"
                                "option name Hash type spin default %ld min 0 max %ld\n"
                                "option name Clear Hash type button\n"
                                //"option name Ponder type check default false\n" // Keep commented out for now
                                //"option name MultiPV type spin default 1 min 1 max 1\n"
                                //"option name UCI_Chess960 type check default false\n"
                                //"option name Contempt type spin default 0 min -100 max 100\n"
                                "uciok\n",
                               newOptions.Hash, maxHash);
                }
                else if (scan("debug")) {
                        if (scan("on")) debug = true;
                        else if (scan("off")) debug = false;
                        ignoreOtherTokens();
                        printf("debug %s\n", debug ? "on" : "off");
                }
                else if (scan("setoption")) {
                        if (scanValue("name Hash value %ld", &newOptions.Hash)) pass;
                        else if (scan("name Ponder value true")) newOptions.Ponder = true;
                        else if (scan("name Ponder value false")) newOptions.Ponder = false;
                        else if (scan("name Clear Hash")) newOptions.ClearHash = !oldOptions.ClearHash;
                        ignoreOtherTokens();
                }
                else if (scan("isready")) {
                        ignoreOtherTokens();
                        updateOptions(self, &oldOptions, &newOptions);
                        printf("readyok\n");
                }
                else if (scan("ucinewgame"))
                        ignoreOtherTokens();

                else if (scan("position")) {
                        searchThread = stopSearch(self, searchThread);

                        if (scan("startpos"))
                                setupBoard(board(self), startpos);
                        else if (scan("fen")) {
                                int n = setupBoard(board(self), line);
                                if (debug && n == 0) printf("info string Invalid position\n");
                                line += n;
                        }
                        if (scan("moves")) {
                                for (int n=1; n>0; line+=n) {
                                        int moves[maxMoves], move;
                                        int nrMoves = generateMoves(board(self), moves);
                                        n = parseMove(board(self), line, moves, nrMoves, &move);
                                        if (n > 0) makeMove(board(self), move);
                                        if (debug && n == -1) printf("info string Illegal move\n");
                                        if (debug && n == -2) printf("info string Ambiguous move\n");
                                }
                        }
                        ignoreOtherTokens();

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

                        args = (struct searchArgs) {
                                .self = self,
                                .ponder = false,
                                .infinite = false,
                        };
                        self->infoFunction = uciSearchInfo;
                        self->infoData = self;

                        long time = 0, btime = 0;
                        long inc = 0, binc = 0;
                        int movestogo = 0;
                        self->targetDepth = maxDepth;
                        long long nodes = maxLongLong;
                        int mate = 0; // TODO: not implemented
                        long movetime = 0;
                        self->searchMoves.len = 0; // TODO: not implemented

                        while (*line != '\0')
                                if ((scan("ponder") && (args.ponder = true))
                                 || scanValue("wtime %ld", &time) || scanValue("btime %ld", &btime)
                                 || scanValue("winc %ld", &inc)   || scanValue("binc %ld", &binc)
                                 || scanValue("movestogo %d", &movestogo)
                                 || scanValue("depth %d", &self->targetDepth)
                                 || scanValue("nodes %lld", &nodes)
                                 || scanValue("mate %d", &mate)
                                 || scanValue("movetime %ld", &movetime)
                                 || (scan("infinite") && (args.infinite = true)))
                                        pass;
                                else if (scan("searchmoves"))
                                        for (int n=1; n>0; line+=n) {
                                                int moves[maxMoves], move;
                                                int nrMoves = generateMoves(board(self), moves);
                                                n = parseMove(board(self), line, moves, nrMoves, &move);
                                                if (n > 0) pushList(self->searchMoves, move);
                                                if (debug && n == -1) printf("info string Illegal move\n");
                                                if (debug && n == -2) printf("info string Ambiguous move\n");
                                        }
                                else ignoreOneToken("Option");

                        if (sideToMove(board(self)) == black)
                                time = btime, inc = binc;
                        setTimeTargets(self, time * ms, inc * ms, movestogo, movetime * ms);
                        printf("info string targetTime %.3f abortTime %.3f\n", self->targetTime, self->abortTime);

                        searchThread = startSearch(&args);
                }
                else if (scan("stop")) {
                        ignoreOtherTokens();
                        searchThread = stopSearch(self, searchThread);
                        if (args.infinite) uciBestMove(self);
                }
                else if (scan("ponderhit")) // TODO: implement ponder
                        ignoreOtherTokens();
                else if (scan("quit")) {
                        ignoreOtherTokens();
                        break;
                }

                /*
                 *  Extra commands
                 */
                else if (scan("help")) {
                        ignoreOtherTokens();
                        fputs(helpMessage, stdout);
                }
                else if (scan("eval")) {
                        ignoreOtherTokens();
                        int score = evaluate(board(self));
                        printf("info score cp %.0f string intern %+d\n", round(score / 10.0), score);
                }
                else if (scan("bench")) {
                        updateOptions(self, &oldOptions, &newOptions);
                        long movetime = 1000;
                        scanValue("movetime %ld", &movetime);
                        ignoreOtherTokens();
                        double nps = uciBenchmark(self, movetime * ms, benchInfoFunction, self);
                        printf("result nps %.f\n", nps);
                }
                else ignoreOneToken("Command");
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

bool uciSearchInfo(void *uciInfoData, const char *string, ...)
{
        Engine_t self = uciInfoData;

        long milliSeconds = round(self->seconds / ms);
        printf("info time %ld", milliSeconds);

        if (string != null) {
                va_list ap;
                va_start(ap, string);
                printf(" nodes %lld string ", self->nodeCount);
                vprintf(string, ap);
                va_end(ap);
                putchar('\n');
                return false;
        }

        if (self->pv.len > 0 || self->depth == 0) {
                char scoreString[16];
                if (isMateScore(self->score))
                        sprintf(scoreString, "mate %d",
                                (self->score < 0) ? (minMate - self->score    ) / 2
                                                  : (maxMate - self->score + 1) / 2);
                else
                        sprintf(scoreString, "cp %.0f", round(self->score / 10.0));
                printf(" depth %d score %s", self->depth, scoreString);
        }

        double nps = (self->seconds > 0.0) ? self->nodeCount / self->seconds : 0.0;
        printf(" nodes %lld nps %.f", self->nodeCount, nps);

        double ttLoad = ttCalcLoad(self);
        printf(" hashfull %d", (int) round(ttLoad * 1000.0));

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
        if (self->seconds >= 0.1)
                fflush(stdout);

        return false; // don't abort the search
}

/*----------------------------------------------------------------------+
 |      uciBestMove                                                     |
 +----------------------------------------------------------------------*/

static void uciBestMove(Engine_t self)
{
        char moveString[maxMoveSize];

        if (self->bestMove) {
                moveToUci(board(self), moveString, self->bestMove);
                printf("bestmove %s", moveString);
        } else
                printf("bestmove 0000"); // When in doubt, do as Shredder

        if (self->ponderMove) {
                moveToUci(board(self), moveString, self->ponderMove);
                printf(" ponder %s", moveString);
        }
        putchar('\n');
}

/*----------------------------------------------------------------------+
 |      benchInfoFunction                                               |
 +----------------------------------------------------------------------*/

static bool benchInfoFunction(void *infoData, const char *string, ...)
{
        (void)infoData;
        (void)string;
        Engine_t engine = infoData;
        char fen[maxFenSize];
        boardToFen(&engine->board, fen);
        double s = engine->seconds;
        double nps = (s > 0.0) ? engine->nodeCount / s : 0.0;
        printf("info time %.f nps %.f fen %s\n", s / ms, nps, fen);
        return false;
}

/*----------------------------------------------------------------------+
 |      startSearch / stopSearch                                        |
 +----------------------------------------------------------------------*/

static void searchThreadStart(void *argsData)
{
        struct searchArgs *args = argsData;

        rootSearch(args->self);

        if (!args->infinite) {
                uciBestMove(args->self);
                fflush(stdout);
        }
}

static xThread_t startSearch(struct searchArgs *args)
{
        return createThread(searchThreadStart, args);
}

static xThread_t stopSearch(Engine_t self, xThread_t searchThread)
{
        if (searchThread != null) {
                self->stopFlag = true;
                joinThread(searchThread);
        }
        return null;
}

/*----------------------------------------------------------------------+
 |                                                                      |
 +----------------------------------------------------------------------*/

