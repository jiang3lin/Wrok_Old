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
#include <stdlib.h>

#include "common.h"
#include <stdio.h>
#include <string.h>

#include "sp5k_ae_api.h"
#include "sp5k_cal_api.h"

#include "app_com_api.h"
#include "app_dbg_api.h"
#include "app_util.h"
#include "app_lens_private.h"
#include "app_lens_api.h"
#include "app_lens_motor.h"
#include "app_aaa.h"

#include "app_lens_debug.h"
#include "app_strobe.h"
#include "app_btn_def.h"
#include "app_playback_def.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define READ8(_reg_)            (*((volatile UINT8  *)(_reg_)))
#define READ16(_reg_)           (*((volatile UINT16 *)(_reg_)))
#define READ32(_reg_)           (*((volatile UINT32 *)(_reg_)))
#define WRITE8(_reg_, _value_)  (*((volatile UINT8  *)(_reg_)) = (_value_))
#define WRITE16(_reg_, _value_) (*((volatile UINT16 *)(_reg_)) = (_value_))
#define WRITE32(_reg_, _value_) (*((volatile UINT32 *)(_reg_)) = (_value_))

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

#if  _PRIVATE_DEBUG_ENABLE_

static UINT8 szParamBuf[_MAX_DBG_PARAM_BUF_SIZE_] =
{
  0
};

static _privateDebugParam_t cmdParam;

UINT32 cmdLineParam = 0;
UINT8 cmdLinePauseEnable = 0;
UINT8 cmdLineResume = 1;

#endif /* _PRIVATE_DEBUG_ENABLE_ */

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
#if  _PRIVATE_DEBUG_ENABLE_

static void dbg_MyTestOnly(void);

#endif /* _PRIVATE_DEBUG_ENABLE_ */

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
void lensMotorGpioConfig(void);
void lensMotorPowerOn(void);
void lensMotorPowerOff(void);
void lensMotorStandby(void);
UINT32 lensMotorPowerCount(void);

UINT8 zoomMotorSpeedGet(void);
void zoomMotorSpeedSet(UINT8 zoom_speed);
void zoomMotorTele(void);
void zoomMotorWide(void);
void zoomMotorBrake(void);
void zoomMotorStandby(void);

void lensMShutOpen(void);
void lensMShutClose(void);
void lensIrisLargeSet(void);
void lensIrisSmallSet(void);
void lensIrisOpModeSet(BOOL isBlocking);
void lensIrisIdleActuate(void);

UINT16 tbUtil_PvRawLumaGet(void);
/**************************************************************************
 *                          L O C A L    D A T A                          *
 **************************************************************************/

#if (_PRIVATE_DEBUG_ENABLE_)
static void
dbg_Delay1ms(UINT32 ms)
{
  printf("\n[GTS_DELAY] %d ms \n\n", ms);
  sp5kTimeDelay(SP5K_TIME_DELAY_1MS, ms);
}

static void
cmdParam_SetP1(UINT32 val)
{
  cmdParam.p1 = val;
#if _debug_cmdLine_parameters_
  printf("--> cmdParam.p1 = %d \n", cmdParam.p1);
#endif
}

static void
cmdParam_SetP2(UINT32 val)
{
  cmdParam.p2 = val;
#if _debug_cmdLine_parameters_
  printf("--> cmdParam.p2 = %d \n", cmdParam.p2);
#endif
}

static void
cmdParam_SetP3(UINT32 val)
{
  cmdParam.p3 = val;
#if _debug_cmdLine_parameters_
  printf("--> cmdParam.p3 = %d \n", cmdParam.p3);
#endif
}

#if 0
static void
cmdParam_SetP4(UINT32 val)
{
  cmdParam.p4 = val;
#if _debug_cmdLine_parameters_
  printf("--> cmdParam.p4 = %d \n", cmdParam.p4);
#endif
}
#endif

static void
cmdParam_SetCommand(UINT8* pszCmd)
{
  cmdParam.pszCmd = pszCmd;
#if _debug_cmdLine_parameters_
  printf("--> cmdParam.pszCmd = %s \n", cmdParam.pszCmd);
#endif
}

static void
cmdParam_SetP1Cmd(UINT8* pszParam)
{
  cmdParam.pszP1 = pszParam;
#if _debug_cmdLine_parameters_
  printf("--> cmdParam.pszP1 = %s \n", cmdParam.pszP1);
#endif
}

static void
cmdParam_SetP2Cmd(UINT8* pszParam)
{
  cmdParam.pszP2 = pszParam;
#if _debug_cmdLine_parameters_
  printf("--> cmdParam.pszP2 = %s \n", cmdParam.pszP2);
#endif
}

static void
cmdParam_SetP3Cmd(UINT8* pszParam)
{
  cmdParam.pszP3 = pszParam;
#if _debug_cmdLine_parameters_
  printf("--> cmdParam.pszP3 = %s \n", cmdParam.pszP3);
#endif
}

#if 0
static void
cmdParam_SetP4Cmd(UINT8* pszParam)
{
  cmdParam.pszP4 = pszParam;
#if _debug_cmdLine_parameters_
  printf("--> cmdParam.pszP4 = %s \n", cmdParam.pszP4);
#endif
}
#endif

static void
dbg_GpioWrite(void)
{
//	UINT32 param = cmdParam.p1;
	UINT32 ioGroup = cmdParam.p1;
	UINT32 ioPin = cmdParam.p2;
	UINT32 ioVal = cmdParam.p3;

	if (strcmp(cmdParam.pszP1, "-?") == 0)
	{
		printf("... Set GPIO state \n>custom gioWR $grp$ $pin$ $val$ ...etc \n");
		return;
	}

	ioVal = (ioVal==0) ? 0 : ~0x00UL;
	sp5kGpioCfgSet(ioGroup, 0x01UL<<ioPin, 0x01UL<<ioPin);
	sp5kGpioWrite(ioGroup, 0x01UL<<ioPin, ioVal);

	printf(" ... Setting GPIO %d:%d=(%x) \n", ioGroup, ioPin, ioVal);
}


static void
debugPauseEnable(UINT8 isEnable)
{
  cmdLinePauseEnable = isEnable ? 1 : 0;
  printf("<cmdLine> Pause control : %s \n",
         (cmdLinePauseEnable ? "Enabled" : "Disabled"));
}

#if 0
static void
debugPause(UINT8* pszPrompt)
{
  if (cmdLinePauseEnable)
    {
      if (pszPrompt)
          printf("\n<cmdLine> %s ...\n", pszPrompt);

      printf("<cmdLine> Paused ...\n\n");
      cmdLineResume = 0;
      while (!cmdLineResume)
        {
          dbg_Delay1ms(100);
        }
    }
}
#endif

static void
debugResume(void)
{
	if (cmdLinePauseEnable)
	{
		cmdLineResume = 1;
	}
}

SINT32 test_expidx=80;
static void dbg_MyTestOnly(void)
{
	test_expidx = cmdParam.p1;
	printf("test_expidx=%d \n", test_expidx);
}
#endif /* (_PRIVATE_DEBUG_ENABLE_) */

/******************************************************************
 * The following function are open to other modules
 ******************************************************************/
void
cmdlinePauseEnable(UINT8 en)
{
#if _PRIVATE_DEBUG_ENABLE_

	if (en)
		debugPauseEnable(1);
	else
		debugPauseEnable(0);

#endif
}

void
cmdlineResume(void)
{
#if _PRIVATE_DEBUG_ENABLE_

	debugResume();
#endif
}

void
cmdParamInit(UINT8* paramIn)
{
#if _PRIVATE_DEBUG_ENABLE_
	UINT8* param, * arg1, * arg2, * arg3, * arg4;
	UINT32 i, len, v2, v3, v4;

#if _debug_cmdLine_parameters_
	printf("-- %s : paramIn = [%s] \n", __FUNCTION__, paramIn);
#endif

	if (paramIn[0] == ';')
	{
		printf("\n<Warning> Null argument for custom command ...\n\n");
		return;
	}

	/* Copy param strings */
	len = strlen(paramIn);

	if (len > _MAX_DBG_PARAM_BUF_SIZE_)
	{
		printf("\n<Warning> command string is too long ...\n\n");
		return;
	}

	memset(szParamBuf, 0, _MAX_DBG_PARAM_BUF_SIZE_);
	memcpy(szParamBuf, paramIn, len);
	param = (UINT8 *) szParamBuf;

	/* get argument 1 */
	arg1 = param;

	for (i = 0 ; i < len ; i++)
	{
		if (param[i] == ' ')
		{
			param[i] = 0;
			break;
		}
	}

	do
	{
		i++;
	}
	while (i < len && param[i] == ' ');

#if _debug_cmdLine_parameters_
	printf("-- arg1 = [%s] \n", arg1);
#endif


	/* get argument 2 */
	arg2 = &param[i];
	for (; i < len ; i++)
	{
		if (param[i] == ' ')
		{
			param[i] = 0;
			break;
		}
	}

	do
	{
		i++;
	}
	while (i < len && param[i] == ' ');

#if _debug_cmdLine_parameters_
	printf("-- arg2 = [%s] \n", arg2);
#endif

	/* get argument 3 */
	arg3 = &param[i];
	for (; i < len ; i++)
	{
		if (param[i] == ' ')
		{
			param[i] = 0;
			break;
		}
	}

	do
	{
		i++;
	}
	while (i < len && param[i] == ' ');

	#if _debug_cmdLine_parameters_
		printf("-- arg3 = [%s] \n", arg3);
	#endif

	/* get argument 4 */
	arg4 = &param[i];
	for (; i < len ; i++)
	{
		if (param[i] == ' ')
		{
			param[i] = 0;
			break;
		}
	}

	#if _debug_cmdLine_parameters_
	printf("-- arg4 = [%s] \n", arg4);
	#endif

	/* transfer arg2 and arg3 from string to value */
	v2 = strtoul(arg2, NULL, 0);
	v3 = strtoul(arg3, NULL, 0);
	v4 = strtoul(arg4, NULL, 0);

	cmdParam_SetCommand(arg1);
	cmdParam_SetP1(v2);
	cmdParam_SetP2(v3);
	cmdParam_SetP3(v4);

	cmdParam_SetP1Cmd(arg2);
	cmdParam_SetP2Cmd(arg3);
	cmdParam_SetP3Cmd(arg4);
#endif /* _PRIVATE_DEBUG_ENABLE_ */
}

void
cmdParamInitEx(UINT8 argc, UINT8 *arg[], UINT32 v[])
{
#if _PRIVATE_DEBUG_ENABLE_
	UINT8 * arg1, * arg2, * arg3, * arg4;
	UINT32 i, v2, v3, v4;

	#if 1 //_debug_cmdLine_parameters_
	for (i=0; i<argc; i++) {
		printf("[%d] arg[]=%s, v=%d \n", i, arg[i], v[i]);
	}
	#endif

	arg1 = arg[0];
	arg2 = arg[1]; v2=v[1];
	arg3 = arg[2]; v3=v[2];
	arg4 = arg[3]; v4=v[3];

	cmdParam_SetCommand(arg1);
	cmdParam_SetP1(v2);
	cmdParam_SetP2(v3);
	cmdParam_SetP3(v4);

	cmdParam_SetP1Cmd(arg2);
	cmdParam_SetP2Cmd(arg3);
	cmdParam_SetP3Cmd(arg4);
#endif /* _PRIVATE_DEBUG_ENABLE_ */
}

void
utility_cmd_dispatch(void)
{
#if _PRIVATE_DEBUG_ENABLE_
	UINT8* param;
	UINT32 p1, p2, p3;

	param = cmdParam.pszCmd;
	p1 = cmdParam.p1;
	p2 = cmdParam.p2;
	p3 = cmdParam.p3;

	if (strcmp(param, "pause") == 0)
    {
		cmdlinePauseEnable(cmdParam.p1);
    }
	else if (strcmp(param, "resume") == 0)
	{
		cmdlineResume();
	}
	else if (strcmp(param, "gioWR") == 0)
	{
		dbg_GpioWrite();
	}
	else if (strcmp(param, "mytest") == 0)
	{
		dbg_MyTestOnly();
	}
	else if (strcmp(param, "lens") == 0)
	{
		if (strcmp(cmdParam.pszP1, "boot") == 0) {
			appLensBoot();
		}
		else if (strcmp(cmdParam.pszP1, "close") == 0) {
			appLensClose();
		}
		else if (strcmp(cmdParam.pszP1, "info") == 0) {
			printf(">> Lens Info ...\n");
			printf("powerCnt : %d \n", lensMotorPowerCount());
			printf("focus pos: %d \n", appLensQuery(LENS_QUERY_FOCUS_STEP_POS));
		}
		else {
			printf("\n>>lens {boot|close} \n\n");
		}
	}
	else if (strcmp(param, "lon") == 0)
	{
		lensMotorGpioConfig();
		lensLedPowerOn(0xFFUL);
		lensMotorPowerOn();
	}
	else if (strcmp(param, "loff") == 0)
	{
		lensMotorPowerOff();
	}
	else if (strcmp(param, "lstdby") == 0)
	{
		lensMotorStandby();
	}
	else if (strcmp(param, "tele") == 0)
	{
		printf("Z ->>> %d \n", cmdParam.p1);
		lensMotorPowerOn();

		zoomMotorTele();
		dbg_Delay1ms(cmdParam.p1);

		zoomMotorStandby();
		lensMotorPowerOff();
	}
	else if (strcmp(param, "wide") == 0)
	{
		printf("Z <<<- %d \n", cmdParam.p1);
		lensMotorPowerOn();

		zoomMotorWide();
		dbg_Delay1ms(cmdParam.p1);

		zoomMotorStandby();
		lensMotorPowerOff();
	}
	else if (strcmp(param, "mshon") == 0)
	{
		lensMotorPowerOn();

		lensMShutOpen();
		//mshutOpenActuate();
		dbg_Delay1ms(100);

		lensMotorPowerOff();
	}
	else if (strcmp(param, "mshoff") == 0)
	{
		lensMotorPowerOn();

		appMShutClose();
		//mshutCloseActuate();
		dbg_Delay1ms(100);

		lensMotorPowerOff();
	}
	else if (strcmp(param, "mshidle") == 0)
	{
		mshutIdleActuate();
	}
	else if (strcmp(param, "apon") == 0)
	{
		//lensIrisOpModeSet(0);
		lensIrisLargeSet();
		//irisLargeActuate();

	}
	else if (strcmp(param, "apoff") == 0)
	{
		//lensIrisOpModeSet(0);
		lensIrisSmallSet();
		//irisSmallActuate();

	}
	else if (strcmp(param, "apidle") == 0)
	{
		lensIrisIdleActuate();
	}
	else if (strcmp(param, "fmacro") == 0)
	{
		int i;

		printf("\nF ->>> (macro) %d \n\n", cmdParam.p1);

		lensMotorPowerOn();
		focusInertiaExcite();

		for (i=0; i<cmdParam.p1; i++)
			focusMotorNearNext();

		focusReleaseHold();
		lensMotorStandby();
	}
	else if (strcmp(param, "finf") == 0)
	{
		int i;

		printf("\nF <<<- (inf) %d \n\n", cmdParam.p1);

		lensMotorPowerOn();
		focusInertiaExcite();

		for (i=0; i<cmdParam.p1; i++)
			focusMotorFarNext();

		focusReleaseHold();
		lensMotorStandby();
	}
	else if (strcmp(param, "focus") == 0)
	{
		printf("\nFocus GoPos %d \n\n", cmdParam.p1);

		appLensCreate();

		appFocusPosGo((SINT16)cmdParam.p1);
		appFocusStopWait();

		appLensDestroy();
	}
	else if (strcmp(param, "fscan") == 0)
	{
		int i;

		printf("\nFocus scan %d ~ %d, %d times\n\n",
				cmdParam.p1, cmdParam.p2, cmdParam.p3);

		appLensAFCreate();

		i = cmdParam.p3;
		do {
			int delta=2, cur=cmdParam.p1;
			do {
				appFocusPosGo((SINT16)cur);
				appFocusStopWait();
				dbg_Delay1ms(500);
				cur += delta;
			} while(cur<cmdParam.p2);
			i--;
		} while(i>0);

		appLensAFDestroy();
	}
	else if (strcmp(param, "frst") == 0)
	{
		lensMotorPowerOn();

		focusMotorReset();
		focusInertiaExcite();

		lensMotorStandby();
	}
	else if (strcmp(param, "ledon") == 0)
	{
		//lensLedPowerOn(~0);
		sp5kGpioWrite(SP5K_GPIO_GRP_LMI, 1<<11, 1<<11);
		sp5kGpioWrite(SP5K_GPIO_GRP_TGL, 0x01, 0x01);
		printf("\nLend LED on !!\n\n");
	}
	else if (strcmp(param, "zhome") == 0)
	{
		//printf("\nZOOM Home (%d) \n\n", lensLedRead(1<<2));
		UINT32 ioval;
		sp5kGpioRead(SP5K_GPIO_GRP_TGL, 1<<2, &ioval);
		printf("\nZOOM Home (%d) \n\n", ioval);
	}
	else if (strcmp(param, "fhome") == 0)
	{
		//printf("\nFOCUS Home (%d) \n\n", lensLedRead(1<<2));
		UINT32 ioval;
		sp5kGpioRead(SP5K_GPIO_GRP_TGL, 1<<3, &ioval);
		printf("\nFOCUS Home (%d) \n\n", ioval);
	}
	else if (strcmp(param, "disknand") == 0)
	{
		printf("Set Disk NAND \n");
		appPbDiskMount(PB_DISK_NAND);
		dbg_Delay1ms(3000);

		appPbActiveDiskSet(PB_DISK_NAND);
		printf("Disk NAND OK !!\n");
	}
	else if (strcmp(param, "disksd") == 0)
	{
		printf("Set Disk SD \n");
		appPbDiskMount(PB_DISK_SD);
		dbg_Delay1ms(5000);

		appPbActiveDiskSet(PB_DISK_SD);
		printf("Disk SD OK !!\n");
	}
	else if (strcmp(param, "getae")==0) {
		SINT32 expidx;
		UINT32 agcidx;
		UINT16 luma;
		printf("getae ...\n");

		sp5kPreviewExpAgcGet(&expidx, &agcidx);
		luma = tbUtil_PvRawLumaGet();

		printf("View: expidx(%d) agcidx(%d), RawLuma(%d) ...\n", expidx, agcidx, luma);
	}
#endif /* _PRIVATE_DEBUG_ENABLE_ */
}


