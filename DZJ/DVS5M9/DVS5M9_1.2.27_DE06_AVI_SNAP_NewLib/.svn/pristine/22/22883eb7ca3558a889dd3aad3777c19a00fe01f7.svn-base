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

/**************************************************************************
 *                            H E A D E R   F I L E S
 **************************************************************************/
#include "app_com_def.h"
#include "customization/app_init.h"
#include "app_ts_cfg.h"
#include "app_ts_init.h"
#include "app_ts_evt.h"
#include "app_ts_gui.h"
#include "app_ts_cal.h"

#include "sp5k_ts_api.h"

#if FUNC_OPT_TP
/**************************************************************************
 *                              C O N S T A N T S
 **************************************************************************/

/**************************************************************************
 *                                  M A C R O S
 **************************************************************************/
#define LCD_WITH     320
#define LCD_HEIGHT   240
/**************************************************************************
 *                              D A T A   T Y P E S
 **************************************************************************/

/**************************************************************************
 *                            G L O B A L   D A T A
 **************************************************************************/
static UINT16 sgTsPosX, sgTsPosY;

/**************************************************************************
 *                      E X T E R N A L   R E F E R E N C E
 **************************************************************************/
extern void ros_thread_sleep(UINT32 ms);
/**************************************************************************
 *                  F U N C T I O N   D E C L A R A T I O N S
 **************************************************************************/
static UINT32 appTsPinCfgInit(void);
static void appTsQuadrantCfg(UINT16 dispWidth, UINT16 dispHeight, TouchScreenCalibration_t *tsCali, TouchScreenThreshold_t tsThr, appTouchQuadrant_t qOrigin);

/**
 * @fn        UINT32 appTouchScreenInit(void)
 * @brief     appTouchScreenInit
 * @param     NONE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-8-24
 * @todo      N/A
 * @bug       N/A
*/
UINT32
appTouchScreenInit(
	void
)
{
	UINT32 ret = FAIL;
	
	/* Touch screen pin configuration */
	ret = appTsPinCfgInit();
	if (ret) {
		printf("TS pin cfg fail\n");
		return ret;
	}

	/* Initial touch screen */
	ret == sp5kTsInit();
	if (ret) {
		printf("sp5kTsInit fail\n");
		return ret;
	}

	/* Config sampling interval */
	sp5kTsSampleIntervalSet(TP_SAMPLE_INTVAL);

	/* Touch screen calibration data load */
	ret = appTsCalCfgInit(TS_CAL_CFG_RES, TS_PANEL_SIZE_H, TS_PANEL_SIZE_V);

	appTsPixFilterInit();

	if (ret == SUCCESS) {
		/* Enable touch screen */
		ret = sp5kTsEnable(1);
	}

	return ret;
}


/**
 * @fn        UINT32 appTsPinCfgInit(void)
 * @brief     appTsPinCfgInit
 * @param     NONE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-8-24
 * @todo      N/A
 * @bug       N/A
*/
static UINT32
appTsPinCfgInit(
	void
)
{
	TouchScreenPinCfg_t tsCfg = {0};
	
	tsCfg.pinXplus = PIN_TP_XPLUS;
	tsCfg.pinXpLvl = PIN_TP_XPLUS_LVL;
	tsCfg.pinXminus = PIN_TP_XMINUS;
	tsCfg.pinXmLvl = PIN_TP_XMINUS_LVL;
	tsCfg.pinYplus = PIN_TP_YPLUS;
	tsCfg.pinYpLvl = PIN_TP_YPLUS_LVL;
	tsCfg.pinYminus = PIN_TP_YMINUS;
	tsCfg.pinYmLvl = PIN_TP_YMINUS_LVL;
	tsCfg.adcDownPin = PIN_TP_DOWN_DETECT;
	tsCfg.adcPosX = ADC_TP_READ_X;
	tsCfg.adcPosY = ADC_TP_READ_Y;
	
	/* config ts pins and adc */
	return sp5kTsPinCfg(&tsCfg);
}

/**
 * @fn        UINT32 appTsCalCfgInit(tsCalCfgInit_t initMode, UINT16 dispWidth, UINT16 dispHeight)
 * @brief     appTsCalCfgInit
 * @param     [in] initMode
 * @param     [in] dispWidth
 * @param     [in] dispHeight
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-8-24
 * @todo      N/A
 * @bug       N/A
*/
UINT32
appTsCalCfgInit(
	tsCalCfgInit_t initMode,
	UINT16 dispWidth,
	UINT16 dispHeight
)
{
	UINT32 ret = FAIL;
	TouchScreenThreshold_t tsThr = {0,0,0,0};
	TouchScreenCalibration_t tsCali = {0,0,0,0,0,0};

	/* Load touch screen calibration value from resource file */
 	if (initMode != TS_CAL_CFG_DEF) {

		/* TODO */
		
		/* ret = sp5kResourceFileLoad(SP5K_RES_TS_CAL, TS_CAL_CFG_FILE); */
		ret = appTsCalParamLoad((UINT8*)(&tsCali));
	
		if (ret == SUCCESS) {
			
			/* data valification */
			ret = sp5kTsCalibrationSet(&tsCali);/**/
			if (ret == SUCCESS) {
				printf("TS CAL set success\n");
				ret = tsCali.xscale + tsCali.xtrans + tsCali.yscale + tsCali.ytrans + tsCali.shift + tsCali.xyswap;
				if(ret)
				{
					printf("TS CAL chk sum success\n");
					return SUCCESS;
				}
				else
				{
					printf("TS CAL chk sum fail\n");
				}
			} else {
				printf("TS CAL set fail,use default param\n");
			}
		} else {
			printf("TS CAL load fail,use default param\n");
		}
	}

	/* Load default touch screen calibration value */
	printf("TS CAL load default val\n");

	/* Set threshold */
	tsThr.minThresholdX = APP_TP_X_MIN_THRD;
	tsThr.minThresholdY = APP_TP_Y_MIN_THRD;
	tsThr.maxThresholdX = APP_TP_X_MAX_THRD;
	tsThr.maxThresholdY = APP_TP_Y_MAX_THRD;
	sp5kTsThresholdSet(&tsThr);

	/* Set default calibration value */
	appTsQuadrantCfg(dispWidth, dispHeight, &tsCali, tsThr, APP_TP_ORIGIN_RIGHT_BOTTOM);

	/* Calculate calibration */
	tsCali.shift = TP_CALIBRATION_SCALE_SHIFT;
	tsCali.xyswap = 0;
	printf("TP xscale=%d, xtrans=%d, yscale=%d, ytrans=%d\n",
		tsCali.xscale, tsCali.xtrans, tsCali.yscale, tsCali.ytrans);

	/* Use calibrated x, y position */
	ret = sp5kTsCalibrationSet(&tsCali);/**/
	sp5kTsSysCfgSet(SP5K_TS_SYS_CFG_COORD_DUMP, 1);
	ret = SUCCESS;/**/

	return ret;
}


/**
 * @fn        void appTsQuadrantCfg(UINT16 dispWidth, UINT16 dispHeight, TouchScreenCalibration_t *tsCali, TouchScreenThreshold_t tsThr, appTouchQuadrant_t qOrigin)
 * @brief     appTsQuadrantCfg
 * @param     [in] dispWidth
 * @param     [in] dispHeight
 * @param     [in] tsCali
 * @param     [in] tsThr
 * @param     [in] qOrigin
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-8-24
 * @todo      N/A
 * @bug       N/A
*/
static void
appTsQuadrantCfg(
	UINT16 dispWidth,
	UINT16 dispHeight,
	TouchScreenCalibration_t *tsCali,
	TouchScreenThreshold_t tsThr, 
	appTouchQuadrant_t qOrigin
)
{
	switch(qOrigin)
	{
		case APP_TP_ORIGIN_LEFT_TOP:
			tsCali->xscale = (SINT32)dispWidth * (1 << APP_TP_CALI_SCALE_SHIFT) / 
							(tsThr.maxThresholdX - tsThr.minThresholdX);
			tsCali->xtrans = ((SINT32)dispWidth << APP_TP_CALI_SCALE_SHIFT) - 
							(tsCali->xscale * tsThr.maxThresholdX);
			tsCali->yscale = (SINT32)dispHeight * (1 << APP_TP_CALI_SCALE_SHIFT) / 
							(tsThr.maxThresholdY - tsThr.minThresholdY);
			tsCali->ytrans = ((SINT32)dispHeight << APP_TP_CALI_SCALE_SHIFT) - 
							(tsCali->yscale * tsThr.maxThresholdY);
			break;
			
		case APP_TP_ORIGIN_RIGHT_TOP:
			tsCali->xscale = (SINT32)dispWidth * (1 << APP_TP_CALI_SCALE_SHIFT) / 
							(tsThr.minThresholdX - tsThr.maxThresholdX);
			tsCali->xtrans = ((SINT32)dispWidth << APP_TP_CALI_SCALE_SHIFT) - 
							(tsCali->xscale * tsThr.minThresholdX);
			tsCali->yscale = (SINT32)dispHeight * (1 << APP_TP_CALI_SCALE_SHIFT) / 
							(tsThr.maxThresholdY - tsThr.minThresholdY);
			tsCali->ytrans = ((SINT32)dispHeight << APP_TP_CALI_SCALE_SHIFT) - 
							(tsCali->yscale * tsThr.maxThresholdY);
			break;
			
		case APP_TP_ORIGIN_LEFT_BOTTOM:
			tsCali->xscale = (SINT32)dispWidth * (1 << APP_TP_CALI_SCALE_SHIFT) / 
							(tsThr.maxThresholdX - tsThr.minThresholdX);
			tsCali->xtrans = ((SINT32)dispWidth << APP_TP_CALI_SCALE_SHIFT) - 
							(tsCali->xscale * tsThr.maxThresholdX);
			tsCali->yscale = (SINT32)dispHeight * (1 << APP_TP_CALI_SCALE_SHIFT) / 
							(tsThr.minThresholdY - tsThr.maxThresholdY);
			tsCali->ytrans = ((SINT32)dispHeight << APP_TP_CALI_SCALE_SHIFT) - 
							(tsCali->yscale * tsThr.minThresholdY);
			break;
			
		case APP_TP_ORIGIN_RIGHT_BOTTOM:
			tsCali->xscale = (SINT32)dispWidth * (1 << APP_TP_CALI_SCALE_SHIFT) / 
							(tsThr.minThresholdX - tsThr.maxThresholdX);
			tsCali->xtrans = ((SINT32)dispWidth << APP_TP_CALI_SCALE_SHIFT) - 
							(tsCali->xscale * tsThr.minThresholdX);
			tsCali->yscale = (SINT32)dispHeight * (1 << APP_TP_CALI_SCALE_SHIFT) / 
							(tsThr.minThresholdY - tsThr.maxThresholdY);
			tsCali->ytrans = ((SINT32)dispHeight << APP_TP_CALI_SCALE_SHIFT) - 
							(tsCali->yscale * tsThr.minThresholdY);
			break;

		default:
			break;
		
	}
}

/**
 * @fn        UINT32 appTsEvtGet(t_t *tsEvt)
 * @brief     appTsEvtGet
 * @param     [in] tsEvt
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-9-19
 * @todo      N/A
 * @bug       N/A
*/
UINT32 appTsEvtGet(
	TouchScreenEvent_t *tsEvt
)
{
	UINT32 ret;

	ret = sp5kTsEventGet(tsEvt);
	if (ret != SUCCESS) {
		return FAIL;
	}
	return appTsPixFilterProc(tsEvt);
}

/**
 * @fn        void appTsTask(UINT32 para)
 * @brief     appTsTask
 * @param     [in] para
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-11-26
 * @todo      N/A
 * @bug       N/A
*/
void
appTsTask(
	UINT32 para
)
{
	UINT32 num, i;
	TouchScreenEvent_t tsEvt;
	void *pPix = &tsEvt;

	para = para;
	while(1) {
		ros_thread_sleep(TP_SAMPLE_INTVAL*1);

		num = sp5kTsEventNumGet();
		for (i = 0; i < num; i++) {
			if (appTsEvtGet(&tsEvt) == SUCCESS) {
				sgTsPosX = tsEvt.x;
				sgTsPosY = tsEvt.y;
				/*printf("%03d,%03d,%01d\n", tsEvt.x, tsEvt.y, tsEvt.pressure);*/
				guiTpEvtHdl((guiPix_t *)pPix);
			} else {
				break;
			}
		}
	}
}
/**
 * @fn        void appTsCoordGet(UINT16 *pPosX, UINT16 *pPosY)
 * @brief     appTsCoordGet
 * @param     [in] pPosX
 * @param     [in] pPosY
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2013-1-23
 * @todo      N/A
 * @bug       N/A
*/
void
appTsCoordGet(
	UINT16 *pPosX,
	UINT16 *pPosY
)
{
	if (pPosX) *pPosX = sgTsPosX;
	if (pPosY) *pPosY = sgTsPosY;
}

#endif /* FUNC_OPT_TP */

