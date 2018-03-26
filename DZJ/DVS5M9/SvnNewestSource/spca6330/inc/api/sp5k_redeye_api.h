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
 *  Author: Edward Kuo
 *
 **************************************************************************/
#ifndef _SP5K_REDEYE_API_H_
#define _SP5K_REDEYE_API_H_

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************
 *                            H E A D E R   F I L E S
 **************************************************************************/
#include "middleware/redeye_def.h"

/**************************************************************************
 *                              C O N S T A N T S
 **************************************************************************/
typedef enum {
	SP5K_REDEYE_FACE_ANGLE_0 = REDEYE_FACE_ANGLE_0,
	SP5K_REDEYE_FACE_ANGLE_30 = REDEYE_FACE_ANGLE_30,
	SP5K_REDEYE_FACE_ANGLE_60 = REDEYE_FACE_ANGLE_60,
	SP5K_REDEYE_FACE_ANGLE_90 = REDEYE_FACE_ANGLE_90,
	SP5K_REDEYE_FACE_ANGLE_120 = REDEYE_FACE_ANGLE_120,
	SP5K_REDEYE_FACE_ANGLE_150 = REDEYE_FACE_ANGLE_150,
	SP5K_REDEYE_FACE_ANGLE_180 = REDEYE_FACE_ANGLE_180,
	SP5K_REDEYE_FACE_ANGLE_210 = REDEYE_FACE_ANGLE_210,
	SP5K_REDEYE_FACE_ANGLE_240 = REDEYE_FACE_ANGLE_240,
	SP5K_REDEYE_FACE_ANGLE_270 = REDEYE_FACE_ANGLE_270,
	SP5K_REDEYE_FACE_ANGLE_300 = REDEYE_FACE_ANGLE_300,
	SP5K_REDEYE_FACE_ANGLE_330 = REDEYE_FACE_ANGLE_330,
	SP5K_REDEYE_FACE_ANGLE_NONE = 0xFF,
} sp5kRedeyeFaceAngle_e;

/**************************************************************************
 *                                  M A C R O S
 **************************************************************************/

/**************************************************************************
 *                              D A T A   T Y P E S
 **************************************************************************/
typedef redeyeFaceInfo_t sp5kRedEyeFaceInfo_t;
typedef redeyeEyeCircleInfo_t sp5kRedEyeEyeCircleInfo_t;

/**************************************************************************
 *                      E X T E R N A L   R E F E R E N C E
 **************************************************************************/

/**************************************************************************
 *                  F U N C T I O N   D E C L A R A T I O N S
 **************************************************************************/
#if 0
UINT32 sp5kRedEyeYuvBufEyeGet(UINT8 *yuvAddr, UINT32 yuvWidth, redeyeFaceInfo_t *pface, UINT32 maxEyes, redeyeEyeInfo_t *peye, UINT32 *pnumEye);
UINT32 sp5kRedEyeYuvBufCropSet(UINT8 *yuvAddr, UINT32 yuvWidth, redeyeFaceInfo_t *pface, redeyeEyeInfo_t *peye, UINT32 numEye, UINT32 isLarge);
#endif

/* \brief      Remove the redeye in main image
 *
 * \param[in]  srcAddr     address of 8Y4U4V main image buffer
 * \param[in]  width       physical width of image buffer
 * \param[in]  height      physical height of image buffer
 * \param[in]  infoWidth   maximum width used in conversion of faceList
 * \param[in]  infoHeight  maximum height used in conversion of faceList 
 * \param[in]  nface       number of faces detected in QV
 * \param[in]  faceList    pointer to face information fetched from face track.
 *                         The x, y, size information is converted into proper
 *                         coordinate system.
 *             
 * \return     SUCCESS or FAIL
 */
UINT32
sp5kRedEyeReductionDo(
	UINT32 srcAddr, 
	UINT32 width, 
	UINT32 height,
	UINT32 infoWidth,
	UINT32 infoHeight,
	UINT32 nface,
	sp5kRedEyeFaceInfo_t *faceList
);

/* \brief      Remove the redeye in main image. This function should be invoked
 *             after redeyeReductionDo.
 *
 * \param[out] peyes      number of eyes applies removal
 * \param[out] infoList   pointer to store eye info. (Host should allocate
 *                        enough buffer).
 *             
 * \return     SUCCESS or FAIL
 */
UINT32
sp5kRedEyeEyeCircleInfoGet(
	UINT32 *peyes,
	sp5kRedEyeEyeCircleInfo_t *infoList
);

#ifdef __cplusplus
}
#endif

#endif /* _SP5K_REDEYE_API_H_ */
