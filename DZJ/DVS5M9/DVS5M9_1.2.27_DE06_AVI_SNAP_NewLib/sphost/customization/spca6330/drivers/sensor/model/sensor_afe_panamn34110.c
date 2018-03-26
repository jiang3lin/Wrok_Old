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

#define MODEL_VERSION "$unp_afe_panamn34110-0.00.01"

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

void sensorAfe_PANAMN34110_Install()
{
	sensorAfeDevInstall(SENSOR_DEV_ID_SENSOR,&sensorAfeDev);
}

static void
sensor_afe_svc_resstop()
{
	/*call this if this afe driver supports agc table*/
	sensor_agctbl_delete();
}
#if EVTBL_TEST_CODE
/*afe tables*/
	static const UINT8 afetbl_stream[]
	COMMON_ALIGNED = {
	0xfe, 0x0, 0x2, 0x0, 0x10, 0x1, 0x30, 0x0, 0x0, 0xaf, 0xe0, 0x0,  0xfe,
	0x2, 0x0, 0x2, 0x0, 0x0, 0x10, 0x8, 0x1, 0x8, 0x10, 0x8, 0x0, 0x0, 0x0,
	0x3, 0x0, 0x7, 0x0, 0xa, 0x0,  0xe, 0x1, 0x1, 0x1, 0x5, 0x1,  0x8, 0x1,
	0xc, 0x2, 0x1, 0x2, 0x4, 0x2,  0x8, 0x2, 0xb, 0x3, 0x1, 0x3,  0x4, 0x3,
	0x8, 0x3, 0xb, 0x4, 0x1, 0x4,  0x5, 0x4, 0x9, 0x4, 0xc, 0x5,  0x3, 0x5,
	0x6, 0x5, 0xa, 0x6, 0x1, 0x6,  0x5, 0x6, 0x8, 0x7, 0x0, 0x7,  0x4, 0x7,
	0x7, 0x8, 0x0, 0x8, 0x3, 0x8,  0x7, 0x9, 0x0, 0x9, 0x3, 0x9,  0x7, 0xa,
	0x0, 0xa, 0x4, 0xa, 0x7, 0xb,  0x1, 0xb, 0x5, 0xb, 0x8, 0xc,  0x2, 0xc,
	0x6, 0xd, 0x0, 0xd, 0x4, 0xd,  0x7, 0xe, 0x2, 0xe, 0x5, 0xf,  0x0, 0xf,
	0x4, 0xf, 0x7,	0xf, 0xb, 0xf,	0xf, 0xf, 0x12,  0xf, 0x16, 0x11,  0x1,
	0x11, 0x5, 0x11, 0x8, 0x11, 0xc, 0x12, 0x0, 0x12, 0x4, 0x12, 0x7, 0x12,
	0xb, 0x13, 0x0, 0x13, 0x4, 0x13, 0x7, 0x13, 0xb, 0x14, 0x1, 0x14,  0x5,
	0x14, 0x8, 0x14, 0xc, 0x15, 0x2, 0x15, 0x6, 0x15, 0xa, 0x16, 0x1, 0x16,
	0x5, 0x16, 0x8, 0x17, 0x0, 0x17, 0x4, 0x17, 0x7, 0x18, 0x0, 0x18,  0x3,
	0x18, 0x7, 0x19, 0x0, 0x19, 0x3, 0x19, 0x7, 0x1a, 0x0, 0x1a, 0x3, 0x1a,
	0x7, 0x1b, 0x1, 0x1b, 0x4, 0x1b, 0x8, 0x1c, 0x2, 0x1c, 0x5, 0x1d,  0x0,
	0x1d, 0x3, 0x1d, 0x7, 0x1e, 0x1, 0x1e, 0x5, 0x1f, 0x0, 0x1f, 0x4, 0x1f,
	0x7, 0x1f,	0xb, 0x1f,	0xe, 0x1f,	0x12, 0x1f,  0x16, 0x31, 0x1, 0x31,
	0x4, 0x31, 0x8, 0x31, 0xb, 0x32, 0x0, 0x32, 0x3, 0x32, 0x7, 0x32,  0xb,
	0x33, 0x0, 0x33, 0x3, 0x33, 0x7, 0x33, 0xb, 0x34, 0x1, 0x34, 0x4, 0x34,
	0x8, 0x34, 0xb, 0x35, 0x2, 0x35, 0x6, 0x35, 0x9, 0x36, 0x1, 0x36,  0x4,
	0x36, 0x8, 0x37, 0x0, 0x37, 0x3, 0x37, 0x7, 0x37, 0xa, 0x38, 0x3, 0x38,
	0x6, 0x38, 0xa, 0x39, 0x3, 0x39, 0x6, 0x3a, 0x0, 0x3a, 0x3, 0x3a,  0x7,
	0x3b, 0x0, 0x3b, 0x4, 0x3b, 0x7, 0x3c, 0x1, 0x3c, 0x5, 0x3c, 0x9, 0x3d,
	0x3, 0x3d, 0x6, 0x3e, 0x1, 0x3e, 0x5, 0x3f, 0x0, 0x3f, 0x3, 0x3f,  0x7,
	0x3f, 0xa,	0x3f, 0xe,	0x3f, 0x12,  0x3f, 0x15,  0x71, 0x0, 0x71, 0x4,
	0x71, 0x7, 0x71, 0xb, 0x72, 0x0, 0x72, 0x3, 0x72, 0x7, 0x72, 0xa, 0x73,
	0x0, 0x73, 0x3, 0x73, 0x7, 0x73, 0xa, 0x74, 0x0, 0x74, 0x4, 0x74,  0x7,
	0x74, 0xb, 0x75, 0x2, 0x75, 0x5, 0x75, 0x9, 0x76, 0x0, 0x76, 0x4, 0x76,
	0x7, 0x76, 0xb, 0x77, 0x3, 0x77, 0x6, 0x77, 0xa, 0x78, 0x2, 0x78,  0x6,
	0x78, 0x9, 0x79, 0x2, 0x79, 0x6, 0x79, 0x9, 0x7a, 0x3, 0x7a, 0x6, 0x7b,
	0x0, 0x7b, 0x3, 0x7b, 0x7, 0x7c, 0x1, 0x7c, 0x5, 0x7c, 0x8, 0x7d,  0x3,
	0x7d, 0x6, 0x7e, 0x1, 0x7e, 0x4, 0x7e, 0x8, 0x7f, 0x3, 0x7f, 0x6, 0x7f,
	0xa, 0x7f, 0xe, 0x7f, 0x11,  0x7f, 0x15, 0x7f, 0x19, 0x7f,	0x1d, 0x7f,
	0x21, 0x7f, 0x25, 0x7f, 0x29, 0x7f, 0x2d, 0x7f, 0x31, 0x7f, 0x35, 0x7f,
	0x39, 0x7f, 0x3d, 0x7f, 0x42, 0x7f, 0x46, 0x7f, 0x4b, 0x7f, 0x4f, 0x7f,
	0x54, 0x7f, 0x58, 0x7f, 0x5d, 0x7f, 0x62, 0x7f, 0x67, 0x7f, 0x6c, 0x7f,
	0x71, 0x7f, 0x76, 0x7f, 0x7b, 0x7f, 0x80, 0x7f, 0x85, 0x7f, 0x8a, 0x7f,
	0x90, 0x7f, 0x95, 0x7f, 0x9b, 0x7f, 0xa0, 0x7f, 0xa6, 0x7f, 0xac, 0x7f,
	0xb2, 0x7f, 0xb8, 0x7f, 0xbe, 0x7f, 0xc4, 0x7f, 0xca, 0x7f, 0xd0, 0x7f,
	0xd7, 0x7f, 0xdd, 0x7f, 0xe4, 0x7f, 0xea, 0x7f, 0xf1, 0x7f, 0xf8, 0x7f,
	0xfe, 0x0, 0x0, 0x40,
};
#endif

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


