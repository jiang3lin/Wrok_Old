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
#ifndef _GSENSOR_DEF_H_
#define _GSENSOR_DEF_H_


 /**************************************************************************
  * 						  C O N S T A N T S 						   *
  **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

typedef enum _gSensorSelector_e {
	GSENSOR_CHIP_ID,
	GSENSOR_CAPABILITY_IDX,
	GSENSOR_CAPABILITY_DATA,
	GSENSOR_VAL_X,
	GSENSOR_VAL_Y,
	GSENSOR_VAL_Z,
	GSENSOR_SHAKE_STATUS,
	GSENSOR_ANGULAR_RATE,
	GSENSOR_SENSITIVITY,
	GSENSOR_INTERRUP_SRC,	//clean interrup pin
} gSensorSelector_e;

typedef enum _gSensorShakeDirection_e{
	GSENSOR_SHAKE_NONE,
	GSENSOR_SHAKE_LEFT,
	GSENSOR_SHAKE_RIGHT,
	GSENSOR_SHAKE_UNDEF,
	
} gSensorShakeDirection_e;

typedef enum _gSensorInitMode_e{
	GSENSOR_NORMAL_INIT,
	GSENSOR_COLLISIONDET_INIT,
	GSENSOR_RESET_INIT,
}gSensorInitMode_e;

typedef enum _gSensorSensitivity_e{
	GSENSOR_SENSITIVITY_HIGHT,
	GSENSOR_SENSITIVITY_MIDDLE,
	GSENSOR_SENSITIVITY_LOW,
}gSensorSensitivity_e;

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
void   gSensorDriverInit(UINT8 mode);
void   gSensorDriverWrite(UINT32 addr,UINT32 data);
void   gSensorDriverRead(UINT32 selID,UINT32 * param);
void   gSensorDriverSet(UINT32 selID, UINT32 param);
UINT8 *gSensorDriverNameGet(void);

#endif  /* _APP_GSENSOR_DEF_H_ */


