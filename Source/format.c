
/*----------------------------------------------------------------------+
 |                                                                      |
 |      format.c -- converting from internal representation to text     |
 |                                                                      |
 +----------------------------------------------------------------------*/

/*
 *  Copyright (C) 1998-2015, Marcel van Kervinck
 *  All rights reserved
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *  1. Redistributions of source code must retain the above copyright
 *  notice, this list of conditions and the following disclaimer.
 *
 *  2. Redistributions in binary form must reproduce the above copyright
 *  notice, this list of conditions and the following disclaimer in the
 *  documentation and/or other materials provided with the distribution.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
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

/*
 *  Convert into UCI notation
 */
extern char *moveToUci(Board_t self, char moveString[maxMoveSize], int move)
{
        int from = from(move);
        int to   = to(move);

        *moveString++ = fileToChar(file(from));
        *moveString++ = rankToChar(rank(from));
        *moveString++ = fileToChar(file(to));
        *moveString++ = rankToChar(rank(to));
        if (isPromotion(self, from, to))
                *moveString++ = tolower(promotionPieceToChar[move>>promotionBits]);
        *moveString = '\0';

        return moveString;
}

/*----------------------------------------------------------------------+
 |      Convert a move to long algebraic notation                       |
 +----------------------------------------------------------------------*/

/*
 *  Convert into long algebraic notation, but without any checkmark
 */
extern char *moveToLongAlgebraic(Board_t self, char moveString[maxMoveSize], int move)
{
        int from = from(move);
        int to   = to(move);

        // Castling
        if (move == specialMove(e1, c1) || move == specialMove(e8, c8))
                return stringCopy(moveString, "O-O-O");
        if (move == specialMove(e1, g1) || move == specialMove(e8, g8))
                return stringCopy(moveString, "O-O");

        // Piece identifier
        char pieceChar = toupper(pieceToChar[self->squares[from]]);
        if (pieceChar != 'P')
                *moveString++ = pieceChar;

        // From square
        *moveString++ = fileToChar(file(from));
        *moveString++ = rankToChar(rank(from));

        // Capture mark
        *moveString++ = (self->squares[to] == empty) ? '-' : 'x';

        // To square
        *moveString++ = fileToChar(file(to));
        *moveString++ = rankToChar(rank(to));

        // Promotion piece
        if (isPromotion(self, from, to)) {
                *moveString++ = '=';
                *moveString++ = promotionPieceToChar[move>>promotionBits];
        }

        *moveString = '\0';
        return moveString;
}

/*----------------------------------------------------------------------+
 |      Convert a move to SAN output                                    |
 +----------------------------------------------------------------------*/

/*
 *  Convert into SAN notation, but without any checkmark
 */
extern char *moveToStandardAlgebraic(Board_t self, char moveString[maxMoveSize], int move, int xMoves[maxMoves], int xlen)
{
        int from = from(move);
        int to   = to(move);

        if (move == specialMove(e1, c1) || move == specialMove(e8, c8))
                return stringCopy(moveString, "O-O-O");

        if (move == specialMove(e1, g1) || move == specialMove(e8, g8))
                return stringCopy(moveString, "O-O");

        if (self->squares[from] == whitePawn || self->squares[from] == blackPawn) {
                // Pawn moves are a bit special

                if (file(from) != file(to)) {
                        *moveString++ = fileToChar(file(from));
                        *moveString++ = 'x';
                }
                *moveString++ = fileToChar(file(to));
                *moveString++ = rankToChar(rank(to));

                // Promote to piece (=Q, =R, =B, =N)
                if (isPromotion(self, from, to)) {
                        *moveString++ = '=';
                        *moveString++ = promotionPieceToChar[move>>promotionBits];
                }
                *moveString = '\0';
                return moveString;
        }

        // Piece identifier (K, Q, R, B, N)
        *moveString++ = toupper(pieceToChar[self->squares[from]]);

        //  Disambiguate using from square information where needed
        int filex = 0, rankx = 0;
        for (int i=0; i<xlen; i++) {
                int xMove = xMoves[i];
                if (to == to(xMove)                     // Must have same destination
                 && move != xMove                       // Different move
                 && self->squares[from] == self->squares[from(xMove)] // Same piece type
                 && isLegalMove(self, xMove)            // And also legal
                ) {
                        rankx |= (rank(from) == rank(from(xMove))) + 1; // Tricky but correct
                        filex |=  file(from) == file(from(xMove));
                }
        }
        if (rankx != filex) *moveString++ = fileToChar(file(from)); // Skip if both are 0 or 1
        if (filex)          *moveString++ = rankToChar(rank(from));

        // Capture sign
        if (self->squares[to] != empty) *moveString++ = 'x';

        // To square
        *moveString++ = fileToChar(file(to));
        *moveString++ = rankToChar(rank(to));

        *moveString = '\0';
        return moveString;
}

/*----------------------------------------------------------------------+
 |      getCheckMark                                                    |
 +----------------------------------------------------------------------*/

/*
 *  Produce a checkmark ('+' or '#')
 *  The move must already be made and sideInfo computed.
 *  sideInfo might be invalid after this function.
 */
extern const char *getCheckMark(Board_t self)
{
        const char *checkmark = "";

        if (inCheck(self)) { // in check, but is it checkmate?
                checkmark = "#";
                int moveList[maxMoves];
                int nrMoves = generateMoves(self, moveList);
                for (int i=0; i<nrMoves; i++) {
                        if (isLegalMove(self, moveList[i])) {
                                checkmark = "+";
                                break;
                        }
                }
        }

        return checkmark;
}

/*----------------------------------------------------------------------+
 |      Convert board to FEN notation                                   |
 +----------------------------------------------------------------------*/

extern void boardToFen(Board_t self, char *fen)
{
        /*
         *  Squares
         */
        for (int rank=rank8; rank!=rank1-rankStep; rank-=rankStep) {
                int emptySquares = 0;
                for (int file=fileA; file!=fileH+fileStep; file+=fileStep) {
                        int square = square(file, rank);
                        int piece = self->squares[square];

                        if (piece == empty) {
                                emptySquares++;
                                continue;
                        }

                        if (emptySquares > 0) *fen++ = '0' + emptySquares;
                        *fen++ = pieceToChar[piece];
                        emptySquares = 0;
                }
                if (emptySquares > 0) *fen++ = '0' + emptySquares;
                if (rank != rank1) *fen++ = '/';
        }

        /*
         *  Side to move
         */
        *fen++ = ' ';
        *fen++ = (sideToMove(self) == white) ? 'w' : 'b';

        /*
         *  Castling flags
         */
        *fen++ = ' ';
        if (self->castleFlags) {
                if (self->castleFlags & castleFlagWhiteKside) *fen++ = 'K';
                if (self->castleFlags & castleFlagWhiteQside) *fen++ = 'Q';
                if (self->castleFlags & castleFlagBlackKside) *fen++ = 'k';
                if (self->castleFlags & castleFlagBlackQside) *fen++ = 'q';
        } else
                *fen++ = '-';

        /*
         *  En-passant square
         */
        *fen++ = ' ';
        normalizeEnPassantStatus(self);
        if (self->enPassantPawn) {
                *fen++ = fileToChar(file(self->enPassantPawn));
                *fen++ = rankToChar((sideToMove(self) == white) ? rank6 : rank3);
        } else
                *fen++ = '-';

        /*
         *  Move number (TODO)
         */

        /*
         *  Halfmove clock (TODO)
         */

        *fen = '\0';
}

/*----------------------------------------------------------------------+
 |                                                                      |
 +----------------------------------------------------------------------*/

