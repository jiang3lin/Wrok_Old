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
#include "app_com_api.h"
#include "app_com_def.h"
#include "sp5k_ftrack_api.h"
#include "app_mmface_detect.h"
#include "app_still.h"
#include "api/sp5k_capture_api.h"
#include "sp5k_utility_api.h"
#if ICAT_FCWS
#include "app_cvrws_api.h"
#endif


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

static UINT32 ftrackFaceData;
static appFdResult_t faceInfo;
/* Add by sc.liu */
static UINT32 otrackObjectData;
static appOdResult_t objectInfo;
/* Add by sc.liu */
static UINT32 BlinkDetectSem;
UINT8 *BlKChkyuvBuf;


/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern UINT32 appVideoSizeGet(UINT8 size_idx, UINT32 *pW, UINT32 *pH);


/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
UINT32 ros_semaphore_create(UINT8 *name, UINT32 init_val);
UINT32 ros_semaphore_put(UINT32 semaphore_id);
UINT32 ros_semaphore_get(UINT32 semaphore_id, UINT32 timeout);

UINT32 View_CoordVisible(UINT32 x, UINT32 y);
/* Add by sc.liu */
/*-------------------------------------------------------------------------
 *  File Name : _odBinDynamicLoad
 *------------------------------------------------------------------------*/
UINT32
_odBinDynamicLoad(
	void
)
{
#ifdef HW_FACETRACK

#else
#ifdef USE_APP_FACETRACK
	if (odAppInit(APP_FACETRACK_FILE) == FAIL) {
		printf("[%s] line %d: ftAppInit failed\n", __FUNCTION__, __LINE__);
		return FAIL;
	}	
#endif
#endif
	return SUCCESS;
}
/* Add by sc.liu */


/*-------------------------------------------------------------------------
 *  File Name : _fd_MaxFaceSet
 *------------------------------------------------------------------------*/
void 
_fd_MaxFaceSet(
	UINT32 val
)
{
	sp5kFaceTrackCfgSet(SP5K_FACE_TRACK_CFG_PROC_MAX_FACE,val);
}



/*-------------------------------------------------------------------------
 *  File Name : _fd_MaxFaceSet
 *------------------------------------------------------------------------*/
void 
_fd_SmileDetectSet(
	UINT32 set
)
{
	sp5kFaceTrackCfgSet(SP5K_FACE_TRACK_CFG_SMILE_DETECT,set);
}


/*-------------------------------------------------------------------------
 *  File Name : _fd_MaxFaceSet
 *------------------------------------------------------------------------*/
void 
_fd_BlinkDetectSet(
	UINT32 set
)
{
	sp5kFaceTrackCfgSet(SP5K_FACE_TRACK_CFG_BLINK_DETECT,set);
	/*1:one eyes blink as blink,2:two eyes blink as blink*/
	sp5kFaceTrackCfgSet(SP5K_FACE_TRACK_CFG_BLINK_NUM,1);

}


/*-------------------------------------------------------------------------
 *  File Name : _fd_MaxFaceSet
 *------------------------------------------------------------------------*/
void 
_fd_EnableDetectSet(
	UINT32 enable
)
{
	sp5kFaceTrackModeSet(enable);
}

/* Add by sc.liu */
/*-------------------------------------------------------------------------
 *  File Name : _od_OdSemInit
 *------------------------------------------------------------------------*/
void 
_od_OdSemInit(
	void
)
{

	if ( otrackObjectData != (UINT32)NULL ) {
		return;
	}
	otrackObjectData = ros_semaphore_create( "app_mmOd", 1 );
	if ( !otrackObjectData ) {
		printf("ERR:app_mmOd create(%s %d)\n", __FUNCTION__, __LINE__);
	}
}


/*-------------------------------------------------------------------------
 *  File Name : _od_OdSemDataFree
 *------------------------------------------------------------------------*/
void 
_od_OdSemDataFree(
	void
)
{
	if (otrackObjectData){
		ros_semaphore_put( otrackObjectData );
	}
}
/* Add by sc.liu */

/*-------------------------------------------------------------------------
 *  File Name : _fd_FdSemInit
 *------------------------------------------------------------------------*/
void 
_fd_FdSemInit(
	void
)
{

	if ( ftrackFaceData != (UINT32)NULL ) 
	{
		return;
	}
	ftrackFaceData = ros_semaphore_create( "app_mmFd", 1 );
	if ( !ftrackFaceData ) 
	{
		printf("ERR:app_mmFd create(%s %d)\n", __FUNCTION__, __LINE__);
	}
}


/*-------------------------------------------------------------------------
 *  File Name : _fd_FdSemDataFree
 *------------------------------------------------------------------------*/
void 
_fd_FdSemDataFree(
	void
)
{
	if (ftrackFaceData)
	{
		ros_semaphore_put( ftrackFaceData );
	}
}



/*-------------------------------------------------------------------------
 *  File Name : _fd_FdSemDataAlloc
 *------------------------------------------------------------------------*/
void 
_fd_FdSemDataAlloc(
	void
)
{
	if (ftrackFaceData==0)
	{
		_fd_FdSemInit();
	}
	ros_semaphore_get( ftrackFaceData, -1 );
}


/*-------------------------------------------------------------------------
 *  File Name : _fd_FdFaceDataGet
 *------------------------------------------------------------------------*/
UINT32 
_fd_FdFaceDataGet(
	appFdResult_t *fdResult
)
{	
	_fd_FdSemDataAlloc();
	memcpy(fdResult, &faceInfo, sizeof(appFdResult_t));
	_fd_FdSemDataFree();
	return SUCCESS;
}


/* Add by sc.liu */
/*-------------------------------------------------------------------------
 *  File Name : _od_OdSemDataAlloc
 *------------------------------------------------------------------------*/
void 
_od_OdSemDataAlloc(
	void
)
{
	if (otrackObjectData==0){
		_od_OdSemInit();
	}
	ros_semaphore_get( otrackObjectData, -1 );
}


 /*-------------------------------------------------------------------------
  *  File Name : _od_OdObjectDataGet
  *------------------------------------------------------------------------*/
 UINT32 
 _od_OdObjectDataGet(
	 appOdResult_t *odResult
 )
 {	 
	 _od_OdSemDataAlloc();
	 memcpy(odResult, &objectInfo, sizeof(appOdResult_t));
	 _od_OdSemDataFree();
	 return SUCCESS;
 }
/* Add by sc.liu */
/*-------------------------------------------------------------------------
 *  File Name : appFdBlinkingDetectCheck
 *------------------------------------------------------------------------*/
void appFdBlinkDetectSemInit(void)
{
	if ( BlinkDetectSem != (UINT32)NULL ) 
	{
		return;
	}
	BlinkDetectSem = ros_semaphore_create( "app_BlinkingDetect", 0 );
	if ( !BlinkDetectSem ) 
	{
		printf("ERR:AppBlinkingDetectSemInit(%s %d)\n", __FUNCTION__, __LINE__);
	}
}


/*-------------------------------------------------------------------------
 *  File Name : appFdBlinkDetectSemPut
 *------------------------------------------------------------------------*/
void appFdBlinkDetectSemPut(void)
{
	if (BlinkDetectSem)
	{
		ros_semaphore_put( BlinkDetectSem );
	}
}


/*-------------------------------------------------------------------------
 *  File Name : appFdBlinkDetectSemGet
 *------------------------------------------------------------------------*/
void appFdBlinkDetectSemGet(void)
{
	if (BlinkDetectSem==0)
	{
		appFdBlinkDetectSemInit();
	}
	ros_semaphore_get( BlinkDetectSem, -1 );
}

/*-------------------------------------------------------------------------
 *  File Name : appFdBlinkDetectSet
 *------------------------------------------------------------------------*/
void 
appFdBlinkDetectSet(
	UINT32 set
)
{
	_fd_BlinkDetectSet(set);
}



/*-------------------------------------------------------------------------
 *  File Name : appFd_FaceRectUpdateMsgSend
 *------------------------------------------------------------------------*/
void 
appFd_FaceRectUpdateMsgSend(
	void
)
{
	sp5kHostMsgSend(APP_UI_MSG_FACE_DETECT_OSD_UPDATE,0x00);
}

/* Add by sc.liu */
/*-------------------------------------------------------------------------
 *  File Name : appFd_FaceRectUpdateMsgSend
 *------------------------------------------------------------------------*/
void 
appOd_ObjectRectUpdateMsgSend(
	void
)
{
	sp5kHostMsgSend(APP_UI_MSG_OBJECT_DETECT_OSD_UPDATE,0x00);
}
/* Add by sc.liu */

/*-------------------------------------------------------------------------
 *  File Name : appFdCfgInit
 *------------------------------------------------------------------------*/

void 
appFdCfgInit(
	void
)
{
	sp5kFaceTrackCfgSet(SP5K_FACE_TRACK_CFG_KEY_FACE,1);
	sp5kFaceTrackCfgSet(SP5K_FACE_TRACK_CFG_CHECK_COLOR, 1);
	#ifdef SIMPLE_FACETRACK
	sp5kFaceTrackCfgSet(SP5K_FACE_TRACK_CFG_PROC_DET_LIST_SEL, 2);
	#endif

}


/*-------------------------------------------------------------------------
 *  File Name : appFdCfgInit
 *------------------------------------------------------------------------*/

void 
appFdCfgSet(
	appFdIniOption_e option
)
{
	appFdCfgInit();
	if (option ==FACE_DETECT_ONLY)
	{
		_fd_MaxFaceSet(MAX_SUPPORT_FACE_NUM);
		_fd_SmileDetectSet(FALSE);
		_fd_BlinkDetectSet(FALSE);
	}
	else if (option ==SMILE_DETECT)
	{
		_fd_MaxFaceSet(1);
		_fd_SmileDetectSet(TRUE);
		_fd_BlinkDetectSet(FALSE);
	}
	else if (option ==BLINK_DETECT)
	{
		_fd_MaxFaceSet(1);
		_fd_SmileDetectSet(FALSE);
		_fd_BlinkDetectSet(TRUE);
		appFdBlinkDetectSemInit();
	}
}

/* Add by sc.liu */
/*-------------------------------------------------------------------------
 *  File Name : appOdCfgInit
 *------------------------------------------------------------------------*/
SINT32 
appObdResultGet(
	UINT32 param, UINT32 *pnobj, sp5kObjTrackResult_t *presult
)
{
	return sp5kObjTrackResultGet(param, pnobj, presult);
}


/*-------------------------------------------------------------------------
 *  File Name : appOdDataUpdate
 *------------------------------------------------------------------------*/
void 
appOdDataUpdate(
	UINT32 obj, sp5kObjTrackResult_t objlist[]
)
{
	UINT32 i,j;
	sp5kGfxPageCapabDesc_t *pPageCap = NULL;

	if(!pPageCap){
		pPageCap = sp5kMalloc(sizeof(sp5kGfxPageCapabDesc_t));
	}
	if(!pPageCap){
		return;
	}
	sp5kGfxPageCapabilityGet(SP5K_GFX_PAGE_OSD_0,pPageCap);
	
	_od_OdSemDataAlloc();
	for (i=j=0 ; i<obj ; i++) {
		#if 1
 		objectInfo.rectObj[j].nStartX= objlist[i].pos.elem.x * 320 / 1024;
		objectInfo.rectObj[j].nStartY = objlist[i].pos.elem.y * 180 / 1024+30;
		objectInfo.rectObj[j].nSize= objlist[i].pos.elem.sz * 320 / 1024;

	if (!View_CoordVisible(0, objectInfo.rectObj[j].nStartY) || !View_CoordVisible(0, objectInfo.rectObj[j].nStartY+objectInfo.rectObj[j].nSize))
			continue;
		#endif
		j++;
	}
	objectInfo.totalObjectNum =j;

	_od_OdSemDataFree();
	sp5kFree(pPageCap);
}

/* Add by sc.liu */


/*-------------------------------------------------------------------------
 *  File Name : appFdCfgInit
 *------------------------------------------------------------------------*/
SINT32 
appFdResultGet(
	UINT32 param,UINT32 *pnface,sp5kFaceTrackFaceRes_t *presult
)
{
	return  sp5kFaceTrackResultGet(param,pnface,presult);
}

UINT32 View_CoordVisible(UINT32 x, UINT32 y)
{
	UINT32 w,h,y0;
	extern UINT8 gVideoSizeTag ;
	
	if(IS_VIDEO_MODE)
	{
		appVideoSizeGet(gVideoSizeTag,&w,&h);
	}
	else
	{
		appStill_GetCurSize( &w, &h);
	}
	//printf("!!!!  w =%d   h=%d  x=%d   y=%d\n",w,h ,x ,y);
	y0 = (240 - 240 * (4 * h) / (3 * w)) / 2;
	return y0<=y && y<=(240-y0);
}

/*-------------------------------------------------------------------------
 *  File Name : appFdDataUpdate
 *------------------------------------------------------------------------*/
void 
appFdDataUpdate(
	UINT32 nface, sp5kFaceTrackFaceRes_t facelist[]
)
{
	UINT32 i,j;
	_fd_FdSemDataAlloc();
	for (i=j=0 ; i<nface ; i++)
	{
		faceInfo.rectFace[j].nStartX = facelist[i].x * 320 / 1024;
		faceInfo.rectFace[j].nStartY = facelist[i].y * 180 /1024+30;
		faceInfo.rectFace[j].nSize = facelist[i].size * 320 / 1024;
		faceInfo.rectFace[j].attr = (facelist[i].attr & 0xff000000) >> 24;

		#if ICAT_FCWS && ICAT_CVR_WARN_SYS_DBG
		/* Draw car frame on video file */
		fcwsInfo.vStartX = facelist[i].x;
		fcwsInfo.vStartY = facelist[i].y;
		fcwsInfo.vSize = facelist[i].size;
		#endif

		//faceInfo.rectFace[j].angle = (facelist[i].attr & 0x3f); 
		if (!View_CoordVisible(0, faceInfo.rectFace[j].nStartY) 
			|| !View_CoordVisible(0, faceInfo.rectFace[j].nStartY+faceInfo.rectFace[j].nSize))
		{
			continue;
		}
		if ((faceInfo.rectFace[j].attr & 0x10)==FD_TYPE_MAINFACE)
		{
			faceInfo.mainFace = j;
			printf("Main Face = %d ,%d\n",i,faceInfo.rectFace[j].attr);
		}
		if ((faceInfo.rectFace[j].attr & 0x80)==FD_TYPE_SMILE)
		{
			faceInfo.rectFace[j].isSmile=1;
		}
		else
		{
			faceInfo.rectFace[j].isSmile=0;
		}
		j++;
#if 0
		printf("attr = %x,idx:%d\n", faceInfo.rectFace[i].attr,i);
		printf("nStartX = %x,j:%d\n", faceInfo.rectFace[j].nStartX,j);
		printf("nStartY = %x,j:%d\n", faceInfo.rectFace[j].nStartY,j);
		printf("nSize = %x,j:%d\n", faceInfo.rectFace[j].nSize,j);
#endif

	}
	
	faceInfo.totalFaceNum =j;
	_fd_FdSemDataFree();
}




/*-------------------------------------------------------------------------
 *  File Name : appFdCreateEx
 *------------------------------------------------------------------------*/
UINT32 
appFdCreateEx(
	appFdIniOption_e iniFlag
)
{
	appFdCfgSet(iniFlag);
	return 1;
}


/*-------------------------------------------------------------------------
 *  File Name : appFdDestroyEx
 *------------------------------------------------------------------------*/
void
appFdDestroyEx(
	void
)
{
	_fd_EnableDetectSet(FALSE);
}


/*-------------------------------------------------------------------------
 *  File Name : appFdProcStartEx
 *------------------------------------------------------------------------*/
void 
appFdProcStartEx(
	void
)
{
	_fd_EnableDetectSet(TRUE);
}



/*-------------------------------------------------------------------------
 *  File Name : appFdProcPauseEx
 *------------------------------------------------------------------------*/
void 
appFdProcPauseEx(
	void
)
{
	_fd_EnableDetectSet(FALSE);
}



/*-------------------------------------------------------------------------
 *  File Name : appFdProcStopEx
 *------------------------------------------------------------------------*/
void
appFdProcStopEx(
	void
)
{
	_fd_EnableDetectSet(FALSE);
}


/*-------------------------------------------------------------------------
 *  File Name : appFdFaceDataGetEx
 *------------------------------------------------------------------------*/
UINT32 
appFdFaceDataGetEx(

	appFdResult_t *fdResult
)
{	
	return(_fd_FdFaceDataGet(fdResult));
}

/* Add by sc.liu */
 /*-------------------------------------------------------------------------
  *  File Name : appOdObjectDataGetEx
  *------------------------------------------------------------------------*/
 UINT32 
 appOdObjectDataGetEx(
 
	 appObjectInfo_t *odResult
 )
 {	 
	#ifdef HW_FACETRACK
	 return(_od_OdObjectDataGet((appOdResult_t *)odResult));
	#else
	 if(_odBinDynamicLoad() != SUCCESS) return FAIL;
	 //return(appOdObjectDataGet((appOdResult_t *)odResult));
	 return 0;
	#endif
 }
/* Add by sc.liu */
/*-------------------------------------------------------------------------
 *  File Name : appFdBlinkingDetectCheck
 *------------------------------------------------------------------------*/
UINT32 
appFdBlinkingDetectCheck(
	UINT32 yuvAddr,
	UINT32 qvgaAddr,
	void * psSizeInfo
)
{
	#define W 640
	#define H 480

	sp5kImgSize_t *pYuvSize=(sp5kImgSize_t *)psSizeInfo;
	UINT32 srcAddr,width,height, qvw, qvh;;
	sp5kFaceTrackYuvBuf_t bufInfo;
	UINT32 ret =SUCCESS;

	srcAddr = yuvAddr;
	width = pYuvSize->cropWidth;
	height = pYuvSize->cropHeight;

	appFdBlinkDetectSet(TRUE);

	/* 
		assume only width is always large than height.
		blink need multiple of 8 pixle in w and h.
	*/
 	qvw = (UINT32)(W+4)&0xfffffff8;
 	qvh = (UINT32)(W*height/width+4)&0xfffffff8;	
	BlKChkyuvBuf = sp5kYuvBufferAlloc(qvw,qvh);
	if (BlKChkyuvBuf == NULL) 
	{
		appFdBlinkDetectSet(FALSE);
		printf("qv memory allocate error!\n");
		return FAIL;
	}

	bufInfo.yuvaddr = BlKChkyuvBuf;
	bufInfo.yuvw = qvw;
	bufInfo.yuvh = qvh;
	bufInfo.dispx = 0;
	bufInfo.dispy = 0;
	bufInfo.dispw =0;
	bufInfo.disph = 0;

	appyuvScale((UINT8 *)srcAddr, width, height, BlKChkyuvBuf, qvw, qvh);
	if (sp5kFaceTrackYuvBufStart(0, &bufInfo, 0, 0) == SUCCESS) 
	{
		
	}
	else
	{
		ret =FAIL;
		printf("\n@@Blink sp5kFaceTrackYuvBufStart Fail\n");
	}
	return ret;
}


/*-------------------------------------------------------------------------
 *  File Name : appFdBlinkDetectCheckEnd
 *------------------------------------------------------------------------*/
void 
appFdBlinkDetectCheckEnd(
	void
)
{
	if (BlKChkyuvBuf!=NULL)
	{
		sp5kYuvBufferFree(BlKChkyuvBuf);
		BlKChkyuvBuf = NULL;
	}
}


