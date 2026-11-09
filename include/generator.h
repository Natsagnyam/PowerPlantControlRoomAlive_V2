#ifndef GENERATOR_H
#define GENERATOR_H

#include <stddef.h>

typedef struct {
    double pm;             // Active power setpoint (pu)
    double ef;             // Excitation voltage (pu)
    double p_calc;         // Calculated Active Power
    double q_calc;         // Calculated Reactive Power
    double delta;          // Power angle (radians)
    double pf;             // Power factor
    int is_lead;           // 1 if leading, 0 if lagging
    
    // Limit Alarms
    int mw_alarm;
    int rotor_alarm;
    int stator_alarm;
    int pole_slip_alarm;
} GeneratorState;

typedef struct {
    double xd;
    double xq;
} GeneratorParams;

void generator_init(GeneratorParams *params);
void update_generator(GeneratorState *gen, double new_pm, double new_ef);
void generator_to_json(const GeneratorState *gen, char *buffer, size_t len);

#endif
