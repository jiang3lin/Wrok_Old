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
 *  Author: Richie Lee                                                    *
 *                                                                        *
 **************************************************************************/
#ifndef _VERSION_API_H_
#define _VERSION_API_H_

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
/**
 * \note string length should be less than 16
 * MAIN means main trunk,
 * BR## means branch and ## is branch number.
 * e.g. 00 is zfupd test branch
 */
#define SP5K_RELEASE_VERSION "BR11-00.23.03"
#define BASEFW_RELEASE_VERSION "19393"
#define BASEFW_REVISION_CODE 19393


/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef void (*verHandler)(UINT8 *param);

typedef struct ver_s {
	const UINT8  *verStr;
	verHandler   phandler;
} ver_t;

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
void sysVersionGet(void);
void osVersionGet(UINT8 *pbuf);
void snapLibVersionGet(UINT8 *pbuf);
void avLibVersionGet(UINT8 *pbuf);
void audLibVersionGet(UINT8 *pbuf);
void libMsgLibVersionGet(UINT8 *pbuf);
void libResourceLibVersionGet(UINT8 *pbuf);
void libSysgLibVersionGet(UINT8 *pbuf);
void sp5kCaptureLibVersionGet(UINT8 *pbuf);
void sp5kCdspLibVersionGet(UINT8 *pbuf);
void cdspLibVersionGet(UINT8 *pbuf);
void libDiskLibVersionGet(UINT8 *pbuf);
void libBpLibVersionGet(UINT8 *pbuf);
void sensorLibVersionGet(UINT8 *pbuf);
void sensorModelLibVersionGet(UINT8 *pbuf);
void aaaLibVersionGet(UINT8 *pbuf);
void sp5kDiskLibVersionGet(UINT8 *pbuf);
void sp5kUtilLibVersionGet(UINT8 *pbuf);
void sp5k3aLibVersionGet(UINT8 *pbuf);
void sp5kCalibLibVersionGet(UINT8 *pbuf);
void pbLibVersionGet(UINT8 *pbuf);
void rsvblkLibVersionGet(UINT8 *pbuf);
void sp5kRsvblkLibVersionGet(UINT8 *pbuf);
void sp5kPbLibVersionGet(UINT8 *pbuf);
void sp5kaudLibVersionGet(UINT8 *pbuf);
void sp5kmediaLibVersionGet(UINT8 *pbuf);
void sp5kDcfLibVersionGet(UINT8 *pbuf);
void vcLibVersionGet(UINT8 *pbuf);
void sp5kUsbLibVersionGet(UINT8 *pbuf);
void dcfLibVersionGet(UINT8 *pbuf);
void otgLibVersionGet(UINT8 *pbuf);
void dpsLibVersionGet(UINT8 *pbuf);
void msdcLibVersionGet(UINT8 *pbuf);
void sidcLibVersionGet(UINT8 *pbuf);
void usbLibVersionGet(UINT8 *pbuf);
void usbVndreqLibVersionGet(UINT8 *pbuf);
void customizationLibVersionGet(UINT8 *pbuf);
void sysbootLibVersionGet(UINT8 *pbuf);
void dispDrvLibVersionGet(UINT8 *pbuf);
void disp5kVersionGet(UINT8 *pbuf);
void dzoomLibVersionGet(UINT8 *pbuf);
void fntLibVersionGet(UINT8 *pbuf);
void globalLibVersionGet(UINT8 *pbuf);
void modeswLibVersionGet(UINT8 *pbuf);
void ftrackLibVersionGet(UINT8 *pbuf);
void sp5kFaceTrackLibVersionGet(UINT8 *pbuf);
void strLibVersionGet(UINT8 *pbuf);
void pipLibVersionGet(UINT8 *pbuf);
void gfxLibVersionGet(UINT8 *pbuf);
void slideLibVersionGet(UINT8 *pbuf);
void fsLibVersionGet(UINT8 *pbuf);
void sp5kDispLibVersionGet(UINT8 *pbuf);
void sp5kDzoomLibVersionGet(UINT8 *pbuf);
void sp5kPipLibVersionGet(UINT8 *pbuf);
void sp5kGfxLibVersionGet(UINT8 *pbuf);
void sp5kGlobalLibVersionGet(UINT8 *pbuf);
void sp5kModeswLibVersionGet(UINT8 *pbuf);
void sp5kSlideLibVersionGet(UINT8 *pbuf);
void halVersionGet(UINT8 *pbuf);
void sp5kDpofLibVersionGet(UINT8 *pbuf);
void sp5kExifLibVersionGet(UINT8 *pbuf);
void dpofLibVersionGet(UINT8 *pbuf);
void exifLibVersionGet(UINT8 *pbuf);
void utilLibVersionGet(UINT8 *pbuf);
void drvLibVersionGet(UINT8 *pbuf);
void libTableLibVersionGet(UINT8 *pbuf);
void sp5kFsLibVersionGet(UINT8 *pbuf);
void sp5kOsLibVersionGet(UINT8 *pbuf);
void effLibVersionGet(UINT8 *pbuf);
void vfsLibVersionGet(UINT8 *pbuf);
void dbgLibVersionGet(UINT8 *pbuf);
void halHwVersionGet(UINT8 *pbuf);
#ifdef SPCA5210
void sp5kLmiLibVersionGet(UINT8 *pbuf);
#endif

#endif /* _VERSION_API_H_ */

