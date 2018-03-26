/**************************************************************************
 *                                                                        *
 *         Copyright (c) 2010 by DXG Technology Co., Ltd.             *
 *                                                                        *
 *  This software is copyrighted by and is the property of DXG        *
 *  Technology Co., Ltd. All rights are reserved by DXG Technology    *
 *  Co., Ltd. This software may only be used in accordance with the       *
 *  corresponding license agreement. Any unauthorized use, duplication,   *
 *  distribution, or disclosure of this software is expressly forbidden.  *
 *                                                                        *
 *  This Copyright notice MUST not be removed or modified without prior   *
 *  written consent of DXG Technology Co., Ltd.                       *
 *                                                                        *
 *  DXG Technology Co., Ltd. reserves the right to modify this        *
 *  software without notice.                                              *
 *                                                                        *
 *  DXG Technology Co., Ltd.                                          *
 *  MIN ZHI IND., PARK,					            *
 *  LONG HUA,BAOAN,SHEN ZHEN,P.R.O.C.                                           *
 *                                                                        *
 *  Author:                                              *
 *                                                                         *
 **************************************************************************/
 
//#include "../../sphost/customization/hostfw/include/app_buzzer.h"
//#include "app_osd_api_lcm.h"
//#include "app_osd_draw_lcm.h"

//#include "middleware/global_def.h"

#if 0
#include "../../sphost/customization/hostfw/include/App_timer.h"
#include "../../sphost/customization/hostfw/cathyware/inc/net/Ndk_types.h"


#include "../../sphost/customization/hostfw/include/app_ui_para.h"
#include "../../sphost/customization/hostfw/include/app_key_def.h"
#include "../../sphost/customization/hostfw/include/App_state_ctrl.h"
#include "../../sphost/customization/hostfw/include/App_scdv_ui_para_def.h"
#endif
//#include "../../sphost/customization/hostfw/include/App_ui_para_dvs5m2.h"
#include "common.h"
#include "api/cmd.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "api/sp5k_utility_api.h"
#include "api/sp5k_global_api.h"
#include "api/sp5k_os_api.h"
#include "api/sp5k_capture_api.h"
#include "api/sp5k_fs_api.h"
#include "api/sp5k_dbg_api.h"
#include "api/sp5k_disk_api.h"
#include "api/sp5k_gfx_api.h"
#include "api/sp5k_media_api.h"
#include "api/sp5k_modesw_api.h"
#include "../../sphost/include/rf/app_rf_def.h"
#include "../../sphost/customization/hostfw/include/app_timer.h"
#include "../../sphost/customization/hostfw/include/app_scdv_global.h"
#include "../../sphost/customization/hostfw/include/App_scdv_ui_para_def.h"

UINT8 aviFile[64];
int nLapseNoFrameAction=0;
UINT8  nSPDVSwitchResolution=FALSE;
UINT8  nHDMIVideoRes=UI_SCDV_VIDEO_SIZE_1080P30;
UINT8  RFSnapMovieMutex = 0;
//typedef struct uiPara_s uiPara_t;
//typedef  UINT8  UINT8;
//typedef  UINT16  UINT16;	
//typedef  UINT32  UINT32;	
//extern uiPara_t* appUiParaGet(void);
#define APP_UI_MSG_RF_RCW5K8_REC				0xFB090001		
#define APP_UI_MSG_RF_RCW5K8_SNAP			0xFB090002
#define APP_UI_MSG_RF_RCWD01_REC_START		0xFB090003
#define APP_UI_MSG_RF_RCWD01_REC_STOP		0xFB090004
#define APP_UI_MSG_RF_RCWD01_SNAP			0xFB090005
	
#define RFMODE_SPI  SP5K_SPI_CLK_IDLE_LOW_LATCH_RISE   //0x0
#define RF_FERQUENCY_SPI  SP5K_GSI_CLK_DIVISOR_128

#define RF2401_RF_RX_SCAN_RATE 	10 ///1ms
// CS must be controlled by host
#define RFHW_POWER   	(1<<2)
#define RFHW_CE    		(1<<4)  		//gpio4  gen
#define RFHW_IRQ      	(1<<13)   	//gpio13 
#define RFHW_CSN  		(1<<0)  		//LMI0  LMIGPIO0

#define RFHW_CLK  		1<<(48-32)    //FMGPIO48
#define RFHW_MISO   		1<<(47-32)    //FMGPIO47  FMH
#define RFHW_MOSI	 	1<<(49-32)    //FMGPIO49  FMH

#define CSN(x);    sp5kGpioWrite(SP5K_GPIO_GRP_LMI,RFHW_CSN,x<<0);
#define CSN(x);    sp5kGpioWrite(SP5K_GPIO_GRP_LMI,RFHW_CSN,x<<0);


#define RF2401_SPI_ID		(1)
#define RF2401_SPI_DFS 	(0x7)

#define RF2401_SPI_CSN_GPIO	

// nRF2401 RegMap  define
#define NRF_RF_CONFIG			(0x00)
#define NRF_EN_AA				(0x01)
#define NRF_EN_RXADDR			(0x02)
#define NRF_SETUP_AW			(0x03)
#define NRF_SETUP_RETR			(0x04)
#define NRF_RF_CH				(0x05)
#define NRF_RF_SETUP			(0x06)
#define NRF_RF_STATUS			(0x07)
#define NRF_OBSERVE_TX			(0x08)
#define NRF_RF_PRD				(0x09)
#define NRF_RX_ADDR_P0			(0x0A)
#define NRF_RX_ADDR_P1			(0x0B)
#define NRF_RX_ADDR_P2			(0x0C)
#define NRF_RX_ADDR_P3			(0x0D)
#define NRF_RX_ADDR_P4			(0x0E)
#define NRF_RX_ADDR_P5			(0x0F)
#define NRF_TX_ADDR				(0x10)
#define NRF_RX_PW_P0			(0x11)
#define NRF_RX_PW_P1			(0x12)
#define NRF_RX_PW_P2			(0x13)
#define NRF_RX_PW_P3			(0x14)
#define NRF_RX_PW_P4			(0x15)
#define NRF_RX_PW_P5			(0x16)
#define NRF_FIFO_STATUS			(0x17)
#define NRF_DYNPD				(0x1C)
#define NRF_FEATURE				(0x1D)


// 24L01plus Register Map 
#define CONFIG_MAP 			0x00
#define EN_AA_MAP 			0x01
#define EN_RXADDR_MAP 		0x02
#define SETUP_AW_MAP		0x03
#define SETUP_RETR_MAP 	0x04
#define RF_CH_MAP 			0x05
#define RF_SETUP_MAP 		0x06
#define STATUS_MAP 			0x07
#define OBSERVE_TX_MAP 	0x08
#define RPD_MAP 			0x09
#define RX_ADDR_P0_MAP 	0x0A
#define RX_ADDR_P1_MAP 	0x0B
#define RX_ADDR_P2_MAP 	0x0C
#define RX_ADDR_P3_MAP 	0x0D
#define RX_ADDR_P4_MAP 	0x0E
#define RX_ADDR_P5_MAP 	0x0F
#define TX_ADDR_MAP 		0x10
#define RX_PW_P0_MAP 		0x11
#define FIFO_STATUS_MAP 	0x17
#define DYNPD_MAP 			0x1C
#define FEATURE_MAP 		0x1D

// Nordic Command
#define NRF_R_REGISTER					0x00
#define NRF_W_REGISTER					0x20
#define NRF_R_RX_PAYLOAD				0x61
#define NRF_W_TX_PAYLOAD				0xA0
#define NRF_FLUSH_TX					0xE1
#define NRF_FLUSH_RX					0xE2
#define NRF_REUSE_TX_PL				0xE3
#define NRF_ACTIVATE					0x50
#define NRF_R_RX_PL_WID				0x60
#define NRF_W_ACK_PAYLOAD				0xA8
#define NRF_W_TX_PAYLOAD_NO_ACK		0xB0
#define NOP         							0xFF  
//----------------------------------
#define RF_TRANSMIT_MODE			(0x5e) 	//01011110b
#define RF_RECEIVE_MODE			(0x3f)	//00111111b
#define RF_CHIP_POWER_OFF			(0x7c)	//01111100b
#define RECEIVE_PAYLOAD			8
#define TEST_TX_PAYLOAD			32
#define RF_PIPE_0					0x00
 
#define CUSTOMER_ADDR0	(0x08)	//CUID code
#define CUSTOMER_ADDR1	(0x08)
#define CUSTOMER_ADDR2	(0x08)
#define CUSTOMER_ADDR3	(0x09) 	//week
#define CUSTOMER_ADDR4	(0x0b) 	//year
	
 //pair mode addr
#define PARN_ADDR0			(0x96)
#define PARN_ADDR1			(0x69)
#define PARN_ADDR2			(0x96)
#define PARN_ADDR3			(0x69)
#define PARN_ADDR4			(0x5A)

 

//return( un access) code ,pair fail 
#define  PARN_FAIL_CODE0	(0xFA)  
#define  PARN_FAIL_CODE1	(0xFA)  
#define  PARN_FAIL_CODE2	(0xFA)  
#define  PARN_FAIL_CODE3	(0xFA)  //week
#define  PARN_FAIL_CODE4	(0xFA)  //year

#define MJPEG_LEN 	(128 * 96 * 3)

//Camera Current  Status 
enum {
	CAMST_IDLE=0, 
	CAMST_PRV, CAMST_REC, CAMST_CAP, CAMST_STOP, 
	CAMST_WIFI_PRV,  CAMST_WIFI_REC,CAMST_WIFI_CAP
};

//Camera Error Code
enum{
	CAMERR_OK=0,  CAMERR_BUSY,
	CAMERR_CARD_FULL, CAMERR_BATLOW, CAMERR_TIMEOUT,	
};


#define RF2401_RF_TASK_EN   0  		// RF2401_RF_RX_IRQ sube 1,     0: use Timer polling.
#define FCT_RC_PAYLOAD_CB
#define RF2401_RF_RX_IRQ	0   		//0: using polling mode , 1: as interrupt mode.

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
enum {MD_RX=0,  MD_TX};
enum {GPIOSPI=0,HWSPI=1,};
static UINT8  nRFMode=0;				//0: as RX / 1: as TX mode
static UINT8 *pRPLdata=NULL;
static UINT8 	nRFSpiMode=HWSPI;		//0: SPI H/W interface, 1: use Gpio function.
static UINT8 	nRFSpiShiftByteLen=7;		//7: work bfs = 8bits
static UINT8 nRFParingMode=0;

static UINT8 nRFRegCtx[32]={0};
static UINT8 gRF24Init=0;

//nRF_Par_Ch_tab
static UINT8 nRFParCHTab[2]={1,79};  //paring 2401Mhz or 2479Mhz

#define  	FREQ_HOP_GRP  0x0// 0x01  0x02 0x03
#define 	FREQ_HOP  1

#if 1
static UINT8 nRFFreqHopTab[4][4]={

	#ifdef FREQ_HOP
		{ 3 , 41, 77, 41},
		{ 41, 3, 77, 3},
		{ 77 , 41, 77, 3},
		{ 3 , 77, 3, 41},
	#else
		{ 41 , 41, 41, 41},
		{ 41, 41,41, 41},
		{ 41 , 41, 41, 41},
		{ 41 , 41, 41, 41},
	#endif
};
#else
static UINT8 nRFFreqHopTab[4][4]={

	#if  1//def FREQ_HOP
		{ 3 , 3, 3, 3},
		{ 3, 3, 3, 3},
		{ 3 , 3, 3, 3},
		{ 3 , 3, 3, 3},
	#elif 0
		{ 90 , 90, 90, 90},
		{ 90, 90,90, 90},
		{ 90 , 90, 90, 90},
		{ 90 , 90, 90, 90},
	#elif 0
		{ 125 , 125, 125, 125},
		{ 125, 125,125, 125},
		{ 125 , 125, 125, 125},
		{ 125 , 125, 125, 125},
	#else
		{ 41 , 41, 41, 41},
		{ 41, 41,41, 41},
		{ 41 , 41, 41, 41},
		{ 41 , 41, 41, 41},	
	#endif
};
#endif



#define SPI_READ_BUF_SIZE			(32)
#define HW_SPI_DELAY  1
enum {START_RECORD=0x01,STOP_RECORD=0x05,SNAP=0x02,REQUEST_SATUS=0xFC };


//UINT32 RFTimerHand = TIMER_NULL;  
static TX_THREAD *pThreadRFSend = NULL;
static TX_THREAD *pThreadRFReceive = NULL;
static TX_THREAD *pEncodeThread = NULL;
static TX_THREAD *pAppUpdateBufferThread = NULL;

UINT8 PingPongCH= 1;
enum  {RCW5K8=0,RCWD01=1};
UINT8 g_nRcwMID=RCWD01;

int printf(const char *format, ...);
 void RFCELOW();
 void RFCEHIGH();
 void spi_delay_task(UINT16 n);
 //extern UINT8* appUiCUIDParacGet(void);
UINT8 ReadPLData[32] = {0};
//UINT8 *ReadPLData = NULL;

//UINT32 wt_config_cuid_num = 0x015B38;
UINT32 nPairRandomCUID = 0x015B38;
struct tDxgCuid{UINT8 Year;UINT8 Week;}  DateCUID={28,17};
//UINT32 nPairRandomCUID = 0x010101;
//struct tDxgCuid{UINT8 Year;UINT8 Week;}  DateCUID={0x01,0x01};

static UINT8 CamStatus=0;
static UINT8 CamErrorCode=0;
static UINT16 CamFileNum=0;
UINT8 ReadPLDataBack[32];
UINT8  nPairAddrEnable=0;
TX_EVENT_FLAGS_GROUP  RF_ISR;
TX_MUTEX   MutexSPI;
/////////////////////HW SPI Driver Begin///////////////
static void HWSPIInit()
{
		//RF_POWER
		sp5kGpioFuncSet(SP5K_GPIO_GRP_GEN,RFHW_POWER,1<<2);
		sp5kGpioCfgSet(SP5K_GPIO_GRP_GEN,RFHW_POWER,1<<2);		
		sp5kGpioPullSet(SP5K_GPIO_GRP_GEN,RFHW_POWER,1<<2);
		sp5kGpioWrite(SP5K_GPIO_GRP_GEN,RFHW_POWER,1<<2);//init 0 or 1
		spi_delay_task(1000);

		//CE
		sp5kGpioCfgSet(SP5K_GPIO_GRP_GEN,RFHW_CE,1<<4);
		sp5kGpioPullSet(SP5K_GPIO_GRP_GEN,RFHW_CE,1<<4);
		sp5kGpioWrite(SP5K_GPIO_GRP_GEN,RFHW_CE,0<<4);//init 0 or 1

		//IRQ
		sp5kGpioCfgSet(SP5K_GPIO_GRP_GEN,RFHW_IRQ,0<<13);
		sp5kGpioPullSet(SP5K_GPIO_GRP_GEN,RFHW_IRQ,1<<13);
		sp5kGpioWrite(SP5K_GPIO_GRP_GEN,RFHW_IRQ,1<<13);//init 0 or 1

		//CSN
		sp5kGpioCfgSet(SP5K_GPIO_GRP_LMI,RFHW_CSN,1<<0);
		sp5kGpioPullSet(SP5K_GPIO_GRP_LMI,RFHW_CSN,1<<0);
		sp5kGpioWrite(SP5K_GPIO_GRP_LMI,RFHW_CSN,1<<0);//init 0 or 1

		
		
}

void gRFDriverInit()
{
	sp5kSscCfgSet(SP5K_SSC_PORT_3, SP5K_SSC_CFG_MODE, SP5K_SSC_MODE_SPI);
	sp5kSscCfgSet(SP5K_SSC_PORT_3, SP5K_SSC_CFG_PIN, SP5K_SSC_CFG_SSC1_PIN1 );
	sp5kSscCfgSet(SP5K_SSC_PORT_3, SP5K_GSI_CFG_PHY, (RFMODE_SPI << 16 | RF_FERQUENCY_SPI << 8));

}
void appSpiWriteData(UINT8 *pData, UINT8 len)
{
	//gRFDriverInit();
	UINT16 nBitCount=0;
	nBitCount = len*8;
	CSN(0);
	sp5kSscCfgSet(SP5K_SSC_PORT_3, SP5K_SSC_LOCK, 1);
	sp5kSpiWrite(SP5K_SSC_PORT_3, pData, nBitCount);//bitcnt
	sp5kSscCfgSet(SP5K_SSC_PORT_3, SP5K_SSC_LOCK, 0);
	CSN(1);
}

//write reg and value for 1bytes  command word
void appSpiWriteCmd(UINT8 nCmdName, UINT8 nCmdWord)
{
	//gRFDriverInit();
	UINT8 nBitCount=0;
	UINT8 nCmdBuffer[2];
	nCmdBuffer[0]=nCmdName|NRF_W_REGISTER;
	nCmdBuffer[1]=nCmdWord;
	nBitCount = 2*8;
	CSN(0);
	sp5kSscCfgSet(SP5K_SSC_PORT_3, SP5K_SSC_LOCK, 1);
	sp5kSpiWrite(SP5K_SSC_PORT_3, nCmdBuffer, nBitCount);//bitcnt
	sp5kSscCfgSet(SP5K_SSC_PORT_3, SP5K_SSC_LOCK, 0);
	CSN(1);
}

UINT8 appSpiReadCmd(UINT8 reg,UINT8 nlen)
{
	//gRFDriverInit();
	
	CSN(0);
	sp5kSscCfgSet(SP5K_SSC_PORT_3, SP5K_SSC_LOCK, 1);
	sp5kSpiWrite(SP5K_SSC_PORT_3, &reg, 8);//bitcnt		
	sp5kSpiRead(SP5K_SSC_PORT_3, ReadPLData, 8*nlen);
	sp5kSscCfgSet(SP5K_SSC_PORT_3, SP5K_SSC_LOCK, 0);
	CSN(1);
	return ReadPLData[0];
}
//p0 addr 5 byte lsb byte c
void appSpiFlushTxFifo()
{
	UINT8 nBitCount=0;
	UINT8 nCmdBuffer[2];
	nCmdBuffer[0]=NRF_FLUSH_TX;
	nCmdBuffer[1]=0;		
	appSpiWriteData(nCmdBuffer, 1);
	
}
void appSpiFlushRxFifo()
{
	UINT8 nBitCount=0;
	UINT8 nCmdBuffer[2];
	nCmdBuffer[0]=NRF_FLUSH_RX;
	nCmdBuffer[1]=0;		
	appSpiWriteData(nCmdBuffer, 1);

}

void appSpiWriteBurstData(UINT8 *pData, UINT8 len)
{
	UINT8 nCmdBuffer[33],i=0;
	nCmdBuffer[0]=NRF_W_TX_PAYLOAD;
	for(i=0;i<len;i++)
		nCmdBuffer[i+1]=pData[i];
	//memcpy(nCmdBuffer+1,pData,len);
	appSpiWriteData(nCmdBuffer, len+1);
}
void appSpiReadBurstData(UINT8 nPipe)
{

	UINT8 len=0;
	len=appSpiReadCmd(NRF_RX_PW_P0|nPipe,1);//p0=p5
	appSpiReadCmd(NRF_R_RX_PAYLOAD,len);
	
}

void appSpiWriteACKPayload(UINT8 nPipe,UINT8 *pData, UINT8 len)
{
	UINT8 nCmdBuffer[33],i=0;
	nCmdBuffer[0]=NRF_W_ACK_PAYLOAD|nPipe;//0-5
	for(i=0;i<len;i++)
		nCmdBuffer[i+1]=pData[i];
	//memcpy(nCmdBuffer+1,pData,len);
	appSpiWriteData(nCmdBuffer, len+1);

}

void TestHWSpi()
{
	UINT8  i=0;
	UINT8 Buf[6]={
		NRF_RX_ADDR_P0|NRF_W_REGISTER,  //byte0
		0x90,
		0x90,
		0x90,
		0x90,
		0x88
		};
	//printf("TestHWSpi\n");
	
	HWSPIInit();
	gRFDriverInit();
	
	RFCELOW();
	appSpiWriteData(Buf,6);
	Buf[0]=NRF_TX_ADDR|NRF_W_REGISTER;
	appSpiWriteData(Buf,6);

	appSpiReadCmd(NRF_TX_ADDR,5);
	//for(i=0;i<5;i++)
	//printf("TX Addr=0x%x 0x%x 0x%x 0x%x 0x%x\n ",ReadPLData[0],ReadPLData[1],ReadPLData[2],ReadPLData[3],ReadPLData[4]);
	ReadPLData[0]=0x77;
	appSpiReadCmd(NRF_RX_ADDR_P0,5);
	//for(i=0;i<5;i++)
	//printf("RX Addr=0x%x 0x%x 0x%x 0x%x 0x%x\n ",ReadPLData[0],ReadPLData[1],ReadPLData[2],ReadPLData[3],ReadPLData[4]);
	

	appSpiWriteCmd(NRF_RF_SETUP,0x06);
	appSpiReadCmd(NRF_RF_SETUP,1);
	//printf("setup =%d\n",ReadPLData[0]);		
	appSpiReadCmd(STATUS_MAP,1);
	//printf("status =%d\n",ReadPLData[0]);	
		
	RFCEHIGH();
	//extern UINT32 previewCallbackTest(void);
	//previewCallbackTest();
	

}

// lock embedded SPI engine, no storage access allowed after this
// note that SP5K_SSC_LOCK is costy so host should use larger granularity
//sp5kSscCfgSet(SP5K_SSC_PORT_2, SP5K_SSC_LOCK, 1);
//sp5kSpiWrite(APP_SPI_PORT|SP5K_PORT_MASK_DMA_MODE, data, byteCnt*8); // note that sp5kSpiWrite use bit count	
// release embedded SPI engine, storage access is OK now
//sp5kSscCfgSet(SP5K_SSC_PORT_2, SP5K_SSC_LOCK, 0);

/////////////////////	HW SPI Driver End		///////////////


void spi_delay_task(UINT16 n)
{	
	UINT16 i=0;
	#ifdef HW_SPI_DELAY
		for(i=0; i<n;i++)
		sp5kTimeDelay(SP5K_TIME_DELAY_1US ,1);
	#else	 
			i=1000; 
			while(i--);   
	#endif
}
 

//===================================================================
void nRF2401_MemoryInit(void)
{	

	//printf("nRF2401 mem init...\n");
	if (pRPLdata == NULL)
		pRPLdata = sp5kMalloc(SPI_READ_BUF_SIZE * 2)	;				
	//printf("PayLoad Heap buffer error!!!\n");
	
}

#define RF_POWER   1<<2
#define RF_CE    (1<<4)  		//gpio4  gen
#define RF_CSN  (1<<13)  		//GPIO13    AUDCLKO/E11

#define RF_CLK  (1<<0)   		//LMI0  LMIGPIO0
#define RF_MISO   1<<(49-32)   //FMGPIO49  FMH
#define RF_MOSI	 1<<(48-32)    //FMGPIO48
#define RF_IRQ      1<<(47-32)   //FMGPIO47 

static void GpioSPIInit(UINT8  en)
{
	if(en)
	{
		//RF_POWER
		sp5kGpioFuncSet(SP5K_GPIO_GRP_GEN,RF_POWER,1<<2);
		sp5kGpioCfgSet(SP5K_GPIO_GRP_GEN,RF_POWER,1<<2);		
		sp5kGpioPullSet(SP5K_GPIO_GRP_GEN,RF_POWER,1<<2);
		sp5kGpioWrite(SP5K_GPIO_GRP_GEN,RF_POWER,1<<2);//init 0 or 1
		spi_delay_task(1000);
		//CE
		sp5kGpioCfgSet(SP5K_GPIO_GRP_GEN,RF_CE,1<<4);
		sp5kGpioPullSet(SP5K_GPIO_GRP_GEN,RF_CE,1<<4);
		sp5kGpioWrite(SP5K_GPIO_GRP_GEN,RF_CE,0<<4);//init 0 or 1

		//CSN
		sp5kGpioCfgSet(SP5K_GPIO_GRP_GEN,RF_CSN,1<<13);
		sp5kGpioPullSet(SP5K_GPIO_GRP_GEN,RF_CSN,1<<13);
		sp5kGpioWrite(SP5K_GPIO_GRP_GEN,RF_CSN,1<<13);//init 0 or 1

		//CLK
		sp5kGpioCfgSet(SP5K_GPIO_GRP_LMI,RF_CLK,1<<0);
		sp5kGpioPullSet(SP5K_GPIO_GRP_LMI,RF_CLK,1<<0);
		sp5kGpioWrite(SP5K_GPIO_GRP_LMI,RF_CLK,1<<0);
		
					
		//MISO
		sp5kGpioCfgSet(SP5K_GPIO_GRP_FMH,RF_MISO,0<<(49-32));
		sp5kGpioPullSet(SP5K_GPIO_GRP_FMH,RF_MISO,1<<(49-32));
		sp5kGpioWrite(SP5K_GPIO_GRP_FMH,RF_MISO,1<<(49-32));

		//MOSI
		sp5kGpioCfgSet(SP5K_GPIO_GRP_FMH,RF_MOSI,1<<(48-32));
		sp5kGpioPullSet(SP5K_GPIO_GRP_FMH,RF_MOSI,1<<(48-32));
		sp5kGpioWrite(SP5K_GPIO_GRP_FMH,RF_MOSI,1<<(48-32));
	
		//IRQ
		sp5kGpioCfgSet(SP5K_GPIO_GRP_FMH,RF_IRQ,0<<(47-32));
		sp5kGpioPullSet(SP5K_GPIO_GRP_FMH,RF_IRQ,1<<(47-32));
					
	}
	else
	{
		//gpio_config(GPIO_RF2401_CSN, GPIO_FUNC_HW);
		//gpio_config(GPIO_RF2401_CLK, GPIO_FUNC_HW);
		//gpio_config(GPIO_RF2401_MOSI, GPIO_FUNC_HW);
		//gpio_config(GPIO_RF2401_MISO, GPIO_FUNC_HW);		
	}
	
}
static void vGpioRF_Init()
{
		//MISO
		sp5kGpioCfgSet(SP5K_GPIO_GRP_FMH,RF_MISO,1<<(49-32));
		sp5kGpioPullSet(SP5K_GPIO_GRP_FMH,RF_MISO,1<<(49-32));
		sp5kGpioWrite(SP5K_GPIO_GRP_FMH,RF_MISO,1<<(49-32));

		//MOSI
		sp5kGpioCfgSet(SP5K_GPIO_GRP_FMH,RF_MOSI,1<<(48-32));
		sp5kGpioPullSet(SP5K_GPIO_GRP_FMH,RF_MOSI,1<<(48-32));
		sp5kGpioWrite(SP5K_GPIO_GRP_FMH,RF_MOSI,0<<(48-32));
	
		//IRQ
		sp5kGpioCfgSet(SP5K_GPIO_GRP_FMH,RF_IRQ,0<<(47-32));
		sp5kGpioPullSet(SP5K_GPIO_GRP_FMH,RF_IRQ,1<(47-32));



}
//RFCEHIGH
 void RFCEHIGH()
{
		
	sp5kGpioWrite(SP5K_GPIO_GRP_GEN,RF_CE,1<<4);
	//printf("--ce high\n");
	
}
//RFCELOW
 void RFCELOW()
{
			
	sp5kGpioWrite(SP5K_GPIO_GRP_GEN,RF_CE,0<<4);
	//printf("--ce low\n");
	
}

static void vGpioRF_CsnSetHigh()
{
	sp5kGpioWrite(SP5K_GPIO_GRP_GEN,RF_CSN,1<<13);
	//printf("--csn high\n");

}
static void vGpioRF_CsnSetLow()
{
	sp5kGpioWrite(SP5K_GPIO_GRP_GEN,RF_CSN,0<<13);
	//printf("--csn low\n");

}

static void vGpioRF_sckSetHigh()
{
	sp5kGpioWrite(SP5K_GPIO_GRP_LMI,RF_CLK,1<<0);
	//printf("--clk high\n");

}
static void vGpioRF_sckSetLow()
{
	sp5kGpioWrite(SP5K_GPIO_GRP_LMI,RF_CLK,0<<0);
	//printf("--clk low\n");

}

static void vGpioRF_MosiSetHigh()
{
	sp5kGpioWrite(SP5K_GPIO_GRP_FMH,RF_MOSI,1<<(48-32));
	//printf("--mosi high\n");

}
static void vGpioRF_MosiSetLow()
{
	sp5kGpioWrite(SP5K_GPIO_GRP_FMH,RF_MOSI,0<<(48-32));
	//printf("--mosi low\n");

}
static void GpioSpiCSN(UINT8 nPinLevel)
{
	if(nPinLevel){		
		sp5kGpioWrite(SP5K_GPIO_GRP_GEN,RF_CSN,1<<13);
	}else{		
		sp5kGpioWrite(SP5K_GPIO_GRP_GEN,RF_CSN,0<<13);
	}
	//printf("--csn=%d\n",lv);
}


static void GpioSpiCLK(UINT8 nPinLevel)
{
	if(nPinLevel){
		sp5kGpioWrite(SP5K_GPIO_GRP_LMI,RF_CLK,1<<0);
	}else{
		sp5kGpioWrite(SP5K_GPIO_GRP_LMI,RF_CLK,0<<0);
	}
}

static void GpioSpiMOSI(UINT8 nPinLevel)
{
	if(nPinLevel){
		sp5kGpioWrite(SP5K_GPIO_GRP_FMH,RF_MOSI,1<<(48-32));
	}else{
		sp5kGpioWrite(SP5K_GPIO_GRP_FMH,RF_MOSI,0<<(48-32));
	}
}

 
static UINT32 GpioSpiMISO(void)
{
	UINT32 nPinLevel=0;	
	sp5kGpioRead(SP5K_GPIO_GRP_FMH, RF_MISO, &nPinLevel);
	if(nPinLevel) nPinLevel=1;
	else nPinLevel=0;
	//printf("miso=%d\n",lv);
	return(nPinLevel);
}


static UINT32 GetRF_IRQLevel(void)
{
	UINT32 nPinLevel=0;	
	if(nRFSpiMode==GPIOSPI)
		sp5kGpioRead(SP5K_GPIO_GRP_FMH, RF_IRQ, &nPinLevel);
	else
		sp5kGpioRead(SP5K_GPIO_GRP_GEN, RFHW_IRQ, &nPinLevel);
	
	if(nPinLevel) nPinLevel=1;
	else nPinLevel=0;
	//printf("irq=%d\n",lv);
	return(nPinLevel);
}

static void GpioSpiDLY(UINT16 dly)
{
	int i,j;
	for(i=0;i<dly;i++){
		for(j=0;j<10;j++);
	}
}


static void GpioSpiWrite(UINT16 id, UINT8 *ptr, UINT8 len)
{
	int btn;
	int i;
	UINT8 dat;
	UINT8 dat_in;
	
	GpioSpiCLK(0);
	GpioSpiCSN(0);
	GpioSpiDLY(10);
	for(i=0;i<len;i++){
		dat = ptr[i];
		dat_in=0;
		for(btn=0;btn<=nRFSpiShiftByteLen;btn++){
			GpioSpiMOSI(dat&0x80);
			dat<<=1;
			GpioSpiDLY(1);
			GpioSpiCLK(1);
			dat_in<<=1;
			dat_in |=  (GpioSpiMISO())?(0x01):(0x00);			
			GpioSpiDLY(1);
			GpioSpiCLK(0);
		}
		ReadPLData[i]=dat_in;
	}

	GpioSpiCLK(0);
	GpioSpiCSN(1);
}

static void RFWriteDat(UINT8 cmd,UINT8 *ptr, UINT8 len)
{
	int i;
	static UINT8 Buf[34];
	
	sp5kOsMutexGet(&MutexSPI, 0);
	Buf[0]= cmd ;//0x1234;

	//if(len>32) 
		//len=31;
	for(i=0;i<len;i++)
	{
		Buf[i+1]=ptr[i];
	}

	if(nRFSpiMode==GPIOSPI) 
	{
		GpioSpiWrite(RF2401_SPI_ID,Buf,len+1);
	}	
	else	
	{	      
		appSpiWriteData(Buf,len+1);   
	}
	sp5kOsMutexPut(&MutexSPI);
	
	

}
static void RFWriteCmd(UINT8 adr,UINT8 Data)
{

	nRFRegCtx[adr]=(UINT8)Data;
	adr=adr |NRF_W_REGISTER;

	RFWriteDat(adr,&Data,1);
}

static void RFClearIrq(void)
{
	RFWriteCmd(NRF_RF_STATUS,0x70);  		// Enable ACK Pipe-0
	spi_delay_task(2);
}

static void RFTxFlushFIFO(void)
{
	RFWriteDat(NRF_FLUSH_TX,NULL,0);     
}

static void RFRxFlushFIFO(void)
{
	RFWriteDat(NRF_FLUSH_RX,NULL,0);            
}
static void RFSetup(UINT8 dat)
{      
	RFWriteCmd(NRF_RF_SETUP,dat);
}

#define RFADDREN	(5)

static void RFSetTxAddr(UINT8 *adr, UINT8 updAdr)
{
	int i;
	UINT8 Buf[RFADDREN]={		 
		0x90,
		0x90,
		0x90,
		0x90,
		0x5A
		};
	
	if(updAdr)
	{		
		for(i=0;i<5;i++)
		{
			Buf[i] = adr[i];
		}
	}
	RFWriteDat(NRF_TX_ADDR|NRF_W_REGISTER,Buf,RFADDREN);
}       

//p0 addr 5 byte lsb byte first  
static void RFSetRxAddr(UINT8 *adr, UINT8 updAdr)
{
	int i;	
	UINT8 Buf[RFADDREN]={		 
		0x90,
		0x90,
		0x90,
		0x90,
		0x5A
		};
	if(updAdr){		
		for(i=0;i<5;i++){
			Buf[i] = adr[i];
		}
	}
	RFWriteDat(NRF_RX_ADDR_P0|NRF_W_REGISTER,Buf,RFADDREN);
       
}


//Function: Write Read command. write 1 byte then receiver len of requestion RX

static UINT8 RFReadWriteCmd(UINT8 adr,  UINT8 len)//p0 6
{
	UINT8 Buf[32];
	int i;
	
	Buf[0]= adr ;
	
	if(nRFSpiMode==GPIOSPI) 
	{
		GpioSpiWrite(RF2401_SPI_ID,Buf,len+1);
		for(i=0;i<len;i++)
		{
			pRPLdata[i]=  ReadPLData[i+1]; //rdata init malloc heap memory
		}
	}
	else
	{				
			appSpiReadCmd(adr,len);
			for(i=0;i<len;i++)
			{
				pRPLdata[i]=  ReadPLData[i]; //rdata init malloc heap memory
			}
	}
	
	//return ((nRFSpiMode==GPIOSPI)?(pRPLdata[1]):(pRPLdata[0])); 
	return pRPLdata[0];
}


//  prepare nRF2401 TX and RX Adress( CUID) .
//  NO CUID nPairRandomCUID = 0 
// Notes:   Adrress need rev argment

static void RFSetTXRXAddr(void)
{
		
		UINT8 Buf[5];
				
		int i;
		i=0;

		if(nRFParingMode)
		{
			
			Buf[i++]= PARN_ADDR0;//0x96  lsb
			Buf[i++]= PARN_ADDR1;//0x69
			Buf[i++]= PARN_ADDR2;//0x96
			Buf[i++]= PARN_ADDR3;//0x69
			Buf[i++]= PARN_ADDR4;//0x5a
			
			//printf("set paring mode Adr....\n");
			
		}
		else if(nPairRandomCUID>=1|| DateCUID.Week>=1||DateCUID.Year>=1 )
		{
			Buf[i++]= DateCUID.Year;
			Buf[i++]= DateCUID.Week;
			Buf[i++]= (UINT8)(((nPairRandomCUID>>16)&0x00ffL)  );
			Buf[i++]= (UINT8)((nPairRandomCUID>>8)&0x00ffL);
			Buf[i++]= (UINT8)(nPairRandomCUID&0x00ffL);
			//printf("CUID= %d\n",nPairRandomCUID);
		}
		else
		{
			//if NO CUID, default ID value  A9876
			Buf[i++]= CUSTOMER_ADDR0;
			Buf[i++]= CUSTOMER_ADDR1;
			Buf[i++]= CUSTOMER_ADDR2;
			Buf[i++]= CUSTOMER_ADDR3;
			Buf[i++]= CUSTOMER_ADDR4;
			//printf("Warning!! NO CUID= %d\n",nPairRandomCUID);
		}
		
		spi_delay_task(1);
		RFSetTxAddr(Buf,1);
		spi_delay_task(1);
		RFReadWriteCmd( NRF_TX_ADDR,5);
		//printf("NRF_TX_ADDR11 =0x%x 0x%x 0x%x 0x%x 0x%x\n",pRPLdata[0],pRPLdata[1],pRPLdata[2],pRPLdata[3],pRPLdata[4]);
		spi_delay_task(1);		
		RFSetRxAddr(Buf,1);		
		spi_delay_task(1);
		RFReadWriteCmd( NRF_RX_ADDR_P0,5);
		//printf("NRF_RX_ADDR_P01 =0x%x 0x%x 0x%x 0x%x 0x%x\n",pRPLdata[0],pRPLdata[1],pRPLdata[2],pRPLdata[3],pRPLdata[4]);
		//spi_delay_task(1);		
		//RFReadWriteCmd( NRF_RF_SETUP,1);
		//printf("NRF_RF_SETUP1=0x%x \n",pRPLdata[1]);
		//printf("nRF2401 TX RX Adr ==0x%x 0x%x 0x%x 0x%x 0x%x\n",Buf[0],Buf[1],Buf[2],Buf[3],Buf[4]);

}

//Set TX RX Frequency (Channel) From 2401~2483Mhz

static void RFChgTXRXCh(UINT8 nChIdx)
{
	UINT8 nCh;

	if(nRFParingMode)
	{
		nCh = nRFParCHTab[nChIdx&0x01];	
		
	}
	else
	{
		nCh = nRFFreqHopTab[FREQ_HOP_GRP&0x03][nChIdx&0x03];		
	}
	RFCELOW();
	RFWriteCmd(NRF_RF_CH,nCh);
	RFCEHIGH();			
}

static void RFPairChgTXRXCh(UINT8 nChIdx)
{
	UINT8 nCh;

	if(nRFParingMode)
	{
		nCh = nRFParCHTab[nChIdx&0x01];
		RFCELOW();
		RFWriteCmd(NRF_RF_CH,nCh);
		RFCEHIGH();
	}
	else
	{
		nCh = nRFFreqHopTab[FREQ_HOP_GRP&0x03][nChIdx&0x03];
		RFCELOW();
		RFWriteCmd(NRF_RF_CH,nCh);
		RFCEHIGH();
	}		
	//printf("Change Ch=%d\n",nCh);
}

//vdxgapp_nRF2401_Init
void RF2401Init(void)
{
    
	
	if(nRFSpiMode==GPIOSPI)	
	{
		GpioSPIInit(1);
	}	
	else
	{
		HWSPIInit();
		gRFDriverInit();
	}
#if 1
	RFCELOW();	
	spi_delay_task(1);
	RFClearIrq();
	spi_delay_task(1);
	RFTxFlushFIFO();
	spi_delay_task(1);
	RFRxFlushFIFO();
	spi_delay_task(1);
	//0 rx 1 tx 
	if(nRFMode ==MD_TX)
	{
		
		RFWriteCmd(NRF_RF_CONFIG,RF_TRANSMIT_MODE);
		spi_delay_task(1);
		RFWriteCmd(NRF_EN_AA,0x01);  		// Enable ACK Pipe-0
		RFWriteCmd(NRF_EN_RXADDR,0x01);   	// Enable Receive Pipe-0
		RFWriteCmd(NRF_SETUP_AW,0x03);   	// Address:5-bytes
		RFWriteCmd(NRF_SETUP_RETR,0x25);   //Auto-Retransmit x 5@@750uS
		RFWriteCmd(NRF_RF_CH,1);			//CH1
		RFWriteCmd(NRF_RF_SETUP,0x27);   	// 250Kbps
		RFWriteCmd(NRF_DYNPD,0x01);   		//Enable dynamic payload Pipe-0
		RFWriteCmd(NRF_FEATURE,0x06);   	//EN_DPL:ON, EN_ACK_PAY:ON
	}
	else
	{
		
		RFWriteCmd(NRF_RF_CONFIG,0x0F);//0x0F
		spi_delay_task(1);
		RFWriteCmd(NRF_EN_AA,0x01);  		// Enable ACK Pipe-0
		//RFWriteCmd(NRF_EN_AA,0x3F);
		RFWriteCmd(NRF_EN_RXADDR,0x00);   	// Enable Receive Pipe-0  diable
		//RFWriteCmd(NRF_EN_RXADDR,0x3F);   // Enable Receive Pipe-0
		RFWriteCmd(NRF_SETUP_AW,0x03);   	// Address:5-bytes
		RFWriteCmd(NRF_SETUP_RETR, 3<<4 |15);   	//Auto-Retransmit x 15@@2ms		
		RFWriteCmd(NRF_RF_CH, nRFFreqHopTab[FREQ_HOP_GRP&0x03][0]); 
		//RFWriteCmd(NRF_RF_SETUP,0x27);   	// 250Kbps
		RFWriteCmd(NRF_DYNPD,0x01);   		//Enable dynamic payload Pipe-0  When EN_AA EN_DPL 
		//RFWriteCmd(NRF_DYNPD,0x3F);
		RFWriteCmd(NRF_FEATURE,0x06);   	//EN_DPL:ON, EN_ACK_PAY:ON	
		
	}		

		RFSetTXRXAddr();
		
		if(nRFMode !=MD_TX)
		{			
			if(g_nRcwMID==RCW5K8)
				RFSetup(0x27);   	// 250Kbps
			else if(g_nRcwMID==RCWD01)
				RFSetup(0x0C);
		}


		
		RFCEHIGH();
	#endif
	//printf("--rfinit end\n");
}


//vdxgapp_nRF2401_Init
void RFSRTInit(void)
{
    
	
	if(nRFSpiMode==GPIOSPI)	
	{
		GpioSPIInit(1);
	}	
	else
	{
		HWSPIInit();
		gRFDriverInit();
	}

	RFCELOW();	
	spi_delay_task(1);
	RFClearIrq();
	spi_delay_task(1);
	RFTxFlushFIFO();
	spi_delay_task(1);
	RFRxFlushFIFO();
	spi_delay_task(1);
	//0 rx 1 tx 
	if(nRFMode ==MD_TX)
	{
		
		RFWriteCmd(NRF_RF_CONFIG,0x0e); 
		spi_delay_task(1);
		//RFWriteCmd(NRF_EN_AA,0x01);  		// Enable ACK Pipe-0
		RFWriteCmd(NRF_EN_AA,0x00);
		RFWriteCmd(NRF_EN_RXADDR,0x01);   	// Enable Receive Pipe-0
		RFWriteCmd(NRF_SETUP_AW,0x03);   	// Address:5-bytes
		RFWriteCmd(NRF_SETUP_RETR,0x00);   //Auto-Retransmit x 5@@750uS 3.75ms 0x3F
		RFWriteCmd(NRF_RF_CH,3);			//CH0		
		RFWriteCmd(NRF_DYNPD,0x01);   		//Enable dynamic payload Pipe-0
		RFWriteCmd(NRF_FEATURE,0x06);   	//EN_DPL:ON, EN_ACK_PAY:ON
		RFWriteCmd(NRF_RX_PW_P0,8); 
	}
	else
	{
		//CH 0  63  125
		RFWriteCmd(NRF_RF_CONFIG,0x0F);
		spi_delay_task(1);
		//RFWriteCmd(NRF_EN_AA,0x01);  		// Enable ACK Pipe-0
		RFWriteCmd(NRF_EN_AA,0x00);
		RFWriteCmd(NRF_EN_RXADDR,0x01);   	// Enable Receive Pipe-0		
		RFWriteCmd(NRF_SETUP_AW,0x03);   	// Address:5-bytes
		RFWriteCmd(NRF_SETUP_RETR, 3<<4 |15);   	//Auto-Retransmit x1 5@@1000uS		
		RFWriteCmd(NRF_RF_CH,0); 		
		RFWriteCmd(NRF_DYNPD,0x01);   		//Enable dynamic payload Pipe-0  When EN_AA EN_DPL 		
		RFWriteCmd(NRF_FEATURE,0x06);   	//EN_DPL:ON, EN_ACK_PAY:ON	
		RFWriteCmd(NRF_RX_PW_P0,8); 
	}		

		RFSetTXRXAddr();										
		RFSetup(0x0F); ///2mbps-0db								
		RFCEHIGH();

}

void PariRemoterInit()
{
	if(nRFSpiMode==GPIOSPI)	
	{
		GpioSPIInit(1);
	}	
	else
	{
		HWSPIInit();
		gRFDriverInit();
	}

}

// RF_Chip_Rx_Burst_Data 
static int RFRxBurstData(void)
{
	
	UINT8 len;
	UINT8 i,ret;

	ret=0;
	len=RFReadWriteCmd( NRF_R_RX_PL_WID,1);

	//printf("lenxx=%d\n",len);	
	if(len>32) len=32;
	if(len)
	{
		RFReadWriteCmd( NRF_R_RX_PAYLOAD,len);
		if(nRFSpiMode==GPIOSPI) 
		{
			for(i=0;i<len;i++)
			{			
				ReadPLDataBack[i]=  ReadPLData[i+1];				
			}
		}
		else
		{
				for(i=0;i<len;i++)
				{			
					ReadPLDataBack[i]=  ReadPLData[i];				
				}
		}			
		ret=len;
	}
	return(ret);
}



static void RFTxBurstDataReSend(void)
{
	UINT8 Buf[5];
	int i=0;			
	Buf[i++]= 0x12;
	Buf[i++]= 0x34;
	Buf[i++]= 0x56;
	Buf[i++]= 0x78;
	Buf[i++]= 0x9A;

	RFWriteDat(NRF_W_TX_PAYLOAD,Buf,i );  //error  NRF_W_REGISTER
}

static void RFTxBurstDataCustom(UINT8 *p,UINT8 len)
{
	RFWriteDat(NRF_W_TX_PAYLOAD,p,len );
}

static void RFReSendCode(void)
{

	RFClearIrq();
	spi_delay_task(1);
	RFTxFlushFIFO();
	spi_delay_task(1);
	RFTxBurstDataReSend();
	spi_delay_task(2);		
	RFCEHIGH();  //10us high 
	spi_delay_task(60);	
	RFCELOW();
	//printf("nRF2401 Send code...\n");
}


// Check paring  Cmd Word  by wrist watch send 
static int RFParingComp(UINT8 len)
{
	
	UINT8 i;
	
	if(!nRFParingMode) 
		return(0);
	
	if(len!=5) 
	{
		//printf("paring length [%d]!=5bytes\n",len);
		return(0);
	}

	//printf("--Pair ID=");
	for(i=0;i<len;i++)
		printf("0x%x ",ReadPLDataBack[i]);
	printf("\n");
	
	///1F AA 55 AA 55 B  D01  AF AA 55 AA 55
	if( (ReadPLDataBack[2]==0x55) && (ReadPLDataBack[1]==0xAA) && (ReadPLDataBack[0]==0x1f) )
	{		
		g_nRcwMID=RCW5K8;
		//printf("5p8 Pair Success!!!\n");
		return(1);
	}
	else if( (ReadPLDataBack[2]==0x55) && (ReadPLDataBack[1]==0xAA) && (ReadPLDataBack[0]==0xaf) )
	{		
		//g_nRcwMID=RCWD01;
		g_nRcwMID=RCW5K8;
		//printf("D01 Pair Success!!!\n");
		return(1);
	}
	
	printf("Pair Fail !!!\n");
	return(0);
}


//  prepare camera ID( CUID) for pairing Mode ACK-payload

void RFParingID(void)
{
	//if(nRFParingMode)
	{     
	
		UINT8 Buf[8];		
		UINT8 i;
		i=0;		
		if(nPairRandomCUID>=1 || DateCUID.Week>=1||DateCUID.Year>=1)
		{		
			
			Buf[i++]= (UINT8)(nPairRandomCUID&0x00ffL);
			Buf[i++]= (UINT8)((nPairRandomCUID>>8)&0x00ffL);
			Buf[i++]= (UINT8)(((nPairRandomCUID>>16)&0x00ffL)  );
			Buf[i++]= DateCUID.Week;
			Buf[i++]= DateCUID.Year;									
		}
		else
		{

			//NO CUID, default ID value
			Buf[i++]= CUSTOMER_ADDR4;  //0X0A  9 8 7 6 
			Buf[i++]= CUSTOMER_ADDR3;   
			Buf[i++]= CUSTOMER_ADDR2;
			Buf[i++]= CUSTOMER_ADDR1;
			Buf[i++]= CUSTOMER_ADDR0;		
		}

              Buf[i++] = 0xAA; //pair package head 
		RFWriteDat(NRF_W_ACK_PAYLOAD|RF_PIPE_0,Buf,i );	//0a 09 08 07 06				
		//printf("nRF2401 ACK CUID =0x%x 0x%x 0x%x 0x%x 0x%x\n",Buf[0],Buf[1],Buf[2],Buf[3],Buf[4]);

	}

}
void RF24ParingFail(void)
{
	if(nRFParingMode)
	{     
		
		UINT8 Buf[5];	
		int i=0;
     		//printf("Worng CUID code!\n");
     		
		Buf[i++] = PARN_FAIL_CODE0;
		Buf[i++] = PARN_FAIL_CODE1;
		Buf[i++] = PARN_FAIL_CODE2;
		Buf[i++] = PARN_FAIL_CODE3;
		Buf[i++] = PARN_FAIL_CODE4;		
		RFWriteDat(NRF_W_ACK_PAYLOAD|RF_PIPE_0,Buf,i );

		//printf("nRF2401 ACK Rply-payLoad =0x%x 0x%x 0x%x 0x%x 0x%x\n",Buf[0],Buf[1],Buf[2],Buf[3],Buf[4]);

	}

}


void  RFSetCameraFileNum(UINT16 nFileNum)
{
	//printf(">>>RF2401, Set  fileNum=%d\n",nFileNum);
	CamFileNum = nFileNum;
}
void RFSetCameraStatus(UINT8 nSts)
{
	//printf(">>>RF2401, Set  CamStatus=%d\n",nSts);
	CamStatus= nSts;
}

void RFSetCameraError(UINT8 nError)
{
	//printf(">>>RF2401, Set  CamError=%d\n",nError);
	CamErrorCode= nError;
}

extern UINT8 nrf_5k8_if_get_record_state(void);
extern UINT32 appActiveStateGet (void);

int CameraStatusAckRply(void)
{
	
	static UINT8 Buf[8];
	UINT16 nChkSum;
	static UINT8 tmp=0;
	UINT8 i=0,j=0;
			
    	RFTxFlushFIFO();
	Buf[i++]= (UINT16)(CamFileNum&0xff);  //total number of  DCIM file
	Buf[i++]= (UINT16)((CamFileNum>>8)&0xff);  
	//if(ReadPLDataBack[1]==0xfc && RFSnapMovieMutex ==1)
	//{
		//Buf[i++]=CAMST_REC;
		//RFSnapMovieMutex=0;
	//}	
	 if(nrf_5k8_if_get_record_state()==TRUE /*||appPreviousStateGet()!=0x210*/ 
	 	/*ReadPLDataBack[1]==0xfc ||ReadPLDataBack[1]==0x1*/ ) 
		Buf[i++]=CAMST_REC;
	else
		Buf[i++]=CAMST_IDLE;	
	
	Buf[i++]= CamErrorCode; 
	nChkSum=0;	
	for(j=0;j<i;j++)
	{
		nChkSum+=Buf[j];
	}
	Buf[i++] = (UINT8)nChkSum;
    
	RFWriteDat(NRF_W_ACK_PAYLOAD|RF_PIPE_0,Buf,i );
	
	//printf("5M2 Status Upload >>>  0x%x 0x%x 0x%x 0x%x chks=0x%x , len=%d\n",Buf[0],Buf[1],Buf[2],Buf[3],nChkSum,i);
    
     return(0);
}

//extern UINT8  hal_nrf_get_clear_irq_flags(void)
static int RFMonitorRxIrq(void)
{
	UINT32 nIrqTriger=1;
	int ret,len;
		
	nIrqTriger=GetRF_IRQLevel();
	ret=0;
	//printf("uRF2401_GpioIRQ=%d\n",nIrqTriger);
	if(nPairAddrEnable==1)
	{
		//RFCELOW();
		RFSetTXRXAddr();
		//RFCEHIGH();
		nPairAddrEnable=0;
	}
	
	if(nIrqTriger==0)
	{
		len=RFRxBurstData();
		ret=len;
		//hal_nrf_get_clear_irq_flags();
		RFRxFlushFIFO();
		if(nRFParingMode)
		{
			if(RFParingComp(len))
			{	
	             spi_delay_task(4);
	             nRFParingMode = 0;								
	             RFSetTXRXAddr();						   
				 RFParingID();							
				 //printf("--nRFParingMode=%d\n",nRFParingMode);												
			}
			else
			{
			      	RF24ParingFail(); 
			}
		}
			
	    RFClearIrq();		
        if(!nRFParingMode)
		{
            spi_delay_task(4);
			CameraStatusAckRply();
        }				
	} 
	else if(nRFParingMode)
	{       
			//paring mode, pre put paring ID to nRF2401 buffer
			RFTxFlushFIFO();
	       	RFParingID();
	        	//printf("set Paring ID\n");
	}       
   return(ret);
}
static int RF5k8MonitorRxIrq(void)
{
	UINT32 nIrqTriger=1;
	int ret,len;
		
	nIrqTriger=GetRF_IRQLevel();
	ret=0;
	//printf("uRF2401_GpioIRQ=%d\n",nIrqTriger);
		
	if(nIrqTriger==0)
	{
		len=RFRxBurstData();
		ret=len;
		RFRxFlushFIFO();					
	     	RFClearIrq();		
            	if(!nRFParingMode)
		{
                   	spi_delay_task(4);
			CameraStatusAckRply();
            	}				
	} 
	
   return(ret);
}

void RF24IRQCallBack(void)
{
	
	
	RFMonitorRxIrq();
}



//0x1d cmd rollkey ver  
void RFRxMsgFilter(UINT8 *uCmdStr,UINT8 nLen)
{        
    UINT8 nkeyID,nKeyCode,nVer;
    static int KeyRollMark=(-1);   
    UINT32  curAppMode = 0;
    UINT8* puiPara = NULL; 
	     
    nVer = uCmdStr[3];//newest 0x12  old ver is 0
    nkeyID =(uCmdStr[0]&0x1f); //0x1d   
    nKeyCode =uCmdStr[1];  //keycode  
   //CamStatus = CAMST_IDLE;   
   //CamErrorCode=CAMERR_OK;
   
   //printf("Key code =0x%x 0x%x 0x%x Ver=%d\n", uCmdStr[0],uCmdStr[1],uCmdStr[2],uCmdStr[3]);   
      	  
	    if(nkeyID==0x1d)
	    {
	        if(KeyRollMark !=uCmdStr[2]) //roll key
		 {
		            KeyRollMark =(unsigned int)uCmdStr[2];
		            if(nKeyCode==START_RECORD)
				{
		                         			          										
					sp5kHostMsgSend(APP_UI_MSG_RF_RCW5K8_REC,0); //normal , rec slow					
					//printf("---rec1\n");
		            }
			     else if(nKeyCode==STOP_RECORD)
			     {
					sp5kHostMsgSend(APP_UI_MSG_RF_RCW5K8_REC,0);
					//printf("---rec2\n");
			     }	 
			     else if(nKeyCode==SNAP)
			     {		                		              
			        sp5kHostMsgSend(APP_UI_MSG_RF_RCW5K8_SNAP,0);
					//printf("--snap3\n");	 
		            }	  
			    else if(nKeyCode==REQUEST_SATUS)
			     {
		            
		            }           
	        }
		else
		{
	        
	        }
	       
	    }
   
}

void  RFRxTimerCallBack(ULONG nEID)
{
	UINT8 len=0,i=0;
	static UINT8 nRFHopCnt=0;
	static UINT8 nRxLostCnt=0;


	//while(1)
	{

		//sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 100);
		
		len=RFMonitorRxIrq();		
		if(len)
		{
			
			if(!nRFParingMode)
			{                								
				RFRxMsgFilter(&ReadPLDataBack[0],len);
			}
			else
			{
				//Parining Action		
			}		               
			nRxLostCnt=0;
		}

		if(++nRxLostCnt>=15){
			nRxLostCnt=0;
			nRFHopCnt++;
			RFChgTXRXCh(nRFHopCnt);
		}

	}
	//RFReSendCode();
}

void  RFRx5K8TimerCallBack(ULONG nEID)
{
	UINT8 len=0,i=0;
	static UINT8 nRFHopCnt=0;
	static UINT8 nRxLostCnt=0;

	//while(1)
	{

		sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 100);
		
		len=RF5k8MonitorRxIrq();		
		if(len)
		{
			
			if(!nRFParingMode)
			{                								
				RFRxMsgFilter(&ReadPLDataBack[0],len);
			}
			else
			{
				//Parining Action		
			}		               
			nRxLostCnt=0;
		}

		if(++nRxLostCnt>=15){
			nRxLostCnt=0;
			nRFHopCnt++;
			RFChgTXRXCh(nRFHopCnt);
		}

	}
	//RFReSendCode();
}


#define SP5K_TIMEOUT 0x00000007
UINT8 nPreviewSuspend=0;
#define NRF_STREAM_REC_TOO_BUSY
#define NRF_STREAM_ADV_MGR 
//#define STREAM_TIMEOUT_FIFO_FLUSH
typedef enum {
	  RF_IDLE,    		//Radio is idle
	  RF_MAX_RT,  	// Maximum number of retries have occured 
	  RF_TX_DS,   	// Data is sent 
	  RF_RX_DR,   	// Data recieved 
	  RF_TX_AP,   	// Ack payload recieved 
	  RF_BUSY     	// Radio is busy 
} radio_status_t;
typedef enum {
	    HAL_NRF_MAX_RT = 4,     	// Max retries interrupt 
	    HAL_NRF_TX_DS,          	// TX data sent interrupt 
	    HAL_NRF_RX_DR           	// RX data received interrupt 
} hal_nrf_irq_source_t;
radio_status_t  status;

#define 	BIT_0 	0x01 
#define 	BIT_1 	0x02 
#define 	BIT_2 	0x04 
#define 	BIT_3 	0x08 
#define 	BIT_4 	0x10 
#define 	BIT_5 	0x20 
#define 	BIT_6 	0x40 
#define 	BIT_7 	0x80 

typedef struct app_nrf_pkt_stats_s {
	UINT32 pkt_drop; 						//packet drop
	UINT32 pkt_chkerr; 					//checksum error
	
	UINT32 pkt_recv;						//packet recv	
	UINT32 pkt_ack_pload_recv;			//packet recv	
	UINT32 pkt_send;						//packet send
	

	UINT32 pkt_recv_data_rate;
	UINT32 pkt_ack_pload_recv_data_rate;
	UINT32 pkt_send_data_rate;	
	
	UINT32 pkt_total_ack_pload_recv;		//packet recv	
	UINT32 pkt_total_recv;					//packet recv	
	UINT32 pkt_total_send;				//packet send	

	UINT32 pkt_video_send;				//packet video send
	UINT32 pkt_FPS; 						//video frames per second
	
}app_nrf_pkt_stats_t;

app_nrf_pkt_stats_t app_nrf_pkt_stats = {0};
UINT8 nPairIDLen=0;
#define STREAM_FIFO_FLUSH_TIMEOUT 500      //0.1ms  1ms  5000
//#define STREAM_CHANGE_CH_TIMEOUT 500
#define NRF_RESET_TIMEOUT 		500      		//0.1ms  1ms  5000

#define BCU_HEADER_LEN 			2

#define NRF_DATA_PALOAD_LEN 	30 	//(RF_PAYLOAD_LENGTH-BCU_HEADER_LEN)

typedef enum NRF_BCU_State_{
	BCU_NORMAL = 0,
	BCU_RESCUE_RESPOND,
	BCU_RESCUE_REQUEST,
}EBCU_state;

#define NRF_BCU_STREAM_PACKET 0x00
#define NRF_BCU_CMD_PACKET 	0x01
#define NRF_BCU_CTRL_PACKET 	0x02
#define NRF_BCU_RESCUE_PACKET 0x03

//CMD pcode
#define NRF_BCU_CMD_PCODE_TIME_REQUEST  0x01	
#define NRF_BCU_CMD_PCODE_TIME_RESPOND  0x02 //Include Set Time

//STREAM CTRL pcode
#define NRF_BCU_CTRL_PCODE_STREAM_BUSY  		0x01	
#define NRF_BCU_CTRL_PCODE_STREAM_REQUEST  		0x02
#define NRF_BCU_CTRL_PCODE_STREAM_RESPOND  	0x03
#define NRF_BCU_CTRL_PCODE_STREAM_DUMMY_REQUEST  0x04
#define NRF_BCU_CTRL_PCODE_STREAM_DUMMY_RESPOND  0x05

//CTRL pcode
#define NRF_BCU_CTRL_PCODE_STATUS_REQUEST  	0x01	
#define NRF_BCU_CTRL_PCODE_STATUS_RESPOND  	0x02	

/* RF basic control unit */
typedef struct NRF_BCU_Header_t
{
	UINT8 type 		  :2; // 4 packet type	
	UINT8 hop  		  :2; // 4 hop (current not hop, use for slot check)
						 	// 0 = unknow (org flow)
							// 1 = slot 0,2
							// 2 = slot 1,4
							// 3 = slot 3,5
	UINT8 pcode 	 	  :4; // reserved
	UINT8 sync	  	  :8; // 0~255 - preamble	
}NRF_BCU_Header_s;

typedef union {
    UINT16 header;
    NRF_BCU_Header_s head;
} NRF_BCU_Header;

typedef struct NRF_Mjpeg_Header_t{
	UINT16 	type;
	UINT32 	crc;
	UINT32 	size;	
	UINT16 	cmd;
	UINT16 	frame_id;	
	UINT32 	mjpeg_crc;
}NRF_Mjpeg_Header_s;
//record_state
#define DEV_RECORD_STOP 0	
#define DEV_RECORD_START 1
//capture state
#define DEV_CAPTURE_IDLE 0	
#define DEV_CAPTURE_START 1
#define DEV_CAPTURE_START_CONFIRM 1
//error state
#define DEV_ERROR_NONE 0
#define DEV_ERROR_CARD_FULL 1
typedef struct STimeStamp_t{
	UINT8 	sec;		/*!<   Second */
	UINT8     min;		/*!<   Minute */
	UINT8     hour;	/*!<   Hour */
	UINT8     day;		/*!<   Day */
	UINT8     mon;		/*!<   Month */
	UINT8     year;		/*!<   Year */ //base on 2000
} STimeStamp_s;

typedef struct NRF_Watch_Control_Packet_t{ //30Byte
	UINT32 	crc;
	UINT8 	record_state;	
	UINT8 	capture_state;	
	UINT8 	error_state;
	STimeStamp_s datetime; //record  time 	
}NRF_Watch_Control_Packet_t;

//storage state
#define DEV_STORAGE_NAND 0
#define DEV_STORAGE_SDCARD 1	

typedef struct NRF_Cam_Status_Respond_Packet_t{ //30Byte
	UINT32  	crc;
	UINT8 	record_state;
	UINT8 	capture_state;	
	UINT8 	error_state;
	UINT8 	storage_type; 

	STimeStamp_s datetime;
}NRF_Cam_Status_Respond_Packet_s;


#define RF_PAYLOAD_LENGTH 32//126//32//10    
UINT8 bcu_sync=0;
UINT8 pkt_sync_data[256][RF_PAYLOAD_LENGTH];
static UINT8 pload_send[RF_PAYLOAD_LENGTH];
EBCU_state BCU_State = BCU_NORMAL;
static UINT8 bcu_rescue_sync=0;
static UINT8 bcu_slot=0;
static UINT16 mjpeg_frame_id=0;
static UINT16 last_mjpeg_frame_id=0;
UINT16 mjpeg_frame_send_cnt=0;

#define U16LE2CPU(a) (*(a) | (*((a) + 1) << 8))
#define CPU2U16LE(a, v) *(a) = (UINT8)((v) & 0xff);*(a+1) = (UINT8)((v) >> 8);

#define U32LE2CPU(a) (*(a) | (*((a) + 1) << 8) | (*((a) + 2) << 16) | (*((a) + 3) << 24))
#define CPU2U32LE(a, v) *(a) = (UINT8)((v) & 0xff);*(a+1) = (UINT8)(((v) >> 8) & 0xff ); *(a+2) = (UINT8)(((v) >> 16) & 0xff );*(a+3) = (UINT8)(((v) >> 24) & 0xff )

UINT32 nFrameStart=0;
UINT32 nFrameRead=0;
UINT32 nFrameWrite=0;


void radio_set_status (radio_status_t new_status)
{
  	status = new_status;
}
UINT8  hal_nrf_get_clear_irq_flags(void)
{
	UINT8 sr;
	RFReadWriteCmd( NRF_RF_STATUS,1);//NOP
	sr=(UINT8)ReadPLData[0];
	sr&= (BIT_6|BIT_5|BIT_4);
	RFWriteCmd(NRF_RF_STATUS,sr);
	//sr&= (BIT_6|BIT_5|BIT_4);
	return sr;
}

BOOL hal_nrf_rx_fifo_empty(void)
{

	UINT8 sr;
	
	RFReadWriteCmd( NRF_RF_STATUS,1);//NOP
	sr=(UINT8)ReadPLData[0];
	sr &= (BIT_1|BIT_2|BIT_3);
	//sr=sr>>1;
	sr>>=1;
	//printf("rec pipe num= %d\n",sr);
	 if(sr==7)
	  {
	    	return TRUE;
	  }
	  else
	  {
	    	return FALSE;
	  }
}
void nrf_request_new_frame(void);
void radio_irq(void)
{

		UINT16 length;
	
		nPairIDLen=0;	                                    				
		switch(hal_nrf_get_clear_irq_flags ())
		{
			
		  case (1<<HAL_NRF_MAX_RT):                 // Max retries reached
			    //RFTxFlushFIFO();
			    //RFRxFlushFIFO();
			    radio_set_status (RF_MAX_RT);			    	    	
			    break;
		  
		  case (1<<HAL_NRF_TX_DS):                  // Packet sent
			    radio_set_status (RF_TX_DS);
			    //printf("-- ps\n");	
			    break;
		  
		  case (1<<HAL_NRF_RX_DR):                  // Packet received
			  //printf("--pr\n");			  
			    if (!hal_nrf_rx_fifo_empty ())
				{					
				   length =  RFRxBurstData();				 
				   	nPairIDLen=length;
				   length = ((length<< 8) >> 8);
				   app_nrf_pkt_stats.pkt_recv += length;					
			    	}
			    	radio_set_status (RF_RX_DR);			  
			    break;
		  case ((1<<HAL_NRF_RX_DR)|(1<<HAL_NRF_TX_DS)): // Ack payload recieved					 	
			   //printf("--Ack\n");
			    if(!hal_nrf_rx_fifo_empty ())
				{
				      length =RFRxBurstData();
				      nPairIDLen=length;  
				      length = ((length<< 8) >> 8);
	  			      app_nrf_pkt_stats.pkt_ack_pload_recv += length;			
			    	}
			    	radio_set_status (RF_TX_AP);		          	
			    	break;

		  default:
		  	RFRxFlushFIFO(); 
		    	break;    
		}
			
}

UINT16  nrf_if_get_boss_record_busy(void)
{		
		return 0;
}

SINT32 app_nrf_sys_ticks(void)
{
	SINT32 ticks;	
	ticks = sp5kOsTimeGet();//10ms os clockTicks
	return ticks;
}
int app_mjpeg_fifo_clear(void)
{
	return 0;
}
radio_status_t radio_get_status (void)
{
	return status;
}

EBCU_state nrf_get_bcu_state(void)
{
	return BCU_State;
}

void nrf_set_bcu_state(EBCU_state state)
{
	BCU_State = state;
}

void nrf_package_bcu_header(UINT8 *pkt_data,UINT8 bcu_type,UINT8 hop,UINT8 pcode)
{

	NRF_BCU_Header BCUpkt;
	
	BCUpkt.head.type = bcu_type;
	BCUpkt.head.hop = hop;
	BCUpkt.head.pcode = pcode;
	BCUpkt.head.sync = bcu_sync;

	CPU2U16LE(pkt_data,BCUpkt.header);

	if(bcu_sync>=255)
	{
		bcu_sync=0;
	}
	else
	{
		bcu_sync++;
	}
}

void nrf_bcu_send_rescue_packet(void)
{	

	NRF_BCU_Header BCUpkt;
	
	if(pkt_sync_data[bcu_rescue_sync][1]!=bcu_rescue_sync)
	{		
		pkt_sync_data[bcu_rescue_sync][1] = bcu_rescue_sync;
	}
	
	RFWriteDat(NRF_W_ACK_PAYLOAD|RF_PIPE_0,pkt_sync_data[bcu_rescue_sync],RF_PAYLOAD_LENGTH );
	nrf_set_bcu_state(BCU_NORMAL);

	app_nrf_pkt_stats.pkt_send += RF_PAYLOAD_LENGTH;	
}

typedef enum 
{
	  FBUF_SEND,    		// Radio is idle 
	  FBUF_RESNED,    	// Radio is idle 
} nrf_buf_status_e;

typedef enum 
{
	NRF_FAIL_SEND=0,
	NRF_SENDING,
	NRF_STREAM_NOT_READY,   	
} nrf_send_status_t;

typedef enum 
{
	NRF_IDLE_MODE=0,
	NRF_STREAM_MODE,
	NRF_TEST_MODE,
} nrf_operation_mode_e;


typedef struct app_nrf_fbuf_stats_s 
{

	UINT8 *frame_buf_ptr;
	
	UINT8 ready;
	UINT8 buf_is_readout;

       UINT32 dataSize;
	UINT8 chunkSize;
	UINT16 chunkIndex;
	UINT16 chunkIndexSending;	
	UINT16 chunkSizeSending;	

	UINT8 headerSended;

}app_nrf_fbuf_stats_t; 
//-----------------------------------------------------------------------------------------------------------------------
// #Header#
//-----------------------------------------------------------------------------------------------------------------------
// [type] 	(u8)1byte	-	Packet indicator
// [len] 		(u32)4byte -Total Packet Length
// [chksum] (u16)2byte	-	Checksum
//-----------------------------------------------------------------------------------------------------------------------
#define NRF_HEADER_PLEN 7
// type
#define NRF_TP_UNKNOW_PACKET 	0x01
#define NRF_TP_CMD_PACKET 			0x02
#define NRF_TP_DATA_PACKET 		0x03
#define NRF_TP_VIDEO_PACKET 		0x04
#define NRF_TP_EVENT_PACKET 		0x05
#define NRF_TP_ACK_PACKET 			0x06
#define NRF_TP_STATUS_PACKET 		0x07
//-----------------------------------------------------------------------------------------------------------------------
// #CMD_PACKET#
// [cmd_id]	(u16)2byte	- 	Identifies the command:
// [param]   xxxx	-	Command parameters
//-----------------------------------------------------------------------------------------------------------------------
#define NRF_CMD_PING 	0x01	
#define NRF_CMD_MJPEG_HEADER 	0x02	
#define ABS(a) ((a)<0 ? -(a) :a)
// [system_tick]		(s32) 4byte	-	system tick
//-----------------------------------------------------------------------------------------------------------------------
/* max size in rick test is 4.7K byte */
#define LIVEVIEW_STREAM_FIFO_SIZE (1024*6)// (1024*10)// (1024*8)//(1024*10)
#define LIVEVIEW_STREAM_KEEP_REMAIN (1024*4)//(1024*5)//(1024*7) //(1024*5) //5K

struct FIFO_DEV                                     
{                                                                                
	  UINT32 current_len;    
	  UINT8 mem[LIVEVIEW_STREAM_FIFO_SIZE];        
	  TX_MUTEX sRFFIFOMutex;           
};

struct FIFO_DEV fdev;

#define NRF_CMD_PING_PLEN 6
typedef enum _ENRF_Interface_Storage_Type
{
	NRF_IF_STORAGE_NAND = 0,
	NRF_IF_STORAGE_SDCARD
}ENRF_IF_Storage_Type;
typedef enum _ENRF_Interface_Capture_State
{
	NRF_IF_CAPTURE_IDLE = 0,
	NRF_IF_CAPTURE_START,
	NRF_IF_CAPTURE_START_CONFIRM	
}ENRF_IF_Capture_State;

typedef enum _ENRF_Interface_Error_State
{
	NRF_IF_ERROR_NONE = 0,
	NRF_IF_ERROR_CARD_FULL,
}ENRF_IF_Error_State;

typedef enum _ENRF_Interface_Record_State
{
	NRF_IF_RECORD_STOP= 0,
	NRF_IF_RECORD_START,
}ENRF_IF_Record_State;

#define NRF_FRAME_BUF_SIZE 		(1024*25) // 25K
UINT8 nrf_frame_buffer[NRF_FRAME_BUF_SIZE];
app_nrf_fbuf_stats_t nrf_fbuf_stats ={0};

const UINT32 nrf_crc32_tab[] = {
        0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f,
        0xe963a535, 0x9e6495a3, 0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
        0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2,
        0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
        0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9,
        0xfa0f3d63, 0x8d080df5, 0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
        0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b, 0x35b5a8fa, 0x42b2986c,
        0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
        0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423,
        0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
        0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d, 0x76dc4190, 0x01db7106,
        0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
        0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d,
        0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
        0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,
        0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
        0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7,
        0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
        0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa,
        0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
        0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81,
        0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
        0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84,
        0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
        0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb,
        0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
        0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e,
        0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
        0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55,
        0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
        0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28,
        0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
        0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f,
        0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
        0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
        0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
        0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69,
        0x616bffd3, 0x166ccf45, 0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
        0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc,
        0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
        0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693,
        0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
        0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};

#define APP_STATE_SPORTDV_STILL_PREVIEW		0x0111

#define APP_STATE_SPORTDV_VIDEO_PREVIEW		0x0210
#define APP_STATE_SPORTDV_VIDEO_REC 			0x0211
#define APP_STATE_SPORTDV_VIDEO_REC_SLOW 	0x0212

#define APP_STATE_CARDV_VIDEO_PREVIEW 		0x0220
#define APP_STATE_CARDV_VIDEO_REC 			0x0221
#define APP_STATE_CARDV_VIDEO_REC_LAPSE 		0x0222

extern UINT32 appActiveDiskGet(void);
extern UINT8 appScdvDiskCheckMemoryFull(void);

UINT8 nrf_5k8_if_get_record_state(void)
{
	
	UINT8 ret=0;
    	switch(appActiveStateGet())
    	{									
		case APP_STATE_SPORTDV_VIDEO_REC:
		case APP_STATE_SPORTDV_VIDEO_REC_SLOW:
		case APP_STATE_CARDV_VIDEO_REC:
		case APP_STATE_CARDV_VIDEO_REC_LAPSE	:
			ret = 1;			
			break;		
		default:			
			 break;	
    	}
		
	return ret;

}
UINT8 State_Get_VideoRF_Status(UINT8 nType)
{
	UINT8 nRet=0;
	if(nType==3)
		nRet=0x0;
	else if(nType==2)
		nRet=0x0F;
	else if(nType==0)
		nRet=0x01;

	return nRet;
}

ENRF_IF_Storage_Type nrf_if_get_storage_type(void)
{

	UINT32 actDisk=appActiveDiskGet();
	ENRF_IF_Storage_Type ret=NRF_IF_STORAGE_NAND;

	if(actDisk==SP5K_DRIVE_NAND)
	{
		ret=NRF_IF_STORAGE_NAND;
	}
	else if(actDisk==SP5K_DRIVE_SD)
	{
		ret=NRF_IF_STORAGE_SDCARD;
	}
	//ret=NRF_IF_STORAGE_SDCARD;
	return ret;

}

ENRF_IF_Error_State nrf_if_get_error_state(void)
{
		
	ENRF_IF_Error_State  ret=NRF_IF_ERROR_NONE;
	if(appScdvDiskCheckMemoryFull()==TRUE)
	{
		ret=NRF_IF_ERROR_CARD_FULL;
	}

	//ret=NRF_IF_ERROR_NONE;
	return ret;
}

ENRF_IF_Capture_State nrf_if_get_capture_state(void)
{
	//TBC
	return NRF_IF_CAPTURE_IDLE;
}



ENRF_IF_Record_State nrf_if_get_boss_record_state(void){
	

	ENRF_IF_Record_State ret=NRF_IF_RECORD_STOP;

    	switch(appActiveStateGet())
    	{									
		case APP_STATE_SPORTDV_VIDEO_REC:
		case APP_STATE_SPORTDV_VIDEO_REC_SLOW:
		case APP_STATE_CARDV_VIDEO_REC:
		case APP_STATE_CARDV_VIDEO_REC_LAPSE	:
			ret = NRF_IF_RECORD_START;
			//printf("---video record start\n");
			break;		
		default:
			//printf("---video record stop\n");
			 break;	
    	}
	
	//ret=NRF_IF_RECORD_START;	
	return ret;

}

void nrf_bcu_send_stream_busy_packet(void)
{	

	
	// BCU Package
	nrf_package_bcu_header( pload_send , 
									NRF_BCU_STREAM_PACKET , 
									bcu_slot , 
									NRF_BCU_CTRL_PCODE_STREAM_BUSY );
	
	RFWriteDat(NRF_W_ACK_PAYLOAD|RF_PIPE_0,pload_send,BCU_HEADER_LEN );

	app_nrf_pkt_stats.pkt_send += BCU_HEADER_LEN;	
}

UINT32  nrf_crc32(const void *buf, unsigned int size)
{
        	const UINT8 *p;
        	UINT32  crc;

        	p = buf;
        	crc = ~0U;
	
        	while (size > 0) 
		{
                	crc = nrf_crc32_tab[(crc ^ *p++) & 0xff] ^ (crc >> 8);
			size--;
		}	
        	return crc ^ ~0U;
}


UINT8 dxg_fifo_open(UINT8* nPattern)
{
		//return fdev.current_len;
		return 0;
}

UINT8 dxg_fifo_close(void)
{
		sp5kOsMutexGet(&fdev.sRFFIFOMutex, 0);
		//memcpy(fdev->mem,0,LIVEVIEW_STREAM_FIFO_SIZE);
		//fdev.current_len=0;
		sp5kOsMutexPut(&fdev.sRFFIFOMutex);
		return 0;
}
int dxg_fifo_read( UINT8*ptr,UINT32 len,int nExtra)
{
	
		int ret;
		
		sp5kOsMutexGet(&fdev.sRFFIFOMutex, 0);
		if(fdev.current_len==0)
		{
			ret=0;
		}	
		else 
		{
			if(len>fdev.current_len)
				len=fdev.current_len;
			memcpy(ptr,fdev.mem,len);
			memcpy(fdev.mem,fdev.mem+len,fdev.current_len-len);
			fdev.current_len=fdev.current_len-len;
			ret=len;
		}	
		sp5kOsMutexPut(&fdev.sRFFIFOMutex);
		return ret;
}

//a whole frame image write fifo buffer 
int dxg_fifo_write( UINT8*ptr,UINT32 len,int nExtra)
{
		int ret=0;
		
		sp5kOsMutexGet(&fdev.sRFFIFOMutex, 0);
		if(fdev.current_len+len>LIVEVIEW_STREAM_FIFO_SIZE)
		{
			ret=0;
		}	
		else
		{
			memcpy(fdev.mem+fdev.current_len,ptr,len);
			ret=len;
		}
		fdev.current_len += len;
		sp5kOsMutexPut(&fdev.sRFFIFOMutex);

		return ret;
}

UINT8 dxg_fifo_is_closed(void)
{
		UINT8 ret=0;
		ret=(fdev.current_len==LIVEVIEW_STREAM_FIFO_SIZE)?1:0;
		return ret;
}

UINT32 dxg_dump_get_fifo_size(void)
{

		return  fdev.current_len; //remain 4k 5m9 no busy 
	
}
int app_stream_fifo_read(UINT8 *ptr, int len)
{

		int nread=0;
		int rval=-1;
		
		nread = dxg_fifo_read( ptr,len,0);
		if (nread != len)
		{			
			//printf("read error! nread:%d len:%d\n",nread,len);
			dxg_fifo_close();
			return 0;
			
		}
		if(nread>0)
		{
			
		}				
		return nread;	
}

#if 0
UINT8 app_mjpeg_fifo_is_empty(void)
{	

		int fifo_size=0;
			
		fifo_size = dxg_dump_get_fifo_size();
		if(fifo_size == 0 )
		{
			return 1; //FIFO EMPTY
		}else{
			return 0;	
		}
	
}

int app_mjpeg_fifo_read(UINT8 *mjpeg_ptr, UINT32 *mjpeg_len){	
	
		int rval;	
		
		// FIFO open 	
		rval = app_stream_fifo_read(mjpeg_len,4);
		rval = app_stream_fifo_read(mjpeg_ptr,*mjpeg_len);	
		dxg_fifo_close();
		
		return rval;
}
#endif

typedef struct tagJpegFifoInfo{
	UINT8 nIndex;
	UINT8 *nImageBuffer;
	UINT8  nImageFill;
	//struct tagJpegFifoInfo *pJpgInfo;
	struct tagJpegFifoInfo *pNext;

}JpegFifoInfo;

 JpegFifoInfo *pHeadNode=NULL;
 JpegFifoInfo *pReadNode=NULL;
 JpegFifoInfo *pWriteNode=NULL;

#if 0
UINT8 app_mjpeg_fifo_is_full(void)
{		

	int ret=0;			
	if(pWriteNode->nImageFill)
		ret=1;////FIFO FULL				
	return ret;
}
extern UINT32 halDmaMemcpy(void *pdst,const void *psrc,UINT32 size);

int app_mjpeg_fifo_write(UINT8 *mjpeg_ptr, UINT32 mjpeg_len)
{
	

	//sp5kOsMutexGet(&fdev.sRFFIFOMutex, 0);		
	//sp5kOsMutexPut(&fdev.sRFFIFOMutex);
	
	memcpy(pWriteNode->nImageBuffer,(UINT8*)&mjpeg_len,4);
	memcpy(pWriteNode->nImageBuffer+4,mjpeg_ptr,mjpeg_len);
	pWriteNode->nImageFill=1;
		
	return 1;
}

UINT8  app_mjpeg_fifo_is_empty(void)
{	

		
		int ret=0;
		
		//sp5kOsMutexGet(&fdev.sRFFIFOMutex, 0);			
		if(pReadNode->nImageFill==FALSE)
			ret=1;//FIFO EMPTY
				
		//sp5kOsMutexPut(&fdev.sRFFIFOMutex);
		return ret;
}

int  app_mjpeg_fifo_read(UINT8 *mjpeg_ptr, UINT32 *mjpeg_len)
{	
	
		int rval;	
		//sp5kOsMutexGet(&fdev.sRFFIFOMutex, 0);
		if(pReadNode->nImageFill)
		{
			
			// FIFO open 				
			memcpy((UINT8*)mjpeg_len,pReadNode->nImageBuffer,4);			
			memcpy(mjpeg_ptr,pReadNode->nImageBuffer+4,*mjpeg_len);												
			rval=*mjpeg_len;			
			pReadNode->nImageFill=0;
		}
		else
			rval=0;		
		//sp5kOsMutexPut(&fdev.sRFFIFOMutex);
		return rval;
}
#else

UINT8 app_mjpeg_fifo_is_full(void)
{	

	
	int ret=0;
	sp5kOsMutexGet(&fdev.sRFFIFOMutex, 0);	
	
	if(pReadNode->pNext==pWriteNode||pWriteNode->pNext==pReadNode)
		ret=1; //FIFO FULL	
	sp5kOsMutexPut(&fdev.sRFFIFOMutex);

	return ret;
}
extern UINT32 halDmaMemcpy(void *pdst,const void *psrc,UINT32 size);

int app_mjpeg_fifo_write(UINT8 *mjpeg_ptr, UINT32 mjpeg_len)
{
	
			
	memcpy(pWriteNode->nImageBuffer,(UINT8*)&mjpeg_len,4);
	memcpy(pWriteNode->nImageBuffer+4,mjpeg_ptr,mjpeg_len);
		
	sp5kOsMutexGet(&fdev.sRFFIFOMutex, 0);
	pWriteNode->nImageFill=1;
	pWriteNode=pWriteNode->pNext;
	sp5kOsMutexPut(&fdev.sRFFIFOMutex);

	return 1;
}

UINT8  app_mjpeg_fifo_is_empty(void)
{	
		
		int ret=0;		
		sp5kOsMutexGet(&fdev.sRFFIFOMutex, 0);			
		if(pWriteNode->nIndex==pReadNode->nIndex)
			ret=1;//FIFO EMPTY		
		sp5kOsMutexPut(&fdev.sRFFIFOMutex);
		return ret;
}

int  app_mjpeg_fifo_read(UINT8 *mjpeg_ptr, UINT32 *mjpeg_len)
{	
	
		int rval;	
		
		if(pReadNode->nImageFill)
		{
			
			// FIFO open 	
			//rval = app_stream_fifo_read(mjpeg_len,4);
			memcpy((UINT8*)mjpeg_len,pReadNode->nImageBuffer,4);
			//rval = app_stream_fifo_read(mjpeg_ptr,*mjpeg_len);
			memcpy(mjpeg_ptr,pReadNode->nImageBuffer+4,*mjpeg_len);			

			sp5kOsMutexGet(&fdev.sRFFIFOMutex, 0);
			pReadNode->nImageFill=0;
			rval=*mjpeg_len;
			pReadNode=pReadNode->pNext;	
			sp5kOsMutexPut(&fdev.sRFFIFOMutex);
			
		}
		else
			rval=0;
		
		
		return rval;
}


#endif



UINT8 app_nrf_is_frame_ready()
{

	UINT8 rval;
	
	nrf_fbuf_stats.frame_buf_ptr = nrf_frame_buffer;

	if(1==nrf_fbuf_stats.buf_is_readout)
	{	
		rval =  1; //ready
	}
	else
	{
	
		if(app_mjpeg_fifo_is_empty())
		{
			rval = 0;
		}
		else
		{	
			rval = app_mjpeg_fifo_read(nrf_frame_buffer,&nrf_fbuf_stats.dataSize);
		}
		
		if( rval > 0 )
		{	
			nrf_fbuf_stats.buf_is_readout=1;	
			rval = 1; //ready			
		}
		else
		{
			//printf("fifo is empty!\n");
			rval = 0; //stack empty
		}
	}
			
	return rval;
}

void nrf_bcu_send_stream_respond_packet(UINT8 *packet, UINT8 length)
{	


	nrf_package_bcu_header( pload_send , NRF_BCU_STREAM_PACKET , bcu_slot , NRF_BCU_CTRL_PCODE_STREAM_RESPOND );
	memcpy(pload_send+BCU_HEADER_LEN,packet,length); 
	
	// rescue backup 
	memcpy(pkt_sync_data[ pload_send[1] ],pload_send,RF_PAYLOAD_LENGTH);//sync		
	RFWriteDat(NRF_W_ACK_PAYLOAD|RF_PIPE_0,pload_send,length+BCU_HEADER_LEN );
	
	//app_nrf_pkt_stats.pkt_send += length;		
	app_nrf_pkt_stats.pkt_send += (length+BCU_HEADER_LEN);		
}

int nrf_packet_mjpeg_header_send(nrf_buf_status_e resend)
{	

	UINT8 cmd_id;
	UINT8  ch;
	UINT32 crc=0;	
	//-------------------------------------------------
	//  Set up the payload
	//  packet type  - 1byte
	//  length 	     - 4byte
	//-------------------------------------------------	

	UINT8 tmp_pload[NRF_DATA_PALOAD_LEN]={0};	

	if(1==app_nrf_is_frame_ready())
	{

		cmd_id = NRF_CMD_MJPEG_HEADER;

		if( (resend==FBUF_SEND) )
		{
				
			tmp_pload[0] = NRF_TP_VIDEO_PACKET;	
			tmp_pload[1] = 0xFF;
			//nrf_crc32
			tmp_pload[2] = 0x00; //CRC32
			tmp_pload[3] = 0x00; //CRC32		
			tmp_pload[4] = 0x00;//CRC32	
			tmp_pload[5] = 0x00;//CRC32	
			
			CPU2U32LE(tmp_pload+6,nrf_fbuf_stats.dataSize);	//[2]~[5]
			tmp_pload[10] = 0x00;
			tmp_pload[11] = 0x00;				
			CPU2U16LE(tmp_pload+12,cmd_id);		
			tmp_pload[14] = 0x00;	
			tmp_pload[15] = 0x00;	
			CPU2U16LE(tmp_pload+16,mjpeg_frame_id);			
			tmp_pload[18] = 0x00; //CRC32
			tmp_pload[19] = 0x00; //CRC32		
			tmp_pload[20] = 0x00;//CRC32	
			tmp_pload[21] = 0x00;//CRC32	

			//mjpeg data crc 
			crc = nrf_crc32(nrf_fbuf_stats.frame_buf_ptr,nrf_fbuf_stats.dataSize);
			//crc=0;
			CPU2U32LE(tmp_pload+18,crc);	

			//header packet crc 
			crc = nrf_crc32(tmp_pload+6,NRF_DATA_PALOAD_LEN-6);
			CPU2U32LE(tmp_pload+2,crc);				
			//printf("frame len=%d\n",nrf_fbuf_stats.dataSize);
			nrf_bcu_send_stream_respond_packet( tmp_pload,NRF_DATA_PALOAD_LEN);
			
			last_mjpeg_frame_id = mjpeg_frame_id;
			mjpeg_frame_id++;

		}
		else if(resend==FBUF_RESNED)
		{ //resend
		
			tmp_pload[0] = NRF_TP_VIDEO_PACKET;	
			tmp_pload[1] = 0xFF;
			//nrf_crc32
			tmp_pload[2] = 0x00; //CRC32
			tmp_pload[3] = 0x00; //CRC32		
			tmp_pload[4] = 0x00;//CRC32	
			tmp_pload[5] = 0x00;//CRC32	
			
			CPU2U32LE(tmp_pload+6,nrf_fbuf_stats.dataSize);	//[6]~[7]
			tmp_pload[10] = 0x00;
			tmp_pload[11] = 0x00;				
			CPU2U16LE(tmp_pload+12,cmd_id);	//[12]~[13]	
			tmp_pload[14] = 0x00;	
			tmp_pload[15] = 0x00;	
			CPU2U16LE(tmp_pload+16,last_mjpeg_frame_id);	//[16]~[17]			
			tmp_pload[18] = 0x00; //CRC32
			tmp_pload[19] = 0x00; //CRC32		
			tmp_pload[20] = 0x00;//CRC32	
			tmp_pload[21] = 0x00;//CRC32	

			//mjpeg data crc 
			crc = nrf_crc32(nrf_fbuf_stats.frame_buf_ptr,nrf_fbuf_stats.dataSize);			
			CPU2U32LE(tmp_pload+18,crc);	

			//header packet crc
			crc = nrf_crc32(tmp_pload+6,NRF_DATA_PALOAD_LEN-6);			
			CPU2U32LE(tmp_pload+2,crc);				

			nrf_bcu_send_stream_respond_packet( tmp_pload,NRF_DATA_PALOAD_LEN);
		
		}
		else
		{			
			return NRF_FAIL_SEND;	
		}

	}
	else
	{
		return NRF_STREAM_NOT_READY;
	}

	return NRF_SENDING;	
	
}


void nrf_request_new_frame(void)
{
	 
		nrf_fbuf_stats.ready = 0;
		nrf_fbuf_stats.dataSize = 0;
		nrf_fbuf_stats.chunkIndex = 0;
		nrf_fbuf_stats.headerSended = 0;
		//nrf_fbuf_stats.chunkSize = 32;
		nrf_fbuf_stats.buf_is_readout=0;
	 
}
static UINT32  nSendCnt=0;
int nrf_packet_mjpeg_data_send(nrf_buf_status_e resend)
{	

	UINT16 i;
	static SINT32 last_tick=0;
	nrf_fbuf_stats.chunkSize = NRF_DATA_PALOAD_LEN;
	
	if(1==app_nrf_is_frame_ready())
	{
		nFrameStart=1;
		//nFrameRead++;
		if( resend==FBUF_SEND )
		{

			 // make sure we don't go past the data size
		        if ( nrf_fbuf_stats.chunkIndex + nrf_fbuf_stats.chunkSize > nrf_fbuf_stats.dataSize)
			{
					nrf_fbuf_stats.chunkSize = nrf_fbuf_stats.dataSize - nrf_fbuf_stats.chunkIndex;			
		        }
			 
			nrf_bcu_send_stream_respond_packet( (nrf_fbuf_stats.frame_buf_ptr+nrf_fbuf_stats.chunkIndex),
														NRF_DATA_PALOAD_LEN);
			 	
			///printf("Send Mjpeg Chunk:%u/%u Size:%u Resend:%u\n",nrf_fbuf_stats.chunkIndex,nrf_fbuf_stats.dataSize,nrf_fbuf_stats.chunkSize,resend);	
								
			nrf_fbuf_stats.chunkIndexSending =  nrf_fbuf_stats.chunkIndex;
			nrf_fbuf_stats.chunkSizeSending = nrf_fbuf_stats.chunkSize;

			 // increase byte index by [chunkSize]
		        nrf_fbuf_stats.chunkIndex += nrf_fbuf_stats.chunkSize;

			//done
		       if (nrf_fbuf_stats.chunkIndex >= nrf_fbuf_stats.dataSize) 
			 {								
				//may be need some delay
				//need make sure last chunk is sended and recv ack			
				nrf_request_new_frame();
				nFrameRead++;
				//last_tick=app_nrf_sys_ticks();
				//printf("lt=%d\n",last_tick);
			}

		}
		else if(resend==FBUF_RESNED)
		{ 
				//resend				
				nrf_bcu_send_stream_respond_packet( (nrf_fbuf_stats.frame_buf_ptr+nrf_fbuf_stats.chunkIndexSending),
															NRF_DATA_PALOAD_LEN);													
		}
		else
		{		
			return NRF_FAIL_SEND;
		}

	}
	else
	{
		//Stream Frame is not ready		
		return NRF_STREAM_NOT_READY;
	}
	
	return NRF_SENDING;
}


void nrf_send_stream_respond_packet(void)
{

	int rval;
	static SINT32 curr_tick=0;
	nrf_buf_status_e resend = FBUF_SEND; //always send new one

	#ifndef NRF_STREAM_NAND_REC_LIVEVIEW
		if( (nrf_if_get_storage_type()==NRF_IF_STORAGE_NAND) &&
			(nrf_if_get_boss_record_state()==NRF_IF_RECORD_START))
		{
			nrf_bcu_send_stream_busy_packet(); //Not Support NAND Liveview
			return;
		}
	#endif	
	
	if(0==nrf_fbuf_stats.headerSended)
	{
		rval = nrf_packet_mjpeg_header_send(resend);
		nSendCnt=0;
		//printf("--hs =%d\n",nSendCnt);
		if(rval==NRF_SENDING)
		{
			nrf_fbuf_stats.headerSended=1;
			mjpeg_frame_send_cnt++;
			curr_tick=app_nrf_sys_ticks();
			//printf("ct=%d\n",curr_tick);
		}
	}
	else
	{	
		nSendCnt++;
		//printf("--ds=%d\n",nSendCnt);
		rval = nrf_packet_mjpeg_data_send(resend);	
	}

	if(rval==NRF_STREAM_NOT_READY)
	{
		///printf("sf is busy \n");	
		// Stream Frame is not ready 
		nrf_bcu_send_stream_busy_packet();
	}

}


void nrf_bcu_send_stream_dummy_respond_packet(void)
{
	UINT32 crc=0;	
	UINT8 *data_pload;
	
	//BCU Package 
	nrf_package_bcu_header( pload_send , NRF_BCU_STREAM_PACKET , bcu_slot , NRF_BCU_CTRL_PCODE_STREAM_DUMMY_RESPOND );	
	//data package 
	data_pload = ( pload_send + BCU_HEADER_LEN );
	//load
	data_pload[0] = 0;

	memset(data_pload,0x55,NRF_DATA_PALOAD_LEN); //set pattern			
	RFWriteDat(NRF_W_ACK_PAYLOAD|RF_PIPE_0,pload_send,RF_PAYLOAD_LENGTH );

	app_nrf_pkt_stats.pkt_send += (RF_PAYLOAD_LENGTH);	
}


void nrf_if_update_sys_timestamp(STimeStamp_s *tsp)
{
	//struct tm rtc_tm;
	tmx_t  rtc_tm;

	#ifdef DVSD01_INTERFACE
	//sTimer_Manager sT ;
	#endif

	UINT16 sec_diff,min_diff,hour_diff,day_diff,month_diff,year_diff;

	if( ( (tsp->year+2000) < 2013) || ( (tsp->year+2000) > 2099) )
	{ 
		// datetime verify
		return;
	}
	
	sp5kRtcDateTimeGet(SP5K_DATE_TIME_OPTION ,&rtc_tm);

	sec_diff 	= ABS(rtc_tm.tmx_sec - tsp->sec);
	min_diff 	= ABS(rtc_tm.tmx_min - tsp->min);
	hour_diff 	= ABS(rtc_tm.tmx_hour - tsp->hour);
	day_diff	= ABS(rtc_tm.tmx_mday - tsp->day);
	month_diff 	= ABS( (rtc_tm.tmx_mon+1) - tsp->mon);
	year_diff = rtc_tm.tmx_year - (tsp->year+2000);	

	//printf("[NRF] Datetime diff %u-%u-%u %u:%u:%u\n",year_diff,month_diff,day_diff,	hour_diff,min_diff,sec_diff);	

	if( (sec_diff>=10) | (min_diff !=0) | (hour_diff !=0) | (day_diff !=0) | (month_diff !=0) | (year_diff !=0)   )
	{

		printf("[NRF] Update system datetime\n");
		printf("[NRF] Datetime old %u-%u-%u %u:%u:%u\n",
					rtc_tm.tmx_year,(rtc_tm.tmx_mon+1),rtc_tm.tmx_mday,
					rtc_tm.tmx_hour,rtc_tm.tmx_min,rtc_tm.tmx_sec
		);
		
		rtc_tm.tmx_year = (tsp->year+2000);
	       rtc_tm.tmx_mon = tsp->mon-1;
		rtc_tm.tmx_mday = tsp->day;
	  	rtc_tm.tmx_hour = tsp->hour;
		rtc_tm.tmx_min = tsp->min;
		rtc_tm.tmx_sec = tsp->sec ;	

		printf("[NRF] Datetime new %u-%u-%u %u:%u:%u\n",
					rtc_tm.tmx_year,(rtc_tm.tmx_mon+1),rtc_tm.tmx_mday,
					rtc_tm.tmx_hour,rtc_tm.tmx_min,rtc_tm.tmx_sec
		);	

		#ifdef DVSD01_INTERFACE

			//sT.year = (tsp->year+2000);
			//sT.month = tsp->mon;
			//sT.date = tsp->day;
			//sT.hour = tsp->day;
			//sT.min = tsp->day;		

			//Do_Set_Timer(sT,1);
			//rtc_set_time(&rtc_tm);//setting sec
			sp5kRtcDateTimeSet(SP5K_DATE_TIME_OPTION ,&rtc_tm);
		#else
			//rtc_set_time(&rtc_tm);
			sp5kRtcDateTimeSet(SP5K_DATE_TIME_OPTION ,&rtc_tm);
		#endif
		
	}
	else
	{

		#if 0 //debug 
		printf("[NRF] Datetime System %u-%u-%u %u:%u:%u\n",
					rtc_tm.tm_year,(rtc_tm.tm_mon+1),rtc_tm.tm_mday,
					rtc_tm.tm_hour,rtc_tm.tm_min,rtc_tm.tm_sec
		);

		printf("[NRF] Datetime Recv %u-%u-%u %u:%u:%u\n",
					(tsp->year+2000),tsp->mon,tsp->day,
					tsp->hour,	tsp->min,	tsp->sec
		);		
		#endif		
	
	}


}

void nrf_if_get_sys_timestamp(STimeStamp_s *tsp)
{
		
	tmx_t rtcDateTime;

	sp5kRtcDateTimeGet(SP5K_DATE_TIME_OPTION ,&rtcDateTime);
			
	tsp->year = (rtcDateTime.tmx_year-2000);
	tsp->mon = rtcDateTime.tmx_mon+1;	
	tsp->day = rtcDateTime.tmx_mday;
	tsp->hour = rtcDateTime.tmx_hour;
	tsp->min = rtcDateTime.tmx_min;
	tsp->sec =  rtcDateTime.tmx_sec;
	
}

void nrf_bcu_send_ctrl_status_respond_packet(void)
{

	NRF_Cam_Status_Respond_Packet_s WCTRL_Pkt;
	STimeStamp_s cur_datetime;
	UINT32 crc=0;	
	UINT8 *data_pload;

	nrf_if_get_sys_timestamp(&cur_datetime);

	memset(pload_send,0x00,RF_PAYLOAD_LENGTH);
	
	// BCU Package 
	nrf_package_bcu_header( pload_send , NRF_BCU_CTRL_PACKET , bcu_slot , NRF_BCU_CTRL_PCODE_STATUS_RESPOND );
	
	// data package 
	data_pload = ( pload_send + BCU_HEADER_LEN );
	WCTRL_Pkt.crc = 0;
	if(nrf_if_get_boss_record_state()==NRF_IF_RECORD_START)
	{
		WCTRL_Pkt.record_state = DEV_RECORD_START;
	}
	else
	{
		WCTRL_Pkt.record_state = DEV_RECORD_STOP;	
	}
	WCTRL_Pkt.capture_state = 0 ;	

	// check error status 
	// check storage status

	if(nrf_if_get_error_state()==NRF_IF_ERROR_NONE)
	{
		WCTRL_Pkt.error_state = DEV_ERROR_NONE;	
	}
	else
	{
		//WCTRL_Pkt.error_state = DEV_ERROR_CARD_FULL;
		WCTRL_Pkt.error_state = DEV_ERROR_NONE;
		//printf("---card is full key no action\n");
	}
		
	// check storage is nand or sdcard 
	if( nrf_if_get_storage_type()==NRF_IF_STORAGE_NAND )
	{ 
		WCTRL_Pkt.storage_type = DEV_STORAGE_NAND;		
	}
	else
	{
		WCTRL_Pkt.storage_type = DEV_STORAGE_SDCARD;	
	}	
	#if 0
	WCTRL_Pkt.datetime.sec = cur_datetime.sec;	
	WCTRL_Pkt.datetime.min = cur_datetime.min;	
	WCTRL_Pkt.datetime.hour = cur_datetime.hour;	
	WCTRL_Pkt.datetime.day = cur_datetime.day;	
	WCTRL_Pkt.datetime.mon = cur_datetime.mon;	
	WCTRL_Pkt.datetime.year = cur_datetime.year;		
	#else
	WCTRL_Pkt.datetime.sec = 0;	
	WCTRL_Pkt.datetime.min = 0;	
	WCTRL_Pkt.datetime.hour = 0;	
	WCTRL_Pkt.datetime.day = 0;	
	WCTRL_Pkt.datetime.mon = 0;	
	WCTRL_Pkt.datetime.year = 0;		
	#endif
	//load package
	data_pload[0] = 0;	data_pload[1] = 0; data_pload[2] = 0; data_pload[3] = 0;  // crc 4 byte
	data_pload[4] = WCTRL_Pkt.record_state;
	data_pload[5] = WCTRL_Pkt.capture_state;	
	data_pload[6] = WCTRL_Pkt.error_state;
	data_pload[7] = WCTRL_Pkt.storage_type;

	data_pload[8] = WCTRL_Pkt.datetime.sec;
	data_pload[9] = WCTRL_Pkt.datetime.min;
	data_pload[10] = WCTRL_Pkt.datetime.hour;
	data_pload[11] = WCTRL_Pkt.datetime.day;
	data_pload[12] = WCTRL_Pkt.datetime.mon;
	data_pload[13] = WCTRL_Pkt.datetime.year;	
	
	//CPU2U32LE(data_pload+8,WCTRL_Pkt.rec_time);	//[3]~[6]
		
	// rescue backup 
	memcpy(pkt_sync_data[ pload_send[1] ],pload_send,RF_PAYLOAD_LENGTH);/*sync*/
	RFWriteDat(NRF_W_ACK_PAYLOAD|RF_PIPE_0,pload_send,RF_PAYLOAD_LENGTH );

	app_nrf_pkt_stats.pkt_send += (RF_PAYLOAD_LENGTH);	

	//printf("rWrs:%u\n",WCTRL_Pkt.record_state);
	//printf("rWcs:%u\n",WCTRL_Pkt.capture_state);	
	//printf("rWes:%u\n",WCTRL_Pkt.error_state);
	//printf("WCTRL_Pkt.time:%d",WCTRL_Pkt.time);

}


void nrf_packet_recv_ctrl_handler(UINT8 *pload_rd)
{

	NRF_BCU_Header pktBCU;
	UINT8 *data_pload;
	UINT32 header_crc;
	static UINT8 record_state = DEV_RECORD_STOP;

	NRF_Watch_Control_Packet_t WCTRL_Pkt={0};

	/* BCU Header Process--------------------------------------- */
	pktBCU.header = U16LE2CPU(pload_rd);	
	/* Packet Data Recv */
	data_pload = pload_rd+BCU_HEADER_LEN; //skip bcu	
	WCTRL_Pkt.crc = U32LE2CPU(data_pload); // 4 byte
	WCTRL_Pkt.record_state = *(data_pload+4);
	WCTRL_Pkt.capture_state = *(data_pload+5);	
	WCTRL_Pkt.error_state = *(data_pload+6);
	//WCTRL_Pkt.time = U32LE2CPU(data_pload+7);

	WCTRL_Pkt.datetime.sec =  *(data_pload+7);
	WCTRL_Pkt.datetime.min =  *(data_pload+8);
	WCTRL_Pkt.datetime.hour =  *(data_pload+9);
	WCTRL_Pkt.datetime.day =  *(data_pload+10);
	WCTRL_Pkt.datetime.mon =  *(data_pload+11);
	WCTRL_Pkt.datetime.year =  *(data_pload+12);	

	//printf("Wrs:%u\n",WCTRL_Pkt.record_state);
	//printf("Wcs:%u\n",WCTRL_Pkt.capture_state);	
	//printf("Wes:%u\n",WCTRL_Pkt.error_state);
	//printf("WCTRL_Pkt.time:%d",WCTRL_Pkt.time);
#if 0	
	printk_co(GREEN,"%u-%u-%u %u:%u:%u",
			WCTRL_Pkt.datetime.year+2000,
			WCTRL_Pkt.datetime.mon,
			WCTRL_Pkt.datetime.day,
			WCTRL_Pkt.datetime.hour,			
			WCTRL_Pkt.datetime.min,						
			WCTRL_Pkt.datetime.sec					
	);
#endif

#ifdef NRF_DATETIME_SYNC
	nrf_if_update_sys_timestamp(&WCTRL_Pkt.datetime);
#endif

	if(WCTRL_Pkt.capture_state==1)
	{			
		switch(WCTRL_Pkt.capture_state)
		{
			case DEV_CAPTURE_IDLE:	
				break;
			case DEV_CAPTURE_START:	
				sp5kHostMsgSend(APP_UI_MSG_RF_RCWD01_SNAP,0);
				printf("---snap3\n");
				break;			
			default:
			break;			
		}

	}
	else
	{
		if(record_state != WCTRL_Pkt.record_state)
		{
			record_state = WCTRL_Pkt.record_state;
			switch(WCTRL_Pkt.record_state)
			{
				case DEV_RECORD_START:		
					sp5kHostMsgSend(APP_UI_MSG_RF_RCWD01_REC_START,0);
					printf("---rec1\n");
					break;			
				case DEV_RECORD_STOP:		
					sp5kHostMsgSend(APP_UI_MSG_RF_RCWD01_REC_STOP,0);	
					printf("---rec2\n");
					break;
				default:				
					break;
			}
		}
	}				
		
	nrf_bcu_send_ctrl_status_respond_packet();
	
}


void nrf_packet_recv_handler(UINT8 *pload_rd)
{

	UINT8  sync;
	int rval;

	NRF_BCU_Header pktBCU;
	
	//BCU Header
	pktBCU.header = U16LE2CPU(pload_rd);//a+1 msb  a  lsb
	if(pktBCU.head.type == NRF_BCU_RESCUE_PACKET)
	{				
		bcu_rescue_sync = pktBCU.head.sync;				
		nrf_set_bcu_state(BCU_RESCUE_RESPOND);

		if(nrf_get_bcu_state()==BCU_RESCUE_RESPOND)
		{
			nrf_bcu_send_rescue_packet();
		}		
	}
	else
	{

		if( NRF_BCU_STREAM_PACKET == pktBCU.head.type )
		{

			bcu_slot = pktBCU.head.hop; //for slot check												
			switch(pktBCU.head.pcode){	
				case NRF_BCU_CTRL_PCODE_STREAM_REQUEST:					
					nrf_send_stream_respond_packet();				
				break;
				case NRF_BCU_CTRL_PCODE_STREAM_DUMMY_REQUEST:					
					nrf_bcu_send_stream_dummy_respond_packet();
				break;
				case NRF_BCU_CTRL_PCODE_STREAM_DUMMY_RESPOND:
																
				break;	
			}

		}
		else if( NRF_BCU_CMD_PACKET == pktBCU.head.type )
		{
			bcu_slot = pktBCU.head.hop; //for slot check			
			//nrf_packet_recv_cmd_handler(pload_rd);
		}
		else if( NRF_BCU_CTRL_PACKET == pktBCU.head.type )
		{
			bcu_slot = pktBCU.head.hop; //for slot check			
			nrf_packet_recv_ctrl_handler(pload_rd);
			
		}
		else
		{			
			nrf_packet_recv_ctrl_handler(pload_rd);			
			nrf_bcu_send_ctrl_status_respond_packet();	
		}
	
	}
	
}


UINT8 raw_data_ready =0;
static UINT16 raw_img_w =0;
static UINT16 raw_img_h =0;

#define RF_PREVIEW_BUF_WIDTH	(128)//(432)
#define RF_PREVIEW_BUF_HEIGHT	(96)  //(240)

#define NORMAL_FRAME_LIMIT_TIME 60			//~12 //91  //~ 11 FPS
#define NORMAL_FRAME_RECORD_LIMIT_TIME 332   // 3 FPS

static UINT8  frame_limit_flag = 1;
static SINT32 frame_limit_tick = 0;
static SINT32 frame_limit_time = NORMAL_FRAME_LIMIT_TIME;

#define RF_PREVIEW_BUF_SIZE (RF_PREVIEW_BUF_WIDTH*RF_PREVIEW_BUF_HEIGHT*2) // *2 for Y and UV
#define RF_PREVIEW_U_BUF_SIZE  ((RF_PREVIEW_BUF_WIDTH*RF_PREVIEW_BUF_HEIGHT) >> 1)
#define RF_PREVIEW_V_BUF_SIZE  ((RF_PREVIEW_BUF_WIDTH*RF_PREVIEW_BUF_HEIGHT) >> 1)

#if 0
UINT8 app_mjpeg_fifo_is_full(void)
{	

	int remain;
		
	remain = LIVEVIEW_STREAM_FIFO_SIZE-dxg_dump_get_fifo_size();
	if(remain <= LIVEVIEW_STREAM_KEEP_REMAIN)
	{
		return 1; //FIFO FULL
	}
	else
	{
		return 0;	
	}
	
}
#endif

int rf_liveview_preprocess(void)
{

	//Dual stream pitch preivew width height or pitch=0 pitch 16bytes align 	
		
	//mjpeg fifo is full 
	if( app_mjpeg_fifo_is_full() )
	{			
		return -1;
	}
	//encoding is still processing
	if(raw_data_ready==1)
	{			
		return -1;
	}		
						
	if(frame_limit_flag)
	{						
		if(nrf_if_get_boss_record_state()==NRF_IF_RECORD_START)

		
		{
			frame_limit_time = NORMAL_FRAME_RECORD_LIMIT_TIME;
		}
		else
		{
			frame_limit_time = NORMAL_FRAME_LIMIT_TIME;
		}
		
		if( (app_nrf_sys_ticks()-frame_limit_tick) < frame_limit_time )
		{
			return -1; //Force Limit 
		}
		else
		{
			frame_limit_tick = app_nrf_sys_ticks();
		}		
	}
			
		// if input 352x240 fixed crop to 320x240
	raw_img_w=320;
	raw_img_h=240;			
	raw_data_ready=1;			

	return 0;
}

#if 0
int app_stream_fifo_write(UINT8 *ptr, int len)
{

	int rval = -1;
	
	rval = dxg_fifo_write(ptr, len, -1);

	printf("Fifo len=%d rval=%d\n",len,rval);
	
	if (rval != len) 
	{
		printf("fifo_write fail\n");
	}
	
	return rval;	
}

int app_mjpeg_fifo_write(UINT8 *mjpeg_ptr, UINT32 mjpeg_len)
{	

	int rval;
	rval = app_stream_fifo_write(&mjpeg_len,4);
	rval = app_stream_fifo_write(mjpeg_ptr,mjpeg_len);
	dxg_fifo_close();
	
	return rval;
}
#endif

void  RFRxThreadCallBack(ULONG nEID)
{
	static UINT8 nRFHopCnt=0;
	static UINT8 nRxLostCnt=0;
	ULONG ActualEvents=0;
	static UINT32 nChgCnt=0;
	SINT32 nrf_recv_time=0,cur_tick=0,duration=0;
	UINT32 err=0;
	
	printf("RFRxThreadCallBackxx run\n");
	
	RFRxFlushFIFO();
	RFTxFlushFIFO();
	
	sp5kOsEventFlagsSet(&RF_ISR, (UINT32)13, TX_OR);
	
	while(1)
	{	
		if(nRFParingMode)
		{
			sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 100);
		}	
		else
		{
			err=sp5kOsEventFlagsGet(&RF_ISR, 1, TX_OR_CLEAR, &ActualEvents,10);/// 100ms
			if((err==SP5K_SUCCESS)||err==SP5K_TIMEOUT)
			{
				//if(nPreviewSuspend)
					//continue;
			}
				if(ActualEvents !=13)
					continue;
		}
						
		if(g_nRcwMID==RCW5K8 &&nRFParingMode==0)
		{
			RFRxTimerCallBack(0);
			continue;
		}
		
		#ifdef NRF_STREAM_REC_TOO_BUSY
		if( nrf_if_get_boss_record_busy() > 45)
		{				
			continue;				
		}
		#endif
		
		//5P8 D01
		if(nPairAddrEnable==1)
		{
				//RFCELOW();
				RFSetTXRXAddr();
				RFSetup(0x27);
				//RFCEHIGH();
				nPairAddrEnable=0;
		}
		if(nRFParingMode)
 		{
 				RFTxFlushFIFO();
				RFParingID();
 		}
		if(nRFParingMode)
		{
			if(++nRxLostCnt>=15)
			{
					nRxLostCnt=0;
					nRFHopCnt++;				
					RFChgTXRXCh(nRFHopCnt);
					
			}
		}
		radio_irq();
		cur_tick = app_nrf_sys_ticks(); 

		if(radio_get_status () == RF_IDLE)
		{
				
				#ifdef STREAM_TIMEOUT_FIFO_FLUSH		
				//flush  stream buffer 
				if(cur_tick>nrf_recv_time)
				{					
					if ( (cur_tick-nrf_recv_time) > STREAM_FIFO_FLUSH_TIMEOUT)
					{
						app_mjpeg_fifo_clear();
						nrf_recv_time = app_nrf_sys_ticks(); 	
					}										
				}
				#endif
				#if 1
				if(cur_tick > nrf_recv_time)
				{
					if( (cur_tick - nrf_recv_time) > NRF_RESET_TIMEOUT )
					{
						nrf_recv_time = cur_tick;
						//reset function call
						//printf("5s reset rf system\n");
						//reset msg send
						//sp5kGpioIntEnable(SP5K_GPIO_INT11,SP5K_TRIG_LVL_FALL);
						//continue;
					}
				}
				#endif				
				sp5kGpioIntEnable(SP5K_GPIO_INT11,SP5K_TRIG_LVL_FALL);
				continue;
		}			
		else 
		{
			if( ((radio_get_status ()) == RF_RX_DR)|| ((radio_get_status ()) == RF_TX_AP) )
				{
					//printf("--Get pair from rcwd01\n");				
					RFRxFlushFIFO();											
					if(nRFParingMode)
					{
						if(RFParingComp(nPairIDLen))
						{	
	     						spi_delay_task(4);
						     	nRFParingMode = 0;
							//nPairAddrEnable=2;
							nChgCnt=0;							
						     	RFSetTXRXAddr();					
						     	RFParingID();
						    
						    
							RFCELOW();
							spi_delay_task(10);
							RFWriteCmd(NRF_RF_CONFIG,0x0F);
							spi_delay_task(10);

							 if(g_nRcwMID==RCW5K8)
								RFSetup(0x27); 
							 else							 	
								RFSetup(0x0C);     
							spi_delay_task(10);
							RFWriteCmd(NRF_RF_CH,3);
							spi_delay_task(10);
							RFCEHIGH();								 																			
						     	//printf("--Pair End ok\n");
							
								
						}
						else
						{
						     RF24ParingFail(); 
						}

					}
					else
					{
						if(nPairIDLen)	
						{
							 if(g_nRcwMID==RCW5K8)
							 {
								//spi_delay_task(4);								
								CameraStatusAckRply();	
								RFRxMsgFilter(&ReadPLDataBack[0],nPairIDLen);
															
							 }	
							 else
							 {
								nrf_packet_recv_handler(ReadPLDataBack);
							 }									
							nRxLostCnt=0;
						}	

					}															
					//D01 respond packet bcu header 										
					sp5kGpioIntEnable(SP5K_GPIO_INT11,SP5K_TRIG_LVL_FALL);
					
				}
				else 
				{
					//printf("rgs:%u",radio_get_status ());
				}
		}
		#if 0
		if(cur_tick > nrf_recv_time)
		{
				if( (cur_tick - nrf_recv_time) > NRF_RESET_TIMEOUT )
				{
					nrf_recv_time = cur_tick;
					//reset function call
					//msg send to ios
					sp5kGpioIntEnable(SP5K_GPIO_INT11,SP5K_TRIG_LVL_FALL);
					continue;
				}
		}				
		#endif
		#if 0
		if(!nRFParingMode)
		{
			if(g_nRcwMID==RCW5K8)
			{
				spi_delay_task(4);
				CameraStatusAckRply();
			}
		}
		#endif
		radio_set_status (RF_IDLE);
		sp5kGpioIntEnable(SP5K_GPIO_INT11,SP5K_TRIG_LVL_FALL);
		
	}

	//pThreadRFReceive = NULL;
}

UINT8 nSrtHop[3]={0,63,125};
void PTX_SendMode(ULONG iIrq)
{
	UINT8 data_buffer;
	UINT8 Buf[32];
	UINT8 i;
	UINT8 IRQ;
	UINT32 ActualEvents;
	NRF_BCU_Header pktBCU;
	
	UINT32 err=0;
	static UINT8 nDutMode=0;
	static UINT8 nSendCnt=0;
	//UINT8 nCH=0;
	
	printf("PTX_SendMode\n");
	
	RFWriteCmd(STATUS_MAP, 0x70);		// Clear all interupt flag	01110000
	RFTxFlushFIFO();
	RFRxFlushFIFO();
	
	sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 100);		
	while(1) 
	{	
		
		//err=sp5kOsEventFlagsGet(&RF_ISR, 1, TX_OR_CLEAR, &ActualEvents,20);///1ms
		sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 10);
		if(err==SP5K_TIMEOUT)
		{
			//printf("--SP5K_TIMEOUT\n");
		}	
		else if(err==SP5K_SUCCESS)
		{
			//printf("--SP5K_Event Trigger\n");
		}	
					
		radio_irq();
		if ( (radio_get_status ()) == RF_MAX_RT )
		{
			//continue;						
		}
		if(nDutMode==0)
		{
			nSendCnt++;
			//ch=0;
			memcpy(pload_send,"dxgsafet",8);
			RFWriteDat(NRF_W_TX_PAYLOAD,pload_send,8);
			if(nSendCnt==20)
			{								
				//nCH++;
				//nCH&=0x03;
				//nCH=1;
				nSendCnt=0;
				nDutMode=1;	
			}
							
		}
		else if(nDutMode==1)
		{
			//RFWriteCmd(NRF_RF_CH,95);			
			sprintf(pload_send,"ch%1dxxxxx",1);///1
			RFWriteDat(NRF_W_TX_PAYLOAD,pload_send,8);
			nDutMode=2;
		}
		else if(nDutMode==2)
		{
			RFWriteCmd(NRF_RF_CH,1);			
			nSendCnt++;
			memcpy(pload_send,"dxgsafet",8);
			RFWriteDat(NRF_W_TX_PAYLOAD,pload_send,8);
			if(nSendCnt==20)
			{								
				//nCH++;
				//nCH&=0x03;
				//nCH=2;
				nSendCnt=0;
				nDutMode=3;
			}
			
		}
		else if(nDutMode==3)
		{
			//RFWriteCmd(NRF_RF_CH,95);			
			sprintf(pload_send,"ch%1dxxxxx",2);///2
			RFWriteDat(NRF_W_TX_PAYLOAD,pload_send,8);
			nDutMode=4;
		}	
		else if(nDutMode==4)
		{
			RFWriteCmd(NRF_RF_CH,2);			
			nSendCnt++;
			memcpy(pload_send,"dxgsafet",8);
			RFWriteDat(NRF_W_TX_PAYLOAD,pload_send,8);
			if(nSendCnt==20)
			{								
				//nCH++;
				//nCH&=0x03;
				//nCH=0;
				nSendCnt=0;
				nDutMode=5;
			}
			
		}
		else if(nDutMode==5)
		{
			//RFWriteCmd(NRF_RF_CH,95);			
			sprintf(pload_send,"ch%1dxxxxx",0);
			RFWriteDat(NRF_W_TX_PAYLOAD,pload_send,8);
			nDutMode=6;
		}
		else if(nDutMode==6)
		{
			RFWriteCmd(NRF_RF_CH,0);						
			nSendCnt++;
			memcpy(pload_send,"dxgsafet",8);
			RFWriteDat(NRF_W_TX_PAYLOAD,pload_send,8);
			if(nSendCnt==20)
			{								
				//nCH++;
				//nCH&=0x03;
				//nCH=0;
				nSendCnt=0;
				nDutMode=1;
			}
			//nDutMode=0;			
		}
		
		if ( (radio_get_status ()) == RF_MAX_RT )
		{
								
		}
		
		//printf("nSendCnt=%d\n",nSendCnt);		
		radio_set_status (RF_IDLE);
		
	}
	
	
}

void PRX_RecieveMode(ULONG iIrq)
{
	UINT8 data_buffer;
	UINT8 Buf[32];
	UINT8 i;
	UINT8 IRQ;
	UINT32 ActualEvents;
	NRF_BCU_Header pktBCU;
	UINT32  nRecCnt[3]={0};
	UINT32 err=0;
	UINT8 nDutMode[3]={0};
	UINT8 nCH=0;
	//printf("PRX_SendMode\n");
	
	RFWriteCmd(STATUS_MAP, 0x70);		// Clear all interupt flag	01110000
	RFTxFlushFIFO();
	RFRxFlushFIFO();
	
	sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 100);		
	while(1) 
	{	
		
		err=sp5kOsEventFlagsGet(&RF_ISR, 1, TX_OR_CLEAR, &ActualEvents,1);///1ms
		if(err==SP5K_TIMEOUT)
		{
			//printf("--SP5K_TIMEOUT\n");
		}	
		else if(err==SP5K_SUCCESS)
		{
			printf("--Rx Trigger\n");
		}	
					
		radio_irq();
		//ch=0;
		if ( ((radio_get_status ()) == RF_RX_DR)|| ((radio_get_status ()) == RF_TX_AP) )
		{

			printf("---Info=%c %c %c\n",ReadPLDataBack[0],ReadPLDataBack[1],ReadPLDataBack[2]);
			
			if(nDutMode[0]==0)
			{

				nRecCnt[0]++;
				if(nRecCnt[0]==20)
				{										
					//nRecCnt[0]=0;
				}
			}
			if(nDutMode[1]==0)
			{

				nRecCnt[1]++;
				if(nRecCnt[1]==20)
				{										
					//nRecCnt[1]=0;
				}
			}
			if(nDutMode[2]==0)
			{

				nRecCnt[2]++;
				if(nRecCnt[2]==20)
				{										
					//nRecCnt[2]=0;
				}
			}
			if(ReadPLDataBack[0]=='c'&&ReadPLDataBack[1]=='h')
			{
					
					//printf("--lch=%d rec=%u\n",nCH,nRecCnt[nCH]);
					nCH=ReadPLDataBack[2]-'0';
					
					//printf("-newch=%d\n",nCH);

					RFWriteCmd(NRF_RF_CH,nCH);
					
					for(i=0;i<3;i++)
					{
						if(i==nCH)
						{
							nDutMode[i]=0;
							nRecCnt[i]=0;
						}	
						else
						{
							nDutMode[i]=1;
							nRecCnt[i]=0;
						}	
					}
					ReadPLDataBack[0]=0;
					ReadPLDataBack[1]=0;
					ReadPLDataBack[2]=0;
					
			}
			

												
		}
		
		radio_set_status (RF_IDLE);
		
	}
	
	
}


/*
return 0:ok
1:len error  or nCUID is too big
nCUID[0]=year nCUID[1]=week  1=<(nCUID[2]<<16|nCUID[3]<<8 |nCUID[4])<=99999
*/
UINT8 RFRemoterSetCUID(UINT8 *nCUID,UINT8 nLen)
{

	UINT8 *puiPara=NULL;	
	puiPara = nCUID;
	if(nLen!=5)
	{
		return 1;
	}
	else
	{
		DateCUID.Year=puiPara[0];	
	 	DateCUID.Week=puiPara[1];	
		//nPairRandomCUID=0x00<<16|0x09<<8|0x0a;
		nPairRandomCUID = 0;
		nPairRandomCUID = puiPara[2]<<16;
		nPairRandomCUID += puiPara[3]<<8;
		nPairRandomCUID += puiPara[4];
		if(nPairRandomCUID>=1)
		{ 
			
		}
		else
		{
			//nPairRandomCUID = 0x015B38;
			return 1;
		}
	}
	//printf("--UI CUID =%02x %02x %02x %02x %02x\n",puiPara[0],puiPara[1],puiPara[2],puiPara[3],puiPara[4]);
	//printf("--UI year=%d week=%d %d\n",DateCUID.Year,DateCUID.Week,nPairRandomCUID);
	return 0;


}

void IsrHandle()
{
	UINT32 err;
	err = sp5kOsEventFlagsSet(&RF_ISR, (UINT32)13, TX_OR);			
	//sp5kGpioIntEnable(SP5K_GPIO_INT11,0);	//display frame rate become slow	
}

void IsrAndSemphoreInit()
{

	static BOOL bSemFirst=1;
	UINT32 err=0;
	if(bSemFirst==0)
		return;
	bSemFirst=0;
			
	sp5kGpioIntEnable(SP5K_GPIO_INT11,0);
	sp5kGpioIsrReg(SP5K_GPIO_INT11,SP5K_TRIG_LVL_FALL,IsrHandle );	
	err = sp5kOsEventFlagsCreate(&RF_ISR, "IRQApp");
		
	fdev.sRFFIFOMutex = (TX_MUTEX)NULL;	
	sp5kOsMutexCreate(&fdev.sRFFIFOMutex, "FIFOMutex", 0);
	fdev.current_len=0;	
	sp5kOsMutexCreate(&MutexSPI, "SPIMutex", 0);
}

void IsrEnable()
{
	UINT32 err=0;
	//printf("---IsrEnable\n");
	sp5kGpioIntEnable(SP5K_GPIO_INT11,SP5K_TRIG_LVL_FALL);
	sp5kGpioIsrReg(SP5K_GPIO_INT11,SP5K_TRIG_LVL_FALL,IsrHandle );
	
}

typedef struct appVideoCB_s {
	UINT32 CapTimer;
	UINT32 CapSec;
	UINT8 osd;
	int fCapture;
	int fPause;
	int fStop;
	UINT32 isDiskReady;
	UINT32 err;
	UINT32 framePerSec;
	UINT32 streamBytesPerSec;
	UINT32 aviKbPerSec;
	UINT32 isAutoPowerOffOn;
} appVideoCB_t;
extern appVideoCB_t gVideoCB;
UINT8 FrameFreqencyFactor(void)
{
	
	UINT8 nFactor=3;
	switch(gVideoCB.framePerSec)
	{
		case 30:
			nFactor=3;
			break;
		case 60:
			nFactor=6;
			break;
		case 90:
			nFactor=9;
			break;
		case 240:
			nFactor=24;
			break;
		default:
			nFactor=3;
			break;
	}
	return nFactor;
		
}

void MJPGFifoCreate(void)
{
	
	#if 0
	pHeadNode=sp5kMalloc(sizeof(JpegFifoInfo));

	pHeadNode->nImageBuffer=sp5kMalloc(MJPEG_LEN);
	pHeadNode->nImageFill=0;
	pWriteNode=pHeadNode;
	pReadNode=pHeadNode;	
	#endif
	UINT8 nNN=0;
	if(pHeadNode!=NULL)
		return;	
	for(nNN=0;nNN<3;nNN++)
	{
		if(nNN==0)
		{
			pHeadNode=sp5kMalloc(sizeof(JpegFifoInfo));
			pWriteNode=pHeadNode;
			pReadNode=pHeadNode;
			pHeadNode->nIndex=nNN;
			pHeadNode->pNext=NULL;
			pHeadNode->nImageBuffer=sp5kMalloc(MJPEG_LEN);
			pHeadNode->nImageFill=0;				
		}
		else
		{
			pWriteNode=	sp5kMalloc(sizeof(JpegFifoInfo));
			pWriteNode->nIndex=nNN;			
			pWriteNode->nImageBuffer=sp5kMalloc(MJPEG_LEN);
			pReadNode->pNext=pWriteNode;
			pWriteNode->nImageFill=0;
			pReadNode = pWriteNode;
			if(nNN==2)
				pWriteNode->pNext=pHeadNode;
		}
	}
	
	pWriteNode=pHeadNode;
	pReadNode=pHeadNode;	
	//printf("Fifo Buffer Create\n");
		
}

void   RfSafeRegulationTest(UINT8 nSend)
{
		
	if(nSend)
	{
		nRFSpiMode = HWSPI;
		nRFMode=MD_TX;
	}	
	else
	{
		nRFSpiMode = HWSPI;
		nRFMode=MD_RX;
	}		
	nRF2401_MemoryInit();	  	
	RFSRTInit();
	spi_delay_task(100);	
	
	//cmdBatchRegister(rfspicmds);
	
	if(pThreadRFSend == NULL)
	{						
		//printf("PTX_SendMode  thd create\n");
		if(nSend==1)
		pThreadRFSend= sp5kOsThreadCreate("RF Send", PTX_SendMode, 0, 16, 16, 3, TX_AUTO_START);
		else
		pThreadRFSend= sp5kOsThreadCreate("RF Send", PRX_RecieveMode, 0, 16, 16, 3, TX_AUTO_START);
		
	}	

}
void RFChannelSwitch()
{
	  	UINT8 nCh[4]={1,3,41,77};
		static UINT8 nIdx=0;
		
		RFCELOW();
		spi_delay_task(10);
		#if 0
							
		RFWriteCmd(NRF_RF_CONFIG,0x0F);
		spi_delay_task(10);

		 if(g_nRcwMID==RCW5K8)
			RFSetup(0x27); 
		else							 	
			RFSetup(0x0C);     
		spi_delay_task(10);
							
		#endif
		RFWriteCmd(NRF_RF_CH,nCh);
		nIdx++;
		if(nIdx>4)
			nIdx=0;
		spi_delay_task(10);
		RFCEHIGH();
		printf("---RFChannelSwitch=%d\n",nIdx);

}
#if 1
static cmd_t rfspicmds[] = {
	{ "cehigh", RFCEHIGH,     "custom 1", NULL, },
	{ "cetlow",  RFCELOW,     "custom 2", NULL, },		
	{ "clkhigh",  vGpioRF_sckSetHigh,     "custom 4", NULL, },
	{ "clklow",  vGpioRF_sckSetLow,     "custom 5", NULL, },
	{ "mosihigh",  vGpioRF_MosiSetHigh,     "custom 6", NULL, },	
	{ "mosilow",  vGpioRF_MosiSetLow,     "custom 7", NULL, },	
	{ "irqlevel",  GetRF_IRQLevel,     "custom 9", NULL, },
	{ "csnhigh",  vGpioRF_CsnSetHigh,     "custom 10", NULL, },
	{ "csnlow",  vGpioRF_CsnSetLow,     "custom 10", NULL, },
	{ "gpinit",  vGpioRF_Init,     "custom 10", NULL, },	
	/*{ "readcuid",  fnPairCuidReadFile,     "custom 10", NULL, },*/
	{ "hwspi",  TestHWSpi,     "custom 10", NULL, },
	{ "pair",  appRfPairingStart,     "custom 10", NULL, },
	{ "rfinit",  appRfInit,     "custom 10", NULL, },
	{ "rfrs",  appRfRelease,     "custom 10", NULL, },
	{ "pafh",  appRfParingFinish,     "custom 10", NULL, },
	{ "jpgf",  MJPGFifoCreate,     "custom 10", NULL, },
	{ "irqh",  IsrHandle,     "custom 10", NULL, },
	{ "irqe",  IsrEnable,     "custom 10", NULL, },	
	{ "rfch",  RFChannelSwitch,     "custom 10", NULL, },
	{ NULL,  NULL,            NULL, NULL, }, /* null terminator */
};

void RF24L01DriverLoad(void)
{		
	cmdBatchRegister(rfspicmds);
	//IsrAndSemphoreInit();
	//MJPGFifoCreate();
	//RfSafeRegulationTest(0);
	//appRfInit(NULL);	
}
#endif

#if 0
//readcuid
UINT32  fnPairCuidReadFile()
{

	UINT32 fd;
	UINT32 size=0,len=0;
	UINT8 buffer[256];
	UINT32 nIndex=0;
	printf("--ReadCUID fnPairCuidReadFile\n");
	fd = sp5kFsFileOpen("D:\\CUID.TXT ", SP5K_FS_OPEN_RDONLY);
	
	if (fd) 
	{
		//sp5kFsFileClose(fd);
		printf("appCheckFocusMode Success!\n");
		//return TRUE;
	}
	else
	{
		printf("appCheckFocusMode Failed!\n");
		return 0;
	}
	size = sp5kFsFileSizeGet(fd);
	if(size<10||size>18)
	{
		printf("--error file length=%d\n",size);
	}	
	
	len = sp5kFsFileRead(fd, buffer, size);
	if(len<size)
	{
		printf("read no enough byte\n");
	}	

	if(memcmp( "CUID:", buffer, strlen("CUID:"))!=0)  //'0'
	{		
		printf("read no enough byte\n");
		return 0;
	}	
	nIndex=18-1;
	DateCUID.Year=(buffer[5]-'0')*10+(buffer[6]-'0');
	DateCUID.Week=(buffer[7]-'0')*10+(buffer[8]-'0');
	
	nPairRandomCUID=0;
	#if 0
	nPairRandomCUID+=(buffer[nIndex-1]-'0')*10+(buffer[nIndex]-'0');
	nPairRandomCUID+=((buffer[nIndex-3]-'0')*10+(buffer[nIndex-2]-'0'))*100;
	nPairRandomCUID+=((buffer[nIndex-5]-'0')*10+(buffer[nIndex-4]-'0'))*10000;
	#else
	nPairRandomCUID+=(buffer[nIndex-1]-'0')*10+(buffer[nIndex]-'0');
	nPairRandomCUID+=((buffer[nIndex-3]-'0')*10+(buffer[nIndex-2]-'0'))<<8;
	nPairRandomCUID+=((buffer[nIndex-5]-'0')*10+(buffer[nIndex-4]-'0'))<<16;
	#endif
	//109876
	printf("--Year=%d Week=%d CUID=%d 0x%x\n",DateCUID.Year,DateCUID.Week,nPairRandomCUID,nPairRandomCUID);
	sp5kFsFileClose(fd);
}

#define MAX_RT  (1<<4)
#define TX_DS  (1<<5)
#define RX_DR  (1<<6)

int CheckRFStatus(UINT8 rfmode)
{
	RFReadWriteCmd( NRF_RF_STATUS,1);//NOP
	UINT8 sr=(UINT8)ReadPLData[0];
	if(rfmode==MD_TX)
	{
		
		if(sr&MAX_RT)	
		RFTxFlushFIFO();	
	 	
		//RFClearIrq();    
	       // ���TX_DS��MAX_RT�жϱ�־  
	   
	    	if(sr&TX_DS) 
	    	{
			RFRxBurstData();			
			RFRxFlushFIFO();
			RFWriteCmd(NRF_RF_STATUS,sr);
			printf("sr=0x%x\n",sr);
			printf("r=%d send ok\n",(UINT8)ReadPLDataBack[0]);						
			return(0x00);  
	    	}		
	    	else if(sr&MAX_RT) 
	    	{
			printf("max rt 0x%x\n",sr);	
			RFWriteCmd(NRF_RF_STATUS,sr);
	    	}
		else
		{
			if(sr&RX_DR) 
			printf("ack rec ok 0x%x\n",sr);
			RFWriteCmd(NRF_RF_STATUS,sr);

		}
	        	
	}
	else
	{
		if(sr&RX_DR) 
			printf("rec ok\n");	
		RFWriteCmd(NRF_RF_STATUS,0x70);
		RFRxFlushFIFO();
			
	}
	return(0xff);  
	
}


void PTX_Mode(ULONG iIrq)
{
	UINT8 data_buffer;
	UINT8 Buf[32];
	UINT8 i;
	UINT8 IRQ;

	printf("PTX_Mode\n");	
	RFCELOW();
	RFWriteCmd(RF_CH_MAP, PingPongCH);	// RF Channel, Set channel = (2400 + channel) (MHz)
	RFWriteCmd(EN_AA_MAP, 0x01);			// Enable Auto Ack, enable Auto Ack pipe0
	//RFWriteCmd(EN_AA_MAP, 0x00);
	RFWriteCmd(EN_RXADDR_MAP, 0x01); 		// Enable RX Address, enable pipe0 address
	RFWriteCmd(SETUP_AW_MAP, 0x03);		// Setup of Address Width, 5 bytes address width
	RFWriteCmd(RX_PW_P0_MAP, 0x01);		// RX Payload Width, Set number of bytes in RX payload = 1 byte
	RFWriteCmd(RF_SETUP_MAP, 0x06); 		// Data Rate = 1Mbps, RF_PWR = 0dBm
	RFWriteCmd(NRF_FEATURE,0x02);			//luo enable ack payload
	
	i=0;
	Buf[i++]='D';
	Buf[i++]='X';
	Buf[i++]='G';
	Buf[i++]='R';
	Buf[i++]='F';		
	RFSetRxAddr(Buf,1);	
	RFSetTxAddr(Buf,1);

	RFWriteCmd(SETUP_RETR_MAP, 0x25); // Disable re-transmit
	RFWriteCmd(CONFIG_MAP, 0x5E);		// TX_DS reflected IRQ, Set to PTX
	spi_delay_task(2000); 				// Max 1.5ms start-up from power down mode

	RFWriteCmd(STATUS_MAP, 0x70);		// Clear all interupt flag	01110000
	RFTxFlushFIFO();// Flush Tx FIFO
	RFRxFlushFIFO();

	
	
	while(1) {	

		sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 100);
		CheckRFStatus(MD_TX);
		RFTxFlushFIFO(); //Flush_tx_command();		
		Buf[0]='K';
		RFWriteDat(0xA0, Buf, 1);		
		RFWriteCmd(STATUS_MAP, 0x70);	// Clear all interupt flag
		RFCEHIGH();			
		spi_delay_task(20);///10us
		RFCELOW();	
		sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 1);				
	}
	
	
}
void PRX_Mode(ULONG iIrq)
{
	UINT8 data_buffer=0;
	UINT8 Buf[32];
	UINT8 i;
	UINT8 IRQ;
	UINT8  nrArray[32];
	
	
	printf("PRX_Mode\n");
	
	RFCELOW();
	RFWriteCmd(RF_CH_MAP, PingPongCH);	// RF Channel, Set channel = (2400 + channel) (MHz)
	RFWriteCmd(EN_AA_MAP, 0x01);			// Enable Auto Ack, enable Auto Ack pipe0
	RFWriteCmd(EN_RXADDR_MAP, 0x01); 	// Enable RX Address, enable pipe0 address
	RFWriteCmd(SETUP_AW_MAP, 0x03);	// Setup of Address Width, 5 bytes address width
	RFWriteCmd(RX_PW_P0_MAP, 0x01);	// RX Payload Width, Set number of bytes in RX payload = 1 byte
	RFWriteCmd(RF_SETUP_MAP, 0x06); 	// Data Rate = 1Mbps, RF_PWR = 0dBm
	RFWriteCmd(NRF_FEATURE,0x02);
	i=0;
	Buf[i++]='D';
	Buf[i++]='X';
	Buf[i++]='G';
	Buf[i++]='R';
	Buf[i++]='F';
	RFSetTxAddr(Buf,1);
	RFSetRxAddr(Buf,1);

	RFWriteCmd(CONFIG_MAP, 0x3F);
	
	sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 3);

	RFWriteCmd(STATUS_MAP, 0x70);	// Clear all interupt flag	
	RFRxFlushFIFO(); 
		
	RFCEHIGH();
	spi_delay_task(1);

	while(1) {

			sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 100);			
			if(GetRF_IRQLevel()==0)
			{				
				RFRxBurstData();	
				//printf("K=[%c]\n",(UINT8)ReadPLDataBack[0]);
				nrArray[0]='R' ;
				RFWriteDat(NRF_W_ACK_PAYLOAD|RF_PIPE_0,nrArray,1 );
				CheckRFStatus(MD_RX);
			}
		
	}		
}


void 
inRF2401_monCmd(UINT32 argc,	UINT8 *arg[], UINT32 v[])
{
	UINT32 addr,dat,len,cmd;
	UINT8 Buf[32];
	UINT8 i;

	if (argc >= 2 && strcmp(arg[1], "spi") == 0) {
	}
	else if (argc >= 2 && strcmp(arg[1], "spi_cfg") == 0) {

		//dat = atoi(v[2]);  
		dat=v[2];
		if(dat)
		{			
			nRFSpiMode=HWSPI;			
		}
		else
		{
			nRFSpiMode=GPIOSPI;			
		}
		printf("spi gpio=%d\n",nRFSpiMode);
	}	
	else if (argc >= 2 && strcmp(arg[1], "init") == 0) 
	{
		dat = v[2];  //mode  0123	
		nRFMode=dat;
		if(nRFMode) 
		{
			printf("Init TX mode...=%d\n",nRFMode);
		}
		else
		{
			printf("Init RX mode...=%d\n",nRFMode);
		}
		//vdxgapp_nRF2401_Init();        
	}	
	
	return ;
}
#endif



//////////////////////////////////////////////////////////////////////////////////////
//									RF App Functon									//
//////////////////////////////////////////////////////////////////////////////////////

#define LOGI_UNCACH_BADDR_TO_LOGI_CACH_BADDR(addr) 	((void *)((UINT32)(addr) & ~0x20000000))
//#define USE_VIDEO_DUAL_STREAM

static sp5kGfxObj_t   SrcGfxObj={0,},RfYuvGfxObj={0},CopyGfxObj={0}, RfJfifgfxObj={0};

void appRfBufferInit(void)
{
	

	if(SrcGfxObj.pbuf!=NULL|| CopyGfxObj.pbuf != NULL ||RfYuvGfxObj.pbuf  != NULL||RfJfifgfxObj.pbuf != NULL)
		return;

	
	SrcGfxObj.pbuf = sp5kYuvBufferAlloc(320, 240);
	SrcGfxObj.fmt = SP5K_GFX_FMT_YUV422;
	SrcGfxObj.bufW = 320;
	SrcGfxObj.bufH  = 240;
	SrcGfxObj.roiX = 0;
	SrcGfxObj.roiY  = 0;
	SrcGfxObj.roiW = 320;
	SrcGfxObj.roiH  = 240;
	//Copy, default 128 X 96
	
	CopyGfxObj.pbuf = sp5kYuvBufferAlloc(320,240);
	CopyGfxObj.fmt = SP5K_GFX_FMT_YUV422;
	CopyGfxObj.bufW = 320;
	CopyGfxObj.bufH  = 240;
	CopyGfxObj.roiX = 0;
	CopyGfxObj.roiY  = 0;
	CopyGfxObj.roiW = 320;
	CopyGfxObj.roiH  = 240;
	///sp5kGfxObjectFill(&CopyGfxObj, 0x6b1dff);	//UYV888, blue

	//RfYuv, 128 X 96
	RfYuvGfxObj.pbuf = sp5kYuvBufferAlloc(128, 96);
	RfYuvGfxObj.fmt = SP5K_GFX_FMT_YUV422;
	RfYuvGfxObj.bufW = 128;
	RfYuvGfxObj.bufH  = 96;
	RfYuvGfxObj.roiX = 0;
	RfYuvGfxObj.roiY  = 0;
	RfYuvGfxObj.roiW = 128;
	RfYuvGfxObj.roiH  = 96;
	///sp5kGfxObjectFill(&RfYuvGfxObj, 0x800080);	//UYV888, black

	//RfJfif, 128 X 96
	RfJfifgfxObj.pbuf = sp5kYuvBufferAlloc(128, 96);
	RfJfifgfxObj.fmt = SP5K_GFX_FMT_JFIF;
	RfJfifgfxObj.bufW = 128;
	RfJfifgfxObj.bufH  = 96;
	RfJfifgfxObj.roiX = 0;
	RfJfifgfxObj.roiY  = 0;
	RfJfifgfxObj.roiW = 128;
	RfJfifgfxObj.roiH  = 96;
}



void appRfBufferRelease(void)
{
	
	if(SrcGfxObj.pbuf != NULL)
	{
		sp5kYuvBufferFree(SrcGfxObj.pbuf);
		SrcGfxObj.pbuf = NULL;
	}

	if(CopyGfxObj.pbuf != NULL)
	{
		sp5kYuvBufferFree(CopyGfxObj.pbuf);
		CopyGfxObj.pbuf = NULL;
	}

	if(RfYuvGfxObj.pbuf!=NULL)
	{
		sp5kYuvBufferFree(RfYuvGfxObj.pbuf);
		RfYuvGfxObj.pbuf = NULL;
	}
	
	if(RfJfifgfxObj.pbuf != NULL)
	{
		sp5kYuvBufferFree(RfJfifgfxObj.pbuf);
		RfJfifgfxObj.pbuf = NULL;
	}

}
UINT8 nPreviewSync=0;
void appRfBufferConvert(void)
{

	static UINT32 fullWidth = 16, fullHeight = 9;
	static UINT8 nFirst=1;
	appGetViewSize(&fullWidth, &fullHeight);//
	//printf("fullWidth = %d, fullHeight = %d\n", fullWidth, fullHeight);

	if( fullWidth * 3 == fullHeight * 4)	// 4:3
	{
		RfYuvGfxObj.roiX = 0;
		RfYuvGfxObj.roiY  = 0;	
		RfYuvGfxObj.roiW = 128;
		RfYuvGfxObj.roiH  = 96;
		#if 1
		RfJfifgfxObj.bufW=128;
		RfJfifgfxObj.bufH=96;
		RfYuvGfxObj.roiX = 0;
		RfYuvGfxObj.roiY  = 0;	
		RfJfifgfxObj.roiW = 128;
		RfJfifgfxObj.roiH  = 96;
		#endif		
		///sp5kGfxObjectScale(&CopyGfxObj, &RfYuvGfxObj);

		
	}
	else		// 16:9
	{
		RfYuvGfxObj.roiX = 0;
		RfYuvGfxObj.roiY  = 0;	
		RfYuvGfxObj.roiW = 128;
		RfYuvGfxObj.roiH  = 96;
		//sp5kGfxObjectFill(&RfYuvGfxObj, 0x800080);	//UYV888, black

		RfYuvGfxObj.roiX = 0;
		RfYuvGfxObj.roiY  = 12;
		RfYuvGfxObj.roiW = 128;
		RfYuvGfxObj.roiH  = 72;
		//sp5kGfxObjectScale(&CopyGfxObj, &RfYuvGfxObj); //fail???
		#if 1
		RfJfifgfxObj.bufW=128;
		RfJfifgfxObj.bufH=72;
		RfYuvGfxObj.roiX = 0;
		RfYuvGfxObj.roiY  = 0;	
		RfJfifgfxObj.roiW = 128;
		RfJfifgfxObj.roiH  = 72;
		#endif		
	}

	RfYuvGfxObj.roiX = 90;	//for set JPG qtable idx
	if(nFirst==1)
	{
		//printf("get yuv data\n");
		//fsSimpleWrite("D:\\frame128x96.yuv", RfYuvGfxObj.pbuf, 128*96*2);
		nFirst=0;	
	}
	
	//sp5kGfxObjectConvert(&RfYuvGfxObj, &RfJfifgfxObj);
	RfYuvGfxObj.roiX = 0;
	RfYuvGfxObj.roiY  = 12;
	RfYuvGfxObj.roiW = 128;
	RfYuvGfxObj.roiH  = 72;
	
	CopyGfxObj.roiX= 30;     //     1=800B  12=1.3k  15=1.5K  20  1.8K 30  2k 

	//CopyGfxObj.roiX = 0;
	CopyGfxObj.roiY  = 0;
	CopyGfxObj.roiW = 320;
	CopyGfxObj.roiH  = 240;

	if(nPreviewSync==1)
	{
		sp5kGfxObjectConvert(&CopyGfxObj, &RfJfifgfxObj);
		//printf("--preview jpg data sizexx=%d\n",RfJfifgfxObj.roiX);
		nPreviewSync=0;
	}	


}

UINT8 RFRecSnapMutex=0;
void RFRecCaptureWaitSetting()
{
	RFRecSnapMutex=1;
	sp5kTimeDelay(100, 1);

}
void RFRecCaptureWaitRelease()
{
	RFRecSnapMutex=0;
	//sp5kTimeDelay(100, 1);

}


extern void appRfCdvLapseBufferInit();
extern sp5kGfxObj_t SrcCdvLapseGfxObj,  Src16X9CdvLapseGfxObj,RfCdvLapseJfifgfxObj;
#define ROUND_DOWN_TO_DIVISIBLE(num,div) 		((UINT32)(num) & -(UINT32)(div))
#define ROUND_UP_TO_DIVISIBLE(num,div) 		ROUND_DOWN_TO_DIVISIBLE((UINT32)(num) + (div) - 1, div)
UINT32 appRfCopyBufferCallback(UINT32 fid, UINT8 *pbuf, UINT32 w, UINT32 h)
{
		
		//printf("fid = %d, w = %d, h = %d RFRecSnapMutex=%d\n", fid, w, h,RFRecSnapMutex);
		
		if(CopyGfxObj.pbuf == NULL||nWifiRFMutex==1||(fid%FrameFreqencyFactor())!=0||RFRecSnapMutex==1)		
			return 1;
				

	if(w==320)
	{
			#if 1
			if(nPreviewSync==0)
			{
				SrcGfxObj.pbuf = pbuf;
				SrcGfxObj.fmt = SP5K_GFX_FMT_YUV422;
				SrcGfxObj.bufW = w;
				SrcGfxObj.bufH  = h;
				SrcGfxObj.roiX = 0;
				SrcGfxObj.roiY  = 0;
				SrcGfxObj.roiW = w;
				SrcGfxObj.roiH  = h;
				
				if(CopyGfxObj.bufW != w || CopyGfxObj.bufH != h)
				{					
					if(CopyGfxObj.pbuf != NULL)
					{
						sp5kYuvBufferFree(CopyGfxObj.pbuf);
					}
					CopyGfxObj.pbuf = sp5kYuvBufferAlloc(w, h);
				}
				CopyGfxObj.fmt = SP5K_GFX_FMT_YUV422;
				CopyGfxObj.bufW = w;
				CopyGfxObj.bufH  = h;
				CopyGfxObj.roiX = 0;
				CopyGfxObj.roiY  = 0;
				CopyGfxObj.roiW = w;
				CopyGfxObj.roiH  = h;				
				//sp5kGfxObjectCopy(&SrcGfxObj, &CopyGfxObj);
				memcpy( CopyGfxObj.pbuf,pbuf,w*h*2);//128*96
				nPreviewSync=1;

			}
			
			#else
				//if(CopyGfxObj.pbuf!=NULL)
				memcpy( CopyGfxObj.pbuf,pbuf,w*h*2);//128*96
			#endif

		}	
		else
		{

			
			if(h==1072)
				return 0;
			if(nWifiRFMutex==1&&nCdvLapseWifiRFMutex==1)
				return 0;
			if(appRFSkipFunction()==1)
				return 0;
			if((fid%FrameFreqencyFactor())!=0)
				return 1;
			//printf("--rec 1080p Record\n");
			
			SrcCdvLapseGfxObj.pbuf = pbuf;
			SrcCdvLapseGfxObj.fmt = SP5K_GFX_FMT_YUV422;

			if(w==1920)
			{
				SrcCdvLapseGfxObj.bufW = 1920;
				SrcCdvLapseGfxObj.bufH  = 1088;
			}
			else
			{
				SrcCdvLapseGfxObj.bufW = ROUND_UP_TO_DIVISIBLE(w, 16);
				SrcCdvLapseGfxObj.bufH  = ROUND_UP_TO_DIVISIBLE(h, 16);
			}
			SrcCdvLapseGfxObj.roiX = 0;		
			SrcCdvLapseGfxObj.roiY  = 0;		
			SrcCdvLapseGfxObj.roiW = w;

			if(w==1920)	
				SrcCdvLapseGfxObj.roiH  = 1088;
			else
				SrcCdvLapseGfxObj.roiH  = h;	
			
			SrcCdvLapseGfxObj.roiX = 30;	//for set JPG qtable idx

			//printf("--bufW=%d bufH=%d\n",SrcCdvLapseGfxObj.bufW,SrcCdvLapseGfxObj.bufH);
			
			//return;
			#if 1
			if( app_mjpeg_fifo_is_full() )
			{
				//printf("lapse jpg fifo full.\n");
				return 0;
			}
			#endif
			if(w * 3 == h * 4)		
			{

				//printf("--ratio 4:3\n");
				RfCdvLapseJfifgfxObj.bufW=128;
				RfCdvLapseJfifgfxObj.bufH=96;
				RfCdvLapseJfifgfxObj.roiX = 0;
				RfCdvLapseJfifgfxObj.roiY  = 0;
				RfCdvLapseJfifgfxObj.roiW = 128;
				RfCdvLapseJfifgfxObj.roiH  = 96;			
				
				sp5kGfxObjectConvert(&SrcCdvLapseGfxObj, &RfCdvLapseJfifgfxObj);
			}
			else
			{
				//printf("--ratio 16:9\n");

				//sp5kGfxObjectScale(&SrcCdvLapseGfxObj, &Src16X9CdvLapseGfxObj);

				RfCdvLapseJfifgfxObj.bufW=128;
				RfCdvLapseJfifgfxObj.bufH=72;
				RfCdvLapseJfifgfxObj.roiX = 0;
				RfCdvLapseJfifgfxObj.roiY  = 0;
				RfCdvLapseJfifgfxObj.roiW = 128;
				RfCdvLapseJfifgfxObj.roiH  = 72;
				
				Src16X9CdvLapseGfxObj.roiX = 90;
				//sp5kGfxObjectConvert(&Src16X9CdvLapseGfxObj, &RfCdvLapseJfifgfxObj);
				sp5kGfxObjectConvert(&SrcCdvLapseGfxObj, &RfCdvLapseJfifgfxObj);
				
			}
			//return;
			if(RfCdvLapseJfifgfxObj.roiX<3*1024)
			app_mjpeg_fifo_write(RfCdvLapseJfifgfxObj.pbuf, RfCdvLapseJfifgfxObj.roiX);
			//printf("--Lapsexx Jpg Success =%d\n",RfCdvLapseJfifgfxObj.roiX);

		}

		return 1;
}

void appRfCopyBufferDirect(UINT8 *fileName)
{

	UINT32 w = 128, h = 96;		

	if(CopyGfxObj.pbuf == NULL)
	{
		return;
	}
	
	CopyGfxObj.fmt = SP5K_GFX_FMT_YUV422;
	CopyGfxObj.bufW = w;
	CopyGfxObj.bufH  = h;
	CopyGfxObj.roiX = 0;
	CopyGfxObj.roiY  = 0;
	CopyGfxObj.roiW = w;
	CopyGfxObj.roiH  = h;
	sp5kGfxObjectFill(&CopyGfxObj, 0x800080);	//0x6b1dff UYV888, blue black 

	appRfBufferConvert();
}


void appRfCopyBufferCallbackSet(void)
{
	
	if(appRFSkipFunction()==TRUE)
		return;
	
	sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 50);
	switch(appActiveStateGet())
	{
		case APP_STATE_SPORTDV_VIDEO_REC:
		case APP_STATE_SPORTDV_VIDEO_REC_SLOW:
		case APP_STATE_CARDV_VIDEO_REC:
		case APP_STATE_CARDV_VIDEO_REC_LAPSE:			
			//sp5kPreviewCallbackSet(1, NULL,NULL,NULL);
			//sp5kVideoUrgentCallbackSet(1, appRfCopyBufferCallback);
			break;
		case APP_STATE_SPORTDV_VIDEO_PREVIEW:
		case APP_STATE_CARDV_VIDEO_PREVIEW:
			///sp5kVideoUrgentCallbackSet(0, NULL);
			sp5kPreviewCallbackSet(1, appRfCopyBufferCallback,320,240);
			break;
		case APP_STATE_SPORTDV_STILL_PREVIEW:
		case 0x112:
		case 0x113:
		case 0x114:
		case 0x115:
			///sp5kVideoUrgentCallbackSet(0, NULL);			
			sp5kPreviewCallbackSet(1, appRfCopyBufferCallback,320,240);
			break;
		default:
			//appRfCopyBufferDirect("A:\\UI\\JPG\\VIDEO.JPG");
			break;
	}

	
	sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 50);
}

void appRfCopyBufferCallbackRelease(void)
{
	///sp5kVideoUrgentCallbackSet(0, NULL);
	//sp5kPreviewCallbackSet(1, NULL,320,240);
	//sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 500);
}
void appRfCdvLapseCallbackRelease(void)
{	
	
	sp5kPreviewCallbackSet(1, NULL,320,240);
	sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 1500);

	
}

void appRfCdvLapseWIFICallbackRelease(void)
{	
	sp5kPreviewCallbackSet(1, NULL,320,240);
	sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 1500);
}

extern int  nWifiRFMutex;
UINT8 nModeChg=0;
void  appUpdateBufferThread(ULONG para)
{
	static SINT32 curr_tick=0;
	static SINT32 last_tick=0;
	static UINT32  nFrameEnd=1;
	static UINT32 nStillFrameNum=0;

	printf("appUpdateBufferThread run\n");

	while(1)
	{
		sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 100);
		//continue;
		if(nFrameStart == 1)
		{
			curr_tick=app_nrf_sys_ticks();			
			if(nFrameEnd==1)
			{
				last_tick=curr_tick;
				nFrameEnd=2;
			}	
		}
		
		if( (curr_tick-last_tick) > 5*100) // 1s		
		{
			last_tick=curr_tick;
			//printf("---fr=%d fw=%d\n",nFrameRead,nFrameWrite);
		}
		
		
 		if( app_mjpeg_fifo_is_full() )
		{			
			continue;
		}
				
		UINT32 tCurMode;
		sp5kModeGet(&tCurMode);	
		if(tCurMode==SP5K_MODE_VIDEO_PREVIEW||tCurMode==SP5K_MODE_STILL_PREVIEW)			
		{
			
			
			if(nWifiRFMutex!=1)
			{

				if(tCurMode==SP5K_MODE_VIDEO_PREVIEW)
				{
					nStillFrameNum=0;
					appRfBufferConvert();
				}	
				else if(tCurMode==SP5K_MODE_STILL_PREVIEW)
				{
					nStillFrameNum++;
					if(nStillFrameNum<=4)
						nPreviewSync=0;					
					appRfBufferConvert();					
				}					
				nFrameWrite++;
				if(RfJfifgfxObj.roiX<3*1024)
				app_mjpeg_fifo_write(RfJfifgfxObj.pbuf, RfJfifgfxObj.roiX);
				//printf("rf jpg send success size=%d.\n",RfJfifgfxObj.roiX);
			}
		}
		
			
		
	}

	appRfBufferRelease();
	
}
static UINT32 RCW5k8ScanTimer = 0;
void appRfInit(UINT8 CUID[5],UINT8 nRcwMID)
{
		
	//printf("--appRfInit npd: %d\n", nRcwMID);
	
	///appRfRelease();	
	g_nRcwMID=nRcwMID;
	
	if(CUID)
	{
		RFRemoterSetCUID(CUID,5);
	}
	
	#if 0
		nRFSpiMode = GPIOSPI;
		nRFMode=MD_RX;
	#else
		nRFSpiMode = HWSPI;
		nRFMode=MD_RX;
	#endif	
	
	nRFParingMode =0;
	nPairAddrEnable = 0;
	nRF2401_MemoryInit();
	MJPGFifoCreate();
	//appRfBufferRelease();
	appRfBufferInit();	
	//appRfCopyBufferDirect("A:\\UI\\JPG\\VIDEO.JPG");	

	IsrAndSemphoreInit();
		
	RF2401Init();
	spi_delay_task(50);	
	
	//cmdBatchRegister(rfspicmds);
	//if(g_nRcwMID == RCW5K8)
	{
		//appRfRelease();
		//sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 500);
	RCW5k8ScanTimer=appTimerIsrReg(RFRxTimerCallBack,RF2401_RF_RX_SCAN_RATE/10); 
		
	}
	#if 0
	else
	{

		appTimerIsrUnreg(&RCW5k8ScanTimer);
		sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 500);
		
		if(pThreadRFReceive==NULL)
			pThreadRFReceive= sp5kOsThreadCreate("RFRxThreadCallBack", RFRxThreadCallBack, 0, 16, 16, 3, TX_AUTO_START);
		
		if(g_nRcwMID == RCWD01)
		{
			if(pAppUpdateBufferThread ==NULL)
				pAppUpdateBufferThread = sp5kOsThreadCreate("appUpdateBufferThread", appUpdateBufferThread, 0, 16, 16, 3, TX_AUTO_START);	
		}
	}	
	sp5kGpioIntEnable(SP5K_GPIO_INT11,SP5K_TRIG_LVL_FALL);
	#endif	
	spi_delay_task(100);	
	RFCELOW();	
	spi_delay_task(1);
	RFWriteCmd(NRF_EN_RXADDR,0x01);
	spi_delay_task(1);
	RFCEHIGH();
	
}

void appRfRelease(void)
{

	if(pThreadRFReceive != NULL)
	{
			sp5kOsThreadTerminate(pThreadRFReceive);
			sp5kOsThreadDelete(pThreadRFReceive);
			pThreadRFReceive = NULL;
			//printf("rf Receive thd destroy\n");
	}
	
	if(g_nRcwMID == RCWD01)
	{
		if(pAppUpdateBufferThread != NULL)
		{
			sp5kOsThreadTerminate(pAppUpdateBufferThread);
			sp5kOsThreadDelete(pAppUpdateBufferThread);
			pAppUpdateBufferThread = NULL;
			//printf("rf UpdateBuffer  thd destroy\n");
		}	
	}
	sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 100);
}

void appRfCUIDGenerator()
{
	tmx_t rtcDateTime;
	
	sp5kRtcDateTimeGet(SP5K_DATE_TIME_OPTION ,&rtcDateTime);			
	DateCUID.Year = (rtcDateTime.tmx_year-2000);
	DateCUID.Week = rtcDateTime.tmx_mday;	

	//nPairRandomCUID=0x00<<16|0x09<<8|0x0a;
	nPairRandomCUID = 0;
	nPairRandomCUID =  rtcDateTime.tmx_hour<<16;
	nPairRandomCUID += rtcDateTime.tmx_min<<8;
	nPairRandomCUID += rtcDateTime.tmx_sec;

	//printf("UI year=%d week=%d %d\n",DateCUID.Year,DateCUID.Week,nPairRandomCUID);
}

void appRfPairingStart(void)
{
	appRfCUIDGenerator();//cuid generator
	nRFParingMode=1;
	nPairAddrEnable=1;	
	//printf("---nRFParingMode=%d\n",nRFParingMode);
}	

BOOL appRfParingFinish(UINT8 CUID[5], UINT8* nRcwMID)
{
	UINT8 i=0;
	//printf("---nRFParingStatus=%d\n",nRFParingMode);
	if(nRFParingMode)
	{
		return 0;
	}	
	else
	{
		CUID[i++]= DateCUID.Year;
		CUID[i++]= DateCUID.Week;
		CUID[i++]= (UINT8)(((nPairRandomCUID>>16)&0x00ffL)  );
		CUID[i++]= (UINT8)((nPairRandomCUID>>8)&0x00ffL);
		CUID[i++]= (UINT8)(nPairRandomCUID&0x00ffL);
		
		*nRcwMID = g_nRcwMID;
	}		
	return 1;
}	
//////////////////////////////////////////////////////////////////////////////////////
//									RF App Functon									//
//////////////////////////////////////////////////////////////////////////////////////

//RF dual stream test for function
#define RF_DUAL_STREAM  (2)
#define DUALSTREAM_SAMPLE_JPG_COMP_RATIO   (30) ///1=700  15=1.5k  30=2.0k

static UINT8 *mpszDualStreamDstFn  = "D:\\DUAL0000.JPG";
TX_EVENT_FLAGS_GROUP mDualStreamEvtFlag;
static UINT32 mDualStreamThrdHandle;
static UINT32 mLastJfifAddr;
static UINT32 mLastJfifSize;


//30/3=10
void RFDualStreamYuvCallBack(UINT32 idx,UINT32 pbuf,UINT32 width,UINT32 hight)
{
	UINT32 size=0;
	UINT32 tCurMode;

	sp5kModeGet(&tCurMode);
	
	static SINT32 curr_tick=0;
	static SINT32 last_tick=0;
	static UINT32  nFrameEnd=1;
	//printf("fidstreamx=%d \n", idx);		
	//����֡�ʷ�Ƶ
	//if(fid>=1)
	//return 0;

	if((idx%FrameFreqencyFactor())!=0)
		return ;
	if(nFrameStart==1)
	{
			curr_tick=app_nrf_sys_ticks();			
			if(nFrameEnd==1)
			{
				last_tick=curr_tick;
				nFrameEnd=2;
			}	
		}
		if( (curr_tick-last_tick) > 5*100) // 5s		
		{
			last_tick=curr_tick;
			//printf("-rfr=%d rfw=%d\n",nFrameRead,nFrameWrite);
	}
		
	if (tCurMode==SP5K_MODE_VIDEO_RECORD)
	{
								
		
		UINT32 ret = FAIL;
		UINT32 tIdx = 0;
		UINT32 tAddr = 0;
		
			ret = sp5kMediaRecDualStreamGetStream(&tIdx, &tAddr, &size);
			if ((ret==SUCCESS)&& (size!=0)) 
			{

				mLastJfifAddr = tAddr;
				mLastJfifSize = size;				
				//sp5kOsEventFlagsSet(&mDualStreamEvtFlag, 0x0001, TX_OR);
			}
			else 
			{
				//printf("sp5kMediaRecDualStreamGetStream() return NG (0x%x)\n", ret);
				return ;
				
			}	
			//printf("--rec size=%d\n",mLastJfifSize);
			if( app_mjpeg_fifo_is_full() )
			{													
					return;		
			}
			else
			{			
				#if 1
				
					//app_mjpeg_fifo_write(tAddr ,tSize);						
					if(mLastJfifSize<3*1024)
					{
															
						halDmaMemcpy(pWriteNode->nImageBuffer+4,  (void *)mLastJfifAddr, mLastJfifSize);			
						memcpy(pWriteNode->nImageBuffer,(UINT8*)&mLastJfifSize,4);
						sp5kOsMutexGet(&fdev.sRFFIFOMutex, 0);						
						pWriteNode->nImageFill=1;
						pWriteNode=pWriteNode->pNext;
						sp5kOsMutexPut(&fdev.sRFFIFOMutex);
						nFrameWrite++;
					}
				
				#endif
			}		
					
	}
}



#if 0
void RFDualStreamSampleThrd(ULONG para)
{
	
	ULONG  ulActualEvents;
	UINT32 tSize;
	UINT8  *tAddr=NULL;
	para = para;
	printf("vidoe run\n");
	while( 1 ) 
	{
		#if 0
		sp5kOsEventFlagsGet( &RF_JPG,
								0x0001,
								TX_OR_CLEAR,
								&ulActualEvents,
								TX_WAIT_FOREVER
								);
		 #endif
		sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 100);
		continue;
		tSize = mLastJfifSize;
		tAddr = (UINT8 *)mLastJfifAddr;
		
		if( app_mjpeg_fifo_is_full() )
		{
								
				//printf("ds thrd\n");
				continue;		
		}
		else
		{			
							
				if(tSize<6*1024)
				{
					//sp5kOsMutexGet(&fdev.sRFFIFOMutex, 0);					
					pWriteNode->nImageFill=1;			
					halDmaMemcpy(pWriteNode->nImageBuffer+4,  (void *)tAddr, tSize);			
					memcpy(pWriteNode->nImageBuffer,(UINT8*)&tSize,4);			
					pWriteNode=pWriteNode->pNext;
					//sp5kOsMutexPut(&fdev.sRFFIFOMutex);
					nFrameWrite++;
				}
				
			
		}
		
	}


}
#endif


#define SP5K_RES_TYPE_JPG 0
void RFDualStreamSampleInit(void)
{
		UINT k;
		//printf("%s Line %d. %s()\n", __FILE__, __LINE__, __func__);		

		
		if(nWifiRFMutex==1 ||appRFSkipFunction()==1)
		{
			printf("--RF Dual Sream Stop\n");
			return;
		}	
		else
			printf("--RF Dual Sream Start\n");
		RFRecCaptureWaitSetting();
		//sp5kOsEventFlagsCreate(&mDualStreamEvtFlag, "DualStreamEvt");
		//mDualStreamThrdHandle = (UINT32)sp5kOsThreadCreate("DualStreamThrd", RFDualStreamSampleThrd, 0, 17, 17, 0, TX_AUTO_START);
		appRfCdvLapseBufferInit();
		sp5kVideoUrgentCallbackSet(1, appRfCopyBufferCallback);
		#if 0
		sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_VIDEO_CODEC, SP5K_MEDIA_VIDEO_H264);

		printf("ratio is %d\n",appSdvVideoRecordRatioGet());
		if(appSdvVideoRecordRatioGet()==4)
		{
			sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_DUALSTREAM_WIDTH, 128);
			sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_DUALSTREAM_HEIGHT, 96);
		}
		else if(appSdvVideoRecordRatioGet()==9)
		{
			sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_DUALSTREAM_WIDTH, 128);
			sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_DUALSTREAM_HEIGHT, 72);
		}
					
		sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_DUALSTREAM_Q_FACTOR, DUALSTREAM_SAMPLE_JPG_COMP_RATIO);
		sp5kMediaRecCfgSet(MEDIA_REC_DUALSTREAM_BUF_SIZE, (50*1024));
		sp5kMediaRecCfgSet(MEDIA_REC_DUALSTREAM_BUF_CNT, (10));
		sp5kMediaRecCfgSet(SP5K_MEDIA_REC_DUALSTREAM_JFIF_EN, 1);		
		//30fps
		sp5kVideoUrgentCallbackSet(1, RFDualStreamYuvCallBack);
		#endif		
	

}


void RFDualStreamSampleDeinit(void)
{
	UINT32 ulActualEvents;
	UINT32 k;

	printf("%s Line %d. %s()\n", __FILE__, __LINE__, __func__); 
	if(nWifiRFMutex==1 ||appRFSkipFunction()==1)
			return;
	
	sp5kVideoUrgentCallbackSet(0, NULL);
	
	//sp5kOsThreadDelete((TX_THREAD *)mDualStreamThrdHandle);
	//mDualStreamThrdHandle = 0;
	//sp5kOsEventFlagsDelete(&mDualStreamEvtFlag);
	//mDualStreamEvtFlag = 0;

	//5kMediaRecCfgSet(SP5K_MEDIA_REC_DUALSTREAM_JFIF_EN, 0);	

	sp5kVideoUrgentCallbackSet(1 , NULL);
	
	sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 500);
	
	

}

//RF dual stream test for end

//////////////////////////////////////////////////////////////////////////////////////
//									RF CE/FCC TEST									//
//////////////////////////////////////////////////////////////////////////////////////

static UINT32 RfTestTimer = 0;

void RfTestTimerIsr(void)
{
	RFReSendCode();

	static UINT32 i = 0;
	if((i % 100) == 0)
	{
		printf("nRF2401 Send code %d Times\n", i);
	}
	i ++;
}

void RfTestStop(void)
{
	if(RfTestTimer != 0)
	{
		appTimerIsrUnreg(&RfTestTimer);
	}
}

//ct:0-->disable continuous carrier, 1-->enable continuous carry
//ch[0~79], example: 3 43 79 mean which freq to test...... 
//db:0-->-18db, 1-->-12db, 2-->-6db, 3-->0db
//rate:0-->1Mbps, 1-->2Mbps, 2-->250kbps, 3-->Reserved
void RfTestTx(int ct, int ch, int db, int rate)
{
	printf("%s() input ct = %d, ch=%d, db = %d, rate = %d\n",__func__, ct, ch, db, rate);

	UINT16 RegSetup = 0;

	RegSetup = (ct & 0x01) << 7;
	RegSetup |= (db & 0x01) << 2;
	RegSetup |= (rate & 0x01) << 3;
	RegSetup |= (rate & 0x02) << 5;	
	RegSetup |= (0x1) << 4;	
	printf("RegSetup = 0x%02x\n", RegSetup);
	
	appRfRelease();
	RfTestStop();
	
	RFCELOW();
	spi_delay_task(15);
	RFWriteCmd(NRF_RF_CONFIG, 0x5c);
	spi_delay_task(15);
	RFWriteCmd(NRF_RF_CONFIG, RF_TRANSMIT_MODE);
	spi_delay_task(15);
	RFWriteCmd(NRF_RF_SETUP, RegSetup);
	spi_delay_task(15);
	RFWriteCmd(NRF_RF_CH, (UINT16)ch);
	spi_delay_task(15);
	RFCEHIGH();
	
	RfTestTimer = appTimerIsrReg(RfTestTimerIsr, 50/appTimerIsrTickIntervalGet());

	return;
}

void RfTestRx(int ch)
{
	printf("%s() input Ch=%d",__func__,ch);

	appRfRelease();
	RfTestStop();
	
	RFCELOW();
	spi_delay_task(15);
	RFWriteCmd(NRF_RF_CONFIG, RF_RECEIVE_MODE);
	spi_delay_task(15);
	RFWriteCmd(NRF_RF_CH, (UINT16)ch);
	spi_delay_task(15);
	RFCEHIGH();
}

void cmdRf(	UINT32 argc, UINT8 *arg[], UINT32 v[])
{
	if (argc==1) 
	{
		printf(	" Rf cmd:\n"
				" RfInit:rfType, 0-->RCW5K8, 1-->RCWD01\n"
				" RfTestTx:ct ch db rate, Rf Test begain\n"
				" RfTestRx:ch\n"
				" RfTestStop:stop test\n");
	} 
	else if (strcmp(arg[1], "RfInit")==0) 
	{
		printf("RfInit(%d)\n", v[2]);
		appRfInit(NULL, v[2]);
	} 
	else if (strcmp(arg[1], "RfTestTx")==0) 
	{
		printf("RfTestTx(%d, %d, %d, %d)\n", v[2], v[3], v[4], v[5]);
		RfTestTx(v[2], v[3], v[4], v[5]);
	} 
	else if (strcmp(arg[1], "RfTestRx")==0)
	{
		printf("RfTestRx(%d)\n", v[2]);
		RfTestRx(v[2]);
	} 
	else if (strcmp(arg[1], "RfTestStop")==0) 
	{
		printf("RfTestStop\n");
		RfTestStop();
	}	
}

//////////////////////////////////////////////////////////////////////////////////////
//									RF CE/FCC TEST									//
//////////////////////////////////////////////////////////////////////////////////////
