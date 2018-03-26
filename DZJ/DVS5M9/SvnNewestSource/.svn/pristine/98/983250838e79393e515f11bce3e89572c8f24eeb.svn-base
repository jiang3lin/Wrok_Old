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
 
/**************************************************************************
* 							 M A C R O S							   *
**************************************************************************/
#define ICON_SHOW	1
#define ICON_CLEAN	0

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

typedef enum iconMode_e{
	APP_ICON_MODE_STLLVIEW,
	APP_ICON_MODE_STLLVIEW_HDMI,
	APP_ICON_MODE_VIDEOVIEW,
	APP_ICON_MODE_VIDEOVIEW_HDMI,
	APP_ICON_MODE_AUDIO,
	APP_ICON_MODE_AUDIO_HDMI,
	APP_ICON_MODE_PLAYBACK,
	APP_ICON_MODE_PLAYBACK_HDMI,
	APP_ICON_MODE_VIDEOPLAY,
	APP_ICON_MODE_VIDEOPLAY_HDMI,
	APP_ICON_MODE_VIDEOPLAY_HDMI_DVS5M2,
	APP_ICON_MODE_MAX
}iconMode_t;

typedef struct iconLayoutFile_s{
	UINT8 *fileName;
}iconLayoutFile_t;

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/

UINT32 appOsdIconModeSet(UINT32 mode);
UINT32 appOsdIconPosGet(UINT32 iconID,UINT16 *x,UINT16 *y);
UINT32 appIconShow(UINT32 iconID,UINT8 show,UINT32 align);

