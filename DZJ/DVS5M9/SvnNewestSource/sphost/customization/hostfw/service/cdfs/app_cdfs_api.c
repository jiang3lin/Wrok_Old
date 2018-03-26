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
#include "app_com_def.h"
#include "app_com_api.h"
#include "app_dbg_api.h"
#include "app_cdfs_api.h"
#include "sp5k_dcf_api.h"
#include "sp5k_fs_api.h"
#include "sp5k_msg_def.h"
#include "sp5k_global_api.h"
#include "gpio_custom.h"
#include "app_video.h"
#include "app_state_ctrl.h"
#include "app_osdfont.h"
#include "app_osd_api.h"
#include "app_icon_def.h"
#include "app_pal.h"
#include "app_user_setting.h"
#include "sp5k_rsvblk_api.h"
#include "app_menu_tab.h"
#include "app_errorosd.h"
#include "Sp5k_disk_api.h"
#include "app_ui_para.h"

#include "sp5k_media_api.h"

#include "app_scdv_ui_para_def.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define NEW_FLOW_DBG 1

#if NEW_FLOW_DBG
#define CDFS_DBG(fmt, args...)	printf(fmt, ##args) 
#else
#define CDFS_DBG(fmt, args...)
#endif

#define HOST_DEBUG_NORMAL		1
#define HOST_DEBUG_IMPORTANT	1

#define CDFS_POOL_MAX	100
#define CDFS_POOL_UNIT	500
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
 
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
static CDFS_TAB_T *CDFS_TAB_VIDEO = NULL;
static CDFS_TAB_T *CDFS_TAB_JPG = NULL;
static CDFS_TAB_T *CDFS_TAB_EVENT = NULL;
static CDFS_TAB_T *CDFS_TAB_AUDIO = NULL;

static CDFS_TAB_T *CDFS_FREE_VIDEO = NULL;
static CDFS_TAB_T *CDFS_FREE_JPG = NULL;
static CDFS_TAB_T *CDFS_FREE_EVENT = NULL;
static CDFS_TAB_T *CDFS_FREE_AUDIO = NULL;

static CDFS_TAB_T *CDFS_LAST_VIDEO = NULL;
static CDFS_TAB_T *CDFS_LAST_JPG = NULL;
static CDFS_TAB_T *CDFS_LAST_EVENT = NULL;
static CDFS_TAB_T *CDFS_LAST_AUDIO = NULL;

/* the address of each pool */
static CDFS_TAB_T * CDFS_VIDEO_POOL[CDFS_POOL_MAX]={NULL};
static CDFS_TAB_T * CDFS_JPG_POOL[CDFS_POOL_MAX]={NULL};
static CDFS_TAB_T * CDFS_EVENT_POOL[CDFS_POOL_MAX]={NULL};
static CDFS_TAB_T * CDFS_AUDIO_POOL[CDFS_POOL_MAX]={NULL};

static UINT32 CDFS_VIDEO_CUR_POOL=0;
static UINT32 CDFS_JPG_CUR_POOL=0;
static UINT32 CDFS_EVENT_CUR_POOL=0;
static UINT32 CDFS_AUDIO_CUR_POOL=0;

static UINT32 CDFS_NUM_VIDEO;
static UINT32 CDFS_NUM_JPG;
static UINT32 CDFS_NUM_EVENT;
static UINT32 CDFS_NUM_AUDIO;

static UINT32 CDFS_FIDX_VIDEO;
static UINT32 CDFS_FIDX_JPG;
static UINT32 CDFS_FIDX_EVENT;
static UINT32 CDFS_FIDX_AUDIO;

static UINT32 CDFS_FILE_FOLDER;

static UINT32 CDFS_EVENT_OCCUR;
static UINT64 CDFS_VIDEO_SIZE;/*unit: byte*/
static UINT32 CDFS_DEL_VIDEO_METHOD = CDFS_PROTECTED_SKIP;
static UINT8 CDFS_LAST_FILE_NAME[CDFS_FILE_NAME_LENGTH];
static UINT8 CDFS_PRE_FILE_NAME[CDFS_FILE_NAME_LENGTH];
static UINT8 CDFS_LAST_AVI_NAME[CDFS_FILE_NAME_LENGTH];
static UINT8 CDFS_CAP_NUM;
static UINT8 CDFS_ADD_FILE;
static UINT8 headSem;

UINT8 eventLockFileName[CDFS_FILE_NAME_LENGTH];
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern uiParamSetting_t *pUiSetting;
extern UINT8 alreadyRecFileFlag;
extern UINT8 gsensorRecEventFlag;
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
/*CDFS stand for:Customer Defined File System*/
void appCdfsUnitInfoClear(CDFS_TAB_T *pObj)
{
	pObj->attr = 0;
	pObj->dcfattr = 0;
	memset(pObj->FileName,0,sizeof(pObj->FileName));
	pObj->FileSize=0;
	pObj->FileType=0;
}

void appCdfsFilePoolInit(void)
{
	appCdfsPoolReset(CDFS_FILE_FOLDER_VIDEO);
	appCdfsPoolReset(CDFS_FILE_FOLDER_JPG);
	appCdfsPoolReset(CDFS_FILE_FOLDER_EVENT);
	appCdfsPoolReset(CDFS_FILE_FOLDER_AUDIO);
}

void appCdfsPoolReset(UINT32 folder)
{
	UINT32 i;

	switch(folder){
		case CDFS_FILE_FOLDER_VIDEO:
			for(i=0;i<CDFS_POOL_MAX;i++){
				if(CDFS_VIDEO_POOL[i]){
					sp5kFree((void *)CDFS_VIDEO_POOL[i]);
					CDFS_VIDEO_POOL[i] = NULL;
				}
			}
			CDFS_VIDEO_CUR_POOL = 0;
			CDFS_FREE_VIDEO = appCdfsFileNewPoolReg(CDFS_FILE_FOLDER_VIDEO);
			CDFS_TAB_VIDEO = NULL;
			CDFS_LAST_VIDEO = NULL;
			CDFS_VIDEO_SIZE = 0;
			CDFS_NUM_VIDEO = 0;
			break;
		case CDFS_FILE_FOLDER_JPG:
			for(i=0;i<CDFS_POOL_MAX;i++){
				if(CDFS_JPG_POOL[i]){
					sp5kFree((void *)CDFS_JPG_POOL[i]);
					CDFS_JPG_POOL[i] = NULL;
				}
			}
			CDFS_JPG_CUR_POOL = 0;
			CDFS_FREE_JPG = appCdfsFileNewPoolReg(CDFS_FILE_FOLDER_JPG);
			CDFS_TAB_JPG = NULL;
			CDFS_LAST_JPG = NULL;
			CDFS_NUM_JPG = 0;
			break;
		case CDFS_FILE_FOLDER_EVENT:
			for(i=0;i<CDFS_POOL_MAX;i++){
				if(CDFS_EVENT_POOL[i]){
					sp5kFree((void *)CDFS_EVENT_POOL[i]);
					CDFS_EVENT_POOL[i] = NULL;
				}
			}
			CDFS_EVENT_CUR_POOL = 0;
			CDFS_FREE_EVENT = appCdfsFileNewPoolReg(CDFS_FILE_FOLDER_EVENT);
			CDFS_TAB_EVENT = NULL;
			CDFS_LAST_EVENT = NULL;
			CDFS_NUM_EVENT = 0;
			break;
		case CDFS_FILE_FOLDER_AUDIO:
			for(i=0;i<CDFS_POOL_MAX;i++){
				if(CDFS_AUDIO_POOL[i]){
					sp5kFree((void *)CDFS_AUDIO_POOL[i]);
					CDFS_AUDIO_POOL[i] = NULL;
				}
			}
			CDFS_AUDIO_CUR_POOL = 0;
			CDFS_FREE_AUDIO = appCdfsFileNewPoolReg(CDFS_FILE_FOLDER_AUDIO);
			CDFS_TAB_AUDIO = NULL;
			CDFS_LAST_AUDIO = NULL;
			CDFS_NUM_AUDIO = 0;
			break;
	}

}

CDFS_TAB_T * appCdfsFileNewPoolReg(UINT32 folder)
{
	UINT32 i;
	CDFS_TAB_T *tmpA=NULL;
	CDFS_TAB_T *tmpB=NULL;
	CDFS_TAB_T * tempAddr = sp5kMalloc(sizeof(CDFS_TAB_T)*CDFS_POOL_UNIT);

	if(tempAddr == NULL){
		return NULL;
	}

	memset(tempAddr,0,sizeof(sizeof(CDFS_TAB_T)*CDFS_POOL_UNIT));
	tmpA = tempAddr;
	tmpB = tempAddr+1;

	for(i=0;i<CDFS_POOL_UNIT-1;i++){
		tmpA->next = tmpB;
		tmpB->pre = tmpA;
		tmpA++;
		tmpB++;
		appCdfsUnitInfoClear(tmpA);
	}
	tmpA->next = NULL;
	
	switch(folder){
		case CDFS_FILE_FOLDER_VIDEO:
			CDFS_DBG("CDFS_VIDEO_CUR_POOL:%d\n",CDFS_VIDEO_CUR_POOL);
			if(CDFS_VIDEO_CUR_POOL>=CDFS_POOL_MAX){
				sp5kFree(tempAddr);
				return NULL;
			}
			CDFS_VIDEO_POOL[CDFS_VIDEO_CUR_POOL] = tempAddr;
			CDFS_VIDEO_CUR_POOL++;
			break;
		case CDFS_FILE_FOLDER_JPG:
			CDFS_DBG("CDFS_JPG_CUR_POOL:%d\n",CDFS_JPG_CUR_POOL);
			if(CDFS_JPG_CUR_POOL>=CDFS_POOL_MAX){
				sp5kFree(tempAddr);
				return NULL;
			}
			CDFS_JPG_POOL[CDFS_JPG_CUR_POOL] = tempAddr;
			CDFS_JPG_CUR_POOL++;
			break;
		case CDFS_FILE_FOLDER_EVENT:
			CDFS_DBG("CDFS_EVENT_CUR_POOL:%d\n",CDFS_EVENT_CUR_POOL);
			if(CDFS_EVENT_CUR_POOL>=CDFS_POOL_MAX){
				sp5kFree(tempAddr);
				return NULL;
			}
			CDFS_EVENT_POOL[CDFS_EVENT_CUR_POOL] = tempAddr;
			CDFS_EVENT_CUR_POOL++;
			break;
		case CDFS_FILE_FOLDER_AUDIO:
			CDFS_DBG("CDFS_AUDIO_CUR_POOL:%d\n",CDFS_AUDIO_CUR_POOL);
			if(CDFS_AUDIO_CUR_POOL>=CDFS_POOL_MAX){
				sp5kFree(tempAddr);
				return NULL;
			}
			CDFS_AUDIO_POOL[CDFS_AUDIO_CUR_POOL] = tempAddr;
			CDFS_AUDIO_CUR_POOL++;
			break;
		default:
			break;
	}

	return tempAddr;
}

void appCdfsPoolInfoDump(UINT32 folder)
{
	UINT32 num=0;
	UINT32 i,j;
	CDFS_TAB_T *pObj=NULL;
	
	if(folder == CDFS_FILE_FOLDER_VIDEO)
	{
		CDFS_DBG("CDFS_VIDEO_CUR_POOL:%d\n",CDFS_VIDEO_CUR_POOL);

		for(i=0;i<CDFS_VIDEO_CUR_POOL;i++){
			pObj = CDFS_VIDEO_POOL[i];
			for(j=0;j<CDFS_POOL_UNIT;j++){
				CDFS_DBG("POOL[%d][%d]:0x%p,%s\n",i,j,pObj,pObj->FileName);
				pObj++;
			}
			CDFS_DBG("\n");		
		}

		CDFS_DBG("Video Free List:\n");
		pObj = CDFS_FREE_VIDEO;
		num = 0;
		while(pObj){
			CDFS_DBG("0x%p-->Free[%d]\n",pObj,num);
			pObj = pObj->next;
			num++;
		}

		CDFS_DBG("\nVideo Used List:\n");
		num=0;
		pObj = CDFS_TAB_VIDEO;
		while(pObj){
			CDFS_DBG("0x%p-->Used[%d]:%s\n",pObj,num,pObj->FileName);
			pObj = pObj->next;
			num++;
		}
	}
	else if(folder == CDFS_FILE_FOLDER_JPG)
	{
		CDFS_DBG("CDFS_JPG_CUR_POOL:%d\n",CDFS_JPG_CUR_POOL);

		for(i=0;i<CDFS_JPG_CUR_POOL;i++){
			pObj = CDFS_JPG_POOL[i];
			for(j=0;j<CDFS_POOL_UNIT;j++){
				CDFS_DBG("POOL[%d][%d]:0x%p,%s\n",i,j,pObj,pObj->FileName);
				pObj++;
			}
			CDFS_DBG("\n");		
		}

		CDFS_DBG("JPG Free List:\n");
		pObj = CDFS_FREE_JPG;
		num = 0;
		while(pObj){
			CDFS_DBG("0x%p-->Free[%d]\n",pObj,num);
			pObj = pObj->next;
			num++;
		}

		CDFS_DBG("\nJPG Used List:\n");
		num=0;
		pObj = CDFS_TAB_JPG;
		while(pObj){
			CDFS_DBG("0x%p-->Used[%d]:%s\n",pObj,num,pObj->FileName);
			pObj = pObj->next;
			num++;
		}
	}
	else if(folder == CDFS_FILE_FOLDER_EVENT)
	{
		CDFS_DBG("CDFS_EVENT_CUR_POOL:%d\n",CDFS_EVENT_CUR_POOL);

		for(i=0;i<CDFS_EVENT_CUR_POOL;i++){
			pObj = CDFS_EVENT_POOL[i];
			for(j=0;j<CDFS_POOL_UNIT;j++){
				CDFS_DBG("POOL[%d][%d]:0x%p,%s\n",i,j,pObj,pObj->FileName);
				pObj++;
			}
			CDFS_DBG("\n");		
		}

		CDFS_DBG("EVENT Free List:\n");
		pObj = CDFS_FREE_EVENT;
		num = 0;
		while(pObj){
			CDFS_DBG("0x%p-->Free[%d]\n",pObj,num);
			pObj = pObj->next;
			num++;
		}

		CDFS_DBG("\nEVENT Used List:\n");
		num=0;
		pObj = CDFS_TAB_EVENT;
		while(pObj){
			CDFS_DBG("0x%p-->Used[%d]:%s\n",pObj,num,pObj->FileName);
			pObj = pObj->next;
			num++;
		}
	}
	else if(folder == CDFS_FILE_FOLDER_AUDIO)
	{
		CDFS_DBG("CDFS_AUDIO_CUR_POOL:%d\n",CDFS_AUDIO_CUR_POOL);

		for(i=0;i<CDFS_AUDIO_CUR_POOL;i++){
			pObj = CDFS_AUDIO_POOL[i];
			for(j=0;j<CDFS_POOL_UNIT;j++){
				CDFS_DBG("POOL[%d][%d]:0x%p,%s\n",i,j,pObj,pObj->FileName);
				pObj++;
			}
			CDFS_DBG("\n");		
		}

		CDFS_DBG("AUDIO Free List:\n");
		pObj = CDFS_FREE_AUDIO;
		num = 0;
		while(pObj){
			CDFS_DBG("0x%p-->Free[%d]\n",pObj,num);
			pObj = pObj->next;
			num++;
		}

		CDFS_DBG("\nAUDIO Used List:\n");
		num=0;
		pObj = CDFS_TAB_AUDIO;
		while(pObj){
			CDFS_DBG("0x%p-->Used[%d]:%s\n",pObj,num,pObj->FileName);
			pObj = pObj->next;
			num++;
		}
	}
}

UINT32 appCdfsFolder2Path(UINT32 folder,UINT8 *path)
{
	UINT8 driver[CDFS_FILE_NAME_LENGTH]={0};
	UINT8 folderName[CDFS_FILE_NAME_LENGTH]={0};
	if(IS_CARD_EXIST)
	{
		uiPara_t* puiPara = appUiParaGet();
		
		switch (puiPara->ScdvMainMode)
		{
			case UI_SCDV_MAIN_MODE_SPORT:
				sprintf(driver, "D:/%s/", CDFS_ROOT_NAME_SDV);
				break;


				
			case UI_SCDV_MAIN_MODE_CAR:
				sprintf(driver, "D:/%s/", CDFS_ROOT_NAME_CDV);
				break;
			default:
				strcpy(driver,"D:/");
				break;
		}
	}
	else
	{
		strcpy(driver,"C:/");
	}

	switch(folder)
	{
		case CDFS_FILE_FOLDER_VIDEO:
			strcpy(folderName,"VIDEO");
			break;
		case CDFS_FILE_FOLDER_JPG:
			strcpy(folderName,"JPG");
			break;
		case CDFS_FILE_FOLDER_EVENT:
			strcpy(folderName,"EVENT");
			break;
		case CDFS_FILE_FOLDER_AUDIO:
			strcpy(folderName,"AUDIO");
			break;
		default:
			break;
	}
	sprintf(path,"%s%s",driver,folderName);
	return SUCCESS;
}

UINT8* appCdfsFolderRootGet(void)
{
	uiPara_t* puiPara = appUiParaGet();
	
	switch (puiPara->ScdvMainMode)
	{
		case UI_SCDV_MAIN_MODE_SPORT:
			return CDFS_ROOT_NAME_SDV;
			break;
		case UI_SCDV_MAIN_MODE_CAR:
			return CDFS_ROOT_NAME_CDV;
			break;
		default:
			return NULL;
			break;
	}
}

UINT32  appCdfsFolderInit(UINT32 folder)/*OK*/
{
	#if 1	//for dvs5m2 support sub folder

	UINT32 ret = SUCCESS;
	UINT8 path[CDFS_FILE_NAME_LENGTH], beforePath[CDFS_FILE_NAME_LENGTH];
	UINT8* afterPath;
	appCdfsFolder2Path(folder,path);
	if(sp5kFsDirChange(path)==FAIL)
	{
		afterPath = strchr(path, '/');
		afterPath = strchr(afterPath + 1, '/');
		while(afterPath != NULL)
		{
			strncpy(beforePath, path, afterPath - path);
			beforePath[afterPath - path] = '\0';
			sp5kFsDirMake(beforePath);
		
			afterPath = strchr(afterPath + 1, '/');
		}
	
		sp5kFsDirMake(path);
		ret = sp5kFsDirChange(path);
	}
	return ret;

	#else
	
	UINT32 ret = SUCCESS;
	UINT8 path[20];
	appCdfsFolder2Path(folder,path);
	if(sp5kFsDirChange(path)==FAIL)
	{
		sp5kFsDirMake(path);
		ret = sp5kFsDirChange(path);
	}
	return ret;
	
	#endif
}

UINT32 appCdfsAddFileStsGet(void)
{
	return CDFS_ADD_FILE;
}
UINT32 appCdvAddFileStsGet(void)
{
	return CDFS_ADD_FILE=1;
}
UINT32 appCdfsNextFileNameGet(UINT32 filetype,UINT8 *pFileName)/*OK*/
{
	tmx_t time;
	UINT8 fname[CDFS_FILE_NAME_LENGTH]={0};
	#if 1
	UINT8 checkName[CDFS_FILE_NAME_LENGTH]={0};
	#else
	static UINT8 lastName[64]={0};
	UINT8 attachName[4]={0};
	static UINT32 sameNameNum = 0;
	#endif
	UINT32 ret = SP5K_ERR_SUCCESS;

	if (pFileName == NULL)
	{
		return SP5K_ERR_DCF_PARAM_GROUP;
	}

	sp5kRtcDateTimeGet( SP5K_DATE_TIME_OPTION, &time);
    
	sprintf(fname,"%s/%04d%02d%02d_%02d%02d%02d",
						sp5kFsGetCurPath(),
						time.tmx_year+1900,
						time.tmx_mon,
						time.tmx_mday,
						time.tmx_hour,
						time.tmx_min,
						time.tmx_sec
	);
	/*jj add for burst snap, resolve same name file is covered by new file*/
	#if 1	
	appCdfsFileNameExistCheck(fname);/**/
	#else
	if(SP5K_DCF_FILETYPE_JPG == filetype)
	{
		if(strcmp(lastName,fname)==0)
		{
			sameNameNum++;
			sprintf(attachName,"_%02d",sameNameNum);
			strcat(fname, attachName);
		}
		else
		{
			strcpy(lastName,fname);
			sameNameNum = 0;
		}	
	}
	#endif
	/*jj add for same name file*/
	switch(filetype)
	{
		case SP5K_DCF_FILETYPE_JPG:
			strcat(fname, ".JPG");
			break;
		case SP5K_DCF_FILETYPE_AVI:
			strcat(fname, ".AVI");
			break;
		case SP5K_DCF_FILETYPE_MOV:
			strcat(fname, ".MOV");
			break;
		case SP5K_DCF_FILETYPE_WAV:
			strcat(fname, ".WAV");
			break;
		default:
			ret = SP5K_ERR_DCF_PARAM_GROUP;
			__FUNC_TRACK__;
			RPrintf("can't Get file name!\n");
			break;
	}
	/*dw++@20120519. need copy path to des buffer.*/
	strcpy(pFileName, fname);
	strcpy(CDFS_LAST_FILE_NAME,fname);

	if(gsensorRecEventFlag == 1)
	{
		/* G-sensor trigger rec in last 1/3, protect next video file */
		gsensorRecEventFlag = 0;
		appCdfsEventFlagSet(VIDEO_EVENT_IN_VIDEO);
	}
	
       return ret;
}

UINT8 * appCdfsPreFileNameGet(void)
{
	return CDFS_PRE_FILE_NAME;
}

UINT8 * appCdfsLastFileNameGet(void)
{
	return CDFS_LAST_FILE_NAME;
}
UINT8 * appCdfsLastAviNameGet(void)
{
	return CDFS_LAST_AVI_NAME;
}

UINT32 appCdfsAbsolutePathToFile(UINT8 *filename)
{
	UINT8 name[CDFS_FILE_NAME_LENGTH]={0};
	UINT8 *pchar=NULL;
	strcpy(name,filename);
	pchar = name;

	if(name[1] == ':')
	{
		while(1)
		{
			if((*pchar>='0')&&(*pchar<='9'))
			{
				strcpy(filename,pchar);
				break;
			}
			pchar++;
		}
	}
	return SUCCESS;
}


UINT32 appCdfsEventProcess(UINT8 *filename,UINT32 event)
{
	UINT8 src[CDFS_FILE_NAME_LENGTH]={0};
	UINT8 dst[CDFS_FILE_NAME_LENGTH]={0};
	
	appCdfsFolder2Path(CDFS_FILE_FOLDER_VIDEO,src);
	if(event == JPG_EVENT_IN_VIDEO)
	{
		appCdfsFolder2Path(CDFS_FILE_FOLDER_JPG,dst);
		
	}
	else if(event == VIDEO_EVENT_IN_VIDEO)
	{
		appCdfsFolder2Path(CDFS_FILE_FOLDER_EVENT,dst);
	}
	sprintf(src,"%s/%s",src,filename);
	sprintf(dst,"%s/%s",dst,filename);
	sp5kFsRename(src,dst);
	return SUCCESS;
}


UINT32 appCdfsFileNameToFileType(UINT8 *filename)
{
	#if 1
	
	UINT8 temp[CDFS_FILE_NAME_LENGTH];

	strcpy(temp, filename);
	strupr(temp);

	if(strcmp(temp + strlen(temp) - strlen(".AVI"), ".AVI") == 0)
	{
		return SP5K_DCF_FILETYPE_AVI;
	}
	else if(strcmp(temp + strlen(temp) - strlen(".JPG"), ".JPG") == 0)
	{
		return SP5K_DCF_FILETYPE_JPG;
	}
	else if(strcmp(temp + strlen(temp) - strlen(".WAV"), ".WAV") == 0)
	{
		return SP5K_DCF_FILETYPE_WAV;
	}
	else if(strcmp(temp + strlen(temp) - strlen(".MOV"), ".MOV") == 0)
	{
		return SP5K_DCF_FILETYPE_MOV;
	}
	else
	{
		__FUNC_TRACK__;
		return SP5K_DCF_FILETYPE_AVI;
	}
	
	#else
	
	if((*(filename+16) == 'A')||(*(filename+16) == 'a')||(*(filename+19) == 'A')||(*(filename+19) == 'a'))
	{
		return SP5K_DCF_FILETYPE_AVI;
	}
	else if((*(filename+16) == 'J')||(*(filename+16) == 'j')||(*(filename+19) == 'J')||(*(filename+19) == 'j'))
	{
		return SP5K_DCF_FILETYPE_JPG;
	}
	else if((*(filename+16) == 'W')||(*(filename+16) == 'w')||(*(filename+19) == 'W')||(*(filename+19) == 'w'))
	{
		return SP5K_DCF_FILETYPE_WAV;
	}
	else if((*(filename+16) == 'M')||(*(filename+16) == 'm')||(*(filename+19) == 'M')||(*(filename+19) == 'm'))
	{
		return SP5K_DCF_FILETYPE_MOV;
	}
	else
	{
		__FUNC_TRACK__;
	}
	return SP5K_DCF_FILETYPE_AVI;
	
	#endif
}

UINT8 appCdfsCapNumIncrease(void)
{
	CDFS_CAP_NUM++;
	return CDFS_CAP_NUM;
}
UINT8 appCdfsCapNumSet(UINT8 value)
{
	CDFS_CAP_NUM = value;
	return CDFS_CAP_NUM;
}

UINT32 appCdfsAddFile2List(UINT8 *filename,UINT32 filenamelen,UINT32 filesize,UINT32 fileattr)
{
	CDFS_TAB_T *pNew=NULL;
	CDFS_TAB_T *pCur=NULL;
	CDFS_TAB_T *pPre=NULL;
	CDFS_TAB_T *pHead=NULL;
	CDFS_TAB_T *pFree=NULL;
	CDFS_TAB_T *pLast=NULL;
	UINT32 fileCount=0;
	UINT32 fd=0;
	UINT32 attr=0;
	/*UINT32 fIdx=1;*/
	/*UINT32 num=0;*/
	UINT32 filetype=SP5K_DCF_FILETYPE_AVI;
	UINT8 dirFileName[CDFS_FILE_NAME_LENGTH]={0};

	if (filename == NULL){
		return SP5K_ERR_DCF_PARAM_GROUP;
	}

	//HLPrintf1("cdfs file add:%s\n",filename);
	strcpy(CDFS_PRE_FILE_NAME,filename);
	while(headSem)
	{
		appTimeDelay(20,1);
		RPrintf("waiting for sem in appCdfsFileAdd\n");
	}
	headSem = 1;
	strcpy(dirFileName,filename);
	if(!filename)
	{
		headSem = 0;
		return SP5K_ERR_DCF_PARAM_GROUP;
	}
	
	appCdfsAbsolutePathToFile(filename);

	if((appCdfsEventFlagGet()&JPG_EVENT_IN_VIDEO)
		&&((appCdfsFileNameToFileType(filename)&SP5K_DCF_FILETYPE_AVI)
		||(appCdfsFileNameToFileType(filename)&SP5K_DCF_FILETYPE_MOV)))
	{
		CDFS_ADD_FILE=1;
		strcpy(CDFS_LAST_AVI_NAME,dirFileName);
		headSem = 0;
		return SP5K_ERR_SUCCESS;/*avoid adding video file when adding jpg*/
	}
	
	if((appCdfsEventFlagGet()&VIDEO_EVENT_IN_VIDEO)
		&&((appCdfsFileNameToFileType(filename)&SP5K_DCF_FILETYPE_AVI)
		||(appCdfsFileNameToFileType(filename)&SP5K_DCF_FILETYPE_MOV)))
	{
		CDFS_FILE_FOLDER = CDFS_FILE_FOLDER_EVENT;
		
	}
	else if((appCdfsEventFlagGet()&JPG_EVENT_IN_VIDEO)
		&&(appCdfsFileNameToFileType(filename)&SP5K_DCF_FILETYPE_JPG))
	{
		CDFS_FILE_FOLDER = CDFS_FILE_FOLDER_JPG;
	}


	if(appCdfsFileConfirm(filename,CDFS_FILE_FOLDER) == FAIL)
	{
		headSem = 0;
		return SP5K_ERR_SUCCESS;/*file is not file we need*/
	}

	
	if(CDFS_FILE_FOLDER == CDFS_FILE_FOLDER_VIDEO)
	{
		pHead = CDFS_TAB_VIDEO;/*Get current tab header*/
		pFree = CDFS_FREE_VIDEO;
		pLast = CDFS_LAST_VIDEO;
		fileCount = CDFS_NUM_VIDEO;
		filetype = appCdfsFileNameToFileType(filename);
	}
	else if(CDFS_FILE_FOLDER == CDFS_FILE_FOLDER_JPG)
	{
		pHead = CDFS_TAB_JPG;
		pFree = CDFS_FREE_JPG;
		pLast = CDFS_LAST_JPG;
		fileCount = CDFS_NUM_JPG;
		filetype = SP5K_DCF_FILETYPE_JPG;
	}
	else if(CDFS_FILE_FOLDER == CDFS_FILE_FOLDER_EVENT)
	{
		pHead = CDFS_TAB_EVENT;
		pFree = CDFS_FREE_EVENT;
		pLast = CDFS_LAST_EVENT;
		fileCount = CDFS_NUM_EVENT;
		filetype = appCdfsFileNameToFileType(filename);
	}
	else if(CDFS_FILE_FOLDER == CDFS_FILE_FOLDER_AUDIO)
	{
		pHead = CDFS_TAB_AUDIO;
		pFree = CDFS_FREE_AUDIO;
		pLast = CDFS_LAST_AUDIO;
		fileCount = CDFS_NUM_AUDIO;
		filetype = SP5K_DCF_FILETYPE_WAV;
	}

	#if 1
	if(pFree == NULL){
		CDFS_DBG("Alloc new Pool\n");
		pFree = appCdfsFileNewPoolReg(CDFS_FILE_FOLDER);
		DBG_ASSERT(pFree);
	}
	
	pNew = pFree;
	//CDFS_DBG("pFree:0x%p,next:0x%p\n",pFree,pFree->next);
	#else
	pNew=sp5kMalloc(sizeof(CDFS_TAB_T));
	memset(pNew,0,sizeof(CDFS_TAB_T));
	DBG_ASSERT(pNew);
	#endif
	/****************************************/
	strncpy(pNew->FileName,filename,sizeof(pNew->FileName));
	pNew->FileSize=filesize;
	
	pNew->attr = fileattr;
	if(fileattr & SP5K_FS_ATTR_READONLY)
	{
		pNew->dcfattr |= SP5K_DCF_FILE_ATTR_LOCK;
	}
	else
	{
		pNew->dcfattr = 0;
	}
		
	/*Del the file avoid showing "file error"*/
	//if((pNew->FileSize <= (512L<<10))&&(filetype != SP5K_DCF_FILETYPE_JPG))
	if(pNew->FileSize == 0)
	{
		//sp5kDcfFsFileOperate(SP5K_DCF_FILEOP_RECOVER_FAT,(UINT32)pNew->FileName,0,NULL,0);

		if(CDFS_FILE_FOLDER == CDFS_FILE_FOLDER_VIDEO ){
			 sp5kDcfRecoverFileList_t *pList;
			 pList=sp5kMalloc(sizeof(sp5kDcfRecoverFileList_t));
			 memset(pList,0,sizeof(sp5kDcfRecoverFileList_t));
			   
			 if(appCdfsFileNameToFileType(pNew->FileName)&SP5K_DCF_FILETYPE_MOV)
			   pList->fileType = SP5K_DCF_FILETYPE_MOV ;
			 else
			   pList->fileType = SP5K_DCF_FILETYPE_AVI ;
			 
			 strcpy(pList->pathName , pNew->FileName) ;
			 pList->pnext = NULL ;
			 sp5kDcfFsFileOperate(SP5K_DCF_FILEOP_RECOVER_FILE, (UINT32)pList, 0, 0, 0); 
			 sp5kFree(pList);
		}else{		
			sp5kFsFileDelete(pNew->FileName);
			//sp5kFree(pNew);
			HLPrintf1("file %s is NULL\n", pNew->FileName);
			headSem = 0;
			return SP5K_ERR_SUCCESS;
		}
	}
	
	pNew->FileType = filetype;
	/****************************************/
	fileCount++;

	if(CDFS_FILE_FOLDER == CDFS_FILE_FOLDER_VIDEO)
	{
		if(CDFS_DEL_VIDEO_METHOD == CDFS_PROTECTED_SKIP)
		{
			if(!(pNew->dcfattr & SP5K_DCF_FILE_ATTR_LOCK))
			{
				CDFS_VIDEO_SIZE+=(UINT64)pNew->FileSize;
			}
		}
		else
		{
			CDFS_VIDEO_SIZE+=pNew->FileSize;
		}
		/*__FUNC_TRACK__;*/
		/*printf("size = %u(B),%u(MB)\n",(UINT32)CDFS_VIDEO_SIZE,((UINT32)CDFS_VIDEO_SIZE)>>20);*/
	}
	/*****************************************/
	if(pHead == NULL)/*create first object of table*/
	{
		/*add file@head*/
		pFree = pFree->next;
		if(pFree){
			pFree->pre = NULL;
			appCdfsUnitInfoClear(pFree);
		}
		pNew->next = NULL;
		pNew->pre = NULL;
		pHead = pNew;
		pLast = pNew;
	}
	else
	{
		#if 1
		pCur = pLast;

		while(1){
			if(strcmp(pNew->FileName,pCur->FileName) >0)/*sort by date*/
			{
				/*add file@object added is newest*/
				pFree = pFree->next;
				if(pFree){
					pFree->pre = NULL;
					appCdfsUnitInfoClear(pFree);
				}
				pNew->pre = pCur;
				pNew->next = pCur->next;
				pCur->next=pNew;
				while(1){
					if(pCur->next){
						pCur = pCur->next;
					}else{
						pLast = pCur;
						break;
					}
				}
				
				break;
			}
			else if(strcmp(pNew->FileName,pCur->FileName) == 0)/*the same name*/
			{/*the new file replace the old file when two file name is same*/
				pCur->FileSize = pNew->FileSize;
				pCur->attr = pNew->attr;
				pCur->dcfattr = pNew->dcfattr;
				fileCount--;
				break;
			}
			else
			{
				if(pCur->pre){
					pPre= pCur;
					pCur=pCur->pre;
				}else{
					pFree = pFree->next;
					if(pFree){
						pFree->pre = NULL;
						appCdfsUnitInfoClear(pFree);
					}
					pCur->pre = pNew;
					pNew->next = pCur;
					pHead = pNew;
					break;
				}
			}
		}
		#else
		pPre=pCur=pHead;
		while(1)
		{
			if(strcmp(pNew->FileName,pCur->FileName) >0)/*sort by date*/
			{
				/*fIdx++;*/
				if(pCur->next)
				{
					pPre=pCur;
					pCur=pCur->next;
				}
				else
				{/*add file@object added is newest*/
					pCur->next=pNew;
					pNew->next=NULL;
					pNew->pre = pCur;
					break;
				}
			}
			else if(strcmp(pNew->FileName,pCur->FileName) == 0)/*the same name*/
			{/*the new file replace the old file when two file name is same*/
				pCur->FileSize = pNew->FileSize;
				pCur->attr = pNew->attr;
				pCur->dcfattr = pNew->dcfattr;
				fileCount--;
				break;
			}
			else
			{/*add file@insert head*/
				if(pCur==pHead)
				{	pNew->pre = NULL;
					pNew->next = pHead;
					pHead->pre = pNew;
					pHead = pNew;
				}
				else/*add file@insert to list*/
				{
					pPre->next = pNew;
					pNew->pre = pPre;
					pNew->next = pCur;
					pCur->pre = pNew;
				}
				break;
			}
		}
		#endif
	}
	
	/*save table header*/
	if(CDFS_FILE_FOLDER == CDFS_FILE_FOLDER_VIDEO)
	{
		CDFS_TAB_VIDEO = pHead;
		CDFS_FREE_VIDEO = pFree;
		CDFS_LAST_VIDEO = pLast;
		CDFS_NUM_VIDEO = fileCount;
		CDFS_ADD_FILE=0;
		/*CDFS_FIDX_VIDEO = fIdx;*/
	}
	else if(CDFS_FILE_FOLDER == CDFS_FILE_FOLDER_JPG)
	{
		CDFS_TAB_JPG = pHead;
		CDFS_FREE_JPG = pFree;
		CDFS_LAST_JPG = pLast;
		CDFS_NUM_JPG = fileCount;
		/*CDFS_FIDX_JPG = fIdx;*/
	}
	else if(CDFS_FILE_FOLDER == CDFS_FILE_FOLDER_EVENT)
	{
		CDFS_TAB_EVENT = pHead;
		CDFS_FREE_EVENT = pFree;
		CDFS_LAST_EVENT = pLast;
		CDFS_NUM_EVENT = fileCount;
		CDFS_ADD_FILE=0;
		/*CDFS_FIDX_EVENT = fIdx;*/
	}
	else if(CDFS_FILE_FOLDER == CDFS_FILE_FOLDER_AUDIO)
	{
		CDFS_TAB_AUDIO= pHead;
		CDFS_FREE_AUDIO = pFree;
		CDFS_LAST_AUDIO = pLast;
		CDFS_NUM_AUDIO= fileCount;
		CDFS_ADD_FILE=0;
	}


	if((appCdfsEventFlagGet()&VIDEO_EVENT_IN_VIDEO)
		&&((appCdfsFileNameToFileType(filename)&SP5K_DCF_FILETYPE_AVI)
		||(appCdfsFileNameToFileType(filename)&SP5K_DCF_FILETYPE_MOV)))
	{
		appCdfsEventProcess(filename,VIDEO_EVENT_IN_VIDEO);
		CDFS_FILE_FOLDER = CDFS_FILE_FOLDER_VIDEO;
		appCdfsEventFlagClean(VIDEO_EVENT_IN_VIDEO);
	}
	else if((appCdfsEventFlagGet()&JPG_EVENT_IN_VIDEO)
		&&(appCdfsFileNameToFileType(filename)&SP5K_DCF_FILETYPE_JPG))
	{
		appCdfsEventProcess(filename,JPG_EVENT_IN_VIDEO);
		CDFS_FILE_FOLDER = CDFS_FILE_FOLDER_VIDEO;
		appCdfsEventFlagClean(JPG_EVENT_IN_VIDEO);
	}
	
	headSem = 0;
	return SP5K_ERR_SUCCESS;
}


UINT32 appCdfsFileAdd(UINT8 *filename)
{
	CDFS_TAB_T *pNew=NULL;
	CDFS_TAB_T *pCur=NULL;
	CDFS_TAB_T *pPre=NULL;
	CDFS_TAB_T *pHead=NULL;
	CDFS_TAB_T *pFree=NULL;
	CDFS_TAB_T *pLast=NULL;
	UINT32 fileCount=0;
	UINT32 fd=0;
	UINT32 attr=0;
	/*UINT32 fIdx=1;*/
	/*UINT32 num=0;*/
	UINT32 filetype=SP5K_DCF_FILETYPE_AVI;
	UINT8 dirFileName[CDFS_FILE_NAME_LENGTH]={0};

	profLogAdd(3,"add file  --- s");
	if (filename == NULL){
		return SP5K_ERR_DCF_PARAM_GROUP;
	}

	//HLPrintf1("cdfs file add:%s\n",filename);
	strcpy(CDFS_PRE_FILE_NAME,filename);
	while(headSem)
	{
		appTimeDelay(20,1);
		RPrintf("waiting for sem in appCdfsFileAdd\n");
	}
	headSem = 1;
	strcpy(dirFileName,filename);
	if(!filename)
	{
		headSem = 0;
		return SP5K_ERR_DCF_PARAM_GROUP;
	}
	
	appCdfsAbsolutePathToFile(filename);

	if(	appCdfsFileNameToFileType(filename) & SP5K_DCF_FILETYPE_MOV
		|| appCdfsFileNameToFileType(filename) & SP5K_DCF_FILETYPE_AVI)
	{
		alreadyRecFileFlag = 1;
		memset(eventLockFileName, 0, 32);
		strcpy(eventLockFileName, dirFileName);
	}

	if((appCdfsEventFlagGet()&JPG_EVENT_IN_VIDEO)
		&&((appCdfsFileNameToFileType(filename)&SP5K_DCF_FILETYPE_AVI)
		||(appCdfsFileNameToFileType(filename)&SP5K_DCF_FILETYPE_MOV)))
	{
		CDFS_ADD_FILE=1;
		strcpy(CDFS_LAST_AVI_NAME,dirFileName);
		headSem = 0;
		return SP5K_ERR_SUCCESS;/*avoid adding video file when adding jpg*/
	}
	
	if((appCdfsEventFlagGet()&VIDEO_EVENT_IN_VIDEO)
		&&((appCdfsFileNameToFileType(filename)&SP5K_DCF_FILETYPE_AVI)
		||(appCdfsFileNameToFileType(filename)&SP5K_DCF_FILETYPE_MOV)))
	{
		CDFS_FILE_FOLDER = CDFS_FILE_FOLDER_EVENT;
		
	}
	else if((appCdfsEventFlagGet()&JPG_EVENT_IN_VIDEO)
		&&(appCdfsFileNameToFileType(filename)&SP5K_DCF_FILETYPE_JPG))
	{
		CDFS_FILE_FOLDER = CDFS_FILE_FOLDER_JPG;
	}


	if(appCdfsFileConfirm(filename,CDFS_FILE_FOLDER) == FAIL)
	{
		headSem = 0;
		return SP5K_ERR_SUCCESS;/*file is not file we need*/
	}

	
	if(CDFS_FILE_FOLDER == CDFS_FILE_FOLDER_VIDEO)
	{
		pHead = CDFS_TAB_VIDEO;/*Get current tab header*/
		pFree = CDFS_FREE_VIDEO;
		pLast = CDFS_LAST_VIDEO;
		fileCount = CDFS_NUM_VIDEO;
		filetype = appCdfsFileNameToFileType(filename);
	}
	else if(CDFS_FILE_FOLDER == CDFS_FILE_FOLDER_JPG)
	{
		pHead = CDFS_TAB_JPG;
		pFree = CDFS_FREE_JPG;
		pLast = CDFS_LAST_JPG;
		fileCount = CDFS_NUM_JPG;
		filetype = SP5K_DCF_FILETYPE_JPG;
	}
	else if(CDFS_FILE_FOLDER == CDFS_FILE_FOLDER_EVENT)
	{
		pHead = CDFS_TAB_EVENT;
		pFree = CDFS_FREE_EVENT;
		pLast = CDFS_LAST_EVENT;
		fileCount = CDFS_NUM_EVENT;
		filetype = appCdfsFileNameToFileType(filename);
	}
	else if(CDFS_FILE_FOLDER == CDFS_FILE_FOLDER_AUDIO)
	{
		pHead = CDFS_TAB_AUDIO;
		pFree = CDFS_FREE_AUDIO;
		fileCount = CDFS_NUM_AUDIO;
		pLast = CDFS_LAST_AUDIO;
		filetype = SP5K_DCF_FILETYPE_WAV;
	}
	
	#if 1
	if(pFree == NULL){
		CDFS_DBG("Alloc new Pool\n");
		pFree = appCdfsFileNewPoolReg(CDFS_FILE_FOLDER);
		DBG_ASSERT(pFree);
	}
	
	pNew = pFree;
	CDFS_DBG("pFree:0x%p,next:0x%p\n",pFree,pFree->next);
	#else
	pNew=sp5kMalloc(sizeof(CDFS_TAB_T));
	memset(pNew,0,sizeof(CDFS_TAB_T));
	DBG_ASSERT(pNew);
	#endif
	/****************************************/
	strncpy(pNew->FileName,filename,sizeof(pNew->FileName));
	fd=sp5kFsFileOpen(filename,SP5K_FS_OPEN_RDONLY);
	if(fd)
	{
		pNew->FileSize=sp5kFsFileSizeGet(fd);
		sp5kFsFileAttrGet(filename, &attr);
		pNew->attr = attr;
		if(attr & SP5K_FS_ATTR_READONLY)
		{
			pNew->dcfattr |= SP5K_DCF_FILE_ATTR_LOCK;
		}
		else
		{
			pNew->dcfattr = 0;
		}
		sp5kFsFileClose(fd);
		
	}
	/*Del the file avoid showing "file error"*/
	//if((pNew->FileSize <= (512L<<10))&&(filetype != SP5K_DCF_FILETYPE_JPG))
	if(pNew->FileSize == 0)
	{
		if(CDFS_FILE_FOLDER == CDFS_FILE_FOLDER_VIDEO ){
			sp5kDcfRecoverFileList_t *pList;
			pList=sp5kMalloc(sizeof(sp5kDcfRecoverFileList_t));
			memset(pList,0,sizeof(sp5kDcfRecoverFileList_t));

			if(appCdfsFileNameToFileType(pNew->FileName)&SP5K_DCF_FILETYPE_MOV)
			pList->fileType = SP5K_DCF_FILETYPE_MOV ;
			else
			pList->fileType = SP5K_DCF_FILETYPE_AVI ;

			strcpy(pList->pathName , pNew->FileName) ;
			pList->pnext = NULL ;
			sp5kDcfFsFileOperate(SP5K_DCF_FILEOP_RECOVER_FILE, (UINT32)pList, 0, 0, 0); 
			sp5kFree(pList);
		}else{
			//sp5kDcfFsFileOperate(SP5K_DCF_FILEOP_RECOVER_FAT,(UINT32)pNew->FileName,0,NULL,0);
			sp5kFsFileDelete(pNew->FileName);
			//sp5kFree(pNew);
			HLPrintf1("file %s is NULL\n", pNew->FileName);
			headSem = 0;
			return SP5K_ERR_SUCCESS;
		}
	}
	
	pNew->FileType = filetype;
	/****************************************/
	fileCount++;

	if(CDFS_FILE_FOLDER == CDFS_FILE_FOLDER_VIDEO)
	{
		if(CDFS_DEL_VIDEO_METHOD == CDFS_PROTECTED_SKIP)
		{
			if(!(pNew->dcfattr & SP5K_DCF_FILE_ATTR_LOCK))
			{
				CDFS_VIDEO_SIZE+=(UINT64)pNew->FileSize;
			}
		}
		else
		{
			CDFS_VIDEO_SIZE+=pNew->FileSize;
		}
		/*__FUNC_TRACK__;*/
		/*printf("size = %u(B),%u(MB)\n",(UINT32)CDFS_VIDEO_SIZE,((UINT32)CDFS_VIDEO_SIZE)>>20);*/
	}
	/*****************************************/
	if(pHead == NULL)/*create first object of table*/
	{
		/*add file@head*/
		pFree = pFree->next;
		if(pFree){
			pFree->pre = NULL;
			appCdfsUnitInfoClear(pFree);
		}
		pNew->next = NULL;
		pNew->pre = NULL;
		pHead = pNew;
		pLast = pNew;
	}
	else
	{
		#if 1
		pCur = pLast;

		while(1){
			if(strcmp(pNew->FileName,pCur->FileName) >0)/*sort by date*/
			{
				/*add file@object added is newest*/
				pFree = pFree->next;
				if(pFree){
					pFree->pre = NULL;
					appCdfsUnitInfoClear(pFree);
				}
				pNew->pre = pCur;
				pNew->next = pCur->next;
				pCur->next=pNew;
				while(1){
					if(pCur->next){
						pCur = pCur->next;
					}else{
						pLast = pCur;
						break;
					}
				}
				break;
			}
			else if(strcmp(pNew->FileName,pCur->FileName) == 0)/*the same name*/
			{/*the new file replace the old file when two file name is same*/
				pCur->FileSize = pNew->FileSize;
				pCur->attr = pNew->attr;
				pCur->dcfattr = pNew->dcfattr;
				fileCount--;
				break;
			}
			else
			{
				if(pCur->pre){
					pPre= pCur;
					pCur=pCur->pre;
				}else{
					pFree = pFree->next;
					if(pFree){
						pFree->pre = NULL;
						appCdfsUnitInfoClear(pFree);
					}
					pCur->pre = pNew;
					pNew->next = pCur;
					pHead = pNew;
					break;
				}
			}
		}
		#else
		pPre=pCur=pHead;
		while(1)
		{
			if(strcmp(pNew->FileName,pCur->FileName) >0)/*sort by date*/
			{
				/*fIdx++;*/
				if(pCur->next)
				{
					pPre=pCur;
					pCur=pCur->next;
				}
				else
				{/*add file@object added is newest*/
					pCur->next=pNew;
					pNew->next=NULL;
					pNew->pre = pCur;
					break;
				}
			}
			else if(strcmp(pNew->FileName,pCur->FileName) == 0)/*the same name*/
			{/*the new file replace the old file when two file name is same*/
				pCur->FileSize = pNew->FileSize;
				pCur->attr = pNew->attr;
				pCur->dcfattr = pNew->dcfattr;
				fileCount--;
				break;
			}
			else
			{/*add file@insert head*/
				if(pCur==pHead)
				{	pNew->pre = NULL;
					pNew->next = pHead;
					pHead->pre = pNew;
					pHead = pNew;
				}
				else/*add file@insert to list*/
				{
					pPre->next = pNew;
					pNew->pre = pPre;
					pNew->next = pCur;
					pCur->pre = pNew;
				}
				break;
			}
		}
		#endif
	}
	
	/*save table header*/
	if(CDFS_FILE_FOLDER == CDFS_FILE_FOLDER_VIDEO)
	{
		CDFS_TAB_VIDEO = pHead;
		CDFS_FREE_VIDEO = pFree;
		CDFS_LAST_VIDEO = pLast;
		CDFS_NUM_VIDEO = fileCount;
		CDFS_ADD_FILE=0;
		/*CDFS_FIDX_VIDEO = fIdx;*/
	}
	else if(CDFS_FILE_FOLDER == CDFS_FILE_FOLDER_JPG)
	{
		CDFS_TAB_JPG = pHead;
		CDFS_FREE_JPG = pFree;
		CDFS_LAST_JPG= pLast;
		CDFS_NUM_JPG = fileCount;
		/*CDFS_FIDX_JPG = fIdx;*/
	}
	else if(CDFS_FILE_FOLDER == CDFS_FILE_FOLDER_EVENT)
	{
		CDFS_TAB_EVENT = pHead;
		CDFS_FREE_EVENT = pFree;
		CDFS_LAST_EVENT= pLast;
		CDFS_NUM_EVENT = fileCount;
		CDFS_ADD_FILE=0;
		/*CDFS_FIDX_EVENT = fIdx;*/
	}
	else if(CDFS_FILE_FOLDER == CDFS_FILE_FOLDER_AUDIO)
	{
		CDFS_TAB_AUDIO= pHead;
		CDFS_FREE_AUDIO = pFree;
		CDFS_LAST_AUDIO = pLast;
		CDFS_NUM_AUDIO= fileCount;
		CDFS_ADD_FILE=0;
	}


	if((appCdfsEventFlagGet()&VIDEO_EVENT_IN_VIDEO)
		&&((appCdfsFileNameToFileType(filename)&SP5K_DCF_FILETYPE_AVI)
		||(appCdfsFileNameToFileType(filename)&SP5K_DCF_FILETYPE_MOV)))
	{
		appCdfsEventProcess(filename,VIDEO_EVENT_IN_VIDEO);
		CDFS_FILE_FOLDER = CDFS_FILE_FOLDER_VIDEO;
		appCdfsEventFlagClean(VIDEO_EVENT_IN_VIDEO);
	}
	else if((appCdfsEventFlagGet()&JPG_EVENT_IN_VIDEO)
		&&(appCdfsFileNameToFileType(filename)&SP5K_DCF_FILETYPE_JPG))
	{
		appCdfsEventProcess(filename,JPG_EVENT_IN_VIDEO);
		CDFS_FILE_FOLDER = CDFS_FILE_FOLDER_VIDEO;
		appCdfsEventFlagClean(JPG_EVENT_IN_VIDEO);
	}
	
	headSem = 0;
	profLogAdd(3,"add file  --- e");
	return SP5K_ERR_SUCCESS;
}
UINT32 appCdfsFileDel(UINT8 *filename)
{
	CDFS_TAB_T *pCur=NULL;
	CDFS_TAB_T *pPre=NULL;
	CDFS_TAB_T *pNext=NULL;
	CDFS_TAB_T *pHead=NULL;
	CDFS_TAB_T *pFree=NULL;
	CDFS_TAB_T *pLast=NULL;
	UINT32 fileCount=0;
	while(headSem)
	{
		appTimeDelay(20,1);
		RPrintf("waiting for sem in appCdfsFileDel\n");
	}
	headSem = 1;

	/*UINT32 fIdx=1;*/
	if(CDFS_FILE_FOLDER == CDFS_FILE_FOLDER_VIDEO)
	{
		pHead = CDFS_TAB_VIDEO;/*Get current tab header*/
		pFree = CDFS_FREE_VIDEO;
		fileCount = CDFS_NUM_VIDEO;
	}
	else if(CDFS_FILE_FOLDER == CDFS_FILE_FOLDER_JPG)
	{
		pHead = CDFS_TAB_JPG;
		pFree = CDFS_FREE_JPG;
		fileCount = CDFS_NUM_JPG;
	}
	else if(CDFS_FILE_FOLDER == CDFS_FILE_FOLDER_EVENT)
	{
		pHead = CDFS_TAB_EVENT;
		pFree = CDFS_FREE_EVENT;
		fileCount = CDFS_NUM_EVENT;
	}
	else if(CDFS_FILE_FOLDER == CDFS_FILE_FOLDER_AUDIO)
	{
		pHead = CDFS_TAB_AUDIO;
		pFree = CDFS_FREE_AUDIO;
		fileCount = CDFS_NUM_AUDIO;
	}
	
	if(!filename  || !pHead)
	{
		headSem = 0;
		return FAIL;
	}

	appCdfsAbsolutePathToFile(filename);
	pCur=pPre=pHead;
	while(1)
	{
		if(strcmp(pCur->FileName,filename)==0)/*find the file*/
		{
			pPre->next = pCur->next;
			pNext = pCur->next;
			if(pNext)
			{
				pNext->pre = pPre;
			}
			
			if(pCur == pHead)
			{
				if(pCur->next)
				{
					pHead = pCur->next;
					pHead->pre = NULL;
				}
				else
				{
					printf("table is null\n");
					pHead = NULL;
				}
			}
			/****************************************/
			if(CDFS_FILE_FOLDER == CDFS_FILE_FOLDER_VIDEO)
			{
				if(CDFS_DEL_VIDEO_METHOD == CDFS_PROTECTED_SKIP)
				{
					if(!(pCur->dcfattr & SP5K_DCF_FILE_ATTR_LOCK))
					{
						CDFS_VIDEO_SIZE-=pCur->FileSize;
					}
				}
				else
				{
					CDFS_VIDEO_SIZE-=pCur->FileSize;
				}

			}
			/*****************************************/
			HLPrintf1("filename=%s\n",pCur->FileName);
			pCur->next = pFree;
			if(pFree){
				pFree->pre = pCur;
			}
			pFree = pCur;
			pFree->pre = NULL;
			appCdfsUnitInfoClear(pFree);
			//sp5kFree(pCur);
			//pCur = NULL;
			fileCount--;
			/*if(fIdx > fileCount)
			{
				fIdx = fileCount;
			}*/
			break;
		}
		else/*not find file*/
		{
			if(pCur->next)/*keep on finding*/
			{
				pPre=pCur;
				pCur=pCur->next;
				/*fIdx++;*/
			}
			else
			{
				printf("there isn't this file\n");
				break;/*there isn't this file in tab*/
			}
		}
	}
	
	pLast = pHead;
	if(pLast!=NULL)
	{
		while(pLast->next){
			pLast = pLast->next;
		}
	}

	if(CDFS_FILE_FOLDER == CDFS_FILE_FOLDER_VIDEO)
	{
		CDFS_TAB_VIDEO = pHead;
		CDFS_FREE_VIDEO = pFree;
		CDFS_LAST_VIDEO = pLast;
		CDFS_NUM_VIDEO = fileCount;
		/*CDFS_FIDX_VIDEO = fIdx;*/
	}
	else if(CDFS_FILE_FOLDER == CDFS_FILE_FOLDER_JPG)
	{
		CDFS_TAB_JPG = pHead;
		CDFS_FREE_JPG = pFree;
		CDFS_LAST_JPG= pLast;
		CDFS_NUM_JPG = fileCount;
		/*CDFS_FIDX_JPG = fIdx;*/
	}
	else if(CDFS_FILE_FOLDER == CDFS_FILE_FOLDER_EVENT)
	{
		CDFS_TAB_EVENT = pHead;
		CDFS_FREE_EVENT = pFree;
		CDFS_LAST_EVENT= pLast;
		CDFS_NUM_EVENT = fileCount;
		/*CDFS_FIDX_EVENT = fIdx;*/
	}
	else if(CDFS_FILE_FOLDER == CDFS_FILE_FOLDER_AUDIO)
	{
		CDFS_TAB_AUDIO= pHead;
		CDFS_FREE_AUDIO = pFree;
		CDFS_LAST_AUDIO = pLast;
		CDFS_NUM_AUDIO= fileCount;
		/*CDFS_FIDX_EVENT = fIdx;*/
	}
	headSem = 0;

	if(!fileCount){
		appCdfsPoolReset(CDFS_FILE_FOLDER);
	}
	return SUCCESS;

}

CDFS_TAB_T *appCdfsFileListLastGet(UINT32 folder)
{
	if(folder == CDFS_FILE_FOLDER_VIDEO)
	{
		return CDFS_LAST_VIDEO;
	}
	else if(folder == CDFS_FILE_FOLDER_JPG)
	{
		return CDFS_LAST_JPG;
	}
	else if(folder == CDFS_FILE_FOLDER_EVENT)
	{
		return CDFS_LAST_EVENT;
	}
	else if(folder == CDFS_FILE_FOLDER_AUDIO)
	{
		return CDFS_LAST_AUDIO;
	}
	return CDFS_LAST_VIDEO;
}

CDFS_TAB_T *appCdfsFileListHeadGet(UINT32 folder)
{
	if(folder == CDFS_FILE_FOLDER_VIDEO)
	{
		return CDFS_TAB_VIDEO;
	}
	else if(folder == CDFS_FILE_FOLDER_JPG)
	{
		return CDFS_TAB_JPG;
	}
	else if(folder == CDFS_FILE_FOLDER_EVENT)
	{
		return CDFS_TAB_EVENT;
	}
	else if(folder == CDFS_FILE_FOLDER_AUDIO)
	{
		return CDFS_TAB_AUDIO;
	}
	return CDFS_TAB_VIDEO;
}

UINT32 appCdfsFileListTabInit(UINT32 folder)
{
#if 1
	appCdfsPoolReset(folder);
	return SUCCESS;
#else
	CDFS_TAB_T *pCur;
	CDFS_TAB_T *pPre;
	switch(folder)
	{
		case CDFS_FILE_FOLDER_VIDEO:
			pCur = CDFS_TAB_VIDEO;
			if(pCur == NULL)
			{
				break;
			}
			while(pCur)
			{
				pPre = pCur;
				pCur = pCur->next;
				sp5kFree(pPre);
			}
			/*sp5kFree(pCur);*/
			CDFS_TAB_VIDEO = NULL;
			CDFS_LAST_VIDEO = NULL;
			CDFS_VIDEO_SIZE = 0;
			CDFS_NUM_VIDEO = 0;
			break;
		case CDFS_FILE_FOLDER_JPG:
			pCur = CDFS_TAB_JPG;
			if(pCur == NULL)
			{
				break;
			}

			while(pCur)
			{
				pPre = pCur;
				pCur = pCur->next;
				sp5kFree(pPre);
			}
			/*sp5kFree(pCur);*/
			CDFS_TAB_JPG = NULL;
			CDFS_LAST_JPG = NULL;
			CDFS_NUM_JPG = 0;
			break;
		case CDFS_FILE_FOLDER_AUDIO:
			pCur = CDFS_TAB_AUDIO;
			if(pCur == NULL)
			{
				break;
			}
			while(pCur)
			{
				pPre = pCur;
				pCur = pCur->next;
				sp5kFree(pPre);
			}
			/*sp5kFree(pCur);*/
			CDFS_TAB_AUDIO= NULL;
			CDFS_LAST_AUDIO= NULL;
			CDFS_NUM_AUDIO= 0;
			break;
		case CDFS_FILE_FOLDER_EVENT:
			pCur = CDFS_TAB_EVENT;
			if(pCur == NULL)
			{
				break;
			}
			while(pCur)
			{
				pPre = pCur;
				pCur = pCur->next;
				sp5kFree(pPre);
			}
			/*sp5kFree(pCur);*/
			CDFS_TAB_EVENT = NULL;
			CDFS_LAST_EVENT = NULL;
			CDFS_NUM_EVENT = 0;
			break;
		default:
			break;
	}
	return SUCCESS;
#endif	
}
void appCdfsFileListPrintf(UINT32 folder)
{
	UINT32 num=0;
	CDFS_TAB_T *pObj=NULL;
	
	if(folder == CDFS_FILE_FOLDER_VIDEO)
	{
		pObj = CDFS_TAB_VIDEO;
	}
	else if(folder == CDFS_FILE_FOLDER_JPG)
	{
		pObj = CDFS_TAB_JPG;
	}
	else if(folder == CDFS_FILE_FOLDER_EVENT)
	{
		pObj = CDFS_TAB_EVENT;
	}
	else if(folder == CDFS_FILE_FOLDER_AUDIO)
	{
		pObj = CDFS_TAB_AUDIO;
	}
	
	while(pObj)
	{
		printf("file[%d]---->%s\n",num,pObj->FileName);
		/*printf("attr=0x%x\n",pObj->dcfattr);
		printf("filesize=%d\n",pObj->FileSize);*/
		if(pObj->next)
		{
			pObj=pObj->next;
			num++;
		}
		else
		{
			break;
		}
	}
	printf("list printf finish!\n");
}
UINT8 appCdfsFileNameExistCheck(UINT8 *pCheckFileName)
{
	CDFS_TAB_T *pObj=NULL;
	UINT32 folder;
	UINT8 fName[CDFS_FILE_NAME_LENGTH]={0};
	UINT8 fPathName[CDFS_FILE_NAME_LENGTH]={0};
	UINT8 attName[4]={0};
	UINT32 sameNameNum=0,attNum=0,sameNameMax=0;
	UINT8 ret;
	
	folder =appCdfsFileFolderGet();
	if(folder == CDFS_FILE_FOLDER_VIDEO)
	{
		pObj = CDFS_TAB_VIDEO;
	}
	else if(folder == CDFS_FILE_FOLDER_JPG)
	{
		pObj = CDFS_TAB_JPG;
	}
	else if(folder == CDFS_FILE_FOLDER_EVENT)
	{
		pObj = CDFS_TAB_EVENT;
	}
	else if(folder == CDFS_FILE_FOLDER_AUDIO)
	{
		pObj = CDFS_TAB_AUDIO;
	}
	
	while(pObj)
	{
		strcpy(fName,pObj->FileName);
		fName[15] = '\0';
		sprintf(fPathName,"%s/%s",sp5kFsGetCurPath(),fName);
		if(strcmp(fPathName,pCheckFileName)==0)
		{
			if(pObj->FileName[15]=='_')/*save and find max attach name */
			{
				attNum =((pObj->FileName[16]-48)*10+pObj->FileName[17]-48);/*get attach num*/
				/*printf("attNum = %d,sameNameMax = %d\n",attNum,sameNameMax);*/
				sameNameMax = attNum>sameNameMax?attNum:sameNameMax;/*record max attach num*/
			}
			sameNameNum = sameNameMax +1;
			if(sameNameNum == 99)
			{
				sameNameNum = 99;
				RPrintf("same name file is over 100,cover the last file\n");
			}
			/*printf("file---->%s,sameNameNum =%d\n",fPathName,sameNameNum);*/
		}
		if(pObj->next)
		{
			pObj=pObj->next;
		}
		else
		{
			if(sameNameNum)
			{
				sprintf(attName,"_%02d",sameNameNum);
				strcat(pCheckFileName, attName);
				ret = TRUE;
			}
			else
			{
				ret = FALSE;
			}
			printf("final file---->%s\n",pCheckFileName);/**/
			break;
		}
	}
	printf("list check finish!\n");
	return ret;
}

UINT32 appCdfsFileConfirm(UINT8* fname,UINT32 folder)
{
	#if 1
	
	uiPara_t* puiPara = appUiParaGet();
	UINT8 temp[CDFS_FILE_NAME_LENGTH];
	UINT32 ret = FAIL;
	
	strcpy(temp, fname);
	strupr(temp);	
	switch(folder)
	{
		case CDFS_FILE_FOLDER_VIDEO:
		case CDFS_FILE_FOLDER_EVENT:
			if(	(puiPara->MediaFormat == MEDIA_FORMAT_AVI && strcmp(temp + strlen(temp) - strlen(".AVI"), ".AVI") == 0)
				|| (puiPara->MediaFormat == MEDIA_FORMAT_MOV && strcmp(temp + strlen(temp) - strlen(".MOV"), ".MOV") == 0))
			{
				ret = SUCCESS;
			}
			break;
		case CDFS_FILE_FOLDER_JPG:
			if(strcmp(temp + strlen(temp) - strlen(".JPG"), ".JPG") == 0)
			{
				ret = SUCCESS;
			}
			break;
		case CDFS_FILE_FOLDER_AUDIO:
			if(strcmp(temp + strlen(temp) - strlen(".WAV"), ".WAV") == 0)
			{
				ret = SUCCESS;
			}
			break;
	}

	return ret;

	#else

	UINT8 fileName[CDFS_FILE_NAME_LENGTH];
	char exName=0;
	uiPara_t* puiPara = appUiParaGet();

	if(folder == CDFS_FILE_FOLDER_VIDEO)
	{
		if(puiPara->MediaFormat == MEDIA_FORMAT_AVI)
		{
			exName = 'A';/*AVI*/
		}
		else
		{
			exName = 'M';/*MOV*/
		}
	}
	else if(folder == CDFS_FILE_FOLDER_JPG)
	{
		exName = 'J';/*JPG*/
	}
	else if(folder == CDFS_FILE_FOLDER_EVENT)
	{
		if(puiPara->MediaFormat == MEDIA_FORMAT_AVI)
		{
			exName = 'A';/*AVI*/
		}
		else
		{
			exName = 'M';/*MOV*/
		}
	}
	else if(folder == CDFS_FILE_FOLDER_AUDIO)
	{
		exName = 'W';/*JPG*/
	}

	
	strncpy(fileName,fname,20);
	if((fileName[8] == '_')&&((fileName[15] == '.')||(fileName[18] == '.'))&&((fileName[16] == exName)||(fileName[19] == exName)))/*AVI,JPG*/
	{
		return SUCCESS;
	}
	else
	{
		return FAIL;
	}

	#endif
}

UINT32 appCdfsCurrFolderFileListCreate(UINT32 folder,UINT8* dirPath)
{
    int cntItem = 0;
    UINT32 ret;
    UINT32 flag;
    char fstname[CDFS_FILE_NAME_LENGTH];/*file short name*/
    char curPath[256];
    UINT8 *flgname;/*file long name*/
    UINT32 fnLen=32;
	UINT32 attr;
	UINT64 fSz;
	tmx_t  tm;
	
    if (dirPath!= NULL)
    {
        printf("Current Root %s\n", dirPath);
        cntItem = sp5kDirFileCntGet(dirPath);
        appCdfsFileListTabInit(folder);
        if (cntItem != 0)
        {
            strcpy(curPath, sp5kFsGetCurPath());
            printf("%s has %d child: \n", dirPath, cntItem);
            if (sp5kFsDirChange(dirPath) == 0)
            {
				appCdfsFileFolderSet(folder);
				profLogAdd(3,"Start");
                while (cntItem)
                {
					ret = sp5kFsDirNextFileGet(fstname,&fnLen, &attr, &fSz, &tm);
					flgname = sp5kFsAliasNameGet(fstname,  SP5K_FS_STR_NATIVE);
                    if (ret == 0)
                    {
						cntItem--;
	                    if(flgname)
	                	{
	                		if(strlen(flgname) > 25){
								/* file name is too long */
								continue;		
							}
	                	}
						else
						{
							continue;		
						}
						if(flgname && (folder == CDFS_FILE_FOLDER_VIDEO))
						{
							strcpy(CDFS_LAST_AVI_NAME,flgname);
							printf("Last file name:%s\n",CDFS_LAST_AVI_NAME);
						}
						appCdfsAddFile2List(flgname,fnLen,(UINT32)fSz,attr);
					}
                }
				profLogAdd(3,"End");
            }
            printf("\n Search finish!\n");
        }
        else
        {
			strcpy(curPath, sp5kFsGetCurPath());
			printf("%s has %d child: \n", dirPath, cntItem);
			if (sp5kFsDirChange(dirPath) == 0)
			{
				appCdfsFileFolderSet(folder);
			}
			printf("%s has no child in.", dirPath);
		}
    }
    return SUCCESS;
}

/**
**	flag:
**	JPG_EVENT_IN_VIDEO		= 1<<0,
**	VIDEO_EVENT_IN_VIDEO		= 1<<1,
*/
void appCdfsEventFlagSet(UINT32 flag)
{
	CDFS_EVENT_OCCUR |= flag;
	if(flag == VIDEO_EVENT_IN_VIDEO)
	{
		appOsd_GfxIconIDSet(SP5K_RES_OSD_ICON);
		appOsdLib_OsdLayIconDraw(0,64,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_PROTECT);
	}
}

void appCdfsEventFlagClean(UINT32 flag)
{
	CDFS_EVENT_OCCUR &= ~flag;
	if(flag == VIDEO_EVENT_IN_VIDEO)
	{
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT,0, 64,36,32,0,0);
	}
}

UINT32 appCdfsEventFlagGet(void)
{
	return CDFS_EVENT_OCCUR;
}

UINT32 appCdfsFileFolderSet(UINT32 folder)
{
	CDFS_FILE_FOLDER = folder;
	return SUCCESS;
}

UINT32 appCdfsFileFolderGet(void)
{
	return CDFS_FILE_FOLDER;
}

UINT32 appCdfsCurFileIdxSet(UINT32 fileIdx)
{
	if(CDFS_FILE_FOLDER == CDFS_FILE_FOLDER_VIDEO)
	{
		CDFS_FIDX_VIDEO = fileIdx;
	}
	else if(CDFS_FILE_FOLDER == CDFS_FILE_FOLDER_JPG)
	{
		CDFS_FIDX_JPG = fileIdx;
	}
	else if(CDFS_FILE_FOLDER == CDFS_FILE_FOLDER_EVENT)
	{
		CDFS_FIDX_EVENT = fileIdx;
	}
	else if(CDFS_FILE_FOLDER == CDFS_FILE_FOLDER_AUDIO)
	{
		CDFS_FIDX_AUDIO= fileIdx;
	}
	return SP5K_ERR_SUCCESS;
}


UINT32 appCdfsCurFileIdxGet(UINT32 *fileIdx)
{
	if(CDFS_FILE_FOLDER == CDFS_FILE_FOLDER_VIDEO)
	{
		*fileIdx = CDFS_FIDX_VIDEO;
	}
	else if(CDFS_FILE_FOLDER == CDFS_FILE_FOLDER_JPG)
	{
		*fileIdx = CDFS_FIDX_JPG;
	}
	else if(CDFS_FILE_FOLDER == CDFS_FILE_FOLDER_EVENT)
	{
		*fileIdx = CDFS_FIDX_EVENT;
	}
	else if(CDFS_FILE_FOLDER == CDFS_FILE_FOLDER_AUDIO)
	{
		*fileIdx = CDFS_FIDX_AUDIO;
	}

	return SP5K_ERR_SUCCESS;
}

UINT32 appCdfsFileNumGet(UINT32 *filenum)
{
	if(CDFS_FILE_FOLDER == CDFS_FILE_FOLDER_VIDEO)
	{
		*filenum = CDFS_NUM_VIDEO;
	}
	else if(CDFS_FILE_FOLDER == CDFS_FILE_FOLDER_JPG)
	{
		*filenum = CDFS_NUM_JPG;
	}
	else if(CDFS_FILE_FOLDER == CDFS_FILE_FOLDER_EVENT)
	{
		*filenum = CDFS_NUM_EVENT;
	}
	else if(CDFS_FILE_FOLDER == CDFS_FILE_FOLDER_AUDIO)
	{
		*filenum = CDFS_NUM_AUDIO;
	}
	return SUCCESS;
}

/*idx start from 1*/
CDFS_TAB_T * appCdfsFileHandleGet(UINT32 idx)
{
	CDFS_TAB_T *pCur=NULL;

	if(idx == 0)
	{
		return NULL;
	}
	
	if(CDFS_FILE_FOLDER == CDFS_FILE_FOLDER_VIDEO)
	{
		pCur = CDFS_TAB_VIDEO;/*Get current tab header*/
	}
	else if(CDFS_FILE_FOLDER == CDFS_FILE_FOLDER_JPG)
	{
		pCur = CDFS_TAB_JPG;
	}
	else if(CDFS_FILE_FOLDER == CDFS_FILE_FOLDER_EVENT)
	{
		pCur = CDFS_TAB_EVENT;
	}
	else if(CDFS_FILE_FOLDER == CDFS_FILE_FOLDER_AUDIO)
	{
		pCur = CDFS_TAB_AUDIO;
	}
	
	if(pCur == NULL)
	{
		return pCur;
	}

	if(idx)
	{
		idx--;
	}
	while(idx)
	{
		if(pCur->next)
		{
			pCur = pCur->next;
		}
		else
		{
			pCur = NULL;
			break;
		}
		idx--;
	}

	return pCur;
}


UINT32 appCdfsFileAttrGet(UINT32 fileIdx, sp5kDcfAttrElm_t* pfileInfoAtr, UINT8* pname)
{
	CDFS_TAB_T *pCur;
	UINT8 fname[CDFS_FILE_NAME_LENGTH]={0};
	UINT32 fileNum;
	UINT32 date,time;
	UINT8 *pTime;
	pCur=appCdfsFileHandleGet(fileIdx);
	if(!pCur)
	{
		RPrintf("NO file In this index\n");
		return FAIL;
	}
	pTime = &(pCur->FileName[9]);
	date = strtol(pCur->FileName,NULL,10);
	time = strtol(pTime,NULL,10);
	appCdfsFileNumGet(&fileNum);
	sprintf(fname,"%s/",sp5kFsGetCurPath());
	strcat(fname, pCur->FileName);/*absolute path*/
	strcpy(pname, fname); 
	strcpy(pfileInfoAtr->filename,pCur->FileName);
	strcpy(pfileInfoAtr->dirname,sp5kFsGetCurPath());
	pfileInfoAtr->attribute = pCur->dcfattr;
	pfileInfoAtr->filesize = pCur->FileSize;
	pfileInfoAtr->fileType = pCur->FileType;
	pfileInfoAtr->dirNum = 1;
	pfileInfoAtr->fileNum = fileNum;
	pfileInfoAtr->time.year = date/10000;
	pfileInfoAtr->time.mon = date%10000/100;
	pfileInfoAtr->time.day =  date%10000%100;
	pfileInfoAtr->time.hour = time/10000;
	pfileInfoAtr->time.min = time%10000/100;
	pfileInfoAtr->time.sec = time%10000%100;
	return SUCCESS;
}


UINT32 appCdfsInfoGet(UINT32 option, UINT32* pValue)
{
	UINT32 ret = SUCCESS;
	switch(option)
	{
		case SP5K_DCF_INFO_TOTAL_FILENUM:
			appCdfsFileNumGet(pValue);
			break;
		default:
			break;
	}
	
	return ret;
}
UINT32 appCdfsAllFileProcess(UINT32 opt)
{
	UINT32 num=0;
	CDFS_TAB_T *pObj=NULL;
	CDFS_TAB_T *pCur=NULL;
	UINT8 fileName[CDFS_FILE_NAME_LENGTH];
	UINT8 dirName[CDFS_FILE_NAME_LENGTH];
	if(CDFS_FILE_FOLDER == CDFS_FILE_FOLDER_VIDEO)
	{
		pObj = CDFS_TAB_VIDEO;
		pCur = CDFS_TAB_VIDEO;
	}
	else if(CDFS_FILE_FOLDER == CDFS_FILE_FOLDER_JPG)
	{
		pObj = CDFS_TAB_JPG;
		pCur = CDFS_TAB_JPG;
	}
	else if(CDFS_FILE_FOLDER == CDFS_FILE_FOLDER_EVENT)
	{
		pObj = CDFS_TAB_EVENT;
		pCur = CDFS_TAB_EVENT;
	}
	else if(CDFS_FILE_FOLDER == CDFS_FILE_FOLDER_AUDIO)
	{
		pObj = CDFS_TAB_AUDIO;
		pCur = CDFS_TAB_AUDIO;
	}
	strcpy(dirName,sp5kFsGetCurPath());
	
	if(CDFS_FILE_FOLDER == CDFS_FILE_FOLDER_VIDEO)
	{
		if(CDFS_DEL_VIDEO_METHOD == CDFS_PROTECTED_SKIP)
		{
			CDFS_VIDEO_SIZE=0;
		}
	}

	if(opt == SP5K_DCF_FILEOP_DELALL){
		while(pCur){
			if(pCur->dcfattr & SP5K_DCF_FILE_ATTR_LOCK)
			{
				CDFS_DBG("file locked:%s\n",pCur->FileName);
				pCur = pCur->next;
			}
			else
			{
				sprintf(fileName,"%s/%s",dirName,pCur->FileName);
				//printf("Del file : %s\n",fileName);
				CDFS_DBG("pObj:0x%p:%s\n",pCur,pCur->FileName);
				sp5kFsFileDelete(fileName);
				/* save the next file's address*/
				pObj = pCur->next;  
				appCdfsFileDel(pCur->FileName);
				pCur = pObj;
			}
		}
	}else{
		while(pObj)
		{
			switch(opt)
			{
				case SP5K_DCF_FILEOP_LOCKALL:
					sprintf(fileName,"%s/%s",dirName,pObj->FileName);
					pObj->dcfattr |= SP5K_DCF_FILE_ATTR_LOCK;
					sp5kFsFileAttrSet(fileName,pObj->attr | SP5K_FS_ATTR_READONLY);
					break;
				case SP5K_DCF_FILEOP_UNLOCKALL:
					sprintf(fileName,"%s/%s",dirName,pObj->FileName);
					pObj->dcfattr &= ~SP5K_DCF_FILE_ATTR_LOCK;
					sp5kFsFileAttrSet(fileName,pObj->attr &(~ SP5K_FS_ATTR_READONLY));
					if(CDFS_FILE_FOLDER == CDFS_FILE_FOLDER_VIDEO)
					{
						if(CDFS_DEL_VIDEO_METHOD == CDFS_PROTECTED_SKIP)
						{
							CDFS_VIDEO_SIZE += pObj->FileSize;
						}
					}
					
					break;
				#if 0	
				case SP5K_DCF_FILEOP_DELALL:/*hao.xu 2103 10 10 delete all*/
					if(pObj->dcfattr & SP5K_DCF_FILE_ATTR_LOCK)
					{
						break;
					}
					else
					{
						sprintf(fileName,"%s/%s",dirName,pObj->FileName);
						//printf("Del file : %s\n",fileName);
						printf("pObj:0x%p:%s\n",pObj,pObj->FileName);
						sp5kFsFileDelete(fileName);
						appCdfsFileDel(pObj->FileName);
					}
					break;
				#endif	
				default:
					break;

			}
			
			if(pObj->next)
			{
				pObj=pObj->next;
				num++;
			}
			else
			{
				break;
			}
		}
	}
	return SUCCESS;
}
UINT32 appCdfsFileOperate(UINT32 option, UINT32 param, UINT32 async, fpsp5kCallback_t pcallback, UINT32 length)
{
	CDFS_TAB_T *pCur;
	UINT32 fileIdx;
	UINT8 fileName[CDFS_FILE_NAME_LENGTH];
	UINT32 ret = SUCCESS;
	appCdfsCurFileIdxGet(&fileIdx);
	pCur = appCdfsFileHandleGet(fileIdx);
	strcpy(fileName,sp5kFsGetCurPath());
	sprintf(fileName,"%s/%s",fileName,pCur->FileName);
	CDFS_DBG("option = %d, filename=%s\n",option, fileName);/**/
	switch(option)
	{
		case SP5K_DCF_FILEOP_COPY:
			break;
		case SP5K_DCF_FILEOP_COPYALL:
			break;
		case SP5K_DCF_FILEOP_DEL:
			sp5kFsFileDelete(fileName);/*file name with dir path*/
			appCdfsFileDel(pCur->FileName);
			break;
		case SP5K_DCF_FILEOP_DELALL:
			appCdfsAllFileProcess(SP5K_DCF_FILEOP_DELALL);
			break;
		case SP5K_DCF_FILEOP_LOCK:
			pCur->dcfattr |= SP5K_DCF_FILE_ATTR_LOCK;
			sp5kFsFileAttrSet(fileName,pCur->attr |SP5K_FS_ATTR_READONLY);
			if(CDFS_FILE_FOLDER == CDFS_FILE_FOLDER_VIDEO)
			{
				if(CDFS_DEL_VIDEO_METHOD == CDFS_PROTECTED_SKIP)
				{
					CDFS_VIDEO_SIZE-=pCur->FileSize;
				}
			}

			break;
		case SP5K_DCF_FILEOP_LOCKALL:
			appCdfsAllFileProcess(SP5K_DCF_FILEOP_LOCKALL);
			break;
		case SP5K_DCF_FILEOP_UNLOCK:
			pCur->dcfattr &= ~SP5K_DCF_FILE_ATTR_LOCK;
			sp5kFsFileAttrSet(fileName,pCur->attr &(~SP5K_FS_ATTR_READONLY));
			if(CDFS_FILE_FOLDER == CDFS_FILE_FOLDER_VIDEO)
			{
				if(CDFS_DEL_VIDEO_METHOD == CDFS_PROTECTED_SKIP)
				{
					CDFS_VIDEO_SIZE+=pCur->FileSize;
				}
			}

			break;
		case SP5K_DCF_FILEOP_UNLOCKALL:
			appCdfsAllFileProcess(SP5K_DCF_FILEOP_UNLOCKALL);
			break;
		case SP5K_DCF_FILEOP_FORMAT:
			if(IS_CARD_EXIST)
			{
				sp5kDiskFormat(DRIVE_SD, "FAT32", 0); 
				sp5kDiskMount(DRIVE_SD);
			}
			else
			{
				sp5kDiskFormat(DRIVE_NAND, "FAT32", 0); 
				sp5kDiskMount(DRIVE_NAND);
			}
			
			appCdfsFileListTabInit(CDFS_FILE_FOLDER_VIDEO);
			appCdfsFileListTabInit(CDFS_FILE_FOLDER_JPG);
			appCdfsFileListTabInit(CDFS_FILE_FOLDER_EVENT);
            #if !CAM_TYPE_CVR
			appCdfsFileListTabInit(CDFS_FILE_FOLDER_AUDIO);
			appCdfsFolderInit(CDFS_FILE_FOLDER_AUDIO);
		    #endif
			appCdfsFolderInit(CDFS_FILE_FOLDER_EVENT);
			appCdfsFolderInit(CDFS_FILE_FOLDER_JPG);
			appCdfsFolderInit(CDFS_FILE_FOLDER_VIDEO);
			break;
		case SP5K_DCF_FILEOP_ABORT:
			break;
		case SP5K_DCF_FILEOP_COPY_BY_DIR:
			break;
		case SP5K_DCF_FILEOP_COPYALL_BY_DIR:
			break;
		default:
			break;
	}
	return ret;
}

UINT64 appCdfsCanDelVideoSizeGet(void)
{
	return CDFS_VIDEO_SIZE;/*unit: Byte*/
}

UINT32 appCdfsTheAllFileDel(UINT32 folder)
{
	CDFS_TAB_T *pCur, *pNext;
	UINT8 fileName[CDFS_FILE_NAME_LENGTH];
	UINT8 dirName[CDFS_FILE_NAME_LENGTH];
	UINT32 ret=SUCCESS;

	appCdfsFileFolderSet(folder);
	appCdfsFolderInit(folder);

	strcpy(dirName,sp5kFsGetCurPath());
	pCur = appCdfsFileListHeadGet(folder);

	while(pCur)
	{
		pNext = pCur->next;
		if((pCur->dcfattr & SP5K_DCF_FILE_ATTR_LOCK) && CDFS_DEL_VIDEO_METHOD == CDFS_PROTECTED_SKIP)
		{
			//protected will skip
		}
		else
		{
			CDFS_DBG("dirName = %s, pCur->FileName = %s\n",dirName, pCur->FileName);
			sprintf(fileName,"%s/%s",dirName,pCur->FileName);
			CDFS_DBG("Del file : %s\n",fileName);
			sp5kFsFileDelete(fileName);/*file name with dir path*/
			appCdfsFileDel(pCur->FileName);
		}

		if(pNext)
		{
			pCur = pNext;
		}
		else
		{
			break;
		}
	}

	return ret;
}

UINT32 appCdfsTheNewestFileDel(UINT32 folder)	//last file
{
	CDFS_TAB_T *pCur;
	UINT8 fileName[CDFS_FILE_NAME_LENGTH];
	UINT8 dirName[CDFS_FILE_NAME_LENGTH];
	UINT32 ret=SUCCESS;

	if(	folder == CDFS_FILE_FOLDER_VIDEO || folder == CDFS_FILE_FOLDER_JPG
		|| folder == CDFS_FILE_FOLDER_EVENT || folder == CDFS_FILE_FOLDER_AUDIO)
	{
		appCdfsFileFolderSet(folder);
		appCdfsFolderInit(folder);	
		
		strcpy(dirName,sp5kFsGetCurPath());
		pCur = appCdfsFileListLastGet(folder);
		if(pCur == NULL)
		{
			return FAIL;
		}
		
		if(CDFS_DEL_VIDEO_METHOD == CDFS_PROTECTED_SKIP)
		{
			while(1)
			{
				if(pCur->dcfattr & SP5K_DCF_FILE_ATTR_LOCK)
				{
					if(pCur->pre)
					{
						pCur = pCur->pre;
					}
					else
					{
						ret = FAIL;
						break;
					}
				}
				else
				{
					sprintf(fileName,"%s/%s",dirName,pCur->FileName);
					CDFS_DBG("Del file : %s\n",fileName);
					sp5kFsFileDelete(fileName);/*file name with dir path*/
					appCdfsFileDel(pCur->FileName);
					break;
				}
			}
		}
		else
		{
			sprintf(fileName,"%s/%s",dirName,pCur->FileName);
			sp5kFsFileDelete(fileName);/*file name with dir path*/
			appCdfsFileDel(pCur->FileName);
		}
	}
	else if(folder == CDFS_FILE_FOLDER_LAST)
	{
		CDFS_TAB_T *pVideoCur, *pJpgCur;
	
		pVideoCur = appCdfsFileListLastGet(CDFS_FILE_FOLDER_VIDEO);
		pJpgCur = appCdfsFileListLastGet(CDFS_FILE_FOLDER_JPG);

		if(pVideoCur != NULL  && pJpgCur != NULL)
		{
			if(strcmp(pVideoCur->FileName, pJpgCur->FileName) > 0)
			{
				return appCdfsTheNewestFileDel(CDFS_FILE_FOLDER_VIDEO);
			}
			else
			{
				return appCdfsTheNewestFileDel(CDFS_FILE_FOLDER_JPG);
			}
		}
		else if(pVideoCur != NULL  && pJpgCur == NULL)
		{
			return appCdfsTheNewestFileDel(CDFS_FILE_FOLDER_VIDEO);
		}
		else if(pVideoCur == NULL  && pJpgCur != NULL)
		{
			return appCdfsTheNewestFileDel(CDFS_FILE_FOLDER_JPG);
		}
		else
		{
			ret = FAIL;
		}		
	}

	return ret;
}

UINT32 appCdfsTheOldestFileDel(void)
{
	CDFS_TAB_T *pCur;
	UINT8 fileName[CDFS_FILE_NAME_LENGTH];
	UINT8 dirName[CDFS_FILE_NAME_LENGTH];
	UINT32 ret=SUCCESS;

	if(CDFS_FILE_FOLDER == CDFS_FILE_FOLDER_VIDEO)
	{
		strcpy(dirName,sp5kFsGetCurPath());
		pCur = appCdfsFileListHeadGet(CDFS_FILE_FOLDER_VIDEO);
		if(CDFS_DEL_VIDEO_METHOD == CDFS_PROTECTED_SKIP)
		{
			while(1)
			{
				if(pCur->dcfattr & SP5K_DCF_FILE_ATTR_LOCK)
				{
					if(pCur->next)
					{
						pCur = pCur->next;
					}
					else
					{
						ret = FAIL;
						break;
					}
				}
				else
				{
					sprintf(fileName,"%s/%s",dirName,pCur->FileName);
					CDFS_DBG("Del file : %s\n",fileName);
					sp5kFsFileDelete(fileName);/*file name with dir path*/
					appCdfsFileDel(pCur->FileName);
					break;
				}
			}
		}
		else
		{
			sprintf(fileName,"%s/%s",dirName,pCur->FileName);
			sp5kFsFileDelete(fileName);/*file name with dir path*/
			appCdfsFileDel(pCur->FileName);
		}
	}

	return ret;
}

UINT32 appCdfsFreeSpaceCheck(UINT32 folder, BOOL bOverWrite)
{	
	UINT32 sec,presec=0;
	CDFS_TAB_T *pCur;
	/*UINT8 fileName[CDFS_FILE_NAME_LENGTH];*/
	UINT8 dirName[CDFS_FILE_NAME_LENGTH];
	UINT32 ret=SUCCESS;
	if(folder != CDFS_FILE_FOLDER_VIDEO)
	{
		return SUCCESS;
	}
	
	if(appActiveStateGet() == APP_STATE_VIDEO_REC)
	{
		//sec = appVideoSeamLessSecGet()+50;/*space =2* time + 50 */
		sp5kMediaRecAttrGet(SP5K_MEDIA_ATTR_SEAMLESS_TIME_SLOT, &sec);
		presec = sec;
		sec += 50;
	}
	else
	{
		//sec = appVideoSeamLessSecGet()*3/2+50;/*space = 3*time + 50*/
		sp5kMediaRecAttrGet(SP5K_MEDIA_ATTR_SEAMLESS_TIME_SLOT, &sec);
		presec = sec;
		sec = sec * 3 / 2 + 50;
	}
	
	HLPrintf2("  remain time=%d sec,limit time%d sec\n",appVideoRemainSecGet(),sec);

	uiPara_t* puiPara = appUiParaGet();
	if(puiPara->ScdvMainMode == UI_SCDV_MAIN_MODE_SPORT)
	{
		if(appVideoRemainSecGet() < presec * 1.3)
		{
			sp5kHostMsgSend(SP5K_MSG_MEDIA_REC_STOP,0);//APP_KEY_PRESS_OK
			printf("--presec1=%d\n",presec);
		}
		else
			printf("--presec2=%d\n",presec);
	}

	if(appVideoRemainSecGet() > sec)
	{
		return SUCCESS;
	}
	else if(bOverWrite == TRUE)
	{
		printf("--File dele\n");
		strcpy(dirName,sp5kFsGetCurPath());
		pCur = appCdfsFileListHeadGet(CDFS_FILE_FOLDER_VIDEO);

		while(1)
		{
			if(appVideoRemainSecGet() < sec)
			{
				if(appCdfsCanDelVideoSizeGet())
				{
					RPrintf("--del the oldest file\n");				
					ret = appCdfsTheOldestFileDel();												
				}
				else
				{
					ret = FAIL;
				}
				
				if(ret == FAIL)
				{
					break;
				}
			}
			else
			{
				ret = SUCCESS;
				break;
			}
		}
	}
	
	return ret;
}


UINT32 appCdvCdfsFreeSpaceCheck(UINT32 folder, UINT32 nSecond,BOOL bOverWrite)
{	
	UINT32 sec,presec=0;
	CDFS_TAB_T *pCur;
	/*UINT8 fileName[CDFS_FILE_NAME_LENGTH];*/
	UINT8 dirName[CDFS_FILE_NAME_LENGTH];
	UINT32 ret=SUCCESS;

	if(folder != CDFS_FILE_FOLDER_VIDEO)
	{
		return SUCCESS;
	}
						
	sec=nSecond;						
	HLPrintf2("  remain time=%d sec,limit time%d sec\n",appVideoRemainSecGet(),sec);
	
	uiPara_t* puiPara = appUiParaGet();
	
	if(appVideoRemainSecGet() > sec)
	{
		return SUCCESS;
	}
	else if(bOverWrite == TRUE)
	{
		printf("--cdv File dele\n");
		strcpy(dirName,sp5kFsGetCurPath());
		pCur = appCdfsFileListHeadGet(CDFS_FILE_FOLDER_VIDEO);

		while(1)
		{
			if(appVideoRemainSecGet() < sec)
			{
				if(appCdfsCanDelVideoSizeGet())
				{
					RPrintf("--cdv del the oldest file\n");				
					ret = appCdfsTheOldestFileDel();					
					if(CDFS_NUM_VIDEO==0&&appVideoRemainSecGet() > sec)
						break;					
				}
				else
				{
					ret = FAIL;
				}
				
				if(ret == FAIL)
				{
					break;
				}
			}
			else
			{
				ret = SUCCESS;
				break;
			}
		}
	}
	
	return ret;
}


UINT32 appCdfsFileNameValidCheck(UINT32 filetype)
{
	UINT8 lastName[CDFS_FILE_NAME_LENGTH];
	UINT8 filename[CDFS_FILE_NAME_LENGTH];
	strcpy(lastName,appCdfsLastFileNameGet());
	appCdfsNextFileNameGet(filetype,filename);
	if(strcmp(lastName,filename) != 0)
	{
		return SUCCESS;
	}
	else
	{
		return FAIL;
	}
}


void appCdfsInit(void)
{
	UINT8 path[CDFS_FILE_NAME_LENGTH]={0};
	
	appBtnDisable(BTN_ALL);
	//appOsdMessageBoxShow(TRUE, ID_STR_WAITING___);
	
	#if SP5K_DISK_SPI
	
	if(IS_CARD_EXIST)
	{
		appCdfsFilePoolInit();
		
        	#if !CAM_TYPE_CVR
		/*AUDIO*/
		appCdfsFolderInit(CDFS_FILE_FOLDER_AUDIO);
		appCdfsFolder2Path(CDFS_FILE_FOLDER_AUDIO,path);
		sp5kFsDirCacheSet(path, 1);/*speed up*/
		appCdfsCurrFolderFileListCreate(CDFS_FILE_FOLDER_AUDIO, path);
		sp5kFsDirCacheSet(path, 0);
		#endif
		
		uiPara_t* puiPara = appUiParaGet();
		if(puiPara->ScdvMainMode == UI_SCDV_MAIN_MODE_CAR)
		{
			/*EVENT*/
			appCdfsFolderInit(CDFS_FILE_FOLDER_EVENT);
			appCdfsFolder2Path(CDFS_FILE_FOLDER_EVENT,path);
			sp5kFsDirCacheSet(path, 1);/*speed up*/
			appCdfsCurrFolderFileListCreate(CDFS_FILE_FOLDER_EVENT, path);
			sp5kFsDirCacheSet(path, 0);
		}
		
		/*JPG*/
		appCdfsFolderInit(CDFS_FILE_FOLDER_JPG);
		appCdfsFolder2Path(CDFS_FILE_FOLDER_JPG,path);
		sp5kFsDirCacheSet(path, 1);
		appCdfsCurrFolderFileListCreate(CDFS_FILE_FOLDER_JPG, path);
		sp5kFsDirCacheSet(path, 0);

		/*VIDEO*/
		appCdfsFolderInit(CDFS_FILE_FOLDER_VIDEO);
		appCdfsFolder2Path(CDFS_FILE_FOLDER_VIDEO, path);
		sp5kFsDirCacheSet(path, 1);
		appCdfsCurrFolderFileListCreate(CDFS_FILE_FOLDER_VIDEO, path);
		sp5kFsDirCacheSet(path, 0);
	}
	else
	{
		appCdfsFileListTabInit(CDFS_FILE_FOLDER_VIDEO);
		appCdfsFileListTabInit(CDFS_FILE_FOLDER_JPG);
		appCdfsFileListTabInit(CDFS_FILE_FOLDER_EVENT);
        	#if !CAM_TYPE_CVR
		appCdfsFileListTabInit(CDFS_FILE_FOLDER_AUDIO);
        	#endif
	}
	
	#else
	
	appCdfsFilePoolInit();
	appCdfsFolderInit(CDFS_FILE_FOLDER_EVENT);
	appCdfsFolderInit(CDFS_FILE_FOLDER_JPG);
	appCdfsFolderInit(CDFS_FILE_FOLDER_VIDEO);
	
        #if !CAM_TYPE_CVR
	appCdfsFolderInit(CDFS_FILE_FOLDER_AUDIO);
	/*AUDIO*/
	appCdfsFolder2Path(CDFS_FILE_FOLDER_AUDIO,path);
	sp5kFsDirCacheSet(path, 1);/*speed up*/
	appCdfsCurrFolderFileListCreate(CDFS_FILE_FOLDER_AUDIO, path);
	sp5kFsDirCacheSet(path, 0);
	#endif
	
	/*EVENT*/
	appCdfsFolder2Path(CDFS_FILE_FOLDER_EVENT,path);
	sp5kFsDirCacheSet(path, 1);/*speed up*/
	appCdfsCurrFolderFileListCreate(CDFS_FILE_FOLDER_EVENT, path);
	sp5kFsDirCacheSet(path, 0);
	
	/*JPG*/
	appCdfsFolder2Path(CDFS_FILE_FOLDER_JPG,path);
	sp5kFsDirCacheSet(path, 1);
	appCdfsCurrFolderFileListCreate(CDFS_FILE_FOLDER_JPG, path);
	sp5kFsDirCacheSet(path, 0);
	
	/*VIDEO*/
	appCdfsFolder2Path(CDFS_FILE_FOLDER_VIDEO, path);
	sp5kFsDirCacheSet(path, 1);
	appCdfsCurrFolderFileListCreate(CDFS_FILE_FOLDER_VIDEO, path);
	sp5kFsDirCacheSet(path, 0);
	
	#endif
	
	//appOsdMessageBoxShow(FALSE, ID_STR_WAITING___);
	switch( STATE_GROUP(appActiveStateGet()) )
	{
		case APP_STATE_GROUP_VIEW:
			appCdfsFileFolderSet(CDFS_FILE_FOLDER_JPG);
			appCdfsFolderInit(CDFS_FILE_FOLDER_JPG);
			break;
		case APP_STATE_GROUP_VIDEO:
			appCdfsFileFolderSet(CDFS_FILE_FOLDER_VIDEO);
			appCdfsFolderInit(CDFS_FILE_FOLDER_VIDEO);
			break;
		case APP_STATE_GROUP_AUDIO:
			appCdfsFileFolderSet(CDFS_FILE_FOLDER_AUDIO);
			appCdfsFolderInit(CDFS_FILE_FOLDER_AUDIO);
			break;
		default:
			break;
	}
	appBtnEnable(BTN_ALL);
}
 
void appCdfsFileListSecondCreate()
{
	UINT8 path[CDFS_FILE_NAME_LENGTH]={0};	
	printf("--appCdfsFileListSecondCreate\n");
	appBtnDisable(BTN_ALL);
	if(IS_CARD_EXIST)
	{
		appCdfsFilePoolInit();
		/*VIDEO*/
		appCdfsFolderInit(CDFS_FILE_FOLDER_VIDEO);
		appCdfsFolder2Path(CDFS_FILE_FOLDER_VIDEO, path);
		sp5kFsDirCacheSet(path, 1);
		appCdfsCurrFolderFileListCreate(CDFS_FILE_FOLDER_VIDEO, path);
		sp5kFsDirCacheSet(path, 0);
		
	}
	appBtnEnable(BTN_ALL);
}