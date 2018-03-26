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
#include "app_gSensor.h"
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

#define GSENSOR_CMD_X_DATA_L 			0x28
#define GSENSOR_CMD_X_DATA_H 			0x29
#define GSENSOR_CMD_Y_DATA_L 			0x2a
#define GSENSOR_CMD_Y_DATA_H			0x2b
#define GSENSOR_CMD_Z_DATA_L 			0x2c
#define GSENSOR_CMD_Z_DATA_H 			0x2d
#define GSENSOR_CMD_STATUS           		0x27

/* select interface */
#define GSENSOR_INTERFACE_I2C			1
#define GSENSOR_INTERFACE_SPI_PORT2		0
#define GSENSOR_INTERFACE_SPI_PORT3		0

/* i2c interface */
#if   GSENSOR_INTERFACE_I2C
#define GSENSOR_SLAVE_ADDR	    0x32
#define GSENSOR_FERQUENCY_I2C   SP5K_I2C_CLK_DIVISOR_8192
#define READ_MODE	   			0x04/*0:read w/o dummy write, 1:read with dummy write, 2:read with dummy write & stop bit omission*/
#define I2C_WR_UNIT				8
#define I2C_WR_CNT				REG_DATA_BIT/I2C_WR_UNIT /*  */
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
static UINT8 gSensorName[] = "G-Sensor LIS35DH";

/* Register Initialization Table	*/
UINT8 regInitTbl_2g[] = {		/* {ADDR, DATA} */
#if 0
	0x20,0x47,
	0x21,0x00,
	0x22,0x1f,
	0x30,0x6a, 
	0x32,0x04,
	0x33,0x03, 
	0x34,0x6a,
	0x36,0x04, 
	0x37,0x03,
	0x38,0x55,
	0x39,0x55,
	0x3b,0xff,
	0x3c,0x0f
	#endif
	0x20,0x47,//0x00
	0x21,0x00,
	//0x22,0x00,
	0x23,0x08,
	0x24,0x40,
	0x25,0x00,
	0x26,0x00,
	0x2e,0x00,
	0x30,0x7f, 
	0x32,0x00,
	0x33,0x00, 
	//0x38,0x15,
	//0x3a,0x7f,
	0x22,0x00,
	0x38,0x0f,//0x3f,
	0x3a,0x4c,
	0x3b,0xff,	
	0x3c,0x80,
	0x3d,0x80,
};

UINT8 regInitTbl_4g[] = {		/* {ADDR, DATA} */
	#if 0
	0x20,0x47,
	0x21,0x00,
	0x22,0x1f,
	0x30,0x6a, 
	0x32,0x04,
	0x33,0x03, 
	0x34,0x6a,
	0x36,0x04, 
	0x37,0x03,
	0x38,0x55,
	0x39,0x55,
	0x3b,0xff,
	0x3c,0x0f
	#endif 
	0x20,0x47,//0x00
	0x21,0x00,
	//0x22,0x00,
	0x23,0x18,
	0x24,0x40,
	0x25,0x00,
	0x26,0x00,
	0x2e,0x00,
	0x30,0x7f, 
	0x32,0x00,
	0x33,0x00, 
	//0x38,0x15,
	//0x3a,0x7f,
	0x22,0x00,
	0x38,0x0f,//0x3f,
	0x3a,0x4c,
	0x3b,0xff,	
	0x3c,0x80,
	0x3d,0x80,
};

UINT8 regInitTbl_8g[] = {		/* {ADDR, DATA} */
	#if 0
	0x20,0x47,
	0x21,0x00,
	0x22,0x1f,
	0x30,0x6a, 
	0x32,0x04,
	0x33,0x03, 
	0x34,0x6a,
	0x36,0x04, 
	0x37,0x03,
	0x38,0x55,
	0x39,0x55,
	0x3b,0xff,
	0x3c,0x0f
	#endif
	0x20,0x47,//0x00
	0x21,0x00,
	//0x22,0x00,
	0x23,0x28,
	0x24,0x40,
	0x25,0x00,
	0x26,0x00,
	0x2e,0x00,
	0x30,0x7f, 
	0x32,0x00,
	0x33,0x00, 
	//0x38,0x15,
	//0x3a,0x7f,
	0x22,0x00,
	0x38,0x0f,//0x3f,
	0x3a,0x4c,
	0x3b,0xff,	
	0x3c,0x80,
	0x3d,0x80,
};

UINT8 regInitTbl_16g[] = {		/* {ADDR, DATA} */
	#if 0
	0x20,0x47,
	0x21,0x00,
	0x22,0x1f,
	0x30,0x6a, 
	0x32,0x04,
	0x33,0x03, 
	0x34,0x6a,
	0x36,0x04, 
	0x37,0x03,
	0x38,0x55,
	0x39,0x55,
	0x3b,0xff,
	0x3c,0x0f
	#endif
	0x20,0x47,//0x00
	0x21,0x00,
	//0x22,0x00,
	0x23,0x38,
	0x24,0x40,
	0x25,0x00,
	0x26,0x00,
	0x2e,0x00,
	0x30,0x7f, 
	0x32,0x00,
	0x33,0x00, 
	//0x38,0x15,
	//0x3a,0x7f,
	0x22,0x00,
	0x38,0x0f,//0x3f,
	0x3a,0x4c,
	0x3b,0xff,	
	0x3c,0x80,
	0x3d,0x80,
	
};

UINT8 regInitTbl_CollisionDetect[] = {
	0x20,0x57,		// odr:50Hz 
	0x21,0xbf,		// fds:enable
	0x22,0x40,		// AOI1 interrupt
	0x23,0x08,
	//0x24,0x00,		// FIFO disable
	0x24,0x08,		// FIFO disable
	//0x25,0x02,
	0x25,0x00,
	//0x2e,0x00,
	//0x32,0x19,		// 400mg
	//0x32,0x1a,		// 416mg
	0x32,0x04,
	//0x32,0x50,
	0x33,0x00, 
	0x26,0x7f,
	0x30,0x2a,		// high event interrupt 
};

#define DEFAULT_MODE		regInitTbl_2g
#define COLLISION_MODE		regInitTbl_CollisionDetect

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
		for(index = 0; index < sizeof(DEFAULT_MODE)/sizeof(DEFAULT_MODE[0]); index += 2){
			gSensorDriverWrite(DEFAULT_MODE[index], DEFAULT_MODE[index + 1]);
		}
	}
	else if(mode == GSENSOR_COLLISIONDET_INIT)
	{
		for(index = 0; index < sizeof(COLLISION_MODE)/sizeof(COLLISION_MODE[0]); index += 2){
			gSensorDriverWrite(COLLISION_MODE[index], COLLISION_MODE[index + 1]);
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
	UINT32 statusVal_H = 0;
	UINT32 statusVal_L = 0;
	switch(selID){
		case GSENSOR_CHIP_ID:
			*param = GSENSOR_SLAVE_ADDR;
			break;
		case GSENSOR_VAL_X: 
			gSensorDriverSerRead(GSENSOR_CMD_X_DATA_H, &statusVal_H);
			gSensorDriverSerRead(GSENSOR_CMD_X_DATA_L, &statusVal_L);
			*param = ((statusVal_H & 0x00ff) << 8) | (0x00ff & statusVal_L);
			break;
		case GSENSOR_VAL_Y:
			gSensorDriverSerRead(GSENSOR_CMD_Y_DATA_H, &statusVal_H);
			gSensorDriverSerRead(GSENSOR_CMD_Y_DATA_L, &statusVal_L);
			*param = ((statusVal_H & 0x00ff) << 8) | (0x00ff & statusVal_L);
			break;
		case GSENSOR_VAL_Z:
			gSensorDriverSerRead(GSENSOR_CMD_Z_DATA_H, &statusVal_H);
			gSensorDriverSerRead(GSENSOR_CMD_Z_DATA_L, &statusVal_L);
			*param = ((statusVal_H & 0x00ff) << 8) | (0x00ff & statusVal_L);
			break;
		case GSENSOR_SHAKE_STATUS:
			gSensorDriverSerRead(GSENSOR_CMD_STATUS, &statusVal_H);
			*param = statusVal_H;
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
