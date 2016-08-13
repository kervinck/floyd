
/*----------------------------------------------------------------------+
 |                                                                      |
 |      parse.c -- converting from text to internal representation      |
 |                                                                      |
 +----------------------------------------------------------------------*/

/*
 *  Copyright (C) 2015-2016, Marcel van Kervinck
 *  All rights reserved
 *
 *  Please read the enclosed file `LICENSE' or retrieve this document
 *  from https://marcelk.net/floyd/LICENSE for terms and conditions.
 */

/*----------------------------------------------------------------------+
 |      Includes                                                        |
 +----------------------------------------------------------------------*/

// C standard
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

// C extension
#include "cplus.h"

// Own interface
#include "Board.h"

/*----------------------------------------------------------------------+
 |      Definitions                                                     |
 +----------------------------------------------------------------------*/

/*
 *  Square notation
 */

#define rankToChar(rank)        ('1'   + rankStep * ((rank) - rank1))
#define charToRank(c)           (rank1 + rankStep * ((c) - '1'))
#define fileToChar(file)        ('a'   + fileStep * ((file) - fileA))
#define charToFile(c)           (fileA + fileStep * ((c) - 'a'))

/*----------------------------------------------------------------------+
 |      Data                                                            |
 +----------------------------------------------------------------------*/

static const int promotionFlags[] = {
        ['q'] = queenPromotionFlags,  ['r'] = rookPromotionFlags,
        ['b'] = bishopPromotionFlags, ['n'] = knightPromotionFlags,
};

/*----------------------------------------------------------------------+
 |      Functions                                                       |
 +----------------------------------------------------------------------*/

/*----------------------------------------------------------------------+
 |      setupBoard                                                      |
 +----------------------------------------------------------------------*/

extern int setupBoard(Board_t self, const char *fen)
{
        int ix = 0;

        /*
         *  Squares
         */

        while (isspace(fen[ix])) ix++;

        int file = fileA, rank = rank8;
        int nrWhiteKings = 0, nrBlackKings = 0;
        memset(self->squares, empty, boardSize);
        self->materialKey = 0;
        while (rank != rank1 || file != fileH + fileStep) {
                int piece = empty;
                int count = 1;

                switch (fen[ix]) {
                case 'K': piece = whiteKing; nrWhiteKings++; break;
                case 'Q': piece = whiteQueen; break;
                case 'R': piece = whiteRook; break;
                case 'B': piece = whiteBishop; break;
                case 'N': piece = whiteKnight; break;
                case 'P': piece = whitePawn; break;
                case 'k': piece = blackKing; nrBlackKings++; break;
                case 'r': piece = blackRook; break;
                case 'q': piece = blackQueen; break;
                case 'b': piece = blackBishop; break;
                case 'n': piece = blackKnight; break;
                case 'p': piece = blackPawn; break;
                case '/': rank -= rankStep; file = fileA; ix++; continue;
                case '1': case '2': case '3': case '4':
                case '5': case '6': case '7': case '8':
                        count = fen[ix] - '0';
                        break;
                default:
                        return 0; // FEN error
                }
                int squareColor = squareColor(square(file,rank));
                self->materialKey += materialKeys[piece][squareColor];
                do {
                        self->squares[square(file,rank)] = piece;
                        file += fileStep;
                } while (--count && file != fileH + fileStep);
                ix++;
        }
        if (nrWhiteKings != 1 || nrBlackKings != 1) return 0;

        /*
         *  Side to move
         */

        self->plyNumber = 2 + (fen[ix+1] == 'b'); // 2 means full move number starts at 1
        //self->lastZeroing = self->plyNumber;
        ix += 2;

        /*
         *  Castling flags
         */

        while (isspace(fen[ix])) ix++;

        self->castleFlags = 0;
        for (;; ix++) {
                switch (fen[ix]) {
                case 'K': self->castleFlags |= castleFlagWhiteKside; continue;
                case 'Q': self->castleFlags |= castleFlagWhiteQside; continue;
                case 'k': self->castleFlags |= castleFlagBlackKside; continue;
                case 'q': self->castleFlags |= castleFlagBlackQside; continue;
                case '-': ix++; break;
                default: break;
                }
                break;
        }

        /*
         *  En passant square
         */

        while (isspace(fen[ix])) ix++;

        if ('a' <= fen[ix] && fen[ix] <= 'h') {
                file = charToFile(fen[ix]);
                ix++;

                rank = (sideToMove(self) == white) ? rank5 : rank4;
                if (isdigit(fen[ix])) ix++; // ignore what it says

                self->enPassantPawn = square(file, rank);
        } else {
                self->enPassantPawn = 0;
                if (fen[ix] == '-')
                        ix++;
        }

        // Eat move number and halfmove clock. TODO: process this properly
        while (isspace(fen[ix])) ix++;
        while (isdigit(fen[ix])) ix++;
        while (isspace(fen[ix])) ix++;
        while (isdigit(fen[ix])) ix++;

        self->sideInfoPlyNumber = -1; // side info is invalid now

        // Reset the undo stack
        self->undoStack.len = 0;

        // Initialize hash and its history
        self->hash = hash(self);
        self->hashHistory.len = 0;

        self->pawnKingHash = pawnKingHash(self);
        self->pkHashHistory.len = 0;

        normalizeEnPassantStatus(self); // Only safe after update of hash

        self->eloDiff = 0;

        return ix;
}

/*----------------------------------------------------------------------+
 |      Move parser                                                     |
 +----------------------------------------------------------------------*/

/*
 *  Accept: "O-O" "O-O-O" "o-o" "0-0" "OO" "000" etc
 *  Reject: "OO-O" "O--O" "o-O" "o0O" etc
 */
static int parseCastling(const char *line, int *len)
{
        int nrOh = 0;
        int ix = 0;
        int oh = line[ix];
        if (oh == 'O' || oh == 'o' || oh == '0') {
                do {
                        nrOh++;
                        if (line[++ix] == '-')  ix++;
                } while (line[ix] == oh);
        }
        if (ix != nrOh && ix != 2 * nrOh - 1) { // still looks malformed
                nrOh = 0;
                ix = 0;
        }
        *len = ix;
        return nrOh;
}

extern int parseUciMove(Board_t self, const char *line, int xMoves[maxMoves], int xlen, int *move)
{
        int ix = 0; // index into line
        int rawMove = 0;

        while (isspace(line[ix])) // Skip white space
                ix++;

        int castleLen;
        int nrOh = parseCastling(&line[ix], &castleLen);

        if (nrOh == 2) { // King-side castling
                int rank = (sideToMove(self) == white) ? rank1 : rank8;
                rawMove = move(square(fileE, rank), square(fileG, rank));
                ix += castleLen;
        } else if (nrOh == 3) { // Queen-side castling
                int rank = (sideToMove(self) == white) ? rank1 : rank8;
                rawMove = move(square(fileE, rank), square(fileC, rank));
                ix += castleLen;
        } else { // Regular move
                if ('a' > line[ix+0] || line[ix+0] > 'h'
                 || '1' > line[ix+1] || line[ix+1] > '8'
                 || 'a' > line[ix+2] || line[ix+2] > 'h'
                 || '1' > line[ix+3] || line[ix+3] > '8')
                        return 0;

                int fromFile = charToFile(line[ix++]);
                int fromRank = charToRank(line[ix++]);
                int toFile   = charToFile(line[ix++]);
                int toRank   = charToRank(line[ix++]);

                rawMove = move(square(fromFile, fromRank), square(toFile, toRank));

                if (line[ix] == 'q' || line[ix] == 'r'
                 || line[ix] == 'b' || line[ix] == 'n')
                        rawMove += promotionFlags[(int)line[ix++]];
        }

        if (!isspace(line[ix]) && line[ix] != '\0')
                return 0; // Reject garbage following the move

        // Find matching move from move list and verify its legality
        for (int i=0; i<xlen; i++) {
                int xMove = xMoves[i];
                if ((xMove & ~specialMoveFlag) == rawMove && isLegalMove(self, xMove))
                        return *move = xMove, ix;
        }
        return *move = -1, ix;
}

/*----------------------------------------------------------------------+
 |                                                                      |
 +----------------------------------------------------------------------*/

