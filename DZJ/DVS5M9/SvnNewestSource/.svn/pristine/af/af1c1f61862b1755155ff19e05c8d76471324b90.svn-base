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
#define HOST_DBG 0
#include "app_com_def.h"
#include "app_com_api.h"
#include "sp5k_global_api.h"
#include "sp5k_ae_api.h"
#include "sp5k_awb_api.h"
#include "sp5k_disp_api.h"
#include "sp5k_dcf_api.h"
#include "app_playback_private.h"
#include "app_playback_def.h"
#include "app_playback_osd.h"
#include "app_lens_api.h"
#include "app_dpof.h"
#include "app_view_osd.h"
#include "app_menu_disp.h" /*for clean warning*/
#if SPCA5310
#include "api/sp5k_pip_api.h"
#endif
#include "app_still.h" /*added by Leo @20100727*/
#include "app_ui_para.h"
#include "app_cdfs_api.h"
#include "app_menu.h"
#include "app_msg.h"  
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define PB_LENS_CLOSE_TIME		15*1000 /* unit: ms */
#define WRITE8(a,b)	*(volatile UINT8*)(a) = (b)


/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

typedef enum {
	PB_MAIN_SXN_START,
	PB_MAIN_SXN_END,
} pbMainKeySessionDef_t;

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

#if (PB_PRIVATE_DBG_MASTER)
UINT8 szOsdStr[PB_TMP_OSD_STR_SIZE];
#endif
static BOOL pbflag = 0;
pbUIParam_t gpbUiParam = {0}; /* Use appPb_UiParamPtrGet() for extern reference */
pbMainCB_t	gpbMainCb = {0};	  /* Use appPb_MainCbPtrGet() for extern reference */
pbImgCb_t	gpbImgCb = {0};   /* Use appPb_ImageCbPtrGet() for extern reference */
pbDiskCB_t	gpbDiskCb;
pbImgExifInfo_t gpbExifInfo; /* Use appPb_ExifInfoPtrGet() for extern reference */

static BOOL gpbIsViewIdle = TRUE; /* Use appPb_IsViewIdle() for extern reference */
UINT8 trimCount;
UINT8 fileTypeSelect;
UINT32 gpbHistBuf[PB_HISTOGRAM_BAR_NUM]; /* the Histogram buffer */
//extern BOOL audioBGDraw;
BOOL gProtectAllFinish = FALSE;
BOOL seamlessPlayEn = FALSE;

#if HDMI_MENU_BY_JPG
extern UINT32 pipHDMIMenuFlag;
#endif
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
static UINT32 appPbMainEnter(UINT32 playImgIdx);

/*-------------------------------------------------------------------------
 *  File Name : appPb_IsViewIdle
 *------------------------------------------------------------------------*/
BOOL appPb_IsViewIdle(void)
{
    printf("~~~~~gpbIsViewIdle=%d\n",gpbIsViewIdle);
	return gpbIsViewIdle;
}

/*-------------------------------------------------------------------------
 *  File Name : appMediaFileUpLoad
 *------------------------------------------------------------------------*/
UINT32
appMediaFileUpLoad(
	UINT8 *filename
)
{
    printf("~~~~~~~~~%s~~~~~~~~~~\n\n", filename);
	return 0;
}


/*-------------------------------------------------------------------------
 *  File Name : appPb_ExifInfoPtrGet
 *------------------------------------------------------------------------*/
pbImgExifInfo_t *appPb_ExifInfoPtrGet(void)
{
	return &gpbExifInfo;
}


/*-------------------------------------------------------------------------
 *  File Name : appPb_ImageCbPtrGet
 *------------------------------------------------------------------------*/
pbImgCb_t *appPb_ImageCbPtrGet(void)
{
	return &gpbImgCb;
}

/*-------------------------------------------------------------------------
 *  File Name : appPb_ImageCbPtrGet
 *------------------------------------------------------------------------*/
pbUIParam_t *appPb_UiParamPtrGet(void)
{
	return &gpbUiParam;
}

/*-------------------------------------------------------------------------
 *  File Name : appPb_ImageCbPtrGet
 *------------------------------------------------------------------------*/
pbMainCB_t *appPb_MainCbPtrGet(void)
{
	return &gpbMainCb;
}

/*-------------------------------------------------------------------------
 *  File Name : appPb_ImageCbPtrGet
 *------------------------------------------------------------------------*/
pbDiskCB_t *appPb_DiskCbPtrGet(void)
{
	return &gpbDiskCb;
}


/*-------------------------------------------------------------------------
 *  File Name : pbmainUIKeyControl
 *------------------------------------------------------------------------*/
static
void
pbmainUIKeyControl(
	UINT8 ctrl
)
{
	switch (ctrl){
	case PB_MAIN_SXN_START:
		appBtnDisable(BTN_LEFT|BTN_RIGHT);
		break;
	case PB_MAIN_SXN_END:
		appBtnEnable(BTN_LEFT|BTN_RIGHT);
		break;
	default:
		PB_VERBOSE("ZPan: undefined key session !!\n");
		break;
	}
}

/*-------------------------------------------------------------------------
 *  File Name : pbmainKeyPreCheck
 *------------------------------------------------------------------------*/
static
UINT32
pbmainKeyPreCheck(
	UINT32 keyMsg
)
{
	PB_VERBOSE("%s: keyMsg(0x%08x) \n", __FUNCTION__, keyMsg);

	if (gpbIsViewIdle==FALSE || gpbMainCb.isIniting==TRUE) 
	{
		PB_VERBOSE("%s,%d: Initializing, ignore key (0x%08x) !! \n", __FUNCTION__, __LINE__, keyMsg);
		return FAIL;
	}

	if (gpbImgCb.isFileErr)
	{
		if (keyMsg==APP_KEY_PRESS_TELE && keyMsg==APP_KEY_PRESS_WIDE) 
		{
			PB_VERBOSE("%s,%d: Illegal key (0x%08x) \n", __FUNCTION__, __LINE__, keyMsg);
			return FAIL;
		}
	}
	return SUCCESS;

}


/*-------------------------------------------------------------------------
 *  File Name : playInitCreate
 *------------------------------------------------------------------------*/
static
void
playInitCreate(
	void
)
{
	
	
	PB_VERBOSE("%s \n", __FUNCTION__);
#if CAM_TYPE_CVR
	uiPara_t* puiPara = appUiParaGet();
#endif

	memset(&gpbMainCb, 0, sizeof(pbMainCB_t));
	memset(&gpbDiskCb, 0, sizeof(pbDiskCB_t));

	gpbImgCb.fileType = gpbMainCb.lastFileType = PB_FT_UNKNOWN;
	gpbMainCb.closeLensTmrId = TIMER_NULL;
	gpbMainCb.asyncOpTmrId = TIMER_NULL;
	gpbMainCb.isIniting = TRUE;
     
	sp5kExifCfgSet(SP5K_EXIF_CFG_MPF_SUPPORT, 1); /* support mpf big thumb */
	sp5kDcfFsFileTypeSet(APP_PB_FILETYPE_ALL);
	
	if(gpbMainCb.totalImg != 0)
	{
		sp5kZoomFactorSet(SP5K_MODE_STILL_PLAYBACK, 1000);
	}
		
	#if PLAYBACK_ROTATE_BY_EXIF_TAG	
	sp5kStillPlayCfgSet(SP5K_SPB_QV_ROTATE_BASE,SP5K_SPB_QV_ROTATE_BY_THUMB);
	sp5kStillPlayCfgSet(SP5K_SPB_THUMB_ROTATE_BASE,SP5K_SPB_THUMB_ROTATE_BY_PRIMARY);
	sp5kStillPlayCfgSet(SP5K_SPB_PRIMARY_ROTATE_BASE,SP5K_SPB_PRIMARY_ROTATE_BY_PRIMARY);
	#endif
	
 	#if (KIT_WITH_HDMI)
	if (IS_HDMI_IN)
	{
	#if PIP_32M 
		sp5kStillPlayCfgSet(SP5K_SPB_RING_BUFFER_MAX_NUM,6);
	#else
		sp5kStillPlayCfgSet(SP5K_SPB_RING_BUFFER_MAX_NUM,9);
	#endif 
		sp5kModeSet(SP5K_MODE_STANDBY);
		sp5kModeWait(SP5K_MODE_STANDBY);
		appPbPIPInit();
	}
	else 
	{
		sp5kStillPlayCfgSet(SP5K_SPB_RING_BUFFER_MAX_NUM,9);
    #if CAM_TYPE_CVR
		if(puiPara->fileType != FILETYPE_JPG)  
		{
			sp5kModeCfgSet(SP5K_MODE_CFG_STANDBY_DUP, 1);
			sp5kModeSet(SP5K_MODE_STANDBY);
			sp5kModeWait(SP5K_MODE_STANDBY);
			appPbPIPInit(); 
		}
	#endif	
	}
	#endif
 }


/*-------------------------------------------------------------------------
 *  File Name : playDeviceConfig
 *------------------------------------------------------------------------*/
static
void
playDeviceConfig(
	void
)
{
	UINT32 appPrevStat;

	appPrevStat = appPreviousStateGet();
	/* %TODO% */
	/*-- AE/AWB */
	if (STATE_GROUP(appPrevStat)==APP_STATE_GROUP_VIEW ||
		STATE_GROUP(appPrevStat)==APP_STATE_GROUP_VIDEO  )
	{
		sp5kAeModeSet(SP5K_AE_MODE_OFF);
		sp5kAwbModeSet(SP5K_AWB_MODE_OFF);
	}
	/*-- Button Control */

	/*-- LEDs Control */

	/*-- Disable strobe charge */

	/*-- Lens control */

	/*-- DISP control */
}


/*-------------------------------------------------------------------------
 *  File Name : appPbMPlayCreate
 *------------------------------------------------------------------------*/
static
UINT32
appPbMPlayCreate(
	void
)
{
	UINT32 ret = 0;
	PB_VERBOSE("appPbMPlayCreate \n");

	gpbMainCb.mainStat = PB_MAIN_STAT_MPLAY_SUSPEND;

	appModeSet(SP5K_MODE_STANDBY);

	PB_VERBOSE("MPlayCreate: Changing state to MPlay \n");
	appStateChange(APP_STATE_PB_MPLAY, STATE_PARAM_NORMAL_INIT);

	return ret;
}

/*-------------------------------------------------------------------------
 *  File Name : appPbZoomPanCreate
 *------------------------------------------------------------------------*/
static
UINT32
appPbZoomPanCreate(
	void
)
{
	PB_VERBOSE("appPbZoomPanCreate \n");

	/*-- update state flags */
	gpbMainCb.mainStat = PB_MAIN_STAT_ZPAN_SUSPEND;
	
	/*-- Main image check */
	#if KIT_WITH_HDMI
	if (!gpbImgCb.isMainImgOK || IS_HDMI_IN)
	#else
	if (!gpbImgCb.isMainImgOK)
	#endif
	{
		PB_DBG("ZPanCreate: Main not ready yet !!\n");
		/*-- Main image has not ready yet !! */
		#if KIT_WITH_HDMI
		if (!IS_PB_MAIN_PLAYING || IS_HDMI_IN) 
		#else
		if (!IS_PB_MAIN_PLAYING)
		#endif
		{
			/* main play not yet fired */
		//	appPbJpgMainPlayStart();
		}
		#if (KIT_WITH_HDMI)
		if(IS_HDMI_IN)
			gpbMainCb.subStat = PB_ZPAN_STAT_STANDBY;
		else
		#endif	
			gpbMainCb.subStat = PB_ZPAN_STAT_WAIT_MAIN_IMG;
	} 
	else 
	{
		gpbMainCb.subStat = PB_ZPAN_STAT_STANDBY;
		
		PB_VERBOSE("ZPanCreate: Changing state to ZPAN \n");
		appStateChange(APP_STATE_PB_ZOOM, STATE_PARAM_NORMAL_INIT);
	}

	return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  File Name : appPbMainStIamgeShift
 *------------------------------------------------------------------------*/
static
void
appPbMainStImageShift(
	UINT8 shiftCtrl
)
{
	UINT32 osdWidth,osdHeight,iconWidth,iconHeight;
	PB_DBG("PbMain: Shift (%s) \n", (shiftCtrl==PB_SHIFT_NEXT ? "NEXT" : "PRE"));

	pbmainUIKeyControl(PB_MAIN_SXN_START);

	switch (shiftCtrl) {
	case PB_SHIFT_NEXT:
	case PB_SHIFT_PRE:
		/*-- Clear center msg before shifting image */
		if (gpbImgCb.isFileErr) 
		{
			appPbMsg_Clear();
		}

		//PB_BEEP;
		#if KIT_WITH_HDMI
		if(IS_HDMI_IN){
			osdWidth   = 640;
			osdHeight  = 360;
			iconWidth  = 48;
			iconHeight = 38;
		}else
		#endif
		{
			osdWidth   = 320;
			osdHeight  = 240;
			iconWidth  = 32;
			iconHeight = 28;
		}
		//appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);/*for mantis bug 0047439*/
		//appOsdLib_OsdLayerShapeClear(APP_OSD_SHAPE_RECT,0, iconHeight+10,osdWidth, osdHeight-iconHeight-10, 0, 0);

		if (FAIL==appPbImageShift(shiftCtrl)) 
		{
			/*-- For PbMain, the only shift_error shall be FILE_ERROR !!
			 * otherwise, it should be a BUG !?
			 */
			appPbErrorProcess(PB_ERR_FILE_ERROR);
		}

		/*-- UPDATE UI 
		if((gpbImgCb.fileType == PB_FT_AVI)||(gpbImgCb.fileType == PB_FT_WAV))
		{
			if(gpbMainCb.osdType==PB_DISP_FULL)
			{
				gpbMainCb.osdType = PB_DISP_BASIC;
			}
		}*/
		
		if (gpbMainCb.postponeUI==0) 
		{
			appPlayOsd_MainInfoDraw(PB_OSD_DRAW_NORMAL);
			PB_LOG("MainInfoDraw @ %s,%d \n", __FUNCTION__, __LINE__);
		} 
		else 
		{
			gpbMainCb.postponeUI=2; /* PB_OSD_DRAW_NORMAL */
		}
		/*-- Emulate Key Repeat */
		if((gpbImgCb.fileType == PB_FT_AVI)||(gpbImgCb.fileType == PB_FT_WAV)||(gpbImgCb.fileType == PB_FT_MOV)||(gpbImgCb.fileType == PB_FT_MP4))
		{
			appPbImageShiftRepeatCheck(0);
		}
		break;
	default:
		PB_DBG("PbMain: undefined shiftCtrl(%d) \n", shiftCtrl);
		break;
	}

	pbmainUIKeyControl(PB_MAIN_SXN_END);

}

/*-------------------------------------------------------------------------
 *  File Name : appPbMainEnter
 *------------------------------------------------------------------------*/
static
UINT32
appPbMainEnter(
	UINT32 playImgIdx
)
{
	/*-- argument playImgIdx is used to override the
	 * DCF image index to play at creation.
	 * If playImgIdx==0, use the last image for play,
	 * otherwise, use designated index.
	 */

	PB_VERBOSE("%s: playImgIdx(%d) \n", __FUNCTION__, playImgIdx);
   
	/*-- while calling appPbMainEnter(),
	 * DISK shall have been mounted !!
	 */
	trimCount =0 ;
	/*-- check up Disk mounted successfully !! */
	if (gpbDiskCb.isDiskErr)
	{
		gpbMainCb.err = PB_ERR_DISK_ERROR;
		appPbErrorProcess(PB_ERR_DISK_ERROR);
		return FAIL;
			/*-- Return FAIL to indicate that appPbMainEnter()
			 * did not play any image.
			 */
	}

	/*-- Checkup & Renew Disk status */
	appPbDiskCheckup(&gpbDiskCb);

	/*-- check up IMG count */
	gtotalImgCount = appPbTotalFileCountGet();
	if (!gtotalImgCount) 
	{
		gpbMainCb.err = PB_ERR_NO_IMAGE;
		appPbErrorProcess(PB_ERR_NO_IMAGE);
		#if CAM_TYPE_CVR
		gpbMainCb.isPlayDone = FALSE;
		#endif
		return FAIL;
			/*-- Return FAIL to indicate that appPbMainEnter()
			 * did not play any image.
			 */
	}
	gpbMainCb.isNoImage = FALSE;

	/*-- Go on & playback image */
	playImgIdx = (playImgIdx==0 ? gtotalImgCount : playImgIdx);

	PB_LOG("[PbMain] Playing %d of %d ...\n", playImgIdx, gtotalImgCount);

	gpbImgCb.isFileErr = FALSE;
	if (FAIL==appPbImagePlay(playImgIdx)) 
	{
		gpbMainCb.err = PB_ERR_FILE_ERROR;
		gpbImgCb.isFileErr = TRUE;/*willow++ for clear the video"file error" when shift*/
		appPbErrorProcess(PB_ERR_FILE_ERROR);
		return FAIL;
			/*-- Return FAIL to indicate that appPbMainEnter()
			 * did not play any image.
			 */
	}
	
	gpbMainCb.postponeUI=0;
	if (IS_FILETYPE_JPG(gpbImgCb.dcfAttr.fileType)) 
	{
		/*-- Playing JPG image, shall postpone UI OSD
		 * till the PLAYBACK_DONE message.
		 */
		gpbMainCb.postponeUI=1;
	}
     
	/*appSensorPowerDownSet(TRUE);*/
	sensorDevPowerCustomCb(SENSOR_DEV_POWER_ID_SYS_OFF);


	gpbMainCb.err = PB_ERR_NONE;
	return SUCCESS;
			/*-- Return SUCCESS to indicate that appPbMainEnter()
			 * has fired the image playback.
			 */

}

/*-------------------------------------------------------------------------
 *  File Name : appPbMainInit
 *------------------------------------------------------------------------*/
static
UINT32
appPbMainInit(
	void
)
{
	//UINT32 appPrevStat;
	//UINT32 pbErr, ret;
	uiPara_t* puiPara = appUiParaGet();
	PB_LOG("~~~~%s: mainStat(%d) \n", __FUNCTION__, gpbMainCb.mainStat);

	PB_CLR_ALL;	

	gpbMainCb.postponeUI=0;
		/* Do need to wait playback_done msg for UI update by default */

	switch (gpbMainCb.mainStat)
	{
	case PB_MAIN_STAT_NULL:
		PB_DBG("PB_MAIN_STAT_NULL \n");
		/*-- Enter PbMain State */
		APP_OSD_REFRESH_OFF;
		appPbMainEnter(0);
      #if CAM_TYPE_CVR
		if(IS_CARD_EXIST && (gtotalImgCount!=0) && 
		   (gpbMainCb.err != PB_ERR_DISK_ERROR) && 
		   puiPara->fileType != FILETYPE_JPG)
		{	   
		   appPlayOsd_VideoInfoDraw();
		}
      #endif
	   APP_OSD_REFRESH_ON;
		break;

	case PB_MAIN_STAT_MENU_SUSPEND:
		PB_DBG("PB_MAIN_STAT_MENU_SUSPEND \n");

		#if HDMI_MENU_BY_JPG
		#if KIT_WITH_HDMI   /*Add by Lation@20130717*/
		if(IS_HDMI_IN){
			appPbMainEnter(0);
			break;
		}
		#endif
		#endif
	#if CAM_TYPE_CVR
		if (gtotalImgCount != 0) 
		{
			/*-- PB Menu is returning from different image.
			 * We shall replay the last image.
			 */
			APP_OSD_REFRESH_OFF;
			appPbMainEnter(gcurrDcfIdx);
			if(puiPara->fileType != FILETYPE_JPG){
				appPlayOsd_VideoFileIconDrow(TRUE);
				appPlayOsd_VidFileNameChangeDrow();
			}
			APP_OSD_REFRESH_ON;
			break;
		} 
 	#else
		if (gpbMainCb.lastImgIdx != gcurrDcfIdx) 
		{
			/*-- PB Menu is returning from different image.
			 * We shall replay the last image.
			 */
			appPbMainEnter(0);
			break;
		} 
	#endif
		else 
		{
			/*-- The MENU return to same IMG.
			 * Do not need to replay image.
			 */
			if (gpbDiskCb.isDiskErr) 
			{
				//appPbErrorProcess(PB_ERR_DISK_ERROR);
				gpbMainCb.err = PB_ERR_CARD_ERROR;
				appErrMsgWithSound(ERR_MSG_CARD_ERROR, ERR_MSG_WITH_WARNING_SOUND, PB_ERR_SHOW_FOREVER);
				break;
			} 
			else if (gtotalImgCount==0) 
			{
				gpbMainCb.err = PB_ERR_NO_IMAGE;
				gpbMainCb.isNoImage = TRUE;
				appPbErrorProcess(PB_ERR_NO_IMAGE);
				//appErrMsgShow(ERR_MSG_NO_IMG, PB_ERR_SHOW_FOREVER);
				break;
			} 
			else if (gpbImgCb.isFileErr) 
			{
				//appPbErrorProcess(PB_ERR_FILE_ERROR);
				gpbMainCb.err = PB_ERR_FILE_ERROR;
				appErrMsgShow(ERR_MSG_FILE_ERROR, PB_ERR_SHOW_FOREVER);
				break;
			}
		}
		break;

	case PB_MAIN_STAT_ZPAN_SUSPEND:
		PB_DBG("PbMain: PB_MAIN_STAT_ZPAN_SUSPEND \n");
		//appPbMainEnter(gcurrDcfIdx);
		gpbImgCb.dcfIdx = gcurrDcfIdx;
		if (SUCCESS==playImgDcfAttrGet(&gpbImgCb)){
			appPlayOsd_MainInfoDraw(PB_OSD_DRAW_NORMAL);
		}
		break;
	case PB_MAIN_STAT_MPLAY_SUSPEND:
		PB_DBG("PbMain: PB_MAIN_STAT_MPLAY_SUSPEND \n");
		if (gtotalImgCount==0) 
		{
			appPbErrorProcess(PB_ERR_NO_IMAGE);
		} 
		else 
		{
			appPbImagePlay(gcurrDcfIdx);
			if (IS_FILETYPE_JPG(gpbImgCb.dcfAttr.fileType)) 
			{
				gpbMainCb.postponeUI = 1;
			}
		}
		break;

	case PB_MAIN_STAT_IMG_PROT_SUSPEND:
		PB_DBG("PbMain: PB_MAIN_STAT_IMG_PROT_SUSPEND \n");
		break;

	case PB_MAIN_STAT_IMG_DEL_SUSPEND:
		PB_DBG("PbMain: PB_MAIN_STAT_IMG_DEL_SUSPEND \n");
		appPbMainEnter(gpbMainCb.lastImgIdx);
		break;

	case PB_MAIN_STAT_C2C_SUSPEND:
		PB_DBG("PbMain: PB_MAIN_STAT_C2C_SUSPEND \n");
		appPbMainEnter(gpbMainCb.lastImgIdx);
		break;

	case PB_MAIN_STAT_SLIDE_SUSPEND:
		PB_DBG("PbMain: PB_MAIN_STAT_SLIDE_SUSPEND \n");
		#if HDMI_MENU_BY_JPG
		#if (KIT_WITH_HDMI)   /*Add by Lation@20130717*/
		if(IS_HDMI_IN){
			appPbMainEnter(0);
		}
		#endif
		#endif
		break;

	case PB_MAIN_STAT_JPG_ROTATE:
		PB_DBG("PbMain: PB_MAIN_STAT_JPG_ROTATE(0x%0x) \n", gpbMainCb.mainStat);
		appPbMainEnter(gpbMainCb.lastImgIdx);
		break;

	case PB_MAIN_STAT_JPG_EFFECT:
	case PB_MAIN_STAT_JPG_PHOTOFRAME:
	case PB_MAIN_STAT_JPG_RESIZE:
		PB_DBG("PbMain: PB_MAIN_STAT_JPG_XXX(0x%0X) \n", gpbMainCb.mainStat);
		appPbMainEnter(gpbMainCb.lastImgIdx);
		break;

	case PB_MAIN_STAT_VIDEO_SUSPEND:
		PB_DBG("PbMain: PB_MAIN_STAT_VIDEO_SUSPEND(0x%0x) \n", gpbMainCb.mainStat);
		if(!IS_CARD_EXIST)
		{
			break;
		}
		appPbMainEnter(gpbMainCb.lastImgIdx);
        #if CAM_TYPE_CVR
		appPlayOsd_VideoFileIconDrow(TRUE);
		appPlayOsd_VidFileNameChangeDrow();
		#endif
		break;

	case PB_MAIN_STAT_DPOF_SUSPEND:
	case PB_MAIN_STAT_MEMOREC_SUSPEND:
		PB_DBG("PbMain: PB_MAIN_STAT_MEMOREC_SUSPEND(0x%0x) \n", gpbMainCb.mainStat);
		appPbMainEnter(gpbMainCb.lastImgIdx);
		break;

	default:
		PB_DBG("PbMain: !! WARNING !!, unknown mainStat(0x%x) !! \n", gpbMainCb.mainStat);
	}

	if (gpbMainCb.postponeUI==0 && /*for AVI/MOV filetype */
	IS_CARD_EXIST && gtotalImgCount!=0 && 
	(gpbMainCb.err != PB_ERR_DISK_ERROR)) 
	{
		appPlayOsd_MainInfoDraw(PB_OSD_DRAW_NORMAL);
	}
	PB_LOG("MainInfoDraw @ %s,%d \n", __FUNCTION__, __LINE__);

	gpbMainCb.mainStat = PB_MAIN_STAT_STANDBY;

	return SUCCESS;
}



/*-------------------------------------------------------------------------
 *  File Name : appPbMainState_Close
 *------------------------------------------------------------------------*/
static
void
appPbMainState_Close(
	void
)
{
	gpbMainCb.mainStat = PB_MAIN_STAT_NULL;
	gpbIsViewIdle = TRUE;
	#if (KIT_WITH_HDMI)
	#if HDMI_MENU_BY_JPG
		if(IS_HDMI_IN){
			pipHDMIMenuFlag = 0;
			sp5kPipTerm();
		}
	#else
		AppPbDestroyshowJPGInPIP();
	#endif
	//AppPbDestroyshowJPGInPIP();
	#endif
	if (appLensQuery(LENS_QUERY_MOTION_STATE)) 
	{
		appLensStopWait();
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appPbMainState_Init
 *------------------------------------------------------------------------*/
static
void
appPbMainState_Init(
	void
)
{
	PB_VERBOSE("%s: \n", __FUNCTION__);
#if 1
	/*-- Check if DISK READY */
	if (!IS_DISK_MOUNT_READY ) 
	{
		printf("%s: DISK Not Ready !! \n", __FUNCTION__);
		gpbMainCb.err = PB_ERR_DISK_NOT_READY;
		gpbDiskCb.isDiskReady = FALSE;
		sp5kHostMsgSend(APP_UI_MSG_PB_WAIT_DISK, 0 /* timeout counter */);
		return;
	}
#endif	
	gpbDiskCb.isDiskReady = TRUE;

	sp5kHostMsgSend(APP_UI_MSG_PB_CHECK_DISK, 0);
	return;
}

/*-------------------------------------------------------------------------
 *  File Name : appPbMainState_OnMsg
 *------------------------------------------------------------------------*/
static
void
appPbMainState_OnMsg(
	UINT32 msg,
	UINT32 param
)
{
	UINT32 tmpU32;

	PB_VERBOSE("PB_OnMsg: 0x%08x, 0x%08x ... \n", msg, param);


	switch (msg) {
	case APP_UI_MSG_PB_WAIT_DISK:
		printf("[PB] WAIT_DISK (0x%08x)\n", param);
		if (!IS_DISK_MOUNT_READY) 
		{
			if (param>(10000/PB_WAIT_DISK_INTERVAL)) 
			{
				PB_LOG("[PbMain] !!FATAL!!, DISK Mount timed out !!\n");
			}
			APP_SLEEP_MS(PB_WAIT_DISK_INTERVAL);
			sp5kHostMsgSend(APP_UI_MSG_PB_WAIT_DISK, param+1);
			return;
		}

		gpbDiskCb.isDiskReady = TRUE;
		sp5kHostMsgSend(APP_UI_MSG_PB_CHECK_DISK, 0);
		break;

	case APP_UI_MSG_PB_CHECK_DISK:
		PB_LOG("[PB] CHK_DISK (0x%08x)\n", param);
		/*-- DISK mount completed, go & and checkup disk */
		gpbDiskCb.isDiskErr = FALSE;
		if (IS_DISK_MOUNT_FAIL) 
		{
			PB_LOG("%s: DISK Mount Failed !! \n", __FUNCTION__);
			gpbMainCb.err = PB_ERR_DISK_ERROR;
			gpbDiskCb.isDiskErr = TRUE;
		}
		else 
		{
			gtotalImgCount = appPbTotalFileCountGet();
			gpbMainCb.isNoImage = FALSE;
			if (gtotalImgCount==0) 
			{
				gpbMainCb.err = PB_ERR_NO_IMAGE;
				gpbMainCb.isNoImage = TRUE;
			}
			else
			{ //mantis 0024588: dpof init only when image coount > 0
				/*-- Init DPOF */
				appDpofInit();
				#if SP5K_CDFS_OPT
				appCdfsFolderInit(appCdfsFileFolderGet());
				#endif
			}
		}

		sp5kHostMsgSend(APP_UI_MSG_PB_CREATE, 0);
		break;

	case APP_UI_MSG_PB_CREATE:
		PB_LOG("[PB] CREATE (0x%08x)\n", param);
		gpbIsViewIdle = TRUE;
		tmpU32 = appPreviousStateGet();
		if (STATE_GROUP(tmpU32)==APP_STATE_GROUP_VIEW 
			||STATE_GROUP(tmpU32)==APP_STATE_GROUP_VIDEO  )
		{
			/*-- Create TIMER to close lens */
			#if KIT_WITH_LENS
			if (!appLensQuery(LENS_QUERY_LENS_CLOSED)) 
			{
				gpbMainCb.closeLensTmrId = appTimerSet(PB_LENS_CLOSE_TIME, "PB LENS");
				if (TIMER_NULL==gpbMainCb.closeLensTmrId) 
				{
					PB_LOG("\n!!ERROR!!, Acquire TIMER failed !!\n\n");
				}
			}
			#endif

			/*-- Shutdown SP5K Live View, if necessary */
			if (gpbMainCb.isNoImage==TRUE || gpbDiskCb.isDiskErr==TRUE) 
			{
				PB_LOG("IDLE to shutdown preview !!\n");
				appModeSet(SP5K_MODE_STANDBY);
				gpbIsViewIdle = FALSE;
					/*-- INIT Flow is suspended here to wait for IDLE ready.
					 * appGroupPlaybackProc() will intercept MODE_RDY(IDLE)
					 * and send APP_UI_MSG_PB_INIT to resume the initialization
					 * flow !!
					 */
				return;
			}
		}

		if (gpbIsViewIdle==TRUE) 
		{
			sp5kHostMsgSend(APP_UI_MSG_PB_INIT, 1 /* CREATE Triggered */);
		}
		break;

	case APP_UI_MSG_PB_INIT:
		PB_LOG("[PB] INIT(0x%08x)\n", param);
		gpbIsViewIdle = TRUE;
		gpbMainCb.isIniting = FALSE;
		appPbMainInit();
		
		break;

	case SP5K_MSG_STILLPLAY_DONE:
		PB_VERBOSE("SP5K_MSG_STILLPLAY_DONE(0x%08x)\n", param);
		/*-- NOTE :
		 *   Common Error Process has been done in
		 *   appGroupPlaybackProc() @ app_state_preproc.c
		 */
		FOOT_PRINT(3);
		tmpU32 = appPbStillPlayDoneParse(param);
		if (tmpU32 & PB_DONE_ERROR) 
		{
			appPbErrorProcess(gpbMainCb.err);
			break;
		}

		if ( (IS_PB_QV_PLAYING && tmpU32==PB_DONE_QV_FINISH) 
			|| (IS_PB_THUMB_PLAYING && tmpU32==PB_DONE_THUMB_FINISH) )
		{
			PB_DBG("THUMB/QV Done !!\n");
			//appTimeDelayMs(30000);
			appPlayOsd_MainInfoDraw(PB_OSD_DRAW_NORMAL);
			gpbImgCb.isThumbImgOK = TRUE;
			if (IS_PB_THUMB_PLAYING 
				&& tmpU32==PB_DONE_THUMB_FINISH)
			{
				//appTimeDelayMs(30000);
				appPbJpgMainPlayStart();
				//appTimeDelayMs(30000);
			}
			else
			{
				gpbMainCb.isPlayDone = TRUE;
				gpbImgCb.isMainImgOK = TRUE;
				gpbMainCb.mainStat = PB_MAIN_STAT_STANDBY;
				gpbMainCb.playStat = PB_PLAY_STAT_STANDBY;
			}

		}
		else if (IS_PB_MAIN_PLAYING && tmpU32==PB_DONE_MAIN_FINISH) 
		{
			PB_DBG("MAIN Done !!\n");
			printf("MAIN Done !!\n");

			/*-- Update OSD ASAP !! */
			if (gpbImgCb.isThumbImgOK==FALSE) 
			{
				appPlayOsd_MainInfoDraw(PB_OSD_DRAW_NORMAL);
			}
			/*-- Update flags */
			gpbMainCb.playStat = PB_PLAY_STAT_STANDBY;
			gpbMainCb.isPlayDone = TRUE;
			gpbImgCb.isMainImgOK = TRUE;
			gpbMainCb.mainStat = PB_MAIN_STAT_STANDBY;
		}
		
		appPbImageShiftRepeatCheck(0);
		break;
	case APP_UI_MSG_PB_AUDIO_TITLE_DONE:
		break;
	case APP_UI_MSG_PB_SHIFT_NEXT:
	case APP_UI_MSG_PB_SHIFT_PRE:
		if (gpbMainCb.playStat != PB_PLAY_STAT_STANDBY)
		{
			break;
		}
		
		if (gpbMainCb.isNoImage == TRUE)
		{
			break;
		}
		appPbMainStImageShift((msg==APP_UI_MSG_PB_SHIFT_NEXT ?
									PB_SHIFT_NEXT :
									PB_SHIFT_PRE));
		break;
	case SP5K_MSG_DCF_FILEOP_COMPLETE:
		gProtectAllFinish = TRUE;
		break;
	default :
		break;
	}
}


/*-------------------------------------------------------------------------
 *  File Name : appPbMainState_OnKey
 *------------------------------------------------------------------------*/
static
void
appPbMainState_OnKey(
	UINT32 msg,
	UINT32 param
)
{
	UINT32 tmpU32, ret;
	UINT32 w, h;
	#if CAM_TYPE_CVR
	uiPara_t* puiPara = appUiParaGet(); 
	UINT16 ncuridx;
	#endif	
	/*sp5kPipLayerCfg_t pipLayer;*/ /*for clean warning*/

	ret = ret;
	PB_VERBOSE("%s : [0x%x] [0x%x]\n",__FUNCTION__,msg,param);

	if (FAIL==pbmainKeyPreCheck(msg)) 
	{
		PB_VERBOSE("%s: Key(0x%x) ignored ...\n", __FUNCTION__, msg);
		return;
	}

	if(!gProtectAllFinish && appPreviousStateGet() == APP_STATE_PB_IMG_PROT)
		return;
	
	switch (msg) {
	case APP_KEY_PRESS_S2:
		break;
	case APP_KEY_PRESS_MENU:
	case APP_KEY_PRESS_DEL:
		PB_VERBOSE("PB_MAIN: APP_KEY_PRESS_MENU\n");
		//PB_BEEP;
		if(gtotalImgCount!=0)
		{
            #if CAM_TYPE_CVR
			CLEAR_OSD_SRC;
			if(puiPara->fileType != FILETYPE_JPG)
			{
				AppPbDestroyshowJPGInPIP();
			}
			pbflag = 1;
			#endif
			gpbMainCb.mainStat = PB_MAIN_STAT_STANDBY;
			gpbMainCb.lastImgIdx = gcurrDcfIdx;
			appStateChange(APP_STATE_MENU, STATE_PARAM_NORMAL_INIT);
		}
		break;
	case APP_KEY_PRESS_MODE:
		/*Change to Video Mode*/
     #if CAM_TYPE_CVR
		 appStateChange(APP_STATE_VIDEO_PREVIEW, STATE_PARAM_NORMAL_INIT);
     #else
		#if (KIT_WITH_HDMI)
		if(IS_HDMI_IN)
		{			
			return ;
		}
		#endif
		appStateChange(APP_STATE_STILL_PREVIEW,STATE_PARAM_NORMAL_INIT);	
    #endif
		break;
	case APP_KEY_LONG_PRESS_OK:
		appStateChange(APP_STATE_SCDV_MENU, STATE_PARAM_NORMAL_INIT);	
		break;
	case APP_KEY_PRESS_OK:
	case APP_KEY_PRESS_SET:
		#if CAM_TYPE_CVR
		if (gpbMainCb.err == PB_ERR_FILE_ERROR)
		{
			printf("FILE ERROR !!\n");
			break;
		}
		if (!gpbMainCb.isPlayDone) 
		{
			PB_VERBOSE("Playing busy!!, Ignore ... \n");
			break;
		}
		//	PB_BEEP;
		tmpU32 = gpbImgCb.dcfAttr.fileType;
		if (gpbImgCb.fileType==PB_FT_JPG_MEMO || IS_FILETYPE_WAV(tmpU32)) 
		{
			gpbMainCb.lastImgIdx = gcurrDcfIdx;
			gpbMainCb.mainStat = PB_MAIN_STAT_AUDIO_SUSPEND;
			appStateChange(APP_STATE_PB_AUDIO, APP_UI_MSG_PB_AUDIO_CREATE);
		} 
		else if (IS_FILETYPE_VIDEO(tmpU32)) 
		{
			gpbMainCb.lastImgIdx = gcurrDcfIdx;
			gpbMainCb.mainStat = PB_MAIN_STAT_VIDEO_SUSPEND;
			APP_OSD_REFRESH_OFF;
			CLEAR_OSD_SRC;
			AppPbDestroyshowJPGInPIP();
			APP_OSD_REFRESH_ON;
			appStateChange(APP_STATE_PB_VIDEO, STATE_PARAM_NORMAL_INIT);
		} 
		else
		#endif
		{	/* JPEG Information */
			gpbMainCb.osdType++;
			if(gpbMainCb.osdType == PB_DISP_TOTAL)
			{
				gpbMainCb.osdType = PB_DISP_BASIC;
			}
			
			if(gpbMainCb.osdType == PB_DISP_FULL)
			{
				appPbImgExifInfoGet(&gpbExifInfo);
			}
			appPlayOsd_MainInfoDraw(PB_OSD_DRAW_NORMAL);
		}
		break;
	case APP_KEY_PRESS_PB:
     #if !CAM_TYPE_CVR
		#if (KIT_WITH_HDMI)
		if(IS_HDMI_IN)
		{			
			AppPbDestroyshowJPGInPIP();
			w=1920;	h=1080;
		}
		else{
			w=320;	h=240;
		}
		#endif
		CLEAR_OSD_SRC;
		#if 0
		audioBGDraw=TRUE;
		appJpgFileDraw(SP5K_RES_TYPE_JPG, RES_JPG_BLACK, 0, 0, w, h);
		#endif
		appUiModeChange();
	#endif
		break;
 #if CAM_TYPE_CVR
	 case APP_KEY_PRESS_UP:
		 if (gpbMainCb.playStat != PB_PLAY_STAT_STANDBY)
		 {
			 break;
		 }
		 if (gpbMainCb.isNoImage == TRUE)
		 {
			 break;
		 }
		 trimCount=0;//reset video trim flag
		 APP_OSD_REFRESH_OFF;
		 appViewOsd_AllClean();
		 appPbMainStImageShift(PB_SHIFT_NEXT);	 
		 ncuridx = gpbMainCb.totalImg - gpbMainCb.curImgIdx;
		 if(puiPara->fileType != FILETYPE_JPG)
		 {
			 if(!ncuridx)
			 {
				 appPlayOsd_VideoInfoDraw();
			 }
			 else
			 {
				 appPlayOsd_VidFileNameChangeDrow();
			 }
		 }
		 APP_OSD_REFRESH_ON; 
		 break;
	 case APP_KEY_PRESS_POWER:
	 case APP_KEY_PRESS_DOWN:
		 if (gpbMainCb.playStat != PB_PLAY_STAT_STANDBY)
		 {
			 break;
		 }
		 if (gpbMainCb.isNoImage == TRUE)
		 {
			 break;
		 }
		 trimCount=0;//reset video trim flag
		 APP_OSD_REFRESH_OFF;
		 appViewOsd_AllClean();
		 appPbMainStImageShift(PB_SHIFT_PRE);
		 ncuridx = gpbMainCb.totalImg - gpbMainCb.curImgIdx;
		 if(puiPara->fileType != FILETYPE_JPG)
		 {
			 if(!ncuridx)
			 {
				 appPlayOsd_VideoInfoDraw();
			 }
			 else
			 {
				 appPlayOsd_VidFileNameChangeDrow();
			 }
		 }
		 APP_OSD_REFRESH_ON;
		 break;
	 case APP_KEY_PRESS_LEFT:
		 fileTypeSelect=1;
		 appStateChange(APP_STATE_MENU,STATE_PARAM_NORMAL_INIT);/**/
		 break;
	/*jj add for FF+RW seamless play*/
	case APP_KEY_PRESS_TELE: 
		if(CDFS_FILE_FOLDER_JPG != appCdfsFileFolderGet())
		{
			seamlessPlayEn = TRUE;
			sp5kHostMsgSend(APP_KEY_PRESS_SET, 0);
		}
		break;
    #else
	case APP_KEY_PRESS_UP:
		if(appPbTotalFileCountGet()==0){
			break;
		}
		 if(trimCount >0)//switch video trim mode
		 {
			if(IS_FILETYPE_VIDEO(gpbImgCb.dcfAttr.fileType))
			{
				CLEAR_OSD_SRC ;
				gpbMainCb.lastImgIdx = gcurrDcfIdx;
				gpbMainCb.mainStat = PB_MAIN_STAT_VIDEO_SUSPEND;
				
				if(trimCount == 1)
				{
					gpbMainCb.playStat = PB_PLAY_STAT_VIDEO_TRIM_P2P;
				}
				else
				{
					gpbMainCb.playStat = PB_PLAY_STAT_VIDEO_TRIM_SEC;
				}
				appStateChange(APP_STATE_PB_TRIMVIDEO, STATE_PARAM_NORMAL_INIT);
				PB_BEEP;
			}
		}
		else//normal play
		{
			if (!gpbMainCb.isPlayDone) 
			{
				PB_VERBOSE("Playing busy!!, Ignore ... \n");
				break;
			}
			PB_BEEP;
			tmpU32 = gpbImgCb.dcfAttr.fileType;
			if (gpbImgCb.fileType==PB_FT_JPG_MEMO || IS_FILETYPE_WAV(tmpU32)) 
			{
				gpbMainCb.lastImgIdx = gcurrDcfIdx;
				gpbMainCb.mainStat = PB_MAIN_STAT_AUDIO_SUSPEND;
				appStateChange(APP_STATE_PB_AUDIO, APP_UI_MSG_PB_AUDIO_CREATE);
			} 
			else if (IS_FILETYPE_VIDEO(tmpU32)) 
			{
				gpbMainCb.lastImgIdx = gcurrDcfIdx;
				gpbMainCb.mainStat = PB_MAIN_STAT_VIDEO_SUSPEND;
				appStateChange(APP_STATE_PB_VIDEO, STATE_PARAM_NORMAL_INIT);
			} 
		}
		
		break;
		
	case APP_KEY_PRESS_DOWN:
		#if KIT_WITH_HDMI
		if (IS_HDMI_IN) 
		{
			/*if (IS_PB_JPG_PLAYING) 
			{
				PB_DBG("PbMain: aborting playing while going multiplay !\n");
				sp5kStillPlayAbort();
			}
			PB_BEEP;
			if (gtotalImgCount!=0) 
			{
				printf("==============%s %d==============\n", __FUNCTION__, __LINE__);
				if(IS_HDMI_IN)	
				{
					AppPbDestroyshowJPGInPIP();		
				}
				appPbMPlayCreate();
			}*/                           /*modify for mantis bug 0047057*/
		}
		else
		{
			if(IS_FILETYPE_VIDEO(gpbImgCb.dcfAttr.fileType) && gtotalImgCount)
			{
				trimCount ++ ;
				if(trimCount == 3)
				{
					trimCount = 0;
				}
				appPlayOsd_VideoTrimCntrlDraw(TRUE, trimCount );
			}	
		}
		#endif
		break;
	case APP_KEY_PRESS_LEFT:
		if (gpbMainCb.playStat != PB_PLAY_STAT_STANDBY)
		{
			break;
		}
		if (gpbMainCb.isNoImage == TRUE)
		{
			break;
		}
		trimCount=0;//reset video trim flag
		appPbMainStImageShift(PB_SHIFT_PRE);
		break;
	case APP_KEY_PRESS_RIGHT:
		if (gpbMainCb.playStat != PB_PLAY_STAT_STANDBY)
		{
			break;
		}
		if (gpbMainCb.isNoImage == TRUE)
		{
			break;
		}
		trimCount=0;//reset video trim flag
		appPbMainStImageShift(PB_SHIFT_NEXT);
		break;
	case APP_KEY_PRESS_TELE:
		if (!IS_FILETYPE_JPG(gpbImgCb.dcfAttr.fileType) ||
			gpbMainCb.mainStat != PB_MAIN_STAT_STANDBY) 
		{
			break;
		}
		PB_BEEP;
		if (gtotalImgCount!=0) 
		{	
			#if (KIT_WITH_HDMI)
			if(IS_HDMI_IN)
			{
			//	AppPbDestroyshowJPGInPIP();	
				
			}
			#endif 
			appPbZoomPanCreate();
		}
		break;
	case APP_KEY_PRESS_WIDE:
		if (IS_PB_JPG_PLAYING) 
		{
			PB_DBG("PbMain: aborting playing while going multiplay !\n");
			break;
			//sp5kStillPlayAbort();
		}
		PB_BEEP;
		if (gtotalImgCount!=0) 
		{ 
		    #if (KIT_WITH_HDMI)
			if(IS_HDMI_IN){
				AppPbDestroyshowJPGInPIP();
			}
			#endif 
			appPbMPlayCreate();
		}
		break;
	#endif
	}
	
	switch (msg) 
	{	
		case APP_KEY_PRESS_MODE:			
		case APP_KEY_PRESS_SET:
		case APP_KEY_PRESS_PB:		
		case APP_KEY_PRESS_MENU:
		case APP_KEY_PRESS_DEL:		
		appBeep(TRUE, FALSE);
		break;
		default:
			  break;
	}

}

/*-------------------------------------------------------------------------
 *  File Name : appPbMainState
 *------------------------------------------------------------------------*/
void
appPbMainState(
	UINT32 msg,
	UINT32 param
)
{
	UINT32 appPrevStat;
	uiPara_t* puiPara = appUiParaGet();
	appPrevStat = appPreviousStateGet();

	switch (msg) 
	{
	case APP_STATE_MSG_INIT:
		RPrintf("appPbMainState");
		printf("filetype=%d\n",puiPara->fileType);	
		appMsgFlush(APP_MSG_UI_QUE);  /*add for mantis bug 0047610*/
		appAutoPowerOffEnable(TRUE);/*++ for  power saving while playback main state*/
		#if SP5K_CDFS_OPT
		if(puiPara->fileType == FILETYPE_VIDEO)
		{
			__FUNC_TRACK__;
			appCdfsFileFolderSet(CDFS_FILE_FOLDER_VIDEO);
			appCdfsFolderInit(CDFS_FILE_FOLDER_VIDEO);
		}
		else if(puiPara->fileType == FILETYPE_JPG)
		{
			__FUNC_TRACK__;
			appCdfsFileFolderSet(CDFS_FILE_FOLDER_JPG);
			appCdfsFolderInit(CDFS_FILE_FOLDER_JPG);
		}
		else if(puiPara->fileType == FILETYPE_EVENT)
		{
			__FUNC_TRACK__;
			appCdfsFileFolderSet(CDFS_FILE_FOLDER_EVENT);
			appCdfsFolderInit(CDFS_FILE_FOLDER_EVENT);
		}
		else if(puiPara->fileType == FILETYPE_AUDIO)
		{
			__FUNC_TRACK__;
			appCdfsFileFolderSet(CDFS_FILE_FOLDER_AUDIO);
			appCdfsFolderInit(CDFS_FILE_FOLDER_AUDIO);
		}

		#endif

		sensorDevPowerCustomCb(SENSOR_DEV_POWER_ID_SYS_OFF);
		sp5kModeSet(SP5K_MODE_STANDBY);
		sp5kModeWait(SP5K_MODE_STANDBY);
		appRfCopyBufferCallbackSet();
	
	#if CAM_TYPE_CVR
		if (!IS_PLAYBACK_GROUP(appPreviousStateGet())&& (!pbflag)) 
		{		
			PB_CLR_ALL; 
			playDeviceConfig();
			playInitCreate();
			gpbMainCb.mainStat = PB_MAIN_STAT_NULL;
		}
		else
		{
		}
		if (gpbMainCb.mainStat==PB_MAIN_STAT_NULL) 
		{
			appPbMainState_Init();
		}
		else if(IS_MENU_STATE(appPreviousStateGet()))
		{		
			CLEAR_OSD_SRC;			
			gpbMainCb.mainStat = PB_MAIN_STAT_MENU_SUSPEND;
			
			if(puiPara->fileType != FILETYPE_JPG || IS_HDMI_IN)
			{				
				sp5kModeSet(SP5K_MODE_STANDBY);
				sp5kModeWait(SP5K_MODE_STANDBY);
				appPbPIPInit();
			}
			pbflag = 0;
			sp5kHostMsgSend(APP_UI_MSG_PB_INIT, 2 /* INIT Triggered */);
		}
		else if (appPrevStat == APP_STATE_PB_VIDEO)
		{
			sp5kModeSet(SP5K_MODE_STANDBY);
			sp5kModeWait(SP5K_MODE_STANDBY);
			appPbPIPInit();
			pbflag = 0;
			sp5kHostMsgSend(APP_UI_MSG_PB_INIT, 2);
		}
		else if (appPrevStat == APP_STATE_PB_IMG_DEL)
		{
			sp5kModeSet(SP5K_MODE_STANDBY);
			sp5kModeWait(SP5K_MODE_STANDBY);
			appPbPIPInit();
			pbflag = 0;
			sp5kHostMsgSend(APP_UI_MSG_PB_INIT, 2);
		}
	#else
		if ((!IS_PLAYBACK_GROUP(appPreviousStateGet()))&&(!IS_MENU_STATE(appPreviousStateGet()))) 
		{
			playDeviceConfig();
			playInitCreate();
			gpbMainCb.mainStat = PB_MAIN_STAT_NULL;
		}
		else if(IS_MENU_STATE(appPreviousStateGet()))
		{
			gtotalImgCount = appPbTotalFileCountGet();
			gpbMainCb.isNoImage = FALSE;
			if (gtotalImgCount==0) 
			{
				gpbMainCb.err = PB_ERR_NO_IMAGE;
				gpbMainCb.isNoImage = TRUE;
				gpbMainCb.mainStat = PB_MAIN_STAT_MENU_SUSPEND;
			}
			/*else
			{
				gpbMainCb.mainStat = PB_MAIN_STAT_STANDBY;
			}*/
			//appPlayOsd_MainInfoDraw(PB_OSD_DRAW_NORMAL);
		}
		
		if (gpbMainCb.mainStat==PB_MAIN_STAT_NULL) 
		{
			appPbMainState_Init();
		}
		else 
		{
			sp5kHostMsgSend(APP_UI_MSG_PB_INIT, 2 /* INIT Triggered */);
		}
	#endif
		
		#if HDMI_MENU_BY_JPG
		#if (KIT_WITH_HDMI)   /*Add by Lation@20130717*/
		if(IS_HDMI_IN){
			menuHDMIPIPInit();
		}
		#endif
		#endif
		
		gpbMainCb.isIniting = TRUE;
	    /*-- To mark the period from here to APP_UI_MSG_PB_INIT.
		 * To ignore all buttons to guarantee
	     *   the completion of PLAY INIT period.
	     * The UI buttons will be handled after APP_UI_MSG_PB_INIT.
	     */
		/* do nothing */

		break;

	case APP_STATE_MSG_CLOSE:
		appViewOsd_AllClean();
		appPbMainState_Close();
		appStateCloseDone();
		break;
	default:
		if (IS_APP_KEY_MSG(msg)) 
		{
			appPbMainState_OnKey(msg,param);
		}
		else
		{
			appPbMainState_OnMsg(msg,param);
		}
		break;
	}

}


