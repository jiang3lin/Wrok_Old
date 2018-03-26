/**************************************************************************
 *
 *	Copyright (c) 2006-2011 by Sunplus mMedia Inc., Ltd.
 *
 *	This software is copyrighted by and is the property of Sunplus
 *	mMedia Inc., Ltd. All rights are reserved by Sunplus mMedia
 *	Inc., Ltd. This software may only be used in accordance with the
 *	corresponding license agreement. Any unauthorized use, duplication,
 *	distribution, or disclosure of this software is expressly forbidden.
 *
 *	This Copyright notice MUST not be removed or modified without prior
 *	written consent of Sunplus mMedia Inc., Ltd.
 *
 *	Sunplus mMedia Inc., Ltd. reserves the right to modify this
 *	software without notice.
 *
 *	Sunplus mMedia Inc., Ltd.
 *	19-1, Innovation First Road, Science-Based Industrial Park,
 *	Hsin-Chu, Taiwan, R.O.C.
 *
 **************************************************************************/
#ifndef __AWB_API_H__
#define __AWB_API_H__

/**************************************************************************
 * awb Clib api
 **************************************************************************/

/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/
#include "common.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

typedef enum{
	WB_PARAM_Enable,//(UINT32)isenable	
	WB_PARAM_Flash,//(UINT32)isflash
	WB_PARAM_Cvr,//(UINT32)isCvr
	WB_PARAM_Func,//WbFunc_e
	
	WB_PARAM_PvStep = 0x01000000,//val=1,2,4,8,16,32,def:4
	WB_PARAM_ShiftX,
	WB_PARAM_ShiftY,
	WB_PARAM_ALight,
	WB_PARAM_PvBitNum,
	WB_PARAM_SnapBitNum,
	WB_PARAM_SmoothFrameCnt,//val<60,def:30

	WB_PARAM_CvrDayGid = 0x01100000,//Ae_Lv_e,def:Lv11
	WB_PARAM_CvrDayPvStep,//see WB_PARAM_PvStep,def 4
	WB_PARAM_CvrDaySmoothFrameCnt,//see WB_PARAM_SmoothFrameCnt,def 30
	WB_PARAM_CvrNightGid,//Ae_Lv_e,def:Lv8
	WB_PARAM_CvrNightPvStep,//see WB_PARAM_PvStep,def 16
	WB_PARAM_CvrNightSmoothFrameCnt,//see WB_PARAM_SmoothFrameCnt,def 60

	WB_PARAM_GetAll = 0x06000000,
	WB_PARAM_GetPara,
	WB_PARAM_GetInfo,
	WB_PARAM_GetPixel,
	WB_PARAM_Get3aInfo,
	WB_PARAM_GetPvRg,
	WB_PARAM_GetPvBg,
	WB_PARAM_GetSnapRg,
	WB_PARAM_GetSnapBg,

	WB_DEBUG_Printf = 0x08000000,
	WB_DEBUG_Info,
	WB_DEBUG_Wnd,
	WB_DEBUG_Check,
	WB_DEBUG_Snap,
} WbParam_e;

typedef enum {
	WB_MODE_Auto,				// 0 WB_MODE_AUTO       		
	WB_MODE_D65,				// 1 WB_MODE_DAYLIGHT       	
	WB_MODE_Cloudy,				// 2 WB_MODE_CLOUDY       	
	WB_MODE_Tungsten,			// 3 WB_MODE_TUNGSTEN       	
	WB_MODE_Fluorescent_L,		// 4 WB_MODE_FLUORESCENT_L 	
	WB_MODE_Fluorescent_H,		// 5 WB_MODE_FLUORESCENT_H	 
	WB_MODE_Fluorescent,		// 6 WB_MODE_FLUORESCENT
	WB_MODE_Custom,				// 7 WB_MODE_CUSTOM  

	WB_MODE_Sunny,
	WB_MODE_Sunset,

	WB_MODE_Max,
	WB_MODE_Reset,

} WbMode_e;

typedef enum{
	WB_FUNC_Null,
	WB_FUNC_Avg,
	WB_FUNC_Cvr,	
	WB_FUNC_Scanner,
	WB_FUNC_Indoor,
	WB_FUNC_Outdoor,
} WbFunc_e;

typedef enum {
	WB_Proc_Pv,
	WB_Proc_Snap,	
} WbProc_e;

typedef enum{
	WB_DEBUG_Null,
	WB_DEBUG_Start,
	WB_DEBUG_Stop,
	WB_DEBUG_Write,
}WbDebug_e;

typedef enum {
	Lv0 = 16*0,
	Lv1 = 16*1,
	Lv2 = 16*2,
	Lv3 = 16*3,
	Lv4 = 16*4,
	Lv5 = 16*5,
	Lv6 = 16*6,
	Lv7 = 16*7,
	Lv8 = 16*8,
	Lv9 = 16*9,
	Lv10 = 16*10,
	Lv11 = 16*11,
	Lv12 = 16*12,
	Lv13 = 16*13,
	Lv14 = 16*14,
	Lv15 = 16*15,
	Lv16 = 16*16,
	Lv17 = 16*17,
}Ae_Lv_e;
//////////////////////////////////////////////////////////////////////////
//UINT32 wbApiSet(const UINT32 sector,UINT32 value, ...);
//void* wbGet(const UINT32 sector);
//UINT32 wbProcPv( UINT16 * rs, UINT16 * gs, UINT16 * bs ,UINT16 *gbs );
//UINT32 wbProcSnap( UINT16 * rs, UINT16 * gs, UINT16 * bs ,UINT16 *gbs );
void wbProcess(
	UINT16 * rs
	, UINT16 * grs
	, UINT16 * bs 
	,UINT16 *gbs
	, UINT16 *rgain
	, UINT16 *bgain );

void wbApiInit(UINT32 size,UINT32 precision);
UINT32 wbApiConfig(void* pt);

void wbApiEnd();
void wbApiSetCtc(UINT16 ax,UINT16 ay,UINT16 cx,UINT16 cy,UINT16 dx,UINT16 dy);
void wbApiSetAlgo(UINT32 algo);
void wbApiSetProcess(UINT16 proc);
void wbApiSetProject(UINT16 proc,UINT16 xdim,UINT16 ydim,UINT16 bitNum);
void wbApiSetAeInfo(UINT16 gid,UINT16 luma,UINT16 contrast);
void wbApiSetImage(UINT16 w,UINT16 h,UINT16 fmt,UINT16 bayer);
void wbApiSetMode(UINT32 mode);

void wbApiSet(const WbParam_e sector,UINT32 value);
UINT32 wbApiGet(const WbParam_e sector,void* rst,UINT32 size);
#endif
