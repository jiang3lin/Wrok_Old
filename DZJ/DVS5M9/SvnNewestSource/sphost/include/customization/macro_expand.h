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

#include "common.h"

/**************************************************************************
 *                                _DEV_INIT_H_
 **************************************************************************/
#ifdef _DEV_INIT_H_
#ifndef _MACRO_EXPAND_H_DEV_
#define _MACRO_EXPAND_H_DEV_

/*_________________________________________________________________________

  audio codec.
 _________________________________________________________________________
*/
	/*
	  primary audio device, for SP5K_AUD_DEV_PRIMARY.
	*/
extern 	void audCodecHdmiAudRegister();

#if   (SP5K_AUD_DEVICE_INTERNAL == 1)
	#define audioCodecInit audCodecOnChipRegister
#elif (SP5K_AUD_DEVICE_HDMI == 1)
	#define audioCodecInit audCodecHdmiAudRegister
#elif (SP5K_AUD_DEVICE_WS8751 == 1)
	#define audioCodecInit audCodecWs8751Register
#endif

#ifdef audioCodecInit
void audioCodecInit(void);
#else
#error Please select audio CODEC in dev_init.h...
#endif

#if SPCA5330
	/*
	  secondary audio device, for SP5K_AUD_DEV_SECONDARY.
	*/
#if   (SP5K_AUD_DEVICE2_INTERNAL == 1)
	#define audioCodec2Init audCodecOnChipRegister
#elif (SP5K_AUD_DEVICE2_HDMI == 1)
	#define audioCodec2Init audCodecHdmiAudRegister
#elif (SP5K_AUD_DEVICE2_WS8751 == 1)
	#define audioCodec2Init audCodecWs8751Register
#endif

#ifdef audioCodec2Init
	void audioCodec2Init(void);
#else
#error Please select audio CODEC #2 in dev_init.h...
#endif
#endif


/*_________________________________________________________________________

  disp device.
 _________________________________________________________________________
*/
#if SPCA5210
/*
  auo.
*/
#if   (SP5K_LCD_A015AN04_UPS051)
	#define dispLcdInit() dispAuoA015an04Ups051Open()
	UINT32 dispAuoA015an04Ups051Open(void);
#elif (SP5K_LCD_A015AN04_UPS052)
	#define dispLcdInit() dispAuoA015an04Ups052Open()
	UINT32 dispAuoA015an04Ups052Open(void);

#elif (SP5K_LCD_A020BL01_UPS052)
	#define dispLcdInit() dispAuoA020BL01U052Open()
	UINT32 dispAuoA020BL01U052Open(void);

#elif (SP5K_LCD_A025BN01_UPS051)
	#define dispLcdInit() dispAuoA025bn01Ups051Open()
	UINT32 dispAuoA025bn01Ups051Open(void);
#elif (SP5K_LCD_A025BN01_UPS052)
	#define dispLcdInit() dispAuoA025bn01Ups052W320Open()
	UINT32 dispAuoA025bn01Ups052W320Open(void);
#elif (SP5K_LCD_A025BN01_CCIR656_W640)
	#define dispLcdInit() dispAuoA025bn01Ccir656W640Open()
	UINT32 dispAuoA025bn01Ccir656W640Open(void);
#elif (SP5K_LCD_A025BN01_YUV_W640)
	#define dispLcdInit() dispAuoA025bn01YuvW640Open()
	UINT32 dispAuoA025bn01YuvW640Open(void);
#elif (SP5K_LCD_A025BN01_YUV_W720)
	#define dispLcdInit() dispAuoA025bn01YuvW720Open()
		UINT32 dispAuoA025bn01YuvW720Open(void);


#elif (SP5K_LCD_A025DL01_UPS052)
	#define dispLcdInit() dispAuoA025DL01U052Open()
	UINT32 dispAuoA025DL01U052Open(void);

#elif (SP5K_LCD_A027DN01_UPS052)
	#define dispLcdInit() dispAuoA027dn01Ups052Open()
	UINT32 dispAuoA027dn01Ups052Open();

#elif (SP5K_LCD_A030DL01_UPS052)
	#define dispLcdInit() dispAuoA030DL01U052Open()
	UINT32 dispAuoA030DL01U052Open(void);

/*
  Tianma.
*/
#elif (SP5K_LCD_TS027CAARD02_UPS052)
	#define dispLcdInit() dispTMATS027CAARD02Ups052Open()
	UINT32 dispTMATS027CAARD02Ups052Open();

/*
  toppoly.
*/
#elif (SP5K_LCD_TD025THEB1_UPS052)
	#define dispLcdInit() dispTpTd025theb1Ups052Open()
	UINT32 dispTpTd025theb1Ups052Open(void);

#elif (SP5K_LCD_TD027LCJT02_UPS052)
	#define dispLcdInit() dispTpLcj027T002Ups052Open()
	UINT32 dispTpLcj027T002Ups052Open(void);

#elif (SP5K_LCD_GP024G48238QS5_UPS052)
	#define dispLcdInit() dispGp024G48238QS5Ups052Open()
	UINT32 dispGp024G48238QS5Ups052Open(void);

/*s Add by Aries 10/02/04*/
#elif (SP5K_LCD_TD020THEG1_UPS052)
	#define dispLcdInit() dispTpTd020theg1Ups052Open()
	UINT32 dispTpTd020theg1Ups052Open(void);

/*e Add by Aries 10/02/04*/

#elif (SP5K_LCD_TD025THEEA_UPS052)
	#define dispLcdInit() dispTpTd025theeaUps052Open()
	UINT32 dispTpTd025theeaUps052Open(void);

/*e Add by CS 11/12/13 */
#elif (SP5K_LCD_TXDT250C_UPS052)
	#define dispLcdInit() dispTxdTxdt025cUps052Open()
	UINT32 dispTxdTxdt025cUps052Open(void);

/*e Add by Careyliao 12/03/13 */
#elif (SP5K_LCD_WDF9648W7FLWA_UPS052)
	#define dispLcdInit() dispWtWdf9648w7flwaUps052Open()
	UINT32 dispWtWdf9648w7flwaUps052Open(void);

#elif (SP5K_LCD_WDF9648W7FLWA_UPS051)
	#define dispLcdInit() dispWtWdf9648w7flwaUps051Open()
	UINT32 dispWtWdf9648w7flwaUps051Open(void);

/*e Add by CS 12/05/24 */
#elif (SP5K_LCM_TXDT240TQ_LCM_H)
	#define dispLcdInit() dispTxdt240LcmHOpen()
	UINT32 dispTxdt240LcmHOpen(void);
#elif (SP5K_LCM_TXDT240TQ_LCM_V)
	#define dispLcdInit() dispTxdt240LcmVOpen()
		UINT32 dispTxdt240LcmVOpen(void);
	
/*s Add by xuan.ruan 13/10/12*/
#elif (SP5K_LCM_DJN_ILI9225)
	#define dispLcdInit() dispILI9225LcmOpen()
	UINT32 dispILI9225LcmOpen(void);

/*s Add by wujf 14/05/07*/
#elif (SP5K_LCM_VSN_M01120)
	#define dispLcdInit() dispM01120LcmOpen()
	UINT32 dispM01120LcmOpen(void);


/*e Add by xuan.ruan 13/10/12 */

#elif (SP5K_LCD_DUMMY)
	#define dispLcdInit() dispDummyOpen()
	UINT32 dispDummyOpen(void);


#else
#error Please select LCD type in dev_init.h...
#endif

/*
  tv ntsc.
*/
#define dispTvNtscOpen  dispCompositeTvNtscOpen  /* run-time setting resolution by sp5kDispResolutionSet(). */

/*
  tv pal.
*/
#define dispTvPalOpen  dispCompositeTvPalOpen  /* run-time setting resolution by sp5kDispResolutionSet(). */

/*
  HDMI.
*/
#define dispHdmiOpen  dispHdmiTvOpen  /* run-time setting resolution by sp5kDispResolutionSet(). */

/*s Add by Aries 10/05/18*/
#if SPCA5310
/*#undef dispHdmiOpen*/

#if   (SP5K_HDMI_ANA_ANX7150)
	#define dispHdmiOpen() dispAnaAnx7150Open()
	UINT32 dispAnaAnx7150Open(void);
#elif (SP5K_HDMI_EP_EP932M)
	#define dispHdmiOpen() dispEP932mOpen()
	UINT32 dispEP932mOpen(void);
#else
/*#error Please select TV HDMI type in dev_init.h...*/
#endif

#endif
/*e Add by Aries 10/05/18*/

#else /* SPCA5110 */

#if   (SP5K_LCD_A015AN04_UPS051) /*FIXME*/ /*SPI/I2C*/
	#define dispLcdInit() dispAuoA015AN04U051Open()
	UINT32 dispAuoA015AN04U051Open(void);
#elif (SP5K_LCD_A015AN04_UPS052)
	#define dispLcdInit() dispAuoA015AN04U052Open()
	UINT32 dispAuoA015AN04U052Open(void);
#elif (SP5K_LCD_A020BL01_UPS052)
	#define dispLcdInit() dispAuoA020BL01U052Open()
	UINT32 dispAuoA020BL01U052Open(void);
#elif (SP5K_LCD_A025BN01_UPS052)
	#define dispLcdInit() dispAuoA025BN01U052Open()
	UINT32 dispAuoA025BN01U052Open(void);
#elif (SP5K_LCD_A025DL01_UPS052)
	#define dispLcdInit() dispAuoA025DL01U052Open()
	UINT32 dispAuoA025DL01U052Open(void);
#elif (SP5K_LCD_A027DN01_UPS052)
	#define dispLcdInit() dispAuoA027DN01U052Open()
	UINT32 dispAuoA027DN01U052Open();
#elif (SP5K_LCD_A030DL01_UPS052)
	#define dispLcdInit() dispAuoA030DL01U052Open()
	UINT32 dispAuoA030DL01U052Open(void);
#elif (SP5K_LCD_TD025THEB1_UPS052)
	#define dispLcdInit() dispTpTD025THEB1U052Open()
	UINT32 dispTpTD025THEB1U052Open(void);
#else
#error Please select LCD type in dev_init.h...
#endif

#endif /* SPCA5110 */

/*_________________________________________________________________________

  front: SNAP MSHUTTER.
 _________________________________________________________________________
*/
#if SP5K_SNAP_SHUTTER_SUPPORT
 void sensorMshInit(void);
#else
 #define sensorMshInit()
#endif

/*s Add by Aries 10/05/12*/
#if SPCA5310
#if SMEAR_REDUCT_SUPPORT
 extern void smearPvModeInit(void);
#else
 #define smearPvModeInit()
#endif
#endif
/*e Add by Aries 10/05/12*/


/*_________________________________________________________________________

  front: selection of sensor/TG, AFE.
 _________________________________________________________________________
*/
#if SP5K_SENSOR_OV7740_YUV
	#define devFrontTgInstall()  sensorTg_ov7740_yuv_Install()
	#define devFrontAfeInstall() sensorAfe_ov7740_yuv_Install()
				void sensorTg_ov7740_yuv_Install(void);
				void sensorAfe_ov7740_yuv_Install(void);
#endif


#if SP5K_SENSOR_MI1320
	#define devFrontTgInstall()  sensorTg_mi1320_Install()
	#define devFrontAfeInstall() sensorAfe_mi1320_Install()
			void sensorTg_mi1320_Install(void);
			void sensorAfe_mi1320_Install(void);
#endif


#if SP5K_SENSOR_OV7740
	#define devFrontTgInstall()  sensorTg_ov7740_Install()
	#define devFrontAfeInstall() sensorAfe_ov7740_Install()
		void sensorTg_ov7740_Install(void);
		void sensorAfe_ov7740_Install(void);
#endif


#if SP5K_SENSOR_MI1600
	#define devFrontTgInstall()  sensorTg_mi1600_Install()
	#define devFrontAfeInstall() sensorAfe_mi1600_Install()
		void sensorTg_mi1600_Install(void);
		void sensorAfe_mi1600_Install(void);
#endif

#if SP5K_SENSOR_TT3131
	#define devFrontTgInstall()  sensorTg_tt3131_Install()
	#define devFrontAfeInstall() sensorAfe_tt3131_Install()
	void sensorTg_tt3131_Install(void);
	void sensorAfe_tt3131_Install(void);
#endif
#if SP5K_SENSOR_SONYIMX175
	#define devFrontTgInstall()  sensorTg_sonyimx175_Install()
	#define devFrontAfeInstall() sensorAfe_sonyimx175_Install()
	void sensorTg_sonyimx175_Install(void);
	void sensorAfe_sonyimx175_Install(void);
#endif
#if SP5K_SENSOR_OV2610
	#define devFrontTgInstall()  sensorTg_ov2610_Install()
	#define devFrontAfeInstall() sensorAfe_ov2610_Install()
	void sensorTg_ov2610_Install(void);
	void sensorAfe_ov2610_Install(void);
#endif
#if SP5K_SENSOR_OV5610
	#define devFrontTgInstall()  sensorTg_ov5610_Install()
	#define devFrontAfeInstall() sensorAfe_ov5610_Install()
	void sensorTg_ov5610_Install(void);
	void sensorAfe_ov5610_Install(void);
#endif
#if SP5K_SENSOR_OV5620
	#define devFrontTgInstall()  sensorTg_ov5620_Install()
	#define devFrontAfeInstall() sensorAfe_ov5620_Install()
	void sensorTg_ov5620_Install(void);
	void sensorAfe_ov5620_Install(void);
#endif

#if SP5K_SENSOR_OV5653
	#define devFrontTgInstall()  sensorTg_ov5653_Install()
	#define devFrontAfeInstall() sensorAfe_ov5653_Install()
	void sensorTg_ov5653_Install(void);
	void sensorAfe_ov5653_Install(void);
#endif

#if SP5K_SENSOR_OV9810
	#define devFrontTgInstall()  sensorTg_ov9810_Install()
	#define devFrontAfeInstall() sensorAfe_ov9810_Install()
	void sensorTg_ov9810_Install(void);
	void sensorAfe_ov9810_Install(void);
#endif

#if SP5K_SENSOR_OV16820
	#define devFrontTgInstall()  sensorTg_ov16820_Install()
	#define devFrontAfeInstall() sensorAfe_ov16820_Install()
	void sensorTg_ov16820_Install(void);
	void sensorAfe_ov16820_Install(void);
#endif

#if SP5K_SENSOR_OV8810
	#define devFrontTgInstall()  sensorTg_ov8810_Install()
	#define devFrontAfeInstall() sensorAfe_ov8810_Install()
	void sensorTg_ov8810_Install(void);
	void sensorAfe_ov8810_Install(void);
#endif

#if SP5K_SENSOR_OV8825
	#define devFrontTgInstall()  sensorTg_ov8825_Install()
	#define devFrontAfeInstall() sensorAfe_ov8825_Install()
	void sensorTg_ov8825_Install(void);
	void sensorAfe_ov8825_Install(void);
#endif

#if SP5K_SENSOR_OV8830
	#define devFrontTgInstall()  sensorTg_ov8830_Install()
	#define devFrontAfeInstall() sensorAfe_ov8830_Install()
	void sensorTg_ov8830_Install(void);
	void sensorAfe_ov8830_Install(void);
#endif

#if SP5K_SENSOR_OV8833
	#define devFrontTgInstall()  sensorTg_ov8833_Install()
	#define devFrontAfeInstall() sensorAfe_ov8833_Install()
	void sensorTg_ov8833_Install(void);
	void sensorAfe_ov8833_Install(void);
#endif

#if SP5K_SENSOR_OV9714
	#define devFrontTgInstall()  sensorTg_ov9714_Install()
	#define devFrontAfeInstall() sensorAfe_ov9714_Install()
	void sensorTg_ov9714_Install(void);
	void sensorAfe_ov9714_Install(void);
#endif

#if SP5K_SENSOR_PANAMN34110
	#define devFrontTgInstall()  sensorTg_PANAMN34110_Install()
	#define devFrontAfeInstall() sensorAfe_PANAMN34110_Install()
	void sensorTg_PANAMN34110_Install(void);
	void sensorAfe_PANAMN34110_Install(void);
#endif

#if SP5K_SENSOR_SONYIMX083
	#define devFrontTgInstall()  sensorTg_SONYIMX083_Install()
	#define devFrontAfeInstall() sensorAfe_SONYIMX083_Install()
	void sensorTg_SONYIMX083_Install(void);
	void sensorAfe_SONYIMX083_Install(void);
#endif

#if SP5K_SENSOR_SONYIMX117
	#define devFrontTgInstall()  sensorTg_SONYIMX117_Install()
	#define devFrontAfeInstall() sensorAfe_SONYIMX117_Install()
	void sensorTg_SONYIMX117_Install(void);
	void sensorAfe_SONYIMX117_Install(void);
#endif

#if SP5K_SENSOR_OV5633
	#define devFrontTgInstall()  sensorTg_ov5633_Install()
	#define devFrontAfeInstall() sensorAfe_ov5633_Install()
	void sensorTg_ov5633_Install(void);
	void sensorAfe_ov5633_Install(void);
#endif
#if SP5K_SENSOR_OV9712
	#define devFrontTgInstall()  sensorTg_ov9712_Install()
	#define devFrontAfeInstall() sensorAfe_ov9712_Install()
	void sensorTg_ov9712_Install(void);
	void sensorAfe_ov9712_Install(void);
#endif
#if SP5K_SENSOR_OV2715
	#define devFrontTgInstall()  sensorTg_ov2715_Install()
	#define devFrontAfeInstall() sensorAfe_ov2715_Install()
	void sensorTg_ov2715_Install(void);
	void sensorAfe_ov2715_Install(void);
#endif

#if SP5K_SENSOR_MI5100
	#define devFrontTgInstall()  sensorTg_mi5100_Install()
	#define devFrontAfeInstall() sensorAfe_mi5100_Install()
	void sensorTg_mi5100_Install(void);
	void sensorAfe_mi5100_Install(void);
#endif
#if SP5K_SENSOR_MI5100p
	#define devFrontTgInstall()  sensorTg_mi5100p_Install()
	#define devFrontAfeInstall() sensorAfe_mi5100p_Install()
	void sensorTg_mi5100p_Install(void);
	void sensorAfe_mi5100p_Install(void);
#endif
#if SP5K_SENSOR_MI9130
	#define devFrontTgInstall()  sensorTg_mi9130_Install()
	#define devFrontAfeInstall() sensorAfe_mi9130_Install()
	void sensorTg_mi9130_Install(void);
	void sensorAfe_mi9130_Install(void);
#endif
#if SP5K_SENSOR_MI5130
	#define devFrontTgInstall()  sensorTg_mi5130_Install()
	#define devFrontAfeInstall() sensorAfe_mi5130_Install()
	void sensorTg_mi5130_Install(void);
	void sensorAfe_mi5130_Install(void);
#endif
#if SP5K_SENSOR_MT9J003
#define devFrontTgInstall()  sensorTg_mt9J003_Install()
#define devFrontAfeInstall() sensorAfe_mt9J003_Install()
	void sensorTg_mt9J003_Install(void);
	void sensorAfe_mt9J003_Install(void);
#endif
#if SP5K_SENSOR_AR0330
#define devFrontTgInstall()  sensorTg_ar0330_Install()
#define devFrontAfeInstall() sensorAfe_ar0330_Install()
	void sensorTg_ar0330_Install(void);
	void sensorAfe_ar0330_Install(void);
#endif

#if SP5K_SENSOR_AR0130
#define devFrontTgInstall()  sensorTg_ar0130_Install()
#define devFrontAfeInstall() sensorAfe_ar0130_Install()
	void sensorTg_ar0130_Install(void);
	void sensorAfe_ar0130_Install(void);
#endif

#if SP5K_SENSOR_AR0130_DUAL
#define devFrontTgInstall()  sensorTg_dual_ar0130_Install()
#define devFrontAfeInstall() sensorAfe_dual_ar0130_Install()
	void sensorTg_dual_ar0130_Install(void);
	void sensorAfe_dual_ar0130_Install(void);
#endif

#if SP5K_SENSOR_AR0330CS
#define devFrontTgInstall()  sensorTg_ar0330cs_Install()
#define devFrontAfeInstall() sensorAfe_ar0330cs_Install()
	void sensorTg_ar0330cs_Install(void);
	void sensorAfe_ar0330cs_Install(void);
#endif

#if SP5K_SENSOR_AR0330CS_PLCC
#define devFrontTgInstall()  sensorTg_ar0330cs_plcc_Install()
#define devFrontAfeInstall() sensorAfe_ar0330cs_Install()
	void sensorTg_ar0330cs_plcc_Install(void);
	void sensorAfe_ar0330cs_Install(void);
#endif

#if SP5K_SENSOR_AR0331
#define devFrontTgInstall()  sensorTg_ar0331_Install()
#define devFrontAfeInstall() sensorAfe_ar0331_Install()
	void sensorTg_ar0331_Install(void);
	void sensorAfe_ar0331_Install(void);
#endif
#if SP5K_SENSOR_TG_PANX12067_MN39830
	#define devFrontTgInstall()  sensorTg_panx12067_mn39830_Install()
	void sensorTg_panx12067_mn39830_Install(void);
#endif
#if SP5K_SENSOR_AFE_PANX12067
	#define devFrontAfeInstall()  sensorAfe_panx12067_Install()
	void sensorAfe_panx12067_Install(void);
#endif
#if SP5K_SENSOR_TG_PANX12069_MN39830
	#define devFrontTgInstall()  sensorTg_panx12069_mn39830_Install()
	void sensorTg_panx12069_mn39830_Install(void);
#endif
#if SP5K_SENSOR_TG_PANX12069_MN39600
	#define devFrontTgInstall()  sensorTg_panx12069_mn39600_Install()
	void sensorTg_panx12069_mn39600_Install(void);
#endif
#if SP5K_SENSOR_AFE_PANX12069
	#define devFrontAfeInstall()  sensorAfe_panx12069_Install()
	void sensorAfe_panx12069_Install(void);
#endif
#if SP5K_SENSOR_TG_CXD4805GA_SONY624
	#define devFrontTgInstall()  sensorTg_cxd4805ga_sony624_Install()
	void sensorTg_cxd4805ga_sony624_Install(void);
#endif
#if SP5K_SENSOR_AFE_CXD4805GA
	#define devFrontAfeInstall()  sensorAfe_cxd4805ga_Install()
	void sensorAfe_cxd4805ga_Install(void);
#endif
#if SP5K_SENSOR_TG_CXD3626GA_SONY455
	#define devFrontTgInstall()  sensorTg_cxd3626ga_sony455_Install()
	void sensorTg_cxd3626ga_sony455_Install(void);
#endif
#if SP5K_SENSOR_AFE_AD9943
	#define devFrontAfeInstall()  sensorAfe_ad9943_Install()
	void sensorAfe_ad9943_Install(void);
#endif
#if SP5K_SENSOR_TG_AD9923_SONY629
	#define devFrontTgInstall()  sensorTg_ad9923_sony629_Install()
	void sensorTg_ad9923_sony629_Install(void);
#endif
#if SP5K_SENSOR_AFE_AD9923
	#define devFrontAfeInstall()  sensorAfe_ad9923_Install()
	void sensorAfe_ad9923_Install(void);
#endif
#if SP5K_SENSOR_TG_AD9920_SONY636
	#define devFrontTgInstall()  sensorTg_ad9920_sony636_Install()
	void sensorTg_ad9920_sony636_Install(void);
#endif
#if SP5K_SENSOR_AFE_AD9000
	#define devFrontAfeInstall()  sensorAfe_ad9000_Install()
	void sensorAfe_ad9000_Install(void);
#endif
#if SP5K_SENSOR_AFE_AD9920
	#define devFrontAfeInstall()  sensorAfe_ad9920_Install()
	void sensorAfe_ad9920_Install(void);
#endif
#if SP5K_SENSOR_AFE_PANX12081A
	#define devFrontAfeInstall()  sensorAfe_panx12081a_Install()
	void sensorAfe_panx12081a_Install(void);
#endif
#if SP5K_SENSOR_TG_AD9920_SONY646
	#define devFrontTgInstall()  sensorTg_ad9920_sony646_Install()
	void sensorTg_ad9920_sony646_Install(void);
#endif
#if SP5K_SENSOR_TG_AD9920_SHARPRJ23T3A
	#define devFrontTgInstall()  sensorTg_ad9920_sharprj23t3a_Install()
	void sensorTg_ad9920_sharprj23t3a_Install(void);
#endif
#if SP5K_SENSOR_TG_AD9920_SHARPRJ23V3B
	#define devFrontTgInstall()  sensorTg_ad9920_sharprj23v3b_Install()
	void sensorTg_ad9920_sharprj23v3b_Install(void);
#endif
#if SP5K_SENSOR_TG_AD9920_SHARPRJ23U3B
	#define devFrontTgInstall()  sensorTg_ad9920_sharprj23u3b_Install()
	void sensorTg_ad9920_sharprj23u3b_Install(void);
#endif
#if SP5K_SENSOR_TG_AD9000_SHARPRJ23V3B
	#define devFrontTgInstall()  sensorTg_ad9000_sharprj23v3b_Install()
	void sensorTg_ad9000_sharprj23v3b_Install(void);
#endif
#if SP5K_SENSOR_TG_AD9000_SHARPRJ23U3B
	#define devFrontTgInstall()  sensorTg_ad9000_sharprj23u3b_Install()
	void sensorTg_ad9000_sharprj23u3b_Install(void);
#endif
#if SP5K_SENSOR_TG_PANX12081A_MN34510
	#define devFrontTgInstall()  sensorTg_panx12081a_mn34510_Install()
	void sensorTg_panx12081a_mn34510_Install(void);
#endif
#if SP5K_SENSOR_TG_AD9000_SONY675
	#define devFrontTgInstall()  sensorTg_ad9000_sony675_Install()
	void sensorTg_ad9000_sony675_Install(void);
#endif

#if SP5K_SENSOR_MT9F002
	#define devFrontTgInstall()  sensorTg_mt9F002_Install()
	#define devFrontAfeInstall() sensorAfe_mt9F002_Install()
	void sensorTg_mt9F002_Install(void);
	void sensorAfe_mt9F002_Install(void);
#endif

#if SP5K_SENSOR_OV9713
#define devFrontTgInstall()  sensorTg_ov9713_Install()
#define devFrontAfeInstall() sensorAfe_ov9713_Install()
	void sensorTg_ov9713_Install(void);
	void sensorAfe_ov9713_Install(void);
#endif

#if SP5K_SENSOR_TVP5150  /* SP5K_TVINDEC_TVP5150 */
#define devFrontTgInstall()  sensorTg_tvp5150_Install()
#define devFrontAfeInstall() sensorAfe_tvp5150_Install()
	void sensorTg_tvp5150_Install(void);
	void sensorAfe_tvp5150_Install(void);
#endif

#if SP5K_SENSOR_TG_CUSTOM
	/*put custom sensor TG driver installation here
	#define devFrontTgInstall()  sensorTg_custom_xxx_yyy_Install()
	void sensorTg_custom_xxx_yyy_Install(void);
	*/
#endif
#if SP5K_SENSOR_AFE_CUSTOM
	/*put custom sensor AFE driver installation here
	#define devFrontAfeInstall()  sensorAfe_custom_xxx_yyy_Install()
	void sensorAfe_custom_xxx_yyy_Install(void);
	*/
#endif
#if SP5K_TVIN_DEC_SAA7113
	#define devFrontTvInDecInstall()  tvInDec_Saa7113_Install()
	void tvInDec_Saa7113_Install(void);
#endif
#if SP5K_TVIN_DEC_TVP5150
	#define devFrontTvInDecInstall()  do { \
			sensorDoFuncStopOutput(); \
			tvInDec_Tvp5150_Install(); \
		} while (0)
	void tvInDec_Tvp5150_Install(void);
	void sensorDoFuncStopOutput(void);
#endif

#ifndef devFrontTgInstall
 #define devFrontTgInstall()
#endif
#ifndef devFrontAfeInstall
 #define devFrontAfeInstall()
#endif
#ifndef devFrontTvInDecInstall
 #define devFrontTvInDecInstall()
#endif

#endif /*_MACRO_EXPAND_H_DEV_*/
#endif /*_DEV_INIT_H_*/


/**************************************************************************
 *                                _APP_INIT_H_
 **************************************************************************/
#ifdef _APP_INIT_H_
#ifndef _MACRO_EXPAND_H_APP_
#define _MACRO_EXPAND_H_APP_

/*_________________________________________________________________________

  command.c
 _________________________________________________________________________
*/
#if (CMD_MEM == 0)
#define cmdMemInit()
#endif
#if (CMD_OS == 0)
#define cmdOsInit()
#endif
#if (CMD_DOS == 0)
#define cmdDosInit()
#endif
#if (CMD_DISK == 0)
#define cmdDiskInit()
#endif
#if (CMD_ERR == 0)
#define cmdErrInit()
#endif
#if (CMD_MEDIA == 0)
#define cmdMediaInit()
#endif
#if (CMD_ISP == 0)
#define cmdIspInit()
#endif
#if (CMD_HW == 0)
#define cmdHwInit()
#endif
#if (CMD_NAND == 0)
#define cmdNandInit()
#endif
#if (CMD_GDB == 0)
#define cmdGdbInit()
#endif

/*_________________________________________________________________________

  app_init.c
 _________________________________________________________________________
*/
#if CMD_TASK_INIT
 extern void cmdTaskInit(void);
#else
 #undef cmdTaskInit
 #define cmdTaskInit()
#endif

/*_________________________________________________________________________

  media: decoders.
 _________________________________________________________________________
*/
#if MEDIA_DEC_MJPG
 void mjpgDecoderRegister(void);
#else
 #define mjpgDecoderRegister()
#endif

#if MEDIA_DEC_PCM
 void pcmDecoderRegister(void);
#else
 #define pcmDecoderRegister()
#endif

#if MEDIA_DEC_ULAW
 void mulawDecoderRegister(void);/* FIXME: modified by Edward because mulaw audio is using in ui_AudioMain.c */
#else
 #define mulawDecoderRegister()
#endif

#if MEDIA_DEC_ALAW
 UINT32 alawDecoderAdd(void);
#else
 #define alawDecoderAdd()
#endif

#if MEDIA_DEC_ADPCM
 void adpcmDecoderRegister(void);
#else
 #define adpcmDecoderRegister()
#endif

#if MEDIA_DEC_AAC
 void aacDecoderRegister(void);
#else
 #define aacDecoderRegister()
#endif

#if MEDIA_DEC_MP3
 void mp3DecoderRegister(void);
#else
 #define mp3DecoderRegister()
#endif

#if MEDIA_DEC_WMA
 UINT32 wmaDecoderAdd(void);
#else
 #define wmaDecoderAdd()
#endif

/*_________________________________________________________________________

  media: encoders.
 _________________________________________________________________________
*/
#if MEDIA_ENC_MJPG
 void mjpgEncoderRegister(void);
#else
 #define mjpgEncoderRegister()
#endif

#if MEDIA_ENC_PCM
 void pcmEncoderRegister(void);
#else
 #define pcmEncoderRegister()
#endif

#if MEDIA_ENC_ULAW
 void mulawEncoderRegister(void);/* FIXME: modified by Edward because mulaw audio is using in ui_AudioMain.c */
#else
 #define mulawEncoderRegister()
#endif

#if MEDIA_ENC_ALAW
 UINT32 alawEncoderAdd(void);
#else
 #define alawEncoderAdd()
#endif

#if MEDIA_ENC_ADPCM
 void adpcmEncoderRegister(void);
#else
 #define adpcmEncoderRegister()
#endif

#if MEDIA_ENC_AAC
 void aacEncoderRegister(void);
#else
 #define aacEncoderRegister()
#endif

/*_________________________________________________________________________

  media: mux recorders.
 _________________________________________________________________________
*/
#if MEDIA_RECORD_AVI
 UINT32 mediaAviRecorderInit(void);     /* support record Avi format */
#else
 #define mediaAviRecorderInit()
#endif

#if MEDIA_RECORD_ASF
 UINT32 mediaAsfRecorderInit(void);     /* support record Asf format */
#else
 #define mediaAsfRecorderInit()
#endif

#if MEDIA_RECORD_WAV
 UINT32 mediaWavRecorderInit(void);     /* support record Wav format */
#else
 #define mediaWavRecorderInit()
#endif

#if MEDIA_RECORD_MP3
 UINT32 mediaMp3RecorderInit(void);     /* support record Mp3 format */
#else
 #define mediaMp3RecorderInit()
#endif

#if MEDIA_RECORD_MOV || MEDIA_RECORD_MP4
 UINT32 mediaMovRecorderInit(void);     /* support record mov format */
#else
 #define mediaMovRecorderInit()
#endif

/*_________________________________________________________________________

  media: demux players.
 _________________________________________________________________________
*/
#if MEDIA_PLAY_AVI
 UINT32 mediaAviPlayerInit(void);       /* support play Avi format */
#else
 #define mediaAviPlayerInit()
#endif

#if MEDIA_PLAY_ASF
 UINT32 mediaAsfPlayerInit(void);       /* support play Asf format */
#else
 #define mediaAsfPlayerInit()
#endif

#if MEDIA_PLAY_WAV
 UINT32 mediaWavPlayerInit(void);       /* support play Wav format */
#else
 #define mediaWavPlayerInit()
#endif

#if MEDIA_PLAY_MP3
 UINT32 mediaMp3PlayerInit(void);       /* support play Mp3 format */
#else
 #define mediaMp3PlayerInit()
#endif

#if MEDIA_PLAY_WMA
 UINT32 mediaWmaPlayerInit(void);       /* support play Wma format */
#else
 #define mediaWmaPlayerInit()
#endif

#if MEDIA_PLAY_MOV || MEDIA_PLAY_MP4
 UINT32 mediaMovPlayerInit(void);       /* support play Mov format */
#else
 #define mediaMovPlayerInit()
#endif
/*_________________________________________________________________________

  sunplus default ae / awb.
 _________________________________________________________________________
*/
#if AE_DEFAULT_SUPPORT
 extern void aeDefaultInit(void);
#else
 #define aeDefaultInit()
#endif

#if AWB_DEFAULT_SUPPORT
 extern void awbDefaultInit(void);
#else
 #define awbDefaultInit()
#endif

/* -- Sunplus DIST support -- */
#if STILL_DIST_SUPPORT
 extern void stillDistInit(void);
#else
 #define stillDistInit()
#endif
#if VIDEO_DIST_SUPPORT
 extern void videoDistInit(void);
#else
 #define videoDistInit()
#endif

/* -- Sunplus face tracking support -- */
#if FACE_TRACK_PREVIEW_SUPPORT
 extern void ftrackPvInit(void);
#else
 #define ftrackPvInit()
#endif

#if FACE_TRACK_SNAP_SUPPORT
 extern void ftrackSnapInit(void);
#else
 #define ftrackSnapInit()
#endif

#if FACE_TRACK_PLAYBACK_SUPPORT
 extern void ftrackPbInit(void);
#else
 #define ftrackPbInit()
#endif

#if FACE_TRACK_TINY_FACE_DETECT
 extern void ftrackPvSoftIIModuleAdd(void);
#else
 #define ftrackPvSoftIIModuleAdd()
#endif

#if FACE_TRACK_TINY_SMILE_DETECT || FACE_TRACK_TINY_BLINK_DETECT || \
(!FACE_TRACK_TINY_FACE_DETECT && \
(FACE_TRACK_SMILE_DETECT || FACE_TRACK_BLINK_DETECT))
 extern void ftrackFDSoftIIModuleAdd(void);
#else
 #define ftrackFDSoftIIModuleAdd()
#endif

#if FACE_TRACK_CHECK_FACE_COLOR
 extern void ftrackFDSoftUVModuleAdd(void);
#else
 #define ftrackFDSoftUVModuleAdd()
#endif


/*_________________________________________________________________________

  misc.
 _________________________________________________________________________
*/
extern void yuvFilterHDRInit(void);



#if XDENOISE_SUPPORT
 extern void xdenoiseInit(void);
#else
 #define xdenoiseInit()
#endif

#if CALIB_BP_SUPPORT
 extern void sp5kCalBpInit(void);
#else
 #define sp5kCalBpInit()
#endif

#if CALIB_LSC_SUPPORT
 extern void sp5kCalLscInit(void);
#else
 #define sp5kCalLscInit()
#endif

#if PANORAMA_SUPPORT
 extern void panoramaInit(void);
#else
 #define panoramaInit()
#endif

#define COMPRESSED_RES 1
#if COMPRESSED_RES
 extern void resCoderInit(void);
#else
 #define resCoderInit()
#endif

#endif /*_MACRO_EXPAND_H_APP_*/
#endif  /* _APP_INIT_H_ */

