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
#include "common.h"
#include <stdio.h>
#include <string.h>
#include "app_com_def.h"
#include "app_com_api.h"
#include "app_dbg_api.h"
#include "app_util.h"
#include "app_lens_private.h"
#include "app_lens_motor.h"
#include "app_lens_api.h"
//#include "app_lens_zoom.h"
//#include "app_lens_focus.h"
//#include "app_lens_iris.h"
//#include "app_lens_mshut.h"
//#include "app_ui_msg.h"

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
#if KIT_WITH_LENS 
static UINT32 glensLedPowerStat = 0;
static BOOL isLensLedOn = FALSE; /* %REVIEW% : Not good idea !! */
#endif

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/*-------------------------------------------------------------------------
 *  File Name : lensLedPowerQuery
 *------------------------------------------------------------------------*/
#if KIT_WITH_LENS
BOOL
lensLedPowerQuery (
	UINT32 combo_id
)
{
	BOOL isLedOn;

	if (combo_id==LENS_LED_ID_ALL) {
		isLedOn = TRUE;
		isLedOn = (isLedOn && (!!(glensLedPowerStat & LENS_LED_ID_FHM)));
		isLedOn = (isLedOn && (!!(glensLedPowerStat & LENS_LED_ID_ZCLK)));
		isLedOn = (isLedOn && (!!(glensLedPowerStat & LENS_LED_ID_ZHM)));
	} else {
		isLedOn = !!(glensLedPowerStat & combo_id);
	}
	return isLedOn;
}
#endif
/*-------------------------------------------------------------------------
 *  File Name : lensLedPowerOn
 *------------------------------------------------------------------------*/
void
lensLedPowerOn (
	UINT32 combo_id
)
{
#if KIT_WITH_LENS
	LENS_ISR_DBG("LED_ON: combo_id(0x%x)\n", combo_id);

	isLensLedOn = TRUE; /*-- %TODO%: Here accompanys potential side effect !! */

	if (combo_id & LENS_LED_ID_ZHM) {
		IO_LENS_LED_ZHM_PWR_ON;
		glensLedPowerStat |= LENS_LED_ID_ZHM;
	}

	if (combo_id & LENS_LED_ID_ZCLK) {
		IO_LENS_LED_ZCLK_PWR_ON;
		glensLedPowerStat |= LENS_LED_ID_ZCLK;
	}

	if (combo_id & LENS_LED_ID_FHM) {
		IO_LENS_LED_FHM_PWR_ON;
		glensLedPowerStat |= LENS_LED_ID_FHM;
	}

	LENS_SLEEP(LENS_LED_ACTUATE_LAG);
#endif
}


/*-------------------------------------------------------------------------
 *  File Name : lensLedPowerOff
 *------------------------------------------------------------------------*/
void
lensLedPowerOff (
	UINT32	combo_id
)
{
return;
#if KIT_WITH_LENS
	LENS_ISR_DBG("LED_OFF: combo_id(0x%x)\n", combo_id);

	isLensLedOn = FALSE; /*-- %TODO%: Here accompanys potential side effect !! */

	if (combo_id & LENS_LED_ID_ZHM) {
		IO_LENS_LED_ZHM_PWR_OFF;
		glensLedPowerStat &= ~LENS_LED_ID_ZHM;
	}

	if (combo_id & LENS_LED_ID_ZCLK) {
		IO_LENS_LED_ZCLK_PWR_OFF;
		glensLedPowerStat &= ~LENS_LED_ID_ZCLK;
	}

	if (combo_id & LENS_LED_ID_FHM) {
		IO_LENS_LED_FHM_PWR_OFF;
		glensLedPowerStat &= ~LENS_LED_ID_FHM;
	}
#endif
}

/*-------------------------------------------------------------------------
 *  File Name : lensLedRead
 *------------------------------------------------------------------------*/
UINT32
lensLedRead (
	lensLedID_t id
)
{
#if KIT_WITH_LENS
	UINT32 iolevel;

	LENS_VERBOSE("%s: id(0x%x)\n", __FUNCTION__, id);

	switch (id) {
	default :
		LENS_DBG("\n\n@@@@@ %s: ALERT !! Illegal Lens Led ID(%x)\n\n", __FUNCTION__, id);
		iolevel = ~((UINT32)0);
		#if 0
			while(1) { }
		#endif
		break;
	case LENS_LED_ID_ZHM :
		iolevel = IO_LENS_LED_ZHM_GET;
		break;
	case LENS_LED_ID_ZCLK:
		iolevel = IO_LENS_LED_ZCLK_GET;
		break;
	case LENS_LED_ID_FHM:
		iolevel = IO_LENS_LED_FHM_GET;
		break;
	}

	return (iolevel);
#else
	return 0;
#endif
}


