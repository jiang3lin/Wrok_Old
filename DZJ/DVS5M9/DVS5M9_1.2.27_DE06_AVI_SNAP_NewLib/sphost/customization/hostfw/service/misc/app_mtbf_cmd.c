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
 * @file		app_mtbf_cmd.c
 * @brief		App for MTBF test
 * @author	GQ
 * @since	2013-06-17
 * @date		2013-06-19
 */
#include "app_sys_def.h" 
#if MTBF_DBG_EN
#include "app_com_def.h"
#include "app_com_api.h"
#include "app_mtbf_cmd.h"
#include "app_ui_para.h"
#include "app_video.h"
#include "app_still.h"
#include "app_view_param_def.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define MTBF_CMD_NUM_MAX 10

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
UINT32 mtbf_mode = 0;  /*  0:single run 1:loop run */ 
static TX_THREAD *ptrdMtbf;
static TX_MUTEX mtbfLock;
static TX_MUTEX mtbfCmdTblLock;

static TX_EVENT_FLAGS_GROUP mtbfTaskEvt;
static TX_EVENT_FLAGS_GROUP mtbfSnapEvt;

static mtbf_cmd_t mtbfCmdTbl[MTBF_CMD_NUM_MAX];
static UINT32 curCmdIdx=0;
static UINT32 mtbfLoopCnt = 0;

mtbf_cmd_name_t mtbfCmdName[] = {
	[MTBF_CMD_NULL]		= {"null",appMTBFNull},
	[MTBF_VIDEO_SIZE] 	= {"vid",appMTBFVideoSizeSet},
	[MTBF_VIDEO_REC] 	= {"rec",appMTBFVideoRec},
	[MTBF_STILL_SIZE]	= {"img",appMTBFImgSizeSet}, 	
	[MTBF_STILL_SNAP] 	= {"snap",appMTBFStillCapture},
	[MTBF_PB_MAIN] 		= {"pb",appMTBFPB},
	[MTBF_FILE_DEL]		= {"dele",appMTBFFileDel},
};

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern UINT32 SnapSetflag; 
extern void appVideo_SetSeamLess(UINT8 idx);
extern UINT32 appVideoCheckReady(UINT32 question);
extern UINT32 appStillCheckReady(UINT32 question);
extern UINT32 sp5kPbTotalFileNumGet(UINT32 *pnum);
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
static void appMTBFVideoRecIsrFunc(void);

void appMTBFCmdShow(void)
{
	UINT32 i;
	printf("\nMTBF CMD: ");

	for(i=0;i<MTBF_CMD_MAX;i++){
		printf("'%s' ",mtbfCmdName[i].cmdName);
	}
	printf("'start'");
	printf("\n");
	printf("Input 'start' to start test\n");
	//printf("Try 'cmd help' for more information.\n");
}

void appMTBFCmdHelp(UINT32 cmdID)
{
	switch(cmdID){
		case MTBF_VIDEO_SIZE:
			printf("usage:%s option\n",mtbfCmdName[MTBF_VIDEO_SIZE].cmdName);
			printf("	  1080p\n");
			printf("	  720p 30\n");
			printf("	  720p 60\n");
			printf("	  vga\n");
			printf("	  qvga\n");
			printf("ex:%s 1080p\n",mtbfCmdName[MTBF_VIDEO_SIZE].cmdName);
			break;
		case MTBF_VIDEO_REC:
			printf("If you not specify the record time,it will recorded until you manually stop.\n");
			printf("ex:%s 5 (record 5 minutes)\n",mtbfCmdName[MTBF_VIDEO_REC].cmdName);
			sp5kTimeDelay(SP5K_TIME_DELAY_1MS,2000);
			break;
		case MTBF_STILL_SIZE:
			printf("usage:%s option\n",mtbfCmdName[MTBF_STILL_SIZE].cmdName);
			printf("	  10m\n");
			printf("	  9m\n");
			printf("	  8m\n");
			printf("	  7m\n");
			printf("	  5m\n");
			printf("	  3m\n");
			printf("	  2m\n");
			printf("	  1m\n");
			printf("	  vga\n");
			printf("ex:%s 8m\n",mtbfCmdName[MTBF_STILL_SIZE].cmdName);
			break;
		case MTBF_STILL_SNAP:
			printf("ex:%s 3(shot 3 photos)\n",mtbfCmdName[MTBF_STILL_SNAP].cmdName);
			break;
		case MTBF_PB_MAIN:
			printf("switch to pb mode \n");
			break;
	}
}

void appMTBFNull(UINT32 temp)
{
	temp = temp;
	
	if(mtbf_mode){
		appMTBFJobFinEvtPut();
	}
}
void appMTBFVideoSizeSet(UINT32 size)
{
	uiPara_t* puiPara = appUiParaGet();

	puiPara->VideoSize = size;
	
	appStateChange(APP_STATE_VIDEO_PREVIEW,STATE_PARAM_NORMAL_INIT);
	APP_SLEEP_MS(100);
	if(mtbf_mode){
		sp5kHostMsgSend(APP_UI_MSG_MTBF_JOB_DONE,0);
	}
}

static void appMTBFVideoRecIsrFunc(void)
{
	SnapSetflag = 0;
	if(appActiveStateGet() == APP_STATE_VIDEO_REC){
		sp5kHostMsgSend(APP_KEY_PRESS_S1 , 0);
	}
	if(mtbf_mode){
		sp5kHostMsgSend(APP_UI_MSG_MTBF_JOB_DONE,0);
	}
}

/* time: minute */
void appMTBFVideoRec(UINT32 time)
{
	uiPara_t* puiPara = appUiParaGet();
	UINT32 timeMs = time*60*1000;
	UINT32 actState = appActiveStateGet();
	
	puiPara->Seamless = UI_VIDEO_SEAMLESS_3MIN;
	appVideo_SetSeamLess(puiPara->Seamless);

	if(actState != APP_STATE_VIDEO_PREVIEW){
		appStateChange(APP_STATE_VIDEO_PREVIEW,STATE_PARAM_NORMAL_INIT);
		APP_SLEEP_MS(100);	
	}

	if(appVideoCheckReady(CAN_I_START_RECORD)==VIDEO_STATE_YES){
		SnapSetflag = 1;
		sp5kHostMsgSend(APP_KEY_PRESS_S1 , 0);
		if(timeMs){
			sp5kTimerIsrOneShotReg(0,timeMs,appMTBFVideoRecIsrFunc);
		}
	}else{
		sp5kHostMsgSend(APP_KEY_PRESS_S1 , 0); /* only use for show err msg */
		sp5kHostMsgSend(APP_UI_MSG_MTBF_TASK_DEL,0);
	}
}

void appMTBFImgSizeSet(UINT32 size)
{
	uiPara_t* puiPara = appUiParaGet();

	puiPara->ImageSize = size;
	
	appStateChange(APP_STATE_STILL_PREVIEW,STATE_PARAM_NORMAL_INIT);
	APP_SLEEP_MS(100);
	if(mtbf_mode){
		sp5kHostMsgSend(APP_UI_MSG_MTBF_JOB_DONE,0);
	}
}

void appMTBFStillCapEvtPut(void)
{
	if(mtbfSnapEvt){
		sp5kOsEventFlagsSet(&mtbfSnapEvt,1,TX_OR);
	}
}

void appMTBFStillCapture(UINT32 num)
{
	ULONG flagsPtr;

	if(!num){
		if(mtbf_mode){
			sp5kHostMsgSend(APP_UI_MSG_MTBF_JOB_DONE,0);
		}
		return;
	}
	
	if(appActiveStateGet() != APP_STATE_STILL_PREVIEW){
		appStateChange(APP_STATE_STILL_PREVIEW,STATE_PARAM_NORMAL_INIT);
		APP_SLEEP_MS(100);
	}

	sp5kOsEventFlagsCreate(&mtbfSnapEvt,"mtbfSnapDoneEvt");
	
	while(num){
		SnapSetflag = 1;
		if(appActiveStateGet() == APP_STATE_STILL_PREVIEW){
			if(appStillCheckReady(CAN_I_START_SNAP)!=STILL_STATE_YES){
				SnapSetflag = 0;
				sp5kOsEventFlagsDelete(&mtbfSnapEvt);
				sp5kHostMsgSend(APP_KEY_PRESS_S1,0);
				sp5kHostMsgSend(APP_UI_MSG_MTBF_TASK_DEL,0);
				return;
			}
			sp5kHostMsgSend(APP_KEY_PRESS_S1,0);
			sp5kOsEventFlagsGet(&mtbfSnapEvt,1,TX_OR_CLEAR,&flagsPtr,TX_WAIT_FOREVER);
			num--;
		}
	}
	SnapSetflag = 0;
	sp5kOsEventFlagsDelete(&mtbfSnapEvt);
	if(mtbf_mode){
		sp5kHostMsgSend(APP_UI_MSG_MTBF_JOB_DONE,0);
	}
}

void appMTBFPB(UINT32 param)
{
	param = param;
	
	if(appActiveStateGet() != APP_STATE_PB_MAIN){
		appStateChange(APP_STATE_PB_MAIN,STATE_PARAM_NORMAL_INIT);
	}
	
	if(mtbf_mode){
		sp5kHostMsgSend(APP_UI_MSG_MTBF_JOB_DONE,0);
	}
	sp5kHostMsgSend(APP_UI_MSG_MTBF_TASK_DEL,0);
}

void appMTBFFileDel(UINT32 param)
{
	UINT32 curIdx,fileNum;
	UINT32 ret;
	
	ret = sp5kPbTotalFileNumGet(&fileNum);
	
	//printf("fileNum:%d\n",fileNum);
	if(ret == SUCCESS){
		if(!fileNum){
			printf("No file!!!\n");
		}else{
			if(param){
				ret = sp5kDcfFsFileOperate(SP5K_DCF_FILEOP_DELALL, 0, 0, 0, 0);
				printf("delete all file %s!\n",(ret==SUCCESS)?"SUCCESS":"FAIL");
			}else{
				sp5kDcfFsCurFileIdxGet(&curIdx);
				sp5kDcfFsCurFileIdxSet(curIdx);
				ret = sp5kDcfFsFileOperate(SP5K_DCF_FILEOP_DEL, 0, 0, 0, 0);
				printf("delete file %s!\n",(ret==SUCCESS)?"SUCCESS":"FAIL");
			}
		}
	}
	
	if(mtbf_mode){
		sp5kHostMsgSend(APP_UI_MSG_MTBF_JOB_DONE,0);
	}
}

void appMTBFTask(ULONG para)
{
	UINT32 i=0;
	ULONG flagsPtr;

	para = para;
	
	while(1){
		sp5kOsMutexGet(&mtbfLock,TX_WAIT_FOREVER);
		sp5kOsMutexGet(&mtbfCmdTblLock,TX_WAIT_FOREVER);
		appMTBFCmdProcess(mtbfCmdTbl[i].cmdID,mtbfCmdTbl[i].param);
		sp5kOsMutexPut(&mtbfCmdTblLock);
		//sp5kOsMutexPut(&mtbfLock);
		sp5kOsEventFlagsGet(&mtbfTaskEvt,1,TX_OR_CLEAR,&flagsPtr,TX_WAIT_FOREVER);
		i++;
		if(i>=MTBF_CMD_NUM_MAX){
			i = 0;
			if(mtbfLoopCnt){
				mtbfLoopCnt --;
			}else{
				sp5kHostMsgSend(APP_UI_MSG_MTBF_TASK_DEL,0);
				break;
			}
		}
	}
}

void appMTBFTestInit(UINT32 input)
{
	UINT32 i;
	//UINT32 preState = appPreviousStateGet();

	if(input){
		curCmdIdx = 0;
		ptrdMtbf = sp5kOsThreadCreate("MTBF",appMTBFTask,0,16,16,0,1);

		if(ptrdMtbf){
			for(i = 0;i < MTBF_CMD_NUM_MAX;i++){
				mtbfCmdTbl[i].cmdID= MTBF_CMD_NULL;
				mtbfCmdTbl[i].param= 0;
			}
			sp5kOsEventFlagsCreate(&mtbfTaskEvt,"mtbfTaskLock");
			sp5kOsMutexCreate(&mtbfLock,"mtbfLock",0);
			sp5kOsMutexGet(&mtbfLock,TX_WAIT_FOREVER);
			sp5kOsMutexCreate(&mtbfCmdTblLock,"mtbfCmdTblLock",0);
			appMTBFCmdShow();
		}
	}else{
		mtbf_mode = 0;
		
		if(mtbfTaskEvt){
			sp5kOsEventFlagsDelete(&mtbfTaskEvt);
		}
		if(mtbfLock){
			sp5kOsMutexDelete(&mtbfLock);
		}
		if(ptrdMtbf){
			sp5kOsThreadDelete(ptrdMtbf);
		}
		if(mtbfCmdTblLock){			
			sp5kOsMutexDelete(&mtbfCmdTblLock);
		}

		/*if(STATE_GROUP(preState) == APP_STATE_GROUP_VIDEO){
			appStateChange(APP_STATE_VIDEO_PREVIEW,STATE_PARAM_NORMAL_INIT);
		}else if(STATE_GROUP(preState) == APP_STATE_GROUP_VIEW){
			appStateChange(APP_STATE_STILL_PREVIEW,STATE_PARAM_NORMAL_INIT);
		}else{
			appStateChange(APP_STATE_PB_MAIN,STATE_PARAM_NORMAL_INIT);
		}*/
	}
}

void appMTBFJobFinEvtPut(void)
{
	if(mtbfTaskEvt){
		sp5kOsEventFlagsSet(&mtbfTaskEvt,1,TX_OR);
	}
}

UINT32 appMTBFCmdParse(UINT8 *cmd)
{
	UINT32 i;

	for(i=0;i<MTBF_CMD_MAX;i++){
		if(strcmp(cmd,mtbfCmdName[i].cmdName) == 0){
			return i;
		}
	}
	return MTBF_CMD_INVALID;
}

UINT32 appMTBFCmdParaParse(UINT32 cmdID,UINT8 *option,UINT32 param)
{
	UINT32 ret = 0;

	switch(cmdID){
		case MTBF_VIDEO_SIZE:
			if((strcmp(option,"1080p")==0)||(strcmp(option,"1080P")==0)){
				ret = UI_VID_SIZE_FHD;
			}else if((strcmp(option,"720p")==0)||(strcmp(option,"720P")==0)){
				if(param == 30){
					ret = UI_VID_SIZE_HD_30FPS;
				}else{
					ret = UI_VID_SIZE_HD_60FPS;
				}
			}else if((strcmp(option,"vga")==0)||(strcmp(option,"VGA")==0)){
				ret = UI_VID_SIZE_VGA;
			}else if((strcmp(option,"qvga")==0)||(strcmp(option,"QVGA")==0)){
				ret = UI_VID_SIZE_QVGA;
			}else if((strcmp(option,"svga")==0)||(strcmp(option,"SVGA")==0)){
				ret = UI_VID_SIZE_SVGA;
			}else{
				ret = UI_VID_SIZE_FHD;
			}
			break;
		case MTBF_STILL_SIZE:
			if((strcmp(option,"10m")==0)||(strcmp(option,"10M")==0)){
				ret = UI_PIC_SIZE_10M;
			}
			#if !CAM_TYPE_CVR
			else if((strcmp(option,"9m")==0)||(strcmp(option,"9M")==0)){
				ret = UI_PIC_SIZE_9M;
			}
			#endif
			else if((strcmp(option,"8m")==0)||(strcmp(option,"8M")==0)){
				ret = UI_PIC_SIZE_8M;
			}
			#if CAM_TYPE_CVR
			else if((strcmp(option,"6m")==0)||(strcmp(option,"6M")==0)){
				ret = UI_PIC_SIZE_6M;
			}
			#endif
			#if !CAM_TYPE_CVR
			else if((strcmp(option,"7m")==0)||(strcmp(option,"7M")==0)){
				ret = UI_PIC_SIZE_7M;
			}else if((strcmp(option,"5m")==0)||(strcmp(option,"5M")==0)){
				ret = UI_PIC_SIZE_5M;
			}
			#endif
			else if((strcmp(option,"3m")==0)||(strcmp(option,"3M")==0)){
				ret = UI_PIC_SIZE_3M;
			}else if((strcmp(option,"2m")==0)||(strcmp(option,"2M")==0)){
				ret = UI_PIC_SIZE_2M;
			}
			#if !CAM_TYPE_CVR
			else if((strcmp(option,"1m")==0)||(strcmp(option,"1M")==0)){
				ret = UI_PIC_SIZE_1M;
			}
			#endif
			else if((strcmp(option,"vga")==0)||(strcmp(option,"VGA")==0)){
				ret = UI_PIC_SIZE_VGA;
			}else{
				ret = UI_PIC_SIZE_10M;	
			}
			break;
		case MTBF_FILE_DEL:
			if((strcmp(option,"all")==0)||(strcmp(option,"ALL")==0)){
				ret = 1;
			}
			break;
		default:
			break;
	}
	return ret;
}

void appMTBFCmdPut(UINT8 *cmd,UINT8 *option,UINT32 param)
{
	UINT32 i;
	UINT32 id;

	sp5kOsMutexGet(&mtbfCmdTblLock,TX_WAIT_FOREVER);
	if(strcmp(cmd,"show") == 0){
		putchar('\n');
		for(i = 0;i < curCmdIdx;i++){
			printf("%s: %d\n",mtbfCmdName[mtbfCmdTbl[i].cmdID].cmdName,mtbfCmdTbl[i].param);
		}
		sp5kOsMutexPut(&mtbfCmdTblLock);
		return;
	}else if(strcmp(cmd,"start")==0){
		if(param){
			mtbfLoopCnt = param;
			mtbfLoopCnt --;
		}else{
			mtbfLoopCnt = 0;
		}
		sp5kOsMutexPut(&mtbfCmdTblLock);
		sp5kOsMutexPut(&mtbfLock);
		return;
	}

	if(curCmdIdx >= MTBF_CMD_NUM_MAX){
		printf("cmd number is over the limit\n");
		sp5kOsMutexPut(&mtbfCmdTblLock);
		return;
	}
	
	id = appMTBFCmdParse(cmd);
	if(id == MTBF_CMD_INVALID){
		printf("invaild cmd\n");
		sp5kOsMutexPut(&mtbfCmdTblLock);
		return;
	}

	if((id == MTBF_VIDEO_SIZE) || (id == MTBF_STILL_SIZE) || (id == MTBF_FILE_DEL)){
		if(option != NULL){
			param = appMTBFCmdParaParse(id,option,param);
		}else{
			param = 0;
		}
	}

	mtbfCmdTbl[curCmdIdx].cmdID = id;
	mtbfCmdTbl[curCmdIdx].param = param;
	curCmdIdx++;
	sp5kOsMutexPut(&mtbfCmdTblLock);
}

void appMTBFCmdProcess(UINT32 cmdID,UINT32 param)
{
	if(cmdID == 0xFFFFFFFF){
		return;
	}
	mtbfCmdName[cmdID].pMtbfFunc(param);
}
#endif

