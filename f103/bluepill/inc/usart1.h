#ifndef USART1_H_
#define USART1_H_

#include "main.h"
extern "C" void USART1_IRQHandler(void);
extern "C" void TIM4_IRQHandler(void);
//--------------------------------------------------------------------------------------------------

class ModbusMaster
{
public:
    ModbusMaster()
    {
        usart1_ini();
        timer4_ini();
    }
    static void sendByte(volatile uint8_t byte)
    {
        while(!(USART1->SR&USART_SR_TC)) {}
        USART1->DR=byte;
        //USART2->SR&=~USART_SR_TC;
    }
    static void sendSTR(uint8_t* str)
    {
	    uint8_t i=0;
	    while(str[i])
	    {
		    sendByte(str[i++]);		
	    }
    }
    static void sendBytes(uint8_t* buf, uint8_t size)
    {
        for(uint8_t i=0; i<size;i++)
        {
            sendByte(buf[i]);
        }
    }
    static volatile uint8_t Rx[16];
    static volatile uint8_t Tx[8];
    static volatile uint16_t Regs[8];
    static volatile uint8_t addr;
    static const uint8_t func=0x03;
    static const uint16_t regAddr=0x0000; 
    static const uint16_t regCount=0x0004;    
    static volatile uint8_t counter; 
    static const uint8_t delay=35;
    static volatile uint32_t delayCounter;
    static volatile bool endReciveFlag;
    //static uint16_t CRC_Tx;
    static uint16_t CRC16(volatile uint8_t * buf,uint8_t size)
    {
        uint16_t reg_crc=0xFFFF;//Load a 16–bit register with FFFF hex (all 1’s).
        volatile uint8_t* buf_ptr=buf;
        while(size--)
        {
            uint8_t data1=*buf_ptr;
            buf_ptr++;                     // увеличиваем указатель
            reg_crc ^= data1; //Exclusive OR the first 8–bit byte of the message with the low–order byte of the 16–bit CRC register,
                              //putting the result in the CRC register.
            for(uint8_t j=0;j<8;j++)
            {
                //Shift the CRC register one bit to the right (toward the LSB),
                //zero–filling the MSB. //"EXTRACT!!!- то есть сравнивать байт до XOR-а => в if-е" and examine the LSB.

                //(If the LSB was 1): Exclusive OR the CRC register with the polynomial value 0xA001
                //(1010 0000 0000 0001).
                if(reg_crc & 0x0001){ reg_crc=(reg_crc>>1) ^0xA001;} // LSB(b0)=1
                // (If the LSB was 0): Repeat Step 3 (another shift).
                else reg_crc=reg_crc>>1;
            }
        }
        return reg_crc;
    }

    static void readData()
    {
        	
         
        //проверка принятого CRC
        uint16_t readCRC=(Rx[counter-1]<<8)|(Rx[counter-2]);
        uint16_t countCRC = CRC16(Rx,(counter-2));
        if(countCRC==readCRC)
        {            
            uint8_t j = Rx[2]/2; //количество двойных байт данных
            for(uint8_t i=0; i<j;i++)
            {
                Regs[i]=(Rx[3+2*i]<<8)|(Rx[4+2*i]); //заполняем регистры
            }
           //Rx[3]=99;            
        }
        else {Regs[0]=0;Regs[1]=0;Regs[2]=0;Regs[3]=0;} //если ошибка вск обнуляем
        counter=0;
    }
    static void readRequest() //посылаем запрос на чтение регистров
    {
        ModbusMaster::Tx[0]=0x01;//ModbusMaster::addr;
        Tx[1]=func;
        ModbusMaster::Tx[2]=(ModbusMaster::regAddr>>8)&(0xFF);
        ModbusMaster::Tx[3]=(ModbusMaster::regAddr)&(0xFF);
        ModbusMaster::Tx[4]=(ModbusMaster::regCount>>8)&(0xFF);
        ModbusMaster::Tx[5]=(ModbusMaster::regCount)&(0xFF);
        ModbusMaster::Tx[6]=(CRC16(ModbusMaster::Tx,6)&(0xFF));
        ModbusMaster::Tx[7]=(CRC16(ModbusMaster::Tx,6)>>8)&(0xFF);
        for (uint8_t i=0;i<8;i++)
        {
            sendByte(ModbusMaster::Tx[i]);
        }              
    }

private:
    void usart1_ini()
    {
        // настраиваем пины
        RCC->APB2ENR|=RCC_APB2ENR_IOPAEN; // тактирование на порт А
        //**************************Tx portA-9
        GPIOA->CRH|=GPIO_CRH_MODE9; //11: Output mode, max speed 50 MHz.
        GPIOA->CRH|=GPIO_CRH_CNF9_1;
        GPIOA->CRH&=~GPIO_CRH_CNF9_0; //10: Alternate function output Push-pull
        //**************************Rx portA-10
        GPIOA->CRH&=~GPIO_CRH_MODE10; //00: Input mode (reset state).
        GPIOA->CRH|=GPIO_CRH_CNF10_0;
        GPIOA->CRH&=~GPIO_CRH_CNF10_1; //01: Floating input (reset state)

        //настраиваем альтернативные пины usart1
        RCC->APB2ENR|=RCC_APB2ENR_USART1EN; //подаем тактирование
        //AFIO->MAPR&=~AFIO_MAPR_USART1_REMAP; // 0: No remap (TX/PA9, RX/PA10)
        //AFIO->EXTICR[2]|=AFIO_EXTICR3_EXTI10_PA; //0000: PA[x] pin
        //AFIO->EXTICR[2]|=AFIO_EXTICR3_EXTI9_PA;  //0000: PA[x] pin
      
        USART1->CR1&=~USART_CR1_M; //0 - 8 Data bits
        USART1->CR1|=USART_CR1_TE; // Transmission enabled
        USART1->CR1|=USART_CR1_RE; // Recieving enabled
        USART1->CR1|=USART_CR1_RXNEIE; //enable interrupt on Rx from usart1
        //USART1->BRR=0x445C; //2400 (APB2CLK - baudrate/2)/baudrate
        USART1->BRR=0x9C3; //9600 //(48000000-4800)/9600 !!! какогото хрена частота шины в ДВА раза МЕНЬШЕ!!!!
	    //USART1->BRR=0x16C; //115200
        USART1->CR1|=USART_CR1_UE; //USART EN
        NVIC_EnableIRQ(USART1_IRQn);
    }
    void timer4_ini()
    {// инициализация таймера для сброса счетчика принятых байтов
        RCC->APB1ENR|=RCC_APB1ENR_TIM4EN;
        //TIM4->CR1|=TIM_CR1_ARPE;
        TIM4->PSC=48; //24000000 Hz APB PSC=2 => 48 for 1000000 Hz Timer4 frequency
        TIM4->ARR = 350-1;  // 1/9600 ~ 100us - one symbol =>* 3.5 = 350us =>350
        TIM4->DIER|=TIM_DIER_UIE; // прерывание по переполнению
        //TIM4->CR1|=TIM_CR1_CEN; //включаем таймер
        NVIC_EnableIRQ(TIM4_IRQn);         
    }
};
volatile uint8_t ModbusMaster::counter=0;
volatile bool ModbusMaster::endReciveFlag=0;
volatile uint8_t ModbusMaster::Rx[16];
volatile uint8_t ModbusMaster::Tx[8];
volatile uint16_t ModbusMaster::Regs[8];
volatile uint32_t ModbusMaster::delayCounter=0;


    

extern "C" void TIM4_IRQHandler(void) //таймер конца посылки и сигнала начала обработки данных
{    
    //GPIOC->ODR^=GPIO_ODR_ODR13;   
    TIM4->SR&=~TIM_SR_UIF;
    ModbusMaster::delayCounter++;     
    if(ModbusMaster::delayCounter>ModbusMaster::delay) // если прошло время с приема последнего байта
    {
        //GPIOC->ODR^=GPIO_ODR_ODR13;
        ModbusMaster::endReciveFlag=1; //выставляем флаг окончания приема
        TIM4->CNT=0;
        TIM4->CR1&=~TIM_CR1_CEN; //выключаем таймер
    }    
    else{ModbusMaster::endReciveFlag=0;}
    //TIM4->CR1&=~TIM_CR1_CEN; //выключаем таймер
    //TIM3->CR1&=~TIM_CR1_CEN; //выключаем таймер
}


extern "C" void USART1_IRQHandler(void) // обработка прерывания от USART1 (только на прием)
{       
    
	if(USART1->SR & USART_SR_RXNE)
	{
        USART1->SR&=~USART_SR_RXNE; // скидываем флаг
        //GPIOC->ODR ^= GPIO_ODR_ODR13;
       				
		ModbusMaster::Rx[ModbusMaster::counter++] = USART1->DR; // записываем в масси принятые данные			        
        //TIM4->CNT=0;
        TIM4->CR1|=TIM_CR1_CEN; //включаем таймер        
	}
    ModbusMaster::delayCounter=0;
}	

#endif //USART1_H_