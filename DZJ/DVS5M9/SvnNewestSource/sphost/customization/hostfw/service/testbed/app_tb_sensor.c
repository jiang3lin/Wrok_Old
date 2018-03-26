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
#include "customization\dev_init.h"

#include "stdio.h"
#include "stdarg.h"
#include "string.h"
#include "app_com_def.h"
#include "app_com_api.h"

#include "sp5k_global_api.h"
#include "sp5k_sensor_api.h"

#include "app_aaa.h"
#include "app_calib_api.h"
#include "app_simple_menu.h"
#include "app_osd_api.h"
#include "app_tb_util.h"
#include "app_menu_tab.h" 
#include "app_menu_core.h"
#include "app_menu_disp.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#if SP5K_SENSOR_TG_AD9000_SHARPRJ23V3B
#define TBSENSOR_AGCIDX_FOR_PV_EXP_TBL	(pv_svidx_to_agcidx(80))
#define TBSENSOR_AGCIDX_FOR_CAP_EXP_TBL	(cap_svidx_to_agcidx(80))
#define TBSENSOR_EXPIDX_FOR_PV_AGC_TBL	(EXP_1_128+8)
#elif SP5K_SENSOR_TG_AD9000_SHARPRJ23U3B
#define TBSENSOR_AGCIDX_FOR_PV_EXP_TBL	(pv_svidx_to_agcidx(80))
#define TBSENSOR_AGCIDX_FOR_CAP_EXP_TBL	(cap_svidx_to_agcidx(80))
#define TBSENSOR_EXPIDX_FOR_PV_AGC_TBL	(EXP_1_128+8)
#elif SP5K_SENSOR_TG_AD9000_SONY675
#define TBSENSOR_AGCIDX_FOR_PV_EXP_TBL	(pv_svidx_to_agcidx(80))
#define TBSENSOR_AGCIDX_FOR_CAP_EXP_TBL	(cap_svidx_to_agcidx(80))
#define TBSENSOR_EXPIDX_FOR_PV_AGC_TBL	(EXP_1_128+8)
#elif SP5K_SENSOR_TG_PANX12081A_MN34510
#define TBSENSOR_AGCIDX_FOR_PV_EXP_TBL	(10)
#define TBSENSOR_AGCIDX_FOR_CAP_EXP_TBL	(25)
#define TBSENSOR_EXPIDX_FOR_PV_AGC_TBL	(EXP_1_256+8)
#elif SP5K_SENSOR_MI9130
#define TBSENSOR_AGCIDX_FOR_PV_EXP_TBL	(pv_svidx_to_agcidx(80))
#define TBSENSOR_AGCIDX_FOR_CAP_EXP_TBL	(cap_svidx_to_agcidx(80))
#define TBSENSOR_EXPIDX_FOR_PV_AGC_TBL	(EXP_1_128)
#elif SP5K_SENSOR_AR0331	
#define TBSENSOR_AGCIDX_FOR_PV_EXP_TBL	(pv_svidx_to_agcidx(80))
#define TBSENSOR_AGCIDX_FOR_CAP_EXP_TBL	(cap_svidx_to_agcidx(80))
#define TBSENSOR_EXPIDX_FOR_PV_AGC_TBL	(EXP_1_128)

#elif SP5K_SENSOR_OV5610
#define TBSENSOR_AGCIDX_FOR_PV_EXP_TBL	(pv_svidx_to_agcidx(80))
#define TBSENSOR_AGCIDX_FOR_CAP_EXP_TBL	(cap_svidx_to_agcidx(80))
#define TBSENSOR_EXPIDX_FOR_PV_AGC_TBL	(EXP_1_128)
#else
#define TBSENSOR_AGCIDX_FOR_PV_EXP_TBL	(pv_svidx_to_agcidx(80))
#define TBSENSOR_AGCIDX_FOR_CAP_EXP_TBL	(cap_svidx_to_agcidx(80))
#define TBSENSOR_EXPIDX_FOR_PV_AGC_TBL	(EXP_1_128)
#endif


typedef enum {
	TBSENSOR_P1_SEL_BACK,
	TBSENSOR_P1_SEL_IRIS_DIFF,
	TBSENSOR_P1_SEL_MINI_GAIN,
	TBSENSOR_P1_SEL_AE_LV_6_15,
	TBSENSOR_P1_SEL_CAP_LV_6_15,
	TBSENSOR_P1_SEL_MORE,
	TBSENSOR_P1_SEL_TOTAL,
} tbSensorTestMenuSelect_P1_t;

static UINT8 tbSensorTest_P1_TitleText[]="SensorTest P1";
static UINT32 tbSensorTest_P1_SelList[] = {
		 (UINT32)"Back...",
		 (UINT32)"IRIS Diff LV12",
		 (UINT32)"Mini Gain LV12",
		 (UINT32)"PV LV 4-16",
		 (UINT32)"CAP LV 4-16",
		 (UINT32)"P2...",
		};
#define TBSENSOR_P1_SelectGet()		SMENU_SelectGet()
#define TBSENSOR_P1_Scroll(isUp)	SMENU_Scroll(isUp)
#define TBSENSOR_P1_Show()			SMENU_Show()
#define TBSENSOR_P1_Config()		SMENU_Config(tbSensorTest_P1_TitleText, 6, tbSensorTest_P1_SelList, 0, 18)
#define TBSENSOR_P1_REDRAW()		do { TBSENSOR_P1_Config(); TBSENSOR_P1_Show(); } while(0)


typedef enum {
	TBSENSOR_P2_SEL_BACK,
	TBSENSOR_P2_SEL_PV_EXP_TBL_SCAN,
	TBSENSOR_P2_SEL_CAP_EXP_TBL_SCAN,
	TBSENSOR_P2_SEL_AGC_TBL_SCAN,
	TBSENSOR_P2_SEL_SHUT_DLY,
	TBSENSOR_P2_SEL_MORE,
	TBSENSOR_P2_SEL_TOTAL,
} tbSensorTestMenuSelect_P2_t;
static UINT8 tbSensorTest_P2_TitleText[]="SensorTest P2";
static UINT32 tbSensorTest_P2_SelList[] = {
		 (UINT32)"Back...",
		 (UINT32)"PV EXP Scan LV10",
		 (UINT32)"CAP EXP Scan LV10",
		 (UINT32)"AGC Scan LV10",
		 (UINT32)"ShutDelay",
		 (UINT32)"More...",
		};
#define TBSENSOR_P2_SelectGet()		SMENU_SelectGet()
#define TBSENSOR_P2_Scroll(isUp)	SMENU_Scroll(isUp)
#define TBSENSOR_P2_Show()			SMENU_Show()
#define TBSENSOR_P2_Config()		SMENU_Config(tbSensorTest_P2_TitleText, 6, tbSensorTest_P2_SelList, 0, 18)
#define TBSENSOR_P2_REDRAW()		do { TBSENSOR_P2_Config(); TBSENSOR_P2_Show(); } while(0)


static UINT32 tbSensorTestSelect;

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef void (*pfunc_KeySet_Process_t)(UINT32 param);

static pfunc_KeySet_Process_t pfunc_KeySet_Process=NULL;
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
UINT32 appIrisLargeSet(void);
UINT32 appIrisSmallSet(void);
appAeCb_t *appAeCbGet(void);
void appAePreviewSet(void);
void appAePreviewRestore(void);
static void TB_SensorTest_StateExit(UINT32 nxtSt, UINT32 param);
void appCalibDataSave(void);
UINT8 appAeStableGet(void);
SINT32 appPreviewAeBvIdxGet(void);
SINT32 pv_svidx_to_agcidx(SINT32 svidx);
SINT32 cap_svidx_to_agcidx(SINT32 svidx);
void appGetRawAvgRGGB(UINT16 *r, UINT16 *gr, UINT16 *gb, UINT16 *b);
void tbSensor_LVScanTest(BOOL isPv);
extern void tbMsh_ShutterCloseLagTest(BOOL obOn,UINT32 iris);
extern void appStillUtil_CaptureSet(void);
extern UINT32 menuPosBackup;  

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/


/*-------------------------------------------------------------------------
 * Function : tbSensor_TestCalibRawCb
 *------------------------------------------------------------------------*/
void tbSensor_TestCalibRawCb(void)
{
	SINT32 expidx;
	UINT32 agcidx, capAgc, viewAgc;
	UINT16 cluma;
	UINT32 idxMax, idxMin, agcMax, agcMin;
	UINT16 lowBound,hiBound;
	BOOL isSuccess;

	/*-- Set View Config */
	tbUtil_ViewEnter();

	/*-- AE/AWB Off */
	tbUtil_AeAwb_Dark_Off();

	/*-- Large IRIS */
	appIrisLargeSet();

	/*-- OB Subtraction */
	tbUtil_ObApply();

	/***************************************
	 * SNAP @ specific exp/agc
	 ***************************************/
	expidx=CALIB_EXP_1_128; agcidx = CALIB_ISO_100;
	sp5kPreviewExpAgcSet(expidx ,agcidx);
	sp5kStillCapExpAgcSet(expidx ,agcidx);
	sp5kSensorVvalidWait();
	sp5kSensorVvalidWait();

	cluma = tbUtil_CapRawLumaGet();

	printf("\n*********************************\n");
	printf("CAP: expidx(%d), agcidx(%d) >>> cluma(%d) \n\n", expidx, agcidx, cluma);
	capAgc = agcidx;

	if (cluma>65536-5000) {
		printf("\n!!!!!!!!!!!!!!!!!!!!!\n");
		printf("!! Expousre overflow !!!!");
		printf("!!!!!!!!!!!!!!!!!!!!!\n\n");
		APP_SLEEP_MS(5000);
		goto _exit;
	}

	/**************************************
	 * VIEW to approach the luma
	 **************************************/
	lowBound= (cluma * 98) / 100;
	hiBound = (cluma * 102) / 100;
	DBG_PRINT("VIEW: To approach Luma lowBound(%d), hiBound(%d) \n", lowBound,hiBound);

	/*-- VIEW : Retrieve Gain limitation */
	tbUtil_ViewAgcLimtGet(&agcMax, &agcMin);
	idxMax = agcMax; idxMin = agcMin;

	agcidx = (idxMax+idxMin)/2;
	expidx = CALIB_EXP_1_128+16;
	isSuccess = TRUE;
	do{
		sp5kPreviewExpAgcSet(expidx ,agcidx);
		sp5kSensorVvalidWait();
		sp5kSensorVvalidWait();

		cluma = tbUtil_PvRawLumaGet();

		DBG_PRINT("\nSenDiff : agcidx = %d (%d)\n",agcidx, cluma);

		if ((cluma<lowBound && agcidx>=agcMax) ||
			(cluma>hiBound && agcidx<=agcMin)) {
			/*-- Out of AE limitation !! */
			isSuccess=FALSE;
			break;
		}

		if (cluma >= lowBound && cluma <= hiBound) {
			/*-- Converged */
			isSuccess = TRUE;
			break;
		}

		if(cluma < lowBound && agcidx < agcMax) {
			/*-- Too Dark !! */
			if (idxMax-idxMin <4) {
				agcidx++;
			} else {
				idxMin = agcidx; agcidx=(idxMax+idxMin)/2;
			}
		} else if(cluma > hiBound && agcidx > agcMin){
			/*-- Too Bright !! */
			if (idxMax-idxMin <4) {
				agcidx--;
			} else {
				idxMax = agcidx; agcidx=(idxMax+idxMin)/2;
			}
		}
	}while(1);

	if (isSuccess) {
		viewAgc = agcidx-16;
		DBG_PRINT("\n******* View: approached @ agc = %d *********\n\n", viewAgc);
	} else {
		printf("\n!!!!!!!!!!!!!!!!!!!!!\n");
		printf("!! View failed to approach SNAP \n");
		printf("!!!!!!!!!!!!!!!!!!!!!\n\n");
		goto _exit;
	}

	DBG_PRINT("\n************************************\n");
	DBG_PRINT("Sensitivity: Approach lBound = %d uBound = %d \n", lowBound, hiBound);
	DBG_PRINT("Sensitivity: viewAgc(%d) capAgc(%d) ... Offset(%d)\n\n", viewAgc, capAgc, capAgc-viewAgc);

_exit:
	sp5kAeModeSet(SP5K_AE_MODE_INFO_ONLY);
	sp5kAwbModeSet(SP5K_AWB_MODE_INFO_ONLY);
}
/*-------------------------------------------------------------------------
 * Function : tbSensor_SensitivityDiff
 *------------------------------------------------------------------------*/
void tbSensor_SensitivityDiff(void)
{
	SINT32 expidx;
	UINT32 agcidx, capAgc, viewAgc;
	UINT16 cluma;
	UINT32 idxMax, idxMin, agcMax, agcMin;
	UINT16 lowBound,hiBound;
	BOOL isSuccess;

	/*-- Set View Config */
	tbUtil_ViewEnter();

	/*-- AE/AWB Off */
	tbUtil_AeAwb_Dark_Off();

	/*-- Large IRIS */
	appIrisLargeSet();

	/*-- OB Subtraction */
	tbUtil_ObApply();

	/***************************************
	 * SNAP @ specific exp/agc
	 ***************************************/
	expidx=CALIB_EXP_1_128; agcidx = CALIB_ISO_100;
	sp5kPreviewExpAgcSet(expidx ,agcidx);
	sp5kStillCapExpAgcSet(expidx ,agcidx);
	sp5kSensorVvalidWait();
	sp5kSensorVvalidWait();

	cluma = tbUtil_CapRawLumaGet();

	printf("\n*********************************\n");
	printf("CAP: expidx(%d), agcidx(%d) >>> cluma(%d) \n\n", expidx, agcidx, cluma);
	capAgc = agcidx;

	if (cluma>65536-5000) {
		printf("\n!!!!!!!!!!!!!!!!!!!!!\n");
		printf("!! Expousre overflow !!!!");
		printf("!!!!!!!!!!!!!!!!!!!!!\n\n");
		APP_SLEEP_MS(5000);
		goto _exit;
	}

	/**************************************
	 * VIEW to approach the luma
	 **************************************/
	lowBound= (cluma * 98) / 100;
	hiBound = (cluma * 102) / 100;
	DBG_PRINT("VIEW: To approach Luma lowBound(%d), hiBound(%d) \n", lowBound,hiBound);

	/*-- VIEW : Retrieve Gain limitation */
	tbUtil_ViewAgcLimtGet(&agcMax, &agcMin);
	idxMax = agcMax; idxMin = agcMin;

	agcidx = (idxMax+idxMin)/2;
	expidx = CALIB_EXP_1_128+16;
	isSuccess = TRUE;
	do{
		sp5kPreviewExpAgcSet(expidx ,agcidx);
		sp5kSensorVvalidWait();
		sp5kSensorVvalidWait();

		cluma = tbUtil_PvRawLumaGet();

		DBG_PRINT("\nSenDiff : agcidx = %d (%d)\n",agcidx, cluma);

		if ((cluma<lowBound && agcidx>=agcMax) ||
			(cluma>hiBound && agcidx<=agcMin)) {
			/*-- Out of AE limitation !! */
			isSuccess=FALSE;
			break;
		}

		if (cluma >= lowBound && cluma <= hiBound) {
			/*-- Converged */
			isSuccess = TRUE;
			break;
		}

		if(cluma < lowBound && agcidx < agcMax) {
			/*-- Too Dark !! */
			if (idxMax-idxMin <4) {
				agcidx++;
			} else {
				idxMin = agcidx; agcidx=(idxMax+idxMin)/2;
			}
		} else if(cluma > hiBound && agcidx > agcMin){
			/*-- Too Bright !! */
			if (idxMax-idxMin <4) {
				agcidx--;
			} else {
				idxMax = agcidx; agcidx=(idxMax+idxMin)/2;
			}
		}
	}while(1);

	if (isSuccess) {
		viewAgc = agcidx-16;
		DBG_PRINT("\n******* View: approached @ agc = %d *********\n\n", viewAgc);
	} else {
		printf("\n!!!!!!!!!!!!!!!!!!!!!\n");
		printf("!! View failed to approach SNAP \n");
		printf("!!!!!!!!!!!!!!!!!!!!!\n\n");
		goto _exit;
	}

	DBG_PRINT("\n************************************\n");
	DBG_PRINT("Sensitivity: Approach lBound = %d uBound = %d \n", lowBound, hiBound);
	DBG_PRINT("Sensitivity: viewAgc(%d) capAgc(%d) ... Offset(%d)\n\n", viewAgc, capAgc, capAgc-viewAgc);

_exit:
	sp5kAeModeSet(SP5K_AE_MODE_INFO_ONLY);
	sp5kAwbModeSet(SP5K_AWB_MODE_INFO_ONLY);
}


void tbSensor_IrisSensDiff(void)
{
	SINT32 expidx;
	UINT32 agcidx;
	UINT16 clumaLarge, agcLarge, agcSmall;
	UINT32 idxMax, idxMin, agcMax, agcMin;
	UINT16 lowBound,hiBound;
	BOOL isSuccess = TRUE;
	appCalibData_t *pcalib;
	pcalib = appCalibDataGet();

	/*-- Set View Config */
	tbUtil_ViewEnter();

	/*-- AE/AWB Off */
	tbUtil_AeAwb_Dark_Off();

	/*-- OB Subtraction */
	tbUtil_ObApply();

	/***************************************
	 * SNAP Diff
	 ***************************************/
	/*-- Large IRIS */
	tbUtil_IrisCtrl(1/*islarge*/);

	/*-- Retrieve Gain limitation */
	tbUtil_SnapAgcLimtGet(&agcMax, &agcMin);
	idxMax = agcMax; idxMin = agcMin;

	agcidx = (idxMax+idxMin)/2;

/*s Modify by Aries 10/02/04*/
#if (_CAM_A900_EVM)||(_CAM_M900_EVM)||(_CAM_A900_D3) ||(_CAM_CATHY_EVM)
 	expidx=CALIB_EXP_1_256;
 #else
	expidx=CALIB_EXP_1_512;
 #endif
/*e Modify by Aries 10/02/04*/

	sp5kPreviewExpAgcSet(expidx ,agcidx);
	sp5kStillCapExpAgcSet(expidx ,agcidx);
	sp5kSensorVvalidWait();
	sp5kSensorVvalidWait();

	clumaLarge = tbUtil_CapRawLumaGet();
	agcLarge = agcidx;
	printf("Large IRIS rawLuma(%d) @ agcidx(%d) \n", clumaLarge, agcLarge);

	if (clumaLarge > 50000) {
		isSuccess = FALSE;
		printf("\n@@@@@@@@@@@@@@@@@@@@@@\n");
		printf("Capture Luma overflew !! \n\n");
		goto _exit;
	}
	/*-- Small IRIS */
	tbUtil_IrisCtrl(0/*islarge*/);
	sp5kPreviewExpAgcSet(expidx ,agcidx);
	sp5kStillCapExpAgcSet(expidx ,agcidx);
	sp5kSensorVvalidWait();
	sp5kSensorVvalidWait();

	lowBound= (clumaLarge * 98) / 100;
	hiBound = (clumaLarge * 102) / 100;
	DBG_PRINT("Small IRIS approaching lowBound(%d), hiBound(%d) \n", lowBound,hiBound);

	agcidx = (idxMax+idxMin)/2;
	isSuccess = TRUE;
	agcidx = tbUtil_AgcApproachTest(0/*isPvMode*/, lowBound, hiBound, expidx);

	if (agcidx==0) {
		isSuccess = FALSE;
	}

	if (isSuccess) {
		agcSmall = agcidx;
		pcalib->irisDiff = agcSmall-agcLarge;
		DBG_PRINT("\n******* IrisDiff (CAP): diff=%d, agcLarge(%d), agcSmall(%d)*********\n\n", pcalib->irisDiff, agcLarge, agcSmall);
	} else {
		printf("\n!!!!!!!!!!!!!!!!!!!!!\n");
		printf("!! View failed to approach SNAP \n");
		printf("!!!!!!!!!!!!!!!!!!!!!\n\n");
		goto _exit;
	}

_exit:
	if (isSuccess) {
		appCalibDataSave();
		DBG_PRINT("\n** IRIS Diff **********************************\n");
		DBG_PRINT("Capture: Large(%d), Small(%d), Diff=%d \n", agcLarge, agcSmall, pcalib->irisDiff);
	} else {
		pcalib->irisDiff = 0;
	}

	tbUtil_QuickSweepHostMessage(300);
	sp5kAeModeSet(SP5K_AE_MODE_INFO_ONLY);
	sp5kAwbModeSet(SP5K_AWB_MODE_INFO_ONLY);
}

/*-------------------------------------------------------------------------
 * Function : tbSensor_CapExpTblScan
 * LV14~15, EXP_1_32, Large-IRIS, scan gain table for minimum saturation gain @ View
 * execute @ sensor test page
 *------------------------------------------------------------------------*/
static void tbSensor_PvMiniGainTest(void)
{
	UINT32 agcidx, agcMax, agcMin;
	SINT32 expidx;
	BOOL isSuccess;
	UINT16 satLuma;

	/*-- Set View Config */
	tbUtil_ViewEnter();
	tbUtil_CalibCdspSet(1/*isObOn*/);
	//dbgOsd_Print(1,"CALIB miniGAIN LV14");

	/*-- AE/AWB Off */
	tbUtil_AeAwb_Dark_Off();

	/*-- Large IRIS */
	tbUtil_IrisCtrl(1/*isLarge*/);

	/*-- OB Subtraction */
	tbUtil_ObApply();

	/*-- Get gain limitation */
	tbUtil_ViewAgcLimtGet(&agcMax, &agcMin);

	/*-- Observe Saturation raw value */
	expidx = EXP_1_32; agcidx = agcMax;
	printf("Getting saturation value ...\n");
	printf("exp=%d agc=%d \n", expidx, agcidx);
	sp5kPreviewExpAgcSet(expidx, agcMax);
	sp5kSensorVvalidWait();
	sp5kSensorVvalidWait();
	if (FALSE==tbUtil_PvRawSaturatedCheck(62000, &satLuma)) {
		printf("\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
		printf("Not yet saturated -- %d \n\n", satLuma);
		isSuccess = FALSE;
		goto _exit;
	}
	printf("... Saturated @ %d \n", satLuma);

	/*-- Look for mini gain  */
	isSuccess = tbUtil_PvMiniGainSearch(62000, EXP_1_32, &agcidx);

	if (isSuccess) {
		printf("\n**********************************\n");
		printf("PV Minimum Gain : %d \n\n", agcidx);
	} else {
		printf("\n!!!!!!!!!!!!!!!!!!!!!\n");
		printf("!!!! Mini Gain FAIL @ %d \n", agcidx);
		printf("!!!!!!!!!!!!!!!!!!!!!\n\n");
		goto _exit;
	}

	dbgOsd_Print(4, "PV min G : %d", agcidx);

_exit:
	tbUtil_QuickSweepHostMessage(100);
	sp5kAeModeSet(SP5K_AE_MODE_INFO_ONLY);
	sp5kAwbModeSet(SP5K_AWB_MODE_INFO_ONLY);
}

void tbSensor_CapMiniGainTest(void)
{
	UINT32 agcidx, agcMax, agcMin;
	SINT32 expidx;
	BOOL isSuccess;
	UINT16 satLuma;
	appCalibData_t *pcalib;

	/*-- Set View Config */
	tbUtil_ViewEnter();
	tbUtil_CalibCdspSet(1/*isObOn*/);
	dbgOsd_Print(1,"CALIB miniGAIN LV12");

	/*-- AE/AWB Off */
	tbUtil_AeAwb_Dark_Off();

	/*-- Large IRIS */
	tbUtil_IrisCtrl(1/*isLarge*/);

	/*-- OB Subtraction */
	tbUtil_ObApply();

	/*-- Get gain limitation */
	tbUtil_SnapAgcLimtGet(&agcMax, &agcMin);

	/*-- Observe Saturation raw value */
	pcalib = appCalibDataGet();
	pcalib->capSatGainSign = 0;
	expidx = EXP_1_04; agcidx = agcMax;
	printf("Getting saturation value ...\n");
	printf("exp=%d agc=%d \n", expidx, agcidx);
	sp5kStillCapExpAgcSet(expidx, agcMax);
	sp5kSensorVvalidWait();
	sp5kSensorVvalidWait();
	if (FALSE==tbUtil_CapRawSaturatedCheck(62000, &satLuma)) {
		printf("\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
		printf("Not yet saturated -- %d \n\n", satLuma);
		isSuccess = FALSE;
		goto _mini_g_exit;
	}
	printf("... Saturated @ %d \n", satLuma);

	/*-- Look for mini gain  */
	isSuccess = tbUtil_CapMiniGainSearch(62000, EXP_2_01, &agcidx);

	if (isSuccess) {
		printf("\n**********************************\n");
		printf("Minimum Gain : %d \n\n", agcidx);
		pcalib->capSatGain = (SINT16)agcidx;
		pcalib->capSatGainSign = 1;
		appCalibDataSave();
	} else {
		printf("\n!!!!!!!!!!!!!!!!!!!!!\n");
		printf("!!!! Mini Gain FAIL @ %d \n", agcidx);
		printf("!!!!!!!!!!!!!!!!!!!!!\n\n");
		pcalib->capSatGainSign = 0;
		goto _mini_g_exit;
	}

	dbgOsd_Print(4, "min G : %d", agcidx);
	tbUtil_QuickSweepHostMessage(300);

_mini_g_exit:
	sp5kAeModeSet(SP5K_AE_MODE_INFO_ONLY);
	sp5kAwbModeSet(SP5K_AWB_MODE_INFO_ONLY);

}

void tbSensor_LvDiff(void)
{
	SINT32 ae_lvidx, lvbox=10*16; /* LV 10 */

	appCalibData_t *pcalib;
	appAeCb_t *paeCb;

	/*-- Set View Config */
	tbUtil_ViewEnter();
	dbgOsd_Print(1, "LV_Diff   LV10");

	/*-- AE/AWB On */
	tbUtil_AeControl(1);
	tbUtil_AwbControl(1);

	/*-- OB Subtraction */
	tbUtil_ObApply();

	/*-- Check prerequisite */
	pcalib = appCalibDataGet();
	if (!(pcalib->ObSign && pcalib->IsoSign && pcalib->ShutterSign)) {
		dbgOsd_Print(4, "OB ISO MShut Not Yet");
		printf("\nPlease calib OB/ISO/MShut first !!! \n\n");
		APP_SLEEP_MS(2000);
		return;
	}

	/*- Run AE */
	dbgOsd_Print(4, "Pls Wait AE ...");
	tbUtil_RunAe();

	paeCb = appAeCbGet();
	if (appAeStableGet()==FALSE) {
		printf("ERROR: AE not converged !!! \n");
		dbgOsd_Print(6, "AE Failed");
		APP_SLEEP_MS(2000);
		return;
	}

	ae_lvidx = paeCb->lvidx;
	printf("\n*** AE Lvidx(%d), LV Diff(%d) \n\n", ae_lvidx, (lvbox-ae_lvidx));
	dbgOsd_Print(6, "LV Diff (%d)", (lvbox-ae_lvidx));
	APP_SLEEP_MS(2000);

}

void tbSensor_MShut_SUB_Test(SINT32 exp, UINT32 delay)
{
	/*-- Set View Config */
	tbUtil_ViewEnter();

	/*-- AE/AWB Off */
	tbUtil_AeAwb_Dark_Off();

	/*-- Large IRIS */
	appIrisLargeSet();

	/*-- OB Subtraction */
	tbUtil_ObApply();

	/*-- Configure exposure time to adjust SUB */
	sp5kPreviewExpAgcSet(80, 30);
	sp5kStillCapExpAgcSet(exp, 30);

	/*-- Configure Shutter delay */
	sp5kSnapShutterDelaySet(SP5K_SNAP_SHUTTER_CLOSE_EXP_END, 0, delay);

	sp5kSensorVvalidWait();
	sp5kSensorVvalidWait();

	/*-- Capture RAW & Get RawLuma */
	tbUtil_CapRawLumaGet();

}

/*-------------------------------------------------------------------------
 * Function : _evN200toP200_scan
 *------------------------------------------------------------------------*/
UINT32 _evN200toP200_scan(SINT32 evIdx)
{
 	SINT32 expIdx;
    UINT32 agcIdx;
    UINT16 luma;

	printf("\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
	printf(">>>>> EV (%d) \n", evIdx*33-200);
	printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n\n");

	tbLogBuf_Add(evIdx*33-200); /* log data */

	/*-- Set View Config */
	tbUtil_AeControl(1);
	tbUtil_AwbControl(1);
	tbUtil_ViewSet();

	/*-- Configure EV Bias */
	appAeEvBiasSet((UINT8)evIdx);
	APP_SLEEP_MS(100);

	tbUtil_QuickSweepHostMessage(50);
	/*- Run AE */
	CLEAR_OSD_SRC;
	dbgOsd_Print(4, "Pls Wait AE ...");
	tbUtil_RunAe();
	CLEAR_OSD_SRC;

	/*-- PREVIEW */
	/*- Get exp/agc, and log to buffer */
	sp5kPreviewExpAgcGet(&expIdx, &agcIdx);
	luma = tbUtil_PvRawLumaGet();
	tbLogBuf_Add(expIdx);
	tbLogBuf_Add(agcIdx);

	tbLogBuf_Add(tbUtil_QueryIrisStop());	// iris stop
	tbLogBuf_Add(appAeStableGet());	// aeStable

	tbLogBuf_Add(appPreviewAeBvIdxGet()+80); // lvidx
	tbLogBuf_Add(luma);

	return SUCCESS;
}
/*-------------------------------------------------------------------------
 * Function : _evN200toP200_save
 *------------------------------------------------------------------------*/
void _evN200toP200_save(void)
{
	SINT32 ev;
	UINT32 luma;
	UINT32 irisStop;
 	SINT32 expIdx;
    UINT32 agcIdx;
	UINT32 aeStable;
	UINT32 lvidx;

	ev = (SINT32)tbLogBuf_Get();
	expIdx=tbLogBuf_Get();
	agcIdx=tbLogBuf_Get();
	irisStop=tbLogBuf_Get();
	aeStable = tbLogBuf_Get();
	lvidx = tbLogBuf_Get();
	luma=tbLogBuf_Get();
	tbLogFile_Print("%d %d %d %d %d %d.%d %d\n",
					ev,
					expIdx, agcIdx,
					(!!irisStop*32)+48,
					(1*aeStable),
					lvidx/16, lvidx%16*100/16,
					luma);
}

/*-------------------------------------------------------------------------
 * Function : _lv4to17_scan
 *------------------------------------------------------------------------*/
UINT32 _lv4to17_scan(BOOL isPv, SINT32 curLV,BOOL jpgflag )
{
 	SINT32 expIdx;
	UINT32 agcIdx;
	UINT8 szTmp[128];
	UINT16 luma;	
	
	/*-- Confirm Light Source Box */
	sprintf(szTmp, "LVBox: %d.%d Rdy ?", curLV/10, curLV%10);
	printf("\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
	printf(">>>>>%s \n", szTmp);
	printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n\n");
	CLEAR_OSD_SRC;
	SMENU_YesNoConfig(szTmp);
	SMENU_YesNoShow();
	if (SMENU_SEL_NO==SMENU_SelScan()) {
		return FAIL;
	}

	tbLogBuf_Add(curLV); /* log data */

	/*-- Set View Config */
	tbUtil_AeControl(1);
	tbUtil_AwbControl(1);
	tbUtil_ViewSet();

	/*- Run AE */
	CLEAR_OSD_SRC;
	dbgOsd_Print(4, "Pls Wait AE ...");
	tbUtil_RunAe();
	CLEAR_OSD_SRC;

	if (isPv==TRUE) {
		/*-- PREVIEW */
		/*- Get exp/agc, and log to buffer */
		sp5kPreviewExpAgcGet(&expIdx, &agcIdx);
		luma = tbUtil_PvRawLumaGet();
		tbLogBuf_Add(expIdx);
		tbLogBuf_Add(agcIdx);

		tbLogBuf_Add(tbUtil_QueryIrisStop());	// iris stop
		tbLogBuf_Add(appAeStableGet());	// aeStable

		tbLogBuf_Add(appPreviewAeBvIdxGet()+80); // lvidx
		tbLogBuf_Add(luma);
	} else {
		/*-- CAPTURE */
		tbUtil_RunFastAe();

		tbUtil_AeCaptureSet();
		/*- Get exp/agc, and log to buffer */
		sp5kStillCapExpAgcGet(&expIdx, &agcIdx);
		tbLogBuf_Add(expIdx);
		tbLogBuf_Add(agcIdx);

		tbLogBuf_Add(tbUtil_QueryIrisStop());	// iris stop
		tbLogBuf_Add(appAeStableGet());	// aeStable

		tbLogBuf_Add(appPreviewAeBvIdxGet()+80); // lvidx

		if (jpgflag) {
			appStillUtil_CaptureSet();
			appHostMsgWaitExact(SP5K_MSG_MODE_READY, SP5K_MODE_STILL_PREVIEW, 3000);
			luma =0;
		}
		else {
			luma = tbUtil_CapRawLumaGet();		
		}
		
		tbLogBuf_Add(luma);

		appAePreviewSet();
		appAePreviewRestore();

	}
	return SUCCESS;
}
/*-------------------------------------------------------------------------
 * Function : _lv4to17_save
 *------------------------------------------------------------------------*/
void _lv4to17_save(BOOL isPv)
{
	UINT32 lv, luma;
	UINT32 irisStop;
 	SINT32 expIdx;
    UINT32 agcIdx;
	UINT32 aeStable;
	UINT32 lvidx;

	lv = tbLogBuf_Get();
	expIdx=tbLogBuf_Get();
	agcIdx=tbLogBuf_Get();
	irisStop=tbLogBuf_Get();
	aeStable = tbLogBuf_Get();
	lvidx = tbLogBuf_Get();
	luma=tbLogBuf_Get();
	tbLogFile_Print("%2d.%1d %3d %3d %3d %1d %2d.%02d %5d\n",
					lv/10, lv%10,
					expIdx, agcIdx,
					(!!irisStop*32)+48,
					(1*aeStable),
					lvidx/16, lvidx%16*100/16,
					luma);
}
/*-------------------------------------------------------------------------
 * Function : tbSensor_LV4to17
 *------------------------------------------------------------------------*/
void tbSensor_LV4to17(BOOL isPv)
{
	SINT32 curLV;
	SINT32 startLV, endLV, deltaLV;
	UINT32 jpgflag = 0;	
	
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

	/*-- Open Log File */
	if (isPv==1) {
		if (!tbLogFile_Open("D:\\PV_LV417.TXT")) return;
	} else {
		if (!tbLogFile_Open("D:\\CP_LV417.TXT")) return;
	}

	/*-- Clear & reset tbBuf */
	tbLogBuf_Clear();

	extern UINT32 xdata[]; 
	if(xdata[0]) {
		jpgflag = 1;	
		printf("JPG capture is enable.....\n");
	}
	if(xdata[1]) {
		appAeCapIsoSet(xdata[1]);
		printf("Capture ISO set : %d\n", xdata[1]);
	}


	/*-- LOOP 1 -------------------------------------------------------
	 * LV 17 - 4
	 */
	startLV=170; endLV=40; deltaLV=10;
	for (curLV = startLV; curLV>=endLV; curLV-=deltaLV) {
		if (FAIL==_lv4to17_scan(isPv, curLV,jpgflag)) goto _exit;
	}

	/*-- LOOP 2 -------------------------------------------------------
	 * LV 4 - 17
	 */
	tbUtil_BeepWarn();
	CLEAR_OSD_SRC;
	dbgOsd_Print(4, "LV 4 - 17"); APP_SLEEP_MS(1500);
	printf("\n****** ((((( Revert Scan LV 4 ~ 17 ))))) ******\n\n");

	startLV=40; endLV=170; deltaLV=10;
	for (curLV = startLV; curLV<=endLV; curLV+=deltaLV) {
		if (FAIL==_lv4to17_scan(isPv, curLV,jpgflag)) goto _exit;
	}

_exit:
	/*-- Save log -------------------------------------------------------
	 */
	tbLogBuf_PtrReset();

	if (isPv==TRUE) {
		tbLogFile_Print("\nLV Scan (VIEW) : 17.0 ~ 4.0 \n\n");
		tbLogFile_Print("LV   exp agc iris stable lvidx RawLuma\n");
	} else {
		tbLogFile_Print("\nLV Scan (CAPTURE): 17.0 ~ 4.0 \n\n");
		tbLogFile_Print("LV   exp agc iris stable lvidx RawLuma\n");
	}

	startLV=170; endLV=40; deltaLV=10;
	for (curLV = startLV; curLV>=endLV; curLV-=deltaLV) {
		_lv4to17_save(isPv);
	}

	if (isPv==TRUE) {
		tbLogFile_Print("\nLV Scan (VIEW) : 4.0 ~ 17.0 \n\n");
		tbLogFile_Print("LV   exp agc iris stable lvidx RawLuma\n");
	} else {
		tbLogFile_Print("\nLV Scan (CAPTURE): 4.0 ~ 17.0 \n\n");
		tbLogFile_Print("LV   exp agc iris stable lvidx RawLuma\n");
	}

	startLV=40; endLV=170; deltaLV=10;
	for (curLV = startLV; curLV<=endLV; curLV+=deltaLV) {
		_lv4to17_save(isPv);
	}

	tbLogFile_Close();

	CLEAR_OSD_SRC;
	dbgOsd_Print(4, "File Saved ...");
	tbUtil_QuickSweepHostMessage(3000);

	return;
}

/*-------------------------------------------------------------------------
 * Function : tbAE_EvBiasTest
 *------------------------------------------------------------------------*/
void tbAE_EvBiasTest(void)
{
    SINT32 curEV;
    SINT32 startEV, endEV, deltaEV;

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

	/*-- Open Log File */
	if (!tbLogFile_Open("D:\\EVBIASVW.TXT")) return;

	/*-- Clear & reset tbBuf */
	tbLogBuf_Clear();

	/*-- LOOP 1 -------------------------------------------------------
	 * LV 10, -2.0EV to +2.0EV
	 */
	startEV=0; endEV=12; deltaEV=1;
	for (curEV=startEV; curEV<=endEV; curEV+=deltaEV) {
		if (FAIL==_evN200toP200_scan(curEV)) goto _exit;
	}

_exit:
	/*-- Save log -------------------------------------------------------
	 */
	tbLogBuf_PtrReset();

	tbLogFile_Print("\nEV Bias Scan (VIEW) : -2.0EV ~ 2.0EV \n\n");
	tbLogFile_Print("ui_idx   exp agc iris stable lvidx RawLuma\n");

	startEV=0; endEV=12; deltaEV=1;
	for (curEV=startEV; curEV<=endEV; curEV+=deltaEV) {
		_evN200toP200_save();
	}

	tbLogFile_Close();

	CLEAR_OSD_SRC;
	dbgOsd_Print(4, "File Saved ...");
	APP_SLEEP_MS(3000);

	return;

}
/******************************************
 * @fn        void tbSensor_PreviewFlickerTableScan(void)
 * @brief        tbSensor_PreviewFlickerTableScan
 * @param     NONE
 * @retval      NONE
 * @see         NULL
 * @author     fan.xiao
 * @since       2011-08-10 , 21:03:20
 * @todo        N/A
 * @bug         N/A
*******************************************/
extern void halCdspWbGainGet( UINT32 *rgain, UINT32 *grgain,  UINT32 *bgain,  UINT32 *gbgain );
void tbSensor_PreViewFlickerTableScan(void)
{
	UINT16 i,aeLuma;
	UINT16 expIdx,agcIdx;
	UINT32 rgain, grgain, bgain, gbgain;
	
	/*-- Open log file */
	if (!tbLogFile_Open("D:\\PVFTblSC.TXT")) 
		return;

	/*-- Set View Config */
	tbUtil_ViewEnter();
	/*-- AE/AWB Off */
	tbUtil_AeAwb_Dark_Off();
	/*-- OB Subtraction */
	tbUtil_ObApply();
	/*-- Large IRIS */
	tbUtil_IrisCtrl(1/*islarge*/);

	/*-- Clean Data Buf */
	tbLogBuf_Clear();
/*********60Hz***********/
	for(i = 0 ; i < 159 ; i ++){  /*expindex index 176*/
		appAeFlickerTableExpAgcIdxGet(30,60,i,&expIdx,&agcIdx);
		sp5kPreviewExpAgcSet(expIdx, agcIdx);
		sp5kSensorVvalidWait();
		sp5kSensorVvalidWait();
		sp5kSensorVvalidWait();
		sp5kSensorVvalidWait();
		
		aeLuma = tbUtil_PvRawLumaGet();
		halCdspWbGainGet( &rgain, &grgain,  &bgain,  &gbgain);
		{
			UINT32 luma;
			luma =  aeLuma;
			luma = luma *gbgain/1024;
			aeLuma = luma>65535?65535:luma;
		}
		
		/*-- Store and show result */
		tbLogBuf_Add(i);
		tbLogBuf_Add(aeLuma);
		tbLogBuf_Add(expIdx);
		tbLogBuf_Add(agcIdx);	
	}
	/*-- Store result to DISK */
	tbLogBuf_PtrReset();
	tbLogFile_Print("\n----- 60Hz AE Table Scan ---------------\n");
	tbLogFile_Print("\n");

	tbLogFile_Print("AEidx expIdx agcIdx luma\n");
	for (i = 0; i < 159; i++)
	{
		UINT32 idx, luma;//expidx,agcidx;
		idx = tbLogBuf_Get();
		luma = tbLogBuf_Get();
		expIdx = tbLogBuf_Get();
		agcIdx = tbLogBuf_Get();
		tbLogFile_Print("%5d %5d %5d %5d \n", idx, expIdx, agcIdx,luma);
		//printf("AE Luma = %5d @ AEIdx(%d) \n", aeLuma, i);
	}
/*********50Hz***********/
	/*-- Clean Data Buf */
	tbLogBuf_Clear();
	for(i = 0 ; i < 159 ; i ++){  /*expindex index 176*/
		appAeFlickerTableExpAgcIdxGet(30,50,i,&expIdx,&agcIdx);
		sp5kPreviewExpAgcSet(expIdx, agcIdx);
		sp5kSensorVvalidWait();
		sp5kSensorVvalidWait();
		sp5kSensorVvalidWait();
		sp5kSensorVvalidWait();
		
		aeLuma = tbUtil_PvRawLumaGet();
		halCdspWbGainGet( &rgain, &grgain,  &bgain,  &gbgain);
		{
			UINT32 luma;
			luma =  aeLuma;
			luma = luma *gbgain/1024;
			aeLuma = luma>65535?65535:luma;
		}		
		
		/*-- Store and show result */
		tbLogBuf_Add(i);
		tbLogBuf_Add(aeLuma);
		tbLogBuf_Add(expIdx);
		tbLogBuf_Add(agcIdx);

	}
	/*-- Store result to DISK */
	tbLogBuf_PtrReset();
	tbLogFile_Print("\n\n----- 50Hz AE Table Scan ---------------\n");
	tbLogFile_Print("\n");

	tbLogFile_Print("AEidx expIdx agcIdx luma\n");
	for (i = 0; i < 159; i++)
	{
		UINT32 idx, luma;//expidx,agcidx;
		idx = tbLogBuf_Get();
		luma = tbLogBuf_Get();
		expIdx = tbLogBuf_Get();
		agcIdx = tbLogBuf_Get();
		tbLogFile_Print("%5d %5d %5d %5d \n", idx, expIdx, agcIdx,luma);
		//printf("AE Luma = %5d @ AEIdx(%d) \n", aeLuma, i);
	}

	tbLogFile_Close();
	tbUtil_QuickSweepHostMessage(100);

}

/*-------------------------------------------------------------------------
 * Function : tbSensor_ExpTableScan
 * LV10, Large-IRIS, scan 1/256s ~ 1/2000s
 * execute @ sensor test page
 *------------------------------------------------------------------------*/
static void tbSensor_ExpTableScan(void)
{

 	SINT32 expidx;
    UINT32 agcidx;
    SINT32 idxMax, idxMin;
	SINT32 startIdx, endIdx, deltaIdx;
    UINT16 aeLuma;

	/*- Confirm Testbed setup */
	//tbUtil_TestProlog("LV10 Init ?");

	/*-- Open log file */
	if (!tbLogFile_Open("D:\\EXPTSCAN.TXT")) return;

	/*-- Set View Config */
	tbUtil_ViewEnter();

	/*-- AE/AWB Off */
	tbUtil_AeAwb_Dark_Off();

	/*-- OB Subtraction */
	tbUtil_ObApply();

	/*-- Large IRIS */
	tbUtil_IrisCtrl(1/*islarge*/);

	/*-- Configure View exposure settings */
	agcidx = TBSENSOR_AGCIDX_FOR_PV_EXP_TBL;
	/*s Modify by Aries 10/02/04*/
	#if (_CAM_A900_D3)||(_CAM_CATHY_EVM)
	sp5kPreviewExpAgcSet(EXP_1_16, agcidx);
	#else
	sp5kPreviewExpAgcSet(EXP_1_04, agcidx);
	#endif
	/*e Modify by Aries 10/02/04*/

	sp5kSensorVvalidWait();

	/*-- Retrieve EXP limitation */
	tbUtil_ViewExpLimtGet(&idxMax, &idxMin);

	/*-- Clean Data Buf */
	tbLogBuf_Clear();

	/*-- Loop ExpT test */
	/*s Modify Aries 10/02/04*/
	#if (_CAM_A900_D3)||(_CAM_CATHY_EVM)
	startIdx = EXP_1_16; endIdx = idxMax; deltaIdx=2;
	#else
	startIdx = EXP_1_04; endIdx = idxMax; deltaIdx=2;
	#endif
	/*e Modify Aries 10/02/04*/
	for (expidx=startIdx; expidx<=endIdx; expidx+=deltaIdx) {
		/*- Get preview luma */
		sp5kPreviewExpAgcSet(expidx, agcidx);
		sp5kSensorVvalidWait();
		sp5kSensorVvalidWait();
		sp5kSensorVvalidWait();
		sp5kSensorVvalidWait();


		aeLuma = tbUtil_PvRawLumaGet();
		
		/*-- Store and show result */
		tbLogBuf_Add(expidx);
		tbLogBuf_Add(aeLuma);

		//dbgOsd_Print(4, "AE Luma(%d), %d", aeLuma, expidx);
		printf("AE Luma = %5d @ ExpIdx(%d) \n", aeLuma, expidx);
	}

	/*-- Store result to DISK */
	tbLogBuf_PtrReset();

	tbLogFile_Print("\n----- Exposure Table Limitation ---------------\n");
	tbLogFile_Print("Tbl Min : %d \n", idxMin);
	tbLogFile_Print("Tbl Max : %d \n", idxMax);
	tbLogFile_Print("Agc : %d \n", agcidx);
	tbLogFile_Print("\n\n");

	tbLogFile_Print("expidx Luma\n");
	for (expidx=startIdx; expidx<=endIdx; expidx+=deltaIdx)
	{
		UINT32 idx, luma;
		idx = tbLogBuf_Get();
		luma = tbLogBuf_Get();
		tbLogFile_Print("%3d %d \n", idx, luma);
	}

	tbLogFile_Close();

	tbUtil_QuickSweepHostMessage(100);


}
/*-------------------------------------------------------------------------
 * Function : tbSensor_CapExpTblScan
 * LV15~8, ISO100, Small-IRIS, scan 1/256s ~ 1/2000s
 * execute @ sensor test page
 *------------------------------------------------------------------------*/
static void tbSensor_CapExpTblScan(UINT16 iris)
{
 	SINT32 expidx;
    UINT32 agcidx;
    SINT32 idxMax, idxMin;
	SINT32 startIdx, endIdx, deltaIdx;
    UINT16 aeLuma;
    unsigned char szLogFile[]="D:\\CAPEXP_H.TXT";


	/*-- Retrieve EXP limitation */
	tbUtil_SnapExpLimtGet(&idxMax, &idxMin);

	/*-- Set View Config */
	tbUtil_ViewEnter();


	/*-- AE/AWB Off */
	tbUtil_AeAwb_Dark_Off();

	/*-- OB Subtraction */
	tbUtil_ObApply();

	if (iris==0) {
		szLogFile[9]='L';
		agcidx = TBSENSOR_AGCIDX_FOR_CAP_EXP_TBL;		
		startIdx = idxMin; endIdx = idxMax; deltaIdx=4;
		//startIdx = idxMin; endIdx = EXP_1_02; deltaIdx=2;
		/*-- Large IRIS for dark scene */
		tbUtil_IrisCtrl(1/*islarge*/);	
	} else {
		szLogFile[10]='S';
		agcidx = TBSENSOR_AGCIDX_FOR_CAP_EXP_TBL;		
		//startIdx = idxMin; endIdx = idxMax; deltaIdx=4;
		startIdx = EXP_1_64; endIdx = idxMax; deltaIdx=4;		
		/*-- Small IRIS for medium bright scene */
		tbUtil_IrisCtrl(0/*islarge*/);
	}	

	printf("\n*********************\n");
	printf("Testing @ Iris-%d, Agc -%d \n", iris,agcidx);
	printf("*********************\n\n");

	/*-- Open log file */
	if (!tbLogFile_Open(szLogFile)) return;
	
	/*-- Create tbLogBuf */
	tbLogBuf_Create();

	/*-- Clean Data Buf */
	tbLogBuf_Clear();

	#if 0
	if (FALSE==_pretest_ExpTblScan(agcidx, idxMax, idxMin)) {
		printf("\n!!!!!!!!!!!!!!!!!!!!\n");
		printf(" Failed PreTest !!\n");
		printf("!!!!!!!!!!!!!!!!!!!!\n\n");
		goto _exit_capexpscan;
	}
	#endif

	#if 0
	startIdx =60;
	#endif

	for (expidx=startIdx; expidx<=endIdx; expidx+=deltaIdx) {
		/*- Get capture luma */
		UINT16 loop;
		tbLogBuf_Add(expidx);
		for (loop=0; loop<3; loop++)
		{		
			sp5kStillCapExpAgcSet(expidx, agcidx);	
			/*s Add by Aries 10/03/02*/
			sp5kSensorVvalidWait();
			/*e Add by Aries 10/03/02*/
			aeLuma = tbUtil_CapRawLumaGet();			
			tbLogBuf_Add(aeLuma);
			printf(" expidx(%d), AE Luma = %d \n", expidx, aeLuma);
		}
	}

	/*-- Store result to DISK */
	tbLogBuf_PtrReset();

	tbLogFile_Print("============ Capture EXP Table Scan ===============\n\n");
	tbLogFile_Print("----- Exposure Table Limitation ---------------\n");
	tbLogFile_Print("Tbl Min : %d \n", idxMin);
	tbLogFile_Print("Tbl Max : %d \n", idxMax);
	tbLogFile_Print("Tested @ agcidx=%d, Iris=%d", agcidx, iris);
	tbLogFile_Print("\n\n");

	tbLogFile_Print("expidx L0 L1 L2 avgLuma\n");
	for (expidx=startIdx; expidx<=endIdx; expidx+=deltaIdx)
	{
		UINT32 idx, luma;
		UINT32 l0, l1, l2;
		idx = tbLogBuf_Get();

		l0 = tbLogBuf_Get();
		l1 = tbLogBuf_Get();
		l2 = tbLogBuf_Get();
		luma = (l0+l1+l2)/3;
		/*luma = (l0+l1)/2;*/
		/*
		tbLogFile_Print("%d %d %d %d %d \n",
				idx, l0, l1, l2, luma); */
		tbLogFile_Print("%3d %d \n", idx, luma);
	}

/* _exit_capexpscan: */
	tbLogFile_Close();

	tbUtil_QuickSweepHostMessage(100);

}

/*-------------------------------------------------------------------------
 * Function : tbSensor_AgcTableScan
 * LV10, Large-IRIS, scan whole gain table
 * execute @ sensor test page
 *------------------------------------------------------------------------*/
static void tbSensor_AgcTableScan(void)
{
 	UINT32 expidx;
    UINT32 agcidx;
    UINT32 idxMax, idxMin;
	UINT32 startIdx, endIdx, deltaIdx;
    UINT16 aeLuma;

	/*- Confirm Testbed setup */
	tbUtil_TestProlog("LV10 Init ?");

	/*-- Open log file */
	if (!tbLogFile_Open("D:\\AGCTSCAN.TXT")) return;

	/*-- Set View Config */
	tbUtil_ViewEnter();

	/*-- AE/AWB Off */
	tbUtil_AeAwb_Dark_Off();

	/*-- OB Subtraction */
	tbUtil_ObApply();

	/*-- Large IRIS */
	tbUtil_IrisCtrl(1/*islarge*/);

	/*-- Retrieve AGC limitation */
	tbUtil_ViewAgcLimtGet(&idxMax, &idxMin);

	/*-- Clean Data Buf */
	tbLogBuf_Clear();

	/*-- Loop ExpT test */
	expidx = TBSENSOR_EXPIDX_FOR_PV_AGC_TBL;
	/*s Modify by Aries 09/07/27*/
	startIdx = idxMin; endIdx = idxMax; deltaIdx=1;
	/*e Modify by Aries 09/07/27*/
	for (agcidx=startIdx; agcidx<=endIdx; agcidx+=deltaIdx) {
#if 1
		/*- Get preview luma */
		sp5kPreviewExpAgcSet(expidx, agcidx);
		sp5kSensorVvalidWait();
		sp5kSensorVvalidWait();

		aeLuma = tbUtil_PvRawLumaGet(); 
#else
		sp5kStillCapExpAgcSet(expidx, agcidx);			
		aeLuma = tbUtil_CapRawLumaGet();
#endif
		/*-- Store and show result */
		tbLogBuf_Add(agcidx);
		tbLogBuf_Add(aeLuma);

		//dbgOsd_Print(4, "AE Luma(%d), %d", aeLuma, agcidx);
		printf("AE Luma = %d @ AgcIdx(%d) \n", aeLuma, agcidx);
	}

	/*-- Store result to DISK */
	tbLogBuf_PtrReset();

	tbLogFile_Print("\n----- Gain Table Limitation ---------------\n");
	tbLogFile_Print("Tbl Min : %d \n", idxMin);
	tbLogFile_Print("Tbl Max : %d \n", idxMax);
	tbLogFile_Print("Tested @ expidx(%d) \n", expidx);
	tbLogFile_Print("\n\n");

	tbLogFile_Print("agcidx Luma\n");
	for (agcidx=startIdx; agcidx<=endIdx; agcidx+=deltaIdx)
	{
		UINT32 idx, luma;
		idx = tbLogBuf_Get();
		luma = tbLogBuf_Get();
		tbLogFile_Print("%3d %d \n", idx, luma);
	}

	tbLogFile_Close();

	tbUtil_QuickSweepHostMessage(100);

}


/*-------------------------------------------------------------------------
 * Function : tbSensor_ObScan
 *------------------------------------------------------------------------*/
void tbSensor_ObScan(void)
{
	SINT32 ob, agcidx;
	SINT16 luma0, luma1, luma2, luma3;
    SINT32 idxMax, idxMin;
	SINT32 startIdx, endIdx, deltaIdx;

	/*-- Set View Config */
	tbUtil_ViewEnter();
	tbUtil_CalibCdspSet(0/*isObOn*/);

	/*-- AE/AWB Off */
	tbUtil_AeAwb_Dark_Off();

	/*-- Large IRIS */
	tbUtil_IrisCtrl(1/*isLarge*/);

	/*-- Retrieve AGC limitation */
	tbUtil_ViewAgcLimtGet(&idxMax, &idxMin);

	/*-- Clean Data Buf */
	tbLogBuf_Clear();

	/*-- Loop ExpT test */
	startIdx = idxMin; endIdx = idxMax; deltaIdx=4;
	for (agcidx=startIdx; agcidx<=endIdx; agcidx+=deltaIdx) {
		sp5kPreviewExpAgcSet(CALIB_EXP_1_32, agcidx);
		sp5kStillCapExpAgcSet(CALIB_EXP_1_32,agcidx);
		sp5kSensorVvalidWait();
		sp5kSensorVvalidWait();

		/*-- Close Shutter */
		tbUtil_MShutterCtrl(1/*isClose*/);

		/*-- Calib STILL_RAW */
		ob = tbUtil_CapRawLumaGet();
		appGetRawAvgRGGB(&luma0, &luma1, &luma2, &luma3);

		/*-- Open MShutter */
		tbUtil_MShutterCtrl(0/*isClose*/);

		dbgOsd_Print(2, "OB: %d", ob);
		dbgOsd_Print(3, "%d", luma0);
		dbgOsd_Print(4, "%d", luma1);
		dbgOsd_Print(5, "%d", luma2);
		dbgOsd_Print(6, "%d", luma3);

		tbLogBuf_Add(agcidx);
		tbLogBuf_Add(ob);
		printf("%d %d \n", agcidx, ob);
	}

	/*-- AE/AWB On */
	sp5kPreviewExpAgcSet(CALIB_EXP_1_32, 20);

	tbLogBuf_PtrReset();
	tbLogFile_PrintEx(FALSE, "\nagcidx OB\n");
	for (agcidx=startIdx; agcidx<=endIdx; agcidx+=deltaIdx) {
		tbLogFile_PrintEx(FALSE, "%d %d\n", tbLogBuf_Get(), tbLogBuf_Get());
	}

	tbUtil_QuickSweepHostMessage(100);

}

/*-------------------------------------------------------------------------*/
#if 0
static void tbSensor_PvLumaCheck(UINT32 loop)
{
 	SINT32 expidx = 96;
	UINT32 agcidx = 0;
	UINT16 aeLuma,i;

	/*-- Set View Config */
	tbUtil_ViewEnter();

	/*-- AE/AWB Off */
	tbUtil_AeAwb_Dark_Off();

	/*-- OB Subtraction */
	tbUtil_ObApply();

	/*-- Large IRIS */
	tbUtil_IrisCtrl(1/*islarge*/);	

	/*-- Open log file */
	if (!tbLogFile_Open("D:\\PVLUMA.TXT")) return;
	
	/*-- Clean Data Buf */
	tbLogBuf_Clear();
	
	/*-- Loop ExpT test */
	for (i=0; i<=loop; i++) {
		/*- Get preview luma */
		sp5kPreviewExpAgcSet(expidx, agcidx);
		sp5kSensorVvalidWait();
		sp5kSensorVvalidWait();

		aeLuma = tbUtil_PvRawLumaGet();

		/*-- Store and show result */
		tbLogBuf_Add(i);
		tbLogBuf_Add(aeLuma);

		printf("AE Luma = %d @ loop(%d) \n", aeLuma, i);
	}

	/*-- Store result to DISK */
	tbLogBuf_PtrReset();

	tbLogFile_Print("============ PV Luma Get ===============\n\n");
	tbLogFile_Print("Tested @ agcidx=%d, expidx=%d", agcidx, expidx);
	tbLogFile_Print("\n\n");

	tbLogFile_Print("pv Luma\n");
	for (i=0; i<=loop; i++) 
	{
		UINT32 idx, luma;
		idx = tbLogBuf_Get();
		luma = tbLogBuf_Get();
		tbLogFile_Print("%3d %d \n", idx, luma);
	}

	tbLogFile_Close();

	tbUtil_QuickSweepHostMessage(100);

}
#endif
/*-------------------------------------------------------------------------
 * Function : KeySET_TBSENSOR_P2
 *------------------------------------------------------------------------*/
static void KeySET_TBSENSOR_P2(UINT32 param)
{
	tbSensorTestSelect = TBSENSOR_P2_SelectGet();
	switch (tbSensorTestSelect) {
	default:
		break;
	case TBSENSOR_P2_SEL_BACK:
		CLEAR_OSD_SRC;
		TB_SensorTest_StateExit(APP_STATE_TEST_MAIN, STATE_PARAM_NORMAL_INIT);
		break;
	case TBSENSOR_P2_SEL_CAP_EXP_TBL_SCAN:
		CLEAR_OSD_SRC;
		//tbSensor_TestCalibRawCb();
		tbSensor_CapExpTblScan(0);
		TBSENSOR_P2_REDRAW();
		break;
	case TBSENSOR_P2_SEL_PV_EXP_TBL_SCAN:
		CLEAR_OSD_SRC;
		tbSensor_ExpTableScan();
		TBSENSOR_P2_REDRAW();
		break;
	case TBSENSOR_P2_SEL_AGC_TBL_SCAN:
		CLEAR_OSD_SRC;
		tbSensor_AgcTableScan();
		TBSENSOR_P2_REDRAW();
		break;
	case TBSENSOR_P2_SEL_SHUT_DLY:
		CLEAR_OSD_SRC;		
		tbMsh_ShutterCloseLagTest(1,0);
		TBSENSOR_P2_REDRAW();		
		break;
	case TBSENSOR_P2_SEL_MORE:
		CLEAR_OSD_SRC;
		/*-- Create SUB-Menu */
		//TBSENSOR_P2_Config();
		//TBSENSOR_P2_Show();
		//pfunc_KeySet_Process = KeySET_TBSENSOR_P2;
	#if 0		
		tbAE_EvBiasTest();	
	#endif
		TBSENSOR_P2_REDRAW();
		break;
	}

}

/*-------------------------------------------------------------------------
 * Function : KeySET_TBSENSOR_P1
 *------------------------------------------------------------------------*/
static void KeySET_TBSENSOR_P1(UINT32 param)
{
	tbSensorTestSelect = TBSENSOR_P1_SelectGet();
	switch (tbSensorTestSelect) {
	default:
		break;
	case TBSENSOR_P1_SEL_BACK:
		CLEAR_OSD_SRC;
		TB_SensorTest_StateExit(APP_STATE_TEST_MAIN, STATE_PARAM_NORMAL_INIT);
		break;
	case TBSENSOR_P1_SEL_AE_LV_6_15:
		CLEAR_OSD_SRC;
		tbSensor_LV4to17(1/*isPv*/);
		TBSENSOR_P1_REDRAW();
		break;
	case TBSENSOR_P1_SEL_CAP_LV_6_15:
		CLEAR_OSD_SRC;
		tbSensor_LV4to17(0/*isPv*/);
		TBSENSOR_P1_REDRAW();
		break;
	case TBSENSOR_P1_SEL_IRIS_DIFF:
		CLEAR_OSD_SRC;
		tbSensor_IrisSensDiff();
		TBSENSOR_P1_REDRAW();
		break;
	case TBSENSOR_P1_SEL_MINI_GAIN:
		CLEAR_OSD_SRC;
		tbSensor_CapMiniGainTest();
		TBSENSOR_P1_REDRAW();
		break;
	case TBSENSOR_P1_SEL_MORE:
		CLEAR_OSD_SRC;
		/*-- Create SUB-Menu */
		TBSENSOR_P2_Config();
		TBSENSOR_P2_Show();
		pfunc_KeySet_Process = KeySET_TBSENSOR_P2;
		break;
	}

}

/*-------------------------------------------------------------------------
 * Function : TB_SensorTest_StateInit
 *------------------------------------------------------------------------*/
static void TB_SensorTest_StateInit(void)
{
	CLEAR_OSD_SRC;

	/*-- Create tbLogBuf */
	tbLogBuf_Create();

	/*-- Create SUB-Menu */
	menuPosBackup = MENU_MARK_TESTBENCH | 0x0a0d0201; 
	menuProcCurrPosSet (menuPosBackup);
	menuProcDisp();

	pfunc_KeySet_Process = KeySET_TBSENSOR_P1;
}

/*-------------------------------------------------------------------------
 * Function : TB_SensorTest_StateClose
 *------------------------------------------------------------------------*/
static void TB_SensorTest_StateClose(void)
{
	/*-- Destroy tbLogBuf */
	tbLogBuf_Destroy();
}

/*-------------------------------------------------------------------------
 * Function : TB_SensorTest_StateExit
 *------------------------------------------------------------------------*/
static void TB_SensorTest_StateExit(UINT32 nxtSt, UINT32 param)
{
	TB_SensorTest_StateClose();
	appStateChange(nxtSt, param);
}

/*-------------------------------------------------------------------------
 * Function : TB_SensorTest_StateOnMsg
 *------------------------------------------------------------------------*/
static void TB_SensorTest_StateOnMsg(UINT32 msg, UINT32 param)
{
	switch (msg) {
	default:
		break;
	case SP5K_MSG_AE_READY:
		//appAePreviewAdjust(param);
		break;
	case SP5K_MSG_AWB_READY:
		/*appAwbPreviewAdjust(param);*/
		break;
	}
}

/*-------------------------------------------------------------------------
 * Function : TB_SensorTest_StateOnKey
 *------------------------------------------------------------------------*/
static void TB_SensorTest_StateOnKey(UINT32 msg, UINT32 param)
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
		//KeySET_TBSENSOR_P1(param);
		pfunc_KeySet_Process(param);
		break;

	case APP_KEY_PRESS_MENU:
		//beep_echo();
		//Key_MENU_Service(param);
		break;

	case APP_KEY_PRESS_DEL:
		TB_SensorTest_StateExit(APP_STATE_TEST_MAIN, STATE_PARAM_NORMAL_INIT);
		break;
	case APP_KEY_PRESS_LEFT:
	case APP_KEY_PRESS_RIGHT:
	default:
		break;
	}
}

/*-------------------------------------------------------------------------
 * Function : TB_SensorTest_StateMain
 *------------------------------------------------------------------------*/
void TB_SensorTest_StateMain(UINT32 msg, UINT32 param)
{
	switch (msg) {
	case APP_STATE_MSG_INIT:
		RPrintf("TB_SensorTest_StateMain");
		TB_SensorTest_StateInit();
		break;
	case APP_STATE_MSG_CLOSE:
		TB_SensorTest_StateClose();
		appStateCloseDone();
		break;
	default:
		if (IS_APP_KEY_MSG(msg)) {
			TB_SensorTest_StateOnKey(msg,param);
		}
		else{
			TB_SensorTest_StateOnMsg(msg,param);
		}
		break;
	}

}
/*-------------------------------------------------------------------------
 * Function : tbSensor_LVScanTest
 *------------------------------------------------------------------------*/
void tbSensor_LVScanTest(BOOL isPv)
{
    SINT32 curLV;
    SINT32 startLV, endLV, deltaLV;

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

	/*-- Open Log File */
	if (isPv==1) {
		if (!tbLogFile_Open("D:\\PV_LVSCN.TXT")) return;
	} else {
		if (!tbLogFile_Open("D:\\CP_LVSCN.TXT")) return;
	}

	/*-- Clear & reset tbBuf */
	tbLogBuf_Clear();

	/*-- LOOP 1 -------------------------------------------------------
	 * LV 17 - 4
	 */
	startLV=160; endLV=100; deltaLV=10;
	for (curLV = startLV; curLV>=endLV; curLV-=deltaLV) {
		if (FAIL==_lv4to17_scan(isPv, curLV,0)) goto _exit;
	}

	#if 0
	/*-- LOOP 2 -------------------------------------------------------
	 * LV 4 - 17
	 */
	tbUtil_BeepWarn();
	CLEAR_OSD_SRC;
	dbgOsd_Print(4, "LV 4 - 17"); APP_SLEEP_MS(1500);
	printf("\n****** ((((( Revert Scan LV 4 ~ 17 ))))) ******\n\n");

	startLV=40; endLV=170; deltaLV=10;
	for (curLV = startLV; curLV<=endLV; curLV+=deltaLV) {
		if (FAIL==_lv4to17_scan(isPv, curLV,0)) goto _exit;
	}
	#endif

_exit:
	/*-- Save log -------------------------------------------------------
	 */
	tbLogBuf_PtrReset();

	if (isPv==TRUE) {
		tbLogFile_Print("\nLV Scan (VIEW) : 17.0 ~ 4.0 \n\n");
		tbLogFile_Print("LV   exp agc iris stable lvidx RawLuma\n");
	} else {
		tbLogFile_Print("\nLV Scan (CAPTURE): 17.0 ~ 4.0 \n\n");
		tbLogFile_Print("LV   exp agc iris stable lvidx RawLuma\n");
	}

	startLV=160; endLV=100; deltaLV=10;
	for (curLV = startLV; curLV>=endLV; curLV-=deltaLV) {
		_lv4to17_save(isPv);
	}

	#if 0
	if (isPv==TRUE) {
		tbLogFile_Print("\nLV Scan (VIEW) : 4.0 ~ 17.0 \n\n");
		tbLogFile_Print("LV   exp agc iris stable lvidx RawLuma\n");
	} else {
		tbLogFile_Print("\nLV Scan (CAPTURE): 4.0 ~ 17.0 \n\n");
		tbLogFile_Print("LV   exp agc iris stable lvidx RawLuma\n");
	}

	startLV=40; endLV=170; deltaLV=10;
	for (curLV = startLV; curLV<=endLV; curLV+=deltaLV) {
		_lv4to17_save(isPv);
	}
	#endif

	tbLogFile_Close();

	CLEAR_OSD_SRC;
	dbgOsd_Print(4, "File Saved ...");
	tbUtil_QuickSweepHostMessage(3000);

	return;
}


void cmdSENSOR(UINT32 argc, 	UINT8 *arg[], UINT32 v[])
{
	//SINT32 exp;
	//UINT32 agc;
	if (argc==1) {
		printf("sensor usage:\n"
			" sensor pvMinGain : search PV min saturation gain \n"
			" sensor capExpTbl : scan capture exp table \n"
			" sensor obscan : scan OB for various agcidx \n"
		);
	} else if (strcmp(arg[1], "pvMinGain")==0) {
		printf("sensor capExpTbl ...\n");
		tbSensor_PvMiniGainTest();
	} else if (strcmp(arg[1], "capExpTbl")==0) {		
		printf("sensor capExpTbl @ Iris : %d ...\n", v[2]);
		tbSensor_CapExpTblScan(v[2]);
	} else if (strcmp(arg[1], "obscan")==0) {
		printf("sensor obscan ...\n");
		tbSensor_ObScan();
	}else {
		//appAeEnable(v[1]);
	}
}




