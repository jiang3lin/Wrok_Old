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
 *                                                                        *
 *  00.00.01, 20130630 Chris.Wang                                         *
 *            1. Sensor Max Analog Gain:                                  *
 *            2. Disable PreGain.                                         *
 *                                                                        *
 *            Note:                                                       *
 **************************************************************************/

#define MODEL_VERSION "$unp_afe_tvp5150-01.01.01"
#include "common.h"
#include "../flow/sensor_model.h"
#include "../flow/general.h"


/*prototypes of sensor model methods*/
static void sensor_afe_svc_resstop();
static void sensor_afe_svc_resload();
static void sensor_afe_svc_stop();
static void sensor_afe_svc_init();
static void sensor_afe_mode_set(UINT32 mode);
static void sensor_cmdport_agc_write(UINT32 nreg,const UINT8 *pdata);
static void sensor_cmdport_dcoffset_write(UINT32 idx);

#define SENSOR_PROF_LOG_EN        (1)
#define PROF_ID                   (3)
#define EVTBL_TEST_CODE           (1) /* use static agc table not from resource.*/

#if SENSOR_PROF_LOG_EN
    #define SENSOR_PROF_LOG_ADD(str)                profLogAdd(PROF_ID, str)
    #define SENSOR_PROF_LOG_PRINT(fmt, arg...)      profLogPrintf(PROF_ID, fmt, ##arg)
#if 0
    #define SENSOR_PROF_LOG_PRINT(fmt, arg...) \
    do{\
        profLogPrintf(PROF_ID, "[OV9713] %s(%d)_"fmt,__FUNCTION__,__LINE__, ##arg);\
        printf("[OV9713] %s(%d)_"fmt"\n",__FUNCTION__,__LINE__, ##arg);\
    }while(0)
#endif

#else
    #define SENSOR_PROF_LOG_ADD(id, str)
    #define SENSOR_PROF_LOG_PRINT(id, fmt,arg...)
#endif

#define _SENSOR_DBG_ 1

#if(_SENSOR_DBG_)
    /*debug by TGGPIO2*/
    #define DBG_GPIO(x) if(x){WRITE8(0xb0009032, (READ8(0xb0009032)|(0x02)));}else{WRITE8(0xb0009032, (READ8(0xb0009032)&(~0x02)));}
#else
    #define DBG_GPIO(x)
#endif

/**
 * \a .img_src_type:  one of frontSensorSrc_t,
 *     in case of FRONT_IMG_SRC_TV, adds on bitmaps TV_LEVEL_SHIFT_Y/_U/_V;
 * \a .img_src_cfg:   config img src according to .img_src_type,
 *     if .img_src_type is FRONT_IMG_SRC_TV, .img_src_cfg bits defined as
 *     bit[1:0] uvsel: 0=Cb, 1=Y1, 2=Cr, 3=Y2;
 *     bit[2]   selffielden: to generate 601 mode field signal (5110 revoked)
 *     bit[3]   fieldpol:  invert field signal to CDSP (5210 revoked)
 *     bit[4]   bt656en:   0=601 mode, 1=656 mode;
 *     bit[6:5] tvsyncen:  0=TV decoding v:hvalid, 3=SSG v:hvalid;
 *     bit[7]   fldsvden:  field signal transition sync to VD; (5110 revoked)
 */

/**
 * \param  val
 *     bit[1:0] uvsel: 0=Cb, 1=Y1, 2=Cr, 3=Y2;
 *     bit[2]   selffielden: to generate 601 mode field signal (5110 revoked)
 *     bit[3]   fieldpol:  invert field signal to CDSP; (5210 revoked)
 *     bit[4]   bt656en:   0=601 mode, 1=656 mode;
 *     bit[6:5] tvsyncen:  0=TV decoding v:hvalid, 3=SSG v:hvalid;
 *     bit[7]   fldsvden:  field signal transition sync to VD; (5110 revoked)
 *     bit[8]   ys128en:   subtract 128 from Y;
 *     bit[9]   us128en:   subtract 128 from U;
 *     bit[10]  vs128en:   subtract 128 from V;
 */

/*sensor cfg*/
static sensorAfeCfg_t sensorAfeCfg = {
	/*sensor:normal,gray,diff:int;siggen;tv:yuvS*/
#if 0
	.img_src_type = FRONT_IMG_SRC_SENSOR,
	.img_src_cfg = 0x00, /*cfg for diff,siggen,tv*/
#else
    .img_src_type = FRONT_IMG_SRC_TV
     + TV_LEVEL_SHIFT_U + TV_LEVEL_SHIFT_V ,
    .img_src_cfg = 0x00 + (1<<5) + (1<<6) + (1<<7) , /* CCIR601 */
    /* .img_src_cfg = 0x10 , CCIR656 */
#endif
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
#if EVTBL_TEST_CODE /****TODO*5110*evtbl resource workaround*/
/*afe tables*/
static const UINT8 afetbl_stream[]
COMMON_ALIGNED = {
/* AgctblCfg Content (0 n_entry,1 tblidx_min,2 n_reg,3 exptbl_idx0_unit,4 exptbl_idx_unit,5 n_burstsec) */
64,
0,
1,
0,
16,
1,
/* Template [0]  */
0x00,
0x00,
/* AgcBurstCfg Content (0 n_burstidx,1 nbyte_entry,2 burstidx_min,3 n_op) */
64,
1,
0,
1,
/* BurstProtocolCfg [0] Content (0 tmpl_reg,1 entry_lsb,2 reg_lsb,3 reg_mask_nbits)*/
0,
0,
8,
8,
/*  Gain Table Data BEGIN (total entry = 64 , byte entry = 1) */
0,1,2,3,4,5,6,7,8,9,
1,1,2,3,4,5,6,7,8,9,
2,1,2,3,4,5,6,7,8,9,
3,1,2,3,4,5,6,7,8,9,
4,1,2,3,4,5,6,7,8,9,
5,1,2,3,4,5,6,7,8,9,
6,1,2,3,
/* Gain Table Data END */
/* Error Table EvErrorCfg Content (0 n_eidx,1 eidx_min,2 err_unit)*/
0x0,
0x0,
0x40
/* Error Table Data BEGIN (total entry = 0 ) */
/* Error Table Data BEGIN (total entry = 0 ) */
/* Error Table END */
/* END */
};
#endif
void sensorAfe_tvp5150_Install()
{
#if 1
	sensorAfeDevInstall(SENSOR_DEV_ID_SENSOR,&sensorAfeDev);
#else
    sensorAfeDevInstall(SENSOR_DEV_ID_TV_IN,&sensorAfeDev);
#endif
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
sensor_afe_svc_init(
    void
)
{
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
}
static void
sensor_cmdport_dcoffset_write(
    UINT32 idx
)
{
}

