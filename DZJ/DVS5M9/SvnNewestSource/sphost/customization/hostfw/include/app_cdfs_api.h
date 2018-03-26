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
#ifndef _APP_CDFS_H_
#define _APP_CDFS_H_

/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/
/**************************************************************************
 *                               M A C R O S                              *
 **************************************************************************/
#if !CAM_TYPE_CVR
#define SP5K_CDFS_OPT 0
#else
#define SP5K_CDFS_OPT 1
#endif

#define CDFS_FILE_NAME_LENGTH		64
#define CDFS_ROOT_NAME_SDV		"SDV"
#define CDFS_ROOT_NAME_CDV		"CDV"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
 
typedef struct CDFS_TAB_S
{
	UINT8 FileName[25];
	UINT32 attr;/*read from fdb*/
	UINT32 dcfattr;/*for host*/
	UINT32 FileSize;
	UINT32 FileType;
	struct CDFS_TAB_S *next;
	struct CDFS_TAB_S *pre;
}CDFS_TAB_T;

enum FILE_FORMAT
{
	CDFS_FILE_FOLDER_VIDEO,
	CDFS_FILE_FOLDER_JPG,
	CDFS_FILE_FOLDER_EVENT,
	CDFS_FILE_FOLDER_AUDIO,
	CDFS_FILE_FOLDER_LAST,
};

 enum DEL_METHOD
{
	CDFS_PROTECTED_SKIP,
	CDFS_PROTECTED_DEL,
};

 enum CDFS_EVENT
{
	JPG_EVENT_IN_VIDEO	= 1<<0,
	VIDEO_EVENT_IN_VIDEO		= 1<<1,
};

 
/**************************************************************************
 *          M O D U L E   V A R I A B L E S   R E F E R E N C E S         *
 **************************************************************************/

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/
UINT32  appCdfsFolderInit(UINT32 folder);
void appCdfsFilePoolInit(void);
void appCdfsPoolReset(UINT32 folder);
void appCdfsPoolInfoDump(UINT32 folder);
CDFS_TAB_T * appCdfsFileNewPoolReg(UINT32 folder);
UINT32 appCdfsCurrFolderFileListCreate(UINT32 folder,UINT8* dirPath);
UINT32 appCdfsAddFile2List(UINT8 *filename,UINT32 filenamelen,UINT32 filesize,UINT32 fileattr);
UINT32 appCdfsFileAdd(UINT8 *filename);
UINT32 appCdfsFileDel(UINT8 *filename);
UINT32 appCdfsNextFileNameGet(UINT32 filetype,UINT8 *pFileName);
UINT32 appCdfsFileFolderSet(UINT32 folder);
UINT32 appCdfsFileFolderGet(void);
UINT32 appCdfsCurFileIdxSet(UINT32 fileIdx);
UINT32 appCdfsCurFileIdxGet(UINT32 *fileIdx);
UINT32 appCdfsFileNumGet(UINT32 *filenum);
UINT32 appCdfsFileAttrGet(UINT32 fileIdx, sp5kDcfAttrElm_t* pfileInfoAtr, UINT8* pname);
UINT32 appCdfsInfoGet(UINT32 option, UINT32* pValue);
UINT32 appCdfsFileOperate(UINT32 option, UINT32 param, UINT32 async, fpsp5kCallback_t pcallback, UINT32 length);
UINT32 appCdfsFileConfirm(UINT8* fname,UINT32 folder);
UINT64 appCdfsCanDelVideoSizeGet(void);
UINT32 appCdfsTheAllFileDel(UINT32 folder);
UINT32 appCdfsTheNewestFileDel(UINT32 folder);
UINT32 appCdfsTheOldestFileDel(void);
UINT32 appCdfsFreeSpaceCheck(UINT32 folder, BOOL bOverWrite);
UINT32 appCdfsEventFlagGet(void);
UINT32 appCdfsFileNameValidCheck(UINT32 filetype);
UINT32 appCdfsFileNameToFileType(UINT8 *filename);
UINT32 appCdfsFileListTabInit(UINT32 folder);
UINT32 appCdfsEventProcess(UINT8 *filename,UINT32 event);
UINT32 appCdfsFolder2Path(UINT32 folder,UINT8 *path);
UINT8* appCdfsFolderRootGet(void);
UINT32 appCdfsAddFileStsGet(void);
void appCdfsFileListPrintf(UINT32 folder);
UINT8 appCdfsFileNameExistCheck(UINT8 *pCheckFileName);
void appCdfsEventFlagSet(UINT32 flag);
void appCdfsEventFlagClean(UINT32 flag);
UINT8 * appCdfsPreFileNameGet(void);
UINT8 * appCdfsLastFileNameGet(void);
UINT8 * appCdfsLastAviNameGet(void);
UINT8 appCdfsCapNumIncrease(void);
UINT8 appCdfsCapNumSet(UINT8 value);
CDFS_TAB_T *appCdfsFileListLastGet(UINT32 folder);
CDFS_TAB_T *appCdfsFileListHeadGet(UINT32 folder);
CDFS_TAB_T * appCdfsFileHandleGet(UINT32 idx);
void 
	appCdfsInit(void);
UINT32 appCdfsAbsolutePathToFile(UINT8 *filename);

#endif  /* _APP_CDFS_H_ */


