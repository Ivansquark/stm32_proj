#ifndef FLASH_H
#define FLASH_H

#include "stm32g070xx.h"

#define FLASH_PAGE_SIZE 2048 // 0x800
#define FLASH_PAGE_NUMBER 63
#define FLASH_START 0x80000000
#define FLASH_LAST_PAGE 0x0801F800
#define FLASH_PAGE FLASH_LAST_PAGE

//! @brief 0:RED 1:GREEN 2:BLUE   
//! @brief 3:ALARM_HOUR 4:ALARM_MINUTE 5:ALARM_DAY_START 6:ALARM_DAY_STOP 7:ALARM_TURN_STATE
//! @brief 8:BANG_HOUR_START 9:BANG_HOUR_STOP 10:BANG_TURN_STATE 11:0 12:0 13:0 14:0 15:0
typedef struct FLASH_DATA {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t auto_brightness;
    uint8_t alarm_hour;
    uint8_t alarm_minute;
    uint8_t alarm_day_start;
    uint8_t alarm_day_stop;
    uint8_t alarm_turn_state;
    uint8_t bang_hour_start;
    uint8_t bang_hour_stop;
    uint8_t bang_turn_state; 
    uint8_t zero12; uint8_t zero13; uint8_t zero14; uint8_t zero15;   // aligned double word
} Flash_Data;

extern Flash_Data flash_data;

void flash_unlock();
void flash_lock();
void flash_erase_page();
void flash_read_bytes(uint8_t* arr, uint8_t bytes_num);
void flash_write_bytes(uint8_t* arr, uint8_t bytes_num);
void flash_read_data();
void flash_write_data(uint8_t* arr);
#endif //FLASH_H