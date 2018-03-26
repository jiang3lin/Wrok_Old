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
#define HOST_DBG 1
#include "app_com_def.h"
#include "app_com_api.h"

#include "sp5k_capture_api.h"
#include "sp5k_disk_api.h"
#include "app_still.h"
#include "app_view_param.h"
#include "app_disk.h"
#include "app_view_osd.h"
#include "app_lens_api.h"
#include "app_zoom_api.h"
#include "app_audio.h"
#include "app_led.h"
#include "app_strobe.h"
#include "app_ui_para.h"
#include "app_still_timelapse.h"
#include "app_cdfs_api.h"

#if SPCA6330
#include "app_ptp.h"
#endif
#if ICAT_WIFI
#include "app_wifi_utility.h"
#endif
#if MTBF_DBG_EN
#include "../../service/misc/app_mtbf_cmd.h"
#endif

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef struct {
	UINT32 imgId;
	UINT32 svPrg;
	UINT32 cluSzInByte;
	UINT32 dskFree;
	UINT32 estFileSize;
} StillBackgroundCB_t;

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
static StillBackgroundCB_t gSBG = {
	.imgId = 0,
	.svPrg = 0,
	.cluSzInByte = 16384,
	.dskFree = 0,
	.estFileSize = 0,
};

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern UINT32 BlinkGet ;
extern UINT8 timelapse_act;
extern UINT8 TimLapVidFile[64];
extern void SP_AwbPointBufDump(void); // for clean warning
extern UINT32 _StillCheckDiskReady(void);
extern  void app_SpeedRemainNumUpdate(void);
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/*-------------------------------------------------------------------------
 *	Function Name:  appStillFlashRecharge()
 *	Description: The flash/strobe power will decrease with time, we need to
 				 recharge it every a period. However, the lens moving and
 				 strobe charging can not work the same time.
 *------------------------------------------------------------------------*/
void
appStillFlashRecharge(
	UINT32 param
)
{
	if(gStillCB.flashChargeMonitorTimer == param)
	{
		appTimerClear(&gStillCB.flashChargeMonitorTimer);
		if(gStillCB.flashMode != UI_FLASH_OFF)
		{
			if(appLensQuery(LENS_QUERY_MOTION_STATE))
			{
				/* when the lens is moving, we retry later */
				gStillCB.flashChargeMonitorTimer = appTimerSet(10, "Flash ReCharge");
			}
			else
			{
				/* start recharge and wait messge "APP_UI_MSG_STROBE_CHARGE_READY" */
				DBG_PRINT("%s:  STROBE_RECHARGE\n", __FUNCTION__);
				appStrobeChargeStart();
				gStillCB.flashCharging = 1;
			}
		}
	}
}

/*-------------------------------------------------------------------------
 *	Function Name:  appStillBackground()
 *	Description:	It will effect for all still/video group states.
 *					This will be called in appGroupViewProc().
 *					The storing/saving messages may vary with different
 *					memory card, capture size and zoom ratio. So we put
 *					it in group preproces to catch all informations.
 *
 *					1). Here only "memory card full" will stop the burst capture.
 *					    The other stop case will maintain on burst state.
 *
 *					2). We assume the IMAGE_ID will always advance
 * 						SAVE_PROGRESS.
 *
 *					3). Some other functions will also be also maitained here.
 *						osd (hitogram, zoom-bar), background AE for menu state and
 *						face track disable for scene menu. Lens messags share with
 *						video group.
 *------------------------------------------------------------------------*/
void
appStillBackground(UINT32 msg, UINT32 param)
{
	#if KIT_WITH_LENS
	UINT32 tmpU32; // for clean warning
	#endif
	UINT32 cluSzInByte, dskFree, realFileSz;
	UINT16 curDirIdx, curFileIdx;
	uiPara_t* puiPara = appUiParaGet();

	UINT8 fileName[128];

	switch(msg)
	{
		/* monitor flash/strobe */
		case APP_UI_MSG_STROBE_CHARGE_READY:
			DBG_PRINT("%s:  STROBE_CHARGE_READY!\n", __FUNCTION__);
			appStrobeChargeStop();
			gStillCB.flashCharging=0;
			if(gStillCB.flashChargeMonitorTimer!=TIMER_NULL)
			{
				appTimerClear(&gStillCB.flashChargeMonitorTimer);
			}
			gStillCB.flashChargeMonitorTimer = appTimerSet(FLASH_RECHARGE_TIME_PERIOD, "Flash ReCharge");
			GREEN_LED_ON;
			break;

		/* monitor key press */
		case APP_KEY_PRESS_S1:
			gStillCB.s1press=1;
			break;
		case APP_KEY_PRESS_S2:
			gStillCB.s2press=1;
			break;
		case APP_KEY_RELEASE_S1:
			#if PTP_CAPTURE&&ICAT_WIFI
			if(appPtpStatGet())
				appStill_PIMA_Send_iCatch_Event(PTP_ICAT_EVENT_SHUTTER_S1_OFF, 0, 0);
			#endif
			gStillCB.s1press=0;
			break;
		case APP_KEY_RELEASE_S2:
			#if PTP_CAPTURE&&ICAT_WIFI
			if(appPtpStatGet())
				appStill_PIMA_Send_iCatch_Event(PTP_ICAT_EVENT_SHUTTER_S2_OFF, 0, 0);
			#endif
			gStillCB.s2press=0;
			break;

		/* backgroud store */
		case SP5K_MSG_STILL_CAPTURE_IMAGE_ID:
			/* record the raw image number and check the disk size */
			gSBG.imgId++;

			#if CAP_FILE_ERROR  /*just for file error debug*/
			printf("\033[32m^^^^^gSBG.dskFree=%d, gSBG.estFileSize=%d^^^^^\033[0m\n", gSBG.dskFree, gSBG.estFileSize);
			#endif

			if ((gSBG.imgId - gSBG.svPrg + 1)*gSBG.estFileSize > gSBG.dskFree )
			{
				sp5kStillCapAbort();
				DBG_PRINT("%s: memory full\n", __FUNCTION__);
			}
			if(gStillCB.diskSpaceInImgId>gStillCB.estFileSize)
			{
				gStillCB.diskSpaceInImgId -= gStillCB.estFileSize;
			}
			#if SP5K_CDFS_OPT
			if(puiPara->TimeLapse != UI_TIMELAPSE_OFF)
			{
				appCdfsFileFolderSet(CDFS_FILE_FOLDER_JPG);
				appCdfsFolderInit(CDFS_FILE_FOLDER_JPG);
			}
			#endif
			DBG_PRINT("%s: imgId=%d, svPrg=%d para:0x%x\n", __FUNCTION__, gSBG.imgId, gSBG.svPrg,param);
			break;
		case SP5K_MSG_STILL_SAVE_PROGRESSIVE:
			LED_FLICKER_IMAGE_SAVING;
			/* record the save file number and calculate the disk free size */
			gSBG.svPrg++;
			cluSzInByte = gSBG.cluSzInByte;
			dskFree = gSBG.dskFree;
			if(param % cluSzInByte)
			{
				realFileSz = ((param/cluSzInByte +1)*cluSzInByte)>>10;
			}
			else
			{
				realFileSz = param>>10;
			}
			gSBG.dskFree = (dskFree > realFileSz)? (dskFree - realFileSz) : 0;
			/*
			 * All read images have become vlc files, so we can estimate
			 * the rest space and show it on OSD.
			 */
			if((gSBG.imgId == gSBG.svPrg) && (appActiveStateGet()== APP_STATE_STILL_PREVIEW))
			{
				sp5kHostMsgSend(APP_UI_MSG_VIEW_DISKSIZE_READY,0);
			}
			if(gStillCB.estFileSize > realFileSz)
			{
				gStillCB.diskSpaceInImgId += (gStillCB.estFileSize - realFileSz);
			}
			else
			{
				gStillCB.diskSpaceInImgId -= (realFileSz - gStillCB.estFileSize);
			}

			#if CAP_FILE_ERROR  /*just for file error debug*/
			printf("\033[32m^^^dskFree=%d, gSBG.dskFree=%d, gSBG.imgId=%d, gSBG.svPrg=%d^^^\033[0m\n", dskFree, gSBG.dskFree, gSBG.imgId, gSBG.svPrg);
			#endif

			DBG_PRINT("%s: est=%d, real=%d\n", __FUNCTION__, gSBG.estFileSize, realFileSz);
			DBG_PRINT("%s: real/est = [ %d%% ]\n", __FUNCTION__, gSBG.estFileSize>0? realFileSz*100/gSBG.estFileSize:0);
			break;
		case SP5K_MSG_STILL_STORAGE_FULL:
			DBG_PRINT("%s: STORAGE_FULL\n",__FUNCTION__);
			sp5kStillCapAbort();
			break;
		case SP5K_MSG_STILL_CONFIRM_STORE:
			DBG_PRINT("%s: CONFIRM_STORE\n",__FUNCTION__);
			if ((((IS_SCENE_MODE)&&(IS_BLINK_DETECTION_MODE))||puiPara->FaceTracking== UI_FACETRACK_BLINKING_DETECT) && BlinkGet == 0)
			{
				/*UINT32 storeflag=0;*/   /*junqiong.hu@20121126*/
				appStill_SetQV(0);
				appStill_CfgCaptureConfirmStore(0);
				appStill_SetCaptureConfirmStore(1); 	/*junqiong.hu@20121126*/
			}
			break;
		case SP5K_MSG_STILL_STORE_COMPLETE:
			#if MTBF_DBG_EN
			appMTBFStillCapEvtPut();
			#endif
			#if ICAT_WIFI && SPCA6330 /* Notify the pima capture is done */
			{
				if(appPtpStatGet()){
					UINT32 dskFree;
					sp5kUsbPimaCapDoneNotify(0);
					if (appStillBackgroundGetFree(&dskFree) == SUCCESS)
					{
						if(IS_CARD_EXIST)
							appStill_PIMA_Send_Event(PTP_EC_STORAGE_INFO_CHANGED, SP5K_DRIVE_SD, 0, 0);
						else
							appStill_PIMA_Send_Event(PTP_EC_STORAGE_INFO_CHANGED, SP5K_DRIVE_NAND, 0, 0);

						printf("[PIMA Storage Info Changed] dsk free: %d bytes = %d image capacity\n", dskFree, dskFree/gStillCB.estFileSize);
					}
				}
			}
			#endif
			if(!appStrobeQueryChargeEnable())
			{
				LED_NORMAL_ON;
			}
			DBG_PRINT("%s: STORE_COMPLETE,para:0x%x\n",__FUNCTION__,param);
	        	appAePreviewRestore();
			#if 0
			{
				appDiskInfo_t* pDsk;
				pDsk = appCurDiskInfoGet();
				DBG_PRINT("%s: est_diskfree=%d, real_diskfree=%d\n", __FUNCTION__, gSBG.dskFree, pDsk->freeSz);
			}
			#endif
			if (appActiveStateGet()== APP_STATE_STILL_PREVIEW)
			{
				sp5kHostMsgSend(APP_UI_MSG_VIEW_DISKSIZE_READY,0);
			}
			SP_AwbPointBufDump();
			if(puiPara->TimeLapse != UI_TIMELAPSE_OFF)
			{
	        		#if CAM_TYPE_CVR
				strcpy(fileName,appCdfsLastFileNameGet());
	       		#else
				printf("Timelapse Que Put!\n");
				sp5kDcfFsCurKeyGet(&curDirIdx, &curFileIdx);
				sprintf(fileName,"D:\\DCIM\\%03dMEDIA\\SUNP%04d.JPG",curDirIdx,curFileIdx);
				#endif
				#if OLD_TLS_OPT
				appJpgFileNameSet(fileName);
				#endif
				#if SP5K_CDFS_OPT
				appCdfsFileFolderSet(CDFS_FILE_FOLDER_VIDEO);
				appCdfsFolderInit(CDFS_FILE_FOLDER_VIDEO);
				#endif
				sp5kHostMsgSend(APP_UI_MSG_TIMELAPSE_MERGE,0);
				app_SpeedRemainNumUpdate();
				if(puiPara->TimeLapse >= TIMELAPSE_POWER_OFF && timelapse_act)
				{
					sp5kHostMsgSend(APP_UI_MSG_POWER_OFF, APP_POWER_OFF_NORMAL);
				}
			}
			break;
		#if ICAT_WIFI && SPCA6330
		case SP5K_MSG_MEDIA_REC_STOP:
			if(appPtpStatGet())
			{
				UINT32 dskFree;
				if (appStillBackgroundGetFree(&dskFree) == SUCCESS)
				{
					if(IS_CARD_EXIST)
						appStill_PIMA_Send_Event(PTP_EC_STORAGE_INFO_CHANGED, SP5K_DRIVE_SD, 0, 0);
					else
						appStill_PIMA_Send_Event(PTP_EC_STORAGE_INFO_CHANGED, SP5K_DRIVE_NAND, 0, 0);

					printf("[PIMA Storage Info Changed]dsk free: %d bytes = %d image capacity\n",dskFree,dskFree/gStillCB.estFileSize);
				}

				if(app_PTP_ContRecMode_Get()&&app_PTP_Get_DscOpMode(MODE_ACTIVE)==PTP_DSCOP_MODE_VIDEO_ON)
				{
					app_PTP_Set_DscOpMode(PTP_DSCOP_MODE_VIDEO_OFF);

					appStill_PIMA_Send_iCatch_Event(PTP_ICAT_EVENT_OPMODE_CHANGE,PTP_DSCOP_MODE_VIDEO_OFF,0);
				}
			}
			else /* APP closed or WiFi OFF, video record is stopped by FW UI */
			{
				if(app_PTP_ContRecMode_Get()&&app_PTP_Get_DscOpMode(MODE_ACTIVE)==PTP_DSCOP_MODE_VIDEO_ON)
				{
					app_PTP_Set_DscOpMode(PTP_DSCOP_MODE_VIDEO_OFF);
				}
			}
			break;
		#endif

		/* osd and timer*/
		case SP5K_MSG_TIMER:
			//appViewOsd_ZoomBarProcess(param);
			//appViewOsd_HistogramProcess(param);
			//appStillFlashRecharge(param);
			break;
		case APP_UI_MSG_FACE_DETECT_OSD_UPDATE:
			#if 0
			if(appActiveStateGet()== APP_STATE_STILLSENCEMENU)
			{
				appStill_SetFaceTracking(VIEW_FT_STATUS_DESTROY);
				appStillOsd_UpdateFaceOsd(TRUE);
			#endif
			break;

		/* ae awb message */
		case SP5K_MSG_AE_READY:
			if(IS_MENU_STATE(appActiveStateGet()))
			{
				#if SP5K_ALGORITHM_AE

				#else
				appAePreviewAdjust(param);
				#endif
			}
			break;
		case SP5K_MSG_AWB_READY:
			SP_AwbGainRead();
			break;

		/* LENs */
		#if KIT_WITH_LENS
		case APP_UI_MSG_LENS_STOP_NOTIFY:
			if (IS_KEY_TELE_PRESSED)
			{
				/* TELE pressed at this moment */
				tmpU32 = appLensQuery(LENS_QUERY_ZOOM_TELE_PARKED);
				if (!tmpU32)
				{
					appStillZoomTele();
				}
			}
			else if (IS_KEY_WIDE_PRESSED)
			{
				/* WIDE pressed at this moment */
				appStillZoomWide();
			}
			gStillCB.lensZoomSeg = appLensQuery(LENS_QUERY_ZOOM_SEG_POS);
			appAeZoomLevelSet(gStillCB.lensZoomSeg);
			/* lens stop (power-on/zooming)=(1/0) */
			if(!(param & 0x00FF0000))
			{
				appViewOsd_ZoomBarProcessStart();
				if(!appLensQuery(LENS_QUERY_MOTION_STATE))
				{
					if(puiPara->FaceTracking)
					{
						appStill_SetFaceTracking(VIEW_FT_STATUS_RUN);
					}
				}
			}
			break;
		case APP_UI_MSG_LENS_POS_UPDATE:
			tmpU32 = 0xFFUL & param;
			appViewOsd_ZoomFrameDraw(TRUE);
			appViewOsd_OptZoomPosDraw((UINT8)tmpU32);
			break;
		#endif /* KIT_WITH_LENS */

		default:
			break;
	}
}

/*-------------------------------------------------------------------------
 *	Function Name:  appStillBackgroundInit()
 *	Description:	Init it while into still view from other state group
 *------------------------------------------------------------------------*/
void
appStillBackgroundInit(
	UINT32 cluSzInByte,
	UINT32 dskFree,
	UINT32 estFileSize
)
{
	int spaceInKb;

	spaceInKb = dskFree - APP_DISK_DPOF_RSVD_SIZE_KB;
	gSBG.imgId = 0;
	gSBG.svPrg = 0;
	gSBG.cluSzInByte = cluSzInByte==0? 16384 : cluSzInByte;
	gSBG.dskFree = spaceInKb>0? spaceInKb : 0;
	gSBG.estFileSize = estFileSize;
}

/*-------------------------------------------------------------------------
 *	Function Name:  appStillBackgroundSet()
 *	Description:	Set it before into capture/burst.
 *------------------------------------------------------------------------*/
void
appStillBackgroundSet(
	UINT32 estFileSize
)
{
	gSBG.estFileSize = estFileSize;
	gSBG.imgId = gSBG.imgId - gSBG.svPrg;
	gSBG.svPrg = 0;
}

/*-------------------------------------------------------------------------
 *	Function Name:  appStillBackgroundGetFree()
 *	Description:	This can let still view state calculate the image number.
 *					It will be faster than "store complete message". The Unit
 *					is KB.
 *------------------------------------------------------------------------*/
UINT32
appStillBackgroundGetFree(UINT32* dskFree)
{
	if(gSBG.imgId == gSBG.svPrg)
	{
		if(gSBG.dskFree>10000)/*10M*/
		{
			*dskFree = gSBG.dskFree-10000;
		}
		else
		{
			*dskFree =0;
		}
		return SUCCESS;
	}
	else
	{
		if(gSBG.dskFree>10000)/*10M*/
		{
			*dskFree = gSBG.dskFree-10000;
		}
		else
		{
			*dskFree =0;
		}
		return FAIL;
	}
}

/*-------------------------------------------------------------------------
 *	Function Name:  appStillBackgroundDiskFreeUpdate()
 *	Description:	update the disk free
 *	param: get from appCurDiskInfoGet(), freeSz
 *------------------------------------------------------------------------*/


void
appStillBackgroundDiskFreeUpdate(UINT32 dskFree)
{
	int spaceInKb;
	spaceInKb = dskFree - APP_DISK_DPOF_RSVD_SIZE_KB;
	gSBG.dskFree = spaceInKb>0? spaceInKb : 0;
}

UINT32 appStillRemainNumGet(void)
{
	UINT32 dskFree;
	appDiskInfo_t* pDsk;
	uiPara_t* puiPara = appUiParaGet();
	if(_StillCheckDiskReady() == FALSE){
		return 0;
	}
	gStillCB.estFileSize = appStillEstFileSize(puiPara->ImageSize, puiPara->ImageQuality, gStillCB.diskClusterSize);
	printf("gStillCB.isDiskReady:%d\n",gStillCB.isDiskReady);
	printf("gStillCB.estFileSize:%d\n",gStillCB.estFileSize);
	if (gStillCB.isDiskReady && gStillCB.estFileSize){
		pDsk = appCurDiskInfoGet();
		appStillBackgroundDiskFreeUpdate(pDsk->freeSz);
		appStillBackgroundGetFree(&dskFree);
		return (dskFree / gStillCB.estFileSize);
	}
	return 0;
}


