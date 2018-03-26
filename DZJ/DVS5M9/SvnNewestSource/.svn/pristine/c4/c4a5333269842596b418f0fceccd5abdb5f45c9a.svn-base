/**************************************************************************
 *
 *      Copyright (c) 2005-2008 by Sunplus mMedia Inc., Ltd.
 *
 *  This software is copyrighted by and is the property of Sunplus
 *  mMedia Inc., Ltd. All rights are reserved by Sunplus mMedia
 *  Inc., Ltd. This software may only be used in accordance with the
 *  corresponding license agreement. Any unauthorized use, duplication,
 *  distribution, or disclosure of this software is expressly forbidden.
 *
 *  This Copyright notice MUST not be removed or modified without prior
 *  written consent of Sunplus mMedia Inc., Ltd.
 *
 *  Sunplus mMedia Inc., Ltd. reserves the right to modify this
 *  software without notice.
 *
 *  Sunplus mMedia Inc., Ltd.
 *  19-1, Innovation First Road, Science-Based Industrial Park,
 *  Hsin-Chu, Taiwan.
 *
 **************************************************************************/
#include "common.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "api/sp5k_capture_api.h"
#include "api/sp5k_cdsp_api.h"
#include "api/sp5k_fs_api.h"
#include "app_Sys_Def.h"
#include "app_util.h"
#include "app_ChangeRaw.h"
//#include "ui_ViewMain.h"
//#include "ui_clbtlib.h"
//#include "zoom.h"
#include "app_zoom_api.h"
#include "app_calib_api.h"
#include "app_view_osd.h"



/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/\

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define RAW_BATCH

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
void AppCaptureWbCb( UINT32 Addr, UINT32 RawFmt, void* ImgSize );

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/


rawbatch_t rawCb;
static UINT8 *RawBatch;
static UINT32 RawBatchPos,RawBatchPos0,RawBatchSize;

static UINT32 RawBatchTokenGet(UINT8 *buf, UINT32 bufsize)
{
	UINT32 i,l=0;
	if (RawBatchPos>=RawBatchSize)
		return FAIL;
	for (i=RawBatchPos ; i<RawBatchSize ; i++) {
		switch (RawBatch[i]) {
		case 10: case 13: case 9: case ' ':
			if (l) goto Exit_RawBatchTokenGet;
			continue;
			break;
		default:
			buf[l++] = RawBatch[i];
			if (l>=bufsize) goto Exit_RawBatchTokenGet;
			break;
		}
	}
Exit_RawBatchTokenGet:
	if (l+1<=bufsize)
		buf[l] = 0;
	RawBatchPos0 = RawBatchPos;
	RawBatchPos = i;
	return RawBatchPos<=RawBatchSize && l+1<=bufsize ? SUCCESS : FAIL;
}

static void RawBatchTokenGetUndo()
{
	RawBatchPos = RawBatchPos0;
}

static UINT32 RawBatchTokenU16Get(UINT16 *val)
{
	UINT8 buf[8];
	if (RawBatchTokenGet(buf, sizeof(buf))!=SUCCESS) return FAIL;
	*val = atoi(buf);
	return SUCCESS;
}

static UINT32 RawBatchEntryGet(rawbatch_t *pRawCb)
{
	UINT8 buf[256];
	if (RawBatchTokenGet(pRawCb->file, 256)!=SUCCESS) return FAIL;
	
	while (RawBatchTokenGet(buf, sizeof(buf))==SUCCESS) {
		if (strcmp(buf, "wb")==0) {
			if (RawBatchTokenU16Get(&(pRawCb->wb.rgain))!=SUCCESS) return FAIL;
			if (RawBatchTokenU16Get(&(pRawCb->wb.grgain))!=SUCCESS) return FAIL;
			if (RawBatchTokenU16Get(&(pRawCb->wb.bgain))!=SUCCESS) return FAIL;
			if (RawBatchTokenU16Get(&(pRawCb->wb.gbgain))!=SUCCESS) return FAIL;
			pRawCb->wbEn = 1;
		} else if (strcmp(buf, "rawinfo")==0) {
			if (RawBatchTokenU16Get(&(pRawCb->raw.width))!=SUCCESS) return FAIL;
			if (RawBatchTokenU16Get(&(pRawCb->raw.height))!=SUCCESS) return FAIL;
			if (RawBatchTokenU16Get(&(pRawCb->raw.bit))!=SUCCESS) return FAIL;
			if (RawBatchTokenU16Get(&(pRawCb->raw.format))!=SUCCESS) return FAIL;
		} else if (strcmp(buf, "iso")==0) {
			if (RawBatchTokenU16Get(&(pRawCb->iso))!=SUCCESS) return FAIL;
			pRawCb->isoEn = 1;
		} else if (strcmp(buf, "tv")==0) {
			if (RawBatchTokenU16Get(&(pRawCb->tv))!=SUCCESS) return FAIL;
			pRawCb->tvEn = 1;
		} else if (strcmp(buf, "zoom")==0) {
			if (RawBatchTokenU16Get(&(pRawCb->zoom))!=SUCCESS) return FAIL;
			pRawCb->zoomEn = 1;
		} else if (strcmp(buf, "dzoom")==0) {
			if (RawBatchTokenU16Get(&(pRawCb->dzoom))!=SUCCESS) return FAIL;
			pRawCb->dzoomEn = 1;
		} else if (strcmp(buf, "size")==0) {
			if (RawBatchTokenU16Get(&(pRawCb->size))!=SUCCESS) return FAIL;
			pRawCb->sizeEn = 1;
		} else if (strcmp(buf, "sharp")==0) {
			if (RawBatchTokenU16Get(&(pRawCb->sharp))!=SUCCESS) return FAIL;
			pRawCb->sharpEn = 1;
		} else if (strcmp(buf, "lv")==0) {
			if (RawBatchTokenU16Get(&(pRawCb->lv))!=SUCCESS) return FAIL;
			pRawCb->lvEn = 1;
		} else {
			RawBatchTokenGetUndo();
			break;
		}
	}
	return SUCCESS;
}

void RawBatchCheck()
{
	static BOOL fRawBatchInited=0;
	/*
	if (CLBT_GetRAWType() != 0x01 && CLBT_GetRAWType() != 0x02)
		return;
	*/
	if (!fRawBatchInited) {
		UINT8 *p;
		fRawBatchInited = 1;
		printf("Read batch file %s\n", "D:\\RAWBATCH.TXT");
		ReadFileEx("D:\\RAWBATCH.TXT", &p, (UINT32*)&RawBatchSize);
		RawBatch = p;
		RawBatchPos0 = RawBatchPos = 0;
		printf("@#$ RawBatch mode, size=%d\n", RawBatchSize);
	}
	if (RawBatchSize) {
		if (RawBatchEntryGet(&rawCb)==FAIL) {
			printf("@#$ RawBatch end: batch infornmation not complete\n");
			sp5kFree(RawBatch);
			memset(&rawCb, 0, sizeof(rawCb));
			RawBatchSize = 0;
			return;
		} else {	
			printf("Batch info:\n");
			printf("Batch info:filename %s\n", rawCb.file);
			printf("Batch info:raw size %d\n", rawCb.size);
			printf("Batch info:width  %d\n", rawCb.raw.width);
			printf("Batch info:height %d\n", rawCb.raw.height);
			printf("Batch info:precision %d\n", rawCb.raw.bit);
			printf("Batch info:format %d\n", rawCb.raw.format);
            printf("Batch info:iso %d\n", rawCb.iso);
            printf("Batch info:tv %d\n", rawCb.tv);
            printf("Batch info:rgain %d\n", rawCb.wb.rgain);
            printf("Batch info:grgain %d\n", rawCb.wb.grgain);
            printf("Batch info:bgain %d\n", rawCb.wb.bgain);
            printf("Batch info:gbgain %d\n", rawCb.wb.gbgain);
		}
	}
}



void iqGetEv(INT32 *CTvEntry, INT32 *CAvEntry, INT32 *ISOValue)
{
	
	*CTvEntry = rawCb.tv;
	*ISOValue = rawCb.iso;
	
}

UINT32 iqGetZoom()
{
	
		return rawCb.zoom;
	
}

UINT32 iqGetDZoom()
{
	
		return rawCb.dzoom;
	
}

UINT32 iqGetSize()
{
	
		return rawCb.size;
	
}

UINT32 iqGetSharp()
{
	
		return rawCb.sharp;
	
}

UINT32 iqGetLv()
{
	
		return rawCb.lv;
	
}


UINT32 IqChangeRaw(UINT32 Addr, UINT32 RawFmt, void* pImgSize)
{
	UINT16 *ImageAddress=(UINT16*)Addr;
	UINT32 bufw,bufh,bufcfa,imgw=0,imgh=0,imgcfa=0;
	UINT32 fd,fl;
	sp5kImgWbInfo_t *info=(sp5kImgWbInfo_t*)pImgSize;
	char fname[32];
	//static UINT32 id=1;

	#if 0 // raw pattern gen
	memset(ImageAddress, 0x33, info->width*info->height*2);
	int i;
	for (i=8 ; i<72 ; i+=16) {
		memset(ImageAddress+info->width*i, 0, info->width*4);
		memset(ImageAddress+info->width*(info->height-i), 0, info->width*4);
		memset(ImageAddress+info->width*(i+8), 0xff, info->width*4);
		memset(ImageAddress+info->width*(info->height-i-8), 0xff, info->width*4);
	}
	sp5kWbGain_t wb={0x40,0x40,0x40,0x40};
	sp5kImgWbGainSet(&wb);
	return 1;
	#endif
	/*
	if (CLBT_GetRAWType() == 0x02) {				//Multi-change RAW use 0x02
		sprintf(fname, "D:\\ISO%04d.RAW", id++);
 	} else { 										//Single-change RAW use 0x01
		strcpy(fname, "D:\\SGCHART.RAW");
 	}
 	*/
 	RawBatchCheck();
	
 	strcpy(fname, "D:\\SGCHART.RAW");
	
	if (rawCb.file[0]) {
		strcpy(fname, rawCb.file);
		rawCb.file[0] = 0;
	}
	
 	printf("RawChange:%s ...", fname);
	fd = sp5kFsFileOpen(fname, SP5K_FS_OPEN_RDONLY); 
	printf("%s\n", fd ? "OK" : "NG");
	
	if (fd==0) return 0;
	fl = sp5kFsFileSizeGet(fd);
	bufw = info->width;
	bufh = info->height;
	bufcfa = info->bayerOrder;

	
	if (rawCb.raw.bit==16) {
		imgw = rawCb.raw.width;
		imgh = rawCb.raw.height;
		imgcfa = rawCb.raw.format;
	} else

	
	if (bufw*bufh*2==fl) {
		imgw = bufw; imgh = bufh;
	}
	//printf("%dx%dx%d %x\n%dx%dx16 %x\n", imgw, imgh, rawCb.raw.bit, imgcfa, bufw, bufh, bufcfa); 
	if (imgw>0 && imgh>0) {
		if (bufw==imgw && bufh==imgh) {
			UINT32 ofs=0;
			if ((imgcfa^bufcfa)&1) ofs += 2;
			if ((imgcfa^bufcfa)&2) ofs += imgw*2;
			if (ofs) sp5kFsFileSeek(fd, ofs, SP5K_FS_SEEK_SET);
			sp5kFsFileRead(fd, (UINT8*)ImageAddress, imgw*imgh*2-ofs);
		} else {
			UINT16 *src,*dst;
			UINT32 y;
			src = ImageAddress + (bufw*bufh - imgw*imgh)*2;
			dst = ImageAddress;
			sp5kFsFileRead(fd, (UINT8*)src, imgw*imgh*2);
			for (y=0 ; y<imgh ; y++) {
				memcpy(dst, src, imgw*2);
				src += imgw;
				dst += bufw;
			}
		}
	}
	sp5kFsFileClose(fd);
	//memset(ImageAddress, 0x40, bufw*bufh*2);
	
	if (rawCb.wbEn) {
		//rawCb.wb.rgain = rawCb.wb.grgain = rawCb.wb.gbgain = rawCb.wb.bgain = 0x40;
		profLogPrintf(0, "@#$ RawBatch WB:%d %d/%d %d\n", rawCb.wb.rgain, rawCb.wb.grgain, rawCb.wb.gbgain, rawCb.wb.bgain);
		sp5kImgModeSet(SP5K_MODE_STILL_SNAP);
		sp5kImgWbGainSet(&rawCb.wb);
		sp5kImgModeSetDone(SP5K_MODE_STILL_SNAP);
		rawCb.wbEn = 0;
	}
	
	if (RawBatchSize==0) {
		memset(&rawCb, 0, sizeof(rawCb));
		printf("RawBatch: no parameters been set!! batchsize = 0\n");
		return 0;
	}

    profLogPrintf(0, "__IqChangeRaw__ end\n");
	return 1;
}

