#ifndef BANG_H
#define BANG_H

#include "stm32g070xx.h"
#include "ds3231.h"
#include <stdint.h>

#define BANG_ON ((GPIOA->BSRR) |= (GPIO_BSRR_BS15));
#define BANG_OFF ((GPIOA->BSRR) |= (GPIO_BSRR_BR15));

typedef struct BANG {
    uint8_t hour_start;
    uint8_t hour_stop;
    uint8_t turn_state;
} Bang;
extern Bang bang;

extern dateTime d_t_dec;
extern uint32_t bang_counter;
void bang_init();
void bang_bang();
void bang_delay_ms(volatile uint32_t ms);
uint8_t is_bang_hour_bigger(Bang* bang);
uint8_t check_bang_start();

#endif //BANG_H