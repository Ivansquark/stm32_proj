#include "alarm.h"
Alarm  alarm = {0};

uint8_t is_day_bigger(Alarm* alarm) {
    if(alarm->day_stop > alarm->day_start) {
        return 1;
    } else return 0;
}

//! set 1 on alarm time - one second (to clear flag on next second)
uint8_t check_alarm_start() {
    if(alarm.turn_state && alarm.day_start <= d_t_dec.dayweek && alarm.day_stop >= d_t_dec.dayweek ) {
        if(alarm.hour == 0 && alarm.minute == 0) {
            if(d_t_dec.hour == 23 && d_t_dec.minute == 59 && d_t_dec.second == 59) {
                return 1;
            } else return 0;
        } else if(alarm.minute == 0) {
            if(((d_t_dec.hour-1) == alarm.hour) && (d_t_dec.second == 59)) {
                return 1;
            } else return 0;
        } else {
            if(d_t_dec.hour == alarm.hour && d_t_dec.second == 59) {            
                if(d_t_dec.minute-1 == alarm.minute) {
                    return 1;
                } else return 0;                        
            } else return 0;
        }        
    } else return 0;    
}