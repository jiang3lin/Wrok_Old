/**************************************************************************
 *
 *        Copyright (c) 2009 by Sunplus mMedia Inc., Ltd.
 *
 *  This software is copyrighted by and is the property of Sunplus
 *  mMedia Inc., Ltd. All rights are reserved by Sunplus mMedia
 *  Inc., Ltd. This software may only be used in accordance with the
 *  corresponding license agreement. Any unauthorized use, duplication,
 *  distribution, or disclosure of this software is expressly forbidden.
 *
 *  This Copyright notice may not be removed or modified without prior
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
#ifndef _SP5K_YUV_HISTOGRAM_H_
#define _SP5K_YUV_HISTOGRAM_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
*                          D A T A    T Y P E S
******************************************************************************/
typedef struct {
	UINT32 sumVal;
} sp5kYSumRoiVal_t;

typedef struct {
	UINT32 num;			/* number of ROI */
	UINT32 ySumRoiHstr;	/* mapping to normalization 0~1024 value */
	UINT32 ySumRoiHend;	/* mapping to normalization 0~1024 value */
	UINT32 ySumRoiVstr;	/* mapping to normalization 0~1024 value */
	UINT32 ySumRoiVend;	/* mapping to normalization 0~1024 value */
} sp5kYsumRoiNormalRegion_t;

/*paramId*/
typedef enum {
	SP5K_YSUM_CFG_ROI_TOTALNUM_SET	= 0,/* param1: total number of ROI, 0~128 */
	SP5K_YSUM_CFG_USER_ROI_SET			= 1,/* param1: total number of sp5kYsumRoiNormalRegion_t pointer */
	SP5K_YSUM_CFG_WDR_ROI_SET		= 2,/* param1: NULL  */
	SP5K_YSUM_CFG_PROC_EN			= 3,/* param1: typedef sp5kYSumSwitch_e  */
} sp5kYSumCfgSel_e;

typedef enum {
	SP5K_YSUM_VAL_HISTOGRAM_TBL		= 0,/* param1: an variable to hold a return histogram buffer address */
	SP5K_YSUM_VAL_ROI_Y				= 1,/* param1: a pointer to sp5kYSumRoiVal_t */
} sp5kYSumValueSel_e;

typedef enum {
	SP5K_YSUM_EN_DISABLE		= 0,/* disable */
	SP5K_YSUM_EN_ENABLE			= 1,/* enable */
} sp5kYSumEn_e;


UINT32 sp5kYSumCfgSet(UINT32 selector, UINT32 value);
UINT32 sp5kYSumValueGet(UINT32 selector, UINT32 *pvalue);
UINT32 sp5kYSumDo(void);


#ifdef __cplusplus
}
#endif

#endif /* _SP5K_YUV_HISTOGRAM_H_*/

