#!/usr/bin/env

import json

with open('vector.json', 'r') as fp:
        vector = json.load(fp)

def getPst(pieceName):

        # Build file coefficients
        files, a = [], 0
        for i in range(7):
                key = '%sByFile_%d' % (pieceName, i)
                b = vector[key]
                files.append(b - a)
                a = b
        files.append(-a)

        # Build rank coefficients
        ranks, a = [], 0
        for i in range(7):
                key = '%sByRank_%d' % (pieceName, i)
                if key not in vector:
                        break # for pawns
                b = vector[key]
                ranks.append(b - a)
                a = b
        ranks.append(-a)

        if len(ranks) < 8: # for pawns
                ranks = [None] + ranks + [None]

        # Print table
        print 'Table:', pieceName
        print 'Files:', files
        print 'Ranks:', ranks
        for rank in reversed(range(8)):
                rankValue = ranks[rank]
                if rankValue is None:
                        continue
                print ' %s ' % chr(ord('a') + rank),
                for file in range(8):
                        fileValue = files[file]
                        print '%5d' % (fileValue + rankValue),
                print
        print ' '*7,
        print (' '*5).join([str(i+1) for i in range(8)]) # 1 2 3 ...
        print

for pieceName in ['knight', 'pawn']:
        getPst(pieceName)
