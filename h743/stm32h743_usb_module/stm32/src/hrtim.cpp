#include "hrtim.h"

Hrtim* Hrtim::pThis = nullptr;

Hrtim::Hrtim() {
    init();
}

void Hrtim::setDuty(uint16_t duty) {
    HRTIM1->sTimerxRegs[0].CMP1xR = duty;
}

void Hrtim::init() {
    //! Gpio init PC6 - channelA1 and A2 
    RCC->AHB4ENR |= RCC_AHB4ENR_GPIOCEN;
    GPIOC->MODER |= (GPIO_MODER_MODE6_1 | GPIO_MODER_MODE7_1);
    GPIOC->OSPEEDR |= (GPIO_OSPEEDR_OSPEED6 | GPIO_OSPEEDR_OSPEED6) ; // max speed
    GPIOC->AFR[0] = (1<<24); //AF1 HRtim chA1

    RCC->AHB3ENR |= RCC_APB2ENR_HRTIMEN;

    HRTIM1->sTimerxRegs[0].PERxR = Period; // period of tim channelA1  f = (6.1kHz*65535/4000) ~ 100 kHz
    HRTIM1->sTimerxRegs[0].CMP1xR = 0;
    //! enabling dead time   (tHRTIM = 1/fHRTIM = 1/360000000) = 2.7 ns
    HRTIM1->sTimerxRegs[0].OUTxR |= HRTIM_OUTR_DTEN; //dead time enable between compementary pair
    HRTIM1->sTimerxRegs[0].DTxR |= (HRTIM_DTR_DTPRSC_0 | HRTIM_DTR_DTPRSC_1); //DT prescaler tDTG = tHRTIM = 2.7 ns  
    HRTIM1->sTimerxRegs[0].DTxR |= (HRTIM_DTR_DTR_0 | HRTIM_DTR_DTR_1 | HRTIM_DTR_DTR_2 | HRTIM_DTR_DTR_3); //tDTR = 15*tDTG = 40.5 ns

    //! set duty cycle
    HRTIM1->sTimerxRegs[0].SETx1R |= (HRTIM_SET1R_PER| HRTIM_RST1R_CMP1); // set when count till period, reset on first comparator

    //! set cmplementary pair of channelA output
    HRTIM1->sCommonRegs.OENR |= (HRTIM_OENR_TA1OEN | HRTIM_OENR_TA2OEN);
    HRTIM1->sTimerxRegs[0].TIMxCR |= HRTIM_TIMCR_CONT; // counter channelA enable

    //! setup Master timer
    HRTIM1->sMasterRegs.MPER = 65535; //master timer period
    HRTIM1->sMasterRegs.MCR |= ( HRTIM_MCR_TACEN | HRTIM_MCR_MCEN); // channel A1 enable  master counter enable    
}