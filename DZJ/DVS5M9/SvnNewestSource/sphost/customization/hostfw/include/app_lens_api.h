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
#ifndef APP_LENS_API_H
#define APP_LENS_API_H

/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/
#include "gpio_custom.h"
#include "sp5k_global_api.h"
#include "api/sp5k_os_api.h"
#include "app_hw_def.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

#define LENS_DO_BLOCKING		1
#define LENS_DO_NONE_BLOCKING	0

/*-- Motor & Led configuration */
#define LENS_LED_ACTUATE_LAG			1	/* in ms, to wait Lens Led after actuated */
#define LENS_MOTOR_ACTUATE_LAG			0	/* in ms, to wait Lens Motor after actuated */

/*-- ZOOM Lens param */
#if defined(LENS_MODEL_D160)
/*-- Lens Configuration */
#define LENS_CLOSE_ZHM_LED_LEVEL_HI		0
	/* the Zoom Home Led IO level when Lens is closed */

#define ZOOM_ZS_NUM				(6)
		/* number of default stop of Zoom lens.
		 * It incluse the WS & TS. WS=ZS[0], TS=ZS[ZOOM_ZS_NUM-1]
		 */


//Modified by Brian 08/08/08
//The following  configuration is NOT CORRECT
#define FOCUS_HP_POS			(40)
#define FOCUS_CLOSE_POS			(FOCUS_HP_POS-40)
#define FOCUS_SPEC_CLOSE_POS	(-40)

#define FOCUS_POSITION_OFFSET	(FOCUS_CLOSE_POS - FOCUS_SPEC_CLOSE_POS)

#define FOCUS_0_S 70  
#define FOCUS_0_E 250 
#define FOCUS_1_S 70  
#define FOCUS_1_E 250 
#define FOCUS_2_S 70  
#define FOCUS_2_E 250 
#define FOCUS_3_S 70  
#define FOCUS_3_E 250 
#define FOCUS_4_S 70  
#define FOCUS_4_E 250 
#define FOCUS_5_S 70  
#define FOCUS_5_E 250 
#define FOCUS_6_S 70  
#define FOCUS_6_E 250
//Macro      
#define FOCUS_7_S 324   
#define FOCUS_7_E 369 
//

#define FOCUS_0_STEP	  5
#define FOCUS_1_STEP	  5
#define FOCUS_2_STEP	  5
#define FOCUS_3_STEP	  5
#define FOCUS_4_STEP	  5
#define FOCUS_5_STEP	  5
#define FOCUS_6_STEP	  5
#define FOCUS_7_STEP	  5


#define FOCUS_0_2M	282
#define FOCUS_1_2M	240
#define FOCUS_2_2M	208
#define FOCUS_3_2M	184
#define FOCUS_4_2M	184
#define FOCUS_5_2M	192
#define FOCUS_6_2M	174


#else
/*-- Lens Configuration */
#define LENS_CLOSE_ZHM_LED_LEVEL_HI		0
	/* the Zoom Home Led IO level when Lens is closed */

#define ZOOM_ZS_NUM				(7)
		/* number of default stop of Zoom lens.
		 * It incluse the WS & TS. WS=ZS[0], TS=ZS[ZOOM_ZS_NUM-1]
		 */
	 
//Modified by Brian 08/08/08	
	/*--
	 * Focus Position
	 */	
#define FOCUS_HP_POS			(40)
#define FOCUS_CLOSE_POS			(FOCUS_HP_POS-40)
#define FOCUS_SPEC_CLOSE_POS	(-40)
	
#define FOCUS_POSITION_OFFSET	(FOCUS_CLOSE_POS - FOCUS_SPEC_CLOSE_POS)
	
#define FOCUS_0_S 274       	
#define FOCUS_0_E 312 
#define FOCUS_1_S 228         
#define FOCUS_1_E 285 
#define FOCUS_2_S 190         
#define FOCUS_2_E 271 
#define FOCUS_3_S 159         
#define FOCUS_3_E 274 
#define FOCUS_4_S 149         
#define FOCUS_4_E 309 
#define FOCUS_5_S 141         
#define FOCUS_5_E 362 
#define FOCUS_6_S 100         
#define FOCUS_6_E 402 
		//Macro 	 
#define FOCUS_7_S 324   
#define FOCUS_7_E 369 
		//
		
#define FOCUS_0_STEP	  5
#define FOCUS_1_STEP	  6
#define FOCUS_2_STEP	  8
#define FOCUS_3_STEP	  9
#define FOCUS_4_STEP	  11
#define FOCUS_5_STEP	  12
#define FOCUS_6_STEP	  14
#define FOCUS_7_STEP	  5
		
#define FOCUS_0_2M	282
#define FOCUS_1_2M	240
#define FOCUS_2_2M	208
#define FOCUS_3_2M	184
#define FOCUS_4_2M	184
#define FOCUS_5_2M	192
#define FOCUS_6_2M	174


#endif


#define MSH_GPIO_ISR_ENABLE 0  //use sensor shutter trigger to close m-shutter

#if MSH_GPIO_ISR_ENABLE
#define SENSOR_RAW_RESET_TIME  797  //7.97ms
#else
#if (SP5K_SENSOR_MI9130)
#define SENSOR_RAW_RESET_TIME  3120 //31.2ms
#else
#define SENSOR_RAW_RESET_TIME  0
#endif
#endif


/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

typedef enum lensBattCheckCase_s {
	LENS_BATT_CASE_RESET_BOOT,
	LENS_BATT_CASE_LENS_CREATE,
	LENS_BATT_CASE_LENS_DESTROY,
	LENS_BATT_CASE_MAX
} lensBattCheckCase_t;

typedef enum lensLedID_s {
	LENS_LED_ID_ZHM			= (((UINT32)1)<<0),	/* zoom home */
	LENS_LED_ID_ZCLK		= (((UINT32)1)<<1),	/* zoom clk */
	LENS_LED_ID_FHM			= (((UINT32)1)<<2),	/* focus home */
	LENS_LED_ID_ALL			= (LENS_LED_ID_FHM | LENS_LED_ID_ZCLK | LENS_LED_ID_ZHM),
} lensLedID_t;

/* lensApp task event */
typedef enum lensAppTaskEventBitOrder_e {
	BO_EVENT_LENS_APP_TASK_INIT=0,
	BO_EVENT_LENS_APP_BOOT,
	BO_EVENT_LENS_APP_OPEN,
	BO_EVENT_LENS_APP_CLOSE,
	BO_EVENT_LENS_APP_TELE,
	BO_EVENT_LENS_APP_WIDE,
	BO_EVENT_LENS_APP_LENS_STOP_ACK,
	BO_EVENT_LENS_APP_LENS_BOOT_FOCUS_EARLY,
	BO_EVENT_LENS_APP_TASK_DEL=31
} lensAppTaskEventBitOrder_t;

typedef enum lensAppTaskEventDef_s {
	EVENT_LENS_APP_TASK_INIT		= (((UINT32)1)<<BO_EVENT_LENS_APP_TASK_INIT),
	EVENT_LENS_APP_BOOT				= (((UINT32)1)<<BO_EVENT_LENS_APP_BOOT),
	EVENT_LENS_APP_OPEN				= (((UINT32)1)<<BO_EVENT_LENS_APP_OPEN),
	EVENT_LENS_APP_CLOSE			= (((UINT32)1)<<BO_EVENT_LENS_APP_CLOSE),
	EVENT_LENS_APP_TELE				= (((UINT32)1)<<BO_EVENT_LENS_APP_TELE),
	EVENT_LENS_APP_WIDE				= (((UINT32)1)<<BO_EVENT_LENS_APP_WIDE),
	EVENT_LENS_APP_LENS_STOP_ACK	= (((UINT32)1)<<BO_EVENT_LENS_APP_LENS_STOP_ACK),
	EVENT_LENS_APP_LENS_BOOT_FOCUS_EARLY	= (((UINT32)1)<<BO_EVENT_LENS_APP_LENS_BOOT_FOCUS_EARLY),
	EVENT_LENS_APP_TASK_DEL			= (((UINT32)1)<<BO_EVENT_LENS_APP_TASK_DEL)
} lensAppTaskEventDef_t;

/*-- Lens Query ID */
typedef enum lensQueryID_s {
	LENS_QUERY_MOTION_STATE,
	LENS_QUERY_BOOT_DONE,
	LENS_QUERY_LENS_CLOSED,
	LENS_QUERY_ZOOM_TELE_PARKED,
	LENS_QUERY_ZOOM_WIDE_PARKED,
	LENS_QUERY_ZOOM_SEG_POS,
	LENS_QUERY_ZOOM_SEG_NUM,
	LENS_QUERY_ZOOM_SEG2POS,
	LENS_QUERY_ZOOM_STEP_POS,
	LENS_QUERY_IRIS_SEG_POS,
	LENS_QUERY_FOCUS_MOTION_STATE,
	LENS_QUERY_FOCUS_HOME_POS,
	LENS_QUERY_FOCUS_IS_HOME,
	LENS_QUERY_FOCUS_STEP_POS,
	LENS_QUERY_MAX
} lensQueryID_t;

/*-- Lens Status bit-mask order */
typedef enum lensStatusBitOrder_e {
	LENS_ST_BO_MOTION_STATE,
	LENS_ST_BO_MAX
} lensStatusBitOrder_t;

typedef enum lensStatusBitMask_e {
	LENS_ST_MSK_MOTION_STATE	= (((UINT32)1)<<LENS_ST_BO_MOTION_STATE),
	LENS_ST_MSK_CLR_ALL			= (~((UINT32)0))
} lensStatusBitMask_t;

typedef void appLensStopNotifyCBF_t(void);


typedef enum {
	MSHUT_SHORT_EXP_DELAY,
	MSHUT_LONG_EXP_DELAY,
	MSHUT_NONE_CLOSE
} mshutDelayMode_t;


/**************************************************************************
 *                               M A C R O S                              *
 **************************************************************************/

#define LENS_SLEEP(ms)	do { if (((UINT32)ms)>0) sp5kTimeDelay(SP5K_TIME_DELAY_1MS, (UINT32)ms); } while(0)

#define LENS_MSG_POST(msg, param) sp5kHostMsgSend((UINT32)(msg), (UINT32)(param))

/*-- Lens position check */
#if !defined(LENS_CLOSE_ZHM_LED_LEVEL_HI)
#error "LENS_CLOSE_ZHM_LED_LEVEL_HI is not defined !!"
#endif
#if LENS_CLOSE_ZHM_LED_LEVEL_HI
#define IS_LENS_CLOSE 	IO_LENS_LED_ZHM_GET
#else
#define IS_LENS_CLOSE 	(!IO_LENS_LED_ZHM_GET)
#endif

/**************************************************************************
 *          M O D U L E   V A R I A B L E S   R E F E R E N C E S         *
 **************************************************************************/

/*-- The following indicate whether if lensMotorPowerOff
 *   shall be ignored because MShut or Iris is busy.
 */
extern BOOL	glensPwrIsMShutBusy;
extern BOOL	glensPwrIsIrisBusy;

#if (LENS_BOOT_FOCUS_EARLY)
extern TX_EVENT_FLAGS_GROUP evLensBootFocusEarly;
	/*-- interactive event, which notify appLens task to trigger
	 * focus operatio at Lens BOOT phase. The time to start focus boot
	 * depends upon HOST.
	 */
#endif

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/
BOOL lensLedPowerQuery(UINT32 combo_id);
void lensLedPowerOn(UINT32 combo_id);
void lensLedPowerOff(UINT32 combo_id);
UINT32 lensLedRead(lensLedID_t id);

void lensAppTask(ULONG init_data);
void lensAppTaskInit(void);
void appLensAFCreate(void);
void appLensAFDestroy(void);
void appLensCreate(void);
void appLensDestroy(void);
UINT32 appLensQuery(UINT32 queryId);
UINT32 appLensStopWait(void);
UINT32 appLensCrashRecover(void);
void appLensInit(void);
void appLensBoot(void);
void appLensOpen(void);
UINT32 appLensClose(void);
UINT32 appLensTele(void);
UINT32 appLensWide(void);
void appLensStop(void);
void appLensStopCallbackSet(appLensStopNotifyCBF_t *pcallbackFunc);
void calibLensZoomSegGo(	UINT32 zmSeg);
UINT32 appLensZoomBootSlipCalib(void);
UINT32 appLensZoomTeleWideSlipCalib(UINT32 startSeg, UINT32 stopSeg);
UINT32 appLensZoomBlashSlipCalib(UINT32 startSeg, UINT32 stopSeg);
void appLensMShutLmiModeSet(UINT32 off);
/*-- Iris APIs */
void appIrisInit(void);
UINT32 appIrisLargeSet(void);
UINT32 appIrisSmallSet(void);

/*-- Focus APIs */
UINT32 appFocusPosGo(SINT16 pos);
void appFocusStopWait(void);

/*-- LMI Control APIs */
UINT32 appLMI_CTRL_Init(void);

/*-- MShutter APIs */
void lensMShutOpModeSet(BOOL isBlocking);
void lensMShutInit(void);
void lensMShutIdle(void);
UINT32 appMShutOpen(void);
UINT32 appMShutClose(void);
void lensMShutPowerOn(void);
void lensMShutPowerOff(void);
void lensMShutSnapCloseDelaySet(UINT32 delay);
void appLensMShutCallbackRegister(void);
UINT32 appLensMShutDelaySet(mshutDelayMode_t mode, UINT32 delay);

/*-------------------------------------------------------------------------
 *  DEBUG FUNCTION DECLARATIONS                                           *
 -------------------------------------------------------------------------*/
#define ZOOM_DBG_FUNCTION_EXPORT		1
#define DBG_ZOOM_NULL_FUNC				do { } while(0)
#if ZOOM_DBG_FUNCTION_EXPORT
void dbg_zoomMotorTele(void);
void dbg_zoomMotorWide(void);
void dbg_zoomMotorBrake(void);
void dbg_zoomMotorStandby(void);
#else
#define dbg_zoomMotorTele()		DBG_ZOOM_NULL_FUNC
#define dbg_zoomMotorWide()		DBG_ZOOM_NULL_FUNC
#define dbg_zoomMotorBrake()	DBG_ZOOM_NULL_FUNC
#define dbg_zoomMotorStandby()	DBG_ZOOM_NULL_FUNC
#endif /* DBG_ZOOM_NULL_FUNC */

#endif  /* APP_LENS_API_H */


