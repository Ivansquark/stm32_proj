#include "main.h"
#include "rs485.h"
#include "leds.h"
#include "motor.h"
//extern "C" void _exit(void) { while(1);}
// FREQUENCY = 8 MHz (on all buses)
int main(void) {

    //RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    //GPIOA->MODER &=~ (GPIO_MODER_MODER5_1);
    //GPIOA->MODER |= (GPIO_MODER_MODER5_0);
    //GPIOA->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR5); //push pull high speed
    //TODO: inits all
    Motor::init();
    Modbus::init();
    Timer::timer3_init();
    Timer::timer16_init();
    RS485::init();
    Leds::init();
    __enable_irq();
    while(1) {
        Motor::stateMachineHandler();        
    }
    return 0;
}
