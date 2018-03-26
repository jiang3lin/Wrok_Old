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
 #ifndef _SOLUTION_DEF_H_
#define _SOLUTION_DEF_H_


/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
/* select one of the following platform */
/* FPGA */
#undef _FPGA_HW_

/* Customer's HW */
#define _CUSTOM_HW_ 1

/* EVB */
#undef _EVB_SPE7001_HW_

#if _FPGA_HW_ + _CUSTOM_HW_ + _EVB_SPE7001_HW_ != 1
#error select one platform!
#endif
/*-- Project Configuration */

#define KIT_WITH_LENS	 0
#define KIT_WITH_FOCUS	 0
#define KIT_WITH_MSHUT	 0
#define KIT_WITH_IRIS	 0
#define KIT_SHUT_CTRL_TIMER	 0
#define KIT_FLASH_ENABLE 0
#define KIT_LMI_CTRL		 0

#define LENS_MODEL_DUMMY
#define PIP_32M    0
#define KIT_SENSOR_FIT_IMAGE 1 
#define FACE_WITH_AE  0
#if 0
#if SPCA6330
#define PANORAMA_MODE 1
#else
#define PANORAMA_MODE 0
#endif
#endif

#undef LENS_ZOOM_SLIP_DYNAMIC_ADJ
#endif  /* _SOLUTION_DEF_H_ */



