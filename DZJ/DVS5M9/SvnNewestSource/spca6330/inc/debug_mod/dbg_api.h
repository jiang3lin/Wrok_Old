/*******************************************************************************
*
*       Copyright (c) 2004-2011 by iCatch Technology, Inc.
*
*  This software is copyrighted by and is the property of iCatch
*  Technology, Inc.. All rights are reserved by iCatch Technology, Inc..
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
*  Author: Matt Wang
*
*******************************************************************************/
#ifndef _DBG_API_H_
#define _DBG_API_H_

/*******************************************************************************
*                            H E A D E R   F I L E S
*******************************************************************************/
#include "common.h"

/*******************************************************************************
*                              C O N S T A N T S
*******************************************************************************/
/* force turn off debug code generating. */
#define DBG_NO_DBG  0  /* if 1, force turn off debug code generating, for testing (need clean and rebuild all). */
#if DBG_NO_DBG == 1
#undef DBG_FUNC_TRACE_EXT_EN
#undef DBG_ASSERT_EXT_EN
#undef DBG_PRINT_EXT_EN
#undef DBG_PRINT_EXT_LVL

#define DBG_FUNC_TRACE_EXT_EN  0
#define DBG_ASSERT_EXT_EN      0
#define DBG_PRINT_EXT_EN       0
#define DBG_PRINT_EXT_LVL      0
#endif

/* check and set DBG_FUNC_TRACE_EN. */
#ifdef DBG_FUNC_TRACE_EXT_EN  /* if defined in makefile exists, use it. */
#undef DBG_FUNC_TRACE_EN
#define DBG_FUNC_TRACE_EN  DBG_FUNC_TRACE_EXT_EN  /* suppressed by external. */
#endif

#ifndef DBG_FUNC_TRACE_EN  /* if not defined, set off as default. */
#define DBG_FUNC_TRACE_EN  0
#endif

#if ( DBG_FUNC_TRACE_EN != 0  && DBG_FUNC_TRACE_EN != 1 )  /* invalid setting? */
#error Invalid DBG_FUNC_TRACE_EN setting.
#endif

/* check and set DBG_ASSERT_EN. */
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

/* check and set DBG_PRINT_EN. */
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

/* check and set DBG_PRINT_LVL. */
#ifdef DBG_PRINT_EXT_LVL  /* if defined in makefile exists, use it. */
#undef DBG_PRINT_LVL
#define DBG_PRINT_LVL  DBG_PRINT_EXT_LVL  /* suppressed by external. */
#endif

#ifndef DBG_PRINT_LVL  /* if not defined, set off as default. */
#define DBG_PRINT_LVL  0
#endif

#if ( DBG_PRINT_LVL > 9 )  /* invalid setting? */
#error Invalid DBG_PRINT_LVL setting.
#endif

/* check and set DBG_GPIO_EN. */
#ifdef DBG_GPIO_EXT_EN  /* if defined in makefile exists, use it. */
#undef DBG_GPIO_EN
#define DBG_GPIO_EN  DBG_GPIO_EXT_EN  /* suppressed by external. */
#endif

#ifndef DBG_GPIO_EN  /* if not defined, set off as default. */
#define DBG_GPIO_EN  0
#endif

#if ( DBG_GPIO_EN != 0 && DBG_GPIO_EN != 1 )  /* invalid setting? */
#error Invalid DBG_GPIO_EN setting.
#endif

/* check and set DEBUG_VER. */
#undef DEBUG_VER
#if ( DBG_FUNC_TRACE_EN == 1 && DBG_ASSERT_EN == 1 && \
      DBG_PRINT_EN == 1 && DBG_GPIO_EN == 1 )
#define DEBUG_VER  1
#else
#define DEBUG_VER  0
#endif

/* check and set RELEASE_VER. */
#undef RELEASE_VER
#if ( DBG_FUNC_TRACE_EN == 0 && DBG_ASSERT_EN == 0 && \
      DBG_PRINT_EN == 0 && DBG_GPIO_EN == 0 )
#define RELEASE_VER  1
#else
#define RELEASE_VER  0
#endif

/*
  <module name>_DBG_SWITCH_SET(),
  <module name>_DBG_SWITCH_GET().
*/
#define DBG_SWITCH_FUNC_ENTER_RETURN  (1 << 0)
#define DBG_SWITCH_FUNC_ARG_RET_VAL   (1 << 1)
#define DBG_SWITCH_ASSERT             (1 << 2)
#define DBG_SWITCH_PRINT              (1 << 3)
#define DBG_SWITCH_GPIO               (1 << 4)
#define DBG_SWITCH_TYPE_MASK          (0x1F)
#define DBG_SWITCH_TYPE_MASK_OFF      (!(0x1F))

#define DBG_SWITCH_OUTPUT_UART        (0 << 5)
#define DBG_SWITCH_OUTPUT_USB         (1 << 5)
#define DBG_SWITCH_OUTPUT_CARD        (2 << 5)
#define DBG_SWITCH_OUTPUT_MASK        (0x60)
#define DBG_SWITCH_OUTPUT_MASK_OFF    (!(0x60))

/* DBG_GPIO_SET(). */
typedef enum {
	DBG_GPIO_GRP_TGL  = 0,
	DBG_GPIO_GRP_LMI  = 1,
	DBG_GPIO_GRP_FML  = 2,
	DBG_GPIO_GRP_FMH  = 3,
	DBG_GPIO_GRP_GEN  = 4,
	DBG_GPIO_GRP_DISP = 5,
	DBG_GPIO_GRP_POCU = 6,
	DBG_GPIO_GRP_DISPH = 7,
} dbgGpioGrp_t;

#if 1  /* these definition will be removed in future. */
/* DBG_ERR_REPORT(). */
#define DBG_ERR_WEIGHT_INFO      0
#define DBG_ERR_WEIGHT_WARNING   1
#define DBG_ERR_WEIGHT_CRITICAL  2
#endif

/*******************************************************************************
*                            G L O B A L   D A T A
*******************************************************************************/

/*******************************************************************************
*                      E X T E R N A L   R E F E R E N C E
*******************************************************************************/
extern int getch(void);
extern int printf(const char *format, ...)
	__attribute__ ((format (printf, 1, 2)));

/*******************************************************************************
*                                  M A C R O S
*******************************************************************************/
/*
  global switch.

  macros:
    DBG_SWITCH_SET(),
    DBG_SWITCH_GET().
*/
#define DBG_SWITCH_SET(sw)
#define DBG_SWITCH_GET()  0

/*
  file name declaration.

  macros:
    DBG_FILE_NAME_DECLARE().
*/
#define DBG_FILE_NAME_DECLARE()

/*
  function tracing.

  macros:
    DBG_FUNC_ENTER(),
    DBG_FUNC_ENTERn), n=0~9,
    DBG_FUNC_RETURN(),
    DBG_FUNC_RETURNn(), n=0~9,
    DBG_FUNC_RETURN_VOID(),
    DBG_FUNC_RETURN_VOIDn(), n=0~9.
*/
#if DBG_FUNC_TRACE_EN == 0
#define DBG_FUNC_ENTER(strFmt, args...)
#define DBG_FUNC_ENTER0  DBG_FUNC_ENTER
#define DBG_FUNC_ENTER1  DBG_FUNC_ENTER
#define DBG_FUNC_ENTER2  DBG_FUNC_ENTER
#define DBG_FUNC_ENTER3  DBG_FUNC_ENTER
#define DBG_FUNC_ENTER4  DBG_FUNC_ENTER
#define DBG_FUNC_ENTER5  DBG_FUNC_ENTER
#define DBG_FUNC_ENTER6  DBG_FUNC_ENTER
#define DBG_FUNC_ENTER7  DBG_FUNC_ENTER
#define DBG_FUNC_ENTER8  DBG_FUNC_ENTER
#define DBG_FUNC_ENTER9  DBG_FUNC_ENTER

#define DBG_FUNC_RETURN(retVal, strFmt, args...)  return (retVal)
#define DBG_FUNC_RETURN0  DBG_FUNC_RETURN
#define DBG_FUNC_RETURN1  DBG_FUNC_RETURN
#define DBG_FUNC_RETURN2  DBG_FUNC_RETURN
#define DBG_FUNC_RETURN3  DBG_FUNC_RETURN
#define DBG_FUNC_RETURN4  DBG_FUNC_RETURN
#define DBG_FUNC_RETURN5  DBG_FUNC_RETURN
#define DBG_FUNC_RETURN6  DBG_FUNC_RETURN
#define DBG_FUNC_RETURN7  DBG_FUNC_RETURN
#define DBG_FUNC_RETURN8  DBG_FUNC_RETURN
#define DBG_FUNC_RETURN9  DBG_FUNC_RETURN

#define DBG_FUNC_RETURN_VOID(strFmt, args...)  return
#define DBG_FUNC_RETURN_VOID0  DBG_FUNC_RETURN_VOID
#define DBG_FUNC_RETURN_VOID1  DBG_FUNC_RETURN_VOID
#define DBG_FUNC_RETURN_VOID2  DBG_FUNC_RETURN_VOID
#define DBG_FUNC_RETURN_VOID3  DBG_FUNC_RETURN_VOID
#define DBG_FUNC_RETURN_VOID4  DBG_FUNC_RETURN_VOID
#define DBG_FUNC_RETURN_VOID5  DBG_FUNC_RETURN_VOID
#define DBG_FUNC_RETURN_VOID6  DBG_FUNC_RETURN_VOID
#define DBG_FUNC_RETURN_VOID7  DBG_FUNC_RETURN_VOID
#define DBG_FUNC_RETURN_VOID8  DBG_FUNC_RETURN_VOID
#define DBG_FUNC_RETURN_VOID9  DBG_FUNC_RETURN_VOID

#elif DBG_FUNC_TRACE_EN == 1
#define DBG_FUNC_ENTER(strFmt, args...) \
	dbgFuncEnter(__FUNCTION__, __LINE__, strFmt, ##args)

#define DBG_FUNC_RETURN(retVal, strFmt, args...) do { \
	dbgFuncReturn(__FUNCTION__, __LINE__, strFmt, ##args); \
	return (retVal); \
} while (0)

#define DBG_FUNC_RETURN_VOID(strFmt, args...) do { \
	dbgFuncReturn(__FUNCTION__, __LINE__, strFmt, ##args); \
	return; \
} while (0)

	#define DBG_FUNC_ENTER0  DBG_FUNC_ENTER
	#define DBG_FUNC_RETURN0  DBG_FUNC_RETURN
	#define DBG_FUNC_RETURN_VOID0  DBG_FUNC_RETURN_VOID

	#if DBG_PRINT_LVL >= 1
	#define DBG_FUNC_ENTER1  DBG_FUNC_ENTER
	#define DBG_FUNC_RETURN1  DBG_FUNC_RETURN
	#define DBG_FUNC_RETURN_VOID1  DBG_FUNC_RETURN_VOID
	#else
	#define DBG_FUNC_ENTER1(strFmt, args...)
	#define DBG_FUNC_RETURN1(retVal, strFmt, args...)  return (retVal)
	#define DBG_FUNC_RETURN_VOID1(strFmt, args...)  return
	#endif  /* 1 */

	#if DBG_PRINT_LVL >= 2
	#define DBG_FUNC_ENTER2  DBG_FUNC_ENTER
	#define DBG_FUNC_RETURN2  DBG_FUNC_RETURN
	#define DBG_FUNC_RETURN_VOID2  DBG_FUNC_RETURN_VOID
	#else
	#define DBG_FUNC_ENTER2(strFmt, args...)
	#define DBG_FUNC_RETURN2(retVal, strFmt, args...)  return (retVal)
	#define DBG_FUNC_RETURN_VOID2(strFmt, args...)  return
	#endif  /* 2 */

	#if DBG_PRINT_LVL >= 3
	#define DBG_FUNC_ENTER3  DBG_FUNC_ENTER
	#define DBG_FUNC_RETURN3  DBG_FUNC_RETURN
	#define DBG_FUNC_RETURN_VOID3  DBG_FUNC_RETURN_VOID
	#else
	#define DBG_FUNC_ENTER3(strFmt, args...)
	#define DBG_FUNC_RETURN3(retVal, strFmt, args...)  return (retVal)
	#define DBG_FUNC_RETURN_VOID3(strFmt, args...)  return
	#endif  /* 3 */

	#if DBG_PRINT_LVL >= 4
	#define DBG_FUNC_ENTER4  DBG_FUNC_ENTER
	#define DBG_FUNC_RETURN4  DBG_FUNC_RETURN
	#define DBG_FUNC_RETURN_VOID4  DBG_FUNC_RETURN_VOID
	#else
	#define DBG_FUNC_ENTER4(strFmt, args...)
	#define DBG_FUNC_RETURN4(retVal, strFmt, args...)  return (retVal)
	#define DBG_FUNC_RETURN_VOID4(strFmt, args...)  return
	#endif  /* 4 */

	#if DBG_PRINT_LVL >= 5
	#define DBG_FUNC_ENTER5  DBG_FUNC_ENTER
	#define DBG_FUNC_RETURN5  DBG_FUNC_RETURN
	#define DBG_FUNC_RETURN_VOID5  DBG_FUNC_RETURN_VOID
	#else
	#define DBG_FUNC_ENTER5(strFmt, args...)
	#define DBG_FUNC_RETURN5(retVal, strFmt, args...)  return (retVal)
	#define DBG_FUNC_RETURN_VOID5(strFmt, args...)  return
	#endif  /* 5 */

	#if DBG_PRINT_LVL >= 6
	#define DBG_FUNC_ENTER6  DBG_FUNC_ENTER
	#define DBG_FUNC_RETURN6  DBG_FUNC_RETURN
	#define DBG_FUNC_RETURN_VOID6  DBG_FUNC_RETURN_VOID
	#else
	#define DBG_FUNC_ENTER6(strFmt, args...)
	#define DBG_FUNC_RETURN6(retVal, strFmt, args...)  return (retVal)
	#define DBG_FUNC_RETURN_VOID6(strFmt, args...)  return
	#endif  /* 6 */

	#if DBG_PRINT_LVL >= 7
	#define DBG_FUNC_ENTER7  DBG_FUNC_ENTER
	#define DBG_FUNC_RETURN7  DBG_FUNC_RETURN
	#define DBG_FUNC_RETURN_VOID7  DBG_FUNC_RETURN_VOID
	#else
	#define DBG_FUNC_ENTER7(strFmt, args...)
	#define DBG_FUNC_RETURN7(retVal, strFmt, args...)  return (retVal)
	#define DBG_FUNC_RETURN_VOID7(strFmt, args...)  return
	#endif  /* 7 */

	#if DBG_PRINT_LVL >= 8
	#define DBG_FUNC_ENTER8  DBG_FUNC_ENTER
	#define DBG_FUNC_RETURN8  DBG_FUNC_RETURN
	#define DBG_FUNC_RETURN_VOID8  DBG_FUNC_RETURN_VOID
	#else
	#define DBG_FUNC_ENTER8(strFmt, args...)
	#define DBG_FUNC_RETURN8(retVal, strFmt, args...)  return (retVal)
	#define DBG_FUNC_RETURN_VOID8(strFmt, args...)  return
	#endif  /* 8 */

	#if DBG_PRINT_LVL >= 9
	#define DBG_FUNC_ENTER9  DBG_FUNC_ENTER
	#define DBG_FUNC_RETURN9  DBG_FUNC_RETURN
	#define DBG_FUNC_RETURN_VOID9  DBG_FUNC_RETURN_VOID
	#else
	#define DBG_FUNC_ENTER9(strFmt, args...)
	#define DBG_FUNC_RETURN9(retVal, strFmt, args...)  return (retVal)
	#define DBG_FUNC_RETURN_VOID9(strFmt, args...)  return
	#endif  /* 9 */

#endif

#define DBG_FUNC_DEPTH_GET()  dbgFuncDepthGet()

/*
  asserting.

  macros:
    ASSERT(),
    ASSERT_MSG(),
    DBG_ASSERT(),
    DBG_ASSERT_MSG(),
*/
#undef ASSERT
#undef ASSERT_MSG
#undef DBG_ASSERT
#undef DBG_ASSERT_MSG

#define ASSERT(expr)  do { \
	if ( !(expr) ) { \
		dbgAssert(__FILE__, __LINE__, NULL); \
	} \
} while (0)

#define ASSERT_MSG(expr, msgStr, args...)  do { \
	if ( !(expr) ) { \
		dbgAssert(__FILE__, __LINE__, msgStr, ##args); \
	} \
} while (0)

#if DBG_ASSERT_EN == 0
#define DBG_ASSERT(expr)
#define DBG_ASSERT_MSG(expr, msgStr)

#elif DBG_ASSERT_EN == 1
#define DBG_ASSERT  ASSERT
#define DBG_ASSERT_MSG  ASSERT_MSG

#endif

/*
  printing.

  macros:
    TAG(),
    TAG_MSG(),
    DBG_PRINT(),
    DBG_PRINTn(), n=0~9,
    DBG_PRINT_PAUSE(),
    DBG_PRINT_PAUSEn(), n=0~9,
    DBG_LINE_PRINT(),
    DBG_LINE_PRINTn(), n=0~9,
    DBG_MSK_PRINT(),
    DBG_MSK_PRINTn(), n=0~9,
    DBG_MEM_DUMP(),
    DBG_MEM_DUMPn(), n=0~9,
    DBG_REG_PRINT(),
    DBG_REG_PRINTn(), n=0~9,
    DBG_TAG(),
    DBG_TAGn(), n=0~9.
*/
#if 1
#define TAG_STR  __FUNCTION__
#else
#define TAG_STR  __FILE__
#endif

#undef TAG
#define TAG() \
	printf("%s[%d]\n", TAG_STR, __LINE__)

#undef TAG_MSG
#define TAG_MSG(strFmt, args...) do { \
	printf("%s[%d] ", TAG_STR, __LINE__); \
	printf(strFmt, ##args); \
	printf("\n"); \
} while (0)

#if DBG_PRINT_EN == 0
#define DBG_PRINT(strFmt, args...)
#define DBG_PRINT0(strFmt, args...)
#define DBG_PRINT1(strFmt, args...)
#define DBG_PRINT2(strFmt, args...)
#define DBG_PRINT3(strFmt, args...)
#define DBG_PRINT4(strFmt, args...)
#define DBG_PRINT5(strFmt, args...)
#define DBG_PRINT6(strFmt, args...)
#define DBG_PRINT7(strFmt, args...)
#define DBG_PRINT8(strFmt, args...)
#define DBG_PRINT9(strFmt, args...)

#define DBG_PRINT_PAUSE(strFmt, args...)
#define DBG_PRINT_PAUSE0(strFmt, args...)
#define DBG_PRINT_PAUSE1(strFmt, args...)
#define DBG_PRINT_PAUSE2(strFmt, args...)
#define DBG_PRINT_PAUSE3(strFmt, args...)
#define DBG_PRINT_PAUSE4(strFmt, args...)
#define DBG_PRINT_PAUSE5(strFmt, args...)
#define DBG_PRINT_PAUSE6(strFmt, args...)
#define DBG_PRINT_PAUSE7(strFmt, args...)
#define DBG_PRINT_PAUSE8(strFmt, args...)
#define DBG_PRINT_PAUSE9(strFmt, args...)

#define DBG_LINE_PRINT(strFmt, args...)
#define DBG_LINE_PRINT0(strFmt, args...)
#define DBG_LINE_PRINT1(strFmt, args...)
#define DBG_LINE_PRINT2(strFmt, args...)
#define DBG_LINE_PRINT3(strFmt, args...)
#define DBG_LINE_PRINT4(strFmt, args...)
#define DBG_LINE_PRINT5(strFmt, args...)
#define DBG_LINE_PRINT6(strFmt, args...)
#define DBG_LINE_PRINT7(strFmt, args...)
#define DBG_LINE_PRINT8(strFmt, args...)
#define DBG_LINE_PRINT9(strFmt, args...)

#define DBG_MSK_PRINT(msk, strFmt, args...)
#define DBG_MSK_PRINT0(msk, strFmt, args...)
#define DBG_MSK_PRINT1(msk, strFmt, args...)
#define DBG_MSK_PRINT2(msk, strFmt, args...)
#define DBG_MSK_PRINT3(msk, strFmt, args...)
#define DBG_MSK_PRINT4(msk, strFmt, args...)
#define DBG_MSK_PRINT5(msk, strFmt, args...)
#define DBG_MSK_PRINT6(msk, strFmt, args...)
#define DBG_MSK_PRINT7(msk, strFmt, args...)
#define DBG_MSK_PRINT8(msk, strFmt, args...)
#define DBG_MSK_PRINT9(msk, strFmt, args...)

#define DBG_MEM_DUMP(addr, size, titleStr)
#define DBG_MEM_DUMP0(addr, size, titleStr)
#define DBG_MEM_DUMP1(addr, size, titleStr)
#define DBG_MEM_DUMP2(addr, size, titleStr)
#define DBG_MEM_DUMP3(addr, size, titleStr)
#define DBG_MEM_DUMP4(addr, size, titleStr)
#define DBG_MEM_DUMP5(addr, size, titleStr)
#define DBG_MEM_DUMP6(addr, size, titleStr)
#define DBG_MEM_DUMP7(addr, size, titleStr)
#define DBG_MEM_DUMP8(addr, size, titleStr)
#define DBG_MEM_DUMP9(addr, size, titleStr)

#define DBG_REG_PRINT(addr, titleStr)
#define DBG_REG_PRINT0(addr, titleStr)
#define DBG_REG_PRINT1(addr, titleStr)
#define DBG_REG_PRINT2(addr, titleStr)
#define DBG_REG_PRINT3(addr, titleStr)
#define DBG_REG_PRINT4(addr, titleStr)
#define DBG_REG_PRINT5(addr, titleStr)
#define DBG_REG_PRINT6(addr, titleStr)
#define DBG_REG_PRINT7(addr, titleStr)
#define DBG_REG_PRINT8(addr, titleStr)
#define DBG_REG_PRINT9(addr, titleStr)

#define DBG_TAG()
#define DBG_TAG0()
#define DBG_TAG1()
#define DBG_TAG2()
#define DBG_TAG3()
#define DBG_TAG4()
#define DBG_TAG5()
#define DBG_TAG6()
#define DBG_TAG7()
#define DBG_TAG8()
#define DBG_TAG9()

#elif DBG_PRINT_EN == 1
#define DBG_PRINT(strFmt, args...) \
	dbgPrint(strFmt, ##args)

#define DBG_PRINT_PAUSE(strFmt, args...) do { \
	dbgPrint(strFmt, ##args); \
	dbgPrint("  ... press any key to continue ...\n"); \
	getch(); \
} while (0)

#define DBG_LINE_PRINT(strFmt, args...) \
	dbgLinePrint(__FILE__, __LINE__, strFmt, ##args)

#define DBG_MSK_PRINT(msk, strFmt, args...) \
	dbgMskPrint(msk, strFmt, ##args)

#define DBG_MEM_DUMP(addr, size, titleStr) \
	dbgMemDump(__FILE__, __LINE__, addr, size, titleStr)

#define DBG_REG_PRINT(addr, titleStr) \
	dbgRegPrint(addr, titleStr)

#define DBG_TAG() \
	printf("%s[%d]\n", __FILE__, __LINE__)

	#define DBG_PRINT0  DBG_PRINT
	#define DBG_PRINT_PAUSE0  DBG_PRINT_PAUSE
	#define DBG_LINE_PRINT0  DBG_LINE_PRINT
	#define DBG_MSK_PRINT0  DBG_MSK_PRINT
	#define DBG_MEM_DUMP0  DBG_MEM_DUMP
	#define DBG_REG_PRINT0  DBG_REG_PRINT
	#define DBG_TAG0  DBG_TAG

	#if DBG_PRINT_LVL >= 1
	#define DBG_PRINT1  DBG_PRINT
	#define DBG_PRINT_PAUSE1  DBG_PRINT_PAUSE
	#define DBG_LINE_PRINT1  DBG_LINE_PRINT
	#define DBG_MSK_PRINT1  DBG_MSK_PRINT
	#define DBG_MEM_DUMP1  DBG_MEM_DUMP
	#define DBG_REG_PRINT1  DBG_REG_PRINT
	#define DBG_TAG1  DBG_TAG
	#else
	#define DBG_PRINT1(strFmt, args...)
	#define DBG_PRINT_PAUSE1(strFmt, args...)
	#define DBG_LINE_PRINT1(strFmt, args...)
	#define DBG_MSK_PRINT1(msk, strFmt, args...)
	#define DBG_MEM_DUMP1(addr, size, titleStr)
	#define DBG_REG_PRINT1(addr, titleStr)
	#define DBG_TAG1()
	#endif  /* 1 */

	#if DBG_PRINT_LVL >= 2
	#define DBG_PRINT2  DBG_PRINT
	#define DBG_PRINT_PAUSE2  DBG_PRINT_PAUSE
	#define DBG_LINE_PRINT2  DBG_LINE_PRINT
	#define DBG_MSK_PRINT2  DBG_MSK_PRINT
	#define DBG_MEM_DUMP2  DBG_MEM_DUMP
	#define DBG_REG_PRINT2  DBG_REG_PRINT
	#define DBG_TAG2  DBG_TAG
	#else
	#define DBG_PRINT2(strFmt, args...)
	#define DBG_PRINT_PAUSE2(strFmt, args...)
	#define DBG_LINE_PRINT2(strFmt, args...)
	#define DBG_MSK_PRINT2(msk, strFmt, args...)
	#define DBG_MEM_DUMP2(addr, size, titleStr)
	#define DBG_REG_PRINT2(addr, titleStr)
	#define DBG_TAG2()
	#endif  /* 2 */

	#if DBG_PRINT_LVL >= 3
	#define DBG_PRINT3  DBG_PRINT
	#define DBG_PRINT_PAUSE3  DBG_PRINT_PAUSE
	#define DBG_LINE_PRINT3  DBG_LINE_PRINT
	#define DBG_MSK_PRINT3  DBG_MSK_PRINT
	#define DBG_MEM_DUMP3  DBG_MEM_DUMP
	#define DBG_REG_PRINT3  DBG_REG_PRINT
	#define DBG_TAG3  DBG_TAG
	#else
	#define DBG_PRINT3(strFmt, args...)
	#define DBG_PRINT_PAUSE3(strFmt, args...)
	#define DBG_LINE_PRINT3(strFmt, args...)
	#define DBG_MSK_PRINT3(msk, strFmt, args...)
	#define DBG_MEM_DUMP3(addr, size, titleStr)
	#define DBG_REG_PRINT3(addr, titleStr)
	#define DBG_TAG3()
	#endif  /* 3 */

	#if DBG_PRINT_LVL >= 4
	#define DBG_PRINT4  DBG_PRINT
	#define DBG_PRINT_PAUSE4  DBG_PRINT_PAUSE
	#define DBG_LINE_PRINT4  DBG_LINE_PRINT
	#define DBG_MSK_PRINT4  DBG_MSK_PRINT
	#define DBG_MEM_DUMP4  DBG_MEM_DUMP
	#define DBG_REG_PRINT4  DBG_REG_PRINT
	#define DBG_TAG4  DBG_TAG
	#else
	#define DBG_PRINT4(strFmt, args...)
	#define DBG_PRINT_PAUSE4(strFmt, args...)
	#define DBG_LINE_PRINT4(strFmt, args...)
	#define DBG_MSK_PRINT4(msk, strFmt, args...)
	#define DBG_MEM_DUMP4(addr, size, titleStr)
	#define DBG_REG_PRINT4(addr, titleStr)
	#define DBG_TAG4()
	#endif  /* 4 */

	#if DBG_PRINT_LVL >= 5
	#define DBG_PRINT5  DBG_PRINT
	#define DBG_PRINT_PAUSE5  DBG_PRINT_PAUSE
	#define DBG_LINE_PRINT5  DBG_LINE_PRINT
	#define DBG_MSK_PRINT5  DBG_MSK_PRINT
	#define DBG_MEM_DUMP5  DBG_MEM_DUMP
	#define DBG_REG_PRINT5  DBG_REG_PRINT
	#define DBG_TAG5  DBG_TAG
	#else
	#define DBG_PRINT5(strFmt, args...)
	#define DBG_PRINT_PAUSE5(strFmt, args...)
	#define DBG_LINE_PRINT5(strFmt, args...)
	#define DBG_MSK_PRINT5(msk, strFmt, args...)
	#define DBG_MEM_DUMP5(addr, size, titleStr)
	#define DBG_REG_PRINT5(addr, titleStr)
	#define DBG_TAG5()
	#endif  /* 5 */

	#if DBG_PRINT_LVL >= 6
	#define DBG_PRINT6  DBG_PRINT
	#define DBG_PRINT_PAUSE6  DBG_PRINT_PAUSE
	#define DBG_LINE_PRINT6  DBG_LINE_PRINT
	#define DBG_MSK_PRINT6  DBG_MSK_PRINT
	#define DBG_MEM_DUMP6  DBG_MEM_DUMP
	#define DBG_REG_PRINT6  DBG_REG_PRINT
	#define DBG_TAG6  DBG_TAG
	#else
	#define DBG_PRINT6(strFmt, args...)
	#define DBG_PRINT_PAUSE6(strFmt, args...)
	#define DBG_LINE_PRINT6(strFmt, args...)
	#define DBG_MSK_PRINT6(msk, strFmt, args...)
	#define DBG_MEM_DUMP6(addr, size, titleStr)
	#define DBG_REG_PRINT6(addr, titleStr)
	#define DBG_TAG6()
	#endif  /* 6 */

	#if DBG_PRINT_LVL >= 7
	#define DBG_PRINT7  DBG_PRINT
	#define DBG_PRINT_PAUSE7  DBG_PRINT_PAUSE
	#define DBG_LINE_PRINT7  DBG_LINE_PRINT
	#define DBG_MSK_PRINT7  DBG_MSK_PRINT
	#define DBG_MEM_DUMP7  DBG_MEM_DUMP
	#define DBG_REG_PRINT7  DBG_REG_PRINT
	#define DBG_TAG7  DBG_TAG
	#else
	#define DBG_PRINT7(strFmt, args...)
	#define DBG_PRINT_PAUSE7(strFmt, args...)
	#define DBG_LINE_PRINT7(strFmt, args...)
	#define DBG_MSK_PRINT7(msk, strFmt, args...)
	#define DBG_MEM_DUMP7(addr, size, titleStr)
	#define DBG_REG_PRINT7(addr, titleStr)
	#define DBG_TAG7()
	#endif  /* 7 */

	#if DBG_PRINT_LVL >= 8
	#define DBG_PRINT8  DBG_PRINT
	#define DBG_PRINT_PAUSE8  DBG_PRINT_PAUSE
	#define DBG_LINE_PRINT8  DBG_LINE_PRINT
	#define DBG_MSK_PRINT8  DBG_MSK_PRINT
	#define DBG_MEM_DUMP8  DBG_MEM_DUMP
	#define DBG_REG_PRINT8  DBG_REG_PRINT
	#define DBG_TAG8  DBG_TAG
	#else
	#define DBG_PRINT8(strFmt, args...)
	#define DBG_PRINT_PAUSE8(strFmt, args...)
	#define DBG_LINE_PRINT8(strFmt, args...)
	#define DBG_MSK_PRINT8(msk, strFmt, args...)
	#define DBG_MEM_DUMP8(addr, size, titleStr)
	#define DBG_REG_PRINT8(addr, titleStr)
	#define DBG_TAG8()
	#endif  /* 8 */

	#if DBG_PRINT_LVL >= 9
	#define DBG_PRINT9  DBG_PRINT
	#define DBG_PRINT_PAUSE9  DBG_PRINT_PAUSE
	#define DBG_LINE_PRINT9  DBG_LINE_PRINT
	#define DBG_MSK_PRINT9  DBG_MSK_PRINT
	#define DBG_MEM_DUMP9  DBG_MEM_DUMP
	#define DBG_REG_PRINT9  DBG_REG_PRINT
	#define DBG_TAG9  DBG_TAG
	#else
	#define DBG_PRINT9(strFmt, args...)
	#define DBG_PRINT_PAUSE9(strFmt, args...)
	#define DBG_LINE_PRINT9(strFmt, args...)
	#define DBG_MSK_PRINT9(msk, strFmt, args...)
	#define DBG_MEM_DUMP9(addr, size, titleStr)
	#define DBG_REG_PRINT9(addr, titleStr)
	#define DBG_TAG9()
	#endif  /* 9 */

#endif

/*
  gpio setting.

  macros:
    DBG_GPIO_CFG(),      DBG_GPIO_SET(),
    DBG_GPIO_TGL_CFG(),  DBG_GPIO_TGL_SET(),
    DBG_GPIO_TGH_CFG(),  DBG_GPIO_TGH_SET(),
    DBG_GPIO_FML_CFG(),  DBG_GPIO_FML_SET(),
    DBG_GPIO_FMH_CFG(),  DBG_GPIO_FMH_SET(),
    DBG_GPIO_GEN_CFG(),  DBG_GPIO_GEN_SET(),
    DBG_GPIO_DISP_CFG(), DBG_GPIO_DISP_SET().
*/
#if DBG_GPIO_EN == 0
#define DBG_GPIO_CFG(grpId, pinMsk)
#define DBG_GPIO_TGL_CFG(pinMsk)
#define DBG_GPIO_TGH_CFG(pinMsk)
#define DBG_GPIO_FML_CFG(pinMsk)
#define DBG_GPIO_FMH_CFG(pinMsk)
#define DBG_GPIO_GEN_CFG(pinMsk)
#define DBG_GPIO_DISP_CFG(pinMsk)
#define DBG_GPIO_SET(grpId, pinMsk, val)
#define DBG_GPIO_TGL_SET(pinMsk, val)
#define DBG_GPIO_TGH_SET(pinMsk, val)
#define DBG_GPIO_FML_SET(pinMsk, val)
#define DBG_GPIO_FMH_SET(pinMsk, val)
#define DBG_GPIO_GEN_SET(pinMsk, val)
#define DBG_GPIO_DISP_SET(pinMsk, val)

#elif DBG_GPIO_EN == 1
#define DBG_GPIO_CFG(grpId, pinMsk) \
	dbgGpioCfg(grpId, pinMsk)

#define DBG_GPIO_TGL_CFG(pinMsk) \
	dbgGpioTglCfg(pinMsk)

#define DBG_GPIO_TGH_CFG(pinMsk) \
	dbgGpioTghCfg(pinMsk)

#define DBG_GPIO_FML_CFG(pinMsk) \
	dbgGpioFmlCfg(pinMsk)

#define DBG_GPIO_FMH_CFG(pinMsk) \
	dbgGpioFmhCfg(pinMsk)

#define DBG_GPIO_GEN_CFG(pinMsk) \
	dbgGpioGenCfg(pinMsk)

#define DBG_GPIO_DISP_CFG(pinMsk) \
	dbgGpioDispCfg(pinMsk)

#define DBG_GPIO_SET(grpId, pinMsk, val) \
	dbgGpioSet(grpId, pinMsk, val)

#define DBG_GPIO_TGL_SET(pinMsk, val) \
	dbgGpioTglSet(pinMsk, val)

#define DBG_GPIO_TGH_SET(pinMsk, val) \
	dbgGpioTghSet(pinMsk, val)

#define DBG_GPIO_FML_SET(pinMsk, val) \
	dbgGpioFmlSet(pinMsk, val)

#define DBG_GPIO_FMH_SET(pinMsk, val) \
	dbgGpioFmhSet(pinMsk, val)

#define DBG_GPIO_GEN_SET(pinMsk, val) \
	dbgGpioGenSet(pinMsk, val)

#define DBG_GPIO_DISP_SET(pinMsk, val) \
	dbgGpioDispSet(pinMsk, val)

#endif

/* time measure. */
/* in one api only. */
#define DBG_TIME_DECLARE(depth) \
	UINT32 _tim__[depth], _lin__[depth]; \
	UINT32 _cnt__ = 0; \

#define DBG_TIME_RESET() \
	_cnt__ = 0; \

#define DBG_TIME_RESET_GET() \
	DBG_TIME_RESET(); \
	DBG_TIME_GET(); \

#define DBG_TIME_GET() \
	_lin__[_cnt__] = __LINE__; \
	_tim__[_cnt__++] = dbgTimeGet(); \

#define DBG_TIME_GET_DUMP() \
	DBG_TIME_GET(); \
	dbgTimeDump(__FUNCTION__, _tim__, _lin__, _cnt__, 0); \

#define DBG_TIME_TOT_GET() \
	dbgTimeTotGet(_tim__, _cnt__); \

/* across more than two apis. */
#define DBG_GLOB_TIME_DECLARE(depth, var) \
	UINT32 var##DbgTime__[depth], var##DbgLine__[depth]; \
	UINT32 var##DbgCnt__ = 0; \
	UINT32 var##DbgRnd__ = 1; \

#define DBG_GLOB_TIME_REFER(var) \
	extern UINT32 var##DbgTime__[], var##DbgLine__[]; \
	extern UINT32 var##DbgCnt__; \
	extern UINT32 var##DbgRnd__; \

#define DBG_GLOB_TIME_RESET(var) \
	var##DbgCnt__ = 0; \

#define DBG_GLOB_TIME_RESET_GET(var) \
	DBG_GLOB_TIME_RESET(var); \
	DBG_GLOB_TIME_GET(var); \

#define DBG_GLOB_TIME_GET(var) \
	var##DbgLine__[var##DbgCnt__] = __LINE__; \
	var##DbgTime__[var##DbgCnt__++] = dbgTimeGet(); \

#define DBG_GLOB_TIME_GET_DUMP(var) \
	DBG_GLOB_TIME_GET(var); \
	dbgTimeDump(__FUNCTION__, var##DbgTime__, var##DbgLine__, var##DbgCnt__, var##DbgRnd__); \
	var##DbgRnd__++;

/*******************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*******************************************************************************/
/*
  dbg_assert.c, DO NOT USE THESE FUNCTION DIRECTLY!
  USE MACRO DBG_ASSERT().
*/
void dbgAssert(const char *fileName, UINT32 lineNum, const char *msgStr, ...)
	__attribute__ ((format (printf, 3, 4)));

/*
  dbg_func_trace.c, DO NOT USE THESE FUNCTION DIRECTLY!
  USE MACRO DBG_FUNC_ENTER(), DBG_FUNC_RETURN() and DBG_FUNC_RETURN_VOID().
*/
void dbgFuncEnter(const char *funcName, UINT32 lineNum, const char *strFmt, ...)
	__attribute__ ((format (printf, 3, 4)));

void dbgFuncReturn(const char *funcName, UINT32 lineNum, const char *strFmt, ...)
	__attribute__ ((format (printf, 3, 4)));

/*
  dbg_func_trace_get.c, DO NOT USE THESE FUNCTION DIRECTLY!
  USE MACRO DBG_FUNC_DEPTH_GET().
*/
SINT32 dbgFuncDepthGet(void);

/*
  dbg_gpio.c, DO NOT USE THESE FUNCTION DIRECTLY!
  USE MACRO DBG_GPIO_SET().
*/
void dbgGpioCfg(UINT32 grpId, UINT32 pinMsk);
void dbgGpioSet(UINT32 grpId, UINT32 pinMsk, UINT32 val);

/*
  dbg_gpio_disp.c, DO NOT USE THESE FUNCTION DIRECTLY!
  USE MACRO DBG_GPIO_DISP_SET().
*/
void dbgGpioDispCfg(UINT32 pinMsk);
void dbgGpioDispSet(UINT32 pinMsk, UINT32 val);

/*
  dbg_gpio_fmh.c, DO NOT USE THESE FUNCTION DIRECTLY!
  USE MACRO DBG_GPIO_FMH_SET().
*/
void dbgGpioFmhCfg(UINT32 pinMsk);
void dbgGpioFmhSet(UINT32 pinMsk, UINT32 val);

/*
  dbg_gpio_fml.c, DO NOT USE THESE FUNCTION DIRECTLY!
  USE MACRO DBG_GPIO_FML_SET().
*/
void dbgGpioFmlCfg(UINT32 pinMsk);
void dbgGpioFmlSet(UINT32 pinMsk, UINT32 val);

/*
  dbg_gpio_gen.c, DO NOT USE THESE FUNCTION DIRECTLY!
  USE MACRO DBG_GPIO_GEN_SET().
*/
void dbgGpioGenCfg(UINT32 pinMsk);
void dbgGpioGenSet(UINT32 pinMsk, UINT32 val);

/*
  dbg_gpio_tgh.c, DO NOT USE THESE FUNCTION DIRECTLY!
  USE MACRO DBG_GPIO_TGH_SET().
*/
void dbgGpioTghCfg(UINT32 pinMsk);
void dbgGpioTghSet(UINT32 pinMsk, UINT32 val);

/*
  dbg_gpio_tgl.c, DO NOT USE THESE FUNCTION DIRECTLY!
  USE MACRO DBG_GPIO_TGL_SET().
*/
void dbgGpioTglCfg(UINT32 pinMsk);
void dbgGpioTglSet(UINT32 pinMsk, UINT32 val);

/*
  dbg_line_print.c, DO NOT USE THESE FUNCTION DIRECTLY!
  USE MACRO DBG_LINE_PRINT().
*/
void dbgLinePrint(const char *fileName, UINT32 lineNum, const char *strFmt, ...)
	__attribute__ ((format (printf, 3, 4)));

/*
  dbg_mem_dump.c, DO NOT USE THESE FUNCTION DIRECTLY!
  USE MACRO DBG_MEM_DUMP().
*/
void dbgMemDump(const char *fileName, UINT32 lineNum, const void *addr, UINT32 size, const char *titleStr);

/*
  dbg_msk_print.c, DO NOT USE THESE FUNCTION DIRECTLY!
  USE MACRO DBG_MSK_PRINT().
*/
void dbgMskPrint(UINT32 msk, const char *strFmt, ...)
	__attribute__ ((format (printf, 2, 3)));

/*
  dbg_print.c, DO NOT USE THESE FUNCTION DIRECTLY!
  USE MACRO DBG_PRINT().
*/
void dbgPrint(const char *strFmt, ...)
	__attribute__ ((format (printf, 1, 2)));

/*
  dbg_reg_print.c, DO NOT USE THESE FUNCTION DIRECTLY!
  USE MACRO DBG_REG_PRINT().
*/
void dbgRegPrint(UINT32 regAddr, const char *titleStr);

/*
  dbg_time.c, DO NOT USE THESE FUNCTION DIRECTLY!
  USE MACRO DBG_TIME_GET() and DBG_TIME_GET_DUMP().
*/
UINT32 dbgTimeGet(void);
void dbgTimeDump(const char *funcName, UINT32 time[], UINT32 line[], UINT32 count, UINT32 round);
UINT32 dbgTimeTotGet(UINT32 time[], UINT32 count);


extern void callStack(void);
extern void codeSentryDump(void);
extern void cpuPcRecResultDump(void);

#endif  /* _DBG_API_H_ */

