#!/usr/bin/env python
#-----------------------------------------------------------------------
#
#  updateDefaults.py -- tool to update the vector.h source from vector.json
#
#  Usage: python updateDefaults.py <vector.json> < oldVector.h > newVector.h
#
#-----------------------------------------------------------------------

import re
import sys
import json

with open(sys.argv[1], 'r') as fp:
        vector = json.load(fp)

nrMatches = 0

def update(match):
        id = match.group('id')
        if id in vector:
                global nrMatches
                nrMatches += 1
                return 'P(%s, %d)' % (id, vector[id])
        else:
                return match.group(0)

for line in sys.stdin:
        line = re.sub(
                'P\( (?P<id>\w+), [^)]* \)', # Matches 'X(<id>, <value>)'
                update,
                line,
                flags=re.VERBOSE)
        sys.stdout.write(line)

print >>sys.stderr, '%d defaults updated' % nrMatches

#-----------------------------------------------------------------------
#
#-----------------------------------------------------------------------

