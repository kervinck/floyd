
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
        P(winBonus, 1483),

        P(castleK, -132),
        P(castleQ, -119),
        P(castleKQ, -68),

        P(queenValue, 6821),
        P(rookValue, 4598),
        P(bishopValue, 2534),
        P(knightValue, 2372),
        P(pawnValue1, 994),
        P(pawnValue2, 1670),
        P(pawnValue3, 2348),
        P(pawnValue4, 3029),
        P(pawnValue5, 3698),
        P(pawnValue6, 4347),
        P(pawnValue7, 4961),
        P(pawnValue8, 5478),

        P(queenAndQueen, -618),
        P(queenAndRook, -638),
        P(queenAndBishop, 84),
        P(queenAndKnight, 26),
        P(queenAndPawn_1, -65),
        P(queenAndPawn_2, -114),

        P(rookAndRook, -277),
        P(rookAndBishop, -99),
        P(rookAndKnight, -91),
        P(rookAndPawn_1, -53),
        P(rookAndPawn_2, -57),

        P(bishopAndBishop, 122),
        P(bishopAndKnight, 25),
        P(bishopAndPawn_1, -48),
        P(bishopAndPawn_2, 2),

        P(knightAndKnight, -17),
        P(knightAndPawn_1, -24),
        P(knightAndPawn_2, -1),

        P(queenVsRook, -295),
        P(queenVsBishop, 74),
        P(queenVsKnight, 18),
        P(queenVsPawn_1, 152),
        P(queenVsPawn_2, -136),

        P(rookVsBishop, -24),
        P(rookVsKnight, 7),
        P(rookVsPawn_1, -4),
        P(rookVsPawn_2, -49),

        P(bishopVsKnight, 6),
        P(bishopVsPawn_1, -9),
        P(bishopVsPawn_2, 3),

        P(knightVsPawn_1, -2),
        P(knightVsPawn_2, -19),

        /*
         *  Board control
         */

        P(controlCenter, 26),
        P(controlExtendedCenter, 33),
        P(controlOutside, 20),

        /*
         *  King safety
         */

        // scaling
        P(safetyScalingOffset, -956),
        P(safetyAndQueen, -1793),
        P(safetyAndRook, 3209),
        P(safetyAndBishop, 1878),
        P(safetyAndKnight, 1931),
        P(safetyAndPawn, -497),
        P(safetyVsQueen, 12751),
        P(safetyVsRook, -3740),
        P(safetyVsBishop, -2438),
        P(safetyVsKnight, -2177),
        P(safetyVsPawn, -831),

        // shelter
        P(shelterPawn_0, 138), // rank3
        P(shelterPawn_1, 266), // rank4
        P(shelterPawn_2, 259), // rank5
        P(shelterPawn_3, 90), // rank6
        P(shelterPawn_4, -249), // rank7
        P(shelterPawn_5, 291), // no pawn

        P(shelterKing_0, -281), // fileA (fileH)
        P(shelterKing_1, -297),
        P(shelterKing_2, -179), // -fileD (-fileE)

        P(shelterWalkingKing, 47), // rank2 or up
        P(shelterCastled, 48), // fraction of 256

        // attacks
        P(attackSquares_0, 77), // 0 attacks
        P(attackSquares_1, -6),
        P(attackSquares_2, -124),
        P(attackSquares_3, -236),
        P(attackSquares_4, -294),
        P(attackSquares_5, -254), // -(6 or more attacks)

        P(attackByPawn_0, 132),
        P(attackByPawn_1, 294),
        P(attackByPawn_2, 70),
        P(attackByMinor_0, -109),
        P(attackByMinor_1, 13),
        P(attackByMinor_2, -266),
        P(attackByRook_0, 450),
        P(attackByRook_1, 959),
        P(attackByRook_2, -429),
        P(attackByQueen, 183),
        P(attackByKing, -276),

        // TODO: pawn storm

        // king mobility
        P(mobilityKing_0, 92),
        P(mobilityKing_1, 41),

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

        P(pawnByFile_0, 24), // fileA
        P(pawnByFile_1, 49),
        P(pawnByFile_2, 14),
        P(pawnByFile_3, -32),
        P(pawnByFile_4, -55),
        P(pawnByFile_5, -14),
        P(pawnByFile_6, 43), // -fileH

        P(pawnByFile_0x, -132), // fileA
        P(pawnByFile_1x, -237),
        P(pawnByFile_2x, -298),
        P(pawnByFile_3x, -375),
        P(pawnByFile_4x, -373),
        P(pawnByFile_5x, -302),
        P(pawnByFile_6x, -133), // -fileH

        // 5 parameters for 6 pawn ranks
        P(pawnByRank_0, -51), // rank2
        P(pawnByRank_1, -93),
        P(pawnByRank_2, -93),
        P(pawnByRank_3, -18),
        P(pawnByRank_4, 54), // -rank7

        P(doubledPawnA, -179),
        P(doubledPawnB, -103),
        P(doubledPawnC, -135),
        P(doubledPawnD, -86),
        P(doubledPawnE, -81),
        P(doubledPawnF, -32),
        P(doubledPawnG, -14),
        P(doubledPawnH, -181),

        P(backwardPawnClosedByRank_0, 16),
        P(backwardPawnClosedByRank_1, 10),
        P(backwardPawnClosedByRank_2, -9),
        P(backwardPawnClosedByRank_3, 0),
        P(backwardPawnClosedByRank_4, 0),

        P(backwardPawnOpenByRank_0, 25),
        P(backwardPawnOpenByRank_1, 18),
        P(backwardPawnOpenByRank_2, 13),
        P(backwardPawnOpenByRank_3, 41),
        P(backwardPawnOpenByRank_4, 0),

        P(backwardPawnClosedA, 56),
        P(backwardPawnClosedB, 3),
        P(backwardPawnClosedC, 27),
        P(backwardPawnClosedD, -31),
        P(backwardPawnClosedE, -61),
        P(backwardPawnClosedF, -21),
        P(backwardPawnClosedG, -2),
        P(backwardPawnClosedH, 56),

        P(backwardPawnOpenA, -27),
        P(backwardPawnOpenB, -53),
        P(backwardPawnOpenC, -23),
        P(backwardPawnOpenD, -49),
        P(backwardPawnOpenE, -33),
        P(backwardPawnOpenF, -23),
        P(backwardPawnOpenG, -43),
        P(backwardPawnOpenH, 6),

        P(rammedWeakPawnA, -24),
        P(rammedWeakPawnB, -15),
        P(rammedWeakPawnC, -10),
        P(rammedWeakPawnD, -7),
        P(rammedWeakPawnE, 20),
        P(rammedWeakPawnF, 13),
        P(rammedWeakPawnG, -20),
        P(rammedWeakPawnH, -50),

        P(isolatedPawnClosedA, 150),
        P(isolatedPawnClosedB, 52),
        P(isolatedPawnClosedC, -2),
        P(isolatedPawnClosedD, 11),
        P(isolatedPawnClosedE, -84),
        P(isolatedPawnClosedF, -111),
        P(isolatedPawnClosedG, -144),
        P(isolatedPawnClosedH, -54),

        P(isolatedPawnOpenA, 126),
        P(isolatedPawnOpenB, 78),
        P(isolatedPawnOpenC, -30),
        P(isolatedPawnOpenD, -63),
        P(isolatedPawnOpenE, -91),
        P(isolatedPawnOpenF, -48),
        P(isolatedPawnOpenG, 0),
        P(isolatedPawnOpenH, 58),

        P(sidePawnClosedA, 114),
        P(sidePawnClosedB, 51),
        P(sidePawnClosedC, 77),
        P(sidePawnClosedD, -30),
        P(sidePawnClosedE, -46),
        P(sidePawnClosedF, -70),
        P(sidePawnClosedG, -133),
        P(sidePawnClosedH, -37),

        P(sidePawnOpenA, 83),
        P(sidePawnOpenB, 54),
        P(sidePawnOpenC, 22),
        P(sidePawnOpenD, -31),
        P(sidePawnOpenE, -35),
        P(sidePawnOpenF, -18),
        P(sidePawnOpenG, 15),
        P(sidePawnOpenH, 31),

        P(middlePawnClosedA, 0),
        P(middlePawnClosedB, 22),
        P(middlePawnClosedC, 114),
        P(middlePawnClosedD, -13),
        P(middlePawnClosedE, -17),
        P(middlePawnClosedF, -15),
        P(middlePawnClosedG, -98),
        P(middlePawnClosedH, 0),

        P(middlePawnOpenA, 0),
        P(middlePawnOpenB, -60),
        P(middlePawnOpenC, 29),
        P(middlePawnOpenD, -6),
        P(middlePawnOpenE, 5),
        P(middlePawnOpenF, -39),
        P(middlePawnOpenG, -54),
        P(middlePawnOpenH, 0),

        P(duoPawnA, 13),
        P(duoPawnB, -11),
        P(duoPawnC, 0),
        P(duoPawnD, 22),
        P(duoPawnE, 27),
        P(duoPawnF, 12),
        P(duoPawnG, 32),
        P(duoPawnH, -60),

        P(openFilePawn_0, -26),
        P(openFilePawn_1, -30),
        P(openFilePawn_2, 35),
        P(openFilePawn_3, 18),
        P(openFilePawn_4, 110),
        P(openFilePawn_5, 259),

        P(stoppedPawn_0, -40),
        P(stoppedPawn_1, 25),
        P(stoppedPawn_2, 24),
        P(stoppedPawn_3, -10),
        P(stoppedPawn_4, 0),
        P(stoppedPawn_5, 0),

        P(duoPawn_0, 1),
        P(duoPawn_1, -1),
        P(duoPawn_2, 30),
        P(duoPawn_3, 147),
        P(duoPawn_4, 498),
        P(duoPawn_5, 1436),

        P(trailingPawn_0, -12),
        P(trailingPawn_1, -47),
        P(trailingPawn_2, -70),
        P(trailingPawn_3, -40),
        P(trailingPawn_4, -20),
        P(trailingPawn_5, -48),

        P(capturePawn_0, 98),
        P(capturePawn_1, -60),
        P(capturePawn_2, -69),
        P(capturePawn_3, 40),
        P(capturePawn_4, 296),
        P(capturePawn_5, 0),

        P(defendedPawn_0, 0),
        P(defendedPawn_1, 63),
        P(defendedPawn_2, 9),
        P(defendedPawn_3, 110),
        P(defendedPawn_4, 234),
        P(defendedPawn_5, 175),

        P(pawnLever_0, 0),
        P(pawnLever_1, 0),
        P(pawnLever_2, -37),
        P(pawnLever_3, -35),
        P(pawnLever_4, -29),
        P(pawnLever_5, -22),
        P(pawnLever_6, 0),

        P(rammedPawn_0, -55),
        P(rammedPawn_1, -1),
        P(rammedPawn_2, -11),
        P(rammedPawn_3, -56),
        P(rammedPawn_4, 30),
        P(rammedPawn_5, 0),

        P(mobilePawn_0, -1),
        P(mobilePawn_1, 12),
        P(mobilePawn_2, 19),
        P(mobilePawn_3, 39),
        P(mobilePawn_4, 105),
        P(mobilePawn_5, 192),

        // Quadratic polynomials for passers
        P(passerA_0, -32),
        P(passerB_0, 31),
        P(passerC_0, 158),
        P(passerD_0, 165),
        P(passerE_0, 278),
        P(passerF_0, 122),
        P(passerG_0, 16),
        P(passerH_0, -51),

        P(passerA_1, 69),
        P(passerB_1, 1),
        P(passerC_1, -63),
        P(passerD_1, -52),
        P(passerE_1, -107),
        P(passerF_1, -1),
        P(passerG_1, 67),
        P(passerH_1, 123),

        P(passerA_2, 138),
        P(passerB_2, 189),
        P(passerC_2, 235),
        P(passerD_2, 207),
        P(passerE_2, 259),
        P(passerF_2, 182),
        P(passerG_2, 118),
        P(passerH_2, 66),

        P(passerScalingOffset, 3717),
        P(passerAndQueen, 1475),
        P(passerAndRook, 755),
        P(passerAndBishop, 758),
        P(passerAndKnight, 341),
        P(passerAndPawn, -600),
        P(passerVsQueen, -695),
        P(passerVsRook, -645),
        P(passerVsBishop, -1408),
        P(passerVsKnight, -1003),
        P(passerVsPawn, 249),

        P(protectedPasser, -26), // TODO: check this after tuning
        P(connectedPasser, -57), // TODO: check this after tuning

        P(safePasser, 162), // no blocker and totally safe passage

        P(blocker_0, -103), // stop square
        P(blocker_1, -80),
        P(blocker_2, -25),
        P(blocker_3, 8),
        P(blocker_4, 2), // -furtest square

        P(blockedByOwn, 58), // TODO: check this after tuning
        P(blockedByOther, -73), // TODO: check this after tuning

        P(controller_0, -57), // stop square
        P(controller_1, -35),
        P(controller_2, 25),
        P(controller_3, 26),
        P(controller_4, 6), // -furtest square

        P(unstoppablePasser, 443),

        P(kingToPasser, 37),
        P(kingToOwnPasser, -41),

        P(candidateByRank_0, 36),
        P(candidateByRank_1, 53),
        P(candidateByRank_2, 95),
        P(candidateByRank_3, 137),
        P(candidateByRank_4, 119),

        P(candidateA, -25),
        P(candidateB, -76),
        P(candidateC, -54),
        P(candidateD, -81),
        P(candidateE, -8),
        P(candidateF, 2),
        P(candidateG, -14),
        P(candidateH, 29),

        /*
         *  Knights
         */

        // Kings on same side
        P(knightByFile_0, -176), // fileA
        P(knightByFile_1, -207),
        P(knightByFile_2, -221),
        P(knightByFile_3, -159),
        P(knightByFile_4, -96),
        P(knightByFile_5, -25),
        P(knightByFile_6, 22), // -fileH

        // Opposite side kings
        P(knightByFile_0x, -137), // fileA
        P(knightByFile_1x, -168),
        P(knightByFile_2x, -140),
        P(knightByFile_3x, -44),
        P(knightByFile_4x, 39),
        P(knightByFile_5x, 127),
        P(knightByFile_6x, 108), // -fileH

        P(knightByRank_0, -121), // rank1
        P(knightByRank_1, -205),
        P(knightByRank_2, -229),
        P(knightByRank_3, -182),
        P(knightByRank_4, -82),
        P(knightByRank_5, 64),
        P(knightByRank_6, 124), // -rank8

        P(knightAndSpan_0, -136), // span0 (pawnless)
        P(knightAndSpan_1, -91),
        P(knightAndSpan_2, -23),
        P(knightAndSpan_3, 27), // -span4 (7-8 files)

        P(knightVsSpan_0, -311), // span0 (pawnless)
        P(knightVsSpan_1, -208),
        P(knightVsSpan_2, -82),
        P(knightVsSpan_3, -2), // -span4 (7-8 files)

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
        P(bishopBySquare_0, -148),
        P(bishopBySquare_1, 23),
        P(bishopBySquare_2, -50),
        P(bishopBySquare_3, 108),
        P(bishopBySquare_4, -35),
        P(bishopBySquare_5, -2),
        P(bishopBySquare_6, -70),
        P(bishopBySquare_7, 70),

        P(bishopBySquare_8, -22),
        P(bishopBySquare_9, -21),
        P(bishopBySquare_10, 112),
        P(bishopBySquare_11, 10),
        P(bishopBySquare_12, 47),
        P(bishopBySquare_13, 124),
        P(bishopBySquare_14, 88),
        P(bishopBySquare_15, 138),

        P(bishopBySquare_16, -102),
        P(bishopBySquare_17, 70),
        P(bishopBySquare_18, 50),
        P(bishopBySquare_19, 65),
        P(bishopBySquare_20, 59),
        P(bishopBySquare_21, 109),
        P(bishopBySquare_22, 78),
        P(bishopBySquare_23, 110),

        P(bishopBySquare_24, -62),
        P(bishopBySquare_25, -51),
        P(bishopBySquare_26, 68),
        P(bishopBySquare_27, 109),
        P(bishopBySquare_28, 170),
        P(bishopBySquare_29, 130),
        P(bishopBySquare_30, 106),
        P(bishopBySquare_31, 105),

        P(bishopBySquare_32, -54),
        P(bishopBySquare_33, 21),
        P(bishopBySquare_34, 25),
        P(bishopBySquare_35, 98),
        P(bishopBySquare_36, 123),
        P(bishopBySquare_37, 156),
        P(bishopBySquare_38, 43),
        P(bishopBySquare_39, 101),

        P(bishopBySquare_40, -76),
        P(bishopBySquare_41, 9),
        P(bishopBySquare_42, 66),
        P(bishopBySquare_43, 6),
        P(bishopBySquare_44, 126),
        P(bishopBySquare_45, 253),
        P(bishopBySquare_46, 61),
        P(bishopBySquare_47, 47),

        P(bishopBySquare_48, -55),
        P(bishopBySquare_49, 47),
        P(bishopBySquare_50, 50),
        P(bishopBySquare_51, 64),
        P(bishopBySquare_52, 8),
        P(bishopBySquare_53, 175),
        P(bishopBySquare_54, -42),
        P(bishopBySquare_55, 49),

        P(bishopBySquare_56, -222),
        P(bishopBySquare_57, -58),
        P(bishopBySquare_58, -6),
        P(bishopBySquare_59, 40),
        P(bishopBySquare_60, -8),
        P(bishopBySquare_61, 129),
        P(bishopBySquare_62, -233),
        P(bishopBySquare_63, -58),

        P(bishopBySquare_0x, -72),
        P(bishopBySquare_1x, -12),
        P(bishopBySquare_2x, 50),
        P(bishopBySquare_3x, 3),
        P(bishopBySquare_4x, 38),
        P(bishopBySquare_5x, 94),
        P(bishopBySquare_6x, -219),
        P(bishopBySquare_7x, -74),

        P(bishopBySquare_8x, -11),
        P(bishopBySquare_9x, -35),
        P(bishopBySquare_10x, 33),
        P(bishopBySquare_11x, 106),
        P(bishopBySquare_12x, 64),
        P(bishopBySquare_13x, 179),
        P(bishopBySquare_14x, 73),
        P(bishopBySquare_15x, 47),

        P(bishopBySquare_16x, -79),
        P(bishopBySquare_17x, 25),
        P(bishopBySquare_18x, 79),
        P(bishopBySquare_19x, 86),
        P(bishopBySquare_20x, 164),
        P(bishopBySquare_21x, 172),
        P(bishopBySquare_22x, 133),
        P(bishopBySquare_23x, 65),

        P(bishopBySquare_24x, -47),
        P(bishopBySquare_25x, 21),
        P(bishopBySquare_26x, 92),
        P(bishopBySquare_27x, 131),
        P(bishopBySquare_28x, 172),
        P(bishopBySquare_29x, 218),
        P(bishopBySquare_30x, 78),
        P(bishopBySquare_31x, 149),

        P(bishopBySquare_32x, -23),
        P(bishopBySquare_33x, 91),
        P(bishopBySquare_34x, 125),
        P(bishopBySquare_35x, 181),
        P(bishopBySquare_36x, 218),
        P(bishopBySquare_37x, 205),
        P(bishopBySquare_38x, 179),
        P(bishopBySquare_39x, 147),

        P(bishopBySquare_40x, -58),
        P(bishopBySquare_41x, 110),
        P(bishopBySquare_42x, 153),
        P(bishopBySquare_43x, 186),
        P(bishopBySquare_44x, 179),
        P(bishopBySquare_45x, 197),
        P(bishopBySquare_46x, 175),
        P(bishopBySquare_47x, 184),

        P(bishopBySquare_48x, 15),
        P(bishopBySquare_49x, 160),
        P(bishopBySquare_50x, 162),
        P(bishopBySquare_51x, 96),
        P(bishopBySquare_52x, 161),
        P(bishopBySquare_53x, 181),
        P(bishopBySquare_54x, 182),
        P(bishopBySquare_55x, 176),

        P(bishopBySquare_56x, -114),
        P(bishopBySquare_57x, 92),
        P(bishopBySquare_58x, 48),
        P(bishopBySquare_59x, 124),
        P(bishopBySquare_60x, 95),
        P(bishopBySquare_61x, 63),
        P(bishopBySquare_62x, -20),
        P(bishopBySquare_63x, 10),
#endif

        P(bishopAndSpan_0, -197), // span0 (pawnless)
        P(bishopAndSpan_1, -156),
        P(bishopAndSpan_2, -99),
        P(bishopAndSpan_3, -61), // -span4 (7-8 files)

        P(bishopVsSpan_0, -404), // span0 (pawnless)
        P(bishopVsSpan_1, -289),
        P(bishopVsSpan_2, -172),
        P(bishopVsSpan_3, -87), // -span4 (7-8 files)

        P(bishopAndLikePawn_1, -42),
        P(bishopAndLikePawn_2, -3),
        P(bishopAndLikeRammedPawn, -61),

        P(bishopVsLikePawn_1, -26),
        P(bishopVsLikePawn_2, -6),
        P(bishopVsLikeRammedPawn, 6),

        /*
         *  Rooks
         */

        P(rookByFile_0, -64), // fileA
        P(rookByFile_1, -61),
        P(rookByFile_2, -28), // -fileD

        P(rookByRank_0, -102), // rank1
        P(rookByRank_1, -268),
        P(rookByRank_2, -410),
        P(rookByRank_3, -490),
        P(rookByRank_4, -438),
        P(rookByRank_5, -306),
        P(rookByRank_6, -148), // -rank8

        P(rookOnHalfOpen_0, 155), // fileA
        P(rookOnHalfOpen_1, 70),
        P(rookOnHalfOpen_2, 17),
        P(rookOnHalfOpen_3, 12), // fileD

        P(rookOnOpen_0, 191), // fileA
        P(rookOnOpen_1, 149),
        P(rookOnOpen_2, 106),
        P(rookOnOpen_3, 115), // fileD

        P(rookToWeakPawn_0, 145),
        P(rookToWeakPawn_1, 138),
        P(rookToWeakPawn_2, 121),
        P(rookToWeakPawn_3, 101),

        P(rookToKing_0, 255), // d=0
        P(rookToKing_1, 156), // d=1
        P(rookToKing_2, 101), // d=2
        P(rookToKing_3, 53),
        P(rookToKing_4, 3),
        P(rookToKing_5, -81),
        P(rookToKing_6, -129),
        P(rookToKing_7, -138),

        /*
         *  Queens
         */

        P(queenByFile_0, -38), // fileA
        P(queenByFile_1, -46),
        P(queenByFile_2, -31), // -fileD

        P(queenByRank_0, -104), // rank1
        P(queenByRank_1, -178),
        P(queenByRank_2, -249),
        P(queenByRank_3, -290),
        P(queenByRank_4, -259),
        P(queenByRank_5, -189),
        P(queenByRank_6, -123), // -rank8

        P(queenToKing, -19),
        P(queenToOwnKing, -10),
        P(rookToKing, -13),
        P(rookToOwnKing, 3),
        P(bishopToKing, -1),
        P(bishopToOwnKing, -5),
        P(knightToKing, -10),
        P(knightToOwnKing, -6),

        /*
         *  Kings
         */

        P(kingByFile_0, -50), // fileA
        P(kingByFile_1, -51),
        P(kingByFile_2, -36), // -fileD

        P(kingByRank_0, -262), // rank1
        P(kingByRank_1, -429),
        P(kingByRank_2, -578),
        P(kingByRank_3, -637),
        P(kingByRank_4, -542),
        P(kingByRank_5, -307),
        P(kingByRank_6, -68), // -rank8

        // Drawness. Positive is more drawish. Negative is more sharp
        P(drawOffset, 128),
        P(drawQueen, 111),
        P(drawRook, 14),
        P(drawBishop, 1),
        P(drawKnight, 21),
        P(drawPawn, -838),
        P(drawQueenEnding, 6),
        P(drawRookEnding, 741),
        P(drawKnightEnding, 751),
        P(drawBishopEnding, 1881),
        P(drawPawnEnding, -573),
        P(drawPawnless, 152),

        P(drawQueenImbalance, -792),
        P(drawRookImbalance, 453),
        P(drawMinorImbalance, 45),

        P(drawUnlikeBishops, 2298),
        P(drawUnlikeBishopsAndQueens, 772),
        P(drawUnlikeBishopsAndRooks, 432),
        P(drawUnlikeBishopsAndKnights, 705),

        P(drawRammed_0, 572), // fileA/H
        P(drawRammed_1, 522),
        P(drawRammed_2, 577),
        P(drawRammed_3, 448), // fileD/E
// }

/*----------------------------------------------------------------------+
 |                                                                      |
 +----------------------------------------------------------------------*/

