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
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "common.h"
#include "api/cmd_api.h"
#include "api/sp5k_cdsp_api.h"
#include "api/sp5k_disp_api.h"
#include "api/sp5k_capture_api.h"
#include "api/sp5k_fs_api.h"
#include "api/sp5k_os_api.h"
#include "api/Sp5k_cal_api.h"
#include "api/Sp5k_modesw_api.h"
#include "api/sp5k_ae_api.h"
#include "api/sp5k_awb_api.h"
#include "api/sp5k_gfx_api.h"
#include "api/sp5k_dcf_api.h"
#include "api/sp5k_media_api.h"
#include "api/sp5k_utility_api.h"
#include "api/sp5k_global_api.h"
#include "api/sp5k_sensor_api.h"
#include "api/sp5k_usb_api.h"
#include "api/sp5k_dzoom_api.h"
#include "api/sp5k_disk_api.h"
#include "hostfw/include/app_osdfont.h"
#include "hostfw/include/app_res_def.h"
#include "hostfw/include/app_osd_api.h"
#include "hostfw/include/app_state_ctrl.h"
#include "debug_mod/dbg_api.h"
#include "hostfw/include/app_msg.h"
#include "hostfw/include/app_cdfs_api.h"
#include "hostfw/include/app_view_param.h"
#include "hostfw/include/app_icon.h"
#include "hostfw/include/app_user_setting.h"
#include "hostfw/include/app_disk.h"
#include "hostfw/include/app_ui_para.h"
#include "hostfw/include/app_still_timelapse.h"

#if ICAT_WIFI
#include "hostfw/include/app_wifi_utility.h"
#endif
#if ICAT_FCWS || ICAT_LDWS
#include "hostfw/include/app_cvrws_api.h"
#endif

#if SPCA6330
#include "hostfw/include/app_ptp.h"
#endif
/*s Add by Aries 10/05/18*/
#if SPCA5310
#include "api/sp5k_pip_api.h"
#endif
/*e Add by Aries 10/05/18*/
#include "middleware/proflog.h"

#include "api/sp5k_rsvblk_api.h"
#include "customization/app_init.h"
#include "hostfw/include/App_key_def.h"
#include "hostfw/include/App_state_ctrl.h"
#include "customization/dev_init.h"
#include "customization/app_still_iq.h"
/*s Add by Aries 10/05/18*/
#if SPCA5310
#if SP5K_HDMI_ANA_ANX7150
#include "drivers/disp/ANX7150_Sys7150.h"
#endif
/*#include "include/app_state_ctrl.h" */
#if SP5K_HDMI_EP_EP932M
#include "drivers/disp/EP932_SysController.h"
#include "drivers/disp/EP932_SystemConfig.h"
#endif

#include "hostfw/include/gpio_custom.h"

#endif

/*e Add by Aries 10/05/18*/
#define AV_UNIT         1
#define EV_UNIT         16
#define SV_UNIT         EV_UNIT
#define TV_UNIT         EV_UNIT
#define SV_UNIT         EV_UNIT

/*****************************************************************
 * STD ISO/EXP, prefix with STD_ISO_ & STD_EXP_
 *****************************************************************/
#define STD_ISO_25              (STD_ISO_50 - SV_UNIT)
#define STD_ISO_50              (STD_ISO_100 - SV_UNIT)
#define STD_ISO_100             (5*SV_UNIT)
#define STD_ISO_200             (STD_ISO_100 + SV_UNIT)
#define STD_ISO_400             (STD_ISO_200 + SV_UNIT)
/*s Add by Aries 09/08/11*/
#define STD_ISO_640             (STD_ISO_400 + (SV_UNIT/2))
/*e Add by Aries 09/08/11*/
#define STD_ISO_800             (STD_ISO_400 + SV_UNIT)
#define STD_ISO_1600    (STD_ISO_800 + SV_UNIT)

#define STD_EXP_16_01   (STD_EXP_8_01 - TV_UNIT)
#define STD_EXP_8_01    (STD_EXP_4_01 - TV_UNIT)
#define STD_EXP_4_01    (STD_EXP_2_01 - TV_UNIT)
#define STD_EXP_2_01    (STD_EXP_1_01 - TV_UNIT)

#define STD_EXP_1_01    (STD_EXP_1_02 - TV_UNIT)
#define STD_EXP_1_02    (STD_EXP_1_04 - TV_UNIT)

#define STD_EXP_1_04    (STD_EXP_1_08 - TV_UNIT)
#define STD_EXP_1_08    (STD_EXP_1_16 - TV_UNIT)
#define STD_EXP_1_16    (STD_EXP_1_32 - TV_UNIT)
#define STD_EXP_1_24    (STD_EXP_1_32 - (TV_UNIT / 2))
#define STD_EXP_1_32    (5*TV_UNIT)
#define STD_EXP_1_64    (STD_EXP_1_32 + TV_UNIT)
#define STD_EXP_1_128   (STD_EXP_1_64 + TV_UNIT)
#define STD_EXP_1_256   (STD_EXP_1_128 + TV_UNIT)
#define STD_EXP_1_512   (STD_EXP_1_256 + TV_UNIT)
#define STD_EXP_1_1024  (STD_EXP_1_512 + TV_UNIT)
#define STD_EXP_1_2048  (STD_EXP_1_1024 + TV_UNIT)
#define STD_EXP_1_4096  (STD_EXP_1_2048 + TV_UNIT)
#define STD_EXP_1_8192  (STD_EXP_1_4096 + TV_UNIT)

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
enum {
	DRAM_TEST_PATTERN_BIT_CHANGE,
	DRAM_TEST_PATTERN_CUSTOM1,
	DRAM_TEST_PATTERN_MAX
};


/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define READ8(a) (*((volatile UINT8*)(a)))
#define WRITE8(a,b)	*(volatile UINT8*)(a) = (b)
#define READ16(a) (*((volatile UINT16*)(a)))
#define WRITE16(a,b)	*(volatile UINT16*)(a) = (b)
#define READ32(a) (*((volatile UINT32*)(a)))
#define WRITE32(a,b)	*(volatile UINT32*)(a) = (b)
#define GPIO_MASK(PinNo)	(0x00000001L << (PinNo))

#define appTimeDelayMs(t)  do { if (t) sp5kTimeDelay( SP5K_TIME_DELAY_1MS , t); } while(0)

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef UINT32 (*bkfunc_t)(UINT32);

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern UINT32 ros_bkjob_send(UINT32 (*)(UINT32), UINT32);
extern void   avCunit(void);
extern void   audioCunit(void);
extern void   dispCunit(void);
extern UINT32 WriteFile(UINT8 *fname, UINT8 *buf, UINT32 size);
void cmdDefaultSet(UINT32 enable);
void sioCtrl(UINT32 opt);
void cmdSioFilter(UINT8 *arg[], UINT32 v[]);
void codeSum(void);
void cmdMemDump(UINT8 *param);
UINT32 halDmaMemcpy(void *pdst, void *psrc, UINT32 size);
#if SPCA5330

void halPllDispPanelClkSet(UINT32, UINT32);
#else
void halPllDispClkSet(UINT32, UINT32);
#endif
UINT32 appCustomerCmd(UINT8 argc, UINT8 *arg[], UINT32 v[]);

/* Edward for IQ debug */
extern SINT32 pv_agcidx_to_svidx(SINT32 agcidx);
extern SINT32 cap_agcidx_to_svidx(SINT32 agcidx);
extern UINT8 svidx_to_aeIso_tblidx(SINT32 svidx);
extern UINT32 _IqSetBlockIQ(IQ_SWH_t *iq_swh);
extern void appAePvAutoExpTblSet(UINT8 idx_start, UINT8 step, UINT8 dir);
extern void appAePvAutoExpTblSingleSet(UINT8 idx);
extern void appAePvAutoExpTblCurrIdxGet(void);
extern void appAePvAutoExpTblSwapSet(UINT8 idx1, UINT8 idx2, UINT8 times);
extern void appCalibLSC(void);
extern void appCalibPVOB();
extern void sensorCmdPortExpLineSend(UINT16 expLine);
extern void appAeTablePrint(UINT32 fps,UINT32 flicker);
extern void tbSensor_PreViewFlickerTableScan(void);
extern void appStillSpeedyCapture(UINT32	CapNum);
extern UINT32 tmrTimeClockGet();
extern void sensor_frame_cb_list_unreg(UINT32 svcid, UINT32 cbidx);
extern UINT32 sensor_frame_repeat_cb_reg(UINT32 svcid,void *f,UINT32 param);
extern void appIsInPBMode(void);
extern SINT32 cap_svidx_to_agcidx(SINT32 svidx);
extern void _stillUtil_ShutterDelaySet(void);
extern void appBtnEnable(UINT32 btnID);
extern void appAePreviewShutBoundarySet(UINT8 pasmvs,UINT32 shH, UINT32 shL);
extern void appBtnDisable(UINT32 btnID);
extern void appStillSnapIqSet(void);
extern UINT32 sensorSerialBlockRead(UINT32 cmdportid,UINT32 nreg,UINT8 *pdata,UINT32 sync);
extern UINT32 sensorSerialBulkWrite(UINT32 cmdportid,UINT32 nreg,const UINT8 *pdata,UINT32 sync);
extern void AppBuzzerOn(UINT32 freq,UINT32 duty);
extern void AppBuzzerOff(void);
extern void appOsd_FontIconFontSet(unsigned char *szIconFile);
extern void appAeFlickerTableExpAgcIdxSet(UINT32 tblIdx);
extern void util_start_connection(UINT8 wifiParm);
extern void appStillCaptureTrigger(sp5kUsbTrigCaptureType_e cap_type);
extern UINT32 usbPIMAGetMPFQv(void);

extern unsigned char* appOsd_FontIconResSelect();
extern void appStillBackgroundDiskFreeUpdate(UINT32 dskFree);
extern void ros_thread_sleep(UINT32 ms);
extern UINT32 appStillBackgroundGetFree(UINT32* dskFree);
extern UINT32 trigFlg;

extern void appCdvVideoRecLapseStart(void);
extern void appCdvVideoRecLapseStop(void);




/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
void cmdInit(void);
void cmdMonitor(void);
void cmdCUnitInit(void);
void cmdUsbInit(void);
void cmdGpeInit(void);
void cmdNandInit(void);
void cmdPwrInit(void);
void cmdCdspInit(void);
/*s Mask by Aries 10/05/24*/
#if 0
UINT32 dramReliabilityTest(UINT32, UINT32);
#endif
/*e Mask by Aries 10/05/24*/
void memdump(void *mem, UINT32 len);

void cmdLcdCalibrateInit(void);
void jpeg_add_quant_table(int quality, unsigned char *qtable);
/*s Add by Aries 10/05/18*/
void switchHdmi(void);
void showJPG(void);
/*e Add by Aries 10/05/18*/
/**************************************************************************
 *                          L O C A L    D A T A                          *
 **************************************************************************/

/**************************************************************************
 *                                                                        *
 *  Function Name: cmdTask                                                *
 *                                                                        *
 *  Purposes: Task body of RS-232 command shell                           *
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
#if CMD_TASK_INIT
extern void cmdGlobalInit(void);
void
cmdAppInit(
	void
)
{
	printf("******************************\n");
	printf("*  Press 'Enter' to continue  *\n");
	printf("******************************\n");
	cmdMemInit();   /* commands for dump/fill/search/ */
	cmdOsInit();    /* commands for displaying OS information      */
	cmdDosInit();   /* commands for MS-DOS like FS */
	cmdNandInit();
	cmdCdspInit();
	cmdDiskInit();
	cmdGlobalInit();
	cmdAaaInit();
	cmdLcdCalibrateInit();
	cmdIspInit(); /*xian ++ */
	cmdGdbInit();	/*dw++*/
	cmdSpiInit();
	cmdGpioInit();
	cmdRawYuvInit();/* commands for view/snap/play RAW/YUV */
	cmdDefaultSet(1);

	#if ICAT_WIFI
	extern void net_system_init(); net_system_init();
	#endif

	printf("cmdAppInit Finished\n");
}
UINT32 appCalibRawGet(UINT32 addr, UINT32 fmt, void *rawInfo)
{
	sp5kImgWbInfo_t *info=(sp5kImgWbInfo_t*)rawInfo;
	static UINT16 rawfilenumber=0 ;
	UINT32 filename[32];

	sprintf((UINT8 *)filename, "D:\\CAL%d_%dx%d.RAW",rawfilenumber, info->width, info->height);
	WriteFile((UINT8 *)filename, (UINT8 *)addr, info->width*info->height*2 );
	rawfilenumber++;
	printf("### %s  %d\n",__FUNCTION__ ,__LINE__);

	return 0;
}

void AppVideoSeamlessCB(void)
{
	UINT32 value,ret;
	tmx_t rtc;
	UINT8 fileChar[6];

	sp5kRtcDateTimeGet((UINT32)SP5K_DATE_TIME_OPTION, &rtc);	/*	get current RTC time */
	value = (rtc.tmx_mon*100 )+ rtc.tmx_mday ;  			/*replace "SUNP" as 月日"  */
	sprintf(fileChar ,"%.4d" , value);
	ret=sp5kDcfFsFreeCharSet("MEDIA" , fileChar, SP5K_DCF_NEXTKEY_MAX);
	value = (rtc.tmx_min*100 )+ rtc.tmx_sec ;
	sp5kDcfFsNextKeySet(100 ,value);
}


#define CP_BUF_SIZE	256*1024
void appFileCopy(char *src, char *dst)
{
	UINT32 fdSrc,fdDst;
	UINT8 *buf;
	UINT32 l;
	printf("cp [%s] [%s]\n", src, dst);
	fdSrc = sp5kFsFileOpen(src, SP5K_FS_OPEN_RDONLY);
	if (fdSrc==0) {
		printf("Cannot open src:%s\n", src);
		return;
	}
	fdDst = sp5kFsFileOpen(dst, SP5K_FS_OPEN_CREATE);
	if (fdDst==0) {
		printf("Cannot open dst:%s\n", dst);
		sp5kFsFileClose(fdSrc);
		return;
	}
	l = sp5kFsFileSizeGet(fdSrc);
	buf = sp5kMalloc(CP_BUF_SIZE);
	if (buf==NULL) {
		printf("malloc err\n");
		goto cp_closefile;
	}
	while (l) {
		UINT32 size=l>=CP_BUF_SIZE ? CP_BUF_SIZE : l;
		sp5kFsFileRead(fdSrc, buf, size);
		sp5kFsFileWrite(fdDst, buf, size);
		l -= size;
		printf(".");
	}
	printf("\n");
	sp5kFree(buf);
cp_closefile:
	sp5kFsFileClose(fdDst);
	sp5kFsFileClose(fdSrc);
}

#if SP5K_SENSOR_TG_AD9000_SONY675
static UINT32
rawSonySave(
	UINT32 Addr,
	UINT32 RawFmt,
	void* ImgSize
)
{
	static UINT32 n=0;
	UINT8 fname[32];
	sp5kImgSize_t *size=(sp5kImgSize_t*)ImgSize;
	sprintf(fname, "D:\\%04d.RAW", n);
	WriteFile(fname, (UINT8*)Addr, size->width*size->height*2);
	n++;
	return 0;
}
#endif
TX_EVENT_FLAGS_GROUP  evtCmdTask;
#define EVENT_CMD_DONE 0x0001

static UINT32 frm_time_t0_tag;
static UINT32 frm_time_t1_tag;
static UINT32 frm_time_count;
static UINT32 frm_time_count_max=10;/*average n frames*/
static UINT32 frm_time_cal_id;
static UINT32 frame_rate_h,frame_rate_l,frame_time;

void frm_rate_cb_proc(void)
{
	if(!frm_time_count)
		frm_time_t0_tag=tmrTimeClockGet();
	if(frm_time_count>=frm_time_count_max)
	{
		UINT64 temp0,temp1;
		frm_time_t1_tag=tmrTimeClockGet();
		sensor_frame_cb_list_unreg(0,frm_time_cal_id);/*destroy callback*/
		temp0=((UINT64)frm_time_t1_tag-(UINT64)frm_time_t0_tag);
		#if 1 /* mark by Phil.Lin for the UINT64 is NOT long enough for the const */
		temp1=(UINT64)frm_time_count*(UINT64)(1000000000000)/temp0;
		#else
		temp1=(UINT64)frm_time_count*(UINT64)(1000000000)/temp0;
		#endif
		frame_time=temp0;
		frame_rate_l=(UINT32)(temp1%(UINT64)1000000);
		frame_rate_h=(UINT32)(temp1/(UINT64)1000000);
		frm_time_cal_id=0;
		sp5kOsEventFlagsSet(&evtCmdTask,EVENT_CMD_DONE,TX_OR);
	}
	else
		frm_time_count++;
}

void frm_rate_cb_proc_report()
{
	printf("\nFrame rate calculation done !!!");
	printf("\nFrame Time %dx= %d us, FrameRate MHz =%d.%d\n\n",frm_time_count,frame_time,frame_rate_h,frame_rate_l);
	frame_rate_h=0;
	frame_rate_l=0;
	frame_time=0;
}
void FrmRateCalProc(void)
{
	ULONG ulActualEvents;
	sp5kOsEventFlagsCreate(&evtCmdTask, "CmdTaskEvent");
	frm_time_cal_id=sensor_frame_repeat_cb_reg(0,frm_rate_cb_proc, 0);
	sp5kOsEventFlagsGet(&evtCmdTask,EVENT_CMD_DONE,TX_OR_CLEAR,&ulActualEvents,TX_WAIT_FOREVER);
	frm_rate_cb_proc_report();
	sp5kOsEventFlagsDelete(&evtCmdTask);/*Destroy the event*/
	frm_time_count=0;
}
/*s Add by Aries 10/05/18*/
/**************************************************************************
 *                                                                        *
 *  Function Name: switchHdmi                                                *
 *                                                                        *
 *  Purposes: Task body of RS-232 command shell                           *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 *************************************************************************/
void switchHdmi(void)
{
#if KIT_WITH_HDMI

#if SP5K_AUD_DEVICE_ANA_ANX7150 /* SPCA5310 */

	UINT32 i;
	unsigned char c = 0x00;
	sp5kDispDevSwitch(SP5K_DISP_DEV_HDMI);
	ANX7150_API_DetectDevice();
	ANX7150_API_Initial();
	ANX7150_API_ShutDown(0);
	ANX7150_API_HDCP_ONorOFF(0/* stanley 1*/);
	for (i=0;i<100;i++) {
		if (!ANX7150_shutdown) {
			ANX7150_API_Input_Changed();
			ANX7150_Task();
										/*config ANX7150 after system is stable*/
			if(!ANX7150_app_hold_system_config) {
				if( ANX7150_parse_edid_done == 1
						&&	 ANX7150_system_config_done == 0)
				{
					printf("ANX7150_parse_edid_done = %d \n", ANX7150_parse_edid_done);
					printf("ANX7150_system_config_done = %d \n", ANX7150_system_config_done);

					/*system should config all the parameters here*/
					debug_puts("******** system config here.");
				  /*  ANX7150_API_PRINT_EDID_Parsing_Result();*/
					ANX7150_API_System_Config();
					ANX7150_system_config_done = 1;
				}
			}
			else {
				ANX7150_system_config_done = 0;
			}
		}
		delay_ms(5);
	}

	ANX7150_i2c_write_p1_reg(0x07,0x4);
	ANX7150_i2c_read_p1_reg(0x07, &c);
	debug_puts("0x07, data = 0x%.2x\n",c );

	ANX7150_i2c_write_p1_reg(0x4,0x12);
	ANX7150_i2c_read_p1_reg(0x4, &c);
	debug_puts("0x4, data = 0x%.2x\n",c );

	ANX7150_i2c_write_p1_reg(0x5,0x28);
	ANX7150_i2c_read_p1_reg(0x5, &c);
	debug_puts("0x51, data = 0x%.2x\n",c );

#endif
#if SP5K_AUD_DEVICE_EP_EP932M
	UINT8 i;
	printf("HDMI 2\n");

	EP_HDMI_Init();
	printf("EP_HDMI_Init\n");

	EP_HDMI_Set_Video_Timing( 0 );   /*need change parameter*/
	printf("EP_HDMI_Set_Video_Timing\n");
	EP_HDMI_Set_Video_Output(0);
	printf("EP_HDMI_Set_Video_Output\n");
	EP_HDMI_SetAudFmt(0, 3/*AUD_SF_48000Hz*/); /* need change Audio sample rate */
	printf("EP_HDMI_SetAudFmt\n");

	for (i=0;i<200;i++) {

	/*	printf("i=%d\n",i); */
		EP932Controller_Task();
		EP932Controller_Timer();
	}

	UINT8 c = 0x00;
	IIC_Write(0x70, 0x3e, &c,1);
#endif
#endif
}

/**************************************************************************
 *                                                                        *
 *  Function Name: showJPG                                                *
 *                                                                        *
 *  Purposes: Task body of RS-232 command shell                           *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 *************************************************************************/
void showJPG(void)
{
#if KIT_WITH_HDMI

#if SPCA5310
	UINT32 w = 1280, h = 720;
	UINT32 panel_w, panel_h;
	UINT32 arr[4] = {0,0, w, h};
	pipLayerCfg_t pLayer;
	sp5kDispDevAttrSet(SP5K_DISP_OSD_ACTIVE, 0, 0, 0, 0);
	sp5kDispDevAttrSet(SP5K_DISP_IMG_ACTIVE, 1, 0, 0, 0);
	sp5kDispDevAttrSet(SP5K_DISP_BG_COLOR,255,255,255, 0);
	sp5kPipInitCfgSet(SP5K_PIP_INIT_MAIN_FRAME_SIZE, w, h, 0, 0);
	sp5kPipInit();
	pLayer.alpha = 32;
	pLayer.frmH = h;
	pLayer.frmW = w;
	pLayer.frmRoiH = h;
	pLayer.frmRoiW = w;
	pLayer.isShow = 1;
	pLayer.winX = 0;
	pLayer.winY = 0;
	sp5kPipLayerCreate(0, &pLayer);
	sp5kDispDimensionGet(&panel_w, &panel_h);
	sp5kDispDevAttrSet(SP5K_DISP_IMG_WINDOW,0,0,panel_w, panel_h);
	sp5kResourceFilePagePlay(SP5K_RES_TYPE_JPG, "D:\\2.JPG", SP5K_GFX_PAGE_PIP_BASE+0, arr, 0);
	sp5kGfxAttrSet(SP5K_GFX_PIP_REFRESH_ACTIVE,1,0,0,0);

	if(getch()) {
		sp5kPipLayerDelete(0);
		sp5kPipTerm();
	};
#endif
#endif
}


/**************************************************************************
 *                                                                        *
 *  Function Name: showOSD                                                *
 *                                                                        *
 *  Purposes: Task body of RS-232 command shell                           *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 *************************************************************************/
void showOSD(void)
{
#if KIT_WITH_HDMI

#if SPCA5310
	UINT32 dispWidth,dispHeight;

	sp5kGfxPageClear(SP5K_GFX_PAGE_OSD_0,0);

	sp5kDispDevAttrSet(SP5K_DISP_OSD_ACTIVE, 1, 0, 0, 0);
	sp5kDispDevAttrSet(SP5K_DISP_IMG_ACTIVE, 1, 0, 0, 0);
	sp5kDispDevAttrSet(SP5K_DISP_BG_COLOR,255,255,255, 0);
	sp5kDispDimensionGet(&dispWidth, &dispHeight);

	sp5kGfxInitCfgSet(SP5K_GFX_OSD_WINDOW, 0, 0, dispWidth, dispHeight);

	sp5kGfxInitCfgSet(SP5K_GFX_OSD_PAGE_TOT, 1, 0, 0, 0);
	sp5kGfxInitCfgSet(SP5K_GFX_OSD_FMT, SP5K_GFX_FMT_IDX8, 0, 0, 0);
	sp5kGfxInitCfgSet(SP5K_GFX_AB_FRAME_SIZE, 640, 480, 0, 0);

	sp5kGfxInitCfgSet(SP5K_GFX_OSD_FRAME_SIZE, 320, 240, 0, 0);
	sp5kGfxInitCfgSet(SP5K_GFX_OSD_WINDOW, 40, 0, dispWidth-80, dispHeight-40);
	sp5kGfxInitCfgSet(SP5K_GFX_PIP_WINDOW, 40, 0, dispWidth-80, dispHeight -40);

	sp5kGfxInit();
	appIsInPBMode();
#endif
#endif
}
/*e Add by Aries 10/05/18*/

/*s Add by Aries 10/05/24*/
static const char *DramTestStr[DRAM_TEST_PATTERN_MAX]={
	[DRAM_TEST_PATTERN_BIT_CHANGE] = "toggle all bits",
	[DRAM_TEST_PATTERN_CUSTOM1] = "0080 0000 0000 FFFF",
};

#define DRAM_TEST_LOOP_CNT	0x10000

UINT32 dramReliabilityTest_CMD(UINT32 n, UINT32 opt)
{
	UINT32 i, k, errcnt, totalerr, err;
	UINT16 *pmembuf[3];
	UINT16 rs, rb1, rb2;

	if (opt>=DRAM_TEST_PATTERN_MAX)
		return FAIL;
	printf("# DRAM test: %s\n", DramTestStr[opt]);

	const UINT32 datasize = opt==DRAM_TEST_PATTERN_BIT_CHANGE ? 2 : 4;
	const UINT32 bufsize = DRAM_TEST_LOOP_CNT * datasize * sizeof(UINT16);

	#if 1
	pmembuf[0] = (UINT16 *)sp5kMalloc(bufsize);
	pmembuf[1] = (UINT16 *)sp5kMalloc(bufsize);
	pmembuf[2] = (UINT16 *)sp5kMalloc(bufsize);
	#else
	pmembuf[0] = (UINT16 *)0xA0700000;
	pmembuf[1] = (UINT16 *)0xA0800000;
	pmembuf[2] = (UINT16 *)0xA0900000;
	#endif

	for(i=0 ; i<DRAM_TEST_LOOP_CNT ; i++) {
		if (opt==DRAM_TEST_PATTERN_BIT_CHANGE) {
		*(pmembuf[0] + i*2) = i;
		*(pmembuf[0] + i*2 +1) = (0xFFFF - i);
		} else { /* custom1 */
		*(pmembuf[0] + i*4)   = 0x0080;
		*(pmembuf[0] + i*4+1) = 0x0000;
		*(pmembuf[0] + i*4+2) = 0x0000;
		*(pmembuf[0] + i*4+3) = 0xFFFF;
		}
	}

	/*printf("Buffer Allocation SRC=%x , DST1=%x , DST2=%x \n",(UINT32) pmembuf[0],(UINT32) pmembuf[1],(UINT32) pmembuf[2]);*/
	err = totalerr = 0;
	for(i=1 ; i<=n ; i++) {
		printf("\r%d ", i);
		if(i!=0xFFFF) {
			halDmaMemcpy((UINT8 *)pmembuf[1],(UINT8 *)pmembuf[0], bufsize);
			halDmaMemcpy((UINT8 *)pmembuf[2],(UINT8 *)pmembuf[1], bufsize);
		}
		/* if (i==10) pmembuf[1][100] = 0x1234; */
		errcnt = 0;
		for(k=0 ; k<DRAM_TEST_LOOP_CNT*datasize ; k++) {
			rs = *(pmembuf[0]+k);
			rb1 = *(pmembuf[1]+k);
			rb2 = *(pmembuf[2]+k);
			if(( rs != rb1 ) || (rs != rb2)) {
				printf("\nPos=%x --- src=%x, dst1=%x, dst2=%x\n", (k*2), rs, rb1,rb2);
				errcnt ++;
			}
		}
		if (errcnt) {
			/*printf("ERROR CNT = %d \n", errcnt);*/
			err = 1;
			totalerr += errcnt;
		}
	}
	#if 1
	sp5kFree(pmembuf[0]);
	sp5kFree(pmembuf[1]);
	sp5kFree(pmembuf[2]);
	#endif
	if (err) {
		printf("\nFail, total err=%d\n", totalerr);
		return totalerr;
	} else {
		printf("\nPass !!\n");
		return 0;
	}
}
/*e Add by Aries 10/05/24*/

#if 1
/**
 * @fn        void appCmdVirtualKBHelp(void)
 * @brief     Help info for virtual key board
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-11-6
 * @todo      N/A
 * @bug       N/A
*/
void
appCmdVirtualKBHelp(
	void
)
{
	printf("\n");
	printf("Enter Host Key Control !!\n");
	printf("\n");
	printf(" ---------------------------------\n");
	printf("|            / = (PB)   * =(DEL)  |\n");
	printf("| 7 = (MENU) 8 = (UP)   9 =(MODE) |\n");
	printf("| 4 = (LEFT) 5 = (SET)  6 =(RIGHT)|\n");
	printf("| 1 = (WIDE) 2 = (DOWN) 3 =(TEL)  |\n");
	printf("| 0 = (S2)              0 =(S1)   |\n");
	printf(" ---------------------------------\n");
	printf("\n");
}

/**
 * @fn        UINT8 appCmdVirtualKBHdl(UINT8 ch)
 * @brief     virtual key board handle
 * @param     [in] ch
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-11-6
 * @todo      N/A
 * @bug       N/A
*/
int
appCmdVirtualKBHdl(
	int ch
)
{
	struct {
		UINT8 ch;
		UINT32 msg;
	} kbMap[] = {
		                     {'/',KEY_GROUP_PB},  {'*',KEY_GROUP_DEL},  {'-',KEY_GROUP_WIDE},
		{'7',KEY_GROUP_MENU},{'8',KEY_GROUP_UP},  {'9',KEY_GROUP_MODE}, {'+',KEY_GROUP_TELE},
		{'4',KEY_GROUP_LEFT},{'5',KEY_GROUP_SET}, {'6',KEY_GROUP_RIGHT},
		{'1',KEY_GROUP_WIDE},{'2',KEY_GROUP_DOWN},{'3',KEY_GROUP_TELE},
		{'0',KEY_GROUP_S2},                       {'.',KEY_GROUP_S1},
	};

	UINT32 i;

	for(i = 0; i < sizeof(kbMap); i++) {
		if (kbMap[i].ch == ch) {
			sp5kHostMsgSend((APP_KEY_MSG|kbMap[i].msg|KEY_PRESS), 0);
			sp5kHostMsgSend((APP_KEY_MSG|kbMap[i].msg|KEY_RELEASE), 0);
			break;
		}
	}

	return 0;
}
/**
 * @fn        UINT8 appCmdVirtualKB(t argc, UINT8 *argv)
 * @brief     virtual key board command line parsing
 * @param     [in] argc
 * @param     [in] argv
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-11-6
 * @todo      N/A
 * @bug       N/A
*/
void
appCmdVirtualKB(
	int argc,
	UINT8 *argv[]
)
{
	/*UINT32 sr;*/
	UINT8 ch = 0;

	if (argc > 1 && !strcmp(argv[1], "off")) {
		printf("VKB off\n");
		/*ENTER_CRITICAL(sr);*/
		sp5kUartRxCallbackSet(0, NULL);
		/*EXIT_CRITICAL(sr);*/
	} else {
		appCmdVirtualKBHelp();

		if (argc > 1 && !strcmp(argv[1], "on")) {
			printf("VKB on\n");
			/*ENTER_CRITICAL(sr);*/
			sp5kUartRxCallbackSet(0, appCmdVirtualKBHdl);
			/*EXIT_CRITICAL(sr);*/
		} else {
			printf("Quit by press ESC ..\n");
			while(ch != 0x1B){ /* if ESC, quit */
				ch = (UINT8)getch();
				appCmdVirtualKBHdl(ch);
			}
		}
	}
}
#endif

#define ARG_MAX	16
#define FIF_LUT_LOAD(fif, id, bin) { UINT8*p=fifEntryFind(fif, bin); if (p) cdspResourceDataLoad(id, p+8, 0); }

UINT8 G_USE_ENUM_KEY_MSG;

static void AeTestCmdHandle(void)
{
	/* Key Definition */

	UINT8 ch = 0;
	UINT32 key = 0xffffffff;
	sp5kExpLimit_t cap_expLimit;
	sp5kExpLimit_t prv_expLimit;
	sp5kAgcLimit_t agcLimit;
	UINT32 expidx,agcidx;
	UINT32 smode,smodeidx;
	sp5kAeExpLimitGet(0x30,&prv_expLimit);
	sp5kAeExpLimitGet(0x40,&cap_expLimit);
	sp5kAeAgcLimitGet(0x30,&agcLimit);
	smode=sp5kSensorModeCfgGet(SP5K_MODE_STILL_PREVIEW);
	smodeidx=smode;
	sp5kAeModeSet(0);
	sp5kAwbModeSet(0);
	printf("AE/AEB Off\n");
	printf("\n");
	printf("Enter Front Test Control !!\n");
	printf("Preview index Min=%d Max=%d\n",prv_expLimit.expIdxTblMin,prv_expLimit.expIdxMax);
	printf("Capture index Min=%d Max=%d\n",cap_expLimit.expIdxTblMin,cap_expLimit.expIdxMax);
	printf("Agc     index Min=%d Max=%d\n",agcLimit.agcIdxMin,agcLimit.agcIdxMax);
	sp5kPreviewExpAgcIrisGet(&expidx,&agcidx,NULL);
	printf("\n");
	printf("+-------------------------------------------------------+\n");
	printf("| 7 = (Smode-)   8 = ( AE ++   )  9 =(Smode+)             |\n");
	printf("| 4 = (AGC --) E(%3d) G(%3d) M(%2x)6 =(AGC ++)             |\n",expidx,agcidx,smode);
	printf("| 1 = (      )   2 = ( AE --   )  3 =(      )             |\n");
	printf("+-------------------------------------------------------+\n");
	printf("\n");
	printf("Quit by press ESC ..\n");
	printf("\n");
	G_USE_ENUM_KEY_MSG = 1;
	while(ch != 0x1B){/* if ESC, quit */
		ch = (UINT8 )getch();
		printf("ch=0x%x\n",ch);
		sp5kPreviewExpAgcIrisGet(&expidx,&agcidx,NULL);
		switch ( ch ) {
		case 0x30:/* ASM/Delete key */
			key = APP_KEY_PRESS_DEL;
			break;
		case 0x31:/* WIDE key */
			key = APP_KEY_PRESS_WIDE;
			break;
		case 0x32:/* Down key */
			key = APP_KEY_PRESS_DOWN;
			if(!expidx)
			expidx--;
			break;
		case 0x33:/* TEL key */
			key = APP_KEY_PRESS_TELE;
			break;
		case 0x34:/* LEFT Key */
			key = APP_KEY_PRESS_LEFT;
			if(!agcidx)
			agcidx--;
			break;
		case 0x35:/* SET key */
			key = APP_KEY_PRESS_SET;
			break;
		case 0x36:/* RIGHT key */
			key = APP_KEY_PRESS_RIGHT;
			agcidx++;
			break;
		case 0x37:/* MENU key */
			key = APP_KEY_PRESS_MENU;
			if(smodeidx<0x3F)
				smodeidx++;
			break;
		case 0x38:/* UP key */
			key = APP_KEY_PRESS_UP;
			expidx++;
			break;
		case 0x39:/* MODE key */
			key = APP_KEY_PRESS_MODE;
			if(smodeidx>0x30)
				smodeidx--;
			break;
		case 0x61:/* MODE key A */
			key = APP_KEY_RELEASE_S2;
			break;
		case 0x62:/* MODE key B*/
			key = APP_KEY_RELEASE_S1;
			break;
		case 0x63:/* MODE key C*/
			key = APP_KEY_PRESS_S1;
			break;
		case 0x64:/* MODE key D*/
			key = APP_KEY_PRESS_S2;
			break;
		 case 0x65:/* MODE key E*/
			key = 0xffffffff;
			break;
		default:
			key = 0xffffffff;
			break;
		}
		if ( key != 0xffffffff ) {
			printf("key=%x\n",key);
		}
		sp5kPreviewExpAgcIrisSet(expidx,agcidx,0x7FFFFFFF);
		sp5kSensorVvalidWait();
		sp5kSensorVvalidWait();
		sp5kPreviewExpAgcIrisGet(&expidx,&agcidx,NULL);
		smode=sp5kSensorModeCfgGet(SP5K_MODE_STILL_PREVIEW);
		if (smode!=smodeidx) {
			sp5kSensorModeCfgSet(SP5K_MODE_STILL_PREVIEW,smodeidx);
			sp5kModeSet(SP5K_MODE_STILL_PREVIEW);
			sp5kModeWait(SP5K_MODE_STILL_PREVIEW);
			smodeidx=sp5kSensorModeCfgGet(SP5K_MODE_STILL_PREVIEW);
		}
		printf("\n");
		printf("Enter Front Test Control !!\n");
		printf("Preview index Min=%d Max=%d\n",prv_expLimit.expIdxTblMin,prv_expLimit.expIdxMax);
		printf("Capture index Min=%d Max=%d\n",cap_expLimit.expIdxTblMin,cap_expLimit.expIdxMax);
		printf("Agc 	index Min=%d Max=%d\n",agcLimit.agcIdxMin,agcLimit.agcIdxMax);
		printf("\n");
		printf("+-------------------------------------------------------+\n");
		printf("| 7 = (Smode-)	 8 = ( AE ++   )  9 =(Smode+)			  |\n");
		printf("| 4 = (AGC --) E(%3d) G(%3d) M(%2x)6 =(AGC ++)			   |\n",expidx,agcidx,smode);
		printf("| 1 = ( 	 )	 2 = ( AE --   )  3 =(		)			  |\n");
		printf("+-------------------------------------------------------+\n");
		printf("\n");
		printf("Quit by press ESC ..\n");
		printf("\n");
	}

	printf("Quit Host Key Control !!\n");
	printf("\n");
}

/**************************************************************************
 *                                                                        *
 *  Function Name: customerCmd                                            *
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
 /*s Modify by Aries 09/12/13*/
extern UINT32 tmrMsTimeGet(void);
void dispTestEvtIsr(void)
{
	printf("\t\t\t\ttime:%dms\n", tmrMsTimeGet());
}

void appCardIspControl(UINT32 elapseCnt, UINT32 totalCnt)
{
	sioCtrl(0);
	printf("Step to %d/%d ...\n", elapseCnt, totalCnt);
	if(elapseCnt == totalCnt)
	{
		tmx_t rtc;
		UINT32 r,reliableCode;
		r = sp5kRtcReliableGet(&reliableCode);
		sp5kRtcDateTimeGet(SP5K_DATE_TIME_OPTION | SP5K_DATE_TIME_BLOCK_WAIT, &rtc);
		rtc.tmx_sec += 5;
		if (rtc.tmx_sec >= 60) {
			rtc.tmx_sec -= 60;
			rtc.tmx_min++;
			if (rtc.tmx_min >= 59) {
				rtc.tmx_min -= 59;
				rtc.tmx_hour++;
			}
		}
		sp5kRtcDateTimeSet(SP5K_WAKEUP_OPTION | SP5K_DATE_TIME_BLOCK_WAIT, &rtc);
	}
}

void customerCmd
(
	int argc,
	UINT8 *argv[]
)
{
	if (!*argv) {
		printf("null custom command\n");
		return;
	}

#if 0

	if(strcmp(*argv, "disp1")==0)
	{
		dispCallbackReg(DISP_EVENT_PANEL_VSYNC_FALL, dispTestEvtIsr, 5, 10);
		printf("\t\t\t\tF time:%dms\n", tmrMsTimeGet());
	}
	if(strcmp(*argv, "disp2")==0)
	{
		dispCallbackReg(DISP_EVENT_PANEL_VSYNC_RISE, dispTestEvtIsr, 5, 10);
		printf("\t\t\t\tF time:%dms\n", tmrMsTimeGet());
	}
	if(strcmp(*argv, "disp3")==0)
	{
		dispCallbackReg(DISP_EVENT_LCM_VSYNC_FALL, dispTestEvtIsr, 5, 10);
		printf("\t\t\t\tF time:%dms\n", tmrMsTimeGet());
	}
	if(strcmp(*argv, "disp4")==0)
	{
		dispCallbackReg(DISP_EVENT_LCM_VSYNC_RISE, dispTestEvtIsr, 5, 10);
		printf("\t\t\t\tF time:%dms\n", tmrMsTimeGet());
	}

#endif

#if 1

	if(strcmp(*argv, "sdupd")==0)
	{
		#define FW_FILE   	"D:\\SPHOST.BRN"

		UINT32 curMode;

		sp5kModeGet(&curMode);
		if(curMode != SP5K_MODE_STANDBY){
			sp5kModeSet(SP5K_MODE_STANDBY);
			sp5kModeWait(SP5K_MODE_STANDBY);
		}

		printf("start FW update...\n");
		if (sp5kDscFirmwareUpdate( FW_FILE, appCardIspControl )==SUCCESS)
		{
			printf("FW update OK!\n");
			return;
		}
	}


#endif


/*test timelapse h.264 sample code must update basefw to 17859 or higher*/
#if 0 /*IBBP test*/
		else if (strcmp(*argv, "vtls") == 0)
		{
			UINT32 i;
			if (v[1] == 1)
			{	/*start*/
				startFunc();
			}
			else if (v[1] == 2){   /*trigger*/
				printf("trigger one\n");
				trigFlg = 1;
			}
			else if(v[1] == 0){  /*stop*/
				stopFunc();
			}
		}
#elif 0 /*IIII test*/
		if (strcmp(*argv, "vtls") == 0) {
			UINT32 v = strtoul(argv[1], NULL, 0);
			if (v == 1)
			{	/*start*/
				appTimelapseStart();
			}
			else if (v == 2)
			{	/*trigger*/
				printf("trigger one\n");
				trigFlg = 1;
			}
			else if(v == 0)
			{  /*stop*/
				appTimelapseStop();
			}
		}
#endif

	UINT32 i,v[ARG_MAX];
	memset(v,0,sizeof(v));
	for (i=0 ; i<(UINT32)argc ; i++)
	{
		if(strncmp(argv[i], "0x", 2) == 0 && strlen(argv[i]) >= 3)
		{
			sscanf(argv[i], "0x%8x", &v[i]);
		}
		else
		{
			v[i] = strtoul(argv[i], NULL, 0);
		}
	}

	if (strcmp(*argv, "dramtest")==0) {
		/*s Modify by Aries 10/05/24*/
		if (v[1]==0) v[1] = 1000;
		if (v[2]>DRAM_TEST_PATTERN_MAX) v[2] = DRAM_TEST_PATTERN_BIT_CHANGE;
		if (argc<=2) {
			UINT32 i;
			for (i=0 ; i<DRAM_TEST_PATTERN_MAX ; i++) {
				dramReliabilityTest_CMD(v[1], i);
			}
		}
		else {
			dramReliabilityTest_CMD(v[1], v[2]);
		}
		/*e Modify by Aries 10/05/24*/
	}else if(strcmp(*argv,"pool") == 0){
		printf("Pool:%d\n",v[1]);
		appCdfsPoolInfoDump(v[1]);
	}
	#if MSG_NEW_PROC
	else if (strcmp(*argv, "msg")==0) {
		appMsgDebugPrint();
	}
	#endif
	else if (strcmp(*argv, "vtls") == 0) {
		if (v[1] == 1)
		{	/*start*/
			appTimelapseStart();
		}
		else if (v[1] == 2)
		{	/*trigger*/
			printf("trigger one\n");
			trigFlg = 1;
		}
		else if(v[1] == 0)
		{  /*stop*/
			appTimelapseStop();
		}
	}
	else if (strcmp(*argv, "lapse") == 0) {
		appMsgFlush(APP_MSG_KEY_QUE);
		printf("--lapse\n");
	}
	else if (strcmp(*argv, "dxgvtls") == 0) {
		if (v[1] == 1)
		{	/*start*/
			appCdvVideoRecLapseStart();
		}
		else if(v[1] == 0)
		{  /*stop*/
			appCdvVideoRecLapseStop();
		}
	}
	#if ICAT_WIFI
	else if (strcmp(*argv, "wifi") == 0) {
		/*
			WIFI_LOAD 		0x01
			DHCP_SERVER    0x02
			HOST_AP		0x04
			PTP_IP			0x08
		*/
		if (argc == 3 && strncmp(argv[1], "on", 2) == 0) {
			uiPara_t* puiPara = appUiParaGet();
			puiPara->VideoSize=UI_VID_SIZE_HD_30FPS;
			appWiFiStartConnection(v[2]);
		}
		else if (argc == 2 && strncmp(argv[1], "off", 3) == 0) {
			appWiFiStopConnection(0x1F);
		}
		else if(argc == 2 && strncmp(argv[1], "mac", 3) == 0)
		{
			appMacAddressGet(NULL);
		}
		else {
			printf("Usage:\n");
			printf("  wifi on [OPTION]\n");
			printf("  wifi off\n");
		}
	}
	#if 0
	else if (strcmp(*argv, "ssid") == 0) { /*ssid apmode ssidnumber */
		extern UINT8 gssidname[20];
		extern void apModeParamSet(UINT8 modeParam);
		extern UINT8 apModeParamGet();
		UINT8 fname[20];
		if(v[1]<= 2)
			apModeParamSet(v[1]);
		else
			apModeParamSet(0);

		if(v[2]){
			sprintf(fname, "rtwap_custom%03d",v[2]);
			memcpy(gssidname,fname,20*sizeof(UINT8));
		}

		printf("apmode = %d[%s] ; ssid = %s\n",apModeParamGet(),apModeParamGet()==2?"WPA":"OPEN",gssidname);
	}
	#endif
	else if (strcmp(*argv, "excep")==0) {
		appExceptionHandle();
	}
	else if (strcmp(*argv, "dskf")==0) {
		UINT32 dskFree;
		appDiskInfo_t* pDsk;
		pDsk = appCurDiskInfoGet();
		appStillBackgroundDiskFreeUpdate(pDsk->freeSz);
		appStillBackgroundGetFree(&dskFree);
		printf("disk free:%d\n",dskFree);
	}
	else if (strcmp(*argv, "ptp") == 0) {
		if (argc > 1) {
			if (strcmp(argv[1], "usb") == 0) {
				sp5kUsbSIDCFuncReg();
			}
			else if (strcmp(argv[1], "ip") == 0) {
				extern void ptpip_reg_n_start(void);
				ptpip_reg_n_start();
			}
			else if (strcmp(argv[1], "push") == 0) {
				UINT32 idx = 1, max = 1;
				UINT8 drv = 'C';
				if (argc > 2)	drv = argv[2][0];
				if (argc > 3)	max = atoi(argv[3]);
				UINT8 fname[256];
				do {
					while (sp5kUsbPimaSendingFileCheck())
						ros_thread_sleep(100);
					if (idx > max)	break;
					sprintf(fname, "%c:\\DCIM\\100MEDIA\\SUNP%04d.JPG", drv, idx++);
					printf("%s\n",fname);
				} while(sp5kUsbPimaSpecFileSend(1, fname));
			}
			else if (strcmp(argv[1], "rot") == 0 && argc > 2) {
				while (sp5kUsbPimaSendingFileCheck())
					ros_thread_sleep(100);
				sp5kUsbPimaSpecFileSend(0, argv[2]);
			}
		}
	}

	#if SPCA6330
    else if (strcmp(*argv, "ptpcap")==0) {
       if (v[1] ==1) {
            appStillCaptureTrigger(0x00);
       }else{

       }
		printf("Show ptpcap\n");
    }
	else if (strcmp(*argv, "ptpevts") == 0) {
		switch (v[1]) {
			case PTP_EC_OBJECT_ADDED:
			case PTP_EC_OBJECT_REMOVED:
			case PTP_EC_OBJECT_INFO_CHANGED:
			case PTP_EC_REQUEST_OBJECT_TRANSFER:
				appStill_PIMA_Send_Event(v[1],(UINT32)argv[2],0, 0); /* refer to usbPIMAEventCode_e*/
				break;
			case PTP_EC_CANCEL_TRANSACTION:
			case PTP_EC_STORE_ADDED:
			case PTP_EC_STORE_REMOVED:
			case PTP_EC_DEVICE_PROP_CHANGED:
			case PTP_EC_DEVICE_INFO_CHANGED:
			case PTP_EC_STORE_FULL:
			case PTP_EC_DEVICE_RESET:
			case PTP_EC_STORAGE_INFO_CHANGED:
			case PTP_EC_CAPTURE_COMPLETE:
			case PTP_EC_ICATCH_HOST_EVENT:
				appStill_PIMA_Send_Event(v[1],v[2],v[3], v[4]); /* refer to usbPIMAEventCode_e*/
				break;
		}
	}
	else if (strcmp(*argv, "mpfqv") == 0) {
		usbPIMAGetMPFQv();
	}
	#endif
	#endif
	else if (strcmp(*argv, "pg")==0) {
		if (argc==1) {
			WRITE8(0xb00090d0, 0);
		} else {
			WRITE8(0xb00090d0, 1);
			WRITE8(0xb00090d6, v[1]); /* for snap: Sony8M:0 Sharp7M:2 Sharp6M:0 */
			if (argc>=3)
				WRITE8(0xb00090d4, v[2]);
		}
	}
	else if (strcmp(*argv, "r")==0) {
		static UINT32 addr=0x2480;
		if (argc<=1) v[1] = addr; /* dump last */
		if (argc<=2) v[2] = 32;
		if (v[1] < 0x80000000) v[1] += 0xb0000000;
		memdump((void *)v[1], v[2]);
		addr = v[1];
	}
	else if (strcmp(*argv, "w")==0) {
		if (argc>=3) { /* write */
			if (v[1] < 0x80000000) v[1] += 0xb0000000;
			WRITE8(v[1], v[2]);
			memdump((void *)v[1], 16);
		}
	}
	else if (strcmp(*argv, "cdsp")==0) {
		if (argc==1) {
			printf("1:OB 2:LSC 4:BP 8:CLR 20:Gamma 40:YGamma 100:WB 1000:Edge 2000:Denoise 20000:AHS 40000:AA 80000:HSC 100000:VMF\n");
		}
		else {
			UINT32 mask;
			printf("cdsp 0x%x %d\n", v[1], v[2]);
			for (mask=1 ; mask ; mask<<=1) {
				if (v[1] & mask)
					sp5kImgCdspBlockEnable(mask, v[2]);
			}
		}
	}
	else if (strcmp(*argv, "regdump")==0)
	{
		memdump((void*)0xb0000000, 0x100);
		memdump((void*)0xb0001000, 0x260);
		memdump((void*)0xb0002000, 0x500);
		memdump((void*)0xb0003000, 0x100);
		memdump((void*)0xb0004000, 0x100);
		memdump((void*)0xb0005000, 0x500);
		memdump((void*)0xb0006000, 0x200);
		memdump((void*)0xb0006400, 0x080);
		memdump((void*)0xb0007000, 0x100);
		memdump((void*)0xb0008000, 0x200);
		memdump((void*)0xb0009000, 0x250);
		memdump((void*)0xb0009300, 0x040);
		memdump((void*)0xb000a000, 0x150);
		memdump((void*)0xb000b000, 0x100);
	}
	else if(strcmp(*argv, "format")==0)
	{
		printf("--format %d\n", v[1]);
		//sp5kDiskFormat(v[1], "FAT32", 0); 	//DRIVE_SD SP5K_DRIVE_RSVB
		UINT8 ret = sp5kRsvBlkFormat(v[1]);
		if (ret != SUCCESS)
		{
			printf("format errxx=%x\n", ret);
			return ret;
		}
	}
	else if(strcmp(*argv, "mount")==0)
	{
		printf("mount %d\n", v[1]);
		sp5kDiskMount(v[1]);	//DRIVE_SD
	}
	else if (strcmp(*argv, "dd")==0)
	{
 		 if(strcmp(argv[1], "search")==0)
		{
			appCdfsCurrFolderFileListCreate(CDFS_FILE_FOLDER_VIDEO,"D:/VIDEO");
		}
		else if(strcmp(argv[1], "open")==0)
		{
			UINT32 fd;
			fd = sp5kFsFileOpen(argv[2], SP5K_FS_OPEN_RDONLY);
			if (fd==0)
			{
				printf("\nFILE Open failed !!\n\n");
			}
		}
		else if(strcmp(argv[1], "show")==0)
		{
			appCdfsFileListPrintf(strtol(argv[2],NULL,10));
		}
		else if(strcmp(argv[1], "del")==0)
		{
			appCdfsFileDel(argv[2]);
		}
		else if(strcmp(argv[1], "add")==0)
		{
			appCdfsFileAdd(argv[2]);
		}
		else if(strcmp(argv[1], "gpath")==0)
		{
			printf("currentpath=%s\n",sp5kFsGetCurPath());
 		}
 		else if(strcmp(argv[1], "crpath")==0)
 		{
			sp5kFsDirMake(argv[2]);
 		}
 		else if(strcmp(argv[1], "cdpath")==0)
 		{
			sp5kFsDirChange(argv[2]);
 		}
		else if(strcmp(argv[1], "cdfsinit")==0)
		{
			printf("appCdfsInit\n");
			appCdfsInit();
		}
 		else if(strcmp(argv[1], "time")==0)
 		{
 			printf("start\n");
			appCdfsFileHandleGet(9999);
			printf("end\n");
 		}
 		else if(strcmp(argv[1], "pre")==0)
 		{
			CDFS_TAB_T *pCur;
			appCdfsFileFolderSet(CDFS_FILE_FOLDER_VIDEO);
			printf("argv2=%s\n",argv[2]);
			pCur = appCdfsFileHandleGet(strtol(argv[2],NULL,10));
			while(pCur)
			{
				printf("filename=%s\n",pCur->FileName);
				pCur = pCur->pre;
			}
 		}
 		else if(strcmp(argv[1], "next")==0)
 		{
			CDFS_TAB_T *pCur;
			appCdfsFileFolderSet(CDFS_FILE_FOLDER_VIDEO);
			pCur = appCdfsFileHandleGet(strtol(argv[2],NULL,10));
			while(pCur)
			{
				printf("filename=%s\n",pCur->FileName);
				pCur = pCur->next;
			}
 		}
 		else if(strcmp(argv[1], "size")==0)
 		{
 			UINT64 size;
			size=appCdfsCanDelVideoSizeGet();
			printf("size = %u(B),%u(MB)\n",(UINT32)size,((UINT32)size)>>20);
 		}
 		else if(strcmp(argv[1], "event")==0)
 		{
 			printf("video event\n");
			appCdfsEventFlagSet(VIDEO_EVENT_IN_VIDEO);
 		}
	}
#if 0 /* set 1 for load snap cdsppara.bin */
	else if (strcmp(param, "cdsppara")==0) {
		extern void snapBinAddrSet(UINT32 id, UINT8 *pbuf);
		static UINT8 buf[2048];
		if (v[1]) {
			UINT32 fd,fl;
			fd = sp5kFsFileOpen("D:\\CDSPPARA.BIN", SP5K_FS_OPEN_RDONLY);
			if (fd) {
				fl = sp5kFsFileSizeGet(fd);
				sp5kFsFileRead(fd, buf, fl);
				sp5kFsFileClose(fd);
				printf("@#$ snapBinAddrSet\n");
				snapBinAddrSet(0, buf);
			}
		} else {
			snapBinAddrSet(0, NULL);
		}
	}
#endif
	else if (strcmp(argv[0], "cm")==0) {
		UINT32 addr=0;
		int ofs1,ofs2;
		switch (*argv[1]) {
		case 'r': addr=0xb00022c0; ofs1= 2; ofs2= 4; break;
		case 'g': addr=0xb00022c8; ofs1=-2; ofs2= 2; break;
		case 'b': addr=0xb00022d0; ofs1=-4; ofs2=-2; break;
		}
		if (addr) {
			printf("set %c coeff=%d %d %d\n", *argv[1], 0x40+v[2]+v[3], -v[2], -v[3]);
			WRITE16(addr, 0x40+v[2]+v[3]);
			WRITE16(addr+ofs1, 1024-v[2]);
			WRITE16(addr+ofs2, 1024-v[3]);
		}
		else {
			printf("usage: cm r/g/b ofs1 ofs2\n set r/g/b coeff to 0x40+ofs1+ofs2, -ofs1, ofs2\n");
		}
	}
/* display */
	else if (strcmp(*argv, "colorshow")==0) {
		/*s Modify by Aries 10/02/03*/

		UINT32 value=0;
		value =v[1]<<16|v[2]<<8|v[3];

		printf("colorshow A= v[1]:0x%x v[2]:0x%x v[3]:0x%x\n",v[1],v[2],v[3]);
		printf("colorshow B= value 0x%x\n",value);

		/*sp5kModeSet(0x88); *//*Set to standby Mode*/

		appOsdLib_OsdLayerClearDraw();

		#if 1
		sp5kGfxAttrSet(0x05,v[1], 0, 0, 0);	/*SP5K_GFX_FILL_COLOR*/
		sp5kGfxAttrSet(0x02,v[1], 0, 0, 0);	/*SP5K_GFX_PEN_COLOR*/
		#else
		sp5kGfxAttrSet(0x05,RGB888_TO_RGB565(value), 0, 0, 0);	/*SP5K_GFX_FILL_COLOR*/
		sp5kGfxAttrSet(0x02,RGB888_TO_RGB565(value), 0, 0, 0);	/*SP5K_GFX_PEN_COLOR*/
		#endif

		sp5kGfxRectDraw(8,0,0,320,240);
		/*e Modify by Aries 10/02/03*/
	}
	else if (strcmp(*argv, "dispclk")==0) {
	    #if SPCA5330
		halPllDispPanelClkSet(v[1], v[2]);

		#else
		halPllDispClkSet(v[1], v[2]);
		#endif
		printf("disp clk = %d KHz\n", v[2]);
	}
	else if (strcmp(*argv, "expset")==0) {
		UINT32 exp;
		UINT32 agc;
		if (v[1]==1) {
			sp5kPreviewExpAgcGet(&exp,&agc);
			appTimeDelayMs(5);
			sp5kPreviewExpAgcSet(v[2],agc);
			printf("pvExpAgcSet:exp:%d,agc:%d\n",v[2] ,agc);
		}
		else {
			sp5kStillCapExpAgcGet(&exp,&agc);
			appTimeDelayMs(5);
			sp5kStillCapExpAgcSet(v[2],agc);
			printf("capExpAgcSet:exp:%d,agc:%d\n",v[2] ,agc);

        }
    }
    else if (strcmp(*argv, "showiso")==0) {
     	/*s Mask by Aries 10/03/08*/
    	#if 0
        UINT32 exp;
        UINT32 agc;
        SINT32 isoIdx;
        SINT32 svIdx;


        sp5kPreviewExpAgcGet(&exp,&agc);
        appTimeDelayMs(5);
        svIdx = pv_agcidx_to_svidx(agc);
        isoIdx = svidx_to_aeIso_tblidx(svIdx);

        printf("PV agc:%d, isoIdx:%d\n",(agc) ,isoIdx);

        sp5kStillCapExpAgcGet(&exp,&agc);
        appTimeDelayMs(5);
        svIdx = pv_agcidx_to_svidx(agc);
        isoIdx = svidx_to_aeIso_tblidx(svIdx);
        printf("Capture agc:%d, isoIdx:%d\n",(agc) ,isoIdx);
	    #endif
	    /*e Mask by Aries 10/03/08*/
    }
    else if (strcmp(*argv, "iqmode")==0) {
      #if 0
    	#if SPCA5310
        if (argc > 1) {
            switch (*argv[1]) {
                case 'n':
                    printf("IQ Normal mode\n");

                    gIQ_mode = IQ_NORMAL_MODE;

                    break;
                case 't':
                    printf("IQ Test mode\n");

                    gIQ_mode = IQ_TEST_MODE;


                    break;
            }
            if (argc > 2)
                gIQ_blkidx = strtoul(argv[2], 0, 0);
            else
                gIQ_blkidx = 0;

        } else {
            printf("-------------------------------------\n");
            printf("iqmode n <idx>: normal mode with block idx\n");
            printf("iqmode t <idx>: test mode with block idx\n");
            printf("-------------------------------------\n");
        }
	#endif
	#endif
    }
    else if (strcmp(*argv, "dumpgtbl")==0) {
     	/*s Modify by Aries 10/03/08*/
    	#if SPCA5310
    	extern UINT32 halCdspDumpSram( UINT32 id );
        halCdspDumpSram(3);
        halCdspDumpSram(4);
        halCdspDumpSram(7);
		#endif
    }
    else if (strcmp(*argv, "stateget")==0) {
       printf("State Get\n");
	appStateCtrlInfoPrint("_stateInitial - Get");
    }
    else if (strcmp(*argv, "expagcset")==0) {
        UINT32 exp;
        UINT32 agc;
        if (v[1]==1) {
            sp5kPreviewExpAgcGet(&exp,&agc);
            appTimeDelayMs(5);
            sp5kPreviewExpAgcSet(v[2],v[3]);
            printf("pvExpAgcSet:exp:%d,agc:%d\n",v[2],v[3]);
        }
        else {
            sp5kStillCapExpAgcGet(&exp,&agc);
            appTimeDelayMs(5);
            sp5kStillCapExpAgcSet(v[2],v[3]);
            printf("capExpAgcSet:exp:%d,agc:%d\n",v[2],v[3]);
           /* printf("sp5kModeSet:0x%x\n",SP5K_MODE_STILL_SNAP);
			 appTimeDelayMs(50);
        	sp5kModeSet(SP5K_MODE_STILL_SNAP); */
		}
	}
	else if (strcmp(*argv, "agcset")==0) {
        UINT32 exp;
        UINT32 agc;
        if (v[1]==1) {
            sp5kPreviewExpAgcGet(&exp,&agc);
            appTimeDelayMs(5);
            sp5kPreviewExpAgcSet(exp,v[2]);
            printf("pvExpAgcSet:exp:%d,agc:%d\n",exp ,v[2]);
        }
        else {
            sp5kStillCapExpAgcGet(&exp,&agc);
            appTimeDelayMs(5);
            sp5kStillCapExpAgcSet(exp,v[2]);
            printf("capExpAgcSet:exp:%d,agc:%d\n",exp ,v[2]);

		}
	}
	else if (strcmp(*argv, "expagcget")==0) {
		UINT32 exp;
		UINT32 agc;
		if (v[1]==1) {
			sp5kPreviewExpAgcGet(&exp,&agc);
			printf("pvExpAgcGet:exp:%d,agc:%d\n",exp ,agc);
		}
		else {
			sp5kStillCapExpAgcGet(&exp,&agc);
			printf("capExpAgcGet:exp:%d,agc:%d\n",exp ,agc);
        }
	}
	else if (strcmp(*argv, "autoev") ==0) {

		appAePvAutoExpTblSet(v[1], v[2], v[3]);

	}
	else if (strcmp(*argv, "sev") ==0) {

		appAePvAutoExpTblSingleSet(v[1]);

	}
	else if (strcmp(*argv, "gev") ==0) {
		appAePvAutoExpTblCurrIdxGet();
	}
	else if (strcmp(*argv, "swapev") ==0) {

		appAePvAutoExpTblSwapSet(v[1], v[2], v[3]);

    }
    else if (strcmp(*argv, "rawiso")==0) {
        UINT32 agc, exp;
        switch(v[1]) {
        case 50:
            agc = cap_svidx_to_agcidx(STD_ISO_50);
            break;
        case 100:
            agc = cap_svidx_to_agcidx(STD_ISO_100);
            break;
        case 200:
            agc = cap_svidx_to_agcidx(STD_ISO_200);
            break;
        case 400:
            agc = cap_svidx_to_agcidx(STD_ISO_400);
            break;
        case 800:
            agc = cap_svidx_to_agcidx(STD_ISO_800);
            break;
        case 1600:
            agc = cap_svidx_to_agcidx(STD_ISO_1600);
            break;
        case 3200:
            agc = cap_svidx_to_agcidx(STD_ISO_1600);
            break;
        case 6400:
            agc = cap_svidx_to_agcidx(STD_ISO_1600);
            break;
        default:
            agc = cap_svidx_to_agcidx(STD_ISO_1600);
            break;
        };
        #if SPCA5310
        sp5kImgModeSet(SP5K_MODE_STILL_PREVIEW);
        #endif
        sp5kImgCdspBlockEnable(SP5K_CDSP_OB, 1);
        #if SPCA5310
        sp5kImgModeSetDone(SP5K_MODE_STILL_PREVIEW);
        #endif
        #if SPCA5310
        sp5kImgModeSet(SP5K_MODE_STILL_SNAP);
        #endif
        sp5kImgCdspBlockEnable(SP5K_CDSP_OB, 1);
        #if SPCA5310
        sp5kImgModeSetDone(SP5K_MODE_STILL_SNAP);
        #endif

        printf("AE/AEB Off\n");
        sp5kAeModeSet(0);
        sp5kAwbModeSet(0);

        exp = STD_EXP_1_32;
        appTimeDelayMs(5);
        sp5kPreviewExpAgcSet(exp,agc);
        printf("pvExpAgcSet:exp:%d,agc:%d\n",exp ,agc);

        appTimeDelayMs(5);
        sp5kStillCapExpAgcSet(exp,agc);
        printf("capExpAgcSet:exp:%d,agc:%d\n",exp ,agc);

        printf("%s raw capture\n", "Enable");
        sp5kStillCapAdvCfgSet(SP5K_CAPTURE_SAVE_RAW, 1);
        printf("sp5kModeSet:0x%x\n",0x13);
        sp5kModeSet(0x13);

        printf("AE/AWB On\n");
        sp5kAeModeSet(2);
        sp5kAwbModeSet(1);
    }
    else if (strcmp(*argv, "framerate")==0) {
		if (frm_time_count>0) {
			printf("Under Calculating return....");
		} else {
			if(v[1]>0)
				frm_time_count_max=v[1];
			printf("\nstart calculate framerate....Total= %d frames \n",frm_time_count_max);
			printf("Plase Wait ...\n");
			FrmRateCalProc();
		}
    }
    else if (strcmp(*argv, "aeoff")==0) {
        sp5kAeModeSet(0);
        sp5kAwbModeSet(0);
        printf("AE/AEB Off\n");
    }
    else if (strcmp(*argv, "aeon")==0) {
        sp5kAeModeSet(2);
        sp5kAwbModeSet(2);
        printf("AE/AWB On\n");
    }
    else if (strcmp(*argv, "speedy")==0) {
		appStillSpeedyCapture(60);
		printf("speedy capture start \n");
    }
    else if (strcmp(*argv, "swhdmi")==0) {
			switchHdmi();
			printf("swhdmi\n");
    }
    else if (strcmp(*argv, "swlcd")==0) {
		sp5kDispDevSwitch(SP5K_DISP_DEV_LCD);
		printf("swlcd\n");
    }
    else if(strcmp(*argv, "swlcm")==0){
		extern void dispLcmInit(void);
		dispLcmInit();
		printf("swlcm\n");
    }
    else if (strcmp(*argv, "swshow")==0) {
		showJPG();
		printf("swshow\n");
	}
	else if (strcmp(*argv, "swosd")==0) {
		showOSD();
		printf("Show OSD\n");
		/*e Add by Aries 10/05/18*/
    }
    else if (strcmp(*argv, "usboff")==0) {
        sp5kUsbSwCon(0);
		#if 0 /* mantis 45742 */
		sp5kGpioWrite(3, GPIO_MASK(13-32), (UINT32)((!!1) ? GPIO_MASK(13-32) : 0));
		appBtnEnable(((((UINT32)0xF)<<28) + ((UINT32)0x0FFFFFFF)));
		#endif
        printf("usboff\n");
    }else if (strcmp(*argv, "praw")==0){

     	printf("cali View Raw\n");
		sp5kCaliPreviewRawCallbackSet(appCalibRawGet);
		sp5kCalibrationCfgSet(SP5K_CALIBRATION_CALLBACK, SP5K_CALIB_CB_PREVIEW_RAW);
		sp5kTimeDelay(SP5K_TIME_DELAY_1MS,	500);
		sp5kCalibrationCfgSet(SP5K_CALIBRATION_OFF, SP5K_CALIB_CB_PREVIEW_RAW);
		sp5kTimeDelay(SP5K_TIME_DELAY_1MS,	200);
	 }else if (strcmp(*argv, "capraw")==0){

     	printf("cali View Raw\n");
		sp5kStillCapRawCallbackSet(appCalibRawGet);
		sp5kCalibrationCfgSet(SP5K_CALIBRATION_CALLBACK, SP5K_CALIB_CB_STILL_RAW);
		sp5kTimeDelay(SP5K_TIME_DELAY_1MS,	500);
		sp5kCalibrationCfgSet(SP5K_CALIBRATION_OFF, SP5K_CALIB_CB_STILL_RAW);
		sp5kTimeDelay(SP5K_TIME_DELAY_1MS,	200);
	 }
    else if (strcmp(*argv, "aetest")==0) {
        appAePreviewShutBoundarySet(0, 14/*SHUTTER_1_2000S*/, 7/*SHUTTER_1_15S*/);
        printf("aetest Tesy\n");
    }
    else if (strcmp(*argv, "usbon")==0) {
        if ((v[1]>0)&&(v[1]<=4)) {
            sp5kUsbSwCon(0);
            sp5kUsbModeSet(v[1]);
            if (v[1] ==1) {
				#if 0 /* mantis 45742 */
				sp5kGpioWrite(3, GPIO_MASK(13-32), (UINT32)((!!0) ? GPIO_MASK(13-32) : 0));
				appBtnDisable(((((UINT32)0xF)<<28) + ((UINT32)0x0FFFFFFF)));
				#endif
				sp5kResourceFileLoad(SP5K_RES_USB_CONF_BIN, RES_USB_USBUVC);
                printf("usbon PCCAM\n");
            }
            else if (v[1] ==2) {
                printf("usbon MSDC\n");
				sp5kUsbLunSet(1, 0);
				sp5kUsbLunSet(2, 1);
            }else if (v[1] ==3){
                printf("usbon SIDC\n");
				#if 0 /*ICAT_WIFI  */
				extern void app_ptp_statset(BOOL en);
				app_ptp_statset(1);

                sp5kUsbPtpIpClose();
				#endif
            }
            else {
                printf("usbon APITEST\n");
            }
            sp5kUsbSwCon(1);
        }else{
            printf("usbon Fail\n");
		}
	}
	else if (strcmp(*argv, "telewide")==0) {
		UINT32 ulcurmode = 0;
		#define DZOOM_SEG 0
		static UINT32 gdzoomRatioArray[] =
		{
			1000+DZOOM_SEG,
			1022,
			1044+DZOOM_SEG,
			1067,
			1091+DZOOM_SEG,
			1114,
			1139+DZOOM_SEG,
			1164,
			1189+DZOOM_SEG,
			1215,
			1242+DZOOM_SEG,
			1269,
			1297+DZOOM_SEG,
			1325,
			1354+DZOOM_SEG,
			1384,
			1414+DZOOM_SEG,
			1445,
			1477+DZOOM_SEG,
			1509,
			1542+DZOOM_SEG,
			1576,
			1610+DZOOM_SEG,
			1646,
			1682+DZOOM_SEG,
			1719,
			1756+DZOOM_SEG,
			1795,
			1834+DZOOM_SEG,
			1874,
			1915+DZOOM_SEG,
			1957,
			2000+DZOOM_SEG,
			2044,
			2089+DZOOM_SEG,
			2134,
			2181+DZOOM_SEG,
			2229,
			2278+DZOOM_SEG,
			2327,
			2378+DZOOM_SEG,
			2430,
			2484+DZOOM_SEG,
			2538,
			2594+DZOOM_SEG,
			2650,
			2709+DZOOM_SEG,
			2768,
			2828+DZOOM_SEG,
			2890,
			2954+DZOOM_SEG,
			3018,
			3084+DZOOM_SEG,
			3152,
			3221+DZOOM_SEG,
			3292,
			3364+DZOOM_SEG,
			3437,
			3513+DZOOM_SEG,
			3589,
			3668+DZOOM_SEG,
			3748,
			3830+DZOOM_SEG,
			3914,
			4000+DZOOM_SEG,
		};
		sp5kModeGet(&ulcurmode);

		for (i=0;i <64 ;i++){
			sp5kTimeDelay(SP5K_TIME_DELAY_1MS ,10);
			if (v[1] ==1) {
				if (gdzoomRatioArray[i] == 2000) {
					printf("sp5kMode: %x, Param: %x, i: %x\n", ulcurmode, v[1], i);

					sp5kSensorModeCfgSet(ulcurmode, 0x31);
					sp5kModeSet(ulcurmode);
					sp5kModeWait(ulcurmode);
				}
				sp5kZoomFactorSet(ulcurmode, gdzoomRatioArray[i]);
			}
			else if (v[1] ==2) {
				if (gdzoomRatioArray[i] == 2000) {
					printf("sp5kMode: %x, Param: %x, i: %x\n", ulcurmode, v[1], i);

					sp5kSensorModeCfgSet(ulcurmode, 0x30);
					sp5kModeSet(ulcurmode);
					sp5kModeWait(ulcurmode);
				}
				sp5kZoomFactorSet(ulcurmode, gdzoomRatioArray[63 - i]);
			}
			else
				printf("Param Err!\n");
		}
	}
	else if (strcmp(*argv, "modeget")==0) {
		UINT32 mode ;
		sp5kModeGet(&mode);
		printf("sp5kModeGet:0x%x\n",mode);
	}
	else if (strcmp(*argv, "smode")==0) {
		if ((v[1]&0x30)==0x30)
			sp5kSensorModeCfgSet(SP5K_MODE_STILL_PREVIEW, v[1]);
		else
			sp5kSensorModeCfgSet(SP5K_MODE_STILL_SNAP, v[1]);

		sp5kModeCfgSet(SP5K_MODE_CFG_PV_DISP_FIT, 1);
		sp5kModeSet(SP5K_MODE_STILL_PREVIEW);
		sp5kModeWait(SP5K_MODE_STILL_PREVIEW);
		printf("[modesw] stillpv -> stillpv 0x%x\n",v[1]);
	}
	else if (strcmp(*argv, "mode")==0) {
		#if 0
		if(argc == 1)
		{
			UINT32 mode ;
			sp5kModeGet(&mode);
			printf("Current Mode:0x%x\n",mode);
		}
		else
		{
			printf("Set Mode:0x%x\n",v[1]);
			sp5kModeSet(v[1]);
			sp5kModeWait(v[1]);
		}
		#else
		UINT32 mode =0x11;
		if (v[1] ==1) {
			mode = 0x11 ;   /*SP5K_MODE_STILL_PREVIEW*/
		}
		else if (v[1] ==2) {
			/*sp5kStillCapAdvCfgSet( SP5K_CAPTURE_DARK_FRAME, 1 );*/
			mode = 0x13 ;   /*SP5K_MODE_STILL_SNAP*/
			/*s Add by Aries 10/03/02*/
			appStillSnapIqSet();
			/*e Add by Aries 10/03/02*/
		}
		else if (v[1] ==3) {
			mode = 0x41 ;   /*SP5K_MODE_MULTI_PLAYBACK*/
		}
		else if (v[1] ==4) {
			mode = 0x42 ;   /*SP5K_MODE_STILL_PLAYBACK*/
		}
		printf("sp5kModeSet:0x%x\n",mode);
		_stillUtil_ShutterDelaySet();

		sp5kModeSet(mode);
		#endif
	}
	else if (strcmp(*argv, "sregread")==0) {
		UINT8 regRead[2];
		regRead[0]=v[1];

		sensorSerialBlockRead(0,1,regRead,0);
		appTimeDelayMs(30);
		printf("ReadReg:V[1]:0x%x\n",v[1]);
		printf("Data:0x%x\n",regRead[1]);
	}
	else if (strcmp(*argv, "sregwrite")==0) {
		UINT8 regWrite[2];
		regWrite[0]=v[1];
		regWrite[1]=v[2];

		extern UINT32 sensorSerialBulkWrite(UINT32 cmdportid,UINT32 nreg,const UINT8 *pdata,UINT32 sync);
		sensorSerialBulkWrite(0,1,regWrite,0);
		printf("WriteReg:V[1]:0x%x,V[2]:0x%x\n",v[1],v[2]);
	}
	else if (strcmp(*argv, "key")==0) {
		UINT32 msg;
		UINT32 param = 0;
		if ((v[1]==0x01)||(v[1]==0x02)||(v[1]==0x03)||(v[1]==0x04)||(v[1]==0x0D)||(v[1]==0x10)||(v[1]==0x11)){
			/*********************************
			TELE  	-->0x01
			WIDE 	-->0x02
			S1 		-->0x03
			S2 		-->0x04
			PB 		-->0x0D
			OK		-->0x10
			POWER	-->0x11
			*********************************/
			if (v[1] ==0x01){
				param =0x00;
			}else if (v[1] ==0x02){
				param =0x01;
			}else if (v[1] ==0x03){
				param =0x02;
			}else if (v[1] ==0x04){
				param =0x03;
			}else if (v[1] ==0x0D){
				param =0x05;
			}else if (v[1] ==0x10){
				param =0x06;
			}else if (v[1] ==0x11){
				param =0x07;
			}
			msg=0x0100;
		}else{
			/*********************************
			UP  			-->0x05
			DOWN 		-->0x06
			LEFT 		-->0x07
			RIGHT 		-->0x08
			MENU 		-->0x09
			DEL 			-->0x0A
			SET 			-->0x0B
			MODE 		-->0x0C
			*********************************/
			if (v[1] ==0x05){
				param =0x00;
			}else if (v[1] ==0x06){
				param =0x01;
			}else if (v[1] ==0x07){
				param =0x02;
			}else if (v[1] ==0x08){
				param =0x03;
			}else if (v[1] ==0x0B){
				param =0x04;
			}else if (v[1] ==0x0A){
				param =0x05;
			}else if (v[1] ==0x09){
				param =0x06;
			}else if (v[1] ==0x0C){
				param =0x07;
			}
			msg=0x0103;
		}

		extern UINT32 appBtnMsgToKey(UINT32 msgId,UINT32 msgParam);
		sp5kHostMsgSend(appBtnMsgToKey(msg, param),0);
		printf("key:0x%x\n",v[1]);
		}else if (strcmp(*argv, "spi")==0) {
	}
	else if (strcmp(*argv, "spir")==0) {
	}
	else if (strcmp(*argv, "host")==0) {
		appCmdVirtualKB(1, NULL);
	}
	else if (strcmp(*argv, "aetestcmd")==0) {
		AeTestCmdHandle();
	}
	else if (strcmp(*argv, "buzzer")==0) {
		if (v[1]==1){
			AppBuzzerOn(v[2],v[3]);
			printf("Buzzer Duty:%d\n",v[2]);
		}else{
			AppBuzzerOff();
			printf("Buzzer Off\n");
		}
	}
	else if (strcmp(*argv, "alpha")==0)
	{
		sp5kGfxPageClear(SP5K_GFX_PAGE_OSD_0,0);
		sp5kGfxInitCfgSet(SP5K_GFX_OSD_FMT, SP5K_GFX_FMT_IDX8, 0, 0, 0);
		sp5kGfxInitCfgSet(SP5K_GFX_OSD_WITH_ALPHA_PLANE, 1, 0, 0, 0); /* En 0: disable, En 1:Enable for param 2*/
		sp5kGfxInit();

		appOsd_FontIconFontSet(ID_FONT_ICON);
		sp5kGfxAttrSet(SP5K_GFX_BLEND_FACTOR, 64, 0, 0, 0);
		sp5kGfxIconDraw(SP5K_GFX_PAGE_OSD_0,180, 80,SP5K_GFX_ALIGN_CENTER, 0x0006);
		sp5kGfxAttrSet(SP5K_GFX_BLEND_FACTOR, 128, 0, 0, 0);
	}
	else if (strcmp(*argv, "frame")==0) {
	}
	else if (strcmp(*argv, "shutter")==0) {
	}
	else if (strcmp(*argv, "ad")==0) {
		UINT32 i,ch,val[8],min[8],max[8],avg[8],cnt[8];
		if (v[1]<=0) v[1] = 100;
		memset(min, 0xff, sizeof(min));
		memset(max, 0, sizeof(max));
		memset(avg, 0, sizeof(avg));
		memset(cnt, 0, sizeof(cnt));
		for (i=0 ; i<v[1] ; i++) {
			for (ch=0 ; ch<8 ; ch++) {
				if (sp5kAdcRead(ch, &val[ch])==SUCCESS) {
				if (min[ch]>val[ch]) min[ch] = val[ch];
				if (max[ch]<val[ch]) max[ch] = val[ch];
				avg[ch] += val[ch];
					cnt[ch] ++;
				}
			}
			if (v[1]!=100) printf("%4d %4d %4d %4d %4d %4d %4d %4d\n", val[0], val[1], val[2], val[3],val[4],val[5],val[6],val[7]);
		}
		if (v[1]==100) {
			printf("ch times  avg (8b)  min  max\n");
			for (ch=0 ; ch<8 ; ch++) {
				avg[ch] /= cnt[ch];
				printf("%2d %5d %4d(%3d) %4d %4d\n", ch, cnt[ch], avg[ch], avg[ch]/4, min[ch], max[ch]);
			}
		}
	}
	else if(strcmp(*argv, "raw") ==0 ) {
		sp5kStillCapAdvCfgSet(SP5K_CAPTURE_SAVE_RAW, v[1]?1:0);
		printf("%s raw capture\n", v[1]?"Enable":"Disable");
	}
	else if(strcmp(*argv, "dispflip")==0){
		 sp5kDispDevAttrSet(SP5K_DISP_IMG_FLIP,v[1], v[2],v[3], v[4]);

	}
	else if(strcmp(*argv, "zt") ==0 ) {
		#define DZOOM_SEG 0
		static UINT32 gdzoomRatioArray[] =
		{
			1000+DZOOM_SEG,
			1022,
			1044+DZOOM_SEG,
			1067,
			1091+DZOOM_SEG,
			1114,
			1139+DZOOM_SEG,
			1164,
			1189+DZOOM_SEG,
			1215,
			1242+DZOOM_SEG,
			1269,
			1297+DZOOM_SEG,
			1325,
			1354+DZOOM_SEG,
			1384,
			1414+DZOOM_SEG,
			1445,
			1477+DZOOM_SEG,
			1509,
			1542+DZOOM_SEG,
			1576,
			1610+DZOOM_SEG,
			1646,
			1682+DZOOM_SEG,
			1719,
			1756+DZOOM_SEG,
			1795,
			1834+DZOOM_SEG,
			1874,
			1915+DZOOM_SEG,
			1957,
			2000+DZOOM_SEG,
			2044,
			2089+DZOOM_SEG,
			2134,
			2181+DZOOM_SEG,
			2229,
			2278+DZOOM_SEG,
			2327,
			2378+DZOOM_SEG,
			2430,
			2484+DZOOM_SEG,
			2538,
			2594+DZOOM_SEG,
			2650,
			2709+DZOOM_SEG,
			2768,
			2828+DZOOM_SEG,
			2890,
			2954+DZOOM_SEG,
			3018,
			3084+DZOOM_SEG,
			3152,
			3221+DZOOM_SEG,
			3292,
			3364+DZOOM_SEG,
			3437,
			3513+DZOOM_SEG,
			3589,
			3668+DZOOM_SEG,
			3748,
			3830+DZOOM_SEG,
			3914,
			4000+DZOOM_SEG,
		};
		if (v[1] ==2)
			sp5kModeSet(SP5K_MODE_VIDEO_PREVIEW);
		for (i=0;i <64 ;i++) {
			sp5kTimeDelay(SP5K_TIME_DELAY_1MS ,10);
			if(v[1] ==1)
				sp5kZoomFactorSet(SP5K_MODE_STILL_PREVIEW, gdzoomRatioArray[i]);
			else if(v[1] ==2)
				sp5kZoomFactorSet(SP5K_MODE_VIDEO_PREVIEW, gdzoomRatioArray[i]);
		}
	}
	else if(strcmp(*argv, "zz") ==0 ) {
		if(v[1] ==1){
			sp5kZoomFactorSet(SP5K_MODE_STILL_PREVIEW, 2502);
		/*	sp5kZoomFactorSet(SP5K_MODE_STILL_SNAP, 2502);  */

		}
		else if(v[1] ==2){
			sp5kZoomFactorSet(SP5K_MODE_STILL_PREVIEW, 2580);
		/*	sp5kZoomFactorSet(SP5K_MODE_STILL_SNAP, 2580);  */
		}
		else if(v[1] ==3)
			sp5kZoomFactorSet(SP5K_MODE_STILL_PREVIEW, 2580);
		else if(v[1] ==4)
			sp5kZoomFactorSet(SP5K_MODE_STILL_PREVIEW, 2600);
		else if(v[1] ==5) {
			sp5kZoomFactorSet(SP5K_MODE_STILL_PREVIEW, 2726);
		/*	sp5kZoomFactorSet(SP5K_MODE_STILL_SNAP, 2726);  */
		}
		else if(v[1] ==6) {
			sp5kZoomFactorSet(SP5K_MODE_STILL_PREVIEW, 3800);
         /*   sp5kZoomFactorSet(SP5K_MODE_STILL_SNAP, 3800);  */
			}
		/*sp5kModeSet(SP5K_MODE_STILL_SNAP); */
	}
	else if(strcmp(*argv, "mp3play") ==0 ) {
		/*sp5kDcfAttrElm_t currDcfFileAttr;*/
		UINT16 currFileIdx;
		/*UINT16 totalFileCount;*/
		UINT8 filename[30];

		memset(filename, 0, 30);
		currFileIdx=1;
		sp5kModeSet(0x88);
		appTimeDelayMs(300);
		#if 0
		sp5kDcfFsFileTypeSet(0x0001|0x0004|0x0010);
		sp5kDcfFsInfoGet(SP5K_DCF_INFO_TOTAL_FILENUM,&totalFileCount);
		printf("Total file is (%d))!!!\n",totalFileCount);

		sp5kDcfFsCurFileIdxSet(currFileIdx);
		sp5kDcfFsFileAttrGet(currFileIdx,&currDcfFileAttr,filename);
		printf("Current file is (%s,0x%x))!!!\n",filename,currDcfFileAttr.fileType);
		#endif

		sp5kMediaPlayFileNameSet("D:\\DCIM\\100MEDIA\\愛很簡單.MP3");
		sp5kMediaPlayAttrParse();
		sp5kMediaPlayControl( 0, 0, 0 );

		printf("MP3 Play\n");
	}
	else if (strcmp(*argv, "mp3test") ==0 ) {
		if (v[1]==1) {
			sp5kMediaPlayControl( 2, 0, 0 ); /*SP5K_MEDIA_PLAY_PAUSE*/
		}else if (v[1]==2){
			sp5kMediaPlayControl( 0, 0, 0 ); /*SP5K_MEDIA_PLAY_START*/
		}else if (v[1]==3){
    			sp5kMediaPlayControl( 5, 4, 0 ); /*SP5K_MEDIA_PLAY_FORWARD*/
		}else if (v[1]==4){
    			sp5kMediaPlayControl( 6, 4, 0 ); /*SP5K_MEDIA_PLAY_REWIND*/
		}else if (v[1]==5){
    			sp5kMediaPlayControl( 3, 0, 0 ); /*SP5K_MEDIA_PLAY_RESUME*/
		}
	}
#if SP5K_SENSOR_TG_AD9000_SONY675
	else if (strcmp(*argv, "rawsony")==0) {
		sp5kStillCapRawCallbackSetSony3div(v[1]?rawSonySave:NULL);
	}
#endif
	else if (strcmp(*argv, "pvraw")==0) {
		sp5kCalibrationCfgSet(SP5K_CALIBRATION_SAVE_PREVIEW_RAW, 0);
	}
	else if (strcmp(*argv, "snapstate")==0) {
		extern void snapDbgAllState(void);
		snapDbgAllState();
	}
	else if (strcmp(*argv, "pbstate")==0) {
		extern void pbDbgAllState(void);
		pbDbgAllState();
	}
	else if(!strcmp(*argv, "senr")){
		static UINT8 regread[3];
		regread[0]=v[1]>>8;
		regread[1]=v[1]&0xFF;
		sensorSerialBlockRead(0,1,regread,0);
		tmrUsWait(100000);/*100ms*/
		printf("READ [0x%x] = [0x%x]\n",v[1],regread[2]);
	}
	else if(!strcmp(*argv, "senw")){
		static UINT8 regdata[3];
		regdata[0]=v[1]>>8;
		regdata[1]=v[1]&0xFF;
		regdata[2]=v[2]&0xFF;
		sensorSerialBulkWrite(0,1,regdata,0);
		tmrUsWait(100000);/*100ms*/
		printf("WRITE [0x%x] = [0x%x]\n",v[1],v[2]);
	}
	else if(!strcmp(*argv, "msenr")){
		static UINT8 regread[4];
		regread[0]=v[1]>>8;
		regread[1]=v[1]&0xFF;
		sensorSerialBlockRead(0,1,regread,0);
		tmrUsWait(100000);/*100ms*/
		printf("Aptina READ [0x%x] = [0x%x%02x]\n",v[1],regread[2],regread[3]);
	}
	else if(!strcmp(*argv, "msenw")){
		static UINT8 regdata[4];
		regdata[0]=v[1]>>8;
		regdata[1]=v[1]&0xFF;
		regdata[2]=v[2]>>8;
		regdata[3]=v[2]&0xFF;
		sensorSerialBulkWrite(0,1,regdata,0);
		tmrUsWait(100000);/*100ms*/
		printf("Aptina WRITE [0x%x] = [0x%x]\n",v[1],v[2]);
	}
	else if(!strcmp(*argv, "psenw")){
		static UINT8 regdata[4];
		regdata[0]=v[1]&0xFF;
		regdata[1]=v[1]>>8;
		regdata[2]=v[2]&0xFF;
		regdata[3]=v[2]>>8;
		sensorSerialBulkWrite(0,1,regdata,0);
		tmrUsWait(100000);/*100ms*/
		printf("Pana 14M CMOS WRITE-[0x%x] = [0x%x]\n",v[1],v[2]);
	}
	else if(!strcmp(*argv, "ssenw")){
		static UINT8 regdata[4];
		regdata[0]=0x81;
		regdata[1]=0x00;
		regdata[2]=v[1]&0xFF;
		regdata[3]=v[2]&0xFF;
		sensorSerialBulkWrite(0,1,regdata,0);
		tmrUsWait(100000);/*100ms*/
		printf("SONY 16M CMOS WRITE-[0x%x] = [0x%x]\n",v[1],v[2]);
	}
	else if (strcmp(*argv, "prof")==0) {
		static UINT32 profId=0;
		if (argc==1) {
			sioCtrl(0); /* default output */
			printf("usage:\n"
					" prof n: dump prof slot n\n"
					" prof id n: set prof slot n\n"
					" prof len bufsize: set prof size (0 to free)\n");
			profLogDump(profId);
		}
		else {
			if (strcmp(argv[1], "id")==0) {
				if (v[2]<4) {
					profId = v[2];
					printf("set prof(%d)\n", profId);
				}
			}
			else if (strcmp(argv[1], "len")==0) {
				if (v[2] && v[2]<4096) v[2] = 4096;
				if (v[2]) {
					printf("set prof(%d) len=%d\n", profId, v[2]);
					profLogInit(profId, v[2]);
				}
				else {
					printf("free prof(%d)\n", profId);
					profLogTerm(profId);
				}
			}
			else {
				if (v[1]<4) {
					sioCtrl(0); /* default output */
					profLogDump(v[1]); /* dump will clear buf */
				}
			}
		}
	}
	else if (strcmp(*argv, "codesum")==0) {
		codeSum();
	}
	else if (strcmp(*argv, "sio")==0) {
		cmdSioFilter(argv, v);
	}
	else if (strcmp(*argv, "type")==0) {
		if (argc>=2) {
			UINT8 *buf;
			UINT32 fd,len;
			#define DUMP_LEN	4096
			buf = sp5kMalloc(DUMP_LEN);
			if (buf) {
				fd = sp5kFsFileOpen(argv[1], SP5K_FS_OPEN_RDONLY);
				printf("dump %s(%x)\n", argv[1], fd);
				if (fd) {
					len = sp5kFsFileSizeGet(fd);
					if (v[2]<len) {
							len = v[2]+DUMP_LEN<len ? DUMP_LEN : len-v[2];
						if (v[2])
							sp5kFsFileSeek(fd, v[2], SEEK_SET);
						sp5kFsFileRead(fd, buf, len);
						memdump(buf, len);
					}
					sp5kFsFileClose(fd);
				}
				sp5kFree(buf);
			}
		}
	}
	else if (strcmp(*argv, "fcopy")==0) {
		if (argc==3) appFileCopy(argv[1], argv[2]);
	}
	else if (strcmp(*argv, "qtbl")==0) {
		UINT8 qtbl[0x80];
		if (v[1]==0) v[1] = 100;
		jpeg_add_quant_table( v[1], qtbl );
		printf("Q=%d\n", v[1]);
		memdump(qtbl, 0x80);
	}
	else if(strcmp(*argv, "icon")==0)
	{
		appOsd_FontIconFontSet("UI\\ICON\\ICON_32.SFN");
		appOsdLib_OsdLayIconDraw(v[1],v[2],0x11,0x4c);
	}
	else if (strcmp(*argv, "videoon")==0) {
		sp5kMediaRecCfgSet(SP5K_MEDIA_REC_DIS_LEVEL,0x02);
		sp5kModeSet(SP5K_MODE_VIDEO_PREVIEW);
		sp5kModeWait(SP5K_MODE_VIDEO_PREVIEW);

		printf("....SP5K_MODE_VIDEO_PREVIEW\n");
	}
	else if (strcmp(*argv, "videooff")==0) {
		sp5kModeSet(SP5K_MODE_STANDBY);
		sp5kModeWait(SP5K_MODE_STANDBY);
		sp5kMediaRecCfgSet(SP5K_MEDIA_REC_DIS_LEVEL,0x00);
		printf("....SP5K_MODE_STANDBY\n");
	}
	else if (strcmp(*argv, "view")==0) {
		sp5kModeSet(SP5K_MODE_STILL_PREVIEW);
		sp5kModeWait(SP5K_MODE_STILL_PREVIEW);
		printf("....SP5K_MODE_STILL_PREVIEW");
	}
	else if (strcmp(*argv, "view")==0) {
		sp5kModeSet(SP5K_MODE_VIDEO_PREVIEW);
	}
	else if(strcmp(*argv, "cardvr")==0) {
#if 0
		if (v[1]==0) v[1] = 60;
		sp5kMediaRecAttrSet(MEDIA_ATTR_SEAMLESS_TIME_SLOT, v[1]); /* Second */
		sp5kMediaRecCfgSet(SP5K_MEDIA_REC_SEAMLESS_CALLBACK, (UINT32)AppVideoSeamlessCB);
#endif
		UINT32 i,ms ;
		/*refer to video record sample for other video record configuration */
		sp5kMediaRecAttrSet(MEDIA_ATTR_SEAMLESS_TIME_SLOT,30 ); /* Second */
		sp5kMediaRecCfgSet(SP5K_MEDIA_REC_SEAMLESS_CALLBACK, (UINT32)AppVideoSeamlessCB);
		AppVideoSeamlessCB();
		sp5kModeSet(SP5K_MODE_VIDEO_RECORD);
		sp5kModeWait(SP5K_MODE_VIDEO_RECORD);

		for (i = 0; i < 30000; i++)
		{
		sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 500);
		/* get current recording time in msec */
		sp5kMediaRecAttrGet(SP5K_MEDIA_ATTR_ELAPSED_TIME, &ms);
		printf("recording %d msec\n", ms);
		}

		/* exit video recording by entering video preview mode */
		sp5kModeSet(SP5K_MODE_STILL_PREVIEW);
		sp5kModeWait(SP5K_MODE_STILL_PREVIEW);
	}
	#if 1
	else if (strcmp(*argv, "lscon")==0) {
		sp5kResourceFileLoad(SP5K_RES_LSC,"A:\\RO_RES\\IQ\\LSC.BIN");
		sp5kImgCdspBlockEnable(SP5K_CDSP_LSC, 1);
	}
	else if (strcmp(*argv, "lsccal")==0) {
		appCalibLSC();
	}
	else if (strcmp(*argv, "pvob")==0) {
		appCalibPVOB();
	}
	else if (strcmp(*argv, "line")==0) {
		sensorCmdPortExpLineSend(v[1]);
	}
	#if 1
	else if (strcmp(*argv, "aetbl")==0) {
		appAeTablePrint(v[1],v[2]);
	}
	else if (strcmp(*argv, "aetblscan")==0) {
		tbSensor_PreViewFlickerTableScan();
	}
	else if (strcmp(*argv, "aetblset")==0) {
		appAeFlickerTableExpAgcIdxSet(v[1]);
	}
	#endif
	#endif
	else if(strcmp(*argv, "state")==0){
		if (argc > 1) {
			printf("change state to %x", v[1]);
			appStateChange(v[1], 0);
		}
	}
	else if (strcmp(*argv, "vkb")==0) {
		appCmdVirtualKB(argc, argv);
	}
	#if ICAT_FCWS || ICAT_LDWS /*JerryWang*/
	else if(strcmp(*argv, "das")==0){
		/* 1 is enable, 0 is disable */
		appCvrwsEnableSet(v[1]);
	}
	else if(strcmp(*argv, "dascal")==0){
		/* 1 is move left, 0 is move right */
		appCvrwsCentralPointUpdate(v[1]);
	}
	#endif
	else if(strcmp(*argv, "cdspflip")==0){
		extern void appCdspFlipSet(BOOL bFlip, BOOL Immediate);
		appCdspFlipSet(v[1], v[2]);
	}
	else if(strcmp(*argv, "cdspflip")==0){
		extern void appCdspFlipSet(BOOL bFlip, BOOL Immediate);
		appCdspFlipSet(v[1], v[2]);
	}
	else if (strcmp(*argv, "cardisp")==0) {
		extern UINT32 appFirmwareUpdate(void);	
		appFirmwareUpdate();
	}
	else if (strcmp(*argv, "sensorstb")==0) {
		printf("sensor standby: %d\n", v[1]);
		if(v[1] == 0)
		{
			IO_SENSOR_STB_OFF;
		}
		else
		{
			IO_SENSOR_STB_ON;
		}
	}	
	else if (strcmp(*argv, "reboot")==0) {
		printf("reboot\n");
		extern void appSystemReboot(void);
		appSystemReboot();
	}
	else if (strcmp(*argv, "aeframerate")==0) {
		printf("aeframerate %d\n", v[1]);
		extern void appAeFrameRateSet(UINT32 fps);
		appAeFrameRateSet(v[1]);
	}
	else if (strcmp(*argv, "syscfg")==0) {
		printf("sp5kSystemCfgSet %d, %d, %d\n", v[1], v[2], v[3]);
		sp5kSystemCfgSet(v[1], v[2], v[3]);
	}
	else if(strcmp(*argv, "rfinit")==0)
	{
		printf("rfinit\n");
		void appRfInit(UINT8 CUID[5], UINT8 nRcwMID);
		appRfInit(NULL, v[1]);
	}
	else if(strcmp(*argv, "rfrelease")==0)
	{
		printf("rfrelease\n");
		void appRfRelease(void);
		appRfRelease();
	}
	else if(strcmp(*argv, "rfpairingstart")==0)
	{
		printf("rfparingstart\n");
		void appRfPairingStart(void);
		appRfPairingStart();
	}
	else if(strcmp(*argv, "rfpairingfinish")==0)
	{
		BOOL ret = FALSE;
		UINT8 CUID[5], RfType;
		BOOL appRfParingFinish(UINT8 CUID[5], UINT8* nRcwMID);
		
		ret = appRfParingFinish(CUID, &RfType);		
		printf("rfparingfinish %d (0x%x 0x%x 0x%x 0x%x 0x%x)\n", ret, CUID[0], CUID[1], CUID[2], CUID[3], CUID[4]);
	}
	else if(strcmp(*argv, "smcg")==0)
	{
		printf("smcg = 0x%x\n", sp5kSensorModeCfgGet(v[1]));
	}
	else if(strcmp(*argv, "pvugcb")==0)
	{
		printf("pvugcb");
		extern UINT32 appRfCopyBufferCallback(UINT32 idx,UINT8 *yuvAddr, UINT32 srcW, UINT32 srcH);
		sp5kPreviewUrgentCallbackSet(1, appRfCopyBufferCallback);
	}
	else if(strcmp(*argv, "vdugcb")==0)
	{
		printf("vdugcb");
		extern UINT32 appRfCopyBufferCallback(UINT32 idx,UINT8 *yuvAddr, UINT32 srcW, UINT32 srcH);
		sp5kVideoUrgentCallbackSet(1, appRfCopyBufferCallback);
	}
	else if(strcmp(*argv, "bten")==0)
	{
		printf("bten");
		appBtnEnable(v[1]);
	}
	else if(strcmp(*argv, "pwmset")==0)
	{
		printf("pwm%d fre = %d duty = %d en = %d", v[1], v[2], v[3], v[4]);

		sp5kPwmCfgSet(v[1], v[2], v[3]);
		if(v[4] == 0)
		{
			sp5kPwmTrig( 0,  1 << v[1] );
		}
		else
		{
			sp5kPwmTrig( 1 << v[1], 0 );
		}
	}
	else {
		appCustomerCmd(argc,argv, v);
	}
}

 /*e Modify by Aries 09/12/13*/

#endif /*CMD_TASK_INIT*/



