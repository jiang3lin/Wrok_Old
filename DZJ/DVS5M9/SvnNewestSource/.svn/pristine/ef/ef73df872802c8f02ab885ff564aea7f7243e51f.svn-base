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
#define HOST_DBG 0
#include "app_com_def.h"
#include "app_com_api.h"
#include "sp5k_global_api.h"
#include "sp5k_disp_api.h"
#include "sp5k_dcf_api.h"
#include "app_playback_private.h"
#include "app_playback_def.h"
#include "app_playback_osd.h"
#include "app_dpof.h"
#include "app_tvlcd.h" /* Eric 100827 */
#include "api/sp5k_rsvblk_api.h"
#include "api/sp5k_gfx_api.h"
#include "api/sp5k_pip_api.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

#define MPLAY_LAYOUT_LEFTMARGIN            	18
#define MPLAY_LAYOUT_GAP_X             		8
#define MPLAY_LAYOUT_RIGHTMARGIN            150
#define MPLAY_LAYOUT_TOPMARGIN            	20//2
#define MPLAY_LAYOUT_GAP_Y            		10//4
#define MPLAY_LAYOUT_BOTTOMMARGIN           20//34

#define MPLAY_KEY_REPEAT_INTERVAL			300 /* ms */

#define MPLAY_SELECT_MAX					64
#define MPLAY_THUMB_PER_PAGE				9
#define THUMB_OSD_UPDATE_ALL				0
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

#define IS_PB_DISP_LCD		(1) /* %TODO% */
#define IS_PB_TV_PAL		(0) /* %TODO% */

#define THUMB_PAGE_ID(dcfIdx)	(((dcfIdx)+MPLAY_THUMB_PER_PAGE-1)/MPLAY_THUMB_PER_PAGE)
#define THUMB_INDEX(dcfIdx)		(((dcfIdx-1)%MPLAY_THUMB_PER_PAGE)+1)
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

typedef enum {
	MPLAY_SXN_START,
	MPLAY_SXN_END,
	MPLAY_SXN_EXE_START,
	MPLAY_SXN_EXE_END,
} mplayUIKeyCtrl_t;

typedef enum {
	PB_MPLAY_UP,
	PB_MPLAY_DOWN,
	PB_MPLAY_LEFT,
	PB_MPLAY_RIGHT,
	PB_MPLAY_SHIFT_TOTAL,
} mplayPanDir_t;


typedef enum {
	MPLAY_UI_STAT_STANDBY,
	MPLAY_UI_STAT_LOCK_MAIN_MENU,
	MPLAY_UI_STAT_LOCK_CONFIRM,
	MPLAY_UI_STAT_LOCK_ALL_CONFIRM,
	MPLAY_UI_STAT_UNLOCK_ALL_CONFIRM,
	MPLAY_UI_STAT_DELETE_MAIN_MENU,
	MPLAY_UI_STAT_DELETE_CONFIRM,
	MPLAY_UI_STAT_DELETE_ALL_CONFIRM,
	MPLAY_UI_STAT_PAN_SET,
	MPLAY_UI_STAT_TOTAL,
} mplayUIStat_t;


typedef enum {
	MPLAY_MAIN_MENU_SEL_LOCK,
	MPLAY_MAIN_MENU_SEL_DEL,
} mplayMainMenuSel_t;

typedef enum {
	MPLAY_EXE_ALL_LOCK,
	MPLAY_EXE_ALL_UNLOCK,
	MPLAY_EXE_ALL_DEL,
} mplayExecuteAllCmd_t;

typedef struct {
	UINT32 slot[MPLAY_SELECT_MAX]; /* to store the DCF index of selected file */
	UINT16 curSlot;
	UINT16 usedNum;
} mplaySelTable_t;

typedef struct {
	UINT8 fattrLock[MPLAY_THUMB_PER_PAGE+1]; /* file attr : protected ? */
	UINT8 fattrType[MPLAY_THUMB_PER_PAGE+1]; /* file attr : type (aud/vid/memo) */
	UINT8 selStatus[MPLAY_THUMB_PER_PAGE+1]; /* osd icon : protect ? delete ? */
	UINT32 dcfIdx[MPLAY_THUMB_PER_PAGE+1];
	UINT16 focusIdx;
	UINT16 lastIdxInPage;
	UINT16 pageIdx;
} mplayThumbAttrInfo_t;

typedef enum {
	MPLAY_THUMB_ICON_LOCK,
	MPLAY_THUMB_ICON_FILETYPE,
	MPLAY_THUMB_ICON_DELETE,
	MPLAY_THUMB_ICON_ALL,
	MPLAY_THUMB_ICON_TOTAL,
} mplayThumbIconIdDef_t;

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

static sp5kMpbNxMLayout_t mplayNormLayout;
static sp5kMpbEnlargeWindow_t mplayEnlargeWindow;
static UINT32 mplayUIStat;
static UINT8  mplayMainMenuSel=MPLAY_MAIN_MENU_SEL_LOCK;
static mplaySelTable_t mplaySelTbl;
static mplayThumbAttrInfo_t thumbInfo;
static UINT32 thumbOsdX[MPLAY_THUMB_PER_PAGE+1];
static UINT32 thumbOsdY[MPLAY_THUMB_PER_PAGE+1];
static UINT8 mplayShift;
static UINT32 exeProcIdx;
#if SPCA6330
static sp5kMpbFrameSize_t mplayFrameSize;
#endif
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
static void appPbMPlayStExit(UINT32 nextSt, UINT32 stMsg);
static void exeProc_LockExecute(void);
static void exeProc_DeleteExecute(void);

/*-------------------------------------------------------------------------
 *  File Name : zpanUIKeyControl
 *------------------------------------------------------------------------*/
static
void
mplayUIKeyControl(
	UINT8 ctrl
)
{
	PB_VERBOSE("\n%s: %d \n\n", __FUNCTION__, ctrl);

	switch (ctrl){
	case MPLAY_SXN_START:
		appBtnDisable(BTN_4WAY);
		break;
	case MPLAY_SXN_END:
		appBtnEnable(BTN_4WAY);
		break;
	case MPLAY_SXN_EXE_START:
		appBtnDisable(BTN_ALL);
		appBtnEnable(BTN_S2|BTN_PB|BTN_MODE);
		break;
	case MPLAY_SXN_EXE_END:
		appBtnEnable(BTN_ALL);
		break;
	default:
		PB_VERBOSE("MPLAY: undefined key session !!\n");
		break;
	}
}

/*-------------------------------------------------------------------------
 *  File Name : mplayEmulateKeyRepeat
 *------------------------------------------------------------------------*/
static BOOL isDoKeyDebounce=FALSE;
static
void
mplayEmulateKeyRepeat(
	void
)
{
	if (isDoKeyDebounce) 
	{
		isDoKeyDebounce=FALSE;
		APP_SLEEP_MS(MPLAY_KEY_REPEAT_INTERVAL);
	}
	if (IS_KEY_LEFT_PRESSED && mplayShift==PB_MPLAY_LEFT) 
	{
		PB_DBG("\nMPLY: LEFT Repeated !\n\n");
		sp5kHostMsgSend(APP_UI_MSG_PB_MPLAY_LEFT, 0);
	} 
	else if (IS_KEY_RIGHT_PRESSED && mplayShift==PB_MPLAY_RIGHT) 
	{
		PB_DBG("\nMPLY: RIGHT Repeated !\n\n");
		sp5kHostMsgSend(APP_UI_MSG_PB_MPLAY_RIGHT, 0);
	} 
	else if (IS_KEY_UP_PRESSED && mplayShift==PB_MPLAY_UP) 
	{
		PB_DBG("\nMPLY: UP Repeated !\n\n");
		sp5kHostMsgSend(APP_UI_MSG_PB_MPLAY_UP, 0);
	} 
	else if (IS_KEY_DOWN_PRESSED && mplayShift==PB_MPLAY_DOWN) 
	{
		PB_DBG("\nMPLY: DOWN Repeated !\n\n");
		sp5kHostMsgSend(APP_UI_MSG_PB_MPLAY_DOWN, 0);
	} 
	else 
	{
		mplayShift=PB_MPLAY_SHIFT_TOTAL;
	}
}

/*-------------------------------------------------------------------------
 *  File Name : selTable_Reset
 *------------------------------------------------------------------------*/
static
void
selTable_Reset(
	void
)
{
	memset(&mplaySelTbl.slot[0], 0, MPLAY_SELECT_MAX*sizeof(mplaySelTbl.slot[0]));
	mplaySelTbl.curSlot = 0;
	mplaySelTbl.usedNum = 0;
}

/*-------------------------------------------------------------------------
 *  File Name : selTable_FreeSlotGet
 *------------------------------------------------------------------------*/
static
UINT32
selTable_FreeSlotGet(
	void
)
{
	int i;
	UINT16 curSlot;

	curSlot = mplaySelTbl.curSlot;
	for (i=0; i<MPLAY_SELECT_MAX; i++) 
	{
		PB_INDEX_INC(curSlot, 0, MPLAY_SELECT_MAX-1);
		if (mplaySelTbl.slot[curSlot]==0) 
		{
			PB_VERBOSE("%s: Got one @ %d \n", __FUNCTION__, curSlot);
			mplaySelTbl.curSlot = curSlot;
			break;
		}
	}

	if (i==MPLAY_SELECT_MAX) 
	{
		PB_VERBOSE("%s: Out of free slot !!\n", __FUNCTION__);
		curSlot = MPLAY_SELECT_MAX;
	}

	return curSlot;
}

/*-------------------------------------------------------------------------
 *  File Name : selTable_Update
 *------------------------------------------------------------------------*/
static
void
selTable_Update(
	UINT32	slotIdx,
	UINT32	dcfIdx
)
{
	/* dcfIdx : shall contain the DCF index of the selected file, or
	 *        0 to indicate the slot is available for Available scan
	 * selStatus : the UI select status
	 */
	if (slotIdx>=MPLAY_SELECT_MAX) 
	{
		PB_DBG("%s: Illegal slotIdx(%d) !!\n", __FUNCTION__, slotIdx);
		return;
	}

	mplaySelTbl.slot[slotIdx] = dcfIdx;
}

/*-------------------------------------------------------------------------
 *  File Name : selTable_Search
 *------------------------------------------------------------------------*/
static
UINT32
selTable_Search(
	UINT32 data
)
{
	/*-- Input : data : the dcfIndex of the thumbnail
	 *   Output: UINT32: the slot index which contains the input.
	 *           if not found, return MPLAY_SELECT_MAX !!
	 */
	int i;

	for (i=0; i<MPLAY_SELECT_MAX; i++) 
	{
		if (mplaySelTbl.slot[i]==data) 
		{
			//PB_VERBOSE("\n%s: Found (%d) @ index(%d) !!\n\n", __FUNCTION__, data, i);
			break;
		}
	}
	return i;
}
/*-------------------------------------------------------------------------
 *  File Name : mplaySelTableAdd
 *------------------------------------------------------------------------*/
static
UINT32
selTable_Add(
	UINT32 dcfIdx
)
{
	/*-- Input : dcfIdx : the dcfIndex of the thumbnail to be added to the table
	 *   Output: UINT32: FAIL/SUCCESS
	 */

	UINT32 ret;

	ret = selTable_Search(dcfIdx);
	if (ret==MPLAY_SELECT_MAX) 
	{
		/*--Not found in Table, Check free slot to add on */
		ret = selTable_FreeSlotGet();
		if (ret==MPLAY_SELECT_MAX) 
		{
			PB_DBG("%s: Out of slot !!\n", __FUNCTION__);
			return FAIL;
		}
	}

	mplaySelTbl.usedNum++;
	selTable_Update(ret, dcfIdx);
	return SUCCESS;
}
/*-------------------------------------------------------------------------
 *  File Name : selTable_Remove
 *------------------------------------------------------------------------*/
static
UINT32
selTable_Remove(
	UINT32 dcfIdx
)
{
	/*-- Input : dcfIdx : the dcfIndex of the thumbnail to be removed from the table
	 *   Output: UINT32: FAIL/SUCCESS
	 */

	UINT32 ret;

	ret = selTable_Search(dcfIdx);
	if (ret==MPLAY_SELECT_MAX) 
	{
		/*--Not found in Table, Do nothing and exit */
		return SUCCESS;
	}

	/*-- Found in Table, annotate as free slot !! */
	selTable_Update(ret, 0);
	mplaySelTbl.usedNum--;
	return SUCCESS;
}
/*-------------------------------------------------------------------------
 *  File Name : selTable_UsedNumGet
 *------------------------------------------------------------------------*/
static
UINT16
selTable_UsedNumGet(
	void
)
{
	return mplaySelTbl.usedNum;
}

#if HOST_DBG
/*-------------------------------------------------------------------------
 *  File Name : selTable_DebugDump
 *------------------------------------------------------------------------*/
static
void
selTable_DebugDump(
	void
)
{
	int i, j, k=4;
	UINT32 data;

	PB_DBG("\n===== SelTable: Used(%d) ====================\n", mplaySelTbl.usedNum);
	j=0;
	for (i=0; i<MPLAY_SELECT_MAX; i++) 
	{
		data = mplaySelTbl.slot[i];
		if (data>0) 
		{
			j++;
			PB_DBG("[%03d]%04d ", i, data);
		}
		if (j==k) 
		{
			j=0;
			PB_DBG("\n");
		}
	}
}
#endif


/*-------------------------------------------------------------------------
 *  File Name : thumbOsd_DeleteIconDraw
 *------------------------------------------------------------------------*/
static
void
thumbOsd_DeleteIconDraw(
	UINT32 thumbIdx /*-- zeor based */
)
{
	#if HOST_DBG
	if (thumbIdx > MPLAY_THUMB_PER_PAGE) 
	{
		PB_VERBOSE("%s: Illegal thumbIdx(%d) !!\n", __FUNCTION__, thumbIdx);
		return;
	}
	#endif

	if (thumbInfo.selStatus[thumbIdx] && mplayMainMenuSel==MPLAY_MAIN_MENU_SEL_DEL) 
	{
		appPlayOsd_ThumbnailIconDraw(TRUE, MPLAYBACK_DELETE, thumbOsdX[thumbIdx], thumbOsdY[thumbIdx]);
	}
	else 
	{
		appPlayOsd_ThumbnailIconDraw(FALSE, MPLAYBACK_DELETE, thumbOsdX[thumbIdx], thumbOsdY[thumbIdx]);
	}


}
/*-------------------------------------------------------------------------
 *  File Name : thumbOsd_LockIconDraw
 *------------------------------------------------------------------------*/
static
void
thumbOsd_LockIconDraw(
	UINT32 thumbIdx
)
{
	UINT32 iconOsd;
	UINT8 selStatus, fileAttr;

	#if HOST_DBG
	if (thumbIdx > MPLAY_THUMB_PER_PAGE) 
	{
		PB_VERBOSE("%s: Illegal thumbIdx(%d) !!\n", __FUNCTION__, thumbIdx);
		return;
	}
	#endif

	fileAttr = thumbInfo.fattrLock[thumbIdx];
	selStatus = (thumbInfo.selStatus[thumbIdx] &&
				mplayMainMenuSel==MPLAY_MAIN_MENU_SEL_LOCK);

	if (selStatus==0) 
	{
		/*-- attribute not changed */
		iconOsd = (fileAttr) ? MPLAYBACK_PROTECTED : 0xFFFF;
	} 
	else
	{
		iconOsd = (fileAttr) ? MPLAYBACK_UNPROTECTED : MPLAYBACK_NEWPROTECTED;
	}

	if (iconOsd==0xFFFF) 
	{
		appPlayOsd_ThumbnailIconDraw(FALSE, MPLAYBACK_PROTECTED, thumbOsdX[thumbIdx], thumbOsdY[thumbIdx]);
	} 
	else 
	{
		appPlayOsd_ThumbnailIconDraw(TRUE, iconOsd, thumbOsdX[thumbIdx], thumbOsdY[thumbIdx]);
	}

}

/*-------------------------------------------------------------------------
 *  File Name : thumbOsd_FileTypeIconDraw
 *------------------------------------------------------------------------*/
static
void
thumbOsd_FileTypeIconDraw(
	UINT32 thumbIdx /*-- zeor based */
)
{
	UINT32 iconOsd;
	UINT8 fileType, bShow;

	#if HOST_DBG
	if (thumbIdx > MPLAY_THUMB_PER_PAGE) 
	{
		PB_VERBOSE("%s: Illegal thumbIdx(%d) !!\n", __FUNCTION__, thumbIdx);
		return;
	}
	#endif

	fileType = thumbInfo.fattrType[thumbIdx];
	bShow = TRUE;
	iconOsd = MPLAYBACK_VIDEO;
	switch(fileType) 
	{
		default:
			bShow = FALSE;
			break;
		case PB_FT_MOV:
		case PB_FT_AVI:
		case PB_FT_MP4:
			iconOsd = MPLAYBACK_VIDEO;
			break;
		case PB_FT_JPG_MEMO:
			iconOsd = MPLAYBACK_VOICE_MEMO;
			break;
	}
	appPlayOsd_ThumbnailIconDraw(bShow, iconOsd, thumbOsdX[thumbIdx], thumbOsdY[thumbIdx]);

}

/*-------------------------------------------------------------------------
 *  File Name : thumbOsd_DeleteLockedWarning
 *------------------------------------------------------------------------*/
static
void
thumbOsd_DeleteLockedWarning(
	UINT32 thumbIdx
)
{
	int i;

	/* Warning Sound */
	appPreloadSoundPlay(SOUND_ID_WARNING);
	/* Blink ICON 4(2/sec)*/
	for (i=0; i<4; i++) 
	{
		appPlayOsd_ThumbnailIconDraw(TRUE, MPLAYBACK_PROTECTED, thumbOsdX[thumbIdx], thumbOsdY[thumbIdx]);
		APP_SLEEP_MS(250);
		appPlayOsd_ThumbnailIconDraw(FALSE, MPLAYBACK_PROTECTED, thumbOsdX[thumbIdx], thumbOsdY[thumbIdx]);
		APP_SLEEP_MS(250);
	}
	appPlayOsd_ThumbnailIconDraw(TRUE, MPLAYBACK_PROTECTED, thumbOsdX[thumbIdx], thumbOsdY[thumbIdx]);
}
/*-------------------------------------------------------------------------
 *  File Name : thumbOsdInfo_DispXYSetup
 Coordinates of images in MPB:
(Thumb ID)

 (1)______________x,y,z,w________________=(18,2,105,65)
 (2)______________x,y,z,w________________=(114,2,201,65)
 (3)______________x,y,z,w________________=(210,2,297,65)
 (4)______________x,y,z,w________________=(18,70,105,133)
 (5)______________x,y,z,w________________=(114,70,201,133)
 (6)______________x,y,z,w________________=(210,70,297,133)
 (7)______________x,y,z,w________________=(18,138,105,201)
 (8)______________x,y,z,w________________=(114,138,201,201)
 (9)______________x,y,z,w________________=(210,138,297,201)


 *------------------------------------------------------------------------*/
static
void
thumbOsdInfo_DispXYSetup(
	void
)
{
	int i;

	i=0;
	thumbOsdX[i]=0; thumbOsdY[i]=0; i++;
	
	#if KIT_WITH_HDMI
	if(IS_HDMI_IN){
		thumbOsdX[i]=90; thumbOsdY[i]=28; i++;
		thumbOsdX[i]=240; thumbOsdY[i]=28; i++;
		thumbOsdX[i]=390; thumbOsdY[i]=28; i++;

		thumbOsdX[i]=90; thumbOsdY[i]=132; i++;
		thumbOsdX[i]=240; thumbOsdY[i]=132; i++;
		thumbOsdX[i]=390; thumbOsdY[i]=132; i++;

		thumbOsdX[i]=90;thumbOsdY[i]=236; i++;
		thumbOsdX[i]=240; thumbOsdY[i]=236; i++;
		thumbOsdX[i]=390; thumbOsdY[i]=236; i++;
	}else{
	#else
		{
	#endif
		thumbOsdX[i]=24; thumbOsdY[i]=10/*20*/; i++;
		thumbOsdX[i]=122/*114*/; thumbOsdY[i]=10; i++;
		thumbOsdX[i]=220/*205*/; thumbOsdY[i]=10; i++;

		thumbOsdX[i]=24; thumbOsdY[i]=79/*88*/; i++;
		thumbOsdX[i]=122; thumbOsdY[i]=79; i++;
		thumbOsdX[i]=220; thumbOsdY[i]=79; i++;

		thumbOsdX[i]=24;thumbOsdY[i]=148/*156*/; i++;
		thumbOsdX[i]=122; thumbOsdY[i]=148; i++;
		thumbOsdX[i]=220; thumbOsdY[i]=148; i++;
	}
}
/*-------------------------------------------------------------------------
 *  File Name : thumbOsdInfo_Reset
 *------------------------------------------------------------------------*/
static
void
thumbOsdInfo_Reset(
	void
)
{
	int i;

	//PB_VERBOSE("%s \n", __FUNCTION__);

	/*-- Note: I waste [0] to coincident with thumbnail index
	 *         which started from 1.
	 */
	for (i=1; i<=MPLAY_THUMB_PER_PAGE; i++) {
		thumbInfo.fattrLock[i]=0;
		thumbInfo.fattrType[i]=PB_FT_UNKNOWN;
		thumbInfo.selStatus[i]=0;
		thumbInfo.dcfIdx[i]=0;
	}
	thumbInfo.pageIdx=0;
	thumbInfo.lastIdxInPage=0;
	thumbInfo.focusIdx=1;
}
#if 0
/*-------------------------------------------------------------------------
 *  File Name : thumbOsdInfo_SelStatusReset
 *------------------------------------------------------------------------*/
static
void
thumbOsdInfo_SelStatusReset(
	void
)
{
	int i;

	//PB_VERBOSE("%s \n", __FUNCTION__);

	/*-- Note: I waste [0] to coincident with thumbnail index
	 *         which started from 1.
	 */
	for (i=1; i<=MPLAY_THUMB_PER_PAGE; i++) {
		thumbInfo.selStatus[i]=0;
	}
}
#endif
/*-------------------------------------------------------------------------
 *  File Name : thumbOsdInfo_Construct
 *------------------------------------------------------------------------*/
static
void
thumbOsdInfo_Construct(
	void
)
{
	int i, offset;
	UINT32 tmpU32, tmpIdx;
	sp5kDcfAttrElm_t fileInfoAttr;
	UINT8 szFilename[48];

	tmpU32 = gcurrDcfIdx % MPLAY_THUMB_PER_PAGE;
	thumbInfo.focusIdx = (UINT16)((tmpU32==0) ? MPLAY_THUMB_PER_PAGE : tmpU32);
	thumbInfo.pageIdx = THUMB_PAGE_ID(gcurrDcfIdx);
	#if HOST_DBG
	PB_VERBOSE("focusIdx(%d), pageIdx(%d), dcfIdx(%d) \n",
				thumbInfo.focusIdx, thumbInfo.pageIdx, gcurrDcfIdx);
	#endif

	for (i=1; i<=MPLAY_THUMB_PER_PAGE; i++) {
		/*-- Calculate & check DFC Index */
		offset = i - thumbInfo.focusIdx;
		tmpU32 = gcurrDcfIdx+offset;
		if (tmpU32>gtotalImgCount) {
			PB_VERBOSE("%s: Out of last file \n", __FUNCTION__);
			break;
		}

		/*-- DCF Index */
		thumbInfo.dcfIdx[i] = tmpU32;

		/*-- Select Status  */
		tmpIdx = selTable_Search(tmpU32);
		if (tmpIdx==MPLAY_SELECT_MAX) {
			/*-- No record in SelTable */
			thumbInfo.selStatus[i] = FALSE;
		} else {
			/*-- Ever selected, restore selStatus */
			thumbInfo.selStatus[i] = TRUE;
		}

		#if 0 //HOST_DBG
		PB_VERBOSE("thumbIdx(%d): focusIdx(%d), offset(%d), dcfIdx(%d) \n",
					i, thumbInfo.focusIdx, offset, tmpU32);
		#endif

		/*-- Get DCF Attributes */
		sp5kDcfFsFileAttrGet(tmpU32, &fileInfoAttr, &szFilename[0]);

		/*-- Lock attr */
		if (fileInfoAttr.attribute & SP5K_DCF_FILE_ATTR_LOCK) 
		{
			thumbInfo.fattrLock[i]=TRUE;
		} 
		else 
		{
			thumbInfo.fattrLock[i]=FALSE;
		}

		/*-- File Type */
		tmpU32 = fileInfoAttr.fileType;
		if (fileInfoAttr.attribute & SP5K_DCF_FILE_ATTR_MEMO) 
		{
			thumbInfo.fattrType[i]=PB_FT_JPG_MEMO;
		}
		else if (tmpU32 & SP5K_DCF_FILETYPE_AVI) 
		{
			thumbInfo.fattrType[i]=PB_FT_AVI;
		}
		else if (tmpU32 & SP5K_DCF_FILETYPE_MOV) 
		{
			thumbInfo.fattrType[i]=PB_FT_MOV;
		}  
		else if (tmpU32 & SP5K_DCF_FILETYPE_WAV) 
		{
			thumbInfo.fattrType[i]=PB_FT_WAV;
		} 
		else if (tmpU32 & SP5K_DCF_FILETYPE_JPG) 
		{
			thumbInfo.fattrType[i]=PB_FT_JPG;
		} 
		else if (tmpU32 & SP5K_DCF_FILETYPE_MP4) 
		{
			thumbInfo.fattrType[i]=PB_FT_MP4;
		}
		else 
		{
			thumbInfo.fattrType[i]=PB_FT_UNKNOWN;
		}

		#if HOST_DBG
		PB_DBG("thumbIdx(%d): Lock(%d), Type(%d), dcfIdx(%d) \n",
					i, thumbInfo.fattrLock[i], thumbInfo.fattrType[i],
					thumbInfo.dcfIdx[i]);
		#endif
	}

	thumbInfo.lastIdxInPage = i-1;
	#if HOST_DBG
	PB_DBG("\nlastIdxInPage(%d), thumbPageId(%d) \n\n",
			thumbInfo.lastIdxInPage, thumbInfo.pageIdx);
	#endif

}

/*-------------------------------------------------------------------------
 *  File Name : thumbOsdInfo_DispUpdate
 *------------------------------------------------------------------------*/
static
void
thumbOsdInfo_DispUpdate(
	UINT32 thumbIdx,
	UINT32 whichIcon
)
{
	/*-- thumbIdx : 1~MPLAY_THUMB_PER_PAGE to update designated file.
	 *              (THUMB_OSD_UPDATE_ALL) to update all
	 *-- whichIcon: see mplayThumbIconIdDef_t
	 */

	int i;
	PB_VERBOSE("%s: thumbIdx(%d), whichIcon(%d) \n", __FUNCTION__, thumbIdx, whichIcon);

	#if HOST_DBG
	if (thumbIdx > MPLAY_THUMB_PER_PAGE) 
	{
		PB_VERBOSE("%s: Illegal thumbIdx(%d) !!\n", __FUNCTION__, thumbIdx);
		return;
	}
	#endif

	if (thumbIdx==THUMB_OSD_UPDATE_ALL) 
	{
		/*-- Update All Thumbnails */
		for (i=1; i<=thumbInfo.lastIdxInPage; i++) 
		{
			switch(whichIcon) 
			{
				default:
					break;
				case MPLAY_THUMB_ICON_ALL:
					thumbOsd_LockIconDraw(i);
					thumbOsd_DeleteIconDraw(i);
					thumbOsd_FileTypeIconDraw(i);
					break;
				case MPLAY_THUMB_ICON_LOCK:
					thumbOsd_LockIconDraw(i);
					break;
				case MPLAY_THUMB_ICON_FILETYPE:
					thumbOsd_FileTypeIconDraw(i);
					break;
				case MPLAY_THUMB_ICON_DELETE:
					thumbOsd_DeleteIconDraw(i);
					break;
			}
		}
	}
	else 
	{
		/*-- Update Designated Thumbnail
		 */
		switch(whichIcon) 
		{
			default:
				break;
			case MPLAY_THUMB_ICON_ALL:
				thumbOsd_LockIconDraw(thumbIdx);
				thumbOsd_DeleteIconDraw(thumbIdx);
				thumbOsd_FileTypeIconDraw(thumbIdx);
				break;
			case MPLAY_THUMB_ICON_LOCK:
				thumbOsd_LockIconDraw(thumbIdx);
				break;
			case MPLAY_THUMB_ICON_FILETYPE:
				thumbOsd_FileTypeIconDraw(thumbIdx);
				break;
			case MPLAY_THUMB_ICON_DELETE:
				thumbOsd_DeleteIconDraw(thumbIdx);
				break;
		}
	}
}

/*-------------------------------------------------------------------------
 *  File Name : thumbOsdInfo_DispClear
 *------------------------------------------------------------------------*/
static
void
thumbOsdInfo_DispClear(
	UINT32 thumbIdx,
	UINT32 whichIcon
)
{
	/*-- thumbIdx : 1~MPLAY_THUMB_PER_PAGE to update designated file.
	 *              (THUMB_OSD_UPDATE_ALL) to update all
	 *-- whichIcon: see mplayThumbIconIdDef_t
	 */

	int i;
	PB_VERBOSE("%s: thumbIdx(%d), whichIcon(%d) \n", __FUNCTION__, thumbIdx, whichIcon);

	#if HOST_DBG
	if (thumbIdx > MPLAY_THUMB_PER_PAGE) 
	{
		PB_VERBOSE("%s: Illegal thumbIdx(%d) !!\n", __FUNCTION__, thumbIdx);
		return;
	}
	#endif

	if (thumbIdx==THUMB_OSD_UPDATE_ALL) 
	{
		/*-- Clear All Thumbnails*/
		for (i=1; i<=MPLAY_THUMB_PER_PAGE; i++) 
		{
			switch(whichIcon) 
			{
			default:
				break;
			case MPLAY_THUMB_ICON_ALL:
				/*-- Lock/Delete/FileType Icon */
				appPlayOsd_ThumbnailIconDraw(FALSE, MPLAYBACK_PROTECTED, thumbOsdX[i], thumbOsdY[i]);
				appPlayOsd_ThumbnailIconDraw(FALSE, MPLAYBACK_DELETE, thumbOsdX[i], thumbOsdY[i]);
				appPlayOsd_ThumbnailIconDraw(FALSE, MPLAYBACK_VOICE_MEMO, thumbOsdX[i], thumbOsdY[i]);
				break;
			case MPLAY_THUMB_ICON_LOCK:
				/*-- Lock Icon */
				appPlayOsd_ThumbnailIconDraw(FALSE, MPLAYBACK_PROTECTED, thumbOsdX[i], thumbOsdY[i]);
				break;
			case MPLAY_THUMB_ICON_FILETYPE:
				/*-- FileType Icon */
				appPlayOsd_ThumbnailIconDraw(FALSE, MPLAYBACK_VOICE_MEMO, thumbOsdX[i], thumbOsdY[i]);
				break;
			case MPLAY_THUMB_ICON_DELETE:
				/*-- Delete Icon */
				appPlayOsd_ThumbnailIconDraw(FALSE, MPLAYBACK_DELETE, thumbOsdX[i], thumbOsdY[i]);
				break;
			}
		}
	}
	else 
	{
		/*-- Update Designated Thumbnail
		 */
		switch(whichIcon) 
		{
			default:
				break;
			case MPLAY_THUMB_ICON_ALL:
				/*-- Lock/Delete/FileType Icon */
				appPlayOsd_ThumbnailIconDraw(FALSE, MPLAYBACK_PROTECTED, thumbOsdX[thumbIdx], thumbOsdY[thumbIdx]);
				appPlayOsd_ThumbnailIconDraw(FALSE, MPLAYBACK_DELETE, thumbOsdX[thumbIdx], thumbOsdY[thumbIdx]);
				appPlayOsd_ThumbnailIconDraw(FALSE, MPLAYBACK_VOICE_MEMO, thumbOsdX[thumbIdx], thumbOsdY[thumbIdx]);
				break;
			case MPLAY_THUMB_ICON_LOCK:
				/*-- Lock Icon */
				appPlayOsd_ThumbnailIconDraw(FALSE, MPLAYBACK_PROTECTED, thumbOsdX[thumbIdx], thumbOsdY[thumbIdx]);
				break;
			case MPLAY_THUMB_ICON_FILETYPE:
				/*-- FileType Icon */
				appPlayOsd_ThumbnailIconDraw(FALSE, MPLAYBACK_VOICE_MEMO, thumbOsdX[thumbIdx], thumbOsdY[thumbIdx]);
				break;
			case MPLAY_THUMB_ICON_DELETE:
				/*-- Delete Icon */
				appPlayOsd_ThumbnailIconDraw(FALSE, MPLAYBACK_DELETE, thumbOsdX[thumbIdx], thumbOsdY[thumbIdx]);
				break;
		}
	}
}


/*-------------------------------------------------------------------------
 *  File Name : MPLAY_ScrollBar
 *------------------------------------------------------------------------*/
#define MPLAY_SCROLL_BAR_W		10
#define MPLAY_SCROLL_BAR_H		196
#define MPLAY_SCROLL_BAR_X		306
#define MPLAY_SCROLL_BAR_Y		24//4
#define MPLAY_SCROLL_FRAME_W	2

static
void
MPLAY_ScrollBar(
	UINT32 bShow,
	UINT32 cur,
	UINT32 total,
	UINT32 numPerPage
)
{
	float osdHeightPerPage;
	UINT32 csrY, csrH;
	UINT32 barX,barY,barW,barH,barFrameW;

	#if KIT_WITH_HDMI
	if(IS_HDMI_IN){
		barX = 560;
		barY = 50;
		barW = 20;
		barH = 300;
		barFrameW = 3;
	}else{
	#else
	{
	#endif
		barX = MPLAY_SCROLL_BAR_X;
		barY = MPLAY_SCROLL_BAR_Y;
		barW = MPLAY_SCROLL_BAR_W;
		barH = MPLAY_SCROLL_BAR_H;
		barFrameW = MPLAY_SCROLL_FRAME_W;
	}
	
	if (bShow==FALSE) 
	{
		appOsd_ColorDrawShapeColorSet(0x00000000, PAL_BLEND_100, 0x00000000, PAL_BLEND_100);
		appOsdLib_PIPLayerShapeDraw(
				APP_OSD_SHAPE_RECT,
				barX, barY, barW, barH, 0, 0);
		return;
	}

	total = (total+numPerPage-1) / numPerPage;
	cur = (cur-1) / numPerPage;

	//PB_VERBOSE("total(%d), cur(%d) \n", total, cur);
	osdHeightPerPage = ((float)(barH-(barFrameW<<1)))/total;
	csrH = (UINT32)osdHeightPerPage;
	csrY = (UINT32)(osdHeightPerPage*cur);

	//PB_VERBOSE("csrH(%d), csrY(%d), hPerPage(%lf) \n", csrH, csrY, osdHeightPerPage);

	/*-- Draw BAR Background */
	appOsd_ColorDrawShapeColorSet(PIP_COLOR565_BLUE, PAL_BLEND_100, PIP_COLOR565_BLUE, PAL_BLEND_100);
	appOsdLib_PIPLayerShapeDraw(
			APP_OSD_SHAPE_RECT,
			barX, barY, barW, barH, 0, 0);

	/*-- Draw BAR Cursor */
	appOsd_ColorDrawShapeColorSet(PIP_COLOR565_WHITE, PAL_BLEND_100, PIP_COLOR565_WHITE, PAL_BLEND_100);
	appOsdLib_PIPLayerShapeDraw(
			APP_OSD_SHAPE_RECT,
			barX+barFrameW,
			(SINT32)(csrY+barFrameW+barY),
			barW-(barFrameW<<1),
			(SINT32)csrH,
			0, 0);

}

/*-------------------------------------------------------------------------
 *  File Name : MPLAY_LockConfirm_DlgConfig
 *------------------------------------------------------------------------*/
static
void
MPLAY_LockConfirm_DlgConfig(
	void
)
{
	//PB_VERBOSE("%s \n", __FUNCTION__);

	appPlayOsd_DlgConfirmYesNoConfig(ID_STR_UNPROTECT_FILE_, 0);
}

/*-------------------------------------------------------------------------
 *  File Name : MPLAY_LockConfirm_DlgShow
 *------------------------------------------------------------------------*/
static
void
MPLAY_LockConfirm_DlgShow(
	void
)
{
	appPlayOsd_DlgBoxShow();
	appPbOsd_AuxNote_OKSet(TRUE);
}

/*-------------------------------------------------------------------------
 *  File Name : MPLAY_DeleteConfirm_DlgConfig
 *------------------------------------------------------------------------*/
static
void
MPLAY_DeleteConfirm_DlgConfig(
	void
)
{
	appPlayOsd_DlgConfirmYesNoConfig(ID_STR_DELETE_ALL_FILE_, 0);
}

/*-------------------------------------------------------------------------
 *  File Name : MPLAY_DeleteConfirm_DlgShow
 *------------------------------------------------------------------------*/
static
void
MPLAY_DeleteConfirm_DlgShow(
	void
)
{
	appPlayOsd_DlgBoxShow();
	appPbOsd_AuxNote_OKSet(TRUE);
}

/*-------------------------------------------------------------------------
 *  File Name : MPLAY_LockAll_DlgConfig
 *------------------------------------------------------------------------*/
static
void
MPLAY_LockAll_DlgConfig(
	void
)
{
	appPlayOsd_DlgConfirmYesNoConfig(ID_STR_PROTECT_ALL_, 0);
}

/*-------------------------------------------------------------------------
 *  File Name : MPLAY_LockAll_DlgShow
 *------------------------------------------------------------------------*/
static
void
MPLAY_LockAll_DlgShow(
	void
)
{
	appPlayOsd_DlgBoxShow();
	appPbOsd_AuxNote_OKSelect(TRUE);
}

/*-------------------------------------------------------------------------
 *  File Name : MPLAY_UnLockAll_DlgConfig
 *------------------------------------------------------------------------*/
static
void
MPLAY_UnLockAll_DlgConfig(
	void
)
{
	appPlayOsd_DlgConfirmYesNoConfig(ID_STR_UNPROTECT_ALL_, 0);
}

/*-------------------------------------------------------------------------
 *  File Name : MPLAY_UnLockAll_DlgShow
 *------------------------------------------------------------------------*/
static
void
MPLAY_UnLockAll_DlgShow(
	void
)
{
	appPlayOsd_DlgBoxShow();
	appPbOsd_AuxNote_OKSelect(TRUE);
}

/*-------------------------------------------------------------------------
 *  File Name : MPLAY_LockMain_DlgConfig
 *------------------------------------------------------------------------*/
static UINT32 dlgStrID_SelectProtUnProtAllBack[4] = {ID_STR_SELECT, ID_STR_NO, ID_STR_UNPROTECT_ALL_, ID_STR_BACK};
static
void
MPLAY_LockMain_DlgConfig(
	void
)
{
	appPlayOsd_DlgBoxConfig(ID_STR_PROTECT_FILE_, 4, dlgStrID_SelectProtUnProtAllBack, 0, -10);
}

/*-------------------------------------------------------------------------
 *  File Name : MPLAY_LockMain_DlgShow
 *------------------------------------------------------------------------*/
static
void
MPLAY_LockMain_DlgShow(
	void
)
{
	appPlayOsd_DlgBoxShow();
	appPbOsd_AuxNote_LockProtect(TRUE, TRUE);
	appPbOsd_AuxNote_TrashCanDelet(TRUE, FALSE);
}

/*-------------------------------------------------------------------------
 *  File Name : MPLAY_DeleteAll_DlgConfig
 *------------------------------------------------------------------------*/
static
void
MPLAY_DeleteAll_DlgConfig(
	void
)
{

	appPlayOsd_DlgConfirmYesNoConfig(ID_STR_DELETE_ALL_FILE_, 0);
}

/*-------------------------------------------------------------------------
 *  File Name : MPLAY_LockAll_DlgShow
 *------------------------------------------------------------------------*/
static
void
MPLAY_DeleteAll_DlgShow(
	void
)
{

	appPlayOsd_DlgBoxShow();
	appPbOsd_AuxNote_OKSelect(TRUE);
}


/*-------------------------------------------------------------------------
 *  File Name : MPLAY_DeleteMain_DlgConfig
 *------------------------------------------------------------------------*/
static UINT32 dlgStrID_SelectAllBack[3] = {ID_STR_SELECT, ID_STR_ALL, ID_STR_BACK};
static
void
MPLAY_DeleteMain_DlgConfig(
	void
)
{
	//PB_VERBOSE("%s \n", __FUNCTION__);

	appPlayOsd_DlgBoxConfig(ID_STR_DELETE, 3, dlgStrID_SelectAllBack, 0, 0);
}


/*-------------------------------------------------------------------------
 *  File Name : MPLAY_DeleteMain_DlgShow
 *------------------------------------------------------------------------*/
static
void
MPLAY_DeleteMain_DlgShow(
	void
)
{
	//PB_VERBOSE("%s \n", __FUNCTION__);

	appPlayOsd_DlgBoxShow();
	appPbOsd_AuxNote_LockProtect(TRUE, FALSE);
	appPbOsd_AuxNote_TrashCanDelet(TRUE, TRUE);
}

/*-------------------------------------------------------------------------
 *  File Name : mplayMainDisplayResume
 *------------------------------------------------------------------------*/
static
void
mplayMainDisplayResume(
	void
)
{
	PB_VERBOSE("%s \n", __FUNCTION__);

#if 0
	thumbOsdInfo_SelStatusReset();
	selTable_Reset();
	appOsdRefresh(DISABLE); /*-- Refresh OFF */
	PB_OSD_CLR;
	thumbOsdInfo_DispUpdate(THUMB_OSD_UPDATE_ALL, MPLAY_THUMB_ICON_ALL);
	appPbOsd_AuxNote_OKDispPhoto(TRUE);
	appOsdRefresh(ENABLE); /*-- Refresh ON */
#else
	selTable_Reset();
	thumbOsdInfo_Reset();
	thumbOsdInfo_Construct();
	appOsdRefresh(DISABLE); /*-- Refresh OFF */
	PB_OSD_CLR;
	thumbOsdInfo_DispUpdate(THUMB_OSD_UPDATE_ALL, MPLAY_THUMB_ICON_ALL);
	appPbOsd_AuxNote_OKDispPhoto(TRUE);
	appOsdRefresh(ENABLE); /*-- Refresh ON */
#endif
	mplayUIStat = MPLAY_UI_STAT_STANDBY;
}
/*-------------------------------------------------------------------------
 *  File Name : exeProc_Reset
 *------------------------------------------------------------------------*/
static
void
exeProc_Reset(
	void
)
{
	PB_VERBOSE("%s \n", __FUNCTION__);

	#if HOST_DBG
	selTable_DebugDump();
	#endif

	exeProcIdx=0;
}
/*-------------------------------------------------------------------------
 *  File Name : exeProc_LockExecute
 *------------------------------------------------------------------------*/
static
void
exeProc_LockExecute(
	void
)
{
	UINT32 dcfIdx;
	sp5kDcfAttrElm_t fileInfoAtr;
	UINT8 szName[32];

	PB_VERBOSE("%s \n", __FUNCTION__);

	do 
	{
		dcfIdx = mplaySelTbl.slot[exeProcIdx];
		exeProcIdx++;
		if (dcfIdx==0) 
		{
			continue;
		} 
		else 
		{
			sp5kDcfFsFileAttrGet(dcfIdx, &fileInfoAtr, szName);
			sp5kDcfFsCurFileIdxSet(dcfIdx);
			if (fileInfoAtr.attribute==SP5K_DCF_FILE_ATTR_LOCK) 
			{
				/*-- file was locked, UNLOCK this one !!*/
				PB_VERBOSE("\n>>> Going UNLOCK dcfIdx(%d) !! \n\n",  dcfIdx);
				sp5kDcfFsFileOperate(SP5K_DCF_FILEOP_UNLOCK, 0, PB_OP_SYNC, 0, 0);
			} 
			else 
			{
				/*-- file was not locked, LOCK this one !!*/
				PB_VERBOSE("\n>>> Going LOCK dcfIdx(%d) !! \n\n",  dcfIdx);
				sp5kDcfFsFileOperate(SP5K_DCF_FILEOP_LOCK, 0, PB_OP_SYNC, 0, 0);
			}
			break;
		}
	} while (exeProcIdx<MPLAY_SELECT_MAX);

	if (MPLAY_SELECT_MAX>exeProcIdx) 
	{
		PB_VERBOSE("Sending : APP_UI_MSG_PB_MPLAY_EXECUTE(%d) \n", MPLAY_MAIN_MENU_SEL_LOCK);
		sp5kHostMsgSend(APP_UI_MSG_PB_MPLAY_EXECUTE, MPLAY_MAIN_MENU_SEL_LOCK);
	} 
	else 
	{
		PB_VERBOSE("Sending : APP_UI_MSG_PB_MPLAY_EXECUTE_DONE \n");
		sp5kHostMsgSend(APP_UI_MSG_PB_MPLAY_EXECUTE_DONE, 0);
	}
}
/*-------------------------------------------------------------------------
 *  File Name : exeProc_DeleteExecute
 *------------------------------------------------------------------------*/
static
void
exeProc_DeleteExecute(
	void
)
{
	UINT32 dcfIdx;

	PB_VERBOSE("%s \n", __FUNCTION__);

	do {
		dcfIdx = mplaySelTbl.slot[exeProcIdx];
		exeProcIdx++;
		if (dcfIdx==0) 
		{
			continue;
		}
		else 
		{
			sp5kDcfFsCurFileIdxSet(dcfIdx);
			sp5kDcfFsFileOperate(SP5K_DCF_FILEOP_DEL, 0, PB_OP_SYNC, 0, 0);
			appDpofJobDelete(APP_DPOF_MODE_STD, APP_DPOF_JOB_CURR);
			gpbImgCb.isDpofSet=FALSE;
			gpbImgCb.dpofPrintNum = 0;
			gpbImgCb.isDpofStamp = FALSE;
			//dpofDebugDump(1/*isQryDpof*/);
			break;
		}
	} while (exeProcIdx<MPLAY_SELECT_MAX);

	if (MPLAY_SELECT_MAX>exeProcIdx) 
	{
		PB_VERBOSE("Sending : APP_UI_MSG_PB_MPLAY_EXECUTE(%d) \n", MPLAY_MAIN_MENU_SEL_DEL);
		sp5kHostMsgSend(APP_UI_MSG_PB_MPLAY_EXECUTE, MPLAY_MAIN_MENU_SEL_DEL);
	} 
	else
	{
		PB_VERBOSE("Sending : APP_UI_MSG_PB_MPLAY_EXECUTE_DONE \n");
		sp5kHostMsgSend(APP_UI_MSG_PB_MPLAY_EXECUTE_DONE, 0);
	}
}

/*-------------------------------------------------------------------------
 *  File Name : allProc_Reset
 *------------------------------------------------------------------------*/
static
void
allProc_Reset(
	void
)
{
	PB_VERBOSE("%s \n", __FUNCTION__);

	exeProcIdx=1;
}
/*-------------------------------------------------------------------------
 *  File Name : allProc_LockExecute
 *------------------------------------------------------------------------*/
static
void
allProc_LockExecute(
	UINT32 isToLock
)
{
	PB_VERBOSE("%s \n", __FUNCTION__);

	sp5kDcfFsCurFileIdxSet(exeProcIdx);
	if (isToLock) 
	{
		sp5kDcfFsFileOperate(SP5K_DCF_FILEOP_LOCK, 0, PB_OP_SYNC, 0, 0);
	}
	else 
	{
		sp5kDcfFsFileOperate(SP5K_DCF_FILEOP_UNLOCK, 0, PB_OP_SYNC, 0, 0);
	}
	gcurrDcfIdx = exeProcIdx;

	exeProcIdx++;
	if (exeProcIdx <= gtotalImgCount) 
	{
		if (isToLock) 
		{
			PB_VERBOSE("Sending : APP_UI_MSG_PB_MPLAY_EXECUTE_ALL(%d) \n", MPLAY_EXE_ALL_LOCK);
			sp5kHostMsgSend(APP_UI_MSG_PB_MPLAY_EXECUTE_ALL, MPLAY_EXE_ALL_LOCK);
		} 
		else 
		{
			PB_VERBOSE("Sending : APP_UI_MSG_PB_MPLAY_EXECUTE_ALL(%d) \n", MPLAY_EXE_ALL_UNLOCK);
			sp5kHostMsgSend(APP_UI_MSG_PB_MPLAY_EXECUTE_ALL, MPLAY_EXE_ALL_UNLOCK);
		}
	} 
	else 
	{
		PB_VERBOSE("Sending : APP_UI_MSG_PB_MPLAY_EXECUTE_DONE \n");
		sp5kHostMsgSend(APP_UI_MSG_PB_MPLAY_EXECUTE_DONE, 0);
	}
}
/*-------------------------------------------------------------------------
 *  File Name : allProc_DeleteExecute
 *------------------------------------------------------------------------*/
static
void
allProc_DeleteExecute(
	void
)
{
	UINT32 dcfIdx;
	sp5kDcfAttrElm_t fileInfoAtr;
	UINT8 szName[32];
	BOOL isLocked;

	PB_VERBOSE("%s \n", __FUNCTION__);

	/*-- Search the dcfIdx where file is not protected */
	do 
	{
		dcfIdx = exeProcIdx;
		sp5kDcfFsFileAttrGet(dcfIdx, &fileInfoAtr, szName);
		PB_DBG("\nexeProcIdx(%d), gtotalImgCount(%d) \n\n", exeProcIdx, gtotalImgCount);
		if (fileInfoAtr.attribute==SP5K_DCF_FILE_ATTR_LOCK) 
		{
			/*-- this dcfIdx is locked, advance to next index */
			isLocked = TRUE;
			exeProcIdx++;
			continue;
		} 
		else
		{
			/*-- the file indexed is not protected, go & delete it */
			isLocked = FALSE;
			sp5kDcfFsCurFileIdxSet(dcfIdx);
			sp5kDcfFsFileOperate(SP5K_DCF_FILEOP_DEL, 0, PB_OP_SYNC, 0, 0);
			appDpofJobDelete(APP_DPOF_MODE_STD, APP_DPOF_JOB_CURR);
			gpbImgCb.isDpofSet=FALSE;
			gpbImgCb.dpofPrintNum = 0;
			gpbImgCb.isDpofStamp = FALSE;
			//dpofDebugDump(1/*isQryDpof*/);
			gtotalImgCount--;
			break;
		}
	} while (exeProcIdx<gtotalImgCount);

	PB_DBG("\n>>exeProcIdx(%d), gtotalImgCount(%d) \n\n", exeProcIdx, gtotalImgCount);

	if (gtotalImgCount>=exeProcIdx && isLocked==FALSE) 
	{
		PB_VERBOSE("Sending : APP_UI_MSG_PB_MPLAY_EXECUTE_ALL(%d) \n", MPLAY_EXE_ALL_DEL);
		sp5kHostMsgSend(APP_UI_MSG_PB_MPLAY_EXECUTE_ALL, MPLAY_EXE_ALL_DEL);
	} 
	else 
	{
		PB_VERBOSE("Sending : APP_UI_MSG_PB_MPLAY_EXECUTE_DONE \n");
		sp5kHostMsgSend(APP_UI_MSG_PB_MPLAY_EXECUTE_DONE, 0);
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appMPlayShiftOne
 *------------------------------------------------------------------------*/
static
void
appMPlayShiftOne(
	UINT8 navCtrl,
	BOOL isKeyDebounce
)
{
	UINT32 curThumbIdx;

	if (gpbMainCb.subStat!=PB_MPLAY_STAT_STANDBY) 
	{
		PB_DBG("MPLAY: (%d) busy !!\n", navCtrl);
		return;
	}
	PB_BEEP;
	curThumbIdx = THUMB_INDEX(gcurrDcfIdx);
	switch (navCtrl) 
	{
		case PB_MPLAY_RIGHT:
			sp5kMultiPlayShift(SP5K_MPB_SHIFT_DOWN, 1);
			gpbMainCb.subStat = PB_MPLAY_STAT_BUSY;
			mplayShift = PB_MPLAY_RIGHT;
			break;
		case PB_MPLAY_LEFT:
			sp5kMultiPlayShift(SP5K_MPB_SHIFT_UP, 1);
			gpbMainCb.subStat = PB_MPLAY_STAT_BUSY;
			mplayShift = PB_MPLAY_LEFT;
			break;
		default:
			PB_DBG("MPLAY: undefined navCtrl(%d) \n", navCtrl);
			break;
	}

	isDoKeyDebounce = (!!isKeyDebounce) ? TRUE : FALSE;

}

/*-------------------------------------------------------------------------
 *  File Name : appMPlayShiftDown
 *------------------------------------------------------------------------*/
static
void
appMPlayShiftDown(
	BOOL isKeyDebounce
)
{
	UINT32 totalImg, curDcfIdx;

	if (gpbMainCb.subStat!=PB_MPLAY_STAT_STANDBY) 
	{
		PB_DBG("MPLAY: busy (down) !!\n");
		return;
	}

	PB_BEEP;

	//mplayUIKeyControl(MPLAY_SXN_START);
	totalImg = gtotalImgCount;
	curDcfIdx = gcurrDcfIdx;

	UINT32 curThumbIdx;
	curThumbIdx = THUMB_INDEX(curDcfIdx);
	if (totalImg>9) 
	{
		if (curThumbIdx+3>9 || curDcfIdx+3>totalImg) 
		{
			/*-- Going to change page, clear current icon !! */
			thumbOsdInfo_DispClear(THUMB_OSD_UPDATE_ALL, MPLAY_THUMB_ICON_ALL);
		}
	}

	if( totalImg <= 9 )
	{
		if(( curDcfIdx + 3 ) > totalImg )
		{
			if(( curDcfIdx > 3 ) && ( curDcfIdx <= 6)) 
			{
				PB_DBG("8 shift [%d]\n",3);
				sp5kMultiPlayShift(SP5K_MPB_SHIFT_UP, 3);
			}
			else if( curDcfIdx > 6 ) 
			{
				PB_DBG("9 shift [%d]\n",6);
				sp5kMultiPlayShift(SP5K_MPB_SHIFT_UP, 6);
			}
			else
			{
				PB_DBG("10 shift [%d]\n", 0);
				sp5kMultiPlayShift(SP5K_MPB_SHIFT_DOWN, 0);
			}
		}
		else
		{
			PB_DBG("11 shift [%d]\n",3);
			sp5kMultiPlayShift(SP5K_MPB_SHIFT_DOWN, 3);
		}
	}
	else
	{
		if( curDcfIdx + 3 <= totalImg ) 
		{
			PB_DBG("30 shift [%d]\n",3);
			sp5kMultiPlayShift(SP5K_MPB_SHIFT_DOWN,3);
		}
		else
		{
			if( totalImg % 9 == 1 )
			{
				if( totalImg > curDcfIdx )
				{
					PB_DBG("38 shift [%d]\n",totalImg - curDcfIdx);
					sp5kMultiPlayShift(SP5K_MPB_SHIFT_DOWN,totalImg - curDcfIdx);
				}
				else
				{
					PB_DBG("39 shift [%d]\n",1);
					sp5kMultiPlayShift(SP5K_MPB_SHIFT_DOWN,1);
				}
			}
			else if(( totalImg % 9 == 2 ) &&
				(totalImg == curDcfIdx + 2)) 
			{
				PB_DBG("50 shift [%d]\n",2);
				sp5kMultiPlayShift(SP5K_MPB_SHIFT_DOWN,2);
			}
			else if(( totalImg % 3 ) == 0 ) 
			{
				PB_DBG("31 shift [%d]\n",3);
				sp5kMultiPlayShift(SP5K_MPB_SHIFT_DOWN,3);
			}
			else if(( totalImg % 3 ) == 1 ) 
			{
				if( curDcfIdx + 2 == totalImg ) 
				{
					PB_DBG("32 shift [%d]\n",4);
					sp5kMultiPlayShift(SP5K_MPB_SHIFT_DOWN,4);
				}
				else if( curDcfIdx + 1 == totalImg ) 
				{
					PB_DBG("33 shift [%d]\n",4);
					sp5kMultiPlayShift(SP5K_MPB_SHIFT_DOWN,4);
				}
				else
				{
					PB_DBG("34 shift [%d]\n",1);
					sp5kMultiPlayShift(SP5K_MPB_SHIFT_DOWN,1);
				}
			}
			else if(( totalImg % 3 ) == 2 ) 
			{
				if( curDcfIdx + 2 == totalImg ) 
				{
					PB_DBG("35 shift [%d]\n",5);
					sp5kMultiPlayShift(SP5K_MPB_SHIFT_DOWN,5);
				}
				else if( curDcfIdx + 1 == totalImg ) 
				{
					PB_DBG("36 shift [%d]\n",2);
					sp5kMultiPlayShift(SP5K_MPB_SHIFT_DOWN,2);
				}
				else
				{
					PB_DBG("37 shift [%d]\n",2);
					sp5kMultiPlayShift(SP5K_MPB_SHIFT_DOWN,2);
				}
			}
		}
	}

	gpbMainCb.subStat = PB_MPLAY_STAT_BUSY;
	mplayShift = PB_MPLAY_DOWN;

	isDoKeyDebounce = (!!isKeyDebounce) ? TRUE : FALSE;

	//mplayUIKeyControl(MPLAY_SXN_END);

}

/*-------------------------------------------------------------------------
 *  File Name : appMPlayShiftUp
 *------------------------------------------------------------------------*/
static
void
appMPlayShiftUp(
	BOOL isKeyDebounce
)
{
	UINT32 totalImg, curDcfIdx;

	if (gpbMainCb.subStat!=PB_MPLAY_STAT_STANDBY) 
	{
		PB_DBG("MPLAY: busy (up) !!\n");
		return;
	}

	PB_BEEP;

	//mplayUIKeyControl(MPLAY_SXN_START);
	totalImg = gtotalImgCount;
	curDcfIdx = gcurrDcfIdx;

	UINT32 curThumbIdx;
	curThumbIdx = THUMB_INDEX(curDcfIdx);
	if (curThumbIdx<=3 && totalImg>9) 
	{
		/*-- Going to change page, clear current icon !! */
		thumbOsdInfo_DispClear(THUMB_OSD_UPDATE_ALL, MPLAY_THUMB_ICON_ALL);
	}

	if( totalImg <= 9 ) 
	{
		if( totalImg <= 3 )
		{
			if(( totalImg + 6 ) <= totalImg ) 
			{
				PB_DBG("1 shift [%d]\n",6);
				sp5kMultiPlayShift(SP5K_MPB_SHIFT_DOWN, 6);
			}
			else if(( curDcfIdx + 3 ) <= totalImg) 
			{
				PB_DBG("2 shift [%d]\n", 3);
				sp5kMultiPlayShift(SP5K_MPB_SHIFT_DOWN, 3);
			}
			else
			{
				PB_DBG("3 shift [%d]\n", 0);
				sp5kMultiPlayShift(SP5K_MPB_SHIFT_UP, 0);
			}
		}
		else if(( curDcfIdx > 3 ) && ( curDcfIdx <= 6 )) 
		{
				PB_DBG("4 shift [%d]\n",3);
				sp5kMultiPlayShift(SP5K_MPB_SHIFT_UP,3);
		}
		else
		{
			PB_DBG("5 shift [%d]\n",3);
			sp5kMultiPlayShift(SP5K_MPB_SHIFT_UP, 3);
		}
	}
	else
	{
		if( curDcfIdx > 3 ) 
		{
			PB_DBG("40 shift [%d]\n",3);
			sp5kMultiPlayShift(SP5K_MPB_SHIFT_UP, 3);
		}
		else
		{
			if( totalImg % 9 == 1 ) 
			{
				PB_DBG("49 shift [%d]\n", curDcfIdx);
				sp5kMultiPlayShift(SP5K_MPB_SHIFT_UP, curDcfIdx);
			}
			else if(( totalImg % 9 == 2 ) && (curDcfIdx == 3)) 
			{
				PB_DBG("41 shift [%d]\n",3);
				sp5kMultiPlayShift(SP5K_MPB_SHIFT_UP,3);
			}
			else if(( totalImg % 3 ) == 0 ) 
			{
				PB_DBG("42 shift [%d]\n",3);
				sp5kMultiPlayShift(SP5K_MPB_SHIFT_UP,3);
			}
			else if(( totalImg % 3 ) == 1 ) 
			{
				if( curDcfIdx == 3 )
				{
					PB_DBG("43 shift [%d]\n",4);
					sp5kMultiPlayShift(SP5K_MPB_SHIFT_UP,4);
				}
				else if( curDcfIdx == 2 ) 
				{
					PB_DBG("44 shift [%d]\n",4);
					sp5kMultiPlayShift(SP5K_MPB_SHIFT_UP,4);
				}
				else
				{
					PB_DBG("45 shift [%d]\n",1);
					sp5kMultiPlayShift(SP5K_MPB_SHIFT_UP,1);
				}
			}
			else if(( totalImg % 3 ) == 2 )
			{
				if( curDcfIdx == 3 )
				{
					PB_DBG("46 shift [%d]\n",5);
					sp5kMultiPlayShift(SP5K_MPB_SHIFT_UP,5);
				}
				else if( curDcfIdx == 2 )
				{
					PB_DBG("47 shift [%d]\n",2);
					sp5kMultiPlayShift(SP5K_MPB_SHIFT_UP,2);
				}
				else
				{
					PB_DBG("48 shift [%d]\n",2);
					sp5kMultiPlayShift(SP5K_MPB_SHIFT_UP,2);
				}
			}
		}
	}

	gpbMainCb.subStat = PB_MPLAY_STAT_BUSY;
	mplayShift = PB_MPLAY_UP;

	isDoKeyDebounce = (!!isKeyDebounce) ? TRUE : FALSE;

	//mplayUIKeyControl(MPLAY_SXN_END);

}



/*-------------------------------------------------------------------------
 *  File Name : mplayKey_SET_Service
 *------------------------------------------------------------------------*/
static
void
mplayKey_SET_Service(
	void
)
{
	UINT8 dlgSel;
	UINT32 thumbIdx;

	PB_VERBOSE("%s: uiStat(%d) \n", __FUNCTION__, mplayUIStat);

	PB_BEEP;

	dlgSel = appPlayOsd_DlgBoxSelectGet();

	switch (mplayUIStat) {
	default:
		break;
	case MPLAY_UI_STAT_STANDBY:
		/*-- display photo */
		appPbMPlayStExit(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
		break;
	case MPLAY_UI_STAT_PAN_SET:
		/*-- set attribute !!
		 */
		thumbIdx = THUMB_INDEX(gcurrDcfIdx);
		/*-- check if going to delete file which is protected !? */
		if (mplayMainMenuSel==MPLAY_MAIN_MENU_SEL_DEL &&
			thumbInfo.fattrLock[thumbIdx]==TRUE)
		{
			thumbOsd_DeleteLockedWarning(thumbIdx);
			break;
		}
		/*-- Toggle selStatus */
		thumbInfo.selStatus[thumbIdx] = !thumbInfo.selStatus[thumbIdx];
		/*-- Update ICON */
		dlgSel = (mplayMainMenuSel==MPLAY_MAIN_MENU_SEL_LOCK) ? MPLAY_THUMB_ICON_LOCK : MPLAY_THUMB_ICON_DELETE;
		thumbOsdInfo_DispUpdate(thumbIdx, (UINT32)dlgSel);
		/*-- update selTable */
		if (thumbInfo.selStatus[thumbIdx]) 
		{
			/*-- status changed, Add into SelTable!! */
			if (FAIL==selTable_Add(thumbInfo.dcfIdx[thumbIdx])) 
			{
				PB_LOG("\n!! FATAL !!, Out of resource @ %s, %d \n\n", __FUNCTION__, __LINE__);
			}
		} 
		else 
		{
			/*-- unselected, Remove from SelTable !! */
			selTable_Remove(thumbInfo.dcfIdx[thumbIdx]);
		}
		break;
	case MPLAY_UI_STAT_LOCK_CONFIRM:
		if (dlgSel==0) 
		{
			/*-- YES */
			/*-- Clear UI */
			appPbMsg_BusyWait();
			/*-- Go on execution */
			gpbMainCb.lastImgIdx=gcurrDcfIdx; /* backup current IDX */
			gpbMainCb.subStat=PB_MPLAY_STAT_EXE_BUSY;
			exeProc_Reset();
			mplayUIKeyControl(MPLAY_SXN_EXE_START); /* END @ StClose or MSG:EXE_DONE */
			sp5kHostMsgSend(APP_UI_MSG_PB_MPLAY_EXECUTE, MPLAY_MAIN_MENU_SEL_LOCK);
		} 
		else 
		{
			/*-- NO */
			appOsdRefresh(DISABLE); /*-- Refresh OFF */
			PB_OSD_CLR;
			thumbOsdInfo_DispUpdate(THUMB_OSD_UPDATE_ALL, MPLAY_THUMB_ICON_ALL);
			appPbOsd_AuxNote_OKSelect(TRUE);
			appPbOsd_AuxNote_MenuBack(TRUE);
			appOsdRefresh(ENABLE); /*-- Refresh ON */
			mplayUIStat = MPLAY_UI_STAT_PAN_SET;
		}
		break;
	case MPLAY_UI_STAT_DELETE_CONFIRM:
		if (dlgSel==0) 
		{
			/*-- YES */
			/*-- Workaround for the ridiculous bug --
			 *   IMG Layer will become BLACK after FILEDEL operation !!
			 */
			/*-- Clear UI */
			PB_BLACK_BKGND;
			MPLAY_ScrollBar(FALSE, 0, 0, 0);
			/*-- BUSY Wait */
			appPbMsg_BusyWait();
			/*-- Go on execution */
			gpbMainCb.lastImgIdx=gcurrDcfIdx; /* backup current IDX */
			gpbMainCb.subStat=PB_MPLAY_STAT_EXE_BUSY;
			exeProc_Reset();
			mplayUIKeyControl(MPLAY_SXN_EXE_START); /* END @ StClose or MSG:EXE_DONE */
			sp5kHostMsgSend(APP_UI_MSG_PB_MPLAY_EXECUTE, MPLAY_MAIN_MENU_SEL_DEL);
		} 
		else
		{
			/*-- NO */
			appOsdRefresh(DISABLE); /*-- Refresh OFF */
			PB_OSD_CLR;
			thumbOsdInfo_DispUpdate(THUMB_OSD_UPDATE_ALL, MPLAY_THUMB_ICON_ALL);
			appPbOsd_AuxNote_OKSelect(TRUE);
			appPbOsd_AuxNote_MenuBack(TRUE);
			appOsdRefresh(ENABLE); /*-- Refresh ON */
			mplayUIStat = MPLAY_UI_STAT_PAN_SET;
		}
		break;
	case MPLAY_UI_STAT_LOCK_MAIN_MENU:
		PB_OSD_CLR;
		if (dlgSel==0) 
		{
			/*-- SELECT */
			thumbOsdInfo_DispUpdate(THUMB_OSD_UPDATE_ALL, MPLAY_THUMB_ICON_ALL);
			appPbOsd_AuxNote_OKSelect(TRUE);
			appPbOsd_AuxNote_MenuBack(TRUE);
			mplayUIStat = MPLAY_UI_STAT_PAN_SET;
		} 
		else if (dlgSel==1)
		{
			/*-- PROTECT_ALL */
			MPLAY_LockAll_DlgConfig();
			MPLAY_LockAll_DlgShow();
			mplayUIStat = MPLAY_UI_STAT_LOCK_ALL_CONFIRM;
		} 
		else if (dlgSel==2) 
		{
			/*-- UNPROTECT_ALL */
			MPLAY_UnLockAll_DlgConfig();
			MPLAY_UnLockAll_DlgShow();
			mplayUIStat = MPLAY_UI_STAT_UNLOCK_ALL_CONFIRM;
		}
		else 
		{
			/*-- BACK */
			mplayMainDisplayResume();
			mplayUIStat = MPLAY_UI_STAT_STANDBY;
		}
		break;
	case MPLAY_UI_STAT_DELETE_MAIN_MENU:
		PB_OSD_CLR;
		if (dlgSel==0) 
		{
			/*-- SELECT */
			//selTable_Reset();
			thumbOsdInfo_DispUpdate(THUMB_OSD_UPDATE_ALL, MPLAY_THUMB_ICON_ALL);
			appPbOsd_AuxNote_OKSelect(TRUE);
			appPbOsd_AuxNote_MenuBack(TRUE);
			mplayUIStat = MPLAY_UI_STAT_PAN_SET;
		} 
		else if (dlgSel==1) 
		{
			/*-- DELETE_ALL */
			MPLAY_DeleteAll_DlgConfig();
			MPLAY_DeleteAll_DlgShow();
			mplayUIStat = MPLAY_UI_STAT_DELETE_ALL_CONFIRM;
		} 
		else 
		{
			/*-- BACK */
			mplayMainDisplayResume();
			//mplayUIStat = MPLAY_UI_STAT_STANDBY;
		}
		break;
	case MPLAY_UI_STAT_LOCK_ALL_CONFIRM:
		if (dlgSel==0) 
		{
			/*-- YES */
			#if 0
			appPbMsg_BusyWait();
			sp5kDcfFsFileOperate(SP5K_DCF_FILEOP_LOCKALL, 0, PB_OP_SYNC, 0, 0);
			mplayMainDisplayResume();
			#else
			/*-- BUSY Wait */
			appPbMsg_BusyWait();
			/*-- Go on execution */
			gpbMainCb.lastImgIdx=gcurrDcfIdx; /* backup current IDX */
			gpbMainCb.subStat=PB_MPLAY_STAT_EXE_BUSY;
			allProc_Reset();
			mplayUIKeyControl(MPLAY_SXN_EXE_START); /* END @ StClose or MSG:EXE_DONE */
			sp5kHostMsgSend(APP_UI_MSG_PB_MPLAY_EXECUTE_ALL, MPLAY_EXE_ALL_LOCK);
			#endif
		} 
		else 
		{
			/*-- NO */
			PB_OSD_CLR;
			MPLAY_LockMain_DlgConfig();
			appPlayOsd_DlgBoxSelectSet(1 /* PROTECT_ALL */);
			MPLAY_LockMain_DlgShow();
			mplayUIStat = MPLAY_UI_STAT_LOCK_MAIN_MENU;
		}
		break;
	case MPLAY_UI_STAT_UNLOCK_ALL_CONFIRM:
		if (dlgSel==0) 
		{
			/*-- YES */
			#if 0
			appPbMsg_BusyWait();
			sp5kDcfFsFileOperate(SP5K_DCF_FILEOP_UNLOCKALL, 0, PB_OP_SYNC, 0, 0);
			PB_OSD_CLR;
			mplayMainDisplayResume();
			#else
			/*-- BUSY Wait */
			appPbMsg_BusyWait();
			/*-- Go on execution */
			gpbMainCb.lastImgIdx=gcurrDcfIdx; /* backup current IDX */
			gpbMainCb.subStat=PB_MPLAY_STAT_EXE_BUSY;
			allProc_Reset();
			mplayUIKeyControl(MPLAY_SXN_EXE_START); /* END @ StClose or MSG:EXE_DONE */
			sp5kHostMsgSend(APP_UI_MSG_PB_MPLAY_EXECUTE_ALL, MPLAY_EXE_ALL_UNLOCK);
			#endif
		} 
		else 
		{
			/*-- NO */
			PB_OSD_CLR;
			MPLAY_LockMain_DlgConfig();
			appPlayOsd_DlgBoxSelectSet(2 /* UNPROTECT_ALL */);
			MPLAY_LockMain_DlgShow();
			mplayUIStat = MPLAY_UI_STAT_LOCK_MAIN_MENU;
		}
		break;
	case MPLAY_UI_STAT_DELETE_ALL_CONFIRM:
		if (dlgSel==0) 
		{
			/*-- YES */
			/*-- BUSY Wait */
			appPbMsg_BusyWait();
			/*-- Go on execution */
			gpbMainCb.lastImgIdx=gcurrDcfIdx; /* backup current IDX */
			gpbMainCb.subStat=PB_MPLAY_STAT_EXE_BUSY;
			allProc_Reset();
			mplayUIKeyControl(MPLAY_SXN_EXE_START); /* END @ StClose or MSG:EXE_DONE */
			sp5kHostMsgSend(APP_UI_MSG_PB_MPLAY_EXECUTE_ALL, MPLAY_EXE_ALL_DEL);
		} 
		else 
		{
			/*-- NO */
			PB_OSD_CLR;
			MPLAY_DeleteMain_DlgConfig();
			appPlayOsd_DlgBoxSelectSet(1 /* DELETE_ALL */);
			MPLAY_DeleteMain_DlgShow();
			mplayUIStat = MPLAY_UI_STAT_DELETE_MAIN_MENU;
		}
		break;
	}
}

/*-------------------------------------------------------------------------
 *  File Name : mplayKey_MENU_Service
 *------------------------------------------------------------------------*/
static
void
mplayKey_MENU_Service(
	void
)
{
	PB_BEEP;

	PB_VERBOSE("%s: uiStat(%d) \n", __FUNCTION__, mplayUIStat);

	switch (mplayUIStat) {
	default:
		break;
	case MPLAY_UI_STAT_STANDBY:
		/*-- Check CARD Locked */
		if (IS_CARD_LOCK && IS_CARD_EXIST) 
		{
			gpbMainCb.err = PB_ERR_CARD_LOCKED;
			appPbErrorProcess(gpbMainCb.err);
			mplayMainDisplayResume();
			break;
		}

		PB_OSD_CLR;
		selTable_Reset();
		if (mplayMainMenuSel==MPLAY_MAIN_MENU_SEL_LOCK)
		{
			MPLAY_LockMain_DlgConfig();
			MPLAY_LockMain_DlgShow();
			mplayUIStat=MPLAY_UI_STAT_LOCK_MAIN_MENU;
		} 
		else 
		{
			MPLAY_DeleteMain_DlgConfig();
			MPLAY_DeleteMain_DlgShow();
			mplayUIStat=MPLAY_UI_STAT_DELETE_MAIN_MENU;
		}
		break;
	case MPLAY_UI_STAT_LOCK_MAIN_MENU:
	case MPLAY_UI_STAT_LOCK_ALL_CONFIRM:
	case MPLAY_UI_STAT_UNLOCK_ALL_CONFIRM:
	case MPLAY_UI_STAT_DELETE_MAIN_MENU:
	case MPLAY_UI_STAT_DELETE_ALL_CONFIRM:
		mplayMainDisplayResume();
		break;
	case MPLAY_UI_STAT_LOCK_CONFIRM:
		mplayMainDisplayResume();
		break;
	case MPLAY_UI_STAT_DELETE_CONFIRM:
		mplayMainDisplayResume();
		break;
	case MPLAY_UI_STAT_PAN_SET:
		appPbOsd_AuxNote_OKSelect(FALSE);
		appPbOsd_AuxNote_MenuBack(FALSE);
		if (selTable_UsedNumGet()==0)
		{
			/*-- None is selected */
			//mplayMainDisplayResume();
			appPbOsd_AuxNote_OKDispPhoto(TRUE);
			mplayUIStat = MPLAY_UI_STAT_STANDBY;
		}
		else 
		{
			/*-- Status changed */
			if (mplayMainMenuSel==MPLAY_MAIN_MENU_SEL_LOCK)
			{
				MPLAY_LockConfirm_DlgConfig();
				MPLAY_LockConfirm_DlgShow();
				mplayUIStat = MPLAY_UI_STAT_LOCK_CONFIRM;
			}
			else 
			{
				MPLAY_DeleteConfirm_DlgConfig();
				MPLAY_DeleteConfirm_DlgShow();
				mplayUIStat = MPLAY_UI_STAT_DELETE_CONFIRM;
			}
		}
		break;
	}
}

/*-------------------------------------------------------------------------
 *  File Name : mplayKey_UP_DOWN_Service
 *------------------------------------------------------------------------*/
static
void
mplayKey_UP_DOWN_Service(
	BOOL isUp,
	BOOL isKeyDebounce
)
{
	PB_VERBOSE("%s: uiStat(%d) \n", __FUNCTION__, mplayUIStat);

	if (mplayUIStat==MPLAY_UI_STAT_STANDBY || mplayUIStat==MPLAY_UI_STAT_PAN_SET) 
	{
		/*-- shifting images */
		if (isUp) 
		{
		
			appMPlayShiftUp(isKeyDebounce);
		} 
		else 
		{
			appMPlayShiftDown(isKeyDebounce);
		}
	} 
	else 
	{
		/*-- dialog scroll */
		PB_BEEP;
		appPlayOsd_DlgBoxScroll(isUp);
	}
}

/*-------------------------------------------------------------------------
 *  File Name : mplayKey_LEFT_RIGHT_Service
 *------------------------------------------------------------------------*/
static
void
mplayKey_LEFT_RIGHT_Service(
	BOOL isLeft,
	BOOL isKeyDebounce
)
{
	PB_VERBOSE("%s: uiStat(%d) \n", __FUNCTION__, mplayUIStat);

	if (mplayUIStat==MPLAY_UI_STAT_STANDBY || mplayUIStat==MPLAY_UI_STAT_PAN_SET) 
	{
		//PB_BEEP;
		/*-- BEEP in appMPlayShiftOne */
		appMPlayShiftOne( (isLeft ? PB_MPLAY_LEFT : PB_MPLAY_RIGHT), isKeyDebounce);
	} 
	else if (mplayUIStat==MPLAY_UI_STAT_DELETE_MAIN_MENU) 
	{
		PB_BEEP;
		appPlayOsd_DlgBoxClear(); //PB_OSD_CLR;
		MPLAY_LockMain_DlgConfig();
		MPLAY_LockMain_DlgShow();
		mplayUIStat = MPLAY_UI_STAT_LOCK_MAIN_MENU;
		mplayMainMenuSel = MPLAY_MAIN_MENU_SEL_LOCK;
	}  
	else if (mplayUIStat==MPLAY_UI_STAT_LOCK_MAIN_MENU) 
	{
		PB_BEEP;
		appPlayOsd_DlgBoxClear(); //PB_OSD_CLR;
		MPLAY_DeleteMain_DlgConfig();
		MPLAY_DeleteMain_DlgShow();
		mplayUIStat = MPLAY_UI_STAT_DELETE_MAIN_MENU;
		mplayMainMenuSel = MPLAY_MAIN_MENU_SEL_DEL;
	} 
	else 
	{
		/* Do Nothing */
	}
}
/*-------------------------------------------------------------------------
 *  File Name : appPbMPlayStClose
 *------------------------------------------------------------------------*/
static
void
appPbMPlayStClose(
	void
)
{
	PB_DBG("MPlay: State closing \n");

	if (gpbMainCb.subStat==PB_MPLAY_STAT_BUSY) {
		PB_DBG("MPLAY: Abort to close !!\n");
		sp5kMultiPlayAbort();
	}

	if (gpbMainCb.subStat==PB_MPLAY_STAT_EXE_BUSY) {
		mplayUIKeyControl(MPLAY_SXN_EXE_END);
		gtotalImgCount = appPbTotalFileCountGet();
		if (gpbMainCb.lastImgIdx>gtotalImgCount) {
			gpbMainCb.lastImgIdx =gcurrDcfIdx = gtotalImgCount;
		}
	}

	gpbMainCb.subStat = PB_MPLAY_STAT_NULL;
	MPLAY_ScrollBar(FALSE, 0, 0, 0);
}

/*-------------------------------------------------------------------------
 *  File Name : appPbMPlayStExit
 *------------------------------------------------------------------------*/
static
void
appPbMPlayStExit(
	UINT32 nextSt,
	UINT32 stMsg
)
{
	PB_DBG("MPlay: State exiting \n");
	appPbMPlayStClose();
	appStateChange(nextSt, stMsg);
}

/*-------------------------------------------------------------------------
 *  File Name : appPbMPlayStInit
 *------------------------------------------------------------------------*/
static
UINT32
appPbMPlayStInit(
	void
)
{
	UINT32 firstFileIdx;

	PB_VERBOSE("MPlay: State initing \n");

	sp5kDcfFsCurFileIdxSet(gpbMainCb.curImgIdx);

	/*-- Config Layout */
	#if (KIT_WITH_HDMI)
	if (IS_HDMI_IN) 
	{
		mplayNormLayout.valueM = 3;
		mplayNormLayout.valueN = 3;
		mplayNormLayout.leftMargin = MPLAY_LAYOUT_LEFTMARGIN;
		mplayNormLayout.gapX = 20;
		mplayNormLayout.rightMargin = MPLAY_LAYOUT_RIGHTMARGIN;
		mplayNormLayout.topMargin = 100;
		mplayNormLayout.gapY = 20;
		mplayNormLayout.bottomMargin = 100;
		
		mplayEnlargeWindow.offX= -12;
		mplayEnlargeWindow.offY= -9;
		mplayEnlargeWindow.deltaW= 32;
		mplayEnlargeWindow.deltaH= 24;
		#if SPCA6330
		mplayFrameSize.frmW = 640;
		mplayFrameSize.frmH = 480;
		mplayFrameSize.LargeFrmMode = 1;
		#endif
	} 
	else if(IS_PB_DISP_LCD)
	{
	#else
	if(IS_PB_DISP_LCD)
	{
	#endif
		mplayNormLayout.valueM = 3;
		mplayNormLayout.valueN = 3;
		mplayNormLayout.leftMargin = MPLAY_LAYOUT_LEFTMARGIN;
		//mplayNormLayout.gapX = MPLAY_LAYOUT_GAP_X;
		//mplayNormLayout.rightMargin = 30;
		mplayNormLayout.topMargin = MPLAY_LAYOUT_TOPMARGIN;
		mplayNormLayout.gapY = MPLAY_LAYOUT_GAP_Y;
		mplayNormLayout.bottomMargin = MPLAY_LAYOUT_BOTTOMMARGIN;
		
		mplayEnlargeWindow.offX= -12;
		mplayEnlargeWindow.offY= -9;
		mplayEnlargeWindow.deltaW= 32;
		mplayEnlargeWindow.deltaH= 24;

		if(IS_TV_IN){
			mplayNormLayout.gapX = 20;
		}else{
			mplayNormLayout.gapX = MPLAY_LAYOUT_GAP_X;
		}
		
		if(IS_TV_IN){
			mplayNormLayout.rightMargin = 80;
		}
		else{
			mplayNormLayout.rightMargin = 30;
		}
		
	}
	else 
	{	/*-- Setup Layout for TV */
		if (IS_PB_TV_PAL) 
		{
			mplayNormLayout.valueM = 3;
			mplayNormLayout.valueN = 3;
			mplayNormLayout.leftMargin = 72;
			mplayNormLayout.gapX = 10;
			mplayNormLayout.rightMargin = 60;
			mplayNormLayout.topMargin = 46 -3;
			mplayNormLayout.gapY = 6;
			mplayNormLayout.bottomMargin = 98 +3;
			 
			mplayEnlargeWindow.offX= -12;
			mplayEnlargeWindow.offY= -9;
			mplayEnlargeWindow.deltaW= 32;
			mplayEnlargeWindow.deltaH= 24;
			#if 0
			if KIT_WITH_HDMI
			mplayFrameSize.LargeFrmMode = 0;
			#endif
		}
		else 
		{
			mplayNormLayout.valueM = 3;
			mplayNormLayout.valueN = 3;
			mplayNormLayout.leftMargin = 72;
			mplayNormLayout.gapX = 10;
			mplayNormLayout.rightMargin = 46;
			mplayNormLayout.topMargin = 34 -3;
			mplayNormLayout.gapY = 6;
			mplayNormLayout.bottomMargin = 70 +3;
			mplayEnlargeWindow.offX= -12;
			mplayEnlargeWindow.offY= -9;
			mplayEnlargeWindow.deltaW= 32;
			mplayEnlargeWindow.deltaH= 24;
			#if 0
			if KIT_WITH_HDMI
			mplayFrameSize.LargeFrmMode = 0;
			#endif

		}
	}

	/*-- Clear OSD & init Layout
	 */
	PB_OSD_CLR; //sp5kGfxPageClear(0,0);
	#if SPCA6330
	sp5kMultiPlayCfgSet(SP5K_MPB_FRAME_SIZE,(UINT32)(&mplayFrameSize), NULL);
	#endif
	sp5kMultiPlayLayoutInit(SP5K_MPB_NXM_LAYOUT,&mplayNormLayout, NULL);

	/*-- Configure the file index of the first thumbnail */
	firstFileIdx = gpbMainCb.curImgIdx % 9;
	if (gpbMainCb.curImgIdx > 9) 
	{
		if (firstFileIdx == 0) 
		{
			firstFileIdx = gpbMainCb.curImgIdx - 8;
		} 
		else 
		{
			firstFileIdx = gpbMainCb.curImgIdx - firstFileIdx + 1;
		}
	} 
	else 
	{
		firstFileIdx = 1;
	}
	sp5kMultiPlayCfgSet(SP5K_MPB_START_FILEINDEX, firstFileIdx, NULL);
	#if SPCA6330    /*Add by Lation@20130717*/
	sp5kMultiPlayCfgSet(SP5K_MPB_STYLE, SP5K_MPB_STYLE_SHOW_ONE_ONCE, NULL);
	#endif

	/*-- Setup Thumbnail IMG for WAV & ERR_FILE
	 */
	sp5kMultiPlayCfgSet(SP5K_MPB_ICON_FOR_WAV, 0, RES_JPG_AUDIO_THUMB);
	sp5kMultiPlayCfgSet(SP5K_MPB_ICON_FOR_ERR_FILE, 0, RES_JPG_ERR_THUMB);

	/*-- Setup FONT & COLOR for MPlay
	 */
	sp5kMultiPlayCfgSet(SP5K_MPB_FOCUS_OSD_COLOR, 3/*COLOR_Red*/, NULL);
	sp5kMultiPlayCfgSet(SP5K_MPB_BACKGROUND_COLOR, 0, NULL);


	sp5kMultiPlayCfgSet(SP5K_MPB_ENLARGE_CURRENT_FILE,1, NULL);
	sp5kMultiPlayCfgSet(SP5K_MPB_ENLARGE_WINDOW,(UINT32)(&mplayEnlargeWindow), NULL);

	/*-- Trigger MultiPlay
	 */
	sp5kModeSet(SP5K_MODE_MULTI_PLAYBACK);

	/*-- Post Initialization
	 */
	gpbMainCb.subStat = PB_MPLAY_STAT_BUSY;
	mplayUIStat = MPLAY_UI_STAT_STANDBY;
	mplayMainMenuSel = MPLAY_MAIN_MENU_SEL_LOCK;

	thumbOsdInfo_Reset();
	thumbOsdInfo_DispXYSetup();

	selTable_Reset();

	return SUCCESS;
}



/*-------------------------------------------------------------------------
 *  File Name : appPbMPlayStOnMsg
 *------------------------------------------------------------------------*/
static UINT8 uThumbIdx=1;
void
appPbMPlayStOnMsg(
	UINT32 msg,
	UINT32 param
)
{
	UINT32 tmpU32;

	PB_VERBOSE("MPLAY_OnMsg: 0x%08x, 0x%08x ... \n", msg, param);

	switch (msg) {
	case APP_UI_MSG_PB_MPLAY_EXECUTE_ALL:
		PB_VERBOSE("\nAPP_UI_MSG_PB_MPLAY_EXECUTE_ALL(%d) !!\n\n", param);
		if (param==MPLAY_EXE_ALL_LOCK) 
		{
			allProc_LockExecute(1 /* do lock */);
		} 
		else if (param==MPLAY_EXE_ALL_UNLOCK) 
		{
			allProc_LockExecute(0 /* do unlock */);
		} 
		else if (param==MPLAY_EXE_ALL_DEL)
		{
			allProc_DeleteExecute();
		}
		break;
	case APP_UI_MSG_PB_MPLAY_EXECUTE:
		PB_VERBOSE("\nAPP_UI_MSG_PB_MPLAY_SEL_EXECUTE(%d) !!\n\n", param);
		if (param==MPLAY_MAIN_MENU_SEL_LOCK) 
		{
			exeProc_LockExecute();
		} 
		else 
		{
			exeProc_DeleteExecute();
		}
		break;
	case APP_UI_MSG_PB_MPLAY_EXECUTE_DONE:
		PB_VERBOSE("\nAPP_UI_MSG_PB_MPLAY_EXECUTE_DONE !!\n\n");
		gpbMainCb.subStat = PB_MPLAY_STAT_STANDBY;
		mplayUIKeyControl(MPLAY_SXN_EXE_END);
		if (mplayMainMenuSel==MPLAY_MAIN_MENU_SEL_DEL) 
		{
			gtotalImgCount = appPbTotalFileCountGet();
			if (gpbMainCb.lastImgIdx>gtotalImgCount)
			{
				gpbMainCb.lastImgIdx =gcurrDcfIdx = gtotalImgCount;
			}

			if (gtotalImgCount==0) 
			{
				appPbMPlayStExit(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
				break;
			}
			appPbMPlayStInit();
		} 
		else 
		{
			gcurrDcfIdx = gpbMainCb.lastImgIdx;
			sp5kDcfFsCurFileIdxSet(gcurrDcfIdx);
			mplayMainDisplayResume();
		}
		break;
	case SP5K_MSG_MULTIPLAY_SHIFT:
		PB_VERBOSE("\nMULTIPLAY_SHIFT_DONE !!\n\n");
		/* update current DCF index */
		gcurrDcfIdx = gpbImgCb.dcfIdx = (param >> 16);
		/*gpbMainCb.subStat = PB_MPLAY_STAT_STANDBY;*/

		tmpU32 = THUMB_PAGE_ID(gcurrDcfIdx);
		#if HOST_DBG
		PB_VERBOSE("curPageIdx(%d), thumbPageIdx(%d), dcfIdx(%d), thumbIdx(%d) \n\n",
					tmpU32, thumbInfo.pageIdx, gcurrDcfIdx,
					THUMB_INDEX(gcurrDcfIdx));
		#endif

		if (tmpU32 != thumbInfo.pageIdx) 
		{
			/*-- thumb page changed, go & renew */
			PB_DBG("Thumb Page changed !!\n");			
			uThumbIdx=1;
			appOsdRefresh(DISABLE); /* Refresh OFF */
			MPLAY_ScrollBar(TRUE, gcurrDcfIdx, gtotalImgCount, MPLAY_THUMB_PER_PAGE);
			thumbOsdInfo_DispClear(THUMB_OSD_UPDATE_ALL, MPLAY_THUMB_ICON_ALL);
			thumbOsdInfo_Construct();
			thumbOsdInfo_DispUpdate(THUMB_OSD_UPDATE_ALL, MPLAY_THUMB_ICON_ALL);
			appOsdRefresh(ENABLE); /* Refresh ON */
		}
		
		if(uThumbIdx <= thumbInfo.lastIdxInPage) {
			appOsdRefresh(DISABLE); 
			thumbOsdInfo_DispUpdate(uThumbIdx, MPLAY_THUMB_ICON_ALL);
			appOsdRefresh(ENABLE); 
			uThumbIdx++;
		}
		else {
			gpbMainCb.subStat = PB_MPLAY_STAT_STANDBY;
		}
		appPlayOsd_FileNumDraw(TRUE, gcurrDcfIdx, gtotalImgCount);
		
		/*-- Emulate KEY Repeat !! */
		mplayEmulateKeyRepeat();
		break;
	case SP5K_MSG_MULTIPLAY_DONE:
		PB_VERBOSE("\nMULTIPLAY_DONE !!\n\n");
		gpbMainCb.subStat = PB_MPLAY_STAT_STANDBY;
		thumbOsdInfo_Construct();
		thumbOsdInfo_DispUpdate(THUMB_OSD_UPDATE_ALL, MPLAY_THUMB_ICON_ALL);
		MPLAY_ScrollBar(TRUE, gcurrDcfIdx, gtotalImgCount, MPLAY_THUMB_PER_PAGE);
		appPbOsd_AuxNote_OKDispPhoto(TRUE);
		appPlayOsd_FileNumDraw(TRUE, gcurrDcfIdx, gtotalImgCount);
		
		break;
	case APP_UI_MSG_PB_MPLAY_UP:
	case APP_UI_MSG_PB_MPLAY_DOWN:
		mplayKey_UP_DOWN_Service((msg==APP_UI_MSG_PB_MPLAY_UP), FALSE);
		break;
	case APP_UI_MSG_PB_MPLAY_LEFT:
	case APP_UI_MSG_PB_MPLAY_RIGHT:
		mplayKey_LEFT_RIGHT_Service((msg==APP_UI_MSG_PB_MPLAY_LEFT), FALSE);
		break;
	default :
		break;
	}
}


/*-------------------------------------------------------------------------
 *  File Name : appPbMPlayStOnKey
 *------------------------------------------------------------------------*/
void
appPbMPlayStOnKey(
	UINT32 msg,
	UINT32 param
)
{
	printf("MPLAY_OnKey : [0x%x] [0x%x]\n", msg,param);

	switch (msg) {
	case APP_KEY_PRESS_MENU:
		break;//disable menu
		mplayKey_MENU_Service();
		break;
	case APP_KEY_PRESS_UP:
	case APP_KEY_PRESS_DOWN:
		if(gtotalImgCount==1)
		{
			break;
		}
		mplayKey_UP_DOWN_Service((msg==APP_KEY_PRESS_UP), TRUE);
		break;
	case APP_KEY_PRESS_LEFT:
	case APP_KEY_PRESS_RIGHT:
		if(gtotalImgCount==1)
		{
			break;
		}
		mplayKey_LEFT_RIGHT_Service((msg==APP_KEY_PRESS_LEFT), TRUE);
		break;
	case APP_KEY_PRESS_S1:
	case APP_KEY_PRESS_S2:
        #if KIT_WITH_HDMI  /*modify for mantis bug 0048437*/
 		if (IS_HDMI_IN)
		{  
			sp5kModeSet(SP5K_MODE_STANDBY);
			sp5kModeWait(SP5K_MODE_STANDBY);
			appPbPIPInit();
		}
        #endif
		if (gpbMainCb.subStat != PB_MPLAY_STAT_STANDBY)
		{
				PB_DBG("MPLAY: busy !!\n");
				return;
		}
		mplayKey_SET_Service();
		break;
	case APP_KEY_PRESS_PB:
		#if !CAM_TYPE_CVR
		if (gpbMainCb.subStat != PB_MPLAY_STAT_STANDBY)
		{
				PB_DBG("MPLAY: busy !!\n");
				return;
		}
		PB_BEEP;
		appUiModeChange();
		#endif
		break;
	case APP_KEY_PRESS_SET:
	case APP_KEY_PRESS_TELE:
		if (gpbMainCb.subStat != PB_MPLAY_STAT_STANDBY)
		{
				PB_DBG("MPLAY: busy !!\n");
				return;
		}
		if (mplayUIStat==MPLAY_UI_STAT_STANDBY || mplayUIStat==MPLAY_UI_STAT_PAN_SET) 
		{
			PB_BEEP;
			#if KIT_WITH_HDMI
 			if (IS_HDMI_IN)
			{  
				sp5kModeSet(SP5K_MODE_STANDBY);
				sp5kModeWait(SP5K_MODE_STANDBY);
				appPbPIPInit();
			}
 			#endif
			appPbMPlayStExit(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
		}
		break;
	default:
		break;
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appPbMPlayState
 *------------------------------------------------------------------------*/
void
appPbMPlayState(
	UINT32 msg,
	UINT32 param
)
{
	PB_VERBOSE("%s : [0x%x] [0x%x]\n",__FUNCTION__,msg,param);

	switch (msg) {
	case APP_STATE_MSG_INIT:
		RPrintf("appPbMPlayState");
		appPbMPlayStInit();
		/* do nothing */
		break;
	case APP_STATE_MSG_CLOSE:
		appPbMPlayStClose();
		appStateCloseDone();
		break;
	default:
		if (IS_APP_KEY_MSG(msg))
		{
			appPbMPlayStOnKey(msg,param);
		}
		else
		{
			appPbMPlayStOnMsg(msg,param);
		}
		break;
	}

}


