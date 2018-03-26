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
#define MODEL_VERSION "$unp_sensor_driver_util-0.00.01"

#include "common.h"
#include "stdio.h"
#include "flow/sensor_model.h"
#include "middleware/fs_def.h"

void*  osMemAllocCache(UINT32 size);
UINT32 osMemFree(void *pointer);
void cpuDCacheFlush(void *addrstart, UINT32 size);

#define LOGI_CACH_BADDR_TO_LOGI_UNCACH_BADDR(addr) \
	((void *)((UINT32)(addr) | 0x20000000))

typedef struct {
	UINT16 rsv1;
	UINT16 sig;
	UINT16 ver;
	UINT16 datasize;
} spbinhdr_t;

#define HDRSIZE	sizeof(spbinhdr_t)

UINT32
afeCmdBufExecute(
	UINT8 *cmdbuf,
	UINT32 bufsize,
	UINT32 cmdportid,
	UINT8 nbitaddr,
	UINT8 nbitdata
)
{
	UINT32 cmdnum,us,cmdbytes,i,cmdhdrlen;
	spbinhdr_t *hdr;

	hdr = (spbinhdr_t *)cmdbuf;
	if (hdr->sig!=5110 || hdr->ver!=1 || hdr->datasize+HDRSIZE!=bufsize) {
		printf("** hdr err(sig=%d ver=%d size=%d)\n", hdr->sig, hdr->ver, hdr->datasize);
		return FAIL;
	}
	cmdbuf += HDRSIZE;
	bufsize -= HDRSIZE;

	cmdhdrlen = cmdbuf[0];
	if (cmdhdrlen!=HDRSIZE || cmdbuf[1]!=nbitaddr || cmdbuf[2]!=nbitdata) {
		printf("** cmdhdr err(%d %d %d)\n", cmdhdrlen, cmdbuf[1], cmdbuf[2]);
		return FAIL;
	}
	cmdbytes = cmdbuf[3];
	i = cmdhdrlen;
	while (i+4<bufsize) {
		cmdnum = *(UINT16 *)(cmdbuf+i); i += sizeof(UINT16);
		us = *(UINT16 *)(cmdbuf+i); i += sizeof(UINT16);
		if (i + cmdnum * cmdbytes > bufsize) {
			printf("** size exceed [cmdnum=%d?]\n", cmdnum);
			break;
		}
		sensorSerialBulkWrite(cmdportid, cmdnum, cmdbuf+i, 0);
		tmrUsWait(us);
		i += cmdnum * cmdbytes;
		if (i&1) i++;
	}
	return SUCCESS;
}

UINT8*
afeCmdFileRead(
	UINT8 *fname,
	UINT8 *pbuf,
	UINT32 buflen,
	UINT32 *filelen
)
{
	UINT32 fd,fsize;
	fd = fsFileOpen(fname, 0x12); /* read only/binary */
	if (!fd) {
		printf("** afeinit res(%s)load fail\n", fname);
		goto afefileread_exit;
	}
	fsize = fsFileSizeGet(fd);
	if (pbuf==NULL) {
		pbuf = osMemAllocCache(fsize);
		if (!pbuf) {
			printf("** agcinit res no space\n");
			goto afefileread_exit;
		}
	} else if (buflen<fsize) {
		printf("** agcinit buf too small (%d/%d)\n", buflen, fsize);
		goto afefileread_exit;
	}
	cpuDCacheFlush(pbuf, fsize);
	fsFileRead(fd, (void *)LOGI_CACH_BADDR_TO_LOGI_UNCACH_BADDR(pbuf), fsize);
	if (filelen)
		*filelen = fsize;

afefileread_exit:
	if (fd)	fsFileClose(fd);
	return pbuf;
}

UINT32
afeCmdFileExecute(
	UINT8 *fname,
	UINT32 cmdportid,
	UINT8 nbitaddr,
	UINT8 nbitdata
)
{
	UINT32 fsize,err=FAIL;
	UINT8 *pbuf;

	pbuf = afeCmdFileRead(fname, NULL, 0, &fsize);

	if (pbuf) {
		if (fsize) {
			err = afeCmdBufExecute(pbuf, fsize, cmdportid, nbitaddr, nbitdata);
			if (err!=SUCCESS)
				printf("** execute %s err\n", fname);
		}
		osMemFree(pbuf);
	}
	return err;
}

