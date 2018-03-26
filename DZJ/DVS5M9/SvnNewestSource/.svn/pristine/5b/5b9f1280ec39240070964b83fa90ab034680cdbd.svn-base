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
#ifndef APP_LENS_MOTOR_H
#define APP_LENS_MOTOR_H

/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/*--
 * LENS Motor Driver Configuration Session
 */
#define LENS_MOTOR_DRV_DIRECT	0
#define LENS_MOTOR_DRV_MD3_IN4	1
#define LENS_MOTOR_DRV_SH_IRIS	2
#define LENS_MOTOR_DRV_DUMMY	0xFF

#define LENS_MOTOR_DRIVING LENS_MOTOR_DRV_DUMMY

#define IS_LENS_MOTOR_DRV_DIRECT 	(LENS_MOTOR_DRIVING==LENS_MOTOR_DRV_DIRECT)
#define IS_LENS_MOTOR_DRV_MD3_IN4	(LENS_MOTOR_DRIVING==LENS_MOTOR_DRV_MD3_IN4)
#define IS_LENS_MOTOR_DRV_SH_IRIS	(LENS_MOTOR_DRIVING==LENS_MOTOR_DRV_SH_IRIS)
#define IS_LENS_MOTOR_DRV_DUMMY		(LENS_MOTOR_DRIVING==LENS_MOTOR_DRV_DUMMY)

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef enum {
	MOTOR_ACTUATE_QUERY_ZOOM,
	MOTOR_ACTUATE_QUERY_FOCUS,
	MOTOR_ACTUATE_QUERY_IRIS,
	MOTOR_ACTUATE_QUERY_MSHUT,
	MOTOR_ACTUATE_QUERY_TOTAL
} motorActuateQuery_t;
/**************************************************************************
 *                               M A C R O S                              *
 **************************************************************************/

/**************************************************************************
 *          M O D U L E   V A R I A B L E S   R E F E R E N C E S         *
 **************************************************************************/

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/
void lensMotorGpioConfig(void);
void lensMotorPowerOn(void);
void lensMotorPowerOff(void);
UINT32 lensMotorSafeActuateQuery(UINT32 whoisit);
UINT32 lensMotorPowerCount(void);
UINT8 zoomMotorSpeedGet(void);
void zoomMotorSpeedSet(UINT8 zoom_speed);
void zoomMotorTele(void);
void zoomMotorWide(void);
void zoomMotorBrake(void);
void zoomMotorStandby(void);


UINT8 focusMotorCmdGet(void);
UINT8 focusMotorCmdIdxGet(void);
void focusMotorNearNext(void);
void focusMotorFarNext(void);
void focusInertiaExcite(void);
void focusReleaseHold(void);
void focusMotorReset(void);


void irisIdleActuate(void);
void irisLargeActuate(void);
void irisSmallActuate(void);


void mshutIdleActuate(	void);
void mshutOpenActuate(void);
void mshutCloseActuate(void);

#endif  /* APP_LENS_MOTOR_H */


