/**************************************************************************
 *
 *       Copyright (c) 2007-2012 by iCatch Technology, Inc.
 *
 *  This software is copyrighted by and is the property of iCatch
 *  Technology, Inc.. All rights are reserved by iCatch Technology, Inc..
 *  This software may only be used in accordance with the corresponding
 *  license agreement. Any unauthorized use, duplication, distribution,
 *  or disclosure of this software is expressly forbidden.
 *
 *  This Copyright notice MUST not be removed or modified without prior
 *  written consent of iCatch Technology, Inc..
 *
 *  iCatch Technology, Inc. reserves the right to modify this software
 *  without notice.
 *
 *  iCatch Technology, Inc.
 *  19-1, Innovation First Road, Science-Based Industrial Park,
 *  Hsin-Chu, Taiwan, R.O.C.
 *
 *  Author: Edward Kuo
 *
 **************************************************************************/
/* DO NOT EDIT the next 2 lines for auto revision by subversion
$Rev$
$Date::                     $
 */

/**************************************************************************
 *                         H E A D E R   F I L E S
 **************************************************************************/
#include <stdio.h>
#include "common.h"

/**************************************************************************
 *                           C O N S T A N T S
 **************************************************************************/
#if FPGA
#define IIS_SETTING_2  1  /* 0: channel 1, 1: channel 2. */
#else
#define IIS_SETTING_2  0  /* 0: channel 1, 1: channel 2. */
#endif

#if 1  /* dirty temp definition/declaration. */
typedef enum _halAudAttrMsk_e {
	/* bit[1:0]. */
	HAL_AUD_FILTER_PRIO_MSK    = ( 3 <<  0),  /**< (SPCA6330) For HAL_AUD_FILTER_REC_FIRST, HAL_AUD_FILTER_REC_PLAY, HAL_AUD_FILTER_REC_ONLY and HAL_AUD_FILTER_PLAY_ONLY. */
	/* bit[3:2]. */
	HAL_AUD_I2S_BCLK_INV_MSK   = ( 1 <<  2),  /**< (SPCA6330) For HAL_AUD_I2S_BCLK_INV_DIS and HAL_AUD_I2S_BCLK_INV_EN. */
	HAL_AUD_REC_LOOKBACK_MSK   = ( 1 <<  3),  /**< (SPCA6330) For HAL_AUD_REC_LOOKBACK_DIS and HAL_AUD_REC_LOOKBACK_EN. */
	/* bit[4]. */
	HAL_AUD_PLAY_LOOKBACK_MSK  = ( 1 <<  4),  /**< For HAL_AUD_PLAY_LOOKBACK_DIS and HAL_AUD_PLAY_LOOKBACK_EN. */
	/* bit[10:5]. */
	HAL_AUD_PLAY_PATH_MSK      = (17 <<  5),  /**< For HAL_AUD_PLAY_PATH_NONE, HAL_AUD_PLAY_PATH_EXTERNAL, HAL_AUD_PLAY_PATH_INTERNAL and HAL_AUD_PLAY_PATH_BOTH. */
	HAL_AUD_REC_PATH_MSK       = (17 <<  6),  /**< For HAL_AUD_REC_PATH_NONE, HAL_AUD_REC_PATH_EXTERNAL, HAL_AUD_REC_PATH_INTERNAL and HAL_AUD_REC_PATH_BOTH. */
	HAL_AUD_I2S_MODE_MSK       = ( 1 <<  7),  /**< For HAL_AUD_I2S_MASTER and HAL_AUD_I2S_SLAVE. */
	HAL_AUD_REC_DUP_L_TO_R_MSK = ( 1 <<  8),  /**< For HAL_AUD_REC_DUP_L_TO_R_DIS and HAL_AUD_REC_DUP_L_TO_R_EN. */
	/* bit[13:11]. */
	HAL_AUD_INT_MODE_MSK       = ( 1 << 11),  /**< For HAL_AUD_INT_LEVEL_TRIG and HAL_AUD_INT_EDGE_TRIG. */
	HAL_AUD_OIS_INT_MODE_MSK   = ( 1 << 12),  /**< For HAL_AUD_OIS_INT_LEVEL_TRIG and HAL_AUD_OIS_INT_EDGE_TRIG. */
	HAL_AUD_DRAM_MODE_MSK      = ( 1 << 13),  /**< For HAL_AUD_DRAM_AUTO_STOP and HAL_AUD_DRAM_ENDLESS_PLAY. */
	/* bit[19:18, 14]. */
	HAL_AUD_ENERGY_CALC_MSK    = ( 1 << 14),  /**< For HAL_AUD_ENERGY_CALC_BEF_DGAIN and HAL_AUD_ENERGY_CALC_AFT_DGAIN. */
	HAL_AUD_AGC_SRC_MSK        = ( 3 << 18),  /**< For HAL_AUD_AGC_SRC_FW, HAL_AUD_AGC_SRC_HW_L_CH, HAL_AUD_AGC_SRC_HW_R_CH and HAL_AUD_AGC_SRC_HW_INDEPEND. */
	/* bit[17:15]. */
	HAL_AUD_I2S_MSB_LSB_MSK    = ( 1 << 15),  /**< For HAL_AUD_I2S_MSB_FIRST and HAL_AUD_I2S_LSB_FIRST. */
	HAL_AUD_REC_LR_MSK         = ( 1 << 16),  /**< For HAL_AUD_REC_LR_NOSWAP and HAL_AUD_REC_LR_SWAP. */
	HAL_AUD_PLAY_LR_MSK        = ( 1 << 17),  /**< For HAL_AUD_PLAY_LR_NOSWAP and HAL_AUD_PLAY_LR_SWAP. */
	/* bit[20]. */
	HAL_AUD_I2S_REC_SHF_MSK    = ( 1 << 20),  /**< For HAL_AUD_I2S_REC_SHF_0 and HAL_AUD_I2S_REC_SHF_1. */
	/* bit[21]. */
	HAL_AUD_DAC_AUTO_MUTE_MSK  = ( 1 << 21),  /**< For HAL_AUD_DAC_AUTO_MUTE_DIS and HAL_AUD_DAC_AUTO_MUTE_EN. */
} halAudAttrMsk_e;

typedef enum _halAudAttr_e {
	/* bit[1:0]. */
	HAL_AUD_FILTER_REC_FIRST      = ( 0 <<  0),
	HAL_AUD_FILTER_REC_PLAY       = ( 1 <<  0),
	HAL_AUD_FILTER_REC_ONLY       = ( 2 <<  0),
	HAL_AUD_FILTER_PLAY_ONLY      = ( 3 <<  0),
	/* bit[3:2]. */
	HAL_AUD_I2S_BCLK_INV_DIS      = ( 0 <<  2),
	HAL_AUD_I2S_BCLK_INV_EN       = ( 1 <<  2),
	HAL_AUD_REC_LOOKBACK_DIS      = ( 0 <<  3),
	HAL_AUD_REC_LOOKBACK_EN       = ( 1 <<  3),
	/* bit[4]. */
	HAL_AUD_PLAY_LOOKBACK_DIS     = ( 0 <<  4),
	HAL_AUD_PLAY_LOOKBACK_EN      = ( 1 <<  4),
	/* bit[10:5]. */
	HAL_AUD_PLAY_PATH_NONE        = ( 0 <<  5),
	HAL_AUD_PLAY_PATH_EXTERNAL    = ( 1 <<  5),
	HAL_AUD_PLAY_PATH_INTERNAL    = (16 <<  5),
	HAL_AUD_PLAY_PATH_BOTH        = (17 <<  5),
	HAL_AUD_REC_PATH_NONE         = ( 0 <<  6),
	HAL_AUD_REC_PATH_EXTERNAL     = ( 1 <<  6),
	HAL_AUD_REC_PATH_INTERNAL     = (16 <<  6),
	HAL_AUD_REC_PATH_BOTH         = (17 <<  6),
	HAL_AUD_I2S_MASTER            = ( 0 <<  7),
	HAL_AUD_I2S_SLAVE             = ( 1 <<  7),
	HAL_AUD_REC_DUP_L_TO_R_DIS    = ( 0 <<  8),
	HAL_AUD_REC_DUP_L_TO_R_EN     = ( 1 <<  8),
	/* bit[13:11]. */
	HAL_AUD_INT_LEVEL_TRIG        = ( 0 << 11),
	HAL_AUD_INT_EDGE_TRIG         = ( 1 << 11),
	HAL_AUD_OIS_INT_LEVEL_TRIG    = ( 0 << 12),
	HAL_AUD_OIS_INT_EDGE_TRIG     = ( 1 << 12),
	HAL_AUD_DRAM_AUTO_STOP        = ( 0 << 13),
	HAL_AUD_DRAM_ENDLESS_PLAY     = ( 1 << 13),
	/* bit[19:18, 14]. */
	HAL_AUD_ENERGY_CALC_BEF_DGAIN = ( 0 << 14),
	HAL_AUD_ENERGY_CALC_AFT_DGAIN = ( 1 << 14),
	HAL_AUD_AGC_SRC_FW            = ( 0 << 18),
	HAL_AUD_AGC_SRC_HW_L_CH       = ( 1 << 18),
	HAL_AUD_AGC_SRC_HW_R_CH       = ( 2 << 18),
	HAL_AUD_AGC_SRC_HW_INDEPEND   = ( 3 << 18),
	/* bit[17:15]. */
	HAL_AUD_I2S_MSB_FIRST         = ( 0 << 15),
	HAL_AUD_I2S_LSB_FIRST         = ( 1 << 15),
	HAL_AUD_REC_LR_NOSWAP         = ( 0 << 16),
	HAL_AUD_REC_LR_SWAP           = ( 1 << 16),
	HAL_AUD_PLAY_LR_NOSWAP        = ( 0 << 17),
	HAL_AUD_PLAY_LR_SWAP          = ( 1 << 17),
	/* bit[20]. */
	HAL_AUD_I2S_REC_SHF_0         = ( 0 << 20),
	HAL_AUD_I2S_REC_SHF_1         = ( 1 << 20),
	/* bit[21]. */
	HAL_AUD_DAC_AUTO_MUTE_DIS     = ( 0 << 21),
	HAL_AUD_DAC_AUTO_MUTE_EN      = ( 1 << 21),
} halAudAttr_e;

typedef enum _audInputSel_e {
	AUD_IN_NONE = 0,
	AUD_IN_LINE = 1,
	AUD_IN_MIC  = 2,
} audInputSel_e;

typedef enum _audOutputSel_e {
	AUD_OUT_NONE      = 0,
	AUD_OUT_SPEAKER   = 1,
	AUD_OUT_HEADPHONE = 2,
	AUD_OUT_LINEOUT   = 3,
} audOutputSel_e;
#endif

/**************************************************************************
 *                           D A T A   T Y P E S
 **************************************************************************/
#if 1  /* dirty temp definition/declaration. */
typedef struct audCodecItf_s {
	void (*pf_switch[2])(void);
	void (*pf_init[2])(void);
	void (*pf_vol_set[2])(UINT32, UINT32);
	void (*pf_vol_get[2])(UINT32, UINT32 *);
	void (*pf_output_set)(UINT32);
	void (*pf_output_get)(UINT32 *);
	void (*pf_input_set)(UINT32);
	void (*pf_input_get)(UINT32 *);
} audCodecItf_t;
#endif

/**************************************************************************
 *                              M A C R O S
 **************************************************************************/
#if 0
#define DBG_PRINT(fmt, args...)  printf(fmt, ##args)
#else
#define DBG_PRINT(fmt, args...)
#endif

#if 1  /* dirty temp definition/declaration. */
#define READ8(_reg_)            (*((volatile UINT8  *)(_reg_)))
#define READ16(_reg_)           (*((volatile UINT16 *)(_reg_)))
#define WRITE8(_reg_, _value_)  (*((volatile UINT8  *)(_reg_)) = (_value_))
#define WRITE16(_reg_, _value_) (*((volatile UINT16 *)(_reg_)) = (_value_))
#endif

/**************************************************************************
 *                         G L O B A L    D A T A
 **************************************************************************/
static UINT32 recVol, playVol;
static UINT32 outputDev, inputDev;

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S
 **************************************************************************/
static void ws8751RecInit(void);
static void ws8751PlayInit(void);
static void ws8751RecVolumeSet(UINT32,UINT32);
static void ws8751RecVolumeGet(UINT32,UINT32 *);
static void ws8751PlayVolumeSet(UINT32,UINT32);
static void ws8751PlayVolumeGet(UINT32,UINT32 *);
static void ws8751OutputSet(UINT32);
static void ws8751OutputGet(UINT32 *);
static void ws8751InputSet(UINT32);
static void ws8751InputGet(UINT32 *);

static void iicCodecRegSet(UINT32, UINT32);
static void halaudioSsc2Write(UINT8 *, UINT8 *, UINT8, UINT8, UINT8);
extern UINT32 halAudAttrSet(UINT32 chnlId,UINT32 msk,UINT32 val);
extern void halGenGpioFuncSel(UINT32 func,UINT32 msk);
extern void audCodecRegister(audCodecItf_t *pitfFuncs);


/**************************************************************************
 *
 *  Function Name: audCodecWs8751Register
 *
 *  Purposes:
 *
 *  Descriptions:
 *
 *  Arguments:
 *
 *  Returns:
 *
 *  See also:
 *
 **************************************************************************/ 
void
audCodecWs8751Register(
	void
)
{
	audCodecItf_t itf;

	DBG_PRINT("[%s]\n", __FUNCTION__);

	itf.pf_switch[0]  = NULL;
	itf.pf_switch[1]  = NULL;
	itf.pf_init[0]    = ws8751RecInit;
	itf.pf_init[1]    = ws8751PlayInit;
	itf.pf_vol_set[0] = ws8751RecVolumeSet;
	itf.pf_vol_get[0] = ws8751RecVolumeGet;
	itf.pf_vol_set[1] = ws8751PlayVolumeSet;
	itf.pf_vol_get[1] = ws8751PlayVolumeGet;
	itf.pf_output_set = ws8751OutputSet;
	itf.pf_output_get = ws8751OutputGet;
	itf.pf_input_set  = ws8751InputSet;
	itf.pf_input_get  = ws8751InputGet;

	WRITE8(0xb00000b0, 0x1f);
#if IIS_SETTING_2
	WRITE16(0xb0009034, READ16(0xb0009034) | 0x3000);  /* tggpio[13:12] map to gpio. */
	WRITE16(0xb0009044, READ16(0xb0009044) | 0x3000);
	WRITE16(0xb0009040, READ16(0xb0009040) & 0xCFFF);
	WRITE8(0xb000925f, READ8(0xb000925f) | 0x03);  /* lmigpi[9:8] map to sif2sdo/sif2sck. */
#else
	WRITE16(0xb0009034, READ16(0xb0009034) & 0xCFFF);  /* tggpio[13:12] map to sif2sdo/sif2sck. */
#endif

	audCodecRegister(&itf);
	ws8751RecVolumeSet(0,16);
	ws8751PlayVolumeSet(0,16);
}

/**************************************************************************
 *
 *  Function Name: ws8751RecInit
 *
 *  Purposes:
 *
 *  Descriptions:
 *
 *  Arguments:
 *
 *  Returns:
 *
 *  See also:
 *
 **************************************************************************/
static void
ws8751RecInit(
	void
)
{
	DBG_PRINT("[%s]\n", __FUNCTION__);

	iicCodecRegSet(0x1e, 0x00); /* register 0Fh reset */
	iicCodecRegSet(0x10, 0x00); /* register 08h*/
	iicCodecRegSet(0x0e, 0x42); /* register 07h_16bit_IIS */
	iicCodecRegSet(0x01, 0x17); /* register 0h_ADC unmute */
	iicCodecRegSet(0x03, 0x17); /* register 1h_ADC_unmute */
	iicCodecRegSet(0x31, 0x04); /* register 18h */
	iicCodecRegSet(0x33, 0xfe); /* register 19h */
	iicCodecRegSet(0x40, 0x30); /* register 20h */
	iicCodecRegSet(0x42, 0x30); /* register 21h */
	iicCodecRegSet(0x2b, 0xf0); /* register 15h: adc digital gain */
	iicCodecRegSet(0x2d, 0xf0); /* register 16h: adc digital gain */

	halAudAttrSet(0, HAL_AUD_REC_PATH_MSK, HAL_AUD_REC_PATH_EXTERNAL);
	halGenGpioFuncSel(0x0000, 0x3D00);
	#if 1 /* for 1628 EVB. */
	halAudAttrSet(0, HAL_AUD_I2S_REC_SHF_MSK | HAL_AUD_I2S_BCLK_INV_MSK | HAL_AUD_I2S_MODE_MSK,
	              HAL_AUD_I2S_REC_SHF_0 | HAL_AUD_I2S_BCLK_INV_EN | HAL_AUD_I2S_SLAVE);
	#else
	halAudAttrSet(0, HAL_AUD_I2S_MODE_MSK, HAL_AUD_I2S_SLAVE);
	#endif
	#if FPGA
	halAudCfgSet(0, HAL_AUD_I2S_REC_BCLK_PHASE, 15);
	#else
	#if 0
	halAudCfgSet(0, HAL_AUD_I2S_REC_BCLK_PHASE, 0);
	halAudAttrSet(0, HAL_AUD_I2S_BCLK_INV_MSK, HAL_AUD_I2S_BCLK_INV_EN);
	#endif
	#endif
}

/**************************************************************************
 *
 *  Function Name: ws8751PlayInit
 *
 *  Purposes:
 *
 *  Descriptions:
 *
 *  Arguments:
 *
 *  Returns:
 *
 *  See also:
 *
 **************************************************************************/
static void
ws8751PlayInit(
	void
)
{
	DBG_PRINT("[%s]\n", __FUNCTION__);

	iicCodecRegSet(0x1e, 0x00); /* register 0Fh reset */
	iicCodecRegSet(0x10, 0x00); /* register 08h       */
	iicCodecRegSet(0x2e, 0x00); /* register 17h       */
	iicCodecRegSet(0x0a, 0x00); /* register 05h       */
	iicCodecRegSet(0x0e, 0x02); /* register 07h       */

	iicCodecRegSet(0x33, 0xf0); /* register 19h       */
	iicCodecRegSet(0x35, 0xf0); /* register 1ah       */
	iicCodecRegSet(0x45, 0x00); /* register 22h       */
	iicCodecRegSet(0x4b, 0x00); /* register 25h       */

	iicCodecRegSet(0x04, 0x7f); /* register 02h       */
	iicCodecRegSet(0x07, 0x7f); /* register 03h       */

	halAudAttrSet(0, HAL_AUD_PLAY_PATH_MSK, HAL_AUD_PLAY_PATH_EXTERNAL);
	halGenGpioFuncSel(0x0000, 0x3D00);
	halAudAttrSet(0, HAL_AUD_I2S_MODE_MSK, HAL_AUD_I2S_MASTER);
}

/**************************************************************************
 *
 *  Function Name: ws8751RecVolumeSet
 *
 *  Purposes:
 *
 *  Descriptions:
 *
 *  Arguments:
 *
 *  Returns:
 *
 *  See also:
 *
 **************************************************************************/
static void
ws8751RecVolumeSet(
	UINT32 chnlId,
	UINT32 volume
)
{
	DBG_PRINT("[%s] vol:%d\n", __FUNCTION__, volume);

	recVol = volume & 0x1F;

	volume = (volume * 2) & 0x3F;

	iicCodecRegSet(0x01, volume); /* 0h */
	iicCodecRegSet(0x03, volume); /* 1h */
}

/**************************************************************************
 *
 *  Function Name: ws8751RecVolumeGet
 *
 *  Purposes:
 *
 *  Descriptions:
 *
 *  Arguments:
 *
 *  Returns:
 *
 *  See also:
 *
 **************************************************************************/
void
ws8751RecVolumeGet(
	UINT32 chnlId,
	UINT32 *volume
)
{
	*volume = recVol;

	DBG_PRINT("[%s] vol:%d\n", __FUNCTION__, *volume);
}

/**************************************************************************
 *
 *  Function Name: ws8751PlayVolumeSet
 *
 *  Purposes:
 *
 *  Descriptions:
 *
 *  Arguments:
 *
 *  Returns:
 *
 *  See also:
 *
 **************************************************************************/
static void
ws8751PlayVolumeSet(
	UINT32 chnlId,
	UINT32 volume
)
{
	DBG_PRINT("[%s] vol:%d\n", __FUNCTION__, volume);

	volume &= 0x1F;
	playVol = volume;
	if (volume >= 16) {
		volume = 0x78 + ((volume - 16) >> 1);
	}
	else if (volume > 0) {
		volume = 0x78 - ((16 - volume) >> 1);
	}
	else {
		volume = 0;
	}

	iicCodecRegSet(0x05, volume); /* 02h */
	iicCodecRegSet(0x07, volume); /* 03h */
#if 0
	iicCodecRegSet(0x51, volume); /* 28h */
	iicCodecRegSet(0x53, volume); /* 29h */
	iicCodecRegSet(0x54, volume); /* 2Ah */
#endif
}

/**************************************************************************
 *
 *  Function Name: ws8751PlayVolumeGet
 *
 *  Purposes:
 *
 *  Descriptions:
 *
 *  Arguments:
 *
 *  Returns:
 *
 *  See also:
 *
 **************************************************************************/
void
ws8751PlayVolumeGet(
	UINT32 chnlId,
	UINT32 *volume
)
{
	*volume = playVol;

	DBG_PRINT("[%s] vol:%d\n", __FUNCTION__, *volume);
}

/**************************************************************************
 *
 *  Function Name: ws8751OutputSet
 *
 *  Purposes:
 *
 *  Descriptions:
 *
 *  Arguments:
 *
 *  Returns:
 *
 *  See also:
 *
 **************************************************************************/
static void
ws8751OutputSet(
	UINT32 selector
)
{
	DBG_PRINT("[%s] sel:%d\n", __FUNCTION__, selector);

	outputDev = selector;

	switch (selector) {
	case AUD_OUT_NONE:
		iicCodecRegSet(0x34, 0); /* 1Ah */
		break;
	case AUD_OUT_SPEAKER:
		iicCodecRegSet(0x35, 0x98); /* 1Ah */
		iicCodecRegSet(0x51, 0x79); /* 28h */
		iicCodecRegSet(0x53, 0x79); /* 29h */
		break;
	case AUD_OUT_HEADPHONE:
		iicCodecRegSet(0x35, 0xe0); /* 1Ah */
		iicCodecRegSet(0x05, 0x79); /* 02h */
		iicCodecRegSet(0x07, 0x79); /* 03h */
		break;
	case AUD_OUT_LINEOUT:
		iicCodecRegSet(0x35, 0x84); /* 1Ah*/
		iicCodecRegSet(0x4d, 0x50); /* 26h */
		iicCodecRegSet(0x4f, 0x50); /* 27h */
		break;
	}
}

/**************************************************************************
 *
 *  Function Name: ws8751OutputGet
 *
 *  Purposes:
 *
 *  Descriptions:
 *
 *  Arguments:
 *
 *  Returns:
 *
 *  See also:
 *
 **************************************************************************/
static void
ws8751OutputGet(
	UINT32 *selector
)
{
	*selector = outputDev;

	DBG_PRINT("[%s] sel:%d\n", __FUNCTION__, *selector);
}

/**************************************************************************
 *
 *  Function Name: ws8751InputSet
 *
 *  Purposes:
 *
 *  Descriptions:
 *
 *  Arguments:
 *
 *  Returns:
 *
 *  See also:
 *
 **************************************************************************/
static void
ws8751InputSet(
	UINT32 selector
)
{
	DBG_PRINT("[%s] sel:%d\n", __FUNCTION__, selector);

	inputDev = selector;

	if (selector == AUD_IN_NONE) {
		iicCodecRegSet(0x32, 0x00); /* 19h */
	}
	else {
		iicCodecRegSet(0x33, 0xfe); /* 19h */
	}
}

/**************************************************************************
 *
 *  Function Name: ws8751InputGet
 *
 *  Purposes:
 *
 *  Descriptions:
 *
 *  Arguments:
 *
 *  Returns:
 *
 *  See also:
 *
 **************************************************************************/
static void
ws8751InputGet(
	UINT32 *selector
)
{
	*selector = inputDev;

	DBG_PRINT("[%s] sel:%d\n", __FUNCTION__, *selector);
}

/**************************************************************************
 *
 *  Function Name: iicCodecRegSet
 *
 *  Purposes:
 *
 *  Descriptions:
 *
 *  Arguments:
 *
 *  Returns:
 *
 *  See also:
 *
 **************************************************************************/
static void
iicCodecRegSet(
	UINT32 reg,
	UINT32 value
)
{
	UINT8 regaddr[1];
	UINT8 regdata[1];
	UINT8 i2ccnt, i2cmode;

#if IIS_SETTING_2
	WRITE8(0xb0009240,0x34); /* slave addr */
	WRITE8(0xb0009230,0x02); /* i2c freguency */
#else
	WRITE8(0xb0009140,0x34); /* slave addr */
	WRITE8(0xb0009130,0x02); /* i2c freguency */
#endif
	regaddr[0] = reg;
	regdata[0] = value;

	i2ccnt = 0;
	i2cmode = 0;
	halaudioSsc2Write(regaddr, regdata, i2ccnt, i2cmode, 0);
}

/**************************************************************************
 *
 *  Function Name: halaudioSsc2Write
 *
 *  Purposes:
 *
 *  Descriptions:
 *
 *  Arguments:
 *
 *  Returns:
 *
 *  See also:
 *
 **************************************************************************/
static void
halaudioSsc2Write(
	UINT8 *pregaddr,
	UINT8 *pregdata,
	UINT8 cnt,
	UINT8 mode,
	UINT8 sync
)
{
#if IIS_SETTING_2
	UINT32 i, timeout;

	WRITE8(0xb0009200, mode); /* Select Sequential/Burst mode */
	WRITE8(0xb0009208, sync); /* Synchronize with Vd or Vvalid not */
	WRITE8(0xb000923C, cnt);

	if (mode) {
		for (i = 0; i < (cnt+1); i++) {
			if ((READ8(0xb0009238) & 0x30) == 0x30) {
				WRITE8(0xb0009210+i*4, pregaddr[i*2]);
				WRITE8(0xb0009210+i*4+2, pregaddr[i*2+1]);
			}
			else {
				WRITE8(0xb0009210+i*2, pregaddr[i]);
			}
			WRITE8(0xb0009210+i*2+1, pregdata[i]);
		}
	}
	else {
		WRITE8(0xb0009210, pregaddr[0]);
		WRITE8(0xb0009212, pregaddr[1]);
		for (i = 0; i < (cnt+1); i++) {
			WRITE8(0xb0009210+i*2+1, pregdata[i]);
		}
	}
	WRITE8(0xb0009204, 0x01);
	timeout = 0;
	do {
		i = READ8(0xb0009204);
		timeout++;
		if(timeout & 5000000){
			/*printf("timeOut\n");*/
		}
	} while (i == 0x01);
#else
	UINT32 i, timeout;

	WRITE8(0xb0009100, mode); /* Select Sequential/Burst mode */
	WRITE8(0xb0009108, sync); /* Synchronize with Vd or Vvalid not */
	WRITE8(0xb000913C, cnt);

	if (mode) {
		for (i = 0; i < (UINT32)(cnt+1); i++) {
			if ((READ8(0xb0009138) & 0x30) == 0x30) {
				WRITE8(0xb0009110+i*4, pregaddr[i*2]);
				WRITE8(0xb0009110+i*4+2, pregaddr[i*2+1]);
			}
			else {
				WRITE8(0xb0009110+i*2, pregaddr[i]);
			}
			WRITE8(0xb0009110+i*2+1, pregdata[i]);
		}
	}
	else {
		WRITE8(0xb0009110, pregaddr[0]);
		WRITE8(0xb0009112, pregaddr[1]);
		for (i = 0; i < (UINT32)(cnt+1); i++) {
			WRITE8(0xb0009110+i*2+1, pregdata[i]);
		}
	}
	WRITE8(0xb0009104, 0x01);
	timeout = 0;
	do {
		i = READ8(0xb0009104);
		timeout++;
		if(timeout & 100000){
			;/*printf("timeOut\n");*/
		}
	} while (i == 0x01);
#endif
}

