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
#include "sp5k_media_api.h"
#include "sp5k_dcf_api.h"
#include "app_playback_private.h"
#include "app_playback_def.h"
#include "app_playback_osd.h"
#include "app_msg.h"
#include "app_icon_def.h"


/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define PB_ZFACTOR_TOTAL_12X	(sizeof(zmFactor12x)/sizeof(zmFactor12x[0]))
#define PB_ZFACTOR_TOTAL_10X	(sizeof(zmFactor10x)/sizeof(zmFactor10x[0]))
#define PB_ZFACTOR_TOTAL_8X		(sizeof(zmFactor8x)/sizeof(zmFactor8x[0]))
#define PB_ZFACTOR_TOTAL_6X		(sizeof(zmFactor6x)/sizeof(zmFactor6x[0]))
#define PB_ZFACTOR_TOTAL_4X		(sizeof(zmFactor4x)/sizeof(zmFactor4x[0]))
#define PB_ZFACTOR_TOTAL_2X		(sizeof(zmFactor2x)/sizeof(zmFactor2x[0]))

#define PB_ZOOM_WIDTH_MIN	(320)
#define PB_ZOOM_HEIGHT_MIN	(240)
#define PB_ZOOM_SIZE_MIN	(PB_ZOOM_WIDTH_MIN*PB_ZOOM_HEIGHT_MIN)
#define PB_PAN_NORM_STEP	30 /* normalize pan steps */
#define PB_ZPAN_IDX_MAX		(sizeof(gpbZPanStep)/sizeof(gpbZPanStep[0]))

#define PB_NEXT_DZOOM_RATIO_INDEX(a)	do {\
											if (a==PB_ZOOM_TELE) zmFactorIdx++; \
											else if (a==PB_ZOOM_WIDE) zmFactorIdx--; \
										} while(0)

#define SET_PB_DZOOM_RATIO(r)	do {sp5kZoomFactorSet(SP5K_MODE_STILL_PLAYBACK, r);} while(0)
#define SET_PB_PAN_CEN(w, h)	do {sp5kPanCenterSet(SP5K_MODE_STILL_PLAYBACK, w, h);} while(0)
#define PLAY_MAX_PAN_STEP       5


/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

typedef enum {
	ZPAN_ZOOM_SXN_START,
	ZPAN_ZOOM_SXN_END,
	ZPAN_PAN_SXN_START,
	ZPAN_PAN_SXN_END,
	ZPAN_TRIM_SXN_START,
	ZPAN_TRIM_SXN_END,
} zpanUIKeyCtrl_t;

typedef enum {
	PB_ZPAN_UP,
	PB_ZPAN_DOWN,
	PB_ZPAN_LEFT,
	PB_ZPAN_RIGHT,
} pbZoomPanDir_t;

/*-- ZOOM direction */
typedef enum {
	PB_ZOOM_TELE,
	PB_ZOOM_WIDE
} pbZoomDir_t;

#if (KIT_WITH_HDMI)
typedef struct tagPLAY_BACK_ZOOM
{
	UINT32 wImgWidth;
	UINT32 wImgHigh;
	UINT32 wZFactor;
	UINT32 wZmTblIdx;
	UINT32 wZmTblMaxIdx;
	UINT32 wCenter_H;
	UINT32 wCenter_V;
} SPLAY_BACK_ZOOM;
SPLAY_BACK_ZOOM psPBZoom;
#endif
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
static UINT32 imgWidth, imgHeight;
static UINT32 cropWidth, cropHeight;
static SINT32 panCenX, panCenY;
static UINT16 zmFactorIdx;
static UINT32 zmMaxFactor;
static UINT16 ratioWidth, ratioHeight;
extern BOOL doFirstTime;

static UINT32 zmFactor12x[] =				/* 10M */
{
	1000,1013,1026,1040,1053,1067,1081,1095,1109,1124,1138,1153,1168,1183,1199,   
	1214,1230,1246,1262,1279,1295,1312,1329,1347,1364,1382,1400,1418,1437,1455,   
	1474,1494,1513,1533,1553,1573,1593,1614,1635,1657,1678,1700,1722,1745,1767,   
	1790,1814,1837,1861,1885,1910,1935,1960,1986,2011,2038,2064,2091,2118,2146,   
	2174,2202,2231,2260,2289,2319,2349,2380,2411,2442,2474,2507,2539,2572,2606,   
	2640,2674,2709,2744,2780,2816,2853,2890,2928,2966,3004,3044,3083,3123,3164,   
	3205,3247,3289,3332,3376,3420,3464,3509,3555,3601,3648,3696,3744,3793,3842,   
	3892,3943,3994,4046,4099,4152,4206,4261,4317,4373,4430,4488,4546,4605,4665,   
	4726,4788,4850,4913,4977,5042,5108,5174,5241,5310,5379,5449,5520,5592,5665,   
	5739,5813,5889,5966,6043,6122,6202,6283,6365,6447,6531,6616,6703,6790,6878,   
	6968,7059,7151,7244,7338,7434,7531,7629,7728,7829,7931,8034,8139,8245,8352,   
	8461,8571,8683,8796,8911,9027,9144,9263,9384,9506,9630,9755,9883,10011,10142, 
	10274,10408,10543,10681,10820,10961,11103,11248,11395,11543,11693,11846,12000,
};

static UINT32 zmFactor10x[] =				/* 8M */
{
	1000,1014,1029,1044,1059,1075,1090,1106,1122,1138,1155,1172,1189,1206,1223,
	1241,1259,1277,1296,1314,1334,1353,1372,1392,1413,1433,1454,1475,1496,1518,
	1540,1562,1585,1608,1631,1655,1679,1703,1728,1753,1778,1804,1830,1857,1884,
	1911,1939,1967,1995,2024,2054,2083,2113,2144,2175,2207,2239,2271,2304,2337,
	2371,2406,2441,2476,2512,2548,2585,2623,2661,2699,2738,2778,2818,2859,2901,
	2943,2985,3029,3073,3117,3162,3208,3255,3302,3350,3398,3447,3497,3548,3600,
	3652,3705,3758,3813,3868,3924,3981,4039,4097,4157,4217,4278,4340,4403,4467,
	4532,4597,4664,4732,4800,4870,4940,5012,5085,5158,5233,5309,5386,5464,5543,
	5623,5705,5788,5872,5957,6043,6131,6219,6310,6401,6494,6588,6683,6780,6879,
	6978,7079,7182,7286,7392,7499,7608,7718,7830,7943,8058,8175,8294,8414,8536,
	8660,8785,8913,9042,9173,9306,9441,9577,9716,9857,10000,
};

static UINT32 zmFactor8x[] =				/* 6M */
{
	1000,1016,1033,1050,1067,1085,1102,1120,1139,1157,1176,1196,1215,1235,1255,
	1276,1297,1318,1340,1362,1384,1407,1430,1453,1477,1501,1526,1551,1576,1602,
	1628,1655,1682,1709,1737,1766,1795,1824,1854,1884,1915,1947,1978,2011,2044,
	2077,2111,2146,2181,2217,2253,2290,2327,2366,2404,2444,2484,2524,2566,2608,
	2650,2694,2738,2783,2828,2875,2922,2970,3018,3068,3118,3169,3221,3274,3327,
	3382,3437,3494,3551,3609,3668,3728,3789,3851,3914,3978,4044,4110,4177,4246,
	4315,4386,4458,4531,4605,4680,4757,4835,4914,4994,5076,5159,5244,5330,5417,
	5506,5596,5688,5781,5875,5972,6069,6169,6270,6373,6477,6583,6691,6800,6912,
	7025,7140,7257,7376,7497,7619,7744,7871,8000,
};

static UINT32 zmFactor6x[] =				/* 3M */
{
	1000,1019,1038,1058,1078,1098,1118,1140,1161,1183,1205,1228,1251,1275,1299,
	1323,1348,1373,1399,1426,1452,1480,1508,1536,1565,1595,1625,1655,1686,1718,
	1751,1784,1817,1851,1886,1922,1958,1995,2032,2071,2110,2149,2190,2231,2273,
	2316,2360,2404,2449,2496,2543,2591,2639,2689,2740,2791,2844,2898,2952,3008,
	3064,3122,3181,3241,3302,3364,3428,3492,3558,3625,3693,3763,3834,3906,3979,
	4054,4131,4209,4288,4369,4451,4535,4620,4707,4796,4886,4978,5072,5168,5265,
	5364,5465,5568,5673,5780,5889,6000,
};

static UINT32 zmFactor4x[] =				/* 2M */
{
	1000,1022,1044,1067,1091,1114,1139,1164,1189,1215,1242,1269,1297,1325,1354,
	1384,1414,1445,1477,1509,1542,1576,1610,1646,1682,1719,1756,1795,1834,1874,
	1915,1957,2000,2044,2089,2134,2181,2229,2278,2327,2378,2430,2484,2538,2594,
	2650,2709,2768,2828,2890,2954,3018,3084,3152,3221,3292,3364,3437,3513,3589,
	3668,3748,3830,3914,4000,
};

static UINT32 zmFactor2x[] =				/* VGA */
{
	1000,1022,1044,1067,1091,1114,1139,1164,1189,1215,1242,1269,1297,1325,1354,
	1384,1414,1445,1477,1509,1542,1576,1610,1646,1682,1719,1756,1795,1834,1874,
	1915,1957,2000,

};

static UINT32 gpbZPanStep[] = {25,20,20,15,15};

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
static void appPbZPanStClose(void);
/*-------------------------------------------------------------------------
 *  File Name : zpanUIKeyControl
 *------------------------------------------------------------------------*/
static
void
zpanUIKeyControl(
	UINT8 ctrl
)
{
	switch (ctrl)
	{
	case ZPAN_ZOOM_SXN_START:
	case ZPAN_PAN_SXN_START:
	case ZPAN_TRIM_SXN_START:
		//appBtnDisable(BTN_ALL);
		break;
	case ZPAN_ZOOM_SXN_END:
	case ZPAN_PAN_SXN_END:
	case ZPAN_TRIM_SXN_END:
		//appBtnEnable(BTN_ALL);
		break;
	default:
		PB_VERBOSE("ZPan: undefined key session !!\n");
		break;
	}
}

/*-------------------------------------------------------------------------
 *  File Name : TRIM_ConfirmDlgConfig
 *------------------------------------------------------------------------*/
static
void
TRIM_ConfirmDlgConfig(
	void
)
{
	appPlayOsd_DlgConfirmYesNoConfig(ID_STR_TRIM_THIS_PHOTO_, 1);
}
/*-------------------------------------------------------------------------
 *  File Name : TRIM_ConfirmDlgShow
 *------------------------------------------------------------------------*/
static
void
TRIM_ConfirmDlgShow(
	void
)
{
	appPlayOsd_DlgBoxShow();
	appPbOsd_AuxNote_OKSet(TRUE);
}


#if (KIT_WITH_HDMI)
/*-------------------------------------------------------------------------
 *  File Name : appPbZoomPosGet
 *------------------------------------------------------------------------*/
BOOL appPbZoomPosGet(
	unsigned short *pLeft, 
	unsigned short *pTop,
	unsigned short *pWidth, 
	unsigned short *pHeight
)
{
	if (psPBZoom.wZFactor == 1000) {
		*pLeft = 0;
		*pTop = 0;
		*pWidth  = psPBZoom.wImgWidth;
		*pHeight = psPBZoom.wImgHigh;
		psPBZoom.wCenter_H = psPBZoom.wImgWidth >> 1;
		psPBZoom.wCenter_V = psPBZoom.wImgHigh >> 1;
	}
	else {
		sp5kStillPlayZoomInfoGet(pLeft, pTop, pWidth, pHeight);
		psPBZoom.wCenter_H = (*pLeft + (*pWidth>>1));
		psPBZoom.wCenter_V = (*pTop + (*pHeight>>1));
	}
	//printf("\n\n zmInfo(%d,%d,%d,%d)psPBZoom->wZFactor =%d \n",*pLeft, *pTop, *pWidth, *pHeight,psPBZoom->wZFactor);
	return TRUE;
}


/*-------------------------------------------------------------------------
 *  File Name : appPbZoomRoiSet
 *------------------------------------------------------------------------*/
void appPbZoomRoiSet(
	UINT32 idx
)
{
	UINT16 x0,y0,width,height;
	UINT16 wCenterX,wCenterY;


	appPbZoomPosGet(&x0, &y0, &width, &height);
	if((width == 0)||(height == 0)){
		width = psPBZoom.wImgWidth;
		height = psPBZoom.wImgHigh;
		x0 = 0; y0 = 0;
	}

	wCenterX = x0 + (width >> 1);
	wCenterY = y0 + (height >> 1);

	//printf("wCenterX = %d ; wCenterY = %d (%d x %d)\n",wCenterX,wCenterY,psPBZoom->wImgWidth,psPBZoom->wImgHigh);

	if(psPBZoom.wImgWidth < psPBZoom.wImgHigh){
		height = psPBZoom.wImgWidth;
		width = (height * 4)/3;
	}
	else{
		width = psPBZoom.wImgWidth;
		height = (width * 3)/4;
	}

	width = (((width + 15) >> 4)<<4); //16-alignment
	height = (((height + 7) >> 3)<<3); //8-alignment

	//printf("width = %d ; height = %d\n",width,height);

	//boundary check
	if(wCenterX >= (width >> 1)) {
		x0 = wCenterX - (width >> 1);
	}
	else {
		x0 = 0;
	}
	if(wCenterY >= (height >> 1)){
		y0 = wCenterY - (height >> 1);
	}
	else{
		y0 = 0;
	}
	if((x0 + width) >= psPBZoom.wImgWidth){
		x0 = psPBZoom.wImgWidth - width;
	}
	if((y0 + height) >= psPBZoom.wImgHigh){
		y0 = psPBZoom.wImgHigh - height;
	}

	//printf("PBzm ROI set.[%d,%d,%d,%d](idx=%d)\n",x0,y0,width,height,idx);
	sp5kZoomRoiSet(SP5K_MODE_STILL_PLAYBACK,x0,y0,width,height);
}


/*-------------------------------------------------------------------------
 *  File Name : appPbZoomFacToRoi		 modify zoom 
 *------------------------------------------------------------------------*/
void appPbZoomFacToRoi(UINT32 zFact){
	UINT16 x0,y0,width,height;
	UINT16 wCenterX,wCenterY;

	if(zFact == 1000){
		width = psPBZoom.wImgWidth;
		height = psPBZoom.wImgHigh;
		x0 = 0; y0 = 0;
		if((width/16*9) == height){	/* mantis 45327 */
			ratioWidth = 16;
			ratioHeight = 9;
		}
		else{
			ratioWidth = 4;
			ratioHeight = 3;
		}
	}
	else{
		appPbZoomPosGet(&x0, &y0, &width, &height);
		if((width == 0)||(height == 0)){
			width = psPBZoom.wImgWidth;
			height = psPBZoom.wImgHigh;
			x0 = 0; y0 = 0;
		}

		wCenterX = x0 + (width >> 1);
		wCenterY = y0 + (height >> 1);
		//printf("wCenterX = %d ; wCenterY = %d (%d x %d)\n",wCenterX,wCenterY,psPBZoom->wImgWidth,psPBZoom->wImgHigh);

		if(psPBZoom.wImgWidth < psPBZoom.wImgHigh){
			//PB_printf("zfac base : width\n");
			width = (psPBZoom.wImgWidth* 1000)/zFact;
			height = (width * ratioHeight)/16;
		}
		else{
			//PB_printf("zfac base : height\n");
			height = (psPBZoom.wImgHigh * 1000)/zFact;
			width = (height * ratioWidth)/ratioHeight;
		}

		#if 1 //workaround to fix image broken if VGA + dzm 9.0x + move to right side
		width = (((width + 7) >> 3)<<3); //8-alignment
		#else
		width = (width + 1) & ~1; //2-alignment
		#endif
		if(psPBZoom.wImgWidth==3240&&psPBZoom.wImgHigh==4320)//KIMI 110214 SPT-0438 SPT-0381
		{	
			if(zFact!=12000&&zFact!=5100)
			height = (width * ratioHeight)/ratioWidth+1;
			else
			height = (width * ratioHeight)/ratioWidth;	
		}
		else if(psPBZoom.wImgWidth==2880&&psPBZoom.wImgHigh==4320)
		{
			if(zFact!=9000&&zFact!=4500&&zFact!=7200)
			height = (width * ratioHeight)/ratioWidth+1;
			else
			height = (width * ratioHeight)/ratioWidth;	
		}
		else
		{
			height = (width * ratioHeight)/ratioWidth;//KIMI 110214 SPT-0438 SPT-0381
		}
		height = (height + 1) & ~1; //2-alignment

		//printf("width = %d ; height = %d\n",width,height);

		//boundary check
		if(wCenterX >= (width >> 1)) {
			x0 = wCenterX - (width >> 1);
		}
		else {
			x0 = 0;
		}
		if(wCenterY >= (height >> 1)){
			y0 = wCenterY - (height >> 1);
		}
		else{
			y0 = 0;
		}

		//constrain x0/y0 to be 2-alignment can reduce pan center flicker effect
		x0 = (x0 + 1) & ~1; //2-alignment
		y0 = (y0 + 1) & ~1; //2-alignment

		if((x0 + width) >= psPBZoom.wImgWidth){
			x0 = psPBZoom.wImgWidth - width;
		}
		if((y0 + height) >= psPBZoom.wImgHigh){
			y0 = psPBZoom.wImgHigh - height;
		}
	}

	//PB_printf("PBzm ROI set[%d,%d,%d,%d](zfac=%d)=>(%d,%d)\n",x0,y0,width,height,zFact,(x0+width/2),(y0+height/2));
	//printf("PBzm ROI set[%d,%d,%d,%d](zfac=%d)\n",x0,y0,width,height,zFact);
	sp5kZoomRoiSet(SP5K_MODE_STILL_PLAYBACK,x0,y0,width,height);
}


/*-------------------------------------------------------------------------
 *  File Name : appPbZoomFactorSet		 modify zoom 
 *------------------------------------------------------------------------*/
BOOL appPbZoomFactorSet(UINT32 zFact)
{
	UINT32 ret = SUCCESS;
	UINT16 left,top,width,height;

	appPbZoomFacToRoi(zFact);

	if (psPBZoom.wZFactor > 1000) {
		appPbZoomPosGet(&left, &top, &width, &height);
		#if 0
		//constrain max zoom factor to 6x if image size is smaller than 320x240
		if((( psPBImage->wHSize <= 320 )&&( psPBImage->wVSize <= 240 ))||(( psPBImage->wHSize <= 240 )&&( psPBImage->wVSize <= 320 ))){
			if(psPBZoom->wZFactor >= 6000){
				zoomAlreadyMax = TRUE;
				ret = FAIL;
			}
			else{
				zoomAlreadyMax = FALSE;
				ret = SUCCESS;
			}
		}
		else {
			zoomAlreadyMax = FALSE;
			ret = SUCCESS;
		}
		#endif
	}

	return ret;
}


/*-------------------------------------------------------------------------
 *  File Name : appPbZoomFactorSet		 modify zoom 
 *------------------------------------------------------------------------*/
UINT32 appPbPanControl(
	UINT8 panDir
)
{
	unsigned short usLeft,  usTop;
	unsigned short usWidth, usHeight;
	UINT32 panFac = 0, panFacIdx = 0;
	UINT32 ret = SUCCESS;

	panFacIdx = (psPBZoom.wZFactor / 1000) >> 1;

	if (panFacIdx >= PLAY_MAX_PAN_STEP) {
		panFacIdx = PLAY_MAX_PAN_STEP - 1;
	}
	panFac = gpbZPanStep[panFacIdx];

	if(( psPBZoom.wImgWidth <= 640 ) || ( psPBZoom.wImgHigh <= 480 )){
		panFac = panFac - 10;/* To slow down the speed */
	}
	else if(( psPBZoom.wImgWidth <= 1024 ) || ( psPBZoom.wImgHigh <= 768 )){
		panFac = panFac - 5;/* To slow down the speed */
	}
	printf("panFac = %d\n",panFac);

	appPbZoomPosGet(&usLeft, &usTop, &usWidth, &usHeight);

	switch (panDir) {
		case PB_ZPAN_RIGHT:
			doFirstTime=TRUE;
			if (psPBZoom.wCenter_H < (psPBZoom.wImgWidth -(usWidth/2))) {
				psPBZoom.wCenter_H += panFac;
			}
			else {
				psPBZoom.wCenter_H = psPBZoom.wImgWidth -(usWidth/2);
				//printf(">>>>>>out of range(R)!!!\n");
				ret = FAIL;
			}
			if (IS_KEY_RIGHT_PRESSED) 
			{
				sp5kHostMsgSend(APP_UI_MSG_PB_PAN_RIGHT, 0);
			}
			break;
		case PB_ZPAN_LEFT:
			doFirstTime=TRUE;
			if ((psPBZoom.wCenter_H > (usWidth/2)) && ( psPBZoom.wCenter_H > panFac)) {
				psPBZoom.wCenter_H -= panFac;
			}
			else {
				psPBZoom.wCenter_H = usWidth/2;
				//printf(">>>>>>out of range(L)!!!\n");
				ret = FAIL;
			}
			if (IS_KEY_LEFT_PRESSED) 
			{
				sp5kHostMsgSend(APP_UI_MSG_PB_PAN_LEFT, 0);
			}
			break;
		case PB_ZPAN_UP:
			doFirstTime=TRUE;
			if ((psPBZoom.wCenter_V > (usHeight/2)) && (psPBZoom.wCenter_V > panFac)) {
				psPBZoom.wCenter_V -= panFac;
			}
			else {
				psPBZoom.wCenter_V = usHeight/2;
				//printf(">>>>>>out of range(T)!!!\n");
				ret = FAIL;
			}
			if (IS_KEY_UP_PRESSED) 
			{
				sp5kHostMsgSend(APP_UI_MSG_PB_PAN_UP, 0);
			}
			break;
		case PB_ZPAN_DOWN:
			doFirstTime=TRUE;
			if (psPBZoom.wCenter_V < (psPBZoom.wImgHigh - (usHeight/2))) {
				psPBZoom.wCenter_V += panFac;
			}
			else {
				psPBZoom.wCenter_V = psPBZoom.wImgHigh - (usHeight/2);
				//printf(">>>>>>out of range(B)!!!\n");
				ret = FAIL;
			}
			if (IS_KEY_DOWN_PRESSED) 
			{
				sp5kHostMsgSend(APP_UI_MSG_PB_PAN_DOWN, 0);
			}
			break;
	}

	if (psPBZoom.wCenter_H < (usWidth/2)) {
		psPBZoom.wCenter_H = (usWidth/2);
	}
	if (psPBZoom.wCenter_V < (usHeight/2)) {
		psPBZoom.wCenter_V = (usHeight/2);
	}
	if (psPBZoom.wCenter_H > (psPBZoom.wImgWidth - (usWidth/2))) {
		psPBZoom.wCenter_H = psPBZoom.wImgWidth - (usWidth/2);
	}
	if (psPBZoom.wCenter_V > (psPBZoom.wImgHigh - (usHeight/2))) {
		psPBZoom.wCenter_V = psPBZoom.wImgHigh - (usHeight/2);
	}
	return ret;
}


/*-------------------------------------------------------------------------
 *  File Name : appPbZoomFactorSet		 modify zoom 
 *------------------------------------------------------------------------*/
UINT32 appPbPanCenterSet(
	SINT32 panCenterH,
	SINT32 panCenterV
)
{
	UINT32 ret;
	UINT16 x0,y0,width,height;

	appPbZoomPosGet(&x0, &y0, &width, &height);
	x0 = panCenterH - (width >> 1);
	y0 = panCenterV - (height >> 1);
	printf("PBzm pan ROI[%d,%d,%d,%d]\n",x0,y0,width,height);
	ret = sp5kZoomRoiSet(SP5K_MODE_STILL_PLAYBACK,x0,y0,width,height);
	if(ret != SUCCESS){
		printf("[ERR] : sp5kPanCenterSet() err : 0x%x\n",ret);
	}
	printf("PB_PlaybackPanCenterSet(%d,%d)\b",panCenterH,panCenterV);

	#if 1
	appPlayOsd_DigitalZoomWinDraw(panCenterH, panCenterV, (UINT16)imgWidth, (UINT16)imgHeight, zmFactor8x[psPBZoom.wZmTblIdx], gpbImgCb.imgOrientDim);
	#else
	if(3548 < imgWidth){ 			/* 10M */
		appPlayOsd_DigitalZoomWinDraw(panCenterH, panCenterV, (UINT16)imgWidth, (UINT16)imgHeight, zmFactor12x[psPBZoom.wZmTblIdx], gpbImgCb.imgOrientDim);
	}
	else if(3164 < imgWidth && imgWidth < 3500){		/* 8M */
		appPlayOsd_DigitalZoomWinDraw(panCenterH, panCenterV, (UINT16)imgWidth, (UINT16)imgHeight, zmFactor10x[psPBZoom.wZmTblIdx], gpbImgCb.imgOrientDim);
	}
	else if(2460 < imgWidth && imgWidth < 3100){		/* 6M */
		appPlayOsd_DigitalZoomWinDraw(panCenterH, panCenterV, (UINT16)imgWidth, (UINT16)imgHeight, zmFactor8x[psPBZoom.wZmTblIdx], gpbImgCb.imgOrientDim);
	}
	else if(1948 < imgWidth && imgWidth < 2460){		/* 3M */
		appPlayOsd_DigitalZoomWinDraw(panCenterH, panCenterV, (UINT16)imgWidth, (UINT16)imgHeight, zmFactor6x[psPBZoom.wZmTblIdx], gpbImgCb.imgOrientDim);
	}
	else if(924 < imgWidth && imgWidth < 1948){ 		/* 2M */
		appPlayOsd_DigitalZoomWinDraw(panCenterH, panCenterV, (UINT16)imgWidth, (UINT16)imgHeight, zmFactor4x[psPBZoom.wZmTblIdx], gpbImgCb.imgOrientDim);
	}
	else{												/* VGA */
		appPlayOsd_DigitalZoomWinDraw(panCenterH, panCenterV, (UINT16)imgWidth, (UINT16)imgHeight, zmFactor2x[psPBZoom.wZmTblIdx], gpbImgCb.imgOrientDim);
	}
	#endif
	
	return ret;
}
#endif


/*-------------------------------------------------------------------------
 *  File Name : playZoomIn
 *------------------------------------------------------------------------*/
static
void
playZoomIn(
	void
)
{
    #if !MSG_NEW_PROC   /* avoid warning */
	UINT32 msg, param;
    #endif
	float ratio;

	ratio = ratio;   /* avoid warning */
#if 1    /*junqiong.hu@20121220 add for zoom 8x  through fair and foul*/
	do 
	{
		#if (KIT_WITH_HDMI)
		if ( (zmFactorIdx>=PB_ZFACTOR_TOTAL_8X-1) || (psPBZoom.wZmTblIdx==PB_ZFACTOR_TOTAL_8X-1) ) {
			break;
		}
		if (IS_HDMI_IN){
			psPBZoom.wZmTblIdx ++; 
			psPBZoom.wZFactor = zmFactor8x[psPBZoom.wZmTblIdx];
			appPbZoomFactorSet(psPBZoom.wZFactor);
			appPlayOsd_DigitalZoomWinDraw(psPBZoom.wCenter_H, psPBZoom.wCenter_V, (UINT16)psPBZoom.wImgWidth, (UINT16)psPBZoom.wImgHigh, zmFactor8x[psPBZoom.wZmTblIdx], gpbImgCb.imgOrientDim);
		}else{
			PB_NEXT_DZOOM_RATIO_INDEX(PB_ZOOM_TELE);
			SET_PB_DZOOM_RATIO(zmFactor8x[zmFactorIdx]);
			appPlayOsd_DigitalZoomWinDraw(panCenX, panCenY, (UINT16)imgWidth, (UINT16)imgHeight, zmFactor8x[zmFactorIdx], gpbImgCb.imgOrientDim);
		}
		#else
		if ( zmFactorIdx>=PB_ZFACTOR_TOTAL_8X-1 ) {
			break;
		}
		PB_NEXT_DZOOM_RATIO_INDEX(PB_ZOOM_TELE);
		SET_PB_DZOOM_RATIO(zmFactor8x[zmFactorIdx]);
		appPlayOsd_DigitalZoomWinDraw(panCenX, panCenY, (UINT16)imgWidth, (UINT16)imgHeight, zmFactor8x[zmFactorIdx], gpbImgCb.imgOrientDim);
		#endif
		#if MSG_NEW_PROC
		appMsgFlush(APP_MSG_KERNEL_QUE);
		#else
		sp5kHostMsgReceive(&msg, &param);
		#endif
	} while (IS_KEY_TELE_PRESSED);
	
#else
	if(3548 < imgWidth){ 		/* 10M */
		do 
		{
			#if (KIT_WITH_HDMI)
			if ( (zmFactorIdx>=PB_ZFACTOR_TOTAL_12X-1) || (psPBZoom.wZmTblIdx==PB_ZFACTOR_TOTAL_12X-1) ) {
				break;
			}
			if (IS_HDMI_IN){
				psPBZoom.wZmTblIdx ++; 
				psPBZoom.wZFactor = zmFactor12x[psPBZoom.wZmTblIdx];
				appPbZoomFactorSet(psPBZoom.wZFactor);
				appPlayOsd_DigitalZoomWinDraw(psPBZoom.wCenter_H, psPBZoom.wCenter_V, (UINT16)psPBZoom.wImgWidth, (UINT16)psPBZoom.wImgHigh, zmFactor12x[psPBZoom.wZmTblIdx], gpbImgCb.imgOrientDim);
			}else{
				PB_NEXT_DZOOM_RATIO_INDEX(PB_ZOOM_TELE);
				SET_PB_DZOOM_RATIO(zmFactor12x[zmFactorIdx]);
				appPlayOsd_DigitalZoomWinDraw(panCenX, panCenY, (UINT16)imgWidth, (UINT16)imgHeight, zmFactor12x[zmFactorIdx], gpbImgCb.imgOrientDim);
			}
			#else
			if ( zmFactorIdx>=PB_ZFACTOR_TOTAL_12X-1 ) {
				break;
			}
			PB_NEXT_DZOOM_RATIO_INDEX(PB_ZOOM_TELE);
			SET_PB_DZOOM_RATIO(zmFactor12x[zmFactorIdx]);
			appPlayOsd_DigitalZoomWinDraw(panCenX, panCenY, (UINT16)imgWidth, (UINT16)imgHeight, zmFactor12x[zmFactorIdx], gpbImgCb.imgOrientDim);
			#endif
			sp5kHostMsgReceive(&msg, &param);
		} while (IS_KEY_TELE_PRESSED);
	}
	else if(3164 < imgWidth && imgWidth < 3500){	/* 8M */
		do 
		{
			#if (KIT_WITH_HDMI)
			if ( (zmFactorIdx>=PB_ZFACTOR_TOTAL_10X-1) || (psPBZoom.wZmTblIdx==PB_ZFACTOR_TOTAL_10X-1)) {
				break;
			}
			if (IS_HDMI_IN){
				psPBZoom.wZmTblIdx ++; 
				psPBZoom.wZFactor = zmFactor10x[psPBZoom.wZmTblIdx];
				appPbZoomFactorSet(psPBZoom.wZFactor);
				appPlayOsd_DigitalZoomWinDraw(psPBZoom.wCenter_H, psPBZoom.wCenter_V, (UINT16)psPBZoom.wImgWidth, (UINT16)psPBZoom.wImgHigh, zmFactor10x[psPBZoom.wZmTblIdx], gpbImgCb.imgOrientDim);
			}else{
				PB_NEXT_DZOOM_RATIO_INDEX(PB_ZOOM_TELE);
				SET_PB_DZOOM_RATIO(zmFactor10x[zmFactorIdx]);
				appPlayOsd_DigitalZoomWinDraw(panCenX, panCenY, (UINT16)imgWidth, (UINT16)imgHeight, zmFactor10x[zmFactorIdx], gpbImgCb.imgOrientDim);
			}
			#else
			if ( zmFactorIdx>=PB_ZFACTOR_TOTAL_10X-1 ) {
				break;
			}
			PB_NEXT_DZOOM_RATIO_INDEX(PB_ZOOM_TELE);
			SET_PB_DZOOM_RATIO(zmFactor12x[zmFactorIdx]);
			appPlayOsd_DigitalZoomWinDraw(panCenX, panCenY, (UINT16)imgWidth, (UINT16)imgHeight, zmFactor12x[zmFactorIdx], gpbImgCb.imgOrientDim);
			#endif
			sp5kHostMsgReceive(&msg, &param);
		} while (IS_KEY_TELE_PRESSED);
	}
	else if(2460 < imgWidth && imgWidth < 3100){	/* 6M */
		do 
		{
			#if (KIT_WITH_HDMI)
			if ( (zmFactorIdx>=PB_ZFACTOR_TOTAL_8X-1) || (psPBZoom.wZmTblIdx==PB_ZFACTOR_TOTAL_8X-1) ) {
				break;
			}
			if (IS_HDMI_IN){
				psPBZoom.wZmTblIdx ++; 
				psPBZoom.wZFactor = zmFactor8x[psPBZoom.wZmTblIdx];
				appPbZoomFactorSet(psPBZoom.wZFactor);
				appPlayOsd_DigitalZoomWinDraw(psPBZoom.wCenter_H, psPBZoom.wCenter_V, (UINT16)psPBZoom.wImgWidth, (UINT16)psPBZoom.wImgHigh, zmFactor8x[psPBZoom.wZmTblIdx], gpbImgCb.imgOrientDim);
			}else{
				PB_NEXT_DZOOM_RATIO_INDEX(PB_ZOOM_TELE);
				SET_PB_DZOOM_RATIO(zmFactor8x[zmFactorIdx]);
				appPlayOsd_DigitalZoomWinDraw(panCenX, panCenY, (UINT16)imgWidth, (UINT16)imgHeight, zmFactor8x[zmFactorIdx], gpbImgCb.imgOrientDim);
			}
			#else
			if ( zmFactorIdx>=PB_ZFACTOR_TOTAL_8X-1 ) {
				break;
			}
			PB_NEXT_DZOOM_RATIO_INDEX(PB_ZOOM_TELE);
			SET_PB_DZOOM_RATIO(zmFactor12x[zmFactorIdx]);
			appPlayOsd_DigitalZoomWinDraw(panCenX, panCenY, (UINT16)imgWidth, (UINT16)imgHeight, zmFactor12x[zmFactorIdx], gpbImgCb.imgOrientDim);
			#endif
			sp5kHostMsgReceive(&msg, &param);
		} while (IS_KEY_TELE_PRESSED);
	}
	else if(1948 < imgWidth && imgWidth < 2460){	/* 3M */
		do 
		{
			#if (KIT_WITH_HDMI)
			if ( (zmFactorIdx>=PB_ZFACTOR_TOTAL_6X-1)  || (psPBZoom.wZmTblIdx==PB_ZFACTOR_TOTAL_6X-1) ) {
				break;
			}
			if (IS_HDMI_IN){
				psPBZoom.wZmTblIdx ++; 
				psPBZoom.wZFactor = zmFactor6x[psPBZoom.wZmTblIdx];
				appPbZoomFactorSet(psPBZoom.wZFactor);
				appPlayOsd_DigitalZoomWinDraw(psPBZoom.wCenter_H, psPBZoom.wCenter_V, (UINT16)psPBZoom.wImgWidth, (UINT16)psPBZoom.wImgHigh, zmFactor6x[psPBZoom.wZmTblIdx], gpbImgCb.imgOrientDim);
			}else{
				PB_NEXT_DZOOM_RATIO_INDEX(PB_ZOOM_TELE);
				SET_PB_DZOOM_RATIO(zmFactor6x[zmFactorIdx]);
				appPlayOsd_DigitalZoomWinDraw(panCenX, panCenY, (UINT16)imgWidth, (UINT16)imgHeight, zmFactor6x[zmFactorIdx], gpbImgCb.imgOrientDim);
			}
			#else
			if ( zmFactorIdx>=PB_ZFACTOR_TOTAL_6X-1 ) {
				break;
			}
			PB_NEXT_DZOOM_RATIO_INDEX(PB_ZOOM_TELE);
			SET_PB_DZOOM_RATIO(zmFactor12x[zmFactorIdx]);
			appPlayOsd_DigitalZoomWinDraw(panCenX, panCenY, (UINT16)imgWidth, (UINT16)imgHeight, zmFactor12x[zmFactorIdx], gpbImgCb.imgOrientDim);
			#endif
			sp5kHostMsgReceive(&msg, &param);
		} while (IS_KEY_TELE_PRESSED);
	}
	else if(924 < imgWidth && imgWidth < 1948){ 	/* 2M */
		do 
		{
			#if (KIT_WITH_HDMI)
			if ( (zmFactorIdx>=PB_ZFACTOR_TOTAL_4X-1) || (psPBZoom.wZmTblIdx==PB_ZFACTOR_TOTAL_4X-1) ) {
				break;
			}
			if (IS_HDMI_IN){
				psPBZoom.wZmTblIdx ++; 
				psPBZoom.wZFactor = zmFactor4x[psPBZoom.wZmTblIdx];
				appPbZoomFactorSet(psPBZoom.wZFactor);
				appPlayOsd_DigitalZoomWinDraw(psPBZoom.wCenter_H, psPBZoom.wCenter_V, (UINT16)psPBZoom.wImgWidth, (UINT16)psPBZoom.wImgHigh, zmFactor4x[psPBZoom.wZmTblIdx], gpbImgCb.imgOrientDim);
			}else{
				PB_NEXT_DZOOM_RATIO_INDEX(PB_ZOOM_TELE);
				SET_PB_DZOOM_RATIO(zmFactor4x[zmFactorIdx]);
				appPlayOsd_DigitalZoomWinDraw(panCenX, panCenY, (UINT16)imgWidth, (UINT16)imgHeight, zmFactor4x[zmFactorIdx], gpbImgCb.imgOrientDim);
			}
			#else
			if ( zmFactorIdx>=PB_ZFACTOR_TOTAL_4X-1 ) {
				break;
			}
			PB_NEXT_DZOOM_RATIO_INDEX(PB_ZOOM_TELE);
			SET_PB_DZOOM_RATIO(zmFactor12x[zmFactorIdx]);
			appPlayOsd_DigitalZoomWinDraw(panCenX, panCenY, (UINT16)imgWidth, (UINT16)imgHeight, zmFactor12x[zmFactorIdx], gpbImgCb.imgOrientDim);
			#endif
			sp5kHostMsgReceive(&msg, &param);
		} while (IS_KEY_TELE_PRESSED);
	}
	else{											/* VGA */
		do 
		{
			#if (KIT_WITH_HDMI)
			if ( (zmFactorIdx>=PB_ZFACTOR_TOTAL_2X-1) || (psPBZoom.wZmTblIdx==PB_ZFACTOR_TOTAL_2X-1) ) {
				break;
			}
			if (IS_HDMI_IN){
				psPBZoom.wZmTblIdx ++; 
				psPBZoom.wZFactor = zmFactor2x[psPBZoom.wZmTblIdx];
				appPbZoomFactorSet(psPBZoom.wZFactor);
				appPlayOsd_DigitalZoomWinDraw(psPBZoom.wCenter_H, psPBZoom.wCenter_V, (UINT16)psPBZoom.wImgWidth, (UINT16)psPBZoom.wImgHigh, zmFactor2x[psPBZoom.wZmTblIdx], gpbImgCb.imgOrientDim);
			}else{
				PB_NEXT_DZOOM_RATIO_INDEX(PB_ZOOM_TELE);
				SET_PB_DZOOM_RATIO(zmFactor2x[zmFactorIdx]);
				appPlayOsd_DigitalZoomWinDraw(panCenX, panCenY, (UINT16)imgWidth, (UINT16)imgHeight, zmFactor2x[zmFactorIdx], gpbImgCb.imgOrientDim);
			}
			#else
			if ( zmFactorIdx>=PB_ZFACTOR_TOTAL_2X-1 ) {
				break;
			}
			PB_NEXT_DZOOM_RATIO_INDEX(PB_ZOOM_TELE);
			SET_PB_DZOOM_RATIO(zmFactor12x[zmFactorIdx]);
			appPlayOsd_DigitalZoomWinDraw(panCenX, panCenY, (UINT16)imgWidth, (UINT16)imgHeight, zmFactor12x[zmFactorIdx], gpbImgCb.imgOrientDim);
			#endif
			sp5kHostMsgReceive(&msg, &param);
		} while (IS_KEY_TELE_PRESSED);
	}
#endif
}

/*-------------------------------------------------------------------------
 *  File Name : playZoomOut
 *------------------------------------------------------------------------*/
static
void
playZoomOut(
	void
)
{
	#if !MSG_NEW_PROC     /* avoid warning */
	UINT32 msg, param;
	#endif
#if 1  /*junqiong.hu@20121220 add for zoom 8x  through fair and foul*/
	do 
	{
		#if (KIT_WITH_HDMI)
		if (IS_HDMI_IN){
			psPBZoom.wZmTblIdx --;
			psPBZoom.wZFactor = zmFactor8x[psPBZoom.wZmTblIdx];
			appPbZoomFactorSet(psPBZoom.wZFactor);
			appPlayOsd_DigitalZoomWinDraw(psPBZoom.wCenter_H, psPBZoom.wCenter_V, (UINT16)imgWidth, (UINT16)imgHeight, zmFactor8x[psPBZoom.wZmTblIdx], gpbImgCb.imgOrientDim);
			zmFactorIdx=1;
		}else{
			PB_NEXT_DZOOM_RATIO_INDEX(PB_ZOOM_WIDE);
			SET_PB_DZOOM_RATIO(zmFactor8x[zmFactorIdx]);
			appPlayOsd_DigitalZoomWinDraw(panCenX, panCenY, (UINT16)imgWidth, (UINT16)imgHeight, zmFactor8x[zmFactorIdx], gpbImgCb.imgOrientDim);
			psPBZoom.wZmTblIdx=1;
		}
		#else
		PB_NEXT_DZOOM_RATIO_INDEX(PB_ZOOM_WIDE);
		SET_PB_DZOOM_RATIO(zmFactor8x[zmFactorIdx]);
		appPlayOsd_DigitalZoomWinDraw(panCenX, panCenY, (UINT16)imgWidth, (UINT16)imgHeight, zmFactor8x[zmFactorIdx], gpbImgCb.imgOrientDim);
		#endif
        #if MSG_NEW_PROC
		appMsgFlush(APP_MSG_KERNEL_QUE);
        #else
		sp5kHostMsgReceive(&msg, &param);
        #endif
		#if (KIT_WITH_HDMI)
		if(zmFactorIdx==0 || psPBZoom.wZmTblIdx==0)
		{
			doFirstTime=TRUE;
			zmFactorIdx=0;
			psPBZoom.wZFactor=0;
			appPbZPanStClose();
			appStateChange(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
			return;
		}
		#else
		if(zmFactorIdx==0)
		{
			doFirstTime=TRUE;
			zmFactorIdx=0;
			appPbZPanStClose();
			appStateChange(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
			return;
		}
		#endif
	} while (IS_KEY_WIDE_PRESSED);
	
#else
	if(3548 < imgWidth){ 		/* 10M */
		do 
		{
			#if (KIT_WITH_HDMI)
			if (IS_HDMI_IN){
				psPBZoom.wZmTblIdx --;
				psPBZoom.wZFactor = zmFactor12x[psPBZoom.wZmTblIdx];
				appPbZoomFactorSet(psPBZoom.wZFactor);
				appPlayOsd_DigitalZoomWinDraw(psPBZoom.wCenter_H, psPBZoom.wCenter_V, (UINT16)imgWidth, (UINT16)imgHeight, zmFactor12x[psPBZoom.wZmTblIdx], gpbImgCb.imgOrientDim);
				zmFactorIdx=1;
			}else{
				PB_NEXT_DZOOM_RATIO_INDEX(PB_ZOOM_WIDE);
				SET_PB_DZOOM_RATIO(zmFactor12x[zmFactorIdx]);
				appPlayOsd_DigitalZoomWinDraw(panCenX, panCenY, (UINT16)imgWidth, (UINT16)imgHeight, zmFactor12x[zmFactorIdx], gpbImgCb.imgOrientDim);
				psPBZoom.wZmTblIdx=1;
			}
			#else
			PB_NEXT_DZOOM_RATIO_INDEX(PB_ZOOM_WIDE);
			SET_PB_DZOOM_RATIO(zmFactor12x[zmFactorIdx]);
			appPlayOsd_DigitalZoomWinDraw(panCenX, panCenY, (UINT16)imgWidth, (UINT16)imgHeight, zmFactor12x[zmFactorIdx], gpbImgCb.imgOrientDim);
			#endif
			sp5kHostMsgReceive(&msg, &param);
			#if (KIT_WITH_HDMI)
			if(zmFactorIdx==0 || psPBZoom.wZmTblIdx==0)
			{
				doFirstTime=TRUE;
				zmFactorIdx=0;
				psPBZoom.wZFactor=0;
				appPbZPanStClose();
				appStateChange(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
				return;
			}
			#else
			if(zmFactorIdx==0)
			{
				doFirstTime=TRUE;
				zmFactorIdx=0;
				appPbZPanStClose();
				appStateChange(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
				return;
			}
			#endif
		} while (IS_KEY_WIDE_PRESSED);

	}
	else if(3164 < imgWidth && imgWidth < 3500){	/* 8M */
		do 
		{
			#if (KIT_WITH_HDMI)
			if (IS_HDMI_IN){
				psPBZoom.wZmTblIdx --;
				psPBZoom.wZFactor = zmFactor10x[psPBZoom.wZmTblIdx];
				appPbZoomFactorSet(psPBZoom.wZFactor);
				appPlayOsd_DigitalZoomWinDraw(psPBZoom.wCenter_H, psPBZoom.wCenter_V, (UINT16)imgWidth, (UINT16)imgHeight, zmFactor10x[psPBZoom.wZmTblIdx], gpbImgCb.imgOrientDim);
				zmFactorIdx=1;
			}else{
				PB_NEXT_DZOOM_RATIO_INDEX(PB_ZOOM_WIDE);
				SET_PB_DZOOM_RATIO(zmFactor10x[zmFactorIdx]);
				appPlayOsd_DigitalZoomWinDraw(panCenX, panCenY, (UINT16)imgWidth, (UINT16)imgHeight, zmFactor10x[zmFactorIdx], gpbImgCb.imgOrientDim);
				psPBZoom.wZmTblIdx=1;
			}
			#else
			PB_NEXT_DZOOM_RATIO_INDEX(PB_ZOOM_WIDE);
			SET_PB_DZOOM_RATIO(zmFactor12x[zmFactorIdx]);
			appPlayOsd_DigitalZoomWinDraw(panCenX, panCenY, (UINT16)imgWidth, (UINT16)imgHeight, zmFactor12x[zmFactorIdx], gpbImgCb.imgOrientDim);
			#endif
			sp5kHostMsgReceive(&msg, &param);
			#if (KIT_WITH_HDMI)
			if(zmFactorIdx==0 || psPBZoom.wZmTblIdx==0)
			{
				doFirstTime=TRUE;
				zmFactorIdx=0;
				psPBZoom.wZFactor=0;
				appPbZPanStClose();
				appStateChange(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
				return;
			}
			#else
			if(zmFactorIdx==0)
			{
				doFirstTime=TRUE;
				zmFactorIdx=0;
				appPbZPanStClose();
				appStateChange(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
				return;
			}
			#endif
		} while (IS_KEY_WIDE_PRESSED);
	}
	else if(2460 < imgWidth && imgWidth < 3100){	/* 6M */
		do 
		{
			#if (KIT_WITH_HDMI)
			if (IS_HDMI_IN){
				psPBZoom.wZmTblIdx --;
				psPBZoom.wZFactor = zmFactor8x[psPBZoom.wZmTblIdx];
				appPbZoomFactorSet(psPBZoom.wZFactor);
				appPlayOsd_DigitalZoomWinDraw(psPBZoom.wCenter_H, psPBZoom.wCenter_V, (UINT16)imgWidth, (UINT16)imgHeight, zmFactor8x[psPBZoom.wZmTblIdx], gpbImgCb.imgOrientDim);
				zmFactorIdx=1;
			}else{
				PB_NEXT_DZOOM_RATIO_INDEX(PB_ZOOM_WIDE);
				SET_PB_DZOOM_RATIO(zmFactor8x[zmFactorIdx]);
				appPlayOsd_DigitalZoomWinDraw(panCenX, panCenY, (UINT16)imgWidth, (UINT16)imgHeight, zmFactor8x[zmFactorIdx], gpbImgCb.imgOrientDim);
				psPBZoom.wZmTblIdx=1;
			}
			#else
			PB_NEXT_DZOOM_RATIO_INDEX(PB_ZOOM_WIDE);
			SET_PB_DZOOM_RATIO(zmFactor12x[zmFactorIdx]);
			appPlayOsd_DigitalZoomWinDraw(panCenX, panCenY, (UINT16)imgWidth, (UINT16)imgHeight, zmFactor12x[zmFactorIdx], gpbImgCb.imgOrientDim);
			#endif
			sp5kHostMsgReceive(&msg, &param);
			#if (KIT_WITH_HDMI)
			if(zmFactorIdx==0 || psPBZoom.wZmTblIdx==0)
			{
				doFirstTime=TRUE;
				zmFactorIdx=0;
				psPBZoom.wZFactor=0;
				appPbZPanStClose();
				appStateChange(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
				return;
			}
			#else
			if(zmFactorIdx==0)
			{
				doFirstTime=TRUE;
				zmFactorIdx=0;
				appPbZPanStClose();
				appStateChange(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
				return;
			}
			#endif
		} while (IS_KEY_WIDE_PRESSED);
	}
	else if(1948 < imgWidth && imgWidth < 2460){	/* 3M */
		do 
		{
			#if (KIT_WITH_HDMI)
			if (IS_HDMI_IN){
				psPBZoom.wZmTblIdx --;
				psPBZoom.wZFactor = zmFactor6x[psPBZoom.wZmTblIdx];
				appPbZoomFactorSet(psPBZoom.wZFactor);
				appPlayOsd_DigitalZoomWinDraw(psPBZoom.wCenter_H, psPBZoom.wCenter_V, (UINT16)imgWidth, (UINT16)imgHeight, zmFactor6x[psPBZoom.wZmTblIdx], gpbImgCb.imgOrientDim);
				zmFactorIdx=1;
			}else{
				PB_NEXT_DZOOM_RATIO_INDEX(PB_ZOOM_WIDE);
				SET_PB_DZOOM_RATIO(zmFactor6x[zmFactorIdx]);
				appPlayOsd_DigitalZoomWinDraw(panCenX, panCenY, (UINT16)imgWidth, (UINT16)imgHeight, zmFactor6x[zmFactorIdx], gpbImgCb.imgOrientDim);
				psPBZoom.wZmTblIdx=1;
			}
			#else
			PB_NEXT_DZOOM_RATIO_INDEX(PB_ZOOM_WIDE);
			SET_PB_DZOOM_RATIO(zmFactor12x[zmFactorIdx]);
			appPlayOsd_DigitalZoomWinDraw(panCenX, panCenY, (UINT16)imgWidth, (UINT16)imgHeight, zmFactor12x[zmFactorIdx], gpbImgCb.imgOrientDim);
			#endif
			sp5kHostMsgReceive(&msg, &param);
			#if (KIT_WITH_HDMI)
			if(zmFactorIdx==0 || psPBZoom.wZmTblIdx==0)
			{
				doFirstTime=TRUE;
				zmFactorIdx=0;
				psPBZoom.wZFactor=0;
				appPbZPanStClose();
				appStateChange(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
				return;
			}
			#else
			if(zmFactorIdx==0)
			{
				doFirstTime=TRUE;
				zmFactorIdx=0;
				appPbZPanStClose();
				appStateChange(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
				return;
			}
			#endif
		} while (IS_KEY_WIDE_PRESSED);
	}
	else if(924 < imgWidth && imgWidth < 1948){ 	/* 2M */
		do 
		{	
			#if (KIT_WITH_HDMI)
			if (IS_HDMI_IN){
				psPBZoom.wZmTblIdx --;
				psPBZoom.wZFactor = zmFactor4x[psPBZoom.wZmTblIdx];
				appPbZoomFactorSet(psPBZoom.wZFactor);
				appPlayOsd_DigitalZoomWinDraw(psPBZoom.wCenter_H, psPBZoom.wCenter_V, (UINT16)imgWidth, (UINT16)imgHeight, zmFactor4x[psPBZoom.wZmTblIdx], gpbImgCb.imgOrientDim);
				zmFactorIdx=1;
			}else{
				PB_NEXT_DZOOM_RATIO_INDEX(PB_ZOOM_WIDE);
				SET_PB_DZOOM_RATIO(zmFactor4x[zmFactorIdx]);
				appPlayOsd_DigitalZoomWinDraw(panCenX, panCenY, (UINT16)imgWidth, (UINT16)imgHeight, zmFactor4x[zmFactorIdx], gpbImgCb.imgOrientDim);
				psPBZoom.wZmTblIdx=1;
			}
			#else
			PB_NEXT_DZOOM_RATIO_INDEX(PB_ZOOM_WIDE);
			SET_PB_DZOOM_RATIO(zmFactor12x[zmFactorIdx]);
			appPlayOsd_DigitalZoomWinDraw(panCenX, panCenY, (UINT16)imgWidth, (UINT16)imgHeight, zmFactor12x[zmFactorIdx], gpbImgCb.imgOrientDim);
			#endif
			sp5kHostMsgReceive(&msg, &param);
			#if (KIT_WITH_HDMI)
			if(zmFactorIdx==0 || psPBZoom.wZmTblIdx==0)
			{
				doFirstTime=TRUE;
				zmFactorIdx=0;
				psPBZoom.wZFactor=0;
				appPbZPanStClose();
				appStateChange(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
				return;
			}
			#else
			if(zmFactorIdx==0)
			{
				doFirstTime=TRUE;
				zmFactorIdx=0;
				appPbZPanStClose();
				appStateChange(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
				return;
			}
			#endif
		} while (IS_KEY_WIDE_PRESSED);
	}
	else{											/* VGA */
		do 
		{
			#if (KIT_WITH_HDMI)
			if (IS_HDMI_IN){
				psPBZoom.wZmTblIdx --;
				psPBZoom.wZFactor = zmFactor2x[psPBZoom.wZmTblIdx];
				appPbZoomFactorSet(psPBZoom.wZFactor);
				appPlayOsd_DigitalZoomWinDraw(psPBZoom.wCenter_H, psPBZoom.wCenter_V, (UINT16)imgWidth, (UINT16)imgHeight, zmFactor2x[psPBZoom.wZmTblIdx], gpbImgCb.imgOrientDim);
				zmFactorIdx=1;
			}else{
				PB_NEXT_DZOOM_RATIO_INDEX(PB_ZOOM_WIDE);
				SET_PB_DZOOM_RATIO(zmFactor2x[zmFactorIdx]);
				appPlayOsd_DigitalZoomWinDraw(panCenX, panCenY, (UINT16)imgWidth, (UINT16)imgHeight, zmFactor2x[zmFactorIdx], gpbImgCb.imgOrientDim);
				psPBZoom.wZmTblIdx=1;
			}
			#else
			PB_NEXT_DZOOM_RATIO_INDEX(PB_ZOOM_WIDE);
			SET_PB_DZOOM_RATIO(zmFactor12x[zmFactorIdx]);
			appPlayOsd_DigitalZoomWinDraw(panCenX, panCenY, (UINT16)imgWidth, (UINT16)imgHeight, zmFactor12x[zmFactorIdx], gpbImgCb.imgOrientDim);
			#endif
			sp5kHostMsgReceive(&msg, &param);
			#if (KIT_WITH_HDMI)
			if(zmFactorIdx==0 || psPBZoom.wZmTblIdx==0)
			{
				doFirstTime=TRUE;
				zmFactorIdx=0;
				psPBZoom.wZFactor=0;
				appPbZPanStClose();
				appStateChange(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
				return;
			}
			#else
			if(zmFactorIdx==0)
			{
				doFirstTime=TRUE;
				zmFactorIdx=0;
				appPbZPanStClose();
				appStateChange(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
				return;
			}
			#endif
		} while (IS_KEY_WIDE_PRESSED);
	}
#endif
}

/*-------------------------------------------------------------------------
 *  File Name : appPbTrim
 *------------------------------------------------------------------------*/
static
UINT32
appPbTrim(
	void
)
{
	UINT32 ret;
#if SPCA5330 
	UINT16 topleft_x, topleft_y, zoomWidth, zoomHeight; 
#else 
	UINT32 topleft_x, topleft_y, zoomWidth, zoomHeight; 
#endif 
	/*-- Validity check
	 * --> check remaining storage capacity
	 * --> card lock ?
	 * --> ZPAN stat is ready ? 
	 */
	/*-- Start Trimming job
	 */
	gpbMainCb.subStat = PB_ZPAN_STAT_TRIM_BUSY;

	zpanUIKeyControl(ZPAN_TRIM_SXN_START);

	ret = FAIL;
	if (IS_FILETYPE_JPG(gpbImgCb.dcfAttr.fileType)) 
	{
		#if SPCA5330
		sp5kStillPlayZoomInfoGet( &topleft_x, &topleft_y, &zoomWidth, &zoomHeight);
		if(panCenX>(zoomWidth>>1))
		{
			topleft_x = panCenX-(zoomWidth>>1);
		}
		else
		{
			topleft_x = 0;
		}
		if(panCenY>(zoomHeight>>1))
		{
			topleft_y = panCenY-(zoomHeight>>1);
		}
		else
		{
			topleft_y = 0;
		}
		#else
		zoomWidth = cropWidth;
		zoomHeight = cropHeight;
		topleft_x = panCenX-(cropWidth>>1);
		topleft_y = panCenY-(cropHeight>>1); 
		#endif
		printf("PB_TRIM: JPG file, (%d, %d), ceter(%d, %d), size(%d x %d) trimmed !!\n", topleft_x, topleft_y,panCenX,panCenY, zoomWidth, zoomHeight);		
		ret = sp5kImageEdit(SP5K_IMGEDIT_CROP, topleft_x, topleft_y, zoomWidth, zoomHeight);
		if (SUCCESS==ret) 
		{
			ret = sp5kImageEdit(SP5K_IMGEDIT_SAVE, 0, 1, 1, 0);
			/* !! %TODO% !! : Wait SP5K_MSG_IMGEDIT_DONE */
		}
	} 
	else if (IS_FILETYPE_AVI(gpbImgCb.dcfAttr.fileType)) 
	{
		ret = sp5kMediaPlayControl(SP5K_MEDIA_PLAY_FRAME_CAPTURE, 0, 0);
	}

	gpbMainCb.subStat = PB_ZPAN_STAT_STANDBY;

	zpanUIKeyControl(ZPAN_TRIM_SXN_END);

	if (ret==SUCCESS) 
	{
		gtotalImgCount++;
		gcurrDcfIdx = gtotalImgCount;
		appPbZPanStClose();
		appStateChange(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
	}
	return ret;
}


/*-------------------------------------------------------------------------
 *  File Name : appPbPan
 *------------------------------------------------------------------------*/
static
void
appPbPan(
	UINT8 panDir
)
{
	UINT32 panStep;
	UINT32 zmFac, bound;
/* Modify by sc.liu */	
	float ratio;

	#if 1
	ratio = ((float)zmFactor8x[zmFactorIdx])/1000;
	cropWidth = (UINT32)(imgWidth/ratio);
	cropHeight= (UINT32)(imgHeight/ratio);
	zmFac = zmFactor8x[zmFactorIdx];
	UINT32 tmpU32 = (zmFac/1000)>>1;
	UINT32 zpanStpIdx = (tmpU32>=PB_ZPAN_IDX_MAX) ? (PB_ZPAN_IDX_MAX-1) : tmpU32;
	panStep = gpbZPanStep[zpanStpIdx];
	#else
	if(3548 < imgWidth){				/* 10M */
		ratio = ((float)zmFactor12x[zmFactorIdx])/1000;
		cropWidth = (UINT32)(imgWidth/ratio);
		cropHeight= (UINT32)(imgHeight/ratio);
		zmFac = zmFactor12x[zmFactorIdx];
		UINT32 tmpU32 = (zmFac/1000)>>1;
		UINT32 zpanStpIdx = (tmpU32>=PB_ZPAN_IDX_MAX) ? (PB_ZPAN_IDX_MAX-1) : tmpU32;
		panStep = gpbZPanStep[zpanStpIdx];
	}
	else if(3164 < imgWidth && imgWidth < 3500){		/* 8M */
		ratio = ((float)zmFactor10x[zmFactorIdx])/1000;
		cropWidth = (UINT32)(imgWidth/ratio);
		cropHeight= (UINT32)(imgHeight/ratio);
		zmFac = zmFactor10x[zmFactorIdx];
		UINT32 tmpU32 = (zmFac/1000)>>1;
		UINT32 zpanStpIdx = (tmpU32>=PB_ZPAN_IDX_MAX) ? (PB_ZPAN_IDX_MAX-1) : tmpU32;
		panStep = gpbZPanStep[zpanStpIdx];
	}
	else if(2460 < imgWidth && imgWidth < 3100){		/* 6M */
		ratio = ((float)zmFactor8x[zmFactorIdx])/1000;
		cropWidth = (UINT32)(imgWidth/ratio);
		cropHeight= (UINT32)(imgHeight/ratio);
		zmFac = zmFactor8x[zmFactorIdx];
		UINT32 tmpU32 = (zmFac/1000)>>1;
		UINT32 zpanStpIdx = (tmpU32>=PB_ZPAN_IDX_MAX) ? (PB_ZPAN_IDX_MAX-1) : tmpU32;
		panStep = gpbZPanStep[zpanStpIdx];
	}
	else if(1948 < imgWidth && imgWidth < 2460){		/* 3M */
		ratio = ((float)zmFactor6x[zmFactorIdx])/1000;
		cropWidth = (UINT32)(imgWidth/ratio);
		cropHeight= (UINT32)(imgHeight/ratio);
		zmFac = zmFactor6x[zmFactorIdx];
		UINT32 tmpU32 = (zmFac/1000)>>1;
		UINT32 zpanStpIdx = (tmpU32>=PB_ZPAN_IDX_MAX) ? (PB_ZPAN_IDX_MAX-1) : tmpU32;
		panStep = gpbZPanStep[zpanStpIdx];
	}
	else if(924 < imgWidth && imgWidth < 1948){			/* 2M */
		ratio = ((float)zmFactor4x[zmFactorIdx])/1000;
		cropWidth = (UINT32)(imgWidth/ratio);
		cropHeight= (UINT32)(imgHeight/ratio);
		zmFac = zmFactor4x[zmFactorIdx];
		UINT32 tmpU32 = (zmFac/1000)>>1;
		UINT32 zpanStpIdx = (tmpU32>=PB_ZPAN_IDX_MAX) ? (PB_ZPAN_IDX_MAX-1) : tmpU32;
		panStep = gpbZPanStep[zpanStpIdx];
	}	
	else{												/* VGA */
		ratio = ((float)zmFactor2x[zmFactorIdx])/1000;
		cropWidth = (UINT32)(imgWidth/ratio);
		cropHeight= (UINT32)(imgHeight/ratio);
		zmFac = zmFactor2x[zmFactorIdx];
		UINT32 tmpU32 = (zmFac/1000)>>1;
		UINT32 zpanStpIdx = (tmpU32>=PB_ZPAN_IDX_MAX) ? (PB_ZPAN_IDX_MAX-1) : tmpU32;
		panStep = gpbZPanStep[zpanStpIdx];
	}
	#endif
	
	UINT16 x,y,width,height;

	HLPrintf1("ZPan: panStep(%d) \n", panStep);

	zpanUIKeyControl(ZPAN_PAN_SXN_START);

	switch (panDir) 
	{
		case PB_ZPAN_UP:
		{
			doFirstTime=TRUE;
			bound = (cropHeight)>>1;
			if (panCenY>(SINT32)(bound+panStep)) 
			{
				panCenY -= (SINT32)panStep;
			} 
			else 
			{
				PB_DBG("ZPan: UP bounded !!\n");
				panCenY = (SINT32)bound-1;
			}

			if (IS_KEY_UP_PRESSED) 
			{
				sp5kHostMsgSend(APP_UI_MSG_PB_PAN_UP, 0);
			}
			break;
		}
		case PB_ZPAN_DOWN:
		{
			doFirstTime=TRUE;
			bound = imgHeight - (cropHeight>>1);
			if (panCenY+panStep < (bound-1)) 
			{
				panCenY += panStep;
			} 
			else 
			{
				PB_DBG("ZPan: DOWN bounded !!\n");
				panCenY = bound-1;
			}

			if (IS_KEY_DOWN_PRESSED) 
			{
				sp5kHostMsgSend(APP_UI_MSG_PB_PAN_DOWN, 0);
			}
			break;
		}
		case PB_ZPAN_LEFT:
		{
			doFirstTime=TRUE;
			sp5kStillPlayZoomInfoGet( &x, &y, &width, &height);
			bound = cropWidth>>1;
			if (panCenX>(SINT32)(bound+panStep)) 
			{
				panCenX -= (SINT32)panStep;
			}
			else 
			{
				PB_DBG("ZPan: Left bounded !!\n");
				panCenX = (SINT32)bound-1;
			}

			if (IS_KEY_LEFT_PRESSED) 
			{
				sp5kHostMsgSend(APP_UI_MSG_PB_PAN_LEFT, 0);
			}
			break;
		}
		case PB_ZPAN_RIGHT:
		{
			doFirstTime=TRUE;
			bound = imgWidth - (cropWidth>>1);
			if (panCenX+panStep < (bound-1)) 
			{
				panCenX += panStep;
			} 
			else 
			{
				PB_DBG("ZPan: Right bounded !!\n");
				panCenX = bound-1;
			}

			if (IS_KEY_RIGHT_PRESSED) 
			{
				sp5kHostMsgSend(APP_UI_MSG_PB_PAN_RIGHT, 0);
			}
			break;
		}
		default:
			PB_DBG("ZPan: undefined Pan !! \n");
			break;
	}
	
	SET_PB_PAN_CEN(panCenX, panCenY);
#if 0
	sp5kStillPlayZoomInfoGet( &x, &y, &width, &height);
	printf("panCenX = %d, panCenY = %d\n", panCenX, panCenY);
	if(panCenX > (width >> 1))
	{
		x = panCenX - (width >> 1);
	}
	else
	{
		x=0;
	}
			
	if(panCenY >((height) >> 1))
	{
		y = panCenY - ((height) >> 1);
	}
	else
	{
		y=0;
	}
	printf("PBzm pan ROI[%d,%d,%d,%d]\n",x,y,width,height);
	sp5kZoomRoiSet(SP5K_MODE_STILL_PLAYBACK,x,y,width,height);
#endif
	#if 1
	appPlayOsd_DigitalZoomWinDraw(panCenX, panCenY, (UINT16)imgWidth, (UINT16)imgHeight, zmFactor8x[zmFactorIdx], gpbImgCb.imgOrientDim);
	#else
	if(3548 < imgWidth){ 			/* 10M */
		appPlayOsd_DigitalZoomWinDraw(panCenX, panCenY, (UINT16)imgWidth, (UINT16)imgHeight, zmFactor12x[zmFactorIdx], gpbImgCb.imgOrientDim);
	}
	else if(3164 < imgWidth && imgWidth < 3500){		/* 8M */
		appPlayOsd_DigitalZoomWinDraw(panCenX, panCenY, (UINT16)imgWidth, (UINT16)imgHeight, zmFactor10x[zmFactorIdx], gpbImgCb.imgOrientDim);
	}
	else if(2460 < imgWidth && imgWidth < 3100){		/* 6M */
		appPlayOsd_DigitalZoomWinDraw(panCenX, panCenY, (UINT16)imgWidth, (UINT16)imgHeight, zmFactor8x[zmFactorIdx], gpbImgCb.imgOrientDim);
	}
	else if(1948 < imgWidth && imgWidth < 2460){		/* 3M */
		appPlayOsd_DigitalZoomWinDraw(panCenX, panCenY, (UINT16)imgWidth, (UINT16)imgHeight, zmFactor6x[zmFactorIdx], gpbImgCb.imgOrientDim);
	}
	else if(924 < imgWidth && imgWidth < 1948){ 		/* 2M */
		appPlayOsd_DigitalZoomWinDraw(panCenX, panCenY, (UINT16)imgWidth, (UINT16)imgHeight, zmFactor4x[zmFactorIdx], gpbImgCb.imgOrientDim);
	}
	else{												/* VGA */
		appPlayOsd_DigitalZoomWinDraw(panCenX, panCenY, (UINT16)imgWidth, (UINT16)imgHeight, zmFactor2x[zmFactorIdx], gpbImgCb.imgOrientDim);
	}
	#endif
	PB_LOG("\nZPAN: panCenX(%d), panCenY(%d), panStep(%d), cropWidth(%d), cropHeight(%d) \n\n", panCenX, panCenY, panStep, cropWidth, cropHeight);

	zpanUIKeyControl(ZPAN_PAN_SXN_END);

	#if HOST_DBG
	PB_VERBOSE("%s : Pan Param ==================\n", __FUNCTION__);
	PB_VERBOSE("imgW(%d), imgH(%d), zmFac(%d), panStep(%d) \n", imgWidth, imgHeight, zmFac, panStep);
	PB_VERBOSE("zoomImgW(%d), zoomImgH(%d), bound(%d) \n", imgWidth*1000/zmFac, imgHeight*1000/zmFac, bound);
	PB_VERBOSE("panCenX(%d), panCenY(%d) \n", panCenX, panCenY);
	PB_VERBOSE("\n");
	#endif
}

/*-------------------------------------------------------------------------
 *  File Name : appPbZoomIn
 *------------------------------------------------------------------------*/
static
void
appPbZoomIn(
	void
)
{
	zpanUIKeyControl(ZPAN_ZOOM_SXN_START);
	
	#if SPCA6330
	sp5kStillPlayCfgSet(SP5K_SPB_SCALE_QUALITY,SP5K_SPB_SCALE_QUALITY_FAST);
	#endif
	playZoomIn();
#if 1    /*junqiong.hu@20121220 add for zoom 8x  through fair and foul*/
	PB_LOG("\nZmIN, fac(%d), idx(%d), cropW(%d), cropH(%d) \n\n", zmFactor8x[zmFactorIdx], zmFactorIdx, cropWidth, cropHeight);
#else
	if(3548 < imgWidth){ 			/* 10M */
		PB_LOG("\nZmIN, fac(%d), idx(%d), cropW(%d), cropH(%d) \n\n", zmFactor12x[zmFactorIdx], zmFactorIdx, cropWidth, cropHeight);
	}
	else if(3164 < imgWidth && imgWidth < 3500){		/* 8M */
		PB_LOG("\nZmIN, fac(%d), idx(%d), cropW(%d), cropH(%d) \n\n", zmFactor10x[zmFactorIdx], zmFactorIdx, cropWidth, cropHeight);
	}
	else if(2460 < imgWidth && imgWidth < 3100){		/* 6M */
		PB_LOG("\nZmIN, fac(%d), idx(%d), cropW(%d), cropH(%d) \n\n", zmFactor8x[zmFactorIdx], zmFactorIdx, cropWidth, cropHeight);
	}
	else if(1948 < imgWidth && imgWidth < 2460){		/* 3M */
		PB_LOG("\nZmIN, fac(%d), idx(%d), cropW(%d), cropH(%d) \n\n", zmFactor6x[zmFactorIdx], zmFactorIdx, cropWidth, cropHeight);
	}
	else if(924 < imgWidth && imgWidth < 1948){			/* 2M */
		PB_LOG("\nZmIN, fac(%d), idx(%d), cropW(%d), cropH(%d) \n\n", zmFactor4x[zmFactorIdx], zmFactorIdx, cropWidth, cropHeight);
	}
	else{												/* VGA */
		PB_LOG("\nZmIN, fac(%d), idx(%d), cropW(%d), cropH(%d) \n\n", zmFactor2x[zmFactorIdx], zmFactorIdx, cropWidth, cropHeight);
	}
#endif	
	zpanUIKeyControl(ZPAN_ZOOM_SXN_END);
}

/*-------------------------------------------------------------------------
 *  File Name : appPbZoomOut
 *------------------------------------------------------------------------*/
static
void
appPbZoomOut(
	void
)
{
	zpanUIKeyControl(ZPAN_ZOOM_SXN_START);

	#if SPCA6330
	sp5kStillPlayCfgSet(SP5K_SPB_SCALE_QUALITY,SP5K_SPB_SCALE_QUALITY_FAST);
	#endif
	playZoomOut();
#if 1   /*junqiong.hu@20121220 add for zoom 8x  through fair and foul*/
	PB_LOG("\nZ-OUT, fac(%d), idx(%d), cropW(%d), cropH(%d) \n\n", zmFactor8x[zmFactorIdx], zmFactorIdx, cropWidth, cropHeight);
#else
	if(3548 < imgWidth){ 			/* 10M */
		PB_LOG("\nZ-OUT, fac(%d), idx(%d), cropW(%d), cropH(%d) \n\n", zmFactor12x[zmFactorIdx], zmFactorIdx, cropWidth, cropHeight);
	}
	else if(3164 < imgWidth && imgWidth < 3500){		/* 8M */
		PB_LOG("\nZ-OUT, fac(%d), idx(%d), cropW(%d), cropH(%d) \n\n", zmFactor10x[zmFactorIdx], zmFactorIdx, cropWidth, cropHeight);
	}
	else if(2460 < imgWidth && imgWidth < 3100){		/* 6M */
		PB_LOG("\nZ-OUT, fac(%d), idx(%d), cropW(%d), cropH(%d) \n\n", zmFactor8x[zmFactorIdx], zmFactorIdx, cropWidth, cropHeight);
	}
	else if(1948 < imgWidth && imgWidth < 2460){		/* 3M */
		PB_LOG("\nZ-OUT, fac(%d), idx(%d), cropW(%d), cropH(%d) \n\n", zmFactor6x[zmFactorIdx], zmFactorIdx, cropWidth, cropHeight);
	}
	else if(924 < imgWidth && imgWidth < 1948){			/* 2M */
		PB_LOG("\nZ-OUT, fac(%d), idx(%d), cropW(%d), cropH(%d) \n\n", zmFactor4x[zmFactorIdx], zmFactorIdx, cropWidth, cropHeight);
	}
	else{												/* VGA */
		PB_LOG("\nZ-OUT, fac(%d), idx(%d), cropW(%d), cropH(%d) \n\n", zmFactor2x[zmFactorIdx], zmFactorIdx, cropWidth, cropHeight);
	}
#endif
	zpanUIKeyControl(ZPAN_ZOOM_SXN_END);
}

/*-------------------------------------------------------------------------
 *  File Name : appPbZPanStClose
 *------------------------------------------------------------------------*/
static
void
appPbZPanStClose(
	void
)
{
	PB_VERBOSE("ZPan: State closing \n");
	gpbMainCb.subStat = PB_ZPAN_STAT_NULL;
}

/*-------------------------------------------------------------------------
 *  File Name : appPbZPanStInit
 *------------------------------------------------------------------------*/
static
UINT32
appPbZPanStInit(
	void
)
{
	float ratio;

	#if PLAYBACK_ROTATE_BY_EXIF_TAG
		if(gpbImgCb.imgOrientDim & PB_IMG_ORIENT_HORIZONTAL)
		{
			imgWidth = gpbImgCb.imgWidth;
			imgHeight = gpbImgCb.imgHeight;
		}
		else
		{
			imgWidth = gpbImgCb.imgHeight;
			imgHeight = gpbImgCb.imgWidth;
		}
	#else
		imgWidth = gpbImgCb.imgWidth;
		imgHeight = gpbImgCb.imgHeight;
	#endif

	#if (KIT_WITH_HDMI)
	if (IS_HDMI_IN){
		psPBZoom.wImgWidth = gpbImgCb.imgWidth;
		psPBZoom.wImgHigh = gpbImgCb.imgHeight;
	}
	psPBZoom.wZmTblIdx = 0;
	#endif
	doFirstTime = TRUE;
	panCenX = imgWidth >> 1;
	panCenY = imgHeight >> 1;

	zmFactorIdx = 0;

	/*-- Calculate the max acceptable zoom factor
	 *   according to image size.
	 */
	ratio = (imgWidth*imgHeight)/PB_ZOOM_SIZE_MIN;
	zmMaxFactor = (UINT32)(ratio*1000);
	#if 1
	PB_DBG("ZPan: Param =====================\n");
	PB_DBG("imgW & imgH : %d, %d \n", imgWidth, imgHeight);
	PB_DBG("center X & Y : %d, %d \n", panCenX, panCenY);
	PB_DBG("max factor : %d \n", zmMaxFactor);
	PB_DBG("\n");
	#endif

	sp5kZoomCfgSet(SP5K_DZOOM_RATIO_TYPE, SP5K_DZOOM_RATIO_KEEP_INSIDE, 0, 0, 0);
	#if (KIT_WITH_HDMI)
	if(IS_HDMI_IN){
		psPBZoom.wZFactor	= zmFactor8x[psPBZoom.wZmTblIdx];
		appPbZoomRoiSet(1000);		/* modify zoom */
		appPbZoomFactorSet(psPBZoom.wZFactor);
	}else{
		sp5kZoomFactorSet(SP5K_MODE_STILL_PLAYBACK, 1000);
	}
	#else
	sp5kZoomFactorSet(SP5K_MODE_STILL_PLAYBACK, 1000);
	#endif
	PB_OSD_CLR;

	if (gpbMainCb.subStat==PB_ZPAN_STAT_STANDBY) 
	{
		appPbZoomIn();
	}
	appOsdLib_OsdLayIconDraw(140,110, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_MAGNIFY);

	return SUCCESS;
}



/*-------------------------------------------------------------------------
 *  File Name : appPbZPanStOnMsg
 *------------------------------------------------------------------------*/
void
appPbZPanStOnMsg(
	UINT32 msg,
	UINT32 param
)
{
	UINT32 tmpU32;

	PB_VERBOSE("ZPan_OnMsg: 0x%08x, 0x%08x ... \n", msg, param);

	switch (msg) 
	{
	case SP5K_MSG_STILLPLAY_DONE:
		tmpU32 = appPbStillPlayDoneParse(param);
		if (tmpU32 & PB_DONE_ERROR) 
		{
			PB_DBG("%s: !! WARNING !!, PB_DONE error(0x%x) !!\n", __FUNCTION__, tmpU32);
			PB_BLACK_BKGND;
			break;
		}

		if (IS_PB_MAIN_PLAYING && tmpU32==PB_DONE_MAIN_FINISH) 
		{
			PB_DBG("MAIN Done !!\n");
			gpbMainCb.playStat = PB_PLAY_STAT_STANDBY;
			gpbMainCb.isPlayDone = TRUE;
			gpbImgCb.isMainImgOK = TRUE;
			gpbMainCb.subStat = PB_ZPAN_STAT_STANDBY;
			playZoomIn();
		}
		break;
	case APP_UI_MSG_PB_ZOOM_IN:
		appPbZoomIn();
		break;
	case APP_UI_MSG_PB_ZOOM_OUT:
		appPbZoomOut();
		break;
	case APP_UI_MSG_PB_PAN_UP:
		#if (KIT_WITH_HDMI)
		if (IS_HDMI_IN){
			appPbPanControl(PB_ZPAN_UP);
			appPbPanCenterSet(psPBZoom.wCenter_H, psPBZoom.wCenter_V);
		}else{
			appPbPan(PB_ZPAN_UP);
		}
		#else
		appPbPan(PB_ZPAN_UP);
		#endif
		break;
	case APP_UI_MSG_PB_PAN_DOWN:
		#if (KIT_WITH_HDMI)
		if (IS_HDMI_IN){
			appPbPanControl(PB_ZPAN_DOWN);
			appPbPanCenterSet(psPBZoom.wCenter_H, psPBZoom.wCenter_V);
		}else{
			appPbPan(PB_ZPAN_DOWN);
		}
		#else
		appPbPan(PB_ZPAN_DOWN);
		#endif
		break;
	case APP_UI_MSG_PB_PAN_LEFT:
		#if (KIT_WITH_HDMI)
		if (IS_HDMI_IN){
			appPbPanControl(PB_ZPAN_LEFT);
			appPbPanCenterSet(psPBZoom.wCenter_H, psPBZoom.wCenter_V);
		}else{
			appPbPan(PB_ZPAN_LEFT);
		}
		#else
		appPbPan(PB_ZPAN_LEFT);
		#endif
		break;
	case APP_UI_MSG_PB_PAN_RIGHT:
		#if (KIT_WITH_HDMI)
		if (IS_HDMI_IN){
			appPbPanControl(PB_ZPAN_RIGHT);
			appPbPanCenterSet(psPBZoom.wCenter_H, psPBZoom.wCenter_V);
		}else{
			appPbPan(PB_ZPAN_RIGHT);
		}
		#else
		appPbPan(PB_ZPAN_RIGHT);
		#endif
		break;
	default :
		break;
	}
}


/*-------------------------------------------------------------------------
 *  File Name : appPbZPanStOnKey
 *------------------------------------------------------------------------*/
void
appPbZPanStOnKey(
	UINT32 msg,
	UINT32 param
)
{
	UINT32 freeClust, fileClust;
	UINT16 x,y,width,hight;
	
	if (gpbMainCb.subStat==PB_ZPAN_STAT_TRIM_CONFIRM_MENU) 
	{
		if (msg==APP_KEY_PRESS_UP) 
		{
			PB_BEEP;
			appPlayOsd_DlgBoxScroll(TRUE);
		} 
		else if (msg==APP_KEY_PRESS_DOWN) 
		{
			PB_BEEP;
			appPlayOsd_DlgBoxScroll(FALSE);
		} 
		else if (msg==APP_KEY_PRESS_SET) 
		{
			PB_BEEP;
			if (0==appPlayOsd_DlgBoxSelectGet()) 
			{
				/* YES */
				sp5kStillPlayZoomInfoGet( &x, &y, &width, &hight);
				freeClust = appPbDiskFreeClusterGet(gpbDiskCb.dskInfo.actDsk);
				fileClust = appPbBytes2ClusterCalculate(width*hight);
				/*-- Check remaining disk space !! */
				if(freeClust < fileClust)
				{
					appPbErrorProcess(PB_ERR_DISK_FULL);
					PB_OSD_CLR;
					PB_PIP_0_CLR;
					appPlayOsd_DigitalZoomWinDraw(panCenX, panCenY, (UINT16)imgWidth, (UINT16)imgHeight, zmFactor8x[zmFactorIdx], gpbImgCb.imgOrientDim);
					gpbMainCb.subStat = PB_ZPAN_STAT_STANDBY;
				}
				else
				{
					appPbMsg_BusyWait();
					appPbTrim();
					PB_OSD_CLR;
				}
			} 
			else 
			{
				/* NO */
				PB_OSD_CLR;
				PB_PIP_0_CLR;

				#if 1
				appPlayOsd_DigitalZoomWinDraw(panCenX, panCenY, (UINT16)imgWidth, (UINT16)imgHeight, zmFactor8x[zmFactorIdx], gpbImgCb.imgOrientDim);
				#else
				if(3548 < imgWidth){ 			/* 10M */
					appPlayOsd_DigitalZoomWinDraw(panCenX, panCenY, (UINT16)imgWidth, (UINT16)imgHeight, zmFactor12x[zmFactorIdx], gpbImgCb.imgOrientDim);
				}
				else if(3164 < imgWidth && imgWidth < 3500){		/* 8M */
					appPlayOsd_DigitalZoomWinDraw(panCenX, panCenY, (UINT16)imgWidth, (UINT16)imgHeight, zmFactor10x[zmFactorIdx], gpbImgCb.imgOrientDim);
				}
				else if(2460 < imgWidth && imgWidth < 3100){		/* 6M */
					appPlayOsd_DigitalZoomWinDraw(panCenX, panCenY, (UINT16)imgWidth, (UINT16)imgHeight, zmFactor8x[zmFactorIdx], gpbImgCb.imgOrientDim);
				}
				else if(1948 < imgWidth && imgWidth < 2460){		/* 3M */
					appPlayOsd_DigitalZoomWinDraw(panCenX, panCenY, (UINT16)imgWidth, (UINT16)imgHeight, zmFactor6x[zmFactorIdx], gpbImgCb.imgOrientDim);
				}
				else if(924 < imgWidth && imgWidth < 1948){			/* 2M */
					appPlayOsd_DigitalZoomWinDraw(panCenX, panCenY, (UINT16)imgWidth, (UINT16)imgHeight, zmFactor4x[zmFactorIdx], gpbImgCb.imgOrientDim);
				}
				else{												/* VGA */
					appPlayOsd_DigitalZoomWinDraw(panCenX, panCenY, (UINT16)imgWidth, (UINT16)imgHeight, zmFactor2x[zmFactorIdx], gpbImgCb.imgOrientDim);
				}
				#endif
				gpbMainCb.subStat = PB_ZPAN_STAT_STANDBY;
			}
		}
		return;
	}   
	switch (msg) 
	{
		case APP_KEY_PRESS_SET:
			PB_BEEP;
			PB_OSD_CLR;
			TRIM_ConfirmDlgConfig();
			TRIM_ConfirmDlgShow();
			gpbMainCb.subStat = PB_ZPAN_STAT_TRIM_CONFIRM_MENU;
			break;
		case APP_KEY_PRESS_S2:
			break;
		case APP_KEY_PRESS_PB:
			#if !CAM_TYPE_CVR
			appBeep(TRUE,FALSE);
			zmFactorIdx=0;
			if(3548 < imgWidth){ 			/* 10M */
				#if (KIT_WITH_HDMI)
				if (IS_HDMI_IN){
					psPBZoom.wZmTblIdx = 0;
					psPBZoom.wZFactor	= zmFactor12x[psPBZoom.wZmTblIdx];	
					appPbZoomRoiSet(1000);
					appPbZoomFactorSet(psPBZoom.wZFactor);
				}else{
					sp5kZoomFactorSet(SP5K_MODE_STILL_PLAYBACK, zmFactor12x[zmFactorIdx]);
				}
				#else
				sp5kZoomFactorSet(SP5K_MODE_STILL_PLAYBACK, zmFactor12x[zmFactorIdx]);
				#endif
			}
			else if(3164 < imgWidth && imgWidth < 3500){		/* 8M */
				#if (KIT_WITH_HDMI)
				if (IS_HDMI_IN){
					psPBZoom.wZmTblIdx = 0;
					psPBZoom.wZFactor	= zmFactor10x[psPBZoom.wZmTblIdx];	
					appPbZoomRoiSet(1000);
					appPbZoomFactorSet(psPBZoom.wZFactor);
				}else{
					sp5kZoomFactorSet(SP5K_MODE_STILL_PLAYBACK, zmFactor10x[zmFactorIdx]);
				}
				#else
				sp5kZoomFactorSet(SP5K_MODE_STILL_PLAYBACK, zmFactor10x[zmFactorIdx]);
				#endif
			}
			else if(2460 < imgWidth && imgWidth < 3100){		/* 6M */
				#if (KIT_WITH_HDMI)
				if (IS_HDMI_IN){
					psPBZoom.wZmTblIdx = 0;
					psPBZoom.wZFactor	= zmFactor8x[psPBZoom.wZmTblIdx];	
					appPbZoomRoiSet(1000);
					appPbZoomFactorSet(psPBZoom.wZFactor);
				}else{
					sp5kZoomFactorSet(SP5K_MODE_STILL_PLAYBACK, zmFactor8x[zmFactorIdx]);
				}
				#else
				sp5kZoomFactorSet(SP5K_MODE_STILL_PLAYBACK, zmFactor8x[zmFactorIdx]);
				#endif
			}
			else if(1948 < imgWidth && imgWidth < 2460){		/* 3M */
				#if (KIT_WITH_HDMI)
				if (IS_HDMI_IN){
					psPBZoom.wZmTblIdx = 0;
					psPBZoom.wZFactor	= zmFactor6x[psPBZoom.wZmTblIdx];	
					appPbZoomRoiSet(1000);
					appPbZoomFactorSet(psPBZoom.wZFactor);
				}else{
					sp5kZoomFactorSet(SP5K_MODE_STILL_PLAYBACK, zmFactor6x[zmFactorIdx]);
				}
				#else
				sp5kZoomFactorSet(SP5K_MODE_STILL_PLAYBACK, zmFactor6x[zmFactorIdx]);
				#endif
			}
			else if(924 < imgWidth && imgWidth < 1948){			/* 2M */
				#if (KIT_WITH_HDMI)
				if (IS_HDMI_IN){
					psPBZoom.wZmTblIdx = 0;
					psPBZoom.wZFactor	= zmFactor4x[psPBZoom.wZmTblIdx];	
					appPbZoomRoiSet(1000);
					appPbZoomFactorSet(psPBZoom.wZFactor);
				}else{
					sp5kZoomFactorSet(SP5K_MODE_STILL_PLAYBACK, zmFactor4x[zmFactorIdx]);
				}
				#else
				sp5kZoomFactorSet(SP5K_MODE_STILL_PLAYBACK, zmFactor4x[zmFactorIdx]);
				#endif
			}
			else{												/* VGA */
				#if (KIT_WITH_HDMI)
				if (IS_HDMI_IN){
					psPBZoom.wZmTblIdx = 0;
					psPBZoom.wZFactor	= zmFactor2x[psPBZoom.wZmTblIdx];
					appPbZoomRoiSet(1000);
					appPbZoomFactorSet(psPBZoom.wZFactor);
				}else{
					sp5kZoomFactorSet(SP5K_MODE_STILL_PLAYBACK, zmFactor2x[zmFactorIdx]);
				}
				#else
				sp5kZoomFactorSet(SP5K_MODE_STILL_PLAYBACK, zmFactor2x[zmFactorIdx]);
				#endif
			}
			doFirstTime=TRUE;
			appPbZPanStClose();
			appStateChange(APP_STATE_PB_MAIN,STATE_PARAM_NORMAL_INIT);
			#endif
			break;
		case APP_KEY_PRESS_UP:
			appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR,PAL_NOCOLOR,PAL_NOCOLOR,PAL_BLEND_100);
			appOsdLib_OsdLayerShapeClear(APP_OSD_SHAPE_RECT,140,110,36,32,0,0); 	
			#if (KIT_WITH_HDMI)
			if (IS_HDMI_IN){
				appPbPanControl(PB_ZPAN_UP);
				appPbPanCenterSet(psPBZoom.wCenter_H, psPBZoom.wCenter_V);
			}else{
				appPbPan(PB_ZPAN_UP);
			}
			#else
			appPbPan(PB_ZPAN_UP);
			#endif
			break;
		case APP_KEY_PRESS_DOWN:
			appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR,PAL_NOCOLOR,PAL_NOCOLOR,PAL_BLEND_100);
			appOsdLib_OsdLayerShapeClear(APP_OSD_SHAPE_RECT,140,110,36,32,0,0); 	
			#if (KIT_WITH_HDMI)
			if (IS_HDMI_IN){
				appPbPanControl(PB_ZPAN_DOWN);
				appPbPanCenterSet(psPBZoom.wCenter_H, psPBZoom.wCenter_V);
			}else{
				appPbPan(PB_ZPAN_DOWN);
			}
			#else
			appPbPan(PB_ZPAN_DOWN);
			#endif
			break;
		case APP_KEY_PRESS_LEFT:
			appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR,PAL_NOCOLOR,PAL_NOCOLOR,PAL_BLEND_100);
			appOsdLib_OsdLayerShapeClear(APP_OSD_SHAPE_RECT,140,110,36,32,0,0); 	
			#if (KIT_WITH_HDMI)
			if (IS_HDMI_IN){
				appPbPanControl(PB_ZPAN_LEFT);
				appPbPanCenterSet(psPBZoom.wCenter_H, psPBZoom.wCenter_V);
			}else{
				appPbPan(PB_ZPAN_LEFT);
			}
			#else
			appPbPan(PB_ZPAN_LEFT);
			#endif
			break;
		case APP_KEY_PRESS_RIGHT:
			appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR,PAL_NOCOLOR,PAL_NOCOLOR,PAL_BLEND_100);
			appOsdLib_OsdLayerShapeClear(APP_OSD_SHAPE_RECT,140,110,36,32,0,0); 	
			#if (KIT_WITH_HDMI)
			if (IS_HDMI_IN){
				appPbPanControl(PB_ZPAN_RIGHT);
				appPbPanCenterSet(psPBZoom.wCenter_H, psPBZoom.wCenter_V);
			}else{
				appPbPan(PB_ZPAN_RIGHT);
			}
			#else
			appPbPan(PB_ZPAN_RIGHT);
			#endif
			break;
		case APP_KEY_PRESS_TELE:
			appOsdLib_OsdLayIconDraw(140,110, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_MAGNIFY);
			appPbZoomIn();
			break;
		case APP_KEY_PRESS_WIDE:
			appOsdLib_OsdLayIconDraw(140,110,SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_MAGNIFY);
			appPbZoomOut();
			break;
		default:
			break;
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appPbZoomPanState
 *------------------------------------------------------------------------*/
void
appPbZoomPanState(
	UINT32 msg,
	UINT32 param
)
{

	switch (msg) 
	{
	case APP_STATE_MSG_INIT:
		RPrintf("appPbZoomPanState");
		appPbZPanStInit();
		break;
	case APP_STATE_MSG_CLOSE:
		appPbZPanStClose();
		appStateCloseDone();
		break;
	default:
		if (IS_APP_KEY_MSG(msg)) 
		{
			appPbZPanStOnKey(msg,param);
		}
		else
		{
			appPbZPanStOnMsg(msg,param);
		}
		break;
	}

}


