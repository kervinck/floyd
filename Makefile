floydVersion:=0.1a

CFLAGS:=-std=c11 -pedantic -Wall -O3 -DfloydVersion=$(floydVersion)

# Cross-compiler for windows
# Installed from gcc-4.8.0-qt-4.8.4-for-mingw32.dmg
xcc_win32:=/usr/local/gcc-4.8.0-qt-4.8.4-for-mingw32/win32-gcc/bin/i586-mingw32-gcc

all: module floyd
win: $(win32_exe)

win32_exe:=floyd$(floydVersion).w32.exe
win: $(win32_exe)

# As Python module
module:
	python setup.py build

# As native UCI engine
SOURCES=bench.c cplus.c evaluate.c floydmain.c format.c kpk.c moves.c parse.c search.c ttable.c uci.c zobrist.c
floyd: $(addprefix Source/, $(SOURCES)) $(wildcard Source/*.h)
	$(CC) $(CFLAGS) -o $@ $(addprefix Source/, $(SOURCES))

# As Win32 UCI engine
$(win32_exe): $(addprefix Source/, $(SOURCES)) $(wildcard Source/*.h)
	$(xcc_win32) $(CFLAGS) -o $@ $(addprefix Source/, $(SOURCES))

# TODO: allow testing before install
test: install
	python Tools/searchtest.py

# 1 seconds tests
easy wac krk5 tt eg ece3:
	python Tools/bmtest.py 1 < Data/$@.epd

# 10 seconds tests
hard draw nodraw mate bk:
	python Tools/bmtest.py 10 < Data/$@.epd

# 1000 seconds tests
nolot:
	python Tools/bmtest.py 1000 < Data/$@.epd

todo: # xtodo
	@find . -not -path './.git/*' -type f -size -1M -print0 | xargs -0 grep -i todo | grep -v xtodo

# Dump resulting evaluation tables for easy inspection
tables: Tuning/tables.png
	[ `uname -s` = 'Darwin' ] && open Tuning/tables.png

Tuning/tables.png: Tools/plotTables.py Tuning/vector.json
	python Tools/plotTables.py Tuning/vector.json

residual:
	bzcat Data/ccrl-shuffled-3M.epd.bz2 | python Tools/tune.py -q Tuning/vector.json

tune:
	bzcat Data/ccrl-shuffled-3M.epd.bz2 | python Tools/tune.py -s 2 Tuning/vector.json

ftune:
	bzcat Data/ccrl-shuffled-3M.epd.bz2 | head -500000 | python Tools/tune.py -s 2 -m 100000 fvector.json

update: clean Tuning/tables.png
	python Tools/updateDefaults.py Tuning/vector.json < Source/vector.h > vector.h.tmp
	[ -s vector.h.tmp ] && mv vector.h.tmp Source/vector.h

install:
	python setup.py install --user

clean:
	python setup.py clean --all
	rm -f floyd $(win32_exe)

# vi: noexpandtab
