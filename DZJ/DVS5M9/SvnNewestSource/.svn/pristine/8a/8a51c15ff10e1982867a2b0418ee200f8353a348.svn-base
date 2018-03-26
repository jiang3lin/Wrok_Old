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
#ifndef APP_AAA_H
#define APP_AAA_H
#include "api/sp5k_ae_api.h"
#include "api/sp5k_awb_api.h"
#include "api/sp5k_cdsp_api.h"

#include "app_com_def.h"

/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/



enum {
	AE_AGC_MIN_BOUNDARY = 0,
	AE_AGC_MAX_BOUNDARY,
};

enum {
	AE_EXP_MIN_BOUNDARY = 0,
	AE_EXP_MAX_BOUNDARY,
};

#define AE_ZOOM_LEVEL_MAX	7
#define AE_BIAS_MAX			13

#define AE_IRIS_AUTO 0
#define AE_SHUTTER_AUTO 0
#define AE_ISO_AUTO 0

#define AV_UNIT		1
#define EV_UNIT		16
#define SV_UNIT		EV_UNIT
#define TV_UNIT		EV_UNIT
#define SV_UNIT		EV_UNIT

#define AE_LV10		160
#define AE_LV11		(AE_LV10 + EV_UNIT)
#define AE_LV12		(AE_LV11 + EV_UNIT)
#define AE_LV13		(AE_LV12 + EV_UNIT)
#define AE_LV14		(AE_LV13 + EV_UNIT)
#define AE_LV15		(AE_LV14 + EV_UNIT)

/*****************************************************************
 * STD ISO/EXP, prefix with STD_ISO_ & STD_EXP_
 *****************************************************************/
#define STD_ISO_25		(STD_ISO_50 - SV_UNIT)			// 48
#define STD_ISO_50		(STD_ISO_100 - SV_UNIT)			// 64
#define STD_ISO_100		(5*SV_UNIT)						// 80
#define STD_ISO_200		(STD_ISO_100 + SV_UNIT)			// 96
#define STD_ISO_400		(STD_ISO_200 + SV_UNIT)			// 112
/*s Add by Aries 09/08/11*/
#define STD_ISO_640		(STD_ISO_400 + (SV_UNIT/2))		// 120
/*e Add by Aries 09/08/11*/
#define STD_ISO_800		(STD_ISO_400 + SV_UNIT)			// 128
#define STD_ISO_1600	(STD_ISO_800 + SV_UNIT)			// 144

#define STD_EXP_16_01	(STD_EXP_8_01 - TV_UNIT)
#define STD_EXP_8_01	(STD_EXP_4_01 - TV_UNIT)
#define STD_EXP_4_01	(STD_EXP_2_01 - TV_UNIT)
#define STD_EXP_2_01	(STD_EXP_1_01 - TV_UNIT)

#define STD_EXP_1_01	(STD_EXP_1_02 - TV_UNIT)		// 0
#define STD_EXP_1_02	(STD_EXP_1_04 - TV_UNIT)		//16

#define STD_EXP_1_04	(STD_EXP_1_08 - TV_UNIT) 		//32
#define STD_EXP_1_08	(STD_EXP_1_16 - TV_UNIT)		//48
#define STD_EXP_1_16	(STD_EXP_1_32 - TV_UNIT)		//64
#define STD_EXP_1_24	(STD_EXP_1_32 - (TV_UNIT / 2))	//72
#define STD_EXP_1_32	(5*TV_UNIT)						//80
#define STD_EXP_1_64	(STD_EXP_1_32 + TV_UNIT)		//96
#define STD_EXP_1_128	(STD_EXP_1_64 + TV_UNIT)		//112
#define STD_EXP_1_256	(STD_EXP_1_128 + TV_UNIT)		//128
#define STD_EXP_1_512	(STD_EXP_1_256 + TV_UNIT)		//144
#define STD_EXP_1_1024	(STD_EXP_1_512 + TV_UNIT)		//160
#define STD_EXP_1_2048	(STD_EXP_1_1024 + TV_UNIT)		//176
#define STD_EXP_1_4096	(STD_EXP_1_2048 + TV_UNIT)		//192
#define STD_EXP_1_8192	(STD_EXP_1_4096 + TV_UNIT)		//208

/*****************************************************************
 * VIEW ISO/EXP, prefix with PV_ISO_ & PV_EXP_
 *****************************************************************/

/*****************************************************************
 * CAPTURE ISO/EXP, prefix with CAP_ISO_ & CAP_EXP_
 *****************************************************************/
#define ISO_NUM		6
#define ISO_50		(ISO_100 - SV_UNIT/2)
#define ISO_75		(ISO_100 - SV_UNIT/4)
#define ISO_100		(80-VIEW_AGCIDX_OFS)
#define ISO_200		(ISO_100 + SV_UNIT)
#define ISO_400		(ISO_200 + SV_UNIT)
#define ISO_800		(ISO_400 + SV_UNIT)
#define ISO_1600	(ISO_800 + SV_UNIT)

/*s Modify  by Aries 09/07/27*/
#if 1
#define EXP_NUM		21
#else
#define EXP_NUM		18
#endif
/*e Modify  by Aries 09/07/27*/

#define EXP_16_01	(EXP_8_01 - TV_UNIT)
#define EXP_8_01	(EXP_4_01 - TV_UNIT)
#define EXP_4_01	(EXP_2_01 - TV_UNIT)
#define EXP_2_01	(EXP_1_01 - TV_UNIT)

#define EXP_1_01	(EXP_1_02 - TV_UNIT) // 0
#define EXP_1_02	(EXP_1_04 - TV_UNIT) // 16

#define EXP_1_04	(EXP_1_08 - TV_UNIT) // 32
#define EXP_1_08	(EXP_1_16 - TV_UNIT) // 48
#define EXP_1_16	(EXP_1_32 - TV_UNIT) // 64
#define EXP_1_20	(EXP_1_32 - 11)
#define EXP_1_24	(EXP_1_32 - (TV_UNIT / 2))
#define EXP_1_32	(80)
#define EXP_1_64	(EXP_1_32 + TV_UNIT) //96
#define EXP_1_128	(EXP_1_64 + TV_UNIT) //112
#define EXP_1_256	(EXP_1_128 + TV_UNIT) //128
#define EXP_1_512	(EXP_1_256 + TV_UNIT) //144
#define EXP_1_1024	(EXP_1_512 + TV_UNIT) //160
#define EXP_1_2048	(EXP_1_1024 + TV_UNIT) //176
#define EXP_1_4096	(EXP_1_2048 + TV_UNIT) // 192
#define EXP_1_8192	(EXP_1_4096 + TV_UNIT) // 208
#define EXP_1_16384	(EXP_1_8192 + TV_UNIT) // 224

#define AE_DEFAULT_OB			CALIB_AE_DEFAULT_OB 

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef struct appAeStatus_s {
	SINT16 iris;
	SINT16 shutter;
	SINT16 iso;
	SINT16 ev;
	SINT16 av;
	SINT16 tv;
} appAeStatus_t;

typedef struct appAeSet_s {
   UINT8 pasm;
   UINT8 scene;
   UINT8 meter;
   UINT8 evbias;
   UINT8 iris;
   UINT8 shutter;
   UINT8 iso;
   UINT8 aeb;
   UINT8 stabilizer;
   UINT8 dist;
   UINT8 flash;
} appAeSet_t;


typedef struct appAeCb_s {
	UINT32 frameid;
	UINT16 ob;
	SINT16 pviso;
	SINT16 capiso;
	UINT16 luma;
	SINT16 lvidx;
	SINT16 expidx;
	SINT16 agcidx;
	SINT16 aeStep;
	SINT16 avidx;
	SINT32 capexp;
	SINT32 capagc;
	SINT16 capav;
	SINT32 capaebexp[3];
	SINT32 capaebagc[3];
	SINT16 evdiff;
	UINT8 target;
	UINT8 zml;
	UINT8 stable;
	UINT8 mode;
	/*UINT8 ae_max_isoidx;*/
	UINT8 cap_max_isoidx;
	UINT8 iris_stop;
	appAeSet_t aeset;
} appAeCb_t;

typedef struct appAePara_s {
	#if 0
	SINT16 tv;
	SINT16 sv;
	SINT16 av;
	#else
	SINT32 tv; /* view TVIDX*/
	SINT32 sv; /* view SVIDX*/
	SINT32 av; /* view AVIDX*/
	SINT32 cap_tv; /* CAPTURE TVIDX*/
	SINT32 cap_sv; /* CAPTURE SVIDX*/
	SINT32 cap_av; /* CAPTURE AVIDX*/
	SINT16 ui_iris;
	SINT16 ui_iso;
	UINT8  cap_flash;
	SINT16 save_tv; /* to save real tv while flash is on (debug purpose)*/
	SINT16 save_sv; /* to save real sv while flash is on*/
	SINT16 save_ui_iso; /* to save real ui_iso while flash is on*/
	UINT8  rsvd;
	#endif
} appAePara_t;

typedef struct appAwbStatus_s {
	UINT32 wbMode;
	UINT32 rGain;
	UINT32 bGain;
} appAwbStatus_t;

/*s Add by Aries 10/03/11*/
typedef void (*appAeCBF_t)(SINT32 expidxCB,SINT32 agcidxCB);
/*e Add by Aries 10/03/11*/


/**************************************************************************
 *                               M A C R O S                              *
 **************************************************************************/
/* check API doc:sp5kAeEvUnitGet() */

#define VIEW_AGCIDX_OFS						(80-(52-48)) /*=76 This is the offset of real ISO100 - default PV ISO100 agcidx*/
#define CAPTURE_EXPIDX_OFS					0 /* EVB*/
#define SV_TO_VIEW_AGCIDX(sv)		(((sv)*evUnit.evUnit)-evUnit.agcIdx0Unit-VIEW_AGCIDX_OFS)
#define SVIDX_TO_VIEW_AGCIDX(svidx)	((svidx)-evUnit.agcIdx0Unit-VIEW_AGCIDX_OFS)
#define VIEW_AGCIDX_TO_SVIDX(agc)	((agc)+evUnit.agcIdx0Unit+VIEW_AGCIDX_OFS)

#define SV_TO_CAP_AGCIDX(sv)		((sv)*evUnit.evUnit-evUnit.agcIdx0Unit+aeCapIsoOfs)
#define SVIDX_TO_CAP_AGCIDX(svidx)	((svidx)-evUnit.agcIdx0Unit+aeCapIsoOfs)
/*--Never used !!! #define CAP_AGCIDX_TO_SVIDX(agc)	((agc)+evUnit.agcIdx0Unit-aeCapIsoOfs)*/

#define TV_TO_EXPIDX(tv)			((tv)*evUnit.evUnit-evUnit.expIdx0Unit)
#define TVIDX_TO_EXPIDX(tvidx)		((tvidx)*evUnit.evUnit-evUnit.expIdx0Unit)
#define EXPIDX_TO_TVIDX(exp)		((exp)+evUnit.expIdx0Unit)

#define TV_TO_CAP_EXPIDX(tv)		((tv)*evUnit.evUnit+evUnit.expIdx0Unit)
#define TVIDX_TO_CAP_EXPIDX(tvidx)	((tvidx)-evUnit.expIdx0Unit+CAPTURE_EXPIDX_OFS)
#define CAP_EXPIDX_TO_TVIDX(agc)	((agc)+evUnit.expIdx0Unit-CAPTURE_EXPIDX_OFS)

#define UI_IRIS_TO_AVIDX(av)		((av)*evUnit.evUnit)
#define AVIDX_TO_UI_IRIS(uidx)		((uidx)/evUnit.evUnit)

#define TVIDX_TO_UI_SHUTTER(tvidx)	(((tvidx)-evUnit.expIdx0Unit)/evUnit.evUnit+5)
#define UI_SHUTTER_TO_TVIDX(uidx)	((((uidx)-5)*evUnit.evUnit+evUnit.expIdx0Unit)
#define SVIDX_TO_UI_ISO(svidx)		(((svidx)-evUnit.agcIdx0Unit+aeViewIsoOfs+8)/evUnit.evUnit-2)
#define UI_ISO_TO_SVIDX(uidx)		((((uidx)+3)*evUnit.evUnit+evUnit.agcIdx0Unit)

/**************************************************************************
 *          M O D U L E   V A R I A B L E S   R E F E R E N C E S         *
 **************************************************************************/
extern sp5kEvUnit_t evUnit;
extern sp5kExpLimit_t expLimitPv,expLimitCap;
extern sp5kAgcLimit_t agcLimitPv,agcLimitCap;
extern SINT16 aeViewIsoOfs;
extern SINT16 aeCapIsoOfs;

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/
/* ae */
/*s Add by Aries 10/03/11*/
void appAeConvergedCBRegister(appAeCBF_t paeCBF);
void appAeConvergedCBSet(SINT32 expidx,SINT32 agcidx);
/*e Add by Aries 10/03/11*/
void appAeInit();
void appAeEnable(BOOL fEnable);
void appAeInfoEnable(BOOL fEnable);
void appAePreviewRestore();
appAeCb_t *appAeCbGet(void);
appAePara_t *appAeParaGet(void);
appAeSet_t * appAeSetGet(void);
UINT8 appAeStableGet(void);
UINT16	appAeLumaGet(void);
UINT16 appAeTargetGet(void);
UINT8 appAeFastAdjust(UINT32 param);
UINT32 appAePreviewAdjust(UINT32 param);
UINT32 appAePreviewAdjustB(UINT32 param); //dual sensor
UINT32 appAePreviewExpAdjust(UINT32 param, SINT32 *pstep);
void appAeIrisGet(UINT32 *numerator, UINT32 *denominator);
void appAeCapParaGet(SINT16 *piris, SINT16 *pshutter, SINT16 *piso);
UINT32 appAeIrisSet(UINT8 irisStop); /*UINT32 appAeIrisSet(UINT32 APidx);*/
void appAeZoomLevelSet(UINT32 zml);
void appAeCapShutterSet(UINT8 shutidx);
void appAeCapIsoSet(UINT8 isoidx);
void appAeCapIrisSet(UINT8 irisidx);
void appAeMeterSet(UINT8 bMeter);
void appAeEvBiasSet(UINT8 bEvBias);
void appAeAebSet(UINT8 aeb);
void appAeStabilizerSet(UINT8 stabilizer);
void appAeDistStatusGet(UINT8 *dist);
void appAePreviewShutBoundarySet(UINT8 pasmvs,UINT32 shH, UINT32 shL);
void appAePreviewIsoBoundarySet(UINT8 pasmv,UINT32 max, UINT32 min);
/*s Add by Aries 09/08/11*/
void appAeExpFlickerModeSet(UINT8 flickeridx);
void appAeFrameRateSet(UINT32 fps);
/*e Add by Aries 09/08/11*/
void appAePvReset(void); /* added by Eric Lee 100712 */
void appAePreviewShutBoundaryGet(UINT32 *pmax, UINT32 *pmin);
void appAeCapShutBoundarySet(UINT8 scene, UINT32 shH, UINT32 shL);
void appAeCapShutBoundaryGet(UINT32* shH, UINT32* shL);
void appAeCaptureSet(UINT8 imgId);
void appAePreviewSet(void);
void cmdAE(UINT32 argc, UINT8 *arg[], UINT32 v[]);
void appAeGetCurrentExpIsoUiIdx(UINT16* ui_exp_idx, UINT16* ui_iso_idx); 
void appAeCaptureFlashAeOverrideSet(void);

/*lester 20120612 s*/
#ifdef FLASH_ROLLING
UINT8 appAeFlashNeedGet(void); 
#endif
/*lester 20120612 e*/

/* awb */
void appAwbInit();
void aaaAwbEnable(BOOL fEnable);
void aaaAwbInfoEnable(BOOL fEnable);
UINT32 appAwbPreviewAdjust(UINT32 param);
void cmdAWB(UINT32 argc, UINT8 *arg[], UINT32 v[]);
void SP_AWBInit();
void SP_AwbGainRead(void);

/* af */
void appFocusModeChange(UINT8 idx);
void appFocusScan(UINT32 param);

void appAfValueReset(void);




UINT16 appFocusInitPosGet(UINT8 idx);
UINT32 appAfInit(void);
void aaa_AutoFocus(void);
UINT8 AfStatusGet(void);
void AfStatusSet(UINT8 status);
UINT16 * appFocusInfoGet(void);
UINT16 appFocusMaxPosGet();

/* iq&aaa debug info in exif */
void SP_IQLogInit(void);
void SP_IQLogPirntf(const char *szFmt, ...);
void SP_IQAWBLog(void);

void appAeFlickerTableExpAgcIdxGet(UINT32 fps,UINT32 flicker,UINT16 tblIdx,UINT16 *expIdx,UINT16 *agcIdx);
void appAeFlickerTableExpAgcIdxSet(UINT32 tblIdx);

#endif  /* APP_AAA_H */

