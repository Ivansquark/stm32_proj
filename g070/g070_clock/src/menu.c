#include "menu.h"

uint8_t screen_state = 0;
static uint8_t main_menu_state = 0;
static uint8_t RGB_menu_state = 0;
static uint8_t time_menu_state = 0;
static uint8_t date_menu_state = 0;
static uint8_t alarm_menu_state = 0;
static uint8_t bang_menu_state = 0;

char temp_arr[10] = {0};
uint8_t red_brightness = 0;
uint8_t green_brightness = 0;
uint8_t blue_brightness = 0;
uint8_t auto_brightness = 0;

dateTime d_t; dateTime d_t_dec;
//!______________________ Auxiliary functions___________________________________
void menu_init() {
    flash_read_data();
    red_brightness = flash_data.red; green_brightness = flash_data.green; 
    blue_brightness = flash_data.blue; auto_brightness = flash_data.auto_brightness;
    alarm.hour = flash_data.alarm_hour; alarm.minute = flash_data.alarm_minute;
    alarm.day_start = flash_data.alarm_day_start; alarm.day_stop = flash_data.alarm_day_stop;
    alarm.turn_state = alarm.turn_state; bang.hour_start = flash_data.bang_hour_start;
    bang.hour_stop = flash_data.bang_hour_stop; bang.turn_state = flash_data.bang_turn_state;    
    for(int i = 0; i<10; i++) {temp_arr[i]=0;}
    d_t.second=1; d_t.minute=1; d_t.hour=1; d_t.day=1; d_t.month=1; d_t.year=1; d_t.dayweek=1;
}
void bcd_to_dec_struct(dateTime* d_t, dateTime* d_t_dec) {
    d_t_dec->hour = bcd_to_dec(d_t->hour); d_t_dec->minute = bcd_to_dec(d_t->minute);
    d_t_dec->second = bcd_to_dec(d_t->second);
    d_t_dec->year = bcd_to_dec(d_t->year); d_t_dec->month = bcd_to_dec(d_t->month);
    d_t_dec->day = bcd_to_dec(d_t->day); d_t->dayweek = d_t_dec->dayweek;
}
void dec_to_bcd_struct(dateTime* d_t_dec, dateTime* d_t) {
    d_t->hour = dec_to_bcd(d_t_dec->hour); d_t->minute = dec_to_bcd(d_t_dec->minute);
    d_t->second = dec_to_bcd(d_t_dec->second);
    d_t->year = dec_to_bcd(d_t_dec->year); d_t->month = dec_to_bcd(d_t_dec->month); 
    d_t->day = dec_to_bcd(d_t_dec->day); d_t->dayweek = d_t_dec->dayweek;
}
void int_to_char_array(int value, char* arr) {
    uint8_t count = 0;
    //! fill array with chars
    if(value == 0) {
        arr[0] = 0 + 0x30; arr[1] = '\0';
    } else {
        while(value) {
            arr[count] = value%10 + 0x30;
            value /= 10;
            count++;
        }
        arr[count] = '\0';
        //!reverse array
        for(int i=0; i<count/2; i++) {
            char temp = arr[i];
            arr[i] = arr[count-1-i];
            arr[count-1-i] = temp;
        }
    }
}
void daynumber_to_array(uint8_t day_num, char* arr) {
    switch(day_num) {
        case MO: arr[0] = '�'; arr[1] = '�'; arr[2] = '\0'; break;
        case TU: arr[0] = '�'; arr[1] = '�'; arr[2] = '\0'; break;
        case WE: arr[0] = '�'; arr[1] = '�'; arr[2] = '\0'; break;
        case TH: arr[0] = '�'; arr[1] = '�'; arr[2] = '\0'; break;
        case FR: arr[0] = '�'; arr[1] = '�'; arr[2] = '\0'; break;
        case SA: arr[0] = '�'; arr[1] = '�'; arr[2] = '\0'; break;
        case SU: arr[0] = '�'; arr[1] = '�'; arr[2] = '\0'; break;
    }
}

//!______________________ First menu _______________________________________________
void main_menu() {
    GLCD_ClearText();
    draw_main_menu();
    switch (main_menu_state) {
        case SET_RGB: {GLCD_WriteText(9,10,">");} break; 
        case SET_TIME: {GLCD_WriteText(9,12,">");} break;
        case SET_DATE: {GLCD_WriteText(9,14,">");} break;
        case SET_ALARM: {GLCD_WriteText(9,16,">");} break;
        case SET_BANG: {GLCD_WriteText(9,18,">");} break;
        case MAIN_MENU_EXIT: {GLCD_WriteText(9,20,">");} break;
        default: break;
    }
    main_menu_buttons_handler();
}
void draw_main_menu() {
    GLCD_WriteText(10,2,"����");
    GLCD_WriteText(10,10,"��������� �������");
    GLCD_WriteText(10,12,"��������� �������");
    GLCD_WriteText(10,14,"��������� ����");
    GLCD_WriteText(10,16,"��������� ����������");
    GLCD_WriteText(10,18,"��������� �����");
    GLCD_WriteText(10,20,"�����");
}

void main_menu_buttons_handler() {
    if(up_short_pressed_flag || up_long_pressed_flag) {
        up_short_pressed_flag = 0; up_long_pressed_flag = 0;
        if(main_menu_state == 0) {
            main_menu_state = 5;
        } else {
            main_menu_state--;
        }
        up_short_pressed_flag = 0; up_long_pressed_flag = 0;
    } else if(down_short_pressed_flag || down_long_pressed_flag) {
        down_short_pressed_flag = 0; down_long_pressed_flag = 0;
        if(main_menu_state == 5) {
            main_menu_state = 0;
        } else {
            main_menu_state++;
        }
    } else if(enter_short_pressed_flag || enter_long_pressed_flag) {
        enter_short_pressed_flag = 0; enter_long_pressed_flag = 0;
        switch(main_menu_state) {
            case SET_RGB:   {
                auto_brightness = 0; flash_read_data(&flash_data,16); //presetted brightness
                set_all_diods_color(flash_data.red,flash_data.green,flash_data.blue);
                GLCD_ClearGraphic(); GLCD_ClearText(); screen_state = RGB_MENU; 
            } break; 
            case SET_TIME:  {
                GLCD_ClearGraphic(); GLCD_ClearText(); 
                readDateTime((uint32_t*)&d_t); 
                bcd_to_dec_struct(&d_t, &d_t_dec);
                screen_state = TIME_MENU;                
             } break; 
            case SET_DATE:  {
                GLCD_ClearGraphic(); GLCD_ClearText(); 
                readDateTime((uint32_t*)&d_t); 
                bcd_to_dec_struct(&d_t, &d_t_dec);
                screen_state = DATE_MENU; 
            } break; 
            case SET_ALARM: GLCD_ClearGraphic(); GLCD_ClearText(); screen_state = ALARM_MENU; break;
            case SET_BANG:  GLCD_ClearGraphic(); GLCD_ClearText(); screen_state = BANG_MENU; break; 
            case MAIN_MENU_EXIT: {
                flash_data.red = red_brightness;
                flash_data.green = green_brightness;
                flash_data.blue = blue_brightness;
                flash_data.auto_brightness = auto_brightness;
                flash_data.alarm_hour = alarm.hour;
                flash_data.alarm_minute = alarm.minute;
                flash_data.alarm_day_start = alarm.day_start;
                flash_data.alarm_day_stop = alarm.day_stop;
                flash_data.alarm_turn_state = alarm.turn_state;
                flash_data.bang_hour_start = bang.hour_start;
                flash_data.bang_hour_stop = bang.hour_stop;
                flash_data.bang_turn_state = bang.turn_state;
                flash_write_data((uint8_t*)&flash_data);
                GLCD_ClearGraphic(); GLCD_ClearText(); screen_state = CURRENT_TIME;
            }  break; 
        }
    }
}
//! ____________________ RGB brightness preset menu_________________________________
void RGB_menu() {
    GLCD_ClearText();
    draw_RGB_menu();    
    if(menu_set_value_flag) {
        switch (RGB_menu_state) {
            case SET_RED: {GLCD_WriteText(9,10,"=");} break;
            case SET_GREEN: {GLCD_WriteText(9,12,"=");} break;
            case SET_BLUE: {GLCD_WriteText(9,14,"=");} break;
            case SET_AUTO_STATE: {GLCD_WriteText(9,16,"=");} break;
        }
    } else {
        switch (RGB_menu_state) {
            case SET_RED: {GLCD_WriteText(9,10,">");} break;
            case SET_GREEN: {GLCD_WriteText(9,12,">");} break;
            case SET_BLUE: {GLCD_WriteText(9,14,">");} break;
            case SET_AUTO_STATE: {GLCD_WriteText(9,16,">");} break;
            case RGB_MENU_EXIT: {GLCD_WriteText(9,20,">");} break; //EXIT
        }    
    }      
    RGB_menu_buttons_handler();  
}
void draw_RGB_menu() {
    GLCD_WriteText(10,2,"��������� �������");
    GLCD_WriteText(10,10,"�������"); int_to_char_array(red_brightness,temp_arr);
    GLCD_WriteText(18,10,temp_arr);
    GLCD_WriteText(10,12,"�������"); int_to_char_array(green_brightness,temp_arr); 
    GLCD_WriteText(18,12,temp_arr);
    GLCD_WriteText(10,14,"�����"); int_to_char_array(blue_brightness,temp_arr); 
    GLCD_WriteText(18,14,temp_arr);
    GLCD_WriteText(10,16,"����");  
    if(auto_brightness) {
        temp_arr[0]='�'; temp_arr[1]='�'; temp_arr[2]='�'; temp_arr[3]='\0'; 
        GLCD_WriteText(18,16,temp_arr);
    } else {
        temp_arr[0]='�'; temp_arr[1]='�'; temp_arr[2]='�'; temp_arr[3]='�'; temp_arr[4]='\0';
        GLCD_WriteText(18,16,temp_arr);
    }
    GLCD_WriteText(10,20,"�����");    
}
void RGB_menu_buttons_handler() {
    if(menu_set_value_flag) {
        switch(RGB_menu_state) {
            case SET_RED: { 
                if(up_short_pressed_flag || up_long_pressed_flag) {
                    up_short_pressed_flag=0; up_long_pressed_flag=0;
                    if(red_brightness > 9) {
                        red_brightness-=10;
                        set_all_diods_color(red_brightness,green_brightness,blue_brightness);    
                    }
                } else if(down_short_pressed_flag || down_long_pressed_flag) {
                    down_short_pressed_flag=0; down_long_pressed_flag=0;
                    if(red_brightness < 246) {
                        red_brightness+=10;
                        set_all_diods_color(red_brightness,green_brightness,blue_brightness);    
                    }
                } else if(enter_short_pressed_flag || enter_long_pressed_flag) {
                    enter_short_pressed_flag=0; enter_long_pressed_flag=0;
                    menu_set_value_flag = 0;
                    set_all_diods_color(red_brightness,green_brightness,blue_brightness);
                }       
            } break;
            case SET_GREEN: { 
                if(up_short_pressed_flag || up_long_pressed_flag) {
                    up_short_pressed_flag=0; up_long_pressed_flag=0;
                    if(green_brightness > 9) {
                        green_brightness-=10;
                        set_all_diods_color(red_brightness,green_brightness,blue_brightness);    
                    }
                } else if(down_short_pressed_flag || down_long_pressed_flag) {
                    down_short_pressed_flag=0; down_long_pressed_flag=0;
                    if(green_brightness < 246) {
                        green_brightness+=10;
                        set_all_diods_color(red_brightness,green_brightness,blue_brightness);    
                    }
                } else if(enter_short_pressed_flag || enter_long_pressed_flag) {
                    enter_short_pressed_flag=0; enter_long_pressed_flag=0;
                    menu_set_value_flag = 0;
                    set_all_diods_color(red_brightness,green_brightness,blue_brightness);
                }
            } break;
            case SET_BLUE: { 
                if(up_short_pressed_flag || up_long_pressed_flag) {
                    up_short_pressed_flag=0; up_long_pressed_flag=0;
                    if(blue_brightness > 9) {
                        blue_brightness-=10;
                        set_all_diods_color(red_brightness,green_brightness,blue_brightness);    
                    }
                } else if(down_short_pressed_flag || down_long_pressed_flag) {
                    down_short_pressed_flag=0; down_long_pressed_flag=0;                    
                    if(blue_brightness < 246) {
                        blue_brightness+=10;
                        set_all_diods_color(red_brightness,green_brightness,blue_brightness);
                    }
                } else if(enter_short_pressed_flag || enter_long_pressed_flag) {
                    enter_short_pressed_flag=0; enter_long_pressed_flag=0;
                    menu_set_value_flag = 0;
                    set_all_diods_color(red_brightness,green_brightness,blue_brightness);
                }
            } break;
            case SET_AUTO_STATE: { 
                if(up_short_pressed_flag || up_long_pressed_flag) {
                    up_short_pressed_flag=0; up_long_pressed_flag=0;
                    if(auto_brightness) {auto_brightness = 0;}
                    else {auto_brightness = 1;}
                } else if(down_short_pressed_flag || down_long_pressed_flag) {
                    down_short_pressed_flag=0; down_long_pressed_flag=0;                    
                    if(auto_brightness) {auto_brightness = 0;}
                    else {auto_brightness = 1;}
                } else if(enter_short_pressed_flag || enter_long_pressed_flag) {
                    enter_short_pressed_flag=0; enter_long_pressed_flag=0;
                    menu_set_value_flag = 0;
                }
            } break;
        }
    } else {
        if(up_short_pressed_flag || up_long_pressed_flag) {
            up_short_pressed_flag=0; up_long_pressed_flag=0;
            if(RGB_menu_state == 0) {
                RGB_menu_state = 4;
            } else {
                RGB_menu_state--;
            }
        } else if(down_short_pressed_flag || down_long_pressed_flag) {
            down_short_pressed_flag = 0; down_long_pressed_flag = 0;
            if(RGB_menu_state == 4) {
                RGB_menu_state = 0;
            } else {
                RGB_menu_state++;
            }
        } else if(enter_short_pressed_flag || enter_long_pressed_flag) {
            enter_short_pressed_flag = 0; enter_long_pressed_flag = 0;
            switch(RGB_menu_state) {
                case RGB_MENU_EXIT: {
                    screen_state = MAIN_MENU;
                } break; //go to main_menu
                default: menu_set_value_flag = 1; break; //set flag for changing value
            }
        }    
    }
}

//!___________________________ Time changing menu ______________________________
void time_menu() {
    GLCD_ClearText();
    draw_time_menu();
    if(menu_set_value_flag) {
        switch (time_menu_state) {
            case SET_HOUR: {GLCD_WriteText(9,10,"=");} break;
            case SET_MINUTE: {GLCD_WriteText(9,12,"=");} break;
            case SET_SECOND: {GLCD_WriteText(9,14,"=");} break;
        }
    } else {
        switch (time_menu_state) {
            case SET_HOUR: {GLCD_WriteText(9,10,">");} break;
            case SET_MINUTE: {GLCD_WriteText(9,12,">");} break;
            case SET_SECOND: {GLCD_WriteText(9,14,">");} break;
            case TIME_MENU_EXIT: {GLCD_WriteText(9,20,">");} break; //EXIT
        }    
    }
    time_menu_buttons_handler();
}

void draw_time_menu() {
    GLCD_WriteText(10,2,"��������� �������");
    GLCD_WriteText(10,10,"���."); int_to_char_array(d_t_dec.hour,temp_arr);
    GLCD_WriteText(18,10,temp_arr);
    GLCD_WriteText(10,12,"���."); int_to_char_array(d_t_dec.minute,temp_arr); 
    GLCD_WriteText(18,12,temp_arr);
    GLCD_WriteText(10,14,"���."); int_to_char_array(d_t_dec.second,temp_arr); 
    GLCD_WriteText(18,14,temp_arr);
    GLCD_WriteText(10,20,"�����");   
}

void time_menu_buttons_handler() {
    if(menu_set_value_flag) {
        switch(time_menu_state) {
            case SET_HOUR: { 
                if(up_short_pressed_flag || up_long_pressed_flag) {
                    up_short_pressed_flag=0; up_long_pressed_flag=0;
                    if(d_t_dec.hour > 0) {d_t_dec.hour --;}
                } else if(down_short_pressed_flag || down_long_pressed_flag) {
                    down_short_pressed_flag=0; down_long_pressed_flag=0;
                    if(d_t_dec.hour < 23) {d_t_dec.hour++;}
                } else if(enter_short_pressed_flag || enter_long_pressed_flag) {
                    enter_short_pressed_flag = 0; enter_long_pressed_flag = 0;
                    menu_set_value_flag = 0;
                }       
            } break;
            case SET_MINUTE: { 
                if(up_short_pressed_flag || up_long_pressed_flag) {
                    up_short_pressed_flag=0; up_long_pressed_flag=0;
                    if(d_t_dec.minute > 0) {d_t_dec.minute--;}
                } else if(down_short_pressed_flag || down_long_pressed_flag) {
                    down_short_pressed_flag=0; down_long_pressed_flag=0;
                    if(d_t_dec.minute < 59) {d_t_dec.minute++;}
                } else if(enter_short_pressed_flag || enter_long_pressed_flag) {
                    enter_short_pressed_flag=0; enter_long_pressed_flag=0;
                    menu_set_value_flag = 0;
                }
            } break;
            case SET_SECOND: { 
                if(up_short_pressed_flag || up_long_pressed_flag) {
                    up_short_pressed_flag=0; up_long_pressed_flag=0;
                    if(d_t_dec.second > 0) {d_t_dec.second--;}
                } else if(down_short_pressed_flag || down_long_pressed_flag) {
                    down_short_pressed_flag=0; down_long_pressed_flag=0;                    
                    if(d_t_dec.second < 59) {d_t_dec.second++;}
                } else if(enter_short_pressed_flag || enter_long_pressed_flag) {
                    enter_short_pressed_flag=0; enter_long_pressed_flag=0;
                    menu_set_value_flag = 0;
                }
            } break;
        }
    } else {
        if(up_short_pressed_flag || up_long_pressed_flag) {
            up_short_pressed_flag=0; up_long_pressed_flag=0;
            if(time_menu_state == 0) {
                time_menu_state = 3;
            } else {
                time_menu_state--;
            }
        } else if(down_short_pressed_flag || down_long_pressed_flag) {
            down_short_pressed_flag = 0; down_long_pressed_flag = 0;
            if(time_menu_state == 3) {
                time_menu_state = 0;
            } else {
                time_menu_state++;
            }
        } else if(enter_short_pressed_flag || enter_long_pressed_flag) {
            enter_short_pressed_flag = 0; enter_long_pressed_flag = 0;
            switch(time_menu_state) {
                case TIME_MENU_EXIT: {
                    dec_to_bcd_struct(&d_t_dec, &d_t);
                    setTime(d_t.hour, d_t.minute, d_t.second);
                    screen_state = MAIN_MENU;
                } break; //go to main_menu
                default: menu_set_value_flag = 1; break; //set flag for changing value
            }
        }    
    }
}

//!___________________________ Date changing menu ______________________________
void date_menu() {
    GLCD_ClearText();
    draw_date_menu();
    if(menu_set_value_flag) {
        switch (date_menu_state) {
            case SET_DAY: {GLCD_WriteText(9,10,"=");} break;
            case SET_MONTH: {GLCD_WriteText(9,12,"=");} break;
            case SET_YEAR: {GLCD_WriteText(9,14,"=");} break;
            case SET_DAYWEEK: {GLCD_WriteText(9,16,"=");} break;
        }
    } else {
        switch (date_menu_state) {
            case SET_DAY: {GLCD_WriteText(9,10,">");} break;
            case SET_MONTH: {GLCD_WriteText(9,12,">");} break;
            case SET_YEAR: {GLCD_WriteText(9,14,">");} break;
            case SET_DAYWEEK: {GLCD_WriteText(9,16,">");} break;
            case DATE_MENU_EXIT: {GLCD_WriteText(9,20,">");} break; //EXIT
        }    
    }
    date_menu_buttons_handler();
}

void draw_date_menu() {
    GLCD_WriteText(10,2,"��������� ����");
    GLCD_WriteText(10,10,"����"); int_to_char_array(d_t_dec.day,temp_arr);
    GLCD_WriteText(18,10,temp_arr);
    GLCD_WriteText(10,12,"�����"); int_to_char_array(d_t_dec.month,temp_arr); 
    GLCD_WriteText(18,12,temp_arr);
    GLCD_WriteText(10,14,"���"); int_to_char_array(d_t_dec.year,temp_arr); 
    GLCD_WriteText(18,14,temp_arr);
    GLCD_WriteText(10,16,"�-�"); daynumber_to_array(d_t_dec.dayweek,temp_arr); 
    GLCD_WriteText(18,16,temp_arr);
    GLCD_WriteText(10,20,"�����");   
}

void date_menu_buttons_handler() {
    if(menu_set_value_flag) {
        switch(date_menu_state) {
            case SET_DAY: { 
                if(up_short_pressed_flag || up_long_pressed_flag) {
                    up_short_pressed_flag=0; up_long_pressed_flag=0;
                    if(d_t_dec.day > 0) {d_t_dec.day--;}
                } else if(down_short_pressed_flag || down_long_pressed_flag) {
                    down_short_pressed_flag=0; down_long_pressed_flag=0;
                    if(d_t_dec.day < 31) {d_t_dec.day++;}
                } else if(enter_short_pressed_flag || enter_long_pressed_flag) {
                    enter_short_pressed_flag = 0; enter_long_pressed_flag = 0;
                    menu_set_value_flag = 0;
                }       
            } break;
            case SET_MONTH: { 
                if(up_short_pressed_flag || up_long_pressed_flag) {
                    up_short_pressed_flag=0; up_long_pressed_flag=0;
                    if(d_t_dec.month > 0) {d_t_dec.month--;}
                } else if(down_short_pressed_flag || down_long_pressed_flag) {
                    down_short_pressed_flag=0; down_long_pressed_flag=0;
                    if(d_t_dec.month < 11) {d_t_dec.month++;}
                } else if(enter_short_pressed_flag || enter_long_pressed_flag) {
                    enter_short_pressed_flag=0; enter_long_pressed_flag=0;
                    menu_set_value_flag = 0;
                }
            } break;
            case SET_YEAR: { 
                if(up_short_pressed_flag || up_long_pressed_flag) {
                    up_short_pressed_flag=0; up_long_pressed_flag=0;
                    if(d_t_dec.year > 0) {d_t_dec.year--;}
                } else if(down_short_pressed_flag || down_long_pressed_flag) {
                    down_short_pressed_flag=0; down_long_pressed_flag=0;                    
                    if(d_t_dec.year < 98) {d_t_dec.year++;}
                } else if(enter_short_pressed_flag || enter_long_pressed_flag) {
                    enter_short_pressed_flag=0; enter_long_pressed_flag=0;
                    menu_set_value_flag = 0;
                }
            } break;
            case SET_DAYWEEK: { 
                if(up_short_pressed_flag || up_long_pressed_flag) {
                    up_short_pressed_flag=0; up_long_pressed_flag=0;
                    if(d_t_dec.dayweek > 0) {d_t_dec.dayweek--;}
                } else if(down_short_pressed_flag || down_long_pressed_flag) {
                    down_short_pressed_flag=0; down_long_pressed_flag=0;                    
                    if(d_t_dec.dayweek < 6) {d_t_dec.dayweek++;}
                } else if(enter_short_pressed_flag || enter_long_pressed_flag) {
                    enter_short_pressed_flag=0; enter_long_pressed_flag=0;
                    menu_set_value_flag = 0;
                }
            } break;
        }
    } else {
        if(up_short_pressed_flag || up_long_pressed_flag) {
            up_short_pressed_flag=0; up_long_pressed_flag=0;
            if(date_menu_state == 0) {
                date_menu_state = 4;
            } else {
                date_menu_state--;
            }
        } else if(down_short_pressed_flag || down_long_pressed_flag) {
            down_short_pressed_flag = 0; down_long_pressed_flag = 0;
            if(date_menu_state == 4) {
                date_menu_state = 0;
            } else {
                date_menu_state++;
            }
        } else if(enter_short_pressed_flag || enter_long_pressed_flag) {
            enter_short_pressed_flag = 0; enter_long_pressed_flag = 0;
            switch(date_menu_state) {
                case DATE_MENU_EXIT: {
                    dec_to_bcd_struct(&d_t_dec, &d_t); setDate(d_t.day, d_t.month, d_t.year, d_t.dayweek);
                    screen_state = MAIN_MENU;
                } break; //go to main_menu
                default: menu_set_value_flag = 1; break; //set flag for changing value
            }
        }    
    }
}

//!___________________________ Alarm settings menu ______________________________
void alarm_menu() {
    GLCD_ClearText();
    draw_alarm_menu();
    if(menu_set_value_flag) {
        switch (alarm_menu_state) {
            case SET_ALARM_HOUR: {GLCD_WriteText(9,10,"=");} break;
            case SET_ALARM_MINUTE: {GLCD_WriteText(9,12,"=");} break;
            case SET_ALARM_DAY_START: {GLCD_WriteText(9,14,"=");} break;
            case SET_ALARM_DAY_STOP: {GLCD_WriteText(9,16,"=");} break;
            case SET_ALARM_TURN: {GLCD_WriteText(9,18,"=");} break;
        }
    } else {
        switch (alarm_menu_state) {
            case SET_ALARM_HOUR: {GLCD_WriteText(9,10,">");} break;
            case SET_ALARM_MINUTE: {GLCD_WriteText(9,12,">");} break;
            case SET_ALARM_DAY_START: {GLCD_WriteText(9,14,">");} break;
            case SET_ALARM_DAY_STOP: {GLCD_WriteText(9,16,">");} break;
            case SET_ALARM_TURN: {GLCD_WriteText(9,18,">");} break;
            case ALARM_MENU_EXIT: {GLCD_WriteText(9,20,">");} break;
        }    
    }
    alarm_menu_buttons_handler();
}

void draw_alarm_menu() {
    GLCD_WriteText(10,2,"��������� ����������");
    GLCD_WriteText(10,10,"���"); int_to_char_array(alarm.hour,temp_arr);
    GLCD_WriteText(18,10,temp_arr);
    GLCD_WriteText(10,12,"���."); int_to_char_array(alarm.minute,temp_arr); 
    GLCD_WriteText(18,12,temp_arr);
    GLCD_WriteText(10,14,"�"); daynumber_to_array(alarm.day_start,temp_arr); 
    GLCD_WriteText(18,14,temp_arr);
    GLCD_WriteText(10,16,"��"); daynumber_to_array(alarm.day_stop,temp_arr); 
    GLCD_WriteText(18,16,temp_arr);
    GLCD_WriteText(10,18,"����."); 
    if(alarm.turn_state) {
        temp_arr[0]='�'; temp_arr[1]='�'; temp_arr[2]='�'; temp_arr[3]='\0'; 
        GLCD_WriteText(18,18,temp_arr);
    } else {
        temp_arr[0]='�'; temp_arr[1]='�'; temp_arr[2]='�'; temp_arr[3]='�'; temp_arr[4]='\0';
        GLCD_WriteText(18,18,temp_arr);
    }
    
    GLCD_WriteText(10,20,"�����");
}

void alarm_menu_buttons_handler() {
    if(menu_set_value_flag) {
        switch(alarm_menu_state) {
            case SET_ALARM_HOUR: { 
                if(up_short_pressed_flag || up_long_pressed_flag) {
                    up_short_pressed_flag=0; up_long_pressed_flag=0;
                    if(alarm.hour > 0) {alarm.hour--;}
                } else if(down_short_pressed_flag || down_long_pressed_flag) {
                    down_short_pressed_flag=0; down_long_pressed_flag=0;
                    if(alarm.hour < 23) {alarm.hour++;}
                } else if(enter_short_pressed_flag || enter_long_pressed_flag) {
                    enter_short_pressed_flag = 0; enter_long_pressed_flag = 0;
                    menu_set_value_flag = 0;
                }       
            } break;
            case SET_ALARM_MINUTE: { 
                if(up_short_pressed_flag || up_long_pressed_flag) {
                    up_short_pressed_flag=0; up_long_pressed_flag=0;
                    if(alarm.minute > 0) {alarm.minute--;}
                } else if(down_short_pressed_flag || down_long_pressed_flag) {
                    down_short_pressed_flag=0; down_long_pressed_flag=0;
                    if(alarm.minute < 59) {alarm.minute++;}
                } else if(enter_short_pressed_flag || enter_long_pressed_flag) {
                    enter_short_pressed_flag=0; enter_long_pressed_flag=0;
                    menu_set_value_flag = 0;
                }
            } break;
            case SET_ALARM_DAY_START: { 
                if(up_short_pressed_flag || up_long_pressed_flag) {
                    up_short_pressed_flag=0; up_long_pressed_flag=0;
                    if(alarm.day_start > 0) {alarm.day_start--;}
                } else if(down_short_pressed_flag || down_long_pressed_flag) {
                    down_short_pressed_flag=0; down_long_pressed_flag=0;                    
                    if(alarm.day_start < 6) {alarm.day_start++; alarm.day_stop = alarm.day_start;}
                } else if(enter_short_pressed_flag || enter_long_pressed_flag) {
                    enter_short_pressed_flag=0; enter_long_pressed_flag=0;
                    menu_set_value_flag = 0;
                }
            } break;
            case SET_ALARM_DAY_STOP: { 
                if(up_short_pressed_flag || up_long_pressed_flag) {
                    up_short_pressed_flag=0; up_long_pressed_flag=0;
                    if(alarm.day_stop > 0 && is_day_bigger(&alarm)) {alarm.day_stop--;}
                } else if(down_short_pressed_flag || down_long_pressed_flag) {
                    down_short_pressed_flag=0; down_long_pressed_flag=0;                    
                    if(alarm.day_stop < 6) {alarm.day_stop++;}
                } else if(enter_short_pressed_flag || enter_long_pressed_flag) {
                    enter_short_pressed_flag=0; enter_long_pressed_flag=0;
                    menu_set_value_flag = 0;
                }
            } break;
            case SET_ALARM_TURN: { 
                if(up_short_pressed_flag || up_long_pressed_flag) {
                    up_short_pressed_flag=0; up_long_pressed_flag=0;
                    if(alarm.turn_state) {alarm.turn_state = 0;}
                    else {alarm.turn_state = 1;}
                } else if(down_short_pressed_flag || down_long_pressed_flag) {
                    down_short_pressed_flag=0; down_long_pressed_flag=0;                    
                    if(alarm.turn_state) {alarm.turn_state = 0;}
                    else {alarm.turn_state = 1;}
                } else if(enter_short_pressed_flag || enter_long_pressed_flag) {
                    enter_short_pressed_flag=0; enter_long_pressed_flag=0;
                    menu_set_value_flag = 0;
                }
            } break;
        }
    } else {
        if(up_short_pressed_flag || up_long_pressed_flag) {
            up_short_pressed_flag=0; up_long_pressed_flag=0;
            if(alarm_menu_state == 0) {
                alarm_menu_state = 5;
            } else {
                alarm_menu_state--;
            }
        } else if(down_short_pressed_flag || down_long_pressed_flag) {
            down_short_pressed_flag = 0; down_long_pressed_flag = 0;
            if(alarm_menu_state == 5) {
                alarm_menu_state = 0;
            } else {
                alarm_menu_state++;
            }
        } else if(enter_short_pressed_flag || enter_long_pressed_flag) {
            enter_short_pressed_flag = 0; enter_long_pressed_flag = 0;
            switch(alarm_menu_state) {
                case ALARM_MENU_EXIT: {
                    screen_state = MAIN_MENU;
                } break; 
                default: menu_set_value_flag = 1; break; //set flag for changing value
            }
        }    
    }
}

//!___________________________ Bang settings menu ______________________________
void bang_menu() {
    GLCD_ClearText();
    draw_bang_menu();
    if(menu_set_value_flag) {
        switch (bang_menu_state) {
            case SET_BANG_HOUR_START: {GLCD_WriteText(9,10,"=");} break;
            case SET_BANG_HOUR_STOP: {GLCD_WriteText(9,12,"=");} break;
            case SET_BANG_TURN: {GLCD_WriteText(9,14,"=");} break;
        }
    } else {
        switch (bang_menu_state) {
            case SET_BANG_HOUR_START: {GLCD_WriteText(9,10,">");} break;
            case SET_BANG_HOUR_STOP: {GLCD_WriteText(9,12,">");} break;
            case SET_BANG_TURN: {GLCD_WriteText(9,14,">");} break;
            case BANG_MENU_EXIT: {GLCD_WriteText(9,20,">");} break;
        }    
    }
    bang_menu_buttons_handler();
}
void draw_bang_menu() {
    GLCD_WriteText(10,2,"��������� �����");
    GLCD_WriteText(10,10,"��� �"); int_to_char_array(bang.hour_start,temp_arr);
    GLCD_WriteText(18,10,temp_arr);
    GLCD_WriteText(10,12,"��� ��"); int_to_char_array(bang.hour_stop,temp_arr); 
    GLCD_WriteText(18,12,temp_arr);
    GLCD_WriteText(10,14,"����."); 
    if(bang.turn_state) {
        temp_arr[0]='�'; temp_arr[1]='�'; temp_arr[2]='�'; temp_arr[3]='\0'; 
        GLCD_WriteText(18,14,temp_arr);
    } else {
        temp_arr[0]='�'; temp_arr[1]='�'; temp_arr[2]='�'; temp_arr[3]='�'; temp_arr[4]='\0';
        GLCD_WriteText(18,14,temp_arr);
    }    
    GLCD_WriteText(10,20,"�����");
}
void bang_menu_buttons_handler() {
    if(menu_set_value_flag) {
        switch(bang_menu_state) {
            case SET_BANG_HOUR_START: { 
                if(up_short_pressed_flag || up_long_pressed_flag) {
                    up_short_pressed_flag=0; up_long_pressed_flag=0;
                    if(bang.hour_start > 0) {bang.hour_start--;}
                } else if(down_short_pressed_flag || down_long_pressed_flag) {
                    down_short_pressed_flag=0; down_long_pressed_flag=0;                    
                    if(bang.hour_start < 23) {bang.hour_start++; bang.hour_stop = bang.hour_start;}
                } else if(enter_short_pressed_flag || enter_long_pressed_flag) {
                    enter_short_pressed_flag=0; enter_long_pressed_flag=0;
                    menu_set_value_flag = 0;
                }
            } break;
            case SET_BANG_HOUR_STOP: { 
                if(up_short_pressed_flag || up_long_pressed_flag) {
                    up_short_pressed_flag=0; up_long_pressed_flag=0;
                    if(bang.hour_stop > 0 && is_bang_hour_bigger(&bang)) {bang.hour_stop--;}
                } else if(down_short_pressed_flag || down_long_pressed_flag) {
                    down_short_pressed_flag=0; down_long_pressed_flag=0;                    
                    if(bang.hour_stop < 23) {bang.hour_stop++;}
                } else if(enter_short_pressed_flag || enter_long_pressed_flag) {
                    enter_short_pressed_flag=0; enter_long_pressed_flag=0;
                    menu_set_value_flag = 0;
                }
            } break;
            case SET_BANG_TURN: { 
                if(up_short_pressed_flag || up_long_pressed_flag) {
                    up_short_pressed_flag=0; up_long_pressed_flag=0;
                    if(bang.turn_state) {bang.turn_state = 0;}
                    else {bang.turn_state = 1;}
                } else if(down_short_pressed_flag || down_long_pressed_flag) {
                    down_short_pressed_flag=0; down_long_pressed_flag=0;                    
                    if(bang.turn_state) {bang.turn_state = 0;}
                    else {bang.turn_state = 1;}
                } else if(enter_short_pressed_flag || enter_long_pressed_flag) {
                    enter_short_pressed_flag=0; enter_long_pressed_flag=0;
                    menu_set_value_flag = 0;
                }
            } break;
        }
    } else {
        if(up_short_pressed_flag || up_long_pressed_flag) {
            up_short_pressed_flag=0; up_long_pressed_flag=0;
            if(bang_menu_state == 0) {
                bang_menu_state = 3;
            } else {
                bang_menu_state--;
            }
        } else if(down_short_pressed_flag || down_long_pressed_flag) {
            down_short_pressed_flag = 0; down_long_pressed_flag = 0;
            if(bang_menu_state == 3) {
                bang_menu_state = 0;
            } else {
                bang_menu_state++;
            }
        } else if(enter_short_pressed_flag || enter_long_pressed_flag) {
            enter_short_pressed_flag = 0; enter_long_pressed_flag = 0;
            switch(bang_menu_state) {
                case BANG_MENU_EXIT: {
                    screen_state = MAIN_MENU;
                } break; 
                default: menu_set_value_flag = 1; break; //set flag for changing value
            }
        }    
    }
}