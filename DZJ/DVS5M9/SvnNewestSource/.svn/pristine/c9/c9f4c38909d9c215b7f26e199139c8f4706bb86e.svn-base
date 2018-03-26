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
 /*

 This calibration Awb item provides three modes.
 1. Calibrate AWB rb gain for different samples but the same modle. (the same sensor)
 		In this mode we will capture the LV10 raw image and compute the rb gains.
 		Then compare with the referened LV10 rb gains, which could create by mode 3.

 2. Create AWBCTT.BIN's three color temperature wb rb gains.
 		This mode will creat and record the WB rb gains in three different color temperatures.
 		They are a light, cool white and d light. Our default AWB algorithm will according to
 		these CT to develop lots of area and boundaries.

 3. Create LV10 color temperature wb rb gain.
 		To record the LV10 rb gains as references in mode 1. It will be the four paramters of
 		AWBCTT.BIN.

 Normally we only need to do the mode 1 in calibration flow.
 The paramters in mode 2 and mode 3 will pre-built according to different sensors.

 */
#define HOST_DBG 1
#include "app_com_def.h"
#include "app_com_api.h"

#include "sp5k_global_api.h"
#include "sp5k_sensor_api.h"
#include "sp5k_capture_api.h"
#include "sp5k_cal_api.h"
#include "Sp5k_rsvblk_api.h"

#include "app_still.h"
#include "app_osd.h"
#include "app_osdfont.h"
#include "app_osd_api.h"
#include "app_simple_menu.h"
#include "app_view_param_def.h"
#include "app_calib_api.h"
#include "app_lens_api.h"
#include "app_awbalg_api.h"
#include "app_menu_tab.h"  
#include "app_menu_core.h"
#include "app_menu_disp.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define CALIB_AWB_SAVE_IMAGE 0
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
typedef enum {
	CALIB_AWB_MODE_MENU,
	CALIB_AWB_MODE_CTT,
	CALIB_AWB_MODE_LV10,
	CALIB_AWB_MODE_LV10_STANDARD,
	CALIB_AWB_MODE_TOTAL,
} calibAwbModeSelect_t;

typedef enum {
	CALIB_AWB_MENU_SEL_BACK,
	CALIB_AWB_MENU_SEL_CALIB_AWB,
	CALIB_AWB_MENU_SEL_CREAT_CTT,
	CALIB_AWB_MENU_SEL_CREAT_LV10_CT,
	CALIB_AWB_MENU_SEL_CREAT_LV10_STANDARD,
	CALIB_AWB_MENU_SEL_TOTAL,
} calibAwbMenuSelect_t;

UINT8 szCalibAwb_Title[]="AWB Calib";

UINT32 calibAwb_OptList[] = {
	[CALIB_AWB_MENU_SEL_BACK] = (UINT32)"Back",
	[CALIB_AWB_MENU_SEL_CALIB_AWB] = (UINT32)"Calib_Awb",
	[CALIB_AWB_MENU_SEL_CREAT_CTT] = (UINT32)"Creat_Ctt",
	[CALIB_AWB_MENU_SEL_CREAT_LV10_CT] = (UINT32)"Creat_Lv10_Calibration",
	[CALIB_AWB_MENU_SEL_CREAT_LV10_STANDARD] = (UINT32)"Creat_Lv10_Standard",
};
static UINT32 calRGain,calBGain;
//static UINT32 aRgain, aBgain, tl84Rgain, tl84Bgain, dRgain, dBgain, cwRgain, cwBgain, officeRgain, officeBgain;
static appCalibData_t *pCalData;
static UINT8* pOsdTextBuf;
static UINT16* pAwbCctBuf;
static UINT32 calibAwbMode = CALIB_AWB_MODE_MENU;
static UINT32 calibAwbModeStatus = 0;
extern UINT16 rgain_avg, bgain_avg;
SINT32 myExpIdx = EXP_1_32;
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern UINT32 menuPosBackup;  
extern UINT8 proc_awb_calib;
extern UINT32 appStillWbCb(UINT32 addr,UINT32 rawfmt,void* size);
extern void appGetRawAvgRGGB(UINT16 *r, UINT16 *gr, UINT16 *gb, UINT16 *b);
extern void SP_CaptureDefaultPostWbSampling(UINT32 addr, UINT32 RawFmt, void * RawInfo);
void
_Aechange(
	int step
)
{
#if 0
	if(updown) {
		myExpIdx--;
		if(myExpIdx<0) myExpIdx=0;
		sp5kPreviewExpAgcSet(myExpIdx ,ISO_100);
		sp5kStillCapExpAgcSet(TVIDX_TO_CAP_EXPIDX(myExpIdx),ISO_100);
	}
	else {
		myExpIdx++;
		if(myExpIdx>EXP_1_2048) myExpIdx=EXP_1_2048;

		sp5kPreviewExpAgcSet(myExpIdx ,ISO_100);
		sp5kStillCapExpAgcSet(TVIDX_TO_CAP_EXPIDX(myExpIdx),ISO_100);
	}
#else
	myExpIdx += step;
	if(myExpIdx>EXP_1_2048) myExpIdx=EXP_1_2048;
	if(myExpIdx<0) myExpIdx=0;

	/*s Modify by Aries 10/03/25*/
	#if 1
	if (pCalData->IsoSign  ){
		/*sp5kPreviewExpAgcSet(myExpIdx ,pCalData->PvIso);*/
		sp5kStillCapExpAgcSet(TVIDX_TO_CAP_EXPIDX(myExpIdx),pCalData->CapIso);
	}else{
		/*sp5kPreviewExpAgcSet(myExpIdx ,ISO_100);*/
		sp5kStillCapExpAgcSet(TVIDX_TO_CAP_EXPIDX(myExpIdx),ISO_100);
	}
	#else
	sp5kPreviewExpAgcSet(myExpIdx ,8);
	sp5kStillCapExpAgcSet(TVIDX_TO_CAP_EXPIDX(myExpIdx),8);
	#endif
	/*e Modify by Aries 10/03/25*/

#endif
	printf("expidx = %d\n", myExpIdx);
	sprintf(pOsdTextBuf,"expidx %d", myExpIdx);
	appOsdLib_TextDraw(0, 32*5, SP5K_GFX_ALIGN_TOP_LEFT, pOsdTextBuf);
}




/*-------------------------------------------------------------------------
 *  File Name :
 *------------------------------------------------------------------------*/
/*s Modify by Aries 12/02/23*/
void _DoCalibAwb_ObSet(UINT8 isObOn)
{
       appCalibData_t *pCalData;

	pCalData = appCalibDataGet();

	if (isObOn == TRUE){
		if(pCalData->IsoObSign || pCalData->ObSign || pCalData->pvOBSign) {
			if (pCalData->pvOBSign) {	/* PV OB*/
				printf("PVOB");
				sp5kIqBlockEnable(IQ_BLK_OB,1);
				sp5kIqBlockEnable(IQ_BLK_WB_OFS,1);
				sp5kIqCfgSet(SP5K_IQ_CFG_OB_MODE,SP5K_OB_MANUAL);
				sp5kIqCfgSet(SP5K_IQ_CFG_WB_OFS_R,-(pCalData->pvOB[0]));
				sp5kIqCfgSet(SP5K_IQ_CFG_WB_OFS_GR,-(pCalData->pvOB[1]));
				sp5kIqCfgSet(SP5K_IQ_CFG_WB_OFS_B,-(pCalData->pvOB[2]));
				sp5kIqCfgSet(SP5K_IQ_CFG_WB_OFS_GB,-(pCalData->pvOB[3]));
				printf("R=%d,Gr=%d,B=%d,Gb=%d\n",pCalData->pvOB[0],pCalData->pvOB[1],pCalData->pvOB[2],pCalData->pvOB[3]);
			}else if (pCalData->IsoObSign) {	/*ISO OB*/
				printf("ISOOB");
				sp5kIqBlockEnable(IQ_BLK_WB_OFS, 1);
				sp5kIqBlockEnable(IQ_BLK_OB,1);
				sp5kIqCfgSet(SP5K_IQ_CFG_OB_MODE,SP5K_OB_MANUAL);
				sp5kIqCfgSet(SP5K_IQ_CFG_OB_MANUAL_VAL,(UINT32)(pCalData->isoOb[1]));
			} else {        		/*OB*/
				printf("NORMAL OB");
				sp5kIqBlockEnable(IQ_BLK_WB_OFS, 1);
				sp5kIqBlockEnable(IQ_BLK_OB,1);
				sp5kIqCfgSet(SP5K_IQ_CFG_OB_MODE,SP5K_OB_MANUAL);
				sp5kIqCfgSet(SP5K_IQ_CFG_OB_MANUAL_VAL,(UINT32)(pCalData->Ob));
			}
		}else{
				printf("OB value = IcatchIQOB[%d]\n",100);
				sp5kIqBlockEnable(IQ_BLK_WB_OFS, 1);
				sp5kIqBlockEnable(IQ_BLK_OB,1);
				sp5kIqCfgSet(SP5K_IQ_CFG_OB_MODE,SP5K_OB_MANUAL);
                    		sp5kIqCfgSet(SP5K_IQ_CFG_OB_MANUAL_VAL,100);
		}

	}else{
		sp5kIqBlockEnable(SP5K_IQ_BLOCK_OB,0);
		sp5kIqBlockEnable(IQ_BLK_WB_OFS,0);

	}
}





void _DoCalibAwb_LSCSet(UINT8 islscOn)
{

	if (islscOn == TRUE){
		sp5kIqCfgSet(SP5K_IQ_CFG_LSC_GAIN, 100); 
		sp5kResourceFileLoad(SP5K_RES_LSC, RES_IQ_LSC);
		sp5kIqBlockEnable(SP5K_IQ_BLOCK_LSC,1);
	}else{
		sp5kIqBlockEnable(SP5K_IQ_BLOCK_LSC,0);

	}
}

/*e Modify by Aries 12/02/23*/

/*-------------------------------------------------------------------------
 *  File Name :
 *------------------------------------------------------------------------*/
static void
_StateInit()
{
	UINT32 mode;

	/* osd init */
	CLEAR_OSD_SRC;
	appOsd_GfxIconIDSet(SP5K_RES_OSD_ICON);
	appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
	
	menuPosBackup = MENU_MARK_WBCLBT | 0x0a060100; 
	menuProcCurrPosSet (menuPosBackup);
	menuProcDisp();


	/* state init */
	calibAwbMode = CALIB_AWB_MODE_MENU;
	pCalData = appCalibDataGet();
	pOsdTextBuf = (UINT8*)sp5kMalloc(512);
	pAwbCctBuf = (UINT16*)sp5kMalloc(256);
	appLensInit();
	appLensMShutCallbackRegister();
	lensMShutSnapCloseDelaySet(CALIB_AE_DEFAULT_SHUTTER_EARLY_1);

	/* config to still preview*/
	appStill_SetResolution(UI_PIC_SIZE_MAX);
	appStill_SetQuality(UI_QUALITY_FINE);
	sp5kModeSet(SP5K_MODE_STILL_PREVIEW);
	while(mode != SP5K_MODE_STILL_PREVIEW) {
		APP_SLEEP_MS(20);
		sp5kModeGet(&mode);
	}

	/*sp5kAwbModeSet(SP5K_AWB_MODE_OFF);
	sp5kAeModeSet(SP5K_AE_MODE_OFF);*/
	sp5kAwbParamSet(SP5K_AWB_MODE_SET, SP5K_AWB_FLOW_CALIB);

	if (pCalData->IsoSign  ){
		sp5kPreviewExpAgcSet(myExpIdx ,pCalData->PvIso);
		sp5kStillCapExpAgcSet(TVIDX_TO_CAP_EXPIDX(myExpIdx),pCalData->CapIso);
	}else{
		sp5kPreviewExpAgcSet(myExpIdx ,ISO_100);
		sp5kStillCapExpAgcSet(TVIDX_TO_CAP_EXPIDX(myExpIdx),ISO_100);
	}

  	sp5kImgModeSet(SP5K_MODE_STILL_PREVIEW);
	_DoCalibAwb_ObSet(TRUE);
	_DoCalibAwb_LSCSet(FALSE);
	sp5kImgModeSetDone(SP5K_MODE_STILL_PREVIEW);

#if CALIB_AWB_SAVE_IMAGE
	sp5kStillCapCfgSet(SP5K_CAPTURE_SHUTTER_SND_NAME, RES_WAV_SHUTTER);	
	sp5kStillCapAdvCfgSet(SP5K_CAPTURE_CONFIRM_STORE, 0);
	//sp5kStillCapAdvCfgSet(SP5K_CAPTURE_SAVE_RAW, 1);
#else
	sp5kStillCapAdvCfgSet(SP5K_CAPTURE_CONFIRM_STORE, 1);
#endif

}

/*-------------------------------------------------------------------------
 *  File Name :
 *------------------------------------------------------------------------*/
static void
_StateClose()
{
	if(pOsdTextBuf != NULL) {
		sp5kFree(pOsdTextBuf);
		pOsdTextBuf = NULL;
	}

	if(pAwbCctBuf != NULL) {
		sp5kFree(pAwbCctBuf);
		pAwbCctBuf = NULL;
	}
	sp5kAwbParamSet(SP5K_AWB_MODE_SET, SP5K_AWB_FLOW_AUTO);
}
/*-------------------------------------------------------------------------
 *  File Name :
 *------------------------------------------------------------------------*/
static void
_DoPreview()
{
#if 1
	UINT16 max=0;
	UINT16 r=0,g=0,b=0;
	int step=32;
	sp5kCaliPreviewRawCallbackSet(appCalibRawCb);
	sp5kCalibrationCfgSet( SP5K_CALIBRATION_PURE_RAW, 0);/*pureRaw:1*/
	while(1){
		sp5kCalibrationCfgSet(SP5K_CALIBRATION_CALLBACK, SP5K_CALIB_CB_PREVIEW_RAW);
		sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 500);
		//appHostMsgWait(SP5K_MSG_CALI_COMPLETE, NULL, 20000);
		appGetRawAvgRGB(&r,&g,&b);
		printf("rgb == %d,%d,%d\n",r,g,b);

		max = r;
		if(max<g) max = g;
		if(max<b) max = b;

		if(myExpIdx==0 || myExpIdx>EXP_1_2048) {
			break;
		}
		else if(max>210*256) {
			if(step<0) {
				step = -step;
				if(step!=1 && step!= -1) step = step/2;
			}
			_Aechange(step);
		}
		else if(max<150*256){
			if(step>0) {
				step = -step;
				if(step!=1 && step!= -1) step = step/2;
			}
			_Aechange(step);
		}
		else  {
			break;
		}
	}
	calRGain = g*1024/r;
	calBGain = g*1024/b;
#else
	sp5kAwbParamSet(SP5K_AWB_MODE_SET,SP5K_AWB_PROC_SNAP | SP5K_AWB_FLOW_CALIB);
	sp5kCaliPreviewRawCallbackSet(appStillWbCb);
	sp5kCalibrationCfgSet(SP5K_CALIBRATION_CALLBACK, SP5K_CALIB_CB_PREVIEW_RAW);
	appHostMsgWait(SP5K_MSG_CALI_COMPLETE, NULL, 20000);
#endif
	sp5kCalibrationCfgSet(SP5K_CALIBRATION_OFF, SP5K_CALIB_CB_PREVIEW_RAW);
	sp5kCaliPreviewRawCallbackSet(NULL);
}

#if 0  /*mark for clean warning*/
static void
_DoCapture()
{
	UINT32 max;
	UINT16 r=0,g=0,b=0;
	int step=32;
#if 0
	sp5kStillCapRawCallbackSet(appCalibRawCb);

	while(1) {
		/* capture */

		appStillUtil_CalibCaptureSet(1);

		appHostMsgWait(SP5K_MSG_CALI_COMPLETE, NULL, 20000);

		appGetRawAvgRGB(&r,&g,&b);
		printf("rgb == %d,%d,%d\n",r,g,b);

		max = r;
		if(max<g) max = g;
		if(max<b) max = b;

		if(myExpIdx==0 || myExpIdx>EXP_1_2048) {
			break;
		}
		else if(max>210*256) {
			printf("+",max,210*256);
			if(step<0) {
				step = -step;
				if(step!=1 && step!= -1) step = step/2;
			}
			_Aechange(step);
		}
		else if(max<150*256){
			if(step>0) {
				step = -step;
				if(step!=1 && step!= -1) step = step/2;
			}
			_Aechange(step);
		}
		else  {
			break;
		}
	}
	#endif
	sp5kAwbParamSet(SP5K_AWB_MODE_SET,SP5K_AWB_PROC_SNAP | SP5K_AWB_FLOW_CALIB);
	/*sp5kStillCapWbCallbackSet((fp_sp5kCallback_t)appStillWbCb);*/
	sp5kStillCapRawCallbackSet((fp_sp5kCallback_t)appStillWbCb);
	appStillUtil_CalibCaptureSet(1);
	appHostMsgWait(SP5K_MSG_CALI_COMPLETE, NULL, 20000);

	sp5kCalibrationCfgSet(SP5K_CALIBRATION_OFF, SP5K_CALIB_CB_PREVIEW_RAW);
	sp5kStillCapRawCallbackSet(NULL);
}
#endif

/////////////////////////////////////////////////////////////////////
static UINT32 _DoAwbCttRead(){
	if(ReadFile(RES_AWB_CTT, (UINT8*)pAwbCctBuf, 256) != SUCCESS) {
		appOsdLib_TextDraw(0,32,SP5K_GFX_ALIGN_TOP_LEFT, "Read AWBCTT.BIN fail.");
		return FALSE;
	}
	return TRUE;
}
static void _DoAwbCttCalibration(UINT32 idx){
	UINT32 rg,bg;
#if 0
	UINT64 rs=0,bs=0;
	UINT32 i,c=20;
	//_DoPreview();
	//sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 2500);
	//rg = appAWBALG_WBParamGet(WB_PARAM_GetPvRg, NULL, 0);
	//bg = appAWBALG_WBParamGet(WB_PARAM_GetPvBg, NULL, 0);
	appAWBALG_WBParamSet(WB_PARAM_Func, WB_FUNC_Calibration);
	sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 1000);
	for(i=0;i<c;i++){
		sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 40);
		rg = appAWBALG_WBParamGet(WB_PARAM_GetPvRg, NULL, 0);
		bg = appAWBALG_WBParamGet(WB_PARAM_GetPvBg, NULL, 0);
		printf("%u,%u\n",rg,bg);
		rs+=rg;
		bs+=bg;
	}
	rg = rs/c;
	bg = bs/c;
#else
	_DoPreview();
	rg = calRGain;
	bg = calBGain;
#endif
	printf("(%d %d)\n",rg,bg);
	/*_AwbCalibGainGet(&avgRgain, &avgBgain);*/
	sprintf(pOsdTextBuf,"(%d %d)\n",rg,bg);

	CLEAR_OSD_SRC;
	appOsdLib_TextDraw(0,32*3,SP5K_GFX_ALIGN_TOP_LEFT, "Press Set Key...");
	appOsdLib_TextDraw(0,32*4,SP5K_GFX_ALIGN_TOP_LEFT, pOsdTextBuf);

	pAwbCctBuf[10+idx] = rg;
	pAwbCctBuf[10+idx+1] = bg;
}
static UINT32 _DoAwbCttWrite(){
	pAwbCctBuf[10+10] = 'x';//write ctt ratio
	pAwbCctBuf[10+11] = 'y';
	pAwbCctBuf[10+12] = 1024;//write ctt ratio
	pAwbCctBuf[10+13] = 1024;
	if(WriteFile(RES_AWB_CTT, (UINT8*)pAwbCctBuf, 256)!= SUCCESS) {
		appOsdLib_TextDraw(0,32,SP5K_GFX_ALIGN_TOP_LEFT, "Write AWBCTT.BIN fail.");
		pCalData->WbSign = FALSE;
		return FALSE;
	}else{
		pCalData->WbSign = TRUE;
		appOsdLib_TextDraw(0,32,SP5K_GFX_ALIGN_TOP_LEFT, "Success!");
		return TRUE;
	}
}
static void _DoCreatCttCapture(){
	switch(calibAwbModeStatus){
	case 0:
		if(FALSE==_DoAwbCttRead()){calibAwbModeStatus = 0xfe;return;}
		memset(pAwbCctBuf+10,0,20);
		_DoAwbCttCalibration(calibAwbModeStatus);
		appOsdLib_TextDraw(0,32,SP5K_GFX_ALIGN_TOP_LEFT, "Please turn on the cw light.");
		appOsdLib_TextDraw(0,32*2,SP5K_GFX_ALIGN_TOP_LEFT, "And target to a gray paper.");
		break;
	case 2:
		_DoAwbCttCalibration(calibAwbModeStatus);
		appOsdLib_TextDraw(0,32,SP5K_GFX_ALIGN_TOP_LEFT, "Please turn on the Day light.");
		appOsdLib_TextDraw(0,32*2,SP5K_GFX_ALIGN_TOP_LEFT, "And target to a gray paper.");
		break;
	case 4:
		_DoAwbCttCalibration(calibAwbModeStatus);
		sprintf(pOsdTextBuf,"(%d %d)(%d %d)(%d %d)"
			,pAwbCctBuf[10+0],pAwbCctBuf[10+1]
			,pAwbCctBuf[10+2],pAwbCctBuf[10+3]
			,pAwbCctBuf[10+4],pAwbCctBuf[10+5]);
		appOsdLib_TextDraw(0,32*2,SP5K_GFX_ALIGN_TOP_LEFT, pOsdTextBuf);

		if(FALSE==_DoAwbCttWrite()){calibAwbModeStatus = 0xfe;return;}
		calibAwbModeStatus = 0xfe;
		return;
	}
	calibAwbModeStatus+=2;
}
static void _DoCreatLv10StandardCapture(){
	if(FALSE==_DoAwbCttRead()){calibAwbModeStatus = 0xfe;return;}
	_DoAwbCttCalibration(6);
	appOsdLib_TextDraw(0,32*2,SP5K_GFX_ALIGN_TOP_LEFT, "One Standard Light.");
	if(FALSE==_DoAwbCttWrite()){calibAwbModeStatus = 0xfe;return;}
	calibAwbModeStatus = 0xfe;
}
static void _DoCreatLv10CalibCapture(){
	if(FALSE==_DoAwbCttRead()){calibAwbModeStatus = 0xfe;return;}
	_DoAwbCttCalibration(8);
	appOsdLib_TextDraw(0,32*2,SP5K_GFX_ALIGN_TOP_LEFT, "One Calibration Light.");
	if(FALSE==_DoAwbCttWrite()){calibAwbModeStatus = 0xfe;return;}
	calibAwbModeStatus = 0xfe;
}
#if 0
static void _AwbCalibGainGet(UINT16 *rg,UINT16 *bg)
{
	/*sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 100);*/
	*rg = sp5kAwbParamGet(SP5K_AWB_AVG_RGAIN);
	*bg = sp5kAwbParamGet(SP5K_AWB_AVG_BGAIN);
	printf("gain(%u,%u)\n",*rg,*bg);
}
#endif
static void
_DoCalibAwb()
{
	//TODO: LV10 is save to other place but AWBCTT.BIN.
	//		Modify these after LV10 method ready.
#if 0
	UINT16 thisRgain, thisBgain, refRgain, refBgain;

	_DoCapture();

	CLEAR_OSD_SRC;
	/* This LV10 WB rb gain */
	thisRgain = pCalData->WbRgain;
	thisBgain = pCalData->WbBgain;
	appOsdLib_TextDraw(0,32,SP5K_GFX_ALIGN_TOP_LEFT, "(LV10) WB Gain");
	sprintf(pOsdTextBuf,"(This) r:%d, b:%d", thisRgain, thisBgain);
	appOsdLib_TextDraw(0,64,SP5K_GFX_ALIGN_TOP_LEFT, pOsdTextBuf);
	DBG_PRINT("%s\n", pOsdTextBuf);

	/* The reference LV10 WB rb gian */
	if(ReadFile(RES_AWB_CTT, (UINT8*)pAwbCctBuf, 80) != SUCCESS) {
		appOsdLib_TextDraw(0,165,SP5K_GFX_ALIGN_TOP_LEFT, "Read AWBCTT.BIN fail.");
	}
	else {
		if(pAwbCctBuf[8] == 0x40 && pAwbCctBuf[8] == 0x40) {
			appOsdLib_TextDraw(0,96,SP5K_GFX_ALIGN_TOP_LEFT, "(Refered) No Info");
		}
		else {
			refRgain = pAwbCctBuf[8] <<4;
			refBgain = pAwbCctBuf[9] <<4;
			sprintf(pOsdTextBuf,"(Refered) r:%d, b:%d", refRgain, refBgain);
			appOsdLib_TextDraw(0,96,SP5K_GFX_ALIGN_TOP_LEFT, pOsdTextBuf);
			DBG_PRINT("%s\n", pOsdTextBuf);
			pCalData->WbRgain = thisRgain - refRgain;
			pCalData->WbBgain = thisBgain - refBgain;
			pCalData->WbSign = 1;
		}
 	}

	APP_SLEEP_MS(5000);
#endif
	CLEAR_PIP_SRC;
	CLEAR_OSD_SRC;
	if(FALSE==_DoAwbCttRead()){calibAwbModeStatus = 0xfe;return;}
	appOsdLib_TextDraw(0,32*2,SP5K_GFX_ALIGN_TOP_LEFT, "AwbCtt.Bin");
	sprintf(pOsdTextBuf
			,"(%u,%u)(%u,%u)(%u,%u)\n"
			,pAwbCctBuf[10+0],pAwbCctBuf[10+1]
			,pAwbCctBuf[10+2],pAwbCctBuf[10+3]
			,pAwbCctBuf[10+4],pAwbCctBuf[10+5]);
	appOsdLib_TextDraw(0,32*4,SP5K_GFX_ALIGN_TOP_LEFT, pOsdTextBuf);
	sprintf(pOsdTextBuf
			,"(%u,%u)(%u,%u)\n"
			,pAwbCctBuf[10+6],pAwbCctBuf[10+7]
			,pAwbCctBuf[10+8],pAwbCctBuf[10+9]);
	appOsdLib_TextDraw(0,32*5,SP5K_GFX_ALIGN_TOP_LEFT, pOsdTextBuf);
	calibAwbModeStatus = 0xfe;
}

/*-------------------------------------------------------------------------
 *  File Name :
 *------------------------------------------------------------------------*/
static void
_DoCreatCttStart()
{
	CLEAR_PIP_SRC;
	CLEAR_OSD_SRC;
	appOsdLib_TextDraw(0,32,SP5K_GFX_ALIGN_TOP_LEFT, "3 Light Calibration ");
	appOsdLib_TextDraw(0,64,SP5K_GFX_ALIGN_TOP_LEFT, "Please turn on the A light.");
	appOsdLib_TextDraw(0,96,SP5K_GFX_ALIGN_TOP_LEFT, "And target to a gray paper.");
	appOsdLib_TextDraw(0,128,SP5K_GFX_ALIGN_TOP_LEFT, "Then Press Set Key.");
	calibAwbMode = CALIB_AWB_MODE_CTT;
	calibAwbModeStatus = 0;
}

#if 0
static void
_DoCreatCttCapture()
{
#if 0
	UINT16 c1,c2,c3,c4;
	appCalibData_t *pCalib;
	extern UINT8* awbInfoName;
	UINT8 names[][15]={"AWBINF0A.JPG","AWBINFCW.JPG","AWBINF0D.JPG"};
	pCalib = appCalibDataGet();

	/*s Add by Aries 10/03/25*/
	#if SPCA5310
 	 sp5kImgModeSet(SP5K_MODE_STILL_SNAP);
	#endif
	tbUtil_ObApply();
	#if SPCA5310
	sp5kImgModeSetDone(SP5K_MODE_STILL_SNAP);
	#endif
	/*e Add by Aries 10/03/25*/
	/*awbInfoName = names[calibAwbModeStatus];*/
	#if 1
	_DoCapture();
	#else
	_DoPreview();
	#endif


	UINT16 avgRgain = sp5kAwbParamGet(SP5K_AWB_AVG_RGAIN);
	UINT16 avgBgain = sp5kAwbParamGet(SP5K_AWB_AVG_BGAIN);
	/*appGetRawAvgRGGB(&c1,&c2,&c3,&c4);
	sprintf(pOsdTextBuf,"%d %d %d %d-%d %d-%d %d", c1>>8,c2>>8,c3>>8,c4>>8,(c2+c3)/2*1024/c1,(c2+c3)/2*1024/c4,avgRgain,avgBgain);
	*/
#endif
	UINT16 avgRgain,avgBgain;
	UINT16 c1,c2,c3,c4;

	_AwbCalibGainGet(&avgRgain, &avgBgain);
	printf("avgRgain: %d,avgBgain:%d\n",avgRgain,avgBgain);



	_DoPreview();
	appGetRawAvgRGGB(&c1,&c2,&c3,&c4);
	printf("R: %d,RG:%d,GB:%d,B:%d\n",c1,c2,c3,c4);

	avgRgain=(c2+c3)/2*1024/c1;
	avgBgain=(c2+c3)/2*1024/c4;
	/*_AwbCalibGainGet(&avgRgain, &avgBgain);*/
	sprintf(pOsdTextBuf,"%d %d",avgRgain,avgBgain);

	CLEAR_OSD_SRC;
	appOsdLib_TextDraw(0,32*2,SP5K_GFX_ALIGN_TOP_LEFT, "And target to a gray paper.");
	appOsdLib_TextDraw(0,32*3,SP5K_GFX_ALIGN_TOP_LEFT, "Then Press Set Key.");
	appOsdLib_TextDraw(0,32*4,SP5K_GFX_ALIGN_TOP_LEFT, pOsdTextBuf);
	calibAwbModeStatus++;

	/* record gains */
	switch(calibAwbModeStatus){
	case 1:
		aRgain = avgRgain;//(c2+c3)/2*1024/c1;
		aBgain = avgBgain;//(c2+c3)/2*1024/c4;
		appOsdLib_TextDraw(0,32,SP5K_GFX_ALIGN_TOP_LEFT, "Please turn on the cw light.");
		break;
	case 2:
		cwRgain = avgRgain;//(c2+c3)/2*1024/c1;
		cwBgain =avgBgain;//(c2+c3)/2*1024/c4;
		appOsdLib_TextDraw(0,32,SP5K_GFX_ALIGN_TOP_LEFT, "Please turn on the Day light.");
		break;
	case 3:
		dRgain =avgRgain;//(c2+c3)/2*1024/c1;
		dBgain = avgBgain;//(c2+c3)/2*1024/c4;
		calibAwbModeStatus=0xff;
		//officeRgain = (c2+c3)/2*1024/c1;
		//officeBgain = (c2+c3)/2*1024/c4;
		CLEAR_OSD_SRC;
		sprintf(pOsdTextBuf," a wb gain(r:%d,b:%d)", aRgain, aBgain);
		appOsdLib_TextDraw(0,32,SP5K_GFX_ALIGN_TOP_LEFT, pOsdTextBuf);
		printf("%s\n", pOsdTextBuf);
		sprintf(pOsdTextBuf,"cw wb gain(r:%d,b:%d)", cwRgain, cwBgain);
		appOsdLib_TextDraw(0,32*2,SP5K_GFX_ALIGN_TOP_LEFT, pOsdTextBuf);
		printf("%s\n", pOsdTextBuf);
		sprintf(pOsdTextBuf," d wb gain(r:%d,b:%d)", dRgain, dBgain);
		appOsdLib_TextDraw(0,32*3,SP5K_GFX_ALIGN_TOP_LEFT, pOsdTextBuf);
		printf("%s\n", pOsdTextBuf);
		appOsdLib_TextDraw(0,32*6,SP5K_GFX_ALIGN_TOP_LEFT, "Press Set Key to Back.");
		pCalData->WbSign = FALSE;
		if(ReadFile(RES_AWB_CTT, (UINT8*)pAwbCctBuf, 256) != SUCCESS) {
			appOsdLib_TextDraw(0,165,SP5K_GFX_ALIGN_TOP_LEFT, "Read AWBCTT.BIN fail.");
		}
		else {
			/*s Modify by Aries 09/08/18*/
			awb_res_header_t *pheader=(awb_res_header_t*)pAwbCctBuf;
			pAwbCctBuf[(pheader->idxCTC)/2+2] = aRgain>>4;
			pAwbCctBuf[(pheader->idxCTC)/2+3] = aBgain>>4;
			pAwbCctBuf[(pheader->idxCTC)/2+4] = cwRgain>>4;
			pAwbCctBuf[(pheader->idxCTC)/2+5] = cwBgain>>4;
			pAwbCctBuf[(pheader->idxCTC)/2+6] = dRgain>>4;
			pAwbCctBuf[(pheader->idxCTC)/2+7] = dBgain>>4;
			pAwbCctBuf[(pheader->idxCTC)/2+8] = 0;
			pAwbCctBuf[(pheader->idxCTC)/2+9] = 0;
			pAwbCctBuf[(pheader->idxCTC)/2+10] = 0;
			pAwbCctBuf[(pheader->idxCTC)/2+11] = 0;
			/*pCalData->WbRgain = 0;
			pCalData->WbBgain = 0;
			sp5kAwbParamSet(SP5K_AWB_CALIBRATION_RGAIN,0);
			sp5kAwbParamSet(SP5K_AWB_CALIBRATION_BGAIN,0);*/
			/*e Modify by Aries 09/08/18*/
			if(WriteFile(RES_AWB_CTT, (UINT8*)pAwbCctBuf, 256)!= SUCCESS) {
				appOsdLib_TextDraw(0,200,SP5K_GFX_ALIGN_TOP_LEFT, "Write AWBCTT.BIN fail.");
			}
			else
			{
				pCalData->WbSign = TRUE;
			}
		}
		break;
	}

}

#endif
static void
_DoCreatLv10CalibStart()
{
	CLEAR_PIP_SRC;
	CLEAR_OSD_SRC;
	appOsdLib_TextDraw(0,32,SP5K_GFX_ALIGN_TOP_LEFT, "One Light Calibration ");
	appOsdLib_TextDraw(0,64,SP5K_GFX_ALIGN_TOP_LEFT, "Turn on the Calibration light.");
	appOsdLib_TextDraw(0,96,SP5K_GFX_ALIGN_TOP_LEFT, "And target to a gray paper.");
	appOsdLib_TextDraw(0,128,SP5K_GFX_ALIGN_TOP_LEFT, "Then Press Set Key.");
	calibAwbMode = CALIB_AWB_MODE_LV10;
	calibAwbModeStatus = 0;
}

#if 0
static void
_DoCreatLv10CalibCapture()
{
#if 0
	UINT16 c1,c2,c3,c4;
	UINT16 v1,v2,v3,v4;

	calibAwbModeStatus=0xff;

	/*s Add by Aries 10/03/25*/
	#if SPCA5310
 	 sp5kImgModeSet(SP5K_MODE_STILL_SNAP);
	#endif
	tbUtil_ObApply();
	#if SPCA5310
	sp5kImgModeSetDone(SP5K_MODE_STILL_SNAP);
	#endif
	/*e Add by Aries 10/03/25*/
	_DoCapture();
	appGetRawAvgRGGB(&c1,&c2,&c3,&c4);
	printf("val(%d,%d,%d,%d)\n",c1,c2,c3,c4);
	if(c1 && c2 && c3 && c4)
	{
		Lv10Rgain = rgain_avg;//(UINT32)(c2+c3)/2*1024/c1;
		Lv10Bgain = bgain_avg;//(UINT32)(c2+c3)/2*1024/c4;
	}
	/*
	_DoPreview();
	appGetRawAvgRGGB(&v1,&v2,&v3,&v4);
	*/
#endif
#if 0
	UINT16 avgRgain,avgBgain;
	_AwbCalibGainGet(&avgRgain, &avgBgain);
	printf(pOsdTextBuf,"%d %d",avgRgain,avgBgain);

	Lv10Rgain = avgRgain;
	Lv10Bgain = avgBgain;
	/* osd */
	CLEAR_OSD_SRC;
	sprintf(pOsdTextBuf,"WB Gain (LV10) r:%d, b:%d", Lv10Rgain, Lv10Bgain);
	appOsdLib_TextDraw(0,32,SP5K_GFX_ALIGN_TOP_LEFT, pOsdTextBuf);
	appOsdLib_TextDraw(0,32*2,SP5K_GFX_ALIGN_TOP_LEFT, "Press Set Key to Back.");

	/* show the channel value */
	/*sprintf(pOsdTextBuf,"cap %d %d %d %d", c1>>8,c2>>8,c3>>8,c4>>8);
	appOsdLib_TextDraw(0,32*3,SP5K_GFX_ALIGN_TOP_LEFT, pOsdTextBuf);*/
	/*
	sprintf(pOsdTextBuf,"view %d %d %d %d", v1>>8,v2>>8,v3>>8,v4>>8);
	appOsdLib_TextDraw(0,32*4,SP5K_GFX_ALIGN_TOP_LEFT, pOsdTextBuf);
	*/

	/*
	// Don't save LV10 to AWBCTT.BIN.
	// TODO: find other bin file to save LV10
	if(ReadFile(CALIB_AWB_DATA_FILENAME, (UINT8*)pAwbCctBuf, 80) != SUCCESS) {
		appOsdLib_TextDraw(0,165,SP5K_GFX_ALIGN_TOP_LEFT, "Read AWBCTT.BIN fail.");
	}
	else {
		pAwbCctBuf[8] = Lv10Rgain>>4;
		pAwbCctBuf[9] = Lv10Bgain>>4;
		if(WriteFile(CALIB_AWB_DATA_FILENAME, (UINT8*)pAwbCctBuf, 80)!= SUCCESS) {
			appOsdLib_TextDraw(0,200,SP5K_GFX_ALIGN_TOP_LEFT, "Write AWBCTT.BIN fail.");
		}
 	}
 	*/
 	pCalData->WbSign=TRUE;
 	pCalData->WbRgain = Lv10Rgain>>4;
	pCalData->WbBgain = Lv10Bgain>>4;
	/*set calib gain to awb */
  /*s Modify by Aries 09/08/18*/
	sp5kAwbParamSet(SP5K_AWB_CALIBRATION_RGAIN,Lv10Rgain);
	sp5kAwbParamSet(SP5K_AWB_CALIBRATION_BGAIN,Lv10Bgain);
  /*e Modify by Aries 09/08/18*/

#endif
	UINT16 rg,bg;
	_DoCalibration(&rg, &bg);
	calibAwbModeStatus=0xff;
	pCalData->WbSign=FALSE;
	if(ReadFile(RES_AWB_CTT, (UINT8*)pAwbCctBuf, 256) != SUCCESS) {
		appOsdLib_TextDraw(0,32,SP5K_GFX_ALIGN_TOP_LEFT, "Read AWBCTT.BIN fail.");
	}else {
		awb_res_header_t *pheader=(awb_res_header_t*)pAwbCctBuf;
		if(!pAwbCctBuf[(pheader->idxCTC)/2+8] || !pAwbCctBuf[(pheader->idxCTC)/2+9]){
			appOsdLib_TextDraw(0,32,SP5K_GFX_ALIGN_TOP_LEFT, "Standard Data Error\n");
		}else{			
			pAwbCctBuf[(pheader->idxCTC)/2+10] = rg;
			pAwbCctBuf[(pheader->idxCTC)/2+11] = bg;
			/*pCalData->WbRgain = 0;
			pCalData->WbBgain = 0;
			sp5kAwbParamSet(SP5K_AWB_CALIBRATION_RGAIN,0);
			sp5kAwbParamSet(SP5K_AWB_CALIBRATION_BGAIN,0);*/
			/*e Modify by Aries 09/08/18*/
			if(WriteFile(RES_AWB_CTT, (UINT8*)pAwbCctBuf, 256)!= SUCCESS) {
				appOsdLib_TextDraw(0,32,SP5K_GFX_ALIGN_TOP_LEFT, "Write AWBCTT.BIN fail.");
			}
			else
			{
				pCalData->WbSign = TRUE;
				appOsdLib_TextDraw(0,pos+96,SP5K_GFX_ALIGN_TOP_LEFT, "OK!! Press Set Key to Back.");
			}
		}
	}



}
#endif

static void
_DoCreatLv10StandardStart()
{
	CLEAR_PIP_SRC;
	CLEAR_OSD_SRC;
	appOsdLib_TextDraw(0,32,SP5K_GFX_ALIGN_TOP_LEFT, "One Light Standard ");
	appOsdLib_TextDraw(0,64,SP5K_GFX_ALIGN_TOP_LEFT, "Turn on the Calibration light.");
	appOsdLib_TextDraw(0,96,SP5K_GFX_ALIGN_TOP_LEFT, "And target to a gray paper.");
	appOsdLib_TextDraw(0,128,SP5K_GFX_ALIGN_TOP_LEFT, "Then Press Set Key.");
	calibAwbMode = CALIB_AWB_MODE_LV10_STANDARD;
	calibAwbModeStatus = 0;
}

#if 0
static void
_DoCreatLv10StandardCapture()
{
#if 0
	UINT16 c1,c2,c3,c4;
	/*UINT16 v1,v2,v3,v4;*/

	calibAwbModeStatus=0xff;

	/*s Add by Aries 10/03/25*/
	#if SPCA5310
 	 sp5kImgModeSet(SP5K_MODE_STILL_SNAP);
	#endif
	tbUtil_ObApply();
	#if SPCA5310
	sp5kImgModeSetDone(SP5K_MODE_STILL_SNAP);
	#endif
	/*e Add by Aries 10/03/25*/
	_DoCapture();
	appGetRawAvgRGGB(&c1,&c2,&c3,&c4);
	Lv10Rgain = rgain_avg;//(UINT32)(c2+c3)/2*1024/c1;
	Lv10Bgain = bgain_avg;//(UINT32)(c2+c3)/2*1024/c4;

	/*
	_DoPreview();
	appGetRawAvgRGGB(&v1,&v2,&v3,&v4);
	*/
#endif
	UINT16 avgRgain,avgBgain;
	_AwbCalibGainGet(&avgRgain, &avgBgain);
	sprintf(pOsdTextBuf,"%d %d",avgRgain,avgBgain);

	/* osd */
	CLEAR_OSD_SRC;
	sprintf(pOsdTextBuf,"WB Gain (LV10) r:%d, b:%d", avgRgain, avgBgain);
	appOsdLib_TextDraw(0,32,SP5K_GFX_ALIGN_TOP_LEFT, pOsdTextBuf);
	appOsdLib_TextDraw(0,32*2,SP5K_GFX_ALIGN_TOP_LEFT, "Press Set Key to Back.");

	/* show the channel value */
	/*sprintf(pOsdTextBuf,"cap %d %d %d %d", c1>>8,c2>>8,c3>>8,c4>>8);
	appOsdLib_TextDraw(0,32*3,SP5K_GFX_ALIGN_TOP_LEFT, pOsdTextBuf);*/
	/*
	sprintf(pOsdTextBuf,"view %d %d %d %d", v1>>8,v2>>8,v3>>8,v4>>8);
	appOsdLib_TextDraw(0,32*4,SP5K_GFX_ALIGN_TOP_LEFT, pOsdTextBuf);
	*/

	/* Don't save LV10 to AWBCTT.BIN.*/
	/* TODO: find other bin file to save LV10*/
	if(ReadFile(RES_AWB_CTT, (UINT8*)pAwbCctBuf, 256) != SUCCESS) {
		appOsdLib_TextDraw(0,165,SP5K_GFX_ALIGN_TOP_LEFT, "Read AWBCTT.BIN fail.");
	}	
	else {
		/*s Modify by Aries 09/08/18*/
		awb_res_header_t *pheader=(awb_res_header_t*)pAwbCctBuf;
		pAwbCctBuf[(pheader->idxCTC)/2+8] = avgRgain>>4; 
		pAwbCctBuf[(pheader->idxCTC)/2+9] = avgBgain>>4;	
		/*e Modify by Aries 09/08/18*/
		if(WriteFile(RES_AWB_CTT, (UINT8*)pAwbCctBuf, 256)!= SUCCESS) {
			appOsdLib_TextDraw(0,200,SP5K_GFX_ALIGN_TOP_LEFT, "Write AWBCTT.BIN fail.");
		}
 	}


}
#endif
static void
_Key_SET_Service(
	UINT32 action
)
{
	switch(action) {
	case CALIB_AWB_MENU_SEL_BACK:
		_StateClose();
		appStateChange(APP_STATE_CALIBRATION, STATE_PARAM_NORMAL_INIT);
		break;
	case CALIB_AWB_MENU_SEL_CALIB_AWB:
		_DoCalibAwb();
		break;
	case CALIB_AWB_MENU_SEL_CREAT_CTT:
		_DoCreatCttStart();
		break;
	case CALIB_AWB_MENU_SEL_CREAT_LV10_CT:
		_DoCreatLv10CalibStart();
		break;
	case CALIB_AWB_MENU_SEL_CREAT_LV10_STANDARD:
		_DoCreatLv10StandardStart();
		break;
	}
}

/*-------------------------------------------------------------------------
 *  File Name :
 *------------------------------------------------------------------------*/
 #if 0
static void
_ShowModeInfo(
	UINT32 action
)
{
	appOsd_ColorDrawTextColorSet(PAL_NOCOLOR, PAL_NOCOLOR, PAL_NOCOLOR, PAL_BLEND_0);
	appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, 0, 0, 320, 32, 0, 0);

	appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
	switch(action) {
	case CALIB_AWB_MENU_SEL_BACK:
		break;
	case CALIB_AWB_MENU_SEL_CALIB_AWB:
		appOsdLib_TextDraw(0,0,SP5K_GFX_ALIGN_TOP_LEFT, "LV10 On, Key Set to Con't");
		break;
	case CALIB_AWB_MENU_SEL_CREAT_CTT:
		appOsdLib_TextDraw(0,0,SP5K_GFX_ALIGN_TOP_LEFT, "Key Set to Con't");
		break;
	case CALIB_AWB_MENU_SEL_CREAT_LV10_CT:
		appOsdLib_TextDraw(0,0,SP5K_GFX_ALIGN_TOP_LEFT, "Key Set to Con't");
		break;
	}


}
#endif
/*-------------------------------------------------------------------------
 *  File Name :
 *------------------------------------------------------------------------*/
static void
_StateOnMsg(
	UINT32 msg,
	UINT32 param
)
{
	switch (msg) {
	case SP5K_MSG_PREFLASH_READY:
		break;
	case SP5K_MSG_AE_READY:
		appAePreviewAdjust(param);
		break;
	case SP5K_MSG_AWB_READY:
		break;
	default:
		break;
	}
}

/*-------------------------------------------------------------------------
 *  File Name :
 *------------------------------------------------------------------------*/
static void
_StateOnKey(
	UINT32 msg,
	UINT32 param
)
{
	switch(calibAwbModeStatus){
	case 0xff:
		calibAwbMode = CALIB_AWB_MODE_MENU;
		calibAwbModeStatus = 0;
		CLEAR_OSD_SRC;
		SMENU_Show();
		return;
	case 0xfe:
		appOsdLib_TextDraw(0,32*3,SP5K_GFX_ALIGN_TOP_LEFT, "Press Set Key...");
		calibAwbModeStatus = 0xff;
		return;
	}
	switch(calibAwbMode) {
	case CALIB_AWB_MODE_MENU:
		switch (msg) {
		case APP_KEY_PRESS_UP:
			menuProcMoveUp();
			menuProcDisp();
			break;
		case APP_KEY_PRESS_DOWN:
			APP_OSD_REFRESH_OFF;
			menuProcMoveDown(); 
		    menuProcDisp();
//			SMENU_ActiveBarChange();
			APP_OSD_REFRESH_ON;
			break;

		case APP_KEY_PRESS_S2:
		case APP_KEY_PRESS_SET:
			_Key_SET_Service(SMENU_SelectGet());
			break;
		default:
			break;
		}
		break;

	case CALIB_AWB_MODE_CTT:
		switch (msg) {

		case APP_KEY_PRESS_S2:
		case APP_KEY_PRESS_SET:
			_DoCreatCttCapture();
			break;
		}
		break;

	case CALIB_AWB_MODE_LV10:
		switch (msg) {

		case APP_KEY_PRESS_S2:
		case APP_KEY_PRESS_SET:
			_DoCreatLv10CalibCapture();
			//appCalibDataSave();

			break;
		}
		break;

	case CALIB_AWB_MODE_LV10_STANDARD:
		switch (msg) {

		case APP_KEY_PRESS_S2:
		case APP_KEY_PRESS_SET:
			_DoCreatLv10StandardCapture();
			break;
		}
		break;
	}
}

/*-------------------------------------------------------------------------
 *  File Name :
 *------------------------------------------------------------------------*/
void
appCalibAwbState(
	UINT32 msg,
	UINT32 param
)
{
	switch (msg) {
	case APP_STATE_MSG_INIT:
		_StateInit();
		break;
	case APP_STATE_MSG_CLOSE:
		_StateClose();
		appStateCloseDone();
		break;
	default:
		if (IS_APP_KEY_MSG(msg)) {
			_StateOnKey(msg,param);
		}
		else{
			_StateOnMsg(msg,param);
		}
		break;
	}
}

