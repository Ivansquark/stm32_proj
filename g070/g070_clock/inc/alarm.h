#ifndef ALARM_H
#define ALARM_H

#include "ds3231.h"
#include <stdint.h>

typedef struct ALARM {
    uint8_t hour;
    uint8_t minute;
    uint8_t day_start;
    uint8_t day_stop;
    uint8_t turn_state;
} Alarm;

extern Alarm alarm;

uint8_t is_day_bigger(Alarm* alarm);
extern dateTime d_t_dec;
uint8_t check_alarm_start();

#endif //ALARM_H