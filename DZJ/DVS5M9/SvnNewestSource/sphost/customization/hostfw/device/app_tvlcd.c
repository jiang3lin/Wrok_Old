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
#include "sp5k_os_api.h"
#include "sp5k_disp_api.h"
#include "sp5k_gfx_api.h"
#include "sp5k_rsvblk_api.h"
#include "app_tvlcd.h"
#include "app_osd_api.h"
#include "app_ui_para.h"
#include "api/sp5k_aud_api.h" 
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define TVLCD_INIT_THREAD_PRIORITY 	8
#define TVLCD_INIT_THREAD_TIME_SLICE   5
#define EVENT_LCD_INIT			0x0001
#define EVENT_TV_INIT			0x0002
#define EVENT_TVLCD_INIT		(EVENT_LCD_INIT | EVENT_TV_INIT)
#ifdef HW_PRJ_6330
#define BACKLIGHT_CONTROL_BY_GPIO 1
#else
#define BACKLIGHT_CONTROL_BY_GPIO 0

#endif

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
TX_EVENT_FLAGS_GROUP  evTvLcdInit;

static BOOL isDispFifInit = FALSE;
static BOOL gbIsLcdInit = FALSE; /* boolean flag to tell whether if the LCD has been init'd */
appDispMode_e gCurrDispMode = APP_DISP_NONE;
static BOOL isOsdInit = 0;
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern UINT32 fsFifOpen(const UINT8 *fileName);
extern UINT32 disp5kSpiRead(UINT32 addr,UINT32 bitsLen,UINT32 lsbFirst);
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/*-------------------------------------------------------------------------
 *  File Name : _dispClkSet
 *------------------------------------------------------------------------*/
void
_dispClkSet(
	UINT32 kHz
)
{
	#if SPCA5330
	extern void halPllDispPanelClkSet(UINT32, UINT32);
	halPllDispPanelClkSet(kHz/4, kHz);
	#else
	extern void halPllDispClkSet(UINT32, UINT32);
	halPllDispClkSet(kHz/4, kHz);
	#endif
}
/*S modify by xuan.ruan @ 2013.07.29*/
/*delete function:_dispSpiWrite()| _dispSpiRead()| _dispLcdInit()
   Move display init cmd table to driver file*/
/*E modify by xuan.ruan @ 2013.07.29*/

/*-------------------------------------------------------------------------
 *  File Name : _dispLcdBacklightPwmCtrl
 *------------------------------------------------------------------------*/
void
_dispLcdBacklightPwmCtrl(
	UINT32 a
)
{
#if SP5K_LCD_A025BN01_UPS052 || SP5K_LCD_TS027CAARD02_UPS052

#else
	if (a)
	{
		sp5kPwmCfgSet(1, 1, 35);
		sp5kPwmTrig( 1 << 1, 0 );
	}
	else
	{
		sp5kPwmTrig( 0, 1 << 1 );
	}
#endif
}
/*-------------------------------------------------------------------------
 *  File Name : _dispTvOut
 *------------------------------------------------------------------------*/
 void
_dispTvOut(
	UINT32 dispMode,
	UINT32 isWaitSwitchRdy
)
{
	DBG_PRINT("%s(%d,%d)\n",__FUNCTION__,dispMode,isWaitSwitchRdy);

	if (dispMode == APP_DISP_TV_NTSC)
	{
		gCurrDispMode = APP_DISP_TV_NTSC;
		sp5kDispDevSwitch( SP5K_DISP_DEV_NTSC );
	}
	else if(dispMode == APP_DISP_TV_PAL)
	{
		gCurrDispMode = APP_DISP_TV_PAL;
		sp5kDispDevSwitch( SP5K_DISP_DEV_PAL);
	}
	#if KIT_WITH_HDMI
	if (dispMode == APP_DISP_HDMI)
	{
		gCurrDispMode = APP_DISP_HDMI;
		//sp5kDispDevSwitch( SP5K_DISP_DEV_HDMI);
	}
	#endif
	if (!isWaitSwitchRdy)
	{
		return;
	}

	DBG_PRINT("\n\nbefore wait for SP5K_MSG_DISP_SWITCH_READY !!! \n");

	//appHostMsgWait(SP5K_MSG_DISP_SWITCH_READY, &param, 0);

	appDispGfxInit();

}

/*-------------------------------------------------------------------------
 *  File Name : _dispLcdOut
 *------------------------------------------------------------------------*/
 void
_dispLcdOut(
	UINT32 isWaitSwitchRdy
)
{
	/* %TODO% %REVIEW% :
	 * shall separate from different LCD panels
	 */
	UINT32 param;

	DBG_PRINT("%s(%d)\n",__FUNCTION__,isWaitSwitchRdy);

	profLogAdd(0, "_dispLcdOut:s");

	gCurrDispMode = APP_DISP_LCD;
	sp5kDispDevSwitch(SP5K_DISP_DEV_LCD);

	if (isWaitSwitchRdy)
	{
		DBG_PRINT("\n\nbefore wait for SP5K_MSG_DISP_SWITCH_READY !!! \n");
		appHostMsgWait(SP5K_MSG_DISP_SWITCH_READY, &param,0);
	}

	if (!gbIsLcdInit)
	{
		gbIsLcdInit = TRUE;
		appDispGfxInit();
	}
	else
	{
		APP_SLEEP_MS(250);
		LCD_WAKEUP;	//LCD_WAKEUP(1);
	}
	APP_SLEEP_MS(1);	//Delay1ms(1);
}

void appLcdTurnOn(void)
{
	#if BACKLIGHT_CONTROL_BY_GPIO
		LCD_BACKLIGHT_ON; //PwrLCDBackLight(0);
	#else
		_dispLcdBacklightPwmCtrl(1);
	#endif

	sp5kDispDevAttrSet(SP5K_DISP_GLOB_ACTIVE,ENABLE,0,0,0);

}


/*-------------------------------------------------------------------------
 *  File Name : appLcdTurnOff
 *------------------------------------------------------------------------*/
void appLcdTurnOff(void)
{

	#if BACKLIGHT_CONTROL_BY_GPIO
		LCD_BACKLIGHT_OFF; //PwrLCDBackLight(0);
	#else
		_dispLcdBacklightPwmCtrl(0);
	#endif
	sp5kDispDevAttrSet(SP5K_DISP_GLOB_ACTIVE,DISABLE,0,0,0);
}

/*-------------------------------------------------------------------------
 *  File Name : appDispGfxInit
 *------------------------------------------------------------------------*/
void
appDispGfxInit(
	void
)
{
	profLogAdd(0, "disp gfx init -s");

	if(!isDispFifInit)
	{
		profLogAdd(0, "disp fif init -s");
		if (fsFifOpen(FIF_DISP_DATA)==0)
		{
			profLogAdd(0, "disp load fail..");
			printf("[ERR] load %s fail..\n",FIF_DISP_DATA);
		}
		profLogAdd(0, "disp fif init -e");
		isDispFifInit = TRUE;
	}

	if(gCurrDispMode == APP_DISP_LCD)
	{
		//initial display display gamma
		sp5kResourceFileLoad(SP5K_RES_DISP_GAMMA , "DISP\\DISPGAMA.BIN");
		//initial display color matrix
		sp5kResourceFileLoad(SP5K_RES_DISP_COLOR_MATRIX, "DISP\\DISPMAX.BIN");

		profLogAdd(0, "Osd LcdMode :s");
		appOsdInit_LCDModeSet(gCurrDispMode);
		profLogAdd(0, "Osd LcdMode :e");
	}
	else
	{
		if(gCurrDispMode == APP_DISP_TV_NTSC){
			//sp5kDispDevSwitch( SP5K_DISP_DEV_NTSC );
			#if 1
				sp5kResourceFileLoad(SP5K_RES_DISP_GAMMA , "DISP\\DISPGAM0.BIN");
				//initial display color matrix
				sp5kResourceFileLoad(SP5K_RES_DISP_COLOR_MATRIX,  "DISP\\DISPMAX0.BIN"); //TV
			#endif
			/* for better TV out display quality, window width must be multiple of osd buffer(320)	*/
			sp5kGfxInitCfgSet(SP5K_GFX_AB_FRAME_SIZE, 320, 240, 0, 0);

			sp5kDispDevAttrSet(SP5K_DISP_IMG_WINDOW,42, 28, 640, 432);/*shrink image layer too*/
			sp5kDispDevAttrSet(SP5K_DISP_OSD_WINDOW,42, 28, 640, 432);/*shrink osd layer too*/
			profLogAdd(0, "Osd TvMode :s");
			appOsdInit_TVModeSet(gCurrDispMode);
			profLogAdd(0, "Osd TvMode :e");
		}
		else if(gCurrDispMode == APP_DISP_TV_PAL){
			#if 1
				sp5kResourceFileLoad(SP5K_RES_DISP_GAMMA , "DISP\\DISPGAM0.BIN");
					//initial display color matrix
				sp5kResourceFileLoad(SP5K_RES_DISP_COLOR_MATRIX,  "DISP\\DISPMAX0.BIN"); //TV
			#endif
					/* for better TV out display quality, window width must be multiple of osd buffer(320)	*/
			sp5kGfxInitCfgSet(SP5K_GFX_AB_FRAME_SIZE, 320, 240, 0, 0);

			sp5kDispDevAttrSet(SP5K_DISP_IMG_WINDOW,50, 32, 640, 512 );/*shrink image layer too*/
			sp5kDispDevAttrSet(SP5K_DISP_OSD_WINDOW,50, 32, 640, 512 );/*shrink osd layer too*/
			profLogAdd(0, "Osd TvMode :s");
			appOsdInit_TVModeSet(gCurrDispMode);
			profLogAdd(0, "Osd TvMode :e");
		}
		else{		/* DISP HDMI */
			//initial display display gamma
			sp5kResourceFileLoad(SP5K_RES_DISP_GAMMA , "DISP\\DISPGAM0.BIN");
			//initial display color matrix
			sp5kResourceFileLoad(SP5K_RES_DISP_COLOR_MATRIX, "DISP\\DISPMAX0.BIN"); //TV
			profLogAdd(0, "Osd TvMode :s");
			appOsdInit_TVModeSet(gCurrDispMode);
			profLogAdd(0, "Osd TvMode :e");
		}
	}

	profLogAdd(0, "disp gfx init -e");

}
/*-------------------------------------------------------------------------
 *  File Name : appLcdTvSwitch
 *------------------------------------------------------------------------*/
 void
 appLcdTvSwitch(
 	appDispMode_e dispDev,
 	UINT32 isWaitSwitchRdy
)
{
	UINT32 mode;

	profLogAdd(0, "tvlcdsw:s");
	DBG_PRINT("%s(%d %d,%d)\n",__FUNCTION__,gCurrDispMode, dispDev,isWaitSwitchRdy);
	
	if(gCurrDispMode == dispDev)
	{
		profLogAdd(0, "tvlcdsw:e(do nothing)");
		return;
	}

	//set STANDBY mode
	sp5kModeGet(&mode);
	if(mode != SP5K_MODE_STANDBY)
	{
		profLogAdd(0, "tvlcdsw:standby");

		sp5kModeSet(SP5K_MODE_STANDBY);
		appHostMsgWaitExact(SP5K_MSG_MODE_READY,SP5K_MODE_STANDBY,1000);
	}

	if (dispDev == APP_DISP_LCD)
	{ /* TV to LCD */
		DBG_PRINT("TV --> LCD \n");
		_dispLcdOut(isWaitSwitchRdy);
	}
	else
	{ /* LCD to TV or NTSC <-> PAL or HDMI*/
		DBG_PRINT("LCD --> TV or NTSC <-> PAL \n");
		if (IS_LCD_ACTIVE)
		{
			appLcdTurnOff();
		}
		_dispTvOut(dispDev,isWaitSwitchRdy);
	}
	profLogAdd(0, "tvlcdsw:e");
}

/*-------------------------------------------------------------------------
 *  File Name : appCurrDispModeGet
 *------------------------------------------------------------------------*/

appDispMode_e
appCurrDispModeGet(
	void
)
{
	return gCurrDispMode;
}


/*-------------------------------------------------------------------------
 *  File Name : _tvLcdInitThread
 *------------------------------------------------------------------------*/
void
_tvLcdInitThread(
	ULONG init_data
)
{
	ULONG ulActualEvents;
	uiPara_t* puiPara = appUiParaGet();
	
	DBG_PRINT("%s start\n",__FUNCTION__);

	while(1)
	{
		sp5kOsEventFlagsGet(&evTvLcdInit,EVENT_TVLCD_INIT,TX_OR_CLEAR,&ulActualEvents,TX_WAIT_FOREVER);

		profLogPrintf(0, "::_tvLcdInitThread(%d)",ulActualEvents);

		if (ulActualEvents & EVENT_LCD_INIT)
		{
			appLcdTvSwitch(APP_DISP_LCD,0);
		}
		else
		{
			appLcdTvSwitch(puiPara->TVMode,0);
		}

		if(!isOsdInit)
		{
			profLogAdd(0, "osd init");
			appOsdInit();
			isOsdInit = 1;
		}
	}
}

/*-------------------------------------------------------------------------
 *  File Name : _tvLcdInitStart
 *------------------------------------------------------------------------*/
void
_tvLcdInitStart(
	UINT32 event
)
{
	uiPara_t* puiPara = appUiParaGet();
	
	if((event == EVENT_LCD_INIT && gCurrDispMode == APP_DISP_LCD)
		  || (event != EVENT_LCD_INIT && gCurrDispMode == puiPara->TVMode))
	{
		profLogAdd(0, "_tvLcdInitStart:(do nothing)");
		return;
	}
	DBG_PRINT("%s start\n",__FUNCTION__);
	profLogPrintf(0, "_tvLcdInitStart(%d)",event);
	#if 0
	sp5kOsEventFlagsSet(&evTvLcdInit,event,TX_OR);
	#else
	if (event == EVENT_LCD_INIT)
	{
		sp5kOsEventFlagsSet(&evTvLcdInit,event,TX_OR);
	}
	else
	{
		appLcdTvSwitch(puiPara->TVMode,TRUE);
		if(!isOsdInit)
		{
			profLogAdd(0, "osd init");
			appOsdInit();
			isOsdInit = 1;
		}
	}
	#endif
}

/*-------------------------------------------------------------------------
 *  File Name : appTvLcdInit
 *------------------------------------------------------------------------*/
void
appTvLcdInit(
	void
)
{
	TX_THREAD *ptrdTvLcd;

	DBG_PRINT("%s start\n",__FUNCTION__);
	profLogAdd(0, "tvlcd init");

	sp5kOsEventFlagsCreate(&evTvLcdInit, "TvLcdInitEvent");

	ptrdTvLcd = sp5kOsThreadCreate(
					  "TvLcdInitThread",
			                _tvLcdInitThread,
			                0,
			                TVLCD_INIT_THREAD_PRIORITY,
			                TVLCD_INIT_THREAD_PRIORITY,
			                TVLCD_INIT_THREAD_TIME_SLICE,
			                TX_AUTO_START );

}

/*-------------------------------------------------------------------------
 *  File Name : appTvLcdStart
 *------------------------------------------------------------------------*/
void
appTvLcdStart(
	BOOL fastBoot
)
{
	profLogAdd(0, "tvlcd start");
	DBG_PRINT("%s (%d) start\n",__FUNCTION__,fastBoot);
	if (fastBoot)
	{
		if (!IS_TV_IN)
		{
			_tvLcdInitStart(EVENT_LCD_INIT);
		}
	}
	else
	{
		_tvLcdInitStart(IS_TV_IN ? EVENT_TV_INIT : EVENT_LCD_INIT);
	}
}
