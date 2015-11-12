
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
        P(tempo, 36),
        P(winBonus, 3452),

        P(castleK, -85),
        P(castleQ, -75),
        P(castleKQ, 15),

        P(queenValue, 7058),
        P(rookValue, 3984),
        P(bishopValue, 2276),
        P(knightValue, 2072),
        P(pawnValue1, 1073), P(pawnValue2, 1742), P(pawnValue3, 2387), P(pawnValue4, 3037),
        P(pawnValue5, 3684), P(pawnValue6, 4331), P(pawnValue7, 4965), P(pawnValue8, 5592),

        P(queenAndQueen, -937),
        P(queenAndRook, -636),
        P(queenAndBishop, 15),
        P(queenAndKnight, -63),
        P(queenAndPawn_1, -83), P(queenAndPawn_2, -86),
        P(rookAndRook, -305),
        P(rookAndBishop, -148),
        P(rookAndKnight, -169),
        P(rookAndPawn_1, -51), P(rookAndPawn_2, -62),
        P(bishopAndBishop, 32),
        P(bishopAndKnight, -68),
        P(bishopAndPawn_1, -56), P(bishopAndPawn_2, -19),
        P(knightAndKnight, -94),
        P(knightAndPawn_1, -27), P(knightAndPawn_2, -1),

        P(queenVsRook, -247),
        P(queenVsBishop, 74),
        P(queenVsKnight, -9),
        P(queenVsPawn_1, 27), P(queenVsPawn_2, -82),
        P(rookVsBishop, 7),
        P(rookVsKnight, 25),
        P(rookVsPawn_1, -22), P(rookVsPawn_2, -26),
        P(bishopVsKnight, 4),
        P(bishopVsPawn_1, -11), P(bishopVsPawn_2, 0),
        P(knightVsPawn_1, 2), P(knightVsPawn_2, -17),

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
        P(pawnByFile_1, -22),
        P(pawnByFile_2, -92),
        P(pawnByFile_3, -124),
        P(pawnByFile_4, -139),
        P(pawnByFile_5, -72),
        P(pawnByFile_6, 23), // "-fileH"

        // 5 parameters for 6 pawn ranks
        P(pawnByRank_0, -27), // "rank2"
        P(pawnByRank_1, -63),
        P(pawnByRank_2, -79),
        P(pawnByRank_3, -39),
        P(pawnByRank_4, 72), // "-rank7"

        P(doubledPawnA, -204), P(doubledPawnB, -118), P(doubledPawnC, -163), P(doubledPawnD, -120),
        P(doubledPawnE, -103), P(doubledPawnF, -43), P(doubledPawnG, -18), P(doubledPawnH, -226),

        P(backwardPawnA, -9), P(backwardPawnB, -46), P(backwardPawnC, -57), P(backwardPawnD, -105),
        P(backwardPawnE, -86), P(backwardPawnF, -38), P(backwardPawnG, -16), P(backwardPawnH, 27),

        P(isolatedPawnClosedA, 45), P(isolatedPawnClosedB, -75),
        P(isolatedPawnClosedC, -50), P(isolatedPawnClosedD, 56),
        P(isolatedPawnClosedE, -15), P(isolatedPawnClosedF, -50),
        P(isolatedPawnClosedG, -35), P(isolatedPawnClosedH, 5),

        P(isolatedPawnOpenA, -7), P(isolatedPawnOpenB, -40),
        P(isolatedPawnOpenC, -67), P(isolatedPawnOpenD, -116),
        P(isolatedPawnOpenE, -105), P(isolatedPawnOpenF, -89),
        P(isolatedPawnOpenG, -65), P(isolatedPawnOpenH, 25),

        P(sidePawnClosedA, 3), P(sidePawnClosedB, -25),
        P(sidePawnClosedC, -3), P(sidePawnClosedD, -1),
        P(sidePawnClosedE, -7), P(sidePawnClosedF, 1),
        P(sidePawnClosedG, -20), P(sidePawnClosedH, 4),

        P(sidePawnOpenA, -4), P(sidePawnOpenB, 41),
        P(sidePawnOpenC, 20), P(sidePawnOpenD, -22),
        P(sidePawnOpenE, -9), P(sidePawnOpenF, 8),
        P(sidePawnOpenG, 25), P(sidePawnOpenH, 52),

        P(middlePawnClosedA, 0), P(middlePawnClosedB, -10),
        P(middlePawnClosedC, 3), P(middlePawnClosedD, 16),
        P(middlePawnClosedE, 19), P(middlePawnClosedF, 7),
        P(middlePawnClosedG, -17), P(middlePawnClosedH, 0),

        P(middlePawnOpenA, 0), P(middlePawnOpenB, 16),
        P(middlePawnOpenC, 31), P(middlePawnOpenD, 40),
        P(middlePawnOpenE, 55), P(middlePawnOpenF, 6),
        P(middlePawnOpenG, 2), P(middlePawnOpenH, 0),

        P(duoPawnA, -22), P(duoPawnB, 7),
        P(duoPawnC, -13), P(duoPawnD, 53),
        P(duoPawnE, 42), P(duoPawnF, 13),
        P(duoPawnG, 49), P(duoPawnH, -67),

        // Quadratic polynomials for passers
        P(passerA_0, -84), P(passerB_0, -24), P(passerC_0, 4), P(passerD_0, -85),
        P(passerE_0, 13), P(passerF_0, 35), P(passerG_0, 3), P(passerH_0, 43),

        P(passerA_1, 94), P(passerB_1, 34), P(passerC_1, -6), P(passerD_1, 9),
        P(passerE_1, -38), P(passerF_1, -69), P(passerG_1, -22), P(passerH_1, 20),

        P(passerA_2, 154), P(passerB_2, 218), P(passerC_2, 235), P(passerD_2, 207),
        P(passerE_2, 223), P(passerF_2, 248), P(passerG_2, 198), P(passerH_2, 144),

        P(passerScalingOffset, 5099),
        P(passerAndQueen, -453),
        P(passerAndRook, -373),
        P(passerAndBishop, 127),
        P(passerAndKnight, -170),
        P(passerAndPawn, -589),
        P(passerVsQueen, 1014),
        P(passerVsRook, 38),
        P(passerVsBishop, -551),
        P(passerVsKnight, -391),
        P(passerVsPawn, -4),

#if 0
        P(drawBlockedA, 0), P(drawBlockedB, 0), P(drawBlockedC, 0), P(drawBlockedD, 0),
        P(drawBlockedE, 0), P(drawBlockedF, 0), P(drawBlockedG, 0), P(drawBlockedH, 0),

        P(drawPasser_0, 0), P(drawPasser_1, 0), P(drawPasser_2, 0),
#endif

        /*
         *  Knights
         */

        P(knightByFile_0, -154), // fileA
        P(knightByFile_1, -198),
        P(knightByFile_2, -215),
        P(knightByFile_3, -152),
        P(knightByFile_4, -87),
        P(knightByFile_5, -19),
        P(knightByFile_6, 25), // -fileH

        P(knightByRank_0, -100), // rank1
        P(knightByRank_1, -173),
        P(knightByRank_2, -198),
        P(knightByRank_3, -134),
        P(knightByRank_4, -10),
        P(knightByRank_5, 129),
        P(knightByRank_6, 198), // -rank8

        /*
         *  Bishops
         */

        P(bishopOnLong_0, 4),
        P(bishopOnLong_1, 33),

        P(bishopByFile_0, -23), // fileA
        P(bishopByFile_1, -31),
        P(bishopByFile_2, -22),
        P(bishopByFile_3, -14),
        P(bishopByFile_4, 2),
        P(bishopByFile_5, 0),
        P(bishopByFile_6, 9), // -fileH

        P(bishopByRank_0, -96), // rank1
        P(bishopByRank_1, -118),
        P(bishopByRank_2, -101),
        P(bishopByRank_3, -76),
        P(bishopByRank_4, -41),
        P(bishopByRank_5, 49),
        P(bishopByRank_6, 25), // -rank8

        /*
         *  Rooks
         */

        P(rookByFile_0, -74), // fileA
        P(rookByFile_1, -77),
        P(rookByFile_2, -50),
        P(rookByFile_3, -11),
        P(rookByFile_4, 33),
        P(rookByFile_5, 52),
        P(rookByFile_6, 90), // -fileH

        P(rookByRank_0, -136), // rank1
        P(rookByRank_1, -335),
        P(rookByRank_2, -492),
        P(rookByRank_3, -540),
        P(rookByRank_4, -447),
        P(rookByRank_5, -275),
        P(rookByRank_6, -108), // -rank8

        /*
         *  Queens
         */

        P(queenByFile_0, -60), // fileA
        P(queenByFile_1, -100),
        P(queenByFile_2, -102),
        P(queenByFile_3, -93),
        P(queenByFile_4, -79),
        P(queenByFile_5, -64),
        P(queenByFile_6, -48), // -fileH

        P(queenByRank_0, -144), // rank1
        P(queenByRank_1, -249),
        P(queenByRank_2, -337),
        P(queenByRank_3, -383),
        P(queenByRank_4, -325),
        P(queenByRank_5, -178),
        P(queenByRank_6, -110), // -rank8

        /*
         *  Kings
         */

        P(kingByFile_0, -5), // fileD
        P(kingByFile_1, -26),
        P(kingByFile_2, 10), // -fileH

        P(kingByRank_0, -126), // rank1
        P(kingByRank_1, -235),
        P(kingByRank_2, -322),
        P(kingByRank_3, -297),
        P(kingByRank_4, -150),
        P(kingByRank_5, 82),
        P(kingByRank_6, 219), // -rank8


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
        P(drawOffset, -4),
        P(drawQueen, -2060),
        P(drawRook, 210),
        P(drawBishop, -13),
        P(drawKnight, 12),
        P(drawPawn, -832),
        P(drawQueenEnding, 1685),
        P(drawRookEnding, 464),
        P(drawKnightEnding, 766),
        P(drawBishopEnding, 1723),
        P(drawPawnEnding, -16553),
        P(drawPawnless, 361),

        P(drawQueenImbalance, 985),
        P(drawRookImbalance, 132),
        P(drawMinorImbalance, 264),

        P(drawUnlikeBishops, 3848),
        P(drawUnlikeBishopsAndQueens, -432),
        P(drawUnlikeBishopsAndRooks, 446),
        P(drawUnlikeBishopsAndKnights, 1171),
        // TODO: pawn and pawn block square colors


// }

/*----------------------------------------------------------------------+
 |                                                                      |
 +----------------------------------------------------------------------*/

