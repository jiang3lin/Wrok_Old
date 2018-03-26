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
#include "app_playback_private.h"
#include "app_playback_def.h"
#include "app_playback_osd.h"
#include "app_jpg_draw.h"
#include "sp5k_ftrack_api.h"
#include "sp5k_redeye_api.h"
#include "api/sp5k_moe_api.h"
#include "api/sp5k_cdsp_api.h"
#include "api/sp5k_capture_api.h"
#include "app_General.h"
#include "app_ui_para.h" 
#include "sp5k_utility_api.h"
#include "app_menu.h"
#include "app_menu_disp.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

#define PB_EDIT_ROTATE_SAVE_NEW 0
#define PB_EDIT_RESIZE_SAVE_NEW 1
#define FTRACK_RES_COORDX 1024
#define FTRACK_RES_COORDY 1024

#define FTRACK_SIZE_MAX 160*16

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#ifdef FACE_BEAUTY
void sample_MoeFMTChg(sp5kGfxObj_t *gfxObjSrc,sp5kGfxObj_t *gfxObjDst,UINT8 srcFMT,UINT8 dstFMT);
void sample_MoeImgFilter(UINT8 *psrc,UINT8 *pdst,UINT32 width,UINT32 height);
void Sample_SkinPerfectMoeCalucate(UINT8 *SrcUBuf,UINT8 *SrcVBuf,UINT32 width,UINT32 height);
void sample_gfxObjDemo(sp5kGfxObj_t *gfxObjSrc,sp5kGfxObj_t *gfxObjDst);
UINT32 appPbEffectBeautySkinProc(UINT32 ulAddr,UINT32 w,UINT32 h);
#endif

extern void cpuDCacheFlush(void *addrstart, UINT32 size);

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

#define JPG_PHOTOFRAM1		"A:\\RO_RES\\UI\\JPG\\FRAME001.JPG"
#define JPG_PHOTOFRAM2		"A:\\RO_RES\\UI\\JPG\\FRAME002.JPG"
#define JPG_PHOTOFRAM3		"A:\\RO_RES\\UI\\JPG\\FRAME003.JPG"
#define JPG_PHOTOFRAM4		"A:\\RO_RES\\UI\\JPG\\FRAME004.JPG"
#define JPG_PHOTOFRAM5		"A:\\RO_RES\\UI\\JPG\\FRAME005.JPG"

#define ID_ICON_PHOTOFRAM1	"A:\\RO_RES\\UI\\JPG\\FRAMEDP1.JPG"
#define ID_ICON_PHOTOFRAM2	"A:\\RO_RES\\UI\\JPG\\FRAMEDP2.JPG"
#define ID_ICON_PHOTOFRAM3	"A:\\RO_RES\\UI\\JPG\\FRAMEDP3.JPG"
#define ID_ICON_PHOTOFRAM4	"A:\\RO_RES\\UI\\JPG\\FRAMEDP4.JPG"
#define ID_ICON_PHOTOFRAM5	"A:\\RO_RES\\UI\\JPG\\FRAMEDP5.JPG"

static UINT8 *gpbFrameFileTable[PB_UI_FRAME_TOTAL] = 
{
	JPG_PHOTOFRAM1,
	JPG_PHOTOFRAM2,
	JPG_PHOTOFRAM3,
	JPG_PHOTOFRAM4,
	JPG_PHOTOFRAM5,
};

static UINT8 *gpbFrameIconFileTable[PB_UI_FRAME_TOTAL] = 
{
	ID_ICON_PHOTOFRAM1,
	ID_ICON_PHOTOFRAM2,
	ID_ICON_PHOTOFRAM3,
	ID_ICON_PHOTOFRAM4,
	ID_ICON_PHOTOFRAM5,
};

static UINT8 gpbCurrAppliedPhotoframe = PB_UI_FRAME_NONE;
static UINT32 gpbRedEyesFaceNum;
static redeyeFaceInfo_t gpbRedEyesFaceInfo[12];
static UINT16 gpbRedEyesFaceQvWidth, gpbRedEyesFaceQvHeight;


/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern void cpuCacheFlushAll(unsigned int op);
extern UINT32 sp5kCapCfgQvResolutionSet(sp5kImgSize_t *psize);

/**************************************************************************
 *                    S T A T I C    F U N C T I O N                      *
 **************************************************************************/

/*-------------------------------------------------------------------------
 *  File Name : playOrient2Sp5kRotFlip
 *------------------------------------------------------------------------*/
UINT32
editEffectUI2SP5K(
	pbUiEffectSel_t uiEff
)
{
	switch(uiEff) {
	default:
		PB_VERBOSE("%s: UNKNOWN !! \n", __FUNCTION__);
		return SP5K_EFFECT_EMBOSS; /* !! baseFW does not define UNKNOWN type !! */
	case PB_UI_EFFECT_BLACK_WHITE:
		return SP5K_EFFECT_BW;
	case PB_UI_EFFECT_NEGA:
		return SP5K_EFFECT_NEGATIVE;
	case PB_UI_EFFECT_SEPIA:
		return SP5K_EFFECT_SEPIA;
	case PB_UI_EFFECT_MOSAIC:
		return SP5K_EFFECT_MOSAIC;
	case PB_UI_EFFECT_RED:
		return SP5K_EFFECT_RED;
	case PB_UI_EFFECT_GREEN:
		return SP5K_EFFECT_GREEN;
	case PB_UI_EFFECT_BLUE:
		return SP5K_EFFECT_BLUE;
	}
}

/*-------------------------------------------------------------------------
 *  File Name : playOrient2Sp5kRotFlip
 *------------------------------------------------------------------------*/
UINT32
editRotateOrientUI2SP5K(
	pbRotCmdDef_t rotCmd
)
{
	switch(rotCmd) 
	{
	default:
		return SP5K_ROTATE_SET_NUM;
	case PB_ROT_ORIENT_0:
		return SP5K_ROTATE_0;
	case PB_ROT_ORIENT_90:
		return SP5K_ROTATE_90;
	case PB_ROT_ORIENT_180:
		return SP5K_ROTATE_180;
	case PB_ROT_ORIENT_270:
		return SP5K_ROTATE_270;
	}
}


/*-------------------------------------------------------------------------
 *  File Name : editResizeTargetSizeGet
 *------------------------------------------------------------------------*/
UINT32
editResizeTargetSizeGet(
	pbImgCb_t *pimgCb,
	pbUiResizeSel_t resizeSel,
	UINT32 *pwidth,
	UINT32 *pheight
)
{
	UINT8 imgDim;
	imgDim = pimgCb->imgOrientDim;
	if (imgDim & PB_IMG_ORIENT_4V3) 
	{
		if (imgDim==PB_IMG_ORIENT_4X3) 
		{
			*pwidth = gpbSizeGrade[resizeSel][0].w;
			*pheight = gpbSizeGrade[resizeSel][0].h;
		} 
		else 
		{
			*pwidth = gpbSizeGrade[resizeSel][0].h;
			*pheight = gpbSizeGrade[resizeSel][0].w;
		}
	} 
	else if (imgDim & PB_IMG_ORIENT_16V9) 
	{
		if (imgDim==PB_IMG_ORIENT_16X9) 
		{
			*pwidth = gpbSizeGrade[resizeSel][2].w;
			*pheight = gpbSizeGrade[resizeSel][2].h;
		} 
		else 
		{
			*pwidth = gpbSizeGrade[resizeSel][2].h;
			*pheight = gpbSizeGrade[resizeSel][2].w;
		}
	} 
	else if (imgDim & PB_IMG_ORIENT_3V2)
	{
		if (imgDim==PB_IMG_ORIENT_3X2)
		{
			*pwidth = gpbSizeGrade[resizeSel][1].w;
			*pheight = gpbSizeGrade[resizeSel][1].h;
		} 
		else
		{
			*pwidth = gpbSizeGrade[resizeSel][1].h;
			*pheight = gpbSizeGrade[resizeSel][1].w;
		}
	} 
	else
	{
		*pwidth = 0;
		*pheight = 0;
		return FAIL;
	}
	PB_VERBOSE("RESIZE : Target W(%d), H(%d) \n", *pwidth, *pheight);
	return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  File Name : editValidityCheckup
 *------------------------------------------------------------------------*/
UINT32
editValidityCheckup(
	pbImgCb_t *pimgCb
)
{
	sp5kDcfAttrElm_t *pdcfAttr;

	PB_VERBOSE("%s: checking %s \n", __FUNCTION__, &pimgCb->szDcfFileName[0]);

	pdcfAttr = &pimgCb->dcfAttr;

	gpbMainCb.err = PB_ERR_NONE;
	if (FALSE==IS_FILETYPE_JPG(pdcfAttr->fileType)) 
	{
		PB_VERBOSE("%s: !!ERROR!!, Not JPG image !!\n", __FUNCTION__);
		gpbMainCb.err = PB_ERR_FILETYPE_ERROR;
		return FAIL;
	}

	if (pimgCb->isFileErr) 
	{
		gpbMainCb.err = PB_ERR_FILETYPE_ERROR;
		return FAIL;
	}

	return SUCCESS;
}



/**************************************************************************
 *                    G L O B A L    F U N C T I O N                      *
 **************************************************************************/
/*-------------------------------------------------------------------------
 *  File Name : appPbPhotoframeCreate
 *------------------------------------------------------------------------*/
UINT32
appPbPhotoframeCreate(
	pbImgCb_t *pimgCb
)
{
	PB_DBG("%s: IMG (%s) \n", __FUNCTION__, pimgCb->szDcfFileName);

	/*-- checkup disk current status
	 */
	if (FAIL==editValidityCheckup(pimgCb)) 
	{
		goto lbl_pfCreateFail; //return FAIL;
	}

	/*-- checkup image properties
	 */
	pimgCb->imgOrientDim = appPbImageOrientDimGet((UINT16)pimgCb->imgWidth, (UINT16)pimgCb->imgHeight);
	if (!(pimgCb->imgOrientDim & PB_IMG_ORIENT_4X3))
	{
		PB_VERBOSE("%s: NOT 4X3 IMG \n", __FUNCTION__);
		gpbMainCb.err = PB_ERR_FILETYPE_ERROR;
		goto lbl_pfCreateFail; //return FAIL;
	}

	gpbCurrAppliedPhotoframe = PB_UI_FRAME_NONE;
	gpbMainCb.mainStat = PB_MAIN_STAT_JPG_PHOTOFRAME;

	return SUCCESS;
lbl_pfCreateFail:
	PB_OSD_CLR;
	printf("Create PhotoFrame Error\n");
	appPbErrorProcess(PB_ERR_FILETYPE_ERROR);
	return FAIL;
}

/*-------------------------------------------------------------------------
 *  File Name : appPbPhotoframeClear
 *------------------------------------------------------------------------*/
void
appPbPhotoframeClear(
	void
)
{
	/* %REVIEW% : Check OSD */
	sp5kGfxAttrSet(SP5K_GFX_PEN_COLOR, 0x000000, 0, 0, 0);
	sp5kGfxAttrSet(SP5K_GFX_FILL_COLOR, 0x000000, 0, 0, 0);
	sp5kGfxRectDraw(SP5K_GFX_PAGE_PF_0, 0, 0, 400, 300);

	gpbCurrAppliedPhotoframe = PB_UI_FRAME_NONE;
}

/*-------------------------------------------------------------------------
 *  File Name : appPbPhotoframePreview
 *------------------------------------------------------------------------*/
UINT32
appPbPhotoframePreview(
	pbUiPhotoframeSel_t frameId
)
{
	UINT32 status;
	switch (frameId) 
	{
		default:
			gpbMainCb.err = PB_ERR_EFFECT_ERROR;
			return FAIL;
		case PB_UI_FRAME_NONE:
			appPbPhotoframeClear();
			return SUCCESS;
		case PB_UI_FRAME_1:
		case PB_UI_FRAME_2:
		case PB_UI_FRAME_3:
		case PB_UI_FRAME_4:
		case PB_UI_FRAME_5:
			status = appJpgDraw_PfLayer(gpbFrameIconFileTable[frameId], 0, 0, 320, 240);
			if (status==SUCCESS) 
			{
				gpbCurrAppliedPhotoframe = frameId;
			}
			break;
	}

	return status;
}

/*-------------------------------------------------------------------------
 *  File Name : appPbPhotoframeExecute
 *------------------------------------------------------------------------*/
UINT32
appPbPhotoframeExecute(
	pbImgCb_t *pimgCb,
	UINT8	frameId
)
{
	UINT32 status, tmElapse;

	PB_VERBOSE("%s: %s \n", __FUNCTION__, &pimgCb->szDcfFileName[0]);

	PB_OSD_CLR;

	/*-- Photoframe combine & Save Image */
	if (FAIL==appPbEditSaveDiskCheckup(pimgCb)){
		gpbMainCb.err = PB_ERR_DISK_FULL;
		return FAIL;
	}

	appPbMsg_BusyWait();

	if (FAIL==sp5kImageEdit(SP5K_IMGEDIT_PHOTOFRM_FILE, (UINT32)gpbFrameFileTable[frameId], 0, 0, 0)) {
		PB_VERBOSE("%s: Photoframe IMGEDIT failed \n", __FUNCTION__);
		gpbMainCb.err = PB_ERR_PHOTOFRAME_ERROR;
		return FAIL;
	}

	tmElapse = appMsTimeGet();
	appPbMsg_ImgSaved();

	status = appPbImageDcfSave(PB_SAVE_NEW);

	if (status==FAIL) {
		PB_VERBOSE("%s: Failed \n", __FUNCTION__);
		gpbMainCb.err = PB_ERR_DISK_ERROR;
	} else {
		gtotalImgCount = appPbTotalFileCountGet();
		gcurrDcfIdx = gtotalImgCount;
		gpbMainCb.err = PB_ERR_NONE;
	}

	tmElapse = appMsTimeGet() - tmElapse;

	if(2000 > tmElapse) {
		APP_SLEEP_MS(2000-tmElapse);
	}
	PB_OSD_CLR;

	return status;
}

/*-------------------------------------------------------------------------
 *  File Name : appPbEffectDestroy
 *------------------------------------------------------------------------*/
void
appPbPhotoframeDestroy(
	pbImgCb_t *pimgCb
)
{
	PB_VERBOSE("%s: IMG (%s) \n", __FUNCTION__, pimgCb->szDcfFileName);

	if (gpbCurrAppliedPhotoframe != PB_UI_FRAME_NONE) {
		PB_VERBOSE("Cleaning PHOTOFRAME(%d) Preview !! \n", gpbCurrAppliedPhotoframe);
		appPbPhotoframeClear();
	}

	gpbMainCb.mainStat = PB_MAIN_STAT_JPG_PHOTOFRAME;
	gpbCurrAppliedPhotoframe = PB_UI_FRAME_NONE;

	return;
}

/*-------------------------------------------------------------------------
 *  File Name : appPbEffectExecute
 *------------------------------------------------------------------------*/
UINT32 
appPbEffectRedEyeProc(
	UINT32 srcAddr, UINT32 width, UINT32 height
)
{

/*s Modify by Aries 09/12/13*/
#if SPCA5310
	UINT32 ret=FAIL;
	UINT8 i;
	if (width<320 && height<320) {
		printf("redeyeCb reject(%d/%d)\n", width, height);
		return 0;
	}

	if (gpbRedEyesFaceNum > 0 ) {
		for (i = 0; i < gpbRedEyesFaceNum; i++) {
			if((gpbRedEyesFaceInfo[i].faceSize)*width/FTRACK_RES_COORDX > FTRACK_SIZE_MAX)
				gpbRedEyesFaceInfo[i].faceSize = FTRACK_SIZE_MAX*FTRACK_RES_COORDX/width;
		}
	}
	ret = sp5kRedEyeReductionDo(srcAddr, width, height, FTRACK_RES_COORDX, FTRACK_RES_COORDY, gpbRedEyesFaceNum, gpbRedEyesFaceInfo);

	return SUCCESS;	/*return SUCCESS no matter what the result is.*/

#else
	static UINT32 jobCnt = 0;
	UINT32 ret=FAIL;
	/* skip QV display buffer */
	if (jobCnt == 0) {
		jobCnt ++;
		return 0;
	}
	else {
		jobCnt = 0;
	}

	ret = sp5kRedEyeReductionDo(srcAddr, width, height, gpbRedEyesFaceQvWidth,
	 	gpbRedEyesFaceQvHeight, gpbRedEyesFaceNum, gpbRedEyesFaceInfo);
	
	return ret;
#endif
/*s Modify by Aries 09/12/13*/
}

/*-------------------------------------------------------------------------
 *  File Name : appPbEffectExecute  Beauty Skin
 *------------------------------------------------------------------------*/

#define FACE_BEAUTY 1
//#define test_pattern 1
#ifdef FACE_BEAUTY
static UINT8 MoeYLLut[256] = 
{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 5, 4, 
	4, 4, 3, 3, 3, 3, 3, 2, 2, 2, 2, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 2, 2, 2, 3, 3, 3, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
};
	 
	 
static UINT8 MoeYHLut[256] = 
{	
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 8, 8, 9, 
	 9, 10, 12, 12, 13, 13, 13, 13, 14, 14, 14, 14, 14, 15, 16, 16, 
	16, 16, 16, 17, 17, 17, 18, 18, 19, 19, 19, 19, 19, 20, 20, 20, 
	20, 21, 21, 21, 22, 22, 22, 22, 23, 24, 24, 24, 25, 25, 25, 25, 
	25, 25, 24, 24, 24, 24, 23, 23, 23, 22, 22, 22, 22, 22, 21, 21, 
	21, 21, 21, 21, 20, 20, 20, 19, 19, 19, 18, 18, 18, 18, 18, 18, 
	18, 18, 18, 18, 18, 18, 18, 18, 19, 19, 19, 19, 18, 18, 18, 18, 
	18, 17, 17, 17, 17, 16, 16, 16, 16, 16, 16, 15, 15, 15, 15, 14, 
	13, 12, 11, 11, 9, 9, 8, 7, 7, 6, 5, 4, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
};
#if 0
static UINT8 BlurImgFilter[49]=
{
	 2, 2, 2, 3, 2, 2, 2,
	 2, 3, 3, 3, 3, 3, 2,
	 2, 3, 3, 3, 3, 3, 2,
	 3, 3, 3, 4, 3, 3, 3,
	 2, 3, 3, 3, 3, 3, 2,
	 2, 3, 3, 3, 3, 3, 2,
	 2, 2, 2, 3, 2, 2, 2,
};
#endif
#endif

#ifdef FACE_BEAUTY
void sample_HueCorrect(sp5kGfxObj_t *gfxObjSrc, sp5kGfxObj_t *gfxObjDst)
{
	sp5kIqModeSet(SP5K_MODE_STILL_PLAYBACK);
	sp5kIqBlockEnable(SP5K_IQ_BLOCK_HUE, 1);
	sp5kResourceFileLoad(SP5K_RES_HUE_CORRECT, "IQ\\HUECORR.BIN");
	sp5kIqModeSetDone(SP5K_MODE_STILL_PLAYBACK);
	sp5kGfxObjectEdit(gfxObjSrc, gfxObjDst, SP5K_GFX_OBJECT_EDIT_CDSP, 0);
	sp5kIqModeSet(SP5K_MODE_STILL_PLAYBACK);
	sp5kIqBlockEnable(SP5K_IQ_BLOCK_HUE, 0);
	sp5kIqModeSetDone(SP5K_MODE_STILL_PLAYBACK);
}

/*
	YUV convert to	UNPACKAGE UV BLOCK 
		src : SP5K_MOE_DATA_FMT_PACK_UV , dst : SP5K_MOE_DATA_FMT_UNPACK_UV
	YUV convert to	UNPACKAGE Y BLOCK 
		src : SP5K_MOE_DATA_FMT_YUV_PLANE , dst : SP5K_MOE_DATA_FMT_8BIT	
	UNPACKAGE Y BLOCK convert to PACKAGE Y
		src : SP5K_MOE_DATA_FMT_8BIT , dst : SP5K_MOE_DATA_FMT_PACK_Y
*/
	 
void sample_MoeFMTChg(sp5kGfxObj_t *gfxObjSrc, sp5kGfxObj_t *gfxObjDst, UINT8 srcFMT, UINT8 dstFMT)
{
	UINT32 handle; 
	UINT32 ret; 
	sp5kMoeObj_t psrc; 
	sp5kMoeObj_t pdst;
	sp5kMoeLutCfg_t plutCfg;
	sp5kMoeRoiCfg_t proiCfg;
	 
	memset(&psrc, 0, sizeof(sp5kMoeObj_t)); 	
	memset(&pdst, 0, sizeof(sp5kMoeObj_t)); 	
	memset(&plutCfg, 0, sizeof(sp5kMoeLutCfg_t));	
	memset(&proiCfg, 0, sizeof(sp5kMoeRoiCfg_t));	
	 
	plutCfg.lutBit = 0;
	plutCfg.lutSelect = 0;
	proiCfg.dstRoiH = proiCfg.srcRoiH = gfxObjSrc->bufW;
	proiCfg.dstRoiV = proiCfg.srcRoiV = gfxObjSrc->bufH;
	if(srcFMT == SP5K_MOE_DATA_FMT_PACK_UV)
		psrc.pbuf = gfxObjSrc->pbuf+0x100;
	else 
		psrc.pbuf = gfxObjSrc->pbuf;
	 
	psrc.fmt = srcFMT ;
	pdst.fmt = dstFMT;//SP5K_MOE_DATA_FMT_PACK_Y;
	 
	pdst.pbuf = gfxObjDst->pbuf;
	pdst.height = psrc.height = gfxObjSrc->bufH;
	pdst.width = psrc.width= gfxObjSrc->bufW;
	 
	handle = sp5kMoeHandleInit();
	if (!handle) {
		printf("moe handle init failed\n");
		return;
	}
	ret = sp5kMoeYuvScaleSet(handle, &psrc, &pdst, &plutCfg, &proiCfg);
	if (ret != SUCCESS) {
		printf("operation failed\n");
	}
	ret = sp5kMoeHandleRun(handle);
	if (ret != SUCCESS) {
		printf("run failed\n");
		goto quit_err_moe;
	}
	else {
	}		
quit_err_moe:
	if (handle) {
		/* release the handle */
		sp5kMoeHandleRelease(handle);
	}	
}

void Sample_SkinPerfectMoeCalucate(UINT8 *SrcUBuf, UINT8 *SrcVBuf, UINT32 width, UINT32 height)
{
	UINT8 *srcU;
	UINT8 *srcV;
	UINT8 *dst;
	UINT32 moeHandle=0, ret;
	UINT32 i;
	sp5kMoeObj_t src1, src2;
	UINT8 *tmpbuf1, *tmpbuf2, *dstbuf;
	sp5kMoeClipCfg_t pclip;
	sp5kMoeObj_t dst1;
	 
	void *psrc;
	void *psrc2;

	dst = dst;
	i = i;
	tmpbuf1 = tmpbuf1;

	psrc = sp5kYuvBufferAlloc(width, height);
	psrc2 = sp5kYuvBufferAlloc(width, height);
	 
	tmpbuf2 = NULL;
	srcU = SrcUBuf;
	srcV = SrcVBuf;
	dstbuf = NULL;
	 
	moeHandle = sp5kMoeHandleInit();
	if (!moeHandle) {
		printf("moe handle init failed\n");
		return;
	}
	memset(psrc, 0x80, ((width)*(height)));
	memset(psrc2, 0x80, ((width)*(height)));
/**************************************************************************/
/*abs(src - 128) clip 0x7F*/
	pclip.min = 0;
	pclip.max = 0x7f;	
	/* set src1 object */
	src1.pbuf = srcU;
	src1.width = width>>1;
	src1.height = height;
	src1.x = 0;
	src1.y = 0;
	src1.fmt = SP5K_MOE_DATA_FMT_8BIT;
	src1.sign = SP5K_MOE_DATA_TYPE_UNSIGNED;
	src1.shift = 0;
	src1.abs = 0;
	src2.pbuf = psrc;
	src2.width = width>>1;
	src2.height = height;
	src2.x = 0;
	src2.y = 0;
	src2.fmt = SP5K_MOE_DATA_FMT_8BIT;
	src2.sign = SP5K_MOE_DATA_TYPE_UNSIGNED;
	src2.shift = 0;
	src2.abs = 0;
	 
	dst1.pbuf = srcU;
	dst1.width = width>>1;
	dst1.height = height;
	dst1.x = 0;
	dst1.y = 0;
	dst1.fmt = SP5K_MOE_DATA_FMT_8BIT;
	dst1.sign = SP5K_MOE_DATA_TYPE_UNSIGNED;
	dst1.shift = 0;
	dst1.abs = 1;	
 
	ret = sp5kMoeMatrixPointByPointSet(moeHandle, &src1, &src2, &dst1, 
		&pclip, (width>>1), (height), SP5K_MOE_MAT_CAL_SUB);
	if (ret != SUCCESS) {
		printf("operation failed\n");
		goto quit_err_moe;
	}	
	dst1.pbuf = srcV;
	src1.pbuf = srcV;
	ret = sp5kMoeMatrixPointByPointSet(moeHandle, &src1, &src2, &dst1, 
		&pclip, (width>>1), (height), SP5K_MOE_MAT_CAL_SUB);
	if (ret != SUCCESS) {
		printf("operation failed\n");
		goto quit_err_moe;
	}	
	src1.abs = 0;
	src2.abs = 0;
	dst1.abs = 0;
/**************************************************************************/
/*(src * src ) */
	src1.pbuf = srcU;
	dst1.pbuf = psrc;
	dst1.fmt  = SP5K_MOE_DATA_FMT_16BIT;
	dst1.shift = 0;
	ret = sp5kMoeMatrixPointByPointSet(moeHandle, &src1, &src1, &dst1, 
		NULL, (width>>1), (height), SP5K_MOE_MAT_CAL_MUL);
	if (ret != SUCCESS) {
		printf("operation failed\n");
	}
	src1.pbuf = srcV;
	dst1.pbuf = psrc2;
	dst1.shift = 0;
	ret = sp5kMoeMatrixPointByPointSet(moeHandle, &src1, &src1, &dst1, 
		NULL, (width>>1), (height), SP5K_MOE_MAT_CAL_MUL);
	if (ret != SUCCESS) {
		printf("operation failed\n");
	}
/**************************************************************************/
/*(src + src)>> 7*/
	src1.pbuf = psrc;
	src2.pbuf = psrc2;
	src1.fmt  = SP5K_MOE_DATA_FMT_16BIT;
	src2.fmt  = SP5K_MOE_DATA_FMT_16BIT;
	dst1.pbuf = srcU;
	dst1.fmt  = SP5K_MOE_DATA_FMT_8BIT;
	dst1.shift = 7; 
	/* set matrix point by point operation */
	ret = sp5kMoeMatrixPointByPointSet(moeHandle, &src1, &src2, &dst1, 
		NULL, (width>>1), (height), SP5K_MOE_MAT_CAL_ADD);
	if (ret != SUCCESS) {
		printf("operation failed\n");
	}
	/* run the operations in the handle */
	ret = sp5kMoeHandleRun(moeHandle);
	if (ret != SUCCESS) {
		printf("run failed\n");
		goto quit_err_moe;
	}
	else {
		/* the data in dstBuf1 is the result of point by point operation */
		/* the data in dstBuf2 is the result of point by common operation */
	}	
/**************************************************************************/
quit_err_moe:
	if (moeHandle) {
		/* release the handle */
	sp5kMoeHandleRelease(moeHandle);
	}
	 
	 sp5kYuvBufferFree(psrc);	
	 sp5kYuvBufferFree(psrc2);	
/**************************************************************************/
}
	


	 
void sample_MoeImgFilter(UINT8 *psrc, UINT8 *pdst, UINT32 width, UINT32 height)
{
	UINT32 handle; 
	UINT32 ret; 
	UINT32 i,j; 
	UINT32 MAX_ROIW = 154; 
	UINT32 ROIW_RANGE = 128; 
	UINT32 roiW ; 
	UINT8 *pbuf;
	UINT32 Moe_W; 
	sp5kMoeFilterObj_t flr;
	sp5kMoeObj_t moeSrc; 
	sp5kMoeObj_t moeDst;
	sp5kMoeMirrorCfg_t mirror;

	i = i;
	j = j;
	roiW = roiW;
	pbuf = NULL;
	memset(&moeSrc, 0, sizeof(sp5kMoeObj_t));	
	memset(&moeDst, 0, sizeof(sp5kMoeObj_t)); 
	 
	handle = sp5kMoeHandleInit();
	if (!handle) {
		printf("moe handle init failed\n");
		return;
	}
	flr.para0 = 4;
	flr.para1 = 3;
	flr.para2 = 3;
	flr.para3 = 3;
	flr.para4 = 3;
	flr.para5 = 3;
	flr.para6 = 3;
	flr.para7 = 2;
	flr.para8 = 2;
	flr.para9 = 2;
	 
	mirror.topMrrPixel	 = 3;
	mirror.topMrrEn 	 = 1;
	mirror.botMrrPixel	 = 3;
	mirror.botMrrEn 	 = 1;
	mirror.leftMrrPixel  = 3;
	mirror.leftMrrEn	 = 1;
	mirror.rightMrrPixel = 3;
	mirror.rightMrrEn	 = 1;
	mirror.reserved 	 = 0;
	 
	moeSrc.fmt = SP5K_MOE_DATA_FMT_8BIT;
	moeSrc.width = width;
	moeSrc.height = height;
	moeSrc.pbuf = psrc;
	moeSrc.abs = 0;
	moeSrc.shift = 0;
	moeSrc.sign = 0;
	moeSrc.x = moeSrc.y = 0;
	 
	 
	moeDst.fmt = SP5K_MOE_DATA_FMT_8BIT;
	moeDst.width = width;
	moeDst.height = height;
	moeDst.pbuf = pdst; 
	moeDst.shift = 7;// Filter sum / 128
	moeDst.x = moeDst.y = 0;
	moeDst.abs = 0;
	moeDst.sign = 0;
	moeDst.x = 0;
	moeSrc.x = 0;
	if(width <= MAX_ROIW) {
		Moe_W = width;
		ret = sp5kMoeImgFilter7x7Set(handle,&moeSrc,&flr,&moeDst,NULL,&mirror,Moe_W,height);
		if (ret != SUCCESS) {
			printf("operation failed\n");
		}
	} else {
		do {
			mirror.leftMrrEn = mirror.rightMrrEn = 0;
			if(moeSrc.x == 0) {
				mirror.leftMrrEn = 1;
			}
			if(moeSrc.x + ROIW_RANGE >= width ) {
				mirror.rightMrrEn = 1;
				Moe_W = (width-moeSrc.x);
			} else {
				Moe_W = ROIW_RANGE;
			}
			//printf("moe filter srcX : %d ,dstX : %d, ROI W : %d \t\n",moeSrc.x,moeDst.x,Moe_W);
			ret = sp5kMoeImgFilter7x7Set(handle,&moeSrc,&flr,&moeDst,NULL,&mirror,Moe_W,height);
			if (ret != SUCCESS) {
				printf("operation failed\n");
			}
			moeDst.x = moeSrc.x + ROIW_RANGE - 3;
			moeSrc.x = moeDst.x - 3;	
		} while (mirror.rightMrrEn==0);
	}
	ret = sp5kMoeHandleRun(handle);
	if (ret != SUCCESS) {
		printf("run failed\n");
		goto quit_err_moe;
	}
	else {
	}
quit_err_moe:
	if (handle) {
		/* release the handle */
		sp5kMoeHandleRelease(handle);
	}
}

UINT32 
appPbEffectBeautySkinProc(
	UINT32 uladdr, UINT32 w, UINT32 h
)
{
	int fp;
	char szFile[128] = {0};
	UINT32 width = w;
	UINT32 height= h;
	UINT8 *psrc;
	UINT8 *psrc2;
	UINT8 *tmp;
	UINT8 *paplane;
	UINT32 i, j, k;
	sp5kGfxObj_t gfxObjSrc,psrcObj;
	sp5kGfxObj_t gfxObjOp,pdstObj;
	//sp5kModeSet(SP5K_MODE_STANDBY);
	//sp5kModeWait(SP5K_MODE_STANDBY);	
	fp = fp;
	j = j;
	k = k; 
	psrc = sp5kYuvBufferAlloc(width, height);
	psrc2 = sp5kYuvBufferAlloc(width, height);
	paplane = sp5kYuvBufferAlloc(width, (height>>1));
/***********************************************************************************/	
/***Get Mask Alpha plane************************************************************/	
/***********************************************************************************/	
//load a JPG	------> get ImgBuf
#ifdef test_pattern	
	sprintf(szFile, "D:\\img.yuv");
	fp = sp5kFsFileOpen(szFile, FS_OPEN_RDWR);
	if (fp != NULL) 
	{
		sp5kFsFileRead(fp, psrc, width * height* 2);
		sp5kFsFileClose(fp);
	}else
	{
		printf("load img file fail\n");
		return;
	}
#else
	psrcObj.pbuf = (UINT8*)uladdr;
	psrcObj.bufW = width;
	psrcObj.bufH = height;
	psrcObj.paplaneBuf = NULL;
	psrcObj.fmt = 0;
	psrcObj.roiX = 0;
	psrcObj.roiY = 0;
	psrcObj.roiW = width;
	psrcObj.roiH = height;

	pdstObj.pbuf = psrc;
	pdstObj.bufW = width;
	pdstObj.bufH = height;
	pdstObj.paplaneBuf = NULL;
	pdstObj.fmt = 0;
	pdstObj.roiX = 0;
	pdstObj.roiY = 0;
	pdstObj.roiW = width;
	pdstObj.roiH = height;
	sp5kGfxObjectCopy(&psrcObj,&pdstObj);
#endif
	gfxObjSrc.pbuf = psrc;
	gfxObjOp.pbuf = psrc2;
	gfxObjSrc.roiX = gfxObjSrc.roiY = gfxObjOp.roiX = gfxObjOp.roiY = 0;
	gfxObjSrc.bufH = gfxObjOp.bufH = gfxObjSrc.roiH = gfxObjOp.roiH = height;
	gfxObjSrc.bufW= gfxObjOp.bufW  = gfxObjSrc.roiW = gfxObjOp.roiW = width;
	gfxObjSrc.fmt = gfxObjOp.fmt = SP5K_GFX_FMT_YUV422;
	gfxObjSrc.paplaneBuf = gfxObjOp.paplaneBuf = NULL;
	//Hue correct	------> get ImgBuf
	sample_HueCorrect(&gfxObjSrc, &gfxObjOp);
	//separate YUV	------> get hueImg UV buf unpackage
	sample_MoeFMTChg(&gfxObjOp, &gfxObjSrc, SP5K_MOE_DATA_FMT_PACK_UV, SP5K_MOE_DATA_FMT_UNPACK_UV);
	gfxObjSrc.pbuf = (psrc+(width*height));
	//separate YUV	------> get hueImg Y buf unpackage
	sample_MoeFMTChg(&gfxObjOp, &gfxObjSrc, SP5K_MOE_DATA_FMT_YUV_PLANE, SP5K_MOE_DATA_FMT_8BIT);
	//MOE caculate	------> get normalize U^2+V^2
	tmp = (psrc+(height*(width>>1)));
	Sample_SkinPerfectMoeCalucate(psrc, tmp, width, height);
	//scale up
	gfxObjSrc.pbuf = psrc;
	gfxObjOp.pbuf = psrc2;
	gfxObjSrc.fmt = gfxObjOp.fmt = SP5K_GFX_FMT_IDX8;
	gfxObjSrc.roiX = gfxObjSrc.roiY = gfxObjOp.roiX = gfxObjOp.roiY = 0;
	gfxObjSrc.bufW = gfxObjSrc.roiW = width>>1;
	gfxObjSrc.bufH = gfxObjSrc.roiH = height;
	gfxObjOp.bufW  = gfxObjOp.roiW	= width>>1;
	gfxObjOp.bufH = gfxObjOp.roiH = height>>1;
	sp5kGfxObjectScale(&gfxObjSrc, &gfxObjOp);
	 
	gfxObjSrc.pbuf = psrc2;
	gfxObjOp.pbuf = psrc;	
	gfxObjSrc.bufW = gfxObjSrc.roiW = width>>1;
	gfxObjSrc.bufH = gfxObjSrc.roiH = height>>1;
	gfxObjOp.bufW  = gfxObjOp.roiW	= width;
	gfxObjOp.bufH = gfxObjOp.roiH = height;
	sp5kGfxObjectScale(&gfxObjSrc, &gfxObjOp);
	 
	//get Mask alpha plane tb
#if 0
		//Method 1 : slow 
		tmp = psrc+width*height;
		for(i = 0 ; i < width*height ; i++) {
			if( (*(psrc+i)<MoeYLLut[*(tmp+i)])||(*(psrc+i)>=MoeYHLut[*(tmp+i)])) {
				*(psrc+i) = 0;//mask
			} else {
				*(psrc+i) = 0x80;//mask
			}
		}
		profLogAdd(1, "SkinPerfect CPU mode Y threshold End"); 
	 
		sample_MoeImgFilter(psrc, paplane, width, height);//alpha 2 tb
#else
 #define LOGI_UNCACH_BADDR_TO_LOGI_CACH_BADDR(addr) 	((void *)((UINT32)(addr) & ~0x20000000))
	//Method 2 : faster
	UINT8 *pCache = LOGI_UNCACH_BADDR_TO_LOGI_CACH_BADDR(psrc);
	const UINT32 l=width*height;
	for(i = 0 ; i < l ; i+=4) {
		UINT32 ww=0;
		UINT32 y=*((UINT32*)(pCache+l+i));
		if (pCache[i]>=MoeYLLut[y&0xff] && pCache[i]<MoeYHLut[y&0xff]) {
			ww = 0x80;
		}
		y >>= 8;
		if (pCache[i+1]>=MoeYLLut[y&0xff] && pCache[i+1]<MoeYHLut[y&0xff]) {
			ww |= 0x8000;
		}
		y >>= 8;
		if (pCache[i+2]>=MoeYLLut[y&0xff] && pCache[i+2]<MoeYHLut[y&0xff]) {
			ww |= 0x800000;
		}
		y >>= 8;
		if (pCache[i+3]>=MoeYLLut[y] && pCache[i+3]<MoeYHLut[y]) {
			ww |= 0x80000000;
		}
		*((UINT32*)(pCache+i)) = ww;
	}
	cpuDCacheFlush(pCache, l*2);
	profLogAdd(1, "SkinPerfect CPU mode Y threshold End"); 
	 
	sample_MoeImgFilter(pCache, paplane, width, height);//alpha 2 tb
#endif
	 
/***********************************************************************************/	
/***Get Mask Img *******************************************************************/	
/***********************************************************************************/	
#ifdef test_pattern	

    sprintf(szFile, "D:\\img.yuv");
	fp = sp5kFsFileOpen(szFile, FS_OPEN_RDWR);
	if (fp != NULL) 
	{
		sp5kFsFileRead(fp, psrc, width * height* 2);
		sp5kFsFileClose(fp);
	}else
	{
		printf("load img file fail\n");
		return;
	}
#else
	psrcObj.pbuf = (UINT8*)uladdr;
	psrcObj.bufW = width;
	psrcObj.bufH = height;
	psrcObj.paplaneBuf = NULL;
	psrcObj.fmt = 0;
	psrcObj.roiX = 0;
	psrcObj.roiY = 0;
	psrcObj.roiW = width;
	psrcObj.roiH = height;

	pdstObj.pbuf = psrc;
	pdstObj.bufW = width;
	pdstObj.bufH = height;
	pdstObj.paplaneBuf = NULL;
	pdstObj.fmt = 0;
	pdstObj.roiX = 0;
	pdstObj.roiY = 0;
	pdstObj.roiW = width;
	pdstObj.roiH = height;
	sp5kGfxObjectCopy(&psrcObj,&pdstObj);
#endif
	gfxObjSrc.pbuf = psrc;
	gfxObjOp.pbuf = psrc2;
	gfxObjSrc.roiX = gfxObjSrc.roiY = gfxObjOp.roiX = gfxObjOp.roiY = 0;
	gfxObjSrc.bufH = gfxObjOp.bufH = gfxObjSrc.roiH = gfxObjOp.roiH = height;
	gfxObjSrc.bufW= gfxObjOp.bufW  = gfxObjSrc.roiW = gfxObjOp.roiW = width;
	gfxObjSrc.fmt = gfxObjOp.fmt = SP5K_GFX_FMT_YUV422;
	gfxObjSrc.paplaneBuf = gfxObjOp.paplaneBuf = NULL;
	//separate YUV	------> get Org Y buf unpackage
	memset(psrc2, 0, (width*height*2));
	sample_MoeFMTChg(&gfxObjSrc, &gfxObjOp, SP5K_MOE_DATA_FMT_YUV_PLANE, SP5K_MOE_DATA_FMT_8BIT);
	//get Blur Y 
	sample_MoeImgFilter(psrc2, (psrc2+width*height), width, height);
	 
	gfxObjSrc.pbuf = (psrc2+width*height);
	gfxObjOp.pbuf = psrc;
	//combine to org IM to get Mask Img 
	sample_MoeFMTChg(&gfxObjSrc, &gfxObjOp, SP5K_MOE_DATA_FMT_8BIT, SP5K_MOE_DATA_FMT_PACK_Y);
	 
	 
	gfxObjOp.paplaneBuf = paplane;
#ifdef test_pattern		
	sprintf(szFile, "D:\\img.yuv");
	fp = sp5kFsFileOpen(szFile, FS_OPEN_RDWR);
	if (fp != NULL) {
		sp5kFsFileRead(fp, psrc2, width * height* 2);
		sp5kFsFileClose(fp);
	} else {
		printf("load img file fail\n");
		return;
	}
#else
	psrcObj.pbuf = (UINT8*)uladdr;
	psrcObj.bufW = width;
	psrcObj.bufH = height;
	psrcObj.paplaneBuf = NULL;
	psrcObj.fmt = 0;
	psrcObj.roiX = 0;
	psrcObj.roiY = 0;
	psrcObj.roiW = width;
	psrcObj.roiH = height;

	pdstObj.pbuf = psrc2;
	pdstObj.bufW = width;
	pdstObj.bufH = height;
	pdstObj.paplaneBuf = NULL;
	pdstObj.fmt = 0;
	pdstObj.roiX = 0;
	pdstObj.roiY = 0;
	pdstObj.roiW = width;
	pdstObj.roiH = height;
	sp5kGfxObjectCopy(&psrcObj,&pdstObj);
#endif
	/***********************************************************************************/	
	/***Get Blending result ************************************************************/	
	/***********************************************************************************/		
	gfxObjSrc.pbuf = psrc2;
	gfxObjSrc.paplaneBuf = NULL;
	//blending
	sp5kGfxObjectPerPixelBlend(&gfxObjOp, &gfxObjSrc, &gfxObjSrc);
	 
	sprintf(szFile, "D:\\Final2.jpg");
	sp5kResourceJpgSave(gfxObjSrc.pbuf, width, height, width, height, szFile);
	#ifndef test_pattern	
	psrcObj.pbuf = gfxObjSrc.pbuf;
	psrcObj.bufW = width;
	psrcObj.bufH = height;
	psrcObj.paplaneBuf = NULL;
	psrcObj.fmt = 0;
	psrcObj.roiX = 0;
	psrcObj.roiY = 0;
	psrcObj.roiW = width;
	psrcObj.roiH = height;

	pdstObj.pbuf = (UINT8*)uladdr;
	pdstObj.bufW = width;
	pdstObj.bufH = height;
	pdstObj.paplaneBuf = NULL;
	pdstObj.fmt = 0;
	pdstObj.roiX = 0;
	pdstObj.roiY = 0;
	pdstObj.roiW = width;
	pdstObj.roiH = height;
	sp5kGfxObjectCopy(&psrcObj,&pdstObj);
	#endif
	sp5kYuvBufferFree(psrc);
	sp5kYuvBufferFree(psrc2);
	sp5kYuvBufferFree(paplane);
	return 0;
}

#endif
/*-------------------------------------------------------------------------
 *  File Name : appPbImageCustomizeEffect
 *------------------------------------------------------------------------*/
UINT32
appPbImageCustomizeEffect(
	void
)
{
	PB_VERBOSE("appPbImageCustomizeEffect\n");
	return sp5kImageEdit(SP5K_IMGEDIT_CUSTOMIZE, 0, 0, 0, 0);
}


/*-------------------------------------------------------------------------
 *  File Name : appPbEffectRedEyeReduction
 *------------------------------------------------------------------------*/
UINT32 appPbEffectRedEyeReduction(
	void
)
{
	pbImgCb_t *ppbImgCb = appPb_ImageCbPtrGet();
	sp5kFaceTrackYuvBuf_t ftYuvBuf;
	UINT32 param, i;
	UINT32 addr, w, h;
	sp5kFaceTrackFaceRes_t facelist[12];

	UINT32 ret =SUCCESS;

	sp5kStillPlayYUVInfoGet(SP5K_SPB_IMG_QV, &addr, &w, &h);
	
	gpbRedEyesFaceQvWidth = w;
	gpbRedEyesFaceQvHeight = h;
	
	ftYuvBuf.yuvaddr = (UINT8 *)addr;
	ftYuvBuf.yuvw = w;
	ftYuvBuf.yuvh = h;
	ftYuvBuf.dispx = ftYuvBuf.dispy = ftYuvBuf.dispw = ftYuvBuf.disph = 0;

	if (sp5kFaceTrackYuvBufStart(ppbImgCb->dcfIdx, &ftYuvBuf, 0, 0) == SUCCESS) {
		if (appHostMsgWait(SP5K_MSG_FACE_TRACK_READY,&param,5000)== SUCCESS){
			if (sp5kFaceTrackResultGet(param, &gpbRedEyesFaceNum, facelist) == SUCCESS) {
				if (gpbRedEyesFaceNum > 0 ) {
					for (i = 0; i < gpbRedEyesFaceNum; i++) {
						UINT32 startx, starty, size, attr;
						#if 0
						startx = (facelist[i] & 0xff) << 1;
						starty = (facelist[i] >> 7) & 0x1fe;
						size = (facelist[i] >> 15) & 0x1fe;
						attr = (facelist[i] >> 24) & 0xff;
						#endif
						startx = facelist[i].x;
						starty = facelist[i].y;
						size = facelist[i].size;
						attr = facelist[i].attr;
						
						gpbRedEyesFaceInfo[i].faceX = startx;
						gpbRedEyesFaceInfo[i].faceY = starty;
						gpbRedEyesFaceInfo[i].faceSize = size;
						#if 0
						OsdLib_RECTDraw(startx, starty, startx+size, starty+2);//top
						OsdLib_RECTDraw(startx, starty, startx+2, starty+size);//left
						OsdLib_RECTDraw(startx, starty+size-2, startx+size, starty+size);//bottom
						OsdLib_RECTDraw(startx+size-2, starty, startx+size, starty+size);//right
						#endif
					}
				}else{
					printf("\nRedEye_Reduction no face\n");
				}
			}
		}
		sp5kFaceTrackYuvBufEnd(ppbImgCb->dcfIdx);
	}
	return ret;
}


/*-------------------------------------------------------------------------
 *  File Name : appPbEffectCreate
 *------------------------------------------------------------------------*/
UINT32
appPbEffectCreate(
	pbImgCb_t *pimgCb
)
{
	PB_DBG("%s: IMG (%s) \n", __FUNCTION__, pimgCb->szDcfFileName);

	/*-- checkup disk current status
	 */
	if (FAIL==editValidityCheckup(pimgCb)) {
		return FAIL;
	}

	gpbMainCb.mainStat = PB_MAIN_STAT_JPG_EFFECT;

	return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  File Name : appPbImageEffect
 *------------------------------------------------------------------------*/
UINT32
appPbImageEffect(
	pbUiEffectSel_t effect
)
{
	UINT32 sp5kEff;
	UINT32 imgWidth,imgHeight;

	PB_VERBOSE("appPbImageEffect: effect(%d) \n", effect);

	sp5kEff = editEffectUI2SP5K(effect);
	PB_VERBOSE("%s: Transformed effect(%d) \n", __FUNCTION__, sp5kEff);
	sp5kExifImageInfoGet(SP5K_IMG_PRIMARY, SP5K_IMG_ATTR_WIDTH, &imgWidth);
	sp5kExifImageInfoGet(SP5K_IMG_PRIMARY, SP5K_IMG_ATTR_HEIGHT, &imgHeight);
	return sp5kImageEdit(SP5K_IMGEDIT_ATTR_PREVIEW | SP5K_IMGEDIT_SPECIAL_EFFECT, sp5kEff, imgWidth, imgHeight, 0);
}

/*-------------------------------------------------------------------------
 *  File Name : appPbEffectPreview
 *------------------------------------------------------------------------*/
UINT32
appPbEffectPreview(
	pbUiEffectSel_t effCmd
)
{
	PB_VERBOSE("appPbEffectPreview: %d \n", effCmd);

	switch (effCmd) {
		case PB_UI_EFFECT_BLACK_WHITE:
		case PB_UI_EFFECT_SEPIA:
		case PB_UI_EFFECT_NEGA:
		case PB_UI_EFFECT_MOSAIC:
		case PB_UI_EFFECT_RED:
		case PB_UI_EFFECT_GREEN:
		case PB_UI_EFFECT_BLUE:
			/*-- Do effect */
			gpbMainCb.err = PB_ERR_NONE;
			if (FAIL==appPbImageEffect(effCmd)) {
				PB_VERBOSE("%s:  PB Effect(%d) failed !!\n", __FUNCTION__, effCmd);
				gpbMainCb.err = PB_ERR_EFFECT_ERROR;
				return FAIL;
			}
			break;
		case PB_UI_EFFECT_SKIN_BEAUTY:
			gpbMainCb.err = PB_ERR_NONE;
			sp5kImageEditCallbackSet((fp_pbImgEditCallback_t)appPbEffectBeautySkinProc);
			appPbImageCustomizeEffect();
			break;
		default:
			PB_VERBOSE("%s: Unknown effCmd(%d) \b", __FUNCTION__, effCmd);
			gpbMainCb.err = PB_ERR_NONE;
			sp5kImageEditCallbackSet((fp_pbImgEditCallback_t)appPbEffectRedEyeProc);
			appPbImageCustomizeEffect();
			appPbMsg_BusyWait();
			appPbEffectRedEyeReduction();
			break;
	}
	return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  File Name : appPbEffectExecute
 *------------------------------------------------------------------------*/
UINT32
appPbEffectExecute(
	pbImgCb_t *pimgCb
)
{
	UINT32 status;

	PB_VERBOSE("appPbEffectExecute: %s \n", &pimgCb->szDcfFileName[0]);

	/*-- Save Image */
	if (FAIL==appPbEditSaveDiskCheckup(pimgCb)){
		gpbMainCb.err = PB_ERR_DISK_FULL;
		return FAIL;
	}

	PB_OSD_CLR;
	PB_PIP_0_CLR;
	appPbMsg_ImgSaved();

	status = appPbImageDcfSave(PB_SAVE_NEW);

	PB_OSD_CLR;

	if (status==FAIL) {
		gpbMainCb.err = PB_ERR_DISK_ERROR;
	} else {
		gtotalImgCount = appPbTotalFileCountGet();
		gcurrDcfIdx = gtotalImgCount;
		gpbMainCb.err = PB_ERR_NONE;
	}

	return status;
}


/*-------------------------------------------------------------------------
 *  File Name : appPbEffectDestroy
 *------------------------------------------------------------------------*/
void
appPbEffectDestroy(
	pbImgCb_t *pimgCb
)
{
	PB_VERBOSE("%s: IMG (%s) \n", __FUNCTION__, pimgCb->szDcfFileName);

	/*-- reset ROTATE orientation state */
	gpbMainCb.mainStat = PB_MAIN_STAT_JPG_EFFECT;

	return;
}

/*-------------------------------------------------------------------------
 *  File Name : appPbResizeCreate
 *------------------------------------------------------------------------*/
UINT32
appPbResizeCreate(
	pbImgCb_t *pimgCb
)
{
	PB_DBG("%s: IMG (%s) \n", __FUNCTION__, pimgCb->szDcfFileName);

	/*-- checkup disk current status
	 */
	if (FAIL==editValidityCheckup(pimgCb)) 
	{
		PB_DBG("%s: Illegal filetype !!! \n", __FUNCTION__);
		gpbMainCb.err = PB_ERR_FILETYPE_ERROR;
		return FAIL;
	}

	if (pimgCb->sizeGrade==PB_IMG_SIZE_GRADE_UNKNOWN)
	{
		gpbMainCb.err = PB_ERR_FILETYPE_ERROR;
		return FAIL;
	}
	
	if (pimgCb->sizeGrade < PB_IMG_SIZE_GRADE_1)
	{
		gpbMainCb.err = PB_ERR_FILETYPE_ERROR;
		return FAIL;
	}

	gpbMainCb.err = PB_ERR_NONE;
	gpbMainCb.mainStat = PB_MAIN_STAT_JPG_RESIZE;

	return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  File Name : appPbResizeExecute
 *------------------------------------------------------------------------*/
UINT32
appPbResizeExecute(
	pbImgCb_t *pimgCb,
	pbUiResizeSel_t resizeSel
)
{
	UINT32 srcWidth, srcHeight, tmpU32, freeClust;
	UINT32 tgtWidth, tgtHeight;
	float ratio;

	if (FAIL==editResizeTargetSizeGet(pimgCb, resizeSel, &tgtWidth, &tgtHeight)) 
	{
		return FAIL;
	}

	/*-- Check remaining disk size for new file !!
	 */
	/*-- Disk remaining clusters */
	freeClust = appPbDiskFreeClusterGet(gpbDiskCb.dskInfo.actDsk);

	/*-- Estimate new file size */
	srcWidth = gpbImgCb.imgWidth;
	srcHeight = gpbImgCb.imgHeight;
	ratio = ((float)(tgtWidth*tgtHeight))/(srcWidth*srcHeight);
	tmpU32 = (UINT32)(((float)gpbImgCb.dcfAttr.filesize)*ratio); /* img of small size would have problem */
	tmpU32 = appPbBytes2ClusterCalculate(tmpU32);
	
	#if PB_EDIT_RESIZE_SAVE_NEW
	if (freeClust < tmpU32)
	{
		gpbMainCb.err = PB_ERR_DISK_FULL;
		return FAIL;
	}
	#endif

	/*-- DO RESIZE */
	if (ratio>=1) 
	{
		tmpU32 = sp5kImageEdit(SP5K_IMGEDIT_SCALE_UP, tgtWidth, tgtHeight, 0, 0);
	}
	else 
	{
		tmpU32 = sp5kImageEdit(SP5K_IMGEDIT_SCALE_DOWN, tgtWidth, tgtHeight, 0, 0);
	}
	if (tmpU32==FAIL) 
	{
		gpbMainCb.err = PB_ERR_RESIZE_ERROR;
		return FAIL;
	}

	freeClust = appMsTimeGet();
	PB_OSD_CLR;
	appPbMsg_ImgSaved();

	/*-- SAVE IMAGE */
	#if PB_EDIT_RESIZE_SAVE_NEW
	tmpU32 = appPbImageDcfSave(PB_SAVE_NEW);
	#else
	tmpU32 = appPbImageDcfSave(PB_SAVE_REPLACE);
	#endif

	if (tmpU32==FAIL) 
	{
		gpbMainCb.err = PB_ERR_RESIZE_ERROR;
		return FAIL;
	}
	freeClust = appMsTimeGet()-freeClust;

	if (2000 > freeClust) 
	{
		APP_SLEEP_MS(2000-freeClust);
	}
	PB_OSD_CLR;

	gpbMainCb.err = (tmpU32==FAIL) ? PB_ERR_DISK_ERROR : PB_ERR_NONE;

	return tmpU32;
}

/*-------------------------------------------------------------------------
 *  File Name : appPbResizeDestroy
 *------------------------------------------------------------------------*/
void
appPbResizeDestroy(
	pbImgCb_t *pimgCb
)
{
	PB_VERBOSE("%s: IMG (%s) \n", __FUNCTION__, pimgCb->szDcfFileName);

	gpbMainCb.mainStat = PB_MAIN_STAT_JPG_RESIZE;

	return;
}

/*-------------------------------------------------------------------------
 *  File Name : ROTATE_DialogConfig
 *------------------------------------------------------------------------*/
static UINT32 strID_DlgRotate[3] = {ID_STR_ROTATE, ID_STR_SAVE, ID_STR_BACK};
static
void
ROTATE_DialogConfig(
	void
)
{
	PB_VERBOSE("%s \n", __FUNCTION__);

	appPlayOsd_DlgBoxConfig(ID_STR_ROTATE_THIS_PHOTO_, 3, strID_DlgRotate, 0, 20);
}

/*-------------------------------------------------------------------------
 *  File Name : ROTATE_DialogShow
 *------------------------------------------------------------------------*/
static
void
ROTATE_DialogShow(
	void
)
{
	PB_VERBOSE("%s \n", __FUNCTION__);

	appPlayOsd_DlgBoxShow();
	appPbOsd_AuxNote_OKSelect(TRUE);
}


/*-------------------------------------------------------------------------
 *  File Name : appGetCurImgOrient
 *------------------------------------------------------------------------*/
UINT32 appPbGetCurImgOrient(
	void
)
{
	UINT32 ret = FAIL;
	UINT32 orientTag = 0;
	UINT32 orient = SP5K_ROTATE_0;

	ret = sp5kExifImageInfoGet(SP5K_IMG_PRIMARY, SP5K_IMG_ATTR_ORIENT, (UINT32 *)&(orientTag));

	if(ret != SUCCESS){
		printf("sp5kExifImageInfoGet FAIL!\n");
		return 0;
	}

	if(orientTag == 6){  		/* exif 270-degree. */
		orient= SP5K_ROTATE_270;
	}
	else if(orientTag == 3){  	/* exif 180-degree. */
		orient = SP5K_ROTATE_180;
	}
	else if(orientTag == 8){  	/* exif 90-degree. */
		orient = SP5K_ROTATE_90;
	}
	else {  					/* exif 0-degree. */
		orient = SP5K_ROTATE_0;
	}
	return orient;
}


/*-------------------------------------------------------------------------
 *  File Name : appPbRotateCreate
 *------------------------------------------------------------------------*/
UINT32
appPbRotateCreate(
	pbImgCb_t *pimgCb
)
{
	PB_DBG("%s: IMG (%s) \n", __FUNCTION__, pimgCb->szDcfFileName);

	/*-- checkup disk current status
	 */
	PB_OSD_CLR;
	if (FAIL==editValidityCheckup(pimgCb)) {
		appPbErrorProcess(PB_ERR_FILETYPE_ERROR);
		return FAIL;
	} else if (pimgCb->isProtected) {
		appPbErrorProcess(PB_ERR_IMG_PROTECTED);
		return FAIL;
	}
	
	pimgCb->rotOrient = appPbGetCurImgOrient();

	gpbMainCb.mainStat = PB_MAIN_STAT_JPG_ROTATE;

	ROTATE_DialogConfig();
	ROTATE_DialogShow();

	gpbMainCb.subStat = PB_ROTATE_STAT_CONFIRM_MENU;

	return SUCCESS;
}


/*-------------------------------------------------------------------------
 *  File Name : appPbRotatePreview
 *------------------------------------------------------------------------*/
UINT32
appPbRotatePreview(
	pbRotCmdDef_t rotCmd
)
{
	UINT32 sp5kRot;

	PB_VERBOSE("%s: rotCmd(%d) \n", __FUNCTION__, rotCmd);

	switch (rotCmd) {
	default:
		PB_DBG("ROT: !!ERROR!!, Illegal rotCmd(%d) !!\n", rotCmd);
		return FAIL;
	case PB_ROT_CW:
		PB_VERBOSE("ROT: CW Rotate \n");
		if (gpbImgCb.rotOrient==PB_ROT_ORIENT_0) {
			gpbImgCb.rotOrient = PB_ROT_ORIENT_270;
		} else {
			gpbImgCb.rotOrient--;
		}
		break;
	case PB_ROT_CCW:
		PB_VERBOSE("ROT: CCW Rotate \n");
		if (gpbImgCb.rotOrient==PB_ROT_ORIENT_270) {
			gpbImgCb.rotOrient = PB_ROT_ORIENT_0;
		} else {
			gpbImgCb.rotOrient++;
		}
		break;
	case PB_ROT_CANCEL:
		PB_VERBOSE("ROT: CANCEL !! \n");
		gpbImgCb.rotOrient = PB_ROT_ORIENT_0;
		break;
	}

	sp5kRot = editRotateOrientUI2SP5K(gpbImgCb.rotOrient);
	#if PLAYBACK_ROTATE_BY_EXIF_TAG
	return sp5kImageEdit(SP5K_IMGEDIT_ROTATE_EXIF_TAG, sp5kRot, 0, 0, 0);
	#else
	return sp5kImageEdit(SP5K_IMGEDIT_ROTATE, sp5kRot, 0, 0, 0);
	#endif
	
}

/*-------------------------------------------------------------------------
 *  File Name : appPbRotateExecute
 *------------------------------------------------------------------------*/
UINT32
appPbRotateExecute(
	pbImgCb_t *pimgCb,
	pbRotCmdDef_t rotCmd
)
{
	UINT32 status;
	UINT32 elapT;

	PB_VERBOSE("appPbRotateExecute: cmd(%d), %s \n", rotCmd, &pimgCb->szDcfFileName[0]);

	switch (rotCmd) {
	default:
		PB_VERBOSE("%s: Unknown rotCmd(%d) \b", __FUNCTION__, rotCmd);
		status = FAIL;
		break;
	#if 0
	case PB_ROT_CW: /* Rotate Clockwise */
	case PB_ROT_CCW: /* Rotate Counter Clockwise */
		status = appPbRotatePreview(rotCmd);
		break;
	#endif
	case PB_ROT_SAVE:
		PB_PIP_0_CLR;
		PB_OSD_CLR;
		elapT = appMsTimeGet();
		appPbMsg_ImgSaved();

		#if (PB_EDIT_ROTATE_SAVE_NEW)
		if (FAIL==appPbEditSaveDiskCheckup(pimgCb)){
			gpbMainCb.err = PB_ERR_DISK_FULL;
			return FAIL;
		}
		status = appPbImageDcfSave(PB_SAVE_NEW);
		#else
		status = appPbImageDcfSave(PB_SAVE_REPLACE);
		#endif

		elapT = appMsTimeGet()-elapT;
		if (2000 > elapT) {
			APP_SLEEP_MS(2000-elapT);
		}

		if (status==FAIL) {
			gpbMainCb.err = PB_ERR_DISK_ERROR;
		} else {
			gtotalImgCount = appPbTotalFileCountGet();
			gcurrDcfIdx = gtotalImgCount;
			gpbMainCb.err = PB_ERR_NONE;
		}
		break;
	case PB_ROT_CANCEL:
		status = appPbRotatePreview(rotCmd);
		if (status==FAIL) {
			gpbMainCb.err = PB_ERR_ROTATE_ERROR;
		}
		break;
	}

	return status;
}

/*-------------------------------------------------------------------------
 *  File Name : appPbRotateDestroy
 *------------------------------------------------------------------------*/
void
appPbRotateDestroy(
	pbImgCb_t *pimgCb
)
{
	PB_VERBOSE("%s: IMG (%s) \n", __FUNCTION__, pimgCb->szDcfFileName);

	gpbMainCb.mainStat = PB_MAIN_STAT_JPG_ROTATE;
	return;
}





/*-------------------------------------------------------------------------
 *  File Name : appPbRotateStClose
 *------------------------------------------------------------------------*/
static
void
appPbRotateStClose(
	void
)
{
	PB_DBG("EDIT: State closing \n");
	gpbMainCb.subStat = PB_ROTATE_STAT_NULL;
}

/*-------------------------------------------------------------------------
 *  File Name : appPbRotateStExit
 *------------------------------------------------------------------------*/
static
void
appPbRotateStExit(
	UINT32 nextSt,
	UINT32 stMsg
)
{
	PB_DBG("EDIT: State exiting \n");

	gpbMainCb.mainStat = PB_MAIN_STAT_ROTATE_SUSPEND;

	appPbRotateStClose();
	appStateChange(nextSt, stMsg);
}

/*-------------------------------------------------------------------------
 *  File Name : appPbRotateStInit
 *------------------------------------------------------------------------*/
static
UINT32
appPbRotateStInit(
	void
)
{
	PB_VERBOSE("%s ... \n", __FUNCTION__);

	gpbMainCb.subStat = PB_ROTATE_STAT_STANDBY;
	sp5kHostMsgSend(APP_UI_MSG_PB_ROTATE_CREATE, 0);

	return SUCCESS;
}


/*-------------------------------------------------------------------------
 *  File Name : appPbRotateStOnMsg
 *------------------------------------------------------------------------*/
void
appPbRotateStOnMsg(
	UINT32 msg,
	UINT32 param
)
{
	PB_VERBOSE("ROTATE: (OnMsg) 0x%08x, 0x%08x ... \n", msg, param);

	switch (msg) {
	case SP5K_MSG_IMGEDIT_DONE:
		PB_VERBOSE("SP5K_MSG_IMGEDIT_DONE(0x%08x)\n", param);
		#if 0
		if (param!=SUCCESS) {
			printf("SIMG: SAVE IMG FAILED !! \n");
			gpbMainCb.err = PB_ERR_SAVE_IMG_ERROR;
		}
		appTimerClear(&gpbMainCb.asyncOpTmrId); /*-- Clear the TIMEOUT timer for IMGEDIT_SAVE */
		gpbMainCb.err = PB_ERR_NONE;
		appPbRotateStExit(APP_STATE_PLAY_MAINMENU, STATE_PARAM_NORMAL_INIT);
		#endif
		break;

	case APP_UI_MSG_PB_ROTATE_CREATE:
		if (FAIL==appPbRotateCreate(&gpbImgCb)) {
			appPbRotateStExit(APP_STATE_MENU, STATE_PARAM_NORMAL_INIT);
		}
		break;
	default :
		break;
	}
}


/*-------------------------------------------------------------------------
 *  File Name : appPbRotateStOnKey
 *------------------------------------------------------------------------*/
void
appPbRotateStOnKey(
	UINT32 msg,
	UINT32 param
)
{
	uiPara_t* puiPara = appUiParaGet();
	
	PB_VERBOSE("ROTATE: (OnKey) [0x%x] [0x%x]\n", msg,param);

	switch (msg) {
	case APP_KEY_PRESS_UP:
	case APP_KEY_PRESS_DOWN:
		if (gpbMainCb.subStat==PB_ROTATE_STAT_CONFIRM_MENU) {
			PB_BEEP;
			appPlayOsd_DlgBoxScroll(((msg==APP_KEY_PRESS_UP) ? TRUE : FALSE));
		}
		break;
	case APP_KEY_PRESS_SET:
		if (gpbMainCb.subStat != PB_ROTATE_STAT_CONFIRM_MENU) 
		{
			PB_VERBOSE("ROTATE: Ignore SET Key, Wrong state !! \n");
			break;
		}
		PB_BEEP;
		puiPara->selImgRotate=appPlayOsd_DlgBoxSelectGet();
		if (puiPara->selImgRotate==PB_UI_ROTATE_SELECT_SET) 
		{
		
			/*-- Rotate */
			if (FAIL==appPbRotatePreview(PB_ROT_CW)) 
			{
				appPbErrorProcess(gpbMainCb.err);
				appPbRotateDestroy(&gpbImgCb);
				appStateChange(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
				return;
			}
		} 
		else if (puiPara->selImgRotate==PB_UI_ROTATE_SELECT_SAVE) 
		{
			/*-- SAVE */
			#if KIT_WITH_HDMI   /*Add by Lation@20130717*/
			if(IS_HDMI_IN) /*add for mantis bug 0047536*/
			{
			#if HDMI_MENU_BY_JPG
				menuHDMIClean();
			#endif
			}
			#endif
			if (FAIL==appPbRotateExecute(&gpbImgCb, PB_ROT_SAVE)) 
			{
				appPbErrorProcess(gpbMainCb.err);
			}
			appPbMsg_ImgSaved();
			appPbRotateDestroy(&gpbImgCb);
			appStateChange(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
			return;
		} 
		else 
		{
			/*-- BACK */
			appPlayOsd_DlgBoxClear();
			appPbRotateExecute(&gpbImgCb, PB_ROT_CANCEL);
			gpbMainCb.err = PB_ERR_NONE;
			appPbRotateDestroy(&gpbImgCb);
			appStateChange(APP_STATE_MENU, STATE_PARAM_NORMAL_INIT);
			return;
		}
		break;

	default:
		break;
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appPbRotateState
 *------------------------------------------------------------------------*/
void
appPbRotateState(
	UINT32 msg,
	UINT32 param
)
{
	PB_VERBOSE("%s : [0x%x] [0x%x]\n",__FUNCTION__,msg,param);

	switch (msg) {
	case APP_STATE_MSG_INIT:
		RPrintf("appPbRotateState");
		appPbRotateStInit();
		/* do nothing */
		break;
	case APP_STATE_MSG_CLOSE:
		appPbRotateStClose();
		appStateCloseDone();
		break;
	default:
		if (IS_APP_KEY_MSG(msg)) {
			appPbRotateStOnKey(msg,param);
		}
		else{
			appPbRotateStOnMsg(msg,param);
		}
		break;
	}

}

#define PB_LOMO_SHIFT_BITS		8
/*-------------------------------------------------------------------------
 *  File Name : appLomoTableCreate
 *------------------------------------------------------------------------*/
static UINT16 *appLomoTableCreate(
	UINT32 width, /*!< 8n */
	UINT32 height,
	UINT32 thresold /*!< 1~100 */
)
{
	UINT16 *pbuf=NULL;
	UINT32 rth, rmax, rd, x0, y0;
	UINT32 i, j, r;
	printf("%s : [%d] [%d] [%d]\n",__FUNCTION__,width,height,thresold);

	/**
	 * \note y = f(r)
	 * y = 1  if r <= rth
	 * y = (rmax - r) / (rmax - rth) if r > rth
	 */  
	pbuf = sp5kMallocCache(width * height * sizeof(UINT16));
	if (pbuf) {
		/*!< create scale table for testing only */
		rmax = (width * width + height * height) >> 2;
		rth = thresold * rmax / 100;
		rd = rmax - rth;
		x0 = width >> 1;
		y0 = height >> 1;
		for (j = 0; j < height; j++) {
			for (i = 0; i < width; i++) {
				r = (i - x0) * (i - x0) + (j - y0) * (j - y0);
				if (r <= rth) {
					pbuf[j * width + i] = (1 << PB_LOMO_SHIFT_BITS);
				}
				else {
					/*!< possible overflow? */
					pbuf[j * width + i] = (1 << PB_LOMO_SHIFT_BITS) * (rmax - r) / rd;
				}
			}	
		}
	}

	return pbuf;
}

/*!< apply lomo table into the input yuv frame */
static UINT32 appLomoApply(
	UINT8 *psrc,
	UINT32 srcWidth, /*!< 8n */
	UINT32 srcHeight,
	UINT32 srcDramW,
	UINT16 *plomoBuf,
	UINT32 lomoWidth, /*!< 8n */
	UINT32 lomoHeight
)
{
	UINT32 ret=FAIL;
	UINT32 scaX, scaY, gain, pos;
	UINT8 *psrcY;
	UINT32 i, j;
	
	printf("%s : [0x%p] [%d] [%d] [0x%p] [%d] [%d]\n",__FUNCTION__,psrc,srcWidth,srcHeight,plomoBuf,lomoWidth,lomoHeight);

	if (psrc == NULL || plomoBuf == NULL) {
		return FAIL;
	}

	if ((srcWidth < lomoWidth) || (srcHeight < lomoHeight)) {
		return FAIL;
	}

	scaX = (srcWidth + lomoWidth - 1) / lomoWidth;
	scaY = (srcHeight + lomoHeight - 1) / lomoHeight;

	/*!< to cache address */
	psrcY = (UINT8 *)((UINT32)psrc & 0x8FFFFFFF);
	for (j = 0; j < srcHeight; j++) {
		pos = (j / scaY) * lomoWidth ;
		for (i = 0; i < srcWidth; i++) {
			gain = plomoBuf[pos + (i / scaX)];
			psrcY[((i >> 3) << 4) + (i & 7)] = (gain * psrcY[((i >> 3) << 4) + (i & 7)]) >> PB_LOMO_SHIFT_BITS;
		}
		psrcY += srcDramW * 2;
	}
	cpuCacheFlushAll(0x200/*D_CACHE_FLUSH_ALL_WBINV*/);
	ret = SUCCESS;

	return ret;
}

UINT8
appPbEffectLomo(
	UINT32 srcAddr, UINT32 width, UINT32 height
)
{
 printf("\n appPbEffectLomo \n");
	UINT16 *plomoBuf, lomoW, lomoH, ret;
	lomoW = 320;
	lomoH = 240;
	plomoBuf = appLomoTableCreate( lomoW, lomoH, 20 );
	if (!plomoBuf) {
		printf("lomo table failed\n");
		return FAIL;
	}
	ret = appLomoApply((UINT8 *)srcAddr, width, height, width, plomoBuf, lomoW, lomoH);
	if (plomoBuf) {
		sp5kFree(plomoBuf);
	}
	return ret;
}

void
appPBEffectLomoFunction(
void
)
{
	UINT32 qvWidth, qvHeight, mode;

	qvWidth = qvWidth;
	qvHeight = qvHeight;
	mode = mode;
	
	sp5kModeSet(SP5K_MODE_STANDBY);
	sp5kModeWait(SP5K_MODE_STANDBY);
	
	UINT32 arr[4] = {320,240, 320, 240};
	sp5kCapCfgQvResolutionSet((sp5kImgSize_t *)arr);
	
	sp5kModeSet(SP5K_MODE_STILL_PLAYBACK);
	sp5kModeWait(SP5K_MODE_STILL_PLAYBACK);
	
	sp5kImageEditCallbackSet((fp_pbImgEditCallback_t)appPbEffectLomo);
	sp5kImageEdit(SP5K_IMGEDIT_CUSTOMIZE, 0, 0, 0, 0);
	appPbImageDcfSave(PB_SAVE_NEW);
	sp5kImageEditCallbackSet(NULL);
	
	//UINT32 arr1[4] = {1280, 760, 320, 240};
	sp5kCapCfgQvResolutionSet((sp5kImgSize_t *)arr);
	
	sp5kModeSet(SP5K_MODE_STANDBY);
	sp5kModeWait(SP5K_MODE_STANDBY);
	gpbMainCb.totalImg++;
	gpbImgCb.dcfIdx = gpbMainCb.totalImg;
	
	sp5kModeSet(SP5K_MODE_STILL_PLAYBACK);
	sp5kModeWait(SP5K_MODE_STILL_PLAYBACK);
	appPbImagePlay(gpbMainCb.totalImg);

}

void appPbImgEffectInit()
{
	UINT32 strId;
	
	strId = ID_STR_SAVE_THIS_FILE_;
	appPlayOsd_DlgConfirmYesNoConfig(strId, 1);
	appPlayOsd_DlgBoxShow();
}

void appPbEffectState(
	UINT32 msg,
	UINT32 param
)
{	
	pbUIParam_t* gpbUiParam=appPb_UiParamPtrGet();
	pbMainCB_t* gpbMainCb=appPb_MainCbPtrGet(); 
	pbImgCb_t *ppbImgCb = appPb_ImageCbPtrGet();
	
	PB_VERBOSE("%s : [0x%x] [0x%x]\n",__FUNCTION__,msg,param);

	switch (msg) {
	case APP_STATE_MSG_INIT:
		appPbImgEffectInit();
		/* do nothing */
		break;
	case APP_STATE_MSG_CLOSE:
		appStateCloseDone();
		break;
	case APP_KEY_PRESS_UP:
		PB_BEEP;
		appPlayOsd_DlgBoxScroll(TRUE );
		break;
	case APP_KEY_PRESS_DOWN:
		PB_BEEP;
		appPlayOsd_DlgBoxScroll(FALSE);
		break;
	case APP_KEY_PRESS_SET:
		if (0==appPlayOsd_DlgBoxSelectGet()) 
		{
			if (FAIL==appPbEffectExecute(ppbImgCb)) {
				appPbErrorProcess(gpbMainCb->err);
				menuProcDisp();
			} else {
				appPbEffectDestroy(ppbImgCb);
				appStateChange(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
			}
			if(gpbUiParam->selImgEffect == PB_UI_EFFECT_RED_EYE || gpbUiParam->selImgEffect ==PB_UI_EFFECT_SKIN_BEAUTY)
			{ 
				sp5kImageEditCallbackSet(NULL);
			}
		}
		else{
			appPbEffectDestroy(ppbImgCb);
			appStateChange(APP_STATE_MENU, STATE_PARAM_NORMAL_INIT);
		}
		break;
	default:
		break;
	}

}



