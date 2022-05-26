#include "rcc.h"

Rcc::Rcc(int freq) {
    rcc_init(freq);
}


void Rcc::rcc_init(int i) {
    switch(i) {        
        case 48: {
            RCC->CFGR = 0; //set HIS oscillator
            RCC->CR &=~ RCC_CR_PLLON; //turn off PLL to set new values            
            RCC->PLLCFGR = 0; // reset clocks 
            RCC->CR &=~ RCC_CR_HSION;
            RCC->CR |= RCC_CR_HSEON;
            while(!(RCC->CR & RCC_CR_HSERDY));
            /////*!< SYSCLK not divided */  
            /*!< HCLK divided by 2 (APB1)*/ 
            /*!< HCLK divided by 1 (APB2)*/ //(APB1-24 APB2-48MHz)
            RCC -> CFGR |= (RCC_CFGR_HPRE_DIV1|RCC_CFGR_PPRE1_DIV2|RCC_CFGR_PPRE2_DIV1);
            /*<! CLK /M *N /P (/Q for usb) 
                M=4, N=192, P=8, Q=8  = >*/
            RCC->PLLCFGR |= ((8<<24)|(RCC_PLLCFGR_PLLSRC_HSE)|(3<<16)|(192<<6)|(4<<0));
            RCC->CR |= RCC_CR_PLLON;
            while(!(RCC->CR & RCC_CR_PLLRDY));
            /*!< Flash: instruction cache, data cache, wait state, flash prefetch >*/
            FLASH->ACR |= FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_LATENCY_1WS | FLASH_ACR_PRFTEN;            
            RCC->CFGR |= RCC_CFGR_SW_PLL;
            while(!(RCC->CFGR & RCC_CFGR_SWS_PLL));        
        } break;
        case 168: {
            RCC->CFGR = 0; //set HIS oscillator
            RCC->CR &=~ RCC_CR_PLLON; //turn off PLL to set new values            
            RCC->PLLCFGR = 0; // reset clocks 
            RCC->CR &=~ RCC_CR_HSION;
            RCC->CR |= RCC_CR_HSEON;
            while(!(RCC->CR & RCC_CR_HSERDY));
            /////*!< SYSCLK not divided */  
            /*!< HCLK divided by 4 (APB1)*/ 
            /*!< HCLK divided by 2 (APB2)*/ //(42 84MHz)
            RCC -> CFGR |= (RCC_CFGR_HPRE_DIV1|RCC_CFGR_PPRE1_DIV4|RCC_CFGR_PPRE2_DIV2);
            /*<! CLK /M *N /P (/Q for usb) 
                M=4, N=168, P=2, Q=7  = 8/4*168/2 (/7) = >*/
            RCC->PLLCFGR |= ((7<<24)|(RCC_PLLCFGR_PLLSRC_HSE)|(0<<16)|(168<<6)|(4<<0));
            RCC->CR |= RCC_CR_PLLON;
            while(!(RCC->CR & RCC_CR_PLLRDY));
            /*!< Flash: instruction cache, data cache, wait state, flash prefetch >*/
            FLASH->ACR |= FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_LATENCY_5WS | FLASH_ACR_PRFTEN;
            
            RCC->CFGR |= RCC_CFGR_SW_PLL;
            while(!(RCC->CFGR & RCC_CFGR_SWS_PLL));            
        } break;
        default:
            RCC->CFGR = 0; //set HIS oscillator
            RCC->PLLCFGR = 0; // reset clocks 
            RCC->CR = RCC_CR_HSION; //internal RC clock   16 MHz
        break;
    }
}
