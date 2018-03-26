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
 *  Hsin-Chu, Taiwan                                                      *
 *                                                                        *
 *  Author: Anmin Deng                                                    *
 *                                                                        *
 **************************************************************************/
#ifndef _AASTAT_DEF_H_
#define _AASTAT_DEF_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"

#define AASTAT_DUAL_SENSOR_AE_EN 1

typedef enum {
	AASTAT_INFO_TYPE_AEWIN_Y,
	AASTAT_INFO_TYPE_AEWIN_RGB,
	AASTAT_INFO_TYPE_AEWIN_R = AASTAT_INFO_TYPE_AEWIN_RGB,
	AASTAT_INFO_TYPE_AEWIN_G = AASTAT_INFO_TYPE_AEWIN_RGB,
	AASTAT_INFO_TYPE_AEWIN_B = AASTAT_INFO_TYPE_AEWIN_RGB,
#if SPCA5110 /*5000:aewin; 5110:subsample*/
	AASTAT_INFO_TYPE_AWB_RGB = AASTAT_INFO_TYPE_AEWIN_RGB,
#endif
	AASTAT_INFO_TYPE_AEWIN_TOTAL,
	AASTAT_INFO_TYPE_AWBWIN = AASTAT_INFO_TYPE_AEWIN_TOTAL, /*5000 only*/
	AASTAT_INFO_TYPE_HIS_Y,
	AASTAT_INFO_TYPE_HIS_RGB, /*5000 only*/
#if SPCA5110
	AASTAT_INFO_TYPE_HIS_R, /*5110 only*/
	AASTAT_INFO_TYPE_HIS_G, /*5110 only*/
	AASTAT_INFO_TYPE_HIS_B, /*5110 only*/
#endif
	AASTAT_INFO_TYPE_TOTAL,
} aastat_info_type_t;

typedef enum {
	AASTAT_INFO_FLAG_AEWIN_Y = 1 << AASTAT_INFO_TYPE_AEWIN_Y,
	AASTAT_INFO_FLAG_AEWIN_RGB = 1 << AASTAT_INFO_TYPE_AEWIN_RGB,
	AASTAT_INFO_FLAG_AEWIN_R = AASTAT_INFO_FLAG_AEWIN_RGB,
	AASTAT_INFO_FLAG_AEWIN_G = AASTAT_INFO_FLAG_AEWIN_RGB,
	AASTAT_INFO_FLAG_AEWIN_B = AASTAT_INFO_FLAG_AEWIN_RGB,
	AASTAT_INFO_FLAG_AWBWIN = 1 << AASTAT_INFO_TYPE_AWBWIN,
	AASTAT_INFO_FLAG_HIS_Y = 1 << AASTAT_INFO_TYPE_HIS_Y,
	AASTAT_INFO_FLAG_HIS_RGB = 1 << AASTAT_INFO_TYPE_HIS_RGB,
#if SPCA5110
	AASTAT_INFO_FLAG_AEWIN_YRGB = AASTAT_INFO_FLAG_AEWIN_Y,
	AASTAT_INFO_FLAG_AWB_RGB = 1 << AASTAT_INFO_TYPE_AWB_RGB,
	AASTAT_INFO_FLAG_CDSP_AE = AASTAT_INFO_FLAG_AEWIN_Y,
	AASTAT_INFO_FLAG_CDSP_AWB = AASTAT_INFO_FLAG_AWB_RGB,
	AASTAT_INFO_FLAG_HIS_R = 1 << AASTAT_INFO_TYPE_HIS_R,
	AASTAT_INFO_FLAG_HIS_G = 1 << AASTAT_INFO_TYPE_HIS_G,
	AASTAT_INFO_FLAG_HIS_B = 1 << AASTAT_INFO_TYPE_HIS_B,
	AASTAT_INFO_FLAG_HIS_ALL =
		AASTAT_INFO_FLAG_HIS_Y | AASTAT_INFO_FLAG_HIS_R |
		AASTAT_INFO_FLAG_HIS_G | AASTAT_INFO_FLAG_HIS_B,
#else
	AASTAT_INFO_FLAG_AEWIN_YRGB =
		AASTAT_INFO_FLAG_AEWIN_Y | AASTAT_INFO_FLAG_AEWIN_RGB,
	AASTAT_INFO_FLAG_CDSP_AE = AASTAT_INFO_FLAG_AEWIN_YRGB,
	AASTAT_INFO_FLAG_CDSP_AWB = AASTAT_INFO_FLAG_AWBWIN,
	AASTAT_INFO_FLAG_HIS_ALL =
		AASTAT_INFO_FLAG_HIS_Y | AASTAT_INFO_FLAG_HIS_RGB,
#endif
	AASTAT_INFO_FLAG_ALL = (1 << AASTAT_INFO_TYPE_TOTAL) - 1,
} aastat_info_flag_t;

typedef struct {
	SINT16 expIdx; /*sensor_private.h*/
	UINT8  agcIdx;
	UINT8  aperIdx;
	SINT8  tverror;
	UINT8  tverr_unit;
	SINT8  sverror;
	UINT8  sverr_unit;
#if AASTAT_DUAL_SENSOR_AE_EN	
	SINT16 exp2Idx; /*sensor_private.h*/
	UINT8  agc2Idx;
	UINT8  aper2Idx;
	SINT8  tv2error;
	SINT8  sv2error;
	UINT8  dual_ae_en;
#endif
} aeEvInfo_t;

typedef struct {
	UINT16 rGain;
	UINT16 grGain;
	UINT16 bGain;
	UINT16 gbGain;
} afeWBGain_t;
typedef struct {
	UINT16 rGain;
	UINT16 grGain;
	UINT16 bGain;
	UINT16 gbGain;
} cdspPreWBGain_t;
typedef struct {
	UINT16 rGain;
	UINT16 gGain;
	UINT16 bGain;
} cdspPosWBGain_t;
typedef struct {
	afeWBGain_t afeWBGain;
	cdspPreWBGain_t preWBGain;
	cdspPosWBGain_t posWBGain;
	cdspPosWBGain_t posWB2Gain;
} aaa_wb_info_t;
typedef struct {
	UINT16 rGain;
	UINT16 grGain;
	UINT16 bGain;
	UINT16 gbGain;
} aaa_wb_result_t;

/*******>>>>CDSP WORKAROUND*/

/***TODO****!!!*/
typedef struct {
	UINT16 rvalue;
	UINT16 grvalue;
	UINT16 bvalue;
	UINT16 gbvalue;
	UINT32 sumcnt;
} cdspAWBWinResult_t;

typedef struct {
	UINT16 rhithr          ;
	UINT16 rlowthr         ;
	UINT16 ghithr          ;
	UINT16 glowthr         ;
	UINT16 bhithr          ;
	UINT16 blowthr         ;
} cdspRGBHisThr_t;
typedef struct {
	UINT32 rhicnt          ;
	UINT32 rlowcnt         ;
	UINT32 ghicnt          ;
	UINT32 glowcnt         ;
	UINT32 bhicnt          ;
	UINT32 blowcnt         ;
} cdspRGBHisResult_t;
typedef struct {
	UINT8  hwincnt         ;
	UINT8  vwincnt         ;
	UINT16 hoffset         ;
	UINT16 voffset         ;
	UINT16 hsize           ;
	UINT16 vsize           ;
	UINT16 hintvl          ;
	UINT16 vintvl          ;
} cdspAfwinSize_t;

/*******<<<<CDSP WORKAROUND*/


typedef enum {
	AAA_PROC_INFO_AEAWB_VALID = 1,
	AAA_PROC_INFO_PREFLASH_VALID = 0x80,
} aaaProcInfoValid_t; /*bitwise mask*/
typedef enum {
	AAA_INFO_RESULT_EV_STEP_VALID = 1,
	AAA_INFO_RESULT_AE_VALID = AAA_INFO_RESULT_EV_STEP_VALID,
	AAA_INFO_RESULT_LUMA_VALID = 2,
	AAA_INFO_RESULT_RGB_VALID = 4,
	AAA_INFO_RESULT_AWB_VALID = 8,
} aastatInfoResultValid_t; /*bitwise mask*/

typedef struct {
	UINT32 frame_id;
	UINT8  valid; /*aaaProcInfoValid_t*/
	aeEvInfo_t evInfo;
	aaa_wb_info_t wbinfo;
	UINT8  aewin_hgrid;
	UINT8  aewin_vgrid;
	UINT16 *paewin_y;
	UINT16 *paewin_r; /****5110*pawb_r*/
	UINT16 *paewin_g; /****5110*pawb_gr*/
	UINT16 *paewin_b; /****5110*pawb_b*/
	UINT32 aewin_pixelcnt;
	cdspAWBWinResult_t *prgbvalue;
	UINT32 *pyhis;
	cdspRGBHisResult_t *prgbhis;
#if SPCA5110 /****TODO*5110*rgbhis*/
	UINT8  awb_hgrid;
	UINT8  awb_vgrid;
	UINT8  awb_bayer_fmt; /*0:GRBG, 1:RGGB, 2:BGGR, 3:GBRG*/
	UINT16 *pawb_gb;
	UINT32 *prhis;
	UINT32 *pghis;
	UINT32 *pbhis;
	UINT16 *pawb_bayer;
#endif
} aaaProcInfo_t;

typedef enum {
	AWB_CONV_STATUS_NULL = 0,
	AWB_CONV_STATUS_CONVERGED = 1,
	AWB_CONV_STATUS_OFF_LIMIT = 2,
} awbConvStatus_t; /*bitwise mask*/

typedef struct {
	UINT16 rValue;
	UINT16 grValue;
	UINT16 bValue;
	UINT16 gbValue;
} aaa_wb_rgbvalue_t;
typedef struct {
	UINT8 valid; /*aastatInfoResultValid_t*/
	UINT8 awbConvStatus; /*awbConvStatus_t*/
	SINT8 aeStep;
	SINT8 aeDevStep;
	UINT16 aeLuma;
	aaa_wb_result_t wb_result;
	aaa_wb_rgbvalue_t rgbvalue;
} aaaProcResult_t;


typedef struct {
	UINT8 valid;
	SINT8 aeStep;
	SINT8 aeDevStep;
	UINT16 aeLuma;
	aeEvInfo_t evInfo;
} aeResult_t;

/****TODO***!!!!!!!!
*typedef struct {
*(TBD) wbGainResult;
*(TBD) rgbValue;
*(TBD) wbInfo;
*} sp5kAwbResult_t;
*/
typedef struct {
	UINT8 valid;
	UINT8 awbConvStatus;
	aaa_wb_result_t wbGainResult;
	aaa_wb_rgbvalue_t rgbvalue;
	aaa_wb_info_t wbinfo;
} awbResult_t;

typedef void (*fpaaaProc_t)(const aaaProcInfo_t *pinfo,
				aaaProcResult_t *presult);
typedef void (*fpaaaEarlyCb_t)();
typedef void (*fpafProc_t)(UINT32 fid);

typedef struct {
	UINT8 evUnit; /*how many evidx increasing when APEX EV +1*/
	SINT8 expIdx0Unit; /*what is tv*evUnit when expIdx==0*/
	UINT8 agcIdx0Unit; /*what is sv*evUnit when agcIdx==0*/
	UINT8 aperIdx0Unit; /*what is av*evUnit when aperIdx==0*/
} aeEvUnit_t;
typedef struct {
	SINT16 expIdxSysMin;
	SINT16 expIdxTblMin;
	SINT16 expIdxShortMin;
	SINT16 expIdxMax;
} sensor_expidx_limits_t;
typedef struct {
	UINT8 agcIdxMin;
	UINT8 agcIdxMax;
} sensor_agcidx_limits_t;

typedef struct {
	UINT8  valid;
	UINT8  sensorMode;
	SINT16 expIdx;
	SINT8  tverror;
	UINT8  tverr_unit;
} aaa_sensor_exp_sts_t;/*sensor_private.h*/

typedef struct {
	aaa_sensor_exp_sts_t sts;
	UINT8  longexp;
	UINT8  vtbl_type;
	UINT32 vcount;
	SINT16 mshtrim;
} aaa_sensor_exp_full_sts_t;/*sensor_private.h*/

typedef struct {
	UINT8  valid;
	UINT8  sensorMode;
	UINT8  agcIdx;
	SINT8  sverror;
	UINT8  sverr_unit;
} aaa_sensor_agc_sts_t;/*sensor_private.h*/

typedef struct {
	aaa_sensor_exp_full_sts_t fullsts;
/*	sensor_exp_sts_t sts;
	UINT8  sensorMode;
	SINT16 expIdx;
	SINT8  tverror;
	UINT8  tverr_unit;
	UINT8  longexp;
	UINT8  vtbl_type;
	UINT16 vcount;
	SINT16 mshtrim;
*/
	UINT8  nreg;
	UINT8  nbyte;
	UINT8  burstrenderbufitem[];
} aaa_aeExpSts_t;/*sensor_private.h*/
typedef struct { 
	aaa_sensor_agc_sts_t sts;
	/*
	UINT8  valid;
	UINT8  sensorMode;
	UINT8  agcIdx;
	SINT8  sverror;
	UINT8  sverr_unit;
	*/
	UINT8  nreg;
	UINT8  nbyte;
	UINT8  burstrenderbufitem[];
} aaa_aeAgcSts_t;/*sensor_private.h*/



typedef SINT16 ae_expidx_t;

#define AE_EXPIDX_BSHUTTER ((ae_expidx_t)(-1L<<(sizeof(ae_expidx_t)*8-1)))
#define AE_SYS_TV_LLIMIT (-16)
#define AE_SYS_TV_ULIMIT 16

typedef struct {
	UINT16  lv;     /* lv status*/
	UINT16  irise;   /*Iris status*/
	SINT16  expIdx; 
	SINT16  agcIdx;
    UINT32  isovalue;
}  AeCustomSts_t ;

typedef void (*fpAeStsCallback_t )(UINT32 mode , AeCustomSts_t ae_sts);

#ifdef __cplusplus
}
#endif

#endif
