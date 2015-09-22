
/*----------------------------------------------------------------------+
 |                                                                      |
 |      vector.h                                                        |
 |                                                                      |
 +----------------------------------------------------------------------*/

/*
 *  This header is included three times from within evaluate.c, each time
 *  with a redefinition of macro P for a different use case:
 *  1. to define enum identifiers for the evaluation parameters
 *  2. to generate a table with names so that these are available at runtime
 *  3. to generate a vector with default values
 *
 *  Some naming conventions are:
 *
 *  Suffix '[A-H]' refers to the file when the player's king is on file E..H.
 *  Suffix '[A-H]X' refers to the file when the opponent king is on file E..H.
 *  If the specified king is on A..D instead, the meansing is flipped
 *  (A<->H, B<->G, etc.).
 *
 *  Suffix '_[0-2]' refers to polynomial coefficients (constant, linear, quadratic)
 *  Suffix '_[0-7]' refers to items of a series
 *
 *  In all these cases the order in the vector must be preserved because the
 *  evaluator calculates the index to read the right value.
 */

// {
        P(eloDiff, 52),
        P(tempo, 57),
        P(winBonus, 24759),

        P(castleK, -114),
        P(castleQ, -119),
        P(castleKQ, -12),

        P(queenValue, 6610),
        P(rookValue, 3718),
        P(bishopValue, 2272),
        P(knightValue, 1960),
        P(pawnValue1, 1007), P(pawnValue2, 726), P(pawnValue3, 678), P(pawnValue4, 680),
        P(pawnValue5, 695), P(pawnValue6, 680), P(pawnValue7, 700), P(pawnValue8, 730),

        P(queenAndQueen, -987),
        P(queenAndRook, -769),
        P(queenAndBishop, -122),
        P(queenAndKnight, -109),
        P(queenAndPawn_1, -96), P(queenAndPawn_2, -68),
        P(rookAndRook, -425),
        P(rookAndBishop, -249),
        P(rookAndKnight, -269),
        P(rookAndPawn_1, -50), P(rookAndPawn_2, -67),
        P(bishopAndBishop, -107),
        P(bishopAndKnight, -212),
        P(bishopAndPawn_1, -57), P(bishopAndPawn_2, -19),
        P(knightAndKnight, -201),
        P(knightAndPawn_1, -22), P(knightAndPawn_2, -6),

        P(queenVsRook, -221),
        P(queenVsBishop, 19),
        P(queenVsKnight, 32),
        P(queenVsPawn_1, -25), P(queenVsPawn_2, -33),
        P(rookVsBishop, 51),
        P(rookVsKnight, 46),
        P(rookVsPawn_1, -10), P(rookVsPawn_2, -12),
        P(bishopVsKnight, -56),
        P(bishopVsPawn_1, -3), P(bishopVsPawn_2, 1),
        P(knightVsPawn_1, 7), P(knightVsPawn_2, -15),

        P(controlCenter, 46),
        P(controlExtendedCenter, 50),
        P(controlOutside, 36),

        /*
         *  The 8 coefficients for file scoring are built from 7 tunable
         *  parameters as given by:
         *
         *                           parameter
         *                      0  1  2  3  4  5  6
         *               fileA  1  -  -  -  -  -  -
         *               fileB -1  1  -  -  -  -  -
         *               fileC  - -1  1  -  -  -  -
         *  coefficient  fileD  -  - -1  1  -  -  -
         *               fileE  -  -  - -1  1  -  -
         *               fileF  -  -  -  - -1  1  -
         *               fileG  -  -  -  -  - -1  1
         *               fileH  -  -  -  -  -  - -1
         *
         *  This scheme is used because:
         *   - The sum of coefficients is 0: no overdetermination
         *   - Each parameter has a small scope: no dependency chaining issues
         *   - Suitable for direct use: no preprocessing needed prior to evaluate
         *  (Note that this is different from "tuning the differences between files")
         */

        P(pawnByFile_0, 7), // "fileA"
        P(pawnByFile_1, 3),
        P(pawnByFile_2, -61),
        P(pawnByFile_3, -106),
        P(pawnByFile_4, -138),
        P(pawnByFile_5, -63),
        P(pawnByFile_6, 24), // "-fileH"

        // 5 parameters for 6 pawn ranks
        P(pawnByRank_0, -13), // "rank2"
        P(pawnByRank_1, -38),
        P(pawnByRank_2, -49),
        P(pawnByRank_3, -8),
        P(pawnByRank_4, 90), // "-rank7"

        P(doubledPawnA, -259), P(doubledPawnB, -120), P(doubledPawnC, -182), P(doubledPawnD, -152),
        P(doubledPawnE, -125), P(doubledPawnF, -57), P(doubledPawnG, -78), P(doubledPawnH, -279),

        P(backwardPawnA, 10), P(backwardPawnB, -54), P(backwardPawnC, -72), P(backwardPawnD, -145),
        P(backwardPawnE, -125), P(backwardPawnF, -83), P(backwardPawnG, -40), P(backwardPawnH, 30),

        // Quadratic polynomials for passers
        P(passerA_0, -157), P(passerB_0, -120), P(passerC_0, -161), P(passerD_0, -178),
        P(passerE_0, -136), P(passerF_0, -125), P(passerG_0, -87), P(passerH_0, 2),

        P(passerA_1, 98), P(passerB_1, 38), P(passerC_1, 34), P(passerD_1, 23),
        P(passerE_1, 6), P(passerF_1, -46), P(passerG_1, -4), P(passerH_1, 37),

        P(passerA_2, 126), P(passerB_2, 185), P(passerC_2, 178), P(passerD_2, 152),
        P(passerE_2, 169), P(passerF_2, 236), P(passerG_2, 170), P(passerH_2, 106),

#if 0
        P(drawBlockedA, 0), P(drawBlockedB, 0), P(drawBlockedC, 0), P(drawBlockedD, 0),
        P(drawBlockedE, 0), P(drawBlockedF, 0), P(drawBlockedG, 0), P(drawBlockedH, 0),

        P(drawPasser_0, 0), P(drawPasser_1, 0), P(drawPasser_2, 0),
#endif

        /*
         *  Knights
         */

        P(knightByFile_0, -106), // fileA
        P(knightByFile_1, -157),
        P(knightByFile_2, -171),
        P(knightByFile_3, -118),
        P(knightByFile_4, -74),
        P(knightByFile_5, -11),
        P(knightByFile_6, 23), // -fileH

        P(knightByRank_0, -76), // rank1
        P(knightByRank_1, -138),
        P(knightByRank_2, -156),
        P(knightByRank_3, -86),
        P(knightByRank_4, 50),
        P(knightByRank_5, 198),
        P(knightByRank_6, 255), // -rank8

        /*
         *  Bishops
         */

        P(bishopOnLong_0, 0),
        P(bishopOnLong_1, 0),

        P(bishopByFile_0, 0), // fileA
        P(bishopByFile_1, 0),
        P(bishopByFile_2, 0),
        P(bishopByFile_3, 0),
        P(bishopByFile_4, 0),
        P(bishopByFile_5, 0),
        P(bishopByFile_6, 0), // -fileH

        P(bishopByRank_0, 0), // rank1
        P(bishopByRank_1, 0),
        P(bishopByRank_2, 0),
        P(bishopByRank_3, 0),
        P(bishopByRank_4, 0),
        P(bishopByRank_5, 0),
        P(bishopByRank_6, 0), // -rank8

        /*
         *  Rooks
         */

        P(rookByFile_0, -71), // fileA
        P(rookByFile_1, -78),
        P(rookByFile_2, -51),
        P(rookByFile_3, -11),
        P(rookByFile_4, 20),
        P(rookByFile_5, 37),
        P(rookByFile_6, 93), // -fileH

        P(rookByRank_0, -129), // rank1
        P(rookByRank_1, -318),
        P(rookByRank_2, -481),
        P(rookByRank_3, -529),
        P(rookByRank_4, -431),
        P(rookByRank_5, -262),
        P(rookByRank_6, -103), // -rank8

        /*
         *  Queens
         */

        P(queenByFile_0, 0), // fileA
        P(queenByFile_1, 0),
        P(queenByFile_2, 0),
        P(queenByFile_3, 0),
        P(queenByFile_4, 0),
        P(queenByFile_5, 0),
        P(queenByFile_6, 0), // -fileH

        P(queenByRank_0, 0), // rank1
        P(queenByRank_1, 0),
        P(queenByRank_2, 0),
        P(queenByRank_3, 0),
        P(queenByRank_4, 0),
        P(queenByRank_5, 0),
        P(queenByRank_6, 0), // -rank8

        /*
         *  Kings
         */

        P(kingByFile_0, 0), // fileD
        P(kingByFile_1, 0),
        P(kingByFile_2, 0), // -fileH

        P(kingByRank_0, 0), // rank1
        P(kingByRank_1, 0),
        P(kingByRank_2, 0),
        P(kingByRank_3, 0),
        P(kingByRank_4, 0),
        P(kingByRank_5, 0),
        P(kingByRank_6, 0), // -rank8


#if 0
        P(attackForceQueen, 0),  P(attackForceQueenX, 0),
        P(attackForceRook, 0),   P(attackForceRookX, 0),
        P(attackForceBishop, 0), P(attackForceBishopX, 0),
        P(attackForceKnight, 0), P(attackForceKnightX, 0),
        P(attackForcePawn, 0),   P(attackForcePawnX, 0),

        P(defenceForceQueen, 0),  P(defenceForceQueenX, 0),
        P(defenceForceRook, 0),   P(defenceForceRookX, 0),
        P(defenceForceBishop, 0), P(defenceForceBishopX, 0),
        P(defenceForceKnight, 0), P(defenceForceKnightX, 0),
        P(defenceForcePawn, 0),   P(defenceForcePawnX, 0),

        P(passerOffset, 0),
        P(passerAndQueen, 0),
        P(passerAndRook, 0),
        P(passerAndBishop, 0),
        P(passerAndKnight, 0),
        P(passerAndPawn, 0),
        P(passerVsQueen, 0),
        P(passerVsRook, 0),
        P(passerVsBishop, 0),
        P(passerVsKnight, 0),
        P(passerVsPawn, 0),
#endif

        // Drawness. Positive is more drawish. Negative is more sharp
        P(drawOffset, -1190),
        P(drawQueen, -1984),
        P(drawRook, 600),
        P(drawBishop, 108),
        P(drawKnight, 127),
        P(drawPawn, -779),
        P(drawQueenEnding, 592),
        P(drawRookEnding, -206),
        P(drawKnightEnding, 1648),
        P(drawBishopEnding, 2720),
        P(drawPawnEnding, -14212),
        P(drawPawnless, 215),

        P(drawQueenImbalance, 1553),
        P(drawRookImbalance, 247),
        P(drawMinorImbalance, 471),

        P(drawUnlikeBishops, 3460),
        P(drawUnlikeBishopsAndQueens, 302),
        P(drawUnlikeBishopsAndRooks, 308),
        P(drawUnlikeBishopsAndKnights, 1887),
        // TODO: pawn and pawn block square colors


// }

/*----------------------------------------------------------------------+
 |                                                                      |
 +----------------------------------------------------------------------*/

