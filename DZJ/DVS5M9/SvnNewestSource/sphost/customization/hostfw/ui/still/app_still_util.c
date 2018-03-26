/**************************************************************************
 *
 *        Copyright (c) 2006-2008 by Sunplus mMedia Inc., Ltd.
 *
 *  This software is copyrighted by and is the property of Sunplus
 *  mMedia Inc., Ltd. All rights are reserved by Sunplus mMedia
 *  Inc., Ltd. This software may only be used in accordance with the
 *  corresponding license agreement. Any unauthorized use, duplication,
 *  distribution, or disclosure of this software is expressly forbidden.
 *
 *  This Copyright notice MUST not be removed or modified without prior
 *  written consent of Sunplus mMedia Inc., Ltd.
 *
 *  Sunplus mMedia Inc., Ltd. reserves the right to modify this
 *  software without notice.
 *
 *  Sunplus mMedia Inc., Ltd.
 *  19-1, Innovation First Road, Science-Based Industrial Park,
 *  Hsin-Chu, Taiwan, R.O.C. 
 *
 **************************************************************************/
#define HOST_DBG 1
#include <math.h>
#include "app_com_def.h"
#include "app_com_api.h"
#include "app_sys_def.h"

#include "sp5k_cal_api.h"
#include "sp5k_ae_api.h"
#include "sp5k_sensor_api.h"

#include "app_aaa.h"
#include "app_still.h"
#include "app_calib_api.h"
#include "app_lens_api.h"
#include "app_view_param.h"
#include "app_ui_para.h" 

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
 /*s Modify by Aries 10/02/04*/

#if KIT_SHUT_CTRL_TIMER
UINT32 CapExpTime;
UINT32 CapShutColseTime;
#endif
/*s Add by Aries 09/11/05*/
UINT32 CapWithFaceCheck=0;
/*e Add by Aries 09/11/05*/




/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
void
_stillUtil_ShutterDelaySet(
	void
)
{


}



void 
appStillUtil_CaptureSet(
	void
)
{
	_stillUtil_ShutterDelaySet();
	sp5kModeSet(SP5K_MODE_STILL_SNAP);	
}

void 
appStillUtil_CalibCaptureSet(
	BOOL enable
)
{
	if (enable){
		_stillUtil_ShutterDelaySet();
		sp5kCalibrationCfgSet(SP5K_CALIBRATION_CALLBACK, SP5K_CALIB_CB_STILL_RAW);
		
	}		
	else {
		sp5kCalibrationCfgSet(SP5K_CALIBRATION_OFF, SP5K_CALIB_CB_STILL_RAW);
		
	}
}
/*s Add by Aries 09/11/05*/
/*-------------------------------------------------------------------------
 *  File Name : appStillUtil_CaptureWithFaceCheck
 *------------------------------------------------------------------------*/
void 
appStillUtil_CaptureWithFaceCheck(
	void
)
{
	uiPara_t* puiPara = appUiParaGet();

	 if ((IS_SCENE_MODE && IS_BLINK_DETECTION_MODE) || puiPara->FaceTracking == UI_FACETRACK_BLINKING_DETECT){
		if(appStill_GetMainFaceData(NULL,NULL,NULL,NULL) == SUCCESS){
			CapWithFaceCheck =TRUE;
		}
	 }
}
/*e Add by Aries 09/11/05*/



