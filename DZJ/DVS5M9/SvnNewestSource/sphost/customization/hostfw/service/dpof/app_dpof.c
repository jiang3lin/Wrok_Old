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
#include "app_dpof.h"
#include "sp5k_dpof_api.h"
#include "sp5k_global_api.h"
#include "app_cdfs_api.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef struct appDpofCtrl_s {
	BOOL isInit;
	BOOL isUpdated;
} appDpofCtrl_t;
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
static appDpofCtrl_t dpofCtrl = {FALSE, FALSE};

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
BOOL
_dpofJobQuery(
	appDpofMode_e mode
)
{	
	#if !SP5K_CDFS_OPT
	if(sp5kDpofJobQuery(mode) == SUCCESS) {
		return TRUE;
	}
	#endif
	return FALSE;
}

void
_dpofHeaderUpdate(
	void
)
{
	unsigned char dpofTm[32];
	tmx_t time;
	sp5kRtcDateTimeGet(SP5K_DATE_TIME_OPTION, &time);
	sprintf(dpofTm,"%04d:%02d:%02d:%02d:%02d:%02d",
						time.tmx_year+1900,
						time.tmx_mon,
						time.tmx_mday,
						time.tmx_hour,
						time.tmx_min,
						time.tmx_sec
	);
	sp5kDpofFileAttrSet(SP5K_DPOF_PARAMID_GEN_DTM,dpofTm);
}

void
appDpofInit(
	void
)
{
	DBG_PRINT("\n\n[**DPOF**]%s enter\n\n",__FUNCTION__);
	
	if(dpofCtrl.isInit) {
		printf("[WARN] : %s(%d)\n",__FUNCTION__,__LINE__);
		return;
	}	
	
	sp5kDpofInit(APP_DPOF_CRT_NAME);
	sp5kDpofFileAttrSet(SP5K_DPOF_PARAMID_GEN_CRT,APP_DPOF_CRT_NAME);
	dpofCtrl.isInit = TRUE;
}

void
appDpofClose(
	BOOL write2File
)
{
	DBG_PRINT("\n\n[**DPOF**]%s(%d) enter\n\n",__FUNCTION__,write2File);

	if(!dpofCtrl.isInit) 
	{
		printf("[WARN] : %s(%d)\n",__FUNCTION__,__LINE__);
		return;
	}		
	if(write2File) 
	{
		sp5kDpofFileWrite();			
		dpofCtrl.isUpdated = FALSE;	
	}
	sp5kDpofClose();
	dpofCtrl.isInit = FALSE;
}

void
appDpofFileRemove(
	UINT32 isCardUsed
)
{
	DBG_PRINT("[**DPOF**]%s enter",__FUNCTION__);

	if( isCardUsed )	{
		sp5kFsFileDelete(DPOF_FILENAME_SDCARD);
	}
	else {
		sp5kFsFileDelete(DPOF_FILENAME_NAND);
	}
}

BOOL
appDpofJobExistQuery(
	appDpofMode_e dpofMode
)
{		
	//DBG_PRINT("[**DPOF**]%s enter",__FUNCTION__);

	return (_dpofJobQuery(dpofMode));
}



UINT32
appDpofPrintQuantityGet(
	appDpofMode_e dpofMode
)
{
	UINT32 printQty = 0;

	//DBG_PRINT("[**DPOF**]%s enter",__FUNCTION__);
	
	if(_dpofJobQuery(dpofMode)) {
		sp5kDpofJobAttrGet(dpofMode,SP5K_DPOF_PARAMID_PRT_QTY,(void *)&printQty);		
	}
	return printQty;
}

BOOL
appDpofDateStampExistQuery(
	appDpofMode_e dpofMode
)
{

	UINT8 dateStamp[32];

	//DBG_PRINT("[**DPOF**]%s enter",__FUNCTION__);

	if (_dpofJobQuery(dpofMode)){
		sp5kDpofJobAttrGet(dpofMode,SP5K_DPOF_PARAMID_CFG_DSC_AD,(void *)dateStamp);		
		DBG_PRINT("dateStamp=[%s]@0x%08x \n", dateStamp, (UINT32)dateStamp);
		return (dateStamp[0]=='\0' ? FALSE : TRUE);
	}
	return FALSE;
}

UINT32
appDpofDateStampGet(
	appDpofMode_e dpofMode,
	UINT8 *pszDTBuf
)
{
	int i;
	UINT8 dateStamp[32];

	//DBG_PRINT("[**DPOF**]%s enter",__FUNCTION__);

	if (_dpofJobQuery(dpofMode)){
		sp5kDpofJobAttrGet(dpofMode,SP5K_DPOF_PARAMID_CFG_DSC_AD,(void *)dateStamp);
		DBG_PRINT("dateStamp=[%s] \n", dateStamp);
		if (dateStamp) {
			i = strlen(dateStamp);
			if (i<24) {
				memcpy(pszDTBuf, dateStamp, i);
				*(pszDTBuf+i)='\0';
				return SUCCESS;
			}
		}
	}
	*pszDTBuf='\0';
	return FAIL;
}

UINT32
appDpofPrintSizeGet(
	void
)
{
	UINT32 size = 0;

	//DBG_PRINT("[**DPOF**]%s enter",__FUNCTION__);

	if (_dpofJobQuery(APP_DPOF_MODE_SIZ))
	{
		sp5kDpofJobAttrGet(APP_DPOF_MODE_SIZ,SP5K_DPOF_PARAMID_PRT_TYP,(void *)&size);				
	}
	return size;
}

UINT32 
appDpofJobDelete(
	appDpofMode_e dpofMode,
	appDpofJobOption_e jobOption
)
{
	dpofCtrl.isUpdated = TRUE;

	DBG_PRINT("[**DPOF**]%s enter",__FUNCTION__);

	return(sp5kDpofJobDelete(dpofMode,jobOption));
}


UINT32
appDpofStdJobAdd(
	UINT32 printQty,
	UINT8 *dateStamp
)
{
	DBG_PRINT("[**DPOF**]%s enter",__FUNCTION__);

	_dpofHeaderUpdate();
	if (_dpofJobQuery(APP_DPOF_MODE_STD)== 0) 
	{
		if(sp5kDpofJobAdd(SP5K_DPOF_PRINT_MODE_STD) != SUCCESS)
		{
			return FAIL;
		}
	}
	sp5kDpofJobAttrSet(SP5K_DPOF_PRINT_MODE_STD,SP5K_DPOF_PARAMID_CFG_DSC_AD,(void*)dateStamp);
	sp5kDpofJobAttrSet(SP5K_DPOF_PRINT_MODE_STD, SP5K_DPOF_PARAMID_PRT_QTY, (void*)&printQty);
	dpofCtrl.isUpdated = TRUE;
	return SUCCESS;
}

