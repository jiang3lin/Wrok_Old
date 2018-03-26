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
#include <stdio.h>
#include <string.h>
#include "api/sp5k_cdsp_api.h"
#include "app_dbg_api.h"
#include "app_aaa.h"
#include "app_awbalg_api.h"

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
static BOOL fAwbEnable=1;
static BOOL fAwbInfoEnable=0;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

void appAwbInit(
	void
)
{
#if SP5K_ALGORITHM_AWB
	appAWBALG_Init();
	sp5kAwbModeSet(SP5K_AWB_MODE_INFO_ONLY);	
	//appAWBALG_WBParamSet(0x01000004, 1);//for save raw
	//appAWBALG_WBParamSet(WB_PARAM_Func, WB_FUNC_Scanner);//enable scanner function
#else
	SP_AWBInit();
	sp5kAwbModeSet(SP5K_AWB_MODE_AUTO_SET);	
#endif
#if SP5K_SENSOR_AR0330CS
	appAWBALG_WBParamSet(WB_PARAM_PvBitNum, 12); 
#endif
}

 /**************************************************************************
 *                                                                        *
 *  Function Name: appAwbPreviewAdjust                                     *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT32  
appAwbPreviewAdjust(
	UINT32 param
)
{
	sp5kAwbResult_t awb;
	SINT32 ret;
	ret = sp5kAwbPreviewResultGet(param, &awb);
	if (ret != SUCCESS || !(awb.valid & AAA_INFO_RESULT_AWB_VALID)) {
		DBG_PRINT("awb result overrun\n");
		return FAIL;
	}
	if (fAwbInfoEnable) {
		UINT32 g = (awb.rgbvalue.grValue + awb.rgbvalue.gbValue ) /2;
		printf("rgb=[%d %d(%d,%d) %d]", awb.rgbvalue.rValue, g,
			awb.rgbvalue.grValue, awb.rgbvalue.gbValue, awb.rgbvalue.bValue);
		printf("ratio=[%d,%d]\n", awb.rgbvalue.rValue*1000/g, awb.rgbvalue.bValue*1000/g);
	}
	return SUCCESS;
}

void 
aaaAwbEnable(
	BOOL fEnable
)
{
	fAwbEnable = fEnable;
#if SP5K_ALGORITHM_AWB
	sp5kAwbModeSet(fEnable ? SP5K_AWB_MODE_INFO_ONLY: SP5K_AWB_MODE_OFF);
#else
	sp5kAwbModeSet(fEnable ? SP5K_AWB_MODE_AUTO_SET : SP5K_AWB_MODE_OFF);
#endif
}

void 
aaaAwbInfoEnable(
	BOOL fEnable
)
{
	extern UINT32 awb_debug_info;
	
	fAwbInfoEnable = fEnable;	
	awb_debug_info = fEnable;	
}

void 
aaaAwbGrayworldOnlyEnable(
	BOOL fEnable
)
{	
	extern UINT32 awb_grayworld_only;
	
	awb_grayworld_only = fEnable;	
}

UINT32 awbVersionGet(void);
void awbThresholdInit();
void awbPrintfInfo();
extern UINT8 dbgSaveInfo;

extern UINT8 awbPrintfEn,awbProc;
extern void appAWBALG_Cmd(UINT32 argc,UINT8 *arg[], UINT32 v[]);
void 
cmdAWB(
	UINT32 argc,
	UINT8 *arg[], 
	UINT32 v[]
)
{
#if SP5K_ALGORITHM_AWB
	appAWBALG_Cmd(argc,arg, v);
#else
printf("cmdAwb:%s\n%lx,%lx,%lx,%lx,%lx,%lx\n",arg[1],v[1],v[1],v[2],v[3],v[4],v[5]);
	if (argc==1) {
		printf("awb usage:\n"
			" awb 0|1 : disable/enable AWB\n"
			" awb info 0|1 : disable/enable AWB info\n"
		);
	} else if (strcmp(arg[1], "info")==0) {
		awbPrintfInfo();
		//aaaAwbInfoEnable(v[2]);
	} else if (strcmp(arg[1], "gray")==0) {
		aaaAwbGrayworldOnlyEnable(v[2]);
	} else if (strcmp(arg[1], "mode")==0) {
		printf("result:%x\n",(UINT16)sp5kAwbParamSet(SP5K_AWB_FLOW_MODE,v[2]));
	} else if (strcmp(arg[1], "shift")==0) {
		
		sp5kAwbParamSet(SP5K_AWB_CUSTOM_RGAIN_SHIFT,v[2]);
		sp5kAwbParamSet(SP5K_AWB_CUSTOM_BGAIN_SHIFT,v[3]);
		
		printf("custom shift:(%ld,%ld)\n",
			v[1],v[2]);
	} else if (strcmp(arg[1], "outdoor")==0) {
		UINT32 ret1=0,ret2=0,ret3=0;
		if(v[2])
			ret1=sp5kAwbParamSet(SP5K_AWB_AE_DARK_LUMA,v[2]);
		if(v[3])
			ret2=sp5kAwbParamSet(SP5K_AWB_AE_BRIGHT_AELV,v[3]);
		if(v[4])
			ret3=sp5kAwbParamSet(SP5K_AWB_AE_BRIGHT_CONTRAST,v[4]);
		printf("darkluma(%ld,%ld)\nbrightAeLv(%ld,%ld)\nbrightConstract(%ld,%ld)\n",
			ret1,v[2],ret2,v[3],ret3,v[4]);

	} else if (strcmp(arg[1], "gainset")==0) {
		sp5kWbGain_t wb;
		wb.rgain=v[1];
		wb.bgain=v[2];
		wb.grgain=1024;
		wb.gbgain=1024;
		sp5kImgWbGainSet(&wb);
	} else if (strcmp(arg[1], "version")==0) {
		printf("awb version:0x%lx\n",awbVersionGet());
	} else if (strcmp(arg[1], "dbg")==0) {
		
		UINT8 idx ;
		extern UINT8 dbgAwbCnt;
		for(idx = 0;idx<5;idx++)
		{
			printf("%u:%u\n",idx,v[idx]);
		}
		
		awbPrintfEn = v[2];
		dbgAwbCnt = v[3];
		printf("2-printf:%u,4-Cnt:%u\n",awbPrintfEn,dbgAwbCnt);
	} else if (strcmp(arg[1], "wnd")==0) {	
		dbgSaveInfo = 1;
		printf("saveInfo:%u\n",dbgSaveInfo);	
	} else if (strcmp(arg[1],"proc")==0){
		sp5kAwbParamSet(SP5K_AWB_ALGORITHM,v[2]);
	} else if (strcmp(arg[1],"snap")==0){

	} else {
		aaaAwbEnable(v[1]);
	}
#endif
}

