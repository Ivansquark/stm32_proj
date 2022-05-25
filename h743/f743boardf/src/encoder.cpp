#include "encoder.h"

Encoder* Encoder::pThis = nullptr;

Encoder::Encoder() {
    encoder_init();
    shag_init();
    pThis = this;
}

void Encoder::enc1_counter_change() {
    enc1_state_new = (((GPIOA->IDR & GPIO_IDR_ID3) >> 3) | ((GPIOB->IDR & GPIO_IDR_ID7) >> 6));
    if(enc1_counter == 100) {
        enc1_counter = 99;
    }
    if(enc1_state_old == 3) {
        if(enc1_state_new == 1) {
            enc1_counter--;
        } else if (enc1_state_new == 2) {
            enc1_counter++;
        }
    } else if (enc1_state_old == 2) {
        if(enc1_state_new == 3) {
            enc1_counter--;
        } else if (enc1_state_new == 0) {
            enc1_counter++;
        }
    } else if (enc1_state_old == 1) {
        if(enc1_state_new == 0) {
            enc1_counter--;
        } else if (enc1_state_new == 3) {
            enc1_counter++;
        }
    } else if (enc1_state_old == 0) {
        if(enc1_state_new == 1) {
            enc1_counter++;
        } else if (enc2_state_new == 2) {
            enc1_counter--;
        }
    }
    if(enc1_counter == 101) {
        enc1_counter = 99;
    }
    if(enc1_counter == 65535) {
        enc1_counter = 0;
    }
    enc1_state_old = enc1_state_new;
}

void Encoder::enc2_counter_change() {
    enc2_state_new = (((GPIOA->IDR & GPIO_IDR_ID8) >> 8) | ((GPIOB->IDR & GPIO_IDR_ID5) >> 4));
    if(enc2_counter == 100) {
        enc2_counter = 99;
    }
    if(enc2_state_old == 3) {
        if(enc2_state_new == 1) {
            enc2_counter--;
        } else if (enc2_state_new == 2) {
            enc2_counter++;
        }
    } else if (enc2_state_old == 2) {
        if(enc2_state_new == 3) {
            enc2_counter--;
        } else if (enc2_state_new == 0) {
            enc2_counter++;
        }
    } else if (enc2_state_old == 1) {
        if(enc2_state_new == 0) {
            enc2_counter--;
        } else if (enc2_state_new == 3) {
            enc2_counter++;
        }
    } else if (enc2_state_old == 0) {
        if(enc2_state_new == 1) {
            enc2_counter++;
        } else if (enc2_state_new == 2) {
            enc2_counter--;
        }
    }
    if(enc2_counter == 101) {
        enc2_counter = 99;
    }
    if(enc2_counter == 65535) {
        enc2_counter = 0;
    }
    enc2_state_old = enc2_state_new;
}

void Encoder::SM_forward(uint16_t steps) {
    for(int i =0; i<steps; i++) {
        SM_step_forward();
    }
}
void Encoder::SM_backward(uint16_t steps) {
    for(int i =0; i<steps; i++) {
        SM_step_backward();
    }
}

void Encoder::SM_step_forward() {
    SM_set_1(); GP_Timers::pThis[0]->delay_ms(10);
    SM_set_2(); GP_Timers::pThis[0]->delay_ms(10);
    SM_set_3(); GP_Timers::pThis[0]->delay_ms(10);
    SM_set_4(); GP_Timers::pThis[0]->delay_ms(10);
}
void Encoder::SM_step_backward() {
    SM_set_4(); GP_Timers::pThis[0]->delay_ms(10);
    SM_set_3(); GP_Timers::pThis[0]->delay_ms(10);
    SM_set_2(); GP_Timers::pThis[0]->delay_ms(10);
    SM_set_1(); GP_Timers::pThis[0]->delay_ms(10);
}
void Encoder::SM_set_1() {
    GPIOB->BSRR |= GPIO_BSRR_BS0; GPIOB->BSRR |= GPIO_BSRR_BR1;
    GPIOB->BSRR |= GPIO_BSRR_BR2; GPIOB->BSRR |= GPIO_BSRR_BR6;
}
void Encoder::SM_set_2() {
    GPIOB->BSRR |= GPIO_BSRR_BR0; GPIOB->BSRR |= GPIO_BSRR_BS1;
    GPIOB->BSRR |= GPIO_BSRR_BR2; GPIOB->BSRR |= GPIO_BSRR_BR6;
}
void Encoder::SM_set_3() {
    GPIOB->BSRR |= GPIO_BSRR_BR0; GPIOB->BSRR |= GPIO_BSRR_BR1;
    GPIOB->BSRR |= GPIO_BSRR_BS2; GPIOB->BSRR |= GPIO_BSRR_BR6;
}
void Encoder::SM_set_4() {
    GPIOB->BSRR |= GPIO_BSRR_BR0; GPIOB->BSRR |= GPIO_BSRR_BR1;
    GPIOB->BSRR |= GPIO_BSRR_BR2; GPIOB->BSRR |= GPIO_BSRR_BS6;
}
void Encoder::shag_init() {
    GPIOB->MODER &=~ GPIO_MODER_MODE0_1;      // input mode
    GPIOB->MODER |= GPIO_MODER_MODE0_0;      // input mode
    GPIOB->MODER &=~ GPIO_MODER_MODE1_1;      // input mode
    GPIOB->MODER |= GPIO_MODER_MODE1_0;      // input mode
    GPIOB->MODER &=~ GPIO_MODER_MODE2_1;      // input mode
    GPIOB->MODER |= GPIO_MODER_MODE2_0;      // input mode
    GPIOB->MODER &=~ GPIO_MODER_MODE6_1;      // input mode
    GPIOB->MODER |= GPIO_MODER_MODE6_0;      // input mode
}

void Encoder::encoder_init() {
    //! GpioA PA3-Enc1_CW PB7-Enc1_CCW(should be soldered to right pin) PC3_Enc1_but ||||  PA-8-Enc2_Cw  - PB5-Enc2_CCW   PC9_Enc1_but
    //! but Enc1
    RCC->AHB4ENR |= RCC_AHB4ENR_GPIOCEN; 
    GPIOC->MODER &=~ GPIO_MODER_MODE3;      // input mode
    GPIOC->PUPDR &=~ GPIO_PUPDR_PUPD3;      // no-pullup no-pulldown (connected to Vdd)
    //! Enc2
    RCC->AHB4ENR |= RCC_AHB4ENR_GPIOAEN;
    GPIOA->MODER &=~ GPIO_MODER_MODE8;      // input mode
    GPIOA->PUPDR &=~ GPIO_PUPDR_PUPD8;      // no-pullup no-pulldown (connected to Vdd)
    GPIOA->MODER &=~ GPIO_MODER_MODE3;      // input mode
    GPIOA->PUPDR &=~ GPIO_PUPDR_PUPD3;      // no-pullup no-pulldown (connected to Vdd)
    RCC->AHB4ENR |= RCC_AHB4ENR_GPIOBEN;
    GPIOB->MODER &=~ GPIO_MODER_MODE5;      // input mode
    GPIOB->PUPDR &=~ GPIO_PUPDR_PUPD5;      // no-pullup no-pulldown (connected to Vdd)
    GPIOB->MODER &=~ GPIO_MODER_MODE7;      // input mode
    GPIOB->PUPDR &=~ GPIO_PUPDR_PUPD7;      // no-pullup no-pulldown (connected to Vdd)
    GPIOC->MODER &=~ GPIO_MODER_MODE9;      // input mode
    GPIOC->PUPDR &=~ GPIO_PUPDR_PUPD9;      // no-pullup no-pulldown (connected to Vdd)
    
    //! but enc1
    RCC->APB4ENR |= RCC_APB4ENR_SYSCFGEN;
    SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI3_PA; 
    EXTI->FTSR1 |= EXTI_FTSR1_TR3;  // on fall edge
    EXTI->IMR1 |= EXTI_IMR1_IM3; // unmask input interrupt
    EXTI->PR1 |= EXTI_PR1_PR3;
    NVIC_EnableIRQ(EXTI3_IRQn);
    // PB7
    SYSCFG->EXTICR[1] |= SYSCFG_EXTICR2_EXTI7_PB; 
    EXTI->FTSR1 |= EXTI_FTSR1_TR7;  // on fall edge
    EXTI->IMR1 |= EXTI_IMR1_IM7; // unmask input interrupt
    EXTI->PR1 |= EXTI_PR1_PR7;
    //CCW enc2
    // PB5
    SYSCFG->EXTICR[1] |= SYSCFG_EXTICR2_EXTI5_PB; 
    EXTI->FTSR1 |= EXTI_FTSR1_TR5;  // on fall edge
    EXTI->IMR1 |= EXTI_IMR1_IM5; // unmask input interrupt
    EXTI->PR1 |= EXTI_PR1_PR5;
    // PA8
    //SYSCFG->EXTICR[2] |= SYSCFG_EXTICR3_EXTI8_PA; 
    //EXTI->FTSR1 |= EXTI_FTSR1_TR8;  // on fall edge
    //EXTI->IMR1 |= EXTI_IMR1_IM8; // unmask input interrupt
    //EXTI->PR1 |= EXTI_PR1_PR8;
    // PC9
    SYSCFG->EXTICR[2] |= SYSCFG_EXTICR3_EXTI9_PC; 
    EXTI->FTSR1 |= EXTI_FTSR1_TR9;  // on fall edge
    EXTI->IMR1 |= EXTI_IMR1_IM9; // unmask input interrupt
    EXTI->PR1 |= EXTI_PR1_PR9;
    NVIC_EnableIRQ(EXTI9_5_IRQn);
}

extern "C" void EXTI3_IRQHandler(void) {
    EXTI->PR1|=EXTI_PR1_PR3;

    Encoder::pThis->But_PA3 = true;
}

extern "C" void EXTI9_5_IRQHandler(void) {
    if(EXTI->PR1 & EXTI_PR1_PR5) {
        Encoder::pThis->But_PB5 = true;
        EXTI->PR1 = EXTI_PR1_PR5; // clear pending flag
    }
    if(EXTI->PR1 & EXTI_PR1_PR7) {
        Encoder::pThis->But_PB7 = true;
        EXTI->PR1 = EXTI_PR1_PR7; // clear pending flag
    }
    if(EXTI->PR1 & EXTI_PR1_PR8) {
        //Encoder::pThis->But_PA8 = true;
        EXTI->PR1 = EXTI_PR1_PR8; // clear pending flag
    }
    if(EXTI->PR1 & EXTI_PR1_PR9) {
        Encoder::pThis->But_PC9 = true;
        EXTI->PR1 = EXTI_PR1_PR9; // clear pending flag
    }
}