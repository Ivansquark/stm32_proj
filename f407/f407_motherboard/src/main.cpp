#include "main.h"

#define LED_ON ((GPIOD->ODR) &= ~GPIO_ODR_ODR_7)
#define LED_OFF ((GPIOD->ODR) |= GPIO_ODR_ODR_7)
#define LED_TOGGLE ((GPIOD->ODR) ^= GPIO_ODR_ODR_7)
//void * __dso_handle = 0;
//void * __cxa_guard_acquire = 0;
//_____Ethernet packet buffers_________
volatile uint8_t RxBuf[2048]; //need to be aligned on four
volatile uint8_t TxBuf[2048];
//___________________________________
void leds_init();

int main() {
    //SCB->VTOR = (FLASH_BASE+0x20000); /* Vector Table Relocation in Internal FLASH */ // (its done in bootloader)
    // ----------  FPU initialization -----------------------------------
    SCB->CPACR |= ((3UL << 10*2)|(3UL << 11*2));  /* set CP10 and CP11 Full Access FPU init */    
    //Rcc rcc(8); //RCC config
    Rcc rcc(24); //RCC config
    Flash flash; 
    SD sd;
    uint8_t x = sd.sd_init();
    uint8_t res = sd.fat_init();
    leds_init();
    Eth eth((uint8_t *)RxBuf, (uint8_t *)TxBuf);
    //! Ethernet reinitialization if something gone wrong
    while(!eth.successInitFlag) {
        eth.hard_reset();
        eth.successInitFlag = eth.init();
        //for(volatile uint32_t i=0; i < 0x80000000; i++);
        //GPIOD->ODR ^= GPIO_ODR_ODR_15;
        LED_TOGGLE;
    }
    eth.receive_frame();
    for (uint8_t i = 0; i < 60; i++) ;
    GpTimer tim(2); //500 ms
    __enable_irq();   
    eth.arp_send(); // tell who we are
    for(uint32_t i=0;i<9000000;i++);
    eth.frame_read();
    eth.ReceiveFlag=false;
    uint32_t debugCounter = 0;
    bool startFlag=false;
    Eth::pThis->ReceiveDL[0] |= (1<<31); //sets OWN bit to DMA (read completed)
    //GPIOD->ODR ^= GPIO_ODR_ODR_12;
    while(1)
    {
        if(GpTimer::timFlag) {
            //debugCounter++;
            //GPIOD->ODR ^= GPIO_ODR_ODR_12;
            //GPIOD->ODR ^= GPIO_ODR_ODR_14;
            LED_TOGGLE;
            GpTimer::timFlag = false;
        }
        if(eth.ReceiveFlag) {
            eth.frame_read();
            eth.ReceiveFlag=false;
            Eth::pThis->ReceiveDL[0] |= (1<<31); //sets OWN bit to DMA (read completed)
        }
        //! UDP received
        if(eth.UDPflag) {
            eth.UDPflag = false;
            //GPIOD->ODR ^= GPIO_ODR_ODR_13;
            //udp received data [0]='<', [1]=PackNum_high, [2]=PackNum_low, [3]=size(255 max) ... 
            //...max 255 bytes of data (if less 255 => last PackNum)
            if(eth.UDP_data[0] == '<') {
                eth.packNum = (eth.UDP_data[1] << 8) + eth.UDP_data[2];
                eth.size = eth.UDP_data[3];
                if(eth.size < 128) {
                    if(eth.packNum == 0) {
                        // if received packet with command, otherwise with data
                        if ((eth.UDP_data[4] == 'c') && (eth.UDP_data[5] == 'o') &&
                            (eth.UDP_data[6] == 'm') && (eth.UDP_data[7] == 'm') &&
                            (eth.UDP_data[8] == 'a') && (eth.UDP_data[9] == 'n') &&
                            (eth.UDP_data[10] == 'd')) {
                            // TODO:: command handler
                            if (eth.UDP_data[11] == ' ' && eth.UDP_data[12] == 'b' &&
                                eth.UDP_data[13] == 'o' && eth.UDP_data[14] == 'o' &&
                                eth.UDP_data[15] == 't') {
                                //GPIOD->ODR ^= GPIO_ODR_ODR_15;
                                uint8_t bootFlag = 0x1;
                                //! write boote flag to 1 and then programm reset controller
                                flash.writeBootFlags(&bootFlag,1);
                                //______________ Software RESET _______________________
                                //__ To write to this register, you must write 0x5FA
                                //__ to the VECTKEY field, otherwise the
                                //__ processor ignores the write._____________________________
                                SCB->AIRCR = 0x05FA0004;
                            }
                            uint8_t temp_arr[132];
                            temp_arr[0] = '>';
                            temp_arr[1] = eth.packNum << 8;
                            temp_arr[2] = eth.packNum;
                            char answer[] = "command ok";
                            temp_arr[3] = sizeof(answer);
                            for (int i = 4; i < 4 + sizeof(answer); i++) {
                                temp_arr[i] = answer[i - 4];
                            }                            
                            eth.udp_writeReply(temp_arr, 4 + temp_arr[3]);
                        } else {
                            // last data packet
                        }
                    }
                } else {
                    // data packet uint8_t* data,uint16_t len
                    uint8_t temp_arr[132];
                    temp_arr[0] = '>'; temp_arr[1] = eth.packNum << 8; temp_arr[2] = eth.packNum;
                    temp_arr[4] = 'o'; temp_arr[5] = 'k'; temp_arr[2] = 2;
                    eth.udp_writeReply(temp_arr, 4 + temp_arr[2]);
                }
            }
            //Eth::pThis->ReceiveDL[0] |= (1<<31); //sets OWN bit to DMA (read completed)
        }
        else {}        
    }
    return 0;
}

void leds_init() {
    //!________ D7 led pin push-pull ______________
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
    GPIOD->MODER |= (GPIO_MODER_MODER7_0);
    GPIOD->MODER &= ~(GPIO_MODER_MODER7_1);
    //!________ E3 E4 E5 analog HiZ (not disturb SPI)__
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;
    GPIOE->MODER |= (GPIO_MODER_MODER3 | GPIO_MODER_MODER4 | GPIO_MODER_MODER5); //1:1 - high impedance analog
}
