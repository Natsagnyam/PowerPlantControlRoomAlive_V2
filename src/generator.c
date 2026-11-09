#include "generator.h"
#include <math.h>
#include <stdio.h>

#define Xd 1.1
#define Xq 0.7
#define Vt 1.0
#define P_MAX 1.1
#define STATOR_SMAX 1.15

void generator_init(GeneratorParams *params) {
    (void)params;
}

void update_generator(GeneratorState *gen, double new_pm, double new_ef) {
    gen->pm = new_pm;
    gen->ef = new_ef;

    double arg = (gen->pm * Xd) / (gen->ef * Vt);
    if (arg > 1.0) arg = 1.0;
    if (arg < -1.0) arg = -1.0;
    gen->delta = asin(arg);

    gen->p_calc = (gen->ef * Vt / Xd) * sin(gen->delta) + (Vt * Vt / 2.0) * (1.0/Xq - 1.0/Xd) * sin(2.0 * gen->delta);
    gen->q_calc = (gen->ef * Vt / Xd) * cos(gen->delta) - (Vt * Vt / Xd);

    double apparent_s = sqrt(gen->p_calc * gen->p_calc + gen->q_calc * gen->q_calc);
    gen->pf = gen->p_calc / apparent_s;
    gen->is_lead = (gen->q_calc < 0) ? 1 : 0;

    gen->mw_alarm = (gen->p_calc >= P_MAX) ? 1 : 0;
    gen->rotor_alarm = (gen->ef > 2.0) ? 1 : 0;
    gen->stator_alarm = (apparent_s > STATOR_SMAX) ? 1 : 0;
    gen->pole_slip_alarm = (gen->q_calc < (- (Vt * Vt / Xq) + 0.1) || gen->delta >= (85.0 * M_PI / 180.0)) ? 1 : 0;
}

void generator_to_json(const GeneratorState *gen, char *buffer, size_t len) {
    snprintf(buffer, len,
        "{\n"
        "  \"author\": \"Natska Natsagnyam NAMKHAI\",\n"
        "  \"active_power_mw\": %.2f,\n"
        "  \"reactive_power_mvar\": %.2f,\n"
        "  \"power_angle_deg\": %.1f,\n"
        "  \"power_factor\": %.2f,\n"
        "  \"is_leading\": %d,\n"
        "  \"alarms\": {\n"
        "    \"mw_limit\": %d,\n"
        "    \"rotor_overcurrent\": %d,\n"
        "    \"stator_overcurrent\": %d,\n"
        "    \"pole_slip\": %d\n"
        "  }\n"
        "}\n",
        gen->p_calc, gen->q_calc,
        gen->delta * 180.0 / M_PI,
        gen->pf, gen->is_lead,
        gen->mw_alarm, gen->rotor_alarm,
        gen->stator_alarm, gen->pole_slip_alarm
    );
}
