#include "ds3231.h"

inline uint8_t dec_to_bcd(uint8_t dec) {
  return ((((dec / 10) & 0xF) << 4) | ((dec % 10) & 0xF));
}
inline uint8_t bcd_to_dec(uint8_t bcd) {
    return ((bcd >> 4)*10 + (0x0F & bcd));
}
void setSecond(uint8_t sec) {
    write_i2c_reg(0x00,sec);
}
void setMinute(uint8_t min) {
    write_i2c_reg(0x01,min);
}
void setHour(uint8_t hour) {
    write_i2c_reg(0x02,hour);
}
void setDayWeek(uint8_t dayweek) {
    write_i2c_reg(0x03,dayweek);
}
void setDay(uint8_t day) {
    write_i2c_reg(0x04,day);
}
void setMonth(uint8_t month) {
    write_i2c_reg(0x05,month);
}
void setYear(uint8_t year) {
    write_i2c_reg(0x06,year);
}

void setTime(uint8_t hour, uint8_t min, uint8_t sec) {
    setHour(hour); setMinute(min); setSecond(sec);
}
void setDate(uint8_t day, uint8_t month, uint8_t year, uint8_t dayweek) {
    setYear(year); setMonth(month); setDay(day); setDayWeek(dayweek);
}

void readDateTime(uint32_t* dateTimeArray) {
    read_i2c_regs(0,dateTimeArray,7);
}

inline void i2c_start(void) {
    // Send 'Start' condition, and wait for acknowledge.
    I2C1->CR2 |=  (I2C_CR2_START);
    uint32_t timeout = 0xFFFF;
    while ((I2C1->CR2 & I2C_CR2_START) && timeout--) {}
}
inline void i2c_stop(void) {
  // Send 'Stop' condition, and wait for acknowledge.
  I2C1->CR2 |=  (I2C_CR2_STOP);
  uint32_t timeout = 0xFFFF;
  while ((I2C1->CR2 & I2C_CR2_STOP) && timeout--) {}
  // Reset the ICR ('Interrupt Clear Register') event flag.
  I2C1->ICR |=  (I2C_ICR_STOPCF);
  timeout = 0xFFFF;
  while ((I2C1->ICR & I2C_ICR_STOPCF) && timeout--) {}
}

uint8_t i2c_write_byte(uint8_t dat) {
    I2C1->TXDR = (I2C1->TXDR & 0xFFFFFF00) | dat;
    // Wait for one of these ISR bits:
    // 'TXIS' ("ready for next byte")
    // 'TC'   ("transfer complete")
    uint32_t timeout = 0xFFFF;
    while (!(I2C1->ISR & (I2C_ISR_TXIS | I2C_ISR_TC))&& timeout--) {}
    if(!timeout) {return 0;}
    return 1;
    // (Also of interest: 'TXE' ("TXDR register is empty") and
    //  'TCR' ("transfer complete, and 'RELOAD' is set."))
}
uint8_t i2c_read_byte(void) {
    // Wait for a byte of data to be available, then read it.
    uint32_t timeout = 0xFFFF;
    while (!(I2C1->ISR & I2C_ISR_RXNE) && timeout--) {
        if(I2C1->ISR & I2C_ISR_NACKF){
            I2C1->ICR |= I2C_ICR_NACKCF;
            return 0;
        }
    }
    return (I2C1->RXDR & 0xFF);
}

void ds3231_init() {
    //!_____________GPIO init  PA9_SCL PA10_SDA ____________
    RCC->IOPENR |= RCC_IOPENR_GPIOAEN;
    GPIOA->MODER &=~ (GPIO_MODER_MODE9 | GPIO_MODER_MODE10);
    GPIOA->MODER |= (GPIO_MODER_MODE9_1 | GPIO_MODER_MODE10_1); //1:0 alternative function
    GPIOA->OTYPER |= (GPIO_OTYPER_OT9 | GPIO_OTYPER_OT10); // 1 - open drain
    GPIOA->PUPDR |= (GPIO_PUPDR_PUPD9_0 | GPIO_PUPDR_PUPD10_0); // 0:1 - pull-up
    GPIOA->OSPEEDR |= (GPIO_OSPEEDR_OSPEED9 | GPIO_OSPEEDR_OSPEED10); //1:1 max speed
    GPIOA->AFR[1] |= ((6<<4) | (6<<8)); // AF6 I2C

    // I2C init    
    RCC->APBENR1 |= RCC_APBENR1_I2C1EN;
    RCC->CCIPR &=~ RCC_CCIPR_I2C1SEL;
    RCC->CCIPR |= RCC_CCIPR_I2C1SEL_1; // 1:0 HSI16   16 MHz
    I2C1->CR2 &=~ I2C_CR2_NACK; //ACK in answer
    // Clock = 16MHz, 0.0625 us, need 5us (*80)
    I2C1->TIMINGR = (0<<28) | (3<<20) | (0<<16) | (7<<8) | (25); // 0x00300719 - from excel calculator
    I2C1->CR1 = I2C_CR1_PE;

    // Set the DS3231's I2C address.
    I2C1->CR2 &= ~(I2C_CR2_SADD);
    I2C1->CR2 |=  (0xD0 << I2C_CR2_SADD_Pos);
}

uint8_t write_i2c_reg(uint8_t reg, uint8_t data){
    // Set 'write' I2C direction again.
    uint8_t temp = 0;
    I2C1->CR2 &= ~(I2C_CR2_RD_WRN);
    uint32_t timeout = 0xFFFF;
    while((I2C1->ISR & I2C_ISR_BUSY) && timeout--) ;  // check busy
    timeout = 0xFFFF;
    // Send 'Start' condition, and wait for acknowledge.
    I2C1->CR2 &= ~(I2C_CR2_NBYTES);
    I2C1->CR2 |= 2<<I2C_CR2_NBYTES_Pos;  // 1 byte reg + 1 byte data
    i2c_start();
    temp = i2c_write_byte(reg);
    temp = i2c_write_byte(data);
    i2c_stop();
    return temp;
}

uint8_t read_i2c_regs(uint8_t start_reg, uint32_t *data, uint8_t nbytes){
    uint8_t result = 0;
    uint8_t* data_ptr = (uint8_t*)data;
    uint32_t timeout = 0xFFFF;
    // Set 'write' I2C direction again.
    I2C1->CR2 &= ~(I2C_CR2_RD_WRN);
    while((I2C1->ISR & I2C_ISR_BUSY) && timeout--) ;  // check busy
    // Set '1 byte to send.'
    I2C1->CR2 &= ~(I2C_CR2_NBYTES);
    I2C1->CR2 |=  (1 << I2C_CR2_NBYTES_Pos);
    // Start the I2C write transmission.
    i2c_start();
    // Send the register address.
    i2c_write_byte(start_reg);
    // Stop the I2C write transmission.
    i2c_stop();
    // Set 'read' I2C direction.
    I2C1->CR2 |=  (I2C_CR2_RD_WRN);
    // Set '1 byte to send.'
    I2C1->CR2 &= ~(I2C_CR2_NBYTES);
    I2C1->CR2 |=  (nbytes << I2C_CR2_NBYTES_Pos);
    // Start the I2C read transmission.
    i2c_start();
    uint8_t i;
    for(i = 0; i < nbytes; ++i) {
        timeout = 0xFFFF;
        while(!(I2C1->ISR & I2C_ISR_RXNE) && timeout--) { // wait for data
            if(I2C1->ISR & I2C_ISR_NACKF){
                I2C1->ICR |= I2C_ICR_NACKCF;
                return 0;
            }
        }
        result = I2C1->RXDR;
        data_ptr[i] = result;
    }
    i2c_stop();
    // Set 'write' I2C direction again.
    I2C1->CR2 &= ~(I2C_CR2_RD_WRN);
    return 1;
}