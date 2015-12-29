
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
        P(winBonus, 3440),

        P(castleK, -57),
        P(castleQ, -102),
        P(castleKQ, -21),

        P(queenValue, 6978),
        P(rookValue, 4312),
        P(bishopValue, 2352),
        P(knightValue, 2192),
        P(pawnValue1, 1056), P(pawnValue2, 1701), P(pawnValue3, 2354), P(pawnValue4, 3020),
        P(pawnValue5, 3693), P(pawnValue6, 4345), P(pawnValue7, 4957), P(pawnValue8, 5555),

        P(queenAndQueen, -720),
        P(queenAndRook, -676),
        P(queenAndBishop, 42),
        P(queenAndKnight, -60),
        P(queenAndPawn_1, -115), P(queenAndPawn_2, -75),
        P(rookAndRook, -308),
        P(rookAndBishop, -126),
        P(rookAndKnight, -158),
        P(rookAndPawn_1, -63), P(rookAndPawn_2, -61),
        P(bishopAndBishop, 72),
        P(bishopAndKnight, -27),
        P(bishopAndPawn_1, -56), P(bishopAndPawn_2, -19),
        P(knightAndKnight, -70),
        P(knightAndPawn_1, -27), P(knightAndPawn_2, -3),

        P(queenVsRook, -296),
        P(queenVsBishop, 84),
        P(queenVsKnight, -15),
        P(queenVsPawn_1, 93), P(queenVsPawn_2, -107),
        P(rookVsBishop, 2),
        P(rookVsKnight, 25),
        P(rookVsPawn_1, -19), P(rookVsPawn_2, -42),
        P(bishopVsKnight, 8),
        P(bishopVsPawn_1, -11), P(bishopVsPawn_2, -7),
        P(knightVsPawn_1, -2), P(knightVsPawn_2, -19),

        P(controlCenter, 30),
        P(controlExtendedCenter, 37),
        P(controlOutside, 27),

        /*
         *  king safety
         */

        // scaling
        P(safetyScalingOffset, -3836),
        P(safetyAndQueen, 1098),
        P(safetyAndRook, 2073),
        P(safetyAndBishop, 672),
        P(safetyAndKnight, 797),
        P(safetyAndPawn, -1220),
        P(safetyVsQueen, 15786),
        P(safetyVsRook, -1850),
        P(safetyVsBishop, -1432),
        P(safetyVsKnight, -1130),
        P(safetyVsPawn, -797),

        // shelter
        P(shelterPawn_0, 135), // rank3
        P(shelterPawn_1, 267), // rank4
        P(shelterPawn_2, 267), // rank5
        P(shelterPawn_3, 130), // rank6
        P(shelterPawn_4, -98), // rank7
        P(shelterPawn_5, 268), // no pawn

        P(shelterKing_0, 70), // fileA (fileH)
        P(shelterKing_1, -61),
        P(shelterKing_2, -57), // -fileD (-fileE)

        P(shelterWalkingKing, 31), // rank2 or up
        P(shelterCastled, 4), // fraction of 256

        // attacks
        P(attackSquares_0, 97), // 0 attacks
        P(attackSquares_1, 46),
        P(attackSquares_2, -59),
        P(attackSquares_3, -157),
        P(attackSquares_4, -219),
        P(attackSquares_5, -199), // -(6 or more attacks)

        P(attackByPawn_0, 44),
        P(attackByPawn_1, 99),
        P(attackByPawn_2, -12),
        P(attackByMinor_0, -116),
        P(attackByMinor_1, 10),
        P(attackByMinor_2, -293),
        P(attackByRook_0, 0),
        P(attackByRook_1, 175),
        P(attackByRook_2, -314),
        P(attackByQueen, 158),
        P(attackByKing, -196),

        // TODO: pawn storm

        // king mobility
        P(mobilityKing_0, 101),
        P(mobilityKing_1, 60),

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
        P(pawnByFile_1, 9),
        P(pawnByFile_2, -41),
        P(pawnByFile_3, -78),
        P(pawnByFile_4, -101),
        P(pawnByFile_5, -50),
        P(pawnByFile_6, 36), // -fileH

        P(pawnByFile_0x, -85), // fileA
        P(pawnByFile_1x, -112),
        P(pawnByFile_2x, -159),
        P(pawnByFile_3x, -218),
        P(pawnByFile_4x, -237),
        P(pawnByFile_5x, -177),
        P(pawnByFile_6x, -60), // -fileH

        // 5 parameters for 6 pawn ranks
        P(pawnByRank_0, -54), // rank2
        P(pawnByRank_1, -88),
        P(pawnByRank_2, -92),
        P(pawnByRank_3, -33),
        P(pawnByRank_4, 66), // -rank7

        P(doubledPawnA, -213), P(doubledPawnB, -135), P(doubledPawnC, -181), P(doubledPawnD, -110),
        P(doubledPawnE, -118), P(doubledPawnF, -72), P(doubledPawnG, -60), P(doubledPawnH, -224),

        P(backwardPawnClosedA, 32),
        P(backwardPawnClosedB, 1),
        P(backwardPawnClosedC, 19),
        P(backwardPawnClosedD, -46),
        P(backwardPawnClosedE, -73),
        P(backwardPawnClosedF, -15),
        P(backwardPawnClosedG, -12),
        P(backwardPawnClosedH, 50),

        P(backwardPawnOpenA, -14),
        P(backwardPawnOpenB, -68),
        P(backwardPawnOpenC, -59),
        P(backwardPawnOpenD, -82),
        P(backwardPawnOpenE, -85),
        P(backwardPawnOpenF, -75),
        P(backwardPawnOpenG, -71),
        P(backwardPawnOpenH, -2),

        P(rammedWeakPawnA, -5),
        P(rammedWeakPawnB, -10),
        P(rammedWeakPawnC, -23),
        P(rammedWeakPawnD, -6),
        P(rammedWeakPawnE, 8),
        P(rammedWeakPawnF, -7),
        P(rammedWeakPawnG, -29),
        P(rammedWeakPawnH, -55),

        P(isolatedPawnClosedA, 94), P(isolatedPawnClosedB, -15),
        P(isolatedPawnClosedC, 1), P(isolatedPawnClosedD, 12),
        P(isolatedPawnClosedE, -57), P(isolatedPawnClosedF, -74),
        P(isolatedPawnClosedG, -81), P(isolatedPawnClosedH, 2),

        P(isolatedPawnOpenA, 72), P(isolatedPawnOpenB, 50),
        P(isolatedPawnOpenC, -34), P(isolatedPawnOpenD, -101),
        P(isolatedPawnOpenE, -76), P(isolatedPawnOpenF, -49),
        P(isolatedPawnOpenG, -32), P(isolatedPawnOpenH, 35),

        P(sidePawnClosedA, 34), P(sidePawnClosedB, 21),
        P(sidePawnClosedC, 54), P(sidePawnClosedD, -24),
        P(sidePawnClosedE, -25), P(sidePawnClosedF, -40),
        P(sidePawnClosedG, -83), P(sidePawnClosedH, -2),

        P(sidePawnOpenA, 17), P(sidePawnOpenB, 50),
        P(sidePawnOpenC, 9), P(sidePawnOpenD, -40),
        P(sidePawnOpenE, -28), P(sidePawnOpenF, -11),
        P(sidePawnOpenG, -3), P(sidePawnOpenH, 17),

        P(middlePawnClosedA, 0), P(middlePawnClosedB, 2),
        P(middlePawnClosedC, 84), P(middlePawnClosedD, -5),
        P(middlePawnClosedE, -6), P(middlePawnClosedF, -7),
        P(middlePawnClosedG, -74), P(middlePawnClosedH, 0),

        P(middlePawnOpenA, 0), P(middlePawnOpenB, -38),
        P(middlePawnOpenC, 15), P(middlePawnOpenD, 2),
        P(middlePawnOpenE, 13), P(middlePawnOpenF, -30),
        P(middlePawnOpenG, -67), P(middlePawnOpenH, 0),

        P(duoPawnA, 4),
        P(duoPawnB, -2),
        P(duoPawnC, -7),
        P(duoPawnD, 30),
        P(duoPawnE, 28),
        P(duoPawnF, 9),
        P(duoPawnG, 39),
        P(duoPawnH, -63),

        P(openFilePawn_0, -24),
        P(openFilePawn_1, -17),
        P(openFilePawn_2, 35),
        P(openFilePawn_3, -5),
        P(openFilePawn_4, 17),
        P(openFilePawn_5, 173),

        P(stoppedPawn_0, -38),
        P(stoppedPawn_1, 12),
        P(stoppedPawn_2, 3),
        P(stoppedPawn_3, -2),
        P(stoppedPawn_4, 0),
        P(stoppedPawn_5, 0),

        P(duoPawn_0, -2),
        P(duoPawn_1, -11),
        P(duoPawn_2, 36),
        P(duoPawn_3, 174),
        P(duoPawn_4, 518),
        P(duoPawn_5, 1507),

        P(trailingPawn_0, -3),
        P(trailingPawn_1, -46),
        P(trailingPawn_2, -53),
        P(trailingPawn_3, -16),
        P(trailingPawn_4, 10),
        P(trailingPawn_5, -22),

        P(capturePawn_0, 105),
        P(capturePawn_1, -100),
        P(capturePawn_2, -20),
        P(capturePawn_3, 35),
        P(capturePawn_4, 442),
        P(capturePawn_5, 0),

        P(defendedPawn_0, 0),
        P(defendedPawn_1, 53),
        P(defendedPawn_2, 8),
        P(defendedPawn_3, 89),
        P(defendedPawn_4, 203),
        P(defendedPawn_5, 110),

        P(pawnLever_0, 0),
        P(pawnLever_1, 0),
        P(pawnLever_2, -30),
        P(pawnLever_3, -30),
        P(pawnLever_4, -24),
        P(pawnLever_5, -24),
        P(pawnLever_6, 0),

        P(rammedPawn_0, -9),
        P(rammedPawn_1, 26),
        P(rammedPawn_2, -116),
        P(rammedPawn_3, -43),
        P(rammedPawn_4, 10),
        P(rammedPawn_5, 0),

        // Quadratic polynomials for passers
        P(passerA_0, -36), P(passerB_0, 37), P(passerC_0, 76), P(passerD_0, 73),
        P(passerE_0, 177), P(passerF_0, 125), P(passerG_0, 87), P(passerH_0, 99),

        P(passerA_1, 75), P(passerB_1, -6), P(passerC_1, -40), P(passerD_1, -21),
        P(passerE_1, -69), P(passerF_1, -40), P(passerG_1, 32), P(passerH_1, 47),

        P(passerA_2, 157), P(passerB_2, 219), P(passerC_2, 244), P(passerD_2, 203),
        P(passerE_2, 247), P(passerF_2, 242), P(passerG_2, 178), P(passerH_2, 148),

        P(passerScalingOffset, 4223),
        P(passerAndQueen, 35),
        P(passerAndRook, 118),
        P(passerAndBishop, 523),
        P(passerAndKnight, 230),
        P(passerAndPawn, -646),
        P(passerVsQueen, 834),
        P(passerVsRook, -271),
        P(passerVsBishop, -1077),
        P(passerVsKnight, -732),
        P(passerVsPawn, 72),

#if 0
        P(drawBlockedA, 0), P(drawBlockedB, 0), P(drawBlockedC, 0), P(drawBlockedD, 0),
        P(drawBlockedE, 0), P(drawBlockedF, 0), P(drawBlockedG, 0), P(drawBlockedH, 0),

        P(drawPasser_0, 0), P(drawPasser_1, 0), P(drawPasser_2, 0),
#endif

        /*
         *  Knights
         */

        // Kings on same side
        P(knightByFile_0, -185), // fileA
        P(knightByFile_1, -220),
        P(knightByFile_2, -233),
        P(knightByFile_3, -167),
        P(knightByFile_4, -104),
        P(knightByFile_5, -27),
        P(knightByFile_6, 24), // -fileH

        // Opposite side kings
        P(knightByFile_0x, -118), // fileA
        P(knightByFile_1x, -149),
        P(knightByFile_2x, -118),
        P(knightByFile_3x, -30),
        P(knightByFile_4x, 57),
        P(knightByFile_5x, 141),
        P(knightByFile_6x, 118), // -fileH

        P(knightByRank_0, -124), // rank1
        P(knightByRank_1, -201),
        P(knightByRank_2, -221),
        P(knightByRank_3, -157),
        P(knightByRank_4, -45),
        P(knightByRank_5, 97),
        P(knightByRank_6, 173), // -rank8

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
       P(bishopBySquare_0, -192),
        P(bishopBySquare_1, -7),
        P(bishopBySquare_2, -83),
        P(bishopBySquare_3, 79),
        P(bishopBySquare_4, -55),
        P(bishopBySquare_5, -6),
        P(bishopBySquare_6, -62),
        P(bishopBySquare_7, 22),

        P(bishopBySquare_8, -58),
        P(bishopBySquare_9, -52),
        P(bishopBySquare_10, 82),
        P(bishopBySquare_11, -25),
        P(bishopBySquare_12, 29),
        P(bishopBySquare_13, 106),
        P(bishopBySquare_14, 98),
        P(bishopBySquare_15, 107),

        P(bishopBySquare_16, -128),
        P(bishopBySquare_17, 43),
        P(bishopBySquare_18, 18),
        P(bishopBySquare_19, 41),
        P(bishopBySquare_20, 34),
        P(bishopBySquare_21, 85),
        P(bishopBySquare_22, 77),
        P(bishopBySquare_23, 98),

        P(bishopBySquare_24, -96),
        P(bishopBySquare_25, -67),
        P(bishopBySquare_26, 51),
        P(bishopBySquare_27, 80),
        P(bishopBySquare_28, 136),
        P(bishopBySquare_29, 98),
        P(bishopBySquare_30, 77),
        P(bishopBySquare_31, 73),

        P(bishopBySquare_32, -73),
        P(bishopBySquare_33, 10),
        P(bishopBySquare_34, 6),
        P(bishopBySquare_35, 81),
        P(bishopBySquare_36, 74),
        P(bishopBySquare_37, 129),
        P(bishopBySquare_38, 3),
        P(bishopBySquare_39, 70),

        P(bishopBySquare_40, -92),
        P(bishopBySquare_41, 0),
        P(bishopBySquare_42, 45),
        P(bishopBySquare_43, -21),
        P(bishopBySquare_44, 104),
        P(bishopBySquare_45, 221),
        P(bishopBySquare_46, 51),
        P(bishopBySquare_47, -11),

        P(bishopBySquare_48, -59),
        P(bishopBySquare_49, 38),
        P(bishopBySquare_50, 12),
        P(bishopBySquare_51, 52),
        P(bishopBySquare_52, -8),
        P(bishopBySquare_53, 152),
        P(bishopBySquare_54, -61),
        P(bishopBySquare_55, 59),

        P(bishopBySquare_56, -237),
        P(bishopBySquare_57, -63),
        P(bishopBySquare_58, -4),
        P(bishopBySquare_59, 44),
        P(bishopBySquare_60, -26),
        P(bishopBySquare_61, 133),
        P(bishopBySquare_62, -207),
        P(bishopBySquare_63, -78),

        P(bishopBySquare_0x, -76),
        P(bishopBySquare_1x, -54),
        P(bishopBySquare_2x, 19),
        P(bishopBySquare_3x, -22),
        P(bishopBySquare_4x, 15),
        P(bishopBySquare_5x, 100),
        P(bishopBySquare_6x, -224),
        P(bishopBySquare_7x, -129),

        P(bishopBySquare_8x, -36),
        P(bishopBySquare_9x, -70),
        P(bishopBySquare_10x, -2),
        P(bishopBySquare_11x, 57),
        P(bishopBySquare_12x, 20),
        P(bishopBySquare_13x, 122),
        P(bishopBySquare_14x, 25),
        P(bishopBySquare_15x, -80),

        P(bishopBySquare_16x, -121),
        P(bishopBySquare_17x, -17),
        P(bishopBySquare_18x, 43),
        P(bishopBySquare_19x, 62),
        P(bishopBySquare_20x, 131),
        P(bishopBySquare_21x, 108),
        P(bishopBySquare_22x, 95),
        P(bishopBySquare_23x, 90),

        P(bishopBySquare_24x, -81),
        P(bishopBySquare_25x, -8),
        P(bishopBySquare_26x, 76),
        P(bishopBySquare_27x, 96),
        P(bishopBySquare_28x, 134),
        P(bishopBySquare_29x, 156),
        P(bishopBySquare_30x, 60),
        P(bishopBySquare_31x, 96),

        P(bishopBySquare_32x, -91),
        P(bishopBySquare_33x, 91),
        P(bishopBySquare_34x, 91),
        P(bishopBySquare_35x, 150),
        P(bishopBySquare_36x, 196),
        P(bishopBySquare_37x, 151),
        P(bishopBySquare_38x, 112),
        P(bishopBySquare_39x, 71),

        P(bishopBySquare_40x, -77),
        P(bishopBySquare_41x, 82),
        P(bishopBySquare_42x, 124),
        P(bishopBySquare_43x, 155),
        P(bishopBySquare_44x, 154),
        P(bishopBySquare_45x, 118),
        P(bishopBySquare_46x, 158),
        P(bishopBySquare_47x, 173),

        P(bishopBySquare_48x, -9),
        P(bishopBySquare_49x, 145),
        P(bishopBySquare_50x, 138),
        P(bishopBySquare_51x, 67),
        P(bishopBySquare_52x, 134),
        P(bishopBySquare_53x, 178),
        P(bishopBySquare_54x, 117),
        P(bishopBySquare_55x, 134),

        P(bishopBySquare_56x, -44),
        P(bishopBySquare_57x, 90),
        P(bishopBySquare_58x, 40),
        P(bishopBySquare_59x, 94),
        P(bishopBySquare_60x, 88),
        P(bishopBySquare_61x, 66),
        P(bishopBySquare_62x, -25),
        P(bishopBySquare_63x, -36),
#endif

        /*
         *  Rooks
         */

        P(rookByFile_0, -63), // fileA
        P(rookByFile_1, -85),
        P(rookByFile_2, -58),
        P(rookByFile_3, -12),
        P(rookByFile_4, 35),
        P(rookByFile_5, 53),
        P(rookByFile_6, 106), // -fileH

        P(rookByFile_0x, -85), // fileA
        P(rookByFile_1x, -68),
        P(rookByFile_2x, -72),
        P(rookByFile_3x, -55),
        P(rookByFile_4x, 1),
        P(rookByFile_5x, 40),
        P(rookByFile_6x, 75), // -fileH

        P(rookByRank_0, -138), // rank1
        P(rookByRank_1, -317),
        P(rookByRank_2, -468),
        P(rookByRank_3, -521),
        P(rookByRank_4, -438),
        P(rookByRank_5, -282),
        P(rookByRank_6, -121), // -rank8

        /*
         *  Queens
         */

        P(queenByFile_0, -106), // fileA
        P(queenByFile_1, -168),
        P(queenByFile_2, -171),
        P(queenByFile_3, -147),
        P(queenByFile_4, -115),
        P(queenByFile_5, -84),
        P(queenByFile_6, -36), // -fileH

        P(queenByFile_0x, -16), // fileA
        P(queenByFile_1x, -10),
        P(queenByFile_2x, 6),
        P(queenByFile_3x, 54),
        P(queenByFile_4x, 114),
        P(queenByFile_5x, 168),
        P(queenByFile_6x, 130), // -fileH

        P(queenByRank_0, -167), // rank1
        P(queenByRank_1, -272),
        P(queenByRank_2, -350),
        P(queenByRank_3, -389),
        P(queenByRank_4, -338),
        P(queenByRank_5, -230),
        P(queenByRank_6, -144), // -rank8

        /*
         *  Kings
         */

        P(kingByFile_0, 39), // fileD
        P(kingByFile_1, 52),
        P(kingByFile_2, 46), // -fileH

        P(kingByRank_0, -207), // rank1
        P(kingByRank_1, -342),
        P(kingByRank_2, -450),
        P(kingByRank_3, -451),
        P(kingByRank_4, -311),
        P(kingByRank_5, -63),
        P(kingByRank_6, 133), // -rank8

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
        P(drawOffset, 226),
        P(drawQueen, -1575),
        P(drawRook, 106),
        P(drawBishop, -30),
        P(drawKnight, -17),
        P(drawPawn, -774),
        P(drawQueenEnding, 1059),
        P(drawRookEnding, 391),
        P(drawKnightEnding, 530),
        P(drawBishopEnding, 1420),
        P(drawPawnEnding, -18052),
        P(drawPawnless, 230),

        P(drawQueenImbalance, 629),
        P(drawRookImbalance, 328),
        P(drawMinorImbalance, 42),

        P(drawUnlikeBishops, 3700),
        P(drawUnlikeBishopsAndQueens, -286),
        P(drawUnlikeBishopsAndRooks, 413),
        P(drawUnlikeBishopsAndKnights, 960),
        // TODO: pawn and pawn block square colors


// }

/*----------------------------------------------------------------------+
 |                                                                      |
 +----------------------------------------------------------------------*/

