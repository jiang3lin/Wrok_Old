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
#include "app_rtc.h"
#include "sp5k_global_api.h"
#include "app_ui_para.h"

BOOL
appRtcLostCheck(
	void
)	
{
	UINT32 rtcReliable = 0;
	
	profLogAdd(0, "RTC check");
	
	sp5kRtcReliableGet(&rtcReliable);
	if (rtcReliable != APP_RTC_RELIABLE_CODE && rtcReliable != 0x5b && rtcReliable != 0x5d){				
		printf("[WARN]RTC Lost!!!!\n");
		return TRUE;
	}
	#if HOST_DBG 
	else {
		appRTC_t tt;
		appRtcGet(&tt);
		DBG_PRINT("\nRTC : %d : %d : %d : %d : %d : %d\n\n",tt.year, tt.mon,tt.day,tt.hour,tt.min,tt.sec);
	}
	#endif
	return FALSE;
}

void
appRtcSet(
	appRTC_t *ptime
)
{
	tmx_t rtc;
	
	rtc.tmx_year = ptime->year-1900;
	rtc.tmx_mon = ptime->mon;
	rtc.tmx_mday = ptime->day;
	rtc.tmx_hour = ptime->hour;
	rtc.tmx_min = ptime->min;
	rtc.tmx_sec = ptime->sec;

	sp5kRtcDateTimeSet( SP5K_DATE_TIME_OPTION, &rtc);
}


void
appRtcWakeUpSet(
	appRTC_t *ptime
)
{
	tmx_t rtc;
	
	rtc.tmx_year = ptime->year-1900;
	rtc.tmx_mon = ptime->mon;
	rtc.tmx_mday = ptime->day;
	rtc.tmx_hour = ptime->hour;
	rtc.tmx_min = ptime->min;
	rtc.tmx_sec = ptime->sec;

	sp5kRtcDateTimeSet( SP5K_WAKEUP_OPTION, &rtc);
}


void
appRtcGet(
	appRTC_t *ptime
)
{
	tmx_t rtc;
	sp5kRtcDateTimeGet( SP5K_DATE_TIME_OPTION, &rtc);
	
	ptime->year = rtc.tmx_year+1900;
	ptime->mon = rtc.tmx_mon;
	ptime->day = rtc.tmx_mday;
	ptime->hour = rtc.tmx_hour;
	ptime->min = rtc.tmx_min;
	ptime->sec = rtc.tmx_sec;
}

BOOL 
appLeapYearCheck( 
	UINT16 year 
) 
{

	/* if mutiple of 4 */
	if ( year % 4 ) {
		return FALSE;  
	}
	/* if mutiple of 100, but not 400 */
	if ( year % 100 == 0 ) {
		if ( year % 400 ) {
			return FALSE; 
		}
	}
	return TRUE;
}

void
appRtcDefaultSet(
	void
)
{
	appRTC_t time;
	
	profLogAdd(0, "RTC reset");

/*s Eric Lee 100716 modifed the default time */	
	time.year = APP_RTC_DEFAULT_YEAR;
	time.mon = 1;
	time.day = 1;
	time.hour = 0;
	time.min = 0;
	time.sec = 0;
/*e Eric Lee 100716 modifed the default time */		
	appRtcSet(&time);
}

void appTimezoneChange(UINT32 oldzone,UINT32 newzone)
{
	 UINT8 zonetime=0;
	 UINT8  daymax;
 	 appRTC_t ptime;	
	 
	 if(oldzone==newzone)
	 {
		return;
	 }

	appRtcGet(&ptime);
	 
	 daymax = 31;
	 if ((ptime.mon == 4) || (ptime.mon == 6) || (ptime.mon == 9) || (ptime.mon == 11))
	 {
	     daymax = 30;
	 }
	 else if(ptime.mon == 2)
	 {
		  if ( (((ptime.year % 4) == 0) && ((ptime.year% 100) != 0)) || ((ptime.year % 400) == 0) )
		  {
		 	  daymax = 29;
		  }
		  else
		  {
		   daymax = 28;
		  }
	 }
 
 
	 if(newzone>oldzone)
	 {
	      	 zonetime=newzone-oldzone;
	      	 ptime.hour +=zonetime;

		 //printf("@@----- ptime.hour=%d   ptime.day=%d \n", ptime.hour,ptime.day);
		  if(ptime.hour>=24)
		  {
			   ptime.hour-=24;
			   ptime.day++;
			   if(ptime.day>daymax)
			   {
				    ptime.day=1;
				    ptime.mon++;
				    if(ptime.mon>12)
				    {
				     ptime.mon=1;
				     ptime.year++;
				    }
			 
			   }
		   
		  }
	 
	 }
	 else 
	 {
	  
		  zonetime=oldzone-newzone;
		  if(ptime.hour>=zonetime)
		  {
		   	 ptime.hour-=zonetime;
		   }
	  	else 
		{
		        ptime.hour=24+ptime.hour-zonetime;
			    if(ptime.day>1)
			    {
			     	ptime.day--;
			     }
		    	    else
			    {
			    	      ptime.day=31;
				     if(ptime.mon>1)
				     {
				      	ptime.mon--;
				      }
				     else 
				     {
					      ptime.mon=12;
					      ptime.year--;
				      } 
			     }
		 
		    daymax = 31;
		    if ((ptime.mon == 4) || (ptime.mon == 6) || (ptime.mon == 9) || (ptime.mon == 11))
		    {
		     	daymax = 30;
		    }
		    else if(ptime.mon == 2)
		    {
			     if ( (((ptime.year % 4) == 0) && ((ptime.year% 100) != 0)) || ((ptime.year % 400) == 0) )
			     {
			          daymax = 29;
			     }
			     else
			     {
			      daymax = 28;
			     }
		    }
		    if(ptime.day>=daymax)
		    {
		      ptime.day=daymax;
		     }
		   }
		 
		 }
//printf(" end --zone==%d:  update rtc time=%4d:%2d:%2d:%2d:%2d:%2d\n",zone,ptime->year ,ptime->mon,ptime->day,ptime->hour,ptime->min,ptime->sec);
	appRtcSet(&ptime);
}

BOOL appRtcRangeCheck(appRTC_t *ptime)
{
	UINT8 dayMax = 31;
	BOOL bChange = FALSE;

	if (ptime->year > APP_RTC_MAX_YEAR)
	{
		ptime->year = APP_RTC_MIN_YEAR;
		bChange = TRUE;
	}
	else if(ptime->year < APP_RTC_MIN_YEAR)
	{
		ptime->year = APP_RTC_MAX_YEAR;
		bChange = TRUE;
	}

	switch(ptime->mon)
	{
		case 2:
			if(appLeapYearCheck(ptime->year))
			{
				dayMax = 29;
			}
			else
			{
				dayMax = 28;
			}
			break;			
		case 4:
		case 6:
		case 9:
		case 11:
			dayMax = 30;
			break;
	}

	if (ptime->day > dayMax)
	{
		ptime->day = 1;
		bChange = TRUE;
	}
	else if(ptime->day < 1)
	{
		ptime->day = dayMax;
		bChange = TRUE;
	}

	return bChange;
}
	
