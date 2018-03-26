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
#ifndef _APP_GSENSOR_H_
#define _APP_GSENSOR_H_

#define GSENSOR_INTRRUP_PIN		SP5K_GPIO_INT8
#define GSENSOR_DATA_CNT			6

typedef struct _gSensorInit_t {
	UINT32 ulAddr;
	UINT32 ulSize;
} gSensorInit_t;

typedef struct _gSensorCfg_t {
	UINT32 ulselector;
	UINT32 ulvalue;
} gSensorCfg_t;

typedef struct _gSensorCapab_t {
	UINT32 ucMeasureRange;
	UINT32 ucSensitivity;
} gSensorCapab_t;

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
void appGsensorInit(UINT8 mode);
void appGSensorIsrSvrFunc(void);
static void appGSensorThread(UINT32 parm);
BOOL appGSensorFlipGet(void);
void appGSensorAccelerateRead(float *pAccelerateX, float *pAccelerateY, float *pAccelerateZ);
void appGSensorAccelerateGet(float *pAccelerateX, float *pAccelerateY, float *pAccelerateZ);
void appGsensorXYZRead(SINT16 * x_data, SINT16 * y_data, SINT16 * z_data);
void appGsensorSensortivitySet(void);
void appGsensorBasePointGet(void);
void appGsensorStatusGet(UINT8 sel,UINT32 *param);
UINT8 appGsensorIntrStatusGet(void);
UINT8 appGsensorInfoGet(UINT8 *buf, UINT32 len);
void appGsensorInfoWrite2Mov(void);


#endif  /* _APP_GSENSOR_H_ */



