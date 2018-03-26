#/**************************************************************************
# *                                                                        *
# *       Copyright (c) 2002-2009 by Sunplus Technology Co., Ltd.          *
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
GOAL := app_lens.a

#CFLAGS += -Wall
CFLAGS += -mgp32
#CFLAGS += -ansi
CFLAGS += -G 8
CFLAGS += -DAPP_LUCKY7=1

CFLAGS += -I$(PLATFORM_ROOT)/inc
CFLAGS += -I$(PLATFORM_ROOT)/inc/api
CFLAGS += -I$(HOST_ROOT)/include
CFLAGS += -I./aaaopen
CFLAGS += -I./device
CFLAGS += -I./device/lens
CFLAGS += -I../../include
CFLAGS += -I./EyeFiCard/include
CFLAGS += -I./EyeFiCard/src/core
CFLAGS += -I./EyeFiCard/src/os/SP5K

CFLAGS += -G 8
CFLAGS += -DAPP_BUILD=1

CFLAGS += -I./include

CGDBFLAGS = $(CFLAGS)
CGDBFLAGS += -g

CFLAGS += -gstabs+
CFLAGS += -O2

#---------------------------------------------------------------------------
# Intermediate objects
#---------------------------------------------------------------------------
OBJS :=
OBJS += app_lens_api.o
OBJS += app_lens_io.o
OBJS += app_lens_zoom.o
OBJS += app_lens_focus.o
OBJS += app_lens_mshut.o
OBJS += app_lens_iris.o
OBJS += app_lens_debug.o
OBJS += app_lens_motor_dir.o
OBJS += app_lens_motor_md3in4.o
OBJS += app_lens_motor_dummy.o
OBJS += app_lens_motor_sh_iris.o

#---------------------------------------------------------------------------
# Referring to generic rules
#---------------------------------------------------------------------------
$(eval $(call GEN_GOAL_RULES))
