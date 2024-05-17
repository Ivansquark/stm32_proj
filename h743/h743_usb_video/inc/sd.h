#ifndef SD_H
#define SD_H

#include "stm32h743xx.h"

class SD{
    enum SDResult {
        SDR_Success             = 0x00,
	    SDR_Timeout             = 0x01,  // Timeout
	    SDR_CRCError            = 0x02,  // Response for command received but CRC check failed
	    SDR_ReadError           = 0x03,  // Read block error (response for CMD17)
	    SDR_WriteError          = 0x04,  // Write block error (response for CMD24)
	    SDR_WriteErrorInternal  = 0x05,  // Write block error due to internal card error
	    SDR_Unsupported         = 0x06,  // Unsupported card found
	    SDR_BadResponse         = 0x07,
	    SDR_SetBlockSizeFailed  = 0x08,  // Set block size command failed (response for CMD16)
	    SDR_UnknownCard         = 0x09,
	    SDR_NoResponse          = 0x0A,
	    SDR_AddrOutOfRange      = 0x0B,  // Address out of range
	    SDR_WriteCRCError       = 0x0C,  // Data write rejected due to a CRC error
	    SDR_InvalidVoltage      = 0x0D,  // Unsupported voltage range
	    SDR_DataTimeout         = 0x0E,  // Data block transfer timeout
	    SDR_DataCRCFail         = 0x0F,  // Data block transfer CRC failed
	    SDR_RXOverrun           = 0x10,  // Receive FIFO overrun
	    SDR_TXUnderrun          = 0x11,  // Transmit FIFO underrun
	    SDR_StartBitError       = 0x12,  // Start bit not detected on all data signals
	    SDR_AddrMisaligned      = 0x13,  // A misaligned address which did not match the block length was used in the command
	    SDR_BlockLenError       = 0x14,  // The transfer block length is not allowed for this card
	    SDR_EraseSeqError       = 0x15,  // An error in the sequence of erase commands occurred
	    SDR_EraseParam          = 0x16,  // An invalid selection of write-blocks for erase occurred
	    SDR_WPViolation         = 0x17,  // Attempt to write to a protected block or to the write protected card
	    SDR_LockUnlockFailed    = 0x18,  // Error in lock/unlock command
	    SDR_ComCRCError         = 0x19,  // The CRC check of the previous command failed
	    SDR_IllegalCommand      = 0x1A,  // Command is not legal for the the current card state
	    SDR_CardECCFailed       = 0x1B,  // Card internal ECC was applied but failed to correct the data
	    SDR_CCError             = 0x1C,  // Internal card controller error
	    SDR_GeneralError        = 0x1D,  // A general or an unknown error occurred during the operation
	    SDR_StreamUnderrun      = 0x1E,  // The card could not sustain data transfer in stream read operation
	    SDR_StreamOverrun       = 0x1F,  // The card could not sustain data programming in stream mode
	    SDR_CSDOverwrite        = 0x20,  // CSD overwrite error
	    SDR_WPEraseSkip         = 0x21,  // Only partial address space was erased
	    SDR_ECCDisabled         = 0x22,  // The command has been executed without using the internal ECC
	    SDR_EraseReset          = 0x23,  // An erase sequence was cleared before executing
	    SDR_AKESeqError         = 0x24,  // Error in the sequence of the authentication process
	    SDR_UnknownError        = 0xFF   // Unknown error
    };
    //SD_result SDResult;
    // SD card description
    struct SDCard_TypeDef {
    	uint8_t     Type;            // Card type (detected by SD_Init())
    	uint32_t    Capacity;        // Card capacity (MBytes for SDHC/SDXC, bytes otherwise)
    	uint32_t    BlockCount;      // SD card blocks count
    	uint32_t    BlockSize;       // SD card block size (bytes), determined in SD_ReadCSD()
    	uint32_t    MaxBusClkFreq;   // Maximum card bus frequency (MHz)
    	uint8_t     CSDVer;          // SD card CSD register version
    	uint16_t    RCA;             // SD card RCA address (only for SDIO)
    	uint8_t     MID;             // SD card manufacturer ID
    	uint16_t    OID;             // SD card OEM/Application ID
    	uint8_t     PNM[5];          // SD card product name (5-character ASCII string)
    	uint8_t     PRV;             // SD card product revision (two BCD digits: '6.2' will be 01100010b)
    	uint32_t    PSN;             // SD card serial number
    	uint16_t    MDT;             // SD card manufacturing date
    	uint8_t     CSD[16];         // SD card CSD register (card structure data)
    	uint8_t     CID[16];         // SD card CID register (card identification number)
    	uint8_t     SCR[8];          // SD card SCR register (SD card configuration)
    };
    // SD card response type
    enum {
    	SD_R1  = 0x01, // R1
    	SD_R1b = 0x02, // R1b
    	SD_R2  = 0x03, // R2
    	SD_R3  = 0x04, // R3
    	SD_R6  = 0x05, // R6 (SDIO only)
    	SD_R7  = 0x06  // R7
    };    
    // Card type
    enum {
    	SDCT_UNKNOWN = 0x00,
    	SDCT_SDSC_V1 = 0x01,  // Standard capacity SD card v1.0
    	SDCT_SDSC_V2 = 0x02,  // Standard capacity SD card v2.0
    	SDCT_MMC     = 0x03,  // MMC
    	SDCT_SDHC    = 0x04   // High capacity SD card (SDHC or SDXC)
    };
//---------------------------------------------------------------------------------------------------------    
public:
    SD();
static SD* pThis;
    SDCard_TypeDef SDCard;

    void Cmd(uint8_t cmd, uint32_t arg, uint32_t resp_type);
    SDResult GetR1Resp(uint8_t cmd);
    SDResult GetR2Resp(uint32_t *pBuf);
    SDResult GetR3Resp(void);
    SDResult GetR6Resp(uint8_t cmd, uint16_t *pRCA);
    SDResult GetR7Resp(void);
    SDResult GetSCR(uint32_t *pSCR);
    SDResult SetBlockSize(uint32_t block_size);
    SDResult SetBusWidth(uint32_t BW);
    void SetBusClock(uint32_t clk_div);
    void GetCardInfo(void);
    SDResult CmdSwitch(uint32_t argument, uint8_t *resp);
    SDResult HighSpeed(void);
    SDResult StopTransfer(void);
    SDResult GetCardState(uint8_t *pState);
    SDResult ReadBlock(uint32_t addr, uint32_t *pBuf, uint32_t length);
    SDResult WriteBlock(uint32_t addr, uint32_t *pBuf, uint32_t length);    
    SDResult EraseBlock(uint32_t addr1, uint32_t addr2);

private:
    void init();
    void gpio_init();
    void sdmmc_init();
    SDResult SD_Init(void);

    // Mask for ACMD41
    static constexpr uint32_t SD_STD_CAPACITY  = 0x00000000U;
    static constexpr uint32_t SD_HIGH_CAPACITY = 0x40000000U;

    static constexpr uint32_t SD_CMD_TIMEOUT = 0x00010000U; //65536
    static constexpr uint32_t SDIO_ICR_STATIC = (SDMMC_ICR_CCRCFAILC | SDMMC_ICR_DCRCFAILC | SDMMC_ICR_CTIMEOUTC | 
				   SDMMC_ICR_DTIMEOUTC | SDMMC_ICR_TXUNDERRC | SDMMC_ICR_RXOVERRC  | 
				   SDMMC_ICR_CMDRENDC  | SDMMC_ICR_CMDSENTC  | SDMMC_ICR_DATAENDC  | 
				   SDMMC_ICR_DBCKENDC);
    // Bitmap to clear the SDIO data flags
    static constexpr uint32_t SDIO_ICR_DATA = (SDMMC_ICR_RXOVERRC | SDMMC_ICR_DCRCFAILC | 
											   SDMMC_ICR_DTIMEOUTC | SDMMC_ICR_DBCKENDC | 
											   SDMMC_ICR_DABORTC);               
    // SDIO timeout for data transfer ((48MHz / CLKDIV / 1000) * timeout_ms)
    static constexpr uint32_t SD_DATA_R_TIMEOUT = (((48000000U / 2U / 1000U) * 100U)); // Data read timeout is 100ms
    static constexpr uint32_t SD_DATA_W_TIMEOUT = (((48000000U / 2U / 1000U) * 250U)); // Date write timeout is 250ms
    // Following commands are SD Card Specific commands.
    // SD_CMD_APP_CMD should be sent before sending these commands.
    static constexpr uint8_t SD_CMD_SET_BUS_WIDTH       = 6U; // ACMD6
    static constexpr uint8_t SD_CMD_SD_SEND_OP_COND     = 41U; // ACMD41
    static constexpr uint8_t SD_CMD_SET_CLR_CARD_DETECT = 42U; // ACMD42
    static constexpr uint8_t SD_CMD_SEND_SCR            = 51U; // ACMD51

    // Mask for errors in card status value
    static constexpr uint32_t SD_OCR_ALL_ERRORS         = 0xFDFFE008U; // All possible error bits
    static constexpr uint32_t SD_OCR_OUT_OF_RANGE       = 0x80000000U; // The command's argument was out of allowed range
    static constexpr uint32_t SD_OCR_ADDRESS_ERROR      = 0x40000000U; // A misaligned address used in the command
    static constexpr uint32_t SD_OCR_BLOCK_LEN_ERROR    = 0x20000000U; // The transfer block length is not allowed for this card
    static constexpr uint32_t SD_OCR_ERASE_SEQ_ERROR    = 0x10000000U; // An error in the sequence of erase commands occurred
    static constexpr uint32_t SD_OCR_ERASE_PARAM        = 0x08000000U; // An invalid selection of write-blocks for erase occurred
    static constexpr uint32_t SD_OCR_WP_VIOLATION       = 0x04000000U; // Attempt to write to a protected block or to the write protected card
    static constexpr uint32_t SD_OCR_LOCK_UNLOCK_FAILED = 0x01000000U; // Sequence or password error in lock/unlock card command
    static constexpr uint32_t SD_OCR_COM_CRC_ERROR      = 0x00800000U; // The CRC check of the previous command failed
    static constexpr uint32_t SD_OCR_ILLEGAL_COMMAND    = 0x00400000U; // Command not legal for the card state
    static constexpr uint32_t SD_OCR_CARD_ECC_FAILED    = 0x00200000U; // Card internal ECC was applied but failed to correct the data
    static constexpr uint32_t SD_OCR_CC_ERROR           = 0x00100000U; // Internal card controller error
    static constexpr uint32_t SD_OCR_ERROR              = 0x00080000U; // A general or an unknown error occurred during the operation
    static constexpr uint32_t SD_OCR_STREAM_R_UNDERRUN  = 0x00040000U; // The card could not sustain data transfer in stream read operation
    static constexpr uint32_t SD_OCR_STREAM_W_OVERRUN   = 0x00020000U; // The card could not sustain data programming in stream mode
    static constexpr uint32_t SD_OCR_CSD_OVERWRITE      = 0x00010000U; // CSD overwrite error
    static constexpr uint32_t SD_OCR_WP_ERASE_SKIP      = 0x00008000U; // Only partial address space was erased
    static constexpr uint32_t SD_OCR_CARD_ECC_DISABLED  = 0x00004000U; // The command has been executed without using the internal ECC
    static constexpr uint32_t SD_OCR_ERASE_RESET        = 0x00002000U; // An erase sequence was cleared before executing
    static constexpr uint32_t SD_OCR_AKE_SEQ_ERROR      = 0x00000008U; // Error in the sequence of the authentication process        
    
    // Pattern for R6 response
    static constexpr uint32_t SD_CHECK_PATTERN = 0x000001AAU;
    // R6 response error bits
    static constexpr uint32_t SD_R6_GENERAL_UNKNOWN_ERROR = 0x00002000U;
    static constexpr uint32_t SD_R6_ILLEGAL_CMD           = 0x00004000U;
    static constexpr uint32_t SD_R6_COM_CRC_FAILED        = 0x00008000U;

    // Trials count for ACMD41
    static constexpr uint32_t SD_ACMD41_TRIALS = 0x0000FFFFU;
    // Argument for ACMD41 to select voltage window
    static constexpr uint32_t SD_OCR_VOLTAGE = 0x80100000U;

    // SD commands  index
    static constexpr uint8_t SD_CMD_GO_IDLE_STATE        = 0U;
    static constexpr uint8_t SD_CMD_SEND_OP_COND         = 1U;  // MMC only
    static constexpr uint8_t SD_CMD_ALL_SEND_CID         = 2U;  // Not supported in SPI mode
    static constexpr uint8_t SD_CMD_SEND_REL_ADDR        = 3U;  // Not supported in SPI mode
    static constexpr uint8_t SD_CMD_SWITCH_FUNC          = 6U;
    static constexpr uint8_t SD_CMD_SEL_DESEL_CARD       = 7U;  // Not supported in SPI mode
    static constexpr uint8_t SD_CMD_HS_SEND_EXT_CSD      = 8U;
    static constexpr uint8_t SD_CMD_SEND_CSD             = 9U;
    static constexpr uint8_t SD_CMD_SEND_CID             = 10U;
    static constexpr uint8_t SD_CMD_READ_DAT_UNTIL_STOP  = 11U; // Not supported in SPI mode
    static constexpr uint8_t SD_CMD_STOP_TRANSMISSION    = 12U;
    static constexpr uint8_t SD_CMD_SEND_STATUS          = 13U;
    static constexpr uint8_t SD_CMD_GO_INACTIVE_STATE    = 15U; // Not supported in SPI mode
    static constexpr uint8_t SD_CMD_SET_BLOCKLEN         = 16U;
    static constexpr uint8_t SD_CMD_READ_SINGLE_BLOCK    = 17U;
    static constexpr uint8_t SD_CMD_READ_MULT_BLOCK      = 18U;
    static constexpr uint8_t SD_CMD_WRITE_DAT_UNTIL_STOP = 20U; // Not supported in SPI mode
    static constexpr uint8_t SD_CMD_WRITE_BLOCK          = 24U;
    static constexpr uint8_t SD_CMD_WRITE_MULTIPLE_BLOCK = 25U;
    static constexpr uint8_t SD_CMD_PROG_CSD             = 27U;
    static constexpr uint8_t SD_CMD_SET_WRITE_PROT       = 28U; // Not supported in SPI mode
    static constexpr uint8_t SD_CMD_CLR_WRITE_PROT       = 29U; // Not supported in SPI mode
    static constexpr uint8_t SD_CMD_SEND_WRITE_PROT      = 30U; // Not supported in SPI mode
    static constexpr uint8_t SD_CMD_SD_ERASE_GRP_START   = 32U; // To set the address of the first write block to be erased. 
    static constexpr uint8_t SD_CMD_SD_ERASE_GRP_END     = 33U; //set the address of the last write block of the continuous range to be erased
    static constexpr uint8_t SD_CMD_UNTAG_SECTOR         = 34U;
    //static constexpr uint8_t SD_CMD_ERASE_GRP_START      = 35U; // for MMC
    //static constexpr uint8_t SD_CMD_ERASE_GRP_END        = 36U; // for MMC
    static constexpr uint8_t SD_CMD_UNTAG_ERASE_GROUP    = 37U;    
    static constexpr uint8_t SD_CMD_ERASE                = 38U;
    static constexpr uint8_t SD_CMD_LOCK_UNLOCK          = 42U;
    static constexpr uint8_t SD_CMD_APP_CMD              = 55U; // 
    static constexpr uint8_t SD_CMD_READ_OCR             = 58U; // Read OCR register
    static constexpr uint8_t SD_CMD_CRC_ON_OFF           = 59U; // On/Off CRC check by SD Card (in SPI mode)
    // SDIO CMD response type
    static constexpr uint32_t SD_RESP_NONE  = 0x00000000U; // No response
    static constexpr uint32_t SD_RESP_SHORT = SDMMC_CMD_WAITRESP_0;  // Short response
    static constexpr uint32_t SD_RESP_LONG  = SDMMC_CMD_WAITRESP;    // Long response
    // RCA for the MMC card
    static constexpr uint16_t SDIO_MMC_RCA = 0x0001U;

    // SDIO bus width
    static constexpr uint32_t SD_BUS_1BIT = 0x00000000U; // 1-bit wide bus (SDIO_D0 used)
    static constexpr uint32_t SD_BUS_4BIT = SDMMC_CLKCR_WIDBUS_0;  // 4-bit wide bus (SDIO_D[3:0] used)
    static constexpr uint32_t SD_BUS_8BIT = SDMMC_CLKCR_WIDBUS_1;  // 8-bit wide bus (SDIO_D[7:0] used)
    // SDIO transfer flags
    static constexpr uint32_t SDIO_XFER_COMMON_FLAGS = (SDMMC_STA_DTIMEOUT | SDMMC_STA_DCRCFAIL);
    // SDIO flags for single block receive
    static constexpr uint32_t SDIO_RX_SB_FLAGS =       (SDIO_XFER_COMMON_FLAGS | SDMMC_STA_DBCKEND | SDMMC_STA_DATAEND | SDMMC_STA_RXOVERR);
    // SDIO flags for multiple block receive
    static constexpr uint32_t SDIO_RX_MB_FLAGS =       (SDIO_XFER_COMMON_FLAGS | SDMMC_STA_DATAEND | SDMMC_STA_RXOVERR);
    // SDIO flags for single block transmit
    static constexpr uint32_t SDIO_TX_SB_FLAGS =       (SDIO_XFER_COMMON_FLAGS | SDMMC_STA_TXUNDERR | SDMMC_STA_DATAEND);
    // SDIO flags for multiple block transmit
    static constexpr uint32_t SDIO_TX_MB_FLAGS =       (SDIO_XFER_COMMON_FLAGS | SDMMC_STA_DBCKEND | SDMMC_STA_TXUNDERR);
    // SDIO transfer error flags
    static constexpr uint32_t SDIO_XFER_ERROR_FLAGS =  (SDIO_XFER_COMMON_FLAGS | SDMMC_STA_TXUNDERR | SDMMC_STA_RXOVERR);

    // Card state (OCR[12:9] bits CURRENT_STATE)
    static constexpr uint8_t SD_STATE_IDLE  = 0x00U; // Idle
    static constexpr uint8_t SD_STATE_READY = 0x01U; // Ready
    static constexpr uint8_t SD_STATE_IDENT = 0x02U; // Identification
    static constexpr uint8_t SD_STATE_STBY  = 0x03U; // Stand-by
    static constexpr uint8_t SD_STATE_TRAN  = 0x04U; // Transfer
    static constexpr uint8_t SD_STATE_DATA  = 0x05U; // Sending data
    static constexpr uint8_t SD_STATE_RCV   = 0x06U; // Receive data
    static constexpr uint8_t SD_STATE_PRG   = 0x07U; // Programming
    static constexpr uint8_t SD_STATE_DIS   = 0x08U; // Disconnect
    static constexpr uint8_t SD_STATE_ERROR = 0xFFU; // Error or unknown state
};

#endif //SD_H