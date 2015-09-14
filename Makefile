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

pst:
	python Tools/getPst.py

tune:
	bzcat Data/ccrl-shuffled-3M.epd.bz2 | head -1000000 | python Tools/tune.py vector.json

ftune:
	bzcat Data/ccrl-shuffled-3M.epd.bz2 | head -100000 | python Tools/tune.py fvector.json

install:
	python setup.py install --user

clean:
	python setup.py clean

# vi: noexpandtab
