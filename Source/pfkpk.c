
/*
 *  pfkpk.c -- KPK tester
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "Board.h" // to get geometry
#include "kpk.h"

struct {
        int side, wKing, wPawn, bKing, expected;
} tests[] = {
        { 0, a1, a2, a8, 0 },
        { 0, a1, a2, h8, 1 },
        { 1, a1, a2, a8, 0 },
        { 1, a1, a2, h8, -1 },
        { 1, a1, a2, g2, 0 },
        { 1, a1, a2, g1, -1 },
        { 0, a5, a4, d4, 1 },
        { 1, a5, a4, d4, 0 },
        { 0, a1, f4, a3, 1 },
        { 1, a1, f4, a3, 0 },
        { 1, a3, a4, f3, -1 },
        { 0, h6, g6, g8, 1 },
        { 0, h3, h2, b7, 1 },
        { 1, a5, a4, e6, 0 },
        { 1, f8, g6, h8, 0 },
        { 0, f6, g5, g8, 1 },
        { 0, d1, c3, f8, 1 },
        { 0, d4, c4, e6, 1 },
        { 0, c6, d6, d8, 1 },
        { 1, d6, e6, d8, -1 },
        { 0, g6, g5, h8, 1 },
        { 1, g6, g5, h8, -1 },
        { 0, e4, e3, e6, 0 },
        { 1, e4, e3, e6, -1 },
        { 1, h3, b2, h5, -1 },
        { 0, g2, b2, g5, 1 },
        { -1 },
};

int main(void)
{
        int err = 0;
        bool passed;

        /*
         *  kpkGenerate speed
         */
        clock_t start = clock();
        int size = kpkGenerate();
        clock_t finish = clock();
        printf("kpkGenerate CPU time [seconds]: %g\n",
                (double)(finish - start) / CLOCKS_PER_SEC);

        /*
         *  kpkTable size
         */
        printf("kpkTable size [bytes]: %d\n", size);

        /*
         *  kpkSelfCheck
         */
        passed = kpkSelfCheck();
        printf("kpkSelfCheck: %s\n", passed ? "OK" : "FAILED");
        if (!passed)
                err = EXIT_FAILURE;

        /*
         *  kpkProbe
         */
        int nrPassed = 0;
        int ix;
        for (ix=0; tests[ix].side>=0; ix++) {
                int result = kpkProbe(tests[ix].side,
                                      tests[ix].wKing,
                                      tests[ix].wPawn,
                                      tests[ix].bKing);
                if (result == tests[ix].expected)
                        nrPassed++;
        }
        passed = (nrPassed == ix);
        printf("kpkProbe %d/%d: %s\n", nrPassed, ix, passed ? "OK" : "FAILED");
        if (!passed)
                err = EXIT_FAILURE;

        return err;
}

