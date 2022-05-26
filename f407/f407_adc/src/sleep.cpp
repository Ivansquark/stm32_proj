#include "sleep.h"

Sleep* Sleep::pThis = nullptr;

Sleep::Sleep() {
    init();
}

void Sleep::sleep() {
    __WFI();
}

void Sleep::init() {
    SCB->SCR &=~ SCB_SCR_SLEEPDEEP_Msk; // 0:SLeep   1::DEEPSLEEP
    SCB->SCR &=~ SCB_SCR_SLEEPONEXIT_Msk; // 0:WFI or WFE    1: SLEEP when return from ISR   
}