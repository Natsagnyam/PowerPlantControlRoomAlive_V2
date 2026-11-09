#include "sysfs_led.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define LED3_PATH "/sys/class/leds/beaglebone:green:usr3"

int led3_init(void) {
    char path[128];
    snprintf(path, sizeof(path), "%s/trigger", LED3_PATH);
    int fd = open(path, O_WRONLY);
    if (fd >= 0) {
        write(fd, "none", 4);
        close(fd);
        return 0;
    }
    return -1;
}

void led3_set_state(int state) {
    char path[128];
    snprintf(path, sizeof(path), "%s/brightness", LED3_PATH);
    int fd = open(path, O_WRONLY);
    if (fd >= 0) {
        char val = state ? '1' : '0';
        write(fd, &val, 1);
        close(fd);
    }
}

void led3_close(void) {
    led3_set_state(0);
}
