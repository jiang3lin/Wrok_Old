/**************************************************************************
 *
 *       Copyright (c) 2012 by iCatch Technology, Inc.
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
 *  Author: Andy.Li
 *
 **************************************************************************/
#ifndef __NET_VER_H__
#define __NET_VER_H__

#define NET_RELEASE_VERSION "00.13.00"
#define NET_SVN_NUMBER "18580"

/* Inner used only */
#define NET_SVN_REVISION	"$Rev: 17759 $"
#define NET_SVN_DATE		"$Date: 2014-03-24 22:45:04 +0800 (週一, 24 三月 2014) $"

#include <../inc/version.h>

#ifndef SPCA6330
#error SPCA6330 not defined
#endif

#if !defined(BASEFW_REVISION_CODE) || BASEFW_REVISION_CODE < 18579
#error Need basefw revision 18579 or higher.
#endif

#endif
