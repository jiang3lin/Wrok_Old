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
 *  Author: Anmin Deng                                                    *
 *                                                                        *
 **************************************************************************/

#define EVTBL_TEST_CODE 1


#define MODEL_VERSION "$unp_afe_ov9655-1.00.02"

/*HISTORY
 * 1.00.02 20071221 anmin
 *   updating installed struct
 * prerelease
 *   modify code for the SOP 06/03/27
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

void sensorAfe_ov7740_yuv_Install()
{
	sensorAfeDevInstall(SENSOR_DEV_ID_SENSOR,&sensorAfeDev);
}
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

	/*call this if this afe driver supports agc table*/
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
}
static void
sensor_cmdport_dcoffset_write(
		UINT32 idx
		)
{
}

