
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
        P(winBonus, 3481),

        P(castleK, -100),
        P(castleQ, -82),
        P(castleKQ, 2),

        P(queenValue, 7187),
        P(rookValue, 4074),
        P(bishopValue, 2285),
        P(knightValue, 2103),
        P(pawnValue1, 1047), P(pawnValue2, 1713), P(pawnValue3, 2362), P(pawnValue4, 3023),
        P(pawnValue5, 3683), P(pawnValue6, 4330), P(pawnValue7, 4962), P(pawnValue8, 5564),

        P(queenAndQueen, -988),
        P(queenAndRook, -663),
        P(queenAndBishop, 38),
        P(queenAndKnight, -62),
        P(queenAndPawn_1, -96), P(queenAndPawn_2, -86),
        P(rookAndRook, -312),
        P(rookAndBishop, -137),
        P(rookAndKnight, -166),
        P(rookAndPawn_1, -56), P(rookAndPawn_2, -62),
        P(bishopAndBishop, 49),
        P(bishopAndKnight, -56),
        P(bishopAndPawn_1, -56), P(bishopAndPawn_2, -19),
        P(knightAndKnight, -90),
        P(knightAndPawn_1, -27), P(knightAndPawn_2, -1),

        P(queenVsRook, -238),
        P(queenVsBishop, 83),
        P(queenVsKnight, -8),
        P(queenVsPawn_1, 43), P(queenVsPawn_2, -107),
        P(rookVsBishop, 4),
        P(rookVsKnight, 26),
        P(rookVsPawn_1, -22), P(rookVsPawn_2, -31),
        P(bishopVsKnight, 6),
        P(bishopVsPawn_1, -11), P(bishopVsPawn_2, -5),
        P(knightVsPawn_1, -1), P(knightVsPawn_2, -18),

        P(controlCenter, 36),
        P(controlExtendedCenter, 41),
        P(controlOutside, 32),

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

        P(pawnByFile_0, -1), // fileA
        P(pawnByFile_1, -9),
        P(pawnByFile_2, -78),
        P(pawnByFile_3, -104),
        P(pawnByFile_4, -117),
        P(pawnByFile_5, -63),
        P(pawnByFile_6, 26), // -fileH

        P(pawnByFile_0x, -68), // fileA
        P(pawnByFile_1x, -86),
        P(pawnByFile_2x, -138),
        P(pawnByFile_3x, -172),
        P(pawnByFile_4x, -158),
        P(pawnByFile_5x, -109),
        P(pawnByFile_6x, -34), // -fileH

        // 5 parameters for 6 pawn ranks
        P(pawnByRank_0, -34), // "rank2"
        P(pawnByRank_1, -73),
        P(pawnByRank_2, -86),
        P(pawnByRank_3, -45),
        P(pawnByRank_4, 64), // "-rank7"

        P(doubledPawnA, -229), P(doubledPawnB, -135), P(doubledPawnC, -179), P(doubledPawnD, -132),
        P(doubledPawnE, -121), P(doubledPawnF, -41), P(doubledPawnG, -8), P(doubledPawnH, -223),

        P(backwardPawnA, 4), P(backwardPawnB, -40), P(backwardPawnC, -51), P(backwardPawnD, -96),
        P(backwardPawnE, -102), P(backwardPawnF, -53), P(backwardPawnG, -29), P(backwardPawnH, 7),

        P(isolatedPawnClosedA, 46), P(isolatedPawnClosedB, -52),
        P(isolatedPawnClosedC, -20), P(isolatedPawnClosedD, 21),
        P(isolatedPawnClosedE, -33), P(isolatedPawnClosedF, -75),
        P(isolatedPawnClosedG, -82), P(isolatedPawnClosedH, 10),

        P(isolatedPawnOpenA, 13), P(isolatedPawnOpenB, -28),
        P(isolatedPawnOpenC, -75), P(isolatedPawnOpenD, -135),
        P(isolatedPawnOpenE, -126), P(isolatedPawnOpenF, -79),
        P(isolatedPawnOpenG, -54), P(isolatedPawnOpenH, 19),

        P(sidePawnClosedA, -7), P(sidePawnClosedB, 3),
        P(sidePawnClosedC, 41), P(sidePawnClosedD, -7),
        P(sidePawnClosedE, -10), P(sidePawnClosedF, -15),
        P(sidePawnClosedG, -45), P(sidePawnClosedH, 31),

        P(sidePawnOpenA, -23), P(sidePawnOpenB, 44),
        P(sidePawnOpenC, 5), P(sidePawnOpenD, -41),
        P(sidePawnOpenE, -24), P(sidePawnOpenF, 10),
        P(sidePawnOpenG, 30), P(sidePawnOpenH, 49),

        P(middlePawnClosedA, 0), P(middlePawnClosedB, -2),
        P(middlePawnClosedC, 59), P(middlePawnClosedD, 16),
        P(middlePawnClosedE, 8), P(middlePawnClosedF, 1),
        P(middlePawnClosedG, -22), P(middlePawnClosedH, 0),

        P(middlePawnOpenA, 0), P(middlePawnOpenB, 0),
        P(middlePawnOpenC, 25), P(middlePawnOpenD, 22),
        P(middlePawnOpenE, 33), P(middlePawnOpenF, -9),
        P(middlePawnOpenG, -4), P(middlePawnOpenH, 0),

        P(duoPawnA, -19), P(duoPawnB, 5),
        P(duoPawnC, -26), P(duoPawnD, 61),
        P(duoPawnE, 37), P(duoPawnF, 18),
        P(duoPawnG, 57), P(duoPawnH, -70),

        // Quadratic polynomials for passers
        P(passerA_0, -87), P(passerB_0, -22), P(passerC_0, 7), P(passerD_0, -43),
        P(passerE_0, 68), P(passerF_0, 70), P(passerG_0, 49), P(passerH_0, 48),

        P(passerA_1, 80), P(passerB_1, 12), P(passerC_1, -29), P(passerD_1, -5),
        P(passerE_1, -51), P(passerF_1, -55), P(passerG_1, 2), P(passerH_1, 32),

        P(passerA_2, 159), P(passerB_2, 217), P(passerC_2, 247), P(passerD_2, 211),
        P(passerE_2, 243), P(passerF_2, 276), P(passerG_2, 222), P(passerH_2, 166),

        P(passerScalingOffset, 4782),
        P(passerAndQueen, -124),
        P(passerAndRook, -56),
        P(passerAndBishop, 192),
        P(passerAndKnight, 79),
        P(passerAndPawn, -653),
        P(passerVsQueen, 1035),
        P(passerVsRook, -51),
        P(passerVsBishop, -861),
        P(passerVsKnight, -570),
        P(passerVsPawn, 55),

#if 0
        P(drawBlockedA, 0), P(drawBlockedB, 0), P(drawBlockedC, 0), P(drawBlockedD, 0),
        P(drawBlockedE, 0), P(drawBlockedF, 0), P(drawBlockedG, 0), P(drawBlockedH, 0),

        P(drawPasser_0, 0), P(drawPasser_1, 0), P(drawPasser_2, 0),
#endif

        /*
         *  Knights
         */

        // Kings on same side
        P(knightByFile_0, -177), // fileA
        P(knightByFile_1, -220),
        P(knightByFile_2, -233),
        P(knightByFile_3, -168),
        P(knightByFile_4, -101),
        P(knightByFile_5, -31),
        P(knightByFile_6, 21), // -fileH

        // Opposite side kings
        P(knightByFile_0x, -102), // fileA
        P(knightByFile_1x, -135),
        P(knightByFile_2x, -118),
        P(knightByFile_3x, -28),
        P(knightByFile_4x, 51),
        P(knightByFile_5x, 128),
        P(knightByFile_6x, 106), // -fileH

        P(knightByRank_0, -109), // rank1
        P(knightByRank_1, -193),
        P(knightByRank_2, -220),
        P(knightByRank_3, -155),
        P(knightByRank_4, -34),
        P(knightByRank_5, 118),
        P(knightByRank_6, 184), // -rank8

        /*
         *  Bishops
         */

#if 0
        P(bishopOnLong_0, 5),
        P(bishopOnLong_1, 31),

        P(bishopByFile_0, -40), // fileA
        P(bishopByFile_1, -50),
        P(bishopByFile_2, -41),
        P(bishopByFile_3, -26),
        P(bishopByFile_4, -6),
        P(bishopByFile_5, -5),
        P(bishopByFile_6, 11), // -fileH

        P(bishopByRank_0, -102), // rank1
        P(bishopByRank_1, -133),
        P(bishopByRank_2, -119),
        P(bishopByRank_3, -94),
        P(bishopByRank_4, -63),
        P(bishopByRank_5, 30),
        P(bishopByRank_6, 9), // -rank8
#else
       P(bishopBySquare_0, -184),
        P(bishopBySquare_1, 9),
        P(bishopBySquare_2, -89),
        P(bishopBySquare_3, 53),
        P(bishopBySquare_4, -72),
        P(bishopBySquare_5, -34),
        P(bishopBySquare_6, -75),
        P(bishopBySquare_7, 53),

        P(bishopBySquare_8, -34),
        P(bishopBySquare_9, -61),
        P(bishopBySquare_10, 69),
        P(bishopBySquare_11, -21),
        P(bishopBySquare_12, -1),
        P(bishopBySquare_13, 58),
        P(bishopBySquare_14, 40),
        P(bishopBySquare_15, 110),

        P(bishopBySquare_16, -121),
        P(bishopBySquare_17, 48),
        P(bishopBySquare_18, 12),
        P(bishopBySquare_19, 41),
        P(bishopBySquare_20, 38),
        P(bishopBySquare_21, 43),
        P(bishopBySquare_22, 31),
        P(bishopBySquare_23, 100),

        P(bishopBySquare_24, -114),
        P(bishopBySquare_25, -62),
        P(bishopBySquare_26, 47),
        P(bishopBySquare_27, 73),
        P(bishopBySquare_28, 140),
        P(bishopBySquare_29, 95),
        P(bishopBySquare_30, 81),
        P(bishopBySquare_31, 80),

        P(bishopBySquare_32, -117),
        P(bishopBySquare_33, -12),
        P(bishopBySquare_34, 6),
        P(bishopBySquare_35, 67),
        P(bishopBySquare_36, 87),
        P(bishopBySquare_37, 149),
        P(bishopBySquare_38, 28),
        P(bishopBySquare_39, 84),

        P(bishopBySquare_40, -88),
        P(bishopBySquare_41, -47),
        P(bishopBySquare_42, 15),
        P(bishopBySquare_43, -26),
        P(bishopBySquare_44, 122),
        P(bishopBySquare_45, 242),
        P(bishopBySquare_46, 74),
        P(bishopBySquare_47, 2),

        P(bishopBySquare_48, -88),
        P(bishopBySquare_49, 21),
        P(bishopBySquare_50, 11),
        P(bishopBySquare_51, 13),
        P(bishopBySquare_52, -14),
        P(bishopBySquare_53, 185),
        P(bishopBySquare_54, -51),
        P(bishopBySquare_55, -6),

        P(bishopBySquare_56, -239),
        P(bishopBySquare_57, -73),
        P(bishopBySquare_58, -6),
        P(bishopBySquare_59, 35),
        P(bishopBySquare_60, 29),
        P(bishopBySquare_61, 180),
        P(bishopBySquare_62, -187),
        P(bishopBySquare_63, -48),

        P(bishopBySquare_0x, -140),
        P(bishopBySquare_1x, -50),
        P(bishopBySquare_2x, 21),
        P(bishopBySquare_3x, -18),
        P(bishopBySquare_4x, 2),
        P(bishopBySquare_5x, 117),
        P(bishopBySquare_6x, -210),
        P(bishopBySquare_7x, -91),

        P(bishopBySquare_8x, -93),
        P(bishopBySquare_9x, -109),
        P(bishopBySquare_10x, -34),
        P(bishopBySquare_11x, 27),
        P(bishopBySquare_12x, 41),
        P(bishopBySquare_13x, 120),
        P(bishopBySquare_14x, 16),
        P(bishopBySquare_15x, -24),

        P(bishopBySquare_16x, -145),
        P(bishopBySquare_17x, -24),
        P(bishopBySquare_18x, 13),
        P(bishopBySquare_19x, 23),
        P(bishopBySquare_20x, 121),
        P(bishopBySquare_21x, 153),
        P(bishopBySquare_22x, 82),
        P(bishopBySquare_23x, 28),

        P(bishopBySquare_24x, -124),
        P(bishopBySquare_25x, -37),
        P(bishopBySquare_26x, 55),
        P(bishopBySquare_27x, 62),
        P(bishopBySquare_28x, 110),
        P(bishopBySquare_29x, 183),
        P(bishopBySquare_30x, 94),
        P(bishopBySquare_31x, 122),

        P(bishopBySquare_32x, -80),
        P(bishopBySquare_33x, 34),
        P(bishopBySquare_34x, 75),
        P(bishopBySquare_35x, 113),
        P(bishopBySquare_36x, 155),
        P(bishopBySquare_37x, 151),
        P(bishopBySquare_38x, 141),
        P(bishopBySquare_39x, 91),

        P(bishopBySquare_40x, -112),
        P(bishopBySquare_41x, 59),
        P(bishopBySquare_42x, 111),
        P(bishopBySquare_43x, 135),
        P(bishopBySquare_44x, 136),
        P(bishopBySquare_45x, 117),
        P(bishopBySquare_46x, 117),
        P(bishopBySquare_47x, 134),

        P(bishopBySquare_48x, -7),
        P(bishopBySquare_49x, 100),
        P(bishopBySquare_50x, 116),
        P(bishopBySquare_51x, 63),
        P(bishopBySquare_52x, 100),
        P(bishopBySquare_53x, 149),
        P(bishopBySquare_54x, 114),
        P(bishopBySquare_55x, 126),

        P(bishopBySquare_56x, -111),
        P(bishopBySquare_57x, 54),
        P(bishopBySquare_58x, 41),
        P(bishopBySquare_59x, 61),
        P(bishopBySquare_60x, 63),
        P(bishopBySquare_61x, 24),
        P(bishopBySquare_62x, -38),
        P(bishopBySquare_63x, -39),
#endif

        /*
         *  Rooks
         */

        P(rookByFile_0, -71), // fileA
        P(rookByFile_1, -89),
        P(rookByFile_2, -58),
        P(rookByFile_3, -11),
        P(rookByFile_4, 35),
        P(rookByFile_5, 52),
        P(rookByFile_6, 104), // -fileH

        P(rookByFile_0x, -82), // fileA
        P(rookByFile_1x, -51),
        P(rookByFile_2x, -40),
        P(rookByFile_3x, -25),
        P(rookByFile_4x, 22),
        P(rookByFile_5x, 55),
        P(rookByFile_6x, 70), // -fileH

        P(rookByRank_0, -140), // rank1
        P(rookByRank_1, -337),
        P(rookByRank_2, -491),
        P(rookByRank_3, -546),
        P(rookByRank_4, -449),
        P(rookByRank_5, -281),
        P(rookByRank_6, -114), // -rank8

        /*
         *  Queens
         */

        P(queenByFile_0, -87), // fileA
        P(queenByFile_1, -147),
        P(queenByFile_2, -151),
        P(queenByFile_3, -139),
        P(queenByFile_4, -128),
        P(queenByFile_5, -104),
        P(queenByFile_6, -69), // -fileH

        P(queenByFile_0x, 17), // fileA
        P(queenByFile_1x, 5),
        P(queenByFile_2x, -4),
        P(queenByFile_3x, 21),
        P(queenByFile_4x, 66),
        P(queenByFile_5x, 110),
        P(queenByFile_6x, 79), // -fileH

        P(queenByRank_0, -164), // rank1
        P(queenByRank_1, -284),
        P(queenByRank_2, -382),
        P(queenByRank_3, -435),
        P(queenByRank_4, -383),
        P(queenByRank_5, -239),
        P(queenByRank_6, -148), // -rank8

        /*
         *  Kings
         */

        P(kingByFile_0, 1), // fileD
        P(kingByFile_1, -19),
        P(kingByFile_2, 11), // -fileH

        P(kingByRank_0, -163), // rank1
        P(kingByRank_1, -299),
        P(kingByRank_2, -404),
        P(kingByRank_3, -406),
        P(kingByRank_4, -264),
        P(kingByRank_5, -18),
        P(kingByRank_6, 157), // -rank8

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
        P(drawOffset, 221),
        P(drawQueen, -1937),
        P(drawRook, 103),
        P(drawBishop, -25),
        P(drawKnight, -7),
        P(drawPawn, -795),
        P(drawQueenEnding, 1312),
        P(drawRookEnding, 399),
        P(drawKnightEnding, 511),
        P(drawBishopEnding, 1450),
        P(drawPawnEnding, -14802),
        P(drawPawnless, 292),

        P(drawQueenImbalance, 829),
        P(drawRookImbalance, 263),
        P(drawMinorImbalance, 52),

        P(drawUnlikeBishops, 3764),
        P(drawUnlikeBishopsAndQueens, -389),
        P(drawUnlikeBishopsAndRooks, 397),
        P(drawUnlikeBishopsAndKnights, 878),
        // TODO: pawn and pawn block square colors


// }

/*----------------------------------------------------------------------+
 |                                                                      |
 +----------------------------------------------------------------------*/

