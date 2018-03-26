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

#define MODEL_VERSION "$unp_afe_ov9810-0.00.01"

/*HISTORY
 * 0.00.01 20090626 cslin
 * first draft
 *
 */

#include "common.h"
#include "../flow/sensor_model.h"

#include "api/sp5k_global_api.h"


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

void sensorAfe_ov9810_Install()
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

	sensor_agctbl_init();

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
	sensorSerialAsyncWrite(sensorAfeCfg.cmdportid_agc, nreg, pdata);	   	
	
}
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
