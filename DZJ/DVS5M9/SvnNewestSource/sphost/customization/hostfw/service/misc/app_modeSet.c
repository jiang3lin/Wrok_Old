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
#define HOST_DBG 0
#include "app_com_def.h"
#include "sp5k_global_api.h"
#include "sp5k_rsvblk_api.h"
#include "sp5k_aud_api.h"
#include "sp5k_gfx_api.h"
#include "sp5k_fs_api.h"
#include "sp5k_os_api.h"
#include "sp5k_utility_api.h"
#include "sp5k_modesw_api.h"


/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
static BOOL appModeWaitFlag = FALSE;

extern UINT8 gVideoSizeTag ;

void
appModeWaitCfg(
	BOOL wait
)
{
	
	appModeWaitFlag = wait;
}


void
appModeSet(
	UINT32 mode
)
{
	UINT32 ret;
	#ifdef NO_VIDEO_PREVIEW_MODE
	
	UINT32 currMode;
	BOOL bEnfit = 0;
	if(mode == SP5K_MODE_VIDEO_PREVIEW)
	{
		sp5kModeGet(&currMode);
		if(currMode == SP5K_MODE_VIDEO_RECORD)
		{
			ret = sp5kModeSet(SP5K_MODE_VIDEO_PREVIEW);
			/*sp5kModeWait(SP5K_MODE_VIDEO_PREVIEW); */
			appHostMsgWait(SP5K_MSG_MODE_READY,NULL,5000);
		}
		bEnfit = 1;
		mode = SP5K_MODE_STILL_PREVIEW;
	}
	else if(mode == SP5K_MODE_VIDEO_RECORD)
	{
		ret = sp5kModeSet(SP5K_MODE_VIDEO_PREVIEW);
		sp5kModeWait(SP5K_MODE_VIDEO_PREVIEW);
	}

	
	if(mode == SP5K_MODE_STILL_PREVIEW)
	{
		if(((appActiveStateGet()==APP_STATE_VIDEO_SUBMENU)
			||(appActiveStateGet()==APP_STATE_VIDEO_PREVIEW)
			||(appActiveStateGet()==APP_STATE_VIDEO_REC))) 
		{
			UINT32  img_w, img_h;	
			appVideoSizeGet(gVideoSizeTag, &img_w, &img_h);
			if ((img_w ==848)||(img_w ==1280))
			{
				bEnfit = 1;
			}
			else
			{
				bEnfit = 0;
			}
		}
		#if KIT_SENSOR_FIT_IMAGE
		sp5kModeCfgSet(SP5K_MODE_CFG_PV_DISP_FIT, bEnfit);
		#endif
	}
	#endif

	#if SP5K_SENSOR_OV2715
	sp5kModeCfgSet(SP5K_MODE_CFG_PV_DISP_FIT, 1);
	#endif

	ret = sp5kModeSet(mode);
	if (appModeWaitFlag)
	{
		sp5kModeWait(mode);		
		//appHostMsgWait(SP5K_MSG_MODE_READY,NULL,5000);
	}
}

UINT32 appModeGet(void)
{
	UINT32 Mode;
	sp5kModeGet(&Mode);

	return Mode;
}

