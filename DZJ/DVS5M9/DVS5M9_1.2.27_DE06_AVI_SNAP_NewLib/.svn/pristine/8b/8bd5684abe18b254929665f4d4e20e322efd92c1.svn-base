/**************************************************************************
 *
 *       Copyright (c) 2012 by iCatch Technology, Inc.
 *
 *  This software is copyrighted by and is the property of iCatch Technology,
 *  Inc.. All rights are reserved by iCatch Technology, Inc..
 *  This software may only be used in accordance with the corresponding
 *  license agreement. Any unauthorized use, duplication, distribution,
 *  or disclosure of this software is expressly forbidden.
 *
 *  This Copyright notice MUST not be removed or modified without prior
 *  written consent of iCatch Technology, Inc..
 *
 *  iCatch Technology, Inc. reserves the right to modify this software
 *  without notice.
 *
 *  iCatch Technology, Inc.
 *  19-1, Innovation First Road, Science-Based Industrial Park,
 *  Hsin-Chu, Taiwan, R.O.C.
 *
 *  Author: Andy.Li
 *
 **************************************************************************/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ndk_global_api.h>
#include <ndk_getopt.h>
#include <ndk_streaming.h>
#include <ctype.h>
#include <api/cmd.h>
#include <sp5k_dbg_api.h>
#include <sp5k_global_api.h>
#include <api/sp5k_fs_api.h>
#include <proflog.h>
#include <app_timer.h> /* appTimerSet() */

#define MYINFO(args...)     do {printf("<<<%s:%d>>> ", __func__, __LINE__); printf(args); }while(0)
#define IAMHERE(args...)    printf("<<<%s:%d>>> I'm here\n", __func__, __LINE__)

extern int ndk_hapd_test(const char* cfg_file, const char* ssid_name, int cnt);

/*
 * Streaming
 */
#ifdef ICAT_STREAMING
static BOOL cmdPsDumpEnable = FALSE;

#include <ndk_streaming.h>
#include <api/sp5k_modesw_api.h>
#include <api/sp5k_aud_api.h>
#include <api/sp5k_media_api.h>
#include <api/sp5k_sensor_api.h>
#include <api/sp5k_capture_api.h>
#include <api/sp5k_dcf_api.h>
#include <app_state_ctrl.h>
#include <app_com_api.h>
#include <app_ptp.h>
#include <app_video.h>
#include <app_view_param.h>

#define ALIGN_TO(x, n)      ( ((x)+(n-1)) & ~(n-1) )

static BOOL _urlStrNoCaseCmp(const char *src, const char *dst, size_t len);
extern void dosSimDel(UINT8 *);
extern UINT32 tmrTimeClockGet();
/* test hapd start/stop */
extern int ndk_hapd_test(const char* cfg_file, const char* ssid_name, int cnt);
// PS-DUMP
extern unsigned _dbg_proc_ps_en(unsigned en);
extern void     _dbg_proc_ps_dump(unsigned n);
extern UINT32   osTaskInfoDisplay(UINT32 mode);
extern void     wdgOperSet(UINT32 mode, UINT32 interval);

typedef struct {
	BOOL   storage_on;
	BOOL   audio_on;
	UINT32 file_type;

	UINT32 h264_width;
	UINT32 h264_height;
	UINT32 h264_bitrate;
	UINT32 h264_frmrate;

	UINT32 jpeg_width;
	UINT32 jpeg_height;
	UINT32 jpeg_q_factor;
	UINT32 jpeg_bitrate;
} CmdRtpStreamParams;
static CmdRtpStreamParams st_params;

///////////////////////////////////////////////////////////////////////////////
// URL parsing
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

static struct H264ResolDef      h264ResolDefs[] = {
	{ "HD1080", 1920, 1080, {8000000, 6000000, 4000000}},
	{ "HD720",  1280, 720,  {4000000, 3000000, 2000000}},
	{ "XGA",    1024, 768,  {3000000, 2250000, 1500000}},
	{ "SVGA",   800,  600,  {1800000, 1350000, 9000000}},
	{ "VGA",    640,  480,  {1200000, 900000,  600000}},
	{ "QVGA",   320,  240,  {300000,  225000,  150000}}
};
#define VIDEO_RESOL_NR		NDK_ARRAY_SIZE(h264ResolDefs)
static int			h264FpsDefs[3] = {30, 30, 15};
static struct UrlAttrDef	h264UrlAttrs[] =
{
	{"BITRATE", URL_H264_BITRATE},
	{"FRMRATE", URL_H264_FRMRATE},
	{NULL, 0}
};

static struct UrlAttrDef	mjpgUrlAttrs[] =
{
	{"W",  URL_MJPG_WIDTH},
	{"H",  URL_MJPG_HEIGHT},
	{"Q",  URL_MJPG_Q_FACTOR},
	{"BR", URL_MJPG_BITRATE},
	{NULL, 0}
};

///////////////////////////////////////////////////////////////////////////////

static inline void _cmdSetAndWaitMode(UINT32 mode)
{
	sp5kModeSet(mode);
	sp5kModeWait(mode);
}

static void _cmdRtpUpdateMediaAttrs(CmdRtpStreamParams *sp)
{
	sp->jpeg_width = ALIGN_TO(sp->jpeg_width, 8);
	if (sp->jpeg_width > 640) {
		sp->jpeg_width = 640;
	}

	sp->jpeg_height = ALIGN_TO(sp->jpeg_height, 8);
	if (sp->jpeg_height > 360) {
		sp->jpeg_height = 360;
	}

	if (sp->jpeg_q_factor > RTP_MJPG_Q_FINE)
		sp->jpeg_q_factor = RTP_MJPG_Q_FINE;
	else if (sp->jpeg_q_factor < RTP_MJPG_Q_ECONOMY)
		sp->jpeg_q_factor = RTP_MJPG_Q_ECONOMY;

	if (sp->jpeg_bitrate > RTP_MJPG_BR_FINE)
		sp->jpeg_bitrate = RTP_MJPG_BR_FINE;
	else if (sp->jpeg_bitrate < RTP_MJPG_BR_ECONOMY)
		sp->jpeg_bitrate = RTP_MJPG_BR_ECONOMY;

	sp5kMediaRecCfgSet(SP5K_MEDIA_REC_RTP_STREAMING_EN, 0);
	sp5kMediaRecCfgSet(SP5K_MEDIA_REC_DISABLE_STORAGE, 0);

	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_STAMP_OPTION, 0);
	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_VIDEO_BRC_TYPE, SP5K_MEDIA_CBR);
	sp5kMediaRecCfgSet(SP5K_MEDIA_REC_MUTE_PERIOD, 100);
	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_FILE_TYPE, sp->file_type);
	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_AUDIO_CODEC, SP5K_MEDIA_AUDIO_PCM);
	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_AUDIO_SAMPLE_RATE, 44100);
	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_AUDIO_SAMPLE_BITS, 16);
	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_AUDIO_CHANNELS, 2);
	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_AUDIO_ALC_MAX_VOL, 31);
	sp5kMediaRecCfgSet(SP5K_MEDIA_REC_ALC_MUTE, 0);
	sp5kMediaRecCfgSet(SP5K_MEDIA_REC_ALC, SP5K_MEDIA_REC_OFF);
	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_AUDIO_ALC_HB, 500);
	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_AUDIO_ALC_LB, 100);
	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_SEAMLESS_TIME_SLOT, 0);
	sp5kMediaRecCfgSet(SP5K_MEDIA_REC_ALC, SP5K_MEDIA_REC_OFF);
	sp5kMediaRecCfgSet(SP5K_MEDIA_REC_ALC_MODE, SP5K_MEDIA_REC_ALC_DRC_MODE);
	sp5kMediaRecCfgSet(SP5K_MEDIA_REC_ALC, SP5K_MEDIA_REC_ON);

	MYINFO("Mode = %d\n", ndk_st_get_mode());
	if (ndk_st_get_mode() == NDK_ST_MONO_JPEG) {
		sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_WIDTH, sp->jpeg_width);
		sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_HEIGHT, sp->jpeg_height);
		sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_MJPG_INITIAL_Q_FACTOR, sp->jpeg_q_factor);
		sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_VIDEO_AVG_BITRATE, sp->jpeg_bitrate);
		sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_VIDEO_FRAME_RATE, 30);/*30*/
		sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_VIDEO_CODEC, SP5K_MEDIA_VIDEO_MJPG);

		sp5kSensorModeCfgSet(SP5K_MODE_VIDEO_PREVIEW, SENSOR_MODE_1080P);
	}
	else {
		sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_WIDTH, sp->h264_width);
		sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_HEIGHT, sp->h264_height);
		sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_VIDEO_AVG_BITRATE, sp->h264_bitrate);
		sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_VIDEO_FRAME_RATE, sp->h264_frmrate);
		sp5kMediaRecAttrSet(MEDIA_ATTR_H264_GOP_STRUCTURE, 0x10);
		sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_VIDEO_CODEC, SP5K_MEDIA_VIDEO_H264);

		/*if (sp->h264_width *sp->h264_height == 1920 * 1080) {
			sp5kSensorModeCfgSet(SP5K_MODE_VIDEO_PREVIEW, SP5K_SENSOR_MODE_PREVIEW + 1);
		}
		else */{
			sp5kSensorModeCfgSet(SP5K_MODE_VIDEO_PREVIEW, SENSOR_MODE_1080P);
		}
	}

	if (0) {
		UINT32 w, h;
		sp5kMediaRecAttrGet(SP5K_MEDIA_ATTR_WIDTH, &w);
		sp5kMediaRecAttrGet(SP5K_MEDIA_ATTR_HEIGHT, &h);
		MYINFO("width = %d, height = %d\n", w, h);
	}

	if (!sp->storage_on) {
		sp5kMediaRecCfgSet(SP5K_MEDIA_REC_DISABLE_STORAGE, 1);
	}

	/* Only after the RTP Muxer is enabled, the liveStreaming can get stream data from encoder. */
	if (ndk_st_get_mode() != NDK_ST_DUAL_STREAMING) {
		sp5kMediaRecCfgSet(SP5K_MEDIA_REC_RTP_STREAMING_EN, 1);
	}
}

static void _cmdRtpStartStreaming()
{
	CmdRtpStreamParams *sp = &st_params;

	UINT32 curMode;
	sp5kModeGet(&curMode);

	if (ndk_st_is_dual_streaming()) {
		if (curMode != SP5K_MODE_VIDEO_RECORD && curMode != SP5K_MODE_VIDEO_PREVIEW) {
			// enter video preview
			_cmdRtpUpdateMediaAttrs(sp);
			_cmdSetAndWaitMode(SP5K_MODE_VIDEO_PREVIEW);
		}

		if (ndk_st_dualstream_start(sp->jpeg_width, sp->jpeg_height, sp->jpeg_q_factor, 1) != 0)
			MYINFO("error dualstream");
	}
	else {
		if (curMode != SP5K_MODE_VIDEO_PREVIEW) {
			_cmdRtpUpdateMediaAttrs(sp);
			_cmdSetAndWaitMode(SP5K_MODE_VIDEO_PREVIEW);
			_cmdRtpUpdateMediaAttrs(sp);
		}

		_cmdSetAndWaitMode(SP5K_MODE_VIDEO_RECORD);
	}
	MYINFO("start record\n");

	if (cmdPsDumpEnable) {
		printf("\nPS DUMP begin\n");
		wdgOperSet(0, 0);
		_dbg_proc_ps_en(1);
	}
}

static void _cmdRtpStopStreaming()
{
	if (cmdPsDumpEnable) {
		osTaskInfoDisplay(1);
	}

	if (ndk_st_is_dual_streaming()) {
		if (ndk_st_dualstream_stop())
			MYINFO("error dualstream");
	}
	else {
		UINT32 curMode;
		sp5kModeGet(&curMode);
		if (curMode == SP5K_MODE_VIDEO_RECORD)
			_cmdSetAndWaitMode(SP5K_MODE_VIDEO_PREVIEW);
	}

	if (cmdPsDumpEnable) {
		_dbg_proc_ps_en(0);
		_dbg_proc_ps_dump(0);
	}
	MYINFO("stop record\n");
}

static void _cmdRtpChangeMode(NDKStMode newmode)
{
	if (ndk_st_get_mode() == newmode)
		return;

	UINT32 t = tmrTimeClockGet();

	if (ndk_st_streaming_is_started())
		_cmdRtpStopStreaming();

	if (ndk_st_change_mode(newmode, NULL) < 0) {
		printf("Change mode failed\n");
		return;
	}

	if (ndk_st_streaming_is_started())
		_cmdRtpStartStreaming();

	t = tmrTimeClockGet() - t;
	printf("time spent: %ums\n", t/1000);
}

static void _cmdRtpSwitchMode(NDKStMode newmode, int cnt)
{
	NDKStMode mode0 = newmode;
	NDKStMode mode1 = ndk_st_get_mode();

	if (mode0 == mode1 || cnt == 0)
		return;

	int i;
	UINT32 t = tmrTimeClockGet();
	BOOL st_strted = ndk_st_streaming_is_started();

	for (i = 0; i < cnt; ++i) {
		if (st_strted)
			_cmdRtpStopStreaming();

		if (ndk_st_change_mode(i & 0x01 ? mode1 : mode0, NULL) < 0) {
			printf("Change mode failed\n");
			return;
		}

		if (st_strted)
			_cmdRtpStartStreaming();
	}
	t = tmrTimeClockGet() - t;

	printf("switch mode: cnt = %d, total time spent = %ums\n", cnt, t/1000);
}

static struct H264ResolDef *_urlGetResolDef(const char *resol_str, int len) {
	unsigned i;
	for (i = 0; i < NDK_ARRAY_SIZE(h264ResolDefs); ++i) {
		if (_urlStrNoCaseCmp(h264ResolDefs[i].resol_str, resol_str, len)) {
			return &h264ResolDefs[i];
		}
	}
	return NULL;
}

static BOOL _urlStrNoCaseCmp(const char *src, const char *dst, size_t len)
{
	while (*src && *dst && len > 0) {
		if (toupper(*src) != toupper(*dst)) {
			return FALSE;
		}

		++src;
		++dst;
		--len;
	}

	return len == 0 ? 1 : 0;
}

/* H264: /VGA?bitrate=high&frmrate=high
   H264: /VGA?bitrate=high&frmrate=high
   MJPG: ?W=320&H=160&Q=85 */
static BOOL _urlGetNextElem(char **url_, char leading_char, char **name, int *len)
{
	char *url = *url_;
	if (*url == 0) {
		return FALSE;
	}

	if ((leading_char && *url != leading_char) || !strchr("/?=&", *url)) {
		return FALSE;
	}

	char *p = url + 1;
	while (*p) {
		if (strchr("/?=&", *p)) {
			break;
		}
		++p;
	}
	*name = url + 1;
	*len = (int)(p - url - 1);
	*url_ = p;

	return TRUE;
}

static BOOL _urlParseAttrs(struct UrlAttrDef *urlAttrs, char *url
	, BOOL (*urlAttrHandler)(int id, char* val))
{
	if (!url || *url == 0)
		return TRUE;

	if (*url != '?') // must start with ?
		return FALSE;

	struct UrlAttrDef *pAttr;
	char *str;
	int  len;

	while (1) {
		if (!_urlGetNextElem(&url, 0, &str, &len))
			return FALSE;

		for (pAttr = urlAttrs; pAttr->name != NULL; ++pAttr) {
			if (_urlStrNoCaseCmp(pAttr->name, str, len)) {
				if (!_urlGetNextElem(&url, '=', &str, &len))
					return FALSE;
				char c = str[len];
				str[len] = 0;
				BOOL r = urlAttrHandler(pAttr->id, str);
				str[len] = c;
				if (!r)
					return FALSE;
				break;
			}
		}

		if (pAttr->name == NULL) // Unknow attributes
			return FALSE;

		if (*url == 0) // finished
			break;

		if (*url != '&') // name=value pair must start with character '&' except the first one.
			return FALSE;
	}

	return TRUE;
}

static BOOL _urlH264AttrHandler(int id, char* val)
{
	CmdRtpStreamParams *sp = &st_params;

	switch (id) {
	case URL_H264_BITRATE:
		if (!strcasecmp(val, "HIGH"))
			sp->h264_bitrate = 0;
		else if (!strcasecmp(val, "NORMAL"))
			sp->h264_bitrate = 1;
		else if (!strcasecmp(val, "LOW"))
			sp->h264_bitrate = 2;
		else
			return FALSE;
		break;
	case URL_H264_FRMRATE:
		if (!strcasecmp(val, "HIGH"))
			sp->h264_frmrate = 0;
		else if (!strcasecmp(val, "NORMAL"))
			sp->h264_frmrate = 1;
		else if (!strcasecmp(val, "LOW"))
			sp->h264_frmrate = 2;
		else
			return FALSE;
		break;
	default:
		return FALSE;
	}

	return TRUE;
}

/* HD1080?BITRATE=HIGH&FRMRATE=HIGH
   Return 0 to abort streaming prcess */
static BOOL _urlParseH264Attrs(char *url)
{
	CmdRtpStreamParams params_bak;
	memcpy(&params_bak, &st_params, sizeof(params_bak));

	struct H264ResolDef *resoldef;
	char *name;
	int  len;

	if (*url == 0) {
		return TRUE;
	}

	if (!_urlGetNextElem(&url, '/', &name, &len)) {
		return FALSE;
	}

	resoldef = _urlGetResolDef(name, len);
	if (!resoldef) {
		return FALSE;
	}

	if (!_urlParseAttrs(h264UrlAttrs, url, _urlH264AttrHandler))
		return FALSE;

	st_params.h264_width = resoldef->width;
	st_params.h264_height = resoldef->height;

	HOST_ASSERT(st_params.h264_bitrate >= 0 && st_params.h264_bitrate <= 2);
	st_params.h264_bitrate = resoldef->bitrates[st_params.h264_bitrate];

	HOST_ASSERT(st_params.h264_frmrate >= 0 && st_params.h264_frmrate <= 2);
	st_params.h264_frmrate = h264FpsDefs[st_params.h264_frmrate];

	if (memcmp(&st_params, &params_bak, sizeof(params_bak)) != 0) {
#if 0
		_cmdRtpStopStreaming();
		_cmdRtpStartStreaming();
#endif
	}

	return TRUE;
}

static BOOL _urlMjpgAttrHandler(int id, char* val)
{
	CmdRtpStreamParams *sp = &st_params;

	switch (id) {
	case URL_MJPG_WIDTH:
		sp->jpeg_width = strtoul(val, NULL, 10);
		break;

	case URL_MJPG_HEIGHT:
		sp->jpeg_height = strtoul(val, NULL, 10);
		break;

	case URL_MJPG_Q_FACTOR:
		if (!strcasecmp(val, "FINE"))
			sp->jpeg_q_factor = RTP_MJPG_Q_FINE;
		else if (!strcasecmp(val, "NORMAL"))
			sp->jpeg_q_factor = RTP_MJPG_Q_NORMAL;
		else if (!strcasecmp(val, "ECONOMY"))
			sp->jpeg_q_factor = RTP_MJPG_Q_ECONOMY;
		else
			sp->jpeg_q_factor = strtoul(val, NULL, 10);
		break;
	case URL_MJPG_BITRATE:
		if (!strcasecmp(val, "FINE"))
			sp->jpeg_bitrate = RTP_MJPG_BR_FINE;
		else if (!strcasecmp(val, "NORMAL"))
			sp->jpeg_bitrate = RTP_MJPG_BR_NORMAL;
		else if (!strcasecmp(val, "ECONOMY"))
			sp->jpeg_bitrate = RTP_MJPG_BR_ECONOMY;
		else
			sp->jpeg_bitrate = strtoul(val, NULL, 10);
		break;
	default:
		return FALSE;
	}

	return TRUE;
}

static BOOL _urlParseJpegAttrs(char *url)
{
	HOST_ASSERT(url);

	CmdRtpStreamParams params_bak;
	memcpy(&params_bak, &st_params, sizeof(params_bak));

	if (!_urlParseAttrs(mjpgUrlAttrs, url, _urlMjpgAttrHandler))
		return FALSE;

	if (memcmp(&st_params, &params_bak, sizeof(params_bak)) != 0) {
		// changed
	}

	return TRUE;
}

static BOOL _cmdRtpEventHandler(UINT32 event, UINT32 data)
{
	switch (event) {
	case NDK_ST_EVT_RTSP_REQUEST: {
		char *url = (char *)data;
		MYINFO("URL = '%s'\n", url);

		if (_urlStrNoCaseCmp(url, "H264", 4) &&
		    (ndk_st_get_mode() == NDK_ST_DUAL_H264JPEG ||
		     ndk_st_get_mode() == NDK_ST_MONO_H264)
		   )
		{
			return _urlParseH264Attrs(url + 4);
		}
		else if (_urlStrNoCaseCmp(url, "MJPG", 4) && ndk_st_get_mode() != NDK_ST_MONO_H264) {
			return _urlParseJpegAttrs(url + 4);
		}
		else if (_urlStrNoCaseCmp(url, "FILE", 4)) {
			return TRUE;
		}
		else if (_urlStrNoCaseCmp(url, "PCM", 3)) {
			return TRUE;
		}
		else if (_urlStrNoCaseCmp(url, "H264PCM", 7)) {
			return TRUE;
		}
		else {
			MYINFO("Illegal URL\n");
			return FALSE;
		}
		break; }

	case NDK_ST_EVT_ON_STOPPED: {
		_cmdRtpStopStreaming();
		return TRUE; }

	case NDK_ST_EVT_ON_STARTED: {
		_cmdRtpStartStreaming();
		return TRUE; }

	case NDK_ST_EVT_FRAME_DROPPED: {
		return TRUE; }
	}

	return FALSE;
}

static void _netcmd_mediasrv(int argc, char **argv)
{
	extern void cmdProcess(char *);	cmdProcess((char*)"os nice Cmd 31");
	extern void sioCtrl(UINT32); sioCtrl(3);

	CmdRtpStreamParams *sp = &st_params;
	NDKStMode stmode = NDK_ST_DUAL_STREAMING;
	const char *root_dir = "B:/UDF";
	unsigned keepalive_secs = 30;
	int i;

	sp->storage_on    = TRUE;
	sp->audio_on      = FALSE;
	sp->file_type     = SP5K_MEDIA_MOV;

	sp->h264_width    = 1920;
	sp->h264_height   = 1080;
	sp->h264_bitrate  = 6000000;
	sp->h264_frmrate  = 30;

	sp->jpeg_width    = 640;
	sp->jpeg_height   = 360;
	sp->jpeg_q_factor = 40;
	sp->jpeg_bitrate  = 4500000;

	for (i = 0; i < argc; ++i) {
		if (!strcmp(argv[i], "-h")) {
			goto lUSAGE;
		}
		else if (!strcmp(argv[i], "-x")) {
			ndk_st_stop_server();
			return;
		}
		else if (!strcmp(argv[i], "-dhj") || !strcmp(argv[i], "-djh")) {
			stmode = NDK_ST_DUAL_H264JPEG;
			sp->jpeg_height = 360;
		}
		else if (!strcmp(argv[i], "-dj")) {
			stmode = NDK_ST_DUAL_STREAMING;
			sp->jpeg_height = 360;
		}
		else if (!strcmp(argv[i], "-mh")) {
			stmode = NDK_ST_MONO_H264;
		}
		else if (!strcmp(argv[i], "-mj")) {
			stmode = NDK_ST_MONO_JPEG;
		}
		else if (!strcmp(argv[i], "-nf")) {
			sp->storage_on = FALSE;
		}
		else if (!strcmp(argv[i], "-audio")) {
			sp->audio_on = TRUE;
		}
		else if (!strcmp(argv[i], "-avi")) {
			sp->file_type = SP5K_MEDIA_AVI;
		}
		else if (!strcmp(argv[i], "-mov")) {
			sp->file_type = SP5K_MEDIA_MOV;
		}
		else if (!strcmp(argv[i], "-psdump")) {
			cmdPsDumpEnable = TRUE;
		}
		// The following options need argument
		else if ( (i + 1) >= argc ) {
			goto lUSAGE;
		}
		else if (!strcmp(argv[i], "-r")) {
			root_dir = argv[++i];
		}
		else if (!strcmp(argv[i], "-hw")) {
			sp->h264_width = atoi(argv[++i]);
		}
		else if (!strcmp(argv[i], "-hh")) {
			sp->h264_height = atoi(argv[++i]);
		}
		else if (!strcmp(argv[i], "-hb")) {
			sp->h264_bitrate = atoi(argv[++i]);
		}
		else if (!strcmp(argv[i], "-hf")) {
			sp->h264_frmrate = atoi(argv[++i]);
		}
		else if (!strcmp(argv[i], "-jw")) {
			sp->jpeg_width = atoi(argv[++i]);
		}
		else if (!strcmp(argv[i], "-jh")) {
			sp->jpeg_height = atoi(argv[++i]);
		}
		else if (!strcmp(argv[i], "-jq")) {
			sp->jpeg_q_factor = atoi(argv[++i]);
		}
		else if (!strcmp(argv[i], "-jb")) {
			sp->jpeg_bitrate = atoi(argv[++i]);
		}
		else if (!strcmp(argv[i], "-ka")) {
			keepalive_secs = atoi(argv[++i]);
		}
		else {
			goto lUSAGE;
		}
	}

	if (stmode == NDK_ST_DUAL_STREAMING && !sp->storage_on) {
		printf("Error: NDK_ST_DUAL_STREAMING is set but mux to file is disabled.\n");
		return;
	}

	struct NDKStCfg cfg = {
		.root_dir = root_dir,
		.port = 554,
		.st_mode = stmode,
		.audio_on = sp->audio_on,
		.keepalive_secs = keepalive_secs,
		.evt_handler = _cmdRtpEventHandler
	};

	if (ndk_st_start_server(&cfg) != 0) {
		MYINFO("start server failed");
	}
	return;

lUSAGE:
	printf("mediasrv [options]\n");
	printf("  -h     : Show help\n");
	printf("  -x     : Stop media server\n");
	printf("  -r dir : Set root direcotyr for media server\n");
	printf("  -dhj   : Dual Streaming.\n");
	printf("  -dj    : Dual Streaming but only stream MJPEG videos.\n");
	printf("  -mh    : Mono Streaming with H264 encoding.\n");
	printf("  -mj    : Mono Streaming with JPEG encoding.\n");
	printf("  -nf    : Disable file storage. Cannot coexist with -dj\n");
	printf("  -hw n  : H264 width.\n");
	printf("  -hh n  : H264 height.\n");
	printf("  -hb n  : H264 bit-rate.\n");
	printf("  -hf n  : H264 frame-rate.\n");
	printf("  -jw n  : JPEG width.\n");
	printf("  -jh n  : JPEG height.\n");
	printf("  -jq n  : JPEG Q factor.\n");
	printf("  -jb n  : JPEG bit-rate.\n");
}

extern UINT32 mediaRecDualStreamJfifControl(mediaRecDualStreamCtrlCode_e code);

static void _netcmd_mediactl(int argc, char **argv)
{
	CmdRtpStreamParams *sp = &st_params;

	if (argc == 0)
		goto lUSAGE;

	char *subcmd = argv[0];
	--argc;
	++argv;

	if (!strcmp(subcmd, "-h")) {
		goto lUSAGE;
	}
	else if (!strcmp(subcmd, "rec")) {
		if (!ndk_st_is_dual_streaming()) {
			printf("Only support dual-streaming\n");
			return;
		}

		UINT32 mode;
		sp5kModeGet(&mode);

		if (argc == 0 || (argc == 1 && !strcmp(argv[0], "+"))) {
			if (mode != SP5K_MODE_VIDEO_RECORD) {
				if (mode != SP5K_MODE_VIDEO_PREVIEW) {
					_cmdRtpUpdateMediaAttrs(sp);
					_cmdSetAndWaitMode(SP5K_MODE_VIDEO_PREVIEW);
				}

				_cmdSetAndWaitMode(SP5K_MODE_VIDEO_RECORD);
			}
		}
		else if (argc == 1 && !strcmp(argv[0], "-")) {
			if (mode == SP5K_MODE_VIDEO_RECORD) {
				//ndk_st_dualstream_stop();
				_cmdRtpUpdateMediaAttrs(sp);
				_cmdSetAndWaitMode(SP5K_MODE_VIDEO_PREVIEW);
				//sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 100);
				//ndk_st_dualstream_start(sp->jpeg_width, sp->jpeg_height, sp->jpeg_q_factor);
			}
		}
		else if (argc >= 1 && !strcmp(argv[0], "sw")) {
			int i, sleep = 0, n = 1;
			if (argc >= 2)
				n = atoi(argv[1]);
			if (argc >= 3)
				sleep = atoi(argv[2]);

			// preview <-> record switch
			_cmdRtpUpdateMediaAttrs(sp);
			if (mode == SP5K_MODE_VIDEO_PREVIEW) {
				for (i = 0; i < n; ++i) {
					_cmdSetAndWaitMode(SP5K_MODE_VIDEO_RECORD);
					if (sleep > 0)
						sp5kTimeDelay(SP5K_TIME_DELAY_1MS, sleep);
					_cmdSetAndWaitMode(SP5K_MODE_VIDEO_PREVIEW);
				}
			}
			// record <-> preview switch
			else if (mode == SP5K_MODE_VIDEO_RECORD) {
				for (i = 0; i < n; ++i) {
					_cmdSetAndWaitMode(SP5K_MODE_VIDEO_PREVIEW);
					if (sleep > 0)
						sp5kTimeDelay(SP5K_TIME_DELAY_1MS, sleep);
					_cmdSetAndWaitMode(SP5K_MODE_VIDEO_RECORD);
				}
			}
			else {
				printf("mode error\n");
			}
		}
		else
			goto lUSAGE;
	}
	else if (!strcmp(subcmd, "snap")) {
		UINT32 t0, t1, t;
		int i, cnt = 1;

		if (argc >= 1)
			cnt = atoi(argv[0]);

		t0 = t1 = 0;
		for (i = 0; i < cnt; ++i) {
			t = tmrTimeClockGet()/1000;
			_cmdRtpStopStreaming();
			t0 += tmrTimeClockGet()/1000 - t;

			t = tmrTimeClockGet()/1000;
			_cmdRtpStartStreaming();
			t1 += tmrTimeClockGet()/1000 - t;
		}

		printf("snap test: cnt=%d, stop-time=(%u, %u)ms, start-time=(%u, %u)ms\n"
			, cnt, t0, t0/cnt, t1, t1/cnt);
		return;
	}
	else if (!strcmp(subcmd, "j")) {
		if (argc != 4)
			goto lUSAGE;

		sp->jpeg_width = atoi(argv[0]);
		sp->jpeg_height = atoi(argv[1]);
		sp->jpeg_bitrate = atoi(argv[2]);
		sp->jpeg_q_factor = atoi(argv[3]);

		UINT32 t = tmrTimeClockGet()/1000;

		_cmdRtpStopStreaming();
		_cmdRtpStartStreaming();

		t = tmrTimeClockGet()/1000 - t;
		printf("time spent = %ums\n", t);
	}
	else if (!strcmp(subcmd, "h")) {
		if (argc != 3)
			goto lUSAGE;

		sp->h264_width = atoi(argv[0]);
		sp->h264_height = atoi(argv[1]);
		sp->h264_bitrate = atoi(argv[2]);

		UINT32 t = tmrTimeClockGet()/1000;

		_cmdRtpStopStreaming();
		_cmdRtpStartStreaming();

		t = tmrTimeClockGet()/1000 - t;
		printf("time spent = %ums\n", t);
	}
	else if (!strcmp(subcmd, "mon")) {
		extern void ndk_st_start_monitor();
		ndk_st_start_monitor();
	}
	else if (!strcmp(subcmd, "close")) {
		ndk_st_close_streaming();
	}
	else if (!strcmp(subcmd, "sm")) {
		int cnt = 1;

		if (argc >= 1)
			cnt = atoi(argv[0]);

		if (ndk_st_is_dual_streaming())
			_cmdRtpSwitchMode(NDK_ST_MONO_JPEG, cnt);
		else
			_cmdRtpSwitchMode(NDK_ST_DUAL_H264JPEG, cnt);
	}
	else if (!strcmp(subcmd, "cm")) {
		if (argc != 1)
			goto lUSAGE;

		NDKStMode mode;

		if (!strcmp(argv[0], "dj")) {
			mode = NDK_ST_DUAL_STREAMING;
		}
		else if (!strcmp(argv[0], "dhj") || !strcmp(argv[0], "djh")) {
			mode = NDK_ST_DUAL_H264JPEG;
		}
		else if (!strcmp(argv[0], "mh")) {
			mode = NDK_ST_MONO_H264;
		}
		else if (!strcmp(argv[0], "mj")) {
			mode = NDK_ST_MONO_JPEG;
		}
		else {
			printf("Wrong mode\n");
			return;
		}

		_cmdRtpChangeMode(mode);
	}
	else {
		goto lUSAGE;
	}

	return;

lUSAGE:
	printf("mc h w h br      : change h264 settings\n");
	printf("mc j w h br q    : change jpeg settings\n");
	printf("mc close         : close streaming\n");
	printf("mc mon           : monitor streaming\n");
	printf("mc cm [dj|mh|mj] : change mode\n");
	printf("mc sm]           : dual <==> mono\n");
	printf("mc rec +/-       : start/stop record in dual-streaming mode\n");
	printf("mc rec sw [nr] [sleep-time] : switch test\n");
}

static void _netcmd_mediadump(int argc, char *argv[])
{
	extern void mediaRecAttrDump();
	extern void mediaRecCfgDump();

	printf("rec attrs: \n");
	mediaRecAttrDump();

	printf("\nrec cfgs: \n");
	mediaRecCfgDump();
}
#endif // ICAT_STREAMING

static void _cmdNetSysEventHandler(NDKSysEvt evt, unsigned long param, unsigned long udata)
{
	switch (evt) {
	case NDK_SYSEVT_HAPD_ASSOC:
		MYINFO("ASSOC: %s\n", eth_addr_ntoa((unsigned char*)param));
		break;

	case NDK_SYSEVT_HAPD_DISASSOC:
		MYINFO("DIS-ASSOC: %s\n", eth_addr_ntoa((unsigned char*)param));
		break;

	case NDK_SYSEVT_DHCPD_ALLOC_ADDR:
		MYINFO("dhcpd alloc: %s\n", ipaddr_ntoa((ip_addr_t*)param));
		break;

	default:
		MYINFO("event %u, %lu\n", evt, param);
		break;
	}
}

/*
 * Network commands
 */
static void _netcmd_sysinfo(int argc, char *argv[])
{
	printf("NDK Release Version: %s\n", ndk_get_version());
}

static void _netcmd_load(int argc, char *argv[])
{
	const char *opt_ipaddr = "192.168.123.101";
	const char *opt_drvname = NULL;

	int c;
	ndk_getopt_reset(0);
	while ((c = ndk_getopt(argc, argv, "d:a:")) != -1) {
		switch (c) {
		case 'd':
			opt_drvname = ndk_optarg;
			break;
		case 'a':
			opt_ipaddr = ndk_optarg;
			break;
		default:
			printf("load [-d drv]\n");
			return;
		}
	}

	if (ndk_netdrv_init(opt_drvname) != 0) {
		printf("Loading WiFi module failed %s\n", opt_drvname);
		return;
	}

	struct in_addr ipaddr, netmask, gw;

	inet_aton(opt_ipaddr, &ipaddr);
	netmask.s_addr = htonl(0xFFFFFF00U);
	gw.s_addr = (ipaddr.s_addr & htonl(0xFFFFFF00U)) | htonl(0x00000001);

	printf("ipaddr= %08x, netmask= %08x, gw= %08x\n"
			, ipaddr.s_addr, netmask.s_addr, gw.s_addr);

	ndk_sysevt_handler_set(_cmdNetSysEventHandler, 0);
	ndk_netif_ioctl(NDK_IOCS_IF_UP, (long)"wlan0", NULL);
	ndk_netif_set_address("wlan0", &ipaddr, &netmask, &gw);
	printf("Loading WiFi module succeed\n");
}

static void _netcmd_unload(int argc, char *argv[])
{
	ndk_netif_set_address("wlan0", NULL, NULL, NULL);
	ndk_netif_ioctl(NDK_IOCS_IF_DOWN, (long)"wlan0", NULL);
	ndk_netdrv_exit();
	printf("Release WiFi module succeed\n");
}

static void _netcmd_join(int argc, char *argv[])
{
	const char *wep_key = NULL;
	const char *wpa_key = NULL;
	const char *essid = NULL;
	char buf[64];
	int c;

	ndk_getopt_reset(0);
	while ((c = ndk_getopt(argc, argv, "e:p:")) != -1) {
		switch (c) {
		case 'e':
			wep_key = ndk_optarg;
			break;
		case 'p':
			wpa_key = ndk_optarg;
			break;
		default:
			return;
		}
	}
	essid = argv[ndk_optind];

	if (!essid) {
		printf("join [-e wep-passwd | -p wpa-passwd] essid\n");
		return;
	}

	if (wep_key && wpa_key) {
		printf("Select WEP or WPA. Not both.");
		return;
	}

	if (wpa_key) {
		if (ndk_wpac_begin() != 0) {
			return;
		}

		char buf[64];
		int r = -1;

		if (ndk_wpac_request("add_network")) {
			goto lEND;
		}

		/* Assume new network id is 0. */
		sprintf(buf, "set_network 0 ssid '\"%s\"'", essid);
		if (ndk_wpac_request(buf)) {
			goto lEND;
		}

		sprintf(buf, "set_network 0 psk '\"%s\"'", wpa_key);

		if (ndk_wpac_request(buf) ||
		    ndk_wpac_request("set_network 0 key_mgmt WPA-PSK") ||
		    ndk_wpac_request("enable_network 0")
		   )
		{
			goto lEND;
		}

		r = 0;

lEND:
		ndk_wpac_end();
		return;
	}

	if (wep_key) {
		sprintf(buf, "iwconfig wlan0 key %s", wep_key);
		ndk_exec_main(buf, iwconfig_main);
	}

	sprintf(buf, "iwconfig wlan0 essid %s", essid);
	ndk_exec_main(buf, iwconfig_main);
}

static void _netcmd_dhcp(int argc, char *argv[])
{
	if (argc == 2 && !strcmp(argv[1], "start")) {
		ndk_netif_set_address_s(argv[0], "0.0.0.0", "255.255.255.0", NULL);
		ndk_dhcp_start(argv[0]);
	}
	else if (argc == 2 && !strcmp(argv[1], "stop")) {
		ndk_dhcp_stop(argv[0]);
	}
	else {
		printf("  dhcp ifname start/stop.\n");
	}
}

static void _netcmd_dhcpd(int argc, char *argv[])
{
	const char *local, *start;
	int num;

	if (argc == 0) {
		local = "192.168.1.1";
		start = "192.168.1.10";
		num = 10;
	}
	else if (argc == 1 && !strcmp("info", argv[0])) {
		ndk_dhcpd_printall();
		return;
	}
	else if (argc == 1 && !strcmp("stop", argv[0])) {
		ndk_dhcpd_stop();
		return;
	}
	else if (argc == 3) {
		local = argv[0];
		start = argv[1];
		num = atoi(argv[2]);
	}
	else {
		printf("Usage:\n");
		printf("  dhcpd      # using default settings: 192.168.1.1 192.168.1.10 10\n");
		printf("  dhcpd local-address start-adderss address-num\n");
		printf("  dhcpd info\n");
		printf("  dhcpd stop\n");
		return;
	}

	if (ndk_dhcpd_is_started()) {
		return;
	}

	ndk_netif_set_address_s("wlan0", local, "255.255.255.0", NULL);
	ndk_dhcpd_start_s("wlan0", start, num);
}

static void _netcmd_hapd(int argc, char *argv[])
{
	if (argc == 0) {
		ndk_hapd_start("B:/UDF/HAPD0.CFG", NULL);
	}
	else if (argc == 1 && !strcmp(argv[0], "-x")) {
		ndk_hapd_stop();
	}
	else if (argc == 1) {
		ndk_hapd_start(argv[0], NULL);
	}
	else if (argc == 2 && !strcmp(argv[0], "-t")) {
		ndk_hapd_test("B:/UDF/HAPD0.CFG", NULL, atoi(argv[1]));
	}
	else if (argc == 2) {
		// hapd config_file ssid_name
		NDKHapdOpt opts[] = {
			{"ssid", argv[1]},
			{NULL, NULL}
		};
		ndk_hapd_start(argv[0], opts);
	}
	else if (argc >= 3 && (argc & 0x01)) {
		// hapd config_file opt_name opt_value opt_name opt_value ...
		NDKHapdOpt opts[16];
		int i;
		for (i = 1; (i <= argc) && ((i-1)/2 < 16); i += 2) {
			opts[(i-1)/2].name = argv[i];
			opts[(i-1)/2].value = argv[i+1];
		}
		opts[(i-1)/2].name = NULL;

		ndk_hapd_start(argv[0], opts);
	}
	else {
		printf("Usage:\n");
		printf("  hapd	   -- start hapd daemon\n");
		printf("  hapd -h  -- help\n");
		printf("  hapd -x  -- stop hapd daemon\n");
		printf("  hapd config_file ssid_name\n");
		printf("  hapd config_file [opt_name opt_value [opt_name opt_value ...]]\n");
	}
}

static void netcmd_ifconfig(int argc, char **argv)
{
	if (argc == 2 && !strcmp(argv[1], "up")) {
		ndk_netif_ioctl(NDK_IOCS_IF_UP, (long)argv[0], NULL);
	}
	else if (argc == 2 && !strcmp(argv[1], "down")) {
		ndk_netif_ioctl(NDK_IOCS_IF_DOWN, (long)"wlan0", NULL);
	}
	else if (argc >= 2 && argc <= 4) {
		ndk_netif_set_address_s(argv[0], argv[1]
			, argc >= 3 ? argv[2] : NULL
			, argc == 4 ? argv[3] : NULL);
	}
	else if (argc == 0) {
		ndk_netif_printall();
	}
	else {
		printf("ifcfg interface up|down\n");
		printf("ifcfg interface ip_addr <netmask> <gw>\n");
	}
}

static void _netcmd_iwlist(int argc, char *argv[])
{
	char *av[16] = { "iwlist" };
	int   ac;

	for (ac = 1; argc > 0; --argc, ++ac) {
		av[ac] = argv[ac - 1];
	}

	iwlist_main(ac, av);
}

static void _netcmd_iwconfig(int argc, char *argv[])
{
	char *av[16] = { "iwconfig" };
	int   ac;

	for (ac = 1; argc > 0; --argc, ++ac) {
		av[ac] = argv[ac - 1];
	}

	iwconfig_main(ac, av);
}

static void _netcmd_iwpriv(int argc, char *argv[])
{
	char *av[16] = { "iwpriv" };
	int   ac;

	for (ac = 1; argc > 0; --argc, ++ac) {
		av[ac] = argv[ac - 1];
	}

	iwpriv_main(ac, av);
}

static void _netcmd_wpac(int argc, char *argv[])
{
	char *av[16] = { "wpac" };
	int   ac;

	for (ac = 1; argc > 0; --argc, ++ac) {
		av[ac] = argv[ac - 1];
	}

	wpa_cli_main(ac, av);
}

static void _netcmd_wpas(int argc, char *argv[])
{
	if (argc == 1 && !strcmp(argv[0], "-x")) {
		ndk_wpas_stop();
	}
	else if (argc == 0) {
		if (ndk_wpas_start() == 0) {
			printf("wpas started\n");
		}
	}
	else {
		printf("Usage:\n");
		printf("  wpas -h  -- help\n");
		printf("  wpas -x  -- exit\n");
		printf("  wpas     -- start wpa_supplicant daemon\n");
	}
}

static void _netcmd_ping(int argc, char *argv[])
{
	if (argc < 1) {
		printf("ping host\n");
		return;
	}

	long triptime;
	if (ndk_ping_s(argv[0], 5000, &triptime) == 0)
		printf("%s is alive! time=%lu.%lu ms\n", argv[0], triptime / 10, triptime % 10);
	else
		printf("timeout\n");
}

extern void appWiFiStartConnection(UINT8 wifiParm);
static void _netcmd_station_test(int argc, char *argv[])
{
	char* ap_name = "dir812";
	char* ap_addr = "192.168.10.1";

	if (argc >= 1)
		ap_name = argv[0];

	if (argc >= 2)
		ap_addr = argv[1];

	if (ndk_netdrv_init("rtk") != 0) {
		printf("Loading WiFi module failed\n");
		return;
	}

	ndk_sysevt_handler_set(_cmdNetSysEventHandler, 0);
	ndk_netif_ioctl(NDK_IOCS_IF_UP, (long)"wlan0", NULL);
	ndk_netif_set_address_s("wlan0", "192.168.10.101", "255.255.255.0", "192.168.10.1");

	{
		char* av[] = { ap_name };
		_netcmd_join(1, av);
	}

	{
		char* av[] = { ap_addr };
		sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 300);
		_netcmd_ping(1, av);
		_netcmd_ping(1, av);
	}

	//appWiFiStartConnection(0x08 | 0x10);
}

static void _netcmd_hostap_test(int argc, char *argv[])
{
	char* ap_name = "aaaaaaaa";

	if (argc >= 1)
		ap_name = argv[0];

	if (ndk_netdrv_init("rtk") != 0) {
		printf("Loading WiFi module failed\n");
		return;
	}

	ndk_sysevt_handler_set(_cmdNetSysEventHandler, 0);
	ndk_netif_ioctl(NDK_IOCS_IF_UP, (long)"wlan0", NULL);
	ndk_netif_set_address_s("wlan0", "10.1.1.1", "255.255.255.0", "10.1.1.1");
	ndk_dhcpd_start_s("wlan0", "10.1.1.200", 10);

	NDKHapdOpt opts[] = {
		{"ssid", ap_name},
		{"channel", "auto"},
		{NULL, NULL}
	};
	ndk_hapd_start("D:/HAPD0.CFG", opts);
}

static void _netcmd_mac_address(int argc, char *argv[])
{
	UINT8 hwaddr[32] = "\0"; 
	ndk_netif_ioctl(NDK_IOCG_IF_HWADDR, (long)"wlan0", (long *)hwaddr); 

	printf(	"Get mac addr %x:%x:%x:%x:%x:%x\n", 
	                hwaddr[0],hwaddr[1],hwaddr[2], 
	                hwaddr[3],hwaddr[4],hwaddr[5]); 	
}

static cmd_t subnetcmds[] = {
	{"info", _netcmd_sysinfo, "Show ndk info."},
	{"load", _netcmd_load, "Loading WiFi modules", NULL},
	{"unload",_netcmd_unload, "Release WiFi modules", NULL},
	{"dhcp", _netcmd_dhcp, "Start DHCP client", NULL},
	{"dhcpd", _netcmd_dhcpd, "dhcpd [local-address start-address count]|dump", NULL},
	{"hapd", _netcmd_hapd, "Start or stop hostapd", NULL},
	{"ifconfig", netcmd_ifconfig, "ifconfig", NULL},
	{"iwlist", _netcmd_iwlist, "iwlist", NULL},
	{"iwconfig", _netcmd_iwconfig, "iwconfig", NULL},
	{"iwpriv", _netcmd_iwpriv, "iwpriv", NULL},
	{"join", _netcmd_join, "join [-e wep-passwd | -p wpa-passwd] essid", NULL},
	{"ping", _netcmd_ping, "ping host", NULL},
	{"wpac", _netcmd_wpac, "call wpa_cli command", NULL},
	{"wpas", _netcmd_wpas, "Start or stop wpa-supplicant", NULL},
#ifdef ICAT_STREAMING
	{"mediasrv", _netcmd_mediasrv, "Start media server", NULL},
	{"ms", _netcmd_mediasrv, "Start media server", NULL},
	{"mc", _netcmd_mediactl, "Control media server", NULL},
	{"mediadump", _netcmd_mediadump, "Dump media attrs and cfgs", NULL},
#endif
	{"-", _netcmd_station_test, "Station mode. Private Command", NULL},
	{"+", _netcmd_hostap_test, "HostAP mode. Private Command", NULL},
	{"mac", _netcmd_mac_address, "get mac addr. Private Command", NULL},
	
	{NULL,  NULL, NULL, NULL}
};

static void _netcmd_not_found(int argc, char *argv[])
{
	printf("Available commands:\n");

	cmd_t *pcmd;
	const char *spaces = " . . . . . . . . . . . :";

	for (pcmd = subnetcmds; pcmd->cmd; ++pcmd) {
		printf(" %s %s %s\n", pcmd->cmd, spaces + strlen(pcmd->cmd) + 3, pcmd->phelp);
	}

	printf("\n");
}

static void _netcmd_main(int argc, char *argv[])
{
	static cmd_t *psubcmds = NULL;
	if (!psubcmds) {
		psubcmds = cmdSubBatchRegister(0, &subnetcmds[0]);
	}

	cmdSubProcess(argc, argv, psubcmds, _netcmd_not_found);
}

static cmd_t netcmds[] = {
	{ "appnet", _netcmd_main,     "Custom Net command", NULL, },
	{ NULL,  NULL,            NULL, NULL, }, /* null terminator */
};


void net_system_init()
{
	ndk_global_init(20);
	cmdBatchRegister(netcmds);
	printf("WiFi system init done.\n");
}
