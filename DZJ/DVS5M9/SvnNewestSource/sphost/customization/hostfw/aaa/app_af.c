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
#define HOST_DBG	1
#include "common.h"
#include <stdio.h>
#include <string.h>
#include "app_com_def.h"
#include "app_com_api.h"
#include "api/Sp5k_af_api.h"
#include "app_lens_api.h"
#include "api/Sp5k_ae_api.h"
#include "api/Sp5k_awb_api.h"
#include "app_state_ctrl.h"
#include "app_view_param.h"
#include "app_still.h"
#include "sp5k_sensor_api.h"
#include "app_lens_focus.h"


/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

static UINT32 isFaceAf = 0;
static UINT16 CurrPos, bg , end;
static UINT16 itvl =1;
static UINT32 afvaluemax = 0, afvaluemin = 0xffffffff;
static UINT16 afvaluemaxpos = 0, afvalueminpos = 0;
static UINT32 afval[9];
static BOOL IsBracket = FALSE;
static UINT8 BktInterval= 5,BktSnapNum = 10;
static UINT8 IsFirst = TRUE;
static UINT8 Agc_ISO100 = 0;

UINT8 afend =0;

static UINT8 NosCntTbl[6] = {12, 16, 24, 32, 52, 88};
#define NosSlope(agc)				(UINT32)((UINT32)(NosCntTbl[(agc/Agc_ISO100) + 1] - (UINT32)NosCntTbl[(agc/Agc_ISO100)]) <<6) /*  X1024   and then /16  */
#define CurrCorThr					(*(volatile UINT8 *)0xb0002405)

#define AF_LOG
#ifdef	AF_LOG
static UINT8 *AF_LOG_BUF = NULL, *AF_LOG_REC = NULL;
UINT32 AF_LOG_REC_fd;
#define AF_LOG_BUF_RESET			AF_LOG_BUF = AF_LOG_REC;memset(AF_LOG_BUF, 0x00, 4096 * sizeof(AF_LOG_BUF)) 
#define AF_LOG_BUF_GET				AF_LOG_REC + strlen(AF_LOG_REC)
#define AF_LOG_ADD(fmt, args...)	printf("\n");printf(fmt, ##args);sprintf(AF_LOG_BUF_GET, "\n");sprintf(AF_LOG_BUF_GET, fmt, ##args)
#define AF_LOG_DUMP					sp5kFsFileWrite(AF_LOG_REC_fd, AF_LOG_REC, 4096 * sizeof(UINT8));AF_LOG_BUF_RESET
#define AF_LOG_CLOSE				AF_LOG_DUMP;sp5kFsFileClose(AF_LOG_REC_fd);sp5kFree(AF_LOG_BUF)
#else
#define AF_LOG_ADD					DBG_PRINT
#endif

#ifdef AF_CURVE_SHOW
static UINT32 *AfCurve = NULL; 
#define ValueRange					(afvaluemax - afvaluemin)
#define ScanRange					(end - bg)
#define LCD_SIZE_X					320                                                                                                                                                       
#define LCD_SIZE_Y					240  
#define AF_CURVE_X_Scale			(UINT32)(LCD_SIZE_X/(ScanRange/itvl))
#define AF_CURVE_Y_Scale			(afvaluemax < LCD_SIZE_Y ? 1 : (afvaluemax / LCD_SIZE_Y))

void AfValueCurveNormal(void);


#endif

static UINT8 IsKeyInNosThr = FALSE;

#define RE_1ST_SCAN


//Modified by Brian 08/08/08
/* Z1 ~Z7 Macro*/


static UINT16 afscanInfo[ZOOM_ZS_NUM+1][3] ={	
	{FOCUS_0_S + FOCUS_POSITION_OFFSET, FOCUS_0_E + FOCUS_POSITION_OFFSET, FOCUS_0_STEP}, 
	{FOCUS_1_S + FOCUS_POSITION_OFFSET, FOCUS_1_E + FOCUS_POSITION_OFFSET, FOCUS_1_STEP}, 
	{FOCUS_2_S + FOCUS_POSITION_OFFSET, FOCUS_2_E + FOCUS_POSITION_OFFSET, FOCUS_2_STEP}, 
	{FOCUS_3_S + FOCUS_POSITION_OFFSET, FOCUS_3_E + FOCUS_POSITION_OFFSET, FOCUS_3_STEP}, 
	{FOCUS_4_S + FOCUS_POSITION_OFFSET, FOCUS_4_E + FOCUS_POSITION_OFFSET, FOCUS_4_STEP},
	{FOCUS_5_S + FOCUS_POSITION_OFFSET, FOCUS_5_E + FOCUS_POSITION_OFFSET, FOCUS_5_STEP},
	{FOCUS_6_S + FOCUS_POSITION_OFFSET, FOCUS_6_E + FOCUS_POSITION_OFFSET, FOCUS_6_STEP},
	{FOCUS_7_S + FOCUS_POSITION_OFFSET, FOCUS_7_E + FOCUS_POSITION_OFFSET, FOCUS_7_STEP}
};



/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern 	void tbUtil_DataNormal(UINT32 *pData, UINT32 NumData,UINT32 DataMax, UINT32 DataMin, UINT32 NorMaxX, UINT32 NorMaxY);
extern 	void tbUtil_DataCurveDraw(UINT32 *pData, UINT32 NumData);


/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

static void AfScanRangeInit()
{
		UINT16 zlevel;

		zlevel=appLensQuery(LENS_QUERY_ZOOM_SEG_POS); //lensZoomCurSegGet();
		if (pViewParam->af==UI_FOCUS_MACRO)
		{
			#if 0
			bg =afscanInfo[ZOOM_ZS_NUM][0];
			end =afscanInfo[ZOOM_ZS_NUM][1];
			itvl = afscanInfo[ZOOM_ZS_NUM][2];			
			#else
			bg =afscanInfo[0][1] -20;
			end =afscanInfo[0][1]+ 80;
			itvl = afscanInfo[ZOOM_ZS_NUM][2];						
			#endif
			DBG_PRINT("%s: MACRO, zlevel(%d), bg(%d), end(%d), itvl(%d)", __FUNCTION__, zlevel, bg, end, itvl);
		}
		else
		{
			#ifdef afskip
			bg =afscanInfo[zlevel][0] - afscanInfo[zlevel][2];
			#else
			bg =afscanInfo[zlevel][0];			
			#endif
			end =afscanInfo[zlevel][1];
			itvl = afscanInfo[zlevel][2];
			DBG_PRINT("%s: NORMAL, zlevel(%d), bg(%d), end(%d), itvl(%d)", __FUNCTION__, zlevel, bg, end, itvl);
		}
		DBG_PRINT("af info :(%d) %d %d %d\n", zlevel, bg, end, itvl);
		CurrPos = bg;
	
}


UINT16 appFocusInitPosGet(UINT8 idx)
{
	UINT16 InitPos=0;
	switch (idx)
	{	
		case UI_FOCUS_NORMAL :
			InitPos = ((afscanInfo[appLensQuery(LENS_QUERY_ZOOM_SEG_POS)][1])-20);
			break;
		case UI_FOCUS_MACRO:
			InitPos = afscanInfo[ZOOM_ZS_NUM][1];
			break;
		case UI_FOCUS_INFINITY:	
			InitPos = afscanInfo[appLensQuery(LENS_QUERY_ZOOM_SEG_POS)][0];
			break;
		default :
			break;				
	}
	return InitPos;
}

UINT16* appFocusInfoGet(void)
{
	UINT16 *p= &afscanInfo[0][0];
	return(p);
}


UINT16 appFocusMaxPosGet()
{
	return (afvaluemaxpos - itvl);
}


UINT8 AfStatusGet()
{
	return afend;
}

void AfStatusSet(UINT8 status)
{
	afend = status;
}
void appFocusModeChange(UINT8 idx)
{
	appLensCreate();
	appFocusPosGo(appFocusInitPosGet(idx));
	appFocusStopWait();
	appLensDestroy();
}

void 
appAfValueReset(
	void)
{
	afvaluemax = 0;
	afvaluemin = 0xffffffff;
	afvaluemaxpos= CurrPos = bg =end= itvl = 0;
}

/*
appFocusScanPosGo() waits VD to make moving of focus lens overlap flushing sensor.
This benefits the accuracy of AF value.
*/
UINT32 appFocusScanPosGo(
	SINT16 pos)
{
	UINT32 ret;
	appFocusStopWait();
	sp5kSensorVvalidWait();
	ret = appFocusPosGo(pos);
	return ret;
	
}
#if 0
UINT8 NosThrGet(SINT32 exp, UINT32 agc) 
{
	agc -= (CurrPos - bg) / 100;
	agc = ((agc > 80) ? 80 : agc);
	UINT32 NosThr = 0;
	if(IsKeyInNosThr){
		NosThr = CurrCorThr;
	}else{
	 	NosThr = (((NosSlope(agc))*(agc - ((agc / Agc_ISO100) * Agc_ISO100)))>> 10) + NosCntTbl[(agc/Agc_ISO100)]; 
		NosThr = NosThr&0x000000FF;
	}
	AF_LOG_ADD("\n\n NosThrGet(%d, %d)=%d\n",exp, agc, NosThr);
	return NosThr;
}
#else
UINT8 NosThrGet(SINT32 exp, UINT32 agc) 
{
	agc = ((agc > 80) ? 80 : agc);
	UINT32 NosThr = 0;
	if(IsKeyInNosThr){
		NosThr = CurrCorThr;
	}else{
	 	NosThr = (((NosSlope(agc))*(agc - ((agc / Agc_ISO100) * Agc_ISO100)))>> 10) + NosCntTbl[(agc/Agc_ISO100)]; 
		NosThr = NosThr&0x000000FF;
	}
	AF_LOG_ADD("\n\n NosThrGet(%d, %d)=%d\n",exp, agc, NosThr);
	return NosThr;
}

#endif

void aaa_AutoFocus()
{
	SINT32 CurrExp;
	UINT32 CurrAgc;
	UINT8 CurrNos;
	afend = AF_BUSY;
	appFocusStopWait();
	sp5kAfModeSet(SP5K_AF_MODE_ON);
	sp5kPreviewExpAgcGet(&CurrExp, &CurrAgc);
	CurrNos = NosThrGet(CurrExp, CurrAgc);
	sp5kAfCfgSet(SP5K_AF_CORING_THR, CurrNos);
	sp5kAfCfgSet(SP5K_AF_CORING_VALUE, CurrNos);
	#ifdef RE_1ST_SCAN
	IsFirst = TRUE;
	#endif		
	/*
	for ensuring the setting of SP5K_AF_CORING_THR
	printf("\n\nSP5K_AF_CORING_THR == %d\n", *(volatile UINT8 *)0xb0002405);
	*/

}

UINT32 appAfInit()
{
	UINT32 ret;
	
	#ifdef AF_LOG
	if(AF_LOG_BUF != NULL && AF_LOG_BUF != AF_LOG_REC)
	{
		AF_LOG_BUF_RESET;
	}
	else
	{
		AF_LOG_BUF = sp5kMalloc(4096 * sizeof(AF_LOG_BUF));
		memset(AF_LOG_BUF, 0x00, 4096 * sizeof(AF_LOG_BUF)); 
		AF_LOG_REC = AF_LOG_BUF;
	}
	AF_LOG_REC_fd = sp5kFsFileOpen("D:\\AF___LOG.LOG", SP5K_FS_OPEN_APPEND);
	if(!AF_LOG_REC_fd)
	{
		AF_LOG_REC_fd = sp5kFsFileOpen("D:\\AF___LOG.LOG", SP5K_FS_OPEN_CREATE);
	}
	sp5kFsFileSeek(AF_LOG_REC_fd, 0, SP5K_FS_SEEK_END);
	#endif

	
	DBG_PRINT("%s ...\n", __FUNCTION__);
	appAaaAfWinSet(&isFaceAf);

	appAeCb_t *paeCb = appAeCbGet();
	Agc_ISO100 = paeCb->pviso;

	AfScanRangeInit();		
	appLensAFCreate();

	ret = appFocusPosGo(CurrPos);

	#ifdef AF_CURVE_SHOW
	AfCurve = sp5kMalloc(((end - bg)/itvl + 1) * sizeof(UINT32));
	memset(AfCurve, 0x00, ((end - bg)/itvl + 1));
	#endif


	return ret;
}

void appFocusScan(UINT32 param)
{
		UINT32 sum_afvalue = 0;
		memset(afval, 0x00, sizeof(afval));


		/*
		The 1st AF_READY is not relevant, because the exposure is not stated till SP5K_AF_MODE_ON;
		however, it's no way to be predict the timing of executing "sp5kAfModeSet(SP5K_AF_MODE_ON)"
		, and this results in shortening the time of exposure.
		*/
		#ifdef RE_1ST_SCAN
		if(IsFirst == TRUE && CurrPos == bg){
			IsFirst = FALSE;
			return;
		}
		#endif
		if (CurrPos <= end ){		
			if (sp5kAfWinGet(param, SP5K_AF_G, afval)) {
				DBG_PRINT("\n\n!!!!!!!! AfWinGet failed !!!!!!!!!\n\n");
				return;
			}
			if(isFaceAf) {
				sum_afvalue = afval[0];							
			}
			else {
				UINT8 i;
				for(i = 0; i < 9; i ++) {
					sum_afvalue += afval[i];
				}			
			}
			sum_afvalue = (sum_afvalue == 0 ? 1:sum_afvalue);

			#ifdef AF_CURVE_SHOW
			*(AfCurve + ((CurrPos - bg)/itvl)) = sum_afvalue;
			#endif
			
			if (sum_afvalue > afvaluemax){
				afvaluemax = sum_afvalue;
				afvaluemaxpos = CurrPos;
				}
			if (sum_afvalue < afvaluemin){
				afvaluemin = sum_afvalue;
				afvalueminpos = CurrPos;
				}
			//AF_LOG_ADD("%d: %d, %d, %d, %d, %d, %d",CurrPos, afval[0], afval[1], afval[2], afval[3], afval[4], afval[5]);
			AF_LOG_ADD("%d, %d [%d, %d], %d: %d", appLensQuery(LENS_QUERY_ZOOM_SEG_POS), appMsTimeGet(), bg, end, CurrPos, sum_afvalue);

			profLogPrintf(0 , "AF e, %x", param);
			appFocusScanPosGo(CurrPos+itvl);
			CurrPos += itvl;
			}

		if(CurrPos > end ) {	
			if (afend == AF_BUSY){	
			/*
			Threshold of Success: Max/Min >=2
			*/
				if(afvaluemin >(afvaluemax >> 1) || (appFocusMaxPosGet() + (itvl<<1) >= end)) {
					AF_LOG_ADD("\nAF_FAIL !!\n");
					afend = AF_FAIL;
				}
				else {
					AF_LOG_ADD("\nAF_SUCCESS !!\n");
					afend = AF_SUCCESS;
				}
				if(!(IS_CALIB_GROUP(appActiveStateGet()))){
					if(IsBracket){
						appFocusPosGo(appFocusMaxPosGet() - BktInterval * BktSnapNum);
					}else{
						appFocusPosGo(appFocusMaxPosGet());
					}	
				}
			}
			if (appLensQuery(LENS_QUERY_FOCUS_MOTION_STATE) == 1){
				appFocusStopWait();
			}else
			{
				DBG_PRINT("%d: max pos %d\n", appMsTimeGet(), appFocusMaxPosGet());
				appLensAFDestroy();
				sp5kAfModeSet(SP5K_AF_MODE_OFF);
				#ifdef AF_LOG
				UINT16 dirkey, filekey;
				sp5kDcfFsNextKeyGet(&dirkey, &filekey);
				AF_LOG_ADD("dirkey = %d, filekey = %d\n", dirkey, filekey);
				AF_LOG_CLOSE;
				#endif
				#ifdef AF_CURVE_SHOW
				AfValueCurveNormal();
				#endif

				}
			}
}



/*
The following functions and commands are designed for AF tests, 
,including bracket tests and noise-related tests. These functions
have nothing to do with normal AF operation or calibration.

Brian 2008/09/26.
*/

#define FOCUS_STOP_TAG (~0x00)

#ifdef AF_CURVE_SHOW

void AfValueCurveNormal(void)
{
	tbUtil_DataNormal(AfCurve, ((end - bg)/itvl + 1), afvaluemax, afvaluemin, LCD_SIZE_X, LCD_SIZE_Y);
}

void AfValueCurveDraw(UINT8 focusStatus)
{
	SINT32 CurrExp;
	UINT32 CurrAgc;
	UINT8 *szStr = sp5kMalloc(32 * sizeof(UINT8));

	sp5kPreviewExpAgcGet(&CurrExp, &CurrAgc);
	sprintf(szStr, "(%3d,%3d):%2d   (%3d,%3d),%2d,%2d", 
					CurrExp, CurrAgc, CurrCorThr, bg, end, appFocusMaxPosGet(), (end - bg)/itvl + 1);
	appOsdLib_TextDraw(0, 0, SP5K_GFX_ALIGN_TOP_LEFT, szStr);
	sprintf(szStr, "%d--%d", afvaluemin, afvaluemax); 
	appOsdLib_TextDraw(0, 32, SP5K_GFX_ALIGN_TOP_LEFT, szStr);		

	if(focusStatus == AF_SUCCESS){
		appOsd_ColorDrawShapeColorSet(12, 4, 12, 4);	//GREEN
	}else
	{
		appOsd_ColorDrawShapeColorSet(18, 4, 18, 4);	//RED
	}
	tbUtil_DataCurveDraw(AfCurve, ((end - bg)/itvl + 1));
	sp5kFree(AfCurve);
	sp5kFree(szStr);
}
#endif

void AfFocusStopTagFind(void){
	UINT32 msg, param;
	profLogAdd(0, "Tag 1");
	do
	{
		sp5kHostMsgReceive(&msg, &param);
		if(SP5K_MSG_AF_READY == msg)
		profLogPrintf(0, "Recv AF Rdy, %x", param);
	}while((param != (UINT32)FOCUS_STOP_TAG) && (msg != (UINT32)SP5K_MSG_AF_READY)) ;

	profLogAdd(0, "Tag 2");
}

static UINT32 AfCurrRdyParamGet(void){
	UINT32 msg, param;
	AfFocusStopTagFind();
	do
	{
		sp5kHostMsgReceive(&msg, &param);
	}while((msg != SP5K_MSG_AF_READY));

	return param;
}

void
cmdAF(
	UINT32 argc,
	UINT8 *arg[],
	UINT32 v[]
)
{
	if (argc==1) {
		printf("af usage:\n"
			" af 0|1 : disable/enable AF\n"
			" af thrhd _value_ : set af threshold \n"
			" af win 0|1 : show afwin \n"
			#if 0
			" af info 0|1 : disable/enable AF info\n"
			" af exp IDX : set preview exp\n"
			" af agc IDX : set preview agc\n"
			" af cexp IDX : set capture exp\n"
			" af cagc IDX : set capture agc\n"
			" af report : show aeInitReport \n"
			" af freq 0|1 : control freqDbg \n"
			#endif
		);
	} else if (strcmp(arg[1], "thrhd")==0) {
		if (v[2]==0) return;
		printf("af thrhd %d \n", v[2]);
		sp5kAfCfgSet(SP5K_AF_NOISE_THR, v[2]);
	} else if (strcmp(arg[1], "win")==0) {
		printf("af win %d \n", v[2]);
		sp5kAfCfgSet(SP5K_AF_WIN_POS_TEST, (v[2] ? 1: 0));
	} else if ((strcmp(arg[1], "bracket") == 0)&&(pViewParam->drive==UI_DRIVE_BURST)){
		IsBracket = TRUE;
		if(argc == 2){
			BktInterval = 5;
			BktSnapNum = 10;
		}else if(argc == 3){
			BktInterval = atoi(arg[2]);
			BktSnapNum = 10;
		}else if(argc == 4){
			BktInterval = atoi(arg[2]);
			BktSnapNum = atoi(arg[3]);
		}
		if(BktInterval == 0){
			IsBracket = FALSE;
			printf("AF Bracket disabled.\n");
		}else{
			printf("AF Bracket enabled, with the interval of %d. steps and %d snapshots.\n", BktInterval, BktSnapNum);
		}
	}else if(strcmp(arg[1], "value") == 0){
		UINT32 afwin[9] = {0}, NumAFGet, frameid;
		UINT32 afresult = 1, i, StepIntval = 0;

		
		sp5kAfModeSet(SP5K_AF_MODE_ON);
		sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 200);
		if(argc == 3 && v[2] != 0)						//Get AF value once on a specific focus step.
		{
			NumAFGet = 1;
		}else if(argc == 4  && v[2] >= 0			)	//Continuously get AF value on a single focus step.
		{
			StepIntval = 0;
			NumAFGet = v[3];

		}else if(argc == 5 && v[2] >= 100 && v[4] >= 1)	//Continuously get AF value within a specific range.
		{	
			StepIntval = v[4];
			NumAFGet = (v[3] - v[2])/v[4];
		}else{
			return;
		}
		
		for(i = 0; i <= NumAFGet; i++)
		{
			//printf("focus [%d] --> [%d], ", appLensQuery(LENS_QUERY_FOCUS_STEP_POS), v[2] + (StepIntval * i));

			if(StepIntval!= 0 || appLensQuery(LENS_QUERY_FOCUS_STEP_POS) != v[2]) {
			/*-- Power on lens motor 
			lensMotorPowerOn();
			lensLedPowerOn(LENS_LED_ID_FHM);
			*/
			lensFocusJob( v[2] + (StepIntval * i));
			/*-- Power off lens motor 
			lensMotorPowerOff();
			lensLedPowerOff(LENS_LED_ID_FHM);
			*/
			}
			frameid = AfCurrRdyParamGet();
			afresult = sp5kAfWinGet(frameid, SP5K_AF_G, afwin);
			if(afresult != SUCCESS){
				memset(afwin, 0x00, 9);
				frameid = AfCurrRdyParamGet();
				afresult = sp5kAfWinGet(frameid, SP5K_AF_G, afwin);
			}
			printf("The Af value is %d\n",(afwin[0] + afwin[1] + afwin[2] + afwin[3] + afwin[4] + afwin[5] + afwin[6] + afwin[7] + afwin[8]));
			memset(afwin, 0x00, sizeof(afwin) );
			
		}
		
		sp5kAfModeSet(SP5K_AF_MODE_OFF);
		
	}else if(strcmp(arg[1], "corthr") ==0){
		UINT32 afwin[9] = {0}, NumAFGet, frameid;
		UINT32 afresult = 1, i, StepIntval = 0;
		UINT8 currsetting = *(volatile UINT8 *)0xb0002405;

		sp5kAfModeSet(SP5K_AF_MODE_ON);
		sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 200);
		if(argc == 3)									//Get AF value once with a coring threshold value.
		{
			NumAFGet = 1;
		}else if(argc == 4 && v[2] >= 0)				//Continuously get AF values with a coring threshold value.
		{
			StepIntval = 0;
			NumAFGet = v[3];
		}else if(argc == 5 && v[2] >= 0 && v[4] >= 1)	//Continuously get AF values within a specific range of coring threshold values.
		{	
			StepIntval = v[4];
			NumAFGet = (v[3] - v[2])/v[4];
		}else{
			return;
		}
		
		printf("Current focus position is [%3d]; current coring THRESHOLD is [%d].\n", appLensQuery(LENS_QUERY_FOCUS_STEP_POS), currsetting);
		for(i = 0; i <= NumAFGet; i++)
		{
			sp5kAfCfgSet(SP5K_AF_NOISE_CORING, v[2] + (StepIntval * i));
			sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 200);
			
			frameid = AfCurrRdyParamGet();
			afresult = sp5kAfWinGet(frameid, SP5K_AF_G, afwin);
			if(afresult != SUCCESS){
				memset(afwin, 0x00, 9);
				frameid = AfCurrRdyParamGet();
				afresult = sp5kAfWinGet(frameid, SP5K_AF_G, afwin);
		}
			printf("Noise coring threshold = %3d, The Af value is %d\n",v[2] + (StepIntval * i), (afwin[0] + afwin[1] + afwin[2] + afwin[3] + afwin[4] + afwin[5] + afwin[6] + afwin[7] + afwin[8]));
		}
		
		sp5kAfCfgSet(SP5K_AF_NOISE_CORING, currsetting);
		sp5kAfModeSet(SP5K_AF_MODE_OFF);
		
	}else if(strcmp(arg[1], "corval") ==0){
		UINT32 afwin[9] = {0}, NumAFGet, frameid;
		UINT32 afresult = 1, i, StepIntval = 0;
		UINT8 currsetting = *(volatile UINT8 *)0xb0002406;

		sp5kAfModeSet(SP5K_AF_MODE_ON);
		sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 200);
		if(argc == 3)										//Get AF value once with a coring value.
		{
			NumAFGet = 1;
		}else if(argc == 4  && v[2] >= 0 )					//Continuously get AF value with a coring value.
		{
			StepIntval = 0;
			NumAFGet = v[3];
		}else if(argc == 5  && v[2] >= 0 && v[4] >= 1)		//Continuously get AF value within a specific range of coring values.
		{	
			StepIntval = v[4];
			NumAFGet = (v[3] - v[2])/v[4];
		}else{
			return;
		}
		
		printf("Current focus position is [%3d]; current coring VALUE is [%d].\n", appLensQuery(LENS_QUERY_FOCUS_STEP_POS), currsetting);
		for(i = 0; i <= NumAFGet; i++)
		{
			sp5kAfCfgSet(SP5K_AF_CORING_VALUE, v[2] + (StepIntval * i));
			sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 200);
			
			frameid = AfCurrRdyParamGet();
			afresult = sp5kAfWinGet(frameid, SP5K_AF_G, afwin);
			if(afresult != SUCCESS){
				memset(afwin, 0x00, 9);
				frameid = AfCurrRdyParamGet();
				afresult = sp5kAfWinGet(frameid, SP5K_AF_G, afwin);
		}
			printf("Noise coring value = %3d, The Af value is %d\n",v[2] + (StepIntval * i), (afwin[0] + afwin[1] + afwin[2] + afwin[3] + afwin[4] + afwin[5] + afwin[6] + afwin[7] + afwin[8]));
			printf("%d\n",(afwin[9] + afwin[1] + afwin[2] + afwin[3] + afwin[4] + afwin[5] + afwin[6] + afwin[7] + afwin[8]));
		}
		
		sp5kAfCfgSet(SP5K_AF_CORING_VALUE, currsetting);
		sp5kAfModeSet(SP5K_AF_MODE_OFF);
	}else if(strcmp(arg[1], "noscnt") ==0){
		UINT32 afwin[9] = {0}, NumAFGet, frameid;
		UINT32 afresult = 1, i, StepIntval = 0;
		UINT8 currsetting = *(volatile UINT8 *)0xb0002407;

		sp5kAfModeSet(SP5K_AF_MODE_ON);
		sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 200);
		if(argc == 3)									//Get AF value once with a noise threshold value.
		{
			NumAFGet = 1;
		}else if(argc == 4  && v[2] >= 0 )				//Continuously get AF value with a noise threshold value.
		{
			StepIntval = 0;
			NumAFGet = v[3];
		}else if(argc == 5 && v[2] >= 0 && v[4] >= 1)	//Continuously get AF value within a specific range of noise threshold values.
		{	
			StepIntval = v[4];
			NumAFGet = (v[3] - v[2])/v[4];
		}else{
			return;
		}
		
		printf("Current focus position is [%3d]; current NOISE threshold is [%d].\n", appLensQuery(LENS_QUERY_FOCUS_STEP_POS), currsetting);
		for(i = 0; i <= NumAFGet; i++)
		{
			sp5kAfCfgSet(SP5K_AF_NOISE_THR, v[2] + (StepIntval * i));
			sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 200);
			
			frameid = AfCurrRdyParamGet();
			afresult = sp5kAfWinGet(frameid, SP5K_AF_NOISE_CNT, afwin);
			if(afresult != SUCCESS){
				memset(afwin, 0x00, 9);
				frameid = AfCurrRdyParamGet();
				afresult = sp5kAfWinGet(frameid, SP5K_AF_NOISE_CNT, afwin);
		}
			printf("Noise threshold value = %3d, The Af NOISE COUNT is %d\n",v[2] + (StepIntval * i), (afwin[0] + afwin[1] + afwin[2] + afwin[3] + afwin[4] + afwin[5] + afwin[6] + afwin[7] + afwin[8]));
		}
		
		sp5kAfCfgSet(SP5K_AF_NOISE_THR, currsetting);
		sp5kAfModeSet(SP5K_AF_MODE_OFF);
	}else if(strcmp(arg[1], "corthrval") ==0){
		UINT32 afwin[9] = {0}, NumAFGet, frameid;
		UINT32 afresult = 1, i, StepIntval = 0;
		UINT8 currCorThr = *(volatile UINT8 *)0xb0002405;
		UINT8 currCorVal = *(volatile UINT8 *)0xb0002406;

		sp5kAfModeSet(SP5K_AF_MODE_ON);
		sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 200);
		if(argc == 4)													//Get AF value once with a coring threshold value.
		{
			NumAFGet = 1;
		}else if(argc == 5  && v[2] >= 0)								//Continuously get AF values with a coring threshold value.
		{
			StepIntval = 0;
			NumAFGet = v[3];
		}else if(argc == 6 && v[2] < v[3] && v[2] >= 0 && v[4] >= 1)	//Continuously get AF values within a specific range of coring threshold values.
		{	
			StepIntval = v[4];
			NumAFGet = (v[3] - v[2])/v[4];
		}else{
			return;
		}

		printf("Current Configuration: Focus position [%3d]; Coring THRESHOLD [%3d],Coring VALUE [%3d].\n", appLensQuery(LENS_QUERY_FOCUS_STEP_POS), currCorThr, currCorVal);

		sp5kAfCfgSet(SP5K_AF_NOISE_CORING, v[5]);
		sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 200);

		for(i = 0; i <= NumAFGet; i++)
		{
			sp5kAfCfgSet(SP5K_AF_CORING_VALUE, v[2] + (StepIntval * i));
			sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 200);			

			
			frameid = AfCurrRdyParamGet();
			afresult = sp5kAfWinGet(frameid, SP5K_AF_G, afwin);
			if(afresult != SUCCESS){
				memset(afwin, 0x00, 9);
				frameid = AfCurrRdyParamGet();
				afresult = sp5kAfWinGet(frameid, SP5K_AF_G, afwin);
		}
			printf("Coring THRESHOLD [%3d],Coring VALUE [%3d], The Af value is %d\n",v[5], v[2] + (StepIntval * i), (afwin[0] + afwin[1] + afwin[2] + afwin[3] + afwin[4] + afwin[5] + afwin[6] + afwin[7] + afwin[8]));
		}
		
		sp5kAfCfgSet(SP5K_AF_NOISE_CORING, currCorThr);
		sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 200);
		sp5kAfCfgSet(SP5K_AF_CORING_VALUE, currCorVal);
		
		sp5kAfModeSet(SP5K_AF_MODE_OFF);
	
	}else if(strcmp(arg[1], "nosthrget") ==0){
			
			v[2] = ((v[2] > 80) ? 80 : v[2]);
			appAeCb_t *paeCb = appAeCbGet();
			Agc_ISO100 = paeCb->pviso;

			UINT32 v1 = ((UINT32)v[2]);
			UINT32 remainder = v1 - ((v1 / Agc_ISO100) * Agc_ISO100);
			UINT32 slop = NosSlope(v[2]);
			UINT32 NosThr = ((slop*remainder)>> 10) + NosCntTbl[(v[2]/Agc_ISO100)]; 
			printf("Slop = %d \n", slop);
			printf("((UINT8)v[2]) = %d, (v1 mod( Agc_ISO100)) = %d\n", v1, remainder);
			printf("NosThrGet(EXP = n/a, AGC = %d) = %d\n", v[2], NosThr);
	}else if(strcmp(arg[1], "nosthrset") ==0){
		if(strcmp(arg[2], "auto")==0) {
			IsKeyInNosThr = FALSE;
			return;
		}else{
			IsKeyInNosThr = TRUE;
			sp5kAfCfgSet(SP5K_AF_CORING_THR, v[2]);
		}

	}else if(strcmp(arg[1], "focusae") ==0){
		UINT32 NumAFGet = 0, StepIntval = 0;
		UINT8 i;
		SINT32 CurrExp;
		UINT32 CurrAgc;
		
		appAeEnable(1);
		sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 200);
		
		if(argc == 5 && v[2] >= 100 && v[4] >= 1)	//Continuously get AF value within a specific range.
				{	
					StepIntval = v[4];
					NumAFGet = (v[3] - v[2])/v[4];
				}else{
					return;
				}
				
				for(i = 0; i <= NumAFGet; i++)
				{
					printf("focus [%d] --> [%d], ", appLensQuery(LENS_QUERY_FOCUS_STEP_POS), v[2] + (StepIntval * i));
					if(StepIntval!= 0 || appLensQuery(LENS_QUERY_FOCUS_STEP_POS) != v[2]) {
					lensFocusJob( v[2] + (StepIntval * i));
					sp5kPreviewExpAgcGet(&CurrExp, &CurrAgc);
					printf("EXP/ AGC = %d, %d\n", CurrExp, CurrAgc);
					}
				}


	}
}

BOOL IsBracketMode(void){
	return IsBracket;
}
UINT8 appBracketIntervalGet(void){
	return BktInterval;
}
UINT8 appBracketSnapNumGet(void){
	return BktSnapNum;
}

