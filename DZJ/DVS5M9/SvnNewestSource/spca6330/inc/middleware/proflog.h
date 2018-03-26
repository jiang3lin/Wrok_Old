/**************************************************************************
 *                                                                        *
 *         Copyright (c) 2007 by Sunplus Technology Co., Ltd.             *
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
 *  Author: Paul May                                                      *
 *                                                                        *
 **************************************************************************/
/* DO NOT EDIT the next 2 lines for auto revision by subversion
$Rev: 6955 $
$Date:: 2011-02-25 23:32:16#$
 */

#ifndef _PROF_LOG_H_
#define _PROF_LOG_H_

/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/
#include "common.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define SYS_PROF_ID      0

typedef enum {
	PROF_LOG_MODE_KEEP_OLD, /* manual trigger at log init until full */
	PROF_LOG_MODE_KEEP_NEW, /* auto wrapping */
} profLogMode_t;

/**************************************************************************
 *                               M A C R O S                              *
 **************************************************************************/
#undef PROF_LOG_INIT
#undef PROF_LOG_ADD
#undef PROF_LOG_PRINT
#undef PROF_LOG_CALLSTACK
#undef PROF_LOG_DUMP
#undef PROF_LOG_TERM
#if PROF_LOG_EN
#define PROF_LOG_INIT(id,size)          profLogInit(id,size)
#define PROF_LOG_ADD(id,msg)            profLogAddN(id,msg,sizeof(msg)-1)
#define PROF_LOG_ADDN(id,msg,n)         profLogAddN(id,msg,n)
#define PROF_LOG_PRINT(id,str,args...)  profLogPrintf(id,str,##args)
#define PROF_LOG_CALLSTACK(id)          profLogCallStack(id)
#define PROF_LOG_DUMP(id)               profLogDump(id)
#define PROF_LOG_TERM(id)               profLogTerm(id)
#else
#define PROF_LOG_INIT(id,size)
#define PROF_LOG_ADD(id,msg)
#define PROF_LOG_ADDN(id,msg,n)
#define PROF_LOG_PRINT(id,str,args...)
#define PROF_LOG_CALLSTACK(id)
#define PROF_LOG_DUMP(id)
#define PROF_LOG_TERM(id)
#endif

#define SYS_PROF_LOG_ADD(s)             profLogAddN(SYS_PROF_ID,s,sizeof(s)-1)
#define SYS_PROF_LOG_ADDN(s,n)          profLogAddN(SYS_PROF_ID,s,n)
#define SYS_PROF_LOG_PRINT(s,args...)   profLogPrintf(SYS_PROF_ID,s,##args)
#define SYS_PROF_LOG_CALLSTACK()        profLogCallStack(SYS_PROF_ID)
#define SYS_PROF_LOG_DUMP()             profLogDump(SYS_PROF_ID)

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/* usage:
	init: profLogInit(id, 4096) to allocate buffer. id is 0-3

	run:  profLogAdd(id, "start Op A") to keep a message and calling time (us)

	end:  profLogDump(id) to print all log through UART.

	release: profLogTerm(id) to free memory
 */

UINT32 profLogOpen(void);
UINT32 profLogInit(UINT32 id, UINT32 size);
UINT32 profLogModeSet(UINT32 id, UINT32 mode);
UINT32 profLogAdd(UINT32 id, const char *msg);
UINT32 profLogAddN(UINT32 id, const char *msg, UINT32 n);
UINT32 profLogPrintf(UINT32 slot, const char *format, ...)
			__attribute__ ((format (printf, 2, 3)));
UINT32 profLogCallStack(UINT32 id);
UINT32 profLogDump(UINT32 id);
void   profLogDumpAll(void);
UINT32 profLogTerm(UINT32 id);

#endif  /* _PROF_LOG_H_ */
