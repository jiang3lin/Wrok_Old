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
#ifndef _APP_WIFI_H_
#define _APP_WIFI_H_
/*#if ICAT_WIFI*/
/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/
#ifdef ICAT_STREAMING
#include <api/sp5k_modesw_api.h>
#include <api/sp5k_aud_api.h>
#include <api/sp5k_media_api.h>
#include <api/sp5k_sensor_api.h>
#include <api/sp5k_capture_api.h>
#include <api/sp5k_dcf_api.h>
#include <app_state_ctrl.h>
#include <app_com_api.h>
#include <app_video.h>
#include <app_view_param.h>
#include <app_cdfs_api.h>
#endif
#include "app_ptp.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define BTN_LCK			0x40
#define WIFI_LOAD		0x01
#define DHCP_SERVER	0x02
#define HOST_AP			0x04
#define PTP_IP			0x08
#define RTP_STREM		0x10
#define FTP_SRV			0x20
#define DO_ASYNC			0x80
#define WIFI_INT (BTN_LCK|WIFI_LOAD|DHCP_SERVER|HOST_AP|PTP_IP|FTP_SRV|RTP_STREM)

#define MAX_SSID_NAME_LEN 	(20)
#define MAX_PASSWORD_LEN 	(63)
#define PTPIP_ALIVE_TIME 	(60)
#define RTP_ALIVE_TIME 		(60)

#define FIX_SENSOR_MODE_4_CAM_MODE (0)
#define SENSOR_MODE_CHANGE_WITH_VIDEOSIZE (1)
#define NDK_MP_TOOL_EN (0)

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef struct {
	UINT32 file_type;

	UINT32 h264_width;
	UINT32 h264_height;
	UINT32 h264_bitrate;
	UINT32 h264_frmrate;

	UINT32 jpeg_width;
	UINT32 jpeg_height;
	UINT32 jpeg_q_factor;
	UINT32 jpeg_bitrate;
} NetStParams;

struct H264ResolDef {
	const char *resol_str;
	UINT32 width, height;
	UINT32 bitrates[3]; /* bit-rate maps */
};

struct UrlAttrDef
{
	const char *name;
	int id;
};

enum {
	URL_H264_BITRATE,
	URL_H264_FRMRATE,

	URL_MJPG_WIDTH,
	URL_MJPG_HEIGHT,
	URL_MJPG_Q_FACTOR,
	URL_MJPG_BITRATE
};

enum {
	RTP_MJPG_Q_FINE = 45,
	RTP_MJPG_Q_NORMAL = 30,
	RTP_MJPG_Q_ECONOMY = 15,

	RTP_MJPG_BR_FINE = 8000000,
	RTP_MJPG_BR_NORMAL = 6000000,
	RTP_MJPG_BR_ECONOMY = 4000000
};

static struct UrlAttrDef	mjpgUrlAttrs[] =
{
	{"W",  URL_MJPG_WIDTH},
	{"H",  URL_MJPG_HEIGHT},
	{"Q",  URL_MJPG_Q_FACTOR},
	{"BR", URL_MJPG_BITRATE},
	{NULL, 0}
};

/**************************************************************************
 *                               M A C R O S                              *
 **************************************************************************/
#define MYINFO(args...)     do {printf("<<<%s:%d>>> ", __func__, __LINE__); printf(args); }while(0)
#define ALIGN_TO(x, n)      ( ((x)+(n-1)) & ~(n-1) )

/**************************************************************************
 *          M O D U L E   V A R I A B L E S   R E F E R E N C E S         *
 **************************************************************************/

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/
extern int	wdrv_init_csr();
extern void wdrv_exit_csr();

extern int	wdrv_init_mtk();
extern void wdrv_exit_mtk();

extern int	wdrv_init_rtk_v411();
extern void wdrv_exit_rtk_v411();

extern int	wdrv_init_rtk_v412();
extern void wdrv_exit_rtk_v412();

extern void appVideoStop(void);
static void appRtpResetMediaAttrs(void);
static void appDhcpServerStart(void);
static void appDhcpServerStop(void);
static void appHostAPStart(const char* cfg_file, const char* ssid_name, const char* passphrase,const char* channel_scan);
static void appHostAPStop(void);
static void appPtpIpStart(void);
static void appPtpIpStop(void);
static void appMediaSrvStop(void);
static void appStreamClose(void);
static void appNetDoInit(UINT32 parm);
static BOOL appSSIDNameGet(UINT8 *ssidname);
static BOOL appAPModeGet(UINT8 *hapMode);
static BOOL appUrlStrNoCaseCmp(const char *src, const char *dst, size_t len);
static BOOL appUrlGetNextElem(char **url_, char leading_char, char **name, int *len);
static BOOL appUrlParseAttrs(struct UrlAttrDef *urlAttrs, char *url, BOOL (*urlAttrHandler)(int id, char* val));
static BOOL appUrlMjpgAttrHandler(int id, char* val);
static BOOL appUrlParseJpegAttrs(char *url);
static BOOL appRtpEventHandler(UINT32 event, UINT32 data);
static BOOL appWiFiSuspend(void);
static BOOL appWiFiResume(void);
#ifdef ICAT_STREAMING
static void appMediaSrvStart(UINT32 w, UINT32 h);
#endif
static void appVideoSizeReset(NetStParams *stparam,UINT32 w,UINT32 h);
static void ptpipConnectcb(UINT32 sessnum);
void appRtpUpdateMediaAttrs(NetStParams *sp,UINT8 vidsize);
void appInitStateSet(UINT8 state);
void appInitStateUnset(UINT8 state);
void appRtpResetParams(NetStParams *sp,UINT8 vidsize);
void appRtpStartStreaming(void);
void appRtpStopStreaming(void);
void appRtpStreamStateSet(BOOL en);
void appInitStateSet(UINT8 state);
void appInitStateUnset(UINT8 state);
void appNetStateReset(UINT8 wifiParm);
BOOL appWiFiInit(BOOL bMptoolEn,const char *drvname);
UINT8 appInitStateGet(void);
UINT8 appInitStateGet(void);
int appNdkNetDrvInit(const char* drvname);
/* External using API */
void appWiFiStartConnection(UINT8 wifiParm);
void appWiFiStopConnection(UINT8 wifiParm);
void appPtpStateSet(BOOL en);
void appExceptionHandle(void);
void appNetDoExit(UINT8 parm);
void appNetSTAInfoGet(long mac,int expire,int expire_max);
void appMediaAttrUpdate(UINT8 vidsize);
BOOL appPtpStatGet(void);
BOOL appRtpStreamStateGet(void);
BOOL appIsStreamingActive(void);
UINT32 appPtpOpModeHandler(PTPDscOpMode_e opModeState, BOOL *isStopRec, BOOL *isStartRec);
UINT32 appPtpVideoHandler(NetStParams *sp, BOOL isStopRec, BOOL isStartRec);
UINT32 appNetSTANumGet();
BOOL appMacAddressGet(UINT8 *strMac);
/*#endif*/  /* ICAT_WIFI */
#endif  /* _APP_WIFI_H_ */


