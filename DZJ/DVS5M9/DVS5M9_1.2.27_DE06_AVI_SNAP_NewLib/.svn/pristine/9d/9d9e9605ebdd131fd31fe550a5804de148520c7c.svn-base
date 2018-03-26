/**************************************************************************
 *                                                                        *
 *         Copyright (c) 2005 by Sunplus Technology Co., Ltd.             *
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
 *  Author:Stephen Hung                                                   *
 *                                                                        *
 **************************************************************************/
#ifndef _SP5K_CAPTURE_API_H_
#define _SP5K_CAPTURE_API_H_

#include "api/sp5k_global_api.h"
#include "middleware/common_types.h"
#include "middleware/dist_def.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
/* Error code */
#define SP5K_ERR_SUCCESS                        0x00000000
#define SP5K_ERR_CAPTURE_CFG                    0x00000001

/* Host message */
/* Moved to sp5k_msg_def.h */

/* -- Still capture configuration (sp5kStillCapCfgSet) -- */
/* Selector */
#define SP5K_CAPTURE_MODE                       0x01
#define SP5K_CAPTURE_MAIN_IMAGESIZE             0x02
#define SP5K_CAPTURE_MAIN_COMPRESSION           0x03
#define SP5K_CAPTURE_QV_LENGTH                  0x04
#define SP5K_CAPTURE_FRAME                      0x05
#define SP5K_CAPTURE_STAMP                      0x06
#define SP5K_CAPTURE_SHUTTER_SND_NAME           0x07
#define SP5K_CAPTURE_BURST_LENGTH               0x08
#define SP5K_CAPTURE_BURST_QV_LAYOUT            0x09
/* replace SP5K_CAPTURE_MAIN_COMPRESSION */
#define SP5K_CAPTURE_MAIN_COMPRESSION_RATIO     0x0a
#define SP5K_CAPTURE_QV_NAIL_TYPE               0x0b
#define SP5K_CAPTURE_QV_RESOLUTION              0x0c
/* 100x version of SP5K_CAPTURE_MAIN_COMPRESSION_RATIO */
#define SP5K_CAPTURE_MAIN_COMPRESSION_PRECISE_RATIO 0x0d
#define SP5K_CAPTURE_WB_SIZE					0x0e



/* Definition Of Selector; SP5K_CAPTURE_MODE */
#define SP5K_CAPTURE_SINGLE_VLC                 0x01
#define SP5K_CAPTURE_SINGLE_RGB                 0x02
#define SP5K_CAPTURE_SINGLE_HDR                 0x03
#define SP5K_CAPTURE_BURST_VLC                  0x04
#define SP5K_CAPTURE_BURST_RGB                  0x05

/* Definition Of Selector: SP5K_CAPTURE_MAIN_COMPRESSION */
#define SP5K_CAPTURE_MAIN_COMPRESSION_2         0x01
#define SP5K_CAPTURE_MAIN_COMPRESSION_4         0x02
#define SP5K_CAPTURE_MAIN_COMPRESSION_6         0x03
#define SP5K_CAPTURE_MAIN_COMPRESSION_8         0x04
#define SP5K_CAPTURE_MAIN_COMPRESSION_10        0x05
#define SP5K_CAPTURE_MAIN_COMPRESSION_12        0x06
#define SP5K_CAPTURE_MAIN_COMPRESSION_14        0x07
#define SP5K_CAPTURE_MAIN_COMPRESSION_16        0x08
#define SP5K_CAPTURE_MAIN_COMPRESSION_18        0x09
#define SP5K_CAPTURE_MAIN_COMPRESSION_20        0x0a
#define SP5K_CAPTURE_MAIN_COMPRESSION_3         0x0b
#define SP5K_CAPTURE_MAIN_COMPRESSION_5         0x0c
#define SP5K_CAPTURE_MAIN_COMPRESSION_7         0x0d
#define SP5K_CAPTURE_MAIN_COMPRESSION_9         0x0e
#define SP5K_CAPTURE_MAIN_COMPRESSION_11        0x0f
#define SP5K_CAPTURE_MAIN_COMPRESSION_13        0x10
#define SP5K_CAPTURE_MAIN_COMPRESSION_15        0x11
#define SP5K_CAPTURE_MAIN_COMPRESSION_17        0x12
#define SP5K_CAPTURE_MAIN_COMPRESSION_19        0x13

/* Definition Of Structure; sp5kCaptureStamp _t */
#define SP5K_CAPTURE_DATE_YYYYMMDD              0x01   /*SNAP_DATE_YYYYMMDD*/
#define SP5K_CAPTURE_DATE_MMDDYYYY              0x02   /*SNAP_DATE_MMDDYYYY*/
#define SP5K_CAPTURE_DATE_DDMMYYYY              0x03   /*SNAP_DATE_DDMMYYYY*/
#define SP5K_CAPTURE_DATE_CUSTOM                0x09   /*SNAP_DATE_CUSTOM*/
#define SP5K_CAPTURE_DATE_TIME                  0x0100 /*SNAP_DATE_TIME*/
#define SP5K_CAPTURE_DATE_TIME_NO_SECOND        0x0200 /*SNAP_DATE_TIME_NO_SECOND*/
#define SP5K_CAPTURE_DATE_TIME_12HR             0x8100 /*SNAP_DATE_TIME_12HR*/
#define SP5K_CAPTURE_DATE_TIME_NO_SECOND_12HR   0x8200 /*SNAP_DATE_TIME_NO_SECOND_12HR*/

/* Definition of SP5K_CAPTURE_QV_NAIL_TYPE */
#define SP5K_CAPTURE_QV_NAIL_FMT_VLC             0x0
#define SP5K_CAPTURE_QV_NAIL_FMT_JPG             0x1

/* -- Still capture configuration (sp5kStillCapAdvCfgSet) -- */
/* Selector */
#define SP5K_CAPTURE_ORIENT                     0x51
#define SP5K_CAPTURE_SAVE_PROGRESSIVE_SIZE      0x52
#define SP5K_CAPTURE_QVGA_IMG                   0x53
#define SP5K_CAPTURE_HDR_REDO                   0x54
#define SP5K_CAPTURE_DARK_FRAME                 0x55
#define SP5K_CAPTURE_CONFIRM_STORE              0x56
#define SP5K_CAPTURE_BRC                        0x57
#define SP5K_CAPTURE_VIEW_DISP_CTRL				0x59
#define SP5K_CAPTURE_FIELD_QV					0x5a
#define SP5K_CAPTURE_COMPRESSION_DELAY			0x5b
#define SP5K_CAPTURE_CUSTOM_QTABLE				0x5c
#define SP5K_CAPTURE_KEEP_YUV_BUF               0x5d
#define SP5K_CAPTURE_FIELD_QV_AHD               0x5e
#define SP5K_CAPTURE_HDR_MODE					0x5f
#define SP5K_CAPTURE_HDR_NUM					0x60
#define SP5K_CAPTURE_HDR_ACC_OPT				0x61
#define SP5K_CAPTURE_OB_WITH_DARK_SUB			0x62
#define SP5K_CAPTURE_RAWMAP						0x63
#define SP5K_CAPTURE_HDR_EV_BIAS				0x64
#define SP5K_CAPTURE_CROP_SCALE_IN_JPEG			0x65
#define SP5K_CAPTURE_YUV_CB_AFTER_STAMP			0x66
#define SP5K_CAPTURE_BURST_INTERVAL             0x67
#define SP5K_CAPTURE_YUV_TRIG_NUM	            0x68
#define SP5K_CAPTURE_MPO_IMGNUM					0x69


/* obsolete: decide when to enter pv mode earlier based on JPEG rows have be processed, 0 to disable */
#define SP5K_CAPTURE_EARLY_SNAP_ENTER_PV_Y		0x70
/* decide when to really trigger sensor snap based on JPEG rows have be processed, 0 to disable */
#define SP5K_CAPTURE_EARLY_SNAP_SEN_TRIG_Y		0x71

/* Definition Of Selector SP5K_CAPTURE_ORIENT */
#define SP5K_CAPTURE_ORIENT_ROTATE_0            0x00
#define SP5K_CAPTURE_ORIENT_ROTATE_90           0x01
#define SP5K_CAPTURE_ORIENT_ROTATE_180          0x02
#define SP5K_CAPTURE_ORIENT_ROTATE_270          0x03
#define SP5K_CAPTURE_ORIENT_FLIP_V              0x04
#define SP5K_CAPTURE_ORIENT_FLIP_H              0x05

/* -- Still advanced capture mode configuration (sp5kStillCapAdvModeSet) -- */
/* Selector */
#define SP5K_CAPTURE_CALIBRATION                0x71
#define SP5K_CAPTURE_SAVE_RAW                   0x72

#define SP5K_CAPTURE_SR_ENABLE                  0x74
#define SP5K_CAPTURE_SR_EDGE_ENHANCE            0x75


/* view display control */
/*
 * sp5kStillCapAdvCfgSet(SP5K_CAPTURE_VIEW_DISP_CTRL, Option);
 * Option:
 *   - Auto:
 *       firmware controls the on/off. All turn off when entering capture,
 *       turn on when entering preivew if the original setting is on.
 *
 *   - Otherwise:
 *       bit[31:24] Enter snap. Image layer setting.
 *       bit[23:16] Enter snap. Osd layer setting.
 *       bit[15:8]  Exit  snap. Image layer setting.
 *       bit[0:7]   Eixt  snap. Osd layer setting.
 *       setting values: 0 -> off, 1 -> on, 2 -> bypass (let host control).
 */
#define SP5K_CAPTURE_VIEW_DISP_OFF        0x00000000
#define SP5K_CAPTURE_VIEW_DISP_ON         0x00000101
#define SP5K_CAPTURE_VIEW_DISP_AUTO       0xffffffff
#define SP5K_CAPTURE_VIEW_DISP_BYPASS     0x00000202
#define SP5K_CAPTURE_VIEW_DISP_BYPASS_OSD 0x00020102

/* -- Callback function selectors -- */
typedef enum {
	/* UINT32 wbCb(UINT32 addr, UINT32 rawFmt, sp5kImgWbInfo_t *pinfo) */
	SP5K_CAPTURE_CALLBACK_WB,
	/* UINT32 rawCb(UINT32 addr, UINT32 rawFmt, sp5kImgSize_t *psize) */
	SP5K_CAPTURE_CALLBACK_RAW,
	/*
	 * UINT32 yuvCb(UINT32 yuvAddr, UINT32 rsvd, sp5kImgVirtualSize_t *psize)
	 * return: [0] stop capture flow after yuvCb.
	 */
	SP5K_CAPTURE_CALLBACK_YUV,
	/*
	 * UINT32 yuvCb(UINT32 yuvAddr, UINT32 rsvd, sp5kImgVirtualSize_t *psize)
	 * return: [0] stop capture flow after yuvCb.
	 */
	SP5K_CAPTURE_CALLBACK_YUV_IN_JPEG,
	/* UINT32 exifpCb(UINT32 imgId, UINT32 bufsize, void *ptr) */
	SP5K_CAPTURE_CALLBACK_EXIF_PREPARE,
	/* UINT32 exifCb(UINT32 imgId, UINT32 bufsize, void *ptr) */
	SP5K_CAPTURE_CALLBACK_EXIF_CREATE,
	/* UINT32 viewNotifyCb(UINT32 rsvd1, UINT32 rsvd2, void *rsvd3) */
	SP5K_CAPTURE_CALLBACK_VIEW_NOTIFY,
	/* UINT32 qvCb(UINT32 imgId, UINT32 qvaddr, sp5kImgSize_t *psize) */
	SP5K_CAPTURE_CALLBACK_QV,
	/* UINT32 vlcCb(UINT32 imgId, UINT32 vlcsize, void *rsvd) */
	SP5K_CAPTURE_CALLBACK_VLC,
	/* UINT32 dcfCb(UINT32 rsvd1, UINT32 rsvd2, void *rsvd3) */
	SP5K_CAPTURE_CALLBACK_DCF,
	/* UINT32 burstCb(UINT32 rsvd1, UINT32 rsvd2, void *rsvd3) */
	SP5K_CAPTURE_CALLBACK_BURST,
	/* UINT32 darksubCb(UINT32 rsvd1, UINT32 rsvd2, void *rsvd3) */
	SP5K_CAPTURE_CALLBACK_DARKSUB,
	/*
	 * UINT32 jpgReencUpdCb(UINT32 proc, UINT32 vlcSize, UINT32 bufSize)
	 * proc   - 0: main, 1: thumbnail, 2: qv.
	 * return - 0: continue re-encode, 1: abort re-encode.
	 */
	SP5K_CAPTURE_CALLBACK_JPG_REENC_UPDATE,
	/**
	 * \note callback function after QV is displayed
	 * UINT32 postQvCb(UINT32 imgId, UINT32 rsvd2, void *rsvd3)
	 */
	SP5K_CAPTURE_CALLBACK_POST_QV,
	/*
	 * UINT32 stampProc(UINT32 rsvd1, UINT32 rsvd2, struct tm *ptime)
	 */
	SP5K_CAPTURE_CALLBACK_STAMP_PROC,
	SP5K_CAPTURE_CALLBACK_HDR,
	SP5K_CAPTURE_CALLBACK_SNAP_TRIGGER,
	SP5K_CAPTURE_CALLBACK_POST_YUV,
	SP5K_CAPTURE_CALLBACK_IQ_TRIGGER,
	SP5K_CAPTURE_CALLBACK_MPF_ATTR_VALUE_SET
} sp5kStillCapCbFunc_e;



/* -- Callback Services (sp5kStillCapRawCallbackSet) -- */
/* raw data formats */
#define SP5K_CAPTURE_RAW_FMT_10                 0x01
#define SP5K_CAPTURE_RAW_FMT_12                 0x02
#define SP5K_CAPTURE_RAW_FMT_14                 0x03
#define SP5K_CAPTURE_RAW_FMT_16                 0x04

/* -- Debug Option -- */
#define SP5K_CAPTURE_DEBUG_1                 	0x01 /* Debug for snap flow's steps */
#define SP5K_CAPTURE_DEBUG_2                 	0x02 /* Print out to files for CDSP parameters */
#define SP5K_CAPTURE_DEBUG_9                 	0x09 /* Print out the information during snap flow */

/* -- Max STAMP strings -- */
#define SP5K_CAPTURE_DATE_CUSTOM_MAX_LEN        SNAP_DATE_CUSTOM_MAX_LEN

/* Raw callback return value */
#define SP5K_RAW_CB_RET_REDO_LSC 		SNAP_RAW_CB_RET_REDO_LSC
#define SP5K_RAW_CB_RET_REDO_WB_OFFSET	SNAP_RAW_CB_RET_REDO_WB_OFFSET

/* YUV callback return value */
#define SP5K_YUV_CB_RET_SKIP_FLOW SNAP_YUV_CB_RET_SKIP_FLOW  /* the snap flow will be skipped if host return this value in yuv callback */
#define SP5K_YUV_CB_RET_REDO_AHD SNAP_YUV_CB_RET_REDO_AHD  /* redo AHD again after yuv callback */
#define SP5K_YUV_CB_RET_SKIP_AND_LOCK_BUFF (SNAP_YUV_CB_RET_LOCK_BUFF | SNAP_YUV_CB_RET_SKIP_FLOW) /* the snap flow will be skipped and rgb/yuv buffer will not be returned if host return this value in yuv callback */
#define SP5K_YUV_CB_RET_RELEASE_ALL_LOCKED_BUFF (SNAP_YUV_CB_RET_UNLOCK_ALL_BUFF) /* all locked rgb/yuv buffer will be returned if host return this value in yuv callback */

#define SP5K_CAPTURE_HDR_MODE_OFF 		SNAP_HDR_MODE_OFF
#define SP5K_CAPTURE_HDR_MODE_ACC 		SNAP_HDR_MODE_ACC
#define SP5K_CAPTURE_HDR_MODE_NORMAL 	SNAP_HDR_MODE_NORMAL

#define SP5K_CAPTURE_HDR_ACC_AVERAGE_OFF	SNAP_HDR_ACC_AVERAGE_OFF
#define SP5K_CAPTURE_HDR_ACC_AVERAGE_ON		SNAP_HDR_ACC_AVERAGE_ON

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define SP5K_FEATURE_CAPTURE_EXIF_PREPARE
#define SP5K_FEATURE_CAPTURE_QV_CALLBACK
#define SP5K_FEATURE_CAPTURE_POST_QV_CALLBACK

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
/* For SP5K callback phototype */
/* typedef UINT32 (*fp_sp5kCallback_t)(UINT32, UINT32, void *); */
#define fp_sp5kCallback_t                       fpsp5kCallback_t

/* Definition Of Structure; sp5kImgSize_t */
typedef struct sp5kImgSize_s {
	UINT32 width;
	UINT32 height;
	UINT32 cropWidth;
	UINT32 cropHeight;
} sp5kImgSize_t;


/* Definition of structure; sp5kImgVirtualSize_t */
typedef struct sp5kImgVirtualSize_s {
	UINT32 width;
	UINT32 height;
	UINT32 phyWidth;
	UINT32 phyHeight;
} sp5kImgVirtualSize_t;

typedef struct sp5kImgWbInfo_s {
	UINT32 width;
	UINT32 height;
	UINT32 cropWidth;
	UINT32 cropHeight;
	UINT8 rowOfs;
	UINT8 row0;
	UINT8 row1;
	UINT8 bayerOrder;
} sp5kImgWbInfo_t;

/* Definition Of Structure; sp5kCaptureFrame_t */
typedef struct sp5kCaptureFrame_s {
	UINT8 *pf;
	UINT32 pathLen;
	UINT16 w;
	UINT16 h;
	UINT16 x;
	UINT16 y;
}sp5kCaptureFrame_t;

/* Definition Of Structure; sp5kCaptureStamp_t */
typedef struct sp5kCaptureStamp_s {
	UINT32 dateFmtId;
	UINT8 *pdate;
	UINT32 dateLen;
	UINT32 backgndColor;
	UINT32 fontColor;
	UINT32 OutlineColor;
	UINT32 scaleRatio;
	UINT16 x;
	UINT16 y;
}sp5kCaptureStamp_t;

/* Definition Of Structure; sp5kCaptureBurstQvLayout_t */
typedef struct sp5kCaptureBurstQvLayout_s {
	UINT32 qvEn;
	UINT16 n;
	UINT16 m;
	UINT16 w;
	UINT16 gapX;
	UINT16 h;
	UINT16 gapY;
}sp5kCaptureBurstQvLayout_t;

/* Preview callback */
typedef UINT32 (*fp_preCallback_t)(
	UINT32 fid, 	/* Frame ID */
	UINT8 *pbuf,	/* Frame buffer pointer */
	UINT32 w,	/* Buffer width */
	UINT32 h	/* Buffer height */
);

/* Preview YUV ROI */
typedef struct sp5kPrevYuvRoi_s {
	UINT32 width;	/* Buffer width */
	UINT32 height;	/* Buffer height */
	UINT32 roix;	/* offset x of roi */
	UINT32 roiy;	/* offset y of roi */
	UINT32 roiw;	/* width of roi */
	UINT32 roih;	/* height of roi */
} sp5kPrevYuvRoi_t;

#if SPCA5210
typedef struct stillDistGmvInfo_s sp5kStillDistGmvInfo_t;


typedef enum {
	SP5K_CAPTURE_DIST_EW_BLOCKS_H = STILL_DIST_EW_BLOCKS_H,
	SP5K_CAPTURE_DIST_EW_BLOCKS_V = STILL_DIST_EW_BLOCKS_V,
	SP5K_CAPTURE_DIST_EW_THRESHOLD_H = STILL_DIST_EW_THRESHOLD_H,
	SP5K_CAPTURE_DIST_EW_THRESHOLD_V = STILL_DIST_EW_THRESHOLD_V,
	SP5K_CAPTURE_DIST_ME_BLOCKS_THRESHOLD = STILL_DIST_ME_BLOCKS_THRESHOLD,
	SP5K_CAPTURE_DIST_GMV_INFO_LENGTH = STILL_DIST_GMV_INFO_LENGTH,
	SP5K_CAPTURE_DIST_EXP_ISO_STEP = STILL_DIST_EXP_ISO_STEP,
	SP5K_CAPTURE_DIST_ENV_MODE = STILL_DIST_ENV_MODE,

} sp5kStillDistCfgSel_e;
#endif

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
/* General APIs */
UINT32 sp5kStillCapCfgSet(UINT32 selector, void *pvalue);
UINT32 sp5kStillCapAbort(void);
UINT32 sp5kStillCapAdvCfgSet(UINT32 selector, UINT32 value);
UINT32 sp5kStillCapStoreConfirm(UINT32 flag);
UINT32 sp5kStillCapJobInfo(UINT32 *pwaitJob, UINT32 *pavailableJob);
UINT32 sp5kStillCapDistCfgSet(UINT32 selector, UINT32 value);
UINT32 sp5kStillCapDistCfgGet(UINT32 selector, UINT32 *pvalue);
UINT32 sp5kStillCapDistStart(void);
UINT32 sp5kStillCapDistStop(void);
UINT32 sp5kStillCapDistGmvInfoGet(sp5kStillDistGmvInfo_t **, UINT32 *);
UINT32 sp5kStillCapDistExpStepGet(UINT32, SINT32 *);

/* Callback functions */
UINT32 sp5kStillCapCallbackSet(UINT32 selector, void *pfunc);
/* Use sp5kStillCapCallbackSet, the following functions are obsolete... */
void sp5kStillCapWbCallbackSet(fp_sp5kCallback_t pfun);
void sp5kStillCapRawCallbackSet(fp_sp5kCallback_t pfun);
void sp5kStillCapYUVCallbackSet(fp_sp5kCallback_t pfun);
void sp5kStillCapBurstCallbackSet(fp_sp5kCallback_t pfun);
void sp5kStillCapExifPrepareCallbackSet(fp_sp5kCallback_t pfun);
void sp5kStillCapExifHdrCallbackSet(fp_sp5kCallback_t pfun);
void sp5kStillCapViewNotifyCallbackSet(fp_sp5kCallback_t pfun);
void sp5kStillCapQvCallbackSet(fp_sp5kCallback_t pfun);
void sp5kStillCapVlcCallbackSet(fp_sp5kCallback_t pfun);
void sp5kStillCapDcfCallbackSet(fp_sp5kCallback_t pfun);

/* preview callbacks */
UINT32 sp5kPreCallbackSet(UINT32 interval, fp_preCallback_t pfunc);
UINT32 sp5kPreviewCallbackSet(UINT32 interval,fp_preCallback_t pfunc,UINT32 w,UINT32 h);
UINT32 sp5kPreviewUrgentCallbackSet(UINT32 interval, fp_preCallback_t pfunc);
UINT32 sp5kPreviewMvInfoGet(UINT32 *fid, SINT16 *x1, SINT16 *y1, SINT16 *x2, SINT16 *y2, UINT32 *numx, UINT32 *numy);

/* Misc */
void sp5kStillCapCropSizeGet(UINT32 *width, UINT32 *height);
void sp5kStillCapQvSizeGet(UINT32 imgW, UINT32 imgH, UINT32 qvW, UINT32 qvH, UINT32 *realW, UINT32 *realH);

void sp5kStillCapFastPreviewShow(void);
UINT32 sp5kStillCapPbBufInit(UINT32 width, UINT32 height);
UINT32 sp5kStillCapPbBufDisplay(UINT8 *paddr);
UINT32 sp5kStillCapPbBufDestory(void);

/* Speedy Burst Preview Yuv */
/**
 * \note pfunc in sp5kPreviewYuvCapCallbackSet()
 * 1st arg: frame idx
 * 2nd arg: YUV buffer address
 * 3rd arg: pointer to sp5kPrevYuvRoi_t
 */
void sp5kPreviewYuvCapCallbackSet(fp_sp5kCallback_t pfunc);
UINT32 sp5kPreviewYuvCapStart(UINT32 num, UINT8 *pbuf, UINT32 maxSize);
UINT32 sp5kPreviewYuvCapStop(void);
UINT32 sp5kPreviewJpegSave(UINT8 *pbuf, sp5kPrevYuvRoi_t *proi, UINT32 jpegW, UINT32 jpegH);
void sp5kPreviewYuvCapDateTimeSet(tmx_t *ptime);

/* Preshot support */
void sp5kStillCapTrigPreShot(UINT32 flag);
void sp5kStillCapPreShotSet(UINT32 flag, UINT32 number);

#endif /* _SP5K_CAPTURE_API_H_ */

