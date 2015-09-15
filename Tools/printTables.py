#!/usr/bin/env

import json
import sys

with open(sys.argv[1], 'r') as fp:
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
                print ' %s ' % (rank+1),
                for file in range(8):
                        fileValue = files[file]
                        print '%5d' % (fileValue + rankValue),
                print
        print ' '*7,
        print (' '*5).join([chr(ord('a')+i) for i in range(8)]) # a b c ...
        print

def printPasserTable():

        # Get coefficients
        polyList = []
        for file in range(8):
                fileChar = chr(ord('A') + file)
                poly = []
                for exp in range(3):
                        key = 'passer%s_%d' % (fileChar, exp)
                        poly.append(vector[key])
                polyList.append(poly)

        # Print passer table
        print 'Table: passers'
        for rank in [6-i for i in range(6)]:
                print ' %s ' % (1+rank),
                for file in range(8):
                        poly = polyList[file]
                        value = poly[0] + (rank-1) * poly[1] + (rank-1) * (rank-2) * poly[2]
                        print '%5d' % value,
                print
        print ' '*7,
        print (' '*5).join([chr(ord('a')+i) for i in range(8)]) # a b c ...
        print


for pieceName in ['rook', 'knight', 'pawn']:
        getPst(pieceName)

printPasserTable()
