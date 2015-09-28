CFLAGS=-std=c11 -pedantic -Wall -O3 -DfloydVersion=1.0a

all: module floyd

# as python module
module:
	python setup.py build

SOURCES=cplus.c evaluate.c floyd.c format.c kpk.c moves.c polyglot.c search.c uci.c

# as uci engine
floyd: $(addprefix Source/, $(SOURCES)) $(wildcard Source/*.h)
	gcc $(CFLAGS) -o $@ $(addprefix Source/, $(SOURCES))

# TODO: allow testing before install
test:
	python Tools/searchtest.py

wac:
	python Tools/bmtest.py 1 < Data/wacnew.epd

mate:
	python Tools/bmtest.py 10 < Data/mate.epd

todo:
	find . -type f -size -1M -print0 | xargs -0 grep -i todo

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

# vi: noexpandtab
