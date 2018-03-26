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
#include "stdio.h"
#include "stdarg.h"
#include "string.h"
#include "app_com_def.h"
#include "app_com_api.h"

#include "sp5k_global_api.h"
#include "app_osd_api.h"
#include "app_osd.h"
#include "app_battery.h"



/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
	

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

void appTbCIPA_FileDelete(void);
void apptbCIPA_Log(UINT32 capCnt);
void appTbCIPA_CapNumShow(UINT32 Cnt);


/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
 
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define CIPA_CAPTURE_CNT_X	150
#define CIPA_CAPTURE_CNT_Y	100

 
/**************************************************************************
 *								M A C R O S 							  *
 **************************************************************************/
	


/*-------------------------------------------------------------------------
 * Function : _TbCIPA_AdcGet
 *------------------------------------------------------------------------*/
static UINT32 
_TbCIPA_AdcGet(
	void
)
{

 	UINT8 i=0;
	UINT32 BatGet =0;
	UINT32 BatValue=0;

	UINT8 cnt =10;

	for (i=0 ; i<cnt ; i++){
		BatGet=appBatteryAdcValueGet();
		BatValue=BatValue+BatGet;
		APP_SLEEP_MS(10);
	}

	BatValue =BatValue/cnt;

	return BatValue;
	
}


/*-------------------------------------------------------------------------
 * Function : appTbCIPA_FileDelete
 *------------------------------------------------------------------------*/
void 
appTbCIPA_FileDelete(
	void
)
{
	sp5kFsFileDelete("D:\\CIPA_LOG.TXT");
	APP_SLEEP_MS(1000);
}



/*-------------------------------------------------------------------------
 *  File Name : appTbCIPA_CapNumShow
 *------------------------------------------------------------------------*/
void
appTbCIPA_CapNumShow(
	UINT32 Cnt
)
{
	char buf[12];
	UINT32 BatAD =0;

	BatAD=_TbCIPA_AdcGet();
	sprintf(buf,"%05d %04d",Cnt,BatAD);

	appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, CIPA_CAPTURE_CNT_X, CIPA_CAPTURE_CNT_Y,100 ,32, 0, 0);

	appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_TextDraw(CIPA_CAPTURE_CNT_X,CIPA_CAPTURE_CNT_Y,SP5K_GFX_ALIGN_TOP_LEFT,buf);

}




/*-------------------------------------------------------------------------
 * Function : apptbCIPA_Log
 *------------------------------------------------------------------------*/
void 
apptbCIPA_Log(
	UINT32 capCnt
)
{
	UINT32 fd;
	UINT8 szTmp[12];
	UINT32 BatAD =0;

	BatAD=_TbCIPA_AdcGet();

	/*-- Open log file */
	fd = sp5kFsFileOpen("D:\\CIPA_LOG.TXT", SP5K_FS_OPEN_APPEND|SP5K_FS_OPEN_RDWR);
	if (fd==0) {
		fd = sp5kFsFileOpen("D:\\CIPA_LOG.TXT", SP5K_FS_OPEN_CREATE|SP5K_FS_OPEN_RDWR);
		if (fd==0) {
			printf("\nFILE Open failed !!\n\n");
			return;
		}
	}else{
		sp5kFsFileSeek(fd, 0, SP5K_FS_SEEK_END);
	}

	sprintf(szTmp, "%05d %04d\n", capCnt,BatAD);
	printf("\nCIPALog:%s\n",szTmp);

	sp5kFsFileWrite(fd, szTmp, strlen(szTmp));
	
	sp5kFsFileClose(fd);

}




