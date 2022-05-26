//#include "pff.h"
#include "diskio.h"
#include "sd_spi.h"

/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/
/* Physical drive nmuber to identify the drive */
DSTATUS disk_initialize (void) {
	uint8_t res = SD::pThis->sd_init();
	if(res != STA_NOINIT){
		return 0; // already initialized
	} else return STA_NOINIT;	
}

/*-----------------------------------------------------------------------*/
/* Read partial sector                                                   */
/*-----------------------------------------------------------------------*/
DRESULT disk_readp (
	BYTE *buff,		/* Pointer to the read buffer (NULL:Read bytes are forwarded to the stream) */
	DWORD sector,	/* Sector number (LBA) */
	UINT offset,	/* Byte offset to read from (0..511) */
	UINT count		/* Number of bytes to read (ofs + cnt mus be <= 512) */
) {
	DRESULT res = RES_ERROR;
	res = (DRESULT)SD::pThis->read_sector(buff, sector, offset, count);
	return res;
}

/*-----------------------------------------------------------------------*/
/* Write partial sector                                                  */
/*-----------------------------------------------------------------------*/
DRESULT disk_writep (
	const BYTE *buff,	/* Pointer to the bytes to be written (NULL:Initiate/Finalize sector write) */
	DWORD sc			/* Number of bytes to send, Sector number (LBA) or zero */
)
{
	DRESULT res = RES_ERROR;
	res = (DRESULT)SD::pThis->write_sector(buff, sc);
	return res;
}