/**************************************************************************
 *
 *        Copyright (c) 2008 by Sunplus mMedia Inc., Ltd.
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
#ifndef _SP5K_CVR_WARNING_SYSTEM_PRIVATE_H_
#define _SP5K_CVR_WARNING_SYSTEM_PRIVATE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"
#include "middleware/lanedetect_def.h"


#define SIGN_RECOGNIZE	0x00000001
#define SPEED_LIMIT		0x00000002
#define HEAD_LIGHT		0x00000004
#define LDWS			0x00000008


typedef UINT32 (*sp5kCVRWarningSystemInputYUVSrc)(UINT8* buff, UINT32 width, UINT32 height);
#define CVRWarningSystemInputYUVSrc sp5kCVRWarningSystemInputYUVSrc


void sp5kCvrWarningSystemInit(UINT32 FunFlag);

#ifdef __cplusplus
}
#endif

#endif /* _SP5K_CVR_WARNING_SYSTEM_PRIVATE_H_ */

