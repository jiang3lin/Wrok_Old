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
#include "app_dcf.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define MAX(a, b)		( (a) > (b) ? (a) : (b) )
#define IS_DCF_KEY_MAX(dir, file)  ((dir == DCF_DIR_NUM_MAX) && (file == DCF_FILE_NUM_MAX))
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef struct dcfKeyInfo_s 
{
	BOOL dcfKeyFull;	//999-9999
	BOOL createDir;
	UINT16 nextDirKey;
	UINT16 nextFileKey;
} dcfKeyInfo_t;

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
static dcfKeyInfo_t dcfKey = {FALSE,FALSE,0,0};

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/*-------------------------------------------------------------------------
 *  File Name : appDcfDefaultFreeCharGet
 *------------------------------------------------------------------------*/
void
_dcfDefaultFreeCharGet(
	UINT8 *szdir,
	UINT8 *szfile
)
{	
	if ((szdir == NULL) ||(szfile == NULL))
	{
		printf("[ERR] : %s (%d)\n",__FUNCTION__,__LINE__);
		return;
	}
	
	memcpy(szdir,  DCF_DIR_FREE_CHAR,  6);
	memcpy(szfile, DCF_FILE_FREE_CHAR, 6);
	szdir[5] = 0;
	szfile[5] = 0;
}

void
_dcfKeyGet(
	UINT16 *dirKey,
	UINT16 *fileKey
)
{
	UINT32 totalFile;
	UINT8  fname[32];
       sp5kDcfAttrElm_t fileAttr;
	   
	sp5kDcfFsInfoGet(SP5K_DCF_INFO_TOTAL_FILENUM,&totalFile);
	if(totalFile)
	{
		sp5kDcfFsFileAttrGet(totalFile, &fileAttr,fname );
		*dirKey =  fileAttr.dirNum;
		*fileKey =  fileAttr.fileNum;
	}
	else 
	{
		*dirKey = 0;
		*fileKey = 0;
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appDcfInit
 *------------------------------------------------------------------------*/
UINT32
appDcfInit(
	UINT32 drive
)
{	
    #if 0   //for clean warning
	UINT32 fnumRule;
    #endif
	UINT32 totalFile;
	UINT16 dirKey, fileKey;
	UINT8 dirChar[6],fileChar[6];
	UINT8  fname[32], tmpBuff[6];
       sp5kDcfAttrElm_t fileAttr;

	//dcf init
	if (FAIL==sp5kDcfFsInit(drive,0,0)) 
	{
		return FAIL;
	}
	sp5kDcfFsActive(drive);
	//free char set
	_dcfDefaultFreeCharGet(dirChar,fileChar);
	sp5kDcfFsFreeCharSet(dirChar, fileChar, SP5K_DCF_NEXTKEY_MAX);
	//
	sp5kDcfFsInfoGet(SP5K_DCF_INFO_TOTAL_FILENUM,&totalFile);
	if(totalFile) 
	{
		memset(tmpBuff ,0,sizeof(tmpBuff ));
		sp5kDcfFsFileAttrGet(totalFile, &fileAttr,fname );
		memcpy(tmpBuff,&fileAttr.dirname[3],5);
		dirKey =  fileAttr.dirNum;
		fileKey =  fileAttr.fileNum;
		DBG_PRINT("[DCF] exist DCF key (%d,%d) - (%s)\n",dirKey,fileKey,fileAttr.dirname);
		DBG_PRINT("[DCF] exist DCF dir char (%s,%s)\n",dirChar,fileAttr.dirname);

		//check if 999-9999 already?
		if(IS_DCF_KEY_MAX(dirKey,fileKey))
		{
			dcfKey.dcfKeyFull = TRUE;
			DBG_PRINT("[DCF] No more free DCF key!!\n");
		}		
		//check free char of max dir
		else if(strcmp(dirChar, tmpBuff) != 0) 
		{
			if (dirKey == DCF_DIR_NUM_MAX) 
			{
				dcfKey.dcfKeyFull = TRUE;
				DBG_PRINT("[DCF] No more free DCF key!!\n");
			}
			else{
				dcfKey.createDir = TRUE;
				DBG_PRINT("[DCF] Create new dir!!!\n");
			}
		}
	}
	#if 0
	appDcfUserSettingGet(&fnumRule,&dirKey,&fileKey);
	appDcfNexDcfKeySet((fnumRule == FILENUM_RESET),dirKey,fileKey);
	#endif
	return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  File Name : appDcfNexDcfKeySet
 *------------------------------------------------------------------------*/
UINT32
appDcfNexDcfKeySet(
	UINT32 isReset,
 	UINT16 dirKey, 
 	UINT16 fileKey
)
{
	UINT16 existDirKey,existFileKey;
	UINT16 newDirKey,newFileKey;


	DBG_PRINT("%s (%d,%d,%d)\n",__FUNCTION__,isReset,dirKey,fileKey);

	newDirKey = dirKey;
	newFileKey = fileKey;

	if (dcfKey.dcfKeyFull) 
	{
		return FAIL;
	}

	_dcfKeyGet(&existDirKey,&existFileKey);

	//if exist dcf key is larger than input config key 
	if (newDirKey < existDirKey)
	{
		newDirKey = existDirKey;
		newFileKey = existFileKey;
	} 	
	else if (newDirKey == existDirKey) 
	{
		newFileKey = MAX(newFileKey,existFileKey);
	}
	else if (isReset) 
	{ //if file num setting is RESET and exist dcf key is smaller than input config key
		newDirKey = existDirKey;
		newFileKey = existFileKey;
	}

	if (newDirKey< DCF_DIR_NUM_MIN) 
	{
		newDirKey = DCF_DIR_NUM_MIN;
	}
	
	if(IS_DCF_KEY_MAX(newDirKey,newFileKey))
	{
		dcfKey.dcfKeyFull = TRUE;
		DBG_PRINT("[DCF] No more free DCF key!!\n");
		return FAIL;

	}

	newFileKey ++;
	if((newFileKey > DCF_FILE_NUM_MAX) || dcfKey.createDir) 
	{
		dcfKey.createDir = FALSE;
		newDirKey ++;
		newFileKey = DCF_FILE_NUM_MIN;			
	}
	
	if(IS_DCF_KEY_MAX(newDirKey,newFileKey))
	{
		dcfKey.dcfKeyFull = TRUE;
	}

	DBG_PRINT("next DCF KEY set (%d,%d)",newDirKey, newFileKey);

	dcfKey.nextDirKey = newDirKey;
	dcfKey.nextFileKey = newFileKey;

	sp5kDcfFsNextKeySet(newDirKey, newFileKey);
	
	return SUCCESS;

}
 	
 


/*-------------------------------------------------------------------------
 *  File Name : appDcfKeyFullCkeck
 *------------------------------------------------------------------------*/
UINT32
appDcfKeyFullCkeck(
	void
)
{
	UINT16 existDirKey,existFileKey;

    	_dcfKeyGet(&existDirKey,&existFileKey);

	if(IS_DCF_KEY_MAX(existDirKey,existFileKey))
	{
		return TRUE;
	}
	else if (dcfKey.dcfKeyFull)
	{
		return TRUE;
	}		
	return FALSE;
}
/*-------------------------------------------------------------------------
 *  File Name : appDcfRemainKeyQuery
 *------------------------------------------------------------------------*/
BOOL
appDcfRemainKeyQuery(
 	UINT32 requestNum 
)
{
	UINT16 existDirKey,existFileKey;
	_dcfKeyGet(&existDirKey, &existFileKey);
	if (((existFileKey + requestNum -1) > DCF_FILE_NUM_MAX) 
		&& (existDirKey == DCF_DIR_NUM_MAX)) 
	{
		return FALSE;
	}
	return TRUE;
}

/*-------------------------------------------------------------------------
 *  File Name : appDcfKeySettingUpdate
 *------------------------------------------------------------------------*/
void
appDcfKeySettingUpdate(
	UINT16 *dirKey,
	UINT16 *fileKey
)
{
	UINT16 nextDirKey,nextFileKey;
	sp5kDcfFsNextKeyGet(&nextDirKey, &nextFileKey);
	DBG_PRINT("next DCF KEY get (%d,%d)",nextDirKey, nextFileKey);
	if ((nextDirKey == dcfKey.nextDirKey) && (nextFileKey == dcfKey.nextFileKey))
	{
		DBG_PRINT("DCF key setting update : do nothing, no file is created\n");
		return;
	}
	_dcfKeyGet(dirKey,fileKey);
	DBG_PRINT("DCF key setting update (%d,%d)\n",*dirKey,*fileKey);
}



