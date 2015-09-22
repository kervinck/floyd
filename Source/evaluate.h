
/*
 *  Definitions
 */

struct evaluation {

        /*
         *  Piece counts per side
         */
        int nrKings[2];
        int nrQueens[2];
        int nrRooks[2];
        int nrBishops[2];
        int nrKnights[2];
        int nrPawns[2];
        int nrMinors[2];

        /*
         *  Also counting bishops and pawns for only the "odd" square color.
         *  With that we can figure out unlike bishops etc
         */
        int nrBishopsX[2];
        int nrPawnsX[2];

        /*
         *  Maximum distance of pawn to rank1, for each file and both sides.
         *  And then the same from rank8 point of view.
         *  Used to detect open files, doubled pawns, passers, etc.
         */
        int maxPawnFromRank1[10][2];
        int maxPawnFromRank8[10][2];

        /*
         *  Outmost files with pawns, to extract pawn span and center.
         *  We don't calculate 'min' directly for easier initialization.
         */
        int maxPawnFromFileA[2]; 
        int maxPawnFromFileH[2]; 

        /*
         *  Material
         */
        int material[2];

        int attackForce[2][2];          // available material per flank and side
        int pawnShelter[8][2];          // pawn shelter per file
        int kingLocation[2];            // back rank is good, side is good. get added to shelter
        int kingAttacks[2];             // attacks on the king 3x3 area

                                        // something about attacking strength on the king flank
                                        // something about defenders. this is always a problem in rookie

        int safety[2];                  // total king safety

        int kings[2];                   // PST, distance to weak pawns, distance to passers (w+b)
        int queens[2];                  // PST
        int rooks[2];                   // PST, doubled, strong squares
        int bishops[2];                 // PST, trapped, strong squares, can engage enemy pawns, not blocked by own pawns, pawn span is good
        int knights[2];                 // PST, strong squares, pawn span is bad
        int pawns[2];                   // PST, doubled, grouped, mobile

        //int pawnBlocks;

        int mobility[2];           // use extended attack maps
        int control[2];            // control of each square

        int passerScaling[2];           // based on material
        int passers[2];                 // passers, not scalled

        // rooks behind passers (w+b)
        // rooks before passers (w+b)
        // distance from enemy king file
        // occupancy of front square
        // occupancy of second square
        // occupancy of any  square
        // control of front square
        // control of second square
        // control of any square
        // supported passer
        // connected passers

        int sideToMove;
        int contempt;                   // linked to own queen (typically for black)
                                        // "Positive values of contempt favor more "risky" play,"

        int wiloScore;
        int drawScore;
        double P;
        int score;
};

/*
 *  Data
 */

extern int globalVector[];
extern const int vectorLen;
extern const char * const vectorLabels[];

/*
 *  Functions
 */

int evaluate(Board_t self);

