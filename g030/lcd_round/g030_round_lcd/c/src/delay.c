#include "delay.h"

void delay(volatile uint32_t x) {
    while(x--);
}