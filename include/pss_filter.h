#ifndef PSS_FILTER_H
#define PSS_FILTER_H

typedef struct {
    double t1;             // Lead time constant
    double t2;             // Lag time constant
    double dc_offset;      // Washout DC offset
} PSSController;

void pss_init(PSSController *pss, double t1, double t2);
double pss_process(PSSController *pss, double raw_omega);

#endif
