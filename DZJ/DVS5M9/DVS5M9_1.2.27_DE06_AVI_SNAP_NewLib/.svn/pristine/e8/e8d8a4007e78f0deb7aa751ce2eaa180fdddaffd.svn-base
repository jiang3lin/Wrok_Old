#ifndef APP_3AALG_H
#define APP_3AALG_H

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
#define EVENT_DEFAULT_LV	0x0000
#define EVENT_DAY_LV		0x0001
#define EVENT_DUSK_LV		0x0002
#define EVENT_NIGHT_LV		0x0004
#define EVENT_SWITCH_LV		(EVENT_DAY_LV | EVENT_DUSK_LV | EVENT_NIGHT_LV) 
	
#define EVENT_IR_OFF		0x0000
#define EVENT_IR_ON			0x0001
#define EVENT_IR_ERROR		0x0002

#define SENSOR_PRI          0x01
#define SENSOR_SEC          0x10
#define SENSOR_BOTH         0x11
#define SENSOR_COMA         0x00
#define SENSOR_COMB         0x01
#define SENSORSEL_CHECK(val, chk) ( ((val&chk)!=chk) ? 0 : 1 )

typedef enum _AWBALG_PARA_SEL{
	WB_PARA_CONVERGE_SPEED	= (unsigned char)0x00,
	WB_PARA_DARK_LUMA		= (unsigned char)0x01,
	WB_PARA_MODE_SET		= (unsigned char)0x02,
} AWBALG_PARA_SEL;

typedef enum _AEALG_PARAM_SEL{
	AE_PARA_EVSET	        = (unsigned char)0x00,
	AE_PARA_METERINGSET		= (unsigned char)0x01,	
    AE_PARA_TARGETSET		= (unsigned char)0x02,
    AE_PARA_TARGETGET		= (AE_PARA_TARGETSET),
    AE_PARA_WINDOWSET		= (unsigned char)0x03,

    AE_PARA_AEDEBUG         = (unsigned char)0x10,
    AE_PARA_PVDEBUG         = (unsigned char)0x11,
    AE_PARA_CALISO100       = (unsigned char)0x12,
    AE_PARA_ISOOFFSET       = (unsigned char)0x13,
} AEALG_PARAM_SEL;

typedef enum _AEALG_PARA_METERING{
	AE_METERING_CENTER	    = (unsigned char)0x00,
	AE_METERING_MULTI   	= (unsigned char)0x01,
	AE_METERING_SPOT       	= (unsigned char)0x02,
} AEALG_PARA_METERING;

typedef struct aeInfo_node{
	UINT32 frame_id;
	UINT8  aewin_hgrid;
	UINT8  aewin_vgrid;
	UINT16 *paewin_y;
    UINT32 *pyhis;
    SINT32 lvidx;
    struct aeInfo_dnode *next;
}__attribute__((packed))aeInfo_list; //ae info backup linklist 
#endif
