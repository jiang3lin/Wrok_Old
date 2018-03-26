/**************************************************************************
 *                                                                        *
 *       Copyright (c) 2004-2006 by Sunplus Technology Co., Ltd.          *
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
 *  Hsin-Chu, Taiwan                                                      *
 *                                                                        *
 *  Author: Will Chiu                                                     *
 *                                                                        *
 **************************************************************************/

#define MODEL_VERSION "$unp_afe_ov5633-0.00.01"
#define EVTBL_TEST_CODE 1
/*HISTORY
 * 0.00.01 20090626 cslin
 * first draft
 *
 */

#include "common.h"
#include "../flow/sensor_model.h"

/*prototypes of sensor model methods*/
static void sensor_afe_svc_resstop();
static void sensor_afe_svc_resload();
static void sensor_afe_svc_stop();
static void sensor_afe_svc_init();
static void sensor_afe_mode_set(UINT32 mode);
static void sensor_cmdport_agc_write(UINT32 nreg,const UINT8 *pdata);
static void sensor_cmdport_dcoffset_write(UINT32 idx);

/*s Added by Eric 100413 */
extern UINT8 Dummydata[6];
extern UINT8 lastDummydata[2];
extern UINT8 AgcDummySyncWrite;
/*e Added by Eric 100413 */

/*sensor cfg*/
static sensorAfeCfg_t sensorAfeCfg = {
	/*sensor:normal,gray,diff:int;siggen;tv:yuvS*/
	.img_src_type = FRONT_IMG_SRC_SENSOR,
	.img_src_cfg = 0, /*cfg for diff,siggen,tv*/
	.nbit_rgb = FRONT_IMG_SENSOR_BIT_10,
	.afe_effect_frame_count = 1, /*CCD typically 1*/
};

static const sensorAfeDev_t sensorAfeDev = {
	.afeCfg = &sensorAfeCfg,
	.afeSvc = &(const sensorAfeSvc_t) {
		.resload = sensor_afe_svc_resload,
		.resstop = sensor_afe_svc_resstop,
		.init = sensor_afe_svc_init,
		.stop = sensor_afe_svc_stop,
		.pwrsave = sensor_afe_mode_set,
		.agcwrite = sensor_cmdport_agc_write,
		.dcoffsetwrite = sensor_cmdport_dcoffset_write,
	},
	/*remove this definition if no separate channel support*/
	/****TODO****AFE RGB separate gain,offset*/
	/*.afeRGGB = &(const sensorAfeRGGB_t) { , },*/
	.afeRGGB = NULL,
	.verStr = MODEL_VERSION,
};

void sensorAfe_ov5633_Install()
{
	sensorAfeDevInstall(SENSOR_DEV_ID_SENSOR,&sensorAfeDev);
}

#if EVTBL_TEST_CODE /****TODO*5110*evtbl resource workaround*/
/*afe tables*/
static const UINT8 afetbl_stream[]
COMMON_ALIGNED = {
64,
0,
1,
0,
16,
1,
0x30,
0x00,
0,
64,
1,
0,
1,
0,
0,
16,
8,
0,
0,
1,
2,
3,
3,
4,
5,
6,
7,
8,
9,
10,
12,
13,
14,
15,
15,
17,
18,
19,
19,
20,
21,
22,
23,
24,
25,
26,
28,
29,
30,
31,
31,
49,
50,
51,
51,
52,
53,
54,
55,
56,
57,
58,
60,
61,
62,
63,
63,
113,
114,
115,
115,
116,
117,
118,
119,
120,
121,
122,
124,
125,
126,
64,
0,
64,
0,
4,
2,
1,
0,
4,
3,
3,
3,
2,
3,
3,
3,
0,
1,
2,
3,
7,
2,
1,
0,
4,
3,
3,
3,
2,
3,
3,
3,
0,
1,
2,
3,
7,
2,
1,
0,
4,
3,
3,
3,
2,
3,
3,
3,
0,
1,
2,
3,
7,
2,
1,
0,
4,
3,
3,
3,
2,
3,
3,
3,
0,
1,
2
};
#endif

static void
sensor_afe_svc_resstop()
{
	/*call this if this afe driver supports agc table*/
	sensor_agctbl_delete();
}
static void
sensor_afe_svc_resload()
{
	sensorAfeCfg_t *pacfg = &sensorAfeCfg;
	/*if afe shares identical port with tg*/
	pacfg->cmdportid_agc =
	pacfg->cmdportid_dcoffset =
	pacfg->cmdportid_afe = sensorCmdportIdGet(SENSOR_CMDPORTID_SEL_XTG);

#if EVTBL_TEST_CODE /****TODO*5110*evtbl resource workaround*/
	sensor_agctbl_create(afetbl_stream, sizeof(afetbl_stream));
#else
	sensor_agctbl_init();
#endif

}

static void
sensor_afe_svc_stop()
{
}
static void
sensor_afe_svc_init()
{
	#if 0
	sensorAfeCfg_t *pacfg = &sensorAfeCfg;
	UINT32 cmdportid;
	static const UINT8 regdata[][2] = {
		{0x11, 0x40,},

	};
	/*if afe shares identical port with tg*/
	cmdportid = pacfg->cmdportid_afe;

	sensorSerialBulkWrite(cmdportid,
		sizeof(regdata)/sizeof(regdata[0]), regdata[0],0);
	#endif
}
static void
sensor_afe_mode_set(
		UINT32 mode
		)
{
/*****TODO****/
}
static void
sensor_cmdport_agc_write(
		UINT32 nreg,
		const UINT8 *pdata
		)
{
/*s modified by Eric Lee 20100413 */
if (AgcDummySyncWrite == 1)
{
	#if 0
	  printf("Dummydata A[0]:0x%x\n",Dummydata[0]);	
	  printf("Dummydata A[1]:0x%x\n",Dummydata[1]);	
	  printf("Dummydata A[2]:0x%x\n",Dummydata[2]);
	  
	  printf("Dummydata A[3]:0x%x\n",Dummydata[3]);	
	  printf("Dummydata A[4]:0x%x\n",Dummydata[4]);	
	  printf("Dummydata A[5]:0x%x\n",Dummydata[5]);	  

	   printf("nreg:0x%x\n",nreg);	  
	#endif
	UINT8 data[9];

	nreg =3;

	#if 1
	data[0]=pdata[0];
	data[1]=pdata[1];
	data[2]=pdata[2];
	data[3]=Dummydata[0];
	data[4]=Dummydata[1];
	data[5]=Dummydata[2];
	data[6]=Dummydata[3];
	data[7]=Dummydata[4];
	data[8]=Dummydata[5];
	#else
	data[0]=Dummydata[0];
	data[1]=Dummydata[1];
	data[2]=Dummydata[2];
	data[3]=Dummydata[3];
	data[4]=Dummydata[4];
	data[5]=Dummydata[5];
	data[6]=pdata[0];
	data[7]=pdata[1];
	data[8]=pdata[2];

	#endif

	sensorSerialAsyncWrite(sensorAfeCfg.cmdportid_agc, nreg, data);
/*	printf(" Agc - Dummy - Sync Write \n"); */
	
}
else
{
	sensorSerialAsyncWrite(sensorAfeCfg.cmdportid_agc, nreg, pdata);
/*	printf(" exp - Dummy - Sync Write 2\n"); */
	
}
}
/*e modified by Eric Lee 20100413 */
static void
sensor_cmdport_dcoffset_write(
		UINT32 idx
		)
{
	#if 0
	static UINT8 afebuf[2] = {0x28,0,};
	afebuf[1] = idx;
	sensorSerialAsyncWrite(sensorAfeCfg.cmdportid_dcoffset,1,afebuf);
	#endif
}
#if 0 /****TODO****AFE RGGB**/
static void
sensor_afe_agc_rggb_set(
		UINT32 gainR,
		UINT32 gainGr,
		UINT32 gainB,
		UINT32 gainGb,
		UINT32 cfa_pattern
		)
{
/*****TODO****/
}
static void
sensor_afe_dcoffset_rggb_set(
		UINT32 cfa_pattern,
		UINT32 dcoffsetR,
		UINT32 dcoffsetGr,
		UINT32 dcoffsetGb,
		UINT32 dcoffsetB
		)
{
/*****TODO****/
}
void
sensor_afe_capability()
{
/*****TODO****/
	/*agc range*/
	/*dcoffset range*/
	/*analog separate channel gain, range*/
	/*analog separate channel dcoffset, range*/
}

static UINT32 app_wb_preview_rggb_agc_set(
		UINT32 gainR,
		UINT32 gainGr,
		UINT32 gainB,
		UINT32 gainGb,
		UINT32 cfa_pattern
		)
{
/*****TODO****/
	return SUCCESS;
}
#endif
