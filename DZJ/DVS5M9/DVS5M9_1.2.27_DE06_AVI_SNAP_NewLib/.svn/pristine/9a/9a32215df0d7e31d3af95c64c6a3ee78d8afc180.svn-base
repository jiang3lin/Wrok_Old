/**************************************************************************
 *                                                                        *
 *         Copyright (c) 2012 by iCatch Technology Co., Ltd.             *
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
 *  Author: Andy.Li                                                       *
 *                                                                        *
 **************************************************************************/
#ifndef __NDK_TCPIP_H__
#define __NDK_TCPIP_H__

#include <ndk_types.h>
#include <tcpip_port.h>

#ifndef gethostname
# define gethostname	ndk_gethostname
#endif

#ifdef __cplusplus
extern "C" {
#endif

const char* eth_addr_ntoa(unsigned char *hwaddr);
int ndk_gethostname(char *name, size_t len);

#ifdef __cplusplus
}
#endif

#endif

