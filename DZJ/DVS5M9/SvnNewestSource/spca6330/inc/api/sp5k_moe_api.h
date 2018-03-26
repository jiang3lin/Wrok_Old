/*******************************************************************************
 *
 *       Copyright (c) 2009-2011 by iCatch Technology, Inc.
 *
 *  This software is copyrighted by and is the property of iCatch Technology,
 *  Inc. All rights are reserved by iCatch Technology, Inc.
 *  This software may only be used in accordance with the corresponding
 *  license agreement. Any unauthorized use, duplication, distribution,
 *  or disclosure of this software is expressly forbidden.
 *
 *  This Copyright notice MUST not be removed or modified without prior
 *  written consent of iCatch Technology, Inc.
 *
 *  iCatch Technology, Inc. reserves the right to modify this software
 *  without notice.
 *
 *  iCatch Technology, Inc.
 *  19-1, Innovation First Road, Science-Based Industrial Park,
 *  Hsin-Chu, Taiwan, R.O.C.
 *
 *  Author:
 *
 **************************************************************************/
#ifndef _SP5K_MOE_API_H_
#define _SP5K_MOE_API_H_

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************
 *                            H E A D E R   F I L E S
 **************************************************************************/
 
 /**************************************************************************
 *                              C O N S T A N T S
 **************************************************************************/

/**************************************************************************
 *                                  M A C R O S
 **************************************************************************/

/**************************************************************************
 *                              D A T A   T Y P E S
 **************************************************************************/
typedef enum {
	#if SPCA5330
	SP5K_MOE_DATA_FMT_UNPACK_24BIT	= 0, /*!< II gen / II scale */
	SP5K_MOE_DATA_FMT_PACK_24BIT 	= 1, /*!< II gen / I scale */
	SP5K_MOE_DATA_FMT_32BIT			= 0, /*!< SAD / SII gen */
	SP5K_MOE_DATA_FMT_PACK_Y 		= 0,
	SP5K_MOE_DATA_FMT_8BIT			= 1,
	SP5K_MOE_DATA_FMT_PACK_UV		= 2,
	SP5K_MOE_DATA_FMT_UNPACK_UV		= 3,
	SP5K_MOE_DATA_FMT_UNPACK_UV_2X	= 4,
	SP5K_MOE_DATA_FMT_16BIT			= 5,
	SP5K_MOE_DATA_FMT_YUV_PLANE		= 6,
	SP5K_MOE_DATA_FMT_YUYV			= 7,
	#else
	SP5K_MOE_DATA_FMT_8BIT			= 0,
	SP5K_MOE_DATA_FMT_16BIT			= 1,
	SP5K_MOE_DATA_FMT_YUV422_844 	= 2,
	SP5K_MOE_DATA_FMT_YUV422_FDWT	= 3,
	SP5K_MOE_DATA_FMT_YUV422_211 	= 4,
	SP5K_MOE_DATA_FMT_YUV422_UNPACK	= 5,
	SP5K_MOE_DATA_FMT_YUV444_UNPACK	= 6,
	#endif
} sp5kMoeDataFmt_t;

typedef enum {
	SP5K_MOE_DATA_TYPE_UNSIGNED		= 0,
	SP5K_MOE_DATA_TYPE_SIGNED		= 1,
} sp5kMoeDataType_t;

typedef enum {
	SP5K_MOE_DATA_ABS_DIS			= 0,
	SP5K_MOE_DATA_ABS_EN			= 1,
} sp5kMoeDataAbs_t;

typedef enum {
	SP5K_MOE_MAT_CAL_ADD			= 0, /*!< src1 + src2 */
	SP5K_MOE_MAT_CAL_SUB			= 1, /*!< src1 - src2 */
	SP5K_MOE_MAT_CAL_MUL			= 2, /*!< src1 * src2, shift is 0 if 16-bit */
	SP5K_MOE_MAT_CAL_DIV			= 3, /*!< src1 / src2, only support 16-bit/8-bit or 8-bit/8-bit */
	#if SPCA5330 
	SP5K_MOE_MAT_CAL_MAX			= 4, /*!< max(src1, src2) */
	SP5K_MOE_MAT_CAL_MIN			= 5, /*!< min(src1, src2) */
	#else
	SP5K_MOE_MAT_CAL_ABS_DIFF		= 4, /*!< abs(src1 - src2) */ 
	SP5K_MOE_MAT_CAL_MAX			= 5, /*!< max(src1, src2) */
	SP5K_MOE_MAT_CAL_MIN			= 6, /*!< min(src1, src2) */
	#endif
} sp5kMoeMatrixCal_t;

typedef struct {
	SINT32 min;	/*!< 2's complement */
	SINT32 max; /*!< 2's complement */
} sp5kMoeClipCfg_t;

typedef struct {
	void *pbuf;
	UINT16 width;	/*!< max 8184, multiple of 8 */
	UINT16 height;	/*!< max 4095 */
	UINT16 x;		/*!< max width-1 */
	UINT16 y;		/*!< max height-1 */
	UINT8 fmt;		/*!< sp5kMoeDataFmt_t */
	UINT8 sign;		/*!< sp5kMoeDataType_t */
	UINT8 shift;	/*!< max 16, left shift for src, right shift for dst */ 
	UINT8 abs;		/*!< see sp5kMoeDataAbs_t */
} sp5kMoeObj_t;

typedef struct {
	UINT16 common;
	UINT8 sign;		/*!< sp5kMoeDataType_t */
	UINT8 shift;	/*!< left shift */
} sp5kMoeComObj_t;

typedef struct {
	UINT16 srcRoiH;/*!< 8n */
	UINT16 srcRoiV;
	UINT16 dstRoiH;/*!< 8n */
	UINT16 dstRoiV;
} sp5kMoeRoiCfg_t;

/**
 * \note 7x7 filter
 * p9 p8 p7 p6 p7 p8 p9 
 * p8 p5 p4 p3 p4 p5 p8
 * p7 p4 p2 p1 p2 p4 p7
 * p6 p3 p1 p0 p1 p3 p6
 * p7 p4 p2 p1 p2 p4 p7
 * p8 p5 p4 p3 p4 p5 p8
 * p9 p8 p7 p6 p7 p8 p9
 */
typedef struct {
	SINT16 para0;	/*!< 1S10bit */
	SINT16 para1;	/*!< 1S10bit */
	SINT16 para2;	/*!< 1S10bit */
	SINT16 para3;	/*!< 1S10bit */
	SINT16 para4;	/*!< 1S10bit */
	SINT16 para5;	/*!< 1S10bit */
	SINT16 para6;	/*!< 1S10bit */
	SINT16 para7;	/*!< 1S10bit */
	SINT16 para8;	/*!< 1S10bit */
	SINT16 para9;	/*!< 1S10bit */
} sp5kMoeFilterObj_t;

typedef enum {
	SP5K_MOE_LUT_BIT_16			= 0,
	SP5K_MOE_LUT_BIT_8			= 1,
} sp5kMoeLutBit_t;

typedef enum {
	SP5K_MOE_LUT_DISABLE		= 0,
	SP5K_MOE_LUT_SRC1_ENABLE	= 1,
	SP5K_MOE_LUT_SRC2_ENABLE	= 2,
} sp5kMoeLutSel_t;

typedef struct {
	UINT8 lutBit;/*!< see sp5kMoeLutBit_t */
	UINT8 lutSelect;/*!< see sp5kMoeLutSel_t */ 
} sp5kMoeLutCfg_t;

typedef struct {
	UINT8 topMrrPixel;/*!< 0 ~ 3 */
	UINT8 topMrrEn;/*!< 0 or 1 */
	UINT8 botMrrPixel;/*!< 0 ~ 3 */
	UINT8 botMrrEn;/*!< 0 or 1 */
	UINT8 leftMrrPixel;/*!< 0 ~ 3 */
	UINT8 leftMrrEn;/*!< 0 or 1 */
	UINT8 rightMrrPixel;/*!< 0 ~ 3 */
	UINT8 rightMrrEn;/*!< 0 or 1 */
	UINT8 reserved;/*!< should be 0 */
} sp5kMoeMirrorCfg_t;

/**************************************************************************
 *                      E X T E R N A L   R E F E R E N C E
 **************************************************************************/

/**************************************************************************
 *                  F U N C T I O N   D E C L A R A T I O N S
 **************************************************************************/
/**
 * \brief get the handle of MOE resource
 * \return handle value, 0 is error
 */
UINT32 sp5kMoeHandleInit(void);

/**
 * \brief reset the handle of MOE resource then all operations will be clear
 * \param[in] handle the handle of MOE resource
 * \return SUCCESS or FAIL
 */
UINT32 sp5kMoeHandleReset(UINT32 handle);

/**
 * \brief run the operations in the handle of MOE resource
 * \param[in] handle the handle of MOE resource
 * \return SUCCESS or FAIL
 */
UINT32 sp5kMoeHandleRun(UINT32 handle);

/**
 * \brief release the handle of MOE resource
 * \param[in] handle the handle of MOE resource
 * \return SUCCESS or FAIL
 */
UINT32 sp5kMoeHandleRelease(UINT32 handle);

/**
 * \brief set matrix point by point operation
 * \param[in] handle the handle of MOE resource
 * \param[in] psrc1 the pointer to the source obj 1
 * \param[in] psrc2 the pointer to the source obj 2
 * \param[out] pdst the pointer to the destination obj for the result
 * \param[in] pclip the pointer to the clip config, use default if NULL
 * \param[in] roiW the roi width in src1/src2/dst to do operation
 * \param[in] roiH the roi height in src1/src2/dst to do operation
 * \param[in] cal the calculation in sp5kMoeMatrixCal_t
 * \return SUCCESS or FAIL
 * \note dst = clip{cal(src1,src2)}
 */
UINT32 sp5kMoeMatrixPointByPointSet(UINT32 handle, sp5kMoeObj_t *psrc1, 
	sp5kMoeObj_t *psrc2, sp5kMoeObj_t *pdst, sp5kMoeClipCfg_t *pclip, 
	UINT32 roiW, UINT32 roiH, UINT32 cal);

/**
 * \brief set matrix point by point + LUT operation
 * \param[in] handle the handle of MOE resource
 * \param[in] psrc1 the pointer to the source obj 1
 * \param[in] psrc2 the pointer to the source obj 2
 * \param[out] pdst the pointer to the destination obj for the result
 * \param[in] plut the pointer to the lut config, use default if NULL
 * \param[in] pclip the pointer to the clip config, use default if NULL
 * \param[in] roiW the roi width in src1/src2/dst to do operation
 * \param[in] roiH the roi height in src1/src2/dst to do operation
 * \param[in] cal the calculation in sp5kMoeMatrixCal_t
 * \return SUCCESS or FAIL
 * \note dst = clip{cal(src1,src2)}
 */
UINT32 sp5kMoeMatrixPointByPointLutSet(UINT32 handle, sp5kMoeObj_t *psrc1, 
	sp5kMoeObj_t *psrc2, sp5kMoeObj_t *pdst, sp5kMoeLutCfg_t *plut, sp5kMoeClipCfg_t *pclip, 
	UINT32 roiW, UINT32 roiH, UINT32 cal);

/**
 * \brief set matrix point by common operation
 * \param[in] handle the handle of MOE resource
 * \param[in] psrc1 the pointer to the source obj
 * \param[in] pcom the pointer to the common obj
 * \param[out] pdst the pointer to the destination obj for the result
 * \param[in] pclip the pointer to the clip config, use default if NULL
 * \param[in] roiW the roi width in src/dst to do operation
 * \param[in] roiH the roi height in src/dst to do operation
 * \param[in] cal the calculation in sp5kMoeMatrixCal_t
 * \return SUCCESS or FAIL
 * \note dst = clip{cal(src,com)}
 */
UINT32 sp5kMoeMatrixPointByCommonSet(UINT32 handle, sp5kMoeObj_t *psrc, 
	sp5kMoeComObj_t *pcom, sp5kMoeObj_t *pdst, sp5kMoeClipCfg_t *pclip, 
	UINT32 roiW, UINT32 roiH, UINT32 cal);

/**
 * \brief set 7x7 image filter operation
 * \param[in] handle the handle of MOE resource
 * \param[in] psrc the pointer to the source obj
 * \param[in] pfilt the pointer to the image filter
 * \param[out] pdst the pointer to the destination obj for the result
 * \param[in] pclip the pointer to the clip config, use default if NULL
 * \param[in] pmirr the pointer to the mirror config
 * \param[in] roiW the roi width in src/dst to do operation, <= max(154,psrc->width-6)
 * \param[in] roiH the roi height in src/dst to do operation, <= psrc->height - 6
 * \return SUCCESS or FAIL
 */
UINT32 sp5kMoeImgFilter7x7Set(UINT32 handle, sp5kMoeObj_t *psrc, 
	sp5kMoeFilterObj_t *pfilt, sp5kMoeObj_t *pdst, sp5kMoeClipCfg_t *pclip,
	sp5kMoeMirrorCfg_t *pmirr, UINT32 roiW, UINT32 roiH);

/**
 * \brief set update look up table function
 * \param[in] handle the cmd handle
 * \param[in] ptblObj the MOE object of lookup table
 * \return SUCCESS or FAIL
 */
UINT32 sp5kMoeUpdateLutSet(UINT32 handle, sp5kMoeObj_t *ptblObj);

/**
 * \brief set YUV scale operation
 * \param[in] handle the handle of MOE resource
 * \param[in] psrc the pointer to the source obj
 * \param[in] pdst the pointer to the destation obj
 * \param[in] plutCfg the LUT configuration
 * \param[in] proiCfg the ROI configuration
 * \return SUCCESS or FAIL
 */
UINT32 sp5kMoeYuvScaleSet(UINT32 handle, sp5kMoeObj_t *psrc, sp5kMoeObj_t *pdst,
	sp5kMoeLutCfg_t *plutCfg, sp5kMoeRoiCfg_t *proiCfg);

#ifdef __cplusplus
}
#endif

#endif /*_SP5K_MOE_API_H_*/
