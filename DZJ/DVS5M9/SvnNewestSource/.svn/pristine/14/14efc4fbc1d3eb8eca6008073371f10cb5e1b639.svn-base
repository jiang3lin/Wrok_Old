/**************************************************************************
 *                                                                        *
 *       Copyright (c) 2004-2006 by Sunplus Technology Co., Ltd.          *
 *                                                                        *
 *  This software is copyrighted by and is the property of Sunplus        *
 *  Technology Co., Ltd. All rights are reserved by Sunplus Technology    *
 *  Co., Ltd. This software may only be used in accordance with the       *
 *  corresponding license agreement. Any unauthorized use, duplication,   *
 *  distribution, or disclosure of this software is expressly forbidden.  *
 *                                                                        *
 *  This Copyright notice MUST not be removed or modified without prior   *
 *  written consent of Sunplus Technology Co., Ltd.                       *
 *                                                                        *
 *  Sunplus Technology Co., Ltd. reserves the right to modify this        *
 *  software without notice.                                              *
 *                                                                        *
 *  Sunplus Technology Co., Ltd.                                          *
 *  19, Innovation First Road, Science-Based Industrial Park,             *
 *  Hsin-Chu, Taiwan.                                                     *
 *                                                                        *
 *  Author: Anmin Deng                                                    *
 *                                                                        *
 **************************************************************************/
#ifndef _SP5K_AE_API_H_
#define _SP5K_AE_API_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"
#include "middleware/aastat_def.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
typedef enum {
	SP5K_AE_MODE_OFF = 0,
	SP5K_AE_MODE_INFO_ONLY = 2,
} sp5kAeMode_t;

typedef enum {
	SP5K_AE_TARGET_LUMA,
	SP5K_AE_COMPENSATION,
	SP5K_AE_BACKLIGHT,
	SP5K_AE_PARAM_TOTAL,
} sp5kAeParamSel_t;

typedef enum {
	SP5K_AE_ACCUM_PERIOD,
	SP5K_AE_INVALID_MSG_SEND,
	SP5K_AE_HIS_Y_REQUEST,
	SP5K_AE_HIS_RGB_REQUEST,
	SP5K_AE_WIN_POS_TEST, /* obsolete */
	SP5K_AE_Y_LOW_THR, /* obsolete */
	SP5K_AE_RGB_HIGH_THR, /* obsolete */
	SP5K_AE_HIS_ALL_REQUEST,
	SP5K_AE_WIN_PATH, /* obsolete */
	SP5K_AE_HIS_ALL_READY_REQUEST,
	SP5K_AE_RGB_USE_BAYER_PATTERN,
	SP5K_AE_CONTINOUS_ENABLE,
	SP5K_AE_HIS_WIN_PATH,
	SP5K_AE_CFG_TOTAL,
} sp5kAeCfgSel_t;

typedef enum {
	SP5K_AE_WIN_PATH_BEFORE_WDR = 0,
	SP5K_AE_WIN_PATH_AFTER_WDR,
	SP5K_AE_WIN_PATH_2ND_PATH,
} sp5kAeWinPathSel_t;


typedef aeEvUnit_t sp5kEvUnit_t;
typedef sensor_expidx_limits_t sp5kExpLimit_t;
typedef sensor_agcidx_limits_t sp5kAgcLimit_t;
typedef aaa_aeExpSts_t sp5kExpSts_t;
typedef aaa_aeAgcSts_t sp5kAgcSts_t;


UINT32 sp5kAeWeightResourceSet(UINT32 aeWinIdx);
UINT32 sp5kAeWeightCfgSet(const UINT8 *pwinCfg);
UINT32 sp5kAeModeSet(UINT32 mode);
UINT32 sp5kAeModeGet(UINT32 *pmode);
UINT32 sp5kAeParamSet(UINT32 selector,UINT32 value);
UINT32 sp5kAeCfgSet(UINT32 selector,UINT32 value);

#define SP5K_AE_EXP_AGC_DONT_CHANGE 0x7fffffff
#define SP5K_AE_EXP_BSHUTTER AE_EXPIDX_BSHUTTER

UINT32 sp5kPreviewExpAgcIrisSet(SINT32 expIdx,UINT32 agcIdx,UINT32 irisIdx);
UINT32 sp5kPreviewIrisActSet(UINT32 irisIdx);
UINT32 sp5kPreviewExpAgcSet(SINT32 expIdx,UINT32 agcIdx);
UINT32 sp5kStillCapExpAgcSet(SINT32 expIdx,UINT32 agcIdx);
UINT32 sp5kStillCapLongExpStop(void);
UINT32 sp5kPreviewAfeOffsetSet(UINT32 idx);
UINT32 sp5kStillCapAfeOffsetSet(UINT32 idx);
UINT32 sp5kPreviewExpAgcIrisGet(SINT32 *pexpIdx,UINT32 *pagcIdx,UINT32 *pirisIdx);
UINT32 sp5kPreviewExpAgcGet(SINT32 *pexpIdx,UINT32 *pagcIdx);
UINT32 sp5kStillCapExpAgcGet(SINT32 *pexpIdx,UINT32 *pagcIdx);
UINT32 sp5kAeExpAgcIrisTestSet(UINT32 mode,SINT32 expIdx,UINT32 agcIdx,UINT32 irisIdx);
UINT32 sp5kAeExpAgcIrisTestModeSet(UINT32 mode,UINT32 enable);
/*For Dual sensor Ae */
UINT32 sp5kPreviewDualSelSet (UINT32 Idx);
UINT32 sp5kStillCapDualSelSet (UINT32 Idx);

typedef aeResult_t sp5kAeResult_t;
SINT32 sp5kAePreviewResultGet(UINT32 param, sp5kAeResult_t *presult);
UINT32 sp5kAeEvUnitGet(sp5kEvUnit_t *punit);
UINT32 sp5kAeEvUnitSet(const sp5kEvUnit_t *punit);
UINT32 sp5kAeExpLimitGet(UINT32 mode,sp5kExpLimit_t *plimit);
UINT32 sp5kAeAgcLimitGet(UINT32 mode,sp5kAgcLimit_t *plimit);
UINT32 sp5kAeExpStsGet(UINT32 expIdx,UINT32 sensorMode,sp5kExpSts_t *pexpsts);
UINT32 sp5kAeAgcStsGet(UINT32 agcIdx,UINT32 sensorMode,sp5kAgcSts_t *pagcsts);


UINT32 sp5kAeApertureDeclare(UINT32 aperIdx);

typedef fpaaaProc_t fpsp5kAeCallback_t;
#define SP5K_AEAWB_INFO_BIT_AEWIN_Y   AASTAT_INFO_FLAG_AEWIN_Y
#define SP5K_AEAWB_INFO_BIT_AEWIN_RGB AASTAT_INFO_FLAG_AEWIN_RGB
#define SP5K_AEAWB_INFO_BIT_AEWIN_R   AASTAT_INFO_FLAG_AEWIN_R
#define SP5K_AEAWB_INFO_BIT_AEWIN_G   AASTAT_INFO_FLAG_AEWIN_G
#define SP5K_AEAWB_INFO_BIT_AEWIN_B   AASTAT_INFO_FLAG_AEWIN_B
#define SP5K_AEAWB_INFO_BIT_AWBWIN    AASTAT_INFO_FLAG_AWBWIN
#define SP5K_AEAWB_INFO_BIT_HIS_Y     AASTAT_INFO_FLAG_HIS_Y
#if SPCA5110
#define SP5K_AEAWB_INFO_BIT_AWB_RGB   AASTAT_INFO_FLAG_AWB_RGB
#define SP5K_AEAWB_INFO_BIT_HIS_R     AASTAT_INFO_FLAG_HIS_R
#define SP5K_AEAWB_INFO_BIT_HIS_G     AASTAT_INFO_FLAG_HIS_G
#define SP5K_AEAWB_INFO_BIT_HIS_B     AASTAT_INFO_FLAG_HIS_B
#else
#define SP5K_AEAWB_INFO_BIT_HIS_RGB   AASTAT_INFO_FLAG_HIS_RGB
#endif

UINT32 sp5kAeCustomCbSet(UINT32 infoflag, fpsp5kAeCallback_t pfunc);
typedef fpaaaEarlyCb_t fpsp5kAeEarlyCallback_t;
UINT32 sp5kAeCustomEarlyCbSet(fpsp5kAeEarlyCallback_t pfunc);
UINT32 sp5kAeCustomAewinSizeSet(UINT32 hgrid, UINT32 vgrid);

typedef AeCustomSts_t sp5kAeCustomSts_t ;
typedef fpAeStsCallback_t fpsp5kAeStsCallback_t ;
void sp5kAeStsCbSet( fpsp5kAeStsCallback_t  pfunc); 
void sp5kAeStsSet(UINT32 mode ,sp5kAeCustomSts_t  ae_sts);

typedef enum {
	SP5K_AE_LIGHT_MODE_NORMAL,
	SP5K_AE_LIGHT_MODE_DARK,
	SP5K_AE_LIGHT_MODE_HIGHLIGHT,
	SP5K_AE_LIGHT_MODE_STRONG_FRONTLIGHTING,
	SP5K_AE_LIGHT_MODE_BACK_LIGHT,
	SP5K_AE_LIGHT_MODE_STRONG_BACK_LIGHT,
	SP5K_AE_LIGHT_MODE_UNKNOWN,
	SP5K_AE_LIGHT_MODE_MAX,
} sp5kAeLightMode_t;

/* hist has 256 elements */
UINT32 sp5kAeLightModeGet(UINT32 ev, UINT32 *hist, sp5kAeLightMode_t *mode);

#ifdef __cplusplus
}
#endif

#endif /* _SP5K_AE_API_H_ */
