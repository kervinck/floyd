
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
        P(winBonus, 3261),

        P(castleK, -68),
        P(castleQ, -118),
        P(castleKQ, -23),

        P(queenValue, 6712),
        P(rookValue, 4403),
        P(bishopValue, 2415),
        P(knightValue, 2271),
        P(pawnValue1, 1034), P(pawnValue2, 1696), P(pawnValue3, 2359), P(pawnValue4, 3037),
        P(pawnValue5, 3706), P(pawnValue6, 4341), P(pawnValue7, 4939), P(pawnValue8, 5509),

        P(queenAndQueen, -631),
        P(queenAndRook, -658),
        P(queenAndBishop, 54),
        P(queenAndKnight, -32),
        P(queenAndPawn_1, -115), P(queenAndPawn_2, -72),
        P(rookAndRook, -287),
        P(rookAndBishop, -113),
        P(rookAndKnight, -139),
        P(rookAndPawn_1, -58), P(rookAndPawn_2, -61),
        P(bishopAndBishop, 97),
        P(bishopAndKnight, -8),
        P(bishopAndPawn_1, -56), P(bishopAndPawn_2, -20),
        P(knightAndKnight, -55),
        P(knightAndPawn_1, -27), P(knightAndPawn_2, -3),

        P(queenVsRook, -324),
        P(queenVsBishop, 64),
        P(queenVsKnight, -7),
        P(queenVsPawn_1, 115), P(queenVsPawn_2, -105),
        P(rookVsBishop, -16),
        P(rookVsKnight, 9),
        P(rookVsPawn_1, -15), P(rookVsPawn_2, -41),
        P(bishopVsKnight, 20),
        P(bishopVsPawn_1, -11), P(bishopVsPawn_2, -13),
        P(knightVsPawn_1, -4), P(knightVsPawn_2, -21),

        P(controlCenter, 29),
        P(controlExtendedCenter, 37),
        P(controlOutside, 26),

        /*
         *  king safety
         */

        // scaling
        P(safetyScalingOffset, -3279),
        P(safetyAndQueen, -164),
        P(safetyAndRook, 2970),
        P(safetyAndBishop, 1670),
        P(safetyAndKnight, 1679),
        P(safetyAndPawn, -741),
        P(safetyVsQueen, 14288),
        P(safetyVsRook, -3241),
        P(safetyVsBishop, -2287),
        P(safetyVsKnight, -2126),
        P(safetyVsPawn, -784),

        // shelter
        P(shelterPawn_0, 150), // rank3
        P(shelterPawn_1, 277), // rank4
        P(shelterPawn_2, 274), // rank5
        P(shelterPawn_3, 144), // rank6
        P(shelterPawn_4, -140), // rank7
        P(shelterPawn_5, 291), // no pawn

        P(shelterKing_0, -86), // fileA (fileH)
        P(shelterKing_1, -167),
        P(shelterKing_2, -114), // -fileD (-fileE)

        P(shelterWalkingKing, 36), // rank2 or up
        P(shelterCastled, 15), // fraction of 256

        // attacks
        P(attackSquares_0, 90), // 0 attacks
        P(attackSquares_1, 33),
        P(attackSquares_2, -65),
        P(attackSquares_3, -162),
        P(attackSquares_4, -218),
        P(attackSquares_5, -198), // -(6 or more attacks)

        P(attackByPawn_0, 128),
        P(attackByPawn_1, 266),
        P(attackByPawn_2, 71),
        P(attackByMinor_0, -115),
        P(attackByMinor_1, 14),
        P(attackByMinor_2, -296),
        P(attackByRook_0, 254),
        P(attackByRook_1, 677),
        P(attackByRook_2, -493),
        P(attackByQueen, 177),
        P(attackByKing, -210),

        // TODO: pawn storm

        // king mobility
        P(mobilityKing_0, 128),
        P(mobilityKing_1, 70),

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

        P(pawnByFile_0, 6), // fileA
        P(pawnByFile_1, 34),
        P(pawnByFile_2, -3),
        P(pawnByFile_3, -40),
        P(pawnByFile_4, -71),
        P(pawnByFile_5, -36),
        P(pawnByFile_6, 39), // -fileH

        P(pawnByFile_0x, -123), // fileA
        P(pawnByFile_1x, -183),
        P(pawnByFile_2x, -248),
        P(pawnByFile_3x, -311),
        P(pawnByFile_4x, -320),
        P(pawnByFile_5x, -246),
        P(pawnByFile_6x, -97), // -fileH

        // 5 parameters for 6 pawn ranks
        P(pawnByRank_0, -53), // rank2
        P(pawnByRank_1, -90),
        P(pawnByRank_2, -92),
        P(pawnByRank_3, -31),
        P(pawnByRank_4, 70), // -rank7

        P(doubledPawnA, -189), P(doubledPawnB, -112), P(doubledPawnC, -164), P(doubledPawnD, -109),
        P(doubledPawnE, -107), P(doubledPawnF, -58), P(doubledPawnG, -46), P(doubledPawnH, -217),

        P(backwardPawnClosedByRank_0, 13),
        P(backwardPawnClosedByRank_1, 16),
        P(backwardPawnClosedByRank_2, -6),
        P(backwardPawnClosedByRank_3, -2),
        P(backwardPawnClosedByRank_4, 0),

        P(backwardPawnOpenByRank_0, 17),
        P(backwardPawnOpenByRank_1, 13),
        P(backwardPawnOpenByRank_2, 7),
        P(backwardPawnOpenByRank_3, 43),
        P(backwardPawnOpenByRank_4, 0),

        P(backwardPawnClosedA, 50),
        P(backwardPawnClosedB, -2),
        P(backwardPawnClosedC, 16),
        P(backwardPawnClosedD, -39),
        P(backwardPawnClosedE, -58),
        P(backwardPawnClosedF, -21),
        P(backwardPawnClosedG, -9),
        P(backwardPawnClosedH, 47),

        P(backwardPawnOpenA, -12),
        P(backwardPawnOpenB, -61),
        P(backwardPawnOpenC, -59),
        P(backwardPawnOpenD, -75),
        P(backwardPawnOpenE, -72),
        P(backwardPawnOpenF, -75),
        P(backwardPawnOpenG, -71),
        P(backwardPawnOpenH, 3),

        P(rammedWeakPawnA, -16),
        P(rammedWeakPawnB, -8),
        P(rammedWeakPawnC, -12),
        P(rammedWeakPawnD, -8),
        P(rammedWeakPawnE, 2),
        P(rammedWeakPawnF, -12),
        P(rammedWeakPawnG, -24),
        P(rammedWeakPawnH, -60),

        P(isolatedPawnClosedA, 144), P(isolatedPawnClosedB, 36),
        P(isolatedPawnClosedC, 14), P(isolatedPawnClosedD, 16),
        P(isolatedPawnClosedE, -57), P(isolatedPawnClosedF, -88),
        P(isolatedPawnClosedG, -109), P(isolatedPawnClosedH, -7),

        P(isolatedPawnOpenA, 104), P(isolatedPawnOpenB, 49),
        P(isolatedPawnOpenC, -29), P(isolatedPawnOpenD, -86),
        P(isolatedPawnOpenE, -89), P(isolatedPawnOpenF, -48),
        P(isolatedPawnOpenG, -18), P(isolatedPawnOpenH, 45),

        P(sidePawnClosedA, 72), P(sidePawnClosedB, 43),
        P(sidePawnClosedC, 73), P(sidePawnClosedD, -19),
        P(sidePawnClosedE, -30), P(sidePawnClosedF, -61),
        P(sidePawnClosedG, -114), P(sidePawnClosedH, -31),

        P(sidePawnOpenA, 36), P(sidePawnOpenB, 58),
        P(sidePawnOpenC, 18), P(sidePawnOpenD, -37),
        P(sidePawnOpenE, -31), P(sidePawnOpenF, -15),
        P(sidePawnOpenG, 11), P(sidePawnOpenH, 11),

        P(middlePawnClosedA, 0), P(middlePawnClosedB, 7),
        P(middlePawnClosedC, 100), P(middlePawnClosedD, -8),
        P(middlePawnClosedE, -5), P(middlePawnClosedF, -20),
        P(middlePawnClosedG, -93), P(middlePawnClosedH, 0),

        P(middlePawnOpenA, 0), P(middlePawnOpenB, -40),
        P(middlePawnOpenC, 15), P(middlePawnOpenD, 3),
        P(middlePawnOpenE, 10), P(middlePawnOpenF, -37),
        P(middlePawnOpenG, -52), P(middlePawnOpenH, 0),

        P(duoPawnA, 10),
        P(duoPawnB, -10),
        P(duoPawnC, -5),
        P(duoPawnD, 28),
        P(duoPawnE, 27),
        P(duoPawnF, 13),
        P(duoPawnG, 25),
        P(duoPawnH, -56),

        P(openFilePawn_0, -21),
        P(openFilePawn_1, -18),
        P(openFilePawn_2, 32),
        P(openFilePawn_3, 9),
        P(openFilePawn_4, 50),
        P(openFilePawn_5, 237),

        P(stoppedPawn_0, -40),
        P(stoppedPawn_1, 17),
        P(stoppedPawn_2, 16),
        P(stoppedPawn_3, -3),
        P(stoppedPawn_4, 0),
        P(stoppedPawn_5, 0),

        P(duoPawn_0, 2),
        P(duoPawn_1, -9),
        P(duoPawn_2, 28),
        P(duoPawn_3, 146),
        P(duoPawn_4, 527),
        P(duoPawn_5, 1534),

        P(trailingPawn_0, -8),
        P(trailingPawn_1, -53),
        P(trailingPawn_2, -58),
        P(trailingPawn_3, -25),
        P(trailingPawn_4, 4),
        P(trailingPawn_5, -27),

        P(capturePawn_0, 132),
        P(capturePawn_1, -70),
        P(capturePawn_2, -59),
        P(capturePawn_3, 46),
        P(capturePawn_4, 336),
        P(capturePawn_5, 0),

        P(defendedPawn_0, 0),
        P(defendedPawn_1, 59),
        P(defendedPawn_2, 10),
        P(defendedPawn_3, 100),
        P(defendedPawn_4, 206),
        P(defendedPawn_5, 174),

        P(pawnLever_0, 0),
        P(pawnLever_1, 0),
        P(pawnLever_2, -36),
        P(pawnLever_3, -35),
        P(pawnLever_4, -29),
        P(pawnLever_5, -24),
        P(pawnLever_6, 0),

        P(rammedPawn_0, -22),
        P(rammedPawn_1, 20),
        P(rammedPawn_2, -59),
        P(rammedPawn_3, -41),
        P(rammedPawn_4, 9),
        P(rammedPawn_5, 0),

        P(mobilePawn_0, 1),
        P(mobilePawn_1, 10),
        P(mobilePawn_2, 11),
        P(mobilePawn_3, 10),
        P(mobilePawn_4, 24),
        P(mobilePawn_5, 49),

        // Quadratic polynomials for passers
        P(passerA_0, -32), P(passerB_0, 53), P(passerC_0, 118), P(passerD_0, 110),
        P(passerE_0, 252), P(passerF_0, 164), P(passerG_0, 95), P(passerH_0, 68),

        P(passerA_1, 71), P(passerB_1, -10), P(passerC_1, -58), P(passerD_1, -35),
        P(passerE_1, -89), P(passerF_1, -20), P(passerG_1, 59), P(passerH_1, 92),

        P(passerA_2, 144), P(passerB_2, 210), P(passerC_2, 243), P(passerD_2, 199),
        P(passerE_2, 245), P(passerF_2, 202), P(passerG_2, 134), P(passerH_2, 86),

        P(passerScalingOffset, 4038),
        P(passerAndQueen, 739),
        P(passerAndRook, 376),
        P(passerAndBishop, 571),
        P(passerAndKnight, 372),
        P(passerAndPawn, -633),
        P(passerVsQueen, -12),
        P(passerVsRook, -475),
        P(passerVsBishop, -1272),
        P(passerVsKnight, -896),
        P(passerVsPawn, 92),

        P(candidateByRank_0, -2),
        P(candidateByRank_1, 25),
        P(candidateByRank_2, 75),
        P(candidateByRank_3, 165),
        P(candidateByRank_4, 246),

        P(candidateA, 50),
        P(candidateB, -39),
        P(candidateC, -5),
        P(candidateD, -37),
        P(candidateE, 22),
        P(candidateF, 36),
        P(candidateG, 3),
        P(candidateH, 58),
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
        P(knightByFile_5, -24),
        P(knightByFile_6, 24), // -fileH

        // Opposite side kings
        P(knightByFile_0x, -121), // fileA
        P(knightByFile_1x, -155),
        P(knightByFile_2x, -129),
        P(knightByFile_3x, -36),
        P(knightByFile_4x, 50),
        P(knightByFile_5x, 133),
        P(knightByFile_6x, 115), // -fileH

        P(knightByRank_0, -125), // rank1
        P(knightByRank_1, -206),
        P(knightByRank_2, -226),
        P(knightByRank_3, -165),
        P(knightByRank_4, -55),
        P(knightByRank_5, 97),
        P(knightByRank_6, 159), // -rank8

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
       P(bishopBySquare_0, -178),
        P(bishopBySquare_1, 5),
        P(bishopBySquare_2, -74),
        P(bishopBySquare_3, 88),
        P(bishopBySquare_4, -55),
        P(bishopBySquare_5, 0),
        P(bishopBySquare_6, -58),
        P(bishopBySquare_7, 51),

        P(bishopBySquare_8, -41),
        P(bishopBySquare_9, -46),
        P(bishopBySquare_10, 83),
        P(bishopBySquare_11, -20),
        P(bishopBySquare_12, 34),
        P(bishopBySquare_13, 104),
        P(bishopBySquare_14, 80),
        P(bishopBySquare_15, 116),

        P(bishopBySquare_16, -122),
        P(bishopBySquare_17, 46),
        P(bishopBySquare_18, 23),
        P(bishopBySquare_19, 53),
        P(bishopBySquare_20, 38),
        P(bishopBySquare_21, 88),
        P(bishopBySquare_22, 69),
        P(bishopBySquare_23, 110),

        P(bishopBySquare_24, -84),
        P(bishopBySquare_25, -55),
        P(bishopBySquare_26, 56),
        P(bishopBySquare_27, 91),
        P(bishopBySquare_28, 146),
        P(bishopBySquare_29, 89),
        P(bishopBySquare_30, 99),
        P(bishopBySquare_31, 73),

        P(bishopBySquare_32, -77),
        P(bishopBySquare_33, 21),
        P(bishopBySquare_34, 12),
        P(bishopBySquare_35, 83),
        P(bishopBySquare_36, 92),
        P(bishopBySquare_37, 128),
        P(bishopBySquare_38, 22),
        P(bishopBySquare_39, 68),

        P(bishopBySquare_40, -74),
        P(bishopBySquare_41, 4),
        P(bishopBySquare_42, 55),
        P(bishopBySquare_43, -13),
        P(bishopBySquare_44, 113),
        P(bishopBySquare_45, 227),
        P(bishopBySquare_46, 45),
        P(bishopBySquare_47, -6),

        P(bishopBySquare_48, -52),
        P(bishopBySquare_49, 50),
        P(bishopBySquare_50, 36),
        P(bishopBySquare_51, 47),
        P(bishopBySquare_52, -14),
        P(bishopBySquare_53, 154),
        P(bishopBySquare_54, -60),
        P(bishopBySquare_55, 14),

        P(bishopBySquare_56, -209),
        P(bishopBySquare_57, -57),
        P(bishopBySquare_58, 7),
        P(bishopBySquare_59, 30),
        P(bishopBySquare_60, -1),
        P(bishopBySquare_61, 128),
        P(bishopBySquare_62, -232),
        P(bishopBySquare_63, -80),

        P(bishopBySquare_0x, -105),
        P(bishopBySquare_1x, -31),
        P(bishopBySquare_2x, 29),
        P(bishopBySquare_3x, -7),
        P(bishopBySquare_4x, 7),
        P(bishopBySquare_5x, 110),
        P(bishopBySquare_6x, -216),
        P(bishopBySquare_7x, -84),

        P(bishopBySquare_8x, -85),
        P(bishopBySquare_9x, -68),
        P(bishopBySquare_10x, 9),
        P(bishopBySquare_11x, 75),
        P(bishopBySquare_12x, 48),
        P(bishopBySquare_13x, 141),
        P(bishopBySquare_14x, 34),
        P(bishopBySquare_15x, 4),

        P(bishopBySquare_16x, -110),
        P(bishopBySquare_17x, -2),
        P(bishopBySquare_18x, 61),
        P(bishopBySquare_19x, 62),
        P(bishopBySquare_20x, 139),
        P(bishopBySquare_21x, 155),
        P(bishopBySquare_22x, 88),
        P(bishopBySquare_23x, 31),

        P(bishopBySquare_24x, -74),
        P(bishopBySquare_25x, 2),
        P(bishopBySquare_26x, 77),
        P(bishopBySquare_27x, 95),
        P(bishopBySquare_28x, 140),
        P(bishopBySquare_29x, 179),
        P(bishopBySquare_30x, 91),
        P(bishopBySquare_31x, 125),

        P(bishopBySquare_32x, -52),
        P(bishopBySquare_33x, 78),
        P(bishopBySquare_34x, 97),
        P(bishopBySquare_35x, 146),
        P(bishopBySquare_36x, 177),
        P(bishopBySquare_37x, 164),
        P(bishopBySquare_38x, 177),
        P(bishopBySquare_39x, 107),

        P(bishopBySquare_40x, -82),
        P(bishopBySquare_41x, 76),
        P(bishopBySquare_42x, 130),
        P(bishopBySquare_43x, 155),
        P(bishopBySquare_44x, 148),
        P(bishopBySquare_45x, 166),
        P(bishopBySquare_46x, 151),
        P(bishopBySquare_47x, 156),

        P(bishopBySquare_48x, -8),
        P(bishopBySquare_49x, 135),
        P(bishopBySquare_50x, 126),
        P(bishopBySquare_51x, 64),
        P(bishopBySquare_52x, 131),
        P(bishopBySquare_53x, 174),
        P(bishopBySquare_54x, 166),
        P(bishopBySquare_55x, 154),

        P(bishopBySquare_56x, -92),
        P(bishopBySquare_57x, 63),
        P(bishopBySquare_58x, 42),
        P(bishopBySquare_59x, 80),
        P(bishopBySquare_60x, 74),
        P(bishopBySquare_61x, 58),
        P(bishopBySquare_62x, -17),
        P(bishopBySquare_63x, -23),
#endif

        /*
         *  Rooks
         */

        P(rookByFile_0, -64), // fileA
        P(rookByFile_1, -85),
        P(rookByFile_2, -57),
        P(rookByFile_3, -9),
        P(rookByFile_4, 39),
        P(rookByFile_5, 57),
        P(rookByFile_6, 109), // -fileH

        P(rookByFile_0x, -85), // fileA
        P(rookByFile_1x, -71),
        P(rookByFile_2x, -77),
        P(rookByFile_3x, -59),
        P(rookByFile_4x, -5),
        P(rookByFile_5x, 38),
        P(rookByFile_6x, 66), // -fileH

        P(rookByRank_0, -138), // rank1
        P(rookByRank_1, -318),
        P(rookByRank_2, -464),
        P(rookByRank_3, -522),
        P(rookByRank_4, -437),
        P(rookByRank_5, -284),
        P(rookByRank_6, -127), // -rank8

        /*
         *  Queens
         */

        P(queenByFile_0, -109), // fileA
        P(queenByFile_1, -168),
        P(queenByFile_2, -171),
        P(queenByFile_3, -147),
        P(queenByFile_4, -112),
        P(queenByFile_5, -77),
        P(queenByFile_6, -29), // -fileH

        P(queenByFile_0x, -7), // fileA
        P(queenByFile_1x, -7),
        P(queenByFile_2x, 17),
        P(queenByFile_3x, 64),
        P(queenByFile_4x, 127),
        P(queenByFile_5x, 168),
        P(queenByFile_6x, 125), // -fileH

        P(queenByRank_0, -142), // rank1
        P(queenByRank_1, -231),
        P(queenByRank_2, -303),
        P(queenByRank_3, -332),
        P(queenByRank_4, -289),
        P(queenByRank_5, -192),
        P(queenByRank_6, -109), // -rank8

        /*
         *  Kings
         */

        P(kingByFile_0, 42), // fileD
        P(kingByFile_1, 56),
        P(kingByFile_2, 48), // -fileH

        P(kingByRank_0, -237), // rank1
        P(kingByRank_1, -398),
        P(kingByRank_2, -524),
        P(kingByRank_3, -553),
        P(kingByRank_4, -429),
        P(kingByRank_5, -174),
        P(kingByRank_6, 43), // -rank8

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
        P(drawOffset, 24),
        P(drawQueen, -313),
        P(drawRook, 105),
        P(drawBishop, 13),
        P(drawKnight, 39),
        P(drawPawn, -710),
        P(drawQueenEnding, 71),
        P(drawRookEnding, 506),
        P(drawKnightEnding, 516),
        P(drawBishopEnding, 1420),
        P(drawPawnEnding, -18126),
        P(drawPawnless, 139),

        P(drawQueenImbalance, -458),
        P(drawRookImbalance, 408),
        P(drawMinorImbalance, 14),

        P(drawUnlikeBishops, 3483),
        P(drawUnlikeBishopsAndQueens, 620),
        P(drawUnlikeBishopsAndRooks, 386),
        P(drawUnlikeBishopsAndKnights, 865),
        // TODO: pawn and pawn block square colors


// }

/*----------------------------------------------------------------------+
 |                                                                      |
 +----------------------------------------------------------------------*/

