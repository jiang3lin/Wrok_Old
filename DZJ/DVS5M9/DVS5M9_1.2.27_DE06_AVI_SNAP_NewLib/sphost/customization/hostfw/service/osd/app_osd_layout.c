/**************************************************************************
 *         Copyright(c) 2012 by iCatch Technology Co., Ltd.              *
 *                                                                        *
 *  This software is copyrighted by and is the property of iCatch Tech-  *
 *  nology Co., Ltd. All rights are reserved by iCatch Technology Co.,   *
 *  Ltd. This software may only be used in accordance with the            *
 *  corresponding license agreement. Any unauthorized use, duplication,   *
 *  distribution, or disclosure of this software is expressly forbidden.  *
 *                                                                        *
 *  This Copyright notice "M U S T" not be removed or modified without    *
 *  prior written consent of iCatch Technology Co., Ltd.                 *
 *                                                                        *
 *  iCatch Technology Co., Ltd. reserves the right to modify this        *
 *  software without notice.                                              *
 *                                                                        *
 *  iCatch Technology Co., Ltd.                                          *
 *  19-1, Innovation First Road, Science-Based Industrial Park,           *
 *  Hsin-Chu, Taiwan, R.O.C.                                              *
 *                                                                        *
 *************************************************************************/

 /**
 * @author    GQ
 * @date      2012-11-26
*/

#include "app_com_def.h"
#include "app_osd_api.h"
#include "app_osd.h"
#include "app_osdfont.h"
#include "sp5k_gfx_api.h"
#include "sp5k_fs_api.h"
#include "sp5k_os_api.h"
#include "gpio_custom.h"
#include "api/sp5k_disp_api.h"  /*clean warning.*/
#include "app_icon_def.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
//now the ini file limit to record 512 ICONs' info,and each ICON was used 4bytes area
//if you've not modified the iconlayout2bin.exe,you should not to change this macro's value

#define appOsdIconPosFileSize   2048

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
static UINT16 *appOsdIconPos=NULL;
static iconLayoutFile_t lytFile[] = {
	[APP_ICON_MODE_STLLVIEW]  		= {"A:\\RO_RES\\UI\\ICON_POS\\iconPV.BIN"},
	[APP_ICON_MODE_STLLVIEW_HDMI]   = {"A:\\RO_RES\\UI\\ICON_POS\\iconPV_HDMI.BIN"},	
	[APP_ICON_MODE_VIDEOVIEW] 		= {"A:\\RO_RES\\UI\\ICON_POS\\iconVid.BIN"},
	[APP_ICON_MODE_VIDEOVIEW_HDMI] 	= {"A:\\RO_RES\\UI\\ICON_POS\\iconVid_HDMI.BIN"},
	[APP_ICON_MODE_AUDIO]     		= {"A:\\RO_RES\\UI\\ICON_POS\\iconAud.BIN"},
	[APP_ICON_MODE_AUDIO_HDMI]     	= {"A:\\RO_RES\\UI\\ICON_POS\\iconAud_HDMI.BIN"},
	[APP_ICON_MODE_PLAYBACK]  		= {"A:\\RO_RES\\UI\\ICON_POS\\iconPB.BIN"},
	[APP_ICON_MODE_PLAYBACK_HDMI]  	= {"A:\\RO_RES\\UI\\ICON_POS\\iconPB_HDMI.BIN"},
	[APP_ICON_MODE_VIDEOPLAY] 		= {"A:\\RO_RES\\UI\\ICON_POS\\iconPBVid.BIN"},
	[APP_ICON_MODE_VIDEOPLAY_HDMI] 	= {"A:\\RO_RES\\UI\\ICON_POS\\iconPBVid_HDMI.BIN"},
	[APP_ICON_MODE_VIDEOPLAY_HDMI_DVS5M2] 	= {"A:\\RO_RES\\UI\\ICON_POS\\iconPBVid_HDMI_DVS5M2.BIN"},
};

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
 UINT32 appOsdIconModeSet(UINT32 mode)
 {
	UINT32 fd;	 
	static UINT32 preMode = 0xff;
	UINT8 *fileName = lytFile[mode].fileName;

	if(mode >= APP_ICON_MODE_MAX){
		return FAIL;
	}

	if(mode == preMode && appOsdIconPos){
		return SUCCESS;
	}

	fd = sp5kFsFileOpen(fileName,FS_OPEN_RDONLY | FS_OPEN_BINARY);
	if(!fd){
		printf("ERR:%s open fail!!!\n",fileName);
		return FAIL;
	}

	if(sp5kFsFileSizeGet(fd) != appOsdIconPosFileSize){
		printf("ERR:file size is not match!!!\n");
		sp5kFsFileClose(fd);
		return FAIL;
	}

	if(appOsdIconPos){
		sp5kFree(appOsdIconPos);
	}

	appOsdIconPos = sp5kMalloc(appOsdIconPosFileSize);
	if(appOsdIconPos == NULL){
		printf("ERR:memory alloc fail!!!");
		return FAIL;
	}

	sp5kFsFileRead(fd,(UINT8*)appOsdIconPos,appOsdIconPosFileSize);
	sp5kFsFileClose(fd);

	printf("OSD load %s\n",fileName);
	preMode = mode;
	return SUCCESS;
}

UINT32 appOsdIconPosGet(UINT32 iconID,UINT16 *x,UINT16 *y)
{
	UINT16 idx;

	if(appOsdIconPos == NULL){
		printf("ERR:not found file which record the ICON postion info!!!\n");
		return FAIL;
	}

	if(iconID > (appOsdIconPosFileSize/4)){
		printf("ERR:ID:0x%4x is overflow!!!,limit up to 0x200\n",iconID);
		return FAIL;
	}

	idx = iconID*2;
	*x = appOsdIconPos[idx];
	*y = appOsdIconPos[idx+1];

	if( (*x == 0xffff) && (*y == 0xffff) ){
		printf("ERR:ID:0x%4x,not found position info!!!\n",iconID);
		return FAIL;
	}
	return SUCCESS;
}

UINT32 appIconShow(UINT32 iconID,UINT8 show,UINT32 align)
{
	UINT16 x,y;
	UINT32 width,height;
	UINT32 osdLimitX = OSD_DEFAULT_FRAMSIZE_W,osdLimitY = OSD_DEFAULT_FRAMSIZE_H;

	#if (KIT_WITH_HDMI)
	if(IS_HDMI_IN){
		/* see appHDMIshowOSD() */
		osdLimitX = 640;
		osdLimitY = 360;
	}
	#endif

	if(appOsdIconPosGet(iconID,&x,&y) == FAIL){
		return FAIL;
	}
	if(iconID==ID_ICON_VIDEOPLAY_START || iconID==ID_ICON_VIDEOPLAY_PAUSE)
	{
		y-=10;
		x+=10-90;
	}
	if((x>=osdLimitX) || (y>=osdLimitY)){
		printf("ERR:position is over the OSD buffer limit!\n");
		return FAIL;
	}

	if(show){
		appOsdLib_OsdLayIconDraw((SINT32)x,(SINT32)y,align,iconID);
	}else{
		sp5kGfxIconSizeGet(iconID,&width,&height);
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, (SINT32)x, (SINT32)y,(SINT32)width,(SINT32)height, 0, 0);		 
	}
	return SUCCESS;
}


