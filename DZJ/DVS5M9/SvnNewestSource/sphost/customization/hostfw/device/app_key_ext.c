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
#include "common.h"
#include "api/sp5k_global_api.h"
#include "api/sp5k_os_api.h"
#include "App_com_api.h"
#include "App_com_def.h"
#include "app_script.h"

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

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/


#if 0
/**************************************************************************/
/* dup key service */

/* Repeating sending button msg in button driver is not a good approach.
   Repeating key is an application-dependent behavior,
   Camera don't need repeated key in most cases.
   Therefore, I make it a host service */

typedef struct {
	UINT32 key,key0;
	UINT32 count;
	UINT32 timer;
	UINT32 inTime;
} appDupKey_t;
appDupKey_t appDupKey={
	0, 0xffffffff,
	0,
	TIMER_NULL
};

void
appDupKeySet(
	UINT32 key
)
{
	if (appDupKey.timer != TIMER_NULL)
		appTimerClear(&appDupKey.timer);
	if (KEY_STATE(key)==KEY_PRESS) {
		appDupKey.inTime = appMsTimeGet();
		if (key != appDupKey.key) {
			appDupKey.key = key;
			appDupKey.count = 0;
		}
	} else
		appDupKey.key = 0;
	DBG_PRINT("appDupKeySet:new=%x cur=%x\n", key, appDupKey.key);
}

UINT32 
appDupKeyCfgSetEx(
	UINT32 msDelay1, 
	UINT32 msDelay2, 
	UINT32 thr
)
{
	UINT32 msDelay,ms;
	DBG_PRINT("appDupKey:set %x\n", appDupKey.key);
	if (KEY_STATE(appDupKey.key)==KEY_PRESS) {
		if (msDelay1 && msDelay2) {
			appDupKey.key0 = appDupKey.key;
			msDelay = (appDupKey.count < thr) ? msDelay1 : msDelay2;
			ms = appMsTimeGet();
			ms = MS_DIFF(appDupKey.inTime, ms);
			msDelay = msDelay>ms+10 ? msDelay-ms : 10; 
			appDupKey.timer = appTimerSet(msDelay, "DupKey");
			DBG_PRINT("appDupKey:%d used, set %x cnt=%d delay=%d\n", ms, appDupKey.key, appDupKey.count, msDelay);
			if (appDupKey.timer != TIMER_NULL)
				return 0;
		}
	}
	return 1;
}

UINT32 
appDupKeyCfgSet(
	UINT16 ms
)
{
	return appDupKeyCfgSetEx(ms, ms, 1);
}

void 
appDupKeyClear(
	void
)
{
	DBG_PRINT("appDupKey:clear %x\n", appDupKey.key);
	appDupKey.key = 0;
	appDupKey.count = 0;
	if (appDupKey.timer != TIMER_NULL)
		appTimerClear(&appDupKey.timer);
}

UINT16 
appDupKeyCount(
	UINT32 key
)
{
	return key == appDupKey.key ? appDupKey.count : 0;
}
#endif

#if 0
/**************************************************************************/
/* key deferring */

#define DEFER_FLG_RELEASE	(1<<0) /* send release key also */

struct {
	UINT32 key;
	UINT32 timer;
	UINT32 flg;
} DeferKeyCb[DEFER_KEY_SLOT_MAX]={
	{0, TIMER_NULL},
	{0, TIMER_NULL},
	{0, TIMER_NULL},
};

UINT32 
appDeferKeySet(
	UINT32 slot,
	UINT32 key,
	UINT32 delayMs
)
{
	if (slot>=DEFER_KEY_SLOT_MAX)
		return FAIL;
	appTimerClear(&DeferKeyCb[slot].timer);
	DeferKeyCb[slot].timer = appTimerSet(delayMs, "DeferKey");
	DeferKeyCb[slot].key = key;
	DeferKeyCb[slot].flg = 0;
	return SUCCESS;
}

UINT32 
appDeferKeyGet(
	UINT32 slot,
	UINT32 *pKey
)
{
	if (slot<DEFER_KEY_SLOT_MAX && DeferKeyCb[slot].key) {
		if (pKey)
			*pKey = DeferKeyCb[slot].key;
		return SUCCESS;
	}
	return FAIL;
}
#endif

#if 0

/**************************************************************************/
/* key logging */
static UINT8 KeyLogEnable=0;
static UINT32 KeyLogTimeStamp=0;
static UINT32 *KeyLogBuf=NULL;
static UINT32 KeyLogIdx,KeyLogMax;

UINT32
KeyLogAdd(
	UINT16 delay100ms,
	UINT16 key
)
{
	if (KeyLogIdx>=KeyLogMax)
		return 1;
	KeyLogBuf[KeyLogIdx++] = (((UINT32)delay100ms)<<16) | key;
	return KeyLogIdx>=KeyLogMax ? 1 : 0;
}

void
appKeyLogOpen(
	UINT32 KeyMax
)
{
	if (KeyLogBuf)
		sp5kFree(KeyLogBuf);
	KeyLogBuf = sp5kMalloc(KeyMax*sizeof(*KeyLogBuf));
	KeyLogIdx = 0;
	KeyLogMax = KeyMax;
	KeyLogEnable = 1;
}

UINT32
appKeyLogClose(
	char *fname
)
{
	UINT32 r=SUCCESS;
	KeyLogEnable = 0;
	if (KeyLogBuf) {
		r = appScriptKeyLogSave(KeyLogBuf, KeyLogIdx, fname);
		sp5kFree(KeyLogBuf);
		KeyLogBuf = NULL;
	}
	return r;
}

void
appKeyLogAdd(
	UINT32 key
)
{
	UINT32 t;
	DBG_ASSERT((key&0xffff)==key);
	if (KeyLogEnable==0)
		return;
	t = appMsTimeGet();
	if (KeyLogIdx==0)
		KeyLogTimeStamp = t;
	if (KeyLogAdd(MS_DIFF(KeyLogTimeStamp, t)/100, (UINT16)key)==1) {
		/* buffer full */
		appKeyLogClose("C:\\KEYLOG.TXT");
	}
	KeyLogTimeStamp = t;
}
#endif

#if 0
/**************************************************************************/
/* main entry of key ext */
void
appKeyExtKeyHandler(
	UINT32 key
)
{
	UINT32 key0;
	appDupKeySet(key);
	/* to maintain key press & release order for defer key */ 
 	if (KEY_STATE(key)==KEY_RELEASE && appDeferKeyGet(DEFER_KEY, &key0)==SUCCESS) {
		if (KEY_STATE(key0)==KEY_PRESS && KEY_CODE(key)==KEY_CODE(key0))
			DeferKeyCb[DEFER_KEY].flg |= DEFER_FLG_RELEASE;
		}
	
	

}

void
appKeyExtTimerHandler(
	UINT32 timer
)
{
	if (timer == appDupKey.timer) {
		appTimerClear(&appDupKey.timer);
		DBG_PRINT("DupKeyTimer:cnt=%d key=%x %x\n", appDupKey.count, appDupKey.key, appDupKey.key0);
		if (appDupKey.key == appDupKey.key0) {
			appDupKey.count++;
			sp5kHostMsgSend(SP5K_MSGX_KEY, appDupKey.key);
		}
	} else {
		int slot;
		for (slot=0 ; slot<DEFER_KEY_SLOT_MAX ; slot++) {
			if (timer==DeferKeyCb[slot].timer) {
				appTimerClear(&DeferKeyCb[slot].timer);
				sp5kHostMsgSend(SP5K_MSGX_KEY, DeferKeyCb[slot].key);
				if (DeferKeyCb[slot].flg & DEFER_FLG_RELEASE)
					sp5kHostMsgSend(SP5K_MSGX_KEY, KEY_CODE(DeferKeyCb[slot].key)|KEY_RELEASE);
				DeferKeyCb[slot].key = 0;
				break;
			}
		}
	}
}
#endif

