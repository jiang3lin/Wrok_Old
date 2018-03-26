/**************************************************************************
 *                                                                        *
 *         Copyright (c) 2005 by Sunplus Technology Co., Ltd.             *
 *                                                                        *
 *  This software is copyrighted by and is the property of Sunplus        *
 *  Technology Co., Ltd. All rights are reserved by Sunplus Technology    *
 *  Co., Ltd. This software may only be used in accordance with the       *
 *  corresponding license agreement. Any unauthorized use, duplication,   *
 *  distribution, or disclosure of this software is expressly forbidden.  *
 *                                                                        *
 *  This Copyright notice MUST not be removed or modified without prior   *
 *  written consent of Sunplus Technology Co., Ltd.                       *
 *                                                                        *
 *  Sunplus Technology Co., Ltd. reserves the right to modify this        *
 *  software without notice.                                              *
 *                                                                        *
 *  Sunplus Technology Co., Ltd.                                          *
 *  19, Innovation First Road, Science-Based Industrial Park,             *
 *  Hsin-Chu, Taiwan, R.O.C.                                              *
 *                                                                        *
 *  Author:Stephen Hung                                                   *
 *                                                                        *
 **************************************************************************/
/* DO NOT EDIT the next 2 lines for auto revision by subversion
$Rev$
$Date::                     $
 */

#ifndef _SYSG_RESOURCE_DEF_H_
#define _SYSG_RESOURCE_DEF_H_

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
/* -- Resource arrangement of system -- */
/* ID for no free resource file*/
typedef enum {
	SYSG_RESOURCE_ID_EXPOSURE           	= 0x01,
	SYSG_RESOURCE_ID_AGC                	= 0x02,
	SYSG_RESOURCE_ID_BP_VIEW            	= 0x03,
	SYSG_RESOURCE_ID_BP_CAPTURE         	= 0x04,
	SYSG_RESOURCE_ID_EDGE               	= 0x05,
	SYSG_RESOURCE_ID_RB_GAMMA           	= 0x06,
	SYSG_RESOURCE_ID_G_GAMMA            	= 0x07,
	SYSG_RESOURCE_ID_AE_CFG             	= 0x08,
	SYSG_RESOURCE_ID_AWB_CTT            	= 0x09,
	SYSG_RESOURCE_ID_HDR                	= 0x0A,
	SYSG_RESOURCE_ID_USB_STR            	= 0x0B,
	SYSG_RESOURCE_ID_ADF_UDF            	= 0x0C,
	SYSG_RESOURCE_ID_FONT               	= 0x0D,
	SYSG_RESOURCE_ID_STRING             	= 0x0E,
	SYSG_RESOURCE_ID_DATE_STAMP_FONT    	= 0x0F,
	SYSG_RESOURCE_ID_CLICK_SOUND        	= 0x10,
	SYSG_RESOURCE_ID_Y_GAMMA            	= 0x11,
	SYSG_RESOURCE_ID_LSC                	= 0x12,
	SYSG_RESOURCE_ID_ICON               	= 0x13,
	SYSG_RESOURCE_ID_EDGE_E_LUT         	= 0x14,
	SYSG_RESOURCE_ID_UV_LUT             	= 0x15,
	SYSG_RESOURCE_ID_DATE_STAMP_FONT_2  	= 0x16,
	SYSG_RESOURCE_ID_PRE_GAMMA1         	= 0x17,
	SYSG_RESOURCE_ID_PRE_GAMMA2         	= 0x18,
	SYSG_RESOURCE_ID_ICON_2             	= 0x19,
	SYSG_RESOURCE_ID_ICON_3             	= 0x1A,
	SYSG_RESOURCE_ID_ICON_4             	= 0x1B,
	SYSG_RESOURCE_ID_ICON_5             	= 0x1C,
    /* new for 5110 */
	SYSG_RESOURCE_ID_AHS					= 0x1D,
	SYSG_RESOURCE_ID_ANTI_ALIAS				= 0x1E,
	SYSG_RESOURCE_ID_HUE_CORRECT			= 0x1F,
	SYSG_RESOURCE_ID_XDENOISE				= 0x20,
	#if SPCA5210
	SYSG_RESOURCE_ID_AHD					= 0x21,
	#endif
	SYSG_RESOURCE_ID_XDN_UVCLIP_LUT     	= 0x22,

    /* new for 5310 */
	SYSG_RESOURCE_ID_CORNER_EDGE			= 0x23,
	SYSG_RESOURCE_ID_WDR					= 0x25,
	SYSG_RESOURCE_ID_CA						= 0x26,
	SYSG_RESOURCE_ID_3D_LUT					= 0x27,
	SYSG_RESOURCE_ID_LDC					= 0x28,
	SYSG_RESOURCE_ID_ARD					= 0x29,

	SYSG_RESOURCE_ID_FQV_AHD				= 0x2A,

	SYSG_RESOURCE_ID_HDR_WEI				= 0x2B,
	SYSG_RESOURCE_ID_HDR_DIV				= 0x2C,

	SYSG_RESOURCE_ID_PRE_AHD_HUE_CORRECT	= 0x2D,
	SYSG_RESOURCE_ID_PRE_AHD_ANTI_ALIAS		= 0x2E,
	SYSG_RESOURCE_ID_PRE_AHD_EDGE_E_LUT		= 0x2F,
	SYSG_RESOURCE_ID_PRE_AHD_CORNER_EDGE	= 0x30,
	SYSG_RESOURCE_ID_PRE_AHD_EDGE			= 0x31,

	SYSG_RESOURCE_ID_AHD_ANTI_ALIAS			= 0x32,

	SYSG_RESOURCE_ID_REDO_HUE_CORRECT		= 0x33,
	SYSG_RESOURCE_ID_REDO_ANTI_ALIAS		= 0x34,
	SYSG_RESOURCE_ID_REDO_EDGE_E_LUT		= 0x35,
	SYSG_RESOURCE_ID_REDO_CORNER_EDGE		= 0x36,
	SYSG_RESOURCE_ID_REDO_EDGE				= 0x37,

	SYSG_RESOURCE_ID_API_INVOKE				= 0x80,

	SYSG_RESOURCE_ID_END
} sysg_res_e;

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/


#endif /* _SYSG_RESOURCE_DEF_H_ */

