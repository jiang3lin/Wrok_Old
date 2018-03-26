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

#include "api/sp5k_global_api.h"
#include "customization/dev_init.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define SCL_MSK  (1 << 14)
#define SDA_MSK  (1 << 15)

#define CS_MSK   (1 << 13)
#define RSET_MSK (1 << 12)
#define	BL       (0)
#define LCD_MSK  (CS_MSK | RSET_MSK | BL)

#define	SCL_LO	sp5kGpioWrite(SP5K_GPIO_GRP_DISP, SCL_MSK, 0)
#define	SCL_HI	sp5kGpioWrite(SP5K_GPIO_GRP_DISP, SCL_MSK, SCL_MSK)
#define	SDA_LO	sp5kGpioWrite(SP5K_GPIO_GRP_DISP, SDA_MSK, 0)
#define	SDA_HI	sp5kGpioWrite(SP5K_GPIO_GRP_DISP, SDA_MSK, SDA_MSK)

#define	CS_LO	sp5kGpioWrite(SP5K_GPIO_GRP_DISP, CS_MSK, 0)
#define	CS_HI	sp5kGpioWrite(SP5K_GPIO_GRP_DISP, CS_MSK, CS_MSK)

#define DELAY	{ UINT32 _delay; for (_delay=0 ; _delay<3 ; _delay++); }

/**************************************************************************
 *                     M O D U L E   V A R I A B L E S                    *
 **************************************************************************/
/*
    Vendor      bitsLen  lsbFirst
    ------      -------  --------
    AUO           16        0
    CASIO         12        1
    SAMSUNG       24        0
    SONY          16        0
    Toppoly       16        0
*/
void
disp5kSpiCtrl(
	UINT32 val,
	UINT32 bitsLen,
	UINT32 lsbFirst
)
{
	UINT32	i;
	UINT32	data;

#if	0
	printf("val=%04x, bitsLen=%d, lsbFirst=%d\n",val, bitsLen, lsbFirst);
#endif

	/* 1.SAMSUNG, Hitachi, LOAD/DATA/CLK HI before transfer */
	/* CS_HI; CS_EN; SDA_HI; SDA_EN; SCL_HI; SCL_EN;        */
	/* 2.others, LOAD HI before transfer                    */
	#ifdef FPGA
	sp5kGpioWrite (SP5K_GPIO_GRP_DISP, LCD_MSK, RSET_MSK | CS_MSK | SCL_MSK | SDA_MSK | BL);
	sp5kGpioCfgSet(SP5K_GPIO_GRP_DISP, LCD_MSK, LCD_MSK);
	#else
	sp5kGpioWrite (SP5K_GPIO_GRP_DISP, LCD_MSK, LCD_MSK);
	/*s Modify by Aries 09/10/05*/
	#if SP5K_LCD_TD025THEB1_UPS052||SP5K_LCD_TD027LCJT02_UPS052 || SP5K_LCD_TXDT250C_UPS052
	sp5kGpioWrite (SP5K_GPIO_GRP_DISP, SCL_MSK | SDA_MSK, 0);
	#else
	sp5kGpioWrite (SP5K_GPIO_GRP_DISP, SCL_MSK | SDA_MSK, SCL_MSK | SDA_MSK);
	#endif
	/*e Modify by Aries 09/10/05*/
	sp5kGpioCfgSet(SP5K_GPIO_GRP_DISP, SCL_MSK | SDA_MSK, SCL_MSK | SDA_MSK);
	sp5kGpioCfgSet(SP5K_GPIO_GRP_DISP, LCD_MSK, LCD_MSK);
	#endif
/*	CS_LO; CS_EN; SDA_LO; SDA_EN; SCL_LO; SCL_EN;*/

	if ( lsbFirst ) {
		data = val;
	}
	else {
		data = 0;
		for ( i = 0; i < 24; i++ ) {
			if ( val & 0x800000 ) {
				data |= (0x0001 << i);
			}
			val <<= 1;
		}
		data >>= (24 - bitsLen);
	}

	/* Start transfer:
	   LOAD:
	   HI -> LO
		1.SAMSUNG, Hitachi, DATA/CLK HI
		2.others, DATA/CLK LO
	   LO -> HI
		1.PVI3001A
	*/
	CS_LO;/* SDA_LO; SCL_LO;*/
	for ( i = 0; i < bitsLen; i++ ) {
		SCL_LO;
		if ( data & 0x0001 ) {
			SDA_HI;
		}
		else {
			SDA_LO;
		}
		DELAY;
		SCL_HI;
		data >>= 1;
		DELAY;
		SCL_LO;
		DELAY
		#if SP5K_LCD_TD025THEB1_UPS052||SP5K_LCD_TD027LCJT02_UPS052 || SP5K_LCD_TXDT250C_UPS052
		SDA_LO;
		#endif
	}
	CS_HI;
}

UINT32
disp5kSpiRead(
	UINT32 addr,
	UINT32 bitsLen,
	UINT32 lsbFirst
)
{
	UINT32	i, j;
	UINT32	data,val;

	#ifdef FPGA
	sp5kGpioWrite (SP5K_GPIO_GRP_DISP, LCD_MSK, RSET_MSK | CS_MSK | SCL_MSK | SDA_MSK | BL);
	sp5kGpioCfgSet(SP5K_GPIO_GRP_DISP, LCD_MSK, LCD_MSK);
	#else
	sp5kGpioWrite (SP5K_GPIO_GRP_DISP, LCD_MSK, LCD_MSK);
	/*s Modify by Aries 09/10/05*/
	#if SP5K_LCD_TD025THEB1_UPS052||SP5K_LCD_TD027LCJT02_UPS052 || SP5K_LCD_TXDT250C_UPS052
	sp5kGpioWrite (SP5K_GPIO_GRP_DISP, SCL_MSK | SDA_MSK, 0);
	#else
	sp5kGpioWrite (SP5K_GPIO_GRP_DISP, SCL_MSK | SDA_MSK, SCL_MSK | SDA_MSK);
	#endif
	/*e Modify by Aries 09/10/05*/
	sp5kGpioCfgSet(SP5K_GPIO_GRP_DISP, SCL_MSK | SDA_MSK, SCL_MSK | SDA_MSK);
	sp5kGpioCfgSet(SP5K_GPIO_GRP_DISP, LCD_MSK, LCD_MSK);
	#endif
		
	if ( lsbFirst ) {
		data = addr;
	}
	else {
		data = 0;
		for ( i = 0; i < 24; i++ ) {
			if ( addr & 0x800000 ) {
				data |= (0x0001 << i);
			}
			addr <<= 1;
		}
		data >>= (24 - bitsLen);
	}

	CS_LO;/* SDA_LO; SCL_LO;*/
	for ( i = 0; i < bitsLen ; i++ ) {
		SCL_LO;
		if ( data & 0x0001 ) {
			SDA_HI;
		}
		else {
			SDA_LO;
		}
		for ( j = 0; j < 3; j++ );
		SCL_HI;
		data >>= 1;
		for ( j = 0; j < 3; j++ );
		SCL_LO;
		for ( j = 0; j < 3; j++ );
		/*s Modify by Aries 09/10/05*/
		#if SP5K_LCD_TD025THEB1_UPS052||SP5K_LCD_TD027LCJT02_UPS052 || SP5K_LCD_TXDT250C_UPS052
		SDA_LO;
		#endif
		/*e Modify by Aries 09/10/05*/
	}
	
	val = 0;
	sp5kGpioCfgSet(SP5K_GPIO_GRP_DISP, SCL_MSK | SDA_MSK, SCL_MSK );
		
	for ( i = 0; i < 8; i++ ) {
		SCL_HI;
		sp5kGpioRead(SP5K_GPIO_GRP_DISP, SDA_MSK, &data);
		if(data & SDA_MSK)
			val |= (0x01 << (0x07 - i));
		for ( j = 0; j < 3; j++ );
		SCL_LO;
		for ( j = 0; j < 3; j++ );
	}
	CS_HI;
	return(val);
}




