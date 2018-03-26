/**************************************************************************
 *
 *       Copyright (c) 2006-2012 by iCatch Technology, Inc.
 *
 *  This software is copyrighted by and is the property of iCatch Technology,
 *  Inc.. All rights are reserved by iCatch Technology, Inc..
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
 *  Author: Phil Lin
 *
 **************************************************************************/
/* DO NOT EDIT the next 2 lines for auto revision by subversion
$Rev$
$Date::                     $
 */

#ifndef _APP_TS_CFG_H_
#define _APP_TS_CFG_H_

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************
 *                            H E A D E R   F I L E S
 **************************************************************************/

/**************************************************************************
 *                              C O N S T A N T S
 **************************************************************************/
/**
 * \note
 * Resource file for touch panel calibration data
 */
#define TS_PANEL_SIZE_H 320
#define TS_PANEL_SIZE_V 240

/**
 * \note
 * Resource file for touch panel calibration data
 */
#define TS_CAL_CFG_FILE "A:\\RO_RES\\CALIB\\TS_CAL.BIN"

/**
 * \note
 * Touch panel ADC pin configuration
 */
#define PIN_TP_YPLUS					((GPIO_GRP_SAR << 8) | 7)
#define PIN_TP_YPLUS_LVL				1
#define PIN_TP_YMINUS					((GPIO_GRP_SAR << 8) | 6)
#define PIN_TP_YMINUS_LVL				0
#define PIN_TP_XPLUS					((GPIO_GRP_SAR << 8) | 4)
#define PIN_TP_XPLUS_LVL				1
#define PIN_TP_XMINUS					((GPIO_GRP_SAR << 8) | 5)
#define PIN_TP_XMINUS_LVL				0
#define PIN_TP_DOWN_DETECT				0	/*0: x plus, 1: y minus*/
#define ADC_TP_READ_X					4
#define ADC_TP_READ_Y					6
#define TP_SAMPLE_INTVAL				10	/* msec */
#define PNL_WIDTH						320
#define PNL_HEIGHT						240
#define TP_CALIBRATION_SCALE_SHIFT		12
#define TP_MAX_THRESHOLD				976
#define TP_MIN_THRESHOLD				48
#define APP_TP_CALI_SCALE_SHIFT			12 /*!> bits for left shift */
#define APP_TP_CALI_XY_SWAP             0

/**
 * \note
 * minimal and maximal value read from touch panel
 */
#define APP_TP_X_MIN_THRD			30
#define APP_TP_X_MAX_THRD			1040
#define APP_TP_Y_MIN_THRD			64
#define APP_TP_Y_MAX_THRD			1040


/* touch panel debounce definition */
#define APP_TP_PEN_EVT_LAT    5
#define APP_TP_PEN_UP_DEC    8  /* check cycle is (APP_TP_PEN_UP_DEC*TP_SAMPLE_INTVAL) */

/**************************************************************************
 *                                  M A C R O S
 **************************************************************************/

/**************************************************************************
 *                              D A T A   T Y P E S
 **************************************************************************/

/**************************************************************************
 *                      E X T E R N A L   R E F E R E N C E
 **************************************************************************/

/**************************************************************************
 *                  F U N C T I O N   D E C L A R A T I O N S
 **************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _APP_TS_CFG_H_ */


