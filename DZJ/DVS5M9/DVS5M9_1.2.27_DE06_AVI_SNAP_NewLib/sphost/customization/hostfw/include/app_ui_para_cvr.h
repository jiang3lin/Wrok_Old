/**************************************************************************
 *         Copyright(c) 2012 by iCatch Technology Co., Ltd.              *
 *                                                                        *
 *  This software is copyrighted by and is the property of iCatch Tech-  *
 *  nology Co., Ltd. All rights are reserved by iCatch Technology Co.,   *
 *  Ltd. This software may only be used in accordance with the            *
 *  corresponding license agreement. Any unauthorized use, duplication,   *
 *  distribution, or disclosure of this software is expressly forbidden.  *
 *                                                                        *
 *  This Copyright notice "M U S T" not be removed or modified without    *
 *  prior written consent of iCatch Technology Co., Ltd.                 *
 *                                                                        *
 *  iCatch Technology Co., Ltd. reserves the right to modify this        *
 *  software without notice.                                              *
 *                                                                        *
 *  iCatch Technology Co., Ltd.                                          *
 *  19-1, Innovation First Road, Science-Based Industrial Park,           *
 *  Hsin-Chu, Taiwan, R.O.C.                                              *
 *                                                                        *
 *************************************************************************/

/**
 * @file		app_ui_para.h
 * @brief		App for UI setting parameter definition
 * @author	Phil Lin
 * @since		2008-02-20
 * @date		2008-02-20
 */
#ifndef __APP_UI_PARA_CVR_H__
#define __APP_UI_PARA_CVR_H__
/*
 //=============================================================================
//Header file
//=============================================================================

//=============================================================================
//Symbol
//=============================================================================
//-----------------------------------------------------------------------------
//Constant
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//Type definition
//-----------------------------------------------------------------------------
*/
typedef struct uiPara_s {	/* Can't be larger than 512Bytes */
/* Note: Don't add any struct member at here !!! */
/* ------ Note: Auto create code zone start flag, don't modify or delete this line!!! ------ */
	UINT8 EmptyPara;		/* Auto create code zone, don't modify or delete! */
	UINT8 ImageSize;		/* Auto create code zone, don't modify or delete! */
	UINT8 WBMode;		/* Auto create code zone, don't modify or delete! */
	UINT8 DriverMode;		/* Auto create code zone, don't modify or delete! */
	UINT8 PhotoBurst;		/* Auto create code zone, don't modify or delete! */
	UINT8 VideoSize;		/* Auto create code zone, don't modify or delete! */
	UINT8 VideoQuality;		/* Auto create code zone, don't modify or delete! */
	UINT8 Seamless;		/* Auto create code zone, don't modify or delete! */
	UINT8 WDRMode;		/* Auto create code zone, don't modify or delete! */
	UINT8 MotionDetect;		/* Auto create code zone, don't modify or delete! */
	UINT8 VideoStampMode;		/* Auto create code zone, don't modify or delete! */
	UINT8 selImgProtect;		/* Auto create code zone, don't modify or delete! */
	UINT8 selImgDelete;		/* Auto create code zone, don't modify or delete! */
	UINT8 BeepSound;		/* Auto create code zone, don't modify or delete! */
	UINT8 Volume;		/* Auto create code zone, don't modify or delete! */
	UINT8 VoiceRec;		/* Auto create code zone, don't modify or delete! */
	UINT8 SleepTime;		/* Auto create code zone, don't modify or delete! */
	UINT8 rtcYear;		/* Auto create code zone, don't modify or delete! */
	UINT8 rtcMonth;		/* Auto create code zone, don't modify or delete! */
	UINT8 rtcDay;		/* Auto create code zone, don't modify or delete! */
	UINT8 rtcHour;		/* Auto create code zone, don't modify or delete! */
	UINT8 rtcMinute;		/* Auto create code zone, don't modify or delete! */
	UINT8 DateStyle;		/* Auto create code zone, don't modify or delete! */
	UINT8 Language;		/* Auto create code zone, don't modify or delete! */
	UINT8 TVMode;		/* Auto create code zone, don't modify or delete! */
	UINT8 G_Sensor;		/* Auto create code zone, don't modify or delete! */
	UINT8 LightFreq;		/* Auto create code zone, don't modify or delete! */
	UINT8 GPS;		/* Auto create code zone, don't modify or delete! */
	UINT8 WaterMark;		/* Auto create code zone, don't modify or delete! */
	UINT8 LOGOMark;		/* Auto create code zone, don't modify or delete! */
	UINT8 Stabilizer;		/* Auto create code zone, don't modify or delete! */
	UINT8 InvertMode;		/* Auto create code zone, don't modify or delete! */
	UINT8 CollisionDetection;		/* Auto create code zone, don't modify or delete! */
	UINT8 TimeZones;		/* Auto create code zone, don't modify or delete! */
	UINT8 SystemMode;		/* Auto create code zone, don't modify or delete! */
	UINT8 WIFIPassword_7;		/* Auto create code zone, don't modify or delete! */
	UINT8 WIFIPassword_6;		/* Auto create code zone, don't modify or delete! */
	UINT8 WIFIPassword_5;		/* Auto create code zone, don't modify or delete! */
	UINT8 WIFIPassword_4;		/* Auto create code zone, don't modify or delete! */
	UINT8 WIFIPassword_3;		/* Auto create code zone, don't modify or delete! */
	UINT8 WIFIPassword_2;		/* Auto create code zone, don't modify or delete! */
	UINT8 WIFIPassword_1;		/* Auto create code zone, don't modify or delete! */
	UINT8 WIFIPassword_0;		/* Auto create code zone, don't modify or delete! */
	UINT8 TimeLapse;		/* Auto create code zone, don't modify or delete! */
	UINT8 SceneMode;		/* Auto create code zone, don't modify or delete! */
	UINT8 fileType;		/* Auto create code zone, don't modify or delete! */
/* ------ Note: Auto create code zone end flag, don't modify or delete this line!!! ------ */
/* Note: You can add struct member after this line !!! */
	UINT8 AEMode;
	UINT8 ImageEdge;
	UINT8 flash;
	UINT8 resCreate;
	UINT8 BlinkMode;	
	UINT8 redEyePb;
	UINT8 tpCorrect;
	UINT8 shutter;
	UINT8 soundcontrol;
	UINT8 af;
	UINT8 batteryType;
	UINT8 fileNumber;
	UINT16 dcfDirKey;
	UINT16 dcfFileKey;
	UINT8 OpenPic;
	UINT8 PeriodicalCap;
	UINT8 ImageQuality;
	UINT8 AEMetering;
	UINT8 Sharpness;
	UINT8 ImageEffect;
	UINT8 FeatureEffect;
	UINT8 StampMode;
	UINT8 FaceTracking;
	UINT8 Digit;
	UINT8 RollingShutter;
	UINT8 VideoFaceTracking;
	UINT8 DiaMode;
	UINT8 AdvanceDenoise;
	UINT8 AutoReviewTime;
	UINT8 USBMode;
	UINT8 VideoFormat;
	UINT8 Iso;
	UINT8 AEB;
	UINT8 PhotoFrame;
	UINT8 DigitalZoom;
	UINT8 DisMode;
	UINT8 MediaFormat;
	UINT8 selSlideMusic;
	UINT8 selSlideTrans;
	UINT8 selSlideRepeat;
	UINT8 selCopy2Card;
	UINT8 selImgEffect;
	UINT8 selImgRotate;
	UINT8 selImgResize;
	UINT8 selVoiceMemo;
	UINT8 selStartImage;
	UINT8 selPhotoframe;
	UINT8 selSlideInteval;
	UINT8 StartupSound;
	UINT8 ShutterSound;
	UINT8 CurViewMode; /* PASM ,Scene ,Video, Audio */
	UINT8 CurScene; /* Current scene */
	UINT8 ParkingMode;
#if ICAT_FCWS || ICAT_LDWS
	UINT8 DAS_mode;
	UINT8 cvrwsRightCnt;
	UINT8 cvrwsLeftCnt;
#endif
/* Note: Next is checksum , don't add anything below */
	UINT8 iCatchFlag[6];
	UINT32 Checksum;
} uiPara_t;


typedef enum uiParaInitMode_e {
	PARA_INIT_WITH_DEF = 0x00,
	PARA_INIT_FROM_UDF = 0x01,
} uiParaInitMode_t;

typedef enum uiItemOrder_e {
	ITEM_EMPTY = 0x80,
	ITEM_FIRST = 0x81,
	ITEM_MAX   = 0xfe,
	ITEM_NULL  = 0xff
} uiItemOrder_t;
/*
//-----------------------------------------------------------------------------
//Variable
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//Function declare
//-----------------------------------------------------------------------------*/
void appUiParaInit(uiParaInitMode_t mode);
uiPara_t* appUiParaGet(void);
UINT32 appUIParaSave(void);
UINT8 appUiItemOrderGet(UINT8 idx,UINT8 *pTable,UINT8 *length);
void appUiItemOrderCfg(void);
UINT8 appUiItemStsSet(UINT8 idx,UINT8 val);
UINT8  appUiItemStsGet(UINT8 idx);

#endif /*__APP_UI_PARA_CVR_H__*/
