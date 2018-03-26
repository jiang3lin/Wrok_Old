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

#ifndef _APP_ERROROSD_H_
#define _APP_ERROROSD_H_


#include "app_stringid.h"
#include "app_menu_tab.h"


/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

#define TK_WARNING_X  60
#define TK_WARNING_Y  65
#define TK_WARNING_W  200
#define TK_WARNING_H  110
#define TK_WARNING_ICON_X  (TK_WARNING_X+(TK_WARNING_W/2)-23)
#define TK_WARNING_ICON_Y  TK_WARNING_Y+10
#define TK_WARNING_TEXT_CLEAR_W  (TK_WARNING_W-8)
#define TK_WARNING_TEXT_CLEAR_H  40

#define TK_WARNING_VOICEMEMO_Y  70

#define TK_WARNING_FRAME2_X  40
#define TK_WARNING_FRAME2_Y  65
#define TK_WARNING_FRAME2_W  240
#define TK_WARNING_FRAME2_H  110
#define TK_WARNING_FRAME2_ICON_X  (TK_WARNING_FRAME2_X+(TK_WARNING_FRAME2_W/2)-23)
#define TK_WARNING_FRAME2_ICON_Y  TK_WARNING_FRAME2_Y+10
#define TK_WARNING_FRAME2_TEXT_CLEAR_W  (TK_WARNING_FRAME2_W-8)


/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/



/**************************************************************************
 *                           VARIABLE                          *
 **************************************************************************/


/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/



/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/
void  appOsdMessageBoxShow(BOOL bShow, unsigned short usStrId);
void  appErrorOsd_VoiceMemoWarnScreenDraw(BOOL bShow, unsigned short usStrId);
void  appErrorOsd_BlackWarnScreen(BOOL bShow, unsigned short usStrId);

#endif /* _APP_ERROROSD_H_ */



