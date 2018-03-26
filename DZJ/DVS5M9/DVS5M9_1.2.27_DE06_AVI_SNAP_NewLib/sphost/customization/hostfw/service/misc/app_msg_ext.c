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
#include "app_com_api.h"
#include "sp5k_global_api.h"
#include "app_msg.h"

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
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern void ros_thread_sleep(UINT32 ms);

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/


/* queue msg during msg wait and dump them back to msg queue later */

#define WAIT_MSG_HISTORY_MAX	128
UINT32 msgQidx,msgQ[WAIT_MSG_HISTORY_MAX];

void 
_MsgWaitHistoryInit(
	void
)
{
	msgQidx = 0;
}

BOOL
_MsgWaitIgnoreMsgCheck(
	UINT32 msg, 
	UINT32 param
)
{
	switch(msg) {
		//sp5k msg
		case SP5K_MSG_POWER_OFF:
		case SP5K_MSG_DISK_INSERT:
		case SP5K_MSG_DISK_REMOVAL:	
		case SP5K_MSG_STILL_STORE_COMPLETE:	
		case SP5K_MSG_STILL_STORAGE_FULL:
		//ui msg
		case APP_UI_MSG_POWER_OFF:
		case APP_UI_MSG_POWER_SAVE_TIME_OUT:
		case APP_UI_MSG_BATT_LEVEL_CHANGE:
		//case APP_UI_MSG_STROBE_CHARGE_READY:
		case APP_UI_MSG_STROBE_CHARGE_ERR:
		case APP_UI_MSG_LENS_ERR:
		case APP_UI_MSG_LENS_CLOSE	:				
			return FALSE;
		default:
			break;
	}	
	return TRUE;
}

void 
_MsgWaitHistoryPush(
	UINT32 msg, 
	UINT32 param
)
{
	if (msgQidx>=WAIT_MSG_HISTORY_MAX) {
		printf("[ERR] %s : overflow\n",__FUNCTION__);
		return;
	}
	if(_MsgWaitIgnoreMsgCheck(msg,param)) {
		msgQ[msgQidx++] = msg;
		msgQ[msgQidx++] = param;
	}
	else {
		printf("[WARN] Ignore msg : (0x%x,0x%x)\n", msg,param);
	}
}

void 
_MsgWaitRemainMsgPuch(
	void
)	
{
	UINT32 msg, param;
	sp5kHostMsgSend(APP_UI_MSG_WAIT_MSG_REMAIN_CHECK, 0x00);

	while (1) {
		sp5kHostMsgReceive(&msg, &param);
		if ((msg==APP_UI_MSG_WAIT_MSG_REMAIN_CHECK) && (param==0x00))
			break;
		_MsgWaitHistoryPush(msg,param);
	}
}


void 
_MsgWaitHistoryDumpToHost(
	void
)
{
	UINT32 i;
	for (i=0 ; i<msgQidx ; i+=2) {
		sp5kHostMsgSend(msgQ[i], msgQ[i+1]);
	}
	msgQidx = 0;
}

UINT32
_MsgWaitHistoryFind(
	UINT32 msg, 
	UINT32 fMatchParam, 
	UINT32 param
)
{
	UINT32 i,c;
	for (i=c=0 ; i<msgQidx ; i+=2) {
		DBG_PRINT("%x/%x %d %x/%x\n", msg, msgQ[i], fMatchParam, param, msgQ[i+1]);
		if (msgQ[i]==msg && (fMatchParam==0 || msgQ[i+1]==param))
			c++;
	}
	DBG_PRINT("MsgWaitHistoryFind:%d\n", c);
	return c;
}
/* core MsgWait */
static UINT32
_MsgWait(
	UINT32 waitMsg,
	UINT32 *pWaitParam,
	UINT32 timeoutMs
)
{
#if MSG_NEW_PROC
	UINT32 t1=0,t2=0;
	UINT32 ret;
	
	DBG_PRINT("MsgWait:%x start\n", waitMsg);
	t1 = sp5kMsTimeGet();

	while(1){
		ros_thread_sleep(10);
		ret = appMsgSeek(waitMsg,pWaitParam);
		t2 = sp5kMsTimeGet();
		if(ret == SUCCESS){
			break;
		}
		if (timeoutMs) {
			if((t2 - t1)>=timeoutMs){
				ret = FAIL;
				printf("msg wait timeout!!!\n");
				break;
			}
		}
	}
#else
	UINT32 timer=TIMER_NULL;
	UINT32 msg, param, ret;

	DBG_PRINT("MsgWait:%x start\n", waitMsg);
	if (timeoutMs) {
		timer = appTimerSet(timeoutMs, "MsgWait");
	}
	
	while (1) {
		sp5kHostMsgReceive(&msg, &param);
		if (msg==waitMsg) {
			if (pWaitParam) {
				*pWaitParam = param;
			}
			ret = SUCCESS;
			break;
		}
		if (waitMsg==SP5K_MSGX_KEY && IS_SP5K_KEY_PRESS_MSG(msg, param)) {
			ret = SUCCESS;
			break;
		}
		if (msg==SP5K_MSG_TIMER && (param==timer)) {
			DBG_PRINT("MsgWait:%x Timeout\n", waitMsg);
			ret = FAIL;
			break;
		}		
		_MsgWaitHistoryPush(msg, param);
	}
	appTimerClear(&timer);	
#endif	
	DBG_PRINT("MsgWait:%x ret=%d\n", waitMsg, ret);

	return ret;
}

#if MSG_NEW_PROC
static UINT32
_MsgWaitExt(
	UINT32 waitMsg,
	UINT32 waitParam,
	UINT32 timeoutMs
)
{
	appMsgQue_t *pPre = NULL;
	appMsgQue_t *pRet = NULL;
	UINT32 t1=0,t2=0;
	UINT32 ret;

	t1 = sp5kMsTimeGet();
	while(1){
		if(pRet){
			pPre = pRet;
		}
		ros_thread_sleep(10);
		ret = appMsgSeekExt(waitMsg,waitParam,pPre,&pRet);
		t2 = sp5kMsTimeGet();
		if(ret == SUCCESS){
			break;
		}
		if (timeoutMs) {
			if((t2 - t1)>=timeoutMs){			
				printf("msg wait timeout!!!\n");
				ret = FAIL;
				break;
			}
		}
	}
	return ret;
}
#endif

UINT32 
appHostMsgWait(
	UINT32 waitMsg,
	UINT32 *pParam,
	UINT32 timeoutMs
)
{
	UINT32 ret;
	profLogAdd(0, "MsgWait:s");
	
#if !MSG_NEW_PROC
	_MsgWaitHistoryInit();
#endif

	ret = _MsgWait(waitMsg, pParam, timeoutMs);
	
#if !MSG_NEW_PROC	
	_MsgWaitRemainMsgPuch();
	_MsgWaitHistoryDumpToHost();
#endif

	profLogAdd(0, "MsgWait:e");
	return ret;
}

UINT32 
appHostMsgWaitExact(
	UINT32 waitMsg,
	UINT32 waitParam,
	UINT32 timeoutMs
)
{
	UINT32 ret;
	profLogAdd(0, "MsgWaitExact:s");
	#if MSG_NEW_PROC
	
	ret = _MsgWaitExt(waitMsg,waitParam,timeoutMs);

	#else
	UINT32 param,t0,t1,t;
	
	_MsgWaitHistoryInit();
	t0 = appMsTimeGet();	
	while (1) 
	{
		ret = _MsgWait(waitMsg, &param, timeoutMs); /* todo: multiple wait will have longer timeout */
		if (ret==SUCCESS && param==waitParam)
		{
			break;
		}
		t1 = appMsTimeGet();
		t = MS_DIFF(t0, t1);
		if (t > timeoutMs) 
		{
			ret = FAIL; 
			break;
		} 
		else 
		{
			timeoutMs -= t;
		}
		t0 = t1;
	}
	_MsgWaitHistoryDumpToHost();
	#endif
	profLogAdd(0, "MsgWaitExact:e");
	return ret;
}

UINT32 
appHostMsgWaitResultFind(
	UINT32 msg,
	UINT32 fMatchParam,
	UINT32 param
)
{
	return _MsgWaitHistoryFind(msg, fMatchParam, param);
}





