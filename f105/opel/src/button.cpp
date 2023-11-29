#include "button.h"

Button *Button::pThis = nullptr;

Button::Button() {
  pThis = this;
  init();
}

void Button::init() {
  // GPIO inits C8_UP C7_DOWN - buttons //pull_up
  RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
  GPIOC->CRH &= (GPIO_CRH_MODE8);
  GPIOC->CRH |= GPIO_CRH_CNF8_1;
  GPIOC->ODR |= GPIO_ODR_ODR8; 
  GPIOC->CRL &= (GPIO_CRL_MODE7);
  GPIOC->CRL |= GPIO_CRL_CNF7_1;
  GPIOC->ODR |= GPIO_ODR_ODR7; 

  RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
  AFIO->EXTICR[1] |= (AFIO_EXTICR2_EXTI7_PC); //7 to EXTI PC
  AFIO->EXTICR[2] |= (AFIO_EXTICR3_EXTI8_PC); //8 to EXTI PC
  
  EXTI->IMR |= (EXTI_IMR_IM7 | EXTI_IMR_IM8);
  EXTI->FTSR |= (EXTI_FTSR_FT7 | EXTI_FTSR_FT8); // falling trigger
  NVIC_EnableIRQ(EXTI9_5_IRQn);
  //!__________ TIM2 init short press button timer______________________
  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
  TIM2->PSC = 8000; // TIM2 clk = 32 MHz  TIM3_clk - 1 kHz
  // TIM2->ARR = 500; // period = 500ms
  //TIM2->ARR = 300;            // period = 500ms
  TIM2->ARR = 150;            // period = 500ms
  TIM2->DIER |= TIM_DIER_UIE; // update interrupt enabled
  //TIM2->CR1 |= TIM_CR1_CEN;   // counter enabled
  NVIC_EnableIRQ(TIM2_IRQn);
  //!__________ TIM3 init long press button timer______________________
  RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
  TIM3->PSC = 8000;          // TIM3 clk = 32 MHz  TIM3_clk - 1 kHz
  //TIM3->ARR = 600;            // period = 500ms
  TIM3->ARR = 300;            // period = 500ms
  TIM3->DIER |= TIM_DIER_UIE; // update interrupt enabled
  //TIM3->CR1 |= TIM_CR1_CEN;   // counter enabled
  NVIC_EnableIRQ(TIM3_IRQn);

  //PB10 floating input if(0) => buttons on wheel
  RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
  GPIOB->CRH &= ~(GPIO_CRH_MODE10);
  GPIOB->CRH &= ~GPIO_CRH_CNF10;
  GPIOB->CRH |= GPIO_CRH_CNF10_0;
}


extern "C" void EXTI9_5_IRQHandler(void) {
  TIM2->CR1 &= ~TIM_CR1_CEN; // stop timer
  TIM2->CNT = 0;
  NVIC_DisableIRQ(EXTI9_5_IRQn); //! disable interrupt while timer6 updates
  TIM2->CR1 |= TIM_CR1_CEN;       // start timer
}

extern "C" void TIM2_IRQHandler(void) {
  // NVIC_ClearPendingIRQ(TIM2_IRQn);
  // NVIC_ClearPendingIRQ(EXTI0_1_IRQn);
  // NVIC_ClearPendingIRQ(EXTI4_15_IRQn);
  // TIM2->SR &= ~TIM_SR_UIF;   // clear interrupt flag
  // TIM2->CR1 &= ~TIM_CR1_CEN; // stop timer

  //// pressed both buttons

  // if ((EXTI->PR & EXTI_PR_PIF1) and (EXTI->PR & EXTI_PR_PIF7)) {
  //  // Was pressed both buttons
  //  EXTI->PR |= EXTI_PR_PIF1; // clear flag
  //  EXTI->PR |= EXTI_PR_PIF7; // clear flag
  //  NVIC_ClearPendingIRQ(
  //      EXTI4_15_IRQn); // without this sets another interrupt!!!!!!!!!!
  //  NVIC_ClearPendingIRQ(
  //      EXTI0_1_IRQn); // without this sets another interrupt!!!!!!!!!!
  //  Button::pThis->butState = ButtonState::UP_DOWN;
  //} else if (EXTI->PR & EXTI_PR_PIF1) {
  //  EXTI->PR |= EXTI_PR_PIF1; // clear flag
  //  NVIC_ClearPendingIRQ(EXTI0_1_IRQn);
  //  Button::pThis->butState = ButtonState::UP;
  //} else if (EXTI->PR & EXTI_PR_PIF7) {
  //  EXTI->PR |= EXTI_PR_PIF7; // clear flag
  //  NVIC_ClearPendingIRQ(EXTI4_15_IRQn);
  //  Button::pThis->butState = ButtonState::DOWN;
  //}
  // if (!(GPIOB->IDR & GPIO_IDR_1)) {
  //  EXTI->PR |= EXTI_PR_PIF1; // clear flag
  //  NVIC_ClearPendingIRQ(EXTI0_1_IRQn);
  //  Button::pThis->butState = ButtonState::UP_LONG;
  //  TIM3->CR1 |= TIM_CR1_CEN; // start long pressed button timer
  //} else if (!(GPIOA->IDR & GPIO_IDR_7)) {
  //  EXTI->PR |= EXTI_PR_PIF7; // clear flag
  //  NVIC_ClearPendingIRQ(EXTI4_15_IRQn);
  //  Button::pThis->butState = ButtonState::DOWN_LONG;
  //  TIM3->CR1 |= TIM_CR1_CEN; // start long pressed button timer
  //} else {
  //  NVIC_EnableIRQ(
  //      EXTI0_1_IRQn); // short pressed + pause for rattle is finished
  //  NVIC_EnableIRQ(
  //      EXTI4_15_IRQn); // short pressed + pause for rattle is finished
  //}
  // TIM2->CNT = 0;

  // little delay timer
  // check if buttons still pressed? if not not count it
  TIM2->SR &= ~TIM_SR_UIF;   // clear interrupt flag
  TIM2->CR1 &= ~TIM_CR1_CEN; // stop timer
  TIM2->CNT = 0;
  if ((EXTI->PR & EXTI_PR_PIF7) and (EXTI->PR & EXTI_PR_PIF8) and
      (!(GPIOC->IDR & GPIO_IDR_IDR7)) and (!(GPIOC->IDR & GPIO_IDR_IDR8))) {
    // Was pressed both buttons
    EXTI->PR |= EXTI_PR_PIF7; // clear flag
    EXTI->PR |= EXTI_PR_PIF8; // clear flag
    // without this sets another interrupt!!!!!!!!!!
    NVIC_ClearPendingIRQ(EXTI9_5_IRQn);
    Button::pThis->butState = ButtonState::UP_DOWN;
    TIM3->CR1 |= TIM_CR1_CEN; // start long pressed button timer
  } else if (EXTI->PR & EXTI_PR_PIF7 and (!(GPIOC->IDR & GPIO_IDR_IDR7))) {
    EXTI->PR |= EXTI_PR_PIF7; // clear flag
    NVIC_ClearPendingIRQ(EXTI9_5_IRQn);
    Button::pThis->butStateFirst = ButtonState::UP;
    TIM3->CR1 |= TIM_CR1_CEN; // start long pressed button timer
  } else if (EXTI->PR & EXTI_PR_PIF8 and (!(GPIOC->IDR & GPIO_IDR_IDR8))) {
    EXTI->PR |= EXTI_PR_PIF8; // clear flag
    NVIC_ClearPendingIRQ(EXTI9_5_IRQn);
    Button::pThis->butStateFirst = ButtonState::DOWN;
    TIM3->CR1 |= TIM_CR1_CEN; // start long pressed button timer
  } else {
    Button::pThis->butState = ButtonState::NOT_PRESSED;
    Button::pThis->butStateFirst = ButtonState::NOT_PRESSED;
    NVIC_ClearPendingIRQ(EXTI9_5_IRQn);
    EXTI->PR |= EXTI_PR_PIF7; // clear flag
    EXTI->PR |= EXTI_PR_PIF8; // clear flag
    NVIC_EnableIRQ(EXTI9_5_IRQn);
  }
}

extern "C" void TIM3_IRQHandler(void) {
  // NVIC_EnableIRQ(EXTI4_15_IRQn); // short pressed + pause for rattle is
  // finished NVIC_EnableIRQ(EXTI0_1_IRQn);  // short pressed + pause for rattle
  // is finished EXTI->PR |= EXTI_PR_PIF1;      // clear flag EXTI->PR |=
  // EXTI_PR_PIF7;      // clear flag NVIC_ClearPendingIRQ(EXTI4_15_IRQn);
  // NVIC_ClearPendingIRQ(EXTI0_1_IRQn);
  // TIM3->SR &= ~TIM_SR_UIF;   // clear interrupt flag
  // TIM3->CR1 &= ~TIM_CR1_CEN; // disable timer
  // TIM3->CNT = 0;

  ///////////////////////////////////////////////////////////////////////////////////
  if (!Button::pThis->afterPressedCounter) {
    Button::pThis->afterPressedCounter = true;
    if (!(GPIOC->IDR & GPIO_IDR_IDR7)) {
      Button::pThis->butState = ButtonState::UP_LONG;
      Button::pThis->butStateFirst = ButtonState::NOT_PRESSED;
    } else if (!(GPIOC->IDR & GPIO_IDR_IDR8)) {
      Button::pThis->butState = ButtonState::DOWN_LONG;
      Button::pThis->butStateFirst = ButtonState::NOT_PRESSED;
    } else if (Button::pThis->butStateFirst == ButtonState::DOWN) {
      Button::pThis->butState = ButtonState::DOWN;
      Button::pThis->butStateFirst = ButtonState::NOT_PRESSED;
    } else if (Button::pThis->butStateFirst == ButtonState::UP) {
      Button::pThis->butState = ButtonState::UP;
      Button::pThis->butStateFirst = ButtonState::NOT_PRESSED;
    }
    TIM2->CNT = 0;
    TIM3->CR1 |= TIM_CR1_CEN; // start long pressed button timer
  } else {
    Button::pThis->afterPressedCounter = false;
    // short pressed + pause for rattle is finished
    NVIC_EnableIRQ(EXTI9_5_IRQn);
    EXTI->PR |= EXTI_PR_PIF7; // clear flag
    EXTI->PR |= EXTI_PR_PIF8; // clear flag
    NVIC_ClearPendingIRQ(EXTI9_5_IRQn);
    TIM3->SR &= ~TIM_SR_UIF;   // clear interrupt flag
    TIM3->CR1 &= ~TIM_CR1_CEN; // disable timer
    TIM3->CNT = 0;
    Button::pThis->butStateFirst = ButtonState::NOT_PRESSED;
  }
}
