/**************************************************************************
 *                                                                        *
 *       Copyright (c) 2005-2007 by Sunplus Technology Co., Ltd.          *
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
 *  Hsin-Chu, Taiwan, R.O.C.                                              *
 *                                                                        *
 **************************************************************************/
#include "mips.h"
#include "customization/solution_def.h"
#include "customization/dev_init.h"


#define USE_FRM_DRAM_BOOT_PARAM        0 
#define USE_CPU_TIMER5_AS_TIMESTAMPS   1

#define TIMESTAMPS_CLK_HZ  1000000
#ifdef _FPGA_HW_
 #define TIMER_BASE_CLK_HZ (MASTER_CLK * 1000000)
#else /*real chip*/
 #define TIMER_BASE_CLK_HZ 27000000
#endif
#define TIMESTAMPS_TIMER_BASE  (TIMER_BASE_CLK_HZ / TIMESTAMPS_CLK_HZ - 1)


	.extern __init
	.extern __main
#ifndef FPGA
	.extern	sysClkSwSet
#endif

	.set	noreorder

	.section .exceptRam, "ax"
	#ifdef RAM_PROGRAM
	.ascii "DRAM CODE"
	#else
	.ascii "ROM CODE "
	#endif

	/* New feature here: it allows modification of registers after the first
	 * block is read onto DRAM!
	 * NOTE! backdoor code text end can not exceed one block size
	 */
	/* in backdoor code all branches must be PC-related or PC-region
	 * accessing any static linked variable out of 0th block is not allowed
	 */
	.section .exceptRom, "ax"
	.globl	_backdoorEntry
	.ent	_backdoorEntry
_backdoorEntry:

#ifndef _FPGA_HW_
#if USE_FRM_DRAM_BOOT_PARAM /* dram parameter set by FRM */
	/*FIXME*/ /****TODO*DRAM param workaround*/
	/*workaround by CS: change DRAM read parameter for VC83*/
	lui	t2, 0xa000
	lw	t1, 0x100(t2)
	lui	t0, 0xb000
	andi	t3, t1, 0xff
	beqz	t3, $$defaultDramAdj
	srl	t1, t1, 8
	sb	t1, 0x7051(t0)
	srl	t1, t1, 8
	sb	t1, 0x7056(t0)
	lw	t2, 0x104(t2)
	srl	t1, t1, 8
	sb	t1, 0x7057(t0)
	sb	t2, 0x7058(t0)
	srl	t2, t2, 8
	sb	t2, 0x7073(t0)
	srl	t2, t2, 8
	sb	t2, 0x7080(t0)
	srl	t2, t2, 8
	j	$$dramAdjDone
	sb	t2, 0x00d1(t0)

$$defaultDramAdj:
	/* using default parameter */
	li	t1, 0x06
	sb	t1, 0x7051(t0)
	li	t1, 0xb5
	sb	t1, 0x7056(t0)
	li	t1, 0x14
	sb	t1, 0x7057(t0)
	li	t1, 0x1b
	sb	t1, 0x7058(t0)
	li	t1, 0x2a
	sb	t1, 0x7073(t0)
	sb	$0, 0x7080(t0)
	li	t1, 0xa8
	sb	t1, 0x00d1(t0)
$$dramAdjDone:
#else
	lui	t0, 0xb000
#endif
#if SPCA5330 && !SPCA6330
	lb	t1, 0x00ff(t0)
	li	t2, 0x20
	bnez t1, $$backIdDone
	nop
	sh	t2, 0x00ca(t0)
	li	t2, 1
	sb	t2, 0x00c9(t0)
	sb	t2, 0x00c8(t0)
$$backIdDone:
#endif
#else /* _FPGA_HW_ */
	lui	t0, 0xb000
#endif /* #ifndef _FPGA_HW_ */

/* move tg gpio internal pull to here for usb detect pin internal pull disable */
#ifdef SP5K_GPIO_TGL_PULL_MIN
	lui t1, SP5K_GPIO_TGL_PULL_MIN>>16
	ori t1, t1, SP5K_GPIO_TGL_PULL_MIN&0xffff
	sw	t1, 0x0108(t0)
#endif
#if SPCA5310 && !SPCA5330
	/* delay about 8ms for gpio pull stable*/
	li	t1, 0x6000
	li	t2,	1 
$$pullDelayStart:
	subu t1, t1, t2
	bnez t1, $$pullDelayStart
	nop

	/* Check PWRON1 == 1 && TGGPIO6 == 1 */
	lb	t1, (0x00c1)(t0)
	lb	t2, (0x9038)(t0)
	andi t1, t1, 0x2
	beqz	t1, $$cpuClkAdjDone
	andi t2, t2, 0x40
	beqz	t2, $$cpuClkAdjDone
	nop
#elif SPCA5330
	/*FIXME: is PWRON3 correct?*/
	/* Check backup PWRON3 == 1 */
	lb	t1, (0x1304)(t0)
	nop
	andi t1, t1, 0x8
	beqz	t1, $$cpuClkAdjDone
	nop
#endif
	la	t2, bUsbPowerOn
	nop
	sw	t1,	(0x0)(t2)
	
	li	t1, 0x0f
	sb	t1, (0x0089)(t0)
	li	t1, 0x34
	sb	t1, (0x00c4)(t0)
	li	t1, 1
	sb 	t1, (0x5105)(t0)	

$$cpuClkAdjDone:

	#ifndef RAM_PROGRAM
	li	t1, 0
	sb	t1, (0x1055)(t0)/* ROM cycle */
	#endif
	li	t1, NAND_ACT_RCV_TIME
	sb	t1, (0x4061)(t0)

	lw	t1, bUsbPowerOn
	nop
	bnez	t1, $$backdoorSignature
	nop

#ifndef _FPGA_HW_
#if 1/*def POCU_SAR0_LOWBAT_CHECK_WORKAROUND_THRESHOLD_VALUE*/
	/*t0 should be untouched and left as 0xb0000000*/
	/*FIXME*/ /****TODO*low battery check pouc workaround*/
	/*workaround by CS: config ADC SAR0 for checking battery*/
	/*workaround by CS: read ADC SAR0 value later after loaded and boot*/
	li	t1, 1
	#if SPCA5330
	sb	t1, 0x00D0(t0)
	#else
	sb	t1, 0x0074(t0)
	#endif
	lbu	t1, 0x0060(t0)
	lbu	t2, 0x0065(t0)
	ori	t1, t1, 3
	sb	t1, 0x0060(t0)
	ori	t2, t2, 0xd1
	sb	t1, 0x0065(t0)

	li	t1, 0x20
	sb	t1, 0x6450(t0)	# paudReg->tadda = 0x20
	li	t1, 0x05
	sb	t1, 0x6454(t0)	# paudReg->envref = 5, strong mode
	li	t1, 1
	sb	t1, 0x6468(t0)	# paudReg->sarclksel = 1
	sb	$0, 0x6464(t0)	# paudReg->sars = 0
	sb	t1, 0x6460(t0)	# paudReg->ensar = 1
#endif
#endif /* #ifndef _FPGA_HW_ */

#if USE_CPU_TIMER5_AS_TIMESTAMPS
	sb	$0, 0x1190(t0)	# timer5 disable
	li	t1, TIMESTAMPS_TIMER_BASE
	sh	t1, 0x1192(t0)
	li	t1, TIMESTAMPS_CLK_HZ
	sw	t1, 0x1194(t0)
	li	t1, 1
	sb	t1, 0x1190(t0)
#endif

$$backdoorSignature:

/* GPIO pull set */
#ifdef SP5K_GPIO_LMI_PULL_MIN
	lui	t1, SP5K_GPIO_LMI_PULL_MIN>>16
	ori	t1, t1, SP5K_GPIO_LMI_PULL_MIN&0xffff
	sw	t1, 0x0100(t0)
#endif
#ifdef SP5K_GPIO_DISP_PULL_MIN
	lui	t1, SP5K_GPIO_DISP_PULL_MIN>>16
	ori	t1, t1, SP5K_GPIO_DISP_PULL_MIN&0xffff
	sw	t1, 0x0104(t0)
#endif
#ifdef SP5K_GPIO_FML_PULL_MIN
	lui	t1, SP5K_GPIO_FML_PULL_MIN>>16
	ori	t1, t1, SP5K_GPIO_FML_PULL_MIN&0xffff
	sw	t1, 0x010c(t0)
#endif
#ifdef SP5K_GPIO_FMH_PULL_MIN
	lui	t1, SP5K_GPIO_FMH_PULL_MIN>>16
	ori	t1, t1, SP5K_GPIO_FMH_PULL_MIN&0xffff
	sw	t1, 0x0110(t0)
#endif
#ifdef SP5K_GPIO_GEN_PULL_MIN
	lui	t1, SP5K_GPIO_GEN_PULL_MIN>>16
	ori	t1, t1, SP5K_GPIO_GEN_PULL_MIN&0xffff
	sw	t1, 0x0114(t0)
#endif

#if SP5K_GPIO_LMI_DS0_MIN
	lui	t1, SP5K_GPIO_LMI_DS0_MIN>>16
	ori	t1, t1, SP5K_GPIO_LMI_DS0_MIN&0xffff
	sw	t1, 0x0120(t0)
#endif

#if SP5K_GPIO_DISP_DS0_MIN
	lui	t1, SP5K_GPIO_DISP_DS0_MIN>>16
	ori	t1, t1, SP5K_GPIO_DISP_DS0_MIN&0xffff
	sw	t1, 0x0128(t0)
#endif
#if SP5K_GPIO_DISP_DS1_MIN
	/*lui	t1, SP5K_GPIO_DISP_DS0_MIN>>16*/
	ori	t1, t1, SP5K_GPIO_DISP_DS1_MIN&0x000f
	sw	t1, 0x012c(t0)
#endif

#if SP5K_GPIO_TGL_DS0_MIN
	lui	t1, SP5K_GPIO_TGL_DS0_MIN>>16
	ori	t1, t1, SP5K_GPIO_TGL_DS0_MIN&0xffff
	sw	t1, 0x0130(t0)
#endif
#if SP5K_GPIO_TGL_DS1_MIN
	/*lui	t1, SP5K_GPIO_TGL_DS1_MIN>>16*/
	ori	t1, t1, SP5K_GPIO_TGL_DS1_MIN&0x03ff
	sw	t1, 0x0130(t0)
#endif

#if SP5K_GPIO_GEN_DS0_MIN
	lui	t1, SP5K_GPIO_GEN_DS0_MIN>>16
	ori	t1, t1, SP5K_GPIO_GEN_DS0_MIN&0xffff
	sw	t1, 0x0138(t0)
#endif
#if SP5K_GPIO_GEN_DS1_MIN
	/*lui	t1, SP5K_GPIO_GEN_DS1_MIN>>16*/
	ori	t1, t1, SP5K_GPIO_GEN_DS1_MIN&0x000f
	sw	t1, 0x013c(t0)
#endif

#if SP5K_GPIO_FML_DS0_MIN
	lui	t1, SP5K_GPIO_FML_DS0_MIN>>16
	ori	t1, t1, SP5K_GPIO_FML_DS0_MIN&0xffff
	sw	t1, 0x0140(t0)
#endif
#if SP5K_GPIO_FML_DS1_MIN
	lui	t1, SP5K_GPIO_FML_DS1_MIN>>16
	ori	t1, t1, SP5K_GPIO_FML_DS1_MIN&0xffff
	sw	t1, 0x0144(t0)
#endif

#if SP5K_GPIO_FMH_DS0_MIN
	lui	t1, SP5K_GPIO_FMH_DS0_MIN>>16
	ori	t1, t1, SP5K_GPIO_FMH_DS0_MIN&0xffff
	sw	t1, 0x0148(t0)
#endif
#if SP5K_GPIO_FMH_DS1_MIN
	/*lui	t1, SP5K_GPIO_FMH_DS1_MIN>>16*/
	ori	t1, t1, SP5K_GPIO_FMH_DS1_MIN&0x00ff
	sw	t1, 0x014c(t0)
#endif

	/*FIXME*/ /****TODO*fill a signature for confirming backdoor run*/
	li	t1, 0x12345678

#if MULTI_BOOT_STAGE
	/* using long calls to engage icache */
	la	t2, bkdoorNandLoadFW
	jr	t2
	sw	t1, (0x1300)(t0)
#else
	jr	ra
	sw	t1, (0x1300)(t0)
#endif
	.end	_backdoorEntry


	/*_____________________________________________________________________

	  Reset Handler:
	    1. Detect and initialize DRAM (by reading I/O trap)
	    2. Initialize address varialbes (referenced by __init())
	    3. Disable timers and clear timer's interrupt
	  _____________________________________________________________________
	*/
	.section .boot, "ax"
	.ent	__resetTrampoline
	.globl	__resetTrampoline
__resetTrampoline:

	lui	t0, 0xb000

#ifdef FPGA
  #ifndef RAM_PROGRAM
	li	t1, 0
	sb	t1, (0x1055)(t0)/* ROM cycle */
  #endif
	li	t1, NAND_ACT_RCV_TIME
	sb	t1, (0x4061)(t0)
#endif

#ifndef FPGA
  #if SPCA5210
	/* 5210 LDO enable */
	li	t1, 1
	sb	t1, 0x00c2(t0)
  #endif

  #ifdef POCU_SAR0_LOWBAT_CHECK_WORKAROUND_THRESHOLD_VALUE
	/*t0 should be untouched and left as 0xb0000000*/
	/*FIXME*/ /****TODO*low battery check pouc workaround*/
	/*workaround by CS: config ADC SAR0 for checking battery*/
	/*workaround by CS: read ADC SAR0 value later after loaded and boot*/
	/*if bat sar0 value too low, shut down POCU directly*/
	/* paudReg->snapsardata = 1 */
	li	t1, 1
	sb	t1, 0x646c(t0)
	nop
	/* val = paudReg->saro, with 10-bit 2's complement signed-extend */
	lhu	t1, 0x6470(t0)
	nop
	xori	t1, t1, 0x200	# val += 512
/*TEST*/
	sw	t1, (0x1300)(t0)

	/* if (val < THRESHOLD) shutdown() */
	sltu	t1, t1, POCU_SAR0_LOWBAT_CHECK_WORKAROUND_THRESHOLD_VALUE
	bnez	t1, $$EMERGENCY_SHUTDOWN
	nop
  #endif
#endif /* #ifndef FPGA */

	/* SDRAM initialization */
#ifndef FPGA
	/* Enable DC-DC conversion */
	li	t1, 0x02
	sb	t1, (0x00c0)(t0)
#endif /* #ifndef FPGA */
	
	li	t1, -1
	sb	zero, (0x1018)(t0)  /* Disable global interrupt */
	sw	zero, (0x100c)(t0)  /* Disable peripheral interrupt */
	sh	zero, (0x1008)(t0)  /* Disable interrupt detection */
	sw	t1,   (0x1004)(t0)  /* Clear peripheral interrupt  */

#if defined(FPGA) && defined(RAM_PROGRAM)
#else
	jal	sdramInit
	nop
#endif

#if 0 /* move enable clock to main.c */
	/* Enable clock */
	/*FIXME* only turn on essential clocks and turn on other clocks
	 * in individual device modules */
	li	t1, -1
	sb	t1, (0x0060)(t0)
	sw	t1, (0x0064)(t0)
#else
	/* Enable clock */
	li      t1, 0x3
	sb      t1, (0x0060)(t0)
	lui     t1, 0x08cb
	ori     t1, t1, 0x0247
	sw      t1, (0x0064)(t0) /* 0xb0000064 ~ 0xb0000067 = 0x088b0247 */
#endif

	/* Enable LDO AD/DA */
	nop
	li      t1, 0x01
	sb      t1, (0x00c2)(t0)

#if ROM_PROGRAM || RAM_PROGRAM
	/* text image already in place */
	/* anmin-20080910
	 * this absolute addr long call takes advantage of icache
	 */
	/* Move and clear data area */
	la	t3, __init
	jalr	t3
	nop

#else
	/* text image needs copy in __init() */
	/* anmin-20080910
	 * PC-region call due to text not init yet,
	 * after image is in place
	 */
	/* Move and clear data area */
	jal	__init
	nop

#endif

	/* Set to write-back mode */
	mfc0	t0, $20
	nop
	ori	t0, t0, (1 << 7)
	mtc0	t0, $20
	nop

	/* this call accesses static variable, only allowed after __init() */
#ifndef FPGA
	la	t3, sysClkSwSet
	jalr	t3
	li	a0, CPU_CLK
#endif

/* anmin-20080910
 * applications may choose to enable interrupt now or leave it disabled.
 * interrupt will be auto enabled in os scheduler loop.
 * applications need interrupts before entering os scheduler loop must
 * enable interrupt after variable _tx_thread_system_state in .data section
 * copied ok in __init() for ROM/RAM boot */
	mfc0	t0, $12
	nop
	ori	t0, t0, 0xfc01   /* Enable interrupt [5:0] */
	mtc0	t0, $12

/****************************/
#if 0
	li      t1, 0x02
	sb      t1, (0x00c0)(t0)

	li	t1, 0x01
	sb	t1, (0x1034)(t0)
	
$$GPIO_LOOP:
	li	t1, 0x00
	sb	t1, (0x1030)(t0)
	nop
	nop
	
	li	t1, 0x01
	sb	t1, (0x1030)(t0)
	nop
	nop

	j	$$GPIO_LOOP
	nop
#endif
/****************************/

/* anmin-20080910
 * there must be an absolute addr long call, "see start"
 * o. before the call the control could be in ROM in .boot, __main is in .text
 * o. or boot loader jumped in uncache jump uncache->cache if all text in RAM
 */
	/*_____________________________________________________________________

	  jump to __main() (never back again)
	  _____________________________________________________________________
	*/
	la	t4, __main
	jr	t4               /* Never return */
	nop
	.end	__resetTrampoline

	.ent	sdramInit
sdramInit:
	lui	t0, 0xb000

#ifdef FPGA
	/* Enable clock */
	/*FIXME* only turn on essential clocks and turn on other clocks
	 * in individual device modules */
	li	t1, -1
	sb	t1, (0x0060)(t0)
	sw	t1, (0x0064)(t0)

  #if SPCA5310 && !SPCA5330
	/* Enable DRAM parameter */
	#if 1
	li      t1, 0x08/* 64MB DDR */
	#else
	li      t1, 0x04/* 32MB DDR */
	#endif
	sb      t1, (0x7050)(t0) /* Later in init.c, the size will be calibrated */
	li      t1, 0x30
	sb      t1, (0x7053)(t0) /*cmddepth*/
	li      t1, 0x02
	sb      t1, (0x7073)(t0) /*rd_lat_sel*/
	li      t1, 0x23
	sb      t1, (0x7077)(t0) /*w_r_lat[3:0],r_w_lat[3:0]*/
	li      t1, 0x4B
	sb      t1, (0x7054)(t0) /*refintval,clock 10M*/
	li      t1, 0x04
	sb      t1, (0x7055)(t0) /*refintval,clock 10M*/
	li      t1, 0x33
	sb      t1, (0x7056)(t0) /*trfc[3:0],twrp[3:0]*//*twrp=3 for FPGA use;assume trfc>rdyend_1d*/
	li      t1, 0x00
	sb      t1, (0x705d)(t0) /*trfc[4]*/
	li      t1, 0x09
	sb      t1, (0x7057)(t0) /*trrd[2:0],trcd[2:0]*/
	li      t1, 0x09
	sb      t1, (0x7058)(t0) /*trp[3],tmrd[2:0],trp[2:0]*/
	li      t1, 0x11
	sb      t1, (0x705a)(t0) /*trc[3:0],tfaw[3:0]*/
	li      t1, 0x02
	sb      t1, (0x705b)(t0) /*trc[4],1'h0,mrscaslat[2:0]*//*actually samsung DDR CL=1 in 10MHz*/
	li      t1, 0x00
	sb      t1, (0x7120)(t0) /*cs_n*/
	li      t2, 0x02
	sb      t2, (0x70e6)(t0)
	li      t2, 0x00
	sb      t2, (0x7051)(t0)
  #endif /*SPCA5310*/
  #if SPCA5330
	/* Enable DRAM parameter */
	#if DDR
	li      t1, 0x01
	sb      t1, (0x00d0)(t0) /*mem io type*/
	li      t1, 0x40
	sb      t1, (0x7120)(t0) /*mcke_trig*/
	#if 1
	li      t1, 0x08/* 64MB DDR */
	#else
	li      t1, 0x04/* 32MB DDR */
	#endif
	sb      t1, (0x7050)(t0) /* Later in init.c, the size will be calibrated */
	li      t1, 0x30
	sb      t1, (0x7053)(t0) /*cmddepth*/
	li      t1, 0x50
	sb      t1, (0x7054)(t0) /*refintval,clock 10M*/
	li      t1, 0x07
	sb      t1, (0x7055)(t0) /*refintval,clock 10M*/
	li      t1, 0xff
	sb      t1, (0x7056)(t0) /*zqcsintvl,clock 10M*/
	li      t1, 0xff
	sb      t1, (0x7057)(t0) /*zqcsintvl,clock 10M*/
	li      t1, 0x37
	sb      t1, (0x7058)(t0) /*{1'h0,tmrd[2:0],trp[3:0]}*/
	li      t1, 0x13
	sb      t1, (0x7059)(t0) /*txsnr[7:0]*/
	li      t1, 0x01
	sb      t1, (0x705a)(t0) /*tfaw[3:0]*/
	li      t1, 0x11
	sb      t1, (0x705b)(t0) /*trc[4:0]*/
	li      t1, 0x13
	sb      t1, (0x705d)(t0) /*trfc[5:0]*/
	li      t1, 0x0c
	sb      t1, (0x705e)(t0) /*twrp[4:0]*/
	li      t1, 0x34
	sb      t1, (0x705f)(t0) /*{1'h0,trrd[2:0],1'h0,trcd[2:0] min=1}*/
	li      t1, 0x20
	sb      t1, (0x7064)(t0) /*mrscaslat*//*actually samsung DDR CL=1 in 10MHz*/
	li      t1, 0x01
	sb      t1, (0x7065)(t0) /*{tWR,resetdll}*/
	li      t1, 0x01
	sb      t1, (0x7066)(t0) /*mddr123emr[7:0]*/
	li      t1, 0x00
	sb      t1, (0x7067)(t0) /*mddr123emr[9:8]*/
	li      t1, 0x00
	sb      t1, (0x706a)(t0) /*mems10_drv*/
	li      t1, 0x00
	sb      t1, (0x706b)(t0) /*mems10_drv*/
	li      t1, 0x00
	sb      t1, (0x7073)(t0) /*rd_lat_sel*//*-1 for CL1*/
	li      t1, 0x05
	sb      t1, (0x7075)(t0) /*tr2w[3:0]*//*-1 for CL1*/
	li      t1, 0x05
	sb      t1, (0x7074)(t0) /*tw2r[4:0]*/
	li      t1, 0x02
	sb      t1, (0x7080)(t0) /*rdata_sel1 for FPGA*/
	li      t1, 0x08
	sb      t1, (0x7082)(t0) /*dlysel0*/
	li      t1, 0x00
	sb      t1, (0x7120)(t0) /*cs_n*/
	li      t2, 0x02
	sb      t2, (0x70e6)(t0)
	li      t2, 0x04
	sb      t2, (0x7051)(t0)
	#endif /* DDR */
	#ifdef MDDR
	li      t1, 0x00
	sb      t1, (0x00d0)(t0) /*mem io type*/
	li      t1, 0x40
	sb      t1, (0x7120)(t0) /*mcke_trig*/
	li      t1, 0x48
	sb      t1, (0x7050)(t0) /* Later in init.c, the size will be calibrated */
	li      t1, 0x30
	sb      t1, (0x7053)(t0) /*cmddepth*/
	li      t1, 0x50
	sb      t1, (0x7054)(t0) /*refintval,clock 10M*/
	li      t1, 0x07
	sb      t1, (0x7055)(t0) /*refintval,clock 10M*/
	li      t1, 0xff
	sb      t1, (0x7056)(t0) /*zqcsintvl,clock 10M*/
	li      t1, 0xff
	sb      t1, (0x7057)(t0) /*zqcsintvl,clock 10M*/
	li      t1, 0x16
	sb      t1, (0x7058)(t0) /*{1'h0,tmrd[2:0],trp[3:0]}*/
	li      t1, 0x22
	sb      t1, (0x7059)(t0) /*txsnr[7:0]*/
	li      t1, 0x01
	sb      t1, (0x705a)(t0) /*tfaw[3:0]*/
	li      t1, 0x0d
	sb      t1, (0x705b)(t0) /*trc[4:0]*/
	li      t1, 0x1a
	sb      t1, (0x705d)(t0) /*trfc[5:0]*/
	li      t1, 0x0b
	sb      t1, (0x705e)(t0) /*twrp[4:0]*/
	li      t1, 0x23
	sb      t1, (0x705f)(t0) /*{1'h0,trrd[2:0],1'h0,trcd[2:0] min=1}*/
	li      t1, 0x30
	sb      t1, (0x7064)(t0) /*mrscaslat*//*actually samsung MDDR CL=2 in 10MHz*/
	li      t1, 0x00
	sb      t1, (0x7065)(t0) /*{tWR,resetdll}*/
	li      t1, 0x00
	sb      t1, (0x7066)(t0) /*mddr123emr[7:0]*/
	li      t1, 0x00
	sb      t1, (0x7067)(t0) /*mddr123emr[9:8]*/
	li      t1, 0x00
	sb      t1, (0x7068)(t0) /*ddr23emr2[ 7:0]*/
	li      t1, 0x00
	sb      t1, (0x7069)(t0) /*ddr23emr2[10:8]*/
	li      t1, 0x00
	sb      t1, (0x706a)(t0) /*mems10_drv*/
	li      t1, 0x00
	sb      t1, (0x706b)(t0) /*mems10_drv*/
	li      t1, 0x01
	sb      t1, (0x7073)(t0) /*rd_lat_sel*//*-1 for CL2*/
	li      t1, 0x06
	sb      t1, (0x7074)(t0) /*tw2r[4:0]*/
	li      t1, 0x06
	sb      t1, (0x7075)(t0) /*tr2w[3:0]*//*-1 for CL2*/
	li      t1, 0x02
	sb      t1, (0x7080)(t0) /*rdata_sel1 for FPGA*/
	li      t1, 0x08
	sb      t1, (0x7082)(t0) /*dlysel0*/
	li      t1, 0x00
	sb      t1, (0x7120)(t0) /*cs_n*/
	li      t2, 0x02
	sb      t2, (0x70e6)(t0)
	li      t2, 0x04
	sb      t2, (0x7051)(t0)
	#endif
	#ifdef DDR2
	li      t1, 0x00
	sb      t1, (0x00d0)(t0) /*mem io type*/
	li      t1, 0x40
	sb      t1, (0x7120)(t0) /*mcke_trig*/
	li      t1, 0xd0
	sb      t1, (0x7050)(t0) /* Later in init.c, the size will be calibrated */
	li      t1, 0x30
	sb      t1, (0x7053)(t0) /*cmddepth*/
	li      t1, 0x50
	sb      t1, (0x7054)(t0) /*refintval,clock 10M*/
	li      t1, 0x07
	sb      t1, (0x7055)(t0) /*refintval,clock 10M*/
	li      t1, 0xff
	sb      t1, (0x7056)(t0) /*zqcsintvl,clock 10M*/
	li      t1, 0xff
	sb      t1, (0x7057)(t0) /*zqcsintvl,clock 10M*/
	li      t1, 0x16
	sb      t1, (0x7058)(t0) /*{1'h0,tmrd[2:0],trp[3:0]}*/
	li      t1, 0x22
	sb      t1, (0x7059)(t0) /*txsnr[7:0]*/
	li      t1, 0x0c
	sb      t1, (0x705a)(t0) /*tfaw[3:0]*/
	li      t1, 0x0d
	sb      t1, (0x705b)(t0) /*trc[4:0]*/
	li      t1, 0x1f
	sb      t1, (0x705d)(t0) /*trfc[5:0]*/
	li      t1, 0x0c
	sb      t1, (0x705e)(t0) /*twrp[4:0]*/
	li      t1, 0x21
	sb      t1, (0x705f)(t0) /*{1'h0,trrd[2:0],1'h0,trcd[2:0] min=1}*/
	li      t1, 0x30
	sb      t1, (0x7064)(t0) /*mrscaslat*//*actually samsung DDR2 CL=2 in 10MHz*/
	li      t1, 0x07
	sb      t1, (0x7065)(t0) /*{tWR,resetdll}*/
	li      t1, 0x09
	sb      t1, (0x7066)(t0) /*mddr123emr[7:0]*/
	li      t1, 0x00
	sb      t1, (0x7067)(t0) /*mddr123emr[9:8]*/
	li      t1, 0x00
	sb      t1, (0x7068)(t0) /*ddr23emr2[ 7:0]*/
	li      t1, 0x00
	sb      t1, (0x7069)(t0) /*ddr23emr2[10:8]*/
	li      t1, 0x00
	sb      t1, (0x706a)(t0) /*mems10_drv*/
	li      t1, 0x00
	sb      t1, (0x706b)(t0) /*mems10_drv*/
	li      t1, 0x02
	sb      t1, (0x7073)(t0) /*rd_lat_sel*//*-1 for CL2*/
	li      t1, 0x07
	sb      t1, (0x7074)(t0) /*tw2r[4:0]*/
	li      t1, 0x05
	sb      t1, (0x7075)(t0) /*tr2w[3:0]*//*-1 for CL2*/
	li      t1, 0x02
	sb      t1, (0x7080)(t0) /*rdata_sel1 for FPGA*/
	li      t1, 0x08
	sb      t1, (0x7082)(t0) /*dlysel0*/
	li      t1, 0x00
	sb      t1, (0x7120)(t0) /*cs_n*/
	li      t2, 0x02
	sb      t2, (0x70e6)(t0)
	li      t2, 0x04
	sb      t2, (0x7051)(t0)
	#endif /* DDR2 */
	#ifdef DDR3
	li      t1, 0x00
	sb      t1, (0x00d0)(t0) /*mem io type*/
	li      t1, 0x40
	sb      t1, (0x7120)(t0) /*mcke_trig*/
	li      t1, 0x90
	sb      t1, (0x7050)(t0) /* Later in init.c, the size will be calibrated */
	li      t1, 0x30
	sb      t1, (0x7053)(t0) /*cmddepth*/
	li      t1, 0x50
	sb      t1, (0x7054)(t0) /*refintval,clock 10M*/
	li      t1, 0x07
	sb      t1, (0x7055)(t0) /*refintval,clock 10M*/
	li      t1, 0xff
	sb      t1, (0x7056)(t0) /*zqcsintvl,clock 10M*/
	li      t1, 0x04
	sb      t1, (0x7057)(t0) /*zqcsintvl,clock 10M*/
	li      t1, 0x3a
	sb      t1, (0x7058)(t0) /*{1'h0,tmrd[2:0],trp[3:0]}*/
	li      t1, 0x1e
	sb      t1, (0x7059)(t0) /*txsnr[7:0]*/
	li      t1, 0x09
	sb      t1, (0x705a)(t0) /*tfaw[3:0]*/
	li      t1, 0x0c
	sb      t1, (0x705b)(t0) /*trc[4:0]*/
	li      t1, 0x1a
	sb      t1, (0x705d)(t0) /*trfc[5:0]*/
	li      t1, 0x13
	sb      t1, (0x705e)(t0) /*twrp[4:0]*/
	li      t1, 0x31
	sb      t1, (0x705f)(t0) /*{1'h0,trrd[2:0],1'h0,trcd[2:0] min=1}*/
	li      t1, 0x10
	sb      t1, (0x7064)(t0) /*mrscaslat*//*actually samsung DDR3 CL=4 in 10MHz*/
	li      t1, 0x03
	sb      t1, (0x7065)(t0) /*{tWR,resetdll}*/
	li      t1, 0x11
	sb      t1, (0x7066)(t0) /*mddr123emr[7:0]*/
	li      t1, 0x00
	sb      t1, (0x7067)(t0) /*mddr123emr[9:8]*/
	li      t1, 0x00
	sb      t1, (0x7068)(t0) /*ddr23emr2[ 7:0]*/
	li      t1, 0x00
	sb      t1, (0x7069)(t0) /*ddr23emr2[10:8]*/
	li      t1, 0x00
	sb      t1, (0x706a)(t0) /*mems10_drv*/
	li      t1, 0x00
	sb      t1, (0x706b)(t0) /*mems10_drv*/
	li      t1, 0x06
	sb      t1, (0x7073)(t0) /*rd_lat_sel*//*-1 for CL4*/
	li      t1, 0x0c
	sb      t1, (0x7074)(t0) /*tw2r[4:0]*/
	li      t1, 0x04
	sb      t1, (0x7075)(t0) /*tr2w[3:0]*//*-1 for CL4*/
	li      t1, 0x02
	sb      t1, (0x7080)(t0) /*rdata_sel1 for FPGA*/
	li      t1, 0x08
	sb      t1, (0x7082)(t0) /*dlysel0*/
	li      t1, 0x00
	sb      t1, (0x7120)(t0) /*cs_n*/
	li      t2, 0x02
	sb      t2, (0x70e6)(t0)
	li      t2, 0x00
	sb      t2, (0x7051)(t0)
	#endif  /* DDR3 */
  #endif /*SPCA5330*/
#endif

	jr	ra
	nop

	.end	sdramInit

#ifndef RAM_PROGRAM
	.globl	addrInit
	.ent	addrInit
addrInit:
	la	k0, _dram_content_lma_start
	la	k1, memProfile
	sw	k0, (0x0)(k1)
	la	k0, _dram_content_vma_start
	jr	ra
	sw	k0, (0x4)(k1)
	.end	addrInit
#endif

