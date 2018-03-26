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
#ifndef APP_DISK_H
#define APP_DISK_H

/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define APP_DISK_DPOF_RSVD_SIZE_KB		512

typedef enum 
{
	APP_DISK_STATUS_NO_CARD,
	APP_DISK_STATUS_MOUNTING,
	APP_DISK_STATUS_MOUNTED_OK,
	APP_DISK_STATUS_MOUNTED_FAIL,
	APP_DISK_STATUS_CARD_LOCKED,
	APP_DISK_STATUS_MEMORY_FULL,
}appDiskStatus_e;

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef struct appDiskInfo_s {
	UINT32 actDsk;	/* active disk : default 0 (not init yet) */
	UINT32 dskMbSz;	/* disk size : default 0 */
	UINT32 freeSz;	/* disk remain free size in k byte : default 0 */
	UINT32 cluSz;	/* disk cluster size : default 0*/
} appDiskInfo_t;

/**************************************************************************
 *          M O D U L E   V A R I A B L E S   R E F E R E N C E S         *
 **************************************************************************/

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/
UINT32 appActiveDiskSet(UINT32 dskId);
UINT32 appActiveDiskGet(void);
UINT32 appDiskMountReady(void);
UINT32 appDiskMountFailed(void);
BOOL appDiskMountQuery(UINT32 dskId);
appDiskInfo_t* appCurDiskInfoGet(void) ;
void appDiskMount(UINT32 driver);
UINT32 appDiskFormat(UINT32 driver);

UINT32 appDiskFreeClusterGet(UINT32 actDsk);
UINT32 appBytes2ClusterCalculate(UINT32 bytes);
UINT32 appDiskClusterSizeGet(void);
void appDiskStatusSet(UINT8 diskStatus);
UINT8 appDiskStatusGet(void);


/**************************************************************************
 *                               M A C R O S                              *
 **************************************************************************/

#define IS_DISK_MOUNT_READY    appDiskMountReady()
#define IS_DISK_MOUNT_FAIL    appDiskMountFailed()

#define IS_DISK_NAND_MOUNT	appDiskMountQuery(SP5K_DRIVE_NAND)
#define IS_DISK_SD_MOUNT	appDiskMountQuery(SP5K_DRIVE_SD)
#endif  /* APP_DISK_H*/

