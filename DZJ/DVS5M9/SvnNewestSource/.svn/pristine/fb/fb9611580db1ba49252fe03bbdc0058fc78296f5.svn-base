#/**************************************************************************
# *                                                                        *
# *       Copyright (c) 2005-2006 by Sunplus Technology Co., Ltd.          *
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

GOAL := libsensor_custom.a

#CFLAGS += -Wall
CFLAGS += -Os
CFLAGS += -G 8
#CFLAGS += -ansi
CFLAGS += -msoft-float
CFLAGS += -I$(HOST_ROOT)/include
CFLAGS += -I$(PLATFORM_ROOT)/inc
CFLAGS += -EL
CFLAGS += -fno-common

#---------------------------------------------------------------------------
# Intermediate objects
#---------------------------------------------------------------------------
SEN_OBJS :=

# CISPLUS CMOS Image Sensor


# OmniVision CMOS Image Sensor
SEN_OBJS += model/sensor_tg_ov5610.o
SEN_OBJS += model/sensor_afe_ov5610.o
SEN_OBJS += model/sensor_tg_ov7740.o
SEN_OBJS += model/sensor_afe_ov7740.o
SEN_OBJS += model/sensor_tg_ov7740_yuv.o
SEN_OBJS += model/sensor_afe_ov7740_yuv.o
SEN_OBJS += model/sensor_tg_ov5653.o
SEN_OBJS += model/sensor_afe_ov5653.o
SEN_OBJS += model/sensor_tg_ov5633.o
SEN_OBJS += model/sensor_afe_ov5633.o
SEN_OBJS += model/sensor_tg_ov8810.o
SEN_OBJS += model/sensor_afe_ov8810.o

# Micro CMOS Image Sensor
SEN_OBJS += model/sensor_tg_mt9J003.o
SEN_OBJS += model/sensor_afe_mt9J003.o
SEN_OBJS += model/sensor_tg_mi1600.o
SEN_OBJS += model/sensor_afe_mi1600.o
SEN_OBJS += model/sensor_tg_mi1320.o
SEN_OBJS += model/sensor_afe_mi1320.o
SEN_OBJS += model/sensor_tg_mt9F002.o
SEN_OBJS += model/sensor_afe_mt9F002.o


# Others CMOS Image Sensor


# ADI AFE
#SEN_OBJS += model/sensor_afe_ad9010.o

# TI AFE
#SEN_OBJS += model/sensor_afe_vsp6244.o

# Sony CCD Image Sensor


# Sharp CCD Image Sensor


# Panasonic CCD Image Sensor
#SEN_OBJS += model/sensor_tg_vsp6244_panamn34571.o
#SEN_OBJS += model/sensor_tg_ad9010_panamn34575.o
#SEN_OBJS += model/sensor_tg_vsp6244_panamn34575.o

# TV decoder


SEN_OBJS += sensor_util.o

OBJS := $(SEN_OBJS)

#---------------------------------------------------------------------------
# Referring to generic rules
#---------------------------------------------------------------------------
$(eval $(call GEN_GOAL_RULES))
