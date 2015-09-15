
/*----------------------------------------------------------------------+
 |                                                                      |
 |      vector.h                                                        |
 |                                                                      |
 +----------------------------------------------------------------------*/

/*
 *  This header is included three times from within evaluate.c, each time
 *  with a redefinition of macro X for a different use case:
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
        X(eloDiff, 0),
        X(tempo, 0),
        X(winBonus, 3500),

        X(queenValue, 9000),
        X(rookValue, 5000),
        X(bishopValue, 3300),
        X(knightValue, 3200),
        X(pawnValue1, 1500), X(pawnValue2, 1400), X(pawnValue3, 1300), X(pawnValue4, 1200),
        X(pawnValue5, 1100), X(pawnValue6, 1000), X(pawnValue7, 900), X(pawnValue8, 800),

        X(queenAndQueen, 0),
        X(queenAndRook, 0),
        X(queenAndBishop, 0),
        X(queenAndKnight, 0),
        X(queenAndPawn_1, 0), X(queenAndPawn_2, 0),
        X(rookAndRook, 0),
        X(rookAndBishop, 0),
        X(rookAndKnight, 0),
        X(rookAndPawn_1, 0), X(rookAndPawn_2, 0),
        X(bishopAndBishop, 200),
        X(bishopAndKnight, 0),
        X(bishopAndPawn_1, 0), X(bishopAndPawn_2, 0),
        X(knightAndKnight, 0),
        X(knightAndPawn_1, 0), X(knightAndPawn_2, 0),

        X(queenVsRook, 0),
        X(queenVsBishop, 0),
        X(queenVsKnight, 0),
        X(queenVsPawn_1, 0), X(queenVsPawn_2, 0),
        X(rookVsBishop, 0),
        X(rookVsKnight, 0),
        X(rookVsPawn_1, 0), X(rookVsPawn_2, 0),
        X(bishopVsKnight, 0),
        X(bishopVsPawn_1, 0), X(bishopVsPawn_2, 0),
        X(knightVsPawn_1, 0), X(knightVsPawn_2, 0),

        X(controlCenter, 60),
        X(controlExtendedCenter, 50),
        X(controlOutside, 40),

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

        X(pawnByFile_0, 0), // "fileA"
        X(pawnByFile_1, 0),
        X(pawnByFile_2, 0),
        X(pawnByFile_3, 0),
        X(pawnByFile_4, 0),
        X(pawnByFile_5, 0),
        X(pawnByFile_6, 0), // "-fileH"

        // 5 parameters for 6 pawn ranks
        X(pawnByRank_0, 0), // "rank2"
        X(pawnByRank_1, 0),
        X(pawnByRank_2, 0),
        X(pawnByRank_3, 0),
        X(pawnByRank_4, 0), // "-rank7"

        X(doubledPawnA, -100), X(doubledPawnB, -100), X(doubledPawnC, -100), X(doubledPawnD, -100),
        X(doubledPawnE, -100), X(doubledPawnF, -100), X(doubledPawnG, -100), X(doubledPawnH, -100),

        X(backwardPawnA, 0), X(backwardPawnB, 0), X(backwardPawnC, 0), X(backwardPawnD, 0),
        X(backwardPawnE, 0), X(backwardPawnF, 0), X(backwardPawnG, 0), X(backwardPawnH, 0),

        // Quadratic polynomials for passers
        X(passerA_0, 0), X(passerB_0, 0), X(passerC_0, 0), X(passerD_0, 0),
        X(passerE_0, 0), X(passerF_0, 0), X(passerG_0, 0), X(passerH_0, 0),

        X(passerA_1, 0), X(passerB_1, 0), X(passerC_1, 0), X(passerD_1, 0),
        X(passerE_1, 0), X(passerF_1, 0), X(passerG_1, 0), X(passerH_1, 0),

        X(passerA_2, 0), X(passerB_2, 0), X(passerC_2, 0), X(passerD_2, 0),
        X(passerE_2, 0), X(passerF_2, 0), X(passerG_2, 0), X(passerH_2, 0),

#if 0
        X(drawBlockedA, 0), X(drawBlockedB, 0), X(drawBlockedC, 0), X(drawBlockedD, 0),
        X(drawBlockedE, 0), X(drawBlockedF, 0), X(drawBlockedG, 0), X(drawBlockedH, 0),

        X(drawPasser_0, 0), X(drawPasser_1, 0), X(drawPasser_2, 0),
#endif

        /*
         *  Knights
         */

        X(knightByFile_0, 0), // "fileA"
        X(knightByFile_1, 0),
        X(knightByFile_2, 0),
        X(knightByFile_3, 0),
        X(knightByFile_4, 0),
        X(knightByFile_5, 0),
        X(knightByFile_6, 0), // "-fileH"

        X(knightByRank_0, 0), // "rank1"
        X(knightByRank_1, 0),
        X(knightByRank_2, 0),
        X(knightByRank_3, 0),
        X(knightByRank_4, 0),
        X(knightByRank_5, 0),
        X(knightByRank_6, 0), // "-rank8"

#if 0
        X(attackForceQueen, 0),  X(attackForceQueenX, 0),
        X(attackForceRook, 0),   X(attackForceRookX, 0),
        X(attackForceBishop, 0), X(attackForceBishopX, 0),
        X(attackForceKnight, 0), X(attackForceKnightX, 0),
        X(attackForcePawn, 0),   X(attackForcePawnX, 0),

        X(defenceForceQueen, 0),  X(defenceForceQueenX, 0),
        X(defenceForceRook, 0),   X(defenceForceRookX, 0),
        X(defenceForceBishop, 0), X(defenceForceBishopX, 0),
        X(defenceForceKnight, 0), X(defenceForceKnightX, 0),
        X(defenceForcePawn, 0),   X(defenceForcePawnX, 0),

        X(passerOffset, 0),
        X(passerAndQueen, 0),
        X(passerAndRook, 0),
        X(passerAndBishop, 0),
        X(passerAndKnight, 0),
        X(passerAndPawn, 0),
        X(passerVsQueen, 0),
        X(passerVsRook, 0),
        X(passerVsBishop, 0),
        X(passerVsKnight, 0),
        X(passerVsPawn, 0),
#endif

        // Drawness. Positive is more drawish. Negative is more sharp
        X(drawOffset, 0),
        X(drawQueen, 0),
        X(drawRook, 0),
        X(drawBishop, 0),
        X(drawKnight, 0),
        X(drawPawn, 0),
        X(drawQueenEnding, 0),
        X(drawRookEnding, 0),
        X(drawKnightEnding, 0),
        X(drawBishopEnding, 0),
        X(drawPawnEnding, 0),
        X(drawPawnless, 0),

        X(drawQueenImbalance, 0),
        X(drawRookImbalance, 0),
        X(drawMinorImbalance, 0),

        X(drawUnlikeBishops, 0),
        X(drawUnlikeBishopsAndQueens, 0),
        X(drawUnlikeBishopsAndRooks, 0),
        X(drawUnlikeBishopsAndKnights, 0),
        // TODO: pawn and pawn block square colors


// }

/*----------------------------------------------------------------------+
 |                                                                      |
 +----------------------------------------------------------------------*/

