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
#include "common.h"
#include "api/sp5k_utility_api.h"
#include "api/sp5k_global_api.h"
#include "app_gSensor.h"

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
#define GSENSOR_SLAVE_ADDR	    0x3a
#define GSENSOR_FERQUENCY_I2C   SP5K_I2C_CLK_DIVISOR_1024
#define EN_16BIT_ADDR  			0
#define READ_MODE	   			0x04


#define REG_ADDR_BIT 			8
#define REG_DATA_BIT			8
#define NBIT_UNIT				8


#define CS_PIN_GRP_ID	    	SP5K_GPIO_GRP_LMI	/* SPI CHIP SELECT GPIO GROUP ID */
#define CS_PIN_MASK     		(1 << 33)
#define GSENSOR_FERQUENCY_SPI   SP5K_SPI_PORT2_CLK_6MHZ
#define MODE_SPI	   		 	0x03

/* device name */
static UINT8 gSensorName[] = "G-Sensor EWTS9P";

/* Register Initialization Table	*/
UINT8 regInitTbl_01[] = {		/* {ADDR, DATA} */
};

#define DEFAULT_MODE   regInitTbl_01

#define GSENSOR_CMD_ANGULAR_RATE 0xff
#define WR_CNT     6

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

static void gSensorDriverConfig(void);

void  
gSensorDriverInit(
	UINT8 mode
)
{
	UINT8  index = 0;

	/* config control mode */
	gSensorDriverConfig();
  
	/* write init table to device ,init table format should be:first addr then data*/
	if(mode == GSENSOR_NORMAL_INIT)
	{
		for(index = 0; index < sizeof(DEFAULT_MODE)/sizeof(DEFAULT_MODE[0]); index += 2){
			gSensorDriverWrite(DEFAULT_MODE[index], DEFAULT_MODE[index + 1],(REG_DATA_BIT/NBIT_UNIT));
		}
	}
}

void 
gSensorDriverWrite(
	UINT32 addr,
	UINT32 data,
	UINT32 cnt
)
{
	#if   GSENSOR_INTERFACE_I2C        /*I2c use port LMI8 and LMI9*/
	sp5kI2cWriteMulti((addr),(&data),(cnt));

	#elif GSENSOR_INTERFACE_SPI_PORT2  /*SPI port 2*/
	sp5kGpioWrite(CS_PIN_GRP_ID, CS_PIN_MASK, 0);\
	sp5kSpiWrite (SP5K_SSC_PORT_2, (UINT8 *)&(addr), REG_ADDR_BIT * cnt);\
	sp5kSpiWrite (SP5K_SSC_PORT_2, (UINT8 *)&(data), REG_DATA_BIT * cnt);\
	sp5kGpioWrite(CS_PIN_GRP_ID, CS_PIN_MASK, CS_PIN_MASK);
	
	#else  					/*SPI port 3*/	
	#define SERIAL_WRITE(addr,data,cnt) \
	sp5kGpioWrite(CS_PIN_GRP_ID, CS_PIN_MASK, ((UINT32)0));\
	sp5kSpiWrite (SP5K_SSC_PORT_3, (UINT8 *)&(addr), REG_ADDR_BIT * cnt);\
	sp5kSpiWrite (SP5K_SSC_PORT_3, (UINT8 *)&(data), REG_DATA_BIT * cnt);\
	sp5kGpioWrite(CS_PIN_GRP_ID, CS_PIN_MASK, CS_PIN_MASK);
	#endif	
}

static void
gSensorDriverConfig(
	void
)
{
	#if GSENSOR_INTERFACE_I2C  /*  init I2c port use GPIO LMI8 and LMI9*/
	UINT32 param1;
	param1 = ((GSENSOR_SLAVE_ADDR << 16) | (GSENSOR_FERQUENCY_I2C<<8) | (EN_16BIT_ADDR<<5) | READ_MODE);
	sp5kSscCfgSet(SP5K_SSC_PORT_1, SP5K_SSC_CFG_MODE, SP5K_SSC_MODE_I2C);
	sp5kSscCfgSet(SP5K_SSC_PORT_1, SP5K_SSC_CFG_PIN, SP5K_SSC_CFG_SSC1_PIN1);
	sp5kSscCfgSet(SP5K_SSC_PORT_1, SP5K_I2C_CFG_PHY, param1);					/* ena2byte : slave address whether to write two bytes */

	#elif GSENSOR_INTERFACE_SPI_PORT2
	sp5kSscCfgSet(SP5K_SSC_PORT_2, SP5K_SPI_CFG_PHY, (GSENSOR_FERQUENCY_SPI<<24)|(MODE_SPI<<2));
	sp5kGpioCfgSet(CS_PIN_GRP_ID, CS_PIN_MASK, CS_PIN_MASK);	
	sp5kGpioWrite(CS_PIN_GRP_ID,  CS_PIN_MASK,  CS_PIN_MASK);
	halPinMux293(0);
	
	#else 	/* SPI_PORT3_INTERFACE	*/
	sp5kSscCfgSet(SP5K_SSC_PORT_3, SP5K_SSC_CFG_PIN, 0);
	sp5kSscCfgSet(SP5K_SSC_PORT_3, SP5K_GSI_CFG_PHY, (MODE_SPI << 16 | GSENSOR_FERQUENCY_SPI << 8));
	sp5kGpioCfgSet(CS_PIN_GRP_ID, CS_PIN_MASK, CS_PIN_MASK);	
	sp5kGpioWrite(CS_PIN_GRP_ID,  CS_PIN_MASK, CS_PIN_MASK);
	halPinMux293(0);
	#endif
}

static void 
gSensorDriverSerRead(
	UINT32  addr,
	UINT32 *data,
	UINT32  cnt
)
{
	if((addr == NULL) || (cnt < 0))
		return;
	
	#if   GSENSOR_INTERFACE_I2C        /*I2c use port LMI8 and LMI9*/
	sp5kI2cRead((addr),(data),(cnt));

	#elif GSENSOR_INTERFACE_SPI_PORT2  /*SPI port 2*/
	sp5kGpioWrite(CS_PIN_GRP_ID, CS_PIN_MASK, ((UINT32)0));\
	sp5kSpiWrite (SP5K_SSC_PORT_2,(UINT8 *)&(addr), REG_ADDR_BIT * cnt);\
	sp5kSpiRead  (SP5K_SSC_PORT_2,(UINT8 *)(data), REG_DATA_BIT * cnt);\
	sp5kGpioWrite(CS_PIN_GRP_ID, CS_PIN_MASK, CS_PIN_MASK);
			
	#else  					/*SPI port 3*/
	sp5kGpioWrite(CS_PIN_GRP_ID, CS_PIN_MASK, ((UINT32)0));\
	sp5kSpiWrite (SP5K_SSC_PORT_3, (UINT8 *)&(addr), REG_ADDR_BIT * cnt);\
	sp5kSpiRead  (SP5K_SSC_PORT_3, (UINT8 *)(data), REG_DATA_BIT * cnt);\
	sp5kGpioWrite(CS_PIN_GRP_ID, CS_PIN_MASK, CS_PIN_MASK);
			
	#endif
}

void 
gSensorDriverRead(
	UINT32 selID,
	UINT32 * param
)
{
	if( param == NULL)
		return;
	UINT32 statusVal[WR_CNT] = {0};
	gSensorDriverSerRead(GSENSOR_CMD_ANGULAR_RATE, statusVal,WR_CNT * (REG_ADDR_BIT/NBIT_UNIT));
	
	switch(selID){
		case GSENSOR_CHIP_ID:
			*param = GSENSOR_SLAVE_ADDR;
			break;
		case GSENSOR_VAL_X: 
			*param = (statusVal[1] << 8) + statusVal[2];
			break;
		case GSENSOR_VAL_Y:
			*param = (statusVal[4] << 8) + statusVal[5];
			break;
		case GSENSOR_VAL_Z:
			*param = 0;
			break;
		case GSENSOR_SHAKE_STATUS:
			break;
		default :
			break;
	}
}

UINT8 *
gSensorDriverNameGet(
	void
)
{
	return gSensorName;
}

