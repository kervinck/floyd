
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
        P(tempo, 38),
        P(winBonus, 3429),

        P(castleK, -67),
        P(castleQ, -112),
        P(castleKQ, -22),

        P(queenValue, 6899),
        P(rookValue, 4312),
        P(bishopValue, 2372),
        P(knightValue, 2211),
        P(pawnValue1, 1033), P(pawnValue2, 1700), P(pawnValue3, 2353), P(pawnValue4, 3027),
        P(pawnValue5, 3697), P(pawnValue6, 4342), P(pawnValue7, 4958), P(pawnValue8, 5549),

        P(queenAndQueen, -745),
        P(queenAndRook, -677),
        P(queenAndBishop, 42),
        P(queenAndKnight, -61),
        P(queenAndPawn_1, -119), P(queenAndPawn_2, -72),
        P(rookAndRook, -309),
        P(rookAndBishop, -126),
        P(rookAndKnight, -156),
        P(rookAndPawn_1, -63), P(rookAndPawn_2, -61),
        P(bishopAndBishop, 73),
        P(bishopAndKnight, -29),
        P(bishopAndPawn_1, -56), P(bishopAndPawn_2, -20),
        P(knightAndKnight, -71),
        P(knightAndPawn_1, -27), P(knightAndPawn_2, -3),

        P(queenVsRook, -317),
        P(queenVsBishop, 79),
        P(queenVsKnight, -14),
        P(queenVsPawn_1, 96), P(queenVsPawn_2, -106),
        P(rookVsBishop, 2),
        P(rookVsKnight, 25),
        P(rookVsPawn_1, -19), P(rookVsPawn_2, -42),
        P(bishopVsKnight, 10),
        P(bishopVsPawn_1, -11), P(bishopVsPawn_2, -10),
        P(knightVsPawn_1, -4), P(knightVsPawn_2, -20),

        P(controlCenter, 30),
        P(controlExtendedCenter, 37),
        P(controlOutside, 27),

        /*
         *  king safety
         */

        // scaling
        P(safetyScalingOffset, -3714),
        P(safetyAndQueen, 831),
        P(safetyAndRook, 2190),
        P(safetyAndBishop, 822),
        P(safetyAndKnight, 796),
        P(safetyAndPawn, -1205),
        P(safetyVsQueen, 15643),
        P(safetyVsRook, -1853),
        P(safetyVsBishop, -1514),
        P(safetyVsKnight, -1157),
        P(safetyVsPawn, -809),

        // shelter
        P(shelterPawn_0, 142), // rank3
        P(shelterPawn_1, 272), // rank4
        P(shelterPawn_2, 273), // rank5
        P(shelterPawn_3, 135), // rank6
        P(shelterPawn_4, -84), // rank7
        P(shelterPawn_5, 276), // no pawn

        P(shelterKing_0, 54), // fileA (fileH)
        P(shelterKing_1, -68),
        P(shelterKing_2, -66), // -fileD (-fileE)

        P(shelterWalkingKing, 31), // rank2 or up
        P(shelterCastled, 29), // fraction of 256

        // attacks
        P(attackSquares_0, 92), // 0 attacks
        P(attackSquares_1, 36),
        P(attackSquares_2, -59),
        P(attackSquares_3, -157),
        P(attackSquares_4, -215),
        P(attackSquares_5, -198), // -(6 or more attacks)

        P(attackByPawn_0, 80),
        P(attackByPawn_1, 181),
        P(attackByPawn_2, 28),
        P(attackByMinor_0, -114),
        P(attackByMinor_1, 13),
        P(attackByMinor_2, -298),
        P(attackByRook_0, 17),
        P(attackByRook_1, 199),
        P(attackByRook_2, -326),
        P(attackByQueen, 168),
        P(attackByKing, -214),

        // TODO: pawn storm

        // king mobility
        P(mobilityKing_0, 113),
        P(mobilityKing_1, 61),

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

        P(pawnByFile_0, 1), // fileA
        P(pawnByFile_1, 15),
        P(pawnByFile_2, -32),
        P(pawnByFile_3, -69),
        P(pawnByFile_4, -101),
        P(pawnByFile_5, -49),
        P(pawnByFile_6, 36), // -fileH

        P(pawnByFile_0x, -89), // fileA
        P(pawnByFile_1x, -128),
        P(pawnByFile_2x, -178),
        P(pawnByFile_3x, -233),
        P(pawnByFile_4x, -238),
        P(pawnByFile_5x, -178),
        P(pawnByFile_6x, -65), // -fileH

        // 5 parameters for 6 pawn ranks
        P(pawnByRank_0, -54), // rank2
        P(pawnByRank_1, -89),
        P(pawnByRank_2, -91),
        P(pawnByRank_3, -33),
        P(pawnByRank_4, 68), // -rank7

        P(doubledPawnA, -214), P(doubledPawnB, -128), P(doubledPawnC, -181), P(doubledPawnD, -124),
        P(doubledPawnE, -124), P(doubledPawnF, -72), P(doubledPawnG, -58), P(doubledPawnH, -236),

        P(backwardPawnClosedByRank_0, 0),
        P(backwardPawnClosedByRank_1, 0),
        P(backwardPawnClosedByRank_2, 0),
        P(backwardPawnClosedByRank_3, 0),
        P(backwardPawnClosedByRank_4, 0),

        P(backwardPawnOpenByRank_0, 0),
        P(backwardPawnOpenByRank_1, 0),
        P(backwardPawnOpenByRank_2, 0),
        P(backwardPawnOpenByRank_3, 0),
        P(backwardPawnOpenByRank_4, 0),

        P(backwardPawnClosedA, 43),
        P(backwardPawnClosedB, 2),
        P(backwardPawnClosedC, 17),
        P(backwardPawnClosedD, -46),
        P(backwardPawnClosedE, -62),
        P(backwardPawnClosedF, -17),
        P(backwardPawnClosedG, 1),
        P(backwardPawnClosedH, 50),

        P(backwardPawnOpenA, -13),
        P(backwardPawnOpenB, -68),
        P(backwardPawnOpenC, -60),
        P(backwardPawnOpenD, -75),
        P(backwardPawnOpenE, -78),
        P(backwardPawnOpenF, -77),
        P(backwardPawnOpenG, -68),
        P(backwardPawnOpenH, 9),

        P(rammedWeakPawnA, -11),
        P(rammedWeakPawnB, -9),
        P(rammedWeakPawnC, -17),
        P(rammedWeakPawnD, -7),
        P(rammedWeakPawnE, 7),
        P(rammedWeakPawnF, -19),
        P(rammedWeakPawnG, -30),
        P(rammedWeakPawnH, -60),

        P(isolatedPawnClosedA, 102), P(isolatedPawnClosedB, 4),
        P(isolatedPawnClosedC, -1), P(isolatedPawnClosedD, 12),
        P(isolatedPawnClosedE, -54), P(isolatedPawnClosedF, -81),
        P(isolatedPawnClosedG, -87), P(isolatedPawnClosedH, 3),

        P(isolatedPawnOpenA, 78), P(isolatedPawnOpenB, 40),
        P(isolatedPawnOpenC, -32), P(isolatedPawnOpenD, -99),
        P(isolatedPawnOpenE, -81), P(isolatedPawnOpenF, -47),
        P(isolatedPawnOpenG, -24), P(isolatedPawnOpenH, 36),

        P(sidePawnClosedA, 37), P(sidePawnClosedB, 32),
        P(sidePawnClosedC, 60), P(sidePawnClosedD, -22),
        P(sidePawnClosedE, -24), P(sidePawnClosedF, -48),
        P(sidePawnClosedG, -86), P(sidePawnClosedH, -3),

        P(sidePawnOpenA, 17), P(sidePawnOpenB, 53),
        P(sidePawnOpenC, 12), P(sidePawnOpenD, -40),
        P(sidePawnOpenE, -26), P(sidePawnOpenF, -11),
        P(sidePawnOpenG, 3), P(sidePawnOpenH, 16),

        P(middlePawnClosedA, 0), P(middlePawnClosedB, 2),
        P(middlePawnClosedC, 83), P(middlePawnClosedD, -16),
        P(middlePawnClosedE, -7), P(middlePawnClosedF, -13),
        P(middlePawnClosedG, -74), P(middlePawnClosedH, 0),

        P(middlePawnOpenA, 0), P(middlePawnOpenB, -35),
        P(middlePawnOpenC, 15), P(middlePawnOpenD, -1),
        P(middlePawnOpenE, 11), P(middlePawnOpenF, -32),
        P(middlePawnOpenG, -68), P(middlePawnOpenH, 0),

        P(duoPawnA, 7),
        P(duoPawnB, -2),
        P(duoPawnC, -7),
        P(duoPawnD, 28),
        P(duoPawnE, 29),
        P(duoPawnF, 11),
        P(duoPawnG, 33),
        P(duoPawnH, -60),

        P(openFilePawn_0, -27),
        P(openFilePawn_1, -17),
        P(openFilePawn_2, 35),
        P(openFilePawn_3, -1),
        P(openFilePawn_4, 22),
        P(openFilePawn_5, 196),

        P(stoppedPawn_0, -38),
        P(stoppedPawn_1, 14),
        P(stoppedPawn_2, 4),
        P(stoppedPawn_3, -1),
        P(stoppedPawn_4, 0),
        P(stoppedPawn_5, 0),

        P(duoPawn_0, -1),
        P(duoPawn_1, -11),
        P(duoPawn_2, 36),
        P(duoPawn_3, 169),
        P(duoPawn_4, 510),
        P(duoPawn_5, 1440),

        P(trailingPawn_0, -5),
        P(trailingPawn_1, -46),
        P(trailingPawn_2, -57),
        P(trailingPawn_3, -14),
        P(trailingPawn_4, 8),
        P(trailingPawn_5, -28),

        P(capturePawn_0, 110),
        P(capturePawn_1, -97),
        P(capturePawn_2, -52),
        P(capturePawn_3, 38),
        P(capturePawn_4, 446),
        P(capturePawn_5, 0),

        P(defendedPawn_0, 0),
        P(defendedPawn_1, 53),
        P(defendedPawn_2, 9),
        P(defendedPawn_3, 90),
        P(defendedPawn_4, 201),
        P(defendedPawn_5, 148),

        P(pawnLever_0, 0),
        P(pawnLever_1, 0),
        P(pawnLever_2, -33),
        P(pawnLever_3, -30),
        P(pawnLever_4, -26),
        P(pawnLever_5, -22),
        P(pawnLever_6, 0),

        P(rammedPawn_0, -14),
        P(rammedPawn_1, 26),
        P(rammedPawn_2, -80),
        P(rammedPawn_3, -43),
        P(rammedPawn_4, 8),
        P(rammedPawn_5, 0),

        P(mobilePawn_0, 0),
        P(mobilePawn_1, 0),
        P(mobilePawn_2, 0),
        P(mobilePawn_3, 0),
        P(mobilePawn_4, 0),
        P(mobilePawn_5, 0),

        // Quadratic polynomials for passers
        P(passerA_0, -33), P(passerB_0, 47), P(passerC_0, 79), P(passerD_0, 67),
        P(passerE_0, 193), P(passerF_0, 126), P(passerG_0, 90), P(passerH_0, 90),

        P(passerA_1, 71), P(passerB_1, -10), P(passerC_1, -46), P(passerD_1, -22),
        P(passerE_1, -78), P(passerF_1, -38), P(passerG_1, 37), P(passerH_1, 46),

        P(passerA_2, 150), P(passerB_2, 218), P(passerC_2, 243), P(passerD_2, 203),
        P(passerE_2, 245), P(passerF_2, 234), P(passerG_2, 166), P(passerH_2, 144),

        P(passerScalingOffset, 4218),
        P(passerAndQueen, 175),
        P(passerAndRook, 154),
        P(passerAndBishop, 524),
        P(passerAndKnight, 303),
        P(passerAndPawn, -646),
        P(passerVsQueen, 662),
        P(passerVsRook, -258),
        P(passerVsBishop, -1151),
        P(passerVsKnight, -755),
        P(passerVsPawn, 84),

        P(candidateByRank_0, 0),
        P(candidateByRank_1, 0),
        P(candidateByRank_2, 0),
        P(candidateByRank_3, 0),
        P(candidateByRank_4, 0),

        P(candidateA, 0),
        P(candidateB, 0),
        P(candidateC, 0),
        P(candidateD, 0),
        P(candidateE, 0),
        P(candidateF, 0),
        P(candidateG, 0),
        P(candidateH, 0),
#if 0
        P(drawBlockedA, 0), P(drawBlockedB, 0), P(drawBlockedC, 0), P(drawBlockedD, 0),
        P(drawBlockedE, 0), P(drawBlockedF, 0), P(drawBlockedG, 0), P(drawBlockedH, 0),

        P(drawPasser_0, 0), P(drawPasser_1, 0), P(drawPasser_2, 0),
#endif

        /*
         *  Knights
         */

        // Kings on same side
        P(knightByFile_0, -184), // fileA
        P(knightByFile_1, -221),
        P(knightByFile_2, -233),
        P(knightByFile_3, -166),
        P(knightByFile_4, -104),
        P(knightByFile_5, -27),
        P(knightByFile_6, 23), // -fileH

        // Opposite side kings
        P(knightByFile_0x, -116), // fileA
        P(knightByFile_1x, -146),
        P(knightByFile_2x, -124),
        P(knightByFile_3x, -33),
        P(knightByFile_4x, 54),
        P(knightByFile_5x, 139),
        P(knightByFile_6x, 121), // -fileH

        P(knightByRank_0, -124), // rank1
        P(knightByRank_1, -201),
        P(knightByRank_2, -221),
        P(knightByRank_3, -158),
        P(knightByRank_4, -48),
        P(knightByRank_5, 100),
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
       P(bishopBySquare_0, -182),
        P(bishopBySquare_1, -4),
        P(bishopBySquare_2, -83),
        P(bishopBySquare_3, 80),
        P(bishopBySquare_4, -62),
        P(bishopBySquare_5, -1),
        P(bishopBySquare_6, -52),
        P(bishopBySquare_7, 46),

        P(bishopBySquare_8, -44),
        P(bishopBySquare_9, -54),
        P(bishopBySquare_10, 77),
        P(bishopBySquare_11, -22),
        P(bishopBySquare_12, 27),
        P(bishopBySquare_13, 101),
        P(bishopBySquare_14, 73),
        P(bishopBySquare_15, 112),

        P(bishopBySquare_16, -129),
        P(bishopBySquare_17, 40),
        P(bishopBySquare_18, 16),
        P(bishopBySquare_19, 48),
        P(bishopBySquare_20, 34),
        P(bishopBySquare_21, 80),
        P(bishopBySquare_22, 64),
        P(bishopBySquare_23, 106),

        P(bishopBySquare_24, -89),
        P(bishopBySquare_25, -60),
        P(bishopBySquare_26, 51),
        P(bishopBySquare_27, 85),
        P(bishopBySquare_28, 137),
        P(bishopBySquare_29, 85),
        P(bishopBySquare_30, 96),
        P(bishopBySquare_31, 68),

        P(bishopBySquare_32, -83),
        P(bishopBySquare_33, 11),
        P(bishopBySquare_34, 4),
        P(bishopBySquare_35, 78),
        P(bishopBySquare_36, 83),
        P(bishopBySquare_37, 121),
        P(bishopBySquare_38, 15),
        P(bishopBySquare_39, 71),

        P(bishopBySquare_40, -84),
        P(bishopBySquare_41, -3),
        P(bishopBySquare_42, 48),
        P(bishopBySquare_43, -22),
        P(bishopBySquare_44, 105),
        P(bishopBySquare_45, 218),
        P(bishopBySquare_46, 38),
        P(bishopBySquare_47, -14),

        P(bishopBySquare_48, -56),
        P(bishopBySquare_49, 43),
        P(bishopBySquare_50, 30),
        P(bishopBySquare_51, 43),
        P(bishopBySquare_52, -17),
        P(bishopBySquare_53, 147),
        P(bishopBySquare_54, -61),
        P(bishopBySquare_55, 9),

        P(bishopBySquare_56, -210),
        P(bishopBySquare_57, -69),
        P(bishopBySquare_58, 2),
        P(bishopBySquare_59, 25),
        P(bishopBySquare_60, -7),
        P(bishopBySquare_61, 130),
        P(bishopBySquare_62, -235),
        P(bishopBySquare_63, -75),

        P(bishopBySquare_0x, -103),
        P(bishopBySquare_1x, -32),
        P(bishopBySquare_2x, 26),
        P(bishopBySquare_3x, -12),
        P(bishopBySquare_4x, 4),
        P(bishopBySquare_5x, 112),
        P(bishopBySquare_6x, -213),
        P(bishopBySquare_7x, -77),

        P(bishopBySquare_8x, -85),
        P(bishopBySquare_9x, -75),
        P(bishopBySquare_10x, 6),
        P(bishopBySquare_11x, 71),
        P(bishopBySquare_12x, 46),
        P(bishopBySquare_13x, 136),
        P(bishopBySquare_14x, 37),
        P(bishopBySquare_15x, 1),

        P(bishopBySquare_16x, -118),
        P(bishopBySquare_17x, -3),
        P(bishopBySquare_18x, 59),
        P(bishopBySquare_19x, 59),
        P(bishopBySquare_20x, 132),
        P(bishopBySquare_21x, 145),
        P(bishopBySquare_22x, 83),
        P(bishopBySquare_23x, 27),

        P(bishopBySquare_24x, -75),
        P(bishopBySquare_25x, -7),
        P(bishopBySquare_26x, 78),
        P(bishopBySquare_27x, 93),
        P(bishopBySquare_28x, 139),
        P(bishopBySquare_29x, 175),
        P(bishopBySquare_30x, 89),
        P(bishopBySquare_31x, 118),

        P(bishopBySquare_32x, -56),
        P(bishopBySquare_33x, 80),
        P(bishopBySquare_34x, 93),
        P(bishopBySquare_35x, 145),
        P(bishopBySquare_36x, 172),
        P(bishopBySquare_37x, 157),
        P(bishopBySquare_38x, 167),
        P(bishopBySquare_39x, 101),

        P(bishopBySquare_40x, -79),
        P(bishopBySquare_41x, 72),
        P(bishopBySquare_42x, 124),
        P(bishopBySquare_43x, 149),
        P(bishopBySquare_44x, 144),
        P(bishopBySquare_45x, 158),
        P(bishopBySquare_46x, 147),
        P(bishopBySquare_47x, 150),

        P(bishopBySquare_48x, -6),
        P(bishopBySquare_49x, 134),
        P(bishopBySquare_50x, 122),
        P(bishopBySquare_51x, 62),
        P(bishopBySquare_52x, 124),
        P(bishopBySquare_53x, 173),
        P(bishopBySquare_54x, 162),
        P(bishopBySquare_55x, 153),

        P(bishopBySquare_56x, -89),
        P(bishopBySquare_57x, 58),
        P(bishopBySquare_58x, 42),
        P(bishopBySquare_59x, 76),
        P(bishopBySquare_60x, 72),
        P(bishopBySquare_61x, 56),
        P(bishopBySquare_62x, -11),
        P(bishopBySquare_63x, -31),
#endif

        /*
         *  Rooks
         */

        P(rookByFile_0, -64), // fileA
        P(rookByFile_1, -85),
        P(rookByFile_2, -59),
        P(rookByFile_3, -12),
        P(rookByFile_4, 36),
        P(rookByFile_5, 53),
        P(rookByFile_6, 108), // -fileH

        P(rookByFile_0x, -85), // fileA
        P(rookByFile_1x, -69),
        P(rookByFile_2x, -71),
        P(rookByFile_3x, -53),
        P(rookByFile_4x, 2),
        P(rookByFile_5x, 45),
        P(rookByFile_6x, 69), // -fileH

        P(rookByRank_0, -138), // rank1
        P(rookByRank_1, -319),
        P(rookByRank_2, -465),
        P(rookByRank_3, -523),
        P(rookByRank_4, -437),
        P(rookByRank_5, -283),
        P(rookByRank_6, -126), // -rank8

        /*
         *  Queens
         */

        P(queenByFile_0, -111), // fileA
        P(queenByFile_1, -169),
        P(queenByFile_2, -173),
        P(queenByFile_3, -147),
        P(queenByFile_4, -114),
        P(queenByFile_5, -77),
        P(queenByFile_6, -28), // -fileH

        P(queenByFile_0x, -13), // fileA
        P(queenByFile_1x, -14),
        P(queenByFile_2x, 8),
        P(queenByFile_3x, 57),
        P(queenByFile_4x, 125),
        P(queenByFile_5x, 171),
        P(queenByFile_6x, 127), // -fileH

        P(queenByRank_0, -159), // rank1
        P(queenByRank_1, -259),
        P(queenByRank_2, -341),
        P(queenByRank_3, -379),
        P(queenByRank_4, -338),
        P(queenByRank_5, -230),
        P(queenByRank_6, -136), // -rank8

        /*
         *  Kings
         */

        P(kingByFile_0, 37), // fileD
        P(kingByFile_1, 52),
        P(kingByFile_2, 45), // -fileH

        P(kingByRank_0, -211), // rank1
        P(kingByRank_1, -353),
        P(kingByRank_2, -463),
        P(kingByRank_3, -470),
        P(kingByRank_4, -329),
        P(kingByRank_5, -75),
        P(kingByRank_6, 115), // -rank8

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
        P(drawOffset, 226),
        P(drawQueen, -1163),
        P(drawRook, 104),
        P(drawBishop, -30),
        P(drawKnight, -20),
        P(drawPawn, -767),
        P(drawQueenEnding, 863),
        P(drawRookEnding, 389),
        P(drawKnightEnding, 510),
        P(drawBishopEnding, 1404),
        P(drawPawnEnding, -17501),
        P(drawPawnless, 208),

        P(drawQueenImbalance, 297),
        P(drawRookImbalance, 328),
        P(drawMinorImbalance, 25),

        P(drawUnlikeBishops, 3710),
        P(drawUnlikeBishopsAndQueens, -147),
        P(drawUnlikeBishopsAndRooks, 407),
        P(drawUnlikeBishopsAndKnights, 901),
        // TODO: pawn and pawn block square colors


// }

/*----------------------------------------------------------------------+
 |                                                                      |
 +----------------------------------------------------------------------*/

