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
#include "app_osd.h"

#include "middleware/Pwrlog.h"

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
#if 0 
 UINT8 ResPath[256]={'0'};
#endif
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/* allow diff within 1 min */
int 
RtcDiff(
	struct tm* rtc0, 
	struct tm* rtc
)
{
	if (rtc->tm_sec>=rtc0->tm_sec)
	{
		return rtc->tm_sec - rtc0->tm_sec;
	}
	else
	{
		return 60 + rtc->tm_sec - rtc0->tm_sec;
	}
}

void
appHmsStrMake(
	char *buf,
	UINT32 sec
)
{
	if (sec>=3600)
	{
		sprintf(buf, "%02d:%02d:%02d", sec/3600, sec%3600/60, sec%60);
	}
	else
	{
		sprintf(buf, "%02d:%02d", sec/60, sec%60);
	}
}

#define POWER2_ARG_MIN	-16
#define POWER2_ARG_MAX	16

SINT32
appPower2Get(
	SINT32 numerator,
	UINT32 denominator,
	UINT32 *pResultNumerator,
	UINT32 *pResultDenominator
)
{
	SINT32 val;
	if (numerator <= POWER2_ARG_MIN * (SINT32)denominator) 
	{
		val = -1;
		*pResultNumerator = 1;
		*pResultDenominator = (1UL << (UINT32)-POWER2_ARG_MIN) - 1;
	} 
	else if (numerator >= POWER2_ARG_MAX * (SINT32)denominator) 
	{
		val = 1;
		*pResultNumerator = (1UL << POWER2_ARG_MAX) - 1;
		*pResultDenominator = 1;
	}
	else 
	{
		val = power2xyND(numerator, denominator, pResultNumerator, pResultDenominator);
	}
	return val;
}

UINT32 WriteFile(UINT8 *fname, UINT8 *buf, UINT32 size)
{
	UINT32 fd;
	printf("%s %s from %x l=%d\n", __FUNCTION__, fname, (UINT32)buf, size);
	fd = sp5kFsFileOpen(fname, SP5K_FS_OPEN_CREATE);
	if (fd) 
	{
		sp5kFsFileWrite(fd, buf, size);
		sp5kFsFileClose(fd);
		return SUCCESS;
	}
	return FAIL;
}

UINT32 ReadFile(UINT8 *fname, UINT8 *buf, UINT32 size)
{
	UINT32 fd;
	printf("%s %s from %x l=%d\n", __FUNCTION__, fname, (UINT32)buf, size);
	fd = sp5kFsFileOpen(fname, SP5K_FS_OPEN_RDONLY);
	if (fd) 
	{
		sp5kFsFileRead(fd, buf, size);
		sp5kFsFileClose(fd);
		return SUCCESS;
	}
	return FAIL;
}

UINT32 ReadFileEx(UINT8 *fname, UINT8**pbuf, UINT32 *psize )
{
	UINT32 fd,size,ret=FAIL;
	UINT8 *buf;
	fd = sp5kFsFileOpen(fname, SP5K_FS_OPEN_RDONLY);
	if (fd) 
	{
		size = sp5kFsFileSizeGet(fd);
		buf = sp5kMalloc(size);
		if (buf)
		{
			printf("%s %s from %x l=%d\n", __FUNCTION__, fname, (UINT32)buf, size);
			sp5kFsFileRead(fd, buf, size);
			*pbuf = buf;
			*psize = size;
			ret = SUCCESS;
		}
		sp5kFsFileClose(fd);
	}
	return ret;
}

BOOL
appFileExistCheck(
	UINT8 *filename
)
{
	UINT32 fd, readLen = 0;	
	
	fd = sp5kFsFileOpen(filename, FS_OPEN_RDONLY);
	if (fd != 0)
	{
		readLen = sp5kFsFileSizeGet(fd);	
		sp5kFsFileClose(fd);
		if (readLen) 
		{
			return TRUE;
		}
	}		
	return FALSE;
}

#if 0
UINT8 * appRootGet(UINT8 *input) 
{

#if RESOURCE_IN_RAMDISK 
    const UINT8 *p = input;
    UINT8 len = strlen(input);

    //printf("<lester> appRootGet %s %s %d \n",input,ResPath,len);    
	if(input==NULL || len<=0)
	{        
		return NULL;
	}
	else
	{  
       
		if((input[0]=='A'||input[0]=='B')&&input[1]==':')
		{
			strncpy (ResPath, input, len);
			strncpy (ResPath, "H",1);
		}    
        else
            return input;
	}
	ResPath[len]='\0';		
    
	return ResPath;
#else
    return input;
#endif
}
#endif
