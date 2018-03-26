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
#include "sp5k_sensor_api.h"

#include "app_timer.h"
#include "app_simple_menu.h"

#include "app_lens_api.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

#define CALIB_ZMLENS_TMP_BUF_LENGTH		128

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

typedef enum {
	CALIB_ZOOM_SEL_BACK,
	CALIB_ZOOM_SEL_BOOT_SLIP,
	CALIB_ZOOM_SEL_TW_SLIP,
	CALIB_ZOOM_SEL_BLASH_SLIP,
	CALIB_ZOOM_SEL_TOTAL,
} calibZoomLensSelect_t;

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

static UINT32 *ptmpBuf;

#if 0
static UINT8 szCalibStrb_Option1[]="H_POS Scan";
static UINT8 szCalibStrb_Option2[]="Max Width Scan";
static UINT8 szCalibStrb_Option3[]="---";
static UINT8 szCalibStrb_Option4[]="Back";
#endif
static UINT8 menuZMLENS_TitleText[]="ZMLENS Calib";
static UINT32 menuZMLENS_SelList[] =
		{(UINT32)"Back",
		 (UINT32)"BOOT Slip",
		 (UINT32)"Tele Slip",
		 (UINT32)"Blash Slip",
		};
static UINT32 menuSelect;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
static void appCalibZoomLensStateExit(UINT32 nxtSt, UINT32 param);

/*-------------------------------------------------------------------------
 * MENU_ZMLENS Operations
 *------------------------------------------------------------------------*/
#define MENU_ZMLENS_SelectGet()		SMENU_SelectGet()
#define MENU_ZMLENS_Scroll(isUp)	SMENU_Scroll(isUp)
#define MENU_ZMLENS_Show()			SMENU_Show()
#define MENU_ZMLENS_Config()		SMENU_Config(menuZMLENS_TitleText, 4, menuZMLENS_SelList, 0, 0)

/*-------------------------------------------------------------------------
 * Function : calibZoomBootSlip
 *------------------------------------------------------------------------*/
void calibZoomBootSlip(void)
{
	int i;
	UINT32 slip, curZPos;

	for (i=0; i<10; i++)
	{
		if (i>=CALIB_ZMLENS_TMP_BUF_LENGTH) {
			break;
		}
		printf("*********************\n");
		printf(" RUN %d \n", i);
		printf("*********************\n");
		slip = appLensZoomBootSlipCalib();
		curZPos = appLensQuery(LENS_QUERY_ZOOM_STEP_POS);
		*(ptmpBuf+i) = (slip<<16)+(0x0000FFFF & curZPos);
	}
}

/*-------------------------------------------------------------------------
 * Function : calibZoomTWSlip
 *------------------------------------------------------------------------*/
void calibZoomTWSlip(void)
{
	int i, j, doMax=5;
	UINT32 slip, avg;
	UINT32 startSeg, stopSeg;

	/*-- Tele Test */
	j = 0;
	for (startSeg=0; startSeg<(ZOOM_ZS_NUM-1); startSeg++)
	{
		for (stopSeg=startSeg+1; stopSeg<ZOOM_ZS_NUM; stopSeg++)
		{
		avg = 0;
		for (i=0; i<doMax; i++)
		{
			if (i>=CALIB_ZMLENS_TMP_BUF_LENGTH) {
				break;
			}
			printf("*********************\n");
			printf(" RUN %d \n", i);
			printf("*********************\n");
			slip = appLensZoomTeleWideSlipCalib(startSeg, stopSeg);
			avg += slip;
				/*
			deltaPos = appLensQuery(LENS_QUERY_ZOOM_SEG2POS+(stopSeg<<16)) -
						appLensQuery(LENS_QUERY_ZOOM_SEG2POS+(startSeg<<16));
							*/
		}
			avg /= i;
			*(ptmpBuf+j) = (avg<<16)+(0x0000FF00 & (startSeg<<8))+(0x000000FF & stopSeg);
			j++;
			appBeep(TRUE, FALSE);
		}
		appBeep(TRUE, FALSE);
	}
}

/*-------------------------------------------------------------------------
 * Function : calibZoomBlashSlip
 *------------------------------------------------------------------------*/
void calibZoomBlashSlip(void)
{
	int i, j, doMax=10;
	UINT32 slip, avg;
	int startSeg, stopSeg;

	/*-- Blash Test */
	j = 0;
	for (startSeg=(ZOOM_ZS_NUM-1); startSeg>0; startSeg--)
	{
		for (stopSeg=startSeg-1; stopSeg>=0; stopSeg--)
		{
			avg = 0;
			for (i=0; i<doMax; i++)
			{
				if (i>=CALIB_ZMLENS_TMP_BUF_LENGTH) {
					break;
				}
				printf("*********************\n");
				printf(" RUN %d : startSeg(%d), stopSeg(%d) \n", i, startSeg, stopSeg);
				printf("*********************\n");
				slip = appLensZoomBlashSlipCalib(startSeg, stopSeg);
				avg += slip;
				/*
				deltaPos = appLensQuery(LENS_QUERY_ZOOM_SEG2POS+(startSeg<<16)) -
							appLensQuery(LENS_QUERY_ZOOM_SEG2POS+(stopSeg<<16));
						*/
			}
			avg /= i;
			*(ptmpBuf+j) = (avg<<16)+(0x0000FF00 & (startSeg<<8))+(0x000000FF & stopSeg);
			j++;
			appBeep(TRUE, FALSE);
		}
		appBeep(TRUE, FALSE);LENS_SLEEP(200);appBeep(TRUE, FALSE);LENS_SLEEP(200);appBeep(TRUE, FALSE);
	}
}


/*-------------------------------------------------------------------------
 * Function : Key_MENU_Service
 *------------------------------------------------------------------------*/
static void Key_MENU_Service(UINT32 param)
{
	int i;
	UINT32 slip, curZPos;
	UINT32 startSeg, stopSeg;

	menuSelect = MENU_ZMLENS_SelectGet();
	switch (menuSelect) {
	case CALIB_ZOOM_SEL_BACK:
		break;
	case CALIB_ZOOM_SEL_BOOT_SLIP:
		printf("\n== BOOT Slip ==============\n");
		for (i=0; i<CALIB_ZMLENS_TMP_BUF_LENGTH; i++) {
			slip = *(ptmpBuf+i) >> 16;
			curZPos = 0x0000FFFF & *(ptmpBuf+i);
			printf("[%02d] %d @ %d \n", i, slip, curZPos);
		}
		break;
	case CALIB_ZOOM_SEL_TW_SLIP:
	case CALIB_ZOOM_SEL_BLASH_SLIP:
		printf("\n== TW/BL Slip ==============\n");
		printf("[  ] slip  S  E\n");
		for (i=0; i<32; i++) {
			slip = *(ptmpBuf+i) >> 16;
			startSeg = (*(ptmpBuf+i)>>8) & 0x000000FF;
			stopSeg = *(ptmpBuf+i) & 0x000000FF;
			printf("[%02d] %4d  %1d  %1d\n", i, slip, startSeg, stopSeg);
		}
		break;
	}
}

/*-------------------------------------------------------------------------
 * Function : Key_MENU_Service
 *------------------------------------------------------------------------*/
static void Key_SET_Service(UINT32 param)
{
	menuSelect = MENU_ZMLENS_SelectGet();
	switch (menuSelect) {
	case CALIB_ZOOM_SEL_BACK:
		CLEAR_OSD_SRC;
		appCalibZoomLensStateExit(APP_STATE_CALIBRATION, STATE_PARAM_NORMAL_INIT);
		break;
	case CALIB_ZOOM_SEL_BOOT_SLIP:
		CLEAR_OSD_SRC;
		calibZoomBootSlip();
		MENU_ZMLENS_Show();
		break;
	case CALIB_ZOOM_SEL_TW_SLIP:
		CLEAR_OSD_SRC;
		calibZoomTWSlip();
		MENU_ZMLENS_Show();
		break;
	case CALIB_ZOOM_SEL_BLASH_SLIP:
		CLEAR_OSD_SRC;
		calibZoomBlashSlip();
		MENU_ZMLENS_Show();
		break;
	}
}

/*-------------------------------------------------------------------------
 * Function : appCalibZoomLensStateInit
 *------------------------------------------------------------------------*/
static void appCalibZoomLensStateInit(void)
{
	CLEAR_OSD_SRC;

	/*-- Enter View mode */
	sp5kAeModeSet(SP5K_AE_MODE_INFO_ONLY);
#if SP5K_ALGORITHM_AWB
	sp5kAwbModeSet(SP5K_AWB_MODE_INFO_ONLY);
#else
	sp5kAwbModeSet(SP5K_AWB_MODE_AUTO_SET);
#endif

	sp5kModeSet(SP5K_MODE_STILL_PREVIEW);
	appHostMsgWaitExact(SP5K_MSG_MODE_READY,
						SP5K_MODE_STILL_PREVIEW,
						3000);
	printf("\n>>> View Mode ready !! \n\n");

	/*-- Init Global Data */
	ptmpBuf = (UINT32 *)sp5kMalloc(CALIB_ZMLENS_TMP_BUF_LENGTH*sizeof(UINT32));

	/*-- Create SUB-Menu */
	MENU_ZMLENS_Config();
	MENU_ZMLENS_Show();
}

/*-------------------------------------------------------------------------
 * Function : appCalibZoomLensStateClose
 *------------------------------------------------------------------------*/
static void appCalibZoomLensStateClose(void)
{
	if(ptmpBuf != NULL) {
		sp5kFree(ptmpBuf);
	}
}

/*-------------------------------------------------------------------------
 * Function : appCalibZoomLensStateExit
 *------------------------------------------------------------------------*/
static void appCalibZoomLensStateExit(UINT32 nxtSt, UINT32 param)
{
	appCalibZoomLensStateClose();
	appStateChange(nxtSt, param);
}

/*-------------------------------------------------------------------------
 * Function : appCalibZoomLensStateOnMsg
 *------------------------------------------------------------------------*/
static void appCalibZoomLensStateOnMsg(UINT32 msg, UINT32 param)
{
	switch (msg) {
	default:
		break;
	}
}

/*-------------------------------------------------------------------------
 * Function : appCalibZoomLensStateOnKey
 *------------------------------------------------------------------------*/
static void appCalibZoomLensStateOnKey(UINT32 msg, UINT32 param)
{
	switch (msg) {
	case APP_KEY_PRESS_S1:
		break;

	case APP_KEY_PRESS_UP:
	case APP_KEY_PRESS_DOWN:
		appBeep(TRUE, FALSE);
		MENU_ZMLENS_Scroll(((msg==APP_KEY_PRESS_UP) ? TRUE : FALSE));
		break;

	case APP_KEY_PRESS_SET:
		appBeep(TRUE, FALSE);
		Key_SET_Service(param);
		break;

	case APP_KEY_PRESS_MENU:
		appBeep(TRUE, FALSE);
		Key_MENU_Service(param);
		break;

	case APP_KEY_PRESS_DEL:
		//appBeep(TRUE, FALSE);
		break;
	case APP_KEY_PRESS_LEFT:
	case APP_KEY_PRESS_RIGHT:
	default:
		break;
	}
}

/*-------------------------------------------------------------------------
 * Function : appCalibZoomLensState
 *------------------------------------------------------------------------*/
void appCalibZoomLensState(UINT32 msg, UINT32 param)
{
	switch (msg) {
	case APP_STATE_MSG_INIT:
		RPrintf("appCalibZoomLensState");
		appCalibZoomLensStateInit();
		break;
	case APP_STATE_MSG_CLOSE:
		appCalibZoomLensStateClose();
		appStateCloseDone();
		break;
	default:
		if (IS_APP_KEY_MSG(msg)) {
			appCalibZoomLensStateOnKey(msg,param);
		}
		else{
			appCalibZoomLensStateOnMsg(msg,param);
		}
		break;
	}

}

