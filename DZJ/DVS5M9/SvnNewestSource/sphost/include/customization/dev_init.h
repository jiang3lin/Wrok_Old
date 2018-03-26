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
#ifndef _DEV_INIT_H_
#define _DEV_INIT_H_

#include "customization/solution_def.h"

/*_________________________________________________________________________

  DDR2 support is controlled by makefile by config.sh
 _________________________________________________________________________
*/

#if SPCA5110 && !SPCA5310 && !SPCA5110
 /* CPU CLK = 131.5MHz */
 #define CPU_CLK         1
#endif


#define DRAM_SIZE	0x4000000

/*_________________________________________________________________________

  NAND timing.
 _________________________________________________________________________
*/

/*_________________________________________________________________________

  power on battery voltage detecting workaround.
 _________________________________________________________________________
*/
/*DO NOT DEFINE THIS IF YOUR APPLICATION NEEDS NO SUCH WORKAROUND*/
#undef POCU_SAR0_LOWBAT_CHECK_WORKAROUND_THRESHOLD_VALUE
/* #define POCU_SAR0_LOWBAT_CHECK_WORKAROUND_THRESHOLD_VALUE  600 */
/* sar0 threshold value range 0 .. +1023 */

/*_________________________________________________________________________

  RTC
 _________________________________________________________________________
*/

#define RTC_RELIABLE_CODE	0x5a
#define RTC_TIMELAPSE_WAKEUP	0x5d

#ifndef __ASSEMBLER__

#include "api/sp5k_global_api.h"

/*******************************************************************************
*                              C O N S T A N T S
*******************************************************************************/
/*_________________________________________________________________________

  storage media.
 _________________________________________________________________________
*/
#if HW_DISK_NAND
#define SP5K_DISK_NAND            1
#else
#define SP5K_DISK_NAND            0
#endif
#define SP5K_DISK_SD              1
#define SP5K_DISK_OTG             0


#define SP5K_DISK_EMMC            0
#if SP5K_DISK_EMMC
#undef SP5K_DISK_NAND
#define SP5K_DISK_NAND            1
#endif

#define SP5K_DISK_CF_IDE          0
#define SP5K_DISK_CF_MEM          0
#define SP5K_DISK_MS              0
#define SP5K_DISK_RAM             0
#define SP5K_DISK_XD              0
#if HW_DISK_SPI
#define SP5K_DISK_SPI             1
#else
#define SP5K_DISK_SPI             0
#endif
#define SP5K_DISK_TOTAL           (SP5K_DISK_NAND + SP5K_DISK_SD + \
                                  SP5K_DISK_CF_IDE + SP5K_DISK_CF_MEM + \
                                  SP5K_DISK_MS + SP5K_DISK_RAM + SP5K_DISK_XD+ SP5K_DISK_SPI+SP5K_DISK_OTG)

/* SD gpio pin define
 *
 * SD_DETECT_XXX : for SD card detect pin
 * SD_PWREN_XXX  : for SD power on pin
 * SD_WRP_XXX    : for SD write protect pin
 *   XXX_GRP     : GPIO group type in sp5kGpioGrp_t
 *   XXX_ID      : GPIO pin number
 *   XXX_POL     : GPIO polarity in sp5kDiskGpioPolarity_t
 */
#if SPCA6330
#ifdef HW_PRJ_6330
#define SP5K_SD_DETECT_GRP        SP5K_GPIO_GRP_GEN
#define SP5K_SD_DETECT_ID         15
#define SP5K_SD_DETECT_POL        SP5K_GPIO_ACTIVE_LOW
#else
#if SPCA_6330M
#define SP5K_SD_DETECT_GRP        SP5K_GPIO_GRP_GEN
#define SP5K_SD_DETECT_ID         15
#define SP5K_SD_DETECT_POL        SP5K_GPIO_ACTIVE_LOW
#else
#define SP5K_SD_DETECT_GRP        SP5K_GPIO_GRP_SAR
#define SP5K_SD_DETECT_ID         7
#define SP5K_SD_DETECT_POL        SP5K_GPIO_ACTIVE_LOW
#endif

#endif
#else
#define SP5K_SD_DETECT_GRP        SP5K_GPIO_GRP_FMH
#define SP5K_SD_DETECT_ID         (46-32)
#define SP5K_SD_DETECT_POL        SP5K_GPIO_ACTIVE_LOW
#endif
#define SP5K_SD_PWREN_GRP         SP5K_GPIO_GRP_FMH
#define SP5K_SD_PWREN_ID          (47-32)
#define SP5K_SD_PWREN_POL         SP5K_GPIO_ACTIVE_HIGH

#define SP5K_SD_WRP_GRP           SP5K_GPIO_GRP_GEN
#define SP5K_SD_WRP_ID            14
#define SP5K_SD_WRP_POL           SP5K_GPIO_ACTIVE_HIGH

/*_________________________________________________________________________

  disp device.
 _________________________________________________________________________
*/
/* default display device.
 * 1: lcd panel
 * 2: composite tv ntsc
 * 3: composite tv pal
 */

/* lcd panel display device selection list.
 * A015AN04_UPS051: AUO 1.5"
 * A020BL01_UPS052: AUO 2.0"
 * A025BN01_UPS052: AUO 2.5"
 * A027BN01_UPS052: AUO 2.7"
 * TD025THEB1_UPS052: Topoly 2.5"
 */
#ifndef SP5K_LCD_CUSTOM_DEFINE
#define SP5K_LCD_A015AN04_UPS051        0   /* 280*220 */
#define SP5K_LCD_A015AN04_UPS052        0   /* 305*220 */
#define SP5K_LCD_A020BL01_UPS052        0
#define SP5K_LCD_GP024G48238QS5_UPS052        0  /* 320*240 */
#define SP5K_LCD_A025BN01_UPS051        0  /* 640*240 */
#define SP5K_LCD_A025BN01_UPS052        0   /* 320*240 */
#define SP5K_LCD_A025BN01_CCIR656_W640  0  /* 640*240 */
#define SP5K_LCD_A025BN01_YUV_W640      0  /* 640*240 */
#define SP5K_LCD_A025DL01_UPS052        0   /* 320*240 */
#define SP5K_LCD_A027DN01_UPS052        0  /* 320*240 */
#define SP5K_LCD_A030DL01_UPS052        0   /* 320*240 */
#define SP5K_LCD_TD025THEB1_UPS052      1   /* 320*240 */
#define SP5K_LCD_TS027CAARD02_UPS052       0 /* 320*240 */
#define SP5K_LCD_TD027LCJT02_UPS052       0 /* 320*240 */
#define SP5K_LCD_TD020THEG1_UPS052        0  /* 320*240 */
#endif

#define SP5K_DISP_DEFAULT_DEV  1

#ifdef _EVB_HW_/* --------- EVB ---------------------------------- */
#define	SP5K_DISP_LCD_RESET_GRP			(SP5K_GPIO_GRP_NO_USE)
#define	SP5K_DISP_LCD_RESET_MASK		(1<<18)
#define	SP5K_DISP_LCD_BACKLIGHT_GRP		(SP5K_GPIO_GRP_NO_USE)
#define	SP5K_DISP_LCD_BACKLIGHT_MASK	(1<<14)
#define	SP5K_DISP_LCD_SHDB_GRP			(SP5K_GPIO_GRP_NO_USE)
#define	SP5K_DISP_LCD_SHDB_MASK			(1<<17)
#endif
/*e Modify by Aries 10/02/04*/


/*_________________________________________________________________________

  thee-wire gpio pin selection.
  plz refer to sp5kGpioGrp_t in sp5k_global_api.h to select gpio group.
 _____________________________________________\____________________________
 */
#ifdef _EVB_HW_ /* --------- EVB ---------------------------------- */
#define SP5K_DISP_SPI_SCL_GRP  SP5K_GPIO_GRP_TGH
#define SP5K_DISP_SPI_SCL_ID   (38-32)
#define SP5K_DISP_SPI_SDA_GRP  SP5K_GPIO_GRP_TGH
#define SP5K_DISP_SPI_SDA_ID   (39-32)
#define SP5K_DISP_SPI_CS_GRP   SP5K_GPIO_GRP_DISP
#define SP5K_DISP_SPI_CS_ID    25
#define SP5K_DISP_IO_CFG      ((1 << 13) | (1 << 17) | (1 << 18))
#define SP5K_DISP_IO_VAL      ((1 << 13) | (1 << 17) | (1 << 18))
#endif

/*_________________________________________________________________________

  usb.
 _________________________________________________________________________
*/
/* USB gpio pin define
 *
 * VBUS_DETECT_XXX : for USB plug-in detect pin
 *   XXX_GRP       : GPIO group type in sp5kGpioGrp_t
 *   XXX_ID        : GPIO pin number
 *   XXX_POL       : GPIO polarity in sp5kDiskGpioPolarity_t
 */
/* --------- EVB ---------------------------------- */
#define SP5K_VBUS_DETECT_GRP      SP5K_GPIO_GRP_DISP
#define SP5K_VBUS_DETECT_ID       17
#define SP5K_VBUS_DETECT_POL      SP5K_GPIO_ACTIVE_HIGH


/*_________________________________________________________________________

  front.
 _________________________________________________________________________
*/
/*select sensor TG master clock source, select either ONE of the 3*/

#if defined(_EVB_HW_) /* --------- EVB ---------------------------------- */
    #define SP5K_FRONT_MCLK_SRC_NOT_CONNECT   0 /*sensor/XTG has own MCLK*/
    #define SP5K_FRONT_MCLK_SRC_SP5K_TGPLL    1
    #define SP5K_FRONT_MCLK_SRC_ADCLPI_PAD    0
#else
	#error Undefined HW Configured !!
#endif


/*select sensor SyncSigGen clock source master, select either ONE of the 2
 *NOTE: not all the sensor drivers support both modes
 *such sensor drivers will ignore this setting
 */
#define SP5K_FRONT_SYNC_SIG_SP5K_MASTER   0
#define SP5K_FRONT_SYNC_SIG_SP5K_SLAVE    1

/*select sensor/TG, AFE*/
/* CISPLUS CMOS Image Sensor */
#ifndef SP5K_SENSOR_CUSTOM_DEFINE

#define SP5K_SENSOR_TT3131                0

/*SONY CMOS*/
#define SP5K_SENSOR_SONYIMX175 0
/* OmniVision CMOS Image Sensor */
#define SP5K_SENSOR_OV2610                0
#define SP5K_SENSOR_OV5610                0
#define SP5K_SENSOR_OV5620                0
#define SP5K_SENSOR_OV5633                0
#define SP5K_SENSOR_OV5653               0
#define SP5K_SENSOR_OV9712                0
#define SP5K_SENSOR_OV9810			  0
#define SP5K_SENSOR_OV16820			  0
#define SP5K_SENSOR_OV8810			  1
#define SP5K_SENSOR_PANAMN34110       0
#define SP5K_SENSOR_OV2715       	         0
#define SP5K_SENSOR_SONYIMX083        0


/* Micro CMOS Image Sensor */
#define SP5K_SENSOR_MI5100                0
#define SP5K_SENSOR_MI9130                0
#define SP5K_SENSOR_MI5130                0
#define SP5K_SENSOR_MT9J003              0
#define SP5K_SENSOR_AR0331                0
#define SP5K_SENSOR_AR0330		  0
#define SP5K_SENSOR_AR0330CS		  0
#define SP5K_SENSOR_MI5100p                0


/* Panasonic CCD Image Sensor */
#define SP5K_SENSOR_TG_PANX12067_MN39830  0
#define SP5K_SENSOR_AFE_PANX12067         0
#define SP5K_SENSOR_TG_PANX12069_MN39830  0
#define SP5K_SENSOR_TG_PANX12069_MN39600  0
#define SP5K_SENSOR_AFE_PANX12069         0
#define SP5K_SENSOR_TG_PANX12081A_MN34510  0
#define SP5K_SENSOR_AFE_PANX12081A         0

/* Sony CCD Image Sensor */
#define SP5K_SENSOR_TG_CXD4805GA_SONY624  0
#define SP5K_SENSOR_AFE_CXD4805GA         0
#define SP5K_SENSOR_TG_CXD3626GA_SONY455  0
#define SP5K_SENSOR_TG_AD9923_SONY629     0
#define SP5K_SENSOR_TG_AD9920_SONY636     0
#define SP5K_SENSOR_TG_AD9920_SONY646     0
#define SP5K_SENSOR_TG_AD9000_SONY675     0

/* customer Sensor */
#define SP5K_SENSOR_AFE_CUSTOM            0
#define SP5K_SENSOR_TG_CUSTOM             0

/* Sharp CCD Image Sensor */
#define SP5K_SENSOR_TG_AD9920_SHARPRJ23T3A 0
#define SP5K_SENSOR_TG_AD9920_SHARPRJ23V3B 0
#define SP5K_SENSOR_TG_AD9920_SHARPRJ23U3B 0
#define SP5K_SENSOR_TG_AD9000_SHARPRJ23V3B 0
#define SP5K_SENSOR_TG_AD9000_SHARPRJ23U3B 0

/* ADI AFE */
#define SP5K_SENSOR_AFE_AD9000            0
#define SP5K_SENSOR_AFE_AD9920            0
#define SP5K_SENSOR_AFE_AD9923            0
#define SP5K_SENSOR_AFE_AD9943            0
#endif
/*mechanical shutter support for snap*/
#define SP5K_SNAP_SHUTTER_SUPPORT     1

/*select TV-in decoder. set all to 0 if no TV-in support*/
#define SP5K_TVIN_DEC_XXX 0
#define SP5K_TVIN_DEC_YYY 0

/*_________________________________________________________________________

  audio device.
 _________________________________________________________________________
*/
/* audio device select
 *
 * AUD_DEVICE_INTERNAL : chip embedded audio controller
 * AUD_DEVICE_WS8751   : external WS8751 audio controller
 */
#define SP5K_AUD_DEVICE_INTERNAL    1
#define SP5K_AUD_DEVICE_HDMI        0
#define SP5K_AUD_DEVICE_WS8751      0

#if SPCA5330
#define SP5K_AUD_DEVICE2_INTERNAL    0
#define SP5K_AUD_DEVICE2_HDMI        1
#define SP5K_AUD_DEVICE2_WS8751      0
#endif


#if 0
#if SPCA5310
#define SP5K_AUD_DEVICE_ANA_ANX7150 0
#define SP5K_AUD_DEVICE_EP_EP932M 1
#endif
#endif

/* speaker gpio pin define
 *
 * AUD_DEV_SPEAKER_XXX : for speaker power on pin
 *   XXX_GRP           : GPIO group type in sp5kGpioGrp_t
 *   XXX_ID            : GPIO pin number
 *   XXX_POL           : GPIO polarity in sp5kDiskGpioPolarity_t
 */
#ifdef _EVB_HW_ /* --------- EVB ---------------------------------- */
#define SP5K_AUD_DEV_SPEAKER_GRP    SP5K_GPIO_GRP_TGH
#define SP5K_AUD_DEV_SPEAKER_ID     12
#define SP5K_AUD_DEV_SPEAKER_POL    SP5K_GPIO_ACTIVE_HIGH
#endif

/*_________________________________________________________________________

  key button.
 _________________________________________________________________________
*/

#if defined(_EVB_HW_) /* --------- EVB ---------------------------------- */

#ifdef HW_DVS5M2
    #define SP5K_BUTTON_DIRECT_GPIO_NUMBER  9
    #define SP5K_BUTTON_MATRIX_GPIO_NUMBER  0
    #define SP5K_BUTTON_ADC_NUMBER          0
#else
    #define SP5K_BUTTON_DIRECT_GPIO_NUMBER  7
    #define SP5K_BUTTON_MATRIX_GPIO_NUMBER  0
    #define SP5K_BUTTON_ADC_NUMBER          16
#endif

#else
	#error Undefined HW Configured !!
#endif


/*_________________________________________________________________________

  power init.
 _________________________________________________________________________
*/
#define SP5K_POWER_ON_LOOP_COUNT    1     /* 1ms */
#define SP5K_POWER_OFF_LOOP_COUNT   50  /* 50ms */
BOOL appGetBoot4ChargingFlag();

/*_________________________________________________________________________

  macros.
 _________________________________________________________________________
*/
#include "macro_expand.h"

#endif  /* __ASSEMBLER__ */


#endif  /* _DEV_INIT_H_ */

