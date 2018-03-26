/*******************************************************************************
 *
 *         Copyright (c) 2002 by Sunplus Technology Co., Ltd.
 *
 *  This software is copyrighted by and is the property of Sunplus
 *  Technology Co., Ltd. All rights are reserved by Sunplus Technology
 *  Co., Ltd. This software may only be used in accordance with the
 *  corresponding license agreement. Any unauthorized use, duplication,
 *  distribution, or disclosure of this software is expressly forbidden.
 *
 *  This Copyright notice MUST not be removed or modified without prior
 *  written consent of Sunplus Technology Co., Ltd.
 *
 *  Sunplus Technology Co., Ltd. reserves the right to modify this
 *  software without notice.
 *
 *  Sunplus Technology Co., Ltd.
 *  19, Innovation First Road, Science-Based Industrial Park,
 *  Hsin-Chu, Taiwan, R.O.C.
 *
 *  Author: Ymkao
 *
 ******************************************************************************/
/* DO NOT EDIT the next 2 lines for auto revision by subversion
$Rev$
$Date::                     $
 */


#ifndef _DIST_DEF_H_
#define _DIST_DEF_H_

/*******************************************************************************
 *                           C O N S T A N T S
 ******************************************************************************/

typedef enum {
	STILL_DIST_EW_BLOCKS_H = 0x00,
	STILL_DIST_EW_BLOCKS_V,
	STILL_DIST_EW_THRESHOLD_H,
	STILL_DIST_EW_THRESHOLD_V,
	STILL_DIST_ME_BLOCKS_THRESHOLD = 0x10,
	STILL_DIST_GMV_INFO_LENGTH = 0x20,
	STILL_DIST_EXP_ISO_STEP = 0x30,
	STILL_DIST_ENV_MODE = 0x40,
} stillDistSel_t;

/*******************************************************************************
 *                              M A C R O S
 ******************************************************************************/

/*******************************************************************************
 *                          D A T A    T Y P E S
 ******************************************************************************/
/** @brief Gmv Related Information */
typedef struct stillDistGmvInfo_s {
	UINT32 frameId;    /*<! the frame id of this gmv pair */
	SINT16 gmvx, gmvy; /*<! the x, y gmv value */
	SINT16 gmvx2, gmvy2; /*<! the x, y gmv value */
	UINT8 numx, numy;  /*<! number of blocks that have x, y movement */
} stillDistGmvInfo_t;



/*******************************************************************************
 *                         G L O B A L    D A T A
 ******************************************************************************/

/*******************************************************************************
 *                  E X T E R N A L    R E F E R E N C E
 ******************************************************************************/

/*******************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S
 ******************************************************************************/

#endif /* _DIST_DEF_H_ */


