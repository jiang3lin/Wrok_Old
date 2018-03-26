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
#ifndef _APP_CHANGE_RAW_H

#define _APP_CHANGE_RAW_H

#include "common.h"
#include "api/sp5k_cdsp_api.h"
#include "app_General.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/\

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef struct rawinfo_s {
	UINT16 width;
	UINT16 height;
	UINT16 bit;
	UINT16 format;
} rawinfo_t;

typedef struct {
	UINT8 file[32];
	sp5kWbGain_t wb;
	rawinfo_t raw;
	UINT16 iso;
	UINT16 tv;
	UINT16 zoom;
	UINT16 dzoom;
	UINT16 size;
	UINT16 sharp;
	UINT16 lv;
	int wbEn:1;
	int isoEn:1;
	int tvEn:1;
	int zoomEn:1;
	int dzoomEn:1;
	int sizeEn:1;
	int sharpEn:1;
	int lvEn:1;
} rawbatch_t;

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
extern rawbatch_t rawCb;


/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

void RawBatchCheck(void);

void iqGetEv(INT32 *CTvEntry, INT32 *CAvEntry, INT32 *ISOEntry);

UINT32 iqGetZoom();
UINT32 iqGetDZoom();
UINT32 iqGetSize();
UINT32 iqGetSharp();
UINT32 iqGetLv();

UINT32 IqChangeRaw(UINT32 Addr, UINT32 RawFmt, void* pImgSize);

#endif // _APP_CHANGE_RAW_H

