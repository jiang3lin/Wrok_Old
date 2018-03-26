#/**************************************************************************
# *                                                                        *
# *         Copyright (c) 2002 by Sunplus Technology Co., Ltd.             *
# *                                                                        *
# *  This software is copyrighted by and is the property of Sunplus        *
# *  Technology Co., Ltd. All rights are reserved by Sunplus Technology    *
# *  Co., Ltd. This software may only be used in accordance with the       *
# *  corresponding license agreement. Any unauthorized use, duplication,   *
# *  distribution, or disclosure of this software is expressly forbidden.  *
# *                                                                        *
# *  This Copyright notice MUST not be removed or modified without prior   *
# *  written consent of Sunplus Technology Co., Ltd.                       *
# *                                                                        *
# *  Sunplus Technology Co., Ltd. reserves the right to modify this        *
# *  software without notice.                                              *
# *                                                                        *
# *  Sunplus Technology Co., Ltd.                                          *
# *  19, Innovation First Road, Science-Based Industrial Park,             *
# *  Hsin-Chu, Taiwan, R.O.C.                                              *
# *                                                                        *
# *  Author: Anmin Deng                                                    *
# *                                                                        *
# **************************************************************************/
#init empty simple flavor variables before any include
CFLAGS :=
ASFLAGS :=


HOST_ROOT := ../../../..
include $(HOST_ROOT)/host.def
ifndef PLATFORM_PATH
PLATFORM_PATH := $(PLATFORM)
endif
PLATFORM_ROOT := $(HOST_ROOT)/../$(PLATFORM_PATH)
TOOL_PATH := $(HOST_ROOT)/../tool
HOST_FW_PATH := $(HOST_ROOT)/customization/HostFw

# output
SRC_DIR   := .
DEST_DIR  := $(HOST_ROOT)/_$(PRJ_NAME)
OBJS_PATH := $(DEST_DIR)/_$(PLATFORM_PATH)

# Include the definition of tool chain
include $(PLATFORM_ROOT)/tool_chain.def

#---------------------------------------------------------------------------
# Variable definition:
#   1. Cross-compling tools
#   2. Linker script file
#   3. C complier options
#   4. Assembly compiler options
#   5. Linker options
#---------------------------------------------------------------------------
ifeq (cc,$(CC))
CC   := $(PREFIX)/gcc
AR   := $(PREFIX)/ar
NM   := $(PREFIX)/nm
ECHO := echo
endif

GOAL := app_tb.a


#CFLAGS += -gstabs+
#CFLAGS += -Wall
CFLAGS += -Os
CFLAGS += -G 8
#CFLAGS += -ansi
CFLAGS += -msoft-float
CFLAGS += -I$(PLATFORM_ROOT)/inc
CFLAGS += -I$(PLATFORM_ROOT)/inc/api
CFLAGS += -I$(HOST_ROOT)/include
CFLAGS += -I$(HOST_FW_PATH)/include

CFLAGS += -EL
CFLAGS += -fno-common

#---------------------------------------------------------------------------
# Intermediate objects
#---------------------------------------------------------------------------

OBJS += app_tb_main.o
OBJS += app_tb_util.o
OBJS += app_tb_flash.o
OBJS += app_tb_sensor.o
OBJS += app_tb_af.o
OBJS += app_tb_batcurve.o
OBJS += app_tb_msh.o
OBJS += app_tb_cipa.o

#---------------------------------------------------------------------------
# Referring to generic rules
#---------------------------------------------------------------------------
$(eval $(call GEN_GOAL_RULES))
