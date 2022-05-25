#ifndef MENU_H
#define MENU_H

/** * @brief MAIN MENU
 * @brief main menu 0: set RGB brightness
 * @brief main menu 1: set time
 * @brief main menu 2: set date
 * @brief main menu 3: set alarm
 * @brief main menu 4: set days period BANG
 * @brief main menu 5: EXIT
 * **/
/** * @brief RGB BRIGTNESS
 * @brief RGB brightness 0: set RED brightness
 * @brief RGB brightness 1: set GREEN brightness
 * @brief RGB brightness 2: set BLUE brightness
 * @brief RGB brightness 3: SET_AUTO_STATE
 * @brief RGB brightness 4: EXIT
 * **/
/** * @brief TIME
 * @brief set time 0: set hour
 * @brief set time 1: set minute
 * @brief set time 2: set second
 * @brief set time 3: EXIT
 * **/
/** * @brief DATE
 * @brief set time 0: set day
 * @brief set time 1: set month
 * @brief set time 2: set year
 * @brief set time 3: EXIT
 * **/
/** * @brief ALARM
 * @brief set alarm 0: set hour
 * @brief set alarm 1: set minute
 * @brief set alarm 2: set day start
 * @brief set time 3: set day end
 * @brief set time 4: EXIT
 * **/
/** * @brief BANG HOURS
 * @brief set time 0: set hour start bang
 * @brief set time 1: set hour stop bang
 * @brief set time 2: EXIT
 * **/

#include "sed1335.h"
#include "buttons.h"
#include "ds3231.h"
#include "led.h"
#include "flash.h"
#include "alarm.h"
#include "bang.h"

#include <stdint.h>

enum screen_states {
    CURRENT_TIME,
    MAIN_MENU,
    RGB_MENU,
    TIME_MENU,
    DATE_MENU,
    ALARM_MENU,
    BANG_MENU,
    CURRENT_DATE,
    CURRENT_TEMPERATURE,
    ALARM_WORK
};
enum main_menu_states {
    SET_RGB,
    SET_TIME,
    SET_DATE,
    SET_ALARM,
    SET_BANG,
    MAIN_MENU_EXIT
};
enum RGB_menu_states {
    SET_RED,
    SET_GREEN,
    SET_BLUE,
    SET_AUTO_STATE,
    RGB_MENU_EXIT
};
enum time_menu_states {
    SET_HOUR,
    SET_MINUTE,
    SET_SECOND,
    TIME_MENU_EXIT
};
enum date_menu_states {
    SET_DAY,
    SET_MONTH,
    SET_YEAR,
    SET_DAYWEEK,
    DATE_MENU_EXIT
};
enum alarm_menu_states {
    SET_ALARM_HOUR,
    SET_ALARM_MINUTE,
    SET_ALARM_DAY_START,
    SET_ALARM_DAY_STOP,
    SET_ALARM_TURN,
    ALARM_MENU_EXIT
};
enum bang_menu_states {
    SET_BANG_HOUR_START,
    SET_BANG_HOUR_STOP,
    SET_BANG_TURN,
    BANG_MENU_EXIT
};

extern uint8_t screen_state;

static uint8_t main_menu_state;
static uint8_t RGB_menu_state;
static uint8_t time_menu_state;
static uint8_t date_menu_state;
static uint8_t alarm_menu_state;
static uint8_t bang_menu_state;

static uint8_t menu_set_value_flag;

extern uint8_t red_brightness;
extern uint8_t green_brightness;
extern uint8_t blue_brightness;
extern uint8_t auto_brightness;
extern char temp_arr[];
extern dateTime d_t;
extern dateTime d_t_dec;

void bcd_to_dec_struct(dateTime* d_t, dateTime* d_t_dec);
void dec_to_bcd_struct(dateTime* d_t_dec, dateTime* d_t);
void int_to_char_array(int value, char* arr);
void menu_init();
void daynumber_to_array(uint8_t day_num, char* arr);

void main_menu();
void draw_main_menu();
void main_menu_buttons_handler();

void RGB_menu();
void draw_RGB_menu();
void RGB_menu_buttons_handler();

void time_menu();
void draw_time_menu();
void time_menu_buttons_handler();

void date_menu();
void draw_date_menu();
void date_menu_buttons_handler();

void alarm_menu();
void draw_alarm_menu();
void alarm_menu_buttons_handler();

void bang_menu();
void draw_bang_menu();
void bang_menu_buttons_handler();

#endif //MENU_H