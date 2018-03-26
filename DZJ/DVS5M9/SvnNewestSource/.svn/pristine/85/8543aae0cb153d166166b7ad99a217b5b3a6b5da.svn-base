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
#ifndef APP_AE_PRIVATE_H
#define APP_AE_PRIVATE_H


/**************************************************************************
 * Debug Control                                                          *
 **************************************************************************/
#define AE_DBG_MSG 0

#if AE_DBG_MSG
#define printf_t printf
#else
__inline__ static void printf_t(char *pstr, ...) {}
#endif

#define printf_x(str, arg...)	do {} while(0)

#define _AE_PASM_DBG_

/***-- AE FREQ Debug !! */
#define AE_FREQ_DBG
extern BOOL aeFreqDbg;
extern BOOL aeRunDump;
extern int	aeFreqDbgCnt;
extern UINT32	aeFreqDbgSlot;

#define AE_FREQ_DBG_FREQ 2 
#define AE_FREQ_DBG_0 	  (0xFFFFFFFF)
#define AE_FREQ_DBG_1 	  (0x00000001)
#define AE_FREQ_DBG_2 	  (0x1UL<<16 | 0x01UL<<0)
#define AE_FREQ_DBG_3 	  (0x1UL<<20 | 0x01UL<<10 | 0x01UL<<0)
#define AE_FREQ_DBG_4 	  (0x1UL<<24 | 0x1UL<<16 | 0x01UL<<8 | 0x01UL<<0)
#define AE_FREQ_DBG_5 	  (AE_FREQ_DBG_7)
#define AE_FREQ_DBG_6 	  (0x1UL<<24 | 0x1UL<<19 | 0x1UL<<14 | 0x1UL<<9 | 0x01UL<<4 | 0x01UL<<0)
#define AE_FREQ_DBG_7 	  (AE_FREQ_DBG_7)
#define AE_FREQ_DBG_8 	  (0x1UL<<28 | 0x1UL<<24 | 0x1UL<<20 | 0x1UL<<16 | 0x1UL<<12 | 0x1UL<<8 | 0x01UL<<4 | 0x01UL<<0)

#define IS_FREQ_DBG_0 	  (aeRunDump && aeFreqDbg && (aeFreqDbgSlot & AE_FREQ_DBG_0))
#define IS_FREQ_DBG_1 	  (aeRunDump && aeFreqDbg && (aeFreqDbgSlot & AE_FREQ_DBG_1))
#define IS_FREQ_DBG_2 	  (aeRunDump && aeFreqDbg && (aeFreqDbgSlot & AE_FREQ_DBG_2))
#define IS_FREQ_DBG_3 	  (aeRunDump && aeFreqDbg && (aeFreqDbgSlot & AE_FREQ_DBG_3))
#define IS_FREQ_DBG_4 	  (aeRunDump && aeFreqDbg && (aeFreqDbgSlot & AE_FREQ_DBG_4))
#define IS_FREQ_DBG_5 	  (aeRunDump && aeFreqDbg && (aeFreqDbgSlot & AE_FREQ_DBG_5))
#define IS_FREQ_DBG_6 	  (aeRunDump && aeFreqDbg && (aeFreqDbgSlot & AE_FREQ_DBG_6))
#define IS_FREQ_DBG_7 	  (aeRunDump && aeFreqDbg && (aeFreqDbgSlot & AE_FREQ_DBG_7))
#define IS_FREQ_DBG_X 	  (0)

/**************************************************************************
 * Platform AE Configuration                                              *
 **************************************************************************/
#if SP5K_SENSOR_OV5610

#define DO_CAP_SAT_GAIN_COMPENSATION

#undef	DO_ISO_XSHIFT
#define DEFAULT_SAT_AGCIDX		(14)
#define DEFAULT_PV_SAT_AGCIDX	(0)
#define DEFAULT_VIEW_ISO100		(24)
#define DEFAULT_CAP_ISO100		(41)
#define XSHIFT_CAP_ISO100		(DEFAULT_SAT_AGCIDX)
#define XSHIFT_PV_ISO100		(0)
#define CAP_ISO_XSHIFT			(0)
#define PV_ISO_XSHIFT			(-20)

#define IRIS_STEP			2
#define IRIS_STEP_1			48
#define IRIS_STEP_2			68
#define IRIS_SWITCH			EXP_1_512
#define IRIS_S2L_SWITCH		EXP_1_128
#define IRIS_L2S_SWITCH		EXP_1_512

#define AE_TARGET			160
#define AGC_BOUNDARY_L		32
#define AGC_BOUNDARY_H		80 
#define AGC_CAP_BOUNDARY_L	STD_ISO_50
#define AGC_CAP_BOUNDARY_H	STD_ISO_200
#define EXP_BOUNDARY_L		EXP_1_32
#define EXP_BOUNDARY_H		EXP_1_4096
#define EXP_CAP_BOUNDARY_L	EXP_8_01
#define EXP_CAP_BOUNDARY_H	EXP_1_2048

#define CAP_EXP_50HZ_1_100  106
#define CAP_EXP_50HZ_1_50 	90
#define CAP_EXP_50HZ_1_30 	81
#define CAP_EXP_50HZ_1_25 	74
#define CAP_EXP_50HZ_1_20 	69
#define CAP_EXP_50HZ_1_16 	65
#define CAP_EXP_50HZ_1_8 	49
#define CAP_EXP_50HZ_1_4 	32
#define CAP_EXP_50HZ_1_2 	16
#define CAP_EXP_50HZ_1_01 	0

#define CAP_EXP_60HZ_1_120 111
#define CAP_EXP_60HZ_1_60 	96
#define CAP_EXP_60HZ_1_30 	85
#define CAP_EXP_60HZ_1_20 	69
#define CAP_EXP_60HZ_1_16 	60
#define CAP_EXP_60HZ_1_8 	48
#define CAP_EXP_60HZ_1_4 	32
#define CAP_EXP_60HZ_1_2 	16
#define CAP_EXP_60HZ_1_01 	0
#define AE_CAP_FLICKER_SHUT_NUM 9


#define PRV_EXP_50HZ_1_100  106
#define PRV_EXP_50HZ_1_50 	90
#define PRV_EXP_50HZ_1_30 	81
#define PRV_EXP_50HZ_1_20 	69
#define PRV_EXP_50HZ_1_16 	65
#define PRV_EXP_50HZ_1_8 	49
#define PRV_EXP_50HZ_1_4 	32
#define PRV_EXP_50HZ_1_2 	16
#define PRV_EXP_50HZ_1_01 	0

#define PRV_EXP_60HZ_1_120 111
#define PRV_EXP_60HZ_1_60 	96
#define PRV_EXP_60HZ_1_30 	85
#define PRV_EXP_60HZ_1_20 	69
#define PRV_EXP_60HZ_1_16 	60
#define PRV_EXP_60HZ_1_8 	48
#define PRV_EXP_60HZ_1_4 	32
#define PRV_EXP_60HZ_1_2 	16
#define PRV_EXP_60HZ_1_01 	0

#define PRV_AGC_MIN_IDX 	80

#elif SP5K_SENSOR_MI9130  /* A900 */
#define DO_CAP_SAT_GAIN_COMPENSATION

#undef	DO_ISO_XSHIFT
#define DEFAULT_SAT_AGCIDX		(6)
#define DEFAULT_PV_SAT_AGCIDX	(6)
#define DEFAULT_VIEW_ISO100		(20)
#define DEFAULT_CAP_ISO100		(20)
#define XSHIFT_CAP_ISO100		(DEFAULT_SAT_AGCIDX)
#define XSHIFT_PV_ISO100		(0)
#define CAP_ISO_XSHIFT			(0)
#define PV_ISO_XSHIFT			(0)

#define IRIS_STEP			2
#define IRIS_STEP_1			48
#define IRIS_STEP_2			(IRIS_STEP_1 + 46)

#define IRIS_SWITCH			EXP_1_512
#define IRIS_S2L_SWITCH		EXP_1_128
#define IRIS_L2S_SWITCH		EXP_1_512

#define AE_TARGET			150

#define AGC_BOUNDARY_L		2//STD_ISO_50
#define AGC_BOUNDARY_H	STD_ISO_400+8	//STD_ISO_400 
#define AGC_CAP_BOUNDARY_L	2//STD_ISO_50
#define AGC_CAP_BOUNDARY_H	STD_ISO_1600//STD_ISO_800
#define EXP_BOUNDARY_L		EXP_1_32
#define EXP_BOUNDARY_H		EXP_1_4096
#define EXP_CAP_BOUNDARY_L	EXP_8_01
#define EXP_CAP_BOUNDARY_H	EXP_1_2048

#define CAP_EXP_50HZ_1_100  106
#define CAP_EXP_50HZ_1_50 	90
#define CAP_EXP_50HZ_1_30 	81
#define CAP_EXP_50HZ_1_25 	74
#define CAP_EXP_50HZ_1_20 	69
#define CAP_EXP_50HZ_1_16 	65
#define CAP_EXP_50HZ_1_8 	49
#define CAP_EXP_50HZ_1_4 	32
#define CAP_EXP_50HZ_1_2 	16
#define CAP_EXP_50HZ_1_01 	0

#define CAP_EXP_60HZ_1_120 111
#define CAP_EXP_60HZ_1_60 	96
#define CAP_EXP_60HZ_1_30 	85
#define CAP_EXP_60HZ_1_20 	69
#define CAP_EXP_60HZ_1_16 	60
#define CAP_EXP_60HZ_1_8 	48
#define CAP_EXP_60HZ_1_4 	32
#define CAP_EXP_60HZ_1_2 	16
#define CAP_EXP_60HZ_1_01 	0
#define AE_CAP_FLICKER_SHUT_NUM 9


#define PRV_EXP_50HZ_1_100  106
#define PRV_EXP_50HZ_1_50 	90
#define PRV_EXP_50HZ_1_30 	81
#define PRV_EXP_50HZ_1_20 	69
#define PRV_EXP_50HZ_1_16 	65
#define PRV_EXP_50HZ_1_8 	49
#define PRV_EXP_50HZ_1_4 	32
#define PRV_EXP_50HZ_1_2 	16
#define PRV_EXP_50HZ_1_01 	0

#define PRV_EXP_60HZ_1_120 111
#define PRV_EXP_60HZ_1_60 	96
#define PRV_EXP_60HZ_1_30 	85
#define PRV_EXP_60HZ_1_20 	69
#define PRV_EXP_60HZ_1_16 	60
#define PRV_EXP_60HZ_1_8 	48
#define PRV_EXP_60HZ_1_4 	32
#define PRV_EXP_60HZ_1_2 	16
#define PRV_EXP_60HZ_1_01 	0

#define PRV_AGC_MIN_IDX 	80

#elif SP5K_SENSOR_OV5620 
#define DO_CAP_SAT_GAIN_COMPENSATION

#undef	DO_ISO_XSHIFT
#define DEFAULT_SAT_AGCIDX		(14)
#define DEFAULT_PV_SAT_AGCIDX	(0)
#define DEFAULT_VIEW_ISO100		(0)
#define DEFAULT_CAP_ISO100		(41)
#define XSHIFT_CAP_ISO100		(DEFAULT_SAT_AGCIDX)
#define XSHIFT_PV_ISO100		(0)
#define CAP_ISO_XSHIFT			(0)
#define PV_ISO_XSHIFT			(0)

#define IRIS_STEP			1
#define IRIS_STEP_1			48
#define IRIS_STEP_2			68
#define IRIS_SWITCH			EXP_1_512
#define IRIS_S2L_SWITCH		EXP_1_128
#define IRIS_L2S_SWITCH		EXP_1_512

#define AE_TARGET			160

#define AGC_BOUNDARY_L		STD_ISO_100
#define AGC_BOUNDARY_H		STD_ISO_400
#define AGC_CAP_BOUNDARY_L	STD_ISO_50
#define AGC_CAP_BOUNDARY_H	STD_ISO_800
#define EXP_BOUNDARY_L		EXP_1_16
#define EXP_BOUNDARY_H		EXP_1_4096
#define EXP_CAP_BOUNDARY_L	EXP_1_01
#define EXP_CAP_BOUNDARY_H	EXP_1_16384

#define CAP_EXP_50HZ_1_100  106
#define CAP_EXP_50HZ_1_50 	90
#define CAP_EXP_50HZ_1_30 	81
#define CAP_EXP_50HZ_1_25 	74
#define CAP_EXP_50HZ_1_20 	69
#define CAP_EXP_50HZ_1_16 	65
#define CAP_EXP_50HZ_1_8 	49
#define CAP_EXP_50HZ_1_4 	32
#define CAP_EXP_50HZ_1_2 	16
#define CAP_EXP_50HZ_1_01 	0

#define CAP_EXP_60HZ_1_120 111
#define CAP_EXP_60HZ_1_60 	96
#define CAP_EXP_60HZ_1_30 	85
#define CAP_EXP_60HZ_1_20 	69
#define CAP_EXP_60HZ_1_16 	60
#define CAP_EXP_60HZ_1_8 	48
#define CAP_EXP_60HZ_1_4 	32
#define CAP_EXP_60HZ_1_2 	16
#define CAP_EXP_60HZ_1_01 	0
#define AE_CAP_FLICKER_SHUT_NUM 9


#define PRV_EXP_50HZ_1_100  106
#define PRV_EXP_50HZ_1_50 	90
#define PRV_EXP_50HZ_1_30 	81
#define PRV_EXP_50HZ_1_20 	69
#define PRV_EXP_50HZ_1_16 	65
#define PRV_EXP_50HZ_1_8 	49
#define PRV_EXP_50HZ_1_4 	32
#define PRV_EXP_50HZ_1_2 	16
#define PRV_EXP_50HZ_1_01 	0

#define PRV_EXP_60HZ_1_120 111
#define PRV_EXP_60HZ_1_60 	96
#define PRV_EXP_60HZ_1_30 	85
#define PRV_EXP_60HZ_1_20 	69
#define PRV_EXP_60HZ_1_16 	60
#define PRV_EXP_60HZ_1_8 	48
#define PRV_EXP_60HZ_1_4 	32
#define PRV_EXP_60HZ_1_2 	16
#define PRV_EXP_60HZ_1_01 	0

#define PRV_AGC_MIN_IDX 	80

#elif SP5K_SENSOR_OV5633 
#define DO_CAP_SAT_GAIN_COMPENSATION

#undef	DO_ISO_XSHIFT
#define DEFAULT_SAT_AGCIDX		(1)
#define DEFAULT_PV_SAT_AGCIDX	(0)
#define DEFAULT_VIEW_ISO100	(16) 
#define DEFAULT_CAP_ISO100		(16)
#define XSHIFT_CAP_ISO100		(DEFAULT_SAT_AGCIDX)
#define XSHIFT_PV_ISO100		(0)
#define CAP_ISO_XSHIFT			(0)
#define PV_ISO_XSHIFT			(0)

#define IRIS_STEP			1
#define IRIS_STEP_1			48
#define IRIS_STEP_2			68
#define IRIS_SWITCH			EXP_1_512

#define IRIS_S2L_SWITCH		EXP_1_128
#define IRIS_L2S_SWITCH		EXP_1_512
#define AE_TARGET			250/*150 */

#define AGC_BOUNDARY_L		STD_ISO_100
#define AGC_BOUNDARY_H		STD_ISO_400
#define AGC_CAP_BOUNDARY_L	STD_ISO_50
#define AGC_CAP_BOUNDARY_H	STD_ISO_800
#define EXP_BOUNDARY_L		EXP_1_16
#define EXP_BOUNDARY_H		EXP_1_4096
#define EXP_CAP_BOUNDARY_L	EXP_1_02
#define EXP_CAP_BOUNDARY_H	EXP_1_16384  

#define CAP_EXP_50HZ_1_100  106
#define CAP_EXP_50HZ_1_50 	90
#define CAP_EXP_50HZ_1_30 	81
#define CAP_EXP_50HZ_1_25 	74
#define CAP_EXP_50HZ_1_20 	69
#define CAP_EXP_50HZ_1_16 	65
#define CAP_EXP_50HZ_1_8 	49
#define CAP_EXP_50HZ_1_4 	32
#define CAP_EXP_50HZ_1_2 	16
#define CAP_EXP_50HZ_1_01 	0

#define CAP_EXP_60HZ_1_120 111
#define CAP_EXP_60HZ_1_60 	96
#define CAP_EXP_60HZ_1_30 	85
#define CAP_EXP_60HZ_1_20 	69
#define CAP_EXP_60HZ_1_16 	60
#define CAP_EXP_60HZ_1_8 	48
#define CAP_EXP_60HZ_1_4 	32
#define CAP_EXP_60HZ_1_2 	16
#define CAP_EXP_60HZ_1_01 	0
#define AE_CAP_FLICKER_SHUT_NUM 9


#define PRV_EXP_50HZ_1_100  106
#define PRV_EXP_50HZ_1_50 	90
#define PRV_EXP_50HZ_1_30 	81
#define PRV_EXP_50HZ_1_20 	69
#define PRV_EXP_50HZ_1_16 	65
#define PRV_EXP_50HZ_1_8 	49
#define PRV_EXP_50HZ_1_4 	32
#define PRV_EXP_50HZ_1_2 	16
#define PRV_EXP_50HZ_1_01 	0

#define PRV_EXP_60HZ_1_120 111
#define PRV_EXP_60HZ_1_60 	96
#define PRV_EXP_60HZ_1_30 	85
#define PRV_EXP_60HZ_1_20 	69
#define PRV_EXP_60HZ_1_16 	60
#define PRV_EXP_60HZ_1_8 	48
#define PRV_EXP_60HZ_1_4 	32
#define PRV_EXP_60HZ_1_2 	16
#define PRV_EXP_60HZ_1_01 	0

#define PRV_AGC_MIN_IDX 	80

#elif SP5K_SENSOR_OV5653 
#define DO_CAP_SAT_GAIN_COMPENSATION
#define	DO_ISO_XSHIFT

#define DEFAULT_SAT_AGCIDX		(1) 
#define DEFAULT_PV_SAT_AGCIDX		(1)
#define DEFAULT_VIEW_ISO100		(17)
#define DEFAULT_CAP_ISO100		(17)
#define XSHIFT_CAP_ISO100		(DEFAULT_SAT_AGCIDX)
#define XSHIFT_PV_ISO100		(0)
#define CAP_ISO_XSHIFT			(0)
#define PV_ISO_XSHIFT			(-16)

#define IRIS_STEP			1
#define IRIS_STEP_1			48
#define IRIS_STEP_2			68
#define IRIS_SWITCH			EXP_1_512
#define IRIS_S2L_SWITCH		EXP_1_128
#define IRIS_L2S_SWITCH		EXP_1_512

#define AE_TARGET			160

#define AGC_BOUNDARY_L		STD_ISO_100
#define AGC_BOUNDARY_H		STD_ISO_400
#define AGC_CAP_BOUNDARY_L	STD_ISO_50
#define AGC_CAP_BOUNDARY_H	STD_ISO_800
#define EXP_BOUNDARY_L		EXP_1_32
#define EXP_BOUNDARY_H		EXP_1_4096
#define EXP_CAP_BOUNDARY_L	EXP_1_02
#define EXP_CAP_BOUNDARY_H	EXP_1_2048 


#define CAP_EXP_50HZ_1_100  106
#define CAP_EXP_50HZ_1_50 	90
#define CAP_EXP_50HZ_1_30 	81
#define CAP_EXP_50HZ_1_25 	74
#define CAP_EXP_50HZ_1_20 	69
#define CAP_EXP_50HZ_1_16 	65
#define CAP_EXP_50HZ_1_8 	49
#define CAP_EXP_50HZ_1_4 	32
#define CAP_EXP_50HZ_1_2 	16
#define CAP_EXP_50HZ_1_01 	0

#define CAP_EXP_60HZ_1_120 111
#define CAP_EXP_60HZ_1_60 	96
#define CAP_EXP_60HZ_1_30 	85
#define CAP_EXP_60HZ_1_20 	69
#define CAP_EXP_60HZ_1_16 	60
#define CAP_EXP_60HZ_1_8 	48
#define CAP_EXP_60HZ_1_4 	32
#define CAP_EXP_60HZ_1_2 	16
#define CAP_EXP_60HZ_1_01 	0
#define AE_CAP_FLICKER_SHUT_NUM 9


#define PRV_EXP_50HZ_1_100  106
#define PRV_EXP_50HZ_1_50 	90
#define PRV_EXP_50HZ_1_30 	81
#define PRV_EXP_50HZ_1_20 	69
#define PRV_EXP_50HZ_1_16 	65
#define PRV_EXP_50HZ_1_8 	49
#define PRV_EXP_50HZ_1_4 	32
#define PRV_EXP_50HZ_1_2 	16
#define PRV_EXP_50HZ_1_01 	0

#define PRV_EXP_60HZ_1_120 111
#define PRV_EXP_60HZ_1_60 	96
#define PRV_EXP_60HZ_1_30 	85
#define PRV_EXP_60HZ_1_20 	69
#define PRV_EXP_60HZ_1_16 	60
#define PRV_EXP_60HZ_1_8 	48
#define PRV_EXP_60HZ_1_4 	32
#define PRV_EXP_60HZ_1_2 	16
#define PRV_EXP_60HZ_1_01 	0

#define PRV_AGC_MIN_IDX 	80

#elif SP5K_SENSOR_OV9712
#define DO_CAP_SAT_GAIN_COMPENSATION

#undef	DO_ISO_XSHIFT
#define DEFAULT_SAT_AGCIDX		(1)
#define DEFAULT_PV_SAT_AGCIDX	(0)
#define DEFAULT_VIEW_ISO100	(8)
#define DEFAULT_CAP_ISO100		(8)
#define XSHIFT_CAP_ISO100		(DEFAULT_SAT_AGCIDX)
#define XSHIFT_PV_ISO100		(0)
#define CAP_ISO_XSHIFT			(0)
#define PV_ISO_XSHIFT			(0)

#define IRIS_STEP			1
#define IRIS_STEP_1			48
#define IRIS_STEP_2			68
#define IRIS_SWITCH			EXP_1_512

#define IRIS_S2L_SWITCH		EXP_1_128
#define IRIS_L2S_SWITCH		EXP_1_512
#define AE_TARGET			160

#define AGC_BOUNDARY_L		STD_ISO_100
#define AGC_BOUNDARY_H		STD_ISO_400
#define AGC_CAP_BOUNDARY_L	STD_ISO_50
#define AGC_CAP_BOUNDARY_H	STD_ISO_800
#define EXP_BOUNDARY_L		EXP_1_16
#define EXP_BOUNDARY_H		EXP_1_4096
#define EXP_CAP_BOUNDARY_L	EXP_1_02
#define EXP_CAP_BOUNDARY_H	EXP_1_16384  

#define CAP_EXP_50HZ_1_100  106
#define CAP_EXP_50HZ_1_50 	90
#define CAP_EXP_50HZ_1_30 	81
#define CAP_EXP_50HZ_1_25 	74
#define CAP_EXP_50HZ_1_20 	69
#define CAP_EXP_50HZ_1_16 	65
#define CAP_EXP_50HZ_1_8 	49
#define CAP_EXP_50HZ_1_4 	32
#define CAP_EXP_50HZ_1_2 	16
#define CAP_EXP_50HZ_1_01 	0

#define CAP_EXP_60HZ_1_120 111
#define CAP_EXP_60HZ_1_60 	96
#define CAP_EXP_60HZ_1_30 	85
#define CAP_EXP_60HZ_1_20 	69
#define CAP_EXP_60HZ_1_16 	60
#define CAP_EXP_60HZ_1_8 	48
#define CAP_EXP_60HZ_1_4 	32
#define CAP_EXP_60HZ_1_2 	16
#define CAP_EXP_60HZ_1_01 	0
#define AE_CAP_FLICKER_SHUT_NUM 9


#define PRV_EXP_50HZ_1_100  106
#define PRV_EXP_50HZ_1_50 	90
#define PRV_EXP_50HZ_1_30 	81
#define PRV_EXP_50HZ_1_20 	69
#define PRV_EXP_50HZ_1_16 	65
#define PRV_EXP_50HZ_1_8 	49
#define PRV_EXP_50HZ_1_4 	32
#define PRV_EXP_50HZ_1_2 	16
#define PRV_EXP_50HZ_1_01 	0

#define PRV_EXP_60HZ_1_120 111
#define PRV_EXP_60HZ_1_60 	96
#define PRV_EXP_60HZ_1_30 	85
#define PRV_EXP_60HZ_1_20 	69
#define PRV_EXP_60HZ_1_16 	60
#define PRV_EXP_60HZ_1_8 	48
#define PRV_EXP_60HZ_1_4 	32
#define PRV_EXP_60HZ_1_2 	16
#define PRV_EXP_60HZ_1_01 	0

#define PRV_AGC_MIN_IDX 	80

#elif SP5K_SENSOR_OV9810 
#define DO_CAP_SAT_GAIN_COMPENSATION

#undef	DO_ISO_XSHIFT
#define DEFAULT_SAT_AGCIDX		(1)
#define DEFAULT_PV_SAT_AGCIDX	(0)
#define DEFAULT_VIEW_ISO100		(1)
#define DEFAULT_CAP_ISO100		(17)
#define XSHIFT_CAP_ISO100		(DEFAULT_SAT_AGCIDX)
#define XSHIFT_PV_ISO100		(0)
#define CAP_ISO_XSHIFT			(0)
#define PV_ISO_XSHIFT			(0)

#define IRIS_STEP			1
#define IRIS_STEP_1			48
#define IRIS_STEP_2			68
#define IRIS_SWITCH			EXP_1_512
#define IRIS_S2L_SWITCH		EXP_1_128
#define IRIS_L2S_SWITCH		EXP_1_512

#define AE_TARGET			160

#define AGC_BOUNDARY_L		STD_ISO_100
#define AGC_BOUNDARY_H		STD_ISO_400
#define AGC_CAP_BOUNDARY_L	STD_ISO_50
#define AGC_CAP_BOUNDARY_H	STD_ISO_800
#define EXP_BOUNDARY_L		EXP_1_16
#define EXP_BOUNDARY_H		EXP_1_4096
#define EXP_CAP_BOUNDARY_L	EXP_1_02
#define EXP_CAP_BOUNDARY_H	EXP_1_16384 

#define CAP_EXP_50HZ_1_100  106
#define CAP_EXP_50HZ_1_50 	90
#define CAP_EXP_50HZ_1_30 	81
#define CAP_EXP_50HZ_1_25 	74
#define CAP_EXP_50HZ_1_20 	69
#define CAP_EXP_50HZ_1_16 	65
#define CAP_EXP_50HZ_1_8 	49
#define CAP_EXP_50HZ_1_4 	32
#define CAP_EXP_50HZ_1_2 	16
#define CAP_EXP_50HZ_1_01 	0

#define CAP_EXP_60HZ_1_120 111
#define CAP_EXP_60HZ_1_60 	96
#define CAP_EXP_60HZ_1_30 	85
#define CAP_EXP_60HZ_1_20 	69
#define CAP_EXP_60HZ_1_16 	60
#define CAP_EXP_60HZ_1_8 	48
#define CAP_EXP_60HZ_1_4 	32
#define CAP_EXP_60HZ_1_2 	16
#define CAP_EXP_60HZ_1_01 	0
#define AE_CAP_FLICKER_SHUT_NUM 9


#define PRV_EXP_50HZ_1_100  106
#define PRV_EXP_50HZ_1_50 	90
#define PRV_EXP_50HZ_1_30 	81
#define PRV_EXP_50HZ_1_20 	69
#define PRV_EXP_50HZ_1_16 	65
#define PRV_EXP_50HZ_1_8 	49
#define PRV_EXP_50HZ_1_4 	32
#define PRV_EXP_50HZ_1_2 	16
#define PRV_EXP_50HZ_1_01 	0

#define PRV_EXP_60HZ_1_120 111
#define PRV_EXP_60HZ_1_60 	96
#define PRV_EXP_60HZ_1_30 	85
#define PRV_EXP_60HZ_1_20 	69
#define PRV_EXP_60HZ_1_16 	60
#define PRV_EXP_60HZ_1_8 	48
#define PRV_EXP_60HZ_1_4 	32
#define PRV_EXP_60HZ_1_2 	16
#define PRV_EXP_60HZ_1_01 	0

#define PRV_AGC_MIN_IDX 	80

#elif SP5K_SENSOR_MT9J003
#define DO_CAP_SAT_GAIN_COMPENSATION
#undef	DO_ISO_XSHIFT

#define DEFAULT_SAT_AGCIDX		(1) 
#define DEFAULT_PV_SAT_AGCIDX		(1)
#define DEFAULT_VIEW_ISO100		(5)
#define DEFAULT_CAP_ISO100		(38)
#define XSHIFT_CAP_ISO100		(DEFAULT_SAT_AGCIDX)
#define XSHIFT_PV_ISO100		(0)
#define CAP_ISO_XSHIFT			(0)
#define PV_ISO_XSHIFT			(0)

#define IRIS_STEP			1
#define IRIS_STEP_1			48
#define IRIS_STEP_2			68
#define IRIS_SWITCH			EXP_1_512
#define IRIS_S2L_SWITCH		EXP_1_128
#define IRIS_L2S_SWITCH		EXP_1_512

#define AE_TARGET			160

#define AGC_BOUNDARY_L		STD_ISO_100
#define AGC_BOUNDARY_H		STD_ISO_400
#define AGC_CAP_BOUNDARY_L	STD_ISO_50
#define AGC_CAP_BOUNDARY_H	STD_ISO_800
#define EXP_BOUNDARY_L		EXP_1_32
#define EXP_BOUNDARY_H		EXP_1_4096
#define EXP_CAP_BOUNDARY_L	EXP_1_02
#define EXP_CAP_BOUNDARY_H	EXP_1_2048 


#define CAP_EXP_50HZ_1_100  106
#define CAP_EXP_50HZ_1_50 	90
#define CAP_EXP_50HZ_1_30 	81
#define CAP_EXP_50HZ_1_25 	74
#define CAP_EXP_50HZ_1_20 	69
#define CAP_EXP_50HZ_1_16 	65
#define CAP_EXP_50HZ_1_8 	49
#define CAP_EXP_50HZ_1_4 	32
#define CAP_EXP_50HZ_1_2 	16
#define CAP_EXP_50HZ_1_01 	0

#define CAP_EXP_60HZ_1_120 111
#define CAP_EXP_60HZ_1_60 	96
#define CAP_EXP_60HZ_1_30 	85
#define CAP_EXP_60HZ_1_20 	69
#define CAP_EXP_60HZ_1_16 	60
#define CAP_EXP_60HZ_1_8 	48
#define CAP_EXP_60HZ_1_4 	32
#define CAP_EXP_60HZ_1_2 	16
#define CAP_EXP_60HZ_1_01 	0
#define AE_CAP_FLICKER_SHUT_NUM 9

#define PRV_EXP_50HZ_1_100  109//107
#define PRV_EXP_50HZ_1_50 	90
#define PRV_EXP_50HZ_1_30 	81
#define PRV_EXP_50HZ_1_20 	69
#define PRV_EXP_50HZ_1_16 	65
#define PRV_EXP_50HZ_1_8 	49
#define PRV_EXP_50HZ_1_4 	32
#define PRV_EXP_50HZ_1_2 	16
#define PRV_EXP_50HZ_1_01 	0

#define PRV_EXP_60HZ_1_120 111
#define PRV_EXP_60HZ_1_60 	96
#define PRV_EXP_60HZ_1_30 	85
#define PRV_EXP_60HZ_1_20 	69
#define PRV_EXP_60HZ_1_16 	60
#define PRV_EXP_60HZ_1_8 	48
#define PRV_EXP_60HZ_1_4 	32
#define PRV_EXP_60HZ_1_2 	16
#define PRV_EXP_60HZ_1_01 	0

#define PRV_AGC_MIN_IDX 	80

#elif SP5K_SENSOR_AR0130
#undef DO_CAP_SAT_GAIN_COMPENSATION
#define DO_ISO_XSHIFT 

#define DEFAULT_SAT_AGCIDX		(1) 
#define DEFAULT_PV_SAT_AGCIDX	(1)
#define DEFAULT_VIEW_ISO100		(32)
#define DEFAULT_CAP_ISO100		(32)
#define XSHIFT_CAP_ISO100		(DEFAULT_SAT_AGCIDX)
#define XSHIFT_PV_ISO100		(0)
#define CAP_ISO_XSHIFT			(48) //for dual tbl limit
#define PV_ISO_XSHIFT			(0) 

#define IRIS_STEP			1
#define IRIS_STEP_1			48
#define IRIS_STEP_2			68
#define IRIS_SWITCH			EXP_1_512
#define IRIS_S2L_SWITCH		EXP_1_128
#define IRIS_L2S_SWITCH		EXP_1_512

#define AE_TARGET			216

#define AGC_BOUNDARY_L		STD_ISO_100
#define AGC_BOUNDARY_H		STD_ISO_400
#define AGC_CAP_BOUNDARY_L	STD_ISO_50
#define AGC_CAP_BOUNDARY_H	STD_ISO_800
#define EXP_BOUNDARY_L		EXP_1_32
#define EXP_BOUNDARY_H		EXP_1_4096
#define EXP_CAP_BOUNDARY_L	EXP_1_02
#define EXP_CAP_BOUNDARY_H	EXP_1_16384 

#define CAP_EXP_50HZ_1_100  106
#define CAP_EXP_50HZ_1_50 	90
#define CAP_EXP_50HZ_1_30 	81
#define CAP_EXP_50HZ_1_25 	74
#define CAP_EXP_50HZ_1_20 	69
#define CAP_EXP_50HZ_1_16 	61
#define CAP_EXP_50HZ_1_8 	54
#define CAP_EXP_50HZ_1_4 	32
#define CAP_EXP_50HZ_1_2 	16
#define CAP_EXP_50HZ_1_01 	0

#define CAP_EXP_60HZ_1_120 112
#define CAP_EXP_60HZ_1_60 	94
#define CAP_EXP_60HZ_1_30 	80
#define CAP_EXP_60HZ_1_20 	75
#define CAP_EXP_60HZ_1_16 	66
#define CAP_EXP_60HZ_1_8 	50
#define CAP_EXP_60HZ_1_4 	33
#define CAP_EXP_60HZ_1_2 	16
#define CAP_EXP_60HZ_1_01 	0
#define AE_CAP_FLICKER_SHUT_NUM 9

#define PRV_EXP_50HZ_1_100  106
#define PRV_EXP_50HZ_1_50 	90
#define PRV_EXP_50HZ_1_30 	81
#define PRV_EXP_50HZ_1_20 	69
#define PRV_EXP_50HZ_1_16 	61
#define PRV_EXP_50HZ_1_8 	54
#define PRV_EXP_50HZ_1_4 	32
#define PRV_EXP_50HZ_1_2 	16
#define PRV_EXP_50HZ_1_01 	0

#define PRV_EXP_60HZ_1_120 112
#define PRV_EXP_60HZ_1_60 	95
#define PRV_EXP_60HZ_1_30 	85
#define PRV_EXP_60HZ_1_20 	75
#define PRV_EXP_60HZ_1_16 	66
#define PRV_EXP_60HZ_1_8 	49
#define PRV_EXP_60HZ_1_4 	33
#define PRV_EXP_60HZ_1_2 	16
#define PRV_EXP_60HZ_1_01 	0

#define PRV_AGC_MIN_IDX 	80

#elif SP5K_SENSOR_AR0330
#define DO_CAP_SAT_GAIN_COMPENSATION
#define        DO_ISO_XSHIFT 

#define DEFAULT_SAT_AGCIDX		(1) 
#define DEFAULT_PV_SAT_AGCIDX		(1)
#define DEFAULT_VIEW_ISO100		(32)
#define DEFAULT_CAP_ISO100		(32)
#define XSHIFT_CAP_ISO100		(DEFAULT_SAT_AGCIDX)
#define XSHIFT_PV_ISO100		(0)
#define CAP_ISO_XSHIFT			(0)
#define PV_ISO_XSHIFT			(-16)

#define IRIS_STEP			1
#define IRIS_STEP_1			48
#define IRIS_STEP_2			68
#define IRIS_SWITCH			EXP_1_512
#define IRIS_S2L_SWITCH		EXP_1_128
#define IRIS_L2S_SWITCH		EXP_1_512

#define AE_TARGET			216

#define AGC_BOUNDARY_L		STD_ISO_100
#define AGC_BOUNDARY_H		STD_ISO_400
#define AGC_CAP_BOUNDARY_L	STD_ISO_50
#define AGC_CAP_BOUNDARY_H	STD_ISO_800
#define EXP_BOUNDARY_L		EXP_1_32
#define EXP_BOUNDARY_H		EXP_1_4096
#define EXP_CAP_BOUNDARY_L	EXP_1_02
#define EXP_CAP_BOUNDARY_H	EXP_1_16384 

#define CAP_EXP_50HZ_1_100  106
#define CAP_EXP_50HZ_1_50 	90
#define CAP_EXP_50HZ_1_30 	81
#define CAP_EXP_50HZ_1_25 	74
#define CAP_EXP_50HZ_1_20 	69
#define CAP_EXP_50HZ_1_16 	61
#define CAP_EXP_50HZ_1_8 	54
#define CAP_EXP_50HZ_1_4 	32
#define CAP_EXP_50HZ_1_2 	16
#define CAP_EXP_50HZ_1_01 	0

#define CAP_EXP_60HZ_1_120 112
#define CAP_EXP_60HZ_1_60 	95
#define CAP_EXP_60HZ_1_30 	85
#define CAP_EXP_60HZ_1_20 	75
#define CAP_EXP_60HZ_1_16 	66
#define CAP_EXP_60HZ_1_8 	49
#define CAP_EXP_60HZ_1_4 	33
#define CAP_EXP_60HZ_1_2 	16
#define CAP_EXP_60HZ_1_01 	0
#define AE_CAP_FLICKER_SHUT_NUM 9


#define PRV_EXP_50HZ_1_100  106
#define PRV_EXP_50HZ_1_50 	90
#define PRV_EXP_50HZ_1_30 	81
#define PRV_EXP_50HZ_1_20 	69
#define PRV_EXP_50HZ_1_16 	61
#define PRV_EXP_50HZ_1_8 	54
#define PRV_EXP_50HZ_1_4 	32
#define PRV_EXP_50HZ_1_2 	16
#define PRV_EXP_50HZ_1_01 	0

#define PRV_EXP_60HZ_1_120 112
#define PRV_EXP_60HZ_1_60 	95
#define PRV_EXP_60HZ_1_30 	85
#define PRV_EXP_60HZ_1_20 	75
#define PRV_EXP_60HZ_1_16 	66
#define PRV_EXP_60HZ_1_8 	49
#define PRV_EXP_60HZ_1_4 	33
#define PRV_EXP_60HZ_1_2 	16
#define PRV_EXP_60HZ_1_01 	0

#define PRV_AGC_MIN_IDX 	80

#elif SP5K_SENSOR_AR0330CS
#define DO_CAP_SAT_GAIN_COMPENSATION
#undef  DO_ISO_XSHIFT
#define DEFAULT_SAT_AGCIDX (1)
#define DEFAULT_PV_SAT_AGCIDX (1)
#define DEFAULT_VIEW_ISO100 (2)
#define DEFAULT_CAP_ISO100 (2)
#define XSHIFT_CAP_ISO100 (DEFAULT_SAT_AGCIDX)
#define XSHIFT_PV_ISO100 (0)
#define CAP_ISO_XSHIFT (0)
#define PV_ISO_XSHIFT (0)

#define IRIS_STEP 1
#define IRIS_STEP_1 48
#define IRIS_STEP_2 68
#define IRIS_SWITCH EXP_1_512
#define IRIS_S2L_SWITCH EXP_1_128
#define IRIS_L2S_SWITCH EXP_1_512

#define AE_TARGET 160
#define AGC_BOUNDARY_L STD_ISO_50
#define AGC_BOUNDARY_H STD_ISO_400
#define AGC_CAP_BOUNDARY_L STD_ISO_50
#define AGC_CAP_BOUNDARY_H STD_ISO_200
#define EXP_BOUNDARY_L EXP_1_32
#define EXP_BOUNDARY_H EXP_1_4096
#define EXP_CAP_BOUNDARY_L EXP_1_16
#define EXP_CAP_BOUNDARY_H EXP_1_2048

#define CAP_EXP_50HZ_1_100 106
#define CAP_EXP_50HZ_1_50 90
#define CAP_EXP_50HZ_1_30 81
#define CAP_EXP_50HZ_1_25 74
#define CAP_EXP_50HZ_1_20 69
#define CAP_EXP_50HZ_1_16 65
#define CAP_EXP_50HZ_1_8 49
#define CAP_EXP_50HZ_1_4 32
#define CAP_EXP_50HZ_1_2 16
#define CAP_EXP_50HZ_1_01 0

#define CAP_EXP_60HZ_1_120 111
#define CAP_EXP_60HZ_1_60 96
#define CAP_EXP_60HZ_1_30 80
#define CAP_EXP_60HZ_1_20 69
#define CAP_EXP_60HZ_1_16 60
#define CAP_EXP_60HZ_1_8 48
#define CAP_EXP_60HZ_1_4 32
#define CAP_EXP_60HZ_1_2 16
#define CAP_EXP_60HZ_1_01 0
#define AE_CAP_FLICKER_SHUT_NUM 9
#define PRV_EXP_50HZ_1_100 106
#define PRV_EXP_50HZ_1_50 90
#define PRV_EXP_50HZ_1_30 81
#define PRV_EXP_50HZ_1_20 69
#define PRV_EXP_50HZ_1_16 65
#define PRV_EXP_50HZ_1_8 49
#define PRV_EXP_50HZ_1_4 32
#define PRV_EXP_50HZ_1_2 16
#define PRV_EXP_50HZ_1_01 0

#define PRV_EXP_60HZ_1_120 111
#define PRV_EXP_60HZ_1_60 96
#define PRV_EXP_60HZ_1_30 80	
#define PRV_EXP_60HZ_1_20 69
#define PRV_EXP_60HZ_1_16 60
#define PRV_EXP_60HZ_1_8 48
#define PRV_EXP_60HZ_1_4 32
#define PRV_EXP_60HZ_1_2 16
#define PRV_EXP_60HZ_1_01 0
#define PRV_AGC_MIN_IDX 	80


#elif SP5K_SENSOR_OV2715
#define DO_CAP_SAT_GAIN_COMPENSATION
#define        DO_ISO_XSHIFT 

#define DEFAULT_SAT_AGCIDX		(1) 
#define DEFAULT_PV_SAT_AGCIDX		(1)
#define DEFAULT_VIEW_ISO100		(32)
#define DEFAULT_CAP_ISO100		(32)
#define XSHIFT_CAP_ISO100		(DEFAULT_SAT_AGCIDX)
#define XSHIFT_PV_ISO100		(0)
#define CAP_ISO_XSHIFT			(0)
#define PV_ISO_XSHIFT			(-23)

#define IRIS_STEP			1
#define IRIS_STEP_1			48
#define IRIS_STEP_2			68
#define IRIS_SWITCH			EXP_1_512
#define IRIS_S2L_SWITCH		EXP_1_128
#define IRIS_L2S_SWITCH		EXP_1_512

#define AE_TARGET			216

#define AGC_BOUNDARY_L		STD_ISO_100
#define AGC_BOUNDARY_H		STD_ISO_400
#define AGC_CAP_BOUNDARY_L	STD_ISO_50
#define AGC_CAP_BOUNDARY_H	STD_ISO_800
#define EXP_BOUNDARY_L		EXP_1_32
#define EXP_BOUNDARY_H		EXP_1_4096
#define EXP_CAP_BOUNDARY_L	EXP_1_02
#define EXP_CAP_BOUNDARY_H	EXP_1_16384 

#define CAP_EXP_50HZ_1_100  106
#define CAP_EXP_50HZ_1_50 	90
#define CAP_EXP_50HZ_1_30 	81
#define CAP_EXP_50HZ_1_25 	74
#define CAP_EXP_50HZ_1_20 	69
#define CAP_EXP_50HZ_1_16 	61
#define CAP_EXP_50HZ_1_8 	54
#define CAP_EXP_50HZ_1_4 	32
#define CAP_EXP_50HZ_1_2 	16
#define CAP_EXP_50HZ_1_01 	0

#define CAP_EXP_60HZ_1_120 112
#define CAP_EXP_60HZ_1_60 	95
#define CAP_EXP_60HZ_1_30 	85
#define CAP_EXP_60HZ_1_20 	75
#define CAP_EXP_60HZ_1_16 	66
#define CAP_EXP_60HZ_1_8 	49
#define CAP_EXP_60HZ_1_4 	33
#define CAP_EXP_60HZ_1_2 	16
#define CAP_EXP_60HZ_1_01 	0
#define AE_CAP_FLICKER_SHUT_NUM 9


#define PRV_EXP_50HZ_1_100  106
#define PRV_EXP_50HZ_1_50 	90
#define PRV_EXP_50HZ_1_30 	81
#define PRV_EXP_50HZ_1_20 	69
#define PRV_EXP_50HZ_1_16 	61
#define PRV_EXP_50HZ_1_8 	54
#define PRV_EXP_50HZ_1_4 	32
#define PRV_EXP_50HZ_1_2 	16
#define PRV_EXP_50HZ_1_01 	0

#define PRV_EXP_60HZ_1_120 112
#define PRV_EXP_60HZ_1_60 	95
#define PRV_EXP_60HZ_1_30 	85
#define PRV_EXP_60HZ_1_20 	75
#define PRV_EXP_60HZ_1_16 	66
#define PRV_EXP_60HZ_1_8 	49
#define PRV_EXP_60HZ_1_4 	33
#define PRV_EXP_60HZ_1_2 	16
#define PRV_EXP_60HZ_1_01 	0

#define PRV_AGC_MIN_IDX 	80
#elif SP5K_SENSOR_SONYIMX083
#define DO_CAP_SAT_GAIN_COMPENSATION
#define        DO_ISO_XSHIFT 

#define DEFAULT_SAT_AGCIDX		(1) 
#define DEFAULT_PV_SAT_AGCIDX		(1)
#define DEFAULT_VIEW_ISO100		(28)
#define DEFAULT_CAP_ISO100		(28)
#define XSHIFT_CAP_ISO100		(DEFAULT_SAT_AGCIDX)
#define XSHIFT_PV_ISO100		(0)
#define CAP_ISO_XSHIFT			(0)
#define PV_ISO_XSHIFT			(-18)

#define IRIS_STEP			1
#define IRIS_STEP_1			48
#define IRIS_STEP_2			68
#define IRIS_SWITCH			EXP_1_512
#define IRIS_S2L_SWITCH		EXP_1_128
#define IRIS_L2S_SWITCH		EXP_1_512

#define AE_TARGET			210

#define AGC_BOUNDARY_L		STD_ISO_100
#define AGC_BOUNDARY_H		STD_ISO_800
#define AGC_CAP_BOUNDARY_L	STD_ISO_50
#define AGC_CAP_BOUNDARY_H	STD_ISO_1600
#define EXP_BOUNDARY_L		EXP_1_32
#define EXP_BOUNDARY_H		EXP_1_4096
#define EXP_CAP_BOUNDARY_L	EXP_1_02
#define EXP_CAP_BOUNDARY_H	EXP_1_4096 

#define CAP_EXP_50HZ_1_100  108
#define CAP_EXP_50HZ_1_50 	90
#define CAP_EXP_50HZ_1_30 	81
#define CAP_EXP_50HZ_1_25 	74
#define CAP_EXP_50HZ_1_20 	69
#define CAP_EXP_50HZ_1_16 	61
#define CAP_EXP_50HZ_1_8 	54
#define CAP_EXP_50HZ_1_4 	32
#define CAP_EXP_50HZ_1_2 	16
#define CAP_EXP_50HZ_1_01 	0

#define CAP_EXP_60HZ_1_120 112
#define CAP_EXP_60HZ_1_60 	96
#define CAP_EXP_60HZ_1_30 	80
#define CAP_EXP_60HZ_1_20 	70
#define CAP_EXP_60HZ_1_16 	64
#define CAP_EXP_60HZ_1_8 	48
#define CAP_EXP_60HZ_1_4 	32
#define CAP_EXP_60HZ_1_2 	16
#define CAP_EXP_60HZ_1_01 	0
#define AE_CAP_FLICKER_SHUT_NUM 9


#define PRV_EXP_50HZ_1_100  108
#define PRV_EXP_50HZ_1_50 	90
#define PRV_EXP_50HZ_1_30 	81
#define PRV_EXP_50HZ_1_20 	69
#define PRV_EXP_50HZ_1_16 	61
#define PRV_EXP_50HZ_1_8 	54
#define PRV_EXP_50HZ_1_4 	32
#define PRV_EXP_50HZ_1_2 	16
#define PRV_EXP_50HZ_1_01 	0

#define PRV_EXP_60HZ_1_120 112
#define PRV_EXP_60HZ_1_60 	96
#define PRV_EXP_60HZ_1_30 	80
#define PRV_EXP_60HZ_1_20 	70
#define PRV_EXP_60HZ_1_16 	64
#define PRV_EXP_60HZ_1_8 	48
#define PRV_EXP_60HZ_1_4 	32
#define PRV_EXP_60HZ_1_2 	16
#define PRV_EXP_60HZ_1_01 	0

#define PRV_AGC_MIN_IDX 	80

#elif SP5K_SENSOR_MT9F002
#define DO_CAP_SAT_GAIN_COMPENSATION
#undef	DO_ISO_XSHIFT

#define DEFAULT_SAT_AGCIDX		(1) 
#define DEFAULT_PV_SAT_AGCIDX		(1)
#define DEFAULT_VIEW_ISO100		(20)
#define DEFAULT_CAP_ISO100		(20)
#define XSHIFT_CAP_ISO100		(DEFAULT_SAT_AGCIDX)
#define XSHIFT_PV_ISO100		(0)
#define CAP_ISO_XSHIFT			(0)
#define PV_ISO_XSHIFT			(-15)

#define IRIS_STEP			1
#define IRIS_STEP_1			48
#define IRIS_STEP_2			68
#define IRIS_SWITCH			EXP_1_512
#define IRIS_S2L_SWITCH		EXP_1_128
#define IRIS_L2S_SWITCH		EXP_1_512
/*s Modify by Aries 12/06/07*/
#define AE_TARGET			190
/*e Modify by Aries 12/06/07*/

#define AGC_BOUNDARY_L		STD_ISO_100
#define AGC_BOUNDARY_H		STD_ISO_400
#define AGC_CAP_BOUNDARY_L	STD_ISO_50
#define AGC_CAP_BOUNDARY_H	STD_ISO_800
#define EXP_BOUNDARY_L		EXP_1_32
#define EXP_BOUNDARY_H		EXP_1_4096
#define EXP_CAP_BOUNDARY_L	EXP_1_02
#define EXP_CAP_BOUNDARY_H	EXP_1_16384 //EXP_1_2048 

#define CAP_EXP_50HZ_1_100  106
#define CAP_EXP_50HZ_1_50 	90
#define CAP_EXP_50HZ_1_30 	81
#define CAP_EXP_50HZ_1_25 	74
#define CAP_EXP_50HZ_1_20 	69
#define CAP_EXP_50HZ_1_16 	65
#define CAP_EXP_50HZ_1_8 	49
#define CAP_EXP_50HZ_1_4 	32
#define CAP_EXP_50HZ_1_2 	16
#define CAP_EXP_50HZ_1_01 	0

#define CAP_EXP_60HZ_1_120 111
#define CAP_EXP_60HZ_1_60 	96
#define CAP_EXP_60HZ_1_30 	85
#define CAP_EXP_60HZ_1_20 	69
#define CAP_EXP_60HZ_1_16 	60
#define CAP_EXP_60HZ_1_8 	48
#define CAP_EXP_60HZ_1_4 	32
#define CAP_EXP_60HZ_1_2 	16
#define CAP_EXP_60HZ_1_01 	0
#define AE_CAP_FLICKER_SHUT_NUM 9

#define PRV_EXP_50HZ_1_100  109//107
#define PRV_EXP_50HZ_1_50 	90
#define PRV_EXP_50HZ_1_30 	81
#define PRV_EXP_50HZ_1_20 	69
#define PRV_EXP_50HZ_1_16 	65
#define PRV_EXP_50HZ_1_8 	49
#define PRV_EXP_50HZ_1_4 	32
#define PRV_EXP_50HZ_1_2 	16
#define PRV_EXP_50HZ_1_01 	0

#define PRV_EXP_60HZ_1_120 111
#define PRV_EXP_60HZ_1_60 	96
#define PRV_EXP_60HZ_1_30 	85
#define PRV_EXP_60HZ_1_20 	69
#define PRV_EXP_60HZ_1_16 	60
#define PRV_EXP_60HZ_1_8 	48
#define PRV_EXP_60HZ_1_4 	32
#define PRV_EXP_60HZ_1_2 	16
#define PRV_EXP_60HZ_1_01 	0

#define PRV_AGC_MIN_IDX 	80
#elif SP5K_SENSOR_MI5100p
#define DO_CAP_SAT_GAIN_COMPENSATION
#define        DO_ISO_XSHIFT 

#define DEFAULT_SAT_AGCIDX		(1) 
#define DEFAULT_PV_SAT_AGCIDX		(1)
#define DEFAULT_VIEW_ISO100		(32)
#define DEFAULT_CAP_ISO100		(32)
#define XSHIFT_CAP_ISO100		(DEFAULT_SAT_AGCIDX)
#define XSHIFT_PV_ISO100		(0)
#define CAP_ISO_XSHIFT			(0)
#define PV_ISO_XSHIFT			(-23)

#define IRIS_STEP			1
#define IRIS_STEP_1			48
#define IRIS_STEP_2			68
#define IRIS_SWITCH			EXP_1_512
#define IRIS_S2L_SWITCH		EXP_1_128
#define IRIS_L2S_SWITCH		EXP_1_512

#define AE_TARGET			216

#define AGC_BOUNDARY_L		STD_ISO_100
#define AGC_BOUNDARY_H		STD_ISO_400
#define AGC_CAP_BOUNDARY_L	STD_ISO_50
#define AGC_CAP_BOUNDARY_H	STD_ISO_800
#define EXP_BOUNDARY_L		EXP_1_32
#define EXP_BOUNDARY_H		EXP_1_4096
#define EXP_CAP_BOUNDARY_L	EXP_1_02
#define EXP_CAP_BOUNDARY_H	EXP_1_16384 

#define CAP_EXP_50HZ_1_100  106
#define CAP_EXP_50HZ_1_50 	90
#define CAP_EXP_50HZ_1_30 	81
#define CAP_EXP_50HZ_1_25 	74
#define CAP_EXP_50HZ_1_20 	69
#define CAP_EXP_50HZ_1_16 	61
#define CAP_EXP_50HZ_1_8 	54
#define CAP_EXP_50HZ_1_4 	32
#define CAP_EXP_50HZ_1_2 	16
#define CAP_EXP_50HZ_1_01 	0

#define CAP_EXP_60HZ_1_120 112
#define CAP_EXP_60HZ_1_60 	95
#define CAP_EXP_60HZ_1_30 	85
#define CAP_EXP_60HZ_1_20 	75
#define CAP_EXP_60HZ_1_16 	66
#define CAP_EXP_60HZ_1_8 	49
#define CAP_EXP_60HZ_1_4 	33
#define CAP_EXP_60HZ_1_2 	16
#define CAP_EXP_60HZ_1_01 	0
#define AE_CAP_FLICKER_SHUT_NUM 9


#define PRV_EXP_50HZ_1_100  106
#define PRV_EXP_50HZ_1_50 	90
#define PRV_EXP_50HZ_1_30 	81
#define PRV_EXP_50HZ_1_20 	69
#define PRV_EXP_50HZ_1_16 	61
#define PRV_EXP_50HZ_1_8 	54
#define PRV_EXP_50HZ_1_4 	32
#define PRV_EXP_50HZ_1_2 	16
#define PRV_EXP_50HZ_1_01 	0

#define PRV_EXP_60HZ_1_120 112
#define PRV_EXP_60HZ_1_60 	95
#define PRV_EXP_60HZ_1_30 	85
#define PRV_EXP_60HZ_1_20 	75
#define PRV_EXP_60HZ_1_16 	66
#define PRV_EXP_60HZ_1_8 	49
#define PRV_EXP_60HZ_1_4 	33
#define PRV_EXP_60HZ_1_2 	16
#define PRV_EXP_60HZ_1_01 	0

#define PRV_AGC_MIN_IDX 	80

#elif SP5K_SENSOR_OV8810
#define DO_CAP_SAT_GAIN_COMPENSATION
#define        DO_ISO_XSHIFT 

#define DEFAULT_SAT_AGCIDX		(1)
#define DEFAULT_PV_SAT_AGCIDX		(1)
#define DEFAULT_VIEW_ISO100		(10)
#define DEFAULT_CAP_ISO100		(26)/*(26)*/
#define XSHIFT_CAP_ISO100		(DEFAULT_SAT_AGCIDX)
#define XSHIFT_PV_ISO100		(0)
#define CAP_ISO_XSHIFT			(0)
#define PV_ISO_XSHIFT			(0)

#define IRIS_STEP			1
#define IRIS_STEP_1			48
#define IRIS_STEP_2			68
#define IRIS_SWITCH			EXP_1_512
#define IRIS_S2L_SWITCH		EXP_1_128
#define IRIS_L2S_SWITCH		EXP_1_512

#define AE_TARGET			216

#define AGC_BOUNDARY_L		STD_ISO_100
#define AGC_BOUNDARY_H		STD_ISO_400
#define AGC_CAP_BOUNDARY_L	STD_ISO_50
#define AGC_CAP_BOUNDARY_H	STD_ISO_800
#define EXP_BOUNDARY_L		EXP_1_32
#define EXP_BOUNDARY_H		EXP_1_4096
#define EXP_CAP_BOUNDARY_L	EXP_1_02
#define EXP_CAP_BOUNDARY_H	EXP_1_16384  

#define CAP_EXP_50HZ_1_100  106
#define CAP_EXP_50HZ_1_50 	90
#define CAP_EXP_50HZ_1_30 	81
#define CAP_EXP_50HZ_1_25 	74
#define CAP_EXP_50HZ_1_20 	69
#define CAP_EXP_50HZ_1_16 	61
#define CAP_EXP_50HZ_1_8 	54
#define CAP_EXP_50HZ_1_4 	32
#define CAP_EXP_50HZ_1_2 	16
#define CAP_EXP_50HZ_1_01 	0

#define CAP_EXP_60HZ_1_120 112
#define CAP_EXP_60HZ_1_60 	95
#define CAP_EXP_60HZ_1_30 	85
#define CAP_EXP_60HZ_1_20 	75
#define CAP_EXP_60HZ_1_16 	66
#define CAP_EXP_60HZ_1_8 	49
#define CAP_EXP_60HZ_1_4 	33
#define CAP_EXP_60HZ_1_2 	16
#define CAP_EXP_60HZ_1_01 	0
#define AE_CAP_FLICKER_SHUT_NUM 9


#define PRV_EXP_50HZ_1_100  106
#define PRV_EXP_50HZ_1_50 	90
#define PRV_EXP_50HZ_1_30 	81
#define PRV_EXP_50HZ_1_20 	69
#define PRV_EXP_50HZ_1_16 	61
#define PRV_EXP_50HZ_1_8 	54
#define PRV_EXP_50HZ_1_4 	32
#define PRV_EXP_50HZ_1_2 	16
#define PRV_EXP_50HZ_1_01 	0

#define PRV_EXP_60HZ_1_120 112
#define PRV_EXP_60HZ_1_60 	95
#define PRV_EXP_60HZ_1_30 	85
#define PRV_EXP_60HZ_1_20 	75
#define PRV_EXP_60HZ_1_16 	66
#define PRV_EXP_60HZ_1_8 	49
#define PRV_EXP_60HZ_1_4 	33
#define PRV_EXP_60HZ_1_2 	16
#define PRV_EXP_60HZ_1_01 	0

#define PRV_AGC_MIN_IDX 	80

#else

#define DO_CAP_SAT_GAIN_COMPENSATION

#undef	DO_ISO_XSHIFT
#define DEFAULT_SAT_AGCIDX		(1)
#define DEFAULT_PV_SAT_AGCIDX	(1)
#define DEFAULT_VIEW_ISO100		(18)
#define DEFAULT_CAP_ISO100		(20)
#define XSHIFT_CAP_ISO100		(DEFAULT_SAT_AGCIDX)
#define XSHIFT_PV_ISO100		(0)
#define CAP_ISO_XSHIFT			(0)
#define PV_ISO_XSHIFT			(0)

#define IRIS_STEP			2
#define IRIS_STEP_1			48
#define IRIS_STEP_2			68
#define IRIS_SWITCH			EXP_1_512
#define IRIS_S2L_SWITCH		EXP_1_128
#define IRIS_L2S_SWITCH		EXP_1_512

#define AE_TARGET			160

#define AGC_BOUNDARY_L		STD_ISO_50
#define AGC_BOUNDARY_H		STD_ISO_200
#define AGC_CAP_BOUNDARY_L	STD_ISO_50
#define AGC_CAP_BOUNDARY_H	STD_ISO_200
#define EXP_BOUNDARY_L		EXP_1_32
#define EXP_BOUNDARY_H		EXP_1_2048
#define EXP_CAP_BOUNDARY_L	EXP_1_16
#define EXP_CAP_BOUNDARY_H	EXP_1_2048

#define CAP_EXP_50HZ_1_100  106
#define CAP_EXP_50HZ_1_50 	90
#define CAP_EXP_50HZ_1_30 	81
#define CAP_EXP_50HZ_1_25 	74
#define CAP_EXP_50HZ_1_20 	69
#define CAP_EXP_50HZ_1_16 	65
#define CAP_EXP_50HZ_1_8 	49
#define CAP_EXP_50HZ_1_4 	32
#define CAP_EXP_50HZ_1_2 	16
#define CAP_EXP_50HZ_1_01 	0

#define CAP_EXP_60HZ_1_120 111
#define CAP_EXP_60HZ_1_60 	96
#define CAP_EXP_60HZ_1_30 	85
#define CAP_EXP_60HZ_1_20 	69
#define CAP_EXP_60HZ_1_16 	60
#define CAP_EXP_60HZ_1_8 	48
#define CAP_EXP_60HZ_1_4 	32
#define CAP_EXP_60HZ_1_2 	16
#define CAP_EXP_60HZ_1_01 	0
#define AE_CAP_FLICKER_SHUT_NUM 9


#define PRV_EXP_50HZ_1_100  106
#define PRV_EXP_50HZ_1_50 	90
#define PRV_EXP_50HZ_1_30 	81
#define PRV_EXP_50HZ_1_20 	69
#define PRV_EXP_50HZ_1_16 	65
#define PRV_EXP_50HZ_1_8 	49
#define PRV_EXP_50HZ_1_4 	32
#define PRV_EXP_50HZ_1_2 	16
#define PRV_EXP_50HZ_1_01 	0

#define PRV_EXP_60HZ_1_120 111
#define PRV_EXP_60HZ_1_60 	96
#define PRV_EXP_60HZ_1_30 	85
#define PRV_EXP_60HZ_1_20 	69
#define PRV_EXP_60HZ_1_16 	60
#define PRV_EXP_60HZ_1_8 	48
#define PRV_EXP_60HZ_1_4 	32
#define PRV_EXP_60HZ_1_2 	16
#define PRV_EXP_60HZ_1_01 	0

#define PRV_AGC_MIN_IDX 	80

#endif


/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                               M A C R O S                              *
 **************************************************************************/

/**************************************************************************
 *          M O D U L E   V A R I A B L E S   R E F E R E N C E S         *
 **************************************************************************/

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/

#endif  /* APP_AE_PRIVATE_H */
