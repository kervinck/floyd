
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

uciSources:=cplus.c engine.c evaluate.c floydmain.c format.c kpk.c moves.c\
            parse.c search.c test.c ttable.c uci.c zobrist.c
uciSources:=$(addprefix Source/, $(uciSources))

osType:=$(shell uname -s)

CFLAGS:=-std=c11 -pedantic -Wall -Wextra -O3 -fstrict-aliasing -fomit-frame-pointer\
	-DfloydVersion=$(floydVersion)

# Use a real gcc for pgo
ifeq "$(osType)" "Darwin"
 GCC:=gcc-mp-4.8 # From MacPorts
endif
ifeq "$(osType)" "Linux"
 ifneq "$(shell which gcc-4.8)" ""
   GCC:=gcc-4.8
 endif
 ifneq "$(shell which gcc-4.9)" ""
   GCC:=gcc-4.9
 endif
endif

ifeq "$(osType)" "Linux"
 LDFLAGS:=-lm -lpthread
endif

win32_exe:=floyd.w32.exe

# Cross-compiler for windows 32bit (from gcc-4.8.0-qt-4.8.4-for-mingw32.dmg)
xcc_win32:=/usr/local/gcc-4.8.0-qt-4.8.4-for-mingw32/win32-gcc/bin/i586-mingw32-gcc
win32_flags:=-Wno-format # Suppress warnings about "%lld"/"%I64d". Both work fine.

# Allow testing before installation
export PYTHONPATH:=build/lib/python:${PYTHONPATH}

#-----------------------------------------------------------------------
#       Targets
#-----------------------------------------------------------------------

# Compile both as Python module and as native UCI engine
all: .module floyd

# Compile as Python module
.module: $(wildcard Source/*) Makefile setup.py versions.json
	env CC="$(CC)" CFLAGS="$(CFLAGS)" floydVersion="$(floydVersion)" python setup.py build
	env floydVersion="$(floydVersion)" python setup.py install --home=build && touch .module

# Compile as native UCI engine
floyd: $(wildcard Source/*) Makefile versions.json
	$(CC) $(CFLAGS) -o $@ $(uciSources) $(LDFLAGS)

# Compile with profile-guided optimization
pgo: floyd-pgo1 floyd-pgo2

floyd-pgo1: $(wildcard Source/*) Makefile versions.json
	$(GCC) $(CFLAGS) -DNDEBUG -o $@ $(uciSources) $(LDFLAGS) -fprofile-generate
	echo 'bench movetime 500 bestof 1' | ./$@ | grep result

floyd-pgo2: $(wildcard Source/*) Makefile versions.json floyd-pgo1
	$(GCC) $(CFLAGS) -DNDEBUG -o $@ $(uciSources) $(LDFLAGS) -fprofile-use
	mkdir -p build && cp -p $@ build/floyd-$(floydVersion) # Keep old versions

# Cross-compile as Win32 UCI engine
win: $(win32_exe)
$(win32_exe): $(wildcard Source/*) Makefile versions.json
	$(xcc_win32) $(CFLAGS) $(win32_flags) -o $@ $(uciSources)

# Run 1 second position tests
easy wac krk5 tt eg ece3: .module
	@python Tools/epdtest.py 1 < Data/$@.epd

# Run 10 second position tests
hard draw nodraw bk zz: .module
	@python Tools/epdtest.py 10 < Data/$@.epd

# Run 100 second position tests
mate mated qmate: .module
	@python Tools/epdtest.py 100 < Data/$@.epd

# Run 1000 second position tests
nolot: .module
	@python Tools/epdtest.py 1000 < Data/$@.epd

# Run the Strategic Test Suite
sts: .module
	@for STS in Data/STS/*.epd; do\
	 printf "%-40s: " `basename $${STS}`;\
	 python Tools/epdtest.py 0.15 < "$${STS}" | awk '/total 100$$/{print $$2}';\
	done | awk '{print;n++;s+=$$NF}END{printf "Total score: %d (%.1f%%)\n", s, s/n}'

# Run node count regression test
nodes: .module
	@python Tools/nodetest.py 8 < Data/thousand.epd | awk '\
	/ nodes / { n[$$5] += $$10; n[-1] += !$$5 }\
	END       { for (d=0; n[d]; d++) print d, n[d], n[d] / n[d-1] }'

bench: floyd-pgo2 floyd
	for N in 1 2 3; do echo bench | ./floyd-pgo2 | grep result; done
	echo bench | ./floyd | grep result # for comparison

# Calculate residual of evaluation function
residual: .module
	@bzcat Data/ccrl-shuffled-3M.epd.bz2 | python Tools/tune.py -q Tuning/vector.json

# Run one standard iteration of the evaluation tuner
tune: .module
	bzcat Data/ccrl-shuffled-3M.epd.bz2 | python Tools/tune.py Tuning/vector.json

# One standard iteration only for the parameters listed in `params'
ptune: .module
	bzcat Data/ccrl-shuffled-3M.epd.bz2 | python Tools/shuffle `head -1 params` | python Tools/tune.py Tuning/vector.json `grep -v "^[#-]" params`

# Coarse tuning (1M positions)
ctune: .module
	bzcat Data/ccrl-shuffled-3M.epd.bz2 | python Tools/shuffle -1000000 | python Tools/tune.py Tuning/vector.json

# Extended tuning (10M positions)
xtune: .module Data/ccrl-shuffled-10M.epd.bz2
	bzcat Data/ccrl-shuffled-10M.epd.bz2 | python Tools/tune.py Tuning/vector.json

Data/ccrl-shuffled-10M.epd.bz2:
	cd Data && wget -N https://marcelk.net/chess/epd/`basename "$@"`

# Deep tuning (1M positions at 2 ply)
dtune: .module
	bzcat Data/ccrl-shuffled-3M.epd.bz2 | head -1000000 | python Tools/tune.py -d 2 Tuning/vector.json

# Plot evaluation tables for easy inspection
tables: Tuning/tables.png
	[ `uname -s` != 'Darwin' ] || open Tuning/tables.png

Tuning/tables.png: Tools/plotTables.py Tuning/vector.json
	python Tools/plotTables.py Tuning/vector.json

# Update source code with the tuned coefficients
update: clean
	python Tools/updateDefaults.py Tuning/vector.json < Source/vector.h > vector.h.tmp
	[ -s vector.h.tmp ] && mv vector.h.tmp Source/vector.h

# Install Python module for the current user
install: .module
	env floydVersion=$(floydVersion) python setup.py install --user

# Install Python module for all system users ('sudo make sysinstall')
sysinstall: .module
	env floydVersion=$(floydVersion) python setup.py install

# Remove compilation intermediates and results
clean:
	env floydVersion=$(floydVersion) python setup.py clean --all
	rm -f floyd $(win32_exe) floyd-pgo[12] *.gcda .module *.tmp
	rm -rf build

# Show all open to-do items
todo: # xtodo
	@git ls-files | grep -v '/fingerprint$$' | xargs grep -I -i todo | grep -v xtodo

# Make fingerprint for regression testing
fingerprint: clean
	@env floydVersion=$(floydVersion) sh Tools/fingerprint.sh 2>&1 | tee fingerprint
	[ `uname -s` != 'Darwin' ] || opendiff Docs/fingerprint fingerprint

# Run search test (20,000 positions at 1 second)
search: .module floyd
	./floyd < /dev/null | grep Version > $@.tmp
	python -u Tools/epdtest.py 1 < Data/$@.epd | tee -a $@.tmp
	sort -n $@.tmp > $@.out
	rm $@.tmp

# Shootout against last version, 1000 games 10+0.15
shootout: floyd-pgo2
	cutechess-cli -concurrency 8 -rounds 1000 -repeat -each tc=10+0.15\
	 -openings file=Data/book-6000-openings.pgn order=random\
	 -resign movecount=1 score=500\
	 -engine cmd=./floyd-pgo2 proto=uci\
	 -engine cmd=floyd0.8 proto=uci\
	 -pgnout shootout.pgn

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
