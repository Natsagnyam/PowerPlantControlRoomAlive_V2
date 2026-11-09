#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include "generator.h"
#include "sysfs_led.h"
#include "pss_filter.h"
#include "telemetry.h"

int keep_running = 1;
GeneratorState gen_state;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void handle_sigint(int sig) {
    (void)sig;
    keep_running = 0;
}

void* blinker_thread(void* arg) {
    (void)arg;
    int led_state = 0;
    while (keep_running) {
        pthread_mutex_lock(&lock);
        int alarm = gen_state.mw_alarm || gen_state.rotor_alarm || 
                    gen_state.stator_alarm || gen_state.pole_slip_alarm;
        pthread_mutex_unlock(&lock);

        if (alarm) {
            led_state = !led_state;
            led3_set_state(led_state);
            usleep(125000); // 4Hz flash on alarm
        } else {
            led3_set_state(0);
            usleep(200000);
        }
    }
    led3_set_state(0);
    return NULL;
}

int main(int argc, char *argv[]) {
    (void)argc; (void)argv;
    signal(SIGINT, handle_sigint);

    printf("====================================================\n");
    printf(" BBB Synchronous Generator Control & Telemetry Daemon\n");
    printf(" Author: Natska Natsagnyam NAMKHAI\n");
    printf("====================================================\n");

    if (led3_init() != 0) {
        printf("[Warning] LED3 sysfs not writable. Running simulation check.\n");
    }

    if (telemetry_server_init(9000) == 0) {
        printf("[Info] Telemetry TCP server listening on port 9000\n");
    } else {
        printf("[Error] Failed to bind telemetry port 9000\n");
    }

    pthread_t thread;
    pthread_create(&thread, NULL, blinker_thread, NULL);

    int cycle = 0;
    while (keep_running) {
        pthread_mutex_lock(&lock);
        double test_pm = 0.6 + 0.1 * (cycle % 6);
        update_generator(&gen_state, test_pm, 1.4);
        
        printf("[Cycle %02d] P=%.2f pu, Q=%.2f pu, Delta=%.1f deg | Alarms -> MW:%d Rotor:%d Stator:%d PoleSlip:%d\n",
               cycle, gen_state.p_calc, gen_state.q_calc, 
               gen_state.delta * 180.0 / 3.14159,
               gen_state.mw_alarm, gen_state.rotor_alarm, 
               gen_state.stator_alarm, gen_state.pole_slip_alarm);
        pthread_mutex_unlock(&lock);

        sleep(1);
        cycle++;
    }

    keep_running = 0;
    telemetry_server_stop();
    pthread_join(thread, NULL);
    led3_close();
    printf("Daemon exited cleanly.\n");
    return 0;
}
