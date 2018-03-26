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
#include "app_sys_def.h" 
#if MTBF_DBG_EN

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef void(*pfMTBF)(UINT32 param);
 
typedef struct mtbf_cmd_s{
	UINT32 cmdID;
	UINT32 param;
}mtbf_cmd_t;

typedef struct mtbf_cmd_name_s{
	UINT8 *cmdName;
	pfMTBF pMtbfFunc;
}mtbf_cmd_name_t;	

typedef enum{
	MTBF_CMD_NULL,
	MTBF_VIDEO_SIZE,
	MTBF_VIDEO_REC,
	MTBF_STILL_SIZE,
	MTBF_STILL_SNAP,
	MTBF_PB_MAIN,
	MTBF_FILE_DEL,
	MTBF_CMD_MAX,
	MTBF_CMD_INVALID
}MTBF_CMD_T;
	
/**************************************************************************
 *				 F U N C T I O N	D E C L A R A T I O N S 			  *
 **************************************************************************/
void appMTBFTestInit(UINT32 input);
void appMTBFTask(ULONG para);
void appMTBFCmdHelp(UINT32 cmdID);
void appMTBFCmdPut(UINT8 *cmd,UINT8 *option,UINT32 param);
void appMTBFCmdProcess(UINT32 cmdID,UINT32 param);
void appMTBFJobFinEvtPut(void);
UINT32 appMTBFCmdParaParse(UINT32 cmdID,UINT8 *option,UINT32 param);

void appMTBFNull(UINT32 temp);
void appMTBFVideoSizeSet(UINT32 size);
void appMTBFVideoRec(UINT32 time);
void appMTBFImgSizeSet(UINT32 size);
void appMTBFStillCapEvtPut(void);
void appMTBFStillCapture(UINT32 num);
void appMTBFPB(UINT32 param);
void appMTBFFileDel(UINT32 param);
#endif
