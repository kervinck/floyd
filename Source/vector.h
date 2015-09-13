
/*----------------------------------------------------------------------+
 |                                                                      |
 |      vector.h                                                        |
 |                                                                      |
 +----------------------------------------------------------------------*/

/*
 *  This header is included three times from within evaluate.c, each time
 *  with a different definition for the macro X for a different use case:
 *  1. to define enum identifiers for the evaluation coefficents
 *  2. to generate a table with names so that these are available at runtime
 *  3. to generate a vector with default values
 *
 *  Some naming conventions used are:
 *
 *   Suffix '[A-H]' refers to the file, provided the side's king is on the king flank.
 *   Otherwise it is flipped.
 *
 *   Suffix '_[0-2]' refers to higher order terms (constant, linear, quadratic)
 */

// {
        X(eloDiff, 0),
        X(tempo, 0),
        X(winBonus, 3500),

        X(queenValue, 9000),
        X(rookValue, 5000),
        X(bishopValue, 3300),
        X(knightValue, 3200),
        X(pawnValue1, 1500), X(pawnValue2, 1400), X(pawnValue3, 1300), X(pawnValue4, 1200),
        X(pawnValue5, 1100), X(pawnValue6, 1000), X(pawnValue7, 900), X(pawnValue8, 800),

        X(queenAndQueen, 0),
        X(queenAndRook, 0),
        X(queenAndBishop, 0),
        X(queenAndKnight, 0),
        X(queenAndPawn_1, 0), X(queenAndPawn_2, 0),
        X(rookAndRook, 0),
        X(rookAndBishop, 0),
        X(rookAndKnight, 0),
        X(rookAndPawn_1, 0), X(rookAndPawn_2, 0),
        X(bishopAndBishop, 200),
        X(bishopAndKnight, 0),
        X(bishopAndPawn_1, 0), X(bishopAndPawn_2, 0),
        X(knightAndKnight, 0),
        X(knightAndPawn_1, 0), X(knightAndPawn_2, 0),

        X(queenVsRook, 0),
        X(queenVsBishop, 0),
        X(queenVsKnight, 0),
        X(queenVsPawn_1, 0), X(queenVsPawn_2, 0),
        X(rookVsBishop, 0),
        X(rookVsKnight, 0),
        X(rookVsPawn_1, 0), X(rookVsPawn_2, 0),
        X(bishopVsKnight, 0),
        X(bishopVsPawn_1, 0), X(bishopVsPawn_2, 0),
        X(knightVsPawn_1, 0), X(knightVsPawn_2, 0),

        X(controlCenter, 60),
        X(controlExtendedCenter, 50),
        X(controlOutside, 40),

        X(pawnA, 0), X(pawnB, 0), X(pawnC, 0), X(pawnD, 0),
        X(pawnE, 0), X(pawnF, 0), X(pawnG, 0), X(pawnH, 0),
        X(pawn2, 0), X(pawn3, 0), X(pawn4, 0), X(pawn5, 0), X(pawn6, 0), X(pawn7, 0),

        X(doubledPawnA, -100), X(doubledPawnB, -100), X(doubledPawnC, -100), X(doubledPawnD, -100),
        X(doubledPawnE, -100), X(doubledPawnF, -100), X(doubledPawnG, -100), X(doubledPawnH, -100),

        X(backwardPawnA, 0), X(backwardPawnB, 0), X(backwardPawnC, 0), X(backwardPawnD, 0),
        X(backwardPawnE, 0), X(backwardPawnF, 0), X(backwardPawnG, 0), X(backwardPawnH, 0),

        X(passerA_0, 0), X(passerB_0, 0), X(passerC_0, 0), X(passerD_0, 0),
        X(passerE_0, 0), X(passerF_0, 0), X(passerG_0, 0), X(passerH_0, 0),
        X(passerA_1, 0), X(passerB_1, 0), X(passerC_1, 0), X(passerD_1, 0),
        X(passerE_1, 0), X(passerF_1, 0), X(passerG_1, 0), X(passerH_1, 0),
        X(passerA_2, 0), X(passerB_2, 0), X(passerC_2, 0), X(passerD_2, 0),
        X(passerE_2, 0), X(passerF_2, 0), X(passerG_2, 0), X(passerH_2, 0),

#if 0
        X(drawBlockedA, 0), X(drawBlockedB, 0), X(drawBlockedC, 0), X(drawBlockedD, 0),
        X(drawBlockedE, 0), X(drawBlockedF, 0), X(drawBlockedG, 0), X(drawBlockedH, 0),

        X(drawPasser_0, 0), X(drawPasser_1, 0), X(drawPasser_2, 0),
#endif

#if 0
        X(attackForceQueen, 0),  X(attackForceQueenX, 0),
        X(attackForceRook, 0),   X(attackForceRookX, 0),
        X(attackForceBishop, 0), X(attackForceBishopX, 0),
        X(attackForceKnight, 0), X(attackForceKnightX, 0),
        X(attackForcePawn, 0),   X(attackForcePawnX, 0),

        X(defenceForceQueen, 0),  X(defenceForceQueenX, 0),
        X(defenceForceRook, 0),   X(defenceForceRookX, 0),
        X(defenceForceBishop, 0), X(defenceForceBishopX, 0),
        X(defenceForceKnight, 0), X(defenceForceKnightX, 0),
        X(defenceForcePawn, 0),   X(defenceForcePawnX, 0),

        X(passerOffset, 0),
        X(passerAndQueen, 0),
        X(passerAndRook, 0),
        X(passerAndBishop, 0),
        X(passerAndKnight, 0),
        X(passerAndPawn, 0),
        X(passerVsQueen, 0),
        X(passerVsRook, 0),
        X(passerVsBishop, 0),
        X(passerVsKnight, 0),
        X(passerVsPawn, 0),
#endif

        // Drawness. Positive is more drawish. Negative is more sharp
        X(drawOffset, 0),
        X(drawQueen, 0),
        X(drawRook, 0),
        X(drawBishop, 0),
        X(drawKnight, 0),
        X(drawPawn, 0),
        X(drawQueenEnding, 0),
        X(drawRookEnding, 0),
        X(drawKnightEnding, 0),
        X(drawBishopEnding, 0),
        X(drawPawnEnding, 0),
        X(drawPawnless, 0),

        X(drawQueenImbalance, 0),
        X(drawRookImbalance, 0),
        X(drawMinorImbalance, 0),

        X(drawUnlikeBishops, 0),
        X(drawUnlikeBishopsAndQueens, 0),
        X(drawUnlikeBishopsAndRooks, 0),
        X(drawUnlikeBishopsAndKnights, 0),
        // TODO: pawn and pawn block square colors


// }

/*----------------------------------------------------------------------+
 |                                                                      |
 +----------------------------------------------------------------------*/

