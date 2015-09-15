
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
        P(eloDiff, 53),
        P(tempo, 51),
        P(winBonus, 17967),

        P(castleK, -90),
        P(castleQ, -138),
        P(castleKQ, 3),

        P(queenValue, 7460),
        P(rookValue, 4057),
        P(bishopValue, 2401),
        P(knightValue, 2178),
        P(pawnValue1, 915), P(pawnValue2, 730), P(pawnValue3, 678), P(pawnValue4, 690),
        P(pawnValue5, 688), P(pawnValue6, 693), P(pawnValue7, 723), P(pawnValue8, 841),

        P(queenAndQueen, -1264),
        P(queenAndRook, -1179),
        P(queenAndBishop, -503),
        P(queenAndKnight, -470),
        P(queenAndPawn_1, -111), P(queenAndPawn_2, -48),
        P(rookAndRook, -385),
        P(rookAndBishop, -290),
        P(rookAndKnight, -301),
        P(rookAndPawn_1, -39), P(rookAndPawn_2, -76),
        P(bishopAndBishop, -2),
        P(bishopAndKnight, -206),
        P(bishopAndPawn_1, -54), P(bishopAndPawn_2, -13),
        P(knightAndKnight, -127),
        P(knightAndPawn_1, -18), P(knightAndPawn_2, -9),

        P(queenVsRook, -221),
        P(queenVsBishop, 27),
        P(queenVsKnight, 37),
        P(queenVsPawn_1, 21), P(queenVsPawn_2, -3),
        P(rookVsBishop, -4),
        P(rookVsKnight, 28),
        P(rookVsPawn_1, 4), P(rookVsPawn_2, -16),
        P(bishopVsKnight, -37),
        P(bishopVsPawn_1, 8), P(bishopVsPawn_2, 4),
        P(knightVsPawn_1, 13), P(knightVsPawn_2, 0),

        P(controlCenter, 58),
        P(controlExtendedCenter, 64),
        P(controlOutside, 38),

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

        P(pawnByFile_0, 20), // "fileA"
        P(pawnByFile_1, 24),
        P(pawnByFile_2, -14),
        P(pawnByFile_3, -50),
        P(pawnByFile_4, -54),
        P(pawnByFile_5, 4),
        P(pawnByFile_6, 56), // "-fileH"

        // 5 parameters for 6 pawn ranks
        P(pawnByRank_0, 12), // "rank2"
        P(pawnByRank_1, -12),
        P(pawnByRank_2, -30),
        P(pawnByRank_3, 12),
        P(pawnByRank_4, 82), // "-rank7"

        P(doubledPawnA, -160), P(doubledPawnB, -92), P(doubledPawnC, -108), P(doubledPawnD, -124),
        P(doubledPawnE, -112), P(doubledPawnF, -20), P(doubledPawnG, -72), P(doubledPawnH, -214),

        P(backwardPawnA, -6), P(backwardPawnB, -44), P(backwardPawnC, -68), P(backwardPawnD, -140),
        P(backwardPawnE, -131), P(backwardPawnF, -90), P(backwardPawnG, -65), P(backwardPawnH, 4),

        // Quadratic polynomials for passers
        P(passerA_0, -112), P(passerB_0, -92), P(passerC_0, -146), P(passerD_0, -132),
        P(passerE_0, -171), P(passerF_0, -130), P(passerG_0, -50), P(passerH_0, 6),

        P(passerA_1, 96), P(passerB_1, 51), P(passerC_1, 52), P(passerD_1, 48),
        P(passerE_1, 28), P(passerF_1, 40), P(passerG_1, 45), P(passerH_1, 78),

        P(passerA_2, 128), P(passerB_2, 171), P(passerC_2, 159), P(passerD_2, 107),
        P(passerE_2, 167), P(passerF_2, 148), P(passerG_2, 132), P(passerH_2, 106),

#if 0
        P(drawBlockedA, 0), P(drawBlockedB, 0), P(drawBlockedC, 0), P(drawBlockedD, 0),
        P(drawBlockedE, 0), P(drawBlockedF, 0), P(drawBlockedG, 0), P(drawBlockedH, 0),

        P(drawPasser_0, 0), P(drawPasser_1, 0), P(drawPasser_2, 0),
#endif

        /*
         *  Knights
         */

        P(knightByFile_0, -8), // "fileA"
        P(knightByFile_1, -52),
        P(knightByFile_2, -76),
        P(knightByFile_3, -104),
        P(knightByFile_4, -102),
        P(knightByFile_5, -74),
        P(knightByFile_6, -58), // "-fileH"

        P(knightByRank_0, -51), // "rank1"
        P(knightByRank_1, -68),
        P(knightByRank_2, -94),
        P(knightByRank_3, -44),
        P(knightByRank_4, 56),
        P(knightByRank_5, 194),
        P(knightByRank_6, 258), // "-rank8"

        /*
         *  Rooks
         */

        P(rookByFile_0, -55), // "fileA"
        P(rookByFile_1, -58),
        P(rookByFile_2, -34),
        P(rookByFile_3, -6),
        P(rookByFile_4, 17),
        P(rookByFile_5, 32),
        P(rookByFile_6, 85), // "-fileH"

        P(rookByRank_0, -97), // "rank1"
        P(rookByRank_1, -266),
        P(rookByRank_2, -407),
        P(rookByRank_3, -447),
        P(rookByRank_4, -369),
        P(rookByRank_5, -230),
        P(rookByRank_6, -73), // "-rank8"

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
        P(drawOffset, -2781),
        P(drawQueen, -1662),
        P(drawRook, 876),
        P(drawBishop, 38),
        P(drawKnight, 35),
        P(drawPawn, -540),
        P(drawQueenEnding, -216),
        P(drawRookEnding, -697),
        P(drawKnightEnding, 3143),
        P(drawBishopEnding, 3811),
        P(drawPawnEnding, -13093),
        P(drawPawnless, 1800),

        P(drawQueenImbalance, 876),
        P(drawRookImbalance, 715),
        P(drawMinorImbalance, 350),

        P(drawUnlikeBishops, 2215),
        P(drawUnlikeBishopsAndQueens, 1326),
        P(drawUnlikeBishopsAndRooks, 978),
        P(drawUnlikeBishopsAndKnights, 2943),
        // TODO: pawn and pawn block square colors


// }

/*----------------------------------------------------------------------+
 |                                                                      |
 +----------------------------------------------------------------------*/

