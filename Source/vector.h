
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
 *  Suffix '[A-H]x' refers to the file when the opponent king is on file E..H.
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
        P(tempo, 37),
        P(winBonus, 3471),

        P(castleK, -83),
        P(castleQ, -75),
        P(castleKQ, 14),

        P(queenValue, 7121),
        P(rookValue, 4023),
        P(bishopValue, 2284),
        P(knightValue, 2087),
        P(pawnValue1, 1069), P(pawnValue2, 1742), P(pawnValue3, 2387), P(pawnValue4, 3037),
        P(pawnValue5, 3683), P(pawnValue6, 4327), P(pawnValue7, 4948), P(pawnValue8, 5565),

        P(queenAndQueen, -967),
        P(queenAndRook, -658),
        P(queenAndBishop, 26),
        P(queenAndKnight, -63),
        P(queenAndPawn_1, -88), P(queenAndPawn_2, -86),
        P(rookAndRook, -315),
        P(rookAndBishop, -144),
        P(rookAndKnight, -166),
        P(rookAndPawn_1, -54), P(rookAndPawn_2, -62),
        P(bishopAndBishop, 36),
        P(bishopAndKnight, -63),
        P(bishopAndPawn_1, -56), P(bishopAndPawn_2, -19),
        P(knightAndKnight, -92),
        P(knightAndPawn_1, -27), P(knightAndPawn_2, -1),

        P(queenVsRook, -249),
        P(queenVsBishop, 85),
        P(queenVsKnight, -5),
        P(queenVsPawn_1, 27), P(queenVsPawn_2, -89),
        P(rookVsBishop, 5),
        P(rookVsKnight, 26),
        P(rookVsPawn_1, -22), P(rookVsPawn_2, -28),
        P(bishopVsKnight, 7),
        P(bishopVsPawn_1, -11), P(bishopVsPawn_2, -3),
        P(knightVsPawn_1, 0), P(knightVsPawn_2, -17),

        P(controlCenter, 40),
        P(controlExtendedCenter, 43),
        P(controlOutside, 33),

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

        P(pawnByFile_0, -8), // "fileA"
        P(pawnByFile_1, -20),
        P(pawnByFile_2, -90),
        P(pawnByFile_3, -124),
        P(pawnByFile_4, -139),
        P(pawnByFile_5, -74),
        P(pawnByFile_6, 23), // "-fileH"

        // 5 parameters for 6 pawn ranks
        P(pawnByRank_0, -28), // "rank2"
        P(pawnByRank_1, -65),
        P(pawnByRank_2, -82),
        P(pawnByRank_3, -43),
        P(pawnByRank_4, 70), // "-rank7"

        P(doubledPawnA, -204), P(doubledPawnB, -115), P(doubledPawnC, -160), P(doubledPawnD, -120),
        P(doubledPawnE, -101), P(doubledPawnF, -41), P(doubledPawnG, -14), P(doubledPawnH, -221),

        P(backwardPawnA, -8), P(backwardPawnB, -45), P(backwardPawnC, -55), P(backwardPawnD, -100),
        P(backwardPawnE, -85), P(backwardPawnF, -33), P(backwardPawnG, -15), P(backwardPawnH, 27),

        P(isolatedPawnClosedA, 45), P(isolatedPawnClosedB, -84),
        P(isolatedPawnClosedC, -55), P(isolatedPawnClosedD, 23),
        P(isolatedPawnClosedE, -24), P(isolatedPawnClosedF, -57),
        P(isolatedPawnClosedG, -40), P(isolatedPawnClosedH, 4),

        P(isolatedPawnOpenA, -8), P(isolatedPawnOpenB, -49),
        P(isolatedPawnOpenC, -75), P(isolatedPawnOpenD, -122),
        P(isolatedPawnOpenE, -105), P(isolatedPawnOpenF, -92),
        P(isolatedPawnOpenG, -64), P(isolatedPawnOpenH, 26),

        P(sidePawnClosedA, 2), P(sidePawnClosedB, -26),
        P(sidePawnClosedC, -5), P(sidePawnClosedD, -27),
        P(sidePawnClosedE, -13), P(sidePawnClosedF, -1),
        P(sidePawnClosedG, -22), P(sidePawnClosedH, 4),

        P(sidePawnOpenA, -10), P(sidePawnOpenB, 39),
        P(sidePawnOpenC, 11), P(sidePawnOpenD, -30),
        P(sidePawnOpenE, -12), P(sidePawnOpenF, 8),
        P(sidePawnOpenG, 25), P(sidePawnOpenH, 52),

        P(middlePawnClosedA, 0), P(middlePawnClosedB, -4),
        P(middlePawnClosedC, 10), P(middlePawnClosedD, -9),
        P(middlePawnClosedE, 19), P(middlePawnClosedF, 10),
        P(middlePawnClosedG, -15), P(middlePawnClosedH, 0),

        P(middlePawnOpenA, 0), P(middlePawnOpenB, 20),
        P(middlePawnOpenC, 30), P(middlePawnOpenD, 35),
        P(middlePawnOpenE, 57), P(middlePawnOpenF, 2),
        P(middlePawnOpenG, 2), P(middlePawnOpenH, 0),

        P(duoPawnA, -22), P(duoPawnB, 7),
        P(duoPawnC, -15), P(duoPawnD, 58),
        P(duoPawnE, 42), P(duoPawnF, 11),
        P(duoPawnG, 52), P(duoPawnH, -70),

        // Quadratic polynomials for passers
        P(passerA_0, -84), P(passerB_0, -9), P(passerC_0, 13), P(passerD_0, -62),
        P(passerE_0, 38), P(passerF_0, 52), P(passerG_0, 11), P(passerH_0, 48),

        P(passerA_1, 97), P(passerB_1, 32), P(passerC_1, -8), P(passerD_1, 3),
        P(passerE_1, -46), P(passerF_1, -73), P(passerG_1, -22), P(passerH_1, 19),

        P(passerA_2, 154), P(passerB_2, 219), P(passerC_2, 240), P(passerD_2, 210),
        P(passerE_2, 223), P(passerF_2, 252), P(passerG_2, 198), P(passerH_2, 144),

        P(passerScalingOffset, 5050),
        P(passerAndQueen, -492),
        P(passerAndRook, -278),
        P(passerAndBishop, 146),
        P(passerAndKnight, -137),
        P(passerAndPawn, -627),
        P(passerVsQueen, 1065),
        P(passerVsRook, 29),
        P(passerVsBishop, -613),
        P(passerVsKnight, -409),
        P(passerVsPawn, 20),

#if 0
        P(drawBlockedA, 0), P(drawBlockedB, 0), P(drawBlockedC, 0), P(drawBlockedD, 0),
        P(drawBlockedE, 0), P(drawBlockedF, 0), P(drawBlockedG, 0), P(drawBlockedH, 0),

        P(drawPasser_0, 0), P(drawPasser_1, 0), P(drawPasser_2, 0),
#endif

        /*
         *  Knights
         */

        // Kings on same side
        P(knightByFile_0, -179), // fileA
        P(knightByFile_1, -220),
        P(knightByFile_2, -234),
        P(knightByFile_3, -168),
        P(knightByFile_4, -95),
        P(knightByFile_5, -20),
        P(knightByFile_6, 34), // -fileH

        // Opposite side kings
        P(knightByFile_0x, -93), // fileA
        P(knightByFile_1x, -110),
        P(knightByFile_2x, -101),
        P(knightByFile_3x, -20),
        P(knightByFile_4x, 53),
        P(knightByFile_5x, 123),
        P(knightByFile_6x, 99), // -fileH

        P(knightByRank_0, -101), // rank1
        P(knightByRank_1, -179),
        P(knightByRank_2, -210),
        P(knightByRank_3, -148),
        P(knightByRank_4, -29),
        P(knightByRank_5, 114),
        P(knightByRank_6, 188), // -rank8

        /*
         *  Bishops
         */

        P(bishopBySquare_0, -159),
        P(bishopBySquare_1, 5),
        P(bishopBySquare_2, -61),
        P(bishopBySquare_3, 43),
        P(bishopBySquare_4, -47),
        P(bishopBySquare_5, 7),
        P(bishopBySquare_6, -113),
        P(bishopBySquare_7, 6),

        P(bishopBySquare_8, -30),
        P(bishopBySquare_9, -55),
        P(bishopBySquare_10, 55),
        P(bishopBySquare_11, -9),
        P(bishopBySquare_12, 3),
        P(bishopBySquare_13, 66),
        P(bishopBySquare_14, 47),
        P(bishopBySquare_15, 50),

        P(bishopBySquare_16, -108),
        P(bishopBySquare_17, 41),
        P(bishopBySquare_18, 13),
        P(bishopBySquare_19, 36),
        P(bishopBySquare_20, 50),
        P(bishopBySquare_21, 51),
        P(bishopBySquare_22, 53),
        P(bishopBySquare_23, 76),

        P(bishopBySquare_24, -119),
        P(bishopBySquare_25, -56),
        P(bishopBySquare_26, 47),
        P(bishopBySquare_27, 66),
        P(bishopBySquare_28, 122),
        P(bishopBySquare_29, 114),
        P(bishopBySquare_30, 62),
        P(bishopBySquare_31, 87),

        P(bishopBySquare_32, -111),
        P(bishopBySquare_33, 0),
        P(bishopBySquare_34, 27),
        P(bishopBySquare_35, 84),
        P(bishopBySquare_36, 105),
        P(bishopBySquare_37, 146),
        P(bishopBySquare_38, 40),
        P(bishopBySquare_39, 79),

        P(bishopBySquare_40, -89),
        P(bishopBySquare_41, -22),
        P(bishopBySquare_42, 25),
        P(bishopBySquare_43, 2),
        P(bishopBySquare_44, 123),
        P(bishopBySquare_45, 156),
        P(bishopBySquare_46, 100),
        P(bishopBySquare_47, 82),

        P(bishopBySquare_48, -90),
        P(bishopBySquare_49, 33),
        P(bishopBySquare_50, 19),
        P(bishopBySquare_51, 26),
        P(bishopBySquare_52, 16),
        P(bishopBySquare_53, 167),
        P(bishopBySquare_54, 7),
        P(bishopBySquare_55, 80),

        P(bishopBySquare_56, -224),
        P(bishopBySquare_57, -41),
        P(bishopBySquare_58, 6),
        P(bishopBySquare_59, 61),
        P(bishopBySquare_60, 27),
        P(bishopBySquare_61, 171),
        P(bishopBySquare_62, -121),
        P(bishopBySquare_63, -26),

        /*
         *  Rooks
         */

        P(rookByFile_0, -74), // fileA
        P(rookByFile_1, -77),
        P(rookByFile_2, -49),
        P(rookByFile_3, -10),
        P(rookByFile_4, 34),
        P(rookByFile_5, 53),
        P(rookByFile_6, 91), // -fileH

        P(rookByRank_0, -137), // rank1
        P(rookByRank_1, -336),
        P(rookByRank_2, -493),
        P(rookByRank_3, -541),
        P(rookByRank_4, -448),
        P(rookByRank_5, -276),
        P(rookByRank_6, -107), // -rank8

        /*
         *  Queens
         */

        P(queenByFile_0, -62), // fileA
        P(queenByFile_1, -109),
        P(queenByFile_2, -111),
        P(queenByFile_3, -102),
        P(queenByFile_4, -87),
        P(queenByFile_5, -72),
        P(queenByFile_6, -51), // -fileH

        P(queenByRank_0, -152), // rank1
        P(queenByRank_1, -264),
        P(queenByRank_2, -360),
        P(queenByRank_3, -412),
        P(queenByRank_4, -358),
        P(queenByRank_5, -212),
        P(queenByRank_6, -140), // -rank8

        /*
         *  Kings
         */

        P(kingByFile_0, -5), // fileD
        P(kingByFile_1, -26),
        P(kingByFile_2, 10), // -fileH

        P(kingByRank_0, -144), // rank1
        P(kingByRank_1, -268),
        P(kingByRank_2, -363),
        P(kingByRank_3, -355),
        P(kingByRank_4, -210),
        P(kingByRank_5, 27),
        P(kingByRank_6, 177), // -rank8

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
        P(drawOffset, 171),
        P(drawQueen, -2030),
        P(drawRook, 119),
        P(drawBishop, -31),
        P(drawKnight, -13),
        P(drawPawn, -797),
        P(drawQueenEnding, 1439),
        P(drawRookEnding, 401),
        P(drawKnightEnding, 566),
        P(drawBishopEnding, 1520),
        P(drawPawnEnding, -17553),
        P(drawPawnless, 324),

        P(drawQueenImbalance, 885),
        P(drawRookImbalance, 274),
        P(drawMinorImbalance, 74),

        P(drawUnlikeBishops, 3686),
        P(drawUnlikeBishopsAndQueens, -424),
        P(drawUnlikeBishopsAndRooks, 402),
        P(drawUnlikeBishopsAndKnights, 977),
        // TODO: pawn and pawn block square colors


// }

/*----------------------------------------------------------------------+
 |                                                                      |
 +----------------------------------------------------------------------*/

