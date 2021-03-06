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

 #define HOST_DBG 1
#include "app_com_def.h"
#include "app_com_api.h"

#include "sp5k_global_api.h"
#include "sp5k_disk_api.h"
#include "sp5k_disp_api.h"
#include "sp5k_sensor_api.h"
#include "sp5k_aud_api.h"
#include "sp5k_ae_api.h"
#include "sp5k_awb_api.h"
#include "sp5k_gfx_api.h"
#include "sp5k_rsvblk_api.h"
#include "sp5k_media_api.h"
#include "sp5k_capture_api.h"
#include "sp5k_usb_api.h"

#include "app_hw_def.h"
#include "app_tvlcd.h"
#include "app_exif.h"
#include "app_osd_api.h"
#include "app_aaa.h"

#include "customization/dev_init.h"
#include "gpio_custom.h"	/* %REVIEW% for gpioInitialCfgSet() */
#include "app_lens_api.h"	/* %REVIEW% : just for appLensInit() */
#include "app_sound.h"
#include "app_dev_plug.h"
#include "app_battery.h"
#include "app_rtc.h"
#include "app_gSensor.h"
#include "app_still.h"
#include "app_jpg_draw.h"
#include "app_calib_api.h"
#include "app_fast_boot.h"
#include "app_gSensor_def.h"
#include "app_view_param.h"
#include "app_ui_para.h"
#include "app_video.h"
#include "app_ts_init.h"
#include "app_ts_gui.h"
#include "app_ts_evt.h"
#include "app_diq.h"
#include "dev_init.h"
#include "app_view_param_def.h"
#if SP5K_MOTION_DETECT
#include "app_md.h"
#endif

#include "app_osd_api_lcm.h"
#include "app_osd_draw_lcm.h"
#include "app_scdv_ui_para_def.h"
#include "app_rf_def.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define PURE_FW_POWER_ON   0 //please turn off FAST_BOOT is PURE_FW_POWER_ON is turn on
#define PURE_TESTMODE_POWER_ON   0

#define FAST_BOOT  1
#define FIF_ENABLE 1

#define RTC_CHECK_SIG	"A:\\RO_RES\\ENGSMALL.SFN"


typedef enum {
	POWER_ON_PHASE_DISK_DETECT = 0x0000,
	POWER_ON_PHASE_OSD_PRELOAD,
	POWER_ON_PHASE_BTN_INIT
} appPowerOnPhase_e;
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define WRITE8(a,b)	*(volatile UINT8*)(a) = (b)

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef struct appPowerOnInfo_s {
	BOOL  isPbPowerOn;
	BOOL  isMutePowerOn;
	BOOL  isTestModePowerOn;
	BOOL  isAudioPowerOn;
	BOOL  isLensFastBoot;
	BOOL  isPvFreeze;
	BOOL  isPowerOnForCharging;
} appPowerOnInfo_t;
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
static appPowerOnInfo_t appPowerOnInfo = {FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE};
uiParamSetting_t uiParam;
uiParamSetting_t *pUiSetting = &uiParam;
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern UINT8 timelapse_act;
extern UINT32 appVbusInState;
extern void appAEALGInit(void);
extern void appBtnInit(void);
extern void appBackgroundTimerInit(void);
extern UINT8 appBatteryCharge_Init(void);
extern UINT32 fsFifOpen(const UINT8 *fileName);
extern void usbVndrIcatPreCaptureCallbackSet(void *pfunc);

#if Calibration_ADC_KEY
extern UINT32 appBtnLoad();
extern UINT32 appBtnCalib();
extern UINT32 appBtnSave();
#endif
extern void appIqPowerOnInit(void);
extern void Icatsnap3Acb(void);

extern UINT8 appVidSenModeGet(UINT16 *sensormode);
extern UINT32 _IqGetCurrISO(IQ_STATE_t state);

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
void appUiParamInit(void);
void devDiskIoScanStart(void);
//extern unsigned _dbg_proc_ps_en(unsigned en);
//extern void _dbg_proc_ps_dump(void);
/**************************************************************************
 *  Function Name: Program All                                       *
 **************************************************************************/
BOOL appProgAllCheck(void)
{
	UINT32 fd;
	fd = sp5kFsFileOpen(RES_RTC_CHECK_SIG, SP5K_FS_OPEN_RDONLY);
	if (fd==0)
		return 0;
	sp5kFsFileClose(fd);
	printf("ProgAllCheck: TRUE!!\n");
	return 1;
}

void appProgAllClear(void)
{
	sp5kFsFileDelete(RES_RTC_CHECK_SIG);
}

/*-------------------------------------------------------------------------
 *  File Name : usbPrevResolutionChgCb
 *------------------------------------------------------------------------*/
void
usbPrevResolutionChgCb(
        UINT32 width,
        UINT32 height
)
{
        UINT32 w = width, h = height, mode = 0x30;
		UINT32 fac2nd = 0x11;

		printf("host w=%x,h=%x",width,height);
		#if SPCA6330
		if(width == 0 && height == 0) {
			sp5kSystemCfgSet(SP5K_CDSP_2NDPATH_FACTOR_CFG, 0x11);
			return;
		}
		#endif
		/*should make the w and h most relative to
		the sensor,s size.*/
		/*set for 0V5633 sensor*/

#if SP5K_SENSOR_OV5633
        if ((width==1280)&&(height==720)) {
                w = 1280;
                h = 720;
                mode = 0x30;  /*1280*960*/
        }
        else if ((width==640)&&(height==480)) {
                w = 640;
                h = 480;
                mode = 0x30;
        }
		else if ((width==320)&&(height==240)){
                w = 320;
                h = 240;
                mode = 0x30;
        }
		else if ((width==1920)&&(height==1080)){
                w = 1920;
                h = 1080;
                mode = 0x32;
				fac2nd = 0x22;
        }
		else if ((width==1600)&&(height==1200)){
                 w = 640;
                h = 480;
                mode = 0x30;
        }
#elif SP5K_SENSOR_MT9J003
		if (((width==640)&&(height==480))||((width==320)&&(height==240)))
		{
			w = width;
            h = height;
            mode = 0x30;
		}
		else if ((width==1600)&&(height==1200))
		{
			w = 640;
			h = 480;
			mode = 0x30;
		}
		else
		{
			w = 1280;
            h = 720;
            mode = 0x32;
		}
#elif SP5K_SENSOR_SONYIMX083
		if (((width==640)&&(height==480))||((width==320)&&(height==240)))
		{
			w = width;
            h = height;
            mode = 0x30;
		}
		else if ((width==1600)&&(height==1200))
		{
			w = 640;
			h = 480;
			mode = 0x30;
		}
		else if ((width==1920)&&(height==1080))
		{
			w = 1920;
			h = 1080;
			mode = 0x33;
			fac2nd = 0x22;
		}
		else
		{
			w = 1280;
            h = 720;
            mode = 0x32;
		}
#elif SP5K_SENSOR_OV2715
		if (((width==640)&&(height==480))||((width==320)&&(height==240)))
		{
			w = width;
            h = height;
            mode = 0x33;
		}
		else if ((width==1600)&&(height==1200))
		{
			w = 640;
			h = 480;
			mode = 0x33;
		}
		else if ((width==1920)&&(height==1080))
		{
			w = width;
			h = height;
			mode = 0x30;
			fac2nd = 0x22;
		}
		else
		{
			w = 1280;
            h = 720;
            mode = 0x31;
		}
#endif
		#if SPCA6330
		sp5kSystemCfgSet(SP5K_CDSP_2NDPATH_FACTOR_CFG, fac2nd);
        #endif
		sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_WIDTH, w);
        sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_HEIGHT, h);
        sp5kSensorModeCfgSet(SP5K_MODE_VIDEO_PREVIEW, mode);
		sp5kAeModeSet(SP5K_AE_MODE_INFO_ONLY);   /*junqiong.hu@20121130*/
		sp5kAwbModeSet(SP5K_AWB_MODE_INFO_ONLY);  /*junqiong.hu@20121130*/
        sp5kModeSet(SP5K_MODE_VIDEO_PREVIEW);
        sp5kModeWait(SP5K_MODE_VIDEO_PREVIEW);
}

/**************************************************************************
 *  Function Name: _powerOnStateGet                                        *
 **************************************************************************/

UINT32
_powerOnStateGet(
	void
)
{
	UINT32 state = APP_STATE_NULL;
	uiPara_t* puiPara = appUiParaGet();

	/*
		Definition for USB boot which defined in ".\customization\hostfw\makefile"
	*/

#if (OPT_USB_BOOT || CMD_GDB)	/*add gdb */
	printf("Power on with USB boot or GDB\n");
     #if CMD_GDB
	if (getDgbType() == 1)
     #endif
 	{
		appCalibAPITestSettingSet(1);
 	}
#endif

	if (IS_USB_IN && (appCalibUsbSettingGet() || appCalibAPITestSettingGet()))
	{
		state = APP_STATE_USB_CREATE;
	}
	else
	{
		#if POWERON_PB_WITHOUT_SENSOR
			appPowerOnInfo.isPbPowerOn =1 ;
		#endif
		#if COMPEL_POWEROFF_CHARGE
		if(appGetBoot4ChargingFlag())
			appPowerOnInfo.isPowerOnForCharging =1 ;
		#endif
		if( appPowerOnInfo.isPbPowerOn )
		{
			state = APP_STATE_PB_MAIN;
		}
		else if(IO_KEY_OK_GET == TRUE)
		{
			//upgreade fw from sd card
			state = APP_STATE_SCDV_CARD_ISP;
		}
		#if COMPEL_POWEROFF_CHARGE
		else if(appPowerOnInfo.isPowerOnForCharging)
		{
			state = APP_STATE_USB_BATTERY_CHARGING;
		}
		#endif
		#if CAM_TYPE_CVR
		else
		{
			if((puiPara->TimeLapse != UI_TIMELAPSE_OFF) && timelapse_act)
			{
				state = APP_STATE_STILL_PREVIEW;
			}
			else
			{
				//state = APP_STATE_VIDEO_PREVIEW;
				if(puiPara->ScdvMainMode == UI_SCDV_MAIN_MODE_SPORT)
				{
					state = APP_STATE_SPORTDV_VIDEO_PREVIEW;
				}
				else// if(puiPara->ScdvMainMode == UI_SCDV_MAIN_MODE_CAR)
				{
					state = APP_STATE_CARDV_VIDEO_PREVIEW;
				}
				#ifdef USE_AUTO_WIFI_ON
				state = APP_STATE_WIFI_CONNECTION;
				#endif
			}
		}
		#else
		else if (puiPara->CurViewMode == AUDIO_MODE)
		{
			state = APP_STATE_AUDIO_REC;
		}
		else if (puiPara->CurViewMode == VIDEO_MODE)
		{
			state = APP_STATE_VIDEO_PREVIEW;
		}
		else
		{
			state = APP_STATE_STILL_PREVIEW;
		}
		#endif
	}

	#ifdef USE_PURE_CODE
	state = APP_STATE_NULL;
	#endif

	return state;
}

/**************************************************************************
 *  Function Name: _powerOnKeyGet                                        *
 **************************************************************************/

void
_powerOnKeyGet(
	void
)
{
	UINT32 isPlayKeyPowerOn;

	sp5kPwrDetectGet(0, &isPlayKeyPowerOn);

	if (isPlayKeyPowerOn == 1)
	{
		appPowerOnInfo.isPbPowerOn = TRUE;
	}
	#if(!(defined USE_DVS5M9_HW || defined USE_DVS5M2_HW))
	if ((!appPowerOnInfo.isPbPowerOn) && (IS_KEY_LEFT_PRESSED) )
	{
		appPowerOnInfo.isTestModePowerOn = TRUE;
	}
	#endif
}
/**************************************************************************
 *  Function Name: appHostFastBootInit                                        *
 **************************************************************************/
void
appLensFastBootInit(
	void
)
{
#if FAST_BOOT
	UINT32 ret;
	UINT8 mode,batType,batLevel;

	//_dbg_proc_ps_en(2);

	sp5kAdcInit();
	appLensInit();

	sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 20);

	profLogAdd(0, "appLensFastBootInit");
	gpioInitialCfgSet();
	_powerOnKeyGet();

	ret = appFastBootLensInfoGet(&mode, &batType, &batLevel);
	printf("fast boot err=%x (mode : %d, batType = %d, batLevel = %d)\n", ret, mode, batType, batLevel);
	if (ret !=SUCCESS || batType>=BATT_TYPE_MAX)
	{
		return;
	}

	#if KIT_WITH_LENS
	if(!appPowerOnInfo.isPbPowerOn && (mode != AUDIO_MODE) && !IS_USB_IN)
	{
		#ifndef EVB_CFG_COUGAR
		if (appBatteryAdcValueGet() < appLensBootBatteryLevelGet())
		{
			printf("[ERR] fail to lens boot (%d : %d)\n", appBatteryAdcValueGet(),appLensBootBatteryLevelGet() );
			return;
		}
		#endif
		profLogAdd(0, "*fb lens init");
		appPowerOnInfo.isLensFastBoot = TRUE;
		if (SUCCESS==appLensCrashRecover())
		{
			appLensBoot();
		}
	}
	#endif

#endif
}
/**************************************************************************
 *  Function Name: appHostFastBootInit                                        *
 **************************************************************************/
void
appHostFastBootInit(
	void
)
{
#if FAST_BOOT
	DBG_PRINT("%s (%d)\n",__FUNCTION__, tmrMsTimeGet());
	profLogAdd(0, "appHostFastBootInit start");

	appTvLcdInit();

	appTvLcdStart(TRUE);

	appExifInit();

	profLogAdd(0, "ui param init");
	appUiParamInit();

	appBatteryCheckInit();
	profLogAdd(0, "appHostFastBootInit end");
#endif
}

/**************************************************************************
 *  Function Name: appFifInit                                        *
 **************************************************************************/

void
appFifInit(
	void
)
{
#if 1
	profLogAdd(0, "FIF init:s");

	if (fsFifOpen(FIFB_UI_DATA)==0)
	{
		profLogAdd(0, "uidata load fail..");
		printf("[ERR] load %s fail..\n",FIFB_UI_DATA);
	}
	profLogAdd(0, "uidata fif init -e");
	if (fsFifOpen(FIF_COLD_DATA)==0)
	{
		profLogAdd(0, "colddata load fail..");
		printf("[ERR] load %s fail..\n",FIF_COLD_DATA);
	}
	profLogAdd(0, "colddata fif init -e");

	profLogAdd(0, "FIF init:e");
#endif
}
/**************************************************************************
 *  Function Name: appUiParamInit                                        *
 **************************************************************************/
//CIQ
#if SP5K_CIQ

#if 0
cus_tablename_t custbl[] = {
    {"CIQRES.BIN"} /*, {"CIQRES1.BIN"}, {"CIQRES2.BIN"} , {"CIQRES3.BIN"} ,*/
};
#endif

cus_tablename_t custbl_pview[] = {
    {"PCIQ00.BIN"},
    {"PCIQ01.BIN"},
    {"PCIQ02.BIN"},
    {"PCIQ03.BIN"},
    {"PCIQ04.BIN"},
    {"PCIQ05.BIN"},
    {"PCIQ06.BIN"},
};

cus_tablename_t custbl_snap[] = {
    {"SCIQ00.BIN"},
    {"SCIQ01.BIN"},
    {"SCIQ02.BIN"},
    {"SCIQ03.BIN"},
    {"SCIQ04.BIN"},
    {"SCIQ05.BIN"},
    {"SCIQ06.BIN"},
};

cus_tablename_t custbl_pview720p60fps[] = {
    {"PCIQ30.BIN"},
    {"PCIQ31.BIN"},
    {"PCIQ32.BIN"},
    {"PCIQ33.BIN"},
    {"PCIQ34.BIN"},
    {"PCIQ35.BIN"},
    {"PCIQ36.BIN"},
};


/*
renaming rule
P/S CIQ x1 x2    for x1 -> resolution , x2 -> ISO
 RESOLUTION
 SENSOR_MODE_1080P               0
 SENSOR_MODE_1080P_60FPS    1
 SENSOR_MODE_720P_30FPS      2
 SENSOR_MODE_720P_60FPS      3
 SENSOR_MODE_VGA_120FPS     4
 SENSOR_MODE_QVGA_240FPS   5

ISO     No
50       0
100     1
200     2
400     3
800     4
1600   5
3200   6
*/

cus_tablename_t* Cus_Bin_test(IQ_STATE_t IQState)
{
    UINT32 iso_indx = _IqGetCurrISO(IQState);
    UINT16 sen_mode;

    //printf("<lester> Cus_Bin_test mode = %x iso_indx=%x senmode=%x \n",IQState ,iso_indx ,sen_mode);

    if(appVidSenModeGet(&sen_mode)==FAIL)
    {
       printf("CIQ: appVidSenModeGet Fail\n");
        return(NULL);
    }

switch(IQState) {
	case OPSTATE_PV_INIT:
	case OPSTATE_PV:
	case OPSTATE_VIDEO:
		if(sen_mode == SENSOR_MODE_720P_60FPS)
			return(&custbl_pview720p60fps[iso_indx]);
		else
			return(&custbl_pview[iso_indx]);
		break;
	case OPSTATE_SNAP:
		return(&custbl_snap[iso_indx]);
		break;

	default:
		break;
}

#if 0 //sensor mode sample
    if(sen_mode==SENSOR_MODE_1080P)
        printf("CIQ:SENSOR_MODE_1080P\n");
    else if(sen_mode==SENSOR_MODE_720P_30FPS)
        printf("CIQ:SENSOR_MODE_720P_30FPS\n");
    else if(sen_mode==SENSOR_MODE_720P_60FPS)
        printf("CIQ:SENSOR_MODE_720P_60FPS\n");
    else if(sen_mode==SENSOR_MODE_VGA_120FPS)
        printf("CIQ:SENSOR_MODE_VGA_120FPS\n");
    else if(sen_mode==SENSOR_MODE_QVGA_240FPS)
        printf("CIQ:SENSOR_MODE_QVGA_240FPS\n");
#endif

#if 0 //iso index sample
    if(iso_indx==3)
        return(&custbl[0]);
    else
#endif

        return(NULL);
}
#endif

UINT32
appRsvBCheck
(
	void
)
{
	UINT32 ret = SUCCESS;
	UINT32 hFif,fd;
	//return FAIL;
	hFif = fsFifOpen("B:\\USER_RES\\UIDATA.BIN");
	fsFifClose(hFif);
	if (hFif ==0)
	{
		RPrintf("READ UIDATA error\n");
		return FAIL;
	}


	#if ICAT_WIFI
	fd = sp5kFsFileOpen("B:\\UDF\\APMODE.CFG", SP5K_FS_OPEN_RDONLY);
	sp5kFsFileClose(fd);
	if (fd==0)
	{
		RPrintf("READ APMODE.CFG error\n");
		return FAIL;
	}
	fd = sp5kFsFileOpen("B:\\UDF\\HAPD0.CFG", SP5K_FS_OPEN_RDONLY);
	sp5kFsFileClose(fd);
	if (fd==0)
	{
		RPrintf("READ HAPD0.CFG error\n");
		return FAIL;
	}
	fd = sp5kFsFileOpen("B:\\UDF\\SSID.CFG", SP5K_FS_OPEN_RDONLY);
	sp5kFsFileClose(fd);
	if (fd==0)
	{
		RPrintf("READ SSID.CFG error\n");
		return FAIL;
	}
	fd = sp5kFsFileOpen("B:\\UDF\\SSID_PW.CFG", SP5K_FS_OPEN_RDONLY);
	sp5kFsFileClose(fd);
	if (fd==0)
	{
		RPrintf("READ SSID_PW.CFG error\n");
		return FAIL;
	}


	#if 1
	/*
	fd = sp5kFsFileOpen("B:\\USER_RES\\UDF.DAT", SP5K_FS_OPEN_RDONLY);
	sp5kFsFileClose(fd);
	if (fd==0)
	{
		RPrintf("READ B:\\USER_RES\\UDF.DAT error\n");
		return FAIL;
	}
	fd = sp5kFsFileOpen("B:\\USER_RES\\UDF.BIN", SP5K_FS_OPEN_RDONLY);
	sp5kFsFileClose(fd);
	if (fd==0)
	{
		RPrintf("READ B:\\USER_RES\\UDF.BIN error\n");
		return FAIL;
	}*/
	fd = sp5kFsFileOpen("B:\\UDF\\CUSTOMER_ID.CFG", SP5K_FS_OPEN_RDONLY);
	sp5kFsFileClose(fd);
	if (fd==0)
	{
		RPrintf("READ CUSTOMER_ID.CFG error\n");
		return FAIL;
	}
	#endif
	#endif
	return ret;
}
UINT32
appRsvBCheckSecond
(
	void
)
{
	UINT32 ret = SUCCESS;
	UINT32 hFif,fd;
	//return FAIL;
	hFif = fsFifOpen("B:\\USER_RES\\UIDATA.BIN");
	fsFifClose(hFif);
	if (hFif ==0)
	{
		RPrintf("READ UIDATA error\n");
		return FAIL;
	}


	#if ICAT_WIFI
	fd = sp5kFsFileOpen("B:\\UDF\\APMODE.CFG", SP5K_FS_OPEN_RDONLY);
	sp5kFsFileClose(fd);
	if (fd==0)
	{
		RPrintf("READ APMODE.CFG error\n");
		return FAIL;
	}
	fd = sp5kFsFileOpen("B:\\UDF\\HAPD0.CFG", SP5K_FS_OPEN_RDONLY);
	sp5kFsFileClose(fd);
	if (fd==0)
	{
		RPrintf("READ HAPD0.CFG error\n");
		return FAIL;
	}
	fd = sp5kFsFileOpen("B:\\UDF\\SSID.CFG", SP5K_FS_OPEN_RDONLY);
	sp5kFsFileClose(fd);
	if (fd==0)
	{
		RPrintf("READ SSID.CFG error\n");
		return FAIL;
	}
	fd = sp5kFsFileOpen("B:\\UDF\\SSID_PW.CFG", SP5K_FS_OPEN_RDONLY);
	sp5kFsFileClose(fd);
	if (fd==0)
	{
		RPrintf("READ SSID_PW.CFG error\n");
		return FAIL;
	}


	#if 1
	/*
	fd = sp5kFsFileOpen("B:\\USER_RES\\UDF.DAT", SP5K_FS_OPEN_RDONLY);
	sp5kFsFileClose(fd);
	if (fd==0)
	{
		RPrintf("READ B:\\USER_RES\\UDF.DAT error\n");
		return FAIL;
	}
	fd = sp5kFsFileOpen("B:\\USER_RES\\UDF.BIN", SP5K_FS_OPEN_RDONLY);
	sp5kFsFileClose(fd);
	if (fd==0)
	{
		RPrintf("READ B:\\USER_RES\\UDF.BIN error\n");
		return FAIL;
	}*/
	fd = sp5kFsFileOpen("B:\\UDF\\CUSTOMER_ID.CFG", SP5K_FS_OPEN_RDONLY);
	sp5kFsFileClose(fd);
	if (fd==0)
	{
		RPrintf("READ CUSTOMER_ID.CFG error\n");
		return FAIL;
	}
	#endif
	#endif
	return ret;
}
UINT32
appRsvBRecovery
(
	void
)
{
	UINT32 ret;
	if (appRsvBCheck() == SUCCESS)
	{
		RPrintf("B rsv check ok,needn't recovery\n");
		return FAIL;
	}
	RPrintf("Recover RSV B... ");
	ret = sp5kRsvBlkFormat(SP5K_DRIVE_RSVB);
	if (ret != SUCCESS)
	{
		RRPrintf("format err=%x\n", ret);
		return ret;
	}
	/* recover your UI data here by Func sp5kFsFileCopy*/
	ret = sp5kFsFileCopy("A:\\RO_RES\\UIDATA.BIN", "B:\\USER_RES\\UIDATA.BIN");
	if (ret != SUCCESS)
	{
		RRPrintf("cp UIDATA.BIN err=%x\n", ret);
	}
	#if ICAT_WIFI
	ret = sp5kFsFileCopy("A:\\ADF\\APMODE.CFG", "B:\\UDF\\APMODE.CFG");
	if (ret != SUCCESS)
	{
		RRPrintf("cp APMODE.CFGerr=%x\n", ret);
	}
	ret = sp5kFsFileCopy("A:\\ADF\\HAPD0.CFG", "B:\\UDF\\HAPD0.CFG");
	if (ret != SUCCESS)
	{
		RRPrintf("cp HAPD0.CFGerr=%x\n", ret);
	}
	ret = sp5kFsFileCopy("A:\\ADF\\SSID.CFG", "B:\\UDF\\SSID.CFG");
	if (ret != SUCCESS)
	{
		RRPrintf("cp SSID.CFGerr=%x\n", ret);
	}
	ret = sp5kFsFileCopy("A:\\ADF\\SSID_PW.CFG", "B:\\UDF\\SSID_PW.CFG");
	if (ret != SUCCESS)
	{
		RRPrintf("cp SSID_PW.CFGerr=%x\n", ret);
	}
	/*ret = sp5kFsFileCopy("A:\\ADF\\UDF.BIN", "B:\\UDF\\UDF.BIN");
	if (ret != SUCCESS)
	{
		RRPrintf("cp UDF.BIN err=%x\n", ret);
	}
	ret = sp5kFsFileCopy("A:\\ADF\\UDF.DAT", "B:\\USER_RES\\UDF.DAT");
	if (ret != SUCCESS)
	{
		RRPrintf("cp UDF.DAT err=%x\n", ret);
	}*/
	ret = sp5kFsFileCopy("A:\\ADF\\CUSTOMER_ID.CFG", "B:\\UDF\\CUSTOMER_ID.CFG");
	if (ret != SUCCESS)
	{
		RRPrintf("cp CUSTOMER_ID.CFG err=%x\n", ret);
	}
	#endif

	RPrintf("ReopenYY...\n");
	#if 1
	ret = appRsvBCheckSecond();
	if (ret == FAIL)
	{ /* error */
		RPrintf("reopen error,recovery fail\n");
	}
	#endif
	return ret;
}

void
appUiParamInit(
	void
)
{
	uiPara_t *psPara = appUiParaGet();
	uiPara_t* puiPara = appUiParaGet();

	profLogAdd(0, "ui param init:s");
	UINT32 ret;
	#if 1
	ret = appRsvBRecovery();
	if(ret == FAIL)
	{
		printf("no need recover or recover fail\n");
	}
	else
	{
		printf("recover success\n");
	}
	#endif
	appFifInit();

	appGlobalInit();
	appUiParaInit(PARA_INIT_FROM_UDF);
	/*
	appUserSettingLoad(pUiSetting);
	*/
	appCalibDataLoad();

	UiParamViewInit();
	appSundVolumeSet(psPara->Volume);
	if(psPara->CurViewMode == AUDIO_MODE)
	{
		appPowerOnInfo.isAudioPowerOn = TRUE;
	}
	if(puiPara->TimeLapse == UI_TIMELAPSE_OFF)
	{
		timelapse_act = 0;
	}
	profLogAdd(0, "ui param init:e");

    #if SP5K_CIQ
    appIqCustomCBRegister(Cus_Bin_test);
    #endif

	appIqPowerOnInit();

}
/**************************************************************************
 *  Function Name: appFrontInit                                        *
 **************************************************************************/
void
appFrontInit(
	void
)
{
#if 0
	profLogAdd(0, "front init");
	/* Not used anymore !! */
#endif
}
/**************************************************************************
 *  Function Name: appAudioInit                                        *
 **************************************************************************/
void
appAudioInit(
	void
)
{
	sp5kAudDevSrcSet(SP5K_AUD_DEV_PLAY, AUD_PLAY_SRC_FOR_LCD);
	sp5kAudDevSrcSet(SP5K_AUD_DEV_REC, AUD_REC_SRC);
}
/**************************************************************************
 *  Function Name: appPanelInit                                        *
 **************************************************************************/
void
appPanelInit(
	void
)
{
#ifndef FAST_BOOT
	uiPara_t* puiPara = appUiParaGet();
#endif

	profLogAdd(0, "panel init");
#if FAST_BOOT
	appTvLcdStart(FALSE);
#else
	appDispMode_e dispMode;
	dispMode = IS_TV_IN? (puiPara->TVMode) : APP_DISP_LCD;
	appLcdTvSwitch(dispMode,IS_TV_IN);

#endif
	#if 0
#if FUNC_HOST_TOUCH_PANEL /* Touch panel initial */
	appTouchScreenInit();
	guiTpInit();
	guiTpEnSet(1);
	guiObjInit();
	guiDbgEnSet(0);

	/* The priority of host thread is 16 */
	sp5kOsThreadCreate("app_ts", appTsTask, 0, 20, 20, 0, TX_AUTO_START);
		#endif
#endif
}
/**************************************************************************
 *  Function Name: appAeAwbInit                                        *
 **************************************************************************/
void
appAeAwbInit(
	void
)
{
	profLogAdd(0, "AA nit");
    DynIqTaskInstall(SP5K_DIQ_GAMMA|(SP5K_DIQ_WDR<<1));
    DynIqTaskInit();
	appAeInit();
	appAwbInit();
}
/**************************************************************************
 *  Function Name: appDiskDetect                                        *
 **************************************************************************/
void
appDiskDetect(
	void
)
{
 	/* Disk driver start to detect */
	profLogAdd(0, "disk detect -s");
	sp5kDiskMount(SP5K_START_DETECT);
	profLogAdd(0, "disk detect -e");
}
/**************************************************************************
 *  Function Name: appStartUpImageShow                                        *
 **************************************************************************/
void
appPowerOnLogoShow(
	void
)
{
	uiPara_t* puiPara = appUiParaGet();
	//check start up image setting is ON
	profLogAdd(0, "show logo-s");

	if (puiPara->OpenPic == POWERON_LOGO_NONE) return;

	profLogAdd(0, "show logo-show");
	sp5kGfxAttrSet(SP5K_GFX_REFRESH_ACTIVE,0, 0, 0, 0);
	//show logo
	appOsd_ColorDrawShapeColorSet(PAL_BLACK, PAL_BLEND_100, PAL_BLACK, PAL_BLEND_100);
	appOsdLib_PFLayerShapeDraw(APP_OSD_SHAPE_RECT,0, 0, 320, 240, 0, 0);
	if ((puiPara->OpenPic == POWERON_LOGO_CUSTOM) && appFileExistCheck(RESB_JPG_STARTUPCUSTOM)) {
		appJpgDraw_PipLayer(RESB_JPG_STARTUPCUSTOM,0,0,0,0);
	}
	else {
		appJpgDraw_PipLayer(RES_JPG_LOGO_SYS,0,0,0,0);
	}
	sp5kDispDevAttrSet(SP5K_DISP_PIP_ACTIVE, 1, 0, 0, 0);
	sp5kGfxAttrSet(SP5K_GFX_REFRESH_ACTIVE,1, 0, 1, 0);
	if (!IS_TV_IN)
	{
		LCD_BACKLIGHT_ON;
		LCD_WAKEUP;
	}

	profLogAdd(0, "show logo-end");
}

/**************************************************************************
 *  Function Name: appStartUpSoundPlay                                        *
 **************************************************************************/
void
appPowerOnSoundPlay(
	void
)
{
#if SOUND_PLAY_NEW_PROC
	UINT32 soundID[]= {
		SOUND_ID_NULL,
		SOUND_ID_START_1,
		SOUND_ID_START_2,
		SOUND_ID_START_3,
	};
	uiPara_t * puiPara = appUiParaGet();
	UINT32 para;

	appAudioDevOn();
	if(puiPara->StartupSound == POWERON_SOUND_NONE){
		return;
	}
	appAddSoundToPlay(soundID[puiPara->StartupSound]);
	appHostMsgWait(APP_UI_MSG_AUDIO_PLAY_FINISH,&para,1000);
#else
	UINT8 *wavFilename[POWERON_SOUND_MAX] =
	{
		NULL,
		RES_WAV_POWER_ON1,
		RES_WAV_POWER_ON2,
		RES_WAV_POWER_ON3
	};
	uiPara_t * puiPara = appUiParaGet();

	appAudioDevOn();
	if(puiPara->StartupSound== POWERON_SOUND_NONE)
	{
		return;
	}
	sp5kResourceFilePlay(SP5K_RES_TYPE_WAV, appRootGet(wavFilename[puiPara->StartupSound]), NULL);
#endif
}
/**************************************************************************
 *  Function Name: appStartUpSoundPlay                                        *
 **************************************************************************/
void
appPowerOnPreviewFreeze(
	BOOL flag
)
{
#if 0
	if(appPowerOnInfo.isPvFreeze != flag) {
		profLogPrintf(0, "pv freeze[%d]-s",flag);
		sp5kDispDevAttrSet(SP5K_DISP_PREVIEW_FREEZE, flag, 0, 0, 0);  /* �I�� */
		appPowerOnInfo.isPvFreeze = flag;
		profLogPrintf(0, "pv freeze[%d]-e",flag);
	}
#endif
}
/**************************************************************************
 *  Function Name: appPowerOnViewSet                                        *
 **************************************************************************/
void
appPowerOnViewSet(
	UINT32 startupState
)
{
	sp5kImgSize_t capSize;
	uiPara_t* puiPara = appUiParaGet();

	printf("@@@appPowerOnViewSet A..\n");
	appModeWaitCfg(TRUE);
	printf("@@@appPowerOnViewSet C..:%d\n",startupState);
	//please note : capture size / video size MUST be set befor mode set
	if(startupState == APP_STATE_STILL_PREVIEW)
	{
		appPowerOnPreviewFreeze(TRUE);
		profLogAdd(0, "be view set");
		appStill_GetCurSize(&capSize.width,&capSize.height);
		appStill_SetResolution(puiPara->ImageSize); /*junqiong.hu@20121121*/
		sp5kStillCapCfgSet(SP5K_CAPTURE_MAIN_IMAGESIZE, &capSize);
		appModeSet(SP5K_MODE_STILL_PREVIEW);
		profLogAdd(0, "af view set");
	}
	else if(startupState == APP_STATE_VIDEO_PREVIEW)
	{
	//	appStill_SetPasmSceneParam(VIDEO_MODE, 0);
		appVidSizeSet(puiPara->VideoSize);
		//sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_VIDEO_FRAME_RATE, 30);
		appPowerOnPreviewFreeze(TRUE);
		profLogAdd(0, "be video view set");
		appModeSet(SP5K_MODE_VIDEO_PREVIEW);
		profLogAdd(0, "af video view set");
	}
	else/*juanjuan add for mantis bug 0046693*/
	{
		appPowerOnPreviewFreeze(TRUE);
		profLogAdd(0, "be audio view set");
		appStill_GetCurSize(&capSize.width,&capSize.height);
		appStill_SetResolution(puiPara->ImageSize);
		sp5kStillCapCfgSet(SP5K_CAPTURE_MAIN_IMAGESIZE, &capSize);
		appModeSet(SP5K_MODE_STILL_PREVIEW);
		profLogAdd(0, "af audio view set");

	}

}
/**************************************************************************
 *  Function Name: appPowerOnStateInit                                        *
 **************************************************************************/
void
appPowerOnStateInit(
	void
)
{
	UINT32 startupState;
	uiPara_t* puiPara = appUiParaGet();

	sp5kUsbCfgSet(SP5K_USB_UVC_RESOLUTION_CHANGE_CB, usbPrevResolutionChgCb);

#if (!FAST_BOOT)
	appUiParamInit();
	appBatteryCheckInit();
#endif

	if (appLowBatteryCheck(APP_BATT_CHK_SYS_BOOT))
	{
		return;
	}

	appBatteryCharge_Init();
	appBackgroundTimerInit();

	LED_FLICKER_POWER_ON;	 /* to light on the LED_POWER of camera */
#if (!FAST_BOOT)
	_powerOnKeyGet();
	gpioInitialCfgSet();
#endif

#if (!FAST_BOOT)
	appExifInit();
#endif

	#if KIT_WITH_LENS
	if(!appPowerOnInfo.isLensFastBoot)
	{
		if(!appPowerOnInfo.isPbPowerOn && !appPowerOnInfo.isAudioPowerOn && !IS_USB_IN)
		{
			if (appLowBatteryCheck(APP_BATT_CHK_LENS_BOOT))
			{
				return;
			}
			appLensInit();
			appLensBoot();
		}
	}
	#endif

	appPanelInit();

	/* RTC cjeck */
	if(appRtcLostCheck())
	{
		appRtcDefaultSet();
		sp5kRtcReliableSet(APP_RTC_RELIABLE_CODE);
	}
	#if COMPEL_POWEROFF_CHARGE
	if(!appGetBoot4ChargingFlag())
	#endif
	{
		appFrontInit();
		appAudioInit();
		#if SOUND_PLAY_NEW_PROC
		appSoundInit();
		#endif

		/* show logo */
		appPowerOnLogoShow();
		/* play opening sound*/
		appPowerOnSoundPlay();
		profLogAdd(0, "sound init :s");
		if (fsFifOpen(FIF_AUDIO_DATA)==0)
		{
			profLogAdd(0, "audio load fail..");
			printf("[ERR] load %s fail..\n",FIF_AUDIO_DATA);
		}

		appPreloadSoundInit();
		sp5kStillCapCfgSet(SP5K_CAPTURE_SHUTTER_SND_NAME, (puiPara->ShutterSound)? RES_WAV_SHUTTER : NULL);

		#if !(POWERON_PB_WITHOUT_SENSOR)
		appAeAwbInit();
		#endif

		#if SP5K_ALGORITHM_AE
		appAEALGInit();
		#endif

		#if SP5K_MOTION_DETECT
		appMotionDetectSet(APP_MD_REG, TRUE);
		#endif

		if(puiPara->ScdvMainMode == UI_SCDV_MAIN_MODE_SPORT)
		{
			appGsensorInit(GSENSOR_NORMAL_INIT);
		}
		//appGsensorBasePointGet();
		appScdvRfInit();	//RF module Setting
		extern void appCdvStillPvStateInit(UINT32 preState);
		if(puiPara->ScdvMainMode == UI_SCDV_MAIN_MODE_CAR)
			appCdvStillPvStateInit(NULL);
		//extern void RF24L01DriverLoad(void);
		//RF24L01DriverLoad();
		#if 0
		#if (PURE_TESTMODE_POWER_ON)
		#else
		startupState = _powerOnStateGet();
		profLogAdd(0, "_powerOnStateGet");
		appPowerOnViewSet(startupState);
		profLogAdd(0, "appPowerOnViewSet");
			#endif
		#endif
	}
	appDiskDetect();
	sp5kUsbCfgSet(SP5K_USB_UVC_RESOLUTION_CHANGE_CB, usbPrevResolutionChgCb);
	usbVndrIcatPreCaptureCallbackSet(Icatsnap3Acb);
	/*sp5kUsbCfgSet(SP5K_USB_CFG_UAC_ENABLE, 1);*/
	dispOpSecondPowerOn();
	sp5kHostMsgSend(APP_UI_MSG_POWER_ON_FLOW_GO_ON, POWER_ON_PHASE_OSD_PRELOAD);

}
/**************************************************************************
 *  Function Name: appPowerOnFlowGoOn                                        *
 **************************************************************************/
void
appPowerOnFlowGoOn(
	UINT32 phase
)
{

	if(phase == POWER_ON_PHASE_OSD_PRELOAD)
	{
#if (FAST_BOOT)
		appOsdResPreload();
		//sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 100);
		appLcmOsdInit();
		//sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 100);
		appLcmOsdDrawPowerOnLogo();
#else
		profLogAdd(0, "osd init");
		appOsdInit();
		appOsdResPreload();
		appLcmOsdInit();
		appLcmOsdDrawPowerOnLogo();
#endif

		sp5kHostMsgSend(APP_UI_MSG_POWER_ON_FLOW_GO_ON, POWER_ON_PHASE_BTN_INIT);
	}
	else if(phase == POWER_ON_PHASE_BTN_INIT)
	{
		#if Calibration_ADC_KEY
		if (appBtnLoad()!=SUCCESS)
		{
			sioCtrl(0);
			appBtnCalib();
			appBtnSave();
		}
		#endif
		appBtnInit();
		appBtnDisable(BTN_ALL);		/* Button disable at Power on state */
		appBuzzerPlay(BUZZER_POWER_ON);
		/* Calibration mode power on (W + MENU + PWR)*/
		#if (PURE_TESTMODE_POWER_ON)
		appPowerOnInfo.isTestModePowerOn =TRUE;
		sp5kHostMsgSend(APP_UI_MSG_POWER_ON_ENTER_CALI_MODE, 0);
		#else
		if(appPowerOnInfo.isTestModePowerOn)
		{
			sp5kHostMsgSend(APP_UI_MSG_POWER_ON_ENTER_CALI_MODE, 0);
		}
		else
		{
			if(appProgAllCheck())
			{
				sp5kHostMsgSend(APP_UI_MSG_POWER_ON_ENTER_SET_LANGUAGE, 0);

			}
			else
			{
				sp5kHostMsgSend(APP_UI_MSG_POWER_ON_ENTER_UI_MODE, 0);
			}
		}
		#endif
	}
	else
	{
		profLogAdd(0, "power on go on error!!");
		printf("[ERR] power on go on error!!!\n");
	}
}
/**************************************************************************
 *  Function Name: appPowerOnStateClose                                        *
 **************************************************************************/
void
appPowerOnStateClose(
	void
)
{
	DBG_PRINT("%s ..\n",__FUNCTION__);
	/*appTimeDelay(50, 1);*/
	sp5kDispDevAttrSet(SP5K_DISP_IMG_ACTIVE, 1, 0, 0, 0);
}
extern UINT8 nDisableHdmiDetect;
void appPowerOnEnterUIMode(void)
{
	uiPara_t* puiPara = appUiParaGet();
	profLogAdd(0, "msg enter UI mode");

	appOsDelayMs(100);

	LED_NORMAL_ON; /* to light on the LED_POWER of camera */
	appStateChange(_powerOnStateGet(),STATE_PARAM_POWER_ON);
	#if (KIT_WITH_HDMI)
	if (!IS_TV_IN && !IS_HDMI_IN)
	{
	#else
	if (!IS_TV_IN )
	{
	#endif
		LCD_BACKLIGHT_ON;
		LCD_WAKEUP;
		LCD_WAKEUP;
	}
	#if FACE_WITH_AE
	#endif
	appProgAllClear();
	nDisableHdmiDetect=0;
	//printf("--cec hdmi diable=%x\n",nDisableHdmiDetect);
	#if (KIT_WITH_HDMI)
	if(IS_HDMI_IN)
	{

			#if SPCA6330
			//printf("--cec hdmi plug1\n");
			appStateChange(APP_STATE_CEC_CAPABILITY_CTRL,STATE_PARAM_HDMI_PLUG);
			#else
			//printf("--cec hdmi plug2\n");
			appStateChange(APP_STATE_HDMI_PLUG,STATE_PARAM_HDMI_PLUG);
			#endif

	}
	else
	#endif
	{
		//printf("--power state=%x\n",_powerOnStateGet());
		//appStateChange(_powerOnStateGet(),STATE_PARAM_POWER_ON);
	}
	if(puiPara->ScdvMainMode == UI_SCDV_MAIN_MODE_CAR)
		appGsensorInit(GSENSOR_NORMAL_INIT);
}
/**************************************************************************
 *  Function Name: appPowerOnState                                        *
 **************************************************************************/
void
appPowerOnState(
	UINT32 msg,
	UINT32 param
)
{
	DBG_PRINT("%s : [0x%x] [0x%x]\n",__FUNCTION__,msg,param);

	switch (msg) {
	case APP_STATE_MSG_INIT:
		RPrintf("appPowerOnState");
		profLogAdd(0, "enter power on state");
		#if PURE_FW_POWER_ON
			//pure FW power on
			break;
		#endif
		appPowerOnStateInit();
		break;
	case APP_UI_MSG_POWER_ON_FLOW_GO_ON:
		profLogPrintf(0, "power on flow go on : %d",param);
		appPowerOnFlowGoOn(param);
		//mask by xuan.ruan for power on optimize
		/*APP_SLEEP_MS(60);*/
		break;
	case APP_UI_MSG_POWER_ON_ENTER_SET_LANGUAGE:
  		break;
	case APP_UI_MSG_POWER_ON_ENTER_UI_MODE:
			appPowerOnEnterUIMode();
			break;
	case APP_UI_MSG_POWER_ON_ENTER_CALI_MODE:
		appPowerOnPreviewFreeze(FALSE);
		LED_NORMAL_ON; /* to light on the LED_POWER of camera */
		if (!IS_TV_IN)
		{
			LCD_BACKLIGHT_ON;
			LCD_WAKEUP;
			LCD_WAKEUP;
		}
		appStateChange(APP_STATE_CALIBRATION,STATE_PARAM_POWER_ON);
		break;
	case SP5K_MSG_AE_READY:
		appAePreviewAdjust(param);
		#if KIT_WITH_LENS
		if(!gStillCB.isLensReady)
		{
			if(!appLensQuery(LENS_QUERY_MOTION_STATE))
			{
				profLogAdd(0, "still pv lens ready a");
				gStillCB.isLensReady = TRUE;
				gStillCB.lensZoomSeg =  appLensQuery(LENS_QUERY_ZOOM_SEG_POS);
			}
		}
		#endif
		break;
	case APP_STATE_MSG_CLOSE:
		appPowerOnStateClose();
		profLogAdd(0, "leave power on state");
		appStateCloseDone();
		break;
	default:
		break;

	}
}
/**************************************************************************
 *  Function Name: appPwerOnInfoGet                                        *
 **************************************************************************/
void
appPwerOnInfoGet(
	BOOL *pbPowerOn,
	BOOL *mutePowerOn,
	BOOL *testModePowerOn
)
{
	*pbPowerOn = appPowerOnInfo.isPbPowerOn;
	*mutePowerOn = appPowerOnInfo.isMutePowerOn;
	*testModePowerOn = appPowerOnInfo.isTestModePowerOn;
}



