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
#ifndef APP_PTP_H
#define APP_PTP_H

#include "api/sp5k_usb_api.h"


/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

typedef enum PTPiCatchEventCode_enum {
	PTP_ICAT_EVENT_DAILSW					= 0x3001,
	PTP_ICAT_EVENT_SHUTTER_S1_OFF		= 0x3101,
	PTP_ICAT_EVENT_SHUTTER_S1_ON		= 0x3102,
	PTP_ICAT_EVENT_SHUTTER_S2_OFF		= 0x3201,
	PTP_ICAT_EVENT_SHUTTER_S2_ON		= 0x3202,
	PTP_ICAT_EVENT_ZOOM_VECTOR			= 0x3301,
	PTP_ICAT_EVENT_VIDEMODE_ON			= 0x3401,
	PTP_ICAT_EVENT_VIDEMODE_OFF			= 0x3402,
	PTP_ICAT_EVENT_FLASHRAISE_OFF		= 0x3501,
	PTP_ICAT_EVENT_FLASHRAISE_ON		= 0x3502,
	PTP_ICAT_EVENT_LENSOPEN_OFF			= 0x3601,
	PTP_ICAT_EVENT_LENSOPEN_ON			= 0x3602,
	PTP_ICAT_EVENT_SDCARD_IN				= 0x3701,
	PTP_ICAT_EVENT_SDCARD_OUT				= 0x3702,
	PTP_ICAT_EVENT_SDCARD_ERROR			= 0x370f,
	PTP_ICAT_EVENT_BATTERY_LEVEL		= 0x3801,
	PTP_ICAT_EVENT_AF_OK					= 0x4001,
	PTP_ICAT_EVENT_AF_NG					= 0x4002,
	PTP_ICAT_EVENT_HOST_CAPTURING		= 0xC001,
	PTP_ICAT_EVENT_OPMODE_CHANGE		= 0xD001,
	PTP_ICAT_EVENT_EXCEPTION				= 0xE001,
} PTPiCatchEventCode_e;


typedef enum PTPEventCode_enum {
	PTP_EC_UNDEFINED 					= USB_PIMA_EC_UNDEFINED,
	PTP_EC_CANCEL_TRANSACTION 	= USB_PIMA_EC_CANCEL_TRANSACTION,
	PTP_EC_OBJECT_ADDED 			= USB_PIMA_EC_OBJECT_ADDED,
	PTP_EC_OBJECT_REMOVED 			= USB_PIMA_EC_OBJECT_REMOVED,
	PTP_EC_STORE_ADDED 				= USB_PIMA_EC_STORE_ADDED,
	PTP_EC_STORE_REMOVED 			= USB_PIMA_EC_STORE_REMOVED,
	PTP_EC_DEVICE_PROP_CHANGED 	= USB_PIMA_EC_DEVICE_PROP_CHANGED,
	PTP_EC_OBJECT_INFO_CHANGED 	= USB_PIMA_EC_OBJECT_INFO_CHANGED,
	PTP_EC_DEVICE_INFO_CHANGED 	= USB_PIMA_EC_DEVICE_INFO_CHANGED,
	PTP_EC_REQUEST_OBJECT_TRANSFER = USB_PIMA_EC_REQUEST_OBJECT_TRANSFER ,
	PTP_EC_STORE_FULL 					= USB_PIMA_EC_STORE_FULL,
	PTP_EC_DEVICE_RESET 				= USB_PIMA_EC_DEVICE_RESET,
	PTP_EC_STORAGE_INFO_CHANGED 		= USB_PIMA_EC_STORAGE_INFO_CHANGED,
	PTP_EC_CAPTURE_COMPLETE 			= USB_PIMA_EC_CAPTURE_COMPLETE,
	PTP_EC_UNREPORTED_STATUS 			= USB_PIMA_EC_UNREPORTED_STATUS,
	/* iCatch vendor event code */
	PTP_EC_ICATCH_HOST_EVENT 			= USB_PIMA_EC_ICATCH_HOST_EVENT,
	/* end */
	PTP_EC_NUM_MAX 						= USB_PIMA_EC_NUM_MAX
} PTPEventCode_e;

typedef enum PTPCusOpCodeParam_enum {
	PTP_CUSOPCODE_S1_PRESS				= 0xC002,
	PTP_CUSOPCODE_DZOOM_TELE_PRESS = 0xC005,
	PTP_CUSOPCODE_DZOOM_WIDE_PRESS = 0xC006,
	PTP_CUSOPCODE_OZOOM_TELE_PRESS = 0xC007,
	PTP_CUSOPCODE_OZOOM_WIDE_PRESS = 0xC008,
	PTP_CUSOPCODE_QUEUE_EVENT_POLLING  = 0xD001,
} PTPCusOpCodeParam_e;

typedef enum PTPCusRespCodeParam_enum {
	PTP_RESP_OPERATION_OK 			= 0xC100,
	PTP_RESP_OPERATION_NG 			= 0xC200,
	PTP_RESP_QUEUE_EVENT_OK		= 0xD002,
	PTP_RESP_QUEUE_EVENT_EMPTY  = 0xD003,
} PTPCusRespCodeParam_e;

typedef enum PTPDscOpMode_enum
{
 	PTP_DSCOP_MODE_VIDEO_OFF				= 0x0001,
	PTP_DSCOP_MODE_SHARE					= 0X0002,
 	PTP_DSCOP_MODE_CAMERA					= 0X0003,
 	PTP_DSCOP_MODE_APP						= 0X0004,
 	PTP_DSCOP_MODE_EDIT						= 0X0005,
 	PTP_DSCOP_MODE_BACKWARD				= 0X0006,
	PTP_DSCOP_MODE_VIDEO_ON				= 0x0011
}PTPDscOpMode_e;

typedef enum PTPUISupportWiFiFunc_enum
{
	PTP_WIFI_FUNC_BROWSER          = 0x0001, /* only browser */
 	PTP_WIFI_FUNC_CAPTURE			= 0x0002, /* capture + browser */
	PTP_WIFI_FUNC_VIDEO				= 0X0003, /* video record + browser */
 	PTP_WIFI_FUNC_PREVIEW			= 0X0004, /* capture + video record + browser */
}PTPUISupportWiFiFunc_e;

typedef enum DscOpModeState_enum
{
	MODE_PREV = 0x00,
 	MODE_ACTIVE = 0x01,
}DscOpModeState_e;


typedef enum PTPEventDialModeParam_enum
{
	PTP_EVENT_DIAL_MODE_AUTO				= 0x01,
	PTP_EVENT_DIAL_MODE_PROGRAM			= 0x02,
	PTP_EVENT_DIAL_MODE_SHUTTER			= 0x03,
	PTP_EVENT_DIAL_MODE_APERTURE		= 0x04,
	PTP_EVENT_DIAL_MODE_MANUAL			= 0x05,
	PTP_EVENT_DIAL_MODE_CONT_SHOOT		= 0x06,
	PTP_EVENT_DIAL_MODE_SCENE				= 0x07
}PTPEventDialModeParam_e;

typedef enum PTPDpcDialModeValue_enum
{
	PTP_DPC_DIAL_MODE_AUTO 			= 0x0002,
	PTP_DPC_DIAL_MODE_PROGRAM			= 0xC001,
	PTP_DPC_DIAL_MODE_SHUTTER			= 0x0004,
	PTP_DPC_DIAL_MODE_APERTURE		= 0x0003,
	PTP_DPC_DIAL_MODE_MANUAL			= 0x0001,
	PTP_DPC_DIAL_MODE_CONT_SHOOT	= 0xC002,
	PTP_DPC_DIAL_MODE_SCENE			= 0xC003,
	PTP_DPC_DIAL_MODE_FACEDETECT    = 0x0007
}PTPDpcDialModeValue_e;

typedef enum PTPDpcBandSelectValue_enum
{
	PTP_DPC_BAND_SELECT_50HZ			= 0x0000,
	PTP_DPC_BAND_SELECT_60HZ			= 0x0001,
	PTP_DPC_BAND_SELECT_AUTO 			= 0x0002,
}PTPDpcBandSelectValue_e;

typedef enum PTPDpcBurstValue_enum
{
	PTP_DPC_BURST_OFF			= 0x0001,
	PTP_DPC_BURST_3				= 0x0002,
	PTP_DPC_BURST_5 			= 0x0003,
	PTP_DPC_BURST_10 			= 0x0004,
	PTP_DPC_BURST_HS			= 0x0000,
}PTPDpcBurstValue_e;

typedef enum PTPDpcCapTimeLpsIntervalValue_enum
{
	PTP_DPC_CAP_TIMELPS_INT_OFF			= 0x0001,
	PTP_DPC_CAP_TIMELPS_INT_1S			= 0x0002,
	PTP_DPC_CAP_TIMELPS_INT_5S			= 0x0003,
	PTP_DPC_CAP_TIMELPS_INT_10S			= 0x0004,
	PTP_DPC_CAP_TIMELPS_INT_20S			= 0x0005,
	PTP_DPC_CAP_TIMELPS_INT_30S			= 0x0006,
	PTP_DPC_CAP_TIMELPS_INT_1M			= 0x0007,
	PTP_DPC_CAP_TIMELPS_INT_5M			= 0x0008,
	PTP_DPC_CAP_TIMELPS_INT_10M			= 0x0009,
	PTP_DPC_CAP_TIMELPS_INT_30M			= 0x000A,
	PTP_DPC_CAP_TIMELPS_INT_1HR			= 0x000B,
}PTPDpcCapTimeLpsIntervalValue_e;

typedef enum PTPDpcCapTimeLpsDuraValue_enum
{
	PTP_DPC_CAP_TIMELPS_DUR_OFF			= 0x0001,
	PTP_DPC_CAP_TIMELPS_DUR_5M			= 0x0002,
	PTP_DPC_CAP_TIMELPS_DUR_10M			= 0x0003,
	PTP_DPC_CAP_TIMELPS_DUR_15M			= 0x0004,
	PTP_DPC_CAP_TIMELPS_DUR_20M			= 0x0005,
	PTP_DPC_CAP_TIMELPS_DUR_30M			= 0x0006,
	PTP_DPC_CAP_TIMELPS_DUR_60M			= 0x0007,
	PTP_DPC_CAP_TIMELPS_DUR_UNLMT      = 0xFFFF,
}PTPDpcCapTimeLpsDuraValue_e;

typedef enum PTPDpcVidTimeLpsIntervalValue_enum
{
	PTP_DPC_VID_TIMELPS_INT_OFF			= 0x0001,
	PTP_DPC_VID_TIMELPS_INT_1M			= 0x0002,
	PTP_DPC_VID_TIMELPS_INT_5M			= 0x0003,
	PTP_DPC_VID_TIMELPS_INT_10M			= 0x0004,
	PTP_DPC_VID_TIMELPS_INT_EVT			= 0xFFFF
}PTPDpcVidTimeLpsIntervalValue_e;

typedef enum PTPDpcVidTimeLpsDuraValue_enum
{
	PTP_DPC_VID_TIMELPS_DUR_OFF			= 0x0001,
	PTP_DPC_VID_TIMELPS_DUR_5S			= 0x0002,
	PTP_DPC_VID_TIMELPS_DUR_10S			= 0x0003,
	PTP_DPC_VID_TIMELPS_DUR_15S			= 0x0004,
	PTP_DPC_VID_TIMELPS_DUR_20S			= 0x0005,
	PTP_DPC_VID_TIMELPS_DUR_30S			= 0x0006,
	PTP_DPC_VID_TIMELPS_DUR_1M			= 0x0007,
	PTP_DPC_VID_TIMELPS_DUR_5M			= 0x0008,
	PTP_DPC_VID_TIMELPS_DUR_10M			= 0x0009,
}PTPDpcVidTimeLpsDuraValue_e;

typedef enum PTPEventZoomParam_enum
{
	PTP_ZOOM_RATIO_1 = 0x01,
	PTP_ZOOM_RATIO_2 = 0x02,
	PTP_ZOOM_RATIO_3 = 0x03,
	PTP_ZOOM_RATIO_4 = 0x04,
	PTP_ZOOM_RATIO_5 = 0x05,
	PTP_ZOOM_RATIO_6 = 0x06,
	PTP_ZOOM_RATIO_7 = 0x07,
	PTP_ZOOM_RATIO_8 = 0x08,
	PTP_ZOOM_RATIO_9 = 0x09,
	PTP_ZOOM_RATIO_10 = 0x0a,
}PTPEventZoomParam_e;

#define PTP_EVENT_MAX_Q_NUM (10)

typedef struct PTPEventQ_S{
	UINT8  CurQ1stIndex;
	UINT8  EventQIndex;
	UINT8  TotalQNum;
	UINT32 EventQCode[PTP_EVENT_MAX_Q_NUM][4]; /* ex, PTP_ICAT_EVENT_EXCEPTION*/
}ptpEventQueue_s;

typedef struct
{
    UINT32 PTP_code;
    UINT32 PTP_dat[10];
}PTPCodeCmd_t;

typedef struct
{
    UINT8        PTPNum;
    PTPCodeCmd_t PTPCode[32];
}PTPInfo_t;



/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *          M O D U L E   V A R I A B L E S   R E F E R E N C E S         *
 **************************************************************************/

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/
void appStill_PIMA_Send_Resp(UINT32 iCatchResp, UINT32 param1, UINT32 param2);
void appStill_PIMA_Send_Event(usbPIMAEventCode_e EventCode, UINT32 param1, UINT32 param2, UINT32 param3);
void appStill_PIMA_Send_iCatch_Event(UINT32 iCatchEvt, UINT32 param1, UINT32 param2);
void appStill_PTPToUI_Set(void);
void appStill_PIMA_DPC_Update(UINT8 nMenuItem);
void app_PTP_Set_DscOpMode(PTPDscOpMode_e dscOpMode );
void app_PTP_Event2Queue(ptpEventQueue_s *ptpEventQueue,UINT32 iCatchEvt, UINT32 param1, UINT32 param2, UINT32 param3);
void app_PTP_EventQ_process(ptpEventQueue_s *ptpEventQueue);
void app_PTP_EventQ_clear(ptpEventQueue_s *ptpEventQueue);
void app_PTP_clear_EventQ(void);
UINT8 app_PTP_EventQNum_Get(ptpEventQueue_s ptpEventQueue);
UINT8 app_PTP_Get_DscOpMode(DscOpModeState_e opModeState);
UINT8 app_PTP_ContRecMode_Get();
UINT8 app_PTP_Get_WiFiOpFunc();
UINT16 app_PTP_Get_CustomName();
void app_PTP_Set_TimeLapse_param(UINT16 dpc,UINT32 param);
UINT32 app_PTP_Get_TimeLapse_param(UINT16 dpc);

extern UINT32 pimaDelFileCb(UINT8 *ppayload, UINT8 *pdata, UINT32);

/**************************************************************************
 *                               M A C R O S                              *
 **************************************************************************/
#define PTP_EVT_SEND_ENABLE 	(1)
#define PTP_EVT_QUEUE_ENABLE 	(1)
#define PTP_SUPPORT_CNT_VIDEOREC (1)
#define PTP_SUPPORT_VIDEO_SIZE_FPS (1) /* warning : must pair with basefw rev.15799, or will cause err */
#define PTP_TIME_LAPSE_EN (0)
#define PTP_BURST_INTERVAL_EN	(0)
#define PTP_DZOOM_EN (0)
#endif  /* APP_PTP_H*/

