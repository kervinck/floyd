#!/usr/bin/env python

import json
import math
import matplotlib.pyplot as pyplot
import numpy as np
import sys

with open(sys.argv[1], 'r') as fp:
        vector = {item[0]:item[1] for item in json.load(fp)}

def pawn(fileIndex, rankIndex, xKing):
        if rankIndex in [0, 7]:
                return None
        value = 0
        if fileIndex > 0: value -= vector['pawnByFile_%d' % (fileIndex - 1)]
        if fileIndex < 7: value += vector['pawnByFile_%d' % (fileIndex)]
        if rankIndex > 1: value -= vector['pawnByRank_%d' % (rankIndex - 1 - 1)]
        if rankIndex < 6: value += vector['pawnByRank_%d' % (rankIndex - 1)]
        return value

def knight(fileIndex, rankIndex, xKing):
        value = 0
        x = 'x' if xKing else ''
        if fileIndex > 0: value -= vector['knightByFile_%d%s' % (fileIndex - 1, x)]
        if fileIndex < 7: value += vector['knightByFile_%d%s' % (fileIndex, x)]
        if rankIndex > 0: value -= vector['knightByRank_%d' % (rankIndex - 1)]
        if rankIndex < 7: value += vector['knightByRank_%d' % (rankIndex)]
        return value

def bishop(fileIndex, rankIndex, xKing):
        squareIndex = fileIndex * 8 + rankIndex
        x = 'x' if xKing else ''
        return vector['bishopBySquare_%d%s' % (squareIndex, x) ]

def rook(fileIndex, rankIndex, xKing):
        value = 0
        if fileIndex > 0: value -= vector['rookByFile_%d' % (fileIndex - 1)]
        if fileIndex < 7: value += vector['rookByFile_%d' % (fileIndex)]
        if rankIndex > 0: value -= vector['rookByRank_%d' % (rankIndex - 1)]
        if rankIndex < 7: value += vector['rookByRank_%d' % (rankIndex)]
        return value

def queen(fileIndex, rankIndex, xKing):
        value = 0
        if fileIndex > 0: value -= vector['queenByFile_%d' % (fileIndex - 1)]
        if fileIndex < 7: value += vector['queenByFile_%d' % (fileIndex)]
        if rankIndex > 0: value -= vector['queenByRank_%d' % (rankIndex - 1)]
        if rankIndex < 7: value += vector['queenByRank_%d' % (rankIndex)]
        return value

def king(fileIndex, rankIndex, xKing):
        value = 0
        if fileIndex < 4: fileIndex ^= 7
        if fileIndex > 4: value -= vector['kingByFile_%d' % (fileIndex - 4 - 1)]
        if fileIndex < 7: value += vector['kingByFile_%d' % (fileIndex - 4)]
        if rankIndex > 0: value -= vector['kingByRank_%d' % (rankIndex - 1)]
        if rankIndex < 7: value += vector['kingByRank_%d' % (rankIndex)]
        return value

def passer(fileIndex, rankIndex, xKing):
        if rankIndex in [0, 7]:
                return None

        # Get coefficients
        fileChar = chr(ord('A') + fileIndex)
        poly = [vector['passer%s_%d' % (fileChar, exp)] for exp in range(3)]

        value = poly[0] + \
                (rankIndex - 1) * poly[1] + \
                (rankIndex - 1) * (rankIndex - 2) * poly[2]

        return value

def plotMap(ax, evaluate, xKing, title, scale=300):

        data = []
        minFile, minRank = 7, 7
        for rankIndex in range(8):
                rank = []
                for fileIndex in range(8):
                        value = evaluate(fileIndex, rankIndex, xKing)
                        if value is not None:
                                rank.append(value)
                                minFile = min(minFile, fileIndex)
                                minRank = min(minRank, rankIndex)
                if len(rank) > 0:
                        data.append(rank)

        matrix = np.array(data)
        nrRanks, nrFiles = matrix.shape

        ax.set_title(title)

        fileLabels = list('abcdefgh')
        rankLabels = list('12345678')

        X = np.linspace(0, nrFiles, nrFiles + 1) + minFile
        Y = np.linspace(0, nrRanks, nrRanks + 1) + minRank
        if scale is None: # passers...
                ax.pcolor(X, Y, matrix, cmap=pyplot.cm.coolwarm)
        else:
                ax.pcolor(X, Y, matrix, cmap=pyplot.cm.RdYlGn, vmin=-scale, vmax=scale)
        ax.axis([0, 8, 0, 8])

        ax.plot(2 if xKing else 6, 7.75, 'o', color='black')
        if xKing is not None:
                ax.plot(6, 0.25, 'o', color='white')
        ax.plot([4, 4], [0,8], color='grey', linestyle=':')

        # centering
        ax.set_xticks(np.arange(8) + 0.5)
        ax.set_yticks(np.arange(8) + 0.5)

        ax.set_xticklabels(fileLabels)
        ax.set_yticklabels(rankLabels)

        for tic in ax.xaxis.get_major_ticks() + ax.yaxis.get_major_ticks():
                tic.tick1On = tic.tick2On = False

        for i, j in [(i, j) for i in range(nrFiles) for j in range(nrRanks)]:
                ax.annotate(matrix[j][i],\
                            xy=(minFile + i + 0.5, minRank + j + 0.5),\
                            horizontalalignment='center',\
                            verticalalignment='center')

if __name__ == '__main__':
        pyplot.rcParams.update({'font.size': 8})

        fig, axes = pyplot.subplots(3, 4)
        fig.set_size_inches(16, 12)

        pawnValue = (vector['pawnValue7'] - vector['pawnValue3']) / 4.0 # approximate middlegame values

        knightValue = vector['knightValue']
        bishopValue = vector['bishopValue']
        rookValue =   vector['rookValue']
        queenValue =  vector['queenValue']

        plotMap(axes[0][0], pawn,   True,  'Pawns (%d = %.2fp)' % (pawnValue, pawnValue / pawnValue))
        plotMap(axes[0][1], pawn,   False, 'Pawns')
        plotMap(axes[0][2], knight, True,  'Knights (%d = %.2fp)' % (knightValue, knightValue / pawnValue))
        plotMap(axes[0][3], knight, False, 'Knights')
        plotMap(axes[1][0], bishop, True,  'Bishops (%d = %.2fp)' % (bishopValue, bishopValue / pawnValue))
        plotMap(axes[1][1], bishop, False, 'Bishops')
        plotMap(axes[1][2], rook,   True,  'Rooks (%d = %.2fp)' % (rookValue, rookValue / pawnValue))
        plotMap(axes[1][3], rook,   False, 'Rooks')
        plotMap(axes[2][0], queen,  True,  'Queens (%d = %.2fp)' % (queenValue, queenValue / pawnValue))
        plotMap(axes[2][1], queen,  False, 'Queens')
        plotMap(axes[2][2], king,   None,  'King')
        plotMap(axes[2][3], passer, False, 'Passers', scale=None)

        pyplot.savefig('Tuning/tables.png', bbox_inches='tight')

