#ifndef RTC_H
#define RTC_H

#include "stm32h743xx.h"

class Rtc {
public:
static Rtc* pThis;
    struct Time {
        uint8_t second;
        uint8_t minute;
        uint8_t hour;
    };
    Time currentTime;
    struct Date {
        uint8_t year;
        uint8_t month;
        uint8_t day;
        uint8_t weekDay; // sets days of the week mon, tue,...
    };
    Date currentDate;
public:
    Rtc();
    void setTime(uint8_t hour, uint8_t minute, uint8_t second);
    void setDate(uint8_t year, uint8_t month, uint8_t day, uint8_t wd);
    Time* getTime();
    Date* getDate();
private:
    void init();
    void LSE_init();
    
};

#endif //RTC_H