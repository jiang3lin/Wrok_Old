/*
* Copyright (c) 2001-2003 Swedish Institute of Computer Science.
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice,
*    this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright notice,
*    this list of conditions and the following disclaimer in the documentation
*
*    and/or other materials provided with the distribution.
* 3. The name of the author may not be used to endorse or promote products
*    derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
* SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
*  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
* OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
*INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
* OF SUCH DAMAGE.
*
* This file is part of the lwIP TCP/IP stack.
* Author: Adam Dunkels <adam@sics.se>
*
*/

#ifndef __CC_H__
#define __CC_H__

#include <sys/time.h>
#include <stdio.h>
#include <ndk_types.h>
#include <sp5k_os_api.h>
#include <sp5k_dbg_api.h>

#ifdef __cplusplus
extern "C" {
#endif

UINT32 profLogPrintf(UINT32 slot, const char *format, ...);
UINT32 profLogAddN(UINT32 id, const char *msg, UINT32 n);

typedef u32_t			mem_ptr_t;

/* Define (sn)printf formatters for these lwIP types */
#define U16_F "hu"
#define S16_F "hd"
#define X16_F "hx"
#define U32_F "u"
#define S32_F "d"
#define X32_F "x"

#define BYTE_ORDER		LITTLE_ENDIAN

#define PACK_STRUCT_BEGIN
#define PACK_STRUCT_STRUCT /*__attribute__ ((__packed__))*/
#define PACK_STRUCT_END
#define PACK_STRUCT_FIELD(x) x

#define LWIP_PLATFORM_DIAG(x)	printf x

#ifndef LWIP_NOASSERT
	#define LWIP_PLATFORM_ASSERT(expr)	HOST_ASSERT(expr);
#else
	#define LWIP_PLATFORM_ASSERT(expr)
#endif

#define LWIP_TIMEVAL_PRIVATE 0

#ifdef __cplusplus
}
#endif

#endif /* __CC_H__ */

