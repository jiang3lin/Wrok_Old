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

#include "sp5k_global_api.h"
#include "sp5k_ae_api.h"
#include "sp5k_capture_api.h"
#include "sp5k_disp_api.h"
#include "sp5k_disk_api.h"
#include "sp5k_media_api.h"

#include "sp5k_dzoom_api.h"
#include "app_still.h"
#include "app_view_param.h"
#include "app_view_osd.h"
#include "app_osdfont.h"

#include "app_icon.h"
#include "app_icon_def.h"
#include "app_osd_api.h"
#include "app_osd.h"
#include "app_error_msg.h"
#include "app_ui_para.h" 

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define PANORAMA_STATUS_NORMAL 0
#define PANORAMA_STATUS_ABOURT 1
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef struct Panorama_s {
	UINT32 aeb;
	UINT32 status;
	UINT32 imgId; 		/* message IMAGE_ID accumulate number */
}Panorama_t;
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
Panorama_t gPanoramaCB; 	/* Panorama control block */
UINT16 total_panorama;

static struct {
	UINT16 pvw;
	UINT16 pvh;
	UINT16 CapImgW_LR;
	UINT16 CapImgH_LR;
	UINT16 CapImgW_UD;
	UINT16 CapImgH_UD;
} PanoramapvSize[3]={
	{ 1920,1080,7152,1080,1920,4912},
	{ 1280, 720,4768,720,1280,3280},
	{ 640, 480,3168,480,640,1648},
};

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern void sp5kStillCapPanoramaSizeCfg(
		UINT32 hSize,	/* image width for horizontal stitch, size should be 16n */
		UINT32 vSize	/* image height for vertical stitch, size should be 16n */
	);
extern UINT32 sp5kStillCapPanoramaNumGet(UINT8 direction);
extern void sp5kStillCapPanoramaStart(UINT8 opMode,UINT8 direction);

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
void appStillPanorama_SizeSet(UINT8 angle ,UINT32 id)
{
	angle =0; 
	if ((id ==UI_PV_PANORAMA_LR)||(id ==UI_PV_PANORAMA_RL)){
		sp5kStillCapPanoramaSizeCfg(PanoramapvSize[1].CapImgW_LR,PanoramapvSize[1].CapImgH_LR);	
	
	}else{
		sp5kStillCapPanoramaSizeCfg(PanoramapvSize[1].CapImgW_UD,PanoramapvSize[1].CapImgH_UD);	
	}
	
} 
void 
appStillPanoramaProgressBar(void)
{
	#if PANORAMA_MODE
	appOsd_FontIconFontSet(ID_FONT_ICON);
	appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
	switch(pViewParam->panorama){
		case 0:
		case 1:
		case 2:
		case 3:
			appOsdLib_OsdLayerShapeClear(APP_OSD_SHAPE_RECT, 20, 190,5*total_panorama+51, 40, 0, 0);
			appOsd_ColorDrawShapeColorSet(PAL_BLACK, PAL_BLEND_100, PAL_BLACK, PAL_BLEND_100);
			appOsd_GfxRectShapeDraw(SP5K_GFX_PAGE_OSD_0,20, 220,5*total_panorama+45,230);
			appOsd_ColorDrawShapeColorSet(PAL_RED, PAL_BLEND_100, PAL_RED, PAL_BLEND_100);
			appOsd_GfxRectShapeDraw(SP5K_GFX_PAGE_OSD_0,22+gPanoramaCB.imgId*5,222,42+gPanoramaCB.imgId*5, 228);
			break;
		
		}
			
	APP_OSD_REFRESH_ON;
    #endif

}

void 
appStillPanoramaStateInit(void)
{	
	#if PANORAMA_MODE
	/*UINT32 ret=0;*/
	/*sp5kImgSize_t captSize={1280,720};*/
	/*extern halCdspPrevBufSizeCfg(UINT32 hsize,UINT32 vsize);*/
	sp5kAeModeSet(SP5K_AE_MODE_INFO_ONLY);

	sp5kZoomFactorSet(SP5K_MODE_STILL_SNAP, 100);
	//appAeCaptureSet(0);
	//sp5kStillCapCfgSet(SP5K_CAPTURE_MAIN_IMAGESIZE, &captSize);

	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_WIDTH, PanoramapvSize[1].pvw);/*0:1080P,1,720P,2:VGA*/
	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_HEIGHT, PanoramapvSize[1].pvh);/*0:1080P,1,720P,2:VGA*/	

	appModeSet(SP5K_MODE_VIDEO_PREVIEW);

	appStillPanorama_SizeSet(0 ,pViewParam->panorama);
	total_panorama=sp5kStillCapPanoramaNumGet(pViewParam->panorama+1);
	/*appAeAebSet(puiPara->AEB);  */
	/* we miss the s2 release message */
	if(gStillCB.s2press == 0) {
		sp5kHostMsgSend(APP_KEY_RELEASE_S2, 0);
	}
	gStillCB.isReady2View = 0;
	gPanoramaCB.status = PANORAMA_STATUS_NORMAL;
	gPanoramaCB.imgId = 0;
	//gPanoramaCB.aeb = puiPara->AEB;
	#if 0
	#if CAP_AUTO_ENABLE
	sp5kStillCapAdvCfgSet(SP5K_CAPTURE_COMPRESSION_DELAY, 1);
	#else
	sp5kStillCapAdvCfgSet(SP5K_CAPTURE_COMPRESSION_DELAY, -1);
	#endif
	#endif	
	//printf("~~~~~~~~~~~~~~~~~~~~~~total_panorama=%d\n",total_panorama);
	//ret = sp5kStillCapCfgSet(SP5K_CAPTURE_BURST_LENGTH, &total_panorama);
	profLogAdd(0, "Panomrma sp5kModeSet()");
	//appStillUtil_CaptureSet();
	profLogPrintf(0 ,"start %s snap\n", gPanoramaCB.aeb? "AEB":"Panorama");
	sp5kStillCapPanoramaStart(0,pViewParam->panorama+1);
	appViewOsd_AllClean();
	#endif
}

void 
appStillPanoramaStateOnMsg(
	UINT32 msg,
	UINT32 param
)
{	
    #if PANORAMA_MODE
	UINT32 next_state,next_param;	
	
	switch (msg) {
	case SP5K_MSG_STILL_CAPTURE_IMAGE_ID:
		profLogAdd(0, "Panorama IMAGE_ID");
		gPanoramaCB.imgId++;
		appStillPanoramaProgressBar();
		break;
	case SP5K_MSG_STILL_QV_TIMEOUT:
		DBG_PRINT("Cap Panorama msg: QV_TIMEOUT\n");
		break;
	case SP5K_MSG_STILL_VIEW_SETTING_NOTIFY:
		break;
	case SP5K_MSG_STILL_STORE_FAIL:	
		profLogAdd(0, "Panorama SP5K_MSG_STILL_STORE_FAIL");
		appViewOsd_AllClean();	
		appOsd_ColorDrawTextColorSet(PAL_RED, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
		appOsd_GfxStringDraw(160, 120, SP5K_GFX_ALIGN_CENTER, "FAIL ,MOVE ERROR !!");
		sp5kDispDevAttrSet(SP5K_DISP_OSD_ACTIVE, 1, 0, 0, 0);
		appTimeDelay(2000,1);
	case SP5K_MSG_STILL_STORE_ALL_JOB_COMPLETE :
		profLogAdd(0, "Panorama SP5K_MSG_STILL_STORE_ALL_JOB_COMPLETE");
		sp5kModeSet(SP5K_MODE_STILL_PREVIEW);
		sp5kDispDevAttrSet(SP5K_DISP_OSD_ACTIVE, 1, 0, 0, 0);
		sp5kDispDevAttrSet(SP5K_DISP_IMG_ACTIVE, 1, 0, 0, 0);
		break;	
		DBG_PRINT("Cap Panorama msg: VIEW_SETTING_NOTIFY\n");
		if(gPanoramaCB.status == PANORAMA_STATUS_ABOURT) {
			appTimeDelay(100,1);
			if(gPanoramaCB.imgId < total_panorama){
				/*UINT32 ii;*/
				//for(ii=0;ii<6000000;ii++) ;				
				appViewOsd_AllClean();	
				appOsd_ColorDrawTextColorSet(PAL_RED, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
				appOsd_GfxStringDraw(160, 120, SP5K_GFX_ALIGN_CENTER, "FAIL ,MOVE ERROR !!");
				sp5kDispDevAttrSet(SP5K_DISP_OSD_ACTIVE, 1, 0, 0, 0);
				appTimeDelay(2000,1);
				printf("\n~~~~~~~show error message\n");
			}	
			appStill_SetSp5kStillSize(UI_PIC_SIZE_FULL);			
			appStillViewIqSet();			
			appAePreviewSet();			
			appAePreviewRestore();			
			appOsdLib_PIPLayerShapeClear(APP_OSD_SHAPE_RECT, 0, 0, LCD_SIZE_X, LCD_SIZE_Y, 0, 0);
			sp5kModeSet(SP5K_MODE_STILL_PREVIEW);
			appStillFlow_GetNextState(&next_state, &next_param);
			appStateChange(next_state, next_param);
			sp5kDispDevAttrSet(SP5K_DISP_OSD_ACTIVE, 1, 0, 0, 0);
			sp5kDispDevAttrSet(SP5K_DISP_IMG_ACTIVE, 1, 0, 0, 0);
		}	
		break;
	case SP5K_MSG_MODE_READY:
		printf("Cap Panorama msg: MODE_READY\n");
		if (param==SP5K_MODE_STILL_PREVIEW) {
			appStillYuvCbSet(FALSE);
			appStillFlow_GetNextState(&next_state, &next_param);
			appStateChange(next_state, next_param);
		}
		break;	
	case APP_UI_MSG_VIEW_YUV_CALLBACK:
		break;	
	default:
		break;
	}	
	#endif
}

void 
appStillPanoramaStateOnKey(
	UINT32 msg,
	UINT32 param
)
{
    #if PANORAMA_MODE
	switch (msg) {
	case APP_KEY_RELEASE_S2:
		if( (!IsBracketMode())) {
			gPanoramaCB.status = PANORAMA_STATUS_ABOURT;
		}
		break; 
	default:
		break;
	}
	#endif
}

void 
appStillPanoramaState(
	UINT32 msg,
	UINT32 param
)
{	
#if PANORAMA_MODE

	//printf("~~~~~~~~~~~~~~~~~~~PANORAMA =%x~~~~~%x\n",msg,param);
	switch (msg) {
	case APP_STATE_MSG_INIT:		
		RPrintf("appStillPanoramaState");
		appStillPanoramaStateInit();		
		break;
	case APP_STATE_MSG_CLOSE:
		//sp5kStillCapAbort();
		appStateCloseDone();
		break;	
	default:
		if (IS_APP_KEY_MSG(msg)) {
			appStillPanoramaStateOnKey(msg,param);
		}
		else {
			appStillPanoramaStateOnMsg(msg,param);
		}
		break;
	}	
#endif
}


