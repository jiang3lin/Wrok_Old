#/**************************************************************************
# *
# *       Copyright (c) 2002-2012 by iCatch Technology, Inc.
# *
# *  This software is copyrighted by and is the property of iCatch
# *  Technology, Inc.. All rights are reserved by iCatch Technology, Inc..
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
# *  File Description:
# *
# **************************************************************************/
#init empty simple flavor variables before any include
CFLAGS :=
ASFLAGS :=
CFG_DEFS :=
CFG_HEADERS :=
LDFLAGS :=
LDFLAGS_BSS :=
RST_OBJS :=
INI_OBJS :=
EXT_OBJS :=
GEN_OBJS :=
CUSTOM_SUBLIBS :=
CUSTOM_SUBDIRS :=
CUSTOM_LIBS :=
PRJ_LIB :=
SUBLIBS :=
SUBDIRS :=
LIBS :=
BIN :=
CDSP_BIN :=
DAT8 :=
DAT16 :=
DAT32 :=
PRJ_BIN :=
PRJ_CIQ_BIN :=
PRJ_DAT8 :=
PRJ_DAT16 :=
PRJ_DAT32 :=
SHARED_BIN :=
SHARED_DAT8 :=
SHARED_DAT16 :=
SHARED_DAT32 :=
SHARED2_BIN :=
SHARED2_DAT8 :=
SHARED2_DAT16 :=
SHARED2_DAT32 :=
SHARED3_BIN :=
SHARED3_DAT8 :=
SHARED3_DAT16 :=
SHARED3_DAT32 :=


# include & environment
HOST_MAKE := YES
HOST_ROOT := .
SRC_PATH        := customization
HOST_FW_PATH    := $(SRC_PATH)/HostFw
TOOL_PATH       := ../tool
LD_PATH         := linker_script
RES_PATH        := resource
RO_RES_PATH     := $(RES_PATH)/A/RO_RES


THIRD_PARTY_ARCSOFT := ./customization/third_party/arcsoft
APP_LENS_PATH := ./customization/hostfw/device/lens
APP_CALIB_PATH := ./customization/hostfw/service/calib
APP_TB_PATH := ./customization/hostfw/service/testbed
APP_LENS_CP_SOU_PATH := /customization/HostFw/device/lens
DRV_FM_CP_SOU_PATH := /customization/drivers/fm
APP_CAL_CP_SOU_PATH := /customization/hostfw/service/calib
APP_SCRIPT_CP_SOU_PATH := /customization/hostfw/service/script
APP_TESTBED_CP_SOU_PATH := /customization/hostfw/service/testbed
APP_NDK_PATH := ./customization/hostfw/service/network
ICATWARE_ROOT_PATH :=  ./customization/hostfw/cathyware
ICATWARE_LIB_PATH :=  $(ICATWARE_ROOT_PATH)/lib
ICATWARE_SRC_PATH := $(ICATWARE_ROOT_PATH)/src
APP_ICATWARE_IQ_PATH := $(ICATWARE_SRC_PATH)/iq
APP_ICATWARE_DIQ_PATH := $(ICATWARE_SRC_PATH)/diq
APP_ICATWARE_AE_PATH := $(ICATWARE_SRC_PATH)/aealg
APP_ICATWARE_AWB_PATH := $(ICATWARE_SRC_PATH)/awbalg/awbalg
APP_ICATWARE_AWB_G3_PATH := $(ICATWARE_SRC_PATH)/awbalg/awbg3alg
APP_ICATWARE_AWB_G4_PATH := $(ICATWARE_SRC_PATH)/awbalg/awbg4alg
APP_ICATWARE_AWB_G46_PATH := $(ICATWARE_SRC_PATH)/awbalg/awbg46alg
APP_ICATWARE_MD_PATH := $(ICATWARE_SRC_PATH)/md
APP_ICATWARE_NDK_LIB_PATH := $(ICATWARE_LIB_PATH)/net
APP_LIB_PATH := ./lib
EZGUI_LIB_PATH :=  customization/hostfw/ez_gui/lib


include $(HOST_ROOT)/host.def
ifndef PLATFORM_PATH
PLATFORM_PATH   := $(PLATFORM)
endif
PLATFORM_ROOT   := ../$(PLATFORM_PATH)
ifeq ($(FPGA), YES)
LIB_PATH        := $(PLATFORM_ROOT)/lib.fpga
else
LIB_PATH        := $(PLATFORM_ROOT)/lib
endif

# output
DEST_DIR        := _$(PRJ)
OBJS_PATH       := $(DEST_DIR)/objs
DEST_SUBLIB_DIR := $(DEST_DIR)/_$(PLATFORM_PATH)
DOWN_PATH       := $(DEST_DIR)/download
RO_DOWN_PATH    := $(DOWN_PATH)/A/RO_RES
LSC_DOWN_PATH   := $(DOWN_PATH)/A/RO_RES/IQ
USER_DOWN_PATH  := $(DOWN_PATH)/B/USER_RES
SYS_RES_PATH    := $(PLATFORM_ROOT)/resource

# PLATFORM = 5210 or spca5310 which is defined in tool_chain.def
include $(PLATFORM_ROOT)/tool_chain.def

# project
AUD_DRV_PATH   := $(SRC_PATH)/drivers/audio
ifneq ($(PLATFORM), 5210)
DISP_DRV_PATH   := $(SRC_PATH)/drivers/disp
else
DISP_DRV_PATH   := $(SRC_PATH)/drivers/disp.5210
endif
ifneq ($(PLATFORM), spca5310)
SEN_DRV_PATH := $(SRC_PATH)/$(PLATFORM)/drivers/sensor
else
SEN_DRV_PATH := $(SRC_PATH)/drivers/sensor
endif
GSEN_DRV_PATH := $(SRC_PATH)/drivers/gsensor
RF_DRV_PATH := $(SRC_PATH)/drivers/rf

FM_DRV_PATH   := $(SRC_PATH)/drivers/fm

# 3rd party
#RELEASE_TO_3RDPARTY := 1

# dbg lib suffix, unmark it to enable debug library (libsnap, libos, libdrv)
#DBG_SUFFIX = .dbg

ISP_BIN_RES := $(SYS_RES_PATH)/$(PLATFORM)isp.bin
ifeq (,$(wildcard $(ISP_BIN_RES)))
	ifeq ($(FPGA), YES)
	ISP_BIN := spca5330isp_fpga.bin
	else
	ISP_BIN := spca5330isp.bin
	endif
	ISP_BIN_RES := $(RES_PATH)/ISP/$(ISP_BIN)
endif
FRM_INI := frm_user.ini

# define FRM_DRAM_PRM in project def to use project-specific prm
ifdef FRM_DRAM_PRM
     ifeq ($(PRJ_NAME), $(filter $(PRJ_NAME), EVB_DSC ))
     FRM_DRAM_PRM_SRC := $(RES_PATH)/$(addprefix $(PRJ_NAME), $(CHIP))/$(FRM_DRAM_PRM)
     else
     FRM_DRAM_PRM_SRC := $(RES_PATH)/$(PRJ_NAME)/$(FRM_DRAM_PRM)
     endif
else
 ifneq ($(CHIP), 5210)
  FRM_DRAM_PRM := drampara-ddr2-5310-189MHz.prm
  ifeq ($(findstring DSP5K_CPU_CLK_CUSTOM_DEFINE,$(CFLAGS)), DSP5K_CPU_CLK_CUSTOM_DEFINE)
   ifeq ($(findstring DCPU_CLK=2,$(CFLAGS)), DCPU_CLK=2)
    FRM_DRAM_PRM := drampara-ddr2-5310-162MHz.prm
   endif
  endif
 else
  FRM_DRAM_PRM := drampara-ddr2-5210-162MHz.prm
 endif
 FRM_DRAM_PRM_SRC := $(RES_PATH)/ISP/$(FRM_DRAM_PRM)
endif

#define FRM_LED_GRP & FRM_LED_PIN in project def to use project-specific LED
ifndef FRM_LED_GRP
 FRM_LED_GRP := 0 # 0:TGL 1:TGH 2:GEN ..., check FRM for details
endif
ifndef FRM_LED_PIN
 FRM_LED_PIN := 1
endif

#command
CP := cp -f
MV := mv -f
RM := rm -f
RMR := rm -rf
MKDIR := mkdir -p -m 777

#---------------------------------------------------------------------------
# Default definition:
#---------------------------------------------------------------------------
ifeq (,$(wildcard $(DEST_DIR)/multiboot.def))
-include $(DEST_DIR)/multiboot.def
include $(HOST_ROOT)/multiboot.def.tmpl
else
include $(DEST_DIR)/multiboot.def
endif

#---------------------------------------------------------------------------
# Variable definition:
#   1. Cross-compling tools
#   2. Linker script file
#   3. C complier options
#   4. Assembly compiler options
#   5. Linker options
#---------------------------------------------------------------------------
ifeq (cc,$(CC))
CC      := $(PREFIX)/gcc
AR      := $(PREFIX)/ar
OBJDUMP := $(PREFIX)/objdump
OBJCOPY := $(PREFIX)/objcopy
SIZE    := $(PREFIX)/size
ECHO    := echo
endif

#CFLAGS += -Wall
CFLAGS += -mgp32
#CFLAGS += -ansi
CFLAGS += -I$(PLATFORM_ROOT)/inc
CFLAGS += -I$(PLATFORM_ROOT)/include
CFLAGS += -I./include
CFLAGS += -I./include/customization
CFLAGS += -I./customization/hostfw/include
CFLAGS += -G 8
ifeq ($(GDB), YES)
CFLAGS += -g
CFLAGS += -DCMD_GDB=1
GDB_SUFFIX :=.gdb
else
CFLAGS += -O2
CFLAGS += -DCMD_GDB=0
GDB_SUFFIX :=
endif
CFLAGS += -DAPP_LUCKY7=1

ifeq ($(nand), 1)
TARGET := 1
LDSCRIPT := $(LD_PATH)/$(PLATFORM)nand.ld
CFLAGS += -DRAM_PROGRAM=1
endif

ifeq ($(ram), 1)
$(error doesn't support ram now...)
TARGET := 1
LDSCRIPT := $(LD_PATH)/$(PLATFORM)ram.ld
endif

# default build to rom
ifndef TARGET
$(error doesn't support rom now...)
TARGET := 0
rom := 1
LDSCRIPT := $(LD_PATH)/$(PLATFORM)rom.ld
CFLAGS += -DROM_PROGRAM=1
endif

ifeq ($(isrimem),1)
LDSCRIPT := $(LDSCRIPT:.ld=_isrimem.ld)
CFLAGS += -DISR_ON_IMEM=1
endif

LDFLAGS += -static
LDFLAGS += -nostdlib
LDFLAGS += -G 512

LDFLAGS_BSS += -R .sbss
LDFLAGS_BSS += -R .bss

#---------------------------------------------------------------------------
# Intermediate objects
#---------------------------------------------------------------------------
ifneq ($(PLATFORM), 5210)
RST_OBJS += $(LIB_PATH)/reset.o
endif

ifeq ($(MULTI_BOOT),YES)

ifeq ($(BOOT_FROM_SPI), 1)
LDSCRIPT := $(LD_PATH)/$(PLATFORM)spi.ld
EXT_OBJS += $(LIB_PATH)/bkdoor_spi.o
else
LDSCRIPT := $(LD_PATH)/$(PLATFORM)nand.ld
EXT_OBJS += $(LIB_PATH)/bkdoor_nand.o
endif

STAGE_OBJS :=
STAGE_OBJS += $(SRC_PATH)/$(PLATFORM)/crt0.o
STAGE_OBJS += $(SRC_PATH)/init.o
STAGE_OBJS += $(SRC_PATH)/disp_spi.nostage$(MULTI_BOOT_STAGE_DISP).o
STAGE_OBJS += $(SRC_PATH)/app_init.nostage$(MULTI_BOOT_STAGE_DISK_RSV).o
STAGE_OBJS += $(SRC_PATH)/$(PLATFORM)/dev_init.o
STAGE_OBJS += $(SRC_PATH)/multiboot_custom.o
STAGE_OBJS += $(SRC_PATH)/sensor_custom.nostage$(MULTI_BOOT_STAGE_SENSOR).o
STAGE_OBJS += $(SRC_PATH)/command.nostage$(MULTI_BOOT_STAGE_CMD).o
STAGE_OBJS += $(SRC_PATH)/sp5k_job.nostage$(MULTI_BOOT_STAGE_HOST).o

BUILD_STAGE_OBJS := $(addprefix $(OBJS_PATH)/, $(STAGE_OBJS))

## EXT_OBJS += $(LIB_PATH)/bkdoor_nand.o  # moved to linker script

endif

INI_OBJS += $(SRC_PATH)/$(PLATFORM)/crt0.o

ifneq ($(rom), 1)
INI_OBJS += $(SRC_PATH)/boot_param.o
endif

INI_OBJS += $(SRC_PATH)/init.o
INI_OBJS += $(SRC_PATH)/heap_init.o

GEN_OBJS += $(SRC_PATH)/disp_spi.o
GEN_OBJS += $(SRC_PATH)/app_init.o
GEN_OBJS += $(SRC_PATH)/$(PLATFORM)/dev_init.o
GEN_OBJS += $(SRC_PATH)/multiboot_custom.o
GEN_OBJS += $(SRC_PATH)/fastboot_custom.o
GEN_OBJS += $(SRC_PATH)/sensor_custom.o
GEN_OBJS += $(SRC_PATH)/command.o
GEN_OBJS += $(SRC_PATH)/sp5k_job.o

#==============================================================================
ifdef MT6622
MT6622 := 1
CUSTOM_LIBS += ./lib/MT6622/libmtk.a

CFLAGS += -DWITH_MT6622=1 \
	-D__BT_SPP_PROFILE__ \
	-D__SPP_SHARED_MEMORY__

GEN_OBJS += $(SRC_PATH)/mtk_btstack_api.o

SUBLIBS += customization/thirdparty/bst/libbst.a
endif
#------------------------------------------------------------------------------

ifdef WIFI # <<<<<< WIFI

# Default settings
WIFI_MODULE ?= rtk
LIVEST      ?= 0
export LIVEST WIFI_MODULE WIFI !ICATWARE

#CUSTOM_SUBLIBS +=  $(APP_NDK_PATH)/libWiFi.a

# ndk 1ibs are merged into one from ver. 00.02.02
CUSTOM_LIBS += $(ICATWARE_ROOT_PATH)/lib/net/libndk_lwip_v33.a 

endif # >>>>>> WIFI

OBJS := $(INI_OBJS) $(GEN_OBJS)

BUILD_INI_OBJS := $(addprefix $(OBJS_PATH)/, $(INI_OBJS))
BUILD_GEN_OBJS := $(addprefix $(OBJS_PATH)/, $(GEN_OBJS))
BUILD_OBJS := $(addprefix $(OBJS_PATH)/, $(OBJS))

BUILD_GOAL := $(DEST_DIR)/$(PLATFORM)


#[NOTES] subdirs and sublibs =========
#SUBLIBS are archives which get built from opened source trees
#CUSTOM_LIBS and PRJ_LIB are custom provided binaries (archives)
#CUSTOM_SUBLIBS are archives which get built from custom provided source trees
#SUBDIRS are opened source subtrees need to get built
#CUSTOM_SUBDIRS are custom provided source subtrees need to get built

#SUBLIBS += <path>/<archive> is equivalent to
#SUBDIRS += <path> and
#LIBS += $(DEST_SUBLIB_DIR)/<path>/<archive>

#[CUSTOM_]SUBLIBS += <path>/<archive> is equivalent to
#[CUSTOM_]SUBDIRS += <path> and
#[CUSTOM_]LIBS += $(OBJS_PATH)/<path>/<archive>
ifeq ($(ICATWARE),YES)
SUBLIBS += $(ICATWARE_SRC_PATH)/app_icatware_common.a
endif
#CUSTOM_SUBLIBS +=  $(APP_NDK_PATH)/libWiFi.a
CUSTOM_SUBLIBS += $(HOST_FW_PATH)/libhostfw.a
ifdef WIFI
CUSTOM_SUBLIBS +=  $(APP_NDK_PATH)/libWiFi.a
endif
ifeq ($(SPHOST_LIB_SOURCE_RELEASE), YES)
SUBLIBS += $(APP_LENS_PATH)/app_lens.a
SUBLIBS += $(APP_CALIB_PATH)/app_calib.a
SUBLIBS += $(APP_TB_PATH)/app_tb.a
endif

ifeq ($(ICATWARE),YES)
SUBLIBS += $(APP_ICATWARE_IQ_PATH)/lib_app_iq.a
SUBLIBS += $(APP_ICATWARE_DIQ_PATH)/app_diq.a
ifeq ($(ALGORITHM_AE), 1)
SUBLIBS += $(APP_ICATWARE_AE_PATH)/app_aealg.a
endif
ifeq ($(ALGORITHM_AWB), 1)
SUBLIBS += $(APP_ICATWARE_AWB_PATH)/app_awbalg.a
endif
ifeq ($(ALGORITHM_G3_AWB), 1)
SUBLIBS += $(APP_ICATWARE_AWB_G3_PATH)/app_awbalg.a
endif
ifeq ($(ALGORITHM_G4_AWB), 1)
SUBLIBS += $(APP_ICATWARE_AWB_G4_PATH)/app_awbalg.a
endif
ifeq ($(ALGORITHM_G46_AWB), 1)
SUBLIBS += $(APP_ICATWARE_AWB_G46_PATH)/app_awbalg.a
endif
ifeq ($(MOTION_DETECT), YES)
SUBLIBS += $(APP_ICATWARE_MD_PATH)/app_md.a
endif



endif

SUBLIBS += $(AUD_DRV_PATH)/libaud_custom.a
SUBLIBS += $(DISP_DRV_PATH)/libdisp_custom.a
SUBLIBS += $(SEN_DRV_PATH)/libsensor_custom.a
ifeq ($(OTG), YES)
SUBLIBS += $(FM_DRV_PATH)/libfm_custom.a
endif
SUBLIBS += $(GSEN_DRV_PATH)/libgsensor_custom.a
SUBLIBS += $(RF_DRV_PATH)/librf_custom.a

SUBLIBS += ${NET_SUBLIBS}

SUBDIRS += $(CUSTOM_SUBDIRS)

CUSTOM_LIBS += $(PRJ_LIB)

BUILD_SUBLIBS := $(addprefix $(DEST_SUBLIB_DIR)/,$(SUBLIBS))
BUILD_CUSTOM_SUBLIBS := $(addprefix $(OBJS_PATH)/,$(CUSTOM_SUBLIBS))

LIBS += $(BUILD_CUSTOM_SUBLIBS) $(CUSTOM_LIBS) $(BUILD_SUBLIBS)
LIBS += $(OBJS_PATH)/$(HOST_FW_PATH)/libhostfw.a ##dirty circular linkage

#---------------------------------------------------------------------------
# Libraries not provided by GCC
#---------------------------------------------------------------------------
# DON'T MODIFY LINES BELOW --------------
LIBS += $(LIB_PATH)/version.a
# DON'T MODIFY LINES ABOVE --------------

# Clean include from here!!!
LIBS += $(wildcard $(LIB_PATH)/apimon.a)
LIBS += $(LIB_PATH)/libcmd.a
LIBS += $(LIB_PATH)/libvcmd.a
LIBS += $(LIB_PATH)/lib5kmoe.a
LIBS += $(LIB_PATH)/lib5kmodesw.a
LIBS += $(LIB_PATH)/lib5kotrack.a
LIBS += $(LIB_PATH)/lib5kredeye.a
LIBS += $(LIB_PATH)/lib5kftrack.a
LIBS += $(LIB_PATH)/lib5kcvrwarningsystem.a
LIBS += $(LIB_PATH)/liblanedetect.a
LIBS += $(LIB_PATH)/libheadlight.a
LIBS += $(LIB_PATH)/libspeedlimit.a
LIBS += $(LIB_PATH)/libsignrecognize.a
LIBS += $(LIB_PATH)/lib5kts.a
LIBS += $(LIB_PATH)/lib5klmi.a
LIBS += $(LIB_PATH)/lib5ksensor.a
LIBS += $(LIB_PATH)/sp5k_global.a
LIBS += $(LIB_PATH)/libboot$(GDB_SUFFIX).a
LIBS += $(LIB_PATH)/sp5k_slide.a
LIBS += $(LIB_PATH)/lib5kcapture.a
LIBS += $(LIB_PATH)/lib5kcalib.a
LIBS += $(LIB_PATH)/lib5k3a.a
LIBS += $(LIB_PATH)/lib5kpb.a
LIBS += $(LIB_PATH)/lib5kmedia.a
LIBS += $(LIB_PATH)/lib5kdisp.a
LIBS += $(LIB_PATH)/lib5kcec.a
LIBS += $(LIB_PATH)/lib5krsvblk.a
LIBS += $(LIB_PATH)/lib5kgfx.a
LIBS += $(LIB_PATH)/lib5kpip.a
LIBS += $(LIB_PATH)/lib5kmview.a
LIBS += $(LIB_PATH)/lib5kutil.a
LIBS += $(LIB_PATH)/lib5kdisk.a
LIBS += $(LIB_PATH)/lib5kusb$(GDB_SUFFIX).a
LIBS += $(LIB_PATH)/lib5kaud.a
LIBS += $(LIB_PATH)/lib5kdcf.a
LIBS += $(LIB_PATH)/lib5kdpof.a
LIBS += $(LIB_PATH)/lib5kexif.a
LIBS += $(LIB_PATH)/lib5kcdsp.a
LIBS += $(LIB_PATH)/lib5kos.a

LIBS += $(LIB_PATH)/libusb.a  # FIXME! mw usb using others linkage workaround
LIBS += $(LIB_PATH)/libotrack.a
LIBS += $(LIB_PATH)/libredeye.a
LIBS += $(LIB_PATH)/libftrack.a

ifdef WIFI
LIBS += $(LIB_PATH)/libsdio.a
ifeq ($(LIVEST),1)
LIBS +=  $(ICATWARE_LIB_PATH)/libstdc++.a # C++
endif
endif

LIBS += $(LIB_PATH)/libbanding.a

ifneq ($(PLATFORM),5210)
LIBS += $(LIB_PATH)/libxfilter.a
LIBS += $(LIB_PATH)/libldc.a
LIBS += $(wildcard $(LIB_PATH)/libsmear.a)
endif
LIBS += $(LIB_PATH)/modesw.a
LIBS += $(LIB_PATH)/libsnap$(DBG_SUFFIX).a
ifeq ($(PLATFORM),5210)
LIBS += $(LIB_PATH)/libxdn.a
endif
LIBS += $(LIB_PATH)/libav.a
LIBS += $(LIB_PATH)/libpb.a
LIBS += $(LIB_PATH)/libeffect.a
LIBS += $(LIB_PATH)/libbp.a
LIBS += $(LIB_PATH)/libgfx.a
LIBS += $(LIB_PATH)/libpip.a
LIBS += $(LIB_PATH)/libmview.a
LIBS += $(LIB_PATH)/fnt.a
LIBS += $(LIB_PATH)/str.a
LIBS += $(LIB_PATH)/slide.a
LIBS += $(LIB_PATH)/lib3a.a
LIBS += $(LIB_PATH)/libmview.a
#LIBS += $(LIB_PATH)/libcdsp.a
LIBS += $(LIB_PATH)/libcdsp$(DBG_SUFFIX).a
LIBS += $(LIB_PATH)/libmview.a
LIBS += $(LIB_PATH)/libsensor.a
LIBS += $(LIB_PATH)/lib3a.a  ##dirty circular linkage
LIBS += $(LIB_PATH)/librsvblk.a
LIBS += $(LIB_PATH)/libresource.a
LIBS += $(LIB_PATH)/libdisp.a
LIBS += $(LIB_PATH)/libusb.a
LIBS += $(LIB_PATH)/libdcf.a
LIBS += $(LIB_PATH)/libdpof.a
ifeq ($(CHIP), 5330)
LIBS += $(LIB_PATH)/libdisk_exp.a
else
LIBS += $(LIB_PATH)/libdisk.a
endif
LIBS += $(LIB_PATH)/fs.a
USE_POSIX_VFS := YES
ifeq ($(USE_POSIX_VFS), YES)
LIBS += $(LIB_PATH)/libvfsposix.a
else
LIBS += $(LIB_PATH)/libvfs.a
endif

LIBS += $(LIB_PATH)/libaud.a      # Audio device H/W driver
LIBS += $(LIB_PATH)/global.a
LIBS += $(LIB_PATH)/libLZO.a
ifdef MT6622
LIBS += $(LIB_PATH)/libdrv.hsuart$(DBG_SUFFIX).a
else
LIBS += $(LIB_PATH)/libdrv$(DBG_SUFFIX).a
endif
LIBS += $(LIB_PATH)/libutil$(GDB_SUFFIX).a
LIBS += $(LIB_PATH)/libdist.a
ifdef MT6622
LIBS += $(LIB_PATH)/hal.hsuart$(GDB_SUFFIX).a
else
LIBS += $(LIB_PATH)/hal$(GDB_SUFFIX).a
endif
LIBS += $(LIB_PATH)/libmsg.a
#LIBS += $(LIB_PATH)/libmp3.a
LIBS += $(LIB_PATH)/libts.a
#LIBS += $(LIB_PATH)/libstroke.a
ifeq ($(GDB), YES)
LIBS += $(LIB_PATH)/libos$(GDB_SUFFIX).a
else
LIBS += $(LIB_PATH)/libos$(DBG_SUFFIX).a
endif
LIBS += $(LIB_PATH)/libcmd.a  # FIXME! cmd used in prog dead linkage workaround

ifdef MT6622
LIBS += $(LIB_PATH)/libdrv.hsuart$(DBG_SUFFIX).a  # FIXME! timer used in os, hal linkage workaround
LIBS += $(LIB_PATH)/hal.hsuart$(GDB_SUFFIX).a  # FIXME! timer using hal linkage workaround
else
LIBS += $(LIB_PATH)/libdrv$(DBG_SUFFIX).a  # FIXME! timer used in os, hal linkage workaround
LIBS += $(LIB_PATH)/hal$(GDB_SUFFIX).a  # FIXME! timer using hal linkage workaround
endif

LIBS += $(LIB_PATH)/libutil$(GDB_SUFFIX).a  # FIXME! codeSum() circular linkage workaround
LIBS += $(LIB_PATH)/libexif.a
ifeq ($(V33), YES)
LIBS += $(LIB_PATH)/libh264.a
endif
LIBS += $(LIB_PATH)/liblanedetect.a
LIBS += $(LIB_PATH)/libheadlight.a
LIBS += $(LIB_PATH)/libc_ex.a
LIBS += $(LIB_PATH)/debug_mod.a
#LIBS += $(LIB_PATH)/liblua.a
LIBS += $(LIB_PATH)/libutil$(GDB_SUFFIX).a  # FIXME! printf() circular linkage workaround
ifdef MT6622
LIBS += $(LIB_PATH)/hal.hsuart$(GDB_SUFFIX).a  # FIXME! code sentry using hal linkage workaround
else
LIBS += $(LIB_PATH)/hal$(GDB_SUFFIX).a  # FIXME! code sentry using hal linkage workaround
endif
ifeq ($(GDB), YES)
LIBS += $(LIB_PATH)/libos$(GDB_SUFFIX).a  # FIXME! bitwidth circular linkage workaround
LIBS += $(LIB_PATH)/libgdbstub.a # for gdb debug support
else
LIBS += $(LIB_PATH)/libos$(DBG_SUFFIX).a  # FIXME! bitwidth circular linkage workaround
endif

ifeq ($(V33), YES)
LIBS += $(LIB_PATH)/libh264.a
endif
CFG_DEFS += multiboot.def

BUILD_CFG_FILES := $(addprefix $(DEST_DIR)/,$(CFG_DEFS) $(CFG_HEADERS))

# FIC supporting
ifeq ($(nand), 1)
DOWN_FIC := $(DOWN_PATH)/FIC.BIN
endif

# Project IO header
 IO_HEADER := $(HOST_FW_PATH)/device/io/app_Gpio_$(HW_NAME)_$(HW_VER).h

#######################   EZ_GUI LIB   ###################################
ifdef WIFI
LIBS += $(EZGUI_LIB_PATH)/libcustomer_id.a
LIBS += $(EZGUI_LIB_PATH)/libmacAddress.a
endif

#---------------------------------------------------------------------------
# Explicit rules
#---------------------------------------------------------------------------
.PHONY: $(PLATFORM) resource FRM_PARTIAL_FILELIST
$(PLATFORM): resource $(DOWN_PATH)/$(PRJ_NAME).bin FRM_PARTIAL_FILELIST
	@$(ECHO) "Building $(PRJ_NAME) $(HW_NAME) $(HW_VER) done"
	@$(ECHO) "PRJ := $(PRJ_NAME)" > lastprj.def
	$(RM) *.ld
	@date
ifeq ($(RES), NO)
ifeq ($(ISP), YES)
	@$(TOOL_PATH)/FRM-F5.EXE
endif
endif

$(IO_HEADER): io
.PHONY: io
io:
	@$(MAKE) -C $(HOST_FW_PATH)/device/io -f MakefileIO

.PHONY: $(SUBDIRS)
$(SUBDIRS): %: $(IO_HEADER)
	@echo "Making $@"
	@$(MAKE) -C $@ PRJ_NAME=$(PRJ_NAME)

.PHONY: $(SUBLIBS) $(CUSTOM_SUBLIBS)
$(SUBLIBS) $(CUSTOM_SUBLIBS): %: $(IO_HEADER)
	@echo "Making $@"
	@$(MAKE) -C $(dir $@) PRJ_NAME=$(PRJ_NAME)

$(BUILD_SUBLIBS): $(DEST_SUBLIB_DIR)/%: % #ie $(SUBLIBS)
	$(MSG_GOAL_OK) #NOTE! empty rule results in .elf not always rebuilt

$(BUILD_CUSTOM_SUBLIBS): $(OBJS_PATH)/%: % #ie $(CUSTOM_SUBLIBS)
	$(MSG_GOAL_OK) #NOTE! empty rule results in .elf not always rebuilt


$(DOWN_PATH)/FRM.exe: ../tool/FRM/FRM.exe resource
	@$(RM) $@
	@$(MKDIR) $(DOWN_PATH)/Profiles # avoid .svn so no cp -R
	@$(CP) -u ../tool/FRM/Profiles/* $(DOWN_PATH)/Profiles/
	@$(CP) -u $(filter-out $< ../tool/FRM/Profiles ../tool/FRM/ResImageMaker,$(wildcard ../tool/FRM/*)) $(DOWN_PATH)/
	@$(MKDIR) $(DOWN_PATH)/ResImageMaker
	@$(CP) -u ../tool/FRM/ResImageMaker/* $(DOWN_PATH)/ResImageMaker/
	@$(CP) -u frm_used.txt $(DOWN_PATH)/
#	@./newrsv.sh $(RSV_A_SIZE) $(RSV_B_SIZE) $(DOWN_PATH)/ResImageMaker/config_template.xml
	@$(CP) $(ISP_BIN_RES) $(DOWN_PATH)/$(PLATFORM)isp.bin
	@sed -e "/ADefSize/s/=.*/=$(RSV_A_SIZE)/" -e "/BDefSize/s/=.*/=$(RSV_B_SIZE)/" \
		 -e "/FIRMWARE.BIN/s/FIRMWARE/$(PRJ_NAME)/" \
		 -e "/SpcaPlatformName/s/SpcaPlatformName/SPCA$(CHIP)/" \
		 -e "/ISPName/s/ISP.BIN/spca$(CHIP)isp.bin/" \
		 -e "/SettingFilename/s/drampara.prm/$(FRM_DRAM_PRM)/" \
		 -e "/Custom_CurGpio/s/XXX/$(FRM_LED_GRP)/" \
		 -e "/Custom_CurMethod/s/XXX/$(FRM_LED_PIN)/" \
			$(RES_PATH)/ISP/$(FRM_INI) > $(DOWN_PATH)/$(FRM_INI)
	#@$(ECHO) $(PRJ_BIN)
	#@$(ECHO) $(FRM_DRAM_PRM)  $(PRJ_NAME)  $(PRJ)
	#@$(ECHO) $(TOOL_PATH)/drampara $(FRM_DRAM_PRM_SRC:.prm=.txt) $(DOWN_PATH)/$(FRM_DRAM_PRM)
	#@$(TOOL_PATH)/drampara $(FRM_DRAM_PRM_SRC:.prm=.txt) $(DOWN_PATH)/$(FRM_DRAM_PRM)
ifeq ($(V33), YES)
	@$(TOOL_PATH)/drampara $(FRM_DRAM_PRM_SRC:.prm=.txt) $(DOWN_PATH)/$(FRM_DRAM_PRM) 1
else
	@$(TOOL_PATH)/drampara $(FRM_DRAM_PRM_SRC:.prm=.txt) $(DOWN_PATH)/$(FRM_DRAM_PRM)
endif
	#@$(CP) $(FRM_DRAM_PRM_SRC) $(DOWN_PATH)/$(FRM_DRAM_PRM)
ifeq ($(PRJ_NAME), $(filter $(PRJ_NAME), EVB_DSC ))
	@$(CP) $(RES_PATH)/$(addprefix $(PRJ_NAME), $(CHIP))/BGA_DDR3_1G_300MHz_ODT_TEST.prm $(DOWN_PATH)
	@$(CP) $(RES_PATH)/$(addprefix $(PRJ_NAME), $(CHIP))/BGA_DDR3_1G_360MHz_ODT_TEST.prm $(DOWN_PATH)
else	
	@$(CP) $(RES_PATH)/$(PRJ_NAME)/BGA_DDR3_1G_300MHz_ODT_TEST.prm $(DOWN_PATH)
	@$(CP) $(RES_PATH)/$(PRJ_NAME)/BGA_DDR3_1G_360MHz_ODT_TEST.prm $(DOWN_PATH)
endif	
	@$(CP) ../tool/SetPara.exe $(DOWN_PATH)
	@$(CP) ../tool/SetReg.exe $(DOWN_PATH)
	@$(CP) ../tool/SetReg.ini $(DOWN_PATH)
	@$(CP) $< $@ # copying the target last for dependency checks

ifeq ($(nand), 1)
$(DEST_DIR)/sumpatch.dat: $(BUILD_GOAL).elf
	@$(RM) $@
	@x=[[:xdigit:]]; s=[[:blank:]]; e=[[:space:]]; \
	bkdoorsz=`$(OBJDUMP) -t $< | sed -n -e \
	"s/^8\($$x\{7\}\)$$s\+.*$$s\+_bkdoor_block_limit$$e\?$$/0x\1/p"` ; \
	$(ECHO) "**** backdoor image size $$(( $$bkdoorsz )) bytes *******" ; \
	$(OBJDUMP) -t $< | sed -n -e \
	"s/^8\($$x\{7\}\)$$s\+.*$$s\+signature$$e\?$$/0x\1 60/p" > $@
else
ifeq ($(V33), YES)
$(DEST_DIR)/sumpatch.dat: $(BUILD_GOAL).elf
	@$(RM) $@
	@x=[[:xdigit:]]; s=[[:blank:]]; e=[[:space:]]; \
	$(OBJDUMP) -t $< | sed -n \
	-e "s/^9fc\($$x\{5\}\)$$s\+.*$$s\+signature$$e\?$$/0x\1 60/p" \
	-e "s/^9fc\($$x\{5\}\)$$s\+.*$$s\+_dram_content_lma_start$$e\?$$/0x\1/p" \
	> $@.tmp
	@cat $@.tmp | sed -e "{:q;N;s/\n/ /g;t q}" > $@
	$(RM) $@.tmp
else
$(DEST_DIR)/sumpatch.dat:
endif
endif

# removing all the stage builds if stage list or objs / libs order changed
ifeq ($(MULTI_BOOT),YES)
$(BUILD_GOAL).stagelist: $(BUILD_GOAL).stagelist.bak $(BUILD_GOAL).multiboot.bak
	$(MULTI_BOOT_ERR)
	@$(ECHO) -e "\n****building multi boot stage list: $(STAGE_LIST) ***\n"
else
$(BUILD_GOAL).stagelist: $(BUILD_GOAL).stagelist.bak
endif
	@if ! diff -q -N $@ $< ; then \
		$(RM) $(BUILD_GOAL){,.stage*}.{elf,map,bin} \
			$(DOWN_PATH)/$(PRJ_NAME){,.stage*}.bin \
			$(DEST_DIR)/$(LDSCRIPT:.ld=.stage*.ld) && \
		$(CP) $< $@ ; \
	fi

.PHONY: $(BUILD_GOAL).stagelist.bak
$(BUILD_GOAL).stagelist.bak:
	@$(MKDIR) $(dir $@)
	@$(RM) $@
	@$(ECHO) $(STAGE_LIST) > $@
	@$(ECHO) "$(strip $(BUILD_STAGE_OBJS) $(RST_OBJS) $(BUILD_INI_OBJS) \
		$(EXT_OBJS) $(BUILD_GEN_OBJS) $(LIBS))" \
		| sed -e 's/ /\n/g' >> $@

# FIC using multiload info too
$(DOWN_PATH)/$(PRJ_NAME).bin: \
$(BUILD_GOAL).bin $(DEST_DIR)/sumpatch.dat $(DEST_DIR)/multiload.dat $(DOWN_FIC)
	@$(MKDIR) $(dir $@)
ifeq ($(nand), 1)
	@$(TOOL_PATH)/multiboot_load_patch.sh $< \
			$(DEST_DIR)/multiload.dat $(DOWN_FIC) > $@.bak
	@$(TOOL_PATH)/sumpatch.exe $@.bak < $(DEST_DIR)/sumpatch.dat
	@$(MV) $@.bak $@
else
	@$(CP) $< $@
ifeq ($(V33), YES)
	./v33patch.exe $@ < $(DEST_DIR)/sumpatch.dat
endif
endif
	@wc -c $@
	$(MSG_GOAL_OK)

$(BUILD_GOAL).bin: $(BUILD_GOAL).elf
	@$(OBJCOPY) --output-target=binary -S -g -x -X \
		$(LDFLAGS_BSS) -R .reginfo $< $@
	$(MSG_GOAL_OK)


ifneq ($(MULTI_BOOT),YES)
$(BUILD_GOAL).elf: \
$(DEST_DIR)/$(LDSCRIPT) $(RST_OBJS) $(BUILD_OBJS) $(EXT_OBJS) $(LIBS) $(SUBDIRS)
	$(MSG_SPLIT_LINE)
ifeq ($(ICATWARE),YES)  # <<<build ICATWARE s
	$(warning "COPY APP LIB to $(ICATWARE_LIB_PATH)")
ifeq ($(OTG), YES)
	$(CP) $(DEST_SUBLIB_DIR)/$(DRV_FM_CP_SOU_PATH)/libfm_custom.a $(ICATWARE_LIB_PATH)
endif
ifeq ($(SPHOST_LIB_SOURCE_RELEASE),YES) 
	$(CP) $(DEST_SUBLIB_DIR)/$(APP_LENS_CP_SOU_PATH)/app_lens.a $(APP_LIB_PATH)
	$(CP) $(DEST_SUBLIB_DIR)/$(APP_CAL_CP_SOU_PATH)/app_calib.a $(APP_LIB_PATH)
	$(CP) $(DEST_SUBLIB_DIR)/$(APP_TESTBED_CP_SOU_PATH)/app_tb.a $(APP_LIB_PATH)
endif	
	$(MKDIR) $(ICATWARE_LIB_PATH)/$(IQ_NAME)/
	$(CP) $(DEST_SUBLIB_DIR)/$(APP_ICATWARE_IQ_PATH)/lib_app_iq.a $(ICATWARE_LIB_PATH)/$(IQ_NAME)
	$(CP) $(DEST_SUBLIB_DIR)/$(APP_ICATWARE_DIQ_PATH)/app_diq.a $(ICATWARE_LIB_PATH)
	$(CP) $(DEST_SUBLIB_DIR)/$(ICATWARE_SRC_PATH)/app_icatware_common.a $(ICATWARE_LIB_PATH)
ifeq ($(ALGORITHM_AE), 1)
	$(CP) $(DEST_SUBLIB_DIR)/$(APP_ICATWARE_AE_PATH)/app_aealg.a $(ICATWARE_LIB_PATH)
endif
ifeq ($(ALGORITHM_AWB), 1)
	$(CP) $(DEST_SUBLIB_DIR)/$(APP_ICATWARE_AWB_PATH)/app_awbalg.a $(ICATWARE_LIB_PATH)
endif
ifeq ($(ALGORITHM_G3_AWB), 1)
	$(CP) $(DEST_SUBLIB_DIR)/$(APP_ICATWARE_AWB_G3_PATH)/app_awbalg.a $(ICATWARE_LIB_PATH)
endif
ifeq ($(ALGORITHM_G4_AWB), 1)
	$(CP) $(DEST_SUBLIB_DIR)/$(APP_ICATWARE_AWB_G4_PATH)/app_awbalg.a $(ICATWARE_LIB_PATH)
endif
ifeq ($(ALGORITHM_G46_AWB), 1)
	$(CP) $(DEST_SUBLIB_DIR)/$(APP_ICATWARE_AWB_G46_PATH)/app_awbalg.a $(ICATWARE_LIB_PATH)
endif
ifeq ($(MOTION_DETECT), YES)
	$(CP) $(DEST_SUBLIB_DIR)/$(APP_ICATWARE_MD_PATH)/app_md.a $(ICATWARE_LIB_PATH)
endif  
      
endif # <<<build ICATWARE e
ifeq ($(OTG), YES)
	#$(ECHO) $(DEST_SUBLIB_DIR)/$(FM_DRV_PATH)/$(ICATWARE_LIB_PATH)
	$(CP) $(DEST_SUBLIB_DIR)/$(FM_DRV_PATH)/libfm_custom.a $(ICATWARE_LIB_PATH)
endif
# To generate cross-reference, use:
# -Wl,-Map,$(@:.elf=.map) -Wl,--warn-common -Wl,--cref
	@$(CC) -o $@ $(RST_OBJS) $(BUILD_INI_OBJS) \
		$(EXT_OBJS) $(BUILD_GEN_OBJS) \
		$(LDFLAGS) -T $< $(LIBS) -lm -lc -lgcc\
		-Wl,-Map,$(@:.elf=.map) -Wl,--warn-common
	$(MSG_GOAL_OK)

$(DEST_DIR)/$(LDSCRIPT): $(LDSCRIPT) $(DOWN_FIC) $(BUILD_GOAL).stagelist
	@$(MKDIR) $(dir $@)
	@$(CP) $< $@
	@$(TOOL_PATH)/ldscript_fic_patch.sh $@ $(DOWN_FIC)

else #ifneq ($(MULTI_BOOT),YES)

BASE_STAGE := $(firstword $(STAGE_LIST))
LAST_STAGE := $(call host-stage-val, MULTI_BOOT_STAGE_ALL)

# backup archives
BUILD_BAK_LIBS := $(filter-out $(LIB_PATH)/%,$(sort $(LIBS)))

$(BUILD_GOAL).multiboot.bak: $(BUILD_BAK_LIBS)
	@list='$^'; \
	for lib in $$list ; do \
		if [ "$@" -ot "$${lib}" ] ; then \
			$(TOOL_PATH)/multiboot_archive_bak.sh \
					$${lib} $(PREFIX) ; \
			if [ $$? != 0 ] ; then \
				exit 1 ; \
			fi \
		fi \
	done
	@$(RM) $@
	@touch $@


$(BUILD_GOAL).elf: $(BUILD_GOAL).stage$(LAST_STAGE).elf
	@$(RM) $@ $(@:.elf=.map)
	@$(CP) $(<:.elf=.map) $(@:.elf=.map)
	@$(CP) $< $@


define STAGE_GOAL_RULES

 STAGE$(1)_LIBS := $$(foreach lib,$$(LIBS),$$(call stage-archive,$(1),$$(lib)))

 BUILD_STAGE$(1)_ALL_OBJS := $$(foreach obj,\
	$$(RST_OBJS) $$(BUILD_INI_OBJS) $$(EXT_OBJS) $$(BUILD_GEN_OBJS),\
	$$(call stage-obj,$(1),$$(obj),$$(BUILD_STAGE_OBJS)))

 BUILD_STAGE$(1)_OBJS := $$(foreach obj,$$(BUILD_OBJS),\
	$$(call stage-obj,$(1),$$(obj),$$(BUILD_STAGE_OBJS)))


 ifeq ($(1),$$(LAST_STAGE))
 $$(BUILD_GOAL).stage$(1).elf: $$(DEST_DIR)/$$(LDSCRIPT:.ld=.stage$(1).ld) \
 $$(BUILD_STAGE$(1)_ALL_OBJS) $$(STAGE$(1)_LIBS) $$(DOWN_FIC)
	@$$(TOOL_PATH)/ldscript_fic_patch.sh $$< $$(DOWN_FIC)
 else
 $$(BUILD_GOAL).stage$(1).elf: $$(DEST_DIR)/$$(LDSCRIPT:.ld=.stage$(1).ld) \
 $$(BUILD_STAGE$(1)_ALL_OBJS) $$(STAGE$(1)_LIBS)
 endif
	$$(MSG_SPLIT_LINE)
# To generate cross-reference, use:
# -Wl,-Map,$$(@:.elf=.map) -Wl,--warn-common -Wl,--cref
	@$$(CC) -o $$@ $$(LDFLAGS) -T $$< \
		-Wl,-Map,$$(@:.elf=.map) -Wl,--warn-common
	@$$(ECHO) -e "STAGE $(1) ===== multi boot stage has binary size ====\n"
	@$$(SIZE) $$@
	@$$(ECHO) -e "\nSTAGE $(1) ==========================================="
	$$(MSG_GOAL_OK)

endef

define STAGE_LDSCRIPT_RULES

  PRE_STAGE$(1) := $$(call pre-stage,$(1),$$(STAGE_LIST))
  BUILD_PRE_GOAL$(1) := $$(BUILD_GOAL).stage$$(PRE_STAGE$(1))

  $$(DEST_DIR)/$$(LDSCRIPT:.ld=.stage$(1).ld): \
  $$(DEST_DIR)/$$(LDSCRIPT:.ld=.stage$$(PRE_STAGE$(1)).ld) \
  $$(BUILD_PRE_GOAL$(1)).map.input $$(BUILD_GOAL).stagelist
	@$$(RM) $$@ $$@.cmd*
	@$$(ECHO) $$(BUILD_STAGE$(1)_ALL_OBJS) \
		| sed -e 's/^ \+//' -e 's/ \+/\n/g' > $$@.cmdobj
	@$$(ECHO) $$(STAGE$(1)_LIBS) '-lm -lc -lgcc' \
		| sed -e 's/^ \+//' -e 's/ \+/\n/g' > $$@.cmdlib
	@$$(TOOL_PATH)/multiboot_ldscript.exe $$(PRE_STAGE$(1)) $(1) $$@ \
		$$< $$(BUILD_PRE_GOAL$(1)).map.input $$(PREFIX)
	$$(MSG_GOAL_OK)

endef

ifeq ($(OBJMAP),NO)

$(eval $(call STAGE_GOAL_RULES,$(LAST_STAGE)))

else #ifeq ($(OBJMAP),NO)

$(foreach stage, $(STAGE_LIST), $(eval $(call STAGE_GOAL_RULES,$(stage))))

$(foreach stage, $(wordlist 2, $(words $(STAGE_LIST)), $(STAGE_LIST)), \
		$(eval $(call STAGE_LDSCRIPT_RULES,$(stage))))


$(DEST_DIR)/$(LDSCRIPT:.ld=.stage$(BASE_STAGE).ld): \
$(LDSCRIPT:.ld=.stage.ld.tmpl) $(SUBDIRS) $(BUILD_GOAL).stagelist
	@$(MKDIR) $(dir $@)
	@$(RM) $@ $@.cmd*
	@$(ECHO) $(BUILD_STAGE$(BASE_STAGE)_ALL_OBJS) \
		| sed -e 's/^ \+//' -e 's/ \+/\n/g' > $@.cmdobj
	@$(ECHO) $(STAGE$(BASE_STAGE)_LIBS) '-lm -lc -lgcc' \
		| sed -e 's/^ \+//' -e 's/ \+/\n/g' > $@.cmdlib
	@$(TOOL_PATH)/multiboot_ldscript.exe \
			0 $(BASE_STAGE) $@ $< $(words $(STAGE_LIST))
	$(MSG_GOAL_OK)

$(BUILD_GOAL).stage%.map.input: $(BUILD_GOAL).stage%.elf
# keeping backups of stage binaries
	@$(MKDIR) $(DOWN_PATH)/
	@$(RM) $(DOWN_PATH)/$(PRJ_NAME).stage$*.bin
	@$(OBJCOPY) --output-target=binary -S -g -x -X $(LDFLAGS_BSS) \
		-R .reginfo $< $(DOWN_PATH)/$(PRJ_NAME).stage$*.bin
	@wc -c $(DOWN_PATH)/$(PRJ_NAME).stage$*.bin
	@$(ECHO) "**** back up $(DOWN_PATH)/$(PRJ_NAME).stage$*.bin **********"
	@$(RM) $@
	@$(TOOL_PATH)/multiboot_map.exe $(<:.elf=.map) $@

endif #ifeq ($(OBJMAP),NO)

endif #ifneq ($(MULTI_BOOT),YES)

# FIC for non-multiboot needs multiload info too
$(DEST_DIR)/multiload.dat: $(BUILD_GOAL).elf
	@$(TOOL_PATH)/multiboot_load_info.sh $< "$(STAGE_LIST)" $(PREFIX) > $@
	$(MSG_GOAL_OK)


ifneq ($(RES),NO)
ifeq ($(nand), 1)
$(DOWN_FIC): resource
	$(MSG_GOAL_OK) #NOTE! empty rule results in .elf not always rebuilt
endif

resource:
#@$(CP) RESOURCE/$(PRJ_NAME)/A/RO_RES/IQ/$(IQ_NAME)/LSC.BIN $(LSC_DOWN_PATH)/
	@$(MAKE) -f MakefileRes $(MAKECMDGOALS)
	@$(CP) $(USER_DOWN_PATH)/UIDATA.BIN $(RO_DOWN_PATH)/
ifdef WIFI
ifeq ($(WIFI_MODULE),mtk)
	@$(CP) $(RES_PATH)/B/UDF/FW.BIN $(DOWN_PATH)/B/UDF/
endif
	@$(CP) $(RES_PATH)/B/UDF/HAPD0.CFG $(DOWN_PATH)/B/UDF/
	@$(CP) $(RES_PATH)/B/UDF/SSID.CFG $(DOWN_PATH)/B/UDF/
	@$(CP) $(RES_PATH)/B/UDF/SSID_PW.CFG $(DOWN_PATH)/B/UDF/
	@$(CP) $(RES_PATH)/B/UDF/CUSTOMER_ID.CFG $(DOWN_PATH)/B/UDF/
	@$(CP) $(RES_PATH)/B/UDF/APMODE.CFG $(DOWN_PATH)/B/UDF/
	@$(CP) $(RES_PATH)/B/UDF/HAPD0.CFG $(DOWN_PATH)/A/ADF/
	@$(CP) $(RES_PATH)/B/UDF/SSID.CFG $(DOWN_PATH)/A/ADF/
	@$(CP) $(RES_PATH)/B/UDF/SSID_PW.CFG $(DOWN_PATH)/A/ADF/
	@$(CP) $(RES_PATH)/B/UDF/APMODE.CFG $(DOWN_PATH)/A/ADF/
	@$(CP) $(RES_PATH)/B/UDF/CUSTOMER_ID.CFG $(DOWN_PATH)/A/ADF/
	@$(CP) $(RES_PATH)/B/UDF/UDF.BIN $(DOWN_PATH)/A/ADF/
	@$(CP) $(RES_PATH)/B/USER_RES/UDF.DAT $(DOWN_PATH)/A/ADF/
endif

ifeq ($(PRJ_NAME), $(filter $(PRJ_NAME), PRJ_DSC ))
	@$(CP) customization/hostfw/cathyware/RESOURCE/A/RO_RES/IQ/$(CHIP)/$(IQ_NAME)/LSC.BIN $(LSC_DOWN_PATH)/
	@$(CP) customization/hostfw/cathyware/RESOURCE/A/RO_RES/IQ/$(CHIP)/$(IQ_NAME)/AWBCTT.BIN $(RO_DOWN_PATH)/
	@$(CP) customization/hostfw/cathyware/RESOURCE/A/RO_RES/IQ/$(CHIP)/$(IQ_NAME)/AWBFILT.BIN $(RO_DOWN_PATH)/
	@$(CP) $(RO_DOWN_PATH)/IQ/$(CHIP)/$(IQ_NAME)/$(AE_RES_NAME) $(RO_DOWN_PATH)/AEALG.BIN
	@$(CP) customization/hostfw/cathyware/RESOURCE/A/RO_RES/IQ/$(CHIP)/$(IQ_NAME)/WDRFT_DQ.BIN $(RO_DOWN_PATH)/
	@$(CP) RESOURCE/$(PRJ_NAME)/A/RO_RES/USBSTR.TXT $(RO_DOWN_PATH)/
	@$(CP) RESOURCE/$(PRJ_NAME)/A/RO_RES/USBUVC.BIN $(RO_DOWN_PATH)/
	@$(CP) RESOURCE/$(PRJ_NAME)/A/RO_RES/USBUVC_TS.BIN $(RO_DOWN_PATH)/
	@$(CP) RESOURCE/$(PRJ_NAME)/A/RO_RES/USBUVC_PB.BIN $(RO_DOWN_PATH)/
	@$(MKDIR) $(DOWN_PATH)/CIQTXT
	@$(CP) RESOURCE/$(PRJ_NAME)/A/RO_RES/IQ/$(CHIP)/$(IQ_NAME)/CIQTXT/*.*  $(DOWN_PATH)/CIQTXT/
	@$(RM) $(RO_DOWN_PATH)/$(CHIP)/$(IQ_NAME)/AWBCTT.BIN
	@$(RM) $(RO_DOWN_PATH)/$(CHIP)/$(IQ_NAME)/AWBFILT.BIN
	@$(RM) $(RO_DOWN_PATH)/$(CHIP)/$(IQ_NAME)/AEALG.BIN
	#@$(RM) $(RO_DOWN_PATH)/$(CHIP)/$(IQ_NAME)/WDRFT_DQ.BIN
	@$(RMR) $(LSC_DOWN_PATH)/$(CHIP)/$(IQ_NAME)
	@$(RMR) $(LSC_DOWN_PATH)/$(CHIP)
endif

ifeq ($(PRJ_NAME), $(filter $(PRJ_NAME), PRJ_CVR ))
	@$(CP) RESOURCE/$(PRJ_NAME)/A/RO_RES/IQ/$(CHIP)/$(IQ_NAME)/$(LENS_NAME)/LSC.BIN $(LSC_DOWN_PATH)/
	@$(CP) RESOURCE/$(PRJ_NAME)/A/RO_RES/IQ/$(CHIP)/$(IQ_NAME)/$(LENS_NAME)/AWBCTT.BIN $(RO_DOWN_PATH)/
	@$(CP) RESOURCE/$(PRJ_NAME)/A/RO_RES/IQ/$(CHIP)/$(IQ_NAME)/$(LENS_NAME)/AWBFILT.BIN $(RO_DOWN_PATH)/
	@$(CP) RESOURCE/$(PRJ_NAME)/A/RO_RES/IQ/$(CHIP)/$(IQ_NAME)/$(LENS_NAME)/AEALG.BIN $(RO_DOWN_PATH)/
	@$(CP) RESOURCE/$(PRJ_NAME)/A/RO_RES/IQ/$(CHIP)/$(IQ_NAME)/$(LENS_NAME)/WDRFT_DQ.BIN $(RO_DOWN_PATH)/
	@$(CP) RESOURCE/$(PRJ_NAME)/A/RO_RES/USBSTR.TXT $(RO_DOWN_PATH)/
	@$(CP) RESOURCE/$(PRJ_NAME)/A/RO_RES/USBUVC.BIN $(RO_DOWN_PATH)/
	@$(MKDIR) $(DOWN_PATH)/CIQTXT
	#@$(CP) RESOURCE/$(PRJ_NAME)/A/RO_RES/IQ/$(CHIP)/$(IQ_NAME)/$(LENS_NAME)/CIQTXT/*.*  $(DOWN_PATH)/CIQTXT/
	@$(RM) $(RO_DOWN_PATH)/$(CHIP)/$(IQ_NAME)/$(LENS_NAME)/AWBCTT.BIN
	@$(RM) $(RO_DOWN_PATH)/$(CHIP)/$(IQ_NAME)/$(LENS_NAME)/AWBFILT.BIN
	@$(RM) $(RO_DOWN_PATH)/$(CHIP)/$(IQ_NAME)/$(LENS_NAME)/AEALG.BIN
	@$(RM) $(RO_DOWN_PATH)/$(CHIP)/$(IQ_NAME)/$(LENS_NAME)/WDRFT_DQ.BIN
	@$(RMR) $(LSC_DOWN_PATH)/$(CHIP)/$(IQ_NAME)
	@$(RMR) $(LSC_DOWN_PATH)/$(CHIP)
endif

ifeq ($(PRJ_NAME), $(filter $(PRJ_NAME), RDK ))
	@$(CP) customization/hostfw/cathyware/RESOURCE/A/RO_RES/IQ/$(CHIP)/$(IQ_NAME)/LSC.BIN $(LSC_DOWN_PATH)/
	@$(CP) customization/hostfw/cathyware/RESOURCE/A/RO_RES/IQ/$(CHIP)/$(IQ_NAME)/AWBCTT.BIN $(RO_DOWN_PATH)/
	@$(CP) $(RO_DOWN_PATH)/IQ/$(CHIP)/$(IQ_NAME)/AEALG.BIN $(RO_DOWN_PATH)/
	@$(CP) customization/hostfw/cathyware/RESOURCE/A/RO_RES/IQ/$(CHIP)/$(IQ_NAME)/WDRFT_DQ.BIN $(RO_DOWN_PATH)/
	@$(CP) RESOURCE/$(PRJ_NAME)/A/RO_RES/USBSTR.TXT $(RO_DOWN_PATH)/
	@$(CP) RESOURCE/$(PRJ_NAME)/A/RO_RES/USBUVC.BIN $(RO_DOWN_PATH)/
	@$(CP) RESOURCE/$(PRJ_NAME)/A/RO_RES/USBUVC_TS.BIN $(RO_DOWN_PATH)/
	@$(CP) RESOURCE/$(PRJ_NAME)/A/RO_RES/USBUVC_PB.BIN $(RO_DOWN_PATH)/
	@$(RM) $(RO_DOWN_PATH)/$(CHIP)/$(IQ_NAME)/AWBCTT.BIN
	@$(RM) $(RO_DOWN_PATH)/$(CHIP)/$(IQ_NAME)/AEALG.BIN
	@$(RM) $(RO_DOWN_PATH)/$(CHIP)/$(IQ_NAME)/WDRFT_DQ.BIN
	@$(RMR) $(LSC_DOWN_PATH)/$(CHIP)/$(IQ_NAME)
	@$(RMR) $(LSC_DOWN_PATH)/$(CHIP)
endif

ifeq ($(PRJ_NAME), $(filter $(PRJ_NAME), EVB_DSC ))
	@$(CP) customization/hostfw/cathyware/RESOURCE/A/RO_RES/IQ/$(CHIP)/$(IQ_NAME)/LSC.BIN $(LSC_DOWN_PATH)/
	@$(CP) customization/hostfw/cathyware/RESOURCE/A/RO_RES/IQ/$(CHIP)/$(IQ_NAME)/AWBCTT.BIN $(RO_DOWN_PATH)/
	@$(CP) $(RO_DOWN_PATH)/IQ/$(CHIP)/$(IQ_NAME)/$(AE_RES_NAME) $(RO_DOWN_PATH)/AEALG.BIN
	@$(CP) customization/hostfw/cathyware/RESOURCE/A/RO_RES/IQ/$(CHIP)/$(IQ_NAME)/AWBFILT.BIN $(RO_DOWN_PATH)/
	@$(CP) customization/hostfw/cathyware/RESOURCE/A/RO_RES/IQ/$(CHIP)/$(IQ_NAME)/WDRFT_DQ.BIN $(RO_DOWN_PATH)/
	@$(CP) RESOURCE/$(addprefix $(PRJ_NAME), $(CHIP))/A/RO_RES/USBSTR.TXT $(RO_DOWN_PATH)/
	@$(CP) RESOURCE/$(addprefix $(PRJ_NAME), $(CHIP))/A/RO_RES/USBUVC.BIN $(RO_DOWN_PATH)/
	@$(CP) RESOURCE/$(addprefix $(PRJ_NAME), $(CHIP))/A/RO_RES/USBUVC_TS.BIN $(RO_DOWN_PATH)/
	@$(CP) RESOURCE/$(addprefix $(PRJ_NAME), $(CHIP))/A/RO_RES/USBUVC_PB.BIN $(RO_DOWN_PATH)/
	@$(MKDIR) $(DOWN_PATH)/CIQTXT
	@$(CP) RESOURCE/$(addprefix $(PRJ_NAME), $(CHIP))//A/RO_RES/IQ/$(CHIP)/$(IQ_NAME)/CIQTXT/*.*  $(DOWN_PATH)/CIQTXT/
	@$(RM) $(RO_DOWN_PATH)/$(CHIP)/$(IQ_NAME)/AWBCTT.BIN
	@$(RM) $(RO_DOWN_PATH)/$(CHIP)/$(IQ_NAME)/AWBFILT.BIN
	@$(RM) $(RO_DOWN_PATH)/$(CHIP)/$(IQ_NAME)/AEALG.BIN
	#@$(RM) $(RO_DOWN_PATH)/$(CHIP)/$(IQ_NAME)/WDRFT_DQ.BIN
	@$(RMR) $(LSC_DOWN_PATH)/$(CHIP)/$(IQ_NAME)
	@$(RMR) $(LSC_DOWN_PATH)/$(CHIP)
endif

ifeq ($(PRJ_NAME), $(filter $(PRJ_NAME), EVB_CVR ))
	@$(CP) customization/hostfw/cathyware/RESOURCE/A/RO_RES/IQ/$(CHIP)/$(IQ_NAME)/LSC.BIN $(LSC_DOWN_PATH)/
	@$(CP) customization/hostfw/cathyware/RESOURCE/A/RO_RES/IQ/$(CHIP)/$(IQ_NAME)/AWBCTT.BIN $(RO_DOWN_PATH)/
	@$(CP) $(RO_DOWN_PATH)/IQ/$(CHIP)/$(IQ_NAME)/$(AE_RES_NAME) $(RO_DOWN_PATH)/AEALG.BIN
	@$(CP) customization/hostfw/cathyware/RESOURCE/A/RO_RES/IQ/$(CHIP)/$(IQ_NAME)/AWBFILT.BIN $(RO_DOWN_PATH)/
	@$(CP) customization/hostfw/cathyware/RESOURCE/A/RO_RES/IQ/$(CHIP)/$(IQ_NAME)/WDRFT_DQ.BIN $(RO_DOWN_PATH)/
	@$(CP) RESOURCE/$(PRJ_NAME)/A/RO_RES/USBSTR.TXT $(RO_DOWN_PATH)/
	@$(CP) RESOURCE/$(PRJ_NAME)/A/RO_RES/USBUVC.BIN $(RO_DOWN_PATH)/
	@$(CP) RESOURCE/$(PRJ_NAME)/A/RO_RES/USBUVC_TS.BIN $(RO_DOWN_PATH)/
	@$(CP) RESOURCE/$(PRJ_NAME)/A/RO_RES/USBUVC_PB.BIN $(RO_DOWN_PATH)/
	@$(MKDIR) $(DOWN_PATH)/CIQTXT
	@$(CP) RESOURCE/$(PRJ_NAME)/A/RO_RES/IQ/$(CHIP)/$(IQ_NAME)/CIQTXT/*.*  $(DOWN_PATH)/CIQTXT/
	@$(RM) $(RO_DOWN_PATH)/$(CHIP)/$(IQ_NAME)/AWBCTT.BIN
	@$(RM) $(RO_DOWN_PATH)/$(CHIP)/$(IQ_NAME)/AWBFILT.BIN
	@$(RM) $(RO_DOWN_PATH)/$(CHIP)/$(IQ_NAME)/AEALG.BIN
	#@$(RM) $(RO_DOWN_PATH)/$(CHIP)/$(IQ_NAME)/WDRFT_DQ.BIN
	@$(RMR) $(LSC_DOWN_PATH)/$(CHIP)/$(IQ_NAME)
	@$(RMR) $(LSC_DOWN_PATH)/$(CHIP)
endif


ifeq ($(PRJ_NAME), $(filter $(PRJ_NAME), DVS5M2 ))
	@$(CP) RESOURCE/$(PRJ_NAME)/A/RO_RES/IQ/$(CHIP)/$(IQ_NAME)/$(LENS_NAME)/LSC.BIN $(LSC_DOWN_PATH)/
	@$(CP) RESOURCE/$(PRJ_NAME)/A/RO_RES/IQ/$(CHIP)/$(IQ_NAME)/$(LENS_NAME)/PLSC.BIN $(LSC_DOWN_PATH)/	
	@$(CP) RESOURCE/$(PRJ_NAME)/A/RO_RES/IQ/$(CHIP)/$(IQ_NAME)/$(LENS_NAME)/AWBCTT.BIN $(RO_DOWN_PATH)/
	@$(CP) RESOURCE/$(PRJ_NAME)/A/RO_RES/IQ/$(CHIP)/$(IQ_NAME)/$(LENS_NAME)/AWBFILT.BIN $(RO_DOWN_PATH)/
	@$(CP) RESOURCE/$(PRJ_NAME)/A/RO_RES/IQ/$(CHIP)/$(IQ_NAME)/$(LENS_NAME)/AEALG.BIN $(RO_DOWN_PATH)/
	@$(CP) RESOURCE/$(PRJ_NAME)/A/RO_RES/IQ/$(CHIP)/$(IQ_NAME)/$(LENS_NAME)/WDRFT_DQ.BIN $(RO_DOWN_PATH)/
	@$(CP) RESOURCE/$(PRJ_NAME)/A/RO_RES/USBSTR.TXT $(RO_DOWN_PATH)/
	@$(CP) RESOURCE/$(PRJ_NAME)/A/RO_RES/USBUVC.BIN $(RO_DOWN_PATH)/
	@$(CP) RESOURCE/$(PRJ_NAME)/A/RO_RES/USBUVC_TS.BIN $(RO_DOWN_PATH)/
	@$(CP) RESOURCE/$(PRJ_NAME)/A/RO_RES/USBUVC_PB.BIN $(RO_DOWN_PATH)/
	@$(MKDIR) $(DOWN_PATH)/CIQTXT
	@$(CP) RESOURCE/$(PRJ_NAME)/A/RO_RES/IQ/$(CHIP)/$(IQ_NAME)/$(LENS_NAME)/CIQTXT/*.*  $(DOWN_PATH)/CIQTXT/
	@$(RM) $(RO_DOWN_PATH)/$(CHIP)/$(IQ_NAME)/$(LENS_NAME)/AWBCTT.BIN
	@$(RM) $(RO_DOWN_PATH)/$(CHIP)/$(IQ_NAME)/$(LENS_NAME)/AWBFILT.BIN
	@$(RM) $(RO_DOWN_PATH)/$(CHIP)/$(IQ_NAME)/$(LENS_NAME)/AEALG.BIN
	@$(RM) $(RO_DOWN_PATH)/$(CHIP)/$(IQ_NAME)/$(LENS_NAME)/WDRFT_DQ.BIN
	@$(RMR) $(LSC_DOWN_PATH)/$(CHIP)/$(IQ_NAME)
	@$(RMR) $(LSC_DOWN_PATH)/$(CHIP)
endif



ifeq ($(PRJ_NAME), $(filter $(PRJ_NAME ,CHIP ), EVB_DSC ,6330 ))
       @$(CP) RESOURCE/$(PRJ_NAME)/A/RO_RES/USBUVC_TS.BIN $(RO_DOWN_PATH)/
       @$(CP) RESOURCE/$(PRJ_NAME)/A/RO_RES/USBUVC_PB.BIN $(RO_DOWN_PATH)/
endif

ifeq ($(PRJ_NAME), $(filter $(PRJ_NAME ,CHIP ), EVB_CVR ,6330 ))
       @$(CP) RESOURCE/$(PRJ_NAME)/A/RO_RES/USBUVC_TS.BIN $(RO_DOWN_PATH)/
       @$(CP) RESOURCE/$(PRJ_NAME)/A/RO_RES/USBUVC_PB.BIN $(RO_DOWN_PATH)/
endif

ifeq ($(CIQ_TXT_RESOURCE),YES)
	@$(MKDIR) $(DOWN_PATH)/CIQTXT
	@$(CP) RESOURCE/$(PRJ_NAME)/A/RO_RES/IQ/$(CHIP)/$(IQ_NAME)/$(LENS_NAME)/CIQTXT/*.*  $(DOWN_PATH)/CIQTXT/
	@$(CP) RESOURCE/$(PRJ_NAME)/A/RO_RES//IQ/$(CHIP)/$(IQ_NAME)/$(LENS_NAME)/CIQVER.DAT $(RO_DOWN_PATH)/ 
endif

FRM_PARTIAL_FILELIST: $(DOWN_PATH)/FRM.exe
	@cd $(DOWN_PATH) && \
	sed '/\[PARTIAL_FILELIST\]/,99999 d' $(FRM_INI) > $(FRM_INI).bak && \
	./reslist.exe -[PARTIAL_FILELIST] BADPIX COLD.BIN DEFPIX1 DEFTPXL \
		>> $(FRM_INI).bak && \
	$(MV) $(FRM_INI).bak $(FRM_INI)
endif #ifneq ($(RES),NO)


#---------------------------------------------------------------------------
# Implicit rules
#---------------------------------------------------------------------------
ifeq ($(MULTI_BOOT),YES)

define STAGE_OBJ_RULES

 $$(OBJS_PATH)/%.stage$(1).d: CFLAGS += -DMULTI_BOOT_STAGE=$(1)
 $$(OBJS_PATH)/%.stage$(1).o: CFLAGS += -DMULTI_BOOT_STAGE=$(1)

 $$(OBJS_PATH)/%.stage$(1).d: %.c $$(CFG_HEADERS) $(IO_HEADER)
	@$$(MKDIR) $$(dir $$@)
	@$$(CC) $$(CFLAGS) -MM -MT '$$(@:.d=.o) $$@' -o $$@ $$<

 $$(OBJS_PATH)/%.stage$(1).d: %.S $$(CFG_HEADERS) $(IO_HEADER)
	@$$(MKDIR) $$(dir $$@)
	@$$(CC) $$(ASFLAGS) $$(CFLAGS) -MM -MT '$$(@:.d=.o) $$@' -o $$@ $$<

 $$(OBJS_PATH)/%.stage$(1).o: %.c $$(OBJS_PATH)/%.d $(IO_HEADER)
	$$(MSG_SPLIT_LINE)
	$$(MSG_COMPILING)
	@$$(CC) -c $$(CFLAGS) -o $$@ $$<

 $$(OBJS_PATH)/%.stage$(1).o: %.S $$(OBJS_PATH)/%.d $(IO_HEADER)
	$$(MSG_SPLIT_LINE)
	$$(MSG_COMPILING)
	@$$(CC) -c $$(ASFLAGS) $$(CFLAGS) -o $$@ $$<

endef

$(foreach stage,$(STAGE_LIST),$(eval $(call STAGE_OBJ_RULES,$(stage))))

endif

$(OBJS_PATH)/%.d: %.c $(CFG_HEADERS) $(IO_HEADER)
	@$(MKDIR) $(dir $@)
	@$(CC) $(CFLAGS) -MM -MT '$(@:.d=.o) $@' -o $@ $<

$(OBJS_PATH)/%.d: %.S $(CFG_HEADERS) $(IO_HEADER)
	@$(MKDIR) $(dir $@)
	@$(CC) $(ASFLAGS) $(CFLAGS) -MM -MT '$(@:.d=.o) $@' -o $@ $<

$(OBJS_PATH)/%.o: %.c $(OBJS_PATH)/%.d  $(IO_HEADER)
	$(MSG_SPLIT_LINE)
	$(MSG_COMPILING)
	@$(CC) -c $(CFLAGS) -o $@ $<

$(OBJS_PATH)/%.o: %.S $(OBJS_PATH)/%.d $(IO_HEADER)
	$(MSG_SPLIT_LINE)
	$(MSG_COMPILING)
	@$(CC) -c $(ASFLAGS) $(CFLAGS) -o $@ $<


# project dependent config and header files
.PHONY: CFG_FILES $(CFG_DEFS) $(CFG_HEADERS) $(BUILD_CFG_FILES)
CFG_FILES: $(CFG_DEFS) $(CFG_HEADERS)
$(CFG_DEFS) $(CFG_HEADERS): %: $(DEST_DIR)/%
$(BUILD_CFG_FILES): $(DEST_DIR)/%:
	@if [ -f "$@" ]; then \
	  if diff -duN $@.base $(HOST_ROOT)/$*.tmpl > $@.patch; then \
	    $(RM) $@.patch; \
	  else \
	    $(ECHO) "$@: new version '$*.tmpl' differs, merging.."; \
	    if patch --verbose -u -i $@.patch -r $@.rej -o $@.merge $@; then \
	      $(RM) $@.patch; \
	      $(CP) $(HOST_ROOT)/$*.tmpl $@.base && \
	      $(MV) $@.merge $@; \
	      if [ $$? != 0 ] ; then \
	        exit 1 ; \
	      fi \
	    else \
	      $(ECHO) "$@: error: new '$*.tmpl' conflicts"; \
	      $(ECHO) "$@: error: see '$@.{merge,rej}'"; \
	    fi \
	  fi \
	else \
	  $(ECHO) "$@: './config.sh', 'make PRJ=xxx' not run, use default"; \
	  $(MKDIR) $(dir $@) && \
		$(CP) -u $(HOST_ROOT)/$*.tmpl $@.base && \
		$(CP) $@.base $@; \
	  if [ $$? != 0 ] ; then \
	    exit 1 ; \
	  fi \
	fi

# cancelling implicit rules to build source files
$(LIB_PATH)/%: ;

showvars:
	@echo LIBS=${LIBS}
	@echo
	@echo CFLAGS=${CFLAGS}
	@echo
	@echo BUILD_GEN_OBJS=${BUILD_GEN_OBJS}
	@echo
	@echo DEST_SUBLIB_DIR=${DEST_SUBLIB_DIR}
	@echo
	@echo CUSTOM_SUBLIBS=${CUSTOM_SUBLIBS}
	@echo
	@echo ICATWARE=$(ICATWARE)

.PHONY: clean
clean: clean_resource
	$(MSG_SPLIT_LINE)
	#@$(MAKE) -C $(HOST_FW_PATH)/APP/Device -f MakefileIO $(MAKECMDGOALS)
	@for subdir in $(dir $(SUBLIBS) $(CUSTOM_SUBLIBS)) $(SUBDIRS); do \
		echo "Making $@ in $$subdir"; \
		$(MAKE) -C $$subdir $@; \
	done;
	@$(RM) $(BUILD_OBJS:.o=.d) $(BUILD_OBJS)
	@$(RM) $(OBJS:.o=.d) $(OBJS)
	@$(RM) $(BUILD_GOAL).*{map,elf,text,bin,nm}* *.{bin,hex,tmp}
	@$(RM) $(DOWN_PATH)/$(PRJ_NAME){,.stage*}.bin{,*.bak}
	@$(RM) $(BUILD_GOAL).stagelist $(BUILD_GOAL).stagelist.bak
	@$(RM) $(BUILD_GOAL).multiboot.bak
ifneq (,$(strip $(BUILD_BAK_LIBS)))
	@$(RMR) $(foreach lib,$(BUILD_BAK_LIBS),\
		$(dir $(lib))_multiboot.bak/$(notdir $(lib:.a=)))
endif
	@$(RM) $(DEST_DIR)/sumpatch.dat
	@$(RM) $(DEST_DIR)/multiload.dat
	@$(RMR) $(dir $(DEST_DIR)/$(LDSCRIPT))
	@$(RMR) $(DEST_SUBLIB_DIR)
	@$(RMR) $(OBJS_PATH)
	@$(RM) $(DOWN_PATH)/$(PLATFORM)isp.bin $(DOWN_PATH)/$(FRM_INI) $(DOWN_PATH)/frm_used.txt $(DOWN_PATH)/*.prm
	@$(RMR) $(DOWN_PATH)
	@$(RMR) _$(PLATFORM_PATH)

.PHONY: clean_resource
clean_resource:
	@$(MAKE) -f MakefileRes $(MAKECMDGOALS)


ifeq (,$(filter $(BUILD_SUBLIBS) $(BUILD_CUSTOM_SUBLIBS) $(SUBLIBS) \
	$(CUSTOM_SUBLIBS) $(dir $(SUBLIBS) $(CUSTOM_SUBLIBS)) $(SUBDIRS) \
	clean clean_resource resource RES_% io $(IO_HEADER),$(MAKECMDGOALS)))
-include $(BUILD_OBJS:.o=.d)

ifeq ($(MULTI_BOOT),YES)

define STAGE_DEP_RULES
-include $$(BUILD_STAGE$(1)_OBJS:.o=.d)
endef

ifeq ($(OBJMAP),NO)
$(eval $(call STAGE_DEP_RULES,$(LAST_STAGE)))
else
$(foreach stage, $(STAGE_LIST), $(eval $(call STAGE_DEP_RULES,$(stage))))
endif

endif
endif
