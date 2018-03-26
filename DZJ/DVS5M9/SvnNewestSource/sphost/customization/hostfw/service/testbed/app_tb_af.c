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
#include "sp5k_af_api.h"

#include "app_timer.h"
#include "app_simple_menu.h"
#include "app_tb_util.h"
#include "app_state_ctrl.h"

#include "app_zoom_api.h"
#include "app_still.h"
#include "app_lens_api.h"
#include "app_playback_osd.h"
#include "app_sound.h"
#include "app_aaa.h"
#include "app_key_def.h"
#include "app_calib_api.h"
#include "app_strobe.h"
#include "app_menu_tab.h"  
#include "app_menu_core.h"
#include "app_menu_disp.h"

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/


/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

static void P_Main_Key_SET_Service(UINT32 param);
static void P_Sub_Key_SET_Service(UINT32 param);
static void Key_MENU_Service(UINT32 param);
static void P_Scan_Setting_Key_DIR_Service(UINT32 param);
static void P_Scan_Setting_Key_SET_Service(UINT32 param);
static void P_Exp_Setting_Key_DIR_Service(UINT32 msg);
static void P_Agc_Setting_Key_DIR_Service(UINT32 msg);
static void P_ExpAgc_Setting_Key_SET_Service(UINT32 msg);
static void P_StartTesting_Key_SET_Service(UINT32 msg);

static void appTbAFStateExit(UINT32 nxtSt, UINT32 param);
extern UINT32 appAePreviewAdjust(UINT32 param);
extern void lensMotorPowerOn(void);     /*Lation@20121120*/
extern UINT32 lensFocusJob(SINT16 pos);     /*Lation@20121120*/
extern void lensMotorPowerOff(void);    /*Lation@20121120*/
extern UINT32 menuPosBackup;  

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
static UINT32 tbafSelect;
static pfunc_KeyProcess_t pfnKeySet_Proc=NULL, pfnKeyDir_Proc=NULL, pfnKeyMenu_Proc=NULL;
static UINT32 tbaf_P_Scan_Setting_SelList[] =
		{(UINT32)"Start Testing",
		 (UINT32)"Beginning Value: XXX",
		 (UINT32)"End Value: XXX",
		 (UINT32)"Interval: XXX",
 		 (UINT32)"EXP:  XXX",
 		 (UINT32)"AGC:  XXX",
 		 (UINT32)"Back"
		};
static UINT32 setlist[]	={(UINT32)"                   SET"};
static UINT32 focusscanlist[] ={(UINT32)"Start Full Scan: 0--700",
								(UINT32)"Focus B/g Pos: XXX",
								(UINT32)"Focus End Pos: XXX",
								(UINT32)"Start Scan: XXX--XXX",
							  	(UINT32)"Start Pos XXX Only",
							  	(UINT32)"Back"};
#if 0
static UINT32 tbaf_P_ISO_Setting_SelList[] =
		{(UINT32)"ISO 100  ",
		 (UINT32)"ISO 200  ",
		 (UINT32)"ISO 400  ",
		 (UINT32)"ISO 800  ",
		 (UINT32)"Back"
		};
#endif
static UINT32 tbaf_P_Main_SelList[] =
		{(UINT32)"Noise Coring Value Test",
		 (UINT32)"Noise Coring Theshold Test",
		 (UINT32)"Noise Theshold Test",
		 (UINT32)"Back"
		};
static UINT8 AfCfgSelector[3] =  {SP5K_AF_CORING_VALUE, SP5K_AF_NOISE_CORING, SP5K_AF_NOISE_THR};

static SINT32 CurrExp;
static UINT32 CurrAgc;
static UINT8 ScanSetStr[30];
static UINT8 AgcMenuIdx = 0;
static UINT8 TestItem, AfConfSet = FAIL;//, AfWinShow = TRUE;
static SINT16 bgvalue, endvalue = 255, interval = 2, *settingvalue, currvalue;
static SINT16 fendpos = 700, fbgpos = 0;
sp5kExpLimit_t ExpLimit;
sp5kAgcLimit_t AgcLimit;
static UINT8 AeRdyCnt = 0;
static BOOL wFocusScan = FALSE;
static UINT32 ZSeg = 0;

/*Ptr to file recording buffer */
static UINT8 *Noise_Buf = NULL, *Noise_Rec = NULL;
UINT32 Noise_Rec_fd;


/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

enum {
	TBAF_P_Main_SEL_CORVAL,
	TBAF_P_Main_SEL_CORTHR,
	TBAF_P_Main_SEL_NOSCNT,
	TBAF_P_Main_SEL_BACK,
	TBAF_P_Main_SEL_TOTAL
};

enum {
	TBAF_P_Sub_SEL_START,
	TBAF_P_Sub_SEL_BG_VALUE,
	TBAF_P_Sub_SEL_END_VALUE,
	TBAF_P_Sub_SEL_INTERVAL,
	TBAF_P_Sub_SEL_EXP,
	TBAF_P_Sub_SEL_AGC,
	TBAF_P_Sub_SEL_BACK,
	TBAF_P_Sub_SEL_TOTAL
};

enum {
	TBAF_P_SET_SEL_SET,
	TBAF_P_SET_SEL_TOTAL
};
enum {
	TBAF_P_StartTesting_SEL_FULL_fPOS,
	TBAF_P_StartTesting_SEL_BEGIN_fPOS,
	TBAF_P_StartTesting_SEL_END_fPOS,
	TBAF_P_StartTesting_SEL_SPECIED_fPOS,
	TBAF_P_StartTesting_SEL_CURRENT_fPOS,
	TBAF_P_StartTesting_SEL_BACK,
	TBAF_P_StartTesting_SEL_TOTAL
};

#define Agc_ISO100 paeCb->pviso
#define Exp_ISO100 STD_EXP_1_32
#define afcorthr *(volatile UINT8 *)0xb0002405
#define afcorval *(volatile UINT8 *)0xb0002406
#define afnosthr *(volatile UINT8 *)0xb0002407


/**************************************************************************
 *								M A C R O S 							  *
 **************************************************************************/



#define TBAF_MENU_SHOW()					SMENU_Show()
#define TBAF_MENU_SELECTGET()				SMENU_SelectGet()
#define TBAF_MENU_SCROLL(isUp)				SMENU_Scroll(isUp)


#define TBAF_P_MAIN_CONFIG()				SMENU_Config("AF Noise Test", 4, tbaf_P_Main_SelList, 0, 0)
#define TBAF_P_MAIN_REDRAW()				do { TBAF_P_MAIN_CONFIG(); TBAF_MENU_SHOW(); } while(0)



#define TBAF_P_VALUE_SET_CONFIG()			SMENU_Config(ScanSetStr, 1, setlist, 0, 0)
#define TBAF_P_VALUE_SET_REDRAW()			do { TBAF_P_VALUE_SET_CONFIG(); TBAF_MENU_SHOW(); } while(0)
#define TBAF_P_VALUE_SET_UPDATE()			SMENU_TitleUpdate(ScanSetStr)


#define TEST_CONF_SET						sp5kPreviewExpAgcGet(&CurrExp, &CurrAgc);\
											sprintf((char *)tbaf_P_Scan_Setting_SelList[1], "Beginning Value:%3d", bgvalue);\
											sprintf((char *)tbaf_P_Scan_Setting_SelList[2], "End Value:%3d", endvalue);\
											sprintf((char *)tbaf_P_Scan_Setting_SelList[3], "Interval:%3d", interval);\
											sprintf((char *)tbaf_P_Scan_Setting_SelList[4], "EXP:%4d", CurrExp);\
											sprintf((char *)tbaf_P_Scan_Setting_SelList[5], "AGC:%4d", CurrAgc)

#define TBAF_P_SUB_CONFIG(SEL)				TEST_CONF_SET;SMENU_Config((UINT8 *)tbaf_P_Main_SelList[SEL], 7, tbaf_P_Scan_Setting_SelList, 0, 0)
#define TBAF_P_SUB_REDRAW(SEL)				do { TBAF_P_SUB_CONFIG(SEL); TBAF_MENU_SHOW(); } while(0)

#define SCAN_RANGE_SET						sprintf((char *)focusscanlist[1], "Focus B/g Pos: %3d", fbgpos);\
											sprintf((char *)focusscanlist[2], "Focus End Pos: %3d", fendpos);\
											sprintf((char *)focusscanlist[3], "Start Scan: %3d--%3d", fbgpos, fendpos);\
											sprintf((char *)focusscanlist[4], "Start Pos %3d Only", ((fbgpos==fendpos)? fbgpos: (SINT16)appLensQuery(13)))



#define TBAF_P_FOCUSRANGE_CONFIG()			SCAN_RANGE_SET;SMENU_Config("Select Focus Scan Range", 6, focusscanlist, 0, 0)
#define TBAF_P_FOCUSRANGE_REDRAW()			do { TBAF_P_FOCUSRANGE_CONFIG(); TBAF_MENU_SHOW(); } while(0)

#if 0
#define TBAF_P_ISO_SET_CONFIG()				SMENU_Config(tbaf_P_ISO_Setting_SelList[AgcMenuIdx], 1, setlist, 0, 0)
#define TBAF_P_ISO_SET_REDRAW()				do { TBAF_P_ISO_SET_CONFIG(); TBAF_MENU_SHOW(); } while(0)
#define TBAF_P_ISO_SET_UPDATE()				SMENU_TitleUpdate(tbaf_P_ISO_Setting_SelList[AgcMenuIdx])
#endif
#define NOISE_BUF_RESET						Noise_Buf = Noise_Rec;memset(Noise_Buf, 0x00, 40960 * sizeof(Noise_Buf))
#define NOISE_BUF_GET						Noise_Rec + strlen(Noise_Rec)

#define AF_NOISE_LOG_ADD(fmt, args...)		printf(fmt, ##args);sprintf(NOISE_BUF_GET, fmt, ##args)

#define AF_NOISE_LOG_DUMP					sp5kFsFileWrite(Noise_Rec_fd, Noise_Rec, 40960 * sizeof(UINT8));NOISE_BUF_RESET
#define AF_NOISE_LOG_CLOSE					AF_NOISE_LOG_DUMP;sp5kFsFileClose(Noise_Rec_fd);sp5kFree(Noise_Buf)
/*-- Power on lens motor */
#define AF_NOSE_TEST_FOCUS_GO(pos)			lensMotorPowerOn();\
											lensLedPowerOn(LENS_LED_ID_FHM);\
											lensFocusJob(pos);\
											lensMotorPowerOff();\
											lensLedPowerOff(LENS_LED_ID_FHM)

/*-------------------------------------------------------------------------
 * Function : P_Main_Key_SET_Service
 *------------------------------------------------------------------------*/
static void P_Main_Key_SET_Service(UINT32 param)
{
	tbafSelect = TBAF_MENU_SELECTGET();

	switch (tbafSelect) {
	default:
		break;

	case TBAF_P_Main_SEL_CORVAL:
		TestItem = TBAF_P_Main_SEL_CORVAL;
		bgvalue = afcorval;
		break;
	case TBAF_P_Main_SEL_CORTHR:
		TestItem = TBAF_P_Main_SEL_CORTHR;
		bgvalue = afcorthr;
		break;
	case TBAF_P_Main_SEL_NOSCNT:
		TestItem = TBAF_P_Main_SEL_NOSCNT;
		bgvalue = afnosthr;
		break;
	case TBAF_P_Main_SEL_BACK:
		CLEAR_OSD_SRC;
		appTbAFStateExit(APP_STATE_TEST_MAIN, STATE_PARAM_NORMAL_INIT);
		return;
	}


	pfnKeyDir_Proc = NULL;
	pfnKeySet_Proc = P_Sub_Key_SET_Service;
	pfnKeyMenu_Proc = Key_MENU_Service;

	CLEAR_OSD_SRC;
	TBAF_P_SUB_REDRAW(tbafSelect);

}



/*-------------------------------------------------------------------------
 * Function : P_Sub_Key_SET_Service
 *------------------------------------------------------------------------*/
static void P_Sub_Key_SET_Service(UINT32 param)
{
	pfnKeyDir_Proc = P_Scan_Setting_Key_DIR_Service;
	pfnKeySet_Proc = P_Scan_Setting_Key_SET_Service;
	pfnKeyMenu_Proc = NULL;

	tbafSelect = TBAF_MENU_SELECTGET();
	switch (tbafSelect) {
	default:
		break;
	case TBAF_P_Sub_SEL_START:
		CLEAR_OSD_SRC;
		pfnKeyDir_Proc = NULL;
		pfnKeySet_Proc = P_StartTesting_Key_SET_Service;
		TBAF_P_FOCUSRANGE_REDRAW();
		return;
	case TBAF_P_Sub_SEL_BG_VALUE:
		sprintf(ScanSetStr,"Beginning Value: %3d", bgvalue);
		settingvalue = &bgvalue;
		break;
	case TBAF_P_Sub_SEL_END_VALUE:
		sprintf(ScanSetStr,"End Value: %3d", endvalue);
		settingvalue = &endvalue;
		break;
	case TBAF_P_Sub_SEL_INTERVAL:
		sprintf(ScanSetStr,"Interval: %3d", interval);
		settingvalue = &interval;
		break;
	case TBAF_P_Sub_SEL_EXP:
		CLEAR_OSD_SRC;
		sp5kPreviewExpAgcGet(&CurrExp, &CurrAgc);
		sprintf(ScanSetStr,"EXP:  %4d", CurrExp);
		pfnKeyDir_Proc = P_Exp_Setting_Key_DIR_Service;
		pfnKeySet_Proc = P_ExpAgc_Setting_Key_SET_Service;
		break;
	case TBAF_P_Sub_SEL_AGC:
		CLEAR_OSD_SRC;
		sp5kPreviewExpAgcGet(&CurrExp, &CurrAgc);
		sprintf(ScanSetStr,"AGC:  %4d", CurrAgc);
		pfnKeyDir_Proc = P_Agc_Setting_Key_DIR_Service;
		pfnKeySet_Proc = P_ExpAgc_Setting_Key_SET_Service;
		break;
	case TBAF_P_Sub_SEL_BACK:
		CLEAR_OSD_SRC;
		appTbAFStateExit(APP_STATE_TEST_AF, STATE_PARAM_NORMAL_INIT);
		return;
	}

	CLEAR_OSD_SRC;
	TBAF_P_VALUE_SET_REDRAW();
	TBAF_P_VALUE_SET_UPDATE();
}

/*-------------------------------------------------------------------------
 * Function : Key_MENU_Service
 *------------------------------------------------------------------------*/
static void Key_MENU_Service(UINT32 param)
{
	tbUtil_AeControl(1);
	CLEAR_OSD_SRC;
}
/*-------------------------------------------------------------------------
 * Function : P_Scan_Setting_Key_DIR_Service
 *------------------------------------------------------------------------*/
static void P_Scan_Setting_Key_DIR_Service(UINT32 msg)
{

	switch (msg) {
	default:
		break;
	case APP_KEY_PRESS_UP:
		*settingvalue +=((interval>5)&&(settingvalue==&bgvalue || settingvalue==&endvalue) ? interval:5);
		break;
	case APP_KEY_PRESS_DOWN:
		*settingvalue -=((interval>5)&&(settingvalue==&bgvalue || settingvalue==&endvalue) ? interval:5);
		break;
	case APP_KEY_PRESS_LEFT:
		*settingvalue -= 1;
		break;
	case APP_KEY_PRESS_RIGHT:
		*settingvalue += 1;
		break;
	}

	if((settingvalue==&fbgpos || settingvalue==&fendpos)){
		*settingvalue = (*settingvalue > 700 ? 0: *settingvalue);
		*settingvalue < 0 ? *settingvalue = 700: *settingvalue;
		fbgpos = (fbgpos > fendpos ? fendpos : fbgpos);
	}
	else{
		*settingvalue = (*settingvalue > 255 ? 0: *settingvalue);
		*settingvalue < 0 ? *settingvalue = 255: *settingvalue;
		bgvalue = (bgvalue > endvalue ? endvalue : bgvalue);
	}


	sprintf(&(ScanSetStr[strlen(ScanSetStr) - 3 ]),"%3d", *settingvalue);
	TBAF_P_VALUE_SET_UPDATE();
}

/*-------------------------------------------------------------------------
 * Function : P_Scan_Setting_Key_SET_Service
 *------------------------------------------------------------------------*/
static void P_Scan_Setting_Key_SET_Service(UINT32 param)
{
	tbafSelect = TBAF_MENU_SELECTGET();

	pfnKeyDir_Proc = NULL;
	pfnKeyMenu_Proc = Key_MENU_Service;

	switch (tbafSelect) {
	default:
		break;
	case TBAF_P_SET_SEL_SET:
		if(!wFocusScan){
			CLEAR_OSD_SRC;
			TBAF_P_SUB_REDRAW(TestItem);
			pfnKeySet_Proc = P_Sub_Key_SET_Service;
		}else{
			CLEAR_OSD_SRC;
			TBAF_P_FOCUSRANGE_REDRAW();
			pfnKeySet_Proc = P_StartTesting_Key_SET_Service;
		}
		break;
	}
}

/*-------------------------------------------------------------------------
 * Function : P_Exp_Setting_Key_DIR_Service
 *------------------------------------------------------------------------*/
static void P_Exp_Setting_Key_DIR_Service(UINT32 msg)
{

	switch (msg) {
	default:
		break;
	case APP_KEY_PRESS_UP:
		CurrExp += 8;
		break;
	case APP_KEY_PRESS_DOWN:
		CurrExp -= 8;
		break;
	case APP_KEY_PRESS_RIGHT:
		CurrExp += 1;
		break;

	case APP_KEY_PRESS_LEFT:
		CurrExp -= 1;
		break;
	}
	if(CurrExp > ExpLimit.expIdxMax){
		CurrExp = ExpLimit.expIdxMax;
	}
	if(CurrExp < ExpLimit.expIdxTblMin){
		CurrExp = ExpLimit.expIdxTblMin;
	}
	sprintf(&(ScanSetStr[strlen(ScanSetStr) - 4 ]),"%4d", CurrExp);
	TBAF_P_VALUE_SET_UPDATE();
}

/*-------------------------------------------------------------------------
 * Function : P_Agc_Setting_Key_DIR_Service
 *------------------------------------------------------------------------*/
static void P_Agc_Setting_Key_DIR_Service(UINT32 msg)
{

	switch (msg) {
	default:
		break;
	case APP_KEY_PRESS_UP:
		CurrAgc += 8;
		break;
	case APP_KEY_PRESS_DOWN:
		CurrAgc -= 8;
		break;
	case APP_KEY_PRESS_RIGHT:
		CurrAgc += 1;
		break;
	case APP_KEY_PRESS_LEFT:
		CurrAgc -= 1;
		break;
	}

	if(CurrAgc > AgcLimit.agcIdxMax){
		CurrAgc = AgcLimit.agcIdxMax;
	}
	if(CurrAgc < AgcLimit.agcIdxMin){
		CurrAgc = AgcLimit.agcIdxMin;
	}
	sprintf(&(ScanSetStr[strlen(ScanSetStr) - 4 ]),"%4d", CurrAgc);
	TBAF_P_VALUE_SET_UPDATE();
}


/*-------------------------------------------------------------------------
 * Function : P_ExpAgc_Setting_Key_SET_Service
 *------------------------------------------------------------------------*/
static void P_ExpAgc_Setting_Key_SET_Service(UINT32 param)
{
	tbafSelect = TBAF_MENU_SELECTGET();
	switch (tbafSelect) {
	default:
		break;
	case TBAF_P_SET_SEL_SET:
		sp5kPreviewExpAgcSet(CurrExp , CurrAgc);
		pfnKeySet_Proc = P_Sub_Key_SET_Service;
		pfnKeyDir_Proc = NULL;
		pfnKeyMenu_Proc = Key_MENU_Service;
		CLEAR_OSD_SRC;
		sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 200);	/*For sp5kPreviewExpAgcSet(CurrExp , CurrAgc);*/
		TBAF_P_SUB_REDRAW(TestItem);
		break;
	}
}
/*-------------------------------------------------------------------------
 * Function : P_StartTesting_Key_SET_Service
 		     Set focus scanning range.
 *------------------------------------------------------------------------*/
static void P_StartTesting_Key_SET_Service(UINT32 msg)
{
	tbafSelect = TBAF_MENU_SELECTGET();
	switch (tbafSelect) {
	default:
		break;

	case TBAF_P_StartTesting_SEL_FULL_fPOS:
		fbgpos = 0;
		fendpos = 700;
		AF_NOSE_TEST_FOCUS_GO(fbgpos);
		wFocusScan = TRUE;
		break;
	case TBAF_P_StartTesting_SEL_BEGIN_fPOS:
		wFocusScan = TRUE;
		pfnKeyDir_Proc = P_Scan_Setting_Key_DIR_Service;
		pfnKeySet_Proc = P_Scan_Setting_Key_SET_Service;
		pfnKeyMenu_Proc = NULL;
		sprintf(ScanSetStr,"Beginning Pos: %3d", fbgpos);
		settingvalue = &fbgpos;
		CLEAR_OSD_SRC;
		TBAF_P_VALUE_SET_REDRAW();
		TBAF_P_VALUE_SET_UPDATE();
		return;
	case TBAF_P_StartTesting_SEL_END_fPOS:
		wFocusScan = TRUE;
		pfnKeyDir_Proc = P_Scan_Setting_Key_DIR_Service;
		pfnKeySet_Proc = P_Scan_Setting_Key_SET_Service;
		pfnKeyMenu_Proc = NULL;
		sprintf(ScanSetStr,"End Pos: %3d", fendpos);
		settingvalue = &fendpos;
		CLEAR_OSD_SRC;
		TBAF_P_VALUE_SET_REDRAW();
		TBAF_P_VALUE_SET_UPDATE();
		return;

	case TBAF_P_StartTesting_SEL_SPECIED_fPOS:
		AF_NOSE_TEST_FOCUS_GO(fbgpos);
		wFocusScan = TRUE;
		break;
	case TBAF_P_StartTesting_SEL_CURRENT_fPOS:
		AF_NOSE_TEST_FOCUS_GO(fbgpos);
		wFocusScan = FALSE;
		break;
	case TBAF_P_StartTesting_SEL_BACK:
		wFocusScan = FALSE;
		pfnKeyDir_Proc = NULL;
		pfnKeySet_Proc = P_Sub_Key_SET_Service;
		pfnKeyMenu_Proc = Key_MENU_Service;
		CLEAR_OSD_SRC;
		TBAF_P_SUB_REDRAW(tbafSelect);
		return;
	}

	/*buffer and file ptr allocation*/

	if(Noise_Buf != NULL && Noise_Buf != Noise_Rec){
		NOISE_BUF_RESET;
	}else{
		Noise_Buf = sp5kMalloc(40960 * sizeof(Noise_Buf));
		memset(Noise_Buf, 0x00, 40960 * sizeof(Noise_Buf));
		Noise_Rec = Noise_Buf;
	}
	Noise_Rec_fd = sp5kFsFileOpen("D:\\NOISE___.TXT", SP5K_FS_OPEN_APPEND);
	if(!Noise_Rec_fd)
		Noise_Rec_fd = sp5kFsFileOpen("D:\\NOISE___.TXT", SP5K_FS_OPEN_CREATE);
	sp5kFsFileSeek(Noise_Rec_fd, 0, SP5K_FS_SEEK_END);
	/*buffer and file ptr allocation*/

	sp5kPreviewExpAgcGet(&CurrExp, &CurrAgc);
	//AF_NOISE_LOG_ADD("\n%s, Exp/Agc = %p, %p\n", tbaf_P_Main_SelList[TestItem], CurrExp, CurrAgc);/*Remove warning Lation@20121120*/

	CLEAR_OSD_SRC;
	currvalue = bgvalue;
	sp5kAfCfgSet(SP5K_AF_WIN_POS_TEST, 1);

	sp5kAfCfgSet(SP5K_AF_GAIN, 64);
	sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 200);

	//AF_NOISE_LOG_ADD("Current AF Gain = %d\n", (*(volatile UINT8 *)0xb0002402));/*Remove warning Lation@20121120*/
	sp5kAfModeSet(SP5K_AF_MODE_ON);
	sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 200);
	if(TestItem == TBAF_P_Main_SEL_CORVAL){
		sp5kAfCfgSet(SP5K_AF_NOISE_CORING, endvalue );
	}
	//AF_NOISE_LOG_ADD("Noise Coring Threshold = %3d\n",afcorthr);/*Remove warning Lation@20121120*/

	//AF_NOISE_LOG_ADD("Current Focus Position = %d\n", appLensQuery(13));/*Remove warning Lation@20121120*/
	CLEAR_OSD_SRC;

}

#if 0


/*-------------------------------------------------------------------------
 * Function : P_ISO_Setting_Key_DIR_Service
 *------------------------------------------------------------------------*/
static void P_ISO_Setting_Key_DIR_Service(UINT32 msg)
{

	UINT8 NumofList = sizeof(tbaf_P_ISO_Setting_SelList) / 4;
	switch (msg) {
	default:
		break;
	case APP_KEY_PRESS_UP:
	case APP_KEY_PRESS_RIGHT:
		AgcMenuIdx = (AgcMenuIdx+1)%(NumofList - 1);
		break;
	case APP_KEY_PRESS_DOWN:
	case APP_KEY_PRESS_LEFT:
		AgcMenuIdx = (AgcMenuIdx+(NumofList - 2))%(NumofList - 1);
		break;
	}

	TBAF_P_ISO_SET_UPDATE();
}
/*-------------------------------------------------------------------------
 * Function : P_ISO_Setting_Key_SET_Service
 *------------------------------------------------------------------------*/
static void P_ISO_Setting_Key_SET_Service(UINT32 param)
{
	tbafSelect = TBAF_MENU_SELECTGET();
	switch (tbafSelect) {
	default:
		break;
	case TBAF_P_SET_SEL_SET:
		TEST_CONF_SET;
		CLEAR_OSD_SRC;

		TBAF_P_SUB_REDRAW(TestItem);

		#if 0
		sp5kPreviewExpAgcSet(Exp_ISO100, Agc_ISO100 + 16 * AgcMenuIdx);
		#else
		sp5kPreviewExpAgcSet(Exp_ISO100 + 16 * AgcMenuIdx, Agc_ISO100 + 16 * AgcMenuIdx);
		#endif
		pfnKeySet_Proc = P_Sub_Key_SET_Service;
		pfnKeyDir_Proc = NULL;
		pfnKeyMenu_Proc = NULL;
		break;
	}
}
#endif
#if 0
static void testProcCorVal(UINT32 param)
{
	UINT32 afwin[9] = {0};
	int afresult = 0;

	if(currvalue <= endvalue)
	{
		if(AfConfSet == FAIL){
			AfConfSet = sp5kAfCfgSet(SP5K_AF_CORING_VALUE, currvalue);
			return;
		}
		afresult = sp5kAfWinGet(param, SP5K_AF_G, afwin);
		if(afresult != SUCCESS){
			printf("sp5kAfWinGet fail!\n");
			return;
		}
		AF_NOISE_LOG_ADD("Noise coring value = %3d, The Af value is %d\n",currvalue , (afwin[0] + afwin[1] + afwin[2] + afwin[3] + afwin[4] + afwin[5] + afwin[6] + afwin[7] + afwin[8]));

		currvalue += interval;
		AfConfSet = FAIL;
	}else{
		sp5kAfCfgSet(SP5K_AF_CORING_VALUE, bgvalue);
		sp5kAfModeSet(SP5K_AF_MODE_OFF);

		AF_NOISE_LOG_CLOSE;
		TEST_CONF_SET;

		pfnKeySet_Proc = P_Sub_Key_SET_Service;
		pfnKeyDir_Proc = NULL;
		pfnKeyMenu_Proc = NULL;

		CLEAR_OSD_SRC;
		TBAF_P_SUB_REDRAW(tbafSelect);
		sp5kAfCfgSet(SP5K_AF_WIN_POS_TEST, 0);
		return;
	}
}
static void testProcCorThr(UINT32 param)
{
	UINT32 afwin[9] = {0};
	int afresult = 0;

	if(currvalue <= endvalue)
	{
		if(AfConfSet == FAIL){
			AfConfSet = sp5kAfCfgSet(SP5K_AF_NOISE_CORING, currvalue );
			return;
		}
		afresult = sp5kAfWinGet(param, SP5K_AF_G, afwin);
		if(afresult != SUCCESS){
			printf("sp5kAfWinGet fail!\n");
			return;
		}
		AF_NOISE_LOG_ADD("Noise coring threshold = %3d, The Af value is %d\n",currvalue , (afwin[0] + afwin[1] + afwin[2] + afwin[3] + afwin[4] + afwin[5] + afwin[6] + afwin[7] + afwin[8]));
		currvalue += interval;
		AfConfSet = FAIL;
	}else{
		sp5kAfCfgSet(SP5K_AF_NOISE_CORING, bgvalue);
		sp5kAfModeSet(SP5K_AF_MODE_OFF);

		AF_NOISE_LOG_CLOSE;
		TEST_CONF_SET;

		pfnKeySet_Proc = P_Sub_Key_SET_Service;
		pfnKeyDir_Proc = NULL;
		pfnKeyMenu_Proc = NULL;

		CLEAR_OSD_SRC;
		TBAF_P_SUB_REDRAW(tbafSelect);
		sp5kAfCfgSet(SP5K_AF_WIN_POS_TEST, 0);
		return;
	}
}

static void testProcNosCnt(UINT32 param)
{
	UINT32 afwin[9] = {0};
	int afresult = 0;

	if(currvalue <= endvalue)
	{
		if(AfConfSet == FAIL){
			AfConfSet = sp5kAfCfgSet(SP5K_AF_NOISE_THR, currvalue );
			return;
		}
		afresult = sp5kAfWinGet(param, SP5K_AF_NOISE_CNT, afwin);
		if(afresult != SUCCESS){
			printf("sp5kAfWinGet fail!\n");
			return;
		}
		AF_NOISE_LOG_ADD("Noise threshold = %3d, The Af noise count is %d\n",currvalue , (afwin[0] + afwin[1] + afwin[2] + afwin[3] + afwin[4] + afwin[5] + afwin[6] + afwin[7] + afwin[8]));
		currvalue += interval;
		AfConfSet = FAIL;
	}else{
		sp5kAfCfgSet(SP5K_AF_NOISE_THR, bgvalue);
		sp5kAfModeSet(SP5K_AF_MODE_OFF);

		AF_NOISE_LOG_CLOSE;
		TEST_CONF_SET;

		pfnKeySet_Proc = P_Sub_Key_SET_Service;
		pfnKeyDir_Proc = NULL;
		pfnKeyMenu_Proc = NULL;

		CLEAR_OSD_SRC;
		TBAF_P_SUB_REDRAW(TestItem);
		sp5kAfCfgSet(SP5K_AF_WIN_POS_TEST, 0);
		return;
	}
}
#endif
/*-------------------------------------------------------------------------
 * Function : testProcFunction
 *------------------------------------------------------------------------*/
static void testProcFunction(UINT32 param)
{
	UINT32 afwin[9] = {0};
	int afresult = 0;

	if(!wFocusScan){
		if(currvalue <= endvalue)
		{
			if(AfConfSet == FAIL){
				AfConfSet = sp5kAfCfgSet(AfCfgSelector[TestItem], currvalue );
				return;
			}
			afresult = sp5kAfWinGet(param, (TestItem == TBAF_P_Main_SEL_NOSCNT ? SP5K_AF_NOISE_CNT: SP5K_AF_G), afwin);
			if(afresult != SUCCESS){
				printf("sp5kAfWinGet fail!\n");
				return;
			}
			//AF_NOISE_LOG_ADD("%3d, AF value get = %d\n",currvalue , (afwin[0] + afwin[1] + afwin[2] + afwin[3] + afwin[4] + afwin[5] + afwin[6] + afwin[7] + afwin[8]));v
			currvalue += interval;
			AfConfSet = FAIL;
		}else{
			sp5kAfCfgSet(AfCfgSelector[TestItem], bgvalue);
			sp5kAfModeSet(SP5K_AF_MODE_OFF);

			AF_NOISE_LOG_CLOSE;

			pfnKeySet_Proc = P_Sub_Key_SET_Service;
			pfnKeyDir_Proc = NULL;
			pfnKeyMenu_Proc = NULL;

			CLEAR_OSD_SRC;
			TBAF_P_SUB_REDRAW(TestItem);
			sp5kAfCfgSet(SP5K_AF_WIN_POS_TEST, 0);
			return;
		}
	}else{
			if(AfConfSet == FAIL &&  appLensQuery(13) == (UINT32)fbgpos){
				AfConfSet = sp5kAfCfgSet(AfCfgSelector[TestItem], currvalue );
				return;
			}
			afresult = sp5kAfWinGet(param, (TestItem == TBAF_P_Main_SEL_NOSCNT ? SP5K_AF_NOISE_CNT: SP5K_AF_G), afwin);
			if(afresult != SUCCESS){
				printf("sp5kAfWinGet fail!\n");
				return;
			}

			//AF_NOISE_LOG_ADD("\n(%3d, %3d, %3d)",appLensQuery(13), currvalue , (afwin[0] + afwin[1] + afwin[2] + afwin[3] + afwin[4] + afwin[5] + afwin[6] + afwin[7] + afwin[8]));/*Remove warning Lation@20121120*/

			if(appLensQuery(13) < (UINT32)fendpos){
				AF_NOSE_TEST_FOCUS_GO(appLensQuery(13) + 1);
				return;
			}else{
				if(currvalue + interval <= endvalue){
					currvalue += interval;
					AfConfSet = FAIL;
					if(appLensQuery(13) == (UINT32)fendpos){
						AF_NOISE_LOG_DUMP;
						AF_NOSE_TEST_FOCUS_GO(fbgpos);
					}
					return;
				}

				sp5kAfCfgSet(AfCfgSelector[TestItem], bgvalue);
				sp5kAfModeSet(SP5K_AF_MODE_OFF);

				AF_NOISE_LOG_CLOSE;
				TEST_CONF_SET;

				pfnKeySet_Proc = P_Sub_Key_SET_Service;
				pfnKeyDir_Proc = NULL;
				pfnKeyMenu_Proc = NULL;

				wFocusScan = FALSE;

				CLEAR_OSD_SRC;
				TBAF_P_SUB_REDRAW(TestItem);
				sp5kAfCfgSet(SP5K_AF_WIN_POS_TEST, 0);
				return;
			}

		}
}


/*-------------------------------------------------------------------------
 * Function : appTbAFStateInit
 *------------------------------------------------------------------------*/
static void appTbAFStateInit(void)
{

	CLEAR_OSD_SRC;

	/*-- Set View Config */
	tbUtil_ViewEnter();

	/*-- OB Subtraction */
	tbUtil_ObApply();

	/*-- AE OFF/AWB OFF */
	tbUtil_AeControl(0);
	tbUtil_AwbControl(0);

	/*-- Large IRIS */
	appIrisLargeSet();

	appFocusModeChange(2);//UI_FOCUS_INFINITY

	/*-- Create SUB-Menu */
	menuPosBackup = MENU_MARK_TESTBENCH | 0x0a0d0801; 
	menuProcCurrPosSet (menuPosBackup);
	menuProcDisp();


	/*-- Get aeCb data block */
	paeCb = appAeCbGet();
	sp5kAeExpLimitGet(SP5K_SENSOR_MODE_PREVIEW, &ExpLimit);
	sp5kAeAgcLimitGet(SP5K_SENSOR_MODE_PREVIEW, &AgcLimit);

	//sp5kPreviewExpAgcSet(Exp_ISO100, Agc_ISO100);
	AgcMenuIdx = 0;

	//printf("ExpLimit = %d~%d, AgcLimit = %d~%d, pvISO = %d\n", ExpLimit.expIdxTblMin,ExpLimit.expIdxMax, AgcLimit.agcIdxMin, AgcLimit.agcIdxMax, paeCb->pviso);

	pfnKeySet_Proc = P_Main_Key_SET_Service;
	pfnKeyDir_Proc = NULL;
	pfnKeyMenu_Proc = NULL;
	fbgpos = appLensQuery(13);

}
/*-------------------------------------------------------------------------
 * Function : appTbAFStateClose
 *------------------------------------------------------------------------*/
static void appTbAFStateClose(void)
{
	tbLogBuf_Destroy();

	/*-- Enable Strobe Power */
	appStrobePowerOff();
}

/*-------------------------------------------------------------------------
 * Function : appTbAFStateExit
 *------------------------------------------------------------------------*/
static void appTbAFStateExit(UINT32 nxtSt, UINT32 param)
{
	appTbAFStateClose();
	appStateChange(nxtSt, param);
}
/*-------------------------------------------------------------------------
 * Function : appTbAFStateOnMsg
 *------------------------------------------------------------------------*/
static void appTbAFStateOnMsg(UINT32 msg, UINT32 param)
{
	switch (msg) {
	default:
		break;
	case SP5K_MSG_AF_READY:
		testProcFunction(param);
		break;
	case SP5K_MSG_AWB_READY:
		/*appAwbPreviewAdjust(param);*/
		break;
	case SP5K_MSG_AE_READY:
		appAePreviewAdjust(param);
		AeRdyCnt ++;
		if(AeRdyCnt >= 5){
			tbUtil_AeControl(0);
			AeRdyCnt = 0;
			TBAF_P_SUB_REDRAW(TestItem);
		}
		break;

	}
}

/*-------------------------------------------------------------------------
 * Function : appTbAFStateOnKey
 *------------------------------------------------------------------------*/
static void appTbAFStateOnKey(UINT32 msg, UINT32 param)
{
	switch (msg) {
	/* zooming */
	case APP_KEY_PRESS_TELE:
		ZSeg = (ZSeg > 5 ? ZSeg : (ZSeg + 1));
		calibLensZoomSegGo(ZSeg);
		break;
	case APP_KEY_PRESS_WIDE:
		ZSeg = (ZSeg < 1 ? ZSeg : (ZSeg - 1));
		calibLensZoomSegGo(ZSeg);
		break;
	case APP_KEY_PRESS_S1:
		break;

	case APP_KEY_PRESS_UP:
		menuProcMoveUp();
		menuProcDisp();
		break;
	case APP_KEY_PRESS_DOWN:
		if(pfnKeyDir_Proc != NULL)
			(*pfnKeyDir_Proc)(msg);
		else
			menuProcMoveDown(); 
			menuProcDisp();
		break;
	case APP_KEY_PRESS_SET:
		beep_echo();
		(*pfnKeySet_Proc)(param);
		break;

	case APP_KEY_PRESS_MENU:
		beep_echo();
		if(pfnKeyMenu_Proc != NULL){
			(*pfnKeyMenu_Proc)(param);
		}
		break;

	case APP_KEY_PRESS_DEL:
		beep_echo();
		break;
	case APP_KEY_PRESS_LEFT:
	case APP_KEY_PRESS_RIGHT:
		if(pfnKeyDir_Proc != NULL)
			(*pfnKeyDir_Proc)(msg);
		break;
	default:
		break;
	}
}
/*-------------------------------------------------------------------------
 * Function : appTbAFState
 *------------------------------------------------------------------------*/
void appTbAFState(UINT32 msg, UINT32 param)
{
	switch (msg) {
	case APP_STATE_MSG_INIT:
		RPrintf("appTbAFState");
		appTbAFStateInit();
		break;
	case APP_STATE_MSG_CLOSE:
		appTbAFStateClose();
		appStateCloseDone();
		break;
	default:
		if (IS_APP_KEY_MSG(msg)) {
			appTbAFStateOnKey(msg,param);
		}
		else{
			appTbAFStateOnMsg(msg,param);
		}
		break;
	}

}



