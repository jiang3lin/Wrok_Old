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


#ifndef _APP_PAL_H_
#define _APP_PAL_H_


/* check app_osd_init.c:appPalInit() to validate following palette indexs
   todo: a mechanism to do static check for this macro */
/* color palette index */
#define PAL_TRANSPARENCY	0
#define PAL_NOT_DRAW		0	/* gfx skip this color so we can see underneath OSD color */
#define PAL_ERASE_OSD		8	/* gfx draw this transparent color and overwrite underneath OSD color */

#define BLEND(idx)		((idx)+8)

typedef enum _COLOR_DEF
{
	PAL_NOCOLOR,
	PAL_BLACK,
	PAL_GREEN_2,
	PAL_GREEN_3,
	PAL_GREEN_4,
	PAL_YELLOW_1,				
	PAL_BROWN_1,		
	PAL_RED, 			
	PAL_DARKBLUE_1, 	
	PAL_WHITE, 	
	PAL_GRAY_5, 	
	PAL_LIGHTBLUE_1, 	
	PAL_GRAY_2, 	
	PAL_BLACK_1, 	
	PAL_GRAY_1, 	 	
	PAL_BLUE,  
	PAL_GRAY_3, 	
	PAL_GRAY, 	
	PAL_GREEN, 	
	PAL_ORANGEB, 	
	PAL_BLUE_1,  
	PAL_PURPLE_1,
	PAL_DARKBLUE, 
	PAL_IRON, 
	PAL_GRAY_4,
	PAL_RED_1,
	PAL_GREEN_1,   
	PAL_YELLOW,
	PAL_BLUE_2,
	PAL_PINK_4,
	PAL_LIGHTBLUE,
	PAL_BROWN, 
	
}COLOR_DEF;



typedef enum _BLEND_TYPE
{
	PAL_BLEND_0,		//#0
	PAL_BLEND_25,    	//#1
	PAL_BLEND_50,		//#2
	PAL_BLEND_75,		//#3
	PAL_BLEND_100		//#4
}BLEND_TYPE;



/* color palette */

#define COLOR_TRANSPARENT 			0x040404
#define COLOR_BLACK       			0x000400
#define COLOR_PINK        				0xC44562
#define COLOR_PINK_1        			0xDB5D7A
#define COLOR_PINK_2      			0xF07391
#define COLOR_PINK_3      			0xEE9BAF
#define COLOR_BLACK_1       			0x0E1626
#define COLOR_GRAY        			0x495367
#define COLOR_BLUE        				0x85C7F5
#define COLOR_YELLOWGREEN        	0x7C8920
#define COLOR_YELLOWGREEN_1       	0xDEEA86
#define COLOR_YELLOWBROWN        	0xC6A00C
#define COLOR_RED         				0xFF0000
#define COLOR_WHITE       			0xFAFAFA
#define COLOR_GRAY_1      			0x212121
#define COLOR_GRAY_2      			0x585858
#define COLOR_GRAY_3      			0xD0D0D0
#define COLOR_PURPLE      			0xB400FF
#define COLOR_GREEN       			0x00FF00
#define COLOR_ORANGE    			0xF7941D
#define COLOR_BLUE_1        			0xD2E6FF
#define COLOR_PURPLE_1    			0x8000FF
#define COLOR_DARK_BLUE   			0x008080
#define COLOR_IRON        				0x323232
#define COLOR_GRAY_4      			0x808080
#define COLOR_RED_1       			0xFF0000
#define COLOR_GREEN_1     			0x449F59
#define COLOR_YELLOW      			0xFFFF00
#define COLOR_BLUE_2     			0x0000FF
#define COLOR_PINK_4      			0xFF00FF
#define COLOR_LIGHT_BLUE  			0x00FFFF
#define COLOR_BROWN       			0x800000
/*	add in EZ_GUI start	*/
#define COLOR_GREEN_2       			0x0D9D17
#define COLOR_GREEN_3      			0x58D161
#define COLOR_GREEN_4     			0x86E48D
#define COLOR_YELLOW_1    			0xF5D60A
#define COLOR_BROWN_1       			0x6A4704
#define COLOR_DARKBLUE_1			0x14181B
#define COLOR_GRAY_5				0xF0F0F0 /*modified by GQ,for mantis 46797*/	//0xAFAFAF
#define COLOR_LIGHTBLUE_1			0x75E0FF
/*	add in EZ_GUI end		*/

// Jeff Cheng
// The pip layer color setting may need some control/manage interface
// methods like osd layer. Here is temperay definition. The usage is 
// to set gfx fucntions directly like below:
//
// 		sp5kGfxAttrSet(SP5K_GFX_PEN_COLOR, PIP_COLOR565_WHITE, 0, 0, 0);
//		sp5kGfxAttrSet(SP5K_GFX_FILL_COLOR, PIP_COLOR565_WHITE, 0, 0, 0);
//
// You can modify this while you finish the relatived functions.
#define PIP_COLOR565_WHITE 			0xFFFF
#define PIP_COLOR565_RED   			0xF800
#define PIP_COLOR565_GREEN 			0x07E0
#define PIP_COLOR565_BLUE 			0x001F

#endif /* _APP_PAL_H_ */




