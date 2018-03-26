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
#include "app_view_param.h"
#include "app_aaa.h"
#include "app_script.h"
#include "app_calib_api.h"
#include "app_tvlcd.h"
#include "app_osd_api.h"
#include "gpio_custom.h"
#include "api/sp5k_sensor_api.h"
#include "app_tgcmd.h"
#include "app_still.h"
#include "app_sys_def.h"
#if MTBF_DBG_EN
#include "app_mtbf_cmd.h"
#endif
#include "app_video.h"

#include "app_osd_api_lcm.h"

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

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
void cmdScript( UINT32 argc, UINT8 *arg[], UINT32 v[] );
void aaaLabDo(UINT32 fun, UINT32 subfunc, UINT32 sensormode_pv, UINT32 sensormode_snap);
extern void cmdSnap(UINT32 argc, UINT8 *arg[], UINT32 v[]);
extern void cmdVideo(UINT32 argc, UINT8 *arg[], UINT32 v[]);
extern void cmdAE(UINT32 argc, UINT8 *arg[], UINT32 v[]);
extern void cmdAF(UINT32 argc, UINT8 *arg[], UINT32 v[]);
extern void cmdFlash(UINT32 argc, UINT8 *arg[], UINT32 v[]);
extern void cmdFocus(UINT32 argc, UINT8 *arg[], UINT32 v[]);
extern void cmdSENSOR(UINT32 argc, 	UINT8 *arg[], UINT32 v[]);
extern void cmdMSH(UINT32 argc, 	UINT8 *arg[], UINT32 v[]);
extern void cmdRf(UINT32 argc, UINT8 *arg[], UINT32 v[]);
extern void appPanelInit(void);
extern void appBtnInit(void);

extern void appStillSnapIqSet(void);
extern void _IqEdgeSet(UINT8 filterOption,UINT32 ind,UINT16 EgGain);
extern void _IqLutCall(int lutid, const char *fname);
void SampleCode_CaptureDateStamp(void);

extern void  cmdSdvVideoView(UINT32 argc, UINT8 *arg[], UINT32 v[]);

extern void appGsensorXYZRead(	SINT16 *x_data, SINT16 *y_data, SINT16 *z_data);
extern void gSensorDriverSerRead(UINT32  addr, UINT32 *data);
extern void gSensorDriverWrite(UINT32 addr, UINT32 data);

UINT8 tgCmdMode=SP5K_MODE_STILL_PREVIEW;

UINT32 xdata[10];
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/**************************************************************************
 *                          L O C A L    D A T A                          *
 **************************************************************************/

/**************************************************************************
 *                                                                        *
 *  Function Name: appCustomerCmd                                            *
 *                                                                        *
 *  Purposes: Entry function of customer defined command "custom"         *
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
void
appCustomerCmd(
	UINT8 argc,
	UINT8 *arg[],
	UINT32 v[]
)
{
	UINT8 *param=arg[0];
	static UINT8 paraInited=0;
#if MTBF_DBG_EN	
	extern UINT32 mtbf_mode;
	extern mtbf_cmd_name_t mtbfCmdName[];
#endif

	if (param[0] == '.') {
		int i;
		for (i=0 ; i<argc-1 ; i++) {
			*(arg[i]+strlen(arg[i])) = ' ';
		}
		appScriptQuickRun(param+1);
		return;
	}

#if MTBF_DBG_EN
	if(strcmp(param,"mtbf")==0){
		UINT32 ret = 0;
		
		if(arg[1]){
			if((strcmp(arg[1],"on")==0) && !mtbf_mode){
				mtbf_mode = 1;
				appMTBFTestInit(1);
				return;
			}else if((strcmp(arg[1],"off")==0) && mtbf_mode){
				mtbf_mode = 0;
				appMTBFTestInit(0);
			}else if(strcmp(arg[1],"vid") == 0){
				if(arg[2] == NULL){
					appMTBFCmdHelp(MTBF_VIDEO_SIZE);
					return;
				}
				
				ret = appMTBFCmdParaParse(MTBF_VIDEO_SIZE,arg[2],v[3]);
				mtbfCmdName[MTBF_VIDEO_SIZE].pMtbfFunc(ret);
			}else if(strcmp(arg[1],"rec") == 0){
				if(arg[2] == NULL){
					appMTBFCmdHelp(MTBF_VIDEO_REC);
				}
				mtbfCmdName[MTBF_VIDEO_REC].pMtbfFunc(v[2]);
			}else if(strcmp(arg[1],"img") == 0){
				if(arg[2] == NULL){
					appMTBFCmdHelp(MTBF_STILL_SIZE);
					return;
				}
				ret = appMTBFCmdParaParse(MTBF_STILL_SIZE,arg[2],0);
				mtbfCmdName[MTBF_STILL_SIZE].pMtbfFunc(ret);
			}else if(strcmp(arg[1],"snap") == 0){
				if(arg[2] == NULL){
					appMTBFCmdHelp(MTBF_STILL_SNAP);
				}
				mtbfCmdName[MTBF_STILL_SNAP].pMtbfFunc(v[2]);
			}else if(strcmp(arg[1],"pb") == 0){
				mtbfCmdName[MTBF_PB_MAIN].pMtbfFunc(v[2]);
			}else if(strcmp(arg[1],"dele") == 0){
				if(arg[2]!=NULL){
					ret = appMTBFCmdParaParse(MTBF_FILE_DEL,arg[2],0);
				}else{
					ret = 0;
				}
				mtbfCmdName[MTBF_FILE_DEL].pMtbfFunc(ret);
			}
		}else{
			printf("MTBF Test usage:\n");
			printf("single run:\nmtbf cmd param\n");
			printf("Ex:\n");
			printf("   mtbf rec 10\n");
			printf("\nloop run:\n");
			printf("First input 'mtbf on' to open loop mode.\n");
			printf("That you can input cmd continuously!\n");
			printf("And input 'start' to start test.\n");
			printf("Ex:\n");
			printf("   mtbf on\n");
			printf("   cmd1 param\n");
			printf("   cmd2 param\n");
			printf("   cmdn param\n");
			printf("   start\n");
			printf("If you want to stop test,you can input 'mtbf off'.\n");
		}
	}
	
	if(mtbf_mode){
		putchar('\b');
		if(arg[1] != NULL){
			appMTBFCmdPut(param,arg[1],v[1]);
		}else{
			appMTBFCmdPut(param,NULL,v[1]);
		}
		return;
	}
#endif

#if SPCA6330
	if (strcmp(param, "lyt")==0) {
		extern void appLytTest(UINT8 *);
		if(argc > 1){
			appLytTest(arg[1]);
		}
		else{
			appLytTest(NULL);
		}
	}
	else
#endif

	
	/* custom command */
	if (strcmp(param, "test")==0) {
		;
	}
	else if (strcmp(param, "prkon")==0) {
		appVideo_SetParkingMode(1);
	}
	else if (strcmp(param, "prkof")==0) {
		appVideo_SetParkingMode(0);
	}
	/* param manager:
		param op: get, set, get info
		mode op: first power on, power on/off, switch mode */
	else if (strcmp(param, "p")==0) {
		if (strcmp(arg[1], "on")==0) {
			if (paraInited) return;
			UiParamViewInit();
			UiParamViewChangeMode(v[2]);
			paraInited = 1;
		}
		if (paraInited==0) return;
		if (strcmp(arg[1], "off")==0) {
			UiParamViewFini();
			paraInited = 0;
		} else if (strcmp(arg[1], "flush")==0) {
			UiParamViewFlush();
		} else if (strcmp(arg[1], "reset")==0) {
			UiParamViewReset();
		} else if (strcmp(arg[1], "mode")==0) {
			UiParamViewChangeMode(v[2]);
		} else if (strcmp(arg[1], "d")==0) {
			param_info_t info;
			UINT32 i;
			for (i=0 ; i<VIEW_PRM_COUNT ; i++) {
				UiParamViewGetInfo(i, &info); /* you can use VIEW_PRM_ID(af) instead of i */
				printf("%2d: %d(%d) mask=%04x flag=%s(%2x)\n", i, ((UINT8*)pViewParam)[i], info.defVal, info.availMask,
					info.flag & PRM_SAVE ? "Save" : info.flag & PRM_NOUSE ? "NA" : info.flag & PRM_NOSEL ? "Fixed" : "None", info.flag);
			}
		} else if (strcmp(arg[1], "s")==0) {
			((UINT8 *)pViewParam)[v[2]] = v[3];
		}
	}
	/* TG cmd */
	#if 0
	else if (strcmp(param, "tgset")==0) {
			UINT32 id;
			UINT8 regdata[4];
			tgCmdMake(v[1], v[2], regdata);
			printf("set %s tg: 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n", tgCmdModeStrGet(tgCmdMode),
				regdata[0], regdata[1], regdata[2], regdata[3], regdata[4] );
			if (tgCmdMode==SP5K_MODE_STILL_PREVIEW || tgCmdMode==SP5K_MODE_VIDEO_PREVIEW) {
				id = sensorCmdportIdGet(0);
				sensorSerialAsyncWrite(id, 1, regdata);
			}
			tgCmdUpdate(tgCmdMode, regdata);
		}
		else if (strcmp(param, "tgdump")==0) {
			tgCmdDump(tgCmdMode);
		} else if (strcmp(param, "tgclr")==0) {
			tgCmdClear(tgCmdMode);
			tgCmdDump(tgCmdMode);
		} else if (strcmp(arg[1], "s")==0) {
			((UINT8 *)pViewParam)[v[2]] = v[3];
		}
	#endif
	else if (strcmp(param, "tgset")==0) {
		UINT32 id;
		UINT8 regdata[4];
		tgCmdMake(v[1], v[2], regdata);
		printf("set %s tg: 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n", tgCmdModeStrGet(tgCmdMode),
			regdata[0], regdata[1], regdata[2], regdata[3], regdata[4] );
		if (tgCmdMode==SP5K_MODE_STILL_PREVIEW || tgCmdMode==SP5K_MODE_VIDEO_PREVIEW) {
			extern UINT32 sensorCmdportIdGet(UINT32 port);
			extern UINT32 sensorSerialAsyncWrite(UINT32 cmdportid,UINT32 nreg,const UINT8 *pdata);

			id = sensorCmdportIdGet(0);
			sensorSerialAsyncWrite(id, 1, regdata);
			}
			tgCmdUpdate(tgCmdMode, regdata);
		}
		else if (strcmp(param, "tgdump")==0) {
			tgCmdDump(tgCmdMode);
		} else if (strcmp(param, "tgclr")==0) {
			tgCmdClear(tgCmdMode);
			tgCmdDump(tgCmdMode);
		} else if (strcmp(param, "tgsave")==0) {
			((UINT8 *)pViewParam)[v[2]] = v[3];
			printf("tgcmd saved\n");
		} else if (strcmp(param, "tgmode")==0) {
			tgCmdMode = v[1]==0 ? SP5K_MODE_STILL_PREVIEW :
			v[2]==1 ? SP5K_MODE_VIDEO_PREVIEW : SP5K_MODE_STILL_SNAP;
			printf("set tgcmd mode to %s\n", tgCmdModeStrGet(tgCmdMode));
	} else if(strcmp(param, "focus")==0){
		cmdFocus(argc,arg,v);
	} else if(strcmp(param, "sensor")==0){
		cmdSENSOR(argc,arg,v);
	} else if(strcmp(param, "msh")==0){
		cmdMSH(argc,arg,v);
	} else if(strcmp(param, "ae")==0){
		cmdAE(argc,arg,v);
	} else if(strcmp(param, "af")==0){
		cmdAF(argc,arg,v);
	} else if(strcmp(param, "flash")==0){
		cmdFlash(argc,arg,v);
	}else if(strcmp(param, "awb")==0){
		cmdAWB(argc,arg,v);
	}else if(strcmp(param, "calib")==0){
		cmdCalib(argc,arg,v);
	} else if (strcmp(param, "script")==0) {
		cmdScript(argc, arg, v);
	}else if (strcmp(param, "snap")==0) {
		cmdSnap(argc, arg, v);
	}else if (strcmp(param, "video")==0) {
		cmdVideo(argc, arg, v);
	}else if (strcmp(param, "Rf")==0) {
		cmdRf(argc, arg, v);
	} else if (strcmp(param, "aaalab")==0) {
		aaaLabDo(v[1], v[2], SP5K_SENSOR_MODE_PREVIEW, SP5K_SENSOR_MODE_SNAP);
	} else if(strcmp(param, "SdvVideoView")==0) {
		cmdSdvVideoView(argc, arg, v);
	} else if (strcmp(param, "dev")==0) {
		if (strcmp(arg[1], "gpioInit")==0) {
			printf("gpio Init...\n");
			gpioInitialCfgSet();
		}
		else if (strcmp(arg[1], "lcdInit")==0) {
			printf("panel Init...\n");
			appPanelInit();
		}
		else if (strcmp(arg[1], "blOn")==0) {
			printf("BackLight On...\n");
			LCD_BACKLIGHT_ON;
		}
		else if (strcmp(arg[1], "lcmrefresh")==0) {
			printf("lcm refresh...\n");
			extern void dispRefresh(void);
			dispRefresh();
		}
		else if (strcmp(arg[1], "lcmwcmd")==0) {
			printf("lcm write cmd...\n");
			extern void dispWriteCmd(UINT8 cmd);
			dispWriteCmd(v[2]);
		}
		else if (strcmp(arg[1], "lcmtest")==0) {
			printf("lcm test...\n");
			extern void dispTest(UINT8 Mode);
			dispTest(v[2]);
		}
		else if (strcmp(arg[1], "blOff")==0) {
			printf("BackLight Off...\n");
			LCD_BACKLIGHT_OFF;
		}
		else if (strcmp(arg[1], "btnInit")==0) {
			printf("button Init...\n");
			appBtnInit();
		}
		else if (strcmp(arg[1], "ledPowerFlicker")==0) {
			printf("ledPowerFlicker...\n");
			POWER_LED_FLICKER(500);
		}
		else if (strcmp(arg[1], "ledBusyFrontFlicker")==0) {
			printf("ledBusyFrontFlicker...\n");
			BUSY_FRONT_LED_FLICKER(500);
		}
		else if (strcmp(arg[1], "ledChargeFlicker")==0) {
			printf("ledChargeFlicker...\n");
			CHARGE_LED_FLICKER(500);
		}
		else if (strcmp(arg[1], "ledBusyRearFlicker")==0) {
			printf("ledBusyRearFlicker...\n");
			BUSY_REAR_LED_FLICKER(500);
		}
		else if (strcmp(arg[1], "gsensorxyz")==0) {
			printf("gsensorxyz...\n");
			SINT16 tmp16_x, tmp16_y, tmp16_z;
			appGsensorXYZRead(&tmp16_x, &tmp16_y, &tmp16_z);
			printf("X:%d, Y:%d, Z:%d", tmp16_x, tmp16_y, tmp16_z);

			float x, y, z;
			x = tmp16_x * 4.0 / 4096;
			y = tmp16_y * 4.0 / 4096;
			z = tmp16_z * 4.0 / 4096;
			printf("	X:%fg, Y:%fg, Z:%fg\n", x, y, z);
		}	
		else if (strcmp(arg[1], "gsensorr")==0) {
			printf("gsensorread...\n");
			UINT32 addr = v[2], value;
			gSensorDriverSerRead(addr, &value);
			printf("gsensor 0x%x = 0x%x\n", addr, value);
		}	
		else if (strcmp(arg[1], "gsensorw")==0) {
			printf("gsensorw...\n");
			UINT32 addr = v[2], value = v[3];
			gSensorDriverWrite(addr, value);
			printf("gsensow 0x%x = 0x%x\n", addr, value);
			gSensorDriverSerRead(addr, &value);
			printf("gsensor 0x%x = 0x%x\n", addr, value);
		}	
	}
	else if(strcmp(param, "lcmosd")==0){
		if (strcmp(arg[1], "test")==0) {
			appLcmOsdLargeIconShow(v[2], v[3], v[4]);
		}
		else if (strcmp(arg[1], "mainicon")==0) {
			appLcmOsdTest(v[2]);
		}
	}		
	else if (strcmp(param, "iq")==0) {
		if (strcmp(arg[1], "iqinit")==0) {
			appStillSnapIqSet();
			printf("appStillSnapIqSet : done!\n");
		}
		else if (strcmp(arg[1], "aalut")==0) {
			if (argc>=3) {
				_IqLutCall(0x1E,arg[2]); //RES_ID_ANTI_ALIAS
				printf("AA lut load (%s) : done!\n",arg[2]);
			}
			else {
				printf("iq aalut [filename]\n");
			}
		}
		else if (strcmp(arg[1], "ahslut")==0) {
			if (argc>=3) {
				_IqLutCall(0x1D,arg[2]); //RES_ID_AHS
				printf("AHS lut load (%s) : done!\n",arg[2]);
			}
			else {
				printf("iq ahslut [filename]\n");
			}
		}
		else if (strcmp(arg[1], "ylut")==0) {
			if (argc>=3) {
				_IqLutCall(0x05,arg[2]); //RES_ID_EDGE
				printf("edge Y lut load (%s) : done!\n",arg[2]);
			}
			else {
				printf("iq ylut [filename]\n");
			}
		}
		else if (strcmp(arg[1], "elut")==0) {
			if (argc>=3) {
				_IqLutCall(0x14,arg[2]); //RES_ID_EDGE_E_LUT
				printf("edge Gain lut load (%s) : done!\n",arg[2]);
			}
			else {
				printf("iq elut [filename]\n");
			}
		}
#if 0
		else if (strcmp(arg[1], "egain")==0) {
			if (argc>=5) {
				_IqEdgeSet(v[2],v[3],v[4]);
				printf("edge Gain (filer:%d,Iso:%d,eegain:0x%x) : done!\n",v[2],v[3],v[4]);
			}
			else {
				printf("iq egain[filter:0~2] [Iso:0~n][eGain : 0x00~0x20]\n");
			}
		}
#endif
		else if (strcmp(arg[1], "snap")==0) {
			tgCmdExec(SP5K_MODE_STILL_SNAP);
			appStillUtil_CaptureSet();
		}
	}
	else if (strcmp(param, "xdata")==0 || strcmp(param, "x")==0) {
		UINT32 i;
		if (argc==1) {
			for (i=0 ; i<COUNT_OF(xdata) ; i++)
				printf("x[%d]=%d ", i, xdata[i]);
			printf("\n");
		} else {
			if (argc>COUNT_OF(xdata)) argc = COUNT_OF(xdata);
			for (i=1 ; i<argc ; i++)
				xdata[i-1] = v[i];
		}
	}
	else if (strcmp(param, "sp5kmodeget")==0){
		UINT32 mode;
		sp5kModeGet(&mode);
		printf("sp5kmode=0x%x\n", mode);
	}
	else if (strcmp(param, "sp5kmodeset")==0){
		UINT32 mode;
		sp5kModeSet(v[1]);
		sp5kModeGet(&mode);
		printf("sp5kmode=0x%x\n", mode);
	}
	else if (strcmp(param, "s")==0)
	{
	    SampleCode_CaptureDateStamp();
	}
	else if (strcmp(param, "mt")==0)
	{
	    appDiskMount(v[1]);
	}
	else{
		#if 1
		extern void cmdParamInitEx(UINT8 argc, UINT8 *arg[], UINT32 v[]);
		extern void utility_cmd_dispatch(void);

		cmdParamInitEx(argc, arg, v);
		utility_cmd_dispatch();
		#endif
	}
}

