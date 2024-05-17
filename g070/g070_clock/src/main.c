#include "main.h"

void delay(volatile uint32_t x);

extern volatile uint32_t ms_counter;

int main(void) {
    rcc_init(64);
    scheduler_init();
    led_init();    
    delay(5000000);
    lcd_init();
    delay(4000000);
    lcd_init();
    delay(4000000);
    lcd_init();
    ds3231_init();
    buttons_init();
    bang_init();
    adc_init();
    menu_init();
    ds1822_init();
    __enable_irq();    
    CS_1	
	GLCD_LoadFont8x8_();
    CS_2	
	GLCD_LoadFont8x8_();
    diode_blink1();
    flash_read_data(&flash_data,16);
    set_all_diods_color(flash_data.red, flash_data.green, flash_data.blue);
    while(1) {   
        //_________________ Buttons state machine____________________________
        if(screen_state == CURRENT_TIME) {
            if(enter_short_pressed_flag) {
                enter_short_pressed_flag = 0; diode_blink1();
            } else if(up_short_pressed_flag) {
                up_short_pressed_flag = 0; diode_blink1();
                screen_state = CURRENT_DATE;   
            } else if(down_short_pressed_flag) {
                down_short_pressed_flag = 0; diode_blink1();
                screen_state = CURRENT_TEMPERATURE; 
            }
        } else if(screen_state == ALARM_WORK) {
            if(enter_short_pressed_flag || enter_long_pressed_flag ||
            up_short_pressed_flag || up_long_pressed_flag ||
            down_short_pressed_flag || down_long_pressed_flag) {
                screen_state = CURRENT_TIME; //alarm turn off
            }
        }
           
        if(enter_long_pressed_flag) {
            if(screen_state == CURRENT_TIME) {
                enter_long_pressed_flag = 0; enter_short_pressed_flag = 0;
                screen_state = MAIN_MENU;
                CS_1 GLCD_ClearGraphic(); GLCD_ClearText();
            }            
        }
        //__________________ Screen and MENU state machine ___________________________
        if(ms_counter == 1000) {
            switch(screen_state) {
                case CURRENT_TIME: { 
                        readDateTime((uint32_t*)&d_t);
                        bcd_to_dec_struct(&d_t, &d_t_dec);
                        drawNum_left(bcd_to_dec(d_t.hour),0);
                        drawNum_right(bcd_to_dec(d_t.minute),0);
                        set_auto_brigtness();
                        if(check_alarm_start()) {
                            screen_state = ALARM_WORK;
                        } else if(check_bang_start()) {
                            bang_bang();
                        }
                        if(d_t_dec.second%15 == 0) {
                            screen_state = CURRENT_DATE;
                        } else if(d_t_dec.second%17 == 0) {
                            screen_state = CURRENT_TEMPERATURE;
                        }
                        //uint16_t a = adc_read();
                        //drawNum_left(a/100,0);
                        //drawNum_right(a%100,0);
                        ms_counter = 500;
                } break;
                case MAIN_MENU: CS_1 main_menu(); ms_counter = 970; break;
                case RGB_MENU: CS_1 RGB_menu(); ms_counter = 970; break;  
                case TIME_MENU: CS_1 time_menu(); ms_counter = 970; break;
                case DATE_MENU:  CS_1 date_menu(); ms_counter = 970; break;
                case ALARM_MENU: CS_1 alarm_menu(); ms_counter = 970; break;
                case BANG_MENU: CS_1 bang_menu(); ms_counter = 970; break;
                case CURRENT_DATE: {
                    if((d_t_dec.second%15 != 0) || (d_t_dec.second%15 != 1) || (d_t_dec.second%15 != 2)) {
                            screen_state = CURRENT_TIME;
                    }
                    readDateTime((uint32_t*)&d_t);
                    bcd_to_dec_struct(&d_t, &d_t_dec);
                    drawNum_left(d_t_dec.day,0);
                    drawNum_right(d_t_dec.month,0);
                    set_all_diods_color(1,0,0);
                    ms_counter = 0;
                } break;
                case CURRENT_TEMPERATURE: {
                    if((d_t_dec.second%17 != 0) || (d_t_dec.second%17 != 1) || (d_t_dec.second%17 != 2)) {
                        screen_state = CURRENT_TIME;
                    }
                    ds_1822_read_temperature();
                    drawNum_left(temp1822.integer,0);
                    drawNum_right(temp1822.fractional,0);
                    set_all_diods_color(1,1,0);
                    ms_counter = 0; 
                } break;
                case ALARM_WORK: {
                    if(bang_counter == 100) { //how many bangs will executed
                        bang_counter = 0;
                        screen_state = CURRENT_TIME;
                    } else {
                        bang_counter++; 
                    }
                    bang_bang();
                    ms_counter = 0; //bang each second
                } break;
                default: ms_counter = 0; break;
            }                       
        }            
    }    
    return 0;
}

void delay(volatile uint32_t x) {
    while(x--);
}