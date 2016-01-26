
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
        P(winBonus, 3147),

        P(castleK, -123),
        P(castleQ, -103),
        P(castleKQ, -38),

        P(queenValue, 6689),
        P(rookValue, 4457),
        P(bishopValue, 2474),
        P(knightValue, 2327),
        P(pawnValue1, 991),
        P(pawnValue2, 1662),
        P(pawnValue3, 2341),
        P(pawnValue4, 3027),
        P(pawnValue5, 3698),
        P(pawnValue6, 4351),
        P(pawnValue7, 4963),
        P(pawnValue8, 5488),

        P(queenAndQueen, -732),
        P(queenAndRook, -631),
        P(queenAndBishop, 73),
        P(queenAndKnight, -4),
        P(queenAndPawn_1, -90),
        P(queenAndPawn_2, -90),

        P(rookAndRook, -289),
        P(rookAndBishop, -109),
        P(rookAndKnight, -121),
        P(rookAndPawn_1, -55),
        P(rookAndPawn_2, -54),

        P(bishopAndBishop, 118),
        P(bishopAndKnight, 20),
        P(bishopAndPawn_1, -52),
        P(bishopAndPawn_2, -3),

        P(knightAndKnight, -31),
        P(knightAndPawn_1, -24),
        P(knightAndPawn_2, -3),

        P(queenVsRook, -291),
        P(queenVsBishop, 64),
        P(queenVsKnight, 3),
        P(queenVsPawn_1, 138),
        P(queenVsPawn_2, -121),

        P(rookVsBishop, -30),
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
        P(safetyScalingOffset, -1037),
        P(safetyAndQueen, -1836),
        P(safetyAndRook, 3215),
        P(safetyAndBishop, 1864),
        P(safetyAndKnight, 1913),
        P(safetyAndPawn, -537),
        P(safetyVsQueen, 12756),
        P(safetyVsRook, -3669),
        P(safetyVsBishop, -2377),
        P(safetyVsKnight, -2134),
        P(safetyVsPawn, -821),

        // shelter
        P(shelterPawn_0, 149), // rank3
        P(shelterPawn_1, 269), // rank4
        P(shelterPawn_2, 264), // rank5
        P(shelterPawn_3, 118), // rank6
        P(shelterPawn_4, -194), // rank7
        P(shelterPawn_5, 293), // no pawn

        P(shelterKing_0, -183), // fileA (fileH)
        P(shelterKing_1, -241),
        P(shelterKing_2, -162), // -fileD (-fileE)

        P(shelterWalkingKing, 51), // rank2 or up
        P(shelterCastled, 40), // fraction of 256

        // attacks
        P(attackSquares_0, 86), // 0 attacks
        P(attackSquares_1, 14),
        P(attackSquares_2, -95),
        P(attackSquares_3, -198),
        P(attackSquares_4, -261),
        P(attackSquares_5, -230), // -(6 or more attacks)

        P(attackByPawn_0, 135),
        P(attackByPawn_1, 275),
        P(attackByPawn_2, 73),
        P(attackByMinor_0, -110),
        P(attackByMinor_1, 13),
        P(attackByMinor_2, -279),
        P(attackByRook_0, 395),
        P(attackByRook_1, 947),
        P(attackByRook_2, -579),
        P(attackByQueen, 178),
        P(attackByKing, -220),

        // TODO: pawn storm

        // king mobility
        P(mobilityKing_0, 110),
        P(mobilityKing_1, 53),

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

        P(pawnByFile_0, 23), // fileA
        P(pawnByFile_1, 46),
        P(pawnByFile_2, 10),
        P(pawnByFile_3, -35),
        P(pawnByFile_4, -59),
        P(pawnByFile_5, -26),
        P(pawnByFile_6, 45), // -fileH

        P(pawnByFile_0x, -133), // fileA
        P(pawnByFile_1x, -213),
        P(pawnByFile_2x, -274),
        P(pawnByFile_3x, -351),
        P(pawnByFile_4x, -345),
        P(pawnByFile_5x, -278),
        P(pawnByFile_6x, -118), // -fileH

        // 5 parameters for 6 pawn ranks
        P(pawnByRank_0, -51), // rank2
        P(pawnByRank_1, -91),
        P(pawnByRank_2, -91),
        P(pawnByRank_3, -22),
        P(pawnByRank_4, 78), // -rank7

        P(doubledPawnA, -183),
        P(doubledPawnB, -117),
        P(doubledPawnC, -151),
        P(doubledPawnD, -114),
        P(doubledPawnE, -110),
        P(doubledPawnF, -54),
        P(doubledPawnG, -40),
        P(doubledPawnH, -205),

        P(backwardPawnClosedByRank_0, 19),
        P(backwardPawnClosedByRank_1, 14),
        P(backwardPawnClosedByRank_2, -6),
        P(backwardPawnClosedByRank_3, -8),
        P(backwardPawnClosedByRank_4, 0),

        P(backwardPawnOpenByRank_0, 21),
        P(backwardPawnOpenByRank_1, 15),
        P(backwardPawnOpenByRank_2, 8),
        P(backwardPawnOpenByRank_3, 49),
        P(backwardPawnOpenByRank_4, 0),

        P(backwardPawnClosedA, 52),
        P(backwardPawnClosedB, -5),
        P(backwardPawnClosedC, 25),
        P(backwardPawnClosedD, -30),
        P(backwardPawnClosedE, -52),
        P(backwardPawnClosedF, -22),
        P(backwardPawnClosedG, -8),
        P(backwardPawnClosedH, 44),

        P(backwardPawnOpenA, -14),
        P(backwardPawnOpenB, -55),
        P(backwardPawnOpenC, -48),
        P(backwardPawnOpenD, -76),
        P(backwardPawnOpenE, -69),
        P(backwardPawnOpenF, -66),
        P(backwardPawnOpenG, -69),
        P(backwardPawnOpenH, 1),

        P(rammedWeakPawnA, -16),
        P(rammedWeakPawnB, -4),
        P(rammedWeakPawnC, -14),
        P(rammedWeakPawnD, -13),
        P(rammedWeakPawnE, 3),
        P(rammedWeakPawnF, -13),
        P(rammedWeakPawnG, -29),
        P(rammedWeakPawnH, -58),

        P(isolatedPawnClosedA, 156),
        P(isolatedPawnClosedB, 55),
        P(isolatedPawnClosedC, 10),
        P(isolatedPawnClosedD, 29),
        P(isolatedPawnClosedE, -72),
        P(isolatedPawnClosedF, -93),
        P(isolatedPawnClosedG, -132),
        P(isolatedPawnClosedH, -33),

        P(isolatedPawnOpenA, 129),
        P(isolatedPawnOpenB, 65),
        P(isolatedPawnOpenC, -34),
        P(isolatedPawnOpenD, -74),
        P(isolatedPawnOpenE, -99),
        P(isolatedPawnOpenF, -51),
        P(isolatedPawnOpenG, -23),
        P(isolatedPawnOpenH, 51),

        P(sidePawnClosedA, 100),
        P(sidePawnClosedB, 52),
        P(sidePawnClosedC, 79),
        P(sidePawnClosedD, -13),
        P(sidePawnClosedE, -36),
        P(sidePawnClosedF, -65),
        P(sidePawnClosedG, -130),
        P(sidePawnClosedH, -49),

        P(sidePawnOpenA, 78),
        P(sidePawnOpenB, 67),
        P(sidePawnOpenC, 19),
        P(sidePawnOpenD, -39),
        P(sidePawnOpenE, -35),
        P(sidePawnOpenF, -19),
        P(sidePawnOpenG, 4),
        P(sidePawnOpenH, 22),

        P(middlePawnClosedA, 0),
        P(middlePawnClosedB, 18),
        P(middlePawnClosedC, 105),
        P(middlePawnClosedD, -2),
        P(middlePawnClosedE, -4),
        P(middlePawnClosedF, -25),
        P(middlePawnClosedG, -101),
        P(middlePawnClosedH, 0),

        P(middlePawnOpenA, 0),
        P(middlePawnOpenB, -39),
        P(middlePawnOpenC, 22),
        P(middlePawnOpenD, -15),
        P(middlePawnOpenE, 7),
        P(middlePawnOpenF, -45),
        P(middlePawnOpenG, -51),
        P(middlePawnOpenH, 0),

        P(duoPawnA, 9),
        P(duoPawnB, -10),
        P(duoPawnC, -4),
        P(duoPawnD, 29),
        P(duoPawnE, 26),
        P(duoPawnF, 14),
        P(duoPawnG, 27),
        P(duoPawnH, -60),

        P(openFilePawn_0, -19),
        P(openFilePawn_1, -22),
        P(openFilePawn_2, 33),
        P(openFilePawn_3, 7),
        P(openFilePawn_4, 82),
        P(openFilePawn_5, 269),

        P(stoppedPawn_0, -45),
        P(stoppedPawn_1, 22),
        P(stoppedPawn_2, 22),
        P(stoppedPawn_3, -5),
        P(stoppedPawn_4, 0),
        P(stoppedPawn_5, 0),

        P(duoPawn_0, 3),
        P(duoPawn_1, -2),
        P(duoPawn_2, 30),
        P(duoPawn_3, 148),
        P(duoPawn_4, 513),
        P(duoPawn_5, 1468),

        P(trailingPawn_0, -12),
        P(trailingPawn_1, -50),
        P(trailingPawn_2, -62),
        P(trailingPawn_3, -27),
        P(trailingPawn_4, -10),
        P(trailingPawn_5, -54),

        P(capturePawn_0, 102),
        P(capturePawn_1, -62),
        P(capturePawn_2, -85),
        P(capturePawn_3, 51),
        P(capturePawn_4, 278),
        P(capturePawn_5, 0),

        P(defendedPawn_0, 0),
        P(defendedPawn_1, 60),
        P(defendedPawn_2, 10),
        P(defendedPawn_3, 108),
        P(defendedPawn_4, 208),
        P(defendedPawn_5, 149),

        P(pawnLever_0, 0),
        P(pawnLever_1, 0),
        P(pawnLever_2, -34),
        P(pawnLever_3, -34),
        P(pawnLever_4, -26),
        P(pawnLever_5, -20),
        P(pawnLever_6, 0),

        P(rammedPawn_0, -36),
        P(rammedPawn_1, 8),
        P(rammedPawn_2, -42),
        P(rammedPawn_3, -50),
        P(rammedPawn_4, 20),
        P(rammedPawn_5, 0),

        P(mobilePawn_0, 1),
        P(mobilePawn_1, 15),
        P(mobilePawn_2, 17),
        P(mobilePawn_3, 22),
        P(mobilePawn_4, 48),
        P(mobilePawn_5, 123),

        // Quadratic polynomials for passers
        P(passerA_0, -59),
        P(passerB_0, 23),
        P(passerC_0, 153),
        P(passerD_0, 136),
        P(passerE_0, 266),
        P(passerF_0, 181),
        P(passerG_0, 97),
        P(passerH_0, 60),

        P(passerA_1, 68),
        P(passerB_1, -6),
        P(passerC_1, -66),
        P(passerD_1, -42),
        P(passerE_1, -99),
        P(passerF_1, -17),
        P(passerG_1, 64),
        P(passerH_1, 99),

        P(passerA_2, 140),
        P(passerB_2, 199),
        P(passerC_2, 235),
        P(passerD_2, 193),
        P(passerE_2, 247),
        P(passerF_2, 184),
        P(passerG_2, 110),
        P(passerH_2, 69),

        P(passerScalingOffset, 3976),
        P(passerAndQueen, 1032),
        P(passerAndRook, 574),
        P(passerAndBishop, 557),
        P(passerAndKnight, 225),
        P(passerAndPawn, -618),
        P(passerVsQueen, -343),
        P(passerVsRook, -558),
        P(passerVsBishop, -1440),
        P(passerVsKnight, -955),
        P(passerVsPawn, 125),

        P(candidateByRank_0, 8),
        P(candidateByRank_1, 30),
        P(candidateByRank_2, 81),
        P(candidateByRank_3, 171),
        P(candidateByRank_4, 262),

        P(candidateA, 12),
        P(candidateB, -56),
        P(candidateC, -6),
        P(candidateD, -34),
        P(candidateE, 17),
        P(candidateF, 31),
        P(candidateG, -9),
        P(candidateH, 41),

        /*
         *  Knights
         */

        // Kings on same side
        P(knightByFile_0, -183), // fileA
        P(knightByFile_1, -219),
        P(knightByFile_2, -232),
        P(knightByFile_3, -165),
        P(knightByFile_4, -101),
        P(knightByFile_5, -25),
        P(knightByFile_6, 23), // -fileH

        // Opposite side kings
        P(knightByFile_0x, -132), // fileA
        P(knightByFile_1x, -166),
        P(knightByFile_2x, -134),
        P(knightByFile_3x, -37),
        P(knightByFile_4x, 47),
        P(knightByFile_5x, 135),
        P(knightByFile_6x, 118), // -fileH

        P(knightByRank_0, -128), // rank1
        P(knightByRank_1, -211),
        P(knightByRank_2, -230),
        P(knightByRank_3, -170),
        P(knightByRank_4, -61),
        P(knightByRank_5, 91),
        P(knightByRank_6, 146), // -rank8

        P(knightAndSpan_0, -85), // span0 (pawnless)
        P(knightAndSpan_1, -56),
        P(knightAndSpan_2, -6),
        P(knightAndSpan_3, 30), // -span4 (7-8 files)

        P(knightVsSpan_0, -217), // span0 (pawnless)
        P(knightVsSpan_1, -118),
        P(knightVsSpan_2, -16),
        P(knightVsSpan_3, 25), // -span4 (7-8 files)

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
        P(bishopBySquare_0, -164),
        P(bishopBySquare_1, 7),
        P(bishopBySquare_2, -69),
        P(bishopBySquare_3, 91),
        P(bishopBySquare_4, -54),
        P(bishopBySquare_5, -14),
        P(bishopBySquare_6, -79),
        P(bishopBySquare_7, 43),

        P(bishopBySquare_8, -29),
        P(bishopBySquare_9, -35),
        P(bishopBySquare_10, 95),
        P(bishopBySquare_11, 0),
        P(bishopBySquare_12, 35),
        P(bishopBySquare_13, 101),
        P(bishopBySquare_14, 56),
        P(bishopBySquare_15, 117),

        P(bishopBySquare_16, -107),
        P(bishopBySquare_17, 58),
        P(bishopBySquare_18, 44),
        P(bishopBySquare_19, 60),
        P(bishopBySquare_20, 49),
        P(bishopBySquare_21, 90),
        P(bishopBySquare_22, 65),
        P(bishopBySquare_23, 107),

        P(bishopBySquare_24, -68),
        P(bishopBySquare_25, -48),
        P(bishopBySquare_26, 62),
        P(bishopBySquare_27, 106),
        P(bishopBySquare_28, 158),
        P(bishopBySquare_29, 102),
        P(bishopBySquare_30, 102),
        P(bishopBySquare_31, 77),

        P(bishopBySquare_32, -68),
        P(bishopBySquare_33, 21),
        P(bishopBySquare_34, 21),
        P(bishopBySquare_35, 89),
        P(bishopBySquare_36, 108),
        P(bishopBySquare_37, 137),
        P(bishopBySquare_38, 27),
        P(bishopBySquare_39, 73),

        P(bishopBySquare_40, -72),
        P(bishopBySquare_41, 12),
        P(bishopBySquare_42, 66),
        P(bishopBySquare_43, 0),
        P(bishopBySquare_44, 118),
        P(bishopBySquare_45, 235),
        P(bishopBySquare_46, 42),
        P(bishopBySquare_47, 9),

        P(bishopBySquare_48, -54),
        P(bishopBySquare_49, 49),
        P(bishopBySquare_50, 46),
        P(bishopBySquare_51, 56),
        P(bishopBySquare_52, -4),
        P(bishopBySquare_53, 159),
        P(bishopBySquare_54, -57),
        P(bishopBySquare_55, 24),

        P(bishopBySquare_56, -211),
        P(bishopBySquare_57, -53),
        P(bishopBySquare_58, 0),
        P(bishopBySquare_59, 34),
        P(bishopBySquare_60, 3),
        P(bishopBySquare_61, 116),
        P(bishopBySquare_62, -250),
        P(bishopBySquare_63, -77),

        P(bishopBySquare_0x, -87),
        P(bishopBySquare_1x, -22),
        P(bishopBySquare_2x, 35),
        P(bishopBySquare_3x, -5),
        P(bishopBySquare_4x, 23),
        P(bishopBySquare_5x, 100),
        P(bishopBySquare_6x, -227),
        P(bishopBySquare_7x, -95),

        P(bishopBySquare_8x, -69),
        P(bishopBySquare_9x, -56),
        P(bishopBySquare_10x, 21),
        P(bishopBySquare_11x, 87),
        P(bishopBySquare_12x, 46),
        P(bishopBySquare_13x, 152),
        P(bishopBySquare_14x, 35),
        P(bishopBySquare_15x, 17),

        P(bishopBySquare_16x, -89),
        P(bishopBySquare_17x, 11),
        P(bishopBySquare_18x, 89),
        P(bishopBySquare_19x, 71),
        P(bishopBySquare_20x, 157),
        P(bishopBySquare_21x, 174),
        P(bishopBySquare_22x, 102),
        P(bishopBySquare_23x, 38),

        P(bishopBySquare_24x, -61),
        P(bishopBySquare_25x, 11),
        P(bishopBySquare_26x, 85),
        P(bishopBySquare_27x, 117),
        P(bishopBySquare_28x, 154),
        P(bishopBySquare_29x, 199),
        P(bishopBySquare_30x, 99),
        P(bishopBySquare_31x, 132),

        P(bishopBySquare_32x, -31),
        P(bishopBySquare_33x, 84),
        P(bishopBySquare_34x, 117),
        P(bishopBySquare_35x, 156),
        P(bishopBySquare_36x, 208),
        P(bishopBySquare_37x, 185),
        P(bishopBySquare_38x, 188),
        P(bishopBySquare_39x, 118),

        P(bishopBySquare_40x, -72),
        P(bishopBySquare_41x, 92),
        P(bishopBySquare_42x, 140),
        P(bishopBySquare_43x, 171),
        P(bishopBySquare_44x, 170),
        P(bishopBySquare_45x, 190),
        P(bishopBySquare_46x, 152),
        P(bishopBySquare_47x, 162),

        P(bishopBySquare_48x, -1),
        P(bishopBySquare_49x, 146),
        P(bishopBySquare_50x, 143),
        P(bishopBySquare_51x, 73),
        P(bishopBySquare_52x, 148),
        P(bishopBySquare_53x, 178),
        P(bishopBySquare_54x, 171),
        P(bishopBySquare_55x, 169),

        P(bishopBySquare_56x, -94),
        P(bishopBySquare_57x, 64),
        P(bishopBySquare_58x, 40),
        P(bishopBySquare_59x, 89),
        P(bishopBySquare_60x, 75),
        P(bishopBySquare_61x, 53),
        P(bishopBySquare_62x, -30),
        P(bishopBySquare_63x, -11),
#endif

        P(bishopAndSpan_0, -203), // span0 (pawnless)
        P(bishopAndSpan_1, -162),
        P(bishopAndSpan_2, -116),
        P(bishopAndSpan_3, -79), // -span4 (7-8 files)

        P(bishopVsSpan_0, -395), // span0 (pawnless)
        P(bishopVsSpan_1, -283),
        P(bishopVsSpan_2, -177),
        P(bishopVsSpan_3, -98), // -span4 (7-8 files)

        P(bishopAndLikePawn_1, -37),
        P(bishopAndLikePawn_2, -4),
        P(bishopAndLikeRammedPawn, -56),

        P(bishopVsLikePawn_1, -26),
        P(bishopVsLikePawn_2, -5),
        P(bishopVsLikeRammedPawn, 5),

        /*
         *  Rooks
         */

        P(rookByFile_0, -65), // fileA
        P(rookByFile_1, -60),
        P(rookByFile_2, -29), // -fileD

        P(rookByRank_0, -113), // rank1
        P(rookByRank_1, -280),
        P(rookByRank_2, -419),
        P(rookByRank_3, -486),
        P(rookByRank_4, -418),
        P(rookByRank_5, -283),
        P(rookByRank_6, -135), // -rank8

        P(rookOnHalfOpen_0, 163), // fileA
        P(rookOnHalfOpen_1, 101),
        P(rookOnHalfOpen_2, 67),
        P(rookOnHalfOpen_3, 60), // fileD

        P(rookOnOpen_0, 167), // fileA
        P(rookOnOpen_1, 135),
        P(rookOnOpen_2, 109),
        P(rookOnOpen_3, 140), // fileD

        /*
         *  Queens
         */

        P(queenByFile_0, -113), // fileA
        P(queenByFile_1, -168),
        P(queenByFile_2, -169),
        P(queenByFile_3, -147),
        P(queenByFile_4, -115),
        P(queenByFile_5, -87),
        P(queenByFile_6, -33), // -fileH

        P(queenByFile_0x, -8), // fileA
        P(queenByFile_1x, -8),
        P(queenByFile_2x, 19),
        P(queenByFile_3x, 73),
        P(queenByFile_4x, 131),
        P(queenByFile_5x, 169),
        P(queenByFile_6x, 132), // -fileH

        P(queenByRank_0, -134), // rank1
        P(queenByRank_1, -219),
        P(queenByRank_2, -286),
        P(queenByRank_3, -313),
        P(queenByRank_4, -273),
        P(queenByRank_5, -185),
        P(queenByRank_6, -107), // -rank8

        /*
         *  Kings
         */

        P(kingByFile_0, 42), // fileD
        P(kingByFile_1, 56),
        P(kingByFile_2, 50), // -fileH

        P(kingByRank_0, -251), // rank1
        P(kingByRank_1, -414),
        P(kingByRank_2, -547),
        P(kingByRank_3, -593),
        P(kingByRank_4, -488),
        P(kingByRank_5, -249),
        P(kingByRank_6, -18), // -rank8

        // Drawness. Positive is more drawish. Negative is more sharp
        P(drawOffset, -144),
        P(drawQueen, -120),
        P(drawRook, 87),
        P(drawBishop, 7),
        P(drawKnight, 21),
        P(drawPawn, -792),
        P(drawQueenEnding, -4),
        P(drawRookEnding, 694),
        P(drawKnightEnding, 482),
        P(drawBishopEnding, 1417),
        P(drawPawnEnding, -17892),
        P(drawPawnless, 24),

        P(drawQueenImbalance, -388),
        P(drawRookImbalance, 469),
        P(drawMinorImbalance, 88),

        P(drawUnlikeBishops, 2699),
        P(drawUnlikeBishopsAndQueens, 847),
        P(drawUnlikeBishopsAndRooks, 421),
        P(drawUnlikeBishopsAndKnights, 898),

        P(drawRammed_0, 603), // fileA/H
        P(drawRammed_1, 541),
        P(drawRammed_2, 586),
        P(drawRammed_3, 461), // fileD/E
// }

/*----------------------------------------------------------------------+
 |                                                                      |
 +----------------------------------------------------------------------*/

