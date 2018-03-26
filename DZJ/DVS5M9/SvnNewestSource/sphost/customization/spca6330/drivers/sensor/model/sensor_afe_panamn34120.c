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
 *  Author: Vincent Tsao                                                     *
 *  Version: 0.01, 2011.5.10: Modify the pregain calculation after reading from resource. *
 *                                                                        *
 **************************************************************************/

#define MODEL_VERSION "$unp_afe_panamn34120-0.00.01"

/*HISTORY
 * 0.00.01 20090626 cslin
 * first draft
 *
 */
#define AFE_PRE_GAIN_EN           (1)
#define SENSOR_PROF_LOG_EN        (0)

#if SENSOR_PROF_LOG_EN
    #define SENSOR_PROF_LOG_ADD(id, str)			profLogAdd(id, str)
    #define SENSOR_PROF_LOG_PRINT(id, fmt, arg...)	profLogPrintf(id, fmt, ##arg)
#else
    #define SENSOR_PROF_LOG_ADD(id, str)
    #define SENSOR_PROF_LOG_PRINT(id, fmt,arg...)
#endif


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
#if AFE_PRE_GAIN_EN
#define	TG_NBIT_REGDATA	16		/*ssc/spi register data numbers */
#define	TG_NBIT_REGADDR	16		/*ssc/spi register address numbers */
#define PRE_GAIN_REG_ORDER 2    /*which reg# is pregain ,always the last one*/
#define PRE_GAIN_ADDR 0xAFE0    /* {0xAF,0xE0,PRE_GAIN(0x80~0xFF)}*/
static void sensor_afe_cmd_paser(UINT32 nreg,const UINT8 *pdata,UINT32 *addr,UINT32 *data,UINT8 nbitaddr,UINT8 nbitdata);
#endif

#define EVTBL_TEST_CODE    0

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

void sensorAfe_PANAMN34120_Install()
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
    sensorSnapPreGainCfg(0x100);                                                                                                                                                                                                                                                                                                                                                                                                                                                      
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
    #if AFE_PRE_GAIN_EN
        UINT32 agc_addr[3],agc_data[3];                                                                                                                                                                                                                                                                                                                                                                                                                                                             
        UINT32 pregain;                                                                                                                                                                                                                                                                                                                                                                                                                                                                             
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            
        sensor_afe_cmd_paser(nreg,pdata,agc_addr,agc_data,TG_NBIT_REGADDR,TG_NBIT_REGDATA);                                                                                                                                                                                                                                                                                                                                                                                                         
        SENSOR_PROF_LOG_PRINT(1,"[nreg=%d][%x %x %x %x] [%x %x %x %x] [%x %x %x %x]",nreg,pdata[0],pdata[1],pdata[2],pdata[3],pdata[4],pdata[5],pdata[6],pdata[7],pdata[8],pdata[9],pdata[10] ,pdata[11]);                                                                                                                                                                                                                                                                                                                
        SENSOR_PROF_LOG_PRINT(1,"[agc_addr][ %x] [ %x] [ %x] ",agc_addr[0],agc_addr[1],agc_addr[2]);                                                                                                                                                                                                                                                                                                                                                                                                        
        SENSOR_PROF_LOG_PRINT(1,"[agc_data][ %x] [ %x] [ %x] ",agc_data[0],agc_data[1],agc_data[2]);                                                                                                                                                                                                                                                                                                                                                                                                        
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           
        if(agc_addr[PRE_GAIN_REG_ORDER]==PRE_GAIN_ADDR){                                                                                                                                                                                                                                                                                                                                                                                                                                             
            pregain=agc_data[PRE_GAIN_REG_ORDER]; /* PreGain:  1x        ~  4x
                                                                                     Code:     0x0100  ~  0x0400, */                                                                                                                                                                                                                                                                                                                                                                                                                             
            if((sensorModeGet()& 0x40)==0x40){                                                                                                                                                                                                                                                                                                                                                                                                                                                              
                sensorSnapPreGainCfg(pregain);                                                                                                                                                                                                                                                                                                                                                                                                                                                      
            }else{                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  
                sensorPreGainCfg(pregain);                                                                                                                                                                                                                                                                                                                                                                                                                                                          
            }                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       
            SENSOR_PROF_LOG_PRINT(1,"Send Cmd DigitalTbl=%d.\n",agc_data[PRE_GAIN_REG_ORDER]);                                                                                                                                                                                                                                                                                                                                                                                                            
            sensorSerialAsyncWrite(sensorAfeCfg.cmdportid_agc,(nreg-1), pdata);                                                                                                                                                                                                                                                                                                                                                                                                                     
        }                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           
	#else
        sensorSerialAsyncWrite(sensorAfeCfg.cmdportid_agc,nreg, pdata);
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


