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
#include "app_com_def.h"
#include "app_com_api.h"
#include "sp5k_fs_api.h"
#include "app_hw_def.h"
#include "customization/dev_init.h"
#include "gpio_custom.h"
#include "app_btn_def.h"
#include "app_osd_api.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define IR_VALUE_BOUND 0xaa
#define IR_BEGIN_MIN  (0x1c0)
#define IR_BEGIN_MAX (0x240)
#define IR_BEGIN_REPEATE  (0x2A30+0x240)


/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
 //define key val
 #define IR_KEY_SNAP		0x48  //0x090
 #define IR_KEY_UP			0x40
 #define IR_KEY_DOWN		0x10
 #define IR_KEY_LEFT		0x20
 #define IR_KEY_RIGHT		0x60
 #define IR_KEY_SET			0xa0
 #define IR_KEY_MENU		0x90
 #define IR_KEY_MODE		0xE0 //0xd0
 #define IR_KEY_POWER		0x50//0x40

//ir remote thread param
#define IR_THR_PROI		 	12			
#define IR_THR_PREEMPT	  	12
#define IR_THR_TIME_SLICE 	0
#define IR_THR_AUTO_START 	1

//IR scan GPIO pin
#define IR_SCAN_PIN			SP5K_GPIO_INT8
 

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
UINT32 preTimeUs = 0;
UINT32 IRTable[37]={0};
UINT8 ir_i=0;
UINT8 ir_i_temp=0;
UINT8 ir_i_tempon=0;
UINT8 MOTIONSNAP=0;
UINT8 ir_repeate=0;

UINT16 getRepeatevalue;

TX_EVENT_FLAGS_GROUP IR_Key;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern UINT32 tmrTimeClockGet();

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
static void appIrKeyFunc(UINT16 irKeyVal);
static void appIrKey_thread(ULONG param);
void appIrSvrFunc(void);

/**
 * @fn        void appIrKeyInit(void)
 * @brief     appIrKeyInit
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    yong sun
 * @since     2012-9-11
 * @todo      N/A
 * @bug       N/A
*/

void 
appIrKeyInit(
	void
)
{
	TX_THREAD *thr;
	UINT32 err;

	/*BPrintf("appIrKeyInit\n");*/

	sp5kGpioIsrReg(IR_SCAN_PIN, SP5K_TRIG_LVL_RISE, appIrSvrFunc);

	err = sp5kOsEventFlagsCreate(&IR_Key, "IRApp");
	HOST_ASSERT(err == SP5K_SUCCESS);
	
	thr = sp5kOsThreadCreate("thrAppIR", appIrKey_thread, 0,
				IR_THR_PROI, IR_THR_PREEMPT, IR_THR_TIME_SLICE, IR_THR_AUTO_START);
	HOST_ASSERT(thr != 0);
	
	sp5kGpioIntEnable(IR_SCAN_PIN,0x01);

}
/**
 * @fn	 void appIrSvrFunc(void)
 * @brief	  appIrSvrFunc
 * @param	  NONE
 * @retval	  NONE
 * @see 	  NULL
 * @author	  yong sun
 * @since	  2012-9-11
 * @todo	  N/A
 * @bug 	  N/A
*/

void 
appIrSvrFunc(
	void
)
{
	UINT32 curTimeUs,tmpTimeUs;
	UINT32 err;
	
	curTimeUs = tmrTimeClockGet()/10;
	tmpTimeUs = curTimeUs - preTimeUs;	

	if((tmpTimeUs>IR_BEGIN_MIN) && (tmpTimeUs<IR_BEGIN_MAX))
	{
		ir_i_temp=ir_i;
		ir_i_tempon=1;
		ir_i=0;
		ir_repeate = 0;
		getRepeatevalue = 0;
	}
	else if((tmpTimeUs>IR_BEGIN_MAX) && (tmpTimeUs<IR_BEGIN_REPEATE))
	{
		ir_repeate++;
	}
	if(ir_i<37){
		IRTable[ir_i]=tmpTimeUs;
		ir_i+=1;
		if(ir_i == 33){
			err = sp5kOsEventFlagsSet(&IR_Key, (UINT32)ir_i, TX_OR);
			HOST_ASSERT(err == SP5K_SUCCESS);
		}
	}	
	
	preTimeUs=curTimeUs;
}

/**
 * @fn        void appUart_thread(void)
 * @brief     appUart_thread
 * @param     param
 * @retval    NONE
 * @see       NULL
 * @author    yong sun
 * @since     2012-9-11
 * @todo      N/A
 * @bug       N/A
*/

void 
appIrKey_thread(
	ULONG param
)
{
	UINT32 err;
	ULONG ActualEvents; 
	UINT16 getvalue,complement;
	UINT8 tk,temp=0;

	for (;;) {
		err = sp5kOsEventFlagsGet(&IR_Key, 1, TX_OR_CLEAR, &ActualEvents, TX_WAIT_FOREVER);
		HOST_ASSERT(err == SP5K_SUCCESS);

		printf("ActualEvents:%d\n",ActualEvents);
		
		if(ActualEvents == 33){
			getvalue=0x0000;
			complement=0;
			if((IRTable[0]<=0x210)&&(IRTable[0]>=0x1c0))/*check header*/
			{
				for (tk=17;tk<25;tk++)
				{
					if ( (IRTable[tk] <IR_VALUE_BOUND)/*&&(IRTable[tk] >0x50)*/)
					{
						getvalue=getvalue<<1;	
					}
					else if((IRTable[tk] >IR_VALUE_BOUND)/*&&(IRTable[tk] <0xff)*/)
					{
						getvalue=getvalue<<1;	
						getvalue+=1; 
					}
				}
			
				for (tk=25;tk<33;tk++)
				{
					if ( (IRTable[tk] <IR_VALUE_BOUND)/*&&(IRTable[tk] >0x50)*/)
					{
						complement=complement<<1;	
					}
					else if((IRTable[tk] >IR_VALUE_BOUND)/*&&(IRTable[tk] <0xff)*/)
					{
						complement=complement<<1;	
						complement+=1;
					}
				}
			
				if((getvalue^complement)!=0x00ff)
				{
					getvalue=0xffff;
				}
				for(tk=0;tk<37;tk++)
				{
					IRTable[tk]=0;
				}
			}else{
				getvalue=0xffff;	
			}
			ir_i=0;
			getRepeatevalue = getvalue;

			printf("getvalue:%x\n",getvalue);

			appIrKeyFunc(getvalue);
		}
	}	
}

static void 
appIrKeyFunc(
	UINT16 irKeyVal
)
{
	switch(irKeyVal){
		case IR_KEY_SNAP:
			if(IS_HDMI_IN){
				sp5kHostMsgSend(APP_KEY_PRESS_DOWN,0);
			} else{
				if(!IS_VIEW_GROUP(appActiveStateGet())){
					CLEAR_OSD_SRC;
					appStateChange(APP_STATE_STILL_PREVIEW,STATE_PARAM_NORMAL_INIT);
					sp5kHostMsgSend(APP_KEY_PRESS_S1,0x55);
				}else{
					sp5kHostMsgSend(APP_KEY_PRESS_S1,0);
				}
			}
			break;
		case IR_KEY_MODE:
			if(IS_HDMI_IN){
				sp5kHostMsgSend(APP_KEY_PRESS_UP,0);
			}else{
				if(!IS_VIDEO_GROUP(appActiveStateGet())){
					CLEAR_OSD_SRC;
					appStateChange(APP_STATE_VIDEO_PREVIEW,STATE_PARAM_NORMAL_INIT);
					sp5kHostMsgSend(APP_KEY_PRESS_S1,0x55);
				}else{
					sp5kHostMsgSend(APP_KEY_PRESS_S1,0);
				}
			}
			break;
		case IR_KEY_SET:
			sp5kHostMsgSend(APP_KEY_PRESS_SET,0);
			break;
		case IR_KEY_MENU:
			sp5kHostMsgSend(APP_KEY_PRESS_MENU,0);
			break;
		case IR_KEY_UP:
			sp5kHostMsgSend(APP_KEY_PRESS_UP,0);
			break;
		case IR_KEY_DOWN:
			sp5kHostMsgSend(APP_KEY_PRESS_DOWN,0);
			break;
		case IR_KEY_LEFT:
			sp5kHostMsgSend(APP_KEY_PRESS_LEFT,0);
			break;
		case IR_KEY_RIGHT:
			sp5kHostMsgSend(APP_KEY_PRESS_RIGHT,0);
			break;
		case IR_KEY_POWER:
			if(IS_HDMI_IN){
				sp5kHostMsgSend(APP_KEY_PRESS_SET,0);
			}else{
				//sp5kHostMsgSend(SP5K_MSG_POWER_OFF,0);
				sp5kHostMsgSend(APP_UI_MSG_POWER_OFF, APP_POWER_OFF_NORMAL);
			}
			break;
		default:
			break;
	}
}

