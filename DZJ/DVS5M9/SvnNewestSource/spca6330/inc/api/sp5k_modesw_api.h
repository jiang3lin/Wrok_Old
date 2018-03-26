/**************************************************************************
 *                                                                        *
 *         Copyright (c) 2007 by Sunplus mMobile Co., Ltd.                *
 *                                                                        *
 *  This software is copyrighted by and is the property of Sunplus        *
 *  mMobile Co., Ltd. All rights are reserved by Sunplus mMobile          *
 *  Co., Ltd. This software may only be used in accordance with the       *
 *  corresponding license agreement. Any unauthorized use, duplication,   *
 *  distribution, or disclosure of this software is expressly forbidden.  *
 *                                                                        *
 *  This Copyright notice MUST not be removed or modified without prior   *
 *  written consent of Sunplus mMobile Co., Ltd.                          *
 *                                                                        *
 *  Sunplus mMobile Co., Ltd. reserves the right to modify this           *
 *  software without notice.                                              *
 *                                                                        *
 *  Sunplus mMobile Co., Ltd.                                             *
 *  19-1, Innovation First Road, Science-Based Industrial Park,           *
 *  Hsin-Chu, Taiwan, R.O.C.                                              *
 *                                                                        *
 *  Author:                                                               *
 *                                                                        *
 **************************************************************************/
#ifndef _SP5K_MODESW_API_H_
#define _SP5K_MODESW_API_H_

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
typedef enum {
	SP5K_MODE_STILL_PREVIEW  = 0x11,
	SP5K_MODE_STILL_AF       = 0x12, /* obsolete */
	SP5K_MODE_STILL_SNAP     = 0x13,
	SP5K_MODE_VIDEO_PREVIEW  = 0x21,
	SP5K_MODE_VIDEO_AF       = 0x22, /* obsolete */
	SP5K_MODE_VIDEO_RECORD   = 0x23,
	SP5K_MODE_AUDIO_RECORD   = 0x33,
	SP5K_MODE_MULTI_PLAYBACK = 0x41,
	SP5K_MODE_STILL_PLAYBACK = 0x42,
	SP5K_MODE_VIDEO_PLAYBACK = 0x43,
	SP5K_MODE_AUDIO_PLAYBACK = 0x44,
	SP5K_MODE_USB            = 0x51,
	SP5K_MODE_STANDBY        = 0x88,
} sp5kMode_e;

typedef enum {
	SP5K_MODE_CFG_FILE_TYPE		= 0x01, /*param1: sp5kModeFileType_e*/
	SP5K_MODE_CFG_STANDBY_DUP	= 0x02, /*param1: 1= backup last frame, 0= black frame*/
	SP5K_MODE_CFG_STANDBY_FORCE = 0x03, /*param1: 1= enter standby even already in*/
	SP5K_MODE_CFG_PV_FREEZE		= 0x04, /*param1: 1=enable freeze, 0=disable freeze*/
	SP5K_MODE_CFG_PV_DISP_FIT	= 0x05, /*param1: 1=enable pv disp fit by ratio, 0=disable*/
	SP5K_MODE_CFG_STANDBY_FRAME_SIZE= 0x06, /*param1,param2: frame size*/
	SP5K_MODE_CFG_STILL_PV_SIZE	= 0x07, /*param1: width, param2: height, param3: num*/
	SP5K_MODE_CFG_STILL_PV_DELAY    = 8, /*param1: delay frame cnt*/
	SP5K_MODE_CFG_STILL_PV_MV		= 0x09,           /*param1: pv mv enable */
	SP5K_MODE_CFG_BAND_DETECT_ENABLE = 0x0A, /*param1: band detect enable */
 	SP5K_MODE_CFG_BAND_DETECT_PEAK  = 0x0B,  /*param1: 60HZ peak for ALG band detect */	
	SP5K_MODE_CFG_PV_SMOOTH_DZOOM = 0x0C,	 /*param1: pv smooth digital zoom enable (sensor VGA->XGA) */
	SP5K_MODE_CFG_PV_PAN_ENABLE = 0x0D, /*param1: pv pan center enable*/
    SP5K_MODE_CFG_CDSP_DZ_BYPASS = 0x0E,
    SP5K_MODE_CFG_DIST_OUTSIDE_ENABLE= 0x0F,/*param1: dist outside mode enable*/
} sp5kModeCfg_e;

typedef enum {
	SP5K_MODE_FILE_TYPE_JPG  = 0x01,
	SP5K_MODE_FILE_TYPE_AVI  = 0x02,
	SP5K_MODE_FILE_TYPE_WAV  = 0x03,
} sp5kModeFileType_e;

typedef enum {
	SP5K_MODE_STILL_PREVIEW_NORMAL  = 0x00,
	SP5K_MODE_STILL_PREVIEW_FAKE  = 0x01,
} sp5kModeEx_e;

/**************************************************************************
 *                           D A T A   T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/
UINT32 sp5kModeSet(UINT32 mode);
UINT32 sp5kModeGet(UINT32 *pmode);
void sp5kModeWait(UINT32 newmode);
UINT32 sp5kModeCfgSet(sp5kModeCfg_e cfg, ...);
UINT32 sp5kModeGetEx(UINT32 *pmode,	UINT32 *pEx);

void sp5kModeMultiPbInit(void);
void sp5kModeStillPbInit(void);
void sp5kModeAvPlayInit(void);
void sp5kModeStillPrevInit(void);
void sp5kModeStillSnapInit(void);
void sp5kModeVideoPrevInit(void);
void sp5kModeVideoRecInit(void);
void sp5kModeAudioRecInit(void);
void sp5kModeUSBInit(void);

#endif  /* _SP5K_MODESW_API_H_ */

