
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
        P(tempo, 61),
        P(hanging_0, 23), // Side to move
        P(hanging_1, 112),
        P(hanging_2, 89),
        P(hanging_0x, 14), // Other side
        P(hanging_1x, 3),
        P(hanging_2x, 104),
        P(winBonus, 1513),

        P(castleK, -134),
        P(castleQ, -118),
        P(castleKQ, -69),

        P(queenValue, 6985),
        P(rookValue, 4707),
        P(bishopValue, 2639),
        P(knightValue, 2495),
        P(pawnValue1, 986),
        P(pawnValue2, 1659),
        P(pawnValue3, 2341),
        P(pawnValue4, 3031),
        P(pawnValue5, 3703),
        P(pawnValue6, 4355),
        P(pawnValue7, 4971),
        P(pawnValue8, 5498),

        P(queenAndQueen, -706),
        P(queenAndRook, -727),
        P(queenAndBishop, 91),
        P(queenAndKnight, 34),
        P(queenAndPawn_1, -20),
        P(queenAndPawn_2, -166),

        P(rookAndRook, -295),
        P(rookAndBishop, -110),
        P(rookAndKnight, -112),
        P(rookAndPawn_1, -55),
        P(rookAndPawn_2, -57),

        P(bishopAndBishop, 114),
        P(bishopAndKnight, 8),
        P(bishopAndPawn_1, -48),
        P(bishopAndPawn_2, 3),

        P(knightAndKnight, -27),
        P(knightAndPawn_1, -20),
        P(knightAndPawn_2, -3),

        P(queenVsRook, -365),
        P(queenVsBishop, 75),
        P(queenVsKnight, 20),
        P(queenVsPawn_1, 177),
        P(queenVsPawn_2, -173),

        P(rookVsBishop, -16),
        P(rookVsKnight, 16),
        P(rookVsPawn_1, -2),
        P(rookVsPawn_2, -50),

        P(bishopVsKnight, 7),
        P(bishopVsPawn_1, -10),
        P(bishopVsPawn_2, 3),

        P(knightVsPawn_1, -3),
        P(knightVsPawn_2, -17),

        /*
         *  Board control
         */

        P(controlCenter, 26),
        P(controlExtendedCenter, 33),
        P(controlOutside, 19),

        /*
         *  King safety
         */

        // scaling
        P(safetyScalingOffset, -970),
        P(safetyAndQueen, -1835),
        P(safetyAndRook, 3227),
        P(safetyAndBishop, 1889),
        P(safetyAndKnight, 1937),
        P(safetyAndPawn, -490),
        P(safetyVsQueen, 12715),
        P(safetyVsRook, -3733),
        P(safetyVsBishop, -2430),
        P(safetyVsKnight, -2186),
        P(safetyVsPawn, -836),

        // shelter
        P(shelterPawn_0, 134), // rank3
        P(shelterPawn_1, 264), // rank4
        P(shelterPawn_2, 270), // rank5
        P(shelterPawn_3, 103), // rank6
        P(shelterPawn_4, -236), // rank7
        P(shelterPawn_5, 293), // no pawn

        P(shelterKing_0, -320), // fileA (fileH)
        P(shelterKing_1, -328),
        P(shelterKing_2, -202), // -fileD (-fileE)

        P(shelterWalkingKing, 43), // rank2 or up
        P(shelterCastled, 67), // fraction of 256

        // attacks
        P(attackSquares_0, 63), // 0 attacks
        P(attackSquares_1, -24),
        P(attackSquares_2, -146),
        P(attackSquares_3, -254),
        P(attackSquares_4, -305),
        P(attackSquares_5, -260), // -(6 or more attacks)

        P(attackByPawn_0, 121),
        P(attackByPawn_1, 281),
        P(attackByPawn_2, 71),
        P(attackByMinor_0, -111),
        P(attackByMinor_1, -3),
        P(attackByMinor_2, -247),
        P(attackByRook_0, 490),
        P(attackByRook_1, 1041),
        P(attackByRook_2, -456),
        P(attackByQueen, 181),
        P(attackByKing, -269),

        // TODO: pawn storm

        // king mobility
        P(mobilityKing_0, 97),
        P(mobilityKing_1, 39),

        /*
         *  Self normalization: the 8 coefficients for, for example, file
         *  scoring are built from 7 tunable parameters as given by:
         *
         *                           Parameter
         *                      0  1  2  3  4  5  6
         *               fileA  1  -  -  -  -  -  -
         *               fileB -1  1  -  -  -  -  -
         *               fileC  - -1  1  -  -  -  -
         *  Coefficient  fileD  -  - -1  1  -  -  -
         *               fileE  -  -  - -1  1  -  -
         *               fileF  -  -  -  - -1  1  -
         *               fileG  -  -  -  -  - -1  1
         *               fileH  -  -  -  -  -  - -1
         *
         *  This scheme is used because:
         *  - The sum of coefficients is 0: no overdetermination
         *  - Each parameter has a small scope: no dependency chaining issues
         *  - Suitable for direct use: no preprocessing needed prior to evaluate
         *  Note that this is different from "tuning the differences between files".
         */

        P(pawnByFile_0, 14), // fileA
        P(pawnByFile_1, 38),
        P(pawnByFile_2, 7),
        P(pawnByFile_3, -43),
        P(pawnByFile_4, -79),
        P(pawnByFile_5, -36),
        P(pawnByFile_6, 36), // -fileH

        P(pawnByFile_0x, -104), // fileA
        P(pawnByFile_1x, -187),
        P(pawnByFile_2x, -228),
        P(pawnByFile_3x, -297),
        P(pawnByFile_4x, -316),
        P(pawnByFile_5x, -261),
        P(pawnByFile_6x, -99), // -fileH

        // 5 parameters for 6 pawn ranks
        P(pawnByRank_0, -50), // rank2
        P(pawnByRank_1, -95),
        P(pawnByRank_2, -89),
        P(pawnByRank_3, -13),
        P(pawnByRank_4, 63), // -rank7

        P(doubledPawnA, -165),
        P(doubledPawnB, -91),
        P(doubledPawnC, -115),
        P(doubledPawnD, -81),
        P(doubledPawnE, -65),
        P(doubledPawnF, -18),
        P(doubledPawnG, 10),
        P(doubledPawnH, -165),

        P(backwardPawnClosedByRank_0, 18),
        P(backwardPawnClosedByRank_1, 13),
        P(backwardPawnClosedByRank_2, -13),
        P(backwardPawnClosedByRank_3, -7),
        P(backwardPawnClosedByRank_4, 0), // Doesn't occur

        P(backwardPawnOpenByRank_0, 27),
        P(backwardPawnOpenByRank_1, 18),
        P(backwardPawnOpenByRank_2, 5),
        P(backwardPawnOpenByRank_3, 45),
        P(backwardPawnOpenByRank_4, 0), // Doesn't occur

        P(backwardPawnClosedA, 55),
        P(backwardPawnClosedB, -8),
        P(backwardPawnClosedC, 20),
        P(backwardPawnClosedD, -36),
        P(backwardPawnClosedE, -50),
        P(backwardPawnClosedF, -16),
        P(backwardPawnClosedG, 0),
        P(backwardPawnClosedH, 57),

        P(backwardPawnOpenA, -8),
        P(backwardPawnOpenB, -41),
        P(backwardPawnOpenC, -26),
        P(backwardPawnOpenD, -51),
        P(backwardPawnOpenE, -37),
        P(backwardPawnOpenF, -19),
        P(backwardPawnOpenG, -40),
        P(backwardPawnOpenH, 21),

        P(rammedWeakPawnA, -12),
        P(rammedWeakPawnB, 10),
        P(rammedWeakPawnC, 4),
        P(rammedWeakPawnD, 5),
        P(rammedWeakPawnE, 29),
        P(rammedWeakPawnF, 9),
        P(rammedWeakPawnG, -13),
        P(rammedWeakPawnH, -50),

        P(isolatedPawnClosedA, 116),
        P(isolatedPawnClosedB, 38),
        P(isolatedPawnClosedC, -32),
        P(isolatedPawnClosedD, -17),
        P(isolatedPawnClosedE, -84),
        P(isolatedPawnClosedF, -74),
        P(isolatedPawnClosedG, -69),
        P(isolatedPawnClosedH, 18),

        P(isolatedPawnOpenA, 106),
        P(isolatedPawnOpenB, 50),
        P(isolatedPawnOpenC, -30),
        P(isolatedPawnOpenD, -54),
        P(isolatedPawnOpenE, -60),
        P(isolatedPawnOpenF, -16),
        P(isolatedPawnOpenG, 46),
        P(isolatedPawnOpenH, 88),

        P(sidePawnClosedA, 89),
        P(sidePawnClosedB, 40),
        P(sidePawnClosedC, 45),
        P(sidePawnClosedD, -52),
        P(sidePawnClosedE, -43),
        P(sidePawnClosedF, -47),
        P(sidePawnClosedG, -83),
        P(sidePawnClosedH, 0),

        P(sidePawnOpenA, 70),
        P(sidePawnOpenB, 40),
        P(sidePawnOpenC, 19),
        P(sidePawnOpenD, -21),
        P(sidePawnOpenE, -5),
        P(sidePawnOpenF, 17),
        P(sidePawnOpenG, 54),
        P(sidePawnOpenH, 52),

        P(middlePawnClosedA, 0), // Doesn't occur
        P(middlePawnClosedB, 11),
        P(middlePawnClosedC, 76),
        P(middlePawnClosedD, -33),
        P(middlePawnClosedE, -12),
        P(middlePawnClosedF, -10),
        P(middlePawnClosedG, -76),
        P(middlePawnClosedH, 0), // Doesn't occur

        P(middlePawnOpenA, 0), // Doesn't occur
        P(middlePawnOpenB, -68),
        P(middlePawnOpenC, 23),
        P(middlePawnOpenD, 6),
        P(middlePawnOpenE, 35),
        P(middlePawnOpenF, -9),
        P(middlePawnOpenG, -28),
        P(middlePawnOpenH, 0), // Doesn't occur

        P(duoPawnA, 9),
        P(duoPawnB, -8),
        P(duoPawnC, -1),
        P(duoPawnD, 22),
        P(duoPawnE, 27),
        P(duoPawnF, 14),
        P(duoPawnG, 36),
        P(duoPawnH, -64),

        P(openFilePawn_0, -17),
        P(openFilePawn_1, -21),
        P(openFilePawn_2, 43),
        P(openFilePawn_3, 28),
        P(openFilePawn_4, 145),
        P(openFilePawn_5, 244),

        P(stoppedPawn_0, -35),
        P(stoppedPawn_1, 29),
        P(stoppedPawn_2, 21),
        P(stoppedPawn_3, -15),
        P(stoppedPawn_4, 0), // Doesn't occur
        P(stoppedPawn_5, 0), // Doesn't occur

        P(duoPawn_0, 1),
        P(duoPawn_1, -2),
        P(duoPawn_2, 29),
        P(duoPawn_3, 144),
        P(duoPawn_4, 489),
        P(duoPawn_5, 1487),

        P(trailingPawn_0, -12),
        P(trailingPawn_1, -47),
        P(trailingPawn_2, -69),
        P(trailingPawn_3, -44),
        P(trailingPawn_4, -18),
        P(trailingPawn_5, -32),

        P(capturePawn_0, 90),
        P(capturePawn_1, -69),
        P(capturePawn_2, -88),
        P(capturePawn_3, 28),
        P(capturePawn_4, 278),
        P(capturePawn_5, 0),

        P(defendedPawn_0, 0), // Doesn't occur
        P(defendedPawn_1, 61),
        P(defendedPawn_2, 8),
        P(defendedPawn_3, 118),
        P(defendedPawn_4, 247),
        P(defendedPawn_5, 256),

        P(pawnLever_0, 0), // Doesn't occur
        P(pawnLever_1, 0), // Doesn't occur
        P(pawnLever_2, -29),
        P(pawnLever_3, -35),
        P(pawnLever_4, -31),
        P(pawnLever_5, -21),

        P(rammedPawn_0, -72),
        P(rammedPawn_1, -2),
        P(rammedPawn_2, 15),
        P(rammedPawn_3, -56),
        P(rammedPawn_4, 15),
        P(rammedPawn_5, 0), // Doesn't occur

        P(mobilePawn_0, 12),
        P(mobilePawn_1, 29),
        P(mobilePawn_2, 29),
        P(mobilePawn_3, 48),
        P(mobilePawn_4, 76),
        P(mobilePawn_5, 211),

        // Quadratic polynomials for passers
        P(passerA_0, 14),
        P(passerB_0, 56),
        P(passerC_0, 139),
        P(passerD_0, 200),
        P(passerE_0, 258),
        P(passerF_0, 33),
        P(passerG_0, -63),
        P(passerH_0, -104),

        P(passerA_1, 69),
        P(passerB_1, 16),
        P(passerC_1, -40),
        P(passerD_1, -54),
        P(passerE_1, -77),
        P(passerF_1, 72),
        P(passerG_1, 118),
        P(passerH_1, 165),

        P(passerA_2, 156),
        P(passerB_2, 199),
        P(passerC_2, 245),
        P(passerD_2, 240),
        P(passerE_2, 268),
        P(passerF_2, 149),
        P(passerG_2, 106),
        P(passerH_2, 60),

        P(passerScalingOffset, 3603),
        P(passerAndQueen, 1414),
        P(passerAndRook, 742),
        P(passerAndBishop, 743),
        P(passerAndKnight, 396),
        P(passerAndPawn, -552),
        P(passerVsQueen, -791),
        P(passerVsRook, -703),
        P(passerVsBishop, -1249),
        P(passerVsKnight, -860),
        P(passerVsPawn, 256),

        P(protectedPasser, -9),
        P(connectedPasser, -41),

        P(safePasser, 180), // no blocker and totally safe passage

        P(blocker_0, -115), // stop square
        P(blocker_1, -90),
        P(blocker_2, -31),
        P(blocker_3, -1),
        P(blocker_4, -10), // -furtest square

        P(blockedByOwn, 4),
        P(blockedByOther, -113),

        P(controller_0, -72), // stop square
        P(controller_1, -53),
        P(controller_2, 1),
        P(controller_3, 3),
        P(controller_4, -8), // -furtest square

        P(unstoppablePasser, 409),

        P(kingToPasser, 52),
        P(kingToOwnPasser, -62),

        P(candidateByRank_0, 32),
        P(candidateByRank_1, 62),
        P(candidateByRank_2, 94),
        P(candidateByRank_3, 152),
        P(candidateByRank_4, 171),

        P(candidateA, -18),
        P(candidateB, -89),
        P(candidateC, -68),
        P(candidateD, -85),
        P(candidateE, -24),
        P(candidateF, -17),
        P(candidateG, -38),
        P(candidateH, 21),

        /*
         *  Knights
         */

        // Kings on same side
        P(knightByFile_0, -148), // fileA
        P(knightByFile_1, -169),
        P(knightByFile_2, -184),
        P(knightByFile_3, -124),
        P(knightByFile_4, -72),
        P(knightByFile_5, -11),
        P(knightByFile_6, 29), // -fileH

        // Opposite side kings
        P(knightByFile_0x, -126), // fileA
        P(knightByFile_1x, -157),
        P(knightByFile_2x, -131),
        P(knightByFile_3x, -41),
        P(knightByFile_4x, 36),
        P(knightByFile_5x, 118),
        P(knightByFile_6x, 103), // -fileH

        P(knightByRank_0, -111), // rank1
        P(knightByRank_1, -197),
        P(knightByRank_2, -228),
        P(knightByRank_3, -190),
        P(knightByRank_4, -101),
        P(knightByRank_5, 35),
        P(knightByRank_6, 99), // -rank8

        P(knightAndSpan_0, -173), // span0 (pawnless)
        P(knightAndSpan_1, -126),
        P(knightAndSpan_2, -51),
        P(knightAndSpan_3, 19), // -span4 (7-8 files)

        P(knightVsSpan_0, -328), // span0 (pawnless)
        P(knightVsSpan_1, -206),
        P(knightVsSpan_2, -77),
        P(knightVsSpan_3, 6), // -span4 (7-8 files)

        P(knightToKing, -21),
        P(knightToOwnKing, -14),

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
        P(bishopBySquare_0, -125),
        P(bishopBySquare_1, 45),
        P(bishopBySquare_2, -28),
        P(bishopBySquare_3, 143),
        P(bishopBySquare_4, -14),
        P(bishopBySquare_5, 25),
        P(bishopBySquare_6, -33),
        P(bishopBySquare_7, 94),

        P(bishopBySquare_8, 7),
        P(bishopBySquare_9, 2),
        P(bishopBySquare_10, 133),
        P(bishopBySquare_11, 32),
        P(bishopBySquare_12, 71),
        P(bishopBySquare_13, 137),
        P(bishopBySquare_14, 88),
        P(bishopBySquare_15, 171),

        P(bishopBySquare_16, -86),
        P(bishopBySquare_17, 84),
        P(bishopBySquare_18, 65),
        P(bishopBySquare_19, 85),
        P(bishopBySquare_20, 78),
        P(bishopBySquare_21, 124),
        P(bishopBySquare_22, 104),
        P(bishopBySquare_23, 158),

        P(bishopBySquare_24, -51),
        P(bishopBySquare_25, -33),
        P(bishopBySquare_26, 78),
        P(bishopBySquare_27, 125),
        P(bishopBySquare_28, 188),
        P(bishopBySquare_29, 140),
        P(bishopBySquare_30, 141),
        P(bishopBySquare_31, 132),

        P(bishopBySquare_32, -57),
        P(bishopBySquare_33, 25),
        P(bishopBySquare_34, 32),
        P(bishopBySquare_35, 110),
        P(bishopBySquare_36, 142),
        P(bishopBySquare_37, 177),
        P(bishopBySquare_38, 64),
        P(bishopBySquare_39, 132),

        P(bishopBySquare_40, -76),
        P(bishopBySquare_41, 9),
        P(bishopBySquare_42, 73),
        P(bishopBySquare_43, 25),
        P(bishopBySquare_44, 150),
        P(bishopBySquare_45, 281),
        P(bishopBySquare_46, 91),
        P(bishopBySquare_47, 65),

        P(bishopBySquare_48, -51),
        P(bishopBySquare_49, 46),
        P(bishopBySquare_50, 58),
        P(bishopBySquare_51, 78),
        P(bishopBySquare_52, 34),
        P(bishopBySquare_53, 198),
        P(bishopBySquare_54, 13),
        P(bishopBySquare_55, 77),

        P(bishopBySquare_56, -216),
        P(bishopBySquare_57, -56),
        P(bishopBySquare_58, 3),
        P(bishopBySquare_59, 60),
        P(bishopBySquare_60, 32),
        P(bishopBySquare_61, 163),
        P(bishopBySquare_62, -210),
        P(bishopBySquare_63, -24),

        P(bishopBySquare_0x, -49),
        P(bishopBySquare_1x, 13),
        P(bishopBySquare_2x, 77),
        P(bishopBySquare_3x, 35),
        P(bishopBySquare_4x, 62),
        P(bishopBySquare_5x, 140),
        P(bishopBySquare_6x, -180),
        P(bishopBySquare_7x, -47),

        P(bishopBySquare_8x, -40),
        P(bishopBySquare_9x, -15),
        P(bishopBySquare_10x, 53),
        P(bishopBySquare_11x, 138),
        P(bishopBySquare_12x, 87),
        P(bishopBySquare_13x, 216),
        P(bishopBySquare_14x, 111),
        P(bishopBySquare_15x, 92),

        P(bishopBySquare_16x, -62),
        P(bishopBySquare_17x, 41),
        P(bishopBySquare_18x, 117),
        P(bishopBySquare_19x, 102),
        P(bishopBySquare_20x, 205),
        P(bishopBySquare_21x, 232),
        P(bishopBySquare_22x, 173),
        P(bishopBySquare_23x, 109),

        P(bishopBySquare_24x, -36),
        P(bishopBySquare_25x, 34),
        P(bishopBySquare_26x, 104),
        P(bishopBySquare_27x, 151),
        P(bishopBySquare_28x, 193),
        P(bishopBySquare_29x, 245),
        P(bishopBySquare_30x, 159),
        P(bishopBySquare_31x, 206),

        P(bishopBySquare_32x, -5),
        P(bishopBySquare_33x, 99),
        P(bishopBySquare_34x, 141),
        P(bishopBySquare_35x, 189),
        P(bishopBySquare_36x, 236),
        P(bishopBySquare_37x, 224),
        P(bishopBySquare_38x, 240),
        P(bishopBySquare_39x, 192),

        P(bishopBySquare_40x, -54),
        P(bishopBySquare_41x, 109),
        P(bishopBySquare_42x, 164),
        P(bishopBySquare_43x, 205),
        P(bishopBySquare_44x, 196),
        P(bishopBySquare_45x, 225),
        P(bishopBySquare_46x, 191),
        P(bishopBySquare_47x, 227),

        P(bishopBySquare_48x, 27),
        P(bishopBySquare_49x, 162),
        P(bishopBySquare_50x, 178),
        P(bishopBySquare_51x, 111),
        P(bishopBySquare_52x, 184),
        P(bishopBySquare_53x, 207),
        P(bishopBySquare_54x, 204),
        P(bishopBySquare_55x, 224),

        P(bishopBySquare_56x, -103),
        P(bishopBySquare_57x, 63),
        P(bishopBySquare_58x, 64),
        P(bishopBySquare_59x, 127),
        P(bishopBySquare_60x, 105),
        P(bishopBySquare_61x, 100),
        P(bishopBySquare_62x, 19),
        P(bishopBySquare_63x, 48),
#endif

        P(bishopAndSpan_0, -204), // span0 (pawnless)
        P(bishopAndSpan_1, -160),
        P(bishopAndSpan_2, -109),
        P(bishopAndSpan_3, -60), // -span4 (7-8 files)

        P(bishopVsSpan_0, -379), // span0 (pawnless)
        P(bishopVsSpan_1, -263),
        P(bishopVsSpan_2, -156),
        P(bishopVsSpan_3, -73), // -span4 (7-8 files)

        P(bishopAndLikePawn_1, -48),
        P(bishopAndLikePawn_2, -3),
        P(bishopAndLikeRammedPawn, -57),

        P(bishopVsLikePawn_1, -26),
        P(bishopVsLikePawn_2, -6),
        P(bishopVsLikeRammedPawn, 5),

        P(bishopToKing, -1),
        P(bishopToOwnKing, -10),

        /*
         *  Rooks
         */

        P(rookByFile_0, -69), // fileA
        P(rookByFile_1, -62),
        P(rookByFile_2, -28), // -fileD

        P(rookByRank_0, -83), // rank1
        P(rookByRank_1, -236),
        P(rookByRank_2, -372),
        P(rookByRank_3, -452),
        P(rookByRank_4, -410),
        P(rookByRank_5, -294),
        P(rookByRank_6, -151), // -rank8

        P(rookOnHalfOpen_0, 138), // fileA
        P(rookOnHalfOpen_1, 47),
        P(rookOnHalfOpen_2, 6),
        P(rookOnHalfOpen_3, 1), // fileD

        P(rookOnOpen_0, 228), // fileA
        P(rookOnOpen_1, 169),
        P(rookOnOpen_2, 111),
        P(rookOnOpen_3, 122), // fileD

        P(rookToWeakPawn_0, 155),
        P(rookToWeakPawn_1, 141),
        P(rookToWeakPawn_2, 118),
        P(rookToWeakPawn_3, 88),

        P(rookToKing_0, 294), // d=0
        P(rookToKing_1, 191), // d=1
        P(rookToKing_2, 135), // d=2
        P(rookToKing_3, 85),
        P(rookToKing_4, 30),
        P(rookToKing_5, -54),
        P(rookToKing_6, -105),
        P(rookToKing_7, -125),

        P(rookToKing, -17),
        P(rookToOwnKing, 3),

        P(rookInFrontPasser_0, -56), // opponent's
        P(rookInFrontPasser_1, 158), // own passer
        P(rookBehindPasser_0, 376), // opponent's
        P(rookBehindPasser_1, 56), // own passer

        /*
         *  Queens
         */

        P(queenByFile_0, -32), // fileA
        P(queenByFile_1, -40),
        P(queenByFile_2, -28), // -fileD

        P(queenByRank_0, -83), // rank1
        P(queenByRank_1, -142),
        P(queenByRank_2, -207),
        P(queenByRank_3, -246),
        P(queenByRank_4, -225),
        P(queenByRank_5, -170),
        P(queenByRank_6, -122), // -rank8

        P(queenToKing, -27),
        P(queenToOwnKing, -8),

        P(queenInFrontPasser_0, -82), // opponent's
        P(queenInFrontPasser_1, 49), // own passer
        P(queenBehindPasser_0, 162), // opponent's
        P(queenBehindPasser_1, -12), // own passer

        /*
         *  Kings
         */

        P(kingByFile_0, -35), // fileA
        P(kingByFile_1, -30),
        P(kingByFile_2, -22), // -fileD

        P(kingByRank_0, -262), // rank1
        P(kingByRank_1, -433),
        P(kingByRank_2, -595),
        P(kingByRank_3, -678),
        P(kingByRank_4, -603),
        P(kingByRank_5, -384),
        P(kingByRank_6, -141), // -rank8

        // Drawness. Positive is more drawish. Negative is more sharp
        P(drawOffset, 319),
        P(drawQueen, 109),
        P(drawRook, -23),
        P(drawBishop, -28),
        P(drawKnight, -18),
        P(drawPawn, -843),
        P(drawQueenEnding, -103),
        P(drawRookEnding, 629),
        P(drawKnightEnding, 579),
        P(drawBishopEnding, 1749),
        P(drawPawnEnding, -650),
        P(drawPawnless, 47),

        P(drawQueenImbalance, -824),
        P(drawRookImbalance, 427),
        P(drawMinorImbalance, 9),

        P(drawUnlikeBishops, 2356),
        P(drawUnlikeBishopsAndQueens, 883),
        P(drawUnlikeBishopsAndRooks, 418),
        P(drawUnlikeBishopsAndKnights, 675),

        P(drawRammed_0, 572), // fileA/H
        P(drawRammed_1, 530),
        P(drawRammed_2, 590),
        P(drawRammed_3, 474), // fileD/E
// }

/*----------------------------------------------------------------------+
 |                                                                      |
 +----------------------------------------------------------------------*/

