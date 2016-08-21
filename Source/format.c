
/*----------------------------------------------------------------------+
 |                                                                      |
 |      format.c -- converting from internal representation to text     |
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
#include <stdio.h>
#include <string.h>

// C extension
#include "cplus.h"

// Own interface
#include "Board.h"

/*----------------------------------------------------------------------+
 |      Definitions                                                     |
 +----------------------------------------------------------------------*/

// Algebraic square notation
#define rankToChar(rank)        ('1'   + rankStep * ((rank) - rank1))
#define charToRank(c)           (rank1 + rankStep * ((c) - '1'))
#define fileToChar(file)        ('a'   + fileStep * ((file) - fileA))
#define charToFile(c)           (fileA + fileStep * ((c) - 'a'))

/*----------------------------------------------------------------------+
 |      Data                                                            |
 +----------------------------------------------------------------------*/

const char startpos[] = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

static const char pieceToChar[] = {
        [empty] = '\0',
        [whiteKing]   = 'K', [whiteQueen]  = 'Q', [whiteRook] = 'R',
        [whiteBishop] = 'B', [whiteKnight] = 'N', [whitePawn] = 'P',
        [blackKing]   = 'k', [blackQueen]  = 'q', [blackRook] = 'r',
        [blackBishop] = 'b', [blackKnight] = 'n', [blackPawn] = 'p',
};

static const char promotionPieceToChar[] = { 'Q', 'R', 'B', 'N' };

/*----------------------------------------------------------------------+
 |      Functions                                                       |
 +----------------------------------------------------------------------*/

/*----------------------------------------------------------------------+
 |      Convert a move to UCI output                                    |
 +----------------------------------------------------------------------*/

// Convert move into UCI notation
extern char *moveToUci(char moveString[maxMoveSize], int move)
{
        int from = from(move);
        int to   = to(move);

        *moveString++ = fileToChar(file(from));
        *moveString++ = rankToChar(rank(from));
        *moveString++ = fileToChar(file(to));
        *moveString++ = rankToChar(rank(to));
        if (((move & specialMoveFlag) && rank(from) == rank7 && rank(to) == rank8)
         || ((move & specialMoveFlag) && rank(from) == rank2 && rank(to) == rank1))
                *moveString++ = tolower(promotionPieceToChar[(move>>promotionBits)&3]);
        *moveString = '\0';

        return moveString;
}

/*----------------------------------------------------------------------+
 |      Convert board to FEN notation                                   |
 +----------------------------------------------------------------------*/

// Convert move into 6-field Forsyth-Edwards Notation (FEN)
extern void boardToFen(Board_t self, char *fen)
{
        // Squares
        for (int rank=rank8; rank!=rank1-rankStep; rank-=rankStep) {
                int emptySquares = 0;
                for (int file=fileA; file!=fileH+fileStep; file+=fileStep) {
                        int square = square(file, rank);
                        int piece = self->squares[square];

                        if (piece != empty) {
                                if (emptySquares > 0) *fen++ = '0' + emptySquares;
                                *fen++ = pieceToChar[piece];
                                emptySquares = 0;
                        } else
                                emptySquares++;
                }
                if (emptySquares > 0) *fen++ = '0' + emptySquares;
                if (rank != rank1) *fen++ = '/';
        }

        // Side to move
        *fen++ = ' ';
        *fen++ = (sideToMove(self) == white) ? 'w' : 'b';

        // Castling flags
        *fen++ = ' ';
        if (self->castleFlags) {
                if (self->castleFlags & castleFlagWhiteKside) *fen++ = 'K';
                if (self->castleFlags & castleFlagWhiteQside) *fen++ = 'Q';
                if (self->castleFlags & castleFlagBlackKside) *fen++ = 'k';
                if (self->castleFlags & castleFlagBlackQside) *fen++ = 'q';
        } else
                *fen++ = '-';

        // En passant square
        *fen++ = ' ';
        normalizeEnPassantStatus(self);
        if (self->enPassantPawn) {
                *fen++ = fileToChar(file(self->enPassantPawn));
                *fen++ = rankToChar((sideToMove(self) == white) ? rank6 : rank3);
        } else
                *fen++ = '-';

        // Halfmove clock and move number
        sprintf(fen, " %d %d", self->halfmoveClock, self->plyNumber >> 1);
}

/*----------------------------------------------------------------------+
 |                                                                      |
 +----------------------------------------------------------------------*/

