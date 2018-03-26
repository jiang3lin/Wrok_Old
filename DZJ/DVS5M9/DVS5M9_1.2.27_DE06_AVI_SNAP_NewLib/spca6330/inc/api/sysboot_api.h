/**************************************************************************
 *
 *        Copyright (c) 2005-2010 by Sunplus mMedia Inc., Ltd.
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
 *  Hsin-Chu, Taiwan.
 *
 *  Author: Anmin Deng
 *
 **************************************************************************/
#ifndef _SYSBOOT_API_H_
#define _SYSBOOT_API_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "common.h"


/* ----- host sys init callbacks ----- */
extern void heapInit(void);
extern void devInitPrimary(void);
extern void devInitSecondary(void);
extern void appInit(void);

/* loaded after host init, if multi boot supported */
extern void appInitDelay(void);


/**
 * \brief sysAppInitBk APIs, sysboot bk flows
 * . calls sysAppInitBkOpen(fp, prio);
 *   where "fp(hndl)" is the callback run concurrently
 *   with thread priority "prio"
 * . inside "fp(hndl)", the last thing is to call
 *   sysAppInitBkClose(hndl);
 * . calls sysAppInitWait(), typically at the beginning of host main loop.
 *   to wait for all the sysboot bk init threads callback finished
 *   (ie all the callbacks by sysAppInitBkOpen(fp, prio);
 *   where all the "fp(hndl)" had called sysAppInitBkClose(hndl);).
 * . if a sysboot bk init callback depends on other sysboot bk threads,
 *   it should wait for other bk threads to finish by
 *   sysAppInitBkWait(UINT32 hndl); where hndl can be bitwise-OR from
 *   the params "hndl" in the callbacks "fp"
 */
void sysAppInitBkOpen(void (*fp)(UINT32 hndl), UINT32 priority);
void sysAppInitBkClose(UINT32 hndl);
void sysAppInitBkWait(UINT32 hndl);
void sysAppInitWait(void);


#ifdef __cplusplus
}
#endif

#endif /*_SYSBOOT_API_H_*/
