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
#include "common.h"
#include <stdio.h>
#include <string.h>
#include "api/sp5k_global_api.h"
#include "api/sp5k_capture_api.h"
#include "api/sp5k_cal_api.h"
#include "api/sp5k_disp_api.h"
#include "api/sp5k_gfx_api.h"
#include "api/sp5k_sensor_api.h"
#include "api/sp5k_os_api.h"
#include "middleware/PwrLog.h"
#include "app_com_api.h"
#include "app_com_def.h"
#include "app_user_setting.h"
#include "app_aaa.h"
#include "app_calib_private.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
enum { AAA_LAB_AE_PREVIEW, AAA_LAB_AE_CAPTURE, AAA_LAB_AWB, AAA_LAB_AF, AAALAB_FUNC_MAX };
enum { SUB_AE_EXP_LUMA, SUB_AE_AGC_LUMA, SUB_AE_EV_LUMA };

#define AAALAB_DEFAULT_SV	7
#define AAALAB_DEFAULT_TV	8

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define AAALAB_SUB_FUNC_MAX	5
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef void (*fpaaaLabFunc_t)(void);

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
static UINT32 appState0;
static UINT32 timer=TIMER_NULL;
UINT8 aaaLabFunc=0;
UINT8 aaaLabSubFunc=0;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
#if SP5K_ALGORITHM_AE //lester 20120402
extern SINT32 appAEALGLib_ResultGet(UINT32 param, aeResult_t *presult);
#endif
extern void appOsdLib_OsdLayerClearDraw(void);

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
static UINT16 aaaLabRawCbLuma[4];

UINT32 aaaLabRawCb(UINT32 addr, UINT32 fmt, void *imgSize)
{
	UINT16 *p=(UINT16 *)addr;
	UINT32 w=((sp5kImgSize_t *)imgSize)->width;
	UINT32 h=((sp5kImgSize_t *)imgSize)->height;
	UINT32 i,i0=w*(h/2-50)+w/2-50,x,y;
	UINT32 luma[4];
	DBG_PRINT("fmt=%d size=%dx%d\n", fmt, w, h);
	memset(luma, 0, 4*sizeof(UINT32));
	if (fmt==SP5K_CAPTURE_RAW_FMT_16) {
		for (y=0 ; y<100 ; y+=2) {
			i = i0 + y*w;
			for (x=0 ; x<100 ; x+=2) {
				luma[0] += p[i+x];
				luma[1] += p[i+x+1];
				luma[2] += p[w+i+x];
				luma[3] += p[w+i+x+1];
			}
		}
	}
	for (i=0 ; i<4 ; i++)
		aaaLabRawCbLuma[i] = luma[i] / (100*100/4);
	return 0;
}

UINT16 aaaLabAeLumaGet(BOOL fCapture)
{
	UINT32 i,sum;
	if (fCapture) {
		sp5kStillCapRawCallbackSet(aaaLabRawCb); 
		sp5kCalibrationCfgSet(SP5K_CALIBRATION_CALLBACK, 1);
	    appHostMsgWait(SP5K_MSG_CALI_COMPLETE, NULL, 5000); 
	    sp5kCalibrationCfgSet(SP5K_CALIBRATION_OFF, 1);
		appHostMsgWaitExact(SP5K_MSG_MODE_READY, SP5K_MODE_STILL_PREVIEW, 2000);
		for (i=sum=0 ; i<4 ; i++)
			sum += aaaLabRawCbLuma[i];
		return sum/4;
	} else {
		sp5kAeResult_t aeResult;
		UINT32 param,ret;
		for (i=0 ; i<20 ; i++) {
			if (appHostMsgWait(SP5K_MSG_AE_READY, &param, 500) == SUCCESS) {

			#if SP5K_ALGORITHM_AE //lester 20120402
				ret = appAEALGLib_ResultGet(param,&aeResult);
			#else
				ret = sp5kAePreviewResultGet(param, &aeResult);
			#endif

				if (ret == SUCCESS && (aeResult.valid & AAA_INFO_RESULT_AE_VALID))
					break;
			}
		}
		return i<20 ? aeResult.aeLuma : 0;
	}
}

void aaaLabExpAgcSet(BOOL fCapture, UINT32 exp, UINT32 agc)
{
	if (fCapture)
		sp5kStillCapExpAgcSet(exp, agc);
	else
		sp5kPreviewExpAgcSet(exp, agc);
}

void aaaLabExpAgcGet(BOOL fCapture, UINT32 *pExp, UINT32 *pAgc)
{
	if (fCapture)
		sp5kStillCapExpAgcGet(pExp, pAgc);
	else
		sp5kPreviewExpAgcGet(pExp, pAgc);
}

#define AE_CAP_LUMA_MIN			512
#define AE_CAP_TARGET			12800
#define AE_CAP_DELTA_EV_MIN		(-5*evUnit)

SINT32 aaaLabAeGetEvDelta(UINT16 luma, UINT16 evUnit)
{
	SINT32 val, step_num, step_den;
	SINT32 p,d,round,sign;
	if (luma < AE_CAP_LUMA_MIN ||
		(val = log2xyND(luma, AE_CAP_TARGET, &step_num, &step_den))==(SINT32)LOG2_DOMAIN_ERR) {
		val = AE_CAP_DELTA_EV_MIN;
	} else {
		p = step_num * evUnit;
		d = step_den;
		sign = p >> 31;
		/*round = d/2,p>=0; -(d/2),p<0*/
		round = (sign ^ (SINT32)((UINT32)d/2)) - sign;
		val = (p + round) / d;
		if (val > 127) val = 127;
		else if (val < AE_CAP_DELTA_EV_MIN) val = AE_CAP_DELTA_EV_MIN;
	}
	return val;
}

void aaaLabAeExpStableWait(BOOL fCapture)
{
	int i;
	SINT32 step;

	if (fCapture) {
		SINT32 exp,exp0;
		UINT16 luma;
		for (i=0 ; i<20 ; i++) {
			sp5kStillCapExpAgcGet(&exp0, NULL);
			luma = aaaLabAeLumaGet(1);
			step = aaaLabAeGetEvDelta(luma, evUnit.evUnit);
			DBG_PRINT("exp0=%d luma=%d step=%d ", exp0, luma, step);
			if (-evUnit.evUnit/4<=step && step<=evUnit.evUnit/4)
				break;
			exp = exp0 + step;
			if (exp < expLimitCap.expIdxShortMin) {
				exp = expLimitCap.expIdxShortMin;
			} else if (exp > expLimitCap.expIdxMax) {
				exp = expLimitCap.expIdxMax;
			}
			DBG_PRINT("exp=%d\n", exp);
			if (exp==exp0) break;
			sp5kStillCapExpAgcSet(exp, SP5K_AE_EXP_AGC_DONT_CHANGE);
		}
		DBG_PRINT("done\n");
	} else {
		UINT32 param;
		for (i=0 ; i<20 ; i++) {
			appHostMsgWait(SP5K_MSG_AE_READY, &param, 500);
			appAePreviewExpAdjust(param, &step);
			if (-evUnit.evUnit/4<step && step<evUnit.evUnit/4)
				break;
		}
	}
}

/* wait user to adjust light box to meet specified target */
/* press any button to continue */
void aaaLabEvBoxAdjustWait(BOOL fLumaPrompt)
{
	UINT32 msg,param;
	sp5kAeResult_t aeResult;
	while (1) {
		sp5kHostMsgReceive(&msg, &param);
		if (IS_SP5K_KEY_PRESS_MSG(msg, param))
			break;
		if (msg==SP5K_MSG_AE_READY) {

	#if SP5K_ALGORITHM_AE //lester 20120402
		appAEALGLib_ResultGet(param,&aeResult);
	#else
			sp5kAePreviewResultGet(param, &aeResult);
	#endif

			if (fLumaPrompt) printf("\r%d ", aeResult.aeLuma);
		}
	}
	if (fLumaPrompt) printf("\r");
}

#define EXPAGC_LUM_ITER_MAX	10

#define LUMA_DARK(fCap)		(((fCap)?65536:1024)/20)

/* set AGC to max(ISO100, minAGC) and start from EV7 and up to EV15 
   for each EV, scan exp within +-4 EV range */
void aaaLabAeExpLum(BOOL fCapture)
{
	UINT16 luma,*lumaBuf;
	SINT32 exp0,exp,min,max,expDark;
	UINT32 ev,agc,i,expTblCnt;
	UINT8 evtbl[EXPAGC_LUM_ITER_MAX];
	BOOL fDark;
	sp5kExpLimit_t *lmt;

	lmt = fCapture ? &expLimitCap : &expLimitPv;
	expTblCnt = lmt->expIdxMax - lmt->expIdxShortMin + 1;
	lumaBuf = sp5kMalloc(EXPAGC_LUM_ITER_MAX * expTblCnt * sizeof(luma));
	memset(lumaBuf, 0xff, EXPAGC_LUM_ITER_MAX * expTblCnt * sizeof(luma));
	agc  = fCapture ? SV_TO_CAP_AGCIDX(AAALAB_DEFAULT_SV) : SV_TO_VIEW_AGCIDX(AAALAB_DEFAULT_SV);
	agc = agc > agcLimitPv.agcIdxMin ? agc : agcLimitPv.agcIdxMin;
	aaaLabExpAgcSet(fCapture, SP5K_AE_EXP_AGC_DONT_CHANGE, agc);
	ev = 7;
	for (i=0 ; i<EXPAGC_LUM_ITER_MAX ; i++) {
		evtbl[i] = ev;
		printf("Set EV %d\n", ev);
		aaaLabEvBoxAdjustWait(0);
		printf("Do AE\n");
		aaaLabAeExpStableWait(fCapture);
		aaaLabExpAgcGet(fCapture, &exp0, NULL);
		min = exp0 - 4*evUnit.evUnit;
		max = exp0 + 4*evUnit.evUnit;
		if (min<lmt->expIdxShortMin) min = lmt->expIdxShortMin;
		if (max>lmt->expIdxMax) max = lmt->expIdxMax;
		printf("Scan expidx %d-%d (%d)\n", min, max, exp0);
		expDark = exp0 + 3*evUnit.evUnit; 
		if (expDark>lmt->expIdxMax) expDark = lmt->expIdxMax;
		fDark = 0;
		for (exp=min ; exp<=max ; exp++) {
			aaaLabExpAgcSet(fCapture, exp, SP5K_AE_EXP_AGC_DONT_CHANGE);
			luma = aaaLabAeLumaGet(fCapture);
			printf("%d %d\n", exp, luma);
			if (lumaBuf) {
				lumaBuf[i*expTblCnt*sizeof(luma) + exp-lmt->expIdxShortMin] = luma;
			}
			if (!fDark && luma<LUMA_DARK(fCapture)) {
				fDark = 1;
				expDark = exp;
			}
		}
		printf("dark:%d expdark=%d\n", fDark, expDark);
		if (ev>=15) break; // || (exp>max && !fDark)) break;
		ev += 2;
		if (ev>=17) ev = 15;
	}
	if (lumaBuf) {
		printf("=== exp scan result ===\n");
		printf("%s exp scan (agc=%d)\n", fCapture ? "Capture" : "Preview", agc);
		max = i>EXPAGC_LUM_ITER_MAX ? EXPAGC_LUM_ITER_MAX : i+1;
		printf("expidx");
		for (i=0 ; i<(UINT32)max ; i++)
			printf(",%d", evtbl[i]);
		printf("\n");
		for (exp=lmt->expIdxShortMin ; exp<=lmt->expIdxMax ; exp++) {
			printf("%d", exp);
			for (i=0 ; i<(UINT32)max ; i++) {
				if (lumaBuf[i*expTblCnt*sizeof(luma) + exp-lmt->expIdxShortMin]==0xffff)
					printf(",");
				else
					printf(",%d", lumaBuf[i*expTblCnt*sizeof(luma) + exp-lmt->expIdxShortMin]);
			}
			printf("\n");
		}
		printf("\n");
		sp5kFree(lumaBuf);
	}
	printf("=== exp test end ===\n");
}

#define LUMA_OVER(fCap)		(((fCap)?65536:1024)*19/20)

/* start from EV7 and agc=min(minAGC+2*EV, maxAGC), up to EV15 
   for each EV, scan agc within +-4 EV range */
void aaaLabAeAgcLum(BOOL fCapture)
{
	UINT16 luma,*lumaBuf;
	UINT32 agc0,min,max,agcOver;
	UINT32 ev,agc,i,agcTblCnt;
	UINT8 evtbl[EXPAGC_LUM_ITER_MAX];
	SINT32 exptbl[EXPAGC_LUM_ITER_MAX];
	BOOL fOver;
	sp5kAgcLimit_t *lmt;

	lmt = fCapture ? &agcLimitCap : &agcLimitPv;
	agcTblCnt = lmt->agcIdxMax - lmt->agcIdxMin + 1;
	lumaBuf = sp5kMalloc(EXPAGC_LUM_ITER_MAX * agcTblCnt * sizeof(luma));
	memset(lumaBuf, 0xff, EXPAGC_LUM_ITER_MAX * agcTblCnt * sizeof(luma));
	agc = lmt->agcIdxMin+2*evUnit.evUnit;
	if (agc>lmt->agcIdxMax) agc = lmt->agcIdxMax;
	aaaLabExpAgcSet(fCapture, SP5K_AE_EXP_AGC_DONT_CHANGE, agc);
	ev = 7;
	for (i=0 ; i<EXPAGC_LUM_ITER_MAX ; i++) {
		evtbl[i] = ev;
		printf("Set EV %d\n", ev);
		aaaLabEvBoxAdjustWait(0);
		printf("Do AE\n");
		aaaLabAeExpStableWait(fCapture);
		aaaLabExpAgcGet(fCapture, &exptbl[i], &agc0);
		min = agc0 - 4*evUnit.evUnit;
		max = agc0 + 4*evUnit.evUnit;
		if (min<lmt->agcIdxMin) min = lmt->agcIdxMin;
		if (max>lmt->agcIdxMax) max = lmt->agcIdxMax;
		printf("Scan agcidx %d-%d (%d) with exp=%d\n", min, max, agc0, exptbl[i]);
		agcOver = agc0 + 3*evUnit.evUnit; 
		if (agcOver>lmt->agcIdxMax) agcOver = lmt->agcIdxMax;
		fOver = 0;
		for (agc=min ; agc<=max ; agc++) {
			aaaLabExpAgcSet(fCapture, SP5K_AE_EXP_AGC_DONT_CHANGE, agc);
			luma = aaaLabAeLumaGet(fCapture);
			printf("%d %d\n", agc, luma);
			if (lumaBuf) {
				lumaBuf[i*agcTblCnt*sizeof(luma) + agc-lmt->agcIdxMin] = luma;
			}
			if (!fOver && luma>LUMA_OVER(fCapture)) {
				fOver = 1;
				agcOver = agc;
			}
		}
		printf("over:%d agcover=%d\n", fOver, agcOver);
		if (ev>=15) break; // || (agc>max && !fOver)
		ev += 2;
		if (ev>=17) ev = 15;
	}
	if (lumaBuf) {
		printf("=== agc scan result ===\n");
		printf("%s agc scan\n", fCapture ? "Capture" : "Preview");
		max = i>EXPAGC_LUM_ITER_MAX ? EXPAGC_LUM_ITER_MAX : i+1;
		printf("ev");
		for (i=0 ; i<max ; i++)
			printf(",%d", evtbl[i]);
		printf("\n");
		printf("expidx");
		for (i=0 ; i<max ; i++)
			printf(",%d", exptbl[i]);
		printf("\n");
		for (agc=lmt->agcIdxMin ; agc<=lmt->agcIdxMax ; agc++) {
			printf("%d", agc);
			for (i=0 ; i<max ; i++) {
				if (lumaBuf[i*agcTblCnt*sizeof(luma) + agc-lmt->agcIdxMin]==0xffff)
					printf(",");
				else
					printf(",%d", lumaBuf[i*agcTblCnt*sizeof(luma) + agc-lmt->agcIdxMin]);
			}
			printf("\n");
		}
		printf("\n");
		sp5kFree(lumaBuf);
	}
	printf("=== agc test end ===\n");
}

void aaaLabAeEvLum(BOOL fCapture)
{
	UINT16 luma,ev;
	SINT32 exp,exptbl[2][3];
	UINT32 agc,agctbl[3];
	UINT8 evi,agci;
	sp5kExpLimit_t *expLmt;

	expLmt = fCapture ? &expLimitCap : &expLimitPv;
	agctbl[0] = fCapture ? SV_TO_CAP_AGCIDX(5) : SV_TO_VIEW_AGCIDX(5);
	agctbl[1] = fCapture ? SV_TO_CAP_AGCIDX(7) : SV_TO_VIEW_AGCIDX(7);
	agctbl[2] = fCapture ? SV_TO_CAP_AGCIDX(9) : SV_TO_VIEW_AGCIDX(9);
	for (ev=80,evi=0 ; ev<=130 ; ev+=50,evi++) {
		printf("Set to EV%d\n", ev/10);
		appHostMsgWait(SP5K_MSGX_KEY, NULL, 20000);
		for (agci=0 ; agci<3 ; agci++) {
			aaaLabExpAgcSet(fCapture, TV_TO_EXPIDX(4), agctbl[agci]);
			aaaLabAeExpStableWait(0);
			aaaLabExpAgcGet(fCapture, &exp, &agc);
			exptbl[evi][agci] = exp;
			if (exp<expLmt->expIdxTblMin)
				exp = expLmt->expIdxTblMin;
			else if (exp>expLmt->expIdxMax)
				exp = expLmt->expIdxMax;
			exptbl[evi][agci] = exp;
		}
	}
	for (agci=0 ; agci<2 ; agci++)
		for (evi=0 ; evi<2 ; evi++)
			printf("%d/%d ", exptbl[evi][agci], agctbl[agci]);
	printf("\n");

	for (ev=40 ; ev<=170 ; ev+=2) {
		printf("%d.%d ", ev/10, ev%10);
		appHostMsgWait(SP5K_MSGX_KEY, NULL, 20000);
		for (agci=0 ; agci<3 ; agci++) {
			for (evi=0 ; evi<2 ; evi++) {
				aaaLabExpAgcSet(fCapture, exptbl[evi][agci], agctbl[agci]);
				luma = aaaLabAeLumaGet(fCapture);
				printf("%d ", luma);
			}
		}
		printf("\n");
	}
}

void aaaLabAeOB(BOOL fCapture)
{
	UINT16 luma;
	SINT32 exp;
	UINT32 agc;
	sp5kExpLimit_t *expLmt;
	sp5kAgcLimit_t *agcLmt;

	expLmt = fCapture ? &expLimitCap : &expLimitPv;
	agcLmt = fCapture ? &agcLimitCap : &agcLimitPv;
	printf("=== Set EV 0 ===\n");
	aaaLabEvBoxAdjustWait(0);
	printf("exp\\agc");
	for (agc=agcLmt->agcIdxMin ; agc<=agcLmt->agcIdxMax ; agc+=4)
		printf(",%d", agc);
	printf("=\n");
	/* for (exp=expLmt->expIdxTblMin ; exp<=expLmt->expIdxMax ; exp+=8) { */
	for (exp=24 ; exp<=32 ; exp++) {
		printf("%d", exp);
		for (agc=agcLmt->agcIdxMin ; agc<=agcLmt->agcIdxMax ; agc+=4) {
			aaaLabExpAgcSet(fCapture, exp, agc);
			luma = aaaLabAeLumaGet(fCapture);
			printf(",%d", luma);
		}
		printf("=\n");
	}
	printf("=== OB test end ===\n");
}


/* preview ae lab */
void aaaLabAePreviewExpLum(void)
{
	aaaLabAeExpLum(0);
}

void aaaLabAePreviewAgcLum(void)
{
	aaaLabAeAgcLum(0);
}

void aaaLabAePreviewEvLum(void)
{
	aaaLabAeEvLum(0);
}

void aaaLabAePreviewOB(void)
{
	aaaLabAeOB(0);
}

/* capture ae lab */
void aaaLabAeCaptureExpLum(void)
{
	aaaLabAeExpLum(1);
}

void aaaLabAeCaptureAgcLum(void)
{
	aaaLabAeAgcLum(1);
}

void aaaLabAeCaptureEvLum(void)
{
	aaaLabAeEvLum(1);
}

void aaaLabAeCaptureOB(void)
{
	aaaLabAeOB(1);
}

/* function table */
fpaaaLabFunc_t fpaaaLabFunc[AAALAB_FUNC_MAX][AAALAB_SUB_FUNC_MAX]={
	{ aaaLabAePreviewExpLum, aaaLabAePreviewAgcLum, aaaLabAePreviewEvLum, aaaLabAePreviewOB, NULL },
	{ aaaLabAeCaptureExpLum, aaaLabAeCaptureAgcLum, aaaLabAeCaptureEvLum, aaaLabAeCaptureOB, NULL },
	{ aaaLabAePreviewEvLum, aaaLabAePreviewExpLum, aaaLabAePreviewAgcLum, NULL, NULL },
	{ aaaLabAePreviewEvLum, aaaLabAePreviewExpLum, aaaLabAePreviewAgcLum, NULL, NULL },
};

/* calib state ctrl */
void 
appModeAaaLab_OnMsg(
	UINT32 msg,	
	UINT32 param)
{
	if (HOST_DBG && msg!=SP5K_MSG_AE_READY && msg!=SP5K_MSG_AWB_READY)
		DBG_PRINT("aaaLab msg:%x %x\n", msg, param);
	switch (msg) {
	case SP5K_MSG_TIMER:
		if (timer == param) {
			appTimerClear(&timer);
			if (aaaLabSubFunc<AAALAB_SUB_FUNC_MAX) {
				if (fpaaaLabFunc[aaaLabFunc][aaaLabSubFunc])
					fpaaaLabFunc[aaaLabFunc][aaaLabSubFunc]();
			}
			appStateChange(appState0, STATE_PARAM_NORMAL_INIT); /* switch back to prev state before aaalab */
		}
		break;
	}
}

void 
appModeAaaLab_OnEnter(
)
{
	appState0 = appPreviousStateGet();
	/*s Modify by Aries 09/07/21*/
	#if 1
	appOsdLib_OsdLayerClearDraw();
	#else
	sp5kGfxPageClear(0, 0);
	#endif
	/*e Modify by Aries 09/07/21*/
	sp5kPreviewExpAgcSet(TV_TO_EXPIDX(AAALAB_DEFAULT_TV), SV_TO_VIEW_AGCIDX(AAALAB_DEFAULT_SV));
	timer = appTimerSet(1000, "aaaLab");
}

void aaaLabDo(UINT32 func, UINT32 subfunc, UINT32 sensormode_pv, UINT32 sensormode_snap)
{
	if (func<AAALAB_SUB_FUNC_MAX && fpaaaLabFunc[func][subfunc]) {
		aaaLabFunc = func;
		aaaLabSubFunc = subfunc;
		#if 0
		sp5kSensorModeCfgSet(SP5K_MODE_STILL_PREVIEW, sensormode_pv);
		sp5kSensorModeCfgSet(SP5K_MODE_STILL_SNAP, sensormode_snap);
		sp5kAeEvUnitGet(&evUnit);
		sp5kAeExpLimitGet(sensormode_pv, &expLimitPv);
		sp5kAeAgcLimitGet(sensormode_pv, &agcLimitPv);
		sp5kAeExpLimitGet(sensormode_snap, &expLimitCap);
		sp5kAeAgcLimitGet(sensormode_snap, &agcLimitCap);
		#endif
		printf("Unit=%d expIdx0=%d agcIdx0=%d\n", evUnit.evUnit, evUnit.expIdx0Unit, evUnit.agcIdx0Unit);
		printf("pv exp limit:%d %d %d %d\n", expLimitPv.expIdxSysMin, expLimitPv.expIdxTblMin, expLimitPv.expIdxShortMin, expLimitPv.expIdxMax);
		printf("pv agc limit:%d %d\n", agcLimitPv.agcIdxMin, agcLimitPv.agcIdxMax);
		printf("snap exp limit:%d %d %d %d\n", expLimitCap.expIdxSysMin, expLimitCap.expIdxTblMin, expLimitCap.expIdxShortMin, expLimitCap.expIdxMax);
		printf("snap agc limit:%d %d\n", agcLimitCap.agcIdxMin, agcLimitCap.agcIdxMax);
		CalibMode = CALIB_MODE_AAA_LAB;
		appStateChange(APP_STATE_CALIBRATION, STATE_PARAM_NORMAL_INIT);
	}
}

