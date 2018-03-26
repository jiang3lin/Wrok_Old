/**************************************************************************
 *
 *       Copyright (c) 2005-2011 by iCatch Technology, Inc.
 *
 *  This software is copyrighted by and is the property of iCatch
 *  Technology, Inc.. All rights are reserved by iCatch Technology, Inc..
 *  This software may only be used in accordance with the corresponding
 *  license agreement. Any unauthorized use, duplication, distribution,
 *  or disclosure of this software is expressly forbidden.
 *
 *  This Copyright notice MUST not be removed or modified without prior
 *  written consent of iCatch Technology, Inc..
 *
 *  iCatch Technology, Inc. reserves the right to modify this software
 *  without notice.
 *
 *  iCatch Technology, Inc.
 *  19-1, Innovation First Road, Science-Based Industrial Park,
 *  Hsin-Chu, Taiwan, R.O.C.
 *
 *  Author: Phacnia Lai
 *
 **************************************************************************/
#ifndef _SP5K_RSVBLK_API_H_
#define _SP5K_RSVBLK_API_H_

/**************************************************************************
 *                         H E A D E R   F I L E S
 **************************************************************************/
#include "common.h"
#include "middleware/rsvblk_def.h"
#include "middleware/disk_def.h"
#include "api/sp5k_gfx_api.h"

/**************************************************************************
 *                           C O N S T A N T S
 **************************************************************************/
/* Resource file type which are available for sp5kResourceFilePlay() */
typedef enum {
	SP5K_RES_TYPE_JPG         = RES_TYPE_JPG,
	SP5K_RES_TYPE_WAV         = RES_TYPE_WAV,
	SP5K_RES_TYPE_WAV_MIX     = RES_TYPE_WAV_MIX,
	SP5K_RES_TYPE_MP3         = RES_TYPE_MP3,
	SP5K_RES_TYPE_WMA         = RES_TYPE_WMA,
	SP5K_RES_TYPE_JPG_PIP     = RES_TYPE_JPG_PIP,
	SP5K_RES_TYPE_JPG_PF      = RES_TYPE_JPG_PF,
	SP5K_RES_TYPE_AVI         = RES_TYPE_AVI,
	SP5K_RES_TYPE_AVI_PIP     = RES_TYPE_AVI_PIP,
	SP5K_RES_TYPE_AVI_PF      = RES_TYPE_AVI_PF,
	SP5K_RES_TYPE_JPG_GFX_OBJ = RES_TYPE_JPG_GFX_OBJ,
	SP5K_RES_FILE_TYPE_NUM    = RES_FILE_TYPE_NUM,
} sp5kResFileType_e;

typedef enum {
	SP5K_RES_SUBTYPE_JPG_MAIN = RES_SUBTYPE_JPG_MAIN,
	SP5K_RES_SUBTYPE_JPG_QV   = RES_SUBTYPE_JPG_QV,
	SP5K_RES_SUBTYPE_JPG_THM  = RES_SUBTYPE_JPG_THM,
	SP5K_RES_SUBTYPE_AVI_THM  = RES_SUBTYPE_AVI_THM,
} sp5kResFileSubType_e;

/* Resource file type which are available for sp5kResourceFileLoad() */
typedef enum {
	SP5K_RES_OSD_FONT = 0x00,
 	SP5K_RES_OSD_ICON,
	SP5K_RES_OSD_ICON_2,
	SP5K_RES_OSD_ICON_3,
	SP5K_RES_OSD_ICON_4,
	SP5K_RES_OSD_ICON_5,
	SP5K_RES_DATE_STAMP_OSD,
	SP5K_RES_OBSOLETE_1,	/* ID to be recycled */
	SP5K_RES_OSD_STRING,
	SP5K_RES_DISP_COLOR_MATRIX,
	SP5K_RES_DISP_GAMMA,
	SP5K_RES_RB_GAMMA,
	SP5K_RES_G_GAMMA,
	SP5K_RES_Y_GAMMA,
	SP5K_RES_ARD,		 	/* new for 5310 */
	SP5K_RES_BP_VIEW,
	SP5K_RES_BP_CAP,
	SP5K_RES_LSC,
	SP5K_RES_AE_WINDOW,
	SP5K_RES_EXPOUSRE,
	SP5K_RES_AGC,
	SP5K_RES_AWB_WINDOW,
	SP5K_RES_AWB_CTT,
	SP5K_RES_USB_STRING,
	SP5K_RES_CDSP_PARAM,
	SP5K_RES_OBSOLETE_3, 	/* ID to be recycled */
	SP5K_RES_EDGE_Y_LUT,
	SP5K_RES_EDGE_E_LUT,
	SP5K_RES_EDGE_CORNER,	/* new for 5310 */
	SP5K_RES_WDR,			/* new for 5310 */
	SP5K_RES_CA,			/* new for 5310 */
	SP5K_RES_ANTI_ALIAS,
	SP5K_RES_HUE_CORRECT,
	SP5K_RES_OBSOLETE_4,	/* ID to be recycled */
	SP5K_RES_AHD,
	SP5K_RES_AHD_AA,		/* new for 5310 */
	SP5K_RES_3D_LUT, 		/* new for 5310 */
	SP5K_RES_LDC,			/* new for 5310 */
	SP5K_RES_FQV_AHD,       /* new for 5310 */
	SP5K_RES_HDR_WEI,       /* new for 5330 */
	SP5K_RES_HDR_DIV,       /* new for 5330 */

	SP5K_RES_PRE_AHD_HUE_CORRECT,	/* 5330 */
	SP5K_RES_PRE_AHD_ANTI_ALIAS,	/* 5330 */
	SP5K_RES_PRE_AHD_EDGE_E_LUT,	/* 5330 */
	SP5K_RES_PRE_AHD_EDGE_CORNER,	/* 5330 */
	SP5K_RES_PRE_AHD_EDGE_Y_LUT,	/* 5330 */

	SP5K_RES_REDO_HUE_CORRECT,	/* 5330 */
	SP5K_RES_REDO_ANTI_ALIAS,	/* 5330 */
	SP5K_RES_REDO_EDGE_E_LUT,	/* 5330 */
	SP5K_RES_REDO_EDGE_CORNER,	/* 5330 */
	SP5K_RES_REDO_EDGE_Y_LUT,	/* 5330 */
	SP5K_RES_NEXT_GAMMA,        /* 5330 */
	SP5K_RES_USB_CONF_BIN,

	SP5K_RES_OSD_FT2_FONT = 100,
	
	SP5K_RES_LOAD_TYPE_MAX,  /* new added item please add before this one. */
} sp5kRsvBlkResLoadType_e;

#define SP5K_RES_AUTO	(-1)

typedef enum {
	SP5K_RES_PRELOAD_NUM = 0x00000000,
	SP5K_RES_CFG_THUMB_ROTATE_BASE,
	SP5K_RES_CFG_QV_ROTATE_BASE,
	SP5K_RES_CFG_QV_3_2_PATCH = 0x80000000,
} sp5kRsvPreloadCfgSelector_e;

typedef enum {
	SP5K_RES_PRELOAD_THUMB_ROTATE_BY_THUMB = 0x00,
	SP5K_RES_PRELOAD_THUMB_ROTATE_BY_PRIMARY,
} sp5kRsvBlkThmRotateBase_t;

typedef enum {
	SP5K_RES_PRELOAD_QV_ROTATE_NONE, /* no rotation */
	SP5K_RES_PRELOAD_QV_ROTATE_BY_EXIF, /* rotate by exif (main or thumbnail) */
} sp5kRsvBlkQvRotateBase_t;

#define SP5K_RES_DRAM_PLAY	( 0x00 )
#define SP5K_RES_DRAM_ABORT ( 0x01 )

#define sp5kRawSegmentPreCb_t	pfDiskPreCb_t
#define sp5kRsvImageRotate_t	rsvImageRotate_t
#define sp5kRsvImageRoi_t		rsvImageRoi_t

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
UINT32 sp5kUDFRead(void);
UINT32 sp5kADFRead(void);
UINT32 sp5kUDFWrite(void);
UINT32 sp5kADFWrite(void);
UINT32 sp5kUDFParamGet(UINT32 offset,UINT32 length,UINT16 *pdata);
UINT32 sp5kUDFParamSet(UINT32 offset,UINT32 length,UINT16 *pdata);
UINT32 sp5kUserResFileAdd(UINT32 resType,UINT32 resIndex,UINT8 *pfilename);
UINT32 sp5kUserResFileDelete(UINT8 *pfilename);
UINT32 sp5kUserResFileCountGet(UINT32 resType,UINT32 *pcount);
/**
 * \note
 * ctrl:  SP5K_RES_DRAM_PLAY or SP5K_RES_DRAM_ABORT
 * pfilename: input WAV filename
 * phandle: output handle for PLAY and input handle for ABORT
 */
UINT32 sp5kResWavPlay(UINT32 ctrl, UINT8 *pfilename, UINT32 *phandle);
UINT32 sp5kResourceFilePlay(UINT32 resType,UINT8 *pfilename,UINT32 param[4]);
/**\note
 * opt: support SP5K_RES_BP_VIEW & SP5K_RES_BP_CAP only
 * resName: sunplus resource file
 * zRes: original res file
 * zResType: size of bp in byte, support 2 & 4 only
 */
UINT32 sp5kResourceFileLoadExtZ(UINT32 opt, UINT8 *resName, UINT8 *zRes, UINT32 zResType);
/**
 * \note
 * param[0]: offset x in the selected page layer, must be multiple of 2
 * param[1]: offset y in the selected page layer
 * param[2]: roi width in the selected page layer, must be multiple of 2
 * param[3]: roi height in the selected page layer
 */
UINT32 sp5kResourceFilePagePlay(UINT32 resType,UINT8 *pfilename,UINT32 pageId,UINT32 param[4], UINT16 bgColor);
/**
 * \note
 * srcParam[0]: offset x in the input file, must be multiple of 2
 * srcParam[1]: offset y in the input file
 * srcParam[2]: roi width in the input file, must be multiple of 2
 * srcParam[3]: roi height in the input file
 * dstParam[0]: offset x in the selected page layer, must be multiple of 2
 * dstParam[1]: offset y in the selected page layer
 * dstParam[2]: roi width in the selected page layer, must be multiple of 2
 * dstParam[3]: roi height in the selected page layer
 */
UINT32 sp5kResourceFileRoiPlay(UINT32 resType,UINT8 *pfilename,UINT32 srcParam[4],UINT32 pageId,UINT32 dstParam[4], UINT16 bgColor);
/**
 * \note
 * pyuvBuf: input YUV buffer
 * bufW   : width of YUV buffer, must be multiple of 16
 * bufH   : height of YUV buffer, must be multiple of 8
 * jpgW   : width of output JPG, must be multiple of 16
 * jpgH   : height of output JPG, must be multiple of 8
 * pfname : filename of output JPG
 */
UINT32 sp5kResourceJpgSave(UINT8 *pyuvBuf,UINT32 bufW,UINT32 bufH,UINT32 jpgW,UINT32 jpgH,UINT8 *pfname);
UINT32 sp5kResourceFileLoad(UINT32 option,UINT8 *pfilename);
UINT32 sp5kResourceFileLoadWithHdrId(UINT8 *pfilename);
UINT32 sp5kResourceBufSizeSet(UINT32 option, UINT32 size);
UINT32 sp5kResDRAMPreLoad( UINT32 resType, UINT8 *pfilename, UINT32 param1, UINT32 param2, UINT32 param3, UINT32 param4 );
UINT32 sp5kResDRAMControl( UINT32 control, UINT32 resHandle, UINT32 param1, UINT32 param2, UINT32 param3, UINT32 param4 );
UINT32 sp5kResDRAMUnload( UINT32 resHandle );
UINT32 sp5kRsvBlkFormat(UINT32 driver);
UINT32 sp5kRawSegmentWrite(UINT32 offset, UINT32 num, UINT8 *pbuf);
UINT32 sp5kRawSegmentRead(UINT32 offset, UINT32 num, UINT8 *pbuf);
void sp5kRawSegmentPreCbSet(sp5kRawSegmentPreCb_t pfunc);

UINT32 sp5kRsvPreloadCfg(UINT32 num);
UINT32 sp5kRsvPreloadDrawCfgSet(UINT32 selector, UINT32 value);
/**
 * \note
 * width: output width in the preload buffer, must be multiple of 8
 * height: output height in the preload buffer
 */
UINT32 sp5kRsvPreloadFile(UINT32 fileType, UINT8 *pfileName, UINT32 loadId, UINT32 width, UINT32 height, UINT16 bgColor);
UINT32 sp5kRsvPreloadGfxObjGet(UINT32 loadId, sp5kGfxObj_t *pgfxobj);
UINT32 sp5kRsvPreloadInfoGet(UINT32 loadId, UINT8 *pfilename, UINT32 *pfileType, UINT32 *pwidth, UINT32 *pheight, UINT32 *plinebyte);
/**
 * \note
 * param[0]: offset x in the selected page layer, must be multiple of 2
 * param[1]: offset y in the selected page layer
 * param[2]: roi width in the selected page layer, must be multiple of 2
 * param[3]: roi height in the selected page layer
 */
UINT32 sp5kRsvPreloadDraw(UINT32 loadId, UINT32 pageId, UINT32 param[4], UINT16 bgColor);
/**
 * \note
 * param[0]: offset x in the selected page layer, must be multiple of 2
 * param[1]: offset y in the selected page layer
 * param[2]: roi width in the selected page layer, must be multiple of 2
 * param[3]: roi height in the selected page layer
 * rotate: rotation
 */
UINT32 sp5kRsvPreloadDrawRotate(UINT32 loadId, UINT32 pageId, UINT32 param[4], UINT16 bgColor, sp5kRsvImageRotate_t rotate);
/**
 * \note
 * srcParam[0]: offset x in the input file, must be multiple of 2
 * srcParam[1]: offset y in the input file
 * srcParam[2]: roi width in the input file, must be multiple of 2
 * srcParam[3]: roi height in the input file
 * dstParam[0]: offset x in the selected page layer, must be multiple of 2
 * dstParam[1]: offset y in the selected page layer
 * dstParam[2]: roi width in the selected page layer, must be multiple of 2
 * dstParam[3]: roi height in the selected page layer
 */
UINT32 sp5kRsvPreloadDrawRoi(UINT32 loadId, UINT32 srcParam[4], UINT32 pageId, UINT32 dstParam[4], UINT16 bgColor);
/**
 * \note
 * srcParam[0]: offset x in the input file, must be multiple of 2
 * srcParam[1]: offset y in the input file
 * srcParam[2]: roi width in the input file, must be multiple of 2
 * srcParam[3]: roi height in the input file
 * dstParam[0]: offset x in the selected page layer, must be multiple of 2
 * dstParam[1]: offset y in the selected page layer
 * dstParam[2]: roi width in the selected page layer, must be multiple of 2
 * dstParam[3]: roi height in the selected page layer
 * rotate: rotation
 */
UINT32 sp5kRsvPreloadDrawRoiRotate(UINT32 loadId, UINT32 srcParam[4], UINT32 pageId, UINT32 dstParam[4], UINT16 bgColor,
								   sp5kRsvImageRotate_t rotate);
UINT32 sp5kRsvPreloadRelease(void);
UINT32 sp5kResPhotoframeAlphaMskBlend(sp5kGfxObj_t *pPhotoObj, sp5kGfxObj_t *pImageObj);

/**
 * \note
 * PhotoParam[0]: Photo Roi W
 * PhotoParam[1]: Photo Roi H
 * ImageParam[0]: Image Roi W
 * ImageParam[1]: Image Roi H
 */
UINT32 sp5kResPhotoframeAlphaMskBlendRoi(sp5kGfxObj_t *pPhotoObj, sp5kGfxObj_t *pImageObj, UINT32 PhotoParam[2], UINT32 ImageParam[2]);

#if 0
/**
 * \note    save the roi of the input YUV buffer to a JPG file with specified size and filename
 * pyuvBuf  : input YUV buffer
 * width    : width of input YUV buffer
 * height   : height of input YUV buffer
 * proi     : pointer to the roi info of the input YUV buffer
 * jpegW    : width of output JPG file
 * jpegH    : height of output JPG file
 * pfname   : pointer to output filename, use dcf name rule if NULL
 */
UINT32 sp5kRsvYuvJpegSave(UINT8 *pyuvBuf, UINT32 width, UINT32 height, sp5kRsvImageRoi_t *proi, UINT32 jpegW, UINT32 jpegH, UINT8 *pfname);
#endif

UINT32 resSysgIdGet( UINT32 res5kId );

#endif /* _SP5K_RSVBLK_API_H_ */

