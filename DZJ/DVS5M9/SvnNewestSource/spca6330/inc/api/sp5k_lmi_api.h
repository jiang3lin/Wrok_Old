/**************************************************************************
 *
 *      Copyright (c) 2005-2008 by Sunplus mMedia Inc., Ltd.
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
 *  Hsin-Chu, Taiwan.
 *
 **************************************************************************/

#ifndef _SP5K_LMI_API_H_
#define _SP5K_LMI_API_H_

#include "api/sp5k_global_api.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

#define LMI_DIR_FORWARD			0
#define LMI_DIR_BACKWARD		1

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef UINT32 (*fpsp5kLmiIsr_t)(UINT16 stopPhase, UINT16 stepNum);

/* SIN-PWM */
typedef enum {
	SP5K_SINPWM_DEV_0, 
	SP5K_SINPWM_DEV_0_A=SP5K_SINPWM_DEV_0, 
	SP5K_SINPWM_DEV_0_B,

	SP5K_SINPWM_DEV_1=0x10,
	SP5K_SINPWM_DEV_1_A=SP5K_SINPWM_DEV_1, 
	SP5K_SINPWM_DEV_1_B,

	SP5K_SINPWM_DEV_2=0x20,
	SP5K_SINPWM_DEV_2_A=SP5K_SINPWM_DEV_2, 
	SP5K_SINPWM_DEV_2_B,

	SP5K_SINPWM_DEV_3=0x30,
	SP5K_SINPWM_DEV_3_A=SP5K_SINPWM_DEV_3, 
	SP5K_SINPWM_DEV_3_B
	
} sp5kSinPwmDev_t;
 
typedef enum {
	SP5K_SINPWM_PARAM_TABLE, 		/* UINT16 *table, tableLen */
	SP5K_SINPWM_PARAM_PIN, 			/* pin, polarity */
	SP5K_SINPWM_PARAM_SUB_START, 	/* startStepX, startStepY */
	SP5K_SINPWM_PARAM_SUB_STOP, 	/* stopLoopX, stopStepX */
	SP5K_SINPWM_PARAM_SUB_MAXSTEP,	/* max step, backwardEnable  */
	SP5K_SINPWM_PARAM_SUB_STEPHIT,	/* STEPHIT step  */
	SP5K_SINPWM_PARAM_SUB_TIMING, 	/* pulseLength, repeat */
} sp5kSinPwmParam_t;

#define SP5K_SINPWM_MODE_INFINTE		0
#define SP5K_SINPWM_MODE_ROLLING_MASK	0x80000000

#define SP5K_SINPWM_CTRL_DIRECT_FORWARD		0x80000000	/* run specified step forwards directly */
#define SP5K_SINPWM_CTRL_DIRECT_BACKWARD	0x90000000	/* run specified step backwards directly */

typedef enum {
	SP5K_SINPWM_CTRL_START,
	SP5K_SINPWM_CTRL_PAUSE,
	SP5K_SINPWM_CTRL_STOP,
	SP5K_SINPWM_CTRL_FW_START,
	SP5K_SINPWM_CTRL_BW_START,
	SP5K_SINPWM_CTRL_MAX
} sp5kSinPwmCtrl_t;
 
typedef enum {
	SP5K_SINPWM_INFO_BUSY,			/* bit0=devA busy, bit1=devB busy */
	SP5K_SINPWM_INFO_STEP,			/* *data = (stepY<<20)|(stepX<<10)|loopX */
	SP5K_SINPWM_INFO_MAX
} sp5kSinPwmInfo_t;

typedef enum {
	SP5K_SINPWM_ISR_SUB_DONE,
	SP5K_SINPWM_ISR_STOP,
	SP5K_SINPWM_ISR_STEPHIT,
	SP5K_SINPWM_ISR_MAX
} sp5kSinPwmIsr_t;


/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

/**************************************************************************
 *                  E X T E R N A L    R E F E R E N C E                  *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
/* LMI */
UINT32 sp5kLmiFreqSet(UINT32 freq);
void sp5kLmiPinSet(UINT32 pinMsk);
void sp5kLmiDelaySet(UINT32 unit, UINT32 startDelay, UINT32 phaseNum, UINT32 *phaseDelay);
void sp5kLmiPatternSet(UINT32 phaseNum, UINT16 *phasePattern, UINT16 stopPattern);
void sp5kLmiTrigSet(UINT32 startPhase, UINT32 stepNum, UINT32 dir, UINT32 startStopMode);
void sp5kLmiTrig(void);
void sp5kLmiTrigRdy(UINT32 *trigRdy, UINT32 *stopPhase);
UINT32 sp5kLmiRunStepGet(void);
void sp5kLmiAbort(UINT32 *stopPhase, UINT32 *stepNum);
UINT32 sp5kLmiIsrReg(fpsp5kLmiIsr_t lmiisr);

/* SIN-PWM */
UINT32 sp5kSinPwmSetParam(sp5kSinPwmDev_t devId, sp5kSinPwmParam_t selector, UINT32 param1, UINT32 param2);
UINT32 sp5kSinPwmControl(sp5kSinPwmDev_t devId, sp5kSinPwmCtrl_t mode);
UINT32 sp5kSinPwmInfoGet(sp5kSinPwmDev_t devId, sp5kSinPwmInfo_t info, UINT32 *data);
UINT32 sp5kSinPwmIsrSet(UINT32 devId, sp5kSinPwmIsr_t sel, fpsp5kVoidCallback_t isr);

#endif /* #ifndef _SP5K_LMI_API_H_ */
