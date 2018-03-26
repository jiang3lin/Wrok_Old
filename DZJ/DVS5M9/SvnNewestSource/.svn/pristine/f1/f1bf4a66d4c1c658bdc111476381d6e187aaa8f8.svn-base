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

#include <stdio.h>
#include "common.h"
#include "customization\solution_def.h"



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
void   halRtcRegWrite(UINT32 addr, UINT32 data);
UINT32 halRtcRegRead(UINT32 addr);

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

UINT32 
appFastBootLensInfoGet(
	UINT8 *pMode, 
	UINT8 *pBattType, 
	UINT8 *pBattLevel
)
{
	*pMode = halRtcRegRead(0x20);
	if (*pMode!=halRtcRegRead(0x21)) 
	{
		return FAIL;
	}
	
	*pBattType = halRtcRegRead(0x22);
	if (*pBattType!=halRtcRegRead(0x23)) 
	{
		return FAIL;
	}
	
	*pBattLevel = halRtcRegRead(0x24);
	if (*pBattLevel!=halRtcRegRead(0x25)) 
	{
		return FAIL;
	}
	return SUCCESS;
}


void 
appFastBootLensModeSet(
	UINT32 mode
)
{
	halRtcRegWrite(0x20, mode);
	halRtcRegWrite(0x21, mode);
}

void 
appFastBootLensBattSet(
	UINT32 type, 
	UINT32 level
)
{
	halRtcRegWrite(0x22, type);
	halRtcRegWrite(0x23, type);
	halRtcRegWrite(0x24, level);
	halRtcRegWrite(0x25, level);
}

