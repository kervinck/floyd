
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
        P(tempo, 36),
        P(winBonus, 3442),

        P(castleK, -83),
        P(castleQ, -78),
        P(castleKQ, 17),

        P(queenValue, 6994),
        P(rookValue, 3937),
        P(bishopValue, 2265),
        P(knightValue, 2059),
        P(pawnValue1, 1057), P(pawnValue2, 1738), P(pawnValue3, 2384), P(pawnValue4, 3038),
        P(pawnValue5, 3685), P(pawnValue6, 4329), P(pawnValue7, 4966), P(pawnValue8, 5609),

        P(queenAndQueen, -945),
        P(queenAndRook, -652),
        P(queenAndBishop, 11),
        P(queenAndKnight, -68),
        P(queenAndPawn_1, -80), P(queenAndPawn_2, -81),
        P(rookAndRook, -303),
        P(rookAndBishop, -148),
        P(rookAndKnight, -175),
        P(rookAndPawn_1, -52), P(rookAndPawn_2, -61),
        P(bishopAndBishop, 28),
        P(bishopAndKnight, -68),
        P(bishopAndPawn_1, -56), P(bishopAndPawn_2, -19),
        P(knightAndKnight, -96),
        P(knightAndPawn_1, -27), P(knightAndPawn_2, -1),

        P(queenVsRook, -271),
        P(queenVsBishop, 70),
        P(queenVsKnight, -10),
        P(queenVsPawn_1, 14), P(queenVsPawn_2, -73),
        P(rookVsBishop, 11),
        P(rookVsKnight, 22),
        P(rookVsPawn_1, -22), P(rookVsPawn_2, -20),
        P(bishopVsKnight, 7),
        P(bishopVsPawn_1, -11), P(bishopVsPawn_2, 1),
        P(knightVsPawn_1, 3), P(knightVsPawn_2, -17),

        P(controlCenter, 40),
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
        P(pawnByFile_2, -93),
        P(pawnByFile_3, -123),
        P(pawnByFile_4, -139),
        P(pawnByFile_5, -70),
        P(pawnByFile_6, 24), // "-fileH"

        // 5 parameters for 6 pawn ranks
        P(pawnByRank_0, -25), // "rank2"
        P(pawnByRank_1, -61),
        P(pawnByRank_2, -76),
        P(pawnByRank_3, -36),
        P(pawnByRank_4, 74), // "-rank7"

        P(doubledPawnA, -220), P(doubledPawnB, -128), P(doubledPawnC, -170), P(doubledPawnD, -129),
        P(doubledPawnE, -109), P(doubledPawnF, -54), P(doubledPawnG, -31), P(doubledPawnH, -238),

        P(backwardPawnA, -7), P(backwardPawnB, -45), P(backwardPawnC, -60), P(backwardPawnD, -106),
        P(backwardPawnE, -90), P(backwardPawnF, -37), P(backwardPawnG, -17), P(backwardPawnH, 26),

        P(isolatedPawnClosedA, 44), P(isolatedPawnClosedB, -64),
        P(isolatedPawnClosedC, -42), P(isolatedPawnClosedD, 71),
        P(isolatedPawnClosedE, 3), P(isolatedPawnClosedF, -36),
        P(isolatedPawnClosedG, -21), P(isolatedPawnClosedH, 4),

        P(isolatedPawnOpenA, -7), P(isolatedPawnOpenB, -27),
        P(isolatedPawnOpenC, -60), P(isolatedPawnOpenD, -109),
        P(isolatedPawnOpenE, -95), P(isolatedPawnOpenF, -88),
        P(isolatedPawnOpenG, -62), P(isolatedPawnOpenH, 14),

        P(sidePawnClosedA, 4), P(sidePawnClosedB, -25),
        P(sidePawnClosedC, -3), P(sidePawnClosedD, 12),
        P(sidePawnClosedE, -3), P(sidePawnClosedF, 1),
        P(sidePawnClosedG, -19), P(sidePawnClosedH, 5),

        P(sidePawnOpenA, -11), P(sidePawnOpenB, 44),
        P(sidePawnOpenC, 24), P(sidePawnOpenD, -21),
        P(sidePawnOpenE, -4), P(sidePawnOpenF, 9),
        P(sidePawnOpenG, 18), P(sidePawnOpenH, 46),

        P(middlePawnClosedA, 0), P(middlePawnClosedB, -17),
        P(middlePawnClosedC, 1), P(middlePawnClosedD, 24),
        P(middlePawnClosedE, 18), P(middlePawnClosedF, 7),
        P(middlePawnClosedG, -23), P(middlePawnClosedH, 0),

        P(middlePawnOpenA, 0), P(middlePawnOpenB, 15),
        P(middlePawnOpenC, 31), P(middlePawnOpenD, 36),
        P(middlePawnOpenE, 56), P(middlePawnOpenF, 7),
        P(middlePawnOpenG, -3), P(middlePawnOpenH, 0),

        P(duoPawnA, -21), P(duoPawnB, 8),
        P(duoPawnC, -14), P(duoPawnD, 55),
        P(duoPawnE, 40), P(duoPawnF, 15),
        P(duoPawnG, 49), P(duoPawnH, -67),

        // Quadratic polynomials for passers
        P(passerA_0, -88), P(passerB_0, -34), P(passerC_0, -14), P(passerD_0, -98),
        P(passerE_0, -13), P(passerF_0, 22), P(passerG_0, -9), P(passerH_0, 40),

        P(passerA_1, 93), P(passerB_1, 34), P(passerC_1, -6), P(passerD_1, 11),
        P(passerE_1, -37), P(passerF_1, -74), P(passerG_1, -20), P(passerH_1, 21),

        P(passerA_2, 147), P(passerB_2, 207), P(passerC_2, 227), P(passerD_2, 194),
        P(passerE_2, 215), P(passerF_2, 248), P(passerG_2, 192), P(passerH_2, 138),

        P(passerScalingOffset, 5211),
        P(passerAndQueen, -411),
        P(passerAndRook, -489),
        P(passerAndBishop, 91),
        P(passerAndKnight, -250),
        P(passerAndPawn, -561),
        P(passerVsQueen, 972),
        P(passerVsRook, 167),
        P(passerVsBishop, -488),
        P(passerVsKnight, -360),
        P(passerVsPawn, -11),

#if 0
        P(drawBlockedA, 0), P(drawBlockedB, 0), P(drawBlockedC, 0), P(drawBlockedD, 0),
        P(drawBlockedE, 0), P(drawBlockedF, 0), P(drawBlockedG, 0), P(drawBlockedH, 0),

        P(drawPasser_0, 0), P(drawPasser_1, 0), P(drawPasser_2, 0),
#endif

        /*
         *  Knights
         */

        P(knightByFile_0, -150), // fileA
        P(knightByFile_1, -192),
        P(knightByFile_2, -208),
        P(knightByFile_3, -149),
        P(knightByFile_4, -86),
        P(knightByFile_5, -21),
        P(knightByFile_6, 22), // -fileH

        P(knightByRank_0, -98), // rank1
        P(knightByRank_1, -170),
        P(knightByRank_2, -195),
        P(knightByRank_3, -132),
        P(knightByRank_4, -9),
        P(knightByRank_5, 130),
        P(knightByRank_6, 199), // -rank8

        /*
         *  Bishops
         */

        P(bishopOnLong_0, 6),
        P(bishopOnLong_1, 33),

        P(bishopByFile_0, -24), // fileA
        P(bishopByFile_1, -29),
        P(bishopByFile_2, -21),
        P(bishopByFile_3, -11),
        P(bishopByFile_4, 6),
        P(bishopByFile_5, -2),
        P(bishopByFile_6, 8), // -fileH

        P(bishopByRank_0, -90), // rank1
        P(bishopByRank_1, -112),
        P(bishopByRank_2, -94),
        P(bishopByRank_3, -65),
        P(bishopByRank_4, -33),
        P(bishopByRank_5, 57),
        P(bishopByRank_6, 31), // -rank8

        /*
         *  Rooks
         */

        P(rookByFile_0, -72), // fileA
        P(rookByFile_1, -76),
        P(rookByFile_2, -50),
        P(rookByFile_3, -12),
        P(rookByFile_4, 32),
        P(rookByFile_5, 51),
        P(rookByFile_6, 89), // -fileH

        P(rookByRank_0, -136), // rank1
        P(rookByRank_1, -333),
        P(rookByRank_2, -492),
        P(rookByRank_3, -540),
        P(rookByRank_4, -446),
        P(rookByRank_5, -275),
        P(rookByRank_6, -107), // -rank8

        /*
         *  Queens
         */

        P(queenByFile_0, -50), // fileA
        P(queenByFile_1, -91),
        P(queenByFile_2, -93),
        P(queenByFile_3, -83),
        P(queenByFile_4, -73),
        P(queenByFile_5, -62),
        P(queenByFile_6, -48), // -fileH

        P(queenByRank_0, -141), // rank1
        P(queenByRank_1, -237),
        P(queenByRank_2, -319),
        P(queenByRank_3, -365),
        P(queenByRank_4, -298),
        P(queenByRank_5, -148),
        P(queenByRank_6, -97), // -rank8

        /*
         *  Kings
         */

        P(kingByFile_0, -6), // fileD
        P(kingByFile_1, -26),
        P(kingByFile_2, 10), // -fileH

        P(kingByRank_0, -118), // rank1
        P(kingByRank_1, -212),
        P(kingByRank_2, -285),
        P(kingByRank_3, -266),
        P(kingByRank_4, -122),
        P(kingByRank_5, 115),
        P(kingByRank_6, 242), // -rank8


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
        P(passerAndQueen, -411),
        P(passerAndRook, -489),
        P(passerAndBishop, 91),
        P(passerAndKnight, -250),
        P(passerAndPawn, -561),
        P(passerVsQueen, 972),
        P(passerVsRook, 167),
        P(passerVsBishop, -488),
        P(passerVsKnight, -360),
        P(passerVsPawn, -11),
#endif

        // Drawness. Positive is more drawish. Negative is more sharp
        P(drawOffset, -122),
        P(drawQueen, -2223),
        P(drawRook, 260),
        P(drawBishop, -8),
        P(drawKnight, 33),
        P(drawPawn, -864),
        P(drawQueenEnding, 1963),
        P(drawRookEnding, 476),
        P(drawKnightEnding, 886),
        P(drawBishopEnding, 1926),
        P(drawPawnEnding, -16553),
        P(drawPawnless, 408),

        P(drawQueenImbalance, 1136),
        P(drawRookImbalance, 127),
        P(drawMinorImbalance, 285),

        P(drawUnlikeBishops, 3823),
        P(drawUnlikeBishopsAndQueens, -469),
        P(drawUnlikeBishopsAndRooks, 447),
        P(drawUnlikeBishopsAndKnights, 1306),
        // TODO: pawn and pawn block square colors


// }

/*----------------------------------------------------------------------+
 |                                                                      |
 +----------------------------------------------------------------------*/

