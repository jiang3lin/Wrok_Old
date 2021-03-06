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
#include "app_com_def.h"
#include "app_com_api.h"
#include "gpio_custom.h"
#include "api/sp5k_cec_api.h"
#include "app_wifi_connection.h"
#include "app_ui_para.h"
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                         *
 **************************************************************************/
#define CEC_COMMAND_WAIT_BUSY_COUNT 40

typedef enum {		/* EIA-CEA Standard */
	DISP_480P_4X3_60HZ		= 2,
	DISP_720P_16X9_60HZ		= 4,
	DISP_1080I_16X9_60HZ	= 5,
	DISP_480I_4X3_60HZ		= 6,
	DISP_576P_4X3_50HZ 		= 17,
	DISP_720P_16X9_50HZ		= 19,
	DISP_1080I_16X9_50HZ 	= 20,
	DISP_576I_4X3_50HZ 		= 21,
} dispEDIDSet_t;

typedef struct CEC_CMD_FRAME
{
    UINT8  byAddr;
    UINT8  byOpCode;
    UINT8  byDataSize;
    UINT8  byData[SP5K_CEC_OPERAND_TBL_SIZE];
    UINT32 uiTimeStamp;

} CEC_CMD_FRAME, *PCEC_CMD_FRAME;

typedef enum _HCE_CEC_MENU_REQUEST_TYPE
{
	HCE_CEC_MENU_REQUEST_TYPE_ACTIVATE 		= (UINT8 )0x0,
	HCE_CEC_MENU_REQUEST_TYPE_DEACTIVATE 	= 0x1,
	HCE_CEC_MENU_REQUEST_TYPE_QUERY		 	= 0x2
} HCE_CEC_MENU_REQUEST_TYPE, *PHCE_CEC_MENU_REQUEST_TYPE;


/**************************************************************************
 *                           D E C L A R A T I O N S                        *
 **************************************************************************/
void appCecStop(void);
BOOL IsHdmiPlugIn(void);
sp5kCecErrCode_e CecMsgSend( PCEC_CMD_FRAME psCecCmdBlock );
sp5kCecErrCode_e CecMsgSendLoop( PCEC_CMD_FRAME psCecCmdBlock );
void appCecHandle_Send_PhyAddr(void);
void appCecHandle_Send_DeviceVendorID(UINT8 desaddr);
void appCecHandle_Send_ImageViewOn(void);
void appCecHandle_Send_ActiveSource(UINT8 desaddr);
void appCecHandle_Send_GiveDevicePowerStatus(UINT8 desaddr);
void appCecHandleOneTouchPlay(void);
void appCecHandle_Send_FeatureAbort(UINT8 OpCode, UINT8 desaddr);
void appCecHandle_Send_ReportPowerStatus(UINT8 desaddr, UINT8 status);
void appCecHandle_Send_MenuStatus(UINT8 desaddr, UINT8 menustatus);
void appCecHandle_Send_UserControlPressed(UINT8 desaddr,UINT8  byKeyId);
void appCecCtrlLogicalAlloc(void);
void receiveMsgCallback(UINT32 instId,UINT32 iniAddr,UINT32 flwAddr,UINT32 opcode,UINT32 operandNum,UINT8 *operandTbl);
void appCecControlStateOnMsg(UINT32 msg,UINT32 param);
void appCecControlStateInit(void);
extern UINT32 _powerOnStateGet(void);
/**************************************************************************
 *                         G L O B A L    D A T A                        *
 **************************************************************************/
UINT8 resolution = 0xff;
static UINT32 cecPhyAddr;
static UINT8 txOperandTbl[SP5K_CEC_OPERAND_TBL_SIZE];
static UINT32 pbDev;
UINT32 previousStateGet;

/**************************************************************************
 *                             F U N C T I O N                         *
 **************************************************************************/

#if SPCA6330
void appCecStop(
	void
)
{
	sp5kCecControl(0, SP5K_CEC_CTRL_STOP);
	sp5kCecControl(0, SP5K_CEC_CTRL_CLOSE);
}



BOOL
IsHdmiPlugIn(
	void
)
{
	return sp5kDispHdmiHotPlugDetectGet(0);

}



sp5kCecErrCode_e CecMsgSend( PCEC_CMD_FRAME psCecCmdBlock )
{
	profLogPrintf(1, "SP5K_CEC_CTRL_MSG_SEND_WAIT	S OpCode=0x%x",psCecCmdBlock->byOpCode);
	sp5kCecControl(0, SP5K_CEC_CTRL_MSG_SEND_WAIT, pbDev, psCecCmdBlock->byAddr, psCecCmdBlock->byOpCode, psCecCmdBlock->byDataSize, psCecCmdBlock->byData);
	profLogPrintf(1, "SP5K_CEC_CTRL_MSG_SEND_WAIT	E");
	return (sp5kCecErrCode_e)SUCCESS;
}



sp5kCecErrCode_e CecMsgSendLoop( PCEC_CMD_FRAME psCecCmdBlock )
{
	printf(">>>>>>>CecMsgSendLoop >>>>> S  OpCode = 0x%x\n",psCecCmdBlock->byOpCode);
	sp5kCecErrCode_e byRes = SP5K_CEC_ERR_TX_BUSY;
	UINT16 CEC_WaitCount =CEC_COMMAND_WAIT_BUSY_COUNT;

	while( byRes == SP5K_CEC_ERR_TX_BUSY || byRes == SP5K_CEC_ERR_TX_TIMEOUT){
		if(IsHdmiPlugIn())
			byRes = CecMsgSend(psCecCmdBlock);
		else{
			printf("HDMI Unplug!!-1\n");
			break;
		}

		printf("CEC MSG SEND ERROR CODE:    %d\n",byRes);
		APP_SLEEP_MS(40);
		CEC_WaitCount --;
		if(CEC_WaitCount == 0 ) break;
	}
	printf(">>>>>>>CecMsgSendLoop >>>>> E  OpCode = 0x%x\n",psCecCmdBlock->byOpCode);
	return byRes;
}


void
appCecHandle_Send_PhyAddr(
	void
)
{
	CEC_CMD_FRAME	sCmdFrame={0};
	sp5kCecErrCode_e byRes = SP5K_CEC_ERR_TX_BUSY;
	UINT16 CEC_WaitCount =CEC_COMMAND_WAIT_BUSY_COUNT;
	
	CEC_WaitCount = CEC_WaitCount;
	sCmdFrame.byOpCode = SP5K_CEC_MSG_REPORT_PHYSICAL_ADDRESS;//0x84
	sCmdFrame.byAddr = SP5K_CEC_LOGI_ADDR_BROADCAST;
	sCmdFrame.byDataSize = 3;
	sCmdFrame.byData[0] = txOperandTbl[0];
	sCmdFrame.byData[1] = txOperandTbl[1];
	sCmdFrame.byData[2] = txOperandTbl[2];;

	if(pbDev == SP5K_CEC_LOGI_ADDR_UNREGISTERED)		return;
	//if( gbyIsTV_NACK && (sCmdFrame.byAddr==SP5K_CEC_DEV_TYPE_TV) )	return;
	byRes = CecMsgSendLoop(&sCmdFrame);

	if(byRes == SP5K_CEC_ERR_SENT_ACK || byRes == SP5K_CEC_ERR_SENT_NAK){
		printf(">>>>>>>>>>>>> Physical Addr Allocate SUCCESS <<<<<<<<<<<");
	}else{
		printf(">>>>>>>>>>>>> TV has no send ACK/NAK <<<<<<<<<<<");
	}
}



void
appCecHandle_Send_DeviceVendorID(
	UINT8 desaddr
)
{
	CEC_CMD_FRAME	sCmdFrame={0};
	sp5kCecErrCode_e byRes = SP5K_CEC_ERR_TX_BUSY;
	UINT16 CEC_WaitCount =CEC_COMMAND_WAIT_BUSY_COUNT;

	CEC_WaitCount = CEC_WaitCount;
	sCmdFrame.byOpCode = SP5K_CEC_MSG_DEVICE_VENDOR_ID;//0x87
	sCmdFrame.byAddr = desaddr;
	sCmdFrame.byDataSize = 3;
	sCmdFrame.byData[0] = 0x08;
	sCmdFrame.byData[1] = 0x00;
	sCmdFrame.byData[2] = 0x46;

	RPrintf("send SP5K_CEC_MSG_DEVICE_VENDOR_ID//0x87\n");
	byRes = CecMsgSendLoop(&sCmdFrame);
}



void
appCecHandle_Send_ImageViewOn(
	void
)
{
	CEC_CMD_FRAME	sCmdFrame={0};
	sp5kCecErrCode_e byRes = SP5K_CEC_ERR_TX_BUSY;
	UINT16 CEC_WaitCount =CEC_COMMAND_WAIT_BUSY_COUNT;

	CEC_WaitCount = CEC_WaitCount;
	sCmdFrame.byOpCode = SP5K_CEC_MSG_IMAGE_VIEW_ON;//0x04
	sCmdFrame.byAddr = SP5K_CEC_LOGI_ADDR_TV;
	sCmdFrame.byDataSize = 0;

	RPrintf("send SP5K_CEC_MSG_IMAGE_VIEW_ON//0x04\n");

	byRes = CecMsgSendLoop(&sCmdFrame);
}



void
appCecHandle_Send_ActiveSource(
	UINT8 desaddr
)
{
	CEC_CMD_FRAME	sCmdFrame={0};
	sp5kCecErrCode_e byRes = SP5K_CEC_ERR_TX_BUSY;
	UINT16 CEC_WaitCount =CEC_COMMAND_WAIT_BUSY_COUNT;

	CEC_WaitCount = CEC_WaitCount;
	sCmdFrame.byOpCode = SP5K_CEC_MSG_ACTIVE_SOURCE;//0x82
	sCmdFrame.byAddr = desaddr;
	sCmdFrame.byDataSize = 2;
	sCmdFrame.byData[0] = txOperandTbl[0];
	sCmdFrame.byData[1] = txOperandTbl[1];

	RPrintf("send SP5K_CEC_MSG_ACTIVE_SOURCE//0x82\n");
	byRes = CecMsgSendLoop(&sCmdFrame);
}



void
appCecHandle_Send_GiveDevicePowerStatus(
	UINT8 desaddr
)
{
	CEC_CMD_FRAME	sCmdFrame={0};
	sp5kCecErrCode_e byRes = SP5K_CEC_ERR_TX_BUSY;
	UINT16 CEC_WaitCount =CEC_COMMAND_WAIT_BUSY_COUNT;

	CEC_WaitCount = CEC_WaitCount;
	sCmdFrame.byOpCode = SP5K_CEC_MSG_GIVE_DEVICE_POWER_STATUS;//0x8F
	sCmdFrame.byAddr = desaddr;
	sCmdFrame.byDataSize = 0;

	RPrintf("send SP5K_CEC_MSG_GIVE_DEVICE_POWER_STATUS//0x8F\n");
	byRes = CecMsgSendLoop(&sCmdFrame);
}



void
appCecHandleOneTouchPlay(
	void
)
{
	if( pbDev == SP5K_CEC_LOGI_ADDR_UNREGISTERED) 	return;

	printf("--- OneTouchPlay S---\n");
	appCecHandle_Send_ImageViewOn();
	APP_SLEEP_MS(200);
	appCecHandle_Send_ActiveSource(SP5K_CEC_LOGI_ADDR_BROADCAST);
	APP_SLEEP_MS(200);
	appCecHandle_Send_GiveDevicePowerStatus(SP5K_CEC_LOGI_ADDR_TV);	/* Trigger TV power on from standby mode */
	printf("--- OneTouchPlay E---\n");

}



void
appCecHandle_Send_FeatureAbort(
	UINT8 OpCode,
	UINT8 desaddr
)
{
	UINT8 usErrorCode=0;
	CEC_CMD_FRAME	sCmdFrame={0};
	sp5kCecErrCode_e byRes = SP5K_CEC_ERR_TX_BUSY;
	UINT16 CEC_WaitCount =CEC_COMMAND_WAIT_BUSY_COUNT;

	CEC_WaitCount = CEC_WaitCount;
	sCmdFrame.byOpCode = SP5K_CEC_MSG_FEATURE_ABORT;//0x00
	sCmdFrame.byAddr = desaddr;
	sCmdFrame.byDataSize = 2;
	sCmdFrame.byData[0] = OpCode;
	sCmdFrame.byData[1] = usErrorCode;
	RPrintf("send SP5K_CEC_MSG_FEATURE_ABORT//0x00\n");
	byRes = CecMsgSendLoop(&sCmdFrame);
}



void
appCecHandle_Send_ReportPowerStatus(
	UINT8 desaddr,
	UINT8 status
)
{
	CEC_CMD_FRAME	sCmdFrame={0};
	sp5kCecErrCode_e byRes = SP5K_CEC_ERR_TX_BUSY;
	UINT16 CEC_WaitCount =CEC_COMMAND_WAIT_BUSY_COUNT;

	CEC_WaitCount = CEC_WaitCount;
	sCmdFrame.byOpCode = SP5K_CEC_MSG_REPORT_POWER_STATUS;//0x90
	sCmdFrame.byAddr = desaddr;
	sCmdFrame.byDataSize = 1;
	sCmdFrame.byData[0] = status;

	RPrintf("send SP5K_CEC_MSG_REPORT_POWER_STATUS//0x90\n");
	byRes = CecMsgSendLoop(&sCmdFrame);
}



void
appCecHandle_Send_MenuStatus(
	UINT8 desaddr,
	UINT8 menustatus
)
{
	CEC_CMD_FRAME	sCmdFrame={0};
	sp5kCecErrCode_e byRes = SP5K_CEC_ERR_TX_BUSY;
	UINT16 CEC_WaitCount =CEC_COMMAND_WAIT_BUSY_COUNT;

	CEC_WaitCount = CEC_WaitCount;
	sCmdFrame.byOpCode = SP5K_CEC_MSG_MENU_STATUS;//0x8E
	sCmdFrame.byAddr = desaddr;
	sCmdFrame.byDataSize = 1;
	sCmdFrame.byData[0] = menustatus;

	RPrintf("send SP5K_CEC_MSG_MENU_STATUS//0x8E\n");
	byRes = CecMsgSendLoop(&sCmdFrame);
}



void
appCecHandle_Send_UserControlPressed(
	UINT8 desaddr,
	UINT8  byKeyId
)
{
	CEC_CMD_FRAME	sCmdFrame={0};
	sp5kCecErrCode_e byRes = SP5K_CEC_ERR_TX_BUSY;
	UINT16 CEC_WaitCount =CEC_COMMAND_WAIT_BUSY_COUNT;

	CEC_WaitCount = CEC_WaitCount;
	sCmdFrame.byOpCode = SP5K_CEC_MSG_USER_CONTROL_PRESSED;//0x44
	sCmdFrame.byAddr = desaddr;
	sCmdFrame.byDataSize = 1;
	sCmdFrame.byData[0] = byKeyId;

	RPrintf("send SP5K_CEC_MSG_USER_CONTROL_PRESSED//0x44\n");
	byRes = CecMsgSendLoop(&sCmdFrame);
}



void appCecHandle_Send_SetOsdName(
	UINT8 desaddr
)
{
	CEC_CMD_FRAME	sCmdFrame={0};
	sp5kCecErrCode_e byRes = SP5K_CEC_ERR_TX_BUSY;
	UINT16 CEC_WaitCount =CEC_COMMAND_WAIT_BUSY_COUNT;

	sCmdFrame.byOpCode = SP5K_CEC_MSG_SET_OSD_NAME;//0x47
	sCmdFrame.byAddr = desaddr;
	sCmdFrame.byDataSize = 1;
	sCmdFrame.byData[0] = 0x04;
	sCmdFrame.byDataSize = 8;
	//#define OSD_NAME 				"SPCA6330"
	UINT8 *osdName = "SPCA6330";

	RPrintf("send SP5K_CEC_MSG_SET_OSD_NAME//0x47\n");
	while( byRes == SP5K_CEC_ERR_TX_BUSY || byRes == SP5K_CEC_ERR_TX_TIMEOUT){
		byRes = sp5kCecControl(0, SP5K_CEC_CTRL_MSG_SEND_WAIT, pbDev, sCmdFrame.byAddr, sCmdFrame.byOpCode, sCmdFrame.byDataSize, osdName);
		printf("ERROR CODE:    %d\n",byRes);
		APP_SLEEP_MS(40);
		CEC_WaitCount --;
		if(CEC_WaitCount == 0 ) break;
	}
}




void appCecCtrlLogicalAlloc(
	void
)
{
	UINT32 cecAllocAddrReturnVal=0;

	printf("SP5K_CEC_CTRL_LOGICAL_ADDR_ALLOC	S");
	cecAllocAddrReturnVal = sp5kCecControl(0, SP5K_CEC_CTRL_LOGICAL_ADDR_ALLOC, SP5K_CEC_DEV_TYPE_PLAYBACK_DEVICE, &pbDev);
	profLogPrintf(1, "SP5K_CEC_CTRL_LOGICAL_ADDR_ALLOC	E");

	sp5kHostMsgSend(APP_UI_MSG_CEC_CB, 0);
	if(sp5kCecControl(0, SP5K_CEC_CTRL_LOGICAL_ADDR_GET, SP5K_CEC_DEV_TYPE_PLAYBACK_DEVICE, &pbDev)==SUCCESS)
	{
        RPrintf("CEC Supported !!\n");
    }
    else
	{
		RPrintf("CEC Not Supported !!\n");
		return;
    }
	profLogPrintf(1, "SP5K_CEC_CTRL_PHYSICALL_ADDR_GET	S");
	sp5kCecControl(0, SP5K_CEC_CTRL_PHYSICALL_ADDR_GET, &cecPhyAddr);
	txOperandTbl[0] = cecPhyAddr >> 8;	 /* A.B. */
	txOperandTbl[1] = cecPhyAddr & 0xFF;  /* C.D. */
	txOperandTbl[2] = SP5K_CEC_DEV_TYPE_PLAYBACK_DEVICE;
	profLogPrintf(1, "SP5K_CEC_CTRL_PHYSICALL_ADDR_GET	E");
	printf("cecPbDevAddr=0x%x,	byPhysAddr [0]=0x%x,  byPhysAddr [1]=0x%x\n",pbDev,txOperandTbl[0],txOperandTbl[1]);

	printf("Return Value = %d, playback device = %d\n", cecAllocAddrReturnVal, pbDev);


	appCecHandleOneTouchPlay();
	appCecHandle_Send_PhyAddr();
//	appCecHandle_Send_DeviceVendorID( SP5K_CEC_LOGI_ADDR_BROADCAST );

}



void
receiveMsgCallback(
	UINT32 instId,
	UINT32 iniAddr,
	UINT32 flwAddr,
	UINT32 opcode,
	UINT32 operandNum,
	UINT8 *operandTbl
)
{
	profLogPrintf(1, "receiveMsgCallback	S	opcode=0x%x",opcode);
	printf("-------------opcode=0x%x,operandNum=%d  ,  iniAddr=0x%x,flwAddr=0x%x------------\n",opcode,operandNum,iniAddr,flwAddr);
	printf("--operandTbl[0]: 0x%02x  operandTbl[1]: 0x%02x  operandTbl[2]: 0x%02x   operandTbl[3]: 0x%02x--\n", operandTbl[0],operandTbl[1],operandTbl[2],operandTbl[3]);

	if(pbDev == SP5K_CEC_LOGI_ADDR_UNREGISTERED)		return;

	switch ( opcode ) {
		case SP5K_CEC_MSG_USER_CONTROL_PRESSED: // 0x44
			if( operandNum != 1)	break;
			if( (iniAddr==SP5K_CEC_LOGI_ADDR_TV) && (flwAddr==pbDev) )
			{
				printf("---Button 0x%02X Pressed---\n", operandTbl[0]);
				switch(operandTbl[0])
				{
					case SP5K_CEC_USER_CTRL_LEFT:
					sp5kHostMsgSend(SP5K_MSG_ADC_BUTTON_PRESS, 2);	/* Param: ADC key map */
						break;
					case SP5K_CEC_USER_CTRL_RIGHT:
					sp5kHostMsgSend(SP5K_MSG_ADC_BUTTON_PRESS, 3);
						break;
					default:
						appCecHandle_Send_FeatureAbort(SP5K_CEC_MSG_USER_CONTROL_PRESSED,iniAddr);
					break;
				}
			}
		break;

		case SP5K_CEC_MSG_REPORT_PHYSICAL_ADDRESS://0x84
			if((iniAddr!=SP5K_CEC_LOGI_ADDR_BROADCAST) && (flwAddr==pbDev) )
			{
				printf("---SP5K_CEC_MSG_REPORT_PHYSICAL_ADDRESS://0x84---\n");
				appCecHandle_Send_FeatureAbort(SP5K_CEC_MSG_REPORT_PHYSICAL_ADDRESS,iniAddr);
			}
		break;

		case SP5K_CEC_MSG_GIVE_PHYSICAL_ADDRESS://0x83
			if ( flwAddr == pbDev) {
				printf("---SP5K_CEC_MSG_GIVE_PHYSICAL_ADDRESS://0x83---\n");
				appCecHandle_Send_PhyAddr();
			}
		break;

		case SP5K_CEC_MSG_GIVE_DEVICE_VENDOR_ID:	//0x8c
			if ( flwAddr == pbDev) {
				printf("---SP5K_CEC_MSG_GIVE_DEVICE_VENDOR_ID://0x8c---\n");
				appCecHandle_Send_DeviceVendorID(iniAddr);
				#if 0
				operandTbl[0] = 0x00;
				operandTbl[1] = 0x07;
				operandTbl[2] = 0x33;
				sp5kCecControl(0, SP5K_CEC_CTRL_MSG_SEND, cecPbDevAddr, iniAddr, SP5K_CEC_MSG_DEVICE_VENDOR_ID, 3, operandTbl);
				#endif
			}
		break;

		case SP5K_CEC_MSG_VENDOR_COMMAND_WITH_ID:  // 0xA0
			printf("---SP5K_CEC_MSG_VENDOR_COMMAND_WITH_ID://0xA0---\n");
			UINT8 nDataCount =0;
			for(nDataCount=0;nDataCount<operandNum;nDataCount++)
			{
				printf( "operandTbl[%d]: 0x%02x\n", nDataCount,operandTbl[nDataCount]);
			}
			printf( "ucRcvInitAddr: 0x%02x --- byLogAddr: 0x%02x\n", iniAddr, pbDev);
			printf( "ucRcvDestAddr: 0x%02x\n", flwAddr);

			if( (iniAddr!=SP5K_CEC_LOGI_ADDR_BROADCAST) && (flwAddr==pbDev) )
			{
				appCecHandle_Send_FeatureAbort(SP5K_CEC_MSG_VENDOR_COMMAND_WITH_ID,iniAddr);
			}
		break;

		case SP5K_CEC_MSG_SET_MENU_LANGUAGE:  // 0x32
			if( (iniAddr!=SP5K_CEC_LOGI_ADDR_BROADCAST) && (flwAddr==pbDev) )
			{
				printf("---SP5K_CEC_MSG_SET_MENU_LANGUAGE://0x32---\n");
				appCecHandle_Send_FeatureAbort(SP5K_CEC_MSG_SET_MENU_LANGUAGE,iniAddr);
			}
		break;

		case SP5K_CEC_MSG_GET_MENU_LANGUAGE:  // 0x91
			if( flwAddr == pbDev )
			{
				printf("---SP5K_CEC_MSG_GET_MENU_LANGUAGE://0x91---\n");
				appCecHandle_Send_FeatureAbort(SP5K_CEC_MSG_GET_MENU_LANGUAGE, iniAddr);
			}
		break;

		case SP5K_CEC_MSG_DEVICE_VENDOR_ID:  // 0x87
			if( (iniAddr!=SP5K_CEC_LOGI_ADDR_BROADCAST) && (flwAddr==pbDev)  )
			{
				printf("---SP5K_CEC_MSG_DEVICE_VENDOR_ID://0x87---\n");
				appCecHandle_Send_FeatureAbort(SP5K_CEC_MSG_DEVICE_VENDOR_ID,iniAddr);
			}
		break;

		case SP5K_CEC_MSG_GIVE_DEVICE_POWER_STATUS:  // 0x8f
			if( (iniAddr!=SP5K_CEC_LOGI_ADDR_BROADCAST) && (flwAddr==pbDev) )
			{
				printf("---SP5K_CEC_MSG_GIVE_DEVICE_POWER_STATUS://0x8f---\n");
				appCecHandle_Send_ReportPowerStatus(iniAddr,SP5K_CEC_POWER_ON);
			}
		break;

		case SP5K_CEC_MSG_MENU_REQUEST:		//0x8d
			if( operandNum != 1)	break;
			if( (iniAddr!=SP5K_CEC_LOGI_ADDR_BROADCAST) && (flwAddr==pbDev) )
			{
				switch( operandTbl[0] )
				{
					printf("---SP5K_CEC_MSG_MENU_REQUEST://0x8d---\n");
					case HCE_CEC_MENU_REQUEST_TYPE_ACTIVATE:
					case HCE_CEC_MENU_REQUEST_TYPE_DEACTIVATE:
					case HCE_CEC_MENU_REQUEST_TYPE_QUERY:
					{
						appCecHandle_Send_MenuStatus(iniAddr, HCE_CEC_MENU_REQUEST_TYPE_ACTIVATE);
					}
					break;

					default:
						appCecHandle_Send_FeatureAbort(SP5K_CEC_MSG_MENU_REQUEST,iniAddr);
					break;
				}
			}
		break;

		case SP5K_CEC_MSG_REPORT_POWER_STATUS:	//0x90
			if( operandNum != 1)	break;

			if( (iniAddr!=SP5K_CEC_LOGI_ADDR_BROADCAST) && (flwAddr==pbDev) )
			{
				printf("---SP5K_CEC_MSG_REPORT_POWER_STATUS: 0x%x//0x90---\n",  operandTbl[0]);
				if( (operandTbl[0] == SP5K_CEC_POWER_ON) || (operandTbl[0] == SP5K_CEC_POWER_STANDBY_TO_ON))
				{
					printf("TV is On now\n");
					#if 0
					appCecHandle_Send_ImageViewOn();
					APP_SLEEP_MS(200);
					appCecHandle_Send_ActiveSource(SP5K_CEC_LOGI_ADDR_BROADCAST);
					APP_SLEEP_MS(200);
					appCecHandle_Send_MenuStatus(SP5K_CEC_LOGI_ADDR_TV, HCE_CEC_MENU_REQUEST_TYPE_ACTIVATE);
					#endif
				}
				else
				{
					printf("TV is stnadby\n");
					appCecHandle_Send_UserControlPressed(SP5K_CEC_LOGI_ADDR_TV,SP5K_CEC_USER_CTRL_POWER);
					appCecHandle_Send_GiveDevicePowerStatus(SP5K_CEC_LOGI_ADDR_TV);
				}
			}
		break;

		case SP5K_CEC_MSG_GET_CEC_VERSION:	//0x9F
			printf("---SP5K_CEC_MSG_GET_CEC_VERSION://0x9F---\n");

		break;

		/* get <CEC Version> return. */
		case SP5K_CEC_MSG_CEC_VERSION:	//0x9E
			printf("---SP5K_CEC_MSG_CEC_VERSION://0x9E---\n");
			if ( operandTbl[0] == 0x04 ) {
				printf("CEC version 1.3.a\n\n");
			}
			else if ( operandTbl[0] == 0x05 ) {
				printf("CEC version 1.4\n\n");
			}
		break;

		case SP5K_CEC_MSG_GIVE_OSD_NAME:	//0x46
			if( (iniAddr!=SP5K_CEC_LOGI_ADDR_BROADCAST) && (flwAddr==pbDev) )
			{
				printf("---SP5K_CEC_MSG_GIVE_OSD_NAME://0x46---\n");
				appCecHandle_Send_SetOsdName(iniAddr);
			}
			else
			{
				printf("HCE_CEC_OP_CODE_GIVE_OSD_NAME not match!!!!!\n");
			}
		break;

		case SP5K_CEC_MSG_SET_OSD_NAME:
		//	printf("%s, %d SP5K_CEC_MSG_SET_OSD_NAME\n", __FUNCTION__, __LINE__);
		//	printf("osd name=%s", rxOperandTbl);
			appStateChange(APP_STATE_PB_MAIN, STATE_PARAM_HDMI_PLUG);
		break;

		case SP5K_CEC_MSG_VENDOR_REMOTE_BUTTON_DOWN:	//0x8a
			if( (iniAddr!=SP5K_CEC_LOGI_ADDR_BROADCAST) && (flwAddr==pbDev) )
			{
				printf("---SP5K_CEC_MSG_VENDOR_REMOTE_BUTTON_DOWN://0x8a---\n");
				// key function
				appCecHandle_Send_FeatureAbort(SP5K_CEC_MSG_VENDOR_REMOTE_BUTTON_DOWN,iniAddr);
			}
		break;

		case SP5K_CEC_MSG_VENDOR_REMOTE_BUTTON_UP:	//0x8b
			if( (iniAddr!=SP5K_CEC_LOGI_ADDR_BROADCAST) && (flwAddr==pbDev) )
			{
				printf("---SP5K_CEC_MSG_VENDOR_REMOTE_BUTTON_UP://0x8b---\n");
				// key function
				appCecHandle_Send_FeatureAbort(SP5K_CEC_MSG_VENDOR_REMOTE_BUTTON_UP,iniAddr);
			}
		break;

		case SP5K_CEC_MSG_ACTIVE_SOURCE:  // 0x82
			//printf( "Data Size: %d\n", ucDataSize);
			//printf( "operandTbl[0]: 0x%02x\n", operandTbl[0]);
			//printf( "operandTbl[1]: 0x%02x\n", operandTbl[1]);
			//printf( "ucRcvInitAddr: 0x%02x\n", ucRcvInitAddr);
			//printf( "ucRcvDestAddr: 0x%02x\n", ucRcvDestAddr);
			if( (iniAddr!=SP5K_CEC_LOGI_ADDR_BROADCAST) && (flwAddr==SP5K_CEC_LOGI_ADDR_BROADCAST) )
			{
				if( operandNum != 2)	break;
				//printf("*** Rcv <Active Source> ***\n");
				printf("---SP5K_CEC_MSG_ACTIVE_SOURCE://0x82---\n");
				//printf( "byPhysAddr[0]: 0x%02x\n", byPhysAddr[0]);
				//printf( "byPhysAddr[1]: 0x%02x\n", byPhysAddr[1]);
				if((operandTbl[0]!=txOperandTbl[0]) && (operandTbl[1]!=txOperandTbl[1]) )
				{
					// printf("---gbyIsDSCActive = %d://0x82---\n",gbyIsDSCActive);
					// if( gbyIsDSCActive )	HceCecHandle_Send_InactiveSource( iniAddr );
					// printf("SendMsg_1 0x%x\n",MSG_UI_CEC_RETURN_STANDBY);
					// SendUIMsg(MSG_UI_CEC_RETURN_STANDBY);
				}
			}
		break;

		case SP5K_CEC_MSG_REQUEST_ACTIVE_SOURCE:	//0x85
			if( flwAddr == SP5K_CEC_LOGI_ADDR_BROADCAST)
			{
				printf("---SP5K_CEC_MSG_REQUEST_ACTIVE_SOURCE://0x85---\n");
				appCecHandle_Send_ActiveSource( SP5K_CEC_LOGI_ADDR_BROADCAST );
				appCecHandle_Send_MenuStatus(SP5K_CEC_LOGI_ADDR_TV, HCE_CEC_MENU_REQUEST_TYPE_ACTIVATE);
			}
		break;

		case SP5K_CEC_MSG_SET_STREAM_PATH: //0x86
			if( operandNum != 2)	break;
			if( flwAddr == SP5K_CEC_LOGI_ADDR_BROADCAST)
			{
				printf("---SP5K_CEC_MSG_SET_STREAM_PATH://0x86---\n");
				if( (operandTbl[0]==txOperandTbl[0]) && (operandTbl[1]==txOperandTbl[1]) )
				{
					appCecHandle_Send_ActiveSource( SP5K_CEC_LOGI_ADDR_BROADCAST );
					appCecHandle_Send_MenuStatus(SP5K_CEC_LOGI_ADDR_TV, HCE_CEC_MENU_REQUEST_TYPE_ACTIVATE);
				}
			}
		break;

		case SP5K_CEC_MSG_IMAGE_VIEW_ON:  // 0x04
			if( (iniAddr!=SP5K_CEC_LOGI_ADDR_BROADCAST) && (flwAddr==pbDev) )
			{
				printf("---SP5K_CEC_MSG_IMAGE_VIEW_ON://0x04---\n");
				appCecHandle_Send_FeatureAbort(SP5K_CEC_MSG_IMAGE_VIEW_ON,iniAddr);
			}
		break;

		case SP5K_CEC_MSG_TEXT_VIEW_ON:  //0x0d
			if( (iniAddr!=SP5K_CEC_LOGI_ADDR_BROADCAST) && (flwAddr==pbDev) )
			{
				printf("---SP5K_CEC_MSG_TEXT_VIEW_ON://0x0d---\n");
				appCecHandle_Send_FeatureAbort(SP5K_CEC_MSG_TEXT_VIEW_ON,iniAddr);
			}
		break;

		case SP5K_CEC_MSG_FEATURE_ABORT:  // 0x00
			if( (iniAddr!=SP5K_CEC_LOGI_ADDR_BROADCAST) && (flwAddr==pbDev) )
			{
				printf("---SP5K_CEC_MSG_FEATURE_ABORT://0x00---\n");
				printf("*** Rcv <Feature Abort> ***\n");
			}
		break;

		case SP5K_CEC_MSG_ABORT:  // 0xff
		{
			//UINT8 ucCounter = 0;
			printf("---SP5K_CEC_MSG_ABORT://0xff---\n");
			printf(">>HCE_CEC_OP_CODE_ABORT<<\n");
			appCecHandle_Send_FeatureAbort(SP5K_CEC_MSG_ABORT, iniAddr);
		}
		break;
	#if 0
		/* get <User Control Released>. */
		case SP5K_CEC_MSG_USER_CONTROL_RELEASED:	//0x45
			if ( (iniAddr == SP5K_CEC_LOGI_ADDR_TV) && (flwAddr == pbDev))
			{
				printf("---SP5K_CEC_MSG_USER_CONTROL_RELEASED://0x45---\n");
			//	PushKey(KEY_ACTIVE);
			//	PushKey( KEY_CEC_RELEASE);
				printf("Button Released\n");
			}
		break;

		default:
			/* send <Feature Abort>.
			operandTbl[0] = opcode;
			operandTbl[1] = SP5K_CEC_ABORT_UNRECOGNIZED_OPCODE;
			sp5kCecControl(0, SP5K_CEC_CTRL_MSG_SEND, cecPbDevAddr, iniAddr, SP5K_CEC_MSG_FEATURE_ABORT, 2, operandTbl);*/
			if( flwAddr == pbDev )
			{
				UINT8 ucCounter = 0;
				printf("---default:---\n");
				printf("*** Rcv NotSupportedCmd ***\n");
				appCecHandle_Send_FeatureAbort( opcode, iniAddr);
			}
		break;

		case SP5K_CEC_MSG_STANDBY:	// 0x36
			if(IsCECOpen)
			{
				printf("---SP5K_CEC_MSG_STANDBY://0x36---\n");
				//printf("*** Rcv <Standby> ***\n");
				//IsCECStandBy= TRUE; SendAppMsg(MSG_HDMI_OUT);
			//	SendUIMsg(MSG_UI_FORCE_POWER_OFF_CAMERA);
				//printf("*** Rcv <Standby> ***\n");
				//SendAppMsg(UI_MODE_PLAYBACK_MAIN);
				//SendAppMsg(MSG_UI_PLAYBACK_REFRESH_CURRENT_FILE);
			}
		break;

		case SP5K_CEC_MSG_VENDOR_COMMAND:
			sp5kCecControl(0, SP5K_CEC_CTRL_MSG_SEND, pbDev, SP5K_CEC_LOGI_ADDR_TV, SP5K_CEC_MSG_GIVE_DEVICE_POWER_STATUS, 0, NULL);
			if(opcode == SP5K_CEC_MSG_FEATURE_ABORT){
				printf("txOperandTbl[0] = 0x%x\n", txOperandTbl[0]);
				printf("txOperandTbl[1] = 0x%x\n", txOperandTbl[1]);
			}
			if (opcode == SP5K_CEC_MSG_VENDOR_COMMAND){
				txOperandTbl[0] = 0x89;
				txOperandTbl[1] = 0;
				sp5kCecControl(0, SP5K_CEC_CTRL_MSG_SEND, pbDev, SP5K_CEC_LOGI_ADDR_TV, SP5K_CEC_MSG_FEATURE_ABORT, 2, txOperandTbl);
			}
		break;

		case SP5K_CEC_MSG_PLAY: 	//0x41
			if(!IsCECOpen)	return;
			if( operandNum != 1)	break;
			if( (iniAddr!=SP5K_CEC_LOGI_ADDR_BROADCAST) && (flwAddr==pbDev) )
			{
				printf("---SP5K_CEC_MSG_PLAY://0x41---\n");
				switch( operandTbl[0] )
				{
					case SP5K_CEC_PLAY_MODE_PLAY_FORWARD:
						PushKey(KEY_ACTIVE);PushKey( KEY_CEC_PLAY );
						//printf("SendMsg 0x%x\n",MSG_UI_CEC_DECK_CONTROL_STOP);
						//SendUIMsg(MSG_UI_CEC_DECK_CONTROL_STOP);
						ClearIdleTimeOut();
					break;
					case SP5K_CEC_PLAY_MODE_PLAY_STILL:
						PushKey(KEY_ACTIVE);PushKey( KEY_CEC_PAUSE );
						//printf("SendMsg 0x%x\n",MSG_UI_CEC_DECK_CONTROL_STOP);
						//SendUIMsg(MSG_UI_CEC_DECK_CONTROL_STOP);
						ClearIdleTimeOut();
					break;

					default:
						appCecHandle_Send_FeatureAbort(SP5K_CEC_MSG_PLAY,iniAddr);
					break;
				}
			}
		break;

		case SP5K_CEC_MSG_DECK_CONTROL: 	//0x42
			if(!IsCECOpen)	return;
			if( operandNum != 1)	break;
			if( (iniAddr!=SP5K_CEC_LOGI_ADDR_BROADCAST) && (flwAddr==pbDev) )
			{
				printf("---SP5K_CEC_MSG_DECK_CONTROL://0x42---\n");
				switch( operandTbl[0] )
				{
					case SP5K_CEC_DECK_CTRL_STOP:
						PushKey(KEY_ACTIVE);PushKey( KEY_CEC_STOP );
						//printf("SendMsg 0x%x\n",MSG_UI_CEC_DECK_CONTROL_STOP);
						//SendUIMsg(MSG_UI_CEC_DECK_CONTROL_STOP);
						ClearIdleTimeOut();
					break;

					default:
						appCecHandle_Send_FeatureAbort(SP5K_CEC_MSG_DECK_CONTROL,iniAddr);
					break;
				}
			}
		break;

		case SP5K_CEC_MSG_ROUTING_CHANGE: //0x80
			if( (iniAddr!=SP5K_CEC_LOGI_ADDR_BROADCAST) /*&& (ucRcvDestAddr==byLogAddr)*/ )
			{
				printf("---SP5K_CEC_MSG_ROUTING_CHANGE://0x80---\n");
				printf("*** Rcv <Routing Change> *** 0...byPhysAddr[0]=0x%x, byPhysAddr[1]=0x%x\n",txOperandTbl[0],txOperandTbl[1]);
				if( (operandTbl[2]!=txOperandTbl[0]) || (operandTbl[3]!=txOperandTbl[1]) )
				{
					//if( IsCecPollingMessage ) KillTimer2CecPolling();
					printf("SendMsg_2 0x%x\n",MSG_UI_CEC_RETURN_STANDBY);
					SendUIMsg(MSG_UI_CEC_RETURN_STANDBY);
					//gbyIsDSCActive = FALSE;
				}
			#ifdef ENABLE_ONE_TOUCH_PLAY
				if( (operandTbl[2]==txOperandTbl[0]) && (operandTbl[3]==txOperandTbl[1]) )
				{
					if( !gbyIsDoOneTouchByHdmiIn && !gbyIsOneTouchPlay )
					{
						printf("*** Rcv <Routing Change> ***  1...\n");
						appCecHandleOneTouchPlay();
					}
					SendUIMsg(MSG_UI_CEC_FORCE_TO_PLAYBACK);
				}
			#endif

			}
		break;
	#endif
	}
	profLogPrintf(0, "receiveMsgCallback	E	opcode=0x%x",opcode);
}

#endif



void appCecControlStateOnMsg(
	UINT32 msg,
	UINT param
)
{
	DBG_PRINT("%s : [0x%x] [0x%x]\n", __FUNCTION__, msg, param);

	UINT32 videoFmtNum;
	UINT8 *pvideoFmt, i;
	BOOL flag = TRUE;

	switch (msg)
	{
		case SP5K_MSG_CEC_PHYSICAL_ADDR_DISCOVER_FAIL:
			RPrintf("CEC_PHYSICAL_ADDR_DISCOVER_FAIL\n");
			resolution = DISP_1080I_16X9_60HZ;
		//	appStateChange(appPreviousStateGet(), STATE_PARAM_HDMI_PLUG);
			break;
		case SP5K_MSG_CEC_PHYSICAL_ADDR_DISCOVER_DONE:
			RPrintf("CEC_PHYSICAL_ADDR_DISCOVER_SUCCESSFULL\n");
			sp5kCecControl(0, SP5K_CEC_CTRL_VIDEO_FMT_GET, &videoFmtNum, &pvideoFmt);
			printf("videoFmtNum = %d \n", videoFmtNum);
			#if 1
			for(i=0; i<videoFmtNum; ++i){
				printf("videoFmtNum = %d, pvideoFmt= %d\n", videoFmtNum, *(pvideoFmt + i));
			}
			#endif
			for(i=0; i<videoFmtNum; ++i){
				if (*(pvideoFmt + i) == DISP_1080I_16X9_60HZ){
					resolution = DISP_1080I_16X9_60HZ;
					flag = FALSE;
					printf("%s, %d, DISP_1080I_16X9_60HZ \n", __FUNCTION__, __LINE__);
					break;
				}
			}
			for(i=0; i<videoFmtNum; ++i){
				if (*(pvideoFmt + i) == DISP_1080I_16X9_50HZ && flag){
					resolution = DISP_1080I_16X9_50HZ;
					flag = FALSE;
					printf("%s, %d, DISP_1080I_16X9_50HZ \n", __FUNCTION__, __LINE__);
					break;
				}
			}
			for(i=0; i<videoFmtNum; ++i){
				if (*(pvideoFmt + i) == DISP_720P_16X9_60HZ && flag){
					resolution = DISP_720P_16X9_60HZ;
					flag = FALSE;
					printf("%s, %d, DISP_720P_16X9_60HZ \n", __FUNCTION__, __LINE__);
					break;
				}
			}
			for(i=0; i<videoFmtNum; ++i){
				if (*(pvideoFmt + i) == DISP_720P_16X9_50HZ && flag){
					resolution = DISP_720P_16X9_50HZ;
					flag = FALSE;
					printf("%s, %d, DISP_720P_16X9_50HZ \n", __FUNCTION__, __LINE__);
					break;
				}
			}
			for(i=0; i<videoFmtNum; ++i){
				if (*(pvideoFmt + i) == DISP_576P_4X3_50HZ && flag){
					resolution = DISP_576P_4X3_50HZ;
					flag = FALSE;
					printf("%s, %d, DISP_576P_4X3_50HZ \n", __FUNCTION__, __LINE__);
					break;
				}
			}
			for(i=0; i<videoFmtNum; ++i){
				if (*(pvideoFmt + i) == DISP_576I_4X3_50HZ && flag){
					resolution = DISP_576I_4X3_50HZ;
					flag = FALSE;
					printf("%s, %d, DISP_576I_4X3_50HZ \n", __FUNCTION__, __LINE__);
					break;
				}
			}
			for(i=0; i<videoFmtNum; ++i){
				if (*(pvideoFmt + i) == DISP_480P_4X3_60HZ && flag){
					resolution = DISP_480P_4X3_60HZ;
					flag = FALSE;
					printf("%s, %d, DISP_480P_4X3_60HZ \n", __FUNCTION__, __LINE__);
					break;
				}
			}
			for(i=0; i<videoFmtNum; ++i){
				if (*(pvideoFmt + i) == DISP_480I_4X3_60HZ && flag){
					resolution = DISP_480I_4X3_60HZ;
					flag = FALSE;
					printf("%s, %d, DISP_480I_4X3_60HZ \n", __FUNCTION__, __LINE__);
					break;
				}
			}
			if(resolution == 0xff){
				resolution = DISP_1080I_16X9_60HZ;
				printf(">>>>>>>>>>>>>>> EIA-CEA resolution ID is not Standard  <<<<<<<<<<<<<<<\n");
			}
			printf(">>>>>>>>>>>>>>> %s, %d, Get HDMI Resolution: %x  <<<<<<<<<<<<<<<\n", __FUNCTION__, __LINE__, resolution);
			break;
		default :
			break;
	}
}


void appCecControlStateInit(
	void
)
{
	sp5kCecInstCfg_t cecCfg;
	cecCfg.ddcChnlId = 0;
	cecCfg.cecChnlId = 0;
	#if IS_CEC_ON
	cecCfg.rxMsgCallback = (sp5kCecRecieveMsgCallback_t)receiveMsgCallback;
	#else
	cecCfg.rxMsgCallback = NULL;
	#endif
	sp5kCecControl(0, SP5K_CEC_CTRL_OPEN, &cecCfg);
	sp5kCecControl(0, SP5K_CEC_CTRL_DBG_MSG_LOG_ENABLE_SET, 1);
	sp5kCecControl(0, SP5K_CEC_CTRL_START);		/* log start */
}


void
appCecCapabilityCtrlState(
	UINT32 msg,
	UINT32 param
)
{
	DBG_PRINT("%s : [0x%x] [0x%x]\n",__FUNCTION__,msg,param);

	switch (msg) {
	case APP_STATE_MSG_INIT:
		RPrintf("appCecControlState");
		appCecControlStateInit();
		break;
	case APP_STATE_MSG_CLOSE:
		appStateCloseDone();
		break;
	default:
		appCecControlStateOnMsg(msg,param);
		if (resolution != 0xff){
			if(APP_STATE_PB_AUDIO == appPreviousStateGet()){
				previousStateGet = APP_STATE_PB_MAIN;
			}else{

				previousStateGet = appPreviousStateGet();
				#if 1
				switch(previousStateGet)
				{
						case APP_STATE_SPORTDV_VIDEO_REC:
						case APP_STATE_SPORTDV_VIDEO_REC_SLOW:
							previousStateGet=APP_STATE_SPORTDV_VIDEO_PREVIEW;	
							break;
						case APP_STATE_CARDV_VIDEO_REC:
						case APP_STATE_CARDV_VIDEO_REC_LAPSE:
							previousStateGet=APP_STATE_CARDV_VIDEO_PREVIEW;
							break;
						case APP_STATE_POWER_ON:
							previousStateGet=_powerOnStateGet();	//ui para 
							break;
						#if 0	
						default:  
							if(appWiFiConnection_UtilityStateGet() > WIFI_UTILITY_CLOSE)
							{
								previousStateGet=APP_STATE_WIFI_CONNECTION;
							}
							break;
						#endif
						
				}			
				#endif
				
			}
			

		uiPara_t* puiPara = appUiParaGet();//ll@add
		if(previousStateGet == 0x501)
                     {

                            previousStateGet = puiPara-> ReturnSetting;//ll@add

                      }

			printf(">>>>>>>>>>>>>>> appCecCapabilityCtrlState: Return state is  %x  after running HDMI plug <<<<<<<<<<<<<<<\n", previousStateGet);
			appStateChange(APP_STATE_HDMI_PLUG, STATE_PARAM_NORMAL_INIT);
		}
		break;
	}

}

