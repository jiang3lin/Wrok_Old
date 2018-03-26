#/**************************************************************************
# *
# *       Copyright (c) 2012 by iCatch Technology, Inc.
# *
# *  This software is copyrighted by and is the property of iCatch Technology,
# *  Inc.. All rights are reserved by iCatch Technology, Inc..
# *  This software may only be used in accordance with the corresponding
# *  license agreement. Any unauthorized use, duplication, distribution,
# *  or disclosure of this software is expressly forbidden.
# *
# *  This Copyright notice MUST not be removed or modified without prior
# *  written consent of iCatch Technology, Inc..
# *
# *  iCatch Technology, Inc. reserves the right to modify this software
# *  without notice.
# *
# *  iCatch Technology, Inc.
# *  19-1, Innovation First Road, Science-Based Industrial Park,
# *  Hsin-Chu, Taiwan, R.O.C.
# *
# *  Author: Andy.Li
# *
# **************************************************************************/

SRC_DIR		:= .
GOAL		:= libWiFi.a

HOST_ROOT 	:= ../../../..
PLATFORM	:= spca6330

include	$(HOST_ROOT)/host.def
PLATFORM_PATH	?= $(PLATFORM)

PLATFORM_ROOT	:= $(HOST_ROOT)/../$(PLATFORM_PATH)
include	$(PLATFORM_ROOT)/tool_chain.def

DEST_DIR	:= $(HOST_ROOT)/_$(PRJ_NAME)
#OBJS_PATH	:= $(DEST_DIR)/_$(PLATFORM_PATH)
OBJS_PATH	:= $(DEST_DIR)/objs

TARGET		:= 0
CC		:= $(PREFIX)gcc
AR		:= $(PREFIX)ar
OBJDUMP		:= $(PREFIX)objdump
OBJCOPY		:= $(PREFIX)objcopy
ECHO		:= echo

LIVEST		?= 1
WIFI_MODULE	?= rtk

#CFLAGS		+= -gstabs+
#CFLAGS		+= -Wall
CFLAGS		+= -mgp32
CFLAGS		+= -nostartfiles
CFLAGS		+= -G 0
CFLAGS		+= -EL
#CFLAGS		+= -DAPP_BUILD=1
#CFLAGS		+= -Werror
CFLAGS		+= -O2
CFLAGS		+= -std=gnu89

CFLAGS		+= -DTCPIP_LWIP
ifeq ($(LIVEST),1)
CFLAGS		+= -DICAT_STREAMING
endif

OBJS		:=
OBJS		+= app_wifi_utility.o
# net cmd move to ndk lib, removed this file  2013/07/02
#OBJS		+= app_net_cmd.o

# Include user's include directories first.
INCLUDES 	+= -I$(PLATFORM_ROOT)/inc
INCLUDES 	+= -I$(PLATFORM_ROOT)/inc/api
INCLUDES 	+= -I${PLATFORM_ROOT}/include
INCLUDES 	+= -I${PLATFORM_ROOT}/inc/middleware
ifeq ($(ICATWARE),YES)
INCLUDES 	+= -I$(HOST_ROOT)/customization/hostfw/cathyware/inc
INCLUDES 	+= -I$(HOST_ROOT)/customization/hostfw/cathyware/inc/net
INCLUDES 	+= -I$(HOST_ROOT)/customization/hostfw/cathyware/inc/net/ipv4
INCLUDES 	+= -I$(HOST_ROOT)/include
INCLUDES 	+= -I$(HOST_ROOT)/customization/hostfw/include
else
INCLUDES 	+= -I$(HOST_ROOT)/customization/hostfw/cathyware/inc
INCLUDES 	+= -I$(HOST_ROOT)/customization/hostfw/cathyware/inc/net
INCLUDES 	+= -I$(HOST_ROOT)/customization/hostfw/cathyware/inc/net/ipv4
INCLUDES 	+= -I$(HOST_ROOT)/include
INCLUDES 	+= -I$(HOST_ROOT)/customization/hostfw/include
endif

CFLAGS		+= $(OPTIMIZATION) $(INCLUDES)

#---------------------------------------------------------------------------
# Referring to generic rules
#---------------------------------------------------------------------------
$(eval $(call GEN_GOAL_RULES))

show:
	@echo CFLAGS = ${CFLAGS}
	@echo ' '
	@echo INCLUDES = ${INCLUDES}
	@echo ' '
	@echo OBJS = ${OBJS}
	@echo CC = $(CC)
	@echo HOST_ROOT = ${HOST_ROOT}
	@echo PLATFORM_ROOT = ${PLATFORM_ROOT}
