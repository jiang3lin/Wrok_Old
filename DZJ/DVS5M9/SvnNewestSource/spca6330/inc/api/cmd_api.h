/**************************************************************************
 *                                                                        *
 *         Copyright (c) 2004 by Sunplus Technology Co., Ltd.             *
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
 *  Author:                                                               *
 *                                                                        *
 **************************************************************************/
#ifndef _CMD_API_H_
#define _CMD_API_H_

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
void cmdDefaultSet(UINT32);
void cmdMonitor(void);
void cmdMonitorChar(int c);
void cmdCUnitInit(void);
void cmdUsbInit(void);
void cmdGpeInit(void);
void cmdMemInit(void);
void cmdOsInit(void);
void cmdDosInit(void);
void cmdErrInit(void);
void cmdMediaInit(void);
void cmdIspInit(void);
void cmdHwInit(void);
void cmdSnapInit(void);
void cmdIcatInit(void);
void cmdNandInit(void);
void cmdDiskInit(void);
void cmdSpiInit(void);
void cmdLmiInit(void);
void cmdSdInit(void);
void cmdGpioInit(void);
void cmdAaaInit(void);
void cmdApiMonInit(void);
void cmdRawYuvInit(void);
void cmdCdspInit(void);
void cmdOsdInit(void);
void cmdModeswInit(void);

#endif  /* _CMD_API_H_ */
