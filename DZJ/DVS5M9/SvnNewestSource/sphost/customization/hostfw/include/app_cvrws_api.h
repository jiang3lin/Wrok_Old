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
#ifndef APP_CVRWS_API_H
#define APP_CVRWS_API_H

/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/
#if ICAT_FCWS
#include "api/sp5k_ftrack_api.h"
#endif

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define ICAT_CVR_WARN_SYS_DBG	0
/* For iCatch internal debug using, default is 0 */
#define CVR_WARN_SYS_INT_DBG	0
#if CVR_WARN_SYS_INT_DBG
#undef ICAT_CVR_WARN_SYS_DBG
#define ICAT_CVR_WARN_SYS_DBG	1
#endif

#define CVRWS_STR	"[CVRWS]:"
#define CENTRALPOINT_X			160
#define CENTRALPOINT_Y			120
#define CENTRALPOINT_CNT_MAX	10
#define CENTRALPOINT_UNIT		2

#if ICAT_FCWS
#define FCWS_STR	"[FCWS]:"

#if 1
#define SENSOR_FOCAL_LEN	(3.6*10000)
#define SENSOR_PIXEL_UNIT	158
#endif

#define FCWS_SENSOR_FOCAL		SENSOR_FOCAL_LEN	/* SENSOR_FOCAL = focal length(mm)*10000 */
#define FCWS_SENSOR_PIXEL_UNIT	SENSOR_PIXEL_UNIT	/* SENSOR_PIXEL_UNIT = (max_sensor_horizontal_resolution)*(sensor unit size(um))/320*10 */
#define FCWS_WARN_DISTANCE		30
/* FCWS_ROI_CROP_FACTOR+FCWS_ROI_CROP_OFFSET*2 = 1024
* 320* FCWS_ROI_CROP_FACTOR/1024(integer) = 4N
*/
#define FCWS_ROI_CROP_FACTOR	487
#define FCWS_ROI_CROP_OFFSET	268

#define FCWS_WARN_CNT_DBG		10
#endif

#if ICAT_LDWS
#define LDWS_STR	"[LDWS]:"

#define LDWS_ANGLE	60
#define LDWS_THR	32

#define LDWS_SAFE_COLOR		PAL_YELLOW
#define LDWS_WARN_COLOR		PAL_RED

#define LDWS_WARN_CNT_DBG		15
#define LDWS_WARN_CNT_DBG_REC	30

#endif

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef enum {
	CVRWS_CALIB_MOVE_RIGHT,
	CVRWS_CALIB_MOVE_LEFT,
} appCvrwsCalib_e;

#if ICAT_FCWS
typedef struct appFCWSInfo_s {
	UINT16 vStartX;
	UINT16 vStartY;
	UINT16 vSize;
	UINT32 attr;
} appFCWSInfo_t;
#endif /* ICAT_FCWS */

#if ICAT_LDWS
typedef struct appLDWSInfo_s {
	UINT32 ftx1;
	UINT32 ftx2;
	UINT32 ftx3;
	UINT32 ftx4;
	UINT32 fty1;
	UINT32 fty2;
	UINT32 fty3;
	UINT32 fty4;
	UINT32 ROI_h_end;
} appLDWSInfo_t;
#endif /* ICAT_LDWS */

/**************************************************************************
 *                               M A C R O S                              *
 **************************************************************************/

/**************************************************************************
 *          M O D U L E   V A R I A B L E S   R E F E R E N C E S         *
 **************************************************************************/
#if ICAT_FCWS
extern appFCWSInfo_t fcwsInfo;
extern UINT16 Vehicle_LV;
#if CVR_WARN_SYS_INT_DBG
extern UINT32 fcwsWarnCnt;
#endif
#endif

#if ICAT_LDWS
#if CVR_WARN_SYS_INT_DBG
extern UINT32 ldwsBothCnt, ldwsRightCnt, ldwsLeftCnt;
#endif
#endif

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/
void appCvrwsOsdDraw(void);
void appCvrwsCentralPointCal(void);
void appCvrwsCentralPointUpdate(appCvrwsCalib_e param);
void appCvrwsCentralPointSet(void);
void appCvrwsEnableSet(UINT8 enable);

#if ICAT_FCWS
void appForwardCollisionWarnSysParamSet(void);
void appForwardCollisionWarningDayNightExchange(UINT16 lvIdx);
#if ICAT_CVR_WARN_SYS_DBG
void appForwardCollisionWarningInfo(void);
void appFCWS_DbgMsg(UINT32 stampPageId);
#endif
#endif /* ICAT_FCWS */

#if ICAT_LDWS
void appLaneDepartureWarnSysParamSet(void);
UINT32 appCvrWarningSystemFrameProc(UINT32 frmId, UINT8 *buf, UINT32 width, UINT32 height);
void appLaneDepartureWarnSysCoorTranslate(void);
void appLaneDepartureWarningDraw(UINT32 param);
void appLaneDepartureWarningUpdate(UINT32 param);
#if ICAT_CVR_WARN_SYS_DBG
void appLaneDepartureWarningDbg(UINT32 param);
void appLDWS_DbgMsg(UINT32 stampPageId);
#endif
#endif /* ICAT_LDWS */

#endif /* APP_CVRWS_API_H */

