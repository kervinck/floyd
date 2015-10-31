
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
        P(tempo, 40),
        P(winBonus, 2262),

        P(castleK, -74),
        P(castleQ, -69),
        P(castleKQ, 29),

        P(queenValue, 6924),
        P(rookValue, 3859),
        P(bishopValue, 2250),
        P(knightValue, 2039),
        P(pawnValue1, 980), P(pawnValue2, 1679), P(pawnValue3, 2344), P(pawnValue4, 3012),
        P(pawnValue5, 3677), P(pawnValue6, 4335), P(pawnValue7, 5003), P(pawnValue8, 5712),

        P(queenAndQueen, -933),
        P(queenAndRook, -715),
        P(queenAndBishop, -54),
        P(queenAndKnight, -65),
        P(queenAndPawn_1, -96), P(queenAndPawn_2, -76),
        P(rookAndRook, -364),
        P(rookAndBishop, -173),
        P(rookAndKnight, -236),
        P(rookAndPawn_1, -56), P(rookAndPawn_2, -61),
        P(bishopAndBishop, -30),
        P(bishopAndKnight, -132),
        P(bishopAndPawn_1, -56), P(bishopAndPawn_2, -18),
        P(knightAndKnight, -141),
        P(knightAndPawn_1, -29), P(knightAndPawn_2, -1),

        P(queenVsRook, -266),
        P(queenVsBishop, 75),
        P(queenVsKnight, 6),
        P(queenVsPawn_1, -20), P(queenVsPawn_2, -56),
        P(rookVsBishop, 22),
        P(rookVsKnight, 29),
        P(rookVsPawn_1, -21), P(rookVsPawn_2, -14),
        P(bishopVsKnight, -2),
        P(bishopVsPawn_1, -11), P(bishopVsPawn_2, 1),
        P(knightVsPawn_1, 3), P(knightVsPawn_2, -16),

        P(controlCenter, 43),
        P(controlExtendedCenter, 46),
        P(controlOutside, 34),

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

        P(pawnByFile_0, -4), // "fileA"
        P(pawnByFile_1, -27),
        P(pawnByFile_2, -93),
        P(pawnByFile_3, -122),
        P(pawnByFile_4, -137),
        P(pawnByFile_5, -68),
        P(pawnByFile_6, 24), // "-fileH"

        // 5 parameters for 6 pawn ranks
        P(pawnByRank_0, -23), // "rank2"
        P(pawnByRank_1, -52),
        P(pawnByRank_2, -64),
        P(pawnByRank_3, -28),
        P(pawnByRank_4, 74), // "-rank7"

        P(doubledPawnA, -259), P(doubledPawnB, -135), P(doubledPawnC, -186), P(doubledPawnD, -187),
        P(doubledPawnE, -165), P(doubledPawnF, -77), P(doubledPawnG, -69), P(doubledPawnH, -255),

        P(backwardPawnA, -1), P(backwardPawnB, -46), P(backwardPawnC, -68), P(backwardPawnD, -148),
        P(backwardPawnE, -125), P(backwardPawnF, -63), P(backwardPawnG, -29), P(backwardPawnH, 26),

        P(isolatedPawnClosedA, 20), P(isolatedPawnClosedB, -31),
        P(isolatedPawnClosedC, -10), P(isolatedPawnClosedD, 53),
        P(isolatedPawnClosedE, 25), P(isolatedPawnClosedF, -20),
        P(isolatedPawnClosedG, -10), P(isolatedPawnClosedH, 30),

        P(isolatedPawnOpenA, -17), P(isolatedPawnOpenB, -30),
        P(isolatedPawnOpenC, -60), P(isolatedPawnOpenD, -80),
        P(isolatedPawnOpenE, -75), P(isolatedPawnOpenF, -80),
        P(isolatedPawnOpenG, -70), P(isolatedPawnOpenH, -17),

        P(sidePawnClosedA, 0), P(sidePawnClosedB, 6),
        P(sidePawnClosedC, 9), P(sidePawnClosedD, 0),
        P(sidePawnClosedE, -1), P(sidePawnClosedF, -3),
        P(sidePawnClosedG, -1), P(sidePawnClosedH, 3),

        P(sidePawnOpenA, -17), P(sidePawnOpenB, 25),
        P(sidePawnOpenC, 30), P(sidePawnOpenD, -10),
        P(sidePawnOpenE, 3), P(sidePawnOpenF, 11),
        P(sidePawnOpenG, 16), P(sidePawnOpenH, 5),

        P(middlePawnClosedA, 0), P(middlePawnClosedB, -13),
        P(middlePawnClosedC, -5), P(middlePawnClosedD, 10),
        P(middlePawnClosedE, 5), P(middlePawnClosedF, -1),
        P(middlePawnClosedG, -10), P(middlePawnClosedH, 0),

        P(middlePawnOpenA, 0), P(middlePawnOpenB, -10),
        P(middlePawnOpenC, 1), P(middlePawnOpenD, 30),
        P(middlePawnOpenE, 21), P(middlePawnOpenF, -5),
        P(middlePawnOpenG, -10), P(middlePawnOpenH, 0),

        P(duoPawnA, -5), P(duoPawnB, -3),
        P(duoPawnC, 0), P(duoPawnD, 39),
        P(duoPawnE, 30), P(duoPawnF, 19),
        P(duoPawnG, -1), P(duoPawnH, -35),

        // Quadratic polynomials for passers
        P(passerA_0, -112), P(passerB_0, -54), P(passerC_0, -84), P(passerD_0, -179),
        P(passerE_0, -92), P(passerF_0, -32), P(passerG_0, -32), P(passerH_0, 32),

        P(passerA_1, 80), P(passerB_1, 29), P(passerC_1, 5), P(passerD_1, 15),
        P(passerE_1, -25), P(passerF_1, -86), P(passerG_1, -30), P(passerH_1, 19),

        P(passerA_2, 131), P(passerB_2, 184), P(passerC_2, 186), P(passerD_2, 165),
        P(passerE_2, 191), P(passerF_2, 247), P(passerG_2, 186), P(passerH_2, 122),

#if 0
        P(drawBlockedA, 0), P(drawBlockedB, 0), P(drawBlockedC, 0), P(drawBlockedD, 0),
        P(drawBlockedE, 0), P(drawBlockedF, 0), P(drawBlockedG, 0), P(drawBlockedH, 0),

        P(drawPasser_0, 0), P(drawPasser_1, 0), P(drawPasser_2, 0),
#endif

        /*
         *  Knights
         */

        P(knightByFile_0, -137), // fileA
        P(knightByFile_1, -180),
        P(knightByFile_2, -200),
        P(knightByFile_3, -143),
        P(knightByFile_4, -86),
        P(knightByFile_5, -21),
        P(knightByFile_6, 18), // -fileH

        P(knightByRank_0, -92), // rank1
        P(knightByRank_1, -163),
        P(knightByRank_2, -183),
        P(knightByRank_3, -123),
        P(knightByRank_4, 6),
        P(knightByRank_5, 143),
        P(knightByRank_6, 205), // -rank8

        /*
         *  Bishops
         */

        P(bishopOnLong_0, 10),
        P(bishopOnLong_1, 42),

        P(bishopByFile_0, 16), // fileA
        P(bishopByFile_1, 9),
        P(bishopByFile_2, 5),
        P(bishopByFile_3, 0),
        P(bishopByFile_4, 8),
        P(bishopByFile_5, -4),
        P(bishopByFile_6, 4), // -fileH

        P(bishopByRank_0, -74), // rank1
        P(bishopByRank_1, -98),
        P(bishopByRank_2, -77),
        P(bishopByRank_3, -53),
        P(bishopByRank_4, -25),
        P(bishopByRank_5, 42),
        P(bishopByRank_6, 19), // -rank8

        /*
         *  Rooks
         */

        P(rookByFile_0, -71), // fileA
        P(rookByFile_1, -71),
        P(rookByFile_2, -48),
        P(rookByFile_3, -16),
        P(rookByFile_4, 25),
        P(rookByFile_5, 45),
        P(rookByFile_6, 85), // -fileH

        P(rookByRank_0, -137), // rank1
        P(rookByRank_1, -336),
        P(rookByRank_2, -494),
        P(rookByRank_3, -540),
        P(rookByRank_4, -450),
        P(rookByRank_5, -276),
        P(rookByRank_6, -105), // -rank8

        /*
         *  Queens
         */

        P(queenByFile_0, -16), // fileA
        P(queenByFile_1, -34),
        P(queenByFile_2, -43),
        P(queenByFile_3, -70),
        P(queenByFile_4, -75),
        P(queenByFile_5, -72),
        P(queenByFile_6, -55), // -fileH

        P(queenByRank_0, -75), // rank1
        P(queenByRank_1, -155),
        P(queenByRank_2, -228),
        P(queenByRank_3, -270),
        P(queenByRank_4, -214),
        P(queenByRank_5, -85),
        P(queenByRank_6, -52), // -rank8

        /*
         *  Kings
         */

        P(kingByFile_0, -7), // fileD
        P(kingByFile_1, -28),
        P(kingByFile_2, 8), // -fileH

        P(kingByRank_0, -70), // rank1
        P(kingByRank_1, -147),
        P(kingByRank_2, -213),
        P(kingByRank_3, -182),
        P(kingByRank_4, -43),
        P(kingByRank_5, 184),
        P(kingByRank_6, 302), // -rank8


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

        P(passerOffset, 0),
        P(passerAndQueen, 0),
        P(passerAndRook, 0),
        P(passerAndBishop, 0),
        P(passerAndKnight, 0),
        P(passerAndPawn, 0),
        P(passerVsQueen, 0),
        P(passerVsRook, 0),
        P(passerVsBishop, 0),
        P(passerVsKnight, 0),
        P(passerVsPawn, 0),
#endif

        // Drawness. Positive is more drawish. Negative is more sharp
        P(drawOffset, -497),
        P(drawQueen, -2248),
        P(drawRook, 492),
        P(drawBishop, 53),
        P(drawKnight, 108),
        P(drawPawn, -866),
        P(drawQueenEnding, 998),
        P(drawRookEnding, -176),
        P(drawKnightEnding, 1262),
        P(drawBishopEnding, 2292),
        P(drawPawnEnding, -11962),
        P(drawPawnless, -105),

        P(drawQueenImbalance, 1582),
        P(drawRookImbalance, 146),
        P(drawMinorImbalance, 366),

        P(drawUnlikeBishops, 3731),
        P(drawUnlikeBishopsAndQueens, -452),
        P(drawUnlikeBishopsAndRooks, 303),
        P(drawUnlikeBishopsAndKnights, 1482),
        // TODO: pawn and pawn block square colors


// }

/*----------------------------------------------------------------------+
 |                                                                      |
 +----------------------------------------------------------------------*/

