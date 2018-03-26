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
 **************************************************************************/
#ifndef _SYS_REGS_H_
#define _SYS_REGS_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "reg_def.h"
#if SPCA5310
#include "spca5310/reg_front.h"
#elif SPCA5210
#include "5210/reg_front.h"
#else
#include "reg_front.h"
#endif

#ifdef __cplusplus
}
#endif

#endif /* _SYS_REGS_H_ */
