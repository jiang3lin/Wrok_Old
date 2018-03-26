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
#include "sp5k_dcf_api.h"
#include "sp5k_pb_api.h"
#include "sp5k_exif_api.h"
#include "app_playback_private.h"
#include "app_playback_def.h"
#include "app_playback_osd.h"
#include "app_view_param_def.h"
#include "app_lens_api.h"
#include "app_icon_def.h"
#include "sp5k_rsvblk_api.h"

#include "app_error_msg.h"
#include "app_jpg_draw.h"
#include "app_dpof.h"
#include "api/sp5k_pip_api.h"
#include "app_ui_para.h" 
#include "app_msg.h"
#include "app_res_def.h"
#include "app_osdfont.h"
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
UINT8 IsInHDMI=0;
#if 0
UINT16 gpbSizeGrade[11][3][2] =
{		/*4:3*/		/*3:2*/		/*16:9*/
	{{ 640,  480},{ 640,  480},{ 640,  360}},/*VGA*/
	{{1280, 960},{1280,  832},{1280, 720}},/*1M*/
	{{1600, 1200},{1600, 1064},{1600, 896}},/*2M*/
	{{2048, 1536},{2048, 1360},{2048, 1152}},/*3M*/
	/*{{2272, 1704},{2272, 1512},{2272, 1280}},4M*/
	{{2560, 1920},{2560, 1696},{2560, 1440}},/*5M*/
	/*{{2816, 2112},{2816, 1872},{2816, 1584}},6M*/
	{{3072, 2304},{3072, 2048},{3072, 1728}},/*7M*/
	{{3264, 2448},{3264, 2176},{3264, 1840}},/*8M*/
	{{3456, 2592},{3456, 2304},{3456, 1944}},/*9M*/
	{{3648, 2736},{3648, 2432},{3648, 2048}}/*10M*/
};
#else
pbPicSize_t gpbSizeGrade[PB_UI_RESIZE_TOTAL][3]=
{		/*4:3*/ 	/*3:2*/ 	/*16:9*/
	/*{{3648, 2736, ID_ICON_RESOLUTION_10M},{3648, 2432, ID_ICON_RESOLUTION_10M},{3648, 2048, ID_ICON_RESOLUTION_10M}},10M*/
	{{3456, 2592, ID_ICON_RESOLUTION_9M},{3456, 2304, ID_ICON_RESOLUTION_9M},{3456, 1944, ID_ICON_RESOLUTION_9M}},/*9M*/
	{{3264, 2448, ID_ICON_RESOLUTION_8M},{3264, 2176, ID_ICON_RESOLUTION_8M},{3264, 1840, ID_ICON_RESOLUTION_8M}},/*8M*/
	{{3072, 2304, ID_ICON_RESOLUTION_7M},{3072, 2048, ID_ICON_RESOLUTION_7M},{3640, 2048, ID_ICON_RESOLUTION_7M_16_9}},/*7M*/
	/*{{2816, 2112,ID_ICON_RESOLUTION_6M},{2816, 1872,ID_ICON_RESOLUTION_6M},{2816, 1584,ID_ICON_RESOLUTION_6M}},6M*/
	{{2560, 1920, ID_ICON_RESOLUTION_5M},{2560, 1696, ID_ICON_RESOLUTION_5M},{2560, 1440, ID_ICON_RESOLUTION_5M}},/*5M*/
	/*{{2272, 1704,ID_ICON_RESOLUTION_4M},{2272, 1512,ID_ICON_RESOLUTION_4M},{2272, 1280,ID_ICON_RESOLUTION_4M}},4M*/
	{{2048, 1536, ID_ICON_RESOLUTION_3M},{2048, 1360, ID_ICON_RESOLUTION_3M},{2048, 1152, ID_ICON_RESOLUTION_3M}},/*3M*/
	{{1600, 1200, ID_ICON_RESOLUTION_2M},{1600, 1064, ID_ICON_RESOLUTION_2M},{1920, 1080, ID_ICON_RESOLUTION_2M_16_9}},/*2M*/
	{{1280,  960, ID_ICON_RESOLUTION_1M},{1280,  832, ID_ICON_RESOLUTION_1M},{1280, 720, ID_ICON_RESOLUTION_1M}},/*1M*/
	{{ 640,  480, ID_ICON_RESOLUTION_VGA},{ 640,  480, ID_ICON_RESOLUTION_VGA},{640, 360, ID_ICON_RESOLUTION_VGA}}/*VGA*/
};


#endif
BOOL isLastFileIdxFlag=FALSE;
static BOOL pipInitStatus = FALSE;
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
#if !CAM_TYPE_CVR
extern UINT32 gSlideShowBeginIndex;
#endif
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
UINT32 appVideoSizeGet(UINT8 size_idx, UINT32 *pW, UINT32 *pH);

/*-------------------------------------------------------------------------
 *  File Name : appPbDiskMount
 *------------------------------------------------------------------------*/
UINT32
appPbDiskMount(
	pbDiskIdDef_t dskId
)
{
	switch (dskId)
	{
		case PB_DISK_NAND:
		case PB_DISK_SD:
			appDiskMount(dskId);
			return SUCCESS;
			break;
		default:
			PB_DBG("%s: Unknown disk (%d) !!\n", __FUNCTION__, dskId);
			return FAIL;
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appPbActiveDiskSet
 *------------------------------------------------------------------------*/
UINT32
appPbActiveDiskSet(
	pbDiskIdDef_t dskId
)
{
	/*-- Configure & activate DCF active drive */
	if (FAIL==appActiveDiskSet((UINT32)dskId))
	{
		PB_DBG("%s: Setting AciveDisk (%d) failed  !!\n", __FUNCTION__, dskId);
		return FAIL;
	}

	/*-- Specify FILETYPE */
	if (FAIL==sp5kDcfFsFileTypeSet(APP_PB_FILETYPE_ALL))
	{
		PB_VERBOSE("%s: sp5kDcfFsFileTypeSet(%d) failed \n", __FUNCTION__, dskId);
		return FAIL;
	}

	return SUCCESS;
}


/*-------------------------------------------------------------------------
 *  File Name : playMusicDRAMPlayStop
 *------------------------------------------------------------------------*/
void
playMusicDRAMPlayStop(
	void
)
{
	/*-- To Stop Last DRAM Play  */
	sp5kResDRAMControl(SP5K_RES_DRAM_ABORT, 0, TRUE, 0, 0, 0);
}

/*-------------------------------------------------------------------------
 *  File Name : playIsPrivateQVGA
 *------------------------------------------------------------------------*/
BOOL
playIsPrivateQVGA(
	void
)
{
	return TRUE;
}

/*-------------------------------------------------------------------------
 *  File Name : playJpgImgTypeAutoSelect
 *------------------------------------------------------------------------*/
static
UINT8
playJpgImgTypeAutoSelect(
	pbImgCb_t *pimgCb
)
{
	BOOL	isICatchCam = FALSE;

	/*-- Query for thumbnail/main/spQVGA */
	sp5kExifImageQuery(&pimgCb->szDcfFileName[0], &pimgCb->exifImgFlag);

	/*-- Check if Camera of iCatch :-P */
	isICatchCam = playIsPrivateQVGA();

	#if KIT_WITH_HDMI
	if (IS_HDMI_IN)  //Ethan
		return SP5K_SPB_IMG_PRIMARY;
	#endif
	/*-- Examine any QV/THUMB/MAIN */
	if (IS_QV_IMG_EXIST(pimgCb->exifImgFlag) && isICatchCam)
	{
		return SP5K_SPB_IMG_QV;
	}
	else if (IS_THUMB_IMG_EXIST(pimgCb->exifImgFlag))
	{
		return SP5K_SPB_IMG_THUMB;
	}
	else if (IS_MAIN_IMG_EXIST(pimgCb->exifImgFlag))
	{
		return SP5K_SPB_IMG_PRIMARY;
	}
	else
	{
		return 0xFF;
	}
}

/*-------------------------------------------------------------------------
 *  File Name : playJpgCheckup
 *------------------------------------------------------------------------*/
UINT32
playJpgCheckup(
	pbImgCb_t *pimgCb
)
{
	UINT32	imgWidth, imgHeight, imgYuvFmt;


	if (!IS_FILETYPE_JPG(pimgCb->dcfAttr.fileType))
	{
		return FAIL;
	}

	/*-- Query for thumbnail/main/spQVGA */
	sp5kExifImageQuery(&pimgCb->szDcfFileName[0], &pimgCb->exifImgFlag);
	/*-- Check YUV format : only 422/420 supported */
	sp5kExifImageInfoGet(SP5K_IMG_PRIMARY, SP5K_IMG_ATTR_DATA_FMT, &imgYuvFmt);
	if (imgYuvFmt != 0x21 && imgYuvFmt != 0x22)
	{
		pimgCb->isFileErr = TRUE;
		pimgCb->isMainImgOK = pimgCb->isThumbImgOK = FALSE;
		return FAIL;
	}
	pimgCb->imgYuvFmt = imgYuvFmt;

	/*-- Check image W & H : shall be multiple of 8 */
	sp5kExifImageInfoGet(SP5K_IMG_PRIMARY, SP5K_IMG_ATTR_WIDTH, &imgWidth);
	sp5kExifImageInfoGet(SP5K_IMG_PRIMARY, SP5K_IMG_ATTR_HEIGHT, &imgHeight);
	if ((imgWidth & 0x07UL) || (imgHeight & 0x07UL))
	{
		pimgCb->isFileErr = TRUE;
		pimgCb->isMainImgOK = pimgCb->isThumbImgOK = FALSE;
		return FAIL;
	}
	pimgCb->imgWidth = imgWidth;
	pimgCb->imgHeight = imgHeight;
	return SUCCESS;
}
void appIsInPBMode(void)
{
	 appStateChange(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
}
void appPbShowBG(void)
{
	#if (KIT_WITH_HDMI)
		#if PIP_32M
		UINT32 w = 960, h = 540;
		#else
		UINT32 w = 1920, h = 1080;
		#endif
		UINT32 arr[4] = {0,0, w, h};
		if(IS_HDMI_IN)
		{
			sp5kResourceFilePagePlay(SP5K_RES_TYPE_JPG,RES_JPG_PB_BG , SP5K_GFX_PAGE_PIP_MAIN, arr, 0);
			sp5kGfxAttrSet(SP5K_GFX_PIP_REFRESH_ACTIVE,1,0,1,0);
		}else
	#endif

	PB_BG_SHOW;


}


#if (KIT_WITH_HDMI)
void showJPGInPIP(void)
{
	#if PIP_32M
	UINT32 w = 960, h = 540;
	#else
	UINT32 w = 1920, h = 1080;
	#endif
	UINT32 arr[4] = {0,0, w, h};
	sp5kResourceFilePagePlay(SP5K_RES_TYPE_JPG, gpbImgCb.szDcfFileName, SP5K_GFX_PAGE_PIP_MAIN, arr, 0);
	sp5kGfxAttrSet(SP5K_GFX_PIP_REFRESH_ACTIVE,1,0,1,0);
}
/*#define CALLSTACK(str, arg...)	{ printf("%s ", __FUNCTION__); printf(str, ##arg); callStack(); }*/


void AppPbDestroyshowJPGInPIP(void)
{
	 UINT32 ret = SUCCESS;
//	 UINT32 arr[4] = {320, 240, 320, 240};

//	 sp5kCapCfgQvResolutionSet(arr);

	 sp5kRsvPreloadRelease();
	 sp5kModeCfgSet(SP5K_MODE_CFG_STANDBY_DUP, 1);
	 sp5kModeCfgSet(SP5K_MODE_CFG_STANDBY_FORCE, 1);/*1: enter standby mode even already in, 0: enter standby mode if not in*/
	 sp5kModeSet(SP5K_MODE_STANDBY);
	 sp5kModeWait(SP5K_MODE_STANDBY);
	 ret = sp5kPipTerm();
	 pipInitStatus = FALSE;
	 if(ret != SUCCESS)
	 {
		  /*CALLSTACK("sp5kPipTerm err\n");*/
	 }
}


void appPbPIPInit(void)
{
	#if PIP_32M
	UINT32 w = 960, h = 540;
	#else
	UINT32 w, h;
	#endif
	if(IS_HDMI_IN){	
		#if 1 /* [DX][2013/04/24] Fix HDMI top/bottom 4-line disappear */
	    w = 1920; h = 1080;
		#else
		w = 1920;	h = 1088;
		#endif
	}
	else{
		w = 320;	h = 240;
	}
	pipLayerCfg_t pLayer;

	sp5kPipInitCfgSet(SP5K_PIP_INIT_MAIN_FRAME_SIZE, w, h, 0, 0);
	sp5kPipInit();
	sp5kDispDevAttrSet(SP5K_DISP_IMG_ACTIVE, 1, 0, 0, 0);
	pLayer.alpha = 32;
	pLayer.frmH = h;
	pLayer.frmW = w;
	pLayer.frmRoiH = h;
	pLayer.frmRoiW = w;
	pLayer.isShow = 1;
	pLayer.winX = 0;
	pLayer.winY = 0;
//	sp5kPipLayerCreate(0,&pLayer);		/* mantis 45327 */
	sp5kRsvPreloadCfg(1);

	pipInitStatus = TRUE;
}
#endif
/*-------------------------------------------------------------------------
 *  File Name : playJpgPlayStart
 *------------------------------------------------------------------------*/
UINT32
playJpgPlayStart(
	void
)
{

	/*-- Validity checkup JPG file */
	if (FAIL==playJpgCheckup(&gpbImgCb))
	{
		gpbImgCb.isFileErr = TRUE;
		gpbImgCb.fileType = PB_FT_UNKNOWN;
		return FAIL;
	}

	/*-- Auto select QV/THUMB/MAIN to play */
	gpbImgCb.sp5kImgType = playJpgImgTypeAutoSelect(&gpbImgCb);
	gpbImgCb.isFileErr = FALSE;

 	#if KIT_WITH_HDMI
	if (IS_HDMI_IN){
 		gpbImgCb.sp5kImgType = SP5K_SPB_IMG_PRIMARY;
	}
	#endif

	if (gpbImgCb.sp5kImgType==SP5K_SPB_IMG_QV)
	{
		gpbMainCb.playStat = PB_PLAY_STAT_JPG_QV_PLAYING;
	}
	else if (gpbImgCb.sp5kImgType==SP5K_SPB_IMG_THUMB)
	{
		gpbMainCb.playStat = PB_PLAY_STAT_JPG_THUMB_PLAYING;
	}
	else if (gpbImgCb.sp5kImgType==SP5K_SPB_IMG_PRIMARY)
	{
		gpbMainCb.playStat = PB_PLAY_STAT_JPG_MAIN_PLAYING;
	}
	else
	{
		gpbImgCb.isFileErr = TRUE;
		gpbImgCb.fileType = PB_FT_UNKNOWN;
		gpbMainCb.playStat = PB_PLAY_STAT_STANDBY;
		gpbMainCb.err = PB_ERR_FILE_ERROR;
		return FAIL;
	}

	/*-- Go playback select imgType */
	gpbImgCb.isMainImgOK = gpbImgCb.isThumbImgOK = FALSE;
	gpbMainCb.isPlayDone = FALSE;

#if (KIT_WITH_HDMI)
	if (IS_HDMI_IN && pipInitStatus)
	{
	  	showJPGInPIP();
	}
#endif
	sp5kStillPlayCfgSet(SP5K_SPB_IMG_TYPE, gpbImgCb.sp5kImgType);

FOOT_PRINT(1);
	sp5kModeSet(SP5K_MODE_STILL_PLAYBACK);
	sp5kModeWait(SP5K_MODE_STILL_PLAYBACK);
FOOT_PRINT(2);

	/*-- Check JPG IMG Size Grade */
	gpbImgCb.imgOrientDim = appPbImageOrientDimGet(gpbImgCb.imgWidth, gpbImgCb.imgHeight);
	if (gpbImgCb.imgOrientDim & PB_IMG_ORIENT_HORIZONTAL)
	{
		gpbImgCb.sizeGrade = appPbImageSizeGradeGet((UINT16)gpbImgCb.imgWidth, (UINT16)gpbImgCb.imgHeight, 0);
	}
	else if (gpbImgCb.imgOrientDim & PB_IMG_ORIENT_VERTICAL)
	{
		gpbImgCb.sizeGrade = appPbImageSizeGradeGet((UINT16)gpbImgCb.imgWidth, (UINT16)gpbImgCb.imgHeight, 0);
	}
	else
	{
		gpbImgCb.sizeGrade = PB_IMG_SIZE_GRADE_UNKNOWN;
	}

	/*-- Get Histogram & EXIF info, depends on osdType */
	if (gpbMainCb.osdType==PB_DISP_FULL)
	{
		/*-- Histogram */
		if (FAIL==appPbImgHistogramGet(&gpbHistBuf[0], PB_HISTOGRAM_BAR_NUM, PB_HISTOGRAM_OSD_HEIGHT))
		{
			gpbImgCb.pHistBuf = NULL;
		}
		else
		{
			gpbImgCb.pHistBuf = &gpbHistBuf[0];
		}

		/*-- Exif Info */
		if (FAIL==appPbImgExifInfoGet(&gpbExifInfo))
		{
			gpbImgCb.pExifInfo = NULL;
		}
		else
		{
			gpbImgCb.pExifInfo = &gpbExifInfo;
		}
	}

	appDpofInit();
	gpbImgCb.isDpofSet = appDpofJobExistQuery(APP_DPOF_MODE_STD);
	if (gpbImgCb.isDpofSet)
	{
		gpbImgCb.dpofPrintNum = appDpofPrintQuantityGet(APP_DPOF_MODE_STD);
		gpbImgCb.isDpofStamp = appDpofDateStampExistQuery(APP_DPOF_MODE_STD);
	}

	return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  File Name : playVideoTitlePlayStart
 *------------------------------------------------------------------------*/
UINT32
playVideoTitlePlayStart(
	void
)
{
	UINT32 valU32;
#if PIP_32M
	UINT32 param,w=960,h=540;
#else
    #if CAM_TYPE_CVR
	UINT32 param,w=320,h=240;
    #else
	UINT32 param,w=1920,h=1080;
	#endif
#endif
	UINT32 selPos[4] = {0,0,w,h};
	UINT16 BGrgb=0;
#if CAM_TYPE_CVR
	uiPara_t* puiPara = appUiParaGet();
	UINT32 arr[4] = {8,73, 112, 63};	
	UINT32 hdmiArr[4] = {250,340, 480, 270};
	hdmiArr[0]=w/10;
	hdmiArr[1]=h/3;
	hdmiArr[2]=w/4;
	hdmiArr[3]=h/4;
#endif

	valU32 = valU32;
	param = param;
	BGrgb = BGrgb;
	/*-- Set MediaPlay filename :
	 * The filename should be set before parsing attribute !!
	 */
	if (FAIL==sp5kMediaPlayFileNameSet(&gpbImgCb.szDcfFileName[0]))
	{
		PB_LOG("%s: !!ERROR!!, Media filename set failed !! \n", __FUNCTION__);
		return FAIL;
	}

	#if 0 /*SPCA6330  */
	CLEAR_OSD_SRC;
	APP_OSD_REFRESH_ON;
	appModeSet(SP5K_MODE_STANDBY);
	#endif

	/*-- Check current SP5K Mode before playback Video
	 * %REVIEW% : Resolve this shit bug !?
	 */
	/*
	sp5kModeGet(&valU32);
	if (valU32==SP5K_MODE_STILL_PLAYBACK || valU32==SP5K_MODE_MULTI_PLAYBACK)
	{
		sp5kModeCfgSet(SP5K_MODE_CFG_STANDBY_DUP, 1);
	 	sp5kModeCfgSet(SP5K_MODE_CFG_STANDBY_FORCE, 1);
		appModeSet(SP5K_MODE_STANDBY);
		appHostMsgWaitExact(SP5K_MSG_MODE_READY,SP5K_MODE_STANDBY,5000);
	}
	*/
	/*-- Set mode to show Video QView image
	 */
	gpbMainCb.playStat = PB_PLAY_STAT_VIDEO_TITLE_PLAYING;
	gpbMainCb.isPlayDone = FALSE;
	gpbImgCb.isMainImgOK = gpbImgCb.isThumbImgOK = FALSE;
	#if CAM_TYPE_CVR	
	if (appActiveStateGet() == APP_STATE_PB_IMG_DEL ||      
		appActiveStateGet() == APP_STATE_PB_IMG_PROT || 
		appActiveStateGet() == APP_STATE_MENU ||
		puiPara->fileType == FILETYPE_JPG )
    #endif
	{
		#if !MSG_NEW_PROC  /* mask for mantis 46683*/
		sp5kModeSet(SP5K_MODE_VIDEO_PLAYBACK);
		appHostMsgWaitExact(SP5K_MSG_MODE_READY,SP5K_MODE_VIDEO_PLAYBACK,5000);
		#else
		appModeSet(SP5K_MODE_VIDEO_PLAYBACK);
		#endif
	}
	#if CAM_TYPE_CVR
	else
	{
		sp5kGfxAttrSet(SP5K_GFX_PIP_REFRESH_ACTIVE,0,0,0,0);
		if (IS_HDMI_IN)
		{

			appRsvFileScaleDraw(SP5K_GFX_PAGE_PIP_MAIN,SP5K_RES_TYPE_JPG,RES_JPG_PB_CVR_BGHDMI,10,0,1920,1080);
			//sp5kResourceFilePagePlay(SP5K_RES_TYPE_JPG, RES_JPG_PB_CVR_BGHDMI, SP5K_GFX_PAGE_PIP_MAIN, selPos, 0);
				if(FAIL == appRsvFileScaleDraw( SP5K_GFX_PAGE_PIP_MAIN, SP5K_RES_TYPE_AVI,gpbImgCb.szDcfFileName,100,310, 576, 324 ))
			{
					gpbMainCb.playStat = PB_PLAY_STAT_STANDBY;
					sp5kGfxAttrSet(SP5K_GFX_PIP_REFRESH_ACTIVE,1,0,1,0);
					return FAIL; 
			}	
		}
		else
		{
			sp5kResourceFilePagePlay(SP5K_RES_TYPE_JPG, RES_JPG_PB_CVR_BG, SP5K_GFX_PAGE_PIP_MAIN, selPos, 0);

			if(FAIL == sp5kResourceFilePagePlay(SP5K_RES_TYPE_AVI, gpbImgCb.szDcfFileName, SP5K_GFX_PAGE_PIP_MAIN, arr, 0))
			{
				gpbMainCb.playStat = PB_PLAY_STAT_STANDBY;
				sp5kGfxAttrSet(SP5K_GFX_PIP_REFRESH_ACTIVE,1,0,1,0);
				return FAIL; 
			}
		}
		sp5kGfxAttrSet(SP5K_GFX_PIP_REFRESH_ACTIVE,1,0,1,0);
	}
    #endif
	gpbMainCb.isPlayDone = TRUE;

    /*-- Parse Media Attr before playing :
	 */
    if (FAIL==sp5kMediaPlayAttrParse())
	{
		return FAIL;
	}

	/*-- Retrieve media information */
	sp5kMediaPlayAttrGet(SP5K_MEDIA_ATTR_DURATION, &gpbImgCb.mediaTotalTime);	
	printf("S gpbImgCb.mediaTotalTime = %d\n",gpbImgCb.mediaTotalTime);
	gpbImgCb.mediaTotalTime += 2;/*四舍五入*/
	printf("E gpbImgCb.mediaTotalTime = %d\n",gpbImgCb.mediaTotalTime);
	sp5kMediaPlayAttrGet(SP5K_MEDIA_ATTR_WIDTH, &gpbImgCb.imgWidth);
	sp5kMediaPlayAttrGet(SP5K_MEDIA_ATTR_HEIGHT, &gpbImgCb.imgHeight);

	#if KIT_WITH_HDMI
	if (IS_HDMI_IN && pipInitStatus)
	{
	sp5kRsvPreloadFile(SP5K_RES_TYPE_AVI, gpbImgCb.szDcfFileName, 0, w, h, 0);
	sp5kRsvPreloadDraw(0 , SP5K_GFX_PAGE_PIP_BASE+0, selPos, 0);
	sp5kGfxAttrSet(SP5K_GFX_PIP_REFRESH_ACTIVE,1,0,1,0);
	}
	#endif
	/*-- Determine sizeGrade */
	UINT32 i;
	for (i=0; i<UI_VID_SIZE_MAX; i++)
	{
		appVideoSizeGet(i, &w, &h);
		if (gpbImgCb.imgWidth==w && gpbImgCb.imgHeight==h)
		{
			gpbImgCb.sizeGrade = PB_VIDEO_SIZE_GRADE_1+i;
			break;
		}
	}
	if (i==UI_VID_SIZE_MAX)
	{
		gpbImgCb.sizeGrade = PB_VIDEO_SIZE_GRADE_UNKNOWN;
	}

	gpbMainCb.playStat = PB_PLAY_STAT_STANDBY;
	gpbMainCb.isPlayDone = TRUE;
	gpbImgCb.isMainImgOK = gpbImgCb.isThumbImgOK = TRUE;
	//appTimeDelay(100,1);
	PB_LOG("[VIDEO]: %s, total(%d), size(%d x %d) \n",
				gpbImgCb.szDcfFileName,
				gpbImgCb.mediaTotalTime,
				gpbImgCb.imgWidth,
				gpbImgCb.imgHeight);

	return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  File Name : playVideoAttrParamGet
 *------------------------------------------------------------------------*/
UINT32
playVideoAttrParamGet(
	void
)
{
#if CAM_TYPE_CVR/*jj add for FF+RW seamless play*/
#if PIP_32M
	UINT32 w=960,h=540;
#else
	UINT32 w=320,h=240;
#endif

	gpbImgCb.dcfIdx = gcurrDcfIdx;
	if (FAIL==playImgDcfAttrGet(&gpbImgCb))
	{
		return FAIL;
	}
	sp5kDcfFsCurFileIdxSet(gpbImgCb.dcfIdx);
	/*-- Set MediaPlay filename :
	 * The filename should be set before parsing attribute !!
	 */
	if (FAIL==sp5kMediaPlayFileNameSet(&gpbImgCb.szDcfFileName[0]))
	{
		PB_LOG("%s: !!ERROR!!, Media filename set failed !! \n", __FUNCTION__);
		return FAIL;
	}
	/*-- Set mode to show Video QView image
	 */
	gpbMainCb.playStat = PB_PLAY_STAT_VIDEO_TITLE_PLAYING;
	gpbMainCb.isPlayDone = FALSE;
	gpbImgCb.isMainImgOK = gpbImgCb.isThumbImgOK = FALSE;
	gpbMainCb.isPlayDone = TRUE;
    /*-- Parse Media Attr before playing :
	 */
    if (FAIL==sp5kMediaPlayAttrParse())
	{
		return FAIL;
	}

	/*-- Retrieve media information */
	sp5kMediaPlayAttrGet(SP5K_MEDIA_ATTR_DURATION, &gpbImgCb.mediaTotalTime);	
	printf("S gpbImgCb.mediaTotalTime = %d\n",gpbImgCb.mediaTotalTime);
	gpbImgCb.mediaTotalTime += 2;/*四舍五入*/
	printf("E gpbImgCb.mediaTotalTime = %d\n",gpbImgCb.mediaTotalTime);
	sp5kMediaPlayAttrGet(SP5K_MEDIA_ATTR_WIDTH, &gpbImgCb.imgWidth);

	sp5kMediaPlayAttrGet(SP5K_MEDIA_ATTR_HEIGHT, &gpbImgCb.imgHeight);
	/*-- Determine sizeGrade */
	UINT32 i;
	for (i=0; i<UI_VID_SIZE_MAX; i++)
	{
		appVideoSizeGet(i, &w, &h);
		if (gpbImgCb.imgWidth==w && gpbImgCb.imgHeight==h)
		{
			gpbImgCb.sizeGrade = PB_VIDEO_SIZE_GRADE_1+i;
			break;
		}
	}
	if (i==UI_VID_SIZE_MAX)
	{
		gpbImgCb.sizeGrade = PB_VIDEO_SIZE_GRADE_UNKNOWN;
	}

	gpbMainCb.playStat = PB_PLAY_STAT_STANDBY;
	gpbMainCb.isPlayDone = TRUE;
	gpbImgCb.isMainImgOK = gpbImgCb.isThumbImgOK = TRUE;
	//appTimeDelay(100,1);
	PB_LOG("[VIDEO]: %s, total(%d), size(%d x %d) \n",
				gpbImgCb.szDcfFileName,
				gpbImgCb.mediaTotalTime,
				gpbImgCb.imgWidth,
				gpbImgCb.imgHeight);
	#endif
	return SUCCESS;
}
/*-------------------------------------------------------------------------
 *  File Name : playAudioTitlePlayStart
 *------------------------------------------------------------------------*/
UINT32
playAudioTitlePlayStart(
	void
)
{
	PB_VERBOSE("%s \n", __FUNCTION__);

	/*-- Set MediaPlay filename :
	 * The filename should be set before parsing attribute !!
	 */
	if (FAIL==sp5kMediaPlayFileNameSet(&gpbImgCb.szDcfFileName[0]))
	{
		PB_DBG("%s: !!ERROR!!, Media filename set failed !! \n", __FUNCTION__);
		return FAIL;
	}

	/*-- Parse Media Attr before playing :
	 */
	if (FAIL==sp5kMediaPlayAttrParse())
	{
		PB_DBG("%s: !!ERROR!!, Media attr parsing failed !! \n", __FUNCTION__);
		return FAIL;
	}

	/*-- Retrieve media information, for UI Draw */
	sp5kMediaPlayAttrGet(SP5K_MEDIA_ATTR_DURATION, &gpbImgCb.mediaTotalTime);

	/*-- Draw Audio background JPG
	 */
	gpbMainCb.playStat = PB_PLAY_STAT_STANDBY;
	gpbImgCb.isMainImgOK = gpbImgCb.isThumbImgOK = FALSE;
	gpbMainCb.isPlayDone = FALSE;
	appPlayOsd_AudioBGDraw();
	gpbMainCb.playStat = PB_PLAY_STAT_STANDBY;
	gpbMainCb.isPlayDone = TRUE;
	gpbImgCb.isThumbImgOK = TRUE;

	sp5kHostMsgSend(APP_UI_MSG_PB_AUDIO_TITLE_DONE, 0);

	PB_LOG("[AUDIO]: %s, total(%d) \n",
				gpbImgCb.szDcfFileName,
				gpbImgCb.mediaTotalTime);


	return SUCCESS;
}


/*-------------------------------------------------------------------------
 *  File Name : appPbStillPlayDoneParse
 *------------------------------------------------------------------------*/
#define PB_DONE_MSG_GET_IMG_TYPE(p)	(((p) & 0xFFFF0000)>>16)
#define PB_DONE_MSG_GET_ERR_CODE(p)	((p) & 0x000000FF)
#define IS_PB_DONE_QV(p) 	(PB_DONE_MSG_GET_IMG_TYPE(p)==SP5K_SPB_IMG_QV)
#define IS_PB_DONE_MAIN(p) 	(PB_DONE_MSG_GET_IMG_TYPE(p)==SP5K_SPB_IMG_PRIMARY)
#define IS_PB_DONE_THUMB(p) (PB_DONE_MSG_GET_IMG_TYPE(p)==SP5K_SPB_IMG_THUMB)

UINT32
appPbStillPlayDoneParse(
	UINT32 param
)
{
	UINT32 ret, errCode;

	errCode = PB_DONE_MSG_GET_ERR_CODE(param);

	if (IS_PB_DONE_MAIN(param))
	{
		if (errCode==0x00FFUL)
		{
			ret = PB_DONE_MAIN_ABORT;
		}
		else if (errCode==0x00UL)
		{
			ret = PB_DONE_MAIN_FINISH;
		}
		else
		{
			ret = PB_DONE_MAIN_ERROR;
		}
	}
	else if (IS_PB_DONE_QV(param))
	{
		if (errCode==0x00FFUL)
		{
			ret = PB_DONE_QV_ABORT;
		}
		else if (errCode==0x00UL)
		{
			ret = PB_DONE_QV_FINISH;
		}
		else
		{
			ret = PB_DONE_QV_ERROR;
		}
	}
	else if (IS_PB_DONE_THUMB(param))
	{
		if (errCode==0x00FFUL)
		{
			ret = PB_DONE_THUMB_ABORT;
		}
		else if (errCode==0x00UL)
		{
			ret = PB_DONE_THUMB_FINISH;
		}
		else
		{
			ret = PB_DONE_THUMB_ERROR;
		}
	}
	else
	{
		ret = PB_DONE_UNKNOWN_ERROR;
	}

	return ret;
}


/*-------------------------------------------------------------------------
 *  File Name : playImgPlayReset
 *------------------------------------------------------------------------*/
void
playImgPlayReset(
	void
)
{
	PB_VERBOSE("%s \n", __FUNCTION__);

	gpbMainCb.lastFileType = gpbImgCb.fileType;

	memset(&gpbImgCb, 0, sizeof(pbImgCb_t));
}

/*-------------------------------------------------------------------------
 *  File Name : playImgDcfAttrGet
 *------------------------------------------------------------------------*/
UINT32
playImgDcfAttrGet (
	pbImgCb_t *pImgCb
)
{
	sp5kDcfAttrElm_t *pdcfAttr;
	UINT32 fileType;

	PB_VERBOSE("%s \n", __FUNCTION__);

	pdcfAttr = &pImgCb->dcfAttr;

	PB_VERBOSE("Getting Dcf Attr of index(%d) ...\n", pImgCb->dcfIdx);
	if (FAIL==sp5kDcfFsFileAttrGet(pImgCb->dcfIdx, pdcfAttr, &pImgCb->szDcfFileName[0]))
	{
		PB_DBG("%s: !! ERROR !!, getting attr failed \n", __FUNCTION__);
		pImgCb->isFileErr = TRUE;
		pImgCb->fileType = PB_FT_UNKNOWN;
		return FAIL;
	}

	if (pdcfAttr->attribute & SP5K_DCF_FILE_ATTR_LOCK)
	{
		PB_VERBOSE("%s: IMG is locked, %s \n", __FUNCTION__, &pImgCb->szDcfFileName[0]);
		pImgCb->isProtected = TRUE;
	}
	else
	{
		pImgCb->isProtected = FALSE;
	}

	fileType = pdcfAttr->fileType;
	if (!IS_SUPPORTED_FILETYPE(fileType))
	{
		PB_DBG("%s: !! WARNING !!, unknown filetype(0x%08x) \n", __FUNCTION__, pdcfAttr->fileType);
		pdcfAttr->fileType = 0;
		pImgCb->isFileErr = TRUE;
		pImgCb->fileType = PB_FT_UNKNOWN;
		return FAIL;
	}
	pImgCb->isFileErr = FALSE;

	if (IS_FILETYPE_JPG(fileType))
	{
		if (pdcfAttr->attribute & SP5K_DCF_FILE_ATTR_MEMO)
		{
			PB_VERBOSE("\n-->> JPG MEMO \n\n");
			pdcfAttr->fileType = SP5K_DCF_FILETYPE_JPG_MEMO;
			pImgCb->fileType = PB_FT_JPG_MEMO;
		}
		else
		{
			pImgCb->fileType = PB_FT_JPG;
		}
	}
	else if (IS_FILETYPE_WAV(fileType))
	{
		pImgCb->fileType = PB_FT_WAV;
	}
	else if (IS_FILETYPE_AVI(fileType))
	{
		pImgCb->fileType = PB_FT_AVI;
	}
	else if (IS_FILETYPE_MOV(fileType))
	{
		pImgCb->fileType = PB_FT_MOV;
	}
	else if (IS_FILETYPE_MP4(fileType))
	{
		pImgCb->fileType = PB_FT_MP4;
	}
	else
	{
		pImgCb->fileType = PB_FT_UNKNOWN;
	}

	return SUCCESS;
}


/*-------------------------------------------------------------------------
 *  File Name : playImgPlayStart
 *------------------------------------------------------------------------*/
UINT32
playImgPlayStart(
	void
)
{
	sp5kDcfAttrElm_t *pdcfAttr;

	PB_VERBOSE("%s \n", __FUNCTION__);

	pdcfAttr = &gpbImgCb.dcfAttr;

	sp5kDcfFsCurFileIdxSet(gpbImgCb.dcfIdx);

	if (IS_FILETYPE_JPG(pdcfAttr->fileType))
	{
		PB_DBG("[JPG] %s ...\n", &gpbImgCb.szDcfFileName[0]);
		return (playJpgPlayStart());
	}
	else if (IS_SUPPORTED_VIDEO_FILETYPE(pdcfAttr->fileType))
	{
		PB_DBG("[AVI] %s ...\n", &gpbImgCb.szDcfFileName[0]);
		return (playVideoTitlePlayStart());
	}
	else if (IS_SUPPORTED_AUDIO_FILETYPE(pdcfAttr->fileType))
	{
		PB_DBG("[WAV] %s ...\n", &gpbImgCb.szDcfFileName[0]);
		return (playAudioTitlePlayStart());
	}
	else
	{
		PB_DBG("%s: !!ERROR!!, Unsupported file type (%d) \n", __FUNCTION__, pdcfAttr->fileType);
		return FAIL;
	}

}


/*-------------------------------------------------------------------------
 *  File Name : playImageShift
 *------------------------------------------------------------------------*/
static
UINT32
playImageShift(
	UINT8	shiftDir,
	BOOL	isJpgOnly,
	BOOL	isSkipFileErr
)
{
	PB_VERBOSE("%s: (%s) \n", __FUNCTION__, (shiftDir==PB_SHIFT_NEXT ? "Next" : "Pre"));

lbl_image_shift_err_next_scan:
	gpbMainCb.err = PB_ERR_NONE;
	/*-- Go for next image
	 */
	if (isJpgOnly)
	{
		/*-- JPG IMG Only */
		if (FAIL==appPbScanNextJPG(shiftDir, isSkipFileErr))
		{
			PB_VERBOSE("%s: Next JPG Scan failed \n", __FUNCTION__);
			if (isSkipFileErr)
			{
				PB_LOG("\n!!FATAL!! @ %s,%d \n\n", __FUNCTION__, __LINE__);
			}
			return FAIL;
		}
	}
	else
	{
		/*-- Any FileType */
		if (shiftDir==PB_SHIFT_NEXT)
		{
			PB_INDEX_INC(gcurrDcfIdx, 1, gtotalImgCount);
		}
		else if (shiftDir==PB_SHIFT_PRE)
		{
			PB_INDEX_DEC(gcurrDcfIdx, 1, gtotalImgCount);
		}
		else
		{
			PB_DBG("!! FATAL !!, illegal shiftDir !! \n");
			return FAIL;
		}
	}

	/*-- Go and Play selected file
	 */
	gpbMainCb.postponeUI = 0;
	if (FAIL==appPbImagePlay(gcurrDcfIdx))
	{
		if (isSkipFileErr)
		{
			PB_VERBOSE("%s,%d: FileErr, go on next scan !! \n", __FUNCTION__, __LINE__);
			goto lbl_image_shift_err_next_scan;
		}
		else
		{
			gpbMainCb.err = PB_ERR_FILE_ERROR;
			gpbImgCb.isFileErr = TRUE;/*willow++ for clear the video"file error" when shift*/
			return FAIL;
		}
	}

	if (IS_FILETYPE_JPG(gpbImgCb.dcfAttr.fileType))
	{
		gpbMainCb.postponeUI=2;
		/*-- File playback has been fired, wait PLAYBACK_DONE
		 * to tell if this is a true playable file !!
		 * So, we have to postponeUI OSD till then !!
		 */
	}

	PB_VERBOSE("%s, exit !!\n", __FUNCTION__);

	return SUCCESS;
}


/*-------------------------------------------------------------------------
 *  File Name : appPbMediaUIStateGet
 *------------------------------------------------------------------------*/
UINT32
appPbMediaUIStateGet(
	void
)
{
	if ((gpbMainCb.mainStat==PB_MAIN_STAT_VIDEO_SUSPEND&&gpbMainCb.subStat==PB_VIDEO_STAT_PLAYING)
		||(gpbMainCb.mainStat==PB_MAIN_STAT_AUDIO_SUSPEND &&gpbMainCb.subStat==PB_AUDIO_STAT_PLAYING))
	{
		return PB_MEDIA_UI_STAT_PLAYING;
	}
	else if ((gpbMainCb.mainStat==PB_MAIN_STAT_VIDEO_SUSPEND&&gpbMainCb.subStat==PB_VIDEO_STAT_PAUSE)
		||(gpbMainCb.mainStat==PB_MAIN_STAT_AUDIO_SUSPEND &&gpbMainCb.subStat==PB_AUDIO_STAT_PAUSE)
		||(gpbMainCb.mainStat==PB_MAIN_STAT_MEMOREC_SUSPEND &&gpbMainCb.subStat==PB_MEMOREC_STAT_PAUSE))
	{
		return PB_MEDIA_UI_STAT_PAUSE;
	}
	else if ((gpbMainCb.mainStat==PB_MAIN_STAT_VIDEO_SUSPEND &&gpbMainCb.subStat==PB_VIDEO_STAT_FF))
	{
		return PB_MEDIA_UI_STAT_FF;
	}
	else if ((gpbMainCb.mainStat==PB_MAIN_STAT_VIDEO_SUSPEND &&gpbMainCb.subStat==PB_VIDEO_STAT_REW))
	{
		return PB_MEDIA_UI_STAT_REW;
	}
	else if ((gpbMainCb.mainStat==PB_MAIN_STAT_MEMOREC_SUSPEND &&gpbMainCb.subStat==PB_MEMOREC_STAT_RECORDING)	)
	{
		return PB_MEDIA_UI_STAT_RECORDING;
	}
	else if (gpbMainCb.subStat==PB_VIDEO_STAT_SLOW_MOTION)

	{
		return PB_MEDIA_UI_STAT_SLOWMOTION;
	}
	else
	{
		return PB_MEDIA_UI_STAT_STANDBY;
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appPbMsg_Clear
 *------------------------------------------------------------------------*/
void
appPbMsg_Clear(
	void
)
{
	appErrMsgShow(ERR_MSG_CLEAR, PB_ERR_SHOW_FOREVER);

	#if HOST_DBG
	PB_VERBOSE("\n\n");
	PB_VERBOSE("___MSG_CLEAR__\n");
	PB_VERBOSE("\n\n");
	#else
	PB_VERBOSE("\nMSG_CLEAR !!\n\n");
	#endif
}

/*-------------------------------------------------------------------------
 *  File Name : appPbMsg_BusyWait
 *------------------------------------------------------------------------*/
void
appPbMsg_BusyWait(
	void
)
{
	PB_OSD_CLR;
	PB_PIP_0_CLR;
	appErrMsgShow(ERR_MSG_BUSY_WAIT, PB_ERR_SHOW_FOREVER+ERR_MSG_SHOW_1SEC);

	#if HOST_DBG
	PB_VERBOSE("\n\n");
	PB_VERBOSE("___BUSY_WAIT__\n");
	PB_VERBOSE("\n\n");
	#else
	PB_VERBOSE("\nBUSY_WAIT !!\n\n");
	#endif
}

/*-------------------------------------------------------------------------
 *  File Name : appPbMsg_ImgSaved
 *------------------------------------------------------------------------*/
void
appPbMsg_ImgSaved(
	void
)
{
	appErrMsgShow(ERR_MSG_IMG_SAVED, ERR_MSG_SHOW_FOREVER);

	#if HOST_DBG
	PB_VERBOSE("\n\n");
	PB_VERBOSE("___IMG_SAVED__\n");
	PB_VERBOSE("\n\n");
	#else
	PB_VERBOSE("\nIMG_SAVED !!\n\n");
	#endif
}

/*-------------------------------------------------------------------------
 *  File Name : appPbMsg_Completed
 *------------------------------------------------------------------------*/
void
appPbMsg_Completed(
	void
)
{
	appErrMsgShow(ERR_MSG_COMPLETED, ERR_MSG_SHOW_FOREVER);

	#if HOST_DBG
	PB_VERBOSE("\n\n");
	PB_VERBOSE("___COMPLETED__\n");
	PB_VERBOSE("\n\n");
	#else
	PB_VERBOSE("\nCOMPLETED !!\n\n");
	#endif
}



/*-------------------------------------------------------------------------
 *  File Name : appPbErr_LockedIconBlink
 *------------------------------------------------------------------------*/
static
void
appPbErr_LockedIconBlink(
	void
)
{
	int i;

	for (i=0; i<4; i++) {
		appPlayOsd_ProtectDraw(TRUE); APP_SLEEP_MS(250);
		appPlayOsd_ProtectDraw(FALSE); APP_SLEEP_MS(250);
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appPbErr_FileError
 *------------------------------------------------------------------------*/
static
void
appPbErr_FileError(
	void
)
{
	gpbMainCb.err = PB_ERR_FILE_ERROR;
	//sp5kModeSet(SP5K_MODE_STANDBY);
	//PB_BG_SHOW;
		/*-- Draw PB_BG outside this funciton, to prevent from redraw */
	PB_OSD_CLR; PB_BLACK_BKGND;
	PB_PIP_0_CLR;
	#if CAM_TYPE_CVR
	{
		if (IS_HDMI_IN)
		{
			appOsd_ColorDrawShapeColorSet(PAL_BLACK, PAL_BLEND_100, PAL_BLACK, PAL_BLEND_100);
			appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, 66,112,160,90, 0, 0);
			
			appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
			appOsdLib_TextIDDraw(80,240,SP5K_GFX_ALIGN_TOP_LEFT,ID_STR_FILE_ERROR_);
			
			appOsdLib_IconFileDraw(SP5K_GFX_PAGE_OSD_0,114,140,SP5K_GFX_ALIGN_TOP_LEFT,ID_WARNICON,0x0000);
			sp5kGfxAttrSet(SP5K_GFX_PIP_REFRESH_ACTIVE,1,0,1,0);
			
		
		}
		else
		{
			appOsd_ColorDrawShapeColorSet(PAL_BLACK, PAL_BLEND_100, PAL_BLACK, PAL_BLEND_100);
			appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, 6,65, 120,73,0, 0);
			
			appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
			appOsdLib_TextIDDraw(18,158,SP5K_GFX_ALIGN_TOP_LEFT,ID_STR_FILE_ERROR_); 
			
			appOsdLib_IconFileDraw(SP5K_GFX_PAGE_OSD_0,40,80,SP5K_GFX_ALIGN_TOP_LEFT,ID_WARNICON,0x0000);
			sp5kGfxAttrSet(SP5K_GFX_PIP_REFRESH_ACTIVE,1,0,1,0);
		}
	}
	
    #endif
	#if HOST_DBG
	PB_DBG("\n\n");
	PB_DBG("___FILE_ERROR__\n");
	PB_DBG("\n\n");
	#else
	PB_DBG("\nFILE_ERROR !!\n\n");
	#endif
	
}

/*-------------------------------------------------------------------------
 *  File Name : appPbErr_FileTypeError
 *------------------------------------------------------------------------*/
static
void
appPbErr_FileTypeError(
	void
)
{
	gpbMainCb.err = PB_ERR_FILETYPE_ERROR;
	//sp5kModeSet(SP5K_MODE_STANDBY);
		/*-- Dont override the BG !! */
	PB_OSD_CLR;
	PB_PIP_0_CLR;
	appErrMsgWithSound(ERR_MSG_FILETYPE_ERROR, ERR_MSG_WITH_WARNING_SOUND, ERR_MSG_SHOW_2SEC);

	#if HOST_DBG
	PB_DBG("\n\n");
	PB_DBG("___FILE_TYPE_ERROR__\n");
	PB_DBG("\n\n");
	#else
	PB_DBG("\nFILE_TYPE_ERROR !!\n\n");
	#endif
}

/*-------------------------------------------------------------------------
 *  File Name : appPbErr_CopyError
 *------------------------------------------------------------------------*/
static
void
appPbErr_CopyError(
	void
)
{
	gpbMainCb.err = PB_ERR_COPY_ERROR;
	//sp5kModeSet(SP5K_MODE_STANDBY);
		/*-- Dont override the BG !! */
	PB_PIP_0_CLR;
	appErrMsgWithSound(ERR_MSG_COPY_ERROR, ERR_MSG_WITH_WARNING_SOUND, ERR_MSG_SHOW_2SEC);

	#if HOST_DBG
	PB_DBG("\n\n");
	PB_DBG("___COPY_ERROR__\n");
	PB_DBG("\n\n");
	#else
	PB_DBG("\nCOPY_ERROR !!\n\n");
	#endif
}

/*-------------------------------------------------------------------------
 *  File Name : appPbErr_FileLocked
 *------------------------------------------------------------------------*/
void
appPbErr_FileLocked(
	void
)
{
	gpbMainCb.err = PB_ERR_IMG_PROTECTED;
	gpbImgCb.isProtected = TRUE;
	appErrMsgWithSound(ERR_MSG_FILE_LOCKED, ERR_MSG_WITH_WARNING_SOUND, ERR_MSG_SHOW_FOREVER);

	#if HOST_DBG
	PB_DBG("\n\n");
	PB_DBG("___FILE_LOCKED__\n");
	PB_DBG("\n\n");
	#else
	PB_DBG("\nFILE_LOCKED !!\n\n");
	#endif
}

/*-------------------------------------------------------------------------
 *  File Name : appPbErr_NoImage
 *------------------------------------------------------------------------*/
static
void
appPbErr_NoImage(
	void
)
{
	gpbMainCb.err = PB_ERR_NO_IMAGE;
	gpbMainCb.isNoImage = TRUE;

	PB_OSD_CLR;
	PB_PIP_0_CLR;
		/*-- Host Shall take care of Background IMG to enhance performance !! */
	appErrMsgShow(ERR_MSG_NO_IMG, PB_ERR_SHOW_FOREVER);

	#if HOST_DBG
	PB_DBG("\n\n");
	PB_DBG("___NO_IMG__\n");
	PB_DBG("\n\n");
	#else
	PB_DBG("\nNO IMG !!\n\n");
	#endif
}

/*-------------------------------------------------------------------------
 *  File Name : appPbErr_DiskFull
 *------------------------------------------------------------------------*/
void
appPbErr_DiskFull(
	void
)
{
	gpbMainCb.err = PB_ERR_DISK_FULL;
	gpbDiskCb.isDiskFull = TRUE;
	//sp5kModeSet(SP5K_MODE_STANDBY);
	PB_OSD_CLR;
	PB_PIP_0_CLR;
	appErrMsgWithSound(ERR_MSG_MEMORY_FULL, ERR_MSG_WITH_WARNING_SOUND, ERR_MSG_SHOW_2SEC);

	#if HOST_DBG
	PB_DBG("\n\n");
	PB_DBG("___MEM_FULL__\n");
	PB_DBG("\n\n");
	#else
	PB_DBG("\nMEM_FULL !!\n\n");
	#endif
}

/*-------------------------------------------------------------------------
 *  File Name : appPbErr_DcfFull
 *------------------------------------------------------------------------*/
void
appPbErr_DcfFull(
	void
)
{
	gpbMainCb.err = PB_ERR_DCF_KEY_FULL;
	gpbDiskCb.isDcfFull = TRUE;
	PB_OSD_CLR;
	PB_PIP_0_CLR;
	appErrMsgWithSound(ERR_MSG_MEMORY_FULL, ERR_MSG_WITH_WARNING_SOUND, ERR_MSG_SHOW_2SEC);

	#if HOST_DBG
	PB_DBG("\n\n");
	PB_DBG("___DCF_FULL__\n");
	PB_DBG("\n\n");
	#else
	PB_DBG("\nDCF_FULL !!\n\n");
	#endif
}


/*-------------------------------------------------------------------------
 *  File Name : appPbErr_CardLock
 *------------------------------------------------------------------------*/
void
appPbErr_CardLock(
	void
)
{
	gpbMainCb.err = PB_ERR_CARD_LOCKED;
	PB_OSD_CLR;
	PB_PIP_0_CLR;
	appErrMsgWithSound(ERR_MSG_CARD_LOCKED, ERR_MSG_WITH_WARNING_SOUND, ERR_MSG_SHOW_2SEC);

	#if HOST_DBG
	PB_DBG("\n\n");
	PB_DBG("___CARD_LOCKED__\n");
	PB_DBG("\n\n");
	#else
	PB_DBG("\nCARD_LOCKED !!\n\n");
	#endif

}

/*-------------------------------------------------------------------------
 *  File Name : appPbErr_CardError
 *------------------------------------------------------------------------*/
void
appPbErr_CardError(
	void
)
{
	gpbMainCb.err = PB_ERR_CARD_ERROR;

	//sp5kModeSet(SP5K_MODE_STANDBY);
	PB_OSD_CLR; //PB_BLACK_BKGND;
	PB_PIP_0_CLR;
	appErrMsgWithSound(ERR_MSG_CARD_ERROR, ERR_MSG_WITH_WARNING_SOUND, PB_ERR_SHOW_FOREVER);

	#if HOST_DBG
	PB_DBG("\n\n");
	PB_DBG("___CARD_ERROR__\n");
	PB_DBG("\n\n");
	#else
	PB_DBG("\nCARD_ERROR !!\n\n");
	#endif
}


/*-------------------------------------------------------------------------
 *  File Name : appPbErr_CardNotExist
 *------------------------------------------------------------------------*/
void
appPbErr_CardNotExist(
	void
)
{
	gpbMainCb.err = PB_ERR_CARD_NOT_READY;
	PB_OSD_CLR;
	PB_PIP_0_CLR;
	appErrMsgWithSound(ERR_MSG_NO_MEMORY_CARD, ERR_MSG_WITH_WARNING_SOUND, ERR_MSG_SHOW_2SEC);

	#if HOST_DBG
	PB_DBG("\n\n");
	PB_DBG("___CARD_NOT_EXIST__\n");
	PB_DBG("\n\n");
	#else
	PB_DBG("\nCARD_NOT_EXIST !!\n\n");
	#endif
}
/*-------------------------------------------------------------------------
 *  File Name : appPbErr_C2CNotSupport
 *------------------------------------------------------------------------*/
void
appPbErr_C2CNotSupport(
	void
)
{
	gpbMainCb.err = PB_ERR_NOSUPPORT_C2C;
	PB_OSD_CLR;
	PB_PIP_0_CLR;
	appErrMsgWithSound(ERR_MSG_NOTSUPPORT_C2C, ERR_MSG_WITH_WARNING_SOUND, ERR_MSG_SHOW_2SEC);

	#if HOST_DBG
	PB_DBG("\n\n");
	PB_DBG("___NOT_SUPPORT_C2C__\n");
	PB_DBG("\n\n");
	#else
	PB_DBG("\nCNOT_SUPPORT_C2C !!\n\n");
	#endif
}

/*-------------------------------------------------------------------------
 *  File Name : appPbErrorProcess
 *------------------------------------------------------------------------*/
void
appPbErrorProcess(
	UINT32 err
)
{
	PB_VERBOSE("%s: err(%d) \n", __FUNCTION__, err);

	if (err==PB_ERR_DISK_ERROR)
	{
		if (IS_CARD_EXIST)
		{
			err = PB_ERR_CARD_ERROR;
			gpbDiskCb.isCardMounted = FALSE;
		}
		else
		{
			err = PB_ERR_NAND_ERROR;
			gpbDiskCb.isNandMounted = FALSE;
		}
	}

	switch (err)
	{
	default:
		PB_VERBOSE("%s: err(%d) not defined !! \n", __FUNCTION__, err);
		break;
	case PB_ERR_NONE:
//		appErrMsgShow(ERR_MSG_CLEAR, PB_ERR_SHOW_FOREVER);
		break;
	case PB_ERR_CARD_LOCKED:
		appPbErr_CardLock();
		break;
	case PB_ERR_DISK_ERROR:
	case PB_ERR_CARD_ERROR:
	case PB_ERR_NAND_ERROR:
		appPbShowBG();
		appPbErr_CardError();
		break;
	case PB_ERR_CARD_NOT_READY:
		appPbErr_CardNotExist();
		break;
	case PB_ERR_DISK_FULL:
		appPbErr_DiskFull();
		break;
	case PB_ERR_DCF_KEY_FULL:
		appPbErr_DcfFull();
		break;
	case PB_ERR_NO_IMAGE:
		/*-- For performance sake, Host shall draw PB_BG by before appPbErrorProcess !!
		 */
		appPbShowBG();
		appPbErr_NoImage();
		break;
	case PB_ERR_NO_IMAGE_SPLASH:
		appPbShowBG();
		appPbErr_NoImage();
		APP_SLEEP_MS(2000);
		break;
	case PB_ERR_IMG_PROTECTED:
		PB_OSD_CLR;
		PB_PIP_0_CLR;
		appPbErr_FileLocked();
		appPbErr_LockedIconBlink();
		appErrMsgShow(ERR_MSG_CLEAR, ERR_MSG_SHOW_FOREVER);
		break;
	case PB_ERR_FILE_ERROR:
		appPbErr_FileError();
		break;
	case PB_ERR_FILETYPE_ERROR:
		appPbErr_FileTypeError();
		break;
	case PB_ERR_COPY_ERROR:
		appPbErr_CopyError();
		break;
	case PB_ERR_NOSUPPORT_C2C:
		appPbShowBG();
		appPbErr_C2CNotSupport();
		break;
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appPbBytes2ClusterCalculate
 *------------------------------------------------------------------------*/
UINT32
appPbBytes2ClusterCalculate(
	UINT32 bytes
)
{
	UINT32 clusNum;

	PB_VERBOSE("%s: bytes(%d) \n", __FUNCTION__, bytes);

	clusNum = appBytes2ClusterCalculate(bytes);
	PB_LOG("bytes(%d), clusNum(%d) \n", bytes, clusNum);

	return clusNum;
}

/*-------------------------------------------------------------------------
 *  File Name : appPbDiskCheckup
 *------------------------------------------------------------------------*/
void
appPbDiskCheckup(
	pbDiskCB_t *pdskCb
)
{
	/*-- Note : DISK shall be ready before calling this function !!
	 */
	UINT32 actDsk;
	appDiskInfo_t *pdskInfo;

	PB_VERBOSE("%s \n", __FUNCTION__);

	/*-- Retrieve current disk info */
	pdskInfo = &pdskCb->dskInfo;
	memcpy(pdskInfo, appCurDiskInfoGet(), sizeof(appDiskInfo_t));

	actDsk = pdskInfo->actDsk;

	/*-- CARD Locked */
	pdskCb->isCardLock = FALSE;
	if (actDsk==SP5K_DRIVE_SD && IS_CARD_LOCK)
	{
		pdskCb->isCardLock = TRUE;
	}

	/*-- DISK Full */
	pdskCb->isDiskFull = FALSE;
	if (pdskInfo->freeSz < (pdskInfo->cluSz >>10)+PB_DISK_DPOF_RESERVED)
	{
		pdskCb->isDiskFull = TRUE;
	}

	/*-- DCF Key Full */
	pdskCb->isDcfFull = FALSE;
	if (IS_DCF_KEY_FULL)
	{
		pdskCb->isDcfFull = TRUE;
		pdskCb->isDiskFull = TRUE; /* regarded DISK_FULL while DCF_FULL */
	}

	/*-- CARD or NAND */
	pdskCb->isNandMounted = IS_DISK_NAND_MOUNT;
	pdskCb->isCardMounted = IS_DISK_SD_MOUNT;

}

/*-------------------------------------------------------------------------
 *  File Name : appPbDiskFreeClusterGet
 *------------------------------------------------------------------------*/
UINT32
appPbDiskFreeClusterGet(
	UINT32 actDsk
)
{
	UINT32 remaining;

	remaining = appDiskFreeClusterGet(actDsk);
	PB_LOG("drive(%d), free clust(%d) \n", actDsk, remaining);

	return remaining;
}


/*-------------------------------------------------------------------------
 *  File Name : appPbEditSaveDiskCheckup
 *------------------------------------------------------------------------*/
UINT32
appPbEditSaveDiskCheckup(
	pbImgCb_t *pimgCb
)
{
	sp5kDcfAttrElm_t *pdcfAttr;
	UINT32 freeClust, fileClust;

	PB_VERBOSE("%s: checking %s \n", __FUNCTION__, &pimgCb->szDcfFileName[0]);

	pdcfAttr = &pimgCb->dcfAttr;

	/*-- Check remaining disk size for new file !!
	 */
	/*-- Disk remaining clusters */
	freeClust = appPbDiskFreeClusterGet(gpbDiskCb.dskInfo.actDsk);
	PB_VERBOSE("%s: Disk free clust (%d) \n", __FUNCTION__, freeClust);

	/*-- Calculate clusters of current image
	 */
	fileClust = appPbBytes2ClusterCalculate(pdcfAttr->filesize);

	if (fileClust > freeClust)
	{
		return FAIL;
	}

	return SUCCESS;
}


UINT32 appPbImageOrientGet(void)
{
        UINT32 ret = SUCCESS;
        UINT32 orientTag = 0;
        UINT32 orient = SP5K_ROTATE_0;

        if((IS_FILETYPE_JPG(gpbImgCb.dcfAttr.fileType)))
        {
                ret = sp5kExifImageInfoGet(SP5K_IMG_PRIMARY, SP5K_IMG_ATTR_ORIENT, (UINT32 *)&(orientTag));
                if(orientTag == 6)
                {  /* exif 270-degree. */
                        orient = SP5K_ROTATE_270;
                }
                else if(orientTag == 3)
                {  /* exif 180-degree. */
                        orient = SP5K_ROTATE_180;
                }
                else if(orientTag == 8)
                {  /* exif 90-degree. */
                        orient = SP5K_ROTATE_90;
                }
                else
                {  /* exif 0-degree. */
                        orient = SP5K_ROTATE_0;
                }
        }

        return orient;
}


/*-------------------------------------------------------------------------
 *  File Name : appPbImageOrientDimGet
 *------------------------------------------------------------------------*/
pbImageOrientDim_t
appPbImageOrientDimGet(
	UINT16 width,
	UINT16 height
)
{
	UINT32 ratio;
	if((width==0) || (height==0))
	{
		return PB_IMG_ORIENT_UNKNOWN;
	}

	ratio=((width*100)/height);
#if PLAYBACK_ROTATE_BY_EXIF_TAG
	if ((appPbImageOrientGet() == SP5K_ROTATE_0)
		||(appPbImageOrientGet() == SP5K_ROTATE_180))
	{
		if((ratio>=185) || (ratio<=120))
		{
			return PB_IMG_ORIENT_UNKNOWN;
		}

		if(ratio>160)
		{
			/*-- 1.777778 */
			return PB_IMG_ORIENT_16X9;
		}
		else if((ratio <= 160) && (ratio > 140))
		{
			/*-- 1.50 */
			return PB_IMG_ORIENT_3X2;
		}
		else
		{
			/*-- 1.33333 */
			return PB_IMG_ORIENT_4X3;
		}
	}
	else
	{
		/*-- PORTRAIT (Vertical) image
		 */
		if((ratio>=185) || (ratio<=120))
		{
			return PB_IMG_ORIENT_UNKNOWN;
		}

		if(ratio>160)
		{
			/*-- 1.777778 */
			return PB_IMG_ORIENT_9X16;
		}
		else if((ratio <= 160) && (ratio > 140))
		{
			/*-- 1.50 */
			return PB_IMG_ORIENT_2X3;
		}
		else
		{
			/*-- 1.33333 */
			return PB_IMG_ORIENT_3X4;
		}
	}


#else
	if (width>=height)
	{
		if((ratio>=185) || (ratio<=120))
		{
			return PB_IMG_ORIENT_UNKNOWN;
		}

		if(ratio>160)
		{
			/*-- 1.777778 */
			return PB_IMG_ORIENT_16X9;
		}
		else if((ratio <= 160) && (ratio > 140))
		{
			/*-- 1.50 */
			return PB_IMG_ORIENT_3X2;
		}
		else
		{
			/*-- 1.33333 */
			return PB_IMG_ORIENT_4X3;
		}
	}
	else
	{
		/*-- PORTRAIT (Vertical) image
		 */
		if((ratio >= 80) || (ratio <= 45))
		{
			return PB_IMG_ORIENT_UNKNOWN;
		}

		if(ratio < 60)
		{
			/*-- 0.5625 */
			return PB_IMG_ORIENT_9X16;
		}
		else if((ratio >= 60 ) && (ratio < 70))
		{
			/*-- 0.66667 */
			return PB_IMG_ORIENT_2X3;
		}
		else
		{
			/*-- 0.75 */
			return PB_IMG_ORIENT_3X4;
		}
	}
#endif
}

/*-------------------------------------------------------------------------
 *  File Name : appPbImageOrientDimGet
 *------------------------------------------------------------------------*/
pbImageSizeGrade_t
appPbImageSizeGradeGet(
	UINT16 width,
	UINT16 height,
	UINT8 ratioType
)
{
	UINT32 length;
	if(ratioType)//90 or 270
	{
		length = height;
	}
	else//0 or 180
	{
		 length = width;
	}

	if(length > 3000)
	{
		if(width*3 == height*4)
		{
			if(length > 3456)
			{
				return PB_IMG_SIZE_GRADE_10;
			}
			else if(length > 3264)
			{
				return PB_IMG_SIZE_GRADE_9;
			}
			else if(length > 3072)
			{
				return PB_IMG_SIZE_GRADE_8;
			}
			else
			{
				return PB_IMG_SIZE_GRADE_7;
			}
		}
		else
		{
			return PB_IMG_SIZE_GRADE_7;
		}
	}
	else if(length > 2000)
	{
		if(length > 2560)
		{
			return PB_IMG_SIZE_GRADE_6;
		}
		else if(length > 2272)
		{
			return PB_IMG_SIZE_GRADE_5;
		}
		else if(length > 2048)
		{
			return PB_IMG_SIZE_GRADE_4;
		}
		else
		{
			return PB_IMG_SIZE_GRADE_3;
		}
	}
	else
	{
		if(length > 1280)
		{
			return PB_IMG_SIZE_GRADE_2;
		}
		else if(length > 640)
		{
			return PB_IMG_SIZE_GRADE_1;
		}
		else
		{
			return PB_IMG_SIZE_GRADE_0;
		}
	}

}

/*-------------------------------------------------------------------------
 *  File Name : appPbJpgMainPlayStart
 *------------------------------------------------------------------------*/
UINT32
appPbJpgMainPlayStart(
	void
)
{
	UINT32 ret=FAIL;

	if (!IS_MAIN_IMG_EXIST(gpbImgCb.exifImgFlag))
	{
		return FAIL;
	}

	gpbMainCb.playStat = PB_PLAY_STAT_JPG_MAIN_PLAYING;
	sp5kStillPlayCfgSet(SP5K_SPB_IMG_TYPE, SP5K_SPB_IMG_PRIMARY);
	sp5kModeSet(SP5K_MODE_STILL_PLAYBACK);
	ret=appHostMsgWaitExact(SP5K_MSG_STILLPLAY_DONE,(SP5K_SPB_IMG_PRIMARY&0xff)<<16,5000);
	if (ret ==FAIL)
	{
		gpbImgCb.isMainImgOK = FALSE;
		gpbMainCb.isPlayDone = FALSE;
		gpbMainCb.playStat = PB_PLAY_STAT_STANDBY;
		gpbMainCb.mainStat = PB_MAIN_STAT_STANDBY;
	}
	else
	{
		gpbMainCb.isPlayDone = TRUE;
		gpbImgCb.isMainImgOK = TRUE;
		gpbMainCb.mainStat = PB_MAIN_STAT_STANDBY;
		gpbMainCb.playStat = PB_PLAY_STAT_STANDBY;
	}

	return SUCCESS;

}

/*-------------------------------------------------------------------------
 *  File Name : appPbImagePlay
 *------------------------------------------------------------------------*/
UINT32
appPbImagePlay(
	UINT32 dcfIdx
)
{
	PB_VERBOSE("%s: index(%d) \n", __FUNCTION__, dcfIdx);

	/*-- Reset current IMG info struct, gpbImgCb
	 */
	playImgPlayReset();

	/*-- Retrieve DCF attributes of current IMG
	 */
	gpbImgCb.dcfIdx = gcurrDcfIdx = dcfIdx;
	if (FAIL==playImgDcfAttrGet(&gpbImgCb))
	{
		return FAIL;
	}

	/*-- Start to playback IMG
	 */
	if (FAIL==playImgPlayStart())
	{
		return FAIL;
	}

	return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  File Name : appPbImageShift_PreCheck
 *------------------------------------------------------------------------*/
UINT32
appPbImageShift_PreCheck(
	void
)
{
	if (gtotalImgCount<2)
	{
		return FAIL;
	}

	/*-- Check if Playback on-going
	 */
	if (IS_PB_JPG_PLAYING)
	{
		if (gpbImgCb.isThumbImgOK)
		{
			/*-- Thumb or QV has been played, abort MAIN playback */
			sp5kStillPlayAbort();
		}
		else
		{
			/*-- wait till thumb/QV ready, or MAIN completes */
			gpbMainCb.err = PB_ERR_PLAYING_BUSY;
			return FAIL;
		}
	}
	else if (IS_PB_VIDEO_TITLE_PLAYING || IS_PB_AUDIO_TITLE_PLAYING)
	{
		gpbMainCb.err = PB_ERR_PLAYING_BUSY;
		return FAIL;
	}

	return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  File Name : appPbImageShiftRepeatCheck
 *------------------------------------------------------------------------*/
void
appPbImageShiftRepeatCheck(UINT32 dumbMs)
{
	if (dumbMs)
	{
		PB_SLEEP(dumbMs);
	}

	if(IS_KEY_LEFT_PRESSED)
	{
		sp5kHostMsgSend(APP_UI_MSG_PB_SHIFT_PRE, 0);
	}
	else if(IS_KEY_RIGHT_PRESSED)
	{
		sp5kHostMsgSend(APP_UI_MSG_PB_SHIFT_NEXT, 0);
	}
	else
	{
	}

}
/*-------------------------------------------------------------------------
 *  File Name : appPbImageShift
 *------------------------------------------------------------------------*/
UINT32
appPbImageShift(
	UINT8	shiftDir
)
{
	return playImageShift(shiftDir, FALSE/*isJpgOnly*/, FALSE/*isSkipFileErr*/);
}

/*-------------------------------------------------------------------------
 *  File Name : appPbJpgImageShift
 *------------------------------------------------------------------------*/
UINT32
appPbJpgImageShift(
	UINT8	shiftDir,
	BOOL isSkipFileErr
)
{
	return playImageShift(shiftDir, TRUE/*isJpgOnly*/, isSkipFileErr);
}

/*-------------------------------------------------------------------------
 *  File Name : appPbImageDcfSave
 *------------------------------------------------------------------------*/
UINT32
appPbImageDcfSave(
	UINT8 saveOpt
)
{
	if (FAIL==sp5kImageEdit(SP5K_IMGEDIT_SAVE| SP5K_IMGEDIT_ATTR_TIME_SET, 0/*DCF*/, (saveOpt==PB_SAVE_NEW ? 1: 0), 0, 0))
	{
		return FAIL;
	}

	return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  File Name : appPbImgExifInfoGet
 *------------------------------------------------------------------------*/
void
appPbImgExifDateTimeGet(
	UINT8 *pszDTBuf /* min size : 24 */
)
{
	sp5kExifDecodeTagValueGet(EXIF_FORMAT_EXIF, 0x9003, (UINT8 *)pszDTBuf);
}

/*-------------------------------------------------------------------------
 *  File Name : appPbImgExifInfoGet
 *------------------------------------------------------------------------*/
UINT32
appPbImgExifInfoGet(
	pbImgExifInfo_t *pexifInfo
)
{
	memset(pexifInfo, 0 , sizeof(pbImgExifInfo_t));

	sp5kExifDecodeTagValueGet(EXIF_FORMAT_EXIF, 0xA002, (UINT8 *)&pexifInfo->imgWidth);
	sp5kExifDecodeTagValueGet(EXIF_FORMAT_EXIF, 0xA003, (UINT8 *)&pexifInfo->imgHeight);
	sp5kExifDecodeTagValueGet(EXIF_FORMAT_EXIF, 0x829A, (UINT8 *)&pexifInfo->expTime);
	sp5kExifDecodeTagValueGet(EXIF_FORMAT_EXIF, 0x829D, (UINT8 *)&pexifInfo->fNum);
	sp5kExifDecodeTagValueGet(EXIF_FORMAT_EXIF, 0x8827, (UINT8 *)&pexifInfo->iso);
	sp5kExifDecodeTagValueGet(EXIF_FORMAT_EXIF, 0x9209, (UINT8 *)&pexifInfo->flash);
	sp5kExifDecodeTagValueGet(EXIF_FORMAT_EXIF, 0x9003, (UINT8 *)&pexifInfo->szDate);
	sp5kExifDecodeTagValueGet(EXIF_FORMAT_EXIF, 0x9204, (UINT8 *)&pexifInfo->expBias);
	sp5kExifDecodeTagValueGet(EXIF_FORMAT_EXIF, 0xA403, (UINT8 *)&pexifInfo->wbMode);
	sp5kExifDecodeTagValueGet(EXIF_FORMAT_EXIF, 0x9208, (UINT8 *)&pexifInfo->lightSource);
	sp5kExifDecodeTagValueGet(EXIF_FORMAT_EXIF, 0x9207, (UINT8 *)&pexifInfo->meteringMode);
	sp5kExifDecodeTagValueGet(EXIF_FORMAT_EXIF, 0xA406, (UINT8 *)&pexifInfo->sceneMode);
	sp5kExifDecodeTagValueGet(EXIF_FORMAT_EXIF, 0x8822, (UINT8 *)&pexifInfo->expProgMode);

	return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  File Name : appPbImgHistogramGet
 *------------------------------------------------------------------------*/
UINT32
appPbImgHistogramGet(
	UINT32	*pLumaBuf,	/* the buf ptr to store the calcualted histogram */
	UINT32	barNum,		/* number of statistics to be shown */
	UINT32  maxVal		/* designated max luma value */
)
{
	/*-- Descriptions :
	 * The Histogram table presents the distributions of Y(Luma) of the image.
	 * The X-axis represents the quantized luma spread.
	 * For example, if barNum=64, then X=0 depicts those luma from 0~(256/64).
	 * The Y-axis represnts the pixel numbers spreading in the luma range.
	 */
	//UINT32 ret = SUCCESS;
	UINT32 x,y,idx,barWidth;
	UINT32 imgW,imgH,tmpHistData[PB_HISTOGRAM_MAX_VALUE];
	UINT32 countMax,tmpCount;
	UINT64 tmpSum;
	UINT32 imgYuvAddr;
	UINT8 *pbYuvData = NULL;
	UINT8 value;

	/*-- Calculate the Luma range of a 'bar' at X-axis */
	if (barNum > PB_HISTOGRAM_MAX_VALUE)
	{
		barNum = PB_HISTOGRAM_MAX_VALUE;
	}
	barWidth = PB_HISTOGRAM_MAX_VALUE / barNum;

	PB_VERBOSE("barNum = %d\n", barNum);
	PB_VERBOSE("barWidth  = %d\n", barWidth );

	/*-- Get Image YUV buffer address and dimension */
	sp5kStillPlayYUVInfoGet(SP5K_SPB_IMG_QV,&imgYuvAddr,&imgW,&imgH);

	#if HOST_DBG
	PB_VERBOSE("PB_HISTO_BUFF_ADDR = 0x%x\n", imgYuvAddr);
	PB_VERBOSE("PB_HISTO_BUFF_W = %d\n",imgW);
	PB_VERBOSE("PB_HISTO_BUFF_H = %d\n",imgH);
	#endif

	pbYuvData = (UINT8 *)imgYuvAddr;
	if ((pbYuvData == NULL) || (imgW == 0) || (imgH == 0))
	{
		PB_LOG("\nPB_HistogramGet() ERROR!!\n");
		return FAIL;
	}

	/*-- Firstly, find out the luma statistics */
	memset(tmpHistData, 0, PB_HISTOGRAM_MAX_VALUE*sizeof(tmpHistData[0]));
	for(y=0; y<imgH; y++)
	{
		idx = y * imgW * 2;
			/*-- jump to next raw, each raw occupies W*2,
			 * because one pixel takes two bytes
			 */
		tmpCount = 0;
		for (x=0; x< imgW; x++)
		{
			value = *(pbYuvData + idx);
			tmpHistData[value]++;
			tmpCount++;
			/*-- Jump for 8Y4U4V buffer format */
			if ((tmpCount>0)&& ((tmpCount%8) == 0))
			{
				idx = idx + 8;
				tmpCount = 0;
			}
			idx++;
		}
	}

	#if HOST_DBG
	PB_VERBOSE("\n======== raw tmpHistData[] data ==========\n");
	for (x=0; x<PB_HISTOGRAM_MAX_VALUE/8; x++) {
		PB_VERBOSE("\n[%03d]: ", x*8);
		for (y=0; y<8;  y++) {
			PB_VERBOSE("%6d ", tmpHistData[8*x+y]);
		}
	}
	PB_VERBOSE("\n\n");
	#endif

	/*-- Normalize the luma statistics :
	 * The original value in tmpHistData[x] is accumulated from whole image.
	 * There would be a problem that the larger the image size,
	 * the larger accumulation for the same scene the image is shot.
	 * Therefore, we have to normalize the luma statistics.
	 * (here, we take the LCD size as the target).
	 */
	for (x=0; x<PB_HISTOGRAM_MAX_VALUE; x++)
	{
		tmpHistData[x] = (tmpHistData[x] * (320*240)) / (imgW * imgH);
		//printf("tmpHistData[%d]=%d\n",x,tmpHistData[x]);
	}

	#if HOST_DBG
	for (x=0; x<PB_HISTOGRAM_MAX_VALUE/8; x++)
	{
		PB_VERBOSE("\n[%03d]: ", x*8);
		for (y=0; y<8;  y++)
		{
			PB_VERBOSE("%6d ", tmpHistData[8*x+y]);
		}
	}
	PB_VERBOSE("\n\n");
	#endif


	/*-- Now, Go on and accumulate the pixel number a bar contains */
	idx = countMax = 0;
	for (x=0; x<PB_HISTOGRAM_MAX_VALUE; x++)
	{
		tmpSum = tmpHistData[x];
		for (y=1; y<barWidth; y++)
		{
			tmpSum += tmpHistData[x+y];
		}
		x += barWidth - 1;
		pLumaBuf[idx] = tmpSum /barWidth;
		if (pLumaBuf[idx] > countMax)
		{
			countMax = pLumaBuf[idx];
		}
		idx ++;
	}
	barNum = idx;
	PB_VERBOSE("countMax = %d\n", countMax);

	/*-- equalization */
	for (x=0; x<barNum; x++)
	{
		pLumaBuf[x] = (pLumaBuf[x] * (maxVal)) / countMax;
		//printf("pbHistoData[%d]=%d\n",x,pbHistoData[x]);
	}

	#if HOST_DBG
	PB_VERBOSE("\n======== Histgorm data ==========\n");
	for (x=0; x<barNum/8; x++)
	{
		PB_VERBOSE("\n[%03d]: ", x*8);
		for (y=0; y<8;  y++)
		{
			PB_VERBOSE("%4d ", pLumaBuf[8*x+y]);
		}
	}
	PB_VERBOSE("\n\n");
	#endif

	return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  File Name : appPbTotalFileCountGet
 *------------------------------------------------------------------------*/
UINT32
appPbTotalFileCountGet(
	void
)
{
	UINT32 totalFileCount = 0;
	/*uiPara_t* puiPara = appUiParaGet();*/

	#if SP5K_CDFS_OPT
	appCdfsFileNumGet(&totalFileCount);
	#else
	/*if((puiPara->fileType == FILETYPE_VIDEO)||(puiPara->fileType == FILETYPE_EVENT))
	{
		sp5kDcfFsFileTypeSet(SP5K_DCF_FILETYPE_AVI);
		sp5kDcfFsFileTypeSet(SP5K_DCF_FILETYPE_MOV);
	}
	else
	{
		sp5kDcfFsFileTypeSet(SP5K_DCF_FILETYPE_JPG);
	}*/
	sp5kDcfFsInfoGet(SP5K_DCF_INFO_TOTAL_FILENUM,&totalFileCount);
	#endif
	return (totalFileCount);
}

/*-------------------------------------------------------------------------
 *  File Name : appPbScanNextJPG
 *------------------------------------------------------------------------*/
UINT32
appPbScanNextJPG(
	pbImageShiftDir_t scanDir,
	BOOL isSkipFileErr
)
{
	UINT32 dcfIdx;
	UINT32 scanCount;
	UINT32 scanResult;
	pbImgCb_t imgCb;

	if (gtotalImgCount<2)
	{
		PB_VERBOSE("%s: !!warning!!, only 1 IMG !! \n", __FUNCTION__);
		isLastFileIdxFlag=TRUE;
		return SUCCESS;
	}

	if (!(scanDir==PB_SHIFT_NEXT || scanDir==PB_SHIFT_PRE))
	{
		PB_VERBOSE("%s: !!error!!, illegal scanDir(%d) !!", __FUNCTION__, scanDir);
		return FAIL;
	}

	dcfIdx = gcurrDcfIdx;
	scanCount = gtotalImgCount;
	scanResult = FAIL;
	memset(&imgCb, 0, sizeof(pbImgCb_t));
	do
	{
		if (scanDir==PB_SHIFT_NEXT)
		{
			PB_INDEX_INC(dcfIdx, 1, gtotalImgCount);
		}
		else if (scanDir==PB_SHIFT_PRE)
		{
			PB_INDEX_DEC(dcfIdx, 1, gtotalImgCount);
		}
		#if !CAM_TYPE_CVR
		if(dcfIdx==gSlideShowBeginIndex)
		{
			isLastFileIdxFlag=TRUE;
		}
		#endif
		PB_VERBOSE("%s: Shifting to DCF index (%d of %d) \n", __FUNCTION__, dcfIdx, gtotalImgCount);

		/*-- Retrieve DCF attributes of current IMG
		 */
		sp5kDcfFsCurFileIdxSet(dcfIdx);
		imgCb.dcfIdx = dcfIdx;
		if (FAIL==playImgDcfAttrGet(&imgCb))
		{
			if (FALSE==isSkipFileErr)
			{
				PB_DBG("%s: !!ERROR!!, Getting IMG Attr failed !!\n", __FUNCTION__);
				scanResult = FAIL;
				break;
			}
			else
			{
				goto lbl_scan_next_jpg_cont;
			}
		}

		if (IS_FILETYPE_JPG(imgCb.dcfAttr.fileType))
		{
			/*-- JPG IMG */
			if (FAIL==playJpgCheckup(&imgCb))
			{
				if (FALSE==isSkipFileErr)
				{
					/*-- JPG IMG w/t ERROR */
					PB_DBG("%s: !!wanring!!, Stayed at ERROR IMG as requested !!\n", __FUNCTION__);
					scanResult = SUCCESS;
					break;
				}
			}
			else
			{
				/*-- GOOD JPG IMG */
				PB_VERBOSE("%s: JPG (%s) scanned, dcfIdx(%d) \n", __FUNCTION__, &imgCb.szDcfFileName[0], dcfIdx);
				scanResult = SUCCESS;
				break;
			}
		}
lbl_scan_next_jpg_cont:
		scanCount--;
	} while(scanCount>0);

	if (scanResult==FAIL)
	{
		return FAIL;
	}

	memcpy(&gpbImgCb, &imgCb, sizeof(pbImgCb_t));
	gcurrDcfIdx = dcfIdx;

	return SUCCESS;

}

/*-------------------------------------------------------------------------
 *  File Name : appPbBackdoorEventCBF
 *------------------------------------------------------------------------*/
UINT32
appPbBackdoorEventCBF(
	pbAppBackdoorEvent_t appEvent
)
{
	uiPara_t* puiPara = appUiParaGet();
		
	switch (appEvent)
	{
		default:
			return FAIL;
		case PB_APP_EVENT_DISK_FORMAT:
			memset(&gpbMainCb, 0, sizeof(pbMainCB_t));
			memset(&gpbDiskCb, 0, sizeof(pbDiskCB_t));
			/*-- Checkup Disk status */
			appPbDiskCheckup(&gpbDiskCb);
			/*-- check up IMG count */
			gtotalImgCount = gcurrDcfIdx = 0;
			gpbMainCb.isNoImage = TRUE;
			break;
		case PB_APP_EVENT_SETUP_RESET:
			puiPara->selSlideInteval = SLSHOW_1SEC;
			puiPara->selSlideMusic = SLSHOW_NONE;
			puiPara->selSlideRepeat = SLSHOW_REPEAT_OFF;
			puiPara->selSlideTrans = SLSHOW_TRANSITION_TYPE1;
			break;
	}
	return SUCCESS;
}
/*-------------------------------------------------------------------------
 *  File Name : PB_HistogramGet
 *------------------------------------------------------------------------*/

#define PB_HISTOGRAM_VALUE_MAX    256

UINT32 appPbHistogramGet( UINT32 *pbHistoData, UINT32 stepX, UINT32 stepY)
{
	UINT32 ret = SUCCESS;
	UINT32 x,y,idx,barWidth,barNum;
	UINT32 bufW,bufH,tmpHistData[PB_HISTOGRAM_VALUE_MAX];
	UINT32 countMax;
	UINT64 tmpSum;
	UINT32 YuvDataAddr;
	UINT8 *pbYuvData = NULL;

	barNum = stepX;
	barWidth = PB_HISTOGRAM_VALUE_MAX / barNum;
	if (barNum > PB_HISTOGRAM_VALUE_MAX)
	{
		barNum = PB_HISTOGRAM_VALUE_MAX;
	}


	sp5kStillPlayYUVInfoGet(SP5K_SPB_IMG_QV, &YuvDataAddr, &bufW, &bufH);/*Edward 20080821*/

	if ((YuvDataAddr == 0) || (bufW == 0) || (bufH == 0))
	{
		return FAIL;
	}

	/* 21ms -> 7ms */
	pbYuvData = (UINT8 *)(YuvDataAddr&~0x20000000); /* cache space */
	memset(tmpHistData, 0, sizeof(tmpHistData));
	//calculating
	x = bufH*bufW*2;
	for (idx=0 ; idx<x ; idx+=16)
	{
		tmpHistData[pbYuvData[idx]]++;
		tmpHistData[pbYuvData[idx+1]]++;
		tmpHistData[pbYuvData[idx+2]]++;
		tmpHistData[pbYuvData[idx+3]]++;
		tmpHistData[pbYuvData[idx+4]]++;
		tmpHistData[pbYuvData[idx+5]]++;
		tmpHistData[pbYuvData[idx+6]]++;
		tmpHistData[pbYuvData[idx+7]]++;
	}

	idx = countMax = 0;
	for (x=0;x<PB_HISTOGRAM_VALUE_MAX;x+=barWidth)
	{
		tmpSum = tmpHistData[x];
		for (y=1; y<barWidth; y ++)
		{
			tmpSum += tmpHistData[x+y];
		}
		pbHistoData[idx] = tmpSum / barWidth;
		if (pbHistoData[idx] > countMax)
		{
			countMax = pbHistoData[idx];
		}
		idx ++;
	}
	barNum = idx;
	for (x=0;x<barNum;x++)
	{
		pbHistoData[x] = (pbHistoData[x] * (stepY)) / countMax;
	}
	return ret;
}


