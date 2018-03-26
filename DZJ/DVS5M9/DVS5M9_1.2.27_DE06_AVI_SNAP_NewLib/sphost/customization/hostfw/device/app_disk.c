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
#include "sp5k_disk_api.h"
#include "app_dev_plug.h"
#include "gpio_custom.h"
#include "app_dcf.h"
#include "app_cdfs_api.h"
#include "app_stringid.h"
#include "sp5k_global_api.h"
//#include "app_disk.h"
#include "customization/dev_init.h"

extern UINT32 appLogMsgToStorage(UINT32 msg,UINT32 param);

typedef enum {
	APP_CARD_MOUNT_OK = 0x00,
	APP_CARD_MOUNT_FAIL = 0x01,
} appCardMountStatus_e;

typedef struct appDiskStatus_s {
	appDiskInfo_t diskInfo;
	UINT8 isMountReady;
	UINT8 isMountFailed;
	UINT8 isNandMounted;
	UINT8 isCardMounted;
	UINT8 diskStatus;
} appDiskStatus_t;

static appDiskStatus_t appDiskStatus = { {SP5K_DRIVE_NAND,0,0,0}, FALSE, FALSE, FALSE, FALSE, APP_DISK_STATUS_NO_CARD};
UINT8 cdfsFirstCreate = FALSE;

void appSdPowerRecycle(void)
{
	#if !ICAT_WIFI
	sp5kGpioCfgSet(SP5K_GPIO_GRP_FMH,0x00000020,0x00000020);
	sp5kGpioWrite(SP5K_GPIO_GRP_FMH,0x00000020,0x00000020);/*sd power off*/
	
	sp5kGpioCfgSet(SP5K_GPIO_GRP_FML,0x000007e0,0x000007e0);/*FM GPIO 5,6,7,8,9,10*/
	sp5kGpioWrite(SP5K_GPIO_GRP_FML,0x000007e0,0);
	appTimeDelay(200,1);
	sp5kGpioFuncSet(SP5K_GPIO_GRP_FML,0x000007e0,0);
	sp5kGpioWrite(SP5K_GPIO_GRP_FMH,0x00000020,0);/*sd power on*/
	appTimeDelay(50,1);
	#endif
}

void
_diskMountStart(
	UINT32 driver
)
{
	profLogPrintf(0, "mount start(%d)",driver);
	DBG_PRINT("Disk Mount(%d) ",driver);
	if(IS_CARD_EXIST)
	{
		appSdPowerRecycle();
	}
	sp5kDiskMount(driver);
	appDiskStatus.diskInfo.actDsk = driver;
	appDiskStatus.isMountReady= FALSE;
	appDiskStatus.isMountFailed= FALSE;
}

void
_diskMountCompleted(
	UINT32 param
)
{
	DBG_PRINT("Mount Ready(%d) ",param);
	appDiskStatus.isMountReady= TRUE;
	appDiskStatus.isMountFailed= param;

	if (appDiskStatus.diskInfo.actDsk==SP5K_DRIVE_NAND)
	{
		appDiskStatus.isNandMounted = !param;
	} 
	else if (appDiskStatus.diskInfo.actDsk==SP5K_DRIVE_SD)
	{
		appDiskStatus.isCardMounted = !param;
	}
}

void
_diskInfoGet(
	void
)
{
	UINT32 val,drive;
	//disk info get
	drive = appDiskStatus.diskInfo.actDsk;
	sp5kDiskInfoGet(drive, SP5K_SIZE_BY_MEGA_BYTE, &val);
	appDiskStatus.diskInfo.dskMbSz = val;
	//DBG_PRINT("Drive %d Total Size [%d] MB\n",drive,val);
	sp5kDiskInfoGet(drive, SP5K_FREE_SIZE_BY_KBYTE, &val);
	appDiskStatus.diskInfo.freeSz = val;
	//DBG_PRINT("Drive %d Free  Size [%d] KByte\n",drive,val);
	sp5kDiskInfoGet(drive, SP5K_CLUSTER_SIZE_BY_BYTES, &val);
	appDiskStatus.diskInfo.cluSz= val;
	//DBG_PRINT("Drive %d Cluster Size [%d] KByte\n",drive, appDiskStatus.diskInfo.cluSz);
}

UINT32
_diskInit(
	UINT32 drive
)
{
	if (FAIL==appDcfInit(drive))
	{
		appDiskStatus.isMountFailed = TRUE;
		if (appDiskStatus.diskInfo.actDsk==SP5K_DRIVE_NAND)
		{
			appDiskStatus.isNandMounted = FALSE;
		} 
		else if (appDiskStatus.diskInfo.actDsk==SP5K_DRIVE_SD)
		{
			appDiskStatus.isCardMounted = FALSE;
		}
		printf("sp5k DCF init failed !!\n");
		return FAIL;
	}
	_diskInfoGet();

	return SUCCESS;
}


void
appDiskMountMsgHandler(
	UINT32 msg,
	UINT32 param
)
{
	switch( msg){
	case SP5K_MSG_DISK_MOUNT_START:
		//do nothing
		break;
	case SP5K_MSG_DISK_MOUNT_COMPLETE:
		DBG_PRINT("SP5K_MSG_DISK_MOUNT_COMPLETE(%d)\n",param);
		_diskMountCompleted(APP_CARD_MOUNT_OK);
		_diskInit(param);
		#if  1
		{
			UINT32 ret;
			ret = sp5kFatVolumeLabelSet("D:", "MD 87156");
			DBG_PRINT("\nsd card volume label set %s\n", ((ret==SUCCESS)? "SUCCESS":"FAIL"));
		}
		#endif
		appLogMsgToStorage(msg,param); /*add for SD log */
		#if SP5K_CDFS_OPT
		if(cdfsFirstCreate == FALSE)
		{
			cdfsFirstCreate = TRUE;
			appCdfsInit();
		}
		#else
		{			
			sp5kDcfRecoverFileList_t  *pList;
			sp5kDcfRecoverListGet(&pList);
			if(pList){
				printf("plist->path:%s\n",pList->pathName);
				sp5kDcfFsFileOperate(SP5K_DCF_FILEOP_RECOVER_FILE, (UINT32)pList, 0, 0, 0);
			}
		}
		#endif
		break;
	case SP5K_MSG_DISK_MOUNT_FAIL: /* simulate card remove if mount fail */
		printf("[ERR]SP5K_MSG_DISK_MOUNT_FAIL(%d)\n",param );
		_diskMountCompleted(APP_CARD_MOUNT_FAIL);
		#if SP5K_CDFS_OPT
		appCdfsFileListTabInit(CDFS_FILE_FOLDER_VIDEO);
		appCdfsFileListTabInit(CDFS_FILE_FOLDER_JPG);
		appCdfsFileListTabInit(CDFS_FILE_FOLDER_EVENT);
		#endif

		//show card error
		break;
	case SP5K_MSG_DISK_ERROR:
		printf("[ERR]SP5K_MSG_DISK_ERROR(%d)\n",param );
		_diskMountCompleted(APP_CARD_MOUNT_FAIL);
		#if SP5K_CDFS_OPT
		appCdfsFileListTabInit(CDFS_FILE_FOLDER_VIDEO);
		appCdfsFileListTabInit(CDFS_FILE_FOLDER_JPG);
		appCdfsFileListTabInit(CDFS_FILE_FOLDER_EVENT);
		#endif
		//show card error
		break;
	default:
		//do nothing
		break;
	}
}


UINT32
appActiveDiskSet(
	UINT32 dskId
)
{
	switch (dskId)
	{
		case SP5K_DRIVE_NAND:
			if (appDiskStatus.isNandMounted==FALSE)
			{
				return FAIL;
			}
			break;
		case SP5K_DRIVE_SD:
			if (appDiskStatus.isCardMounted==FALSE)
			{
				return FAIL;
			}
			break;
		default:
			DBG_PRINT("%s: Unknown drive(%d) !!\n", __FUNCTION__, dskId);
			return FAIL;
	}

	appDiskStatus.diskInfo.actDsk = dskId;
	if (FAIL==_diskInit(dskId))
	{
		return FAIL;
	}

	return SUCCESS;
}

UINT32
appActiveDiskGet(
	void
)
{
	return (appDiskStatus.diskInfo.actDsk);
}

BOOL
appDiskMountQuery(
	UINT32 dskId
)
{
	if (dskId==SP5K_DRIVE_NAND)
	{
		return (!!appDiskStatus.isNandMounted);
	}
	else if (dskId==SP5K_DRIVE_SD)
	{
		return (!!appDiskStatus.isCardMounted);
	}
	else
	{
		return FALSE;
	}
}

UINT32
appDiskMountReady(
	void
)
{
	return (appDiskStatus.isMountReady);
}

UINT32
appDiskMountFailed(
	void
)
{
	return (appDiskStatus.isMountFailed);
}

appDiskInfo_t*
appCurDiskInfoGet(
	void
)
{
	_diskInfoGet();
	return (&appDiskStatus.diskInfo);
}

void
appDiskMount(
	UINT32 driver
)
{
	_diskMountStart(driver);
}

UINT32
appDiskFormat(
	UINT32 driver
)
{
	UINT32 ret;

	if(driver != appDiskStatus.diskInfo.actDsk) 
	{
		printf("[ERR] format disk is not active disk :%s(%d)\n",__FUNCTION__,__LINE__);
		return FAIL;
	}
	/*ret = sp5kDcfFsFileOperate(SP5K_DCF_FILEOP_FORMAT,(UINT32)DISK_NAME,0,NULL,0);*/

	ret = sp5kDiskFormat(driver,NULL,0);
	if (ret != SUCCESS) 
	{
		printf("[ERR] %s(%d)\n",__FUNCTION__,__LINE__);
		return ret;
	}

	ret = sp5kDiskMount(driver);
	if (ret != SUCCESS) 
	{
		printf("[ERR] %s(%d)\n",__FUNCTION__,__LINE__);
		return ret;
	}

	ret = sp5kDcfFsInit(driver,0,0);
	if (ret != SUCCESS) 
	{
		printf("[ERR] %s(%d)\n",__FUNCTION__,__LINE__);
		return ret;
	}

	ret = sp5kDcfFsActive(driver);
	if (ret != SUCCESS) 
	{
		printf("[ERR] %s(%d)\n",__FUNCTION__,__LINE__);
		return ret;
	}

	return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  File Name : appDiskClusterSizeGet
 *------------------------------------------------------------------------*/
UINT32
appDiskClusterSizeGet(
void
)
{
	UINT32 cluSz;
	cluSz = appDiskStatus.diskInfo.cluSz; /* in Bytes */
	return cluSz;
}

/*-------------------------------------------------------------------------
 *  File Name : appBytes2ClusterCalculate
 *------------------------------------------------------------------------*/
UINT32
appBytes2ClusterCalculate(
	UINT32 bytes
)
{
	UINT32 clusNum, cluSz;

	DBG_PRINT("%s: bytes(%d) \n", __FUNCTION__, bytes);

	cluSz = appDiskStatus.diskInfo.cluSz; /* in Bytes */

	clusNum = (bytes+(cluSz-1))/cluSz;

	DBG_PRINT("bytes(%d), clusterSize(%d) [Bytes] ==> clusters(%d) \n", bytes, cluSz, clusNum);

	return clusNum;
}

/*-------------------------------------------------------------------------
 *  File Name : appDiskFreeClusterGet
 *------------------------------------------------------------------------*/
UINT32
appDiskFreeClusterGet(
	UINT32 actDsk
)
{
	UINT32 remaining, cluSz;
	UINT32 totalSize=0;
	DBG_PRINT("%s: actDsk(%d) \n", __FUNCTION__, actDsk);

	/*-- disk remaining calculation */
	sp5kDiskInfoGet(actDsk, SP5K_FREE_SIZE_BY_KBYTE, &remaining);
	if (remaining < APP_DISK_DPOF_RSVD_SIZE_KB)
	{
		DBG_PRINT("%s: remaining(%d) less than DPOF_RSVD(%d) !!\n", __FUNCTION__, remaining, APP_DISK_DPOF_RSVD_SIZE_KB);
		return 0;
	}
	else
	{
		remaining -= APP_DISK_DPOF_RSVD_SIZE_KB;
	}
	/*-- transform to clusters */
	sp5kDiskInfoGet(actDsk, SP5K_CLUSTER_SIZE_BY_BYTES, &cluSz);
	
	sp5kDiskInfoGet(actDsk, SP5K_SIZE_BY_MEGA_BYTE, &totalSize);
	if(totalSize>0x000003ff)//large capacity >1GB
	{
		cluSz >>= 10; /* in KBytes */
		remaining = ((remaining+cluSz-1)/cluSz);
	}
	else//small capacity
	{
		remaining = (remaining <<10);/* in Bytes */
		remaining = ((remaining+cluSz-1)/cluSz);
	}

	DBG_PRINT("drive(%d), remaining clusters(%d) \n", actDsk, remaining);

	return remaining;
}

UINT32
appCardSpeedGet(
      UINT32 actDsk
){
	UINT32 clock;
	UINT32 speed;

	sp5kDiskInfoGet(actDsk, SP5K_DISK_CLOCK_QUERY, &clock); 
	sp5kDiskInfoGet(actDsk, SP5K_DISK_CLASS_SPEED_QUERY, &speed); 

	printf(" clock [%d] ---- speed [%d]\n", clock, speed);

	if ((clock == 5 && speed == 6)||(clock == 5 && speed == 4))
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

void appDiskStatusSet(UINT8 diskStatus)
{
	appDiskStatus.diskStatus = diskStatus;
}

UINT8 appDiskStatusGet(void)
{
	return appDiskStatus.diskStatus;
}


