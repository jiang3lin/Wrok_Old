/**************************************************************************
 *
 *       Copyright (c) 2007-2011 by iCatch Technology, Inc.
 *
 *  This software is copyrighted by and is the property of iCatch
 *  Technology, Inc.. All rights are reserved by iCatch Technology, Inc..
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
 *  Author: Edward Kuo
 *
 **************************************************************************/
#ifndef _SP5K_AUD_API_H_
#define _SP5K_AUD_API_H_

#include "middleware/aud_dev_def.h"
#include "middleware/aud_filter_def.h"

/**************************************************************************
 *                           C O N S T A N T S
 **************************************************************************/
#define SP5K_AUD_DEV_REC   0x80000000
#define SP5K_AUD_DEV_PLAY  0x80000010

#define SP5K_AUD_DEV_REC_VOL_BOOST	0x80000020
#define SP5K_AUD_DEV_REC_VOL_BOOST_DB	0x80000030


#define SP5K_AUD_DEV_SPEAKER_ON_PIN  ( 0x00 )

typedef enum _sp5kAudDevRecSrc_e {
	SP5K_AUD_DEV_REC_SRC_NONE    = AUD_IN_NONE,
	SP5K_AUD_DEV_REC_SRC_LINE_IN = AUD_IN_LINE,
	SP5K_AUD_DEV_REC_SRC_MIC_IN  = AUD_IN_MIC,
} sp5kAudDevRecSrc_e;

typedef enum _sp5kAudDevPlaySrc_e {
	SP5K_AUD_DEV_PLAY_SRC_NONE      = AUD_OUT_NONE,
	SP5K_AUD_DEV_PLAY_SRC_SPEAKER   = AUD_OUT_SPEAKER,
	SP5K_AUD_DEV_PLAY_SRC_HEADPHONE = AUD_OUT_HEADPHONE,
	SP5K_AUD_DEV_PLAY_SRC_LINEOUT   = AUD_OUT_LINEOUT,
} sp5kAudDevPlaySrc_e;

/**
 For sp5kAudActDevSet()
 direct map to audDevId_e
 @sa audDevId_e
*/
typedef enum _sp5kAudDevId_e {
	SP5K_AUD_DEV_PRIMARY   = AUD_DEV_PRIMARY,
	SP5K_AUD_DEV_SECONDARY = AUD_DEV_SECONDARY,
	SP5K_AUD_DEV_TOT,
} sp5kAudDevId_e;

/**
 direct map to audRecChnl_e
 @sa audRecChnl_e
*/
typedef enum _sp5kAuRecdChnl_e {
	SP5K_AUD_REC_CH_LEFT  = AUD_REC_CH_LEFT,
	SP5K_AUD_REC_CH_RIGHT = AUD_REC_CH_RIGHT,
	SP5K_AUD_REC_CH_ALL   = AUD_REC_CH_ALL,
} sp5kAudRecChnl_e;  

/**
 direct map to audPlayChnl_e
 @sa audPlayChnl_e
*/
typedef enum _sp5kAudPlayChnl_e {
	SP5K_AUD_PLAY_CH_LEFT  = AUD_PLAY_CH_LEFT,
	SP5K_AUD_PLAY_CH_RIGHT = AUD_PLAY_CH_RIGHT,
	SP5K_AUD_PLAY_CH_ALL   = AUD_PLAY_CH_ALL,
} sp5kAudPlayChnl_e;

/**
 direct map to audFilterChnl_e
 @sa audFilterChnl_e
*/
typedef enum _sp5kAudFilterChnl_e {
	SP5K_AUD_FILTER_FIRST  = AUD_FILTER_FIRST,
	SP5K_AUD_FILTER_SECOND = AUD_FILTER_SECOND,
	SP5K_AUD_FILTER_ALL    = AUD_FILTER_ALL,
} sp5kAudFilterChnl_e;

typedef enum {
	SP5K_AUD_STATUS_UNKNOWN    = 0,
	SP5K_AUD_STATUS_PLAY_STOP  = (1 << 0),
	SP5K_AUD_STATUS_PLAY_START = (1 << 1),
	SP5K_AUD_STATUS_PLAY_PAUSE = (1 << 2),
	SP5K_AUD_STATUS_REC_STOP   = (1 << 4),
	SP5K_AUD_STATUS_REC_START  = (1 << 5),
	SP5K_AUD_STATUS_REC_PAUSE  = (1 << 6),
} sp5kAudDevStatus_t;

typedef enum {
	SP5K_AUD_REC_LEVEL_WIN_SIZE_CFG,
	SP5K_AUD_REC_LEVEL_DETECT_EN,
	SP5K_AUD_REC_LEVEL_VALUE_GET,
} sp5kAudRecLevel_t;

/* argument devId in sp5kAudMixerPlayCtrlSet(). */
typedef enum _sp5kAudMixerPlayDevId_e {
	SP5K_AUD_MIXER_PLAY_DEV_0 = 0,
} sp5kAudMixerPlayDevId_e;

/* argument trkId in sp5kAudMixerPlayCtrlSet(). */
typedef enum _sp5kAudMixerPlayTrkId_e {
	SP5K_AUD_MIXER_PLAY_TRK_SOUND = 0,
	SP5K_AUD_MIXER_PLAY_TRK_MEDIA = 1,
} sp5kAudMixerPlayTrkId_e;

/* argument cfgId in sp5kAudMixerPlayCtrlSet(). */
typedef enum _sp5kAudMixerPlayCfgId_e {
	/*
	  trkId = not used, fill 0.
	  para1 = 0: direct output to device, 1: multi track mixing.
	*/
	SP5K_AUD_MIXER_PLAY_MIX_ENABLE,
	/*
	  para1 = volume, 0~256.
	*/
	SP5K_AUD_MIXER_PLAY_VOLUME,
} sp5kAudMixerPlayCfgId_e;

/**************************************************************************
 *                          D A T A    T Y P E S
 **************************************************************************/

/**
 Equalizer shelf filter parameter
 Direct map to audFilterEqShelf_t
 @sa audFilterEqShelf_t
*/
typedef audFilterEqShelf_t sp5kAudFilterEqShelf_t;

/**
 Equalizer peak filter parameter
 Direct map to audFilterEqPeak_t
 @sa audFilterEqPeak_t
*/
typedef audFilterEqPeak_t sp5kAudFilterEqPeak_t;

/**
 High-pass filter parameter
 Direct map to audFilterHp_t
 @sa audFilterHp_t
*/
typedef audFilterHp_t sp5kAudFilterHp_t;

/**
 Notch filter parameter
 Direct map to audFilterNotch_t
 @sa audFilterNotch_t
*/
typedef audFilterNotch_t sp5kAudFilterNotch_t;

/**
 Argument pcfg in sp5kAudFilterEqCfg()
 Direct map to audFilterEqCfg_t
 @sa audFilterEqCfg_t
*/
typedef audFilterEqCfg_t sp5kAudFilterEqCfg_t;

/**
 Argument pcfg in sp5kAudFilterHpCfg()
 Direct map to audFilterHpCfg_t
 @sa audFilterHpCfg_t
*/
typedef audFilterHpCfg_t sp5kAudFilterHpCfg_t;

/**
 Argument pcfg in sp5kAudFilterNotchCfg()
 Direct map to audFilterNotchCfg_t
 @sa audFilterNotchCfg_t
*/
typedef audFilterNotchCfg_t sp5kAudFilterNotchCfg_t;

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S
 **************************************************************************/
UINT32 sp5kAudActDevSet(UINT32 selector, UINT32 devId);
UINT32 sp5kAudDevSrcSet(UINT32 selector, UINT32 ioSrcType);
UINT32 sp5kAudDevSrcGet(UINT32 selector, UINT32 *pioSrcType);
UINT32 sp5kAudDevVolumeSet(UINT32 selector, UINT32 vol);
UINT32 sp5kAudDevVolumeGet(UINT32 selector, UINT32 *pvol);
void   sp5kAudDevStatusGet(UINT32 *status);
void   sp5kAudDevVrefSet(UINT32 mask, UINT32 on);

UINT32 sp5kAudDevRecLevelOp(UINT32 selector, void *val);

UINT32 sp5kAudMixerPlayCtrlSet(UINT32 devId, UINT32 trkId, UINT32 cfgId, ...);

/* FIXME: for 5110 host compatibility */
UINT32 sp5kAudDevSpeakerCtrl(UINT32 enable);
UINT32 sp5kAudDevPinCfg(UINT32 funcPin, UINT32 grp, UINT32 pinId, UINT32 polarity);

/**
 Configure audio EQ filter (Equalization filter)
 @param[in] pcfg Pointer to sp5kAudFilterEqCfg_t
*/
UINT32 sp5kAudFilterEqCfg(sp5kAudFilterEqCfg_t *pcfg);
/**
 Configure audio HP filter (High Pass filter)
 @param[in] pcfg Pointer to sp5kAudFilterEqCfg_t
*/
UINT32 sp5kAudFilterHpCfg(sp5kAudFilterHpCfg_t *pcfg);
/**
 Configure audio Notch filter (Band-reject filter)
 @param[in] pcfg Pointer to sp5kAudFilterNotchCfg_t
*/
UINT32 sp5kAudFilterNotchCfg(sp5kAudFilterNotchCfg_t *pcfg);

#endif  /* _SP5K_AUD_API_H_ */

