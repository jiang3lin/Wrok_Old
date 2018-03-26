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
#include "app_still.h"
#include "app_strobe.h"

#include "app_osd.h"
#include "app_osdfont.h"
#include "app_osd_api.h"
#include "app_lens_api.h"
#include "app_simple_menu.h"
#include "app_flash_api.h"
#include "app_menu_tab.h"  
#include "app_menu_core.h"
#include "app_menu_disp.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define CALIB_PREFLASH_MAX_CNT			500
#define PREVIEW_SENSOR_PXL_PER_LINE		3264
#define PREVIEW_SENSOR_LINE_PER_FRAME	240
#define SENSOR_CLK_PERIOD				28 /* ns */
#define PREFLASH_INIT_PULSE_WIDTH		(PREVIEW_SENSOR_PXL_PER_LINE/2)
#define PREFLASH_INIT_H_POS				(240-10)
#define PREFLASH_DELTA_H_POS			5

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define beep_echo()		do { appBeep(TRUE, FALSE); APP_SLEEP_MS(80); } while(0)

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
static UINT32 testCountMax = CALIB_PREFLASH_MAX_CNT;
static UINT32 testIdx = 0;
static UINT32* pxBuf=NULL;

static UINT32 prefhWidth=20000;
static UINT32 prefhPos=1;
static sp5kAeResult_t aeResult;
static sp5kAwbResult_t awbResult;

static UINT8 menuSelect;
static BOOL isCancelCalib = FALSE;
UINT8 CalibFlashMenuSelect=0;

typedef enum {
	CALIB_STRB_MENU_SEL_BACK,
	CALIB_STRB_MENU_SEL_PRE_ANA,
	CALIB_STRB_MENU_SEL_H_POS,
	CALIB_STRB_MENU_SEL_PULSE_WIDTH,
	CALIB_STRB_MENU_SEL_TOTAL,
} calibStrbMenuSelect_t;

UINT8 szCalibStrb_Title[]="PreFlash Calib";
UINT32 calibStrb_OptList[] =
		{(UINT32)"Back",
		 (UINT32)"Pre-Analyses",
		 (UINT32)"H_POS Scan",
		 (UINT32)"Duration Scan",
		};

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern UINT32 menuPosBackup;  
extern UINT16 tbflash_GetAeLuma(sp5kAeResult_t *paeResult);
extern UINT32 TBFLASH_OkExitKeyScan(void);
extern UINT16 tbflash_GetPreflashResult(sp5kAeResult_t *paeResult);

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
void appCalibFlashStateExit(UINT32 nxtSt, UINT32 param);
/*-------------------------------------------------------------------------
 *  File Name : calib_SimpleMenuSelectGet
 *------------------------------------------------------------------------*/
UINT8
calib_SimpleMenuSelectGet(
	void
)
{
	return SMENU_SelectGet();
}

/*-------------------------------------------------------------------------
 *  File Name : calib_SimpleMenuScroll
 *------------------------------------------------------------------------*/
void
calib_SimpleMenuScroll(
	BOOL isUp
)
{
	SMENU_Scroll(isUp);
}

/*-------------------------------------------------------------------------
 *  File Name : calib_SimpleMenuShow
 *------------------------------------------------------------------------*/
void
calib_SimpleMenuShow(
	void
)
{
	CalibFlashMenuSelect = 0;
	SMENU_Show();
}

/*-------------------------------------------------------------------------
 *  File Name : calib_SimpleMenuConfig
 *------------------------------------------------------------------------*/
void
calib_SimpleMenuConfig(
	UINT8 *pszTitle,
	UINT32 numItems,
	UINT32 *pszItemText,
	UINT32 actIndex,
	SINT32 yOffset
)
{
	SMENU_Config(pszTitle, numItems, pszItemText, actIndex, yOffset);
}

/*-------------------------------------------------------------------------
 *  File Name :
 *------------------------------------------------------------------------*/
static void Do_H_POS_Test(void)
{
	/*-- Check STROBE Ready */
	printf("\n>>> Checking STROBE ...\n\n");
	appStrobeChargeStart();
	do {
		APP_SLEEP_MS(1000);
	} while(appStrobeQueryReady()==FALSE);
	appStrobeChargeStop();
	printf("\n>>> STROBE Ready !! \n\n");

	/*-- Fire Preflash */
	printf("\n>>> Fire Preflash !! \n\n");
	sp5kPreflashTrig();
}

/*-------------------------------------------------------------------------
 *  File Name :
 *------------------------------------------------------------------------*/
static void
calibPreflash_H_POS_Test(void)
{
	UINT16 aeLuma;
	UINT8 szTmp[128];
	sp5kAeResult_t aeResult;
	appPrefhCb_t* pPrefhCb;

	pPrefhCb = appPrefhCbGet();

	testIdx=0;
	prefhPos = PREFLASH_INIT_H_POS;
	isCancelCalib = FALSE;

	printf("\n\n");
	printf("*******************************\n");
	printf("PreFlash H_POS Test \n");
	printf("*******************************\n");
	printf("\n\n");

	/*- Confirm Testbed setup */
	SMENU_YesNoConfig("Testbed Rdy ?");
	SMENU_YesNoShow();
	if (SMENU_SEL_NO==SMENU_SelScan()) {
		return ;
	}
	SMENU_YesNoClear();

	/*-- Set Exp/Agc before Preflash */
	stext_Print(160, 120, "Pls Wait ...");
	sp5kPreviewExpAgcSet(96, 64); // preview 1/60, ISO50
	APP_SLEEP_MS(2000);

	/*-- Get & Check Exp/Agc before Preflash */
	UINT16 originLuma;
	originLuma = tbflash_GetAeLuma(&aeResult);
	CLEAR_OSD_SRC;
	CLEAR_PIP_SRC;

	SINT32 expIdx;
	UINT32 agcIdx;
	sp5kPreviewExpAgcGet(&expIdx, &agcIdx);
	stext_Print(160, 30, "AE Locked ...");
	stext_Print(160, 60, "ExpIdx %d", expIdx);
	stext_Print(160, 90, "AgcIdx %d", agcIdx);
	stext_Print(160, 120, "aeLuma %d", originLuma);
	if (0==TBFLASH_OkExitKeyScan()) {
		/*-- Exit */
		return;
	}
	CLEAR_OSD_SRC;
	CLEAR_PIP_SRC;

	/*-- sp5k Flash Init */
	UINT32 pulseWidth;
	pulseWidth = 100;
	sp5kFlashlightInit(SP5K_FLASHLIGHT_INACTIVE_LO);
	sp5kPreflashCfgSet(SP5K_PREFLASH_CFG_WIDTH, pulseWidth);

	/*-- Clean Data Buf */
	int i;
	for (i=0; i<CALIB_PREFLASH_MAX_CNT ; i++) {
		*(pxBuf+i) = 0;
	}

	/*-- Loop Test */
	UINT32 cur=0, start, end, delta=20;
	start = 10; end = pPrefhCb->hPosLimit;
	for (i=0; ; i++) {
		/*- Confirm go on  */
		if (start+(i*delta)>end) {
			if  (cur != end) cur = end;
			else break;
		} else {
			cur = start+(i*delta);
		}

		CLEAR_OSD_SRC;
		CLEAR_PIP_SRC;
		/*-- Configure Preflash Pulse Width */
		printf("STRB Width(%d), pos(%d) \n", pulseWidth, cur);
		stext_Print(160, 30, "Pulse L(%d) Pos (%d)", pulseWidth, cur);
		sp5kPreflashCfgSet(SP5K_PREFLASH_CFG_H_POS, cur);

		/*-- Charge Strobe */
		printf("\n>>> Checking STROBE ...\n\n");
		stext_Print(160, 70, "Charging Strobe ...", cur);
		appStrobeChargeStart();
		do {
			APP_SLEEP_MS(1000);
		} while(appStrobeQueryReady()==FALSE);
		appStrobeChargeStop();
		printf("\n>>> STROBE Ready !! \n\n");
		stext_Print(160, 105, "Strobe Rdy ...", cur);

		/*-- Fire Preflash */
		printf("\n>>> Fire Preflash !! \n\n");
		sp5kPreflashTrig();
		/*- Get preflash result */
		aeLuma = tbflash_GetPreflashResult(&aeResult);
		stext_Print(160, 140, "Preflash Fired ...", cur);


		/*-- Store and show result */
		beep_echo();
		*(pxBuf+i) = (0x0000FFFF & aeLuma)+(cur << 16);
		stext_Print(160, 180, "AE Luma(%d), %d", aeLuma, cur);
		printf("\nAE Luma = %d @ pos(%d) \n\n", aeLuma, cur);
		APP_SLEEP_MS(1500);
	}

	/*-- Store result to DISK */
	SMENU_YesNoConfig("SAVE Result ?");
	SMENU_YesNoShow();
	if (SMENU_SEL_YES==SMENU_SelScan()) {
		UINT32 fd;
		fd = sp5kFsFileOpen("D:\\ST_H_POS.TXT", SP5K_FS_OPEN_CREATE);
		if (fd==0) {
			printf("\nFILE Open failed !!\n\n");
			return ;
		}
		sprintf(szTmp, "expIdx(%d) agcIdx(%d) ...\n", expIdx, agcIdx);
		sp5kFsFileWrite(fd, szTmp, strlen(szTmp));

		sprintf(szTmp, "\n--- Strobe H_Pos ---\n");
		sp5kFsFileWrite(fd, szTmp, strlen(szTmp));
		sprintf(szTmp, "start: %d \n", start);
		sp5kFsFileWrite(fd, szTmp, strlen(szTmp));
		sprintf(szTmp, "end : %d \n", end);
		sp5kFsFileWrite(fd, szTmp, strlen(szTmp));
		sprintf(szTmp, "delta : %d \n", delta);
		sp5kFsFileWrite(fd, szTmp, strlen(szTmp));
		sprintf(szTmp, "\n\n");
		sp5kFsFileWrite(fd, szTmp, strlen(szTmp));

		sprintf(szTmp, "Strobe PulseWidth : %d \n", pulseWidth);
		sp5kFsFileWrite(fd, szTmp, strlen(szTmp));
		sprintf(szTmp, "Origin aeLuma : %d \n", originLuma);
		sp5kFsFileWrite(fd, szTmp, strlen(szTmp));

		for (i=0; i<CALIB_PREFLASH_MAX_CNT; i++)
		{
			UINT32 idx, luma;
			idx = (*(pxBuf+i)>>16)&0x0000FFFF;
			luma = *(pxBuf+i) & 0x0000FFFF;
			sprintf(szTmp, "[%2d] %4d %d \n", i, idx, luma);
			sp5kFsFileWrite(fd, szTmp, strlen(szTmp));
		}
		sp5kFsFileClose(fd);
	}

}
/*-------------------------------------------------------------------------
 *  File Name :
 *------------------------------------------------------------------------*/
static void
calibPreflash_Duration_Test(void)
{
	UINT16 aeLuma;
	UINT8 szTmp[128];
	sp5kAeResult_t aeResult;
	appPrefhCb_t* pPrefhCb;

	pPrefhCb = appPrefhCbGet();

	testIdx=0;
	prefhPos = PREFLASH_INIT_H_POS;
	isCancelCalib = FALSE;

	printf("\n\n");
	printf("*******************************\n");
	printf("PreFlash Duration Test \n");
	printf("*******************************\n");
	printf("\n\n");

	/*- Confirm Testbed setup */
	SMENU_YesNoConfig("Testbed Rdy ?");
	SMENU_YesNoShow();
	if (SMENU_SEL_NO==SMENU_SelScan()) {
		return ;
	}
	SMENU_YesNoClear();

	/*-- Set Exp/Agc before Preflash */
	stext_Print(160, 120, "Pls Wait ...");
	sp5kPreviewExpAgcSet(112, 48); // preview 1/120, ISO50

	/*-- Small iris */
	appLensCreate();
	appIrisSmallSet();
	APP_SLEEP_MS(50);
	appLensDestroy();

	/*-- Get & Check Exp/Agc before Preflash */
	UINT16 originLuma;
	originLuma = tbflash_GetAeLuma(&aeResult);
	CLEAR_OSD_SRC;
	CLEAR_PIP_SRC;

	SINT32 expIdx;
	UINT32 agcIdx;
	sp5kPreviewExpAgcGet(&expIdx, &agcIdx);
	stext_Print(160, 30, "AE Locked ...");
	stext_Print(160, 60, "ExpIdx %d", expIdx);
	stext_Print(160, 90, "AgcIdx %d", agcIdx);
	stext_Print(160, 120, "aeLuma %d", originLuma);
	if (0==TBFLASH_OkExitKeyScan()) {
		/*-- Exit */
		return;
	}
	CLEAR_OSD_SRC;
	CLEAR_PIP_SRC;
	APP_SLEEP_MS(2000);

	/*-- sp5k Flash Init */
	UINT32 h_pos;
	h_pos = pPrefhCb->hPosLimit-30;
	sp5kFlashlightInit(SP5K_FLASHLIGHT_INACTIVE_LO);
	sp5kPreflashCfgSet(SP5K_PREFLASH_CFG_H_POS, h_pos);

	/*-- Clean Data Buf */
	int i;
	for (i=0; i<CALIB_PREFLASH_MAX_CNT ; i++) {
		*(pxBuf+i) = 0;
	}

	/*-- Loop Test */
	//UINT32 cur=0, start, end, delta=APP_PREFLASH_LINETOTAL/4;
	//start = 1*(APP_PREFLASH_LINETOTAL/4); end = 8*APP_PREFLASH_LINETOTAL;
	UINT32 cur=0, start, end, delta=200;
	start = 300; end =5000;
	for (i=0; ; i++) {
		/*- Confirm go on  */
		if (start+(i*delta)>end) {
			if  (cur != end) cur = end;
			else break;
		} else {
			cur = start+(i*delta);
		}

		CLEAR_OSD_SRC;
		CLEAR_PIP_SRC;
		/*-- Configure Preflash Pulse Width */
		printf("STRB H_POS(%d), pos(%d) \n", h_pos, cur);
		stext_Print(160, 30, "Pulse Pos(%d) L(%d)", h_pos, cur);
		sp5kPreflashCfgSet(SP5K_PREFLASH_CFG_WIDTH, cur);

		/*-- Charge Strobe */
		printf("\n>>> Checking STROBE ...\n\n");
		stext_Print(160, 70, "Charging Strobe ...", cur);
		appStrobeChargeStart();
		do {
			APP_SLEEP_MS(1000);
		} while((appStrobeQueryReady()==FALSE));
		appStrobeChargeStop();
		printf("\n>>> STROBE Ready !! \n\n");
		stext_Print(160, 105, "Strobe Rdy ...", cur);

		/*-- Fire Preflash */
		printf("\n>>> Fire Preflash !! \n\n");
		stext_Print(160, 140, "Preflash Fired", cur);
		sp5kPreflashTrig();
		/*- Get preflash result */
		aeLuma = tbflash_GetPreflashResult(&aeResult);


		/*-- Store and show result */
		//beep_echo();
		*(pxBuf+i) = (0x0000FFFF & aeLuma)+(cur << 16);
		stext_Print(160, 180, "AE Luma(%d), %d", aeLuma, cur);
		printf("\nAE Luma = %d @ pos(%d) \n\n", aeLuma, cur);
		APP_SLEEP_MS(2000);
	}

	/*-- Store result to DISK */
	SMENU_YesNoConfig("SAVE Result ?");
	SMENU_YesNoShow();
	if (SMENU_SEL_YES==SMENU_SelScan()) {
		UINT32 fd;
		fd = sp5kFsFileOpen("D:\\ST_WIDTH.TXT", SP5K_FS_OPEN_CREATE);
		if (fd==0) {
			printf("\nFILE Open failed !!\n\n");
			return ;
		}
		sprintf(szTmp, "expIdx(%d) agcIdx(%d) ...\n", expIdx, agcIdx);
		sp5kFsFileWrite(fd, szTmp, strlen(szTmp));

		sprintf(szTmp, "\n--- Strobe H_Pos ---\n");
		sp5kFsFileWrite(fd, szTmp, strlen(szTmp));
		sprintf(szTmp, "start: %d \n", start);
		sp5kFsFileWrite(fd, szTmp, strlen(szTmp));
		sprintf(szTmp, "end : %d \n", end);
		sp5kFsFileWrite(fd, szTmp, strlen(szTmp));
		sprintf(szTmp, "delta : %d \n", delta);
		sp5kFsFileWrite(fd, szTmp, strlen(szTmp));
		sprintf(szTmp, "\n\n");
		sp5kFsFileWrite(fd, szTmp, strlen(szTmp));

		sprintf(szTmp, "Strobe H_POS : %d \n", h_pos);
		sp5kFsFileWrite(fd, szTmp, strlen(szTmp));
		sprintf(szTmp, "Origin aeLuma : %d \n", originLuma);
		sp5kFsFileWrite(fd, szTmp, strlen(szTmp));

		for (i=0; i<CALIB_PREFLASH_MAX_CNT; i++)
		{
			UINT32 idx, luma;
			idx = (*(pxBuf+i)>>16)&0x0000FFFF;
			luma = *(pxBuf+i) & 0x0000FFFF;
			sprintf(szTmp, "[%2d] %4d %d \n", i, idx, luma);
			sp5kFsFileWrite(fd, szTmp, strlen(szTmp));
		}
		sp5kFsFileClose(fd);
	}

}

#if 0
/*-------------------------------------------------------------------------
 *  File Name :
 *------------------------------------------------------------------------*/
void
appStillStrobStateSet(
	UINT32 flashWidth,
	UINT32 flashHPos,
	UINT32 testTime
)
{
	prefhWidth = flashWidth;
	prefhPos = flashHPos;
	testCountMax	= (testTime==0)? 1 :
		(testTime>CALIB_PREFLASH_MAX_CNT)? CALIB_PREFLASH_MAX_CNT : testTime;
}

void
appStillStrobStateGet(void)
{
	int i;
	if(pxBuf==NULL) {
		printf("There is no avaiable strob data\n");
		return;
	}
	for(i=0; i<testCountMax; i++) {
		printf("%d,\n",pxBuf[i]);
	}
}
#endif

static void Key_MENU_Service(UINT32 param)
{
	UINT32 i;

	switch (menuSelect) {
	case CALIB_STRB_MENU_SEL_H_POS: /*-- H_POS Test */
		printf("\n\n");
		printf("Preflash H_POS Statictics ============...\n");
		printf("init H_POS(%d), delta(%d), pulswW(%d) \n",
				PREFLASH_INIT_H_POS, PREFLASH_DELTA_H_POS,
				PREFLASH_INIT_PULSE_WIDTH);
		for (i=0; i<testIdx; i++) {
			printf("%d: %d \n", i, pxBuf[i]);
		}
		printf("\n\n");
		break;
	case CALIB_STRB_MENU_SEL_PRE_ANA:
	case CALIB_STRB_MENU_SEL_PULSE_WIDTH:
	case CALIB_STRB_MENU_SEL_BACK:
		break;
	}
}

static void Key_SET_Service(UINT32 param)
{
	beep_echo(); CLEAR_OSD_SRC;
	menuSelect = calib_SimpleMenuSelectGet();
	CalibFlashMenuSelect = menuSelect;
	switch (menuSelect) {
	case CALIB_STRB_MENU_SEL_H_POS:
		calibPreflash_H_POS_Test();
		CLEAR_OSD_SRC;
		calib_SimpleMenuConfig(szCalibStrb_Title, 4, calibStrb_OptList, 0, -16);
		calib_SimpleMenuShow();
		break;
	case CALIB_STRB_MENU_SEL_PULSE_WIDTH:
		calibPreflash_Duration_Test();
		CLEAR_OSD_SRC;
		calib_SimpleMenuConfig(szCalibStrb_Title, 4, calibStrb_OptList, 0, -16);
		calib_SimpleMenuShow();
		break;
	case CALIB_STRB_MENU_SEL_PRE_ANA:
		CLEAR_OSD_SRC;
		appCalibFlashStateExit(APP_STATE_TEST_FLASH, STATE_PARAM_NORMAL_INIT);
		break;
	case CALIB_STRB_MENU_SEL_BACK:
		CLEAR_OSD_SRC;
		appCalibFlashStateExit(APP_STATE_CALIBRATION, STATE_PARAM_NORMAL_INIT);
		break;
	}
}

void
appCalibFlashStateInit(void)
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

	/*-- Configure Preflash Settings */
	prefhWidth = PREFLASH_INIT_PULSE_WIDTH;
	prefhPos = PREFLASH_INIT_H_POS;
	sp5kFlashlightInit(SP5K_FLASHLIGHT_INACTIVE_LO);
	sp5kPreflashCfgSet(SP5K_PREFLASH_CFG_WIDTH, prefhWidth);
	sp5kPreflashCfgSet(SP5K_PREFLASH_CFG_H_POS, prefhPos);

	/*-- Init Global Data */
	testIdx = 0;
	pxBuf = (UINT32*)sp5kMalloc((CALIB_PREFLASH_MAX_CNT+10)*sizeof(UINT32));

	/*-- Create SUB-Menu */
	menuPosBackup = MENU_MARK_FLASH | 0x0a080100; 
	menuProcCurrPosSet (menuPosBackup);
	menuProcDisp();
}

void
appCalibFlashStateClose(void)
{
	if(pxBuf != NULL) {
		sp5kFree(pxBuf);
		pxBuf = NULL;
	}
}

void
appCalibFlashStateExit(UINT32 nxtSt, UINT32 param)
{
	appCalibFlashStateClose();
	appStateChange(nxtSt, param);
}

void
appCalibFlashStateOnMsg(
	UINT32 msg,
	UINT32 param
)
{
	switch (msg) {


	case SP5K_MSG_PREFLASH_READY:
		DBG_PRINT("SP5K_MSG_PREFLASH_READY \n");
		sp5kPreflashResultGet(param, &aeResult, &awbResult);
		if(pxBuf!=NULL) {
			pxBuf[testIdx++] = aeResult.aeLuma;
		}
		DBG_PRINT("valid=%d, aeStep=%d, aeDevStep=%d, aeLuma=%d\n",
			aeResult.valid, aeResult.aeStep, aeResult.aeDevStep, aeResult.aeLuma);
		if (aeResult.valid==FALSE) {
			DBG_PRINT("\n!!! Preflash aeResult NOT VALID !!!\n\n");
		}

		if(testIdx>testCountMax || isCancelCalib) {
			calib_SimpleMenuShow();
			break;
		}

		if (menuSelect==0 /*-- H_POS_Test */) {
			prefhPos -= PREFLASH_DELTA_H_POS;
			if (prefhPos<=50) {
				calib_SimpleMenuShow();
				break;
			}
			printf("=========================\n");
			printf("prefhPos(%d) \n\n", prefhPos);
			sp5kPreflashCfgSet(SP5K_PREFLASH_CFG_WIDTH, prefhWidth);
			sp5kPreflashCfgSet(SP5K_PREFLASH_CFG_H_POS, prefhPos);
			Do_H_POS_Test();
		}
		break;

	case SP5K_MSG_AE_READY:
		appAePreviewAdjust(param);
		break;
	case SP5K_MSG_AWB_READY:
		/*appAwbPreviewAdjust(param);*/
		break;
#if 0
	case APP_UI_MSG_STROBE_CHARGE_READY:
		DBG_PRINT("APP_UI_MSG_STROBE_CHARGE_READY \n");
		GREEN_LED_ON;
		appStrobeChargeStop();
		DBG_PRINT("\n**********************\n");
		DBG_PRINT(" STOBE Ready !!\n");
		DBG_PRINT("**********************\n\n");
		break;
#endif
	default:
		break;
	}
}

void
appCalibFlashStateOnKey(
	UINT32 msg,
	UINT32 param
)
{
	switch (msg) {
	case APP_KEY_PRESS_S1:
		//apptestIdxlose();
		//appStateChange(appPreviousStateGet(), APP_STATE_MSG_INIT);
		break;
	case APP_KEY_PRESS_UP:
		menuProcMoveUp();
		menuProcDisp();
		break;
	case APP_KEY_PRESS_DOWN:
		APP_OSD_REFRESH_OFF;
		menuProcMoveDown(); 
		menuProcDisp();
		APP_OSD_REFRESH_ON;
		break;
       case APP_KEY_PRESS_SET:
		Key_SET_Service(param);
		break;
	case APP_KEY_PRESS_MENU:
		beep_echo();
		Key_MENU_Service(param);
		break;
	case APP_KEY_PRESS_DEL:
		beep_echo();
		isCancelCalib = TRUE;
		break;
	case APP_KEY_PRESS_LEFT:
	case APP_KEY_PRESS_RIGHT:
	default:
		break;
	}
}

void
appCalibFlashState(
	UINT32 msg,
	UINT32 param
)
{
	switch (msg) {
	case APP_STATE_MSG_INIT:
		RPrintf("appCalibFlashState");
		appCalibFlashStateInit();
		break;
	case APP_STATE_MSG_CLOSE:
		appCalibFlashStateClose();
		appStateCloseDone();
		break;
	default:
		if (IS_APP_KEY_MSG(msg)) {
			appCalibFlashStateOnKey(msg,param);
		}
		else{
			appCalibFlashStateOnMsg(msg,param);
		}
		break;
	}

}

