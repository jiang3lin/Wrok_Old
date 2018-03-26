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
#include "sp5k_rsvblk_api.h"
#include "sp5k_fs_api.h"
#include "app_jpg_draw.h"
#include "sp5k_utility_api.h"
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

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

UINT32
appJpgFileDraw(
	UINT32 drawLayer, 
	UINT8 *filename,
	UINT32 x0,
	UINT32 y0,
	UINT32 sizeW,
	UINT32 sizeH
)
{
	UINT32 ret, param[4];
	UINT32 layer[JPG_DRAW_LAYER_MAX] = {SP5K_RES_TYPE_JPG, 
										SP5K_RES_TYPE_JPG_PIP, 
										SP5K_RES_TYPE_JPG_PF };
	
	if ((drawLayer >= JPG_DRAW_LAYER_MAX) || (filename == NULL)) {
		printf("[ERR] %s (%d, %s)\n", __FUNCTION__, drawLayer,filename);
		return FAIL;
	}
	
	if ((sizeW == 0) && (sizeH == 0)) {
		param[0] = 0;
		param[1] = 0;
		param[2] = 320;
		param[3] = 240;
	}
	else{
		param[0] = x0;
		param[1] = y0;
		param[2] = sizeW;
		param[3] = sizeH;
	}
	ret = sp5kResourceFilePlay(layer[drawLayer], filename, param);	
	return ret;
}

UINT32 appRsvFileScaleDraw(UINT32 pageID,UINT32 fileType,UINT8 *fileName,UINT32 X,UINT32 Y,UINT32 scaleW,UINT32 scaleH)
{
	UINT32 loadID = 0,ret = FAIL;
	UINT8 *pDstYUVAddr;
	UINT8 *pSrcYUVAddr;
	sp5kGfxObj_t SrcGfxObj;
	sp5kGfxObj_t DstGfxObj;
	sp5kGfxRoi_t RoiObj;

	// align to 16
	scaleW = (scaleW+15)>>4<<4;
	scaleH = (scaleH+15)>>4<<4;

	pDstYUVAddr = sp5kYuvBufferAlloc(scaleW,scaleH);
	
	sp5kRsvPreloadRelease();
	sp5kRsvPreloadDrawCfgSet(SP5K_RES_PRELOAD_NUM,1);
	ret = sp5kRsvPreloadFile(fileType,fileName,loadID,0,0,0);
	if(ret == FAIL){
		return ret;
	}
	ret = sp5kRsvPreloadGfxObjGet(loadID,&SrcGfxObj);
	if(ret == FAIL){
		return ret;
	}
	
	DstGfxObj.pbuf = pDstYUVAddr;
	DstGfxObj.bufW = scaleW;
	DstGfxObj.bufH = scaleH;
	DstGfxObj.fmt = SP5K_GFX_FMT_YUV422;	
	DstGfxObj.roiX = DstGfxObj.roiY =0;
	DstGfxObj.roiW = scaleW;
	DstGfxObj.roiH = scaleH;
	DstGfxObj.paplaneBuf = NULL;

	RoiObj.roiX = X;
	RoiObj.roiY = Y;
	RoiObj.roiW = DstGfxObj.roiW;
	RoiObj.roiH = DstGfxObj.roiH;

	sp5kGfxObjectScale(&SrcGfxObj,&DstGfxObj);

	ret = sp5kGfxObjectDraw(pageID,&RoiObj,0,&DstGfxObj);
	sp5kYuvBufferFree(pDstYUVAddr);
	sp5kRsvPreloadRelease();
	return ret;
}


