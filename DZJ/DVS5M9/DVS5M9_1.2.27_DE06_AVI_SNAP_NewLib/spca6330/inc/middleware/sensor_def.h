/**************************************************************************
 *                                                                        *
 *        Copyright (c) 2004-2006 by Sunplus Technology Co., Ltd.         *
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
 *  Hsin-Chu, Taiwan                                                      *
 *                                                                        *
 *  Author: Anmin Deng                                                    *
 *                                                                        *
 **************************************************************************/
#ifndef _SENSOR_DEF_H_
#define _SENSOR_DEF_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"

typedef enum {
	SENSOR_DEV_ID_SENSOR,
	SENSOR_DEV_ID_TV_IN,
	SENSOR_DEV_ID_SIGGEN,
	SENSOR_DEV_ID_RESERVED,
	SENSOR_DEV_ID_TOTAL,
} sensorDevId_t;

typedef enum {
	SENSOR_MODE_IDLE = 0, /*0,static default*/
	SENSOR_MODE_POWERSAVE = 0x10,
	SENSOR_MODE_STOP = 0x20,
	SENSOR_MODE_PREVIEW = 0x30,
	SENSOR_MODE_SNAP = 0x40,
} sensorMode_t;

#define SENSOR_MCLK_SRC_NOT_CONNECT 0
#define SENSOR_MCLK_SRC_SP5K_TGPLL  1
#define SENSOR_MCLK_SRC_ADCLPI_PAD  2
void sensorMclkSrcCfgSet(UINT32 src);

#define SENSOR_SYNC_SIG_SP5K_SLAVE  0
#define SENSOR_SYNC_SIG_SP5K_MASTER 1
void sensorSyncSigMasterCfgSet(UINT32 master);

typedef enum {
	SENSOR_DEV_POWER_ID_SYS_ON  = 0,
	SENSOR_DEV_POWER_ID_RST_ON  = 1,
	SENSOR_DEV_POWER_ID_SEN_ON  = 2,
	SENSOR_DEV_POWER_ID_HDR_ON  = 3,
	SENSOR_DEV_POWER_ID_AFE_WAKEUP = 4,
	SENSOR_DEV_POWER_ID_TGPLL_ON = 5,
	SENSOR_DEV_POWER_ID_TGPLL_OFF = 0x85,
	SENSOR_DEV_POWER_ID_AFE_SLEEP = 0x84,
	SENSOR_DEV_POWER_ID_HDR_OFF = 0x83,
	SENSOR_DEV_POWER_ID_SEN_OFF = 0x82,
	SENSOR_DEV_POWER_ID_RST_OFF = 0x81,
	SENSOR_DEV_POWER_ID_SYS_OFF = 0x80,
} sensorDevPowerId_t;
UINT32 sensorDevPowerCustomCb(UINT32 id);


/*cfg type in tgSvc/afeSvc-> UINT32 (*devcfg)(UINT32 cfg,UINT32 val);*/
typedef enum {
	SENSOR_DEV_CFG_TG_PWR_SAVE, /*val==0:normal;*/
	SENSOR_DEV_CFG_AFE_PWR_SAVE,
	SENSOR_DEV_CFG_RGBOUT_FLOAT, /*val==0:normal;*/
	SENSOR_DEV_CFG_RGBOUT_TYPE, /*val==0:normal binary;1:gray, etc*/
	SENSOR_DEV_CFG_SNAP_READOUT_FIELD_REPEAT,
	SENSOR_DEV_CFG_TOTAL,
} sensorDevCfg_t;

typedef enum {
	SENSOR_SYS_CFG_AUTO_PWRSAVE_TIMEOUT,
	SENSOR_SYS_CFG_BSHUT_MAX_TIMEOUT,
	SENSOR_SYS_CFG_EVSET_SMOOTH,
	SENSOR_SYS_CFG_SHUTTER_EARLY_FRAME,
	SENSOR_SYS_CFG_MODE_SYNC_EARLY,
	SENSOR_SYS_CFG_FLOW_USE_ABORT,
	SENSOR_SYS_CFG_FLOW_IMMEDIATE_IDLE,
	SENSOR_SYS_CFG_SENSOR_PAUSE,
	SENSOR_SYS_CFG_DUAL_SENSOR_AE_EN,	
	SENSOR_SYS_CFG_TOTAL,
} sensorStsCfg_t;

typedef enum {
	SENSOR_MODESW_NO_KEEP,	/* sensor change as ori ModeSw set */
	SENSOR_MODESW_KEEP,		/* sensor no need to change */
	SENSOR_MODESW_CHANGE_AT_PVENTER,
	SENSOR_MODESW_CHANGE_AT_PVEXIT, /* sensor  change early */
	SENSOR_MODESW_TOTAL,
} SENSOR_MODESW_t;


typedef enum {
	SENSOR_IO_VSUBCTRL1,
	SENSOR_IO_VSUBCTRL2,
	SENSOR_IO_VSUBCTRL3,
	SENSOR_IO_EXTEND,
} sensorIoCtrl_t;


typedef enum {
	SENSOR_DEV_CFG_CUSTOM_TG_INTERNAL,
} sensorDevCustomCfg_t;

typedef struct {
	/* FIXME: only support vertical line */
	UINT16 voffset;
	UINT16 botStart; /*bottom start */
	UINT16 botLen;   /*bottom length*/
	UINT16 topEnd; /*top end*/
	UINT16 topLen; /*top length*/
	UINT16 viewStart; /*view start from voffset*/
	UINT16 viewEnd; /*view end from voffset*/
} sensorTgDummy_t;

typedef struct {
	UINT16 hoffset;
	UINT16 voffset;
	UINT16 hsize; /*2^hsize, hsize=0~14*/
	UINT16 vsize; /*2^vsize, vsize=0~14, hsize+vsize<=16*/
} sensorTgWin_t;

typedef struct {
	UINT8 fieldnum;
	UINT8 fieldsel[16];
} sensorTgFieldSel_t;

typedef enum {
	SENSOR_CAP_EXT_ID_DUMMY, /*sensor preview dummy*/
	SENSOR_CAP_EXT_ID_OBWIN, /*sensor preview ob window*/
	SENSOR_CAP_EXT_ID_FIELDSEL, /*sensor capture fieldsel */
	SENSOR_CAP_EXT_ID_CROP, /*sensor preview crop size for 6330 mode 9 pv bp issue */
	SENSOR_CAP_EXT_TOTAL,
} sensorCapExtId_t;

UINT32 sensorDevIoCtrl(sensorIoCtrl_t devIo, UINT32 opt);
UINT32 sensorDevCfgCustom(sensorDevCustomCfg_t sel, UINT32 opt2);

/** \param cfg is one of sensorStsCfg_t */
UINT32 sensorStsCfg(UINT32 cfg,UINT32 val);


#ifdef __cplusplus
}
#endif

#endif
