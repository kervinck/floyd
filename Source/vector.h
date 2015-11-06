
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
        P(eloDiff, 51),
        P(tempo, 35),
        P(winBonus, 3453),

        P(castleK, -84),
        P(castleQ, -73),
        P(castleKQ, 17),

        P(queenValue, 6995),
        P(rookValue, 3925),
        P(bishopValue, 2264),
        P(knightValue, 2056),
        P(pawnValue1, 1039), P(pawnValue2, 1719), P(pawnValue3, 2378), P(pawnValue4, 3031),
        P(pawnValue5, 3687), P(pawnValue6, 4335), P(pawnValue7, 4989), P(pawnValue8, 5641),

        P(queenAndQueen, -939),
        P(queenAndRook, -651),
        P(queenAndBishop, -3),
        P(queenAndKnight, -67),
        P(queenAndPawn_1, -79), P(queenAndPawn_2, -81),
        P(rookAndRook, -310),
        P(rookAndBishop, -148),
        P(rookAndKnight, -176),
        P(rookAndPawn_1, -52), P(rookAndPawn_2, -61),
        P(bishopAndBishop, 28),
        P(bishopAndKnight, -69),
        P(bishopAndPawn_1, -56), P(bishopAndPawn_2, -19),
        P(knightAndKnight, -96),
        P(knightAndPawn_1, -27), P(knightAndPawn_2, -1),

        P(queenVsRook, -272),
        P(queenVsBishop, 61),
        P(queenVsKnight, -11),
        P(queenVsPawn_1, 14), P(queenVsPawn_2, -68),
        P(rookVsBishop, 11),
        P(rookVsKnight, 22),
        P(rookVsPawn_1, -23), P(rookVsPawn_2, -19),
        P(bishopVsKnight, 7),
        P(bishopVsPawn_1, -11), P(bishopVsPawn_2, 1),
        P(knightVsPawn_1, 3), P(knightVsPawn_2, -17),

        P(controlCenter, 42),
        P(controlExtendedCenter, 44),
        P(controlOutside, 33),

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

        P(pawnByFile_0, -4), // "fileA"
        P(pawnByFile_1, -25),
        P(pawnByFile_2, -94),
        P(pawnByFile_3, -122),
        P(pawnByFile_4, -138),
        P(pawnByFile_5, -69),
        P(pawnByFile_6, 22), // "-fileH"

        // 5 parameters for 6 pawn ranks
        P(pawnByRank_0, -24), // "rank2"
        P(pawnByRank_1, -58),
        P(pawnByRank_2, -70),
        P(pawnByRank_3, -29),
        P(pawnByRank_4, 73), // "-rank7"

        P(doubledPawnA, -229), P(doubledPawnB, -131), P(doubledPawnC, -171), P(doubledPawnD, -144),
        P(doubledPawnE, -121), P(doubledPawnF, -54), P(doubledPawnG, -32), P(doubledPawnH, -242),

        P(backwardPawnA, -9), P(backwardPawnB, -45), P(backwardPawnC, -61), P(backwardPawnD, -110),
        P(backwardPawnE, -93), P(backwardPawnF, -42), P(backwardPawnG, -18), P(backwardPawnH, 26),

        P(isolatedPawnClosedA, 44), P(isolatedPawnClosedB, -61),
        P(isolatedPawnClosedC, -41), P(isolatedPawnClosedD, 80),
        P(isolatedPawnClosedE, 5), P(isolatedPawnClosedF, -36),
        P(isolatedPawnClosedG, -21), P(isolatedPawnClosedH, 3),

        P(isolatedPawnOpenA, -8), P(isolatedPawnOpenB, -26),
        P(isolatedPawnOpenC, -53), P(isolatedPawnOpenD, -108),
        P(isolatedPawnOpenE, -91), P(isolatedPawnOpenF, -88),
        P(isolatedPawnOpenG, -64), P(isolatedPawnOpenH, 6),

        P(sidePawnClosedA, 4), P(sidePawnClosedB, -22),
        P(sidePawnClosedC, -1), P(sidePawnClosedD, 16),
        P(sidePawnClosedE, -1), P(sidePawnClosedF, 1),
        P(sidePawnClosedG, -19), P(sidePawnClosedH, 5),

        P(sidePawnOpenA, -10), P(sidePawnOpenB, 43),
        P(sidePawnOpenC, 24), P(sidePawnOpenD, -22),
        P(sidePawnOpenE, -4), P(sidePawnOpenF, 9),
        P(sidePawnOpenG, 18), P(sidePawnOpenH, 41),

        P(middlePawnClosedA, 0), P(middlePawnClosedB, -19),
        P(middlePawnClosedC, -2), P(middlePawnClosedD, 25),
        P(middlePawnClosedE, 17), P(middlePawnClosedF, 1),
        P(middlePawnClosedG, -24), P(middlePawnClosedH, 0),

        P(middlePawnOpenA, 0), P(middlePawnOpenB, 12),
        P(middlePawnOpenC, 28), P(middlePawnOpenD, 35),
        P(middlePawnOpenE, 53), P(middlePawnOpenF, 6),
        P(middlePawnOpenG, -4), P(middlePawnOpenH, 0),

        P(duoPawnA, -20), P(duoPawnB, 10),
        P(duoPawnC, -14), P(duoPawnD, 58),
        P(duoPawnE, 36), P(duoPawnF, 18),
        P(duoPawnG, 43), P(duoPawnH, -64),

        // Quadratic polynomials for passers
        P(passerA_0, 5), P(passerB_0, -47), P(passerC_0, -44), P(passerD_0, -122),
        P(passerE_0, -38), P(passerF_0, 10), P(passerG_0, -21), P(passerH_0, 37),

        P(passerA_1, 83), P(passerB_1, 31), P(passerC_1, -3), P(passerD_1, 15),
        P(passerE_1, -29), P(passerF_1, -78), P(passerG_1, -24), P(passerH_1, 15),

        P(passerA_2, 138), P(passerB_2, 190), P(passerC_2, 203), P(passerD_2, 170),
        P(passerE_2, 193), P(passerF_2, 242), P(passerG_2, 184), P(passerH_2, 128),

        P(passerScalingOffset, 0),
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

#if 0
        P(drawBlockedA, 0), P(drawBlockedB, 0), P(drawBlockedC, 0), P(drawBlockedD, 0),
        P(drawBlockedE, 0), P(drawBlockedF, 0), P(drawBlockedG, 0), P(drawBlockedH, 0),

        P(drawPasser_0, 0), P(drawPasser_1, 0), P(drawPasser_2, 0),
#endif

        /*
         *  Knights
         */

        P(knightByFile_0, -149), // fileA
        P(knightByFile_1, -189),
        P(knightByFile_2, -207),
        P(knightByFile_3, -147),
        P(knightByFile_4, -86),
        P(knightByFile_5, -21),
        P(knightByFile_6, 19), // -fileH

        P(knightByRank_0, -95), // rank1
        P(knightByRank_1, -168),
        P(knightByRank_2, -193),
        P(knightByRank_3, -128),
        P(knightByRank_4, -6),
        P(knightByRank_5, 132),
        P(knightByRank_6, 200), // -rank8

        /*
         *  Bishops
         */

        P(bishopOnLong_0, 7),
        P(bishopOnLong_1, 34),

        P(bishopByFile_0, -9), // fileA
        P(bishopByFile_1, -19),
        P(bishopByFile_2, -16),
        P(bishopByFile_3, -8),
        P(bishopByFile_4, 6),
        P(bishopByFile_5, -1),
        P(bishopByFile_6, 8), // -fileH

        P(bishopByRank_0, -86), // rank1
        P(bishopByRank_1, -109),
        P(bishopByRank_2, -91),
        P(bishopByRank_3, -65),
        P(bishopByRank_4, -30),
        P(bishopByRank_5, 56),
        P(bishopByRank_6, 29), // -rank8

        /*
         *  Rooks
         */

        P(rookByFile_0, -72), // fileA
        P(rookByFile_1, -75),
        P(rookByFile_2, -49),
        P(rookByFile_3, -12),
        P(rookByFile_4, 31),
        P(rookByFile_5, 50),
        P(rookByFile_6, 89), // -fileH

        P(rookByRank_0, -135), // rank1
        P(rookByRank_1, -333),
        P(rookByRank_2, -492),
        P(rookByRank_3, -541),
        P(rookByRank_4, -446),
        P(rookByRank_5, -274),
        P(rookByRank_6, -107), // -rank8

        /*
         *  Queens
         */

        P(queenByFile_0, -49), // fileA
        P(queenByFile_1, -84),
        P(queenByFile_2, -85),
        P(queenByFile_3, -79),
        P(queenByFile_4, -72),
        P(queenByFile_5, -63),
        P(queenByFile_6, -50), // -fileH

        P(queenByRank_0, -124), // rank1
        P(queenByRank_1, -222),
        P(queenByRank_2, -306),
        P(queenByRank_3, -347),
        P(queenByRank_4, -290),
        P(queenByRank_5, -148),
        P(queenByRank_6, -83), // -rank8

        /*
         *  Kings
         */

        P(kingByFile_0, -6), // fileD
        P(kingByFile_1, -26),
        P(kingByFile_2, 10), // -fileH

        P(kingByRank_0, -106), // rank1
        P(kingByRank_1, -203),
        P(kingByRank_2, -283),
        P(kingByRank_3, -254),
        P(kingByRank_4, -109),
        P(kingByRank_5, 146),
        P(kingByRank_6, 272), // -rank8


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
        P(drawOffset, -156),
        P(drawQueen, -2219),
        P(drawRook, 264),
        P(drawBishop, -8),
        P(drawKnight, 34),
        P(drawPawn, -863),
        P(drawQueenEnding, 1977),
        P(drawRookEnding, 513),
        P(drawKnightEnding, 965),
        P(drawBishopEnding, 2032),
        P(drawPawnEnding, -17553),
        P(drawPawnless, 410),

        P(drawQueenImbalance, 1154),
        P(drawRookImbalance, 137),
        P(drawMinorImbalance, 275),

        P(drawUnlikeBishops, 3848),
        P(drawUnlikeBishopsAndQueens, -427),
        P(drawUnlikeBishopsAndRooks, 444),
        P(drawUnlikeBishopsAndKnights, 1371),
        // TODO: pawn and pawn block square colors


// }

/*----------------------------------------------------------------------+
 |                                                                      |
 +----------------------------------------------------------------------*/

