#include "sd_spi.h"

SD* SD::pThis = nullptr;

SD::SD() {
    pThis = this;
    init();
    //sd_init();
}

uint8_t SD::sd_init()
{
    spiMinSpeed();
    uint8_t ty = 0;
    uint8_t ocr[4] = {0};
    uint32_t tmr=0;
    cs_set();
    cs_idle();
      
    for (uint32_t i = 0; i < 1000000; i++); //delay wait ~ 1ms
    for (uint8_t i = 0; i < 14; i++) {
        read_byte(0xFF); /* 80 Dummy clocks with CS=H */
        //for(volatile int i = 0; i<500; i++);
    }
    
    // for(uint32_t i=0;i<600000;i++);//delay
    cs_set();
    uint8_t respCMD8=0;
    uint16_t counts = 0xFFF;
    while(respCMD8 !=1 && counts--){
        respCMD8 = send_cmd(CMD0, 0);
        for(int i=0; i<10000;i++);
    } 
    respCMD8 = 0;  
    while(respCMD8 !=1){
        respCMD8 = send_cmd(CMD8, 0x1AA);//0x01AA);
        //for(volatile int i=0; i<10000;i++);
    } 
    if( respCMD8 == 1) { /* SDv2 */
        cs_set();
        for (int i = 0; i < 4; i++) {
            ocr[i] = read_byte(0xFF);	/* Get trailing return value of R7 resp */
        }
        if (ocr[2] == 0x01 && ocr[3] == 0xAA) {	
            for (tmr = 10000; tmr && send_cmd(ACMD41, 1UL << 30); tmr--) {
                for(uint32_t i=0; i<10000;i++);
            }            
            uint8_t temp1 = send_cmd(CMD58, 0);
			if (tmr && temp1 == 0) {		/* Check CCS bit in the OCR */
                cs_set();
				for (int n = 0; n < 4; n++) ocr[n] = read_byte(0xFF);
				ty = (ocr[0] & 0x40) ? SD_SD2 | SD_BLOCK : SD_SD2;	/* SDv2 (HC or SC) */
			}
		}
    } else { /* SDv1 or MMCv3 */
        if (send_cmd(ACMD41, 0) <= 1) 	{
			ty = SD_SD1;	/* SDv1 */
            for (tmr = 0xFFFF; tmr && send_cmd(ACMD41, 0); tmr--) ; 
		} else {
			ty = SD_MMC;	/* MMCv3 */
            for (tmr = 0xFFFF; tmr && send_cmd(CMD1, 0); tmr--) ; 
		}
        if (!tmr || send_cmd(CMD16, 512) != 0) // Set R/W block length to 512
            ty = 0;
    }
    CardType = ty;
    spiMaxSpeed();
    return ty ? 0 : 1; //1 == STA_NOINIT
}


uint8_t SD::send_cmd(uint8_t cmd, uint32_t arg) {
    
    uint8_t res = 0;
    uint32_t n = 0;
    if (cmd & 0x80) {	/* ACMD<n> is the command sequense of CMD55-CMD<n> */
		cmd &= 0x7F;
		res = send_cmd(CMD55, 0);
		if (res > 1) return res;
	}
    cs_idle();    
    read_byte(0xFF);  
    cs_set();  
    read_byte(0xFF);    
    read_byte(cmd);
    read_byte((uint8_t)(arg >> 24));
    read_byte((uint8_t)(arg >> 16));
    read_byte((uint8_t)(arg >> 8));
    read_byte((uint8_t)arg);
    n = 0x01;							/* Dummy CRC + Stop */
	if (cmd == CMD0) n = 0x95;			/* Valid CRC for CMD0(0) */
	if (cmd == CMD8) n = 0x87;			/* Valid CRC for CMD8(0x1AA) */
    read_byte(n);
    n=10;
    do {
        res = read_byte(0xFF);
    } while ((res & 0x80) && --n);
    cs_idle();    
    return res;
}

void SD::send_byte(uint8_t byte) {
    uint32_t timeout = 0xFFFF;
    // старт передаем неопределенное кол-во байт    
    SPI4->CR1 &= ~SPI_CR1_SPE;
    SPI4->CR2 = 0;
    SPI4->CR1 |= SPI_CR1_SPE;    
    SPI4->CR1 |= SPI_CR1_CSTART;            //exchange start   
    while (!(SPI4->SR & SPI_SR_TXP) && timeout--); // 1: TxFIFO has enough free location to host 1 data packet
    * (volatile uint8_t *) & (SPI4)->TXDR = byte;    // prevent data packing feature  
    //SPI3->TXDR = byte;     
    timeout = 0xFFFF;
    while (!(SPI4->SR & SPI_SR_TXC) && timeout--);       //1: transfer complete    
}

uint8_t SD::read_byte(uint8_t byte) {
    uint32_t timeout = 0xFFFF;
    //cs_set();
    //! send dummy byte to slave for read byte from it    
    //SPI3->CR1 &= ~SPI_CR1_SPE;
    //SPI3->CR2 = 0;
    //SPI3->CR1 |= SPI_CR1_SPE;
    SPI4->CR1 |= SPI_CR1_CSTART;            //exchange start 
    while ( !( SPI4 -> SR & SPI_SR_TXP ) && timeout--);
    * (volatile uint8_t *) & (SPI4)->TXDR = byte;    // prevent data packing feature  
    //SPI3->TXDR = byte;
    //while (!(SPI3->SR & SPI_SR_TXC));       //1: Last TxFIFO or CRC frame transmission completed 
    //while(!(SPI3->SR & SPI_SR_RXP));
    timeout = 0xFFFF;
    while(!(SPI4->SR & SPI_SR_RXP) && timeout--);  
    //while (!(SPI3->SR & SPI_SR_TXC));       //1: Last TxFIFO or CRC frame transmission completed
    //cs_idle();
    return SPI4->RXDR;
}

void SD::init() {
    sd_spi_init();
    sd_init();
}

void SD::sd_spi_init() {
    //------- SPI4 PE3-NSS PE2-SCK PE5-MISO PE6-MOSI ------------
    //RCC->AHB4ENR |= (RCC_AHB4ENR_GPIOCEN);
    RCC->AHB4ENR |= (RCC_AHB4ENR_GPIOEEN);
    //--------PE3 - programm NSS push-pull output-----------
    GPIOE->MODER |= GPIO_MODER_MODE3_0;  //
    GPIOE->MODER &= ~GPIO_MODER_MODE3_1; // 0:1 output push-pull
    GPIOE->OSPEEDR |= GPIO_OSPEEDR_OSPEED3;
    //------- PE2 -SCK PE5-MISO PE6-MOSI - alt func SPI4 --------------------
    GPIOE->MODER |= (GPIO_MODER_MODE2_1 | GPIO_MODER_MODE5_1 |  GPIO_MODER_MODE6_1);
    GPIOE->MODER &= ~(GPIO_MODER_MODE2_0 | GPIO_MODER_MODE5_0 | GPIO_MODER_MODE6_0); // alt func    
    //GPIOC->PUPDR |= GPIO_PUPDR_PUPDR3_0; // pull-up MOSI
    //GPIOC->PUPDR |= GPIO_PUPDR_PUPD11_0; // pull-up MISO
    //GPIOB->PUPDR |= GPIO_PUPDR_PUPDR10_0;
    GPIOE->AFR[0] |= (5 << 8) | (5 << 20) | (5 << 24); //alt func 5 (SPI4)
    GPIOE->OSPEEDR |= (GPIO_OSPEEDR_OSPEED2 | GPIO_OSPEEDR_OSPEED6);//max speed on pins

    //-------------  SPI-4  ---------------------------
    // SPDIFSEL[1:0] 00: pll1_q_ck clock selected as SPDIFRX kernel clock (default after reset)
    RCC->D2CCIP1R &=~ RCC_D2CCIP1R_SPI45SEL;    //000: APB clock is selected as kernel clock (default after reset)
    //RCC->D2CCIP1R |= RCC_D2CCIP1R_SPI123SEL_2;
    RCC->APB2ENR |= RCC_APB2ENR_SPI4EN;   //clock on fast SPI-4
    //------------- prescaler SPI-4 no more 25 MHz ------------------------
    SPI4->CFG1 |= SPI_CFG1_MBR;             // 1:1:1 SPI master clock/256
    //SPI4->CFG1 &= ~SPI_CFG1_MBR;          // 0:0:0 SPI master clock/2
    //------------- SPI3 settings -----------------------------------------
    //SPI4->CR2 |= 1;                         // 1 - transfer size (transfer by one byte)
    SPI4->CR1 = 0;              
    SPI4->CR2 = 0;                          // 1 - transfer size (transfer by one byte)
    //SPI4->CFG1 |= SPI_CFG1_FTHLV_0;        // 0:0:0:1: Threashold level 
    SPI4->CFG1 &= ~SPI_CFG1_CRCEN;          // 0: CRC calculation disabled
    //SPI4->CFG1 |= 7;                      // 1:1:1 - DSIZE[4:0] - 8bits (reset value)
    
    // SS signal input by software (SSM=1, SSI=1) when SS output mode is enabled (SSOE=1) at master mode.
    
    SPI4->CR1   |=  SPI_CR1_SSI;               // 1: Software slave management enabled
    SPI4->CFG2  |=  SPI_CFG2_SSM; 
    SPI4->CFG2  |=  SPI_CFG2_MASTER;          //1: Master configuration 
    SPI4->CFG2  |=  SPI_CFG2_AFCNTR;          //<Alternate function GPIOs control 
    SPI4->CFG2  |=  SPI_CFG2_SSOE;            //SS output enable     
    SPI4->CFG2  &= ~SPI_CFG2_LSBFRST;        //1: LSB first //0: MSB transmitted first
    SPI4->CFG2  &= ~SPI_CFG2_COMM;           // 0:0 - full duplex (transmit and receive)   
    SPI4->CFG2  &=~ SPI_CFG2_CPOL;           //1: CK to 1 when idle (in datasheet slave)
    //SPI4->CFG2 |= SPI_CFG2_CPOL;          //1: CK to 1 when idle (in datasheet slave)
    //SPI4->CFG2 |= SPI_CFG2_CPHA;          //1: 2 clock transition is the 1 data capture edge (in slave datasheet)
    SPI4->CFG2 &=~ SPI_CFG2_CPHA;           //1: 2 clock transition is the 1 data capture edge (in slave datasheet)

    //NVIC_EnableIRQ(SPI4_IRQn);
    //----------- turn on SPI-2 --------------------------------------------
    SPI4->CR1 |= SPI_CR1_SPE;
    //SPI4->CR1 |= SPI_CR1_CSTART;  
    //SPI3->CR1 |= SPI_CR1_CSTART;            //exchange start  
    //while((SPI4->SR & SPI_SR_TXP) == 0){__NOP();};
    //SPI4->TXDR = 0x55;
    
    //while((SPI4->SR & SPI_SR_TXC) == 0){__NOP();};
    //while((SPI4->SR & SPI_SR_RXP) == 0){__NOP();};    
}

/*-----------------------------------------------------------------------*/
/* Read partial sector                                                   */
/*-----------------------------------------------------------------------*/

SD::L_DRESULT SD::read_sector (
	BYTE *buff,		/* Pointer to the read buffer (NULL:Read bytes are forwarded to the stream) */
	DWORD sector,	/* Sector number (LBA) */
	UINT offset,	/* Byte offset to read from (0..511) */
	UINT count		/* Number of bytes to read (ofs + cnt mus be <= 512) */
) {
	L_DRESULT res;
	BYTE rc;
	UINT bc;    
	if (!(CardType & SD_BLOCK)) {
        sector *= 512;	/* Convert to byte address if needed */
    }
	res = RES_ERROR;
	if (send_cmd(CMD17, sector) == 0) {		/* READ_SINGLE_BLOCK */
		bc = 40000;
        cs_set();
		do {							/* Wait for data packet */
			rc = read_byte(0xFF);
		} while (rc == 0xFF && --bc);
        // uint32_t cnt = 0;
        // do{ //Ждем начала блока
        //     rc=read_byte(0xFF);
        //     cnt++;
        // } while ( (rc!=0xFE)&&(cnt<0xFFFF) ); 
		if (rc == 0xFE) {				/* A data packet arrived */
			bc = 514 - offset - count;
			/* Skip leading bytes */
			if (offset) {
				do {
                    read_byte(0xFF);
                } while (--offset);
			}
			/* Receive a part of the sector */
			if (buff) {	/* Store data to the memory */
				do {
					*buff++ = read_byte(0xFF);;
				} while (--count);
			} else {	/* Forward data to the outgoing stream (depends on the project) */
				do {
//					FORWARD(rcv_spi());
				} while (--count);
			}
			/* Skip remaining bytes and CRC */
			do read_byte(0xFF); while (--bc);
			res = RES_OK;
		}
	}
	cs_idle();
	read_byte(0xFF);
	return res;
}

/*-----------------------------------------------------------------------*/
/* Write partial sector                                                  */
/*-----------------------------------------------------------------------*/
SD::L_DRESULT SD::write_sector (
	const BYTE *buff,	/* Pointer to the bytes to be written (NULL:Initiate/Finalize sector write) */
	DWORD sc			/* Number of bytes to send, Sector number (LBA) or zero */
)
{
	L_DRESULT res = RES_ERROR;
	UINT bc;
	static WORD wc;
	if (buff) {		/* Send data bytes */
		bc = (WORD)sc;
		while (bc && wc) {		/* Send data bytes to the card */
			read_byte(*buff++);
			wc--; bc--;
		}
		res = RES_OK;
	} else {
		if (sc) {	/* Initiate sector write process */
			if (!(CardType & SD_BLOCK)) sc *= 512;	/* Convert to byte address if needed */
			if (send_cmd(CMD24, sc) == 0) {			/* WRITE_SINGLE_BLOCK */
				read_byte(0xFF); read_byte(0xFE);		/* Data block header */
				wc = 512;							/* Set byte counter */
				res = RES_OK;
			}
		} else {	/* Finalize sector write process */
			bc = wc + 2;
			while (bc--) read_byte(0);	/* Fill left bytes and CRC with zeros */
			if ((read_byte(0XFF) & 0x1F) == 0x05) {	/* Receive data resp and wait for end of write process in timeout of 500ms */
				for (bc = 5000; read_byte(0XFF) != 0xFF && bc; bc--) {
                    for(volatile int i = 0; i<10000; i++);
                    //dly_100us();	/* Wait ready */
                } 
				if (bc) res = RES_OK;
			}
			cs_idle();
			read_byte(0XFF);
		}
	}
	return res;
}

uint8_t SD::fat_init() {
    if(sd_init() == 0) {
        uint32_t bytes_readed=0;
        uint8_t Text[20] = {0};
        res=pf_mount(&fs);		//Монтируем FAT
        res=pf_open("1.txt"); 
        res=pf_read(Text, 10, (UINT*)&bytes_readed);
        //Color[10]=0;
        pf_mount(0x00); //демонтируем фат
    }
    return res;
}