/****************************************************************************
 *                                                                          *
 *         Copyright (c) 2012 by Sunplus-prof Technology Co., Ltd.          *
 *                                                                          *
 *  This software is copyrighted by and is the property of Sunplus-prof     *
 *  Technology Co., Ltd. All rights are reserved by Sunplus-prof Technology *
 *  Co., Ltd. This software may only be used in accordance with the         *
 *  corresponding license agreement. Any unauthorized use, duplication,     *
 *  distribution, or disclosure of this software is expressly forbidden.    *
 *                                                                          *
 *  This Copyright notice MUST not be removed or modified without prior     *
 *  written consent of Sunplus-prof Technology Co., Ltd.                    *
 *                                                                          *
 *  Sunplus-prof Technology Co., Ltd. reserves the right to modify this     *
 *  software without notice.                                                *
 *                                                                          *
 *  Sunplus-prof Technology Co., Ltd.                                       *
 *  2nd Floor,R2-B,Hi-tech.S,7th Road, Shenzhen High-tech Industrial Park   *
 *  Shenzhen, China                                                         *
 *                                                                          *
 *  Author: Joe Zhao                                                        *
 *                                                                          *
 ****************************************************************************/
/* Sunplus-prof   Joe Zhao   July,07,2015  10:34:07 For SPCA6330A */
#ifndef _MENU_API_H_
#define _MENU_API_H_

#include "app_com_def.h"

typedef UINT32 (*menufunc) (UINT8);
typedef UINT32 (*menuexe) (UINT8, UINT8, UINT32, UINT32);

UINT32 appMenuEvt_ModeSportDV (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_SdvVideoModeInit (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_SdvVideoModeSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_SdvVideoNormalSizeSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_SdvVideoSlowTimeSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_SdvPhotoModeInit (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_SdvPhotoModeSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_SdvPhotoNormalSizeSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_SdvPhotoBurstTimeSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_SdvPhotoSelfTimerSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_ScdvSensorFlipSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_SdvCarDvSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_SdvExitSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_ModeCarDV (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_CdvVideoModeInit (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_CdvVideoModeSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_CdvVideoNormalSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_CdvVideoLapseSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_CdvLoopingTimeSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_CdvGsensorTriggerSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_CdvGsensorSensitivitySet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_CdvMicPhoneSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_CdvSportDvSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_CdvExitSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_ModeSystem1 (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_SystemDisSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_SystemWdrSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_SystemLedIndicatorSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_SystemOsdFlipSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_SystemOsdBrightnessSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_SystemBeepVolumeSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_SystemDateStampSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_SystemAutoPowerOffSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_System1ExitSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_ModeSystem2 (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_SystemDateTimeInit (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_SystemDateTimeSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_SystemRemoterSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_SystemFrequnecySet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_SystemFormatSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_SystemDefaultSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_SystemWifiPasswordInit (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_SystemWifiPasswordSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_SystemVersionInit (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_SystemVersionSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_SystemExitSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_ModeMenuExit (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_ModeDelete (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_DeleteAllOKSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_DeleteLastOKSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_DeleteMenuExit (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_ModeDeleteLoop (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_DeleteLoopAllOKSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_DeleteLoopLastOKSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_DeleteLoopMenuExit (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_ModeDeleteEvent (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_DeleteEventAllOKSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_DeleteEventLastOKSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_DeleteEventMenuExit (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_ScdvPbFileTypeSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_ModeStill (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_FisrstItemProcess (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_ImageSizeSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_WBModeSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_DriverModeSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_LastItemProcess (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_PhotoBurstSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_ModeVideo (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_VideoSizeSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_VideoQualitySet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_SeamlessSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_VideoWBModeSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_WDRModeSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_MDModeSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_VideoStampModeSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_ModePlayback (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_PbFileProtect (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_PbFileDelete (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_ModeSetup (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_SoundInit (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_SoundSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_SleepTimeSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_RTCInit (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_RTCSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_LanguageSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_TVModeSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_G_SensorSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_LightFreqSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_GPSSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_WaterMarkSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_LOGOMarkSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_StabilizerSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_InvertModeSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_CollisionDetectionSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_TimeZonesSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_SystemModeSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_Format (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_WIFIPasswordInit (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_WIFIPasswordSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_TimeLapseSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_SceneModeSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_FileTypeSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);

UINT32 appMenuAct_SdvVideoMode (UINT8 item);
UINT32 appMenuAct_SdvPhotoMode (UINT8 item);
UINT32 appMenuAct_ScdvSensorFlip (UINT8 item);
UINT32 appMenuAct_SdvCarDv (UINT8 item);
UINT32 appMenuAct_CdvVideoMode (UINT8 item);
UINT32 appMenuAct_CdvLooping (UINT8 item);
UINT32 appMenuAct_CdvGsensorTrigger (UINT8 item);
UINT32 appMenuAct_CdvGsensorSensitivity (UINT8 item);
UINT32 appMenuAct_CdvMicPhone (UINT8 item);
UINT32 appMenuAct_CdvSdvMode (UINT8 item);
UINT32 appMenuAct_SystemDis (UINT8 item);
UINT32 appMenuAct_SystemWdr (UINT8 item);
UINT32 appMenuAct_SystemLedIndicator (UINT8 item);
UINT32 appMenuAct_SystemOsdFlip (UINT8 item);
UINT32 appMenuAct_SystemOsdBrightness (UINT8 item);
UINT32 appMenuAct_SystemBeepVolume (UINT8 item);
UINT32 appMenuAct_SystemDateStamp (UINT8 item);
UINT32 appMenuAct_SystemAutoPowerOff (UINT8 item);
UINT32 appMenuAct_System1Exit (UINT8 item);
UINT32 appMenuAct_SystemDateTime (UINT8 item);
UINT32 appMenuAct_SystemRemoter (UINT8 item);
UINT32 appMenuAct_SystemFrequnecy (UINT8 item);
UINT32 appMenuAct_SystemFormat (UINT8 item);
UINT32 appMenuAct_SystemDefault (UINT8 item);
UINT32 appMenuAct_SystemWifiPassword (UINT8 item);
UINT32 appMenuAct_SystemVersion (UINT8 item);
UINT32 appMenuAct_SystemExit (UINT8 item);
UINT32 appMenuAct_ScdvPbFileType (UINT8 item);
UINT32 appMenuAct_ImageSize (UINT8 item);

#endif /* _MENU_API_H_ */

