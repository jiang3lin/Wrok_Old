/**************************************************************************
 *
 *       Copyright (c) 2005-2011 by iCatch Technology, Inc.
 *
 *  This software is copyrighted by and is the property of iCatch Technology,
 *  Inc.. All rights are reserved by iCatch Technology, Inc..
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
 *  Author: Eric Su, Matt Wang
 *
 **************************************************************************/
/* DO NOT EDIT the next 2 lines for auto revision by subversion
$Rev$
$Date::                     $
 */

#ifndef _GLOBAL_DEF_H_
#define _GLOBAL_DEF_H_

/**************************************************************************
 *                         H E A D E R   F I L E S
 **************************************************************************/
#include <time.h>

/**************************************************************************
 *                           C O N S T A N T S
 **************************************************************************/
/* gpioCfgSet() */
typedef enum {
	GPIO_GRP_TGL = 0,
	GPIO_GRP_LMI,
	GPIO_GRP_FML,
	GPIO_GRP_FMH,
	GPIO_GRP_GEN,
	GPIO_GRP_DISPL,
	GPIO_GRP_DISPH,
	GPIO_GRP_POCU,
	GPIO_GRP_USB,
#if SPCA5330
	GPIO_GRP_SAR,
	GPIO_GRP_OIS,
#endif
#if SPCA6330
	GPIO_GRP_CEC,
#endif
	GPIO_GRP_TOT,
} gpioGrp_t;

/* for backward compatible. */
#define GPIO_GRP_DISP  GPIO_GRP_DISPL

/**************************************************************************
 *                          D A T A    T Y P E S
 **************************************************************************/
/* timeget() callback */
typedef void (*fpTimeCallback_t)(struct tm *);

/**************************************************************************
 *                                  M A C R O S
 **************************************************************************/
#define GPIO_PACK(grp,pin,dir,active)	((((UINT32)dir&0x1)<<31)|(((UINT32)active&0x1)<<30)|(((UINT32)grp&0xf)<<8)|((UINT32)pin&0xff))
#define GPIO_UNPACK_PIN(pack)			(pack&0xff)
#define GPIO_UNPACK_GRP(pack)			((pack>>8)&0xf)
#define GPIO_UNPACK_ACTIVE(pack)		((pack>>30)&0x1)
#define GPIO_UNPACK_DIR(pack)			((pack>>31)&0x1)

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S
 **************************************************************************/
/* gpio.c */
UINT32 gpioCfgSet(UINT32 grpId, UINT32 pinMsk, UINT32 cfg);
UINT32 gpioCfgGet(UINT32 grpId, UINT32 pinMsk);
UINT32 gpioDirSet(UINT32 grpId, UINT32 pinMsk, UINT32 dir);
UINT32 gpioDirGet(UINT32 grpId, UINT32 pinMsk);
UINT32 gpioFuncSet(UINT32 grpId, UINT32 pinMsk, UINT32 val);
UINT32 gpioRead(UINT32 grpId, UINT32 pinMsk, UINT32 *pval);
UINT32 gpioWrite(UINT32 grpId, UINT32 pinMsk, UINT32 val);
UINT32 gpioPullSet(UINT32 grpId, UINT32 pinMsk, UINT32 enable);
UINT32 gpioDrvSet(UINT32 grpId, UINT32 pinMsk, UINT32 hdw, UINT32 ldw);
void   gpioPullInit(void);

#endif  /* _GLOBAL_DEF_H_ */
