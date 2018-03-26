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



#ifndef _APP_OSDLIB_H_
#define _APP_OSDLIB_H_

#include "app_osd_layout.h"


/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/


/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

typedef enum appOsdDrawShapeType_s{
    APP_OSD_SHAPE_RECT               			= 0x00,
    APP_OSD_SHAPE_CIRCLE             			= 0x01,
    APP_OSD_SHAPE_ELLIPSE					= 0x02,
    APP_OSD_SHAPE_LINE             				= 0x03,
    APP_OSD_SHAPE_PIXEL					= 0x04
} appOsdDrawShapeType_t;

/**************************************************************************
 *                           VARIABLE                          *
 **************************************************************************/


#define OSD_FACTOR_X_NUMERATOR					320
#define OSD_FACTOR_X_DENOMINATOR					320

#define OSD_FACTOR_Y_NUMERATOR					240
#define OSD_FACTOR_Y_DENOMINATOR					240

#define OSD_DEFAULT_FRAMSIZE_W					320
#define OSD_DEFAULT_FRAMSIZE_H					240

#define OSD_FACTOR_X_NUMERATOR_LCM				96
#define OSD_FACTOR_X_DENOMINATOR_LCM			96

#define OSD_FACTOR_Y_NUMERATOR_LCM				32
#define OSD_FACTOR_Y_DENOMINATOR_LCM			32

#define OSD_DEFAULT_FRAMSIZE_W_LCM				96
#define OSD_DEFAULT_FRAMSIZE_H_LCM				32

#define CLEAR_OSD_SRC		appOsdLib_OsdLayerClearDraw();	
#define CLEAR_PIP_SRC		appOsdLib_PIPLayerClearDraw();


/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/

void appOsdInit(void);
void appOsdResPreload(void);
void appOsdInit_TVModeSet(unsigned char dispMode);
void appOsdInit_LCDModeSet(unsigned char dispMode);

void appOsdLib_CoordScaleSet(SINT32 *x, SINT32 *y);
void appOsdLib_TextUsrDraw(SINT32 x,SINT32 y,UINT32 align, UINT8 *szStr);
void appOsdLib_TextDraw(SINT32 x,SINT32 y,UINT32 align, UINT8 *szStr);
void appOsdLib_TextPIPDraw(SINT32 x,SINT32 y,UINT32 align, UINT8 *szStr);
void appOsdLib_TextIDUsrDraw(SINT32 x, SINT32 y,UINT32 align,UINT32 StrId);
void appOsdLib_TextIDDraw(SINT32 x, SINT32 y,UINT32 align,UINT32 StrId);
void appOsdLib_TextIDPIPDraw(SINT32 x, SINT32 y,UINT32 align,UINT32 StrId);
void appOsdLib_UsrLayIconDraw(SINT32 x, SINT32 y,UINT32 align,UINT32 IconId);
void appOsdLib_OsdLayIconDraw(SINT32 x, SINT32 y,UINT32 align,UINT32 IconId);
void appOsdLib_PIPLayIconDraw(SINT32 x, SINT32 y,UINT32 align,UINT32 IconId);
void appOsdLib_IconFileDraw(UINT32 page,SINT32 x,SINT32 y,UINT32 align,UINT8 *fileName,UINT32 IconIndex);
void appOsdLib_UsrLayerShapeDraw(UINT8 ShapeType,SINT32 StartX,SINT32 StartY,SINT32 Width, SINT32 High,SINT32 radiusX,SINT32 radiusY);
void appOsdLib_UsrLayerLineDraw(SINT32 x0,SINT32 yo,SINT32 x1, SINT32 y1);
void appOsdLib_UsrLayerFrameDraw(SINT32 x, SINT32 y, UINT32 w, UINT32 h, UINT32 depth);
void appOsdLib_OsdLayerShapeDraw(UINT8 ShapeType,SINT32 StartX,SINT32 StartY,SINT32 Width, SINT32 High,SINT32 radiusX,SINT32 radiusY);
void appOsdLib_OsdLayerLineDraw(SINT32 x0,SINT32 yo,SINT32 x1, SINT32 y1);
void appOsdLib_OsdLayerFrameDraw(SINT32 x, SINT32 y, UINT32 w, UINT32 h, UINT32 depth);
void appOsdLib_PIPLayerShapeDraw(UINT8 ShapeType,SINT32 StartX,SINT32 StartY,SINT32 Width,SINT32 High,SINT32 radiusX,SINT32 radiusY);
void appOsdLib_PIPLayerLineDraw(SINT32 x0,SINT32 yo,SINT32 x1,SINT32 y1);
void appOsdLib_TransparentLayerShapeDraw(UINT8 ShapeType,SINT32 StartX,SINT32 StartY,SINT32 Width,SINT32 High,SINT32 radiusX,SINT32 radiusY);
void appOsdLib_TransparentLayerShapeClear(UINT8 ShapeType,SINT32 StartX,SINT32 StartY,SINT32 Width,SINT32 High,SINT32 radiusX,SINT32 radiusY);
void appOsdLib_OsdLayerShapeClear(UINT8 ShapeType,SINT32 StartX, SINT32 StartY,SINT32 Width, SINT32 High,SINT32 radiusX,SINT32 radiusY);
void appOsdLib_OsdLayerLineClear(SINT32 x0, SINT32 yo,SINT32 x1, SINT32 y1);
void appOsdLib_PIPLayerShapeClear(UINT8 ShapeType,SINT32 StartX, SINT32 StartY,SINT32 Width, SINT32 High,SINT32 radiusX,SINT32 radiusY);
void appOsdLib_PIPLayerLineClear(SINT32 x0,SINT32 yo,SINT32 x1, SINT32 y1);
void appOsdLib_PFLayerShapeDraw(UINT8 ShapeType,SINT32 StartX,SINT32 StartY,SINT32 Width,SINT32 High,SINT32 radiusX,SINT32 radiusY);
void appOsdLib_PFLayerLineDraw(SINT32 x0,SINT32 yo,SINT32 x1, SINT32 y1);
void appOsdLib_PFLayerShapeClear(UINT8 ShapeType,SINT32 StartX,SINT32 StartY,SINT32 Width,SINT32 High,SINT32 radiusX,SINT32 radiusY);
void appOsdLib_PFLayerLineClear(SINT32 x0,SINT32 yo,SINT32 x1, SINT32 y1);
void appOsdLib_UsrLayerClearDraw(void);
void appOsdLib_OsdLayerClearDraw(void);
/*s Add by Aries 09/08/18*/
void  appOsdLib_PIPLayerClearDraw(void);
/*e Add by Aries 09/08/18*/
UINT32 appOsdLib_FontGetStringWidthbyId(UINT32 nStrId);
#endif /* _APP_OSDLIB_H_ */



