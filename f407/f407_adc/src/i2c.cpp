#include "i2c.h"

At24c* At24c::pThis = nullptr;

At24c::At24c() {
	at24c_init();
	pThis = this;
}

void At24c::writeBytes(uint16_t addr, uint8_t* buf, uint16_t size) {   
    uint32_t timeout = Timeout_i2c;    
    I2C1->CR1|=I2C_CR1_ACK;
    I2C1->CR1|=I2C_CR1_START; 
    while (!(I2C1->SR1 & I2C_SR1_SB)) {
       if(!--timeout)return;
    }        
    (void) I2C1->SR1; 
    I2C1->DR=slaveWrite; 
    timeout = Timeout_i2c;
    while (!(I2C1->SR1 & I2C_SR1_ADDR)) {	
        if(!--timeout)return;
    }
    (void) I2C1->SR1;
    (void) I2C1->SR2;        
    I2C1->DR=addr>>8; //send HIGH byte
    timeout = Timeout_i2c;
    while(!(I2C1->SR1 & I2C_SR1_TXE)) {
        if(!--timeout)return;
    }
    I2C1->DR=addr&0xFF; //send LOW byte
    while(!(I2C1->SR1 & I2C_SR1_TXE)){
        if(!--timeout)return;
    }
    for(volatile uint8_t i=0;i<size;i++) {
        timeout = Timeout_i2c;
        I2C1->DR=buf[i]; //write array of bytes        
        while (!(I2C1->SR1 & I2C_SR1_BTF)) {
            if(!--timeout) return;
        }           
    }
    I2C1->CR1|=I2C_CR1_STOP;              
}
void At24c::readBytes(uint16_t addr, void* buf,uint16_t size) {        
    uint32_t timeout = Timeout_i2c;
    I2C1->CR1|=I2C_CR1_ACK;
    I2C1->CR1|=I2C_CR1_START; 
    while (!(I2C1->SR1 & I2C_SR1_SB)){ 
        if(!--timeout) return;
    }        
	(void) I2C1->SR1;
    I2C1->DR = slaveWrite; 
    timeout = Timeout_i2c;
    while (!(I2C1->SR1 & I2C_SR1_ADDR)){
        if(!--timeout)return;
    }
    (void) I2C1->SR1;    	
	(void) I2C1->SR2;        
    I2C1->DR=addr>>8; //send HIGH byte
    while(!(I2C1->SR1&I2C_SR1_TXE)){
        if(!--timeout)return;
    }
    I2C1->DR=addr & 0xFF; //send LOW byte
    timeout = Timeout_i2c;
    while(!(I2C1->SR1&I2C_SR1_TXE)) {
        if(!--timeout)return;
    }
    I2C1->CR1 |= I2C_CR1_START; //restart
    timeout = Timeout_i2c;
    while (!(I2C1->SR1 & I2C_SR1_SB)){
        if(!--timeout)return;
    }
    I2C1->DR = slaveRead; // 
    timeout = Timeout_i2c;
    while (!(I2C1->SR1 & I2C_SR1_ADDR)) {
        if(!--timeout)return;
    }
    (void) I2C1->SR1;
    (void) I2C1->SR2;
    for(volatile uint8_t i=0;i < size;i++) {
        if(i < size-1) {
            I2C1->CR1|=I2C_CR1_ACK;
            timeout = Timeout_i2c;
            while (!(I2C1->SR1&I2C_SR1_RXNE)){
               if(!--timeout)return;
            }
            *((uint8_t*)buf+i) = I2C1->DR;                       
        }
        else if(i == size-1) {
            I2C1->CR1&=~I2C_CR1_ACK; //no acknowlege!!!;
            timeout = Timeout_i2c;
            while (!(I2C1->SR1&I2C_SR1_RXNE)){
                if(!--timeout)return;
            }
            *((uint8_t*)buf+i) = I2C1->DR;
        }           
    }    
    I2C1->CR1|=I2C_CR1_STOP;              
}
void At24c::writeByte(uint16_t address,uint8_t byte) {
    uint32_t timeout = Timeout_i2c;    
    I2C1->CR1|=I2C_CR1_ACK;
    I2C1->CR1|=I2C_CR1_START; 
    while (!(I2C1->SR1 & I2C_SR1_SB)) {            
        if(!--timeout) return;
    }
    (void) I2C1->SR1; 
    I2C1->DR = slaveWrite; 
    timeout = Timeout_i2c;
    while (!(I2C1->SR1 & I2C_SR1_ADDR)) {	
        if(!--timeout)return;
    }    
    (void) I2C1->SR1;
    (void) I2C1->SR2;        //clear ADDR flag
    I2C1->DR=address>>8; 
    timeout = Timeout_i2c;
    while(!(I2C1->SR1&I2C_SR1_TXE)) { 
        if(!--timeout) return;
    }
    I2C1->DR=address&0xFF; 
    timeout = Timeout_i2c;
    while(!(I2C1->SR1&I2C_SR1_TXE)) { 
        if(!--timeout) return;
    }
    I2C1->DR=byte; //send byte
    timeout = Timeout_i2c;
    while (!(I2C1->SR1 & I2C_SR1_BTF)){ 
        if(!--timeout) return;
    }
    I2C1->CR1|=I2C_CR1_STOP;  
}
uint8_t At24c::readByte(uint16_t address) {
    uint32_t timeout = Timeout_i2c;
    uint8_t x=0;
    I2C1->CR1|=I2C_CR1_ACK;
    I2C1->CR1|=I2C_CR1_START;     
    while (!(I2C1->SR1 & I2C_SR1_SB)) { //wait for transmittion  
        if(--timeout)return 0;
    }    
	(void) I2C1->SR1; // clear
    I2C1->DR = slaveWrite; // first byte with address and write bit         
    timeout = Timeout_i2c;
    while (!(I2C1->SR1 & I2C_SR1_ADDR)){ //wait for address transmittion	
        if(!--timeout)return 0;
    }
    (void) I2C1->SR1;    	
	  (void) I2C1->SR2;        
    I2C1->DR=address>>8; //send HIGH byte
    timeout = Timeout_i2c;
    while(!(I2C1->SR1&I2C_SR1_TXE)){ //wait for transmittion
        if(!--timeout)return 0;
    }
    I2C1->DR=address&0xFF; //send LOW byte
    timeout = Timeout_i2c;
    while(!(I2C1->SR1&I2C_SR1_TXE)){ //wait for transmittion         
        if(!--timeout)return 0;
    }
    I2C1->CR1 |= I2C_CR1_START; //restart
    timeout = Timeout_i2c;
    while (!(I2C1->SR1 & I2C_SR1_SB)) { 
        if(!--timeout)return 0;
    }    
    I2C1->DR = slaveRead; // first byte with address and read bit
    timeout = Timeout_i2c;
    while (!(I2C1->SR1 & I2C_SR1_ADDR)){ //wait while transmittion        
        if(!--timeout)return 0;
    }
        (void) I2C1->SR1;
    (void) I2C1->SR2;
    I2C1->CR1&=~I2C_CR1_ACK; //no acknowlege!!!;
    timeout = Timeout_i2c;
    while (!(I2C1->SR1&I2C_SR1_RXNE)){
        if(!--timeout)return 0;
    }
    x = I2C1->DR; //read byte        
    I2C1->CR1|=I2C_CR1_STOP;  //stop signal
    return x;
}

/////////////////////////////////////////////////////////////
void At24c::at24c_init(void) {
	for(int i=0; i<32;i++) {
		At24c_arr[i] = 0;
	}
    //GPIOS init PB8-SCL PB9-SDA  ALT func 1:0 OPE-DRAIN MAX SPEED
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    GPIOB->MODER |= (GPIO_MODER_MODE8_1|GPIO_MODER_MODE9_1);
    GPIOB->MODER &=~ (GPIO_MODER_MODE8_0|GPIO_MODER_MODE9_0);
    GPIOB->OTYPER |= (GPIO_OTYPER_OT8|GPIO_OTYPER_OT9);
    GPIOB->OSPEEDR |= (GPIO_OSPEEDR_OSPEED8 | GPIO_OSPEEDR_OSPEED9);
		GPIOB->PUPDR |= (GPIO_PUPDR_PUPDR8_0|GPIO_PUPDR_PUPDR9_0);
		GPIOB->PUPDR &=~ (GPIO_PUPDR_PUPDR8_1|GPIO_PUPDR_PUPDR9_1);
		//GPIOB->BSRR |= (GPIO_BSRR_BS8|GPIO_BSRR_BS9);
    //GPIOB->ODR |= (GPIO_ODR_OD8|GPIO_ODR_OD9);
		GPIOB->AFR[1] = ((4<<0)|(4<<4));
		// PE0 to GND
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;
    GPIOE->MODER |= (GPIO_MODER_MODE0_0);
    GPIOE->MODER &=~ (GPIO_MODER_MODE0_1);
		GPIOE->ODR &=~ GPIO_ODR_OD0;
    // I2C1 init    
    I2C1->CR1 |= I2C_CR1_SWRST;
    I2C1->CR1 &=~ I2C_CR1_SWRST;
	  I2C1->CR1 &=~ I2C_CR1_PE; 
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
    /*
    //I2C1->CR1 |= I2C_CR1_ACK; //ACK in answer
    I2C1->CR2&=~I2C_CR2_FREQ; 
    I2C1->CR2|=24; //  Peripheral clock frequency  
    // TPCLK1 = 42ns
    I2C1->CCR&=~I2C_CCR_CCR; //reset
    I2C1->CCR|=I2C_CCR_FS; //0: Sm mode I2C     1: Fm mode I2C fast mode
    I2C1->CCR&=~I2C_CCR_DUTY; // 0: Fm mode tlow/thigh = 2 1: Fm mode tlow/thigh = 16/9 (see CCR)
    //FM (DUTY=1 to reach 400kHz): THIGH=9*CCR*TPCLK1; TLOW=16*CCR*TPCLK1; THIGH+TLOW=2500ns =>
    // THIGH+TLOW=1/400kHz => THIGH+TLOW=2.5us=2500ns
    // CCR*(TPCLK1*(9+16))=2500ns => CCR=2500ns/(25*TPCLK1)=100/42=2
    // CCR=2               (if DUTY=0 =>CCR=17)
    I2C1->CCR|=2;
    //I2C1->CR2|=I2C_CR2_ITEVTEN; // interrupts no need
    //TRISE: Tr=300ns (max rise time) => TRISE=(Tr/TPCLK1)+1=8;
    I2C1->TRISE=6; //max time need some less
    //NVIC_EnableIRQ(I2C1_EV_IRQn);
    //NVIC_SetPriority(I2C1_EV_IRQn, 1);
    */
    I2C1->CR1|=I2C_CR1_ACK; //ACK ? ??????       ???????????? ??????? ACK ????? ?????? ??????
        I2C1->CR2&=~I2C_CR2_FREQ; // ??????????;
        I2C1->CR2|=24; //  Peripheral clock frequency - ????????????? ??????? 
        //I2C1->CR2|=I2C_CR2_ITEVTEN; // ???????? ?????????? ?? ??????? 
        // TPCLK1 = 42ns
        I2C1->CCR&=~I2C_CCR_CCR; //???????? CCR - ???????? ???????? ???????
        I2C1->CCR&=~I2C_CCR_FS; //0: Sm mode I2C     1: Fm mode I2C
        I2C1->CCR&=~I2C_CCR_DUTY; // only for fast mode 0: Fm mode tlow/thigh = 2 1: Fm mode tlow/thigh = 16/9 (see CCR)
        //SM: THIGH=CCR*TPCLK1=TLOW ::: THIGH+TLOW=10000ns =>
        //?.?. THIGH+TLOW=1/100kHz => THIGH+TLOW=10us=10000ns
        // CCR*(2*TPCLK1)=10000ns  => CCR=10000ns/(2*42ns) =>
        // CCR=120
        I2C1->CCR|=120;
        //TRISE: Tr=1000ns (max rise time) => TRISE=(Tr/TPCLK1)+1=24;
        I2C1->TRISE=22;  //???????????? ????? => ??????? ??????? ???????
    I2C1->CR1|=I2C_CR1_PE;     
}