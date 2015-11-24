
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
        P(winBonus, 3471),

        P(castleK, -84),
        P(castleQ, -75),
        P(castleKQ, 16),

        P(queenValue, 7002),
        P(rookValue, 3980),
        P(bishopValue, 2266),
        P(knightValue, 2068),
        P(pawnValue1, 1072), P(pawnValue2, 1739), P(pawnValue3, 2387), P(pawnValue4, 3038),
        P(pawnValue5, 3684), P(pawnValue6, 4331), P(pawnValue7, 4964), P(pawnValue8, 5596),

        P(queenAndQueen, -945),
        P(queenAndRook, -636),
        P(queenAndBishop, 24),
        P(queenAndKnight, -64),
        P(queenAndPawn_1, -77), P(queenAndPawn_2, -86),
        P(rookAndRook, -314),
        P(rookAndBishop, -148),
        P(rookAndKnight, -169),
        P(rookAndPawn_1, -50), P(rookAndPawn_2, -61),
        P(bishopAndBishop, 33),
        P(bishopAndKnight, -73),
        P(bishopAndPawn_1, -56), P(bishopAndPawn_2, -19),
        P(knightAndKnight, -95),
        P(knightAndPawn_1, -27), P(knightAndPawn_2, -1),

        P(queenVsRook, -240),
        P(queenVsBishop, 76),
        P(queenVsKnight, -12),
        P(queenVsPawn_1, 29), P(queenVsPawn_2, -83),
        P(rookVsBishop, 2),
        P(rookVsKnight, 22),
        P(rookVsPawn_1, -22), P(rookVsPawn_2, -24),
        P(bishopVsKnight, 7),
        P(bishopVsPawn_1, -11), P(bishopVsPawn_2, 0),
        P(knightVsPawn_1, 2), P(knightVsPawn_2, -17),

        P(controlCenter, 40),
        P(controlExtendedCenter, 41),
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

        P(pawnByFile_0, -5), // "fileA"
        P(pawnByFile_1, -21),
        P(pawnByFile_2, -92),
        P(pawnByFile_3, -123),
        P(pawnByFile_4, -142),
        P(pawnByFile_5, -72),
        P(pawnByFile_6, 24), // "-fileH"

        // 5 parameters for 6 pawn ranks
        P(pawnByRank_0, -27), // "rank2"
        P(pawnByRank_1, -66),
        P(pawnByRank_2, -81),
        P(pawnByRank_3, -36),
        P(pawnByRank_4, 74), // "-rank7"

        P(doubledPawnA, -209), P(doubledPawnB, -115), P(doubledPawnC, -166), P(doubledPawnD, -119),
        P(doubledPawnE, -103), P(doubledPawnF, -42), P(doubledPawnG, -20), P(doubledPawnH, -226),

        P(backwardPawnA, -8), P(backwardPawnB, -44), P(backwardPawnC, -57), P(backwardPawnD, -110),
        P(backwardPawnE, -88), P(backwardPawnF, -37), P(backwardPawnG, -15), P(backwardPawnH, 25),

        P(isolatedPawnClosedA, 44), P(isolatedPawnClosedB, -73),
        P(isolatedPawnClosedC, -50), P(isolatedPawnClosedD, 50),
        P(isolatedPawnClosedE, -10), P(isolatedPawnClosedF, -49),
        P(isolatedPawnClosedG, -32), P(isolatedPawnClosedH, 3),

        P(isolatedPawnOpenA, -11), P(isolatedPawnOpenB, -35),
        P(isolatedPawnOpenC, -61), P(isolatedPawnOpenD, -118),
        P(isolatedPawnOpenE, -97), P(isolatedPawnOpenF, -89),
        P(isolatedPawnOpenG, -66), P(isolatedPawnOpenH, 21),

        P(sidePawnClosedA, 3), P(sidePawnClosedB, -25),
        P(sidePawnClosedC, -3), P(sidePawnClosedD, -4),
        P(sidePawnClosedE, -7), P(sidePawnClosedF, 1),
        P(sidePawnClosedG, -18), P(sidePawnClosedH, 5),

        P(sidePawnOpenA, -7), P(sidePawnOpenB, 45),
        P(sidePawnOpenC, 22), P(sidePawnOpenD, -26),
        P(sidePawnOpenE, -10), P(sidePawnOpenF, 7),
        P(sidePawnOpenG, 21), P(sidePawnOpenH, 53),

        P(middlePawnClosedA, 0), P(middlePawnClosedB, -14),
        P(middlePawnClosedC, 3), P(middlePawnClosedD, 11),
        P(middlePawnClosedE, 18), P(middlePawnClosedF, 14),
        P(middlePawnClosedG, -17), P(middlePawnClosedH, 0),

        P(middlePawnOpenA, 0), P(middlePawnOpenB, 20),
        P(middlePawnOpenC, 31), P(middlePawnOpenD, 36),
        P(middlePawnOpenE, 59), P(middlePawnOpenF, 7),
        P(middlePawnOpenG, 0), P(middlePawnOpenH, 0),

        P(duoPawnA, -19), P(duoPawnB, 6),
        P(duoPawnC, -11), P(duoPawnD, 60),
        P(duoPawnE, 42), P(duoPawnF, 10),
        P(duoPawnG, 54), P(duoPawnH, -70),

        // Quadratic polynomials for passers
        P(passerA_0, -84), P(passerB_0, -33), P(passerC_0, 0), P(passerD_0, -93),
        P(passerE_0, 9), P(passerF_0, 36), P(passerG_0, 1), P(passerH_0, 41),

        P(passerA_1, 93), P(passerB_1, 35), P(passerC_1, -9), P(passerD_1, 13),
        P(passerE_1, -38), P(passerF_1, -72), P(passerG_1, -23), P(passerH_1, 20),

        P(passerA_2, 154), P(passerB_2, 216), P(passerC_2, 235), P(passerD_2, 205),
        P(passerE_2, 223), P(passerF_2, 250), P(passerG_2, 198), P(passerH_2, 144),

        P(passerScalingOffset, 5022),
        P(passerAndQueen, -441),
        P(passerAndRook, -411),
        P(passerAndBishop, 91),
        P(passerAndKnight, -173),
        P(passerAndPawn, -590),
        P(passerVsQueen, 1022),
        P(passerVsRook, 125),
        P(passerVsBishop, -578),
        P(passerVsKnight, -360),
        P(passerVsPawn, 7),

#if 0
        P(drawBlockedA, 0), P(drawBlockedB, 0), P(drawBlockedC, 0), P(drawBlockedD, 0),
        P(drawBlockedE, 0), P(drawBlockedF, 0), P(drawBlockedG, 0), P(drawBlockedH, 0),

        P(drawPasser_0, 0), P(drawPasser_1, 0), P(drawPasser_2, 0),
#endif

        /*
         *  Knights
         */

        P(knightByFile_0, -158), // fileA
        P(knightByFile_1, -203),
        P(knightByFile_2, -218),
        P(knightByFile_3, -153),
        P(knightByFile_4, -86),
        P(knightByFile_5, -16),
        P(knightByFile_6, 28), // -fileH

        P(knightByRank_0, -97), // rank1
        P(knightByRank_1, -173),
        P(knightByRank_2, -198),
        P(knightByRank_3, -135),
        P(knightByRank_4, -13),
        P(knightByRank_5, 129),
        P(knightByRank_6, 200), // -rank8

        /*
         *  Bishops
         */

        P(bishopOnLong_0, 1),
        P(bishopOnLong_1, 30),

        P(bishopByFile_0, -29), // fileA
        P(bishopByFile_1, -39),
        P(bishopByFile_2, -31),
        P(bishopByFile_3, -19),
        P(bishopByFile_4, 1),
        P(bishopByFile_5, -2),
        P(bishopByFile_6, 11), // -fileH

        P(bishopByRank_0, -95), // rank1
        P(bishopByRank_1, -121),
        P(bishopByRank_2, -101),
        P(bishopByRank_3, -71),
        P(bishopByRank_4, -34),
        P(bishopByRank_5, 56),
        P(bishopByRank_6, 32), // -rank8

        /*
         *  Rooks
         */

        P(rookByFile_0, -75), // fileA
        P(rookByFile_1, -78),
        P(rookByFile_2, -50),
        P(rookByFile_3, -10),
        P(rookByFile_4, 36),
        P(rookByFile_5, 55),
        P(rookByFile_6, 91), // -fileH

        P(rookByRank_0, -138), // rank1
        P(rookByRank_1, -335),
        P(rookByRank_2, -491),
        P(rookByRank_3, -541),
        P(rookByRank_4, -448),
        P(rookByRank_5, -274),
        P(rookByRank_6, -107), // -rank8

        /*
         *  Queens
         */

        P(queenByFile_0, -61), // fileA
        P(queenByFile_1, -104),
        P(queenByFile_2, -101),
        P(queenByFile_3, -89),
        P(queenByFile_4, -78),
        P(queenByFile_5, -64),
        P(queenByFile_6, -46), // -fileH

        P(queenByRank_0, -148), // rank1
        P(queenByRank_1, -250),
        P(queenByRank_2, -331),
        P(queenByRank_3, -382),
        P(queenByRank_4, -321),
        P(queenByRank_5, -170),
        P(queenByRank_6, -112), // -rank8

        /*
         *  Kings
         */

        P(kingByFile_0, -6), // fileD
        P(kingByFile_1, -26),
        P(kingByFile_2, 10), // -fileH

        P(kingByRank_0, -126), // rank1
        P(kingByRank_1, -235),
        P(kingByRank_2, -323),
        P(kingByRank_3, -300),
        P(kingByRank_4, -163),
        P(kingByRank_5, 72),
        P(kingByRank_6, 215), // -rank8

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
#endif

        // Drawness. Positive is more drawish. Negative is more sharp
        P(drawOffset, -22),
        P(drawQueen, -2107),
        P(drawRook, 189),
        P(drawBishop, 13),
        P(drawKnight, 34),
        P(drawPawn, -840),
        P(drawQueenEnding, 1742),
        P(drawRookEnding, 508),
        P(drawKnightEnding, 771),
        P(drawBishopEnding, 1735),
        P(drawPawnEnding, -13006),
        P(drawPawnless, 357),

        P(drawQueenImbalance, 1080),
        P(drawRookImbalance, 181),
        P(drawMinorImbalance, 206),

        P(drawUnlikeBishops, 3854),
        P(drawUnlikeBishopsAndQueens, -445),
        P(drawUnlikeBishopsAndRooks, 438),
        P(drawUnlikeBishopsAndKnights, 1086),
        // TODO: pawn and pawn block square colors


// }

/*----------------------------------------------------------------------+
 |                                                                      |
 +----------------------------------------------------------------------*/

