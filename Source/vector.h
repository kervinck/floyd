
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
        P(hanging_1, -107),
        P(hanging_2, -85),
        P(hanging_0x, 19), // Other side
        P(hanging_1x, -4),
        P(hanging_2x, 113),
        P(winBonus, 1500),

        P(castleK, -123),
        P(castleQ, -111),
        P(castleKQ, -62),

        P(queenValue, 6986),
        P(rookValue, 4708),
        P(bishopValue, 2639),
        P(knightValue, 2494),
        P(pawnValue1, 993),
        P(pawnValue2, 1660),
        P(pawnValue3, 2337),
        P(pawnValue4, 3031),
        P(pawnValue5, 3704),
        P(pawnValue6, 4358),
        P(pawnValue7, 4967),
        P(pawnValue8, 5487),

        P(queenAndQueen, -683),
        P(queenAndRook, -726),
        P(queenAndBishop, 94),
        P(queenAndKnight, 33),
        P(queenAndPawn_1, -21),
        P(queenAndPawn_2, -166),

        P(rookAndRook, -296),
        P(rookAndBishop, -111),
        P(rookAndKnight, -112),
        P(rookAndPawn_1, -55),
        P(rookAndPawn_2, -57),

        P(bishopAndBishop, 113),
        P(bishopAndKnight, 7),
        P(bishopAndPawn_1, -48),
        P(bishopAndPawn_2, 4),

        P(knightAndKnight, -27),
        P(knightAndPawn_1, -20),
        P(knightAndPawn_2, -3),

        P(queenVsRook, -368),
        P(queenVsBishop, 75),
        P(queenVsKnight, 21),
        P(queenVsPawn_1, 177),
        P(queenVsPawn_2, -173),

        P(rookVsBishop, -17),
        P(rookVsKnight, 18),
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
        P(shelterPawn_2, 258), // rank5
        P(shelterPawn_3, 91), // rank6
        P(shelterPawn_4, -250), // rank7
        P(shelterPawn_5, 293), // no pawn

        P(shelterKing_0, -320), // fileA (fileH)
        P(shelterKing_1, -328),
        P(shelterKing_2, -202), // -fileD (-fileE)

        P(shelterWalkingKing, 43), // rank2 or up
        P(shelterCastled, 67), // fraction of 256

        // attacks
        P(attackSquares_0, 66), // 0 attacks
        P(attackSquares_1, -18),
        P(attackSquares_2, -146),
        P(attackSquares_3, -254),
        P(attackSquares_4, -305),
        P(attackSquares_5, -261), // -(6 or more attacks)

        P(attackByPawn_0, 122),
        P(attackByPawn_1, 275),
        P(attackByPawn_2, 38),
        P(attackByMinor_0, -111),
        P(attackByMinor_1, 0),
        P(attackByMinor_2, -262),
        P(attackByRook_0, 490),
        P(attackByRook_1, 1040),
        P(attackByRook_2, -411),
        P(attackByQueen, 182),
        P(attackByKing, -250),

        P(attackPieces_0, 0),
        P(attackPieces_1, 0),
        P(attackPieces_2, 0),
        P(attackPieces_3, 0),
        P(attackPieces_4, 0),

        // TODO: pawn storm

        // king mobility
        P(mobilityKing_0, 87),
        P(mobilityKing_1, 34),

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
        P(pawnByFile_1, 40),
        P(pawnByFile_2, 7),
        P(pawnByFile_3, -42),
        P(pawnByFile_4, -78),
        P(pawnByFile_5, -36),
        P(pawnByFile_6, 36), // -fileH

        P(pawnByFile_0x, -104), // fileA
        P(pawnByFile_1x, -194),
        P(pawnByFile_2x, -230),
        P(pawnByFile_3x, -307),
        P(pawnByFile_4x, -325),
        P(pawnByFile_5x, -268),
        P(pawnByFile_6x, -100), // -fileH

        // 5 parameters for 6 pawn ranks
        P(pawnByRank_0, -50), // rank2
        P(pawnByRank_1, -95),
        P(pawnByRank_2, -89),
        P(pawnByRank_3, -13),
        P(pawnByRank_4, 61), // -rank7

        P(doubledPawnA, -162),
        P(doubledPawnB, -93),
        P(doubledPawnC, -119),
        P(doubledPawnD, -79),
        P(doubledPawnE, -61),
        P(doubledPawnF, -14),
        P(doubledPawnG, 9),
        P(doubledPawnH, -151),

        P(backwardPawnClosedByRank_0, 20),
        P(backwardPawnClosedByRank_1, 12),
        P(backwardPawnClosedByRank_2, -13),
        P(backwardPawnClosedByRank_3, -3),
        P(backwardPawnClosedByRank_4, 0),

        P(backwardPawnOpenByRank_0, 29),
        P(backwardPawnOpenByRank_1, 18),
        P(backwardPawnOpenByRank_2, 6),
        P(backwardPawnOpenByRank_3, 43),
        P(backwardPawnOpenByRank_4, 0),

        P(backwardPawnClosedA, 51),
        P(backwardPawnClosedB, -5),
        P(backwardPawnClosedC, 29),
        P(backwardPawnClosedD, -30),
        P(backwardPawnClosedE, -62),
        P(backwardPawnClosedF, -20),
        P(backwardPawnClosedG, -3),
        P(backwardPawnClosedH, 49),

        P(backwardPawnOpenA, 0),
        P(backwardPawnOpenB, -45),
        P(backwardPawnOpenC, -24),
        P(backwardPawnOpenD, -52),
        P(backwardPawnOpenE, -32),
        P(backwardPawnOpenF, -17),
        P(backwardPawnOpenG, -45),
        P(backwardPawnOpenH, 16),

        P(rammedWeakPawnA, -12),
        P(rammedWeakPawnB, 10),
        P(rammedWeakPawnC, -3),
        P(rammedWeakPawnD, 1),
        P(rammedWeakPawnE, 27),
        P(rammedWeakPawnF, 17),
        P(rammedWeakPawnG, -14),
        P(rammedWeakPawnH, -44),

        P(isolatedPawnClosedA, 118),
        P(isolatedPawnClosedB, 30),
        P(isolatedPawnClosedC, -34),
        P(isolatedPawnClosedD, -15),
        P(isolatedPawnClosedE, -76),
        P(isolatedPawnClosedF, -86),
        P(isolatedPawnClosedG, -73),
        P(isolatedPawnClosedH, 16),

        P(isolatedPawnOpenA, 104),
        P(isolatedPawnOpenB, 59),
        P(isolatedPawnOpenC, -28),
        P(isolatedPawnOpenD, -51),
        P(isolatedPawnOpenE, -61),
        P(isolatedPawnOpenF, -20),
        P(isolatedPawnOpenG, 43),
        P(isolatedPawnOpenH, 87),

        P(sidePawnClosedA, 92),
        P(sidePawnClosedB, 40),
        P(sidePawnClosedC, 45),
        P(sidePawnClosedD, -56),
        P(sidePawnClosedE, -41),
        P(sidePawnClosedF, -47),
        P(sidePawnClosedG, -83),
        P(sidePawnClosedH, 0),

        P(sidePawnOpenA, 69),
        P(sidePawnOpenB, 40),
        P(sidePawnOpenC, 18),
        P(sidePawnOpenD, -21),
        P(sidePawnOpenE, -5),
        P(sidePawnOpenF, 16),
        P(sidePawnOpenG, 48),
        P(sidePawnOpenH, 52),

        P(middlePawnClosedA, 0),
        P(middlePawnClosedB, 11),
        P(middlePawnClosedC, 81),
        P(middlePawnClosedD, -37),
        P(middlePawnClosedE, -14),
        P(middlePawnClosedF, -8),
        P(middlePawnClosedG, -79),
        P(middlePawnClosedH, 0),

        P(middlePawnOpenA, 0),
        P(middlePawnOpenB, -73),
        P(middlePawnOpenC, 29),
        P(middlePawnOpenD, 5),
        P(middlePawnOpenE, 39),
        P(middlePawnOpenF, -8),
        P(middlePawnOpenG, -31),
        P(middlePawnOpenH, 0),

        P(duoPawnA, 10),
        P(duoPawnB, -10),
        P(duoPawnC, -2),
        P(duoPawnD, 23),
        P(duoPawnE, 25),
        P(duoPawnF, 13),
        P(duoPawnG, 35),
        P(duoPawnH, -63),

        P(openFilePawn_0, -16),
        P(openFilePawn_1, -21),
        P(openFilePawn_2, 43),
        P(openFilePawn_3, 29),
        P(openFilePawn_4, 151),
        P(openFilePawn_5, 246),

        P(stoppedPawn_0, -37),
        P(stoppedPawn_1, 30),
        P(stoppedPawn_2, 21),
        P(stoppedPawn_3, -15),
        P(stoppedPawn_4, 0),
        P(stoppedPawn_5, 0),

        P(duoPawn_0, 1),
        P(duoPawn_1, -2),
        P(duoPawn_2, 29),
        P(duoPawn_3, 147),
        P(duoPawn_4, 507),
        P(duoPawn_5, 1425),

        P(trailingPawn_0, -11),
        P(trailingPawn_1, -46),
        P(trailingPawn_2, -69),
        P(trailingPawn_3, -44),
        P(trailingPawn_4, -23),
        P(trailingPawn_5, -25),

        P(capturePawn_0, 73),
        P(capturePawn_1, -71),
        P(capturePawn_2, -71),
        P(capturePawn_3, 26),
        P(capturePawn_4, 287),
        P(capturePawn_5, 0),

        P(defendedPawn_0, 0),
        P(defendedPawn_1, 61),
        P(defendedPawn_2, 8),
        P(defendedPawn_3, 118),
        P(defendedPawn_4, 246),
        P(defendedPawn_5, 254),

        P(pawnLever_0, 0),
        P(pawnLever_1, 0),
        P(pawnLever_2, -28),
        P(pawnLever_3, -37),
        P(pawnLever_4, -32),
        P(pawnLever_5, -22),
        P(pawnLever_6, 0),

        P(rammedPawn_0, -68),
        P(rammedPawn_1, -3),
        P(rammedPawn_2, 15),
        P(rammedPawn_3, -57),
        P(rammedPawn_4, 15),
        P(rammedPawn_5, 0),

        P(mobilePawn_0, 12),
        P(mobilePawn_1, 28),
        P(mobilePawn_2, 28),
        P(mobilePawn_3, 48),
        P(mobilePawn_4, 77),
        P(mobilePawn_5, 203),

        // Quadratic polynomials for passers
        P(passerA_0, 11),
        P(passerB_0, 48),
        P(passerC_0, 140),
        P(passerD_0, 202),
        P(passerE_0, 256),
        P(passerF_0, 36),
        P(passerG_0, -64),
        P(passerH_0, -93),

        P(passerA_1, 69),
        P(passerB_1, 17),
        P(passerC_1, -43),
        P(passerD_1, -55),
        P(passerE_1, -78),
        P(passerF_1, 72),
        P(passerG_1, 118),
        P(passerH_1, 165),

        P(passerA_2, 158),
        P(passerB_2, 201),
        P(passerC_2, 245),
        P(passerD_2, 240),
        P(passerE_2, 270),
        P(passerF_2, 148),
        P(passerG_2, 106),
        P(passerH_2, 57),

        P(passerScalingOffset, 3634),
        P(passerAndQueen, 1371),
        P(passerAndRook, 742),
        P(passerAndBishop, 744),
        P(passerAndKnight, 355),
        P(passerAndPawn, -555),
        P(passerVsQueen, -803),
        P(passerVsRook, -703),
        P(passerVsBishop, -1231),
        P(passerVsKnight, -860),
        P(passerVsPawn, 255),

        P(protectedPasser, -5), // TODO: check this after tuning
        P(connectedPasser, -46), // TODO: check this after tuning

        P(safePasser, 181), // no blocker and totally safe passage

        P(blocker_0, -115), // stop square
        P(blocker_1, -89),
        P(blocker_2, -36),
        P(blocker_3, 3),
        P(blocker_4, 7), // -furtest square

        P(blockedByOwn, 0), // TODO: check this after tuning
        P(blockedByOther, -112), // TODO: check this after tuning

        P(controller_0, -69), // stop square
        P(controller_1, -48),
        P(controller_2, 2),
        P(controller_3, -1),
        P(controller_4, -16), // -furtest square

        P(unstoppablePasser, 423),

        P(kingToPasser, 52),
        P(kingToOwnPasser, -62),

        P(candidateByRank_0, 32),
        P(candidateByRank_1, 59),
        P(candidateByRank_2, 100),
        P(candidateByRank_3, 150),
        P(candidateByRank_4, 111),

        P(candidateA, -6),
        P(candidateB, -88),
        P(candidateC, -42),
        P(candidateD, -83),
        P(candidateE, -25),
        P(candidateF, -17),
        P(candidateG, -41),
        P(candidateH, 9),

        /*
         *  Knights
         */

        // Kings on same side
        P(knightByFile_0, -152), // fileA
        P(knightByFile_1, -170),
        P(knightByFile_2, -184),
        P(knightByFile_3, -124),
        P(knightByFile_4, -73),
        P(knightByFile_5, -12),
        P(knightByFile_6, 28), // -fileH

        // Opposite side kings
        P(knightByFile_0x, -124), // fileA
        P(knightByFile_1x, -156),
        P(knightByFile_2x, -126),
        P(knightByFile_3x, -43),
        P(knightByFile_4x, 36),
        P(knightByFile_5x, 117),
        P(knightByFile_6x, 100), // -fileH

        P(knightByRank_0, -112), // rank1
        P(knightByRank_1, -197),
        P(knightByRank_2, -227),
        P(knightByRank_3, -190),
        P(knightByRank_4, -98),
        P(knightByRank_5, 33),
        P(knightByRank_6, 114), // -rank8

        P(knightAndSpan_0, -169), // span0 (pawnless)
        P(knightAndSpan_1, -127),
        P(knightAndSpan_2, -51),
        P(knightAndSpan_3, 20), // -span4 (7-8 files)

        P(knightVsSpan_0, -328), // span0 (pawnless)
        P(knightVsSpan_1, -205),
        P(knightVsSpan_2, -78),
        P(knightVsSpan_3, 6), // -span4 (7-8 files)

        P(knightToKing, -20),
        P(knightToOwnKing, -13),

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
        P(bishopBySquare_0, -128),
        P(bishopBySquare_1, 38),
        P(bishopBySquare_2, -24),
        P(bishopBySquare_3, 138),
        P(bishopBySquare_4, -14),
        P(bishopBySquare_5, 29),
        P(bishopBySquare_6, -38),
        P(bishopBySquare_7, 94),

        P(bishopBySquare_8, -2),
        P(bishopBySquare_9, 1),
        P(bishopBySquare_10, 140),
        P(bishopBySquare_11, 26),
        P(bishopBySquare_12, 70),
        P(bishopBySquare_13, 141),
        P(bishopBySquare_14, 85),
        P(bishopBySquare_15, 160),

        P(bishopBySquare_16, -85),
        P(bishopBySquare_17, 84),
        P(bishopBySquare_18, 64),
        P(bishopBySquare_19, 78),
        P(bishopBySquare_20, 81),
        P(bishopBySquare_21, 121),
        P(bishopBySquare_22, 103),
        P(bishopBySquare_23, 139),

        P(bishopBySquare_24, -60),
        P(bishopBySquare_25, -39),
        P(bishopBySquare_26, 79),
        P(bishopBySquare_27, 122),
        P(bishopBySquare_28, 188),
        P(bishopBySquare_29, 157),
        P(bishopBySquare_30, 141),
        P(bishopBySquare_31, 141),

        P(bishopBySquare_32, -46),
        P(bishopBySquare_33, 22),
        P(bishopBySquare_34, 32),
        P(bishopBySquare_35, 112),
        P(bishopBySquare_36, 140),
        P(bishopBySquare_37, 182),
        P(bishopBySquare_38, 68),
        P(bishopBySquare_39, 129),

        P(bishopBySquare_40, -80),
        P(bishopBySquare_41, 10),
        P(bishopBySquare_42, 69),
        P(bishopBySquare_43, 24),
        P(bishopBySquare_44, 148),
        P(bishopBySquare_45, 283),
        P(bishopBySquare_46, 104),
        P(bishopBySquare_47, 59),

        P(bishopBySquare_48, -58),
        P(bishopBySquare_49, 44),
        P(bishopBySquare_50, 39),
        P(bishopBySquare_51, 70),
        P(bishopBySquare_52, 40),
        P(bishopBySquare_53, 204),
        P(bishopBySquare_54, 0),
        P(bishopBySquare_55, 125),

        P(bishopBySquare_56, -251),
        P(bishopBySquare_57, -56),
        P(bishopBySquare_58, 1),
        P(bishopBySquare_59, 75),
        P(bishopBySquare_60, 13),
        P(bishopBySquare_61, 163),
        P(bishopBySquare_62, -181),
        P(bishopBySquare_63, -24),

        P(bishopBySquare_0x, -25),
        P(bishopBySquare_1x, 10),
        P(bishopBySquare_2x, 64),
        P(bishopBySquare_3x, 21),
        P(bishopBySquare_4x, 60),
        P(bishopBySquare_5x, 126),
        P(bishopBySquare_6x, -202),
        P(bishopBySquare_7x, -131),

        P(bishopBySquare_8x, 1),
        P(bishopBySquare_9x, -19),
        P(bishopBySquare_10x, 47),
        P(bishopBySquare_11x, 125),
        P(bishopBySquare_12x, 84),
        P(bishopBySquare_13x, 191),
        P(bishopBySquare_14x, 91),
        P(bishopBySquare_15x, 25),

        P(bishopBySquare_16x, -70),
        P(bishopBySquare_17x, 32),
        P(bishopBySquare_18x, 114),
        P(bishopBySquare_19x, 99),
        P(bishopBySquare_20x, 189),
        P(bishopBySquare_21x, 226),
        P(bishopBySquare_22x, 175),
        P(bishopBySquare_23x, 104),

        P(bishopBySquare_24x, -40),
        P(bishopBySquare_25x, 25),
        P(bishopBySquare_26x, 99),
        P(bishopBySquare_27x, 145),
        P(bishopBySquare_28x, 193),
        P(bishopBySquare_29x, 235),
        P(bishopBySquare_30x, 111),
        P(bishopBySquare_31x, 185),

        P(bishopBySquare_32x, -49),
        P(bishopBySquare_33x, 99),
        P(bishopBySquare_34x, 135),
        P(bishopBySquare_35x, 186),
        P(bishopBySquare_36x, 254),
        P(bishopBySquare_37x, 226),
        P(bishopBySquare_38x, 209),
        P(bishopBySquare_39x, 158),

        P(bishopBySquare_40x, -60),
        P(bishopBySquare_41x, 118),
        P(bishopBySquare_42x, 164),
        P(bishopBySquare_43x, 208),
        P(bishopBySquare_44x, 191),
        P(bishopBySquare_45x, 175),
        P(bishopBySquare_46x, 200),
        P(bishopBySquare_47x, 227),

        P(bishopBySquare_48x, 24),
        P(bishopBySquare_49x, 161),
        P(bishopBySquare_50x, 173),
        P(bishopBySquare_51x, 115),
        P(bishopBySquare_52x, 181),
        P(bishopBySquare_53x, 212),
        P(bishopBySquare_54x, 171),
        P(bishopBySquare_55x, 197),

        P(bishopBySquare_56x, -75),
        P(bishopBySquare_57x, 103),
        P(bishopBySquare_58x, 58),
        P(bishopBySquare_59x, 133),
        P(bishopBySquare_60x, 124),
        P(bishopBySquare_61x, 90),
        P(bishopBySquare_62x, 9),
        P(bishopBySquare_63x, 40),
#endif

        P(bishopAndSpan_0, -202), // span0 (pawnless)
        P(bishopAndSpan_1, -160),
        P(bishopAndSpan_2, -109),
        P(bishopAndSpan_3, -60), // -span4 (7-8 files)

        P(bishopVsSpan_0, -376), // span0 (pawnless)
        P(bishopVsSpan_1, -264),
        P(bishopVsSpan_2, -156),
        P(bishopVsSpan_3, -73), // -span4 (7-8 files)

        P(bishopAndLikePawn_1, -48),
        P(bishopAndLikePawn_2, -3),
        P(bishopAndLikeRammedPawn, -57),

        P(bishopVsLikePawn_1, -26),
        P(bishopVsLikePawn_2, -6),
        P(bishopVsLikeRammedPawn, 3),

        P(bishopToKing, -1),
        P(bishopToOwnKing, -10),

        /*
         *  Rooks
         */

        P(rookByFile_0, -69), // fileA
        P(rookByFile_1, -62),
        P(rookByFile_2, -28), // -fileD

        P(rookByRank_0, -84), // rank1
        P(rookByRank_1, -239),
        P(rookByRank_2, -377),
        P(rookByRank_3, -455),
        P(rookByRank_4, -414),
        P(rookByRank_5, -295),
        P(rookByRank_6, -150), // -rank8

        P(rookOnHalfOpen_0, 138), // fileA
        P(rookOnHalfOpen_1, 51),
        P(rookOnHalfOpen_2, 6),
        P(rookOnHalfOpen_3, 2), // fileD

        P(rookOnOpen_0, 223), // fileA
        P(rookOnOpen_1, 169),
        P(rookOnOpen_2, 114),
        P(rookOnOpen_3, 122), // fileD

        P(rookToWeakPawn_0, 152),
        P(rookToWeakPawn_1, 139),
        P(rookToWeakPawn_2, 116),
        P(rookToWeakPawn_3, 97),

        P(rookToKing_0, 290), // d=0
        P(rookToKing_1, 191), // d=1
        P(rookToKing_2, 128), // d=2
        P(rookToKing_3, 79),
        P(rookToKing_4, 30),
        P(rookToKing_5, -53),
        P(rookToKing_6, -98),
        P(rookToKing_7, -106),

        P(rookToKing, -17),
        P(rookToOwnKing, 3),

        P(rookInFrontPasser_0, -51), // opponent's
        P(rookInFrontPasser_1, 162), // own passer
        P(rookBehindPasser_0, 377), // opponent's
        P(rookBehindPasser_1, 60), // own passer

        /*
         *  Queens
         */

        P(queenByFile_0, -29), // fileA
        P(queenByFile_1, -40),
        P(queenByFile_2, -28), // -fileD

        P(queenByRank_0, -87), // rank1
        P(queenByRank_1, -145),
        P(queenByRank_2, -209),
        P(queenByRank_3, -249),
        P(queenByRank_4, -225),
        P(queenByRank_5, -168),
        P(queenByRank_6, -128), // -rank8

        P(queenToKing, -27),
        P(queenToOwnKing, -7),

        P(queenInFrontPasser_0, -86), // opponent's
        P(queenInFrontPasser_1, 49), // own passer
        P(queenBehindPasser_0, 180), // opponent's
        P(queenBehindPasser_1, -13), // own passer

        /*
         *  Kings
         */

        P(kingByFile_0, -35), // fileA
        P(kingByFile_1, -30),
        P(kingByFile_2, -22), // -fileD

        P(kingByRank_0, -262), // rank1
        P(kingByRank_1, -433),
        P(kingByRank_2, -597),
        P(kingByRank_3, -678),
        P(kingByRank_4, -603),
        P(kingByRank_5, -383),
        P(kingByRank_6, -138), // -rank8

        // Drawness. Positive is more drawish. Negative is more sharp
        P(drawOffset, 319),
        P(drawQueen, 93),
        P(drawRook, -23),
        P(drawBishop, -27),
        P(drawKnight, -19),
        P(drawPawn, -844),
        P(drawQueenEnding, -120),
        P(drawRookEnding, 628),
        P(drawKnightEnding, 708),
        P(drawBishopEnding, 1746),
        P(drawPawnEnding, -524),
        P(drawPawnless, 62),

        P(drawQueenImbalance, -833),
        P(drawRookImbalance, 429),
        P(drawMinorImbalance, 19),

        P(drawUnlikeBishops, 2356),
        P(drawUnlikeBishopsAndQueens, 819),
        P(drawUnlikeBishopsAndRooks, 408),
        P(drawUnlikeBishopsAndKnights, 731),

        P(drawRammed_0, 572), // fileA/H
        P(drawRammed_1, 535),
        P(drawRammed_2, 590),
        P(drawRammed_3, 467), // fileD/E
// }

/*----------------------------------------------------------------------+
 |                                                                      |
 +----------------------------------------------------------------------*/

