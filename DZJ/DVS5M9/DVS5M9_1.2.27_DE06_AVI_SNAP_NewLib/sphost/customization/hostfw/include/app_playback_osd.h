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

#ifndef APP_PLAYBACK_OSD_H
#define APP_PLAYBACK_OSD_H

#include "app_playback_def.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

#define OSD_FONT_CONST_WIDTH	18
#define OSD_FONT_CONST_HEIGHT	24
#define LCD_SIZE_X							320                                                                                                                                                       
#define LCD_SIZE_Y							240                                                                                                                                                       
#define APPPB_ICON_SIZE_X					32//36
#define APPPB_ICON_SIZE_Y					28//32
#define PB_PLAY_MEDIA_STATUS_ICON_SIZE_X	32//48
#define PB_PLAY_MEDIA_STATUS_ICON_SIZE_Y	28//48


                                                                                                                                                                                          
#define APPPB_ICON_X_GAP						0                                                                                                                                                     
#define APPPB_ICON_Y_GAP						0                                                                                                                                                     
#define PLAYBACK_ICON_X							6
#define PLAYBACK_ICON_Y							4
#define PROTTECT_ICON_X							6                                                                                                                                                     
#define PROTTECT_ICON_Y							PLAYBACK_ICON_Y + APPPB_ICON_SIZE_Y + APPPB_ICON_Y_GAP + 7                                                                                                                            
#define DPOF_ICON_X								PROTTECT_ICON_X	                                                                                                                                        
#define DPOF_ICON_Y								PROTTECT_ICON_Y + APPPB_ICON_SIZE_Y + APPPB_ICON_Y_GAP                                                                                                  
#define VOICE_MEMO_ICON_X						PROTTECT_ICON_X                                                                                                                                       
#define VOICE_MEMO_ICON_Y						DPOF_ICON_Y + APPPB_ICON_SIZE_Y + APPPB_ICON_Y_GAP                                                                                                    

#define EXPLOSURE_ICON_X						VOICE_MEMO_ICON_X
#define EXPLOSURE_ICON_Y						VOICE_MEMO_ICON_Y+APPPB_ICON_SIZE_Y + APPPB_ICON_Y_GAP 
#define ISO_ICON_X								EXPLOSURE_ICON_X
#define ISO_ICON_Y								EXPLOSURE_ICON_Y+APPPB_ICON_SIZE_Y + APPPB_ICON_Y_GAP 


#define CAPTURED_MODE_ICON_X					PROTTECT_ICON_X                                                                                                                                     
#define CAPTURED_MODE_ICON_Y					LCD_SIZE_Y - APPPB_ICON_SIZE_Y -APPPB_ICON_Y_GAP                                                                                                                      
#define RESOLUTION_ICON_X						PROTTECT_ICON_X + APPPB_ICON_SIZE_X + APPPB_ICON_X_GAP + 5                                                                                            
#define RESOLUTION_ICON_Y						CAPTURED_MODE_ICON_Y                                                                                                                                   
#define WHITE_BALANCE_ICON_X					RESOLUTION_ICON_X + APPPB_ICON_SIZE_X + APPPB_ICON_X_GAP                                                                                            
#define WHITE_BALANCE_ICON_Y					CAPTURED_MODE_ICON_Y                                                                                                                                 
#define STROBE_MODE_ICON_X					WHITE_BALANCE_ICON_X + 3*APPPB_ICON_SIZE_X + APPPB_ICON_X_GAP                                                                                            
#define STROBE_MODE_ICON_Y					CAPTURED_MODE_ICON_Y
#define METERING_MODE_ICON_X					STROBE_MODE_ICON_X + APPPB_ICON_SIZE_X + APPPB_ICON_X_GAP                                                                                            
#define METERING_MODE_ICON_Y					CAPTURED_MODE_ICON_Y

#define INDICATE_ARROW_UP_ICON_X				WHITE_BALANCE_ICON_X+APPPB_ICON_SIZE_X + APPPB_ICON_X_GAP
#define INDICATE_ARROW_UP_ICON_Y				CAPTURED_MODE_ICON_Y-4
#define INDICATE_PLAY_ICON_X					INDICATE_ARROW_UP_ICON_X+APPPB_ICON_SIZE_X + APPPB_ICON_X_GAP
#define INDICATE_PLAY_ICON_Y					INDICATE_ARROW_UP_ICON_Y
#define INDICATE_PAUSE_ICON_X					INDICATE_PLAY_ICON_X
#define INDICATE_PAUSE_ICON_Y					INDICATE_PLAY_ICON_Y

#define MENU_ICON_X								CAPTURED_MODE_ICON_X + 2
#define MENU_ICON_Y								LCD_SIZE_Y - APPPB_ICON_Y_GAP - 6
#define ALPHABET_FONT_SIZE_X					12
#define ALPHABET_FONT_SIZE_Y					30
#define PB_DATE_X								44	
#define PB_DATE_Y								PLAYBACK_ICON_Y
#define PB_DATE_SIZE_X							(ALPHABET_FONT_SIZE_X * 10)	
#define PB_TIME_X								154		
#define PB_TIME_Y								PLAYBACK_ICON_Y	
#define PB_TIME_SIZE_X							(ALPHABET_FONT_SIZE_X * 5)	
#define PB_FILE_NAME_X							LCD_SIZE_X  - 14 * ALPHABET_FONT_SIZE_X
#define PB_FILE_NAME_Y							120
#define PB_FILE_NAME_SIZE_X						(ALPHABET_FONT_SIZE_X * 14)
#define PB_FILE_SIZE_X							PB_FILE_NAME_X
#define PB_FILE_SIZE_Y							120 + ALPHABET_FONT_SIZE_Y
#define PB_FILE_SIZE_SIZE_X						PB_FILE_SIZE_X
#define PB_F_NUM_X								PB_FILE_NAME_X
#define PB_F_NUM_Y								120 + (ALPHABET_FONT_SIZE_Y * 2)
#define PB_EXP_F_NUM_SIZE_X						PB_FILE_NAME_SIZE_X

#define PB_HISTOGRAM_X							PB_FILE_NAME_X
#define PB_HISTOGRAM_Y							PB_FILE_NAME_Y - PB_HISTOGRAM_SIZE_Y - 2
#define PB_HISTOGRAM_SIZE_X						128
#define PB_HISTOGRAM_SIZE_Y						60

#define PB_PLAY_MEDIA_STATUS_ICON_X  			144
#define PB_PLAY_MEDIA_STATUS_ICON_Y  			170
#define PB_VOICE_MEMO_PROG_FRAME_SIZE_X			240

#define PB_VOICE_MEMO_PROG_FRAME_SIZE_Y			150


#define PB_VOICE_MEMO_PROG_FRAME_CENT_X				LCD_SIZE_X / 2 + 3
#define PB_VOICE_MEMO_PROG_FRAME_CENT_Y				LCD_SIZE_Y / 2
#define PB_VOICE_MEMO_PROG_FRAME_LEFT_X			PB_VOICE_MEMO_PROG_FRAME_CENT_X  - (PB_VOICE_MEMO_PROG_FRAME_SIZE_X / 2)
#define PB_VOICE_MEMO_PROG_FRAME_UPPER_Y		PB_VOICE_MEMO_PROG_FRAME_CENT_Y - (PB_VOICE_MEMO_PROG_FRAME_SIZE_Y / 2)

#define PB_VOICE_MEMO_BAR_X						PB_VOICE_MEMO_PROG_FRAME_LEFT_X + 37
#define PB_VOICE_MEMO_BAR_Y						PB_VOICE_MEMO_PROG_FRAME_UPPER_Y + 107


#define PB_VOICE_MEMO_BAR_SIZE_X				165
#define PB_VOICE_MEMO_BAR_SIZE_Y				19

#define PB_VOICE_MEMO_TIME_X 					LCD_SIZE_X / 2 - (4 * ALPHABET_FONT_SIZE_X)
#define PB_VOICE_MEMO_TIME_Y					LCD_SIZE_Y / 2 - (ALPHABET_FONT_SIZE_Y / 2) 

#define PB_VIDEO_SEAMPLAY_IDX_X 				2 * ALPHABET_FONT_SIZE_X
#define PB_VIDEO_SEAMPLAY_IDX_Y 				6

#define PB_VIDEO_TIME_X 						LCD_SIZE_X- (8 * ALPHABET_FONT_SIZE_X)
#define PB_VIDEO_TIME_Y 						6

#define PB_AUDIO_STRING_X				LCD_SIZE_X/2
#define PB_AUDIO_STRING_Y				70
#define PB_AUDIO_STRING_W				200


#define MPB_ICON_MARGINE_X			 3
#define MPB_ICON_MARGINE_Y			 3	
#define MPB_PROTECT_ICON_X			 39- MPB_ICON_MARGINE_X /*36*/
#define MPB_PROTECT_ICON_Y			 45 - MPB_ICON_MARGINE_Y /*42*/
#define MPB_DELETE_ICON_X 			 0 + MPB_ICON_MARGINE_X /*3*/
#define MPB_DELETE_ICON_Y 			 10 + MPB_ICON_MARGINE_Y /*13*/
#define MPB_VOICE_MEMO_ICON_X		 MPB_ICON_MARGINE_X/*3*/
#define MPB_VOICE_MEMO_ICON_Y		 MPB_PROTECT_ICON_Y/*42*/

#define FILE_LEFTRIGHT_LEFT_ARROW_X	4
#define FILE_LEFTRIGHT_LEFT_ARROW_Y	216
#define FILE_LEFTRIGHT_RIGHT_ARROW_X	287
#define FILE_LEFTRIGHT_RIGHT_CLEAR_X	306
#define FILE_LEFTRIGHT_ARROW_CLEAR_W	14
#define FILE_LEFTRIGHT_ARROW_CLEAR_H	20


#define MAX_ZOOM_PAN_W_H 67
#define CENT_DZOOM_PAN_X LCD_SIZE_X - 45
#define CENT_DZOOM_PAN_Y LCD_SIZE_Y - 45 


#define DLGBOX_OPTION_NUM_MAX	4
#define DLGBOX_COLOR_TITLE_H	42
#define DLGBOX_NULL_TITLE_H		42
#define DLGBOX_DATA_CONT_H		39
#define DLGBOX_DATA_LAST_H		39
#define DLGBOX_WIDTH			300
#define DLGBOX_WIDTH_HDMI		540
#define DLGBOX_HL_BAR_W			296
#define DLGBOX_HL_BAR_W_HDMI		514
#define DLGBOX_HL_BAR_H			37
#define DLGBOX_HL_BAR_H_HDMI		45


/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

typedef struct {
	/*-- Dialog Box config */
	UINT32 titleStrId;
	UINT32 numOptions;
	UINT32 optionStrID[DLGBOX_OPTION_NUM_MAX];
	UINT32 lastActIndex;
	UINT32 actIndex;

	/*-- Control data */
	UINT32 boxX;
	UINT32 boxY;
	UINT32 boxW;
	UINT32 boxH;
	SINT32 boxY_Offset;

	UINT32 markerIconW;
	UINT32 optionH;

	UINT32 titleX;
	UINT32 titleY;

	UINT32 optX[DLGBOX_OPTION_NUM_MAX];
	UINT32 optY[DLGBOX_OPTION_NUM_MAX];
	UINT32 dlgStatus;

} pbDlgBoxInfo_t;



 enum MediaPlayStaus_S
 {
	 PAUSING	 =	 (unsigned char) 0x00,
	 PLAYING	 =	 (unsigned char) 0x01,
	 RWDING 	 =	 (unsigned char) 0x02,
	 FWDING 	 =	 (unsigned char) 0x03,
	 SLOW    =	 (unsigned char) 0x04,
 };
enum MultiPlaybackInfo_S 
{
	MPLAYBACK_PROTECTED,
	MPLAYBACK_UNPROTECTED,	
	MPLAYBACK_NEWPROTECTED,	
	MPLAYBACK_DELETE,			
	MPLAYBACK_VOICE_MEMO,  			
	MPLAYBACK_VIDEO  
};
enum ConfirmDlgType_S
{
	 DLGTYPE_YES_NO	 	=	 (unsigned char) 0x00,
	 DLGTYPE_ROTATE	 	=	 (unsigned char) 0x01,
	 DLGTYPE_MPB_LOCK 	=	 (unsigned char) 0x02,
	 DLGTYPE_MPB_DELE 	=	 (unsigned char) 0x03,
};
enum ConfirmDlgStatus_S
{
	 DLGSTATUS_IDLE	=	 (unsigned char) 0x00,
	 DLGSTATUS_SHOW	=	 (unsigned char) 0x01,
	 DLGSTATUS_SET		=	 (unsigned char) 0x02,
};

typedef enum 
{
	PB_OSD_DRAW_NORMAL,
	PB_OSD_DRAW_FAST
} pbOsdDrawControl_t;


void appPlayOsd_IconDraw(UINT8 bShow, SINT32 X, SINT32 Y, UINT32 IconID);
void appPlayOsd_DigitalZoomWinDraw(	SINT32 CropImageCentX,	SINT32 CropImageCentY,	UINT32 ImageWidth,	UINT32 ImageHeight,	UINT32 ZoomFactor,	UINT8 ImageAspRatio);
void appPbOsd_AuxNote_OKPause(BOOL bShow);
void appPbOsd_AuxNote_OKPlay(BOOL bShow);
void appPbOsd_AuxNote_OKStop(BOOL bShow);
void appPbOsd_AuxNote_OKRec(BOOL bShow);
void appPbOsd_AuxNote_OKSet(BOOL bShow);
void appPbOsd_AuxNote_OKSelect(BOOL bShow);
void appPbOsd_AuxNote_MenuCancel(BOOL bShow);
void appPbOsd_AuxNote_MenuBack(BOOL bShow);
void appPbOsd_AuxNote_OKDispPhoto(BOOL bShow);
void appPbOsd_AuxNote_TrashCanDelet(BOOL bShow,	BOOL bActive);
void appPbOsd_AuxNote_LockProtect(BOOL bShow,	BOOL bActive);
pbDlgBoxInfo_t *appPlayOsd_DlgBoxCBGet(void);
UINT32 appPlayOsd_DlgBoxStatusGet(void);
void appPlayOsd_DlgBoxSelectSet(UINT32 selIndex);
void appPlayOsd_DlgBoxStatusSet(UINT32 status);
UINT8 appPlayOsd_DlgBoxSelectGet(void);
void appPlayOsd_DlgBoxClear(void);
void appPlayOsd_DlgBoxTitleSet(UINT32 titleStrId);
void appPlayOsd_DlgBoxTitleUpdate(UINT32 titleStrId);
void appPlayOsd_DlgBoxActiveBarChange(void);
void appPlayOsd_DlgBoxScroll(BOOL isUp);
void appPlayOsd_DlgBoxConfig(UINT32 titleStrId, UINT32 numOptions, UINT32 *poptionStrID, UINT32 actIndex, SINT32 yOffset);
void appPlayOsd_DlgBoxShow(void);
void appPlayOsd_DlgMessageShow(UINT32 titleStrId,UINT32 dlgType);
UINT8 appPlayOsd_DlgMessageProcess(UINT32 msg);
/*s added by Leo @20091126*/
void appPlayOsd_DlgBoxSetandShow(UINT32 DlgStartX,UINT32 DlgStartY,	UINT32 DlgFirstItemX,	UINT32 DlgFirstItemY,UINT32 DlgItemGap);
void appPlayOsd_DlgBoxScrollSet(BOOL isUp,UINT32 DlgStartX,UINT32 DlgStartY,UINT32 DlgFirstItemX,	UINT32 DlgFirstItemY,UINT32 DlgItemGap);
void appPlayOsd_DlgBoxActiveBarChangeSet(UINT32 DlgStartX,UINT32 DlgStartY,UINT32 DlgFirstItemX,UINT32 DlgFirstItemY,UINT32 DlgItemGap);
/*e added by Leo @20091126*/
void appPlayOsd_DlgConfirmYesNoConfig(UINT32 strId, UINT32 actIdx);
void appPlayOsd_ModeIconDraw(BOOL bShow);
void appPlayOsd_FileNumDraw(UINT8 bShow, UINT32 currIdx, UINT32 total);
void appPlayOsd_MainInfoDraw(UINT32 osdDrawType);
void appPlayOsd_AudioBGDraw(void);
void playOsd_TextCenterPrint(UINT32 x, UINT32 y, UINT8 *pszText);
void appPlayOsd_PicExifInfoDraw(BOOL bShow, PpbImgExigInfo_t pCurImgExif,pbImgCb_t *pimgCb);
void appPlayOsd_OSDInfoDraw(pbMainCB_t *psPBImage, pbImgExifInfo_t *psPBCurImgExif,	pbImgCb_t *pimgCb);
void appPlayOsd_ProtectDraw(UINT8 bShow);
void appPLayOsd_AudioPlayDraw(BOOL bShow);
void appPLayOsd_AudioPauseResumeDraw(BOOL isPause);
void appPlayOsd_ProgressBarLastTimeReset(void);
void appPlayOsd_ProgressBarDraw(	UINT32 CurrentTime_msec, UINT32 TotalTime_msec);
void appPlayOsd_seamlessPlayIndexShow(BOOL bShow);
void appPlayOsd_VideoTimeDraw(UINT8 bShow,UINT32 CurrentTime_msec);
void appPlayOsd_VideoCntrlDraw(	UINT8 bShow,UINT8 isPlay,UINT8 bShowRWD,UINT8 bShowFWD);
void appPlayOsd_MediaPlayStatusDraw(UINT8 bShow,UINT16 MediaPlayStatus);
void appPlayOsd_ShowTimeInCenter(UINT8 bShow,UINT32 CurrentTime_msec);
void appPlayOsd_ThumbnailIconDraw(UINT8 bShow,UINT32 type,UINT32 X,UINT32 Y);
void appPLayOsd_LeftRightArrow(BOOL bShow);
void appPLayOsd_MemoRecOsdDraw(BOOL bShow);
void appPbShowBG(void);
void appPlayOsd_VideoTrimCntrlDraw(UINT8 bShow,UINT8 item);
void appPlayOsd_VideoInfoDraw(void);
void appPlayOsd_VideoFileNameDraw(void);
void appDcfFileNameGet(UINT32 dcfIdx);
void appPlayOsd_VidFileNameChangeDrow(void);
void appPlayOsd_ImagePlay(void);
void appPlayOsd_VideoFileIconDrow(unsigned char bShow);


#endif /* APP_PLAYBACK_OSD_H */
