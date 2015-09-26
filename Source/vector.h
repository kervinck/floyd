
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
        P(eloDiff, 52),
        P(tempo, 61),
        P(winBonus, 21009),

        P(castleK, -79),
        P(castleQ, -84),
        P(castleKQ, 14),

        P(queenValue, 6719),
        P(rookValue, 3711),
        P(bishopValue, 2216),
        P(knightValue, 1954),
        P(pawnValue1, 1005), P(pawnValue2, 698), P(pawnValue3, 661), P(pawnValue4, 652),
        P(pawnValue5, 653), P(pawnValue6, 669), P(pawnValue7, 661), P(pawnValue8, 759),

        P(queenAndQueen, -1077),
        P(queenAndRook, -907),
        P(queenAndBishop, -134),
        P(queenAndKnight, -140),
        P(queenAndPawn_1, -111), P(queenAndPawn_2, -64),
        P(rookAndRook, -437),
        P(rookAndBishop, -249),
        P(rookAndKnight, -279),
        P(rookAndPawn_1, -56), P(rookAndPawn_2, -66),
        P(bishopAndBishop, -86),
        P(bishopAndKnight, -177),
        P(bishopAndPawn_1, -60), P(bishopAndPawn_2, -18),
        P(knightAndKnight, -195),
        P(knightAndPawn_1, -24), P(knightAndPawn_2, -7),

        P(queenVsRook, -363),
        P(queenVsBishop, 38),
        P(queenVsKnight, 6),
        P(queenVsPawn_1, -23), P(queenVsPawn_2, -46),
        P(rookVsBishop, 33),
        P(rookVsKnight, 44),
        P(rookVsPawn_1, -21), P(rookVsPawn_2, -10),
        P(bishopVsKnight, -12),
        P(bishopVsPawn_1, -10), P(bishopVsPawn_2, 0),
        P(knightVsPawn_1, 3), P(knightVsPawn_2, -16),

        P(controlCenter, 43),
        P(controlExtendedCenter, 45),
        P(controlOutside, 35),

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

        P(pawnByFile_0, 13), // "fileA"
        P(pawnByFile_1, -12),
        P(pawnByFile_2, -78),
        P(pawnByFile_3, -118),
        P(pawnByFile_4, -139),
        P(pawnByFile_5, -66),
        P(pawnByFile_6, 25), // "-fileH"

        // 5 parameters for 6 pawn ranks
        P(pawnByRank_0, -23), // "rank2"
        P(pawnByRank_1, -50),
        P(pawnByRank_2, -61),
        P(pawnByRank_3, -15),
        P(pawnByRank_4, 77), // "-rank7"

        P(doubledPawnA, -274), P(doubledPawnB, -110), P(doubledPawnC, -186), P(doubledPawnD, -177),
        P(doubledPawnE, -145), P(doubledPawnF, -67), P(doubledPawnG, -68), P(doubledPawnH, -265),

        P(backwardPawnA, 0), P(backwardPawnB, -38), P(backwardPawnC, -71), P(backwardPawnD, -138),
        P(backwardPawnE, -124), P(backwardPawnF, -63), P(backwardPawnG, -26), P(backwardPawnH, 31),

        // Quadratic polynomials for passers
        P(passerA_0, -127), P(passerB_0, -74), P(passerC_0, -114), P(passerD_0, -194),
        P(passerE_0, -113), P(passerF_0, -42), P(passerG_0, -69), P(passerH_0, 27),

        P(passerA_1, 79), P(passerB_1, 30), P(passerC_1, 11), P(passerD_1, 20),
        P(passerE_1, -20), P(passerF_1, -87), P(passerG_1, -19), P(passerH_1, 16),

        P(passerA_2, 126), P(passerB_2, 181), P(passerC_2, 184), P(passerD_2, 156),
        P(passerE_2, 184), P(passerF_2, 244), P(passerG_2, 173), P(passerH_2, 119),

#if 0
        P(drawBlockedA, 0), P(drawBlockedB, 0), P(drawBlockedC, 0), P(drawBlockedD, 0),
        P(drawBlockedE, 0), P(drawBlockedF, 0), P(drawBlockedG, 0), P(drawBlockedH, 0),

        P(drawPasser_0, 0), P(drawPasser_1, 0), P(drawPasser_2, 0),
#endif

        /*
         *  Knights
         */

        P(knightByFile_0, -136), // fileA
        P(knightByFile_1, -175),
        P(knightByFile_2, -191),
        P(knightByFile_3, -138),
        P(knightByFile_4, -76),
        P(knightByFile_5, -17),
        P(knightByFile_6, 23), // -fileH

        P(knightByRank_0, -89), // rank1
        P(knightByRank_1, -158),
        P(knightByRank_2, -179),
        P(knightByRank_3, -115),
        P(knightByRank_4, 9),
        P(knightByRank_5, 148),
        P(knightByRank_6, 215), // -rank8

        /*
         *  Bishops
         */

        P(bishopOnLong_0, -9),
        P(bishopOnLong_1, 24),

        P(bishopByFile_0, 13), // fileA
        P(bishopByFile_1, 21),
        P(bishopByFile_2, 20),
        P(bishopByFile_3, 14),
        P(bishopByFile_4, 14),
        P(bishopByFile_5, -10),
        P(bishopByFile_6, 7), // -fileH

        P(bishopByRank_0, -59), // rank1
        P(bishopByRank_1, -78),
        P(bishopByRank_2, -63),
        P(bishopByRank_3, -43),
        P(bishopByRank_4, -20),
        P(bishopByRank_5, 45),
        P(bishopByRank_6, 19), // -rank8

        /*
         *  Rooks
         */

        P(rookByFile_0, -73), // fileA
        P(rookByFile_1, -74),
        P(rookByFile_2, -47),
        P(rookByFile_3, -13),
        P(rookByFile_4, 22),
        P(rookByFile_5, 42),
        P(rookByFile_6, 81), // -fileH

        P(rookByRank_0, -137), // rank1
        P(rookByRank_1, -336),
        P(rookByRank_2, -497),
        P(rookByRank_3, -543),
        P(rookByRank_4, -451),
        P(rookByRank_5, -286),
        P(rookByRank_6, -116), // -rank8

        /*
         *  Queens
         */

        P(queenByFile_0, -15), // fileA
        P(queenByFile_1, -33),
        P(queenByFile_2, -30),
        P(queenByFile_3, -45),
        P(queenByFile_4, -56),
        P(queenByFile_5, -62),
        P(queenByFile_6, -55), // -fileH

        P(queenByRank_0, -70), // rank1
        P(queenByRank_1, -115),
        P(queenByRank_2, -163),
        P(queenByRank_3, -184),
        P(queenByRank_4, -140),
        P(queenByRank_5, -40),
        P(queenByRank_6, -25), // -rank8

        /*
         *  Kings
         */

        P(kingByFile_0, -6), // fileD
        P(kingByFile_1, -28),
        P(kingByFile_2, 7), // -fileH

        P(kingByRank_0, -70), // rank1
        P(kingByRank_1, -122),
        P(kingByRank_2, -183),
        P(kingByRank_3, -145),
        P(kingByRank_4, 2),
        P(kingByRank_5, 225),
        P(kingByRank_6, 338), // -rank8


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
        P(drawOffset, -651),
        P(drawQueen, -2193),
        P(drawRook, 525),
        P(drawBishop, 73),
        P(drawKnight, 128),
        P(drawPawn, -827),
        P(drawQueenEnding, 782),
        P(drawRookEnding, -166),
        P(drawKnightEnding, 1322),
        P(drawBishopEnding, 2277),
        P(drawPawnEnding, -12962),
        P(drawPawnless, -155),

        P(drawQueenImbalance, 1656),
        P(drawRookImbalance, 126),
        P(drawMinorImbalance, 408),

        P(drawUnlikeBishops, 3638),
        P(drawUnlikeBishopsAndQueens, 254),
        P(drawUnlikeBishopsAndRooks, 344),
        P(drawUnlikeBishopsAndKnights, 1726),
        // TODO: pawn and pawn block square colors


// }

/*----------------------------------------------------------------------+
 |                                                                      |
 +----------------------------------------------------------------------*/

