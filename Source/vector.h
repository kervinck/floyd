
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
        P(tempo, 41),
        P(winBonus, 1492),

        P(castleK, -131),
        P(castleQ, -123),
        P(castleKQ, -64),

        P(queenValue, 6974),
        P(rookValue, 4691),
        P(bishopValue, 2597),
        P(knightValue, 2447),
        P(pawnValue1, 999),
        P(pawnValue2, 1677),
        P(pawnValue3, 2354),
        P(pawnValue4, 3035),
        P(pawnValue5, 3699),
        P(pawnValue6, 4345),
        P(pawnValue7, 4957),
        P(pawnValue8, 5474),

        P(queenAndQueen, -706),
        P(queenAndRook, -702),
        P(queenAndBishop, 93),
        P(queenAndKnight, 30),
        P(queenAndPawn_1, -31),
        P(queenAndPawn_2, -152),

        P(rookAndRook, -289),
        P(rookAndBishop, -103),
        P(rookAndKnight, -100),
        P(rookAndPawn_1, -54),
        P(rookAndPawn_2, -58),

        P(bishopAndBishop, 122),
        P(bishopAndKnight, 19),
        P(bishopAndPawn_1, -48),
        P(bishopAndPawn_2, 2),

        P(knightAndKnight, -21),
        P(knightAndPawn_1, -20),
        P(knightAndPawn_2, -1),

        P(queenVsRook, -343),
        P(queenVsBishop, 73),
        P(queenVsKnight, 19),
        P(queenVsPawn_1, 169),
        P(queenVsPawn_2, -162),

        P(rookVsBishop, -21),
        P(rookVsKnight, 12),
        P(rookVsPawn_1, -4),
        P(rookVsPawn_2, -49),

        P(bishopVsKnight, 13),
        P(bishopVsPawn_1, -9),
        P(bishopVsPawn_2, 3),

        P(knightVsPawn_1, -2),
        P(knightVsPawn_2, -19),

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
        P(safetyScalingOffset, -968),
        P(safetyAndQueen, -1793),
        P(safetyAndRook, 3209),
        P(safetyAndBishop, 1878),
        P(safetyAndKnight, 1934),
        P(safetyAndPawn, -495),
        P(safetyVsQueen, 12751),
        P(safetyVsRook, -3740),
        P(safetyVsBishop, -2438),
        P(safetyVsKnight, -2184),
        P(safetyVsPawn, -832),

        // shelter
        P(shelterPawn_0, 137), // rank3
        P(shelterPawn_1, 258), // rank4
        P(shelterPawn_2, 264), // rank5
        P(shelterPawn_3, 102), // rank6
        P(shelterPawn_4, -236), // rank7
        P(shelterPawn_5, 290), // no pawn

        P(shelterKing_0, -281), // fileA (fileH)
        P(shelterKing_1, -297),
        P(shelterKing_2, -179), // -fileD (-fileE)

        P(shelterWalkingKing, 46), // rank2 or up
        P(shelterCastled, 58), // fraction of 256

        // attacks
        P(attackSquares_0, 74), // 0 attacks
        P(attackSquares_1, -7),
        P(attackSquares_2, -127),
        P(attackSquares_3, -236),
        P(attackSquares_4, -291),
        P(attackSquares_5, -248), // -(6 or more attacks)

        P(attackByPawn_0, 132),
        P(attackByPawn_1, 298),
        P(attackByPawn_2, 66),
        P(attackByMinor_0, -108),
        P(attackByMinor_1, 13),
        P(attackByMinor_2, -262),
        P(attackByRook_0, 452),
        P(attackByRook_1, 965),
        P(attackByRook_2, -426),
        P(attackByQueen, 180),
        P(attackByKing, -277),

        // TODO: pawn storm

        // king mobility
        P(mobilityKing_0, 109),
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
        P(pawnByFile_1, 42),
        P(pawnByFile_2, 12),
        P(pawnByFile_3, -34),
        P(pawnByFile_4, -62),
        P(pawnByFile_5, -21),
        P(pawnByFile_6, 37), // -fileH

        P(pawnByFile_0x, -120), // fileA
        P(pawnByFile_1x, -220),
        P(pawnByFile_2x, -280),
        P(pawnByFile_3x, -359),
        P(pawnByFile_4x, -358),
        P(pawnByFile_5x, -288),
        P(pawnByFile_6x, -122), // -fileH

        // 5 parameters for 6 pawn ranks
        P(pawnByRank_0, -51), // rank2
        P(pawnByRank_1, -93),
        P(pawnByRank_2, -91),
        P(pawnByRank_3, -17),
        P(pawnByRank_4, 54), // -rank7

        P(doubledPawnA, -174),
        P(doubledPawnB, -93),
        P(doubledPawnC, -127),
        P(doubledPawnD, -88),
        P(doubledPawnE, -76),
        P(doubledPawnF, -27),
        P(doubledPawnG, -2),
        P(doubledPawnH, -180),

        P(backwardPawnClosedByRank_0, 18),
        P(backwardPawnClosedByRank_1, 15),
        P(backwardPawnClosedByRank_2, -13),
        P(backwardPawnClosedByRank_3, -10),
        P(backwardPawnClosedByRank_4, 0),

        P(backwardPawnOpenByRank_0, 29),
        P(backwardPawnOpenByRank_1, 20),
        P(backwardPawnOpenByRank_2, 8),
        P(backwardPawnOpenByRank_3, 46),
        P(backwardPawnOpenByRank_4, 0),

        P(backwardPawnClosedA, 59),
        P(backwardPawnClosedB, -3),
        P(backwardPawnClosedC, 23),
        P(backwardPawnClosedD, -32),
        P(backwardPawnClosedE, -47),
        P(backwardPawnClosedF, -18),
        P(backwardPawnClosedG, -2),
        P(backwardPawnClosedH, 55),

        P(backwardPawnOpenA, -22),
        P(backwardPawnOpenB, -41),
        P(backwardPawnOpenC, -23),
        P(backwardPawnOpenD, -46),
        P(backwardPawnOpenE, -34),
        P(backwardPawnOpenF, -24),
        P(backwardPawnOpenG, -46),
        P(backwardPawnOpenH, 9),

        P(rammedWeakPawnA, -14),
        P(rammedWeakPawnB, 6),
        P(rammedWeakPawnC, 1),
        P(rammedWeakPawnD, 0),
        P(rammedWeakPawnE, 22),
        P(rammedWeakPawnF, 7),
        P(rammedWeakPawnG, -11),
        P(rammedWeakPawnH, -47),

        P(isolatedPawnClosedA, 133),
        P(isolatedPawnClosedB, 43),
        P(isolatedPawnClosedC, -12),
        P(isolatedPawnClosedD, 2),
        P(isolatedPawnClosedE, -99),
        P(isolatedPawnClosedF, -101),
        P(isolatedPawnClosedG, -115),
        P(isolatedPawnClosedH, -23),

        P(isolatedPawnOpenA, 123),
        P(isolatedPawnOpenB, 63),
        P(isolatedPawnOpenC, -28),
        P(isolatedPawnOpenD, -66),
        P(isolatedPawnOpenE, -86),
        P(isolatedPawnOpenF, -30),
        P(isolatedPawnOpenG, 37),
        P(isolatedPawnOpenH, 80),

        P(sidePawnClosedA, 104),
        P(sidePawnClosedB, 47),
        P(sidePawnClosedC, 70),
        P(sidePawnClosedD, -29),
        P(sidePawnClosedE, -50),
        P(sidePawnClosedF, -63),
        P(sidePawnClosedG, -117),
        P(sidePawnClosedH, -23),

        P(sidePawnOpenA, 84),
        P(sidePawnOpenB, 53),
        P(sidePawnOpenC, 20),
        P(sidePawnOpenD, -32),
        P(sidePawnOpenE, -28),
        P(sidePawnOpenF, 2),
        P(sidePawnOpenG, 42),
        P(sidePawnOpenH, 42),

        P(middlePawnClosedA, 0),
        P(middlePawnClosedB, 19),
        P(middlePawnClosedC, 107),
        P(middlePawnClosedD, -10),
        P(middlePawnClosedE, -15),
        P(middlePawnClosedF, -14),
        P(middlePawnClosedG, -94),
        P(middlePawnClosedH, 0),

        P(middlePawnOpenA, 0),
        P(middlePawnOpenB, -60),
        P(middlePawnOpenC, 22),
        P(middlePawnOpenD, -11),
        P(middlePawnOpenE, 7),
        P(middlePawnOpenF, -28),
        P(middlePawnOpenG, -41),
        P(middlePawnOpenH, 0),

        P(duoPawnA, 9),
        P(duoPawnB, -7),
        P(duoPawnC, -1),
        P(duoPawnD, 24),
        P(duoPawnE, 29),
        P(duoPawnF, 15),
        P(duoPawnG, 35),
        P(duoPawnH, -61),

        P(openFilePawn_0, -25),
        P(openFilePawn_1, -29),
        P(openFilePawn_2, 37),
        P(openFilePawn_3, 24),
        P(openFilePawn_4, 109),
        P(openFilePawn_5, 256),

        P(stoppedPawn_0, -37),
        P(stoppedPawn_1, 26),
        P(stoppedPawn_2, 24),
        P(stoppedPawn_3, -12),
        P(stoppedPawn_4, 0),
        P(stoppedPawn_5, 0),

        P(duoPawn_0, 2),
        P(duoPawn_1, -3),
        P(duoPawn_2, 28),
        P(duoPawn_3, 143),
        P(duoPawn_4, 500),
        P(duoPawn_5, 1477),

        P(trailingPawn_0, -11),
        P(trailingPawn_1, -47),
        P(trailingPawn_2, -69),
        P(trailingPawn_3, -42),
        P(trailingPawn_4, -21),
        P(trailingPawn_5, -52),

        P(capturePawn_0, 104),
        P(capturePawn_1, -64),
        P(capturePawn_2, -83),
        P(capturePawn_3, 36),
        P(capturePawn_4, 282),
        P(capturePawn_5, 0),

        P(defendedPawn_0, 0),
        P(defendedPawn_1, 63),
        P(defendedPawn_2, 8),
        P(defendedPawn_3, 115),
        P(defendedPawn_4, 237),
        P(defendedPawn_5, 226),

        P(pawnLever_0, 0),
        P(pawnLever_1, 0),
        P(pawnLever_2, -33),
        P(pawnLever_3, -36),
        P(pawnLever_4, -30),
        P(pawnLever_5, -22),
        P(pawnLever_6, 0),

        P(rammedPawn_0, -52),
        P(rammedPawn_1, -4),
        P(rammedPawn_2, 2),
        P(rammedPawn_3, -54),
        P(rammedPawn_4, 42),
        P(rammedPawn_5, 0),

        P(mobilePawn_0, 7),
        P(mobilePawn_1, 22),
        P(mobilePawn_2, 26),
        P(mobilePawn_3, 40),
        P(mobilePawn_4, 107),
        P(mobilePawn_5, 196),

        // Quadratic polynomials for passers
        P(passerA_0, -23),
        P(passerB_0, 44),
        P(passerC_0, 120),
        P(passerD_0, 149),
        P(passerE_0, 218),
        P(passerF_0, 17),
        P(passerG_0, -74),
        P(passerH_0, -116),

        P(passerA_1, 71),
        P(passerB_1, 3),
        P(passerC_1, -51),
        P(passerD_1, -53),
        P(passerE_1, -91),
        P(passerF_1, 45),
        P(passerG_1, 105),
        P(passerH_1, 141),

        P(passerA_2, 134),
        P(passerB_2, 189),
        P(passerC_2, 235),
        P(passerD_2, 226),
        P(passerE_2, 269),
        P(passerF_2, 159),
        P(passerG_2, 96),
        P(passerH_2, 60),

        P(passerScalingOffset, 3667),
        P(passerAndQueen, 1477),
        P(passerAndRook, 917),
        P(passerAndBishop, 747),
        P(passerAndKnight, 370),
        P(passerAndPawn, -600),
        P(passerVsQueen, -576),
        P(passerVsRook, -729),
        P(passerVsBishop, -1403),
        P(passerVsKnight, -928),
        P(passerVsPawn, 257),

        P(protectedPasser, -23), // TODO: check this after tuning
        P(connectedPasser, -53), // TODO: check this after tuning

        P(safePasser, 160), // no blocker and totally safe passage

        P(blocker_0, -105), // stop square
        P(blocker_1, -84),
        P(blocker_2, -30),
        P(blocker_3, 1),
        P(blocker_4, -9), // -furtest square

        P(blockedByOwn, 57), // TODO: check this after tuning
        P(blockedByOther, -78), // TODO: check this after tuning

        P(controller_0, -64), // stop square
        P(controller_1, -47),
        P(controller_2, 3),
        P(controller_3, 3),
        P(controller_4, -13), // -furtest square

        P(unstoppablePasser, 427),

        P(kingToPasser, 46),
        P(kingToOwnPasser, -50),

        P(candidateByRank_0, 23),
        P(candidateByRank_1, 59),
        P(candidateByRank_2, 91),
        P(candidateByRank_3, 151),
        P(candidateByRank_4, 192),

        P(candidateA, -25),
        P(candidateB, -73),
        P(candidateC, -54),
        P(candidateD, -74),
        P(candidateE, -11),
        P(candidateF, -2),
        P(candidateG, -20),
        P(candidateH, 26),

        /*
         *  Knights
         */

        // Kings on same side
        P(knightByFile_0, -168), // fileA
        P(knightByFile_1, -195),
        P(knightByFile_2, -207),
        P(knightByFile_3, -146),
        P(knightByFile_4, -90),
        P(knightByFile_5, -22),
        P(knightByFile_6, 22), // -fileH

        // Opposite side kings
        P(knightByFile_0x, -136), // fileA
        P(knightByFile_1x, -168),
        P(knightByFile_2x, -140),
        P(knightByFile_3x, -45),
        P(knightByFile_4x, 37),
        P(knightByFile_5x, 125),
        P(knightByFile_6x, 111), // -fileH

        P(knightByRank_0, -116), // rank1
        P(knightByRank_1, -202),
        P(knightByRank_2, -229),
        P(knightByRank_3, -185),
        P(knightByRank_4, -89),
        P(knightByRank_5, 56),
        P(knightByRank_6, 117), // -rank8

        P(knightAndSpan_0, -161), // span0 (pawnless)
        P(knightAndSpan_1, -111),
        P(knightAndSpan_2, -36),
        P(knightAndSpan_3, 24), // -span4 (7-8 files)

        P(knightVsSpan_0, -330), // span0 (pawnless)
        P(knightVsSpan_1, -210),
        P(knightVsSpan_2, -83),
        P(knightVsSpan_3, 2), // -span4 (7-8 files)

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
        P(bishopBySquare_0, -142),
        P(bishopBySquare_1, 28),
        P(bishopBySquare_2, -48),
        P(bishopBySquare_3, 119),
        P(bishopBySquare_4, -33),
        P(bishopBySquare_5, 4),
        P(bishopBySquare_6, -61),
        P(bishopBySquare_7, 77),

        P(bishopBySquare_8, -10),
        P(bishopBySquare_9, -15),
        P(bishopBySquare_10, 119),
        P(bishopBySquare_11, 15),
        P(bishopBySquare_12, 56),
        P(bishopBySquare_13, 123),
        P(bishopBySquare_14, 79),
        P(bishopBySquare_15, 149),

        P(bishopBySquare_16, -98),
        P(bishopBySquare_17, 71),
        P(bishopBySquare_18, 53),
        P(bishopBySquare_19, 72),
        P(bishopBySquare_20, 71),
        P(bishopBySquare_21, 110),
        P(bishopBySquare_22, 88),
        P(bishopBySquare_23, 132),

        P(bishopBySquare_24, -61),
        P(bishopBySquare_25, -42),
        P(bishopBySquare_26, 70),
        P(bishopBySquare_27, 116),
        P(bishopBySquare_28, 180),
        P(bishopBySquare_29, 125),
        P(bishopBySquare_30, 129),
        P(bishopBySquare_31, 115),

        P(bishopBySquare_32, -66),
        P(bishopBySquare_33, 21),
        P(bishopBySquare_34, 25),
        P(bishopBySquare_35, 99),
        P(bishopBySquare_36, 131),
        P(bishopBySquare_37, 164),
        P(bishopBySquare_38, 50),
        P(bishopBySquare_39, 109),

        P(bishopBySquare_40, -76),
        P(bishopBySquare_41, 6),
        P(bishopBySquare_42, 68),
        P(bishopBySquare_43, 15),
        P(bishopBySquare_44, 140),
        P(bishopBySquare_45, 265),
        P(bishopBySquare_46, 66),
        P(bishopBySquare_47, 39),

        P(bishopBySquare_48, -55),
        P(bishopBySquare_49, 45),
        P(bishopBySquare_50, 51),
        P(bishopBySquare_51, 68),
        P(bishopBySquare_52, 15),
        P(bishopBySquare_53, 183),
        P(bishopBySquare_54, -18),
        P(bishopBySquare_55, 57),

        P(bishopBySquare_56, -226),
        P(bishopBySquare_57, -62),
        P(bishopBySquare_58, 0),
        P(bishopBySquare_59, 45),
        P(bishopBySquare_60, 14),
        P(bishopBySquare_61, 137),
        P(bishopBySquare_62, -232),
        P(bishopBySquare_63, -43),

        P(bishopBySquare_0x, -65),
        P(bishopBySquare_1x, -6),
        P(bishopBySquare_2x, 59),
        P(bishopBySquare_3x, 22),
        P(bishopBySquare_4x, 44),
        P(bishopBySquare_5x, 112),
        P(bishopBySquare_6x, -198),
        P(bishopBySquare_7x, -72),

        P(bishopBySquare_8x, -54),
        P(bishopBySquare_9x, -31),
        P(bishopBySquare_10x, 45),
        P(bishopBySquare_11x, 127),
        P(bishopBySquare_12x, 77),
        P(bishopBySquare_13x, 197),
        P(bishopBySquare_14x, 87),
        P(bishopBySquare_15x, 65),

        P(bishopBySquare_16x, -75),
        P(bishopBySquare_17x, 30),
        P(bishopBySquare_18x, 106),
        P(bishopBySquare_19x, 92),
        P(bishopBySquare_20x, 189),
        P(bishopBySquare_21x, 214),
        P(bishopBySquare_22x, 150),
        P(bishopBySquare_23x, 79),

        P(bishopBySquare_24x, -43),
        P(bishopBySquare_25x, 27),
        P(bishopBySquare_26x, 99),
        P(bishopBySquare_27x, 143),
        P(bishopBySquare_28x, 183),
        P(bishopBySquare_29x, 230),
        P(bishopBySquare_30x, 135),
        P(bishopBySquare_31x, 177),

        P(bishopBySquare_32x, -14),
        P(bishopBySquare_33x, 94),
        P(bishopBySquare_34x, 133),
        P(bishopBySquare_35x, 179),
        P(bishopBySquare_36x, 227),
        P(bishopBySquare_37x, 213),
        P(bishopBySquare_38x, 223),
        P(bishopBySquare_39x, 159),

        P(bishopBySquare_40x, -59),
        P(bishopBySquare_41x, 102),
        P(bishopBySquare_42x, 160),
        P(bishopBySquare_43x, 192),
        P(bishopBySquare_44x, 185),
        P(bishopBySquare_45x, 211),
        P(bishopBySquare_46x, 174),
        P(bishopBySquare_47x, 197),

        P(bishopBySquare_48x, 18),
        P(bishopBySquare_49x, 161),
        P(bishopBySquare_50x, 170),
        P(bishopBySquare_51x, 99),
        P(bishopBySquare_52x, 169),
        P(bishopBySquare_53x, 190),
        P(bishopBySquare_54x, 192),
        P(bishopBySquare_55x, 201),

        P(bishopBySquare_56x, -113),
        P(bishopBySquare_57x, 58),
        P(bishopBySquare_58x, 55),
        P(bishopBySquare_59x, 111),
        P(bishopBySquare_60x, 94),
        P(bishopBySquare_61x, 70),
        P(bishopBySquare_62x, -5),
        P(bishopBySquare_63x, 16),
#endif

        P(bishopAndSpan_0, -201), // span0 (pawnless)
        P(bishopAndSpan_1, -152),
        P(bishopAndSpan_2, -101),
        P(bishopAndSpan_3, -58), // -span4 (7-8 files)

        P(bishopVsSpan_0, -396), // span0 (pawnless)
        P(bishopVsSpan_1, -278),
        P(bishopVsSpan_2, -166),
        P(bishopVsSpan_3, -80), // -span4 (7-8 files)

        P(bishopAndLikePawn_1, -46),
        P(bishopAndLikePawn_2, -3),
        P(bishopAndLikeRammedPawn, -56),

        P(bishopVsLikePawn_1, -25),
        P(bishopVsLikePawn_2, -6),
        P(bishopVsLikeRammedPawn, 5),

        /*
         *  Rooks
         */

        P(rookByFile_0, -68), // fileA
        P(rookByFile_1, -62),
        P(rookByFile_2, -29), // -fileD

        P(rookByRank_0, -101), // rank1
        P(rookByRank_1, -266),
        P(rookByRank_2, -412),
        P(rookByRank_3, -494),
        P(rookByRank_4, -446),
        P(rookByRank_5, -318),
        P(rookByRank_6, -160), // -rank8

        P(rookOnHalfOpen_0, 151), // fileA
        P(rookOnHalfOpen_1, 57),
        P(rookOnHalfOpen_2, 7),
        P(rookOnHalfOpen_3, -9), // fileD

        P(rookOnOpen_0, 200), // fileA
        P(rookOnOpen_1, 143),
        P(rookOnOpen_2, 93),
        P(rookOnOpen_3, 108), // fileD

        P(rookToWeakPawn_0, 149),
        P(rookToWeakPawn_1, 133),
        P(rookToWeakPawn_2, 113),
        P(rookToWeakPawn_3, 82),

        P(rookToKing_0, 275), // d=0
        P(rookToKing_1, 181), // d=1
        P(rookToKing_2, 127), // d=2
        P(rookToKing_3, 78),
        P(rookToKing_4, 24),
        P(rookToKing_5, -60),
        P(rookToKing_6, -114),
        P(rookToKing_7, -141),

        /*
         *  Queens
         */

        P(queenByFile_0, -36), // fileA
        P(queenByFile_1, -43),
        P(queenByFile_2, -29), // -fileD

        P(queenByRank_0, -93), // rank1
        P(queenByRank_1, -158),
        P(queenByRank_2, -226),
        P(queenByRank_3, -265),
        P(queenByRank_4, -242),
        P(queenByRank_5, -180),
        P(queenByRank_6, -120), // -rank8

        P(queenToKing, -22),
        P(queenToOwnKing, -10),
        P(rookToKing, -13),
        P(rookToOwnKing, 3),
        P(bishopToKing, 0),
        P(bishopToOwnKing, -7),
        P(knightToKing, -14),
        P(knightToOwnKing, -9),

        /*
         *  Kings
         */

        P(kingByFile_0, -42), // fileA
        P(kingByFile_1, -39),
        P(kingByFile_2, -27), // -fileD

        P(kingByRank_0, -263), // rank1
        P(kingByRank_1, -434),
        P(kingByRank_2, -592),
        P(kingByRank_3, -667),
        P(kingByRank_4, -583),
        P(kingByRank_5, -356),
        P(kingByRank_6, -107), // -rank8

        // Drawness. Positive is more drawish. Negative is more sharp
        P(drawOffset, 256),
        P(drawQueen, 111),
        P(drawRook, 4),
        P(drawBishop, -29),
        P(drawKnight, -11),
        P(drawPawn, -842),
        P(drawQueenEnding, -81),
        P(drawRookEnding, 656),
        P(drawKnightEnding, 707),
        P(drawBishopEnding, 1795),
        P(drawPawnEnding, -662),
        P(drawPawnless, 93),

        P(drawQueenImbalance, -836),
        P(drawRookImbalance, 435),
        P(drawMinorImbalance, 15),

        P(drawUnlikeBishops, 2339),
        P(drawUnlikeBishopsAndQueens, 842),
        P(drawUnlikeBishopsAndRooks, 454),
        P(drawUnlikeBishopsAndKnights, 706),

        P(drawRammed_0, 571), // fileA/H
        P(drawRammed_1, 529),
        P(drawRammed_2, 585),
        P(drawRammed_3, 468), // fileD/E
// }

/*----------------------------------------------------------------------+
 |                                                                      |
 +----------------------------------------------------------------------*/

