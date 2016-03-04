
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
        P(winBonus, 1491),

        P(castleK, -132),
        P(castleQ, -118),
        P(castleKQ, -63),

        P(queenValue, 6993),
        P(rookValue, 4676),
        P(bishopValue, 2609),
        P(knightValue, 2464),
        P(pawnValue1, 1003),
        P(pawnValue2, 1679),
        P(pawnValue3, 2354),
        P(pawnValue4, 3035),
        P(pawnValue5, 3699),
        P(pawnValue6, 4345),
        P(pawnValue7, 4954),
        P(pawnValue8, 5463),

        P(queenAndQueen, -705),
        P(queenAndRook, -709),
        P(queenAndBishop, 96),
        P(queenAndKnight, 31),
        P(queenAndPawn_1, -25),
        P(queenAndPawn_2, -160),

        P(rookAndRook, -288),
        P(rookAndBishop, -104),
        P(rookAndKnight, -103),
        P(rookAndPawn_1, -54),
        P(rookAndPawn_2, -58),

        P(bishopAndBishop, 119),
        P(bishopAndKnight, 15),
        P(bishopAndPawn_1, -48),
        P(bishopAndPawn_2, 2),

        P(knightAndKnight, -24),
        P(knightAndPawn_1, -20),
        P(knightAndPawn_2, -1),

        P(queenVsRook, -357),
        P(queenVsBishop, 79),
        P(queenVsKnight, 20),
        P(queenVsPawn_1, 171),
        P(queenVsPawn_2, -167),

        P(rookVsBishop, -22),
        P(rookVsKnight, 16),
        P(rookVsPawn_1, -2),
        P(rookVsPawn_2, -49),

        P(bishopVsKnight, 8),
        P(bishopVsPawn_1, -9),
        P(bishopVsPawn_2, 3),

        P(knightVsPawn_1, -2),
        P(knightVsPawn_2, -19),

        /*
         *  Board control
         */

        P(controlCenter, 27),
        P(controlExtendedCenter, 34),
        P(controlOutside, 19),

        /*
         *  King safety
         */

        // scaling
        P(safetyScalingOffset, -968),
        P(safetyAndQueen, -1793),
        P(safetyAndRook, 3209),
        P(safetyAndBishop, 1878),
        P(safetyAndKnight, 1940),
        P(safetyAndPawn, -495),
        P(safetyVsQueen, 12750),
        P(safetyVsRook, -3740),
        P(safetyVsBishop, -2438),
        P(safetyVsKnight, -2184),
        P(safetyVsPawn, -832),

        // shelter
        P(shelterPawn_0, 137), // rank3
        P(shelterPawn_1, 259), // rank4
        P(shelterPawn_2, 266), // rank5
        P(shelterPawn_3, 106), // rank6
        P(shelterPawn_4, -213), // rank7
        P(shelterPawn_5, 292), // no pawn

        P(shelterKing_0, -293), // fileA (fileH)
        P(shelterKing_1, -305),
        P(shelterKing_2, -184), // -fileD (-fileE)

        P(shelterWalkingKing, 46), // rank2 or up
        P(shelterCastled, 58), // fraction of 256

        // attacks
        P(attackSquares_0, 72), // 0 attacks
        P(attackSquares_1, -12),
        P(attackSquares_2, -133),
        P(attackSquares_3, -240),
        P(attackSquares_4, -296),
        P(attackSquares_5, -254), // -(6 or more attacks)

        P(attackByPawn_0, 129),
        P(attackByPawn_1, 292),
        P(attackByPawn_2, 68),
        P(attackByMinor_0, -108),
        P(attackByMinor_1, 8),
        P(attackByMinor_2, -255),
        P(attackByRook_0, 461),
        P(attackByRook_1, 976),
        P(attackByRook_2, -411),
        P(attackByQueen, 183),
        P(attackByKing, -258),

        // TODO: pawn storm

        // king mobility
        P(mobilityKing_0, 98),
        P(mobilityKing_1, 47),

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

        P(pawnByFile_0, 21), // fileA
        P(pawnByFile_1, 40),
        P(pawnByFile_2, 12),
        P(pawnByFile_3, -35),
        P(pawnByFile_4, -63),
        P(pawnByFile_5, -21),
        P(pawnByFile_6, 36), // -fileH

        P(pawnByFile_0x, -120), // fileA
        P(pawnByFile_1x, -220),
        P(pawnByFile_2x, -276),
        P(pawnByFile_3x, -352),
        P(pawnByFile_4x, -354),
        P(pawnByFile_5x, -282),
        P(pawnByFile_6x, -122), // -fileH

        // 5 parameters for 6 pawn ranks
        P(pawnByRank_0, -51), // rank2
        P(pawnByRank_1, -93),
        P(pawnByRank_2, -91),
        P(pawnByRank_3, -16),
        P(pawnByRank_4, 57), // -rank7

        P(doubledPawnA, -169),
        P(doubledPawnB, -88),
        P(doubledPawnC, -117),
        P(doubledPawnD, -79),
        P(doubledPawnE, -71),
        P(doubledPawnF, -22),
        P(doubledPawnG, 4),
        P(doubledPawnH, -175),

        P(backwardPawnClosedByRank_0, 21),
        P(backwardPawnClosedByRank_1, 16),
        P(backwardPawnClosedByRank_2, -13),
        P(backwardPawnClosedByRank_3, -8),
        P(backwardPawnClosedByRank_4, 0),

        P(backwardPawnOpenByRank_0, 31),
        P(backwardPawnOpenByRank_1, 21),
        P(backwardPawnOpenByRank_2, 7),
        P(backwardPawnOpenByRank_3, 41),
        P(backwardPawnOpenByRank_4, 0),

        P(backwardPawnClosedA, 58),
        P(backwardPawnClosedB, -6),
        P(backwardPawnClosedC, 23),
        P(backwardPawnClosedD, -33),
        P(backwardPawnClosedE, -51),
        P(backwardPawnClosedF, -17),
        P(backwardPawnClosedG, -4),
        P(backwardPawnClosedH, 53),

        P(backwardPawnOpenA, -9),
        P(backwardPawnOpenB, -35),
        P(backwardPawnOpenC, -21),
        P(backwardPawnOpenD, -48),
        P(backwardPawnOpenE, -37),
        P(backwardPawnOpenF, -24),
        P(backwardPawnOpenG, -41),
        P(backwardPawnOpenH, 15),

        P(rammedWeakPawnA, -12),
        P(rammedWeakPawnB, 8),
        P(rammedWeakPawnC, 1),
        P(rammedWeakPawnD, 0),
        P(rammedWeakPawnE, 24),
        P(rammedWeakPawnF, 5),
        P(rammedWeakPawnG, -11),
        P(rammedWeakPawnH, -47),

        P(isolatedPawnClosedA, 127),
        P(isolatedPawnClosedB, 36),
        P(isolatedPawnClosedC, -23),
        P(isolatedPawnClosedD, -7),
        P(isolatedPawnClosedE, -101),
        P(isolatedPawnClosedF, -104),
        P(isolatedPawnClosedG, -113),
        P(isolatedPawnClosedH, -23),

        P(isolatedPawnOpenA, 122),
        P(isolatedPawnOpenB, 69),
        P(isolatedPawnOpenC, -28),
        P(isolatedPawnOpenD, -57),
        P(isolatedPawnOpenE, -74),
        P(isolatedPawnOpenF, -25),
        P(isolatedPawnOpenG, 46),
        P(isolatedPawnOpenH, 82),

        P(sidePawnClosedA, 102),
        P(sidePawnClosedB, 44),
        P(sidePawnClosedC, 65),
        P(sidePawnClosedD, -37),
        P(sidePawnClosedE, -51),
        P(sidePawnClosedF, -63),
        P(sidePawnClosedG, -112),
        P(sidePawnClosedH, -23),

        P(sidePawnOpenA, 75),
        P(sidePawnOpenB, 54),
        P(sidePawnOpenC, 21),
        P(sidePawnOpenD, -26),
        P(sidePawnOpenE, -21),
        P(sidePawnOpenF, 6),
        P(sidePawnOpenG, 48),
        P(sidePawnOpenH, 42),

        P(middlePawnClosedA, 0),
        P(middlePawnClosedB, 18),
        P(middlePawnClosedC, 104),
        P(middlePawnClosedD, -18),
        P(middlePawnClosedE, -16),
        P(middlePawnClosedF, -12),
        P(middlePawnClosedG, -90),
        P(middlePawnClosedH, 0),

        P(middlePawnOpenA, 0),
        P(middlePawnOpenB, -62),
        P(middlePawnOpenC, 23),
        P(middlePawnOpenD, -6),
        P(middlePawnOpenE, 13),
        P(middlePawnOpenF, -24),
        P(middlePawnOpenG, -39),
        P(middlePawnOpenH, 0),

        P(duoPawnA, 8),
        P(duoPawnB, -6),
        P(duoPawnC, 0),
        P(duoPawnD, 24),
        P(duoPawnE, 29),
        P(duoPawnF, 14),
        P(duoPawnG, 35),
        P(duoPawnH, -62),

        P(openFilePawn_0, -25),
        P(openFilePawn_1, -28),
        P(openFilePawn_2, 38),
        P(openFilePawn_3, 26),
        P(openFilePawn_4, 112),
        P(openFilePawn_5, 256),

        P(stoppedPawn_0, -37),
        P(stoppedPawn_1, 26),
        P(stoppedPawn_2, 25),
        P(stoppedPawn_3, -11),
        P(stoppedPawn_4, 0),
        P(stoppedPawn_5, 0),

        P(duoPawn_0, 2),
        P(duoPawn_1, -3),
        P(duoPawn_2, 27),
        P(duoPawn_3, 142),
        P(duoPawn_4, 497),
        P(duoPawn_5, 1464),

        P(trailingPawn_0, -11),
        P(trailingPawn_1, -47),
        P(trailingPawn_2, -67),
        P(trailingPawn_3, -42),
        P(trailingPawn_4, -20),
        P(trailingPawn_5, -44),

        P(capturePawn_0, 130),
        P(capturePawn_1, -63),
        P(capturePawn_2, -85),
        P(capturePawn_3, 36),
        P(capturePawn_4, 302),
        P(capturePawn_5, 0),

        P(defendedPawn_0, 0),
        P(defendedPawn_1, 63),
        P(defendedPawn_2, 8),
        P(defendedPawn_3, 115),
        P(defendedPawn_4, 239),
        P(defendedPawn_5, 243),

        P(pawnLever_0, 0),
        P(pawnLever_1, 0),
        P(pawnLever_2, -31),
        P(pawnLever_3, -36),
        P(pawnLever_4, -31),
        P(pawnLever_5, -23),
        P(pawnLever_6, 0),

        P(rammedPawn_0, -50),
        P(rammedPawn_1, -5),
        P(rammedPawn_2, 3),
        P(rammedPawn_3, -55),
        P(rammedPawn_4, 46),
        P(rammedPawn_5, 0),

        P(mobilePawn_0, 9),
        P(mobilePawn_1, 24),
        P(mobilePawn_2, 29),
        P(mobilePawn_3, 47),
        P(mobilePawn_4, 116),
        P(mobilePawn_5, 214),

        // Quadratic polynomials for passers
        P(passerA_0, -23),
        P(passerB_0, 31),
        P(passerC_0, 123),
        P(passerD_0, 149),
        P(passerE_0, 204),
        P(passerF_0, 6),
        P(passerG_0, -84),
        P(passerH_0, -134),

        P(passerA_1, 71),
        P(passerB_1, 14),
        P(passerC_1, -49),
        P(passerD_1, -54),
        P(passerE_1, -86),
        P(passerF_1, 49),
        P(passerG_1, 108),
        P(passerH_1, 151),

        P(passerA_2, 142),
        P(passerB_2, 188),
        P(passerC_2, 236),
        P(passerD_2, 228),
        P(passerE_2, 268),
        P(passerF_2, 161),
        P(passerG_2, 102),
        P(passerH_2, 58),

        P(passerScalingOffset, 3707),
        P(passerAndQueen, 1434),
        P(passerAndRook, 835),
        P(passerAndBishop, 798),
        P(passerAndKnight, 369),
        P(passerAndPawn, -599),
        P(passerVsQueen, -716),
        P(passerVsRook, -728),
        P(passerVsBishop, -1378),
        P(passerVsKnight, -971),
        P(passerVsPawn, 258),

        P(protectedPasser, -10), // TODO: check this after tuning
        P(connectedPasser, -49), // TODO: check this after tuning

        P(safePasser, 186), // no blocker and totally safe passage

        P(blocker_0, -111), // stop square
        P(blocker_1, -87),
        P(blocker_2, -33),
        P(blocker_3, -5),
        P(blocker_4, -12), // -furtest square

        P(blockedByOwn, 39), // TODO: check this after tuning
        P(blockedByOther, -113), // TODO: check this after tuning

        P(controller_0, -71), // stop square
        P(controller_1, -56),
        P(controller_2, -5),
        P(controller_3, -4),
        P(controller_4, -16), // -furtest square

        P(unstoppablePasser, 407),

        P(kingToPasser, 46),
        P(kingToOwnPasser, -51),

        P(candidateByRank_0, 29),
        P(candidateByRank_1, 58),
        P(candidateByRank_2, 90),
        P(candidateByRank_3, 144),
        P(candidateByRank_4, 180),

        P(candidateA, -11),
        P(candidateB, -75),
        P(candidateC, -52),
        P(candidateD, -71),
        P(candidateE, -9),
        P(candidateF, -3),
        P(candidateG, -19),
        P(candidateH, 28),

        /*
         *  Knights
         */

        // Kings on same side
        P(knightByFile_0, -165), // fileA
        P(knightByFile_1, -191),
        P(knightByFile_2, -204),
        P(knightByFile_3, -143),
        P(knightByFile_4, -88),
        P(knightByFile_5, -21),
        P(knightByFile_6, 23), // -fileH

        // Opposite side kings
        P(knightByFile_0x, -136), // fileA
        P(knightByFile_1x, -169),
        P(knightByFile_2x, -141),
        P(knightByFile_3x, -47),
        P(knightByFile_4x, 35),
        P(knightByFile_5x, 122),
        P(knightByFile_6x, 109), // -fileH

        P(knightByRank_0, -115), // rank1
        P(knightByRank_1, -201),
        P(knightByRank_2, -229),
        P(knightByRank_3, -188),
        P(knightByRank_4, -93),
        P(knightByRank_5, 53),
        P(knightByRank_6, 115), // -rank8

        P(knightAndSpan_0, -171), // span0 (pawnless)
        P(knightAndSpan_1, -121),
        P(knightAndSpan_2, -41),
        P(knightAndSpan_3, 22), // -span4 (7-8 files)

        P(knightVsSpan_0, -334), // span0 (pawnless)
        P(knightVsSpan_1, -211),
        P(knightVsSpan_2, -78),
        P(knightVsSpan_3, 4), // -span4 (7-8 files)

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
        P(bishopBySquare_0, -137),
        P(bishopBySquare_1, 32),
        P(bishopBySquare_2, -42),
        P(bishopBySquare_3, 124),
        P(bishopBySquare_4, -31),
        P(bishopBySquare_5, 9),
        P(bishopBySquare_6, -53),
        P(bishopBySquare_7, 90),

        P(bishopBySquare_8, -4),
        P(bishopBySquare_9, -12),
        P(bishopBySquare_10, 123),
        P(bishopBySquare_11, 21),
        P(bishopBySquare_12, 60),
        P(bishopBySquare_13, 130),
        P(bishopBySquare_14, 82),
        P(bishopBySquare_15, 159),

        P(bishopBySquare_16, -94),
        P(bishopBySquare_17, 75),
        P(bishopBySquare_18, 56),
        P(bishopBySquare_19, 77),
        P(bishopBySquare_20, 72),
        P(bishopBySquare_21, 116),
        P(bishopBySquare_22, 98),
        P(bishopBySquare_23, 144),

        P(bishopBySquare_24, -60),
        P(bishopBySquare_25, -40),
        P(bishopBySquare_26, 71),
        P(bishopBySquare_27, 119),
        P(bishopBySquare_28, 180),
        P(bishopBySquare_29, 131),
        P(bishopBySquare_30, 134),
        P(bishopBySquare_31, 128),

        P(bishopBySquare_32, -63),
        P(bishopBySquare_33, 21),
        P(bishopBySquare_34, 28),
        P(bishopBySquare_35, 102),
        P(bishopBySquare_36, 135),
        P(bishopBySquare_37, 166),
        P(bishopBySquare_38, 58),
        P(bishopBySquare_39, 120),

        P(bishopBySquare_40, -77),
        P(bishopBySquare_41, 7),
        P(bishopBySquare_42, 69),
        P(bishopBySquare_43, 19),
        P(bishopBySquare_44, 143),
        P(bishopBySquare_45, 270),
        P(bishopBySquare_46, 72),
        P(bishopBySquare_47, 53),

        P(bishopBySquare_48, -55),
        P(bishopBySquare_49, 45),
        P(bishopBySquare_50, 53),
        P(bishopBySquare_51, 72),
        P(bishopBySquare_52, 21),
        P(bishopBySquare_53, 188),
        P(bishopBySquare_54, -5),
        P(bishopBySquare_55, 66),

        P(bishopBySquare_56, -226),
        P(bishopBySquare_57, -58),
        P(bishopBySquare_58, 1),
        P(bishopBySquare_59, 48),
        P(bishopBySquare_60, 19),
        P(bishopBySquare_61, 146),
        P(bishopBySquare_62, -223),
        P(bishopBySquare_63, -36),

        P(bishopBySquare_0x, -65),
        P(bishopBySquare_1x, -3),
        P(bishopBySquare_2x, 65),
        P(bishopBySquare_3x, 29),
        P(bishopBySquare_4x, 49),
        P(bishopBySquare_5x, 120),
        P(bishopBySquare_6x, -191),
        P(bishopBySquare_7x, -66),

        P(bishopBySquare_8x, -52),
        P(bishopBySquare_9x, -27),
        P(bishopBySquare_10x, 47),
        P(bishopBySquare_11x, 128),
        P(bishopBySquare_12x, 81),
        P(bishopBySquare_13x, 207),
        P(bishopBySquare_14x, 101),
        P(bishopBySquare_15x, 79),

        P(bishopBySquare_16x, -70),
        P(bishopBySquare_17x, 33),
        P(bishopBySquare_18x, 110),
        P(bishopBySquare_19x, 97),
        P(bishopBySquare_20x, 193),
        P(bishopBySquare_21x, 219),
        P(bishopBySquare_22x, 160),
        P(bishopBySquare_23x, 92),

        P(bishopBySquare_24x, -40),
        P(bishopBySquare_25x, 28),
        P(bishopBySquare_26x, 101),
        P(bishopBySquare_27x, 143),
        P(bishopBySquare_28x, 186),
        P(bishopBySquare_29x, 235),
        P(bishopBySquare_30x, 140),
        P(bishopBySquare_31x, 192),

        P(bishopBySquare_32x, -11),
        P(bishopBySquare_33x, 97),
        P(bishopBySquare_34x, 137),
        P(bishopBySquare_35x, 181),
        P(bishopBySquare_36x, 230),
        P(bishopBySquare_37x, 217),
        P(bishopBySquare_38x, 234),
        P(bishopBySquare_39x, 176),

        P(bishopBySquare_40x, -54),
        P(bishopBySquare_41x, 105),
        P(bishopBySquare_42x, 163),
        P(bishopBySquare_43x, 196),
        P(bishopBySquare_44x, 190),
        P(bishopBySquare_45x, 215),
        P(bishopBySquare_46x, 185),
        P(bishopBySquare_47x, 209),

        P(bishopBySquare_48x, 19),
        P(bishopBySquare_49x, 164),
        P(bishopBySquare_50x, 173),
        P(bishopBySquare_51x, 104),
        P(bishopBySquare_52x, 177),
        P(bishopBySquare_53x, 201),
        P(bishopBySquare_54x, 199),
        P(bishopBySquare_55x, 211),

        P(bishopBySquare_56x, -113),
        P(bishopBySquare_57x, 57),
        P(bishopBySquare_58x, 57),
        P(bishopBySquare_59x, 116),
        P(bishopBySquare_60x, 102),
        P(bishopBySquare_61x, 79),
        P(bishopBySquare_62x, 3),
        P(bishopBySquare_63x, 22),
#endif

        P(bishopAndSpan_0, -206), // span0 (pawnless)
        P(bishopAndSpan_1, -158),
        P(bishopAndSpan_2, -103),
        P(bishopAndSpan_3, -58), // -span4 (7-8 files)

        P(bishopVsSpan_0, -398), // span0 (pawnless)
        P(bishopVsSpan_1, -277),
        P(bishopVsSpan_2, -163),
        P(bishopVsSpan_3, -79), // -span4 (7-8 files)

        P(bishopAndLikePawn_1, -46),
        P(bishopAndLikePawn_2, -3),
        P(bishopAndLikeRammedPawn, -58),

        P(bishopVsLikePawn_1, -26),
        P(bishopVsLikePawn_2, -6),
        P(bishopVsLikeRammedPawn, 6),

        /*
         *  Rooks
         */

        P(rookByFile_0, -69), // fileA
        P(rookByFile_1, -63),
        P(rookByFile_2, -29), // -fileD

        P(rookByRank_0, -91), // rank1
        P(rookByRank_1, -251),
        P(rookByRank_2, -393),
        P(rookByRank_3, -473),
        P(rookByRank_4, -430),
        P(rookByRank_5, -308),
        P(rookByRank_6, -157), // -rank8

        P(rookOnHalfOpen_0, 137), // fileA
        P(rookOnHalfOpen_1, 48),
        P(rookOnHalfOpen_2, 7),
        P(rookOnHalfOpen_3, -4), // fileD

        P(rookOnOpen_0, 225), // fileA
        P(rookOnOpen_1, 168),
        P(rookOnOpen_2, 112),
        P(rookOnOpen_3, 122), // fileD

        P(rookToWeakPawn_0, 157),
        P(rookToWeakPawn_1, 143),
        P(rookToWeakPawn_2, 120),
        P(rookToWeakPawn_3, 87),

        P(rookToKing_0, 295), // d=0
        P(rookToKing_1, 190), // d=1
        P(rookToKing_2, 131), // d=2
        P(rookToKing_3, 78),
        P(rookToKing_4, 24),
        P(rookToKing_5, -58),
        P(rookToKing_6, -104),
        P(rookToKing_7, -132),

        P(rookInFrontPasser_0, -50), // opponent's
        P(rookInFrontPasser_1, 123), // own passer

        P(rookBehindPasser_0, 372), // opponent's
        P(rookBehindPasser_1, 56), // own passer

        /*
         *  Queens
         */

        P(queenByFile_0, -33), // fileA
        P(queenByFile_1, -40),
        P(queenByFile_2, -27), // -fileD

        P(queenByRank_0, -91), // rank1
        P(queenByRank_1, -155),
        P(queenByRank_2, -224),
        P(queenByRank_3, -264),
        P(queenByRank_4, -241),
        P(queenByRank_5, -179),
        P(queenByRank_6, -120), // -rank8

        P(queenToKing, -23),
        P(queenToOwnKing, -10),
        P(rookToKing, -14),
        P(rookToOwnKing, 2),
        P(bishopToKing, 0),
        P(bishopToOwnKing, -8),
        P(knightToKing, -15),
        P(knightToOwnKing, -10),

        /*
         *  Kings
         */

        P(kingByFile_0, -40), // fileA
        P(kingByFile_1, -37),
        P(kingByFile_2, -26), // -fileD

        P(kingByRank_0, -264), // rank1
        P(kingByRank_1, -435),
        P(kingByRank_2, -595),
        P(kingByRank_3, -673),
        P(kingByRank_4, -589),
        P(kingByRank_5, -356),
        P(kingByRank_6, -104), // -rank8

        // Drawness. Positive is more drawish. Negative is more sharp
        P(drawOffset, 284),
        P(drawQueen, 111),
        P(drawRook, -13),
        P(drawBishop, -26),
        P(drawKnight, -15),
        P(drawPawn, -842),
        P(drawQueenEnding, -97),
        P(drawRookEnding, 641),
        P(drawKnightEnding, 688),
        P(drawBishopEnding, 1769),
        P(drawPawnEnding, -670),
        P(drawPawnless, 79),

        P(drawQueenImbalance, -821),
        P(drawRookImbalance, 407),
        P(drawMinorImbalance, 38),

        P(drawUnlikeBishops, 2344),
        P(drawUnlikeBishopsAndQueens, 849),
        P(drawUnlikeBishopsAndRooks, 418),
        P(drawUnlikeBishopsAndKnights, 699),

        P(drawRammed_0, 573), // fileA/H
        P(drawRammed_1, 532),
        P(drawRammed_2, 588),
        P(drawRammed_3, 468), // fileD/E
// }

/*----------------------------------------------------------------------+
 |                                                                      |
 +----------------------------------------------------------------------*/

