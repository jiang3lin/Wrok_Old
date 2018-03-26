//@longlv

#define HOST_DBG 0

#include "app_com_def.h"
#include "app_com_api.h"

#include "sp5k_global_api.h"
#include "sp5k_ae_api.h"
#include "sp5k_capture_api.h"
#include "sp5k_disp_api.h"
#include "sp5k_otrack_api.h"
#include "sp5k_usb_api.h"
#include "sp5k_rsvblk_api.h"
#include "sp5k_disk_api.h"
#include "sp5k_cdsp_api.h"
#include "sp5k_gfx_api.h"
#include "sp5k_media_api.h"
#include "sp5k_af_api.h"
#include "Sp5k_usb_api.h"

#include "app_osd_draw_lcd.h"
#include "app_osd_api_lcd.h"
#include "app_osd_api.h"
#include "app_osd.h"
#include "app_osdfont.h"

#include "app_battery.h"
#include "gpio_custom.h"
#include "app_ui_para.h"

void appLcdOsdDrawPowerOnLogo(void)
{
	profLogAdd(0, "appLcmOsdDrawPowerOnLogo s");

	//appLcmOsdClean();
	//appLcmOsdTest(FALSE);
	appLcdOsdTextShow(LCM_OSD_CENTER_X, LCM_OSD_CENTER_Y, SP5K_GFX_ALIGN_CENTER, "What is it?", FALSE);
	//appLcmRefresh();

	LCD_BACKLIGHT_ON; 

	profLogAdd(0, "appLcmOsdDrawPowerOnLogo e");
	
}

/*-------------------------------------------------------------------------
 *  Logic Function for dvs5m3 
 *------------------------------------------------------------------------*/
 
void
apposd_menu_videomode_set(UINT8 layer, UINT8 item, UINT32 msg, UINT32 para,char *level)
{
	uiPara_t* puiPara = appUiParaGet();
	static UINT8 jumpmax;
	
	//char *level=NULL;

	/*logic*/
	if(level==NULL)
		jumpmax=1;
	if(puiPara->Jump==0&&level==NULL&&msg==APP_KEY_PRESS_LEFT)
		puiPara->JumpTmp=puiPara->Jump,puiPara->Jump=0,jumpmax=3,level="Video Mode";
	if(puiPara->Jump==1&&level==NULL&&msg==APP_KEY_PRESS_LEFT)
		puiPara->JumpTmp=puiPara->Jump,puiPara->Jump=0,jumpmax=4,level="Resolution";
	
	if(msg == APP_KEY_PRESS_UP)
		{
		puiPara->Jump--;
		if(puiPara->Jump<0)
			puiPara->Jump=jumpmax;
		}
	if(msg == APP_KEY_PRESS_DOWN)
		{
		puiPara->Jump++;
		if(puiPara->Jump>jumpmax)
			puiPara->Jump=0;
		}
	/*status*/
	if(level =="Video Mode"&&msg ==APP_KEY_PRESS_LEFT)
		{
		switch(puiPara->Jump)
			{
			case 0:;
			break;
			case 1:;
			break;
			case 2:;
			break;
			case 3:;
			break;
			}
		puiPara->Jump=puiPara->JumpTmp,level=NULL;
		}
	
	if(level =="Resolution"&&msg ==APP_KEY_PRESS_LEFT)
		{
		switch(puiPara->Jump)
			{
			case 0:;
			break;
			case 1:;
			break;
			case 2:;
			break;
			case 3:;
			break;
			case 4:;
			break;
			}
		puiPara->Jump=puiPara->JumpTmp,level=NULL;
		}
	
	/*display*/
	apposd_menu_videomode_display(level);
}

void
apposd_menu_shootingmode_set(UINT8 layer, UINT8 item, UINT32 msg, UINT32 para,char *level)
{
	uiPara_t* puiPara = appUiParaGet();
	static UINT8 jumpmax;
	
	//char *level=NULL;

	/*logic*/
	if(level==NULL)
		jumpmax=1;
	if(puiPara->Jump==0&&level==NULL&&msg==APP_KEY_PRESS_LEFT)
		puiPara->JumpTmp=puiPara->Jump,puiPara->Jump=0,jumpmax=3,level="Shooting Mode";
	if(puiPara->Jump==1&&level==NULL&&msg==APP_KEY_PRESS_LEFT)
		puiPara->JumpTmp=puiPara->Jump,puiPara->Jump=0,jumpmax=3,level="Resolution";
	if(puiPara->Jump==0,level=="Shooting Mode"&&msg==APP_KEY_PRESS_LEFT)
		puiPara->JumpTmp=puiPara->Jump,puiPara->Jump=0,level=NULL;
	if(puiPara->Jump==1,level=="Shooting Mode"&&msg==APP_KEY_PRESS_LEFT)
		puiPara->JumpTmp=puiPara->Jump,puiPara->Jump=0,jumpmax=2,level="Burst";
	if(puiPara->Jump==2,level=="Shooting Mode"&&msg==APP_KEY_PRESS_LEFT)
		puiPara->JumpTmp=puiPara->Jump,puiPara->Jump=0,jumpmax=4,level="Lapse";
	if(puiPara->Jump==3,level=="Shooting Mode"&&msg==APP_KEY_PRESS_LEFT)
		puiPara->JumpTmp=puiPara->Jump,puiPara->Jump=0,jumpmax=2,level="Selftimer";	
	
	
	if(msg == APP_KEY_PRESS_UP)
		{
		puiPara->Jump--;
		if(puiPara->Jump<0)
			puiPara->Jump=jumpmax;
		}
	if(msg == APP_KEY_PRESS_DOWN)
		{
		puiPara->Jump++;
		if(puiPara->Jump>jumpmax)
			puiPara->Jump=0;
		}
	/*status*/
	if(level =="Shooting Mode"&&msg ==APP_KEY_PRESS_LEFT)
		{
		switch(puiPara->Jump)
			{
			case 0:;
			break;
			case 1:;
			break;
			case 2:;
			break;
			case 3:;
			break;
			}
		puiPara->Jump=puiPara->JumpTmp,level=NULL;
		}
	
	if(level =="Resolution"&&msg ==APP_KEY_PRESS_LEFT)
		{
		switch(puiPara->Jump)
			{
			case 0:;
			break;
			case 1:;
			break;
			case 2:;
			break;
			case 3:;
			break;
			}
		puiPara->Jump=puiPara->JumpTmp,level=NULL;
		}

		if(level =="Burst"&&msg ==APP_KEY_PRESS_LEFT)
		{
		switch(puiPara->Jump)
			{
			case 0:;
			break;
			case 1:;
			break;
			case 2:;
			break;
			case 3:;
			break;
			}
		puiPara->Jump=puiPara->JumpTmp,level=NULL;
		}

	if(level =="Lapse"&&msg ==APP_KEY_PRESS_LEFT)
		{
		switch(puiPara->Jump)
			{
			case 0:;
			break;
			case 1:;
			break;
			case 2:;
			break;
			case 3:;
			break;
			}
		puiPara->Jump=puiPara->JumpTmp,level=NULL;
		}

	if(level =="Selftimer"&&msg ==APP_KEY_PRESS_LEFT)
		{
		switch(puiPara->Jump)
			{
			case 0:;
			break;
			case 1:;
			break;
			case 2:;
			break;
			case 3:;
			break;
			}
		puiPara->Jump=puiPara->JumpTmp,level=NULL;
		}
	
	/*display*/
	apposd_menu_shootingmode_display(level);
}



void
apposd_menu_photopb_set(UINT8 layer, UINT8 item, UINT32 msg, UINT32 para,char *level)
{
	uiPara_t* puiPara = appUiParaGet();
	static UINT8 jumpmax;
	
	//char *level=NULL;

	/*logic*/
	if(level==NULL)
		jumpmax=1;
	if(puiPara->Jump==0&&level==NULL&&msg==APP_KEY_PRESS_LEFT)
		puiPara->JumpTmp=puiPara->Jump,puiPara->Jump=0,jumpmax=1,level="Delete one";
	if(puiPara->Jump==1&&level==NULL&&msg==APP_KEY_PRESS_LEFT)
		puiPara->JumpTmp=puiPara->Jump,puiPara->Jump=0,jumpmax=1,level="Delete all";
	
	
	if(msg == APP_KEY_PRESS_UP)
		{
		puiPara->Jump--;
		if(puiPara->Jump<0)
			puiPara->Jump=jumpmax;
		}
	if(msg == APP_KEY_PRESS_DOWN)
		{
		puiPara->Jump++;
		if(puiPara->Jump>jumpmax)
			puiPara->Jump=0;
		}
	/*status*/
	if(level =="Delete one"&&msg ==APP_KEY_PRESS_LEFT)
		{
		switch(puiPara->Jump)
			{
			case 0:
				;
			break;
			case 1:
				;
			break;
			}
		puiPara->Jump=puiPara->JumpTmp,level=NULL;
		}
	
	if(level =="Delete all"&&msg ==APP_KEY_PRESS_LEFT)
		{
		switch(puiPara->Jump)
			{
			case 0:;
			break;
			case 1:;
			break;
			case 2:;
			break;
			case 3:;
			break;
			case 4:;
			break;
			}
		puiPara->Jump=puiPara->JumpTmp,level=NULL;
		}
	
	/*display*/
	apposd_menu_photopb_display(level);
}

void
apposd_menu_videopb_set(UINT8 layer, UINT8 item, UINT32 msg, UINT32 para,char *level)
{
	uiPara_t* puiPara = appUiParaGet();
	static UINT8 jumpmax;
	
	//char *level=NULL;

	/*logic*/
	if(level==NULL)
		jumpmax=1;
	if(puiPara->Jump==0&&level==NULL&&msg==APP_KEY_PRESS_LEFT)
		puiPara->JumpTmp=puiPara->Jump,puiPara->Jump=0,jumpmax=1,level="Delete one";
	if(puiPara->Jump==1&&level==NULL&&msg==APP_KEY_PRESS_LEFT)
		puiPara->JumpTmp=puiPara->Jump,puiPara->Jump=0,jumpmax=1,level="Delete all";
	
	
	if(msg == APP_KEY_PRESS_UP)
		{
		puiPara->Jump--;
		if(puiPara->Jump<0)
			puiPara->Jump=jumpmax;
		}
	if(msg == APP_KEY_PRESS_DOWN)
		{
		puiPara->Jump++;
		if(puiPara->Jump>jumpmax)
			puiPara->Jump=0;
		}
	/*status*/
	if(level =="Delete one"&&msg ==APP_KEY_PRESS_LEFT)
		{
		switch(puiPara->Jump)
			{
			case 0:
				;
			break;
			case 1:
				;
			break;
			}
		puiPara->Jump=puiPara->JumpTmp,level=NULL;
		}
	
	if(level =="Delete all"&&msg ==APP_KEY_PRESS_LEFT)
		{
		switch(puiPara->Jump)
			{
			case 0:;
			break;
			case 1:;
			break;
			case 2:;
			break;
			case 3:;
			break;
			case 4:;
			break;
			}
		puiPara->Jump=puiPara->JumpTmp,level=NULL;
		}
	
	/*display*/
	apposd_menu_videopb_display(level);
}

void
apposd_menu_setup_set(UINT8 layer, UINT8 item, UINT32 msg, UINT32 para,char *level)
{
	uiPara_t* puiPara = appUiParaGet();
	static UINT8 jumpmax;
	
	//char *level=NULL;

	/*logic*/
	if(level==NULL)
		jumpmax=11;
	if(puiPara->Jump==0&&level==NULL&&msg==APP_KEY_PRESS_LEFT)
		puiPara->JumpTmp=puiPara->Jump,puiPara->Jump=0,jumpmax=1,level="D.I.S";
	if(puiPara->Jump==1&&level==NULL&&msg==APP_KEY_PRESS_LEFT)
		puiPara->JumpTmp=puiPara->Jump,puiPara->Jump=0,jumpmax=1,level="WDR";
	if(puiPara->Jump==2&&level==NULL&&msg==APP_KEY_PRESS_LEFT)
		puiPara->JumpTmp=puiPara->Jump,puiPara->Jump=0,jumpmax=1,level="Invert";
	if(puiPara->Jump==3&&level==NULL&&msg==APP_KEY_PRESS_LEFT)
		puiPara->JumpTmp=puiPara->Jump,puiPara->Jump=0,jumpmax=2,level="Beep";
	if(puiPara->Jump==4&&level==NULL&&msg==APP_KEY_PRESS_LEFT)
		puiPara->JumpTmp=puiPara->Jump,puiPara->Jump=0,jumpmax=1,level="Date Stamp";
	if(puiPara->Jump==5&&level==NULL&&msg==APP_KEY_PRESS_LEFT)
		puiPara->JumpTmp=puiPara->Jump,puiPara->Jump=0,jumpmax=4,level="Auto power off";
	if(puiPara->Jump==6&&level==NULL&&msg==APP_KEY_PRESS_LEFT)
		puiPara->JumpTmp=puiPara->Jump,puiPara->Jump=0,jumpmax=5,level="Date/Time";
	if(puiPara->Jump==7&&level==NULL&&msg==APP_KEY_PRESS_LEFT)
		puiPara->JumpTmp=puiPara->Jump,puiPara->Jump=0,jumpmax=1,level="Frequency";
	if(puiPara->Jump==8&&level==NULL&&msg==APP_KEY_PRESS_LEFT)
		puiPara->JumpTmp=puiPara->Jump,puiPara->Jump=0,jumpmax=1,level="Format";
	if(puiPara->Jump==9&&level==NULL&&msg==APP_KEY_PRESS_LEFT)
		puiPara->JumpTmp=puiPara->Jump,puiPara->Jump=0,jumpmax=1,level="Default";
	if(puiPara->Jump==10&&level==NULL&&msg==APP_KEY_PRESS_LEFT)
		puiPara->JumpTmp=puiPara->Jump,puiPara->Jump=0,jumpmax=1,level="Car DV";
	if(puiPara->Jump==11&&level==NULL&&msg==APP_KEY_PRESS_LEFT)
		puiPara->JumpTmp=puiPara->Jump,puiPara->Jump=0,jumpmax=0,level="Version No.";

	
	if(msg == APP_KEY_PRESS_UP)
		{
		puiPara->Jump--;
		if(puiPara->Jump<0)
			puiPara->Jump=jumpmax;
		}
	if(msg == APP_KEY_PRESS_DOWN)
		{
		puiPara->Jump++;
		if(puiPara->Jump>jumpmax)
			puiPara->Jump=0;
		}
	/*status*/
	if(level =="D.I.S"&&msg ==APP_KEY_PRESS_LEFT)
		{
		switch(puiPara->Jump)
			{
			case 0:
				;
			break;
			case 1:
				;
			break;
			}
		puiPara->Jump=puiPara->JumpTmp,level=NULL;
		}
	
	if(level =="WDR"&&msg ==APP_KEY_PRESS_LEFT)
		{
		switch(puiPara->Jump)
			{
			case 0:;
			break;
			case 1:;
			break;
			}
		puiPara->Jump=puiPara->JumpTmp,level=NULL;
		}
	
	if(level =="Invert"&&msg ==APP_KEY_PRESS_LEFT)
		{
		switch(puiPara->Jump)
			{
			case 0:;
			break;
			case 1:;
			break;
			}
		puiPara->Jump=puiPara->JumpTmp,level=NULL;
		}	
	
	if(level =="Beep"&&msg ==APP_KEY_PRESS_LEFT)
		{
		switch(puiPara->Jump)
			{
			case 0:;
			break;
			case 1:;
			break;
			case 2:;
			break;
			}
		puiPara->Jump=puiPara->JumpTmp,level=NULL;
		}	

	if(level =="Date Stamp"&&msg ==APP_KEY_PRESS_LEFT)
		{
		switch(puiPara->Jump)
			{
			case 0:;
			break;
			case 1:;
			break;
			
			}
		puiPara->Jump=puiPara->JumpTmp,level=NULL;
		}	
	
	if(level =="Auto power off"&&msg ==APP_KEY_PRESS_LEFT)
		{
		switch(puiPara->Jump)
			{
			case 0:;
			break;
			case 1:;
			break;
			case 2:;
			break;
			case 3:;
			break;
			case 4:;
			break;
			}
		puiPara->Jump=puiPara->JumpTmp,level=NULL;
		}
	
	if(level =="Date/Time"&&msg ==APP_KEY_PRESS_LEFT)
		{
		switch(puiPara->Jump)
			{
			case 0:;
			break;
			case 1:;
			break;
			case 2:;
			break;
			case 3:;
			break;
			case 4:;
			break;
			case 5:;
			break;
			}
		puiPara->Jump=puiPara->JumpTmp,level=NULL;
		}
	
	if(level =="Frequency"&&msg ==APP_KEY_PRESS_LEFT)
		{
		switch(puiPara->Jump)
			{
			case 0:;
			break;
			case 1:;
			break;
			
			}
		puiPara->Jump=puiPara->JumpTmp,level=NULL;
		}	

	if(level =="Format"&&msg ==APP_KEY_PRESS_LEFT)
		{
		switch(puiPara->Jump)
			{
			case 0:;
			break;
			case 1:;
			break;
			
			}
		puiPara->Jump=puiPara->JumpTmp,level=NULL;
		}	

	if(level =="Default"&&msg ==APP_KEY_PRESS_LEFT)
		{
		switch(puiPara->Jump)
			{
			case 0:;
			break;
			case 1:;
			break;
			
			}
		puiPara->Jump=puiPara->JumpTmp,level=NULL;
		}	

	if(level =="Car DV"&&msg ==APP_KEY_PRESS_LEFT)
		{
		switch(puiPara->Jump)
			{
			case 0:;
			break;
			case 1:;
			break;
			
			}
		puiPara->Jump=puiPara->JumpTmp,level=NULL;
		}	

	if(level =="Version No."&&msg ==APP_KEY_PRESS_LEFT)
		{
		switch(puiPara->Jump)
			{
			case 0:;
			break;			
			}
		puiPara->Jump=puiPara->JumpTmp,level=NULL;
		}	

	/*display*/
	apposd_menu_setup_display(level);

}

//CAR DV

