Floyd study engine
==================

*Floyd* is a simple, bare-bones, chess engine study by Marcel van
Kervinck.  It is designed for Windows, Linux and OSX and distributed
under the permissive, "2-clause" or "simplified", open source BSD
license.

Floyd is not a stand-alone chess application. It can be loaded as
an engine in chess GUIs such as Winboard, Arena, XBoard, Shredder
and others. For this Floyd provides a fully compliant UCI interface.
In addition there is also a Python module extension interface for
programming. Just type ``import floyd`` in Python.  Floyd is written
in plain standard C which should be easy to port to other languages
if so desired.

This is the source code archive. Binaries for Windows, Linux and
OSX can be downloaded from https://marcelk.net/floyd/, or you can
compile them yourself from these sources.

Project
=======

The Floyd project is primarily intended as a platform for
experimentation.  This means that emphasis is on ease of change,
clarity, soundness and preferably a very low code count. There are
now only about 4,000 lines of real code. Raw search speed is of a
lesser importance.  Still most, if not all, modern evaluation and
search techniques are present in their basic form and therefore
Floyd can play very strong chess. The project's target, for v1.0,
is to reach 2900 elo at the CCRL 40/40 list, or at least somewhere
in the top-50. Versions 0.x are intermediate stepping stones towards
that.

Outlook after v1.0
==================

After v1.0 it is not clear yet how Floyd will continue as a project.
Additional elo can easily come from making the code faster and
search tuning.

It should be easy to speed it up by a factor of 3. The move generator
design uses a simple mailbox approach because that has advantages
for trying out new evaluation features. Not much is optimized here,
for example, there are no piece lists and capture generation is
done by generating all moves and then filtering out the non-captures.

Also there is no multiprocessing yet. One of the objectives of Floyd
is to use it to explore probability density search (PDS) instead
of traditional SMP search. For that a single-threaded engine is
sufficient. Lazy SMP might be added after v1.0.

Make targets
============

Some tips:

1. Mind to use ``make pgo`` for the best result.
2. If the default compiler doesn't support --std=c11 yet, install gcc-4.8 and type ``make CC=gcc-4.8``
3. If you get "fatal error: Python.h: No such file or directory", install the python-dev package: ``sudo apt-get install python-dev``

Makefile targets are:
```
all                        # Compile both as Python module and as native UCI engine
floyd                      # Compile as native UCI engine
pgo                        # Compile with profile-guided optimization
win                        # Cross-compile as Win32 UCI engine
easy wac krk5 tt eg ece3   # Run 1 second position tests
hard draw nodraw bk        # Run 10 second position tests
mate mated qmate           # Run 100 second position tests
nolot                      # Run 1000 second position tests
sts                        # Run the Strategic Test Suite
nodes                      # Run node count regression test
residual                   # Calculate residual of evaluation function
tune                       # Run one standard iteration of the evaluation tuner
ptune                      # One standard iteration only for the parameters listed in `params'
ctune                      # Coarse tuning (1M positions)
xtune                      # Extended tuning (10M positions)
dtune                      # Deep tuning (1M positions at 2 ply)
tables                     # Plot evaluation tables for easy inspection
update                     # Update source code with the tuned coefficients
install                    # Install Python module for the current user
sysinstall                 # Install Python module for all system users ('sudo make sysinstall')
clean                      # Remove compilation intermediates and results
todo                       # Show all open to-do items
fingerprint                # Make fingerprint for regression testing
shootout                   # Shootout against last version, 1000 games 10+0.15
log                        # Show simplified git log
help                       # Show summary of make targets
```

Python interface (v0.x)
=======================
Note: This interface is planned to change with v1.0 to a class interface!
```
>>> import floyd
>>> help(floyd)

NAME
    floyd - Chess engine study

FUNCTIONS
    evaluate(...)
        evaluate(fen) -> score
    
    search(...)
        search(fen, depth=120, movetime=0.0, info=None) -> score, move
        Valid options for `info' are:
               None    : No info
               'uci'   : Write UCI info lines to stdout
    
    setCoefficient(...)
        setCoefficient(coef, newValue) -> oldValue, name
```

Command interface (UCI)
=======================
```
Floyd Chess Engine - Version 0.x
Copyright (C) 2015-2016, Marcel van Kervinck
All rights reserved

Type "help" for more information, or "quit" to leave.

help
This engine uses the Universal Chess Interface (UCI) protocol.
See https://marcelk.net/chess/uci.html for details.
Supported UCI commands are:
  uci
        Confirm UCI mode, show engine details and options.
  debug [ on | off ]
        Enable/disable debug mode and show its status.
  setoption name <optionName> [ value <optionValue> ]
        Set option. The new value becomes active with the next `isready'.
  isready
        Activate any changed options and reply 'isready' when done.
  ucinewgame
        A new game has started. (ignored)
  position [ startpos | fen <fenField> ... ] [ moves <move> ... ]
        Setup the position on the internal board and play out the sequence
        of moves. In debug mode also show the resulting FEN and board.
  go [ <option> ... ]
        Start searching from the current position within the constraints
        given by the options, or until the `stop' command is received.
        Always show a final result using `bestmove'. (But: see `infinite')
        Command options are:
          searchmoves <move> ...  Only search these moves
          ponder                  Start search in ponder mode
          wtime <millis>          Time remaining on White's clock
          btime <millis>          Time remaining on Black's clock
          winc <millis>           White's increment after each move
          binc <millis>           Black's increment after each move
          movestogo <nrMoves>     Moves to go until next time control
          depth <ply>             Search no deeper than <ply> halfmoves
          nodes <nrNodes>         Search no more than <nrNodes> nodes
          mate <nrMoves>          Search for a mate in <nrMoves> moves or less
          movetime <millis>       Search no longer than this time
          infinite                Postpone `bestmove' result until `stop'
         (Note: In Floyd `ponder' and `infinite' behave the same.)
  ponderhit
        Opponent has played the ponder move. Continue searching in own time.
  stop
        Immediately stop any active `go' command and show its `bestmove' result.
  quit
        Terminate engine.

Extra commands:
  help
        Show this list of commands.
  eval
        Show evaluation.
  bench [ movetime <millis> ] [ bestof <repeat> ]
        Speed test using 40 standard positions. Default: movetime 333 bestof 3
  moves [ depth <ply> ]
        Move generation test. Default: depth 1

Unknown commands and options are silently ignored, except in debug mode.
```

Organization
============

Hierarchy for source modules is as follows:
```
 floydmain.c                            main() for a stand-alone program
 floydmodule.c                          Python interface to search and evaluate
  +--- uci.h
  |     +--- uci.c                      UCI driver
  |     `--- test.c                     Built-in speed benchmark and self test
  +--- Engine.h
  |     +--- search.c                   PVS, scout, quiescence search, SEE
  |     +--- ttable.c                   Transposition table
  |     +--- evaluate.c                 Position evaluation
  |     |     +--- vector.h             Evaluation features and weights
  |     |     `--- kpk.h                In-memory bitbase for King+Pawn vs King
  |     |           `--- kpk.c
  |     `--- engine.c                   Engine object chores
  +--- Board.h
  |     +--- format.c                   Conversion to FEN and UCI move notation
  |     +--- parse.c                    Conversion from FEN and UCI move notation
  |     +--- moves.c                    Move generation, make and undo
  |     |     `--- zobrist.h
  |     |           `--- zobrish.c      Zobrist-Polyglot hash constants
  |     `--- geometry.h                 Definition of board layout
  `--- cplus.h
        `--- cplus.c                    A loose collection of utility functions
```

