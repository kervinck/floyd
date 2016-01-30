
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
        P(winBonus, 3137),

        P(castleK, -126),
        P(castleQ, -103),
        P(castleKQ, -38),

        P(queenValue, 6713),
        P(rookValue, 4485),
        P(bishopValue, 2491),
        P(knightValue, 2331),
        P(pawnValue1, 988),
        P(pawnValue2, 1662),
        P(pawnValue3, 2340),
        P(pawnValue4, 3029),
        P(pawnValue5, 3701),
        P(pawnValue6, 4350),
        P(pawnValue7, 4963),
        P(pawnValue8, 5486),

        P(queenAndQueen, -725),
        P(queenAndRook, -635),
        P(queenAndBishop, 73),
        P(queenAndKnight, 4),
        P(queenAndPawn_1, -88),
        P(queenAndPawn_2, -90),

        P(rookAndRook, -285),
        P(rookAndBishop, -106),
        P(rookAndKnight, -119),
        P(rookAndPawn_1, -55),
        P(rookAndPawn_2, -54),

        P(bishopAndBishop, 119),
        P(bishopAndKnight, 21),
        P(bishopAndPawn_1, -50),
        P(bishopAndPawn_2, -3),

        P(knightAndKnight, -28),
        P(knightAndPawn_1, -24),
        P(knightAndPawn_2, -3),

        P(queenVsRook, -291),
        P(queenVsBishop, 62),
        P(queenVsKnight, 3),
        P(queenVsPawn_1, 141),
        P(queenVsPawn_2, -122),

        P(rookVsBishop, -24),
        P(rookVsKnight, 5),
        P(rookVsPawn_1, -4),
        P(rookVsPawn_2, -49),

        P(bishopVsKnight, 15),
        P(bishopVsPawn_1, -11),
        P(bishopVsPawn_2, -2),

        P(knightVsPawn_1, -4),
        P(knightVsPawn_2, -20),

        /*
         *  Board control
         */

        P(controlCenter, 28),
        P(controlExtendedCenter, 35),
        P(controlOutside, 21),

        /*
         *  King safety
         */

        // scaling
        P(safetyScalingOffset, -971),
        P(safetyAndQueen, -1836),
        P(safetyAndRook, 3215),
        P(safetyAndBishop, 1864),
        P(safetyAndKnight, 1913),
        P(safetyAndPawn, -537),
        P(safetyVsQueen, 12756),
        P(safetyVsRook, -3689),
        P(safetyVsBishop, -2409),
        P(safetyVsKnight, -2190),
        P(safetyVsPawn, -821),

        // shelter
        P(shelterPawn_0, 148), // rank3
        P(shelterPawn_1, 269), // rank4
        P(shelterPawn_2, 264), // rank5
        P(shelterPawn_3, 116), // rank6
        P(shelterPawn_4, -204), // rank7
        P(shelterPawn_5, 294), // no pawn

        P(shelterKing_0, -183), // fileA (fileH)
        P(shelterKing_1, -244),
        P(shelterKing_2, -163), // -fileD (-fileE)

        P(shelterWalkingKing, 52), // rank2 or up
        P(shelterCastled, 40), // fraction of 256

        // attacks
        P(attackSquares_0, 86), // 0 attacks
        P(attackSquares_1, 14),
        P(attackSquares_2, -96),
        P(attackSquares_3, -200),
        P(attackSquares_4, -261),
        P(attackSquares_5, -230), // -(6 or more attacks)

        P(attackByPawn_0, 135),
        P(attackByPawn_1, 277),
        P(attackByPawn_2, 73),
        P(attackByMinor_0, -110),
        P(attackByMinor_1, 13),
        P(attackByMinor_2, -279),
        P(attackByRook_0, 419),
        P(attackByRook_1, 989),
        P(attackByRook_2, -605),
        P(attackByQueen, 180),
        P(attackByKing, -217),

        // TODO: pawn storm

        // king mobility
        P(mobilityKing_0, 111),
        P(mobilityKing_1, 51),

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

        P(pawnByFile_0, 26), // fileA
        P(pawnByFile_1, 46),
        P(pawnByFile_2, 10),
        P(pawnByFile_3, -34),
        P(pawnByFile_4, -59),
        P(pawnByFile_5, -24),
        P(pawnByFile_6, 46), // -fileH

        P(pawnByFile_0x, -133), // fileA
        P(pawnByFile_1x, -220),
        P(pawnByFile_2x, -281),
        P(pawnByFile_3x, -360),
        P(pawnByFile_4x, -349),
        P(pawnByFile_5x, -280),
        P(pawnByFile_6x, -120), // -fileH

        // 5 parameters for 6 pawn ranks
        P(pawnByRank_0, -51), // rank2
        P(pawnByRank_1, -91),
        P(pawnByRank_2, -91),
        P(pawnByRank_3, -22),
        P(pawnByRank_4, 78), // -rank7

        P(doubledPawnA, -188),
        P(doubledPawnB, -116),
        P(doubledPawnC, -150),
        P(doubledPawnD, -115),
        P(doubledPawnE, -109),
        P(doubledPawnF, -53),
        P(doubledPawnG, -40),
        P(doubledPawnH, -210),

        P(backwardPawnClosedByRank_0, 19),
        P(backwardPawnClosedByRank_1, 14),
        P(backwardPawnClosedByRank_2, -8),
        P(backwardPawnClosedByRank_3, -3),
        P(backwardPawnClosedByRank_4, 0),

        P(backwardPawnOpenByRank_0, 23),
        P(backwardPawnOpenByRank_1, 15),
        P(backwardPawnOpenByRank_2, 7),
        P(backwardPawnOpenByRank_3, 44),
        P(backwardPawnOpenByRank_4, 0),

        P(backwardPawnClosedA, 50),
        P(backwardPawnClosedB, -6),
        P(backwardPawnClosedC, 22),
        P(backwardPawnClosedD, -32),
        P(backwardPawnClosedE, -52),
        P(backwardPawnClosedF, -23),
        P(backwardPawnClosedG, -9),
        P(backwardPawnClosedH, 43),

        P(backwardPawnOpenA, -17),
        P(backwardPawnOpenB, -54),
        P(backwardPawnOpenC, -47),
        P(backwardPawnOpenD, -76),
        P(backwardPawnOpenE, -70),
        P(backwardPawnOpenF, -66),
        P(backwardPawnOpenG, -66),
        P(backwardPawnOpenH, 0),

        P(rammedWeakPawnA, -15),
        P(rammedWeakPawnB, -3),
        P(rammedWeakPawnC, -10),
        P(rammedWeakPawnD, -13),
        P(rammedWeakPawnE, 6),
        P(rammedWeakPawnF, -11),
        P(rammedWeakPawnG, -26),
        P(rammedWeakPawnH, -56),

        P(isolatedPawnClosedA, 157),
        P(isolatedPawnClosedB, 54),
        P(isolatedPawnClosedC, 13),
        P(isolatedPawnClosedD, 29),
        P(isolatedPawnClosedE, -79),
        P(isolatedPawnClosedF, -93),
        P(isolatedPawnClosedG, -135),
        P(isolatedPawnClosedH, -36),

        P(isolatedPawnOpenA, 130),
        P(isolatedPawnOpenB, 65),
        P(isolatedPawnOpenC, -35),
        P(isolatedPawnOpenD, -75),
        P(isolatedPawnOpenE, -95),
        P(isolatedPawnOpenF, -51),
        P(isolatedPawnOpenG, -25),
        P(isolatedPawnOpenH, 52),

        P(sidePawnClosedA, 105),
        P(sidePawnClosedB, 53),
        P(sidePawnClosedC, 81),
        P(sidePawnClosedD, -11),
        P(sidePawnClosedE, -38),
        P(sidePawnClosedF, -65),
        P(sidePawnClosedG, -133),
        P(sidePawnClosedH, -50),

        P(sidePawnOpenA, 78),
        P(sidePawnOpenB, 67),
        P(sidePawnOpenC, 20),
        P(sidePawnOpenD, -38),
        P(sidePawnOpenE, -34),
        P(sidePawnOpenF, -20),
        P(sidePawnOpenG, 4),
        P(sidePawnOpenH, 22),

        P(middlePawnClosedA, 0),
        P(middlePawnClosedB, 21),
        P(middlePawnClosedC, 105),
        P(middlePawnClosedD, 3),
        P(middlePawnClosedE, -5),
        P(middlePawnClosedF, -27),
        P(middlePawnClosedG, -101),
        P(middlePawnClosedH, 0),

        P(middlePawnOpenA, 0),
        P(middlePawnOpenB, -37),
        P(middlePawnOpenC, 19),
        P(middlePawnOpenD, -12),
        P(middlePawnOpenE, 6),
        P(middlePawnOpenF, -45),
        P(middlePawnOpenG, -50),
        P(middlePawnOpenH, 0),

        P(duoPawnA, 9),
        P(duoPawnB, -9),
        P(duoPawnC, -5),
        P(duoPawnD, 28),
        P(duoPawnE, 26),
        P(duoPawnF, 15),
        P(duoPawnG, 28),
        P(duoPawnH, -59),

        P(openFilePawn_0, -19),
        P(openFilePawn_1, -23),
        P(openFilePawn_2, 34),
        P(openFilePawn_3, 16),
        P(openFilePawn_4, 89),
        P(openFilePawn_5, 281),

        P(stoppedPawn_0, -41),
        P(stoppedPawn_1, 22),
        P(stoppedPawn_2, 22),
        P(stoppedPawn_3, -6),
        P(stoppedPawn_4, 0),
        P(stoppedPawn_5, 0),

        P(duoPawn_0, 3),
        P(duoPawn_1, -2),
        P(duoPawn_2, 30),
        P(duoPawn_3, 147),
        P(duoPawn_4, 519),
        P(duoPawn_5, 1484),

        P(trailingPawn_0, -12),
        P(trailingPawn_1, -49),
        P(trailingPawn_2, -62),
        P(trailingPawn_3, -27),
        P(trailingPawn_4, -5),
        P(trailingPawn_5, -54),

        P(capturePawn_0, 103),
        P(capturePawn_1, -62),
        P(capturePawn_2, -82),
        P(capturePawn_3, 49),
        P(capturePawn_4, 276),
        P(capturePawn_5, 0),

        P(defendedPawn_0, 0),
        P(defendedPawn_1, 61),
        P(defendedPawn_2, 9),
        P(defendedPawn_3, 108),
        P(defendedPawn_4, 209),
        P(defendedPawn_5, 151),

        P(pawnLever_0, 0),
        P(pawnLever_1, 0),
        P(pawnLever_2, -34),
        P(pawnLever_3, -34),
        P(pawnLever_4, -27),
        P(pawnLever_5, -20),
        P(pawnLever_6, 0),

        P(rammedPawn_0, -36),
        P(rammedPawn_1, 6),
        P(rammedPawn_2, -37),
        P(rammedPawn_3, -49),
        P(rammedPawn_4, 20),
        P(rammedPawn_5, 0),

        P(mobilePawn_0, 1),
        P(mobilePawn_1, 15),
        P(mobilePawn_2, 17),
        P(mobilePawn_3, 21),
        P(mobilePawn_4, 48),
        P(mobilePawn_5, 129),

        // Quadratic polynomials for passers
        P(passerA_0, -60),
        P(passerB_0, 34),
        P(passerC_0, 152),
        P(passerD_0, 136),
        P(passerE_0, 286),
        P(passerF_0, 181),
        P(passerG_0, 104),
        P(passerH_0, 57),

        P(passerA_1, 68),
        P(passerB_1, -6),
        P(passerC_1, -67),
        P(passerD_1, -44),
        P(passerE_1, -107),
        P(passerF_1, -16),
        P(passerG_1, 66),
        P(passerH_1, 100),

        P(passerA_2, 138),
        P(passerB_2, 199),
        P(passerC_2, 234),
        P(passerD_2, 193),
        P(passerE_2, 245),
        P(passerF_2, 178),
        P(passerG_2, 106),
        P(passerH_2, 68),

        P(passerScalingOffset, 3976),
        P(passerAndQueen, 1101),
        P(passerAndRook, 580),
        P(passerAndBishop, 551),
        P(passerAndKnight, 242),
        P(passerAndPawn, -613),
        P(passerVsQueen, -430),
        P(passerVsRook, -559),
        P(passerVsBishop, -1479),
        P(passerVsKnight, -995),
        P(passerVsPawn, 134),

        P(candidateByRank_0, 10),
        P(candidateByRank_1, 33),
        P(candidateByRank_2, 77),
        P(candidateByRank_3, 174),
        P(candidateByRank_4, 271),

        P(candidateA, 15),
        P(candidateB, -50),
        P(candidateC, -2),
        P(candidateD, -34),
        P(candidateE, 13),
        P(candidateF, 28),
        P(candidateG, -7),
        P(candidateH, 38),

        /*
         *  Knights
         */

        // Kings on same side
        P(knightByFile_0, -183), // fileA
        P(knightByFile_1, -219),
        P(knightByFile_2, -233),
        P(knightByFile_3, -165),
        P(knightByFile_4, -99),
        P(knightByFile_5, -24),
        P(knightByFile_6, 24), // -fileH

        // Opposite side kings
        P(knightByFile_0x, -132), // fileA
        P(knightByFile_1x, -166),
        P(knightByFile_2x, -134),
        P(knightByFile_3x, -37),
        P(knightByFile_4x, 48),
        P(knightByFile_5x, 137),
        P(knightByFile_6x, 119), // -fileH

        P(knightByRank_0, -128), // rank1
        P(knightByRank_1, -212),
        P(knightByRank_2, -230),
        P(knightByRank_3, -171),
        P(knightByRank_4, -62),
        P(knightByRank_5, 87),
        P(knightByRank_6, 145), // -rank8

        P(knightAndSpan_0, -97), // span0 (pawnless)
        P(knightAndSpan_1, -66),
        P(knightAndSpan_2, -10),
        P(knightAndSpan_3, 30), // -span4 (7-8 files)

        P(knightVsSpan_0, -262), // span0 (pawnless)
        P(knightVsSpan_1, -150),
        P(knightVsSpan_2, -43),
        P(knightVsSpan_3, 14), // -span4 (7-8 files)

        /*
         *  Bishops
         */

#if 0
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
        P(bishopBySquare_0, -162),
        P(bishopBySquare_1, 7),
        P(bishopBySquare_2, -68),
        P(bishopBySquare_3, 97),
        P(bishopBySquare_4, -57),
        P(bishopBySquare_5, -13),
        P(bishopBySquare_6, -83),
        P(bishopBySquare_7, 43),

        P(bishopBySquare_8, -27),
        P(bishopBySquare_9, -34),
        P(bishopBySquare_10, 97),
        P(bishopBySquare_11, -2),
        P(bishopBySquare_12, 37),
        P(bishopBySquare_13, 101),
        P(bishopBySquare_14, 54),
        P(bishopBySquare_15, 116),

        P(bishopBySquare_16, -106),
        P(bishopBySquare_17, 60),
        P(bishopBySquare_18, 44),
        P(bishopBySquare_19, 62),
        P(bishopBySquare_20, 49),
        P(bishopBySquare_21, 90),
        P(bishopBySquare_22, 64),
        P(bishopBySquare_23, 103),

        P(bishopBySquare_24, -68),
        P(bishopBySquare_25, -45),
        P(bishopBySquare_26, 65),
        P(bishopBySquare_27, 106),
        P(bishopBySquare_28, 162),
        P(bishopBySquare_29, 103),
        P(bishopBySquare_30, 102),
        P(bishopBySquare_31, 73),

        P(bishopBySquare_32, -68),
        P(bishopBySquare_33, 22),
        P(bishopBySquare_34, 23),
        P(bishopBySquare_35, 94),
        P(bishopBySquare_36, 111),
        P(bishopBySquare_37, 140),
        P(bishopBySquare_38, 24),
        P(bishopBySquare_39, 74),

        P(bishopBySquare_40, -71),
        P(bishopBySquare_41, 9),
        P(bishopBySquare_42, 66),
        P(bishopBySquare_43, 3),
        P(bishopBySquare_44, 123),
        P(bishopBySquare_45, 237),
        P(bishopBySquare_46, 47),
        P(bishopBySquare_47, 8),

        P(bishopBySquare_48, -55),
        P(bishopBySquare_49, 51),
        P(bishopBySquare_50, 46),
        P(bishopBySquare_51, 55),
        P(bishopBySquare_52, -1),
        P(bishopBySquare_53, 160),
        P(bishopBySquare_54, -54),
        P(bishopBySquare_55, 20),

        P(bishopBySquare_56, -212),
        P(bishopBySquare_57, -55),
        P(bishopBySquare_58, 0),
        P(bishopBySquare_59, 35),
        P(bishopBySquare_60, 4),
        P(bishopBySquare_61, 116),
        P(bishopBySquare_62, -246),
        P(bishopBySquare_63, -82),

        P(bishopBySquare_0x, -87),
        P(bishopBySquare_1x, -24),
        P(bishopBySquare_2x, 38),
        P(bishopBySquare_3x, -5),
        P(bishopBySquare_4x, 18),
        P(bishopBySquare_5x, 95),
        P(bishopBySquare_6x, -228),
        P(bishopBySquare_7x, -99),

        P(bishopBySquare_8x, -74),
        P(bishopBySquare_9x, -53),
        P(bishopBySquare_10x, 20),
        P(bishopBySquare_11x, 96),
        P(bishopBySquare_12x, 44),
        P(bishopBySquare_13x, 155),
        P(bishopBySquare_14x, 34),
        P(bishopBySquare_15x, 17),

        P(bishopBySquare_16x, -87),
        P(bishopBySquare_17x, 10),
        P(bishopBySquare_18x, 88),
        P(bishopBySquare_19x, 71),
        P(bishopBySquare_20x, 158),
        P(bishopBySquare_21x, 173),
        P(bishopBySquare_22x, 102),
        P(bishopBySquare_23x, 36),

        P(bishopBySquare_24x, -63),
        P(bishopBySquare_25x, 16),
        P(bishopBySquare_26x, 86),
        P(bishopBySquare_27x, 124),
        P(bishopBySquare_28x, 158),
        P(bishopBySquare_29x, 201),
        P(bishopBySquare_30x, 101),
        P(bishopBySquare_31x, 132),

        P(bishopBySquare_32x, -32),
        P(bishopBySquare_33x, 83),
        P(bishopBySquare_34x, 121),
        P(bishopBySquare_35x, 158),
        P(bishopBySquare_36x, 208),
        P(bishopBySquare_37x, 185),
        P(bishopBySquare_38x, 185),
        P(bishopBySquare_39x, 118),

        P(bishopBySquare_40x, -70),
        P(bishopBySquare_41x, 91),
        P(bishopBySquare_42x, 140),
        P(bishopBySquare_43x, 175),
        P(bishopBySquare_44x, 171),
        P(bishopBySquare_45x, 189),
        P(bishopBySquare_46x, 149),
        P(bishopBySquare_47x, 159),

        P(bishopBySquare_48x, 1),
        P(bishopBySquare_49x, 143),
        P(bishopBySquare_50x, 146),
        P(bishopBySquare_51x, 76),
        P(bishopBySquare_52x, 150),
        P(bishopBySquare_53x, 175),
        P(bishopBySquare_54x, 169),
        P(bishopBySquare_55x, 165),

        P(bishopBySquare_56x, -94),
        P(bishopBySquare_57x, 62),
        P(bishopBySquare_58x, 39),
        P(bishopBySquare_59x, 90),
        P(bishopBySquare_60x, 72),
        P(bishopBySquare_61x, 52),
        P(bishopBySquare_62x, -33),
        P(bishopBySquare_63x, -8),
#endif

        P(bishopAndSpan_0, -205), // span0 (pawnless)
        P(bishopAndSpan_1, -161),
        P(bishopAndSpan_2, -115),
        P(bishopAndSpan_3, -76), // -span4 (7-8 files)

        P(bishopVsSpan_0, -401), // span0 (pawnless)
        P(bishopVsSpan_1, -284),
        P(bishopVsSpan_2, -177),
        P(bishopVsSpan_3, -96), // -span4 (7-8 files)

        P(bishopAndLikePawn_1, -38),
        P(bishopAndLikePawn_2, -4),
        P(bishopAndLikeRammedPawn, -54),

        P(bishopVsLikePawn_1, -26),
        P(bishopVsLikePawn_2, -5),
        P(bishopVsLikeRammedPawn, 6),

        /*
         *  Rooks
         */

        P(rookByFile_0, -65), // fileA
        P(rookByFile_1, -61),
        P(rookByFile_2, -29), // -fileD

        P(rookByRank_0, -113), // rank1
        P(rookByRank_1, -280),
        P(rookByRank_2, -419),
        P(rookByRank_3, -487),
        P(rookByRank_4, -420),
        P(rookByRank_5, -287),
        P(rookByRank_6, -138), // -rank8

        P(rookOnHalfOpen_0, 164), // fileA
        P(rookOnHalfOpen_1, 101),
        P(rookOnHalfOpen_2, 66),
        P(rookOnHalfOpen_3, 59), // fileD

        P(rookOnOpen_0, 167), // fileA
        P(rookOnOpen_1, 135),
        P(rookOnOpen_2, 109),
        P(rookOnOpen_3, 140), // fileD

        /*
         *  Queens
         */

        P(queenByFile_0, -112), // fileA
        P(queenByFile_1, -168),
        P(queenByFile_2, -170),
        P(queenByFile_3, -147),
        P(queenByFile_4, -114),
        P(queenByFile_5, -82),
        P(queenByFile_6, -31), // -fileH

        P(queenByFile_0x, -8), // fileA
        P(queenByFile_1x, -8),
        P(queenByFile_2x, 19),
        P(queenByFile_3x, 74),
        P(queenByFile_4x, 134),
        P(queenByFile_5x, 169),
        P(queenByFile_6x, 130), // -fileH

        P(queenByRank_0, -134), // rank1
        P(queenByRank_1, -216),
        P(queenByRank_2, -280),
        P(queenByRank_3, -309),
        P(queenByRank_4, -270),
        P(queenByRank_5, -182),
        P(queenByRank_6, -107), // -rank8

        /*
         *  Kings
         */

        P(kingByFile_0, -51), // fileA
        P(kingByFile_1, -57),
        P(kingByFile_2, -42), // -fileD

        P(kingByRank_0, -253), // rank1
        P(kingByRank_1, -419),
        P(kingByRank_2, -554),
        P(kingByRank_3, -597),
        P(kingByRank_4, -493),
        P(kingByRank_5, -256),
        P(kingByRank_6, -25), // -rank8

        // Drawness. Positive is more drawish. Negative is more sharp
        P(drawOffset, -199),
        P(drawQueen, -88),
        P(drawRook, 86),
        P(drawBishop, 35),
        P(drawKnight, 30),
        P(drawPawn, -802),
        P(drawQueenEnding, 15),
        P(drawRookEnding, 790),
        P(drawKnightEnding, 495),
        P(drawBishopEnding, 1439),
        P(drawPawnEnding, -14699),
        P(drawPawnless, 11),

        P(drawQueenImbalance, -389),
        P(drawRookImbalance, 507),
        P(drawMinorImbalance, 104),

        P(drawUnlikeBishops, 2748),
        P(drawUnlikeBishopsAndQueens, 799),
        P(drawUnlikeBishopsAndRooks, 409),
        P(drawUnlikeBishopsAndKnights, 882),

        P(drawRammed_0, 620), // fileA/H
        P(drawRammed_1, 563),
        P(drawRammed_2, 595),
        P(drawRammed_3, 479), // fileD/E
// }

/*----------------------------------------------------------------------+
 |                                                                      |
 +----------------------------------------------------------------------*/

