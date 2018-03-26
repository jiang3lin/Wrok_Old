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

#define MODEL_VERSION "$unp_afe_ov5653-1.00.03"

/*HISTORY
 * 0.00.01 20090626 cslin
 * first draft
 *
 */
#define AFE_PRE_GAIN_EN 1

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
#define	TG_NBIT_REGDATA	8		/*ssc/spi register data numbers */
#define	TG_NBIT_REGADDR	16		/*ssc/spi register address numbers */
#define PRE_GAIN_REG_ORDER 1    /*which reg# is pregain ,always the last one*/
#define PRE_GAIN_ADDR 0xAFE0    		/* {0xAF,0xE0,PRE_GAIN(0x80~0xFF)}*/

static void sensor_afe_cmd_paser(UINT32 nreg,const UINT8 *pdata,UINT32 *addr,UINT32 *data,UINT8 nbitaddr,UINT8 nbitdata);
#endif
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

void sensorAfe_ov5653_Install()
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
#if 1
	#if AFE_PRE_GAIN_EN
	UINT32 agcAddr,agcData;
	UINT32 preGain;
	sensor_afe_cmd_paser(nreg,pdata,&agcAddr,&agcData,TG_NBIT_REGADDR,TG_NBIT_REGDATA);
	/*printf("agcAddr=%d,data=%d\n",agcAddr,agcData);*/
	if(agcAddr == PRE_GAIN_ADDR){
		preGain = agcData |0x100; /*0x100~0x200*/
		if(sensorModeGet()==0x40){
			sensorSnapPreGainCfg(preGain);		
		}else{
			sensorPreGainCfg(preGain);		
		}
		
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
	UINT8 addrSize,dataSize,offSize,i;

	UINT8 *pData = pdata;
	UINT32 regAddr,regData;

	regAddr = regData = 0;
	
	addrSize = nbitaddr/8;
	dataSize = nbitdata/8;

	offSize = (addrSize + dataSize)*(nreg-1);
	pData = pData + offSize ;

	for(i = 0 ; i < addrSize ; i ++){
		regAddr <<= 8;
		regAddr += *pData;
		pData ++;
	}
	
	for(i = 0 ; i < dataSize ; i ++){
		regData <<= 8;
		regData += *pData;
		pData ++; 
	}
	
	*addr = regAddr;
	*data = regData;
}
#endif


