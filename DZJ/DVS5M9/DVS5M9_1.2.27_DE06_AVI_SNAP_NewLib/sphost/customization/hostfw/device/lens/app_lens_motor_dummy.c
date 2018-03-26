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
#define HOST_DBG 0
#include "common.h"
#include <stdio.h>
#include <string.h>
#include "sp5k_global_api.h"
#include "gpio_custom.h"
#include "app_com_api.h"
#include "app_dbg_api.h"
//#include "app_util.h"
#include "app_lens_private.h"
#include "app_lens_motor.h"
#include "app_lens_zoom.h"

#if (IS_LENS_MOTOR_DRV_DUMMY)

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/*--
 * ZOOM Motor Section
 */
#define ZOOM_PWM_NORMAL				(25)//--->original (20)
#define ZOOM_PWM_FAST				(30)
#define ZOOM_PWM_SLOW				(15)

/*--
 * Focus Motor Section
 */
#define FOCUS_MOTOR_ACTUATE_TIME		500	/* unit: us */
#define FOCUS_MOTOR_INERTIA_TIME		15	/* unit : ms */
#define FOCUS_MOTOR_RELEASE_TIME		15	/* unit : ms */

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
//typedef UINT8 	TYPE_FOCUS_MOTOR_CMD;

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
static UINT32 glensMotorPwrCnt=0;

//static UINT8 zoomMotorSpeed;      //Remove warning Lation@20121120
//static UINT8 focusCmdIdx;            //Remove warning Lation@20121120
//static UINT8 focusCmd;                //Remove warning Lation@20121120

static const UINT8 fmotorCmd[] = {0x09,0x0B,0x0A,0x0E,0x06,0x07,0x05,0x0D};

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/*--
 * ZOOM Motor Section
 */
#define MOTOR_TELE_PIN_SET(a)	do { 	\
									if (a)	sp5kPwmTrig(1 << CFG_ZOOM_PWM_CCW, 0); \
									else 	sp5kPwmTrig(0, 1 << CFG_ZOOM_PWM_CCW); \
								} while(0)
#define MOTOR_WIDE_PIN_SET(a)	do { 	\
									if (a)	sp5kPwmTrig(1 << CFG_ZOOM_PWM_CW, 0); \
									else 	sp5kPwmTrig(0, 1 << CFG_ZOOM_PWM_CW); \
								} while(0)
#define MOTOR_ZOOM_PWM_SET(a)	do {	\
									sp5kPwmCfgSet(CFG_ZOOM_PWM_CTRL, 1, a);	\
									sp5kPwmTrig(1 << CFG_ZOOM_PWM_CTRL, 0);	\
								} while(0)


/*--
 * Focus Motor Section
 */
#define FOCUS_AP_PIN(a)	do { sp5kGpioWrite(SP5K_GPIO_GRP_LMI, (((UINT32)1)<<3), ((!!(a)) ?  ~((UINT32)0) : 0)); } while(0)
#define FOCUS_AN_PIN(a)	do { sp5kGpioWrite(SP5K_GPIO_GRP_LMI, (((UINT32)1)<<2), ((!!(a)) ?  ~((UINT32)0) : 0)); } while(0)
#define FOCUS_BP_PIN(a)	do { sp5kGpioWrite(SP5K_GPIO_GRP_LMI, (((UINT32)1)<<1), ((!!(a)) ?  ~((UINT32)0) : 0)); } while(0)
#define FOCUS_BN_PIN(a)	do { sp5kGpioWrite(SP5K_GPIO_GRP_LMI, (((UINT32)1)<<0), ((!!(a)) ?  ~((UINT32)0) : 0)); } while(0)

#define FOCUS_MOTOR_CMD_IDX_MASK 	0x07
#define FOCUS_MOTOR_CMD_GET(Idx)	((UINT8)fmotorCmd[Idx])
#define FOCUS_MOTOR_ACTUATE_DELAY(us)	do { tmrUsWait(us); } while(0)

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/*-------------------------------------------------------------------------
 *  File Name : lensMotorGpioConfig
 *------------------------------------------------------------------------*/
void lensMotorGpioConfig(void)
{
}
/*-------------------------------------------------------------------------
 *  File Name : lensMotorStandby
 *------------------------------------------------------------------------*/
void lensMotorStandby(void)
{
}
/*-------------------------------------------------------------------------
 *  File Name : lensMotorSafeActuateQuery
 *------------------------------------------------------------------------*/
UINT32 lensMotorSafeActuateQuery(UINT32 whoisit)
{
	return SUCCESS;
}
/*-------------------------------------------------------------------------
 *  File Name : lensMotorPowerCount
 *------------------------------------------------------------------------*/
UINT32 lensMotorPowerCount(void)
{
	return glensMotorPwrCnt;
}
/*-------------------------------------------------------------------------
 *  File Name : lensMotorPowerOn
 *------------------------------------------------------------------------*/
void lensMotorPowerOn(void)
{
}
/*-------------------------------------------------------------------------
 *  File Name : lensMotorPowerOff
 *------------------------------------------------------------------------*/
void lensMotorPowerOff(void)
{
}
/*-------------------------------------------------------------------------
 *  File Name : zoomMotorSpeedGet
 *------------------------------------------------------------------------*/
UINT8 zoomMotorSpeedGet(void)
{
	return 0;
}
/*-------------------------------------------------------------------------
 *  File Name : zoomMotorSpeedSet
 *------------------------------------------------------------------------*/
void zoomMotorSpeedSet(UINT8 zoom_speed)
{
}
/*-------------------------------------------------------------------------
 *  File Name : zoomMotorTele
 *------------------------------------------------------------------------*/
void zoomMotorTele(void)
{
}
/*-------------------------------------------------------------------------
 *  File Name : zoomMotorWide
 *------------------------------------------------------------------------*/
void zoomMotorWide(void)
{
}
/*-------------------------------------------------------------------------
 *  File Name : zoomMotorBrake
 *------------------------------------------------------------------------*/
void zoomMotorBrake(void)
{
}
/*-------------------------------------------------------------------------
 *  File Name : zoomMotorStandby
 *------------------------------------------------------------------------*/
void zoomMotorStandby(void)
{
}

//Remove warning Lation@20121120
/*-------------------------------------------------------------------------
 *  File Name : focusMotorSet
 *------------------------------------------------------------------------*/
 /*
static void focusMotorSet(UINT8 cmd)
{
}
*/
/*-------------------------------------------------------------------------
 *  File Name : focusMotorCmdGet
 *------------------------------------------------------------------------*/
UINT8 focusMotorCmdGet(void)
{
	return 0;
}
/*-------------------------------------------------------------------------
 *  File Name : focusMotorCmdIdxGet
 *------------------------------------------------------------------------*/
UINT8 focusMotorCmdIdxGet(void)
{
	return 0;
}
/*-------------------------------------------------------------------------
 *  File Name : focusMotorNearNext
 *------------------------------------------------------------------------*/
void focusMotorNearNext(void)
{
}
/*-------------------------------------------------------------------------
 *  File Name : focusMotorFarNext
 *------------------------------------------------------------------------*/
void focusMotorFarNext(void)
{
}
/*-------------------------------------------------------------------------
 *  File Name : focusInertiaExcite
 *------------------------------------------------------------------------*/
void focusInertiaExcite(void)
{
}
/*-------------------------------------------------------------------------
 *  File Name : focusReleaseHold
 *------------------------------------------------------------------------*/
void focusReleaseHold(void)
{
}
/*-------------------------------------------------------------------------
 *  File Name : focusMotorSet
 *------------------------------------------------------------------------*/
void focusMotorReset(void)
{
}

//Remove warning Lation@20121120
/*-------------------------------------------------------------------------
 *  File Name : irisOpenPinSet
 *------------------------------------------------------------------------*/
 /*
static void irisOpenPinSet(UINT8 isHi)
{
}
*/

//Remove warning Lation@20121120
/*-------------------------------------------------------------------------
 *  File Name : irisClosePinSet
 *------------------------------------------------------------------------*/
 /*
static void irisClosePinSet(UINT8 isHi)
{
}
*/

/*-------------------------------------------------------------------------
 *  File Name : irisIdleActuate
 *------------------------------------------------------------------------*/
void irisIdleActuate(void)
{
}
/*-------------------------------------------------------------------------
 *  File Name : irisLargeActuate
 *------------------------------------------------------------------------*/
void irisLargeActuate(void)
{
}
/*-------------------------------------------------------------------------
 *  File Name : irisSmallActuate
 *------------------------------------------------------------------------*/
void irisSmallActuate(void)
{
}


//Remove warning Lation@20121120
/*-------------------------------------------------------------------------
 *  File Name : mshutOpenPinSet
 *------------------------------------------------------------------------*/
 /*
static void mshutOpenPinSet(	UINT8 isHi)
{
}
*/

//Remove warning Lation@20121120
/*-------------------------------------------------------------------------
 *  File Name : mshutClosePinSet
 *------------------------------------------------------------------------*/
 /*
static void mshutClosePinSet(UINT8 isHi)
{
}
*/

/*-------------------------------------------------------------------------
 *  File Name : mshutIdleActuate
 *------------------------------------------------------------------------*/
void mshutIdleActuate(	void)
{

}

/*-------------------------------------------------------------------------
 *  File Name : mshutOpenActuate
 *------------------------------------------------------------------------*/
void mshutOpenActuate(void)
{

}

/*-------------------------------------------------------------------------
 *  File Name : mshutCloseActuate
 *------------------------------------------------------------------------*/
void mshutCloseActuate(void)
{

}

#endif /* IS_LENS_MOTOR_DRV_DUMMY */

