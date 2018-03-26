/*******************************************************************************
*
*       Copyright (c) 2006-2011 by iCatch Technology, Inc.
*
*  This software is copyrighted by and is the property of iCatch
*  Technology, Inc.. All rights are reserved by iCatch Technology, Inc..
*  This software may only be used in accordance with the corresponding
*  license agreement. Any unauthorized use, duplication, distribution,
*  or disclosure of this software is expressly forbidden.
*
*  This Copyright notice MUST not be removed or modified without prior
*  written consent of iCatch Technology, Inc..
*
*  iCatch Technology, Inc. reserves the right to modify this software
*  without notice.
*
*  iCatch Technology, Inc.
*  19-1, Innovation First Road, Science-Based Industrial Park,
*  Hsin-Chu, Taiwan, R.O.C.
*
*******************************************************************************/

#include "common.h"
#include "middleware/disp_def.h"

/*******************************************************************************
*                              C O N S T A N T S
*******************************************************************************/
/*
  compile option in Makefile:
    DISP_UPS051=1       : use ups051 timing.
    DISP_UPS052_W320=1  : use ups052 320 timing.
    DISP_CCIR656_W640=1 : use ccir656 640y 320crcb timing.
    DISP_YUV_W640=1     : use yuv 640 timing.
*/

/* render dimension. */
#define DEV_PID  0

#if DISP_UPS051 == 1
#if SPCA5310
#define DEV_TYPE     DISP_PANEL_DEVICE_TYPE_UPS051
#else
#define DEV_TYPE     DISP_DEVICE_TYPE_UPS051
#endif
#define WIDTH        640
#define HEIGHT       240
#define HSYNCW       1
#define VSYNCW       1
#define HPIXEL       823
#define VLINE        262
#define PNLH0        131
#define PNLV0        13
#define PNLH1        (PNLH0 + WIDTH - 1)
#define PNLV1        (PNLV0 + HEIGHT - 1)
#define CLK_SRC      DISP_CLK_SRC_DRAMPLL
#define CLK_1X       12950
#define CLK_NX       24550
#define SCL_CLK      90000
#define FILTER_EVEN  DISP_UPS051_EVEN_BRG
#define FILTER_ODD   DISP_UPS051_ODD_GBR
#endif

#if DISP_UPS052_W320 == 1
#if SPCA5310
#define DEV_TYPE     DISP_PANEL_DEVICE_TYPE_UPS052
#else
#define DEV_TYPE     DISP_DEVICE_TYPE_UPS052
#endif
#define WIDTH        320
#define HEIGHT       240
#define HSYNCW       1
#define VSYNCW       1
#define HPIXEL       390
#define VLINE        264
#define PNLH0        62
#define PNLV0        13
#define PNLH1        (PNLH0 + WIDTH - 1)
#define PNLV1        (PNLV0 + HEIGHT - 1)
#define CLK_SRC      DISP_CLK_SRC_DRAMPLL
#define CLK_1X       6136
#define CLK_NX       24545
#define SCL_CLK      90000
#define FILTER_EVEN  DISP_UPS052_EVEN_BRG
#define FILTER_ODD   DISP_UPS052_ODD_BRG
#endif

#if DISP_CCIR656_W640 == 1
#if SPCA5310
#define DEV_TYPE     DISP_PANEL_DEVICE_TYPE_PROGRESSIVE_CCIR656
#else
#define DEV_TYPE     DISP_DEVICE_TYPE_PROGRESSIVE_CCIR656
#endif
#define WIDTH        (1440/2)
#define HEIGHT       240
#define HSYNCW       1
#define VSYNCW       1
#define HPIXEL       (1716/2)
#define VLINE        262
#define PNLH0        (272/2)
#define PNLV0        18
#define PNLH1        (PNLH0 + WIDTH - 1 + 1)
#define PNLV1        (PNLV0 + HEIGHT - 1)
#define CLK_SRC      DISP_CLK_SRC_DRAMPLL
#define CLK_1X       12272
#define CLK_NX       27000
#define SCL_CLK      90000
#define CCIR_SEQ     DISP_CCIR_YCRYCB
#endif


#if DISP_YUV_W720 == 1
#if SPCA5310
#define DEV_TYPE     DISP_PANEL_DEVICE_TYPE_PROGRESSIVE_CCIR601
#else
#define DEV_TYPE     DISP_DEVICE_TYPE_PROGRESSIVE_CCIR601
#endif
#define WIDTH     (1440/2)/*(1448/2)*/
#define HEIGHT    240
#define HSYNCW    1
#define VSYNCW    1
#define HPIXEL    (1716/2)
#define VLINE     262
#define PNLH0     (252/2)
#define PNLV0     14
#define PNLH1     (PNLH0 + WIDTH - 1 +1)
#define PNLV1     (PNLV0 + HEIGHT - 1)
#define CLK_SRC      DISP_CLK_SRC_DRAMPLL
#define CLK_1X       13372
#define CLK_NX       27000
#define SCL_CLK      90000
#define CCIR_SEQ     DISP_CCIR_YCBYCR
#endif

#if DISP_YUV_W640 == 1
#if SPCA5310
#define DEV_TYPE     DISP_PANEL_DEVICE_TYPE_PROGRESSIVE_CCIR601
#else
#define DEV_TYPE     DISP_DEVICE_TYPE_PROGRESSIVE_CCIR601
#endif
#define WIDTH     (1280/2)
#define HEIGHT    240
#define HSYNCW    1
#define VSYNCW    1
#define HPIXEL    (1560/2)
#define VLINE     262
#define PNLH0     (252/2)
#define PNLV0     17
#define PNLH1     (PNLH0 + WIDTH - 1 +1)
#define PNLV1     (PNLV0 + HEIGHT - 1)
#define CLK_SRC      DISP_CLK_SRC_DRAMPLL
#define CLK_1X       12950
#define CLK_NX       27000
#define SCL_CLK      90000
#define CCIR_SEQ     DISP_CCIR_YCBYCR
#endif

/* 3-wire interface. */
#ifdef FPGA
#define SCL  (1 << 12)
#define SDA  (1 << 13)
#define CS   (1 << 14)
#else
#define SCL  (1 << 14)
#define SDA  (1 << 15)
#define CS   (1 << 13)
#endif

/*******************************************************************************
*                                  M A C R O S
*******************************************************************************/
#if SPCA5310
#define	SCL_LO  dispParaSet(DEV_PID, DISP_PARA_GPIO_DATA, SCL, 0, 0, 0, 0)
#define	SCL_HI  dispParaSet(DEV_PID, DISP_PARA_GPIO_DATA, SCL, SCL, 0, 0, 0)
#define	SDA_LO  dispParaSet(DEV_PID, DISP_PARA_GPIO_DATA, SDA, 0, 0, 0, 0)
#define	SDA_HI  dispParaSet(DEV_PID, DISP_PARA_GPIO_DATA, SDA, SDA, 0, 0, 0)
#define	CS_LO   dispParaSet(DEV_PID, DISP_PARA_GPIO_DATA, CS, 0, 0, 0, 0)
#define	CS_HI   dispParaSet(DEV_PID, DISP_PARA_GPIO_DATA, CS, CS, 0, 0, 0)
#else
#define	SCL_LO  dispParaSet(DISP_PARA_GPIO_DATA, SCL, 0, 0, 0, 0)
#define	SCL_HI  dispParaSet(DISP_PARA_GPIO_DATA, SCL, SCL, 0, 0, 0)
#define	SDA_LO  dispParaSet(DISP_PARA_GPIO_DATA, SDA, 0, 0, 0, 0)
#define	SDA_HI  dispParaSet(DISP_PARA_GPIO_DATA, SDA, SDA, 0, 0, 0)
#define	CS_LO   dispParaSet(DISP_PARA_GPIO_DATA, CS, 0, 0, 0, 0)
#define	CS_HI   dispParaSet(DISP_PARA_GPIO_DATA, CS, CS, 0, 0, 0)
#endif


/*******************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*******************************************************************************/
/* device open. */
#if DISP_UPS051 == 1
#define dispOpen  dispAuoA025bn01Ups051Open
#endif

#if DISP_UPS052_W320 == 1
#define dispOpen  dispAuoA025bn01Ups052W320Open
#endif

#if DISP_CCIR656_W640 == 1
#define dispOpen  dispAuoA025bn01Ccir656W640Open
#endif

#if DISP_YUV_W640 == 1
#define dispOpen  dispAuoA025bn01YuvW640Open
#endif

#if DISP_YUV_W720 == 1
#define dispOpen  dispAuoA025bn01YuvW720Open
#endif

/* operating function. */
static UINT8 *dispOpDevNameGet(void);
static void   dispOpCapabilityGet(dispCapabDesc_t *pcapabDesc, UINT32 descSize);
static UINT32 dispOpPowerOn(void);
static UINT32 dispOpPowerOff(void);

/* serial command. */
static void dispSpiCmdSend(UINT32 val, UINT32 bitsLen);

/*******************************************************************************
*                            G L O B A L   D A T A
*******************************************************************************/
/* serial command table. */
const static UINT16 dispCmdTbl[] = {
	#if DISP_UPS051 == 1
	0x0037,  /* reset. */
	0x003F,  /* normal operation, dc-dc on. */
	0x1060,  /* ups051. */
	0x5009,  /* contrast. */
	0x6038,  /* brightness. */
	0x003F,  /* normal operation, dc-dc on. */
	#endif

	#if DISP_UPS052_W320 == 1
	0x0037,  /* reset. */
	/*0x003F, */ /* normal operation, dc-dc on. */
	0x1071,  /* ups052 320rgb 24.54mhz. */
	0x5009,  /* contrast. */
	0x6038,  /* brightness. */
	0x003F,  /* normal operation, dc-dc on. */
	#endif

	#if DISP_CCIR656_W640 == 1
	0x0037,  /* reset. */
	0x003F,  /* normal operation, dc-dc on. */
	0x1077,  /* CCIR656 27MHZ */
	0x5008,  /* contrast. */
	0x6031,  /* brightness. */
	0x003F,  /* normal operation, dc-dc on. */
	#endif

	#if DISP_YUV_W640 == 1
	0x0037,  /* reset. */
	0x003F,  /* normal operation, dc-dc on. */
	0x1073,  /* CCIR601  27MHZ */
	0x5008,  /* contrast. */
	0x6031,  /* brightness. */
	0x003F,  /* normal operation, dc-dc on. */
	#endif
	
	#if DISP_YUV_W720 == 1
	0x0037,  /* reset. */
	0x003F,  /* normal operation, dc-dc on. */
	0x1074,  /* CCIR601  27MHZ */
	0x5008,  /* contrast. */
	0x6031,  /* brightness. */
	0x003F,  /* normal operation, dc-dc on. */
	#endif
};

/* device name. */
#if SPCA5310
#if DISP_UPS051 == 1
static UINT8 dispDevName[] = "PANEL - AUO A025BN01 UPS051";
#endif

#if DISP_UPS052_W320 == 1
static UINT8 dispDevName[] = "PANEL - AUO A025BN01 UPS052 W320";
#endif

#if DISP_CCIR656_W640 == 1
static UINT8 dispDevName[] = "PANEL - AUO A025BN01 CCIR656 W640";
#endif

#if DISP_YUV_W640 == 1
static UINT8 dispDevName[] = "PANEL - AUO A025BN01 YUV W640";
#endif

#if DISP_YUV_W720 == 1
static UINT8 dispDevName[] = "PANEL - AUO A025BN01 YUV W720";
#endif

#else
#if DISP_UPS051 == 1
static UINT8 dispDevName[] = "LCD - AUO A025BN01 UPS051";
#endif

#if DISP_UPS052_W320 == 1
static UINT8 dispDevName[] = "LCD - AUO A025BN01 UPS052 W320";
#endif

#if DISP_CCIR656_W640 == 1
static UINT8 dispDevName[] = "LCD - AUO A025BN01 CCIR656 W640";
#endif

#if DISP_YUV_W640 == 1
static UINT8 dispDevName[] = "LCD - AUO A025BN01 YUV W640";
#endif

#if DISP_YUV_W720 == 1
static UINT8 dispDevName[] = "PANEL - AUO A025BN01 YUV W720";
#endif

#endif

/* device capability. */
static dispCapabDesc_t dispCapabDesc = {
	.width        = WIDTH,
	.height       = HEIGHT,
	.xoff         = PNLH0,
	.yoff         = PNLV0,
	.aspectRatioX = 4,
	.aspectRatioY = 3,
	.logiEn       = 0,
	.frameRate    = 60000,
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
	dispOperatingFunc_t dispOpFunc;


	dispOpFunc.pfOpDevNameGet    = dispOpDevNameGet;
	dispOpFunc.pfOpPowerOn       = dispOpPowerOn;
	dispOpFunc.pfOpPowerOff      = dispOpPowerOff;
	dispOpFunc.pfOpCapabilityGet = dispOpCapabilityGet;

	return dispDevOpen(&dispOpFunc, sizeof(dispOperatingFunc_t));
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
UINT8 *
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
void
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
UINT32
dispOpPowerOn(
	void
)
{
	UINT32 i;

	/* set clk and type. */
	#if SPCA5310
	dispParaSet(DEV_PID, DISP_PARA_CLK_SRC, CLK_SRC, 0, 0, 0, 0);
	dispParaSet(DEV_PID, DISP_PARA_CLK_FREQ, CLK_1X, CLK_NX, 0, 0, 0);
	dispParaSet(DEV_PID, DISP_PARA_CLK_PHASE, 8, 0, 0, 0, 0);  /* for both v4 & v5 ok. */
	dispParaSet(DEV_PID, DISP_PARA_SCALE_CLK_FREQ, SCL_CLK, 0, 0, 0, 0);
	dispParaSet(DEV_PID, DISP_PARA_DEVICE_TYPE, DEV_TYPE, 0, 0, 0, 0);
	#else
	dispParaSet(DISP_PARA_CLK_SRC, CLK_SRC, 0, 0, 0, 0);
	dispParaSet(DISP_PARA_CLK_FREQ, CLK_1X, CLK_NX, 0, 0, 0);
	dispParaSet(DISP_PARA_CLK_PHASE_CFG, 3, 0, 0, 0, 0);  /* for both v4 & v5 ok. */
	dispParaSet(DISP_PARA_SCALE_CLK_FREQ, SCL_CLK, 0, 0, 0, 0);
	dispParaSet(DISP_PARA_DEVICE_TYPE, DEV_TYPE, 0, 0, 0, 0);
	#endif


	/* set vh timing and format. */
	#if SPCA5310
	dispParaSet(DEV_PID, DISP_PARA_DEVICE_TOTAL_REGION, HPIXEL, VLINE, HSYNCW, VSYNCW, 0);
	dispParaSet(DEV_PID, DISP_PARA_DEVICE_ACTIVE_REGION, PNLH0, PNLV0, PNLH1, PNLV1, 0);
	#if DISP_UPS051 == 1
	dispParaSet(DEV_PID, DISP_PARA_UPS_LAYOUT_MODE, DISP_UPS_LAYOUT_DISABLE, 0, 0, 0, 0);
	dispParaSet(DEV_PID, DISP_PARA_UPS051_LIGHTVIEW_ENABLE, 0, 0, 0, 0, 0);
	dispParaSet(DEV_PID, DISP_PARA_UPS_HSD_MODE, 0, 0, 0, 0, 0);
	dispParaSet(DEV_PID, DISP_PARA_UPS_COLOR_FILTER, FILTER_EVEN, FILTER_ODD, 0, 0, 0);
	#endif
	#if DISP_UPS052 == 1
	dispParaSet(DEV_PID, DISP_PARA_UPS_LAYOUT_MODE, DISP_UPS_LAYOUT_DISABLE, 0, 0, 0, 0);
	dispParaSet(DEV_PID, DISP_PARA_UPS052_RGB_ONLY_ENABLE, 0, 0, 0, 0, 0);
	dispParaSet(DEV_PID, DISP_PARA_UPS_HSD_MODE, 0, 0, 0, 0, 0);
	dispParaSet(DEV_PID, DISP_PARA_UPS_COLOR_FILTER, FILTER_EVEN, FILTER_ODD, 0, 0, 0);
	#endif
	#if DISP_UPS052_W320 == 1
	dispParaSet(DEV_PID, DISP_PARA_UPS_LAYOUT_MODE, DISP_UPS_LAYOUT_DISABLE, 0, 0, 0, 0);
	dispParaSet(DEV_PID, DISP_PARA_UPS052_RGB_ONLY_ENABLE, 0, 0, 0, 0, 0);
	dispParaSet(DEV_PID, DISP_PARA_UPS_HSD_MODE, 0, 0, 0, 0, 0);
	dispParaSet(DEV_PID, DISP_PARA_UPS_COLOR_FILTER, FILTER_EVEN, FILTER_ODD, 0, 0, 0);
	#endif
	#if (DISP_CCIR656_W640 == 1) || (DISP_YUV_W640 == 1) || (DISP_YUV_W720 == 1)
	dispParaSet(DEV_PID,DISP_PARA_CCIR_OUTPUT_SEQUENCE, CCIR_SEQ, 0, 0, 0, 0);
	dispParaSet(DEV_PID,DISP_PARA_INTERLACE_MODE, DISP_PROGRESSIVE, 0, 0, 0, 0);		
	#endif
	dispParaSet(DEV_PID, DISP_PARA_INTERLACE_MODE, DISP_PROGRESSIVE, 0, 0, 0, 0);
	dispParaSet(DEV_PID, DISP_PARA_GLOB_ENABLE, 1, 0, 0, 0, 2);
	#else
	dispParaSet(DISP_PARA_DEVICE_TOTAL_REGION, HPIXEL, VLINE, HSYNCW, VSYNCW, 0);
	dispParaSet(DISP_PARA_DEVICE_ACTIVE_REGION, PNLH0, PNLV0, PNLH1, PNLV1, 0);
	#if DISP_UPS051 == 1 || DISP_UPS052_W320 == 1
	dispParaSet(DISP_PARA_UPS_LAYOUT_MODE, DISP_UPS_LAYOUT_MODE_DISABLE, 0, 0, 0, 0);
	dispParaSet(DISP_PARA_UPS051_LIGHTVIEW_ENABLE, 0, 0, 0, 0, 0);
	dispParaSet(DISP_PARA_UPS052_RGB_ONLY_ENABLE, 0, 0, 0, 0, 0);
	dispParaSet(DISP_PARA_UPS_HSD_CFG, 0, 0, 0, 0, 0);
	dispParaSet(DISP_PARA_UPS_COLOR_FILTER_CFG, FILTER_EVEN, FILTER_ODD, 0, 0, 0);
	dispParaSet(DISP_PARA_PIXEL_SHIFT_CFG, DISP_PIXEL_SHIFT_ODD, 0, 0, 0, 0);
	#endif
	#if DISP_CCIR656_W640 == 1 || DISP_YUV_W640 == 1
	dispParaSet(DISP_PARA_CCIR_OUTPUT_SEQUENCE_CFG , CCIR_SEQ, 0, 0, 0, 0);
	#endif
	dispParaSet(DISP_PARA_INTERLACE_CFG, DISP_PROGRESSIVE, 0, 0, 0, 2);
	#endif

	/* send 3-wire command. */
	#if SPCA5310
	dispParaSet(DEV_PID, DISP_PARA_GPIO_FUNC_CFG, SCL | SDA | CS, SCL | SDA | CS, 0, 0, 0);
	dispParaSet(DEV_PID, DISP_PARA_GPIO_DIR_CFG, SCL | SDA | CS, SCL | SDA | CS, 0, 0, 0);
	#else
	dispParaSet(DISP_PARA_GPIO_FUNC_CFG, SCL | SDA | CS, SCL | SDA | CS, 0, 0, 0);
	dispParaSet(DISP_PARA_GPIO_DIR_CFG, SCL | SDA | CS, SCL | SDA | CS, 0, 0, 0);
	#endif


	for ( i = 0; i < sizeof(dispCmdTbl) / sizeof(dispCmdTbl[0]); i++ ) {
		dispSpiCmdSend(dispCmdTbl[i], 16);
	}

	return SUCCESS;

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
UINT32
dispOpPowerOff(
	void
)
{
	dispSpiCmdSend(0x0000, 16);

	return SUCCESS;
}

/*******************************************************************************
*
* Function Name: dispSpiCmdSend
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
dispSpiCmdSend(
	UINT32 val,
	UINT32 bitsLen
)
{
	UINT32	i, j;


#if	0
	printf("%s() %d: val=%04x, bitsLen=%d, lsbFirst=%d\n",
	       __FUNCTION__, __LINE__, val, bitsLen, lsbFirst);
#endif

	CS_LO;
	SCL_HI;
	for ( i = 0; i < 16; i++ ) {
		SCL_LO;
		if ( val & 0x8000 ) {
			SDA_HI;
		}
		else {
			SDA_LO;
		}
		for ( j = 0; j < 3; j++ );
		SCL_HI;

		val <<= 1;
	}
	CS_HI;
}

