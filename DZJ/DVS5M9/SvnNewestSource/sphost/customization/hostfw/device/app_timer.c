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
#include "app_timer.h"

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

/* ========================================================= */
/* Extended timer service */

static UINT32 appTimerMask=0;

#if HOST_DBG
static char *appTimerOwner[SP5K_TIMER_MAX];
#define SET_TIMER_OWNER(timer,str)	(appTimerOwner[(timer)] = (str))
#define GET_TIMER_OWNER(timer)		appTimerOwner[(timer)]
#else
#define SET_TIMER_OWNER(timer,str)
#define GET_TIMER_OWNER(timer)		""
#endif

UINT32 
appTimerSet(
	UINT32 delayMs,
	char *owner
)
{
	UINT32 timer,delay,enable;
	for (timer=0 ; timer<=SP5K_TIMER_MAX ; timer++) 
	{
		sp5kTimerCfgGet(timer, &delay, &enable);
		if (!enable) 
		{
			sp5kTimerCfgSet(timer, delayMs);
			sp5kTimerEnable(timer, 1);
			appTimerMask |= (1<<timer);
			SET_TIMER_OWNER(timer, owner);
			DBG_PRINT("appTimer:set %d %s(%d)\n", timer, owner, delayMs);
			return timer;
		}
	}
	DBG_PRINT("No avaiable timer\n");
	return TIMER_NULL; /* no available timer */
}

void 
appTimerClear(
	UINT32 *ptimer
)
{
	DBG_ASSERT(ptimer!=NULL);
	if (*ptimer<SP5K_TIMER_MAX) 
	{
		if ((appTimerMask & (1<<(*ptimer))) ) 
		{
			sp5kTimerEnable(*ptimer, 0);
			DBG_PRINT("appTimer:clear %d\n", *ptimer);
			appTimerMask &= ~(1<<(*ptimer));
		}
		*ptimer = TIMER_NULL;
	}
}

void
appTimerDump(
	void
)
{
	UINT32 i,mask;
	printf("Active appTimer\n");
	for (i=0,mask=1 ; i<SP5K_TIMER_MAX ; i++,mask<<=1) 
	{
		if (appTimerMask & mask)
		{
			printf("%d owner=%s\n", i, GET_TIMER_OWNER(i));
		}
	}
}

void 
appTimeDelay(
	UINT32 delayMs,
	UINT32 fCleanMsg
)
{

sp5kTimeDelay(SP5K_TIME_DELAY_1MS, delayMs);

#if 0
	if (fCleanMsg)
	{
		sp5kTimerCfgSet(0, delayMs);
		sp5kTimerEnable(0, 1);
		appHostMsgWaitExact(SP5K_MSG_TIMER, 0, 2000);
		sp5kTimerEnable(0, 0);
	} 
	else 
	{
		sp5kTimeDelay(SP5K_TIME_DELAY_1MS, delayMs);
	}
#endif
}

