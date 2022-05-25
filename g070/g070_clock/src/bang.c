#include "bang.h"

Bang bang = {0};
uint32_t bang_counter = 0;
void bang_init() {
    RCC->IOPENR |= RCC_IOPENR_GPIOAEN;
    GPIOA->MODER |= GPIO_MODER_MODE15_0;
    GPIOA->MODER &=~ GPIO_MODER_MODE15_1; // output mode
    GPIOA->ODR |= GPIO_ODR_OD15; //high level;
    bang_counter = 0;
}

void bang_delay_ms(volatile uint32_t ms) {
    for(int i = 0; i<ms*64000; i++) {
        __ASM("nop");
    }
}

void bang_bang() {
    BANG_ON
    bang_delay_ms(100);
    BANG_OFF
}

uint8_t is_bang_hour_bigger(Bang* bang) {
    if(bang->hour_stop > bang->hour_start) {
        return 1;
    } else return 0;
}

uint8_t check_bang_start() {
    if(bang.turn_state && bang.hour_start <= d_t_dec.hour && bang.hour_stop >= d_t_dec.hour) {
        if(d_t_dec.second == 0) {
            return 1;
        } else return 0;
    } else return 0;
}