/*******************************************************************************
*
*       Copyright (c) 2006-2008 by Sunplus mMedia Inc., Ltd.
*
*  This software is copyrighted by and is the property of Sunplus mMedia
*  Inc., Ltd. All rights are reserved by Sunplus mMedia Inc., Ltd.
*  This software may only be used in accordance with the corresponding
*  license agreement. Any unauthorized use, duplication, distribution,
*  or disclosure of this software is expressly forbidden.
*
*  This Copyright notice MUST not be removed or modified without prior
*  written consent of Sunplus mMedia Inc., Ltd.
*
*  Sunplus mMedia Inc., Ltd. reserves the right to modify this software
*  without notice.
*
*  Sunplus mMedia Inc., Ltd.
*  19-1, Innovation First Road, Science-Based Industrial Park,
*  Hsin-Chu, Taiwan, R.O.C.
*
*  File Description:
*
*  Author: Matt Wang
*
*******************************************************************************/

/*******************************************************************************
*                            H E A D E R   F I L E S
*******************************************************************************/
#include "common.h"
#include "debug_mod/dbg_api.h"
#include "middleware/disp_def.h"
/*
#include "middleware/timer.h"
*/
/*
#include "../sensor/flow/hal_front.h"
#if 0
#include "../sensor/flow/hal_front_private.h"
#endif
#include "../sensor/flow/reg_front.h"

*/
#include "EP932_SysController.h"   /* HDMI Transmiter  */
#include "EP932_SystemConfig.h"

/* render dimension. */
#define DEV_PID  0

/*******************************************************************************
*                              C O N S T A N T S
*******************************************************************************/
/*
  compile option in Makefile:
*/
/* I2C define*/
#if 0
#define ANX7150_PORT0_ADDR	0x72  /*ANX7150 */
#define ANX7150_PORT1_ADDR	0x7A  /*ANX7150 */

#define HDMI_720P_50      0
#define HDMI_720P_30	   1
#endif

#define	TG_FORMAT2  	 FRONT_SSC_READ_HAS_SUBADDR | FRONT_SSC_RESTART_OFF  | SENSOR_SERIAL_FORMAT_SSC_SIF1/*don't change*/
#define	TG_FORMAT3  	 0		      /*don't change*/
#define	TG_SPEED         2
#define	TG_NBIT_REGDATA  8		/*ssc/spi register data numbers */
#define	TG_NBIT_REGADDR  8		/*ssc/spi register address numbers */

/* render dimension. */
#if 1 /* 1280 * 720p  version testing*/
#define DEV_TYPE     DISP_PANEL_DEVICE_TYPE_UPS051
#define WIDTH        1280
#define HEIGHT       720
#define HSYNCW       40
#define VSYNCW       5
#define HPIXEL       1650/*1650*//*1980*/
#define VLINE        750
#define PNLH0        260
#define PNLV0        25
#define PNLH1        (PNLH0 + WIDTH - 1)
#define PNLV1        (PNLV0 + HEIGHT - 1)
#define CLK_SRC      DISP_CLK_SRC_DRAMPLL
#define CLK_1X       74250
#define CLK_NX       74250
#define SCL_CLK      198000


#define CCIR_SEQ     DISP_CCIR_CRYCBY
#define FILTER_EVEN  DISP_UPS051_EVEN_BRG
#define FILTER_ODD   DISP_UPS051_ODD_BRG
#endif

#if 0  /* 480p  test version testing*/
#define DEV_TYPE     DISP_PANEL_DEVICE_TYPE_UPS051
#define WIDTH        720
#define HEIGHT       480
#define HSYNCW       12
#define VSYNCW       5
#define HPIXEL       896/*1650*//*1980*/
#define VLINE        499
#define PNLH0        136
#define PNLV0        13
#define PNLH1        (PNLH0 + WIDTH - 1)
#define PNLV1        (PNLV0 + HEIGHT - 1)
#define CLK_SRC      DISP_CLK_SRC_DRAMPLL
#define CLK_1X       65000
#define CLK_NX       65000

#define CCIR_SEQ     DISP_CCIR_CRYCBY
#define FILTER_EVEN  DISP_UPS051_EVEN_BRG
#define FILTER_ODD   DISP_UPS051_ODD_BRG
#endif

/*******************************************************************************
*                                  M A C R O S
*******************************************************************************/

/*******************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*******************************************************************************/
/* device open. */
#if  1
#define dispOpen        dispEP932mOpen
#define dispOpPowerOn   dispEP932mPowerOn
#define dispOpPowerOff  dispEP932mPowerOff
#endif

/* operating function. */
static UINT8 *dispOpDevNameGet(void);
static void   dispOpCapabilityGet(dispCapabDesc_t *pcapabDesc, UINT32 descSize);
static UINT32 dispOpPowerOn(void);
static UINT32 dispOpPowerOff(void);

/* serial command. */
/*static void dispSpiCmdSend(UINT32 val, UINT32 bitsLen);*/
void disp5kSpiCtrl(UINT32 val, UINT32 bitsLen, UINT32 lsbFirst);


/*******************************************************************************
*                            G L O B A L   D A T A
*******************************************************************************/
/* serial command table. */
const static UINT16 dispCmdTbl[] = {

};

/* device name. */
static  UINT8 dispDevName[] = "HDMI - EP932M";


/* device capability. */
static dispCapabDesc_t dispCapabDesc = {
	.width        = WIDTH,
	.height       = HEIGHT,
	.xoff         = PNLH0,
	.yoff         = PNLV0,
	.aspectRatioX = 16,
	.aspectRatioY = 9,
	.logiEn       = 0,
	.frameRate    = 60000,
};

/* parameter table. */
static dispPara_t dispPwrOnParaTbl[] = {
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, CLK_SRC, DISP_CLK_SRC_DRAMPLL, 0, 0, 0},
	{DISP_PARA_CLK_FREQ, CLK_1X, CLK_NX, 0, 0, 0},
	{DISP_PARA_CLK_PHASE, 8, 0, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, SCL_CLK, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_GLOB_ENABLE, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_TOTAL_REGION, HPIXEL, VLINE, HSYNCW, VSYNCW, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, PNLH0, PNLV0, PNLH1, PNLV1, 0},
	{DISP_PARA_UPS_LAYOUT_MODE, DISP_UPS_LAYOUT_DISABLE, 0, 0, 0, 0},
	{DISP_PARA_UPS051_LIGHTVIEW_ENABLE, 0, 0, 0, 0, 0},
	{DISP_PARA_UPS051_RGB888_ENABLE, 1, 0, 0, 0, 0},
	{DISP_PARA_UPS_HSD_MODE, DISP_UPS_HSD_DELAY_3T, 0, 0, 0, 0},
	{DISP_PARA_UPS_COLOR_FILTER, FILTER_EVEN, FILTER_ODD, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, DISP_PROGRESSIVE, 0, 0, 0, 2},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

/*******************************************************************************
*
* Function Name: dispOpen
*
* Descriptions:
*
* Arguments:
*
* Returns:
*   SUCCESS or FAIL.
*
* See also:
*
*******************************************************************************/
UINT32
dispOpen(
	void
)
{
	UINT32 ret;
	dispOperatingFunc_t dispOpFunc;
	printf("disp_hdmitx\n");

	DBG_FUNC_ENTER(NULL);

	dispOpFunc.pfOpDevNameGet    = dispOpDevNameGet;
	dispOpFunc.pfOpPowerOn       = dispOpPowerOn;
	dispOpFunc.pfOpPowerOff      = dispOpPowerOff;
	dispOpFunc.pfOpCapabilityGet = dispOpCapabilityGet;
	ret = dispDevOpen(&dispOpFunc, sizeof(dispOperatingFunc_t));

	DBG_FUNC_RETURN(ret, NULL);
}

/*******************************************************************************
*
* Function Name: dispOpDevNameGet
*
* Descriptions:
*
* Arguments:
*
* Returns:
*   return = device name string.
*
* See also:
*
*******************************************************************************/
static UINT8 *
dispOpDevNameGet(
	void
)
{
	return dispDevName;
}

/*******************************************************************************
*
* Function Name: dispOpCapabilityGet
*
* Descriptions:
*
* Arguments:
*
* Returns:
*
* See also:
*
*******************************************************************************/
static void
dispOpCapabilityGet(
	dispCapabDesc_t* pcapabDesc,
	UINT32 descSize
)
{
	*pcapabDesc = dispCapabDesc;
}

/*******************************************************************************
*
* Function Name: dispOpPowerOn
*
* Descriptions:
*
* Arguments:
*
* Returns:
*
* See also:
*
*******************************************************************************/
static UINT32
dispOpPowerOn(
	void
)
{
	DBG_FUNC_ENTER(NULL);

	WRITE8(0xb0000086,0x07); /* display clock */
	WRITE8(0xb000a010,0x0F); /* reset display engine*/
	WRITE8(0xb000a010,0x00); /*  */

	dispParaTableSet(DEV_PID, dispPwrOnParaTbl);
	/* set clk and type. */

	WRITE8(0xb0000087,0x00);

	printf("hdmi tx\n");
	*(volatile UINT8  *)0xb000a13c = 0x9;
	*(volatile UINT8  *)0xb000a108 = 0x00;
	*(volatile UINT8  *)0xb000a10a = 0x00;
	*(volatile UINT8  *)0xb000a109 = 0x00;
	*(volatile UINT8  *)0xb00000b2 = 0xFF;
	*(volatile UINT8  *)0xb00000b3 = 0xFF;
	*(volatile UINT8  *)0xb000a148 = 0x1;
	*(volatile UINT8  *)0xb0006070 = 0x1;
	*(volatile UINT8  *)0xb000a1b0 = 0x0; /* close color matrix */

	WRITE8(0xb0001031,READ8(0xb0001031)|0x40); /*HDMI reset */
	WRITE8(0xb0001035,READ8(0xb0001035)|0x40);

	printf("hdmi tx start command\n");
	WRITE8(0xb00000b0, 0x00);/* i2c clock*/
/*	WRITE16(0xb0009034, READ16(0xb0009034) & 0xCFFF);*/
	#if 0 /*test ing iic*/
	for ( i = 0; i < 10; i++ ) {
		ANX7150_i2c_write_p0_reg(0x72+i,0x33);

		ANX7150_i2c_read(0x72+i, regdata, 0,3,0);
		printf("I2C readback value = %x  i=%x,\n", regdata[0],i);
		for (j=0;j<100000;j++){
		}
	}
	#endif

	DBG_FUNC_RETURN(SUCCESS, NULL);
}

/*******************************************************************************
*
* Function Name: dispOpPowerOff
*
* Descriptions:
*
* Arguments:
*
* Returns:
*
* See also:
*
*******************************************************************************/
static UINT32
dispOpPowerOff(
	void
)
{
	DBG_FUNC_ENTER(NULL);

	DBG_FUNC_RETURN(SUCCESS, NULL);
}

#if 0

static void
ANX7150_i2c_write(
	UINT8 *pregaddr,
	UINT8 *pregdata,
	UINT8 cnt,
	UINT8 mode,
	UINT8 sync
)
{
	UINT32 i;

	WRITE8(0xb0009100, mode); /* Select Sequential/Burst mode */
	WRITE8(0xb0009108, sync); /* Synchronize with Vd or Vvalid not */
	WRITE8(0xb000913C, cnt);


	if (mode) {
		for (i = 0; i < (cnt+1); i++) {
			if ((READ8(0xb0009138) & 0x30) == 0x30) {
				WRITE8(0xb0009110+i*4, pregaddr[i*2]);
				WRITE8(0xb0009110+i*4+2, pregaddr[i*2+1]);
			}
			else {
				WRITE8(0xb0009110+i*2, pregaddr[i]);
			}
			WRITE8(0xb0009110+i*2+1, pregdata[i]);
		}
	}
	else {

		WRITE8(0xb0009110, pregaddr[0]);
		WRITE8(0xb0009112, pregaddr[1]);
		for (i = 0; i < (cnt+1); i++) {
			WRITE8(0xb0009110+i*2+1, pregdata[i]);

		}
	}
    WRITE8(0xb0009104, 0x01);

	do {
		i = READ8(0xb0009104);

	} while (i == 0x01);

}

/**************************************************************************************
*                                                                                     *
*  Function Name : halFrontSsc2Read                                                   *
*                                                                                     *
*  Purposes      :                                                                    *
*    To Read data through SSC interface                                               *
*                                                                                     *
*  Desprictions  :                                                                    *
*    Read data from sensor through SSC interface                                      *
*                                                                                     *
*  Arguments     :                                                                    *
*    regaddr  : in, register address of sensor                                        *
*    pregdata : out, point of register data of sensor                                 *
*    cnt      : in, the number of data to Read                                        *
*    mode     : 2: normal read, 3: combine read, 4: combine read with restart         *
*    sync     : in, 0: immediate, 1: sync vdorg, 2: sync vvalid                       *
*                                                                                     *
*  Returns       :                                                                    *
*    None                                                                             *
*                                                                                     *
*  See also      :                                                                    *
*                                                                                     *
***************************************************************************************/

void
ANX7150_i2c_read(

		UINT8 regaddr,
		UINT8  *pregdata,
		UINT8  cnt,
		UINT8  mode,
		UINT8  sync
	)
	{
#if 0
	frontReg1_t *pfrontReg1= (frontReg1_t *)(0xb0000000);
	UINT32 tmp0, i;

		/*UINT32 status=0;*/

		do tmp0 = pfrontReg1->sif1trigger;
		while (tmp0 == 0x01);						  /* Wait SSC Busy */

	/*	printf("I2C read function start\n");*/
		pfrontReg1->sif1mode		= mode;
		pfrontReg1->sif1sync		= sync; 		  /* Synchronize with Vd or Vvalid not */
		pfrontReg1->sif1i2ccount	= cnt;
		pfrontReg1->sif1addr[0] = regaddr & 0xFF;
		pfrontReg1->sif1addr[2] = regaddr >> 8;
		pfrontReg1->sif1trigger 	= 0x01;
		do tmp0 = pfrontReg1->sif1trigger;
		while (tmp0 == 0x01);						  /* Wait SSC Busy */

		for (i = 0; i < (cnt+1); i++)
				pregdata[i] = pfrontReg1->sif1addr[i*2+1];

#else

UINT32 tmp0, i;

	/*UINT32 status=0;*/

	do tmp0 = (*((volatile UINT8*)(0xb0009104)));
	while (tmp0 == 0x01);						  /* Wait SSC Busy */

/*	printf("I2C read function start\n");*/
	*(volatile UINT8*)(0xb0009100) = mode;
	*(volatile UINT8*)(0xb0009108) = sync;
	*(volatile UINT8*)(0xb000913c) = cnt;
	*(volatile UINT8*)(0xb0009110) = regaddr & 0xFF;
	*(volatile UINT8*)(0xb0009112) = regaddr >> 8;
	*(volatile UINT8*)(0xb0009104) = 0x01;

	do tmp0 = (*((volatile UINT8*)(0xb0009104)));
	while (tmp0 == 0x01);						  /* Wait SSC Busy */

	for (i = 0; i < (cnt+1); i++)
			pregdata[i] = (*((volatile UINT8*)(0xb0009110+i*2+1)));






#endif
	}



void ANX7150_i2c_read_p0_reg(BYTE offset, BYTE *d)
{
	UINT8 regaddr[1];
	UINT8 regdata[1];
	regaddr[0]= offset;

	/*printf(" read_p0 off= %x, data= %x\n",offset,*d);*/


    WRITE8(0xb0009140, ANX7150_PORT0_ADDR);
    ANX7150_i2c_read(offset, d, 0, 4, 0);
}

void ANX7150_i2c_read_p1_reg(BYTE offset, BYTE *d)
{
	UINT8 regaddr[1];
	UINT8 regdata[1];
	regaddr[0]= offset;

	/*printf(" read_p1\n");*/

    WRITE8(0xb0009140, ANX7150_PORT1_ADDR);
    ANX7150_i2c_read(offset, d,0 , 4, 0);
}

void ANX7150_i2c_write_p0_reg(BYTE offset, BYTE d)
{

	UINT8 regaddr[1];
	UINT8 regdata[1];
	regaddr[0]= offset;
	regdata[0]= d;
	/*printf(" write_p0 off= %x, data= %x\n",offset,d);*/
	WRITE8(0xb0009140, ANX7150_PORT0_ADDR);
	ANX7150_i2c_write(regaddr, regdata,0, 0, 0);
}

void ANX7150_i2c_write_p1_reg(BYTE offset, BYTE d)
{
	UINT8 regaddr[1];
	UINT8 regdata[1];
	regaddr[0]= offset;
	regdata[0]= d;
	/*printf(" write_p1 off= %x, data= %x\n",offset,d);*/
	WRITE8(0xb0009140, ANX7150_PORT1_ADDR);
	ANX7150_i2c_write(regaddr, regdata,0, 0, 0);
}

void delay_ms(int n)
{
	int i;
	for (i=0;i< n*10000;i++){}
}

#endif

