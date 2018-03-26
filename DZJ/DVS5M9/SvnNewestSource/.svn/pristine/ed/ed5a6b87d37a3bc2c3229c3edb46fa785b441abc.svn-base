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
#include "sp5k_fs_api.h"
#include "app_hw_def.h"
#include "customization/dev_init.h"
#include "gpio_custom.h"
#include "app_btn_def.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define ADC_TYPE_BTN	0
#define ADC_TYPE_DIAL	1

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef struct {
	UINT8 type;
	UINT8 channel;
	UINT8 adcDev;
	UINT8 adcTblCnt;
	UINT16 *adcTbl;
	UINT8 **id;
} adcBtnGrp_t;
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
	static UINT16 adcBtnAADvalue[ADC_BTN_A_NUM] = {ADC_BTN_A_S1,ADC_BTN_A_S2,ADC_BTN_A_S3,ADC_BTN_A_S4};
 	static UINT16 adcBtnBADvalue[ADC_BTN_B_NUM] = {ADC_BTN_B_S1,ADC_BTN_B_S2,ADC_BTN_B_S3,ADC_BTN_B_S4};
	static UINT8 *adcBtnAId[ADC_BTN_A_NUM] = { "UP", "DOWN", "LEFT", "RIGHT" };
	static UINT8 *adcBtnBId[ADC_BTN_B_NUM] = { "OK", "F1", "F2", "F3" }; 
	static adcBtnGrp_t adcBtnGrp[]=
	{
		{ ADC_TYPE_BTN, ADC_BTN_A_CH, ADC_BTN_A_DEV, COUNT_OF(adcBtnAADvalue), adcBtnAADvalue, adcBtnAId },
		{ ADC_TYPE_BTN, ADC_BTN_B_CH, ADC_BTN_B_DEV, COUNT_OF(adcBtnBADvalue), adcBtnBADvalue, adcBtnBId }
	};
	static BOOL btnEnable = FALSE;

#define ADC_BTN_GRP_MAX		COUNT_OF(adcBtnGrp)


/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
int getch(void);
void appKeyInitStatusGet(void);
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

void appGpioBtnInit()
{
	DBG_PRINT("%s \n", __FUNCTION__);

	#ifdef HW_DVS5M2
	
	/* GPIO 00 -> POWER */
	sp5kGpioCfgSet(GRP_BUTTON_POWER, 1<<CFG_BUTTON_POWER, INPUT);
	sp5kButtonGpioCfgSet(BTN_ID_POWER, GRP_BUTTON_POWER, CFG_BUTTON_POWER, ACTIVE_HIGH, GPIO_DBNC, 0);
	/* GPIO 01 -> OK */
	sp5kGpioCfgSet(GRP_BUTTON_OK, 1<<CFG_BUTTON_OK, INPUT);
	sp5kButtonGpioCfgSet(BTN_ID_OK, GRP_BUTTON_OK, CFG_BUTTON_OK, ACTIVE_LOW, GPIO_DBNC, 0);

	//TV?

	//USB??

	//HDMI??
	
	return;

	#endif
	
#if !EZ_DBG_HW
	#ifdef HW_PRJ_6330
	sp5kGpioPullSet(SP5K_GPIO_GRP_GEN, (1 << 11), 0);
	sp5kGpioPullSet(SP5K_GPIO_GRP_GEN, (1 << 12), 0);
	sp5kGpioPullSet(SP5K_GPIO_GRP_GEN, (1 << 10), 0);
	sp5kGpioPullSet(SP5K_GPIO_GRP_GEN, (1 << 8), 0);
	#endif
	
	/* GPIO 11 -> TELE */
	sp5kGpioCfgSet(SP5K_GPIO_GRP_GEN, 1<<11, INPUT);
	sp5kButtonGpioCfgSet(BTN_ID_TELE, SP5K_GPIO_GRP_GEN, 11, ACTIVE_LOW, GPIO_DBNC, GPIO_RPT);
	/* GPIO 12 -> WIDE */
	sp5kGpioCfgSet(SP5K_GPIO_GRP_GEN, 1<<12, INPUT);
	sp5kButtonGpioCfgSet(BTN_ID_WIDE, SP5K_GPIO_GRP_GEN, 12, ACTIVE_LOW, GPIO_DBNC, GPIO_RPT);
	/* GPIO 10 -> S1 */
	sp5kGpioCfgSet(SP5K_GPIO_GRP_GEN, 1<<10, INPUT);
	sp5kButtonGpioCfgSet(BTN_ID_S1, SP5K_GPIO_GRP_GEN, 10, ACTIVE_LOW, GPIO_DBNC, 0);
	/* GPIO 8-> S2 */
	sp5kGpioCfgSet(SP5K_GPIO_GRP_GEN, 1<<8, INPUT);
	sp5kButtonGpioCfgSet(BTN_ID_S2, SP5K_GPIO_GRP_GEN, 8, ACTIVE_LOW, GPIO_DBNC, 0);
	/* FMGPIO 29 -> TV */
	#ifdef HW_PRJ_6330
	sp5kGpioCfgSet(SP5K_GPIO_GRP_GEN, 1<<13, INPUT);
	sp5kButtonGpioCfgSet(BTN_ID_TV, SP5K_GPIO_GRP_GEN, 13, ACTIVE_LOW, 50, 0);
	#else
	#if !ICAT_WIFI
	sp5kGpioCfgSet(SP5K_GPIO_GRP_FML, 1<<29, INPUT);
	sp5kButtonGpioCfgSet(BTN_ID_TV, SP5K_GPIO_GRP_FML, 29, ACTIVE_LOW, 50, 0);
	#endif
	#endif

	
	/* SP5K_GPIO_GRP_POCU 1 -> PB */
	
	sp5kButtonGpioCfgSet(BTN_ID_PB, SP5K_GPIO_GRP_POCU, 1, ACTIVE_HIGH, GPIO_DBNC, 0);
#endif
}


void appAdcBtnInit()
{
#if !EZ_DBG_HW
	UINT32 i,grp,btnIdx;
	/* config all ADC button */
	btnIdx = 0;
	for (grp=0 ; grp<ADC_BTN_GRP_MAX ; grp++)
	{
		for (i=0 ; i<adcBtnGrp[grp].adcTblCnt ; i++, btnIdx++) 
		{
			UINT32 dev,adc=adcBtnGrp[grp].adcTbl[i];
			DBG_PRINT("i=%d adc=%d   adc>>10=%d  adcBtnAADvalue[i]=%d",i,adc ,adc>>10 ,adcBtnAADvalue[i]);
			#if SPCA6330
			if (adc>>12) 
			{
			#else
			if (adc>>10) 
			{
			#endif
				dev = (adc>>10) << 1;
				adc &= 0x3ff;
				DBG_PRINT("	->->dev=%d   adc=%d\n",i,dev,adc);
			}
			else  
			{
				dev = adcBtnGrp[grp].adcDev;
				printf("	&&dev=%d   \n",dev);
			}
			DBG_PRINT(" %d    %d  %d    %d    %d  %d\n",btnIdx, adcBtnGrp[grp].channel, adc-dev, adc+dev, ADC_DBNC, ADC_RPT);
			sp5kButtonAdcCfgSet(btnIdx, adcBtnGrp[grp].channel, adc-dev, adc+dev, ADC_DBNC, ADC_RPT);
		}
	}
#endif		
}

void
appBtnControl(
	UINT32 btnID,
	UINT32 isEnable
)
{
	UINT32 i, msk;

	/*-- Check GPIO Button group
	 */
	if (btnID & BTN_GRP_GPIO) 
	{
		DBG_PRINT("BTN (%s) : GPIO group ! \n", (isEnable ? "Enable" : "Disable") );
		DBG_PRINT("GPIO_BTN_NUM=%d   BTN_ID_GPIO_TOTAL=%d\n",GPIO_BTN_NUM,BTN_ID_GPIO_TOTAL);
		/*-- double check button configuratin !! */
		if (GPIO_BTN_NUM != BTN_ID_GPIO_TOTAL) 
		{
			__FUNC_TRACK__;
			GPrintf(" !! FATAL !!, Check GPIO BTN CFG !!");
		}
		else 
		{
			for (i=0; i<BTN_ID_GPIO_TOTAL; i++) 
			{
				msk = 1 << i;
				if (btnID & msk) 
				{
					DBG_PRINT("GPIO BTN i(%d), msk(0x%08x) \n", i, msk);
					sp5kButtonEnable(SP5K_BUTTON_TYPE_GPIO, i, (isEnable ? ENABLE : DISABLE));
				}
			}
		}
	}

	/*-- Check ADC Button group
	 */
	if (btnID & BTN_GRP_ADC) 
	{
		DBG_PRINT("BTN (%s) : ADC group ! \n", (isEnable ? "Enable" : "Disable") );
		DBG_PRINT("ADC_BTN_TOTAL=%d   BTN_ID_ADC_TOTAL=%d\n",ADC_BTN_TOTAL,BTN_ID_ADC_TOTAL);
		/*-- double check button configuratin !! */
		if (ADC_BTN_TOTAL != BTN_ID_ADC_TOTAL) 
		{
			__FUNC_TRACK__;
			GPrintf(" !! FATAL !!, Check ADC BTN  CFG !!");
			return;
		}
		else
		{
			for (i=0; i<BTN_ID_ADC_TOTAL; i++) 
			{
				msk = 1 << i;
				if (btnID & msk)
				{
					DBG_PRINT("ADC BTN i(%d), msk(0x%08x) \n", i, msk);
					sp5kButtonEnable(SP5K_BUTTON_TYPE_ADC, i, (isEnable ? ENABLE : DISABLE));
				}
			}
		}
	}

}


void
appBtnEnable(
	UINT32 btnID
)
{
	BPrintf("Button Enable\n ");
	btnEnable = TRUE;
	appBtnControl(btnID, ENABLE);
}


void
appBtnDisable(
	UINT32 btnID
)
{
	BPrintf("Button Disable\n ");
	btnEnable = FALSE;
	appBtnControl(btnID, DISABLE);
}


BOOL appBtnEnableGet(void)
{
	return btnEnable;
}


UINT32
adcBtnIoLevelGet(
	UINT32 btnID
)
{
	UINT32 ret, ch_A, ch_B;
	UINT32 dev_A=ADC_BTN_A_DEV;
	UINT32 dev_B=ADC_BTN_B_DEV;

	sp5kAdcRead(ADC_BTN_A_CH, &ch_A);
	sp5kAdcRead(ADC_BTN_B_CH, &ch_B);

	switch (btnID) 
	{
	/*--- ADC Channel A ------------*/
	case BTN_UP:
		if (ADC_BTN_A_S1-dev_A < ch_A && ch_A < ADC_BTN_A_S1+dev_A)
		{
			ret = 1;
		}
		else
		{
			ret = 0;
		}
		break;
	case BTN_DOWN:
		if (ADC_BTN_A_S2-dev_A < ch_A && ch_A < ADC_BTN_A_S2+dev_A)
		{	
			ret = 1;
		}
		else
		{
			ret = 0;
		}
		break;
	case BTN_LEFT:
		if (ADC_BTN_A_S3-dev_A < ch_A && ch_A < ADC_BTN_A_S3+dev_A)
		{
			ret = 1;
		}
		else
		{
			ret = 0;
		}
		break;
	case BTN_RIGHT:
		if (ADC_BTN_A_S4-dev_A < ch_A && ch_A< ADC_BTN_A_S4+dev_A)
		{
			ret = 1;
		}
		else
		{
			ret = 0;
		}
		break;
	/*--- ADC Channel B ------------*/
	case BTN_SET: /* OK */
		if (ADC_BTN_B_S1-dev_B < ch_B && ch_B < ADC_BTN_B_S1+dev_B)
		{
			ret = 1;
		}
		else
		{
			ret = 0;
		}
		break;
	case BTN_DEL: /* F1 */
		if (ADC_BTN_B_S2-dev_B < ch_B && ch_B < ADC_BTN_B_S2+dev_B)
		{
			ret = 1;
		}
		else
		{
			ret = 0;
		}
		break;
	case BTN_MENU: /* F2 */
		if (ADC_BTN_B_S3-dev_B < ch_B && ch_B < ADC_BTN_B_S3+dev_B)
		{
			ret = 1;
		}
		else
		{
			ret = 0;
		}
		break;
	case BTN_MODE: /* F3 */
		if (ADC_BTN_B_S4-dev_B < ch_B && ch_B < ADC_BTN_B_S4+dev_B)
		{
			ret = 1;
		}
		else
		{
			ret = 0;
		}
		break;
	default:
		DBG_PRINT("%s: Button(%d) not supported !!\n", __FUNCTION__, btnID);
		ret = 0;
		break;
	}
	return ret;
}


/**************************************************************************
 *                                                                        *
 *  Function Name: appBtnInit                                        *
 *                                                                        *
 **************************************************************************/
void
appBtnInit(
	void
)
{	
	/* you can open this when turning ADC value
	
	UINT32 val;
	sp5kAdcRead(1, &val); 
	printf("@@4 way adc value=%lu\n",val);
	sp5kAdcRead(7, &val); 
	printf("@@ function adc value=%lu\n",val);
	return;
	*/
	appGpioBtnInit();
	appAdcBtnInit();
	appKeyInitStatusGet();
	appBtnEnable(BTN_ALL);
}	


#if Calibration_ADC_KEY
UINT32 appBtnLoad()
{

	UINT32 fd,grp;
	fd = sp5kFsFileOpen("A:\\RO_RES\\BTNADC.BIN", SP5K_FS_OPEN_RDONLY);
	if (fd==0)
	{
		return FAIL;

	}
	for (grp=0 ; grp<ADC_BTN_GRP_MAX ; grp++) 
	{
		sp5kFsFileRead(fd, (UINT8 *)adcBtnGrp[grp].adcTbl, adcBtnGrp[grp].adcTblCnt*2);
	}
	sp5kFsFileClose(fd);

	return SUCCESS;
}

UINT32 appBtnSave()
{

	UINT32 fd,grp;
	fd = sp5kFsFileOpen("A:\\RO_RES\\BTNADC.BIN", SP5K_FS_OPEN_CREATE);
	if (fd==0)
	{
		return FAIL;
	}
	for (grp=0 ; grp<ADC_BTN_GRP_MAX ; grp++) 
	{
		sp5kFsFileWrite(fd, (UINT8 *)adcBtnGrp[grp].adcTbl, adcBtnGrp[grp].adcTblCnt*2);
	}
	sp5kFsFileClose(fd);
	
	return SUCCESS;
}

static UINT32 adcRangeWait(UINT32 chnl, UINT32 lowbnd, UINT32 highbnd)
{
	UINT32 val,cnt =0;
//printf("adcRangeWait(%d,%d,%d)\n",chnl,lowbnd,highbnd);		
	while (1) 
	{
		if(sp5kAdcRead(chnl, &val) == SUCCESS) 
		{
			if (lowbnd<=val && val<=highbnd) 
			{
	//printf("=> adcRangeWait() : val = %d \n",val);				
				cnt ++;
				if(cnt >= 15)
				{
					return val;
				}
			}
		}
	}
}

static UINT32 adcOutRangeWait(UINT32 chnl, UINT32 lowbnd, UINT32 highbnd)
{
	UINT32 val,cnt =0;
	printf("adcOutRangeWait(%d,%d,%d)\n",chnl,lowbnd,highbnd);	
	while (1) 
	{
		if(sp5kAdcRead(chnl, &val) == SUCCESS) 
		{
			if (val<lowbnd || highbnd<val)
			{
				printf("=> adcOutRangeWait() : val = %d \n",val);	
				cnt ++;
				if(cnt >= 15)
				{
					return val;
				}
			}
		}
	}	
}

static UINT32 adcStableWait(UINT32 chnl)
{
	UINT32 val,sum,hist[20],idx;
	memset(hist, 0, sizeof(hist));
	idx = sum = 0;
	printf("adcStableWait(%d)\n",chnl);	
	
	while (1) 
	{		
		if(sp5kAdcRead(chnl, &val) == SUCCESS) 
		{
			if (val+10>sum/20 && sum/20>val-10) 
			{			
				printf("::val = %d; sum/20 =%d\n",val,sum/20);	
				return sum/20;
			}
			sum = sum + val - hist[idx];
			hist[idx++] = val;
			printf("val = %d; sum =%d\n",val,sum);	
			if (idx>=20) 
			{
				idx = 0;
			}
		}
		sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 10);
	}
}

typedef struct 
{
	UINT16 adc;
	UINT16 idx;
} adcsort_t;

/*
p1>p2:   1
p1=p2:   0
p1<p2:   -1
*/
static int
cdspAdcQsortCmp(
	const void *parg1,
	const void *parg2
)
{
	adcsort_t *p1=(adcsort_t *)parg1;
	adcsort_t *p2=(adcsort_t *)parg2;
	return p1->adc < p2->adc ? -1 :  p1->adc > p2->adc ? 1 : 0;
}

UINT32 appBtnCalib()
{
	UINT32 i,grp,val,r;
	UINT32 j,min,max,sum;
	for (grp=0 ; grp<ADC_BTN_GRP_MAX ; grp++) 
	{
		printf("grp:%d (%s)\n", grp, adcBtnGrp[grp].type?"dial":"btn");
		printf(adcBtnGrp[grp].type ? "rotate dial to right..." : "release all key...");
		//adcRangeWait( adcBtnGrp[grp].channel, min=1000, max=1024);
		adcRangeWait( adcBtnGrp[grp].channel, min=950, max=1024);
		printf("\n");
		for (i=0 ; i<adcBtnGrp[grp].adcTblCnt ; i++) 
		{
			printf("btn(%d) %s...=> press\n", i, adcBtnGrp[grp].id[i]);
			if (adcBtnGrp[grp].type) 
			{
				printf("press UART key twice..."); getch();
			} 
			else 
			{
				adcOutRangeWait(adcBtnGrp[grp].channel, min, max);
				adcStableWait(adcBtnGrp[grp].channel);
			}
			min = 1024; max = 0;
			for (j=0 ; j<1000 ; j++) 
			{
				sp5kAdcRead(adcBtnGrp[grp].channel, &val);
				if (val<min)
				{
					min = val;
				}
				if (val>max) 
				{
					max = val;
				}
				sum += val;
			}
			r = (min+max)/2;
			printf("avg=%d (%d-%d) mid=%d dev=%d...\n", val, min, max, r, max-r);
			adcBtnGrp[grp].adcTbl[i] = r | ((max-r)<<10);
			printf("btn(%d) %s... =>Release\n", i, adcBtnGrp[grp].id[i]);
			if (adcBtnGrp[grp].type==ADC_TYPE_BTN) 
			{
				//adcRangeWait(adcBtnGrp[grp].channel, min=1000, max=1024);
				adcRangeWait(adcBtnGrp[grp].channel, min=950, max=1024);
			}
			printf("\n");
		}
		if (adcBtnGrp[grp].type==ADC_TYPE_DIAL) 
		{
			adcsort_t buf[16];
			UINT32 bnd[16],dev;
			for (i=0 ; i<adcBtnGrp[grp].adcTblCnt ; i++) 
			{
				buf[i].adc = adcBtnGrp[grp].adcTbl[i] & 0x3ff;
				buf[i].idx = i;
			}
			qsort((void *)buf, adcBtnGrp[grp].adcTblCnt, sizeof(buf[0]), cdspAdcQsortCmp);
			bnd[0] = 0; bnd[adcBtnGrp[grp].adcTblCnt] = 1024+30;
			for (i=1 ; i<adcBtnGrp[grp].adcTblCnt ; i++)
			{
				bnd[i] = (buf[i-1].adc + buf[i].adc) / 2;
			}
			#if 0
			for (i=0 ; i<adcBtnGrp[grp].adcTblCnt ; i++) 
			{
				printf("%d %d idx=%d\n", i, buf[i].adc, buf[i].idx);
			}
			for (i=0 ; i<=adcBtnGrp[grp].adcTblCnt ; i++) 
			{
				printf("%d %d\n", i, bnd[i]);
			}
			#endif
			for (i=0 ; i<adcBtnGrp[grp].adcTblCnt ; i++) 
			{
				dev = ((bnd[i+1]-bnd[i])/2 - 1)/2;
				if (dev>=63)
				{
					dev = 63;
				}
				adcBtnGrp[grp].adcTbl[buf[i].idx] = ((bnd[i]+bnd[i+1])/2) | (dev<<10);
				#if 0
				printf("%d %d-%d\n", i, (adcBtnGrp[grp].adcTbl[buf[i].idx]&0x3ff)-dev*2, (adcBtnGrp[grp].adcTbl[buf[i].idx]&0x3ff)+dev*2);
				#endif
			}
		}
	}
	return SUCCESS;
}
#endif

