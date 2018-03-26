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
#include "customization\dev_init.h"

#include "stdio.h"
#include "stdarg.h"
#include "string.h"
#include "app_com_def.h"
#include "app_com_api.h"

#include "sp5k_global_api.h"
#include "sp5k_sensor_api.h"

#include "app_aaa.h"
#include "app_calib_api.h"
#include "app_simple_menu.h"
#include "app_osd_api.h"
#include "app_tb_util.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern SINT32 cap_svidx_to_agcidx(SINT32 svidx);
extern UINT32 appCalibDefaultMshutterDelayGet(void);
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
/*-------------------------------------------------------------------------
 * Function : tbMsh_ShutterCloseStabilityTest
 * -- To verify Mhutter close stability.
 * -- Fixed exposure time, exposure gain and LV.
 * -- Verify if the rawLuma is stable or not
 *------------------------------------------------------------------------*/
void tbMsh_ShutterCloseStabilityTest(UINT32 loopCount,UINT32 iris, UINT32 expidx,UINT32 agcidx)
{	
	UINT32 i,shutterDelay = 0;
	UINT8 filename[16];
	UINT16 luma;

	appCalibData_t *pCalib;
	pCalib = appCalibDataGet();	
	
	
	/*-- Set View Config */
	tbUtil_ViewEnter();

	tbUtil_CalibCdspSet(1/*isObOn*/);
	/*-- OB Subtraction */
	tbUtil_ObApply();

	/*-- AE/AWB Off */
	tbUtil_AeAwb_Dark_Off();

	/*-- Set IRIS */
	tbUtil_IrisCtrl(!iris);

	CLEAR_OSD_SRC;	

/*s Modify by Aries 10/02/04*/
#if (_CAM_A900_EVM)||(_CAM_M900_EVM)||(_CAM_A900_D3)||(_CAM_CATHY_EVM)
	shutterDelay = pCalib->ShutterEarly[iris];
#else
	shutterDelay = appCalibDefaultMshutterDelayGet();	
	/* set defualt shutter delay */
	sp5kSnapShutterDelaySet(SP5K_SNAP_SHUTTER_CLOSE_EXP_END, 0, shutterDelay); /* close @ 1/64" */
#endif	
/*e Modify by Aries 10/02/04*/

	extern UINT32 xdata[]; 
	if(xdata[0]) {
		shutterDelay = xdata[0];
		xdata[0] = 0;
	}
	
	printf("@@default shutter delay : %d\n ",shutterDelay);
	

	sprintf(filename,"D:\\SH_TEST.TXT");
	printf("@@save filename : %s\n ",filename);

	/*-- Open Log File */	
	if (!tbLogFile_Open(filename)) return;

	/*-- Clear & reset tbBuf */
	tbLogBuf_Clear();
	
	/*-- LOOP 1 -------------------------------------------------------
	 * LV-17
	 */
	
	for (i = 0; i<loopCount; i++) {		
		sp5kSensorVvalidWait();
		sp5kStillCapExpAgcSet(expidx, agcidx);

		tbLogBuf_Add(i); /* log data */
		/*-- CAPTURE */
		luma = tbUtil_CapRawLumaGet();
		
		printf("%d--> luma(%d) \n", i, luma);

		tbLogBuf_Add(luma);
		
	}

	/*-- Save log -------------------------------------------------------
	 */
	tbLogBuf_PtrReset();

	tbLogFile_Print("Default Shutter Delay = %d\n",shutterDelay);
	tbLogFile_Print("Exp Index = %d\n",expidx);	
	tbLogFile_Print("Agc Index = %d\n",agcidx);	
	tbLogFile_Print("loop  RawLuma\n");


	for (i = 0; i<loopCount; i++) {		
		tbLogFile_Print("%3d %d\n", tbLogBuf_Get(), tbLogBuf_Get());
	}

	tbLogFile_Close();

	CLEAR_OSD_SRC;
	dbgOsd_Print(4, "File Saved ...");
	APP_SLEEP_MS(3000);

	return;
}
/*-------------------------------------------------------------------------
 * Function : tbMsh_ShutterCloseLagTest
 * -- To verify shutter delay stability and accuracy.
 * -- Fixed exposure gain and LV, vary exposure time .
 * -- Verify if the rawLuma increase while T changes
 *------------------------------------------------------------------------*/
void tbMsh_ShutterCloseLagTest(BOOL obOn,UINT32 iris)
{
	SINT32 cur;
	SINT32 start, end, delta;
	UINT32 idxMax, idxMin;
	UINT32 shutterDelay = 0;
	UINT32 agcIdx = cap_svidx_to_agcidx(80); //must larger than minima gain
	UINT8 filename[16];

	appCalibData_t *pCalib;
	pCalib = appCalibDataGet();	
	
	
	/*-- Set View Config */
	tbUtil_ViewEnter();

	if(obOn){
		tbUtil_CalibCdspSet(1/*isObOn*/);
		/*-- OB Subtraction */
		tbUtil_ObApply();
	}
	else{
		tbUtil_CalibCdspSet(0/*isObOn*/);
	}

	/*-- AE/AWB Off */
	tbUtil_AeAwb_Dark_Off();

	/*-- Set IRIS */
	tbUtil_IrisCtrl(!iris);

	CLEAR_OSD_SRC;	

/*s Modify by Aries 10/02/04*/
#if (_CAM_A900_EVM)||(_CAM_M900_EVM)||(_CAM_A900_D3)||(_CAM_CATHY_EVM)
	shutterDelay = pCalib->ShutterEarly[iris];
#else
	shutterDelay = appCalibDefaultMshutterDelayGet();	
	/* set defualt shutter delay */
	sp5kSnapShutterDelaySet(SP5K_SNAP_SHUTTER_CLOSE_EXP_END, 0, shutterDelay); /* close @ 1/64" */
#endif	
/*e Modify by Aries 10/02/04*/


	extern UINT32 xdata[]; 
	if(xdata[0]) {
		shutterDelay = xdata[0];
		xdata[0] = 0;
	}
	
	printf("@@default shutter delay : %d\n ",shutterDelay);
	

	sprintf(filename,"D:\\SH%04d_%1d.TXT",shutterDelay,iris);
	printf("@@save filename : %s\n ",filename);
	/*-- Open Log File */
	//if (!tbLogFile_Open("D:\\SHLAGTST.TXT")) return;
	if (!tbLogFile_Open(filename)) return;

	/*-- Clear & reset tbBuf */
	tbLogBuf_Clear();
	
	/*-- LOOP 1 -------------------------------------------------------
	 * LV-17
	 */
	tbUtil_SnapExpLimtGet(&idxMax, &idxMin);
	//start=EXP_1_128; end=EXP_1_32; delta=TV_UNIT/8;
	start=idxMax; end=idxMin; delta=TV_UNIT/4;
	for (cur = start; cur>end; cur-=delta) {
		UINT16 luma;
		dbgOsd_Print(4, "idxMin(%d) ...", end);
		dbgOsd_Print(6, "expidx(%d) ...", cur);

		//sp5kStillCapExpAgcSet(cur, cap_svidx_to_agcidx(agcIdx) );
		sp5kStillCapExpAgcSet(cur, agcIdx);

		tbLogBuf_Add(cur); /* log data */
		/*-- CAPTURE */
		#if 1
		luma = tbUtil_CapRawLumaGet();
		#else
		UINT32 loop,avgLuma=0;
		for (loop=0; loop<3; loop++) {
			luma = tbUtil_CapRawLumaGet();
			avgLuma+=luma;
			printf("    %d--> luma(%d) \n", cur, luma);
		}		
		luma = avgLuma/3;
		#endif
		printf("%d--> luma(%d) \n", cur, luma);

		tbLogBuf_Add(luma);
	}

	/*-- Save log -------------------------------------------------------
	 */
	tbLogBuf_PtrReset();

	tbLogFile_Print("Default Shutter Delay = %d\n",shutterDelay);
	tbLogFile_Print("Default Agc Index = %d\n",agcIdx);	
	tbLogFile_Print("expidx  RawLuma\n");


	for (cur = start; cur>end; cur-=delta) {
		UINT32 expidx, luma;
		expidx = tbLogBuf_Get();
		luma = tbLogBuf_Get();
		tbLogFile_Print("%3d %d\n", expidx, luma);
	}

	tbLogFile_Close();

	CLEAR_OSD_SRC;
	dbgOsd_Print(4, "File Saved ...");
	APP_SLEEP_MS(3000);

	return;
}




void cmdMSH(UINT32 argc, 	UINT8 *arg[], UINT32 v[])
{	
	if (argc==1) {
		printf("shutter usage:\n");
		printf(" msh shCloseTest [obOn] [iris]: shutter close lag testing (0: L iris; 1: S iris)\n");	
		printf(" msh shutterTest [iris] [loop]: shutter statbility testing (0: L iris; 1: S iris)\n");	
	} 
	else if (strcmp(arg[1], "shCloseTest")==0) {
		printf("shutter close lag testing , obOn : %d, Iris :%d\n",v[2],v[3]);
		tbMsh_ShutterCloseLagTest(v[2],v[3]);	
	}
	else if (strcmp(arg[1], "shutterTest")==0) {
		UINT32 loop,expidx,agcidx;
		printf("shutter close stability test( Iris:%d,loop:%d\n",v[2],v[3]);	
		loop = v[3];
		if(loop == 0) {
			loop = 1;
		}
		expidx = EXP_1_256;
		agcidx = cap_svidx_to_agcidx(80);
		tbMsh_ShutterCloseStabilityTest(loop,v[2],expidx,agcidx);		
	}
	else {
		//appAeEnable(v[1]);
	}
}


