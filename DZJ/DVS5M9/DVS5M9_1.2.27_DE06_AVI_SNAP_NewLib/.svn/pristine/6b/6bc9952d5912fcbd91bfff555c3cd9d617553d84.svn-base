/**************************************************************************
 *
 *        Copyright (c) 2005-2008 by Sunplus mMedia Inc., Ltd.
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
 *  Hsin-Chu, Taiwan.
 *
 *  Author: Anmin Deng
 *
 **************************************************************************/
#ifndef _SP5K_AF_API_H_
#define _SP5K_AF_API_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"
#include "middleware/aastat_def.h"
#include "middleware/aaa_def.h"


/**< ======= UINT32 sp5kAfModeSet(UINT32 afmode) =========================*/
UINT32 sp5kAfModeSet(UINT32 afmode);
/**< afmode */
typedef enum {
	SP5K_AF_MODE_OFF            = AF_MODE_OFF,
	SP5K_AF_MODE_ON             = AF_MODE_ON,
	SP5K_AF_MODE_AFWIN          = AF_MODE_AFWIN,
#if SPCA5110 /*saf*/
	SP5K_AF_SINGLE_ON           = AF_MODE_SINGLE_AF,
	SP5K_AF_STAT_AND_SINGLE_ON  = AF_MODE_AFWIN_AND_SINGLE_AF,
	SP5K_AF_VERT_ON				= AF_MODE_VERTICAL_AF,
	SP5K_AF_STAT_AND_VERT_ON	= AF_MODE_AFWIN_AND_VERTICAL_AF,
	SP5K_AF_SINGLE_AND_VERT_ON	= AF_MODE_SINGLE_AND_VERTICAL_AF,
	SP5K_AF_STAT_AND_SINGLE_AND_VERT_ON	= AF_MODE_AFWIN_AND_SINGLE_AND_VERTICAL_AF, /* weird */
#endif
	SP5K_AF_MODE_TOTAL,
} sp5kAfMode_t;

/**
 * \limit user change afwin
 * . delayed during modesw/dzoom (so if user wants to change both dzoom/mode
 *   and afwin, it is advised to change afwin first to have faster response);
 *   --> delayed by system internal blocking
 * . after af running, too frequent afwin size change requests
 *   where previous user change afwin is overwritten if not updated yet
 */
/**< = UINT32 sp5kAfWinSizeSet(const sp5kAfWinSize_t *pwin) ==============*/
/**< setting single AF win size if pwin->h/vwincnt == 0 */
typedef cdspAfwinSize_t sp5kAfWinSize_t;
UINT32 sp5kAfWinSizeSet(const sp5kAfWinSize_t *pwin);

/**< == UINT32 sp5kAfCfgSet(UINT32 selector, UINT32 value) ===============*/
UINT32 sp5kAfCfgSet(UINT32 selector, UINT32 value);
/**< selector */
typedef enum {
	SP5K_AF_WIN_POS_TEST = 2, /*value 0:off; 1:afwin; 3: single af*/
	SP5K_AF_NOISE_THR,
	SP5K_AF_GAIN,
	SP5K_AF_CORING_THR,
	SP5K_AF_NOISE_CORING = SP5K_AF_CORING_THR,
#if SPCA5110
	SP5K_AF_STAT_MODE, /*value is sp5kAfStatMode_t*/
#endif
	SP5K_AF_CFG_GAMMA_TYPE,
	SP5K_AF_CFG_FILTER_WEIGHT,
	SP5K_AF_CORING_VALUE,
#if SPCA5210
	SP5K_AF_SINGLE_MODE, /*value 0:high precision; (default)1:normal SAF*/
	SP5K_AF_SINGLE_MEDIAN, /* 0:off, 1:median3, 2,3:media5 */
	SP5K_AF_SINGLE_LPF, /* b0-3=LPF1, b4-7=LPF2 0:off, 1:LPF3, 2,3:LPF5 */
	SP5K_AF_SINGLE_HPF, /* 0:off, 1:HPF4, 2,3:HPF5 */
#endif
	SP5K_AF_WIN_PATH,
	SP5K_AF_CFG_TOTAL,
} sp5kAfCfgSel_t;

/**< == UINT32 sp5kAfCfgSet(SP5K_AF_WIN_PATH, value) ===============*/
typedef enum {
	SP5K_AF_WIN_PATH_PREPROCESS = 0,
	SP5K_AF_WIN_PATH_AFTER_WBGAIN,
	SP5K_AF_WIN_PATH_2ND_PATH,
} sp5kAfWinPath_t;


#if SPCA5110
/**< value when selector == SP5K_AF_STAT_MODE */
typedef enum {
	SP5K_AF_STAT_R =          AF_STAT_R,
	SP5K_AF_STAT_G =          AF_STAT_G,
	SP5K_AF_STAT_B =          AF_STAT_B,
	SP5K_AF_STAT_RGB_SUM =    AF_STAT_RGB_SUM, /*default*/
	SP5K_AF_STAT_R_SQR =      AF_STAT_R_SQR,
	SP5K_AF_STAT_G_SQR =      AF_STAT_G_SQR,
	SP5K_AF_STAT_B_SQR =      AF_STAT_B_SQR,
	SP5K_AF_STAT_RGB_SQRSUM = AF_STAT_RGB_SQRSUM,
	SP5K_AF_STAT_TOTAL,
} sp5kAfStatMode_t;
#endif

/**< = SINT32 sp5kAfWinGet(UINT32 param,UINT32 selector, UINT32 *pafVal) =*/
SINT32 sp5kAfWinGet(UINT32 param,UINT32 selector, UINT32 *pafVal);

/**< selector */
typedef enum {
	SP5K_AF_R                = AF_VAL_R,
	SP5K_AF_G                = AF_VAL_G,
	SP5K_AF_B                = AF_VAL_B,
	SP5K_AF_NOISE            = AF_VAL_NOISE,
	SP5K_AF_NOISE_CNT        = AF_VAL_NOISE_CNT,
	SP5K_AF_DC_VAL           = AF_VAL_DC,
#if SPCA5110
	SP5K_AF_STAT_VAL         = AF_VAL_STAT_WIN,
	SP5K_AF_SINGLE_VAL       = AF_VAL_SINGLE_AF,
	SP5K_AF_SINGLE_NOISE_CNT = AF_VAL_SINGLE_AF_NOISE_CNT,
	SP5K_AF_SINGLE_DC        = AF_VAL_SINGLE_AF_DC,
	SP5K_AF_VERT_VAL         = AF_VAL_VERT_AF,
	SP5K_AF_VERT_NOISE_CNT   = AF_VAL_VERT_AF_NOISE_CNT,
	SP5K_AF_SINGLE_HI_PREC_VAL = AF_VAL_SINGLE_AF_HI_PREC,
	SP5K_AF_STAT_VAL64       = AF_VAL64_STAT_WIN,
	SP5K_AF_SINGLE_VAL64     = AF_VAL64_SINGLE_AF,
	SP5K_AF_VERT_VAL64       = AF_VAL64_VERT_AF,
#endif
	SP5K_AF_VAL_SEL_TOTAL,
} sp5kAfValueSel_t;

/** \brief to get which of the afwin/saf/vaf values are available and
 * the win cnt for afwin pafVal size the user should prepare in sp5kAfWinGet()
 */
SINT32 sp5kAfWinGridGet(UINT32 param,UINT32 *afmode,UINT32 *hcnt,UINT32 *vcnt);


/**< ===== UINT32 sp5kAfCustomCbSet(fpsp5kAfCallback_t pfunc) ============*/
typedef fpafProc_t fpsp5kAfCallback_t;
UINT32 sp5kAfCustomCbSet(fpsp5kAfCallback_t pfunc);


#ifdef __cplusplus
}
#endif

#endif
