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
#ifndef _MENU_TAB_H_
#define _MENU_TAB_H_

#include "app_menu_event.h"

#define	MENU_MARK_SPORT_DV	0x01000000
#define	MENU_MARK_CAR_DV	0x02000000
#define	MENU_MARK_SYSTEM1	0x03000000
#define	MENU_MARK_SYSTEM2	0x04000000
#define	MENU_MARK_RF_REMOTER	0x04020000
#define	MENU_MARK_SYSTEM2_WIFI_PW	0x04060000
#define	MENU_MARK_MENU_EXIT	0x05000000
#define	MENU_MARK_DELETE	0x06000000
#define	MENU_MARK_DELETE_EXIT	0x06030000
#define	MENU_MARK_DELETE_LOOP	0x07000000
#define	MENU_MARK_DELETE_LOOP_EXIT	0x07030000
#define	MENU_MARK_DELETE_EVENT	0x08000000
#define	MENU_MARK_DELETE_EVENT_EXIT	0x08030000
#define	MENU_MARK_SCDV_PB_SELECT	0x09000000
#define	MENU_MARK_SCDV_PB_VIDEO	0x09010101
#define	MENU_MARK_STILL	0x0a000000
#define	MENU_MARK_VIDEO	0x0b000000
#define	MENU_MARK_PLAYBACK	0x0c000000
#define	MENU_MARK_SETUP	0x0d000000
#define	MENU_MARK_SCENEMODE	0x0e000000
#define	MENU_MARK_SCENEMODEITEM	0x0e010000
#define	MENU_MARK_FORMAT_YES	0x0f010101
#define	MENU_MARK_FORMAT_NO	0x0f010102
#define	MENU_MARK_DELONE_YES	0x0f010201
#define	MENU_MARK_DELONE_NO	0x0f010202
#define	MENU_MARK_BLINK_SAVE	0x0f010301
#define	MENU_MARK_BLINK_CANCEL	0x0f010302
#define	MENU_MARK_JPG_CROP_YES	0x0f010401
#define	MENU_MARK_JPG_CROP_NO	0x0f010402
#define	MENU_MARK_FILETYPE_EVENT	0x0f010501
#define	MENU_MARK_REDEYE_WRITE	0x10010000
#define	MENU_MARK_REDEYE_SAVE	0x10020000
#define	MENU_MARK_REDEYE_CANCEL	0x10030000
#define	MENU_MARK_CALIBRATION	0x11000000
#define	MENU_MARK_WBCLBT	0x11060000
#define	MENU_MARK_FLASH	0x11080000
#define	MENU_MARK_CAMCONFIG	0x110c0000
#define	MENU_MARK_TESTBENCH	0x110d0000

#define ID_STR_ENTER		0x040b
#define ID_STR_BACK		0x040c
#define ID_STR_DELETE_THIS_FILE_		0x040d
#define ID_STR_DELETE_ALL_FILE_		0x040e
#define ID_STR_FORMATTING___		0x040f
#define ID_STR_HAS_NO_SIZE_TO_RISIZE_		0x0410
#define ID_STR_FILE_LOCKED_		0x0411
#define ID_STR_PROTECT_FILE_		0x0412
#define ID_STR_UNPROTECT_FILE_		0x0413
#define ID_STR_PROTECT_ALL_		0x0414
#define ID_STR_UNPROTECT_ALL_		0x0415
#define ID_STR_NO_FILE		0x0416
#define ID_STR_NO_CARD_		0x0417
#define ID_STR_REMOVE_CARD_		0x0418
#define ID_STR_CARD_ERROR		0x0419
#define ID_STR_WAITING___		0x041a
#define ID_STR_CARD_LOCKED_		0x041b
#define ID_STR_PAUSE		0x041c
#define ID_STR_REC		0x041d
#define ID_STR_FILE_ERROR_		0x041e
#define ID_STR_PLEASE_PLUG_OUT_CARD		0x041f
#define ID_STR_MEMORY_FULL		0x0420
#define ID_STR_MEMORY_NOT_ENOUGH		0x0421
#define ID_STR_WAITING_FOR_PRINTER		0x0422
#define ID_STR_NO_JPEG		0x0423
#define ID_STR_FORMAT_FAIL_		0x0424
#define ID_STR_LOW_BATTERY		0x0425
#define ID_STR_NOT_SUPPORT_		0x0426
#define ID_STR_CANCEL		0x0427
#define ID_STR_SELECT_UP_TO_24		0x0428
#define ID_STR_PLEASE_SELECT_A_PICTURE		0x0429
#define ID_STR_PRINT_ERROR		0x042a
#define ID_STR_NOT_FIND_PRINTER		0x042b
#define ID_STR_CROP_THIS_PHOTO_		0x042c
#define ID_STR_PICTBRIDGE		0x042d
#define ID_STR_YES		0x042e
#define ID_STR_NO		0x042f
#define ID_STR_SET_THIS_PHOTO_		0x0430
#define ID_STR_REVIEW_THIS_FILE_		0x0431
#define ID_STR_SAVE_THIS_FILE_		0x0432
#define ID_STR_DELETE		0x0433
#define ID_STR_ALL		0x0434
#define ID_STR_SELECT		0x0435
#define ID_STR_COPY_TO_CARD		0x0436
#define ID_STR_COPY_THIS_FILE_		0x0437
#define ID_STR_COPY_ALL_FILES_		0x0438
#define ID_STR_ROTATE		0x0439
#define ID_STR_ROTATE_THIS_PHOTO_		0x043a
#define ID_STR_SAVE		0x043b
#define ID_STR_SET		0x043c
#define ID_STR_STOP		0x043d
#define ID_STR_DELETE_VOICE_MEMO_		0x043e
#define ID_STR_READY_TO_RECORD		0x043f
#define ID_STR_READY_TO_PLAY		0x0440
#define ID_STR_RECORDING_MEMO___		0x0441
#define ID_STR_PLAYING_AUDIO___		0x0442
#define ID_STR_TRIM_THIS_PHOTO_		0x0443
#define ID_STR_COPY_ERROR		0x0444
#define ID_STR_CAPTURE_NOT_ENOUGH_SIZE		0x0445
#define ID_STR_CAPTURE_DONE		0x0446
#define ID_STR_BUSY___PLEASE_WAIT		0x0447
#define ID_STR_NOT_AVAILABLE_FOR_THIS_FILE		0x0448
#define ID_STR_IMAGE_SAVED		0x0449
#define ID_STR_CHOOSE_THIS_PHOTO_		0x044a
#define ID_STR_PRINT_ALL_PHOTO_		0x044b
#define ID_STR_SET_PRINT_COPIES		0x044c
#define ID_STR_PAPER_ERROR		0x044d
#define ID_STR_LOAD_ERROR		0x044e
#define ID_STR_EJECT_ERROR		0x044f
#define ID_STR_PAPER_JAM		0x0450
#define ID_STR_INK_EMPTY		0x0451
#define ID_STR_INK_LOW		0x0452
#define ID_STR_WASTE_INK		0x0453
#define ID_STR_FATAL_ERROR		0x0454
#define ID_STR_SERVICE_CALL		0x0455
#define ID_STR_PRINTER_BUSY		0x0456
#define ID_STR_LEVER_ERROR		0x0457
#define ID_STR_COVER_OPEN		0x0458
#define ID_STR_INK_COVER_OPEN		0x0459
#define ID_STR_NO_INK_CARTRIDGE		0x045a
#define ID_STR_INFO_ERROR		0x045b
#define ID_STR_USB_ERROR		0x045c
#define ID_STR_CONNECTING		0x045d
#define ID_STR_PRINTING___		0x045e
#define ID_STR_CANCELLING		0x045f
#define ID_STR_NO_PAPER		0x0460
#define ID_STR_PLAY		0x0461
#define ID_STR_DISPLAY_FILE		0x0462
#define ID_STR_COMPLETED		0x0463
#define ID_STR_NOT_SUPPORT_C2C		0x0464
#define ID_STR_FIND_START		0x0465
#define ID_STR_FIND_END		0x0466
#define ID_STR_AND_PRESS_OK		0x0467
#define ID_STR_WRITING_VIDEO		0x0468
#define ID_STR_WRITING_FILE_FAIL		0x0469
#define ID_STR_TRIM_VIDEO_IS_TOO_SHORT		0x046a
#define ID_STR_FACTORY_RESET		0x046b
#define ID_STR_MENU		0x046c
#define ID_STR_VIDEO		0x046d
#define ID_STR_SLOW		0x046e
#define ID_STR_PHOTO		0x046f
#define ID_STR_LAPSE		0x0470
#define ID_STR_BURST		0x0471
#define ID_STR_TIMER		0x0472
#define ID_STR_WIFI		0x0473
#define ID_STR_INIT___		0x0474
#define ID_STR_CONN___		0x0475
#define ID_STR_READY		0x0476
#define ID_STR_DEL		0x0477
#define ID_STR_SETUP		0x0478
#define ID_STR_HDMI		0x0479
#define ID_STR_HDMI_PB		0x047a
#define ID_STR_USB		0x047b
#define ID_STR_MSDC		0x047c
#define ID_STR_BURN		0x047d
#define ID_STR_CARD_ISP		0x047e
#define ID_STR_RF_SCANNING		0x047f
#define ID_STR_DISPLAY		0x0480
#define ID_STR_PLAY_PAUSE		0x0481

extern UINT16 CustomIconPtr[];
extern UINT32 CustomMenuRootStart[];
extern UINT32 CustomMenuIndex[];
extern UINT8 CustomMenuNodeItem[];
extern UINT8 CustomMenuSubItem[];
extern menufunc CustomMenuFunc[];
extern menuexe CustomMenuExe[];
extern UINT16 CustomMenuParaTab[];

#endif /* _MENU_TAB_H_ */
