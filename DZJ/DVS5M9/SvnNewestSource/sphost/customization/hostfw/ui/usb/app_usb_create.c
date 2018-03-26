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
#include "app_com_def.h"
#include "app_com_api.h"
#include "gpio_custom.h"
#include "app_dev_plug.h"
#include "sp5k_usb_api.h"
#include "sp5k_disk_api.h"
#include "app_osdfont.h"
#include "app_errorosd.h"
#include "app_osd_api.h"
#include "app_ui_para.h"
#include "app_jpg_draw.h"
#include "app_video.h"
#include "sp5k_capture_api.h"
#include "sp5k_media_api.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

#define JPG_RES_START_X  	0
#define JPG_RES_START_Y  	0
#define JPG_RES_START_W  	320
#define JPG_RES_START_H  	240


/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

 static UINT32 originalState = APP_STATE_NULL; //state before enter USB state

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/


/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/*-------------------------------------------------------------------------
 *  File Name : appUsbCreateStateInit
 *------------------------------------------------------------------------*/

void UsbStrSettingInit()
{
	#if 1
	sp5kUsbStrDesc_t strIdx;
		
	char pstring[20]={"DSC"};
	//printf("--usb msdc state init\n");
	strIdx=SP5K_USB_STR_INDEX_MANUFACTURER;	
	//strcpy(pstring,"DxgTech");
	sp5kUsbStrDescSet(strIdx,pstring,3);
	strIdx=SP5K_USB_STR_INDEX_PRODUCT;
	char pstring1[20]={"MD 87156"};	
	sp5kUsbStrDescSet(strIdx,pstring1,8);
	#endif	

}


void
appUsbCreateStateInit(
	void
)

{


	uiPara_t* puiPara = appUiParaGet();
	//printf("--puiPara->USBMode=%x\n",puiPara->USBMode);
	if((puiPara->USBMode != USB_UVCH264)&&(puiPara->USBMode != USB_UVCMJPG))
	{	
		sensorDevPowerCustomCb(SENSOR_DEV_POWER_ID_SYS_OFF);
		sp5kModeSet(SP5K_MODE_STANDBY);
		//sp5kModeWait(SP5K_MODE_STANDBY);
		//appRfCopyBufferCallbackSet();
	}
	CLEAR_OSD_SRC;
	appAutoPowerOffEnable(FALSE);
	appLcmPowerSaveEnable(FALSE);
	
	appJpgDraw_PipLayer(ID_JPG_RES_USB_MODE,JPG_RES_START_X,JPG_RES_START_Y,JPG_RES_START_W,JPG_RES_START_H);

	appOsdMessageBoxShow(TRUE,ID_STR_CONNECTING);
	sp5kGfxAttrSet(SP5K_GFX_REFRESH_ACTIVE, 1, 0, 1, 0);
	sp5kStillCapAdvCfgSet(SP5K_CAPTURE_CROP_SCALE_IN_JPEG ,0);
	if (IS_USB_IN) 
	{
		if(puiPara->USBMode == USB_MSDC)
		{
			appStateChange(APP_STATE_USB_MSDC,STATE_PARAM_NORMAL_INIT);
		}
		else if((puiPara->USBMode == USB_UVCH264)||(puiPara->USBMode == USB_UVCMJPG))
		{
			appStateChange(APP_STATE_USB_PCCAM,STATE_PARAM_NORMAL_INIT);
		}
		else 
		{
			appStateChange(APP_STATE_USB_PRNT,STATE_PARAM_NORMAL_INIT);
		}
	}
}


/*-------------------------------------------------------------------------
 *  File Name : appUsbBackwardStateGet
 *------------------------------------------------------------------------*/
UINT32 
appUsbBackwardStateGet(
	void
)
{
	return originalState;
}

/*-------------------------------------------------------------------------
 *  File Name : appUsbCreateState
 *------------------------------------------------------------------------*/
void 
appUsbCreateState(
	UINT32 msg,
	UINT32 param
)
{	
	DBG_PRINT("%s : [0x%x] [0x%x]\n",__FUNCTION__,msg,param);
	
	switch (msg) {
	case APP_STATE_MSG_INIT:	
		RPrintf("appUsbCreateState");
		originalState= appActiveStateGet();/*appPreviousStateGet();*//*willow modify for hang up if power on while SD insert and USB connect*/
		appUsbCreateStateInit();
		break;
	case APP_STATE_MSG_CLOSE:		
		appStateCloseDone();
		break;
	case APP_UI_MSG_USB_PLUG:	
		CLEAR_OSD_SRC;
		appStateChange(originalState,STATE_PARAM_USB_PLUG);
		break;
	default:		
		break;
	}

} 

