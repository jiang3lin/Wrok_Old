/**************************************************************************
 *         Copyright(c) 2012 by iCatch Technology Co., Ltd.              *
 *                                                                        *
 *  This software is copyrighted by and is the property of iCatch Tech-  *
 *  nology Co., Ltd. All rights are reserved by iCatch Technology Co.,   *
 *  Ltd. This software may only be used in accordance with the            *
 *  corresponding license agreement. Any unauthorized use, duplication,   *
 *  distribution, or disclosure of this software is expressly forbidden.  *
 *                                                                        *
 *  This Copyright notice "M U S T" not be removed or modified without    *
 *  prior written consent of iCatch Technology Co., Ltd.                 *
 *                                                                        *
 *  iCatch Technology Co., Ltd. reserves the right to modify this        *
 *  software without notice.                                              *
 *                                                                        *
 *  iCatch Technology Co., Ltd.                                          *
 *  19-1, Innovation First Road, Science-Based Industrial Park,           *
 *  Hsin-Chu, Taiwan, R.O.C.                                              *
 *                                                                        *
 *************************************************************************/

#include "app_com_def.h"
#include "app_com_api.h"

#include "app_menu.h"
#include "app_menu_event.h"
#include "app_menu_core.h"
#include "app_menu_tab.h"
#include "app_menu_disp.h"
#include "app_ui_para.h"
#include "app_osd_api.h"
#include "app_icon_def.h"
#include "app_pal.h"

#include "gpio_custom.h"

#include "sp5k_gfx_api.h"
#include "sp5k_pip_api.h"
#include "sp5k_rsvblk_api.h"
#include "app_version.h"
#include "app_playback_osd.h"
#include "app_osdfont.h"
#define ROUND_DOWN_TO_DIVISIBLE(num,div) \
	( (UINT32)(num) & -(UINT32)(div) )
#define ROUND_UP_TO_DIVISIBLE(num,div) \
	ROUND_DOWN_TO_DIVISIBLE( (UINT32)(num) + (div) - 1, div )


/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
#if HDMI_MENU_BY_JPG 
typedef struct menuSrcHDMI_s{
	UINT8* fileName;
	UINT32 pos[4];  /* X pos,Y pos, width ,height  */	
}menuSrcHDMI_t; 

enum{
	HDMI_PIP_MAIN		=	SP5K_GFX_PAGE_PIP_MAIN,		
	HDMI_MENU_LAYER0	=	SP5K_GFX_PAGE_PIP_BASE+0,		
	HDMI_MENU_LAYER1	=	SP5K_GFX_PAGE_PIP_BASE+1,	
	HDMI_MENU_LAYER2	=	SP5K_GFX_PAGE_PIP_BASE+2,	
	HDMI_MENU_LAYER3	=	SP5K_GFX_PAGE_PIP_BASE+3,
	HDMI_MENU_LAYER4	=	SP5K_GFX_PAGE_PIP_BASE+4
};

enum {
	/* reference the order of ID_ICONFILE_MODE_HDMI -- s */
	MENU_STILL_HDMI,
	MENU_VIDEO_HDMI,
	MENU_AUDIO_HDMI,
	MENU_PLAYBACK_HDMI,
	MENU_SETUP_HDMI,
	MENU_PRINTER_HDMI,
	/* reference the order of ID_ICONFILE_MODE_HDMI -- e */
	MENU_SCENE_HDMI,

	MENU_TITLE_POS1_HDMI,
	MENU_TITLE_POS2_HDMI,
	MENU_ADJUST_LEFT_HDMI,
	MENU_ADJUST_RIGHT_HDMI,
	MENU_BUTTON_OK_HDMI,
	MENU_BUTTON_MENU_HDMI,
	
	MENU_BODY_HDMI,
	MENU_FIVE_LINES_BAR_HDMI,
	MENU_FOUR_LINES_BODY_HDMI,
	MENU_FOUR_LINES_BAR_HDMI,
	MENU_THREE_LINES_BODY_HDMI,
	MENU_THREE_LINES_BAR_HDMI,
	MENU_TWO_LINES_BODY_HDMI,

	MENU_FIVE_LINES_SCROLL_HDMI,
	MENU_FOUR_LINES_SCROLL_HDMI,

	MENU_ADJUST_DIALOG_HDMI,
	MENU_SOUND_ADJUST_1_HDMI,
	MENU_SOUND_ADJUST_2_HDMI
};

static menuSrcHDMI_t menuSrcFileInfo[] = {	
	[MENU_STILL_HDMI]				=	{"A:\\RO_RES\\UI\\JPG\\STILL.JPG",			{140,25,166,150}},
	[MENU_VIDEO_HDMI]				=	{"A:\\RO_RES\\UI\\JPG\\VIDEO.JPG",			{140,25,166,150}},
	[MENU_AUDIO_HDMI]				=	{"A:\\RO_RES\\UI\\JPG\\AUDIO.JPG",			{140,25,166,150}},
	[MENU_PLAYBACK_HDMI]			=	{"A:\\RO_RES\\UI\\JPG\\PLAYBACK.JPG",		{140,25,166,150}},
	[MENU_SETUP_HDMI]				=	{"A:\\RO_RES\\UI\\JPG\\SETUP.JPG",			{420,25,166,150}},
	[MENU_PRINTER_HDMI]				=	{"A:\\RO_RES\\UI\\JPG\\PRINTER.JPG",		{140,25,166,150}},
	[MENU_SCENE_HDMI]				=	{"A:\\RO_RES\\UI\\JPG\\SCENE.JPG",			{140,25,166,150}},
	
	[MENU_TITLE_POS1_HDMI]			=	{"A:\\RO_RES\\UI\\JPG\\TITLE.JPG",			{20,5,412,220}},
	[MENU_TITLE_POS2_HDMI]			=	{"A:\\RO_RES\\UI\\JPG\\TITLE.JPG",			{300,5,412,220}},
	[MENU_ADJUST_LEFT_HDMI]			=	{"A:\\RO_RES\\UI\\JPG\\ADJUST_LEFT.JPG",	{1200,325,42,76}},
	[MENU_ADJUST_RIGHT_HDMI]		=	{"A:\\RO_RES\\UI\\JPG\\ADJUST_RIGHT.JPG",	{1456,325,42,76}},
	[MENU_BUTTON_OK_HDMI]			=	{"A:\\RO_RES\\UI\\JPG\\BUTTON_OK.JPG",		{150,985,197,80}},
	[MENU_BUTTON_MENU_HDMI]			=	{"A:\\RO_RES\\UI\\JPG\\BUTTON_MENU.JPG",	{1400,985,198,80}},
		
	[MENU_BODY_HDMI]				=	{"A:\\RO_RES\\UI\\JPG\\MENU_BODY.JPG",		{0,0,1920,1080}},
	[MENU_FIVE_LINES_BAR_HDMI]		=	{"A:\\RO_RES\\UI\\JPG\\FIVE_LINES_BAR.JPG",	{0,220,1858,140}},
	[MENU_FOUR_LINES_BODY_HDMI]		=	{"A:\\RO_RES\\UI\\JPG\\FOUR_LINES_BODY.JPG",{200,295,1618,592}},
	[MENU_FOUR_LINES_BAR_HDMI]		=	{"A:\\RO_RES\\UI\\JPG\\FOUR_LINES_BAR.JPG",	{214,307,1534,134}},
	[MENU_THREE_LINES_BODY_HDMI]	=	{"A:\\RO_RES\\UI\\JPG\\THREE_LINES_BODY.JPG",{180,360,1616,588}},
	[MENU_THREE_LINES_BAR_HDMI]		=	{"A:\\RO_RES\\UI\\JPG\\THREE_LINES_BAR.JPG",{0,0,1593,138}},
	[MENU_TWO_LINES_BODY_HDMI]		=	{"A:\\RO_RES\\UI\\JPG\\TWO_LINES_BODY.JPG",	{150,510,1618,448}},
		
	[MENU_FIVE_LINES_SCROLL_HDMI]	=	{"A:\\RO_RES\\UI\\JPG\\FIVE_LINES_SCROLL.JPG",{0,0,22,90}},
	[MENU_FOUR_LINES_SCROLL_HDMI]	=	{"A:\\RO_RES\\UI\\JPG\\FOUR_LINES_SCROLL.JPG",{0,0,21,90}},
		
	[MENU_ADJUST_DIALOG_HDMI]		=	{"A:\\RO_RES\\UI\\JPG\\ADJUST_DIALOG.JPG",	{1244,320,208,86}},
	[MENU_SOUND_ADJUST_1_HDMI]		=	{"A:\\RO_RES\\UI\\JPG\\SOUND_ADJUST_1.JPG",	{0,0,324,26}},
	[MENU_SOUND_ADJUST_2_HDMI]		=	{"A:\\RO_RES\\UI\\JPG\\SOUND_ADJUST_2.JPG",	{0,0,38,64}},
};

enum{
	PRELOAD_MODE_STILL			=	0,
	PRELOAD_MODE_VIDEO			=	1,
	PRELOAD_MODE_AUDIO			=	2,
	PRELOAD_MODE_PLAYBACK		=	3,
	PRELOAD_MODE_SETUP			=	4,
	PRELOAD_MODE_PRINTER		=	5,
	
	PRELOAD_MENU_BODY			=	6,
	PRELOAD_FIVE_LINES_BAR		=	7,
	PRELOAD_FOUR_LINES_BODY		=	8,
	PRELOAD_FOUR_LINES_BAR		=	9,
	
	PRELOAD_BUTTON_OK			=	10,
	PRELOAD_BUTTON_MENU			=	11,
	PRELOAD_MENU_TITLE			=	12
};
#endif

typedef enum {
	ICON_ID_STILL = 0,
	ICON_ID_VIDEO = 1,
	ICON_ID_AUDIO = 2,
	ICON_ID_PLAYBACK,
	ICON_ID_SETUP,
	ICON_ID_DPS
} modeIconID_t;

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
 
#define ID_ICONFILE_MU2LYBAR		"A:\\RO_RES\\UI\\ICON\\MU2LYBAR.SFN"
#define ID_ICONFILE_MUSUSBAR		"A:\\RO_RES\\UI\\ICON\\MUSUSBAR.SFN"
#define ID_ICONFILE_MUSBAR			"A:\\RO_RES\\UI\\ICON\\MUSBAR.SFN"
#define ID_ICONFILE_MUDLGBAR		"A:\\RO_RES\\UI\\ICON\\MUDLGBAR.SFN"
#define ID_ICONFILE_MUBAR			"A:\\RO_RES\\UI\\ICON\\MUBAR.SFN"
#define ID_ICONFILE_MUSUBSC		"A:\\RO_RES\\UI\\ICON\\MUSUBSC.SFN"
#define ID_ICONFILE_MUDLG			"A:\\RO_RES\\UI\\ICON\\MU2ND2.SFN"
#define ID_ICONFILE_MUBODY			"A:\\RO_RES\\UI\\ICON\\MUBODY.SFN"
#define ID_ICONFILE_MUSCENE		"A:\\RO_RES\\UI\\ICON\\MUSCENE.SFN"
#define ID_ICONFILE_BUTTON			"A:\\RO_RES\\UI\\ICON\\BTNMENUOK.SFN"
#define ID_ICONFILE_MODE			"A:\\RO_RES\\UI\\ICON\\MODEICON.SFN"
#define ID_ICONFILE_MENUTAB		"A:\\RO_RES\\UI\\ICON\\MENUTAB.SFN"

#if HDMI_MENU_OPTIMIZATION
#define ID_ICONFILE_MU2LYBAR_HDMI	"A:\\RO_RES\\UI\\ICON\\MU2LYBAR_HDMI.SFN"
#define ID_ICONFILE_MUSUSBAR_HDMI	"A:\\RO_RES\\UI\\ICON\\MUSUSBAR_HDMI.SFN"
#define ID_ICONFILE_MUBAR_HDMI	"A:\\RO_RES\\UI\\ICON\\MUBAR_HDMI.SFN"
#define ID_ICONFILE_MUSUBSC_HDMI	"A:\\RO_RES\\UI\\ICON\\MUSUBSC_HDMI.SFN"
#define ID_ICONFILE_MUDLG_HDMI	"A:\\RO_RES\\UI\\ICON\\MU2ND2_HDMI.SFN"
#define ID_ICONFILE_MUBODY_HDMI	"A:\\RO_RES\\UI\\ICON\\MUBODY_HDMI.SFN"
#define ID_ICONFILE_BUTTON_HDMI	"A:\\RO_RES\\UI\\ICON\\BTNMENUOK_HDMI.SFN"
#define ID_ICONFILE_MODE_HDMI		"A:\\RO_RES\\UI\\ICON\\MODEICON_HDMI.SFN"
#define ID_ICONFILE_MENUTAB_HDMI	"A:\\RO_RES\\UI\\ICON\\MENUTAB_HDMI.SFN"
#else
#define ID_ICONFILE_MU2LYBAR_HDMI	ID_ICONFILE_MU2LYBAR
#define ID_ICONFILE_MUSUSBAR_HDMI	ID_ICONFILE_MUSUSBAR
#define ID_ICONFILE_MUBAR_HDMI		ID_ICONFILE_MUBAR
#define ID_ICONFILE_MUSUBSC_HDMI	ID_ICONFILE_MUSUBSC
#define ID_ICONFILE_MUDLG_HDMI		ID_ICONFILE_MUDLG
#define ID_ICONFILE_MUBODY_HDMI		ID_ICONFILE_MUBODY
#define ID_ICONFILE_BUTTON_HDMI		ID_ICONFILE_BUTTON
#define ID_ICONFILE_MODE_HDMI		ID_ICONFILE_MODE
#define ID_ICONFILE_MENUTAB_HDMI	ID_ICONFILE_MENUTAB

#endif
#define MENU_PROMPT_OK_OFF	0x00
#define MENU_PROMPT_OK_ON	0x01

#define MENU_PROMPT_MENU_OFF	0x00
#define MENU_PROMPT_MENU_ON	0x02

//#define MENU_MODE_X_ODDSET
static UINT16 MENU_MODE_Y_OFFSET;// 4

UINT16 MENU_CHAR_WIDTH;//  16
UINT16 MENU_CHAR_HEIGHT;// 30//31

static UINT16 MENU_LAYER1_STR_X_OFFSET;// 32
static UINT16 MENU_LAYER1_STR_Y_OFFSET;// 		(43+MENU_MODE_Y_OFFSET)//44
static SINT16 MENU_LAYER1_ICON_X_OFFSET;// (-16)
static UINT16 MENU_LAYER1_ICON_Y_OFFSET;// (43+MENU_MODE_Y_OFFSET)//44
static UINT16 MENU_LAYER2_STR_X_OFFSET;// 56
static UINT16 MENU_LAYER2_STR_Y_OFFSET;// 60
static UINT16 MENU_LAYER2_ICON_X_OFFSET;// 4
static UINT16 MENU_LAYER2_ICON_Y_OFFSET;// 60

static UINT16 MENU_BAR_LAYER1_X_OFFSET;// 24
static UINT16 MENU_BAR_LAYER1_Y_OFFSET;// (43+MENU_MODE_Y_OFFSET-3)//44
static UINT16 MENU_BAR_LAYER2_X_OFFSET;// 48
static UINT16 MENU_BAR_LAYER2_Y_OFFSET;// 61-3
static UINT16 MENU_BAR_LAYER3_X_OFFSET;// 32
static UINT16 MENU_BAR_LAYER3_Y_OFFSET;// 44

static UINT16 MENU_BODY_LAYER1_X_OFFSET;// 16
static UINT16 MENU_BODY_LAYER1_Y_OFFSET;// (33+MENU_MODE_Y_OFFSET)//34
static UINT16 MENU_BODY_LAYER2_X_OFFSET;// 40
static UINT16 MENU_BODY_LAYER2_Y_OFFSET;// 47//49
static UINT16 MENU_BODY_LAYER3_X_OFFSET;// 14
static UINT16 MENU_BODY_LAYER3_Y_OFFSET;// 96-3

static UINT16 MENU_OFFSET; 
static UINT16 MENU_MU2BARSL_WIDTH;
static UINT16 MENU_HEADER_OFFSET;
static UINT16 MENU_BACK_ON_OFFSET;

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
UINT32 menuPosBackup;
UINT16 LCD_WIDTH;// ((UINT16)320)
UINT16 LCD_HEIGHT;// ((UINT16)240)
#if HDMI_MENU_BY_JPG
static UINT32 dlgUpdateFlag = 0;
extern UINT32 pipHDMIMenuFlag;
#endif
#if CAM_TYPE_CVR  /*add for system state*/
static UINT32 strId_Reset[]={ID_STR_YES,ID_STR_NO};
UINT32 RTCTimer=TIMER_NULL;
UINT32 GSENSORTimercount;
#endif
extern UINT8 fileTypeSelect;
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
//"YYYY/MM/DD",
//"MM/DD/YYYY",
//"DD/MM/YYYY",
UINT8 rtc_disp[3][6]={{0, 1, 2, 3, 4, 5}, {1, 2, 0, 3, 4, 5}, {2, 1, 0, 3, 4, 5}};
//UINT8 code exposure[]="\x7e\x7f\x7f\x7e\x7f\x7f\x7e\x7f\x7f\x7e\x7f\x7f\x7e";

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern void _setupMenuFunctionResetAll(void);
#if GPS_SUPPORT
#if CAM_TYPE_CVR  /*add for system state*/
extern void appGpsGsvInfoUpdate(void);
#endif
#endif

/**
 * @fn        void appOsdLayoutAttrGet(UINT8 *hNum, UINT8 *vNum)
 * @brief     appOsdLayoutAttrGet
 * @param     [in] hNum
 * @param     [in] vNum
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-9-21
 * @todo      N/A
 * @bug       N/A
*/
void
appOsdLayoutAttrGet(
	UINT8 *hNum,
	UINT8 *vNum
)
{
	if (hNum) *hNum = 20;
	if (vNum) *vNum = 7;
}

/**
 * @fn        void appOsdFontSizeGet(UINT16 *w, UINT16 *h)
 * @brief     appOsdFontSizeGet
 * @param     [in] w
 * @param     [in] h
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-9-21
 * @todo      N/A
 * @bug       N/A
*/
void
appOsdFontSizeGet(
	UINT16 *w,
	UINT16 *h
)
{
	/* TODO */
#if 0
	sp5kGfxFontSizeGet(1, w, h);
	printf("w:%d,h:%d\n", w, h);
#else
	if (w) *w = 36;
	if (h) *h = 32;
#endif
}

void menuDispParaInit(void)
{
	UINT16 charw,charh;
	
	appOsdFontSizeGet(&charw,&charh);

	if(IS_TV_IN)
	{
		switch(charh)
		{
		case 32:
			LCD_WIDTH = 320;
			LCD_HEIGHT = 240;
			//printf("font 32\n");
			MENU_MODE_Y_OFFSET = 4;

			MENU_CHAR_WIDTH = 16;
			MENU_CHAR_HEIGHT = 34;

			MENU_LAYER1_STR_X_OFFSET = 32;
			MENU_LAYER1_STR_Y_OFFSET = (37+MENU_MODE_Y_OFFSET);
			#if FUNC_HOST_TOUCH_PANEL
			MENU_LAYER1_ICON_X_OFFSET =0 -MENU_CHAR_WIDTH*2;
			#else
			MENU_LAYER1_ICON_X_OFFSET = 0;
			#endif
			MENU_LAYER1_ICON_Y_OFFSET = (43+MENU_MODE_Y_OFFSET);
			MENU_LAYER2_STR_X_OFFSET = 56;
			MENU_LAYER2_STR_Y_OFFSET = 59;
			MENU_LAYER2_ICON_X_OFFSET = 4;
			MENU_LAYER2_ICON_Y_OFFSET = 63;

			MENU_BAR_LAYER1_X_OFFSET = 24;
			MENU_BAR_LAYER1_Y_OFFSET = (40+MENU_MODE_Y_OFFSET-3);//44
			MENU_BAR_LAYER2_X_OFFSET = 45;
			MENU_BAR_LAYER2_Y_OFFSET = 60;
			MENU_BAR_LAYER3_X_OFFSET = 32;
			MENU_BAR_LAYER3_Y_OFFSET = 44;

			MENU_BODY_LAYER1_X_OFFSET = 16;
			MENU_BODY_LAYER1_Y_OFFSET = (33+MENU_MODE_Y_OFFSET);//34
			MENU_BODY_LAYER2_X_OFFSET = 40;
			MENU_BODY_LAYER2_Y_OFFSET = 57;//49
			MENU_BODY_LAYER3_X_OFFSET = 14;
			MENU_BODY_LAYER3_Y_OFFSET = 96-3;

			MENU_OFFSET = 12;
			MENU_MU2BARSL_WIDTH = 66;
			MENU_HEADER_OFFSET = 6;
			MENU_BACK_ON_OFFSET = 3;
			break;
		case 24:
			LCD_WIDTH = 220;
			LCD_HEIGHT = 176;
			//printf("font 24\n");
			MENU_MODE_Y_OFFSET = 2;

			MENU_CHAR_WIDTH = 12;
			MENU_CHAR_HEIGHT = 24;//31

			MENU_LAYER1_STR_X_OFFSET = 24;
			MENU_LAYER1_STR_Y_OFFSET = (28+MENU_MODE_Y_OFFSET);
			#if FUNC_HOST_TOUCH_PANEL
			MENU_LAYER1_ICON_X_OFFSET = (-8)-MENU_CHAR_WIDTH*2;
			#else
			MENU_LAYER1_ICON_X_OFFSET = (-8);
			#endif
			MENU_LAYER1_ICON_Y_OFFSET = (30+MENU_MODE_Y_OFFSET);
			MENU_LAYER2_STR_X_OFFSET = 41;
			MENU_LAYER2_STR_Y_OFFSET = 46;
			MENU_LAYER2_ICON_X_OFFSET = 0;
			MENU_LAYER2_ICON_Y_OFFSET = 47;

			MENU_BAR_LAYER1_X_OFFSET = 16;
			MENU_BAR_LAYER1_Y_OFFSET = (32+MENU_MODE_Y_OFFSET-3);
			MENU_BAR_LAYER2_X_OFFSET = 33;
			MENU_BAR_LAYER2_Y_OFFSET = 46;
			MENU_BAR_LAYER3_X_OFFSET = 21;
			MENU_BAR_LAYER3_Y_OFFSET = 35;

			MENU_BODY_LAYER1_X_OFFSET = 8;
			MENU_BODY_LAYER1_Y_OFFSET = (23+MENU_MODE_Y_OFFSET);
			MENU_BODY_LAYER2_X_OFFSET = 25;
			MENU_BODY_LAYER2_Y_OFFSET = 37;
			MENU_BODY_LAYER3_X_OFFSET = 10;
			MENU_BODY_LAYER3_Y_OFFSET = 73;

			MENU_OFFSET = 0;
			MENU_MU2BARSL_WIDTH = 43;
			MENU_HEADER_OFFSET = 6;
			MENU_BACK_ON_OFFSET = 2;
			break;
		case 16:
			LCD_WIDTH = 160;
			LCD_HEIGHT = 128;
			//printf("font 16\n");
			MENU_MODE_Y_OFFSET = 4;

			MENU_CHAR_WIDTH = 8;
			MENU_CHAR_HEIGHT = 16;

			MENU_LAYER1_STR_X_OFFSET = 22;
			MENU_LAYER1_STR_Y_OFFSET = (21+MENU_MODE_Y_OFFSET);
			#if FUNC_HOST_TOUCH_PANEL
			MENU_LAYER1_ICON_X_OFFSET = 4-MENU_CHAR_WIDTH*2;
			#else
			MENU_LAYER1_ICON_X_OFFSET = 4;
			#endif
			MENU_LAYER1_ICON_Y_OFFSET = (21+MENU_MODE_Y_OFFSET);
			MENU_LAYER2_STR_X_OFFSET = 37;
			MENU_LAYER2_STR_Y_OFFSET = 32;
			MENU_LAYER2_ICON_X_OFFSET = 4;
			MENU_LAYER2_ICON_Y_OFFSET = 32;

			MENU_BAR_LAYER1_X_OFFSET = 16;
			MENU_BAR_LAYER1_Y_OFFSET = (24+MENU_MODE_Y_OFFSET-3);
			MENU_BAR_LAYER2_X_OFFSET = 28;
			MENU_BAR_LAYER2_Y_OFFSET = 32;
			MENU_BAR_LAYER3_X_OFFSET = 20;
			MENU_BAR_LAYER3_Y_OFFSET = 27;

			MENU_BODY_LAYER1_X_OFFSET = 10;
			MENU_BODY_LAYER1_Y_OFFSET = (16+MENU_MODE_Y_OFFSET);
			MENU_BODY_LAYER2_X_OFFSET = 20;
			MENU_BODY_LAYER2_Y_OFFSET = 24;
			MENU_BODY_LAYER3_X_OFFSET = 12;
			MENU_BODY_LAYER3_Y_OFFSET = 33;

			MENU_OFFSET = 10;
			MENU_MU2BARSL_WIDTH = 38;
			MENU_HEADER_OFFSET = 3; 
			MENU_BACK_ON_OFFSET = 2;
			break;
		}
	}else{
		LCD_WIDTH = 320;
		LCD_HEIGHT = 240;
		//printf("TV font 32\n");
		MENU_MODE_Y_OFFSET = 4;

		MENU_CHAR_WIDTH = 16;
		MENU_CHAR_HEIGHT = 34;

		MENU_LAYER1_STR_X_OFFSET = 32;
		MENU_LAYER1_STR_Y_OFFSET = 41;
		#if FUNC_HOST_TOUCH_PANEL
		MENU_LAYER1_ICON_X_OFFSET = 0-MENU_CHAR_WIDTH*2;
		#else
		MENU_LAYER1_ICON_X_OFFSET = 0;
		#endif
		MENU_LAYER1_ICON_Y_OFFSET = 45;
		MENU_LAYER2_STR_X_OFFSET = 56;
		MENU_LAYER2_STR_Y_OFFSET = 59;
		MENU_LAYER2_ICON_X_OFFSET = 4;
		MENU_LAYER2_ICON_Y_OFFSET = 63;

		MENU_BAR_LAYER1_X_OFFSET = 24;
		MENU_BAR_LAYER1_Y_OFFSET = 41;
		MENU_BAR_LAYER2_X_OFFSET = 35;
		MENU_BAR_LAYER2_Y_OFFSET = 60;
		MENU_BAR_LAYER3_X_OFFSET = 16;
		MENU_BAR_LAYER3_Y_OFFSET = 33;

		MENU_BODY_LAYER1_X_OFFSET = 16;
		MENU_BODY_LAYER1_Y_OFFSET = (33+MENU_MODE_Y_OFFSET);
		MENU_BODY_LAYER2_X_OFFSET = 33;
		MENU_BODY_LAYER2_Y_OFFSET = 57;
		MENU_BODY_LAYER3_X_OFFSET = 14;
		MENU_BODY_LAYER3_Y_OFFSET = 96-3;

		MENU_OFFSET = 6;
		MENU_MU2BARSL_WIDTH = 66;
		MENU_HEADER_OFFSET = 6;
		MENU_BACK_ON_OFFSET = 3;
	}
}

/**************************************************************************
 *                                                                        *
 *  Function Name: menuLanguageGet                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8 menuLanguageGet(void)
{
	uiPara_t* puiPara = appUiParaGet();
#if 0
	if (puiPara->Language >= 24/*LANGUAGE_MAX*/)
		puiPara->Language = ENGLISH;
#endif
	return puiPara->Language;
}

/**************************************************************************
 *                                                                        *
 *  Function Name: menuDateItemConv                                       *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8 menuDateItemConv(UINT8 item)
{
	uiPara_t* puiPara = appUiParaGet();

	return rtc_disp[puiPara->DateStyle][item];
}



/**************************************************************************
 *                                                                        *
 *  Function Name: menuLayerSizeGet                                       *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: None                                                       *0
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void menuLayerSizeGet(UINT8 layer, UINT8* line, UINT8* col)
{
	#if 1

	#ifdef USE_DVS5M9_HW
	if((menuProcCurrPosGet() & 0xFFFF0000) == MENU_MARK_SYSTEM2_WIFI_PW)
	{
		*line = 6;
		*col = 1;
	}
	else
	#endif
	{
		*line = 3;
		*col = 1;
	}

	#else
	UINT8 OsdMaxX, OsdMaxY;
	UINT16 cw,ch;
	
	appOsdLayoutAttrGet(&OsdMaxX, &OsdMaxY);
	appOsdFontSizeGet(&cw, &ch);
	if(ch <= 16)OsdMaxY-=1;
		
	if (layer != 2)
	{
		*col = OsdMaxX - 2;
		*line = OsdMaxY - 2;
	}
	else
	{
		*col = OsdMaxX-5;
		*line = OsdMaxY-3;
	}

	if(menuProcCurrRootGet() == MENU_MARK_SCENEMODE)
	{
		*line = 1;
	}
	#endif
}

void appOsdColorSet(UINT8 attr)
{
#if 0
	attr = attr;
#else
	COLOR_DEF enColor;
	COLOR_DEF enShadowColor;
	COLOR_DEF enBkColor;
	BLEND_TYPE enBkBlend;
	
	switch (attr)
	{
		case MENU_STR_GET_FOCUS:
			enBkBlend = PAL_BLEND_100;
			enBkColor = PAL_NOCOLOR;
			enShadowColor = PAL_NOCOLOR;
			enColor = PAL_WHITE;
			break;
		case MENU_STR_LOST_FOCUS:
			enBkBlend = PAL_BLEND_100;
			enBkColor = PAL_NOCOLOR;
			enShadowColor = PAL_BLACK;
			enColor = PAL_WHITE;
			break;
		case MENU_STR_DISABLE:
			enBkBlend = PAL_BLEND_100;
			enBkColor = PAL_NOCOLOR;
			enShadowColor = PAL_BLACK;
			enColor = PAL_GRAY_2;
			break;
		default:
			enBkBlend = PAL_BLEND_100;
			enBkColor = PAL_NOCOLOR;
			enShadowColor = PAL_BLACK;
			enColor = PAL_WHITE;
			break;
	}

	appOsd_ColorDrawTextColorSet(enColor, enShadowColor, enBkColor, enBkBlend);
#endif	
}
/**************************************************************************
 *                                                                        *
 *  Function Name: menuStrDisp                                            *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8 menuStrDisp(UINT8 x, UINT8 y, UINT8* str, UINT8 blendOpt, UINT8 layer)
{
	UINT16 offset = x*MENU_CHAR_WIDTH;

	blendOpt = blendOpt;

	if(layer == 2)
	{
	       #if KIT_WITH_HDMI   /*Add by Lation@20130717*/
		if(IS_HDMI_IN){
			appOsdLib_TextPIPDraw((offset+MENU_LAYER2_STR_X_OFFSET)<<1, ((UINT16)y*MENU_CHAR_HEIGHT+MENU_LAYER2_STR_Y_OFFSET)*3>>1, SP5K_GFX_ALIGN_TOP_LEFT, str);
		}
		else{
			appOsdLib_TextPIPDraw(offset+MENU_LAYER2_STR_X_OFFSET, (UINT16)y*MENU_CHAR_HEIGHT+MENU_LAYER2_STR_Y_OFFSET, SP5K_GFX_ALIGN_TOP_LEFT, str);
		}
		#endif
	}
	else
	{
	     #if KIT_WITH_HDMI   /*Add by Lation@20130717*/
		if(IS_HDMI_IN){
			appOsdLib_TextPIPDraw((offset+MENU_LAYER1_STR_X_OFFSET)<<1, ((UINT16)y*MENU_CHAR_HEIGHT+MENU_LAYER1_STR_Y_OFFSET)*3>>1, SP5K_GFX_ALIGN_TOP_LEFT, str);
		}
		else{
			appOsdLib_TextPIPDraw(offset+MENU_LAYER1_STR_X_OFFSET, (UINT16)y*MENU_CHAR_HEIGHT+MENU_LAYER1_STR_Y_OFFSET, SP5K_GFX_ALIGN_TOP_LEFT, str);
		}
		#endif
	}

	return strlen(str);
}

UINT32 menuStrDraw(UINT32 x, UINT32 y, UINT32 strIdx, UINT32 blendOpt, UINT32 layer)
{
	UINT32 offset = x*MENU_CHAR_WIDTH;

	blendOpt = blendOpt;

	if(layer == 2)
	{
	      #if KIT_WITH_HDMI   /*Add by Lation@20130717*/
		if(IS_HDMI_IN){
			appOsdLib_TextIDPIPDraw((offset+MENU_LAYER2_STR_X_OFFSET)<<1, ((UINT16)y*(MENU_CHAR_HEIGHT-2)+MENU_LAYER2_STR_Y_OFFSET+6)*3>>1, SP5K_GFX_ALIGN_TOP_LEFT, strIdx);
		}
		else if (menuProcCurrRootGet() == MENU_MARK_CALIBRATION)  
		{
			appOsdLib_TextIDPIPDraw(offset-20, (UINT16)y*MENU_CHAR_HEIGHT+MENU_LAYER2_STR_Y_OFFSET, SP5K_GFX_ALIGN_TOP_LEFT, strIdx);
		}
		else
		{
			appOsdLib_TextIDPIPDraw(offset+MENU_LAYER2_STR_X_OFFSET, (UINT16)y*MENU_CHAR_HEIGHT+MENU_LAYER2_STR_Y_OFFSET, SP5K_GFX_ALIGN_TOP_LEFT, strIdx);
		}
		#endif
	}
	else
	{
	      #if KIT_WITH_HDMI   /*Add by Lation@20130717*/
		if(IS_HDMI_IN){
			appOsdLib_TextIDPIPDraw((offset+MENU_LAYER1_STR_X_OFFSET)<<1, ((UINT16)y*(MENU_CHAR_HEIGHT-1)+MENU_LAYER1_STR_Y_OFFSET+7)*3>>1, SP5K_GFX_ALIGN_TOP_LEFT, strIdx);
		}
		else{
			if(fileTypeSelect == 1)
			{
				appOsdLib_TextIDPIPDraw(offset+MENU_LAYER1_STR_X_OFFSET, (UINT16)y*MENU_CHAR_HEIGHT+MENU_LAYER1_STR_Y_OFFSET-20, SP5K_GFX_ALIGN_TOP_LEFT, strIdx);
			}
			else
			{
				appOsdLib_TextIDPIPDraw(offset+MENU_LAYER1_STR_X_OFFSET, (UINT16)y*MENU_CHAR_HEIGHT+MENU_LAYER1_STR_Y_OFFSET, SP5K_GFX_ALIGN_TOP_LEFT, strIdx);
			}
		}
		#endif
	}

	return SUCCESS;
}

void menuStrDispExt(UINT16 x, UINT16 y, UINT8* str, UINT8 blendOpt)
{
	appOsdLib_TextDraw(x, y, SP5K_GFX_ALIGN_TOP_LEFT, str);
}

void menuBarDisp(UINT8 line, UINT8 layer)
{
	UINT8 attr = MENU_BAR_GET_FOCUS;
	UINT8 *pIconFile = NULL;
	SINT32 x = 0, y = 0;
	UINT8 nodeItem = 0;
	#if HDMI_MENU_BY_JPG
	UINT32 p[4];
	#endif
	
	nodeItem = menuProcNodeItemGet(layer, 0);
	if ((layer == 1) && (menuProcLayerGet() == 2)) {
		attr = MENU_BAR_DISABLE;
	}

	appOsdColorSet(attr);

	#if KIT_WITH_HDMI && HDMI_MENU_BY_JPG
	if(IS_HDMI_IN){
		#if 0
		sp5kGfxAttrSet(SP5K_GFX_PEN_COLOR, 0x000000, 0, 0, 0);
		sp5kGfxAttrSet(SP5K_GFX_FILL_COLOR, 0x000000, 0, 0, 0);

		sp5kGfxAttrSet(SP5K_GFX_PIP_REFRESH_ACTIVE, 0, 0, 0, 0);
		appOsd_GfxRectShapeDraw(SP5K_GFX_PAGE_PIP_BASE+0, 0, 220,1920,955);
		sp5kGfxAttrSet(SP5K_GFX_PIP_REFRESH_ACTIVE, 1, 0, 1, 0);
		#endif
	}
	#endif
	
	if(layer == 2){
		#if 0
		if( menuProcSubItemGet(layer,menuProcScrollGet(layer))){
			pIconFile = ID_ICONFILE_MUSUSBAR;
		} else {
			pIconFile = ID_ICONFILE_MUSBAR;
		}
		#endif

		#if KIT_WITH_HDMI   /*Add by Lation@20130717*/
		if(IS_HDMI_IN){
			#if HDMI_MENU_BY_JPG
			p[0] = menuSrcFileInfo[MENU_FOUR_LINES_BAR_HDMI].pos[0];
			p[1] = menuSrcFileInfo[MENU_FOUR_LINES_BAR_HDMI].pos[1]+line*145;
			p[2] = menuSrcFileInfo[MENU_FOUR_LINES_BAR_HDMI].pos[2];
			p[3] = menuSrcFileInfo[MENU_FOUR_LINES_BAR_HDMI].pos[3];

			sp5kRsvPreloadDraw(PRELOAD_FOUR_LINES_BAR, HDMI_MENU_LAYER1, p, 0);
			#else
			x = MENU_BAR_LAYER2_X_OFFSET*2;
			y = (UINT16)line*(MENU_CHAR_HEIGHT+15)+MENU_BAR_LAYER2_Y_OFFSET+39;
			pIconFile = ID_ICONFILE_MUSUSBAR_HDMI;
			#endif
		}
		else{
			x = MENU_BAR_LAYER2_X_OFFSET;
			y = (UINT16)line*MENU_CHAR_HEIGHT+MENU_BAR_LAYER2_Y_OFFSET;
			pIconFile = ID_ICONFILE_MUSUSBAR;
		}
		#endif
		
	}
	else if(layer == 3)
	{
		if(nodeItem>2)
		{
		      #if KIT_WITH_HDMI   /*Add by Lation@20130717*/
			if(IS_HDMI_IN)
			{
				#if HDMI_MENU_BY_JPG
				p[0] = menuSrcFileInfo[MENU_FOUR_LINES_BAR_HDMI].pos[0];
				p[1] = menuSrcFileInfo[MENU_FOUR_LINES_BAR_HDMI].pos[1]+line*145;
				p[2] = menuSrcFileInfo[MENU_FOUR_LINES_BAR_HDMI].pos[2];
				p[3] = menuSrcFileInfo[MENU_FOUR_LINES_BAR_HDMI].pos[3];

				sp5kRsvPreloadDraw(PRELOAD_FOUR_LINES_BAR, HDMI_MENU_LAYER1, p, 0);
				#else
				x = MENU_BAR_LAYER2_X_OFFSET*2;
				y = (UINT16)line*(MENU_CHAR_HEIGHT+14)+MENU_BAR_LAYER2_Y_OFFSET+13;
				pIconFile = ID_ICONFILE_MUSUSBAR_HDMI;
				#endif
			}
			else
			{
				if(fileTypeSelect == 1)
				{
					x = MENU_BAR_LAYER2_X_OFFSET-5;
					y = (UINT16)line*MENU_CHAR_HEIGHT+MENU_BAR_LAYER2_Y_OFFSET-40;
				}
				else
				{
					x = MENU_BAR_LAYER2_X_OFFSET;
					y = (UINT16)line*MENU_CHAR_HEIGHT+MENU_BAR_LAYER2_Y_OFFSET-17;
				}
				pIconFile = ID_ICONFILE_MUSUSBAR;
			}
			#endif

		}
		else
		{
			pIconFile = ID_ICONFILE_MUDLGBAR;
			x = MENU_BAR_LAYER3_X_OFFSET;
			y = (UINT16)line*MENU_CHAR_HEIGHT+5+MENU_BAR_LAYER3_Y_OFFSET;
		}
	}
	else{
	#if KIT_WITH_HDMI   /*Add by Lation@20130717*/
		if(IS_HDMI_IN)
		{
			#if HDMI_MENU_BY_JPG

			p[0] = menuSrcFileInfo[MENU_FIVE_LINES_BAR_HDMI].pos[0];
			p[1] = menuSrcFileInfo[MENU_FIVE_LINES_BAR_HDMI].pos[1]+line*147;
			p[2] = menuSrcFileInfo[MENU_FIVE_LINES_BAR_HDMI].pos[2];
			p[3] = menuSrcFileInfo[MENU_FIVE_LINES_BAR_HDMI].pos[3];
			
			sp5kRsvPreloadDraw(PRELOAD_FIVE_LINES_BAR, HDMI_MENU_LAYER3, p, 0);
			#else
			x = 0;
			y = (UINT16)line*(MENU_CHAR_HEIGHT+15)+MENU_BAR_LAYER1_Y_OFFSET+32;
			pIconFile = ID_ICONFILE_MUBAR_HDMI;
			#endif
		}
		else
		{
			x = 0;
			y = (UINT16)line*MENU_CHAR_HEIGHT+MENU_BAR_LAYER1_Y_OFFSET+1;
			pIconFile = ID_ICONFILE_MUBAR;
		}
		#endif
	}

	if(menuProcCurrRootGet() == MENU_MARK_SCENEMODE)
	{
		pIconFile = NULL;
	}

	#if HDMI_MENU_BY_JPG
	#if KIT_WITH_HDMI   /*Add by Lation@20130717*/
	if(!IS_HDMI_IN){
		appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0, x, y, SP5K_GFX_ALIGN_TOP_LEFT, pIconFile, 0x0000);
	}
	#endif
	#else
	appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0, x, y, SP5K_GFX_ALIGN_TOP_LEFT, pIconFile, 0x0000);
	#endif
}

/**************************************************************************
 *                                                                        *
 *  Function Name: menuIconDisp                                           *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void menuIconDisp(UINT8 x, UINT8 y, UINT16 icon, UINT8 blendOpt, UINT8 layer)
{
	SINT32 posX, posY;
	
	if (icon == ID_ICON_NONE) {
		printf("icon none\n");
		return;
	}

	blendOpt = blendOpt;
	
	if(layer == 2) 
	{
	        #if KIT_WITH_HDMI   /*Add by Lation@20130717*/
		if(IS_HDMI_IN){
			posX = ((UINT16)x*MENU_CHAR_WIDTH+MENU_LAYER2_ICON_X_OFFSET)<<1;
			posY = ((UINT16)y*(MENU_CHAR_HEIGHT-1)+MENU_LAYER2_ICON_Y_OFFSET+4)*3>>1;
		}
		else{
			posX = (UINT16)x*MENU_CHAR_WIDTH+MENU_LAYER2_ICON_X_OFFSET;
			posY = (UINT16)y*MENU_CHAR_HEIGHT+MENU_LAYER2_ICON_Y_OFFSET;
		}
		#endif
	}
	else 
	{
	    #if KIT_WITH_HDMI   /*Add by Lation@20130717*/
		if(IS_HDMI_IN){
			posX = ((UINT16)(x-1)*MENU_CHAR_WIDTH+MENU_LAYER1_ICON_X_OFFSET)<<1;
			posY = ((UINT16)y*(MENU_CHAR_HEIGHT-2)+MENU_LAYER1_ICON_Y_OFFSET+5)*3>>1;
		}
		else{
			posX = (UINT16)(x-1)*MENU_CHAR_WIDTH+MENU_LAYER1_ICON_X_OFFSET;
			posY = (UINT16)y*MENU_CHAR_HEIGHT+MENU_LAYER1_ICON_Y_OFFSET;
		}
		#endif
	}
	printf("ID:0x%x\n",icon);

	appOsdLib_PIPLayIconDraw(posX, posY, SP5K_GFX_ALIGN_TOP_LEFT, icon);
}

void menuIconDispExt(UINT16 x, UINT16 y, UINT16 icon, UINT8 blendOpt)
{
	blendOpt = blendOpt;
	appOsdLib_PIPLayIconDraw(x, y, SP5K_GFX_ALIGN_TOP_LEFT, icon);
}

/**************************************************************************
 *                                                                        *
 *  Function Name: menuHeaderDisp                                         *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void menuHeaderDisp(UINT8 layer)
{
	UINT32 iconId;
	/*when you delete a mode in menu,please delete the icon id of this mode*/
	UINT32 icon_ID[6] = {ICON_ID_STILL,ICON_ID_VIDEO,/*ICON_ID_AUDIO,*/ICON_ID_PLAYBACK,ICON_ID_SETUP,ICON_ID_DPS};
	if((layer == 0xff) || (menuProcCurrRootGet() == MENU_MARK_SCENEMODE)\
		||(menuProcCurrRootGet() == MENU_MARK_CALIBRATION))
	{
		return;
	}

	iconId =icon_ID[(UINT32)((menuPosBackup>>24) & 0xff) - 1];
	
	if (menuProcCurrRootGet() == MENU_MARK_SETUP)
	{	
	    #if KIT_WITH_HDMI   /*Add by Lation@20130717*/
		if(IS_HDMI_IN)
		{
			#if HDMI_MENU_BY_JPG
				#if 1
				sp5kGfxAttrSet(SP5K_GFX_PEN_COLOR, 0x000000, 0, 0, 0);
				sp5kGfxAttrSet(SP5K_GFX_FILL_COLOR, 0x000000, 0, 0, 0);

				appOsd_GfxRectShapeDraw(HDMI_MENU_LAYER3, menuSrcFileInfo[MENU_TITLE_POS1_HDMI].pos[0], menuSrcFileInfo[MENU_TITLE_POS1_HDMI].pos[1],
					menuSrcFileInfo[MENU_TITLE_POS1_HDMI].pos[0]+menuSrcFileInfo[MENU_TITLE_POS1_HDMI].pos[2],menuSrcFileInfo[MENU_TITLE_POS1_HDMI].pos[1]+menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].pos[3]);			
				#endif

				sp5kRsvPreloadDraw(iconId,HDMI_MENU_LAYER2,menuSrcFileInfo[iconId].pos,0);

				sp5kRsvPreloadDraw(PRELOAD_MODE_SETUP,HDMI_MENU_LAYER2,menuSrcFileInfo[MENU_SETUP_HDMI].pos,0);
	
				sp5kRsvPreloadDraw(PRELOAD_MENU_TITLE,HDMI_MENU_LAYER3,menuSrcFileInfo[MENU_TITLE_POS2_HDMI].pos,0);	

			#else
				appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0, 40, 9, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICONFILE_MODE_HDMI, iconId);
				appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0, 97, 0, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICONFILE_MENUTAB_HDMI, 0x0000);
				appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0, 137, 9, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICONFILE_MODE_HDMI, 0x0004);
			#endif
		}
		else
		{
			appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0, 20, 6, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICONFILE_MODE, iconId);
			appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0, 60, 0, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICONFILE_MENUTAB, 0x0000);
			appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0, 80, 6, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICONFILE_MODE, 0x0004);
		}
		#endif
	}
	else
	{
	    #if KIT_WITH_HDMI   /*Add by Lation@20130717*/
		if(IS_HDMI_IN)
		{
			#if HDMI_MENU_BY_JPG
				sp5kGfxAttrSet(SP5K_GFX_PEN_COLOR, 0x000000, 0, 0, 0);
				sp5kGfxAttrSet(SP5K_GFX_FILL_COLOR, 0x000000, 0, 0, 0);

				appOsd_GfxRectShapeDraw(HDMI_MENU_LAYER3, menuSrcFileInfo[MENU_TITLE_POS2_HDMI].pos[0], menuSrcFileInfo[MENU_TITLE_POS2_HDMI].pos[1],
					menuSrcFileInfo[MENU_TITLE_POS2_HDMI].pos[0]+menuSrcFileInfo[MENU_TITLE_POS2_HDMI].pos[2],menuSrcFileInfo[MENU_TITLE_POS2_HDMI].pos[1]+menuSrcFileInfo[MENU_TITLE_POS2_HDMI].pos[3]);

				sp5kRsvPreloadDraw(iconId,HDMI_MENU_LAYER2,menuSrcFileInfo[iconId].pos,0);

				if((menuPosBackup&0x0f000000) != MENU_MARK_DPS)
				{
					sp5kRsvPreloadDraw(PRELOAD_MODE_SETUP,HDMI_MENU_LAYER2,menuSrcFileInfo[MENU_SETUP_HDMI].pos,0);
				}

				sp5kRsvPreloadDraw(PRELOAD_MENU_TITLE,HDMI_MENU_LAYER3,menuSrcFileInfo[MENU_TITLE_POS1_HDMI].pos,0);
			#else
				appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0, 0, 0, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICONFILE_MENUTAB_HDMI, 0x0000);
				appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0, 40, 9, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICONFILE_MODE_HDMI, iconId);

				#if !CAM_TYPE_CVR
				if((menuPosBackup&0x0f000000) != MENU_MARK_DPS)
				{
					appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0, 137, 9, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICONFILE_MODE_HDMI, 0x0004);
				}
				#else
				appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0, 137, 9, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICONFILE_MODE_HDMI, 0x0004);
				#endif
			#endif
		}
		else
		{
			appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0, 0, 0, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICONFILE_MENUTAB, 0x0000);
			appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0, 20, 6, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICONFILE_MODE, iconId);

			#if !CAM_TYPE_CVR
			if((menuPosBackup&0x0f000000) != MENU_MARK_DPS)
			{
				appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0, 80, 6, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICONFILE_MODE, 0x0004);
			}
			#else
			appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0, 80, 6, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICONFILE_MODE, 0x0004);
			#endif
		}
		#endif
	}

	#if 0
	if (layer == 0)
	{
		if (menuProcCurrRootGet() == MENU_MARK_SETUP)
		{
			icon = menuProcIconIDGetByPos(menuPosBackup);
	
			appOsdColorSet(MENU_ICON_DISABLE);
			appOsdLib_OsdLayIconDraw(3*MENU_CHAR_WIDTH, MENU_MODE_Y_OFFSET+2, SP5K_GFX_ALIGN_TOP_LEFT, icon);
	
			icon = menuProcIconIDGetByPos(MENU_MARK_SETUP);
			appOsdColorSet(MENU_TAB_GET_FOCUS);
/*			appOsdLib_OsdLayIconDraw(5*MENU_CHAR_WIDTH+MENU_HEADER_OFFSET, MENU_MODE_Y_OFFSET, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_MUTAB);*/
			appOsdColorSet(MENU_STR_GET_FOCUS);

			appOsdLib_OsdLayIconDraw(6*MENU_CHAR_WIDTH, MENU_MODE_Y_OFFSET+2, SP5K_GFX_ALIGN_TOP_LEFT, icon);
		}
		else
		{
			icon = menuProcIconIDGetByPos(menuPosBackup);
	
			appOsdColorSet(MENU_TAB_GET_FOCUS);
/*			appOsdLib_OsdLayIconDraw(2*MENU_CHAR_WIDTH+MENU_HEADER_OFFSET, MENU_MODE_Y_OFFSET, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_MUTAB);*/
			appOsdColorSet(MENU_STR_GET_FOCUS);
			appOsdLib_OsdLayIconDraw(3*MENU_CHAR_WIDTH, MENU_MODE_Y_OFFSET+2, SP5K_GFX_ALIGN_TOP_LEFT,icon);
			appOsdColorSet(MENU_ICON_DISABLE);
			icon = menuProcIconIDGetByPos(MENU_MARK_SETUP);

			appOsdLib_OsdLayIconDraw(6*MENU_CHAR_WIDTH, MENU_MODE_Y_OFFSET+2, SP5K_GFX_ALIGN_TOP_LEFT, icon);
		}
	}
	else if (layer == 1)
	{
		if (menuProcCurrRootGet() == MENU_MARK_SETUP)
		{
			icon = menuProcIconIDGetByPos(menuPosBackup);	
			appOsdColorSet(MENU_ICON_DISABLE);
			appOsdLib_OsdLayIconDraw(3*MENU_CHAR_WIDTH, MENU_MODE_Y_OFFSET+2, SP5K_GFX_ALIGN_TOP_LEFT,icon);
	
			icon = menuProcIconIDGetByPos(MENU_MARK_SETUP);
			appOsdColorSet(MENU_TAB_LOST_TFOCUS);
/*			appOsdLib_OsdLayIconDraw(5*MENU_CHAR_WIDTH+MENU_HEADER_OFFSET, MENU_MODE_Y_OFFSET, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_MUTAB);*/
			appOsdColorSet(MENU_TAB_GET_FOCUS);

			appOsdLib_OsdLayIconDraw(6*MENU_CHAR_WIDTH, MENU_MODE_Y_OFFSET+2, SP5K_GFX_ALIGN_TOP_LEFT, icon);
		}
		else
		{
			icon = menuProcIconIDGetByPos(menuPosBackup);
	
			appOsdColorSet(MENU_TAB_LOST_TFOCUS);
/*			appOsdLib_OsdLayIconDraw(2*MENU_CHAR_WIDTH+MENU_HEADER_OFFSET, MENU_MODE_Y_OFFSET, SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_MUTAB);*/
			appOsdColorSet(MENU_TAB_GET_FOCUS);
			appOsdLib_OsdLayIconDraw(3*MENU_CHAR_WIDTH, MENU_MODE_Y_OFFSET+2, SP5K_GFX_ALIGN_TOP_LEFT,icon);
	
			appOsdColorSet(MENU_ICON_DISABLE);
			icon = menuProcIconIDGetByPos(MENU_MARK_SETUP);

			appOsdLib_OsdLayIconDraw(6*MENU_CHAR_WIDTH, MENU_MODE_Y_OFFSET+2, SP5K_GFX_ALIGN_TOP_LEFT, icon);
		}
	}
	else if (layer == 2)
	{
		if (menuProcCurrRootGet() == MENU_MARK_SETUP)
		{
			icon = menuProcIconIDGetByPos(menuPosBackup);
	
			appOsdColorSet(MENU_ICON_DISABLE);
			appOsdLib_OsdLayIconDraw(3*MENU_CHAR_WIDTH, MENU_MODE_Y_OFFSET+2, SP5K_GFX_ALIGN_TOP_LEFT,icon);
	
			icon = menuProcIconIDGetByPos(MENU_MARK_SETUP);
			appOsdColorSet(MENU_TAB_DISABLE);
/*			appOsdLib_OsdLayIconDraw(5*MENU_CHAR_WIDTH+MENU_HEADER_OFFSET, MENU_MODE_Y_OFFSET, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_MUTAB);*/			
			appOsdColorSet(MENU_ICON_DISABLE);

			appOsdLib_OsdLayIconDraw(6*MENU_CHAR_WIDTH, MENU_MODE_Y_OFFSET+2, SP5K_GFX_ALIGN_TOP_LEFT, icon);
		}
		else
		{
			icon = menuProcIconIDGetByPos(menuPosBackup);
	
			appOsdColorSet(MENU_TAB_DISABLE);
/*			appOsdLib_OsdLayIconDraw(2*MENU_CHAR_WIDTH+MENU_HEADER_OFFSET, MENU_MODE_Y_OFFSET, SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_MUTAB);*/
			appOsdColorSet(MENU_ICON_DISABLE);
			appOsdLib_OsdLayIconDraw(3*MENU_CHAR_WIDTH, MENU_MODE_Y_OFFSET+2, SP5K_GFX_ALIGN_TOP_LEFT,icon);
	
			appOsdColorSet(MENU_ICON_DISABLE);
			icon = menuProcIconIDGetByPos(MENU_MARK_SETUP);

			appOsdLib_OsdLayIconDraw(6*MENU_CHAR_WIDTH, MENU_MODE_Y_OFFSET+2, SP5K_GFX_ALIGN_TOP_LEFT, icon);
		}
	}
	#if FUNC_HOST_TOUCH_PANEL
	menuProcButIconDisp(0xff,2*MENU_CHAR_WIDTH+MENU_HEADER_OFFSET,MENU_MODE_Y_OFFSET,\
		3*MENU_CHAR_WIDTH,MENU_CHAR_HEIGHT,\
		MENU_OBJ_MODE_NOR,ID_ICON_NONE,MENU_FUNC_MENUMODE,0x00,SP1K_MSG_NULL,0);
	
	menuProcButIconDisp(0xff,5*MENU_CHAR_WIDTH+MENU_HEADER_OFFSET,MENU_MODE_Y_OFFSET,\
		3*MENU_CHAR_WIDTH,MENU_CHAR_HEIGHT,\
		MENU_OBJ_MODE_SETUP,ID_ICON_NONE,MENU_FUNC_MENUMODE,0x01,SP1K_MSG_NULL,0);
	#endif
	#endif
}
void menuBodyDisp(UINT8 layer)
{
	UINT8 attr = MENU_BODY_GET_FOCUS;
	#if  1/*FUNC_HOST_TOUCH_PANEL*/
	UINT8 line,col;
	UINT8 nodeItem;
	#if HDMI_MENU_BY_JPG
	UINT32 ratioX = 1920/640;
	UINT32 ratioY = 1080/360;
	#endif
	
	if(layer)
	{
		menuLayerSizeGet(layer, &line, &col);
		nodeItem = menuProcNodeItemGet(layer, 0);
	}
	else
	{
		menuLayerSizeGet(1, &line, &col);
		nodeItem = menuProcNodeItemGet(1, 0);
	}
	#endif
	if(menuProcCurrRootGet() == MENU_MARK_SCENEMODE)
	{
		appOsdLib_IconFileDraw(SP5K_GFX_PAGE_OSD_0, 14, 4, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICONFILE_MUSCENE, 0x0000);
		return;
	}
	
	if ((layer == 1) && (menuProcLayerGet() == 2))
	{
		attr = MENU_BODY_DISABLE;
	}
	else if (layer == 3)
	{
		attr = MENU_DLG_GET_FOCUS;
	}

	appOsdColorSet(attr);
	if(layer == 2)
	{
	    #if KIT_WITH_HDMI   /*Add by Lation@20130717*/
		if(IS_HDMI_IN)
		{
			#if HDMI_MENU_BY_JPG
				appOsd_ColorDrawShapeColorSet(0x00000000, PAL_BLEND_100, 0x00000000, PAL_BLEND_100);
				appOsdLib_PIPLayerShapeDraw(APP_OSD_SHAPE_RECT,menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].pos[0]/ratioX, menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].pos[1]/ratioY - 2,
					menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].pos[2]/ratioX, menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].pos[3]/ratioY, 0, 0);

				#if 0  
				sp5kGfxAttrSet(SP5K_GFX_PEN_COLOR, 0x000000, 0, 0, 0);
				sp5kGfxAttrSet(SP5K_GFX_FILL_COLOR, 0x000000, 0, 0, 0);
				appOsd_GfxRectShapeDraw(SP5K_GFX_PAGE_PIP_BASE+1, menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].pos[0], menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].pos[1],
					menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].pos[0]+menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].pos[2],menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].pos[1]+menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].pos[3]);
				appOsd_GfxRectShapeDraw(SP5K_GFX_PAGE_PIP_BASE+0, menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].pos[0], menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].pos[1],
					menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].pos[0]+menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].pos[2],menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].pos[1]+menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].pos[3]);
				#endif
	
				sp5kRsvPreloadDraw(PRELOAD_FOUR_LINES_BODY, HDMI_MENU_LAYER2, menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].pos, 0);

			#else
				appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0, (MENU_BODY_LAYER2_X_OFFSET*2-4), (MENU_BODY_LAYER2_Y_OFFSET*2-20), SP5K_GFX_ALIGN_TOP_LEFT, ID_ICONFILE_MUSUBSC_HDMI, 0x0000);
			#endif
		}
		else
		{
			appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0, MENU_BODY_LAYER2_X_OFFSET, MENU_BODY_LAYER2_Y_OFFSET, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICONFILE_MUSUBSC, 0x0000);
		}
		#endif
		
		#if FUNC_HOST_TOUCH_PANEL
		if(nodeItem<=line)
		{
			attr=MENU_BODY_DISABLE;
		}

		menuProcButIconDisp(attr,MENU_BODY_LAYER2_X_OFFSET + 246-MENU_CHAR_WIDTH,MENU_BODY_LAYER2_Y_OFFSET+39-MENU_CHAR_HEIGHT,\
			MENU_CHAR_WIDTH*2,MENU_CHAR_HEIGHT,\
			MENU_OBJ_ICON_UP,ID_ICON_TBUTTON_UP,MENU_FUNC_NULL,0x00,SP1K_MSG_TP_UP,0);
		menuProcButIconDisp(attr,MENU_BODY_LAYER2_X_OFFSET + 246-MENU_CHAR_WIDTH,MENU_BODY_LAYER2_Y_OFFSET+104,\
			MENU_CHAR_WIDTH*2,MENU_CHAR_HEIGHT,\
			MENU_OBJ_ICON_DOWN,ID_ICON_TBUTTON_DOWN,MENU_FUNC_NULL,0x00,SP1K_MSG_TP_DOWN,0);		
		#endif
	}
	else if (layer == 3)
	{
		if(nodeItem > 2)/*item more than 2*/
		{
		    #if KIT_WITH_HDMI   /*Add by Lation@20130717*/
			if(IS_HDMI_IN)
			{
				#if HDMI_MENU_BY_JPG	
				#if 0
				sp5kGfxAttrSet(SP5K_GFX_PEN_COLOR, 0x000000, 0, 0, 0);
				sp5kGfxAttrSet(SP5K_GFX_FILL_COLOR, 0x000000, 0, 0, 0);
				
				appOsd_GfxRectShapeDraw(HDMI_MENU_LAYER2, menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].pos[0], menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].pos[1],
					menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].pos[0]+menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].pos[2],menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].pos[1]+menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].pos[3]);
				//appOsd_GfxRectShapeDraw(HDMI_MENU_LAYER2, menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].pos[0], menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].pos[1],
				//	menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].pos[0]+menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].pos[2],menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].pos[1]+menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].pos[3]);
				#endif
				sp5kRsvPreloadDraw(PRELOAD_FOUR_LINES_BODY, HDMI_MENU_LAYER2, menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].pos, 0);

				#else
				appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0, (MENU_BODY_LAYER2_X_OFFSET*2-4), (MENU_BODY_LAYER2_Y_OFFSET*3-55), SP5K_GFX_ALIGN_TOP_LEFT, ID_ICONFILE_MUSUBSC_HDMI, 0x0000);
				#endif
			}
			else
			{
				if(fileTypeSelect == 1)
				{
					appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0, MENU_BODY_LAYER2_X_OFFSET-6, MENU_BODY_LAYER2_Y_OFFSET-6, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICONFILE_MUSUBSC, 0x0000);
				}
				else
				{
					appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0, MENU_BODY_LAYER2_X_OFFSET, MENU_BODY_LAYER2_Y_OFFSET+20, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICONFILE_MUSUBSC, 0x0000);
				}
			}
			#endif

		}
		else
		{
		     #if KIT_WITH_HDMI   /*Add by Lation@20130717*/
			if(IS_HDMI_IN)
			{
				#if HDMI_MENU_BY_JPG
				sp5kResourceFilePagePlay(SP5K_RES_TYPE_JPG, 
			 		menuSrcFileInfo[MENU_TWO_LINES_BODY_HDMI].fileName, HDMI_MENU_LAYER1, menuSrcFileInfo[MENU_TWO_LINES_BODY_HDMI].pos, 0);
				
				#else
				appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0, MENU_BODY_LAYER3_X_OFFSET<<1, MENU_BODY_LAYER3_Y_OFFSET<<1, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICONFILE_MUDLG_HDMI, 0x0000);
				#endif
			}
			else
			{
				appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0, MENU_BODY_LAYER3_X_OFFSET, MENU_BODY_LAYER3_Y_OFFSET, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICONFILE_MUDLG, 0x0000);
			}
			#endif
		}
	}
	else
	{
	     #if KIT_WITH_HDMI   /*Add by Lation@20130717*/
		if(IS_HDMI_IN){
			#if HDMI_MENU_BY_JPG
			//sp5kResourceFilePagePlay(SP5K_RES_TYPE_JPG | SP5K_RES_SUBTYPE_JPG_MAIN, 
			// menuSrcFileInfo[MENU_BODY_HDMI].fileName, SP5K_GFX_PAGE_PIP_MAIN, menuSrcFileInfo[MENU_BODY_HDMI].pos, 0);
			sp5kRsvPreloadDraw(PRELOAD_MENU_BODY, HDMI_MENU_LAYER4, menuSrcFileInfo[MENU_BODY_HDMI].pos, 0);
			#else
			appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0, 0, 0, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICONFILE_MUBODY_HDMI, 0x0000);
			#endif
		}
		else{
			appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0, 0, 0, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICONFILE_MUBODY, 0x0000);
		}
		#endif
		#if FUNC_HOST_TOUCH_PANEL
		if(nodeItem<=line)
		{
			attr=MENU_BODY_DISABLE;
		}
		menuProcButIconDisp(attr,MENU_BODY_LAYER1_X_OFFSET + 268-MENU_CHAR_WIDTH,MENU_BODY_LAYER1_Y_OFFSET+35-MENU_CHAR_HEIGHT,\
			MENU_CHAR_WIDTH*2,MENU_CHAR_HEIGHT,\
			MENU_OBJ_ICON_UP,ID_ICON_TBUTTON_UP,MENU_FUNC_NULL,0x00,SP1K_MSG_TP_UP,0);
		menuProcButIconDisp(attr,MENU_BODY_LAYER1_X_OFFSET + 268-MENU_CHAR_WIDTH,MENU_BODY_LAYER1_Y_OFFSET+132,\
			MENU_CHAR_WIDTH*2,MENU_CHAR_HEIGHT,\
			MENU_OBJ_ICON_DOWN,ID_ICON_TBUTTON_DOWN,MENU_FUNC_NULL,0x00,SP1K_MSG_TP_DOWN,0);
		#endif
	}
}

#if HDMI_MENU_BY_JPG
void menuHDMIRsvPreload(void)
{
	UINT32 ret;
#if 0	
	UINT32 width,height;
	
	sp5kRsvPreloadDrawCfgSet(SP5K_RES_PRELOAD_NUM,7);

	width  = ROUND_UP_TO_DIVISIBLE(menuSrcFileInfo[MENU_BODY_HDMI].pos[2],16);
	height = ROUND_UP_TO_DIVISIBLE(menuSrcFileInfo[MENU_BODY_HDMI].pos[3],16);
	printf("%d  %d   width:%d height:%d\n",menuSrcFileInfo[MENU_BODY_HDMI].pos[2],menuSrcFileInfo[MENU_BODY_HDMI].pos[3],
		width,height);
	ret = sp5kRsvPreloadFile(SP5K_RES_TYPE_JPG, menuSrcFileInfo[MENU_BODY_HDMI].fileName, PRELOAD_MENU_BODY, width, height, 0);
	printf("MENU_BODY_HDMI preload -- %s\n",(ret==FAIL)?"FAIL":"SUCCESS");

	width  = ROUND_UP_TO_DIVISIBLE(menuSrcFileInfo[MENU_FIVE_LINES_BAR_HDMI].pos[2],16);
	height = ROUND_UP_TO_DIVISIBLE(menuSrcFileInfo[MENU_FIVE_LINES_BAR_HDMI].pos[3],16);
	printf("%d  %d   width:%d height:%d\n",menuSrcFileInfo[MENU_FIVE_LINES_BAR_HDMI].pos[2],menuSrcFileInfo[MENU_FIVE_LINES_BAR_HDMI].pos[3],
		width,height);
	ret = sp5kRsvPreloadFile(SP5K_RES_TYPE_JPG, menuSrcFileInfo[MENU_FIVE_LINES_BAR_HDMI].fileName, PRELOAD_FIVE_LINES_BAR, width, height, 0);
	printf("MENU_FIVE_LINES_BAR_HDMI preload -- %s\n",(ret==FAIL)?"FAIL":"SUCCESS");

	width  = ROUND_UP_TO_DIVISIBLE(menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].pos[2],16);
	height = ROUND_UP_TO_DIVISIBLE(menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].pos[3],16);
	printf("%d  %d   width:%d height:%d\n",menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].pos[2],menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].pos[3],
		width,height);
	ret = sp5kRsvPreloadFile(SP5K_RES_TYPE_JPG, menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].fileName, PRELOAD_FOUR_LINES_BODY, width, height, 0);
	printf("MENU_FOUR_LINES_BODY_HDMI preload -- %s\n",(ret==FAIL)?"FAIL":"SUCCESS");

	width  = ROUND_UP_TO_DIVISIBLE(menuSrcFileInfo[MENU_FOUR_LINES_BAR_HDMI].pos[2],16);
	height = ROUND_UP_TO_DIVISIBLE(menuSrcFileInfo[MENU_FOUR_LINES_BAR_HDMI].pos[3],16);
	printf("%d  %d   width:%d height:%d\n",menuSrcFileInfo[MENU_FOUR_LINES_BAR_HDMI].pos[2],menuSrcFileInfo[MENU_FOUR_LINES_BAR_HDMI].pos[3],
		width,height);
	ret = sp5kRsvPreloadFile(SP5K_RES_TYPE_JPG, menuSrcFileInfo[MENU_FOUR_LINES_BAR_HDMI].fileName, PRELOAD_FOUR_LINES_BAR, width, height, 0);
	printf("MENU_FOUR_LINES_BAR_HDMI preload -- %s\n",(ret==FAIL)?"FAIL":"SUCCESS");

	width  = ROUND_UP_TO_DIVISIBLE(menuSrcFileInfo[MENU_SETUP_HDMI].pos[2],16);
	height = ROUND_UP_TO_DIVISIBLE(menuSrcFileInfo[MENU_SETUP_HDMI].pos[3],16);
	printf("%d  %d   width:%d height:%d\n",menuSrcFileInfo[MENU_SETUP_HDMI].pos[2],menuSrcFileInfo[MENU_SETUP_HDMI].pos[3],
		width,height);
	ret = sp5kRsvPreloadFile(SP5K_RES_TYPE_JPG, menuSrcFileInfo[MENU_SETUP_HDMI].fileName, PRELOAD_MODE_SETUP, width, height, 0);
	printf("MENU_SETUP_HDMI preload -- %s\n",(ret==FAIL)?"FAIL":"SUCCESS");

	width  = ROUND_UP_TO_DIVISIBLE(menuSrcFileInfo[MENU_BUTTON_OK_HDMI].pos[2],16);
	height = ROUND_UP_TO_DIVISIBLE(menuSrcFileInfo[MENU_BUTTON_OK_HDMI].pos[3],16);
	printf("%d  %d   width:%d height:%d\n",menuSrcFileInfo[MENU_BUTTON_OK_HDMI].pos[2],menuSrcFileInfo[MENU_BUTTON_OK_HDMI].pos[3],
		width,height);
	ret = sp5kRsvPreloadFile(SP5K_RES_TYPE_JPG, menuSrcFileInfo[MENU_BUTTON_OK_HDMI].fileName, PRELOAD_BUTTON_OK, width, height, 0);
	printf("MENU_BUTTON_OK_HDMI preload -- %s\n",(ret==FAIL)?"FAIL":"SUCCESS");

	width  = ROUND_UP_TO_DIVISIBLE(menuSrcFileInfo[MENU_BUTTON_MENU_HDMI].pos[2],16);
	height = ROUND_UP_TO_DIVISIBLE(menuSrcFileInfo[MENU_BUTTON_MENU_HDMI].pos[3],16);
	printf("%d  %d   width:%d height:%d\n",menuSrcFileInfo[MENU_BUTTON_MENU_HDMI].pos[2],menuSrcFileInfo[MENU_BUTTON_MENU_HDMI].pos[3],
		width,height);
	ret = sp5kRsvPreloadFile(SP5K_RES_TYPE_JPG, menuSrcFileInfo[MENU_BUTTON_MENU_HDMI].fileName, PRELOAD_BUTTON_MENU, width, height, 0);
	printf("MENU_BUTTON_MENU_HDMI preload -- %s\n",(ret==FAIL)?"FAIL":"SUCCESS");
#elif 0
	sp5kRsvPreloadDrawCfgSet(SP5K_RES_PRELOAD_NUM,13);

	ret = sp5kRsvPreloadFile(SP5K_RES_TYPE_JPG, menuSrcFileInfo[MENU_BODY_HDMI].fileName, PRELOAD_MENU_BODY, menuSrcFileInfo[MENU_BODY_HDMI].pos[2],menuSrcFileInfo[MENU_BODY_HDMI].pos[3], 0);
	printf("MENU_BODY_HDMI preload -- %s\n",(ret==FAIL)?"FAIL":"SUCCESS");

	ret = sp5kRsvPreloadFile(SP5K_RES_TYPE_JPG, menuSrcFileInfo[MENU_FIVE_LINES_BAR_HDMI].fileName, PRELOAD_FIVE_LINES_BAR, menuSrcFileInfo[MENU_FIVE_LINES_BAR_HDMI].pos[2],menuSrcFileInfo[MENU_FIVE_LINES_BAR_HDMI].pos[3], 0);
	printf("MENU_FIVE_LINES_BAR_HDMI preload -- %s\n",(ret==FAIL)?"FAIL":"SUCCESS");

	ret = sp5kRsvPreloadFile(SP5K_RES_TYPE_JPG, menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].fileName, PRELOAD_FOUR_LINES_BODY, menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].pos[2],menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].pos[3], 0);
	printf("MENU_FOUR_LINES_BODY_HDMI preload -- %s\n",(ret==FAIL)?"FAIL":"SUCCESS");

	ret = sp5kRsvPreloadFile(SP5K_RES_TYPE_JPG, menuSrcFileInfo[MENU_FOUR_LINES_BAR_HDMI].fileName, PRELOAD_FOUR_LINES_BAR, menuSrcFileInfo[MENU_FOUR_LINES_BAR_HDMI].pos[2],menuSrcFileInfo[MENU_FOUR_LINES_BAR_HDMI].pos[3], 0);
	printf("MENU_FOUR_LINES_BAR_HDMI preload -- %s\n",(ret==FAIL)?"FAIL":"SUCCESS");

	ret = sp5kRsvPreloadFile(SP5K_RES_TYPE_JPG, menuSrcFileInfo[MENU_SETUP_HDMI].fileName, PRELOAD_MODE_SETUP, menuSrcFileInfo[MENU_SETUP_HDMI].pos[2],menuSrcFileInfo[MENU_SETUP_HDMI].pos[3], 0);
	printf("MENU_SETUP_HDMI preload -- %s\n",(ret==FAIL)?"FAIL":"SUCCESS");

	ret = sp5kRsvPreloadFile(SP5K_RES_TYPE_JPG, menuSrcFileInfo[MENU_BUTTON_OK_HDMI].fileName, PRELOAD_BUTTON_OK, menuSrcFileInfo[MENU_BUTTON_OK_HDMI].pos[2],menuSrcFileInfo[MENU_BUTTON_OK_HDMI].pos[3], 0);
	printf("MENU_BUTTON_OK_HDMI preload -- %s\n",(ret==FAIL)?"FAIL":"SUCCESS");

	ret = sp5kRsvPreloadFile(SP5K_RES_TYPE_JPG, menuSrcFileInfo[MENU_BUTTON_MENU_HDMI].fileName, PRELOAD_BUTTON_MENU, menuSrcFileInfo[MENU_BUTTON_MENU_HDMI].pos[2],menuSrcFileInfo[MENU_BUTTON_MENU_HDMI].pos[3], 0);
	printf("MENU_BUTTON_MENU_HDMI preload -- %s\n",(ret==FAIL)?"FAIL":"SUCCESS");

	ret = sp5kRsvPreloadFile(SP5K_RES_TYPE_JPG, menuSrcFileInfo[MENU_TITLE_POS1_HDMI].fileName, PRELOAD_MENU_TITLE, menuSrcFileInfo[MENU_TITLE_POS1_HDMI].pos[2],menuSrcFileInfo[MENU_TITLE_POS1_HDMI].pos[3], 0);
	printf("MENU_BUTTON_MENU_HDMI preload -- %s\n",(ret==FAIL)?"FAIL":"SUCCESS");

	ret = sp5kRsvPreloadFile(SP5K_RES_TYPE_JPG, menuSrcFileInfo[MENU_STILL_HDMI].fileName, PRELOAD_MODE_STILL, menuSrcFileInfo[MENU_STILL_HDMI].pos[2],menuSrcFileInfo[MENU_STILL_HDMI].pos[3], 0);
	printf("MENU_BUTTON_MENU_HDMI preload -- %s\n",(ret==FAIL)?"FAIL":"SUCCESS");

	ret = sp5kRsvPreloadFile(SP5K_RES_TYPE_JPG, menuSrcFileInfo[MENU_VIDEO_HDMI].fileName, PRELOAD_MODE_VIDEO, menuSrcFileInfo[MENU_VIDEO_HDMI].pos[2],menuSrcFileInfo[MENU_VIDEO_HDMI].pos[3], 0);
	printf("MENU_BUTTON_MENU_HDMI preload -- %s\n",(ret==FAIL)?"FAIL":"SUCCESS");

	ret = sp5kRsvPreloadFile(SP5K_RES_TYPE_JPG, menuSrcFileInfo[MENU_AUDIO_HDMI].fileName, PRELOAD_MODE_AUDIO, menuSrcFileInfo[MENU_AUDIO_HDMI].pos[2],menuSrcFileInfo[MENU_AUDIO_HDMI].pos[3], 0);
	printf("MENU_BUTTON_MENU_HDMI preload -- %s\n",(ret==FAIL)?"FAIL":"SUCCESS");

	ret = sp5kRsvPreloadFile(SP5K_RES_TYPE_JPG, menuSrcFileInfo[MENU_PLAYBACK_HDMI].fileName, PRELOAD_MODE_PLAYBACK, menuSrcFileInfo[MENU_PLAYBACK_HDMI].pos[2],menuSrcFileInfo[MENU_PLAYBACK_HDMI].pos[3], 0);
	printf("MENU_BUTTON_MENU_HDMI preload -- %s\n",(ret==FAIL)?"FAIL":"SUCCESS");

	ret = sp5kRsvPreloadFile(SP5K_RES_TYPE_JPG, menuSrcFileInfo[MENU_PRINTER_HDMI].fileName, PRELOAD_MODE_PRINTER, menuSrcFileInfo[MENU_PRINTER_HDMI].pos[2],menuSrcFileInfo[MENU_PRINTER_HDMI].pos[3], 0);
	printf("MENU_BUTTON_MENU_HDMI preload -- %s\n",(ret==FAIL)?"FAIL":"SUCCESS");

#else
	sp5kRsvPreloadDrawCfgSet(SP5K_RES_PRELOAD_NUM,13);

	ret = sp5kRsvPreloadFile(SP5K_RES_TYPE_JPG, menuSrcFileInfo[MENU_BODY_HDMI].fileName, PRELOAD_MENU_BODY, 0,0, 0);
	printf("MENU_BODY_HDMI preload -- %s\n",(ret==FAIL)?"FAIL":"SUCCESS");

	ret = sp5kRsvPreloadFile(SP5K_RES_TYPE_JPG, menuSrcFileInfo[MENU_FIVE_LINES_BAR_HDMI].fileName, PRELOAD_FIVE_LINES_BAR, 0,0, 0);
	printf("MENU_FIVE_LINES_BAR_HDMI preload -- %s\n",(ret==FAIL)?"FAIL":"SUCCESS");

	ret = sp5kRsvPreloadFile(SP5K_RES_TYPE_JPG, menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].fileName, PRELOAD_FOUR_LINES_BODY, 0,0, 0);
	printf("MENU_FOUR_LINES_BODY_HDMI preload -- %s\n",(ret==FAIL)?"FAIL":"SUCCESS");

	ret = sp5kRsvPreloadFile(SP5K_RES_TYPE_JPG, menuSrcFileInfo[MENU_FOUR_LINES_BAR_HDMI].fileName, PRELOAD_FOUR_LINES_BAR, 0,0, 0);
	printf("MENU_FOUR_LINES_BAR_HDMI preload -- %s\n",(ret==FAIL)?"FAIL":"SUCCESS");

	ret = sp5kRsvPreloadFile(SP5K_RES_TYPE_JPG, menuSrcFileInfo[MENU_SETUP_HDMI].fileName, PRELOAD_MODE_SETUP, 0,0, 0);
	printf("MENU_SETUP_HDMI preload -- %s\n",(ret==FAIL)?"FAIL":"SUCCESS");

	ret = sp5kRsvPreloadFile(SP5K_RES_TYPE_JPG, menuSrcFileInfo[MENU_BUTTON_OK_HDMI].fileName, PRELOAD_BUTTON_OK,0,0, 0);
	printf("MENU_BUTTON_OK_HDMI preload -- %s\n",(ret==FAIL)?"FAIL":"SUCCESS");

	ret = sp5kRsvPreloadFile(SP5K_RES_TYPE_JPG, menuSrcFileInfo[MENU_BUTTON_MENU_HDMI].fileName, PRELOAD_BUTTON_MENU, 0,0, 0);
	printf("MENU_BUTTON_MENU_HDMI preload -- %s\n",(ret==FAIL)?"FAIL":"SUCCESS");

	ret = sp5kRsvPreloadFile(SP5K_RES_TYPE_JPG, menuSrcFileInfo[MENU_TITLE_POS1_HDMI].fileName, PRELOAD_MENU_TITLE, 0,0, 0);
	printf("MENU_TITLE_POS1_HDMI preload -- %s\n",(ret==FAIL)?"FAIL":"SUCCESS");

	ret = sp5kRsvPreloadFile(SP5K_RES_TYPE_JPG, menuSrcFileInfo[MENU_STILL_HDMI].fileName, PRELOAD_MODE_STILL, 0,0, 0);
	printf("MENU_STILL_HDMI preload -- %s\n",(ret==FAIL)?"FAIL":"SUCCESS");

	ret = sp5kRsvPreloadFile(SP5K_RES_TYPE_JPG, menuSrcFileInfo[MENU_VIDEO_HDMI].fileName, PRELOAD_MODE_VIDEO, 0,0, 0);
	printf("MENU_VIDEO_HDMI preload -- %s\n",(ret==FAIL)?"FAIL":"SUCCESS");

	ret = sp5kRsvPreloadFile(SP5K_RES_TYPE_JPG, menuSrcFileInfo[MENU_AUDIO_HDMI].fileName, PRELOAD_MODE_AUDIO, 0,0, 0);
	printf("MENU_AUDIO_HDMI preload -- %s\n",(ret==FAIL)?"FAIL":"SUCCESS");

	ret = sp5kRsvPreloadFile(SP5K_RES_TYPE_JPG, menuSrcFileInfo[MENU_PLAYBACK_HDMI].fileName, PRELOAD_MODE_PLAYBACK, 0,0, 0);
	printf("MENU_PLAYBACK_HDMI preload -- %s\n",(ret==FAIL)?"FAIL":"SUCCESS");

	ret = sp5kRsvPreloadFile(SP5K_RES_TYPE_JPG, menuSrcFileInfo[MENU_PRINTER_HDMI].fileName, PRELOAD_MODE_PRINTER, 0,0, 0);
	printf("MENU_PRINTER_HDMI preload -- %s\n",(ret==FAIL)?"FAIL":"SUCCESS");
#endif
}
#endif
/**************************************************************************
 *                                                                        *
 *  Function Name: menuPromptDisp                                         *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void menuPromptDisp(UINT16 attr)
{
	UINT8	line, col;
	UINT8 OsdMaxX, OsdMaxY;
	UINT16 cw,ch;
	
	appOsdLayoutAttrGet(&OsdMaxX, &OsdMaxY);
	appOsdFontSizeGet(&cw, &ch);
	if(ch <= 16)OsdMaxY-=1;
	
	line = OsdMaxY;
	col = OsdMaxX;

	appOsdColorSet(MENU_PROMPT_GET_FOCUS);
	if (attr & MENU_PROMPT_OK_ON)
	{
	#if FUNC_HOST_TOUCH_PANEL
		menuIconDispExt((MENU_BACK_ON_OFFSET)*MENU_CHAR_WIDTH, (line-1)*MENU_CHAR_HEIGHT+MENU_OFFSET, ID_ICON_BUTTON_OK,0);
		appOsd_GfxStringIDPIPDraw((MENU_BACK_ON_OFFSET+2)*MENU_CHAR_WIDTH+2, (line-1)*MENU_CHAR_HEIGHT+MENU_OFFSET, SP5K_GFX_ALIGN_TOP_LEFT, ID_STR_ENTER);

		menuProcButIconDisp(0xff,(MENU_BACK_ON_OFFSET)*MENU_CHAR_WIDTH,(line-1)*MENU_CHAR_HEIGHT+MENU_OFFSET,\
		(2*MENU_CHAR_WIDTH+2+appOsdLib_FontGetStringWidthbyId(ID_STR_ENTER)),MENU_CHAR_HEIGHT,\
		MENU_OBJ_ICON_OK,ID_ICON_NONE,MENU_FUNC_NULL,0x00,SP1K_MSG_TP_OK,0);	
	#else
	/*		marked by gx
		menuIconDispExt((MENU_BACK_ON_OFFSET)*MENU_CHAR_WIDTH, (line-1)*MENU_CHAR_HEIGHT+MENU_OFFSET, ID_ICON_BUTTON_OK,0);
	*/

	#if HDMI_MENU_BY_JPG
	#if KIT_WITH_HDMI   /*Add by Lation@20130717*/
	if(IS_HDMI_IN){
		sp5kGfxAttrSet(SP5K_GFX_PIP_REFRESH_ACTIVE, 0, 0, 0, 0);
	}
	#endif
	#endif

	#if KIT_WITH_HDMI   /*Add by Lation@20130717*/
	if(IS_HDMI_IN){
		#if HDMI_MENU_BY_JPG
		//sp5kResourceFilePagePlay(SP5K_RES_TYPE_JPG, 
	 	//	menuSrcFileInfo[MENU_BUTTON_OK_HDMI].fileName, SP5K_GFX_PAGE_PIP_BASE+0, menuSrcFileInfo[MENU_BUTTON_OK_HDMI].pos, 0);
		sp5kRsvPreloadDraw(PRELOAD_BUTTON_OK,HDMI_MENU_LAYER1,menuSrcFileInfo[MENU_BUTTON_OK_HDMI].pos,0);
		appOsd_GfxStringIDPIPDraw((MENU_BACK_ON_OFFSET+5)*MENU_CHAR_WIDTH, (line+2)*MENU_CHAR_HEIGHT+MENU_OFFSET+5, SP5K_GFX_ALIGN_TOP_LEFT, ID_STR_ENTER);
		#else
		//appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0,90,320,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICONFILE_BUTTON_HDMI,0x0001);
		//appOsd_GfxStringIDPIPDraw((MENU_BACK_ON_OFFSET+7)*MENU_CHAR_WIDTH, (line+2)*MENU_CHAR_HEIGHT+MENU_OFFSET, SP5K_GFX_ALIGN_TOP_LEFT, ID_STR_ENTER);
		
		UINT32 coordinateMenuIcon_Y, coordinateMenuString_Y;
		coordinateMenuIcon_Y=302;
		coordinateMenuString_Y=302;
		appOsd_FontIconFontSet(ID_FONT_ICON);
		//Draw OK , MENU ICON and String
		//#define ID_ICON_ZZPB_OKHOLD_HDMI		0x01a0
		//#define ID_ICON_ZZPB_OKPRESS_HDMI		0x01a1
		//#define ID_ICON_ZZPB_POWER_HDMI		0x01a2

		
		//appOsdLib_OsdLayIconDraw(250, coordinateMenuIcon_Y, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_ZZPB_POWER_HDMI );
		
		appOsdLib_OsdLayIconDraw(110, coordinateMenuIcon_Y, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_ZZPB_OKPRESS_HDMI);
		appOsdLib_OsdLayIconDraw(350, coordinateMenuIcon_Y, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_ZZPB_POWER_HDMI );
		appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
		
		appOsdLib_TextIDDraw(155,coordinateMenuString_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_STR_ENTER);
		appOsdLib_TextIDDraw(395,coordinateMenuString_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_STR_SELECT);
		//appOsdLib_TextIDDraw(395,coordinateMenuString_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_STR_PLAY);

		#endif
	}
	else{
		appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0,30,214,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICONFILE_BUTTON,0x0001);
		if(IS_TV_IN){
			appOsd_GfxStringIDPIPDraw((MENU_BACK_ON_OFFSET+2)*MENU_CHAR_WIDTH+2, (line-1)*MENU_CHAR_HEIGHT+3, SP5K_GFX_ALIGN_TOP_LEFT, ID_STR_ENTER);
		}else{
			appOsd_GfxStringIDPIPDraw((MENU_BACK_ON_OFFSET+2)*MENU_CHAR_WIDTH+2, (line-1)*MENU_CHAR_HEIGHT+MENU_OFFSET, SP5K_GFX_ALIGN_TOP_LEFT, ID_STR_ENTER);
		}
	}
	#endif
	#endif
	}
	
	if (attr & MENU_PROMPT_MENU_ON)
	{
	#if FUNC_HOST_TOUCH_PANEL
		menuIconDispExt((MENU_BACK_ON_OFFSET+10)*MENU_CHAR_WIDTH-2, (line-1)*MENU_CHAR_HEIGHT+MENU_OFFSET, ID_ICON_BUTTON_MENU,0);
		appOsd_GfxStringIDDraw((MENU_BACK_ON_OFFSET+12)*MENU_CHAR_WIDTH+2-2, (line-1)*MENU_CHAR_HEIGHT+MENU_OFFSET, SP5K_GFX_ALIGN_TOP_LEFT, ID_STR_BACK);

		menuProcButIconDisp(0xff,(MENU_BACK_ON_OFFSET+10)*MENU_CHAR_WIDTH,(line-1)*MENU_CHAR_HEIGHT+MENU_OFFSET,\
		(2*MENU_CHAR_WIDTH+2+appOsdLib_FontGetStringWidthbyId(ID_STR_BACK)),MENU_CHAR_HEIGHT,\
		MENU_OBJ_ICON_BACK,ID_ICON_NONE,MENU_FUNC_NULL,0x00,SP1K_MSG_KEY_PRESS_MENU,0);	

	#else
	/*		marked by gx
		menuIconDispExt((MENU_BACK_ON_OFFSET+10)*MENU_CHAR_WIDTH, (line-1)*MENU_CHAR_HEIGHT+MENU_OFFSET, ID_ICON_BUTTON_MENU,0);
	*/
	#if KIT_WITH_HDMI   /*Add by Lation@20130717*/
	if(IS_HDMI_IN){
		#if HDMI_MENU_BY_JPG
		//sp5kResourceFilePagePlay(SP5K_RES_TYPE_JPG, 
	 	//	menuSrcFileInfo[MENU_BUTTON_MENU_HDMI].fileName, SP5K_GFX_PAGE_PIP_BASE+0, menuSrcFileInfo[MENU_BUTTON_MENU_HDMI].pos, 0);
		sp5kRsvPreloadDraw(PRELOAD_BUTTON_MENU,HDMI_MENU_LAYER1,menuSrcFileInfo[MENU_BUTTON_MENU_HDMI].pos,0);
		appOsd_GfxStringIDPIPDraw((MENU_BACK_ON_OFFSET+31)*MENU_CHAR_WIDTH, (line+2)*MENU_CHAR_HEIGHT+MENU_OFFSET+5, SP5K_GFX_ALIGN_TOP_LEFT, ID_STR_BACK);
		#else
		appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0,410,320,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICONFILE_BUTTON_HDMI,0x0000);
		appOsd_GfxStringIDPIPDraw((MENU_BACK_ON_OFFSET+27)*MENU_CHAR_WIDTH, (line+2)*MENU_CHAR_HEIGHT+MENU_OFFSET, SP5K_GFX_ALIGN_TOP_LEFT, ID_STR_BACK);
		#endif
	}
	else{
		appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0,190,214,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICONFILE_BUTTON,0x0000);
		if(IS_TV_IN){
			appOsd_GfxStringIDPIPDraw((MENU_BACK_ON_OFFSET+12)*MENU_CHAR_WIDTH+2, (line-1)*MENU_CHAR_HEIGHT+3, SP5K_GFX_ALIGN_TOP_LEFT, ID_STR_BACK);
		}else{
			appOsd_GfxStringIDPIPDraw((MENU_BACK_ON_OFFSET+12)*MENU_CHAR_WIDTH+2, (line-1)*MENU_CHAR_HEIGHT+MENU_OFFSET, SP5K_GFX_ALIGN_TOP_LEFT, ID_STR_BACK);
		}
	}
	#endif
	#endif
	}
	
}

/**************************************************************************
 *                                                                        *
 *  Function Name: menuProcDisp                                           *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void menuProcDisp (void)
{
	UINT16	icon;
	UINT8	nodeItem = 0;
	UINT8	subItem=0;
	UINT8	selItem;
	UINT8	layer = 0;
	UINT8	item = 0;
	UINT8	line, col;
	UINT16	i;
	UINT32	menuRoot;
/*	UINT8	attr=0;*/
	UINT8 OsdMaxX, OsdMaxY;
	UINT32 strIdx;
/*	uiPara_t* puiPara = appUiParaGet();*/

	appOsdLayoutAttrGet(&OsdMaxX, &OsdMaxY);
	menuRoot = menuProcCurrRootGet();
	APP_OSD_REFRESH_OFF;
	CLEAR_OSD_SRC;
	CLEAR_PIP_SRC;
	
	appOsd_ColorBlendingSet(PAL_GREEN_2,PAL_BLEND_0);
	appOsd_ColorBlendingSet(PAL_GREEN_3,PAL_BLEND_0);
	appOsd_ColorBlendingSet(PAL_GREEN_4,PAL_BLEND_0);

	#if KIT_WITH_HDMI
	if(IS_HDMI_IN){
		#if !HDMI_MENU_BY_JPG
		appOsd_ColorBlendingSet(PAL_GREEN_2,PAL_BLEND_0);
		appOsd_ColorBlendingSet(PAL_GREEN_3,PAL_BLEND_0);
		appOsd_ColorBlendingSet(PAL_GREEN_4,PAL_BLEND_0);
		#else
		/* init PIP layer */
		if(!pipHDMIMenuFlag){
			menuHDMIPIPInit();
		}else{
			sp5kGfxAttrSet(SP5K_GFX_PEN_COLOR, 0x000000, 0, 0, 0);
			sp5kGfxAttrSet(SP5K_GFX_FILL_COLOR, 0x000000, 0, 0, 0);
			
			sp5kGfxAttrSet(SP5K_GFX_PIP_REFRESH_ACTIVE, 0, 0, 0, 0);
			if (menuProcLayerGet() == 1 || menuProcLayerGet() == 0){
				appOsd_GfxRectShapeDraw(HDMI_MENU_LAYER3, 0, 220,1920,955);	
			}
			appOsd_GfxRectShapeDraw(HDMI_MENU_LAYER0, 0, 220,1920,955);
			appOsd_GfxRectShapeDraw(HDMI_MENU_LAYER1, 0, 220,1920,955);
			appOsd_GfxRectShapeDraw(HDMI_MENU_LAYER2, 0, 0,1920,1080);
			//appOsd_GfxRectShapeDraw(HDMI_MENU_LAYER2, 0, 220,1920,955);
			//appOsd_GfxRectShapeDraw(HDMI_MENU_LAYER4, 0, 0,1920,1080);
			//sp5kGfxAttrSet(SP5K_GFX_PIP_REFRESH_ACTIVE, 1, 0, 1, 0);
		}

		dlgUpdateFlag = 0;
		#endif
	}
	#endif 

	printf("@@@@@@ pos:%x\n",menuProcCurrPosGet());
	#if 1
	 /* Highlight the tab */
	if (menuProcLayerGet() == 0)
	{
		layer = 1;
		item = 0;
		menuLayerSizeGet(layer, &line, &col);
		#if FUNC_HOST_TOUCH_PANEL
		menuGuiClr(0, MENU_OBJ_MAX);
		#endif

		menuBodyDisp(0);
		menuHeaderDisp(0);

		nodeItem = menuProcNodeItemGet(layer, item);
		nodeItem -= menuProcScrollGet(layer);

		if (nodeItem > line)
			nodeItem = line;
		item = menuProcScrollGet(layer);
		
		for(i=0; i<nodeItem; i++,item++)
		{
			menuProcStrGet(layer, item, &strIdx);
			icon = menuProcIconIDGet(layer, item);
			subItem = menuProcSubItemGet(layer, item);
			
			if (subItem > 0)
			{
				selItem = menuProcParaGet (layer, item);
				icon = menuProcChildIconIDGet(layer, item, selItem);
			}
			if (menuProcIsActive(layer, item))
			{
				appOsdColorSet(MENU_STR_LOST_FOCUS);
				menuStrDraw(0, i, strIdx, 0, layer);
				menuIconDisp(col-2, i, icon, 0, layer);
			}
			else
			{
				appOsdColorSet(MENU_STR_DISABLE);
				menuStrDraw(0, i, strIdx, 0, layer);
				menuIconDisp(col-2, i, icon, 0, layer);
			}
			#if FUNC_HOST_TOUCH_PANEL
			menuProcButIconDisp(0xff,MENU_LAYER1_STR_X_OFFSET,i * MENU_CHAR_HEIGHT + MENU_LAYER1_STR_Y_OFFSET,\
				(sp1kOsdIconWidthGet(ID_ICON_MUBAR)),sp1kOsdIconHeightGet(ID_ICON_MUBAR),\
				 MENU_OBJ_ITEM0+i,ID_ICON_NONE,MENU_FUNC_SELITEM,i,SP1K_MSG_NULL,0\
				);
			#endif
		}
		#if FUNC_HOST_TOUCH_PANEL
		menuPromptDisp(MENU_PROMPT_OK_OFF| MENU_PROMPT_MENU_ON);
		#else
		menuPromptDisp(MENU_PROMPT_OK_ON | MENU_PROMPT_MENU_ON);
		#endif
	}
	#endif
	#if 1 /* Highlight the setting menu */
	else if (menuProcLayerGet() == 1)
	{
		layer = 1;
		item = 0;
		
		menuLayerSizeGet(layer, &line, &col);

		#if FUNC_HOST_TOUCH_PANEL
		menuGuiClr(0, MENU_OBJ_MAX);
		#endif

		#if HDMI_MENU_BY_JPG
		#if KIT_WITH_HDMI   /*Add by Lation@20130717*/
		if(IS_HDMI_IN){
			appOsd_GfxRectShapeDraw(HDMI_MENU_LAYER2, 0, 220,1920,955);
		}	
		#endif
		#endif
		menuBodyDisp(layer);
		menuHeaderDisp(layer);
		
		nodeItem = menuProcNodeItemGet(layer, item);
		nodeItem -= menuProcScrollGet(layer);

		if (nodeItem > line)
			nodeItem = line;
		item = menuProcScrollGet(layer);

		for(i=0; i<nodeItem; i++,item++)
		{
			menuProcStrGet(layer,item, &strIdx);
			icon = menuProcIconIDGet(layer, item);
			subItem = menuProcSubItemGet(layer, item);

			if (subItem > 0)
			{
				selItem = menuProcParaGet (layer, item);
				icon = menuProcChildIconIDGet(layer, item, selItem);
			}

			if (!menuProcIsActive(layer, item))
			{
				appOsdColorSet(MENU_STR_DISABLE);
				menuStrDraw(0, i, strIdx, 0, layer);
				menuIconDisp(col-2, i, icon, 0, layer);
			}
			else if (menuProcCurrItemGet(layer) == item)
			{
				menuBarDisp(i, layer);
				appOsdColorSet(MENU_STR_GET_FOCUS);
				menuStrDraw(0, i, strIdx, 0, layer);
				appOsdColorSet(MENU_ICON_GET_FOCUS);
				menuIconDisp(col-2, i, icon, 1, layer);
			}
			else
			{
				appOsdColorSet(MENU_STR_LOST_FOCUS);
				menuStrDraw(0, i, strIdx, 0, layer);
				menuIconDisp(col-2, i, icon, 0, layer);
			}
			#if FUNC_HOST_TOUCH_PANEL
			menuProcButIconDisp(0xff,MENU_LAYER1_STR_X_OFFSET,i * MENU_CHAR_HEIGHT + MENU_LAYER1_STR_Y_OFFSET,\
				(sp1kOsdIconWidthGet(ID_ICON_MUBAR)),sp1kOsdIconHeightGet(ID_ICON_MUBAR),\
				 MENU_OBJ_ITEM0+i,ID_ICON_NONE,MENU_FUNC_SELITEM,i,SP1K_MSG_NULL,0\
				);
			#endif

		}

		#if FUNC_HOST_TOUCH_PANEL
		menuPromptDisp(MENU_PROMPT_OK_OFF| MENU_PROMPT_MENU_ON);
		#else
		menuPromptDisp(MENU_PROMPT_OK_ON | MENU_PROMPT_MENU_ON);
		#endif
	}
	#endif

	#if 1 /* Highlight the option menu */
	else if (menuProcLayerGet() == 2)
	{
		layer = 1;
		item = 0;
		menuLayerSizeGet(layer, &line, &col);
		#if FUNC_HOST_TOUCH_PANEL
		menuGuiClr(0, MENU_OBJ_MAX);
		#endif

		#if HDMI_MENU_BY_JPG
		#if KIT_WITH_HDMI   /*Add by Lation@20130717*/
		if(!IS_HDMI_IN){
			menuBodyDisp(layer);
		}
		#endif
		#else
			menuBodyDisp(layer);
		#endif
		
		menuHeaderDisp(layer);
		nodeItem = menuProcNodeItemGet(layer, item);
		nodeItem -= menuProcScrollGet(layer);

		if (nodeItem > line)
			nodeItem = line;
		item = menuProcScrollGet(layer);

		for(i=0; i<nodeItem; i++,item++)
		{
			menuProcStrGet(layer,item, &strIdx);
			icon = menuProcIconIDGet(layer, item);
			subItem = menuProcSubItemGet(layer, item);
			
			if (subItem > 0)
			{
				selItem = menuProcParaGet (layer, item);
				icon = menuProcChildIconIDGet(layer, item, selItem);
			}

			if (!menuProcIsActive(layer, item))
			{
				appOsdColorSet(MENU_STR_DISABLE);
				menuStrDraw(0, i, strIdx, 0, layer);
				menuIconDisp(col-2, i, icon, 0, layer);
			}
			else if (menuProcCurrItemGet(layer) == item)
			{
				menuBarDisp(i, layer);
				appOsdColorSet(MENU_STR_GET_FOCUS);
				menuStrDraw(0, i, strIdx, 1, layer);
				menuIconDisp(col-2, i, icon, 1, layer);
			}
			else
			{
				appOsdColorSet(MENU_STR_DISABLE);
				menuStrDraw(0, i, strIdx, 0, layer);
				menuIconDisp(col-2, i, icon, 0, layer);
			}
		}
 

		layer = 2;
		item = 0;
		#if FUNC_HOST_TOUCH_PANEL
		menuGuiClr(0, MENU_OBJ_MAX);
		#endif
		menuBodyDisp(layer);
		//menuHeaderDisp(layer);

		menuLayerSizeGet(layer, &line, &col);

		nodeItem = menuProcNodeItemGet(layer, item);
		nodeItem -= menuProcScrollGet(layer);

		if (nodeItem > line)
			nodeItem = line;
		item = menuProcScrollGet(layer);
 

		for(i=0; i<nodeItem; i++,item++)
		{
			menuProcStrGet(layer,item, &strIdx);
			icon = menuProcIconIDGet(layer, item);

			if (!menuProcIsActive(layer, item))
			{
				appOsdColorSet(MENU_STR_DISABLE);
				if(menuRoot == MENU_MARK_PLAYBACK)
				{
					menuStrDraw(0, i, strIdx, 0, layer);
				}
				else
				{
					menuStrDraw(4, i, strIdx, 0, layer);
					menuIconDisp(4, i, icon, 0, layer);
				}
			}
			else if (menuProcCurrItemGet(layer) == item)
			{
				menuBarDisp(i, layer);
				appOsdColorSet(MENU_STR_GET_FOCUS);
				
				if(menuRoot == MENU_MARK_PLAYBACK)
				{
					menuStrDraw(0, i, strIdx, 1, layer);
				}
				else
				{
					menuStrDraw(4, i, strIdx, 1, layer);
					appOsdColorSet(MENU_ICON_GET_FOCUS);
					menuIconDisp(4, i, icon, 1, layer);
				}
			}
			else
			{
				appOsdColorSet(MENU_STR_LOST_FOCUS);
				if(menuRoot == MENU_MARK_PLAYBACK) 
				{
					menuStrDraw(0, i, strIdx, 0, layer);
				}
				else
				{
					menuStrDraw(4, i, strIdx, 0, layer);
					menuIconDisp(4, i, icon, 0, layer);
				}
			}
			#if FUNC_HOST_TOUCH_PANEL
			menuProcButIconDisp(0xff,MENU_LAYER2_STR_X_OFFSET,i * MENU_CHAR_HEIGHT + MENU_LAYER2_STR_Y_OFFSET,\
				sp1kOsdIconWidthGet(ID_ICON_MUSBAR),sp1kOsdIconHeightGet(ID_ICON_MUSBAR),\
				 MENU_OBJ_ITEM0+i,ID_ICON_NONE,MENU_FUNC_SELITEM,i,SP1K_MSG_NULL,0\
				);
			#endif
		}
		#if FUNC_HOST_TOUCH_PANEL
		menuPromptDisp(MENU_PROMPT_OK_OFF| MENU_PROMPT_MENU_ON);
		#else
		menuPromptDisp(MENU_PROMPT_OK_ON | MENU_PROMPT_MENU_ON);
		#endif
	}
	#endif

	#if 1 /* Highlight the option menu */
	else if (menuProcLayerGet() == 3)
	{
		layer = 3;
		item = 0;
		menuLayerSizeGet(layer, &line, &col);

		menuBodyDisp(layer);
		//menuHeaderDisp(layer);

		nodeItem = menuProcNodeItemGet(layer, item);
		nodeItem -= menuProcScrollGet(layer);

		if (nodeItem > line)
			nodeItem = line;
		item = menuProcScrollGet(layer);

		for(i=0; i<nodeItem; i++,item++)
		{
			menuProcStrGet(layer,item, &strIdx);
			icon = menuProcIconIDGet(layer, item);

			if (!menuProcIsActive(layer, item))
			{
				appOsdColorSet(MENU_STR_DISABLE);
				if(fileTypeSelect == 1)
				{
					menuStrDraw(1, i + 1, strIdx, 0, layer);
				}
				else
				{
					menuStrDraw(2, i + (line - nodeItem), strIdx, 0, layer);
				}
			}
			else if (menuProcCurrItemGet(layer) == item)
			{
				if(fileTypeSelect == 1)
				{
					menuBarDisp(i+ 1, layer);
					appOsdColorSet(MENU_STR_GET_FOCUS);
					menuStrDraw(1, i+ 1, strIdx, 1, layer);
				}
				else
				{
					menuBarDisp(i + (line - nodeItem), layer);
					appOsdColorSet(MENU_STR_GET_FOCUS);
					menuStrDraw(2, i + (line - nodeItem), strIdx, 1, layer);
				}
			}
			else
			{
				appOsdColorSet(MENU_STR_LOST_FOCUS);
				if(fileTypeSelect == 1)
				{
					menuStrDraw(1, i+ 1, strIdx, 0, layer);
				}
				else
				{
					menuStrDraw(2, i + (line - nodeItem), strIdx, 0, layer);
				}
			}
		}

		item = menuProcCurrItemGet(layer-1);
		menuProcStrGet(layer-1,item, &strIdx);

		if(fileTypeSelect == 0)
		{
			menuBarDisp(line-nodeItem-1, layer);
			appOsdColorSet(MENU_STR_GET_FOCUS);
			appOsdLib_TextIDDraw(\
				(LCD_WIDTH-appOsdLib_FontGetStringWidthbyId(strIdx))/2, \
				(line-nodeItem)*MENU_CHAR_HEIGHT, \
				SP5K_GFX_ALIGN_TOP_LEFT, strIdx
			);		
		}
		
		#if FUNC_HOST_TOUCH_PANEL
		menuPromptDisp(MENU_PROMPT_OK_OFF| MENU_PROMPT_MENU_OFF);
		#else
		menuPromptDisp(MENU_PROMPT_OK_ON | MENU_PROMPT_MENU_OFF);
		#endif
	}
	#endif

	#if KIT_WITH_HDMI && HDMI_MENU_BY_JPG
	if(IS_HDMI_IN){
		sp5kGfxAttrSet(SP5K_GFX_PIP_LAYER_COLOR_KEY, HDMI_MENU_LAYER0, 10, 1, 0);
		sp5kGfxAttrSet(SP5K_GFX_PIP_LAYER_ACTIVE, HDMI_MENU_LAYER0, 1, 0, 0);

		sp5kGfxAttrSet(SP5K_GFX_PIP_LAYER_COLOR_KEY, HDMI_MENU_LAYER1, 10, 1, 0);
		sp5kGfxAttrSet(SP5K_GFX_PIP_LAYER_ACTIVE, HDMI_MENU_LAYER1, 1, 0, 0);

		sp5kGfxAttrSet(SP5K_GFX_PIP_LAYER_COLOR_KEY, HDMI_MENU_LAYER2, 10, 1, 0);
		sp5kGfxAttrSet(SP5K_GFX_PIP_LAYER_ACTIVE, HDMI_MENU_LAYER2, 1, 0, 0);

		sp5kGfxAttrSet(SP5K_GFX_PIP_LAYER_COLOR_KEY, HDMI_MENU_LAYER3, 10, 1, 0);
		sp5kGfxAttrSet(SP5K_GFX_PIP_LAYER_ACTIVE, HDMI_MENU_LAYER3, 1, 0, 0);

		sp5kGfxAttrSet(SP5K_GFX_PIP_LAYER_COLOR_KEY, HDMI_MENU_LAYER4, 0, 0, 0);
		sp5kGfxAttrSet(SP5K_GFX_PIP_LAYER_ACTIVE, HDMI_MENU_LAYER4, 1, 0, 0);

		sp5kGfxAttrSet(SP5K_GFX_PIP_REFRESH_ACTIVE, 1, 0, 1, 0);
	}
	#endif
	APP_OSD_REFRESH_ON;
}

void menuEVDisp(void)
{
	UINT8 layer = menuProcLayerGet();
	UINT8 item = menuProcCurrItemGet(layer);
/*	UINT8 icon = menuProcIconIDGet(layer, item);*/
	UINT8 line, col;
	UINT8	nodeItem;
	UINT8 count=0;
	UINT16 xpos,ypos,width,heigth;
	UINT8 depth,color;
	UINT32 strIdx;

	menuProcStrGet(layer,item, &strIdx);
	APP_OSD_REFRESH_OFF;
	CLEAR_OSD_SRC;

	menuLayerSizeGet(layer, &line, &col);
	appOsdColorSet(MENU_STR_LOST_FOCUS);
	nodeItem = menuProcNodeItemGet(layer, item);
	menuStrDisp((col-6)/2, line-2, "EV", 0, layer);
	menuStrDraw((col-6)/2+3, line-2, strIdx, 0, layer);
	//menuStrDisp((col-6)/2-2, line-1, exposure, 0, layer);
	xpos=MENU_CHAR_WIDTH*(col/4);
	ypos =(MENU_CHAR_HEIGHT<=16)?(line*MENU_CHAR_HEIGHT+MENU_LAYER2_STR_Y_OFFSET): (LCD_HEIGHT-2*MENU_CHAR_HEIGHT);
	width=MENU_CHAR_WIDTH/2;
	heigth=MENU_CHAR_HEIGHT/4;
	depth=MENU_CHAR_WIDTH/4;
	
	for(count=0;count<13;count++)
	{	xpos+=MENU_CHAR_WIDTH;
		if(item == count)
		{
			color=0x04;
		}
		else
		{
			color=0x3e;
		}
		if(count%3 ==0)
		{
			appOsdLib_OsdLayerFrameDraw(xpos,ypos-heigth,width,heigth*3,depth);
		}
		else
		{
			appOsdLib_OsdLayerFrameDraw(xpos,ypos,width,heigth*2,depth);
		}

	}
	
	//appOsdColorSet(MENU_TAB_GET_FOCUS);
	//osdCharDrawExt((((UINT16)col-6)/2 + menuXOff)*MENU_CHAR_WIDTH+item*fontW +8 , ((UINT16)line+menuYOff)*32 -10, exposure[item],1);
	#if FUNC_HOST_TOUCH_PANEL
	menuPromptDisp(MENU_PROMPT_OK_OFF| MENU_PROMPT_MENU_ON);
	#else
	menuPromptDisp(MENU_PROMPT_OK_ON | MENU_PROMPT_MENU_ON);
	#endif
	APP_OSD_REFRESH_ON;
}

#if CAM_TYPE_CVR
void menuWIFIPasswordDisp(void)
{
	UINT8 layer = menuProcLayerGet();
	UINT8 item = menuProcCurrItemGet(layer);
	UINT8 i;
	UINT8 nodeItem, xpos=0, ypos=1;
	UINT8 str[32];
	uiPara_t* puiPara=appUiParaGet();
	UINT8 Data[8]={
		puiPara->WIFIPassword_7,
		puiPara->WIFIPassword_6,
		puiPara->WIFIPassword_5,
		puiPara->WIFIPassword_4,
		puiPara->WIFIPassword_3,
		puiPara->WIFIPassword_2,
		puiPara->WIFIPassword_1,
		puiPara->WIFIPassword_0
	};
	

	APP_OSD_REFRESH_OFF;
	CLEAR_OSD_SRC;
	CLEAR_PIP_SRC;
	nodeItem = menuProcNodeItemGet(layer, item);
	
	for(i=0, item=0; i<nodeItem; i++,item++)
	{
		sprintf(str, "%d", Data[item]);
	
		if (menuProcCurrItemGet(layer) == item){
			appOsd_ColorDrawTextColorSet(PAL_RED, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
		}else{
				
			appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
		}
		appOsd_GfxStringDraw((xpos+2)*MENU_CHAR_WIDTH, ypos*MENU_CHAR_HEIGHT+MENU_LAYER2_STR_Y_OFFSET, SP5K_GFX_ALIGN_TOP_LEFT,str);	
		xpos += 2;

	}
	
	//xpos += 2;
	menuPromptDisp(MENU_PROMPT_OK_OFF| MENU_PROMPT_MENU_ON);
	
	APP_OSD_REFRESH_ON;
}
#endif

#if FUNC_HOST_TOUCH_PANEL
void menuRTCDisp(void)
{
	UINT8 layer = menuProcLayerGet();
	UINT8 item = menuProcCurrItemGet(layer);
	UINT16 icon = menuProcIconIDGet(layer, item);
	UINT8 line, col, i;
	UINT8 nodeItem, selItem, xpos=0, ypos=1;
	UINT16 iconXpos;
	UINT32 strIdx;

	APP_OSD_REFRESH_OFF;
	CLEAR_OSD_SRC;
	
	menuGuiClr(0,MENU_OBJ_MAX);
	
	menuLayerSizeGet(layer, &line, &col);

 	nodeItem = menuProcNodeItemGet(layer, item);
	
	for(i=0, item=0; i<nodeItem; i++,item++)
	{
		selItem = menuProcParaGet (layer, menuDateItemConv(item));
		menuProcChildStrGet(layer, menuDateItemConv(item), selItem, &strIdx);
		if (menuProcCurrItemGet(layer) == menuDateItemConv(item))
		{
			if(menuProcCurrItemGet(layer)==5)
			{
				iconXpos = (xpos+2)*MENU_CHAR_WIDTH+(appOsdLib_FontGetStringWidthbyId(strIdx))/2-14;
			}
			else
			{
				iconXpos = (xpos+2)*MENU_CHAR_WIDTH;
			}
			appOsdColorSet(OSD_COLOR_YELLOW);
			sp1kOsdStrDrawExt((xpos+2)*MENU_CHAR_WIDTH, ypos*MENU_CHAR_HEIGHT+MENU_LAYER2_STR_Y_OFFSET, buf,0);
			menuProcButIconDisp(MENU_PROMPT_GET_FOCUS,iconXpos,(ypos-1)*MENU_CHAR_HEIGHT+MENU_LAYER2_STR_Y_OFFSET,\
				appOsdLib_FontGetStringWidthbyId(strIdx),MENU_CHAR_HEIGHT,\
				MENU_OBJ_ICON_UP,ID_ICON_TBUTTON_UP,MENU_FUNC_NULL,0x00,SP1K_MSG_KEY_PRESS_UP,0);
			menuProcButIconDisp(MENU_PROMPT_GET_FOCUS,iconXpos,(ypos+1)*MENU_CHAR_HEIGHT+MENU_LAYER2_STR_Y_OFFSET,\
				appOsdLib_FontGetStringWidthbyId(strIdx),MENU_CHAR_HEIGHT,\
				MENU_OBJ_ICON_DOWN,ID_ICON_TBUTTON_DOWN,MENU_FUNC_NULL,0x00,SP1K_MSG_KEY_PRESS_DOWN,0);		
			menuProcButIconDisp(0xff,(xpos+2)*MENU_CHAR_WIDTH,(ypos-0)*MENU_CHAR_HEIGHT+MENU_LAYER2_STR_Y_OFFSET,\
				appOsdLib_FontGetStringWidthbyId(strIdx),MENU_CHAR_HEIGHT,\
				MENU_OBJ_ITEM0+i,ID_ICON_NONE,MENU_FUNC_SELITEM,i,SP1K_MSG_NULL,0\
				);

			xpos += appOsdLib_FontGetStringWidthbyId(strIdx);

		}
		else
		{
			appOsdColorSet(MENU_STR_LOST_FOCUS);
			sp1kOsdStrDrawExt((xpos+2)*MENU_CHAR_WIDTH, ypos*MENU_CHAR_HEIGHT+MENU_LAYER2_STR_Y_OFFSET, buf,0);
			menuProcButIconDisp(0xff,(xpos+2)*MENU_CHAR_WIDTH,(ypos-0)*MENU_CHAR_HEIGHT+MENU_LAYER2_STR_Y_OFFSET,\
				appOsdLib_FontGetStringWidthbyId(strIdx),MENU_CHAR_HEIGHT,\
				MENU_OBJ_ITEM0+i,ID_ICON_NONE,MENU_FUNC_SELITEM,i,SP1K_MSG_NULL,0\
				);

			xpos += appOsdLib_FontGetStringWidthbyId(strIdx);
		}
		if ((item == 0) || (item == 1))
		{
			appOsdColorSet(MENU_STR_LOST_FOCUS);
			sp1kOsdStrDrawExt((xpos+2)*MENU_CHAR_WIDTH, ypos*MENU_CHAR_HEIGHT+MENU_LAYER2_STR_Y_OFFSET, "/",0);
			xpos += strlen("/");
		}
		else if (item == 3)
		{
			appOsdColorSet(MENU_STR_LOST_FOCUS);
			sp1kOsdStrDrawExt((xpos+2)*MENU_CHAR_WIDTH, ypos*MENU_CHAR_HEIGHT+MENU_LAYER2_STR_Y_OFFSET, ":",0);
			xpos += strlen(":");
					
		}
		else if (item == 4)
		{
			if(MENU_CHAR_WIDTH<=8)
				xpos = 2;
			else
				xpos = 3;
			
			ypos += 2;
			
		}
		else
			xpos++;
	}
	menuPromptDisp(MENU_PROMPT_OK_OFF| MENU_PROMPT_MENU_ON);
	
	APP_OSD_REFRESH_ON;
}

void menuProcDisp2Layer(void)
{
	UINT8 layer = menuProcLayerGet();
	UINT8 item = menuProcCurrItemGet(layer);
	UINT16 icon = menuProcIconIDGet(layer, item);
	UINT8 line, col, i;
	UINT8 nodeItem, selItem, subItem;
	UINT32 strIdx;

	APP_OSD_REFRESH_OFF;
	CLEAR_OSD_SRC;

	layer--;
	item = 0;
	menuLayerSizeGet(layer, &line, &col);
	

	menuBodyDisp(layer);
	menuHeaderDisp(layer);
	
	if(layer == 2)
	{
		osdIconDrawExt(MENU_BODY_LAYER2_X_OFFSET, MENU_BODY_LAYER2_Y_OFFSET, ID_ICON_MUSUBSC,0);
	}
	nodeItem = menuProcNodeItemGet(layer, item);
	nodeItem -= menuProcScrollGet(layer);


	if (nodeItem > line)
		nodeItem = line;
	item = menuProcScrollGet(layer);

	for(i=0; i<nodeItem; i++,item++)
	{
		menuProcStrGet(layer,item, &strIdx);
		icon = menuProcIconIDGet(layer, item);
		subItem = menuProcSubItemGet(layer, item);
		if (subItem > 0)
		{
			selItem = menuProcParaGet (layer, item);
			icon = menuProcChildIconIDGet(layer, item, selItem);
		}

		if (!menuProcIsActive(layer, item))
		{
			appOsdColorSet(MENU_STR_DISABLE);
			menuStrDraw(0, i, strIdx, 0, layer);
			menuIconDisp(col-2, i, icon, 0, layer);
		}
		else if (menuProcCurrItemGet(layer) == item)
		{
			menuBarDisp(i, layer);
			appOsdColorSet(MENU_STR_GET_FOCUS);
			menuStrDraw(0, i, strIdx, 1, layer);
			menuIconDisp(col-2, i, icon, 1, layer);
		}
		else
		{
			appOsdColorSet(MENU_STR_DISABLE);
			menuStrDraw(0, i, strIdx, 0, layer);
			menuIconDisp(col-2, i, icon, 0, layer);
		}	


	}

	
	layer++;
	item = 0;
	menuLayerSizeGet(layer, &line, &col);
	menuGuiClr(0,MENU_OBJ_MAX);

	menuBodyDisp(layer);
 	//menuHeaderDisp(layer);
	menuGuiClr(MENU_OBJ_ICON_UP,MENU_OBJ_ICON_DOWN+1);

	if(layer == 2)
	{
		appGuiObjClr(MENU_OBJ_ICON_UP);
		osdIconDrawExt(MENU_BODY_LAYER2_X_OFFSET, MENU_BODY_LAYER2_Y_OFFSET, ID_ICON_MUSUBSC,0);
	}

	nodeItem = menuProcNodeItemGet(layer, item);
	nodeItem -= menuProcScrollGet(layer);

	if (nodeItem > line)
		nodeItem = line;


	item = menuProcScrollGet(layer);

	for(i=0; i<nodeItem; i++,item++)
	{
		menuProcStrGet(layer,item, &strIdx);
		subItem = menuProcSubItemGet(layer, item);
		if (!menuProcIsActive(layer, item))
		{
			appOsdColorSet(MENU_STR_DISABLE);
			menuStrDraw(0, i, strIdx, 0, layer);
			appOsdColorSet(MENU_STR_LOST_FOCUS);
			menuStrDisp(col-6, i, " ", 0, layer);
			appOsdColorSet(MENU_STR_DISABLE);
			if (subItem)
			{
				selItem = menuProcParaGet (layer, item);
				menuProcChildStrGet(layer, item, selItem, &strIdx);
				menuStrDispExt((col-2)*MENU_CHAR_WIDTH+(54-appOsdLib_FontGetStringWidthbyId(strIdx))/2, (i+2)*MENU_CHAR_HEIGHT-2, buf, 1);
			}
		}
		else if (menuProcCurrItemGet(layer) == item)
		{
			//menuBarDisp(i, layer);
			appOsdColorSet(MENU_TAB_GET_FOCUS);
			menuStrDraw(0, i, strIdx, 1, layer);
			if (subItem)
			{
				appOsdColorSet(MENU_TAB_GET_FOCUS);
				menuIconDispExt((col-2)*MENU_CHAR_WIDTH, (i+2)*MENU_CHAR_HEIGHT-2, ID_ICON_MU2BARSL, 1);
				
				menuProcButIconDisp(MENU_BODY_DISABLE,(col-2-2)*MENU_CHAR_WIDTH,(i+2)*MENU_CHAR_HEIGHT-2,\
					MENU_CHAR_WIDTH*2,MENU_CHAR_HEIGHT,\
					MENU_OBJ_ICON_LEFT,ID_ICON_TBUTTON_LEFT,MENU_FUNC_NULL,0x00,SP1K_MSG_KEY_PRESS_LEFT,0);
				menuProcButIconDisp(MENU_BODY_DISABLE,(col-2)*MENU_CHAR_WIDTH+sp1kOsdIconWidthGet(ID_ICON_MU2BARSL),(i+2)*MENU_CHAR_HEIGHT-2,\
					MENU_CHAR_WIDTH*2,MENU_CHAR_HEIGHT,\
					MENU_OBJ_ICON_RIGHT,ID_ICON_TBUTTON_RIGHT,MENU_FUNC_NULL,0x00,SP1K_MSG_KEY_PRESS_RIGHT,0);

				selItem = menuProcParaGet (layer, item);
				menuProcChildStrGet(layer, item, selItem, &strIdx);
				appOsdColorSet(MENU_STR_GET_FOCUS);
				menuStrDispExt((col-2)*MENU_CHAR_WIDTH+(MENU_MU2BARSL_WIDTH-appOsdLib_FontGetStringWidthbyId(strIdx))/2, (i+2)*MENU_CHAR_HEIGHT-2, buf, 1);
			}
		}
		else
		{
			appOsdColorSet(MENU_STR_LOST_FOCUS);
			menuStrDraw(0, i, strIdx, 0, layer);
			menuStrDisp(col-6, i, " ", 0, layer);
			if (subItem)
			{
				selItem = menuProcParaGet (layer, item);
				menuProcChildStrGet(layer, item, selItem, &strIdx);
				menuStrDispExt((col-2)*MENU_CHAR_WIDTH+(MENU_MU2BARSL_WIDTH-appOsdLib_FontGetStringWidthbyId(strIdx))/2, (i+2)*MENU_CHAR_HEIGHT-2, buf, 1);
			}
		}
		menuProcButIconDisp(0xff,MENU_LAYER2_STR_X_OFFSET,i * MENU_CHAR_HEIGHT + MENU_LAYER2_STR_Y_OFFSET,\
			(sp1kOsdIconWidthGet(ID_ICON_MUSBAR)/2),sp1kOsdIconHeightGet(ID_ICON_MUSBAR),\
			 MENU_OBJ_ITEM0+i,ID_ICON_NONE,MENU_FUNC_SELITEM,i,SP1K_MSG_NULL,0\
			);

	}

	menuPromptDisp(MENU_PROMPT_OK_OFF| MENU_PROMPT_MENU_ON);

	APP_OSD_REFRESH_ON;
}

void menuProcDispYesNo(UINT16 idx)
{
	UINT8 layer = menuProcLayerGet();
	UINT8 item = menuProcCurrItemGet(layer);
	UINT16 icon = menuProcIconIDGet(layer, item);
	UINT8 line, col, i;
	UINT8 nodeItem;
	UINT32 strIdx;
	
	APP_OSD_REFRESH_OFF;
	CLEAR_OSD_SRC;
	menuGuiClr(MENU_OBJ_ITEM0, MENU_OBJ_MAX);
	menuLayerSizeGet(layer, &line, &col);

	menuBodyDisp(3);

	nodeItem = menuProcNodeItemGet(layer, item);
	nodeItem -= menuProcScrollGet(layer);;

	if (nodeItem > line)
		nodeItem = line;
	item = menuProcScrollGet(layer);

	for(i=0; i<nodeItem; i++,item++)
	{
		menuProcStrGet(layer,item, &strIdx);
		icon = menuProcIconIDGet(layer, item);

		if (!menuProcIsActive(layer, item))
		{
			appOsdColorSet(MENU_STR_DISABLE);
			menuStrDraw(2, i + (line - nodeItem), strIdx, 0, layer);
		}
		else if (menuProcCurrItemGet(layer) == item)
		{
			menuBarDisp(i + (line - nodeItem), layer);
			appOsdColorSet(MENU_STR_GET_FOCUS);
			menuStrDraw(2, i + (line - nodeItem), strIdx, 1, layer);
		}
		else
		{
			appOsdColorSet(MENU_STR_LOST_FOCUS);
			menuStrDraw(2, i + (line - nodeItem), strIdx, 0, layer);
		}
		menuProcButIconDisp(0xff,MENU_BAR_LAYER3_X_OFFSET,(i + (line - nodeItem)) * MENU_CHAR_HEIGHT+MENU_BAR_LAYER3_Y_OFFSET,\
			(sp1kOsdIconWidthGet(ID_ICON_MUDLGBAR)),sp1kOsdIconHeightGet(ID_ICON_MUDLGBAR),\
			 MENU_OBJ_ITEM0+i,ID_ICON_NONE,MENU_FUNC_SELITEM,i,SP1K_MSG_NULL,0\
			);

	}

	appOsdColorSet(MENU_STR_GET_FOCUS);
	appOsd_GfxStringIDDraw((LCD_WIDTH-appOsdLib_FontGetStringWidthbyId(idx))/2, (line-nodeItem)*(MENU_CHAR_HEIGHT+1), SP5K_GFX_ALIGN_TOP_LEFT, idx);

	menuPromptDisp(MENU_PROMPT_OK_OFF| MENU_PROMPT_MENU_ON);

	APP_OSD_REFRESH_ON;
}

#else
void menuRTCDisp(void)
{
	UINT8 layer = menuProcLayerGet();
	UINT8 item = menuProcCurrItemGet(layer);
/*	UINT8 icon = menuProcIconIDGet(layer, item);*/
	UINT8 line, col, i;
	UINT8 nodeItem, selItem, xpos=0, ypos=1;
	UINT32 strIdx;

	APP_OSD_REFRESH_OFF;
	CLEAR_OSD_SRC;
	CLEAR_PIP_SRC;

	menuLayerSizeGet(layer, &line, &col);

 	nodeItem = menuProcNodeItemGet(layer, item);

	for(i=0, item=0; i<nodeItem; i++,item++)
	{
		selItem = menuProcParaGet (layer, menuDateItemConv(item));
		menuProcChildStrGet(layer, menuDateItemConv(item), selItem, &strIdx);
		if (menuProcCurrItemGet(layer) == menuDateItemConv(item)) {
			appOsdColorSet(OSD_COLOR_YELLOW);
		} else {
			appOsdColorSet(MENU_STR_LOST_FOCUS);
		}
		
		appOsdLib_TextIDDraw((xpos+2)*MENU_CHAR_WIDTH, ypos*MENU_CHAR_HEIGHT+MENU_LAYER2_STR_Y_OFFSET, SP5K_GFX_ALIGN_TOP_LEFT,strIdx);
		xpos += appOsdLib_FontGetStringWidthbyId(strIdx) / 13;

		if ((item == 0) || (item == 1))
		{
			appOsdColorSet(MENU_STR_LOST_FOCUS);
			appOsdLib_TextDraw((xpos+2)*MENU_CHAR_WIDTH, ypos*MENU_CHAR_HEIGHT+MENU_LAYER2_STR_Y_OFFSET, SP5K_GFX_ALIGN_TOP_LEFT, "/");
			xpos += strlen("/");
		}
		else if (item == 3)
		{
			appOsdColorSet(MENU_STR_LOST_FOCUS);
			appOsdLib_TextDraw((xpos+2)*MENU_CHAR_WIDTH, ypos*MENU_CHAR_HEIGHT+MENU_LAYER2_STR_Y_OFFSET, SP5K_GFX_ALIGN_TOP_LEFT, ":");
			xpos += strlen(":");
		}
		else if (item == 4)
		{
			if(MENU_CHAR_WIDTH<=8)
				xpos = 2;
			else
				xpos = 3;
			
			ypos += 2;
		}
		else
			xpos++;
	}

	menuPromptDisp(MENU_PROMPT_OK_ON | MENU_PROMPT_MENU_ON);

	APP_OSD_REFRESH_ON;
}
void menuProcDisp2Layer(void)
{
	UINT8 layer = menuProcLayerGet();
	UINT8 item = menuProcCurrItemGet(layer);
	UINT16 icon = menuProcIconIDGet(layer, item);
	UINT8 line, col, i;
	UINT8 nodeItem, selItem, subItem;
	UINT32 strIdx;
	#if HDMI_MENU_BY_JPG
	UINT32 p[4];
	#endif
	
	APP_OSD_REFRESH_OFF;
	CLEAR_OSD_SRC;
	CLEAR_PIP_SRC;
	
	layer--;
	item = 0;
	menuLayerSizeGet(layer, &line, &col);

	menuBodyDisp(layer);
	menuHeaderDisp(layer);
	
	nodeItem = menuProcNodeItemGet(layer, item);
	nodeItem -= menuProcScrollGet(layer);

	if (nodeItem > line)
		nodeItem = line;
	item = menuProcScrollGet(layer);

	for(i=0; i<nodeItem; i++,item++)
	{
		menuProcStrGet(layer,item, &strIdx);
		icon = menuProcIconIDGet(layer, item);
		subItem = menuProcSubItemGet(layer, item);
		if (subItem > 0)
		{
			selItem = menuProcParaGet (layer, item);
			icon = menuProcChildIconIDGet(layer, item, selItem);
		}

		if (!menuProcIsActive(layer, item))
		{
			appOsdColorSet(MENU_STR_DISABLE);
			menuStrDraw(0, i, strIdx, 0, layer);
			menuIconDisp(col-2, i, icon, 0, layer);
		}
		else if (menuProcCurrItemGet(layer) == item)
		{
			menuBarDisp(i, layer);
			appOsdColorSet(MENU_STR_GET_FOCUS);
			menuStrDraw(0, i, strIdx, 1, layer);
			menuIconDisp(col-2, i, icon, 1, layer);
		}
		else
		{
			appOsdColorSet(MENU_STR_LOST_FOCUS);
			menuStrDraw(0, i, strIdx, 0, layer);
			menuIconDisp(col-2, i, icon, 0, layer);
		}
	}

	layer++;
	item = 0;
	menuLayerSizeGet(layer, &line, &col);

	menuBodyDisp(layer);
 	//menuHeaderDisp(layer);

	nodeItem = menuProcNodeItemGet(layer, item);
	nodeItem -= menuProcScrollGet(layer);

	if (nodeItem > line)
		nodeItem = line;

	item = menuProcScrollGet(layer);

	for(i=0; i<nodeItem; i++,item++)
	{
		menuProcStrGet(layer,item, &strIdx);
		subItem = menuProcSubItemGet(layer, item);
		if (!menuProcIsActive(layer, item))
		{
			appOsdColorSet(MENU_STR_DISABLE);
			menuStrDraw(0, i, strIdx, 0, layer);
			appOsdColorSet(MENU_STR_LOST_FOCUS);
			menuStrDisp(col-6, i, " ", 0, layer);
			appOsdColorSet(MENU_STR_DISABLE);
			if (subItem)
			{
				selItem = menuProcParaGet (layer, item);
				menuProcChildStrGet(layer, item, selItem, &strIdx);
				appOsd_GfxStringIDPIPDraw((col-2)*MENU_CHAR_WIDTH+(54-appOsdLib_FontGetStringWidthbyId(strIdx))/2, (i+2)*MENU_CHAR_HEIGHT-2, SP5K_GFX_ALIGN_TOP_LEFT, strIdx);
			}
		}
		else if (menuProcCurrItemGet(layer) == item)
		{
			appOsdColorSet(MENU_STR_LOST_FOCUS);
			#if KIT_WITH_HDMI   /*Add by Lation@20130717*/
			if(IS_HDMI_IN){
				#if HDMI_MENU_BY_JPG
				
				p[0] = menuSrcFileInfo[MENU_FOUR_LINES_BAR_HDMI].pos[0];
				p[1] = menuSrcFileInfo[MENU_FOUR_LINES_BAR_HDMI].pos[1]+i*145;
				p[2] = menuSrcFileInfo[MENU_FOUR_LINES_BAR_HDMI].pos[2];
				p[3] = menuSrcFileInfo[MENU_FOUR_LINES_BAR_HDMI].pos[3];
				
				sp5kGfxAttrSet(SP5K_GFX_PIP_REFRESH_ACTIVE, 0, 0, 0, 0);
				
				sp5kGfxAttrSet(SP5K_GFX_PEN_COLOR, 0x000000, 0, 0, 0);
				sp5kGfxAttrSet(SP5K_GFX_FILL_COLOR, 0x000000, 0, 0, 0);
				
				appOsd_GfxRectShapeDraw(HDMI_MENU_LAYER1, menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].pos[0], menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].pos[1],
					menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].pos[0]+menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].pos[2],menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].pos[1]+menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].pos[3]);
				
				sp5kRsvPreloadDraw(PRELOAD_FOUR_LINES_BAR, HDMI_MENU_LAYER1, p, 0);
				/*sp5kGfxAttrSet(SP5K_GFX_PIP_REFRESH_ACTIVE, 1, 0, 1, 0);*/
				#else
				appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0,MENU_BAR_LAYER2_X_OFFSET*2+3,(i*(MENU_CHAR_HEIGHT-2)+MENU_BAR_LAYER2_Y_OFFSET+6)*3>>1, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICONFILE_MUSUSBAR_HDMI, 0x0000);
				#endif	
			}
			else{
				appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0,MENU_BAR_LAYER2_X_OFFSET,(i*MENU_CHAR_HEIGHT+MENU_BAR_LAYER2_Y_OFFSET), SP5K_GFX_ALIGN_TOP_LEFT, ID_ICONFILE_MUSUSBAR, 0x0000);
			}
			#endif
			menuStrDraw(0, i, strIdx, 1, layer);
			if (subItem)
			{
				appOsdColorSet(MENU_TAB_GET_FOCUS);
				#if KIT_WITH_HDMI   /*Add by Lation@20130717*/
				if(IS_HDMI_IN){
					#if HDMI_MENU_BY_JPG
					
					
					sp5kGfxAttrSet(SP5K_GFX_PIP_REFRESH_ACTIVE, 0, 0, 0, 0);

					sp5kGfxAttrSet(SP5K_GFX_PEN_COLOR, 0x000000, 0, 0, 0);
					sp5kGfxAttrSet(SP5K_GFX_FILL_COLOR, 0x000000, 0, 0, 0);

					appOsd_GfxRectShapeDraw(HDMI_MENU_LAYER0, menuSrcFileInfo[MENU_ADJUST_LEFT_HDMI].pos[0], menuSrcFileInfo[MENU_ADJUST_DIALOG_HDMI].pos[1],
					menuSrcFileInfo[MENU_ADJUST_RIGHT_HDMI].pos[0]+menuSrcFileInfo[MENU_ADJUST_RIGHT_HDMI].pos[2],menuSrcFileInfo[MENU_ADJUST_DIALOG_HDMI].pos[1]+menuSrcFileInfo[MENU_ADJUST_DIALOG_HDMI].pos[3]+4*145);

					p[0] = menuSrcFileInfo[MENU_ADJUST_DIALOG_HDMI].pos[0];
					p[1] = menuSrcFileInfo[MENU_ADJUST_DIALOG_HDMI].pos[1]+i*145;
					p[2] = menuSrcFileInfo[MENU_ADJUST_DIALOG_HDMI].pos[2];
					p[3] = menuSrcFileInfo[MENU_ADJUST_DIALOG_HDMI].pos[3];
					
					sp5kResourceFilePagePlay(SP5K_RES_TYPE_JPG, 
			 			menuSrcFileInfo[MENU_ADJUST_DIALOG_HDMI].fileName, HDMI_MENU_LAYER0, p, 0);

					p[0] = menuSrcFileInfo[MENU_ADJUST_LEFT_HDMI].pos[0];
					p[1] = menuSrcFileInfo[MENU_ADJUST_LEFT_HDMI].pos[1]+i*145;
					p[2] = menuSrcFileInfo[MENU_ADJUST_LEFT_HDMI].pos[2];
					p[3] = menuSrcFileInfo[MENU_ADJUST_LEFT_HDMI].pos[3];
					
					sp5kResourceFilePagePlay(SP5K_RES_TYPE_JPG, 
			 			menuSrcFileInfo[MENU_ADJUST_LEFT_HDMI].fileName, HDMI_MENU_LAYER0, p, 0);

					p[0] = menuSrcFileInfo[MENU_ADJUST_RIGHT_HDMI].pos[0];
					p[1] = menuSrcFileInfo[MENU_ADJUST_RIGHT_HDMI].pos[1]+i*145;
					p[2] = menuSrcFileInfo[MENU_ADJUST_RIGHT_HDMI].pos[2];
					p[3] = menuSrcFileInfo[MENU_ADJUST_RIGHT_HDMI].pos[3];
					
					sp5kResourceFilePagePlay(SP5K_RES_TYPE_JPG, 
			 			menuSrcFileInfo[MENU_ADJUST_RIGHT_HDMI].fileName, HDMI_MENU_LAYER0, p, 0);

					/*sp5kGfxAttrSet(SP5K_GFX_PIP_REFRESH_ACTIVE, 1, 0, 1, 0);*/
					#else
					appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0, ((col-2)*MENU_CHAR_WIDTH -9)<<1, ((i+2)*(MENU_CHAR_HEIGHT-2)+5)*3>>1, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICONFILE_MU2LYBAR_HDMI, 0x0000);
					#endif
				}
				else{
					appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0, (col-2)*MENU_CHAR_WIDTH - 5, (i+2)*MENU_CHAR_HEIGHT-4, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICONFILE_MU2LYBAR, 0x0000);
				}
				#endif
				selItem = menuProcParaGet (layer, item);
				menuProcChildStrGet(layer, item, selItem, &strIdx);

				#if KIT_WITH_HDMI   /*Add by Lation@20130717*/
				if(IS_HDMI_IN){
					appOsd_GfxStringIDPIPDraw((col-2)*MENU_CHAR_WIDTH*2+(MENU_MU2BARSL_WIDTH-appOsdLib_FontGetStringWidthbyId(strIdx))/2, ((i+2)*(MENU_CHAR_HEIGHT-2))*3>>1, SP5K_GFX_ALIGN_TOP_LEFT, strIdx);
				}
				else{
					appOsd_GfxStringIDPIPDraw((col-2)*MENU_CHAR_WIDTH+(MENU_MU2BARSL_WIDTH-appOsdLib_FontGetStringWidthbyId(strIdx))/2, (i+2)*MENU_CHAR_HEIGHT-9, SP5K_GFX_ALIGN_TOP_LEFT, strIdx);
				}
				#endif
				
			}
		}
		else
		{
			appOsdColorSet(MENU_STR_LOST_FOCUS);
			menuStrDraw(0, i, strIdx, 0, layer);
			menuStrDisp(col-6, i, " ", 0, layer);
			if (subItem)
			{
				selItem = menuProcParaGet (layer, item);
				menuProcChildStrGet(layer, item, selItem, &strIdx);
				#if KIT_WITH_HDMI   /*Add by Lation@20130717*/
				if(IS_HDMI_IN){
					appOsd_GfxStringIDPIPDraw((col-2)*MENU_CHAR_WIDTH*2+(MENU_MU2BARSL_WIDTH-appOsdLib_FontGetStringWidthbyId(strIdx))/2, ((i+2)*(MENU_CHAR_HEIGHT-2))*3>>1, SP5K_GFX_ALIGN_TOP_LEFT, strIdx);
				}
				else{
					appOsd_GfxStringIDPIPDraw((col-2)*MENU_CHAR_WIDTH+(MENU_MU2BARSL_WIDTH-appOsdLib_FontGetStringWidthbyId(strIdx))/2, (i+2)*MENU_CHAR_HEIGHT-9, SP5K_GFX_ALIGN_TOP_LEFT, strIdx);
				}
				#endif
			}
		}
	}

	menuPromptDisp(MENU_PROMPT_OK_ON | MENU_PROMPT_MENU_ON);

	#if KIT_WITH_HDMI   /*Add by Lation@20130717*/
	if(IS_HDMI_IN)
	{
		#if HDMI_MENU_BY_JPG
		sp5kGfxAttrSet(SP5K_GFX_PIP_REFRESH_ACTIVE, 1, 0, 1, 0);
		#endif
	}
	#endif
	APP_OSD_REFRESH_ON;
}

void menuProcDispYesNo(UINT16 idx)
{
	UINT8 layer = menuProcLayerGet();
	UINT8 item = menuProcCurrItemGet(layer);
	UINT16 icon = menuProcIconIDGet(layer, item);
	UINT8 line, col, i;
	UINT8 nodeItem;
	UINT32 strIdx;
	
	APP_OSD_REFRESH_OFF;
	CLEAR_OSD_SRC;

	menuLayerSizeGet(layer, &line, &col);

	menuBodyDisp(3);

	nodeItem = menuProcNodeItemGet(layer, item);
	nodeItem -= menuProcScrollGet(layer);;

	if (nodeItem > line)
		nodeItem = line;
	item = menuProcScrollGet(layer);

	for(i=0; i<nodeItem; i++,item++)
	{
		menuProcStrGet(layer,item, &strIdx);
		icon = menuProcIconIDGet(layer, item);

		if (!menuProcIsActive(layer, item))
		{
			appOsdColorSet(MENU_STR_DISABLE);
			menuStrDraw(2, i + (line - nodeItem), strIdx, 0, layer);
		}
		else if (menuProcCurrItemGet(layer) == item)
		{
			menuBarDisp(i + (line - nodeItem), layer);
			appOsdColorSet(MENU_STR_GET_FOCUS);
			menuStrDraw(2, i + (line - nodeItem), strIdx, 1, layer);
		}
		else
		{
			appOsdColorSet(MENU_STR_LOST_FOCUS);
			menuStrDraw(2, i + (line - nodeItem), strIdx, 0, layer);
		}
	}

	appOsdColorSet(MENU_STR_GET_FOCUS);
	appOsd_GfxStringIDDraw((LCD_WIDTH-appOsdLib_FontGetStringWidthbyId((UINT32)idx))/2,  (line-nodeItem)*MENU_CHAR_HEIGHT, SP5K_GFX_ALIGN_TOP_LEFT, (UINT32)idx);
	menuPromptDisp(MENU_PROMPT_OK_ON | MENU_PROMPT_MENU_OFF);

	APP_OSD_REFRESH_ON;
}

#endif

#if HDMI_MENU_BY_JPG

UINT32 menuHDMIPIPInit(void)
{
	sp5kPipLayerCfg_t pipLayer;

	if(pipHDMIMenuFlag){
		return SUCCESS;
	}		
	RPrintf("HDMI menu ---  create PIP layer\n");
	sp5kPipTerm();
	appModeSet(SP5K_MODE_STANDBY);
	sp5kPipInitCfgSet(SP5K_PIP_INIT_MAIN_FRAME_SIZE, 1920, 1080, 0, 0);
	sp5kPipInit();
	sp5kGfxAttrSet(SP5K_GFX_PIP_REFRESH_ACTIVE, 0, 0, 0, 0);

	pipLayer.frmW = 1920;
	pipLayer.frmH = 1088;
	pipLayer.frmRoiW = 1920;
	pipLayer.frmRoiH = 1088;
	pipLayer.winX = 0;
	pipLayer.winY = 0;
	pipLayer.isShow = 0;
	pipLayer.alpha = 32;
	
	if (SUCCESS != sp5kPipLayerCreate(0, &pipLayer)) {
		printf("PIP layer 0 create err\n");
		return FAIL;
	}

	if (SUCCESS != sp5kPipLayerCreate(1, &pipLayer)) {
		printf("PIP layer 1 create err\n");
		return FAIL;
	}

	if (SUCCESS != sp5kPipLayerCreate(2, &pipLayer)) {
		printf("PIP layer 2 create err\n");
		return FAIL;
	}

	if (SUCCESS != sp5kPipLayerCreate(3, &pipLayer)) {
		printf("PIP layer 3 create err\n");
		return FAIL;
	}

	if (SUCCESS != sp5kPipLayerCreate(4, &pipLayer)) {
		printf("PIP layer 4 create err\n");
		return FAIL;
	}

	pipHDMIMenuFlag = 1;
	return SUCCESS;
}

void menuDlgShow(UINT32 num)
{
	UINT32 fileID = 0;

	switch(num){
		case 2:
			fileID = MENU_TWO_LINES_BODY_HDMI;
			break;
		case 3:
			fileID = MENU_THREE_LINES_BODY_HDMI;
			break;
		case 4:
			fileID = MENU_FOUR_LINES_BODY_HDMI;
			break;	
	}

	if(!dlgUpdateFlag){
		sp5kGfxAttrSet(SP5K_GFX_PEN_COLOR, 0x000000, 0, 0, 0);
		sp5kGfxAttrSet(SP5K_GFX_FILL_COLOR, 0x000000, 0, 0, 0);
	
		appOsd_GfxRectShapeDraw(HDMI_MENU_LAYER2, 0, 0,1920,1080);
	}
	dlgUpdateFlag = 1;
	
	sp5kResourceFilePagePlay(SP5K_RES_TYPE_JPG,menuSrcFileInfo[fileID].fileName,HDMI_MENU_LAYER2,
		menuSrcFileInfo[fileID].pos,0);

	sp5kGfxAttrSet(SP5K_GFX_PIP_LAYER_COLOR_KEY, HDMI_MENU_LAYER2, 1, 1, 0);
	sp5kGfxAttrSet(SP5K_GFX_PIP_LAYER_ACTIVE, HDMI_MENU_LAYER2, 1, 0, 0);
	sp5kGfxAttrSet(SP5K_GFX_PIP_REFRESH_ACTIVE, 1, 0, 1, 0);
}

void menuDlgBarShow(UINT32 dlgNum,UINT32 item)
{
	UINT32 p[4];
	UINT8* fileName = menuSrcFileInfo[MENU_THREE_LINES_BAR_HDMI].fileName;

	p[2] = menuSrcFileInfo[MENU_THREE_LINES_BAR_HDMI].pos[2];
	p[3] = menuSrcFileInfo[MENU_THREE_LINES_BAR_HDMI].pos[3];
	
	switch(dlgNum){
		case 2:
			if(item>1){
				break;
			}
			p[0] = 160;
			p[1] = 660 + item*146;
			break;
		case 3:
			if(item>2){
				break;
			}
			p[0] = 192;
			p[1] = 510 + item*146;
			break;
		case 4:
			if(item>3){
				break;
			}
			p[0] = 200;
			p[1] = 446 + item*146;
			break;
	}

	sp5kGfxAttrSet(SP5K_GFX_PEN_COLOR, 0x000000, 0, 0, 0);
	sp5kGfxAttrSet(SP5K_GFX_FILL_COLOR, 0x000000, 0, 0, 0);
	
	appOsd_GfxRectShapeDraw(HDMI_MENU_LAYER1, 0, 0,1920,1080);	
			
	sp5kResourceFilePagePlay(SP5K_RES_TYPE_JPG,fileName,HDMI_MENU_LAYER1,p,0);
	sp5kGfxAttrSet(SP5K_GFX_PIP_LAYER_ACTIVE, HDMI_MENU_LAYER1, 1, 0, 0);
	sp5kGfxAttrSet(SP5K_GFX_PIP_REFRESH_ACTIVE, 1, 0, 1, 0);
}

void menuHDMIClean(void)
{
	sp5kGfxAttrSet(SP5K_GFX_PIP_REFRESH_ACTIVE, 0, 0, 0, 0);
	sp5kGfxPageClear(HDMI_MENU_LAYER2, PAL_NOCOLOR);
	sp5kGfxPageClear(HDMI_MENU_LAYER1, PAL_NOCOLOR);
	sp5kGfxAttrSet(SP5K_GFX_PIP_REFRESH_ACTIVE, 1, 0, 1, 0);
}
#endif

#if CAM_TYPE_CVR  /*add for system state*/
void appSystemCardInfoShow(void)
{
	unsigned char totalSizeStr[20],freeSizeStr[20],cardType[20];
	UINT32 tempa,tempb;
	appDiskInfo_t* pDiskInfo;

	memset(totalSizeStr,0,20);
	memset(freeSizeStr,0,20);
	memset(cardType,0,20);

	pDiskInfo = appCurDiskInfoGet();

	tempa = (UINT32)((float)(pDiskInfo->dskMbSz)/(float)1024);
	tempb = (UINT32)((((float)(pDiskInfo->dskMbSz)/(float)1024)-tempa)*100);
	printf("size:%f\n",(float)(pDiskInfo->dskMbSz)/(float)1024);
	sprintf(totalSizeStr,"Size: %02d.%02d GB",tempa,tempb);
	printf("%s\n",totalSizeStr);

	tempa = (UINT32)((float)pDiskInfo->freeSz/(1024*1024));
	tempb = (UINT32)(((float)pDiskInfo->freeSz/(1024*1024)-tempa)*100);
	/*sprintf(freeSizeStr,"Free: %02d.%02d GB (%02d%%)",tempa,tempb,(100*pDiskInfo->freeSz)/(pDiskInfo->dskMbSz*1024));*/
	/* resolve 64G TF card freesize overflow */
	sprintf(freeSizeStr,"Free: %02d.%02d GB (%02d%%)",tempa,tempb,(10*pDiskInfo->freeSz)/(pDiskInfo->dskMbSz*102));
	printf("%s\n",freeSizeStr);

	if(pDiskInfo->actDsk == SP5K_DISK_NAND){
		sprintf(cardType,"Name: NAND");
	}else{ /* if(pDiskInfo->actDsk == SP5K_DISK_SD){*/
		sprintf(cardType,"Name: SD");
	}

	printf("type:%s\n",cardType);

	//appModeSet(SP5K_MODE_STANDBY);
	//appJpgFileDraw(SP5K_RES_TYPE_JPG,RES_CARD_INFO,0,0,320,240);

	appOsdLib_PIPLayerClearDraw();
	appOsdLib_OsdLayerClearDraw();
	appOsdLib_PFLayerShapeClear(APP_OSD_SHAPE_RECT, 0, 0, 320, 240, 0, 0);
	APP_OSD_REFRESH_ON;
	appOsd_ColorDrawShapeColorSet(PAL_BLACK, PAL_BLEND_100, PAL_BLACK, PAL_BLEND_100);
	appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, 0, 0,320, 240, 0, 0);

	appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_TextDraw(5,40+30,GFX_ALIGN_BOTTOM_LEFT,"CARD INFO");
	appOsdLib_TextDraw(10,LCD_SIZE_Y/2-35+30,GFX_ALIGN_BOTTOM_LEFT,cardType);
	appOsdLib_TextDraw(10,LCD_SIZE_Y/2-5+30,GFX_ALIGN_BOTTOM_LEFT,totalSizeStr);
	appOsdLib_TextDraw(10,LCD_SIZE_Y/2+25+30,GFX_ALIGN_BOTTOM_LEFT,freeSizeStr);
}


void appSystemMenuInit(void)
{
	uiPara_t* puiPara = appUiParaGet();
	UINT8 buf[128]={0};

	RPrintf("@@-----system  menu \n");
	switch(puiPara->SystemMode){
		case FACTORY_RESET:
			appOsdLib_PIPLayerClearDraw();
			appOsdLib_OsdLayerClearDraw();
			APP_OSD_REFRESH_ON;

			appPlayOsd_DlgBoxConfig(ID_STR_FACTORY_RESET, 2,strId_Reset, 0, 40);
			appPlayOsd_DlgBoxShow();
			appPbOsd_AuxNote_OKSelect(TRUE);
			break;
		case RESET_SYSTEM_INFO:

			appOsdLib_PIPLayerClearDraw();
			appOsdLib_OsdLayerClearDraw();
			appOsdLib_PFLayerShapeClear(APP_OSD_SHAPE_RECT, 0, 0, 320, 240, 0, 0);
			APP_OSD_REFRESH_ON;
			appOsd_ColorDrawShapeColorSet(PAL_BLACK, PAL_BLEND_100, PAL_BLACK, PAL_BLEND_100);
			appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, 0, 0,320, 240, 0, 0);

		//	appOsdLib_TextDraw((LCD_SIZE_X-sizeof(appVersionGet()))/2,LCD_SIZE_Y/2,GFX_ALIGN_BOTTOM_CENTER,appVersionGet());
			appOsdLib_TextDraw((LCD_SIZE_X-sizeof(appVersionGet()))/2,LCD_SIZE_Y/2,GFX_ALIGN_BOTTOM_CENTER,appVersionGet());
			break;
		case RESET_CARD_INFO:
			appSystemCardInfoShow();
			break;
		case GPS_GSVINFO:
			APP_OSD_REFRESH_OFF;
			GSENSORTimercount=0;
			appOsd_ColorDrawShapeColorSet(PAL_BLACK, PAL_BLEND_100, PAL_BLACK, PAL_BLEND_100);
			appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, 0, 0,320, 240, 0, 0);
	
			sprintf(buf,"total:%2d ",0);
			 
			appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
			appOsdLib_TextDraw(20, 40, SP5K_GFX_ALIGN_TOP_LEFT, buf);
			APP_OSD_REFRESH_ON;
			
			if(RTCTimer == TIMER_NULL){
				RTCTimer = appTimerSet(1000,"RTCTimer");
			}
			break;
	}
}

void appSystemStOnKey(UINT32 msg,UINT32 param)
{
	uiPara_t* puiPara = appUiParaGet();
	UINT32 mode ;
	sp5kModeGet(&mode);
	
	switch(msg){
		case APP_KEY_PRESS_DOWN:
		case APP_KEY_PRESS_UP:
			if(puiPara->SystemMode == FACTORY_RESET){
				appBeep(TRUE, FALSE);
				appPlayOsd_DlgBoxScroll(((msg==APP_KEY_PRESS_UP) ? TRUE : FALSE));
			}
			break;
		case APP_KEY_PRESS_SET:
			appBeep(TRUE, FALSE);
			if(puiPara->SystemMode == FACTORY_RESET){
				if(appPlayOsd_DlgBoxSelectGet() == 0){
					
					if(mode == SP5K_MODE_VIDEO_RECORD)
					{						
						menuProcVidRecAlwaysResetExcute();
						break;
					}
					else
					{
						_setupMenuFunctionResetAll();
					}
				}
				appOsdLib_OsdLayerClearDraw();
				//appPlayOsd_DlgBoxClear();
				appStateChange(APP_STATE_MENU,STATE_PARAM_NORMAL_INIT);
			}else{

				if(puiPara->SystemMode == GPS_GSVINFO)
				{
					appTimerClear(&RTCTimer);
					GSENSORTimercount=0;
				}
				appStateChange(APP_STATE_MENU,STATE_PARAM_NORMAL_INIT);
			}

			break;
	}
}

void appSystemStOnMsg(UINT32 msg,UINT32 param)
{
	uiPara_t* puiPara = appUiParaGet();
	
	switch ( msg) {
	    case SP5K_MSG_TIMER:
		 if(param == RTCTimer){
			GSENSORTimercount++;
			if(GSENSORTimercount==2)
			{
				GSENSORTimercount=0;
				#if GPS_SUPPORT
				appGpsGsvInfoUpdate();
				#endif
			}
		}
		break;
	default:
		break;
	}
}
void appSystemState(UINT32 msg, UINT32 param)
{
	switch(msg){
		case APP_STATE_MSG_INIT:
			appSystemMenuInit();
			break;
		case APP_STATE_MSG_CLOSE:
			CLEAR_OSD_SRC;
			appStateCloseDone();
			break;
		default:
			if (IS_APP_KEY_MSG(msg))
			{
				appSystemStOnKey(msg,param);
			}
			else 
			{
				appSystemStOnMsg(msg,param);
			}
			break;
	}
}
#endif

