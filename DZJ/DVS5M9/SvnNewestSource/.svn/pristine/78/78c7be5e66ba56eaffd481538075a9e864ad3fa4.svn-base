 /*************************************************************************
 *                                                                        
 *       Copyright (c) 2004-2006 by Sunplus Technology Co., Ltd.          
 *                                                                        
 *  This software is copyrighted by and is the property of Sunplus        
 *  Technology Co., Ltd. All rights are reserved by Sunplus Technology    
 *  Co., Ltd. This software may only be used in accordance with the       
 *  corresponding license agreement. Any unauthorized use, duplication,   
 *  distribution, or disclosure of this software is expressly forbidden.  
 *                                                                        
 *  This Copyright notice MUST not be removed or modified without prior   
 *  written consent of Sunplus Technology Co., Ltd.                       
 *                                                                        
 *  Sunplus Technology Co., Ltd. reserves the right to modify this        
 *  software without notice.  .                                           
 *                                                                        
 *  Sunplus Technology Co., Ltd.                                          
 *  19, Innovation First Road, Science-Based Industrial Park,             
 *  Hsin-Chu, Taiwan                                                      
 *                                                                        
 *  Author: Ryan Kuo                                                      
 *  Version: 0.01, 2011.9.22:                                             
 *                                                                        
 *************************************************************************/

#define MODEL_VERSION "$unp_afe_sony-imx175-0.00.01"


#include "common.h"
#include "../flow/sensor_model.h"

/*prototypes of sensor model methods*/
extern UINT32 sensorModeGet(void);

static void sensor_afe_svc_resstop();
static void sensor_afe_svc_resload();
static void sensor_afe_svc_stop();
static void sensor_afe_svc_init();
static void sensor_afe_mode_set(UINT32 mode);
static void sensor_cmdport_agc_write(UINT32 nreg,const UINT8 *pdata);
static void sensor_cmdport_dcoffset_write(UINT32 idx);

#define AFE_PRE_GAIN_EN              0

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
	.img_src_cfg = 0x00,                     /*cfg for diff,siggen,tv*/
	.nbit_rgb = FRONT_IMG_SENSOR_BIT_10,
	.afe_effect_frame_count = 1,             /*CCD typically 1*/
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

void sensorAfe_sonyimx175_Install()
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
		sensorSerialBulkWrite(cmdportid, sizeof(regdata)/sizeof(regdata[0]), regdata[0],0);
	#endif
	#if AFE_PRE_GAIN_EN
		sensorPreGainEnSet(1);  
        sensorPreGainCfg(0x100);
	#endif  
}
static void
sensor_afe_mode_set(UINT32 mode)
{
/*****TODO****/
}
static void
sensor_cmdport_agc_write(UINT32 nreg, const UINT8 *pdata)
{
	#if AFE_PRE_GAIN_EN
	
		UINT32 agc_addr[16],agc_data[16]; /*16 is reserve*/
		UINT32 pregain;
		sensor_afe_cmd_paser(nreg,pdata,agc_addr,agc_data,TG_NBIT_REGADDR,TG_NBIT_REGDATA);
		if(agc_addr[nreg-1]==PRE_GAIN_ADDR)/*Find the last reg*/
		{
			pregain=agc_data[PRE_GAIN_REG_ORDER]|0x100; /*0x100~0x200*/
			if(sensorModeGet()==0x40){
				sensorSnapPreGainCfg(pregain);    
			}else{
				sensorPreGainCfg(pregain);    
			}
			/*
			profLogPrintf(1,"Send Cmd DigitalTbl=%d.\n",agc_data[nreg-1]);
			profLogPrintf(1,"SensorMode=%x,Send Cmd DigitalGain=%d.\n",sensorModeGet(),pregain);
			*/
			sensorSerialAsyncWrite(sensorAfeCfg.cmdportid_agc,(nreg-1), pdata);
		}
		else
		{
			sensorSerialAsyncWrite(sensorAfeCfg.cmdportid_agc,nreg, pdata);
		}
	#else
   		/*Sony-IMX175-Analog gain global: 0x0205*/
   		/*group hold*/
        #if 0
		static UINT8 regdata[][3] SENSOR_SIF1DMA_VARBUF = {
                        {0x01, 0x04, 0x01},
						{0x02, 0x05, 0x00},
						{0x02, 0x0F, 0x00},
						{0x02, 0x0E, 0x00},
						{0x02, 0x11, 0x00},
						{0x02, 0x10, 0x00},
						{0x02, 0x13, 0x00},
						{0x02, 0x12, 0x00},
						{0x02, 0x15, 0x00},
						{0x02, 0x14, 0x00},
                        {0x01, 0x04, 0x00},
   					};	
		UINT8 i,j;
	
		for(i=0;i<nreg;i++)
			for(j=0;j<3;j++)
				regdata[i][j]=pdata[i*3+j]; /*copy original value*/
		sensorSerialDirectBufSnoop(regdata, sizeof(regdata));
        
		sensorSerialAsyncWrite( sensorAfeCfg.cmdportid_agc, sizeof(regdata)/6, regdata );
        #else
            sensorSerialAsyncWrite(sensorAfeCfg.cmdportid_agc,nreg, pdata);
        #endif


	#endif


}
static void
sensor_cmdport_dcoffset_write(UINT32 idx)
{
	#if 0
		static UINT8 afebuf[2] = {0x28,0,};
		afebuf[1] = idx;
		sensorSerialAsyncWrite(sensorAfeCfg.cmdportid_dcoffset,1,afebuf);
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
