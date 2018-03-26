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
#include "sp5k_ae_api.h"
#include "sp5k_capture_api.h"
#include "sp5k_cdsp_api.h"
#include "sp5k_cal_api.h"

#include "app_sound.h"
#include "app_calib_api.h"
#include "app_simple_menu.h"
#include "app_aaa.h"
#include "app_lens_api.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

#define TBFLASH_TMP_BUF_LENGTH		512
#define LCD_SIZE_X							320                                                                                                                                                       
#define LCD_SIZE_Y							240                                                                                                                                                       

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
static UINT32 *plogBufData=NULL, *glogBufPtr=NULL;

typedef struct {
	UINT16 rgain;                 
	UINT16 grgain;                
	UINT16 bgain;                 
	UINT16 gbgain;                
}WbGain_t;          
typedef struct {
	SINT16 roffset;               
	SINT16 groffset;              
	SINT16 boffset;               
	SINT16 gboffset;              
}WbOffset_t;          


/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
void calibLensZoomSegGo(UINT32 zmSeg);
UINT32 appCalibRawCb(UINT32 addr, UINT32 fmt, void *imgSize);
void sioCtrl(UINT32 opt);
UINT32 appGetRawMaxChannelAvg(void);
UINT32 appAePreviewAdjust(UINT32 param);
UINT8 appAeFastAdjust(UINT32 param);
void appAeCaptureSet(UINT8 imgId);
UINT8 appAeStableGet(void);
UINT16 appAeLumaGet(void);
UINT32 lensIrisSegPosQuery(void);
void appAeStableReset(void);
void lensMShutPowerOn(void);
void lensMShutPowerOff(void);
UINT32 appMShutClose(void);
UINT32 appMShutOpen(void);
UINT32 appIrisLargeSet(void);
UINT32 appIrisSmallSet(void);
void appGetRawAvgRGGB(UINT16 *r, UINT16 *gr, UINT16 *gb, UINT16 *b);
extern SINT32 appAEALGLib_ResultGet(UINT32 param, aeResult_t *presult);
extern UINT32 appBtnMsgToKey(UINT32 msgId,UINT32 msgParam);
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
static UINT32 fdDbgLogFile=0;


/*s Modify by Aries 10/02/04*/
#if (_CAM_A900_D3)||(_CAM_CATHY_EVM)
 #if KIT_SHUT_CTRL_TIMER
extern UINT32 CapExpTime;
extern UINT32 CapShutColseTime;
#endif
#endif
/*e Modify by Aries 10/02/04*/

extern UINT8 calPVOB;
/*-------------------------------------------------------------------------
 * Function : tbLogBuf_Create
 *------------------------------------------------------------------------*/
void tbLogBuf_Create(void)
{
	if (glogBufPtr) {
		printf("\n\n");
		printf(">>>>>>>> tbLogBuf exists !!!! \n");
		printf("\n\n");
		return;
	}

	glogBufPtr = (UINT32 *)sp5kMalloc(TBFLASH_TMP_BUF_LENGTH*sizeof(UINT32));
	if (glogBufPtr==NULL) {
		printf("\n\n");
		printf(">>>>>>>> tbLogBuf alloc failed !!!! \n");
		printf("\n\n");
		return;
	}
	plogBufData = glogBufPtr;
}
/*-------------------------------------------------------------------------
 * Function : tbLogBuf_Create
 *------------------------------------------------------------------------*/
void tbLogBuf_Destroy(void)
{
	if (glogBufPtr) {
		sp5kFree(glogBufPtr);
	}
	glogBufPtr=NULL;
}
/*-------------------------------------------------------------------------
 * Function : tbLogFile_Print
 *------------------------------------------------------------------------*/
UINT32 tbLogFile_FdGet(void)
{
	return fdDbgLogFile;
}

/*-------------------------------------------------------------------------
 * Function : tbLogFile_Print
 *------------------------------------------------------------------------*/
void tbLogFile_Print(const char *format, ...)
{
 	char buf[256];
	va_list	ap;

	if (fdDbgLogFile==0) {
		printf("\nNULL LogFile !!\n\n");
		return;
	}

	memset(buf, 0, 256);
	va_start(ap, format);
	vsprintf(buf,format,ap);
	va_end(ap);

	sp5kFsFileWrite(fdDbgLogFile, buf, strlen(buf));
	printf("%s", buf);
}
/*-------------------------------------------------------------------------
 * Function : tbLogFile_PrintEx
 *------------------------------------------------------------------------*/
void tbLogFile_PrintEx(BOOL isWrFile, const char *format, ...)
{
 	char buf[256];
	va_list	ap;

	memset(buf, 0, 256);
	va_start(ap, format);
	vsprintf(buf,format,ap);
	va_end(ap);

	if (isWrFile) {
		tbLogFile_Print(buf);
	} else {
		printf(buf);
	}
}
/*-------------------------------------------------------------------------
 * Function : tbLogFile_Open
 *------------------------------------------------------------------------*/
UINT8 tbLogFile_Open(UINT8 *szLogFname)
{
	if (!szLogFname) {
		printf("\nIllegal filename !!\n\n");
		return 0;
	}

	sp5kFsFileDelete(szLogFname);
	APP_SLEEP_MS(1000);

	if (fdDbgLogFile) {
		sp5kFsFileClose(fdDbgLogFile);
		fdDbgLogFile = 0;
		APP_SLEEP_MS(1000);
	}

	fdDbgLogFile = sp5kFsFileOpen(szLogFname, SP5K_FS_OPEN_CREATE|SP5K_FS_OPEN_RDWR);
	if (fdDbgLogFile==0) {
		printf("\nFILE Open failed !!\n\n");
		CLEAR_OSD_SRC;
		dbgOsd_Print(4, "FOpen Err");
		APP_SLEEP_MS(2000);
		return 0;
	}

	printf("**********************************************\n");
	printf("File %s opened !! \n", szLogFname);
	printf("**********************************************\n");
	return 1;
}
/*-------------------------------------------------------------------------
 * Function : tbLogFile_Print
 *------------------------------------------------------------------------*/
void tbLogFile_Close(void)
{
	if (fdDbgLogFile) {
		tbLogFile_Print("\n= End Of Log =\n\n");
		sp5kFsFileClose(fdDbgLogFile);
		fdDbgLogFile = 0;
	}
	fdDbgLogFile = 0;
}

/*-------------------------------------------------------------------------
 * Function : tbLogBuf_Clear
 *------------------------------------------------------------------------*/
void tbLogBuf_Clear(void)
{
	if (glogBufPtr==NULL) {
		printf("\n\n !!!!!!!!!!! %s !!!!!!!!!! \n\n", __FUNCTION__);
		return;
	}

	/*-- Clean Data Buf */
	int i;
	plogBufData = glogBufPtr;
	for (i=0; i<TBFLASH_TMP_BUF_LENGTH ; i++, plogBufData++) {
		*plogBufData = 0;
	}
	plogBufData = glogBufPtr;
}
/*-------------------------------------------------------------------------
 * Function : tbLogBuf_Add
 *------------------------------------------------------------------------*/
void tbLogBuf_Add(UINT32 val)
{
	if(glogBufPtr== NULL)	tbLogBuf_Create();
	*plogBufData = val; plogBufData++;
}
/*-------------------------------------------------------------------------
 * Function : tbLogBuf_Get
 *------------------------------------------------------------------------*/
UINT32 tbLogBuf_Get(void)
{
	UINT32 tmp;
	tmp = *plogBufData; plogBufData++;
	return tmp;
}
/*-------------------------------------------------------------------------
 * Function : tbLogBuf_PtrReset
 *------------------------------------------------------------------------*/
void tbLogBuf_PtrReset(void)
{
	plogBufData = glogBufPtr;
}

/*-------------------------------------------------------------------------
 * Function : tbUtil_TestProlog
 *------------------------------------------------------------------------*/
UINT8 tbUtil_TestProlog(UINT8 *pszTest)
{
	/*- Confirm Testbed setup */
	printf("\n*********************\n");
	printf("%s \n", pszTest);
	printf("*********************\n\n");
	SMENU_YesNoConfig(pszTest);
	SMENU_YesNoShow();
	if (SMENU_SEL_NO==SMENU_SelScan()) {
		return 0;
	}
	SMENU_YesNoClear();
	return 1;
}
/*-------------------------------------------------------------------------
 * Function : tbUtil_Beep
 *------------------------------------------------------------------------*/
void tbUtil_Beep(void)
{
	appBeep(TRUE, TRUE);
}
/*-------------------------------------------------------------------------
 * Function : tbUtil_BeepWarn
 *------------------------------------------------------------------------*/
void tbUtil_BeepWarn(void)
{
	appPreloadSoundPlay(SOUND_ID_WARNING);
}
/*-------------------------------------------------------------------------
 * Function : tbUtil_Beep
 *------------------------------------------------------------------------*/
void tbUtil_BeepOK(void)
{
	appPreloadSoundPlay(SOUND_ID_FOCUSOK);
}
/*-------------------------------------------------------------------------
 * Function : tbUtil_PrintfCtrl
 *------------------------------------------------------------------------*/
void tbUtil_PrintfCtrl(BOOL isOn)
{
	//printf("\n!!!sioCtrl(%d)!!!\n\n",isOn);	
	if (isOn) sioCtrl(0);  /* turn printf on */
	else sioCtrl(1);  /* turn printf off */
}
/*-------------------------------------------------------------------------
 * Function : tbUtil_QueryIrisStop
 *------------------------------------------------------------------------*/
UINT8 tbUtil_QueryIrisStop(void)
{
	#if KIT_WITH_IRIS
	/*-- 1:Small, 0:Large */
	return lensIrisSegPosQuery();
	#else
	return 0;
	#endif
}
/*-------------------------------------------------------------------------
 * Function : tbUtil_CalibCdspSet
 *------------------------------------------------------------------------*/
void tbUtil_CalibCdspSet(BOOL isObOn)
{
	WbOffset_t wb;
	sp5kWbGain_t wbgain;


	#if SPCA5330

	if (isObOn) sp5kIqBlockEnable(IQ_BLK_OB, 1);
	else sp5kIqBlockEnable(IQ_BLK_OB, 0);

	sp5kIqBlockEnable(IQ_BLK_BP, 0);
	sp5kIqBlockEnable(IQ_BLK_LSC, 0);

	/*-- Init W.B. Offset & Gain */
	wb.boffset = wb.gboffset = wb.groffset = wb.roffset = 0;
	wbgain.bgain = wbgain.gbgain = wbgain.grgain = wbgain.rgain = 0x40;
	sp5kIqCfgSet(SP5K_IQ_CFG_WB_OFS_R,wb.roffset);
	sp5kIqCfgSet(SP5K_IQ_CFG_WB_OFS_GR,wb.groffset);
	sp5kIqCfgSet(SP5K_IQ_CFG_WB_OFS_B,wb.boffset);
	sp5kIqCfgSet(SP5K_IQ_CFG_WB_OFS_GB,wb.gboffset);

	
	sp5kImgWbGainSet(&wbgain);

	sp5kIqCfgSet(SP5K_IQ_CFG_WB_GAIN_R_GAIN,wbgain.rgain );
	sp5kIqCfgSet(SP5K_IQ_CFG_WB_GAIN_GR_GAIN,wbgain.grgain);
	sp5kIqCfgSet(SP5K_IQ_CFG_WB_GAIN_B_GAIN,wbgain.bgain);
	sp5kIqCfgSet(SP5K_IQ_CFG_WB_GAIN_GB_GAIN,wbgain.gbgain);


	sp5kIqBlockEnable(SP5K_IQ_BLOCK_WB_OFS, 0); 
	sp5kIqBlockEnable(SP5K_IQ_BLOCK_WB_GAIN, 0); 

	sp5kIqCfgSet(SP5K_IQ_CFG_WB_GAIN_GLOBAL_GAIN, 0x100); 


	#else

	if (isObOn) sp5kImgCdspBlockEnable(SP5K_CDSP_OB, 1);
	else sp5kImgCdspBlockEnable(SP5K_CDSP_OB, 0);

	sp5kImgCdspBlockEnable(SP5K_CDSP_BP, 0);
	sp5kImgCdspBlockEnable(SP5K_CDSP_LSC, 0);

	/*-- Init W.B. Offset & Gain */
	wb.boffset = wb.gboffset = wb.groffset = wb.roffset = 0;
	wbgain.bgain = wbgain.gbgain = wbgain.grgain = wbgain.rgain = 0x40;
	sp5kImgWbOffsetSet(&(wb));
	sp5kImgWbGainSet(&wbgain);
	sp5kImgCdspBlockEnable(SP5K_CDSP_WB_OFFSET, 0);
	sp5kImgCdspBlockEnable(SP5K_CDSP_WB_GAIN, 0);

	sp5kImgCdspBlockEnable(SP5K_CDSP_GLOBAL_GAIN, 0);
	#endif
}
/*-------------------------------------------------------------------------
 * Function : tbUtil_MShutterCtrl
 *------------------------------------------------------------------------*/
void tbUtil_MShutterCtrl(BOOL isClose)
{
	lensMShutPowerOn();
	if (isClose) {
		printf("... Close MShutter ...\n");
		while(FAIL==appMShutClose()) {
			APP_SLEEP_MS(100);
		}
	} else {
		printf("... Open MShutter ...\n");
		while(FAIL==appMShutOpen()) {
			APP_SLEEP_MS(100);
		}
	}
	APP_SLEEP_MS(100);
	lensMShutPowerOff();
}
/*-------------------------------------------------------------------------
 * Function : tbUtil_IrisCtrl
 *------------------------------------------------------------------------*/
void tbUtil_IrisCtrl(BOOL isLarge)
{
	if (isLarge) {
		printf("... Large IRIS ...\n");
		while(FAIL==appIrisLargeSet()) {
			APP_SLEEP_MS(100);
		}
	} else {
		printf("... Small IRIS ...\n");
		while(FAIL==appIrisSmallSet()) {
			APP_SLEEP_MS(100);
		}
	}
	APP_SLEEP_MS(100);
}
/*-------------------------------------------------------------------------
 * Function : tbUtil_QuickSweepHostMessage
 *------------------------------------------------------------------------*/
void tbUtil_QuickSweepHostMessage(UINT32 wait_ms)
{
	UINT32 osTime, tmrID;
	UINT32 msg, param;

	/*-- Register a TIMER to terminate quick sweep process */
	tmrID = appTimerSet(10, "qSweep");

	printf("\n+++++++++++++++ Sweeping Host MSG \n");
	osTime = sp5kOsTimeGet();

	tbUtil_PrintfCtrl(0); /* turn printf off */
	while (sp5kOsTimeGet()-osTime < wait_ms/10)
	{
		sp5kHostMsgReceive(&msg, &param);
	}
	appTimerClear(&tmrID);	
	printf("++ Sweeping Host MSG : DONE !!\n");
	tbUtil_PrintfCtrl(1); /* turn printf on*/
}
/*-------------------------------------------------------------------------
 * Function : tbUtil_GetAeResult
 *------------------------------------------------------------------------*/
UINT16 tbUtil_GetAeResult(sp5kAeResult_t *paeResult)
{
	UINT32 osTime;
	UINT32 msg, param, ret;

	tbUtil_QuickSweepHostMessage(100);

	osTime = appMsTimeGet();
	do {
		sp5kHostMsgReceive(&msg, &param);
		if (msg==SP5K_MSG_AE_READY) {

		#if SP5K_ALGORITHM_AE //lester 20120402
			ret = appAEALGLib_ResultGet(param,paeResult);
		#else
			ret = sp5kAePreviewResultGet(param, paeResult);
		#endif

			if (ret==SUCCESS && (paeResult->valid & AAA_INFO_RESULT_AE_VALID)) {
				ret = paeResult->aeLuma;
				break;
			}
		}

		if(500<(appMsTimeGet()-osTime)) {
			ret = 0;
			break;
		}
	} while(1);

	return ret;
}

/*-------------------------------------------------------------------------
 * Function : tbUtil_RunAe
 *------------------------------------------------------------------------*/
UINT16 tbUtil_RunAe(void)
{
	UINT32 osTime;
	UINT16 aeLuma;
	UINT32 msg, param;

	osTime = appMsTimeGet();
	aeLuma = 0;
	appAeStableReset();
	do {
		sp5kHostMsgReceive(&msg, &param);
		if (msg==SP5K_MSG_AE_READY) {
			//printf("Do ae adj \n");
			appAePreviewAdjust(param);
		}

		#if 1
		if (appAeStableGet()==TRUE && appMsTimeGet()-osTime>1000) {
			aeLuma = appAeLumaGet();
			break;
		} else {
			aeLuma = 0;
		}
		#endif
		/*-- AE TimeOut Check !! */
		if (appMsTimeGet()-osTime>2000) {
			printf("!!!!!!!!!!!!!!!!!!!\n");
			printf("AE Conv Failed !!\n");
			printf("!!!!!!!!!!!!!!!!!!!\n");
			CLEAR_OSD_SRC;
			//beep_warn();
			dbgOsd_Print(3, "AE TimeOut");
			dbgOsd_Print(4, "AE TimeOut");
			dbgOsd_Print(5, "AE TimeOut");
			tbUtil_QuickSweepHostMessage(1000);
			CLEAR_OSD_SRC;
			break;
		}
	} while(1);
	return aeLuma;
}
/*-------------------------------------------------------------------------
 * Function : tbUtil_RunFastAe
 *------------------------------------------------------------------------*/
void tbUtil_RunFastAe(void)
{
	UINT32 osTime;
	UINT32 msg, param;
	UINT8 fastAeRet;

	osTime = appMsTimeGet();
	do {
		sp5kHostMsgReceive(&msg, &param);
		if (msg==SP5K_MSG_AE_READY) {
			fastAeRet = appAeFastAdjust(param);
			if (fastAeRet==0xFF) continue;
			else break;
		}

		/*-- AE TimeOut Check !! */
		if (appMsTimeGet()-osTime>3000) {
			printf("FAST AE Conv Failed !!\n");
			//CLEAR_OSD_SRC;
			//beep_warn();
			//dbgOsd_Print(3, "AE TimeOut");
			//dbgOsd_Print(4, "AE TimeOut");
			//dbgOsd_Print(5, "AE TimeOut");
			//APP_SLEEP_MS(1000);
			//CLEAR_OSD_SRC;
			break;
		}
	} while(1);
	return;
}
/*-------------------------------------------------------------------------
 * Function : tbUtil_AeCaptureSet
 *------------------------------------------------------------------------*/
void tbUtil_AeCaptureSet(void)
{
	appAeCaptureSet(0);
}

/*-------------------------------------------------------------------------
 * Function : tbUtil_ViewSet
 *------------------------------------------------------------------------*/
void tbUtil_ViewSet(void)
{
	UINT32 spMode;

	printf("%s \n", __FUNCTION__);

	tbUtil_PrintfCtrl(0);
	sp5kModeGet(&spMode);
	if (spMode==SP5K_MODE_STILL_PREVIEW) goto _exit;

	sp5kModeSet(SP5K_MODE_STILL_PREVIEW);
	do {
		sp5kModeGet(&spMode);
		if(spMode==SP5K_MODE_STILL_PREVIEW) break;
		else APP_SLEEP_MS(300);
	} while(1);
_exit:
	APP_SLEEP_MS(60);
	tbUtil_PrintfCtrl(1);
}
/*-------------------------------------------------------------------------
 * Function : tbUtil_CalibAeWeightCfg
 *------------------------------------------------------------------------*/
void tbUtil_CalibAeWeightCfg(UINT32 region)
{
	UINT32 i,j;
	UINT32 startWin, aeWinRegion = region;
	UINT8 pweight[] = {15,15,2,  //hgrid, vgrid, ngroup
	                               1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 1*/
	                               1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 2*/
	                               1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 3*/
	                               1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 4*/
	                               1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 5*/
	                               1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 6*/
	                               1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 7*/
	                               1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 8*/
	                               1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 9*/
	                               1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 10*/
	                               1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 11*/
	                               1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 12*/
	                               1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 13*/
	                               1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 14*/
	                               1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 15*/
	                               0, 1 }; /*weight of group*/

	if(region<1 || region>pweight[0]) aeWinRegion = 5;
	startWin = (pweight[0]-aeWinRegion)/2;
	for(i=startWin; i<(startWin + aeWinRegion); i++) {
		for(j=startWin; j<(startWin + aeWinRegion); j++){
			pweight[i*pweight[0] + j + 3] = 0;
		}
	}			
	sp5kAeWeightCfgSet(pweight);

	#if 1
	printf("Calib AE Weight CFG..\n");
	for(i=0; i<15; i++) {
		for(j=0; j<15; j++) {
			printf("%d  ",pweight[i*pweight[0] + j + 3] );
		}
		printf("\n");
	}
	#endif
}
/*-------------------------------------------------------------------------
 * Function : tbUtil_ViewEnter
 *------------------------------------------------------------------------*/
BOOL tbutil_view_enter_sel = 1;
void tbUtil_ViewEnter(void)
{
	extern void appStillPvStateInit(UINT32 preState);
	extern UINT32 appPreviousStateGet (void);
	extern void appStill_SetMetering(UINT8 idx);

	CLEAR_OSD_SRC;
	//appStill_SetMetering(1);
	if (tbutil_view_enter_sel==1) {
		printf("%s: PvStateInit ... \n", __FUNCTION__);
		tbUtil_PrintfCtrl(0);
		appStillPvStateInit(APP_STATE_POWER_ON);
		APP_SLEEP_MS(100);
	} else {
		printf("%s: ViewSet ... \n", __FUNCTION__);
		tbUtil_ViewSet();
	}
	/* AE weighting config */
	tbUtil_CalibAeWeightCfg(5); 

	/* ISO AUTO set*/
	appAeCapIsoSet(0);
	
	tbUtil_PrintfCtrl(1);
	CLEAR_OSD_SRC;
}
/*-------------------------------------------------------------------------
 * Function : tbUtil_AeAwb_Dark_Off
 *------------------------------------------------------------------------*/
void tbUtil_AeAwb_Dark_Off(void)
{
	/*-- AE/AWB Off */
	sp5kAeModeSet(SP5K_AE_MODE_OFF);
	sp5kAwbModeSet(SP5K_AWB_MODE_OFF);
	sp5kStillCapAdvCfgSet(SP5K_CAPTURE_DARK_FRAME, 0);
}
/*-------------------------------------------------------------------------
 * Function : tbUtil_AeControl
 *------------------------------------------------------------------------*/
void tbUtil_AeControl (BOOL isOn)
{
	/*-- AE */
	if (isOn) sp5kAeModeSet(SP5K_AE_MODE_INFO_ONLY);
	else sp5kAeModeSet(SP5K_AE_MODE_OFF);
}
/*-------------------------------------------------------------------------
 * Function : tbUtil_AwbControl
 *------------------------------------------------------------------------*/
void tbUtil_AwbControl(BOOL isOn)
{
	/*-- AWB */
	if (isOn) sp5kAwbModeSet(SP5K_AWB_MODE_INFO_ONLY);
	else sp5kAwbModeSet(SP5K_AWB_MODE_OFF);
}
/*-------------------------------------------------------------------------
 * Function : tbUtil_ObApply
 *------------------------------------------------------------------------*/
void tbUtil_ObApply(void)
{
#if 1
#if SPCA5330
	 appCalibData_t *pCalib;
	 pCalib = appCalibDataGet();

	 if(pCalib->IsoObSign || pCalib->ObSign || pCalib->pvOBSign) {
		 if (pCalib->pvOBSign) { /* PV OB*/
			 //YPrintf("PVOB");
			 sp5kIqBlockEnable(IQ_BLK_OB,1);
			 sp5kIqBlockEnable(IQ_BLK_WB_OFS,1);
			 sp5kIqCfgSet(SP5K_IQ_CFG_WB_OFS_R,-(pCalib->pvOB[0]));
			 sp5kIqCfgSet(SP5K_IQ_CFG_WB_OFS_GR,-(pCalib->pvOB[1]));
			 sp5kIqCfgSet(SP5K_IQ_CFG_WB_OFS_B,-(pCalib->pvOB[2]));
			 sp5kIqCfgSet(SP5K_IQ_CFG_WB_OFS_GB,-(pCalib->pvOB[3]));
			 //printf("R=%d,Gr=%d,B=%d,Gb=%d\n",pCalib->pvOB[0],pCalib->pvOB[1],pCalib->pvOB[2],pCalib->pvOB[3]);
		 }	
		 else if (pCalib->IsoObSign) {	 /*ISO OB*/  
			 //YPrintf("ISOOB");
			 sp5kIqBlockEnable(IQ_BLK_WB_OFS, 1);
			 sp5kIqBlockEnable(IQ_BLK_OB,1);
			 sp5kIqCfgSet(SP5K_IQ_CFG_OB_MODE,0);/*ISO 100*/
			 sp5kIqCfgSet(SP5K_IQ_CFG_OB_MANUAL_VAL,(UINT32)(pCalib->isoOb[1]));
		 } 
		 else { 			 /*OB*/
			 //YPrintf("NORMAL OB");
			 sp5kIqBlockEnable(IQ_BLK_WB_OFS, 1);
			 sp5kIqBlockEnable(IQ_BLK_OB,1);
			 sp5kIqCfgSet(SP5K_IQ_CFG_OB_MODE,0);/*ISO 100*/
			 sp5kIqCfgSet(SP5K_IQ_CFG_OB_MANUAL_VAL,(UINT32)(pCalib->Ob));
		 }
	 }
	 else {
		 sp5kIqBlockEnable(IQ_BLK_WB_OFS, 1);
		 sp5kIqBlockEnable(IQ_BLK_OB,1);
		 sp5kIqCfgSet(SP5K_IQ_CFG_OB_MODE,0);/*ISO 100*/
		 sp5kIqCfgSet(SP5K_IQ_CFG_OB_MANUAL_VAL,CALIB_DEFAULT_OB);/*ISO 100*/
	 } 
#else
	 appCalibData_t *pcalib;
	 pcalib = appCalibDataGet();
	 sp5kImgCdspBlockEnable(SP5K_CDSP_OB, 1);
	 sp5kImgObModeCfgSet(SP5K_OB_MANUAL);
	 sp5kImgObSet(pcalib->Ob);
	 printf("%s: Apply OB (%d) \n", __FUNCTION__, pcalib->Ob);
#endif
#endif
}
/*-------------------------------------------------------------------------
 * Function : tbUtil_ViewExpLimtGet
 *------------------------------------------------------------------------*/
void tbUtil_ViewExpLimtGet(UINT32 *pmax, UINT32 *pmin)
{
	/*-- VIEW : Retrieve EXP limitation */
	sp5kExpLimit_t expLimit;
	sp5kAeExpLimitGet(SP5K_SENSOR_MODE_PREVIEW, &expLimit);
	*pmax = expLimit.expIdxMax;
	*pmin = expLimit.expIdxTblMin;
	printf("\n----- SNAP Gain Table Limitation ---------------\n");
	printf("expIdx MAX : %d \n", *pmax);
	printf("expIdx MIN : %d \n", *pmin);
	printf("\n\n");
}
/*-------------------------------------------------------------------------
 * Function : tbUtil_SnapExpLimtGet
 *------------------------------------------------------------------------*/
void tbUtil_SnapExpLimtGet(UINT32 *pmax, UINT32 *pmin)
{
	/*-- SNAP : Retrieve EXP limitation */
	sp5kExpLimit_t expLimit;
	sp5kAeExpLimitGet(SP5K_SENSOR_MODE_SNAP, &expLimit);
	*pmax = expLimit.expIdxMax;
	*pmin = expLimit.expIdxTblMin;
	printf("\n----- SNAP Gain Table Limitation ---------------\n");
	printf("expIdx MAX : %d \n", *pmax);
	printf("expIdx MIN : %d \n", *pmin);
	printf("\n\n");
}
/*-------------------------------------------------------------------------
 * Function : tbUtil_ViewAgcLimtGet
 *------------------------------------------------------------------------*/
void tbUtil_ViewAgcLimtGet(UINT32 *pmax, UINT32 *pmin)
{
	/*-- VIEW : Retrieve Gain limitation */
	sp5kAgcLimit_t agcLimit;
	sp5kAeAgcLimitGet(SP5K_SENSOR_MODE_PREVIEW, &agcLimit);
	*pmax = agcLimit.agcIdxMax;
	*pmin = agcLimit.agcIdxMin;
	printf("\n----- View Gain Table Limitation ---------------\n");
	printf("agcIdx MAX : %d \n", *pmax);
	printf("agcIdx MIN : %d \n", *pmin);
	printf("\n\n");
}
/*-------------------------------------------------------------------------
 * Function : tbUtil_SnapAgcLimtGet
 *------------------------------------------------------------------------*/
void tbUtil_SnapAgcLimtGet(UINT32 *pmax, UINT32 *pmin)
{
	/*-- SNAP : Retrieve Gain limitation */
	sp5kAgcLimit_t agcLimit;
	sp5kAeAgcLimitGet(SP5K_SENSOR_MODE_SNAP, &agcLimit);
	*pmax = agcLimit.agcIdxMax;
	*pmin = agcLimit.agcIdxMin;
	printf("\n----- SNAP Gain Table Limitation ---------------\n");
	printf("agcIdx MAX : %d \n", *pmax);
	printf("agcIdx MIN : %d \n", *pmin);
	printf("\n\n");
}
/*-------------------------------------------------------------------------
 * Function : tbUtil_PvRawLumaGet
 *------------------------------------------------------------------------*/
#define TBUTIL_RAW_GET_MAX
UINT16 tbUtil_PvRawLumaGet(void)
{
	UINT16 luma;

	sp5kCaliPreviewRawCallbackSet(appCalibRawCb);

	//tbUtil_PrintfCtrl(0); /* turn printf off */
	sp5kCalibrationCfgSet(SP5K_CALIBRATION_CALLBACK, SP5K_CALIB_CB_PREVIEW_RAW);
	/*sp5kCalibrationCfgSet(SP5K_CALIBRATION_CALLBACK, SP5K_CALIB_CB_PREVIEW_RAW_SAVE);*/

	//APP_SLEEP_MS(500);

	sp5kCalibrationCfgSet(SP5K_CALIBRATION_OFF, SP5K_CALIB_CB_PREVIEW_RAW);
	/*sp5kCalibrationCfgSet(SP5K_CALIBRATION_OFF, SP5K_CALIB_CB_PREVIEW_RAW_SAVE);*/

	//APP_SLEEP_MS(200);


	/*APP_SLEEP_MS(1000);*/

	//tbUtil_PrintfCtrl(1); /* turn printf on */

	if(calPVOB){
		UINT16 R,Gr,B,Gb;
		appGetRawAvgRGGB(&R,&Gr,&Gb,&B);
		luma = (UINT16)(((UINT32)R+(UINT32)Gr+(UINT32)B+(UINT32)Gb)/4);
	}
	else{
		#ifdef TBUTIL_RAW_GET_MAX
		luma = appGetRawMaxChannelAvg();
		#else
		{
			UINT16 r, g, b;
			appGetRawAvgRGB(&r, &g, &b);
			luma = g;
		}
		#endif
	}



	return luma;
}


/*-------------------------------------------------------------------------
 * Function : tbUtil_CapRawLumaGet
 *------------------------------------------------------------------------*/
UINT16 tbUtil_CapRawLumaGet(void)
{
	UINT16 luma;

/*s Modify by Aries 10/02/04*/
#if (_CAM_A900_EVM)||(_CAM_M900_EVM)||(_CAM_A900_D3)||(_CAM_CATHY_EVM)
	UINT32 raw_reset_time = SENSOR_RAW_RESET_TIME;
	UINT32 msh_delay_set = 0;
	UINT32 agcIdx = 0;
	SINT32 expIdx = 0;
	UINT16 sh_early = CALIB_AE_DEFAULT_SHUTTER_EARLY_1;
	#if (_CAM_A900_D3)||(_CAM_CATHY_EVM)
	UINT16 capModeChangeIdx= 135;
	#endif
	UINT32 iris_stop;
	appCalibData_t *pCalib;
	mshutDelayMode_t mode;
	
	pCalib = appCalibDataGet();			
	sp5kStillCapExpAgcGet(&expIdx,&agcIdx);

	double Tv = ((double)expIdx)/TV_UNIT;
	double exptime = pow(2, (0.0-Tv))*100000.0; //in 10us

	iris_stop = (appLensQuery(LENS_QUERY_IRIS_SEG_POS)==0) ? 0 : 1;
	//
	#if 0
	extern UINT32 xdata[]; 
	if(xdata[0]) {
		sh_early = xdata[0];		
	}
	else {
		if(pCalib->ShutterSign)  sh_early = pCalib->ShutterEarly[iris_stop];
	}
	#else	
	if(pCalib->ShutterSign)  sh_early = pCalib->ShutterEarly[iris_stop];
	#endif	

	#if (_CAM_A900_EVM)||(_CAM_M900_EVM)
	if (expIdx >= EXP_1_04) {
		msh_delay_set = raw_reset_time + exptime -sh_early;
		mode = MSHUT_SHORT_EXP_DELAY;
	}
	else {
		msh_delay_set = sh_early;
		mode = MSHUT_LONG_EXP_DELAY;
	}
	/*s Modify by Aries 10/02/08*/
	if (expIdx >= capModeChangeIdx) {
		msh_delay_set = sh_early;
		mode = MSHUT_SHORT_EXP_DELAY;
		sensorModeswLinkSet(0x40,0x41);
	}else {
		msh_delay_set = sh_early;
		mode = MSHUT_SHORT_EXP_DELAY;
		sensorModeswLinkSet(0x40,0x40);
	}
	#elif (_CAM_CATHY_EVM)
	msh_delay_set = sh_early;
	mode = MSHUT_SHORT_EXP_DELAY;
	/*e Modify by Aries 10/02/08*/
	#endif

	#if (_CAM_A900_D3)||(_CAM_CATHY_EVM)
		#if KIT_SHUT_CTRL_TIMER
		CapExpTime =exptime;
		CapShutColseTime =exptime-msh_delay_set;
		#endif
	#endif

	appLensMShutDelaySet(mode, msh_delay_set);

	printf("\n\n!!![MSH delay]!!! (exp:%d)(sh Early: %d) SH delay = %d(10us)\n\n",(UINT32)exptime,(UINT32)sh_early,msh_delay_set);	
#endif

	sp5kStillCapWbCallbackSet(NULL);
	sp5kStillCapRawCallbackSet(appCalibRawCb);

	#if KIT_LMI_CTRL
	appLensMShutLmiModeSet(1); /* msh Close */
	#endif
	APP_SLEEP_MS(10);
	
	tbUtil_PrintfCtrl(0); /* turn printf off */
	//appStillUtil_CalibCaptureSet(1);
	sp5kCalibrationCfgSet(SP5K_CALIBRATION_CALLBACK, SP5K_CALIB_CB_STILL_RAW);
	//APP_SLEEP_MS(30);

	//appStillUtil_CalibCaptureSet(0);
	sp5kCalibrationCfgSet(SP5K_CALIBRATION_OFF, SP5K_CALIB_CB_STILL_RAW);
	//APP_SLEEP_MS(30);

	tbUtil_PrintfCtrl(1); /* turn printf on */

	#ifdef TBUTIL_RAW_GET_MAX
	luma = appGetRawMaxChannelAvg();
	#else
	{
		UINT16 r, g, b;
		appGetRawAvgRGB(&r, &g, &b);
		luma = g;
	}
	#endif

	return luma;
/*e Modify by Aries 10/02/04*/

}
/*-------------------------------------------------------------------------
 * Function : tbUtil_PvRawSaturatedCheck
 *------------------------------------------------------------------------*/
BOOL tbUtil_PvRawSaturatedCheck(UINT16 sat_th, UINT16 *pluma)
{
	UINT16 r, gr, gb, b;

	*pluma = tbUtil_PvRawLumaGet();
	appGetRawAvgRGGB(&r, &gr, &gb, &b);
	printf("R-Gr-Gb-B: %d %d %d %d \n", r, gr, gb, b);
	if (*pluma<sat_th) {
		//printf("rawLuma = %d < (%d) \n", *pluma, sat_th);
		return FALSE;
	}

	if (*pluma!=r || *pluma!=gr || *pluma!=gb || *pluma!=b) {
		return FALSE;
	}

	return TRUE;
}

/*-------------------------------------------------------------------------
 * Function : tbUtil_CapRawSaturatedCheck
 *------------------------------------------------------------------------*/
BOOL tbUtil_CapRawSaturatedCheck(UINT16 sat_th, UINT16 *pluma)
{
	UINT16 r, gr, gb, b;

	*pluma = tbUtil_CapRawLumaGet();
	appGetRawAvgRGGB(&r, &gr, &gb, &b);
	printf("R-Gr-Gb-B: %d %d %d %d \n", r, gr, gb, b);
	if (*pluma<sat_th) {
		//printf("rawLuma = %d < (%d) \n", *pluma, sat_th);
		return FALSE;
	}

	if (*pluma!=r || *pluma!=gr || *pluma!=gb || *pluma!=b) {
		return FALSE;
	}

	return TRUE;
}
/*-------------------------------------------------------------------------
 * Function : tbUtil_PvMiniGainSearch
 *------------------------------------------------------------------------*/
BOOL tbUtil_PvMiniGainSearch(UINT16 sat_th, SINT32 expidx, UINT32 *pagcidx)
{
	UINT32 agcMax, agcMin, agcidx;
	UINT16 satLuma;
	BOOL isSuccess = FALSE;

	/*-- Get gain limitation */
	tbUtil_ViewAgcLimtGet(&agcMax, &agcMin);

	/*-- Observe Saturation raw value */
	printf("Looking for minimum gain ...\n");

	for (agcidx=agcMin; agcidx<=(agcMax*3/4); agcidx+=2) {
		printf("exp=%d agc=%d ", expidx, agcidx);
		sp5kPreviewExpAgcSet(expidx, agcidx);
		sp5kSensorVvalidWait();
		sp5kSensorVvalidWait();

		if (TRUE==tbUtil_PvRawSaturatedCheck(sat_th, &satLuma)) {
			isSuccess = TRUE;
			break;
		}
	}

	*pagcidx = agcidx;
	return isSuccess;

}

/*-------------------------------------------------------------------------
 * Function : tbUtil_CapMiniGainSearch
 *------------------------------------------------------------------------*/
BOOL tbUtil_CapMiniGainSearch(UINT16 sat_th, SINT32 expidx, UINT32 *pagcidx)
{
	UINT32 agcMax, agcMin, agcidx;
	UINT16 satLuma;
	BOOL isSuccess = FALSE;

	/*-- Get gain limitation */
	tbUtil_SnapAgcLimtGet(&agcMax, &agcMin);

	/*-- Observe Saturation raw value */
	printf("Looking for minimum gain ...\n");

	for (agcidx=agcMin; agcidx<=(agcMax*3/4); agcidx+=2) {
		printf("exp=%d agc=%d ", expidx, agcidx);
		sp5kStillCapExpAgcSet(expidx, agcidx);
		sp5kSensorVvalidWait();

		if (TRUE==tbUtil_CapRawSaturatedCheck(sat_th, &satLuma)) {
			isSuccess = TRUE;
			break;
		}
	}

	*pagcidx = agcidx;
	return isSuccess;

}
/*-------------------------------------------------------------------------
 * Function : tbUtil_AgcApproachTest
 *------------------------------------------------------------------------*/
UINT32 tbUtil_AgcApproachTest(BOOL isPvMode, UINT32 loBound, UINT32 hiBound, SINT32 expidx)
{
	UINT32 agcidx;
	UINT32 idxMax, idxMin, agcMax, agcMin;
	UINT16 luma;
	BOOL isSuccess;

	/*-- Get AGC Limitation */
	if (isPvMode) {
		tbUtil_ViewAgcLimtGet(&agcMax, &agcMin);
	} else {
		tbUtil_SnapAgcLimtGet(&agcMax, &agcMin);
	}
	idxMax=agcMax; idxMin=agcMin;

	isSuccess=TRUE;
	agcidx = (idxMax+idxMin)/2;
	DBG_PRINT("cali: Capture iso loBound = %d hiBound = %d agcidx = %d\n",loBound,hiBound,agcidx);
	do{
		sp5kPreviewExpAgcSet(expidx ,agcidx);
		sp5kStillCapExpAgcSet(expidx ,agcidx);
		sp5kSensorVvalidWait();
		sp5kSensorVvalidWait();
printf("expidx : %d, agcidx : %d\n",expidx,agcidx);
		if (isPvMode) {
			luma = tbUtil_PvRawLumaGet();
		} else {
			luma = tbUtil_CapRawLumaGet();
		}
		DBG_PRINT(".... agcidx=%d rawLuma(%d)\n",agcidx,luma);

		if ((luma<loBound && agcidx>=agcMax) ||
			(luma>hiBound && agcidx<=agcMin)) {
			/*-- Out of AE limitation !! */
			printf("____ Out of Limitation _____\n");
			isSuccess=FALSE;
			break;
		}

		if (luma >= loBound && luma <= hiBound) {
			/*-- Converged */
			printf("____ Converged _____\n");
			isSuccess = TRUE;
			break;
		}

		if(luma < loBound && agcidx < agcMax) {
			/*-- Too Dark !! */
			if (idxMax-idxMin <2) {
				agcidx++;
			} else {
				idxMin = agcidx; agcidx=(idxMax+idxMin)/2;
			}
		} else if(luma > hiBound && agcidx > agcMin){
			/*-- Too Bright !! */
			if (idxMax-idxMin <2) {
				agcidx--;
			} else {
				idxMax = agcidx; agcidx=(idxMax+idxMin)/2;
			}
		}
	}while(1);

	if (isSuccess) {
		printf("Approach: agcidx(%d) luma(%d) \n", agcidx, luma);
		return agcidx;
	} else {
		return 0;
	}
}
/*-------------------------------------------------------------------------
 * Function : tbUtil_AeAgcConverge
 *------------------------------------------------------------------------*/
SINT32 tbUtil_AeAgcConverge(SINT32 expidx)
{
	UINT32 agcidx;
	UINT32 idxMax, idxMin, agcMax, agcMin;
	BOOL isSuccess;
	UINT32 msg, param, osTime;
	UINT32 ret;
	sp5kAeResult_t aeResult;

	tbUtil_ViewAgcLimtGet(&agcMax, &agcMin);
	idxMax=agcMax; idxMin=agcMin;

	isSuccess=TRUE;
	agcidx = (idxMax+idxMin)/2;
	do{
		printf("...Setting exp(%d) agc(%d) \n", expidx, agcidx);

		sp5kPreviewExpAgcSet(expidx ,agcidx);
		tbUtil_QuickSweepHostMessage(300);

		osTime = appMsTimeGet();
		while (1)
		{
			if (appMsTimeGet()-osTime > 5000) {
				isSuccess = FALSE;
				printf("AE Time out !!!!\n");
				osTime = (UINT32)(~0);
				break;
			}

			isSuccess = FALSE;
			sp5kHostMsgReceive(&msg, &param);
			if (msg==SP5K_MSG_AE_READY)
			{

				#if SP5K_ALGORITHM_AE //lester 20120402
				ret = appAEALGLib_ResultGet(param,&aeResult);
				#else
				ret = sp5kAePreviewResultGet(param,&aeResult);
				#endif

				printf("msg: AE_RDY (%d)(%d)\n", aeResult.valid, aeResult.aeStep);
				if (ret != SUCCESS ||
					!(aeResult.valid & AAA_INFO_RESULT_AE_VALID))
				{
					printf("ae result invalid(%d)\n",ret);
					continue;
				}

				if (aeResult.aeStep<=1 && aeResult.aeStep>=-1) {
					printf(">>> Converged !! aeStep(%d) \n", aeResult.aeStep);
					isSuccess =  TRUE;
					break;
				} else {
					break;
				}
			} else {
				continue;
			}
		}

		if (isSuccess==FALSE && osTime==((UINT32)~0)) {
			/*-- Time out !! */
			agcidx = -1;
			break;
		} else if (isSuccess==FALSE) {
			agcidx -= aeResult.aeStep/2;//4 //fan.xiao modified 2011-11-01 , 15:07:35
			printf("... adjust agcidx(%d) \n", agcidx);
			if (agcidx<agcMin || agcidx>agcMax) {
				printf("!!! Fail converge agc, agcidx(%d), aeStep(%d) \n", agcidx,aeResult.aeStep);
				agcidx = -1;
				break;
			}
		} else {
			break;
		}
	}while(1);

	if (isSuccess) {
		printf("DONE: agcidx(%d) aeStep(%d) \n", agcidx, aeResult.aeStep);
		return agcidx;
	} else {
		return -1;
	}

}


/*-------------------------------------------------------------------------
 *  File Name : tbUtil_TestLoopAbort
 *------------------------------------------------------------------------*/
BOOL tbUtil_TestLoopAbort(void)
{
	UINT32 osTime;
	UINT32 msg, param;
	UINT32 key;

	osTime = appMsTimeGet();
	while(1) {
		if (appMsTimeGet()-osTime>500) return 0;

		sp5kHostMsgReceive(&msg, &param);

		if (!IS_SP5K_BTN_MSG(msg)) {
			continue;
		}
		key = appBtnMsgToKey(msg, param);

		if (key==APP_KEY_PRESS_DEL) {
			return 1;
		}
	}
}
void tbUtil_DataMinMaxFind(UINT32 *pData)
{
	/*TBD*/
}
void tbUtil_DataNormal(UINT32 *pData, UINT32 NumData,UINT32 DataMax, UINT32 DataMin, UINT32 NorMaxX, UINT32 NorMaxY)
{
	if(DataMax < DataMin){
		tbUtil_DataMinMaxFind(pData);
	}
	UINT32 Y_Scale = (DataMax < NorMaxY ? 1 :(DataMax/NorMaxY));
	UINT8 i;
	for(i = 0; i < NumData; i += 1){
		(*(pData + i)) = ((*(pData + i)) / Y_Scale);
	}
}
void tbUtil_DataCurveDraw(UINT32 *pData, UINT32 NumData)
{
	UINT32 i = 0;
	UINT32 X_Scale = (LCD_SIZE_X/ NumData);
	UINT16 y1, y2;
	for(i = 1; i < NumData; i ++){
		if((*(pData + i -1)) > LCD_SIZE_Y){
			y1 = 0; 
		}else{
			y1 = LCD_SIZE_Y - (*(pData + i -1));
		}

		if((*(pData + i)) > LCD_SIZE_Y){
			y2 = 0; 
		}else{
			y2 = LCD_SIZE_Y - (*(pData + i));
		}
		appOsdLib_OsdLayerLineDraw(X_Scale * (i - 1), y1, X_Scale * i, y2);
	}	

}

