
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
        P(winBonus, 3492),

        P(castleK, -93),
        P(castleQ, -81),
        P(castleKQ, 10),

        P(queenValue, 7174),
        P(rookValue, 4052),
        P(bishopValue, 2284),
        P(knightValue, 2087),
        P(pawnValue1, 1066), P(pawnValue2, 1741), P(pawnValue3, 2386), P(pawnValue4, 3030),
        P(pawnValue5, 3684), P(pawnValue6, 4328), P(pawnValue7, 4962), P(pawnValue8, 5562),

        P(queenAndQueen, -992),
        P(queenAndRook, -663),
        P(queenAndBishop, 28),
        P(queenAndKnight, -62),
        P(queenAndPawn_1, -87), P(queenAndPawn_2, -86),
        P(rookAndRook, -315),
        P(rookAndBishop, -143),
        P(rookAndKnight, -167),
        P(rookAndPawn_1, -54), P(rookAndPawn_2, -62),
        P(bishopAndBishop, 40),
        P(bishopAndKnight, -54),
        P(bishopAndPawn_1, -56), P(bishopAndPawn_2, -19),
        P(knightAndKnight, -92),
        P(knightAndPawn_1, -27), P(knightAndPawn_2, -1),

        P(queenVsRook, -245),
        P(queenVsBishop, 83),
        P(queenVsKnight, -5),
        P(queenVsPawn_1, 28), P(queenVsPawn_2, -97),
        P(rookVsBishop, 6),
        P(rookVsKnight, 26),
        P(rookVsPawn_1, -22), P(rookVsPawn_2, -28),
        P(bishopVsKnight, 6),
        P(bishopVsPawn_1, -11), P(bishopVsPawn_2, -3),
        P(knightVsPawn_1, 0), P(knightVsPawn_2, -18),

        P(controlCenter, 40),
        P(controlExtendedCenter, 42),
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

        P(pawnByFile_0, -2), // fileA
        P(pawnByFile_1, -9),
        P(pawnByFile_2, -86),
        P(pawnByFile_3, -124),
        P(pawnByFile_4, -143),
        P(pawnByFile_5, -75),
        P(pawnByFile_6, 23), // -fileH

        P(pawnByFile_0x, -66), // fileA
        P(pawnByFile_1x, -67),
        P(pawnByFile_2x, -91),
        P(pawnByFile_3x, -136),
        P(pawnByFile_4x, -137),
        P(pawnByFile_5x, -76),
        P(pawnByFile_6x, -23), // -fileH

        // 5 parameters for 6 pawn ranks
        P(pawnByRank_0, -28), // "rank2"
        P(pawnByRank_1, -68),
        P(pawnByRank_2, -83),
        P(pawnByRank_3, -42),
        P(pawnByRank_4, 71), // "-rank7"

        P(doubledPawnA, -217), P(doubledPawnB, -132), P(doubledPawnC, -174), P(doubledPawnD, -127),
        P(doubledPawnE, -121), P(doubledPawnF, -38), P(doubledPawnG, -3), P(doubledPawnH, -214),

        P(backwardPawnA, 3), P(backwardPawnB, -42), P(backwardPawnC, -48), P(backwardPawnD, -91),
        P(backwardPawnE, -98), P(backwardPawnF, -55), P(backwardPawnG, -24), P(backwardPawnH, 8),

        P(isolatedPawnClosedA, 44), P(isolatedPawnClosedB, -69),
        P(isolatedPawnClosedC, -38), P(isolatedPawnClosedD, 21),
        P(isolatedPawnClosedE, -33), P(isolatedPawnClosedF, -64),
        P(isolatedPawnClosedG, -64), P(isolatedPawnClosedH, 4),

        P(isolatedPawnOpenA, -8), P(isolatedPawnOpenB, -44),
        P(isolatedPawnOpenC, -79), P(isolatedPawnOpenD, -124),
        P(isolatedPawnOpenE, -104), P(isolatedPawnOpenF, -71),
        P(isolatedPawnOpenG, -54), P(isolatedPawnOpenH, 20),

        P(sidePawnClosedA, -16), P(sidePawnClosedB, -11),
        P(sidePawnClosedC, 18), P(sidePawnClosedD, -14),
        P(sidePawnClosedE, -3), P(sidePawnClosedF, -6),
        P(sidePawnClosedG, -31), P(sidePawnClosedH, 36),

        P(sidePawnOpenA, -24), P(sidePawnOpenB, 41),
        P(sidePawnOpenC, -3), P(sidePawnOpenD, -38),
        P(sidePawnOpenE, -12), P(sidePawnOpenF, 11),
        P(sidePawnOpenG, 27), P(sidePawnOpenH, 53),

        P(middlePawnClosedA, 0), P(middlePawnClosedB, -2),
        P(middlePawnClosedC, 35), P(middlePawnClosedD, 4),
        P(middlePawnClosedE, 15), P(middlePawnClosedF, 1),
        P(middlePawnClosedG, -17), P(middlePawnClosedH, 0),

        P(middlePawnOpenA, 0), P(middlePawnOpenB, 14),
        P(middlePawnOpenC, 26), P(middlePawnOpenD, 32),
        P(middlePawnOpenE, 48), P(middlePawnOpenF, -2),
        P(middlePawnOpenG, -10), P(middlePawnOpenH, 0),

        P(duoPawnA, -25), P(duoPawnB, 4),
        P(duoPawnC, -27), P(duoPawnD, 61),
        P(duoPawnE, 42), P(duoPawnF, 12),
        P(duoPawnG, 53), P(duoPawnH, -71),

        // Quadratic polynomials for passers
        P(passerA_0, -78), P(passerB_0, -31), P(passerC_0, -8), P(passerD_0, -57),
        P(passerE_0, 50), P(passerF_0, 53), P(passerG_0, 41), P(passerH_0, 46),

        P(passerA_1, 85), P(passerB_1, 15), P(passerC_1, -22), P(passerD_1, -1),
        P(passerE_1, -45), P(passerF_1, -54), P(passerG_1, 3), P(passerH_1, 32),

        P(passerA_2, 154), P(passerB_2, 217), P(passerC_2, 240), P(passerD_2, 210),
        P(passerE_2, 235), P(passerF_2, 269), P(passerG_2, 222), P(passerH_2, 166),

        P(passerScalingOffset, 4932),
        P(passerAndQueen, -340),
        P(passerAndRook, -277),
        P(passerAndBishop, 113),
        P(passerAndKnight, 72),
        P(passerAndPawn, -637),
        P(passerVsQueen, 1135),
        P(passerVsRook, 32),
        P(passerVsBishop, -770),
        P(passerVsKnight, -550),
        P(passerVsPawn, 49),

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
        P(knightByFile_3, -165),
        P(knightByFile_4, -94),
        P(knightByFile_5, -26),
        P(knightByFile_6, 26), // -fileH

        // Opposite side kings
        P(knightByFile_0x, -95), // fileA
        P(knightByFile_1x, -124),
        P(knightByFile_2x, -110),
        P(knightByFile_3x, -18),
        P(knightByFile_4x, 50),
        P(knightByFile_5x, 130),
        P(knightByFile_6x, 102), // -fileH

        P(knightByRank_0, -103), // rank1
        P(knightByRank_1, -189),
        P(knightByRank_2, -216),
        P(knightByRank_3, -149),
        P(knightByRank_4, -32),
        P(knightByRank_5, 120),
        P(knightByRank_6, 188), // -rank8

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
       P(bishopBySquare_0, -181),
        P(bishopBySquare_1, 8),
        P(bishopBySquare_2, -85),
        P(bishopBySquare_3, 58),
        P(bishopBySquare_4, -64),
        P(bishopBySquare_5, -30),
        P(bishopBySquare_6, -70),
        P(bishopBySquare_7, 53),

        P(bishopBySquare_8, -33),
        P(bishopBySquare_9, -57),
        P(bishopBySquare_10, 71),
        P(bishopBySquare_11, -15),
        P(bishopBySquare_12, 2),
        P(bishopBySquare_13, 61),
        P(bishopBySquare_14, 36),
        P(bishopBySquare_15, 110),

        P(bishopBySquare_16, -114),
        P(bishopBySquare_17, 47),
        P(bishopBySquare_18, 13),
        P(bishopBySquare_19, 44),
        P(bishopBySquare_20, 38),
        P(bishopBySquare_21, 41),
        P(bishopBySquare_22, 33),
        P(bishopBySquare_23, 106),

        P(bishopBySquare_24, -112),
        P(bishopBySquare_25, -59),
        P(bishopBySquare_26, 49),
        P(bishopBySquare_27, 72),
        P(bishopBySquare_28, 136),
        P(bishopBySquare_29, 95),
        P(bishopBySquare_30, 84),
        P(bishopBySquare_31, 83),

        P(bishopBySquare_32, -113),
        P(bishopBySquare_33, -9),
        P(bishopBySquare_34, 8),
        P(bishopBySquare_35, 64),
        P(bishopBySquare_36, 87),
        P(bishopBySquare_37, 146),
        P(bishopBySquare_38, 28),
        P(bishopBySquare_39, 85),

        P(bishopBySquare_40, -84),
        P(bishopBySquare_41, -45),
        P(bishopBySquare_42, 12),
        P(bishopBySquare_43, -24),
        P(bishopBySquare_44, 122),
        P(bishopBySquare_45, 238),
        P(bishopBySquare_46, 70),
        P(bishopBySquare_47, 4),

        P(bishopBySquare_48, -85),
        P(bishopBySquare_49, 19),
        P(bishopBySquare_50, 10),
        P(bishopBySquare_51, 12),
        P(bishopBySquare_52, -11),
        P(bishopBySquare_53, 185),
        P(bishopBySquare_54, -55),
        P(bishopBySquare_55, -9),

        P(bishopBySquare_56, -237),
        P(bishopBySquare_57, -73),
        P(bishopBySquare_58, -7),
        P(bishopBySquare_59, 35),
        P(bishopBySquare_60, 31),
        P(bishopBySquare_61, 184),
        P(bishopBySquare_62, -195),
        P(bishopBySquare_63, -55),

        P(bishopBySquare_0x, -143),
        P(bishopBySquare_1x, -50),
        P(bishopBySquare_2x, 17),
        P(bishopBySquare_3x, -14),
        P(bishopBySquare_4x, 16),
        P(bishopBySquare_5x, 124),
        P(bishopBySquare_6x, -194),
        P(bishopBySquare_7x, -90),

        P(bishopBySquare_8x, -103),
        P(bishopBySquare_9x, -115),
        P(bishopBySquare_10x, -46),
        P(bishopBySquare_11x, 32),
        P(bishopBySquare_12x, 31),
        P(bishopBySquare_13x, 123),
        P(bishopBySquare_14x, 14),
        P(bishopBySquare_15x, -29),

        P(bishopBySquare_16x, -145),
        P(bishopBySquare_17x, -30),
        P(bishopBySquare_18x, 10),
        P(bishopBySquare_19x, 14),
        P(bishopBySquare_20x, 124),
        P(bishopBySquare_21x, 146),
        P(bishopBySquare_22x, 75),
        P(bishopBySquare_23x, 26),

        P(bishopBySquare_24x, -122),
        P(bishopBySquare_25x, -40),
        P(bishopBySquare_26x, 49),
        P(bishopBySquare_27x, 56),
        P(bishopBySquare_28x, 96),
        P(bishopBySquare_29x, 175),
        P(bishopBySquare_30x, 89),
        P(bishopBySquare_31x, 121),

        P(bishopBySquare_32x, -82),
        P(bishopBySquare_33x, 29),
        P(bishopBySquare_34x, 71),
        P(bishopBySquare_35x, 101),
        P(bishopBySquare_36x, 143),
        P(bishopBySquare_37x, 142),
        P(bishopBySquare_38x, 140),
        P(bishopBySquare_39x, 96),

        P(bishopBySquare_40x, -116),
        P(bishopBySquare_41x, 54),
        P(bishopBySquare_42x, 108),
        P(bishopBySquare_43x, 132),
        P(bishopBySquare_44x, 128),
        P(bishopBySquare_45x, 108),
        P(bishopBySquare_46x, 107),
        P(bishopBySquare_47x, 134),

        P(bishopBySquare_48x, -17),
        P(bishopBySquare_49x, 90),
        P(bishopBySquare_50x, 105),
        P(bishopBySquare_51x, 54),
        P(bishopBySquare_52x, 93),
        P(bishopBySquare_53x, 132),
        P(bishopBySquare_54x, 113),
        P(bishopBySquare_55x, 126),

        P(bishopBySquare_56x, -119),
        P(bishopBySquare_57x, 45),
        P(bishopBySquare_58x, 28),
        P(bishopBySquare_59x, 46),
        P(bishopBySquare_60x, 60),
        P(bishopBySquare_61x, 13),
        P(bishopBySquare_62x, -62),
        P(bishopBySquare_63x, -48),
#endif

        /*
         *  Rooks
         */

        P(rookByFile_0, -69), // fileA
        P(rookByFile_1, -84),
        P(rookByFile_2, -54),
        P(rookByFile_3, -10),
        P(rookByFile_4, 35),
        P(rookByFile_5, 51),
        P(rookByFile_6, 103), // -fileH

        P(rookByFile_0x, -85), // fileA
        P(rookByFile_1x, -54),
        P(rookByFile_2x, -34),
        P(rookByFile_3x, -14),
        P(rookByFile_4x, 31),
        P(rookByFile_5x, 62),
        P(rookByFile_6x, 71), // -fileH

        P(rookByRank_0, -140), // rank1
        P(rookByRank_1, -336),
        P(rookByRank_2, -491),
        P(rookByRank_3, -545),
        P(rookByRank_4, -447),
        P(rookByRank_5, -279),
        P(rookByRank_6, -115), // -rank8

        /*
         *  Queens
         */

        P(queenByFile_0, -80), // fileA
        P(queenByFile_1, -129),
        P(queenByFile_2, -127),
        P(queenByFile_3, -112),
        P(queenByFile_4, -103),
        P(queenByFile_5, -84),
        P(queenByFile_6, -59), // -fileH

        P(queenByFile_0x, -26), // fileA
        P(queenByFile_1x, -55),
        P(queenByFile_2x, -77),
        P(queenByFile_3x, -85),
        P(queenByFile_4x, -26),
        P(queenByFile_5x, 39),
        P(queenByFile_6x, 38), // -fileH

        P(queenByRank_0, -155), // rank1
        P(queenByRank_1, -269),
        P(queenByRank_2, -362),
        P(queenByRank_3, -413),
        P(queenByRank_4, -359),
        P(queenByRank_5, -223),
        P(queenByRank_6, -140), // -rank8

        /*
         *  Kings
         */

        P(kingByFile_0, 0), // fileD
        P(kingByFile_1, -20),
        P(kingByFile_2, 11), // -fileH

        P(kingByRank_0, -154), // rank1
        P(kingByRank_1, -276),
        P(kingByRank_2, -380),
        P(kingByRank_3, -376),
        P(kingByRank_4, -230),
        P(kingByRank_5, 13),
        P(kingByRank_6, 172), // -rank8

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
        P(drawOffset, 194),
        P(drawQueen, -2006),
        P(drawRook, 119),
        P(drawBishop, -33),
        P(drawKnight, -13),
        P(drawPawn, -794),
        P(drawQueenEnding, 1379),
        P(drawRookEnding, 404),
        P(drawKnightEnding, 530),
        P(drawBishopEnding, 1473),
        P(drawPawnEnding, -13741),
        P(drawPawnless, 303),

        P(drawQueenImbalance, 892),
        P(drawRookImbalance, 259),
        P(drawMinorImbalance, 64),

        P(drawUnlikeBishops, 3759),
        P(drawUnlikeBishopsAndQueens, -327),
        P(drawUnlikeBishopsAndRooks, 431),
        P(drawUnlikeBishopsAndKnights, 917),
        // TODO: pawn and pawn block square colors


// }

/*----------------------------------------------------------------------+
 |                                                                      |
 +----------------------------------------------------------------------*/

