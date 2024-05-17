#ifndef I2C1_H_
#define I2C1_H_

#include "main.h"

extern "C" void TIM3_IRQHandler(void);
//**********************************************************

class I2C1_master
{
public:
    I2C1_master(uint8_t x=0)
    {
        if(x==0){ i2c1_iniSM();}  //100 kHz
        else {i2c1_iniFM();}      //400 kHz
        timer3_ini();
    }
    static volatile bool timeFlag;
    void writeBytes(uint16_t addr,const uint8_t* buf,uint16_t size)
    {   
        I2C2->CR1|=I2C_CR1_ACK;
        I2C1->CR1|=I2C_CR1_START; //формируем старт сигнал
        while (!(I2C1->SR1 & I2C_SR1_SB)); //ждем пока пошлется старт бит            
        (void) I2C1->SR1; //считываем регистр для очистки флага старта
        I2C1->DR=slaveWrite; // передаем первый байт с адресом устройства и битом на запись
        while (!(I2C1->SR1 & I2C_SR1_ADDR));	//ждем пока пошлется адрес        
        (void) I2C1->SR1;
        (void) I2C1->SR2;        
        I2C1->DR=addr>>8; //передаем старший байт
        while(!(I2C1->SR1&I2C_SR1_TXE)); //ждем пока освободится буфер
        I2C1->DR=addr&0xFF; //передаем младший байт
        while(!(I2C1->SR1&I2C_SR1_TXE)); //ждем пока освободится буфер
        for(volatile uint8_t i=0;i<size;i++)
        {
            I2C1->DR=buf[i]; //передаем байт данных которые хотим записать в память        
            while (!(I2C1->SR1 & I2C_SR1_BTF)); //ждем окончания передачи            
        }
        I2C1->CR1|=I2C_CR1_STOP;  //формируем стоп сигнал              
    }
    void readBytes(uint16_t addr,uint16_t size) // считываем в массив начиная с адреса
    {        
        I2C2->CR1|=I2C_CR1_ACK;
        I2C1->CR1|=I2C_CR1_START; //формируем старт сигнал
        while (!(I2C1->SR1 & I2C_SR1_SB)); //ждем отправки старт сигнала            
    	(void) I2C1->SR1;
        I2C1->DR=slaveWrite; // передаем первый байт с адресом устройства и битом на запись        
        while (!(I2C1->SR1 & I2C_SR1_ADDR)); //ждем отправки адреса	
        (void) I2C1->SR1;    	
    	(void) I2C1->SR2;        
        I2C1->DR=addr>>8; //передаем старший байт
        while(!(I2C1->SR1&I2C_SR1_TXE)); //ждем пока освободится буфер
        I2C1->DR=addr&0xFF; //передаем младший байт
        while(!(I2C1->SR1&I2C_SR1_TXE)); //ждем пока освободится буфер         
        I2C1->CR1 |= I2C_CR1_START; //restart
        while (!(I2C1->SR1 & I2C_SR1_SB)); //ждем отправки старт сигнала
        I2C1->DR=slaveRead; // передаем первый байт с адресом устройства и битом на запись
        while (!(I2C1->SR1 & I2C_SR1_ADDR)); //ждем пока передастся адрес        
        (void) I2C1->SR1;
        (void) I2C1->SR2;
        for(volatile uint8_t i=0;i<size;i++)
        {
            if(i<size-1)
            {
                I2C1->CR1|=I2C_CR1_ACK;
                while (!(I2C1->SR1&I2C_SR1_RXNE));// пока заполниться буфер
                this->arr[i]=I2C1->DR; //передаем байт данных которые хотим записать в память                        
            }
            else if(i==size-1)
            {
                I2C1->CR1&=~I2C_CR1_ACK; //no acknowlege!!!;
                while (!(I2C1->SR1&I2C_SR1_RXNE));// пока заполниться буфер
                this->arr[i]=I2C1->DR;
            }           
        }    
        I2C1->CR1|=I2C_CR1_STOP;  //формируем стоп сигнал             
    }
    void writeByte(uint16_t address,uint8_t byte)
    {
        I2C2->CR1|=I2C_CR1_ACK;
        I2C1->CR1|=I2C_CR1_START; //формируем старт сигнал
        while (!(I2C1->SR1 & I2C_SR1_SB)); //ждем пока пошлется старт бит            
        (void) I2C1->SR1; //считываем регистр для очистки флага старта
        I2C1->DR=slaveWrite; // передаем первый байт с адресом устройства и битом на запись
        while (!(I2C1->SR1 & I2C_SR1_ADDR));	//ждем пока пошлется адрес        
        (void) I2C1->SR1;
        (void) I2C1->SR2;        
        I2C1->DR=address>>8; //передаем старший байт
        while(!(I2C1->SR1&I2C_SR1_TXE)); //ждем пока освободится буфер
        I2C1->DR=address&0xFF; //передаем младший байт
        while(!(I2C1->SR1&I2C_SR1_TXE)); //ждем пока освободится буфер
        I2C1->DR=byte; //передаем байт данных которые хотим записать в память        
        while (!(I2C1->SR1 & I2C_SR1_BTF)); //ждем окончания передачи
        I2C1->CR1|=I2C_CR1_STOP;  //формируем стоп сигнал
    }
    uint8_t readByte(uint16_t address)
    {
        uint8_t x=0;
        I2C2->CR1|=I2C_CR1_ACK;
        I2C1->CR1|=I2C_CR1_START; //формируем старт сигнал
        while (!(I2C1->SR1 & I2C_SR1_SB)); //ждем отправки старт сигнала            
    	(void) I2C1->SR1;
        I2C1->DR=slaveWrite; // передаем первый байт с адресом устройства и битом на запись        
        while (!(I2C1->SR1 & I2C_SR1_ADDR)); //ждем отправки адреса	
        (void) I2C1->SR1;    	
    	(void) I2C1->SR2;        
        I2C1->DR=address>>8; //передаем старший байт
        while(!(I2C1->SR1&I2C_SR1_TXE)); //ждем пока освободится буфер
        I2C1->DR=address&0xFF; //передаем младший байт
        while(!(I2C1->SR1&I2C_SR1_TXE)); //ждем пока освободится буфер         
        I2C1->CR1 |= I2C_CR1_START; //restart
        while (!(I2C1->SR1 & I2C_SR1_SB)); //ждем отправки старт сигнала
        I2C1->DR=slaveRead; // передаем первый байт с адресом устройства и битом на запись
        while (!(I2C1->SR1 & I2C_SR1_ADDR)); //ждем пока передастся адрес        
        (void) I2C1->SR1;
        (void) I2C1->SR2;
        I2C1->CR1&=~I2C_CR1_ACK; //no acknowlege!!!;
        while (!(I2C1->SR1&I2C_SR1_RXNE));// пока заполниться буфер
        x = I2C1->DR; //считываем байт данных которые хотим записать в память        
        I2C1->CR1|=I2C_CR1_STOP;  //формируем стоп сигнал
        return x;
    }

    uint8_t DecToBinDec(uint8_t bin)
    {
        return ((bin/10)<<4)|(bin%10);
    }
    uint8_t BinDecToDec(uint8_t dec)
    {
        return ((dec>>4)*10+(0x0F&dec));
    }
   

    volatile uint8_t arr[32];
    uint16_t arrSize=0;
    const uint8_t slaveWrite = 0xA0;
    const uint8_t slaveRead = 0xA1;
    const uint8_t arrDefault[20] = {0x14,0x13,0x12,0x11,0x10,
                        0x0F,0x0E,0x0D,0x0C,0x0B,
                        0x0A,0x09,0x08,0x07,0x06,
                        0x05,0x04,0x03,0x02,0x01};
    typedef struct RTC1307
    {
        uint8_t sec=00;
        uint8_t min=58;
        uint8_t hour=17;
        uint8_t day=6;
        uint8_t date=4;
        uint8_t month=4;
        uint8_t year=20;
    } rtc1307;
    rtc1307 rtcWrite;
    volatile rtc1307 rtcRead{0,0,0,0,0,0,0};
    void rtc_write()
    {
        I2C1->CR1|=I2C_CR1_ACK;
        I2C1->CR1|=I2C_CR1_START;
        while(!(I2C1->SR1&I2C_SR1_SB)); //пока не появится старт бит
        void(I2C1->SR1); //сбрасываем старт бит
        I2C1->DR=0xD0; //записываем адрес (0b1101) 1307 + бит записи
        while (!(I2C1->SR1 & I2C_SR1_ADDR));	//ждем пока пошлется адрес
        void(I2C1->SR1);
        void(I2C1->SR2); // двумя считываниями сбрасываем бит адреса
        I2C1->DR=0x00; //записываем первый адрес регистра 1307
        while(!(I2C1->SR1&I2C_SR1_TXE));
        //******* последовательно записываем 7 регистров значениями из структуры rtcWrite
        I2C1->DR = DecToBinDec(rtcWrite.sec); //записываем секунды + бит запуска 1307 (0x80)|
        while (!(I2C1->SR1 & I2C_SR1_BTF)); //ждем окончания передачи
        I2C1->DR = DecToBinDec(rtcWrite.min);
        while (!(I2C1->SR1 & I2C_SR1_BTF)); //ждем окончания передачи
        I2C1->DR = DecToBinDec(rtcWrite.hour); // бит 24 форматного времени (0x40)|
        while (!(I2C1->SR1 & I2C_SR1_BTF)); //ждем окончания передачи
        I2C1->DR = DecToBinDec(rtcWrite.day);
        while (!(I2C1->SR1 & I2C_SR1_BTF)); //ждем окончания передачи
        I2C1->DR = DecToBinDec(rtcWrite.date);
        while (!(I2C1->SR1 & I2C_SR1_BTF)); //ждем окончания передачи
        I2C1->DR = DecToBinDec(rtcWrite.month);
        while (!(I2C1->SR1 & I2C_SR1_BTF)); //ждем окончания передачи
        I2C1->DR = DecToBinDec(rtcWrite.year);
        while (!(I2C1->SR1 & I2C_SR1_BTF)); //ждем окончания передачи
        I2C1->CR1|=I2C_CR1_STOP;
    }
    void rtc_read()
    {
        I2C1->CR1|=I2C_CR1_ACK;
        I2C1->CR1|=I2C_CR1_START;
        while(!(I2C1->SR1&I2C_SR1_SB)); //пока не появится старт бит
        void(I2C1->SR1); //сбрасываем старт бит
        I2C1->DR=0xD0; //записываем адрес (0b1101) 1307 + бит записи
        while (!(I2C1->SR1 & I2C_SR1_ADDR));	//ждем пока пошлется адрес
        void(I2C1->SR1);
        void(I2C1->SR2); // двумя считываниями сбрасываем бит адреса
        I2C1->DR=0x00; //записываем первый адрес регистра 1307
        while(!(I2C1->SR1&I2C_SR1_TXE));
        I2C1->CR1|=I2C_CR1_START;
        while(!(I2C1->SR1&I2C_SR1_SB)); //пока не появится старт бит
        void(I2C1->SR1); //сбрасываем старт бит
        I2C1->DR=0xD1; //записываем адрес (0b1101) 1307 + бит чтения
        while (!(I2C1->SR1 & I2C_SR1_ADDR));	//ждем пока пошлется адрес
        void(I2C1->SR1);
        void(I2C1->SR2); // двумя считываниями сбрасываем бит адреса        
        //******* последовательно считываем 7 регистров значениями в структуру rtcRead
        I2C1->CR1|=I2C_CR1_ACK;
        while(!(I2C1->SR1&I2C_SR1_RXNE));
        rtcRead.sec = BinDecToDec(I2C1->DR);//BinDecToDec(I2C1->DR); //записываем секунды + бит запуска 1307 (0x80)|
        I2C1->CR1|=I2C_CR1_ACK;
        while(!(I2C1->SR1&I2C_SR1_RXNE));
        rtcRead.min = BinDecToDec(I2C1->DR);
        I2C1->CR1|=I2C_CR1_ACK;
        while(!(I2C1->SR1&I2C_SR1_RXNE));
        rtcRead.hour = BinDecToDec(I2C1->DR);
        I2C1->CR1|=I2C_CR1_ACK;
        while(!(I2C1->SR1&I2C_SR1_RXNE));
        rtcRead.day = BinDecToDec(I2C1->DR);
        I2C1->CR1|=I2C_CR1_ACK;
        while(!(I2C1->SR1&I2C_SR1_RXNE));
        rtcRead.date = BinDecToDec(I2C1->DR);
        I2C1->CR1|=I2C_CR1_ACK;
        while(!(I2C1->SR1&I2C_SR1_RXNE));
        rtcRead.month = BinDecToDec(I2C1->DR);
        I2C1->CR1&=~I2C_CR1_ACK;                //NACK
        while(!(I2C1->SR1&I2C_SR1_RXNE));
        rtcRead.year = BinDecToDec(I2C1->DR);        
        I2C1->CR1|=I2C_CR1_STOP;
    }

private:
    void i2c1_iniSM()
    {
        // настройка пинов pb7-data pb6-sck
        RCC->APB2ENR|=RCC_APB2ENR_IOPBEN;        
        //RCC->APB2ENR|=RCC_APB2ENR_AFIOEN;// not nescessary
        GPIOB->CRL|=GPIO_CRL_MODE6|GPIO_CRL_CNF6; //1:1 1:1 alt func open drain max speed
        GPIOB->CRL|=GPIO_CRL_MODE7|GPIO_CRL_CNF7; //1:1 1:1 alt func open drain max speed
        GPIOB->BSRR=GPIO_BSRR_BS6|GPIO_BSRR_BS7;
        //********** настройка I2C1 24 MHz - шина *********************   
        RCC->APB1ENR|=RCC_APB1ENR_I2C1EN;
        //I2C1->CR1&=~I2C_CR1_PE; //выключаем
        I2C1->CR1|=I2C_CR1_ACK; //ACK в ответе       генерировать условие ACK после приёма байтов
        I2C1->CR2&=~I2C_CR2_FREQ; // сбрасываем;
        I2C1->CR2|=24; //  Peripheral clock frequency - устанавливаем частоту 
        //I2C1->CR2|=I2C_CR2_ITEVTEN; // включаем прерывания по событию 
        // TPCLK1 = 42ns
        I2C1->CCR&=~I2C_CCR_CCR; //обнуляем CCR - значения контроля частоты
        I2C1->CCR&=~I2C_CCR_FS; //0: Sm mode I2C     1: Fm mode I2C
        I2C1->CCR&=~I2C_CCR_DUTY; // only for fast mode 0: Fm mode tlow/thigh = 2 1: Fm mode tlow/thigh = 16/9 (see CCR)
        //SM: THIGH=CCR*TPCLK1=TLOW ::: THIGH+TLOW=10000ns =>
        //т.к. THIGH+TLOW=1/100kHz => THIGH+TLOW=10us=10000ns
        // CCR*(2*TPCLK1)=10000ns  => CCR=10000ns/(2*42ns) =>
        // CCR=120
        I2C1->CCR|=120;
        //TRISE: Tr=1000ns (max rise time) => TRISE=(Tr/TPCLK1)+1=24;
        I2C1->TRISE=22;  //максимальное время => заносим немного меньшее
        //NVIC_EnableIRQ(I2C1_EV_IRQn);
        //NVIC_SetPriority(I2C1_EV_IRQn, 1);
        I2C1->CR1|=I2C_CR1_PE; //включаем        
    }
    void i2c1_iniFM()
    {
        // настройка пинов pb7-data pb6-sck
        RCC->APB2ENR|=RCC_APB2ENR_IOPBEN;
        RCC->APB2ENR|=RCC_APB2ENR_AFIOEN;// not nescessary
        GPIOB->CRL|=GPIO_CRL_MODE6|GPIO_CRL_CNF6; //1:1 1:1 alt func open drain max speed
        GPIOB->CRL|=GPIO_CRL_MODE7|GPIO_CRL_CNF7; //1:1 1:1 alt func open drain max speed

        //********** настройка I2C1 24 MHz - шина *********************
        I2C1->CR1&=~I2C_CR1_PE; //выключаем
        RCC->APB1ENR|=RCC_APB1ENR_I2C1EN;
        I2C1->CR1|=I2C_CR1_ACK; //ACK в ответе
        I2C1->CR2&=~I2C_CR2_FREQ; // сбрасываем;
        I2C1->CR2|=24; //  Peripheral clock frequency - устанавливаем частоту 
        // TPCLK1 = 42ns
        I2C1->CCR&=~I2C_CCR_CCR; //обнуляем CCR - значения контроля частоты
        I2C1->CCR|=I2C_CCR_FS; //0: Sm mode I2C     1: Fm mode I2C
        I2C1->CCR&=~I2C_CCR_DUTY; // 0: Fm mode tlow/thigh = 2 1: Fm mode tlow/thigh = 16/9 (see CCR)
        //FM (DUTY=1 to reach 400kHz): THIGH=9*CCR*TPCLK1; TLOW=16*CCR*TPCLK1; THIGH+TLOW=2500ns =>
        // т.к. THIGH+TLOW=1/100kHz => THIGH+TLOW=2.5us=2500ns
        // CCR*(TPCLK1*(9+16))=2500ns => CCR=2500ns/(25*TPCLK1)=100/42=2
        // CCR=2               (if DUTY=0 =>CCR=17)
        I2C1->CCR|=2;
        //I2C1->CR2|=I2C_CR2_ITEVTEN; // включаем прерывания по событию 

        //TRISE: Tr=300ns (max rise time) => TRISE=(Tr/TPCLK1)+1=8;
        I2C1->TRISE=6; //максимальное время => заносим немного меньшее
        //NVIC_EnableIRQ(I2C1_EV_IRQn);
        //NVIC_SetPriority(I2C1_EV_IRQn, 1);
        I2C1->CR1|=I2C_CR1_PE; //включаем
    }
    void timer3_ini()
    {
        RCC->APB1ENR|=RCC_APB1ENR_TIM3EN;
        //TIM3->CR1|=TIM_CR1_ARPE;
        TIM3->PSC=48000; //24000000 Hz APB PSC=2 =>  1000 Hz Timer3 frequency
        TIM3->ARR = 500;  // 200ms
        TIM3->DIER|=TIM_DIER_UIE; // прерывание по переполнению
        TIM3->CR1|=TIM_CR1_CEN; //включаем таймер
        NVIC_EnableIRQ(TIM3_IRQn);     
    }
};
volatile bool I2C1_master::timeFlag=0;
extern "C" void TIM3_IRQHandler(void)
{
    TIM3->SR &=~ TIM_SR_UIF; // сбрасываем флаг прерывания таймера
    I2C1_master::timeFlag=1;
    //GPIOC->ODR^=GPIO_ODR_ODR13;
    //GPIOC->BSRR|=GPIO_BSRR_BS13;
    }
#endif //I2C1_H_