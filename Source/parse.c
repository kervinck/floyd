
/*----------------------------------------------------------------------+
 |                                                                      |
 |      parse.c -- converting from text to internal representation      |
 |                                                                      |
 +----------------------------------------------------------------------*/

/*
 *  Copyright (C) 2015, Marcel van Kervinck
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

static bool isPieceChar(int c)
{
        const char *s = strchr("KQRBNP", c);
        return (s != NULL) && (*s != '\0');
}

extern int parseMove(Board_t self, const char *line, int xMoves[maxMoves], int xlen, int *move)
{
        /*
         *  Phase 1: extract as many as possible move elements from input
         */

        int ix = 0; // index into line

        /*
         *  The line elements for move disambiguation
         *  Capture and checkmarks will be swallowed and are not checked
         *  for correctness either
         */
        char fromPiece = 0;
        char fromFile = 0;
        char fromRank = 0;
        char toPiece = 0;
        char toFile = 0;
        char toRank = 0;
        char promotionPiece = 0;

        while (isspace(line[ix]))       // Skip white spaces
                ix++;

        int castleLen;
        int nrOh = parseCastling(&line[ix], &castleLen);

        if (nrOh == 2) {                // King side castling
                fromPiece = 'K';
                fromFile = 'e';
                toFile = 'g';
                ix += castleLen;
        } else if (nrOh == 3) {         // Queen side castling
                fromPiece = 'K';
                fromFile = 'e';
                toFile = 'c';
                ix += castleLen;
        } else {                        // Regular move
                if (isPieceChar(line[ix])) {
                        fromPiece = line[ix++];
                        if (line[ix] == '/') ix++; // ICS madness
                }

                if ('a' <= line[ix] && line[ix] <= 'h')
                        toFile = line[ix++];

                if ('1' <= line[ix] && line[ix] <= '8')
                        toRank = line[ix++];

                switch (line[ix]) {
                case 'x': case ':':
                        if (isPieceChar(line[++ix]))
                                toPiece = line[ix++];
                        break;
                case '-':
                        ix++;
                }

                if ('a' <= line[ix] && line[ix] <= 'h') {
                        fromFile = toFile;
                        fromRank = toRank;
                        toFile = line[ix++];
                        toRank = 0;
                }

                if ('1' <= line[ix] && line[ix] <= '8') {
                        if (toRank) fromRank = toRank;
                        toRank = line[ix++];
                }

                if (line[ix] == '=')
                        ix++;

                if (isPieceChar(toupper(line[ix])))
                        promotionPiece = toupper(line[ix++]);
        }

        while (line[ix] == '+' || line[ix] == '#' || line[ix] == '!' || line[ix] == '?')
                ix++;

        /*
         *  Phase 2: Reject if it still doesn't look anything like a move
         */

        if (isalnum(line[ix]) || line[ix] == '-' || line[ix] == '=')
                return 0; // Reject garbage following the move

        if (!fromPiece && !toPiece && !promotionPiece) {
                if (!fromFile && !toFile) return 0;             // Reject "", "3", "34"
                if (fromRank && !toRank) return 0;              // Reject "3a", "a3b"
                if (toFile && !toRank && !fromFile) return 0;   // Reject "a"
        }

        /*
         *  Phase 3: Search for a unique legal matching move
         */

        int nrMatches = 0;
        int matchedMove = 0;
        int precedence = -1; // -1 no move, 0 regular move, 1 pawn move, 2 queen promotion

        for (int i=0; i<xlen; i++) {
                int xMove = xMoves[i];
                int xFrom = from(xMove);
                int xTo = to(xMove);
                int xPiece = self->squares[xFrom];
                int xPromotionPiece = 0;
                if (isPromotion(self, xFrom, xTo))
                        xPromotionPiece = promotionPieceToChar[(xMove>>promotionBits)&3];

                // Do all parsed elements match with this candidate move? And is it legal?
                if ((fromPiece      && fromPiece != toupper(pieceToChar[xPiece]))
                 || (fromFile       && fromFile  != fileToChar(file(xFrom)))
                 || (fromRank       && fromRank  != rankToChar(rank(xFrom)))
                 || (toPiece        && toPiece   != toupper(pieceToChar[self->squares[xTo]]))
                 || (toFile         && toFile    != fileToChar(file(xTo)))
                 || (toRank         && toRank    != rankToChar(rank(xTo)))
                 || (promotionPiece && promotionPiece != xPromotionPiece)
                 || !isLegalMove(self, xMove))
                        continue; // no match

                int xPrecedence = 0;
                if (xPiece == whitePawn || xPiece == blackPawn)
                        xPrecedence = (xPromotionPiece == 'Q') ? 2 : 1;

                /*
                 *  Clash with another match is not bad if the new candidate move
                 *  is a pawn move and the previous one isn't.
                 *  This is to accept "bxc3" in the presence "Nb1xc3", for example.
                 *  Same logic to prefer queening if the promoted piece is not given.
                 */
                if (precedence < xPrecedence)
                        nrMatches = 0;

                if (precedence <= xPrecedence) {
                        matchedMove = xMove;
                        precedence = xPrecedence;
                        nrMatches++;
                }
        }

        if (nrMatches == 0)
                return -1; // Move not legal

        if (nrMatches > 1)
                return -2; // More than one legal move

        *move = matchedMove;
        return ix;
}

/*----------------------------------------------------------------------+
 |                                                                      |
 +----------------------------------------------------------------------*/

