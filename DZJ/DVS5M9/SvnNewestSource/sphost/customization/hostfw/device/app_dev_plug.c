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
#include "app_dev_plug.h"
#include "app_tvlcd.h"
#include "app_disk.h"
#include "gpio_custom.h"
#include "sp5k_disk_api.h"
#include "sp5k_usb_api.h"
#include "app_osd_api.h"
#include "sp5k_pip_api.h" /*for clean warning*/
#include "app_still_timelapse.h"
#include "app_aaa.h"
#if ICAT_WIFI
#include "app_wifi_connection.h"
#endif
#include "app_ui_para.h"
#include "App_scdv_ui_para_def.h"
#include "App_rf_def.h"
#if HDMI_MENU_BY_JPG
extern UINT32 pipHDMIMenuFlag; 
#endif
UINT8 nHDMIFastBoot=1;
typedef enum {
	DEVICE_NONE_CHANGED = 0x00,
	DEVICE_PLUG_IN,
	DEVICE_PLUG_OUT,
} appDeviceChangeStatus_e;

typedef struct appDeviceStatus_s {
	UINT32 isCardPlugChanged;
	UINT32 isTvPlugChanged;
	UINT32 isUsbPlugChanged;
 	UINT32 isHDMIPlugChanged;
 } appDeviceStatus_t;

static appDeviceStatus_t appDevPlugInfo = {DEVICE_NONE_CHANGED, DEVICE_NONE_CHANGED, DEVICE_NONE_CHANGED,DEVICE_NONE_CHANGED};
extern BOOL is_power_on;
extern BOOL active_source_done;
extern UINT32 timelapseflag;
extern void appDiskDetect(void);

void
appHotPlugNextStChange(
void
)
{
	UINT32 nextState;

	switch(STATE_GROUP(appPreviousStateGet()))
	{
		case APP_STATE_GROUP_VIEW:
			nextState = APP_STATE_STILL_PREVIEW;
			break;
		case APP_STATE_GROUP_VIDEO:
			nextState = APP_STATE_VIDEO_PREVIEW;
			break;
		case APP_STATE_GROUP_AUDIO:
			nextState = APP_STATE_AUDIO_REC;
			break;
		case APP_STATE_GROUP_PLAYBACK:
			nextState = APP_STATE_PB_MAIN;
			break;
		case APP_STATE_GROUP_USB:
			nextState = APP_STATE_USB_CREATE;
			break;
		default:
			nextState = APP_STATE_VIDEO_PREVIEW;
			break;
	}
	if(appPreviousStateGet() == APP_STATE_USB_PCCAM){
		nextState = APP_STATE_USB_PCCAM;
	}
	appStateChange(nextState,STATE_PARAM_CARD_PLUG);
}


void
_cardPlugHandler(
	void
)
{
	UINT32 cfgSet;

	DBG_PRINT("isCardPlugChanged = %d, IS_CARD_EXIST = %d\n",appDevPlugInfo.isCardPlugChanged,IS_CARD_EXIST);

	#if 0
	if (((appDevPlugInfo.isCardPlugChanged == DEVICE_PLUG_IN) && !IS_CARD_EXIST) ||((appDevPlugInfo.isCardPlugChanged == DEVICE_PLUG_OUT) && IS_CARD_EXIST)) {
		printf("[WARN]Card Plug change status is [%08x] but Card gpio status is [%d]\n",appDevPlugInfo.isCardPlugChanged,IS_CARD_EXIST);
		appDevPlugInfo.isCardPlugChanged = DEVICE_NONE_CHANGED;
		return;
	}
	#endif
	cfgSet = CARD_PLUG_CFG_GET(appActiveStateHotPlugCfgGet());

	switch(cfgSet) {
		case HOT_PLUG_IGNORE:
			printf("[WARN] CARD plug msg is ignored!!!\n\n");
			appDevPlugInfo.isCardPlugChanged = DEVICE_NONE_CHANGED;
			break;
		case HOT_PLUG_PENDING:
			printf("[WARN] CARD plug msg is pending!!!\n\n");
			break;
		case HOT_PLUG_DO_PROC:
			DBG_PRINT("[INFO] Do CARD plaug proc!");
			appDevPlugInfo.isCardPlugChanged = DEVICE_NONE_CHANGED;			
	#if POWER_OFF_AFTER_CARD_PLUG
			appPowerOffUiMsgSend(APP_POWER_OFF_CARD_PLUG);
	#else/*hot plug card do*/
			appOsdLib_OsdLayerClearDraw();
			printf("*******HOT_PLUG_DO_PROC=(%x)\n",appPreviousStateGet()); 
			appDiskDetect();/**/			
			appAePreviewRestore();/*add for it would print "AE Locked !!" when pulling out card at capture*/
			if(!timelapseflag)
			{
				printf("CARD pulg timelapse set off!!");  
				timelapseflag = TS_OFF;
				sp5kHostMsgSend(APP_UI_MSG_TIMELAPSE_TASK_DEL,0);
			}
			if(IS_USB_IN)
			{				
				sp5kUsbSwCon(0);
			}
			#if 1
			if (IS_CARD_EXIST)
			{
				appDiskDetect();/**/
			}
			else
			{

				if(((appActiveStateGet()) == APP_STATE_VIDEO_REC)||(IS_VIDEO_GROUP(appPreviousStateGet())))
				{ 
					appStateChange(APP_STATE_VIDEO_PREVIEW,STATE_PARAM_NORMAL_INIT);
				}
				#if 0
				else if(IS_PLAYBACK_GROUP(appPreviousStateGet()))
				{
					appHotPlugNextStChange();
				}
				#endif
			}
			#endif
			appStateChange(APP_STATE_CARD_PLUG,STATE_PARAM_CARD_PLUG); /**/
	#endif
			break;
		case HOT_PLUG_DO_ABORT:
			printf("[WARN] send APP_STATE_MSG_ABORT !!!\n\n");
			sp5kHostMsgSend(APP_STATE_MSG_ABORT, STATE_PARAM_CARD_PLUG);
			break;
		case HOT_PLUG_DO_POWER_OFF:
			appDevPlugInfo.isCardPlugChanged = DEVICE_NONE_CHANGED;
			appPowerOffUiMsgSend(APP_POWER_OFF_CARD_PLUG);
			break;
		default:
			break;
	}
}


void
_usbPlugHandler(
	void
)
{
	UINT32 cfgSet;

	DBG_PRINT("isUsbPlugChanged = %d, IS_USB_IN = %d\n",appDevPlugInfo.isUsbPlugChanged,IS_USB_IN);
	if (((appDevPlugInfo.isUsbPlugChanged == DEVICE_PLUG_IN) && !IS_USB_IN)
		||((appDevPlugInfo.isUsbPlugChanged == DEVICE_PLUG_OUT) && IS_USB_IN))
	{
		printf("[WARN]USB Plug change status is [%08x] but USB gpio status is [%d]\n",appDevPlugInfo.isUsbPlugChanged,IS_USB_IN);
		appDevPlugInfo.isUsbPlugChanged = DEVICE_NONE_CHANGED;
		return;
	}
	cfgSet = USB_PLUG_CFG_GET(appActiveStateHotPlugCfgGet());

	switch(cfgSet)
	{
		case HOT_PLUG_IGNORE:
			printf("[WARN] USB plug msg is ignored!!!\n\n");
			appDevPlugInfo.isUsbPlugChanged = DEVICE_NONE_CHANGED;
			break;
		case HOT_PLUG_PENDING:
			printf("[WARN] USB plug msg is pending!!!\n\n");
			break;
		case HOT_PLUG_DO_PROC:
			DBG_PRINT("[INFO] Do USB plaug proc!");
			if (appDevPlugInfo.isUsbPlugChanged == DEVICE_PLUG_IN)
			{
				appDevPlugInfo.isUsbPlugChanged = DEVICE_NONE_CHANGED;
				appStateChange(APP_STATE_USB_CREATE,STATE_PARAM_USB_PLUG);
			}
			else
			{
				printf("--usb no chg\n");	
				appDevPlugInfo.isUsbPlugChanged = DEVICE_NONE_CHANGED;
				appLcdTurnOn();
				#if POWER_OFF_AFTER_USB_OUT
				appPowerOffUiMsgSend(APP_POWER_OFF_USB_PLUG);
				#else
				sp5kHostMsgSend(APP_UI_MSG_USB_PLUG, 0x00);
				#endif
			}
			break;
		case HOT_PLUG_DO_ABORT:
			printf("[WARN] send APP_STATE_MSG_ABORT !!!\n\n");
			sp5kHostMsgSend(APP_STATE_MSG_ABORT, STATE_PARAM_USB_PLUG);
			break;
		case HOT_PLUG_DO_POWER_OFF:
			appDevPlugInfo.isUsbPlugChanged = DEVICE_NONE_CHANGED;
			appPowerOffUiMsgSend(APP_POWER_OFF_USB_PLUG);
			break;
		default:
			break;
	}
}
void
_tvPlugHandler(
	void
)
{
	UINT32 cfgSet;
	DBG_PRINT("isTvPlugChanged = %d, IS_TV_IN = %d\n",appDevPlugInfo.isTvPlugChanged,IS_TV_IN);
	if (((appDevPlugInfo.isTvPlugChanged == DEVICE_PLUG_IN) && !IS_TV_IN)
		||((appDevPlugInfo.isTvPlugChanged == DEVICE_PLUG_OUT) && IS_TV_IN))
	{
		printf("[WARN]TV Plug change status is [%08x] but TV gpio status is [%d]\n",appDevPlugInfo.isTvPlugChanged,IS_TV_IN);
		appDevPlugInfo.isTvPlugChanged = DEVICE_NONE_CHANGED;
		return;
	}
	cfgSet = TV_PLUG_CFG_GET(appActiveStateHotPlugCfgGet());

	switch(cfgSet) {
		case HOT_PLUG_IGNORE:
			printf("[WARN] TV plug msg is ignored!!!\n\n");
			appDevPlugInfo.isTvPlugChanged = DEVICE_NONE_CHANGED;
			break;
		case HOT_PLUG_PENDING:
			printf("[WARN] TV plug msg is pending!!!\n\n");
			break;
		case HOT_PLUG_DO_PROC:
			DBG_PRINT("[INFO] Do TV plaug proc!");
			appDevPlugInfo.isTvPlugChanged = DEVICE_NONE_CHANGED;
			appStateChange(APP_STATE_TV_PLUG,STATE_PARAM_TV_PLUG);
			break;
		case HOT_PLUG_DO_ABORT:
			printf("[WARN] send APP_STATE_MSG_ABORT !!!\n\n");
			sp5kHostMsgSend(APP_STATE_MSG_ABORT, STATE_PARAM_TV_PLUG);
			break;
		case HOT_PLUG_DO_POWER_OFF:
			appDevPlugInfo.isTvPlugChanged = DEVICE_NONE_CHANGED;
			appPowerOffUiMsgSend(STATE_PARAM_TV_PLUG);
			break;
		default:
			break;
	}

}

#if (KIT_WITH_HDMI)

void
_hdmiPlugHandler(
	void
)
{
	UINT32 cfgSet;

	printf("ishdmiPlugChanged = %d, IS_HDMI_IN = %d\n",appDevPlugInfo.isHDMIPlugChanged,IS_HDMI_IN);

	if (((appDevPlugInfo.isHDMIPlugChanged == DEVICE_PLUG_IN) && !IS_HDMI_IN) ||((appDevPlugInfo.isHDMIPlugChanged == DEVICE_PLUG_OUT) && IS_HDMI_IN)) {
		printf("[WARN]HDMI Plug change status is [%08x] but HDMI gpio status is [%d]\n",appDevPlugInfo.isHDMIPlugChanged,IS_HDMI_IN);
		appDevPlugInfo.isHDMIPlugChanged = DEVICE_NONE_CHANGED;
		return;
	}
	cfgSet = HDMI_PLUG_CFG_GET(appActiveStateHotPlugCfgGet());

	switch(cfgSet) {
		case HOT_PLUG_IGNORE:
			printf("[WARN] HDMI 1 plug msg is ignored!!!\n\n");
			appDevPlugInfo.isHDMIPlugChanged = DEVICE_NONE_CHANGED;
			break;
		case HOT_PLUG_PENDING:
			printf("[WARN] HDMI 2 plug msg is pending!!!\n\n");
			break;
		case HOT_PLUG_DO_PROC:
			printf("[INFO] Do HDMI plaug proc!");
			uiPara_t* puiPara = appUiParaGet();
			if (appDevPlugInfo.isHDMIPlugChanged == DEVICE_PLUG_OUT){
				appDevPlugInfo.isHDMIPlugChanged = DEVICE_NONE_CHANGED;
				sp5kHostMsgSend(APP_KEY_LONG_PRESS_POWER, KEY_MSG_PARAM_DIRECT_RUN);	
                            //appStateChange(APP_STATE_HDMI_PLUG,STATE_PARAM_HDMI_PLUG);				
				break;
			}
			appDevPlugInfo.isHDMIPlugChanged = DEVICE_NONE_CHANGED;
			appStateChange(APP_STATE_CEC_CAPABILITY_CTRL,STATE_PARAM_HDMI_PLUG);
			break;
		case HOT_PLUG_DO_ABORT:
			printf("[WARN] send APP_STATE_MSG_ABORT !!!\n\n");
			sp5kHostMsgSend(APP_STATE_MSG_ABORT, STATE_PARAM_HDMI_PLUG);
			break;
		case HOT_PLUG_DO_POWER_OFF:
			printf("[WARN] send APP_STATE_MSG_power off !!!\n\n");
			appDevPlugInfo.isHDMIPlugChanged = DEVICE_NONE_CHANGED;
			appPowerOffUiMsgSend(STATE_PARAM_HDMI_PLUG);
			break;
		default:
			break;
	}

}

void
_hdmiPlugHandlerFastBoot(
         void
)

{        

         uiPara_t* puiPara = appUiParaGet();//ll@add

         UINT32 tmpstate1 = appPreviousStateGet();

         UINT32 tmpstate2 = appActiveStateGet();

         UINT32 tmpPlugstate0,tmpPlugstate1,tmpPlugstate2,tmpIOstate0,tmpIOstate1,tmpIOstate2;

         UINT32 cfgSet;

         

         printf("PreviousState = %x,\nActiveState   = %x\n",tmpstate1,tmpstate2);

         printf("ishdmiPlugChanged = %d, IS_HDMI_IN = %d\n",appDevPlugInfo.isHDMIPlugChanged,IS_HDMI_IN);

         

         tmpPlugstate2 = appDevPlugInfo.isHDMIPlugChanged,tmpIOstate2 = IS_HDMI_IN;

         if (((appDevPlugInfo.isHDMIPlugChanged == DEVICE_PLUG_IN) && !IS_HDMI_IN) ||((appDevPlugInfo.isHDMIPlugChanged == DEVICE_PLUG_OUT) && IS_HDMI_IN)){

                   printf("[WARN]HDMI Plug change status is [%08x] but HDMI gpio status is [%d]\n",appDevPlugInfo.isHDMIPlugChanged,IS_HDMI_IN);
                   appDevPlugInfo.isHDMIPlugChanged = DEVICE_NONE_CHANGED;
                   return;

         }
         tmpPlugstate0 = puiPara->HDMIplug0,tmpIOstate0 = puiPara->HDMIio0;
         tmpPlugstate1 = puiPara->HDMIplug1,tmpIOstate1 = puiPara->HDMIio1;
         printf("tmpPlugstate0 = %d, tmpIOstate0 = %d\n",tmpPlugstate0,tmpIOstate0);
         printf("tmpPlugstate1 = %d, tmpIOstate1 = %d\n",tmpPlugstate1,tmpIOstate1);
         printf("tmpPlugstate2 = %d, tmpIOstate2 = %d\n",tmpPlugstate2,tmpIOstate2);

         puiPara->HDMIplug0 = puiPara->HDMIplug1,puiPara->HDMIio0 = puiPara->HDMIio1;
         puiPara->HDMIplug1 = tmpPlugstate2,puiPara->HDMIio1 = tmpIOstate2;
         cfgSet = HDMI_PLUG_CFG_GET(appActiveStateHotPlugCfgGet());

         if(tmpPlugstate0==1&&tmpPlugstate1==1&&tmpPlugstate2==1&&tmpIOstate0==1&&tmpIOstate1==1&&tmpIOstate2==1)
          {

                            printf("[INFO] Ignore 4 HDMI plaug change!\n");
                            appDevPlugInfo.isHDMIPlugChanged = DEVICE_NONE_CHANGED;                        

         }
         else if (tmpPlugstate1!=tmpPlugstate2 || tmpIOstate1!=tmpIOstate2||(tmpPlugstate1==2&&tmpPlugstate2==2&&tmpIOstate1==0&&tmpIOstate2==0))
         {       
         switch(cfgSet) {

                   case HOT_PLUG_IGNORE:

                            printf("[WARN] HDMI 1 plug msg is ignored!!!\n\n");

                            appDevPlugInfo.isHDMIPlugChanged = DEVICE_NONE_CHANGED;

                            break;

                   case HOT_PLUG_PENDING:

                            printf("[WARN] HDMI 2 plug msg is pending!!!\n\n");

                            break;

                   case HOT_PLUG_DO_PROC:

                            printf("[INFO] Do HDMI plaug proc!");

                            if (appDevPlugInfo.isHDMIPlugChanged == DEVICE_PLUG_OUT){

                                     appDevPlugInfo.isHDMIPlugChanged = DEVICE_NONE_CHANGED;
			
					  sp5kHostMsgSend(APP_KEY_LONG_PRESS_POWER, KEY_MSG_PARAM_DIRECT_RUN);	
                                     //appStateChange(APP_STATE_HDMI_PLUG,STATE_PARAM_HDMI_PLUG);

                                     break;

                            }

                            appDevPlugInfo.isHDMIPlugChanged = DEVICE_NONE_CHANGED;

                            appStateChange(APP_STATE_CEC_CAPABILITY_CTRL,STATE_PARAM_HDMI_PLUG);

                            break;

                   case HOT_PLUG_DO_ABORT:

                            printf("[WARN] send APP_STATE_MSG_ABORT !!!\n\n");

                            sp5kHostMsgSend(APP_STATE_MSG_ABORT, STATE_PARAM_HDMI_PLUG);

                            break;

                   case HOT_PLUG_DO_POWER_OFF:

                            printf("[WARN] send APP_STATE_MSG_power off !!!\n\n");

                            appDevPlugInfo.isHDMIPlugChanged = DEVICE_NONE_CHANGED;

                            appPowerOffUiMsgSend(STATE_PARAM_HDMI_PLUG);

                            break;

                   default:

                            break;

                   }

         }

         else printf("[INFO] Ignore 3 HDMI plaug change!\n");

}

#endif

void
_devPlugStatusUpdate(
	UINT32 msg,
	UINT32 param
)
{
	switch(msg) {
	case APP_KEY_CARD_IN:
		appDevPlugInfo.isCardPlugChanged = DEVICE_PLUG_IN;
		break;
	case APP_KEY_CARD_OUT:
		appDevPlugInfo.isCardPlugChanged = DEVICE_PLUG_OUT;
		break;
	case APP_KEY_USB_IN:
		appDevPlugInfo.isUsbPlugChanged = DEVICE_PLUG_IN;
		break;
	case APP_KEY_USB_OUT:
		appDevPlugInfo.isUsbPlugChanged = DEVICE_PLUG_OUT;
		break;
	case APP_KEY_TV_IN:
		appDevPlugInfo.isTvPlugChanged = DEVICE_PLUG_IN;
		break;
	case APP_KEY_TV_OUT:
		appDevPlugInfo.isTvPlugChanged = DEVICE_PLUG_OUT;
		break;
	case APP_KEY_HDMI_IN:
		if (appDevPlugInfo.isHDMIPlugChanged == DEVICE_PLUG_OUT) {
			appDevPlugInfo.isHDMIPlugChanged = DEVICE_NONE_CHANGED;
			printf("[INFO] Ignore 1 HDMI plaug change!");
		}
		else {
			appDevPlugInfo.isHDMIPlugChanged = DEVICE_PLUG_IN;


			
 		}
		break;
	case APP_KEY_HDMI_OUT:
		if (appDevPlugInfo.isHDMIPlugChanged == DEVICE_PLUG_IN) {
			appDevPlugInfo.isHDMIPlugChanged = DEVICE_NONE_CHANGED;
			printf("[INFO] Ignore 2 HDMI plaug change!");
		}
		else 
		{
			if(appCurrDispModeGet() == APP_DISP_HDMI)
			{
 				appDevPlugInfo.isHDMIPlugChanged = DEVICE_PLUG_OUT;
			}
		}
		break;
	}
}

void
appDevPlugStatusChange(
	void
)
{
	if (appDevPlugInfo.isCardPlugChanged != DEVICE_NONE_CHANGED)
	{
		#if ICAT_WIFI
		sp5kHostMsgSend(APP_UI_MSG_POWER_OFF,APP_POWER_OFF_NORMAL);
		#else
		_cardPlugHandler();
		#endif
	}
	else if(appDevPlugInfo.isUsbPlugChanged != DEVICE_NONE_CHANGED)
	{
		#if ICAT_WIFI
		if((appWiFiConnection_UtilityStateGet() <= WIFI_UTILITY_CLOSE)){
			_usbPlugHandler();
		}
		#else
		_usbPlugHandler();
		#endif
	}
	else if(appDevPlugInfo.isTvPlugChanged != DEVICE_NONE_CHANGED)
	{
		if(appActiveStateGet() == APP_STATE_MENU || appActiveStateGet() == APP_STATE_VIDEO_REC)/*add for mantis bug 0047345*/
		{
			appStateChange(APP_STATE_VIDEO_PREVIEW,STATE_PARAM_NORMAL_INIT);	
		}
		#if ICAT_WIFI
		if((appWiFiConnection_UtilityStateGet() <= WIFI_UTILITY_CLOSE)){
			_tvPlugHandler();
		}
		#else
		_tvPlugHandler();
		#endif
	}
	#if (KIT_WITH_HDMI)
	else if(appDevPlugInfo.isHDMIPlugChanged != DEVICE_NONE_CHANGED)
	{
		if(appActiveStateGet() == APP_STATE_MENU )
		{
            		#if HDMI_MENU_BY_JPG
			pipHDMIMenuFlag = 0;			
            		#endif
			/* for 49210*/
			sp5kPipTerm();
		}
		
		//process busy rec/capture state change hdmi		
		if(appActiveStateGet() == APP_STATE_MENU || appActiveStateGet() == APP_STATE_VIDEO_REC)
		{
			appStateChange(APP_STATE_VIDEO_PREVIEW,STATE_PARAM_NORMAL_INIT);	
		}
		
		#if ICAT_WIFI
		if((appWiFiConnection_UtilityStateGet() <= WIFI_UTILITY_CLOSE)){
			if(nHDMIFastBoot)
				_hdmiPlugHandlerFastBoot();
			else
				_hdmiPlugHandler();
		}
		#else
		if(nHDMIFastBoot)
				_hdmiPlugHandlerFastBoot();
			else
		_hdmiPlugHandler();
		#endif
	}
	#endif
}

void
appDevHotPlugHandler(
	UINT32 msg,
	UINT32 param
)
{
	_devPlugStatusUpdate(msg,param);
	appDevPlugStatusChange();
}

