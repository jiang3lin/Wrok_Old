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


#define MODEL_VERSION "$unp_afe_ov2715-1.00.00"

/*HISTORY
 * 1.00.02 20071221 anmin
 *   updating installed struct
 * prerelease
 *   modify code for the SOP 06/03/27
 */

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

#define AFE_PRE_GAIN_EN              1

#if AFE_PRE_GAIN_EN
    #define TG_NBIT_REGDATA          8        /* ssc/spi register data numbers                */
	#define TG_NBIT_REGADDR         16        /* ssc/spi register address numbers             */
    #define PRE_GAIN_ADDR       0xAFE0        /* {0xAF,0xE0,PRE_GAIN(0x80~0xFF)}              */
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

void sensorAfe_ov2715_Install()
{
	sensorAfeDevInstall(SENSOR_DEV_ID_SENSOR,&sensorAfeDev);
}


/*--------method definitions-------*/

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
	sensor_agctbl_init();
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
	cmdportid = pacfg->cmdportid_afe;
	#if AFE_PRE_GAIN_EN
		sensorPreGainEnSet(1);  
        sensorPreGainCfg(0x100);
	#endif  
	profLogPrintf(1,"@@@@@@@@@  sensor_afe_svc_init OV2715  fin\n");
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
	#if AFE_PRE_GAIN_EN

	UINT32 agc_addr[16],agc_data[16]; /*16 is reserve*/
	UINT32 pregain;
	sensor_afe_cmd_paser(nreg,pdata,agc_addr,agc_data,TG_NBIT_REGADDR,TG_NBIT_REGDATA);
	if(agc_addr[nreg-1]==PRE_GAIN_ADDR)/*Find the last reg*/
	{
		pregain=agc_data[nreg-1]|0x100; /*0x100~0x200*/
		if(sensorModeGet()==0x40){
			sensorSnapPreGainCfg(pregain);	  
		}else{
			sensorPreGainCfg(pregain);	  
		}
		
		profLogPrintf(1,"Send Cmd DigitalTbl=%d.\n",agc_data[nreg-1]);
		profLogPrintf(1,"SensorMode=%x,Send Cmd DigitalGain=%d.\n",sensorModeGet(),pregain);
		
		sensorSerialAsyncWrite(sensorAfeCfg.cmdportid_agc,(nreg-1), pdata);
	}
	else
	{
		sensorSerialAsyncWrite(sensorAfeCfg.cmdportid_agc,nreg, pdata);
	}
	#else
		/*profLogPrintf(1,"Send Cmd Gain.\n");*/
		sensorSerialAsyncWrite(sensorAfeCfg.cmdportid_agc,nreg, pdata);
	#endif

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
  
		for(i=0;i<nreg;i++){                                  
  			offset=nbyte_len*i;                                  
  		    addr[i]=0;                                       
  		    idx=offset;                                      
  		    for(j=0;j<nbyte_addr;j++){                        
  				addr[i]|=pdata[idx+j];                         
  				addr[i]=addr[i]<<8;                            
  		    }                                                
  		    addr[i]=addr[i]>>8;                              
  		    data[i]=0;                                       
  		    idx=offset+nbyte_addr;                           
  		    for(j=0;j<nbyte_data;j++){                        
  				data[i]|=pdata[idx+j];                         
  				data[i]=data[i]<<8;                            
  		    }                                                
  		    data[i]=data[i]>>8;                              
  		}                                                    
	}
#endif

static void
sensor_cmdport_dcoffset_write(
		UINT32 idx
		)
{
}

