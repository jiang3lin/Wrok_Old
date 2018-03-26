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
#include "common.h"
#include <stdio.h>
#include "sp5k_disp_api.h"
#include "sp5k_gfx_api.h"
#include "sp5k_disk_api.h"
#include "sp5k_rsvblk_api.h"
#include "sp5k_cdsp_api.h"
#include "sp5k_global_api.h"
#include "app_com_api.h"
#include "app_com_def.h"
#include "app_icon_def.h"
#include "app_osd.h"
#include "app_still.h"
#include "app_view_param.h"
#include "app_osd_api.h"
#include "app_osdfont.h"
#include "app_battery.h"
#include "app_view_osd.h"
#include "app_playback_osd.h"
#include "app_jpg_draw.h"
#include "app_error_msg.h"
#include "app_stringid.h"
#include "app_audio.h"
#include "app_face_detect_ex.h"
#include "app_video.h"
#include "app_tvlcd.h"
#include "app_ui_para.h" 
#include "app_menu_tab.h"


/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
static UINT8 gAutoFitTag = 0;/*enable auto fit in modesw*/


UINT16 PicSizeIcon[]=
{
/*	related function not ready, removed for temporarily by gx
	ID_ICON_RESOLUTION_20M,
	ID_ICON_RESOLUTION_16M,	
	ID_ICON_RESOLUTION_14M,
	ID_ICON_RESOLUTION_12M,
*/
	ID_ICON_RESOLUTION_10M,
	#if !CAM_TYPE_CVR
	ID_ICON_RESOLUTION_9M,
	#endif
	ID_ICON_RESOLUTION_8M,
	ID_ICON_RESOLUTION_6M,
	#if !CAM_TYPE_CVR
	ID_ICON_RESOLUTION_7M_16_9,
	ID_ICON_RESOLUTION_5M,
	#endif
	ID_ICON_RESOLUTION_3M,
	ID_ICON_RESOLUTION_2M_16_9,
	#if !CAM_TYPE_CVR
	ID_ICON_RESOLUTION_2M,
	ID_ICON_RESOLUTION_1M,
	#endif
	ID_ICON_RESOLUTION_VGA
};


UINT16 QualityIcon[]=
{
	ID_ICON_QUALITY_SUPER_FINE, 
	ID_ICON_QUALITY_FINE,
	ID_ICON_QUALITY
};

UINT16 PasmIcon[] = 
{
	ID_ICON_PASM_PROGRAM,
	ID_ICON_PASM_APERTURE,
	ID_ICON_PASM_TV,
	ID_ICON_PASM_MANUAL,            
};	

UINT16 SceneIcon[] = 
{
	[SCENE_SMILE_DETECTION	]=ID_ICON_SMILE_DETECT,
	[SCENE_SHAKE_FREE      	]=ID_ICON_SCENE_SHAKEFREE,			
	[SCENE_LANDSCAPE       	]=ID_ICON_SCENE_LANDSCAPE, 		
	[SCENE_BACK_LIGHT	  	]=ID_ICON_SCENE_BACKLIGHT,	
	[SCENE_NIGHT 			]=ID_ICON_SCENE_NIGHTSCENERY,
	[SCENE_BUILDING			]=ID_ICON_SCENE_BUILDING,
	[SCENE_CHILDREN			]=ID_ICON_SCENE_CHILDREN,
	[SCENE_PORTRAIT			]=ID_ICON_SCENE_PORTRAIT,
	[SCENE_FOOD				]=ID_ICON_SCENE_DINNER,
	[SCENE_TEXT				]=ID_ICON_SCENE_TEXT,
	[SCENE_BEACH_SNOW		]=ID_ICON_SCENE_SNOW,
	[SCENE_FIRE_WORKS		]=ID_ICON_SCENE_FIREWORKS,
	[SCENE_SUNSET			]=ID_ICON_SCENE_SUNSET,
	[SCENE_WDR				]=ID_ICON_SCENE_WDR,
	[SCENE_HDR				]=ID_ICON_SCENE_HDR,
	[SCENE_PANORAMA		]=ID_ICON_SCENE_PANORAMA,
	[SCENE_BLINK			]=ID_ICON_BLINKING_DETECT,
};	

UINT8* aperturevValueStr[7][2]=
{
		{"F2.8", "F5.6"},
		{"F3.1", "F6.3"},
		{"F3.4", "F6.9"},
		{"F3.8", "F7.6"},
		{"F4.2", "F8.5"},
		{"F4.7", "F9.5"},
		{"F5.2", "F10.4"}
};	

UINT8* shutterSpeedValueStr[]=
{
		"AUTO",   
		"16.0",   		
		"8.0",   	
		"4.0",   
		"2.0",   
		"1.0",   
		"1/2",   
		"1/4",  
		"1/8",   
		"1/15",  
		"1/30",  
		"1/60",  
		"1/125",
		"1/250", 
		"1/500", 
		"1/1000",
		"1/2000",
		"1/4000",
		"1/8000",
		"MAX",
};

UINT8* isoValueStr[UI_ISO_MAX]=
{
	[UI_ISO_50] 	= "ISO 50",
	[UI_ISO_100] 	= "ISO 100",
	[UI_ISO_200] 	= "ISO 200",
	[UI_ISO_400] 	= "ISO 400",
	[UI_ISO_800] 	= "ISO 800",

};	

UINT32 isoValue[UI_ISO_MAX]=
{
	[UI_ISO_50] 	= 50,
	[UI_ISO_100] 	= 100,
	[UI_ISO_200] 	= 200,
	[UI_ISO_400] 	= 400,
	[UI_ISO_800] 	= 800,
	[UI_ISO_1600]	= 1600,
};


typedef struct 
{
	UINT32 status;
	UINT32 timer;
} viewosdobj_t;

viewosdobj_t gHistogram =
{
	.status=OSD_OBJ_DESTROYED,
	.timer=TIMER_NULL,
};

UINT32 HistogramBuf[256];
UINT32 ZoomBarCounter =TIMER_NULL;

UINT32 ViewOsdPresentStatus=0;
unsigned int nCaptureRemainNum=0;

#define ID_ICON_DIGITAL_ZOOM		0x0000
#define ID_ICON_OPTICAL_ZOOM		0x0001

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
appFdResult_t faceOsd = 
{ 
	0, 
	0,
	{
		{ 20,  20,  60,  60, 0},
		{100,  20,  60,  60, 0},
		{ 20, 100,  60,  60, 0},
		{100, 100,  60,  60, 0},
		{150, 150,  60,  60, 0}
	}
};

/* Add by sc.liu */
appOdResult_t objectOsd = 
{ 
	0, 
	0,
	{
		{ 20,  20,  60,  60, 0},
		{100,  20,  60,  60, 0},
		{ 20, 100,  60,  60, 0},
		{100, 100,  60,  60, 0},
		{150, 150,  60,  60, 0}
	}
};
/* Add by sc.liu */

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern appAudioCB_t gAudioCB;
extern void appAudioOsdDiskShow(void);
extern photoframeFile_t photoFrameTable[];
extern BOOL ObjTrackTargetStop;		/* Add by sc.liu */

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
void appViewOsd_ASMChangeDraw(void);
void appViewOsdValidAreaAutoFitSet(UINT8 bEnable){
	gAutoFitTag = bEnable;
}
UINT8 appViewOsdValidAreaAutoFitGet(void)
{
	return gAutoFitTag;
}
void 
appViewOsdValidAreaShow(
	UINT16 imgw, 
	UINT16 imgh
)
{
	/*s Modify by Aries 10/06/22*/
	#if KIT_SENSOR_FIT_IMAGE
    UINT8 bAutoFit = 0;
	if(imgw == 1280 && imgh == 720)
	{
		bAutoFit = 1;
	}
	appViewOsdValidAreaAutoFitSet(bAutoFit);
	#else
	// use pf layer to cover unused area instead of sp5k size change	
	static UINT32 h0=0;
	UINT32 h, pfw=LCD_SIZE_X, pfh=LCD_SIZE_Y;
	
	 printf("\n_________%s: iw=%d, ih=%d, h0=%d\n\n", __func__, imgw, imgh, h0);	
	
	
	if(imgw==0) imgw=LCD_SIZE_X;
	if(imgh==0) imgh=LCD_SIZE_Y;
	
	if(imgw*300 < imgh*400*1.1 && imgw*300 > imgh*400*0.9 && h0==0) 
	{
		return;  
	}
		
	switch(appCurrDispModeGet()) 
	{
		case APP_DISP_TV_NTSC:
			pfw=400;
			pfh=240;
			break;
		case APP_DISP_TV_PAL:
			pfw=400;
			pfh=288;
			break;
		case APP_DISP_LCD:
			
			break;	
		default:		
			break;
	}
		
	h = (pfh - pfh * (4 * imgh) / (3 * imgw) ) / 2;
	h = h>0? h+1 : h; //only compensate the differ from capture.

	sp5kGfxAttrSet(SP5K_GFX_REFRESH_ACTIVE, 0, 0, 0, 0);
	if (h0>h) 
	{
		// clean some area
		appOsdLib_PFLayerShapeClear(APP_OSD_SHAPE_RECT, 0, h, pfw, h0, 0, 0);
		appOsdLib_PFLayerShapeClear(APP_OSD_SHAPE_RECT, 0, pfh-h0, pfw, pfh-h, 0, 0);
	}
	else if (h != 0) 
	{
		// draw area
		appOsd_ColorDrawShapeColorSet(PAL_BLACK, PAL_BLEND_100, PAL_BLACK, PAL_BLEND_100);
		appOsdLib_PFLayerShapeDraw(APP_OSD_SHAPE_RECT, 0, 0, 	 pfw, h, 0, 0);
		appOsdLib_PFLayerShapeDraw(APP_OSD_SHAPE_RECT, 0, pfh-h, pfw, pfh, 0, 0);
	}	
	sp5kGfxAttrSet(SP5K_GFX_REFRESH_ACTIVE, 1, 0, 0, 0);		
	h0 = h;
	
	#endif
	/*e Modify by Aries 10/06/22*/
}
void
appViewDrawPhotoFrame(
	UINT32 idx
)
{
	UINT8 buf[sizeof(PF_PATH)+20];	

	#if KIT_WITH_HDMI
	if(IS_HDMI_IN){
		/* HDMI mode,not support photoframe show in preview */
		return;
	}	
	#endif
	
	sp5kGfxPageClear(SP5K_GFX_PAGE_PF_0, PAL_TRANSPARENCY);
	if(idx != UI_PHOTOFRAME_OFF) 
	{
		sprintf(buf, PF_PATH "%s", photoFrameTable[idx].pf_small);
		appJpgDraw_PfLayer(buf,0, 0, 320, 240);
	}	
}

void
appViewOsdDiskShow(
	void
)
{
	UINT32 n, dskFree;
	
	if(appStillBackgroundGetFree(&dskFree)==SUCCESS && gStillCB.isDiskReady) 
	{
		/* 
		This will ready at last save progress message received. In this moment
		we will also sync the diskSpaceInImgId will dskFree.
		*/
		gStillCB.diskSpaceInImgId = dskFree;
		if(gStillCB.estFileSize>0) 
		{
			n = dskFree/gStillCB.estFileSize;
			appViewOsd_RemainNumDraw(1, 0, n);
		}
		else 
		{
			DBG_PRINT("[error] est file size =0\n");
		}	
	} 	
	else if(gStillCB.isDiskReady) 
	{		
		/* 
		This is for fast update the disk space in image number. The diskSpaceInImgId
		will update in every raw image id ready message. 		
		*/
		if(gStillCB.estFileSize>0) 
		{
			n = gStillCB.diskSpaceInImgId/gStillCB.estFileSize;
			appViewOsd_RemainNumDraw(1, 0, n);
		} 
		else 
		{
			DBG_PRINT("[error] est file size =0\n");	
		}			
	}	
	else 
	{
		DBG_PRINT("can not get disk info for OSD\n");
	}	
}

/*-------------------------------------------------------------------------
 *  File Name : appViewOsd_PicSizeDraw
 *------------------------------------------------------------------------*/
void 
appViewOsd_PicSizeDraw(
	UINT8 bShow, 
	UINT8 nPicSize
)
{
	static UINT8 lastRecord=0;
	UINT32 id;
	UINT16 x,y;
	static UINT32 preId=0x1000;  //invalid ID,add for OSD show in HDMI mode
	#if SP5K_SENSOR_OV2715	//we can remove this limitation now
	//uiPara_t* puiPara = appUiParaGet();
	
	//puiPara->ImageSize = UI_PIC_SIZE_2M ;
	#endif
	
	if (bShow)
	{				
		//check
		if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_SIZE && lastRecord==nPicSize) 
		{
			return;
		}	
		if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_SIZE && lastRecord!=nPicSize) 
		{		
			appViewOsd_PicSizeDraw(FALSE, 0);
		}	
		//record
		ViewOsdPresentStatus |= VIEW_OSD_PRESENT_SIZE;
		lastRecord = nPicSize;
		
		//draw
		appOsd_FontIconFontSet(ID_FONT_ICON);
		id = PicSizeIcon[nPicSize<UI_PIC_SIZE_MAX ? nPicSize : UI_PIC_SIZE_FULL];	
		//appOsdLib_OsdLayIconDraw(APPVIEW_PICSIZE_X,APPVIEW_PICSIZE_Y,SP5K_GFX_ALIGN_TOP_LEFT,id);
		appIconShow(preId,ICON_CLEAN,SP5K_GFX_ALIGN_TOP_LEFT);
		appIconShow(id,ICON_SHOW,SP5K_GFX_ALIGN_TOP_LEFT);
		preId = id;
	}
	else if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_SIZE) 
	{
		ViewOsdPresentStatus &= ~VIEW_OSD_PRESENT_SIZE;
		appOsdIconPosGet(ID_ICON_RESOLUTION_5M,&x,&y);
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT,x, y,APPVIEW_ICON_SIZE_W,APPVIEW_ICON_SIZE_H,0,0);		
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appViewOsd_FlashModeDraw
 *------------------------------------------------------------------------*/
void 
appViewOsd_FlashModeDraw(
	UINT8 bShow, 
	UINT8 nFlashMode
)
{
	static UINT8 lastRecord;
	UINT16 x,y;
	
	if (bShow)
	{
		//check
		if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_FLASH&& lastRecord==nFlashMode) 
		{
			return;
		}	
		if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_FLASH&& lastRecord!=nFlashMode) 
		{		
			appViewOsd_FlashModeDraw(FALSE, 0);
		}	
		//record
		ViewOsdPresentStatus |= VIEW_OSD_PRESENT_FLASH;
		lastRecord = nFlashMode;
		appOsd_FontIconFontSet(ID_FONT_ICON);
		switch(nFlashMode) 
		{
			case UI_FLASH_AUTO:
				//appOsdLib_OsdLayIconDraw(APPVIEW_FLASH_X,APPVIEW_FLASH_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_FLASH_AUTO);
				appIconShow(ID_ICON_FLASH_AUTO,ICON_SHOW,SP5K_GFX_ALIGN_TOP_LEFT);
				break;
			case UI_FLASH_OFF:
				//appOsdLib_OsdLayIconDraw(APPVIEW_FLASH_X,APPVIEW_FLASH_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_FLASH_OFF);
				appIconShow(ID_ICON_FLASH_OFF,ICON_SHOW,SP5K_GFX_ALIGN_TOP_LEFT);
				break;
			case UI_FLASH_ON:
				//appOsdLib_OsdLayIconDraw(APPVIEW_FLASH_X,APPVIEW_FLASH_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_FLASH_ON);
				appIconShow(ID_ICON_FLASH_ON,ICON_SHOW,SP5K_GFX_ALIGN_TOP_LEFT);
				break;
			case UI_FLASH_RED_EYE:
				//appOsdLib_OsdLayIconDraw(APPVIEW_FLASH_X,APPVIEW_FLASH_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_FLASH_REDEYE_ON);
				appIconShow(ID_ICON_FLASH_REDEYE_ON,ICON_SHOW,SP5K_GFX_ALIGN_TOP_LEFT);
				break;
			case UI_FLASH_SLOW_SYNC:
				//appOsdLib_OsdLayIconDraw(APPVIEW_FLASH_X,APPVIEW_FLASH_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_FLASH_SLOW_SYNC);
				appIconShow(ID_ICON_FLASH_SLOW_SYNC,ICON_SHOW,SP5K_GFX_ALIGN_TOP_LEFT);
				break;
			default:
				break;
		}		
	}
	else if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_FLASH) 
	{
		ViewOsdPresentStatus &= ~VIEW_OSD_PRESENT_FLASH;
		appOsdIconPosGet(ID_ICON_FLASH_SLOW_SYNC,&x,&y);
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT,x, y,APPVIEW_ICON_SIZE_W,APPVIEW_ICON_SIZE_H,0,0);
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appViewOsd_WBModeDraw
 *------------------------------------------------------------------------*/
void 
appViewOsd_WBModeDraw(
	UINT8 bShow,
	UINT8 nWBMode
)
{
	static UINT8 lastRecord;
	UINT16 x,y;

	if (bShow)
	{
		//check
		if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_WB&& lastRecord==nWBMode)
		{
			return;
		}	
		if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_WB&& lastRecord!=nWBMode)
		{		
			appViewOsd_WBModeDraw(FALSE, 0);
		}	
		//record
		ViewOsdPresentStatus |= VIEW_OSD_PRESENT_WB;
		lastRecord = nWBMode;

		appOsd_FontIconFontSet(ID_FONT_ICON);
		switch(nWBMode) 
		{
			case UI_WB_AUTO:
				//appOsdLib_OsdLayIconDraw(APPVIEW_WB_X,APPVIEW_WB_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_WB_AUTO);
				appIconShow(ID_ICON_WB_AUTO,ICON_SHOW,SP5K_GFX_ALIGN_TOP_LEFT);
				break;
			case UI_WB_CLOUDY:
				//appOsdLib_OsdLayIconDraw(APPVIEW_WB_X,APPVIEW_WB_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_WB_CLOUDY);
				appIconShow(ID_ICON_WB_CLOUDY,ICON_SHOW,SP5K_GFX_ALIGN_TOP_LEFT);
				break;
			case UI_WB_FLUORESCENT_H:
				//appOsdLib_OsdLayIconDraw(APPVIEW_WB_X,APPVIEW_WB_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_WB_FLUORESCENT_H);
				appIconShow(ID_ICON_WB_FLUORESCENT_H,ICON_SHOW,SP5K_GFX_ALIGN_TOP_LEFT);
				break;
			case UI_WB_FLUORESCENT_L:
				//appOsdLib_OsdLayIconDraw(APPVIEW_WB_X,APPVIEW_WB_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_WB_FLUORESCENT_L);
				appIconShow(ID_ICON_WB_FLUORESCENT_L,ICON_SHOW,SP5K_GFX_ALIGN_TOP_LEFT);
				break;
			case UI_WB_FLUORESCENT:
				//appOsdLib_OsdLayIconDraw(APPVIEW_WB_X,APPVIEW_WB_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_WB_FLUORESCENT_L);
				appIconShow(ID_ICON_WB_FLUORESCENT_L,ICON_SHOW,SP5K_GFX_ALIGN_TOP_LEFT);
				break;
			case UI_WB_TUNGSTEN:
				//appOsdLib_OsdLayIconDraw(APPVIEW_WB_X,APPVIEW_WB_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_WB_TUNGSTEN);
				appIconShow(ID_ICON_WB_TUNGSTEN,ICON_SHOW,SP5K_GFX_ALIGN_TOP_LEFT);
				break;
			case UI_WB_DAYLIGHT:
				//appOsdLib_OsdLayIconDraw(APPVIEW_WB_X,APPVIEW_WB_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_WB_DAYLIGHT);
				appIconShow(ID_ICON_WB_DAYLIGHT,ICON_SHOW,SP5K_GFX_ALIGN_TOP_LEFT);
				break;
			case UI_WB_CUSTOM:
				//appOsdLib_OsdLayIconDraw(APPVIEW_WB_X,APPVIEW_WB_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_WB_CUSTOM);
				appIconShow(ID_ICON_WB_CUSTOM,ICON_SHOW,SP5K_GFX_ALIGN_TOP_LEFT);
				break;
			default:
				DBG_PRINT("_unkonw type: %d", nWBMode);
				break;
		}	
	}
	else if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_WB) 
	{
		ViewOsdPresentStatus &= ~VIEW_OSD_PRESENT_WB;
		appOsdIconPosGet(ID_ICON_WB_CUSTOM,&x,&y);
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT,x, y,APPVIEW_ICON_SIZE_W,APPVIEW_ICON_SIZE_H,0,0);
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appViewOsd_FocusModeDraw
 *------------------------------------------------------------------------*/
void 
appViewOsd_FocusModeDraw(
	UINT8 bShow,
	UINT8 nFocusMode
)
{
	static UINT8 lastRecord;
	UINT16 x,y;
	
	if (bShow)
	{
		//check
		if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_AF&& lastRecord==nFocusMode) 
		{
			return;
		}	
		if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_AF&& lastRecord!=nFocusMode)
		{		
			appViewOsd_FocusModeDraw(FALSE, 0);
		}	
		//record
		ViewOsdPresentStatus |= VIEW_OSD_PRESENT_AF;
		lastRecord = nFocusMode;
		
		appOsd_FontIconFontSet(ID_FONT_ICON);
		switch(nFocusMode) 
		{
			case UI_FOCUS_NORMAL:
				//appOsdLib_OsdLayIconDraw(APPVIEW_FOCUSMODE_X,APPVIEW_FOCUSMODE_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_AUTO_FOCUS);
				appIconShow(ID_ICON_AUTO_FOCUS,ICON_SHOW,SP5K_GFX_ALIGN_TOP_LEFT);
				break;
			case UI_FOCUS_MACRO:
				//appOsdLib_OsdLayIconDraw(APPVIEW_FOCUSMODE_X,APPVIEW_FOCUSMODE_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_SCENE_MICROSPUR);
				appIconShow(ID_ICON_SCENE_MICROSPUR,ICON_SHOW,SP5K_GFX_ALIGN_TOP_LEFT);
				break;
			case UI_FOCUS_INFINITY:
				//appOsdLib_OsdLayIconDraw(APPVIEW_FOCUSMODE_X,APPVIEW_FOCUSMODE_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_INFINITY_FOCUS);
				appIconShow(ID_ICON_INFINITY_FOCUS,ICON_SHOW,SP5K_GFX_ALIGN_TOP_LEFT);
				break;
			default:
				break;
		}	
	}
	else if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_AF) 
	{
		ViewOsdPresentStatus &= ~VIEW_OSD_PRESENT_AF;
		appOsdIconPosGet(ID_ICON_INFINITY_FOCUS,&x,&y);
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT,x, y,APPVIEW_ICON_SIZE_W,APPVIEW_ICON_SIZE_H,0,0);
	}
}


/*-------------------------------------------------------------------------
 *  File Name : appViewOsd_ShakeWarnDraw
 *------------------------------------------------------------------------*/
void 
appViewOsd_ShakeWarnDraw(
	UINT8 bShow
)
{	
	UINT16 x,y;
	
	if (bShow) 
	{
		//check
		if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_HAND) 
		{
			return;
		}	
		ViewOsdPresentStatus |= VIEW_OSD_PRESENT_HAND;		
		
		appOsd_FontIconFontSet(ID_FONT_ICON);
		appOsdLib_OsdLayIconDraw(APPVIEW_SHAKE_X,APPVIEW_SHAKE_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_SCENE_SHAKEFREE_OFF);
		appIconShow(ID_ICON_SCENE_SHAKEFREE_OFF,ICON_SHOW,SP5K_GFX_ALIGN_TOP_LEFT);
	}	
	else if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_HAND) 
	{
		ViewOsdPresentStatus &= ~VIEW_OSD_PRESENT_HAND;	
		appOsdIconPosGet(ID_ICON_SCENE_SHAKEFREE_OFF,&x,&y);
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT,x, y,APPVIEW_ICON_SIZE_W,APPVIEW_ICON_SIZE_H,0,0);
	} 	
}



/*-------------------------------------------------------------------------
 *  File Name : appViewOsd_ISOModeDraw
 *------------------------------------------------------------------------*/
void 
appViewOsd_ISOModeDraw(
	UINT8 bShow,
	UINT8 enISO
)
{
	static UINT8 lastRecord;
	UINT16 x,y;
	
	if (bShow)
	{
		//check
		if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_ISO&& lastRecord==enISO)
		{
			return;
		}	
		if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_ISO&& lastRecord!=enISO)
		{		
			appViewOsd_ISOModeDraw(FALSE, 0);
		}	
		//record
		ViewOsdPresentStatus |= VIEW_OSD_PRESENT_ISO;
		lastRecord = enISO;
		
		appOsd_FontIconFontSet(ID_FONT_ICON);	
		switch(enISO) 
		{
			case UI_ISO_AUTO:
				//appOsdLib_OsdLayIconDraw(APPVIEW_ISO_X,APPVIEW_ISO_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_ISO_AUTO);
				appIconShow(ID_ICON_ISO_AUTO,ICON_SHOW,SP5K_GFX_ALIGN_TOP_LEFT);
				break;
			case UI_ISO_50:
				//appOsdLib_OsdLayIconDraw(APPVIEW_ISO_X,APPVIEW_ISO_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_ISO_50);
				appIconShow(ID_ICON_ISO_50,ICON_SHOW,SP5K_GFX_ALIGN_TOP_LEFT);
				break;
			case UI_ISO_100:
				//appOsdLib_OsdLayIconDraw(APPVIEW_ISO_X,APPVIEW_ISO_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_ISO_100);
				appIconShow(ID_ICON_ISO_100,ICON_SHOW,SP5K_GFX_ALIGN_TOP_LEFT);
				break;
			case UI_ISO_200:
				//appOsdLib_OsdLayIconDraw(APPVIEW_ISO_X,APPVIEW_ISO_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_ISO_200);
				appIconShow(ID_ICON_ISO_200,ICON_SHOW,SP5K_GFX_ALIGN_TOP_LEFT);
				break;
			case UI_ISO_400:
				//appOsdLib_OsdLayIconDraw(APPVIEW_ISO_X,APPVIEW_ISO_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_ISO_400);
				appIconShow(ID_ICON_ISO_400,ICON_SHOW,SP5K_GFX_ALIGN_TOP_LEFT);
				break;
			case UI_ISO_800:
				//appOsdLib_OsdLayIconDraw(APPVIEW_ISO_X,APPVIEW_ISO_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_ISO_800);
				appIconShow(ID_ICON_ISO_800,ICON_SHOW,SP5K_GFX_ALIGN_TOP_LEFT);
				break;
			case UI_ISO_1600:
				//appOsdLib_OsdLayIconDraw(APPVIEW_ISO_X,APPVIEW_ISO_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_ISO_1600);
				appIconShow(ID_ICON_ISO_1600,ICON_SHOW,SP5K_GFX_ALIGN_TOP_LEFT);
				break;
			default:
				break;
		}	
	}
	else if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_ISO) 
	{
		ViewOsdPresentStatus &= ~VIEW_OSD_PRESENT_ISO;
		appOsdIconPosGet(ID_ICON_ISO_AUTO,&x,&y);
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT,x, y,APPVIEW_ICON_SIZE_W,APPVIEW_ICON_SIZE_H,0,0);
	}
}



/*-------------------------------------------------------------------------
 *  File Name : appViewOsd_SelfTimerModeDraw
 *------------------------------------------------------------------------*/
void 
appViewOsd_SelfTimerModeDraw(
	UINT8 bShow, 
	UINT8 nSelfTimer
)
{
	static UINT8 lastRecord;
	UINT16 x,y;
	
	if (bShow) 
	{
		//check
		if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_DRIVE&& lastRecord==nSelfTimer)
		{
			return;
		}	
		if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_DRIVE&& lastRecord!=nSelfTimer)
		{		
			appViewOsd_SelfTimerModeDraw(FALSE, 0);
		}	
		//record
		ViewOsdPresentStatus |= VIEW_OSD_PRESENT_DRIVE;
		lastRecord = nSelfTimer;

		appOsd_FontIconFontSet(ID_FONT_ICON);
		switch(nSelfTimer) 
		{
			case UI_DRIVE_OFF:
				break;
			case UI_DRIVE_TIMER_2S:
				//appOsdLib_OsdLayIconDraw(APPVIEW_SELFTIMER_X,APPVIEW_SELFTIMER_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_SELFTIMER_2SEC);
				appIconShow(ID_ICON_SELFTIMER_2SEC,ICON_SHOW,SP5K_GFX_ALIGN_TOP_LEFT);
				break;
			case UI_DRIVE_TIMER_10S:
				//appOsdLib_OsdLayIconDraw(APPVIEW_SELFTIMER_X,APPVIEW_SELFTIMER_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_SELFTIMER_10SEC);
				appIconShow(ID_ICON_SELFTIMER_10SEC,ICON_SHOW,SP5K_GFX_ALIGN_TOP_LEFT);
				break;
			case UI_DRIVE_DOUBLE:
				//appOsdLib_OsdLayIconDraw(APPVIEW_SELFTIMER_X,APPVIEW_SELFTIMER_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_DOUBLE);
				appIconShow(ID_ICON_DOUBLE,ICON_SHOW,SP5K_GFX_ALIGN_TOP_LEFT);
				break;
			case UI_DRIVE_BURST:
				//appOsdLib_OsdLayIconDraw(APPVIEW_SELFTIMER_X,APPVIEW_SELFTIMER_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_BURST);
				appIconShow(ID_ICON_BURST,ICON_SHOW,SP5K_GFX_ALIGN_TOP_LEFT);
				break;
			default:
				break;
		}	
	}	
	else if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_DRIVE) 
	{
		ViewOsdPresentStatus &= ~VIEW_OSD_PRESENT_DRIVE;
		appOsdIconPosGet(ID_ICON_BURST,&x,&y);
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT,x, y,APPVIEW_ICON_SIZE_W,APPVIEW_ICON_SIZE_H,0,0);
	}
}
/*-------------------------------------------------------------------------
 *  Function Name : appViewOsd_ViewModeDraw
 *  Description :
 *------------------------------------------------------------------------*/
void 
appViewOsd_ViewModeDraw(
	UINT8 bShow, 
	UINT8 Pasm,
	UINT8 Scene
)
{
	static UINT8 lastRecord1, lastRecord2;
	UINT16 x,y;
	
	if(bShow)
	{
		//check
		if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_VIEWSCENE_MODE 
			&& lastRecord1==Pasm && lastRecord2==Scene) 
		{
			return;
		}	
		
		if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_VIEWSCENE_MODE
			&& (lastRecord1!=Pasm || lastRecord2!=Scene)) 
		{ 
			appViewOsd_ViewModeDraw(FALSE, 0, 0);
		}	
		//record
		ViewOsdPresentStatus |= VIEW_OSD_PRESENT_VIEWSCENE_MODE;
		lastRecord1 = Pasm;
		lastRecord2 = Scene;
		VIEW_OSD_PERF_LOG(0, "load font -s");
		appOsd_FontIconFontSet(ID_FONT_ICON);
		VIEW_OSD_PERF_LOG(0, "load font -e");	
		
		VIEW_OSD_PERF_LOG(0, "mode draw -s");		
		switch(Pasm) 
		{
			case SCENE_MODE:
				//appOsdLib_OsdLayIconDraw(APPVIEW_MODE_X, APPVIEW_MODE_Y, 			
				//	SP5K_GFX_ALIGN_TOP_LEFT, SceneIcon[Scene]);
				appIconShow(SceneIcon[Scene],ICON_SHOW,SP5K_GFX_ALIGN_TOP_LEFT);
				break;			
			case VIDEO_MODE:
				//appOsdLib_OsdLayIconDraw(APPVIEW_MODE_X, APPVIEW_MODE_Y, 			
				//	SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_MODE_VIDEO);
				appIconShow(ID_ICON_MODE_VIDEO,ICON_SHOW,SP5K_GFX_ALIGN_TOP_LEFT);
				break;	
			case AUDIO_MODE:
				//appOsdLib_OsdLayIconDraw(APPVIEW_MODE_X, APPVIEW_MODE_Y, 			
				//	SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_MODE_VOICE);
				appIconShow(ID_ICON_MODE_VOICE,ICON_SHOW,SP5K_GFX_ALIGN_TOP_LEFT);
			default:				
				//appOsdLib_OsdLayIconDraw(APPVIEW_MODE_X, APPVIEW_MODE_Y, 			
				//	SP5K_GFX_ALIGN_TOP_LEFT, PasmIcon[Pasm]);
				appIconShow(PasmIcon[Pasm],ICON_SHOW,SP5K_GFX_ALIGN_TOP_LEFT);
				break;
		}		
		VIEW_OSD_PERF_LOG(0, "mode draw -e");
	}
	else if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_VIEWSCENE_MODE)
	{
		ViewOsdPresentStatus &= ~VIEW_OSD_PRESENT_VIEWSCENE_MODE;
		appOsdIconPosGet(SceneIcon[Scene],&x,&y);
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT,x, y,APPVIEW_ICON_SIZE_W,APPVIEW_ICON_SIZE_H,0,0);
	}	
}

/*-------------------------------------------------------------------------
 *  Function Name : appViewOsd_MeteringDraw
 *  Description :
 *------------------------------------------------------------------------*/
void 
appViewOsd_MeteringDraw(
	UINT8 bShow, 
	UINT8 idx
)
{
	static UINT8 lastRecord;
	UINT16 x,y;

	if (bShow) 
	{	
		//check
		if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_METER && lastRecord==idx)
		{
			return;
		}	
		if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_METER && lastRecord!=idx)
		{ 
			appViewOsd_MeteringDraw(FALSE, 0);
		}	
		//record
		ViewOsdPresentStatus |= VIEW_OSD_PRESENT_METER;
		lastRecord = idx;

		appOsd_FontIconFontSet(ID_FONT_ICON);
		switch(idx)
		{
			case UI_METER_CENTER:
				//appOsdLib_OsdLayIconDraw(APPVIEW_METER_X,APPVIEW_METER_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_METERING_CENTER);
				appIconShow(ID_ICON_METERING_CENTER,ICON_SHOW,SP5K_GFX_ALIGN_TOP_LEFT);
				break;
			case UI_METER_MULTI:
				//appOsdLib_OsdLayIconDraw(APPVIEW_METER_X,APPVIEW_METER_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_METERING_MULTI);
				appIconShow(ID_ICON_METERING_MULTI,ICON_SHOW,SP5K_GFX_ALIGN_TOP_LEFT);
				break;
			case UI_METER_SPOT:
				//appOsdLib_OsdLayIconDraw(APPVIEW_METER_X,APPVIEW_METER_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_METERING_SPOT);
				appIconShow(ID_ICON_METERING_SPOT,ICON_SHOW,SP5K_GFX_ALIGN_TOP_LEFT);
				break;		
			default:
				break;
		}
	}	
	else if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_METER) 
	{
		ViewOsdPresentStatus &= ~VIEW_OSD_PRESENT_METER;
		appOsdIconPosGet(ID_ICON_METERING_CENTER,&x,&y);
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT,x, y,APPVIEW_ICON_SIZE_W,APPVIEW_ICON_SIZE_H,0,0);
	}	
}

/*-------------------------------------------------------------------------
 *  Function Name : appViewOsd_ExposureBiaseDraw
 *  Description :
 *------------------------------------------------------------------------*/
void 
appViewOsd_ExposureBiaseDraw(
	UINT8 bShow, 
	UINT8 idx
)
{
	static UINT8 lastRecord;
	UINT16 x,y;
	
	if (bShow) 
	{
		//check
		if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_EV && lastRecord==idx) 
		{
			return;
		}	
		if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_EV && lastRecord!=idx)
		{ 
			appViewOsd_ExposureBiaseDraw(FALSE, 0);
		}	
		//record
		ViewOsdPresentStatus |= VIEW_OSD_PRESENT_EV;
		lastRecord = idx;

		appOsd_FontIconFontSet(ID_FONT_ICON);	
		switch(idx)
		{
			case UI_EXP_BIAS_P200:
				//appOsdLib_OsdLayIconDraw(APPVIEW_EV_X,APPVIEW_EV_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_EV_P20);
				appIconShow(ID_ICON_EV_P20,ICON_SHOW,SP5K_GFX_ALIGN_TOP_LEFT);
				break;
			#if !CAM_TYPE_CVR
			case UI_EXP_BIAS_P166:
				//appOsdLib_OsdLayIconDraw(APPVIEW_EV_X,APPVIEW_EV_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_EV_P17);
				appIconShow(ID_ICON_EV_P17,ICON_SHOW,SP5K_GFX_ALIGN_TOP_LEFT);
				break;
			case UI_EXP_BIAS_P133:
				//appOsdLib_OsdLayIconDraw(APPVIEW_EV_X,APPVIEW_EV_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_EV_P13);
				appIconShow(ID_ICON_EV_P13,ICON_SHOW,SP5K_GFX_ALIGN_TOP_LEFT);
				break;
			#endif
			case UI_EXP_BIAS_P100:
				//appOsdLib_OsdLayIconDraw(APPVIEW_EV_X,APPVIEW_EV_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_EV_P10);
				appIconShow(ID_ICON_EV_P10,ICON_SHOW,SP5K_GFX_ALIGN_TOP_LEFT);
				break;
			#if !CAM_TYPE_CVR
			case UI_EXP_BIAS_P066:
				//appOsdLib_OsdLayIconDraw(APPVIEW_EV_X,APPVIEW_EV_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_EV_P07);
				appIconShow(ID_ICON_EV_P07,ICON_SHOW,SP5K_GFX_ALIGN_TOP_LEFT);
				break;
			case UI_EXP_BIAS_P033:
				//appOsdLib_OsdLayIconDraw(APPVIEW_EV_X,APPVIEW_EV_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_EV_P03);
				appIconShow(ID_ICON_EV_P03,ICON_SHOW,SP5K_GFX_ALIGN_TOP_LEFT);
				break;
			#endif
			case UI_EXP_BIAS_ZERO:
				//appOsdLib_OsdLayIconDraw(APPVIEW_EV_X,APPVIEW_EV_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_EV_00);
				appIconShow(ID_ICON_EV_00,ICON_SHOW,SP5K_GFX_ALIGN_TOP_LEFT);
				break;
			#if !CAM_TYPE_CVR
			case UI_EXP_BIAS_N033:
				//appOsdLib_OsdLayIconDraw(APPVIEW_EV_X,APPVIEW_EV_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_EV_N03);
				appIconShow(ID_ICON_EV_N03,ICON_SHOW,SP5K_GFX_ALIGN_TOP_LEFT);
				break;
			case UI_EXP_BIAS_N066:
				//appOsdLib_OsdLayIconDraw(APPVIEW_EV_X,APPVIEW_EV_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_EV_N07);
				appIconShow(ID_ICON_EV_N07,ICON_SHOW,SP5K_GFX_ALIGN_TOP_LEFT);
				break;
			#endif
			case UI_EXP_BIAS_N100:
				//appOsdLib_OsdLayIconDraw(APPVIEW_EV_X,APPVIEW_EV_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_EV_N10);
				appIconShow(ID_ICON_EV_N10,ICON_SHOW,SP5K_GFX_ALIGN_TOP_LEFT);
				break;
			#if !CAM_TYPE_CVR
			case UI_EXP_BIAS_N133:
				//appOsdLib_OsdLayIconDraw(APPVIEW_EV_X,APPVIEW_EV_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_EV_N13);
				appIconShow(ID_ICON_EV_N13,ICON_SHOW,SP5K_GFX_ALIGN_TOP_LEFT);
				break;
			case UI_EXP_BIAS_N166:
				//appOsdLib_OsdLayIconDraw(APPVIEW_EV_X,APPVIEW_EV_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_EV_N17);
				appIconShow(ID_ICON_EV_N17,ICON_SHOW,SP5K_GFX_ALIGN_TOP_LEFT);
				break;
			#endif
			case UI_EXP_BIAS_N200:
				//appOsdLib_OsdLayIconDraw(APPVIEW_EV_X,APPVIEW_EV_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_EV_N20);
				appIconShow(ID_ICON_EV_N20,ICON_SHOW,SP5K_GFX_ALIGN_TOP_LEFT);
				break;
			default:
				break;
		}	
	}	
	else if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_EV)
	{
		ViewOsdPresentStatus &= ~VIEW_OSD_PRESENT_EV;
		appOsdIconPosGet(ID_ICON_EV_00,&x,&y);
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT,x, y,APPVIEW_ICON_SIZE_W,APPVIEW_ICON_SIZE_H,0,0);
	}	
}

/*-------------------------------------------------------------------------
 *  Function Name : appViewOsd_BatteryDraw
 *  Description :
 *------------------------------------------------------------------------*/
void 
appViewOsd_BatteryDraw(
	UINT8 bShow, 
	UINT8 idx
)
{
	static UINT8 lastRecord;
	UINT16 x,y;
	
	if (bShow) 
	{
		//check
		if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_BATTERY && lastRecord==idx)
		{
			return;
		}	
		if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_BATTERY && lastRecord!=idx)
		{ 
			appViewOsd_BatteryDraw(FALSE, 0);
		}	
		//record
		ViewOsdPresentStatus |= VIEW_OSD_PRESENT_BATTERY;
		lastRecord = idx;
		
		appOsd_FontIconFontSet(ID_FONT_ICON);
		switch(idx)
		{
			case APP_BATT_LEVEL_0:
				//appOsdLib_OsdLayIconDraw(APPVIEW_BATTERY_X,APPVIEW_BATTERY_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_BATTERY_LOW);
				appIconShow(ID_ICON_BATTERY_LOW,ICON_SHOW,SP5K_GFX_ALIGN_TOP_LEFT);
				break;
			case APP_BATT_LEVEL_1:
				//appOsdLib_OsdLayIconDraw(APPVIEW_BATTERY_X,APPVIEW_BATTERY_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_BATTERY_HALF);
				appIconShow(ID_ICON_BATTERY_HALF,ICON_SHOW,SP5K_GFX_ALIGN_TOP_LEFT);
				break;
			case APP_BATT_LEVEL_2:
				//appOsdLib_OsdLayIconDraw(APPVIEW_BATTERY_X,APPVIEW_BATTERY_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_BATTERY_FULL);
				appIconShow(ID_ICON_BATTERY_FULL,ICON_SHOW,SP5K_GFX_ALIGN_TOP_LEFT);
				break;
			case APP_BATT_LEVEL_3:
				//appOsdLib_OsdLayIconDraw(APPVIEW_BATTERY_X,APPVIEW_BATTERY_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_BATTERY_FULL);
				///appIconShow(ID_ICON_BATTERY_FULL,ICON_SHOW,SP5K_GFX_ALIGN_TOP_LEFT);
				printf("[WARN] APP_BATT_LEVEL_3 is not used. %s (%d)\n",__FUNCTION__,__LINE__);
				break;
			default:
				break;
		}	
	}
	else if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_BATTERY) 
	{
		ViewOsdPresentStatus &= ~VIEW_OSD_PRESENT_BATTERY;
		appOsdIconPosGet(ID_ICON_BATTERY_FULL,&x,&y);
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT,x, y,APPVIEW_ICON_SIZE_W,APPVIEW_ICON_SIZE_H,0,0);	
	}	
}

/*-------------------------------------------------------------------------
 *  Function Name : appViewOsd_HistogramDraw
 *  Description :
 *------------------------------------------------------------------------*/
void 
appViewOsd_HistogramDraw(
	BOOL showType, 
	UINT32 *pLuma	
)
{
	UINT32 *pBar, tempMax;
	int bar_x, bar_y, bar_w, bar_h, bar_num, sample_interval;
	int i, j;
	UINT32 x,y;
	UINT32 histogramSizeX=APPVIEW_HISTOGRAM_W,histogramSizeY=APPVIEW_HISTOGRAM_H;

#if KIT_WITH_HDMI
	if(IS_HDMI_IN){
		x = APPVIEW_HISTOGRAM_X*2;
		y = APPVIEW_HISTOGRAM_Y*3/2-15;
		histogramSizeX *=2;
		histogramSizeY =histogramSizeY*3/2;
	}else{
#else
	{
#endif
		x = APPVIEW_HISTOGRAM_X;
		y = APPVIEW_HISTOGRAM_Y;
	}
	
	if(showType == OSD_OBJ_DESTROYED) 
	{		
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, x, 
			y,histogramSizeX,histogramSizeY,0,0);
		appOsdLib_TransparentLayerShapeClear(APP_OSD_SHAPE_RECT, x, 
			y,histogramSizeX,histogramSizeY,0,0);
	}
	else 
	{					
		if(!pLuma) 
		{
			return;
		}	
		
		bar_w = 1;
		bar_num = histogramSizeX;
		sample_interval = 256/histogramSizeX;
		pBar = pLuma;
		tempMax = 0;
		
		/* down sample */
		for(i=0, j=0; i<256; i+=sample_interval, j++) 
		{		
			pBar[j] = pLuma[i]; 
			if(pBar[j]>tempMax) 
			{
				tempMax = pBar[j];
			}			
		}
		
		/* equlization */
		if(tempMax >0) 
		{
			for(i=0; i<bar_num; i++) 
			{		
				pBar[i] = (pBar[i]*histogramSizeY)/tempMax;
			}
		}	
		else
		{
			printf("[warn]%s: tempMax is %d\n",__FUNCTION__, tempMax);
		}

		sp5kGfxAttrSet(SP5K_GFX_REFRESH_ACTIVE, 0, 0, 0, 0);
		/* draw the gray backgroud */
		if(showType != OSD_OBJ_RUNNING)
		{
			
			appOsd_ColorDrawShapeColorSet(PAL_GRAY_1, PAL_BLEND_50, PAL_GRAY_1, PAL_BLEND_50);
			appOsd_ColorTransparentShapeColorSet(PAL_GRAY_1, PAL_GRAY_1, PAL_BLEND_50);
			appOsdLib_TransparentLayerShapeDraw(APP_OSD_SHAPE_RECT,	x, 
				y,histogramSizeX,histogramSizeY,0,0);
		}	
		
		/* clean the bar chart */
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, x, 
			y,histogramSizeX,histogramSizeY,0,0);

		/* draw the new bar chart */
		appOsd_ColorDrawShapeColorSet(PAL_WHITE, PAL_BLEND_100, PAL_WHITE, PAL_BLEND_100);
		for(i = 0, bar_x = x; i < bar_num; i++, bar_x += bar_w)
		{
			if(pBar[i]>histogramSizeY)
			{				
				printf("[warn]%s: pBar[i]> his_h\n",__FUNCTION__);
				bar_h =histogramSizeY;
			}	
			else
			{
				bar_h = pBar[i];
			}	
				
			if (bar_h !=0)
			{
				bar_y = y + histogramSizeY - bar_h;
				appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, bar_x, bar_y, bar_w, bar_h, 0, 0);
			}
		}		
		sp5kGfxAttrSet(SP5K_GFX_REFRESH_ACTIVE, 1, 0, 0, 0);
	}
}

/*-------------------------------------------------------------------------
 *  Function Name : appViewOsd_RealFnumberDraw
 *  Description :
 *------------------------------------------------------------------------*/
void 
appViewOsd_RealFnumberDraw(
	UINT8 bShow, 
	UINT8* str
)
{	
	DBG_PRINT("%s\n", __FUNCTION__);
	if (bShow)
	{
		ViewOsdPresentStatus |= VIEW_OSD_PRESENT_REAL_FNUM;
		appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_TextDraw(APPVIEW_REAL_AV_X, APPVIEW_REAL_AV_Y, SP5K_GFX_ALIGN_TOP_RIGHT, str);
	} 
	else if (ViewOsdPresentStatus&VIEW_OSD_PRESENT_REAL_FNUM)
	{
		ViewOsdPresentStatus &= ~VIEW_OSD_PRESENT_REAL_FNUM;
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT,APPVIEW_REAL_AV_X-(APPVIEW_ICON_SIZEX*2+APPVIEW_ICON_Y_GAP+6), APPVIEW_REAL_AV_Y, APPVIEW_ICON_SIZEX*2+APPVIEW_ICON_Y_GAP+6, APPVIEW_ICON_SIZEY, 0, 0);
	}	
}

/*-------------------------------------------------------------------------
 *  Function Name : appViewOsd_RealShutterSpeedDraw
 *  Description :
 *------------------------------------------------------------------------*/
void 
appViewOsd_RealShutterSpeedDraw(
	UINT8 bShow, 
	UINT8* str
)
{
	DBG_PRINT("%s\n", __FUNCTION__);

	if (bShow)
	{
		ViewOsdPresentStatus |= VIEW_OSD_PRESENT_REAL_SHUTTER;	
		appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_TextDraw(APPVIEW_REAL_TV_X, APPVIEW_REAL_TV_Y, SP5K_GFX_ALIGN_TOP_RIGHT, str);
	} 
	else if (ViewOsdPresentStatus&VIEW_OSD_PRESENT_REAL_SHUTTER)
	{
		ViewOsdPresentStatus &= ~VIEW_OSD_PRESENT_REAL_SHUTTER;	
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT,APPVIEW_REAL_TV_X-(APPVIEW_ICON_SIZEX*2+APPVIEW_ICON_Y_GAP+6), APPVIEW_REAL_TV_Y, APPVIEW_ICON_SIZEX*2+APPVIEW_ICON_Y_GAP+6, APPVIEW_ICON_SIZEY, 0, 0);
	} 	
}

/*-------------------------------------------------------------------------
 *  Function Name : appViewOsd_FocusWindowDraw
 *  Description :
 *------------------------------------------------------------------------*/
void 
appViewOsd_FocusWindowDraw(
	UINT8 bShow, 
	UINT8 nFocusStatus
)
{
	static UINT8 lastRecord;
	return;
	if (bShow)
	{	
		//check
		if(faceOsd.totalFaceNum || objectOsd.totalObjectNum)		/* Modify by sc.liu */
		{
			return;
		}				
		if(IS_PASM_OR_SCENE_MODE && gStillCB.err > VIEW_ERR_NO_ERROR)
		{
			return;
		}
		if(IS_VIDEO_MODE && gVideoCB.err > VIEW_ERR_NO_ERROR)
		{
			return;
		}
		if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_FOCUS_WINDOW 
			&& lastRecord==nFocusStatus) 
		{
			return;
		}	
		if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_FOCUS_WINDOW 
			&& lastRecord!=nFocusStatus) 
		{ 
			appViewOsd_FocusWindowDraw(FALSE, 0);
		}	
		//record
		ViewOsdPresentStatus |= VIEW_OSD_PRESENT_FOCUS_WINDOW;
		lastRecord = nFocusStatus;
		
		VIEW_OSD_PERF_LOG(0, "load font -s");
		appOsd_FontIconFontSet(ID_ICON_FOCUS);
		VIEW_OSD_PERF_LOG(0, "load font -e");
			
		VIEW_OSD_PERF_LOG(0, "win draw -s");	
		appOsdLib_OsdLayIconDraw(APPVIEW_FOCUS_X, APPVIEW_FOCUS_Y, SP5K_GFX_ALIGN_TOP_LEFT, nFocusStatus);	
		VIEW_OSD_PERF_LOG(0, "win draw -e");	
		//appOsd_FontIconFontSet(ID_FONT_ICON);
	}
	else if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_FOCUS_WINDOW) 
	{
		ViewOsdPresentStatus &= ~VIEW_OSD_PRESENT_FOCUS_WINDOW;	
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT,APPVIEW_FOCUS_X, APPVIEW_FOCUS_Y, APPVIEW_FOCUS_W, APPVIEW_FOCUS_H,0,0);
		return ;
	}	
}

/*-------------------------------------------------------------------------
 *  Function Name : appViewOsd_AmodeAvDraw
 *  Description :	In PASM-A mode (aperture piority), to draw the aperture
 *					value(F number).
 *------------------------------------------------------------------------*/
void 
appViewOsd_AmodeAvDraw(
	BOOL bShow, 
	UINT8 Select,
	UINT8 Index,
	UINT32 ZoomPoint
)
{
	static UINT8 lastRecord1, lastRecord2, lastRecord3;

	SINT32 x=235;
	SINT32 y=240-30-48-4-6;
	SINT32 w=100;
	SINT32 h=24;
	UINT32 checkZoomPoint, checkIrisIdx;
	
	if (bShow) 
	{		
		//check
		if(gStillCB.err > VIEW_ERR_NO_ERROR )
		{
			return;
		}
		if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_AMODE_FNUM 
			&& lastRecord1==Select && lastRecord2==Index && lastRecord3==ZoomPoint)
		{
				return;			
		}			
		//record
		ViewOsdPresentStatus |= VIEW_OSD_PRESENT_AMODE_FNUM;
		lastRecord1=Select;
		lastRecord2=Index;
		lastRecord3=ZoomPoint;
		
		if (Select ==TRUE)
		{
			appOsd_ColorDrawShapeColorSet(PAL_BLACK, PAL_BLEND_100, PAL_BLACK, PAL_BLEND_100);
			appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT,x, y, w, h, 0, 0);				
			appOsd_ColorDrawTextColorSet(PAL_YELLOW, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);		
			
			appOsd_FontIconFontSet(ID_FONT_ICON);			
			appOsdLib_OsdLayIconDraw(x,y+h+12,SP5K_GFX_ALIGN_BOTTOM_LEFT,ID_ICON_UPDOWN);
		}
		else
		{
			appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
			appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT,x, y, w, h, 0, 0);	
			// for guid line overlay
			if(gStillCB.osd==OSD_VIEW_GRID)
			{
				appOsd_ColorDrawShapeColorSet(PAL_WHITE, PAL_BLEND_100, PAL_WHITE, PAL_BLEND_100);
				appOsdLib_OsdLayerLineDraw(0, LCD_SIZE_Y-LCD_SIZE_Y/3, LCD_SIZE_X, LCD_SIZE_Y-LCD_SIZE_Y/3);
			}
			appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);		
		}
		checkIrisIdx = (Index==APERTURE_2_8)? 0: 1;
		checkZoomPoint= (ZoomPoint>6)? 6: ZoomPoint;
		appOsdLib_TextDraw(LCD_SIZE_X, y+h, SP5K_GFX_ALIGN_BOTTOM_RIGHT, aperturevValueStr[checkZoomPoint][checkIrisIdx]);	
	} 
	else if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_AMODE_FNUM) 
	{
		ViewOsdPresentStatus &= ~VIEW_OSD_PRESENT_AMODE_FNUM;	
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT,x, y, w, h, 0, 0);
		// for guid line overlay
		if(gStillCB.osd==OSD_VIEW_GRID)
		{
			appOsd_ColorDrawShapeColorSet(PAL_WHITE, PAL_BLEND_100, PAL_WHITE, PAL_BLEND_100);
			appOsdLib_OsdLayerLineDraw(0, LCD_SIZE_Y-LCD_SIZE_Y/3, LCD_SIZE_X, LCD_SIZE_Y-LCD_SIZE_Y/3);
		}
	} 
}


/*-------------------------------------------------------------------------
 *  Function Name : appViewOsd_SmodeTvDraw
 *  Description :	In PASM-S mode (shutter piority), to draw the shutter
 *					speed (T value).
 *------------------------------------------------------------------------*/
void 
appViewOsd_SmodeTvDraw(
	BOOL bShow, 
	UINT8 Select,
	UINT8 Index
)
{
	static UINT8 lastRecord1, lastRecord2;

	SINT32 x=235;
	SINT32 y=240-30-24-6-2;
	SINT32 w=100;
	SINT32 h=24;
	
	if (bShow) {
		//check
		if(gStillCB.err > VIEW_ERR_NO_ERROR )
		{
			return;
		}
		if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_SMODE_SHUTTER 
			&& lastRecord1==Select && lastRecord2==Index) 
		{
			return;
		}			
		//record
		ViewOsdPresentStatus |= VIEW_OSD_PRESENT_SMODE_SHUTTER;
		lastRecord1=Select;
		lastRecord2=Index;

		if (Select ==TRUE)
		{
			appOsd_ColorDrawShapeColorSet(PAL_BLACK, PAL_BLEND_100, PAL_BLACK, PAL_BLEND_100);
			appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT,x, y, w, h, 0, 0);				
			appOsd_ColorDrawTextColorSet(PAL_YELLOW, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);		
			
			appOsd_FontIconFontSet(ID_FONT_ICON);
			appOsdLib_OsdLayIconDraw(x,y+h+12,SP5K_GFX_ALIGN_BOTTOM_LEFT,ID_ICON_UPDOWN);
		}
		else
		{
			appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
			appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT,x, y, w, h, 0, 0);		
			appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);		
		}
		appOsdLib_TextDraw(LCD_SIZE_X, y+h, SP5K_GFX_ALIGN_BOTTOM_RIGHT, shutterSpeedValueStr[Index]);		
	}
	else if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_SMODE_SHUTTER) 
	{
		ViewOsdPresentStatus &= ~VIEW_OSD_PRESENT_SMODE_SHUTTER;	
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT,x, y, w, h, 0, 0);
	} 
}

/*-------------------------------------------------------------------------
 *  Function Name : appViewOsd_GuideLineDraw
 *  Description :	
 *------------------------------------------------------------------------*/
void 
appViewOsd_GuideLineDraw(
	BOOL bShow
)
{
	UINT32 int_x;
	UINT32 int_y;
	UINT32 hLine2_end_x, vLine_start_y, vLine_end_y, img_w, img_h, vLine_h;	
	uiPara_t* puiPara = appUiParaGet();
	sp5kGfxPageCapabDesc_t *pPageCap = NULL;

	if(!pPageCap){
		pPageCap = sp5kMalloc(sizeof(sp5kGfxPageCapabDesc_t));
	}
	if(!pPageCap){
		return;
	}
	sp5kGfxPageCapabilityGet(SP5K_GFX_PAGE_OSD_0,pPageCap);
	int_x = pPageCap->frmW/3;
	int_y = pPageCap->frmH/3;
	
	if (bShow)
	{
		if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_GUID_LINE)
		{
			sp5kFree(pPageCap);
			return;
		}			
		ViewOsdPresentStatus |= VIEW_OSD_PRESENT_GUID_LINE;		
		appOsd_ColorDrawShapeColorSet(PAL_WHITE, PAL_BLEND_100, PAL_WHITE, PAL_BLEND_100);		
	}	
	else 
	{		
		if(!(ViewOsdPresentStatus&VIEW_OSD_PRESENT_GUID_LINE))
		{
			sp5kFree(pPageCap);
			return;
		}
		ViewOsdPresentStatus &= ~VIEW_OSD_PRESENT_GUID_LINE;	
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
	}		

	if(IS_VIDEO_MODE)
	{
		appVideoSizeGet(puiPara->VideoSize, &img_w, &img_h);
	}	
	else
	{
		appStill_GetCurSize(&img_w, &img_h);
	}	
	
#if (KIT_WITH_HDMI)
	if(IS_HDMI_IN){
		vLine_h = pPageCap->frmH;
		vLine_start_y = 0;
	}else{
#else
	{
#endif
		vLine_h = pPageCap->frmW * img_h /img_w;
		vLine_start_y = (pPageCap->frmH - vLine_h)/2;
	}
	vLine_end_y = vLine_start_y + vLine_h;
	hLine2_end_x = (IS_PASM_AMODE && gStillCB.err <=VIEW_ERR_NO_ERROR)? (pPageCap->frmH-5) : pPageCap->frmW;	

#if (KIT_WITH_HDMI)
	if(IS_HDMI_IN && IS_VIDEO_MODE){
		appOsdLib_OsdLayerLineDraw(int_x,0,int_x,pPageCap->frmH);
		appOsdLib_OsdLayerLineDraw(pPageCap->frmW-int_x	,0,pPageCap->frmW-int_x	,pPageCap->frmH);
	}
	else{
#else
	{
#endif
		appOsdLib_OsdLayerLineDraw(int_x,vLine_start_y,int_x,vLine_end_y);
		appOsdLib_OsdLayerLineDraw(pPageCap->frmW-int_x	,vLine_start_y,pPageCap->frmW-int_x	,vLine_end_y);		
	}
	appOsdLib_OsdLayerLineDraw(0,int_y,pPageCap->frmW,int_y);		
	appOsdLib_OsdLayerLineDraw(0,pPageCap->frmH-int_y,hLine2_end_x,pPageCap->frmH-int_y);				

	sp5kFree(pPageCap);
}	

/*-------------------------------------------------------------------------
 *  Function Name : appViewOsd_WarmingMsgDraw
 *  Description :	
 *------------------------------------------------------------------------*/
void 
appViewOsd_WarmingMsgDraw(
	BOOL bShow,
	UINT32 viewError,
	BOOL bSound
)
{
	UINT32 needSound, osdtype;
	UINT32 err[VIEW_ERR_MAX]=
	{
		[VIEW_ERR_NULL] = ERR_MSG_NO_IMG,
		[VIEW_ERR_NO_ERROR] = ERR_MSG_NO_IMG,
		[VIEW_ERR_CARD_ERROR] = ERR_MSG_CARD_ERROR,
		[VIEW_ERR_CARD_LOCKED] = ERR_MSG_CARD_LOCKED,
		[VIEW_ERR_MEMORY_FULL] = ERR_MSG_MEMORY_FULL,
		[VIEW_ERR_MEMORY_NOT_ENOUGH] = ERR_MSG_MEMORY_NOT_ENOUGH,
		[VIEW_ERR_TO_POWER_OFF] = ERR_MSG_NO_IMG,
	};
	
	if(bShow) 
	{
		DBG_PRINT("viewError = %d", viewError);		
		if(err[viewError] == ERR_MSG_NO_IMG) 
		{
			return;
		}
		osdtype = IS_VIDEO_MODE? gVideoCB.osd : gStillCB.osd;
		if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_ERROR 
			&& osdtype != OSD_VIEW_GRID
			&& osdtype != OSD_VIEW_SIMPLE
			&& bSound == FALSE ) {
			return;
		}
		// 1st with sound (also can force on in 2nd)
		needSound = (ViewOsdPresentStatus&VIEW_OSD_PRESENT_ERROR)? ERR_MSG_ONLY: ERR_MSG_WITH_WARNING_SOUND;
		needSound = bSound? ERR_MSG_WITH_WARNING_SOUND: needSound;
		
		ViewOsdPresentStatus |= VIEW_OSD_PRESENT_ERROR;
		
		//clean 
		if(IS_PASM_AMODE||IS_PASM_SMODE||IS_PASM_MMODE)
		{
			appViewOsd_AmodeAvDraw(FALSE,0,0,0);
			appViewOsd_SmodeTvDraw(FALSE,0,0);
		}
		appViewOsd_FocusWindowDraw(FALSE,0);
		appViewOsd_HistogramDestroy();
		
		//draw			
		if(err[viewError] != ERR_MSG_NO_IMG)
		{
			if(appActiveStateGet() == APP_STATE_AUDIO_REC){
				#if KIT_WITH_HDMI
				if(IS_HDMI_IN){
					appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, 120, 90, 420, 196, 0, 0);
				}else{
				#else
				{
				#endif
					appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, 20, 40, 280, 163, 0, 0);
				}
			}
			appErrMsgWithSound(err[viewError] ,needSound , ERR_MSG_SHOW_FOREVER);	
		}			
		// if the message play with sound, we may want to flick the remain-time/remain-image number.
		// Here will issue the other message let the current status decide if need to goto still-busy state.
		if(viewError == VIEW_ERR_MEMORY_FULL 
			&& needSound == ERR_MSG_WITH_WARNING_SOUND) 
		{
			sp5kHostMsgSend(APP_UI_MSG_VIEW_MEMORY_FULL, 0);
		}
	}	
	else if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_ERROR)
	{
		ViewOsdPresentStatus &= ~VIEW_OSD_PRESENT_ERROR;	
		appErrMsgWithSound(ERR_MSG_NO_IMG, ERR_MSG_ONLY, ERR_MSG_SHOW_FOREVER);	
		
	}	
}

/*-------------------------------------------------------------------------
 *  Function Name : appViewOsd_PanoramaDraw
 *  Description :   
 *------------------------------------------------------------------------*/
void 
appViewOsd_PanoramaDraw(
	UINT8 bShow, 
	UINT8 idx
)
{
	static UINT8 lastRecord;
	
	lastRecord = idx;
	appOsd_ColorDrawShapeColorSet(PAL_BLACK, PAL_BLEND_75, PAL_BLACK, PAL_BLEND_75);
	appOsd_GfxRectShapeDraw(SP5K_GFX_PAGE_OSD_0,65, 50,254, 80);	
	appOsd_FontIconFontSet(ID_FONT_ICON);
	appOsdLib_OsdLayIconDraw(90,42,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_MEDIA_RIGHT_ARROW);
	appOsdLib_OsdLayIconDraw(130,42,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_MEDIA_LEFT_ARROW);
	appOsdLib_OsdLayIconDraw(170,42,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_MEDIA_DOWN_ARROW);
	appOsdLib_OsdLayIconDraw(210,42,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_MEDIA_UP_ARROW);
	appOsd_ColorIconColorSet(PAL_WHITE,PAL_RED);
	switch(idx)
	{
		case UI_PV_PANORAMA_LR:
			appOsdLib_OsdLayIconDraw(90,42,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_MEDIA_RIGHT_ARROW);
			break;
		case UI_PV_PANORAMA_RL:
			appOsdLib_OsdLayIconDraw(130,42,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_MEDIA_LEFT_ARROW);
			break;
		case UI_PV_PANORAMA_UD:
			appOsdLib_OsdLayIconDraw(170,42,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_MEDIA_DOWN_ARROW);
			break;
		case UI_PV_PANORAMA_DU:
			appOsdLib_OsdLayIconDraw(210,42,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_MEDIA_UP_ARROW);
			break;
		default:
			break;
	}
	appOsd_ColorIconColorSet(PAL_WHITE,PAL_WHITE);
}

/*-------------------------------------------------------------------------
 *  Function Name : appViewOsdShow
 *  Description :   This is the only function will be called in view state.
 *------------------------------------------------------------------------*/
void
appViewOsdShow(
	UINT8 clean,
	UINT8 outInOsdFreeze,
	UINT8 showType
)
{
	UINT32 current_osd_type;
	uiPara_t* puiPara = appUiParaGet();

	#if KIT_WITH_HDMI
	if(IS_HDMI_IN){
		appOsdIconModeSet(APP_ICON_MODE_STLLVIEW_HDMI);
	}else{
		appOsdIconModeSet(APP_ICON_MODE_STLLVIEW);
	}
	#else
	appOsdIconModeSet(APP_ICON_MODE_STLLVIEW);
	#endif

	appOsd_FontIconFontSet(ID_FONT_ICON);
	
	VIEW_PERF_LOG(0, "pv osdtype -s");

	APP_OSD_REFRESH_OFF;
	if(clean) 
	{
		appViewOsd_AllClean();
	}	
	appViewOsd_ZoomBarDestroy();
	switch(showType) 
	{
	case OSD_VIEW_VERBOSE:
		appViewOsd_ISOModeDraw(TRUE, puiPara->Iso);
		appViewOsd_MeteringDraw(TRUE, puiPara->AEMetering);
		appViewOsd_ExposureBiaseDraw(TRUE, puiPara->AEMode);
	default:	
	case OSD_VIEW_SIMPLE:
		appViewOsd_GuideLineDraw(FALSE);
		if(appActiveStateGet()!=APP_STATE_STILL_SMILE_DETECT) 
		{
			appViewOsd_FocusWindowDraw(TRUE, gStillCB.focusStatus);
		}	
		appViewOsd_ViewModeDraw(TRUE, gStillCB.cap.CurViewMode, gStillCB.cap.CurScene);
		appViewOsdDiskShow();
		appViewOsd_PicSizeDraw(TRUE, puiPara->ImageSize);
		appViewOsd_PicQtyDraw(TRUE, puiPara->ImageQuality);
		appViewOsd_FlashModeDraw(TRUE, gStillCB.flashMode);	
		appViewOsd_SelfTimerModeDraw(TRUE,puiPara->DriverMode);	
		appViewOsd_StabilizerModeDraw(puiPara->Stabilizer);
		appViewOsd_FDModeDraw(TRUE, puiPara->FaceTracking);
		appViewOsd_AebDraw(puiPara->AEB);
		appViewOsd_BatteryDraw(TRUE, appBatteryLevelGet());
		break;	
	case OSD_VIEW_NONE:	
		appViewOsd_FocusWindowDraw(TRUE, gStillCB.focusStatus);
		appViewOsd_ISOModeDraw(FALSE, puiPara->Iso);
		appViewOsd_WBModeDraw(FALSE, puiPara->WBMode);		
		appViewOsd_MeteringDraw(FALSE, puiPara->AEMetering);
		appViewOsd_ExposureBiaseDraw(FALSE, puiPara->AEMode);		
		appViewOsd_ViewModeDraw(FALSE, gStillCB.cap.CurViewMode, gStillCB.cap.CurScene);
		appViewOsd_RemainNumDraw(FALSE, 0, 0);
		appViewOsd_PicSizeDraw(FALSE, puiPara->ImageSize);
		appViewOsd_PicQtyDraw(FALSE, puiPara->ImageQuality);
		appViewOsd_FlashModeDraw(FALSE, gStillCB.flashMode);		
		appViewOsd_SelfTimerModeDraw(FALSE, puiPara->DriverMode);		
		appViewOsd_FocusModeDraw(FALSE, puiPara->af);
		appViewOsd_StabilizerModeDraw(FALSE);
		appViewOsd_FDModeDraw(FALSE, puiPara->FaceTracking);
		appViewOsd_AebDraw(FALSE);
		appViewOsd_BatteryDraw(FALSE, appBatteryLevelGet());
		appViewOsd_AllClean();
		break;
	case OSD_VIEW_GRID:
		appViewOsd_GuideLineDraw(TRUE);
		if(gStillCB.err <= VIEW_ERR_NO_ERROR )
		{
			appViewOsd_FocusWindowDraw(TRUE, gStillCB.focusStatus);
		}	
		break;	
	case OSD_VIEW_CLEAN_ALL:
		appViewOsd_AllClean();
		break;
	case OSD_VIEW_S_ONE_HOLD:
		current_osd_type = (IS_VIDEO_MODE)? gVideoCB.osd : gStillCB.osd;
		switch(current_osd_type) 
		{
			case OSD_VIEW_NONE:
			case OSD_VIEW_VERBOSE:
			case OSD_VIEW_SIMPLE:		
				appViewOsd_AllClean();
				break;
			case OSD_VIEW_GRID:
				appViewOsd_AllClean();
				appViewOsd_GuideLineDraw(TRUE);
				break;
		}	
		//draw info
		if(!IS_VIDEO_MODE) 
		{
			appViewOsd_ShakeWarnDraw(TRUE);
			if(gStillCB.lensZoomSeg>6 || (gStillCB.ae.iris-1)>1 || (gStillCB.ae.iris-1)<0) 
			{
				printf("warning: fnum-idx exceed rang(zseg=%d, iris=%d)\n",gStillCB.lensZoomSeg, gStillCB.ae.iris-1);
			}	
			appViewOsd_RealFnumberDraw(TRUE, aperturevValueStr[gStillCB.lensZoomSeg][gStillCB.ae.iris-1]);
			appViewOsd_RealShutterSpeedDraw(TRUE, shutterSpeedValueStr[gStillCB.ae.shutter]);
			appViewOsd_RealIsoValueDraw(TRUE, isoValueStr[gStillCB.ae.iso]);
		}		
		//draw face/focus-win
		if(gStillCB.ftStatus != VIEW_FT_STATUS_DESTROY && faceOsd.totalFaceNum != 0) 
		{
			appStillOsd_MainFaceDraw(gStillCB.focusStatus);
		}
		else 
		{
			#ifdef AF_CURVE_SHOW
			if(puiPara->af != UI_FOCUS_INFINITY)
			{
				AfValueCurveDraw(gStillCB.focusStatus);
			}
			else
			{
				appViewOsd_FocusWindowDraw(TRUE, gStillCB.focusStatus);
			}
			#else
			appViewOsd_FocusWindowDraw(TRUE, gStillCB.focusStatus);
			#endif
		}	
		break;
	}
	
	if(showType!=OSD_VIEW_S_ONE_HOLD)
	{
		appViewOsd_ASMChangeDraw();
	}
	if(gStillCB.err > VIEW_ERR_NO_ERROR) 
	{
		appViewOsd_WarmingMsgDraw(TRUE, gStillCB.err, FALSE);	
	}	
	if(showType==OSD_VIEW_VERBOSE && gStillCB.err<=VIEW_ERR_NO_ERROR ) 
	{
		appViewOsd_HistogramCreat();
	}	
	else 
	{
		appViewOsd_HistogramDestroy();
	}
	
	/*s Mask by Aries 12/02/09*/
	#if PANORAMA_MODE
	if((IS_SCENE_MODE)&&(IS_PANORAMA_MODE))
	{
		appViewOsd_PanoramaDraw(1,pViewParam->panorama);
	}
	#endif
	/*e Mask by Aries 12/02/09*/
	
	if(!outInOsdFreeze) 
	{
		APP_OSD_REFRESH_ON;
	}	
}

/*-------------------------------------------------------------------------
 *  File Name : app_StillOsd_PicSizeVideoDraw
 *------------------------------------------------------------------------*/
void 
appViewOsd_PicSizeVideoDraw(
	BOOL bShow, 
	UINT8 nPicSize
)
{
	static UINT8 lastRecord;
	UINT16 x,y;
	UINT32 strW,strH; /* "XXfps " */
	UINT32 strXPos;
	
	appOsdIconPosGet(ID_ICON_MOVIE_SIZE_1080P,&x,&y);
	sp5kGfxStringSizeGet(STR_ENCODE_ASCII, "60fps ", &strW, &strH);
	strXPos = x - strW;
	
	if (bShow) 
	{
		if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_VIDEO_SIZE && lastRecord==nPicSize) 
		{
			return ;
		}
		if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_VIDEO_SIZE && lastRecord!=nPicSize)
		{ 
			appViewOsd_PicSizeVideoDraw(FALSE, 0);
		}
		ViewOsdPresentStatus |= VIEW_OSD_PRESENT_VIDEO_SIZE;

		appOsd_FontIconFontSet(ID_FONT_ICON);
		switch(nPicSize) 
		{
			default:
				break;
			#if SPCA6330	
			case VIDEO_SIZE_1080P://PIC_SIZE_LARGE			
				appOsdLib_OsdLayerShapeClear(APP_OSD_SHAPE_RECT, strXPos, y, 50, 30, 0, 0);
				//appOsdLib_OsdLayIconDraw(APPVIEW_PICSIZE_X, APPVIEW_PICSIZE_Y, SP5K_GFX_ALIGN_TOP_LEFT , ID_ICON_MOVIE_SIZE_1080P);
				appIconShow(ID_ICON_MOVIE_SIZE_1080P,ICON_SHOW,SP5K_GFX_ALIGN_TOP_LEFT);
				break;
			
			
			case VIDEO_SIZE_720P_60FPS :
				appOsdLib_OsdLayerShapeClear(APP_OSD_SHAPE_RECT, strXPos, y, 50, 30, 0, 0);
				//appOsdLib_OsdLayIconDraw(APPVIEW_PICSIZE_X, APPVIEW_PICSIZE_Y, SP5K_GFX_ALIGN_TOP_LEFT , ID_ICON_MOVIE_SIZE_16_9);
				appIconShow(ID_ICON_MOVIE_SIZE_16_9,ICON_SHOW,SP5K_GFX_ALIGN_TOP_LEFT);
				appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_IRON, PAL_NOCOLOR, PAL_BLEND_50);
				appOsdLib_TextDraw(strXPos, y, SP5K_GFX_ALIGN_TOP_LEFT, "60fps");
				break;
			#endif		
			case VIDEO_SIZE_720P_30FPS://PIC_SIZE_2272
				appOsdLib_OsdLayerShapeClear(APP_OSD_SHAPE_RECT, strXPos, y, 50, 30, 0, 0);
				//appOsdLib_OsdLayIconDraw(APPVIEW_PICSIZE_X, APPVIEW_PICSIZE_Y, SP5K_GFX_ALIGN_TOP_LEFT , ID_ICON_MOVIE_SIZE_16_9);
				appIconShow(ID_ICON_MOVIE_SIZE_16_9,ICON_SHOW,SP5K_GFX_ALIGN_TOP_LEFT);
				appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_IRON, PAL_NOCOLOR, PAL_BLEND_50);
				appOsdLib_TextDraw(strXPos, y, SP5K_GFX_ALIGN_TOP_LEFT, "30fps");
				break;
			case VIDEO_SIZE_SVGA: 
				appOsdLib_OsdLayerShapeClear(APP_OSD_SHAPE_RECT, strXPos, y, 50, 30, 0, 0);
				//appOsdLib_OsdLayIconDraw(APPVIEW_PICSIZE_X, APPVIEW_PICSIZE_Y, SP5K_GFX_ALIGN_TOP_LEFT , ID_ICON_MOVIE_SIZE_640);
				appIconShow(ID_ICON_MOVIE_SIZE_D1,ICON_SHOW,SP5K_GFX_ALIGN_TOP_LEFT);
				break;
			case VIDEO_SIZE_640://PIC_SIZE_2272
				appOsdLib_OsdLayerShapeClear(APP_OSD_SHAPE_RECT, strXPos, y, 50, 30, 0, 0);
				//appOsdLib_OsdLayIconDraw(APPVIEW_PICSIZE_X, APPVIEW_PICSIZE_Y, SP5K_GFX_ALIGN_TOP_LEFT , ID_ICON_MOVIE_SIZE_640);
				appIconShow(ID_ICON_MOVIE_SIZE_640,ICON_SHOW,SP5K_GFX_ALIGN_TOP_LEFT);
				break;
			
			 case VIDEO_SIZE_QVGA :					
				appOsdLib_OsdLayerShapeClear(APP_OSD_SHAPE_RECT, strXPos, y, 50, 30, 0, 0);
        		//appOsdLib_OsdLayIconDraw(APPVIEW_PICSIZE_X, APPVIEW_PICSIZE_Y, SP5K_GFX_ALIGN_TOP_LEFT , ID_ICON_MOVIE_SIZE_320);
				appIconShow(ID_ICON_MOVIE_SIZE_320,ICON_SHOW,SP5K_GFX_ALIGN_TOP_LEFT);
        		break;
			
		}
		lastRecord=nPicSize;

	}
	else if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_VIDEO_SIZE) 
	{
		ViewOsdPresentStatus &= ~VIEW_OSD_PRESENT_VIDEO_SIZE;	
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, x, x,APPVIEW_ICON_SIZE_W, APPVIEW_ICON_SIZE_H, 0, 0);
	}	
}
/*-------------------------------------------------------------------------
 *  File Name : appViewOsd_PicQtyDraw
 *------------------------------------------------------------------------*/
void 
appViewOsd_PicQtyDraw(
	BOOL bShow, 
	UINT8 nPicQty
)
{
	static UINT8 lastRecord;
	UINT16 x,y;

	if (bShow)
	{
		//check
		if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_QUALITY && lastRecord==nPicQty) 
		{
			return;
		}	
		if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_QUALITY && lastRecord!=nPicQty) 
		{ 
			appViewOsd_PicQtyDraw(FALSE, 0);
		}	
		//record
		ViewOsdPresentStatus |= VIEW_OSD_PRESENT_QUALITY;
		lastRecord = nPicQty;
		
		appOsd_FontIconFontSet(ID_FONT_ICON);
		switch(nPicQty)
		{
			default:
			case UI_QUALITY_NORMAL:
				//appOsdLib_OsdLayIconDraw(APPVIEW_PICQTY_X, APPVIEW_PICQTY_Y, SP5K_GFX_ALIGN_TOP_LEFT,  ID_ICON_QUALITY);
				appIconShow(ID_ICON_QUALITY,ICON_SHOW,SP5K_GFX_ALIGN_TOP_LEFT);
				break;
			case UI_QUALITY_FINE:
				//appOsdLib_OsdLayIconDraw(APPVIEW_PICQTY_X, APPVIEW_PICQTY_Y, SP5K_GFX_ALIGN_TOP_LEFT,  ID_ICON_QUALITY_FINE);
				appIconShow(ID_ICON_QUALITY_FINE,ICON_SHOW,SP5K_GFX_ALIGN_TOP_LEFT);
				break;
			case UI_QUALITY_SUPERFINE:
				//appOsdLib_OsdLayIconDraw(APPVIEW_PICQTY_X, APPVIEW_PICQTY_Y, SP5K_GFX_ALIGN_TOP_LEFT,  ID_ICON_QUALITY_SUPER_FINE);
				appIconShow(ID_ICON_QUALITY_SUPER_FINE,ICON_SHOW,SP5K_GFX_ALIGN_TOP_LEFT);
				break;
		}	
	}	
	else if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_QUALITY) 
	{
		ViewOsdPresentStatus &= ~VIEW_OSD_PRESENT_QUALITY;
		appOsdIconPosGet(ID_ICON_QUALITY_SUPER_FINE,&x,&y);
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, x, y,APPVIEW_ICON_SIZE_W, APPVIEW_ICON_SIZE_H, 0, 0);
	}
}
/*-------------------------------------------------------------------------
 *  File Name : appViewOsd_PicQtyDraw
 *------------------------------------------------------------------------*/
void 
appViewOsd_VideoQtyDraw(
	BOOL bShow, 
	UINT8 nPicQty
)
{
	static UINT8 lastRecord;
	UINT16 x,y;

	if (bShow)
	{
		//check
		if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_QUALITY && lastRecord==nPicQty) 
		{
			return;
		}	
		if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_QUALITY && lastRecord!=nPicQty) 
		{ 
			appViewOsd_VideoQtyDraw(FALSE, 0);
		}	
		//record
		ViewOsdPresentStatus |= VIEW_OSD_PRESENT_QUALITY;
		lastRecord = nPicQty;
		
		appOsd_FontIconFontSet(ID_FONT_ICON);
		switch(nPicQty)
		{
			default:
			case UI_QUALITY_NORMAL:
				//appOsdLib_OsdLayIconDraw(APPVIEW_PICQTY_X-162, APPVIEW_PICQTY_Y+2, SP5K_GFX_ALIGN_TOP_LEFT,  ID_ICON_QUALITY);
				appIconShow(ID_ICON_QUALITY,ICON_SHOW,SP5K_GFX_ALIGN_TOP_LEFT);
				break;
			case UI_QUALITY_FINE:
				//appOsdLib_OsdLayIconDraw(APPVIEW_PICQTY_X-162, APPVIEW_PICQTY_Y+2, SP5K_GFX_ALIGN_TOP_LEFT,  ID_ICON_QUALITY_FINE);
				appIconShow(ID_ICON_QUALITY_FINE,ICON_SHOW,SP5K_GFX_ALIGN_TOP_LEFT);
				break;
			case UI_QUALITY_SUPERFINE:
				//appOsdLib_OsdLayIconDraw(APPVIEW_PICQTY_X-162, APPVIEW_PICQTY_Y+2, SP5K_GFX_ALIGN_TOP_LEFT,  ID_ICON_QUALITY_SUPER_FINE);
				appIconShow(ID_ICON_QUALITY_SUPER_FINE,ICON_SHOW,SP5K_GFX_ALIGN_TOP_LEFT);
				break;
		}	
	}	
	else if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_QUALITY) 
	{
		ViewOsdPresentStatus &= ~VIEW_OSD_PRESENT_QUALITY;
		appOsdIconPosGet(ID_ICON_QUALITY,&x,&y);
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, x, y,APPVIEW_ICON_SIZE_W, APPVIEW_ICON_SIZE_H, 0, 0);
	}
}

/*-------------------------------------------------------------------------
 *  File Name : app_StillOsd_RemainNumDraw
 *------------------------------------------------------------------------*/
void 
appViewOsd_RemainNumDraw(
	BOOL bShow, 
	unsigned char bHighLight, 
	unsigned int nRemainNum
)
{
	static unsigned int lastRecord;
	unsigned char szNum[8];
	sp5kGfxPageCapabDesc_t *pPageCap = NULL;
	static UINT32 strW = 0,strH = 0;
	UINT32 offsetX = 3,offsetY = 0;

	#if KIT_WITH_HDMI
	if(IS_HDMI_IN){
		offsetX = 20;
		offsetY = 10;
	}
	#endif

	if(!pPageCap){
		pPageCap = sp5kMalloc(sizeof(sp5kGfxPageCapabDesc_t));
	}
	if(!pPageCap){
		return;
	}
	sp5kGfxPageCapabilityGet(SP5K_GFX_PAGE_OSD_0,pPageCap);
	//printf(" ----------------  window:%d  %d\n",pPageCap->frmW,pPageCap->frmH);
	
	if (nRemainNum>9999)
	{
		nRemainNum=9999;
	}

	nCaptureRemainNum = nRemainNum;

	if (bShow) 
	{
		//check
		if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_REMAIN_IMAGE_NUMBER 
			&& lastRecord==nRemainNum) 
		{
			sp5kFree(pPageCap);
			return;
		}	
		if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_REMAIN_IMAGE_NUMBER 
			&& lastRecord!=nRemainNum) 
		{ 
			appViewOsd_RemainNumDraw(FALSE, 0, 0);
		}	
		//record
		ViewOsdPresentStatus |= VIEW_OSD_PRESENT_REMAIN_IMAGE_NUMBER;
		lastRecord = nRemainNum;
		
		appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_IRON, PAL_NOCOLOR, PAL_BLEND_50);
		sprintf(szNum, "%4d", nRemainNum);
		sp5kGfxStringSizeGet(STR_ENCODE_ASCII, szNum, &strW, &strH);
		appOsdLib_TextDraw(pPageCap->frmW-offsetX, offsetY, SP5K_GFX_ALIGN_TOP_RIGHT, szNum);
	}		
	else if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_REMAIN_IMAGE_NUMBER) 
	{
		ViewOsdPresentStatus &= ~VIEW_OSD_PRESENT_REMAIN_IMAGE_NUMBER;	
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, pPageCap->frmW-strW-offsetX, offsetY,strW, strH, 0, 0);
	}
	sp5kFree(pPageCap);
}

/*-------------------------------------------------------------------------
 *  File Name : app_StillOsd_AudioTotalFreeTimeDraw
 *------------------------------------------------------------------------*/
void 
appViewOsd_AudioTotalFreeTimeDraw(
	BOOL bShow, 
	unsigned char bHighLight, 
	UINT32 nSec
)
{
	unsigned char 	szTime[16];
	//unsigned short 	nHourX=0;
	sp5kGfxPageCapabDesc_t *pPageCap = NULL;
	static UINT32 strW = 0,strH = 0;
	UINT32 offsetX = 3,offsetY = 3;

	#if KIT_WITH_HDMI
	if(IS_HDMI_IN){
		offsetX = 20;
		offsetY = 10;
	}
	#endif

	if(!pPageCap){
		pPageCap = sp5kMalloc(sizeof(sp5kGfxPageCapabDesc_t));
	}
	if(!pPageCap){
		return;
	}
	sp5kGfxPageCapabilityGet(SP5K_GFX_PAGE_OSD_0,pPageCap);
	
	//nHourX=APPVIEW_AUDIO_FREETIME_X+103;

	if (!bShow)
	{
		ViewOsdPresentStatus &= ~VIEW_OSD_PRESENT_VIDEO_REMAIN_TIME;
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, pPageCap->frmW-offsetX-strW, offsetY,strW, strH, 0, 0);
		sp5kFree(pPageCap);
		return ;

	}
	ViewOsdPresentStatus |= VIEW_OSD_PRESENT_VIDEO_REMAIN_TIME;
	
	memset(szTime, 0, 16);

	appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, pPageCap->frmW-offsetX-strW, offsetY,strW, strH, 0, 0);
	sprintf(szTime, "%02u:%02u:%02u", nSec/3600, (nSec%3600)/60, (nSec%3600)%60);
	sp5kGfxStringSizeGet(STR_ENCODE_ASCII, szTime, &strW, &strH);
	
	appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
	if(nSec/3600>99)
	{
		appOsdLib_TextDraw(pPageCap->frmW-offsetX, offsetY, SP5K_GFX_ALIGN_TOP_RIGHT, szTime);
	}
	else
	{
		appOsdLib_TextDraw(pPageCap->frmW-offsetX, offsetY, SP5K_GFX_ALIGN_TOP_RIGHT, szTime);
	}
	sp5kFree(pPageCap);
}

/*-------------------------------------------------------------------------
 *  File Name : app_StillOsd_RealIsoValueDraw
 *------------------------------------------------------------------------*/
void 
appViewOsd_RealIsoValueDraw(
	BOOL bShow,
	UINT8* str
)
{
	DBG_PRINT("%s\n", __FUNCTION__);
	
	if (bShow) 
	{
		if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_REAL_ISO)
		{
			return;
		}	
		ViewOsdPresentStatus |= VIEW_OSD_PRESENT_REAL_ISO;	
		appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_TextDraw(APPVIEW_REAL_ISO_ICON_X, APPVIEW_REAL_ISO_ICON_Y, SP5K_GFX_ALIGN_TOP_RIGHT, str);	
	} 
	else if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_REAL_ISO) 
	{
		ViewOsdPresentStatus &= ~VIEW_OSD_PRESENT_REAL_ISO;	
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT,APPVIEW_REAL_ISO_ICON_X-(APPVIEW_ICON_SIZEX*2+APPVIEW_ICON_Y_GAP+6), APPVIEW_REAL_ISO_ICON_Y, APPVIEW_ICON_SIZEX*2+APPVIEW_ICON_Y_GAP+6, APPVIEW_ICON_SIZEY, 0, 0);	
	}	
}

/*-------------------------------------------------------------------------
 *  File Name : appViewOsd_StabilizerModeDraw
 *------------------------------------------------------------------------*/
void 
appViewOsd_FDModeDraw(
	BOOL bShow,
	UINT8 nFDMode
)
{
	UINT16 x,y;
	
	if (bShow) 
	{
		appOsd_FontIconFontSet(ID_FONT_ICON);
		switch(nFDMode)
		{
			case UI_FACETRACK_ON:
				//appOsdLib_OsdLayIconDraw(APPVIEW_FD_X, APPVIEW_FD_Y,  SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_FACE_TRACKING);
				appIconShow(ID_ICON_FACE_TRACKING,ICON_SHOW,SP5K_GFX_ALIGN_TOP_LEFT);
				break;
			case UI_FACETRACK_SMILE_DETECT:
				//appOsdLib_OsdLayIconDraw(APPVIEW_FD_X, APPVIEW_FD_Y,  SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_SMILE_DETECT);
				appIconShow(ID_ICON_SMILE_DETECT,ICON_SHOW,SP5K_GFX_ALIGN_TOP_LEFT);
				break;
			case UI_FACETRACK_BLINKING_DETECT:
				//appOsdLib_OsdLayIconDraw(APPVIEW_FD_X, APPVIEW_FD_Y,  SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_BLINKING_DETECT);
				appIconShow(ID_ICON_BLINKING_DETECT,ICON_SHOW,SP5K_GFX_ALIGN_TOP_LEFT);
				break;
			case UI_FACETRACK_OBJECT_DETECT:
				//appOsdLib_OsdLayIconDraw(APPVIEW_FD_X, APPVIEW_FD_Y,  SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_OBJECT_DETECT);
				appIconShow(ID_ICON_OBJECT_DETECT,ICON_SHOW,SP5K_GFX_ALIGN_TOP_LEFT);
				break;
			case UI_FACETRACK_OFF:
			default:
				//appOsdLib_OsdLayIconDraw(APPVIEW_FD_X, APPVIEW_FD_Y,  SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_NONE);
				appIconShow(ID_ICON_NONE,ICON_SHOW,SP5K_GFX_ALIGN_TOP_LEFT);
				break;
			}
	}	
	else
	{
		appOsdIconPosGet(ID_ICON_FACE_TRACKING,&x,&y);
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, x, y,APPVIEW_ICON_SIZEX, APPVIEW_ICON_SIZEY, 0, 0);	
	} 
}

#if GPS_SUPPORT
/*-------------------------------------------------------------------------
 *  File Name : appViewOsd_GPSModeDraw
 *------------------------------------------------------------------------*/
 void
 appViewOsd_GPSModeDraw(
 	BOOL bShow,
	UINT8 nGPSMode
)
{
	UINT16 x,y;

	if(bShow)
	{
		appOsd_FontIconFontSet(ID_FONT_ICON);
		switch(nGPSMode)
		{
			case UI_GPS_STATUS_ON:
				appIconShow(ID_ICON_GPS,ICON_SHOW,SP5K_GFX_ALIGN_TOP_LEFT);
				break;
			case UI_GPS_STATUS_OFF:
			default:
				appIconShow(ID_ICON_GPS,ICON_CLEAN,SP5K_GFX_ALIGN_TOP_LEFT);
				break;
		}
	}
	else
	{
		appOsdIconPosGet(ID_ICON_GPS,&x,&y);
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, x, y,APPVIEW_ICON_SIZEX, APPVIEW_ICON_SIZEY, 0, 0);
	}
}
#endif

/*-------------------------------------------------------------------------
 *  File Name : appViewOsd_StabilizerModeDraw
 *------------------------------------------------------------------------*/
void 
appViewOsd_StabilizerModeDraw(
	BOOL bShow
)
{
	UINT16 x,y;
	
	if (bShow) 
	{
		if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_STABILIZER) 
		{
			return;
		}
		ViewOsdPresentStatus |= VIEW_OSD_PRESENT_STABILIZER;
		appOsd_FontIconFontSet(ID_FONT_ICON);	
		//appOsdLib_OsdLayIconDraw(APPVIEW_STABILIZER_X, APPVIEW_STABILIZER_Y,  SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_STABILIZER);
		appIconShow(ID_ICON_STABILIZER,ICON_SHOW,SP5K_GFX_ALIGN_TOP_LEFT);
	}	
	else if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_STABILIZER) 
	{
		ViewOsdPresentStatus &= ~VIEW_OSD_PRESENT_STABILIZER;
		appOsdIconPosGet(ID_ICON_STABILIZER,&x,&y);
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, x, y,APPVIEW_ICON_SIZEX, APPVIEW_ICON_SIZEY, 0, 0);	
	} 
}

/*-------------------------------------------------------------------------
 *  File Name : appViewOsd_AebDraw
 *------------------------------------------------------------------------*/
void appViewOsd_AebDraw(
	UINT8 bShow
)
{
	uiPara_t* puiPara = appUiParaGet();
	UINT16 x,y;
	
	if(bShow)
	{
		if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_AEB)
		{
			return;
		}	
		ViewOsdPresentStatus |= VIEW_OSD_PRESENT_AEB;
		
		appOsd_FontIconFontSet(ID_FONT_ICON);
		//appOsdLib_OsdLayIconDraw(APPVIEW_AEB_X,APPVIEW_AEB_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_AE_BRACKET);
		appIconShow(ID_ICON_AE_BRACKET,ICON_SHOW,SP5K_GFX_ALIGN_TOP_LEFT);
	}
	else if (puiPara->DriverMode == UI_DRIVE_OFF 
		&& ViewOsdPresentStatus&VIEW_OSD_PRESENT_AEB)
	{				
		ViewOsdPresentStatus &= ~VIEW_OSD_PRESENT_AEB;
		appOsdIconPosGet(ID_ICON_AE_BRACKET,&x,&y);
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT,x, y,APPVIEW_ICON_SIZE_W,APPVIEW_ICON_SIZE_H,0,0);
	}	
}

/*-------------------------------------------------------------------------
 *  File Name : appViewOsd_ZoomFrameDraw
 *------------------------------------------------------------------------*/

void appViewOsd_ZoomFrameDraw(
	UINT8 bShow
)
{
	uiPara_t* puiPara = appUiParaGet();

	if(puiPara->DigitalZoom)
	{
		appViewOsd_DigZoomFrameDraw(bShow);
	}else
	{
		appViewOsd_OptZoomFrameDraw(bShow);
	}
}
/*-------------------------------------------------------------------------
 *  File Name : appViewOsd_ZoomFrameDraw
 *------------------------------------------------------------------------*/

void appViewOsd_DigZoomFrameDraw(
	UINT8 bShow
)
{
	if (bShow) 
	{
		appOsd_ColorDrawShapeColorSet(PAL_GRAY_1, PAL_BLEND_50, PAL_GRAY_1, PAL_BLEND_50);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, OPTICAL_ZOOM_BAR_X, OPTICAL_ZOOM_BAR_Y, OPTICAL_ZOOM_BAR_SIZEX + DIGITAL_ZOOM_BAR_SIZEX + 6, OPTICAL_ZOOM_BAR_SIZEY, 0, 0);
		appOsd_FontIconFontSet(ID_ICON_ZOOM);
		appOsdLib_OsdLayIconDraw(VIEW_ZOOM_FRAME_X, VIEW_ZOOM_FRAME_Y, SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_DIGITAL_ZOOM);
	}
	else if (!bShow) 
	{
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, VIEW_ZOOM_FRAME_X, VIEW_ZOOM_FRAME_Y, OPTICAL_DIGITAL_ZOOM_ICON_SIZEX, OPTICAL_DIGITAL_ZOOM_ICON_SIZEY, 0, 0);
	}

}
/*-------------------------------------------------------------------------
 *  File Name : appViewOsd_OptZoomFrameDraw
 *------------------------------------------------------------------------*/

void appViewOsd_OptZoomFrameDraw(
	UINT8 bShow
)
{
	if (bShow) 
	{
		appOsd_GfxIconIDSet(SP5K_RES_OSD_ICON_5);
		appOsd_ColorDrawShapeColorSet(PAL_GRAY_1, PAL_BLEND_50, PAL_GRAY_1, PAL_BLEND_50);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, OPTICAL_ZOOM_BAR_X, OPTICAL_ZOOM_BAR_Y, OPTICAL_ZOOM_BAR_SIZEX, OPTICAL_ZOOM_BAR_SIZEY, 0, 0);
		//appOsd_FontIconFontSet(ID_ICON_ZOOM);
		appOsdLib_OsdLayIconDraw(VIEW_ZOOM_FRAME_X, VIEW_ZOOM_FRAME_Y, SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_OPTICAL_ZOOM);
		appOsd_GfxIconIDSet(SP5K_RES_OSD_ICON);

	}
	else if (!bShow) 
	{
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, VIEW_ZOOM_FRAME_X, VIEW_ZOOM_FRAME_Y, OPTICAL_DIGITAL_ZOOM_ICON_SIZEX, OPTICAL_DIGITAL_ZOOM_ICON_SIZEY, 0, 0);
	}

}

/*-------------------------------------------------------------------------
 *  File Name : appViewOsd_OptZoomPosDraw
 *------------------------------------------------------------------------*/

void appViewOsd_OptZoomPosDraw(
	UINT8 Position
)
{
	if (Position > 100 )
	{
		Position = 100;
	}
	if (Position !=0)
	{
		float BarPosition = Position;
		BarPosition = (BarPosition / 100.0) * ((float)OPTICAL_ZOOM_BAR_SIZEX);
		appOsd_ColorDrawShapeColorSet(PAL_GREEN, PAL_BLEND_100, PAL_GREEN, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, OPTICAL_ZOOM_BAR_X, OPTICAL_ZOOM_BAR_Y, BarPosition, OPTICAL_ZOOM_BAR_SIZEY, 0, 0);
	}
}


/*-------------------------------------------------------------------------
 *  File Name : appViewOsd_DigZoomPosDraw
 *------------------------------------------------------------------------*/

void appViewOsd_DigZoomPosDraw(
	UINT8 Position
)
{
	if (Position > 100 )
	{
		Position = 100;
	}
	appViewOsd_OptZoomPosDraw(100);
	if (Position !=0)
	{
		float BarPosition = Position;
		BarPosition = (BarPosition / 100.0) * ((float)DIGITAL_ZOOM_BAR_SIZEX);
		appOsd_ColorDrawShapeColorSet(PAL_BLUE, PAL_BLEND_100, PAL_BLUE, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, DIGITAL_ZOOM_BAR_X, DIGITAL_ZOOM_BAR_Y, BarPosition, DIGITAL_ZOOM_BAR_SIZEY, 0, 0);
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appViewOsd_ASMChangeDraw
 *------------------------------------------------------------------------*/
void appViewOsd_ASMChangeDraw(
)
{
	uiPara_t* puiPara = appUiParaGet();
	
	if(IS_PASM_AMODE) 
	{
		appViewOsd_AmodeAvDraw(TRUE, 1, pViewParam->aperture, gStillCB.lensZoomSeg);
		appViewOsd_SmodeTvDraw(FALSE, 1, puiPara->shutter);
	}
	else if(IS_PASM_SMODE) 
	{
		appViewOsd_AmodeAvDraw(FALSE, 1, pViewParam->aperture, gStillCB.lensZoomSeg);
		appViewOsd_SmodeTvDraw(TRUE, 1, puiPara->shutter);
	}
	else if(IS_PASM_MMODE) 
	{
		appViewOsd_AmodeAvDraw(TRUE, (IS_PASM_MMODE_AV)? 1:0, pViewParam->aperture, gStillCB.lensZoomSeg);
		appViewOsd_SmodeTvDraw(TRUE, (IS_PASM_MMODE_TV)? 1:0, puiPara->shutter);
	}
	else
	{
		appViewOsd_AmodeAvDraw(FALSE, (IS_PASM_MMODE_AV)? 1:0, pViewParam->aperture, gStillCB.lensZoomSeg);
		appViewOsd_SmodeTvDraw(FALSE, (IS_PASM_MMODE_TV)? 1:0, puiPara->shutter);
	}
}

/*-------------------------------------------------------------------------
 *  Function Name : appViewOsd_HistogramCreat
 *  Description :	To creat the hitogram and assist timer.
 *					The hitogram object has creat/running/destroy status.
 *------------------------------------------------------------------------*/
void 
appViewOsd_HistogramCreat(void)
{
	if(gHistogram.status == OSD_OBJ_RUNNING) 
	{
		return;
	}	
	sp5kImgHistogramGet(HistogramBuf);
	appViewOsd_HistogramDraw(OSD_OBJ_CREATED, HistogramBuf);		
	gHistogram.timer = appTimerSet(500, "hitogram");
	gHistogram.status = OSD_OBJ_RUNNING;
}

/*-------------------------------------------------------------------------
 *  Function Name : appViewOsd_HistogramDestroy
 *  Description :	To cancle the timer and clean hitogram
 *------------------------------------------------------------------------*/
void 
appViewOsd_HistogramDestroy(void)
{
	//printf("_______%s\n", __FUNCTION__);
	if(gHistogram.status == OSD_OBJ_DESTROYED) 
	{
		return;
	}	

	if(gHistogram.status == OSD_OBJ_RUNNING) 
	{
		appTimerClear(&gHistogram.timer);		
	}		
	
	appViewOsd_HistogramDraw(OSD_OBJ_DESTROYED, NULL);
	gHistogram.status = OSD_OBJ_DESTROYED;
}

/*-------------------------------------------------------------------------
 *  Function Name : appViewOsd_HistogramUpdateStop
 *  Description :	only stop the timer
 *------------------------------------------------------------------------*/
void 
appViewOsd_HistogramUpdateStop(void)
{
	if(gHistogram.status == OSD_OBJ_RUNNING) 
	{
		appTimerClear(&gHistogram.timer);
		gHistogram.status = OSD_OBJ_CREATED;		
	}
}

/*-------------------------------------------------------------------------
 *  Function Name : appViewOsd_HistogramUpdateStart
 *  Description :	Re-start timer and init every thing if we didn't creat
 *					before.
 *------------------------------------------------------------------------*/
void 
appViewOsd_HistogramUpdateStart(void)
{
	if(gHistogram.status == OSD_OBJ_DESTROYED) 
	{
		appViewOsd_HistogramCreat();		
	}
	else if(gHistogram.status == OSD_OBJ_DESTROYED) 
	{
		gHistogram.timer = appTimerSet(250, "hitogram");
	}
}

/*-------------------------------------------------------------------------
 *  Function Name : appViewOsd_HistogramProcess
 *  Description :	This will be put on background.	It is responsible to 
 *					receive the timer msg and draw new histogram data.
 *------------------------------------------------------------------------*/
void 
appViewOsd_HistogramProcess(
	UINT32 param
)
{	
	UINT32 curState;
	
	if (param == gHistogram.timer) 
	{		
		
		if(gHistogram.status != OSD_OBJ_RUNNING) 
		{
			return;
		}
	
		/* We will let histogram stop/destroy itself if it leave preview states */
		curState = appActiveStateGet();
		if(curState != APP_STATE_STILL_PREVIEW && curState != APP_STATE_VIDEO_PREVIEW) 
		{	
			appViewOsd_HistogramUpdateStop();
			return;
		}
		else if(!IS_VIEW_GROUP(curState) && !IS_VIDEO_GROUP(curState))
		{
			DBG_PRINT("%s: not group\n", __FUNCTION__);
			appViewOsd_HistogramDestroy();
			return;
		}	
		
		sp5kImgHistogramGet(HistogramBuf);
		appViewOsd_HistogramDraw(OSD_OBJ_RUNNING, HistogramBuf);
	}	
}


/*-------------------------------------------------------------------------
*  File Name : appViewOsd_VideoTotalFreeTimeDraw
*------------------------------------------------------------------------*/
void 
appViewOsd_VideoTotalFreeTimeDraw(
	BOOL bShow, 
	UINT32 nSec
)
{
	unsigned char 	szTime[16];
	//unsigned short 	nHourX=0;
	sp5kGfxPageCapabDesc_t *pPageCap = NULL;
	static UINT32 strW = 0,strH = 0;
	UINT32 offsetX = 3,offsetY = 3;

	#if KIT_WITH_HDMI
	if(IS_HDMI_IN){
		offsetX = 20;
		offsetY = 10;
	}
	#endif

	if(!pPageCap){
		pPageCap = sp5kMalloc(sizeof(sp5kGfxPageCapabDesc_t));
	}
	if(!pPageCap){
		return;
	}
	sp5kGfxPageCapabilityGet(SP5K_GFX_PAGE_OSD_0,pPageCap);

	//nHourX=APPVIEW_AUDIO_FREETIME_X+103;

	if (!bShow)
	{
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, pPageCap->frmW-offsetX-strW, offsetY,strW, strH, 0, 0);
		sp5kFree(pPageCap);
		return ;

	}
	memset(szTime, 0, 16);

	appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, pPageCap->frmW-offsetX-strW, offsetY,strW, strH, 0, 0);
	sprintf(szTime, "%02u:%02u:%02u", nSec/3600, (nSec%3600)/60, (nSec%3600)%60);
	sp5kGfxStringSizeGet(STR_ENCODE_ASCII, szTime, &strW, &strH);
	
	appOsd_ColorDrawTextColorSet(PAL_RED, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
	if(nSec/3600>99)
	{
		appOsdLib_TextDraw(pPageCap->frmW-offsetX, offsetY, SP5K_GFX_ALIGN_TOP_RIGHT, szTime);
	}
	else
	{
		appOsdLib_TextDraw(pPageCap->frmW-offsetX, offsetY, SP5K_GFX_ALIGN_TOP_RIGHT, szTime);
	}
	sp5kFree(pPageCap);
}


/*-------------------------------------------------------------------------
*  File Name : appViewOsd_VideoRecIconDraw
*------------------------------------------------------------------------*/
void 
appViewOsd_VideoRecIconDraw(
	BOOL bShow 
)
{
	uiPara_t* puiPara = appUiParaGet();
	UINT16 x,y;
	UINT32 iconSizeX = APPVIEW_ICON_SIZEX,iconSizeY = APPVIEW_ICON_SIZEY;

	#if KIT_WITH_HDMI
	if(IS_HDMI_IN){
		iconSizeX = 48;
		iconSizeY = 42;
	}
	#endif
	
	if(bShow)
	{
		appOsd_FontIconFontSet(ID_FONT_ICON);	
		if(puiPara->VideoStampMode != 0){
			//appOsdLib_OsdLayIconDraw(APPVIEW_REAL_ISO_ICON_X, 0,  SP5K_GFX_ALIGN_TOP_RIGHT, ID_ICON_VIDEO_RECORD);
			appIconShow(ID_ICON_VIDEO_RECORD_DS,ICON_SHOW,SP5K_GFX_ALIGN_TOP_LEFT);
		}
		else{
			//appOsdLib_OsdLayIconDraw(LCD_SIZE_X-93-APPVIEW_ICON_SIZEX, APPVIEW_AUDIO_FREETIME_Y-3,  SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_VIDEO_RECORD);
			appIconShow(ID_ICON_VIDEO_RECORD,ICON_SHOW,SP5K_GFX_ALIGN_TOP_LEFT);
		}
	}
	else
	{
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		if(puiPara->VideoStampMode != 0){
			appOsdIconPosGet(ID_ICON_VIDEO_RECORD_DS,&x,&y);
			appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, x, y,iconSizeX, iconSizeY, 0, 0);
		}
		else{
			appOsdIconPosGet(ID_ICON_VIDEO_RECORD,&x,&y);
			appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, x, y,iconSizeX, iconSizeY, 0, 0);
		}
	}
}

/*-------------------------------------------------------------------------
*  File Name : appViewOsd_VideoPauseIconDraw
*------------------------------------------------------------------------*/
void 
appViewOsd_VideoPauseIconDraw(
	BOOL bShow 
)
{
	uiPara_t* puiPara = appUiParaGet();
	UINT16 x,y;
	
	if(bShow)
	{
		appOsd_FontIconFontSet(ID_FONT_ICON);	
		if(puiPara->VideoStampMode != 0)
		{
			//appOsdLib_OsdLayIconDraw(APPVIEW_REAL_ISO_ICON_X, 0,  SP5K_GFX_ALIGN_TOP_RIGHT, ID_ICON_VIDEO_RECORD_PAUSE);
			appIconShow(ID_ICON_VIDEO_RECORD_PAUSE_DS,ICON_SHOW,SP5K_GFX_ALIGN_TOP_LEFT);
		}
		else
		{
			//appOsdLib_OsdLayIconDraw(LCD_SIZE_X-93-APPVIEW_ICON_SIZEX, APPVIEW_AUDIO_FREETIME_Y-3,  SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_VIDEO_RECORD_PAUSE);
			appIconShow(ID_ICON_VIDEO_RECORD_PAUSE,ICON_SHOW,SP5K_GFX_ALIGN_TOP_LEFT);
		}
	}
	else
	{
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		if(puiPara->VideoStampMode != 0)
		{
			appOsdIconPosGet(ID_ICON_VIDEO_RECORD_PAUSE_DS,&x,&y);
			appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, x, y,APPVIEW_ICON_SIZEX, APPVIEW_ICON_SIZEY, 0, 0);
		}
		else
		{
			appOsdIconPosGet(ID_ICON_VIDEO_RECORD_PAUSE,&x,&y);
			appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, x, y,APPVIEW_ICON_SIZEX, APPVIEW_ICON_SIZEY, 0, 0);
		}
	}
}

/*-------------------------------------------------------------------------
*  File Name : appViewOsd_VideoTotalFreeTimeDraw
*------------------------------------------------------------------------*/
void 
appViewOsd_VideoRecControlDraw(
	UINT8 Recstatus 
)
{
	uiPara_t* puiPara = appUiParaGet();
	UINT16 x,y;
	UINT32 xPos;

	appOsdIconPosGet(ID_ICON_BUTTON_RIGHT,&x,&y);
	xPos = x+APPVIEW_ICON_SIZE_W;
	#if KIT_WITH_HDMI
	if(IS_HDMI_IN){
		xPos = x+APPVIEW_ICON_SIZE_W+10;
	}
	#endif
	appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT,x, y,x+FONT_W*6+20, FONT_H+10, 0, 0);

	appOsd_FontIconFontSet(ID_FONT_ICON);
	appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
	//appOsdLib_OsdLayIconDraw(LCD_SIZE_X/2-APPVIEW_ICON_SIZEX+8, VIDEO_REC_CONTROL_Y+16, SP5K_GFX_ALIGN_CENTER, ID_ICON_BUTTON_RIGHT);	;/*willow modify@20100611*/
	appIconShow(ID_ICON_BUTTON_RIGHT,ICON_SHOW,SP5K_GFX_ALIGN_TOP_LEFT);

	if(puiPara->VideoStampMode){
		sp5kGfxAttrSet(SP5K_GFX_FONT_ID, SP5K_RES_OSD_FONT, 0, 0, 0);
	}
	switch(Recstatus)
	{
		case VIDEO_REC_RECORDING:
			appViewOsd_VideoRecIconDraw(FALSE);
			appOsdLib_TextIDDraw(xPos,y,SP5K_GFX_ALIGN_TOP_LEFT,ID_STR_REC);
			appViewOsd_VideoPauseIconDraw(TRUE);
			break;
		case VIDEO_REC_PAUSE:
			appViewOsd_VideoPauseIconDraw(FALSE);
			appOsdLib_TextIDDraw(xPos,y,SP5K_GFX_ALIGN_TOP_LEFT,ID_STR_PAUSE);
			appViewOsd_VideoRecIconDraw(TRUE);
			break;
		default:	
		case VIDEO_REC_STOP:
			break;
	}
	if(puiPara->VideoStampMode){
		sp5kGfxAttrSet(SP5K_GFX_FONT_ID, SP5K_RES_DATE_STAMP_OSD, 0, 0, 0);
	}
}
/*-------------------------------------------------------------------------
 *  Function Name : appStillOsd_DrawOneFace
 *  Description : Here we draw line but shap to speed the performance	
 *------------------------------------------------------------------------*/
static void appStillOsd_OneFaceDraw(UINT32 faceIdx, UINT32 color)
{
	UINT32 i, line_w=3, temp;
	sp5kGfxPageCapabDesc_t *pPageCap = NULL;
	
	if(!pPageCap){
		pPageCap = sp5kMalloc(sizeof(sp5kGfxPageCapabDesc_t));
	}
	if(!pPageCap){
		return;
	}
	sp5kGfxPageCapabilityGet(SP5K_GFX_PAGE_OSD_0,pPageCap);
	
	UINT32 x = faceOsd.rectFace[faceIdx].nStartX;
	UINT32 y = faceOsd.rectFace[faceIdx].nStartY;
	UINT32 w = faceOsd.rectFace[faceIdx].nSize;
	UINT32 h = faceOsd.rectFace[faceIdx].nSize;

	#if KIT_WITH_HDMI
	if(IS_HDMI_IN){
		x *= 2;
		y  = (y*3/2);
		w *= 2;
		h  = (h*3/2);
	}
	#endif
	
	sp5kGfxAttrSet(SP5K_GFX_PEN_COLOR, color, 0, 0, 0);
	sp5kGfxAttrSet(SP5K_GFX_FILL_COLOR, color, 0, 0, 0);

	temp = line_w-1;
	x = x<temp? temp : x;
	y = y<temp? temp : y;
	w = (x+w+temp)>=pPageCap->frmW? w-temp : w;
	h = (y+h+temp)>=pPageCap->frmH? h-temp : h;
	
	for(i=0; i<line_w; i++, x--,	y--, w+=2, h+=2) 
	{
		appOsdLib_PIPLayerLineDraw(  x,   y, x+w,   y);//top
		appOsdLib_PIPLayerLineDraw(  x,   y,   x, y+h);//left
		appOsdLib_PIPLayerLineDraw(  x, y+h, x+w, y+h);//down
		appOsdLib_PIPLayerLineDraw(x+w,   y, x+w, y+h);//right
	}

	sp5kFree(pPageCap);
}
/*-------------------------------------------------------------------------
 *  Function Name : appStillOsd_AllFacesDraw
 *  Description :	
 *------------------------------------------------------------------------*/
//static void appStillOsd_AllFacesDraw(BOOL bShow)
void appStillOsd_AllFacesDraw(BOOL bShow)
{
	UINT32 i=0;
	UINT32 color = bShow? PIP_COLOR565_WHITE: 0;
	
	for (i = 0; i < faceOsd.totalFaceNum; i++) 
	{	
		appStillOsd_OneFaceDraw(i, color);
	}	
}

/*-------------------------------------------------------------------------
 *  Function Name : appStillOsd_AllFacesDraw
 *  Description :	Here we have some osd test results. (#>snap osdtest)
 * 		1. The draw area indeed affect the speed.(not proportion)
 *		2. The os content switch also affect the speed.
 *		3. To draw line is faster than draw shap. (in same area)
 *
 *		clear pip layer time 320x240 = 9600/1000 ms
 *		draw one face time = 1200/1000 ms (use 4 shape draw)
 *		clean one face time = 4600/1000 ms (use 1 shape draw)
 *		draw 32x24 time = 300/1000 ms
 *		draw 60x60 time = 4300/1000 ms
 *		draw 320x240 time = 9300/1000 ms
 *		preview cb entry interval time = 14000/100 ms
 *
 *		clear pip layer time 320x240 = 9600/1000 ms
 *		draw one face time = 1100/1000 ms (use 4 shape draw)
 *		clean one face time = 500/1000 ms (use 1 shape draw)
 *		draw 32x24 time = 200/1000 ms
 *		draw 60x60 time = 5400/1000 ms
 *		draw 320x240 time = 10300/1000 ms
 *		preview cb entry interval time = 14000/100 ms
 *
 *		clear pip layer time 320x240 = 10200/1000 ms 
 *		draw one face time = 100/1000 ms (use 8 line draw)
 *		clean one face time = 100/1000 ms (use 8 line draw)
 *		draw 32x24 time = 4300/1000 ms
 *		draw 60x60 time = 500/1000 ms
 *		draw 320x240 time = 9800/1000 ms
 *
 *		clear pip layer time 320x240 = 9900/1000 ms
 *		draw one face time = 300/1000 ms (use 8 line draw)
 *		clean one face time = 300/1000 ms (use 8 line draw)
 *		draw 32x24 time = 300/1000 ms
 *		draw 60x60 time = 5000/1000 ms
 *		draw 320x240 time = 9700/1000 ms
 *
 *		clear pip layer time 320x240 = 9700/1000 ms
 *		draw one face time = 300/1000 ms (use 8 line draw)
 *		clean one face time = 300/1000 ms (use 8 line draw)
 *		draw 32x24 time = 300/1000 ms
 *		draw 60x60 time = 4400/1000 ms
 *		draw 320x240 time = 10300/1000 ms
 *------------------------------------------------------------------------*/

/* Add by sc.liu */
/*-------------------------------------------------------------------------
 *  Function Name : appStillOsd_DrawOneObject
 *  Description : Here we draw line but shap to speed the performance	
 *------------------------------------------------------------------------*/

static void appStillOsd_OneObjectDraw(UINT32 objIdx, UINT32 color)
{
	UINT32 i, line_w=3, temp;
	sp5kGfxPageCapabDesc_t *pPageCap = NULL;

	if(!pPageCap){
		pPageCap = sp5kMalloc(sizeof(sp5kGfxPageCapabDesc_t));
	}
	if(!pPageCap){
		return;
	}
	sp5kGfxPageCapabilityGet(SP5K_GFX_PAGE_OSD_0,pPageCap);

	if(ObjTrackTargetStop){
		sp5kFree(pPageCap);
		return;
	}
	
	UINT32 x = objectOsd.rectObj[objIdx].nStartX-(objectOsd.rectObj[objIdx].nSize/2);
	UINT32 y = objectOsd.rectObj[objIdx].nStartY-(objectOsd.rectObj[objIdx].nSize/2);
	UINT32 w = objectOsd.rectObj[objIdx].nSize;
	UINT32 h = objectOsd.rectObj[objIdx].nSize;

	#if KIT_WITH_HDMI
	if(IS_HDMI_IN){
		x *= 2;
		y  = (y*3/2);
		w *= 2;
		h  = (h*3/2);
	}
	#endif

	sp5kGfxAttrSet(SP5K_GFX_PEN_COLOR, color, 0, 0, 0);
	sp5kGfxAttrSet(SP5K_GFX_FILL_COLOR, color, 0, 0, 0);
			
	temp = line_w-1;
	x = x<temp? temp : x;
	y = y<temp? temp : y;
	w = (x+w+temp)>=pPageCap->frmW? w-temp : w;
	h = (y+h+temp)>=pPageCap->frmH? h-temp : h;

	for(i=0; i<line_w; i++, x--,	y--, w+=2, h+=2) 
	{	
		appOsdLib_PIPLayerLineDraw(  x,   y, x+w,   y);//top
		appOsdLib_PIPLayerLineDraw(  x,   y,   x, y+h);//left
		appOsdLib_PIPLayerLineDraw(  x, y+h, x+w, y+h);//down
		appOsdLib_PIPLayerLineDraw(x+w,   y, x+w, y+h);//right
	}

	sp5kFree(pPageCap);
}


/*-------------------------------------------------------------------------
 *  Function Name : appStillOsd_AllObjectsDraw
 *  Description :	
 *------------------------------------------------------------------------*/
void appStillOsd_AllObjectsDraw(BOOL bShow)
{
	UINT32 i=0;
	UINT32 color = bShow? PIP_COLOR565_WHITE: 0;
	
	for (i = 0; i < objectOsd.totalObjectNum; i++) 
	{	
		appStillOsd_OneObjectDraw(i, color);
	}	
}
/* Add by sc.liu */


//only for osd performace test
void appStillOsd_TestDraw(void)
{
	UINT32 i=0;
	UINT32 t1, t2, tsum;

	t1 = appMsTimeGet();
	for(i=0; i<10; i++)
	{	
		appOsdLib_PIPLayerShapeClear(APP_OSD_SHAPE_RECT, 0, 0, LCD_SIZE_X, LCD_SIZE_Y, 0, 0);
	}
	t2 = appMsTimeGet();
	tsum = (t2-t1)*1000/10;
	printf("clear pip layer time 320x240 = %d/1000 ms\n",tsum);
	
	t1 = appMsTimeGet();
	for(i=0; i<10; i++)
	{	
		appStillOsd_OneFaceDraw(0, PIP_COLOR565_WHITE);
	}
	t2 = appMsTimeGet();
	tsum = (t2-t1)*1000/10;
	printf("draw one face time = %d/1000 ms\n",tsum);

	t1 = appMsTimeGet();
	for(i=0; i<10; i++)
	{	
		appStillOsd_OneFaceDraw(0, 0);
	}
	t2 = appMsTimeGet();
	tsum = (t2-t1)*1000/10;
	printf("clean one face time = %d/1000 ms\n",tsum);

	// area compare
	t1 = appMsTimeGet();
	for(i=0; i<10; i++)
	{
		appOsdLib_PIPLayerShapeDraw(APP_OSD_SHAPE_RECT, 0, 0, 32, 24, 0, 0); 
	}
	t2 = appMsTimeGet();
	tsum = (t2-t1)*1000/10;
	printf("draw 32x24 time = %d/1000 ms\n",tsum);
	
	t1 = appMsTimeGet();
	for(i=0; i<10; i++)
	{	
		appOsdLib_PIPLayerShapeDraw(APP_OSD_SHAPE_RECT, 0, 0, 60, 60, 0, 0); 
	}
	t2 = appMsTimeGet();
	tsum = (t2-t1)*1000/10;
	printf("draw 60x60 time = %d/1000 ms\n",tsum);
	
	t1 = appMsTimeGet();
	for(i=0; i<10; i++)
	{	
		appOsdLib_PIPLayerShapeDraw(APP_OSD_SHAPE_RECT, 0, 0, 320, 240, 0, 0); 
	}
	t2 = appMsTimeGet();
	tsum = (t2-t1)*1000/10;
	printf("draw 320x240 time = %d/1000 ms\n",tsum);	
}

/*-------------------------------------------------------------------------
 *  Function Name : appStillOsd_MainFaceDraw
 *  Description :	
 *------------------------------------------------------------------------*/
void appStillOsd_MainFaceDraw(BOOL isAfOK)
{
	appStillOsd_OneFaceDraw(faceOsd.mainFace, isAfOK==FOCUS_STATUS_OK? PIP_COLOR565_GREEN: PIP_COLOR565_RED);	
}


/*-------------------------------------------------------------------------
 *  Function Name : appStillOsd_UpdateFaceOsd
 *  Description :	
 *------------------------------------------------------------------------*/
UINT32 appStillOsd_UpdateFaceOsd(
	BOOL isClrOnly
)
{
#if CAM_TYPE_CVR
	isClrOnly = isClrOnly;
	return 0;
#else
	UINT32 lastFaceNumRec;

	lastFaceNumRec = faceOsd.totalFaceNum;	
	if(!lastFaceNumRec && isClrOnly) 
	{
		return 0;
	}
	appOsd_GfxOsdRefreshSet(0);
	if (lastFaceNumRec) 
	{
		appStillOsd_AllFacesDraw(FALSE);
	}	

	if(isClrOnly) 
	{
		faceOsd.totalFaceNum = 0;
	}	
	else 
	{
		appFdFaceDataGetEx(&faceOsd);		
		if (faceOsd.totalFaceNum) 
		{
			if(!lastFaceNumRec) 
			{
				appViewOsd_FocusWindowDraw(FALSE, 0);	
			}
			appStillOsd_AllFacesDraw(TRUE);
		}
		else if (lastFaceNumRec)
		{
			appViewOsd_FocusWindowDraw(TRUE, gStillCB.focusStatus);
		}

	}
	appOsd_GfxOsdRefreshSet(1);
	return  faceOsd.totalFaceNum;
#endif
}

/* Add by sc.liu */
/*-------------------------------------------------------------------------
 *  Function Name : appStillOsd_UpdateObjectOsd
 *  Description :	
 *------------------------------------------------------------------------*/
UINT32 appStillOsd_UpdateObjectOsd(
	BOOL isClrOnly
)
{
#if CAM_TYPE_CVR
	isClrOnly = isClrOnly;
	return 0;
#else
	UINT32 lastObjNumRec;

	lastObjNumRec = objectOsd.totalObjectNum;	

	if(!lastObjNumRec && isClrOnly) 
	{
		return 0;
	}
	appOsd_GfxOsdRefreshSet(0);
	if (lastObjNumRec) 
	{
		appStillOsd_AllObjectsDraw(FALSE);
	}	

	if(isClrOnly) 
	{
		objectOsd.totalObjectNum = 0;
	}	
	else 
	{
		appOdObjectDataGetEx(&objectOsd);		
		if (objectOsd.totalObjectNum) 
		{
			if(!lastObjNumRec) 
			{
				appViewOsd_FocusWindowDraw(FALSE, 0);	
			}
			appStillOsd_AllObjectsDraw(TRUE);
		}
		else if (lastObjNumRec)
		{
			appViewOsd_FocusWindowDraw(TRUE, gStillCB.focusStatus);
		}

	}
	appOsd_GfxOsdRefreshSet(1);
	return  objectOsd.totalObjectNum;
#endif
}
/* Add by sc.liu */

/*-------------------------------------------------------------------------
 *  Function Name : appStillOsd_UpdateSmileOsd
 *  Description :	
 *------------------------------------------------------------------------*/
void appStillOsd_UpdateSmileOsd(
	UINT32* nface,
	UINT32* smile,
	BOOL bShow
)
{
#if CAM_TYPE_CVR
	nface = nface;
	smile = smile;
	bShow = bShow;
#else
	UINT32 faceIdx=0;
	
	appOsd_GfxOsdRefreshSet(0);
	appStillOsd_OneFaceDraw(faceIdx, 0);

	if(bShow) 
	{	
		appFdFaceDataGetEx(&faceOsd);
		*nface = faceOsd.totalFaceNum;
		*smile = faceOsd.rectFace[faceIdx].isSmile;
		// draw the new faces if not smile
		if(*nface) 
		{
			appStillOsd_OneFaceDraw(faceIdx, COLOR_GREEN);

		}
	}
	appOsd_GfxOsdRefreshSet(1);
#endif	
}

/*-------------------------------------------------------------------------
 *  Function Name : appStill_GetMainFaceData
 *  Description : This is for AF position information.	
 *------------------------------------------------------------------------*/
BOOL appStill_GetMainFaceData(
	UINT32* x0,
	UINT32* y0,
	UINT32* x1,
	UINT32* y1
)
{
	UINT32 faceIdx = faceOsd.mainFace;
	uiPara_t* puiPara = appUiParaGet();
	
	printf("%s: %d face\n", __FUNCTION__, faceOsd.totalFaceNum);
	if (faceOsd.totalFaceNum != 0 && puiPara->FaceTracking != UI_FACETRACK_OFF) 
	{		
		if(x0!=NULL && y0!=NULL && x1!=NULL && y1!=NULL) 
		{
			*x0 = faceOsd.rectFace[faceIdx].nStartX;
			*y0 = faceOsd.rectFace[faceIdx].nStartY;
			*x1 = faceOsd.rectFace[faceIdx].nStartX+faceOsd.rectFace[faceIdx].nSize;
			*y1 = faceOsd.rectFace[faceIdx].nStartY+faceOsd.rectFace[faceIdx].nSize;
		}	
		return SUCCESS;
	}	
	else 
	{
		return FAIL;
	}	
}

/*-------------------------------------------------------------------------
*  File Name : appViewOsd_ZoomBarProcess
*------------------------------------------------------------------------*/
void 
appViewOsd_ZoomBarProcess(
	UINT32 param
)
{	
	UINT32 curState;

	if (param == ZoomBarCounter) 
	{		
		curState = appActiveStateGet();
		if(curState != APP_STATE_STILL_PREVIEW 
			&& curState != APP_STATE_VIDEO_PREVIEW
			&& curState != APP_STATE_VIDEO_REC)
		{	
			appTimerClear(&ZoomBarCounter);				
			return;
		}
		else if(!IS_VIEW_GROUP(curState) && !IS_VIDEO_GROUP(curState))
		{
			DBG_PRINT("%s: not group\n", __FUNCTION__);
			appTimerClear(&ZoomBarCounter);				
			return;
		}	
	
		appViewOsd_ZoomFrameDraw(FALSE);
		appTimerClear(&ZoomBarCounter);	
		//for overlay with the guid line
		if((IS_PASM_OR_SCENE_MODE && gStillCB.osd==OSD_VIEW_GRID)
			||(IS_VIDEO_MODE && gVideoCB.osd==OSD_VIEW_GRID))
		{
			appOsd_ColorDrawShapeColorSet(PAL_WHITE, PAL_BLEND_100, PAL_WHITE, PAL_BLEND_100);		
			appOsdLib_OsdLayerLineDraw(LCD_SIZE_X-LCD_SIZE_X/3	,0	,LCD_SIZE_X-LCD_SIZE_X/3 ,LCD_SIZE_Y/4);	
		}
	}	
}

/*-------------------------------------------------------------------------
*  File Name : appViewOsd_ZoomBarProcess
*------------------------------------------------------------------------*/
void 
appViewOsd_ZoomBarProcessStart()
{	
	if(ZoomBarCounter != TIMER_NULL) 
	{
		appTimerClear(&ZoomBarCounter);
	}
	ZoomBarCounter=appTimerSet(2000, "ZOOMBAR");
}

/*-------------------------------------------------------------------------
*  Function Name : appViewOsd_ZoomBarDestroy
*  Description : 
*------------------------------------------------------------------------*/
void 
appViewOsd_ZoomBarDestroy(void)
{	
	if(ZoomBarCounter != TIMER_NULL) 
	{
		appTimerClear(&ZoomBarCounter);
		appViewOsd_ZoomFrameDraw(FALSE);
	}
}

/*-------------------------------------------------------------------------
 *  Function Name : appViewOsd_AllClean
 *  Description : 
*------------------------------------------------------------------------*/
void 
appViewOsd_AllClean(void)
{	
	
	CLEAR_OSD_SRC;
	CLEAR_PIP_SRC;
	ViewOsdPresentStatus=0;	
}

/*-------------------------------------------------------------------------
 *  Function Name : appViewOsd_RefreshEffectTest
 *  Description : snap osdt2 
*------------------------------------------------------------------------*/
void 
appViewOsd_RefreshEffectTest(void)
{
	int i, j;
	UINT32 t1, t2;

	APP_OSD_REFRESH_ON;
	
	for(j=0; j<10; j++) 
	{
		t1=appMsTimeGet();
		for(i=0; i<1000; i++) 
		{
			appOsd_FontIconFontSet(ID_FONT_ICON);
			appOsdLib_OsdLayIconDraw(APPVIEW_SHAKE_X,APPVIEW_SHAKE_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_SCENE_SHAKEFREE_OFF);
		}
		t2=appMsTimeGet();
		printf("normal draw 1000 icons (%d) = %dms\n", j, t2-t1); 
	}	

	APP_OSD_REFRESH_OFF;	

	for(j=0; j<10; j++) 
	{
		t1=appMsTimeGet();
		for(i=0; i<1000; i++)
		{
			appOsd_FontIconFontSet(ID_FONT_ICON);
			appOsdLib_OsdLayIconDraw(APPVIEW_SHAKE_X,APPVIEW_SHAKE_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_SCENE_SHAKEFREE_OFF);
		}
		t2=appMsTimeGet();
		printf("no-refresh 1000 draw icon (%d) = %dms\n", j, t2-t1); 	
	}	
	
	APP_OSD_REFRESH_ON;
}

/*-------------------------------------------------------------------------
 *  Function Name : appViewOsd_ModeIconTest
 *  Description : snap osdt2 
*------------------------------------------------------------------------*/
void 
appViewOsd_ModeIconTest(void)
{
	int i, j;
	UINT32 t1, t2;

	APP_OSD_REFRESH_OFF;	
	for(j=0; j<10; j++) 
	{
		appViewOsd_AllClean();
		t1=appMsTimeGet();
		for(i=0; i<100; i++) 
		{
			appViewOsdShow(0,0, OSD_VIEW_SIMPLE);
		}
		t2=appMsTimeGet();
		printf("refres off x100 viewosdinit (%d) = %dms\n", j, t2-t1); 	
	}
	APP_OSD_REFRESH_ON;
	for(j=0; j<10; j++) 
	{
		appViewOsd_AllClean();
		t1=appMsTimeGet();
		for(i=0; i<1000; i++) 
		{
			appViewOsdShow(0,0, OSD_VIEW_SIMPLE);
		}
		t2=appMsTimeGet();
		printf("refres on x100 viewosdinit (%d) = %dms\n", j, t2-t1); 	
	}
}

/*-------------------------------------------------------------------------
 *  Function Name : appViewOsd_DriveModeCountDownDraw
 *  Description : 
*------------------------------------------------------------------------*/

void appViewOsd_DriveModeCountDownDraw(UINT8 Second)
{
	if(Second>11) 
	{
		return;
	}
	else if(!Second) 
	{
	      #if KIT_WITH_HDMI     /*Add by Lation@20130717*/
		if(IS_HDMI_IN){
			/*
			appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
			appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_CIRCLE,112,72,0,0,96,96);*/
			appOsd_ColorTransparentShapeColorSet(PAL_NOCOLOR,PAL_NOCOLOR,PAL_BLEND_100);
			sp5kGfxCircleDraw(0+1,320,180,69);
		}
		else{
			/*
			appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
			appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_CIRCLE,112,72,0,0,96,96);*/
			appOsd_ColorTransparentShapeColorSet(PAL_NOCOLOR,PAL_NOCOLOR,PAL_BLEND_100);
			sp5kGfxCircleDraw(0+1,160,120,46);
		}
		return;
		#endif
	}

	#if KIT_WITH_HDMI   /*Add by Lation@20130717*/
	if(IS_HDMI_IN)
	{
		appOsd_ColorDrawShapeColorSet(PAL_BLACK, PAL_BLEND_100, PAL_BLACK, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, 275, 148, 90, 63, 0, 0);
		appOsd_ColorBlendingSet(PAL_GRAY_1,PAL_BLEND_50);
		appOsd_FontIconFontSet(ID_FONT_TK_TIME_HDMI);
		appOsd_ColorTransparentShapeColorSet(PAL_GRAY_1,PAL_GRAY_1,PAL_BLEND_50);
		sp5kGfxCircleDraw(1,320,180,69);
		appOsdLib_OsdLayIconDraw(248,108,SP5K_GFX_ALIGN_TOP_LEFT,Second);
	}
	else
	{
		/*appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, 130, 99, 60, 42, 0, 0);*/
		appOsd_ColorBlendingSet(PAL_GRAY_1,PAL_BLEND_50);
		appOsd_FontIconFontSet(ID_FONT_TK_TIME);
		appOsd_ColorTransparentShapeColorSet(PAL_GRAY_1,PAL_GRAY_1,PAL_BLEND_50);
		sp5kGfxCircleDraw(1,160,120,46);
		appOsdLib_OsdLayIconDraw(112,72,SP5K_GFX_ALIGN_TOP_LEFT,Second);
	}
	#endif
}

/*-------------------------------------------------------------------------
 *  Function Name : appViewOsd_BlackScreenDraw
 *  Description : 
*------------------------------------------------------------------------*/
void 
appViewOsd_BlackOsdLayerDraw(void)
{
	printf("%s\n", __func__);	
	appOsd_ColorDrawShapeColorSet(PAL_BLACK, PAL_BLEND_100, PAL_BLACK, PAL_BLEND_100);
	appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, 0, 0, LCD_SIZE_X, LCD_SIZE_Y, 0, 0);	
	APP_SLEEP_MS(20);
	//appJpgFileDraw(SP5K_RES_TYPE_JPG, RES_JPG_BLACK, 0, 0, 0, 0);
}

/*-------------------------------------------------------------------------
*	 Func. Name : appViewOsd_DlgSetandDraw()
*------------------------------------------------------------------------*/
void appViewOsd_DlgSetandDraw( unsigned short wTitle,  unsigned short SelectIndex)
{
      wTitle= wTitle; /*avoid warning*/

  
	appPlayOsd_DlgConfirmYesNoConfig(ID_STR_IMAGE_SAVED,SelectIndex);
	appPlayOsd_DlgBoxSetandShow(APPVIEW_BLINK_DLG_START_X,APPVIEW_BLINK_DLG_START_Y,APPVIEW_BLINK_DLG_1STITEM_X, APPVIEW_BLINK_DLG_1STITEM_Y,APPVIEW_BLINK_DLG_ITEM_GAP);			

}
void appViewOsd_WiFiIconDraw(COLOR_DEF color)
{
	appOsd_ColorDrawTextColorSet(color, PAL_NOCOLOR, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_TextDraw(APPVIEW_WIFI_STRING_X, APPVIEW_WIFI_STRING_Y, SP5K_GFX_ALIGN_TOP_LEFT,"WiFi");
}


