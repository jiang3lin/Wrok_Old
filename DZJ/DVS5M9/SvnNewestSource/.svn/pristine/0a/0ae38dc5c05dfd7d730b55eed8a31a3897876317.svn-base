/**************************************************************************
 *                                                                        *
 *         Copyright (c) 2012 by Icatch Technology Co., Ltd.             *
 *                                                                        *
 *  This software is copyrighted by and is the property of Icatch        *
 *  Technology Co., Ltd. All rights are reserved by Icatch Technology    *
 *  Co., Ltd. This software may only be used in accordance with the       *
 *  corresponding license agreement. Any unauthorized use, duplication,   *
 *  distribution, or disclosure of this software is expressly forbidden.  *
 *                                                                        *
 *  This Copyright notice MUST not be removed or modified without prior   *
 *  written consent of Sunplus Technology Co., Ltd.                       *
 *                                                                        *
 *  Icatch Technology Co., Ltd. reserves the right to modify this        *
 *  software without notice.                                              *
 *                                                                        *
 *  Icatch Technology Co., Ltd.                                          *
 *  19, Innovation First Road, Science-Based Industrial Park,             *
 *  Hsin-Chu, Taiwan, R.O.C.                                              *
 *                                                                        *
 *  Author:                                                               *
 *                                                                        *
 **************************************************************************/
/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/
#include "usbotg_init.h"
#include "debug_mod/dbg_api.h"
#include "customization/dev_init.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define DISKID 3    /*the ID "3" , which was point Disk E:*/
#define DMA_READ 1  /*DMA Read mode*/
#define DMA_WRITE 0 /*DMA Write mode*/
struct spiOp spiOpFun = {
	sampleSpiRead,			/* page read */
	sampleSpiWrite,			/* page write */
	sampleSpiSectorErase,	/* sector erase */
	sampleSpiChipErase,		/* chip erase */
};
typedef struct spiOp spiOp_t;

#if 1
typedef struct devInfo_s {
	UINT8  makerId;
	UINT8  hwDevId;
	UINT8  active;      /* Inserted: 1, pulled-out: 0           */
	UINT16 megaByte;    /* Device capacity in unit of MB        */
	UINT8* pdevName;    /* ASCII name of this device            */
	UINT32 nrSects;     /* How many sectors of this device      */
	UINT64 nrFreeBytes; /* Free space in byte                   */
	UINT32 fsDepend;    /* For FAT, fsDepend = byte per cluster */
	UINT32 clock;       /* Flash operating clock                */
	UINT32 busWidth;    /* Number of bits of bus                */
    UINT32 ucFsType;    /* fs type                              */
} devInfo_t;
#endif

typedef struct spiDev_s {
	devInfo_t common; /* Common information for all cards */
	UINT32  id;
	UINT8   string[16];
	UINT32  blkSize;
	UINT32  tolBlk;
	UINT8   bufBlk;
	UINT8   eraseCmd;
	UINT8   bitPagePerBlk;
	UINT8   bitBlkSize;
	UINT32  pageSize;
	spiOp_t *op;
	UINT8 *pbuf;
	UINT8 *pblkBuf;
	UINT16 nrRsvBlks;
	UINT16 rsvBlkOffset;
	UINT32 nrBlks;
	UINT8  bus;
} SampleSpiDev_t;

#if ( SP5K_DISK_OTG == 1 )
SampleSpiDev_t SampleSpiDev;

UINT32 sampleSpiRdId(
    void
)
{
	UINT8 id[4];
	UINT32 ID;
	/*Enable SPI Interface, 
	 you must do it before you start spi signal control flow
	 and the function will lock SPI interface
	 So , don't enable SPI interface twice before disable Interface*/
	spiIfEn(1);
	
	spiClk(SPI12MHZ);/*Set SPI Clock*/
	spiCe(0);/* set SPI CE Pin Low */

	/*Write one byte data into Bus*/
	spiByteWrite(0x9F);  /*Write SPI ID Command 0x9F by PIO Mode, You can see it as writing one byte data "0x9F"*/
	/*Read one byte data from Bus*/
	id[0] = spiByteRead();      /* get Manufacturer's  ID *//*PIO Mode*//*Read one byte data from Bus*/
	id[1] = spiByteRead();      /* get Device ID          *//*PIO Mode*//*Read one byte data from Bus*/
	id[2] = spiByteRead();		/* get Manufacturer's  ID *//*PIO Mode*//*Read one byte data from Bus*/
	spiCe(1);/* set SPI CE Pin High */
   	printf("(Sample)ID %2X %2X %2X\n", id[0], id[1], id[2]);
	ID = ((UINT32)id[0] << 16) | ((UINT32)id[1] << 8) | ((UINT32)id[2] << 0);
	
	/*Disable SPI Interface
	 you must do it after you finish your spi signal control flow
	 and it will release SPI Interface lock*/
	spiIfEn(0);
	return ID;
}

void sampleSpiTimeOutWait(
	UINT32 poll,
	UINT32 *ptimeOut
)
{
	UINT32 sts;
	UINT32 time = 0;
	while (1) {
		spiCe(0); /* set SPI CE Pin Low */
		spiByteWrite(0x05);/*Write SPI Command 0x05 by PIO Mode, You can see it as writing one byte data "0x05"*/
		sts = spiByteRead(); /*Read One Bytes*//*PIO*/
		spiCe(1); /* set SPI CE Pin High */
		if (!(sts & poll)) {
			break;
		}
		time ++;
		if( *ptimeOut < time ){
			printf("ERR %s %d, TimeOut %d, sts=0x%x, poll=0x%x\n",__FUNCTION__,__LINE__,time,sts,poll);
			break;
		}
	}
}

UINT32 sampleSpiChipErase(void)
{	
	UINT32 timeout;
	UINT32 blk;
	UINT32 Addr;

	spiIfEn(1);
	for(blk=0;blk<64;blk++){
		Addr = blk<<SampleSpiDev.bitBlkSize;
#if 0
		spiStsRegWrite(0x02);
#else
		spiCe(0);/* set SPI CE Pin Low */
		spiByteWrite(1);/*PIO*//*Write Status register command*/
		spiByteWrite(2);/*PIO*//*Write data*/
		spiCe(1);/* set SPI CE Pin High */
#endif
		spiCe(0);/* set SPI CE Pin Low */
		spiByteWrite(6); /*Write SPI Command 0x06 by PIO Mode, You can see it as writing one byte data "0x06"*/
		spiCe(1);/* set SPI CE Pin High */
		spiCe(0);/* set SPI CE Pin Low */
		spiByteWrite(0xD8); /*Write SPI Command 0xD8 by PIO Mode, You can see it as writing one byte data "0xD8"*/
		spiByteWrite(Addr >> 16);/*Write SPI address by PIO Mode, You can see it as writing one byte data into SPI bus*/
		spiByteWrite(Addr >> 8);/*Write SPI address by PIO Mode, You can see it as writing one byte data into SPI bus*/
		spiByteWrite(Addr);	/*Write SPI address by PIO Mode, You can see it as writing one byte data into SPI bus*/
		spiCe(1);/* set SPI CE Pin High */
		timeout = 500000;
		sampleSpiTimeOutWait(0x01, &timeout);
	}
	spiIfEn(0);
	return SUCCESS;
}

UINT32 sampleSpiSectorErase(UINT32 blk)
{
	UINT32 timeout;
	spiIfEn(1);
	blk = blk << 8;
#if 0
	spiStsRegWrite(0x02);
#else
	spiCe(0);/* set SPI CE Pin Low */
	spiByteWrite(1);/*PIO*//*Write Status register command*/
	spiByteWrite(2);/*PIO*//*Write data*/
	spiCe(1);/* set SPI CE Pin High */
#endif
	spiCe(0);/* set SPI CE Pin Low */
	spiByteWrite(6);/*Write SPI Command 0x06 by PIO Mode, You can see it as writing one byte data "0x06"*/
	spiCe(1);/* set SPI CE Pin High */
	spiCe(0);/* set SPI CE Pin Low */
	spiByteWrite(0x20);/*Write SPI Command 0x20 by PIO Mode, You can see it as writing one byte data "0x20"*/
	spiByteWrite(blk >> 16);/*Write SPI address by PIO Mode, You can see it as writing one byte data into SPI bus*/
	spiByteWrite(blk >> 8);	/*Write SPI address by PIO Mode, You can see it as writing one byte data into SPI bus*/
	spiByteWrite(blk);/*Write SPI address by PIO Mode, You can see it as writing one byte data into SPI bus*/
	spiCe(1);/* set SPI CE Pin High */
	timeout = 500000;
	sampleSpiTimeOutWait(0x01, &timeout);
	spiIfEn(0);
	return SUCCESS;
}

UINT32
sampleSpiWrite(
	UINT32 addr,
	UINT32 pages,
	UINT8 *pbuf
)
{
	UINT32 err = SUCCESS;
	UINT32 timeout;
	UINT32 pageSize = 0x100;
	UINT8 *p;
	p = pbuf;
	addr = addr<<8;
	spiIfEn(1);
	while( pages ){
		spiCe(0);/* set SPI CE Pin Low */
		spiByteWrite(6);/*PIO*//*Write command*/
		spiCe(1);/* set SPI CE Pin High */

		spiCe(0);/* set SPI CE Pin Low */
	    spiByteWrite(0x02);/*Write SPI Command 0x02 by PIO Mode, You can see it as writing one byte data "0x02"*/
		spiByteWrite((UINT8)(addr >> 16));/*Write SPI address by PIO Mode, You can see it as writing one byte data into SPI bus*/
		spiByteWrite((UINT8)(addr >> 8));/*Write SPI address by PIO Mode, You can see it as writing one byte data into SPI bus*/
		spiByteWrite((UINT8)(addr));/*Write SPI address by PIO Mode, You can see it as writing one byte data into SPI bus*/
		/* p:DRAM memory adress
		 pageSize must be multiple of 16 Byte */
		halFMDMA0(DMA_WRITE,p,pageSize);/*DMA Mode*//*WRITE 16X Bytes data from "DRAM" to SPI*/
		p += pageSize;
		addr += pageSize;
		pages--;
		spiCe(1);/* set SPI CE Pin High */
		timeout = 1000000;
		sampleSpiTimeOutWait(0x01, &timeout);
	}
	spiIfEn(0);
	return err;
}
static UINT8 * psample_buf=0;

UINT32 sampleSpiW(
	void *pdev,
	UINT32 pageIdx,
	UINT32 nrPages,
	UINT8 *pdram
){
	UINT32 ret=SUCCESS;
	UINT32 Sector1,Sector2;
	UINT32 i;
	if(0==psample_buf)
		psample_buf = osMemAlloc(4096);
	if((pageIdx+nrPages)>8192){
		printf("!!!! Addr Fail !!!!\n");
		return 0;
	}
	pageIdx = pageIdx << 1;
	nrPages = nrPages << 1;
	ret = sampleSpiWrite(pageIdx,nrPages,pdram);

	if(SUCCESS == ret)
		return nrPages<<9;
	else return 0;

}
UINT32
sampleSpiRead(
	UINT32 addr,
	UINT32 pages,
	UINT8 *pbuf
)
{
	UINT32 err = SUCCESS;
	UINT32 pageSize = 256;
	UINT8 *buf;
	spiIfEn(1);
	buf = pbuf;
	addr = addr << 8;
	do{
		spiCe(0);/* set SPI CE Pin Low */
		spiByteWrite(0x03); /*Write SPI Command 0x03 by PIO Mode, You can see it as writing one byte data "0x03"*/
		spiByteWrite((UINT8)(addr >> 16));/*Write SPI address by PIO Mode, You can see it as writing one byte data into SPI bus*/
		spiByteWrite((UINT8)(addr >> 8));/*Write SPI address by PIO Mode, You can see it as writing one byte data into SPI bus*/
		spiByteWrite((UINT8)(addr));/*Write SPI address by PIO Mode, You can see it as writing one byte data into SPI bus*/
		/* puf:DRAM memory adress
		 pageSize must be multiple of 16 Byte */
		halFMDMA0(DMA_READ,buf,pageSize);/*DMA Mode*//*Read 16X Bytes data from Bus to "DRAM" */
		spiCe(1);/* set SPI CE Pin High */
		pages --;
		if(0==pages)
			break;
		addr = addr + pageSize;
		buf = buf + pageSize;
	}while(1);
	spiIfEn(0);
	return err;
}

UINT32 sampleSpiR(
	void *pdev,
	UINT32 pageIdx,
	UINT32 nrPages,
	UINT8 *pdram
){
	UINT32 ret;
	UINT32 loop;
	if((pageIdx+nrPages) > 8192){
		printf("!!!! Addr Fail !!!!\n");
		return 0;
	}
	ret = sampleSpiRead(pageIdx<<1,nrPages<<1,pdram);
	if(SUCCESS == ret)
		return nrPages<<9;
	else return 0;

}

UINT32
sampleSpiFormat(
	void *pdev,
	UINT32 opt
){
	sampleSpiChipErase();
}

void sampleSpiInt(void)
{
/* yout can change it by your case*/
	SampleSpiDev_t *pspi;
	pspi = &SampleSpiDev;
	pspi->blkSize = 4096;
	pspi->pageSize = 256;
	pspi->tolBlk = 1024;
	pspi->nrBlks = pspi->tolBlk;
	pspi->bitPagePerBlk = 4;
	pspi->bitBlkSize = 12;
	pspi->eraseCmd = 0x20;
	pspi->bufBlk = 6;
	if(pspi->pblkBuf){
		printf("Spi Block Buffer Release\n");
		osMemFree(pspi->pblkBuf);
	}
	pspi->pbuf = osMemAlloc(pspi->blkSize);

	if(pspi->pbuf){
		printf("Spi Buffer Release\n");
		osMemFree(pspi->pbuf);
	}
	pspi->pbuf = osMemAlloc(pspi->blkSize);

	printf("Device ,blk %d Byte,page %d Bytes, total Block %d ,bitPageBlk=%d,bitBlk=%d,bufBlk=%d,eraseCmd=0x%x\n"
	,pspi->blkSize,pspi->pageSize
	,pspi->tolBlk,pspi->bitPagePerBlk
	,pspi->bitBlkSize,pspi->bufBlk,pspi->eraseCmd);
	pspi->common.active = 1;
	pspi->common.pdevName = "SPI";
	pspi->common.nrSects = 8192;
	pspi->common.megaByte = pspi->common.nrSects / 2048;

	printf("Sample Spi megaByte= %d, nrSects=%d\n",pspi->common.megaByte, pspi->common.nrSects);
	SampleSpiDev.op = &spiOpFun;
}

void sampleSpiSetFunc(void)
{
	/*Please Keep the ID "3" , which point Disk E:*/
	/*you can change the function name 
		SampleSpiDev
		sampleSpiR
		sampleSpiW
		sampleSpiFormat
	*/
	SampleSpiDev.id = DISKID;
	vfsDevProfSet(DISKID, (void *)&SampleSpiDev);
	/*When Plug in , System will call the register function "otgDetect"
		Which is fixed for USB-OTG mode*/
	vfsDevInitFuncSet(DISKID, otgDetect);
	/*File system will call the register function "sampleSpiR"
		to run sector read*/
	vfsDevReadFuncSet(DISKID, sampleSpiR);
	/*File system will call the register function "sampleSpiW"
		to run sector write*/
	vfsDevWriteFuncSet(DISKID, sampleSpiW);
	/*File system will call the register function "sampleSpiFormat"
		to erase all data in disk 
		before write the data of file system*/
	vfsDevSpecFuncSet(DISKID, sampleSpiFormat);
}

UINT32 otgDetect(
	UINT32 drive
){

	SampleSpiDev_t * pspi =&SampleSpiDev;
	printf("\n*************** App OTG SPI interface initial V3 ***************\n\n");
	printf("Disk E sample code for 6330 SPI interface, Device SPI NS N25 S32\n");

	/*  You can inital SPI fuction by 
		sampleSpiRdId()
		sampleSpiInt()*/
	sampleSpiRdId();
	sampleSpiInt();
	
	/*Register function into file system funtion table*/
	sampleSpiSetFunc();
	
	/*vfsUnmount will unmout OTG Device to initial DEVICE*/
	vfsUnmount(DISKID);
	/*You must read UFD the data in physical Sector 0
	  Before you start run vfsMount() flow*/
	(SampleSpiDev.op->pageRead)(0, 2, pspi->pbuf);
	/*memdump(pspi->pbuf,0x200);*/
	vfsMount(DISKID, pspi->pbuf);
	printf("\n***************     SPI interface Mount Ending   ***************\n\n");
}

#endif
 
