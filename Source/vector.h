
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
        P(tempo, 37),
        P(winBonus, 3487),

        P(castleK, -86),
        P(castleQ, -83),
        P(castleKQ, 15),

        P(queenValue, 7158),
        P(rookValue, 4054),
        P(bishopValue, 2293),
        P(knightValue, 2104),
        P(pawnValue1, 1057), P(pawnValue2, 1737), P(pawnValue3, 2388), P(pawnValue4, 3038),
        P(pawnValue5, 3687), P(pawnValue6, 4321), P(pawnValue7, 4945), P(pawnValue8, 5529),

        P(queenAndQueen, -1000),
        P(queenAndRook, -663),
        P(queenAndBishop, 31),
        P(queenAndKnight, -60),
        P(queenAndPawn_1, -91), P(queenAndPawn_2, -86),
        P(rookAndRook, -315),
        P(rookAndBishop, -144),
        P(rookAndKnight, -166),
        P(rookAndPawn_1, -55), P(rookAndPawn_2, -62),
        P(bishopAndBishop, 39),
        P(bishopAndKnight, -62),
        P(bishopAndPawn_1, -56), P(bishopAndPawn_2, -19),
        P(knightAndKnight, -92),
        P(knightAndPawn_1, -27), P(knightAndPawn_2, -1),

        P(queenVsRook, -241),
        P(queenVsBishop, 84),
        P(queenVsKnight, -6),
        P(queenVsPawn_1, 32), P(queenVsPawn_2, -95),
        P(rookVsBishop, 4),
        P(rookVsKnight, 26),
        P(rookVsPawn_1, -22), P(rookVsPawn_2, -30),
        P(bishopVsKnight, 8),
        P(bishopVsPawn_1, -11), P(bishopVsPawn_2, -5),
        P(knightVsPawn_1, -1), P(knightVsPawn_2, -18),

        P(controlCenter, 39),
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

        P(pawnByFile_0, -8), // fileA
        P(pawnByFile_1, -18),
        P(pawnByFile_2, -90),
        P(pawnByFile_3, -123),
        P(pawnByFile_4, -139),
        P(pawnByFile_5, -75),
        P(pawnByFile_6, 24), // -fileH

        P(pawnByFile_0x, -8), // fileA
        P(pawnByFile_1x, -18),
        P(pawnByFile_2x, -90),
        P(pawnByFile_3x, -123),
        P(pawnByFile_4x, -139),
        P(pawnByFile_5x, -75),
        P(pawnByFile_6x, 24), // -fileH

        // 5 parameters for 6 pawn ranks
        P(pawnByRank_0, -31), // "rank2"
        P(pawnByRank_1, -73),
        P(pawnByRank_2, -89),
        P(pawnByRank_3, -46),
        P(pawnByRank_4, 67), // "-rank7"

        P(doubledPawnA, -224), P(doubledPawnB, -116), P(doubledPawnC, -158), P(doubledPawnD, -128),
        P(doubledPawnE, -107), P(doubledPawnF, -45), P(doubledPawnG, -4), P(doubledPawnH, -223),

        P(backwardPawnA, -15), P(backwardPawnB, -47), P(backwardPawnC, -59), P(backwardPawnD, -105),
        P(backwardPawnE, -81), P(backwardPawnF, -40), P(backwardPawnG, -13), P(backwardPawnH, 28),

        P(isolatedPawnClosedA, 46), P(isolatedPawnClosedB, -79),
        P(isolatedPawnClosedC, -59), P(isolatedPawnClosedD, 7),
        P(isolatedPawnClosedE, -26), P(isolatedPawnClosedF, -48),
        P(isolatedPawnClosedG, -56), P(isolatedPawnClosedH, 5),

        P(isolatedPawnOpenA, 0), P(isolatedPawnOpenB, -54),
        P(isolatedPawnOpenC, -73), P(isolatedPawnOpenD, -125),
        P(isolatedPawnOpenE, -115), P(isolatedPawnOpenF, -96),
        P(isolatedPawnOpenG, -77), P(isolatedPawnOpenH, 23),

        P(sidePawnClosedA, 2), P(sidePawnClosedB, -24),
        P(sidePawnClosedC, -7), P(sidePawnClosedD, -33),
        P(sidePawnClosedE, -14), P(sidePawnClosedF, -2),
        P(sidePawnClosedG, -24), P(sidePawnClosedH, 3),

        P(sidePawnOpenA, -8), P(sidePawnOpenB, 37),
        P(sidePawnOpenC, 11), P(sidePawnOpenD, -31),
        P(sidePawnOpenE, -16), P(sidePawnOpenF, 7),
        P(sidePawnOpenG, 25), P(sidePawnOpenH, 53),

        P(middlePawnClosedA, 0), P(middlePawnClosedB, -5),
        P(middlePawnClosedC, 8), P(middlePawnClosedD, -16),
        P(middlePawnClosedE, 21), P(middlePawnClosedF, 12),
        P(middlePawnClosedG, -14), P(middlePawnClosedH, 0),

        P(middlePawnOpenA, 0), P(middlePawnOpenB, 21),
        P(middlePawnOpenC, 17), P(middlePawnOpenD, 36),
        P(middlePawnOpenE, 52), P(middlePawnOpenF, -1),
        P(middlePawnOpenG, 3), P(middlePawnOpenH, 0),

        P(duoPawnA, -20), P(duoPawnB, 9),
        P(duoPawnC, -12), P(duoPawnD, 60),
        P(duoPawnE, 45), P(duoPawnF, 10),
        P(duoPawnG, 59), P(duoPawnH, -73),

        // Quadratic polynomials for passers
        P(passerA_0, -85), P(passerB_0, 9), P(passerC_0, 16), P(passerD_0, -53),
        P(passerE_0, 52), P(passerF_0, 64), P(passerG_0, 21), P(passerH_0, 49),

        P(passerA_1, 98), P(passerB_1, 31), P(passerC_1, -12), P(passerD_1, 0),
        P(passerE_1, -47), P(passerF_1, -73), P(passerG_1, -22), P(passerH_1, 17),

        P(passerA_2, 152), P(passerB_2, 217), P(passerC_2, 249), P(passerD_2, 215),
        P(passerE_2, 227), P(passerF_2, 251), P(passerG_2, 198), P(passerH_2, 148),

        P(passerScalingOffset, 4856),
        P(passerAndQueen, -430),
        P(passerAndRook, -160),
        P(passerAndBishop, 159),
        P(passerAndKnight, -17),
        P(passerAndPawn, -643),
        P(passerVsQueen, 1025),
        P(passerVsRook, 33),
        P(passerVsBishop, -736),
        P(passerVsKnight, -521),
        P(passerVsPawn, 52),

#if 0
        P(drawBlockedA, 0), P(drawBlockedB, 0), P(drawBlockedC, 0), P(drawBlockedD, 0),
        P(drawBlockedE, 0), P(drawBlockedF, 0), P(drawBlockedG, 0), P(drawBlockedH, 0),

        P(drawPasser_0, 0), P(drawPasser_1, 0), P(drawPasser_2, 0),
#endif

        /*
         *  Knights
         */

        // Kings on same side
        P(knightByFile_0, -179), // fileA
        P(knightByFile_1, -220),
        P(knightByFile_2, -234),
        P(knightByFile_3, -168),
        P(knightByFile_4, -95),
        P(knightByFile_5, -20),
        P(knightByFile_6, 34), // -fileH

        // Opposite side kings
        P(knightByFile_0x, -93), // fileA
        P(knightByFile_1x, -110),
        P(knightByFile_2x, -101),
        P(knightByFile_3x, -20),
        P(knightByFile_4x, 53),
        P(knightByFile_5x, 123),
        P(knightByFile_6x, 99), // -fileH

        P(knightByRank_0, -103), // rank1
        P(knightByRank_1, -187),
        P(knightByRank_2, -218),
        P(knightByRank_3, -159),
        P(knightByRank_4, -43),
        P(knightByRank_5, 104),
        P(knightByRank_6, 164), // -rank8

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
       P(bishopBySquare_0, -175),
        P(bishopBySquare_1, 15),
        P(bishopBySquare_2, -88),
        P(bishopBySquare_3, 53),
        P(bishopBySquare_4, -77),
        P(bishopBySquare_5, -37),
        P(bishopBySquare_6, -84),
        P(bishopBySquare_7, 40),

        P(bishopBySquare_8, -16),
        P(bishopBySquare_9, -57),
        P(bishopBySquare_10, 71),
        P(bishopBySquare_11, -27),
        P(bishopBySquare_12, -8),
        P(bishopBySquare_13, 41),
        P(bishopBySquare_14, 20),
        P(bishopBySquare_15, 91),

        P(bishopBySquare_16, -110),
        P(bishopBySquare_17, 56),
        P(bishopBySquare_18, 19),
        P(bishopBySquare_19, 46),
        P(bishopBySquare_20, 26),
        P(bishopBySquare_21, 29),
        P(bishopBySquare_22, 20),
        P(bishopBySquare_23, 89),

        P(bishopBySquare_24, -106),
        P(bishopBySquare_25, -56),
        P(bishopBySquare_26, 51),
        P(bishopBySquare_27, 64),
        P(bishopBySquare_28, 125),
        P(bishopBySquare_29, 83),
        P(bishopBySquare_30, 76),
        P(bishopBySquare_31, 84),

        P(bishopBySquare_32, -118),
        P(bishopBySquare_33, -4),
        P(bishopBySquare_34, 10),
        P(bishopBySquare_35, 71),
        P(bishopBySquare_36, 88),
        P(bishopBySquare_37, 155),
        P(bishopBySquare_38, 29),
        P(bishopBySquare_39, 89),

        P(bishopBySquare_40, -83),
        P(bishopBySquare_41, -49),
        P(bishopBySquare_42, 16),
        P(bishopBySquare_43, -20),
        P(bishopBySquare_44, 136),
        P(bishopBySquare_45, 258),
        P(bishopBySquare_46, 83),
        P(bishopBySquare_47, 6),

        P(bishopBySquare_48, -96),
        P(bishopBySquare_49, 21),
        P(bishopBySquare_50, 12),
        P(bishopBySquare_51, 21),
        P(bishopBySquare_52, -1),
        P(bishopBySquare_53, 213),
        P(bishopBySquare_54, -30),
        P(bishopBySquare_55, -1),

        P(bishopBySquare_56, -244),
        P(bishopBySquare_57, -75),
        P(bishopBySquare_58, -1),
        P(bishopBySquare_59, 47),
        P(bishopBySquare_60, 48),
        P(bishopBySquare_61, 198),
        P(bishopBySquare_62, -172),
        P(bishopBySquare_63, -26),

        P(bishopBySquare_0x, -121),
        P(bishopBySquare_1x, -35),
        P(bishopBySquare_2x, 44),
        P(bishopBySquare_3x, 4),
        P(bishopBySquare_4x, 34),
        P(bishopBySquare_5x, 165),
        P(bishopBySquare_6x, -179),
        P(bishopBySquare_7x, -53),

        P(bishopBySquare_8x, -77),
        P(bishopBySquare_9x, -129),
        P(bishopBySquare_10x, -41),
        P(bishopBySquare_11x, 52),
        P(bishopBySquare_12x, 68),
        P(bishopBySquare_13x, 148),
        P(bishopBySquare_14x, 45),
        P(bishopBySquare_15x, -24),

        P(bishopBySquare_16x, -141),
        P(bishopBySquare_17x, -22),
        P(bishopBySquare_18x, -2),
        P(bishopBySquare_19x, 19),
        P(bishopBySquare_20x, 133),
        P(bishopBySquare_21x, 162),
        P(bishopBySquare_22x, 92),
        P(bishopBySquare_23x, 36),

        P(bishopBySquare_24x, -128),
        P(bishopBySquare_25x, -35),
        P(bishopBySquare_26x, 77),
        P(bishopBySquare_27x, 56),
        P(bishopBySquare_28x, 95),
        P(bishopBySquare_29x, 190),
        P(bishopBySquare_30x, 88),
        P(bishopBySquare_31x, 120),

        P(bishopBySquare_32x, -77),
        P(bishopBySquare_33x, 21),
        P(bishopBySquare_34x, 80),
        P(bishopBySquare_35x, 100),
        P(bishopBySquare_36x, 128),
        P(bishopBySquare_37x, 132),
        P(bishopBySquare_38x, 139),
        P(bishopBySquare_39x, 105),

        P(bishopBySquare_40x, -113),
        P(bishopBySquare_41x, 44),
        P(bishopBySquare_42x, 91),
        P(bishopBySquare_43x, 113),
        P(bishopBySquare_44x, 94),
        P(bishopBySquare_45x, 87),
        P(bishopBySquare_46x, 99),
        P(bishopBySquare_47x, 139),

        P(bishopBySquare_48x, -24),
        P(bishopBySquare_49x, 92),
        P(bishopBySquare_50x, 98),
        P(bishopBySquare_51x, 14),
        P(bishopBySquare_52x, 77),
        P(bishopBySquare_53x, 128),
        P(bishopBySquare_54x, 91),
        P(bishopBySquare_55x, 126),

        P(bishopBySquare_56x, -147),
        P(bishopBySquare_57x, 32),
        P(bishopBySquare_58x, 16),
        P(bishopBySquare_59x, 21),
        P(bishopBySquare_60x, 19),
        P(bishopBySquare_61x, -6),
        P(bishopBySquare_62x, -62),
        P(bishopBySquare_63x, -65),
#endif

        /*
         *  Rooks
         */

        P(rookByFile_0, -76), // fileA
        P(rookByFile_1, -79),
        P(rookByFile_2, -50),
        P(rookByFile_3, -8),
        P(rookByFile_4, 40),
        P(rookByFile_5, 56),
        P(rookByFile_6, 91), // -fileH

        P(rookByFile_0x, -76), // fileA
        P(rookByFile_1x, -79),
        P(rookByFile_2x, -50),
        P(rookByFile_3x, -8),
        P(rookByFile_4x, 40),
        P(rookByFile_5x, 56),
        P(rookByFile_6x, 91), // -fileH

        P(rookByRank_0, -138), // rank1
        P(rookByRank_1, -337),
        P(rookByRank_2, -490),
        P(rookByRank_3, -545),
        P(rookByRank_4, -447),
        P(rookByRank_5, -277),
        P(rookByRank_6, -112), // -rank8

        /*
         *  Queens
         */

        P(queenByFile_0, -77), // fileA
        P(queenByFile_1, -128),
        P(queenByFile_2, -128),
        P(queenByFile_3, -114),
        P(queenByFile_4, -93),
        P(queenByFile_5, -72),
        P(queenByFile_6, -48), // -fileH

        P(queenByFile_0x, -77), // fileA
        P(queenByFile_1x, -128),
        P(queenByFile_2x, -128),
        P(queenByFile_3x, -114),
        P(queenByFile_4x, -93),
        P(queenByFile_5x, -72),
        P(queenByFile_6x, -48), // -fileH

        P(queenByRank_0, -162), // rank1
        P(queenByRank_1, -280),
        P(queenByRank_2, -377),
        P(queenByRank_3, -428),
        P(queenByRank_4, -373),
        P(queenByRank_5, -227),
        P(queenByRank_6, -144), // -rank8

        /*
         *  Kings
         */

        P(kingByFile_0, -6), // fileD
        P(kingByFile_1, -27),
        P(kingByFile_2, 10), // -fileH

        P(kingByRank_0, -158), // rank1
        P(kingByRank_1, -293),
        P(kingByRank_2, -403),
        P(kingByRank_3, -411),
        P(kingByRank_4, -283),
        P(kingByRank_5, -41),
        P(kingByRank_6, 135), // -rank8

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
        P(drawOffset, 214),
        P(drawQueen, -1998),
        P(drawRook, 108),
        P(drawBishop, -41),
        P(drawKnight, -16),
        P(drawPawn, -794),
        P(drawQueenEnding, 1340),
        P(drawRookEnding, 382),
        P(drawKnightEnding, 526),
        P(drawBishopEnding, 1483),
        P(drawPawnEnding, -17303),
        P(drawPawnless, 306),

        P(drawQueenImbalance, 875),
        P(drawRookImbalance, 284),
        P(drawMinorImbalance, 37),

        P(drawUnlikeBishops, 3725),
        P(drawUnlikeBishopsAndQueens, -372),
        P(drawUnlikeBishopsAndRooks, 415),
        P(drawUnlikeBishopsAndKnights, 921),
        // TODO: pawn and pawn block square colors


// }

/*----------------------------------------------------------------------+
 |                                                                      |
 +----------------------------------------------------------------------*/

