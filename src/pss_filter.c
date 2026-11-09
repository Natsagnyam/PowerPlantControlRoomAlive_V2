#include "pss_filter.h"

void pss_init(PSSController *pss, double t1, double t2) {
    pss->t1 = t1;
    pss->t2 = t2;
    pss->dc_offset = 0.5;
}

double pss_process(PSSController *pss, double raw_omega) {
    double line1 = raw_omega;
    double line3 = line1 - pss->dc_offset; // Washout (DC removed)
    double line4 = line3 * (pss->t1 / pss->t2); // Lead-Lag
    
    // Output limiter (+/- 0.25 pu)
    double line5 = line4;
    if (line5 > 0.25) line5 = 0.25;
    if (line5 < -0.25) line5 = -0.25;

    return line5;
}
