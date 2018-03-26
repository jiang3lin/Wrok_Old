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
 *  01.01.01, 20130726 Allen.Liu
 *  create a afe.c
 **************************************************************************/
#define MODEL_VERSION "$unp_afe_ar0130-01.01.01"

#include "common.h"
#include "../flow/sensor_model.h"

#define AFE_PRE_GAIN_EN 0
#define MASK_EXP_CMD	0

/*prototypes of sensor model methods*/
static void sensor_afe_svc_resstop();
static void sensor_afe_svc_resload();
static void sensor_afe_svc_stop();
static void sensor_afe_svc_init();
static void sensor_afe_mode_set(UINT32 mode);
static void sensor_cmdport_agc_write(UINT32 nreg,const UINT8 *pdata);
static void sensor_cmdport_dcoffset_write(UINT32 idx);
#if AFE_PRE_GAIN_EN
#define	TG_NBIT_REGDATA	8		/*ssc/spi register data numbers */
#define	TG_NBIT_REGADDR	16		/*ssc/spi register address numbers */
#define PRE_GAIN_REG_ORDER 1    /*which reg# is pregain ,always the last one*/
#define PRE_GAIN_ADDR 0xAFE0    /* {0xAF,0xE0,PRE_GAIN(0x80~0xFF)}*/

static void sensor_afe_cmd_paser(UINT32 nreg,const UINT8 *pdata,UINT32 *addr,UINT32 *data,UINT8 nbitaddr,UINT8 nbitdata);
#endif
/*sensor cfg*/
static sensorAfeCfg_t sensorAfeCfg = {
	/*sensor:normal,gray,diff:int;siggen;tv:yuvS*/
	.img_src_type = FRONT_IMG_SRC_SENSOR,
	.img_src_cfg = 0x00, /*cfg for diff,siggen,tv*/
	.nbit_rgb = FRONT_IMG_SENSOR_BIT_12,
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
#endif		
}

static void sensor_afe_mode_set(UINT32 mode)
{
/*****TODO****/
}

static void sensor_cmdport_agc_write(UINT32 nreg,const UINT8 *pdata)
{
#if !MASK_EXP_CMD
#if AFE_PRE_GAIN_EN
	UINT32 agc_addr[3],agc_data[3];
	UINT32 pregain;
	sensor_afe_cmd_paser(nreg,pdata,agc_addr,agc_data,TG_NBIT_REGADDR,TG_NBIT_REGDATA);
	if(agc_addr[PRE_GAIN_REG_ORDER]==PRE_GAIN_ADDR)
	{
		pregain=agc_data[PRE_GAIN_REG_ORDER]|0x100; /*0x100~0x200*/
		if(sensorModeGet()==0x40){
			sensorSnapPreGainCfg(pregain);		
		}else{
			sensorPreGainCfg(pregain);
			profLogPrintf(1,"sensorPreGainCfg.\n");
		}

		/*
	      profLogPrintf(3,"Send Cmd DigitalTbl=%d.\n",agc_data[PRE_GAIN_REG_ORDER]);
		profLogPrintf(3,"SensorMode=%x,Send Cmd DigitalGain=%d.\n",sensorModeGet(),pregain);
		*/
		sensorSerialAsyncWrite(sensorAfeCfg.cmdportid_agc,(nreg-1), pdata);		
	}
#else
	sensorSerialAsyncWrite(sensorAfeCfg.cmdportid_agc,nreg, pdata);
#endif
#endif
}

static void sensor_cmdport_dcoffset_write(UINT32 idx)
{
	#if 0
	static UINT8 afebuf[2] = {0x28,0,};
	afebuf[1] = idx;
	sensorSerialAsyncWrite(sensorAfeCfg.cmdportid_dcoffset,1,afebuf);
	#endif
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
		{
			addr[i]|=pdata[idx+j];
			addr[i]=addr[i]<<8;
		}
		addr[i]=addr[i]>>8;

		data[i]=0;
		idx=offset+nbyte_addr;
		for(j=0;j<nbyte_data;j++)
		{
			data[i]|=pdata[idx+j];
			data[i]=data[i]<<8;
		}
		data[i]=data[i]>>8;
	}
}
#endif
