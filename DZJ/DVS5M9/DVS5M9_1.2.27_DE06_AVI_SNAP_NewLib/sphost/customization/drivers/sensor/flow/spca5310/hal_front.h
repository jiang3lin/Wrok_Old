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
 *  19, Innovation First Road, Science-Based Industrial Park,
 *  Hsin-Chu, Taiwan.
 *
 *  Author:
 *
 **************************************************************************/
#ifndef _HAL_FRONT_H_
#define _HAL_FRONT_H_

#ifdef __cplusplus
extern "C" {
#endif


/*
 * hal_front.c
 */
void   halFrontClkCfg(UINT32 mclkmode, UINT32 pclkmode);
void   halFrontClkSet(UINT32 tgpllen, UINT32 tgpllclkest, UINT32 mclkdiv, UINT32 pclkdiv);
void   halFrontClkPhaseCfg(UINT32 masterphase, UINT32 pixelphase, UINT32 adckphase);

#if SPCA5110
#undef halFrontTgPhaseCfg
#define halFrontTgPhaseCfg(fh1phase,fh2phase,rsgatephase,fcdsphase,fsphase)
#undef halFrontInputGate
#define halFrontInputGate(tggz)
#else
void   halFrontTgPhaseCfg(UINT32 fh1phase, UINT32 fh2phase, UINT32 rsgatephase, UINT32 fcdsphase, UINT32 fsphase);
void   halFrontInputGate(UINT32 tggz);
#endif

void   halFrontSiggenSet(UINT32 enable, UINT32 mode);
UINT32 halFrontSiggenGet(UINT32 *pmode);
void   halFrontSiggenSenTypeSet(UINT32 senType);
void   halFrontProbeSet(UINT32 enable, UINT32 mode);
void   halFrontDummyWr(UINT32 addr, UINT32 value);
UINT32 halFrontDummyRd(UINT32 addr);



#define TG_CMOS_SP5K_SLAVE      0
#define TG_CMOS_SP5K_MASTER     1
#define TG_CCD_EXT_SP5K_SLAVE   2
#define TG_CCD_EXT_SP5K_MASTER  3
#define TG_TVIN_DEC             8

#define TG_BMAP_SP5K_MASTER  0x01
#define TG_BMAP_CCD_EXT      0x02
#define TG_BMAP_TVIN_DEC     0x08

/*=================================================================*/
UINT32 halFrontImgSrcCfgSet(UINT32 cfgSel, UINT32 Value);

typedef enum {
	FRONT_IMG_SRC_SET = 0x00000000,
	FRONT_IMG_SIGGEN_CFG, /* revoked */
	FRONT_IMG_SENSOR_BIT_WIDTH,
	FRONT_IMG_SENSOR_CAP_FIELD,
	FRONT_IMG_SENSOR_FIELD_SEL, /*sync bitmask 0x10*/
	FRONT_IMG_TV_CFG,
	FRONT_IMG_DIFF_GRAY_CODE_CFG,
	FRONT_IMG_SRC_IN_PCLK_EDGE,
} halFrontImgSrcCfgSetSel_t;

#define FRONT_IMG_SENSOR_FIELD_SEL_UPDATE_SYNC 0x10

typedef enum {
	FRONT_IMG_SRC_SENSOR = 0, /* sensor with normal AFE */
	FRONT_IMG_SRC_SENSOR_GRAY,
	FRONT_IMG_SRC_SENSOR_DIFF,
	FRONT_IMG_SRC_SENSOR_DIFF_GRAY,
	FRONT_IMG_SRC_TV = 0x08,
} frontSensorSrc_t;

#if SPCA5110
typedef enum {
	FRONT_IMG_SIGGEN_BAYER_GR = 0,
	FRONT_IMG_SIGGEN_BAYER_R  = 1,
	FRONT_IMG_SIGGEN_BAYER_B  = 2,
	FRONT_IMG_SIGGEN_BAYER_GB = 3,
	FRONT_IMG_SIGGEN_SONY     = 4,
	FRONT_IMG_SIGGEN_SHARP    = 4,
	FRONT_IMG_SIGGEN_PANA     = 6,
} frontSiggenSentype_t;
#else
typedef enum {
	FRONT_IMG_SIGGEN_NORM = 0,
	FRONT_IMG_SIGGEN_SONY,
	FRONT_IMG_SIGGEN_SHARP,
	FRONT_IMG_SIGGEN_PANA,
	FRONT_IMG_SIGGEN_FFM, /*5110 not support*/
	FRONT_IMG_SIGGEN_YUV
} frontSiggenSentype_t;
#endif

typedef enum {
	FRONT_IMG_SENSOR_BIT_14,
	FRONT_IMG_SENSOR_BIT_10,
	FRONT_IMG_SENSOR_BIT_12,
} frontSensorBit_t;


/**
 * \param  val
 *     bit[1:0] uvsel: 0=Cb, 1=Y1, 2=Cr, 3=Y2;
 *     bit[2]   selffielden: to generate 601 mode field signal (5110 revoked)
 *     bit[3]   fieldpol:  invert field signal to CDSP; (5210 revoked)
 *     bit[4]   bt656en:   0=601 mode, 1=656 mode;
 *     bit[6:5] tvsyncen:  0=TV decoding v:hvalid, 3=SSG v:hvalid;
 *     bit[7]   fldsvden:  field signal transition sync to VD; (5110 revoked)
 */
enum {
	FRONT_TV_UVSEL_CB = 0,
	FRONT_TV_UVSEL_Y1,
	FRONT_TV_UVSEL_CR,
	FRONT_TV_UVSEL_Y2
};

#define FRONT_TV_601_FIELD_GEN   	4
#define FRONT_TV_FIELD_INV        	8
#define FRONT_TV_601               	0
#define FRONT_TV_656               	16
#define FRONT_TV_HSYNC_MASTER    32
#define FRONT_TV_VSYNC_MASTER    64
#define FRONT_TV_SYNC_MASTER   	96
#define FRONT_TV_SYNC_SLAVE    	0
#define FRONT_TV_LEVEL_SHIFT_Y   0x100
#define FRONT_TV_LEVEL_SHIFT_U   0x200
#define FRONT_TV_LEVEL_SHIFT_V   0x400

enum {
	FRONT_IMG_DIFF_PIXEL_2 = 0,
	FRONT_IMG_DIFF_PIXEL_1,
	FRONT_IMG_DIFF_PIXEL_4,
	FRONT_IMG_DIFF_PIXEL_6
};

#define FRONT_IMG_DIFF_OB_REF_MASK 0x01F0


#ifdef __cplusplus
}
#endif

#endif  /* _HAL_FRONT_H_ */
