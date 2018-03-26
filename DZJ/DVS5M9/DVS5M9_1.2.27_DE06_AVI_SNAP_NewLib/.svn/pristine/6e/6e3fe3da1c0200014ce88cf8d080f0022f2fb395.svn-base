/*******************************************************************************
 *
 *       Copyright (c) 2009-2011 by iCatch Technology, Inc.
 *
 *  This software is copyrighted by and is the property of iCatch Technology,
 *  Inc. All rights are reserved by iCatch Technology, Inc.
 *  This software may only be used in accordance with the corresponding
 *  license agreement. Any unauthorized use, duplication, distribution,
 *  or disclosure of this software is expressly forbidden.
 *
 *  This Copyright notice MUST not be removed or modified without prior
 *  written consent of iCatch Technology, Inc.
 *
 *  iCatch Technology, Inc. reserves the right to modify this software
 *  without notice.
 *
 *  iCatch Technology, Inc.
 *  19-1, Innovation First Road, Science-Based Industrial Park,
 *  Hsin-Chu, Taiwan, R.O.C.
 *
 *  Author: Yi-Ming Kao
 *
 **************************************************************************/
/**
 * @file common_types.h
 * @brief The common used types in middleware.
 * @author Yi-Ming Kao
 */
#ifndef _COMMON_TYPES_H_
#define _COMMON_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************
 *                          D A T A    T Y P E S
 **************************************************************************/
typedef struct frameBufInfo_s {
	UINT8 *pbuf;
	UINT32 width;
	UINT32 height;
	UINT32 roiX;
	UINT32 roiY;
	UINT32 roiW;
	UINT32 roiH;
} frameBufInfo_t;

typedef struct yuvCbImgSize_s {
	UINT32 jpgWidth;  /* jpg target size */
	UINT32 jpgHeight;    
	UINT32 yuvWidth;  /* yuv buffer size */
	UINT32 yuvHeight;
	UINT32 roiX;     /* crop xy offset */
	UINT32 roiY;
	UINT32 roiWidth; /* crop size */
	UINT32 roiHeight;
} yuvCbImgSize_t;

typedef frameBufInfo_t yuvBufInfo_t;

typedef yuvCbImgSize_t sp5kYuvCbImgSize_t;

typedef struct tmx_s {
  	int tmx_sec;  /* seconds, 0-60 */
  	int tmx_min;  /* minutes, 0-59 */
  	int tmx_hour; /* hours, 0-23 */
  	int tmx_mday; /* day of the month, 1-31 */
 	int tmx_mon;  /* months since Jan, 1-12 <---------------------------(not 0-11) */
  	int tmx_year; /* years from 1900 */
  	int tmx_wday; /* days since Sunday, 0-6 */
  	int tmx_yday; /* days since Jan 1, 0-365 */
  	int tmx_isdst; /* Daylight Saving Time indicator */
} tmx_t;


#ifdef __cplusplus
}
#endif

#endif  /* _COMMON_H_ */

