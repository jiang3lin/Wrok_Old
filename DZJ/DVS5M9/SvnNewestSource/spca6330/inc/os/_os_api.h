/**************************************************************************
 *                                                                        *
 *         Copyright (c) 2006 by Sunplus Technology Co., Ltd.             *
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
 *  Author: Stephen Hung                                                  *
 *                                                                        *
 **************************************************************************/
#ifndef _OS_API__H_
#define _OS_API__H_

#ifdef   __cplusplus
extern   "C" {
#endif

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define TINY_STACK_SIZE   256
#define SMALL_STACK_SIZE  512
#define NORMAL_STACK_SIZE 1024
#define LARGE_STACK_SIZE  2048
#define HUGE_STACK_SIZE   4096

/*_________________________________________________________________________

  Memory allocation/free APIs
  _________________________________________________________________________
*/
UINT32 osMemFree(void *pointer);
void   osCustomMemBlkInit(const UINT32 blkprof[][2]);
void   osCustomMemByteInit(UINT32 size);
void*  osCustomMemAlloc(UINT32 size);
void*  osCustomMemAllocCache(UINT32 size);
void*  osCustomMemAllocExt(UINT32 size);
void*  osCustomMemAllocCacheExt(UINT32 size);

#ifdef   __cplusplus
}
#endif

#endif /* _OS_API__H_ */
