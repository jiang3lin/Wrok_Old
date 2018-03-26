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
#include "app_com_api.h"
#include "sp5k_fs_api.h"
#include "sp5k_os_api.h"
#include "sp5k_utility_api.h"


/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define FW_FILE   	"D:\\SPHOST.BRN"

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern void appPsuedoPowerOff(UINT32 powerDown);
extern UINT8 sp5kCardISP(UINT8 *fwfile,UINT8 opt);

UINT16 appGetSum16( UINT16 *Buf, UINT32 Size )
{
	UINT16 Sum = 0;
	while( Size ) 
	{
		Sum += Buf[--Size];
	}
	return Sum;
}

UINT32 appGetSum32( UINT32 *Buf, UINT32 Size )
{
	UINT32 Sum = 0;
	while( Size ) 
	{
		Sum += Buf[--Size];
	}
	return Sum;
}

UINT32 appByteSum(UINT8 *buf, UINT32 len)
{
	UINT32 i,sum;
	for (i=sum=0 ; i<len ; i++) 
	{
		sum += buf[i];
	}
	return sum;
}

UINT32
appFirmwareUpdate(
	void
)
{
#if SPCA5310 
	UINT32 fd, ofs, l;
	UINT8 buf[64], *fw=0;
	UINT32 curMode;

	fd = sp5kFsFileOpen(FW_FILE, SP5K_FS_OPEN_RDONLY);
	if( fd ) {
		sp5kFsFileRead(fd, buf, sizeof(buf));
		ofs = *(UINT32*)(buf+0x1c);
		l = sp5kFsFileSizeGet(fd);
		if (ofs>=l) {
			sp5kFsFileClose(fd);
			goto SDCard_Download_Fail;
		}
		l -= ofs;
		fw = sp5kMalloc(l);
		sp5kFsFileSeek(fd, ofs, SP5K_FS_SEEK_SET);
		sp5kFsFileRead(fd, fw, l);
		sp5kFsFileClose(fd);

		sp5kModeGet(&curMode);
		if(curMode != SP5K_MODE_STANDBY){
			sp5kModeSet(SP5K_MODE_STANDBY);
			sp5kModeWait(SP5K_MODE_STANDBY);
		}
		
		printf("start FW update...\n");
		//if (sp5kDscFirmwareUpdate( brnFile )==SUCCESS){
		if (sp5kDscFirmwareUpdate( FW_FILE,0 )==SUCCESS)
		{
			printf("FW update OK!\n");
			return SUCCESS;
		}
	}

SDCard_Download_Fail:
	printf("Update err\n");
	if (fw) sp5kFree(fw);
	return FAIL;

#else
	UINT32 ret = FAIL;
	UINT32 fd = 0;
	//update firmware
	fd = sp5kFsFileOpen(FW_FILE, SP5K_FS_OPEN_RDONLY);
	if( fd ) 
	{
		sp5kFsFileClose( fd );
		//power off
		appPsuedoPowerOff(FALSE);
		printf("FW update start... %s\n", FW_FILE);
		ret = sp5kDscFirmwareUpdate( FW_FILE );
		printf("FW update done (%d)\n", ret);
		return ret;
	}	
	printf("[ERR] %s (%d)\n", __FUNCTION__,__LINE__);
	return ret;
#endif	
	
}


