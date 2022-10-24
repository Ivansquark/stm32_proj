#include "rtc.h"
Rtc* Rtc::pThis = nullptr;

Rtc::Rtc() {
    init();
    pThis =this;
}

void Rtc::setTime(uint8_t hour, uint8_t minute, uint8_t second) {
    PWR->CR1 |= PWR_CR1_DBP; // enable write bit in RTC backup and sram register
    RCC->APB4ENR |= RCC_APB4ENR_RTCAPBEN;
    RTC->WPR = 0xCA;
    RTC->WPR = 0x53; //take off write protections in RTC registers
    RTC->ISR |= RTC_ISR_INIT; // to enter initialization mode
    uint16_t counter = 0xFFFF;
    while((!(RTC->ISR & RTC_ISR_INITF)) || (counter)) { counter--; }
    //RTC->PRER |= ((127<<16) | (255));//default is the same sets prescalers to generate 1Hz clock
    RTC->CR &=~ RTC_CR_FMT; // sets 24 time format
    
    RTC->TR |= ((((hour/10) << RTC_TR_HT_Pos) + ((hour%10) << RTC_TR_HU_Pos)) |
                (((minute/10) << RTC_TR_MNT_Pos) + ((minute%10) << RTC_TR_MNU_Pos)) |
                (((second/10) << RTC_TR_ST_Pos) + ((second%10) << RTC_TR_SU_Pos)));

    RTC->ISR &=~ RTC_ISR_INIT; // exit initialization and start RTC timer
    RTC->WPR = 0xFF;
}
void Rtc::setDate(uint8_t year, uint8_t month, uint8_t day, uint8_t wd) {
    PWR->CR1 |= PWR_CR1_DBP; // enable write bit in RTC backup and sram register
    RCC->APB4ENR |= RCC_APB4ENR_RTCAPBEN;
    RTC->WPR = 0xCA;
    RTC->WPR = 0x53; //take off write protections in RTC registers
    RTC->ISR |= RTC_ISR_INIT; // to enter initialization mode
    uint16_t counter = 0xFFFF;
    while((!(RTC->ISR & RTC_ISR_INITF)) || (counter)) { counter--; }
    //RTC->PRER |= ((127<<16) | (255));//default is the same sets prescalers to generate 1Hz clock
    RTC->CR &=~ RTC_CR_FMT; // sets 24 time format
    RTC->DR = 0x00002101;
    //RTC->DR &=~ (RTC_DR_WDU | RTC_DR_YT | RTC_DR_YU | RTC_DR_MT | RTC_DR_MU | RTC_DR_DT | RTC_DR_DU);
    RTC->DR |= ((((year/10) << RTC_DR_YT_Pos) | ((year%10) << RTC_DR_YU_Pos)) |
                ((((month/10)&0x01) << RTC_DR_MT_Pos) | ((month%10) << RTC_DR_MU_Pos)) |
                ((((day/10)&0x3)<<RTC_DR_DT_Pos) | ((day%10) << RTC_DR_DU_Pos)) |
                ((wd&0x7) << RTC_DR_WDU_Pos));
                 
    RTC->ISR &=~ RTC_ISR_INIT; // exit initialization and start RTC timer
    RTC->WPR = 0xFF;
}
Rtc::Time* Rtc::getTime() {
    if(RTC->ISR & RTC_ISR_RSF) {
        currentTime.hour = ((RTC->TR>>RTC_TR_HT_Pos) & 0x0F)*10 + ((RTC->TR>>RTC_TR_HU_Pos) & 0x0F) ;
        currentTime.minute = ((RTC->TR>>RTC_TR_MNT_Pos) & 0x0F)*10 + ((RTC->TR>>RTC_TR_MNU_Pos) & 0x0F);
        currentTime.second = ((RTC->TR>>RTC_TR_ST_Pos) & 0x0F)*10 + ((RTC->TR>>RTC_TR_SU_Pos) & 0x0F);
    }    
    return &currentTime; 
}
Rtc::Date* Rtc::getDate() {
    if(RTC->ISR & RTC_ISR_RSF) {
        currentDate.year = ((RTC->DR >> RTC_DR_YT_Pos) & 0xF)*10 + ((RTC->DR >> RTC_DR_YU_Pos) & 0xF) ;
        currentDate.month = ((RTC->DR >> RTC_DR_MT_Pos) & 0x1)*10 + ((RTC->DR >> RTC_DR_MU_Pos) & 0xF) ;
        currentDate.day = ((RTC->DR >> RTC_DR_DT_Pos) & 0xF)*10 + ((RTC->DR>>RTC_DR_DU_Pos) & 0xF) ;
        currentDate.weekDay = (RTC->DR >> RTC_DR_WDU_Pos) & 0xF; //week of days units
    }
    return &currentDate;
}

void Rtc::init() {
    LSE_init();    
    if(RTC->ISR & RTC_ISR_INITS) return;
    PWR->CR1 |= PWR_CR1_DBP; // enable write bit in RTC backup and sram register
    RCC->APB4ENR |= RCC_APB4ENR_RTCAPBEN;
    RTC->WPR = 0xCA;
    RTC->WPR = 0x53; //take off write protections in RTC registers
    RTC->ISR |= RTC_ISR_INIT; // to enter initialization mode
    uint16_t counter = 0xFFFF;
    while((!(RTC->ISR & RTC_ISR_INITF)) || (counter)) { counter--; }
    //RTC->PRER |= ((127<<16) | (255));//default is the same sets prescalers to generate 1Hz clock
    RTC->CR &=~ RTC_CR_FMT; // sets 24 time format
    
    RTC->TR |= ((((1)<<RTC_TR_HT_Pos) + ((5)<<RTC_TR_HU_Pos)) |
                (((5)<<RTC_TR_MNT_Pos) + ((5)<<RTC_TR_MNU_Pos)) |
                (((0)<<RTC_TR_ST_Pos) + ((0)<<RTC_TR_SU_Pos)));
    
    RTC->DR |= ((((2/10)<<RTC_DR_YT_Pos) + ((1%10)<<RTC_DR_YU_Pos)) |
                (((0/10)<<RTC_DR_MT_Pos) + ((1%10)<<RTC_DR_MU_Pos)) |
                (((0/10)<<RTC_DR_DT_Pos) + ((5%10)<<RTC_DR_DU_Pos)) |
                (2<<RTC_DR_WDU_Pos));
    RTC->ISR &=~ RTC_ISR_INIT; // exit initialization and start RTC timer
    RTC->WPR = 0xFF;
}

void Rtc::LSE_init() {
    PWR->CR1 |= PWR_CR1_DBP; // enable write bit in RTC backup and sram register
    if(RTC->ISR & RTC_ISR_INITS) return;        
    RCC->BDCR |= RCC_BDCR_LSEON;
    while((!(RCC->BDCR & RCC_BDCR_LSERDY))); 
    //RCC->BDCR |= RCC_BDCR_BDRST;
    //RCC->BDCR &=~ RCC_BDCR_BDRST;
    RCC->BDCR |= RCC_BDCR_RTCSEL_0; // LSE selected   
   
    RCC->BDCR |= RCC_BDCR_RTCEN;
}