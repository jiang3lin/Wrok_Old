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

#ifndef _RF_DEF_H_
#define _RF_DEF_H_

extern void appRfBufferInit(void);
extern void appRfBufferRelease(void);
extern void appRfBufferConvert(void);
extern UINT32 appRfCopyBufferCallback(UINT32 fid, UINT8 *pbuf, UINT32 w, UINT32 h);
extern void appRfCopyBufferDirect(UINT8 *fileName);
extern void appRfCopyBufferCallbackSet(void);
extern void appRfCopyBufferCallbackRelease(void);
extern void  appUpdateBufferThread(ULONG para);
extern void appRfInit(UINT8 CUID[5],UINT8 nRcwMID);
extern void appRfRelease(void);
extern void appRfCUIDGenerator();
extern void appRfPairingStart(void);
extern BOOL appRfParingFinish(UINT8 CUID[5], UINT8* nRcwMID);
extern void appScdvRfInit(void);
extern UINT8 appRFSkipFunction();
extern UINT8 app_mjpeg_fifo_is_full(void);
extern int app_mjpeg_fifo_write(UINT8 *mjpeg_ptr, UINT32 mjpeg_len);
extern void appRfCdvLapseCallbackRelease(void);
extern void appGetViewSize(UINT32* pWidth, UINT32* pHeight);
extern void UsbStrSettingInit();
extern  UINT32 dispOpSecondPowerOn(void);
extern void RFRecCaptureWaitSetting();
extern void RFRecCaptureWaitRelease();
extern UINT32 appCdvAddFileStsGet(void);
//rf dual stream init or deinit
extern void RFDualStreamSampleInit(void);
extern void RFDualStreamSampleDeinit(void);
extern int GetCurrentMediaFreeSize();
extern UINT32 appCdvCdfsFreeSpaceCheck(UINT32 folder, UINT32 nSecond,BOOL bOverWrite);
extern void RFRecCaptureWaitSetting();
extern void RFRecCaptureWaitRelease();
#define RF_PREVIEW_KEEP
extern int nWifiRFMutex;
extern int nCdvLapseWifiRFMutex;
extern UINT8 nHDMIFastBoot;
extern UINT8  nSPDVSwitchResolution;
extern UINT8  nHDMIVideoRes;
extern UINT8 aviFile[64];
extern int nLapseNoFrameAction;
extern UINT8  RFSnapMovieMutex ;
#endif 
