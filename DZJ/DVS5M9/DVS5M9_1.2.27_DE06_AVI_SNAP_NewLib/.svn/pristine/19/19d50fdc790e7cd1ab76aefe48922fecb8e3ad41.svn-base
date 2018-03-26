#ifndef _USBOTG_INIT_
#define _USBOTG_INIT_
#include "stdio.h"
#include "common.h"
#include "../include/os/vfs_api.h"

struct spiOp {
	UINT32 (*pageRead)(UINT32, UINT32, UINT8 *);
	UINT32 (*pageWrite)(UINT32, UINT32, UINT8 *);
	UINT32 (*sectorErase)(UINT32);
	UINT32 (*chipErase)(void);
};

typedef enum {
	SPI24MHZ   = 0,
	SPI12MHZ   = 1,
	SPI6MHZ    = 2,
	SPI3MHZ    = 3,
	SPI1500KHZ = 4,
	SPI750KHZ  = 5,
	SPI200KHZ  = 6,
	SPI48MHZ   = 7
} sdClkFre_t;

/*Set SPI Clock
Freq:
	SPI24MHZ :set 24MHz 
	.....
	SPI48MHZ :set 48MHz
*/
void spiClk(UINT32 Freq);

/*
High
1 :set SPI CE pin as High
0 :set SPI CE pin as Low 
*/
void spiCe(UINT32 High);

/*
	read one Byte from SPI bus by PIO mode
	which will Read the data
*/
UINT8 spiByteRead(void);

/*
	Write one Byte into SPI bus by PIO mode
	which will Read the data
*/
void spiByteWrite(UINT8 Byte);

/*
High
1 :Enable SPI Interface and lock DMA
0 :Disable SPI Interface and unlock DMA  
*/
void spiIfEn(UINT8 Enable);

/*sample code for initial*/
void sampleSpiInt(void);

/*unmout Disk Device and initial*/
void   vfsUnmount(UINT32 devId);

/*sample code for Read*/
UINT32 sampleSpiRead(UINT32 addr,UINT32 pages,UINT8 *pbuf);
/*sample code for Write*/
UINT32 sampleSpiWrite(UINT32 addr,UINT32 pages,UINT8 *pbuf);
/*sample code, You can set it as null funtion or remove it*/
UINT32 sampleSpiSectorErase(UINT32 blk);
/*sample code, You can set it as null funtion or remove it*/
UINT32 sampleSpiChipErase(void);
/*Register Function , you can rename it*/
UINT32 otgDetect(UINT32 drive);
/*Register Function , Yout can run it to open or reopen OTG Device*/
UINT32 diskOtgOpen(void);
#endif

