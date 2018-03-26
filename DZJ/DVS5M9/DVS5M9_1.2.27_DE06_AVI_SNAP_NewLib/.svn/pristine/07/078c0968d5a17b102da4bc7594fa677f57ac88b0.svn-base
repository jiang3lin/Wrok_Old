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

#ifndef _APP_VIEW_PARAM_H_
#define _APP_VIEW_PARAM_H_

/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/
#include "app_view_param_def.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/* general param */

#define PRM_SHARE	0x80	/* Share parameter with specific parameter mode */
#define PRM_SAVE	0x40	/* Load only parameter with PRM_SAVE flag */
#define PRM_NOUSE	0x20	/* The parameter itself is invalid in this mode */
#define PRM_NOSEL	0x10	/* The parameter can¡¦t be change in this mode */
#define PRM_NOSAVE	0x00	/* Load parameter from B file, when power on */

typedef struct {
	UINT8 flag;
	UINT8 defVal;
	UINT16 availMask;
} param_info_t;

typedef struct {
	UINT8 *fname;
	UINT8 *value; /* allocate */
	param_info_t *info;/* hardcoded */
} ui_param_t;

/* view param */
/*if you add variable to struct,you must modify  relative file in \sphost\resource\B\USER_RES*/
/*and   app_view_param.c*/
typedef struct {
	UINT8 size;
	UINT8 vidsize;
	UINT8 quality;
	UINT8 metering;
	UINT8 wb;

	UINT8 iso;
	UINT8 ev;
	UINT8 drive;
	UINT8 aeb;
	UINT8 highlight;

	UINT8 edge;
	UINT8 effect;
	UINT8 stamp;
	UINT8 stablizer;
	UINT8 facetrack;
	UINT8 photoframe;
	UINT8 dzoom;
	UINT8 af;
	UINT8 shutter;
	UINT8 aperture;
	UINT8 flash;
	UINT8 osd;
	UINT8 pvflickermode;
	UINT8 viddist;
	UINT8 WDR;
	UINT8 VideoSeamlessMode ;
	UINT8 panorama ;
	UINT8 feature;
	UINT8 videoFacetrack;
	UINT8 videoStamp;
	UINT8 videoQuality;
	UINT8 vidrsc;
} view_param_t;


#define VIEW_PRM_COUNT		    sizeof(view_param_t)
#define VIEW_PRM_ID(_param)		(UINT32)(&(((view_param_t *)0)->_param))
#define DEF_VIEW_PRM_INF(_param, _flag, _defval, _mask)		\
			[VIEW_PRM_ID(_param)]={(_flag), (_defval), (_mask)}

typedef enum {
	PRM_MODE_P,
	PRM_MODE_A,
	PRM_MODE_S,
	PRM_MODE_M,
	PRM_MODE_Q,
	PRM_MODE_SMILE,
	PRM_MODE_VIDEO,
	PRM_MODE_SCENE_PEOPLE,
	PRM_MODE_SCENE_NOT_PEOPLE,	
	PRM_MODE_MAX
} param_mode_t;

void UiParamViewGetInfo(UINT8 id, param_info_t *info);
void UiParamViewChangeMode(UINT8 paramMode);
void UiParamViewInit(void);
void UiParamViewFini(void);
void UiParamViewFlush(void);
void UiParamViewReset(void);
view_param_t* appViewParamGet(void);
extern view_param_t *pViewParam;

#endif  /* _APP_VIEW_PARAM_H_ */
