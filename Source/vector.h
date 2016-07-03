
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
        P(tempo, 42),
        P(winBonus, 1498),

        P(castleK, -136),
        P(castleQ, -118),
        P(castleKQ, -67),

        P(queenValue, 6988),
        P(rookValue, 4699),
        P(bishopValue, 2637),
        P(knightValue, 2479),
        P(pawnValue1, 997),
        P(pawnValue2, 1666),
        P(pawnValue3, 2344),
        P(pawnValue4, 3030),
        P(pawnValue5, 3700),
        P(pawnValue6, 4349),
        P(pawnValue7, 4963),
        P(pawnValue8, 5477),

        P(queenAndQueen, -708),
        P(queenAndRook, -728),
        P(queenAndBishop, 99),
        P(queenAndKnight, 31),
        P(queenAndPawn_1, -22),
        P(queenAndPawn_2, -164),

        P(rookAndRook, -295),
        P(rookAndBishop, -111),
        P(rookAndKnight, -106),
        P(rookAndPawn_1, -55),
        P(rookAndPawn_2, -57),

        P(bishopAndBishop, 115),
        P(bishopAndKnight, 11),
        P(bishopAndPawn_1, -48),
        P(bishopAndPawn_2, 4),

        P(knightAndKnight, -26),
        P(knightAndPawn_1, -20),
        P(knightAndPawn_2, -2),

        P(queenVsRook, -368),
        P(queenVsBishop, 83),
        P(queenVsKnight, 19),
        P(queenVsPawn_1, 177),
        P(queenVsPawn_2, -173),

        P(rookVsBishop, -17),
        P(rookVsKnight, 19),
        P(rookVsPawn_1, -2),
        P(rookVsPawn_2, -49),

        P(bishopVsKnight, 6),
        P(bishopVsPawn_1, -10),
        P(bishopVsPawn_2, 3),

        P(knightVsPawn_1, -2),
        P(knightVsPawn_2, -17),

        /*
         *  Board control
         */

        P(controlCenter, 26),
        P(controlExtendedCenter, 34),
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
        P(safetyVsPawn, -838),

        // shelter
        P(shelterPawn_0, 134), // rank3
        P(shelterPawn_1, 260), // rank4
        P(shelterPawn_2, 268), // rank5
        P(shelterPawn_3, 103), // rank6
        P(shelterPawn_4, -229), // rank7
        P(shelterPawn_5, 292), // no pawn

        P(shelterKing_0, -320), // fileA (fileH)
        P(shelterKing_1, -326),
        P(shelterKing_2, -198), // -fileD (-fileE)

        P(shelterWalkingKing, 50), // rank2 or up
        P(shelterCastled, 64), // fraction of 256

        // attacks
        P(attackSquares_0, 69), // 0 attacks
        P(attackSquares_1, -18),
        P(attackSquares_2, -139),
        P(attackSquares_3, -248),
        P(attackSquares_4, -304),
        P(attackSquares_5, -259), // -(6 or more attacks)

        P(attackByPawn_0, 122),
        P(attackByPawn_1, 287),
        P(attackByPawn_2, 71),
        P(attackByMinor_0, -111),
        P(attackByMinor_1, 4),
        P(attackByMinor_2, -252),
        P(attackByRook_0, 481),
        P(attackByRook_1, 1021),
        P(attackByRook_2, -437),
        P(attackByQueen, 180),
        P(attackByKing, -273),

        P(attackPieces_0, 0),
        P(attackPieces_1, -1),
        P(attackPieces_2, 0),
        P(attackPieces_3, -1),
        P(attackPieces_4, 0),

        // TODO: pawn storm

        // king mobility
        P(mobilityKing_0, 96),
        P(mobilityKing_1, 46),

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
        P(pawnByFile_2, 8),
        P(pawnByFile_3, -41),
        P(pawnByFile_4, -78),
        P(pawnByFile_5, -34),
        P(pawnByFile_6, 32), // -fileH

        P(pawnByFile_0x, -110), // fileA
        P(pawnByFile_1x, -197),
        P(pawnByFile_2x, -242),
        P(pawnByFile_3x, -312),
        P(pawnByFile_4x, -329),
        P(pawnByFile_5x, -268),
        P(pawnByFile_6x, -105), // -fileH

        // 5 parameters for 6 pawn ranks
        P(pawnByRank_0, -51), // rank2
        P(pawnByRank_1, -93),
        P(pawnByRank_2, -89),
        P(pawnByRank_3, -14),
        P(pawnByRank_4, 56), // -rank7

        P(doubledPawnA, -167),
        P(doubledPawnB, -92),
        P(doubledPawnC, -115),
        P(doubledPawnD, -77),
        P(doubledPawnE, -65),
        P(doubledPawnF, -18),
        P(doubledPawnG, 11),
        P(doubledPawnH, -166),

        P(backwardPawnClosedByRank_0, 22),
        P(backwardPawnClosedByRank_1, 16),
        P(backwardPawnClosedByRank_2, -13),
        P(backwardPawnClosedByRank_3, -6),
        P(backwardPawnClosedByRank_4, 0),

        P(backwardPawnOpenByRank_0, 30),
        P(backwardPawnOpenByRank_1, 20),
        P(backwardPawnOpenByRank_2, 4),
        P(backwardPawnOpenByRank_3, 43),
        P(backwardPawnOpenByRank_4, 0),

        P(backwardPawnClosedA, 56),
        P(backwardPawnClosedB, -8),
        P(backwardPawnClosedC, 18),
        P(backwardPawnClosedD, -35),
        P(backwardPawnClosedE, -51),
        P(backwardPawnClosedF, -17),
        P(backwardPawnClosedG, -1),
        P(backwardPawnClosedH, 55),

        P(backwardPawnOpenA, -9),
        P(backwardPawnOpenB, -40),
        P(backwardPawnOpenC, -26),
        P(backwardPawnOpenD, -49),
        P(backwardPawnOpenE, -38),
        P(backwardPawnOpenF, -23),
        P(backwardPawnOpenG, -42),
        P(backwardPawnOpenH, 19),

        P(rammedWeakPawnA, -13),
        P(rammedWeakPawnB, 9),
        P(rammedWeakPawnC, 5),
        P(rammedWeakPawnD, 3),
        P(rammedWeakPawnE, 28),
        P(rammedWeakPawnF, 10),
        P(rammedWeakPawnG, -11),
        P(rammedWeakPawnH, -47),

        P(isolatedPawnClosedA, 118),
        P(isolatedPawnClosedB, 36),
        P(isolatedPawnClosedC, -25),
        P(isolatedPawnClosedD, -17),
        P(isolatedPawnClosedE, -90),
        P(isolatedPawnClosedF, -80),
        P(isolatedPawnClosedG, -75),
        P(isolatedPawnClosedH, 6),

        P(isolatedPawnOpenA, 110),
        P(isolatedPawnOpenB, 49),
        P(isolatedPawnOpenC, -25),
        P(isolatedPawnOpenD, -55),
        P(isolatedPawnOpenE, -61),
        P(isolatedPawnOpenF, -15),
        P(isolatedPawnOpenG, 50),
        P(isolatedPawnOpenH, 80),

        P(sidePawnClosedA, 94),
        P(sidePawnClosedB, 40),
        P(sidePawnClosedC, 53),
        P(sidePawnClosedD, -49),
        P(sidePawnClosedE, -50),
        P(sidePawnClosedF, -49),
        P(sidePawnClosedG, -93),
        P(sidePawnClosedH, -2),

        P(sidePawnOpenA, 74),
        P(sidePawnOpenB, 40),
        P(sidePawnOpenC, 24),
        P(sidePawnOpenD, -20),
        P(sidePawnOpenE, -5),
        P(sidePawnOpenF, 20),
        P(sidePawnOpenG, 51),
        P(sidePawnOpenH, 53),

        P(middlePawnClosedA, 0),
        P(middlePawnClosedB, 10),
        P(middlePawnClosedC, 81),
        P(middlePawnClosedD, -31),
        P(middlePawnClosedE, -20),
        P(middlePawnClosedF, -10),
        P(middlePawnClosedG, -88),
        P(middlePawnClosedH, 0),

        P(middlePawnOpenA, 0),
        P(middlePawnOpenB, -69),
        P(middlePawnOpenC, 24),
        P(middlePawnOpenD, 7),
        P(middlePawnOpenE, 31),
        P(middlePawnOpenF, -4),
        P(middlePawnOpenG, -33),
        P(middlePawnOpenH, 0),

        P(duoPawnA, 7),
        P(duoPawnB, -6),
        P(duoPawnC, -2),
        P(duoPawnD, 23),
        P(duoPawnE, 29),
        P(duoPawnF, 14),
        P(duoPawnG, 37),
        P(duoPawnH, -64),

        P(openFilePawn_0, -18),
        P(openFilePawn_1, -24),
        P(openFilePawn_2, 44),
        P(openFilePawn_3, 31),
        P(openFilePawn_4, 152),
        P(openFilePawn_5, 251),

        P(stoppedPawn_0, -37),
        P(stoppedPawn_1, 30),
        P(stoppedPawn_2, 25),
        P(stoppedPawn_3, -16),
        P(stoppedPawn_4, 0),
        P(stoppedPawn_5, 0),

        P(duoPawn_0, 3),
        P(duoPawn_1, -4),
        P(duoPawn_2, 28),
        P(duoPawn_3, 143),
        P(duoPawn_4, 489),
        P(duoPawn_5, 1487),

        P(trailingPawn_0, -11),
        P(trailingPawn_1, -47),
        P(trailingPawn_2, -68),
        P(trailingPawn_3, -45),
        P(trailingPawn_4, -20),
        P(trailingPawn_5, -45),

        P(capturePawn_0, 113),
        P(capturePawn_1, -71),
        P(capturePawn_2, -83),
        P(capturePawn_3, 27),
        P(capturePawn_4, 287),
        P(capturePawn_5, 0),

        P(defendedPawn_0, 0),
        P(defendedPawn_1, 62),
        P(defendedPawn_2, 8),
        P(defendedPawn_3, 118),
        P(defendedPawn_4, 244),
        P(defendedPawn_5, 238),

        P(pawnLever_0, 0),
        P(pawnLever_1, 0),
        P(pawnLever_2, -32),
        P(pawnLever_3, -37),
        P(pawnLever_4, -32),
        P(pawnLever_5, -23),
        P(pawnLever_6, 0),

        P(rammedPawn_0, -54),
        P(rammedPawn_1, -6),
        P(rammedPawn_2, 10),
        P(rammedPawn_3, -54),
        P(rammedPawn_4, 41),
        P(rammedPawn_5, 0),

        P(mobilePawn_0, 11),
        P(mobilePawn_1, 28),
        P(mobilePawn_2, 29),
        P(mobilePawn_3, 47),
        P(mobilePawn_4, 77),
        P(mobilePawn_5, 213),

        // Quadratic polynomials for passers
        P(passerA_0, 16),
        P(passerB_0, 57),
        P(passerC_0, 138),
        P(passerD_0, 196),
        P(passerE_0, 257),
        P(passerF_0, 27),
        P(passerG_0, -63),
        P(passerH_0, -101),

        P(passerA_1, 70),
        P(passerB_1, 18),
        P(passerC_1, -42),
        P(passerD_1, -52),
        P(passerE_1, -78),
        P(passerF_1, 72),
        P(passerG_1, 119),
        P(passerH_1, 167),

        P(passerA_2, 152),
        P(passerB_2, 197),
        P(passerC_2, 245),
        P(passerD_2, 240),
        P(passerE_2, 270),
        P(passerF_2, 150),
        P(passerG_2, 104),
        P(passerH_2, 58),

        P(passerScalingOffset, 3631),
        P(passerAndQueen, 1336),
        P(passerAndRook, 743),
        P(passerAndBishop, 725),
        P(passerAndKnight, 365),
        P(passerAndPawn, -558),
        P(passerVsQueen, -747),
        P(passerVsRook, -692),
        P(passerVsBishop, -1220),
        P(passerVsKnight, -865),
        P(passerVsPawn, 255),

        P(protectedPasser, -5), // TODO: check this after tuning
        P(connectedPasser, -38), // TODO: check this after tuning

        P(safePasser, 180), // no blocker and totally safe passage

        P(blocker_0, -113), // stop square
        P(blocker_1, -89),
        P(blocker_2, -36),
        P(blocker_3, -7),
        P(blocker_4, -14), // -furtest square

        P(blockedByOwn, 9), // TODO: check this after tuning
        P(blockedByOther, -108), // TODO: check this after tuning

        P(controller_0, -69), // stop square
        P(controller_1, -49),
        P(controller_2, 6),
        P(controller_3, 8),
        P(controller_4, -4), // -furtest square

        P(unstoppablePasser, 402),

        P(kingToPasser, 51),
        P(kingToOwnPasser, -62),

        P(candidateByRank_0, 26),
        P(candidateByRank_1, 59),
        P(candidateByRank_2, 91),
        P(candidateByRank_3, 150),
        P(candidateByRank_4, 187),

        P(candidateA, -11),
        P(candidateB, -83),
        P(candidateC, -62),
        P(candidateD, -80),
        P(candidateE, -21),
        P(candidateF, -14),
        P(candidateG, -33),
        P(candidateH, 27),

        /*
         *  Knights
         */

        // Kings on same side
        P(knightByFile_0, -154), // fileA
        P(knightByFile_1, -174),
        P(knightByFile_2, -186),
        P(knightByFile_3, -126),
        P(knightByFile_4, -74),
        P(knightByFile_5, -12),
        P(knightByFile_6, 29), // -fileH

        // Opposite side kings
        P(knightByFile_0x, -132), // fileA
        P(knightByFile_1x, -163),
        P(knightByFile_2x, -136),
        P(knightByFile_3x, -45),
        P(knightByFile_4x, 33),
        P(knightByFile_5x, 119),
        P(knightByFile_6x, 105), // -fileH

        P(knightByRank_0, -111), // rank1
        P(knightByRank_1, -196),
        P(knightByRank_2, -227),
        P(knightByRank_3, -190),
        P(knightByRank_4, -100),
        P(knightByRank_5, 43),
        P(knightByRank_6, 109), // -rank8

        P(knightAndSpan_0, -171), // span0 (pawnless)
        P(knightAndSpan_1, -125),
        P(knightAndSpan_2, -47),
        P(knightAndSpan_3, 20), // -span4 (7-8 files)

        P(knightVsSpan_0, -337), // span0 (pawnless)
        P(knightVsSpan_1, -209),
        P(knightVsSpan_2, -77),
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
        P(bishopBySquare_0, -130),
        P(bishopBySquare_1, 40),
        P(bishopBySquare_2, -32),
        P(bishopBySquare_3, 132),
        P(bishopBySquare_4, -20),
        P(bishopBySquare_5, 21),
        P(bishopBySquare_6, -42),
        P(bishopBySquare_7, 91),

        P(bishopBySquare_8, 2),
        P(bishopBySquare_9, -5),
        P(bishopBySquare_10, 130),
        P(bishopBySquare_11, 28),
        P(bishopBySquare_12, 66),
        P(bishopBySquare_13, 136),
        P(bishopBySquare_14, 93),
        P(bishopBySquare_15, 166),

        P(bishopBySquare_16, -91),
        P(bishopBySquare_17, 80),
        P(bishopBySquare_18, 62),
        P(bishopBySquare_19, 80),
        P(bishopBySquare_20, 78),
        P(bishopBySquare_21, 126),
        P(bishopBySquare_22, 105),
        P(bishopBySquare_23, 154),

        P(bishopBySquare_24, -55),
        P(bishopBySquare_25, -38),
        P(bishopBySquare_26, 74),
        P(bishopBySquare_27, 122),
        P(bishopBySquare_28, 188),
        P(bishopBySquare_29, 138),
        P(bishopBySquare_30, 141),
        P(bishopBySquare_31, 135),

        P(bishopBySquare_32, -62),
        P(bishopBySquare_33, 22),
        P(bishopBySquare_34, 28),
        P(bishopBySquare_35, 104),
        P(bishopBySquare_36, 138),
        P(bishopBySquare_37, 174),
        P(bishopBySquare_38, 68),
        P(bishopBySquare_39, 127),

        P(bishopBySquare_40, -76),
        P(bishopBySquare_41, 7),
        P(bishopBySquare_42, 69),
        P(bishopBySquare_43, 20),
        P(bishopBySquare_44, 148),
        P(bishopBySquare_45, 276),
        P(bishopBySquare_46, 82),
        P(bishopBySquare_47, 57),

        P(bishopBySquare_48, -55),
        P(bishopBySquare_49, 44),
        P(bishopBySquare_50, 54),
        P(bishopBySquare_51, 74),
        P(bishopBySquare_52, 24),
        P(bishopBySquare_53, 193),
        P(bishopBySquare_54, 5),
        P(bishopBySquare_55, 75),

        P(bishopBySquare_56, -223),
        P(bishopBySquare_57, -61),
        P(bishopBySquare_58, 1),
        P(bishopBySquare_59, 52),
        P(bishopBySquare_60, 24),
        P(bishopBySquare_61, 154),
        P(bishopBySquare_62, -217),
        P(bishopBySquare_63, -34),

        P(bishopBySquare_0x, -52),
        P(bishopBySquare_1x, 7),
        P(bishopBySquare_2x, 71),
        P(bishopBySquare_3x, 36),
        P(bishopBySquare_4x, 58),
        P(bishopBySquare_5x, 130),
        P(bishopBySquare_6x, -185),
        P(bishopBySquare_7x, -56),

        P(bishopBySquare_8x, -46),
        P(bishopBySquare_9x, -20),
        P(bishopBySquare_10x, 52),
        P(bishopBySquare_11x, 135),
        P(bishopBySquare_12x, 87),
        P(bishopBySquare_13x, 213),
        P(bishopBySquare_14x, 108),
        P(bishopBySquare_15x, 84),

        P(bishopBySquare_16x, -65),
        P(bishopBySquare_17x, 39),
        P(bishopBySquare_18x, 116),
        P(bishopBySquare_19x, 101),
        P(bishopBySquare_20x, 200),
        P(bishopBySquare_21x, 224),
        P(bishopBySquare_22x, 173),
        P(bishopBySquare_23x, 95),

        P(bishopBySquare_24x, -38),
        P(bishopBySquare_25x, 30),
        P(bishopBySquare_26x, 100),
        P(bishopBySquare_27x, 146),
        P(bishopBySquare_28x, 191),
        P(bishopBySquare_29x, 244),
        P(bishopBySquare_30x, 146),
        P(bishopBySquare_31x, 197),

        P(bishopBySquare_32x, -6),
        P(bishopBySquare_33x, 97),
        P(bishopBySquare_34x, 138),
        P(bishopBySquare_35x, 185),
        P(bishopBySquare_36x, 235),
        P(bishopBySquare_37x, 224),
        P(bishopBySquare_38x, 236),
        P(bishopBySquare_39x, 185),

        P(bishopBySquare_40x, -55),
        P(bishopBySquare_41x, 104),
        P(bishopBySquare_42x, 164),
        P(bishopBySquare_43x, 199),
        P(bishopBySquare_44x, 191),
        P(bishopBySquare_45x, 226),
        P(bishopBySquare_46x, 192),
        P(bishopBySquare_47x, 219),

        P(bishopBySquare_48x, 21),
        P(bishopBySquare_49x, 161),
        P(bishopBySquare_50x, 174),
        P(bishopBySquare_51x, 107),
        P(bishopBySquare_52x, 179),
        P(bishopBySquare_53x, 207),
        P(bishopBySquare_54x, 207),
        P(bishopBySquare_55x, 222),

        P(bishopBySquare_56x, -108),
        P(bishopBySquare_57x, 61),
        P(bishopBySquare_58x, 58),
        P(bishopBySquare_59x, 122),
        P(bishopBySquare_60x, 110),
        P(bishopBySquare_61x, 87),
        P(bishopBySquare_62x, 14),
        P(bishopBySquare_63x, 30),
#endif

        P(bishopAndSpan_0, -200), // span0 (pawnless)
        P(bishopAndSpan_1, -158),
        P(bishopAndSpan_2, -106),
        P(bishopAndSpan_3, -59), // -span4 (7-8 files)

        P(bishopVsSpan_0, -383), // span0 (pawnless)
        P(bishopVsSpan_1, -266),
        P(bishopVsSpan_2, -155),
        P(bishopVsSpan_3, -73), // -span4 (7-8 files)

        P(bishopAndLikePawn_1, -48),
        P(bishopAndLikePawn_2, -3),
        P(bishopAndLikeRammedPawn, -56),

        P(bishopVsLikePawn_1, -26),
        P(bishopVsLikePawn_2, -6),
        P(bishopVsLikeRammedPawn, 5),

        P(bishopToKing, 0),
        P(bishopToOwnKing, -10),

        /*
         *  Rooks
         */

        P(rookByFile_0, -69), // fileA
        P(rookByFile_1, -63),
        P(rookByFile_2, -28), // -fileD

        P(rookByRank_0, -85), // rank1
        P(rookByRank_1, -240),
        P(rookByRank_2, -377),
        P(rookByRank_3, -457),
        P(rookByRank_4, -413),
        P(rookByRank_5, -297),
        P(rookByRank_6, -151), // -rank8

        P(rookOnHalfOpen_0, 140), // fileA
        P(rookOnHalfOpen_1, 50),
        P(rookOnHalfOpen_2, 7),
        P(rookOnHalfOpen_3, -1), // fileD

        P(rookOnOpen_0, 225), // fileA
        P(rookOnOpen_1, 167),
        P(rookOnOpen_2, 110),
        P(rookOnOpen_3, 123), // fileD

        P(rookToWeakPawn_0, 156),
        P(rookToWeakPawn_1, 141),
        P(rookToWeakPawn_2, 117),
        P(rookToWeakPawn_3, 88),

        P(rookToKing_0, 289), // d=0
        P(rookToKing_1, 188), // d=1
        P(rookToKing_2, 133), // d=2
        P(rookToKing_3, 84),
        P(rookToKing_4, 30),
        P(rookToKing_5, -52),
        P(rookToKing_6, -99),
        P(rookToKing_7, -130),

        P(rookToKing, -15),
        P(rookToOwnKing, 3),

        P(rookInFrontPasser_0, -56), // opponent's
        P(rookInFrontPasser_1, 154), // own passer
        P(rookBehindPasser_0, 375), // opponent's
        P(rookBehindPasser_1, 55), // own passer

        /*
         *  Queens
         */

        P(queenByFile_0, -33), // fileA
        P(queenByFile_1, -40),
        P(queenByFile_2, -27), // -fileD

        P(queenByRank_0, -85), // rank1
        P(queenByRank_1, -145),
        P(queenByRank_2, -209),
        P(queenByRank_3, -248),
        P(queenByRank_4, -227),
        P(queenByRank_5, -171),
        P(queenByRank_6, -117), // -rank8

        P(queenToKing, -23),
        P(queenToOwnKing, -10),

        P(queenInFrontPasser_0, -81), // opponent's
        P(queenInFrontPasser_1, 46), // own passer
        P(queenBehindPasser_0, 166), // opponent's
        P(queenBehindPasser_1, -9), // own passer

        /*
         *  Kings
         */

        P(kingByFile_0, -34), // fileA
        P(kingByFile_1, -29),
        P(kingByFile_2, -22), // -fileD

        P(kingByRank_0, -263), // rank1
        P(kingByRank_1, -434),
        P(kingByRank_2, -597),
        P(kingByRank_3, -680),
        P(kingByRank_4, -606),
        P(kingByRank_5, -388),
        P(kingByRank_6, -142), // -rank8

        // Drawness. Positive is more drawish. Negative is more sharp
        P(drawOffset, 323),
        P(drawQueen, 104),
        P(drawRook, -21),
        P(drawBishop, -28),
        P(drawKnight, -15),
        P(drawPawn, -844),
        P(drawQueenEnding, -102),
        P(drawRookEnding, 621),
        P(drawKnightEnding, 670),
        P(drawBishopEnding, 1742),
        P(drawPawnEnding, -664),
        P(drawPawnless, 62),

        P(drawQueenImbalance, -833),
        P(drawRookImbalance, 430),
        P(drawMinorImbalance, 2),

        P(drawUnlikeBishops, 2364),
        P(drawUnlikeBishopsAndQueens, 878),
        P(drawUnlikeBishopsAndRooks, 417),
        P(drawUnlikeBishopsAndKnights, 675),

        P(drawRammed_0, 574), // fileA/H
        P(drawRammed_1, 532),
        P(drawRammed_2, 590),
        P(drawRammed_3, 476), // fileD/E
// }

/*----------------------------------------------------------------------+
 |                                                                      |
 +----------------------------------------------------------------------*/

