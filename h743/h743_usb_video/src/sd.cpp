#include "sd.h"

SD* SD::pThis = nullptr;

SD::SD() {
	pThis = this;
    init();
}

void SD::Cmd(uint8_t cmd, uint32_t arg, uint32_t resp_type) {
    // Clear command flags
	SDMMC2->ICR = (SDMMC_ICR_CCRCFAILC | SDMMC_ICR_CTIMEOUTC |
				   SDMMC_ICR_CMDRENDC | SDMMC_ICR_CMDSENTC); // Bitmap to clear the SDIO command flags
	// Program an argument for command
	SDMMC2->ARG = arg;
	// Program command value and response type, enable CPSM (command path state machine)
	SDMMC2->CMD = cmd | resp_type | SDMMC_CMD_CPSMEN;
}

SD::SDResult SD::GetR1Resp(uint8_t cmd) {
    volatile uint32_t wait = SD_CMD_TIMEOUT;
	uint32_t respR1;
	// Wait for response, error or timeout
	while (!(SDMMC2->STA & (SDMMC_STA_CCRCFAIL | SDMMC_STA_CMDREND | SDMMC_STA_CTIMEOUT)) && --wait);
	// Timeout?
	if ((SDMMC2->STA & SDMMC_STA_CTIMEOUT) && (wait == 0)) {
		SDMMC2->ICR = SDMMC_ICR_CTIMEOUTC;
		return SDR_Timeout;
	}
	// CRC fail?
	if (SDMMC2->STA & SDMMC_STA_CCRCFAIL) {
		SDMMC2->ICR = SDMMC_ICR_CCRCFAILC;
		return SDR_CRCError;
	}
	// Illegal command?
	if (SDMMC2->RESPCMD != cmd) {
		return SDR_IllegalCommand;
	}
	// Clear the static SDIO flags (ICR - interrupt clear register)
	SDMMC2->ICR = SDIO_ICR_STATIC;
	// Get a R1 response and analyze it for errors
	respR1 = SDMMC2->RESP1; //response Register
	if (!(respR1 & SD_OCR_ALL_ERRORS))      { return SDR_Success;          }
	if (respR1 & SD_OCR_OUT_OF_RANGE)       { return SDR_AddrOutOfRange;   }
	if (respR1 & SD_OCR_ADDRESS_ERROR)      { return SDR_AddrMisaligned;   }
	if (respR1 & SD_OCR_BLOCK_LEN_ERROR)    { return SDR_BlockLenError;    }
	if (respR1 & SD_OCR_ERASE_SEQ_ERROR)    { return SDR_EraseSeqError;    }
	if (respR1 & SD_OCR_ERASE_PARAM)        { return SDR_EraseParam;       }
	if (respR1 & SD_OCR_WP_VIOLATION)       { return SDR_WPViolation;      }
	if (respR1 & SD_OCR_LOCK_UNLOCK_FAILED) { return SDR_LockUnlockFailed; }
	if (respR1 & SD_OCR_COM_CRC_ERROR)      { return SDR_ComCRCError;      }
	if (respR1 & SD_OCR_ILLEGAL_COMMAND)    { return SDR_IllegalCommand;   }
	if (respR1 & SD_OCR_CARD_ECC_FAILED)    { return SDR_CardECCFailed;    }
	if (respR1 & SD_OCR_CC_ERROR)           { return SDR_CCError;          }
	if (respR1 & SD_OCR_ERROR)              { return SDR_GeneralError;     }
	if (respR1 & SD_OCR_STREAM_R_UNDERRUN)  { return SDR_StreamUnderrun;   }
	if (respR1 & SD_OCR_STREAM_W_OVERRUN)   { return SDR_StreamOverrun;    }
	if (respR1 & SD_OCR_CSD_OVERWRITE)      { return SDR_CSDOverwrite;     }
	if (respR1 & SD_OCR_WP_ERASE_SKIP)      { return SDR_WPEraseSkip;      }
	if (respR1 & SD_OCR_CARD_ECC_DISABLED)  { return SDR_ECCDisabled;      }
	if (respR1 & SD_OCR_ERASE_RESET)        { return SDR_EraseReset;       }
	if (respR1 & SD_OCR_AKE_SEQ_ERROR)      { return SDR_AKESeqError;      }
	return SDR_Success;
}

SD::SDResult SD::GetR2Resp(uint32_t *pBuf) {
	volatile uint32_t wait = SD_CMD_TIMEOUT;
	// Wait for response, error or timeout
	while (!(SDMMC2->STA & (SDMMC_STA_CCRCFAIL | SDMMC_STA_CMDREND | SDMMC_STA_CTIMEOUT)) && --wait);
	// Timeout?
	if ((SDMMC2->STA & SDMMC_STA_CTIMEOUT) && (wait == 0)) {
		SDMMC2->ICR = SDMMC_ICR_CTIMEOUTC;
		return SDR_Timeout;
	}
	// CRC fail?
	if (SDMMC2->STA & SDMMC_STA_CCRCFAIL) {
		SDMMC2->ICR = SDMMC_ICR_CCRCFAILC;
		return SDR_CRCError;
	}
	// Clear the static SDIO flags
	SDMMC2->ICR = SDIO_ICR_STATIC;
	// SDMMC_RESP[1..4] registers contains the R2 response
    // Use GCC built-in intrinsics (fastest, less code) (GCC v4.3 or later)
	*pBuf++ = __builtin_bswap32(SDMMC2->RESP1);
	*pBuf++ = __builtin_bswap32(SDMMC2->RESP2);
	*pBuf++ = __builtin_bswap32(SDMMC2->RESP3);
	*pBuf   = __builtin_bswap32(SDMMC2->RESP4);
	return SDR_Success;
}

SD::SDResult SD::GetR3Resp(void) {
	volatile uint32_t wait = SD_CMD_TIMEOUT;
	// Wait for response, error or timeout
	while (!(SDMMC2->STA & (SDMMC_STA_CCRCFAIL | SDMMC_STA_CMDREND | SDMMC_STA_CTIMEOUT)) && --wait);
	// Timeout?
	if ((SDMMC2->STA & SDMMC_STA_CTIMEOUT) && (wait == 0)) {
		SDMMC2->ICR = SDMMC_ICR_CTIMEOUTC;
		return SDR_Timeout;
	}
	// Clear the static SDIO flags
	SDMMC2->ICR = SDIO_ICR_STATIC;
	return SDR_Success;
}

SD::SDResult SD::GetR6Resp(uint8_t cmd, uint16_t *pRCA) {
	volatile uint32_t wait = SD_CMD_TIMEOUT;
	uint32_t respR6;
	// Wait for response, error or timeout
	while (!(SDMMC2->STA & (SDMMC_STA_CCRCFAIL | SDMMC_STA_CMDREND | SDMMC_STA_CTIMEOUT)) && --wait);
	// Timeout?
	if ((SDMMC2->STA & SDMMC_STA_CTIMEOUT) && (wait == 0)) {
		SDMMC2->ICR = SDMMC_ICR_CTIMEOUTC;
		return SDR_Timeout;
	}
	// CRC fail?
	if (SDMMC2->STA & SDMMC_STA_CCRCFAIL) {
		SDMMC2->ICR = SDMMC_ICR_CCRCFAILC;
		return SDR_CRCError;
	}
	// Illegal command?
	if (SDMMC2->RESPCMD != cmd) {
		return SDR_IllegalCommand;
	}
	// Clear the static SDIO flags
	SDMMC2->ICR = SDIO_ICR_STATIC;
	// Get a R6 response and analyze it for errors
	respR6 = SDMMC2->RESP1;
	if (!(respR6 & (SD_R6_ILLEGAL_CMD | SD_R6_GENERAL_UNKNOWN_ERROR | SD_R6_COM_CRC_FAILED))) {
		*pRCA = (uint16_t)(respR6 >> 16);
		return SDR_Success;
	}
	if (respR6 & SD_R6_GENERAL_UNKNOWN_ERROR) { return SDR_UnknownError;   }
	if (respR6 & SD_R6_ILLEGAL_CMD)           { return SDR_IllegalCommand; }
	if (respR6 & SD_R6_COM_CRC_FAILED)        { return SDR_ComCRCError;    }

	return SDR_Success;
}

SD::SDResult SD::GetR7Resp(void) {
	volatile uint32_t wait = SD_CMD_TIMEOUT;
	// Wait for response, error or timeout
	while (!(SDMMC2->STA & (SDMMC_STA_CCRCFAIL | SDMMC_STA_CMDREND | SDMMC_STA_CTIMEOUT)) && --wait);
	// Timeout?
	if ((SDMMC2->STA & SDMMC_STA_CTIMEOUT) || (wait == 0)) {
		SDMMC2->ICR = SDMMC_ICR_CTIMEOUTC;
		return SDR_Timeout;
	}
	// Clear command response received flag
	if (SDMMC2->STA & SDMMC_STA_CMDREND) {
		SDMMC2->ICR = SDMMC_ICR_CMDRENDC;
		return SDR_Success;
	}
	return SDR_NoResponse;
}

SD::SDResult SD::GetSCR(uint32_t *pSCR) {
	SDResult cmd_res;
	// Set block size to 8 bytes
	Cmd(SD_CMD_SET_BLOCKLEN, 8, SD_RESP_SHORT); // CMD16
	cmd_res = GetR1Resp(SD_CMD_SET_BLOCKLEN);
	if (cmd_res != SDR_Success) {
		return cmd_res;
	}
	// Send leading command for ACMD<n> command
	Cmd(SD_CMD_APP_CMD, SDCard.RCA << 16, SD_RESP_SHORT); // CMD55
	cmd_res = GetR1Resp(SD_CMD_APP_CMD);
	if (cmd_res != SDR_Success) {
		return cmd_res;
	}
	// Clear the data flags
	SDMMC2->ICR = SDIO_ICR_DATA;
	// Configure the SDIO data transfer
	SDMMC2->DTIMER = 0xFFFF; //SD_DATA_R_TIMEOUT; // Data read timeout
	SDMMC2->DLEN   = 8; // Data length in bytes
	// Data transfer:
	//   - type: block   - direction: card -> controller   - size: 2^3 = 8bytes   - DPSM: enabled
	SDMMC2->DCTRL  = SDMMC_DCTRL_DTDIR | (3 << 4) | SDMMC_DCTRL_DTEN;// | SDMMC_DCTRL_SDIOEN;
	
	// Send SEND_SCR command
	Cmd(SD_CMD_SEND_SCR, 0, SD_RESP_SHORT); // ACMD51
	cmd_res = GetR1Resp(SD_CMD_SEND_SCR);
	if (cmd_res != SDR_Success) {
		return cmd_res;
	}
	// Receive the SCR register value
	uint32_t timeout = 0xFFFFF;
	uint32_t timeout1 = 100;
	uint32_t WordCounter = 0;
	uint32_t Counter1 = 0;
	uint32_t Counter2 = 0;
	while (!(SDMMC2->STA & (SDMMC_STA_RXOVERR | SDMMC_STA_DCRCFAIL | SDMMC_STA_DTIMEOUT | SDMMC_STA_DBCKEND | SDMMC_STA_DATAEND))) {
		// Read word when data available in receive FIFO
		Counter1++;
		if(!(SDMMC2->STA & SDMMC_STA_RXFIFOE)){
			Counter2++;
			if(SDMMC2->DCOUNT != 0) {
				*pSCR++ = SDMMC2->FIFO;
				WordCounter++;
				while(timeout1--);
				timeout1=0;
			}			
		}			 
		if(timeout-- == 0 ){break;}
	}
	// Check for errors
	if (SDMMC2->STA & (SDMMC_STA_DTIMEOUT | SDMMC_STA_DCRCFAIL | SDMMC_STA_RXOVERR )) {
		if (SDMMC2->STA & SDMMC_STA_DTIMEOUT) { cmd_res = SDR_DataTimeout;   }
		if (SDMMC2->STA & SDMMC_STA_DCRCFAIL) { cmd_res = SDR_DataCRCFail;   }
		if (SDMMC2->STA & SDMMC_STA_RXOVERR)  { cmd_res = SDR_RXOverrun;     }
		if (SDMMC2->STA & SDMMC_STA_DABORT) { cmd_res = SDR_StartBitError; }
	}
	// Clear the static SDIO flags
	SDMMC2->ICR = SDIO_ICR_STATIC;
	return cmd_res;
}

SD::SDResult SD::SetBlockSize(uint32_t block_size) {
	// Send SET_BLOCKLEN command
	Cmd(SD_CMD_SET_BLOCKLEN, block_size, SD_RESP_SHORT); // CMD16
	return GetR1Resp(SD_CMD_SET_BLOCKLEN);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//***********************************  INIT  *****************************************************************************************

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SD::SDResult SD::SD_Init(void) {
	volatile uint32_t wait;
	uint32_t sd_type = SD_STD_CAPACITY; // SD card capacity
	SDResult cmd_res;
	// Populate SDCard structure with default values
	SDCard = (SDCard_TypeDef){0};
	SDCard.Type = SDCT_UNKNOWN;
	// Enable the SDIO clock
	SDMMC2->POWER = SDMMC_POWER_PWRCTRL; // power on
	// CMD0
	wait = SD_CMD_TIMEOUT;
	Cmd(SD_CMD_GO_IDLE_STATE, 0x00, SD_RESP_NONE);
	while (!(SDMMC2->STA & (SDMMC_STA_CTIMEOUT | SDMMC_STA_CMDSENT)) && --wait);
	if ((SDMMC2->STA & SDMMC_STA_CTIMEOUT) || !wait) {
		return SDR_Timeout;
	}
	// CMD8: SEND_IF_COND. Send this command to verify SD card interface operating condition
	// Argument: - [31:12]: Reserved (shall be set to '0')
	//           - [11:08]: Supply Voltage (VHS) 0x1 (Range: 2.7-3.6 V)
	//           - [07:00]: Check Pattern (recommended 0xAA)
	Cmd(SD_CMD_HS_SEND_EXT_CSD, SD_CHECK_PATTERN, SD_RESP_SHORT); // CMD8
	cmd_res = GetR7Resp();
	if (cmd_res == SDR_Success) {
		// SD v2.0 or later
		// Check echo-back of check pattern
		if ((SDMMC2->RESP1 & 0x01FF) != (SD_CHECK_PATTERN & 0x01FF)) {
			return SDR_Unsupported;
		}
		sd_type = SD_HIGH_CAPACITY; // SD v2.0 or later
		// Issue ACMD41 command
		wait = SD_ACMD41_TRIALS;
		while (--wait) {
			// Send leading command for ACMD<n> command
			Cmd(SD_CMD_APP_CMD, 0, SD_RESP_SHORT); // CMD55 with RCA 0
			cmd_res = GetR1Resp(SD_CMD_APP_CMD);
			if (cmd_res != SDR_Success) {
				return cmd_res;
			}
			// ACMD41 - initiate initialization process.
			// Set 3.0-3.3V voltage window (bit 20)
			// Set HCS bit (30) (Host Capacity Support) to inform card what host support high capacity
			// Set XPC bit (28) (SDXC Power Control) to use maximum performance (SDXC only)
			Cmd(SD_CMD_SD_SEND_OP_COND, SD_OCR_VOLTAGE | sd_type, SD_RESP_SHORT);
			cmd_res = GetR3Resp();
			if (cmd_res != SDR_Success) {
				return cmd_res;
			}
			if (SDMMC2->RESP1 & (1 << 31)) {
				// The SD card has finished the power-up sequence
				break;
			}
		}
		if (wait == 0) {
			// Unsupported voltage range
			return SDR_InvalidVoltage;
		}
		// This is SDHC/SDXC card?
		SDCard.Type = (SDMMC2->RESP1 & SD_HIGH_CAPACITY) ? SDCT_SDHC : SDCT_SDSC_V2;
	} else if (cmd_res == SDR_Timeout) {
		// SD v1.x or MMC
		// Issue CMD55 to reset 'Illegal command' bit of the SD card
		Cmd(SD_CMD_APP_CMD, 0, SD_RESP_SHORT); // CMD55 with RCA 0
		GetR1Resp(SD_CMD_APP_CMD);
		// Issue ACMD41 command with zero argument
		wait = SD_ACMD41_TRIALS;
		while (--wait) {
			// Send leading command for ACMD<n> command
			Cmd(SD_CMD_APP_CMD, 0, SD_RESP_SHORT); // CMD55 with RCA 0
			cmd_res = GetR1Resp(SD_CMD_APP_CMD);
			if (cmd_res != SDR_Success) {
				return cmd_res;
			}
			// Send ACMD41 - initiate initialization process (bit HCS = 0)
			Cmd(SD_CMD_SD_SEND_OP_COND, SD_OCR_VOLTAGE, SD_RESP_SHORT); // ACMD41
			cmd_res = GetR3Resp();
			if (cmd_res == SDR_Timeout) {
				// MMC will not respond to this command
				break;
			}
			if (cmd_res != SDR_Success) {
				return cmd_res;
			}
			if (SDMMC2->RESP1 & (1 << 31)) {
				// The SD card has finished the power-up sequence
				break;
			}
		}
		if (wait == 0) {
			// Unknown/Unsupported card type
			return SDR_UnknownCard;
		}
		if (cmd_res != SDR_Timeout) {
			// SD v1.x
			SDCard.Type = SDCT_SDSC_V1; // SDv1
		} else {
			// MMC or not SD memory card
			////////////////////////////////////////////////////////////////
			// This part has not been tested due to lack of MMCmicro card //
			////////////////////////////////////////////////////////////////
			wait = SD_ACMD41_TRIALS;
			while (--wait) {
				// Issue CMD1: initiate initialization process.
				Cmd(SD_CMD_SEND_OP_COND, SD_OCR_VOLTAGE, SD_RESP_SHORT); // CMD1
				cmd_res = GetR3Resp();
				if (cmd_res != SDR_Success) {
					return cmd_res;
				}
				if (SDMMC2->RESP1 & (1 << 31)) {
					// The SD card has finished the power-up sequence
					break;
				}
			}
			if (wait == 0) {
				return SDR_UnknownCard;
			}
			SDCard.Type = SDCT_MMC; // MMC
		}
	} else {
		return cmd_res;
	}
	// Now the CMD2 and CMD3 commands should be issued in cycle until timeout to enumerate all cards on the bus
	// Since this module suitable to work with single card, issue this commands one time only
	// Send ALL_SEND_CID command
	Cmd(SD_CMD_ALL_SEND_CID, 0, SD_RESP_LONG); // CMD2
	cmd_res = GetR2Resp((uint32_t *)SDCard.CID); // response is a value of the CID/CSD register
	if (cmd_res != SDR_Success) {
		return cmd_res;
	}
	if (SDCard.Type != SDCT_MMC) {
		// Send SEND_REL_ADDR command to ask the SD card to publish a new RCA (Relative Card Address)
		// Once the RCA is received the card state changes to the stand-by state
		Cmd(SD_CMD_SEND_REL_ADDR, 0, SD_RESP_SHORT); // CMD3
		cmd_res = GetR6Resp(SD_CMD_SEND_REL_ADDR, (uint16_t *)(&SDCard.RCA));
		if (cmd_res != SDR_Success) {
			return cmd_res;
		}
	} else {
		////////////////////////////////////////////////////////////////
		// This part has not been tested due to lack of MMCmicro card //
		////////////////////////////////////////////////////////////////
		// For MMC card host should set a RCA value to the card by SET_REL_ADDR command
		Cmd(SD_CMD_SEND_REL_ADDR, SDIO_MMC_RCA << 16, SD_RESP_SHORT); // CMD3
		cmd_res = GetR1Resp(SD_CMD_SEND_REL_ADDR);
		if (cmd_res != SDR_Success) {
			return cmd_res;
		}
		SDCard.RCA = SDIO_MMC_RCA;
	}
	// Send SEND_CSD command to retrieve CSD register from the card
	Cmd(SD_CMD_SEND_CSD, SDCard.RCA << 16, SD_RESP_LONG); // CMD9
	cmd_res = GetR2Resp((uint32_t *)SDCard.CSD);
	if (cmd_res != SDR_Success) {
		return cmd_res;
	}
	//------------------------------------------------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------------------------------------------
    //------------------------------------------------------------------------------------------------------------------------------
	// Parse the values of CID and CSD registers
	GetCardInfo();
	// Now card must be in stand-by mode, from this point it is possible to increase bus speed
	SetBusClock(7); // here sets SDMMC2 prescaler  360/7=48 MHz  (36MHz)
    //---------------------------------------------------------------------------------------------------------------------------------
	// Put the SD card to the transfer mode
	Cmd(SD_CMD_SEL_DESEL_CARD, SDCard.RCA << 16, SD_RESP_SHORT); // CMD7
	cmd_res = GetR1Resp(SD_CMD_SEL_DESEL_CARD); // In fact R1b response here
	if (cmd_res != SDR_Success) {
		return cmd_res;
	}
	// Disable the pull-up resistor on CD/DAT3 pin of card
	// Send leading command for ACMD<n> command
	Cmd(SD_CMD_APP_CMD, SDCard.RCA << 16, SD_RESP_SHORT); // CMD55
	cmd_res = GetR1Resp(SD_CMD_APP_CMD);
	if (cmd_res != SDR_Success) {
		return cmd_res;
	}
	// Send SET_CLR_CARD_DETECT command
	Cmd(SD_CMD_SET_CLR_CARD_DETECT, 0, SD_RESP_SHORT); // ACMD42
	cmd_res = GetR1Resp(SD_CMD_SET_CLR_CARD_DETECT);
	if (cmd_res != SDR_Success) {
		return cmd_res;
	}
	// Read the SCR register
	if (SDCard.Type != SDCT_MMC) {
		// MMC card doesn't support this feature
		// Warning: this function set block size to 8 bytes
		GetSCR((uint32_t *)SDCard.SCR);
	}
	// For SDv1, SDv2 and MMC card must set block size
	// The SDHC/SDXC always have fixed block size (512 bytes)
	if ((SDCard.Type == SDCT_SDSC_V1) || (SDCard.Type == SDCT_SDSC_V2) || (SDCard.Type == SDCT_MMC)) {
		Cmd(SD_CMD_SET_BLOCKLEN, 512, SD_RESP_SHORT); // CMD16
		cmd_res = GetR1Resp(SD_CMD_SET_BLOCKLEN);
		if (cmd_res != SDR_Success) {
			return SDR_SetBlockSizeFailed;
		}
	}
	return SDR_Success;
}

SD::SDResult SD::SetBusWidth(uint32_t BW) {
	SDResult cmd_res = SDR_Success;
	uint32_t clk;
	if (SDCard.Type != SDCT_MMC) {
		// Send leading command for ACMD<n> command
		Cmd(SD_CMD_APP_CMD, SDCard.RCA << 16, SD_RESP_SHORT); // CMD55
		cmd_res = GetR1Resp(SD_CMD_APP_CMD);
		if (cmd_res != SDR_Success) {
			return cmd_res;
		}
		// Send SET_BUS_WIDTH command
		clk = (BW == SD_BUS_1BIT) ? 0x00000000 : 0x00000002;
		Cmd(SD_CMD_SET_BUS_WIDTH, clk, SD_RESP_SHORT); // ACMD6
		cmd_res = GetR1Resp(SD_CMD_SET_BUS_WIDTH);
		if (cmd_res != SDR_Success) {
			return cmd_res;
		}
	} else {
		// MMC supports only 8-bit ?
	}
	// Configure new bus width
	clk  = SDMMC2->CLKCR;
	clk &= ~SDMMC_CLKCR_WIDBUS;
	clk |= (BW & SDMMC_CLKCR_WIDBUS);
	SDMMC2->CLKCR = clk;
	return cmd_res;
}

void SD::SetBusClock(uint32_t clk_div) {
	uint32_t clk;
	clk  = SDMMC2->CLKCR;
	clk &= ~SDMMC_CLKCR_CLKDIV;
	clk |= (clk_div & SDMMC_CLKCR_CLKDIV); // new value
	SDMMC2->CLKCR = clk;
}

void SD::GetCardInfo(void) {
	uint32_t dev_size;
	uint32_t dev_size_mul;
	// Parse the CSD register
	SDCard.CSDVer = SDCard.CSD[0] >> 6; // CSD version (0-SD 1-SDHC)
	if (SDCard.Type != SDCT_MMC) {
		// SD
		SDCard.MaxBusClkFreq = SDCard.CSD[3];
		if (SDCard.CSDVer == 0) {
			// CSD v1.00 (SDSCv1, SDSCv2)
			dev_size  = (uint32_t)(SDCard.CSD[6] & 0x03) << 10; // Device size
			dev_size |= (uint32_t)SDCard.CSD[7] << 2;
			dev_size |= (SDCard.CSD[8] & 0xc0) >> 6;
			dev_size_mul  = (SDCard.CSD[ 9] & 0x03) << 1; // Device size multiplier
			dev_size_mul |= (SDCard.CSD[10] & 0x80) >> 7;
			SDCard.BlockCount  = dev_size + 1;
			SDCard.BlockCount *= 1 << (dev_size_mul + 2);
			SDCard.BlockSize   = 1 << (SDCard.CSD[5] & 0x0f); // Maximum read data block length
		} else {
			// CSD v2.00 (SDHC, SDXC)
			dev_size  = (SDCard.CSD[7] & 0x3f) << 16;
			dev_size |=  SDCard.CSD[8] << 8;
			dev_size |=  SDCard.CSD[9]; // C_SIZE
			SDCard.BlockSize = 512;
			SDCard.BlockCount = (dev_size + 1)*1024;
			// BlockCount >= 65535 means that this is SDXC card
		}
		SDCard.Capacity = (SDCard.BlockCount/(1024*1024)) * (SDCard.BlockSize);
	} else {
		// MMC
		SDCard.MaxBusClkFreq = SDCard.CSD[3];
		dev_size  = (uint32_t)(SDCard.CSD[6] & 0x03) << 8; // C_SIZE
		dev_size += (uint32_t)SDCard.CSD[7];
		dev_size <<= 2;
		dev_size += SDCard.CSD[8] >> 6;
		SDCard.BlockSize = 1 << (SDCard.CSD[5] & 0x0f); // MMC read block length
		dev_size_mul = ((SDCard.CSD[9] & 0x03) << 1) + ((SDCard.CSD[10] & 0x80) >> 7);
		SDCard.BlockCount = (dev_size + 1) * (1 << (dev_size_mul + 2));
		SDCard.Capacity = SDCard.BlockCount * SDCard.BlockSize;
	}
	// Parse the CID register
	if (SDCard.Type != SDCT_MMC) {
		// SD card
		SDCard.MID = SDCard.CID[0];
		SDCard.OID = (SDCard.CID[1] << 8) | SDCard.CID[2];
		SDCard.PNM[0] = SDCard.CID[3];
		SDCard.PNM[1] = SDCard.CID[4];
		SDCard.PNM[2] = SDCard.CID[5];
		SDCard.PNM[3] = SDCard.CID[6];
		SDCard.PNM[4] = SDCard.CID[7];
		SDCard.PRV = SDCard.CID[8];
		SDCard.PSN = (SDCard.CID[9] << 24) | (SDCard.CID[10] << 16) | (SDCard.CID[11] << 8) | SDCard.CID[12];
		SDCard.MDT = ((SDCard.CID[13] << 8) | SDCard.CID[14]) & 0x0fff;
	} else {
		// MMC
		SDCard.MID = 0x00;
		SDCard.OID = 0x0000;
		SDCard.PNM[0] = '*';
		SDCard.PNM[1] = 'M';
		SDCard.PNM[2] = 'M';
		SDCard.PNM[3] = 'C';
		SDCard.PNM[4] = '*';
		SDCard.PRV = 0;
		SDCard.PSN = 0x00000000;
		SDCard.MDT = 0x0000;
	}
}

SD::SDResult SD::CmdSwitch(uint32_t argument, uint8_t *resp) {
	uint32_t *ptr = (uint32_t *)resp;
	SDResult res = SDR_Success;
	// SD specification says that response size is always 512bits,
	// thus there is no need to set block size before issuing CMD6
	// Clear the data flags
	SDMMC2->ICR = SDIO_ICR_DATA;
	// Data read timeout
	SDMMC2->DTIMER = SD_DATA_R_TIMEOUT;
	// Data length in bytes
	SDMMC2->DLEN = 64;
	// Data transfer:
	//   transfer mode: block   direction: to host   block size: 2^6 = 64 bytes   
	SDMMC2->DCTRL = SDMMC_DCTRL_DTDIR | (6 << 4) | SDMMC_DCTRL_DTEN;
	// Send SWITCH_FUNCTION command
	// Argument:
	//   [31]: MODE: 1 for switch, 0 for check
	//   [30:24]: reserved, all should be '0' [23:20]: GRP6 - reserved    [19:16]: GRP5 - reserved
	//   [15:12]: GRP4 - power limit
	//   [11:08]: GRP3 - driver strength
	//   [07:04]: GRP2 - command system
	//   [03:00]: GRP1 - access mode (a.k.a. bus speed mode)
	//   Values for groups 6..2: 0xF: no influence      0x0: default
	Cmd(SD_CMD_SWITCH_FUNC, argument, SD_RESP_SHORT); // CMD6
	res = GetR1Resp(SD_CMD_SWITCH_FUNC);
	if (res != SDR_Success) {
		return res;
	}
	// Read the CCCR register value
	volatile uint32_t timeout = 0xFFFF;
	uint8_t counter =0;
	while ((!(SDMMC2->STA & (SDMMC_STA_RXOVERR | SDMMC_STA_DCRCFAIL | SDMMC_STA_DTIMEOUT | SDMMC_STA_DBCKEND | SDMMC_STA_DATAEND)))) {
		// The receive FIFO is half full, there are at least 8 words in it
		if (SDMMC2->STA & SDMMC_STA_RXFIFOHF) {
			*ptr++ = SDMMC2->FIFO;
			*ptr++ = SDMMC2->FIFO;
			*ptr++ = SDMMC2->FIFO;
			*ptr++ = SDMMC2->FIFO;
			*ptr++ = SDMMC2->FIFO;
			*ptr++ = SDMMC2->FIFO;
			*ptr++ = SDMMC2->FIFO;
			*ptr++ = SDMMC2->FIFO; 
			counter++;
		}		
		if(timeout == 0) {break;} timeout--;
		
	}
	// Check for errors
	if (SDMMC2->STA & SDIO_XFER_ERROR_FLAGS) {
		if (SDMMC2->STA & SDMMC_STA_DTIMEOUT) { res = SDR_DataTimeout;   }
		if (SDMMC2->STA & SDMMC_STA_DCRCFAIL) { res = SDR_DataCRCFail;   }
		if (SDMMC2->STA & SDMMC_STA_RXOVERR)  { res = SDR_RXOverrun;     }
	}
	// Read the data remnant from the SDIO FIFO (should not be, but just in case)
	timeout=0xFFFF;
	while ((!(SDMMC2->STA & (SDMMC_STA_RXOVERR | SDMMC_STA_DCRCFAIL | SDMMC_STA_DATAEND)))) {
		if(!(SDMMC2->STA & (SDMMC_STA_RXFIFOE))) {
			*ptr++ = SDMMC2->FIFO;			
		}		
		if(timeout == 0) {break;} timeout--;
	}
	// Clear the static SDIO flags
	SDMMC2->ICR = SDIO_ICR_STATIC;
	return res;
}

SD::SDResult SD::HighSpeed(void) {
	uint8_t CCCR[64];
	SDResult cmd_res = SDR_Success;
	// Check if the card supports HS mode
	cmd_res = CmdSwitch(
				(0x0U << 31) | // MODE: check
				(0xFU << 20) | // GRP6: no influence
				(0xFU << 16) | // GRP5: no influence
				(0xFU << 12) | // GRP4: no influence
				(0xFU <<  8) | // GRP3: no influence
				(0xFU <<  4) | // GRP2: default
				(0x1U <<  0),  // GRP1: high speed
				CCCR
			);
	if (cmd_res != SDR_Success) {
		return SDR_UnknownError;
	}
	// Check SHS bit from CCCR
	if ((CCCR[63 - (400 / 8)]) & (0x01 != 0x01)) {
		return SDR_Unsupported;
	}
#if 0
	// Check EHS bit (BSS0) from CCCR
	if (CCCR[63 - (400 / 8)] & 0x02 != 0x02) {
		return SDR_Unsupported;
	}
#endif
	// Ask the card to switch to HS mode
	cmd_res = CmdSwitch(
				(0x1U << 31) | // MODE: switch
				(0xFU << 20) | // GRP6: no influence
				(0xFU << 16) | // GRP5: no influence
				(0xFU << 12) | // GRP4: no influence
				(0xFU <<  8) | // GRP3: no influence
				(0xFU <<  4) | // GRP2: default
				(0x1U <<  0),  // GRP1: high speed
				CCCR
			);
	if (cmd_res != SDR_Success) {
		return SDR_UnknownError;
	}
	// Note: the SD specification says "card shall switch speed mode within 8 clocks
	// after the end bit of the corresponding response"
	// Apparently, this is the reason why some SD cards demand a delay between
	// the request for mode switching and the possibility to continue communication
	return SDR_Success;
}

SD::SDResult SD::StopTransfer(void) {
	// Send STOP_TRANSMISSION command
	Cmd(SD_CMD_STOP_TRANSMISSION, 0, SD_RESP_SHORT); // CMD12
	return GetR1Resp(SD_CMD_STOP_TRANSMISSION);
}

SD::SDResult SD::GetCardState(uint8_t *pState) {
	uint8_t cmd_res;
	// Send SEND_STATUS command
	Cmd(SD_CMD_SEND_STATUS, SDCard.RCA << 16, SD_RESP_SHORT); // CMD13
	cmd_res = GetR1Resp(SD_CMD_SEND_STATUS);
	if (cmd_res != SDR_Success) {
		*pState = SD_STATE_ERROR;
		return (SDResult)cmd_res;
	}
	// Find out a card status
	*pState = (SDMMC2->RESP1 & 0x1e00) >> 9;
	// Check for errors
	return SDR_Success;
}

SD::SDResult SD::ReadBlock(uint32_t addr, uint32_t *pBuf, uint32_t length) {
	SDResult cmd_res = SDR_Success;
	uint32_t blk_count = length >> 9; // Sectors in block  (/512)
	register uint32_t STA; // to speed up SDIO flags checking
	register uint32_t STA_mask; // mask for SDIO flags checking
	// Initialize the data control register
	SDMMC2->DCTRL = 0;
	// SDSC card uses byte unit address and
	// SDHC/SDXC cards use block unit address (1 unit = 512 bytes)
	// For SDHC card addr must be converted to block unit address
	//if (SDCard.Type == SDCT_SDHC) {
	//	addr >>= 9;
	//}
	// Clear the static SDIO flags
	SDMMC2->ICR = SDIO_ICR_STATIC;
	if (blk_count > 1) {
		// Prepare bit checking variable for multiple block transfer
		STA_mask = SDIO_RX_MB_FLAGS;
		// Send READ_MULT_BLOCK command
		Cmd(SD_CMD_READ_MULT_BLOCK, addr, SD_RESP_SHORT); // CMD18
		cmd_res = GetR1Resp(SD_CMD_READ_MULT_BLOCK);
	} else {
		// Prepare bit checking variable for single block transfer
		STA_mask = SDIO_RX_SB_FLAGS;
		// Send READ_SINGLE_BLOCK command
		Cmd(SD_CMD_READ_SINGLE_BLOCK, addr, SD_RESP_SHORT); // CMD17
		cmd_res = GetR1Resp(SD_CMD_READ_SINGLE_BLOCK);
	}
	if (cmd_res != SDR_Success) {
		return cmd_res;
	}
	// Data read timeout
	SDMMC2->DTIMER = SD_DATA_R_TIMEOUT;
	// Data length
	SDMMC2->DLEN   = length;
	// Data transfer:
	//   transfer mode: block   direction: from card   DMA: disabled   block size: 2^9 = 512 bytes   DPSM: enabled
	SDMMC2->DCTRL  = SDMMC_DCTRL_DTDIR | (9 << 4) | SDMMC_DCTRL_DTEN;
	// Receive a data block from the SDIO
	// ----> TIME CRITICAL SECTION BEGIN <----
	uint32_t timeout = 0xFFFF;
	do {
		STA = SDMMC2->STA;
		if (STA & SDMMC_STA_RXFIFOHF) {
			// The receive FIFO is half full, there are at least 8 words in it
			*pBuf++ = SDMMC2->FIFO;
			*pBuf++ = SDMMC2->FIFO;
			*pBuf++ = SDMMC2->FIFO;
			*pBuf++ = SDMMC2->FIFO;
			*pBuf++ = SDMMC2->FIFO;
			*pBuf++ = SDMMC2->FIFO;
			*pBuf++ = SDMMC2->FIFO;
			*pBuf++ = SDMMC2->FIFO;
		}
		if(!timeout){break;}
		timeout--;
	} while (!(STA & STA_mask));
	// <---- TIME CRITICAL SECTION END ---->

	// Send stop transmission command in case of multiple block transfer
	if ((SDCard.Type != SDCT_MMC) && (blk_count > 1)) {
		cmd_res = StopTransfer();
	}

	// Check for errors
	if (STA & SDIO_XFER_ERROR_FLAGS) {
		if (STA & SDMMC_STA_DTIMEOUT) cmd_res = SDR_DataTimeout;
		if (STA & SDMMC_STA_DCRCFAIL) cmd_res = SDR_DataCRCFail;
		if (STA & SDMMC_STA_RXOVERR)  cmd_res = SDR_RXOverrun;
	}

	// Read the data remnant from RX FIFO (if there is still any data)
	timeout = 0xFFF;
	while(!((SDMMC2->STA) & (SDMMC_STA_DATAEND | SDMMC_STA_DBCKEND))) {
		if (!(SDMMC2->STA & SDMMC_STA_RXFIFOE)) {
			*pBuf++ = SDMMC2->FIFO;		
		}
		if(!timeout){break; timeout--;}
	}
	
	// Clear the static SDIO flags
	SDMMC2->ICR = SDIO_ICR_STATIC;
	return cmd_res;
}

SD::SDResult SD::WriteBlock(uint32_t addr, uint32_t *pBuf, uint32_t length) {
	SDResult cmd_res = SDR_Success;
	uint32_t blk_count = length >> 9; // Sectors in block
	uint32_t STA; // To speed up SDIO flags checking
	register uint32_t STA_mask; // Mask for SDIO flags checking
	uint32_t data_sent = 0; // Counter of transferred bytes
	uint8_t card_state; // Card state
	// Initialize the data control register
	SDMMC2->DCTRL = 0;
	// SDSC card uses byte unit address and
	// SDHC/SDXC cards use block unit address (1 unit = 512 bytes)
	// For SDHC card addr must be converted to block unit address
	//if (SDCard.Type == SDCT_SDHC) {
	//	addr >>= 9;
	//}
	if (blk_count > 1) {
		// Prepare bit checking variable for multiple block transfer
		STA_mask = SDIO_TX_MB_FLAGS;
		// Send WRITE_MULTIPLE_BLOCK command
		Cmd(SD_CMD_WRITE_MULTIPLE_BLOCK, addr, SD_RESP_SHORT); // CMD25
		cmd_res = GetR1Resp(SD_CMD_WRITE_MULTIPLE_BLOCK);
	} else {
		// Prepare bit checking variable for single block transfer
		STA_mask = SDIO_TX_SB_FLAGS;
		// Send WRITE_BLOCK command
		Cmd(SD_CMD_WRITE_BLOCK, addr, SD_RESP_SHORT); // CMD24
		cmd_res = GetR1Resp(SD_CMD_WRITE_BLOCK);
	}
	if (cmd_res != SDR_Success) {
		return cmd_res;
	}
	// Clear the static SDIO flags
	SDMMC2->ICR = SDIO_ICR_STATIC;
	// Data write timeout
	SDMMC2->DTIMER = SD_DATA_W_TIMEOUT;
	// Data length
	SDMMC2->DLEN = length;
	// Data transfer:
	//   transfer mode: block   direction: to card   DMA: disabled   block size: 2^9 = 512 bytes   DPSM: enabled
	SDMMC2->DCTRL = (9 << 4) | SDMMC_DCTRL_DTEN;
	// Transfer data block to the SDIO
	// ----> TIME CRITICAL SECTION BEGIN <----
	if (!(length & 0x1F)) {
		// The block length is multiple of 32, simplified transfer procedure can be used
		do {
			if ((SDMMC2->STA & SDMMC_STA_TXFIFOHE) && (data_sent < length)) {
				// The TX FIFO is half empty, at least 8 words can be written
				SDMMC2->FIFO = *pBuf++;
				SDMMC2->FIFO = *pBuf++;
				SDMMC2->FIFO = *pBuf++;
				SDMMC2->FIFO = *pBuf++;
				SDMMC2->FIFO = *pBuf++;
				SDMMC2->FIFO = *pBuf++;
				SDMMC2->FIFO = *pBuf++;
				SDMMC2->FIFO = *pBuf++;
				data_sent += 32;
			}
		} while (!(SDMMC2->STA & STA_mask));
	} else {
		// Since the block length is not a multiple of 32, it is necessary to apply additional calculations
		do {
			if ((SDMMC2->STA & SDMMC_STA_TXFIFOHE) && (data_sent < length)) {
				// TX FIFO half empty, at least 8 words can be written
				uint32_t data_left = length - data_sent;
				if (data_left < 32) {
					// Write last portion of data to the TX FIFO
					data_left = ((data_left & 0x03) == 0) ? (data_left >> 2) : ((data_left >> 2) + 1);
					data_sent += data_left << 2;
					while (data_left--) {
						SDMMC2->FIFO = *pBuf++;
					}
				} else {
					// Write 8 words to the TX FIFO
					SDMMC2->FIFO = *pBuf++;
					SDMMC2->FIFO = *pBuf++;
					SDMMC2->FIFO = *pBuf++;
					SDMMC2->FIFO = *pBuf++;
					SDMMC2->FIFO = *pBuf++;
					SDMMC2->FIFO = *pBuf++;
					SDMMC2->FIFO = *pBuf++;
					SDMMC2->FIFO = *pBuf++;
					data_sent += 32;
				}
			}
		} while (!(SDMMC2->STA & STA_mask));
	}
	// <---- TIME CRITICAL SECTION END ---->
	// Save STA register value for further analysis
	STA = SDMMC2->STA;
	// Send stop transmission command in case of multiple block transfer
	if ((SDCard.Type != SDCT_MMC) && (blk_count > 1)) {
		cmd_res = StopTransfer();
	}
	// Check for errors
	if (STA & SDIO_XFER_ERROR_FLAGS) {
		if (STA & SDMMC_STA_DTIMEOUT) cmd_res = SDR_DataTimeout;
		if (STA & SDMMC_STA_DCRCFAIL) cmd_res = SDR_DataCRCFail;
		if (STA & SDMMC_STA_TXUNDERR) cmd_res = SDR_TXUnderrun;
	}

	// Wait while the card is in programming state
	do {
		if (GetCardState(&card_state) != SDR_Success) {
			break;
		}
	} while ((card_state == SD_STATE_PRG) || (card_state == SD_STATE_RCV));
	// Clear the static SDIO flags
	SDMMC2->ICR = SDIO_ICR_STATIC;
	return cmd_res;
}

//------------------------------------------------------------------------------------------------------------------
//						ERASE
//------------------------------------------------------------------------------------------------------------------
SD::SDResult SD::EraseBlock(uint32_t addr1, uint32_t addr2) {
	SDResult cmd_res = SDR_Success;
	Cmd(SD_CMD_SD_ERASE_GRP_START, addr1, SD_RESP_SHORT); // CMD38
	cmd_res = GetR1Resp(SD_CMD_SD_ERASE_GRP_START);
	if (cmd_res != SDR_Success) {
		return cmd_res;
	}
	Cmd(SD_CMD_SD_ERASE_GRP_END, addr2, SD_RESP_SHORT); // CMD24
	cmd_res = GetR1Resp(SD_CMD_SD_ERASE_GRP_END);
	if (cmd_res != SDR_Success) {
		return cmd_res;
	}
	Cmd(SD_CMD_ERASE, 0, SD_RESP_SHORT); // CMD24
	cmd_res = GetR1Resp(SD_CMD_ERASE);
	if (cmd_res != SDR_Success) {
		return cmd_res;
	}
	// Clear the static SDIO flags
	SDMMC2->ICR = SDIO_ICR_STATIC;
	return cmd_res;
}


//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[  INIT  ]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
void SD::init() {
    gpio_init();
    sdmmc_init();
    SD_Init();	
	SetBusWidth(SD_BUS_4BIT);
	HighSpeed();
}

void SD::sdmmc_init() {
    RCC->AHB2ENR |= RCC_AHB2ENR_SDMMC2EN;
	RCC->D1CCIPR &=~ RCC_D1CCIPR_SDMMCSEL; // 0: q pll_1   1: r pll_2
    // Reset the SDIO peripheral
	RCC->AHB2RSTR |=  RCC_AHB2RSTR_SDMMC2RST;
	RCC->AHB2RSTR &=~ RCC_AHB2RSTR_SDMMC2RST;

    SDMMC2->CLKCR &=~ SDMMC_CLKCR_WIDBUS; // only D0 when initialization
    //SDMMC2->CLKCR |= SDMMC_CLKCR_WIDBUS_0;
    //SDMMC2->CLKCR &=~ SDMMC_CLKCR_WIDBUS_1; // 0:1 - 4-bit wide data bus mode
    SDMMC2->CLKCR |= 1000;    // prescaler on  (360/1000 = 360 kHz)  
	SDMMC2->DCTRL |= SDMMC_DCTRL_SDIOEN;
}

void SD::gpio_init() {
    // B3 and B4 must be reseted from JTAG interface (must be set in f10x)

    // B14-D0, B15-D1, B3-D2, B4-D3, A0-CMD - alt func pull-up   D6-CLK - alt func
    RCC->AHB4ENR |= RCC_AHB4ENR_GPIOAEN;
    RCC->AHB4ENR |= RCC_AHB4ENR_GPIOBEN;
    RCC->AHB4ENR |= RCC_AHB4ENR_GPIODEN;
    GPIOB->MODER |= (GPIO_MODER_MODE3_1 | GPIO_MODER_MODE4_1 | GPIO_MODER_MODE14_1 | GPIO_MODER_MODE15_1);
    GPIOB->MODER &=~ (GPIO_MODER_MODE3_0 | GPIO_MODER_MODE4_0 | GPIO_MODER_MODE14_0 | GPIO_MODER_MODE15_0);
    GPIOB->PUPDR |= (GPIO_PUPDR_PUPD3_0 | GPIO_PUPDR_PUPD4_0 | GPIO_PUPDR_PUPD14_0 | GPIO_PUPDR_PUPD15_0);
    GPIOB->PUPDR &=~ (GPIO_PUPDR_PUPD3_1 | GPIO_PUPDR_PUPD4_1 | GPIO_PUPDR_PUPD14_1 | GPIO_PUPDR_PUPD15_1);
    GPIOB->OSPEEDR |= (GPIO_OSPEEDR_OSPEED3 | GPIO_OSPEEDR_OSPEED4 | GPIO_OSPEEDR_OSPEED14 | GPIO_OSPEEDR_OSPEED15);
    GPIOB->AFR[0] |= ((9<<12) | (9<<16)); // alt func SDMMC2 to pin 3 and 4
    GPIOB->AFR[1] |= ((9<<24) | (9<<28)); // alt func SDMMC2 to pin 14 and 15

    GPIOA->MODER |= GPIO_MODER_MODE0_1;
    GPIOA->MODER &=~ GPIO_MODER_MODE0_0; // 1:0 - alt func
    GPIOA->PUPDR |= GPIO_PUPDR_PUPD0_0;
    GPIOA->PUPDR &=~ GPIO_PUPDR_PUPD0_1; // 0:1 - pull-up
    GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEED0;
    GPIOA->AFR[0] |= 9; // alt func SDMMC2 to pin 0

    GPIOD->MODER |= GPIO_MODER_MODE6_1;
    GPIOD->MODER &=~ GPIO_MODER_MODE6_0; // 1:0 - alt func
    GPIOD->OSPEEDR |= GPIO_OSPEEDR_OSPEED6;
    GPIOD->AFR[0] |= (11<<24); // alt func SDMMC2 to pin 6
}