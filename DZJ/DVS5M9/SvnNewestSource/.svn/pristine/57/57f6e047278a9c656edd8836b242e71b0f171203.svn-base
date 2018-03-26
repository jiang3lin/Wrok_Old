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
#include "app_view_param.h"
#include "app_util.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define PRM_PATH "B:\\USER_RES\\"

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

/**************************************************************************
 *                          L O C A L    D A T A                          *
 **************************************************************************/

/* general param */

static void _UiParamReset(ui_param_t *param, UINT8 size)
{
	UINT32 i;
	for (i=0 ; i<size ; i++) 
	{
		if (param->info[i].flag & (PRM_SHARE|PRM_NOUSE)) 
		{
			continue;
		}
		param->value[i] = param->info[i].defVal;
	}
}

static void _UiParamLoadFile(ui_param_t *param, UINT8 size)
{
	UINT8 i,sum,buf[size+1];
	UINT8 fname[sizeof(PRM_PATH)+20];
	sprintf(fname, PRM_PATH "%s", param->fname);
	if (ReadFile(fname, buf, size+1)!=SUCCESS)
	{
		return;
	}
	for (sum=i=0 ; i<size ; i++)
	{
		sum += buf[i];
	}
	
	if (sum!=buf[size]) 
	{
		printf("checksum err:data:%x file:%x\n", sum, buf[size]);
		_UiParamReset(param,size);
	    return;
	}
	
	for (i=0 ; i<size ; i++) 
	{
		if (param->info[i].flag & PRM_SAVE)
			param->value[i] = buf[i];
	}
}

static void _UiParamSaveFile(ui_param_t *param, UINT8 size)
{
	UINT8 i,sum,buf[size+1];
	UINT8 fname[sizeof(PRM_PATH)+20];
	sprintf(fname, PRM_PATH "%s", param->fname);
	for (sum=i=0 ; i<size ; i++) 
	{
		buf[i] = param->value[i];
		sum += buf[i];
	}
	buf[size] = sum;
	WriteFile(fname, buf, size+1);
}

static void _UiParamInit(ui_param_t *param, UINT8 size)
{
	UINT32 i;
	for (i=0 ; i<size ; i++) 
	{
		if (param->info[i].flag & (PRM_SHARE|PRM_NOUSE)) 
		{
			continue;
		}
		
		if (param->info[i].flag & PRM_NOSEL)
		{
			param->info[i].availMask = 1<<param->info[i].defVal;
		}
		else if (param->info[i].availMask==0)
		{
			param->info[i].availMask = 0xffff;
		}
	}
	_UiParamReset(param, size);
	_UiParamLoadFile(param, size);
}

static void _UiParamBackup(UINT8 *param, ui_param_t *paramSet, UINT8 paramMode, UINT8 size)
{
	UINT32 i,mode,flag;
	for (i=0 ; i<size ; i++) 
	{
		mode = (paramSet[paramMode].info[i].flag & PRM_SHARE) 
				? (paramSet[paramMode].info[i].flag & 0x7f) : paramMode;
		flag = paramSet[mode].info[i].flag;
		if (flag & (PRM_NOUSE|PRM_NOSEL))
		{
			continue;
		}
		paramSet[mode].value[i] = param[i];
	}
}

static void _UiParamRestore(UINT8 *param, ui_param_t *paramSet, UINT8 paramMode, UINT8 size)
{
	UINT32 i,mode,flag;
	for (i=0 ; i<size ; i++) 
	{
		mode = (paramSet[paramMode].info[i].flag & PRM_SHARE) 
				? (paramSet[paramMode].info[i].flag & 0x7f) : paramMode;
		flag = paramSet[mode].info[i].flag;
		param[i] = paramSet[mode].value[i];
	}
}

static void _UiParamGetInfo(ui_param_t *paramSet, UINT8 paramMode, UINT8 paramId, param_info_t *info)
{
	UINT32 mode;
	mode = (paramSet[paramMode].info[paramId].flag & PRM_SHARE) 
			? (paramSet[paramMode].info[paramId].flag & 0x7f) : paramMode;
	memcpy(info, paramSet[mode].info+paramId, sizeof(param_info_t));
}

/* view param */
extern ui_param_t ViewParamSet[];

UINT32 IsUiParamViewInit=0;
UINT8 ViewParamMode=0xff;
view_param_t ViewParam,*pViewParam=&ViewParam;

void UiParamViewGetInfo(UINT8 id, param_info_t *info)
{
	if (ViewParamMode==0xff)
	{
		return;
	}
	_UiParamGetInfo(ViewParamSet, ViewParamMode, id, info);
}

void UiParamViewChangeMode(UINT8 paramMode)
{
	if(IsUiParamViewInit==0)
	{
		UiParamViewInit();
	}
		
	if (ViewParamMode==paramMode) 
	{
		return;
	}
	if (ViewParamMode!=0xff)
	{
		_UiParamBackup((UINT8*)pViewParam, ViewParamSet, ViewParamMode, VIEW_PRM_COUNT);
	}
	if (paramMode!=0xff)
	{
		_UiParamRestore((UINT8*)pViewParam, ViewParamSet, paramMode, VIEW_PRM_COUNT);
	}
	ViewParamMode = paramMode;
}

UINT8 ViewParamBuf[VIEW_PRM_COUNT*PRM_MODE_MAX];
UINT8 ViewParamBufMirror[VIEW_PRM_COUNT*PRM_MODE_MAX];

void UiParamViewInit(void)
{
	UINT32 i;
	memset(ViewParamBuf, 0, sizeof(ViewParamBuf));
	for (i=0 ; i<PRM_MODE_MAX ; i++) 
	{
		ViewParamSet[i].value = ViewParamBuf + i*VIEW_PRM_COUNT;
		_UiParamInit(ViewParamSet+i, VIEW_PRM_COUNT);
	}
	IsUiParamViewInit=1;
	memcpy(ViewParamBufMirror, ViewParamBuf, sizeof(ViewParamBuf));
}

void UiParamViewFlush(void)
{
	UINT32 i;
	if (ViewParamMode!=0xff)
	{
		_UiParamBackup((UINT8*)pViewParam, ViewParamSet, ViewParamMode, VIEW_PRM_COUNT);
	}
	
	if(memcmp(ViewParamBufMirror, ViewParamBuf, sizeof(ViewParamBuf))==0) 
	{
		return;
	}
	memcpy(ViewParamBufMirror, ViewParamBuf, sizeof(ViewParamBuf));
	
	for (i=0 ; i<PRM_MODE_MAX ; i++)
	{
		_UiParamSaveFile(ViewParamSet+i, VIEW_PRM_COUNT);
	}
}

void UiParamViewFini(void)
{
	UiParamViewFlush();
	ViewParamMode = 0xff;
}

void UiParamViewReset(void)
{
	UINT32 i;

	if(IsUiParamViewInit==0)
	{
		UiParamViewInit();
	}

	for (i=0 ; i<PRM_MODE_MAX ; i++) 
	{
		_UiParamReset(ViewParamSet+i, VIEW_PRM_COUNT);
	}
	
	if (ViewParamMode!=0xff)
	{
		_UiParamRestore((UINT8*)pViewParam, ViewParamSet, ViewParamMode, VIEW_PRM_COUNT);
	}
}


view_param_t* appViewParamGet(void)
{
	return pViewParam;
}

