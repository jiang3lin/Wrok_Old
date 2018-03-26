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
#include <math.h>
#include "Sp5k_capture_api.h"
#include "Sp5k_modesw_api.h"
#include "Sp5k_global_api.h"
#include "Sp5k_cal_api.h"
#include "Sp5k_cdsp_api.h"
#include "Sp5k_modesw_api.h"
#include "Sp5k_sensor_api.h"
#include "Sp5k_rsvblk_api.h"
#include "Sp5k_cal_api.h"
#include "sp5k_disp_api.h"
#include "Sp5k_gfx_api.h"
#include "Sp5k_af_api.h"

#include "customization\dev_init.h"

#include "app_sys_def.h"
#include "app_lens_api.h"
#include "app_still.h"
#include "app_com_def.h"
#include "app_com_api.h"
#include "app_calib_api.h"
#include "app_osd.h"
#include "app_osd_api.h"
#include "app_jpg_draw.h"
#include "app_res_def.h"
#include "app_lens_api.h"
#include "app_calib_private.h"

#include "app_tb_util.h"
#include "app_simple_menu.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#if SP5K_SENSOR_TG_AD9000_SHARPRJ23V3B
#define OB_CALIB_ISO				(40)
#define CALIB_MSHUT_ISO				(39)
#define CALIB_MSHUT_LEAK_DELAY		(3210) // test close @ 1/1024 (expidx=160)
#define CALIB_MSHUT_DEFAULT_DELAY	(3260+49) // idea close @ 1/2048 (expidx=176)
#define CALIB_MSHUT_LEAK_EXP 		(CALIB_EXP_1_1024)
#define CALIB_MSHUT_EXTEND_EXP 		(CALIB_MSHUT_LEAK_EXP-24)
#define CALIB_ISO_PV_EXP_OFS		(0)
#define CALIB_ISO_CAP_EXP_OFS		(0)
#define CALIB_ISO_PV_EXP		(CALIB_EXP_1_128) //Large Iris = F2.8
#define CALIB_ISO_CAP_EXP		(CALIB_EXP_1_128) //Large Iris = F2.8
#define CAL_PVOB_DEFAULT_INIT_EXPLINE  (500)
#define CAL_PVOB_DEFAULT_MIN_EXPLINE  (2)
#define CAL_PVOB_DEFAULT_TAG_LUMA (23000)
#elif SP5K_SENSOR_TG_AD9000_SHARPRJ23U3B
#define OB_CALIB_ISO				(40)
#define CALIB_MSHUT_ISO				(OB_CALIB_ISO)
#define CALIB_MSHUT_LEAK_DELAY 		(3222)
#define CALIB_MSHUT_DEFAULT_DELAY	(3276+49) // ideal close @ 1/2000 (expidx=176)
#define CALIB_MSHUT_LEAK_EXP 		(CALIB_EXP_1_1024)
#define CALIB_MSHUT_EXTEND_EXP 		(CALIB_MSHUT_LEAK_EXP-24)
#define CALIB_ISO_PV_EXP_OFS		(0)
#define CALIB_ISO_CAP_EXP_OFS		(0)
#define CALIB_ISO_PV_EXP		(CALIB_EXP_1_128) //Large Iris = F2.8
#define CALIB_ISO_CAP_EXP		(CALIB_EXP_1_128) //Large Iris = F2.8
#define CAL_PVOB_DEFAULT_INIT_EXPLINE  (500)
#define CAL_PVOB_DEFAULT_MIN_EXPLINE  (2)
#define CAL_PVOB_DEFAULT_TAG_LUMA (23000)
#elif SP5K_SENSOR_TG_AD9000_SONY675
#define OB_CALIB_ISO				(40)
#define CALIB_MSHUT_ISO				(32)
#define CALIB_MSHUT_LEAK_DELAY		(3200) // test close @ 1/1024 (expidx=160)
#define CALIB_MSHUT_DEFAULT_DELAY	CALIB_MSHUT_LEAK_DELAY // idea close @ 1/2048 (expidx=176)
#define CALIB_MSHUT_LEAK_EXP 		(CALIB_EXP_1_1024)
#define CALIB_MSHUT_EXTEND_EXP 		(CALIB_MSHUT_LEAK_EXP-16)
#define CALIB_ISO_PV_EXP_OFS		(32)//(16)//(0)
#define CALIB_ISO_CAP_EXP_OFS		(0)
#define CALIB_ISO_PV_EXP		(CALIB_EXP_1_128) //Large Iris = F2.8
#define CALIB_ISO_CAP_EXP		(CALIB_EXP_1_128) //Large Iris = F2.8
#define CAL_PVOB_DEFAULT_INIT_EXPLINE  (500)
#define CAL_PVOB_DEFAULT_MIN_EXPLINE  (2)
#define CAL_PVOB_DEFAULT_TAG_LUMA (23000)
#elif SP5K_SENSOR_TG_PANX12081A_MN34510
#define OB_CALIB_ISO				(10)
#define CALIB_MSHUT_ISO				(0)
#define CALIB_MSHUT_LEAK_DELAY 		(5200)
#define CALIB_MSHUT_DEFAULT_DELAY	CALIB_MSHUT_LEAK_DELAY
#define CALIB_MSHUT_LEAK_EXP 		(CALIB_EXP_1_1024)
#define CALIB_MSHUT_EXTEND_EXP 		(CALIB_MSHUT_LEAK_EXP-16)
#define CALIB_ISO_PV_EXP_OFS		(40)
#define CALIB_ISO_CAP_EXP_OFS		(0)
#define CALIB_ISO_PV_EXP		(CALIB_EXP_1_128) //Large Iris = F2.8
#define CALIB_ISO_CAP_EXP		(CALIB_EXP_1_128) //Large Iris = F2.8
#define CAL_PVOB_DEFAULT_INIT_EXPLINE  (500)
#define CAL_PVOB_DEFAULT_MIN_EXPLINE  (2)
#define CAL_PVOB_DEFAULT_TAG_LUMA (23000)
#elif SP5K_SENSOR_MI9130 /* A900 */
#define OB_CALIB_ISO				(32)
#define CALIB_MSHUT_ISO				cap_svidx_to_agcidx(80)//(0)
#define CALIB_MSHUT_LEAK_DELAY 		(0)
#define CALIB_MSHUT_DEFAULT_DELAY	CALIB_MSHUT_LEAK_DELAY
#define CALIB_MSHUT_LEAK_EXP 	              (CALIB_EXP_1_512)
#define CALIB_MSHUT_EXTEND_EXP 		(CALIB_MSHUT_LEAK_EXP-16)
#define CALIB_ISO_PV_EXP_OFS		(0)
#define CALIB_ISO_CAP_EXP_OFS		(0)
#if 0 //old lens
#define CALIB_ISO_PV_EXP		(CALIB_EXP_1_64 + 8) //Large Iris = F4.0
#define CALIB_ISO_CAP_EXP		(CALIB_EXP_1_64 + 8) //Large Iris = F4.0
#else //new lens
#define CALIB_ISO_PV_EXP		(CALIB_EXP_1_128) //Large Iris = F2.8
#define CALIB_ISO_CAP_EXP		(CALIB_EXP_1_128) //Large Iris = F2.8
#endif
#define CAL_PVOB_DEFAULT_INIT_EXPLINE  (500)
#define CAL_PVOB_DEFAULT_MIN_EXPLINE  (2)
#define CAL_PVOB_DEFAULT_TAG_LUMA (23000)
#elif SP5K_SENSOR_OV5610
#define OB_CALIB_ISO				(10)
#define CALIB_MSHUT_ISO				(CALIB_ISO_100)
#define CALIB_MSHUT_LEAK_DELAY 		(5208) /*45Mhz, 1/1024*/
#define CALIB_MSHUT_DEFAULT_DELAY	(5284+24)
#define CALIB_MSHUT_LEAK_EXP 		(CALIB_EXP_1_1024)
#define CALIB_MSHUT_EXTEND_EXP 		(CALIB_MSHUT_LEAK_EXP-48)
#define CALIB_ISO_PV_EXP_OFS		(0)
#define CALIB_ISO_CAP_EXP_OFS		(0)
#define CALIB_ISO_PV_EXP		(CALIB_EXP_1_128) //Large Iris = F2.8
#define CALIB_ISO_CAP_EXP		(CALIB_EXP_1_128) //Large Iris = F2.8
#define CAL_PVOB_DEFAULT_INIT_EXPLINE  (500)
#define CAL_PVOB_DEFAULT_MIN_EXPLINE  (2)
#define CAL_PVOB_DEFAULT_TAG_LUMA (23000)
#elif SP5K_SENSOR_OV5620 /* A531*/
#define OB_CALIB_ISO				(10)
#define CALIB_MSHUT_ISO				(41)
#define CALIB_MSHUT_LEAK_DELAY 		(0)
#define CALIB_MSHUT_DEFAULT_DELAY	CALIB_MSHUT_LEAK_DELAY
#define CALIB_MSHUT_LEAK_EXP 	              (CALIB_EXP_1_512)
#define CALIB_MSHUT_EXTEND_EXP 		(CALIB_MSHUT_LEAK_EXP-16)
#define CALIB_ISO_PV_EXP_OFS		(0)
#define CALIB_ISO_CAP_EXP_OFS		(0)
#define CALIB_ISO_PV_EXP		(CALIB_EXP_1_128) //Large Iris = F2.8
#define CALIB_ISO_CAP_EXP		(CALIB_EXP_1_128) //Large Iris = F2.8
#define CAL_PVOB_DEFAULT_INIT_EXPLINE  (500)
#define CAL_PVOB_DEFAULT_MIN_EXPLINE  (2)
#define CAL_PVOB_DEFAULT_TAG_LUMA (23000)
#elif SP5K_SENSOR_OV9810
#define OB_CALIB_ISO				(10)
#define CALIB_MSHUT_ISO				(41)
#define CALIB_MSHUT_LEAK_DELAY 		(0)
#define CALIB_MSHUT_DEFAULT_DELAY	CALIB_MSHUT_LEAK_DELAY
#define CALIB_MSHUT_LEAK_EXP 	              (CALIB_EXP_1_512)
#define CALIB_MSHUT_EXTEND_EXP 		(CALIB_MSHUT_LEAK_EXP-16)
#define CALIB_ISO_PV_EXP_OFS		(0)
#define CALIB_ISO_CAP_EXP_OFS		(0)
#define CALIB_ISO_PV_EXP		(CALIB_EXP_1_128) //Large Iris = F2.8
#define CALIB_ISO_CAP_EXP		(CALIB_EXP_1_128) //Large Iris = F2.8
#define CAL_PVOB_DEFAULT_INIT_EXPLINE  (500)
#define CAL_PVOB_DEFAULT_MIN_EXPLINE  (2)
#define CAL_PVOB_DEFAULT_TAG_LUMA (23000)
#elif SP5K_SENSOR_OV5633
#define OB_CALIB_ISO				(10)
#define CALIB_MSHUT_ISO				(41)
#define CALIB_MSHUT_LEAK_DELAY 		(0)
#define CALIB_MSHUT_DEFAULT_DELAY	CALIB_MSHUT_LEAK_DELAY
#define CALIB_MSHUT_LEAK_EXP 	              (CALIB_EXP_1_512)
#define CALIB_MSHUT_EXTEND_EXP 		(CALIB_MSHUT_LEAK_EXP-16)
#define CALIB_ISO_PV_EXP_OFS		(0)
#define CALIB_ISO_CAP_EXP_OFS		(0)
#define CALIB_ISO_PV_EXP		(CALIB_EXP_1_128) //Large Iris = F2.8
#define CALIB_ISO_CAP_EXP		(CALIB_EXP_1_128) //Large Iris = F2.8
#define CAL_PVOB_DEFAULT_INIT_EXPLINE  (500)
#define CAL_PVOB_DEFAULT_MIN_EXPLINE  (2)
#define CAL_PVOB_DEFAULT_TAG_LUMA (23000)
#elif SP5K_SENSOR_OV5653
#define OB_CALIB_ISO				(10)
#define CALIB_MSHUT_ISO				(41)
#define CALIB_MSHUT_LEAK_DELAY 		(0)
#define CALIB_MSHUT_DEFAULT_DELAY	CALIB_MSHUT_LEAK_DELAY
#define CALIB_MSHUT_LEAK_EXP 	              (CALIB_EXP_1_512)
#define CALIB_MSHUT_EXTEND_EXP 		(CALIB_MSHUT_LEAK_EXP-16)
#define CALIB_ISO_PV_EXP_OFS		(0)
#define CALIB_ISO_CAP_EXP_OFS		(0)
#define CALIB_ISO_PV_EXP		(CALIB_EXP_1_128) //Large Iris = F2.8
#define CALIB_ISO_CAP_EXP		(CALIB_EXP_1_128) //Large Iris = F2.8
#define CAL_PVOB_DEFAULT_INIT_EXPLINE  (200)
#define CAL_PVOB_DEFAULT_MIN_EXPLINE  (2)
#define CAL_PVOB_DEFAULT_TAG_LUMA (23000)

#else
#define OB_CALIB_ISO				(40)
#define CALIB_MSHUT_ISO				(OB_CALIB_ISO)
#define CALIB_MSHUT_LEAK_DELAY 		(5208) /*45Mhz, 1/1024*/
#define CALIB_MSHUT_DEFAULT_DELAY	(5284+24)
#define CALIB_MSHUT_LEAK_EXP 		(CALIB_EXP_1_1024)
#define CALIB_MSHUT_EXTEND_EXP 		(CALIB_MSHUT_LEAK_EXP-48)
#define CALIB_ISO_PV_EXP_OFS		(0)
#define CALIB_ISO_CAP_EXP_OFS		(0)
#define CALIB_ISO_PV_EXP		(CALIB_EXP_1_128) //Large Iris = F2.8
#define CALIB_ISO_CAP_EXP		(CALIB_EXP_1_128) //Large Iris = F2.8
#define CAL_PVOB_DEFAULT_INIT_EXPLINE  (500)
#define CAL_PVOB_DEFAULT_MIN_EXPLINE  (2)
#define CAL_PVOB_DEFAULT_TAG_LUMA (23000)
#endif


/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define WRITE8(a,b)	*(volatile UINT8*)(a) = (b)


/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/


/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
static UINT8  buf[32];
static UINT16 aaaCalibRawCbLuma[4];
static UINT16 aaaCalibAvgLuma[5]; //[0] r [1]gr [2]gb [3]b [4]==tag
static UINT32 luma[4];
static appCalibData_t calibdata;
static appAfCalibData_t afCalibdata;
static appAfCalibData_t def2Mdata = {  {FOCUS_0_2M + FOCUS_POSITION_OFFSET,
										FOCUS_1_2M + FOCUS_POSITION_OFFSET,
										FOCUS_2_2M + FOCUS_POSITION_OFFSET,
										FOCUS_3_2M + FOCUS_POSITION_OFFSET,
										FOCUS_4_2M + FOCUS_POSITION_OFFSET,
										FOCUS_5_2M + FOCUS_POSITION_OFFSET,
										FOCUS_6_2M + FOCUS_POSITION_OFFSET} };

static UINT32 CaliAfStatus;
static UINT32 CaliZlevel;
static UINT16 CaliAfScanInfo[ZOOM_ZS_NUM+1][3];
UINT8 calPVOB = 0;
static UINT8 calPVOBSaveRaw = 0;
static UINT32 calPVOBTagLuma = CAL_PVOB_DEFAULT_TAG_LUMA;
static UINT16 calPVOBInitExp = CAL_PVOB_DEFAULT_INIT_EXPLINE;
static UINT16 calPVOBMinExp = CAL_PVOB_DEFAULT_MIN_EXPLINE;
static UINT32 calPVOBAgcIdx = 0xffffffff;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern UINT8 proc_awb_calib;
extern UINT8 CalibMode;
extern UINT32 appStillWbCb(UINT32 addr,UINT32 rawfmt,void* size);
extern SINT32 cap_svidx_to_agcidx(SINT32 svidx);

void dbgOsd_Print(UINT32 row, const char *format, ...);
UINT16 appAeTargetGet(void);
void appAeTargetSet(UINT8 target);
SINT32 appAeMinCapGainGet(void);
void tbSensor_MShut_SUB_Test(SINT32 exp, UINT32 delay);
extern UINT32 dbgTimeGet(void);//haiyan.he added
extern SINT32 pv_svidx_to_agcidx(SINT32 svidx);//haiyan.he added
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
void
_calibDataDefaultSet(
	void
)
{
    UINT32 i;
    
	DBG_PRINT("[WARN] set calibration data as default settings\n");
	/*calibdata.UsbSetting = FALSE;*/
	calibdata.AfSign = FALSE;
	calibdata.ObSign = FALSE;

    for (i=0;i<COUNT_OF(calibdata.Ob); i++) {
		calibdata.Ob[i] = CALIB_AE_DEFAULT_OB;
	}
	calibdata.IsoSign = FALSE;
	calibdata.PvIso = CALIB_ISO_100;
	calibdata.CapIso = CALIB_ISO_100;
	calibdata.ShutterSign = FALSE;
	calibdata.ShutterEarly[0] = CALIB_AE_DEFAULT_SHUTTER_EARLY_1;
	calibdata.ShutterEarly[1] = CALIB_AE_DEFAULT_SHUTTER_EARLY_2;
	calibdata.WbSign = FALSE;
	calibdata.WbRgain = CALIB_WB_DEFAULT_RGAIN;
	calibdata.WbBgain = CALIB_WB_DEFAULT_BGAIN;
	calibdata.FlashSign = FALSE;
	calibdata.Flash = FALSE;
	calibdata.PvDPCSign = FALSE;
	calibdata.CapDPCSign = FALSE;
	calibdata.MTBFSign = FALSE;
	calibdata.MTBF = FALSE;
	calibdata.LcdSign = FALSE;
	calibdata.PvRawSign = FALSE;
	calibdata.StillRawSign = FALSE;
	calibdata.CIPASign = FALSE;
	calibdata.BatterySign = FALSE;
	calibdata.rawBatchEnable = FALSE;
	calibdata.IsoObSign = FALSE;

}

void
_calibAfDataDefaultSet(
	void
)
{
	DBG_PRINT("[WARN] set AF calibration data as default settings\n");
	afCalibdata.AfPosition[0]= 0;
	afCalibdata.AfPosition[1]= 0;
	afCalibdata.AfPosition[2]= 0;
	afCalibdata.AfPosition[3]= 0;
	afCalibdata.AfPosition[4]= 0;
	afCalibdata.AfPosition[5]= 0;
	afCalibdata.AfPosition[6]= 0;
}

UINT32
_calibDataSave(
	void
)
{
	DBG_PRINT("[Calib Data Save...]\n");
	if(WriteFile(RES_CALIB_DATA, (UINT8*)(&calibdata), sizeof(appCalibData_t))!=SUCCESS) {
		DBG_PRINT("[ERR] write file[%s] fail\n", RES_CALIB_DATA);
		return FAIL;
	}
	return SUCCESS;

}

UINT32
_calibAfDataSave(
	void
)
{
	DBG_PRINT("[AF Calib Data Save...]\n");
	if(WriteFile(RES_CALIB_AF_DATA, (UINT8*)(&afCalibdata), sizeof(appAfCalibData_t))!=SUCCESS) {
		DBG_PRINT("[ERR] write file[%s] fail\n", RES_CALIB_AF_DATA);
		return FAIL;
	}
	return SUCCESS;

}

/*-------------------------------------------------------------------------
 * Function : _calibOb
 *------------------------------------------------------------------------*/
UINT32 _calibOb(UINT32 expIdx,UINT32 agcIdx, SINT32 *retOb)
{
	UINT32 ret = SUCCESS;
	SINT32 ob;

	DBG_PRINT("\n**************************\n"
			  " cali: OB \n"
			  "**************************\n"
			  );

	/*-- Set View Config */
	tbUtil_ViewEnter();
	#if SPCA5330
	sp5kIqModeSet(SP5K_MODE_STILL_SNAP);
	tbUtil_CalibCdspSet(0/*isObOn*/);
	sp5kIqModeSetDone(SP5K_MODE_STILL_SNAP); 
	#else
  	sp5kImgModeSet(SP5K_MODE_STILL_SNAP);
	tbUtil_CalibCdspSet(0/*isObOn*/);
	sp5kImgModeSetDone(SP5K_MODE_STILL_SNAP);
	#endif

	appOsdLib_TextDraw(0,0,SP5K_GFX_ALIGN_TOP_LEFT,"CALIB OB");

	/*-- AE/AWB Off */
	tbUtil_AeAwb_Dark_Off();

	/*-- Large IRIS */
	tbUtil_IrisCtrl(1/*isLarge*/);

	sp5kPreviewExpAgcSet(expIdx,agcIdx);
	sp5kStillCapExpAgcSet(expIdx,agcIdx);

	/*-- Close Shutter */
	tbUtil_MShutterCtrl(1/*isClose*/);

	/*-- Calib STILL_RAW */
	ob = tbUtil_CapRawLumaGet();
#if 1
        {
#define min(a, b) ((a)>(b)?(b):(a))
#define max(a, b) ((a)<(b)?(b):(a))
            UINT16 r = 0, g = 0, b = 0;
            /*UINT32 luma;*/ //haiyan.he added
            appGetRawAvgRGB(&r, &g, &b);
            ob = max(g, max(r, b));
            //ob = min(g, min(r, b));
            //ob = (SINT32)((float)ob * 0.95);

        }
#endif

	#if SPCA5330
	ob=ob>>4;
	#endif


	if (ob>5000) {
		printf("\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
		printf("OB Calib Failed \n");
                dbgOsd_Print(2, "OB: %d", ob);
		printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n");
		dbgOsd_Print(4, "OB Failed");
		ret = FAIL;
		goto _ob_exit;
	}

	dbgOsd_Print(2, "OB: %d", ob);
	dbgOsd_Print(3, "%d", aaaCalibRawCbLuma[0]);
	dbgOsd_Print(4, "%d", aaaCalibRawCbLuma[1]);
	dbgOsd_Print(5, "%d", aaaCalibRawCbLuma[2]);
	dbgOsd_Print(6, "%d", aaaCalibRawCbLuma[3]);

	/*-- Enable OB */
	#if SPCA5330
	sp5kIqModeSet(SP5K_MODE_STILL_SNAP);
	sp5kIqCfgSet(SP5K_IQ_CFG_OB_MODE, SP5K_OB_MANUAL); 
	sp5kIqCfgSet(SP5K_IQ_CFG_OB_MANUAL_VAL, ob);
	tbUtil_CalibCdspSet(1);
	sp5kIqModeSetDone(SP5K_MODE_STILL_SNAP); 

	#else
       sp5kImgModeSet(SP5K_MODE_STILL_SNAP);
	sp5kImgObModeCfgSet(SP5K_OB_MANUAL);
	sp5kImgObSet(ob);
	tbUtil_CalibCdspSet(1);
	sp5kImgModeSetDone(SP5K_MODE_STILL_SNAP);
	#endif
	APP_SLEEP_MS(100);

	#if 1
	printf("\n------------------------------------------\n");
	printf("\nGet Raw Luma again after OB(%d) is applied!!\n",ob);
	tbUtil_MShutterCtrl(1/*isClose*/);
	/*tbUtil_CalibCdspSet(1);	*/
	tbUtil_CapRawLumaGet();
	printf("\n------------------------------------------\n");
	#endif

	/*-- Open MShutter */
	tbUtil_MShutterCtrl(0/*isClose*/);

	/*-- AE/AWB On */
	sp5kAeModeSet(SP5K_AE_MODE_INFO_ONLY);
	sp5kAwbModeSet(SP5K_AWB_MODE_INFO_ONLY);

	printf("\n*********************\n");
	printf(" OB = %d \n", ob);
	printf("\n\n");

_ob_exit:
	*retOb = ob;

	return ret;;
}
/*-------------------------------------------------------------------------
 * Function : _calibObAdv
 *------------------------------------------------------------------------*/
void _calibObAdv(UINT32 expidx)
{
	SINT32 ob;
	UINT32 i,j,ret = SUCCESS;
	UINT32 agcmin,agcmax,agctmp;
	UINT32 agcidx = cap_svidx_to_agcidx(80); //ISO agcidx

	tbUtil_SnapAgcLimtGet(&agcmax,&agcmin);
	//ISO 50
	if (agcidx >= SV_UNIT) agctmp = agcidx -SV_UNIT;
	else agctmp =0;

    memset(aaaCalibAvgLuma,0,sizeof(aaaCalibAvgLuma));
	i = 0;

    UINT32 cnt = 0, avg_ob = 0;
    for (cnt = 0; cnt < 5; cnt++) {
        ret = _calibOb(expidx, agctmp, &ob);
        if(ret != SUCCESS){
            goto _exit;
        }
        avg_ob += ob;
    }
    ob = avg_ob / 5;

	calibdata.isoOb[i][0] = ob;
    calibdata.isoOb[i][1] = aaaCalibAvgLuma[0]>>4;
    calibdata.isoOb[i][2] = aaaCalibAvgLuma[1]>>4;
    calibdata.isoOb[i][3] = aaaCalibAvgLuma[2]>>4;
    calibdata.isoOb[i][4] = aaaCalibAvgLuma[3]>>4;
    
	dbgOsd_Print(1, "ISO(0) OB: %d", calibdata.isoOb[i][0]);
	printf("ISO%d: expidx(%d) + agcidx(%d) => ob:%d [ret:%d]\n",i,expidx,agctmp,calibdata.isoOb[i][0],ret);
	tbUtil_QuickSweepHostMessage(3000);

	//ISO 100
	memset(aaaCalibAvgLuma,0,sizeof(aaaCalibAvgLuma));
    
	for (i=1;i<8; i++) {
		agctmp = agcidx+SV_UNIT*(i-1);
        
		if(agctmp > agcmax) 
            break;

        UINT32 cnt = 0, avg_ob = 0;
        for (cnt = 0; cnt < 5; cnt++) {
            ret = _calibOb(expidx, agctmp, &ob);
            if(ret != SUCCESS){
                goto _exit;
            }
            avg_ob += ob;
        }
        ob = avg_ob / 5;
        
		if(ret != SUCCESS){
			goto _exit;
		}
		calibdata.isoOb[i][0] = ob;
        calibdata.isoOb[i][1] = aaaCalibAvgLuma[0]>>4;
        calibdata.isoOb[i][2] = aaaCalibAvgLuma[1]>>4;
        calibdata.isoOb[i][3] = aaaCalibAvgLuma[2]>>4;
        calibdata.isoOb[i][4] = aaaCalibAvgLuma[3]>>4;
		dbgOsd_Print(1, "ISO(%d) OB: %d",i ,calibdata.isoOb[i][0]);
		printf("ISO%d: expidx(%d) + agcidx(%d) => ob:%d [ret:%d]\n",i,expidx,(agcidx+SV_UNIT*i),calibdata.isoOb[i][0],ret);
		tbUtil_QuickSweepHostMessage(3000);
        memset(aaaCalibAvgLuma,0,sizeof(aaaCalibAvgLuma));
	}

	#if SPCA5330
    sp5kImgModeSet(SP5K_MODE_STILL_SNAP);
	sp5kImgObModeCfgSet(SP5K_OB_MANUAL);
	sp5kImgObSet(calibdata.Ob[0]);
	sp5kImgModeSetDone(SP5K_MODE_STILL_SNAP);
	#else
	sp5kImgObModeCfgSet(SP5K_OB_MANUAL);
	sp5kImgObSet(calibdata.Ob[0]);
	#endif

	printf("calibdata.Ob = %d\n",calibdata.Ob[0]);
	for (i=0;i<COUNT_OF(calibdata.isoOb); i++) {
        for(j=0;j<COUNT_OF(calibdata.isoOb[0]);j++)
		  printf("<lester>calibdata.isoOb[%d][%d] = %d\n",i,j,calibdata.isoOb[i][j]);
	}
    
	calibdata.IsoObSign = 1;
    appCalibDataSave();
	return;
_exit:
	calibdata.IsoObSign = 0;
	printf("!!!!!!!!!!!!!_calibObAdv!!!!!!!!!!!!\n");
    printf("\033[41;37m !!!! cali: ISO OB FAIL\033[0m\n");
	dbgOsd_Print(3, "cali: ISO OB FAIL");
}


/*-------------------------------------------------------------------------
 * Function : appCalibMShutter
 *------------------------------------------------------------------------*/
UINT32 _calibMShutter(UINT32 iris,UINT32 leakExpIdx,UINT32 agcidx,UINT32 *shEarly)
{
/*s Modify by Aries 10/02/04*/
	UINT32 loop,loopCount = 10;
	UINT32 avgLuma;
	UINT16 leakLuma, cluma, extLuma,obLuma=0;
	SINT32 extExpIdx;
	UINT32 leakDelay;
	UINT32 isSuccess=TRUE;
	BOOL isLargeIris;

	DBG_PRINT("\n**************************\n"
			  " cali: M Shutter(Iris :%d)\n"
			  "**************************\n",iris );

	/*-- Set View Config */
	tbUtil_ViewEnter();
	tbUtil_CalibCdspSet(1/*isObOn*/);

	/*-- AE/AWB Off */
	tbUtil_AeAwb_Dark_Off();

	/*-- Set IRIS */
	isLargeIris = !iris;
	if(iris ==0) {
		isLargeIris = TRUE;
		dbgOsd_Print(1, "Calib MShut (L Iris)(%d,%d)",leakExpIdx,agcidx);
	}
	else {
		isLargeIris = FALSE;
		dbgOsd_Print(1, "Calib MShut (S Iris)(%d,%d)",leakExpIdx,agcidx);
	}
	tbUtil_IrisCtrl(isLargeIris);

	/*-- OB Subtraction */
	tbUtil_ObApply();

	leakDelay = CALIB_MSHUT_LEAK_DELAY;

	extExpIdx=leakExpIdx - 16;


	sp5kPreviewExpAgcSet(leakExpIdx, agcidx);

	/*-- leakage luma */
	avgLuma=0;
	for (loop=0; loop<loopCount; loop++) 
	{
		sp5kStillCapExpAgcSet(leakExpIdx, agcidx);
		sp5kSnapShutterDelaySet(SP5K_SNAP_SHUTTER_CLOSE_EXP_END, 0, leakDelay);
		sp5kSensorVvalidWait();
		sp5kSensorVvalidWait();

		cluma = tbUtil_CapRawLumaGet();
		avgLuma+=cluma;

		printf("Leak: %d, %d -> %d \n", leakExpIdx, agcidx, cluma);
	}
	leakLuma = avgLuma/loopCount;
	DBG_PRINT("\n****** cali: Luma1(%d) @ idx(%d)\n\n", leakLuma, leakExpIdx);
	dbgOsd_Print(2, "Luma1: %d",leakLuma);

	if (leakLuma > 20000) 
	{
		isSuccess = FALSE;
		goto _fail_exit;
	}
	if (leakLuma <= (obLuma + 15)) 
	{
		isSuccess = FALSE;
		goto _fail_exit;
	}

	/*-- Known extended exposure */
	avgLuma=0;
	for (loop=0; loop<loopCount; loop++) 
	{
		sp5kStillCapExpAgcSet(extExpIdx, agcidx);
		sp5kSnapShutterDelaySet(SP5K_SNAP_SHUTTER_CLOSE_EXP_END, 0, leakDelay);
		sp5kSensorVvalidWait();
		sp5kSensorVvalidWait();

		cluma = tbUtil_CapRawLumaGet();
		avgLuma+=cluma;

		printf("Ext: %d, %d -> %d \n", extExpIdx, agcidx, cluma);
	}
	extLuma = avgLuma/loopCount;
	if (extLuma > 64000) 
	{
		isSuccess = FALSE;
		goto _fail_exit;
	}
	DBG_PRINT("\n****** cali: Luma2(%d) @ idx(%d)\n\n", extLuma, extExpIdx);
	dbgOsd_Print(3, "Luma2: %d",extLuma);

	/*-- Tv = log(2, 1/t) => t = pow(2, 1/Tv) = pow(2, 1/(expidx/16)) */
	double Tv = ((double)leakExpIdx)/TV_UNIT;
	double T1 = pow(2, (0.0-Tv))*1000000.0; //in us
	Tv = ((double)extExpIdx)/TV_UNIT;
	double T2 = pow(2, (0.0-Tv))*1000000.0; //in us
	UINT32 Tx = (UINT32)((T2-T1)/T1);

	float Luma1 = leakLuma; //L1=A+B
	float Luma2 = extLuma; // L2= A+B+xB
	UINT32 lagT = (UINT32)((((Tx+1)*Luma1-Luma2)/(Luma2-Luma1))*T1);

	*shEarly = lagT;

	printf("\n************************************\n");
	printf("** leakExp(%d), extExp(%d), agcidx(%d) \n", leakExpIdx, extExpIdx, agcidx);
	printf("T1(%d),T2(%d) Tx(%d) \n", (UINT32)T1, (UINT32)T2,(UINT32)Tx);
	printf("** L1(%d)@idx(%d), \n", (UINT32)Luma1, leakExpIdx);
	printf("** L2(%d)@idx(%d) \n", (UINT32)Luma2, extExpIdx);
	printf("** lagT(%d) us\n", lagT);

	dbgOsd_Print(4, "lagT: %d",lagT);


	if (lagT > 4000) 
	{
		isSuccess = FALSE;
		printf("!!!! equivalent shutter lag (%d) too long ????? \n", lagT);
		goto _fail_exit;
	}

_fail_exit:

	return isSuccess;
/*e Modify by Aries 10/02/04*/

}

UINT32 _calibLSC(UINT32 opt , UINT32 ratio)
{
	 UINT32 i,ret = SUCCESS;
	 UINT16 logbuf[10];
	 // set capture exposure before calibration
	 // set mechanical shutter callback & delay if mechanical shutter is used

	 UINT value=32;

	 sp5kCalibLscCfgSet(SP5K_CALIB_LSC_STEP,value);

	 sp5kCalibLscCfgSet(SP5K_CALIB_LSC_LUMA_LEVEL, ratio);
	 sp5kCalibLscCfgSet(SP5K_CALIB_LSC_GR_AVG_HIGH, 100);
	 sp5kCalibLscCfgSet(SP5K_CALIB_LSC_LOG_BUF, (UINT32)logbuf); 
     sp5kCalibLscCfgSet(SP5K_CALIB_LSC_OUTPUT_NAME, (UINT32)RES_IQ_LSC);
     
	/*sp5kCalibLscCfgSet(SP5K_CALIB_LSC_OUTPUT_NAME, (UINT32)"D:\\LSC.BIN");*/
	 
	 if (opt == 1){ // load raw for LSC calibration if flag==1
	   printf("Load RAW!\n"); 
		 sp5kCalibLscCfgSet(SP5K_CALIB_LSC_LOAD_RAW, (UINT32)"D:\\LSCDEBUG.RAW");
	 }
	 else if (opt == 2){ // save raw for LSC debug if flag==2
		printf("Capture RAW!\n");
		 sp5kCalibLscCfgSet(SP5K_CALIB_LSC_SAVE_RAW, (UINT32)"D:\\LSCDEBUG.RAW");
	}
	 else{ // no raw load/save otherwise
	   printf("NO RAW!\n");
	   sp5kCalibLscCfgSet(SP5K_CALIB_LSC_SAVE_RAW, 0);
	}
		 
    sp5kIqModeSet(SP5K_MODE_STILL_SNAP);
    sp5kIqBlockEnable(SP5K_IQ_BLOCK_LSC, 0);
    sp5kIqModeSetDone(SP5K_MODE_STILL_SNAP);

	 sp5kCalibrationCfgSet(SP5K_CALIBRATION_LSC, 1);
	 sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 3000);
	 printf("LSC result="); 
	 for (i = 0 ; i < 10 ; i++) {/*0:center x,1:center y,2:step x,3:step y,4:table x number,5:table y number,6:sum of all resulted LSC table entry*/
		 printf("%d ", logbuf[i]);
	 }
	 printf("\n");

	 if(logbuf[7] == 0){
	 	ret = SUCCESS;
	 }
	 else{/*1 : Too bright , 2 : Too Dark*/
		 ret = FAIL;
	 }
	 
	sp5kResourceFileLoad(SP5K_RES_LSC, RES_IQ_LSC);
    sp5kIqModeSet(SP5K_MODE_STILL_PREVIEW);
    sp5kIqBlockEnable(SP5K_CDSP_LSC, 1);
    sp5kIqModeSetDone(SP5K_MODE_STILL_PREVIEW);
	return ret;
}

UINT8 _calibPvOBLinearRegression(float *rgbData, UINT8 rows, float *a, float *b)
{
	UINT8 m;
	float *p, Lxx = 0.0, Lxy = 0.0, xa = 0.0, ya = 0.0;
	if (rgbData == 0 || a == 0 || b == 0 || rows < 1)
		return 0;
	for (p = rgbData, m = 0; m < rows; m ++)
	{
		xa += *p ++;
		ya += *p ++;
	}
	xa /= rows;                                     
	ya /= rows;                                     
	for (p = rgbData, m = 0; m < rows; m ++, p += 2)
	{
		Lxx += ((*p - xa) * (*p - xa));             
		Lxy += ((*p - xa) * (*(p + 1) - ya));       
	}
	*b = Lxy / Lxx;                                 // b = Lxy / Lxx
	*a = ya - *b * xa;                              // a = Ya - b*Xa
	return 1;
}

extern void sensorCmdPortExpLineSend(UINT16 expLine);
void _calibPVOBCfg(UINT32 sel , UINT32 value)
{
	switch(sel){
		case  CAL_PV_OB_SAVE_INFO:
			calPVOBSaveRaw = value;
		break;
		case  CAL_PV_OB_INIT_EXP:
			calPVOBInitExp = value;
		break;		
		case  CAL_PV_OB_MIN_EXP:
			calPVOBMinExp = value;
		break;		
		case  CAL_PV_OB_AGC_IDX:
			calPVOBAgcIdx = value;
		break;	
		case  CAL_PV_OB_TAG_LUMA:
			calPVOBTagLuma = value;
		break;		
		default:
			calPVOBSaveRaw = 0;
			calPVOBTagLuma = CAL_PVOB_DEFAULT_TAG_LUMA;
			calPVOBInitExp = CAL_PVOB_DEFAULT_INIT_EXPLINE;
			calPVOBMinExp = CAL_PVOB_DEFAULT_MIN_EXPLINE;
			calPVOBAgcIdx = 0xffffffff;		
	}
}

UINT8 _calibPVOB(UINT16 *obR,UINT16 *obGr,UINT16 *obB,UINT16 *obGb)
{
	appAeCb_t *pAeCb; 
	UINT16 expLine[9],calR[9],calB[9],calGr[9],calGb[9],luma,i,j;
	float a, b,rgbData[18];
	UINT8 negative = 0,ret = SUCCESS;
	DBG_PRINT("\n**************************\n"
			   " cali: PV OB \n"
			   "**************************\n"
			   );
	 tbUtil_ViewEnter();
	 
	/*Close OB/AWB/LSC/BP....*/
	#if SPCA5330
	sp5kIqModeSet(SP5K_MODE_STILL_PREVIEW);
	tbUtil_CalibCdspSet(0/*isObOn*/);
	sp5kIqModeSetDone(SP5K_MODE_STILL_PREVIEW); 
	#else
  	sp5kImgModeSet(SP5K_MODE_STILL_PREVIEW);
	tbUtil_CalibCdspSet(0/*isObOn*/);
	sp5kImgModeSetDone(SP5K_MODE_STILL_PREVIEW);
	#endif
	
	appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_TextDraw(10,5,SP5K_GFX_ALIGN_TOP_LEFT,"CALIB PV OB");
	/*-- AE/AWB Off */
	tbUtil_AeAwb_Dark_Off();
	/*-- Large IRIS */
	tbUtil_IrisCtrl(1/*isLarge*/);

	 calPVOB = 1;
	 pAeCb = appAeCbGet();
	 if(calPVOBAgcIdx != 0xffffffff){
		 sp5kPreviewExpAgcSet(EXP_1_32, calPVOBAgcIdx);/*1/30s , user define the agc index*/
	 }
	 else{
		 sp5kPreviewExpAgcSet(EXP_1_32, pAeCb->pviso);/*1/30s , ISO 100*/
	 }
	 sp5kSensorVvalidWait();
	 sp5kSensorVvalidWait();  

	expLine[0] = calPVOBInitExp;
	expLine[8] = calPVOBMinExp;
	
	sensorCmdPortExpLineSend(expLine[0]);
	sp5kSensorVvalidWait();
	sp5kSensorVvalidWait();	 
	luma = tbUtil_PvRawLumaGet();
	//printf("\033[32mluma = %d\033[0m\n",luma);
	expLine[0] = (UINT32)calPVOBTagLuma*(UINT32)expLine[0]/luma;

	//printf("expLine[0] = %d\n",expLine[0]);
	for(i = 1 ; i< 8 ; i++){
		expLine[i] =  expLine[0] - (expLine[0] - expLine[8])*i/8;
		//printf("expLine[%d] = %d\n",i,expLine[i]);
	}
	//printf("expLine[8] = %d\n",expLine[8]);

	for(i = 0 ; i < 9 ; i++){
		sensorCmdPortExpLineSend(expLine[i]);
		sp5kSensorVvalidWait();
		sp5kSensorVvalidWait(); 
		luma = tbUtil_PvRawLumaGet();
		if(luma > 39000){
			printf("PVOB Too bright!\n");
			dbgOsd_Print(4, "Cal PV OB Failed");
			sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 3000);
			ret = FAIL;	
			goto EXIT;
		}
		else if(luma < 10000 && i == 0){
			printf("PVOB Too dark!\n");
			dbgOsd_Print(4, "Cal PV OB Failed");
			sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 3000);
			ret = FAIL;	
			goto EXIT;
		}
		/*printf("\033[32mluma[%d] = %d\033[0m\n",i , luma);*/
		calR[i] = aaaCalibRawCbLuma[0];
		calGr[i] = aaaCalibRawCbLuma[1];
		calB[i] = aaaCalibRawCbLuma[3];
		calGb[i] = aaaCalibRawCbLuma[2];
		//printf("\033[33mPVOB : R(%d):Gr(%d)/B(%d):Gb(%d):Exp(%d)\033[0m\n",calR[i],calGr[i],calB[i],calGb[i],expLine[i]);
	}
	
	for(i = 0 ; i < 4 ; i++){
		for(j = 0 ; j < 9 ; j ++){
			if(calR[j] == 0 || calGr[j] == 0 ||calB[j] == 0 || calGb[j] == 0){
				printf("PVOB ERROR!!AVG = 0\n");
				dbgOsd_Print(4, "Cal PV OB Failed");
				sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 3000);
				ret = FAIL;	
				goto EXIT;
			}
			switch( i ){
				case 0: 
					rgbData[j*2+1] = calR[j];
					/*printf("R = %f\n",rgbData[j*2+1]);*/
					break;  	/*R*/
				case 1:
					rgbData[j*2+1] = calGr[j];
					/*printf("Gr = %f\n",rgbData[j*2+1]);*/					
					break; 	/*Gr*/
				case 2: 
					rgbData[j*2+1] = calB[j];
					/*printf("B = %f\n",rgbData[j*2+1]);*/					
					break;	/*B*/
				case 3: 
					rgbData[j*2+1] = calGb[j];
					/*printf("Gb = %f\n",rgbData[j*2+1]);*/					
					break;	/*Gb*/
			}	
			rgbData[j*2] = expLine[j];
			/*printf("exp = %f\n",rgbData[j*2]);	*/		
		}
		if(_calibPvOBLinearRegression((float *)rgbData, 9, &a, &b) != 1){
			dbgOsd_Print(4, "Cal PV OB Failed");
			sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 3000);
			ret = FAIL;	
			goto EXIT;
		}
		else{
			if(a < 0){
				negative = 1;
			}
			switch( i ){
				case 0: 
					*obR = (a+0.5)>0?(a+0.5):0;
					break;  	/*R*/
				case 1:
					*obGr = (a+0.5)>0?(a+0.5):0;
					break; 	/*Gr*/
				case 2: 
					*obB = (a+0.5)>0?(a+0.5):0;
					break;	/*B*/
				case 3: 
					*obGb = (a+0.5)>0?(a+0.5):0;
					break;	/*Gb*/
			}
			printf("\033[32m LINE%d : Y = %fX+%f\033[0m\n" , i, b ,a);
		}
	}
	if(1 || calPVOBSaveRaw){
		if(tbLogFile_Open("D:\\result.bin") == 1){
			tbLogFile_Print("[platform]\n");
			tbLogFile_Print("num = %d\n",5000);
			tbLogFile_Print("[info]\n");
			tbLogFile_Print("R=%d,%d,%d,%d,%d,%d,%d,%d,%d\n",calR[0],calR[1],calR[2],calR[3],calR[4],calR[5],calR[6],calR[7],calR[8]);
			tbLogFile_Print("Gr=%d,%d,%d,%d,%d,%d,%d,%d,%d\n",calGr[0],calGr[1],calGr[2],calGr[3],calGr[4],calGr[5],calGr[6],calGr[7],calGr[8]);
			tbLogFile_Print("B=%d,%d,%d,%d,%d,%d,%d,%d,%d\n",calB[0],calB[1],calB[2],calB[3],calB[4],calB[5],calB[6],calB[7],calB[8]);
			tbLogFile_Print("Gb=%d,%d,%d,%d,%d,%d,%d,%d,%d\n",calGb[0],calGb[1],calGb[2],calGb[3],calGb[4],calGb[5],calGb[6],calGb[7],calGb[8]);
			tbLogFile_Print("[expLine]\n");
			tbLogFile_Print("ExpLine=%d,%d,%d,%d,%d,%d,%d,%d,%d\n",expLine[0],expLine[1],expLine[2],expLine[3],expLine[4],expLine[5],expLine[6],expLine[7],expLine[8]);
			tbLogFile_Close();
		}
	}
	//printf("\033[32mOB_R=%d,OB_Gr=%d,OB_B=%d,OB_Gb=%d\033[0m\n",*obR,*obGr,*obB,*obGb);
	if(*obR > 5000 || *obGr > 5000 || *obB > 5000 || *obGb > 5000){
		printf("PVOB Calib OB failed!\n");
		dbgOsd_Print(4, "Cal PV OB Failed");
		sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 3000);
		ret = FAIL;	
		goto EXIT;
	}
	else{
		if(negative == 1){
			dbgOsd_Print(2, "WARNING: NEGATIVE");
		}
		else{
			dbgOsd_Print(2, "PVOB Calib OB OK");
		}
		dbgOsd_Print(3, "R:%d", *obR);
		dbgOsd_Print(4, "Gr:%d", *obGr);
		dbgOsd_Print(5, "B:%d", *obB);
		dbgOsd_Print(6, "Gb:%d", *obGb);
		sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 3000);
	}
	/*-- Enable OB */
#if SPCA5330
	sp5kIqModeSet(SP5K_MODE_STILL_PREVIEW);
	sp5kIqCfgSet(SP5K_IQ_CFG_OB_MODE, SP5K_OB_MANUAL); 
	//sp5kIqCfgSet(SP5K_IQ_CFG_OB_MANUAL_VAL, ob);
	tbUtil_CalibCdspSet(1);
	sp5kIqModeSetDone(SP5K_MODE_STILL_PREVIEW); 

#else
       sp5kImgModeSet(SP5K_MODE_STILL_PREVIEW);
	sp5kImgObModeCfgSet(SP5K_OB_MANUAL);
	//sp5kImgObSet(ob);
	tbUtil_CalibCdspSet(1);
	sp5kImgModeSetDone(SP5K_MODE_STILL_PREVIEW);
#endif
	/*-- AE/AWB On */
	sp5kAeModeSet(SP5K_AE_MODE_INFO_ONLY);
#if SP5K_ALGORITHM_AWB
	sp5kAwbModeSet(SP5K_AWB_MODE_INFO_ONLY);
#else
	sp5kAwbModeSet(SP5K_AWB_MODE_AUTO_SET);
#endif        
	EXIT: calPVOB = 0;
	calPVOBSaveRaw = 0;
	calPVOBTagLuma = CAL_PVOB_DEFAULT_TAG_LUMA;
	calPVOBInitExp = CAL_PVOB_DEFAULT_INIT_EXPLINE;
	calPVOBMinExp = CAL_PVOB_DEFAULT_MIN_EXPLINE;
	calPVOBAgcIdx = 0xffffffff;
	if(ret == FAIL){
		*obR = *obGr = *obB = *obGb = 0xffff;
	}
#if SPCA5330
	*obR = *obR>>4;
	*obGr = *obGr>>4;
	*obB = *obB>>4;
	*obGb = *obGb>>4;
	/*printf("\033[32m5330_OPEN!\033[0m\n");*/
#endif
	return ret;
}

/*-------------------------------------------------------------------------
 *  Function Name : appCalibRawCb
 	Description: for panasonic ccd with 9 pixel mix mode.
	 	- the pattern is like this below:
	 	G BRB GGG BRB GGG BRB GGG...
	 	R GGG RBR GGG RBR GGG RBR...
	 	G BRB GGG BRB GGG BRB GGG...
	 	R GGG RBR GGG RBR GGG RBR...
	 	- but now it is like this below:
	 	GG BRB GGG BRB GGG BRB GGG...
	 	BR GGG RBR GGG RBR GGG RBR...
	 	GG BRB GGG BRB GGG BRB GGG...
	 	BR GGG RBR GGG RBR GGG RBR...
	 	- for new sensor
	 	GGG BRB GGG BRB GGG BRB GGG...
	 	RBR GGG RBR GGG RBR GGG RBR...
	 	GGG BRB GGG BRB GGG BRB GGG...
	 	RBR GGG RBR GGG RBR GGG RBR...

	 	To save the callback raw in D:\CALX_XXXX_XXXX.RAW.
 *------------------------------------------------------------------------*/
#define RAW_CALLBACK_SAVE_FILE 0 /**/
#define RAW_CALLBACK_SHOW_SAMPLE_POINT 0 /* mark the rggb as 0xffff, 0xfeff, 0xfdff and 0xfcff */
#define RAW_CAL_MAX_PIXEL_NUM 65535

#if RAW_CALLBACK_SAVE_FILE
UINT32 rawfilenumber=0;
#endif

UINT32 appCalibRawCb(UINT32 addr, UINT32 fmt, void *rawInfo)
{
	sp5kImgWbInfo_t *info=(sp5kImgWbInfo_t*)rawInfo;
	UINT8 bayer[4];
	UINT16 *p,*pRaw=(UINT16 *)addr;
	UINT32 w,h,x,y,x0,y0,dx,dy;
	UINT32 i,cnt,filedOffs;
	UINT8 ridx[]={1, 0, 3, 2};//BAYER_GRBG, BAYER_RGGB, BAYER_BGGR, BAYER_GBRG,
	UINT8 gidx[]={0, 1, 2, 3};

	cnt = 0;
	memset(luma, 0, 4*sizeof(UINT32));
	memset(aaaCalibRawCbLuma, 0, 4*sizeof(UINT32));

	if(calPVOB){
		w = info->width / 10;
		h = info->height / 10;
	}else{
		w = info->width / RAW_AREA_RATIO;
		h = info->height / RAW_AREA_RATIO;
	}
	
 	x0 = ((info->width - w)/2) & 0xfffffffe;
	y0 = ((info->height - h)/2) & 0xfffffffe;
	filedOffs = info->width;

	#if 0
	DBG_PRINT("img=(%d %d) ROI=%d %d %d %d\n", info->width, info->height, x0, y0, w, h);
	DBG_PRINT("row info:%d %d/%d bayer=%d\n", info->rowOfs, info->row0, info->row1, info->bayerOrder);
	#endif

	bayer[0] = ridx[info->bayerOrder]; //R
	bayer[1] = gidx[info->bayerOrder]; //Gr
	bayer[2] = 3-gidx[info->bayerOrder]; //Gb
	bayer[3] = 3-ridx[info->bayerOrder]; //B

	p = pRaw + y0 * info->width + x0;

	#if 0  /*fmt 1 : 10bits , 2 : 12bits , 3 : 14bits*/
	if(fmt == 0) {
		dx = 8;
		dy = 8;
		printf("View Raw...\n");
	}
	else{
		dx = 8;
		dy = 8;
		printf("Capture Raw...\n");
	}
	#else
		dx = dy = 0;
		do{
			dx += 2;
			dy += 2;
		}while( ((w/dx+1)*(h/dy+1)) > RAW_CAL_MAX_PIXEL_NUM);
	#endif
	
	for( y = y0; y < y0 + h; y += dy ) 
	{
		for ( x = 0; x < w; x += dx ) 
		{
			luma[0] += p[x + 0];
			luma[1] += p[x + 1];
			luma[2] += p[x + 0 + filedOffs];
			luma[3] += p[x + 1 + filedOffs];
			cnt ++;
		}
		p += dy * info->width;
	}
	
	//DBG_PRINT("cali: Cnt = %d \n",cnt);

	for (i=0 ; i<4 ; i++){
		//aaaCalibRawCbLuma[i] = luma[bayer[i]] / cnt;
		aaaCalibRawCbLuma[i] = (luma[bayer[i]] + (cnt/2))/ cnt;
	}
	DBG_PRINT("cali: RAW R(%d):Gr(%d)/Gb(%d):B(%d)\n",aaaCalibRawCbLuma[0],aaaCalibRawCbLuma[1],aaaCalibRawCbLuma[2],aaaCalibRawCbLuma[3]);
	printf("cali: RAW R(%d):Gr(%d)/Gb(%d):B(%d)\n",aaaCalibRawCbLuma[0],aaaCalibRawCbLuma[1],aaaCalibRawCbLuma[2],aaaCalibRawCbLuma[3]);

	#if RAW_CALLBACK_SAVE_FILE// To save the raw for debug  
	{
		UINT32 filename[32];

		sprintf(filename, "D:\\CAL%d_%dx%d.RAW",rawfilenumber, info->width, info->height);
		WriteFile(filename, addr, info->width*info->height*2 );
		rawfilenumber++;
	}
	#endif

	return 0;
}

void appGetRawAvgRGB(UINT16 *r, UINT16 *g, UINT16 *b)
{
	*r = aaaCalibRawCbLuma[0];
	*g = (aaaCalibRawCbLuma[1] + aaaCalibRawCbLuma[2])/2;
	*b = aaaCalibRawCbLuma[3];
}

void appGetRawAvgRGGB(UINT16 *r, UINT16 *gr, UINT16 *gb, UINT16 *b)
{
	*r = aaaCalibRawCbLuma[0];
	*gr = aaaCalibRawCbLuma[1];
	*gb = aaaCalibRawCbLuma[2];
	*b = aaaCalibRawCbLuma[3];
}

UINT32 appGetRawMaxChannelAvg(void)
{
	int i;
	UINT32 cluma;

	cluma=aaaCalibRawCbLuma[0];
	for(i=1; i<4;i++)
		cluma = (cluma>aaaCalibRawCbLuma[i]) ? cluma: aaaCalibRawCbLuma[i];

	return cluma;
}

void apptgCmdMake(UINT32 addr, UINT32 data, UINT8 *regdata)
{
#if SP5K_SENSOR_AFE_AD9000 // ad9000=4 byte tg cmd
	regdata[0] = addr & 0xff;
	regdata[1] = (addr>>8) & 0xff;
	regdata[2] = data & 0xff;
	regdata[3] = (data>>8) & 0xff;
#elif SP5K_SENSOR_AFE_AD9920 // 5 byte tg cmd
	regdata[0] = addr & 0xff;
	regdata[1] = ((addr>>8)&0xf) + ((data<<4)&0xf0);
	regdata[2] = (data>>4) & 0xff;
	regdata[3] = (data>>12) & 0xff;
	regdata[4] = (data>>20) & 0xff;
#endif
}


void appCalibDataInit(void){
	UINT8 i;
	UINT16 *p;

	DBG_PRINT("[Calib Data Init...]\n");
	if(calibdata.AfSign == TRUE){
		p = appFocusInfoGet();
		for(i=0;i<ZOOM_ZS_NUM && afCalibdata.AfPosition[i]!=0;i++){
			if (calibdata.afFullScan) {
				/*-- AF Debug Full Scan */
				*p = 100;
				*(p+1) = 600;
				*(p+2)=5;
			} else {
				/*-- AF Normal Flow */
					*p = afCalibdata.AfPosition[i] - (def2Mdata.AfPosition[i] - (*p)) -10;
					*(p+1) = afCalibdata.AfPosition[i] - (def2Mdata.AfPosition[i] - (*(p+1))) + 10;
			}
			DBG_PRINT("Cali: %d %d %d, 2M: %d\n",*p,*(p+1),*(p+2), afCalibdata.AfPosition[i]);
			p+=3;
		}
	}
	if(calibdata.PvDPCSign){
        sp5kResourceFileLoad(SP5K_RES_BP_VIEW, RES_6330_BPVIEW);
	}
	if(calibdata.CapDPCSign ){
	UINT8 ret;
		YPrintf("Cap Bad Pixel Init");		
		#if SPCA5330
		ret = sp5kResourceFileLoad(SP5K_RES_BP_CAP, RES_5330_BPSTILL);
		#else
		ret = sp5kResourceFileLoad(SP5K_RES_BP_CAP, RES_6330_BPSTILL);
		#endif
		if(ret == SUCCESS){
			YPrintf("Cap Bad Pixel Init SUCCESS");		
			sp5kImgCdspBlockEnable(SP5K_CDSP_BP, 1);
		}
	}
	if(calibdata.ShutterSign == FALSE){
		calibdata.ShutterEarly[0] = CALIB_AE_DEFAULT_SHUTTER_EARLY_1;
		calibdata.ShutterEarly[1] = CALIB_AE_DEFAULT_SHUTTER_EARLY_2;
	}
	if(calibdata.ObSign == FALSE){
		for (i=0;i<COUNT_OF(calibdata.Ob); i++) {
		    calibdata.Ob[i] = CALIB_AE_DEFAULT_OB;
	    }
	}
	sp5kSnapShutterDelaySet(SP5K_SNAP_SHUTTER_CLOSE_EXP_END, 0, calibdata.ShutterSign && calibdata.ShutterEarly[0] ? calibdata.ShutterEarly[0] : CALIB_AE_DEFAULT_SHUTTER_EARLY_1); /* unit : 10us */
	sp5kSnapShutterDelaySet(SP5K_SNAP_SHUTTER_OPEN_READOUT_END, 0, 8000); /* uint : 10us */
	//calibdata.WbSign ? calibdata.WbRgain : CALIB_WB_DEFAULT_RGAIN;
	//calibdata.WbSign ? calibdata.WbBgain : CALIB_WB_DEFAULT_BGAIN;
	if(calibdata.WbSign)
	{
    /*s Modify by Aries 09/08/18*/
		sp5kAwbParamSet(SP5K_AWB_CALIBRATION_RGAIN,calibdata.WbRgain<<4);
		sp5kAwbParamSet(SP5K_AWB_CALIBRATION_BGAIN,calibdata.WbBgain<<4);
    /*e Modify by Aries 09/08/18*/
	}
}

void appResetFocusInfo(void){
	UINT16 *p;
	p = appFocusInfoGet();
	UINT16 temp[ZOOM_ZS_NUM+1][3] = {
		{FOCUS_0_S + FOCUS_POSITION_OFFSET, FOCUS_0_E + FOCUS_POSITION_OFFSET, FOCUS_0_STEP},
		{FOCUS_1_S + FOCUS_POSITION_OFFSET, FOCUS_1_E + FOCUS_POSITION_OFFSET, FOCUS_1_STEP},
		{FOCUS_2_S + FOCUS_POSITION_OFFSET, FOCUS_2_E + FOCUS_POSITION_OFFSET, FOCUS_2_STEP},
		{FOCUS_3_S + FOCUS_POSITION_OFFSET, FOCUS_3_E + FOCUS_POSITION_OFFSET, FOCUS_3_STEP},
		{FOCUS_4_S + FOCUS_POSITION_OFFSET, FOCUS_4_E + FOCUS_POSITION_OFFSET, FOCUS_4_STEP},
		{FOCUS_5_S + FOCUS_POSITION_OFFSET, FOCUS_5_E + FOCUS_POSITION_OFFSET, FOCUS_5_STEP},
		{FOCUS_6_S + FOCUS_POSITION_OFFSET, FOCUS_6_E + FOCUS_POSITION_OFFSET, FOCUS_6_STEP},
		{FOCUS_7_S + FOCUS_POSITION_OFFSET, FOCUS_7_E + FOCUS_POSITION_OFFSET, FOCUS_7_STEP}
	};
	memcpy(p, &temp[0][0], sizeof(temp));

}
void appCalibAfInit(
	void)
{
	UINT8 i;
	UINT16 *p;
	sp5kAfWinSize_t  win;

	CLEAR_OSD_SRC;

	CalibMode = CALIB_MODE_AF;
	afCalibdata.AfPosition[0]=afCalibdata.AfPosition[1]=afCalibdata.AfPosition[2]=0;
	afCalibdata.AfPosition[3]=afCalibdata.AfPosition[4]=afCalibdata.AfPosition[5]=0;
	afCalibdata.AfPosition[6]=0;

	win.hwincnt = 3;
	win.vwincnt = 3;
	win.hoffset = 358;
	win.voffset = 358;

	win.hsize = 102;
	win.vsize = 102;
	win.hintvl = 0;
	win.vintvl = 0;
	sp5kAfWinSizeSet(&win);

	sp5kModeSet(SP5K_MODE_STILL_PREVIEW);
	sp5kTimeDelay(SP5K_TIME_DELAY_1MS,	1000);

	CaliZlevel =  0;


	appResetFocusInfo();
	p = appFocusInfoGet();
	for(i=0;i<ZOOM_ZS_NUM+1;i++){
		DBG_PRINT("Default: %d %d %d\n",*p,*(p+1),*(p+2));
		p+=3;
	}
	p = appFocusInfoGet();
	memcpy(CaliAfScanInfo,(UINT8 *)p,sizeof(CaliAfScanInfo));
	for(i=0;i<ZOOM_ZS_NUM+1;i++){

		#if 0
		*p-=20;*(p+1)-=20;
		*(p+2)=1;
		#else
		*p=150;*(p+1)=450;
		*(p+2)=5;
		#endif
		DBG_PRINT("Cali: %d %d %d\n",*p,*(p+1),*(p+2));
		p+=3;
	}

	calibLensZoomSegGo(CaliZlevel);
	appAfInit();
	AfStatusSet(AF_BUSY);
	sp5kAfModeSet(SP5K_AF_MODE_ON);
	sp5kAeModeSet(SP5K_AE_MODE_INFO_ONLY);
#if SP5K_ALGORITHM_AWB
	sp5kAwbModeSet(SP5K_AWB_MODE_INFO_ONLY);
#else
	sp5kAwbModeSet(SP5K_AWB_MODE_AUTO_SET);
#endif   
}

void appCalibAf(
	UINT32 msg,
	UINT32 param)
{
	//Modified by Brian 08/08/08
	static UINT8 AE_READY_COUNTER=0;

	if(SP5K_MSG_AF_READY == msg && AE_READY_COUNTER > 5){
		static UINT32 inited=0;
		if (inited==0) {
			inited = 1;
		sp5kAeModeSet(SP5K_AE_MODE_OFF);
		sp5kAwbModeSet(SP5K_AWB_MODE_OFF);
		aaa_AutoFocus();
	}
		if(CaliZlevel < ZOOM_ZS_NUM){
			appFocusScan(param);
			CaliAfStatus = AfStatusGet();
			if (CaliAfStatus == AF_BUSY)
				return;
			afCalibdata.AfPosition[CaliZlevel] = appFocusMaxPosGet();
			DBG_PRINT("CaliZlevel = %d MaxPos = %d\n",CaliZlevel,afCalibdata.AfPosition[CaliZlevel]);
			CaliZlevel++;
			if(CaliZlevel < ZOOM_ZS_NUM){
				calibLensZoomSegGo(CaliZlevel);
				sp5kTimeDelay(SP5K_TIME_DELAY_1MS,	500);
				appAfInit();
				AfStatusSet(AF_BUSY);
			}
			sp5kAfModeSet(SP5K_AF_MODE_ON);
		}else{
			AE_READY_COUNTER = 0;
			calibdata.AfSign = 1;
			CalibMode = CALIB_MODE_IDLE;
			sprintf(buf,"%d %d %d %d %d %d %d\n",afCalibdata.AfPosition[0],afCalibdata.AfPosition[1],afCalibdata.AfPosition[2],
						afCalibdata.AfPosition[3],afCalibdata.AfPosition[4],afCalibdata.AfPosition[5],afCalibdata.AfPosition[6]);
			appOsdLib_TextDraw(0,32,SP5K_GFX_ALIGN_TOP_LEFT,buf);
			memcpy((UINT8 *)appFocusInfoGet(),CaliAfScanInfo,sizeof(CaliAfScanInfo));
			sp5kAfModeSet(SP5K_AF_MODE_OFF);
			calibLensZoomSegGo(0);
			sp5kTimeDelay(SP5K_TIME_DELAY_1MS,2000);
			appStateChange(APP_STATE_CALIBRATION,STATE_PARAM_NORMAL_INIT);


			DBG_PRINT("\n*** FOCUS Calib DONE !!***************\n");
			DBG_PRINT("%d %d %d %d %d %d %d\n\n",
						afCalibdata.AfPosition[0],
						afCalibdata.AfPosition[1],
						afCalibdata.AfPosition[2],
						afCalibdata.AfPosition[3],
						afCalibdata.AfPosition[4],
						afCalibdata.AfPosition[5],
						afCalibdata.AfPosition[6]);
		}
	}
	else if(SP5K_MSG_AE_READY == msg){
		SINT32 ret;
		ret = appAePreviewAdjust(param);
		if(ret == SUCCESS)
		{
			AE_READY_COUNTER++;
		}else{
			AE_READY_COUNTER = 0;
			return;
		}
	}
}

void appCalibAwb()
{
	DBG_PRINT("Calib Awb\n");
	appStateChange(APP_STATE_CALIB_AWB, STATE_PARAM_NORMAL_INIT);
}

void appCalibReport(BOOL isWrFile)
{
	char szFname[]="C:\\CALIBRPT.TXT";
	appCalibData_t *pcalib;

	pcalib = appCalibDataGet();

	if (isWrFile) {
		if (IS_CARD_EXIST) szFname[0]='D';

		if (!tbLogFile_Open(szFname)) {
			CLEAR_OSD_SRC;
			dbgOsd_Print(4, "File ERR");
			APP_SLEEP_MS(2000);
			return;
		}
	}

	tbLogFile_PrintEx(isWrFile, "***** Calibration Report *****************\n\n");
	tbLogFile_PrintEx(isWrFile, "[%1d] OB: %d \n", !!pcalib->ObSign, pcalib->Ob);
	tbLogFile_PrintEx(isWrFile, "[%1d] ISO50 OB: %d \n", !!pcalib->IsoObSign, pcalib->isoOb[0]);
	tbLogFile_PrintEx(isWrFile, "[%1d] ISO100 OB: %d \n", !!pcalib->IsoObSign, pcalib->isoOb[1]);
	tbLogFile_PrintEx(isWrFile, "[%1d] ISO200 OB: %d \n", !!pcalib->IsoObSign, pcalib->isoOb[2]);
	tbLogFile_PrintEx(isWrFile, "[%1d] ISO400 OB: %d \n", !!pcalib->IsoObSign, pcalib->isoOb[3]);
	tbLogFile_PrintEx(isWrFile, "[%1d] ISO800 OB: %d \n", !!pcalib->IsoObSign, pcalib->isoOb[4]);
	tbLogFile_PrintEx(isWrFile, "[%1d] ISO1600 OB: %d \n", !!pcalib->IsoObSign, pcalib->isoOb[5]);
	tbLogFile_PrintEx(isWrFile, "[%1d] MShut[0]: %d\n", !!pcalib->ShutterSign, pcalib->ShutterEarly[0]);
	tbLogFile_PrintEx(isWrFile, "[%1d] MShut[1]: %d\n", !!pcalib->ShutterSign, pcalib->ShutterEarly[1]);
	tbLogFile_PrintEx(isWrFile, "[%1d] ISO: %d, %d\n", !!pcalib->IsoSign, pcalib->PvIso, pcalib->CapIso);
	tbLogFile_PrintEx(isWrFile, "[%1d] miniG: %d \n", !!pcalib->capSatGainSign, pcalib->capSatGain);
	tbLogFile_PrintEx(isWrFile, "[%1d] IrisDiff: %d \n", !!pcalib->irisDiff, pcalib->irisDiff);
	tbLogFile_PrintEx(isWrFile, "[%1d] VIEW BP: %d \n", !!pcalib->PvDPCSign, pcalib->PvDPCSign);
	tbLogFile_PrintEx(isWrFile, "[%1d] STILL BP: %d \n", !!pcalib->CapDPCSign, pcalib->CapDPCSign);


	if (isWrFile) {
		tbLogFile_Close();
	}
}

/*-------------------------------------------------------------------------
 * Function : appCalibOb
 *------------------------------------------------------------------------*/
void appCalibOb()
{

	SINT32 ob;

	UINT32 ret = SUCCESS;

        UINT32 i, avg_ob = 0;
        
        memset(aaaCalibAvgLuma,0,sizeof(aaaCalibAvgLuma));
        
        for (i = 0; i < 5; i++) {
	    ret = _calibOb(CALIB_EXP_1_32, OB_CALIB_ISO,&ob);
            avg_ob += ob;
        }

        ob = avg_ob / 5;
        printf("Avg Ob = %d\n", ob);

	//ret = _calibOb(CALIB_EXP_1_32, OB_CALIB_ISO,&ob);
	if(ret == SUCCESS){

		calibdata.ObSign = 1;
		calibdata.Ob[0] = ob;
        calibdata.Ob[1] = aaaCalibAvgLuma[0]>>4;
        calibdata.Ob[2] = aaaCalibAvgLuma[1]>>4;
        calibdata.Ob[3] = aaaCalibAvgLuma[2]>>4;
        calibdata.Ob[4] = aaaCalibAvgLuma[3]>>4;

        for (i=0;i<COUNT_OF(calibdata.Ob); i++) 
        {        
		  printf("<lester>calibdata.Ob[%d] = %d\n",i,calibdata.Ob[i]);
	    }
        
        appCalibDataSave();
	}
	tbUtil_QuickSweepHostMessage(1000);

	//calib OB @ different ISO
	//_calibObAdv(CALIB_EXP_1_32);

	appStateChange(APP_STATE_CALIBRATION,STATE_PARAM_NORMAL_INIT);

}
 /*-------------------------------------------------------------------------
 * Function : appCalibPVOB
 *------------------------------------------------------------------------*/
 void appCalibPVOB(void)
{
	UINT16 obR,obGr,obB,obGb;
	UINT32 starTime,endTime;
	UINT8 ret;
	starTime = dbgTimeGet();
	_calibPVOBCfg(CAL_PV_OB_SAVE_INFO, 1);
	ret = _calibPVOB(&obR,&obGr,&obB,&obGb);
	if (ret == SUCCESS){
		RPrintf("PVOB OK!");
		calibdata.pvOBSign = 1;
		calibdata.pvOB[0] = obR;
		calibdata.pvOB[1] = obGr; 
		calibdata.pvOB[2] = obB;
		calibdata.pvOB[3] = obGb;
        appCalibDataSave();
	}
	printf("\033[32mobR=%d,obGr=%d,obB=%d,obGb=%d\033[0m\n",obR,obGr,obB,obGb);
	endTime = dbgTimeGet();
	printf("\033[32mCAL_PVOB_TIME = %dms\033[0m\n",(endTime-starTime)/1000);
	appStateChange(APP_STATE_CALIBRATION,STATE_PARAM_NORMAL_INIT);
}

  /*-------------------------------------------------------------------------
  * Function : appCalibLSC
  *------------------------------------------------------------------------*/
void appCalibLSC(void)
{
	UINT32 ret;
	UINT16 luma, extFlag;
	SINT32 pvExpIdx,pvAgcIdx,capExpIdx,capAgcIdx;
	appAeCb_t  *pAeCb; 

	pAeCb = appAeCbGet();
	pvAgcIdx = pAeCb->pviso;
	pvAgcIdx = (pvAgcIdx > 16)?16:pvAgcIdx;
	#if 0
	capAgcIdx = pAeCb->capiso;
	#else
	capAgcIdx = pAeCb->capiso - pAeCb->pviso + pvAgcIdx;
	#endif
	pvExpIdx = EXP_1_32 - 16;    	/*start from exp:1/30 agc:iso100*/
	DBG_PRINT("\n**************************\n"
			  " cali: LSC \n"
			  "**************************\n"
			  );
	tbUtil_ViewEnter();
	/*Close AWB/LSC/BP.... Enable OB*/
#if SPCA5330
	sp5kIqModeSet(SP5K_MODE_STILL_SNAP);
	tbUtil_CalibCdspSet(1);
	sp5kIqModeSetDone(SP5K_MODE_STILL_SNAP); 
#else
  	sp5kImgModeSet(SP5K_MODE_STILL_SNAP);
	tbUtil_CalibCdspSet(1);
	sp5kImgModeSetDone(SP5K_MODE_STILL_SNAP);
#endif
	/*-- AE/AWB Off */
	tbUtil_AeAwb_Dark_Off();
	/*-- Large IRIS */
	tbUtil_IrisCtrl(1/*isLarge*/);

	appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_TextDraw(10,5,SP5K_GFX_ALIGN_TOP_LEFT,"CALIB LSC");
	appOsdLib_TextDraw(120,100,SP5K_GFX_ALIGN_TOP_LEFT,"Waiting....");
	
	luma = 50000;
	extFlag = 0;
	while(luma > 15000 && pvExpIdx < EXP_1_2048 && extFlag == 0)
	{
		if(luma > 30000){
			pvExpIdx += 16;
		}
		else{
			extFlag = 1;
			pvExpIdx += ((float)(luma -(15000/2))/15000)*16;
		}
		sp5kPreviewExpAgcSet(pvExpIdx, pvAgcIdx);
		sp5kSensorVvalidWait();
		sp5kSensorVvalidWait();
		luma = tbUtil_PvRawLumaGet();
		printf("pvExpIdx = %d\n",pvExpIdx);
	}
	if(pvExpIdx > EXP_1_2048 -1 || luma < 9000)
	{
		CLEAR_OSD_SRC;
		appOsdLib_TextDraw(60,100,SP5K_GFX_ALIGN_TOP_LEFT,"LSC CAL ERROR-1!");
		sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 2000);
		printf("LSC cal ERROR:pvExpIdx beyond!\n");
		goto EXIT;
	}
	capExpIdx = pvExpIdx;

	printf("capExpIdx = %d , capAgcIdx = %d\n",capExpIdx,capAgcIdx);
	if(capAgcIdx >= cap_svidx_to_agcidx(STD_ISO_800))
	{
		CLEAR_OSD_SRC;
		appOsdLib_TextDraw(60,100,SP5K_GFX_ALIGN_TOP_LEFT,"LSC CAL ERROR-2!");
		sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 2000);	
		printf("LSC cal ERROR:capAgcIdx beyond!\n");
		goto EXIT;
	}
	printf("PV luma = %d\n",luma);
	sp5kStillCapExpAgcSet(pvExpIdx,capAgcIdx);
	/*luma = tbUtil_CapRawLumaGet();
	printf("CAP luma = %d\n",luma);*/

	ret = _calibLSC(3, 100);
	CLEAR_OSD_SRC;
	if(ret == SUCCESS){
		appOsdLib_TextDraw(60,100,SP5K_GFX_ALIGN_TOP_LEFT,"LSC CAL OK!");
	}
	else{
		appOsdLib_TextDraw(60,100,SP5K_GFX_ALIGN_TOP_LEFT,"LSC CAL ERROR-3!");

	}
	sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 2000);	

	/*-- AE/AWB On */
	sp5kAeModeSet(SP5K_AE_MODE_INFO_ONLY);
#if SP5K_ALGORITHM_AWB
	sp5kAwbModeSet(SP5K_AWB_MODE_INFO_ONLY);
#else
	sp5kAwbModeSet(SP5K_AWB_MODE_AUTO_SET);
#endif
	EXIT:appStateChange(APP_STATE_CALIBRATION,STATE_PARAM_NORMAL_INIT);
}

 /*-------------------------------------------------------------------------
 * Function : appCalibIsoOb
 *------------------------------------------------------------------------*/
void appCalibIsoOb()
{

	//calib OB @ different ISO
	_calibObAdv(CALIB_EXP_1_32);
	tbUtil_QuickSweepHostMessage(3000);

	appStateChange(APP_STATE_CALIBRATION,STATE_PARAM_NORMAL_INIT);

}
/*-------------------------------------------------------------------------
 * Function : appCalibIso
 *------------------------------------------------------------------------*/
void appCalibIso(void)
{
	SINT32 agcidx;
	SINT32 expidx;
	UINT32 luma, loBound,hiBound;
	BOOL isSuccess;
	UINT16 aeTarget;
	UINT32 i, sumLuma=0;

	UINT32 ret = TRUE;

	DBG_PRINT("\n**************************\n"
			  " cali: ISO \n"
			  "**************************\n"
			  );

	/*-- Set View Config */
	tbUtil_ViewEnter();	/*s Add by Aries 10/03/03*/
	#if SPCA5310
  sp5kImgModeSet(SP5K_MODE_STILL_PREVIEW);
	#endif
	/*e Add by Aries 10/03/03*/
	tbUtil_CalibCdspSet(1/*isObOn*/);
	/*s Add by Aries 10/03/03*/
	#if SPCA5310
	sp5kImgModeSetDone(SP5K_MODE_STILL_PREVIEW);
	#endif
	/*e Add by Aries 10/03/03*/
	dbgOsd_Print(1,"CALIB ISO LV11");

	/*-- AE/AWB Off */
	tbUtil_AeAwb_Dark_Off();
	tbUtil_AeControl(1);

	/*-- Large IRIS */
	tbUtil_IrisCtrl(1/*isLarge*/);

	/*-- OB Subtraction */
	/*s Add by Aries 10/03/03*/
	#if SPCA5310
  sp5kImgModeSet(SP5K_MODE_STILL_PREVIEW);
	#endif
	/*e Add by Aries 10/03/03*/
	tbUtil_ObApply();

	/*s Add by Aries 10/03/03*/
	#if SPCA5310
	sp5kImgModeSetDone(SP5K_MODE_STILL_PREVIEW);
	#endif
	/*e Add by Aries 10/03/03*/

	/*-- Reset Calib flag */
	calibdata.IsoSign = 0;

	/*******************
	 * VIEW ISO Calib
	 *******************/
	/*-- target rawLuma range */
	aeTarget = appAeTargetGet();
	appAeTargetSet(aeTarget);
	printf("View ISO approaching ae target %d \n", aeTarget);
 	expidx = CALIB_ISO_PV_EXP + CALIB_ISO_PV_EXP_OFS;
	agcidx = tbUtil_AeAgcConverge(expidx);
	if (agcidx==-1) {
		printf("\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
		printf("!!!! calibIso: Failed View ISO @ aeTGT(%d) !!!! \n", aeTarget);
		printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n");
		ret = FALSE;
		goto _exit;
	}

	printf("AE_TARGET(%d) converged @ exp=%d, agc=%d ...\n",
			aeTarget, expidx, agcidx);

	calibdata.PvIso = agcidx - CALIB_ISO_PV_EXP_OFS;
	/*calibdata.PvIso =1;*/
	printf("\n ******* cali: PvIso = %d, expidx(%d) -> (%d) *********\n",calibdata.PvIso, expidx, expidx-CALIB_ISO_PV_EXP_OFS);

	tbUtil_AeControl(0); /* turn AE off */
	/*-- Get averaged RawLuma @ converged exp/agc */
	printf("... averaging converged rawLuma, please wait !!\n");
	for (i=0; i<5; i++) {
		sp5kPreviewExpAgcSet(expidx, agcidx);
		sp5kSensorVvalidWait();
		sp5kSensorVvalidWait();
		sumLuma += tbUtil_PvRawLumaGet();
		APP_SLEEP_MS(1000); /* Eric Lee 100526 */
	}
	luma = (UINT16)(sumLuma/i);
	printf("AE_TARGET(%d), resulted luma ... %d \n", aeTarget, luma);

	loBound = (luma * 97 ) / 100;
	hiBound = (luma * 103 ) / 100;

		/*-- *64: TARGET is 10-bit. capture raw is 16-bit, so shift pow(2, 6)=64
		 * loBound= 98% of TARGET, hiBound= 102% of TARGET.
		 */
	printf("\ncalibIso: target(%d) loBound = %d hiBound = %d \n", aeTarget, loBound,hiBound);

	/*******************
	 * SNAP ISO Calib
	 *******************/
	tbUtil_AeControl(0); /* turn AE off */
	/*s Add by Aries 10/03/03*/
	#if SPCA5310
  sp5kImgModeSet(SP5K_MODE_STILL_SNAP);
	tbUtil_CalibCdspSet(1/*isObOn*/);
	tbUtil_ObApply();
	sp5kImgModeSetDone(SP5K_MODE_STILL_SNAP);
	#endif
	/*e Add by Aries 10/03/03*/
	expidx=CALIB_ISO_CAP_EXP + CALIB_ISO_CAP_EXP_OFS;

	isSuccess=TRUE;
	agcidx = tbUtil_AgcApproachTest(0/*isPvMode*/, loBound, hiBound, expidx);

	#if 1 /* constraint minimum saturation gain !? */
	if (agcidx<appAeMinCapGainGet()) {
		isSuccess = FALSE;
		ret = FALSE;
		printf("calibIso: ERROR, agcidx(%d) < minCapGain(%d), exp offset(%d)  !!! \n", agcidx, appAeMinCapGainGet(), CALIB_ISO_CAP_EXP_OFS);
	}
	#endif

	if (agcidx==0) {
		isSuccess = FALSE;
		ret = FALSE;
	}

	if (isSuccess) {
		calibdata.CapIso = agcidx-CALIB_ISO_CAP_EXP_OFS;
		/*calibdata.CapIso=15;*/
		DBG_PRINT("\n ******* calibIso: CapIso=%d, \n"
				  "(%d, %d) -> (%d, %d), exp offset = %d *********\n",
					calibdata.CapIso,
					expidx, agcidx, CALIB_EXP_1_128, calibdata.CapIso, CALIB_ISO_CAP_EXP_OFS);
	} else {
		printf("\n!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
		printf("!!!! cali: SNAP Iso FAIL \n");
		printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n");
		ret = FALSE;
		goto _exit;
	}

	calibdata.sensDiff = calibdata.CapIso - calibdata.PvIso;
	printf("\n**** Calib ISO ***********************\n");
	printf("cali: AE_TARGET(%d) \nrawLuma lBound = %d uBound = %d \n", aeTarget, loBound,hiBound);
	printf("cali: PvIso(%d) @ exp=%d \n", calibdata.PvIso, CALIB_EXP_1_128);
	printf("cali: CapIso(%d)@ exp=%d \n", calibdata.CapIso,CALIB_EXP_1_128);
	printf("cali: sensDiff=%d \n\n", calibdata.sensDiff);

	dbgOsd_Print(2, "PvIso  %d", calibdata.PvIso);
	dbgOsd_Print(3, "CapIso %d", calibdata.CapIso);
	dbgOsd_Print(4, "SensDiff %d", calibdata.sensDiff);


	#if 0
	sp5kStillCapExpAgcSet(expidx,calibdata.CapIso);
	luma = tbUtil_CapRawLumaGet();
	sp5kSensorVvalidWait();
	sp5kSensorVvalidWait();
	printf("Apply Capture Gain resulted luma ... %d \n",luma);
	#endif


	sp5kAeModeSet(SP5K_AE_MODE_INFO_ONLY);
	sp5kAwbModeSet(SP5K_AWB_MODE_INFO_ONLY);
	calibdata.IsoSign = 1;

_exit:
	if(ret == FALSE){
		dbgOsd_Print(5, "Cal ISO Failed");
	}
	else{
		dbgOsd_Print(5, "Cal ISO OK");
	}
	tbUtil_QuickSweepHostMessage(3000);
	//sp5kTimeDelay(SP5K_TIME_DELAY_1MS,2000);
	appStateChange(APP_STATE_CALIBRATION,STATE_PARAM_NORMAL_INIT);
}

/*-------------------------------------------------------------------------
 * Function : appCalibMShutter
 *------------------------------------------------------------------------*/
void appCalibMShutter()
{
	UINT32 agcidx;
	UINT32 leakDelay;
	UINT32 isSuccess=TRUE;
	/*UINT32 lagT0,lagT1;*/ //haiyan.he added

	agcidx = CALIB_MSHUT_ISO;
	leakDelay = CALIB_MSHUT_LEAK_DELAY;

	if (calibdata.CapIso!=0)
	{
		agcidx = calibdata.CapIso;
	}

/*_fail_exit:*/
	if (isSuccess==FALSE) 
	{
		calibdata.ShutterSign = 0;
		calibdata.ShutterEarly[0] = CALIB_AE_DEFAULT_SHUTTER_EARLY_1;
		calibdata.ShutterEarly[1] = CALIB_AE_DEFAULT_SHUTTER_EARLY_2;
		printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
		printf("MShutter calib failed !!\n");
		printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
		dbgOsd_Print(4, "Calib NG");
		tbUtil_QuickSweepHostMessage(3000); //APP_SLEEP_MS(3000);
	}
	appStateChange(APP_STATE_CALIBRATION,STATE_PARAM_NORMAL_INIT);
}

void appCalibViewBp()
{
#if 1
	UINT32 thr[4]={ CALIB_BP_VIEW_THR_R, CALIB_BP_VIEW_THR_Gr, CALIB_BP_VIEW_THR_Gb, CALIB_BP_VIEW_THR_B };
	UINT32  ret = SUCCESS;
	UINT32 expidx,agcidx;

	extern UINT32 xdata[];
	if(xdata[0]) {
		 thr[0] =  thr[1] =  thr[2] =  thr[3] = xdata[0];
		 printf("BP THR :%d\n", xdata[0]);
		 xdata[0] = 0;
	}

	DBG_PRINT("\n**************************\n"
			  " cali: VIEW BP (Thr : %d \n"
			  "**************************\n",thr[0] );

	/*-- Set View Config */
	tbUtil_ViewEnter();
	tbUtil_CalibCdspSet(1/*isObOn*/);
	dbgOsd_Print(1,"CALIB VIEW BP LV10");

	/*-- AE/AWB Off */
	tbUtil_AeAwb_Dark_Off();
	tbUtil_AeControl(1);

	/*-- Large IRIS */
	tbUtil_IrisCtrl(1/*isLarge*/);

	/*-- OB Subtraction */
	tbUtil_ObApply();

	expidx = CALIB_EXP_1_128;
	agcidx = pv_svidx_to_agcidx(CALIB_ISO_200);
	sp5kPreviewExpAgcSet(expidx ,agcidx);

	printf("expidx : %d, agcidx = %d\n",expidx ,agcidx);

	sp5kSensorVvalidWait();
	ret = sp5kCalibBpCfgSet(SP5K_CALIB_BP_THR, (UINT32)thr);
	if (ret != SUCCESS) {
		printf("CAP BP calib error (A)\n");
		goto _exit;
	}
	ret = sp5kCalibBpCfgSet(SP5K_CALIB_BP_MAX, CALIB_BP_VIEW_MAX);
	if (ret != SUCCESS) {
		printf("CAP BP calib error (B)\n");
		goto _exit;
	}
	ret = sp5kCalibBpCfgSet(SP5K_CALIB_BP_FILENAME, (UINT32)RES_6330_BPVIEW);
	if (ret != SUCCESS) {
		printf("VIEW BP calib error (C)\n");
		goto _exit;
	}
	ret = sp5kCalibrationCfgSet(SP5K_CALIBRATION_VIEW_BP, 1);
	if (ret != SUCCESS) {
		printf("VIEW BP calib error (D)\n");
		goto _exit;
	}

	sp5kSensorVvalidWait();
	ret = sp5kCalibrationCfgSet(SP5K_CALIBRATION_VIEW_BP, 9);
	if (ret != SUCCESS) {
		printf("VIEW BP calib error (E)\n");
		goto _exit;
	}
	ret = sp5kCalibrationCfgSet(SP5K_CALIBRATION_OFF, SP5K_CALIBRATION_VIEW_BP);
	if (ret != SUCCESS) {
		printf("VIEW BP calib error (F)\n");
		goto _exit;
	}

_exit:
	if(ret != SUCCESS) {
		calibdata.PvDPCSign = 0;
		printf("\n!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
		printf("!!!! cali: VIEW BP FAIL \n");
		printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n");
		dbgOsd_Print(2, "cali: VIEW BP FAIL!");
	}
	else {
		calibdata.PvDPCSign = 1;
		dbgOsd_Print(2, "cali: VIEW BP OK!");

	}
	tbUtil_QuickSweepHostMessage(3000);
	appStateChange(APP_STATE_CALIBRATION,STATE_PARAM_NORMAL_INIT);
#else
	DBG_PRINT("Calib View Bp\n");
	CLEAR_OSD_SRC;
	appStateChange(APP_STATE_STILL_PREVIEW,STATE_PARAM_NORMAL_INIT);
	sp5kTimeDelay(SP5K_TIME_DELAY_1MS,	1000);
	CLEAR_OSD_SRC;
	appOsdLib_TextDraw(0,0,SP5K_GFX_ALIGN_TOP_LEFT,"[CALIB View Bp LV10]");

	tbUtil_CalibCdspSet(1/* isObOn */);

	calibLensZoomSegGo(6);
	sp5kAeModeSet(SP5K_AE_MODE_OFF);
	sp5kAwbModeSet(SP5K_AWB_MODE_OFF);
	appIrisLargeSet();

	sp5kPreviewExpAgcSet(CALIB_EXP_1_128 ,CALIB_ISO_200);
	sp5kSensorVvalidWait();
	sp5kCalibBpCfgSet(SP5K_CALIB_BP_FILENAME, (UINT32)RES_6330_BPVIEW);
	sp5kCalibrationCfgSet(SP5K_CALIBRATION_VIEW_BP, 1);
	appHostMsgWait(SP5K_MSG_CALI_COMPLETE, NULL, 6000);
	lensMShutPowerOn();
	sp5kTimeDelay(SP5K_TIME_DELAY_1MS,20);
	appMShutClose();
	sp5kSensorVvalidWait();
	sp5kCalibrationCfgSet(SP5K_CALIBRATION_VIEW_BP, 9);
	appHostMsgWait(SP5K_MSG_CALI_COMPLETE, NULL, 6000);
	sp5kCalibrationCfgSet(SP5K_CALIBRATION_OFF, SP5K_CALIBRATION_VIEW_BP);
	appHostMsgWaitExact(SP5K_MSG_MODE_READY, SP5K_MODE_STILL_PREVIEW, 2000);

	sp5kAeModeSet(SP5K_AE_MODE_INFO_ONLY);
	sp5kSensorVvalidWait();
	sp5kAwbModeSet(SP5K_AWB_MODE_INFO_ONLY);
	lensMShutPowerOn();
	sp5kTimeDelay(SP5K_TIME_DELAY_1MS,20);
	appMShutOpen();
	sp5kSensorVvalidWait();
	lensMShutPowerOff();
	calibdata.PvDPCSign = 1;
	calibLensZoomSegGo(0);
	sp5kTimeDelay(SP5K_TIME_DELAY_1MS,	1000);
	appStateChange(APP_STATE_CALIBRATION,STATE_PARAM_NORMAL_INIT);
#endif
}

void appCalibCaptureBp()
{
	UINT32 thr[4]={ CALIB_BP_CAP_THR_R, CALIB_BP_CAP_THR_Gr, CALIB_BP_CAP_THR_Gb, CALIB_BP_CAP_THR_B };

#if 1
	UINT32  ret = SUCCESS;
	UINT32 expidx,agcidx;

	extern UINT32 xdata[];
	if(xdata[0]) {
		 thr[0] =  thr[1] =  thr[2] =  thr[3] = xdata[0];

		 xdata[0] = 0;
	}

	DBG_PRINT("\n**************************\n"
			  " cali: Cap BP (Thr : %d \n"
			  "**************************\n",thr[0] );

	/*-- Set View Config */
	tbUtil_ViewEnter();
	tbUtil_CalibCdspSet(1/*isObOn*/);
	
	/*-- AE/AWB Off */
	tbUtil_AeAwb_Dark_Off();
	tbUtil_AeControl(1);

	/*-- Large IRIS */
	tbUtil_IrisCtrl(1/*isLarge*/);

	/*-- OB Subtraction */
	tbUtil_ObApply();
	
	/************White image************/
	expidx = CALIB_EXP_1_128;
	agcidx = cap_svidx_to_agcidx(CALIB_ISO_200);
	sp5kStillCapExpAgcSet(expidx ,agcidx);
	printf("expidx : %d, agcidx = %d\n",expidx ,agcidx);
	
	/*Show OSD , and check OK is pressed.*/
	dbgOsd_Print(1,"White Image");
	dbgOsd_Print(2,"Please use LV10 ,and Press OK");
	while(IS_KEY_OK_PRESSED == 0){};
	RPrintf("Press OK!");
	dbgOsd_Print(2, "  Waiting.......  ");	
	
	/*Threshold init*/
	ret = sp5kCalibBpCfgSet(SP5K_CALIB_BP_THR, (UINT32)thr);
	if (ret != SUCCESS) {
		RPrintf("CAP BP calib error (A)\n");
		goto _exit;
	}
	ret = sp5kCalibBpCfgSet(SP5K_CALIB_BP_MAX, CALIB_BP_CAP_MAX);
	if (ret != SUCCESS) {
		RPrintf("CAP BP calib error (B)\n");
		goto _exit;
	}
	#if SPCA5330
	ret = sp5kCalibBpCfgSet(SP5K_CALIB_BP_FILENAME, (UINT32)RES_5330_BPSTILL);
	#else
	ret = sp5kCalibBpCfgSet(SP5K_CALIB_BP_FILENAME, (UINT32)RES_6330_BPSTILL);
	#endif
	if (ret != SUCCESS) {
		RPrintf("CAP BP calib error (C)\n");
		goto _exit;
	}
	/*Start Cal White Image*/
	ret = sp5kCalibrationCfgSet(SP5K_CALIBRATION_CAP_BP, 1);
	if (ret != SUCCESS) {
		RPrintf("CAP BP calib error (D)\n");
		goto _exit;
	}

/************Black image************/
	/*Show OSD , and check OK is pressed.*/
	#if KIT_WITH_MSHUT
	appMShutClose();
	#else 
	dbgOsd_Print(1, "Black Image");	
	dbgOsd_Print(2, "Cover the Lens ,and Press OK!");	
	while(IS_KEY_OK_PRESSED == 0);	
	RPrintf("Press OK!");	
	#endif
	dbgOsd_Print(2, "  Waiting.......  ");	

	expidx = CALIB_EXP_1_32;
	agcidx = cap_svidx_to_agcidx(CALIB_ISO_400);
	sp5kStillCapExpAgcSet(expidx ,agcidx);
	printf("expidx : %d, agcidx = %d\n",expidx ,agcidx);
	sp5kSensorVvalidWait();
	sp5kSensorVvalidWait();
	sp5kSensorVvalidWait();
	sp5kSensorVvalidWait();
	ret = sp5kCalibrationCfgSet(SP5K_CALIBRATION_CAP_BP, 9);
	if (ret != SUCCESS) {
		printf("CAP BP calib error (E)\n");
		goto _exit;
	}
	ret = sp5kCalibrationCfgSet(SP5K_CALIBRATION_OFF, SP5K_CALIBRATION_CAP_BP);
	if (ret != SUCCESS) {
		printf("CAP BP calib error (F)\n");
		goto _exit;
	}

_exit:
	if(ret != SUCCESS) {
		calibdata.CapDPCSign = 0;
		printf("\n!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
		printf("!!!! cali: Cap BP FAIL \n");
		printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n");
		dbgOsd_Print(2, "cali: Cap BP FAIL!");
	}
	else {
		calibdata.CapDPCSign = 1;
		dbgOsd_Print(2, "cali: Cap BP OK!");

	}
	tbUtil_QuickSweepHostMessage(3000);
	appStateChange(APP_STATE_CALIBRATION,STATE_PARAM_NORMAL_INIT);
#else
	DBG_PRINT("Calib Capture Bp\n");
	CLEAR_OSD_SRC;
	appStateChange(APP_STATE_STILL_PREVIEW,STATE_PARAM_NORMAL_INIT);
	sp5kTimeDelay(SP5K_TIME_DELAY_1MS,	1000);
	CLEAR_OSD_SRC;
	appOsdLib_TextDraw(0,0,SP5K_GFX_ALIGN_TOP_LEFT,"[CALIB Capture Bp LV10]");

	tbUtil_CalibCdspSet(1/* isObOn */);

	calibLensZoomSegGo(6);
	appIrisLargeSet();
	sp5kAeModeSet(SP5K_AE_MODE_OFF);
	sp5kAwbModeSet(SP5K_AWB_MODE_OFF);
	sp5kPreviewExpAgcSet(CALIB_EXP_1_128 ,CALIB_ISO_200);
	sp5kStillCapExpAgcSet(TVIDX_TO_CAP_EXPIDX(CALIB_EXP_1_128) ,CALIB_ISO_200);

	sp5kCalibBpCfgSet(SP5K_CALIB_BP_THR, (UINT32)thr);
	sp5kCalibBpCfgSet(SP5K_CALIB_BP_MAX, 3000);
	sp5kCalibBpCfgSet(SP5K_CALIB_BP_FILENAME, (UINT32)RES_6330_BPSTILL);
	sp5kCalibrationCfgSet(SP5K_CALIBRATION_CAP_BP, 1);
	appHostMsgWait(SP5K_MSG_CALI_COMPLETE, NULL, 6000);

	lensMShutPowerOn();
	sp5kTimeDelay(SP5K_TIME_DELAY_1MS,20);
	appMShutClose();
	sp5kSensorVvalidWait();
	sp5kCalibrationCfgSet(SP5K_CALIBRATION_CAP_BP, 9);
	appHostMsgWait(SP5K_MSG_CALI_COMPLETE, NULL, 6000);
	sp5kCalibrationCfgSet(SP5K_CALIBRATION_OFF, SP5K_CALIBRATION_CAP_BP);
	appHostMsgWaitExact(SP5K_MSG_MODE_READY, SP5K_MODE_STILL_PREVIEW, 2000);
	sp5kAeModeSet(SP5K_AE_MODE_INFO_ONLY);
	sp5kSensorVvalidWait();
	sp5kAwbModeSet(SP5K_AWB_MODE_INFO_ONLY);
	lensMShutPowerOn();
	sp5kTimeDelay(SP5K_TIME_DELAY_1MS,20);
	appMShutOpen();
	sp5kTimeDelay(SP5K_TIME_DELAY_1MS,1000);
	lensMShutPowerOff();
	calibLensZoomSegGo(0);
	calibdata.CapDPCSign = 1;
	sp5kTimeDelay(SP5K_TIME_DELAY_1MS,	1000);
	appStateChange(APP_STATE_CALIBRATION,STATE_PARAM_NORMAL_INIT);
#endif
}

void appCalibFlash()
{
	DBG_PRINT("Calib Flash\n");

	appStateChange(APP_STATE_CALIB_FLASH, STATE_PARAM_NORMAL_INIT);
}


UINT32
appCalibDataLoad(
	void
)
{
    /*UINT32 i,j;*/
	
	DBG_PRINT("[Calib Data Load...]\n");

    if(ReadFile(RES_CALIB_DATA, (UINT8*)(&calibdata), sizeof(appCalibData_t))!= SUCCESS) {
		DBG_PRINT("[ERR] read file[%s] fail\n", RES_CALIB_DATA);
		/* set default */
		_calibDataDefaultSet();
		_calibDataSave();
	}
	if(ReadFile(RES_CALIB_AF_DATA, (UINT8*)(&afCalibdata), sizeof(appAfCalibData_t))!= SUCCESS) {
		DBG_PRINT("[ERR] read file[%s] fail\n", RES_CALIB_AF_DATA);
		/* set default */
		_calibAfDataDefaultSet();
		_calibAfDataSave();

	}
	appCalibDataInit();

#if 0
printf("calibdata.AfSign = 0x%x\n",calibdata.AfSign);
printf("calibdata.ObSign = 0x%x\n",calibdata.ObSign);
printf("calibdata.ShutterSign = 0x%x\n",calibdata.ShutterSign);
printf("calibdata.IsoSign = 0x%x\n",calibdata.IsoSign);
printf("calibdata.WbSign = 0x%x\n",calibdata.WbSign);
printf("calibdata.FlashSign = 0x%x\n",calibdata.FlashSign);
printf("calibdata.PvDPCSign = 0x%x\n",calibdata.PvDPCSign);
printf("calibdata.CapDPCSign = 0x%x\n",calibdata.CapDPCSign);
printf("calibdata.LcdSign = 0x%x\n",calibdata.LcdSign);
printf("calibdata.PvRawSign = 0x%x\n",calibdata.PvRawSign);
printf("calibdata.StillRawSign = 0x%x\n",calibdata.StillRawSign);
printf("calibdata.MTBFSign = 0x%x\n",calibdata.MTBFSign);
printf("calibdata.CIPASign = 0x%x\n",calibdata.CIPASign);
printf("calibdata.BatterySign = 0x%x\n",calibdata.BatterySign);
printf("calibdata.KeySign = 0x%x\n",calibdata.KeySign);
printf("calibdata.capSatGainSign = 0x%x\n",calibdata.capSatGainSign);
printf("calibdata.rawBatchEnable = 0x%x\n",calibdata.rawBatchEnable);
printf("calibdata.prefhStamp = 0x%x\n",calibdata.prefhStamp);
printf("calibdata.afFullScan = 0x%x\n",calibdata.afFullScan);
printf("calibdata.IsoObSign = 0x%x\n",calibdata.IsoObSign);
printf("calibdata.pvOBSign = 0x%x\n",calibdata.pvOBSign);
printf("calibdata.pcApiTest = 0x%x\n",calibdata.pcApiTestSign);
printf("calibdata.videoRecDateStamp = 0x%x\n",calibdata.videoRecDateStamp);
printf("calibdata.Dzoom = 0x%x\n",calibdata.dzoomEnable);
printf("calibdata.aedebug = 0x%x\n",calibdata.aedebug);
printf("calibdata.awbdebug = 0x%x\n",calibdata.awbdebug);

for (i=0;i<COUNT_OF(calibdata.Ob); i++) {
    printf("calibdata.Ob[%d] = 0x%x\n", i,calibdata.Ob[i]);
}
    
printf("calibdata.Flash = 0x%x\n",calibdata.Flash);
printf("calibdata.PvIso = 0x%x\n",calibdata.PvIso);
printf("calibdata.CapIso = 0x%x\n",calibdata.CapIso);
printf("calibdata.ShutterEarly[0] = 0x%x\n",calibdata.ShutterEarly[0]);
printf("calibdata.ShutterEarly[1] = 0x%x\n",calibdata.ShutterEarly[1]);
printf("calibdata.WbRgain = 0x%x\n",calibdata.WbRgain);
printf("calibdata.WbBgain = 0x%x\n",calibdata.WbBgain);
printf("calibdata.MTBF = 0x%x\n",calibdata.MTBF);

printf("calibdata.sensDiff = 0x%x\n",calibdata.sensDiff);
printf("calibdata.irisDiff = 0x%x\n",calibdata.irisDiff);
printf("calibdata.capSatGain = 0x%x\n",calibdata.capSatGain);

for (i=0;i<COUNT_OF(calibdata.isoOb); i++) {
    for(j=0;j<COUNT_OF(calibdata.isoOb[0]);j++)
	  printf("calibdata.isoOb[%d][%d] = %d\n",i,j,calibdata.isoOb[i][j]);
}
    
printf("calibdata.CIPACapCnt = 0x%x\n",calibdata.CIPACapCnt);

for (i=0;i<COUNT_OF(calibdata.pvOB); i++) {
    printf("calibdata.pvOB[%d] = 0x%x\n", i,calibdata.pvOB[i]);
}

printf("afCalibdata.AfPosition[0]= 0x%x\n",afCalibdata.AfPosition[0]);
printf("afCalibdata.AfPosition[1] = 0x%x\n",afCalibdata.AfPosition[1]);
printf("afCalibdata.AfPosition[2] = 0x%x\n",afCalibdata.AfPosition[2]);
printf("afCalibdata.AfPosition[3] = 0x%x\n",afCalibdata.AfPosition[3]);
printf("afCalibdata.AfPosition[4] = 0x%x\n",afCalibdata.AfPosition[4]);
printf("afCalibdata.AfPosition[5] = 0x%x\n",afCalibdata.AfPosition[5]);
printf("afCalibdata.AfPosition[6] = 0x%x\n",afCalibdata.AfPosition[6]);
#endif

	return SUCCESS;
}

void
appCalibDataSave(
	void
)
{
	_calibDataSave();
	_calibAfDataSave();
}


UINT8
appCalibUsbSettingGet(
	void
)
{
	return 0/*calibdata.UsbSetting*/;
}

UINT8
appCalibAPITestSettingGet(
	void
)
{
	return calibdata.pcApiTestSign;
}

void
appCalibAPITestSettingSet(
	UINT8 enable
)
{
	 calibdata.pcApiTestSign=enable;
}

UINT8

appCalibVideoStampSettingGet(
	void
)
{
	return calibdata.videoRecDateStamp;
}

void
appCalibVideoStampSettingSet(
	UINT8 enable
)
{
	 calibdata.videoRecDateStamp=enable;
}

UINT8
appCalibDzoomEnableGet(
	void
)
{
	return calibdata.dzoomEnable;
}

void
appCalibDzoomEnableSet(
	UINT8 enable
)
{
	 calibdata.dzoomEnable=enable;
}

UINT8
appCalibRawBatchEnableGet(
	void
)
{
	return calibdata.rawBatchEnable;
}

UINT32
appCalibDefaultMshutterDelayGet(
	void
)
{
	return CALIB_MSHUT_DEFAULT_DELAY;
}

appCalibData_t *appCalibDataGet(){
	return(&calibdata);
}

void
cmdCalib(
UINT32 argc,
UINT8 *arg[],
UINT32 v[]
)
{
	if (argc==1) {
		DBG_PRINT("calib usage:\n");
		DBG_PRINT(" calib report 0/1 : report calib result (1:WrFile) \n");
		DBG_PRINT(" calib init\n");
		DBG_PRINT(" calib af\n");
		DBG_PRINT(" calib shdelay _delay_ : shutter early delay \n");
		DBG_PRINT(" calib ob\n");
		DBG_PRINT(" calib iso\n");
		DBG_PRINT(" calib sub _expidx_ _delay_ : adjust SUB and shutter trigger timing \n");
		DBG_PRINT(" calib wb\n");
		DBG_PRINT(" calib vraw : show view raw info \n");
		DBG_PRINT(" calib save_vraw : save view raw image \n");
		DBG_PRINT(" calib sraw : show capture raw info \n");
		DBG_PRINT(" calib save_sraw : save capture raw image \n");
		DBG_PRINT(" calib badpixel\n");
		DBG_PRINT(" calib lcd pattern\n");
		DBG_PRINT(" calib obadv : ob on diff ISO\n");
		DBG_PRINT(" calib pvLuma : pv raw data luma\n");
		DBG_PRINT(" calib stillLuma : still raw data luma\n");
	} else if (strcmp(arg[1], "report")==0) {
		DBG_PRINT("cali report %d \n", v[2]);
		appCalibReport(!!v[2]);
	} else if (strcmp(arg[1], "init")==0) {
		DBG_PRINT("cali init\n");

	} else if (strcmp(arg[1], "af")==0) {
		DBG_PRINT("cali af\n");

	} else if (strcmp(arg[1], "shdelay")==0) {
		DBG_PRINT("cali shdelay %d \n", v[2]);
		sp5kSnapShutterDelaySet(SP5K_SNAP_SHUTTER_CLOSE_EXP_END, 0, v[2]);
		#if KIT_LMI_CTRL
		appLensMShutLmiModeSet(1); /* msh Close */
		APP_SLEEP_MS(10);
		#endif
		sp5kModeSet(SP5K_MODE_STILL_SNAP);
	} else if (strcmp(arg[1], "ob")==0) {
		DBG_PRINT("cali ob %d\n",v[2]);
		#if 0
		WRITE8(0xb00022d2, 0);
		WRITE8(0xb0002230, 0);
		WRITE8(0xb0002240, 0);
		WRITE8(0xb00022d2, 0);
		WRITE8(0xb0002266, 0);
		WRITE8(0xb0002270, 0);
		WRITE8(0xb0002280, 0);
		WRITE8(0xb00022b0, 0);
		WRITE8(0xb00022b8, 0);
		WRITE8(0xb00022d8, 0);
		WRITE8(0xb00022e0, 0);
		#endif
		sp5kImgObModeCfgSet(SP5K_OB_MANUAL);
		sp5kImgObSet(v[2]);
	} else if (strcmp(arg[1], "vraw")==0) {
		DBG_PRINT("cali View Raw\n");
		sp5kCaliPreviewRawCallbackSet(appCalibRawCb);
		sp5kCalibrationCfgSet(SP5K_CALIBRATION_CALLBACK, SP5K_CALIB_CB_PREVIEW_RAW);
		sp5kTimeDelay(SP5K_TIME_DELAY_1MS,	500);
		sp5kCalibrationCfgSet(SP5K_CALIBRATION_OFF, SP5K_CALIB_CB_PREVIEW_RAW);
		sp5kTimeDelay(SP5K_TIME_DELAY_1MS,	200);
	} else if (strcmp(arg[1], "save_vraw")==0) {
		DBG_PRINT("cali save_vraw\n");
		sp5kCaliPreviewRawCallbackSet(NULL);
		sp5kCalibrationCfgSet(SP5K_CALIBRATION_SAVE_PREVIEW_RAW, 1);
		sp5kTimeDelay(SP5K_TIME_DELAY_1MS,	500);
		sp5kCalibrationCfgSet(SP5K_CALIBRATION_OFF, SP5K_CALIBRATION_SAVE_PREVIEW_RAW);
		sp5kTimeDelay(SP5K_TIME_DELAY_1MS,	200);
	} else if (strcmp(arg[1], "sraw")==0) {
		DBG_PRINT("cali Still Raw\n");
		sp5kStillCapRawCallbackSet(appCalibRawCb);
		appStillUtil_CalibCaptureSet(1);
		sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 500);
		appStillUtil_CalibCaptureSet(0);
		sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 200);
	} else if (strcmp(arg[1], "save_sraw")==0) {
		DBG_PRINT("cali save_sraw\n");
		sp5kStillCapRawCallbackSet(NULL);
		sp5kCalibrationCfgSet(SP5K_CALIBRATION_SAVE_STILL_RAW, 1);
		sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 500);
		sp5kCalibrationCfgSet(SP5K_CALIBRATION_OFF, SP5K_CALIBRATION_SAVE_STILL_RAW);
		sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 200);
	} else if (strcmp(arg[1], "sub")==0) {
		/*-- test for shutter trigger & exposure eSUB position.
		 * v[2] specify the expidx which controls the eSUB of CCD
		 * v[3] specify the shutter delay which controls the close timing of shutter @ capture time
		 */
		printf("calib sub %d %d \n", v[2], v[3]);
		tbSensor_MShut_SUB_Test(v[2], v[3]);
	} else if (strcmp(arg[1], "wb")==0) {
		DBG_PRINT("cali wb\n");
		printf("gain(0x%x,0x%x)\n",calibdata.WbRgain,calibdata.WbBgain);
	} else if (strcmp(arg[1], "badpxl")==0) {
		DBG_PRINT("cali badpixel\n");

	}else if (strcmp(arg[1], "lcd")==0) {
	#if 0 // should use playback
		UINT8 i;
		char buf[32];
		char *signalname[8] = { "red  ","green","blue ","white","black","gray","color",NULL};
		UINT8 signal[8] = { 0x05,0x03,0x06,0x00,0x07,0x09,0x0b,0xff};
		DBG_PRINT("lcd check\n");
		WRITE8(0xb00022d2, 0);//temp
		WRITE8(0xb00090d0, 1);
		WRITE8(0xb00090d4, 3);
		i=0;
		do{
			sprintf(buf,"%s",signalname[i]);
			appGfxStringDrawEx(0, 0, SP5K_GFX_ALIGN_TOP_LEFT, SP5K_GFX_ENCODE_ASCII, buf, PAL_ERASE_OSD, 0xff, 0xff);
			WRITE8(0xb00090d6,signal[i]);
			sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 2000);
			i++;
		}while(signal[i]!=0xff);

		WRITE8(0xb00090d0, 0);
	#endif
	}
	else if (strcmp(arg[1], "obadv")==0) {
		DBG_PRINT("cali OB on diff ISO\n");
		_calibObAdv(CALIB_EXP_1_32);
	}
	else if (strcmp(arg[1], "pvLuma")==0) {
		UINT16 luma;
		printf("pv raw luma ...\n");
		luma = tbUtil_PvRawLumaGet();
		printf("pv luma = %d\n",luma);
	}
	else if (strcmp(arg[1], "stillLuma")==0) {
		UINT16 luma;
		printf("still raw luma ...\n");
		luma = tbUtil_CapRawLumaGet();
		printf("pv luma = %d\n",luma);
	}
}



