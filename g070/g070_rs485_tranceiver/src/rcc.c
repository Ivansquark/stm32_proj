#include "rcc.h"

void rcc_init(uint8_t frequency) {
    RCC->CR &=~ RCC_CR_PLLON;
    RCC->CR |= RCC_CR_HSION; 
    while(!(RCC->CR & RCC_CR_HSIRDY));
    RCC->CFGR &=~ RCC_CFGR_PPRE; // 0:0:0 => APB prescaler = 1
    RCC->CFGR &=~ RCC_CFGR_HPRE; // 0:0:0:0 => AHB prescaler = 1
    RCC->CFGR &=~ RCC_CFGR_SW;   // clear => HSI source
    
    switch(frequency) {
        case 16: {
            return;
        }; break;
        case 64: {
            //! all clocks = 64 MHz
            RCC->PLLCFGR = 0;
            RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_HSI; // source incternal RC
            RCC->PLLCFGR &=~ RCC_PLLCFGR_PLLM; //0:0:0 => PLLM=1
            RCC->PLLCFGR |= (8<<8); // PLLN = 8
            RCC->PLLCFGR |= (1<<29); //1 => PLLR = 2 
            RCC->PLLCFGR |= RCC_PLLCFGR_PLLREN; // enable pll output
            RCC->PLLCFGR &=~ RCC_PLLCFGR_PLLP;
            RCC->PLLCFGR |= RCC_PLLCFGR_PLLP_0;
            RCC->PLLCFGR |= RCC_PLLCFGR_PLLPEN;
            FLASH->ACR |= FLASH_ACR_LATENCY_1;
        }; break;
        default: return;
    }    
    RCC->CR |= RCC_CR_PLLON;
    while(!(RCC->CR & RCC_CR_PLLRDY)); //enable PLL
    RCC->CFGR |= RCC_CFGR_SW_1;  // 0:1:0 => PLL source
    while(!(RCC->CFGR & RCC_CFGR_SWS_1));
}