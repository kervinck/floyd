
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
        P(hanging_0, -23), // Side to move
        P(hanging_1, -113),
        P(hanging_2, -86),
        P(hanging_0x, 14), // Other side
        P(hanging_1x, 1),
        P(hanging_2x, 110),
        P(winBonus, 1499),

        P(castleK, -123),
        P(castleQ, -111),
        P(castleKQ, -63),

        P(queenValue, 6986),
        P(rookValue, 4707),
        P(bishopValue, 2639),
        P(knightValue, 2495),
        P(pawnValue1, 988),
        P(pawnValue2, 1659),
        P(pawnValue3, 2341),
        P(pawnValue4, 3031),
        P(pawnValue5, 3703),
        P(pawnValue6, 4355),
        P(pawnValue7, 4972),
        P(pawnValue8, 5497),

        P(queenAndQueen, -704),
        P(queenAndRook, -726),
        P(queenAndBishop, 91),
        P(queenAndKnight, 34),
        P(queenAndPawn_1, -21),
        P(queenAndPawn_2, -166),

        P(rookAndRook, -296),
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

        P(queenVsRook, -368),
        P(queenVsBishop, 75),
        P(queenVsKnight, 21),
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
        P(shelterPawn_1, 265), // rank4
        P(shelterPawn_2, 269), // rank5
        P(shelterPawn_3, 99), // rank6
        P(shelterPawn_4, -247), // rank7
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
        P(attackSquares_5, -261), // -(6 or more attacks)

        P(attackByPawn_0, 121),
        P(attackByPawn_1, 281),
        P(attackByPawn_2, 67),
        P(attackByMinor_0, -111),
        P(attackByMinor_1, -3),
        P(attackByMinor_2, -252),
        P(attackByRook_0, 490),
        P(attackByRook_1, 1040),
        P(attackByRook_2, -449),
        P(attackByQueen, 181),
        P(attackByKing, -268),

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
        P(pawnByFile_1, 39),
        P(pawnByFile_2, 7),
        P(pawnByFile_3, -43),
        P(pawnByFile_4, -79),
        P(pawnByFile_5, -36),
        P(pawnByFile_6, 36), // -fileH

        P(pawnByFile_0x, -104), // fileA
        P(pawnByFile_1x, -191),
        P(pawnByFile_2x, -231),
        P(pawnByFile_3x, -299),
        P(pawnByFile_4x, -321),
        P(pawnByFile_5x, -268),
        P(pawnByFile_6x, -100), // -fileH

        // 5 parameters for 6 pawn ranks
        P(pawnByRank_0, -50), // rank2
        P(pawnByRank_1, -95),
        P(pawnByRank_2, -89),
        P(pawnByRank_3, -13),
        P(pawnByRank_4, 62), // -rank7

        P(doubledPawnA, -165),
        P(doubledPawnB, -93),
        P(doubledPawnC, -115),
        P(doubledPawnD, -81),
        P(doubledPawnE, -67),
        P(doubledPawnF, -17),
        P(doubledPawnG, 8),
        P(doubledPawnH, -159),

        P(backwardPawnClosedByRank_0, 18),
        P(backwardPawnClosedByRank_1, 12),
        P(backwardPawnClosedByRank_2, -13),
        P(backwardPawnClosedByRank_3, -3),
        P(backwardPawnClosedByRank_4, 0), // Doesn't occur

        P(backwardPawnOpenByRank_0, 28),
        P(backwardPawnOpenByRank_1, 18),
        P(backwardPawnOpenByRank_2, 5),
        P(backwardPawnOpenByRank_3, 43),
        P(backwardPawnOpenByRank_4, 0), // Doesn't occur

        P(backwardPawnClosedA, 54),
        P(backwardPawnClosedB, -8),
        P(backwardPawnClosedC, 21),
        P(backwardPawnClosedD, -33),
        P(backwardPawnClosedE, -50),
        P(backwardPawnClosedF, -18),
        P(backwardPawnClosedG, 0),
        P(backwardPawnClosedH, 54),

        P(backwardPawnOpenA, -7),
        P(backwardPawnOpenB, -45),
        P(backwardPawnOpenC, -25),
        P(backwardPawnOpenD, -51),
        P(backwardPawnOpenE, -36),
        P(backwardPawnOpenF, -18),
        P(backwardPawnOpenG, -42),
        P(backwardPawnOpenH, 22),

        P(rammedWeakPawnA, -12),
        P(rammedWeakPawnB, 11),
        P(rammedWeakPawnC, 3),
        P(rammedWeakPawnD, 1),
        P(rammedWeakPawnE, 29),
        P(rammedWeakPawnF, 9),
        P(rammedWeakPawnG, -14),
        P(rammedWeakPawnH, -47),

        P(isolatedPawnClosedA, 118),
        P(isolatedPawnClosedB, 38),
        P(isolatedPawnClosedC, -35),
        P(isolatedPawnClosedD, -15),
        P(isolatedPawnClosedE, -84),
        P(isolatedPawnClosedF, -73),
        P(isolatedPawnClosedG, -69),
        P(isolatedPawnClosedH, 18),

        P(isolatedPawnOpenA, 106),
        P(isolatedPawnOpenB, 51),
        P(isolatedPawnOpenC, -30),
        P(isolatedPawnOpenD, -53),
        P(isolatedPawnOpenE, -61),
        P(isolatedPawnOpenF, -17),
        P(isolatedPawnOpenG, 48),
        P(isolatedPawnOpenH, 87),

        P(sidePawnClosedA, 92),
        P(sidePawnClosedB, 40),
        P(sidePawnClosedC, 45),
        P(sidePawnClosedD, -52),
        P(sidePawnClosedE, -40),
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
        P(middlePawnClosedD, -37),
        P(middlePawnClosedE, -12),
        P(middlePawnClosedF, -10),
        P(middlePawnClosedG, -77),
        P(middlePawnClosedH, 0), // Doesn't occur

        P(middlePawnOpenA, 0), // Doesn't occur
        P(middlePawnOpenB, -71),
        P(middlePawnOpenC, 27),
        P(middlePawnOpenD, 6),
        P(middlePawnOpenE, 39),
        P(middlePawnOpenF, -8),
        P(middlePawnOpenG, -30),
        P(middlePawnOpenH, 0), // Doesn't occur

        P(duoPawnA, 10),
        P(duoPawnB, -8),
        P(duoPawnC, -1),
        P(duoPawnD, 22),
        P(duoPawnE, 27),
        P(duoPawnF, 14),
        P(duoPawnG, 36),
        P(duoPawnH, -63),

        P(openFilePawn_0, -17),
        P(openFilePawn_1, -21),
        P(openFilePawn_2, 43),
        P(openFilePawn_3, 28),
        P(openFilePawn_4, 145),
        P(openFilePawn_5, 245),

        P(stoppedPawn_0, -36),
        P(stoppedPawn_1, 29),
        P(stoppedPawn_2, 21),
        P(stoppedPawn_3, -15),
        P(stoppedPawn_4, 0), // Doesn't occur
        P(stoppedPawn_5, 0), // Doesn't occur

        P(duoPawn_0, 1),
        P(duoPawn_1, -2),
        P(duoPawn_2, 29),
        P(duoPawn_3, 145),
        P(duoPawn_4, 495),
        P(duoPawn_5, 1493),

        P(trailingPawn_0, -12),
        P(trailingPawn_1, -46),
        P(trailingPawn_2, -69),
        P(trailingPawn_3, -44),
        P(trailingPawn_4, -18),
        P(trailingPawn_5, -24),

        P(capturePawn_0, 90),
        P(capturePawn_1, -68),
        P(capturePawn_2, -94),
        P(capturePawn_3, 28),
        P(capturePawn_4, 277),
        P(capturePawn_5, 0),

        P(defendedPawn_0, 0), // Doesn't occur
        P(defendedPawn_1, 61),
        P(defendedPawn_2, 8),
        P(defendedPawn_3, 117),
        P(defendedPawn_4, 246),
        P(defendedPawn_5, 254),

        P(pawnLever_0, 0), // Doesn't occur
        P(pawnLever_1, 0), // Doesn't occur
        P(pawnLever_2, -29),
        P(pawnLever_3, -35),
        P(pawnLever_4, -32),
        P(pawnLever_5, -21),

        P(rammedPawn_0, -72),
        P(rammedPawn_1, -2),
        P(rammedPawn_2, 12),
        P(rammedPawn_3, -56),
        P(rammedPawn_4, 15),
        P(rammedPawn_5, 0), // Doesn't occur

        P(mobilePawn_0, 12),
        P(mobilePawn_1, 29),
        P(mobilePawn_2, 29),
        P(mobilePawn_3, 48),
        P(mobilePawn_4, 76),
        P(mobilePawn_5, 206),

        // Quadratic polynomials for passers
        P(passerA_0, 14),
        P(passerB_0, 48),
        P(passerC_0, 139),
        P(passerD_0, 199),
        P(passerE_0, 258),
        P(passerF_0, 35),
        P(passerG_0, -64),
        P(passerH_0, -102),

        P(passerA_1, 69),
        P(passerB_1, 17),
        P(passerC_1, -40),
        P(passerD_1, -54),
        P(passerE_1, -77),
        P(passerF_1, 72),
        P(passerG_1, 118),
        P(passerH_1, 165),

        P(passerA_2, 156),
        P(passerB_2, 201),
        P(passerC_2, 245),
        P(passerD_2, 240),
        P(passerE_2, 268),
        P(passerF_2, 150),
        P(passerG_2, 106),
        P(passerH_2, 58),

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

        P(protectedPasser, -9), // TODO: check this after tuning
        P(connectedPasser, -42), // TODO: check this after tuning

        P(safePasser, 180), // no blocker and totally safe passage

        P(blocker_0, -115), // stop square
        P(blocker_1, -90),
        P(blocker_2, -31),
        P(blocker_3, 3),
        P(blocker_4, -6), // -furtest square

        P(blockedByOwn, 5), // TODO: check this after tuning
        P(blockedByOther, -113), // TODO: check this after tuning

        P(controller_0, -72), // stop square
        P(controller_1, -53),
        P(controller_2, 2),
        P(controller_3, 4),
        P(controller_4, -8), // -furtest square

        P(unstoppablePasser, 412),

        P(kingToPasser, 52),
        P(kingToOwnPasser, -62),

        P(candidateByRank_0, 32),
        P(candidateByRank_1, 62),
        P(candidateByRank_2, 97),
        P(candidateByRank_3, 152),
        P(candidateByRank_4, 171),

        P(candidateA, -6),
        P(candidateB, -89),
        P(candidateC, -66),
        P(candidateD, -85),
        P(candidateE, -25),
        P(candidateF, -17),
        P(candidateG, -41),
        P(candidateH, 21),

        /*
         *  Knights
         */

        // Kings on same side
        P(knightByFile_0, -149), // fileA
        P(knightByFile_1, -169),
        P(knightByFile_2, -184),
        P(knightByFile_3, -124),
        P(knightByFile_4, -73),
        P(knightByFile_5, -11),
        P(knightByFile_6, 29), // -fileH

        // Opposite side kings
        P(knightByFile_0x, -125), // fileA
        P(knightByFile_1x, -156),
        P(knightByFile_2x, -131),
        P(knightByFile_3x, -42),
        P(knightByFile_4x, 36),
        P(knightByFile_5x, 117),
        P(knightByFile_6x, 101), // -fileH

        P(knightByRank_0, -112), // rank1
        P(knightByRank_1, -197),
        P(knightByRank_2, -228),
        P(knightByRank_3, -190),
        P(knightByRank_4, -101),
        P(knightByRank_5, 35),
        P(knightByRank_6, 98), // -rank8

        P(knightAndSpan_0, -172), // span0 (pawnless)
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
        P(bishopBySquare_0, -123),
        P(bishopBySquare_1, 48),
        P(bishopBySquare_2, -28),
        P(bishopBySquare_3, 143),
        P(bishopBySquare_4, -13),
        P(bishopBySquare_5, 28),
        P(bishopBySquare_6, -36),
        P(bishopBySquare_7, 94),

        P(bishopBySquare_8, 7),
        P(bishopBySquare_9, 2),
        P(bishopBySquare_10, 134),
        P(bishopBySquare_11, 32),
        P(bishopBySquare_12, 71),
        P(bishopBySquare_13, 136),
        P(bishopBySquare_14, 87),
        P(bishopBySquare_15, 170),

        P(bishopBySquare_16, -85),
        P(bishopBySquare_17, 83),
        P(bishopBySquare_18, 65),
        P(bishopBySquare_19, 84),
        P(bishopBySquare_20, 80),
        P(bishopBySquare_21, 121),
        P(bishopBySquare_22, 103),
        P(bishopBySquare_23, 163),

        P(bishopBySquare_24, -52),
        P(bishopBySquare_25, -33),
        P(bishopBySquare_26, 79),
        P(bishopBySquare_27, 126),
        P(bishopBySquare_28, 187),
        P(bishopBySquare_29, 140),
        P(bishopBySquare_30, 140),
        P(bishopBySquare_31, 141),

        P(bishopBySquare_32, -57),
        P(bishopBySquare_33, 23),
        P(bishopBySquare_34, 32),
        P(bishopBySquare_35, 109),
        P(bishopBySquare_36, 141),
        P(bishopBySquare_37, 177),
        P(bishopBySquare_38, 65),
        P(bishopBySquare_39, 130),

        P(bishopBySquare_40, -79),
        P(bishopBySquare_41, 10),
        P(bishopBySquare_42, 72),
        P(bishopBySquare_43, 24),
        P(bishopBySquare_44, 151),
        P(bishopBySquare_45, 279),
        P(bishopBySquare_46, 89),
        P(bishopBySquare_47, 64),

        P(bishopBySquare_48, -51),
        P(bishopBySquare_49, 44),
        P(bishopBySquare_50, 56),
        P(bishopBySquare_51, 76),
        P(bishopBySquare_52, 33),
        P(bishopBySquare_53, 198),
        P(bishopBySquare_54, 10),
        P(bishopBySquare_55, 77),

        P(bishopBySquare_56, -218),
        P(bishopBySquare_57, -57),
        P(bishopBySquare_58, 2),
        P(bishopBySquare_59, 59),
        P(bishopBySquare_60, 30),
        P(bishopBySquare_61, 163),
        P(bishopBySquare_62, -209),
        P(bishopBySquare_63, -24),

        P(bishopBySquare_0x, -50),
        P(bishopBySquare_1x, 8),
        P(bishopBySquare_2x, 75),
        P(bishopBySquare_3x, 39),
        P(bishopBySquare_4x, 55),
        P(bishopBySquare_5x, 137),
        P(bishopBySquare_6x, -182),
        P(bishopBySquare_7x, -47),

        P(bishopBySquare_8x, -38),
        P(bishopBySquare_9x, -19),
        P(bishopBySquare_10x, 52),
        P(bishopBySquare_11x, 135),
        P(bishopBySquare_12x, 87),
        P(bishopBySquare_13x, 208),
        P(bishopBySquare_14x, 106),
        P(bishopBySquare_15x, 88),

        P(bishopBySquare_16x, -63),
        P(bishopBySquare_17x, 37),
        P(bishopBySquare_18x, 117),
        P(bishopBySquare_19x, 101),
        P(bishopBySquare_20x, 202),
        P(bishopBySquare_21x, 227),
        P(bishopBySquare_22x, 173),
        P(bishopBySquare_23x, 104),

        P(bishopBySquare_24x, -36),
        P(bishopBySquare_25x, 31),
        P(bishopBySquare_26x, 99),
        P(bishopBySquare_27x, 145),
        P(bishopBySquare_28x, 193),
        P(bishopBySquare_29x, 238),
        P(bishopBySquare_30x, 158),
        P(bishopBySquare_31x, 207),

        P(bishopBySquare_32x, -6),
        P(bishopBySquare_33x, 96),
        P(bishopBySquare_34x, 134),
        P(bishopBySquare_35x, 188),
        P(bishopBySquare_36x, 237),
        P(bishopBySquare_37x, 223),
        P(bishopBySquare_38x, 239),
        P(bishopBySquare_39x, 176),

        P(bishopBySquare_40x, -57),
        P(bishopBySquare_41x, 108),
        P(bishopBySquare_42x, 164),
        P(bishopBySquare_43x, 203),
        P(bishopBySquare_44x, 193),
        P(bishopBySquare_45x, 223),
        P(bishopBySquare_46x, 191),
        P(bishopBySquare_47x, 207),

        P(bishopBySquare_48x, 14),
        P(bishopBySquare_49x, 162),
        P(bishopBySquare_50x, 178),
        P(bishopBySquare_51x, 108),
        P(bishopBySquare_52x, 180),
        P(bishopBySquare_53x, 204),
        P(bishopBySquare_54x, 202),
        P(bishopBySquare_55x, 220),

        P(bishopBySquare_56x, -103),
        P(bishopBySquare_57x, 62),
        P(bishopBySquare_58x, 62),
        P(bishopBySquare_59x, 126),
        P(bishopBySquare_60x, 99),
        P(bishopBySquare_61x, 95),
        P(bishopBySquare_62x, 9),
        P(bishopBySquare_63x, 45),
#endif

        P(bishopAndSpan_0, -202), // span0 (pawnless)
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
        P(bishopVsLikeRammedPawn, 4),

        P(bishopToKing, -1),
        P(bishopToOwnKing, -10),

        /*
         *  Rooks
         */

        P(rookByFile_0, -69), // fileA
        P(rookByFile_1, -62),
        P(rookByFile_2, -28), // -fileD

        P(rookByRank_0, -83), // rank1
        P(rookByRank_1, -239),
        P(rookByRank_2, -375),
        P(rookByRank_3, -453),
        P(rookByRank_4, -411),
        P(rookByRank_5, -295),
        P(rookByRank_6, -151), // -rank8

        P(rookOnHalfOpen_0, 138), // fileA
        P(rookOnHalfOpen_1, 48),
        P(rookOnHalfOpen_2, 6),
        P(rookOnHalfOpen_3, 2), // fileD

        P(rookOnOpen_0, 228), // fileA
        P(rookOnOpen_1, 169),
        P(rookOnOpen_2, 112),
        P(rookOnOpen_3, 122), // fileD

        P(rookToWeakPawn_0, 155),
        P(rookToWeakPawn_1, 139),
        P(rookToWeakPawn_2, 118),
        P(rookToWeakPawn_3, 88),

        P(rookToKing_0, 294), // d=0
        P(rookToKing_1, 191), // d=1
        P(rookToKing_2, 135), // d=2
        P(rookToKing_3, 85),
        P(rookToKing_4, 30),
        P(rookToKing_5, -53),
        P(rookToKing_6, -98),
        P(rookToKing_7, -125),

        P(rookToKing, -17),
        P(rookToOwnKing, 3),

        P(rookInFrontPasser_0, -56), // opponent's
        P(rookInFrontPasser_1, 156), // own passer
        P(rookBehindPasser_0, 376), // opponent's
        P(rookBehindPasser_1, 57), // own passer

        /*
         *  Queens
         */

        P(queenByFile_0, -32), // fileA
        P(queenByFile_1, -40),
        P(queenByFile_2, -28), // -fileD

        P(queenByRank_0, -83), // rank1
        P(queenByRank_1, -143),
        P(queenByRank_2, -209),
        P(queenByRank_3, -246),
        P(queenByRank_4, -225),
        P(queenByRank_5, -170),
        P(queenByRank_6, -122), // -rank8

        P(queenToKing, -27),
        P(queenToOwnKing, -8),

        P(queenInFrontPasser_0, -81), // opponent's
        P(queenInFrontPasser_1, 49), // own passer
        P(queenBehindPasser_0, 163), // opponent's
        P(queenBehindPasser_1, -12), // own passer

        /*
         *  Kings
         */

        P(kingByFile_0, -35), // fileA
        P(kingByFile_1, -30),
        P(kingByFile_2, -22), // -fileD

        P(kingByRank_0, -262), // rank1
        P(kingByRank_1, -433),
        P(kingByRank_2, -596),
        P(kingByRank_3, -678),
        P(kingByRank_4, -603),
        P(kingByRank_5, -383),
        P(kingByRank_6, -139), // -rank8

        // Drawness. Positive is more drawish. Negative is more sharp
        P(drawOffset, 319),
        P(drawQueen, 109),
        P(drawRook, -23),
        P(drawBishop, -28),
        P(drawKnight, -24),
        P(drawPawn, -843),
        P(drawQueenEnding, -105),
        P(drawRookEnding, 628),
        P(drawKnightEnding, 691),
        P(drawBishopEnding, 1747),
        P(drawPawnEnding, -649),
        P(drawPawnless, 65),

        P(drawQueenImbalance, -833),
        P(drawRookImbalance, 429),
        P(drawMinorImbalance, 9),

        P(drawUnlikeBishops, 2364),
        P(drawUnlikeBishopsAndQueens, 844),
        P(drawUnlikeBishopsAndRooks, 416),
        P(drawUnlikeBishopsAndKnights, 686),

        P(drawRammed_0, 572), // fileA/H
        P(drawRammed_1, 535),
        P(drawRammed_2, 586),
        P(drawRammed_3, 467), // fileD/E
// }

/*----------------------------------------------------------------------+
 |                                                                      |
 +----------------------------------------------------------------------*/

