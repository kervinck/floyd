
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
        P(tempo, 39),
        P(winBonus, 3345),

        P(castleK, -63),
        P(castleQ, -115),
        P(castleKQ, -25),

        P(queenValue, 6759),
        P(rookValue, 4329),
        P(bishopValue, 2382),
        P(knightValue, 2237),
        P(pawnValue1, 1038), P(pawnValue2, 1697), P(pawnValue3, 2354), P(pawnValue4, 3030),
        P(pawnValue5, 3702), P(pawnValue6, 4340), P(pawnValue7, 4946), P(pawnValue8, 5542),

        P(queenAndQueen, -637),
        P(queenAndRook, -671),
        P(queenAndBishop, 40),
        P(queenAndKnight, -43),
        P(queenAndPawn_1, -118), P(queenAndPawn_2, -72),
        P(rookAndRook, -298),
        P(rookAndBishop, -123),
        P(rookAndKnight, -151),
        P(rookAndPawn_1, -62), P(rookAndPawn_2, -61),
        P(bishopAndBishop, 81),
        P(bishopAndKnight, -23),
        P(bishopAndPawn_1, -56), P(bishopAndPawn_2, -20),
        P(knightAndKnight, -70),
        P(knightAndPawn_1, -27), P(knightAndPawn_2, -3),

        P(queenVsRook, -323),
        P(queenVsBishop, 64),
        P(queenVsKnight, -5),
        P(queenVsPawn_1, 106), P(queenVsPawn_2, -100),
        P(rookVsBishop, -4),
        P(rookVsKnight, 26),
        P(rookVsPawn_1, -16), P(rookVsPawn_2, -40),
        P(bishopVsKnight, 17),
        P(bishopVsPawn_1, -11), P(bishopVsPawn_2, -13),
        P(knightVsPawn_1, -4), P(knightVsPawn_2, -21),

        P(controlCenter, 29),
        P(controlExtendedCenter, 37),
        P(controlOutside, 26),

        /*
         *  king safety
         */

        // scaling
        P(safetyScalingOffset, -3734),
        P(safetyAndQueen, 581),
        P(safetyAndRook, 2647),
        P(safetyAndBishop, 1203),
        P(safetyAndKnight, 1241),
        P(safetyAndPawn, -997),
        P(safetyVsQueen, 15097),
        P(safetyVsRook, -2622),
        P(safetyVsBishop, -1876),
        P(safetyVsKnight, -1642),
        P(safetyVsPawn, -799),

        // shelter
        P(shelterPawn_0, 149), // rank3
        P(shelterPawn_1, 277), // rank4
        P(shelterPawn_2, 280), // rank5
        P(shelterPawn_3, 140), // rank6
        P(shelterPawn_4, -105), // rank7
        P(shelterPawn_5, 285), // no pawn

        P(shelterKing_0, -1), // fileA (fileH)
        P(shelterKing_1, -104),
        P(shelterKing_2, -84), // -fileD (-fileE)

        P(shelterWalkingKing, 34), // rank2 or up
        P(shelterCastled, 14), // fraction of 256

        // attacks
        P(attackSquares_0, 94), // 0 attacks
        P(attackSquares_1, 37),
        P(attackSquares_2, -65),
        P(attackSquares_3, -158),
        P(attackSquares_4, -216),
        P(attackSquares_5, -197), // -(6 or more attacks)

        P(attackByPawn_0, 102),
        P(attackByPawn_1, 220),
        P(attackByPawn_2, 54),
        P(attackByMinor_0, -115),
        P(attackByMinor_1, 13),
        P(attackByMinor_2, -305),
        P(attackByRook_0, 127),
        P(attackByRook_1, 432),
        P(attackByRook_2, -392),
        P(attackByQueen, 173),
        P(attackByKing, -204),

        // TODO: pawn storm

        // king mobility
        P(mobilityKing_0, 115),
        P(mobilityKing_1, 66),

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

        P(pawnByFile_0, 5), // fileA
        P(pawnByFile_1, 23),
        P(pawnByFile_2, -20),
        P(pawnByFile_3, -61),
        P(pawnByFile_4, -91),
        P(pawnByFile_5, -48),
        P(pawnByFile_6, 35), // -fileH

        P(pawnByFile_0x, -103), // fileA
        P(pawnByFile_1x, -146),
        P(pawnByFile_2x, -204),
        P(pawnByFile_3x, -268),
        P(pawnByFile_4x, -278),
        P(pawnByFile_5x, -205),
        P(pawnByFile_6x, -78), // -fileH

        // 5 parameters for 6 pawn ranks
        P(pawnByRank_0, -53), // rank2
        P(pawnByRank_1, -89),
        P(pawnByRank_2, -92),
        P(pawnByRank_3, -32),
        P(pawnByRank_4, 66), // -rank7

        P(doubledPawnA, -204), P(doubledPawnB, -121), P(doubledPawnC, -172), P(doubledPawnD, -121),
        P(doubledPawnE, -118), P(doubledPawnF, -61), P(doubledPawnG, -54), P(doubledPawnH, -225),

        P(backwardPawnClosedByRank_0, 9),
        P(backwardPawnClosedByRank_1, 14),
        P(backwardPawnClosedByRank_2, 1),
        P(backwardPawnClosedByRank_3, 5),
        P(backwardPawnClosedByRank_4, 0),

        P(backwardPawnOpenByRank_0, 13),
        P(backwardPawnOpenByRank_1, 12),
        P(backwardPawnOpenByRank_2, 11),
        P(backwardPawnOpenByRank_3, 48),
        P(backwardPawnOpenByRank_4, 0),

        P(backwardPawnClosedA, 46),
        P(backwardPawnClosedB, -1),
        P(backwardPawnClosedC, 17),
        P(backwardPawnClosedD, -39),
        P(backwardPawnClosedE, -61),
        P(backwardPawnClosedF, -20),
        P(backwardPawnClosedG, -4),
        P(backwardPawnClosedH, 50),

        P(backwardPawnOpenA, -7),
        P(backwardPawnOpenB, -66),
        P(backwardPawnOpenC, -60),
        P(backwardPawnOpenD, -75),
        P(backwardPawnOpenE, -77),
        P(backwardPawnOpenF, -82),
        P(backwardPawnOpenG, -70),
        P(backwardPawnOpenH, 6),

        P(rammedWeakPawnA, -17),
        P(rammedWeakPawnB, -9),
        P(rammedWeakPawnC, -11),
        P(rammedWeakPawnD, -9),
        P(rammedWeakPawnE, 4),
        P(rammedWeakPawnF, -15),
        P(rammedWeakPawnG, -26),
        P(rammedWeakPawnH, -60),

        P(isolatedPawnClosedA, 124), P(isolatedPawnClosedB, 22),
        P(isolatedPawnClosedC, 1), P(isolatedPawnClosedD, 11),
        P(isolatedPawnClosedE, -53), P(isolatedPawnClosedF, -82),
        P(isolatedPawnClosedG, -89), P(isolatedPawnClosedH, 1),

        P(isolatedPawnOpenA, 86), P(isolatedPawnOpenB, 45),
        P(isolatedPawnOpenC, -35), P(isolatedPawnOpenD, -89),
        P(isolatedPawnOpenE, -82), P(isolatedPawnOpenF, -46),
        P(isolatedPawnOpenG, -26), P(isolatedPawnOpenH, 34),

        P(sidePawnClosedA, 57), P(sidePawnClosedB, 31),
        P(sidePawnClosedC, 65), P(sidePawnClosedD, -22),
        P(sidePawnClosedE, -26), P(sidePawnClosedF, -53),
        P(sidePawnClosedG, -98), P(sidePawnClosedH, -19),

        P(sidePawnOpenA, 22), P(sidePawnOpenB, 54),
        P(sidePawnOpenC, 13), P(sidePawnOpenD, -36),
        P(sidePawnOpenE, -30), P(sidePawnOpenF, -13),
        P(sidePawnOpenG, 2), P(sidePawnOpenH, 7),

        P(middlePawnClosedA, 0), P(middlePawnClosedB, 2),
        P(middlePawnClosedC, 92), P(middlePawnClosedD, -18),
        P(middlePawnClosedE, -5), P(middlePawnClosedF, -16),
        P(middlePawnClosedG, -80), P(middlePawnClosedH, 0),

        P(middlePawnOpenA, 0), P(middlePawnOpenB, -37),
        P(middlePawnOpenC, 13), P(middlePawnOpenD, 6),
        P(middlePawnOpenE, 9), P(middlePawnOpenF, -37),
        P(middlePawnOpenG, -60), P(middlePawnOpenH, 0),

        P(duoPawnA, 6),
        P(duoPawnB, -6),
        P(duoPawnC, -9),
        P(duoPawnD, 28),
        P(duoPawnE, 25),
        P(duoPawnF, 11),
        P(duoPawnG, 30),
        P(duoPawnH, -58),

        P(openFilePawn_0, -23),
        P(openFilePawn_1, -16),
        P(openFilePawn_2, 32),
        P(openFilePawn_3, -3),
        P(openFilePawn_4, 23),
        P(openFilePawn_5, 202),

        P(stoppedPawn_0, -39),
        P(stoppedPawn_1, 13),
        P(stoppedPawn_2, 8),
        P(stoppedPawn_3, -2),
        P(stoppedPawn_4, 0),
        P(stoppedPawn_5, 0),

        P(duoPawn_0, 1),
        P(duoPawn_1, -10),
        P(duoPawn_2, 33),
        P(duoPawn_3, 145),
        P(duoPawn_4, 519),
        P(duoPawn_5, 1465),

        P(trailingPawn_0, -7),
        P(trailingPawn_1, -53),
        P(trailingPawn_2, -57),
        P(trailingPawn_3, -21),
        P(trailingPawn_4, 8),
        P(trailingPawn_5, -35),

        P(capturePawn_0, 134),
        P(capturePawn_1, -71),
        P(capturePawn_2, -56),
        P(capturePawn_3, 45),
        P(capturePawn_4, 369),
        P(capturePawn_5, 0),

        P(defendedPawn_0, 0),
        P(defendedPawn_1, 57),
        P(defendedPawn_2, 11),
        P(defendedPawn_3, 98),
        P(defendedPawn_4, 204),
        P(defendedPawn_5, 152),

        P(pawnLever_0, 0),
        P(pawnLever_1, 0),
        P(pawnLever_2, -36),
        P(pawnLever_3, -34),
        P(pawnLever_4, -28),
        P(pawnLever_5, -24),
        P(pawnLever_6, 0),

        P(rammedPawn_0, -22),
        P(rammedPawn_1, 25),
        P(rammedPawn_2, -71),
        P(rammedPawn_3, -41),
        P(rammedPawn_4, 9),
        P(rammedPawn_5, 0),

        P(mobilePawn_0, 1),
        P(mobilePawn_1, 8),
        P(mobilePawn_2, 7),
        P(mobilePawn_3, 5),
        P(mobilePawn_4, 12),
        P(mobilePawn_5, 23),

        // Quadratic polynomials for passers
        P(passerA_0, -29), P(passerB_0, 53), P(passerC_0, 108), P(passerD_0, 88),
        P(passerE_0, 215), P(passerF_0, 133), P(passerG_0, 99), P(passerH_0, 82),

        P(passerA_1, 71), P(passerB_1, -11), P(passerC_1, -52), P(passerD_1, -25),
        P(passerE_1, -81), P(passerF_1, -25), P(passerG_1, 41), P(passerH_1, 74),

        P(passerA_2, 150), P(passerB_2, 216), P(passerC_2, 245), P(passerD_2, 199),
        P(passerE_2, 245), P(passerF_2, 220), P(passerG_2, 158), P(passerH_2, 112),

        P(passerScalingOffset, 4169),
        P(passerAndQueen, 558),
        P(passerAndRook, 291),
        P(passerAndBishop, 536),
        P(passerAndKnight, 345),
        P(passerAndPawn, -633),
        P(passerVsQueen, 293),
        P(passerVsRook, -420),
        P(passerVsBishop, -1240),
        P(passerVsKnight, -850),
        P(passerVsPawn, 91),

        P(candidateByRank_0, 0),
        P(candidateByRank_1, 13),
        P(candidateByRank_2, 67),
        P(candidateByRank_3, 170),
        P(candidateByRank_4, 245),

        P(candidateA, 64),
        P(candidateB, -37),
        P(candidateC, -7),
        P(candidateD, -31),
        P(candidateE, 23),
        P(candidateF, 25),
        P(candidateG, 4),
        P(candidateH, 59),
#if 0
        P(drawBlockedA, 0), P(drawBlockedB, 0), P(drawBlockedC, 0), P(drawBlockedD, 0),
        P(drawBlockedE, 0), P(drawBlockedF, 0), P(drawBlockedG, 0), P(drawBlockedH, 0),

        P(drawPasser_0, 0), P(drawPasser_1, 0), P(drawPasser_2, 0),
#endif

        /*
         *  Knights
         */

        // Kings on same side
        P(knightByFile_0, -182), // fileA
        P(knightByFile_1, -220),
        P(knightByFile_2, -232),
        P(knightByFile_3, -164),
        P(knightByFile_4, -100),
        P(knightByFile_5, -23),
        P(knightByFile_6, 25), // -fileH

        // Opposite side kings
        P(knightByFile_0x, -119), // fileA
        P(knightByFile_1x, -151),
        P(knightByFile_2x, -125),
        P(knightByFile_3x, -32),
        P(knightByFile_4x, 51),
        P(knightByFile_5x, 136),
        P(knightByFile_6x, 118), // -fileH

        P(knightByRank_0, -124), // rank1
        P(knightByRank_1, -203),
        P(knightByRank_2, -222),
        P(knightByRank_3, -160),
        P(knightByRank_4, -49),
        P(knightByRank_5, 102),
        P(knightByRank_6, 165), // -rank8

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
       P(bishopBySquare_0, -182),
        P(bishopBySquare_1, 0),
        P(bishopBySquare_2, -82),
        P(bishopBySquare_3, 84),
        P(bishopBySquare_4, -59),
        P(bishopBySquare_5, -1),
        P(bishopBySquare_6, -57),
        P(bishopBySquare_7, 44),

        P(bishopBySquare_8, -42),
        P(bishopBySquare_9, -50),
        P(bishopBySquare_10, 82),
        P(bishopBySquare_11, -19),
        P(bishopBySquare_12, 30),
        P(bishopBySquare_13, 102),
        P(bishopBySquare_14, 79),
        P(bishopBySquare_15, 112),

        P(bishopBySquare_16, -127),
        P(bishopBySquare_17, 41),
        P(bishopBySquare_18, 18),
        P(bishopBySquare_19, 51),
        P(bishopBySquare_20, 37),
        P(bishopBySquare_21, 84),
        P(bishopBySquare_22, 69),
        P(bishopBySquare_23, 106),

        P(bishopBySquare_24, -86),
        P(bishopBySquare_25, -59),
        P(bishopBySquare_26, 53),
        P(bishopBySquare_27, 86),
        P(bishopBySquare_28, 141),
        P(bishopBySquare_29, 87),
        P(bishopBySquare_30, 96),
        P(bishopBySquare_31, 74),

        P(bishopBySquare_32, -85),
        P(bishopBySquare_33, 17),
        P(bishopBySquare_34, 9),
        P(bishopBySquare_35, 81),
        P(bishopBySquare_36, 88),
        P(bishopBySquare_37, 123),
        P(bishopBySquare_38, 20),
        P(bishopBySquare_39, 71),

        P(bishopBySquare_40, -80),
        P(bishopBySquare_41, -1),
        P(bishopBySquare_42, 51),
        P(bishopBySquare_43, -16),
        P(bishopBySquare_44, 110),
        P(bishopBySquare_45, 227),
        P(bishopBySquare_46, 40),
        P(bishopBySquare_47, -11),

        P(bishopBySquare_48, -56),
        P(bishopBySquare_49, 45),
        P(bishopBySquare_50, 33),
        P(bishopBySquare_51, 44),
        P(bishopBySquare_52, -13),
        P(bishopBySquare_53, 150),
        P(bishopBySquare_54, -61),
        P(bishopBySquare_55, 12),

        P(bishopBySquare_56, -213),
        P(bishopBySquare_57, -64),
        P(bishopBySquare_58, 2),
        P(bishopBySquare_59, 29),
        P(bishopBySquare_60, -3),
        P(bishopBySquare_61, 129),
        P(bishopBySquare_62, -232),
        P(bishopBySquare_63, -76),

        P(bishopBySquare_0x, -106),
        P(bishopBySquare_1x, -33),
        P(bishopBySquare_2x, 24),
        P(bishopBySquare_3x, -9),
        P(bishopBySquare_4x, 4),
        P(bishopBySquare_5x, 109),
        P(bishopBySquare_6x, -216),
        P(bishopBySquare_7x, -81),

        P(bishopBySquare_8x, -85),
        P(bishopBySquare_9x, -76),
        P(bishopBySquare_10x, 7),
        P(bishopBySquare_11x, 68),
        P(bishopBySquare_12x, 45),
        P(bishopBySquare_13x, 136),
        P(bishopBySquare_14x, 33),
        P(bishopBySquare_15x, -2),

        P(bishopBySquare_16x, -117),
        P(bishopBySquare_17x, -5),
        P(bishopBySquare_18x, 58),
        P(bishopBySquare_19x, 58),
        P(bishopBySquare_20x, 134),
        P(bishopBySquare_21x, 149),
        P(bishopBySquare_22x, 81),
        P(bishopBySquare_23x, 32),

        P(bishopBySquare_24x, -77),
        P(bishopBySquare_25x, -6),
        P(bishopBySquare_26x, 73),
        P(bishopBySquare_27x, 89),
        P(bishopBySquare_28x, 137),
        P(bishopBySquare_29x, 175),
        P(bishopBySquare_30x, 89),
        P(bishopBySquare_31x, 118),

        P(bishopBySquare_32x, -56),
        P(bishopBySquare_33x, 74),
        P(bishopBySquare_34x, 93),
        P(bishopBySquare_35x, 144),
        P(bishopBySquare_36x, 174),
        P(bishopBySquare_37x, 161),
        P(bishopBySquare_38x, 169),
        P(bishopBySquare_39x, 104),

        P(bishopBySquare_40x, -88),
        P(bishopBySquare_41x, 72),
        P(bishopBySquare_42x, 127),
        P(bishopBySquare_43x, 150),
        P(bishopBySquare_44x, 146),
        P(bishopBySquare_45x, 162),
        P(bishopBySquare_46x, 148),
        P(bishopBySquare_47x, 153),

        P(bishopBySquare_48x, -9),
        P(bishopBySquare_49x, 135),
        P(bishopBySquare_50x, 123),
        P(bishopBySquare_51x, 62),
        P(bishopBySquare_52x, 130),
        P(bishopBySquare_53x, 170),
        P(bishopBySquare_54x, 163),
        P(bishopBySquare_55x, 151),

        P(bishopBySquare_56x, -94),
        P(bishopBySquare_57x, 59),
        P(bishopBySquare_58x, 41),
        P(bishopBySquare_59x, 78),
        P(bishopBySquare_60x, 75),
        P(bishopBySquare_61x, 59),
        P(bishopBySquare_62x, -18),
        P(bishopBySquare_63x, -30),
#endif

        /*
         *  Rooks
         */

        P(rookByFile_0, -63), // fileA
        P(rookByFile_1, -83),
        P(rookByFile_2, -56),
        P(rookByFile_3, -10),
        P(rookByFile_4, 37),
        P(rookByFile_5, 54),
        P(rookByFile_6, 109), // -fileH

        P(rookByFile_0x, -86), // fileA
        P(rookByFile_1x, -72),
        P(rookByFile_2x, -78),
        P(rookByFile_3x, -61),
        P(rookByFile_4x, -7),
        P(rookByFile_5x, 38),
        P(rookByFile_6x, 68), // -fileH

        P(rookByRank_0, -138), // rank1
        P(rookByRank_1, -318),
        P(rookByRank_2, -463),
        P(rookByRank_3, -521),
        P(rookByRank_4, -435),
        P(rookByRank_5, -283),
        P(rookByRank_6, -124), // -rank8

        /*
         *  Queens
         */

        P(queenByFile_0, -108), // fileA
        P(queenByFile_1, -167),
        P(queenByFile_2, -171),
        P(queenByFile_3, -147),
        P(queenByFile_4, -114),
        P(queenByFile_5, -79),
        P(queenByFile_6, -30), // -fileH

        P(queenByFile_0x, -6), // fileA
        P(queenByFile_1x, -9),
        P(queenByFile_2x, 14),
        P(queenByFile_3x, 61),
        P(queenByFile_4x, 127),
        P(queenByFile_5x, 171),
        P(queenByFile_6x, 127), // -fileH

        P(queenByRank_0, -154), // rank1
        P(queenByRank_1, -253),
        P(queenByRank_2, -327),
        P(queenByRank_3, -358),
        P(queenByRank_4, -310),
        P(queenByRank_5, -208),
        P(queenByRank_6, -117), // -rank8

        /*
         *  Kings
         */

        P(kingByFile_0, 37), // fileD
        P(kingByFile_1, 53),
        P(kingByFile_2, 46), // -fileH

        P(kingByRank_0, -224), // rank1
        P(kingByRank_1, -371),
        P(kingByRank_2, -481),
        P(kingByRank_3, -502),
        P(kingByRank_4, -377),
        P(kingByRank_5, -129),
        P(kingByRank_6, 80), // -rank8

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
        P(drawOffset, 209),
        P(drawQueen, -908),
        P(drawRook, 99),
        P(drawBishop, -23),
        P(drawKnight, -5),
        P(drawPawn, -759),
        P(drawQueenEnding, 656),
        P(drawRookEnding, 409),
        P(drawKnightEnding, 498),
        P(drawBishopEnding, 1402),
        P(drawPawnEnding, -16501),
        P(drawPawnless, 133),

        P(drawQueenImbalance, 119),
        P(drawRookImbalance, 327),
        P(drawMinorImbalance, 35),

        P(drawUnlikeBishops, 3673),
        P(drawUnlikeBishopsAndQueens, 269),
        P(drawUnlikeBishopsAndRooks, 410),
        P(drawUnlikeBishopsAndKnights, 883),
        // TODO: pawn and pawn block square colors


// }

/*----------------------------------------------------------------------+
 |                                                                      |
 +----------------------------------------------------------------------*/

