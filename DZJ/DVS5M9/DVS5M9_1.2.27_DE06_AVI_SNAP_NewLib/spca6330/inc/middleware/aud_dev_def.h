/**************************************************************************
 *
 *       Copyright (c) 2007-2013 by iCatch Technology, Inc.
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
 *  Author:
 *
 **************************************************************************/
#ifndef _AUD_DEV_DEF_H_
#define _AUD_DEV_DEF_H_

/**************************************************************************
 *                         H E A D E R   F I L E S
 **************************************************************************/
#include "common.h"

/**************************************************************************
 *                           C O N S T A N T S
 **************************************************************************/
typedef enum _audCodecMode_e {
	AUD_CODEC_MODE_REC  = 0,
	AUD_CODEC_MODE_PLAY = 1,
	AUD_CODEC_MODE_TOT,
} audCodecMode_e;

typedef enum _audDevId_e {
	AUD_DEV_PRIMARY   = 0,
	AUD_DEV_SECONDARY = 1,
	AUD_DEV_TOT,
} audDevId_e;

typedef enum _auRecdChnl_e {
	AUD_REC_CH_LEFT  = 0x01,
	AUD_REC_CH_RIGHT = 0x02,
	AUD_REC_CH_ALL   = 0x03,
} audRecChnl_e;  /* direct map to halAudRecChnl_e. */

typedef enum _audPlayChnl_e {
	AUD_PLAY_CH_LEFT  = 0x01,
	AUD_PLAY_CH_RIGHT = 0x02,
	AUD_PLAY_CH_ALL   = 0x03,
} audPlayChnl_e;  /* direct map to halAudPlayChnl_e. */

typedef enum _audInputSel_e {
	AUD_IN_NONE = 0x00,
	AUD_IN_LINE = 0x10,
	AUD_IN_MIC  = 0x20,
} audInputSel_e;

typedef enum _audOutputSel_e {
	AUD_OUT_NONE      = 0,
	AUD_OUT_SPEAKER   = 1,
	AUD_OUT_HEADPHONE = 2,
	AUD_OUT_LINEOUT   = 3,
} audOutputSel_e;

/**************************************************************************
 *                           D A T A   T Y P E S
 **************************************************************************/
/**
 Callback function list for audio codec interface
*/
typedef struct audCodecItf_s {
	void (*pf_switch[AUD_CODEC_MODE_TOT])(void); /**< Handle for switch on/off. 0 for REC; 1 for PLAY. */
	void (*pf_init[AUD_CODEC_MODE_TOT])(void); /**< Handle for codec init. 0 for REC; 1 for PLAY. */
	void (*pf_vol_set[AUD_CODEC_MODE_TOT])(UINT32, UINT32); /**< Handle for volume set. 0 for REC; 1 for PLAY. */
	void (*pf_vol_get[AUD_CODEC_MODE_TOT])(UINT32, UINT32 *); /**< Handle for volume get. 0 for REC; 1 for PLAY. */
	void (*pf_output_set)(UINT32); /**< Handle for output interface. Such as SPEAKER/HP/LINEOUT. */
	void (*pf_output_get)(UINT32 *);
	void (*pf_input_set)(UINT32); /**< Handle for input interface. Such as LINEIN/MICPHONE. */
	void (*pf_input_get)(UINT32 *);
	void (*pf_samplerate_set)(UINT32); /**< Handle for sample rate configuration. */
	void (*pf_samplerate_get)(UINT32 *);
} audCodecItf_t;

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S
 **************************************************************************/

#endif  /* _AUD_DEV_DEF_H_ */

