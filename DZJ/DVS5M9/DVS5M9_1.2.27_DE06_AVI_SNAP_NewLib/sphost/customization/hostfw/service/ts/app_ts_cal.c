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
#include "app_ts_cal.h"
#include "app_ts_cfg.h"
#include "app_ts_gui.h"
#include "app_ts_init.h"
#include "app_ts_api.h"
#include "app_com_api.h"
#include "app_osd_api.h"

/**************************************************************************
 *                              C O N S T A N T S
 **************************************************************************/
#define TS_CAL_PNL_SIZE_X	(320)
#define TS_CAL_PNL_SIZE_Y	(240)
 
#define TS_CAL_POS_TOTAL	(4)

#define TP_CAL_PROC_3_POINTS  0 /* For touch panel calibration value calculation */

/**************************************************************************
 *                                  M A C R O S
 **************************************************************************/
#if 0
#define APP_TS_DEC_CLR() memset(&appTsDec, 0, sizeof(appTsDec_t))
#endif
#define APP_TS_PEN_EVT_BUFF_CLR() memset(&appTsPenEvtBuff[0], -1, sizeof(appTsPenEvtBuff))
#define PIX_X_JUMP_LEN_MAX 5
#define PIX_Y_JUMP_LEN_MAX 5
#define PIX_AXIS_FIX_CNT 2

#if 0
#define appTsPixFilterRst appTsPixFilterInit
#endif
#define APP_TS_CAL_PROC_RST() do {sgTsCalProc = 0;} while(0)
#define APP_TS_CAL_PROC_ADD(AdcX, AdcY) \
do {\
		sgTsCalAdcVal[sgTsCalProc].x = AdcX;\
		sgTsCalAdcVal[sgTsCalProc].y = AdcY;\
		sgTsCalProc++;\
} while (0)


/**************************************************************************
 *                              D A T A   T Y P E S
 **************************************************************************/
typedef struct appTsDec_s {
	UINT16 penEvtLat; /* Host pen event latch point from event report from kernel */
	UINT16 penEvtDly; /* pen up to next pen down delay count */

	UINT8 penEvtDetect;
	UINT8 bEvtBufFull;
} appTsDec_t;
typedef struct appTsCalCoord_s {
	SINT32 x;
	SINT32 y;
} appTsCalCoord_t;


/**************************************************************************
 *                            G L O B A L   D A T A
 **************************************************************************/
static appTsDec_t appTsDec;
static TouchScreenEvent_t appTsPenEvtBuff[APP_TP_PEN_EVT_LAT];
static UINT32 cntMax = 0;
static const appTsCalCoord_t sgTsCalOsdCoord[TS_CAL_POS_TOTAL] = {
	/* {x, y} */
	{TS_CAL_PNL_SIZE_X*1/5, TS_CAL_PNL_SIZE_Y*1/5},
	{TS_CAL_PNL_SIZE_X*4/5, TS_CAL_PNL_SIZE_Y*1/5},
	{TS_CAL_PNL_SIZE_X*4/5, TS_CAL_PNL_SIZE_Y*4/5},
	{TS_CAL_PNL_SIZE_X*1/5, TS_CAL_PNL_SIZE_Y*4/5},
};
static UINT8 sgTsCalProc;
static appTsCalCoord_t sgTsCalAdcVal[TS_CAL_POS_TOTAL];


/**************************************************************************
 *                      E X T E R N A L   R E F E R E N C E
 **************************************************************************/
extern void ros_thread_sleep(UINT32 ms);
/**************************************************************************
 *                  F U N C T I O N   D E C L A R A T I O N S
 **************************************************************************/
 /**
 * @fn      UINT32 appTsCalParamSave(UINT8 *pbuf)
 * @brief     appTsCalParamSave
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    jj.yuan
 * @since     2013-3-7
 * @todo      N/A
 * @bug       N/A
*/
UINT32
appTsCalParamSave(
	UINT8 *pbuf
)
{
	if(pbuf == NULL)
	{
		printf("[ERR] save touch panel cal param file error!!! : %s(%d)\n",__FUNCTION__,__LINE__);
		return FAIL;
	}
	if(WriteFile(TS_CAL_CFG_FILE, (UINT8*)(pbuf), sizeof(TouchScreenCalibration_t))!=SUCCESS) 
	{		
		printf("[ERR] write file[%s] fail\n", TS_CAL_CFG_FILE);
		return FAIL;
	}
	
	return SUCCESS;

}
 /**
 * @fn	UINT32 appTsCalParamLoad(UINT8 *pbuf)
 * @brief	  appTsCalParamLoad
 * @param	  NONE
 * @retval	  NONE
 * @see 	  NULL
 * @author	  jj.yuan
 * @since	  2013-3-7
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT32
appTsCalParamLoad(
	UINT8 *pbuf
)
{
	 if(pbuf == NULL) 
	 {
		 printf("[ERR] load touch panel cal param file error!!! : %s(%d)\n",__FUNCTION__,__LINE__);
		 return FAIL;
	 }
	 if(ReadFile(TS_CAL_CFG_FILE, (UINT8*)(pbuf), sizeof(TouchScreenCalibration_t))!= SUCCESS)
	 {
		 printf("[ERR] read file[%s] fail\n", TS_CAL_CFG_FILE);
		 /* set default */		 
		 return FAIL;
	 }	 
	 return SUCCESS;

}
 /*jj add for new  filter process for new base FW*/
/**
 * @fn		  void appTsPixFilterInit(void)
 * @brief	  appTsPixFilterInit
 * @param	  NONE
 * @retval	  NONE
 * @see 	  NULL
 * @author	  juanjuan.yuan
 * @since	  2013-4-24
 * @todo	  N/A
 * @bug 	  N/A
*/
void
appTsPixFilterInit(
	void
)
{
	APP_TS_PEN_EVT_BUFF_CLR();
}

/**
 * @fn		  void appTsPixFilterRst(void)
 * @brief	  appTsPixFilterRst
 * @param	  NONE
 * @retval	  NONE
 * @see 	  NULL
 * @author	  juanjuan.yuan
 * @since	  2013-4-24
 * @todo	  N/A
 * @bug 	  N/A
*/

void
appTsPixFilterRst(
	void
)
{
	appTsPixFilterInit();
}

/**
 * @fn	UINT32 appTsPixFilterProc(TouchScreenEvent_t *pTsEvt)
 * @brief	  appTsPixFilterProc
 * @param	  NONE
 * @retval	  return = SUCCESS / FAIL
 * @see 	  NULL
 * @author	  juanjuan.yuan
 * @since	  2013-4-24
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT32
appTsPixFilterProc(
	TouchScreenEvent_t *pTsEvt
)
{
	UINT8 en = 1;

	if (!en) return SUCCESS;

	if (pTsEvt->x == 0xFFFF || pTsEvt->y == 0xFFFF) {
		appTsPixFilterRst();
		return SUCCESS;
	}
	#if 0
	if (pTsEvt->x < 0 || pTsEvt->y < 0) {
		return FAIL;
	}
	#endif

	if (appTsPenEvtBuff->x != 0xFFFF && \
		(pTsEvt->x > appTsPenEvtBuff->x + PIX_X_JUMP_LEN_MAX || pTsEvt->y > appTsPenEvtBuff->y + PIX_Y_JUMP_LEN_MAX) && \
		++cntMax < PIX_AXIS_FIX_CNT
	) 
	{
		printf("x:%i,y:%i -> x:%i,y:%i\n", pTsEvt->x, pTsEvt->y, appTsPenEvtBuff->x, appTsPenEvtBuff->y);/**/
		pTsEvt->x = appTsPenEvtBuff->x;
		pTsEvt->y = appTsPenEvtBuff->y;
		return FAIL;
	} else {
		cntMax = 0;
		appTsPenEvtBuff->x = pTsEvt->x;
		appTsPenEvtBuff->y = pTsEvt->y;
	}
	
	return SUCCESS;
	
}
#if 0 /*jj mark for new base FW is not need this filter*/
/**
 * @fn        void appTsPixFilterInit(void)
 * @brief     appTsPixFilterInit
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-9-12
 * @todo      N/A
 * @bug       N/A
*/
void
appTsPixFilterInit(
	void
)
{
	APP_TS_DEC_CLR();
	APP_TS_PEN_EVT_BUFF_CLR();
}

/**
 * @fn        UINT32 appTsPixFilterProc(TouchScreenEvent_t *pTsEvt)
 * @brief     appTsPixFilterProc
 * @param     [in] pTsEvt
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-9-12
 * @todo      N/A
 * @bug       N/A
*/
UINT32
appTsPixFilterProc(
	TouchScreenEvent_t *pTsEvt
)
{
	appTsDec_t *pDec = &appTsDec;
	TouchScreenEvent_t *pBuf = &appTsPenEvtBuff[0];
	UINT32 cnt = 0;

	/* For pen up debounce */
	if (!pTsEvt->pressure) {
		while (cnt++ < APP_TP_PEN_UP_DEC) {
			ros_thread_sleep(TP_SAMPLE_INTVAL);
			if (sp5kTsEventNumGet()) {
				printf("%03d,%03d,%01d -> skip(%d)\n", pTsEvt->x, pTsEvt->y, pTsEvt->pressure, (cnt+1)*10);
				return FAIL;
			}
		}
	}

	if (!pDec->penEvtDetect) {
		if (pTsEvt->pressure) {
			if (pDec->penEvtLat < APP_TP_PEN_EVT_LAT) {
				pBuf[pDec->penEvtLat] = *pTsEvt;
				++pDec->penEvtLat;
				pDec->penEvtDetect = 1;

				return SUCCESS;
			} else {
				/* latch phase error */
				appTsPixFilterRst();
			}
		} else {
			/* the press time is too short */
			appTsPixFilterRst();
		}

		return FAIL;
	}

	if (pDec->penEvtLat < APP_TP_PEN_EVT_LAT) {
		pBuf[pDec->penEvtLat] = *pTsEvt;

		if (!pDec->bEvtBufFull) {
			/* we use the first record if the buffer's NOT full */
			*pTsEvt = pBuf[0]; 
		} else {
			/* we use the last old record if the buffer's full */
			*pTsEvt = pBuf[pDec->penEvtLat+1 >= APP_TP_PEN_EVT_LAT ? 0 : pDec->penEvtLat+1];
		}
		/* recover the pressure flag */
		pTsEvt->pressure = pBuf[pDec->penEvtLat].pressure;
		
		if (pTsEvt->pressure) {
			if (++pDec->penEvtLat >= APP_TP_PEN_EVT_LAT) {
				pDec->penEvtLat = 0;
				/*
				if (!pDec->bEvtBufFull) {
					printf(">rdy\n");
				}
				*/
				pDec->bEvtBufFull = 1;
			}
			
			if (!pDec->bEvtBufFull) {
				/*printf(">");*/
				return FAIL;
			} else {
				return SUCCESS;
			}
		} else {
#if 0 /* for debug */
			UINT32 i;
			for (i = 0; i < APP_TP_PEN_EVT_LAT; i++) {
				printf("(%03d,%03d,%01d)", pBuf[i].x, pBuf[i].y, pBuf[i].pressure);
			}
			printf("\n");
			
#endif			
			appTsPixFilterRst();
			return SUCCESS;
		}
	
	} else {
		/* latch phase error */
		appTsPixFilterRst();
		return FAIL;
	}

	return SUCCESS;
}
#endif

UINT32
appTsCalParaParsing(
	void
)
{
	/*
		[Layout]

		OSD0, OSD1
		
		OSD3, OSD2

		[Formula]
		new_x = (xscale * x + xtrans) >> shift
		new_y = (yscale * y + ytrans) >> shift

		[Equations]
		
		 (ADCx0 * tsCal0.xscale + tsCal0.xtrans) >> tsCal0.shift = OSDx0
		{
		 (ADCx1 * tsCal0.xscale + tsCal0.xtrans) >> tsCal0.shift = OSDx1

		 (ADCy0 * tsCal0.yscale + tsCal0.ytrans) >> tsCal0.shift = OSDy0
		{
		 (ADCy3 * tsCal0.yscale + tsCal0.ytrans) >> tsCal0.shift = OSDy3

		 (ADCx3 * tsCal1.xscale + tsCal1.xtrans) >> tsCal1.shift = OSDx3
		{
		 (ADCx2 * tsCal1.xscale + tsCal1.xtrans) >> tsCal1.shift = OSDx2

		 (ADCy1 * tsCal1.yscale + tsCal1.ytrans) >> tsCal1.shift = OSDy1
		{
		 (ADCy2 * tsCal1.yscale + tsCal1.ytrans) >> tsCal1.shift = OSDy2

		 tsCal.xscale = (tsCal0.xscale + tsCal1.xscale) / 2
		 tsCal.yscale = (tsCal0.yscale + tsCal1.yscale) / 2
	*/
	
	TouchScreenCalibration_t tsCal = {0};
	TouchScreenCalibration_t tsCal0 = {0};
	TouchScreenCalibration_t tsCal1 = {0};
	const appTsCalCoord_t *pCoord, *pAdc;
	UINT32 shift = APP_TP_CALI_SCALE_SHIFT;
	
	pCoord = &sgTsCalOsdCoord[0];
	pAdc   = &sgTsCalAdcVal[0];
	
	tsCal0.xscale = ((pCoord[1].x - pCoord[0].x) << shift) / (pAdc[1].x - pAdc[0].x);
	tsCal0.xtrans = (pCoord[0].x << shift) - pAdc[0].x * tsCal0.xscale;

	tsCal0.yscale = ((pCoord[3].y - pCoord[0].y) << shift) / (pAdc[3].y - pAdc[0].y);
	tsCal0.ytrans = (pCoord[0].y << shift) - pAdc[0].y * tsCal0.yscale;

	tsCal1.xscale = ((pCoord[2].x - pCoord[3].x) << shift) / (pAdc[2].x - pAdc[3].x);
	tsCal1.xtrans = (pCoord[2].x << shift) - pAdc[2].x * tsCal0.xscale;

	tsCal1.yscale = ((pCoord[2].y - pCoord[1].y) << shift) / (pAdc[2].y - pAdc[1].y);
	tsCal1.ytrans = (pCoord[2].y << shift) - pAdc[2].y * tsCal0.yscale;

#if TP_CAL_PROC_3_POINTS
	tsCal.xscale = tsCal0.xscale;
	tsCal.yscale = tsCal0.yscale;
	tsCal.xtrans = tsCal0.xtrans;
	tsCal.ytrans = tsCal0.ytrans;
#else /*TP_CAL_PROC_4_POINTS*/
	tsCal.xscale = (tsCal0.xscale + tsCal1.xscale) >> 1;
	tsCal.yscale = (tsCal0.yscale + tsCal1.yscale) >> 1;
	tsCal.xtrans = (tsCal0.xtrans + tsCal1.xtrans) >> 1;
	tsCal.ytrans = (tsCal0.ytrans + tsCal1.ytrans) >> 1;

	printf(" xscale0:%d, xscale1:%d,\n xtrans0:%d, xtrans1:%d,\n yscale0:%d, yscale1:%d,\n ytrans0:%d, ytrans1:%d\n", \
		tsCal0.xscale, tsCal1.xscale,
		tsCal0.xtrans, tsCal1.xtrans,
		tsCal0.yscale, tsCal1.yscale,
		tsCal0.ytrans, tsCal1.ytrans
	);
#endif
	printf(" xscale:%d,\n xtrans:%d,\n yscale:%d,\n ytrans:%d\n", \
		tsCal.xscale, 
		tsCal.xtrans, 
		tsCal.yscale, 
		tsCal.ytrans 
	);

	tsCal.shift = APP_TP_CALI_SCALE_SHIFT;
	tsCal.xyswap = APP_TP_CALI_XY_SWAP;
	if(SUCCESS==appTsCalParamSave((UINT8*)(&tsCal)))
	{
		RPrintf("save touch panel param success!\n");		
		appStateChange(APP_STATE_CALIBRATION,STATE_PARAM_POWER_ON);
	}
	else
	{
		appStateChange(APP_STATE_CALIB_TS, STATE_PARAM_NORMAL_INIT);
	}
	return sp5kTsCalibrationSet(&tsCal);
}	

UINT32
appTsCalHdl(
	void *ptr
)
{
	UINT16 adcValX, adcValY;
	
	ptr = ptr;
	
	appTsCoordGet(&adcValX, &adcValY);

	printf("\n(%d, %d)\n\n", adcValX, adcValY);
	if (sgTsCalProc < 4) {
		appOsd_ColorDrawShapeColorSet(PAL_BLUE, PAL_BLEND_100, PAL_BLUE, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(\
			APP_OSD_SHAPE_RECT, \
			sgTsCalOsdCoord[sgTsCalProc].x-2,\
			sgTsCalOsdCoord[sgTsCalProc].y+2, \
			8, 1, 0, 0 \
		);
		appOsdLib_OsdLayerShapeDraw(\
			APP_OSD_SHAPE_RECT, \
			sgTsCalOsdCoord[sgTsCalProc].x+2,\
			sgTsCalOsdCoord[sgTsCalProc].y-2, \
			1, 8, 0, 0 \
		);	
		APP_TS_CAL_PROC_ADD(adcValX, adcValY);		

	} 

	if (sgTsCalProc >= 4) {
		appTsCalParaParsing();
		APP_TS_CAL_PROC_RST();
	}
	
	return SUCCESS;
}

void
appTsCalInit(
	void
)
{
	UINT32 i;
	guiObjCusCtrl_t obj = {
		.x0 = APP_TP_X_MIN_THRD,
		.y0 = APP_TP_Y_MIN_THRD,
		.x1 = APP_TP_X_MAX_THRD,
		.y1 = APP_TP_Y_MAX_THRD,
		.penUp.pf = appTsCalHdl
	};

	printf("cal init\n");
	sp5kTsCalibrationSet(0);

	for (i = 0; i < 4; i++) /**/
	{
		char str[2];
		printf("(%03d, %03d)\n", sgTsCalOsdCoord[i].x, sgTsCalOsdCoord[i].y);		
		appOsd_ColorDrawShapeColorSet(PAL_RED_1, PAL_BLEND_100, PAL_RED_1, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(\
			APP_OSD_SHAPE_RECT, \
			sgTsCalOsdCoord[i].x,\
			sgTsCalOsdCoord[i].y, \
			5, 5, 0, 0 \
		);
		#if 1
		sprintf(str,"%d",i+1);
		appOsd_ColorTextColorSet(PAL_YELLOW,PAL_NOCOLOR,PAL_NOCOLOR);
		appOsdLib_TextDraw(sgTsCalOsdCoord[i].x,sgTsCalOsdCoord[i].y,SP5K_GFX_ALIGN_TOP_LEFT,str);
		#endif
	}

	appGuiObjDraw(&obj, 0);

	APP_OSD_REFRESH_ON;
}

/**
 * @fn        void appTsCalState(UINT32 msg, UINT32 param)
 * @brief     appTsCalState
 * @param     [in] msg
 * @param     [in] param
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-12-31
 * @todo      N/A
 * @bug       N/A
*/
void
appTsCalState(
	UINT32 msg,
	UINT32 param
)
{
	switch(msg) {
		case APP_STATE_MSG_INIT:
		appTsCalInit();
		break;
		case APP_STATE_MSG_CLOSE:
		appGuiObjClr(-1);
		appStateCloseDone();
		default:
		break;
	}
}

