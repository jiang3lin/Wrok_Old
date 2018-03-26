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
#define EVTBL_TEST_CODE 1
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

#if EVTBL_TEST_CODE
/*afe tables*/
	static const UINT8 afetbl_stream[]
	COMMON_ALIGNED = {
/* Create by Exposure Table generator tool Version: 1.7*/
/* AgctblCfg Content (0 n_entry,1 tblidx_min,2 n_reg,3 exptbl_idx0_unit,4 exptbl_idx_unit,5 n_burstsec) */
/* AFE : MT9J003*/
0x60,
0x0 ,
0x2 ,
0x0 ,
0x10,
0x1 ,
/* T,emplate [0]  */
0x30,
0x5e,
0x0 ,
0x0 ,
/* T,emplate [1]  */
0xaf,
0xe0,
0x0 ,
0x0 ,
/* A,gcBurstCfg Content (0 n_burstidx,1 nbyte_entry,2 burstidx_min,3 n_op) */
0x60,
0x3 ,
0x0 ,
0x3 ,
/* B,urstProtocolCfg [0] Content (0 tmpl_reg,1 entry_lsb,2 reg_lsb,3 reg_mask_nbits)*/
0x0 ,
0x8 ,
0x10,
0x8 ,
/* B,urstProtocolCfg [1] Content (0 tmpl_reg,1 entry_lsb,2 reg_lsb,3 reg_mask_nbits)*/
0x0 ,
0x0 ,
0x18,
0x8 ,
/* B,urstProtocolCfg [2] Content (0 tmpl_reg,1 entry_lsb,2 reg_lsb,3 reg_mask_nbits)*/
0x1 ,
0x10,
0x10,
0x8 ,
/*  ,Gain Table Data BEGIN (total entry = 96 , byte entry = 3) */
0x40,
0x10,
0x0 ,
0x42,
0x10,
0x3 ,
0x45,
0x10,
0x2 ,
0x48,
0x10,
0x3 ,
0x4c,
0x10,
0x0 ,
0x4f,
0x10,
0x1 ,
0x52,
0x10,
0x2 ,
0x56,
0x10,
0x1 ,
0x5a,
0x10,
0x1 ,
0x5e,
0x10,
0x1 ,
0x62,
0x10,
0x1 ,
0x62,
0x10,
0xc ,
0x6b,
0x10,
0x1 ,
0x70,
0x10,
0x0 ,
0x75,
0x10,
0x0 ,
0x7a,
0x10,
0x0 ,
0x7f,
0x10,
0x1 ,
0x42,
0x18,
0x2 ,
0x45,
0x18,
0x2 ,
0x48,
0x18,
0x2 ,
0x48,
0x18,
0xd ,
0x4f,
0x18,
0x0 ,
0x52,
0x18,
0x2 ,
0x56,
0x18,
0x1 ,
0x5a,
0x18,
0x0 ,
0x5e,
0x18,
0x0 ,
0x62,
0x18,
0x0 ,
0x62,
0x18,
0xc ,
0x6b,
0x18,
0x0 ,
0x6b,
0x18,
0xb ,
0x70,
0x18,
0xb ,
0x7a,
0x18,
0x0 ,
0x7e,
0x18,
0x2 ,
0x42,
0x1c,
0x1 ,
0x45,
0x1c,
0x1 ,
0x48,
0x1c,
0x1 ,
0x48,
0x1c,
0xd ,
0x4f,
0x1c,
0x0 ,
0x52,
0x1c,
0x1 ,
0x56,
0x1c,
0x0 ,
0x56,
0x1c,
0xb ,
0x5a,
0x1c,
0xb ,
0x62,
0x1c,
0x0 ,
0x62,
0x1c,
0xb ,
0x67,
0x1c,
0x9 ,
0x6b,
0x1c,
0xb ,
0x70,
0x1c,
0xa ,
0x75,
0x1c,
0xa ,
0x7f,
0x1c,
0x0 ,
0xc2,
0x1c,
0x1 ,
0xc5,
0x1c,
0x1 ,
0xc8,
0x1c,
0x1 ,
0xc8,
0x1c,
0xc ,
0xcc,
0x1c,
0x9 ,
0xd2,
0x1c,
0x1 ,
0xd2,
0x1c,
0xc ,
0xd6,
0x1c,
0xb ,
0xda,
0x1c,
0xa ,
0xde,
0x1c,
0xa ,
0xe2,
0x1c,
0xa ,
0xe7,
0x1c,
0x9 ,
0xeb,
0x1c,
0xa ,
0xf0,
0x1c,
0x9 ,
0xf5,
0x1c,
0x9 ,
0xfa,
0x1c,
0xa ,
0xc2,
0x1d,
0x0 ,
0xc5,
0x1d,
0x0 ,
0xc8,
0x1d,
0x0 ,
0xc8,
0x1d,
0xb ,
0xcc,
0x1d,
0x8 ,
0xd2,
0x1d,
0x0 ,
0xd2,
0x1d,
0xb ,
0xd6,
0x1d,
0xa ,
0xda,
0x1d,
0x9 ,
0xde,
0x1d,
0x9 ,
0xe2,
0x1d,
0xa ,
0xe7,
0x1d,
0x8 ,
0xeb,
0x1d,
0x9 ,
0xf0,
0x1d,
0x9 ,
0xf5,
0x1d,
0x9 ,
0xfa,
0x1d,
0x9 ,
0xff,
0x1d,
0xa ,
0xff,
0x1d,
0x15,
0xff,
0x1d,
0x22,
0xff,
0x1d,
0x2f,
0xff,
0x1d,
0x3c,
0xff,
0x1d,
0x4a,
0xff,
0x1d,
0x58,
0xff,
0x1d,
0x68,
0xff,
0x1d,
0x78,
0xff,
0x1d,
0x88,
0xff,
0x1d,
0x99,
0xff,
0x1d,
0xac,
0xff,
0x1d,
0xbe,
0xff,
0x1d,
0xd2,
0xff,
0x1d,
0xe7,
/* G,ain Table Data END */
/* E,rror Table EvErrorCfg Content (0 n_eidx,1 eidx_min,2 err_unit)*/
0x0 ,
0x0 ,
0x40,
/* Error Table Data BEGIN (total entry = 0 ) */
/* Error Table Data BEGIN (total entry = 0 ) */
/* Error Table END */
/* END */
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


