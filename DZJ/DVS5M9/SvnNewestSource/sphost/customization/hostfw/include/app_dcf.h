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
#ifndef _APP_DCF_H_
#define _APP_DCF_H_

/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/
#include "sp5k_global_api.h"
#include "sp5k_dcf_api.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

#define DCF_DIR_NUM_MIN		100
#define DCF_FILE_NUM_MIN		1

#define DCF_DIR_NUM_MAX		999
#define DCF_FILE_NUM_MAX		9999


#define DCF_DIR_FREE_CHAR     "MEDIA"
#define DCF_FILE_FREE_CHAR    "SUNP"


/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
 
/**************************************************************************
 *          M O D U L E   V A R I A B L E S   R E F E R E N C E S         *
 **************************************************************************/

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/
UINT32 appDcfInit(UINT32 drive);
void appDcfKeySettingUpdate(UINT16 *dirKey,UINT16 *fileKey);
UINT32 appDcfNexDcfKeySet(UINT32 isReset, UINT16 dirKey, UINT16 fileKey);
UINT32 appDcfKeyFullCkeck(void);
BOOL appDcfRemainKeyQuery(UINT32 requestNum);


/**************************************************************************
 *                               M A C R O S                              *
 **************************************************************************/
#define IS_DCF_KEY_FULL   appDcfKeyFullCkeck()


#endif  /* _APP_DCF_H_ */


