#include "sd_spi.h"

SD* SD::pThis = nullptr;

SD::SD() {
    pThis = this;
    init();
}

uint8_t SD::sd_init()
{
    spiMinSpeed();
    uint8_t ty = 0;
    uint8_t ocr[4] = {0};
    uint32_t tmr=0;
    cs_set();
    cs_idle();
      
    for (uint32_t i = 0; i < 10000000; i++); //delay wait ~ 1ms
    for (uint8_t i = 0; i < 12; i++) {
        send_byte(0xFF); /* 80 Dummy clocks with CS=H */
    }
    
    // for(uint32_t i=0;i<600000;i++);//delay
    cs_set();
    uint8_t respCMD8=0;
    while(respCMD8 !=1){
        respCMD8 = send_cmd(CMD0, 0);
        //for(int i=0; i<10000;i++);
    } 
    respCMD8 = 0;  
    // while(respCMD8 !=1){
    //     respCMD8 = send_cmd(CMD8, 0x1AA);//0x01AA);
    //     //for(volatile int i=0; i<10000;i++);
    // } 
    if( send_cmd(CMD8, 0x1AA) == 1) { /* SDv2 */
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
    send_byte(cmd);
    send_byte((uint8_t)(arg >> 24));
    send_byte((uint8_t)(arg >> 16));
    send_byte((uint8_t)(arg >> 8));
    send_byte((uint8_t)arg);
    n = 0x01;							/* Dummy CRC + Stop */
	if (cmd == CMD0) n = 0x95;			/* Valid CRC for CMD0(0) */
	if (cmd == CMD8) n = 0x87;			/* Valid CRC for CMD8(0x1AA) */
    send_byte(n);
    n=10;
    do {
        res = read_byte(0xFF);
    } while ((res & 0x80) && --n);
    cs_idle();    
    return res;
}

void SD::send_byte(uint8_t byte) {
    //uint32_t timeout = 0xFFF;
    while (!(SPI2->SR & SPI_SR_TXE));
    //while (!(SPI2->SR & SPI_SR_TXE) && timeout--);
    //timeout = 0xFFF;
    SPI2->DR = byte;    
    //while (!(SPI2->SR & SPI_SR_RXNE));
    while ((SPI2->SR & SPI_SR_BSY));
    //while ((SPI2->SR & SPI_SR_BSY) && timeout--);
}

uint8_t SD::read_byte(uint8_t byte) {
    cs_set();
    //uint8_t temp = 0;
    //uint32_t timeout = 0xFFF;
    //while (!(SPI2->SR & SPI_SR_TXE));
    SPI2->DR = byte; //exchange start    
    while (!(SPI2->SR & SPI_SR_RXNE));
    //while (!(SPI2->SR & SPI_SR_RXNE) && timeout--); //wait for new value in receiver buffer
    //for(int i=0; i<1000;i++);
    return SPI2->DR;
    cs_idle();
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
    SPI2->CR1 |= (7<<3);// SPI_CR1_BR; // (000=>psc=2) 1:1:1  => 42000000/256 = 164,0625 kHz - SPI-2 clk
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
    //SPI2->CR1 |= SPI_CR1_CPOL;      //1: CK to 1 when idle (in datasheet slave)
    //SPI2->CR1 |= SPI_CR1_CPHA;     //1: 2 clock transition is the 1 data capture edge (in slave datasheet)
    SPI2->CR1 &=~ SPI_CR1_CPHA;     //1: 2 clock transition is the 1 data capture edge (in slave datasheet)

    //NVIC_EnableIRQ(SPI1_IRQn);
    //----------- turn on SPI-2 --------------------------------------------
    SPI2->CR1 |= SPI_CR1_SPE;
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

	if (!(CardType & SD_BLOCK)) sector *= 512;	/* Convert to byte address if needed */
	res = RES_ERROR;
	if (send_cmd(CMD17, sector) == 0) {		/* READ_SINGLE_BLOCK */
		bc = 40000;
		do {							/* Wait for data packet */
			rc = read_byte(0xFF);
		} while (rc == 0xFF && --bc);
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
			send_byte(*buff++);
			wc--; bc--;
		}
		res = RES_OK;
	} else {
		if (sc) {	/* Initiate sector write process */
			if (!(CardType & SD_BLOCK)) sc *= 512;	/* Convert to byte address if needed */
			if (send_cmd(CMD24, sc) == 0) {			/* WRITE_SINGLE_BLOCK */
				send_byte(0xFF); send_byte(0xFE);		/* Data block header */
				wc = 512;							/* Set byte counter */
				res = RES_OK;
			}
		} else {	/* Finalize sector write process */
			bc = wc + 2;
			while (bc--) send_byte(0);	/* Fill left bytes and CRC with zeros */
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
        uint8_t Color[20] = {0};
        res=pf_mount(&fs);		//Монтируем FAT
        res=pf_open("Zastavka.bmp"); 
        res=pf_read(Color, 10, (UINT*)&bytes_readed);
        Color[10]=0;
        pf_mount(0x00); //демонтируем фат
    }
    return res;
}