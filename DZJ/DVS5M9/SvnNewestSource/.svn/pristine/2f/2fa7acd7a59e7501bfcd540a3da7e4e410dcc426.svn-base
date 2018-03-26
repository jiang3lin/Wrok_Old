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
#include "sp5k_sensor_api.h"
#include "sp5k_cal_api.h"
#include "sp5k_capture_api.h"

#include "app_timer.h"
#include "app_simple_menu.h"
#include "app_tb_util.h"
#include "app_state_ctrl.h"

#include "app_lens_api.h"
#include "app_strobe.h"
#include "app_flash_api.h"
#include "app_playback_osd.h"
#include "app_sound.h"
#include "app_aaa.h"
#include "app_key_def.h"
#include "app_calib_api.h"
#include "app_menu_tab.h"  
#include "app_menu_core.h"
#include "app_menu_disp.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define beep_echo()		do { appBeep(TRUE, FALSE); } while(0)
#define beep_warn()		do { appPreloadSoundPlay(SOUND_ID_WARNING); } while(0)
#define beep_done()		do { appPreloadSoundPlay(SOUND_ID_FOCUSOK); } while(0)

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef void (*pfunc_KeyProcess_t)(UINT32 param);

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
static appAeCb_t *paeCb;
#if 0 /*mark for clean warning , fan.xiao @121108*/
static UINT32 pfhExp;
static UINT32 pfhAgc;
static UINT16 envLuma, pfhLuma, pfhDiffLuma, pfhDefaultLuma;
static UINT32 hPos, pulseW;
static 	int testLoop, iLoop;
#endif
static pfunc_KeyProcess_t pfnKeySet_Proc=NULL;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern UINT32 appAePreviewAdjust(UINT32 param);
extern UINT32 appBtnMsgToKey(UINT32 msgId,UINT32 msgParam);
extern UINT32 appGetRawMaxChannelAvg(void);
extern UINT32 appCalibRawCb(UINT32 addr, UINT32 fmt, void *imgSize);
extern SINT32 pv_tvidx_to_expidx(SINT32 tvidx);
extern SINT32 pv_svidx_to_agcidx(SINT32 svidx);
#if SP5K_ALGORITHM_AE 
extern SINT32 appAEALGLib_ResultGet(UINT32 param, aeResult_t *presult);
#endif
extern UINT32 menuPosBackup; 

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
static void appTbFlashStateExit(UINT32 nxtSt, UINT32 param);
static void P1_Key_SET_Service(UINT32 param);
static void P2_Key_SET_Service(UINT32 param);
static void P3_Key_SET_Service(UINT32 param);
/*-------------------------------------------------------------------------
 * MENU_ZMLENS Operations
 *------------------------------------------------------------------------*/
enum {
	TBFLASH_P1_SEL_BACK,
	TBFLASH_P1_SEL_CURVE_A,
	TBFLASH_P1_SEL_CURVE_B,
	TBFLASH_P1_SEL_CURVE_C,
	TBFLASH_P1_SEL_CURVE_D,
	TBFLASH_P1_SEL_P2,
	TBFLASH_P1_SEL_TOTAL,
};

enum {
	TBFLASH_P2_SEL_BACK,
	TBFLASH_P2_SEL_ZOOM_FAC,
	TBFLASH_P2_SEL_CURVE_X,
	TBFLASH_P2_SEL_CURVE_Y,
	TBFLASH_P2_SEL_CURVE_Z,
	TBFLASH_P2_SEL_P3,
	TBFLASH_P2_SEL_TOTAL,
};

enum {
	TBFLASH_P3_SEL_BACK,
	TBFLASH_P3_SEL_CURVE_SAMPLING_1,
	TBFLASH_P3_SEL_CURVE_SAMPLING_2,
	TBFLASH_P3_SEL_CURVE_SAMPLING_3,
	//TBFLASH_P3_SEL_CURVE_Z,
	//TBFLASH_P3_SEL_P3,
	TBFLASH_P3_SEL_TOTAL,
};

static UINT8 tbflash_P1_Title[]="Preflash P1";
static UINT32 tbflash_P1_SelList[] =
		{(UINT32)"Back",
		 (UINT32)"Curve A0",
		 (UINT32)"Curve A1",
		 (UINT32)"Curve B0",
		 (UINT32)"Curve B1",
		 (UINT32)"P2",
		};

static UINT8 tbflash_P2_Title[]="Preflash P2";
static UINT32 tbflash_P2_SelList[] =
		{(UINT32)"Back",
		 (UINT32)"Zoom Factor",
		 (UINT32)"---",
		 (UINT32)"---",
		 (UINT32)"---",
		 (UINT32)"P3",
		};

static UINT8 tbflash_P3_Title[]="Preflash P3";
static UINT32 tbflash_P3_SelList[] =
		{(UINT32)"Back",
		 (UINT32)"Curv Smpling 1",
		 (UINT32)"Curv Smpling 2",
		 (UINT32)"Curv Smpling 3",
		 (UINT32)"---",
		 (UINT32)"P4",
		};

static UINT32 tbflashSelect;

#define TBFLASH_MENU_SelectGet()	SMENU_SelectGet()
#define TBFLASH_MENU_Scroll(isUp)	SMENU_Scroll(isUp)
#define TBFLASH_MENU_Show()			SMENU_Show()


#define TBFLASH_P1_Config()		SMENU_Config(tbflash_P1_Title, 6, tbflash_P1_SelList, 0, 0)
#define TBFLASH_P1_REDRAW()		do { TBFLASH_P1_Config(); TBFLASH_MENU_Show(); } while(0)

#define TBFLASH_P2_Config()		SMENU_Config(tbflash_P2_Title, 6, tbflash_P2_SelList, 0, 0)
#define TBFLASH_P2_REDRAW()		do { TBFLASH_P2_Config(); TBFLASH_MENU_Show(); } while(0)

#define TBFLASH_P3_Config()		SMENU_Config(tbflash_P3_Title, 6, tbflash_P3_SelList, 0, 0)
#define TBFLASH_P3_REDRAW()		do { TBFLASH_P3_Config(); TBFLASH_MENU_Show(); } while(0)
/*-------------------------------------------------------------------------
 * Function : tbflash_StrobeCharge
 *------------------------------------------------------------------------*/
 #if 0 /*mark for clean warning , fan.xiao @121108*/
static void tbflash_StrobeCharge(void)
{
	int restartCnt=0;
	do {
 		if (FAIL==appStrobeChargeStart()) {
			appStrobeChargeStop();
			restartCnt++;
			if (restartCnt > 5) {
				CLEAR_OSD_SRC;
				dbgOsd_Print(4, "ST_CHG NG");
				APP_SLEEP_MS(5000);
				beep_warn();
				return;
			} else {
				continue;
			}
 		}else {
			break;
 		}
	} while(1);

	do {
		APP_SLEEP_MS(10);
		//printf("++.");
	} while((appStrobeQueryReady()==FALSE));

	appStrobeChargeStop();

	printf(">>> STROBE Ready !! \n");
}
#endif
/*-------------------------------------------------------------------------
 * Function : tbflash_runAe
 *------------------------------------------------------------------------*/
static UINT16 tbflash_RunAe(void)
{
	UINT32 osTime;
	UINT16 aeLuma;
	UINT32 msg, param;

	osTime = appMsTimeGet();
	aeLuma = 0;
	do {
		sp5kHostMsgReceive(&msg, &param);
		if (msg==SP5K_MSG_AE_READY) {
			appAePreviewAdjust(param);
		}

		if (appAeStableGet()==TRUE) {
			aeLuma = appAeLumaGet();
			break;
		} else {
			aeLuma = 0;
		}
		/*-- AE TimeOut Check !! */
		if (appMsTimeGet()-osTime>3000) {
			printf("AE Conv Failed !!\n");
			CLEAR_OSD_SRC;
			beep_warn();
			dbgOsd_Print(3, "AE TimeOut");
			dbgOsd_Print(4, "AE TimeOut");
			dbgOsd_Print(5, "AE TimeOut");
			APP_SLEEP_MS(1000);
			CLEAR_OSD_SRC;
			break;
		}
	} while(1);
	return aeLuma;
}
/*-------------------------------------------------------------------------
 * Function : tbflash_GetAeLuma
 *------------------------------------------------------------------------*/
UINT16 tbflash_GetAeLuma(void)
{
	UINT32 osTime;
	UINT32 msg, param, ret;
	sp5kAeResult_t aeResult;
	UINT16 aeLuma;

	tbUtil_QuickSweepHostMessage(60);

	osTime = appMsTimeGet();
	aeLuma= 0;
	do {
		sp5kHostMsgReceive(&msg, &param);
		if (msg==SP5K_MSG_AE_READY) {
			//printf(" .. ");
			aeLuma = 0;

		#if SP5K_ALGORITHM_AE //lester 20120402
			ret = appAEALGLib_ResultGet(param,&aeResult);
		#else
			ret = sp5kAePreviewResultGet(param, &aeResult);
		#endif

			if (ret==SUCCESS && (aeResult.valid & AAA_INFO_RESULT_AE_VALID)) {
				//printf(" ae ");
				aeLuma = aeResult.aeLuma;
				break;
			}
		}

		if(1000<(appMsTimeGet()-osTime)) {
			break;
		}
	} while(1);

	return aeLuma;

}
/*-------------------------------------------------------------------------
 * Function : tbflash_GetPreflashResult
 *------------------------------------------------------------------------*/
UINT16 tbflash_GetPreflashResult(sp5kAeResult_t *paeResult)
{
	UINT32 osTime;
	UINT32 msg, param;
	sp5kAwbResult_t awbResult;
	UINT16 aeLuma;
	UINT32 ret;

	osTime = appMsTimeGet();
	aeLuma= 0;
	do {
		sp5kHostMsgReceive(&msg, &param);
		if (msg==SP5K_MSG_PREFLASH_READY) {
			//printf("... PREFLASH_RDY @ %d \n", appMsTimeGet());
			//printf(" .. ");
			ret = sp5kPreflashResultGet(param, paeResult, &awbResult);
			if (ret!=0) {
				printf(" !!! sp5kPreflashResultGet failed !!! \n");
				continue;
			}
			if (paeResult->valid & AAA_INFO_RESULT_LUMA_VALID) {
				//printf(" pfh ");
				aeLuma = paeResult->aeLuma;
			} else {
				printf("\n!!! aeResult invalid !!!!\n\n");
				aeLuma = 0;
			}
			break;
		}
	} while(1);

	return aeLuma;
}

/*-------------------------------------------------------------------------
 *  File Name : TBFLASH_OkExitKeyScan
 *------------------------------------------------------------------------*/
UINT32 TBFLASH_OkExitKeyScan(void)
{
	UINT8 szPrompt[]="Set(OK)   Del(EXIT)";
	UINT32 msg, param;
	UINT32 key;

	dbgOsd_Print(7, szPrompt);
	while(1) {
		sp5kHostMsgReceive(&msg, &param);

		if (!IS_SP5K_BTN_MSG(msg)) {
			continue;
		}
		key = appBtnMsgToKey(msg, param);

		if (key==APP_KEY_PRESS_SET) {
			beep_echo();
			return 1;
		} else if (key==APP_KEY_PRESS_DEL) {
			beep_echo();
			return 0;
		}
	}
}

/*-------------------------------------------------------------------------
 * Function : tbflash_ZoomAnalysis
 * Description :
 	Zoom segment will afect the effective aperture size and illumination of
 	sensor. Here we want to konw the ratio and will apply in preflash luma
 	get.
 *------------------------------------------------------------------------*/
#if 0 /*mark for clean warning , fan.xiao @121108*/
static UINT32 tbflash_ZoomAnalysis(void)
{
 	SINT32 expIdx;
    UINT32 agcIdx, sum;
    UINT16 aeLuma;
	int i, j, k;

	stext_Print(160, 120, "Pls Wait ...");
	tbflash_RunAe();
	CLEAR_OSD_SRC;

	/*- Lock AE and show Exp & Agc Index on screen */
	sp5kPreviewExpAgcGet(&expIdx, &agcIdx);
	stext_Print(160, 40, "AE Locked ...");
	stext_Print(160, 40*2, "ExpIdx %d", expIdx);
	stext_Print(160, 40*3, "AgcIdx %d", agcIdx);

	/*-- Loop zoom segment test */
	k=3;
	do{
		for (i=0; i<ZOOM_ZS_NUM; i++) {
			calibLensZoomSegGo(i);
			/*- Get preview luma 10 time and average */
			sum = 0;
			for(j=0; j<10; j++) {
				aeLuma = tbflash_GetAeLuma();
				sum += aeLuma;
				printf("%d ", aeLuma);
			}
			aeLuma = sum/10;
			printf("\n aeLuma = %d @ zoom(%d)\n", aeLuma, i);
		}
	}while(--k);

	/* back to first segment */
	calibLensZoomSegGo(0);

	return SMENU_SEL_YES;
}
#endif
#if 0
/*-------------------------------------------------------------------------
 * Function : tbflash_ISOAnalysis
 * Description :
 	ISO will afect luma of capture. We get preflash luma in preview mode but
 	it does not be applied ISO yet. Here we will consinder the ISO's amplify
 	ratio and decrease/increase some power of snap flash. Base may be ISO50
 	or ISO100.
 *------------------------------------------------------------------------*/
static UINT32 tbflash_ISOAnalysis(void)
{
}
#endif

static UINT32 _adjustFlashPulseWidthDelta(UINT32 cur)
{
	UINT32 delta=250;
#if 0
	/*-- formal */
	if (cur < 150) delta = 10;
	else if (cur < 500)	delta = 20;
	else if (cur < 1000) delta = 50;
	else if (cur < 2000) delta = 100;
	else if (cur < 3000) delta = 150;
	else if (cur < 3500) delta = 200;
	else if (cur < 6000) delta = 350;
	else delta = 500;
#elif 0
	/*-- Just for Test */
	if (cur < 500) delta = 50;
	else if (cur < 3000) 	delta = 300;
	else if (cur < 6000) 	delta = 500;
	else if (cur < 8000) 	delta = 800;
	else delta = 1000;
#endif
	return delta;
}
static void _debugStrobeCharge(void)
{
	int restartCnt=0;
redo:
	if (FAIL==appStrobeChargeStart()) {
		printf("\n\n");
		printf("******************************\n");
		printf(" >> Strobe CHG Start FAIL !!\n");
		printf("******************************\n");
		printf("\n\n");

		appStrobeChargeStop();
		restartCnt++;
		if (restartCnt > 5) {
			printf("\n\n");
			printf("******************************\n");
			printf(" >> FATAL!! STROBE Failed \n");
			printf("******************************\n");
			printf("\n\n");

			appBeep(TRUE, FALSE);APP_SLEEP_MS(500);
			appBeep(TRUE, FALSE);APP_SLEEP_MS(500);
			appBeep(TRUE, FALSE);APP_SLEEP_MS(500);
			appBeep(TRUE, FALSE);APP_SLEEP_MS(500);
			appBeep(TRUE, FALSE);APP_SLEEP_MS(500);

			return ;
		}
		goto redo;
	}
}
/*-------------------------------------------------------------------------
 * Function : tbflash_CurveGen
 *------------------------------------------------------------------------*/
static UINT32 tbflash_CurveGen(UINT8 *pszFname)
{
 	SINT32 expidx;
    UINT32 agcidx;
    UINT16 aeLuma;
	UINT16 envLuma;
	sp5kAeResult_t aeResult;
    int i, logCnt;

	/*-- Confirm Testbed setup */
	if (0==tbUtil_TestProlog("CurveGen Rdy ?")) {
		return SMENU_SEL_NO;
	}

	/*-- Create log file */
	if (!tbLogFile_Open(pszFname)) {
		return SMENU_SEL_NO;
	}
	CLEAR_OSD_SRC;

	/*-- IRIS Control */
	tbUtil_IrisCtrl(1/*isLarge*/);

	/*-- Clean Data Buf */
	tbLogBuf_Clear();

	/*-- Set Exp/Agc before Preflash */
	/*
	expidx = pv_tvidx_to_expidx(5*16);
	agcidx = pv_svidx_to_agcidx(5*16); */
	appPrefhCb_t* pprefhCb;
	pprefhCb = appPrefhCbGet();
	expidx = pprefhCb->prefhExp;
	agcidx = pprefhCb->prefhAgc;

	sp5kPreviewExpAgcSet(expidx, agcidx);

	/*-- You got 10 sec to run away :-) */
	for (i=10; i>0; i--) {
		beep_echo();
		dbgOsd_Print(4, "Count down %d", i);
		tbUtil_QuickSweepHostMessage(1000);
		CLEAR_OSD_SRC;
	}

	/*-- Get envirement Luma */
	envLuma = tbflash_GetAeLuma();

	/*-- Store current parameters of envirement */
	tbLogBuf_Add(expidx);
	tbLogBuf_Add(agcidx);
	tbLogBuf_Add(envLuma);

	/*-- sp5k Flash Init */
	UINT32 strbPos = appPreflashCfgHPosGet();
	sp5kFlashlightInit(SP5K_FLASHLIGHT_INACTIVE_LO);
	sp5kPreflashCfgSet(SP5K_PREFLASH_CFG_H_POS, strbPos);

	/*-- Loop Strobe Curve Test */
	UINT32 cur=0, start, end, delta=50;
	start = 100; end = 17000; //APP_PREFLASH_WIDTH_MAX;
	logCnt=0;
	for (cur=start; cur<=end; cur+=delta) {
		if (tbUtil_TestLoopAbort()) break;

		/*-- adjust delta */
		printf("********** cur(%d) start(%d) end(%d) delta(%d)", cur, start, end, delta);
		delta = _adjustFlashPulseWidthDelta(cur);

		/*-- Configure Preflash Pulse Width */
		CLEAR_OSD_SRC;
		//printf("STRB Width(%d), pos(%d) \n", cur, strbPos);
		dbgOsd_Print(1, "Pulse Width (%d)", cur);
		sp5kPreflashCfgSet(SP5K_PREFLASH_CFG_WIDTH, cur);

		/*-- Charge Strobe */
		//printf("\n>>> Checking STROBE ...\n\n");
		dbgOsd_Print(2, "Charging Strobe");

		/*-- check & recover strobe charge if failure occurs */
		_debugStrobeCharge();
		do {
			APP_SLEEP_MS(10);
		} while(appStrobeQueryReady()==FALSE);

		/*-- Stop strobe charge & fire preflash */
		appStrobeChargeStop();
		sp5kPreflashTrig();

		/*- Get preflash result */
		aeLuma = tbflash_GetPreflashResult(&aeResult);
		envLuma = tbflash_GetAeLuma();

		if (aeLuma==0) aeLuma = 0;
		else if (aeLuma<=envLuma) aeLuma = 1;
		else aeLuma = (aeLuma-envLuma)+1;

		/*-- Store and show result */
		logCnt++;
		tbLogBuf_Add(cur); /* stb width */
		tbLogBuf_Add(envLuma);
		tbLogBuf_Add(aeResult.aeStep);
		tbLogBuf_Add(aeLuma); /* delta luma */

		dbgOsd_Print(6, "AE Luma(%d), %d", aeLuma, cur);
		printf("\nAE Luma = %d @ pulseWidth(%d) \n\n", aeLuma, cur);
		//tbUtil_QuickSweepHostMessage(500);

		#if 1
		if (aeLuma>1005) {
			printf("\nAE Luma (%d) has saturated @ width(%d) \n\n", aeLuma, cur);
			CLEAR_OSD_SRC;
			dbgOsd_Print(4, "Sat.(%d), %d", aeLuma, cur);
			beep_warn();
			break;
		}
		#endif
	}

	printf("\n___________________________\n");
	printf("Pulse Width scan done!! \n");
	printf("___________________________\n\n");

	/*-- Store result to DISK */
	tbLogBuf_PtrReset();
	{
		tbLogFile_Print("\n****************************************\n");
		tbLogFile_Print("Strobe Curve Gen ...\n\n");
		expidx = tbLogBuf_Get();
		agcidx = tbLogBuf_Get();
		envLuma = tbLogBuf_Get();
		tbLogFile_Print("expidx(%d) agcidx(%d) ...\n", expidx, agcidx);

		tbLogFile_Print("\n--- Strobe Pulse Width ---\n");
		tbLogFile_Print("start: %d \n", start);
		tbLogFile_Print("end : %d \n", end);
		tbLogFile_Print("delta : %d \n", delta);
		tbLogFile_Print("\n\n");

		tbLogFile_Print("Strobe H_POS : %d \n", strbPos);
		tbLogFile_Print("Origin aeLuma : %d \n", envLuma);
		tbLogFile_Print("\n\n");

		tbLogFile_Print("Width envLuma aeStep diffLuma ...\n");
		for (i=0;i<logCnt;i++) {
			UINT32 width, diffLuma, envLuma, aeStep;
			width = tbLogBuf_Get();
			envLuma = tbLogBuf_Get();
			aeStep = tbLogBuf_Get();
			diffLuma = tbLogBuf_Get();
			tbLogFile_Print("%d %d %d %d \n", width, envLuma, aeStep, diffLuma);
		}
	}

	tbLogFile_Close();

	SMENU_YesNoConfig("File Saved");
	SMENU_YesNoShow();

	return SMENU_SEL_YES;
}
/*-------------------------------------------------------------------------
 * Function : tbflash_CurveGen
 *------------------------------------------------------------------------*/
static UINT32 tbflash_CurveSampling(UINT8 *pszFname)
{
 	SINT32 expidx;
    UINT32 agcidx;
    UINT16 aeLuma;
	UINT16 envLuma;
	sp5kAeResult_t aeResult;
    int i, logCnt;

	/*-- Confirm Testbed setup */
	if (0==tbUtil_TestProlog("CurvSmpling Rdy ?")) {
		return SMENU_SEL_NO;
	}

	/*-- Create log file */
	if (!tbLogFile_Open(pszFname)) {
		return SMENU_SEL_NO;
	}
	CLEAR_OSD_SRC;

	/*-- IRIS Control */
	tbUtil_IrisCtrl(1/*isLarge*/);

	/*-- Clean Data Buf */
	tbLogBuf_Clear();

	/*-- Set Exp/Agc before Preflash */
	expidx = pv_tvidx_to_expidx(5*16);
	agcidx = pv_svidx_to_agcidx(5*16);
	sp5kPreviewExpAgcSet(expidx, agcidx);

	/*-- You got 5 sec to run away :-) */
	for (i=5; i>0; i--) {
		beep_echo();
		dbgOsd_Print(4, "Count down %d", i);
		tbUtil_QuickSweepHostMessage(1000);
		CLEAR_OSD_SRC;
	}

	/*-- Get envirement Luma */
	envLuma = tbflash_GetAeLuma();

	/*-- Store current parameters of envirement */
	tbLogBuf_Add(expidx);
	tbLogBuf_Add(agcidx);
	tbLogBuf_Add(envLuma);

	/*-- sp5k Flash Init */
	UINT32 strbPos = appPreflashCfgHPosGet();
	sp5kFlashlightInit(SP5K_FLASHLIGHT_INACTIVE_LO);
	sp5kPreflashCfgSet(SP5K_PREFLASH_CFG_H_POS, strbPos);

	/*-- Loop Strobe Curve Test */
	UINT32 cur=0, start, end, delta=200;
	start = 200; end = 3000;
	logCnt=0;
	for (cur=start; cur<=end; cur+=delta) {
		if (tbUtil_TestLoopAbort()) break;

		/*-- adjust delta */
		printf("********** cur(%d) start(%d) end(%d) delta(%d)", cur, start, end, delta);

		/*-- Configure Preflash Pulse Width */
		CLEAR_OSD_SRC;
		//printf("STRB Width(%d), pos(%d) \n", cur, strbPos);
		dbgOsd_Print(1, "Pulse Width (%d)", cur);
		sp5kPreflashCfgSet(SP5K_PREFLASH_CFG_WIDTH, cur);

		/*-- Charge Strobe */
		//printf("\n>>> Checking STROBE ...\n\n");
		dbgOsd_Print(2, "Charging Strobe");

		/*-- check & recover strobe charge if failure occurs */
		_debugStrobeCharge(); APP_SLEEP_MS(200);
		do {
			APP_SLEEP_MS(10);
		} while(appStrobeQueryReady()==FALSE);

		/*-- Stop strobe charge & fire preflash */
		appStrobeChargeStop();
		sp5kPreflashTrig();

		/*- Get preflash result */
		aeLuma = tbflash_GetPreflashResult(&aeResult);
		envLuma = tbflash_GetAeLuma();

		if (aeLuma==0) aeLuma = 0;
		else if (aeLuma<=envLuma) aeLuma = 1;
		else aeLuma = (aeLuma-envLuma)+1;

		/*-- Store and show result */
		logCnt++;
		tbLogBuf_Add(cur); /* stb width */
		tbLogBuf_Add(envLuma);
		tbLogBuf_Add(aeResult.aeStep);
		tbLogBuf_Add(aeLuma); /* delta luma */

		dbgOsd_Print(6, "AE Luma(%d), %d", aeLuma, cur);
		printf("\nAE Luma = %d @ pulseWidth(%d) \n\n", aeLuma, cur);
		//tbUtil_QuickSweepHostMessage(500);

		#if 1
		if (aeLuma>1005) {
			printf("\nAE Luma (%d) has saturated @ width(%d) \n\n", aeLuma, cur);
			CLEAR_OSD_SRC;
			dbgOsd_Print(4, "Sat.(%d), %d", aeLuma, cur);
			beep_warn();
			break;
		}
		#endif
	}

	printf("\n___________________________\n");
	printf("Pulse Width scan done!! \n");
	printf("___________________________\n\n");

	/*-- Store result to DISK */
	tbLogBuf_PtrReset();
	{
		tbLogFile_Print("\n****************************************\n");
		tbLogFile_Print("Strobe Curve Gen ...\n\n");
		expidx = tbLogBuf_Get();
		agcidx = tbLogBuf_Get();
		envLuma = tbLogBuf_Get();
		tbLogFile_Print("expidx(%d) agcidx(%d) ...\n", expidx, agcidx);

		tbLogFile_Print("\n--- Strobe Pulse Width ---\n");
		tbLogFile_Print("start: %d \n", start);
		tbLogFile_Print("end : %d \n", end);
		tbLogFile_Print("delta : %d \n", delta);
		tbLogFile_Print("\n\n");

		tbLogFile_Print("Strobe H_POS : %d \n", strbPos);
		tbLogFile_Print("Origin aeLuma : %d \n", envLuma);
		tbLogFile_Print("\n\n");

		tbLogFile_Print("Width envLuma aeStep diffLuma ...\n");
		for (i=0;i<logCnt;i++) {
			UINT32 width, diffLuma, envLuma, aeStep;
			width = tbLogBuf_Get();
			envLuma = tbLogBuf_Get();
			aeStep = tbLogBuf_Get();
			diffLuma = tbLogBuf_Get();
			tbLogFile_Print("%d %d %d %d \n", width, envLuma, aeStep, diffLuma);
		}
	}

	tbLogFile_Close();

	SMENU_YesNoConfig("SAVE Result ?");
	SMENU_YesNoShow();

	return SMENU_SEL_YES;
}

#if 0
/*-------------------------------------------------------------------------
 * Function : tbflash_PreflashTest
 *------------------------------------------------------------------------*/
static void tbflash_PreflashTest(UINT8 *pszLogName)
{
	CLEAR_OSD_SRC;

	/*-- Clean Data Buf */
	tbLogBuf_Clear();


//	tbUtil_TestProlog("Preflash Rdy ?");

	testLoop=10;
	for (iLoop=0; iLoop<testLoop; iLoop++) {
		printf("\n\nLoop (%d) ...\n", iLoop);
		/*--
		 * Free Run AE and wait AE stable
		 */
		CLEAR_OSD_SRC;
//		dbgOsd_Print(4, "Pls Wait AE ...");
//		tbflash_RunAe();
//		CLEAR_OSD_SRC;


		/*--
		 * Backup Current exp/agc
		 */
//		sp5kPreviewExpAgcGet(&viewExp, &viewAgc);
//		printf("==== Preview Env Info ==\n");
//		printf("exp(%d) agc(%d)\n", viewExp, viewAgc);

		/*--
		 * Set preflash conditions : exp/agc, H_POS & PulseWidth
		 */
		sp5kAeCfgSet(SP5K_AE_ACCUM_PERIOD, 1);
#if 0
		pfhExp = viewExp; pfhAgc=viewAgc;
		sp5kPreviewExpAgcSet(pfhExp, pfhAgc);
#else
		appPreflashCfgExpAgcGet(&pfhExp, &pfhAgc);
		sp5kPreviewExpAgcSet(pfhExp, pfhAgc);
#endif
		envLuma = tbflash_GetAeLuma();
		hPos = appPreflashCfgHPosGet();
		pulseW = appPreflashCfgWidthGet();

		/*--
		 * Show Preflash Conditions
		 */
		printf("===== Preflash Param ==\n");
		printf("exp(%d) agc(%d) \n", pfhExp, pfhAgc);
		printf("HPos(%d), W(%d) \n", hPos, pulseW);
		printf("envLuma(%d) \n", envLuma);

		CLEAR_OSD_SRC;

		/*--
		 * Configure PREFLASH param
		 */
		sp5kFlashlightInit(SP5K_FLASHLIGHT_INACTIVE_LO);
		sp5kPreflashCfgSet(SP5K_PREFLASH_CFG_H_POS, hPos);
		sp5kPreflashCfgSet(SP5K_PREFLASH_CFG_WIDTH, pulseW);

		/*--
		 * Fire Preflash
		 */
		tbflash_StrobeCharge();
		sp5kPreflashTrig();

		/*--
		 * Get preflash result
		 */
		sp5kAeResult_t aeResult;
		pfhLuma = tbflash_GetPreflashResult(&aeResult);
		envLuma = tbflash_GetAeLuma();

		if (pfhLuma <= envLuma) {
			pfhDiffLuma = 1;
		} else {
			pfhDiffLuma = pfhLuma - envLuma;
		}

		/*-- Recalculate New PulseWidth */
		UINT32 tgtLuma = 400;
		UINT32 tgtCalcLuma;

		pfhDefaultLuma = appPreflashCfgLumaGet(pfhDiffLuma);

		float floTmp;
		floTmp = ((float)pfhDefaultLuma)/pfhDiffLuma;

		UINT32 lookupWidth=0;
		UINT32 mfhLuma=0;
		if (tgtLuma <= envLuma) {
			printf(" !! Luma : tgt(%d), env(%d) \n", tgtLuma, envLuma);
			printf("NO Strobe needed !!\n");
			dbgOsd_Print(4, "Preflh Cancelled");
			beep_warn();
			tgtCalcLuma = 0;
		} else {
			floTmp = (tgtLuma-envLuma)*floTmp;
			tgtCalcLuma = (UINT32)floTmp;

			lookupWidth = appPreflashLuma2WidthLookup(tgtCalcLuma);

			/*-- Test MainFlash */
			sp5kPreflashCfgSet(SP5K_PREFLASH_CFG_WIDTH, lookupWidth);
			sp5kPreflashCfgSet(SP5K_PREFLASH_CFG_H_POS, hPos);
			tbflash_StrobeCharge();
			sp5kPreflashTrig();

			sp5kAeResult_t aeResult;
			mfhLuma = tbflash_GetPreflashResult(&aeResult);

			dbgOsd_Print(4, "Main Luma (%d)", mfhLuma);
			printf("\nMAIN Flash Luma (%d) \n", mfhLuma);
		}

		tbLogBuf_Add(envLuma);
		tbLogBuf_Add(pfhLuma);
		tbLogBuf_Add(pfhDiffLuma);
		tbLogBuf_Add(tgtCalcLuma);
		tbLogBuf_Add(mfhLuma);
		tbLogBuf_Add(lookupWidth);

	}
	/*-- Console Print */
	tbLogBuf_PtrReset();
	printf("env  pfh  diff  calc   main  lookupW\n");
	for (iLoop=0; iLoop<testLoop; iLoop++) {
		printf("%d\t%d\t%d\t%d\t%d\t%d\n",
				tbLogBuf_Get(),
				tbLogBuf_Get(),
				tbLogBuf_Get(),
				tbLogBuf_Get(),
				tbLogBuf_Get(),
				tbLogBuf_Get());
	}
	/*-- Store Result */
	if (pszLogName) {
		tbLogFile_Open(pszLogName);
		tbLogBuf_PtrReset();
		tbLogFile_Print("Preflash param === \n");
		tbLogFile_Print("hPos(%d), Width(%d), DefLuma(%d) \n\n", hPos, pulseW, pfhDefaultLuma);
		tbLogFile_Print("pfhExp(%d), pfhAgc(%d) \n\n", pfhExp, pfhAgc);

		tbLogFile_Print("env  pfh  diff  calc   main  lookupW\n");
		for (iLoop=0; iLoop<testLoop; iLoop++) {
			tbLogFile_Print("%d\t%d\t%d\t%d\t%d\t%d\n",
					tbLogBuf_Get(),
					tbLogBuf_Get(),
					tbLogBuf_Get(),
					tbLogBuf_Get(),
					tbLogBuf_Get(),
					tbLogBuf_Get());
		}

		tbLogFile_Close();
	}

	beep_done();
	TBFLASH_P1_REDRAW();
}
#endif
/*-------------------------------------------------------------------------
 * Function : tbflash_PreflashResultTest
 *------------------------------------------------------------------------*/
#if 0
UINT32 *aeDebugBufGet(void);
void aeDebugBufClear(void);
void aeDebugDataAdd(UINT32 data);
UINT32 aeDebugDataGet(void);
void aeDebugBufPtrReset(void);
void aeDebugEnable(BOOL isEn);
void aeDebugDump(void)
{
	int i;
	int row, col;
	UINT32 gridsize;
	UINT32 sumLuma, colLuma;
	UINT32 data;

	aeDebugBufPtrReset();

	for (i=0; i<4*1024; i++) {
		if (aeDebugDataGet()!=0xFFFFAAAA) {
			continue;
		}
		printf("\n\n");
		tbLogFile_Print("\n\n");

		data = aeDebugDataGet();
		printf(">> frame: %d \n", data);
		tbLogFile_Print(">> frame: %d \n", data);

		gridsize= aeDebugDataGet();
		printf(">> gridsize: %d \n", gridsize);
		tbLogFile_Print(">> gridsize: %d \n", gridsize);

		sumLuma = 0;
		for (row=0; row<15; row++) {
			colLuma = 0;
			for (col=0; col<15; col++) {
				data = aeDebugDataGet();
				printf("%4d ", data);
				tbLogFile_Print("%4d ", data);
				colLuma += data;
			}
			colLuma = colLuma/15;
			printf(",\tavg= %d \n", colLuma);
			tbLogFile_Print(",\tavg= %d \n", colLuma);
			sumLuma += colLuma;
		}
		sumLuma = sumLuma / 15;
		printf("\navgLuma= %d <<-\n", sumLuma);
		tbLogFile_Print("\navgLuma= %d <<-\n", sumLuma);
	}

}
#endif
#if 0
void memdump(void *mem, UINT32 len);
static void tbflash_PreflashResultTest(void)
{
	int k;

	testLoop=5;
	for (iLoop=0; iLoop<testLoop; iLoop++) {
		beep_done(); APP_SLEEP_MS(1000);
	}

	CLEAR_OSD_SRC;

	/*-- Clean Data Buf */
	tbLogBuf_Clear();

	hPos = appPreflashCfgHPosGet();
	pulseW = appPreflashCfgWidthGet();

	sp5kFlashlightInit(SP5K_FLASHLIGHT_INACTIVE_LO);

tbLogFile_Open("D:\\SP_PREFH.TXT");
	for (testLoop=20, iLoop=0; iLoop<testLoop; iLoop++) {
		printf("\n\nLoop(%d) ===\n", iLoop);

		/*-- Free Run AE and wait AE stable */
		dbgOsd_Print(4, "Pls Wait AE ...");
		envLuma = tbflash_RunAe();
		CLEAR_OSD_SRC;

		/*-- Backup Current exp/agc */
		sp5kPreviewExpAgcGet(&viewExp, &viewAgc);

		//printf("==== Preview Env Info ==\n");
		//printf("exp(%d) agc(%d)\n", viewExp, viewAgc);
		//printf("envLuma(%d) \n", envLuma);
		sp5kPreviewExpAgcSet(208, 112);

		/*-- Discharge Flash */
		APP_SLEEP_MS(200);beep_warn();
		sp5kPreflashCfgSet(SP5K_PREFLASH_CFG_H_POS, hPos);
		sp5kPreflashCfgSet(SP5K_PREFLASH_CFG_WIDTH, 5000);
		sp5kPreflashTrig();tbflash_GetPreflashResult();
		APP_SLEEP_MS(100);

		/*-- Fire Preflash */
		sp5kPreflashCfgSet(SP5K_PREFLASH_CFG_H_POS, 238/*hPos*/);
		sp5kPreflashCfgSet(SP5K_PREFLASH_CFG_WIDTH, 400/*pulseW*/);
//aeDebugBufClear();
//aeDebugEnable(1); //sp5kSensorVvalidWait();
		for (k=0; k<2; k++) {
			beep_done(); APP_SLEEP_MS(200);
		}
		tbflash_StrobeCharge();
		sp5kPreflashTrig();
//memdump(0xb00090e0, 16);

		/*- Get preflash result */
		pfhLuma = tbflash_GetPreflashResult();
//memdump(0xb00090e0, 16);
//profLogAdd(1, "<<<<<<<<<\n");
//profLogDump(1);

//aeDebugEnable(0);
//aeDebugDump();

		tbLogBuf_Add(viewExp);
		tbLogBuf_Add(viewAgc);
		tbLogBuf_Add(envLuma);
		tbLogBuf_Add(pfhLuma);
		//printf("envLuma(%d), pfhLuma(%d) \n", envLuma, pfhLuma);
	}
tbLogFile_Close();
#if 0
	printf("===== Preflash Param ==\n");
	printf("viewexp(%d) viewagc(%d)\n", viewExp, viewAgc);
	printf("HPos(%d), W(%d) \n", hPos, pulseW);
	printf("envLuma(%d)  pfhLuma(%d) \n", envLuma, pfhLuma);
#endif

	/*-- save file */
	beep_echo();
	SMENU_YesNoConfig("SAVE Result ?");
	SMENU_YesNoShow();
	if (SMENU_SEL_YES==SMENU_SelScan()) {
		tbLogFile_Open("D:\\PFH_STRB.TXT");

		tbLogBuf_PtrReset();

		tbLogFile_Print("=== PreflashGet Reliability Test === \n");
		tbLogFile_Print("hPos(%d), pulseW(%d) \n\n", hPos, pulseW);

		tbLogFile_Print("vwExp vwAgc envLuma pfhLuma\n");
		for (iLoop=0; iLoop<testLoop; iLoop++) {
			tbLogFile_Print("%d\t%d\t%d\t%d\n",
						tbLogBuf_Get(),  /* viewExp */
						tbLogBuf_Get(),	 /* viewAgc */
						tbLogBuf_Get(),  /* envLuma */
						tbLogBuf_Get()	 /* pfhLuma */
						);
		}

		tbLogFile_Close();
	}

	printf("\n");

	beep_done();
	TBFLASH_P1_REDRAW();


}
#endif

/*-------------------------------------------------------------------------
 * Function : tbflash_AeConvergeTest
 *------------------------------------------------------------------------*/
void tbflash_AeConvergeTest(void)
{
	UINT16 aeLuma;
	int i;

	CLEAR_OSD_SRC;

	/*- Confirm Testbed setup */
	tbUtil_TestProlog("AeConv Rdy ?");
	dbgOsd_Print(4, "Pls Wait");

	/*-- Clear Data Buf */
	tbLogBuf_Clear();

	UINT16 convLuma;

	convLuma = tbflash_RunAe();
	SINT32 expIdx;
	UINT32 agcIdx;
	sp5kPreviewExpAgcGet(&expIdx, &agcIdx);

	/*-- Log exp/agc Idx while converged */
	tbLogBuf_Add(convLuma);
	tbLogBuf_Add(expIdx);
	tbLogBuf_Add(agcIdx);
#if 0
	CLEAR_OSD_SRC;
	dbgOsd_Print(1, "AE Conv ...");
	dbgOsd_Print(3, "luma %d", convLuma);
	dbgOsd_Print(4, "exp %d", expIdx);
	dbgOsd_Print(5, "agc %d", agcIdx);
	TBFLASH_OkExitKeyScan();
	CLEAR_OSD_SRC;
#endif
	dbgOsd_Print(4, "Running AeConv");

#if 1
	for (i=0; i<100; i++) {
		aeLuma = tbflash_RunAe();
		tbLogBuf_Add(aeLuma);
	}
#else
	/*-- Log aeLuma */
	aeLuma = 0;
	for (i=0; i<100; i++) {
		do {
			sp5kHostMsgReceive(&msg, &param);
			if (msg==SP5K_MSG_AE_READY) {
				//printf(" .. ");
				aeLuma = 0;
				sp5kAePreviewResultGet(param, &aeResult);
				if (aeResult.valid & AAA_INFO_RESULT_AE_VALID) {
					//printf(" ae ");
					aeLuma = aeResult.aeLuma;
				}
				tbLogBuf_Add(aeLuma);
				break;
			}
		} while(1);
	}
#endif

	/*-- save file */
	tbLogBuf_PtrReset();

	for (i=0; i<3; i++) {
		if (!tbLogFile_Open("D:\\AE_RUN.TXT")) {
			beep_warn(); CLEAR_OSD_SRC;
			dbgOsd_Print(4, "FOpen Err");
			dbgOsd_Print(5, "%dth", i);
			APP_SLEEP_MS(3000);
			CLEAR_OSD_SRC;
			continue;
		}
	}

	tbLogFile_Print("Preview AE Locked params \n");
	tbLogFile_Print("convLuma(%d), exp(%d), agc(%d) \n", tbLogBuf_Get(), tbLogBuf_Get(), tbLogBuf_Get());
	for (i=0; i<100; i++) {
		tbLogFile_Print("aeLuma\t%d \n", tbLogBuf_Get());
	}
	tbLogFile_Close();

	beep_done();

	CLEAR_OSD_SRC;

}

/*-------------------------------------------------------------------------
 * Function : tbflash_ZoomFactorTest
 *------------------------------------------------------------------------*/
void tbflash_ZoomFactorTest(void)
{
	SINT32 expidx;
	UINT32 agcidx;
    UINT16 aeLuma;
	UINT16 envLuma;
		sp5kAeResult_t aeResult;
    int i, testNum;

	/*-- Confirm Testbed setup */
	if (0==tbUtil_TestProlog("ZFacTest Rdy ?")) {
		return ;
	}

	/*-- Create log file */
	if (!tbLogFile_Open("D:\\FH_ZFAC.TXT")) {
		return ;
	}
	CLEAR_OSD_SRC;

	/*-- IRIS Control */
	tbUtil_IrisCtrl(1/*isLarge*/);

	/*-- Clean Data Buf */
	tbLogBuf_Clear();

	/*-- Set Exp/Agc before Preflash */
	appPreflashCfgExpAgcGet(&expidx, &agcidx);
		sp5kPreviewExpAgcSet(expidx, agcidx);

	/*-- You got 5 sec to run away :-) */
	for (i=5; i>0; i--) {
		beep_echo();
		dbgOsd_Print(4, "Count down %d", i);
		tbUtil_QuickSweepHostMessage(1000);
		CLEAR_OSD_SRC;
	}

	/*-- Get envirement Luma */
	envLuma = tbflash_GetAeLuma();

	/*-- sp5k Flash Init */
	UINT32 strbPos = appPreflashCfgHPosGet();
	sp5kFlashlightInit(SP5K_FLASHLIGHT_INACTIVE_LO);
	sp5kPreflashCfgSet(SP5K_PREFLASH_CFG_H_POS, strbPos);

	UINT32 pulseW = appPreflashCfgWidthGet();
	sp5kPreflashCfgSet(SP5K_PREFLASH_CFG_WIDTH, pulseW);

	/*-- Store current parameters of envirement */
	tbLogBuf_Add(expidx);
	tbLogBuf_Add(agcidx);
	tbLogBuf_Add(strbPos);
	tbLogBuf_Add(pulseW);

	/*-- Loop for ZoomFactor Test */
	UINT32 cur=0, start, end, delta=1;
	start = 0; end = ZOOM_ZS_NUM-1;
	for (cur=start; cur<=end; cur+=delta) {
		/*-- adjust delta */
		printf("********** cur(%d) start(%d) end(%d) delta(%d)", cur, start, end, delta);

		APP_SLEEP_MS(500);
		calibLensZoomSegGo(cur);
		CLEAR_OSD_SRC;
		//printf("STRB Width(%d), pos(%d) \n", cur, strbPos);
		dbgOsd_Print(1, "Zoom S(%d)", cur);

		testNum = 10;
		while(testNum>0) {
			/*-- Charge Strobe */
			//printf("\n>>> Checking STROBE ...\n\n");
			dbgOsd_Print(2, "Charging Strobe");

			/*-- check & recover strobe charge if failure occurs */
			_debugStrobeCharge(); APP_SLEEP_MS(200);
			do {
				APP_SLEEP_MS(10);
			} while(appStrobeQueryReady()==FALSE);

			/*-- Stop strobe charge & fire preflash */
			appStrobeChargeStop();
			sp5kPreflashTrig();

			/*- Get preflash result */
			aeLuma = tbflash_GetPreflashResult(&aeResult);
			envLuma = tbflash_GetAeLuma();

			if (aeLuma==0) aeLuma = 0;
			else if (aeLuma<=envLuma) aeLuma = 1;
			else aeLuma = (aeLuma-envLuma)+1;

			/*-- Store and show result */
			testNum--;
			//tbLogBuf_Add(envLuma);
			tbLogBuf_Add(aeLuma); /* delta luma */

			dbgOsd_Print(6, "AE Luma(%d), %d", aeLuma, cur);
			printf("\nAE Luma = %d @ ZS(%d) \n\n", aeLuma, cur);

		}
	}

	printf("\n___________________________\n");
	printf("Flash ZFactor Test done!! \n");
	printf("___________________________\n\n");

	/*-- Store result to DISK */
	tbLogBuf_PtrReset();
	{
		tbLogFile_Print("\n****************************************\n");
		tbLogFile_Print("Flash ZoomFactor Test ...\n\n");
		expidx = tbLogBuf_Get();
		agcidx = tbLogBuf_Get();
		tbLogFile_Print("expidx(%d) agcidx(%d) ...\n", expidx, agcidx);

		tbLogFile_Print("\n--- ZS Motion ---\n");
		tbLogFile_Print("start: %d \n", start);
		tbLogFile_Print("end : %d \n", end);
		tbLogFile_Print("delta : %d \n", delta);
		tbLogFile_Print("\n\n");

		tbLogFile_Print("Strobe H_POS : %d \n", tbLogBuf_Get());
		tbLogFile_Print("Strobe PULSE_W : %d \n", tbLogBuf_Get());
		tbLogFile_Print("\n\n");

		tbLogFile_Print("ZS envLuma aeStep diffLuma ...\n");
		start = 0; end = ZOOM_ZS_NUM-1;
		for (cur=start; cur<=end; cur+=delta) {
			tbLogFile_Print("%dth ", cur);
			testNum = 10;
			while(testNum>0) {
				tbLogFile_Print("%d ", tbLogBuf_Get());
				testNum--;
			}
			tbLogFile_Print("\n");
		}
	}

	tbLogFile_Close();

	appLensClose(); APP_SLEEP_MS(2000);
	appLensOpen(); APP_SLEEP_MS(2000);

	//return SMENU_SEL_YES;

}

/*-------------------------------------------------------------------------
 * Function : Key_MENU_Service
 *------------------------------------------------------------------------*/
static void Key_MENU_Service(UINT32 param)
{
	tbflashSelect = TBFLASH_MENU_SelectGet();
	switch (tbflashSelect) {
	default:
		break;
	}
}

/*-------------------------------------------------------------------------
 * Function : P1_Key_SET_Service
 *------------------------------------------------------------------------*/
static void P1_Key_SET_Service(UINT32 param)
{
	tbflashSelect = TBFLASH_MENU_SelectGet();
	switch (tbflashSelect) {
	default:
		break;
	case TBFLASH_P1_SEL_BACK:
		CLEAR_OSD_SRC;
		appTbFlashStateExit(APP_STATE_CALIBRATION, STATE_PARAM_NORMAL_INIT);
		break;
	case TBFLASH_P1_SEL_CURVE_A:
		CLEAR_OSD_SRC;
		tbflash_CurveGen("D:\\STCURV_A.TXT");
		TBFLASH_P1_REDRAW();
		break;
	case TBFLASH_P1_SEL_CURVE_B:
		CLEAR_OSD_SRC;
		tbflash_CurveGen("D:\\STCURV_B.TXT");
		TBFLASH_P1_REDRAW();
		break;
	case TBFLASH_P1_SEL_CURVE_C:
		CLEAR_OSD_SRC;
		tbflash_CurveGen("D:\\STCURV_C.TXT");
		TBFLASH_P1_REDRAW();
		break;
	case TBFLASH_P1_SEL_CURVE_D:
		CLEAR_OSD_SRC;
		tbflash_CurveGen("D:\\STCURV_D.TXT");
		TBFLASH_P1_REDRAW();
		break;
	case TBFLASH_P1_SEL_P2:
		/*-- Create SUB-Menu */
		TBFLASH_P2_REDRAW();
		pfnKeySet_Proc = P2_Key_SET_Service;
		break;
	}
}

/*-------------------------------------------------------------------------
 * Function : P2_Key_SET_Service
 *------------------------------------------------------------------------*/
static void P2_Key_SET_Service(UINT32 param)
{
	tbflashSelect = TBFLASH_MENU_SelectGet();
	switch (tbflashSelect) {
	default:
		break;
	case TBFLASH_P2_SEL_BACK:
		/*-- Create SUB-Menu */
		TBFLASH_P1_REDRAW();
		pfnKeySet_Proc = P1_Key_SET_Service;
		break;
	case TBFLASH_P2_SEL_ZOOM_FAC:
		CLEAR_OSD_SRC;
		tbflash_ZoomFactorTest();
		TBFLASH_P2_REDRAW();
		break;
	case TBFLASH_P2_SEL_CURVE_X:
		CLEAR_OSD_SRC;
		//tbflash_CurveGen("D:\\STCURV_X.TXT");
		TBFLASH_P2_REDRAW();
		break;
	case TBFLASH_P2_SEL_CURVE_Y:
		CLEAR_OSD_SRC;
		//tbflash_CurveGen("D:\\STCURV_Y.TXT");
		TBFLASH_P2_REDRAW();
		break;
	case TBFLASH_P2_SEL_CURVE_Z:
		CLEAR_OSD_SRC;
		//tbflash_CurveGen("D:\\STCURV_Z.TXT");
		TBFLASH_P2_REDRAW();
		break;
	case TBFLASH_P2_SEL_P3:
		/*-- Create SUB-Menu */
		TBFLASH_P3_REDRAW();
		pfnKeySet_Proc = P3_Key_SET_Service;
		break;
	}
}


/*-------------------------------------------------------------------------
 * Function : P3_Key_SET_Service
 *------------------------------------------------------------------------*/
static void P3_Key_SET_Service(UINT32 param)
{
	tbflashSelect = TBFLASH_MENU_SelectGet();
	switch (tbflashSelect) {
	default:
		break;
	case TBFLASH_P3_SEL_BACK:
		/*-- Create SUB-Menu */
		TBFLASH_P2_REDRAW();
		pfnKeySet_Proc = P2_Key_SET_Service;
		break;
	case TBFLASH_P3_SEL_CURVE_SAMPLING_1:
		CLEAR_OSD_SRC;
		tbflash_CurveSampling("D:\\STSMPL01.TXT");
		TBFLASH_P3_REDRAW();
		break;
	case TBFLASH_P3_SEL_CURVE_SAMPLING_2:
		CLEAR_OSD_SRC;
		tbflash_CurveSampling("D:\\STSMPL02.TXT");
		TBFLASH_P3_REDRAW();
		break;
	case TBFLASH_P3_SEL_CURVE_SAMPLING_3:
		CLEAR_OSD_SRC;
		tbflash_CurveSampling("D:\\STSMPL03.TXT");
		TBFLASH_P3_REDRAW();
		break;
	}
}

/*-------------------------------------------------------------------------
 * Function : appTbFlashStateInit
 *------------------------------------------------------------------------*/
static void appTbFlashStateInit(void)
{

	CLEAR_OSD_SRC;

	/*-- Set View Config */
	tbUtil_ViewEnter();

	/*-- AE/AWB ON */
	tbUtil_AeControl(1);
	tbUtil_AwbControl(1);

	/*-- Large IRIS */
	appIrisLargeSet();

	/*-- OB Subtraction */
	tbUtil_ObApply();

	CLEAR_OSD_SRC;

	/*-- Get aeCb data block */
	paeCb = appAeCbGet();

	/*-- Init Global Data */
	tbLogBuf_Create();

	//sp5kAeCfgSet(SP5K_AE_WIN_POS_TEST, 1);
	sp5kAeCfgSet(SP5K_AE_ACCUM_PERIOD, 3);

	/*-- Enable Strobe Power */
	appStrobePowerOn();
	appStrobeChargeStart();

	appPreflashInit();

	/*-- Create SUB-Menu */
	menuPosBackup = MENU_MARK_TESTBENCH | 0x0a0d0701; 
	menuProcCurrPosSet (menuPosBackup);
	menuProcDisp();

	pfnKeySet_Proc = P1_Key_SET_Service;
}

/*-------------------------------------------------------------------------
 * Function : appTbFlashStateClose
 *------------------------------------------------------------------------*/
static void appTbFlashStateClose(void)
{
	tbLogBuf_Destroy();

	/*-- Enable Strobe Power */
	appStrobePowerOff();
}

/*-------------------------------------------------------------------------
 * Function : appTbFlashStateExit
 *------------------------------------------------------------------------*/
static void appTbFlashStateExit(UINT32 nxtSt, UINT32 param)
{
	appTbFlashStateClose();
	appStateChange(nxtSt, param);
}

/*-------------------------------------------------------------------------
 * Function : appTbFlashStateOnMsg
 *------------------------------------------------------------------------*/
static void appTbFlashStateOnMsg(UINT32 msg, UINT32 param)
{
	switch (msg) {
	default:
		break;
	case SP5K_MSG_AE_READY:
		appAePreviewAdjust(param);
		break;
	case SP5K_MSG_AWB_READY:
		/*appAwbPreviewAdjust(param);*/
		break;
	}
}

/*-------------------------------------------------------------------------
 * Function : appTbFlashStateOnKey
 *------------------------------------------------------------------------*/
static void appTbFlashStateOnKey(UINT32 msg, UINT32 param)
{
	switch (msg) {
	case APP_KEY_PRESS_S1:
		break;

	case APP_KEY_PRESS_UP:
		menuProcMoveUp();
		menuProcDisp();
		break;
	case APP_KEY_PRESS_DOWN:
		menuProcMoveDown(); 
		menuProcDisp();
		break;

	case APP_KEY_PRESS_SET:
		beep_echo();
		(*pfnKeySet_Proc)(param);
		break;

	case APP_KEY_PRESS_MENU:
		beep_echo();
		Key_MENU_Service(param);
		break;

	case APP_KEY_PRESS_DEL:
		//beep_echo();
		break;
	case APP_KEY_PRESS_LEFT:
	case APP_KEY_PRESS_RIGHT:
	default:
		break;
	}
}

/*-------------------------------------------------------------------------
 * Function : appTbFlashState
 *------------------------------------------------------------------------*/
void appTbFlashState(UINT32 msg, UINT32 param)
{
	switch (msg) {
	case APP_STATE_MSG_INIT:
		RPrintf("appTbFlashState");
		appTbFlashStateInit();
		break;
	case APP_STATE_MSG_CLOSE:
		appTbFlashStateClose();
		appStateCloseDone();
		break;
	default:
		if (IS_APP_KEY_MSG(msg)) {
			appTbFlashStateOnKey(msg,param);
		}
		else{
			appTbFlashStateOnMsg(msg,param);
		}
		break;
	}

}

