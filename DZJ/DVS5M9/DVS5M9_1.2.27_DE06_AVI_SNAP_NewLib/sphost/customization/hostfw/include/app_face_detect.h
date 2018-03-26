/**************************************************************************
 *                                                                                                                               *
 *         Copyright (c) 2008 by SunplusMM Technology Co., Ltd.                                       *
 *                                                                                                                               *
 *  This software is copyrighted by and is the property of SunplusMM                                *
 *  Technology Co., Ltd. All rights are reserved by SunplusMM Technology                        *
 *  Co., Ltd. This software may only be used in accordance with the                                 *
 *  corresponding license agreement. Any unauthorized use, duplication,                          *
 *  distribution, or disclosure of this software is expressly forbidden.                                *
 *                                                                                                                               *
 *  This Copyright notice MUST not be removed or modified without prior                          *
 *  written consent of SunplusMM Technology Co., Ltd.                                                    *
 *                                                                                                                               *
 *  SunplusMM Technology Co., Ltd. reserves the right to modify this                                *
 *  software without notice.                                                                                           *
 *                                                                                                                               *
 *  SunplusMM Technology Co., Ltd.                                                                               *
 *  19, Innovation First Road, Science-Based Industrial Park,                                           *
 *  Hsin-Chu, Taiwan, R.O.C.                                                                                        *
 *                                                                                                                               *
 **************************************************************************/

#ifndef _APP_FACE_DETECT_H_
#define _APP_FACE_DETECT_H_

/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define MAX_SUPPORT_FACE_NUM	5
#define MAX_SUPPORT_OBJECT_NUM	5		/* Add by sc.liu */
#define SIZE_FACE_DETECT_WIDTH	160
#define SIZE_FACE_DETECT_HEIGHT	120

#define SIZE_SMILE_DETECT_WIDTH	320
#define SIZE_SMILE_DETECT_HEIGHT	240

#define VIEW_SCREEN_WIDTH		320
#define VIEW_SCREEN_HEIGHT	240


typedef enum {	
	FACE_DETECT_ONLY = 0x00,
	SMILE_DETECT,
	//BLINK_DETECT,
	OPTION_TOTAL
} appFdIniOption_e;

/* Add by sc.liu */
typedef enum {	
	OBJECT_DETECT_ONLY = 0x00,
	SMILE_DETECT,
	OPTION_TOTAL
} appOdIniOption_e;
/* Add by sc.liu */

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

typedef struct {
	SINT16 nLeft;
	SINT16 nTop;
	SINT16 nRight;
	SINT16 nBottom;
	BOOL isSmile;
} appFaceInfo_t;

typedef struct appFdResult_s{
	UINT32 totalFaceNum;
	UINT32 mainFace;
	appFaceInfo_t rectFace[MAX_SUPPORT_FACE_NUM];
}appFdResult_t;

/* Add by sc.liu */
typedef struct {
	SINT16 nLeft;
	SINT16 nTop;
	SINT16 nRight;
	SINT16 nBottom;
	BOOL isSmile;
} appObjectInfo_t;

typedef struct appOdResult_s{
	UINT32 totalObjectNum;
	UINT32 mainObject;
	appObjectInfo_t rectObj[MAX_SUPPORT_OBJECT_NUM];
}appOdResult_t;
/* Add by sc.liu */


typedef UINT32 appSmileDetectCBF_t(UINT32 newfacenum,  appFaceInfo_t *newface);

/**************************************************************************
 *                               M A C R O S                              *
 **************************************************************************/


/**************************************************************************
 *          M O D U L E   V A R I A B L E S   R E F E R E N C E S         *
 **************************************************************************/

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/
void appSmileDetectCallbackSet(appSmileDetectCBF_t *pfun);
UINT32 appFdCreate(appFdIniOption_e iniFlag);
void appFdDestroy(void);
void appFdProcStart(void);
void appFdProcPause(void);
void appFdProcStop(void);
UINT32 appFdCoreProcess(UINT32 addr, 	UINT32 zoomratio);
UINT32 appFdFaceDataGet(appFdResult_t *fdResult);
UINT32 appOdObjectDataGet(appOdResult_t *odResult);		/* Add by sc.liu */


#endif  /* _APP_FACE_DETECT_H_ */



