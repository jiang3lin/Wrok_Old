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
#ifndef _REG_FRONT_H_
#define _REG_FRONT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "reg_def.h"

/**************************************************************************
 *                          D A T A    T Y P E S
 **************************************************************************/
typedef struct frontReg_s {
	volatile UINT8  regOffset[0x9000];
	volatile UINT32 fintevt;            /* 0x9000 ~ 0x9003 */
	volatile UINT8  rsv4[4];            /* 0x9004 ~ 0x9007 */
	volatile UINT32 finten;             /* 0x9008 ~ 0x900b */
	volatile UINT32 signalstate;        /* 0x900c ~ 0x900f */
	volatile UINT32 senrst;             /* 0x9010 ~ 0x9013 */
	volatile UINT8  rsv20[12];          /* 0x9014 ~ 0x901f */
	volatile UINT16 disgatedfpclk;      /* 0x9020 ~ 0x9021 */
	volatile UINT16 disgatedfpclknx;    /* 0x9022 ~ 0x9023 */
	volatile UINT16 disgatedfsclk;      /* 0x9024 ~ 0x9025 */
	volatile UINT8  disgatedflclk;      /* 0x9026 ~ 0x9026 */
	volatile UINT8  rsv39[9];           /* 0x9027 ~ 0x902f */
	volatile UINT16 tggpiooe;           /* 0x9030 ~ 0x9031 */
	volatile UINT16 tggpioo;            /* 0x9032 ~ 0x9033 */
	volatile UINT16 tggpioosel;         /* 0x9034 ~ 0x9035 */
	volatile UINT16 tggpioopol;         /* 0x9036 ~ 0x9037 */
	volatile UINT16 tggpioi;            /* 0x9038 ~ 0x9039 */
	volatile UINT16 tggpioipol;         /* 0x903a ~ 0x903b */
	volatile UINT8  rsv60[2];           /* 0x903c ~ 0x903d */
	volatile UINT8  tgctroe;            /* 0x903e ~ 0x903e */
	volatile UINT8  tgctropol;          /* 0x903f ~ 0x903f */
	volatile UINT8  tgctri;             /* 0x9040 ~ 0x9040 */
	volatile UINT8  tgctripol;          /* 0x9041 ~ 0x9041 */
#if SPCA5210 /*sine pwm*/
	volatile UINT16 sinpwmen;           /* 0x9042 ~ 0x9043 */
#else
	volatile UINT8  rsv66[2];           /* 0x9042 ~ 0x9043 */
#endif
	volatile UINT16 lmigpiooe;          /* 0x9044 ~ 0x9045 */
	volatile UINT16 lmigpioo;           /* 0x9046 ~ 0x9047 */
#if SPCA5210 /*pwm pair*/
	volatile UINT16 lmigpioosel;        /* 0x9048 ~ 0x9049 */
	volatile UINT16 pwmpairen;          /* 0x904a ~ 0x904b */
	volatile UINT16 lmigpioi;           /* 0x904c ~ 0x904d */
	volatile UINT16 syncword[3];        /* 0x904e ~ 0x9053 */
#else
	volatile UINT32 lmigpioosel;        /* 0x9048 ~ 0x904b */
	volatile UINT32 lmigpioi;           /* 0x904c ~ 0x904f */
	volatile UINT8  rsv80[4];           /* 0x9050 ~ 0x9053 */
#endif
	volatile UINT16 intline0;           /* 0x9054 ~ 0x9055 */
	volatile UINT16 intline1;           /* 0x9056 ~ 0x9057 */
	volatile UINT16 intline2;           /* 0x9058 ~ 0x9059 */
	volatile UINT16 intline3;           /* 0x905a ~ 0x905b */
	volatile UINT16 intpixel0;          /* 0x905c ~ 0x905d */
	volatile UINT16 intpixel1;          /* 0x905e ~ 0x905f */
	volatile UINT16 intpixel2;          /* 0x9060 ~ 0x9061 */
	volatile UINT16 intpixel3;          /* 0x9062 ~ 0x9063 */
#if SPCA5210 /*div cnt*/
	volatile UINT32 intpcntdivsel;      /* 0x9064 ~ 0x9067 */
#else
	volatile UINT8  rsv100[4];          /* 0x9064 ~ 0x9067 */
#endif
	volatile UINT16 snaptrg;            /* 0x9068 ~ 0x9069 */
	volatile UINT16 snapnum;            /* 0x906a ~ 0x906b */
	volatile UINT16 fldsel;             /* 0x906c ~ 0x906d */
	volatile UINT16 fldnum;             /* 0x906e ~ 0x906f */
	volatile UINT8  frontrgbadj;        /* 0x9070 ~ 0x9070 */
	volatile UINT8  clpobiadj;          /* 0x9071 ~ 0x9071 */
	volatile UINT16 decodetype;         /* 0x9072 ~ 0x9073 */
	volatile UINT16 diffintv;           /* 0x9074 ~ 0x9075 */
	volatile UINT16 refnum;             /* 0x9076 ~ 0x9077 */
#if SPCA5210 /*lvds*/
	volatile UINT8  datawidth;          /* 0x9078 ~ 0x9078 */
	volatile UINT8  lvdsen;             /* 0x9079 ~ 0x9079 */
	volatile UINT16 ctrlword;           /* 0x907a ~ 0x907b */
	volatile UINT32 s2pphase;           /* 0x907c ~ 0x907f */
#else
	volatile UINT32 datawidth;          /* 0x9078 ~ 0x907b */
	volatile UINT8  rsv124[4];          /* 0x907c ~ 0x907f */
#endif
	volatile UINT16 front2cdspgating;   /* 0x9080 ~ 0x9081 */
	volatile UINT16 hvldsrc;            /* 0x9082 ~ 0x9083 */
	volatile UINT32 syncgenen;          /* 0x9084 ~ 0x9087 */
	volatile UINT16 htotal;             /* 0x9088 ~ 0x9089 */
	volatile UINT16 lasthtotal;         /* 0x908a ~ 0x908b */
	volatile UINT16 hblank;             /* 0x908c ~ 0x908d */
	volatile UINT16 vtotal;             /* 0x908e ~ 0x908f */
	volatile UINT16 vblank;             /* 0x9090 ~ 0x9091 */
	volatile UINT16 vdlatepixel;        /* 0x9092 ~ 0x9093 */
	volatile UINT8  hdorgupdate;        /* 0x9094 ~ 0x9094 */
	volatile UINT8  allowupdate;        /* 0x9095 ~ 0x9095 */
	volatile UINT8  rsv150[4];          /* 0x9096 ~ 0x9099 */
	volatile UINT8  hdiven;             /* 0x909a ~ 0x909a */
	volatile UINT8  hdivcntsft;         /* 0x909b ~ 0x909b */
	volatile UINT16 hoffset2;           /* 0x909c ~ 0x909d */
	volatile UINT16 hoffset3;           /* 0x909e ~ 0x909f */
	volatile UINT32 vvldintv;           /* 0x90a0 ~ 0x90a3 */
	volatile UINT16 hsize;              /* 0x90a4 ~ 0x90a5 */
	volatile UINT16 hoffset;            /* 0x90a6 ~ 0x90a7 */
	volatile UINT16 vsize;              /* 0x90a8 ~ 0x90a9 */
	volatile UINT16 voffset;            /* 0x90aa ~ 0x90ab */
	volatile UINT16 hvldupdate;         /* 0x90ac ~ 0x90ad */
#if SPCA5210 /*hdvd before reshape pol*/
	volatile UINT8  hrshpen;            /* 0x90ae ~ 0x90ae */
	volatile UINT8  hdipol;             /* 0x90af ~ 0x90af */
#else
	volatile UINT16 hrshpen;            /* 0x90ae ~ 0x90af */
#endif
	volatile UINT16 hrise;              /* 0x90b0 ~ 0x90b1 */
	volatile UINT16 hfall;              /* 0x90b2 ~ 0x90b3 */
	volatile UINT16 vrise;              /* 0x90b4 ~ 0x90b5 */
	volatile UINT16 vfall;              /* 0x90b6 ~ 0x90b7 */
	volatile UINT8  rsv184[2];          /* 0x90b8 ~ 0x90b9 */
	volatile UINT8  hdrshpupdate;       /* 0x90ba ~ 0x90ba */
	volatile UINT8  testrstclktgle;     /* 0x90bb ~ 0x90bb */
	volatile UINT16 testhsize;          /* 0x90bc ~ 0x90bd */
	volatile UINT16 testvsize;          /* 0x90be ~ 0x90bf */
	volatile UINT16 tvdecen;            /* 0x90c0 ~ 0x90c1 */
	volatile UINT16 bt656en;            /* 0x90c2 ~ 0x90c3 */
#if SPCA5210 /*fldpol removed*/
	volatile UINT8  rsv196[2];          /* 0x90c4 ~ 0x90c5 */
#else
	volatile UINT16 fldpol;             /* 0x90c4 ~ 0x90c5 */
#endif
	volatile UINT16 uvsel;              /* 0x90c6 ~ 0x90c7 */
	volatile UINT32 ys128en;            /* 0x90c8 ~ 0x90cb */
	volatile UINT8  rsv204[4];          /* 0x90cc ~ 0x90cf */
	volatile UINT16 siggenen;           /* 0x90d0 ~ 0x90d1 */
	volatile UINT8  sigyuven;           /* 0x90d2 ~ 0x90d2 */
	volatile UINT8  siggenrst;          /* 0x90d3 ~ 0x90d3 */
	volatile UINT16 sentype;            /* 0x90d4 ~ 0x90d5 */
	volatile UINT16 sigmode;            /* 0x90d6 ~ 0x90d7 */
	volatile UINT32 firstpixel;         /* 0x90d8 ~ 0x90db */
	volatile UINT8  rsv220[4];          /* 0x90dc ~ 0x90df */
	volatile UINT16 flashmode;          /* 0x90e0 ~ 0x90e1 */
	volatile UINT16 flashtrg;           /* 0x90e2 ~ 0x90e3 */
	volatile UINT16 ftline;             /* 0x90e4 ~ 0x90e5 */
	volatile UINT16 ftpixel;            /* 0x90e6 ~ 0x90e7 */
	volatile UINT32 flashwidth;         /* 0x90e8 ~ 0x90eb */
	volatile UINT8  rsv236[4];          /* 0x90ec ~ 0x90ef */
	volatile UINT16 mshmode;            /* 0x90f0 ~ 0x90f1 */
	volatile UINT16 mshtrg;             /* 0x90f2 ~ 0x90f3 */
	volatile UINT16 mtline;             /* 0x90f4 ~ 0x90f5 */
	volatile UINT16 mtpixel;            /* 0x90f6 ~ 0x90f7 */
	volatile UINT16 probesel;           /* 0x90f8 ~ 0x90f9 */
	volatile UINT16 frontprobe;         /* 0x90fa ~ 0x90fb */
	volatile UINT32 datecode;           /* 0x90fc ~ 0x90ff */
} frontReg0_t;


typedef struct front3Reg_s {
	volatile UINT8  regOffset[0x9300];
	volatile UINT8  lmidir;             /* 0x9300 ~ 0x9300 */
	volatile UINT8  lmiunit;            /* 0x9301 ~ 0x9301 */
	volatile UINT8  lmistartmode;       /* 0x9302 ~ 0x9302 */
	volatile UINT8  lmitotalstate;      /* 0x9303 ~ 0x9303 */
	volatile UINT8  lmiintmshen;        /* 0x9304 ~ 0x9304 */
	volatile UINT8  lmitrg;             /* 0x9305 ~ 0x9305 */
#if SPCA5210
	volatile UINT8  lmiabort;           /* 0x9306 ~ 0x9306 */
	volatile UINT8  lmiunit2;           /* 0x9307 ~ 0x9307 */
#else
	volatile UINT16 lmiabort;           /* 0x9306 ~ 0x9307 */
#endif
	volatile UINT16 lmitotalstep;       /* 0x9308 ~ 0x9309 */
	volatile UINT8  lmistartstate;      /* 0x930a ~ 0x930a */
	volatile UINT8  lmisnap;            /* 0x930b ~ 0x930b */
	volatile UINT16 lmisnapstep;        /* 0x930c ~ 0x930d */
	volatile UINT16 lmistopstep;        /* 0x930e ~ 0x930f */
	volatile UINT16 lmidelay[8];        /* 0x9310 ~ 0x9311 */
	volatile UINT16 lmilongdelay;       /* 0x9320 ~ 0x9321 */
	volatile UINT16 lmibiten;           /* 0x9322 ~ 0x9323 */
	volatile UINT16 lmipat[8];          /* 0x9324 ~ 0x9325 */
	volatile UINT32 lmistop;            /* 0x9334 ~ 0x9337 */
#if SPCA5210 /*sine pwm*/
	volatile UINT8  fixlen;             /* 0x9338 ~ 0x9338 */
	volatile UINT8  nonstop;            /* 0x9339 ~ 0x9339 */
	volatile UINT8  rsv58[6];           /* 0x933a ~ 0x933f */
	volatile UINT8  reg9340;            /* 0x9340 ~ 0x9340 */
	volatile UINT8  reg9341;            /* 0x9341 ~ 0x9341 */
	volatile UINT8  rsv66[62];          /* 0x9342 ~ 0x937f */
	volatile UINT16 sramdata[64];       /* 0x9380 ~ 0x93ff */
	volatile UINT8  rsv256[3068];       /* 0x9400 ~ 0x9ffb */
#else
	volatile UINT8  rsv56[72];          /* 0x9338 ~ 0x937f */
	volatile UINT8  reg9380;            /* 0x9380 ~ 0x9380 */
	volatile UINT8  reg9381;            /* 0x9381 ~ 0x9381 */
	volatile UINT8  reg9382;            /* 0x9382 ~ 0x9382 */
	volatile UINT8  reg9383;            /* 0x9383 ~ 0x9383 */
	volatile UINT8  reg9384;            /* 0x9384 ~ 0x9384 */
	volatile UINT8  reg9385;            /* 0x9385 ~ 0x9385 */
	volatile UINT8  reg9386;            /* 0x9386 ~ 0x9386 */
	volatile UINT8  reg9387;            /* 0x9387 ~ 0x9387 */
	volatile UINT8  rsv136[3188];       /* 0x9388 ~ 0x9ffb */
#endif
	volatile UINT32 datecode;           /* 0x9ffc ~ 0x9fff */
} frontReg3_t;

#ifdef __cplusplus
}
#endif

#endif /* _REG_FRONT_H_ */

