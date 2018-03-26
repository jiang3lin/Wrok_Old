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


HOST_ROOT := ../..
include $(HOST_ROOT)/host.def
ifndef PLATFORM_PATH
PLATFORM_PATH := $(PLATFORM)
endif
PLATFORM_ROOT := $(HOST_ROOT)/../$(PLATFORM_PATH)
TOOL_PATH := $(HOST_ROOT)/../tool

# output
SRC_DIR   := .
DEST_DIR  := $(HOST_ROOT)/_$(PRJ_NAME)
OBJS_PATH := $(DEST_DIR)/objs

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
GOAL := libhostfw.a

#CFLAGS += -Wall
CFLAGS += -mgp32
#CFLAGS += -ansi
CFLAGS += -G 8
CFLAGS += -DAPP_LUCKY7=1

ifeq ($(usboot), 1)
CFLAGS += -DOPT_USB_BOOT=1
else
CFLAGS += -DOPT_USB_BOOT=0
endif

CFLAGS += -I$(PLATFORM_ROOT)/inc
CFLAGS += -I$(PLATFORM_ROOT)/inc/api
CFLAGS += -I$(HOST_ROOT)/include
CFLAGS += -I$(HOST_ROOT)/include/gsensor
CFLAGS += -I$(HOST_ROOT)/include/rf
CFLAGS += -I./device
CFLAGS += -I./device/lens
CFLAGS += -I../../include
CFLAGS += -I../../include/customization
CFLAGS += -I./EyeFiCard/include
CFLAGS += -I./EyeFiCard/src/core
CFLAGS += -I./EyeFiCard/src/os/SP5K

CFLAGS += -G 8
CFLAGS += -DAPP_BUILD=1

CFLAGS += -I./include
CFLAGS += -I./cathyware/inc
CFLAGS += -I./cathyware/inc/net

CGDBFLAGS = $(CFLAGS)
CGDBFLAGS += -g

CFLAGS += -gstabs+
CFLAGS += -O2

ifeq ($(GDB), YES)
CFLAGS += -DCMD_GDB=1
else
CFLAGS += -DCMD_GDB=0
endif

#iCatch Menu/UI Utilities(IMU) Generated(GEN) files
#Phil: Because the IMU can not support multi CPU cores compiling process
#      We marked the last four files for work around
#      This file rules could still work well in most cases.
#
IMU_GEN_FILES :=
IMU_GEN_FILES += ui/menu/app_menu_tab.c
#IMU_GEN_FILES += include/app_menu_tab.h
#IMU_GEN_FILES += include/app_menu_event.h
#IMU_GEN_FILES += include/app_icon_def.h
#IMU_GEN_FILES += include/app_ui_para.h

#iCatch Menu/UI Utilities(IMU) depend(DPD) files and paths
#Host add new differnet height icon path here.
#
IMU_DPD_FILES :=
ifeq ($(PRJ_NAME), EVB_DSC)
IMU_DPD_FILES += $(HOST_ROOT)/resource/$(PRJ_NAME)6330/MENU/icatchtek.xls
else
IMU_DPD_FILES += $(HOST_ROOT)/resource/$(PRJ_NAME)/MENU/icatchtek.xls
endif
IMU_DPD_FILES += $(HOST_ROOT)/script/menu_install.bat
IMU_DPD_FILES += $(HOST_ROOT)/resource/A/RO_RES/UI/ICON_SRC/32
#IMU_DPD_FILES += $(HOST_ROOT)/resource/A/RO_RES/UI/ICON_SRC/48

#Data stamp font Generated(GEN) files
#Host add new differnet height font date stamp here.
DS_GEN_PATH := $(HOST_ROOT)/resource/A/RO_RES/UI/FONT
DS_GEN_FILES :=
#DS_GEN_FILES += $(DS_GEN_PATH)/DATESTAMP_16.SFN
#DS_GEN_FILES += $(DS_GEN_PATH)/DATESTAMP_32.SFN
DS_GEN_FILES += $(DS_GEN_PATH)/DATESTAMP_48.SFN
#DS_GEN_FILES += $(DS_GEN_PATH)/DATESTAMP_64.SFN

#Data stamp font depend(DPD) files and paths
#
DS_DPD_PATH := $(HOST_ROOT)/resource/A/RO_RES/UI/FONT
DS_DPD_FILES :=
DS_DPD_FILES += $(DS_DPD_PATH)/DS_UNICODE.TXT
DS_DPD_FILES += $(HOST_ROOT)/script/ds_gen.bat

#---------------------------------------------------------------------------
# Intermediate objects
#---------------------------------------------------------------------------
OBJS :=
OBJS += app_ver.o
OBJS += sys/app_main.o
OBJS += sys/app_msg2key.o
OBJS += sys/app_power_on.o
OBJS += sys/app_power_off.o
OBJS += sys/app_batcharging.o
OBJS += sys/app_fast_boot.o
OBJS += sys/app_msg.o
ifeq ($(V33), YES)
OBJS += sys/app_ptp.o
endif
OBJS += sys/state_ctrl/app_state_ctrl.o
OBJS += sys/state_ctrl/app_state_null.o
OBJS += sys/state_ctrl/app_state_preproc.o
OBJS += sys/state_ctrl/app_state_postproc.o
#ifeq ($(APPLOAD_FACETRACK), YES)
#OBJS += sys/dynamic_load/app_load_facedetect.o
#endif
OBJS += device/app_disk.o
OBJS += device/app_timer.o
OBJS += device/app_timer_isr.o
OBJS += device/app_key.o
OBJS += device/app_key_name.o
OBJS += device/app_gpio.o
OBJS += device/app_tvlcd.o
ifeq ($(GPS),1)
OBJS += device/app_gps.o
endif
ifeq ($(IR_REMOTE),1)
OBJS += device/app_ir_key.o
endif
OBJS += device/app_sound.o
OBJS += device/app_sensor.o
OBJS += device/app_battery.o
OBJS += device/app_dev_plug.o
OBJS += device/app_buzzer.o
OBJS += device/app_uvc_request.o
OBJS += device/lens/app_lens_api.o	# device_lens section
OBJS += device/lens/app_lens_io.o
OBJS += device/lens/app_lens_zoom.o
OBJS += device/lens/app_lens_focus.o
OBJS += device/lens/app_lens_mshut.o
OBJS += device/lens/app_lens_iris.o
OBJS += device/lens/app_lens_debug.o
OBJS += device/lens/app_lens_motor_dir.o
OBJS += device/lens/app_lens_motor_md3in4.o
OBJS += device/lens/app_lens_motor_dummy.o
OBJS += device/lens/app_lens_motor_sh_iris.o
OBJS += device/app_strobe.o
OBJS += device/app_rtc.o
OBJS += device/app_led.o
OBJS += device/app_gSensor.o
OBJS += service/ts/app_ts_init.o
OBJS += service/ts/app_ts_gui.o
OBJS += service/ts/app_ts_evt.o
OBJS += service/ts/app_ts_api.o
OBJS += service/ts/app_ts_util.o
OBJS += service/ts/app_ts_cal.o
OBJS += service/ts/app_ts_ver.o
OBJS += service/misc/app_misc.o
OBJS += service/misc/app_msg_ext.o
OBJS += service/misc/app_sio_filter.o
OBJS += service/misc/app_custom_cmd.o
OBJS += service/misc/app_param_manager.o
OBJS += service/misc/app_view_param.o
OBJS += service/misc/app_background_timer.o
OBJS += service/misc/app_user_setting.o
OBJS += service/misc/app_errorosd.o
OBJS += service/misc/app_error_msg.o
OBJS += service/misc/app_jpg_draw.o
OBJS += service/misc/app_card_isp.o
OBJS += service/misc/app_simple_menu.o
OBJS += service/misc/app_yuvscale.o
OBJS += service/misc/app_facedetect_ex.o
OBJS += service/misc/app_modeSet.o
OBJS += service/misc/tg_cmd.o
OBJS += service/misc/app_battery_ctrl.o
OBJS += service/misc/app_battery_charge.o
OBJS += service/misc/app_sd_log.o
OBJS += service/misc/app_mtbf_cmd.o
OBJS += service/cvrws/app_cvrws_api.o
ifeq ($(CVR_FCWS), YES)
OBJS += service/cvrws/app_fcws.o
endif
ifeq ($(CVR_LDWS), YES)
OBJS += service/cvrws/app_ldws.o
endif
OBJS += service/osd/app_osd_init.o
OBJS += service/osd/app_osd_lib.o
OBJS += service/osd/app_osd_api.o
OBJS += service/osd/app_osd_layout.o
OBJS += service/exif/app_exif.o
OBJS += service/dcf/app_dcf.o
OBJS += service/cdfs/app_cdfs_api.o
OBJS += service/dpof/app_dpof.o
OBJS += ui/still/app_zoom_api.o
OBJS += ui/still/app_still_view.o
OBJS += ui/still/app_still_timelapse.o
OBJS += ui/still/app_still_callback.o
OBJS += ui/still/app_still_capture.o
OBJS += ui/still/app_still_burst.o
OBJS += ui/still/app_still_setting.o
OBJS += ui/still/app_view_osd.o
OBJS += ui/still/app_still_background.o
OBJS += ui/still/app_still_busy.o
OBJS += ui/still/app_still_cwb.o
OBJS += ui/still/app_still_aaa.o
OBJS += ui/still/app_still_self_timer.o
OBJS += ui/still/app_still_flow.o
#OBJS += ui/still/app_still_flash.o
ifneq ($(CVR),1)
OBJS += ui/still/app_still_smile_detect.o
endif
OBJS += ui/still/app_still_strobe_test.o
OBJS += ui/still/app_still_util.o
ifneq ($(CVR),1)
OBJS += ui/still/app_still_blink_detect.o
endif
OBJS += ui/still/app_still_hdr.o
ifeq ($(V33), YES)
OBJS += ui/still/app_still_panorama.o
endif
OBJS += ui/video/app_video_view.o
OBJS += ui/video/app_video_record.o
OBJS += ui/video/app_video_setting.o
OBJS += ui/video/app_video_osd.o
ifneq ($(CVR),1)
OBJS += ui/audio/app_audio_record.o
OBJS += ui/audio/app_audio_osd.o
endif
OBJS += ui/play/app_playback_main.o
ifneq ($(CVR),1)
OBJS += ui/play/app_playback_zpan.o
OBJS += ui/play/app_playback_multi.o
endif
OBJS += ui/play/app_playback_prot.o
OBJS += ui/play/app_playback_del.o
OBJS += ui/play/app_playback_c2c.o
ifneq ($(CVR),1)
OBJS += ui/play/app_playback_slide.o
OBJS += ui/play/app_playback_edit.o
endif
OBJS += ui/play/app_playback_video.o
OBJS += ui/play/app_playback_trimvideo.o
ifneq ($(CVR),1)
OBJS += ui/play/app_playback_audio.o
OBJS += ui/play/app_playback_memorec.o
OBJS += ui/play/app_playback_startimg.o
OBJS += ui/play/app_playback_dpof.o
endif
OBJS += ui/play/app_playback_lib.o
OBJS += ui/play/app_playback_osd.o # ui_playback MENU & OSD section
OBJS += ui/menu/app_ui_para.o
OBJS += ui/menu/app_menu_state.o
OBJS += ui/menu/app_menu_tab.o
OBJS += ui/menu/app_menu_core.o
OBJS += ui/menu/app_menu_disp.o
ifeq ($(CVR), 1)
OBJS += ui/menu/app_menu_event_cvr.o
OBJS += ui/menu/app_menu_active_cvr.o
else
OBJS += ui/menu/app_menu_event_dsc.o
OBJS += ui/menu/app_menu_active_dsc.o
endif

OBJS += ui/setup/app_setup_lib.o
OBJS += ui/usb/app_usb_create.o
OBJS += ui/usb/app_usb_msdc.o
OBJS += ui/usb/app_usb_pccam.o
ifneq ($(CVR),1)
OBJS += ui/usb/app_usb_dps.o
OBJS += ui/usb/app_usb_dpslib.o
OBJS += ui/usb/app_usb_dpsmenu.o
OBJS += ui/usb/app_usb_dps_2nd_menu.o
OBJS += ui/usb/app_usb_dpssubmenu.o
OBJS += ui/usb/app_usb_dpsimagesel.o
OBJS += ui/usb/app_usb_dpsprint.o
OBJS += ui/usb/app_usb_dps_osd.o
endif
OBJS += ui/hot_plug/app_tv_plug.o
ifeq ($(CHIP), 6330)
OBJS += ui/hot_plug/app_HDMI_plug.o
OBJS += ui/hot_plug/app_cec_capability_ctrl.o
OBJS += ui/layout/app_lyt_test.o
OBJS += ui/layout/app_lyt_show.o
endif
OBJS += ui/hot_plug/app_card_plug.o
#OBJS += EyeFiCard/src/os/SP5K/Utils.o
#OBJS += EyeFiCard/src/os/SP5K/DeviceIO.o
#OBJS += EyeFiCard/src/core/Comm.o
#OBJS += EyeFiCard/src/core/EyeFiCard.o
ifeq ($(ALGORITHM_DUAL_AE), 1)
OBJS += aaa/app_ae_dual.o
else
OBJS += aaa/app_ae.o
endif
OBJS += aaa/app_af.o
OBJS += aaa/app_awb.o
OBJS += aaa/app_still_awb.o
OBJS += aaa/app_still_flash.o
OBJS += service/testbed/app_tb_main.o
OBJS += service/testbed/app_tb_util.o
OBJS += service/testbed/app_tb_flash.o
OBJS += service/testbed/app_tb_sensor.o
OBJS += service/testbed/app_tb_af.o
OBJS += service/testbed/app_tb_batcurve.o
OBJS += service/testbed/app_tb_msh.o
OBJS += service/testbed/app_tb_cipa.o
OBJS += service/script/app_script.o
OBJS += service/script/app_script_lib.o
ifeq ($(WIFI),1)
OBJS += ui/wifi/app_WiFi_connection.o
endif

#for DVS5M2
OBJS += ui/menu/app_menu_event_dvs5m2.o
OBJS += ui/menu/app_menu_active_dvs5m2.o

OBJS += service/osd/app_osd_api_lcm.o
OBJS += service/osd/app_osd_draw_lcm.o
OBJS += ui/still/app_sdv_still_view.o
OBJS += ui/still/app_sdv_still_cap.o
OBJS += ui/still/app_sdv_still_cap_lapse.o
OBJS += ui/still/app_sdv_still_cap_burst.o
OBJS += ui/still/app_sdv_still_cap_self_timer.o
OBJS += ui/video/app_sdv_video_view.o
OBJS += ui/video/app_sdv_video_rec.o
OBJS += ui/video/app_sdv_video_rec_slow.o
OBJS += ui/wifi/app_sdv_wifi_mode.o
OBJS += ui/play/app_sdv_pb_delete.o
OBJS += ui/setup/app_sdv_system.o

OBJS += ui/sys/app_scdv_card_isp.o
OBJS += ui/sys/app_scdv_global.o

OBJS += ui/menu/app_scdv_menu_state.o
OBJS += ui/menu/app_scdv_menu_disp.o

OBJS += ui/video/app_cdv_video_view.o
OBJS += ui/video/app_cdv_video_rec.o
OBJS += ui/video/app_cdv_video_rec_lapse.o
#OBJS += ui/video/app_sdv_video_rec_lapse.o
OBJS += ui/wifi/app_cdv_wifi_mode.o
OBJS += ui/play/app_cdv_pb_delete_loop.o
OBJS += ui/play/app_cdv_pb_delete_event.o
OBJS += ui/setup/app_cdv_system.o

OBJS += ui/play/app_scdv_pb_main.o
OBJS += ui/play/app_scdv_pb_video.o

OBJS += ui/rf/app_scdv_rf_pairing.o

#---------------------------------------------------------------------------
# Explicit rules
#---------------------------------------------------------------------------
.PHONY: all

# IMU resource file rules
all: $(IMU_GEN_FILES) $(DS_GEN_FILES)

$(IMU_GEN_FILES): $(IMU_DPD_FILES)
	@ECHO "make resource files for IMU start"
	@../../script/menu_install.bat ..\\..\\script\\
	@ECHO "make resource files for IMU over"

$(DS_GEN_FILES): $(DS_DPD_FILES)
	@ECHO "make resource files for date stamp start"
	@../../script/ds_gen.bat ..\\..\\script\\
	@ECHO "make resource files for date stamp over"

.PHONY: clean clean_IMU

clean: clean_IMU

clean_IMU:
	@ECHO "Clear IMU resource files"
	@$(HOST_ROOT)/script/menu_clean.bat ..\\..\\script\\

#---------------------------------------------------------------------------
# Referring to generic rules
#---------------------------------------------------------------------------
$(eval $(call GEN_GOAL_RULES))
