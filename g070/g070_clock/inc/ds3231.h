#ifndef DS3231_H
#define DS3231_H

#include "stm32g070xx.h"

typedef struct DateTime {
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
    uint8_t dayweek;
    uint8_t day;
    uint8_t month;
    uint8_t year;
} dateTime;

enum WeekDay {
    MO,
    TU,
    WE,
    TH,
    FR,
    SA,
    SU
};

void ds3231_init();
uint8_t i2c_read_byte(void);
uint8_t i2c_write_byte(uint8_t dat);
uint8_t write_i2c_reg(uint8_t reg, uint8_t data);
uint8_t read_i2c_regs(uint8_t start_reg, uint32_t *data, uint8_t nbytes);

void i2c_stop(void);
void i2c_start(void);
uint8_t dec_to_bcd(uint8_t dec);
uint8_t bcd_to_dec(uint8_t bcd);
void setSecond(uint8_t sec);
void setMinute(uint8_t min);
void setHour(uint8_t hour);
void setTime(uint8_t hour, uint8_t min, uint8_t sec);
void setDayWeek(uint8_t dayweek);
void setDay(uint8_t day);
void setMonth(uint8_t month);
void setYear(uint8_t year);
void setDate(uint8_t day, uint8_t month, uint8_t year, uint8_t dayweek);
void readDateTime(uint32_t* dateTimeArray);

#endif //DS3231_H