/**************************************************************************
 *                                                                        *
 *         Copyright (c) 2004 by Sunplus Technology Co., Ltd.             *
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
 *  Hsin-Chu, Taiwan, R.O.C.                                              *
 *                                                                        *
 *  Author: Matt Wang                                                     *
 *                                                                        *
 **************************************************************************/
#ifndef __SP5K_DBG_H__
#define __SP5K_DBG_H__

/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/
#define DBG_ASSERT_EN  1

/**************************************************************************
 *                            C O N S T A N T S                           *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#if DBG_ASSERT_EN
 #define HOST_ASSERT(expr) \
	if ( !(expr) ) { \
		dbgAssert(__FILE__, __LINE__, NULL); \
	}
#else
 #define HOST_ASSERT(expr)
#endif

/**************************************************************************
 *           M O D U L E   V A R I A B L E S   R E F E R E N C E          *
 **************************************************************************/

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/

void dbgAssert(const char *fileName, UINT32 lineNum, const char *msgStr, ...)
	__attribute__ ((format (printf, 3, 4)));

/*
 * sp5k_dbg_en.c
 */
void sp5kDbgEnable(UINT32 id, UINT32 en);

/*
 * sp5k_dbg_init.c
 */
void sp5kDbgInit(void);

#endif  /* __SP5K_DBG_H__ */

