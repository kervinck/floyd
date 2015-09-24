CFLAGS=-std=c99 -pedantic -Wall -O3

all: module

# python module
module:
	python setup.py build

# TODO: allow testing before install
test:
	python Tools/searchtest.py

todo:
	find . -type f -size -1M -print0 | xargs -0 grep -i todo

# Dump resulting evaluation tables for easy inspection
tables: tables.png
	[ `uname -s` = 'Darwin' ] && open tables.png

tables.png: vector.json
	python Tools/plotTables.py vector.json

tune:
	bzcat Data/ccrl-shuffled-3M.epd.bz2 | python Tools/tune.py -s 2 vector.json

ftune:
	bzcat Data/ccrl-shuffled-3M.epd.bz2 | head -500000 | python Tools/tune.py -s 2 -m 100000 fvector.json

update: clean
	python Tools/updateDefaults.py vector.json < Source/vector.h > vector.h.tmp
	[ -s vector.h.tmp ] && mv vector.h.tmp Source/vector.h

install:
	python setup.py install --user

clean:
	python setup.py clean --all

# vi: noexpandtab
