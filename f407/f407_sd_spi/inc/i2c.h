#ifndef I2C_H_
#define I2C_H_

//#include "main.h"
#include "stm32f4xx.h"

class I2C_clock
{
public:
    I2C_clock(){i2c_ini();}

    struct RTC1307
    {
        uint8_t sec=00;
        uint8_t min=30;
        uint8_t hour=23;
        uint8_t day=7;
        uint8_t date=10;
        uint8_t month=5;
        uint8_t year=20;
    };
    RTC1307 rtcWrite;
    volatile RTC1307 rtcRead{0,0,0,0,0,0,0};
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
    volatile uint8_t arr[32];
    uint16_t arrSize=0;
    const uint8_t slaveWrite = 0xA0;
    const uint8_t slaveRead = 0xA1;
    const uint8_t arrDefault[20] = {0x14,0x13,0x12,0x11,0x10,
                        0x0F,0x0E,0x0D,0x0C,0x0B,
                        0x0A,0x09,0x08,0x07,0x06,
                        0x05,0x04,0x03,0x02,0x01};
    uint8_t DecToBinDec(uint8_t bin)
    {
        return ((bin/10)<<4)|(bin%10);
    }
    uint8_t BinDecToDec(uint8_t dec)
    {
        return ((dec>>4)*10+(0x0F&dec));
    }

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

    void i2c_ini()
    {
        RCC->AHB1ENR|=RCC_AHB1ENR_GPIOBEN;
        GPIOB->MODER|=GPIO_MODER_MODER6_1;
        GPIOB->MODER&=~GPIO_MODER_MODER6_0;//10: PB-6 alternate func
        GPIOB->MODER|=GPIO_MODER_MODER7_1;
        GPIOB->MODER&=~GPIO_MODER_MODER7_0;//10: PB-7 alternate func
        GPIOB->OTYPER|=GPIO_OTYPER_OT_6|GPIO_OTYPER_OT_7;
        GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPDR6_0|GPIO_PUPDR_PUPDR6_1);        
        GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPDR7_0|GPIO_PUPDR_PUPDR7_1);           
	    GPIOB->OSPEEDR|=GPIO_OSPEEDER_OSPEEDR6;
        GPIOB->OSPEEDR|=GPIO_OSPEEDER_OSPEEDR7; //11: high speed
        GPIOB->AFR[0]|=(4<<24)|(4<<28);//
        
        RCC->APB1ENR|=RCC_APB1ENR_I2C1EN;
        I2C1->CR1|=I2C_CR1_ACK; //ACK в ответе       генерировать условие ACK после приёма байтов
        I2C1->CR2&=~I2C_CR2_FREQ; // сбрасываем;
        I2C1->CR2|=42; //  Peripheral clock frequency - устанавливаем частоту 
        //I2C1->CR2|=I2C_CR2_ITEVTEN; // включаем прерывания по событию 
        // TPCLK1 = 24ns
        I2C1->CCR&=~I2C_CCR_CCR; //обнуляем CCR - значения контроля частоты
        I2C1->CCR&=~I2C_CCR_FS; //0: Sm mode I2C     1: Fm mode I2C
        I2C1->CCR&=~I2C_CCR_DUTY; // only for fast mode 0: Fm mode tlow/thigh = 2 1: Fm mode tlow/thigh = 16/9 (see CCR)
        //SM: THIGH=CCR*TPCLK1=TLOW ::: THIGH+TLOW=10000ns =>
        //т.к. THIGH+TLOW=1/100kHz => THIGH+TLOW=10us=10000ns
        // CCR*(2*TPCLK1)=10000ns  => CCR=10000ns/(2*24ns) => CCR=208
        I2C1->CCR|=208;
        //TRISE: Tr=1000ns (max rise time) => TRISE=(Tr/TPCLK1)+1=43;
        I2C1->TRISE=41;  //максимальное время => заносим немного меньшее
        //NVIC_EnableIRQ(I2C1_EV_IRQn);
        //NVIC_SetPriority(I2C1_EV_IRQn, 1);
        I2C1->CR1|=I2C_CR1_PE; //включаем        

    }
};

#endif //I2C_H_