
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
        P(tempo, 55),
        P(winBonus, 16667),

        P(castleK, -98),
        P(castleQ, -151),
        P(castleKQ, -7),

        P(queenValue, 7273),
        P(rookValue, 4016),
        P(bishopValue, 2412),
        P(knightValue, 2177),
        P(pawnValue1, 1063), P(pawnValue2, 752), P(pawnValue3, 703), P(pawnValue4, 694),
        P(pawnValue5, 675), P(pawnValue6, 693), P(pawnValue7, 712), P(pawnValue8, 813),

        P(queenAndQueen, -1351),
        P(queenAndRook, -1244),
        P(queenAndBishop, -560),
        P(queenAndKnight, -549),
        P(queenAndPawn_1, -116), P(queenAndPawn_2, -62),
        P(rookAndRook, -333),
        P(rookAndBishop, -298),
        P(rookAndKnight, -309),
        P(rookAndPawn_1, -38), P(rookAndPawn_2, -65),
        P(bishopAndBishop, 14),
        P(bishopAndKnight, -172),
        P(bishopAndPawn_1, -56), P(bishopAndPawn_2, -9),
        P(knightAndKnight, -106),
        P(knightAndPawn_1, -21), P(knightAndPawn_2, -7),

        P(queenVsRook, -145),
        P(queenVsBishop, 98),
        P(queenVsKnight, 37),
        P(queenVsPawn_1, 38), P(queenVsPawn_2, -16),
        P(rookVsBishop, -4),
        P(rookVsKnight, 32),
        P(rookVsPawn_1, 6), P(rookVsPawn_2, -16),
        P(bishopVsKnight, -26),
        P(bishopVsPawn_1, 8), P(bishopVsPawn_2, 2),
        P(knightVsPawn_1, 15), P(knightVsPawn_2, -7),

        P(controlCenter, 53),
        P(controlExtendedCenter, 49),
        P(controlOutside, 34),

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

        P(pawnByFile_0, 25), // "fileA"
        P(pawnByFile_1, 17),
        P(pawnByFile_2, -32),
        P(pawnByFile_3, -72),
        P(pawnByFile_4, -92),
        P(pawnByFile_5, -31),
        P(pawnByFile_6, 43), // "-fileH"

        // 5 parameters for 6 pawn ranks
        P(pawnByRank_0, -10), // "rank2"
        P(pawnByRank_1, -32),
        P(pawnByRank_2, -43),
        P(pawnByRank_3, 10),
        P(pawnByRank_4, 96), // "-rank7"

        P(doubledPawnA, -228), P(doubledPawnB, -110), P(doubledPawnC, -156), P(doubledPawnD, -152),
        P(doubledPawnE, -140), P(doubledPawnF, -62), P(doubledPawnG, -68), P(doubledPawnH, -235),

        P(backwardPawnA, 0), P(backwardPawnB, -34), P(backwardPawnC, -63), P(backwardPawnD, -142),
        P(backwardPawnE, -125), P(backwardPawnF, -63), P(backwardPawnG, -45), P(backwardPawnH, 17),

        // Quadratic polynomials for passers
        P(passerA_0, -142), P(passerB_0, -92), P(passerC_0, -138), P(passerD_0, -180),
        P(passerE_0, -158), P(passerF_0, -124), P(passerG_0, -74), P(passerH_0, 12),

        P(passerA_1, 94), P(passerB_1, 46), P(passerC_1, 32), P(passerD_1, 30),
        P(passerE_1, 14), P(passerF_1, -13), P(passerG_1, 21), P(passerH_1, 49),

        P(passerA_2, 131), P(passerB_2, 171), P(passerC_2, 173), P(passerD_2, 145),
        P(passerE_2, 174), P(passerF_2, 196), P(passerG_2, 143), P(passerH_2, 104),

#if 0
        P(drawBlockedA, 0), P(drawBlockedB, 0), P(drawBlockedC, 0), P(drawBlockedD, 0),
        P(drawBlockedE, 0), P(drawBlockedF, 0), P(drawBlockedG, 0), P(drawBlockedH, 0),

        P(drawPasser_0, 0), P(drawPasser_1, 0), P(drawPasser_2, 0),
#endif

        /*
         *  Knights
         */

        P(knightByFile_0, -106), // "fileA"
        P(knightByFile_1, -134),
        P(knightByFile_2, -143),
        P(knightByFile_3, -100),
        P(knightByFile_4, -62),
        P(knightByFile_5, -22),
        P(knightByFile_6, 15), // "-fileH"

        P(knightByRank_0, -72), // "rank1"
        P(knightByRank_1, -115),
        P(knightByRank_2, -123),
        P(knightByRank_3, -58),
        P(knightByRank_4, 65),
        P(knightByRank_5, 202),
        P(knightByRank_6, 270), // "-rank8"

        /*
         *  Rooks
         */

        P(rookByFile_0, -65), // "fileA"
        P(rookByFile_1, -78),
        P(rookByFile_2, -48),
        P(rookByFile_3, -12),
        P(rookByFile_4, 14),
        P(rookByFile_5, 31),
        P(rookByFile_6, 87), // "-fileH"

        P(rookByRank_0, -111), // "rank1"
        P(rookByRank_1, -300),
        P(rookByRank_2, -454),
        P(rookByRank_3, -508),
        P(rookByRank_4, -409),
        P(rookByRank_5, -251),
        P(rookByRank_6, -90), // "-rank8"

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
        P(drawOffset, -1370),
        P(drawQueen, -1556),
        P(drawRook, 709),
        P(drawBishop, 8),
        P(drawKnight, 97),
        P(drawPawn, -745),
        P(drawQueenEnding, 444),
        P(drawRookEnding, -102),
        P(drawKnightEnding, 2053),
        P(drawBishopEnding, 2933),
        P(drawPawnEnding, -10993),
        P(drawPawnless, 566),

        P(drawQueenImbalance, 1352),
        P(drawRookImbalance, 300),
        P(drawMinorImbalance, 398),

        P(drawUnlikeBishops, 3318),
        P(drawUnlikeBishopsAndQueens, -287),
        P(drawUnlikeBishopsAndRooks, 601),
        P(drawUnlikeBishopsAndKnights, 2328),
        // TODO: pawn and pawn block square colors


// }

/*----------------------------------------------------------------------+
 |                                                                      |
 +----------------------------------------------------------------------*/

