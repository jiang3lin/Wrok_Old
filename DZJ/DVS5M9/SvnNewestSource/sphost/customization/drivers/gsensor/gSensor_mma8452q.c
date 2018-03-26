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
//#define GSENSOR_INTERRUP_PULSE
#define GSENSOR_INTERRUP_TRANSIENT

#define REG_ADDR_BIT 								8
#define REG_DATA_BIT								8

#define GSENSOR_CMD_X_DATA_H 						0x01
#define GSENSOR_CMD_X_DATA_L 						0x02
#define GSENSOR_CMD_Y_DATA_H 						0x05	//0x03
#define GSENSOR_CMD_Y_DATA_L 						0x06	//0x04
#define GSENSOR_CMD_Z_DATA_H 						0x03	//0x05
#define GSENSOR_CMD_Z_DATA_L 						0x04	//0x06
#define GSENSOR_CMD_INTERRUP_SRC					0x0c	//read only

#define GSENSOR_CMD_TRANSIENT_THRESHOLD			0x1f

#define GSENSOR_CMD_PULSE_X_THRESHOLD			0x23
#define GSENSOR_CMD_PULSE_Y_THRESHOLD			0x24
#define GSENSOR_CMD_PULSE_Z_THRESHOLD			0x25

#ifdef GSENSOR_INTERRUP_PULSE
#define GSENSOR_VAL_THRESHOLD_HIGHT				(UINT8)(0.5 / 0.063)
#define GSENSOR_VAL_THRESHOLD_MIDDLE				(UINT8)(1.0 / 0.063)
#define GSENSOR_VAL_THRESHOLD_LOW				(UINT8)(1.5 / 0.063)
#elif defined GSENSOR_INTERRUP_TRANSIENT
#define GSENSOR_VAL_THRESHOLD_HIGHT				(UINT8)(0.5 / 0.063)
#define GSENSOR_VAL_THRESHOLD_MIDDLE				(UINT8)(1.0 / 0.063)
#define GSENSOR_VAL_THRESHOLD_LOW				(UINT8)(1.5 / 0.063)
#endif

/* select interface */
#define GSENSOR_INTERFACE_I2C						1
#define GSENSOR_INTERFACE_SPI_PORT2				0
#define GSENSOR_INTERFACE_SPI_PORT3				0

/* i2c interface */
#if   GSENSOR_INTERFACE_I2C
#define GSENSOR_SLAVE_ADDR	    					0x3a	//SA0 = 1
#define GSENSOR_FERQUENCY_I2C   					SP5K_I2C_CLK_DIVISOR_1024
#define READ_MODE	   								0x04/*0:read w/o dummy write, 1:read with dummy write, 2:read with dummy write & stop bit omission*/
#define I2C_WR_UNIT									8
#define I2C_WR_CNT									REG_DATA_BIT/I2C_WR_UNIT /*  */
#if (I2C_WR_UNIT == 16)
#define EN_16BIT_ADDR  								1
#else
#define EN_16BIT_ADDR  								0
#endif

/* spi interface */
#elif (GSENSOR_INTERFACE_SPI_PORT2 == 1 || GSENSOR_INTERFACE_SPI_PORT3 == 1)
#define CS_PIN_GRP_ID	    							SP5K_GPIO_GRP_LMI	/* SPI CHIP SELECT GPIO GROUP ID */
#define CS_PIN_MASK     								(1 << 0)
#define GSENSOR_FERQUENCY_SPI   					SP5K_SPI_PORT2_CLK_1_5MHZ
#define MODE_SPI	   		 						0x03
#define SPI_WR_CNT									8  /* alway 8 */ 
#endif

/* device name */
static UINT8 gSensorName[] = "G-Sensor MMA8452Q";

/* Register Initialization Table	*/
UINT8 regInitTbl_2g[] = {		/* {ADDR, DATA} */
	0x2b, 0x40,	//40 = reset
	0x2a, 0x18,	//ORD = 100HZ, Active = 0, enable write reg
	0x2b, 0x00,	//00 = normal, 04 = auto sleep	
	0x0e, 0x00,	//00 = 2g, 01 = 4g, 02 = 8g
	
	0x2a, 0x19,	//ORD = 100HZ, Active = 1
	0xff, 0xff,	//end flag
};

UINT8 regInitTbl_4g[] = {		/* {ADDR, DATA} */
	0x2b, 0x40,	//40 = reset
	0x2a, 0x18,	//ORD = 100HZ, Active = 0, enable write reg
	0x2b, 0x00,	//00 = normal, 04 = auto sleep	
	0x0e, 0x01,	//00 = 2g, 01 = 4g, 02 = 8g
	
	0x2a, 0x19,	//ORD = 100HZ, Active = 1
	0xff, 0xff,	//end flag
};

UINT8 regInitTbl_8g[] = {		/* {ADDR, DATA} */
	0x2b, 0x40,	//40 = reset
	0x2a, 0x18,	//ORD = 100HZ, Active = 0, enable write reg
	0x2b, 0x00,	//00 = normal, 04 = auto sleep	
	0x0e, 0x02,	//00 = 2g, 01 = 4g, 02 = 8g
	
	0x2a, 0x19,	//ORD = 100HZ, Active = 1
	0xff, 0xff,	//end flag
};

UINT8 regInitTbl_PulseInt[] = {		/* {ADDR, DATA} */
	0x2b, 0x40,	//40 = reset
	0x2a, 0x18,	//ORD = 100HZ, Active = 0, enable write reg
	0x2b, 0x00,	//00 = normal, 04 = auto sleep	
	0x0e, 0x00,	//00 = 2g, 01 = 4g, 02 = 8g
	
	#ifdef GSENSOR_INTERRUP_PULSE
	
	0x2c, 0x00,	//interrup config
	0x2d, 0x08,	//pulse interrup enable
	0x2e, 0x08,	//pulse interrup mapting to INT1 pin

	//pulse interrup config
	0x21, 0xff,	//pulse config
	//0x22, 0x00,	//pulse source 			(read only)
	0x23, 0x0f,	//pulse X Threshold			(0.063g/step)
	0x24, 0x0f,	//pulse Y Threshold			(0.063g/step)
	0x25, 0x0f,	//pulse Z Threshold			(0.063g/step)
	0x26, 0x01,	//pulse limite(first timer)		(pulse width)
	0x27, 0x01,	//pulse latency				(pulse delay)
	0x28, 0x01,	//pulse 2nd window			(pulse interval)
	
	#elif defined GSENSOR_INTERRUP_TRANSIENT
	
	0x2c, 0x00,	//interrup config
	0x2d, 0x20,	//transient interrup enable
	0x2e, 0x20,	//transient interrup mapting to INT1 pin

	0x1d, 0x1e,	//transient config
	//0x1e, 0x00,	//transient source			(read only)
	0x1f, 0xff,	//transient threshold			(0.063g/step)
	0x20, 0x05, 	//transient count			(keep time)
	
	#endif

	0x2a, 0x19,	//ORD = 100HZ, Active = 1
	0xff, 0xff,	//end flag
};

UINT8 regInitTbl_ResetInt[] = {		/* {ADDR, DATA} */
	0x2b, 0x40,	//40 = reset
	0x2a, 0x18,	//ORD = 100HZ, Active = 0, enable write reg
	0x2b, 0x04,	//00 = normal, 04 = auto sleep	
	0x0e, 0x00,	//00 = 2g, 01 = 4g, 02 = 8g
	
	//0x2a, 0x19,	//ORD = 100HZ, Active = 1
	0xff, 0xff,	//end flag
};
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
static void gSensorDriverConfig(void);

void gSensorDriverInit(	UINT8 mode)
{
	UINT8  index = 0;
	UINT8* regInitTbl = NULL;
	UINT32 param = 0;

	/* config communicate interface */
	gSensorDriverConfig();

	if(mode == GSENSOR_NORMAL_INIT)
	{
		regInitTbl = regInitTbl_2g;
	}
	else	 if(mode == GSENSOR_COLLISIONDET_INIT)
	{
		regInitTbl = regInitTbl_PulseInt;
	}
	else		//GSENSOR_RESET_INIT
	{
		regInitTbl = regInitTbl_ResetInt;
	}
	
	/* write init table to device ,init table format should be:first addr then data*/
	gSensorDriverWrite(regInitTbl[index], regInitTbl[index + 1]);
	sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 100);
	for(index = 2; regInitTbl[index] != 0xff || regInitTbl[index + 1] != 0xff; index += 2)
	{
		//printf("reg[%02d]:0x%02x = 0x%02x\n", index, regInitTbl[index], regInitTbl[index + 1]);
		gSensorDriverWrite(regInitTbl[index], regInitTbl[index + 1]);
	}

	//clean interrup pin
	gSensorDriverRead(GSENSOR_INTERRUP_SRC, &param);
}

static void gSensorDriverConfig(void)
{
	#if GSENSOR_INTERFACE_I2C  /*  init I2c port use GPIO LMI8 and LMI9*/
	UINT32 param1;
	param1 = ((GSENSOR_SLAVE_ADDR << 16) | (GSENSOR_FERQUENCY_I2C << 8) | (EN_16BIT_ADDR << 5) | READ_MODE);
	sp5kSscCfgSet(SP5K_SSC_PORT_1, SP5K_SSC_CFG_MODE, SP5K_SSC_MODE_I2C);
	sp5kSscCfgSet(SP5K_SSC_PORT_1, SP5K_SSC_CFG_PIN, SP5K_SSC_CFG_SSC1_PIN1);
	sp5kSscCfgSet(SP5K_SSC_PORT_1, SP5K_I2C_CFG_PHY, param1);					/* ena2byte : slave address whether to write two bytes */

	#elif GSENSOR_INTERFACE_SPI_PORT2/* SPI_PORT2_INTERFACE	*/
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

void gSensorDriverWrite(UINT32 addr, UINT32 data)
{
	#if   GSENSOR_INTERFACE_I2C        /*I2c use port LMI8 and LMI9*/
	//sp5kI2cWriteMulti((addr),(&data),I2C_WR_CNT);
	sp5kI2cWrite(addr, data);
	
	#elif GSENSOR_INTERFACE_SPI_PORT2  /*SPI port 2*/
	sp5kGpioWrite(CS_PIN_GRP_ID, CS_PIN_MASK, 0);\
	sp5kSpiWrite (SP5K_SSC_PORT_2, (UINT8 *)&(addr), SPI_WR_CNT);\
	sp5kSpiWrite (SP5K_SSC_PORT_2, (UINT8 *)&(data), SPI_WR_CNT);\
	sp5kGpioWrite(CS_PIN_GRP_ID, CS_PIN_MASK, CS_PIN_MASK);
	
	#else  					/*SPI port 3*/	
	sp5kGpioWrite(CS_PIN_GRP_ID, CS_PIN_MASK, ((UINT32)0));\
	sp5kSpiWrite (SP5K_SSC_PORT_3, (UINT8 *)&(addr), SPI_WR_CNT);\
	sp5kSpiWrite (SP5K_SSC_PORT_3, (UINT8 *)&(data), SPI_WR_CNT);\
	sp5kGpioWrite(CS_PIN_GRP_ID, CS_PIN_MASK, CS_PIN_MASK);
	#endif	
}

void gSensorDriverSerRead(UINT32  addr, UINT32 *data)
{
	#if   GSENSOR_INTERFACE_I2C        /*I2c use port LMI8 and LMI9*/
	sp5kI2cRead((addr),(data),I2C_WR_CNT);

	#elif GSENSOR_INTERFACE_SPI_PORT2  /*SPI port 2*/
	sp5kGpioWrite(CS_PIN_GRP_ID, CS_PIN_MASK, ((UINT32)0));\
	sp5kSpiWrite (SP5K_SSC_PORT_2,(UINT8 *)&(addr), SPI_WR_CNT);\
	sp5kSpiRead  (SP5K_SSC_PORT_2,(UINT8 *)(data), SPI_WR_CNT);\
	sp5kGpioWrite(CS_PIN_GRP_ID, CS_PIN_MASK, CS_PIN_MASK);
			
	#else  					/*SPI port 3*/
	sp5kGpioWrite(CS_PIN_GRP_ID, CS_PIN_MASK, ((UINT32)0));\
	sp5kSpiWrite (SP5K_SSC_PORT_3, (UINT8 *)&(addr), SPI_WR_CNT);\
	sp5kSpiRead  (SP5K_SSC_PORT_3, (UINT8 *)(data), SPI_WR_CNT);\
	sp5kGpioWrite(CS_PIN_GRP_ID, CS_PIN_MASK, CS_PIN_MASK);
			
	#endif
}

void gSensorDriverRead(UINT32 selID, UINT32 * param)
{
	if( param == NULL)
		return;
	UINT32 statusVal1 = 0, statusVal2 = 0;
	switch(selID){
		case GSENSOR_CHIP_ID:
			*param = GSENSOR_SLAVE_ADDR;
			break;
		case GSENSOR_VAL_X: 
			gSensorDriverSerRead(GSENSOR_CMD_X_DATA_H, &statusVal1);
			gSensorDriverSerRead(GSENSOR_CMD_X_DATA_L, &statusVal2);
			*param = ((statusVal1 << 8) | (statusVal2 & 0xF0)) >> 4;
			break;
		case GSENSOR_VAL_Y:
			gSensorDriverSerRead(GSENSOR_CMD_Y_DATA_H, &statusVal1);
			gSensorDriverSerRead(GSENSOR_CMD_Y_DATA_L, &statusVal2);
			*param = ((statusVal1 << 8) | (statusVal2 & 0xF0)) >> 4;
			break;
		case GSENSOR_VAL_Z:
			gSensorDriverSerRead(GSENSOR_CMD_Z_DATA_H, &statusVal1);
			gSensorDriverSerRead(GSENSOR_CMD_Z_DATA_L, &statusVal2);
			*param = ((statusVal1 << 8) | (statusVal2 & 0xF0)) >> 4;
			break;
		case GSENSOR_SHAKE_STATUS:
			*param = 0;
			break;
		case GSENSOR_INTERRUP_SRC:
			gSensorDriverSerRead(GSENSOR_CMD_INTERRUP_SRC, param);
			break;
		default :
			break;
	}
}

void gSensorDriverSet(UINT32 selID, UINT32 param)
{
	switch(selID)
	{
		case GSENSOR_SENSITIVITY:
			if(param == GSENSOR_SENSITIVITY_HIGHT)
			{
				#ifdef GSENSOR_INTERRUP_PULSE
				gSensorDriverWrite(GSENSOR_CMD_PULSE_X_THRESHOLD, GSENSOR_VAL_THRESHOLD_HIGHT);
				gSensorDriverWrite(GSENSOR_CMD_PULSE_Y_THRESHOLD, GSENSOR_VAL_THRESHOLD_HIGHT);
				gSensorDriverWrite(GSENSOR_CMD_PULSE_Z_THRESHOLD, GSENSOR_VAL_THRESHOLD_HIGHT);
				#elif defined GSENSOR_INTERRUP_TRANSIENT
				gSensorDriverWrite(GSENSOR_CMD_TRANSIENT_THRESHOLD, GSENSOR_VAL_THRESHOLD_HIGHT);
				#endif
			}
			else if(param == GSENSOR_SENSITIVITY_MIDDLE)
			{
				#ifdef GSENSOR_INTERRUP_PULSE
				gSensorDriverWrite(GSENSOR_CMD_PULSE_X_THRESHOLD, GSENSOR_VAL_THRESHOLD_MIDDLE);
				gSensorDriverWrite(GSENSOR_CMD_PULSE_Y_THRESHOLD, GSENSOR_VAL_THRESHOLD_MIDDLE);
				gSensorDriverWrite(GSENSOR_CMD_PULSE_Z_THRESHOLD, GSENSOR_VAL_THRESHOLD_MIDDLE);
				#elif defined GSENSOR_INTERRUP_TRANSIENT
				gSensorDriverWrite(GSENSOR_CMD_TRANSIENT_THRESHOLD, GSENSOR_VAL_THRESHOLD_MIDDLE);
				#endif
			}
			else if(param == GSENSOR_SENSITIVITY_LOW)
			{
				#ifdef GSENSOR_INTERRUP_PULSE
				gSensorDriverWrite(GSENSOR_CMD_PULSE_X_THRESHOLD, GSENSOR_VAL_THRESHOLD_LOW);
				gSensorDriverWrite(GSENSOR_CMD_PULSE_Y_THRESHOLD, GSENSOR_VAL_THRESHOLD_LOW);
				gSensorDriverWrite(GSENSOR_CMD_PULSE_Z_THRESHOLD, GSENSOR_VAL_THRESHOLD_LOW);
				#elif defined GSENSOR_INTERRUP_TRANSIENT
				gSensorDriverWrite(GSENSOR_CMD_TRANSIENT_THRESHOLD, GSENSOR_VAL_THRESHOLD_LOW);
				#endif
			}
			break;
		default:
			break;
	}
}

UINT8 * gSensorDriverNameGet(void)
{
	return gSensorName;
}

