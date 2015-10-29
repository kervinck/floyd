
#-----------------------------------------------------------------------
#
#       Makefile for Linux and OSX (type 'make help' for an overview)
#
#-----------------------------------------------------------------------

# If the default compiler doesn't support --std=c11 yet, install gcc-4.8
# and type `make CC=gcc-4.8'
#
# If you get "fatal error: Python.h: No such file or directory",
# install the python-dev package: `sudo apt-get install python-dev'

#-----------------------------------------------------------------------
#       Definitions
#-----------------------------------------------------------------------

floydVersion:=$(shell python Tools/getVersion.py versions.json Source/*)

uciSources:=bench.c cplus.c evaluate.c floydmain.c format.c kpk.c moves.c\
            parse.c search.c ttable.c uci.c zobrist.c
uciSources:=$(addprefix Source/, $(uciSources))

osType:=$(shell uname -s)

CFLAGS:=-std=c11 -pedantic -Wall -Wextra -O3 -fstrict-aliasing -fomit-frame-pointer\
	-DfloydVersion=$(floydVersion)

# Use a real gcc for pgo
GCC:=gcc-mp-4.8

ifeq "$(osType)" "Linux"
 LDFLAGS:=-lm -lpthread
endif

win32_exe:=floyd.w32.exe

# Cross-compiler for windows 32bit (from gcc-4.8.0-qt-4.8.4-for-mingw32.dmg)
xcc_win32:=/usr/local/gcc-4.8.0-qt-4.8.4-for-mingw32/win32-gcc/bin/i586-mingw32-gcc
win32_flags:=-Wno-format # Suppress warnings about "%lld"/"%I64d". Both work fine.

# Allow testing before installation 
PYTHONPATH=build/lib/python:$$PYTHONPATH

#-----------------------------------------------------------------------
#       Targets
#-----------------------------------------------------------------------

# Compile both as Python module and as native UCI engine
all: module floyd

# Compile as Python module
module: $(wildcard Source/*) Makefile versions.json
	env CC="$(CC)" CFLAGS="$(CFLAGS)" floydVersion="$(floydVersion)" python setup.py build
	env floydVersion="$(floydVersion)" python setup.py install --home=build && touch module

# Compile as native UCI engine
floyd: $(wildcard Source/*) Makefile versions.json
	$(CC) $(CFLAGS) -o $@ $(uciSources) $(LDFLAGS)

# Compile with profile-guided optimization (gcc-4.8)
pgo: floyd-pgo1 floyd-pgo2

floyd-pgo1: $(wildcard Source/*) Makefile versions.json
	$(GCC) $(CFLAGS) -DNDEBUG -o $@ $(uciSources) $(LDFLAGS) -fprofile-generate
	echo bench | ./$@ | grep result

floyd-pgo2: $(wildcard Source/*) Makefile versions.json floyd-pgo1
	$(GCC) $(CFLAGS) -DNDEBUG -o $@ $(uciSources) $(LDFLAGS) -fprofile-use

# Cross-compile as Win32 UCI engine
win: $(win32_exe)
$(win32_exe): $(wildcard Source/*) Makefile versions.json
	$(xcc_win32) $(CFLAGS) $(win32_flags) -o $@ $(uciSources)

# Run 1 second position tests
easy wac krk5 tt eg ece3: module
	python Tools/epdtest.py 1 < Data/$@.epd

# Run 10 second position tests
hard draw nodraw bk: module
	python Tools/epdtest.py 10 < Data/$@.epd

# Run 100 second position tests
mate mated qmate: module
	python Tools/epdtest.py 100 < Data/$@.epd

# Run 1000 second position tests
nolot: module
	python Tools/epdtest.py 1000 < Data/$@.epd

# Run node count regression test
nodes: module
	python Tools/nodetest.py 8 < Data/thousand.epd | awk '\
	/ nodes / { n[$$5] += $$10; n[-1] += !$$5 }\
	END       { for (d=0; n[d]; d++) print d, n[d], n[d] / n[d-1] }'

# Run nodes per second benchmark 3 times
bench: floyd-pgo2
	for N in 1 2 3; do echo bench | ./floyd-pgo2 | grep result; done

# Calculate residual of evaluation function
residual: module
	bzcat Data/ccrl-shuffled-3M.epd.bz2 | python Tools/tune.py -q Tuning/vector.json

# Run one iteration of the evaluation tuner
tune: module
	bzcat Data/ccrl-shuffled-3M.epd.bz2 | python Tools/tune.py -s 2 Tuning/vector.json

# Run one fast iteration of the evaluation tuner
ftune: module
	bzcat Data/ccrl-shuffled-3M.epd.bz2 | head -500000 | python Tools/tune.py -s 2 -m 100000 fvector.json

# Plot evaluation tables for easy inspection
tables: Tuning/tables.png
	[ `uname -s` = 'Darwin' ] && open Tuning/tables.png

Tuning/tables.png: Tools/plotTables.py Tuning/vector.json
	python Tools/plotTables.py Tuning/vector.json

# Update source code with the tuned coefficients
update: clean Tuning/tables.png
	python Tools/updateDefaults.py Tuning/vector.json < Source/vector.h > vector.h.tmp
	[ -s vector.h.tmp ] && mv vector.h.tmp Source/vector.h

# Install Python module for the current user
install: module
	env floydVersion=$(floydVersion) python setup.py install --user

# Install Python module for all system users ('sudo make sysinstall')
sysinstall: module
	env floydVersion=$(floydVersion) python setup.py install

# Remove compilation intermediates and results
clean:
	env floydVersion=$(floydVersion) python setup.py clean --all
	rm -f floyd $(win32_exe) floyd-pgo[12] *.gcda module
	rm -rf build

# Show all open to-do items
todo: # xtodo
	@find . -not -path './.git/*' -type f -size -1M -print0 | xargs -0 grep -I -i todo | grep -v xtodo

# Make fingerprint for regression testing
fingerprint: clean
	@env floydVersion=$(floydVersion) sh -x Tools/fingerprint.sh | tee fingerprint
	[ `uname -s` = 'Darwin' ] && opendiff Docs/fingerprint fingerprint

# Show simplified git log
log:
	git log --oneline --decorate --graph --all

# Show summary of make targets
help:
	@awk -F: '/^[a-z].*:( .*)?$$/ && last~/^#/ {printf"%-26s %s\n",$$1,last}{last=$$0}' Makefile 

#-----------------------------------------------------------------------
#
#-----------------------------------------------------------------------
# vi: noexpandtab
