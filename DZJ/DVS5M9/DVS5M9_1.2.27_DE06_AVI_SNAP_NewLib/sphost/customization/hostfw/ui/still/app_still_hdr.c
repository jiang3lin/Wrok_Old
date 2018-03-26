/**************************************************************************
 *
 *        Copyright (c) 2006-2008 by Sunplus mMedia Inc., Ltd.
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
 *  Hsin-Chu, Taiwan, R.O.C. 
 *
 **************************************************************************/
#include <stdio.h>
#include <string.h>
#include "api/sp5k_global_api.h"
#include "api/sp5k_msg_def.h"
#include "api/sp5k_os_api.h"
#include "api/sp5k_modesw_api.h"
#include "api/sp5k_capture_api.h"
#include "api/sp5k_ae_api.h"
#include "api/sp5k_utility_api.h"
	  


#include "app_com_def.h"
#include "app_com_api.h"
#include "app_still_hdr.h"

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
static UINT8 *pBufHdr1,*pBufHdr2;
static UINT8 *qvbuf;
static UINT8 hdrCapNumer;

#define LOGI_CACH_BADDR_TO_LOGI_UNCACH_BADDR(addr)  ((void *)((UINT32)(addr) | 0x20000000))
void cpuDCacheFlush(void *addrstart, UINT32 size);

void yuvScale(
	UINT8 *srcbuf, UINT32 yuvw, UINT32 yuvh,
	UINT8 *dstbuf, UINT32 qvw, UINT32 qvh
)
{
	sp5kYuvScale_t s;
 
	srcbuf = LOGI_CACH_BADDR_TO_LOGI_UNCACH_BADDR(srcbuf);
	dstbuf = LOGI_CACH_BADDR_TO_LOGI_UNCACH_BADDR(dstbuf);
	cpuDCacheFlush(srcbuf, yuvw*yuvh*2);
 
	s.psrcAddr = srcbuf;   
	s.pdstAddr = LOGI_CACH_BADDR_TO_LOGI_UNCACH_BADDR(dstbuf);   
	s.srcWidth = yuvw;   
	s.srcHeight = yuvh;  
	s.dstWidth = qvw;   
	s.dstHeight = qvh;  
	s.cropWidth = yuvw;  
	s.cropHeight = yuvh; 
	s.cropOffX = 0;   
	s.cropOffY = 0;   
 
	sp5kUtilYuvScale(&s, SP5K_SCALE_FORMAT_YUV);	
}
UINT32 apphdrCapNumberGet(void){
	return hdrCapNumer;
} 
UINT32 apphdrYuvCallback(UINT32 yuvAddr,UINT32 qvgaAddr,void * psSizeInfo)
{	
	static UINT32 size;
	UINT32 qvw=320, qvh=240, ret=0;
	sp5kImgSize_t *pYuvSize=(sp5kImgSize_t *)psSizeInfo;
	UINT32 idx;
	UINT32 hdrComposeResult;
	/*
	to have better running time,
	it is advised not to allocate consecutive memory space for the 3 normal, under, and over buffers.
	it is suggested that the 3 buffers have some gaps between (gaps in multiples of 16 bytes, for example, 13x16 bytes).
	for examples, you may want to allocate "2048x1536x2 + gap" 2 times, or
	"(2048x1536x2 + gap) x2" at once.
	the reason is a 3M image frame buffer takes 2048x1536x2 bytes, which is a multiple of DCACHE size,
	and which means the same pixels in different frame buffers would share the same DCACHE line aliases
	if there is no gap between frame buffers.
	if no gap, accessing the same pixels in different buffers would kick pixel data in and out intensively and
	results in very slow processing.
	*/
	UINT32 gap = 13*16;
 
	if (hdrCapNumer>=3) {
		hdrCapNumer=0;	
	}	
 
 	printf("%s: hdrCapNumer = %d\n", __FUNCTION__, hdrCapNumer);
	idx =  hdrCapNumer;  
	switch(idx) {
	case 0:
		qvbuf = sp5kMalloc(qvw*qvh*2);		
		size = pYuvSize->cropWidth*pYuvSize->cropHeight*2 + gap;
 
		if (qvbuf) {				
			yuvScale((UINT8 *)yuvAddr, pYuvSize->cropWidth, pYuvSize->cropHeight, qvbuf, qvw, qvh);
			sp5kStillCapPbBufDisplay(qvbuf);		
		}
		else {
			printf("%s: no qv space", __FUNCTION__);
		}		
		pBufHdr1 = sp5kMalloc(size);
		if (pBufHdr1) {
			memcpy(pBufHdr1, (UINT8*)yuvAddr, size);
			sp5kUtilYuvFilter(SP5K_YUV_FILTER_HDR_MARK_UNDER, 0, pBufHdr1, pYuvSize->cropWidth, pYuvSize->cropHeight);
		}
		else {
			printf("%s: no hdr 1 space", __FUNCTION__);
		}		
		ret = 1;
		break;
 
	case 1:		
		if (qvbuf) {			
			yuvScale((UINT8 *)yuvAddr, pYuvSize->cropWidth, pYuvSize->cropHeight, qvbuf, qvw, qvh);
			sp5kStillCapPbBufDisplay(qvbuf);			
		}		
		pBufHdr2 = sp5kMalloc(size);
		if (pBufHdr2) {
			memcpy(pBufHdr2, (UINT8*)yuvAddr, size);
			sp5kUtilYuvFilter(SP5K_YUV_FILTER_HDR_MARK_NORMAL, 0, pBufHdr2, pYuvSize->cropWidth, pYuvSize->cropHeight);
		}
		else {
			printf("%s: no hdr 2 space", __FUNCTION__);
		}
		ret = 1;
		break;
 
	case 2:		
		if (qvbuf) {
			yuvScale((UINT8 *)yuvAddr, pYuvSize->cropWidth, pYuvSize->cropHeight, qvbuf, qvw, qvh);		
			sp5kStillCapPbBufDisplay(qvbuf);
			sp5kStillCapPbBufDestory();			
			sp5kFree(qvbuf);
		}		
 
		sp5kUtilYuvFilter(SP5K_YUV_FILTER_HDR_MARK_OVER, 0, (UINT8*)yuvAddr, pYuvSize->cropWidth, pYuvSize->cropHeight);
		hdrComposeResult = sp5kUtilYuvFilter(SP5K_YUV_FILTER_HDR_COMPOSE, 0, (UINT8*)yuvAddr, pYuvSize->cropWidth, pYuvSize->cropHeight);		
		if (pBufHdr1) 
			sp5kFree(pBufHdr1);
		if (pBufHdr2)
			sp5kFree(pBufHdr2);
		if (hdrComposeResult==0) {
			ret = 0;	
		} 
		else {
			//printf("[warring] HDR_NO_SAVE_NO_QV = %d\n",hdrComposeResult);			
			sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 2500);
			sp5kHostMsgSend(SP5K_MSG_STILL_STORE_COMPLETE, 0);
			ret = 1;			
		}		
		break;
	}	
	hdrCapNumer++;
 
	printf("%s: ret = %d\n", __FUNCTION__, ret);
	return ret;
}
