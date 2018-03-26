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
#ifndef _APP_DBG_API_H_
#define _APP_DBG_API_H_

/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/
#include "sp5k_dbg_api.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#if !defined(HOST_DBG)
#define HOST_DBG 0
#endif

#if HOST_DBG
#define DBG_ASSERT_EXT_EN 1
#define DBG_PRINT_EXT_EN 1
#endif

/*
 * force turn off debug code generating.
 */
#define DBG_NO_DBG  0  /* if 1, force turn off debug code generating, for testing (need clean and rebuild all). */
#if DBG_NO_DBG == 1
#undef DBG_FUNC_TRACE_EXT_EN
#undef DBG_ASSERT_EXT_EN
#undef DBG_PRINT_EXT_EN

#define DBG_FUNC_TRACE_EXT_EN  0
#define DBG_ASSERT_EXT_EN      0
#define DBG_PRINT_EXT_EN       0
#endif

/*
 * check and set DBG_ASSERT_EN.
 */
#ifdef DBG_ASSERT_EXT_EN  /* if defined in makefile exists, use it. */
#undef DBG_ASSERT_EN
#define DBG_ASSERT_EN  DBG_ASSERT_EXT_EN  /* suppressed by external. */
#endif

#ifndef DBG_ASSERT_EN  /* if not defined, set off as default. */
#define DBG_ASSERT_EN  0
#endif

#if ( DBG_ASSERT_EN != 0 && DBG_ASSERT_EN != 1 )  /* invalid setting? */
#error Invalid DBG_ASSERT_EN setting.
#endif

/*
 * check and set DBG_PRINT_EN.
 */
#ifdef DBG_PRINT_EXT_EN  /* if defined in makefile exists, use it. */
#undef DBG_PRINT_EN
#define DBG_PRINT_EN  DBG_PRINT_EXT_EN  /* suppressed by external. */
#endif

#ifndef DBG_PRINT_EN  /* if not defined, set off as default. */
#define DBG_PRINT_EN  0
#endif

#if ( DBG_PRINT_EN != 0 && DBG_PRINT_EN != 1 )  /* invalid setting? */
#error Invalid DBG_PRINT_EN setting.
#endif

/*
 * asserting.
 *
 * macros:
 *   DBG_ASSERT().
 */
 #if SPCA5330
 #if DBG_ASSERT_EN == 0
#define DBG_ASSERT(expr)

#elif DBG_ASSERT_EN == 1
#define DBG_ASSERT(expr) \
	if ( !(expr) ) { \
		dbgAssert((UINT8 *) __FILE__, __LINE__, NULL); \
	} \

#endif
 #else
#if DBG_ASSERT_EN == 0
#define DBG_ASSERT(expr)

#elif DBG_ASSERT_EN == 1
#define DBG_ASSERT(expr) \
	if ( !(expr) ) { \
		 dbgAssert(0x2f, __FILE__, __LINE__, NULL); \
	} \

#endif
#endif
#ifdef DBG_PRINT
 #undef DBG_PRINT
#endif

#if DBG_PRINT_EN == 0

#define DBG_PRINT(strFmt, args...)

#elif DBG_PRINT_EN == 1

#if 1
#define DBG_PRINT(strFmt, args...) \
	printf(strFmt, ##args)
#else
#define DBG_PRINT(strFmt, args...) \
	sioRedirect(NULL); \
	printf(strFmt, ##args); \
	sioRedirect(putc_null);
#endif

#endif

#if 0
#if SPCA5110==1
void sioPutcRedirect(void * pf);
#define sioRedirect sioPutcRedirect
#endif
#endif

//void dbgPrint(UINT32 sw, const UINT8 *strFmt, ...);

#if CMD_GDB
extern void breakpoint(void);
extern int getDgbType(void);
#endif

void sioRedirect(void *pf);
void siofilterPatternDump(void);
UINT32 siofilterPatternSet(BOOL fExclude, char *pattern);

#endif  /* _APP_DBG_API_H_ */

