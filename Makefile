CFLAGS=-std=c11 -pedantic -Wall -O3 -DfloydVersion=0.1a

all: module floyd

# As Python module
module:
	python setup.py build

# As UCI engine
SOURCES=bench.c cplus.c evaluate.c floydmain.c format.c kpk.c moves.c parse.c search.c ttable.c uci.c zobrist.c
floyd: $(addprefix Source/, $(SOURCES)) $(wildcard Source/*.h)
	gcc $(CFLAGS) -o $@ $(addprefix Source/, $(SOURCES))

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
tables: tables.png
	[ `uname -s` = 'Darwin' ] && open tables.png

tables.png: Tools/plotTables.py vector.json
	python Tools/plotTables.py vector.json

residual:
	bzcat Data/ccrl-shuffled-3M.epd.bz2 | python Tools/tune.py -q vector.json

tune:
	bzcat Data/ccrl-shuffled-3M.epd.bz2 | python Tools/tune.py -s 2 vector.json

ftune:
	bzcat Data/ccrl-shuffled-3M.epd.bz2 | head -500000 | python Tools/tune.py -s 2 -m 100000 fvector.json

update: clean tables.png
	python Tools/updateDefaults.py vector.json < Source/vector.h > vector.h.tmp
	[ -s vector.h.tmp ] && mv vector.h.tmp Source/vector.h

install:
	python setup.py install --user

clean:
	python setup.py clean --all
	rm -f floyd

# vi: noexpandtab
