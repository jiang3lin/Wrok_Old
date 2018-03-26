/**************************************************************************
 *                                                                        *
 *         Copyright (c) 2007 by Sunplus Technology Co., Ltd.             *
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
 *  Hsin-Chu, Taiwan.                                                     *
 *                                                                        *
 *  Author: Anmin Deng                                                    *
 *                                                                        *
 **************************************************************************/
#ifndef _TX_API__H_
#define _TX_API__H_

#ifdef   __cplusplus
extern   "C" {
#endif

#include "_tx_port.h"

/* API input parameters and general constants.  */

#define TX_NO_WAIT          0
#define TX_WAIT_FOREVER     0xFFFFFFFFUL
#define TX_AND              2
#define TX_AND_CLEAR        3
#define TX_OR               0
#define TX_OR_CLEAR         1
#define TX_1_ULONG          1
#define TX_2_ULONG          2
#define TX_4_ULONG          4
#define TX_8_ULONG          8
#define TX_16_ULONG         16
#define TX_NO_TIME_SLICE    0
#define TX_MAX_PRIORITIES   32
#define TX_AUTO_START       1
#define TX_DONT_START       0
#define TX_AUTO_ACTIVATE    1
#define TX_NO_ACTIVATE      0
#define TX_TRUE             1
#define TX_FALSE            0
#define TX_NULL             0
#define TX_LOOP_FOREVER     1
#define TX_INHERIT          1
#define TX_NO_INHERIT       0


#ifdef   __cplusplus
}
#endif

#endif /* _TX_API__H_ */
