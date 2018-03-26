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
#define HOST_DBG 1
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
#include "app_lens_focus.h"
#include "app_lens_iris.h"
#include "app_lens_mshut.h"

#if (IS_LENS_MOTOR_DRV_MD3_IN4)

/**************************************************************************
 *                          C O M M O N                                   *
 **************************************************************************/

static UINT32 glensMotorPwrCnt=0;

/**************************************************************************
 *                     Z O O M   S E C T I O N                            *
 **************************************************************************/
#ifdef LENS_MODEL_D160
	/*-- Zoom Control Pin: IN[A]=LMI[5]=TELE, IN[B]=LMI[6]=WIDE */
#define MOTOR_TELE_PIN_SET(a)	do { \
									sp5kGpioWrite(SP5K_GPIO_GRP_LMI, \
													(UINT32)(0x01UL<<5), \
													(UINT32)((0x01UL<<5)&((!!a) ? (~0UL) : 0UL))); \
								} while(0)
#define MOTOR_WIDE_PIN_SET(a)	do { \
									sp5kGpioWrite(SP5K_GPIO_GRP_LMI, \
													(UINT32)(0x01UL<<6), \
													(UINT32)((0x01UL<<6)&((!!a) ? (~0UL) : 0UL))); \
								} while(0)
static UINT8 zoomMotorSpeed;
#endif /*-- LENS_MODEL_D160 */

/**************************************************************************
 *                       F O C U S   S E C T I O N                        *
 **************************************************************************/
#ifdef LENS_MODEL_D160
	/*-- Focus 800pps */
#define FOCUS_MOTOR_ACTUATE_TIME		1250	/* unit: us */
#define FOCUS_MOTOR_INERTIA_TIME		4	/* unit : ms */
#define FOCUS_MOTOR_RELEASE_TIME		10	/* unit : ms */

#define FOCUS_MOTOR_CMD_IDX_MASK 	0x03 /*-- cmd num=4 */
#define FOCUS_MOTOR_CMD_MASK 		0x03 /*-- IN[2:1] */
#define FOCUS_MOTOR_CMD_GET(Idx)	((UINT8)fmotorCmd[Idx])
#define FOCUS_MOTOR_ACTUATE_DELAY(us)	do { tmrUsWait(us); } while(0)

	/*-- Focus Phase Table
	 * MD[3:1]=LMI[2:0]=[110]=0x06
	 * -CCW(NEAR)--------CW(FAR)-----
	 *      A+ [O2] H   H   L   L
	 *      A- [O3] L   L   H   H
	 *      B+ [O6] L   H   H   L
	 *      B- [O7] H   L   L   H
	 *-------------------------------
	 *         IN1  0   1   0   1
	 *         IN2  1   0   0   1
	 *-------------------------------
	 * cmd=IN[2:1]  2   1   0   3
	 */
static const UINT8 fmotorCmd[] = {2, 3, 0, 1};
			/*-- the seq is to let cmdIdx++ for NEAR */
static UINT8 focusCmd;
static UINT8 focusCmdIdx;

#endif /*-- LENS_MODEL_D160 */

/**************************************************************************
 *                       I R I S     S E C T I O N                        *
 **************************************************************************/

/**************************************************************************
 *                       S H U T T E R   S E C T I O N                    *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/


/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern void tmrUsWait(UINT32 delay);

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/*-------------------------------------------------------------------------
 *  File Name : lensMotorGpioConfig
 *------------------------------------------------------------------------*/
void lensMotorGpioConfig(void)
{
#ifdef LENS_MODEL_D160
	UINT32 mask, ioval;

printf("%s \n\n", __FUNCTION__);
	/*-- EN=LMI[10]=PowerEn, shall be set LO at first */
	sp5kGpioWrite(SP5K_GPIO_GRP_LMI, 0x01UL<<10, 0);
	sp5kGpioCfgSet(SP5K_GPIO_GRP_LMI, 0x01UL<<10, 0x01UL<<10);

	/*-- LMI[6:0] for MD[3:1] IN[4:1], LMI[11] for ZFHM_EN */
	mask = (0x7FUL) | (0x01UL<<11);
	ioval= 0; /*-- output ports, default 'LO' */
	sp5kGpioWrite(SP5K_GPIO_GRP_LMI, mask, ioval);
	sp5kGpioCfgSet(SP5K_GPIO_GRP_LMI, mask, mask);

	/*-- TGGPIO for [0]:ZCLK_EN, output port */
	mask = 0x01UL; ioval=0; /*-- default 'LO' */
	sp5kGpioWrite(SP5K_GPIO_GRP_TGL, mask, ioval);
	sp5kGpioCfgSet(SP5K_GPIO_GRP_TGL, mask, mask);

	/*-- TGGPIO for [2]:ZHM, [3]:FHM,  input port */
	mask = (0x01UL<<2) | (0x01UL<<3);
	ioval = 0; /* input port */
	sp5kGpioCfgSet(SP5K_GPIO_GRP_TGL, mask, ioval);

	/*-- GGPIO for [13]:ZCLK,  input port */
	mask = (0x01UL<<13);
	ioval = 0; /* input port */
	sp5kGpioCfgSet(SP5K_GPIO_GRP_GEN, mask, ioval);

#endif
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
#ifdef LENS_MODEL_D160
	glensMotorPwrCnt++;
	#if 1 /*-- PowerOn_Complex */
	/*-- Set Motor to UN-USED config before turn on motor power !!
	 *   MD[3:1]=LMI[2:0]=[000]
	 *   IN[2:1]=LMI[4:3]=[11]
	 *   IN[B:A]=LMI[6:5]=[00]
	 *   ==> LMI[6:0]=bX001_1000=0x18, with MASK=bX111_1111=0x7F
	 */
	sp5kGpioWrite(SP5K_GPIO_GRP_LMI,
					(UINT32)(0x7FUL),
					(UINT32)(0x18UL));

	/*-- Powre Control Truth Table
	 *   EN       MD1
	 *----------------------------
	 * LMI[10]  LMI[0]
	 *----------------------------
	 *    L       L 	PWR Off   (0x000)
	 *    L       H     STANDBY   (0x001)
	 *    H       *     PWR ON    (0x400)
	 */
	sp5kGpioWrite(SP5K_GPIO_GRP_LMI,
					(UINT32)(0x01UL<<10),
					(UINT32)(0x01UL<<10));
	#else /*-- PowerOn_Simple */
	/*-- Set Motor to UN-USED config before turn on motor power !!
	 *   MD[3:1]=LMI[2:0]=[001]
	 *   IN[2:1]=LMI[4:3]=[01]
	 *   IN[B:A]=LMI[6:5]=[00]
	 *   ==> LMI[6:0]=bX000_1001=0x09, with MASK=bX111_1111=0x7F
	 */
	sp5kGpioWrite(SP5K_GPIO_GRP_LMI,
					(UINT32)(0x7FUL),
					(UINT32)(0x09UL));

	/*-- Powre Control Truth Table
	 *   EN       MD1
	 *----------------------------
	 * LMI[10]  LMI[0]
	 *----------------------------
	 *    L       L 	PWR Off   (0x000)
	 *    L       H     STANDBY   (0x001)
	 *    H       *     PWR ON    (0x400)
	 */
	sp5kGpioWrite(SP5K_GPIO_GRP_LMI,
					(UINT32)(0x01UL<<10),
					(UINT32)(0x01UL<<10));
	#endif
#endif /*-- LENS_MODEL_D160 */
}
/*-------------------------------------------------------------------------
 *  File Name : lensMotorSafeActuateQuery
 *------------------------------------------------------------------------*/
UINT32 lensMotorSafeActuateQuery(UINT32 whoisit)
{
	UINT32 chkLevel, ret=SUCCESS;

	switch(whoisit) {
	case MOTOR_ACTUATE_QUERY_ZOOM:
		chkLevel = 0;
		/* INDEPENDENT, Zoom is independent with other lens for md3in4 motor */
		break;
	case MOTOR_ACTUATE_QUERY_FOCUS:
		chkLevel = 1;
		/* ASSOCIATED check, if iris, shutter running, return FAIL */
		break;
	case MOTOR_ACTUATE_QUERY_IRIS:
	case MOTOR_ACTUATE_QUERY_MSHUT:
		chkLevel = 2;
		/* SEVERE check, if any lens is actuated, return FAIL */
		break;
	default:
		return FAIL;
	}

	if (chkLevel==2) {
		/* SEVERE check, if any lens is actuated, return FAIL */
		if ((ZOOM_MXN_STATE_STOP!=lensZoomQuery(ZOOM_QUERY_MOTION_STATE, 0)) ||
			lensFocusQuery(FOCUS_QUERY_ID_MOTION_STATE) ||
			lensIrisBusyQuery() ||
			lensMShutBusyQuery() )
		{
			ret = FAIL;
		}

	} else if (chkLevel==1) {
		/* ASSOCIATED check, if iris, shutter running, return FAIL */
		if (lensIrisBusyQuery() ||
			lensMShutBusyQuery() )
		{
			ret = FAIL;
		}
	}
	#if HOST_DBG
	if (ret==FAIL) {
		printf(">>> Lens Motor Zoom(%d), Focus(%d), Iris(%d), MShut(%d) \n",
				(ZOOM_MXN_STATE_STOP!=lensZoomQuery(ZOOM_QUERY_MOTION_STATE, 0)),
				lensFocusQuery(FOCUS_QUERY_ID_MOTION_STATE),
				lensIrisBusyQuery(),
				lensMShutBusyQuery());
	}
	#endif
	return ret;
}
/*-------------------------------------------------------------------------
 *  File Name : lensMotorStandby
 *------------------------------------------------------------------------*/
void lensMotorStandby(void)
{
#ifdef LENS_MODEL_D160
	LENS_DBG("%s \n\n", __FUNCTION__);
	#if 1 /*-- Standby_Simple */
	/*-- Powre Control Truth Table
	 *   EN       MD1
	 *----------------------------
	 * LMI[10]  LMI[0]
	 *----------------------------
	 *    L       L 	PWR Off   (0x000)
	 *    L       H     STANDBY   (0x001)
	 *    H       *     PWR ON    (0x400)
	 */
	sp5kGpioWrite(SP5K_GPIO_GRP_LMI,
					(UINT32)((0x01UL<<10)|(0x01UL<<0)),
					(UINT32)(0x01UL<<0));
	#else /*-- Standby_Complex */
	/*-- Powre Control Truth Table
	 *   EN       MD1
	 *----------------------------
	 * LMI[10]  LMI[0]
	 *----------------------------
	 *    L       L 	PWR Off   (0x000)
	 *    L       H     STANDBY   (0x001)
	 *    H       *     PWR ON    (0x400)
	 */

	/*--
	 * IN[B:A]IN[2:1]MD[3:1]=LMI[6:0]=[000_1-001]=0x09, mask=0x7F
	 * EN=LMI[10]=[0]
	 */
	sp5kGpioWrite(SP5K_GPIO_GRP_LMI,
					(UINT32)((0x01UL<<10)|(0x7FUL<<0)),
					(UINT32)(0x09UL<<0));
	#endif
#endif /*-- LENS_MODEL_D160 */
}
/*-------------------------------------------------------------------------
 *  File Name : lensMotorPowerOff
 *------------------------------------------------------------------------*/
static void _lensMotorPowerOff(void)
{
#ifdef LENS_MODEL_D160
	/*-- Powre Control Truth Table
	 *   EN       MD1
	 *----------------------------
	 * LMI[10]  LMI[0]
	 *----------------------------
	 *    L       L 	PWR Off   (0x000)
	 *    L       H     STANDBY   (0x001)
	 *    H       *     PWR ON    (0x400)
	 */
	sp5kGpioWrite(SP5K_GPIO_GRP_LMI,
					(UINT32)((0x01UL<<10)|(0x01UL<<0)),
					(UINT32)0x0UL);
#endif /*-- LENS_MODEL_D160 */
}

void lensMotorPowerOff(void)
{
	if (glensMotorPwrCnt<=1) {
		_lensMotorPowerOff();
		glensMotorPwrCnt = 0;
		//LENS_ISR_DBG("Motor_OFF !\n");
	} else {
		glensMotorPwrCnt--;
	}

	#if 0
	printf("\n\n####### LensPwr OFF (%d) \n\n", glensMotorPwrCnt);
	#endif
}



/*-------------------------------------------------------------------------
 *  File Name : zoomMotorSpeedGet
 *------------------------------------------------------------------------*/
UINT8 zoomMotorSpeedGet(void)
{
#ifdef LENS_MODEL_D160
	return zoomMotorSpeed;
#endif /*-- LENS_MODEL_D160 */
}
/*-------------------------------------------------------------------------
 *  File Name : zoomMotorSpeedSet
 *------------------------------------------------------------------------*/
void zoomMotorSpeedSet(UINT8 zoom_speed)
{
#ifdef LENS_MODEL_D160
	zoomMotorSpeed=zoom_speed;
#endif /*-- LENS_MODEL_D160 */
}
/*-------------------------------------------------------------------------
 *  File Name : zoomMotorTele
 *------------------------------------------------------------------------*/
void zoomMotorTele(void)
{
#ifdef LENS_MODEL_D160
	LENS_VERBOSE("Z_TELE \n");

	MOTOR_TELE_PIN_SET(1);
	MOTOR_WIDE_PIN_SET(0);
#endif /*-- LENS_MODEL_D160 */
}
/*-------------------------------------------------------------------------
 *  File Name : zoomMotorWide
 *------------------------------------------------------------------------*/
void zoomMotorWide(void)
{
#ifdef LENS_MODEL_D160
	LENS_VERBOSE("Z_WIDE \n");

	MOTOR_TELE_PIN_SET(0);
	MOTOR_WIDE_PIN_SET(1);
#endif /*-- LENS_MODEL_D160 */
}
/*-------------------------------------------------------------------------
 *  File Name : zoomMotorBrake
 *------------------------------------------------------------------------*/
void zoomMotorBrake(void)
{
#ifdef LENS_MODEL_D160
	LENS_VERBOSE("Z_BRAKE \n");

	MOTOR_TELE_PIN_SET(1);
	MOTOR_WIDE_PIN_SET(1);
#endif /*-- LENS_MODEL_D160 */
}
/*-------------------------------------------------------------------------
 *  File Name : zoomMotorStandby
 *------------------------------------------------------------------------*/
void zoomMotorStandby(void)
{
#ifdef LENS_MODEL_D160
	LENS_VERBOSE("Z_STDBY \n");

	MOTOR_TELE_PIN_SET(0);
	MOTOR_WIDE_PIN_SET(0);
#endif /*-- LENS_MODEL_D160 */
}


/*-------------------------------------------------------------------------
 *  File Name : focusMotorSet
 *------------------------------------------------------------------------*/
static void focusMotorSet(UINT8 cmd)
{
#ifdef LENS_MODEL_D160
	/*-- MD[3:1]=LMI[2:0]=[110]=0x06 */
	sp5kGpioWrite(SP5K_GPIO_GRP_LMI, (UINT32)0x07, (UINT32)0x06);

	/*-- IN[2:1]=LMI[4:3]=cmd<<3 */
	#if 1
	sp5kGpioWrite(SP5K_GPIO_GRP_LMI,
					(UINT32)(FOCUS_MOTOR_CMD_MASK<<3),
					(UINT32)((cmd&FOCUS_MOTOR_CMD_MASK)<<3));
	#else
	mask=(FOCUS_MOTOR_CMD_MASK<<3);
	val = ((cmd&FOCUS_MOTOR_CMD_MASK)<<3);
	printf(">>%s: cmd(%d), mask(%x), val(%x) \n", __FUNCTION__, cmd, mask, val);
	UINT32 mask, val;
	sp5kGpioWrite(SP5K_GPIO_GRP_LMI,
					mask,
					val);
	#endif
#endif /*-- LENS_MODEL_D160 */
}
/*-------------------------------------------------------------------------
 *  File Name : focusMotorCmdGet
 *------------------------------------------------------------------------*/
UINT8 focusMotorCmdGet(void)
{
#ifdef LENS_MODEL_D160
	return FOCUS_MOTOR_CMD_GET(focusCmdIdx);
#endif /*-- LENS_MODEL_D160 */
}
/*-------------------------------------------------------------------------
 *  File Name : focusMotorCmdIdxGet
 *------------------------------------------------------------------------*/
UINT8 focusMotorCmdIdxGet(void)
{
#ifdef LENS_MODEL_D160
	return focusCmdIdx;
#endif /*-- LENS_MODEL_D160 */
}
/*-------------------------------------------------------------------------
 *  File Name : focusMotorNearNext
 *------------------------------------------------------------------------*/
void focusMotorNearNext(void)
{
#ifdef LENS_MODEL_D160
	/*-- Retrieve motor cmd of next step */
	focusCmdIdx++;
	focusCmdIdx &= FOCUS_MOTOR_CMD_IDX_MASK;
	focusCmd = FOCUS_MOTOR_CMD_GET(focusCmdIdx);

	//printf("%s: idx(%d), cmd(%d) \n", __FUNCTION__, focusCmdIdx, fmotorCmd[focusCmdIdx]);

	/*-- Trigger focus motor */
	focusMotorSet( focusCmd);

	/*-- Wait motor actuate lag */
	FOCUS_MOTOR_ACTUATE_DELAY(FOCUS_MOTOR_ACTUATE_TIME);
	//appTimeDelayMs(5);
#endif /*-- LENS_MODEL_D160 */
}
/*-------------------------------------------------------------------------
 *  File Name : focusMotorFarNext
 *------------------------------------------------------------------------*/
void focusMotorFarNext(void)
{
#ifdef LENS_MODEL_D160
	/*-- Retrieve motor cmd of next step */
	focusCmdIdx--;
	focusCmdIdx &= FOCUS_MOTOR_CMD_IDX_MASK;
	focusCmd = FOCUS_MOTOR_CMD_GET(focusCmdIdx);

	//printf("%s: idx(%d), cmd(%d) \n", __FUNCTION__, focusCmdIdx, fmotorCmd[focusCmdIdx]);

	/*-- Trigger focus motor */
	focusMotorSet(focusCmd);

	/*-- Wait motor actuate lag */
	FOCUS_MOTOR_ACTUATE_DELAY(FOCUS_MOTOR_ACTUATE_TIME);
	//appTimeDelayMs(5);
#endif /*-- LENS_MODEL_D160 */
}
/*-------------------------------------------------------------------------
 *  File Name : focusInertiaExcite
 *------------------------------------------------------------------------*/
void focusInertiaExcite(void)
{
#ifdef LENS_MODEL_D160
printf("%s: idx(%d), cmd(%d) \n", __FUNCTION__, focusCmdIdx, fmotorCmd[focusCmdIdx]);
	/*-- Excite focus motor @ current state */
	focusMotorSet(FOCUS_MOTOR_CMD_GET(focusCmdIdx));

	/*-- Wait motor inertial excitation lag */
	FOCUS_MOTOR_ACTUATE_DELAY(FOCUS_MOTOR_INERTIA_TIME*1000);
	//appTimeDelayMs(10);
#endif /*-- LENS_MODEL_D160 */
}
/*-------------------------------------------------------------------------
 *  File Name : focusReleaseHold
 *------------------------------------------------------------------------*/
void focusReleaseHold(void)
{
#ifdef LENS_MODEL_D160
printf("%s: idx(%d), cmd(%d) \n", __FUNCTION__, focusCmdIdx, fmotorCmd[focusCmdIdx]);
	/*-- Excite focus motor @ current state */
	focusMotorSet(FOCUS_MOTOR_CMD_GET(focusCmdIdx));

	/*-- Wait motor inertial excitation lag */
	FOCUS_MOTOR_ACTUATE_DELAY(FOCUS_MOTOR_RELEASE_TIME*1000);
	//appTimeDelayMs(20);
#endif /*-- LENS_MODEL_D160 */
}
/*-------------------------------------------------------------------------
 *  File Name : focusMotorSet
 *------------------------------------------------------------------------*/
void focusMotorReset(void)
{
#ifdef LENS_MODEL_D160
	focusCmdIdx = 0;
	focusCmd= fmotorCmd[focusCmdIdx];
#endif /*-- LENS_MODEL_D160 */
}

#if 0 // LENS_MODEL_D160
/*-------------------------------------------------------------------------
 *  File Name : irisOpenPinSet
 *------------------------------------------------------------------------*/
static void irisOpenPinSet(UINT8 v)
{
}
/*-------------------------------------------------------------------------
 *  File Name : irisClosePinSet
 *------------------------------------------------------------------------*/
static void irisClosePinSet(UINT8 v)
{
}
#endif
/*-------------------------------------------------------------------------
 *  File Name : irisIdleActuate
 *------------------------------------------------------------------------*/
void irisIdleActuate(void)
{
#ifdef LENS_MODEL_D160
	lensMotorStandby();
#endif /*-- LENS_MODEL_D160 */
}
/*-------------------------------------------------------------------------
 *  File Name : irisLargeActuate
 *------------------------------------------------------------------------*/
void irisLargeActuate(void)
{
#ifdef LENS_MODEL_D160
	/*-- MD[3:1]=LMI[2:0]=[101]=0x05 */
	/*-- IN[B:A:2:1]=LMI[6:5:4:3]==>>[O6,O5]
	 *   IN[B:A:2:1]MD[3:1]=LMI[6:0]=[000_0-101] -->>Small
	 *   IN[B:A:2:1]MD[3:1]=LMI[6:0]=[001_0-101] -->>Large
	 */
	sp5kGpioWrite(SP5K_GPIO_GRP_LMI, (UINT32)0x7FUL, (UINT32)0x15UL);
#endif /*-- LENS_MODEL_D160 */
}
/*-------------------------------------------------------------------------
 *  File Name : irisSmallActuate
 *------------------------------------------------------------------------*/
void irisSmallActuate(void)
{
#ifdef LENS_MODEL_D160
	/*-- MD[3:1]=LMI[2:0]=[101]=0x05 */
	/*-- IN[B:A:2:1]=LMI[6:5:4:3]==>>[O6,O5]
	 *   IN[B:A:2:1]MD[3:1]=LMI[6:0]=[000_0-101] -->>Small
	 *   IN[B:A:2:1]MD[3:1]=LMI[6:0]=[001_0-101] -->>Large
	 */
	sp5kGpioWrite(SP5K_GPIO_GRP_LMI, (UINT32)0x7FUL, (UINT32)0x05UL);
#endif /*-- LENS_MODEL_D160 */
}


#if 0
/*-------------------------------------------------------------------------
 *  File Name : mshutOpenPinSet
 *------------------------------------------------------------------------*/
static void mshutOpenPinSet(	UINT8 isHi)
{
}
/*-------------------------------------------------------------------------
 *  File Name : mshutClosePinSet
 *------------------------------------------------------------------------*/
static void mshutClosePinSet(UINT8 isHi)
{
}
#endif /*-- LENS_MODEL_D160 */
/*-------------------------------------------------------------------------
 *  File Name : mshutIdleActuate
 *------------------------------------------------------------------------*/
void mshutIdleActuate(	void)
{
#ifdef LENS_MODEL_D160
	lensMotorStandby();
#endif /*-- LENS_MODEL_D160 */
}

/*-------------------------------------------------------------------------
 *  File Name : mshutOpenActuate
 *------------------------------------------------------------------------*/
void mshutOpenActuate(void)
{
#ifdef LENS_MODEL_D160
	/*-- MD[3:1]=LMI[2:0]=[000]=0x0UL */
	/*-- IN[2:1]=LMI[4:3]==>>[O2,O1]
	 *   IN[2:1]MD[3:1]=LMI[4:0]=[0-0000]=0x00 -->>Close,[O2,O1]=[10]
	 *   IN[2:1]MD[3:1]=LMI[4:0]=[1-0000]=0x10 -->>Open, [O2,O1]=[01]
	 *   wirte mask = 0x1F
	 */
	sp5kGpioWrite(SP5K_GPIO_GRP_LMI, (UINT32)0x1FUL, (UINT32)0x10UL);
#endif /*-- LENS_MODEL_D160 */
}

/*-------------------------------------------------------------------------
 *  File Name : mshutCloseActuate
 *------------------------------------------------------------------------*/
void mshutCloseActuate(void)
{
#ifdef LENS_MODEL_D160
	/*-- MD[3:1]=LMI[2:0]=[000]=0x0UL */
	/*-- IN[2:1]=LMI[4:3]==>>[O2,O1]
	 *   IN[2:1]MD[3:1]=LMI[4:0]=[0-0000]=0x00 -->>Close,[O2,O1]=[10]
	 *   IN[2:1]MD[3:1]=LMI[4:0]=[1-0000]=0x10 -->>Open, [O2,O1]=[01]
	 *   wirte mask = 0x1F
	 */
	sp5kGpioWrite(SP5K_GPIO_GRP_LMI, (UINT32)0x1FUL, (UINT32)0x00UL);
#endif /*-- LENS_MODEL_D160 */
}

#endif /* IS_LENS_MOTOR_DRV_DUMMY */

