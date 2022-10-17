#include "main.h"

#define GPIOD_ODR 0x40000000UL + 0x00020000UL + 0x0C00UL + 0x14UL
#define GPIOD_O 0x40020C14

void delay(uint32_t ticks);

void led_init();
inline void led_on() { GPIOD->BSRR |= GPIO_BSRR_BR7; }
inline void led_off() { GPIOD->BSRR |= GPIO_BSRR_BS7; }
inline void led_toggle() {    
    __ASM("GPIOD_ODR:       .word 0x40020C14");
    __ASM("GPIOD_ODR_OD7:   .long 0x00000080");
    __ASM("    ldr r4, GPIOD_ODR");
    __ASM("    ldr r0, GPIOD_ODR_OD7");
    
    //__ASM("    movw r4, :lower16:GPIOD_ODR");
    //__ASM("    movt r4, :upper16:GPIOD_ODR");
    //__ASM("    movw r4, 0x0C14");
    //__ASM("    movt r4, 0x4002");
    //__ASM("    mov r0, 0x80");

    __ASM("    ldr r3, [r4]");
    __ASM("    eor r3, r3, r0");
    __ASM("    str r3, [r4]");

    //GPIOD->ODR ^= GPIO_ODR_OD7;
}

int main(void) {
	__disable_irq();
    //! Enable FPU and sleep
    //SCB->CPACR |= ((3UL << 10*2)|(3UL << 11*2));  /* set CP10 and CP11 Full Access */
   //! RCC
    Rcc rcc(24);    
    led_init();

	while(1) { 
        led_toggle();
        delay(8000000);
	}
    return 0;
}

void led_init() {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
    GPIOD->MODER |= GPIO_MODER_MODE7_0;
}

void delay(uint32_t ticks) {
    while(ticks--);
}
