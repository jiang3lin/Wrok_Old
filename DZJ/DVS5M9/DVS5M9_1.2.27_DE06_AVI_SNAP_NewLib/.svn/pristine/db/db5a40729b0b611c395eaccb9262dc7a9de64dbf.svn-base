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
#include "stdio.h"
#include "stdarg.h"
#include "string.h"
#include "app_com_def.h"
#include "app_com_api.h"

#include "sp5k_global_api.h"
#include "app_osd_api.h"
#include "app_osd.h"
#include "app_battery.h"


/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
	

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

UINT32 batCurveTimer;
UINT32 BatLogCnt =0;


/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
 
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
	
 
/**************************************************************************
 *								M A C R O S 							  *
 **************************************************************************/
	
#define BATTERY_CURVE_TEST_X		60
#define BATTERY_CURVE_TEST_Y		8

#define BATTERY_CURVE_AD_X		60
#define BATTERY_CURVE_AD_Y		100
#define BATTERY_CURVE_AD_VALUE_X	220


#define BATTERY_CURVE_BACK_X		100
#define BATTERY_CURVE_BACK_Y		200

#define TBBATCURVE_MENU_SHOW()		appTbBat_CurveMenu_Show()



/*-------------------------------------------------------------------------
 *  File Name : appTbBat_CurveMenu_Show
 *------------------------------------------------------------------------*/
void
appTbBat_CurveMenu_Show(
	void
)
{
	appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_TextDraw(BATTERY_CURVE_TEST_X,BATTERY_CURVE_TEST_Y,SP5K_GFX_ALIGN_TOP_LEFT,"BATTERY CURVE TEST");
	appOsdLib_TextDraw(BATTERY_CURVE_AD_X,BATTERY_CURVE_AD_Y,SP5K_GFX_ALIGN_TOP_LEFT,"BATTERY AD: ");
	appOsdLib_TextDraw(BATTERY_CURVE_BACK_X,BATTERY_CURVE_BACK_Y,SP5K_GFX_ALIGN_TOP_LEFT,"MENU: BACK");

}


/*-------------------------------------------------------------------------
 *  File Name : appTbBat_CurveAdc_Show
 *------------------------------------------------------------------------*/
void
appTbBat_CurveAdc_Show(
	UINT32 adc
)
{
	char buf[4];
	sprintf(buf,"%04d",adc);

	appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, BATTERY_CURVE_AD_VALUE_X, BATTERY_CURVE_AD_Y,100 ,32, 0, 0);

	appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_TextDraw(BATTERY_CURVE_AD_VALUE_X,BATTERY_CURVE_AD_Y,SP5K_GFX_ALIGN_TOP_LEFT,buf);

}

/*-------------------------------------------------------------------------
 * Function : appTbBat_CurveStateAdGet
 *------------------------------------------------------------------------*/
UINT32 appTbBat_CurveStateAdcGet(
	void
)
{
 	UINT8 i=0;
	UINT32 BatGet =0;
	UINT32 BatValue=0;

	UINT8 cnt =10;

	for (i=0 ; i<cnt ; i++){
		BatGet=appBatteryAdcValueGet();
		BatValue=BatValue+BatGet;
		APP_SLEEP_MS(10);
	}

	BatValue =BatValue/cnt;

	return BatValue;
}


/*-------------------------------------------------------------------------
 * Function : appTbBat_CurveStateInit
 *------------------------------------------------------------------------*/
static void appTbBat_CurveStateInit(
	void
)
{
	UINT32 BatAD =0;

	CLEAR_OSD_SRC;
	TBBATCURVE_MENU_SHOW();
	BatAD=appTbBat_CurveStateAdcGet();
	appTbBat_CurveAdc_Show(BatAD);
	sp5kFsFileDelete("D:\\BAT_CURV.TXT");
	APP_SLEEP_MS(1000);
	batCurveTimer = appTimerSet(30000, "Bat_Curve");

}

/*-------------------------------------------------------------------------
 * Function : appTbBat_CurveStateClose
 *------------------------------------------------------------------------*/
static void appTbBat_CurveStateClose(
	void
)
{
	BatLogCnt =0;
	appTimerClear(&batCurveTimer);
	sp5kFsFileDelete("D:\\BAT_CURV.TXT");
	APP_SLEEP_MS(1000);
}


/*-------------------------------------------------------------------------
 * Function : apptbBat_CurveLog
 *------------------------------------------------------------------------*/
static void apptbBat_CurveLog(
	UINT32 adc
)
{
	UINT32 fd;
	UINT8 szTmp[12];
	
	/*-- Open log file */
	fd = sp5kFsFileOpen("D:\\BAT_CURV.TXT", SP5K_FS_OPEN_APPEND|SP5K_FS_OPEN_RDWR);
	if (fd==0) {
		fd = sp5kFsFileOpen("D:\\BAT_CURV.TXT", SP5K_FS_OPEN_CREATE|SP5K_FS_OPEN_RDWR);
		if (fd==0) {
			printf("\nFILE Open failed !!\n\n");
			BatLogCnt =0;
			sp5kHostMsgSend(APP_KEY_PRESS_MENU, 0);
			return;
		}
	}else{
		sp5kFsFileSeek(fd, 0, SP5K_FS_SEEK_END);
	}

	BatLogCnt ++;
	sprintf(szTmp, "%05d %04d\n", BatLogCnt,adc);
	printf("\nCurveLog:%s\n",szTmp);

	sp5kFsFileWrite(fd, szTmp, strlen(szTmp));
	
	sp5kFsFileClose(fd);

}


/*-------------------------------------------------------------------------
 * Function : appTbBat_CurveStateOnMsg
 *------------------------------------------------------------------------*/
static void appTbBat_CurveStateOnMsg(
	UINT32 msg, UINT32 param
)
{
	UINT32 BatAD =0;

	switch ( msg) {
	case SP5K_MSG_TIMER:
		if (param == batCurveTimer){
			BatAD=appTbBat_CurveStateAdcGet();
			appTbBat_CurveAdc_Show(BatAD);
			apptbBat_CurveLog(BatAD);
		}
		break;
	default:
		break;
	}

}

/*-------------------------------------------------------------------------
 * Function : appTbBat_CurveStateOnKey
 *------------------------------------------------------------------------*/
static void appTbBat_CurveStateOnKey(
	UINT32 msg, UINT32 param
)
{
	switch (msg) {
	/* zooming */	
	case APP_KEY_PRESS_TELE:
		break;
	case APP_KEY_PRESS_WIDE:
		break;
	case APP_KEY_PRESS_S1:
		break;

	case APP_KEY_PRESS_UP:
	case APP_KEY_PRESS_DOWN:
		break;
	case APP_KEY_PRESS_SET:
		break;
	case APP_KEY_PRESS_MENU:
		appTbBat_CurveStateClose();
		appStateChange(APP_STATE_TEST_MAIN, STATE_PARAM_NORMAL_INIT);
		break;
	case APP_KEY_PRESS_DEL:
		break;
	case APP_KEY_PRESS_LEFT:
	case APP_KEY_PRESS_RIGHT:
		break;
	default:
		break;
	}
}
/*-------------------------------------------------------------------------
 * Function : appTbBat_CurveState
 *------------------------------------------------------------------------*/
void appTbBat_CurveState(
	UINT32 msg, UINT32 param
)
{
	switch (msg) {
	case APP_STATE_MSG_INIT:
		RPrintf("appTbBat_CurveState");
		appTbBat_CurveStateInit();
		break;
	case APP_STATE_MSG_CLOSE:
		appTbBat_CurveStateClose();
		appStateCloseDone();
		break;
	default:
		if (IS_APP_KEY_MSG(msg)) {
			appTbBat_CurveStateOnKey(msg,param);
		}
		else{
			appTbBat_CurveStateOnMsg(msg,param);
		}
		break;
	}

}




