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
/**************************************************************************
*								 M A C R O S							  *
**************************************************************************/
#define MSG_NEW_PROC	1

#if MSG_NEW_PROC

/**************************************************************************
 * 						 D A T A	T Y P E S						   *
 **************************************************************************/
typedef enum appMsgQueID_e{
	APP_MSG_KEY_QUE,
	APP_MSG_UI_QUE,
	APP_MSG_KERNEL_QUE,
	APP_MSG_MAX_QUE
}appMsgQueID_t;  

typedef struct appMsgQue_s{
	UINT32 msg; 	
	UINT32 param;
	UINT32 queID;      /* 0:key msg    1:UI msg    2:kernel msg   */
	struct appMsgQue_s *next;
} appMsgQue_t;

/**************************************************************************
*                F U N C T I O N   D E C L A R A T I O N S               *
**************************************************************************/
 UINT32 appMsgReceive(UINT32 *cmd,UINT32 *param);
 UINT32 appMsgFlush(UINT32 queID);
 UINT32 appMsgSeek(UINT32 waitMsg,UINT32 *pParam);
 UINT32 appMsgSeekExt(UINT32 waitMsg,UINT32 waitParam,appMsgQue_t *pPre,appMsgQue_t** pRet);
 void appMsgInit(void);
 void appMsgDebugPrint(void);
 #endif

