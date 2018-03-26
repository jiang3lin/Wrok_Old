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

/*#define EVTBL_TEST_CODE 1 */


#define MODEL_VERSION "$unp_afe_ov5610-1.00.02"

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

void sensorAfe_ov5610_Install()
{
	sensorAfeDevInstall(SENSOR_DEV_ID_SENSOR,&sensorAfeDev);
}


/*--------method definitions-------*/

#if EVTBL_TEST_CODE /****TODO*5110*evtbl resource workaround*/
/*afe tables*/
static const UINT8 afetbl_stream[]
COMMON_ALIGNED = {
	80,   /*n_entry*/
	0,    /*tblidx_min*/
	1,    /*nreg*/
	80,   /*agcidx0_svscale,iso100*/
	16,   /*agc_svscale*/
	1,    /*n_burst_tbl*/
	0x00, 0x00, /*template*/
	/*loop1 for n_burst_tbl*/
	80,   /*n_entry_burst*/
	1,    /*nbyte_entry*/
	0,    /*burstidx_min*/
	1,  /*nreg_involved*/
	/*loop0 for nreg_involved*/
	0,  /*reg# in template*/
	0,  /*lsb right shift*/
	8,  /*lsb left shift*/
	8,  /*nbit_mask*/
	/*contains, for n_entry_burst*/
	0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
	0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,
	0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,
	0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,
	0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
	0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f,
	0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,
	0x78,0x79,0x7a,0x7b,0x7c,0x7d,0x7e,0x7f,
	0xf0,0xf1,0xf2,0xf3,0xf4,0xf5,0xf6,0xf7,
	0xf8,0xf9,0xfa,0xfb,0xfc,0xfd,0xfe,0xff,
	/*end contains, end loop1*/
	0,0,128, /*error table, n_erridx, emin, err unit in "N"-th ev*/
	/*contains, for n_erridx*/
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
	sensorAfeCfg_t *pacfg = &sensorAfeCfg;
	UINT32 cmdportid;
	static const UINT8 regdata[][2] = {
		/*{0x0c, 0x38,},*/ /*OV AFE update*/
		{0x0D, 0x31,},
		{0x0f, 0x42,},
		/*{0x22, 0x80,},*/ /*OV AFE update*/
		/*{0x23, 0x80,},*/ /*OV AFE update*/
		{0x33, 0x1b,},
		{0x34, 0x5f,}, /*{0x34, 0x5a,},*/ /*OV AFE update*/
		{0x35, 0x48,},
		{0x36, 0x20,},
		{0x37, 0x04,}, /*{0x37, 0x00,},*/ /*OV AFE update*/
		{0x38, 0xd3,},
		{0x3b, 0x18,},
		{0x3c, 0x1f,},
/* HRChen for OV5610 OB calibration */
		{0x3f, 0x0f,},
#if 0
		{0x40, 0x09,},
		{0x41, 0x18,},
		{0x42, 0x10,},
		{0x43, 0x10,},
/* HRChen for OV5610 OB calibration, 2006-6-13*/
#else
		{0x40, 0x76,}, /* B  channel */
		{0x41, 0x96,}, /* R  channel */
		{0x42, 0x60,}, /* Gb channel */
		{0x43, 0x86,}, /* Gr channel */
#endif
/* END of OV5610 calibration */
		{0x44, 0x00,},
		{0x48, 0x80,}, /*{0x48, 0x00,},*/ /*OV AFE update*/
		{0x49, 0x89,}, /*{0x49, 0x19,},*/ /*OV AFE update*/
		{0x4c, 0x30,},
		{0x4d, 0x00,},
		{0x4e, 0x0f,},
		{0x4f, 0x0a,},
		{0x50, 0x4f,},
	};
	/*if afe shares identical port with tg*/
	cmdportid = pacfg->cmdportid_afe;

	sensorSerialBulkWrite(cmdportid,
		sizeof(regdata)/sizeof(regdata[0]), regdata[0],0);
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
	sensorSerialAsyncWrite(sensorAfeCfg.cmdportid_agc, nreg, pdata);
}
static void
sensor_cmdport_dcoffset_write(
		UINT32 idx
		)
{
	static UINT8 afebuf[2] = {0x28,0,};
	afebuf[1] = idx;
	sensorSerialAsyncWrite(sensorAfeCfg.cmdportid_dcoffset,1,afebuf);
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
