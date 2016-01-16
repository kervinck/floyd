
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
        P(winBonus, 3205),

        P(castleK, -125),
        P(castleQ, -88),
        P(castleKQ, -36),

        P(queenValue, 6668),
        P(rookValue, 4360),
        P(bishopValue, 2467),
        P(knightValue, 2323),
        P(pawnValue1, 1018), P(pawnValue2, 1680), P(pawnValue3, 2350), P(pawnValue4, 3029),
        P(pawnValue5, 3700), P(pawnValue6, 4345), P(pawnValue7, 4956), P(pawnValue8, 5503),

        P(queenAndQueen, -711),
        P(queenAndRook, -628),
        P(queenAndBishop, 66),
        P(queenAndKnight, -15),
        P(queenAndPawn_1, -101), P(queenAndPawn_2, -72),
        P(rookAndRook, -287),
        P(rookAndBishop, -113),
        P(rookAndKnight, -129),
        P(rookAndPawn_1, -57), P(rookAndPawn_2, -52),
        P(bishopAndBishop, 112),
        P(bishopAndKnight, -4),
        P(bishopAndPawn_1, -54), P(bishopAndPawn_2, -19),
        P(knightAndKnight, -42),
        P(knightAndPawn_1, -26), P(knightAndPawn_2, -1),

        P(queenVsRook, -313),
        P(queenVsBishop, 50),
        P(queenVsKnight, -7),
        P(queenVsPawn_1, 128), P(queenVsPawn_2, -113),
        P(rookVsBishop, -41),
        P(rookVsKnight, -6),
        P(rookVsPawn_1, -7), P(rookVsPawn_2, -42),
        P(bishopVsKnight, 23),
        P(bishopVsPawn_1, -11), P(bishopVsPawn_2, -13),
        P(knightVsPawn_1, -4), P(knightVsPawn_2, -21),

        P(controlCenter, 28),
        P(controlExtendedCenter, 35),
        P(controlOutside, 22),

        /*
         *  king safety
         */

        // scaling
        P(safetyScalingOffset, -1885),
        P(safetyAndQueen, -1750),
        P(safetyAndRook, 3007),
        P(safetyAndBishop, 1843),
        P(safetyAndKnight, 1892),
        P(safetyAndPawn, -554),
        P(safetyVsQueen, 13258),
        P(safetyVsRook, -3342),
        P(safetyVsBishop, -2383),
        P(safetyVsKnight, -2146),
        P(safetyVsPawn, -795),

        // shelter
        P(shelterPawn_0, 151), // rank3
        P(shelterPawn_1, 276), // rank4
        P(shelterPawn_2, 272), // rank5
        P(shelterPawn_3, 130), // rank6
        P(shelterPawn_4, -173), // rank7
        P(shelterPawn_5, 293), // no pawn

        P(shelterKing_0, -148), // fileA (fileH)
        P(shelterKing_1, -210),
        P(shelterKing_2, -139), // -fileD (-fileE)

        P(shelterWalkingKing, 44), // rank2 or up
        P(shelterCastled, 45), // fraction of 256

        // attacks
        P(attackSquares_0, 91), // 0 attacks
        P(attackSquares_1, 27),
        P(attackSquares_2, -78),
        P(attackSquares_3, -177),
        P(attackSquares_4, -237),
        P(attackSquares_5, -208), // -(6 or more attacks)

        P(attackByPawn_0, 135),
        P(attackByPawn_1, 286),
        P(attackByPawn_2, 71),
        P(attackByMinor_0, -114),
        P(attackByMinor_1, 21),
        P(attackByMinor_2, -293),
        P(attackByRook_0, 360),
        P(attackByRook_1, 849),
        P(attackByRook_2, -516),
        P(attackByQueen, 181),
        P(attackByKing, -201),

        // TODO: pawn storm

        // king mobility
        P(mobilityKing_0, 112),
        P(mobilityKing_1, 51),

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

        P(pawnByFile_0, 17), // fileA
        P(pawnByFile_1, 45),
        P(pawnByFile_2, 5),
        P(pawnByFile_3, -36),
        P(pawnByFile_4, -66),
        P(pawnByFile_5, -27),
        P(pawnByFile_6, 42), // -fileH

        P(pawnByFile_0x, -130), // fileA
        P(pawnByFile_1x, -207),
        P(pawnByFile_2x, -271),
        P(pawnByFile_3x, -342),
        P(pawnByFile_4x, -339),
        P(pawnByFile_5x, -265),
        P(pawnByFile_6x, -110), // -fileH

        // 5 parameters for 6 pawn ranks
        P(pawnByRank_0, -53), // rank2
        P(pawnByRank_1, -91),
        P(pawnByRank_2, -92),
        P(pawnByRank_3, -30),
        P(pawnByRank_4, 75), // -rank7

        P(doubledPawnA, -198), P(doubledPawnB, -132), P(doubledPawnC, -181), P(doubledPawnD, -131),
        P(doubledPawnE, -126), P(doubledPawnF, -67), P(doubledPawnG, -53), P(doubledPawnH, -224),

        P(backwardPawnClosedByRank_0, 11),
        P(backwardPawnClosedByRank_1, 16),
        P(backwardPawnClosedByRank_2, -8),
        P(backwardPawnClosedByRank_3, -6),
        P(backwardPawnClosedByRank_4, 0),

        P(backwardPawnOpenByRank_0, 17),
        P(backwardPawnOpenByRank_1, 15),
        P(backwardPawnOpenByRank_2, 9),
        P(backwardPawnOpenByRank_3, 46),
        P(backwardPawnOpenByRank_4, 0),

        P(backwardPawnClosedA, 56),
        P(backwardPawnClosedB, 0),
        P(backwardPawnClosedC, 21),
        P(backwardPawnClosedD, -36),
        P(backwardPawnClosedE, -55),
        P(backwardPawnClosedF, -23),
        P(backwardPawnClosedG, -4),
        P(backwardPawnClosedH, 49),

        P(backwardPawnOpenA, -24),
        P(backwardPawnOpenB, -56),
        P(backwardPawnOpenC, -50),
        P(backwardPawnOpenD, -75),
        P(backwardPawnOpenE, -73),
        P(backwardPawnOpenF, -73),
        P(backwardPawnOpenG, -66),
        P(backwardPawnOpenH, 4),

        P(rammedWeakPawnA, -20),
        P(rammedWeakPawnB, -4),
        P(rammedWeakPawnC, -18),
        P(rammedWeakPawnD, -10),
        P(rammedWeakPawnE, 0),
        P(rammedWeakPawnF, -12),
        P(rammedWeakPawnG, -24),
        P(rammedWeakPawnH, -57),

        P(isolatedPawnClosedA, 156), P(isolatedPawnClosedB, 50),
        P(isolatedPawnClosedC, 14), P(isolatedPawnClosedD, 30),
        P(isolatedPawnClosedE, -59), P(isolatedPawnClosedF, -97),
        P(isolatedPawnClosedG, -115), P(isolatedPawnClosedH, -22),

        P(isolatedPawnOpenA, 141), P(isolatedPawnOpenB, 64),
        P(isolatedPawnOpenC, -29), P(isolatedPawnOpenD, -83),
        P(isolatedPawnOpenE, -92), P(isolatedPawnOpenF, -53),
        P(isolatedPawnOpenG, -14), P(isolatedPawnOpenH, 58),

        P(sidePawnClosedA, 88), P(sidePawnClosedB, 49),
        P(sidePawnClosedC, 75), P(sidePawnClosedD, -12),
        P(sidePawnClosedE, -31), P(sidePawnClosedF, -67),
        P(sidePawnClosedG, -123), P(sidePawnClosedH, -49),

        P(sidePawnOpenA, 79), P(sidePawnOpenB, 65),
        P(sidePawnOpenC, 21), P(sidePawnOpenD, -40),
        P(sidePawnOpenE, -33), P(sidePawnOpenF, -15),
        P(sidePawnOpenG, 12), P(sidePawnOpenH, 21),

        P(middlePawnClosedA, 0), P(middlePawnClosedB, 18),
        P(middlePawnClosedC, 105), P(middlePawnClosedD, -8),
        P(middlePawnClosedE, -4), P(middlePawnClosedF, -24),
        P(middlePawnClosedG, -97), P(middlePawnClosedH, 0),

        P(middlePawnOpenA, 0), P(middlePawnOpenB, -31),
        P(middlePawnOpenC, 16), P(middlePawnOpenD, -10),
        P(middlePawnOpenE, 8), P(middlePawnOpenF, -32),
        P(middlePawnOpenG, -50), P(middlePawnOpenH, 0),

        P(duoPawnA, 13),
        P(duoPawnB, -10),
        P(duoPawnC, -5),
        P(duoPawnD, 29),
        P(duoPawnE, 27),
        P(duoPawnF, 13),
        P(duoPawnG, 24),
        P(duoPawnH, -55),

        P(openFilePawn_0, -21),
        P(openFilePawn_1, -21),
        P(openFilePawn_2, 32),
        P(openFilePawn_3, 18),
        P(openFilePawn_4, 86),
        P(openFilePawn_5, 267),

        P(stoppedPawn_0, -44),
        P(stoppedPawn_1, 19),
        P(stoppedPawn_2, 20),
        P(stoppedPawn_3, -3),
        P(stoppedPawn_4, 0),
        P(stoppedPawn_5, 0),

        P(duoPawn_0, 2),
        P(duoPawn_1, -7),
        P(duoPawn_2, 28),
        P(duoPawn_3, 151),
        P(duoPawn_4, 528),
        P(duoPawn_5, 1518),

        P(trailingPawn_0, -9),
        P(trailingPawn_1, -52),
        P(trailingPawn_2, -60),
        P(trailingPawn_3, -26),
        P(trailingPawn_4, -2),
        P(trailingPawn_5, -34),

        P(capturePawn_0, 131),
        P(capturePawn_1, -63),
        P(capturePawn_2, -62),
        P(capturePawn_3, 51),
        P(capturePawn_4, 320),
        P(capturePawn_5, 0),

        P(defendedPawn_0, 0),
        P(defendedPawn_1, 61),
        P(defendedPawn_2, 9),
        P(defendedPawn_3, 101),
        P(defendedPawn_4, 196),
        P(defendedPawn_5, 154),

        P(pawnLever_0, 0),
        P(pawnLever_1, 0),
        P(pawnLever_2, -35),
        P(pawnLever_3, -36),
        P(pawnLever_4, -27),
        P(pawnLever_5, -23),
        P(pawnLever_6, 0),

        P(rammedPawn_0, -31),
        P(rammedPawn_1, 11),
        P(rammedPawn_2, -46),
        P(rammedPawn_3, -42),
        P(rammedPawn_4, 18),
        P(rammedPawn_5, 0),

        P(mobilePawn_0, 1),
        P(mobilePawn_1, 17),
        P(mobilePawn_2, 19),
        P(mobilePawn_3, 18),
        P(mobilePawn_4, 29),
        P(mobilePawn_5, 91),

        // Quadratic polynomials for passers
        P(passerA_0, -72), P(passerB_0, 46), P(passerC_0, 141), P(passerD_0, 123),
        P(passerE_0, 263), P(passerF_0, 176), P(passerG_0, 103), P(passerH_0, 59),

        P(passerA_1, 70), P(passerB_1, -9), P(passerC_1, -63), P(passerD_1, -40),
        P(passerE_1, -96), P(passerF_1, -21), P(passerG_1, 62), P(passerH_1, 95),

        P(passerA_2, 142), P(passerB_2, 204), P(passerC_2, 237), P(passerD_2, 199),
        P(passerE_2, 245), P(passerF_2, 196), P(passerG_2, 122), P(passerH_2, 80),

        P(passerScalingOffset, 3980),
        P(passerAndQueen, 942),
        P(passerAndRook, 485),
        P(passerAndBishop, 528),
        P(passerAndKnight, 394),
        P(passerAndPawn, -633),
        P(passerVsQueen, -254),
        P(passerVsRook, -425),
        P(passerVsBishop, -1340),
        P(passerVsKnight, -939),
        P(passerVsPawn, 96),

        P(candidateByRank_0, 4),
        P(candidateByRank_1, 35),
        P(candidateByRank_2, 93),
        P(candidateByRank_3, 170),
        P(candidateByRank_4, 273),

        P(candidateA, 3),
        P(candidateB, -50),
        P(candidateC, -7),
        P(candidateD, -42),
        P(candidateE, 11),
        P(candidateF, 22),
        P(candidateG, 0),
        P(candidateH, 44),
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
        P(knightByFile_1, -219),
        P(knightByFile_2, -231),
        P(knightByFile_3, -163),
        P(knightByFile_4, -100),
        P(knightByFile_5, -24),
        P(knightByFile_6, 23), // -fileH

        // Opposite side kings
        P(knightByFile_0x, -130), // fileA
        P(knightByFile_1x, -164),
        P(knightByFile_2x, -132),
        P(knightByFile_3x, -37),
        P(knightByFile_4x, 49),
        P(knightByFile_5x, 136),
        P(knightByFile_6x, 121), // -fileH

        P(knightByRank_0, -126), // rank1
        P(knightByRank_1, -207),
        P(knightByRank_2, -226),
        P(knightByRank_3, -167),
        P(knightByRank_4, -59),
        P(knightByRank_5, 88),
        P(knightByRank_6, 144), // -rank8

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
       P(bishopBySquare_0, -176),
        P(bishopBySquare_1, 5),
        P(bishopBySquare_2, -72),
        P(bishopBySquare_3, 94),
        P(bishopBySquare_4, -52),
        P(bishopBySquare_5, 1),
        P(bishopBySquare_6, -62),
        P(bishopBySquare_7, 51),

        P(bishopBySquare_8, -31),
        P(bishopBySquare_9, -41),
        P(bishopBySquare_10, 92),
        P(bishopBySquare_11, -10),
        P(bishopBySquare_12, 39),
        P(bishopBySquare_13, 103),
        P(bishopBySquare_14, 79),
        P(bishopBySquare_15, 112),

        P(bishopBySquare_16, -110),
        P(bishopBySquare_17, 52),
        P(bishopBySquare_18, 31),
        P(bishopBySquare_19, 58),
        P(bishopBySquare_20, 45),
        P(bishopBySquare_21, 96),
        P(bishopBySquare_22, 62),
        P(bishopBySquare_23, 106),

        P(bishopBySquare_24, -76),
        P(bishopBySquare_25, -54),
        P(bishopBySquare_26, 62),
        P(bishopBySquare_27, 101),
        P(bishopBySquare_28, 157),
        P(bishopBySquare_29, 92),
        P(bishopBySquare_30, 99),
        P(bishopBySquare_31, 74),

        P(bishopBySquare_32, -78),
        P(bishopBySquare_33, 20),
        P(bishopBySquare_34, 18),
        P(bishopBySquare_35, 95),
        P(bishopBySquare_36, 102),
        P(bishopBySquare_37, 142),
        P(bishopBySquare_38, 24),
        P(bishopBySquare_39, 70),

        P(bishopBySquare_40, -73),
        P(bishopBySquare_41, 8),
        P(bishopBySquare_42, 59),
        P(bishopBySquare_43, -7),
        P(bishopBySquare_44, 123),
        P(bishopBySquare_45, 241),
        P(bishopBySquare_46, 50),
        P(bishopBySquare_47, -5),

        P(bishopBySquare_48, -52),
        P(bishopBySquare_49, 47),
        P(bishopBySquare_50, 38),
        P(bishopBySquare_51, 49),
        P(bishopBySquare_52, -6),
        P(bishopBySquare_53, 163),
        P(bishopBySquare_54, -53),
        P(bishopBySquare_55, 16),

        P(bishopBySquare_56, -217),
        P(bishopBySquare_57, -59),
        P(bishopBySquare_58, 3),
        P(bishopBySquare_59, 29),
        P(bishopBySquare_60, 1),
        P(bishopBySquare_61, 130),
        P(bishopBySquare_62, -232),
        P(bishopBySquare_63, -83),

        P(bishopBySquare_0x, -113),
        P(bishopBySquare_1x, -28),
        P(bishopBySquare_2x, 24),
        P(bishopBySquare_3x, -13),
        P(bishopBySquare_4x, 4),
        P(bishopBySquare_5x, 107),
        P(bishopBySquare_6x, -223),
        P(bishopBySquare_7x, -97),

        P(bishopBySquare_8x, -80),
        P(bishopBySquare_9x, -67),
        P(bishopBySquare_10x, 16),
        P(bishopBySquare_11x, 80),
        P(bishopBySquare_12x, 44),
        P(bishopBySquare_13x, 148),
        P(bishopBySquare_14x, 43),
        P(bishopBySquare_15x, 0),

        P(bishopBySquare_16x, -98),
        P(bishopBySquare_17x, 8),
        P(bishopBySquare_18x, 74),
        P(bishopBySquare_19x, 69),
        P(bishopBySquare_20x, 143),
        P(bishopBySquare_21x, 165),
        P(bishopBySquare_22x, 92),
        P(bishopBySquare_23x, 27),

        P(bishopBySquare_24x, -66),
        P(bishopBySquare_25x, 5),
        P(bishopBySquare_26x, 84),
        P(bishopBySquare_27x, 109),
        P(bishopBySquare_28x, 146),
        P(bishopBySquare_29x, 187),
        P(bishopBySquare_30x, 96),
        P(bishopBySquare_31x, 123),

        P(bishopBySquare_32x, -54),
        P(bishopBySquare_33x, 86),
        P(bishopBySquare_34x, 104),
        P(bishopBySquare_35x, 156),
        P(bishopBySquare_36x, 192),
        P(bishopBySquare_37x, 173),
        P(bishopBySquare_38x, 176),
        P(bishopBySquare_39x, 103),

        P(bishopBySquare_40x, -68),
        P(bishopBySquare_41x, 80),
        P(bishopBySquare_42x, 141),
        P(bishopBySquare_43x, 161),
        P(bishopBySquare_44x, 164),
        P(bishopBySquare_45x, 176),
        P(bishopBySquare_46x, 153),
        P(bishopBySquare_47x, 149),

        P(bishopBySquare_48x, -11),
        P(bishopBySquare_49x, 146),
        P(bishopBySquare_50x, 132),
        P(bishopBySquare_51x, 73),
        P(bishopBySquare_52x, 140),
        P(bishopBySquare_53x, 183),
        P(bishopBySquare_54x, 170),
        P(bishopBySquare_55x, 154),

        P(bishopBySquare_56x, -98),
        P(bishopBySquare_57x, 59),
        P(bishopBySquare_58x, 45),
        P(bishopBySquare_59x, 80),
        P(bishopBySquare_60x, 73),
        P(bishopBySquare_61x, 58),
        P(bishopBySquare_62x, -18),
        P(bishopBySquare_63x, -23),
#endif

        /*
         *  Rooks
         */

        P(rookByFile_0, -58), // fileA
        P(rookByFile_1, -69),
        P(rookByFile_2, -32), // -fileD

        P(rookByRank_0, -102), // rank1
        P(rookByRank_1, -261),
        P(rookByRank_2, -400),
        P(rookByRank_3, -471),
        P(rookByRank_4, -414),
        P(rookByRank_5, -282),
        P(rookByRank_6, -141), // -rank8

        P(rookOnHalfOpen_0, 165), // fileA
        P(rookOnHalfOpen_1, 174),
        P(rookOnHalfOpen_2, 131),
        P(rookOnHalfOpen_3, 160), // fileD

        P(rookOnOpen_0, 155), // fileA
        P(rookOnOpen_1, 128),
        P(rookOnOpen_2, 82),
        P(rookOnOpen_3, 74), // fileD

        /*
         *  Queens
         */

        P(queenByFile_0, -111), // fileA
        P(queenByFile_1, -167),
        P(queenByFile_2, -169),
        P(queenByFile_3, -147),
        P(queenByFile_4, -115),
        P(queenByFile_5, -82),
        P(queenByFile_6, -30), // -fileH

        P(queenByFile_0x, -9), // fileA
        P(queenByFile_1x, -10),
        P(queenByFile_2x, 17),
        P(queenByFile_3x, 69),
        P(queenByFile_4x, 128),
        P(queenByFile_5x, 167),
        P(queenByFile_6x, 129), // -fileH

        P(queenByRank_0, -134), // rank1
        P(queenByRank_1, -220),
        P(queenByRank_2, -290),
        P(queenByRank_3, -319),
        P(queenByRank_4, -278),
        P(queenByRank_5, -189),
        P(queenByRank_6, -110), // -rank8

        /*
         *  Kings
         */

        P(kingByFile_0, 40), // fileD
        P(kingByFile_1, 53),
        P(kingByFile_2, 48), // -fileH

        P(kingByRank_0, -247), // rank1
        P(kingByRank_1, -408),
        P(kingByRank_2, -542),
        P(kingByRank_3, -577),
        P(kingByRank_4, -456),
        P(kingByRank_5, -205),
        P(kingByRank_6, 18), // -rank8

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
        P(drawOffset, -92),
        P(drawQueen, -213),
        P(drawRook, 117),
        P(drawBishop, 18),
        P(drawKnight, 33),
        P(drawPawn, -678),
        P(drawQueenEnding, 30),
        P(drawRookEnding, 520),
        P(drawKnightEnding, 503),
        P(drawBishopEnding, 1437),
        P(drawPawnEnding, -18626),
        P(drawPawnless, 165),

        P(drawQueenImbalance, -507),
        P(drawRookImbalance, 418),
        P(drawMinorImbalance, -6),

        P(drawUnlikeBishops, 3336),
        P(drawUnlikeBishopsAndQueens, 655),
        P(drawUnlikeBishopsAndRooks, 413),
        P(drawUnlikeBishopsAndKnights, 882),
        // TODO: pawn and pawn block square colors


// }

/*----------------------------------------------------------------------+
 |                                                                      |
 +----------------------------------------------------------------------*/

