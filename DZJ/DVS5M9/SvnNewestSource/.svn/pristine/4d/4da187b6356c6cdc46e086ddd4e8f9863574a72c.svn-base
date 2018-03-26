/**************************************************************************
 *                                                                        *
 *         Copyright (c) 2004 by Sunplus Technology Co., Ltd.             *
 *                                                                        *
 *  This software is copyrighted by and is the property of Sunplus        *
 *  Technology Co., Ltd. All rights are reserved by Sunplus Technology    *
 *  Co., Ltd. This software may only be used in accordance with the       *
 *  corresponding license agreement. Any unauthorized use, duplication,   *
 *  distribution, or disclosure of this software is expressly forbidden.  *
 *                                                                        *
 *  This Copyright notice MUST not be removed or modified without prior   *
 *  written consent of Sunplus Technology Co., Ltd.                       *
 *                                                                        *
 *  Sunplus Technology Co., Ltd. reserves the right to modify this        *
 *  software without notice.                                              *
 *                                                                        *
 *  Sunplus Technology Co., Ltd.                                          *
 *  19, Innovation First Road, Science-Based Industrial Park,             *
 *  Hsin-Chu, Taiwan, R.O.C.                                              *
 *                                                                        *
 *  Author: Matt Wang                                                     *
 *                                                                        *
 **************************************************************************/
#ifndef _SP5K_DISK_API_H_
#define _SP5K_DISK_API_H_

#include "common.h"
#include "middleware/disk_def.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef UINT32 (*pfDir_t)(UINT32 ,UINT32, UINT32);
typedef UINT32 (*pfWr_t)(UINT32 ,UINT32, UINT32);
typedef UINT32 (*pfRd_t)(UINT32 ,UINT32, UINT32 *);
typedef UINT32 (*pfpwr_t)(void);

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
typedef enum {
	SP5K_DRIVE_RSVA    = 0,
	SP5K_DRIVE_NAND    = 1,
	SP5K_DRIVE_SD      = 2,
    SP5K_DRIVE_SDIO    = 3,
	SP5K_DRIVE_SMC     = 3,
	SP5K_DRIVE_XD      = 3,
	SP5K_DRIVE_CF      = 4,
	SP5K_DRIVE_MS      = 5,
	SP5K_DRIVE_RAM     = 6,
	SP5K_DRIVE_RSVB    = 7,
	SP5K_DRIVE_CDROM   = 0x0f,
	SP5K_START_DETECT  = 0xffff,
} sp5kDriveId_t;

typedef enum {
	SP5K_DISK_DETECT_PIN             = 0,
	SP5K_DISK_POWER_ON_PIN           = 1,
	SP5K_DISK_BUF_ENABLE_PIN         = 2,
	SP5K_DISK_WRITE_PROTECT_PIN      = 3,
	SP5K_DISK_SD18V_CTRL_PIN         = 4,
	SP5K_DISK_NAND_SPI_WP_CTRL_PIN   = 5,
	SP5K_DISK_PIN_MAX
} sp5kiskFunctionPin_t;

typedef enum {
	SP5K_GPIO_ACTIVE_LOW           = 0,
	SP5K_GPIO_ACTIVE_HIGH          = 1,
} sp5kDiskGpioPolarity_t;

typedef enum {
	SP5K_DISK_STATE                = 0,
	SP5K_SIZE_BY_MEGA_BYTE         = 1,
	SP5K_FREE_SIZE_BY_BYTES        = 2,
	SP5K_CLUSTER_SIZE_BY_BYTES     = 3,
	SP5K_DISK_WRITE_PROTECT_QUERRY = 4,
	SP5K_DISK_TYPE_QUERY           = 5,
	SP5K_DISK_CLOCK_QUERY          = 6,
	SP5K_DISK_BUS_QUERY            = 7,
	SP5K_FREE_SIZE_BY_KBYTE        = 8,
	/*s Add by Aries 09/12/05*/
	SP5K_DISK_CLASS_SPEED_QUERY    = 9,
	/*e Add by Aries 09/12/05*/
	SP5K_DISK_FS_TYPE_INFO         = 10,
	SP5K_SD_REG_WRITE_PROTECT_QUERY  = 11,
	SP5K_SD_CARD_IS_LOCKED_QUERY   = 12
} sp5kDiskInfo_t;

typedef enum {
	SP5K_DISK_IS_NOT_WRITE_PROTECT   = 0,
	SP5K_DISK_IS_UNDER_WRITE_PROTECT = 1,
} sp5kDiskProtectInfo_t;

typedef enum {
	SP5K_SD_TYPE                   = 1,
	SP5K_MMC_TYPE                  = 2,
} sp5kSdTypeCap_t;

typedef enum {
	SP5K_1_BIT_BUS                 = 1,
	SP5K_4_BIT_BUS                 = 2,
	SP5K_8_BIT_BUS                 = 3,
} sp5kSdBusCap_t;

typedef enum {
	SP5K_CLOCK_375KHz             = 1,
	SP5K_CLOCK_6MHz               = 2,
	SP5K_CLOCK_12MHz              = 3,
	SP5K_CLOCK_24MHz              = 4,
	SP5K_CLOCK_48MHz              = 5,
} sp5kSdClockCap_t;


/*define option and value*/

#define SP5K_FS_OPT_FORMAT_METHOD           0x0001
#define SP5K_FS_OPT_FORMAT_TYPE             0x0002
#define SP5K_FS_OPT_READ_TIME               0x0003
#define SP5K_FS_OPT_USE_MODIFY_TIME         0x0004
#define SP5K_FS_OPT_RTC_OFFSET_MINUS        0x0005
#define SP5K_FS_OPT_RTC_OFFSET_TICK         0x0006
#define SP5K_FS_OPT_IGNORE_MTIME            0x0007
#define SP5K_FS_OPT_COPY_USE_MTIME          0x0008
#define SP5K_FS_OPT_GETREMAINCLUS_ON_MOUNT  0x0009
#define SP5K_FS_OPT_FAT_BITMAP              0x0010
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
UINT32 sp5kDiskMount(UINT32);
UINT32 sp5kDiskMountDirect(UINT32);
UINT32 sp5kDiskDriverPinCfg(UINT32 ,UINT32 ,UINT32 ,UINT32 ,UINT32 );
UINT32 sp5kDiskRamAttrSet( UINT32 );
UINT32 sp5kDiskInfoGet(UINT32 ,UINT32 ,UINT32 *);
UINT32 sp5kDiskUnMount(UINT32);
UINT32 sp5kDiskNandBadBlkScan(UINT32, UINT32, UINT32 *, UINT32 *);
UINT32 sp5kDiskFormat(UINT32, UINT8 *, UINT32);
UINT32 sp5kDiskOptionSet(UINT32,UINT32);
#endif  /* _SP5K_DISK_API_H_ */

