
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
        P(winBonus, 3346),

        P(castleK, -65),
        P(castleQ, -116),
        P(castleKQ, -24),

        P(queenValue, 6742),
        P(rookValue, 4349),
        P(bishopValue, 2389),
        P(knightValue, 2244),
        P(pawnValue1, 1036), P(pawnValue2, 1697), P(pawnValue3, 2357), P(pawnValue4, 3031),
        P(pawnValue5, 3703), P(pawnValue6, 4339), P(pawnValue7, 4946), P(pawnValue8, 5531),

        P(queenAndQueen, -640),
        P(queenAndRook, -672),
        P(queenAndBishop, 47),
        P(queenAndKnight, -45),
        P(queenAndPawn_1, -115), P(queenAndPawn_2, -72),
        P(rookAndRook, -293),
        P(rookAndBishop, -114),
        P(rookAndKnight, -140),
        P(rookAndPawn_1, -62), P(rookAndPawn_2, -61),
        P(bishopAndBishop, 88),
        P(bishopAndKnight, -23),
        P(bishopAndPawn_1, -56), P(bishopAndPawn_2, -20),
        P(knightAndKnight, -64),
        P(knightAndPawn_1, -27), P(knightAndPawn_2, -3),

        P(queenVsRook, -329),
        P(queenVsBishop, 64),
        P(queenVsKnight, -5),
        P(queenVsPawn_1, 111), P(queenVsPawn_2, -102),
        P(rookVsBishop, -7),
        P(rookVsKnight, 24),
        P(rookVsPawn_1, -16), P(rookVsPawn_2, -41),
        P(bishopVsKnight, 18),
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
        P(safetyAndQueen, 378),
        P(safetyAndRook, 2752),
        P(safetyAndBishop, 1381),
        P(safetyAndKnight, 1438),
        P(safetyAndPawn, -919),
        P(safetyVsQueen, 14667),
        P(safetyVsRook, -2788),
        P(safetyVsBishop, -2004),
        P(safetyVsKnight, -1731),
        P(safetyVsPawn, -793),

        // shelter
        P(shelterPawn_0, 148), // rank3
        P(shelterPawn_1, 277), // rank4
        P(shelterPawn_2, 280), // rank5
        P(shelterPawn_3, 144), // rank6
        P(shelterPawn_4, -105), // rank7
        P(shelterPawn_5, 285), // no pawn

        P(shelterKing_0, -15), // fileA (fileH)
        P(shelterKing_1, -118),
        P(shelterKing_2, -91), // -fileD (-fileE)

        P(shelterWalkingKing, 33), // rank2 or up
        P(shelterCastled, 14), // fraction of 256

        // attacks
        P(attackSquares_0, 89), // 0 attacks
        P(attackSquares_1, 33),
        P(attackSquares_2, -65),
        P(attackSquares_3, -159),
        P(attackSquares_4, -215),
        P(attackSquares_5, -194), // -(6 or more attacks)

        P(attackByPawn_0, 114),
        P(attackByPawn_1, 241),
        P(attackByPawn_2, 57),
        P(attackByMinor_0, -115),
        P(attackByMinor_1, 13),
        P(attackByMinor_2, -300),
        P(attackByRook_0, 158),
        P(attackByRook_1, 476),
        P(attackByRook_2, -413),
        P(attackByQueen, 171),
        P(attackByKing, -217),

        // TODO: pawn storm

        // king mobility
        P(mobilityKing_0, 123),
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
        P(pawnByFile_1, 24),
        P(pawnByFile_2, -15),
        P(pawnByFile_3, -53),
        P(pawnByFile_4, -82),
        P(pawnByFile_5, -43),
        P(pawnByFile_6, 37), // -fileH

        P(pawnByFile_0x, -111), // fileA
        P(pawnByFile_1x, -162),
        P(pawnByFile_2x, -219),
        P(pawnByFile_3x, -278),
        P(pawnByFile_4x, -290),
        P(pawnByFile_5x, -218),
        P(pawnByFile_6x, -84), // -fileH

        // 5 parameters for 6 pawn ranks
        P(pawnByRank_0, -53), // rank2
        P(pawnByRank_1, -89),
        P(pawnByRank_2, -92),
        P(pawnByRank_3, -31),
        P(pawnByRank_4, 67), // -rank7

        P(doubledPawnA, -201), P(doubledPawnB, -121), P(doubledPawnC, -171), P(doubledPawnD, -118),
        P(doubledPawnE, -116), P(doubledPawnF, -60), P(doubledPawnG, -52), P(doubledPawnH, -226),

        P(backwardPawnClosedByRank_0, 9),
        P(backwardPawnClosedByRank_1, 14),
        P(backwardPawnClosedByRank_2, -3),
        P(backwardPawnClosedByRank_3, 5),
        P(backwardPawnClosedByRank_4, 0),

        P(backwardPawnOpenByRank_0, 13),
        P(backwardPawnOpenByRank_1, 12),
        P(backwardPawnOpenByRank_2, 7),
        P(backwardPawnOpenByRank_3, 44),
        P(backwardPawnOpenByRank_4, 0),

        P(backwardPawnClosedA, 46),
        P(backwardPawnClosedB, -2),
        P(backwardPawnClosedC, 17),
        P(backwardPawnClosedD, -39),
        P(backwardPawnClosedE, -61),
        P(backwardPawnClosedF, -20),
        P(backwardPawnClosedG, -4),
        P(backwardPawnClosedH, 50),

        P(backwardPawnOpenA, -12),
        P(backwardPawnOpenB, -65),
        P(backwardPawnOpenC, -60),
        P(backwardPawnOpenD, -74),
        P(backwardPawnOpenE, -76),
        P(backwardPawnOpenF, -78),
        P(backwardPawnOpenG, -71),
        P(backwardPawnOpenH, 7),

        P(rammedWeakPawnA, -17),
        P(rammedWeakPawnB, -9),
        P(rammedWeakPawnC, -13),
        P(rammedWeakPawnD, -8),
        P(rammedWeakPawnE, 2),
        P(rammedWeakPawnF, -15),
        P(rammedWeakPawnG, -27),
        P(rammedWeakPawnH, -59),

        P(isolatedPawnClosedA, 132), P(isolatedPawnClosedB, 25),
        P(isolatedPawnClosedC, 7), P(isolatedPawnClosedD, 10),
        P(isolatedPawnClosedE, -53), P(isolatedPawnClosedF, -85),
        P(isolatedPawnClosedG, -98), P(isolatedPawnClosedH, 0),

        P(isolatedPawnOpenA, 90), P(isolatedPawnOpenB, 45),
        P(isolatedPawnOpenC, -35), P(isolatedPawnOpenD, -89),
        P(isolatedPawnOpenE, -84), P(isolatedPawnOpenF, -46),
        P(isolatedPawnOpenG, -23), P(isolatedPawnOpenH, 38),

        P(sidePawnClosedA, 63), P(sidePawnClosedB, 35),
        P(sidePawnClosedC, 66), P(sidePawnClosedD, -22),
        P(sidePawnClosedE, -29), P(sidePawnClosedF, -57),
        P(sidePawnClosedG, -101), P(sidePawnClosedH, -19),

        P(sidePawnOpenA, 22), P(sidePawnOpenB, 59),
        P(sidePawnOpenC, 14), P(sidePawnOpenD, -37),
        P(sidePawnOpenE, -31), P(sidePawnOpenF, -16),
        P(sidePawnOpenG, 4), P(sidePawnOpenH, 8),

        P(middlePawnClosedA, 0), P(middlePawnClosedB, 2),
        P(middlePawnClosedC, 92), P(middlePawnClosedD, -12),
        P(middlePawnClosedE, -5), P(middlePawnClosedF, -18),
        P(middlePawnClosedG, -80), P(middlePawnClosedH, 0),

        P(middlePawnOpenA, 0), P(middlePawnOpenB, -38),
        P(middlePawnOpenC, 13), P(middlePawnOpenD, 3),
        P(middlePawnOpenE, 8), P(middlePawnOpenF, -40),
        P(middlePawnOpenG, -55), P(middlePawnOpenH, 0),

        P(duoPawnA, 8),
        P(duoPawnB, -9),
        P(duoPawnC, -9),
        P(duoPawnD, 27),
        P(duoPawnE, 26),
        P(duoPawnF, 11),
        P(duoPawnG, 26),
        P(duoPawnH, -58),

        P(openFilePawn_0, -22),
        P(openFilePawn_1, -18),
        P(openFilePawn_2, 32),
        P(openFilePawn_3, 2),
        P(openFilePawn_4, 38),
        P(openFilePawn_5, 201),

        P(stoppedPawn_0, -39),
        P(stoppedPawn_1, 14),
        P(stoppedPawn_2, 12),
        P(stoppedPawn_3, -3),
        P(stoppedPawn_4, 0),
        P(stoppedPawn_5, 0),

        P(duoPawn_0, 2),
        P(duoPawn_1, -10),
        P(duoPawn_2, 30),
        P(duoPawn_3, 145),
        P(duoPawn_4, 519),
        P(duoPawn_5, 1474),

        P(trailingPawn_0, -8),
        P(trailingPawn_1, -53),
        P(trailingPawn_2, -57),
        P(trailingPawn_3, -24),
        P(trailingPawn_4, 7),
        P(trailingPawn_5, -34),

        P(capturePawn_0, 133),
        P(capturePawn_1, -71),
        P(capturePawn_2, -54),
        P(capturePawn_3, 44),
        P(capturePawn_4, 351),
        P(capturePawn_5, 0),

        P(defendedPawn_0, 0),
        P(defendedPawn_1, 57),
        P(defendedPawn_2, 11),
        P(defendedPawn_3, 98),
        P(defendedPawn_4, 205),
        P(defendedPawn_5, 156),

        P(pawnLever_0, 0),
        P(pawnLever_1, 0),
        P(pawnLever_2, -36),
        P(pawnLever_3, -35),
        P(pawnLever_4, -28),
        P(pawnLever_5, -23),
        P(pawnLever_6, 0),

        P(rammedPawn_0, -22),
        P(rammedPawn_1, 23),
        P(rammedPawn_2, -65),
        P(rammedPawn_3, -41),
        P(rammedPawn_4, 8),
        P(rammedPawn_5, 0),

        P(mobilePawn_0, 1),
        P(mobilePawn_1, 8),
        P(mobilePawn_2, 7),
        P(mobilePawn_3, 5),
        P(mobilePawn_4, 12),
        P(mobilePawn_5, 42),

        // Quadratic polynomials for passers
        P(passerA_0, -29), P(passerB_0, 54), P(passerC_0, 108), P(passerD_0, 88),
        P(passerE_0, 223), P(passerF_0, 144), P(passerG_0, 98), P(passerH_0, 78),

        P(passerA_1, 71), P(passerB_1, -11), P(passerC_1, -56), P(passerD_1, -27),
        P(passerE_1, -84), P(passerF_1, -25), P(passerG_1, 48), P(passerH_1, 74),

        P(passerA_2, 147), P(passerB_2, 213), P(passerC_2, 245), P(passerD_2, 199),
        P(passerE_2, 245), P(passerF_2, 215), P(passerG_2, 150), P(passerH_2, 110),

        P(passerScalingOffset, 4169),
        P(passerAndQueen, 568),
        P(passerAndRook, 328),
        P(passerAndBishop, 535),
        P(passerAndKnight, 346),
        P(passerAndPawn, -633),
        P(passerVsQueen, 294),
        P(passerVsRook, -441),
        P(passerVsBishop, -1272),
        P(passerVsKnight, -861),
        P(passerVsPawn, 91),

        P(candidateByRank_0, -8),
        P(candidateByRank_1, 21),
        P(candidateByRank_2, 67),
        P(candidateByRank_3, 171),
        P(candidateByRank_4, 245),

        P(candidateA, 54),
        P(candidateB, -36),
        P(candidateC, -8),
        P(candidateD, -31),
        P(candidateE, 19),
        P(candidateF, 34),
        P(candidateG, 5),
        P(candidateH, 65),
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
        P(knightByFile_1, -219),
        P(knightByFile_2, -232),
        P(knightByFile_3, -164),
        P(knightByFile_4, -100),
        P(knightByFile_5, -23),
        P(knightByFile_6, 25), // -fileH

        // Opposite side kings
        P(knightByFile_0x, -120), // fileA
        P(knightByFile_1x, -154),
        P(knightByFile_2x, -125),
        P(knightByFile_3x, -34),
        P(knightByFile_4x, 50),
        P(knightByFile_5x, 134),
        P(knightByFile_6x, 116), // -fileH

        P(knightByRank_0, -124), // rank1
        P(knightByRank_1, -204),
        P(knightByRank_2, -223),
        P(knightByRank_3, -160),
        P(knightByRank_4, -49),
        P(knightByRank_5, 101),
        P(knightByRank_6, 162), // -rank8

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
       P(bishopBySquare_0, -179),
        P(bishopBySquare_1, 2),
        P(bishopBySquare_2, -78),
        P(bishopBySquare_3, 87),
        P(bishopBySquare_4, -56),
        P(bishopBySquare_5, 1),
        P(bishopBySquare_6, -57),
        P(bishopBySquare_7, 44),

        P(bishopBySquare_8, -42),
        P(bishopBySquare_9, -50),
        P(bishopBySquare_10, 82),
        P(bishopBySquare_11, -20),
        P(bishopBySquare_12, 33),
        P(bishopBySquare_13, 105),
        P(bishopBySquare_14, 79),
        P(bishopBySquare_15, 112),

        P(bishopBySquare_16, -125),
        P(bishopBySquare_17, 46),
        P(bishopBySquare_18, 20),
        P(bishopBySquare_19, 52),
        P(bishopBySquare_20, 40),
        P(bishopBySquare_21, 86),
        P(bishopBySquare_22, 68),
        P(bishopBySquare_23, 110),

        P(bishopBySquare_24, -87),
        P(bishopBySquare_25, -58),
        P(bishopBySquare_26, 55),
        P(bishopBySquare_27, 90),
        P(bishopBySquare_28, 143),
        P(bishopBySquare_29, 86),
        P(bishopBySquare_30, 101),
        P(bishopBySquare_31, 71),

        P(bishopBySquare_32, -81),
        P(bishopBySquare_33, 18),
        P(bishopBySquare_34, 9),
        P(bishopBySquare_35, 83),
        P(bishopBySquare_36, 89),
        P(bishopBySquare_37, 123),
        P(bishopBySquare_38, 20),
        P(bishopBySquare_39, 74),

        P(bishopBySquare_40, -79),
        P(bishopBySquare_41, 3),
        P(bishopBySquare_42, 54),
        P(bishopBySquare_43, -15),
        P(bishopBySquare_44, 111),
        P(bishopBySquare_45, 226),
        P(bishopBySquare_46, 43),
        P(bishopBySquare_47, -6),

        P(bishopBySquare_48, -53),
        P(bishopBySquare_49, 47),
        P(bishopBySquare_50, 33),
        P(bishopBySquare_51, 47),
        P(bishopBySquare_52, -13),
        P(bishopBySquare_53, 150),
        P(bishopBySquare_54, -59),
        P(bishopBySquare_55, 13),

        P(bishopBySquare_56, -213),
        P(bishopBySquare_57, -58),
        P(bishopBySquare_58, 6),
        P(bishopBySquare_59, 29),
        P(bishopBySquare_60, -4),
        P(bishopBySquare_61, 129),
        P(bishopBySquare_62, -233),
        P(bishopBySquare_63, -81),

        P(bishopBySquare_0x, -111),
        P(bishopBySquare_1x, -28),
        P(bishopBySquare_2x, 25),
        P(bishopBySquare_3x, -10),
        P(bishopBySquare_4x, 3),
        P(bishopBySquare_5x, 110),
        P(bishopBySquare_6x, -211),
        P(bishopBySquare_7x, -84),

        P(bishopBySquare_8x, -83),
        P(bishopBySquare_9x, -76),
        P(bishopBySquare_10x, 10),
        P(bishopBySquare_11x, 70),
        P(bishopBySquare_12x, 45),
        P(bishopBySquare_13x, 135),
        P(bishopBySquare_14x, 33),
        P(bishopBySquare_15x, -2),

        P(bishopBySquare_16x, -117),
        P(bishopBySquare_17x, -5),
        P(bishopBySquare_18x, 60),
        P(bishopBySquare_19x, 61),
        P(bishopBySquare_20x, 137),
        P(bishopBySquare_21x, 151),
        P(bishopBySquare_22x, 83),
        P(bishopBySquare_23x, 29),

        P(bishopBySquare_24x, -78),
        P(bishopBySquare_25x, -4),
        P(bishopBySquare_26x, 76),
        P(bishopBySquare_27x, 95),
        P(bishopBySquare_28x, 140),
        P(bishopBySquare_29x, 175),
        P(bishopBySquare_30x, 92),
        P(bishopBySquare_31x, 123),

        P(bishopBySquare_32x, -56),
        P(bishopBySquare_33x, 76),
        P(bishopBySquare_34x, 93),
        P(bishopBySquare_35x, 146),
        P(bishopBySquare_36x, 175),
        P(bishopBySquare_37x, 162),
        P(bishopBySquare_38x, 173),
        P(bishopBySquare_39x, 106),

        P(bishopBySquare_40x, -86),
        P(bishopBySquare_41x, 73),
        P(bishopBySquare_42x, 127),
        P(bishopBySquare_43x, 153),
        P(bishopBySquare_44x, 149),
        P(bishopBySquare_45x, 164),
        P(bishopBySquare_46x, 153),
        P(bishopBySquare_47x, 153),

        P(bishopBySquare_48x, -6),
        P(bishopBySquare_49x, 135),
        P(bishopBySquare_50x, 128),
        P(bishopBySquare_51x, 63),
        P(bishopBySquare_52x, 130),
        P(bishopBySquare_53x, 172),
        P(bishopBySquare_54x, 163),
        P(bishopBySquare_55x, 155),

        P(bishopBySquare_56x, -89),
        P(bishopBySquare_57x, 54),
        P(bishopBySquare_58x, 40),
        P(bishopBySquare_59x, 78),
        P(bishopBySquare_60x, 70),
        P(bishopBySquare_61x, 58),
        P(bishopBySquare_62x, -19),
        P(bishopBySquare_63x, -25),
#endif

        /*
         *  Rooks
         */

        P(rookByFile_0, -63), // fileA
        P(rookByFile_1, -83),
        P(rookByFile_2, -57),
        P(rookByFile_3, -10),
        P(rookByFile_4, 38),
        P(rookByFile_5, 55),
        P(rookByFile_6, 109), // -fileH

        P(rookByFile_0x, -85), // fileA
        P(rookByFile_1x, -72),
        P(rookByFile_2x, -78),
        P(rookByFile_3x, -61),
        P(rookByFile_4x, -6),
        P(rookByFile_5x, 38),
        P(rookByFile_6x, 66), // -fileH

        P(rookByRank_0, -138), // rank1
        P(rookByRank_1, -318),
        P(rookByRank_2, -463),
        P(rookByRank_3, -521),
        P(rookByRank_4, -435),
        P(rookByRank_5, -283),
        P(rookByRank_6, -126), // -rank8

        /*
         *  Queens
         */

        P(queenByFile_0, -109), // fileA
        P(queenByFile_1, -169),
        P(queenByFile_2, -171),
        P(queenByFile_3, -147),
        P(queenByFile_4, -112),
        P(queenByFile_5, -77),
        P(queenByFile_6, -29), // -fileH

        P(queenByFile_0x, -7), // fileA
        P(queenByFile_1x, -9),
        P(queenByFile_2x, 15),
        P(queenByFile_3x, 62),
        P(queenByFile_4x, 127),
        P(queenByFile_5x, 171),
        P(queenByFile_6x, 127), // -fileH

        P(queenByRank_0, -151), // rank1
        P(queenByRank_1, -246),
        P(queenByRank_2, -319),
        P(queenByRank_3, -352),
        P(queenByRank_4, -310),
        P(queenByRank_5, -207),
        P(queenByRank_6, -117), // -rank8

        /*
         *  Kings
         */

        P(kingByFile_0, 40), // fileD
        P(kingByFile_1, 53),
        P(kingByFile_2, 46), // -fileH

        P(kingByRank_0, -223), // rank1
        P(kingByRank_1, -371),
        P(kingByRank_2, -491),
        P(kingByRank_3, -512),
        P(kingByRank_4, -378),
        P(kingByRank_5, -131),
        P(kingByRank_6, 75), // -rank8

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
        P(drawOffset, 195),
        P(drawQueen, -908),
        P(drawRook, 93),
        P(drawBishop, -23),
        P(drawKnight, 8),
        P(drawPawn, -751),
        P(drawQueenEnding, 526),
        P(drawRookEnding, 423),
        P(drawKnightEnding, 502),
        P(drawBishopEnding, 1392),
        P(drawPawnEnding, -18751),
        P(drawPawnless, 169),

        P(drawQueenImbalance, -10),
        P(drawRookImbalance, 338),
        P(drawMinorImbalance, 32),

        P(drawUnlikeBishops, 3639),
        P(drawUnlikeBishopsAndQueens, 334),
        P(drawUnlikeBishopsAndRooks, 399),
        P(drawUnlikeBishopsAndKnights, 886),
        // TODO: pawn and pawn block square colors


// }

/*----------------------------------------------------------------------+
 |                                                                      |
 +----------------------------------------------------------------------*/

