/**************************************************************************
*                         H E A D E R   F I L E S                        *
**************************************************************************/
#include <stdio.h>
#include "common.h"
#include "sp5k_global_api.h"
#include "sp5k_dbg_api.h"

#include "api/sp5k_rsvblk_api.h"
#include "api/sp5k_usb_api.h"

#include "app_ui_para.h"
#include "app_rtc.h"

// ----------------------------------------------------------------------
// Definition
// ----------------------------------------------------------------------

#define UI_CFG_USB_MODE_BULK_VND    0
#define UI_CFG_USB_MODE_UVC_EXT     0

/* Video Class-Specific Request Codes */
#define RC_UNDEFINED 0x00
#define SET_CUR 0x01
#define SET_MIN 0x02
#define SET_MAX 0x03
#define GET_CUR 0x81
#define GET_MIN 0x82
#define GET_MAX 0x83 
#define GET_RES 0x84
#define GET_LEN 0x85
#define GET_INFO 0x86
#define GET_DEF 0x87

#if UI_CFG_USB_MODE_BULK_VND
#define VND_REQ_UI_CFG_R 0xC0
#define VND_REQ_UI_CFG_W 0xC1
#define VND_REQ_UI_RTC_R 0xC2
#define VND_REQ_UI_RTC_W 0xC3
#endif /*UI_CFG_USB_MODE_BULK_VND*/

#if UI_CFG_USB_MODE_UVC_EXT
#define REQ_PRE_READ 0x10
#define REQ_UI_CFG_R 0x0A
#define REQ_UI_CFG_W 0x0B
#define REQ_UI_RTC_R 0x0C
#define REQ_UI_RTC_W 0x0D

#define REQ_EXT_INFO_LEN 4 /*(sizeof(uvcExtBuff_s.Index) + sizeof(uvcExtBuff_s.pBuffer))*/

typedef struct uvcExtBuff_s {
	UINT16 Index;
	UINT16 Value;
	UINT8 pBuffer[1];
} uvcExtBuff_t;

typedef struct uvcPreReadInfo_s {
	UINT8  valid;
	UINT16 Index;
	UINT16 Value;
} uvcPreReadInfo_t;
#endif /*UI_CFG_USB_MODE_UVC_EXT*/

// ----------------------------------------------------------------------
// Gloabl variables
// ----------------------------------------------------------------------
#if 0
static UINT16 u16UvcXuRegAddr;
static UINT16 u16UvcXuRegData;
#endif
#if UI_CFG_USB_MODE_UVC_EXT
static uvcPreReadInfo_t preReadInfo = {0};
#endif /*UI_CFG_USB_MODE_UVC_EXT*/
extern appRTC_t rtcTime;

// ----------------------------------------------------------------------
// Static Prototype Functions
// ----------------------------------------------------------------------
static UINT32 u32UvcReq_PostOutDataExtensionUnit(UINT16 wIndex, UINT16 wValue, UINT8 *pData, UINT16 len);
static UINT32 u32UvcReq_PreInDataExtensionUnit(UINT16 wIndex, UINT16 wValue, UINT8 *pData, UINT16 len);
#if UI_CFG_USB_MODE_UVC_EXT
static void u32UvcReq_PreReadInfoSet(UINT8 valid, UINT16 wIndex, UINT16 wValue)
{
	uvcPreReadInfo_t *pInfo = &preReadInfo;

	if (valid) {
		pInfo->valid = 1;
	} else {
		pInfo->valid = 0;
	}
	
	pInfo->Index = wIndex;
	pInfo->Value = wValue;
}
#endif /*UI_CFG_USB_MODE_UVC_EXT*/
static UINT32 u32UvcReq_PreInDataExtensionUnit(UINT16 wIndex, UINT16 wValue, UINT8 *pData, UINT16 len)
{
#if UI_CFG_USB_MODE_UVC_EXT
	uiPara_t *puiPara = appUiParaGet();
	uvcPreReadInfo_t *pInfo = &preReadInfo;
	
	printf("<UVC Req>:0x%x\n", (wValue&0xff00) >> 8);

	switch ((wValue&0xff00) >> 8) {
		case REQ_UI_CFG_R:
			if (pInfo->valid) {
				if (pInfo->Index < sizeof(uiPara_t)) {
					pData[0] = *(((volatile UINT8*)puiPara) + pInfo->Index);
				}
				printf("<UI info R>wIndex:0x%x, pData:%d\n", pInfo->Index, pData[0]);
				u32UvcReq_PreReadInfoSet(0, 0, 0);
			}
			break;
		case REQ_UI_RTC_R:
			printf("<RTC info R>wIndex:0x%x, pData:%d\n", pInfo->Index, pData[0]);
			break;
		default:
			break;
	}
#endif /*UI_CFG_USB_MODE_UVC_EXT*/	
	return 1;
}

static UINT32 u32UvcReq_PostOutDataExtensionUnit(UINT16 wIndex, UINT16 wValue, UINT8 *pData, UINT16 len)
{
#if UI_CFG_USB_MODE_UVC_EXT
	uvcExtBuff_t *pBuff = (uvcExtBuff_t *)pData;
	appRTC_t *syncTime = (appRTC_t *)pBuff->pBuffer;
	uiPara_t *puiPara = appUiParaGet();
	
	printf("<UVC Req>:0x%x\n", (wValue&0xff00) >> 8);

	switch ((wValue&0xff00) >> 8) {
		case REQ_UI_CFG_R | REQ_PRE_READ:
			printf("<UI info R_Pre>wIndex:0x%x, Value:%d\n", pBuff->Index, pBuff->Value);
			u32UvcReq_PreReadInfoSet(1, pBuff->Index, pBuff->Value);
			break;
	
		case REQ_UI_CFG_W:
			printf("<UI info W>wIndex:0x%x, pData:%d\n", pBuff->Index, pBuff->pBuffer[0]);
			if (pBuff->Index < sizeof(uiPara_t)) {
				*(((volatile UINT8*)puiPara) + pBuff->Index) = pBuff->pBuffer[0];
			}
			break;
		case REQ_UI_RTC_W:
			printf("<RTC info W>wIndex:0x%x, wValue:0x%x, pData:%d\n", pBuff->Index, pBuff->Value, pBuff->pBuffer[0]);
			if (pBuff->Value == 0) {
				rtcTime.year = syncTime->year;
				rtcTime.mon  = syncTime->mon;
				rtcTime.day  = syncTime->day;
				
				puiPara->rtcYear   = rtcTime.year - 2000;
				puiPara->rtcMonth  = rtcTime.mon - 1;
				puiPara->rtcDay    = rtcTime.day - 1;
			} else if (pBuff->Value == 1) {
				rtcTime.hour = syncTime->year;
				rtcTime.min  = syncTime->mon;
				rtcTime.sec  = syncTime->day;
				
				puiPara->rtcHour   = rtcTime.hour;
				puiPara->rtcMinute = rtcTime.min;
				
				appRtcSet(&rtcTime);
				printf("rtcTime:%d/%d/%d %d:%d:%d\n", rtcTime.year, rtcTime.mon, rtcTime.day, rtcTime.hour, rtcTime.min, rtcTime.sec);
			}
			break;
		default:
			break;
	}
#endif /*UI_CFG_USB_MODE_UVC_EXT*/
	return 1;
}
/*20120702 juling++ Add extension unit CB function*/
static UINT32 u32UvcReq_PreInGetMin(UINT16 wIndex, UINT16 wValue, UINT8 *pData, UINT16 len)
{
	UINT16 *pLen = (UINT16*)pData;

	*pLen = 0x0000;

	printf("<UVC Get Min> 0x%x\n", *pLen);

	return 1;
}

static UINT32 u32UvcReq_PreInGetMax(UINT16 wIndex, UINT16 wValue, UINT8 *pData, UINT16 len)
{
	UINT16 *pLen = (UINT16*)pData;

	*pLen = 0x00FF;

	printf("<UVC Get Max> 0x%x\n", *pLen);
	
	return 1;
}

static UINT32 u32UvcReq_PreInGetRes(UINT16 wIndex, UINT16 wValue, UINT8 *pData, UINT16 len)
{
	UINT16 *pLen = (UINT16*)pData;

	*pLen = 0x0001;

	printf("<UVC Get Res> 0x%x\n", *pLen);

	return 1;
}

static UINT32 u32UvcReq_PreInGetInfo(UINT16 wIndex, UINT16 wValue, UINT8 *pData, UINT16 len)
{
	UINT8 *pLen = (UINT8*)pData;

	*pLen = 0x03;

	printf("<UVC Get Info> 0x%x\n", *pLen);

	return 1;
}

static UINT32 u32UvcReq_PreInGetDef(UINT16 wIndex, UINT16 wValue, UINT8 *pData, UINT16 len)
{
	UINT16 *pLen = (UINT16*)pData;

	*pLen = 0x0000;

	printf("<UVC Get Def> 0x%x\n", *pLen);

	return 1;
}

static UINT32 u32UvcReq_PreInGetLen(UINT16 wIndex, UINT16 wValue, UINT8 *pData, UINT16 len)
{
	UINT16 *pLen = (UINT16*)pData;
	
	printf("<UVC GetLen>Req:0x%x, len:%d\n", (wValue&0xff00) >> 8, len);
	
 	switch ((wValue&0xff00) >> 8) {
#if UI_CFG_USB_MODE_UVC_EXT 	
		case REQ_UI_CFG_W:
			*pLen = 0x01+REQ_EXT_INFO_LEN;
			printf("<UI infoW Len> 0x%x\n", *pLen);
			break;
		case REQ_UI_CFG_R:
			*pLen = 0x01;
			printf("<UI infoR Len> 0x%x\n", *pLen);
			break;
		case REQ_UI_CFG_R | REQ_PRE_READ:
			*pLen = 0x01+REQ_EXT_INFO_LEN;
			printf("<UI infoR_Pre Len> 0x%x\n", *pLen);
			break;
		case REQ_UI_RTC_W:
			*pLen = sizeof(appRTC_t)/2 + REQ_EXT_INFO_LEN; ;/*Work around for WinXP, MUST be less than 11*/
			printf("<RTC infoW Len> 0x%x\n", *pLen);
			break;
#endif /*UI_CFG_USB_MODE_UVC_EXT*/			
		default:
			*pLen = 0x01;
			break;
	}

	return 1;
}

#if UI_CFG_USB_MODE_BULK_VND
static UINT32 appUsbVndReq_UICfgInfoGet(UINT16 wIndex, UINT16 wValue, UINT8 *pData, UINT16 len)
{
	uiPara_t *puiPara = appUiParaGet();

	if (wIndex < sizeof(uiPara_t)) {
		pData[0] = *(((volatile UINT8*)puiPara) + wIndex);
	}

	printf("[UI info R]wIndex:0x%x, pData:%d\n", wIndex, pData[0]);

	return 1;
}

static UINT32 appUsbVndReq_UICfgInfoSet(UINT16 wIndex, UINT16 wValue, UINT8 *pData, UINT16 len)
{
	uiPara_t *puiPara = appUiParaGet();

	if (wIndex < sizeof(uiPara_t)) {
		*(((volatile UINT8*)puiPara) + wIndex) = pData[0];
	}

	printf("[UI info W]wIndex:0x%x, pData:%d\n", wIndex, pData[0]);

	return 1;
}

static UINT32 appUsbVndReq_UICfgRtcSet(UINT16 wIndex, UINT16 wValue, UINT8 *pData, UINT16 len)
{
	appRTC_t *syncTime = (appRTC_t *)pData;
	uiPara_t *puiPara = appUiParaGet();

	printf("[UI RTC W]wIndex:0x%x, len:%d\n", wIndex, len);

	if (wValue == 0) {
		rtcTime.year = syncTime->year;
		rtcTime.mon  = syncTime->mon;
		rtcTime.day  = syncTime->day;
		
		puiPara->rtcYear   = rtcTime.year - 2000;
		puiPara->rtcMonth  = rtcTime.mon - 1;
		puiPara->rtcDay    = rtcTime.day - 1;
	} else if (wValue == 1) {
		rtcTime.hour = syncTime->year;
		rtcTime.min  = syncTime->mon;
		rtcTime.sec  = syncTime->day;
		
		puiPara->rtcHour   = rtcTime.hour;
		puiPara->rtcMinute = rtcTime.min;
		
		appRtcSet(&rtcTime);
		printf("rtcTime:%d/%d/%d %d:%d:%d\n", rtcTime.year, rtcTime.mon, rtcTime.day, rtcTime.hour, rtcTime.min, rtcTime.sec);
	}

	return 1;
}
#endif /*UI_CFG_USB_MODE_BULK_VND*/

void vUvcReq_ExtensionUnitCallbackInit(void)
{
	sp5kUsbVendreqCallbackSet(SP5K_FUNC_WRITE_REQ, SET_CUR, NULL, u32UvcReq_PostOutDataExtensionUnit);
	sp5kUsbVendreqCallbackSet(SP5K_FUNC_READ_REQ, GET_CUR, u32UvcReq_PreInDataExtensionUnit, NULL);
	/*20120702 juling++ Add extension unit CB function*/
	sp5kUsbVendreqCallbackSet(SP5K_FUNC_READ_REQ, GET_MIN, u32UvcReq_PreInGetMin, NULL);
	sp5kUsbVendreqCallbackSet(SP5K_FUNC_READ_REQ, GET_MAX, u32UvcReq_PreInGetMax, NULL);
	sp5kUsbVendreqCallbackSet(SP5K_FUNC_READ_REQ, GET_RES, u32UvcReq_PreInGetRes, NULL);
	sp5kUsbVendreqCallbackSet(SP5K_FUNC_READ_REQ, GET_INFO, u32UvcReq_PreInGetInfo, NULL);
	sp5kUsbVendreqCallbackSet(SP5K_FUNC_READ_REQ, GET_DEF, u32UvcReq_PreInGetDef, NULL);
	sp5kUsbVendreqCallbackSet(SP5K_FUNC_READ_REQ, GET_LEN, u32UvcReq_PreInGetLen, NULL);

#if UI_CFG_USB_MODE_BULK_VND
	sp5kUsbVendreqCallbackSet(SP5K_FUNC_READ_REQ,  VND_REQ_UI_CFG_R, appUsbVndReq_UICfgInfoGet, NULL);
	sp5kUsbVendreqCallbackSet(SP5K_FUNC_WRITE_REQ, VND_REQ_UI_CFG_W, NULL, appUsbVndReq_UICfgInfoSet);
	sp5kUsbVendreqCallbackSet(SP5K_FUNC_WRITE_REQ, VND_REQ_UI_RTC_W, NULL, appUsbVndReq_UICfgRtcSet);
#endif /*UI_CFG_USB_MODE_BULK_VND*/
}

