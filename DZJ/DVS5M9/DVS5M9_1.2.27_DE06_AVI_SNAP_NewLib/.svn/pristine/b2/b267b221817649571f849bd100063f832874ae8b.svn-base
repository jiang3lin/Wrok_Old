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
#define HOST_DBG 0
#include <math.h>
#include "app_com_def.h"
#include "app_com_api.h"

#include "sp5k_sensor_api.h"
#include "sp5k_capture_api.h"
#include "app_strobe.h"
#include "app_view_param_def.h"
#include "app_flash_param.h"
#include "app_flash_api.h"
#include "app_calib_api.h"
#include "app_aaa.h"
#include "app_lens_api.h"
#include "app_still.h"
#include "app_exif.h"
#include "middleware/Pwrlog.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define DBG_PREFLASH_CHECK_STROBE		1	/* shall be off @ release */

#define APP_FLASH_CURVE_ENTRY_NUM		16
#define APP_FLASH_CURVE_MAX_LUMA_TAG	690
#define APP_FLASH_CURVE_MAX_WIDTH_TAG	15000

#define a900_no_preflash	0
#define TOO_DARK_INCREASE_ISO_IN_PREFLASH 1
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef struct {
	UINT32 diffLuma;
	UINT32 pulseWidth;
} prefhCurvEntry_t;

enum {
	SNAPFLASH_ISO_50,
	SNAPFLASH_ISO_100,
	SNAPFLASH_ISO_200,
	SNAPFLASH_ISO_400,
	SNAPFLASH_ISO_800,
	SNAPFLASH_ISO_TOTAL,
} snapflashIsoIndex_t;

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

static appPrefhCb_t appPrefhCb;
static appPrefhCb_t cmdPrefhCb;
static appPrefhCb_t* prefhCbPtr;


#define CURVE_A_LUMA_LOW_LIMIT				109
#define CURVE_A_WIDTH_LOW_LIMIT				100
#define CURVE_A_LUMA_HIGH_LIMIT				673
#define CURVE_A_WIDTH_HIGH_LIMIT			660
#define CURVE_A_DEFAULT_LUMA				66
static prefhCurvEntry_t gcurveA[APP_FLASH_CURVE_ENTRY_NUM] =
{	/*-- X(deltaLumaLN), Y(PulseWidth) */
	{CURVE_A_LUMA_LOW_LIMIT, CURVE_A_WIDTH_LOW_LIMIT},	// effective lower bound
	{493, 210}, {586, 310}, {630, 370}, {650, 430}, //{650, 510},
	//{685, 810}, {690, 1110}, //{625, 15}, {650, 510}, {650, 510},
	{CURVE_A_LUMA_HIGH_LIMIT, CURVE_A_WIDTH_HIGH_LIMIT}, // effective upper bound
	{APP_FLASH_CURVE_MAX_LUMA_TAG, APP_FLASH_CURVE_MAX_WIDTH_TAG}, // table boundary tag
};

#define CURVE_B_DEFAULT_LUMA				66
#define CURVE_B_DIM_LUMA					116
#define CURVE_B_LUMA_LOW_LIMIT				CURVE_B_DEFAULT_LUMA
#define CURVE_B_WIDTH_LOW_LIMIT				APP_PREFLASH_DEFAULT_WIDTH
#define CURVE_B_LUMA_HIGH_LIMIT				771
#define CURVE_B_WIDTH_HIGH_LIMIT			11850
static prefhCurvEntry_t gcurveB[APP_FLASH_CURVE_ENTRY_NUM] =
{	/*-- X(deltaLumaLN), Y(PulseWidth) */
	{CURVE_B_LUMA_LOW_LIMIT, CURVE_B_WIDTH_LOW_LIMIT},	// effective lower bound
	{421, 1850}, {575, 2850}, {646, 3600}, {698, 4850},
	{740, 6600}, {755, 8600}, //{630, 2460}, {645, 3460},
	//{581, 760}, {630, 1210}, {665, 1910}, {681, 3660},
	{CURVE_B_LUMA_HIGH_LIMIT, CURVE_B_WIDTH_HIGH_LIMIT}, // effective upper bound
	{APP_FLASH_CURVE_MAX_LUMA_TAG, APP_FLASH_CURVE_MAX_WIDTH_TAG}, // table boundary tag
};

static prefhCurvEntry_t* curvTblPtr[]={ &gcurveA[0], &gcurveB[0], };
static UINT32 curvDefaultLuma[]=
{
	CURVE_A_DEFAULT_LUMA,
	CURVE_B_DEFAULT_LUMA,
};
static UINT32 gIsoFactor[]=
{	/* base=10 */
	5,	/* iso50 */
	10,	/* iso100 */
	20,	/* iso200 */
	40,	/* iso400 */
	80,	/* iso800 */
	160,	/* iso1600 */
	320,	/* iso3200 */
};

static UINT32 gZmFactor[] =
{	/* base=256 */
	256,	/* ZP1, wide */
	222,	/* ZP2 */
	194, /* ZP3 */
	167, /* ZP4 */
	127, /* ZP5 */
	104, /* ZP6 */
	67,	/* ZP7, tele */
};
#define SNAPFLASH_MAX_ISO_INDEX				SNAPFLASH_ISO_200
#define APP_SNAPFLASH_TARGET_LEVEL			150

#define PREFLASH_SCENE_ALMOST_DARK_LUMA		8

#define SNAPFLASH_LOOKUP_LUMA_MAX_THD		CURVE_B_LUMA_HIGH_LIMIT

static UINT32 gsnapfhIsoToAgcLUT[SNAPFLASH_ISO_TOTAL]=
{
	44,			/*iso50*/
	44+(16*1),	/*iso100*/
	44+(16*2),	/*iso200*/
	44+(16*3),	/*iso400*/
	44+(16*4),	/*iso800*/
};

//static UINT32 LenLumaFactor[]={1024, 1024, 1024, 1024, 1024, 1024, 1024};


static unsigned snapfhAeCnt=0;
static pfhDbgInfoSt_t gstPfhDbg;
			/*-- For Preflash Debug Info, to embedded in EXIF MakerNote */

UINT32 myTestSnapWidth=0;
			
UINT32 sh_mode[2][2]={/*auto, force*/{SHUTTER_1_30S, SHUTTER_1_2000S},/*slow sync*/{SHUTTER_1S, SHUTTER_1_2000S}}; 

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern SINT32 pv_agcidx_to_svidx(SINT32 agcidx);
extern SINT32 cap_agcidx_to_svidx(SINT32 agcidx);
extern SINT32 cap_svidx_to_agcidx(SINT32 svidx);
extern SINT32 pv_tvidx_to_expidx(SINT32 tvidx);
extern SINT32 pv_svidx_to_agcidx(SINT32 svidx);
extern UINT32  appExif_MN_Printf(UINT8 bufID, const char *szFmt, ...);
extern void appExif_MN_BufClear(UINT8 bufID);
extern SINT32 appAEALGLib_ResultGet(UINT32 param, aeResult_t *presult);
extern void appAeFlashAdjust(aeResult_t* pAePar,SINT32* ae_suggest_exp,UINT32* ae_suggest_iso,UINT32 sh_long,UINT32 sh_short);

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

appPrefhCb_t* appPrefhCbGet(void)
{
	return prefhCbPtr;
}

void *appPreflashDebugInfoGet(void)
{
	return (void *)&gstPfhDbg;
}

static prefhCurvEntry_t* _preflashCurveGet(void)
{
	return curvTblPtr[prefhCbPtr->whichCurvTbl];
}

static UINT8 _preflashCurveSelect(UINT32 diffLuma)
{
	/*-- TODO : For CLOSE UP situation */
	prefhCbPtr->whichCurvTbl = 1; /* default to use normal curve */
	return prefhCbPtr->whichCurvTbl;
}

/*-------------------------------------------------------------------------
 * Function Name : appPreflashLuma2WidthLookup
 *------------------------------------------------------------------------*/
UINT32 appPreflashLuma2WidthLookup(UINT32 inLuma)
{
	int i;
	UINT32 hiLuma, loLuma;
	UINT32 hiWidth, loWidth;
	UINT32 retWidth;
	float floTmp;
	prefhCurvEntry_t* pentry;

	DBG_PRINT("Luma2Width Lookup : Using table %d ....\n", prefhCbPtr->whichCurvTbl);

	pentry = _preflashCurveGet();
	prefhCbPtr->whichEntry = 0;

	/*-- boundary condition */
	if (inLuma <= pentry->diffLuma) {
		retWidth = pentry->pulseWidth;
		prefhCbPtr->whichEntry = -1;
		DBG_PRINT("Got MIN Width (%d)...\n", retWidth);
		return retWidth;
	}

	retWidth = pentry->pulseWidth;
	/*-- piece-wise linear */
	for (i=0; i<APP_FLASH_CURVE_ENTRY_NUM-1; i++) {
		hiLuma = (pentry+i+1)->diffLuma; //gcuvTbl[i+1][CURVTBL_IDX_LUMA];
		loLuma = (pentry+i)->diffLuma; // gcuvTbl[i][CURVTBL_IDX_LUMA];

		if (inLuma >= hiLuma && hiLuma==APP_FLASH_CURVE_MAX_LUMA_TAG) {
			retWidth = APP_FLASH_CURVE_MAX_WIDTH_TAG;
			prefhCbPtr->whichEntry = -128;
			DBG_PRINT("Got MAX Width (%d)...\n", retWidth);
			break;
		}

		if (inLuma<hiLuma && inLuma>=loLuma) {
			hiWidth = (pentry+i+1)->pulseWidth; //gcuvTbl[i+1][CURVTBL_IDX_WIDTH];
			loWidth = (pentry+i)->pulseWidth; //gcuvTbl[i][CURVTBL_IDX_WIDTH];
			floTmp = ((float)(inLuma-loLuma))/(hiLuma-loLuma);
			floTmp = floTmp*(hiWidth-loWidth);
			retWidth = ((UINT32)(floTmp))+loWidth;
			prefhCbPtr->whichEntry = i;
			DBG_PRINT("Got CAL Width (%d)...\n", retWidth);
			break;

		}
	}

	#if 0
	if (retWidth==0) {
		printf("%s: !!FATAL!!, Zeor retWidth !! \n", __FUNCTION__);
	}
	#endif

	retWidth = (retWidth>=APP_FLASH_CURVE_MAX_WIDTH_TAG) ? APP_FLASH_CURVE_MAX_WIDTH_TAG: retWidth;

	return retWidth;
}

/*-------------------------------------------------------------------------
 * Function Name : appPreflashWidth2LumaLookup
 *------------------------------------------------------------------------*/
#if 0
UINT32 appPreflashWidth2LumaLookup(UINT32 inWidth)
{
	int i;
	UINT32 hiLuma, loLuma;
	UINT32 hiWidth, loWidth;
	UINT32 retLuma;
	float floTmp;
	prefhCurvEntry_t* pentry;

	DBG_PRINT("%s: inWidth(%d) \n", __FUNCTION__, inWidth);
	DBG_PRINT("Luma2Width Lookup : Using table %d ....\n", prefhCbPtr->whichCurvTbl);

	pentry = _preflashCurveGet();

	/*-- boundary condition */
	if (inWidth <= pentry->pulseWidth) { //gcuvTbl[0][CURVTBL_IDX_WIDTH]) {
		retLuma = pentry->diffLuma; //gcuvTbl[0][CURVTBL_IDX_LUMA];
		DBG_PRINT("Got MIN Luma (%d)...\n", retLuma);
		return retLuma;
	}

	retLuma = 0;
	/*-- piece-wise linear */
	for (i=0; i<APP_FLASH_CURVE_ENTRY_NUM-1; i++) {
		hiWidth = (pentry+i+1)->pulseWidth; //gcuvTbl[i+1][CURVTBL_IDX_WIDTH];
		loWidth = (pentry+i)->pulseWidth; //gcuvTbl[i][CURVTBL_IDX_WIDTH];

		if (inWidth >= hiWidth && hiWidth==APP_FLASH_CURVE_MAX_WIDTH_TAG) {
			retLuma = (pentry+i)->diffLuma; //gcuvTbl[i][CURVTBL_IDX_LUMA]; //APP_FLASH_CURVE_MAX_LUMA_TAG
			DBG_PRINT("Got MAX Luma (%d)...\n", retLuma);
			break;
		}

		if (inWidth<hiWidth && inWidth>=loWidth) {
			hiLuma = (pentry+i+1)->diffLuma; //gcuvTbl[i+1][CURVTBL_IDX_LUMA];
			loLuma = (pentry+i)->diffLuma; //gcuvTbl[i][CURVTBL_IDX_LUMA];
			floTmp = ((float)(inWidth-loWidth))/(hiWidth-loWidth);
			floTmp = floTmp*(hiLuma-loLuma);
			retLuma = ((UINT32)(floTmp))+loLuma;
			DBG_PRINT("Got CAL Luma (%d)...\n", retLuma);
			break;

}
	}

	#if 0
	if (retLuma==0) {
		printf("%s: !!FATAL!!, Zeor Luma !! \n", __FUNCTION__);
	}
	#endif

	retLuma = (retLuma>APP_FLASH_CURVE_MAX_LUMA_TAG) ? APP_FLASH_CURVE_MAX_LUMA_TAG : retLuma;

	return retLuma;
}
#endif

UINT32 appSnapflashCfgHPosGet(void)
{
	return prefhCbPtr->snapHPos;
}

UINT32 appPreflashCfgHPosGet(void)
{
	return prefhCbPtr->hPos;
}

UINT32 appPreflashCfgWidthGet(void)
{
	return prefhCbPtr->pulseWidth;
}

#if 0
UINT32 appPreflashCfgLumaGet(UINT32 diffLuma)
{


	if (prefhCbPtr->whichCurvTbl >= sizeof(curvDefaultLuma)/sizeof(curvDefaultLuma[0]))  {
		return 0;
	}

	prefhCbPtr->refDiffLuma = curvDefaultLuma[prefhCbPtr->whichCurvTbl];
	return prefhCbPtr->refDiffLuma;
}
#endif

void appPreflashCfgExpAgcGet(UINT32 *pfExp, UINT32 *pfAgc)
{
	*pfExp = prefhCbPtr->prefhExp;
	*pfAgc = prefhCbPtr->prefhAgc;
}
void appPreflashCfgExpAgcSet(UINT32 pfExp, UINT32 pfAgc)
{
	prefhCbPtr->prefhExp = pfExp;
	prefhCbPtr->prefhAgc = pfAgc;
}


/*-------------------------------------------------------------------------
 * Function Name : appSnapflashTargetSet
 *------------------------------------------------------------------------*/
void appSnapflashTargetSet(UINT32 luma)
{
	prefhCbPtr->snapTarget = luma;
}

/*-------------------------------------------------------------------------
 * Function Name : appSnapflashTargetGet
 *------------------------------------------------------------------------*/
UINT32 appSnapflashTargetGet(void)
{
	return prefhCbPtr->snapTarget;
}

#if 0
void appPreflashResultParamGet(UINT16 *env_luma, UINT16 *pfh_luma)
{
	*env_luma = prefhCbPtr->sceneLuma;
	*pfh_luma = prefhCbPtr->prefhLuma;
}
#endif


UINT32 appPreflashStrobeCharge(BOOL forceChg)
{
	if (forceChg) {
		if (FAIL==appStrobeChargeStart()) {
			printf("\n__FATAL__, STRB chg failed !!\n\n");
			return FAIL;
		}
	}

	do {
		APP_SLEEP_MS(10);
		//printf("++.");
	} while((appStrobeQueryReady()==FALSE));

	appStrobeChargeStop();

	DBG_PRINT(">>> STROBE Ready !! \n");
	return SUCCESS;
}

void appRedEyeFlashInit(void)
{
	UINT32 msg, param;
	UINT16 pulseArray[]={400,200,0,};/* wait400ms, 300 unit pulse */

	sp5kRedEyeFlashCfgSet(1,pulseArray);
	sp5kRedEyeFlashTrig(SP5K_REDEYE_TRIG_SYNC_FRAME);

	do {
		sp5kHostMsgReceive(&msg, &param);
		if (msg==SP5K_MSG_REDEYE_FLASH_FINISHED) {
			break;
		}
	} while(1);
}
void appShowCurExpAgc(void)
{
	SINT32 expIdx;
	UINT32 agcIdx;

	sp5kPreviewExpAgcGet(&expIdx, &agcIdx);
	printf("sp5k view (exp=%d, agc=%d)\n", expIdx, agcIdx);

	sp5kStillCapExpAgcGet(&expIdx, &agcIdx);
	printf("sp5k cap (exp=%d, agc=%d)\n", expIdx, agcIdx);
}


/*-------------------------------------------------------------------------
 * Function Name : _preflashDiffLumaCalc
 *------------------------------------------------------------------------*/
static UINT32 _preflashDiffLumaCalc(void)
{
	UINT32 ret;
	if (prefhCbPtr->sceneLuma < PREFLASH_SCENE_ALMOST_DARK_LUMA) {
		ret = prefhCbPtr->prefhLuma; // + PREFLASH_SCENE_ALMOST_DARK_LUMA;
	}
	else if (prefhCbPtr->prefhLuma <= prefhCbPtr->sceneLuma) {
		ret = 1;
	}
	else {
		ret = prefhCbPtr->prefhLuma - prefhCbPtr->sceneLuma;
	}
	return ret;
}

/*-------------------------------------------------------------------------
 * Function Name : _preflashResultLumaGet
 * !!!!! Temporary for experiment, shall revise for formal release !!!!
 *------------------------------------------------------------------------*/
UINT16 _preflashResultLumaGet(void)
{
	UINT32 msg, param;
	sp5kAeResult_t aeResult;
	sp5kAwbResult_t awbResult;
	UINT16 aeLuma;
	UINT32 ret;

	aeLuma= 0;
	do {
		sp5kHostMsgReceive(&msg, &param);
		if (msg==SP5K_MSG_PREFLASH_READY) {
			//printf("... PREFLASH_RDY @ %d \n", appMsTimeGet());
			//printf(" .. ");
			ret = sp5kPreflashResultGet(param, &aeResult, &awbResult);
			if (ret!=0) {
				printf("\n__FATAL__ sp5kPreflashResultGet failed !!! \n\n");
				break;
			}
			if (aeResult.valid & AAA_INFO_RESULT_LUMA_VALID) {
				//printf(" pfh ");
				aeLuma = aeResult.aeLuma;
			} else {
				printf("\n__WARNING__ Preflash luma invalid !! \n\n");
				aeLuma = 0;
			}
			break;
		}
	} while(1);

	return aeLuma;
}

/*-------------------------------------------------------------------------
 * Function Name : _preflashSceneLumaGet
 * !!!!! Temporary for experiment, shall revise for formal release !!!!
 *------------------------------------------------------------------------*/
static UINT16 _preflashSceneLumaGet(void)
{
	UINT32 osTime, aeTimeOut=1500;
	UINT32 msg, param, ret;
	sp5kAeResult_t aeResult;
	UINT16 aeLuma;
	//SINT32 ae_suggest_exp;
	//UINT32 ae_suggest_iso;
	UINT32 sh_idx = prefhCbPtr->snapFlashMode == SP5K_SNAP_FLASH_MODE_FORCED_ON_REAR_SYNC? 1: 0;

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
			
			if (ret!=0) {
				DBG_PRINT("\n__WARNING__ aeLuma outdated !!! \n\n");
				continue;
			}

			if (aeResult.valid & AAA_INFO_RESULT_LUMA_VALID) {
				//printf(" ae ");
				aeLuma = aeResult.aeLuma;
				appAeFlashAdjust(&aeResult, \
					&(prefhCbPtr->ae_exp), \
					&(prefhCbPtr->ae_agc), \
					sh_mode[sh_idx][0],\
					sh_mode[sh_idx][1]\
					);
				printf("ae suggest exp/agc %d/%d\n", prefhCbPtr->ae_exp, prefhCbPtr->ae_agc);
				break;
			}
		}
		/*-- Time out check */
		if (aeTimeOut) {
			if(aeTimeOut < (appMsTimeGet()-osTime)) {
				printf("\n__FATAL__ %s timed out !!\n\n", __FUNCTION__);
				break;
			}
		}
	} while(1);

	return aeLuma;
}

/*-------------------------------------------------------------------------
 *  Function Name : aeExif_DbgLogInit
 *------------------------------------------------------------------------*/
void flashExif_DbgLogInit(void)
{
	appExif_MN_BufClear(EXIF_MN_PFH);
}
/*-------------------------------------------------------------------------
 *  Function Name : aeExif_DbgLogSave
 *------------------------------------------------------------------------*/
void flashExif_DbgLogSave(void)
{
	appExif_MN_Printf(EXIF_MN_PFH, "fired=%d mode=%d target(%d) ", (prefhCbPtr->isDummyFlash?0:1), prefhCbPtr->snapFlashMode, prefhCbPtr->snapTarget);
	appExif_MN_Printf(EXIF_MN_PFH, "pvexp=%d pvagc=%d ", prefhCbPtr->prefhExp, prefhCbPtr->prefhAgc);
	appExif_MN_Printf(EXIF_MN_PFH, "tbl=%d entry=%d ", prefhCbPtr->whichCurvTbl, prefhCbPtr->whichEntry);
	appExif_MN_Printf(EXIF_MN_PFH, "envL=%d pfhL=%d ", prefhCbPtr->sceneLuma, prefhCbPtr->prefhLuma);
	appExif_MN_Printf(EXIF_MN_PFH, "diffL=%d tblL=%d ", prefhCbPtr->prefhDiffLuma, prefhCbPtr->refDiffLuma);
	appExif_MN_Printf(EXIF_MN_PFH, "capExp=%d capAgc=%d ", prefhCbPtr->capfhExp, prefhCbPtr->capfhAgc);
	appExif_MN_Printf(EXIF_MN_PFH, "isoidx=%d maxIsoIdx=%d ", prefhCbPtr->snapIsoIdx, prefhCbPtr->snapMaxIsoIdx);
	appExif_MN_Printf(EXIF_MN_PFH, "calcL=%d lookupW=%d ", prefhCbPtr->lookupDiffLuma, prefhCbPtr->snapWidth);
	appExif_MN_Printf(EXIF_MN_PFH, "fillflash =%d/100 ", prefhCbPtr->fillFlashPercent);
}


/*-------------------------------------------------------------------------
 *  Function Name : _snapflashAeConfig
 *------------------------------------------------------------------------*/
static void _snapflashAeConfig(void)
{
	UINT8 isoIdx;

	/*-- Get AE settings */
	appAeCb_t *paeCb;
	paeCb = appAeCbGet();

	/*-- ISO setup */
	isoIdx = paeCb->aeset.iso;
	if (isoIdx==UI_ISO_AUTO) {
		prefhCbPtr->snapMaxIsoIdx = SNAPFLASH_MAX_ISO_INDEX;
		prefhCbPtr->isAutoIso = 1;
	} else  {
		prefhCbPtr->snapMaxIsoIdx = isoIdx-1;
		prefhCbPtr->isAutoIso = 0;
	}
	prefhCbPtr->snapIsoIdx = 0; /* reset ISO idx */

}

/*-------------------------------------------------------------------------
 *  Function Name : _snapflashAeLookupDiffLumaCalc
 *------------------------------------------------------------------------*/
static UINT32 _snapflashAeLookupDiffLumaCalc(UINT32 isoF)
{
	UINT32 calcResult;

	/*-- formula :
	 * lookupDiffLuma@refISO
	 *		= snapExpectDiffLuma@refISO * rebound_coefficient
	 *		= [snapTargetLuma - sceneLuma@xISO)/isoFactor] *flRebound
	 *		= {[snapTargetLuma - (sceneLuma@refISO*isoFactor)]/isoFactor} *flRebound
	 *		= [ (snapTargetLuma/isoFactor) - sceneLuma@refISO ] * flRebound
	 */
	calcResult = (prefhCbPtr->snapTarget*10)/isoF;
	//DBG_PRINT("snapTargetLuma@refISO=%d \n", calcResult);

	calcResult -= (calcResult>prefhCbPtr->sceneLuma)
					? prefhCbPtr->sceneLuma : (calcResult-1);
	//DBG_PRINT("snapExpectDiffLuma@refISO=%d \n", calcResult);

	calcResult = (UINT32)(((float)calcResult)*prefhCbPtr->flRebound);
	//DBG_PRINT("lookupDiffLuma@refISO =%d \n", calcResult);

	return calcResult;
}

/*-------------------------------------------------------------------------
 *  Function Name : _snapflashAeParaCalc
 *------------------------------------------------------------------------*/
static void _snapflashAeParaCalc(UINT8 isoidx, UINT8 isautoiso)
{
	UINT32 isoFactor;
	UINT32 lookupDiffLuma; /* the diff luma@refIso to lookup the pulse width */

	printf("<fA %d isoidx=%d *****\n",snapfhAeCnt, isoidx);
	snapfhAeCnt++;

	isoFactor = gIsoFactor[isoidx];
	lookupDiffLuma = _snapflashAeLookupDiffLumaCalc(isoFactor);
	printf("isoF=%d lookupDiffLuma=%d > MaxThd(%d)\n", isoFactor, lookupDiffLuma, SNAPFLASH_LOOKUP_LUMA_MAX_THD);

	if (lookupDiffLuma>SNAPFLASH_LOOKUP_LUMA_MAX_THD
		&& isoidx<prefhCbPtr->snapMaxIsoIdx
		&& isautoiso)
	{
		isoidx++;
		_snapflashAeParaCalc(isoidx, 1);
	}
	else
	{
		prefhCbPtr->snapIsoIdx = isoidx;
		prefhCbPtr->lookupDiffLuma = lookupDiffLuma;

		//prefhCbPtr->capfhExp = prefhCbPtr->prefhExp;
		prefhCbPtr->capfhAgc = gsnapfhIsoToAgcLUT[isoidx];

		printf("SET: isoidx=%d capfhAgc=%d capfhExp=%d lookupDiffLuma=%d \n ****>\n\n", isoidx, prefhCbPtr->capfhAgc, prefhCbPtr->capfhExp, lookupDiffLuma);
	}
}

/*-------------------------------------------------------------------------
 *  Function Name : _snapflashAeSet
 *------------------------------------------------------------------------*/
static void _snapflashAeSet(void)
{
	SINT32 expidx_diff, r;
	UINT32 x,y;
		
	printf("\n*** snapflash AE \n");
	snapfhAeCnt = 0;

#if 0
	if(prefhCbPtr->snapFlashMode == SP5K_SNAP_FLASH_MODE_FORCED_ON_REAR_SYNC) {
		prefhCbPtr->capfhExp = prefhCbPtr->prefhExp-16;
		prefhCbPtr->sceneLuma*=2;
	}
	else {
		prefhCbPtr->capfhExp = prefhCbPtr->prefhExp;
	}
#else
	prefhCbPtr->capfhExp = prefhCbPtr->ae_exp;
	if (prefhCbPtr->sceneLuma > PREFLASH_SCENE_ALMOST_DARK_LUMA && prefhCbPtr->ae_exp != prefhCbPtr->prefhExp) {
		expidx_diff = prefhCbPtr->prefhExp - prefhCbPtr->ae_exp;		
		r = power2xyND(expidx_diff, 16, &x, &y);				
		printf("sceneluma %d\n", prefhCbPtr->sceneLuma);
		if(y) {
			prefhCbPtr->sceneLuma = prefhCbPtr->sceneLuma*x/y;
			printf("scene %d %d/1000\n", prefhCbPtr->sceneLuma, x*1000/y);
		}
		printf("sceneluma %d  ae_exp %d x %d, y %d\n", prefhCbPtr->sceneLuma, prefhCbPtr->ae_exp,x,y);			
	}
	
	// To find the main flash triger time point, hpos. The unit is 10us.		
	if(STD_EXP_1_32 > prefhCbPtr->capfhExp) { 		
		// long exp than 1/30s use rear curtain.
		r = power2xyND(-(prefhCbPtr->capfhExp), 16, &x, &y);
		prefhCbPtr->snapHPos = APP_SNAPFLASH_DEFAULT_POS + x*100000/y - 500; // sensor raw reset + exp - buffer time.
		printf("exp %d, hpos %d\n", x*100000/y, prefhCbPtr->snapHPos);
	}
	else {
		// short exp use front curtain.
		prefhCbPtr->snapHPos = APP_SNAPFLASH_DEFAULT_POS;
	}	
#endif
	
	if (prefhCbPtr->isAutoIso) {
		/*-- AUTO ISO */
		_snapflashAeParaCalc(0, 1);
	} else {
		_snapflashAeParaCalc(prefhCbPtr->snapMaxIsoIdx, 0);
		/*-- USER ISO constrained */
		printf("\n\n____ Not yet ready !! ___\n\n");
		return;
	}
	
	//debug.
	x = prefhCbPtr->lookupDiffLuma/prefhCbPtr->flRebound*gIsoFactor[prefhCbPtr->snapIsoIdx]/10;
	y = prefhCbPtr->sceneLuma*gIsoFactor[prefhCbPtr->snapIsoIdx]/10;	
	prefhCbPtr->fillFlashPercent = x*100/(x+y);
}

/*-------------------------------------------------------------------------
 *  Function Name : _snapflashIso2AgcInit
 *------------------------------------------------------------------------*/
static void _snapflashIso2AgcInit(void)
{
	/*-- Probably, you have to manually modify the value by sensor.
	 * For example, PANA-10M !?
	 */
	UINT32 minIsoAgc;
	unsigned i;

	minIsoAgc = cap_svidx_to_agcidx(STD_ISO_50);
	for (i=0; i<SNAPFLASH_ISO_TOTAL; i++) {
		gsnapfhIsoToAgcLUT[i] = minIsoAgc+(SV_UNIT*i);
	}
}



/*-------------------------------------------------------------------------
 * Function Name : appSnapFlashInstall
 *------------------------------------------------------------------------*/

void appSnapFlashInstall(UINT32 mode, UINT32 pulseW, UINT32 hPos)
{
	//sp5kSnapFlashCfgSet(SP5K_SNAP_FLASH_CFG_MODE, mode);
	sp5kSnapFlashCfgSet(SP5K_SNAP_FLASH_CFG_WIDTH, pulseW);
	hPos =100;
	sp5kSnapFlashCfgSet(SP5K_SNAP_FLASH_CFG_H_POS, hPos);
	#if (_CAM_A900_EVM)||(_CAM_M900_EVM)
	// we may need this for all cmos sensor.
	// if we use timer, the hpos will be 10us unit input.
	/*sp5kSnapFlashCfgSet(SP5k_SNAP_FLASH_CFG_DELAY_TYPE, SP5K_FLASH_DELAY_TYPE_IMMEDIATE_TIMER);*/
	sp5kSnapFlashCfgSet(SP5k_SNAP_FLASH_CFG_DELAY_TYPE, SP5K_FLASH_DELAY_TYPE_VD_TIMER);
	/*sp5kSnapFlashCfgSet(SP5k_SNAP_FLASH_CFG_DELAY_TYPE, SP5K_FLASH_DELAY_TYPE_H_POS);*/
	#endif
	printf("this mode=%d, width=%d, hpos=%d, \n", prefhCbPtr->snapFlashMode, pulseW, hPos);

	sp5kStillCapExpAgcSet(prefhCbPtr->capfhExp, prefhCbPtr->capfhAgc);

	#if DEBUG_PREFH_USE_DATE_STAMP
	if (prefhCbPtr->isStampDbg==TRUE) 
	{
		extern sp5kCaptureStamp_t gStampInfo;
		sp5kCaptureStamp_t dbgStamp;
		UINT8 szDbgInfo[20];
		memset(szDbgInfo, 0, 20);
		sprintf(szDbgInfo, "%1d/%3d/%3d-%5d/%1d",
				prefhCbPtr->whichCurvTbl,
				prefhCbPtr->prefhDiffLuma,
				prefhCbPtr->lookupDiffLuma,
				prefhCbPtr->snapWidth,
				prefhCbPtr->snapIsoIdx
				);
		memcpy(&dbgStamp, &gStampInfo, sizeof(sp5kCaptureStamp_t));
		dbgStamp.dateFmtId = SP5K_CAPTURE_DATE_CUSTOM;
		dbgStamp.pdate = szDbgInfo;
		dbgStamp.dateLen = 20;
		dbgStamp.scaleRatio = 400;
		dbgStamp.x -= 600;
		dbgStamp.y -= 20;

		sp5kStillCapCfgSet(SP5K_CAPTURE_STAMP, &dbgStamp);
	}
	#endif
}

/*-------------------------------------------------------------------------
 * Function Name : appSnapFlashDismiss
 *------------------------------------------------------------------------*/
void appSnapFlashDismiss(void)
{
	sp5kSnapFlashCfgSet(SP5K_SNAP_FLASH_CFG_MODE, SP5K_SNAP_FLASH_MODE_OFF);
}

/*-------------------------------------------------------------------------
 *  Function Name : appSnapFlashCalculate
 	Description: How much duration of flash need for snap is according to the
 		luma delta. Our luma delta to flash duration table is built under ISO50/
 		Lens Wide/Large Aperture/EV+0. So we need to transfer the preflash luma
 		to equivlant luma of default case.
 	- Lens: This will decrease the illumination of sensor, so we should compensate
 		it back to segment-0.
 	- ISO: This does not be applied in view, so we should mimic its effect on luma.
 	- Others: ?
 *------------------------------------------------------------------------*/
UINT32 appSnapFlashCalculate(UINT16 snapLuma)
{
	UINT32 cur_zm_seg, zm_seg_max, ref_diff_luma;
	/*-- Get preflash diff luma  */
	prefhCbPtr->prefhDiffLuma = _preflashDiffLumaCalc();

	/*-- TODO :
	 * Check CLOSE-UP situation, and get appropriate curve
	 */
	_preflashCurveSelect(prefhCbPtr->prefhDiffLuma);

	/*-- Calculate the rebounding Factor :
	 * reboundFactor = refDiffLuma/(prefhDiffLuma@refISO)
	 */
	cur_zm_seg = appLensQuery(LENS_QUERY_ZOOM_SEG_POS);
	zm_seg_max = appLensQuery(LENS_QUERY_ZOOM_SEG_NUM);
	if (cur_zm_seg > zm_seg_max-1 )
	{
		printf("\n!!Illegal Zoom position !!\n\n");
		cur_zm_seg = 0;
	}
	ref_diff_luma = prefhCbPtr->refDiffLuma;
	if (cur_zm_seg>0)
	{
		ref_diff_luma = (gZmFactor[cur_zm_seg]*ref_diff_luma)>>8;
	}
#if a900_no_preflash
	prefhCbPtr->flRebound=1;
#else 
	prefhCbPtr->flRebound = ((float)(prefhCbPtr->refDiffLuma))/prefhCbPtr->prefhDiffLuma;
#endif

	prefhCbPtr->trueRefDiffLuma = ref_diff_luma;
	prefhCbPtr->zm_seg = (UINT8) cur_zm_seg;

	/*-- Config for the FlashAE followed */
	_snapflashAeConfig();

	/*--Snap Flash AE */
	_snapflashAeSet();

	/*-- lookup for snapflash pulse width */	
	if(myTestSnapWidth) {
		prefhCbPtr->snapWidth = myTestSnapWidth;
	}	
	else {
		prefhCbPtr->snapWidth = appPreflashLuma2WidthLookup(prefhCbPtr->lookupDiffLuma);
	}
	
	#if 0
	printf("\n********************************\n");
	printf("refDiffLuma(%d), diffLuma(%d) \n", prefhCbPtr->refDiffLuma, prefhCbPtr->prefhDiffLuma);
	printf("targetLuma(%d) \n", snapLuma);
	printf("calcLuma(%d) \n\n", prefhCbPtr->lookupDiffLuma);
	#endif

	return prefhCbPtr->snapWidth;
}

/*-------------------------------------------------------------------------
 *  Function Name : appSnapFlashSlowSyncExpLimitSet
 *------------------------------------------------------------------------*/
void appSnapFlashSlowSyncExpLimitSet(UINT16 longest_exp)
{
	sh_mode[1][0] = longest_exp;
}

/*-------------------------------------------------------------------------
 *  Function Name : appSnapFlashUiModeSet
 *------------------------------------------------------------------------*/
void appSnapFlashUiModeSet(UINT16 ui_mode)
{
	prefhCbPtr->ShutterLimmit = (ui_mode == UI_FLASH_SLOW_SYNC)? sh_mode[1][0] : sh_mode[0][0];
}

/*-------------------------------------------------------------------------
 * Function Name : appPreflashTrig
 *------------------------------------------------------------------------*/
void appPreflashTrig(void)
{
	UINT32 isLumaLessThan8Case=0;
	#if (DBG_PREFLASH_CHECK_STROBE)
	/*-- Strobe control */
	if (appStrobeQueryReady()==FALSE) {
		appPreflashStrobeCharge(TRUE);
	}
	#endif

	/*-- speed up AE_ACCUM */
	sp5kAeCfgSet(SP5K_AE_ACCUM_PERIOD, 1);

	/*-- Backup current view settings */
	sp5kPreviewExpAgcGet(&prefhCbPtr->pvExp, &prefhCbPtr->pvAgc);

	/*-- Configure the exp/agc for preflash strobe */
	sp5kPreviewExpAgcSet(prefhCbPtr->prefhExp, prefhCbPtr->prefhAgc);
	sp5kSensorVvalidWait();

	/*-- Catch Scene Luma prior preflash */
	prefhCbPtr->sceneLuma = _preflashSceneLumaGet();
			/*-- blocking and has the potential risk
			 * to lose host message !!
			 */
#if TOO_DARK_INCREASE_ISO_IN_PREFLASH
	/* If it is too dark the base is 8. We should increase the iso to detect the level under 8. */		 
	if (prefhCbPtr->sceneLuma <= PREFLASH_SCENE_ALMOST_DARK_LUMA) {
		isLumaLessThan8Case=1;
		sp5kPreviewExpAgcSet(prefhCbPtr->prefhExp, prefhCbPtr->prefhAgc+32); //x4
		sp5kSensorVvalidWait();
		prefhCbPtr->sceneLuma = _preflashSceneLumaGet();
		if (prefhCbPtr->sceneLuma <= PREFLASH_SCENE_ALMOST_DARK_LUMA) {
			prefhCbPtr->sceneLuma=0;
		}
		else {
			prefhCbPtr->sceneLuma /= 4;
		}
	}	
#endif	
	//if (prefhCbPtr->sceneLuma <= PREFLASH_SCENE_ALMOST_DARK_LUMA) {
	//	prefhCbPtr->refDiffLuma = CURVE_B_DIM_LUMA;
	//	prefhCbPtr->pulseWidth = APP_PREFLASH_DIM_WIDTH;
	//}
	//else {	
		prefhCbPtr->refDiffLuma = CURVE_B_DEFAULT_LUMA;
		prefhCbPtr->pulseWidth = APP_PREFLASH_DEFAULT_WIDTH;
	//}	
	sp5kPreflashCfgSet(SP5K_PREFLASH_CFG_WIDTH, prefhCbPtr->pulseWidth);

	/*-- Trigger preflash */
	if (isLumaLessThan8Case) {
		sp5kPreviewExpAgcSet(prefhCbPtr->prefhExp, prefhCbPtr->prefhAgc+32);		
	}
	else {
		sp5kPreviewExpAgcSet(prefhCbPtr->prefhExp, prefhCbPtr->prefhAgc);//in scen luma get, the exp/agc will be changed.
	}	
	appStrobePowerOn();
	sp5kPreflashTrig();

	/*-- Get preflash result */
	prefhCbPtr->prefhLuma = _preflashResultLumaGet();
			/*-- blocking and has the potential risk
			 * to lose host message !!
			 */
	if (isLumaLessThan8Case) {		 
		if (prefhCbPtr->prefhLuma <= PREFLASH_SCENE_ALMOST_DARK_LUMA) {
			prefhCbPtr->prefhLuma = 1;
		}
		else {
			prefhCbPtr->prefhLuma /= 4;
		}
	}	

	/*-- Restore preview settings */
	sp5kAeCfgSet(SP5K_AE_ACCUM_PERIOD, 3);

	#if HOST_DBG
	printf("\n====== Preflash ===========\n");
	printf("envLuma (%d) pfhLuma(%d) \n\n", prefhCbPtr->sceneLuma, prefhCbPtr->prefhLuma);
	#endif

	#if (DBG_PREFLASH_CHECK_STROBE)
	/*-- Strobe control */
	if (appStrobeQueryReady()==FALSE) {
		appPreflashStrobeCharge(TRUE);
	}
	#endif

	#if PREFH_EXIF_DEBUG
	gstPfhDbg.viewExpIdx = prefhCbPtr->pvExp;
	gstPfhDbg.viewAgcIdx = prefhCbPtr->pvAgc;
	#endif
}

/*-------------------------------------------------------------------------
 * Function Name : appPreflashConfig
 *------------------------------------------------------------------------*/
void appPreflashConfig(UINT32 sp5kFlashMode)
{
	prefhCbPtr->snapFlashMode = sp5kFlashMode;
	sp5kSnapFlashCfgSet(SP5K_SNAP_FLASH_CFG_MODE, sp5kFlashMode);	

	/*-- Retrieve & Set preflash settings */
	sp5kPreflashCfgSet(SP5K_PREFLASH_CFG_H_POS, prefhCbPtr->hPos);
	sp5kPreflashCfgSet(SP5K_PREFLASH_CFG_WIDTH, prefhCbPtr->pulseWidth);
}

/*-------------------------------------------------------------------------
 * Function Name : appPreflashInit
 *------------------------------------------------------------------------*/
void appPreflashInit(void)
{
	/*-- initialize appPrefhCb */
	prefhCbPtr = &appPrefhCb;

	prefhCbPtr->prefhExp = pv_tvidx_to_expidx(80);
	prefhCbPtr->prefhAgc = pv_svidx_to_agcidx(80);
	prefhCbPtr->hPos = APP_PREFLASH_DEFAULT_POS;
	prefhCbPtr->hPosLimit = APP_PREFLASH_POS_MAX;
	prefhCbPtr->pulseWidth = APP_PREFLASH_DEFAULT_WIDTH;

	prefhCbPtr->snapHPos = APP_SNAPFLASH_DEFAULT_POS;
	prefhCbPtr->snapTarget = APP_SNAPFLASH_TARGET_LEVEL;

	prefhCbPtr->curvTblNum = sizeof(curvDefaultLuma)/sizeof(curvDefaultLuma[0]);
	prefhCbPtr->whichCurvTbl = 0;
	prefhCbPtr->refDiffLuma = curvDefaultLuma[0];

	_snapflashIso2AgcInit();

	#if DEBUG_PREFH_USE_DATE_STAMP
	{
		appCalibData_t* pcalib;
		pcalib = appCalibDataGet();
		prefhCbPtr->isStampDbg = !!pcalib->prefhStamp;
	}
	#endif

	sp5kFlashlightInit(SP5K_FLASHLIGHT_INACTIVE_LO);
	sp5kSnapFlashCfgSet(SP5K_SNAP_FLASH_CFG_MODE,SP5K_SNAP_FLASH_MODE_OFF);
}

/*-------------------------------------------------------------------------
 *  Function Name : cmdFlash
 *------------------------------------------------------------------------*/
void
cmdFlash(
	UINT32 argc,
	UINT8 *arg[],
	UINT32 v[]
)
{
	if (argc==1) {
		printf("flash usage:\n"
			" flash ucmd 0/1 : use cmdPrefhCb for debug\n"
			" flash target -LUMA- : set snapflash target\n"
			" flash exp -idx- : set preflash expidx\n"
			" flash agc -idx- : set preflash agcidx\n"
 		);
	} else if (strcmp(arg[1], "ucmd")==0) {
		printf("flash ucmd %d \n", v[2]);
		if (v[2]) {
			/* use cmdPrefhCb to replace appPrefhCb */
			memcpy(&cmdPrefhCb, &appPrefhCb, sizeof(appPrefhCb_t));
			prefhCbPtr = &cmdPrefhCb;
		} else {
			prefhCbPtr = &appPrefhCb;
		}
	} else if (strcmp(arg[1], "target")==0) {
		if (v[2]==0) {
			printf("flash target %d \n", appSnapflashTargetGet());
			return;
		}
		printf("flash target %d \n", v[2]);
		appSnapflashTargetSet(v[2]);
	} else if (strcmp(arg[1], "exp")==0) {
		if (v[2]==0) { v[2]=prefhCbPtr->prefhExp; }
		printf("flash exp %d->%d, %d \n", prefhCbPtr->prefhExp, v[2], prefhCbPtr->prefhAgc);
		prefhCbPtr->prefhExp = v[2];
	} else if (strcmp(arg[1], "agc")==0) {
		if (v[2]==0) { v[2]=prefhCbPtr->prefhAgc; }
		printf("flash exp %d, %d->%d \n", prefhCbPtr->prefhExp, prefhCbPtr->prefhAgc, v[2]);
		prefhCbPtr->prefhAgc = v[2];
	} else if (strcmp(arg[1], "hpos")==0) {
		if (v[2]){
			prefhCbPtr->snapHPos = v[2];
			printf("hpos=%d\n",v[2]);
		}
	} else if (strcmp(arg[1], "width")==0) {		
		myTestSnapWidth = v[2];
		printf("width=%d\n",v[2]);		
	} else if (strcmp(arg[1], "slow")==0) {		
		UINT16 sh = SHUTTER_1S;
		switch (v[2]){
		case 0:	 {sh = SHUTTER_2S; break;}
		case 1:	 {sh = SHUTTER_1S; break;}			
		case 2:	 {sh = SHUTTER_1_2S; break;}
		case 4:	 {sh = SHUTTER_1_4S; break;}
		case 8:	 {sh = SHUTTER_1_8S; break;}
		case 16: {sh = SHUTTER_1_15S; break;}		
		}	
		printf("sh=%d\n", sh);
		appSnapFlashSlowSyncExpLimitSet(sh);
	}
 }

