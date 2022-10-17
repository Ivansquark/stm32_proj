#include "rcc.h"

Rcc::Rcc(uint8_t quarz) {
    init(quarz);
}

void Rcc::init(uint8_t quarz) {
    RCC->CFGR&=~RCC_CFGR_SW;//SW=0 
    RCC->CR&=~RCC_CR_PLLON;
    RCC->CFGR=0;
    RCC->PLLCFGR=0;
    RCC->CR|=RCC_CR_HSION;
    switch(quarz) {
        case 8:{ 
    	    RCC->CR&=~RCC_CR_HSION;
    	    RCC->CR|=RCC_CR_HSEON; 
    	    while(!(RCC->CR&RCC_CR_HSERDY)){}
    	    RCC->CFGR |= (RCC_CFGR_HPRE_DIV1|RCC_CFGR_PPRE1_DIV4|RCC_CFGR_PPRE2_DIV2);
            //RCC->PLLCFGR|=PLL_M|(PLL_N<<6)|(((PLL_P>>1)-1)<<16)|RCC_PLLCFGR_PLLSRC_HSE|(PLL_Q<<24);   	    	
    	    RCC->PLLCFGR|=(4|(168<<6)|(((2>>1)-1)<<16)|RCC_PLLCFGR_PLLSRC_HSE|(7<<24));
    	    RCC->CR|=RCC_CR_PLLON;
    	    while(!(RCC->CR&RCC_CR_PLLRDY)){}
    	    FLASH->ACR|=FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_LATENCY_5WS | FLASH_ACR_PRFTEN;      	    
    	    RCC->CFGR|=RCC_CFGR_SW_PLL;
    	    while(!(RCC->CFGR&RCC_CFGR_SWS_PLL));
        } break;
        case 24:{
            RCC->CR&=~RCC_CR_HSION;
    	    RCC->CR|=RCC_CR_HSEON; 
    	    while(!(RCC->CR&RCC_CR_HSERDY)){}
    	    RCC->CFGR |= (RCC_CFGR_HPRE_DIV1|RCC_CFGR_PPRE1_DIV4|RCC_CFGR_PPRE2_DIV2); 
            //RCC->PLLCFGR|=PLL_M|(PLL_N<<6)|(((PLL_P>>1)-1)<<16)|RCC_PLLCFGR_PLLSRC_HSE|(PLL_Q<<24);  	    	
    	    RCC->PLLCFGR|=(16|(224<<6)|(((2>>1)-1)<<16)|RCC_PLLCFGR_PLLSRC_HSE|(7<<24));
    	    RCC->CR|=RCC_CR_PLLON;
    	    while(!(RCC->CR&RCC_CR_PLLRDY)){}
    	    FLASH->ACR|=FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_LATENCY_5WS | FLASH_ACR_PRFTEN;      	    
    	    RCC->CFGR|=RCC_CFGR_SW_PLL;
    	    while(!(RCC->CFGR&RCC_CFGR_SWS_PLL));
        } break;
        default:{

        } break;
    }
}