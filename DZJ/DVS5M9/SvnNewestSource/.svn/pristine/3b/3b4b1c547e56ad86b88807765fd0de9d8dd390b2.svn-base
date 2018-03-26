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

#include "app_osd_api_lcd.h"
#include "app_osd_draw_lcd.h"
#include "app_osd_api.h"
#include "app_osd.h"
#include "app_osdfont.h"
#include "gpio_custom.h"
#include "app_tvlcd.h"
#include "app_ui_para.h"

void appLcdOsdTextShow(SINT32 x, SINT32 y, UINT32 align, UINT8 *szStr, BOOL bHighLight)
{
	if(bHighLight == TRUE)
	{
		appOsd_FontTextFontSet(ID_FONT_TEXT_ENGLISH_YELLOW);
	}
	else
	{
		appOsd_FontTextFontSet(ID_FONT_TEXT_ENGLISH_GREY);
	}	
	appOsd_GfxFontIDSet(SP5K_RES_OSD_FONT);

	appOsdLib_TextDraw(x, y, align, szStr);

}

/*-------------------------------------------------------------------------
 *  Display functions for dvs5m3 
 *------------------------------------------------------------------------*/
void apposd_menu_videomode_display(char *level)
{	
	uiPara_t* puiPara = appUiParaGet();
	printf("hehe1=%d,level=%s\n",puiPara->Jump,level);
}

void apposd_menu_shootingmode_display(char *level)
{
	uiPara_t* puiPara = appUiParaGet();
	printf("hehe1=%d,level=%s\n",puiPara->Jump,level);
}

void apposd_menu_photopb_display(char *level)
{
	uiPara_t* puiPara = appUiParaGet();
	printf("hehe1=%d,level=%s\n",puiPara->Jump,level);
}

void apposd_menu_videopb_display(char *level)
{	
	uiPara_t* puiPara = appUiParaGet();
	printf("hehe1=%d,level=%s\n",puiPara->Jump,level);
}

void apposd_menu_setup_display(char *level)
{	
	uiPara_t* puiPara = appUiParaGet();
	printf("hehe1=%d,level=%s\n",puiPara->Jump,level);
}

/*-------------------------------------------------------------------------
 *  Mode for dvs5m3 
 *------------------------------------------------------------------------*/
void	apposd_menu_sdv(UINT8 layer, UINT8 item, UINT32 msg, UINT32 para)
		{	
			apposd_menu_videomode_set( layer, item, msg, para , NULL);
		}

void	apposd_menu_still(UINT8 layer, UINT8 item, UINT32 msg, UINT32 para)
		{
			apposd_menu_shootingmode_set( layer, item, msg, para, NULL);
		}

void	apposd_menu_photopb(UINT8 layer, UINT8 item, UINT32 msg, UINT32 para)
		{
			apposd_menu_photopb_set( layer, item, msg, para, NULL);
		}

void	apposd_menu_videopb(UINT8 layer, UINT8 item, UINT32 msg, UINT32 para)
		{
			apposd_menu_videopb_set( layer, item, msg, para, NULL);
		}

void    apposd_menu_setup(UINT8 layer, UINT8 item, UINT32 msg, UINT32 para)
		{
			apposd_menu_setup_set( layer, item, msg, para, NULL);
		}

/*-------------------------------------------------------------------------
 *  event for dvs5m3 
 *------------------------------------------------------------------------*/


void apposd_menu_show(UINT8 layer, UINT8 item, UINT32 msg, UINT32 para)
{	
	uiPara_t* puiPara = appUiParaGet(); 

	if(msg == APP_KEY_PRESS_RIGHT)
		{
		puiPara->Jump = 0;
		puiPara->ModeNum++;
		if(puiPara->ModeNum > 4)
			puiPara->ModeNum = 0;
		}
	printf("Mode = %d\n",puiPara->ModeNum);
	switch(puiPara->ModeNum)
		{
		case 0:
		apposd_menu_sdv( layer, item, msg, para);
		break;

		case 1:
		apposd_menu_still( layer, item, msg, para);
		break;

		case 2:
		apposd_menu_photopb( layer, item, msg, para);
		break;

		case 3:
		apposd_menu_videopb( layer, item, msg, para);
		break;
		
		case 4:
		apposd_menu_setup( layer, item, msg, para);
		break;
		}

}




