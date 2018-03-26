#ifndef __AAA_DEBUG_H__
#define __AAA_DEBUG_H__
//////////////////////////////////////////////////////////////////////////
#include "common.h"
//////////////////////////////////////////////////////////////////////////
typedef struct{
	UINT32 flag;
	UINT32 size;
}aaaDebug_t;
typedef enum{	
	AAA_INFO_Awb = 0x55aaf100,
	AAA_INFO_Ae = 0x55aaf200,
	AAA_INFO_Af = 0x55aaf300,

	AAA_INFO_Mask = 0x55aaff00,
}aaaDebugFlag_e;
//////////////////////////////////////////////////////////////////////////
extern UINT8* aaaInfoBuf;
extern UINT32 aaaInfoBufSize;

/**************************************************************************
 * Append data to buffer,
 * pt: data pointer,start with aaaDebug_t
 * size: data length
 **************************************************************************/
UINT32 aaaInfoAppend(UINT8* pt,UINT32 size);

/**************************************************************************
 * Start up write info function, create info file 
 * fname: info file name
 **************************************************************************/
UINT32 aaaInfoStart(UINT8* fname);
/**************************************************************************
 * Write buffer to storage
 * called by one task
 **************************************************************************/
//UINT32 aaaInfoWrite();
/**************************************************************************
 * Stop and close file
 **************************************************************************/
UINT32 aaaInfoStop();

UINT32 aaaInfoCopyTo(UINT8* pt,UINT32 size);
UINT32 aaaInfoGet(UINT8** ppt);

void aaaInfoFileNameCreate(UINT8* fnext,UINT8* fname);
#endif