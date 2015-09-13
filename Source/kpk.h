
/*----------------------------------------------------------------------+
 |                                                                      |
 |      kpk.h -- pretty fast KPK endgame table generator                |
 |                                                                      |
 +----------------------------------------------------------------------*/

/*
 *  Use an externally defined board geometry to avoid conversions
 *  from the caller's own square indexing. All 8 orientations are
 *  supported by changing this header file. As a demonstration of
 *  this, geometry-h8g8.h is provided.
 */
//#include "geometry-a1a2.h"

/*----------------------------------------------------------------------+
 |      Functions                                                       |
 +----------------------------------------------------------------------*/

/*
 *  Probe a KPK position from the in memory endgame table.
 *  Returns 0 for draw, 1 for win and -1 for loss.
 *
 *  The position must be legal for meaningful results.
 *  `side' is 0 for white to move and 1 for black to move.
 * 
 *  If the table has not been generated yet, this will be
 *  done automatically at the first invocation.
 */
int kpkProbe(int side, int wKing, int wPawn, int bKing);

/*
 *  Explicitly generate the KPK table.
 *  Returns the memory size for info.
 *  This can take up to 2 milliseconds on a 2.6GHz Intel i7.
 */
int kpkGenerate(void);

/*
 *  Perform a self check on the bitbase.
 *  Returns 0 on failure, 1 for success.
 */
int kpkSelfCheck(void);

/*----------------------------------------------------------------------+
 |                                                                      |
 +----------------------------------------------------------------------*/

