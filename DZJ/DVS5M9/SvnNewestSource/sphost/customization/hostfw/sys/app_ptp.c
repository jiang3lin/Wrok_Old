/**************************************************************************
 *
 *        Copyright (c) 20012 by iCatch Technology, Inc.
 *
 *  This software is copyrighted by and is the property of iCatch
 *  Technology, Inc. All rights are reserved by iCatch Technology,
 *  Inc. This software may only be used in accordance with the
 *  corresponding license agreement. Any unauthorized use, duplication,
 *  distribution, or disclosure of this software is expressly forbidden.
 *
 *  This Copyright notice MUST not be removed or modified without prior
 *  written consent of iCatch Technology, Inc.
 *
 *  iCatch Technology, Inc. reserves the right to modify this
 *  software without notice. 
 *
 *  iCatch Technology, Inc.
 *  19-1, Innovation 1st Road, Hsinchu Science Park, Taiwan 300.
 *
 **************************************************************************/
#if ICAT_WIFI 
#define HOST_DBG 0

#include "sp5k_global_api.h"
#include "app_battery.h"
#include "app_icon.h"
#include "app_still.h"
#include "app_video.h"
#include "app_view_param.h"
#include "app_zoom_api.h"
#include "app_General.h"
#if SPCA6330
#include "app_ptp.h"
#endif
#include "App_setup_lib.h"
#include "app_wifi_utility.h"
#include "app_ui_para.h"
#include "sp5k_disk_api.h"
#include "app_version.h"
#include "app_cdfs_api.h"

#include "app_com_def.h"
#include "app_wifi_connection.h"
#include "app_scdv_ui_para_def.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define PTP_WIFIOP_FUNC PTP_WIFI_FUNC_PREVIEW
#define MINUTE (60)
static UINT8 ptpWiFiOpFunc = PTP_WIFIOP_FUNC ;

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
static PTPDscOpMode_e ptpDscOpMode = PTP_DSCOP_MODE_VIDEO_OFF;/* DSC power on : default */
static PTPDscOpMode_e ptpActOpMode = PTP_DSCOP_MODE_VIDEO_OFF;
static PTPDscOpMode_e ptpPrevOpMode = PTP_DSCOP_MODE_VIDEO_OFF;

#if PTP_TIME_LAPSE_EN /* tmp, should check if there is UI param mapping with these params 20130411 */
static UINT32 ptpCapTimeLpsInterval = 0 ;
static UINT16 ptpCapTimeLpsNum = 0 ;
static UINT32 ptpVidTimeLpsInterval = 0 ;
static UINT32 ptpVidTimeLpsLength = 0;
#endif
#if PTP_BURST_INTERVAL_EN
static UINT32 ptpBurstCapInterval = 5;
#endif



#if PTP_SUPPORT_CNT_VIDEOREC
static BOOL ptpContRecMode = TRUE;
#else
static BOOL ptpContRecMode = FALSE;
#endif

#define UI_PTP_UNSUPPORT_CMD 0x8000
#define UI_PTP_UNDEFINE_CMD  0xFFBF

/*===== PIMA Start =====*/
/*== EnumForm Start ==*/


static struct
{
    UINT16 w,h;
    UINT16 UiIndex;
} 
#if 0
PTPUI_Map_Size[]=

{
    { UI_PTP_UNDEFINE_CMD, UI_PTP_UNDEFINE_CMD },
    { 3648, 2736, UI_PIC_SIZE_FULL },
    #if !CAM_TYPE_CVR
    { 3456, 2592, UI_PIC_SIZE_9M},
    #endif
    { 3264, 2448, UI_PIC_SIZE_8M },
    #if !CAM_TYPE_CVR
    { 3072, 2304, UI_PIC_SIZE_7M},
    { 3640, 2048, UI_PIC_SIZE_7M_169},
    { 2560, 1920, UI_PIC_SIZE_5M },
    #endif
    { 2048, 1536, UI_PIC_SIZE_3M },
    #if !CAM_TYPE_CVR
    { 1600, 1200, UI_PIC_SIZE_2M},
    #endif
    { 1920, 1080, UI_PIC_SIZE_2M_169},
    { 640,  480,  UI_PIC_SIZE_VGA},
};
#endif
PTPUI_Map_Size[]=
{
    { UI_PTP_UNDEFINE_CMD, UI_PTP_UNDEFINE_CMD },
   //{ 4640, 3480, UI_SCDV_PHOTO_NORMAL_SIZE_16M },
   { 2592, 1944, UI_SCDV_PHOTO_NORMAL_SIZE_5M },
   { 2048, 1536, UI_SCDV_PHOTO_NORMAL_SIZE_3M },
   { 1920, 1080, UI_SCDV_PHOTO_NORMAL_SIZE_2M_16X9 },
};


static struct
{
    UINT16 quality;
    UINT16 UiIndex;
} PTPUI_Map_Quality[]=
{
    { UI_PTP_UNDEFINE_CMD, UI_PTP_UNDEFINE_CMD },
    {  5, UI_QUALITY_SUPERFINE },
    {  8, UI_QUALITY_FINE},
    { 12, UI_QUALITY_NORMAL},
};

static struct
{
    UINT16 iris;
    UINT16 UiIndex;
} PTPUI_Map_Iris[]=
{
    { UI_PTP_UNDEFINE_CMD, UI_PTP_UNDEFINE_CMD },
    { 280, APERTURE_2_8},
    { 560, APERTURE_5_6 },
};

static struct
{
    UINT16 iso;
    UINT16 UiIndex;
} PTPUI_Map_ISO[]=
{
    { UI_PTP_UNDEFINE_CMD, UI_PTP_UNDEFINE_CMD },
    {   50, UI_ISO_50 },
    {  100, UI_ISO_100 },
    {  200, UI_ISO_200 },
    {  400, UI_ISO_400 },
    {  800, UI_ISO_800 },
    { 1600, UI_ISO_1600 },
};

static struct
{
    SINT16 ev;
    UINT16 UiIndex;
} PTPUI_Map_EV[]=
{
    { UI_PTP_UNDEFINE_CMD, UI_PTP_UNDEFINE_CMD },
    { -200, UI_EXP_BIAS_N200 },
    #if !CAM_TYPE_CVR
    { -166, UI_EXP_BIAS_N166 },
    { -133, UI_EXP_BIAS_N133 },
    #endif
    { -100, UI_EXP_BIAS_N100 },
    #if !CAM_TYPE_CVR
    {  -66, UI_EXP_BIAS_N066 },
    {  -33, UI_EXP_BIAS_N033 },
    #endif
    {    0, UI_EXP_BIAS_ZERO },
    #if !CAM_TYPE_CVR
    {   33, UI_EXP_BIAS_P033 },
    {   66, UI_EXP_BIAS_P066 },
    #endif
    {  100, UI_EXP_BIAS_P100 },
    #if !CAM_TYPE_CVR
    {  133, UI_EXP_BIAS_P133 },
    {  166, UI_EXP_BIAS_P166 },
    #endif
    {  200, UI_EXP_BIAS_P200 },
};

static struct
{
    UINT16 delay;
    UINT16 UiIndex;
} PTPUI_Map_Delay[]=
{
    { UI_PTP_UNDEFINE_CMD, UI_PTP_UNDEFINE_CMD },
    {     0, UI_DRIVE_OFF },
    {  2000, UI_DRIVE_TIMER_2S },
     {  5000, UI_DRIVE_TIMER_5S },
    { 10000, UI_DRIVE_TIMER_10S },
};
/*== EnumForm End ==*/

/*== ItemListForm Start ==*/
static UINT16 PTPUI_Map_WB[]=  //PIMA 0x5005
{
    UI_PTP_UNDEFINE_CMD,
	UI_WB_AUTO,
	UI_WB_DAYLIGHT,
	UI_WB_CLOUDY,
	UI_WB_FLUORESCENT,
	UI_WB_TUNGSTEN,
};

static UINT16 PTPUI_Map_AF[]=  //PIMA 0x500A
{
    UI_PTP_UNDEFINE_CMD,
    UI_PTP_UNSUPPORT_CMD,
    UI_FOCUS_NORMAL,
    UI_FOCUS_MACRO,
};

static UINT16 PTPUI_Map_Metering[]=  //PIMA 0x500B
{
    UI_PTP_UNDEFINE_CMD,
    UI_PTP_UNSUPPORT_CMD,
    UI_METER_CENTER,
    UI_METER_MULTI,
    UI_METER_SPOT,
};

static UINT16 PTPUI_Map_Flash[]=  //PIMA 0x500C
{
    UI_PTP_UNDEFINE_CMD,
    UI_FLASH_AUTO,
    UI_FLASH_OFF,
    UI_PTP_UNSUPPORT_CMD,
    UI_FLASH_RED_EYE,
    UI_PTP_UNSUPPORT_CMD,
    UI_PTP_UNSUPPORT_CMD,
};

static UINT16 PTPUI_Map_Capture[]=  //PIMA 0x5013
{
    UI_PTP_UNDEFINE_CMD,
    UI_DRIVE_OFF,
    UI_DRIVE_BURST,
    UI_PTP_UNSUPPORT_CMD,
};

static UINT16 PTPUI_Map_Sharpness[]=  //PIMA 0x5015
{
    UI_PTP_UNDEFINE_CMD,
    UI_SHARP_LESS,
    UI_SHARP_NORMAL,
    UI_SHARP_MORE,
};

static UINT16 PTPUI_Map_Effect[]=  //PIMA 0x5017
{
    UI_PTP_UNDEFINE_CMD,
    UI_EFFECT_NORMAL,
    UI_EFFECT_BW,
    UI_EFFECT_SEPIA,
};
/*== ItemListForm End ==*/
/*===== PIMA End =====*/


/*===== iCatch Start =====*/
/*== EnumForm Start ==*/
#if PTP_SUPPORT_VIDEO_SIZE_FPS
static struct
{
    UINT16 w,h,fps;
    UINT16 UiIndex;
} PTPUI_Map_SizeVid[]=  //icatch 0xD605
{
    { UI_PTP_UNDEFINE_CMD, UI_PTP_UNDEFINE_CMD },
    { 1920, 1080,30, UI_VID_SIZE_FHD },
    //{ 1280,  960, 30,UI_VID_SIZE_XGA },/*xuan.ruan@20131113 add for mantis bug 0048817*/
    { 1280,  720, 60,UI_VID_SIZE_HD_60FPS },
    { 1280,  720, 30,UI_VID_SIZE_HD_30FPS },       
    //{ 848,  480, 60,UI_VID_SIZE_WVGA },
    //{ 848,  480, 30,UI_VID_SIZE_WVGA },    
    //{ 640,  480, 120,3},
    //{ 320,  240, 240,4},
};

#else
static struct
{
    UINT16 w,h;
    UINT16 UiIndex;
} PTPUI_Map_SizeVid[]=  //icatch 0xD605
{
    { UI_PTP_UNDEFINE_CMD, UI_PTP_UNDEFINE_CMD },
    { 1920, 1080, UI_VID_SIZE_FHD },
    { 1280,  720, UI_VID_SIZE_HD_30FPS },
    { 1280,  720, UI_VID_SIZE_HD_60FPS },
    {  640,  480, UI_VID_SIZE_VGA },
    {  320,  240, UI_VID_SIZE_QVGA },
};
#endif
/*== EnumForm End ==*/

/*== ItemListForm Start ==*/
static UINT16 PTPUI_Map_Stamp[]=  //icatch 0xD607
{
    UI_PTP_UNDEFINE_CMD,
    UI_STAMP_OFF,
    UI_STAMP_DATE,
    UI_STAMP_DATE_TIME,
};
/*== ItemListForm End ==*/
static UINT16 PTPUI_Map_Burst[]=
{/* 1 ~ 5, unit : image burst number */
	UI_PTP_UNDEFINE_CMD,
    0,
	3,
	5,
	10,
	5,
};

#if PTP_TIME_LAPSE_EN
static UINT32 PTPUI_Map_Cap_TimeLpsInt[]= /* capture to capture interval */
{ /* 1 ~ 11 mode, unit : secs */
	UI_PTP_UNDEFINE_CMD,
    0,
	1,
	5,
	10,
	20,
	30,
	MINUTE,
	MINUTE*5,
	MINUTE*10,
	MINUTE*30,
	MINUTE*60
};

static UINT16 PTPUI_Map_Cap_TimeLpsNum[]= /* total capture duration */
{ /* 1 ~ 7 mode, 0xFFFF : unlimited, unit : secs */
	UI_PTP_UNDEFINE_CMD,
    0,
	MINUTE*5,
	MINUTE*10,
	MINUTE*15,
	MINUTE*20,
	MINUTE*30,
	MINUTE*60,
	0xFFFF
};

static UINT32 PTPUI_Map_Vid_TimeLpsInt[]= 
{ /* 1 ~ 4 mode, 0xFFFF, unit : secss */
	UI_PTP_UNDEFINE_CMD,
    0,
	MINUTE,
	MINUTE*5,
	MINUTE*10,
	0xFFFF
};

static UINT32 PTPUI_Map_Vid_TimeLpsLength[]= 
{ /* 1 ~ 9 mode, unit : secs */
	UI_PTP_UNDEFINE_CMD,
    0,
	5,
	10,
	15,
	20,
	30,
	MINUTE,
	MINUTE*5,
	MINUTE*10
};
#endif

/*===== iCatch End =====*/

extern UINT32 appVideoSizeChange(UINT8 vidsize);
extern void appRtpStopStreaming(void);
extern UINT32 appStillRemainNumGet(void);
extern UINT32 app_ptpUsbCustomerSet(UINT32 *pdat);
extern UINT8 appSdvVideoDiskCheckMemeoryFull(void);

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

#define PIMA_DEBUG	0

#if PIMA_DEBUG
static UINT8 *devprop[] =
	{
		"Undefined",
		"BatteryLevel",
		"FunctionalMode",
		"ImageSize",
		"CompressionSetting",
		"WhiteBalance",
		"RGB Gain",
		"F-Number",
		"FocalLength",
		"FocusDistance",
		"FocusMode",
		"ExposureMeteringMode",
		"FlashMode",
		"ExposureTime",
		"ExposureProgramMode",
		"ExposureIndex",
		"ExposureBiasCompensation",
		"DateTime",
		"CaptureDelay",
		"StillCaptureMode",
		"Contrast",
		"Sharpness",
		"DigitalZoom",
		"EffectMode",
		"BurstNumber",
		"BurstInterval",
		"TimelapseNumber",
		"TimelapseInterval",
		"FocusMeteringMode",
		"UploadURL",
		"Artist",
		"CopyrightInfo"
	};
#endif

PTPInfo_t appPTPInfo = {0};
static ptpEventQueue_s sptpEventQueue={0};
UINT32 memoryFullEvtFlg=0;

void appStill_PIMA_Send_Resp(UINT32 iCatchResp, UINT32 param1, UINT32 param2)
{
    sp5kUsbPimaiCatchResp(iCatchResp, param1, param2);

}


void appStill_PIMA_Send_iCatch_Event(UINT32 iCatchEvt, UINT32 param1, UINT32 param2)
{
	#if PTP_EVT_SEND_ENABLE
	if (appPtpStatGet()){
		printf("Host iCatch event sent : 0x%x,0x%x,0x%x\n",iCatchEvt,param1,param2);
   		appStill_PIMA_Send_Event(PTP_EC_ICATCH_HOST_EVENT, iCatchEvt, param1, param2);
	}else{
		printf("WIFI Disconnect,Host iCatch event don't sent : 0x%x,0x%x,0x%x\n",iCatchEvt,param1,param2);
	}
	#else
	return;
	#endif

}

void appStill_PIMA_Send_Event(usbPIMAEventCode_e EventCode, UINT32 param1, UINT32 param2, UINT32 param3)
{
	#if PTP_EVT_SEND_ENABLE
	#if PTP_EVT_QUEUE_ENABLE
	if (appPtpStatGet()){
		app_PTP_Event2Queue(&sptpEventQueue,EventCode,param1,param2,param3);
	}	
	#else
	printf("Host event sent : 0x%x,0x%x,0x%x,0x%x\n",EventCode,param1,param2,param3);
	sp5kUsbPimaEventSend(EventCode,param1,param2,param3);
	#endif
	#else
	return;
	#endif
}








UINT32 appPtpCustomOperation(UINT32 op, UINT32 par1, UINT32 par2)
{

	profLogPrintf(1,"Got iCatch Op:%x, %d, %d\n", op, par1, par2);

    switch (op)
    {
		case PTP_CUSOPCODE_S1_PRESS:
            /*sp5kUsbPimaEventSend(USB_PIMA_EC_ICATCH_HOST_EVENT, PTP_ICAT_EVENT_AF_OK, 0, 0);*/
          	printf("PtpCustomOperation PTP_CUSOPCODE_S1_PRESS\n");
			break;
		case PTP_CUSOPCODE_QUEUE_EVENT_POLLING:
			profLogPrintf(1," Mobile APP is checking host event!\n");
			#if PTP_EVT_SEND_ENABLE&&PTP_EVT_QUEUE_ENABLE
			if(app_PTP_EventQNum_Get(sptpEventQueue)){
				appStill_PIMA_Send_Resp(PTP_RESP_QUEUE_EVENT_OK,app_PTP_EventQNum_Get(sptpEventQueue),0);
				app_PTP_EventQ_process(&sptpEventQueue);
			}
			else
			{
				appStill_PIMA_Send_Resp(PTP_RESP_QUEUE_EVENT_EMPTY,0,0);
			}
			#else
			printf("ptp evt send disabled!");
			#endif
			break;
		case PTP_CUSOPCODE_DZOOM_TELE_PRESS:
			printf("custom operation:0x%x\n",op);
			profLogPrintf(0,"dzoom:tele");
			/* stop streaming before zoom */
			sp5kHostMsgSend(APP_UI_MSG_RTP_STOP,0);
			sp5kHostMsgSend(APP_UI_MSG_WIFI_DZOOM_TELE, 0);
			sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 100);
			sp5kHostMsgSend(APP_UI_MSG_RTP_START,0);
			/* start streaming after zoom */
			appStill_PIMA_Send_Resp(PTP_RESP_OPERATION_OK,0,0);
			printf("dzoom:tele (%d)\n",appDZoomGetRation() / 10);
			break;
		case PTP_CUSOPCODE_DZOOM_WIDE_PRESS:
			printf("custom operation:0x%x\n",op);
			profLogPrintf(0,"dzoom:wide");
			/* stop streaming before zoom */
			sp5kHostMsgSend(APP_UI_MSG_RTP_STOP,0);
			sp5kHostMsgSend(APP_UI_MSG_WIFI_DZOOM_WIDE, 0);
			sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 100);
			sp5kHostMsgSend(APP_UI_MSG_RTP_START,0);
			/* start streaming after zoom */
			appStill_PIMA_Send_Resp(PTP_RESP_OPERATION_OK,0,0);
			printf("dzoom:wide (%d)\n",appDZoomGetRation() / 10);
			break;
		case PTP_CUSOPCODE_OZOOM_TELE_PRESS:
			printf("custom operation:0x%x\n",op);
			/*
				todo
			*/
			appStill_PIMA_Send_Resp(PTP_RESP_OPERATION_OK,0,0);
			break;
		case PTP_CUSOPCODE_OZOOM_WIDE_PRESS:
			printf("custom operation:0x%x\n",op);
			/*
				todo
			*/
			appStill_PIMA_Send_Resp(PTP_RESP_OPERATION_OK,0,0);
			break;	

		default:
			printf("PtpCustomOperation property 0x%x isn't supported.\n", op);
			break;

    }

	return SUCCESS;
}

UINT16*
appStill_PIMA_Get_Map(
    UINT32 PTPCode,
    UINT8* ArraySize,
    UINT8* UI_Param
)
{
    UINT16* MappingTab = 0;
	uiPara_t* puiPara = appUiParaGet();

	switch (PTPCode)
    {
        case USB_PIMA_DPC_WHILE_BALANCE:
		case USB_PIMA_DPC_OPTIC_RATIO:	
            MappingTab = PTPUI_Map_WB;
            *ArraySize = (sizeof(PTPUI_Map_WB)/sizeof(UINT16));
            *UI_Param = puiPara->WBMode;
            break;

        case USB_PIMA_DPC_FOCUS_MODE:
            MappingTab = PTPUI_Map_AF;
            *ArraySize = (sizeof(PTPUI_Map_AF)/sizeof(UINT16));
            *UI_Param = puiPara->af;
            break;

        case USB_PIMA_DPC_EXPOSURE_METERING_MODE:
            MappingTab = PTPUI_Map_Metering;
            *ArraySize = (sizeof(PTPUI_Map_Metering)/sizeof(UINT16));
            *UI_Param = puiPara->AEMetering;
            break;

        case USB_PIMA_DPC_FLASH_MODE:
            MappingTab = PTPUI_Map_Flash;
            *ArraySize = (sizeof(PTPUI_Map_Flash)/sizeof(UINT16));
            *UI_Param = puiPara->flash;
            break;

        case USB_PIMA_DPC_STILL_CAPTURE_MODE:
            MappingTab = PTPUI_Map_Capture;
            *ArraySize = (sizeof(PTPUI_Map_Capture)/sizeof(UINT16));
            *UI_Param = puiPara->DriverMode;
            break;

        case USB_PIMA_DPC_SHARPNESS:
            MappingTab = PTPUI_Map_Sharpness;
            *ArraySize = (sizeof(PTPUI_Map_Sharpness)/sizeof(UINT16));
            *UI_Param = puiPara->ImageEdge;
            break;

        case USB_PIMA_DPC_EFFECT_MODE:
            MappingTab = PTPUI_Map_Effect;
            *ArraySize = (sizeof(PTPUI_Map_Effect)/sizeof(UINT16));
            *UI_Param = puiPara->ImageEffect;
            break;

        case USB_PIMA_DPC_DATESTAMP:
            MappingTab = PTPUI_Map_Stamp;
            *ArraySize = (sizeof(PTPUI_Map_Stamp)/sizeof(UINT16));
            *UI_Param = puiPara->StampMode;
            break;

        default:
            printf("No Case in appStill_PTPToUI_MapGet\n");
            break;
    }

    return MappingTab;
}
typedef enum ptpPIMADevicePropCustomeCode_enum
{
	USB_PIMA_DPC_CUSTOME_VIDEO_PB_SUPPORT   = 0xD75F,
		
} ptpPIMADevicePropCustomeCode_e;

typedef enum PTPDpcVideoPbValue_enum
{
	PTP_DPC_VIDEO_PB_OFF	= 0x0000,
	PTP_DPC_VIDEO_PB_ON		= 0x0001,
}PTPDpcVideoPbValue_e;

#define PTP_VIDEO_PB_SUPOORT	PTP_DPC_VIDEO_PB_ON 
void appStill_PIMA_Get_cb(UINT32 devPropCode, UINT32 *dat)
{
	UINT32 remain,i, cnt = 1;
    UINT16 CurSetting = 0, DefSetting = 0;
    UINT16* MappingTab;
    UINT8 UI_Param = 0, ArraySize = 0;
	uiPara_t* puiPara = appUiParaGet();

	#if PIMA_DEBUG
	printf("Get %x,%x (%s)\n", devPropCode, dat, devprop[devPropCode&0x0ff]);
	printf("%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
        puiPara->WBMode, puiPara->af, puiPara->AEMetering,
		gStillCB.flashMode, puiPara->ImageEffect, puiPara->DriverMode,
		puiPara->ImageQuality, gStillCB.ae.iso, puiPara->Iso,
		gStillCB.remainImg, puiPara->ImageSize, puiPara->ImageEdge);
	#endif

	//printf("cb get:devPropCode:0x%x\n",devPropCode);
	printf("cb get(0x%x):",devPropCode);

	switch (devPropCode)
    {
	//case USB_PIMA_DPC_WHILE_BALANCE:		//Disable AWB Option  for DVS5M2
	//case USB_PIMA_DPC_OPTIC_RATIO:		//Disable AWB Option  for DVS5M2
	
        case USB_PIMA_DPC_FOCUS_MODE:
        case USB_PIMA_DPC_EXPOSURE_METERING_MODE:
        /*case USB_PIMA_DPC_FLASH_MODE:*/
        case USB_PIMA_DPC_STILL_CAPTURE_MODE:
        case USB_PIMA_DPC_SHARPNESS:
        case USB_PIMA_DPC_EFFECT_MODE:
            MappingTab = appStill_PIMA_Get_Map(devPropCode, &ArraySize, &UI_Param);

            dat[cnt++] = EnumForm;
            dat[cnt++] = PTP_PROPGETSET_GETSET;

            for (i = 1 ; i < ArraySize ; i++)
            {
                if(UI_Param == MappingTab[i])
                    CurSetting = i;

                if(!MappingTab[i])
                    DefSetting = i;
            }
            dat[cnt++] = CurSetting;
            dat[cnt++] = DefSetting;

            printf("Cur = %d, Def = %d\n", CurSetting, DefSetting);

            for (i = 1 ; i < ArraySize ; i++)
            {
                if(MappingTab[i] == UI_PTP_UNSUPPORT_CMD)
                    continue;

                dat[cnt++] = i;
            }
            break;

        case USB_PIMA_DPC_IMAGE_SIZE:	//Disable Still Size Option for DVS5M2
            //printf("---cb get image size=%d\n",puiPara->SdvPhotoNormalSize)	;
            dat[cnt++] = EnumForm;
            dat[cnt++] = PTP_PROPGETSET_GETSET;

            for (i = 1 ; i < (sizeof(PTPUI_Map_Size)/sizeof(PTPUI_Map_Size[0])) ; i++)
            {
                if(PTPUI_Map_Size[i].UiIndex ==puiPara->SdvPhotoNormalSize) //puiPara->ImageSize
                    CurSetting = i;

                if(!PTPUI_Map_Size[i].UiIndex)
                    DefSetting = i;
            }
			
            dat[cnt++] = PTPUI_Map_Size[CurSetting].w;
            dat[cnt++] = PTPUI_Map_Size[CurSetting].h;
            dat[cnt++] = PTPUI_Map_Size[DefSetting].w;
            dat[cnt++] = PTPUI_Map_Size[DefSetting].h;

            printf("Cur = %d, Def = %d\n", CurSetting, DefSetting);

            for (i = 1 ; i < (sizeof(PTPUI_Map_Size)/sizeof(PTPUI_Map_Size[0])) ; i++)
            {
                if(PTPUI_Map_Size[i].UiIndex == UI_PTP_UNSUPPORT_CMD)
                    continue;

                dat[cnt++] = PTPUI_Map_Size[i].w;
                dat[cnt++] = PTPUI_Map_Size[i].h;
            }
            break;

        case USB_PIMA_DPC_COMPRESSION_SETTING:
            dat[cnt++] = EnumForm;
            dat[cnt++] = PTP_PROPGETSET_GETSET;

            for (i = 1 ; i < (sizeof(PTPUI_Map_Quality)/sizeof(PTPUI_Map_Quality[0])) ; i++)
            {
                if(PTPUI_Map_Quality[i].UiIndex == puiPara->ImageQuality)
                    CurSetting = i;

                if(PTPUI_Map_Quality[i].UiIndex == UI_QUALITY_SUPERFINE)
                    DefSetting = i;
            }
            dat[cnt++] = PTPUI_Map_Quality[CurSetting].quality;
            dat[cnt++] = PTPUI_Map_Quality[DefSetting].quality;

            printf("Cur = %d, Def = %d\n", CurSetting, DefSetting);

            for (i = 1 ; i < (sizeof(PTPUI_Map_Quality)/sizeof(PTPUI_Map_Quality[0])) ; i++)
            {
                if(PTPUI_Map_Quality[i].UiIndex == UI_PTP_UNSUPPORT_CMD)
                    continue;

                dat[cnt++] = PTPUI_Map_Quality[i].quality;
            }
            break;

		case USB_PIMA_DPC_F_NUMBER:
            dat[cnt++] = EnumForm;
            dat[cnt++] = PTP_PROPGETSET_GETSET;

            for (i = 1 ; i < (sizeof(PTPUI_Map_Iris)/sizeof(PTPUI_Map_Iris[0])) ; i++)
            {
                if(PTPUI_Map_Iris[i].UiIndex == gStillCB.ae.iris)
                    CurSetting = i;

                if(PTPUI_Map_Iris[i].UiIndex == APERTURE_2_8)
                    DefSetting = i;
            }
            dat[cnt++] = PTPUI_Map_Iris[CurSetting].iris;
            dat[cnt++] = PTPUI_Map_Iris[DefSetting].iris;

            printf("Cur = %d, Def = %d\n", CurSetting, DefSetting);

            for (i = 1 ; i < (sizeof(PTPUI_Map_Iris)/sizeof(PTPUI_Map_Iris[0])) ; i++)
            {
                if(PTPUI_Map_Iris[i].UiIndex == UI_PTP_UNSUPPORT_CMD)
                    continue;

                dat[cnt++] = PTPUI_Map_Iris[i].iris;
            }
            break;

        case USB_PIMA_DPC_EXPOSURE_INDEX:
            dat[cnt++] = EnumForm;
            dat[cnt++] = PTP_PROPGETSET_GETSET;

            for (i = 1 ; i < (sizeof(PTPUI_Map_ISO)/sizeof(PTPUI_Map_ISO[0])) ; i++)
            {
                if(PTPUI_Map_ISO[i].UiIndex == puiPara->Iso)
                    CurSetting = i;

                if(!PTPUI_Map_ISO[i].UiIndex)
                    DefSetting = i;
            }
            dat[cnt++] = PTPUI_Map_ISO[CurSetting].iso;
            dat[cnt++] = PTPUI_Map_ISO[DefSetting].iso;

            printf("Cur = %d, Def = %d\n", CurSetting, DefSetting);

            for (i = 1 ; i < (sizeof(PTPUI_Map_ISO)/sizeof(PTPUI_Map_ISO[0])) ; i++)
            {
                if(PTPUI_Map_ISO[i].UiIndex == UI_PTP_UNSUPPORT_CMD)
                    continue;

                dat[cnt++] = PTPUI_Map_ISO[i].iso;
            }
            break;

        case USB_PIMA_DPC_EXPOSURE_BAIS_COMPENSATION:
            dat[cnt++] = EnumForm;
            dat[cnt++] = PTP_PROPGETSET_GETSET;

            for (i = 1 ; i < (sizeof(PTPUI_Map_EV)/sizeof(PTPUI_Map_EV[0])) ; i++)
            {
                if(PTPUI_Map_EV[i].UiIndex == puiPara->AEMode)
                    CurSetting = i;

                if(PTPUI_Map_EV[i].UiIndex == UI_EXP_BIAS_ZERO)
                    DefSetting = i;
            }
            dat[cnt++] = PTPUI_Map_EV[CurSetting].ev;
            dat[cnt++] = PTPUI_Map_EV[DefSetting].ev;

            printf("Cur = %d, Def = %d\n", CurSetting, DefSetting);

            for (i = 1 ; i < (sizeof(PTPUI_Map_EV)/sizeof(PTPUI_Map_EV[0])) ; i++)
            {
                if(PTPUI_Map_EV[i].UiIndex == UI_PTP_UNSUPPORT_CMD)
                    continue;

                dat[cnt++] = PTPUI_Map_EV[i].ev;
            }
            break;
		#if 1
		case USB_PIMA_DPC_CAPTURE_DELAY:		//Disable Self Timer for DVS5M2
            dat[cnt++] = EnumForm;
            dat[cnt++] = PTP_PROPGETSET_GETSET;

	     //if(puiPara->SdvPhotoMode!=UI_SCDV_PHOTO_MODE_SELF_TIMER)
		 	//break;
            for (i = 1 ; i < (sizeof(PTPUI_Map_Delay)/sizeof(PTPUI_Map_Delay[0])) ; i++)
            {
				

		
		if(puiPara->SdvPhotoMode==UI_SCDV_PHOTO_MODE_SELF_TIMER)	
		{
			if(PTPUI_Map_Delay[i].UiIndex == puiPara->SdvPhotoSelfTimer+1/*pViewParam->drive*/)
	                {
	                    	CurSetting = i;
				break;
	                }
			else
				CurSetting = 1; /* [#47762] workaround, drive param may be changed by burst capture setting */
		}
                if(PTPUI_Map_Delay[i].UiIndex == UI_DRIVE_OFF)
                    DefSetting = i;
            }
	     if(puiPara->SdvPhotoMode!=UI_SCDV_PHOTO_MODE_SELF_TIMER)
		 	CurSetting=1;
		 
            dat[cnt++] = PTPUI_Map_Delay[CurSetting].delay;
            dat[cnt++] = PTPUI_Map_Delay[DefSetting].delay;

            printf("Cur = %d, Def = %d\n", CurSetting, DefSetting);

            for (i = 1 ; i < (sizeof(PTPUI_Map_Delay)/sizeof(PTPUI_Map_Delay[0])) ; i++)
            {
                if(PTPUI_Map_Delay[i].UiIndex == UI_PTP_UNSUPPORT_CMD)
                    continue;

                dat[cnt++] = PTPUI_Map_Delay[i].delay;
            }
			break;
		#endif

		case USB_PIMA_DPC_VIDEO_SIZE:		//Disable Video Size  for DVS5M2

		

		if(puiPara->ScdvMainMode == UI_SCDV_MAIN_MODE_SPORT)
		{
			puiPara->SdvVideoNormalSize=UI_SCDV_VIDEO_SIZE_1080P30;
			printf("--cb get video size1=%d\n",puiPara->SdvVideoNormalSize); //
			puiPara->VideoSize=UI_VID_SIZE_FHD;
			dat[cnt++] = EnumForm;
	             dat[cnt++] = PTP_PROPGETSET_GETSET;

	            for (i = 1 ; i < (sizeof(PTPUI_Map_SizeVid)/sizeof(PTPUI_Map_SizeVid[0])) ; i++)
	            {
	                if(PTPUI_Map_SizeVid[i].UiIndex == puiPara->SdvVideoNormalSize/*puiPara->VideoSize*/)
	                    CurSetting = i;

	                if(PTPUI_Map_SizeVid[i].UiIndex == UI_VID_SIZE_VGA)
	                    DefSetting = i;
	            }
	            dat[cnt++] = PTPUI_Map_SizeVid[CurSetting].w;
	            dat[cnt++] = PTPUI_Map_SizeVid[CurSetting].h;
				#if PTP_SUPPORT_VIDEO_SIZE_FPS
				dat[cnt++] = PTPUI_Map_SizeVid[CurSetting].fps;
				#endif
	            dat[cnt++] = PTPUI_Map_SizeVid[DefSetting].w;
	            dat[cnt++] = PTPUI_Map_SizeVid[DefSetting].h;
				#if PTP_SUPPORT_VIDEO_SIZE_FPS
				dat[cnt++] = PTPUI_Map_SizeVid[DefSetting].fps;
				#endif

	            printf("Cur = %d, Def = %d\n", CurSetting, DefSetting);

	            for (i = 1 ; i < (sizeof(PTPUI_Map_SizeVid)/sizeof(PTPUI_Map_SizeVid[0])) ; i++)
	            {
	                if(PTPUI_Map_SizeVid[i].UiIndex == UI_PTP_UNSUPPORT_CMD)
	                    continue;

	                dat[cnt++] = PTPUI_Map_SizeVid[i].w;
	                dat[cnt++] = PTPUI_Map_SizeVid[i].h;
					#if PTP_SUPPORT_VIDEO_SIZE_FPS
					dat[cnt++] = PTPUI_Map_SizeVid[i].fps;
					#endif
	            }
		}
		else
		{

			puiPara->CdvVideoNormalSize=UI_SCDV_VIDEO_SIZE_1080P30;
			printf("--cb get video size2=%d\n",puiPara->CdvVideoNormalSize); //
			puiPara->VideoSize=UI_VID_SIZE_FHD;
			dat[cnt++] = EnumForm;
	             dat[cnt++] = PTP_PROPGETSET_GETSET;

	            for (i = 1 ; i < (sizeof(PTPUI_Map_SizeVid)/sizeof(PTPUI_Map_SizeVid[0])) ; i++)
	            {
	                if(PTPUI_Map_SizeVid[i].UiIndex == puiPara->CdvVideoNormalSize/*puiPara->VideoSize*/)
	                    CurSetting = i;

	                if(PTPUI_Map_SizeVid[i].UiIndex == UI_VID_SIZE_VGA)
	                    DefSetting = i;
	            }
	            dat[cnt++] = PTPUI_Map_SizeVid[CurSetting].w;
	            dat[cnt++] = PTPUI_Map_SizeVid[CurSetting].h;
				#if PTP_SUPPORT_VIDEO_SIZE_FPS
				dat[cnt++] = PTPUI_Map_SizeVid[CurSetting].fps;
				#endif
	            dat[cnt++] = PTPUI_Map_SizeVid[DefSetting].w;
	            dat[cnt++] = PTPUI_Map_SizeVid[DefSetting].h;
				#if PTP_SUPPORT_VIDEO_SIZE_FPS
				dat[cnt++] = PTPUI_Map_SizeVid[DefSetting].fps;
				#endif

	            printf("Cur = %d, Def = %d\n", CurSetting, DefSetting);

	            for (i = 1 ; i < (sizeof(PTPUI_Map_SizeVid)/sizeof(PTPUI_Map_SizeVid[0])) ; i++)
	            {
	                if(PTPUI_Map_SizeVid[i].UiIndex == UI_PTP_UNSUPPORT_CMD)
	                    continue;

	                dat[cnt++] = PTPUI_Map_SizeVid[i].w;
	                dat[cnt++] = PTPUI_Map_SizeVid[i].h;
					#if PTP_SUPPORT_VIDEO_SIZE_FPS
					dat[cnt++] = PTPUI_Map_SizeVid[i].fps;
					#endif
	            }

		}
            break;


		case USB_PIMA_DPC_EXPOSURE_PROGRAM_MODE: /* only read ? */
			dat[cnt++] = EnumForm;
			dat[cnt++] = PTP_PROPGETSET_GETONLY;
			dat[cnt++] = PTP_DPC_DIAL_MODE_AUTO;
			dat[cnt++] = PTP_DPC_DIAL_MODE_AUTO;
			dat[cnt++] = PTP_DPC_DIAL_MODE_MANUAL;
			dat[cnt++] = PTP_DPC_DIAL_MODE_AUTO;
			dat[cnt++] = PTP_DPC_DIAL_MODE_SHUTTER;
			dat[cnt++] = PTP_DPC_DIAL_MODE_FACEDETECT;
			dat[cnt++] = PTP_DPC_DIAL_MODE_PROGRAM;
			dat[cnt++] = PTP_DPC_DIAL_MODE_CONT_SHOOT;
			dat[cnt++] = PTP_DPC_DIAL_MODE_SCENE;
			break;
		case USB_PIMA_DPC_BATTERY_LEVEL:
			{
			UINT8 batlvl[] = {0, 33, 66, 100};
			dat[cnt++] = EnumForm;
			dat[cnt++] = PTP_PROPGETSET_GETONLY;
			dat[cnt++] = batlvl[appBatteryLevelGet()];
			dat[cnt++] = batlvl[APP_BATT_LEVEL_3];
			for (i = 0 ; i < APP_BATT_LEVEL_TOTAL ; i++)
				dat[cnt++] = batlvl[i];
			}
			break;
		#if PTP_DZOOM_EN	
		case USB_PIMA_DPC_DIGITAL_ZOOM:
			dat[cnt++] = RangeForm;
			dat[cnt++] = PTP_PROPGETSET_GETONLY;
			dat[cnt++] = appDZoomGetRation() / 10;
			dat[cnt++] = appDZoomGetRation() / 10;
			dat[cnt++] = 100;
			dat[cnt++] = 1;
			dat[cnt++] = 400;
			break;
		#endif	
		case USB_PIMA_DPC_BURST_NUMBER:
			/*close burst: CVR need burst num 10 20 30 but app only
				support 3 5 10
			*/
			#if CAM_TYPE_CVR
			cnt = 0;
			break;
			#endif
			dat[cnt++] = EnumForm;
			dat[cnt++] = PTP_PROPGETSET_GETSET;
            for (i = 1 ; i < (sizeof(PTPUI_Map_Burst)/sizeof(PTPUI_Map_Burst[0])) ; i++)
            {
                if(PTPUI_Map_Burst[i] == gStillCB.btnum){
                    CurSetting = i;
					break;
                }
            }

			if(gStillCB.btnum){
				puiPara->DriverMode= UI_DRIVE_BURST;
			}else{
				puiPara->DriverMode= UI_DRIVE_OFF;
			}
			
			dat[cnt++] = (puiPara->DriverMode==UI_DRIVE_BURST) ? CurSetting: 1;
			dat[cnt++] = PTP_DPC_BURST_OFF;
			printf("burst capture number:%d\n",gStillCB.btnum);
			dat[cnt++] = PTP_DPC_BURST_OFF;
			dat[cnt++] = PTP_DPC_BURST_3;
			dat[cnt++] = PTP_DPC_BURST_5;
			dat[cnt++] = PTP_DPC_BURST_10;
			dat[cnt++] = PTP_DPC_BURST_HS;
			break;
		case USB_PIMA_DPC_EXPOSURE_TIME:
			dat[cnt++] = RangeForm;
			dat[cnt++] = PTP_PROPGETSET_GETSET;
			dat[cnt++] = sp5kOsTimeGet()&0x3fff;
			dat[cnt++] = 100;
			dat[cnt++] = 2;
			dat[cnt++] = 1;
			dat[cnt++] = 16383;
			break;
		case USB_PIMA_DPC_RGB_GAIN:
			{
			sp5kWbGain_t wb;
			sp5kIqModeSet(SP5K_MODE_STILL_PREVIEW);
			sp5kImgWbGainGet(&wb);
			sp5kIqModeSetDone(SP5K_MODE_STILL_PREVIEW);
			dat[cnt++] = RangeForm;
			dat[cnt++] = PTP_PROPGETSET_GETSET;
			dat[cnt++] = wb.rgain;
			dat[cnt++] = wb.grgain;
			dat[cnt++] = wb.bgain;
			dat[cnt++] = 0x400;
			dat[cnt++] = 0x400;
			dat[cnt++] = 0x400;
			dat[cnt++] = 1;
			dat[cnt++] = 1;
			dat[cnt++] = 1;
			dat[cnt++] = 1;
			dat[cnt++] = 1;
			dat[cnt++] = 1;
			dat[cnt++] = 0x1000;
			dat[cnt++] = 0x1000;
			dat[cnt++] = 0x1000;
			}
			break;
		case USB_PIMA_DPC_DATE_TIME:
			dat[cnt++] = DateTimeForm;
			dat[cnt++] = PTP_PROPGETSET_GETSET;
			{
				tmx_t dtime;
				UINT8 *str = (UINT8 *)&dat[cnt];
				sp5kRtcDateTimeGet(SP5K_DATE_TIME_OPTION, &dtime);
				sprintf(str, "%04d%02d%02dT%02d%02d%02d.0", dtime.tmx_year+1900
									, dtime.tmx_mon, dtime.tmx_mday
									, dtime.tmx_hour, dtime.tmx_min, dtime.tmx_sec);
				printf("date time = %s\n", str);
				cnt += (1 + strlen(str) + 3) / 4;
			}
			break;
		//case USB_PIMA_DPC_BAND_SELECTION:	//Disable Frequency Option  for DVS5M2
			dat[cnt++] = EnumForm;
			dat[cnt++] = PTP_PROPGETSET_GETSET;
			dat[cnt++] = puiPara->LightFreq;
			dat[cnt++] = PTP_DPC_BAND_SELECT_AUTO;
			dat[cnt++] = PTP_DPC_BAND_SELECT_AUTO;
			dat[cnt++] = PTP_DPC_BAND_SELECT_50HZ;
			dat[cnt++] = PTP_DPC_BAND_SELECT_60HZ;
			break;
		case USB_PIMA_DPC_SCENE_MODE:
			/* 0xC001:Scenery
			   0xC002:Backlight
			   0xC003:Night Scenery
			   0xC004:Building
			   0xC005:Kids
			   0xC006:Night Portrait
			   0xC007:Food
			   0xC008:Text
			   0xC009:Snow
			   0xC00A:Fireworks
			   0xC00B:Sunset*/
			dat[cnt++] = EnumForm;
			dat[cnt++] = PTP_PROPGETSET_GETSET;
			dat[cnt++] = 0xC001;
			dat[cnt++] = 0xC001;
			for (i = 0 ; i < 12 ; i++)
				dat[cnt++] = 0xC001+i;
			break;
		#if 0	
		case USB_PIMA_DPC_OPTIC_RATIO:
			/* ex: 1.0x to 2.0x */
			dat[cnt++] = RangeForm;
			dat[cnt++] = PTP_PROPGETSET_GETSET;
			dat[cnt++] = 100;	/* 1.0x */
			dat[cnt++] = 100;
			dat[cnt++] = 100;
			dat[cnt++] = 1;		/* step : 0.01 */
			dat[cnt++] = 200;
			break;
		#endif	
		case USB_PIMA_DPC_DSC_OP_MODE:
			/*
				Video Mode OFF: 0x0001
				Video Mode ON: 0x0011
				Share: 0x0002
				Camera Mode: 0x0003
				Application Mode: 0x0004
				Edit Mode: 0x0005
				Backward: 0x0006
			*/
			dat[cnt++] = EnumForm;
			dat[cnt++] = PTP_PROPGETSET_GETSET;
			dat[cnt++] = app_PTP_Get_DscOpMode(MODE_ACTIVE);
			dat[cnt++] = PTP_DSCOP_MODE_CAMERA;
			dat[cnt++] = PTP_DSCOP_MODE_VIDEO_OFF;
			dat[cnt++] = PTP_DSCOP_MODE_VIDEO_ON;
			dat[cnt++] = PTP_DSCOP_MODE_SHARE;
			dat[cnt++] = PTP_DSCOP_MODE_CAMERA;
			dat[cnt++] = PTP_DSCOP_MODE_APP;
			dat[cnt++] = PTP_DSCOP_MODE_EDIT;
			dat[cnt++] = PTP_DSCOP_MODE_BACKWARD;
			break;
		case USB_PIMA_DPC_UI_SUPPORT_WIFI_OP_FUNC:
			dat[cnt++] = EnumForm;
			dat[cnt++] = PTP_PROPGETSET_GETONLY;
			dat[cnt++] = app_PTP_Get_WiFiOpFunc();
			dat[cnt++] = PTP_WIFI_FUNC_PREVIEW;
			dat[cnt++] = PTP_WIFI_FUNC_BROWSER;
			dat[cnt++] = PTP_WIFI_FUNC_CAPTURE;
			dat[cnt++] = PTP_WIFI_FUNC_VIDEO;
			dat[cnt++] = PTP_WIFI_FUNC_PREVIEW;
			break;	
		case USB_PIMA_DPC_ICATCH_VARIABLE:
			dat[cnt++] = ByteArray;
			dat[cnt++] = PTP_PROPGETSET_GETSET;
			dat[cnt++] = 128;	/* 128 bytes for iCatch buffer */
			{
				UINT8 *ptr = (UINT8 *)&dat[cnt];
				memset(ptr, 0x00, 128);
			}
			cnt += 128 / sizeof(UINT32);
			break;
		case USB_PIMA_DPC_FREE_IN_IMAGE:
			remain = appStillRemainNumGet();
			dat[cnt++] = None;
			dat[cnt++] = PTP_PROPGETSET_GETONLY;
			dat[cnt++] = remain;
			dat[cnt++] = remain;
			printf("remain capture number:%d\n",remain);
			if(IS_CARD_EXIST){
				if(remain==0){
					memoryFullEvtFlg=1;
					appStill_PIMA_Send_Event(PTP_EC_STORE_FULL, SP5K_DRIVE_SD, 0, 0);
				}else if(memoryFullEvtFlg){
					memoryFullEvtFlg=0;
					appStill_PIMA_Send_Event(PTP_EC_STORE_FULL, SP5K_DRIVE_SD, 1, 0);
				}	
			}
			break;
		case USB_PIMA_DPC_REMAIN_CAPACITY_IN_SECS:
			remain = appVideoRemainSecGet();
			dat[cnt++] = RangeForm;
			dat[cnt++] = PTP_PROPGETSET_GETONLY;
			dat[cnt++] = remain;
			dat[cnt++] = remain;
			printf("~~SUNYONG:remain capacity in secs:%d\n",remain);
			dat[cnt++] = 1; /* MIN */
			dat[cnt++] = 1; /* STEP */
			dat[cnt++] = 0xFFFFFFFF;
			#if 1
			if(appSdvVideoDiskCheckMemeoryFull())
			{
				memoryFullEvtFlg=1;
				appStill_PIMA_Send_Event(PTP_EC_STORE_FULL, SP5K_DRIVE_SD, 0, 0);
			}
			else if(memoryFullEvtFlg)
			{
				memoryFullEvtFlg=0;
				appStill_PIMA_Send_Event(PTP_EC_STORE_FULL, SP5K_DRIVE_SD, 1, 0);
			}
			#else
			if(IS_CARD_EXIST){
			#ifdef CAM_TYPE_CVR
				if(remain < 60){
			#else
				if(remain==0){
			#endif
					memoryFullEvtFlg=1;
					appStill_PIMA_Send_Event(PTP_EC_STORE_FULL, SP5K_DRIVE_SD, 0, 0);
				}else if(memoryFullEvtFlg){
					memoryFullEvtFlg=0;
					appStill_PIMA_Send_Event(PTP_EC_STORE_FULL, SP5K_DRIVE_SD, 1, 0);
				}	
			}
			#endif
			break;
		#if PTP_TIME_LAPSE_EN
		case USB_PIMA_DPC_TIMELAPSE_INTERVAL: /* UINT32 */
			dat[cnt++] = EnumForm;
			dat[cnt++] = PTP_PROPGETSET_GETSET;
			for (i = 1 ; i < (sizeof(PTPUI_Map_Cap_TimeLpsInt)/sizeof(PTPUI_Map_Cap_TimeLpsInt[0])) ; i++)
			{
				if(PTPUI_Map_Cap_TimeLpsInt[i] == app_PTP_Get_TimeLapse_param(devPropCode))
					CurSetting = i;
			}		
			dat[cnt++] = CurSetting; 
			dat[cnt++] = PTP_DPC_CAP_TIMELPS_INT_OFF; /* secs */
			printf("Cur = %d\n", CurSetting);
			printf("capture timelapse interval:%d\n",app_PTP_Get_TimeLapse_param(devPropCode));
			dat[cnt++] = PTP_DPC_CAP_TIMELPS_INT_OFF;  
			dat[cnt++] = PTP_DPC_CAP_TIMELPS_INT_1S;  
			dat[cnt++] = PTP_DPC_CAP_TIMELPS_INT_5S;
			dat[cnt++] = PTP_DPC_CAP_TIMELPS_INT_10S;
			dat[cnt++] = PTP_DPC_CAP_TIMELPS_INT_20S;
			dat[cnt++] = PTP_DPC_CAP_TIMELPS_INT_30S;
			dat[cnt++] = PTP_DPC_CAP_TIMELPS_INT_1M;
			dat[cnt++] = PTP_DPC_CAP_TIMELPS_INT_5M;
			dat[cnt++] = PTP_DPC_CAP_TIMELPS_INT_10M;
			dat[cnt++] = PTP_DPC_CAP_TIMELPS_INT_30M;
			dat[cnt++] = PTP_DPC_CAP_TIMELPS_INT_1HR;
			break;
		case USB_PIMA_DPC_TIMELAPSE_NUMBER:/* UINT16 */
			dat[cnt++] = EnumForm;
			dat[cnt++] = PTP_PROPGETSET_GETSET;
			for (i = 1 ; i < (sizeof(PTPUI_Map_Cap_TimeLpsNum)/sizeof(PTPUI_Map_Cap_TimeLpsNum[0])) ; i++)
			{
				if(PTPUI_Map_Cap_TimeLpsNum[i] == app_PTP_Get_TimeLapse_param(devPropCode))
					CurSetting = i;
			}	
			dat[cnt++] = CurSetting; /* duration */
			dat[cnt++] = PTP_DPC_CAP_TIMELPS_DUR_OFF; /* duration */
			printf("Cur = %d\n", CurSetting);
			printf("capture timelapse duration:%d\n",app_PTP_Get_TimeLapse_param(devPropCode));
			dat[cnt++] = PTP_DPC_CAP_TIMELPS_DUR_OFF;  
			dat[cnt++] = PTP_DPC_CAP_TIMELPS_DUR_5M;  
			dat[cnt++] = PTP_DPC_CAP_TIMELPS_DUR_10M;
			dat[cnt++] = PTP_DPC_CAP_TIMELPS_DUR_15M;
			dat[cnt++] = PTP_DPC_CAP_TIMELPS_DUR_20M;
			dat[cnt++] = PTP_DPC_CAP_TIMELPS_DUR_30M;
			dat[cnt++] = PTP_DPC_CAP_TIMELPS_DUR_60M;
			dat[cnt++] = PTP_DPC_CAP_TIMELPS_DUR_UNLMT;
			break;
		case USB_PIMA_DPC_VIDEO_TIMELAPSE_INTERVAL: /* UINT32 */
			dat[cnt++] = EnumForm;
			dat[cnt++] = PTP_PROPGETSET_GETSET;
			for (i = 1 ; i < (sizeof(PTPUI_Map_Vid_TimeLpsInt)/sizeof(PTPUI_Map_Vid_TimeLpsInt[0])) ; i++)
			{
				if(PTPUI_Map_Vid_TimeLpsInt[i] == app_PTP_Get_TimeLapse_param(devPropCode))
					CurSetting = i; 
			}	
			dat[cnt++] = CurSetting; /* secs */
			dat[cnt++] = PTP_DPC_VID_TIMELPS_INT_OFF; /* secs */
			printf("Cur = %d\n", CurSetting);
			printf("video timelapse interval:%d\n",app_PTP_Get_TimeLapse_param(devPropCode));
			dat[cnt++] = PTP_DPC_VID_TIMELPS_INT_OFF;  /* 1~4 selection, 5 : unlimited ? */
			dat[cnt++] = PTP_DPC_VID_TIMELPS_INT_1M;  
			dat[cnt++] = PTP_DPC_VID_TIMELPS_INT_5M;
			dat[cnt++] = PTP_DPC_VID_TIMELPS_INT_10M;
			dat[cnt++] = PTP_DPC_VID_TIMELPS_INT_EVT;
			break;
		case USB_PIMA_DPC_VIDEO_TIMELAPSE_LENGTH: /* UINT32 */
			dat[cnt++] = EnumForm;
			dat[cnt++] = PTP_PROPGETSET_GETSET;
			for (i = 1 ; i < (sizeof(PTPUI_Map_Vid_TimeLpsLength)/sizeof(PTPUI_Map_Vid_TimeLpsLength[0])) ; i++)
			{
				if(PTPUI_Map_Vid_TimeLpsLength[i] == app_PTP_Get_TimeLapse_param(devPropCode))
					CurSetting = i;
			}
			dat[cnt++] = CurSetting; /* secs */
			dat[cnt++] = PTP_DPC_VID_TIMELPS_DUR_OFF; /* secs */
			printf("video timelapse limit length:%d\n",app_PTP_Get_TimeLapse_param(devPropCode));
			dat[cnt++] = PTP_DPC_VID_TIMELPS_DUR_OFF; /* 1 ~ 9 selections */ 
			dat[cnt++] = PTP_DPC_VID_TIMELPS_DUR_5S;  
			dat[cnt++] = PTP_DPC_VID_TIMELPS_DUR_10S;
			dat[cnt++] = PTP_DPC_VID_TIMELPS_DUR_15S;
			dat[cnt++] = PTP_DPC_VID_TIMELPS_DUR_20S;
			dat[cnt++] = PTP_DPC_VID_TIMELPS_DUR_30S;
			dat[cnt++] = PTP_DPC_VID_TIMELPS_DUR_1M;
			dat[cnt++] = PTP_DPC_VID_TIMELPS_DUR_5M;
			dat[cnt++] = PTP_DPC_VID_TIMELPS_DUR_10M;
			break;
		#endif
		case USB_PIMA_DPC_ARTIST: /* Product Name */
			dat[cnt++] = LongStringForm;
			dat[cnt++] = PTP_PROPGETSET_GETONLY;
			UINT8 *artist= (UINT8*)&dat[cnt];
			sprintf(artist, "%s", APP_WIFI_DEVICE_NAME);
			cnt+= (1 + strlen(artist) + 3)/4;
			printf("Product Name: %s,  %d items\n", artist, cnt);
			break;
		case USB_PIMA_DPC_COPYRIGHT_INFO: /* FW version */
			dat[cnt++] = LongStringForm;
			dat[cnt++] = PTP_PROPGETSET_GETONLY;
			UINT8 *fwversion= (UINT8*)&dat[cnt];
			sprintf(fwversion, "%s", appVersionGet());
			cnt+= (1 + strlen(fwversion) + 3)/4;
			printf("FW Version: %s,  %d items\n", fwversion, cnt);
			break;
		#if PTP_BURST_INTERVAL_EN
		case USB_PIMA_DPC_BURST_INTERVAL:
			dat[cnt++] = EnumForm;
			dat[cnt++] = PTP_PROPGETSET_GETSET;
			dat[cnt++] = ptpBurstCapInterval; /* cap speed : fps */
			dat[cnt++] = ptpBurstCapInterval; /* cap speed : fps */
			printf("burst capture interval:%d\n",ptpBurstCapInterval);
			dat[cnt++] = 1;  
			dat[cnt++] = 2;  
			dat[cnt++] = 3;
			dat[cnt++] = 4;
			dat[cnt++] = 5;
			dat[cnt++] = 10;
			break;
		#endif
		case USB_PIMA_DPC_CUSTOMER_NAME: /* to replace the customer name */
			cnt = app_ptpUsbCustomerSet(dat);
			break;
		case USB_PIMA_DPC_CUSTOME_VIDEO_PB_SUPPORT:
			printf("-- VIDEO_PB_SUPPORT cb get11(0x%x):",devPropCode);
			dat[cnt++] = EnumForm;
			dat[cnt++] = PTP_PROPGETSET_GETONLY;
			dat[cnt++] = PTP_VIDEO_PB_SUPOORT;/* current */
			dat[cnt++] = PTP_DPC_VIDEO_PB_OFF;/* default */
			dat[cnt++] = PTP_DPC_VIDEO_PB_OFF;
			dat[cnt++] = PTP_DPC_VIDEO_PB_ON;
			break;	
		case USB_PIMA_DPC_CONTRAST:
		case USB_PIMA_DPC_FOCUS_DISTANCE:
		case USB_PIMA_DPC_FUNCTIONAL_MODE:
		case USB_PIMA_DPC_BURST_INTERVAL:
		case USB_PIMA_DPC_TIMELAPSE_NUMBER:
		case USB_PIMA_DPC_FOCUS_METERING_MODE:
		case USB_PIMA_DPC_FOCAL_LENGTH:
		case USB_PIMA_DPC_TIMELAPSE_INTERVAL:
		#if !PTP_DZOOM_EN
		case USB_PIMA_DPC_DIGITAL_ZOOM:
		#endif	
		default:
			cnt = 0;
			printf("PIMA device property 0x%x isn't supported.\n", devPropCode);
			break;
	}
	dat[0] = cnt;
}

void appStill_PIMA_Apply_cb(UINT32 devPropCode, UINT32 *dat)
{
	uiPara_t* puiPara = appUiParaGet();
	
	#if ICAT_WIFI
	//while(0)
	{
		sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 50);
		printf("\n[PIMA Apply prop cb] Waiting for streaming stop !!\n");
	}
	#endif
	
	//DVS5M2 disable app size setting
	if(devPropCode==USB_PIMA_DPC_VIDEO_SIZE)/* prevent the time delay caused by sp5k msg send */
	{
		UINT8 i;
		printf("\wifi Setting (%d,%d,%d)\n", dat[1],dat[2],dat[3]);
		#if PTP_SUPPORT_VIDEO_SIZE_FPS
		printf("\t-> (%d,%d,%d)\n", dat[1],dat[2],dat[3]);
		#else
		printf("\t-> (%d,%d)\n", dat[1],dat[2]);
		#endif

		  if(appWiFiConnection_UtilityStateGet() > WIFI_UTILITY_CLOSE)
		  {
                	///pViewParam->vidsize = UIIdx;				
			if(dat[1]==1920 && dat[2]==1080 && dat[3]==30)
			{
				puiPara->SdvVideoNormalSize=0;
				puiPara->CdvVideoNormalSize=0;
			}	
			else if(dat[1]==1280 && dat[2]==720 && dat[3]==30)
			{
				puiPara->SdvVideoNormalSize=3;
				puiPara->CdvVideoNormalSize=3;
			}	
			else if(dat[1]==1280 && dat[2]==720 && dat[3]==60)
			{
				puiPara->SdvVideoNormalSize=2;
				puiPara->CdvVideoNormalSize=2;
			}	
			//else if(PTPDat[1]==640 && PTPDat[2]==480 && PTPDat[3]==120)
				//puiPara->SdvVideoNormalSize=6;				  
		  }
		
	
	for (i = 1 ; i < (sizeof(PTPUI_Map_SizeVid)/sizeof(PTPUI_Map_SizeVid[0])) ; i++)
        {
            if (PTPUI_Map_SizeVid[i].w == dat[1] && PTPUI_Map_SizeVid[i].h == dat[2]
			#if PTP_SUPPORT_VIDEO_SIZE_FPS
			&&PTPUI_Map_SizeVid[i].fps == dat[3]
			#endif
			)
            {
                 puiPara->VideoSize= PTPUI_Map_SizeVid[i].UiIndex;
                break;
            }
        }

		#if SENSOR_MODE_CHANGE_WITH_VIDEOSIZE
		/* sensor mode change, flicker issue : s */
		sp5kHostMsgSend(APP_UI_MSG_RTP_STOP,0);
		appVideoSizeChange(puiPara->VideoSize);
		sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 200);
		sp5kHostMsgSend(APP_UI_MSG_RTP_START,0);
		/* sensor mode change, flicker issue : e */
		#endif
		
		printf("0x%x : set param directly in apply cb (%d)\n",devPropCode, puiPara->VideoSize);
		goto EXIT;
	}

	if (devPropCode == USB_PIMA_DPC_DSC_OP_MODE) /* prevent the time delay caused by sp5k msg send */
	{
		printf("\t-> %d\n", dat[1]);
		printf("0x%x : set param directly in apply cb (%d)\n",devPropCode, dat[1]);

		app_PTP_Set_DscOpMode(dat[1]);
		#if ICAT_WIFI
		appWiFiStartConnection(RTP_STREM);
		#endif
		
		goto EXIT;
	}
	    printf("--apply cb video size=%d\n",puiPara->VideoSize);
	    appPTPInfo.PTPCode[appPTPInfo.PTPNum].PTP_code = devPropCode;
	    memcpy(appPTPInfo.PTPCode[appPTPInfo.PTPNum].PTP_dat, dat, (sizeof(UINT32) * 10));
	    appPTPInfo.PTPNum++;
	    sp5kHostMsgSend(APP_UI_MSG_PTP_CMD_SET, appPTPInfo.PTPNum);

	EXIT:
	return;
}

UINT32 appPtp_PIMA_File_Filter(UINT32 folder, const UINT8 *fname)
{
	UINT32 ret = FAIL;
	UINT8 ext[4];

	printf("%d, %s\n", folder, fname);

	if (folder&&(fname!=NULL)){
		#if SP5K_CDFS_OPT /* default supported folder : event, jpg, video */
		if(strcmp(fname, appCdfsFolderRootGet()) == 0 || strcmp(fname, "JPG") == 0||strcmp(fname, "VIDEO") == 0||strcmp(fname, "EVENT") == 0)
		{
			printf("scanning foler:%s\n",fname);
			ret = SUCCESS;
		}
		else
		{
			ret = FAIL;
		}
		#else
			ret = SUCCESS;	/* return FAIL will hide all files under the folder. */
		#endif
	}
	else if (fname) {
		strncpy( ext, fname + strlen( fname ) - 3, 3 );
		ext[3]=0;
		if (strcmp(ext, "JPG") == 0){
			ret = SUCCESS;
		}	
	#if 1 /* support video file or not */
		else if (strcmp(ext, "AVI") == 0)
		{
			ret = SUCCESS;
		}
		else if (strcmp(ext, "MOV") == 0)
		{
			ret = SUCCESS;
		}
	#endif
	}
	return ret;
}















UINT16
appStill_PTPToUI_Map(
    UINT32 PTPCode,
    UINT32* PTPDat
)
{
    UINT8 i;
    UINT16 UIIdx = 0;

    printf("\t video para-> %d  %d  %d\n", PTPDat[1],PTPDat[2],PTPDat[3]);

    switch (PTPCode)
    {
        case USB_PIMA_DPC_IMAGE_SIZE:
            for (i = 1 ; i < (sizeof(PTPUI_Map_Size)/sizeof(PTPUI_Map_Size[0])) ; i++)
            {
                if (PTPUI_Map_Size[i].w == PTPDat[1] && PTPUI_Map_Size[i].h == PTPDat[2])
                {
                    UIIdx = PTPUI_Map_Size[i].UiIndex;
                    break;
                }
            }
            break;

        case USB_PIMA_DPC_COMPRESSION_SETTING:
            for (i = 1 ; i < (sizeof(PTPUI_Map_Quality)/sizeof(PTPUI_Map_Quality[0])) ; i++)
            {
                if (PTPUI_Map_Quality[i].quality== PTPDat[1])
                {
                    UIIdx = PTPUI_Map_Quality[i].UiIndex;
                    break;
                }
            }
            break;

        case USB_PIMA_DPC_WHILE_BALANCE:
		case USB_PIMA_DPC_OPTIC_RATIO:	
            UIIdx = PTPUI_Map_WB[(PTPDat[1])];
            break;

        case USB_PIMA_DPC_F_NUMBER:
            for (i = 1 ; i < (sizeof(PTPUI_Map_Iris)/sizeof(PTPUI_Map_Iris[0])) ; i++)
            {
                if (PTPUI_Map_Iris[i].iris == PTPDat[1])
                {
                    UIIdx = PTPUI_Map_Iris[i].UiIndex;
                    break;
                }
            }
            break;

        case USB_PIMA_DPC_FOCUS_MODE:
            UIIdx = PTPUI_Map_AF[(PTPDat[1])];
            break;

        case USB_PIMA_DPC_EXPOSURE_METERING_MODE:
            UIIdx = PTPUI_Map_Metering[(PTPDat[1])];
            break;

        case USB_PIMA_DPC_FLASH_MODE:
            UIIdx = PTPUI_Map_Flash[(PTPDat[1])];
            break;

        case USB_PIMA_DPC_EXPOSURE_INDEX:
            for (i = 1 ; i < (sizeof(PTPUI_Map_ISO)/sizeof(PTPUI_Map_ISO[0])) ; i++)
            {
                if (PTPUI_Map_ISO[i].iso == PTPDat[1])
                {
                    UIIdx = PTPUI_Map_ISO[i].UiIndex;
                    break;
                }
            }
            break;

        case USB_PIMA_DPC_CAPTURE_DELAY:
            for (i = 1 ; i < (sizeof(PTPUI_Map_Delay)/sizeof(PTPUI_Map_Delay[0])) ; i++)
            {
                if (PTPUI_Map_Delay[i].delay== PTPDat[1])
                {
                    UIIdx = PTPUI_Map_Delay[i].UiIndex;
                    break;
                }
            }
            break;

        case USB_PIMA_DPC_EXPOSURE_BAIS_COMPENSATION:
            for (i = 1 ; i < (sizeof(PTPUI_Map_EV)/sizeof(PTPUI_Map_EV[0])) ; i++)
            {
                if (PTPUI_Map_EV[i].ev == (SINT16)PTPDat[1])
                {
                    UIIdx = PTPUI_Map_EV[i].UiIndex;
                    break;
                }
            }
            break;

        case USB_PIMA_DPC_STILL_CAPTURE_MODE:
            UIIdx = PTPUI_Map_Capture[(PTPDat[1])];
            break;

        case USB_PIMA_DPC_SHARPNESS:
            UIIdx = PTPUI_Map_Sharpness[(PTPDat[1])];
            break;

        case USB_PIMA_DPC_EFFECT_MODE:
            UIIdx = PTPUI_Map_Effect[(PTPDat[1])];
            break;

		#if 0 /* directly set in appStill_PIMA_Apply_cb() */
        case USB_PIMA_DPC_VIDEO_SIZE:

            for (i = 1 ; i < (sizeof(PTPUI_Map_SizeVid)/sizeof(PTPUI_Map_SizeVid[0])) ; i++)
            {
                if (PTPUI_Map_SizeVid[i].w == PTPDat[1] && PTPUI_Map_SizeVid[i].h == PTPDat[2])
                {
                    UIIdx = PTPUI_Map_SizeVid[i].UiIndex;
                    break;
                }
            }

            break;
		#endif

        case USB_PIMA_DPC_DATESTAMP:
            UIIdx = PTPUI_Map_Stamp[(PTPDat[1])];
            break;
    }

    printf("UIIdx = %d\n", UIIdx);

    return UIIdx;
}
UINT nWifiPhotoModeSave=0;
void
appStill_PTPToUI_Set(
    void
)
{
    UINT32 i;
    UINT32 PTPCode = appPTPInfo.PTPCode[0].PTP_code;
    UINT32 *PTPDat = appPTPInfo.PTPCode[0].PTP_dat;
    UINT16 UIIdx = 0;
	UINT32 index, mapVal;
	
	uiPara_t* puiPara = appUiParaGet();
	UINT8 nPhotoSizeIndex=0;	
	
    if(!appPTPInfo.PTPNum)
        return;

	printf("appStill_PTPToUI_Set:0x%x\n",PTPCode);

    UIIdx = appStill_PTPToUI_Map(PTPCode, PTPDat);
    if(UIIdx == UI_PTP_UNSUPPORT_CMD || UIIdx == UI_PTP_UNDEFINE_CMD)
    {
        printf("Unsupport Undefine cmd %d\n", PTPDat[1]);
    }
    else
    {
        switch (PTPCode)
        {
            case USB_PIMA_DPC_IMAGE_SIZE:
                puiPara->ImageSize= UIIdx;
		if(appWiFiConnection_UtilityStateGet() > WIFI_UTILITY_CLOSE)
		{
			 switch(UIIdx) 
			 {
				case 0:				
				case 1:
					nPhotoSizeIndex=UIIdx;
					break;
				case 2:
				case 3:
					nPhotoSizeIndex=UIIdx;
					break;
				default:
					nPhotoSizeIndex=1;
					break;

			 }
			  if(nPhotoSizeIndex<4)
			  {
			  	puiPara->SdvPhotoNormalSize=	nPhotoSizeIndex;		  	
			  	printf("--photo size setting is=%d =%d\n",puiPara->SdvPhotoNormalSize,UIIdx);
			  }
		}
                break;
            case USB_PIMA_DPC_COMPRESSION_SETTING:
                puiPara->ImageQuality= UIIdx;
                break;

            case USB_PIMA_DPC_WHILE_BALANCE:
			case USB_PIMA_DPC_OPTIC_RATIO:
    			puiPara->WBMode= UIIdx;
    			appStill_SetWB(puiPara->WBMode);
                break;

            case USB_PIMA_DPC_FOCUS_MODE:
                puiPara->af = UIIdx;
                break;

            case USB_PIMA_DPC_EXPOSURE_METERING_MODE:
                puiPara->AEMetering= UIIdx;
                appStill_SetMetering(puiPara->AEMetering);
                break;

            case USB_PIMA_DPC_FLASH_MODE:
                puiPara->flash = gStillCB.flashMode = UIIdx;
                break;

            case USB_PIMA_DPC_EXPOSURE_INDEX:
                puiPara->Iso= UIIdx;
                appStill_SetISO(puiPara->Iso);
                break;

            case USB_PIMA_DPC_EXPOSURE_BAIS_COMPENSATION:
                puiPara->AEMode= UIIdx;
                break;

            case USB_PIMA_DPC_CAPTURE_DELAY:
                puiPara->DriverMode= UIIdx;
		 		
		 if(appWiFiConnection_UtilityStateGet() > WIFI_UTILITY_CLOSE)
		 {
						
			if(UIIdx>0)
			{
				puiPara->SdvPhotoMode=UI_SCDV_PHOTO_MODE_SELF_TIMER;
				if(UIIdx==1)
					puiPara->SdvPhotoSelfTimer=UI_SCDV_PHOTO_SELF_TIMER_2S;
				if(UIIdx==2)
					puiPara->SdvPhotoSelfTimer=UI_SCDV_PHOTO_SELF_TIMER_5S;
				if(UIIdx==3)
					puiPara->SdvPhotoSelfTimer=UI_SCDV_PHOTO_SELF_TIMER_10S;				
			}	
			else
				puiPara->SdvPhotoMode=nWifiPhotoModeSave;
			
			printf("--puiPara->SdvPhotoMode=%d\n",puiPara->SdvPhotoMode);
		 }	
		  ////////////////////////////////////		
                break;

            case USB_PIMA_DPC_STILL_CAPTURE_MODE:
                puiPara->DriverMode= UIIdx;
                break;

            case USB_PIMA_DPC_SHARPNESS:
                puiPara->ImageEdge= UIIdx;
                appStill_SetSharpness(puiPara->ImageEdge);
                break;

            case USB_PIMA_DPC_EFFECT_MODE:
                puiPara->ImageEffect= UIIdx;
                appIqEffectSet(puiPara->ImageEffect);
                break;
	    #if 1 /* directly set in appStill_PIMA_Apply_cb() */
            case USB_PIMA_DPC_VIDEO_SIZE:
		  if(appWiFiConnection_UtilityStateGet() > WIFI_UTILITY_CLOSE)
		  {
                	///pViewParam->vidsize = UIIdx;				
			if(PTPDat[1]==1920 && PTPDat[2]==1080 && PTPDat[3]==30)
			{
				puiPara->SdvVideoNormalSize=0;
				puiPara->CdvVideoNormalSize=0;
			}	
			else if(PTPDat[1]==1280 && PTPDat[2]==720 && PTPDat[3]==30)
			{
				puiPara->SdvVideoNormalSize=3;
				puiPara->CdvVideoNormalSize=3;
			}	
			else if(PTPDat[1]==1280 && PTPDat[2]==720 && PTPDat[3]==60)
			{
				puiPara->SdvVideoNormalSize=2;
				puiPara->CdvVideoNormalSize=2;
			}	
			//else if(PTPDat[1]==640 && PTPDat[2]==480 && PTPDat[3]==120)
				//puiPara->SdvVideoNormalSize=6;
			//printf("--video size is %d\n",puiPara->SdvVideoNormalSize/*puiPara->SdvVideoNormalSize*/);		  
		  }
                break;
	    #endif
            case USB_PIMA_DPC_DATESTAMP:
                puiPara->StampMode= UIIdx;
                appStill_SetDateStamp(puiPara->StampMode, pUiSetting->dateTime);
                break;
            case USB_PIMA_DPC_BURST_NUMBER:
                printf("\t-> %d\n", (SINT16)PTPDat[1]);
				if(PTPDat[1]==0) PTPDat[1] = 5;  /* APP, enum wrong issue, tmp fix */
                gStillCB.btnum = PTPUI_Map_Burst[PTPDat[1]];
				if(gStillCB.btnum)
					puiPara->DriverMode= UI_DRIVE_BURST;
				else
					puiPara->DriverMode= UI_DRIVE_OFF;
				printf("burst num = %d, drive = %d\n",gStillCB.btnum,puiPara->DriverMode);
				if(puiPara->DriverMode!= UI_DRIVE_OFF)
				{
					puiPara->AEB= UI_AEB_OFF;
					appStill_SetAEB(puiPara->AEB);
				}
								
                break;
			case USB_PIMA_DPC_BAND_SELECTION:
				printf("\t-> %d\n", PTPDat[1]);
				puiPara->LightFreq= PTPDat[1];
				applightFrequencySetting(puiPara->LightFreq);
				break;
            case USB_PIMA_DPC_RGB_GAIN:
                printf("\t-> %d, %d, %d\n", PTPDat[1], PTPDat[2], PTPDat[3]);
                sp5kWbGain_t wb;
                wb.rgain = PTPDat[1];
                wb.grgain = wb.gbgain = PTPDat[2];
                wb.bgain = PTPDat[2];
                sp5kIqModeSet(SP5K_MODE_STILL_PREVIEW);
                sp5kImgWbGainSet(&wb);
                sp5kIqModeSetDone(SP5K_MODE_STILL_PREVIEW);
                break;
            case USB_PIMA_DPC_DATE_TIME:
                printf("PTPDateTime=%s\n", (UINT8 *)&PTPDat[1]);
                {
                    static UINT8 testSwRtc = 0;
                    tmx_t dtime;
                    UINT8 *str = (UINT8 *)&PTPDat[1];

                    dtime.tmx_year = 0;
                    for (i = 0 ; i < 4 ; i++)
                        dtime.tmx_year = dtime.tmx_year * 10 + str[i] - '0';
                    dtime.tmx_year -= 1900;
                    dtime.tmx_mon = (str[4] - '0') * 10 + str[5] - '0';
                    dtime.tmx_mday = (str[6] - '0') * 10 + str[7] - '0';
                    dtime.tmx_hour = (str[9] - '0') * 10 + str[10] - '0';
                    dtime.tmx_min = (str[11] - '0') * 10 + str[12] - '0';
                    dtime.tmx_sec = (str[13] - '0') * 10 + str[14] - '0';
                    printf("\t->%d/%d/%d %d:%d:%d\n", dtime.tmx_year, dtime.tmx_mon, dtime.tmx_mday, dtime.tmx_hour, dtime.tmx_min, dtime.tmx_sec);
					/*
					if (!testSwRtc) {
                        			sp5kRtcSwEmulationSet(1);
                       			 testSwRtc = 1;
                   			 }
					*/
		if(dtime.tmx_year >= 115&&dtime.tmx_year <= 134){
                    sp5kRtcDateTimeSet(SP5K_DATE_TIME_OPTION, &dtime);

                }
                }
                break;
    		case USB_PIMA_DPC_DSC_OP_MODE:
    			break;
            case USB_PIMA_DPC_ICATCH_VARIABLE:
                break;
			#if PTP_TIME_LAPSE_EN
			case USB_PIMA_DPC_TIMELAPSE_INTERVAL:
				printf("\t-> %d\n", PTPDat[1]);
				index = PTPDat[1];
				mapVal = PTPUI_Map_Cap_TimeLpsInt[PTPDat[1]];
				app_PTP_Set_TimeLapse_param(PTPCode,mapVal);
				break;
			case USB_PIMA_DPC_TIMELAPSE_NUMBER:
				printf("\t-> %d\n", PTPDat[1]);
				index = PTPDat[1];
				mapVal = PTPUI_Map_Cap_TimeLpsNum[PTPDat[1]];
				app_PTP_Set_TimeLapse_param(PTPCode,mapVal);
				break;
			case USB_PIMA_DPC_VIDEO_TIMELAPSE_INTERVAL:
				printf("\t-> %d\n", PTPDat[1]);
				index = PTPDat[1];
				mapVal = PTPUI_Map_Vid_TimeLpsInt[PTPDat[1]];
				app_PTP_Set_TimeLapse_param(PTPCode,mapVal);
				break;
			case USB_PIMA_DPC_VIDEO_TIMELAPSE_LENGTH:
				printf("\t-> %d\n", PTPDat[1]);
				index = PTPDat[1];
				mapVal = PTPUI_Map_Vid_TimeLpsLength[PTPDat[1]];
				app_PTP_Set_TimeLapse_param(PTPCode,mapVal);
				break;
			#endif
			#if PTP_BURST_INTERVAL_EN
			case USB_PIMA_DPC_BURST_INTERVAL:
				printf("\t-> %d\n", PTPDat[1]);
				ptpBurstCapInterval = PTPDat[1];
				break;
			#endif
            default:
                printf("PIMA device property 0x%x isn't supported.\n", PTPCode);
                break;
        }
    }

    appPTPInfo.PTPNum--;
    if(appPTPInfo.PTPNum)
    {
        memcpy(&appPTPInfo.PTPCode[0], &appPTPInfo.PTPCode[1],
            (sizeof(PTPCodeCmd_t) * appPTPInfo.PTPNum));
    }

    #if PIMA_DEBUG
    printf("%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
        puiPara->WBMode, puiPara->af, puiPara->AEMetering,
        gStillCB.flashMode, puiPara->ImageEffect, puiPara->DriverMode,
        puiPara->i, gStillCB.ae.iso, puiPara->Iso,
        gStillCB.remainImg, puiPara->ImageSize, puiPara->ImageEdge);
    #endif
}

void appStill_PIMA_DPC_Update(UINT8 nMenuItem)
{
	usbPIMADevicePropertyCode_e pimaDpcode = 0x0000;
#if 0
	switch(nMenuItem)
	{
	case VIEWMENU_CHANGE:
		break;
	case VIEWMENU_RESOLUTION:
	pimaDpcode = USB_PIMA_DPC_IMAGE_SIZE;
		break;
	case VIEWMENU_SELFTIMER:
	pimaDpcode = USB_PIMA_DPC_CAPTURE_DELAY;
		break;
	case VIEWMENU_ISO:
	pimaDpcode = USB_PIMA_DPC_EXPOSURE_INDEX;
		break;
	case VIEWMENU_VIDEO_SIZE:
	pimaDpcode = USB_PIMA_DPC_VIDEO_SIZE;
		break;
	case VIEWMENU_QUALITY:
	pimaDpcode = USB_PIMA_DPC_COMPRESSION_SETTING;
		break;
	case VIEWMENU_SHARPNESS:
	pimaDpcode = USB_PIMA_DPC_SHARPNESS;
		break;
	case VIEWMENU_DATESTAMP:
	pimaDpcode = USB_PIMA_DPC_DATESTAMP;
		break;
	case VIEWMENU_METERING:
	pimaDpcode = USB_PIMA_DPC_EXPOSURE_METERING_MODE;
		break;
	case VIEWMENU_DIGITAL_ZOOM:
	pimaDpcode = USB_PIMA_DPC_DIGITAL_ZOOM;
		break;
	case VIEWMENU_EFFECT:
	pimaDpcode = USB_PIMA_DPC_EFFECT_MODE;
		break;
	case VIEWMENU_WB:
	pimaDpcode = USB_PIMA_DPC_OPTIC_RATIO; /*USB_PIMA_DPC_WHILE_BALANCE;*/
		break;
	case VIEWMENU_EXPOSUREBIAS:
	pimaDpcode = USB_PIMA_DPC_EXPOSURE_BAIS_COMPENSATION;
		break;
	case VIEWMENU_VIDEO_DATESTAMP:
	case VIEWMENU_HIGH_LIGHT:
	case VIEWMENU_VIDEO_WB:
	case VIEWMENU_PHOTOFRAME:
	case VIEWMENU_STABILIZER:
	case VIEWMENU_AEB:
	case VIEWMENU_VIDEO_METERING:
	case VIEWMENU_FACETRACKING:
	case VIEWMENU_VIDEO_DIST:
	case VIEWMENU_VIDEO_RSC:
	case VIEWMENU_VIDEO_FACETRACKING:
	case VIEWMENU_FEATUREEFFECT:
	case VIEWMENU_VIDEO_QUALITY:
	case VIEWMENU_WDR:
	case VIEWMENU_HDR:
	case VIEWMENU_VIDEO_SEAMLESS_REC:
		printf("Responder doesn't support this property!\n");
	pimaDpcode = 0xFFFF;
	default:
		break;
	}
	printf("PIMA DPC update:0x%x!\n",pimaDpcode);
	if(pimaDpcode!=0xFFFF)
		appStill_PIMA_Send_Event(USB_PIMA_EC_DEVICE_PROP_CHANGED, pimaDpcode, 0, 0);
#endif	
}

void app_PTP_Set_DscOpMode(PTPDscOpMode_e dscOpMode )
{
	ptpPrevOpMode = ptpDscOpMode;
	ptpActOpMode = dscOpMode;
	ptpDscOpMode = ptpActOpMode;
	printf("DSC Op mode set:0x%x,Prev Op mode:0x%x\n",ptpDscOpMode,ptpPrevOpMode);
}

UINT8 app_PTP_Get_WiFiOpFunc()
{
	if(appCheckSdvMainMode())
	{
		return PTP_WIFI_FUNC_PREVIEW;
	}
	else
	{
		return PTP_WIFI_FUNC_VIDEO;
	}

	return ptpWiFiOpFunc;
}

UINT8 app_PTP_Get_DscOpMode(DscOpModeState_e opModeState)
{
	if(opModeState==MODE_PREV){
		printf("Prev Op mode get:0x%x\n",ptpPrevOpMode);
		return ptpPrevOpMode;
	}
	else if(opModeState==MODE_ACTIVE){
		printf("Active Op mode get:0x%x\n",ptpActOpMode);
		return ptpActOpMode;
	}
	else
		return ptpDscOpMode;
}


UINT8 app_PTP_ContRecMode_Get()
{
	return ptpContRecMode;
}



void app_PTP_Event2Queue(ptpEventQueue_s *ptpEventQueue,UINT32 EventCode, UINT32 param1, UINT32 param2, UINT32 param3)
{
	if(ptpEventQueue->TotalQNum >= PTP_EVENT_MAX_Q_NUM)
	{
		printf("[EvtQ] total queue number exceed the MAX\n");
		return;
	}

	ptpEventQueue->TotalQNum++;
	ptpEventQueue->EventQCode[ptpEventQueue->EventQIndex][0] = EventCode;
	switch (EventCode) {
		case PTP_EC_OBJECT_ADDED:
		case PTP_EC_OBJECT_REMOVED:
		case PTP_EC_OBJECT_INFO_CHANGED:
		case PTP_EC_REQUEST_OBJECT_TRANSFER:
			/* par1 is full path */
			ptpEventQueue->EventQCode[ptpEventQueue->EventQIndex][1] = (UINT32)sp5kMalloc(strlen((char *)param1)+1);
			strcpy((char *)ptpEventQueue->EventQCode[ptpEventQueue->EventQIndex][1], (char *)param1);
			break;
		default:
			ptpEventQueue->EventQCode[ptpEventQueue->EventQIndex][1] = param1;
			ptpEventQueue->EventQCode[ptpEventQueue->EventQIndex][2] = param2;
			ptpEventQueue->EventQCode[ptpEventQueue->EventQIndex][3] = param3;
			break;
	}
	
	printf("[EvtQ] event 0x%x queue,total Q : %d\n",
		ptpEventQueue->EventQCode[ptpEventQueue->EventQIndex][0], ptpEventQueue->TotalQNum);
	
	CIRCULAR_INCREASE(ptpEventQueue->EventQIndex,0,PTP_EVENT_MAX_Q_NUM-1); /* queue : [0] [1] ........ [8] [9] */
}

void app_PTP_EventQ_process(ptpEventQueue_s *ptpEventQueue)
{
	UINT8 i = 0, processedQindex = 0,processedQNum = 0;
	UINT8 curQindex = ptpEventQueue->CurQ1stIndex;
	UINT8 totalQNum = ptpEventQueue->TotalQNum;

	/*printf("curQindex:%d,totalQNum:%d\n",curQindex,totalQNum);*/

	processedQNum = totalQNum;
	
	if(totalQNum){

		printf("[EvtQ] total Queue b4 process:%d\n", totalQNum);

		for(i=curQindex;i < PTP_EVENT_MAX_Q_NUM;i++) /* check 1st Q NO. in the ring buffer */
		{
			if(ptpEventQueue->EventQCode[i][0]!= 0)
			{
				curQindex = i;
				printf("[EvtQ] current Queue no:%d\n", curQindex);
				break;
			}
		}
		
		for(i = curQindex ;i < totalQNum + curQindex;i++)
		{			
			printf("[EvtQ] event code = 0x%x,%d\n",ptpEventQueue->EventQCode[i][0],i);
			sp5kUsbPimaEventSend(ptpEventQueue->EventQCode[i][0],
									ptpEventQueue->EventQCode[i][1],
									ptpEventQueue->EventQCode[i][2],
									ptpEventQueue->EventQCode[i][3]);
			
			switch (ptpEventQueue->EventQCode[i][0]) {
				case PTP_EC_OBJECT_ADDED:
				case PTP_EC_OBJECT_REMOVED:
				case PTP_EC_OBJECT_INFO_CHANGED:
				case PTP_EC_REQUEST_OBJECT_TRANSFER:
					/* par1 is full path */
					sp5kFree((void *)ptpEventQueue->EventQCode[i][1]);
				default:
					break;
			}
			memset(ptpEventQueue->EventQCode[i], 0, 4*sizeof(UINT32)); /* clear the processed event Q */
			CIRCULAR_DECREASE(processedQNum,0,PTP_EVENT_MAX_Q_NUM);
			/*processedQindex = i;*/
			/*printf("[%d,%d]\n",processedQNum,processedQindex);*/
		}

		ptpEventQueue->EventQIndex = 0;
		ptpEventQueue->CurQ1stIndex = 0 ;
		ptpEventQueue->TotalQNum = processedQNum;

		/*printf("[EvtQ] QIndex after process:%d\n", ptpEventQueue->CurQ1stIndex);*/
		printf("[EvtQ] total Queue after process:%d\n", ptpEventQueue->TotalQNum);
	}
	else{
		printf("[EvtQ] no event queue !\n");
	}

}
UINT8 app_PTP_EventQNum_Get(ptpEventQueue_s ptpEventQueue)
{
	return ptpEventQueue.TotalQNum;
}

void app_PTP_EventQ_clear(ptpEventQueue_s *ptpEventQueue)
{
	UINT8 i;
	for(i=0;i < PTP_EVENT_MAX_Q_NUM;i++){
		memset(ptpEventQueue->EventQCode[i], 0, 4*sizeof(UINT32));
	}
	ptpEventQueue->TotalQNum = 0;
	printf("[EvtQ] clear event queue !\n");
}

void app_PTP_clear_EventQ(void)
{
	app_PTP_EventQ_clear(&sptpEventQueue);
}


UINT32 pimaDelFileCb(UINT8 *ppayload, UINT8 *pdata, UINT32 n)
{
	UINT32 ret = SUCCESS;

	printf("[PIMA]__delete file cb__ filenamexx\n");
	
	#if 1//SP5K_CDFS_OPT
	UINT8  filename[32];
	UINT8 *pIdx = NULL;
	strcpy(filename,pdata);	
	printf("--filename=%s\n",filename);
	if(pdata != NULL){
		strcpy(filename,pdata);
		pIdx = strchr(filename, '.');
		if(pIdx){
			switch(*(pIdx+1)){
				case 'a':
				case 'A':
					//appCdfsFileFolderSet(SP5K_DCF_FILETYPE_AVI); 
					appCdfsFileFolderSet(CDFS_FILE_FOLDER_VIDEO);
					printf(" delete a video\n");
					break;
				case 'j':
				case 'J':
					//appCdfsFileFolderSet(SP5K_DCF_FILETYPE_JPG);
					appCdfsFileFolderSet(SP5K_DCF_FILETYPE_JPG);
					printf(" delete jpg\n");
					break;
				case 'w':
				case 'W':
					//appCdfsFileFolderSet(SP5K_DCF_FILETYPE_MOV);
					appCdfsFileFolderSet(CDFS_FILE_FOLDER_VIDEO);
					printf(" delete w video\n");
					break;
				default:
					break;
			}
		}
		appCdfsFileNameToFileType(filename);
		appCdfsFileDel(filename);
	}
	#endif
	
	sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 10);
	ret = sp5kDcfFsInit(appActiveDiskGet(),0,0);
	if(ret!=SUCCESS)
	{
		printf("[ERR] %s(%d)\n",__FUNCTION__,__LINE__);
		return ret;
	}

	ret = sp5kDcfFsActive(appActiveDiskGet());
	
	if(ret!=SUCCESS)
	{
		printf("[ERR] %s(%d)\n",__FUNCTION__,__LINE__);
		return ret;
	}	

	if(IS_CARD_EXIST)
		appStill_PIMA_Send_Event(PTP_EC_STORAGE_INFO_CHANGED, /*SP5K_DRIVE_SD*/2, 0, 0);
	else
		appStill_PIMA_Send_Event(PTP_EC_STORAGE_INFO_CHANGED, /*SP5K_DRIVE_NAND*/1, 0, 0);

	return ret;
}

#endif  /*ICAT_WIFI*/
