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
 *  software without notice.  .                                            *
 *                                                                        *
 *  Sunplus Technology Co., Ltd.                                          *
 *  19, Innovation First Road, Science-Based Industrial Park,             *
 *  Hsin-Chu, Taiwan                                                      *
 *                                                                        *
 *  Author: Allen Liu  
 *
 *  01.01.01, 20121221 Allen
 *  For AR0130, enable pre-gain and parallel 12-bit output
 *
 *  02.02.02, 20130110 Allen
 *  Correct agc table error and disable pre-gain
 *
 *  03.02.03, 20130620 Allen
 *  Modify agc table to weaken vertical line issue at large gain environment (use conversion gain)
 **************************************************************************/
#define MODEL_VERSION "$unp_afe_dual_ar0130-03.02.03"

#include <stdio.h>
#include <string.h>
#include "common.h"
#include "../flow/sensor_model.h"

#define AFE_PRE_GAIN_EN 0

/*prototypes of sensor model methods*/
static void sensor_afe_svc_resstop();
static void sensor_afe_svc_resload();
static void sensor_afe_svc_stop();
static void sensor_afe_svc_init();
static void sensor_afe_mode_set(UINT32 mode);
static void sensor_cmdport_agc_write(UINT32 nreg,const UINT8 *pdata);
static void sensor_cmdport_agc2_write(UINT32 nreg,const UINT8 *pdata);
static void sensor_cmdport_dcoffset_write(UINT32 idx);
extern UINT32 profLogPrintf(UINT32 id, const char *format, ...); /*for clean warning*/

#if AFE_PRE_GAIN_EN
#define TG_NBIT_REGDATA	16		/*ssc/spi register data numbers */
#define TG_NBIT_REGADDR	16		/*ssc/spi register address numbers */
#define PRE_GAIN_REG_ORDER 2    		/*which reg# is pregain ,always the last one*/
#define PRE_GAIN_ADDR 0xE0AF    		/* {0xAF,0xE0,PRE_GAIN(0x80~0xFF)}*/

static void sensor_afe_cmd_paser(UINT32 nreg,const UINT8 *pdata,UINT32 *addr,UINT32 *data,UINT8 nbitaddr,UINT8 nbitdata);
#endif

static const UINT8 cmd_zt_sensorA[][2]={
{0xb0,0x16}/*command to sensor A*/
};

static const UINT8 cmd_zt_sensorB[][2]={
{0xb0,0x19}/*command to sensor B*/
};

extern UINT8 g_dualAE;
extern UINT32 cmdportid_zt;
extern UINT32 cmdportid_tg;

/*sensor cfg*/
static sensorAfeCfg_t sensorAfeCfg = {
	/*sensor:normal,gray,diff:int;siggen;tv:yuvS*/
	.img_src_type = FRONT_IMG_SRC_SENSOR,
	.img_src_cfg = 0x00, /*cfg for diff,siggen,tv*/
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
		.agc2write = sensor_cmdport_agc2_write,
		.dcoffsetwrite = sensor_cmdport_dcoffset_write,
	},
	/*remove this definition if no separate channel support*/
	/****TODO****AFE RGB separate gain,offset*/
	/*.afeRGGB = &(const sensorAfeRGGB_t) { , },*/
	.afeRGGB = NULL,
	.verStr = MODEL_VERSION,
};

void sensorAfe_ar0130_Install()
{
	sensorAfeDevInstall(SENSOR_DEV_ID_SENSOR,&sensorAfeDev);
}

static void sensor_afe_svc_resstop()
{
	/*call this if this afe driver supports agc table*/
	sensor_agctbl_delete();
}

static void sensor_afe_svc_resload()
{
	sensorAfeCfg_t *pacfg = &sensorAfeCfg;
	/*if afe shares identical port with tg*/
	pacfg->cmdportid_agc =
	pacfg->cmdportid_dcoffset =
	pacfg->cmdportid_afe = sensorCmdportIdGet(SENSOR_CMDPORTID_SEL_XTG);

	sensor_agctbl_init();
}

static void sensor_afe_svc_stop()
{
}

static void sensor_afe_svc_init()
{
#if AFE_PRE_GAIN_EN
	sensorPreGainEnSet(1);
	sensorPreGainCfg(0x100);
#endif	
}

static void sensor_afe_mode_set(UINT32 mode)
{
	/*****TODO****/
}

typedef struct {
	UINT8  id;/*sensor id*/
	UINT8  valid;
	UINT8  nreg;
	UINT8  data[16];
} ae_data_t;

extern ae_data_t reg_exp[2],reg_agc[2];
extern void dual_sensor_wr_porc();

static void sensor_cmdport_agc_write(UINT32 nreg, const UINT8 *pdata)
{
	profLogPrintf(1,"agc1_ar0130: %x [%x %x %x %x] [%x %x %x %x]",nreg,pdata[0],pdata[1],pdata[2],pdata[3],pdata[4],pdata[5],pdata[6],pdata[7]);
	if (g_dualAE)
	{
		reg_agc[0].valid=1;
		reg_agc[0].nreg=nreg;
		memcpy(reg_agc[0].data,pdata,nreg*4);
		dual_sensor_wr_porc();
		return;
	}
	sensorSerialAsyncWrite(sensorAfeCfg.cmdportid_agc,nreg, pdata);
	sensorSerialFlush(sensorAfeCfg.cmdportid_agc);
}

static void sensor_cmdport_agc2_write(UINT32 nreg, const UINT8 *pdata)
{
	profLogPrintf(1,"agc2_ar0130: %x [%x %x %x %x] [%x %x %x %x]",nreg,pdata[0],pdata[1],pdata[2],pdata[3],pdata[4],pdata[5],pdata[6],pdata[7]);
	if (g_dualAE)
	{
		reg_agc[1].valid=1;
		reg_agc[1].nreg=nreg;
		memcpy(reg_agc[1].data,pdata,nreg*4);
		dual_sensor_wr_porc();
		return;
	}
	sensorSerialAsyncWrite(sensorAfeCfg.cmdportid_agc,nreg, pdata);
	sensorSerialFlush(sensorAfeCfg.cmdportid_agc);
}

static void sensor_cmdport_dcoffset_write(UINT32 idx)
{
}

#if AFE_PRE_GAIN_EN
static void sensor_afe_cmd_paser(UINT32 nreg,const UINT8 *pdata,UINT32 *addr,UINT32 *data,UINT8 nbitaddr,UINT8 nbitdata)
{
	UINT8 i,j;
	UINT8 nbyte_addr,nbyte_data,nbyte_len;
	UINT8 offset,idx;

	nbyte_addr=nbitaddr/8;
	nbyte_data=nbitdata/8;
	nbyte_len=nbyte_addr+nbyte_data;
	
	for(i=0;i<nreg;i++)
	{
		offset=nbyte_len*i;
		addr[i]=0;
		idx=offset;
		for(j=0;j<nbyte_addr;j++)
			addr[i]|=pdata[idx+j]<<(8*j);

		data[i]=0;
		idx=offset+nbyte_addr;
		for(j=0;j<nbyte_data;j++)
			data[i]|=pdata[idx+j]<<(8*j);
	}
}
#endif
