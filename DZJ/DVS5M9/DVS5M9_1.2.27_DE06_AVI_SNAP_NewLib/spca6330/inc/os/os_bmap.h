/**************************************************************************
 *                                                                        *
 *      Copyright (c) 2004,2005,2006 by Sunplus Technology Co., Ltd.      *
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
 *  Author: Anmin Deng                                                    *
 *                                                                        *
 **************************************************************************/
/* DO NOT EDIT the next 2 lines for auto revision by subversion
$Rev: 15140 $
$Date:: 2013-01-30 18:34:05#$
 */
#ifndef _OS_BMAP_H_
#define _OS_BMAP_H_

#ifdef __cplusplus
extern "C" {
#endif

#define _os_bitmap_msb_to_idx  __clz_tab

extern const unsigned char _os_idx_to_bitmap[] __attribute__ ((section(".sdata")));

extern const unsigned char __clz_tab[] __attribute__ ((section(".sdata")));
extern const unsigned char _os_bitmap_lsb_to_idx[] __attribute__ ((section(".sdata")));


/**
 * \return bit width of non-zero bits of value t, 0 if t==0
 */
extern unsigned int u32bitWidthGet(unsigned int t);

__inline__ static unsigned int u8bitWidthGet(unsigned int t)
		__attribute__((always_inline));
unsigned int
u8bitWidthGet(
		unsigned int t
		)
{
	return _os_bitmap_msb_to_idx[t];
}


/**
 * \return the least significant non-zero bit position of value t, 0 if t==0
 */
__inline__ static unsigned int u8Lsb1PosGet(unsigned int t)
		__attribute__((always_inline));
unsigned int
u8Lsb1PosGet(
		unsigned int t
		)
{
	return _os_bitmap_lsb_to_idx[t];
}

extern unsigned int u16Lsb1PosGet(unsigned int t);
extern unsigned int u32Lsb1PosGet(unsigned int t);


#ifdef __cplusplus
}
#endif

#endif
