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
#define HOST_DBG 1
#include "app_com_def.h"
#include "app_com_api.h"
#include "gpio_custom.h"
#include "app_tvlcd.h"
#include "sp5k_disk_api.h"
#include "app_dev_plug.h"

static UINT32 isDiskMount = FALSE;

void
appCardPlugStateInit(
	void
)

{
	UINT32 newDisk;
	if ((!IS_CARD_EXIST) && (appActiveDiskGet() == SP5K_DRIVE_NAND)) {
		//sp5kHostMsgSend(APP_UI_MSG_DUMMY, 0x00);
		appStateChange(appPreviousStateGet(),STATE_PARAM_CARD_PLUG);
 	}
	else {
	#if ICAT_WIFI
		newDisk = SP5K_DRIVE_NAND;
	#else
		newDisk = IS_CARD_EXIST? SP5K_DRIVE_SD : SP5K_DRIVE_NAND;
	#endif
		appDiskMount(newDisk);
		isDiskMount = TRUE;
 	}
}


void
appCardPlugState(
	UINT32 msg,
	UINT32 param
)
{
	DBG_PRINT("%s : [0x%x] [0x%x]\n",__FUNCTION__,msg,param);

	switch (msg) {
	case APP_STATE_MSG_INIT:
		RPrintf("appCardPlugState");
		isDiskMount = FALSE;
		appCardPlugStateInit();
		break;
	case APP_STATE_MSG_CLOSE:
		appStateCloseDone();
		break;
	case SP5K_MSG_DISK_MOUNT_COMPLETE:
	case SP5K_MSG_DISK_MOUNT_FAIL:
	case SP5K_MSG_DISK_ERROR:
		printf("$$ SP5K_MSG_DISK_MOUNT_COMPLETE\n");
		isDiskMount = FALSE;
		appHotPlugNextStChange();
		break;
	/*
	case APP_UI_MSG_DUMMY:
		if(!isDiskMount) {
			appStateChange(appPreviousStateGet(),STATE_PARAM_CARD_PLUG);
		}
		break;
	*/
	default:

		break;
	}

}

