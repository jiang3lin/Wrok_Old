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

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef enum{
	TS_ON,  /*0: on ; 1:off */
	TS_OFF
}TS_T;

typedef struct appFileQue_s{
	UINT32 DirIdx;
	UINT32 FileIdx;
} appFileQue_t;

#define OLD_TLS_OPT	0

/**************************************************************************
 *				 F U N C T I O N	D E C L A R A T I O N S 			  *
 **************************************************************************/
#if OLD_TLS_OPT
void appStillTimelapseTaskInit(UINT32 Tflag);
void appTimelapseInit(void);
void appMediaTimeLapse(UINT8 *jpgFile);
void appJpgFileNameSet(UINT8 *jpgFile);
UINT8 *appJpgFileNameGet(void);
void appTlsGapProc(void);
#endif
UINT32 appTimLapKeyMsgDispose(UINT32 msg,UINT32 param);
UINT32 appTimelapsePreviewCallback(UINT32 fid,UINT8 *pbuf,UINT32 w,UINT32 h);
UINT32 appTimelapseSwTrigCb(UINT8 *pbuf, sp5kYuvCbImgSize_t *pinfo, UINT32 rsvd0, UINT32 rsvd1);
void appTimelapseStart(void);
void appTimelapseStop(void);
void appTimelapseDateStamp(void);
