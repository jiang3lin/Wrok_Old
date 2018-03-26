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

#include <stdio.h>
#include "common.h"
#include "middleware/slide_def.h"
#include "middleware/proflog.h"
#include "api/sp5k_usb_api.h"
#include "api/sp5k_rsvblk_api.h"
#include "api/sp5k_os_api.h"
#include "api/sp5k_exif_api.h"
#include "api/sp5k_global_api.h"
#include "api/sp5k_rsvblk_api.h"
#include "api/sp5k_capture_api.h"
#include "api/sp5k_dcf_api.h"
#include "api/sp5k_otrack_api.h"
#include "api/sp5k_disk_api.h"
#include "customization/app_init.h"
#include "dev_init.h"
#include "api/sp5k_modesw_api.h"
#include "app_cdfs_api.h"
#include "api/sp5k_ftrack_api.h"
#include "api/sp5k_capture_api.h" 
#include "app_msg.h"
#include "app_gps.h"
#include "app_ir_key.h"
#include "api/sp5k_fs_api.h"
#include "customization/dev_init.h"
#if ICAT_LDWS || ICAT_HLWS || ICAT_SLWS || ICAT_SRWS
#include "api/sp5k_cvr_warning_system_private.h"
#endif

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
//only for SFN file 
const rsvSize_t OSDRsvSize[] = {
	//[SP5K_RES_OSD_FONT	]		={128*1024},
	[SP5K_RES_OSD_FONT	]			={256*1024},	
	//[SP5K_RES_OSD_ICON	]		={256*1024},
	[SP5K_RES_OSD_ICON	]			={512*1024},
	[SP5K_RES_OSD_ICON_2]			={32*1024},
	[SP5K_RES_OSD_ICON_3]			={32*1024},
	[SP5K_RES_OSD_ICON_4]			={32*1024},
	[SP5K_RES_OSD_ICON_5]			={32*1024},
	[SP5K_RES_DATE_STAMP_OSD]		={32*1024},
};
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern void powerInit(void);
extern void hostMsgMaxNumSet(UINT32 num);
extern UINT32 hostInit( void );
extern void VC_init(UINT32 *pCallbackFunTab, UINT32 numCallbackFun);
extern void h264EncoderRegister(void);
extern void h264DecoderRegister(void);
extern void sp5kModeMultiPbInit(void);/*clean warning*/
extern void sp5kVehicleDetectInit(void);

extern void ldcPvModeInit(void);
extern UINT32 appPtpCustomOperation(UINT32 op, UINT32 par1, UINT32 par2);
extern void appStill_PIMA_Get_cb(UINT32 devPropCode, UINT32 *dat);
extern void appStill_PIMA_Apply_cb(UINT32 devPropCode, UINT32 *dat);
extern UINT32 appPtp_PIMA_File_Filter(UINT32 folder, const UINT8 *fname);
#if SPCA6330
extern void appStillCaptureTrigger(sp5kUsbTrigCaptureType_e cap_type);
extern void appStillCaptureAbort(sp5kUsbTrigCaptureType_e cap_type);
#endif
#if GPS_SUPPORT
extern void appGpsUartInit(void);
#endif
UINT32 snapStaticInit( void );
void snapHeapBlockSet(UINT32 mode, UINT32 rawBlocks, UINT32 jpegBlocks, UINT32 w, UINT32 h, UINT32 codeSize, UINT32 qvW, UINT32 qvH);
void snapMaxSensorSizeGet( UINT32 *, UINT32 * );
UINT32 pbStaticInit( void );
void pbMaxBufSizeSet(UINT32 maxWidth, UINT32 maxHeight, UINT32 yuvRatio, UINT32 jpgRatio);
void sp5kGfxPaletteCallbackInit(void);
extern void vUvcReq_ExtensionUnitCallbackInit(void);
void appHostFastBootInit(void);
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
#if	RESOURCE_IN_RAMDISK

void appCopyAllResourceToRamDisk(UINT8 *srcDisk ,UINT8 *destDisk ,UINT8 *folder)
{
	int cntItem = 0;
	UINT32 ret;
	UINT32 flag;
	char fstname[32];
	char curPath[256];
	const char *flgname;
	UINT32 len;
	UINT32 fnLen=32;
	
	cntItem = sp5kDirFileCntGet(folder);
	if (cntItem != 0){
		
		strcpy(curPath,folder); 
		curPath[0]='H';
		/*printf("A: has %d child: \n", cntItem); */
		sp5kFsDirMake(curPath);
		if (sp5kFsDirChange(folder) == 0){
			while (cntItem){
				fnLen = sizeof(fstname);
				ret = sp5kFsNextFileNameGet(fstname, &fnLen);
				if (ret != 0){
					break;
				}
				flgname = sp5kFsAliasNameGet(fstname,  SP5K_FS_STR_NATIVE);
				if (sp5kFsFileAttrGet(fstname, &flag) == 0){
					if (flag & SP5K_FS_ATTR_DIR){
						/*printf("%-19s%-19s%8s\n",fstname, flgname, "FOLDER"); */
						char DirName[16];
						sprintf(DirName,"%s/%s",curPath,fstname);
						sp5kFsDirMake(DirName);
					}
					else{
						char ScrName[256];
						char DstName[256];
						UINT32 ret=0;
						/*printf("@@@@%-19s%-19s%8s\n", fstname, flgname, "FILE"); */
						sprintf(ScrName,"%s/%s",curPath,fstname);

						sprintf(DstName,"%s/%s",curPath,fstname);
						ScrName[0]=srcDisk;
						DstName[0]=destDisk ;
						/*printf("@@@%-32s\n @@@@@%-32s%8s\n", ScrName, DstName, "FILE");  */
						ret=sp5kFsFileCopy(ScrName,DstName); 
						
						
					}
				}
				cntItem--;
			}
			if (strcmp(curPath, sp5kFsGetCurPath())){
				sp5kFsDirChange(curPath);
			}
		}
		/*printf("\n Search finish!\n"); */
	}
	else{
		printf("RO_RES has no child in.\n");
	}
	sp5kFsDirChange("H:\\");
}

void appDoResourceFromAB2H()
{
   appCopyAllResourceToRamDisk('A' ,'H',"A:\\ADF\\");
   appCopyAllResourceToRamDisk('A' ,'H',"A:\\RO_RES\\");
   appCopyAllResourceToRamDisk('A' ,'H',"A:\\RO_RES\\6330\\");
   appCopyAllResourceToRamDisk('A' ,'H',"A:\\RO_RES\\CALIB\\");
   appCopyAllResourceToRamDisk('A' ,'H',"A:\\RO_RES\\DISP\\");
   appCopyAllResourceToRamDisk('A' ,'H',"A:\\RO_RES\\FTRACK\\");
   appCopyAllResourceToRamDisk('A' ,'H',"A:\\RO_RES\\IQ\\");
   appCopyAllResourceToRamDisk('A' ,'H',"A:\\RO_RES\\UI\\");
   appCopyAllResourceToRamDisk('A' ,'H',"A:\\RO_RES\\WAV\\");
   appCopyAllResourceToRamDisk('A' ,'H',"A:\\RO_RES\\UI\\FONT\\");
   appCopyAllResourceToRamDisk('A' ,'H',"A:\\RO_RES\\UI\\ICON\\");
   appCopyAllResourceToRamDisk('A' ,'H',"A:\\RO_RES\\UI\\SST\\");
   appCopyAllResourceToRamDisk('A' ,'H',"A:\\RO_RES\\UI\\JPG\\");

   appCopyAllResourceToRamDisk('B' ,'H',"B:\\UDF\\");
   appCopyAllResourceToRamDisk('B' ,'H',"B:\\USER_RES\\");
}

#endif

void
rsvBufInit(
	void
)
{
	sp5kResourceBufSizeSet(SP5K_RES_OSD_FONT,		OSDRsvSize[SP5K_RES_OSD_FONT].size);
	sp5kResourceBufSizeSet(SP5K_RES_OSD_ICON,		OSDRsvSize[SP5K_RES_OSD_ICON].size);
	sp5kResourceBufSizeSet(SP5K_RES_OSD_ICON_2,		OSDRsvSize[SP5K_RES_OSD_ICON_2].size);
	sp5kResourceBufSizeSet(SP5K_RES_OSD_ICON_3,		OSDRsvSize[SP5K_RES_OSD_ICON_3].size);
	sp5kResourceBufSizeSet(SP5K_RES_OSD_ICON_4,		OSDRsvSize[SP5K_RES_OSD_ICON_4].size);
	sp5kResourceBufSizeSet(SP5K_RES_OSD_ICON_5,		OSDRsvSize[SP5K_RES_OSD_ICON_5].size);
	sp5kResourceBufSizeSet(SP5K_RES_DATE_STAMP_OSD,	OSDRsvSize[SP5K_RES_DATE_STAMP_OSD].size);
	
	sp5kResourceBufSizeSet(SP5K_RES_OSD_STRING,		22*1024);
	sp5kResourceBufSizeSet(SP5K_RES_BP_VIEW,		16*1024);
	sp5kResourceBufSizeSet(SP5K_RES_BP_CAP,			64*1024);
} 


UINT32 appImageEditSaveCb(UINT8 *buf, UINT32 size)
{
	return 0;
}


UINT32
appSensorInitDoneCb(
	UINT32 param
)
{
	return SUCCESS;
}

/**
 *  @brief      callback function for disk mount to return status
 *
 *  @param[in]  drvId    drive ID of disk
 *  @param[in]  status   the status of disk mount
 *
 *  @return     SUCCESS or FAIL
 *
 *  Default middleware will send sp5k disk message to host to notify
 *  mount status except following is done.
 *  appDiskMountStatusCb() is enabled when host set 1 to SP5K_DISK_CALLBACK_CFG
 *  selector in sp5kSystemCfgSet(). After host calls sp5kDiskMount(drvId),
 *  the callback function will be called by disk mount task in middleware.
 */
UINT32
appDiskMountStatusCb(
	UINT32 drvId,
	UINT32 status
)
{
	UINT32 ret=SUCCESS;

#if 0
	profLogPrintf(0, "mntStatus %d:%d", drvId, status);

	if (status == DISK_MOUNT_STATUS_START) { /* start to mount */
	}
	else if (status == DISK_MOUNT_STATUS_COMPLETE) { /* mount complete */
		UINT32 mnt=0;
		if (isSdCardExist()) {/* SD */
			if (drvId == SP5K_DRIVE_SD) {
				mnt = 1;
			}
		}
		else { /* NAND */
			if (drvId == SP5K_DRIVE_NAND) {
				mnt = 1;
			}
		}
		if (mnt) { /* ready to init DCF */
			UINT8 dirFreeChar[6] = "DICAM";
			UINT8 fileFreeChar[6] = "DSCI";

			if (sp5kDcfFsInit(drvId, 0, 0) == SUCCESS) {
				sp5kDcfFsActive(drvId);
				sp5kDcfFsFreeCharSet(dirFreeChar, fileFreeChar, SP5K_DCF_NEXTKEY_MAX);
			}
		}
	}
	else if (status == DISK_MOUNT_STATUS_FAIL) { /* fail */
	}
#endif

	return ret;
}

/**
 *  @brief        callback function for recorded media file size
 *
 *  @param[in]    fileSize   media file size in byte
 *
 *  @return       SUCCESS or FAIL
 */
UINT32
appMediaRecFileSizeCb(
	UINT32 fileSize
)
{
#if 0
	UINT32 size;

	size = fileSize / (1024*1024);
	if (size >= 512) {
		size = 511;
	}
	sp5kMediaRecAviChunkUpdate(MEDIA_AVI_CHUNK_STRD,
		7, 1, (UINT8 *)&size);
#endif
	return SUCCESS;
}

/*e Add by Aries 09/12/13*/

/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Limitations:                                                          *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void
appInit(
	void
)
{
	UINT32 w,h;
	#if ICAT_FCWS
	char *url= "A:\\RO_RES\\FTRACK\\FCWS.BIN";
	#else
	char *url= "A:\\RO_RES\\FTRACK\\FTRACK.BIN";
	#endif

	printf("-------------------\n");
	printf("appInit start:%dms\n",sp5kOsTimeGet() * 10);
	printf("-------------------\n");
	/* power on */
	powerInit();

	#if SPCA6330
	/*Only for 6330 base r14665 and later.*/
	/* install sp5k mode function separately */
	sp5kModeStillPrevInit();/* sp5k still pv mode */
	sp5kModeStillSnapInit();/* sp5k still snap mode */
	sp5kModeVideoPrevInit();/* sp5k video pv mode */
	sp5kModeVideoRecInit();/* sp5k video rec mode */
	sp5kModeStillPbInit();/* sp5k still pb mode */
	sp5kModeMultiPbInit();/* sp5k multi pb mode */
	sp5kModeAudioRecInit();/* sp5k audio rec mode */
	sp5kModeAvPlayInit();/* sp5k AV playback mode */
	sp5kModeUSBInit();/* sp5k USB mode */
	/* note: the sp5k mode not initialized will be set as idle mode */
	#endif
	
	cmdTaskInit();

	#if RESOURCE_IN_RAMDISK
	appDoResourceFromAB2H();
	sp5kDiskUnMount(SP5K_DRIVE_RSVA);
	sp5kDiskUnMount(SP5K_DRIVE_RSVB);
	printf("@@@@@ DIAK unMount 1\n");
	#endif
	
#if SPCA6330
      /*dw++@20120516. Only for 6330 base r12142 and later.
        *  Host must choose dcf system it use from baseFw r12141. 
        *  sp5kDcfStdSysInit let to standard dcf, sp5kDcfCusSysInit let to custom system.
        */
	#if SP5K_CDFS_OPT
	{
		sp5kDcfAppCbFuncSet_t cbObj=
		{
			.pfnsp5kDcfCbNextFileNameGet = appCdfsNextFileNameGet,
			.pfnsp5kDcfCbFileNameAdd = appCdfsFileAdd,
			.pfnsp5kDcfCbCurFileIdxSet = appCdfsCurFileIdxSet,
			.pfnsp5kDcfCbCurFileIdxGet = appCdfsCurFileIdxGet,
			.pfnsp5kDcfCbFileAttrGet = appCdfsFileAttrGet,
			.pfnsp5kDcfCbFsInfoGet = appCdfsInfoGet,
			.pfnsp5kDcfCbFileOperate = appCdfsFileOperate
		};

		sp5kDcfCusSysInit();
		sp5kDcfAppCallbackCfg(&cbObj);

	}
	#else
	sp5kDcfStdSysInit();
	#endif
#endif

/* disable software watchdog */ /*modify for mantis bug 0048239*/
	#if 0 /*SPCA6330*/    
	/* enable software watchdog as default */
	sp5kSystemCfgSet(SP5K_SYS_HALT_OPERAT_CFG, SP5K_SYS_HALT_DEBUG,
		4000000, 0);
	#endif
	
	#if !(POWERON_PB_WITHOUT_SENSOR)
	#if COMPEL_POWEROFF_CHARGE
	if(!appGetBoot4ChargingFlag())
	#endif
	snapStaticInit();
	#if SPCA6330
	/*jeffery: Only for 6330 base r14574 and later.*/
	panoramaInit();
	#endif
	#endif
	sp5kSystemCfgSet(SP5K_SNAP_SYS_CFG, SNAP_SYS_MAX_COMPRESSION_RATIO, SNAP_MAX_COMPRESSION_RATIO);

	#ifdef SDIO_INTERFACE_ENABLE
	sp5kSystemCfgSet(SP5K_DISK_SD_HW_PIN_CFG, 1);   /* Cathy_V2 GPIO redefined */
	#endif
	
	sp5kSystemCfgSet(SP5K_SNAP_SYS_CFG,SNAP_SYS_CFG_VLC_BUF_NUM,SNAP_HEAP_JPEG_BLOCKS);
	sp5kSystemCfgSet(SP5K_SNAP_SYS_CFG,SNAP_SYS_CFG_RAW_BUF_NUM,SNAP_HEAP_RAW_BLOCKS);

	sp5kStillCapAdvCfgSet(SP5K_CAPTURE_CROP_SCALE_IN_JPEG ,0);/*Add for HDR,fan.xiao*/

	#if SP5K_SENSOR_TG_AD9000_SONY675
	sp5kSystemCfgSet(SP5K_FIXED_PREV_DZ_CDSP_ASPECT, 1);
	#endif
	
#if 0
	snapMaxSensorSizeGet(&w, &h);
#if DRAM_SIZE>0x2000000
	pbMaxBufSizeSet(w, h, 1, 1);
#else
	pbMaxBufSizeSet(w, h, 4, 1); /* Edward 20080828 */	
#endif
#else
	pbMaxBufSizeSet(PIC_SIZE_WIDTH_MAX, PIC_SIZE_HEIGHT_MAX, 1, 3);
#endif

	pbStaticInit();
	
	sp5kGfxPaletteCallbackInit();

	/*media decoders*/
	mjpgDecoderRegister();
	#if SPCA6330
	h264DecoderRegister();
	#endif
	pcmDecoderRegister();
	mulawDecoderRegister();
	alawDecoderAdd();
	adpcmDecoderRegister();
	mp3DecoderRegister();
	wmaDecoderAdd();
	aacDecoderRegister();

	/*media encoders*/
	mjpgEncoderRegister();
	#if SPCA6330
	h264EncoderRegister();
	#endif
	pcmEncoderRegister();
	mulawEncoderRegister();
	alawEncoderAdd();
	adpcmEncoderRegister();
	aacEncoderRegister();

	/*media mux recorders*/
	mediaAviRecorderInit();
	mediaAsfRecorderInit();
	mediaWavRecorderInit();
	mediaMp3RecorderInit();
	mediaMovRecorderInit();

	/*media demux players*/
	mediaAviPlayerInit();
	mediaAsfPlayerInit();
	mediaWavPlayerInit();
	mediaMp3PlayerInit();
	mediaWmaPlayerInit();
	mediaMovPlayerInit();

	/*res coder init*/
    resCoderInit();

	/*sunplus default ae/awb*/
	aeDefaultInit();
	awbDefaultInit();

	/* -- Sunplus DIST support -- */
	stillDistInit();
	videoDistInit();

	/* -- Sunplus face tracking support -- */
	ftrackPvInit();
	ftrackSnapInit();
	ftrackPbInit();
	ftrackPvSoftIIModuleAdd();
	ftrackFDSoftIIModuleAdd();
	ftrackFDSoftUVModuleAdd();
	otrackPvInit();
	
	#if ICAT_FCWS
	sp5kVehicleDetectInit();
	sp5kFaceTrackCfgSet(SP5K_FACE_TRACK_CFG_PROC_CUSTOM_DB_PATH, (UINT32)url);
	#else
	sp5kFaceTrackCfgSet(SP5K_FACE_TRACK_CFG_PROC_CUSTOM_DB_PATH, (UINT32)url);
	#endif
	#if ICAT_LDWS || ICAT_HLWS || ICAT_SLWS || ICAT_SRWS
	UINT8 cvrwsSelector = 0;
	#if ICAT_LDWS
	cvrwsSelector |= LDWS;
	#endif
	#if ICAT_HLWS
	cvrwsSelector |= HEAD_LIGHT;
	#endif
	#if ICAT_SLWS
	cvrwsSelector |= SPEED_LIMIT;
	#endif
	#if ICAT_SRWS
	cvrwsSelector |= SIGN_RECOGNIZE
	#endif
	sp5kCvrWarningSystemInit(cvrwsSelector);
	#endif

	yuvFilterHDRInit();

	
	xdenoiseInit();

	smearPvModeInit();

	sp5kUsbMSDCFuncReg();
	sp5kUsbSIDCFuncReg();
	sp5kUsbVNDRFuncReg();
	sp5kUsbDPSFuncReg();
	#if SPCA6330 && ICAT_WIFI
	sp5kUsbPimaCbRegister(appStill_PIMA_Get_cb, appStill_PIMA_Apply_cb, appPtpCustomOperation, appStillCaptureTrigger, appStillCaptureAbort, appPtp_PIMA_File_Filter);
	#elif SPCA5330
	printf("%s Ln %u. no sp5kUsbPimaCbRegister(...)\n", __FILE__, __LINE__);
	#endif
	#if API_TEST_INIT
	VC_init(NULL, 0);  /* For API-Test */
	#endif
	/*sp5kUsbFuncCreate();*/


	/*
	 * slide show.
	 */
	#if SLIDE_EFFECT_FADE_SUPPORT == 1
	slideEffectFadeInit();
	#endif

	#if SLIDE_EFFECT_FRAME_DRAG_SUPPORT == 1
	slideEffectFrameDragInit();
	#endif

	#if SLIDE_EFFECT_FRAME_SCROLL_SUPPORT == 1
	slideEffectFrameScrollInit();
	#endif

	#if SLIDE_EFFECT_FRAME_STRETCH_SUPPORT == 1
	slideEffectFrameStretchInit();
	#endif

	#if SLIDE_EFFECT_STRIP_DRAG_SUPPORT == 1
	slideEffectStripeDragInit();
	#endif

	#if SLIDE_EFFECT_DRAPE_DRAG_SUPPORT == 1
	slideEffectDrapeDragInit();
	#endif

	#if SLIDE_EFFECT_DRAPE_PROGRESS_SUPPORT == 1
	slideEffectDrapeProgressInit();
	#endif

	#if SLIDE_EFFECT_SLOPE_PROGRESS_SUPPORT == 1
	slideEffectSlopeProgressInit();
	#endif

	#if SLIDE_EFFECT_ZIPPER_DRAG_SUPPORT == 1
	slideEffectZipperDragInit();
	#endif

	#if SLIDE_EFFECT_ZOOM_FADE_SUPPORT == 1
	slideEffectZoomFadeInit();
	#endif

	#if SLIDE_EFFECT_DIAGONAL_CUT_SUPPORT == 1
	slideEffectDiagonalCutInit();
	#endif

	#if SLIDE_EFFECT_STRIPE_FADE_SUPPORT == 1
	slideEffectStripeFadeInit();
	#endif

	#if SLIDE_EFFECT_TWIST_SUPPORT == 1
	slideEffectTwistInit();
	#endif

	#if SLIDE_EFFECT_SWIRL_DRAG_SUPPORT == 1
	slideEffectSwirlDragInit();
	#endif

	#if SLIDE_EFFECT_CROSS_SPLIT_SUPPORT == 1
	slideEffectCrossSplitInit();
	#endif

	#if SLIDE_EFFECT_DOOR_OPEN_SUPPORT == 1
	slideEffectDoorOpenInit();
	#endif

	ldcPvModeInit();
	sp5kCalBpInit();
	sp5kCalLscInit();

	vUvcReq_ExtensionUnitCallbackInit();

	#if MSG_NEW_PROC
	appMsgInit();
	#endif

	appHostFastBootInit();

	#if GPS_SUPPORT
	printf("GPS Init\n");
	appGpsUartInit();
	#endif

	#if IR_KEY_SUPPORT
	printf("IR_KEY Init\n");
	appIrKeyInit();
	#endif
}

