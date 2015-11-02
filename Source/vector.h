
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
        P(tempo, 34),
        P(winBonus, 2262),

        P(castleK, -84),
        P(castleQ, -85),
        P(castleKQ, 19),

        P(queenValue, 6971),
        P(rookValue, 3906),
        P(bishopValue, 2261),
        P(knightValue, 2066),
        P(pawnValue1, 1010), P(pawnValue2, 1713), P(pawnValue3, 2384), P(pawnValue4, 3052),
        P(pawnValue5, 3716), P(pawnValue6, 4373), P(pawnValue7, 5022), P(pawnValue8, 5688),

        P(queenAndQueen, -916),
        P(queenAndRook, -635),
        P(queenAndBishop, 1),
        P(queenAndKnight, -39),
        P(queenAndPawn_1, -96), P(queenAndPawn_2, -80),
        P(rookAndRook, -329),
        P(rookAndBishop, -166),
        P(rookAndKnight, -208),
        P(rookAndPawn_1, -58), P(rookAndPawn_2, -60),
        P(bishopAndBishop, 15),
        P(bishopAndKnight, -78),
        P(bishopAndPawn_1, -60), P(bishopAndPawn_2, -15),
        P(knightAndKnight, -109),
        P(knightAndPawn_1, -32), P(knightAndPawn_2, 2),

        P(queenVsRook, -227),
        P(queenVsBishop, 76),
        P(queenVsKnight, 28),
        P(queenVsPawn_1, -11), P(queenVsPawn_2, -69),
        P(rookVsBishop, 15),
        P(rookVsKnight, 28),
        P(rookVsPawn_1, -21), P(rookVsPawn_2, -17),
        P(bishopVsKnight, 5),
        P(bishopVsPawn_1, -12), P(bishopVsPawn_2, 1),
        P(knightVsPawn_1, 2), P(knightVsPawn_2, -15),

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

        P(pawnByFile_0, -5), // "fileA"
        P(pawnByFile_1, -26),
        P(pawnByFile_2, -87),
        P(pawnByFile_3, -110),
        P(pawnByFile_4, -131),
        P(pawnByFile_5, -82),
        P(pawnByFile_6, 12), // "-fileH"

        // 5 parameters for 6 pawn ranks
        P(pawnByRank_0, -25), // "rank2"
        P(pawnByRank_1, -61),
        P(pawnByRank_2, -75),
        P(pawnByRank_3, -33),
        P(pawnByRank_4, 73), // "-rank7"

        P(doubledPawnA, -230), P(doubledPawnB, -146), P(doubledPawnC, -184), P(doubledPawnD, -146),
        P(doubledPawnE, -122), P(doubledPawnF, -64), P(doubledPawnG, -45), P(doubledPawnH, -250),

        P(backwardPawnA, -3), P(backwardPawnB, -46), P(backwardPawnC, -68), P(backwardPawnD, -117),
        P(backwardPawnE, -86), P(backwardPawnF, -34), P(backwardPawnG, -24), P(backwardPawnH, 23),

        P(isolatedPawnClosedA, 28), P(isolatedPawnClosedB, -43),
        P(isolatedPawnClosedC, -27), P(isolatedPawnClosedD, 89),
        P(isolatedPawnClosedE, 1), P(isolatedPawnClosedF, -43),
        P(isolatedPawnClosedG, -27), P(isolatedPawnClosedH, 5),

        P(isolatedPawnOpenA, -12), P(isolatedPawnOpenB, -25),
        P(isolatedPawnOpenC, -48), P(isolatedPawnOpenD, -100),
        P(isolatedPawnOpenE, -85), P(isolatedPawnOpenF, -73),
        P(isolatedPawnOpenG, -70), P(isolatedPawnOpenH, -8),

        P(sidePawnClosedA, 5), P(sidePawnClosedB, -17),
        P(sidePawnClosedC, 4), P(sidePawnClosedD, 22),
        P(sidePawnClosedE, -5), P(sidePawnClosedF, -8),
        P(sidePawnClosedG, -23), P(sidePawnClosedH, -7),

        P(sidePawnOpenA, -8), P(sidePawnOpenB, 34),
        P(sidePawnOpenC, 25), P(sidePawnOpenD, -19),
        P(sidePawnOpenE, -5), P(sidePawnOpenF, 17),
        P(sidePawnOpenG, 18), P(sidePawnOpenH, 17),

        P(middlePawnClosedA, 0), P(middlePawnClosedB, -20),
        P(middlePawnClosedC, -3), P(middlePawnClosedD, 24),
        P(middlePawnClosedE, 3), P(middlePawnClosedF, -1),
        P(middlePawnClosedG, -25), P(middlePawnClosedH, 0),

        P(middlePawnOpenA, 0), P(middlePawnOpenB, -4),
        P(middlePawnOpenC, 21), P(middlePawnOpenD, 33),
        P(middlePawnOpenE, 45), P(middlePawnOpenF, 20),
        P(middlePawnOpenG, 0), P(middlePawnOpenH, 0),

        P(duoPawnA, -14), P(duoPawnB, 4),
        P(duoPawnC, -12), P(duoPawnD, 56),
        P(duoPawnE, 29), P(duoPawnF, 30),
        P(duoPawnG, 31), P(duoPawnH, -56),

        // Quadratic polynomials for passers
        P(passerA_0, -99), P(passerB_0, -53), P(passerC_0, -64), P(passerD_0, -126),
        P(passerE_0, -42), P(passerF_0, 20), P(passerG_0, -29), P(passerH_0, 18),

        P(passerA_1, 86), P(passerB_1, 35), P(passerC_1, 4), P(passerD_1, 10),
        P(passerE_1, -37), P(passerF_1, -87), P(passerG_1, -25), P(passerH_1, 19),

        P(passerA_2, 132), P(passerB_2, 183), P(passerC_2, 194), P(passerD_2, 172),
        P(passerE_2, 201), P(passerF_2, 248), P(passerG_2, 186), P(passerH_2, 126),

#if 0
        P(drawBlockedA, 0), P(drawBlockedB, 0), P(drawBlockedC, 0), P(drawBlockedD, 0),
        P(drawBlockedE, 0), P(drawBlockedF, 0), P(drawBlockedG, 0), P(drawBlockedH, 0),

        P(drawPasser_0, 0), P(drawPasser_1, 0), P(drawPasser_2, 0),
#endif

        /*
         *  Knights
         */

        P(knightByFile_0, -145), // fileA
        P(knightByFile_1, -188),
        P(knightByFile_2, -205),
        P(knightByFile_3, -148),
        P(knightByFile_4, -87),
        P(knightByFile_5, -21),
        P(knightByFile_6, 22), // -fileH

        P(knightByRank_0, -92), // rank1
        P(knightByRank_1, -165),
        P(knightByRank_2, -189),
        P(knightByRank_3, -124),
        P(knightByRank_4, 0),
        P(knightByRank_5, 136),
        P(knightByRank_6, 202), // -rank8

        /*
         *  Bishops
         */

        P(bishopOnLong_0, 7),
        P(bishopOnLong_1, 36),

        P(bishopByFile_0, 5), // fileA
        P(bishopByFile_1, 2),
        P(bishopByFile_2, -3),
        P(bishopByFile_3, -3),
        P(bishopByFile_4, 8),
        P(bishopByFile_5, -6),
        P(bishopByFile_6, 4), // -fileH

        P(bishopByRank_0, -79), // rank1
        P(bishopByRank_1, -100),
        P(bishopByRank_2, -82),
        P(bishopByRank_3, -57),
        P(bishopByRank_4, -32),
        P(bishopByRank_5, 53),
        P(bishopByRank_6, 27), // -rank8

        /*
         *  Rooks
         */

        P(rookByFile_0, -70), // fileA
        P(rookByFile_1, -73),
        P(rookByFile_2, -51),
        P(rookByFile_3, -15),
        P(rookByFile_4, 28),
        P(rookByFile_5, 46),
        P(rookByFile_6, 85), // -fileH

        P(rookByRank_0, -135), // rank1
        P(rookByRank_1, -333),
        P(rookByRank_2, -492),
        P(rookByRank_3, -543),
        P(rookByRank_4, -447),
        P(rookByRank_5, -276),
        P(rookByRank_6, -105), // -rank8

        /*
         *  Queens
         */

        P(queenByFile_0, -17), // fileA
        P(queenByFile_1, -46),
        P(queenByFile_2, -54),
        P(queenByFile_3, -75),
        P(queenByFile_4, -75),
        P(queenByFile_5, -70),
        P(queenByFile_6, -55), // -fileH

        P(queenByRank_0, -92), // rank1
        P(queenByRank_1, -166),
        P(queenByRank_2, -236),
        P(queenByRank_3, -279),
        P(queenByRank_4, -220),
        P(queenByRank_5, -95),
        P(queenByRank_6, -60), // -rank8

        /*
         *  Kings
         */

        P(kingByFile_0, -8), // fileD
        P(kingByFile_1, -25),
        P(kingByFile_2, 9), // -fileH

        P(kingByRank_0, -84), // rank1
        P(kingByRank_1, -159),
        P(kingByRank_2, -223),
        P(kingByRank_3, -194),
        P(kingByRank_4, -54),
        P(kingByRank_5, 178),
        P(kingByRank_6, 291), // -rank8


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
        P(drawOffset, -362),
        P(drawQueen, -2219),
        P(drawRook, 455),
        P(drawBishop, -30),
        P(drawKnight, 51),
        P(drawPawn, -868),
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

