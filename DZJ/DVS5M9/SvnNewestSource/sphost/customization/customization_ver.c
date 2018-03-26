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
#include <string.h>
#include "common.h"
#include "stdio.h"
#include "sp5k_ver.h"

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
static UINT8 verStr[] = "$unp_1.00.33_app_customization";
static UINT8 buildNoStr[] = "0000";
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
/* -- External function -- */

/* -- External variable -- */

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/**************************************************************************
 *                                                                        *
 *  Function Name: customizationLibVersionGet                             *
 *                                                                        *
 *  Purposes: Return customization version                                *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *      pbuf: returns version string                                      *
 *                                                                        *
 *  Returns: SUCCESS                                                      *
 *           Error                                                        *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void
customizationLibVersionGet(
	UINT8 *pbuf
)
{
	memcpy(pbuf, verStr + 5, sizeof(verStr) - 5);
}

/**************************************************************************
 *                                                                        *
 *  Function Name: customizationBuildNumberGet                            *
 *                                                                        *
 *  Purposes: Return customization build No                               *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *      pbuf: returns build no string                                     *
 *                                                                        *
 *  Returns: void                                                         *
 *                                                                        *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void
customizationBuildNumberGet(
	UINT8 *pbuf
)
{
	memcpy(pbuf, buildNoStr, sizeof(buildNoStr));
}

