
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
        P(winBonus, 1497),

        P(castleK, -133),
        P(castleQ, -120),
        P(castleKQ, -68),

        P(queenValue, 6980),
        P(rookValue, 4687),
        P(bishopValue, 2626),
        P(knightValue, 2464),
        P(pawnValue1, 1001),
        P(pawnValue2, 1676),
        P(pawnValue3, 2351),
        P(pawnValue4, 3032),
        P(pawnValue5, 3697),
        P(pawnValue6, 4344),
        P(pawnValue7, 4960),
        P(pawnValue8, 5476),

        P(queenAndQueen, -718),
        P(queenAndRook, -725),
        P(queenAndBishop, 100),
        P(queenAndKnight, 35),
        P(queenAndPawn_1, -22),
        P(queenAndPawn_2, -164),

        P(rookAndRook, -294),
        P(rookAndBishop, -110),
        P(rookAndKnight, -105),
        P(rookAndPawn_1, -54),
        P(rookAndPawn_2, -59),

        P(bishopAndBishop, 115),
        P(bishopAndKnight, 12),
        P(bishopAndPawn_1, -48),
        P(bishopAndPawn_2, 4),

        P(knightAndKnight, -24),
        P(knightAndPawn_1, -20),
        P(knightAndPawn_2, -2),

        P(queenVsRook, -362),
        P(queenVsBishop, 84),
        P(queenVsKnight, 23),
        P(queenVsPawn_1, 172),
        P(queenVsPawn_2, -168),

        P(rookVsBishop, -17),
        P(rookVsKnight, 16),
        P(rookVsPawn_1, -3),
        P(rookVsPawn_2, -49),

        P(bishopVsKnight, 5),
        P(bishopVsPawn_1, -10),
        P(bishopVsPawn_2, 3),

        P(knightVsPawn_1, -2),
        P(knightVsPawn_2, -18),

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
        P(safetyAndQueen, -1834),
        P(safetyAndRook, 3209),
        P(safetyAndBishop, 1889),
        P(safetyAndKnight, 1943),
        P(safetyAndPawn, -490),
        P(safetyVsQueen, 12715),
        P(safetyVsRook, -3733),
        P(safetyVsBishop, -2437),
        P(safetyVsKnight, -2184),
        P(safetyVsPawn, -832),

        // shelter
        P(shelterPawn_0, 138), // rank3
        P(shelterPawn_1, 261), // rank4
        P(shelterPawn_2, 270), // rank5
        P(shelterPawn_3, 107), // rank6
        P(shelterPawn_4, -217), // rank7
        P(shelterPawn_5, 291), // no pawn

        P(shelterKing_0, -302), // fileA (fileH)
        P(shelterKing_1, -318),
        P(shelterKing_2, -194), // -fileD (-fileE)

        P(shelterWalkingKing, 45), // rank2 or up
        P(shelterCastled, 63), // fraction of 256

        // attacks
        P(attackSquares_0, 84), // 0 attacks
        P(attackSquares_1, -23),
        P(attackSquares_2, -145),
        P(attackSquares_3, -245),
        P(attackSquares_4, -295),
        P(attackSquares_5, -253), // -(6 or more attacks)

        P(attackByPawn_0, 118),
        P(attackByPawn_1, 295),
        P(attackByPawn_2, 96),
        P(attackByMinor_0, -110),
        P(attackByMinor_1, 16),
        P(attackByMinor_2, -221),
        P(attackByRook_0, 466),
        P(attackByRook_1, 997),
        P(attackByRook_2, -374),
        P(attackByQueen, 195),
        P(attackByKing, -250),

        P(attackPieces_0, -9),
        P(attackPieces_1, 12),
        P(attackPieces_2, -50),
        P(attackPieces_3, -48),
        P(attackPieces_4, 45),

        // TODO: pawn storm

        // king mobility
        P(mobilityKing_0, 94),
        P(mobilityKing_1, 43),

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

        P(pawnByFile_0, 16), // fileA
        P(pawnByFile_1, 41),
        P(pawnByFile_2, 9),
        P(pawnByFile_3, -39),
        P(pawnByFile_4, -72),
        P(pawnByFile_5, -27),
        P(pawnByFile_6, 32), // -fileH

        P(pawnByFile_0x, -115), // fileA
        P(pawnByFile_1x, -206),
        P(pawnByFile_2x, -256),
        P(pawnByFile_3x, -330),
        P(pawnByFile_4x, -340),
        P(pawnByFile_5x, -272),
        P(pawnByFile_6x, -113), // -fileH

        // 5 parameters for 6 pawn ranks
        P(pawnByRank_0, -51), // rank2
        P(pawnByRank_1, -93),
        P(pawnByRank_2, -89),
        P(pawnByRank_3, -15),
        P(pawnByRank_4, 59), // -rank7

        P(doubledPawnA, -164),
        P(doubledPawnB, -88),
        P(doubledPawnC, -118),
        P(doubledPawnD, -78),
        P(doubledPawnE, -68),
        P(doubledPawnF, -20),
        P(doubledPawnG, 5),
        P(doubledPawnH, -173),

        P(backwardPawnClosedByRank_0, 23),
        P(backwardPawnClosedByRank_1, 15),
        P(backwardPawnClosedByRank_2, -13),
        P(backwardPawnClosedByRank_3, -7),
        P(backwardPawnClosedByRank_4, 0),

        P(backwardPawnOpenByRank_0, 32),
        P(backwardPawnOpenByRank_1, 21),
        P(backwardPawnOpenByRank_2, 9),
        P(backwardPawnOpenByRank_3, 43),
        P(backwardPawnOpenByRank_4, 0),

        P(backwardPawnClosedA, 58),
        P(backwardPawnClosedB, -6),
        P(backwardPawnClosedC, 23),
        P(backwardPawnClosedD, -35),
        P(backwardPawnClosedE, -50),
        P(backwardPawnClosedF, -18),
        P(backwardPawnClosedG, -3),
        P(backwardPawnClosedH, 55),

        P(backwardPawnOpenA, -12),
        P(backwardPawnOpenB, -38),
        P(backwardPawnOpenC, -23),
        P(backwardPawnOpenD, -48),
        P(backwardPawnOpenE, -35),
        P(backwardPawnOpenF, -19),
        P(backwardPawnOpenG, -39),
        P(backwardPawnOpenH, 19),

        P(rammedWeakPawnA, -14),
        P(rammedWeakPawnB, 8),
        P(rammedWeakPawnC, 2),
        P(rammedWeakPawnD, 3),
        P(rammedWeakPawnE, 26),
        P(rammedWeakPawnF, 9),
        P(rammedWeakPawnG, -10),
        P(rammedWeakPawnH, -49),

        P(isolatedPawnClosedA, 124),
        P(isolatedPawnClosedB, 36),
        P(isolatedPawnClosedC, -27),
        P(isolatedPawnClosedD, -16),
        P(isolatedPawnClosedE, -100),
        P(isolatedPawnClosedF, -100),
        P(isolatedPawnClosedG, -100),
        P(isolatedPawnClosedH, -14),

        P(isolatedPawnOpenA, 120),
        P(isolatedPawnOpenB, 55),
        P(isolatedPawnOpenC, -26),
        P(isolatedPawnOpenD, -58),
        P(isolatedPawnOpenE, -71),
        P(isolatedPawnOpenF, -27),
        P(isolatedPawnOpenG, 40),
        P(isolatedPawnOpenH, 75),

        P(sidePawnClosedA, 99),
        P(sidePawnClosedB, 43),
        P(sidePawnClosedC, 58),
        P(sidePawnClosedD, -44),
        P(sidePawnClosedE, -52),
        P(sidePawnClosedF, -59),
        P(sidePawnClosedG, -103),
        P(sidePawnClosedH, -12),

        P(sidePawnOpenA, 80),
        P(sidePawnOpenB, 46),
        P(sidePawnOpenC, 24),
        P(sidePawnOpenD, -25),
        P(sidePawnOpenE, -17),
        P(sidePawnOpenF, 7),
        P(sidePawnOpenG, 44),
        P(sidePawnOpenH, 42),

        P(middlePawnClosedA, 0),
        P(middlePawnClosedB, 17),
        P(middlePawnClosedC, 94),
        P(middlePawnClosedD, -24),
        P(middlePawnClosedE, -17),
        P(middlePawnClosedF, -9),
        P(middlePawnClosedG, -86),
        P(middlePawnClosedH, 0),

        P(middlePawnOpenA, 0),
        P(middlePawnOpenB, -69),
        P(middlePawnOpenC, 25),
        P(middlePawnOpenD, -1),
        P(middlePawnOpenE, 18),
        P(middlePawnOpenF, -23),
        P(middlePawnOpenG, -43),
        P(middlePawnOpenH, 0),

        P(duoPawnA, 7),
        P(duoPawnB, -5),
        P(duoPawnC, -2),
        P(duoPawnD, 24),
        P(duoPawnE, 28),
        P(duoPawnF, 14),
        P(duoPawnG, 37),
        P(duoPawnH, -64),

        P(openFilePawn_0, -25),
        P(openFilePawn_1, -28),
        P(openFilePawn_2, 37),
        P(openFilePawn_3, 30),
        P(openFilePawn_4, 135),
        P(openFilePawn_5, 248),

        P(stoppedPawn_0, -38),
        P(stoppedPawn_1, 27),
        P(stoppedPawn_2, 25),
        P(stoppedPawn_3, -13),
        P(stoppedPawn_4, 0),
        P(stoppedPawn_5, 0),

        P(duoPawn_0, 3),
        P(duoPawn_1, -3),
        P(duoPawn_2, 27),
        P(duoPawn_3, 144),
        P(duoPawn_4, 498),
        P(duoPawn_5, 1468),

        P(trailingPawn_0, -11),
        P(trailingPawn_1, -47),
        P(trailingPawn_2, -69),
        P(trailingPawn_3, -44),
        P(trailingPawn_4, -17),
        P(trailingPawn_5, -46),

        P(capturePawn_0, 118),
        P(capturePawn_1, -71),
        P(capturePawn_2, -86),
        P(capturePawn_3, 29),
        P(capturePawn_4, 299),
        P(capturePawn_5, 0),

        P(defendedPawn_0, 0),
        P(defendedPawn_1, 64),
        P(defendedPawn_2, 9),
        P(defendedPawn_3, 117),
        P(defendedPawn_4, 248),
        P(defendedPawn_5, 245),

        P(pawnLever_0, 0),
        P(pawnLever_1, 0),
        P(pawnLever_2, -32),
        P(pawnLever_3, -37),
        P(pawnLever_4, -32),
        P(pawnLever_5, -24),
        P(pawnLever_6, 0),

        P(rammedPawn_0, -44),
        P(rammedPawn_1, -6),
        P(rammedPawn_2, 0),
        P(rammedPawn_3, -55),
        P(rammedPawn_4, 45),
        P(rammedPawn_5, 0),

        P(mobilePawn_0, 10),
        P(mobilePawn_1, 25),
        P(mobilePawn_2, 30),
        P(mobilePawn_3, 43),
        P(mobilePawn_4, 83),
        P(mobilePawn_5, 217),

        // Quadratic polynomials for passers
        P(passerA_0, 29),
        P(passerB_0, 75),
        P(passerC_0, 144),
        P(passerD_0, 178),
        P(passerE_0, 227),
        P(passerF_0, -8),
        P(passerG_0, -79),
        P(passerH_0, -122),

        P(passerA_1, 70),
        P(passerB_1, 18),
        P(passerC_1, -43),
        P(passerD_1, -61),
        P(passerE_1, -84),
        P(passerF_1, 72),
        P(passerG_1, 111),
        P(passerH_1, 156),

        P(passerA_2, 148),
        P(passerB_2, 187),
        P(passerC_2, 237),
        P(passerD_2, 243),
        P(passerE_2, 271),
        P(passerF_2, 146),
        P(passerG_2, 110),
        P(passerH_2, 62),

        P(passerScalingOffset, 3758),
        P(passerAndQueen, 1336),
        P(passerAndRook, 776),
        P(passerAndBishop, 699),
        P(passerAndKnight, 342),
        P(passerAndPawn, -574),
        P(passerVsQueen, -696),
        P(passerVsRook, -736),
        P(passerVsBishop, -1283),
        P(passerVsKnight, -875),
        P(passerVsPawn, 250),

        P(protectedPasser, -21), // TODO: check this after tuning
        P(connectedPasser, -59), // TODO: check this after tuning

        P(safePasser, 184), // no blocker and totally safe passage

        P(blocker_0, -113), // stop square
        P(blocker_1, -87),
        P(blocker_2, -33),
        P(blocker_3, -5),
        P(blocker_4, -14), // -furtest square

        P(blockedByOwn, 26), // TODO: check this after tuning
        P(blockedByOther, -135), // TODO: check this after tuning

        P(controller_0, -75), // stop square
        P(controller_1, -60),
        P(controller_2, -8),
        P(controller_3, -5),
        P(controller_4, -14), // -furtest square

        P(unstoppablePasser, 405),

        P(kingToPasser, 47),
        P(kingToOwnPasser, -61),

        P(candidateByRank_0, 31),
        P(candidateByRank_1, 66),
        P(candidateByRank_2, 98),
        P(candidateByRank_3, 153),
        P(candidateByRank_4, 177),

        P(candidateA, -19),
        P(candidateB, -79),
        P(candidateC, -60),
        P(candidateD, -81),
        P(candidateE, -15),
        P(candidateF, -10),
        P(candidateG, -27),
        P(candidateH, 23),

        /*
         *  Knights
         */

        // Kings on same side
        P(knightByFile_0, -158), // fileA
        P(knightByFile_1, -181),
        P(knightByFile_2, -193),
        P(knightByFile_3, -134),
        P(knightByFile_4, -79),
        P(knightByFile_5, -14),
        P(knightByFile_6, 27), // -fileH

        // Opposite side kings
        P(knightByFile_0x, -131), // fileA
        P(knightByFile_1x, -164),
        P(knightByFile_2x, -137),
        P(knightByFile_3x, -46),
        P(knightByFile_4x, 33),
        P(knightByFile_5x, 119),
        P(knightByFile_6x, 105), // -fileH

        P(knightByRank_0, -110), // rank1
        P(knightByRank_1, -196),
        P(knightByRank_2, -227),
        P(knightByRank_3, -189),
        P(knightByRank_4, -99),
        P(knightByRank_5, 44),
        P(knightByRank_6, 109), // -rank8

        P(knightAndSpan_0, -176), // span0 (pawnless)
        P(knightAndSpan_1, -127),
        P(knightAndSpan_2, -47),
        P(knightAndSpan_3, 21), // -span4 (7-8 files)

        P(knightVsSpan_0, -332), // span0 (pawnless)
        P(knightVsSpan_1, -206),
        P(knightVsSpan_2, -76),
        P(knightVsSpan_3, 7), // -span4 (7-8 files)

        P(knightToKing, -19),
        P(knightToOwnKing, -12),

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
        P(bishopBySquare_0, -133),
        P(bishopBySquare_1, 36),
        P(bishopBySquare_2, -36),
        P(bishopBySquare_3, 130),
        P(bishopBySquare_4, -22),
        P(bishopBySquare_5, 19),
        P(bishopBySquare_6, -45),
        P(bishopBySquare_7, 97),

        P(bishopBySquare_8, 0),
        P(bishopBySquare_9, -7),
        P(bishopBySquare_10, 129),
        P(bishopBySquare_11, 29),
        P(bishopBySquare_12, 64),
        P(bishopBySquare_13, 137),
        P(bishopBySquare_14, 93),
        P(bishopBySquare_15, 165),

        P(bishopBySquare_16, -92),
        P(bishopBySquare_17, 77),
        P(bishopBySquare_18, 61),
        P(bishopBySquare_19, 81),
        P(bishopBySquare_20, 81),
        P(bishopBySquare_21, 125),
        P(bishopBySquare_22, 104),
        P(bishopBySquare_23, 153),

        P(bishopBySquare_24, -58),
        P(bishopBySquare_25, -39),
        P(bishopBySquare_26, 73),
        P(bishopBySquare_27, 124),
        P(bishopBySquare_28, 186),
        P(bishopBySquare_29, 137),
        P(bishopBySquare_30, 141),
        P(bishopBySquare_31, 139),

        P(bishopBySquare_32, -63),
        P(bishopBySquare_33, 21),
        P(bishopBySquare_34, 29),
        P(bishopBySquare_35, 104),
        P(bishopBySquare_36, 139),
        P(bishopBySquare_37, 173),
        P(bishopBySquare_38, 66),
        P(bishopBySquare_39, 129),

        P(bishopBySquare_40, -77),
        P(bishopBySquare_41, 6),
        P(bishopBySquare_42, 70),
        P(bishopBySquare_43, 20),
        P(bishopBySquare_44, 147),
        P(bishopBySquare_45, 274),
        P(bishopBySquare_46, 82),
        P(bishopBySquare_47, 60),

        P(bishopBySquare_48, -54),
        P(bishopBySquare_49, 45),
        P(bishopBySquare_50, 52),
        P(bishopBySquare_51, 72),
        P(bishopBySquare_52, 24),
        P(bishopBySquare_53, 190),
        P(bishopBySquare_54, 2),
        P(bishopBySquare_55, 80),

        P(bishopBySquare_56, -224),
        P(bishopBySquare_57, -63),
        P(bishopBySquare_58, 0),
        P(bishopBySquare_59, 53),
        P(bishopBySquare_60, 22),
        P(bishopBySquare_61, 153),
        P(bishopBySquare_62, -214),
        P(bishopBySquare_63, -14),

        P(bishopBySquare_0x, -59),
        P(bishopBySquare_1x, 1),
        P(bishopBySquare_2x, 68),
        P(bishopBySquare_3x, 33),
        P(bishopBySquare_4x, 55),
        P(bishopBySquare_5x, 124),
        P(bishopBySquare_6x, -188),
        P(bishopBySquare_7x, -55),

        P(bishopBySquare_8x, -48),
        P(bishopBySquare_9x, -22),
        P(bishopBySquare_10x, 51),
        P(bishopBySquare_11x, 135),
        P(bishopBySquare_12x, 87),
        P(bishopBySquare_13x, 215),
        P(bishopBySquare_14x, 110),
        P(bishopBySquare_15x, 87),

        P(bishopBySquare_16x, -67),
        P(bishopBySquare_17x, 35),
        P(bishopBySquare_18x, 114),
        P(bishopBySquare_19x, 102),
        P(bishopBySquare_20x, 199),
        P(bishopBySquare_21x, 221),
        P(bishopBySquare_22x, 169),
        P(bishopBySquare_23x, 102),

        P(bishopBySquare_24x, -42),
        P(bishopBySquare_25x, 28),
        P(bishopBySquare_26x, 101),
        P(bishopBySquare_27x, 147),
        P(bishopBySquare_28x, 191),
        P(bishopBySquare_29x, 240),
        P(bishopBySquare_30x, 146),
        P(bishopBySquare_31x, 201),

        P(bishopBySquare_32x, -8),
        P(bishopBySquare_33x, 95),
        P(bishopBySquare_34x, 138),
        P(bishopBySquare_35x, 186),
        P(bishopBySquare_36x, 235),
        P(bishopBySquare_37x, 223),
        P(bishopBySquare_38x, 238),
        P(bishopBySquare_39x, 185),

        P(bishopBySquare_40x, -54),
        P(bishopBySquare_41x, 106),
        P(bishopBySquare_42x, 164),
        P(bishopBySquare_43x, 199),
        P(bishopBySquare_44x, 193),
        P(bishopBySquare_45x, 222),
        P(bishopBySquare_46x, 193),
        P(bishopBySquare_47x, 218),

        P(bishopBySquare_48x, 21),
        P(bishopBySquare_49x, 162),
        P(bishopBySquare_50x, 173),
        P(bishopBySquare_51x, 107),
        P(bishopBySquare_52x, 178),
        P(bishopBySquare_53x, 208),
        P(bishopBySquare_54x, 210),
        P(bishopBySquare_55x, 216),

        P(bishopBySquare_56x, -109),
        P(bishopBySquare_57x, 56),
        P(bishopBySquare_58x, 54),
        P(bishopBySquare_59x, 119),
        P(bishopBySquare_60x, 107),
        P(bishopBySquare_61x, 84),
        P(bishopBySquare_62x, 15),
        P(bishopBySquare_63x, 28),
#endif

        P(bishopAndSpan_0, -206), // span0 (pawnless)
        P(bishopAndSpan_1, -159),
        P(bishopAndSpan_2, -107),
        P(bishopAndSpan_3, -59), // -span4 (7-8 files)

        P(bishopVsSpan_0, -394), // span0 (pawnless)
        P(bishopVsSpan_1, -272),
        P(bishopVsSpan_2, -159),
        P(bishopVsSpan_3, -75), // -span4 (7-8 files)

        P(bishopAndLikePawn_1, -47),
        P(bishopAndLikePawn_2, -3),
        P(bishopAndLikeRammedPawn, -58),

        P(bishopVsLikePawn_1, -26),
        P(bishopVsLikePawn_2, -6),
        P(bishopVsLikeRammedPawn, 5),

        P(bishopToKing, 0),
        P(bishopToOwnKing, -10),

        /*
         *  Rooks
         */

        P(rookByFile_0, -68), // fileA
        P(rookByFile_1, -63),
        P(rookByFile_2, -28), // -fileD

        P(rookByRank_0, -86), // rank1
        P(rookByRank_1, -242),
        P(rookByRank_2, -380),
        P(rookByRank_3, -461),
        P(rookByRank_4, -418),
        P(rookByRank_5, -301),
        P(rookByRank_6, -152), // -rank8

        P(rookOnHalfOpen_0, 137), // fileA
        P(rookOnHalfOpen_1, 49),
        P(rookOnHalfOpen_2, 6),
        P(rookOnHalfOpen_3, -2), // fileD

        P(rookOnOpen_0, 226), // fileA
        P(rookOnOpen_1, 169),
        P(rookOnOpen_2, 110),
        P(rookOnOpen_3, 123), // fileD

        P(rookToWeakPawn_0, 158),
        P(rookToWeakPawn_1, 141),
        P(rookToWeakPawn_2, 116),
        P(rookToWeakPawn_3, 86),

        P(rookToKing_0, 301), // d=0
        P(rookToKing_1, 193), // d=1
        P(rookToKing_2, 134), // d=2
        P(rookToKing_3, 84),
        P(rookToKing_4, 29),
        P(rookToKing_5, -52),
        P(rookToKing_6, -102),
        P(rookToKing_7, -131),

        P(rookToKing, -15),
        P(rookToOwnKing, 2),

        P(rookInFrontPasser_0, -68), // opponent's
        P(rookInFrontPasser_1, 137), // own passer
        P(rookBehindPasser_0, 377), // opponent's
        P(rookBehindPasser_1, 57), // own passer

        /*
         *  Queens
         */

        P(queenByFile_0, -32), // fileA
        P(queenByFile_1, -40),
        P(queenByFile_2, -27), // -fileD

        P(queenByRank_0, -87), // rank1
        P(queenByRank_1, -149),
        P(queenByRank_2, -214),
        P(queenByRank_3, -254),
        P(queenByRank_4, -233),
        P(queenByRank_5, -174),
        P(queenByRank_6, -117), // -rank8

        P(queenToKing, -23),
        P(queenToOwnKing, -11),

        P(queenInFrontPasser_0, -87), // opponent's
        P(queenInFrontPasser_1, 20), // own passer
        P(queenBehindPasser_0, 166), // opponent's
        P(queenBehindPasser_1, -7), // own passer

        /*
         *  Kings
         */

        P(kingByFile_0, -37), // fileA
        P(kingByFile_1, -34),
        P(kingByFile_2, -24), // -fileD

        P(kingByRank_0, -264), // rank1
        P(kingByRank_1, -436),
        P(kingByRank_2, -599),
        P(kingByRank_3, -681),
        P(kingByRank_4, -602),
        P(kingByRank_5, -374),
        P(kingByRank_6, -119), // -rank8

        // Drawness. Positive is more drawish. Negative is more sharp
        P(drawOffset, 301),
        P(drawQueen, 106),
        P(drawRook, -18),
        P(drawBishop, -28),
        P(drawKnight, -12),
        P(drawPawn, -843),
        P(drawQueenEnding, -106),
        P(drawRookEnding, 643),
        P(drawKnightEnding, 681),
        P(drawBishopEnding, 1762),
        P(drawPawnEnding, -686),
        P(drawPawnless, 77),

        P(drawQueenImbalance, -832),
        P(drawRookImbalance, 420),
        P(drawMinorImbalance, 24),

        P(drawUnlikeBishops, 2361),
        P(drawUnlikeBishopsAndQueens, 870),
        P(drawUnlikeBishopsAndRooks, 414),
        P(drawUnlikeBishopsAndKnights, 698),

        P(drawRammed_0, 573), // fileA/H
        P(drawRammed_1, 534),
        P(drawRammed_2, 586),
        P(drawRammed_3, 473), // fileD/E
// }

/*----------------------------------------------------------------------+
 |                                                                      |
 +----------------------------------------------------------------------*/

