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
 *  Author: Vincent Tsao  
 *
 *  Ver. 0.01  Initial version.
 *                                                                        *
 **************************************************************************/

#define MODEL_VERSION "$unp_afe_mt9j003-0.00.01"

/*HISTORY
 * 0.00.01 20090626 cslin
 * first draft
 *
 */
#define AFE_PRE_GAIN_EN 1

#include "common.h"
#include "../flow/sensor_model.h"
#include "middleware/proflog.h"

/*prototypes of sensor model methods*/
extern UINT32 sensorModeGet(void);

static void sensor_afe_svc_resstop();
static void sensor_afe_svc_resload();
static void sensor_afe_svc_stop();
static void sensor_afe_svc_init();
static void sensor_afe_mode_set(UINT32 mode);
static void sensor_cmdport_agc_write(UINT32 nreg,const UINT8 *pdata);
static void sensor_cmdport_dcoffset_write(UINT32 idx);
#if AFE_PRE_GAIN_EN
#define	TG_NBIT_REGDATA	16		/*ssc/spi register data numbers */
#define	TG_NBIT_REGADDR	16		/*ssc/spi register address numbers */
#define PRE_GAIN_REG_ORDER 1    /*which reg# is pregain ,always the last one*/
#define PRE_GAIN_ADDR 0xE0AF    /* {0xAF,0xE0,PRE_GAIN(0x80~0xFF)}*/

static void sensor_afe_cmd_paser(UINT32 nreg,const UINT8 *pdata,UINT32 *addr,UINT32 *data,UINT8 nbitaddr,UINT8 nbitdata);
#endif

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
		.dcoffsetwrite = sensor_cmdport_dcoffset_write,
	},
	/*remove this definition if no separate channel support*/
	/****TODO****AFE RGB separate gain,offset*/
	/*.afeRGGB = &(const sensorAfeRGGB_t) { , },*/
	.afeRGGB = NULL,
	.verStr = MODEL_VERSION,
};

void sensorAfe_mt9J003_Install()
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
#if AFE_PRE_GAIN_EN
	sensorPreGainEnSet(1);
	sensorPreGainCfg(0x100);
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
	profLogPrintf(0,"[%x %x %x %x] [%x %x %x %x]",pdata[0],pdata[1],pdata[2],pdata[3],pdata[4],pdata[5],pdata[6],pdata[7]);
#if 1		
#if 1/*AFE_PRE_GAIN_EN*/
	UINT32 agc_addr[3],agc_data[3];
	UINT32 pregain;

	static UINT8 regdata[][4] SENSOR_SIF1DMA_VARBUF= { 
	  {0x30, 0x5e, 0x00, 0x00},
	  {0x30, 0x1A, 0x00, 0x1E}
	}; 

	regdata[0][2] = pdata[2];
	regdata[0][3] = pdata[3];	
	
	sensor_afe_cmd_paser(nreg,pdata,agc_addr,agc_data,TG_NBIT_REGADDR,TG_NBIT_REGDATA);

	/*profLogPrintf(1,"agc_addr[0]:0x%x, agc_addr[1]:0x%x",agc_addr[0],agc_addr[PRE_GAIN_REG_ORDER]);*/

	if(agc_addr[PRE_GAIN_REG_ORDER]==PRE_GAIN_ADDR)
	{
		pregain=agc_data[PRE_GAIN_REG_ORDER]|0x100; /*0x100~0x200*/

		if(sensorModeGet()==0x40)
		{
			sensorSnapPreGainCfg(pregain);		
			/*			
			profLogPrintf(0,"[2:RegCnt:%d] [%x %x %x %x]",sizeof(regdata)/sizeof(regdata[0]),regdata[0][0],regdata[0][1],regdata[0][2],regdata[0][3]);
			profLogPrintf(0,"[%x %x %x %x]",regdata[1][0],regdata[1][1],regdata[1][2],regdata[1][3]);	
			sensorSerialAsyncWrite(sensorAfeCfg.cmdportid_agc,(sizeof(regdata)/sizeof(regdata[0]))-1, regdata);			
			*/				
		}
		else
		{		
			sensorPreGainCfg(pregain);		
		}

		/*
		profLogPrintf(1,"[RegCnt:%d] [%x %x %x %x]",nreg,pdata[0],pdata[1],pdata[2],pdata[3]);
		*/		
		sensorSerialDirectBufSnoop((UINT8 *)pdata, (nreg-1)*4);/*liumr modify 20121108*/
		sensorSerialAsyncWrite(sensorAfeCfg.cmdportid_agc,(nreg-1), pdata);

		
	}
#else
	sensorSerialAsyncWrite(sensorAfeCfg.cmdportid_agc,nreg, pdata);
#endif
#endif
}
static void
sensor_cmdport_dcoffset_write(
		UINT32 idx
		)
{
}

#if AFE_PRE_GAIN_EN
static void
sensor_afe_cmd_paser(UINT32 nreg,const UINT8 *pdata,UINT32 *addr,UINT32 *data,UINT8 nbitaddr,UINT8 nbitdata)
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


