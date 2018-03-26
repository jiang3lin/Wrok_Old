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
#ifndef _APP_FACE_DETECT_EX_H_
#define _APP_FACE_DETECT_EX_H_

/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/
 
#include "app_mmface_detect.h"


/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
void appFdBlinkDetectSemInit(void);
void appFdBlinkDetectSemPut(void);
void appFdBlinkDetectSemGet(void);
void  appFdBlinkDetectSet(UINT32 set);
UINT32 appFdCreateEx(appFdIniOption_e iniFlag);
void appFdDestroyEx(void);
void appFdProcStartEx(void);
void appFdProcPauseEx(void);
void appFdProcStopEx(void);
UINT32 appFdFaceDataGetEx(appFdResult_t *fdResult);
UINT32 appOdObjectDataGetEx(appOdResult_t *odResult);		/* Add by sc.liu */
/*s Add by Aries 09/10/30*/
UINT32 appFdBlinkingDetectCheck(UINT32 yuvAddr,UINT32 qvgaAddr,void * psSizeInfo);
void appFdBlinkDetectCheckEnd(void);


#endif  /* _APP_FACE_DETECT_EX_H_ */



