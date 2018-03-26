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
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "api/sp5k_global_api.h"
#include "api/sp5k_capture_api.h"
#include "api/sp5k_cdsp_api.h"
#include "customization/solution_def.h"
#include "customization/dev_init.h"

#include "sp5k_capture_api.h"
#include "app_still.h"
#include "app_calib_api.h"
#include "app_aaa.h"
#include "app_exif.h"
#include "app_flash_api.h"
#include "sp5k_awb_api.h"
#include "app_timer.h"

/* raw AWB sample code */
#define AWB_AREA_RATIO			1.05 /* get center area */
#define CALIB_AWB_AREA_RATIO	4 /* get center area */
#define RB_GAIN_HIS_TBL_SIZE	20
#define AWB_Q_SIZE				18
#define SHOW_POSTWB_SAMPLING_POINT 0 /* 1: show the sampling point, 0: don't show */
#define AWB_SAMPLE_POINT_SAVE	0
#if (defined(EVB_CFG_COUGAR)||defined(EVB_CFG_LYNX))
#define AWB_GRAY_WORD_ONLY 0
#else
#define AWB_GRAY_WORD_ONLY 0
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
 typedef struct awb_pixel_sample_t {
	UINT32 r;
	UINT32 g;
	UINT32 b;
	UINT32 cnt;
} awb_pixel_sample_s;
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
static UINT32 Sum[4];
static UINT32 rgb[4];
UINT16 AwbCT_Rgain[3];
UINT16 AwbCT_Bgain[3];
UINT16 AwbCtt[40]={0x6, 0x400, 0x400, 0x400 , 0x400, 0x400, 0x400};
/*
	The ctt format from sk5p awb utility
	[0] = number of parameters
	[1] = r gain of A light
	[2] = b gain of A light
	[3] = r gain of TL84 light
	[4] = b gain of TL84 light
	[5] = r gain of D light
	[6] = b gain of D light
*/

UINT16 rbGainHistory[RB_GAIN_HIS_TBL_SIZE][2];
UINT32 rbGainHistoryIndex=0;
UINT32 postWbSampleNumber=0;
UINT16 postWbMaxRGB[5];

UINT8 proc_awb_calib;
UINT32 IQlog_BufSize;
UINT8 *IQlog_putsBufCurPtr, *IQ_LogBuf;

#define AWB_SAMPLE_POINT_BUF_SIZE (10*1024)/*35*1024*/
#define AWB_DB_INFO_SIZE (AWB_SAMPLE_POINT_BUF_SIZE+2048)
UINT8 awb_db_info[AWB_DB_INFO_SIZE];
UINT8* awb_db_buf=awb_db_info;
UINT8* awb_point_buf=awb_db_info+2048;
UINT8* awb_pint_buf_cur_pt = awb_db_info+2048;
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern UINT16 aeCurContrastGet(	void);
extern void awbCurAeInfoSet(UINT16 *pInfo);
extern void awbCurRBGainGet(UINT16* viewRgain,UINT16* viewBgain,UINT16* capRgain,UINT16* capBgain);
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
UINT32 SP_AwbCalibSampling(UINT32 addr, UINT32 RawFmt, void * ImgSize);

#if AWB_SAMPLE_POINT_SAVE
void
SP_AwbPointBufReset(void)
{
	awb_pint_buf_cur_pt = awb_point_buf;
}

void
SP_AwbPointPrintf(const char *szFmt, ...)
{
    va_list vaPtr;
    unsigned char szBuffer[100];
	UINT32 buflen;

    va_start(vaPtr, (const char *)szFmt);
    vsprintf((char *)szBuffer, (const char *)szFmt, vaPtr);
	va_end(vaPtr);
	buflen=strlen(szBuffer);

	if((awb_pint_buf_cur_pt - awb_point_buf + buflen) > AWB_SAMPLE_POINT_BUF_SIZE )
		return;
	memcpy(awb_pint_buf_cur_pt, szBuffer, buflen);
	awb_pint_buf_cur_pt = awb_pint_buf_cur_pt + buflen;
}

void
SP_AwbPointBufDump(void)
{
	sp5kDcfAttrElm_t DcfAttr;
	UINT32 FileIdx;
	UINT8 filename[32];
	int i;

	if(awb_pint_buf_cur_pt == awb_point_buf)
		return;
	SP_AwbPointBufReset();

	sp5kDcfFsCurFileIdxGet(&FileIdx);
	sp5kDcfFsFileAttrGet(FileIdx , &DcfAttr, filename);
	//printf("FileIdx = %d \n", FileIdx);
	//printf("%s \n",filename);
	sprintf(strstr(filename, "JPG"), "TXT");
	printf("%s \n",filename);

	WriteFile(filename, (UINT8*)awb_point_buf, AWB_SAMPLE_POINT_BUF_SIZE);
	for(i=0; i<AWB_SAMPLE_POINT_BUF_SIZE; i++) {
			awb_point_buf[i]=0xff;
	}

}
#else
void
SP_AwbPointBufReset(void)
{
}

void
SP_AwbPointPrintf(const char *szFmt, ...)
{
}

void
SP_AwbPointBufDump(void)
{
}
#endif

/*-------------------------------------------------------------------------
 *  Function Name : SP_IQLogInit
 *  Description : This will reset the log address. It is called in Exif CB.
 *------------------------------------------------------------------------*/
void
SP_IQLogInit(void)
{
	appExif_MN_BufClear(EXIF_MN_IQ);
}

/*-------------------------------------------------------------------------
 *  Function Name : SP_IQLogPirntf
 *  Description :
 *------------------------------------------------------------------------*/
void
SP_IQLogPirntf(const char *szFmt, ...)
{
	DBG_PRINT("%s ...", __FUNCTION__);

    va_list vaPtr;
    unsigned char szBuffer[100];
	UINT32 buflen;

    va_start(vaPtr, (const char *)szFmt);
    vsprintf((char *)szBuffer, (const char *)szFmt, vaPtr);
	va_end(vaPtr);
	buflen=strlen(szBuffer);

	appExif_MN_Printf(EXIF_MN_IQ, szBuffer);
}

/*-------------------------------------------------------------------------
 *  Function Name : SP_IQAWBLog
 *  Description :   This function will put the information into EXIF.
 					It will be called in Exif CB.
 *------------------------------------------------------------------------*/

void
SP_IQAWBLog(void)
{	
#if	0
	UINT32 radius;
	int i;
	pfhDbgInfoSt_t *pFlash;

	DBG_PRINT("%s ...", __FUNCTION__);

	SP_IQLogPirntf("AWB CTT gain (r,b)=(%d,%d)(%d,%d)(%d,%d)",
		AwbCT_Rgain[0], AwbCT_Bgain[0],
		AwbCT_Rgain[1], AwbCT_Bgain[1],
		AwbCT_Rgain[2], AwbCT_Bgain[2]);

	SP_IQLogPirntf("raw(rgb,x,y)=(%d,%d,%d,%d,%d)",
		postWbMaxRGB[0],postWbMaxRGB[1],postWbMaxRGB[2],postWbMaxRGB[3],postWbMaxRGB[4]);

	SP_IQLogPirntf("raw avg (rggb)=(%d,%d,%d,%d)",
		Sum[0], Sum[1], Sum[2], Sum[3]);

	SP_IQLogPirntf("-est(%d, %d) -avg(%d,%d) -real(%d, %d)",
		postWbMaxRGB[0]>0? postWbMaxRGB[1]*1024/postWbMaxRGB[0]: 9999,
		postWbMaxRGB[2]>0? postWbMaxRGB[1]*1024/postWbMaxRGB[2]: 9999,
		rgain_avg,
		bgain_avg,
		gStillCB.awb.rGain,
		gStillCB.awb.bGain);

	SP_IQLogPirntf("preview awb gain");
	for(i=RB_GAIN_HIS_TBL_SIZE; i>0; i--) {
		if(rbGainHistoryIndex == 0){
			rbGainHistoryIndex = RB_GAIN_HIS_TBL_SIZE-1;
		}
		SP_IQLogPirntf("(%d,", rbGainHistory[rbGainHistoryIndex][0]);
		SP_IQLogPirntf("%d)", rbGainHistory[rbGainHistoryIndex--][1]);
	}
	rbGainHistoryIndex=0;

	SP_IQLogPirntf("(cnt=%d)",postWbSampleNumber);
	SP_IQLogPirntf("q(");
	for(i=0; i<AWB_Q_SIZE; i++) {
		SP_IQLogPirntf("%d ", q_area[i]);
		postWbSampleNumber -= q_area[i];
	}
	SP_IQLogPirntf(")");
	SP_IQLogPirntf("(wp=%d)",postWbSampleNumber);
	#if 1//SPCA5210
	SP_IQLogPirntf("original %d %d (%d)", mirror_db_avg_rgain, mirror_db_avg_bgain, mirror_db_area);
	#endif

	SP_IQLogPirntf("***______****");
	SP_IQLogPirntf("AF");
	SP_IQLogPirntf("***______****");
	SP_IQLogPirntf("AE");
	SP_IQLogPirntf("***______****");
	SP_IQLogPirntf("IQ");
	SP_IQLogPirntf("***______****");
	pFlash = appPreflashDebugInfoGet();
	SP_IQLogPirntf("Flash");
	SP_IQLogPirntf("SV(view=%d,cap=%d)",pFlash->viewSv, pFlash->capSv);
	SP_IQLogPirntf("(hpos=%d,pulseW=%d)",pFlash->hPos, pFlash->pulseW);
	SP_IQLogPirntf("***______****");

	SP_IQLogPirntf("Others");
#else

	/*appExif_MN_Write16(EXIF_MN_IQ, 0);*/

	/*
	radius = (AwbCT_Rgain[1]/10);
	appExif_MN_Write32(EXIF_MN_IQ, 0xaa550505);
	appExif_MN_Write32(EXIF_MN_IQ, AwbCT_Rgain[0]>>2);
	appExif_MN_Write32(EXIF_MN_IQ, AwbCT_Rgain[1]>>2);
	appExif_MN_Write32(EXIF_MN_IQ, AwbCT_Rgain[2]>>2);
	appExif_MN_Write32(EXIF_MN_IQ, AwbCT_Bgain[0]>>2);
	appExif_MN_Write32(EXIF_MN_IQ, AwbCT_Bgain[1]>>2);
	appExif_MN_Write32(EXIF_MN_IQ, AwbCT_Bgain[2]>>2);
	appExif_MN_Write32(EXIF_MN_IQ, (AwbCT_Rgain[0] + radius)>>2);
	appExif_MN_Write32(EXIF_MN_IQ, (AwbCT_Rgain[1] + radius)>>2);
	appExif_MN_Write32(EXIF_MN_IQ, (AwbCT_Rgain[2] + radius)>>2);
	appExif_MN_Write32(EXIF_MN_IQ, (AwbCT_Rgain[0] - radius)>>2);
	appExif_MN_Write32(EXIF_MN_IQ, (AwbCT_Rgain[1] - radius)>>2);
	appExif_MN_Write32(EXIF_MN_IQ, (AwbCT_Rgain[2] - radius)>>2);
	appExif_MN_Write32(EXIF_MN_IQ, (AwbCT_Bgain[0] + radius)>>2);
	appExif_MN_Write32(EXIF_MN_IQ, (AwbCT_Bgain[1] + radius)>>2);
	appExif_MN_Write32(EXIF_MN_IQ, (AwbCT_Bgain[2] + radius)>>2);
	appExif_MN_Write32(EXIF_MN_IQ, (AwbCT_Bgain[0] - radius)>>2);
	appExif_MN_Write32(EXIF_MN_IQ, (AwbCT_Bgain[1] - radius)>>2);
	appExif_MN_Write32(EXIF_MN_IQ, (AwbCT_Bgain[2] - radius)>>2);
	appExif_MN_Write32(EXIF_MN_IQ, rgain_avg>>2);
	appExif_MN_Write32(EXIF_MN_IQ, bgain_avg>>2);
	appExif_MN_Write32(EXIF_MN_IQ, 0xaa550606);
	*/
	/*
	appExif_MN_Write32(EXIF_MN_IQ, 0xaa550303);
	appExif_MN_WriteBytes(EXIF_MN_IQ, awb_point_buf, AWB_SAMPLE_POINT_BUF_SIZE);
	appExif_MN_WriteBytes(EXIF_MN_IQ, awb_db_info, 768);

	appExif_MN_Write32(EXIF_MN_IQ, 0xaa55fefe);
	*/
	/*
	if(appExif_MN_WriteBytes(EXIF_MN_IQ, awb_db_info, AWB_DB_INFO_SIZE)==FAIL)
		printf("awbinfo buf overflow!\n");
	*/
	UINT32 size=postWbSampleNumber*4+512;
	if(size>AWB_DB_INFO_SIZE)
	{
		size=AWB_DB_INFO_SIZE;
	}

#endif
}
/*-------------------------------------------------------------------------
 *  Function Name : SP_AWBInit
 *  Description :
 *------------------------------------------------------------------------*/

void SP_AWBInit()
{
	appCalibData_t *pCalib;
	SINT16 rgain, bgain;
	UINT32 one=0x400;

	sp5kAwbParamSet(SP5K_AWB_CONVERGE_RANGE, 10);
	sp5kAwbParamSet(SP5K_AWB_PURE_COLOR_AREA, 95);
	sp5kAwbParamSet(SP5K_AWB_CONVERGE_SPEED, 0xffffffff);
	sp5kSystemCfgSet(SP5K_CDSP_INFO_CFG_GROUP, 0, one);
	sp5kAwbParamSet(SP5K_AWB_ALightMode,0); 
	sp5kImgModeSet(SP5K_MODE_STILL_PREVIEW);
	sp5kImgCdspBlockEnable(SP5K_CDSP_2ND_3A, 1);
	sp5kImgModeSetDone(SP5K_MODE_STILL_PREVIEW);

	sp5kAwbCfgSet(SP5K_AWB_WIN_PATH, SP5K_AWB_WIN_PATH_2ND_PATH);


	pCalib = appCalibDataGet();
	rgain = pCalib->WbRgain;
	bgain = pCalib->WbBgain;
	sp5kAwbCTTblGet(AwbCtt);
	printf("awbinit:calib(0x%x,0x%x)D(0x%x,0x%x)\n",rgain,bgain,AwbCtt[6],AwbCtt[7]);
	/* AwbCtt[0] is number of ct */

	/*
	if(pCalib->WbSign){
		AwbCtt[1] += rgain;
		AwbCtt[3] += rgain;
		AwbCtt[5] += rgain;
		AwbCtt[2] += bgain;
		AwbCtt[4] += bgain;
		AwbCtt[6] += bgain;
		sp5kAwbCTTblSet(AwbCtt);
		printf("AWB gain compensation (r=%s%d, b=%s%d)\n",
			rgain>0? "+" : "-",
			rgain>0? rgain: -rgain,
			bgain>0? "+" : "-",
			bgain>0? bgain: -bgain
		);
	}
	*/
#if 0
	printf("awbctt init\n");
	for(i=0; i<6; i+=2) {
		printf("[%d]=%d, [%d]=%d\n",i, AwbCtt[i], i+1, AwbCtt[i+1]);
	}
#endif
	AwbCT_Rgain[0] = AwbCtt[1]<<4;
	AwbCT_Rgain[1] = AwbCtt[3]<<4;
	AwbCT_Rgain[2] = AwbCtt[5]<<4;
	AwbCT_Bgain[0] = AwbCtt[2]<<4;
	AwbCT_Bgain[1] = AwbCtt[4]<<4;
	AwbCT_Bgain[2] = AwbCtt[6]<<4;
#if SP5K_ALGORITHM_AWB
	sp5kAwbModeSet(SP5K_AWB_MODE_INFO_ONLY);
#else
	sp5kAwbModeSet(SP5K_AWB_MODE_AUTO_SET);
#endif

}
/*-------------------------------------------------------------------------
 *  Function Name : SP_AWBCTTResLoad
 *  Description :
 *------------------------------------------------------------------------*/

void SP_AWBCTTResLoad()
{
}

/*-------------------------------------------------------------------------
 *  Function Name : SP_CaptureDefaultPostWbSampling
 *  Description : This will be called in WbRawCapCallback and it's rggb format
 				  is different with different sensors.
 *------------------------------------------------------------------------*/
#if 1
UINT32 SP_CaptureDefaultPostWbSampling(UINT32 addr, UINT32 RawFmt, void * RawInfo)
{
	sp5kImgWbInfo_t *info=(sp5kImgWbInfo_t*)RawInfo;
	UINT16 *ImageAddress=(UINT16 *)addr;
	UINT16 *p0,*p;
	UINT32 x0,y0,w,h;
	UINT32 x,y;
	UINT32 cnt;
	UINT32 r, gr ,b, gb, g;
	UINT8 ridx[]={1, 0, 3, 2};//BAYER_GRBG, BAYER_RGGB, BAYER_BGGR, BAYER_GBRG,
	UINT8 gidx[]={0, 1, 2, 3};
	UINT8 bayerIdx;
	UINT32 WghtXS1, WghtXE1, WghtYS1, WghtYE1, WghtXS2, WghtXE2, WghtYS2, WghtYE2, OrigDX, OrigDY, Xstep, Ystep;
	int spe_dx, spe_dy, spe_iy, spe_ix, temp3=0, spe_buf_idx=0, spe_cntx=0, spe_cnty=0;;
	/*wangjg++*/
	UINT32 xDim = 50,yDim = 40;
	UINT32 DX,DY;
#if AWB_SAMPLE_POINT_SAVE
	SP_AwbPointPrintf("%d\t%d\n", AwbCT_Rgain[0], AwbCT_Bgain[0]);
	SP_AwbPointPrintf("%d\t%d\n", AwbCT_Rgain[1], AwbCT_Bgain[1]);
	SP_AwbPointPrintf("%d\t%d\n", AwbCT_Rgain[2], AwbCT_Bgain[2]);
#endif
#define AWB_SAVE_RAW 0
#if AWB_SAVE_RAW
{
	//UINT32 fd;
	static UINT8 idx=0;		
	UINT8 fname[40];
	sprintf(fname,"RAW%05u.RAW",idx++);
	printf("save info:%s,%u,%u\n",fname,info->width,info->height);		
	WriteFile( fname, ImageAddress,info->width* info->height*2);	
}
#endif

	cnt = Sum[0] = Sum[1] = Sum[2] = Sum[3] = 0;
	postWbMaxRGB[0] = postWbMaxRGB[1] = postWbMaxRGB[2] = 0;

	w = info->width / AWB_AREA_RATIO;
	h = info->height / AWB_AREA_RATIO;
	DX = w/xDim*2;
	DY = h/yDim*2;
	x0 = ((info->width - w) / 2) & 0xfffffffe;
	y0 = ((info->height - h) / 2) & 0xfffffffe;
	y0 = (y0 / info->rowOfs) * info->rowOfs + info->row0;
#if ((SP5K_SENSOR_TG_AD9920_SONY646==1)||(SP5K_SENSOR_TG_AD9000_SONY675==1)||(SP5K_SENSOR_TG_AD9000_SONY677==1))
	x0 = (x0 / 480) * 512;
	w = (w+511) / 512 * 512; //round down (this may be over the sensor width)
#endif
	printf("img=(%d %d) ROI=%d %d %d %d\n", info->width, info->height, x0, y0, w, h);
	printf("row info:%d %d/%d bayer=%d\n", info->rowOfs, info->row0, info->row1, info->bayerOrder);

	p0 = ImageAddress + y0*info->width;

	/*awbProcessParameterReset();*/
	sp5kAwbDefaultReset();

#if  ((SP5K_SENSOR_TG_AD9920_SONY646==1)||(SP5K_SENSOR_TG_AD9000_SONY675==1)||(SP5K_SENSOR_TG_AD9000_SONY677==1))
	x1 = 11;
	bayerIdx = info->bayerOrder;
	if((x0+x1)%2) {
	 bayerIdx +=1;
	}
	if(y0%2) {
	 bayerIdx +=2;
	}
	/* 5110 3H sampling */
	/* Note that 5110 3H pattern is 0 3 1 4 2 5 6 9 7 10 8 11
	while 5210 3H pattern is 0 3 6 9 1 4 7 10 2 5 8 11 12 15 ...,
	so 11 th & 12th pixels are neighboring bayer pixels for both case */
	for( y=y0; y<y0+h; y+=DY ) {
	 p = p0 + x0;
	 for( x=0; x<w; x+=512 ) {
		 for (i=x1 ; i<480 ; i+=DX) { /* i muststart from 5+6N */
			 rgb[0] = p[x+0 + i];
			 rgb[1] = p[x+1 + i];
			#if SHOW_POSTWB_SAMPLING_POINT
			 p[x+0 + i] = 0xffff; p[x+1 + i] = 0xfeff;
			#endif
			 p += (info->row1 - info->row0) * info->width;
			 rgb[2] = p[x+0 + i];
			 rgb[3] = p[x+1 + i];
			#if SHOW_POSTWB_SAMPLING_POINT
			 p[x+0 + i] = 0xfdff; p[x+1 + i] = 0xfcff;
			#endif
			 p -= (info->row1 - info->row0) * info->width;

			 r = rgb[ridx[bayerIdx]];
			 b = rgb[3-ridx[bayerIdx]];
			 gr = rgb[gidx[bayerIdx]];
			 gb = rgb[3-gidx[bayerIdx]];
			 g = ((gr>>1)+(gb>>1));

			 if(r>0 && g>0 && b>0){
				 Sum[0] += r;
				 Sum[1] += gr;
				 Sum[2] += gb;
				 Sum[3] += b;
				 sp5kAwbDefaultFilter(r>>6, g>>6, b>>6);
				 if(g>postWbMaxRGB[1]) {
					 postWbMaxRGB[0] = r;
					 postWbMaxRGB[1] = g;
					 postWbMaxRGB[2] = b;
					 postWbMaxRGB[3] = x;
					 postWbMaxRGB[4] = y;
				 }
				#if AWB_SAMPLE_POINT_SAVE
				 SP_AwbPointPrintf("%d\t%d\n", g*1024/r, g*1024/b);
				#endif
				 cnt++;
			 }
		 }
	 }
	 p0 += DY * info->width;
	}
#else /* normal bayer pattern */
	 printf("DX,DY = %d, %d\n", DX, DY);
	 OrigDX = Xstep = DX;
	 OrigDY = Ystep = DY;
	 WghtXS1 = w / 3;
	 WghtXE1 = w * 2 / 3;
	 WghtYS1 = h / 8;
	 WghtYE1 = h * 2 / 3;

	 WghtXS2 = w / 8;
	 WghtXE2 = w * 7 / 8;
	 WghtYS2 = h * 2 / 3;
	 WghtYE2 = h;

	bayerIdx = info->bayerOrder;
	if(x0%2) {
	 bayerIdx +=1;
	}
	if(y0%2) {
	 bayerIdx +=2;
	}
	spe_dx = (w/DX+6)/14;
	spe_dy = (h/DY+6)/14;
	spe_iy=1;
	spe_cnty=0;
	for( y=y0; y<y0+h; y+=Ystep ) {
	 p = p0 + x0;
	 spe_ix=1;
	 spe_iy--;
	 spe_cntx=0;
	 //printf("y=%d \n", y);
	 for ( x=0; x<w; x+=Xstep ) {
				 if (x >= WghtXS1 && x <= WghtXE1 && y >= WghtYS1 && y <= WghtYE1) {
					 Ystep = OrigDY/2;
					 Xstep = OrigDX/2;
				 } else if (x >= WghtXS2 && x <= WghtXE2 && y >= WghtYS2 && y <= WghtYE2) {
					 Ystep = OrigDY/4;
					 Xstep = OrigDX/4;
				 } else {
					 Ystep = OrigDY;
					 Xstep = OrigDX;
				 }

		 rgb[0] = p[x+0];
		 rgb[1] = p[x+1];
		#if SHOW_POSTWB_SAMPLING_POINT
		 p[x+0] = 0xffff; p[x+1] = 0xfeff;
		#endif
		 p += (info->row1 - info->row0) * info->width;
		 rgb[2] = p[x+0];
		 rgb[3] = p[x+1];
		#if SHOW_POSTWB_SAMPLING_POINT
		 p[x+0] = 0xfdff; p[x+1] = 0xfcff;
		#endif
		 p -= (info->row1 - info->row0) * info->width;

		 r = rgb[ridx[bayerIdx]];
		 b = rgb[3-ridx[bayerIdx]];
		 gr = rgb[gidx[bayerIdx]];
		 gb = rgb[3-gidx[bayerIdx]];
		 g = ((gr>>1)+(gb>>1));
		#if HOST_DBG
		 if(x<w/2+DX && x>w/2-DX && y<h/2+DY && y>h/2-DY )
			 printf("r,gr,gb,b = %d %d %d %d\n", r, gr, gb, b);
		#endif

		 if(r>0 && g>0 && b>0){
			extern UINT8 awbFilter_RGrBGb(UINT32 R,UINT32 Gr,UINT32 B,UINT32 Gb);
		 	awbFilter_RGrBGb(r,gr,b,gb);
			 Sum[0] += r;
			 Sum[1] += gr;
			 Sum[2] += gb;
			 Sum[3] += b;
			 /*awbWhitePointFilter(r>>6, g>>6, b>>6);*/
			 /*awbFilterV2(r>>6,g>>6,b>>6);*/			 
			 
			 if(g>postWbMaxRGB[1]) {
				 postWbMaxRGB[0] = r;
				 postWbMaxRGB[1] = g;
				 postWbMaxRGB[2] = b;
				 postWbMaxRGB[3] = x;
				 postWbMaxRGB[4] = y;
			 }
			#if AWB_SAMPLE_POINT_SAVE
			 SP_AwbPointPrintf("%d\t%d\n", g*1024/r, g*1024/b);
			#endif
			 cnt ++;
		 }

		 // record the sp in jpg makernote
		 spe_ix--;
		 /*if((spe_ix <= 0) && (spe_iy <= 0) && spe_cntx<14 && spe_cnty<14) {*/
			 spe_cntx++;
			 spe_ix = spe_dx;
			 //printf("%d ", x);
			 if(spe_buf_idx<AWB_SAMPLE_POINT_BUF_SIZE) {
				 awb_point_buf[spe_buf_idx++] = 0;/*gb>>8;*/
				 awb_point_buf[spe_buf_idx++] = b>>8;
				 awb_point_buf[spe_buf_idx++] = g>>8;
				 awb_point_buf[spe_buf_idx++] = r>>8;
				 temp3++;
			 }
		 /*}*/
	 }
	 /*p0 += DY * info->width;
			   */
	 p0 += Ystep * info->width;

	 if(spe_iy<=0) {
		 spe_iy = spe_dy;
		 spe_cnty++;
	 }
	}
#endif
	if(cnt) {
	 Sum[0] /= cnt;
	 Sum[1] /= cnt;
	 Sum[2] /= cnt;
	 Sum[3] /= cnt;
	}
	postWbSampleNumber = cnt;

#if 1//HOST_DBG
	printf("SUM : R(%d),Gr(%d)/Gb(%d),B(%d) cnt%d\n",Sum[0],Sum[1],Sum[2],Sum[3], cnt);
	printf("bufCnt:%u(w:%u,h:%u)\n",spe_buf_idx,info->width,info->height);
	//printf("sp in jpg (%d)\n", temp3);
#endif

	/*printf("%u,%u,%u,%u,%u,%u(%u,%u)\n",w,Xstep,h,Ystep,y0,spe_buf_idx,info->width,info->height);*/
	
	sp5kAwbDefaultDeciderPost();
	sp5kAwbDefaultInfo(awb_db_info,64,64,spe_buf_idx);//w/Xstep,(h-y0)/Ystep);

#if AWB_GRAY_WORD_ONLY
	rgain_avg = (Sum[1]+Sum[2])*1024/(Sum[0]*2);
	bgain_avg = (Sum[1]+Sum[2])*1024/(Sum[3]*2);
#endif
	return SUCCESS;
}
#endif

/*-------------------------------------------------------------------------
 *  Function Name : SP_CaptureDefaultPostWbSampling
 *  Description :
 *------------------------------------------------------------------------*/
UINT32 SP_CapturePostWb(UINT32 addr, UINT32 RawFmt, void* ImgSize)
{
	UINT32 one=0x400, min_gain;
	UINT16 awbLastMode=0x0000;
	UINT16 avgRgain,avgBgain;
	sp5kWbGain_t wb;
	appTimeDelay(50,1); 
	/*UINT32 t1=tmrMsTimeGet(),t2;*/
	/*printf("\nSnap AWB Start\n");*/
	appAeCb_t *paecb=appAeCbGet();
	UINT16 awbAeInfo[10];
	awbAeInfo[0]=(UINT16)paecb->lvidx;
	awbAeInfo[1]=(UINT16)paecb->luma;
	awbAeInfo[2]=(UINT16)aeCurContrastGet();
	awbCurAeInfoSet(awbAeInfo);

	awbLastMode=sp5kAwbParamGet(SP5K_AWB_MODE_SET);
	if(appStillCapFlashStatusGet()==1){
		sp5kAwbParamSet(SP5K_AWB_MODE_SET,SP5K_AWB_PROC_SNAP | SP5K_AWB_FLOW_FLASH);
	}else{
		sp5kAwbParamSet(SP5K_AWB_PROCESS_MODE,SP5K_AWB_PROC_SNAP);
	}
	/*extern UINT8 awbPrintfEn;
	awbPrintfEn= 1;*/
	SP_CaptureDefaultPostWbSampling(addr, RawFmt, ImgSize);
	/*awbPrintfEn = 0;*/
	avgRgain = sp5kAwbParamGet(SP5K_AWB_AVG_RGAIN);
	avgBgain = sp5kAwbParamGet(SP5K_AWB_AVG_BGAIN);
 #define __min(a,b) ((a)<(b))?(a):(b)
        min_gain = __min(__min(avgRgain, avgBgain), one);
if(min_gain==0){printf("\n\n>>>>Gain == 0\n\n");return 0;}
else{
	wb.rgain = avgRgain*0x400ul/min_gain;
	wb.bgain = avgBgain*0x400ul/min_gain;
	wb.grgain = one*0x400ul/min_gain;
	wb.gbgain = one*0x400ul/min_gain;

	printf("wbGain:%d %d/%d %d\n", wb.rgain, wb.grgain, wb.gbgain, wb.bgain);
	sp5kImgWbGainSet(&wb);

	/*gStillCB._awb.gGain = wb.grgain;*/
	gStillCB.awb.rGain = wb.rgain;
	gStillCB.awb.bGain = wb.bgain;
}
	sp5kAwbParamSet(SP5K_AWB_MODE_SET,awbLastMode);
	/*t2=tmrMsTimeGet();*/
	/*printf("\nSnap AWB End--time:%ld(%ld~%ld)\n",t2-t1,t1,t2);*/

	return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  Function Name : SP_AwbGainRead
 *  Description :
 *------------------------------------------------------------------------*/
void SP_AwbGainRead(void)
{
	UINT16 wbViewGainR, wbViewGainB, wbCapGainR, wbCapGainB;

	awbCurRBGainGet(&wbViewGainR, &wbViewGainB, &wbCapGainR, &wbCapGainB);
	rbGainHistory[rbGainHistoryIndex][0] = wbViewGainR;
	rbGainHistory[rbGainHistoryIndex++][1] = wbViewGainB;
	if(rbGainHistoryIndex >= RB_GAIN_HIS_TBL_SIZE) {
		rbGainHistoryIndex = 0;
	}
}

/*-------------------------------------------------------------------------
 *  Function Name : SP_AwbGainRead
 *  Description :
 *------------------------------------------------------------------------*/
void
SP_AvgRggbRead(
	UINT32 *r,
	UINT32 *gr,
	UINT32 *gb,
	UINT32 *b
)
{
	*r = rgb[0];
	*gr = rgb[1];
	*gb = rgb[2];
	*b = rgb[3];
}
