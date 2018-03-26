/**************************************************************************
 *                                                                        *
 *       Copyright (c) 2005-2006 by Sunplus Technology Co., Ltd.          *
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
 *  Author: Matt Wang                                                     *
 *                                                                        *
 **************************************************************************/
/* DO NOT EDIT the next 2 lines for auto revision by subversion
$Rev$
$Date::                     $
 */

#ifndef _FS_DEF_H_
#define _FS_DEF_H_


#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/
#include "common.h"
#ifndef _COMMON_TYPES_H_
#include "middleware/common_types.h"
#endif

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#if FS_BODY_TYPE == FS_BODY_VFS

/*
 * fsFileOpen(), definition here MUST manually sync to vfs_api.h.
 */
typedef enum {
	FS_OPEN_APPEND = (1 << 0),
	FS_OPEN_BINARY = (1 << 1),
	FS_OPEN_CREATE = (1 << 2),
	FS_OPEN_RDONLY = (1 << 4),
	FS_OPEN_RDWR   = (1 << 5),
} fsOpenFlag_e;

/*
 * fsFileSeek(), definition here MUST manually sync to vfs_api.h.
 */
typedef enum {
	FS_SEEK_SET = 0,
	FS_SEEK_CUR = 1,
	FS_SEEK_END = 2,
} fsSeekMode_e;

/*
 * fsFileAttrSet().
 */
typedef enum {
	FS_ATTR_READONLY = (1 << 0),
	FS_ATTR_HIDDEN   = (1 << 1),
	FS_ATTR_SYSTEM   = (1 << 2),
	FS_ATTR_LABEL    = (1 << 3),
	FS_ATTR_DIR      = (1 << 4),
	FS_ATTR_DIRTY    = (1 << 5),
} fsAttr_e;

/*
 * fsAliasNameGet().
 */
typedef enum {
	FS_STR_NATIVE  = 0,
	FS_STR_UNICODE = 1,
} fsCodeType_e;

#elif FS_BODY_TYPE == FS_BODY_POSIX

typedef enum {
	FS_SEEK_SET = SEEK_SET,
	FS_SEEK_CUR = SEEK_CUR,
	FS_SEEK_END = SEEK_END,
} fsSeekMode_e;

#endif

/**************************************************************************
 *                           D A T A   T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                               M A C R O S                              *
 **************************************************************************/

/**************************************************************************
 *          M O D U L E   V A R I A B L E S   R E F E R E N C E S         *
 **************************************************************************/

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/
/*
 * fs_alias_name.c
 */
UINT8 *fsAliasNameGet(const UINT8 *fileName, UINT32 codeType);

/*
 * fs_dir_change.c
 */
UINT32 fsDirChange(const UINT8 *pathName);

/*
 * fs_dir_make.c
 */
UINT32 fsDirMake(const UINT8 *pathName);

/*
 * fs_dir_remove.c
 */
UINT32 fsDirRemove(const UINT8 *pathName);

/*
 * fs_file_attr_get.c
 */
UINT32 fsFileAttrGet(const UINT8 *fileName, UINT32 *pattr);

/*
 * fs_file_attr_set.c
 */
UINT32 fsFileAttrSet(const UINT8 *fileName, UINT32 attr);

/*
 * fs_file_close.c
 */
UINT32 fsFileClose(UINT32 fd);

/*
 * fs_file_delete.c
 */
UINT32 fsFileDelete(const UINT8 *fileName);

/*
 * fs_file_open.c
 */
UINT32 fsFileOpen(const UINT8 *fileName, SINT32 flag);

/*
 * fs_file_read.c
 */
UINT32 fsFileRead(UINT32 fd, UINT8 *buf, UINT32 size);

/*
 * fs_file_seek.c
 */
UINT32 fsFileSeek(UINT32 fd, SINT64 offset, UINT32 mode);
UINT32 fsFileSeekI64(UINT32 fd, SINT64 offset, UINT32 mode);
UINT32 fsFileQseek(UINT32 fd, SINT64 offset);
UINT32 fsFileQseekI64(UINT32 fd, SINT64 offset);
UINT32 fsFilePreLseek(UINT32 fd);

/*
 * fs_file_size_get.c
 */
UINT32 fsFileSizeGet(UINT32 fd);
UINT64 fsFileSizeGetI64(UINT32 fd);

/*
 * fs_file_tell.c
 */
UINT32 fsFileTell(UINT32 fd);
UINT64 fsFileTellI64(UINT32 fd);

/*
 * fs_file_write.c
 */
UINT32 fsFileWrite(UINT32 fd, UINT8 *buf, UINT32 size);

/*
 * fs_next_file.c
 */
UINT32 fsNextFileNameGet(UINT8 *fileName, UINT32 *nameLength);

UINT32 fsFifOpen(const UINT8 *fileName);
UINT32 fsFifClose(UINT32 hFif);
UINT32 fsFifOpenCust(const UINT8 *fileName);
UINT32 fsFifCloseCust(UINT32 hFif);


/*
 * fs_simple.c
 */
UINT32 fsSimpleWrite(UINT8 *fname, void *buf, UINT32 size);
UINT32 fsSimpleRead(UINT8 *fname, void *buf, UINT32 size);
UINT32 fsSimpleReadEx(UINT8 *fname, void**pbuf, UINT32 *psize);
UINT32 fsFileCopy(UINT8 *psrcfname, UINT8 *pdstfname);
UINT32 fsIsFileExists(UINT8 *fname);

/*
 * fs_fic.c
 */
#ifndef _FIC_HDR_T_
#define _FIC_HDR_T_
struct ficHdr_s;
typedef struct ficHdr_s ficHdr_t;
#endif /*#ifndef _FIC_HDR_T_*/

ficHdr_t *fsIsFicData(void *p);

/*
 * fs_direct_get.c
 */
/**
 * \brief direct reading file contents to caller or system provided buffer;
 * if file in a FIF and pbuf == NULL, directly returns the buffer
 * address in FIF raw buffer without any copying
 * \param fname, the file name string
 * \param pbuf, caller to assign the buffer to store the contents of file;
 * pbuf==0 means system to assign the buffer; if file in a FIF and pbuf==0,
 * directly returns the buffer address in FIF raw buffer;
 * if pbuf!=0 and in cacheable address, caller is resposible for dcache
 * coherence problems; if pbuf in uncacheable address, cache coherence is
 * handled here
 * \param buflmt, pbuf's size limit, not to overwrite exceeding buflmt size;
 * ignored if pbuf==0
 * \param pfsize, to return the file size, regardless of pbuf limit;
 * ignored if pfsize==0
 * \param phandle, to return a handle, which is needed when caller no long
 * needs the direct buffer and wants to free
 * \param fifSel, get the file from FIF forcibly if 1
 * \return the buffer address of result contains of file. returns 0
 * if reading failed; returns argument pbuf back if pbuf!=0 and read success;
 * returns system provided *cacheable* buffer address if pbuf==0
 * (allocated by system or direct in FIF buffer address if file in a FIF),
 * \note caller is responsible for dcache coherence problems unless argument
 * pbuf is uncacheable buffer
 * \note phandle actually returns an allocated *cacheable* buf pointer to be
 * freed at pbuf=fsFileDirectGet(..., &handle); ...; fsFileDirectFree(handle);
 * when *phandle returns non-zero handle and the pbuf is never dcache dirty or
 * not-invalid, caller may want to convert handle to *uncacheable* and calls
 * if (handle) fsFileDirectFree(TO_UNCACHE(handle)); to save time.
 */
void *fsFileDirectGet(const char *fname, void *pbuf, UINT32 buflmt,
		UINT32 *pfsize, UINT32 *phandle, UINT32 fifSel);

/**
 * \brief to free a previous fs direct buffer returned by fsFileDirectGet()
 * \param handle, the value pointed by *phandle returned in the previous
 * fsFileDirectGet(..., &handle); ...; fsFileDirectFree(handle);
 */
void fsFileDirectFree(UINT32 handle);

/*
 *fs_file_rename.c
 */
UINT32 fsFileRename(UINT8 *srcFile,UINT8 *dstFile);

/*
 *fs_volume_lable_set.c
 */
UINT32 fsFatVolLabSet(void *para0,void *para1);

/*
 *fs_bs_volume_lable_set.c
 */
UINT32 fsFatBSVolLabSet(void *para0,void *para1);
/*
 *fs_dirname_get.c
 */
UINT8 *fsGetCurrDirName(void);

/*
 *fs_cluster_size_get.c
 */
UINT32 fsGetClusterSize(void);

/*
 *fs_file_cnt_get.c
 */
UINT32 fsDirFileCnt(UINT8 *dirPath);

/*
 *fs_dir_cache_set.c
 */
SINT32 fsDirCacheSet(UINT8 *pDirPath,UINT32 option);

/*
 *fs_file_time_get.c
 */
UINT32 fsFileAllTimeGet(UINT8 *pname,tmx_t *pAcessTime,tmx_t *pModifyTime,tmx_t *pChangeTime);

/*
 * fs_dir_nextfile_get.c
 */
UINT32 
fsDirNextFileGet(
	UINT8 *pFileName, /* file name */
	UINT32 *pNameLen, /* file name buffer length */
	UINT32 *pAttr, /* file attribute */
	UINT64 *pSize, /* file size */
	tmx_t *pTm /* file time. decide by DISK option, 0 for ctime, 1 for mtime, 2 for minimum of ctime/mtime */
);

#if 0

/*=============================================*/
UINT32 sp5kFsLongFileNameGet();

UINT8 *vfsLongFileNameGet(UINT8 *pname);

/*=============================================*/
UINT32 sp5kFsShortFileNameGet();

UINT8 *vfsAliasNameGet(UINT8 *pname, UINT32 uni);

/*=============================================*/
UINT32 sp5kFsFileCopy();

UINT32 vfsFileCopy(UINT8 *psrcFile, UINT8 *pdstFile);

#endif


#ifdef __cplusplus
}
#endif

#endif  /* _FS_DEF_H_ */
