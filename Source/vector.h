
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
        P(tempo, 35),
        P(winBonus, 3417),

        P(castleK, 6),
        P(castleQ, -55),
        P(castleKQ, 43),

        P(queenValue, 7169),
        P(rookValue, 4195),
        P(bishopValue, 2317),
        P(knightValue, 2141),
        P(pawnValue1, 1068), P(pawnValue2, 1723), P(pawnValue3, 2358), P(pawnValue4, 3016),
        P(pawnValue5, 3679), P(pawnValue6, 4345), P(pawnValue7, 4965), P(pawnValue8, 5566),

        P(queenAndQueen, -952),
        P(queenAndRook, -661),
        P(queenAndBishop, 35),
        P(queenAndKnight, -65),
        P(queenAndPawn_1, -111), P(queenAndPawn_2, -85),
        P(rookAndRook, -313),
        P(rookAndBishop, -131),
        P(rookAndKnight, -164),
        P(rookAndPawn_1, -61), P(rookAndPawn_2, -62),
        P(bishopAndBishop, 65),
        P(bishopAndKnight, -20),
        P(bishopAndPawn_1, -56), P(bishopAndPawn_2, -19),
        P(knightAndKnight, -73),
        P(knightAndPawn_1, -27), P(knightAndPawn_2, -2),

        P(queenVsRook, -262),
        P(queenVsBishop, 92),
        P(queenVsKnight, -11),
        P(queenVsPawn_1, 53), P(queenVsPawn_2, -108),
        P(rookVsBishop, 4),
        P(rookVsKnight, 25),
        P(rookVsPawn_1, -22), P(rookVsPawn_2, -34),
        P(bishopVsKnight, 6),
        P(bishopVsPawn_1, -11), P(bishopVsPawn_2, -5),
        P(knightVsPawn_1, -1), P(knightVsPawn_2, -19),

        P(controlCenter, 38),
        P(controlExtendedCenter, 41),
        P(controlOutside, 27),

        /*
         *  king safety
         */

        // scaling
        P(safetyScalingOffset, -3525),
        P(safetyAndQueen, 1423),
        P(safetyAndRook, 1255),
        P(safetyAndBishop, 485),
        P(safetyAndKnight, 352),
        P(safetyAndPawn, -1229),
        P(safetyVsQueen, 15960),
        P(safetyVsRook, -1077),
        P(safetyVsBishop, -1478),
        P(safetyVsKnight, -1119),
        P(safetyVsPawn, -782),

        // shelter
        P(shelterPawn_0, 60), // rank3
        P(shelterPawn_1, 227), // rank4
        P(shelterPawn_2, 189), // rank5
        P(shelterPawn_3, 50), // rank6
        P(shelterPawn_4, -158), // rank7
        P(shelterPawn_5, 184), // no pawn

        P(shelterKing_0, 148), // fileA (fileH)
        P(shelterKing_1, -9),
        P(shelterKing_2, -24), // -fileD (-fileE)

        P(shelterWalkingKing, -11), // rank2 or up
        P(shelterCastled, -117), // fraction of 256

        // attacks
        P(attackSquares_0, 96), // 0 attacks
        P(attackSquares_1, 50),
        P(attackSquares_2, -49),
        P(attackSquares_3, -157),
        P(attackSquares_4, -198),
        P(attackSquares_5, -166), // -(6 or more attacks)

        P(attackByPawn_0, 10),
        P(attackByPawn_1, 32),
        P(attackByPawn_2, -61),
        P(attackByMinor_0, -114),
        P(attackByMinor_1, 4),
        P(attackByMinor_2, -331),
        P(attackByRook_0, -57),
        P(attackByRook_1, 59),
        P(attackByRook_2, -288),
        P(attackByQueen, 163),
        P(attackByKing, -201),

        // TODO: pawn storm

        // king mobility
        P(mobilityKing_0, 62),
        P(mobilityKing_1, 49),

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

        P(pawnByFile_0, 0), // fileA
        P(pawnByFile_1, -7),
        P(pawnByFile_2, -66),
        P(pawnByFile_3, -103),
        P(pawnByFile_4, -115),
        P(pawnByFile_5, -59),
        P(pawnByFile_6, 26), // -fileH

        P(pawnByFile_0x, -68), // fileA
        P(pawnByFile_1x, -101),
        P(pawnByFile_2x, -122),
        P(pawnByFile_3x, -175),
        P(pawnByFile_4x, -166),
        P(pawnByFile_5x, -117),
        P(pawnByFile_6x, -34), // -fileH

        // 5 parameters for 6 pawn ranks
        P(pawnByRank_0, -49), // rank2
        P(pawnByRank_1, -89),
        P(pawnByRank_2, -92),
        P(pawnByRank_3, -35),
        P(pawnByRank_4, 69), // -rank7

        P(doubledPawnA, -221), P(doubledPawnB, -140), P(doubledPawnC, -175), P(doubledPawnD, -123),
        P(doubledPawnE, -125), P(doubledPawnF, -41), P(doubledPawnG, -44), P(doubledPawnH, -211),

        P(backwardPawnClosedA, 0),
        P(backwardPawnClosedB, -36),
        P(backwardPawnClosedC, -53),
        P(backwardPawnClosedD, -106),
        P(backwardPawnClosedE, -104),
        P(backwardPawnClosedF, -48),
        P(backwardPawnClosedG, -29),
        P(backwardPawnClosedH, 4),

        P(backwardPawnOpenA, 0),
        P(backwardPawnOpenB, -36),
        P(backwardPawnOpenC, -53),
        P(backwardPawnOpenD, -106),
        P(backwardPawnOpenE, -104),
        P(backwardPawnOpenF, -48),
        P(backwardPawnOpenG, -29),
        P(backwardPawnOpenH, 4),

        P(rammedWeakPawnA, 0),
        P(rammedWeakPawnB, 0),
        P(rammedWeakPawnC, 0),
        P(rammedWeakPawnD, 0),
        P(rammedWeakPawnE, 0),
        P(rammedWeakPawnF, 0),
        P(rammedWeakPawnG, 0),
        P(rammedWeakPawnH, 0),

        P(isolatedPawnClosedA, 48), P(isolatedPawnClosedB, -53),
        P(isolatedPawnClosedC, -11), P(isolatedPawnClosedD, 19),
        P(isolatedPawnClosedE, -62), P(isolatedPawnClosedF, -80),
        P(isolatedPawnClosedG, -93), P(isolatedPawnClosedH, -6),

        P(isolatedPawnOpenA, 12), P(isolatedPawnOpenB, -7),
        P(isolatedPawnOpenC, -79), P(isolatedPawnOpenD, -136),
        P(isolatedPawnOpenE, -119), P(isolatedPawnOpenF, -69),
        P(isolatedPawnOpenG, -51), P(isolatedPawnOpenH, 30),

        P(sidePawnClosedA, 24), P(sidePawnClosedB, 16),
        P(sidePawnClosedC, 49), P(sidePawnClosedD, -2),
        P(sidePawnClosedE, -15), P(sidePawnClosedF, -12),
        P(sidePawnClosedG, -54), P(sidePawnClosedH, 9),

        P(sidePawnOpenA, 1), P(sidePawnOpenB, 44),
        P(sidePawnOpenC, 2), P(sidePawnOpenD, -38),
        P(sidePawnOpenE, -17), P(sidePawnOpenF, 10),
        P(sidePawnOpenG, 31), P(sidePawnOpenH, 29),

        P(middlePawnClosedA, 0), P(middlePawnClosedB, -3),
        P(middlePawnClosedC, 72), P(middlePawnClosedD, 17),
        P(middlePawnClosedE, 5), P(middlePawnClosedF, 0),
        P(middlePawnClosedG, -42), P(middlePawnClosedH, 0),

        P(middlePawnOpenA, 0), P(middlePawnOpenB, -6),
        P(middlePawnOpenC, 29), P(middlePawnOpenD, 36),
        P(middlePawnOpenE, 45), P(middlePawnOpenF, -19),
        P(middlePawnOpenG, -13), P(middlePawnOpenH, 0),

        P(duoPawnA, -6),
        P(duoPawnB, 3),
        P(duoPawnC, -18),
        P(duoPawnD, 66),
        P(duoPawnE, 35),
        P(duoPawnF, -1),
        P(duoPawnG, 47),
        P(duoPawnH, -76),

        P(openFilePawn_0, 0),
        P(openFilePawn_1, 0),
        P(openFilePawn_2, 0),
        P(openFilePawn_3, 0),
        P(openFilePawn_4, 0),
        P(openFilePawn_5, 0),

        P(stoppedPawn_0, 0),
        P(stoppedPawn_1, 0),
        P(stoppedPawn_2, 0),
        P(stoppedPawn_3, 0),
        P(stoppedPawn_4, 0),
        P(stoppedPawn_5, 0),

        P(duoPawn_0, 0),
        P(duoPawn_1, 0),
        P(duoPawn_2, 0),
        P(duoPawn_3, 0),
        P(duoPawn_4, 0),
        P(duoPawn_5, 0),

        P(trailingPawn_0, 0),
        P(trailingPawn_1, 0),
        P(trailingPawn_2, 0),
        P(trailingPawn_3, 0),
        P(trailingPawn_4, 0),
        P(trailingPawn_5, 0),

        P(capturePawn_0, 0),
        P(capturePawn_1, 0),
        P(capturePawn_2, 0),
        P(capturePawn_3, 0),
        P(capturePawn_4, 0),
        P(capturePawn_5, 0),

        P(defendedPawn_0, 0),
        P(defendedPawn_1, 0),
        P(defendedPawn_2, 0),
        P(defendedPawn_3, 0),
        P(defendedPawn_4, 0),
        P(defendedPawn_5, 0),

        P(pawnLever_0, 0),
        P(pawnLever_1, 0),
        P(pawnLever_2, 0),
        P(pawnLever_3, 0),
        P(pawnLever_4, 0),
        P(pawnLever_5, 0),
        P(pawnLever_6, 0),

        P(rammedPawn_0, 0),
        P(rammedPawn_1, 0),
        P(rammedPawn_2, 0),
        P(rammedPawn_3, 0),
        P(rammedPawn_4, 0),
        P(rammedPawn_5, 0),

        // Quadratic polynomials for passers
        P(passerA_0, -82), P(passerB_0, 7), P(passerC_0, 5), P(passerD_0, 12),
        P(passerE_0, 73), P(passerF_0, 80), P(passerG_0, 31), P(passerH_0, 21),

        P(passerA_1, 82), P(passerB_1, 9), P(passerC_1, -29), P(passerD_1, -10),
        P(passerE_1, -50), P(passerF_1, -55), P(passerG_1, 18), P(passerH_1, 47),

        P(passerA_2, 172), P(passerB_2, 224), P(passerC_2, 250), P(passerD_2, 212),
        P(passerE_2, 255), P(passerF_2, 272), P(passerG_2, 218), P(passerH_2, 166),

        P(passerScalingOffset, 4708),
        P(passerAndQueen, -79),
        P(passerAndRook, -35),
        P(passerAndBishop, 402),
        P(passerAndKnight, 80),
        P(passerAndPawn, -653),
        P(passerVsQueen, 869),
        P(passerVsRook, -71),
        P(passerVsBishop, -873),
        P(passerVsKnight, -571),
        P(passerVsPawn, 45),

#if 0
        P(drawBlockedA, 0), P(drawBlockedB, 0), P(drawBlockedC, 0), P(drawBlockedD, 0),
        P(drawBlockedE, 0), P(drawBlockedF, 0), P(drawBlockedG, 0), P(drawBlockedH, 0),

        P(drawPasser_0, 0), P(drawPasser_1, 0), P(drawPasser_2, 0),
#endif

        /*
         *  Knights
         */

        // Kings on same side
        P(knightByFile_0, -192), // fileA
        P(knightByFile_1, -218),
        P(knightByFile_2, -234),
        P(knightByFile_3, -164),
        P(knightByFile_4, -102),
        P(knightByFile_5, -31),
        P(knightByFile_6, 24), // -fileH

        // Opposite side kings
        P(knightByFile_0x, -99), // fileA
        P(knightByFile_1x, -144),
        P(knightByFile_2x, -106),
        P(knightByFile_3x, -29),
        P(knightByFile_4x, 51),
        P(knightByFile_5x, 147),
        P(knightByFile_6x, 102), // -fileH

        P(knightByRank_0, -122), // rank1
        P(knightByRank_1, -196),
        P(knightByRank_2, -220),
        P(knightByRank_3, -154),
        P(knightByRank_4, -47),
        P(knightByRank_5, 90),
        P(knightByRank_6, 184), // -rank8

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
       P(bishopBySquare_0, -174),
        P(bishopBySquare_1, -21),
        P(bishopBySquare_2, -95),
        P(bishopBySquare_3, 57),
        P(bishopBySquare_4, -48),
        P(bishopBySquare_5, -39),
        P(bishopBySquare_6, -55),
        P(bishopBySquare_7, 137),

        P(bishopBySquare_8, -83),
        P(bishopBySquare_9, -53),
        P(bishopBySquare_10, 78),
        P(bishopBySquare_11, -17),
        P(bishopBySquare_12, 16),
        P(bishopBySquare_13, 35),
        P(bishopBySquare_14, 127),
        P(bishopBySquare_15, 104),

        P(bishopBySquare_16, -121),
        P(bishopBySquare_17, 30),
        P(bishopBySquare_18, 11),
        P(bishopBySquare_19, 24),
        P(bishopBySquare_20, 51),
        P(bishopBySquare_21, 33),
        P(bishopBySquare_22, 25),
        P(bishopBySquare_23, 71),

        P(bishopBySquare_24, -103),
        P(bishopBySquare_25, -74),
        P(bishopBySquare_26, 37),
        P(bishopBySquare_27, 66),
        P(bishopBySquare_28, 111),
        P(bishopBySquare_29, 81),
        P(bishopBySquare_30, 96),
        P(bishopBySquare_31, 54),

        P(bishopBySquare_32, -68),
        P(bishopBySquare_33, 1),
        P(bishopBySquare_34, 3),
        P(bishopBySquare_35, 45),
        P(bishopBySquare_36, 57),
        P(bishopBySquare_37, 110),
        P(bishopBySquare_38, 14),
        P(bishopBySquare_39, 65),

        P(bishopBySquare_40, -80),
        P(bishopBySquare_41, -4),
        P(bishopBySquare_42, 47),
        P(bishopBySquare_43, -29),
        P(bishopBySquare_44, 103),
        P(bishopBySquare_45, 226),
        P(bishopBySquare_46, 74),
        P(bishopBySquare_47, -69),

        P(bishopBySquare_48, -60),
        P(bishopBySquare_49, 22),
        P(bishopBySquare_50, 7),
        P(bishopBySquare_51, 8),
        P(bishopBySquare_52, -15),
        P(bishopBySquare_53, 131),
        P(bishopBySquare_54, -169),
        P(bishopBySquare_55, -16),

        P(bishopBySquare_56, -209),
        P(bishopBySquare_57, -63),
        P(bishopBySquare_58, -1),
        P(bishopBySquare_59, 40),
        P(bishopBySquare_60, -19),
        P(bishopBySquare_61, 121),
        P(bishopBySquare_62, -251),
        P(bishopBySquare_63, -125),

        P(bishopBySquare_0x, -130),
        P(bishopBySquare_1x, -60),
        P(bishopBySquare_2x, 1),
        P(bishopBySquare_3x, -50),
        P(bishopBySquare_4x, 8),
        P(bishopBySquare_5x, 27),
        P(bishopBySquare_6x, -250),
        P(bishopBySquare_7x, -241),

        P(bishopBySquare_8x, -50),
        P(bishopBySquare_9x, -49),
        P(bishopBySquare_10x, -50),
        P(bishopBySquare_11x, 54),
        P(bishopBySquare_12x, -1),
        P(bishopBySquare_13x, 92),
        P(bishopBySquare_14x, -12),
        P(bishopBySquare_15x, -212),

        P(bishopBySquare_16x, -102),
        P(bishopBySquare_17x, -20),
        P(bishopBySquare_18x, 20),
        P(bishopBySquare_19x, 24),
        P(bishopBySquare_20x, 58),
        P(bishopBySquare_21x, 97),
        P(bishopBySquare_22x, 31),
        P(bishopBySquare_23x, 23),

        P(bishopBySquare_24x, -84),
        P(bishopBySquare_25x, -38),
        P(bishopBySquare_26x, 72),
        P(bishopBySquare_27x, 41),
        P(bishopBySquare_28x, 86),
        P(bishopBySquare_29x, 152),
        P(bishopBySquare_30x, 56),
        P(bishopBySquare_31x, 171),

        P(bishopBySquare_32x, -115),
        P(bishopBySquare_33x, 60),
        P(bishopBySquare_34x, 85),
        P(bishopBySquare_35x, 88),
        P(bishopBySquare_36x, 154),
        P(bishopBySquare_37x, 126),
        P(bishopBySquare_38x, 64),
        P(bishopBySquare_39x, 122),

        P(bishopBySquare_40x, -92),
        P(bishopBySquare_41x, 115),
        P(bishopBySquare_42x, 98),
        P(bishopBySquare_43x, 130),
        P(bishopBySquare_44x, 125),
        P(bishopBySquare_45x, 56),
        P(bishopBySquare_46x, 88),
        P(bishopBySquare_47x, 114),

        P(bishopBySquare_48x, -20),
        P(bishopBySquare_49x, 105),
        P(bishopBySquare_50x, 105),
        P(bishopBySquare_51x, 102),
        P(bishopBySquare_52x, 100),
        P(bishopBySquare_53x, 120),
        P(bishopBySquare_54x, 91),
        P(bishopBySquare_55x, 131),

        P(bishopBySquare_56x, -69),
        P(bishopBySquare_57x, 133),
        P(bishopBySquare_58x, 11),
        P(bishopBySquare_59x, 133),
        P(bishopBySquare_60x, 104),
        P(bishopBySquare_61x, 51),
        P(bishopBySquare_62x, -48),
        P(bishopBySquare_63x, -8),
#endif

        /*
         *  Rooks
         */

        P(rookByFile_0, -68), // fileA
        P(rookByFile_1, -89),
        P(rookByFile_2, -57),
        P(rookByFile_3, -10),
        P(rookByFile_4, 36),
        P(rookByFile_5, 51),
        P(rookByFile_6, 101), // -fileH

        P(rookByFile_0x, -84), // fileA
        P(rookByFile_1x, -56),
        P(rookByFile_2x, -47),
        P(rookByFile_3x, -34),
        P(rookByFile_4x, 10),
        P(rookByFile_5x, 45),
        P(rookByFile_6x, 64), // -fileH

        P(rookByRank_0, -141), // rank1
        P(rookByRank_1, -320),
        P(rookByRank_2, -474),
        P(rookByRank_3, -532),
        P(rookByRank_4, -439),
        P(rookByRank_5, -279),
        P(rookByRank_6, -115), // -rank8

        /*
         *  Queens
         */

        P(queenByFile_0, -88), // fileA
        P(queenByFile_1, -147),
        P(queenByFile_2, -162),
        P(queenByFile_3, -139),
        P(queenByFile_4, -120),
        P(queenByFile_5, -104),
        P(queenByFile_6, -53), // -fileH

        P(queenByFile_0x, -11), // fileA
        P(queenByFile_1x, -13),
        P(queenByFile_2x, 1),
        P(queenByFile_3x, 15),
        P(queenByFile_4x, 74),
        P(queenByFile_5x, 107),
        P(queenByFile_6x, 102), // -fileH

        P(queenByRank_0, -167), // rank1
        P(queenByRank_1, -276),
        P(queenByRank_2, -369),
        P(queenByRank_3, -425),
        P(queenByRank_4, -361),
        P(queenByRank_5, -256),
        P(queenByRank_6, -145), // -rank8

        /*
         *  Kings
         */

        P(kingByFile_0, -2), // fileD
        P(kingByFile_1, 22),
        P(kingByFile_2, 28), // -fileH

        P(kingByRank_0, -179), // rank1
        P(kingByRank_1, -315),
        P(kingByRank_2, -404),
        P(kingByRank_3, -410),
        P(kingByRank_4, -282),
        P(kingByRank_5, -17),
        P(kingByRank_6, 168), // -rank8

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
        P(drawOffset, 218),
        P(drawQueen, -1853),
        P(drawRook, 106),
        P(drawBishop, -37),
        P(drawKnight, -16),
        P(drawPawn, -794),
        P(drawQueenEnding, 1359),
        P(drawRookEnding, 373),
        P(drawKnightEnding, 500),
        P(drawBishopEnding, 1407),
        P(drawPawnEnding, -15802),
        P(drawPawnless, 312),

        P(drawQueenImbalance, 847),
        P(drawRookImbalance, 325),
        P(drawMinorImbalance, 31),

        P(drawUnlikeBishops, 3771),
        P(drawUnlikeBishopsAndQueens, -857),
        P(drawUnlikeBishopsAndRooks, 403),
        P(drawUnlikeBishopsAndKnights, 795),
        // TODO: pawn and pawn block square colors


// }

/*----------------------------------------------------------------------+
 |                                                                      |
 +----------------------------------------------------------------------*/

