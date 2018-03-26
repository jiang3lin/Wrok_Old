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
#ifndef _AUD_FILTER_DEF_H_
#define _AUD_FILTER_DEF_H_

/**************************************************************************
 *                         H E A D E R   F I L E S
 **************************************************************************/
#include "common.h"

/**************************************************************************
 *                           C O N S T A N T S
 **************************************************************************/
typedef enum _audFilterChnl_e {
	AUD_FILTER_FIRST  = 0x01,
	AUD_FILTER_SECOND = 0x02,
	AUD_FILTER_ALL    = 0x03,
} audFilterChnl_e;  /* direct map to halAudFilterChnl_e. */

/**************************************************************************
 *                           D A T A   T Y P E S
 **************************************************************************/
typedef struct audFilterEqShelf_s {
	UINT32 Fcentre;  /**< Centre frequency in unit of Hz. */
	float  gain;     /**< Gain value in unit of dB. */
} audFilterEqShelf_t;

typedef struct audFilterEqPeak_s {
	UINT32 Fcentre;  /**< Centre frequency in unit of Hz. */
	float  gain;     /**< Gain value in unit of dB. */
	float  Qfactor;  /**< Quality factor. */
} audFilterEqPeak_t;

typedef struct audFilterHp_s {
	UINT32 en;       /**< 0: disable, 1: enable high-pass filter. */
	UINT32 Fcentre;  /**< Centre frequency in unit of Hz. */
	float  Dfactor;  /**< Damping factor. */
} audFilterHp_t;

typedef struct audFilterNotch_s {
	UINT32 en;       /**< 0: disable, 1: enable notch filter. */
	UINT32 Fcentre;  /**< Centre frequency in unit of Hz. */
	float  Qfactor;  /**< Quality factor. */
} audFilterNotch_t;

typedef struct audFilterEqCfg_s {
	UINT32             chnlId;      /**< Channel ID, one of audFilterChnl_e. */
	UINT32             en;          /**< 0: disable, 1: enable equalizer. */
	UINT32             Fsample;     /**< Sampling frequency in unit of Hz. */
	audFilterEqShelf_t lowShelf;    /**< Low shelf filter parameter. */
	audFilterEqPeak_t  firstPeak;   /**< First peaking filter parameter. */
	audFilterEqPeak_t  secondPeak;  /**< Second peaking filter parameter. */
	audFilterEqPeak_t  thirdPeak;   /**< Third peaking filter parameter. */
	audFilterEqShelf_t highShelf;   /**< High shelf filter parameter. */
} audFilterEqCfg_t;

typedef struct audFilterHpCfg_s {
	UINT32        chnlId;       /**< Channel ID, one of audFilterChnl_e. */
	UINT32        Fsample;      /**< Sampling frequency in unit of Hz. */
	audFilterHp_t secondOrder;  /**< Second order high-pass filter parameter. */
} audFilterHpCfg_t;

typedef struct audFilterNotchCfg_s {
	UINT32           chnlId;       /**< Channel ID, one of audFilterChnl_e. */
	UINT32           Fsample;      /**< Sampling frequency in unit of Hz. */
	audFilterNotch_t firstNotch;   /**< First notch filter parameter. */
	audFilterNotch_t secondNotch;  /**< Second notch filter parameter. */
	audFilterNotch_t thirdNotch;   /**< Third notch filter parameter. */
	audFilterNotch_t fourthNotch;  /**< Fourth notch filter parameter. */
	audFilterNotch_t fifthNotch;   /**< Fifth notch filter parameter. */
} audFilterNotchCfg_t;

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S
 **************************************************************************/

#endif  /* _AUD_FILTER_DEF_H_ */

