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
#include <stdio.h>
#include "api/sp5k_utility_api.h"
#include "api/sp5k_global_api.h"
#include "app_gSensor_def.h"


/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

/* G-Sensor */
#define REG_ADDR_BIT 					8
#define REG_DATA_BIT					8

#define GSENSOR_CMD_X_DATA 				0x00
#define GSENSOR_CMD_Y_DATA 				0x01
#define GSENSOR_CMD_STATUS 				0x02

/* select interface */
#define GSENSOR_INTERFACE_I2C			1
#define GSENSOR_INTERFACE_SPI_PORT2		0
#define GSENSOR_INTERFACE_SPI_PORT3		0


/* i2c interface */
#if   GSENSOR_INTERFACE_I2C
#define GSENSOR_SLAVE_ADDR	    0x2a
#define GSENSOR_FERQUENCY_I2C   SP5K_I2C_CLK_DIVISOR_1024
#define READ_MODE	   			0x04/*0:read w/o dummy write, 1:read with dummy write, 2:read with dummy write & stop bit omission*/
#define I2C_WR_UNIT				8
#define I2C_WR_CNT				REG_DATA_BIT/I2C_WR_UNIT 
#if (I2C_WR_UNIT == 16)
#define EN_16BIT_ADDR  			1
#else
#define EN_16BIT_ADDR  			0
#endif

/* spi interface */
#elif (GSENSOR_INTERFACE_SPI_PORT2 == 1 || GSENSOR_INTERFACE_SPI_PORT3 == 1)
#define CS_PIN_GRP_ID	    	SP5K_GPIO_GRP_LMI	/* SPI CHIP SELECT GPIO GROUP ID */
#define CS_PIN_MASK     		(1 << 0)
#define GSENSOR_FERQUENCY_SPI   SP5K_SPI_PORT2_CLK_1_5MHZ
#define MODE_SPI	   		 	0x03
#define SPI_WR_CNT				8  /* alway 8 */ 

#define SPI_CS_HI		sp5kGpioCfgSet(CS_PIN_GRP_ID, CS_PIN_MASK, CS_PIN_MASK);\
						 sp5kGpioWrite(CS_PIN_GRP_ID,  CS_PIN_MASK,  CS_PIN_MASK)
						

#define SPI_CS_LO		sp5kGpioCfgSet(CS_PIN_GRP_ID, CS_PIN_MASK, CS_PIN_MASK);\
						 sp5kGpioWrite(CS_PIN_GRP_ID,  CS_PIN_MASK,  0)
						
#endif

/* device name */
static UINT8 gSensorName[] = "G-Sensor DTOS MXC6225CU";

/* Register Initialization Table	*/
UINT32 regInitTbl[] = {		/* {ADDR, DATA} */
	0x04, 0x38
};

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

	/* config communicate interface */
	gSensorDriverConfig();
  
	/* write init table to device ,init table format should be:first addr then data*/
	if(mode == GSENSOR_NORMAL_INIT)
	{
		for(index = 0; index < sizeof(regInitTbl)/sizeof(regInitTbl[0]); index += 2){
			gSensorDriverWrite(regInitTbl[index], regInitTbl[index + 1]);
		}
	}
}


static void
gSensorDriverConfig(
	void
)
{
	#if GSENSOR_INTERFACE_I2C  /*  init I2c port use GPIO LMI8 and LMI9*/
	UINT32 param1;
	param1 = ((GSENSOR_SLAVE_ADDR << 16) | (GSENSOR_FERQUENCY_I2C << 8) | (EN_16BIT_ADDR << 5) | READ_MODE);
	sp5kSscCfgSet(SP5K_SSC_PORT_1, SP5K_SSC_CFG_MODE, SP5K_SSC_MODE_I2C);
	sp5kSscCfgSet(SP5K_SSC_PORT_1, SP5K_SSC_CFG_PIN, SP5K_SSC_CFG_SSC1_PIN1);
	sp5kSscCfgSet(SP5K_SSC_PORT_1, SP5K_I2C_CFG_PHY, param1);					/* ena2byte : slave address whether to write two bytes */

	#elif GSENSOR_INTERFACE_SPI_PORT2/* SPI_PORT2_INTERFACE	*/
	sp5kSscCfgSet(SP5K_SSC_PORT_2, SP5K_SPI_CFG_PHY, (GSENSOR_FERQUENCY_SPI<<24)|(MODE_SPI<<2));
	SPI_CS_HI;
	halPinMux293(0);
	
	#else 	/* SPI_PORT3_INTERFACE	*/
	sp5kSscCfgSet(SP5K_SSC_PORT_3, SP5K_SSC_CFG_PIN, 0);
	sp5kSscCfgSet(SP5K_SSC_PORT_3, SP5K_GSI_CFG_PHY, (MODE_SPI << 16 | GSENSOR_FERQUENCY_SPI << 8));
	SPI_CS_HI;
	halPinMux293(0);
	#endif
}

void 
gSensorDriverWrite(
	UINT32 addr,
	UINT32 data
)
{
	#if   GSENSOR_INTERFACE_I2C        /*I2c use port LMI8 and LMI9*/
	sp5kI2cWriteMulti((addr),(&data),I2C_WR_CNT);

	#elif GSENSOR_INTERFACE_SPI_PORT2  /*SPI port 2*/
	SPI_CS_LO;
	sp5kSpiWrite (SP5K_SSC_PORT_2, (UINT8 *)&(addr), SPI_WR_CNT);
	sp5kSpiWrite (SP5K_SSC_PORT_2, (UINT8 *)&(data), SPI_WR_CNT);
	SPI_CS_HI;
	
	#else  					/*SPI port 3*/	
	SPI_CS_LO;
	sp5kSpiWrite (SP5K_SSC_PORT_3, (UINT8 *)&(addr), SPI_WR_CNT);
	sp5kSpiWrite (SP5K_SSC_PORT_3, (UINT8 *)&(data), SPI_WR_CNT);
	SPI_CS_HI;
	#endif	
}

static void 
gSensorDriverSerRead(
	UINT32  addr,
	UINT32 *data
)
{
	#if   GSENSOR_INTERFACE_I2C        /*I2c use port LMI8 and LMI9*/
	sp5kI2cRead((addr),(data),I2C_WR_CNT);

	#elif GSENSOR_INTERFACE_SPI_PORT2  /*SPI port 2*/
	SPI_CS_LO;
	sp5kSpiWrite (SP5K_SSC_PORT_2,(UINT8 *)&(addr), SPI_WR_CNT);\
	sp5kSpiRead  (SP5K_SSC_PORT_2,(UINT8 *)(data), SPI_WR_CNT);\
	SPI_CS_HI;
			
	#else  					/*SPI port 3*/
	SPI_CS_LO;
	sp5kSpiWrite (SP5K_SSC_PORT_3, (UINT8 *)&(addr), SPI_WR_CNT);\
	sp5kSpiRead  (SP5K_SSC_PORT_3, (UINT8 *)(data), SPI_WR_CNT);\
	SPI_CS_HI;
			
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
	UINT32 statusVal = 0;
	switch(selID){
		case GSENSOR_CHIP_ID:
			*param = GSENSOR_SLAVE_ADDR;
			break;
		case GSENSOR_VAL_X: 
			gSensorDriverSerRead(GSENSOR_CMD_X_DATA, &statusVal);
			*param = statusVal;
			break;
		case GSENSOR_VAL_Y:
			gSensorDriverSerRead(GSENSOR_CMD_Y_DATA, &statusVal);
			*param = statusVal;
			break;
		case GSENSOR_VAL_Z:
			*param = 0;
			break;
		case GSENSOR_SHAKE_STATUS:
			gSensorDriverSerRead(GSENSOR_CMD_STATUS, &statusVal);
			if((statusVal & 0x60) == 0x20)
				*param = GSENSOR_SHAKE_LEFT;
			else if((statusVal & 0x60) == 0x40)
				*param = GSENSOR_SHAKE_RIGHT;
			else if((statusVal & 0x60) == 0x00)
				*param = GSENSOR_SHAKE_NONE;
			else if((statusVal & 0x60) == 0x60)
				*param = GSENSOR_SHAKE_UNDEF;
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

