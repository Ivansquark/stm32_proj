#include "sd_spi.h"

SD* SD::pThis = nullptr;

SD::SD() {
    pThis = this;
    init();
}

void SD::send_byte(uint8_t byte) {
    uint32_t timeout = 0xFFF;
    //while (!(SPI2->SR & SPI_SR_TXE));
    while (!(SPI2->SR & SPI_SR_TXE) && timeout--);
    timeout = 0xFFF;
    SPI2->DR = byte;    
    while (!(SPI2->SR & SPI_SR_RXNE) && timeout--);
    timeout = 0xFFF;
    while ((SPI2->SR & SPI_SR_BSY) && timeout--);
}

uint8_t SD::read_byte(uint8_t byte) {
    //cs_set();
    //uint8_t temp = 0;
    //uint32_t timeout = 0xFFF;
    //while (!(SPI2->SR & SPI_SR_TXE));
    SPI2->DR = byte; //exchange start    
    while (!(SPI2->SR & SPI_SR_RXNE));
    //while (!(SPI2->SR & SPI_SR_RXNE) && timeout--); //wait for new value in receiver buffer
    //for(int i=0; i<1000;i++);
    return SPI2->DR;
    //cs_idle();
}

void SD::init() {
    sd_spi_init();
}

void SD::sd_spi_init() {
    //------- SPI2 E6-NSS B10-SCK C2-MISO C3-MOSI ------------
    RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN | RCC_AHB1ENR_GPIOEEN);
    //--------PE6 - programm NSS push-pull output-----------
    GPIOE->MODER |= GPIO_MODER_MODER6_0;  //
    GPIOE->MODER &= ~GPIO_MODER_MODER6_1; // 0:1 output push-pull
    GPIOE->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR5;
    //-------B10-SCK C2-MISO C3-MOSI - alt func SPI2 --------------------
    GPIOC->MODER |= (GPIO_MODER_MODER2_1 | GPIO_MODER_MODER3_1);
    GPIOC->MODER &= ~(GPIO_MODER_MODER3_0 | GPIO_MODER_MODER3_0);
    //GPIOC->PUPDR |= GPIO_PUPDR_PUPDR3_0; // pull-up MOSI
    GPIOC->PUPDR |= GPIO_PUPDR_PUPDR2_0; // pull-up MISO
    GPIOB->MODER |= (GPIO_MODER_MODER10_1);
    GPIOB->MODER &= ~(GPIO_MODER_MODER10_0);
    //GPIOB->PUPDR |= GPIO_PUPDR_PUPDR10_0;
    GPIOC->AFR[0] |= (5 << 8) | (5 << 12); //alt func 5 (SPI2)
    GPIOB->AFR[1] |= (5 << 8); //alt func 5 (SPI2)
    GPIOC->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR13 | GPIO_OSPEEDER_OSPEEDR13) ;          //
    GPIOB->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR10;          //max speed on pins

    //-------------  SPI-2  ---------------------------
    RCC->APB1ENR |= RCC_APB1ENR_SPI2EN; //clock on fast SPI-1
    //------------- prescaler SPI-2 -----------------------------
    SPI2->CR1 &=~ (7<<3);
    SPI2->CR1 |= (0<<3);// SPI_CR1_BR; // (000=>psc=2) 1:1:1  => 42000000/256 = 164,0625 kHz - SPI-2 clk
    //------------- SPI2 settings -----------------------------------------
    SPI2->CR1 &= ~SPI_CR1_BIDIMODE; // two wires
    SPI2->CR1 &= ~SPI_CR1_BIDIOE;    //1: Output enabled (transmit-only mode)
    SPI2->CR1 &= ~SPI_CR1_CRCEN;    // 0: CRC calculation disabled
    SPI2->CR1 &= ~SPI_CR1_CRCNEXT;  //0: Data phase (no CRC phase)
    SPI2->CR1 &= ~SPI_CR1_DFF;      //0: 8-bit data frame format is selected for transmission/reception
    SPI2->CR1 &= ~SPI_CR1_RXONLY;   //0: Full duplex (Transmit and receive)
    SPI2->CR1 |= SPI_CR1_SSM;       // 1: Software slave management enabled (programm CS)
    SPI2->CR1 |= SPI_CR1_SSI;       // 1: Software slave management enabled
    SPI2->CR1 &= ~SPI_CR1_LSBFIRST; //1: LSB first //0: MSB transmitted first
    SPI2->CR1 |= SPI_CR1_MSTR;      //1: Master configuration
    
    SPI2->CR1 &=~ SPI_CR1_CPOL;      //1: CK to 1 when idle (in datasheet slave)
    SPI2->CR1 &=~ SPI_CR1_CPHA;     //1: 2 clock transition is the 1 data capture edge (in slave datasheet)
    
    //SPI2->CR1 |= SPI_CR1_CPOL;      //1: CK to 1 when idle (in datasheet slave)
    //SPI2->CR1 |= SPI_CR1_CPHA;     //1: 2 clock transition is the 1 data capture edge (in slave datasheet)
    

    //NVIC_EnableIRQ(SPI1_IRQn);
    //----------- turn on SPI-2 --------------------------------------------
    SPI2->CR1 |= SPI_CR1_SPE;
}
