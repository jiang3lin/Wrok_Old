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
#include <stdlib.h>
#include <string.h>
#include "app_com_def.h"
#include "app_com_api.h"
#include "sp5k_os_api.h"
#include "sp5k_global_api.h"
#include "sp5k_utility_api.h"
#include "sp5k_dbg_api.h"
#include "sp5k_media_api.h"
#include "app_ui_para.h"
#include "app_osd.h"

#include "app_gps.h"
#include "app_gSensor.h"
#include "app_rtc.h"
#include "app_cdfs_api.h"
#include "app_osd_api.h"
#include "app_view_osd.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define GPS_DBG_NULL_FUNC	do {} while(0)
#define GPS_DBG				0
#if GPS_DBG
#define	DBG_GPS_PROF printf
#else
#define DBG_GPS_PROF(str, arg...)  GPS_DBG_NULL_FUNC
#endif

#define UART_PIN_TX   		(1<<28)
#define UART_PIN_RX   		(1<<29)
#define UART_GPS_BAUD_RATE  9600
#define GPS_UART_PORT_0     0
#define GPS_UART_PORT_1     1

#define UART_THR_PROI		14			
#define UART_THR_PREEMPT	14
#define UART_THR_TIME_SLICE 0
#define UART_THR_AUTO_START 1

#define GPS_INFO_GRP		10
#define GPS_INFO_LINE_MAX	128

#define MAX_GSV 			8

//GPS receive data format set
#define RMC_HEAD			"$GPRMC"
#define GSA_HEAD			"$GPGSA"
#define GSV_HEAD			"$GPGSV"
#define GGA_HEAD			"$GPGGA"

#define HEAD_LEN            6  //head string len = strlen(RMC_HEAR)


/***************************************************************
Recommended Minimum Specific GPS/TRANSIT Data（RMC）推荐定位信息

$GPRMC,<1>,<2>,<3>,<4>,<5>,<6>,<7>,<8>,<9>,<10>,<11>,<12>*hh<CR><LF>

<1> UTC时间，hhmmss（时分秒）格式 
<2> 定位状态，A=有效定位，V=无效定位 
<3> 纬度ddmm.mmmm（度分）格式（前面的0也将被传输） 
<4> 纬度半球N（北半球）或S（南半球） 
<5> 经度dddmm.mmmm（度分）格式（前面的0也将被传输） 
<6> 经度半球E（东经）或W（西经） 
<7> 地面速率（000.0~999.9节，前面的0也将被传输） 
<8> 地面航向（000.0~359.9度，以真北为参考基准，前面的0也将被传输） 
<9> UTC日期，ddmmyy（日月年）格式 
<10> 磁偏角（000.0~180.0度，前面的0也将被传输） 
<11> 磁偏角方向，E（东）或W（西） 
<12> 模式指示（仅NMEA0183 3.00版本输出，A=自主定位，D=差分，E=估算，N=数据无效）
********************************************************************************/
/* $GPRMC,<TIME>,<STATUS>,<LATITUDE>,<LATITUDE_IND>,<LONGITUDE>,
<LONGITUDE_IND>,<SPEED>,<IMGDIRECTION>,<DATE>,<>,<>,<>,*CR
*/
#define RMC_TIME          	1/* TIME after first char ','*/
#define RMC_STATUS      	2/* valid info after second char ','*/
#define RMC_LATITUDE      	3
#define RMC_LATITUDE_IND    4
#define RMC_LONGITUDE      	5
#define RMC_LONGITUDE_IND   6
#define RMC_SPEED      		7
#define RMC_IMGDIRECTION    8
#define RMC_DATE     		9

/****************************************************************
Global Positioning System Fix Data（GGA）GPS定位信息

$GPGGA,<1>,<2>,<3>,<4>,<5>,<6>,<7>,<8>,<9>,M,<10>,M,<11>,<12>*hh<CR><LF>

<1> UTC时间，hhmmss（时分秒）格式 
<2> 纬度ddmm.mmmm（度分）格式（前面的0也将被传输） 
<3> 纬度半球N（北半球）或S（南半球） 
<4> 经度dddmm.mmmm（度分）格式（前面的0也将被传输） 
<5> 经度半球E（东经）或W（西经） 
<6> GPS状态：0=未定位，1=非差分定位，2=差分定位，6=正在估算 
<7> 正在使用解算位置的卫星数量（00~12）（前面的0也将被传输） 
<8> HDOP水平精度因子（0.5~99.9） 
<9> 海拔高度（-9999.9~99999.9） 
<10> 地球椭球面相对大地水准面的高度 
<11> 差分时间（从最近一次接收到差分信号开始的秒数，如果不是差分定位将为空） 
<12> 差分站ID号0000~1023（前面的0也将被传输，如果不是差分定位将为空）
**********************************************************************/

#define GGA_TIME          	1/* TIME after first char ','*/
#define GGA_LATITUDE      	2/* LATITUDE after second char ','*/
#define GGA_LATITUDE_IND    3
#define GGA_LONGITUDE      	4
#define GGA_LONGITUDE_IND   5
#define GGA_PFI   			6
#define GGA_STA_CUR_NUM     7
#define GGA_ALTITUDE     	9

/****************************************************************
GPS Satellites in View（GSV）可见卫星信息 
$GPGSV, <1>,<2>,<3>,<4>,<5>,<6>,<7>,?<4>,<5>,<6>,<7>,<8><CR><LF>

<1> GSV语句的总数 
<2> 本句GSV的编号 
<3> 可见卫星的总数，00 至 12。 
<4> 卫星编号， 01 至 32。 
<5>卫星仰角， 00 至 90 度。 
<6>卫星方位角， 000 至 359 度。实际值。 
<7>讯号噪声比（C/No）
*****************************************************************/
#define GSV_MSG_TOTAL   		1
#define GSV_MSG_IDX   		 	2
#define GSV_STATELLITE_TOTAL    3
#define GSV_STATELLITE_IDX    	4

#define PATH_CHECK				"/DCIM/100MEDIA"
#define GPS_NULL_INFO			"201301011200010N00000000W000000000+0000000"

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
TX_EVENT_FLAGS_GROUP Uart_sig;
#if CAM_TYPE_CVR
appRTC_t rtctrue;	
#endif
//Save all uart get info
static UINT8 sGpsUartBuff[GPS_INFO_GRP][GPS_INFO_LINE_MAX+3]={\
{"$GPGGA, 161229.487,3723.2475,N,12158.3416,W,1,07,1.0,9.0,M.0000*18"},\
{"$GPRMC,161229.487,A,3723.2475,N,12158.3416,W,0.13,309.62,120598,,*10"},\
{"$GPVTG,309.62,T,M,0.13,N,0.2,K*6E"},\
{"$GPGSV , 0, 1, 07, 07, 79,048, 42, 02, 51,062, 43, 26, 36,256, 42, 27, 27, 138,42*71"},\
};

//save valid GPS segment info 
static UINT8 sGpsInfoGGA[GPS_INFO_LINE_MAX+3] = {0};
static UINT8 sGpsInfoRMC[GPS_INFO_LINE_MAX+3] = {0};
static UINT8 sGpsInfoGSV[MAX_GSV][GPS_INFO_LINE_MAX+3] = {0};
static UINT8 sGpsInfoGSA[GPS_INFO_LINE_MAX+3] = {0};

//GPS write txt file
static UINT32 sGpsInfoFd  = 0;
static UINT8  sGpsWriteTxtEn = FALSE;

//mutex for every segment
static TX_MUTEX sGpsRMCMutex;
static TX_MUTEX sGpsGGAMutex;
static TX_MUTEX sGpsGSVMutex;
static TX_MUTEX sGpsGSAMutex;

//operation statues
UINT8  sGpsCurrentPath[128] = {0};
static UINT32 sGpsUseDataCnt = 0;
static UINT8  sGpsInfoUpdat = FALSE;
static UINT8  sGpsStatus = GPS_STOP;
static UINT8  sGpsHdrEn  = FALSE;
#if CAM_TYPE_CVR
/*add for timezone*/
UINT8 gps_update_RtcTime_flag=0;
UINT8 RTCTimeDataCheckCout=0;
/*add for gsvinfo update*/
UINT32 gsatellitecount=0;
UINT32 gsvbuf[12][4]={0};
/*add for GPS speed show on OSD*/
UINT8 stamp_GPSSpeedBuf[20]; 
#endif
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern void sioCtrl(UINT32 opt);

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
static int   appGpsInfoSave(int ch);
static void  appUart_thread(ULONG param);
static UINT8 appGpsSegCheckSum(UINT8 * pstr);
static void  appGpsSegmentSave(UINT8 * pDestBuf,UINT8 * pSrc,UINT32 bufLen,TX_MUTEX * pMutex);

/*****************************************************************************************/
/* appRTC_t tt_time;
appTimezone(&tt_time,zone);
tt_time------ is the time of zero time zone.  zone is The local time zone;0-24     W12-W1 0 E1-E12*/
/*****************************************************************************************/
#if CAM_TYPE_CVR 
void appTimezone(appRTC_t *ptime,UINT8 zone)
{
	 UINT8 zonetime=0;
	 UINT8  daymax;
 
	 daymax = 31;
	 if ((ptime->mon == 4) || (ptime->mon == 6) || (ptime->mon == 9) || (ptime->mon == 11))
	 {
	     daymax = 30;
	 }
	 else if(ptime->mon == 2)
	 {
		  if ( (((ptime->year % 4) == 0) && ((ptime->year% 100) != 0)) || ((ptime->year % 400) == 0) )
		  {
		 	  daymax = 29;
		  }
		  else
		  {
		   daymax = 28;
		  }
	 }
 
 
	 if(zone>=12)
	 {
	      	 zonetime=zone-12;
	      	 ptime->hour +=zonetime;

		 //printf("@@----- ptime->hour=%d   ptime->day=%d \n", ptime->hour,ptime->day);
		  if(ptime->hour>=24)
		  {
			   ptime->hour-=24;
			   ptime->day++;
			   if(ptime->day>daymax)
			   {
				    ptime->day=1;
				    ptime->mon++;
				    if(ptime->mon>12)
				    {
				     ptime->mon=1;
				     ptime->year++;
				    }
			 
			   }
		   
		  }
	 
	 }
	 else 
	 {
	  
		  zonetime=12-zone;
		  if(ptime->hour>=zonetime)
		  {
		   	 ptime->hour-=zonetime;
		   }
	  	else 
		{
		        ptime->hour=24+ptime->hour-zonetime;
			    if(ptime->day>1)
			    {
			     	ptime->day--;
			     }
		    	    else
			    {
			    	      ptime->day=31;
				     if(ptime->mon>1)
				     {
				      	ptime->mon--;
				      }
				     else 
				     {
					      ptime->mon=12;
					      ptime->year--;
				      } 
			     }
		 
		    daymax = 31;
		    if ((ptime->mon == 4) || (ptime->mon == 6) || (ptime->mon == 9) || (ptime->mon == 11))
		    {
		     	daymax = 30;
		    }
		    else if(ptime->mon == 2)
		    {
			     if ( (((ptime->year % 4) == 0) && ((ptime->year% 100) != 0)) || ((ptime->year % 400) == 0) )
			     {
			          daymax = 29;
			     }
			     else
			     {
			      daymax = 28;
			     }
		    }
		    if(ptime->day>=daymax)
		    {
		      ptime->day=daymax;
		     }
		   }
		 
		 }
//printf(" end --zone==%d:  update rtc time=%4d:%2d:%2d:%2d:%2d:%2d\n",zone,ptime->year ,ptime->mon,ptime->day,ptime->hour,ptime->min,ptime->sec);

}

void appGPS_Time_String_ASCII_Get(appRTC_t *ptime,UINT8  id,UINT8  ucbuf[])
{

	switch(id){
		case GPS_INFO_DATE:
			ptime->year=2*1000+0*100+(ucbuf[0]-0x30)*10+(ucbuf[1]-0x30);
			ptime->mon=(ucbuf[2]-0x30)*10+(ucbuf[3]-0x30);
			ptime->day=(ucbuf[4]-0x30)*10+(ucbuf[5]-0x30);;
			break;
		case GPS_INFO_TIME:
			ptime->hour=(ucbuf[0]-0x30)*10+(ucbuf[1]-0x30);
			ptime->min=(ucbuf[2]-0x30)*10+(ucbuf[3]-0x30);
			ptime->sec=(ucbuf[4]-0x30)*10+(ucbuf[5]-0x30);
			break;
		default:
			break;
		}
}

void appGPS_Time_String_Get(appRTC_t *ptime,UINT8  id,UINT8  utcbuf[])
{
	switch(id){
		case GPS_INFO_DATE:
			utcbuf[0]=ptime->year/10%10+0x30;
			utcbuf[1]=ptime->year%10+0x30;
			utcbuf[2]=ptime->mon/10%10+0x30;
			utcbuf[3]=ptime->mon%10+0x30;
			utcbuf[4]=ptime->day/10%10+0x30;
			utcbuf[5]=ptime->day%10+0x30;
				break;
		case GPS_INFO_TIME:
			utcbuf[0]=ptime->hour/10%10+0x30;
			utcbuf[1]=ptime->hour%10+0x30;
			utcbuf[2]=ptime->min/10%10+0x30;
			utcbuf[3]=ptime->min%10+0x30;
			utcbuf[4]=ptime->sec/10%10+0x30;
			utcbuf[5]=ptime->sec%10+0x30;
				break;
		default:
			break;
	utcbuf[6]='\0';
	}
}

void appGpsGsvInfoUpdate(void)
{
	UINT32 cnt;
	UINT8 GSVInfoBuf[GPS_INFO_LINE_MAX+3]={0};
	
	if(sGpsStatus == GPS_STOP){
		appOsd_ColorDrawShapeColorSet(PAL_BLACK, PAL_BLEND_100, PAL_BLACK, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, 0, 0,320, 240, 0, 0);
		
		sprintf(GSVInfoBuf,"total:%2d ",0);
		
		appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_TextDraw(20, 40, SP5K_GFX_ALIGN_TOP_LEFT, GSVInfoBuf);
		return;
	}

	appOsd_ColorDrawShapeColorSet(PAL_BLACK, PAL_BLEND_100, PAL_BLACK, PAL_BLEND_100);
	appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, 0, 0,320, 240, 0, 0);

	sprintf(GSVInfoBuf,"total:%2d ",gsatellitecount);
	
	appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_TextDraw(20, 40, SP5K_GFX_ALIGN_TOP_LEFT, GSVInfoBuf);

	appGpsGsvInfoGet(gsvbuf,sizeof(gsvbuf));
	
	for(cnt=0;cnt<gsatellitecount;cnt++)
	{
		sprintf(GSVInfoBuf,"%2d:%2d ",gsvbuf[cnt][0],gsvbuf[cnt][3]);
		
		appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_TextDraw(0+(cnt%4)*80, 80+(cnt/4)*60, SP5K_GFX_ALIGN_TOP_LEFT, GSVInfoBuf);
	}
}
#endif
/*-------------------------------------------------------------------------
 *  File Name : appCmd2ndUartInit
 *------------------------------------------------------------------------*/ 
void 
appGpsUartInit(
	void
)
{
	UINT32 err; 

	/* Create Mutex for every GPS segment */
	err = sp5kOsMutexCreate(&sGpsRMCMutex, "GpsRMCMutex", 0);
	DBG_ASSERT(err == SUCCESS);

	err = sp5kOsMutexCreate(&sGpsGGAMutex, "GpsGGAMutex", 0);
	DBG_ASSERT(err == SUCCESS);

	err = sp5kOsMutexCreate(&sGpsGSVMutex, "GpsGSVMutex", 0);
	DBG_ASSERT(err == SUCCESS);

	err = sp5kOsMutexCreate(&sGpsGSAMutex, "GpsGSAMutex", 0);
	DBG_ASSERT(err == SUCCESS);

	/* init uart port */
	#if GPS_USE_UART_0
	printf("GPS use UART0!\n");
	sp5kUartInit(GPS_UART_PORT_0, UART_GPS_BAUD_RATE);
	sp5kUartRxCallbackSet(0 ,appGpsInfoSave);//set uart0 Callback
	sioCtrl(1);//close debug print
	DBG_GPS_PROF("[GPS-INFO]:Use port UART0");


	#elif GPS_USE_UART_1 
	TX_THREAD *thr;

	printf("GPS use UART1!\n");
	sp5kUartInit(GPS_UART_PORT_1, UART_GPS_BAUD_RATE);
	sp5kGpioFuncSet(SP5K_GPIO_GRP_FML, UART_PIN_TX, 0);
	sp5kGpioFuncSet(SP5K_GPIO_GRP_FML, UART_PIN_RX, 0);
 
	err = sp5kOsEventFlagsCreate(&Uart_sig, "sigApp");
	HOST_ASSERT(err == SP5K_SUCCESS);
	

	thr = sp5kOsThreadCreate("thrAppUart", appUart_thread, 0,
				UART_THR_PROI, UART_THR_PREEMPT, UART_THR_TIME_SLICE, UART_THR_AUTO_START);
	HOST_ASSERT(thr != 0);
 
	err = sp5kOsEventFlagsSet(&Uart_sig, 1, TX_OR);
	HOST_ASSERT(err == SP5K_SUCCESS);

	DBG_GPS_PROF("[GPS-INFO]:Use port UART1");
	
	#else
	#error  use GPS must select a uart port  in  prj.def file
	
	#endif
}

/*-------------------------------------------------------------------------
 *  File Name : appUart_thread
 *------------------------------------------------------------------------*/
void 
appUart_thread(
	ULONG param
)
{
	UINT32 c;
	UINT32 err;
	ULONG flags;
	for (;;) {
		err = sp5kOsEventFlagsGet(&Uart_sig, 1, TX_OR_CLEAR, &flags, TX_WAIT_FOREVER);
		HOST_ASSERT(err == SP5K_SUCCESS);
 
		while(1){
			sp5kUartRead(1, &c);
			appGpsInfoSave(c);
		}
	}
}

int
appGpsInfoSave(
	int ch
)
{
 	static UINT8 lnIdx = 0;
	static UINT8 * pUart = &sGpsUartBuff[0][0];
	static UINT8   uartBuffGrpIdx = 0;
	UINT8  c = (UINT8)ch;
	UINT32 err;
	UINT8  idx = 0;
	
	if (c != '$') {
		if (lnIdx < GPS_INFO_LINE_MAX) {
			*pUart++ = c; /* reserved one byte for this */
			if (c == 0x0a) {
				*pUart++ = '\0'; /* reserved one byte for this */
			}
			lnIdx++;
		}
	} else {
		*pUart++ = '\0'; /* reserved one byte for this */
		
		if (++uartBuffGrpIdx > (GPS_INFO_GRP - 1)) {
			uartBuffGrpIdx = 0;
		}
		pUart = &sGpsUartBuff[uartBuffGrpIdx][0];
		
		*pUart++ = '$';
		lnIdx = 0;
	}
	if(c == '$'){
		idx = uartBuffGrpIdx > 0 ? (uartBuffGrpIdx-1):(GPS_INFO_GRP - 1);
		if(appGpsSegCheckSum(&sGpsUartBuff[idx][0]) == TRUE){
			if(strncmp(&sGpsUartBuff[idx][0], GGA_HEAD, HEAD_LEN) == 0){  //save GGA info 
				appGpsSegmentSave(sGpsInfoGGA, &sGpsUartBuff[idx][0], \
									sizeof(sGpsInfoGGA), &sGpsGGAMutex);
			}else if(strncmp(&sGpsUartBuff[idx][0], RMC_HEAD, HEAD_LEN) == 0){//save rmc info
				appGpsSegmentSave(sGpsInfoRMC, &sGpsUartBuff[idx][0],\
									sizeof(sGpsInfoRMC), &sGpsRMCMutex);
			}else if(strncmp(&sGpsUartBuff[idx][0], GSV_HEAD, HEAD_LEN) == 0){//save gsv info 
				if((sGpsUartBuff[idx][9] - '0' -1) < MAX_GSV){
				appGpsSegmentSave(&sGpsInfoGSV[(sGpsUartBuff[idx][9] - '0' -1)][0], &sGpsUartBuff[idx][0], \
									sizeof(sGpsInfoRMC), &sGpsGSVMutex);
				}
			}else if(strncmp(&sGpsUartBuff[idx][0], GSA_HEAD, HEAD_LEN) == 0){ //save gsa info
				appGpsSegmentSave(sGpsInfoGSA, &sGpsUartBuff[idx][0], \
									sizeof(sGpsInfoGSA), &sGpsGSAMutex);
			} 
			sGpsStatus = GPS_RUN;
			
		} else  {
				DBG_GPS_PROF("[GPS-info]: CheckSum wrong!\n");	
		}
	 }
	return 1;
 }

static UINT8
appGpsSegCheckSum( 
	UINT8 *pstr
)
{
	UINT8 *pstr1 = pstr + 1;
	UINT8 cr1[4];
	UINT8 cr = 0;
	
	while(*pstr1 != '\0' && *pstr1 != '*'){
		cr ^= *pstr1++;
	}
	if(*pstr1 == '\0'){
		return FALSE;
	}  else {
		pstr1++;
	}
	sprintf(cr1, "%02X", cr);	
	return (strncmp(cr1, pstr1, 2))? FALSE : TRUE;
}

static void 
appGpsSegmentSave(
	UINT8      *pDestBuf,
	UINT8      *pSrc,	
	UINT32      bufLen,
	TX_MUTEX   *pMutex
)
{
	UINT32 err = 0;
	DBG_ASSERT(pSrc != NULL);
	DBG_ASSERT(pDestBuf != NULL);
	DBG_ASSERT(pMutex != NULL);
	DBG_ASSERT(bufLen > 0);

	if(strlen(pSrc) > bufLen)
		DBG_ASSERT(1);
	
	err = sp5kOsMutexGet(pMutex, 1);//write get Mutex wait 1ms
	if(err != SUCCESS){
		return;
	}
	
	memcpy(pDestBuf, pSrc, bufLen);

	err = sp5kOsMutexPut(pMutex);  //write release mutex
	DBG_ASSERT(err == SUCCESS);
	
	DBG_GPS_PROF("Num:%s\n",pDestBuf);
}

UINT8 
appGpsInfoValidCheck
(
	void
)
{
	UINT8  buf[8] = {0};
	
	appGpsInfoGet(GPS_INFO_GPSSTATUS, buf, sizeof(buf));
	if(!strncmp(buf,"A",1)) {
		return TRUE;		//A-Valid
	} else {
		return FALSE;	//V-Invalid
	}
}

/**
 * @fn        UINT8* appGpsCutInfo(UINT32 *len,UINT8  cnt)
 * @brief     search len betwen two char ',',return first ',' pointer
 * @param     [in] pStr:search string pointer
 * @param     [in] cnt:the idx of ','
 * @param     [out] bufOut:Get info len
 * @param     [out] bufLen:bufOut len
 * @retval    the char after find NO:cnt char ',' pointer 
 * @see       NULL
 * @author    
 * @since     2013-9-4
 * @todo      N/A
 * @bug       N/A
*/
/* copy segment info[cnt char ',' : cnt+1 char ','] to bufout  , */
static UINT8 
appGpsCutInfo(
	UINT8    *pStr,
	UINT8     cnt,
	UINT8     bufOut[],
	UINT32    buflen,
	TX_MUTEX *pMutex
)
{
	if((pStr == NULL) || (bufOut == NULL) || (cnt <= 0)){
		return 0;
	}
	
	UINT32 idx = 0;
	UINT8 *pStr_old = pStr;
	UINT8 *pStr_save = NULL;
	UINT8 *pStr_new = NULL;
	UINT32 len = 0;
	UINT32 err;

	err = sp5kOsMutexGet(pMutex, 1);//if can't get   wait 1ms
	if(err != SUCCESS){
		bufOut[0] = '\0';
		return 0;
	}
		
	for(idx = 0; idx < cnt; idx++){
		pStr_old = strchr((pStr_old + 1), ',');
		if(pStr_old == NULL){
			goto ERR_RETURN;
		}
	}
	pStr_new = strchr((pStr_old + 1), ',');
	if(pStr_new == NULL){
			pStr_new = strchr((pStr_old + 1), '*');
			if(pStr_new == NULL){
				goto ERR_RETURN;
			}
		}
	len = pStr_new - (pStr_old + 1);

	if(buflen < len){
		printf("buffer too small to get GPS info\n");
		goto ERR_RETURN;
	} 
	else {		
		memcpy(bufOut, pStr_old + 1, len);
		bufOut[len] = '\0';

		err = sp5kOsMutexPut(pMutex);
		DBG_ASSERT(err == SUCCESS);

		return len;
	}	
	
	ERR_RETURN:
		bufOut[0] = '\0';
		err = sp5kOsMutexPut(pMutex);
		DBG_ASSERT(err == SUCCESS);

		return -1;
		
}

/* delete first char c from string pStr */
void 
appGpsStrDelChar
(
	UINT8 *pStr,
	char   c
)
{
	if(pStr == NULL){
		return;
	}
	
	UINT8 *pChar = NULL;
	
	pChar = strchr(pStr, c);
	if(pChar != NULL){
		strcpy(&(pStr[pChar - pStr]), pChar + 1);
	}
}

UINT8 
appGpsInfoGet(
	UINT8  id,
	UINT8  bufOut[],
	UINT32 bufLen
)
{
	UINT32  len = 0;
	UINT8  *pStr = NULL;
	UINT8   buf[2] = {0};
	UINT32  cnt = 0;

	if((bufOut == NULL) || (bufLen <= 0))
		return 0;

	switch(id) {
		case GPS_INFO_LATITUDE:
			len = appGpsCutInfo(sGpsInfoRMC, RMC_LATITUDE, bufOut, bufLen, &sGpsRMCMutex);
			appGpsStrDelChar(bufOut, '.');//del char '.'
			break;
		case GPS_INFO_LATITUDE_IND:
			len = appGpsCutInfo(sGpsInfoRMC, RMC_LATITUDE_IND, bufOut, bufLen, &sGpsRMCMutex);
			break;
		case GPS_INFO_LONGITUDE:
			len = appGpsCutInfo(sGpsInfoRMC, RMC_LONGITUDE, bufOut, bufLen, &sGpsRMCMutex);
			appGpsStrDelChar(bufOut, '.');//del char '.'
			break;
		case GPS_INFO_LONGITUDE_IND:
			len = appGpsCutInfo(sGpsInfoRMC, RMC_LONGITUDE_IND, bufOut, bufLen, &sGpsRMCMutex);
			break;
		case GPS_INFO_PFI:
			len = appGpsCutInfo(sGpsInfoGGA, GGA_PFI, bufOut, bufLen, &sGpsGGAMutex);
			break;
		case GPS_INFO_MSLALTITUDE:
			len = appGpsCutInfo(sGpsInfoGGA, GGA_ALTITUDE, bufOut, bufLen, &sGpsRMCMutex);
			appGpsStrDelChar(bufOut,'.');
			break;
		case GPS_INFO_SPEED:
			len = appGpsCutInfo(sGpsInfoRMC, RMC_SPEED, bufOut, bufLen, &sGpsRMCMutex);
			pStr = strchr(bufOut, '.');//ignore string after '.'
			if(pStr != NULL)
				*pStr = '\0';
			sprintf(bufOut,"%d",(int)(atoi(bufOut) * 1852));
			break;
		case GPS_INFO_GPSSTATUS:
			len = appGpsCutInfo(sGpsInfoRMC, RMC_STATUS, bufOut, bufLen, &sGpsRMCMutex);
			break;;
		case GPS_INFO_DATE:
			len = appGpsCutInfo(sGpsInfoRMC, RMC_DATE, bufOut, bufLen, &sGpsRMCMutex);
			if(len > 5) {//ddmmyy -> yymmdd
				buf[0] = bufOut[0];
				buf[1] = bufOut[1];
				bufOut[0] = bufOut[4];
				bufOut[1] = bufOut[5];
				bufOut[4] = buf[0];
				bufOut[5] = buf[1];
			/*
				memcpy(buf, bufOut, 2);
				memcpy(bufOut, &bufOut[4], 2);
				memcpy(&bufOut[4], buf, 2);	*/		
			}
			break;
		case GPS_INFO_TIME://receive time format:hhmmss.nnn ,last three char ms 
			len = appGpsCutInfo(sGpsInfoRMC, RMC_TIME, bufOut, bufLen, &sGpsRMCMutex);
			pStr = strchr(bufOut, '.');//ignore string after '.'
			if(pStr != NULL)
				*pStr = '\0';
			break;
		case GPS_INFO_IMGDIRECTION:
			len = appGpsCutInfo(sGpsInfoRMC, RMC_IMGDIRECTION, bufOut, bufLen, &sGpsRMCMutex);
			appGpsStrDelChar(buf, '.');//del char '.'
			break;	
		case GPS_INFO_STATELLITE_VIEW_TOTAL:
			len = appGpsCutInfo(&sGpsInfoGSV[0][0], GSV_STATELLITE_TOTAL, bufOut, bufLen, &sGpsGSVMutex);
			break;	
		default:
			break;
	}
	return len;
}

UINT8
appGpsGsvInfoGet(
	UINT32 buf[][],
	UINT32 buflen
)
{
	UINT32 gsvInfo[16][4] = {0};
	UINT8  bufTotal[64] = {0};
	UINT32 *pTemp = &gsvInfo[0][0];
	UINT32 i,j;
	UINT32 msgTotal = 0;
	UINT32 satTotal = 0;
	UINT32 len = 0;
	UINT32 loopCnt = 0;

	if(appGpsCutInfo(&sGpsInfoGSV[0][0], GSV_MSG_TOTAL, bufTotal, sizeof(bufTotal), &sGpsGSVMutex)){
		msgTotal = atoi(bufTotal);
		DBG_GPS_PROF("msg total:%d\n",msgTotal);
	}
	appGpsInfoGet(GPS_INFO_STATELLITE_VIEW_TOTAL,bufTotal,sizeof(bufTotal));
	satTotal = atoi(bufTotal); 
	#if CAM_TYPE_CVR 
	gsatellitecount = satTotal;
	#endif
	if(satTotal < 1){
		memset(buf , 0, buflen);
		return 0;
	}
	
	for(i = 0; i < msgTotal - 1; i++){
		DBG_GPS_PROF("GSV:%s\n",&sGpsInfoGSV[i][0]);
		for(j = 0; j < 4*4; j++){
			len = appGpsCutInfo(&sGpsInfoGSV[i][0], GSV_STATELLITE_IDX + j, bufTotal, sizeof(bufTotal), &sGpsGSVMutex);
			*pTemp = atoi(bufTotal);
			pTemp++;
		}
	}

	if(satTotal%4){
		loopCnt = 4*(satTotal%4);
	} else {
		loopCnt = 4*4;
	}
	DBG_GPS_PROF("GSV:%s\n",&sGpsInfoGSV[msgTotal-1][0]);
	for(j = 0; j < loopCnt; j++){
		len = appGpsCutInfo(&sGpsInfoGSV[msgTotal-1][0], GSV_STATELLITE_IDX + j, bufTotal, sizeof(bufTotal), &sGpsGSVMutex);
		*pTemp = atoi(bufTotal);
		pTemp++;
	}

	if(buflen < satTotal*4){
		printf("buffer too small to get GSV info\n");
		return 0;
	} else {
		memcpy(buf, &gsvInfo[0][0], buflen);
		return satTotal*4;
	}
}


/**********************************************************************
	GPS info write to .MOV .txt
**********************************************************************/
void 
appGpsInfoHdrWriteEn(
	UINT8 en
)
{
	sGpsHdrEn = en;
}

void 
appGpsWrite2TxtEn(
	UINT8 en
)
{
	if(en){
		sGpsWriteTxtEn = TRUE;
	}  
	else{
		sGpsWriteTxtEn = FALSE;
	} 
}

void 
appGpathSave(
	void
)
{
	UINT8  *pStr;
	
	memset(sGpsCurrentPath, 0 , sizeof(sGpsCurrentPath));
	pStr = sp5kFsGetCurPath();
	if(pStr == NULL)
		return;
	
	if(strlen(pStr) < sizeof(sGpsCurrentPath))
		strcpy(sGpsCurrentPath, pStr);
	
	printf("path:%s\n",sGpsCurrentPath);
}

void
appUsrDataWrite(
	void
)
{	
	UINT32 idx = 0;

	sGpsInfoUpdat = FALSE;
	if(sGpsStatus == GPS_STOP){
		#if CAM_TYPE_CVR
		gps_update_RtcTime_flag=0;
		#endif
		memset(stamp_GPSSpeedBuf, 0 , sizeof(stamp_GPSSpeedBuf));
		sprintf(stamp_GPSSpeedBuf,"%3d%s",0,"km/h");
		
		appGpsInfoWrite2MovWhenGPSStop();
		appGsensorInfoWrite2Mov();
	}
	else{
		appGpsInfoWrite2Mov();
		appGsensorInfoWrite2Mov();
		
		#if CAM_TYPE_CVR
		//xuan.ruan add for fix mantis bug@2014.01.10
		if((gps_update_RtcTime_flag==0) && (TRUE == appGpsInfoValidCheck()))
		{
			RTCTimeDataCheckCout++;
			if(RTCTimeDataCheckCout>5)
			{
				/*update RTC time*/
				appRtcSet(&rtctrue);
				RTCTimeDataCheckCout=0;
				gps_update_RtcTime_flag=1;
			}
		}
		#endif
		if(sGpsWriteTxtEn == TRUE) {
			if(sGpsInfoFd == 0){
				UINT32     curIdx = 0;
				UINT8	   fileName[32] = {0};
				
				sp5kDcfFsCurFileIdxGet(&curIdx);
				if(sGpsCurrentPath[2] == '\0'){
					strcat(sGpsCurrentPath, PATH_CHECK);
				}

				
				sprintf(fileName, "%s%s%s%04d%s",sGpsCurrentPath,"/","SUNP",++curIdx, ".txt");			
				printf("Create file name :%s\n", fileName);
						
				sGpsInfoFd = sp5kFsFileOpen(fileName, SP5K_FS_OPEN_CREATE);
				if(sGpsInfoFd == 0) {
					printf("Create file fail!\n");
					return;
				}
				sp5kFsFileSeek(sGpsInfoFd, 0, SP5K_FS_SEEK_SET);
			}	
			
			sp5kFsFileWrite(sGpsInfoFd, sGpsInfoGGA, strlen(sGpsInfoGGA));
			sp5kFsFileWrite(sGpsInfoFd, sGpsInfoGSA, strlen(sGpsInfoGSA));
			for(idx = sGpsInfoGSV[1][9]; idx <= sGpsInfoGSV[1][7]; idx++){
				sp5kFsFileWrite(sGpsInfoFd,&(sGpsInfoGSV[idx-'0'][0]),strlen(&(sGpsInfoGSV[idx-'0'][0])));
			}
			sp5kFsFileWrite(sGpsInfoFd,sGpsInfoRMC,strlen(sGpsInfoRMC));
		}	
	}

	sGpsInfoUpdat = TRUE;
	sGpsStatus = GPS_STOP;
}


void appGpsInfoWrite2MovWhenGPSStop(
	void
)
{
	UINT8 vtagstr[512];
	UINT8 buf[128];
	UINT8 datebuf[9];
	UINT32 len;
	UINT32 idx = 0;

	memset(vtagstr, 0 , sizeof(vtagstr));
	memset(datebuf, 0 , sizeof(datebuf));

	if(sGpsHdrEn){
		UINT8 dir[64];
		appRTC_t tt;
		sGpsUseDataCnt = 0;

		/*Dir */
		memset(dir, 0, sizeof(dir));
		if(sGpsCurrentPath[2] != '/'){  /*dcf */
			strncpy(dir, PATH_CHECK, 8);
		}else{	/* cdfs */
			strncpy(dir, &sGpsCurrentPath[3], 8);
		}
		
		strcpy(vtagstr,dir);

		memset(buf, 0 , sizeof(buf));
		memset(buf, 0x55 , 0x1d-0x08+1);
		strcat(vtagstr,buf);

		memset(buf, 0 , sizeof(buf));
		memset(buf, 0xFE , 2);
		strcat(vtagstr,buf);

		appRtcGet(&tt);
		memset(buf, 0 , sizeof(buf));
		sprintf(buf,"%04d%02d%02d%02d%02d%02d",tt.year,tt.mon,tt.day,tt.hour,tt.min,tt.sec);
		strcat(vtagstr,buf);

		memset(buf, 0 , sizeof(buf));
		memset(buf, 0x0C , 1);
		strcat(vtagstr,buf);

		len = strlen(vtagstr);
		sGpsUseDataCnt += len;
		appGpsInfoHdrWriteEn(0);
	}

	strcpy(vtagstr, GPS_NULL_INFO);
	
	appOsd_ColorDrawTextColorSet(PAL_RED, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
	appOsd_GfxStringDraw(80, 80, SP5K_GFX_ALIGN_CENTER, "GPS OFF !!");

	len = strlen(vtagstr);
	sGpsUseDataCnt += len;
	sp5kMediaRecUserDataUpdate(vtagstr, len, 0, 1);
}

void
appGpsInfoWrite2Mov(
	void
)
{
	UINT8 vtagstr[512];
	UINT8 buf[128];
	UINT8 datebuf[9];
	UINT32 len;
	UINT32 idx = 0;
	#if CAM_TYPE_CVR
	appRTC_t GPStime;	
	uiPara_t* puiPara = appUiParaGet();
	#endif
	/*add info ID you want write to .MOV file*/
	UINT32 selIdTable[] = {GPS_INFO_DATE,GPS_INFO_TIME,GPS_INFO_PFI,
						  GPS_INFO_LATITUDE_IND,GPS_INFO_LATITUDE,
						  GPS_INFO_LONGITUDE_IND,GPS_INFO_LONGITUDE,
						  GPS_INFO_MSLALTITUDE,GPS_INFO_SPEED
						  };

	memset(vtagstr, 0 , sizeof(vtagstr));
	memset(datebuf, 0 , sizeof(datebuf));

	if(sGpsHdrEn){
		UINT8 dir[64];
		appRTC_t tt;
		sGpsUseDataCnt = 0;

		/*Dir */
		memset(dir, 0, sizeof(dir));
		if(sGpsCurrentPath[2] != '/'){  /*dcf */
			strncpy(dir, PATH_CHECK, 8);
		}else{	/* cdfs */
			strncpy(dir, &sGpsCurrentPath[3], 8);
		}
		
		strcpy(vtagstr,dir);

		memset(buf, 0 , sizeof(buf));
		memset(buf, 0x55 , 0x1d-0x08+1);
		strcat(vtagstr,buf);

		memset(buf, 0 , sizeof(buf));
		memset(buf, 0xFE , 2);
		strcat(vtagstr,buf);

		appRtcGet(&tt);
		memset(buf, 0 , sizeof(buf));
		sprintf(buf,"%04d%02d%02d%02d%02d%02d",tt.year,tt.mon,tt.day,tt.hour,tt.min,tt.sec);
		strcat(vtagstr,buf);

		memset(buf, 0 , sizeof(buf));
		memset(buf, 0x0C , 1);
		strcat(vtagstr,buf);

		len = strlen(vtagstr);
		sGpsUseDataCnt += len;
		appGpsInfoHdrWriteEn(0);
	}

	memset(buf, 0 , sizeof(buf));

	
	if(!appGpsInfoValidCheck()) {
		
	}
	appOsd_ColorDrawTextColorSet(PAL_RED, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
	if(appGpsInfoValidCheck()) {
		appOsd_GfxStringDraw(80, 80, SP5K_GFX_ALIGN_CENTER, "GPS ON !!");
		
		for(idx = 0; idx < sizeof(selIdTable)/sizeof(selIdTable[0]); idx++) {
		appGpsInfoGet(selIdTable[idx], buf, sizeof(buf));
		
		#if CAM_TYPE_CVR  /*add for timezone*/
		if(selIdTable[idx] == GPS_INFO_DATE)
		{
			appGPS_Time_String_ASCII_Get(&GPStime,selIdTable[idx],buf);
			strcat(datebuf,buf);
		}
		else if(selIdTable[idx] == GPS_INFO_TIME)
		{
			appGPS_Time_String_ASCII_Get(&GPStime,selIdTable[idx],buf);
			appTimezone(&GPStime,puiPara->TimeZones);
			memcpy(&rtctrue,&GPStime,sizeof(GPStime));
			appGPS_Time_String_Get(&GPStime,selIdTable[idx],buf);
			strcat(vtagstr,datebuf);
			strcat(vtagstr,buf);
		}
		else if(selIdTable[idx] == GPS_INFO_SPEED)
		{
			memset(stamp_GPSSpeedBuf, 0 , sizeof(stamp_GPSSpeedBuf));
			sprintf(stamp_GPSSpeedBuf, "%s%s", buf, "km/h");
			strcat(vtagstr,buf);
		}
		else
		{
			strcat(vtagstr,buf);
		}
		#else
		strcat(vtagstr,buf);
		#endif
	}
	} else {
		appOsd_GfxStringDraw(80, 80, SP5K_GFX_ALIGN_CENTER, "GPS OFF !!");
		strcpy(vtagstr, GPS_NULL_INFO);
	}

	len = strlen(vtagstr);
	sGpsUseDataCnt += len;
	sp5kMediaRecUserDataUpdate(vtagstr, len, 0, 1);

	DBG_GPS_PROF("GPS info %s\n",vtagstr);
}

void
appUsrDataWriteEnd(
	void
)
{
	UINT8  vtagstr[512];
	UINT8  buf[128];
	UINT32 len;
	UINT32 curIdx;

	if(sGpsInfoUpdat == FALSE){
		return;
	}

	memset(vtagstr, 0 , sizeof(vtagstr));

	memset(buf, 0 , sizeof(buf));
	memset(buf, 0x30 , 72);
	strcat(vtagstr,buf);

	memset(buf, 0 , sizeof(buf));
	memset(buf, 0xFD , 2);
	strcat(vtagstr,buf);

	memset(buf, 0 , sizeof(buf));
	memset(buf, 0x30 , 24);
	strcat(vtagstr,buf);

	memset(buf, 0 , sizeof(buf));
	//sp5kMediaRecAttrGet(SP5K_MEDIA_ATTR_FILE_NAME, buf);printf("buf == %s", buf);
	#if SP5K_CDFS_OPT
	memset(buf, 0x58 , 12);
	#else
	sp5kDcfFsCurFileIdxGet(&curIdx);printf("---curIdx = %d", curIdx);
	sprintf(buf, "%s%04d%s","SUNP", ++curIdx, ".MOV");
	#endif
	strcat(vtagstr,buf);

	len=strlen(vtagstr);
	sGpsUseDataCnt += len;
	sGpsUseDataCnt += 14;

	memset(buf, 0 , sizeof(buf));
	sprintf(buf,"%12x",sGpsUseDataCnt);
	strcat(vtagstr,buf);

	memset(buf, 0 , sizeof(buf));
	memset(buf, 0xFC , 2);
	strcat(vtagstr, buf);
	len += 14;

	sp5kMediaRecUserDataUpdate(vtagstr, len, 0, 1);

	if(sGpsInfoFd == 0){
		return;
	}
	sp5kFsFileClose(sGpsInfoFd);
	sGpsInfoFd = 0;
}
