Floyd study engine
==================

`floyd' is a chess engine study by Marcel van Kervinck.

Floyd is not a stand-alone chess program. It can be loaded as an engine
in chess GUIs such as Winboard, Arena, XBoard, Shredder and others.

In addition to the UCI interface, a Python interface is provided
for study purposes. I try to keep the code as simple and clean as
possible. Speed is therefore not as important as clarity and ease
of change.

This is the source code archive. Binaries for Windows, Linux
and Mac can be downloaded from https://marcelk.net/floyd/

Python interface
----------------
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

Command interface
-----------------
```
$ make
$ ./floyd 

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
------------
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

Make targets
------------
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
