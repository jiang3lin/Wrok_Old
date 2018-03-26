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
#include "os/_os_api.h"
#include "dev_init.h"
#include "app_init.h"


/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern UINT32 _bssEnd __attribute__ ((section (".bss")));

/* -- External function -- */
extern void devFrontEnum(void);
extern UINT32 snapMaxRawBufSizeGet(UINT32 mode);
extern void pbDramMaxAddrSet(UINT8 *);

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef struct memProfile_s {
	void *pdataLma;
	void *pdataVma;
	void *pheapStart;
	void *pheapEnd;
} memProfile_t;

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
extern memProfile_t memProfile;
#if !SPCA5310
static UINT32 maxSnapHeapSize;
#endif
/* For customer to create block pool */
#if CUSTOM_HEAP_BLOCK_SUPPORT
static const UINT32 customblkProf[][2] = {
	/*size small to large*/
	/*customers edit their own desired block size and entry*/
	{(128       ), 4}, /* 128 byte x 16 entries */
	{(512       ), 4}, /* 512 byte x 16 entries */
	{(2048      ), 4},  /* 2K  byte x 8 entries */
	{(5120      ), 2},  /* 5K  byte x 8 entries */
	{(16  * 1024), 1},  /* 16K byte x 4  entries */
	{(32  * 1024), 0},  /* 32K byte x 1  entries */
	{0,0}, /* end, must not remove */
};
#endif

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/**************************************************************************
 *                                                                        *
 *  Function Name: heapInit                                               *
 *                                                                        *
 *  Purposes: heap initialization function.                               *
 *                                                                        *
 *  Limitations:                                                          *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void
heapInit(
	void
)
{
	/* %DEBUG% */
	extern UINT32 profLogAdd(UINT32 id, UINT8 *msg);
	profLogAdd(0, "heapInit \n");

	/*
	 * front driver enum.
	 */
	#if !(POWERON_PB_WITHOUT_SENSOR) /*moved from dev_init.c*/
	#if COMPEL_POWEROFF_CHARGE
	if(!appGetBoot4ChargingFlag())
	#endif
	{
		profLogAdd(0, "devFrontEnum \n");
		devFrontEnum();
	}
	#endif

#if CUSTOM_HEAP_BLOCK_SUPPORT
	osCustomMemBlkInit(customblkProf);
#endif
#if CUSTOM_HEAP_BYTE_POOL_SIZE /*0: no separated byte pool custom heap*/
	osCustomMemByteInit(CUSTOM_HEAP_BYTE_POOL_SIZE);
#endif

#if !SPCA5310
	/* modify to reserved for snap and playback manual buffer */
	if (maxSnapHeapSize == 0) {
		maxSnapHeapSize = snapMaxRawBufSizeGet(SNAP_CAPTURE_MODE);
		pbDramMaxAddrSet( (UINT8 *)memProfile.pheapEnd );
	}
	memProfile.pheapEnd -= maxSnapHeapSize;
#endif

}
