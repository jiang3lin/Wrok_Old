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

/* device name */
static UINT8 gSensorName[] = "G-Sensor NULL";

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
	
}


static void
gSensorDriverConfig(
	void
)
{

}

void 
gSensorDriverWrite(
	UINT32 addr,
	UINT32 data
)
{
	
}

static void 
gSensorDriverSerRead(
	UINT32  addr,
	UINT32 *data
)
{
	
}

void 
gSensorDriverRead(
	UINT32 selID,
	UINT32 * param
)
{
	
}

UINT8 *
gSensorDriverNameGet(
	void
)
{
	return gSensorName;
}

