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
 #define USERMC_SPEED
#ifndef _APP_GPS_H_
#define _APP_GPS_H_

/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef enum { 
	GPS_STOP	= 0x0,	
	GPS_RUN,	
} GPS_STATUS_e;

typedef enum { 
	GPS_INFO_LATITUDE = 0x0,	
	GPS_INFO_LATITUDE_IND,	
	GPS_INFO_LONGITUDE,	
	GPS_INFO_LONGITUDE_IND,
	GPS_INFO_PFI,
	GPS_INFO_MSLALTITUDE,
	GPS_INFO_SPEED,
	GPS_INFO_GPSSTATUS,
	GPS_INFO_DATE,
	GPS_INFO_TIME,
	GPS_INFO_IMGDIRECTIONREF,
	GPS_INFO_IMGDIRECTION,
	GPS_INFO_STATELLITE_VIEW_TOTAL,
} GPS_INFO_e;

/* valid data list of GPRMC info   */
typedef struct _GPRMC_s {
	UINT8 startFlag;     	/* char '$'  as start flag */
	UINT8 segName[5];		/*segment name string "GPRMC" */
	UINT8 partFlag_1;		/* char ','  as seprerate flag */
	UINT8 timeUTC[6];		/* time UTC format : hhmmss */
	UINT8 ignore_1[4];		/* ignore the num after point */
	UINT8 partFlag_2;		/* char ','  as seprerate flag */
	UINT8 valid;			/* whether info valid 'A'=Valid */
	UINT8 partFlag_3;		/* char ','  as seprerate flag */
	UINT8 Latitude[9];		/* Latitude info format: ddmm.mmmm*/
	UINT8 partFlag_4;		/* char ','  as seprerate flag */
	UINT8 LatitudeInd; 		/* LatitudeInd 'N' or 'S' */
	UINT8 partFlag_5;		/* char ','  as seprerate flag */
	UINT8 Longitude[10];	/* Longitude format:dddmm.mmmm*/
	UINT8 partFlag_6;		/* char ','  as seprerate flag */
	UINT8 LongitudeInd;		/* LongitudeInd 'E' or 'W' */
	UINT8 partFlag_speed;		/* char ','  as seprerate flag */
	//UINT8 speed[4];		/* speed, when speed is 0 rec char 0.0 char [3] other char [4]*/
	//UINT8 partFlag_8;		/* char ','  as seprerate flag */
	//UINT8 direction[6];		/* direction*/
	//UINT8 partFlag_9;		/* char ','  as seprerate flag */
	//UINT8 dataUTC[6];		/* UTC date format: ddmmyy*/
	/* other info  ignore*/	
} GPRMC_s;

/* valid data list of GPGGA info   */
typedef struct _GPGGA_s {
	UINT8 startFlag;     	/* char '$'  as start flag       */
	UINT8 segName[5];		/*segment name string "GPRMC"    */
	UINT8 partFlag_1;		/* char ','  as seprerate flag   */
	UINT8 timeUTC[6];		/* time UTC format : hhmmss      */
	UINT8 ignore_1[4];		/* ignore the num after point .000 */
	UINT8 partFlag_2;		/* char ','  as seprerate flag     */
	UINT8 Latitude[9];		/* Latitude info format: ddmm.mmmm*/
	UINT8 partFlag_3;		/* char ','  as seprerate flag    */
	UINT8 LatitudeInd; 		/* LatitudeInd 'N' or 'S'         */
	UINT8 partFlag_4;		/* char ','  as seprerate flag */
	UINT8 Longitude[10];	/* Longitude format:dddmm.mmmm*/
	UINT8 partFlag_5;		/* char ','  as seprerate flag */
	UINT8 LongitudeInd;		/* LongitudeInd 'E' or 'W' */
	UINT8 partFlag_6;		/* char ','  as seprerate flag */
	UINT8 PFI;		/* status                      */
	UINT8 partFlag_7;		/* char ','  as seprerate flag */
	UINT8 statelliteNum[2];		/* NO of satellite            */
	UINT8 partFlag_hdop;		/* char ','  as seprerate flag */
	//UINT8 horDivi[4];		/* hor divi*/
	//UINT8 partFlag_9;		/* char ','  as seprerate flag */
	//UINT8 altitude[4];			/* height above sea leave */
	/* other info  ignore*/	
} GPGGA_s;


/**************************************************************************
 *                               M A C R O S                              *
 **************************************************************************/

/**************************************************************************
 *          M O D U L E   V A R I A B L E S   R E F E R E N C E S         *
 **************************************************************************/

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/
void  appGpsUartInit(void);
void  appGpathSave(void);
void  appGpsInfoHdrWriteEn(UINT8 en);
void  appGpsWrite2TxtEn(UINT8 en);
UINT8  appGpsInfoGet(UINT8 id,UINT8 buf[],UINT32 bufLen);
UINT8  appGpsGsvInfoGet(UINT32 buf[][],UINT32 buflen);
UINT8 appGpsInfoValidCheck(void);
void  appUsrDataWrite(void);
void  appGpsInfoWrite2MovWhenGPSStop(void);
void  appGpsInfoWrite2Mov(void);
void  appUsrDataWriteEnd(void);


#endif  /* _APP_GPS_H_ */


