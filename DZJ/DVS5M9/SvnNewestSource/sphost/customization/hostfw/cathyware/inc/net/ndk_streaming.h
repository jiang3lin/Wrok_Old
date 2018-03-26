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
#ifndef __NDK_STREAMING_H__
#define __NDK_STREAMING_H__

#ifdef SPCA6330
# define ICAT_DUALSTREAM
#endif

#ifdef SPCA6350
# undef ICAT_DUALSTREAM
#endif

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************************************
 *                         C O S T A N T S
 *******************************************************************************/
/* \enum NDK Events
 */
enum {
	/* \brief Data is RTSP URL string(const char*).
	 * Return: 1, Accept this RTSP request; 0, refuse.
	 */
	NDK_ST_EVT_RTSP_REQUEST = 0,

	/* \brief Sent before the streaming is about to start. The encoder or meida source
	 *   should be prepared to generate audio/video bistream after receiving this event.
	 * \param (none).
	 * \return 1: start streaming; 0: abort streaming.
	*/
	NDK_ST_EVT_ON_STARTED,

	/* \brief Sent after the streaming is stopped. The encoder or media source can be
	 * closed after receiving this event.
	 * \param (none).
	 * \return ignored.
	 */
	NDK_ST_EVT_ON_STOPPED,

	/* \brief Send after a frame is dropped.
	 * \param frame index.
	 * \return ignored.
	 */
	NDK_ST_EVT_FRAME_DROPPED,
	/* \brief Send when the Wi-Fi driver tx buffer is cleared by media server.
	 * \param (none).
	 * \return ignored.
	 */
	NDK_ST_EVT_TXBUFQ_CLEARED,
};

/* \enum Streaming Mode
 */
typedef enum {
	NDK_ST_DUAL_H264JPEG  = 0, /* This mode is only for testing purpose only. Don't use it in product*/
	NDK_ST_DUAL_JPEGONLY  = 1,

	/* Dual-Streaming */
	/* The H264 is for storage only. */
	NDK_ST_DUAL_STREAMING = NDK_ST_DUAL_JPEGONLY,
	/* Mono-streaming */
	NDK_ST_MONO_H264      = 2,
	NDK_ST_MONO_JPEG      = 3,

	NDK_ST_MODE_NR        = 4,
	NDK_ST_INVALID_MODE   = 0x7FFFFFFF
} NDKStMode;

/* Dual-stream control code. */
typedef enum {
	NDK_DUALSTREAM_PAUSE,  /* Pause dual-streaming. No parameter */
	NDK_DUALSTREAM_RESUME, /* Resume dual-streaming. No parameter */
	NDK_DUALSTREAM_MAXQ_SET,
	NDK_DUALSTREAM_BITRATE_SET,
} NDKDualStreamCtrlCode;

/* attribute code. */
typedef enum {
	/* \brief Timeout to resend last frame if there is no new frame for sending.
	 * Default: 0. Min. 30 ms. 0: don't resend. Unit: ms.
	 */
	NDK_ST_ATTR_TIME_TO_RESEND_FRAME = 0,

	/* \brief Lag time to drop frame.
	 * Default: 150 ms. Min. 60 ms. 0: don't drop frame.
	 */
	NDK_ST_ATTR_TIME_TO_DROP_FRAME,

	/* \brief Network loading measure value.
	 * If network loading is too high, media server will stop sending frames until it
	 * becomes low again.
	 * Default: 80. Set to 0 to disable the network loading checking.
	 */
	NDK_ST_ATTR_NET_LOADING_HIGH,

	/* \brief Clear the TX buffer queue of the WiFi driver .
	 * If the network loading has become high for some time, the media server will
	 * clear the dirver's tx buffer for the (media) data in the buffer may outmode.
	 * Unit: milli-seconds.
	 * Default: 0, this feature is disabled.
	 */
	NDK_ST_ATTR_TIME_CLR_TXBUFQ,

	/* \brief Streaming frame rate. Default: 0. Max.: 30.
	 * The frame rate control is implemented by dropping frames. It is only valid for
	 * Motion-JPEG streaming. For H.264, frame cannot be dropped. Otherwise, decoding in
	 * the streaming client may failed.
	 * The value can be: 6, 10, 15, 20 or 30.
	 */
	NDK_ST_ATTR_FRAME_RATE,

	NDK_ST_ATTR_MAX
} NDKStAttr;

/* Constants for Custom Media Source */

typedef enum {
	NDK_ST_MEDIASRC_FILE = 0,

	NDK_ST_MEDIASRC_NUM
} NDKStMediaSrcType;

enum {
	NDK_ST_MEDIABUFFER_UNKNOW = 0,
	NDK_ST_MEDIABUFFER_VIDEO  = 1,
	NDK_ST_MEDIABUFFER_AUDIO  = 2,
};

/********************************************************************************
 *                         D A T A  T Y P E S
 *******************************************************************************/
/* \brief Streaming event handler.
   \return 1: SUCCEED; 0: FAILED.
 */
typedef BOOL (*NDKStEvtHandler)(UINT32 event, UINT32 data);

/* Streaming Server Configure */
typedef struct NDKStCfg
{
	const char*     root_dir;       /* root_dir: Root directory for local file streaming. */
	UINT16          port;           /* RTSP listening port. */
	NDKStMode       st_mode;
	UINT32          audio_on;
	unsigned        keepalive_secs; /* Keep alive time in seconds for streaming client. 0: no keep-alive check. */
	NDKStEvtHandler	evt_handler;
} NDKStCfg;

/* Definitions for Custom Media Source */
typedef void* NDKStMediaSrcHandle;
typedef void* NDKStMediaBufferObject;

typedef struct NDKStMediaBuffer
{
	/* The internal buffer object defined by customer for this media-buffer. */
	NDKStMediaBufferObject bufobj;
	UINT32 f_buftype    : 2, /* NDK_ST_MEDIABUFFER_AUDIO, NDK_ST_MEDIABUFFER_VIDEO */
	       f_keyframe   : 1, /* For JPEG, always 1. For H.264, set to 1 for I-frame; set to 0 for P-frame and B-frame. */
	       f_paramframe : 1; /* Set to 1 for H.264 SPS and PPS frame. Otherwise, set to 0 */
	/* The bitstream data pointer */
	UINT8  *data;
	/* Length of the bitstream data. */
	UINT32 length;
	/* Frame duration in milli-seconds. */
	UINT32 duration;
	/* Presentation timestampe. */
	struct timeval pts;
} NDKStMediaBuffer;

/* Callback function provided by NDK for pushing buffer to NDK for RPT streaming.
 * \param buf: media buffer pointer.
 * \return void. If pushing failed, NDK will discard this buffer.
 */
typedef void (*NDKStMediaSrcPushBufferFunc)(NDKStMediaBuffer *buf, void *param);

typedef struct NDKStMediaSrcCallback
{
	/* \brief Open the custom media-source for streaming.
	 * \param push_func: a callback function for pushing buffers to NDK.
	 * \param push_param: argument of the pushing function.
	 * \param src_arg: media-source argument. Depending on media-source types:
	 *       NDK_ST_MEDIASRC_FILE: const char *filename
	 * \return NULL: failed. Othewise a handle represents the custom media-source object.
	 */
	NDKStMediaSrcHandle (*open)(NDKStMediaSrcPushBufferFunc push_func, void *push_param, void *src_arg, ...);

	/* \brief Close the custom media-source.
	 * \param h: handle of the custom media-source object
	 */
	void   (*close) (NDKStMediaSrcHandle h);
	/* \brief Tell the custom media-source object to pause pushing buffer to NDK.
	 * \param h: handle of the custom media-source object
	 * \return SUCCEED or FAIL.
	 */
	UINT32 (*pause) (NDKStMediaSrcHandle h);
	/* \brief Tell the custom media-source object to continue pushing buffer to NDK.
	 * \param h: handle of the custom media-source object
	 * \return SUCCEED or FAIL.
	 */
	UINT32 (*resume)(NDKStMediaSrcHandle h);
	/* \brief Test if the end of source is met.
	 * \param h: handle of the custom media-source object
	 * \return TRUE: at the end of source. FALSE: not.
	 */
	BOOL   (*end_of_source)(NDKStMediaSrcHandle h);
	/* \brief Seek media source to position.
	 * \param h: handle of the custom media-source object
	 * \param position: unit is milli-seconds.
	 * \return SUCCEED or FAIL.
	 */
	/* Return: 0, succeed; 1, failed; */
	UINT32 (*seek_to)(NDKStMediaSrcHandle h, UINT32 position/*in milli-seconds */);
	/* \breif Get audio/video attributes. The attribute symbols are same with the one defined in enum mediaAttr_e
	   Below are the attributes used by NDK now:
	    SP5K_MEDIA_ATTR_WIDTH
	    SP5K_MEDIA_ATTR_HEIGHT
	    SP5K_MEDIA_ATTR_DURATION
	    SP5K_MEDIA_ATTR_VIDEO_FRAME_RATE
	    SP5K_MEDIA_ATTR_VIDEO_CODEC
	    SP5K_MEDIA_ATTR_AUDIO_CODEC
	    SP5K_MEDIA_ATTR_AUDIO_SAMPLE_BITS
	    SP5K_MEDIA_ATTR_AUDIO_SAMPLE_RATE
	    SP5K_MEDIA_ATTR_AUDIO_CHANNELS
	 */
	UINT32 (*get_attribute)(NDKStMediaSrcHandle h, UINT32 attr, UINT32 *val);
	/* Free the buffer object. */
	void   (*free_buffer_object)(NDKStMediaBufferObject obj);
} NDKStMediaSrcCallback;

/********************************************************************************
 *                         F U N C A T I O N S
 *******************************************************************************/
/* \brief Start streaming server
 * \param root_dir: Root directory for local file streaming.
 * \param port: RTSP listening port.
 * \return 0 - succeed; others - failed.
 * \note All streaming files must be in root_dir directory.
 *       Length of root_dir string must be less than 63.
 *       The streaming parameters will be reset to default values in this function. So, change
 *       parameters after calling of this function.
 */
int  _ndk_st_start_server(NDKStCfg *cfg);

/* \brief Stop streaming server */
void _ndk_st_stop_server();

/* \brief Set/Get media server attributes. Must be called after the server is started.
 * \param attr: attribute code.
 * \param value: attribute value.
 * \return 0: succeed or -1: failed.
 */
int  _ndk_st_set_attribute(NDKStAttr attr, long value);
int  _ndk_st_get_attribute(NDKStAttr attr, long *value);

/* \brief Close existing streaming connections. */
void _ndk_st_close_streaming();

/* \brief Test if the streaming is already started */
BOOL _ndk_st_is_started();

BOOL _ndk_st_is_dualstream();

BOOL _ndk_st_is_audio_on();

int  _ndk_st_change_mode(NDKStMode new_mode, NDKStMode *old_mode);

NDKStMode _ndk_st_get_mode();

#ifdef ICAT_DUALSTREAM
/*
 * The following is dualstream stream (jpeg) functions.
 */
/* \brief Start DualStream with bitrate control(BRC)
 * \param width: JPEG image width
 * \param height: JPEG image height
 * \param qInitial: Initial Q factor. Must less than qMax.
 * \param qMax: Max JPEG Q factor. 0: disable brc
 * \param frmrate: frames per second. Must > 0 and <= 30.
 * \param bitrate: JPEG bitrate
 * \return 0, succeed; <0, failed.
 */
int _ndk_st_dualstream_start(UINT32 width, UINT32 height, UINT32 qInitial, UINT32 qMax, UINT32 frmrate, UINT32 bitrate);

/* \brief dualstream control. See NDKDualStreamCtrlCode for details.
 */
int _ndk_st_dualstream_control(NDKDualStreamCtrlCode code, long param);

/* \brief Stop DualStream.
 */
int _ndk_st_dualstream_stop();
#endif

int _ndk_st_register_mediasrc(NDKStMediaSrcType type, NDKStMediaSrcCallback *cb);

/********************************************************************************
 *  NDK Streaming API Definitions. Don't call above functions directly
 *******************************************************************************/
#define ndk_st_start_server(a)                          _ndk_st_start_server(a)
#define ndk_st_stop_server()                            _ndk_st_stop_server()
#define ndk_st_close_streaming()                        _ndk_st_close_streaming()
#define ndk_st_set_attribute(a,b)                       _ndk_st_set_attribute(a, b)
#define ndk_st_get_attribute(a,b)                       _ndk_st_get_attribute(a, b)
#define ndk_st_is_started()                             _ndk_st_is_started()
#define ndk_st_is_dualstream()                          _ndk_st_is_dualstream()
#define ndk_st_is_audio_on()                            _ndk_st_is_audio_on()
#define ndk_st_change_mode(a,b)                         _ndk_st_change_mode(a,b)
#define ndk_st_get_mode()                               _ndk_st_get_mode()

#ifdef ICAT_DUALSTREAM
/* DualStream(JPEG) functions. Only valid on SPCA6330 platform */
#define ndk_st_dualstream_start_brc(w, h, q, fr, br)    _ndk_st_dualstream_start(w, h, 10, q, fr, br)
#define ndk_st_dualstream_start(w, h, q, fr)            _ndk_st_dualstream_start(w, h, q, 0, fr, 0)
#define ndk_st_dualstream_control(a,b)                  _ndk_st_dualstream_control(a,b)
#define ndk_st_dualstream_stop()                        _ndk_st_dualstream_stop()
#endif

#define ndk_st_register_mediasrc(a, b)                  _ndk_st_register_mediasrc(a, b)

/*Deprecated*/
#define ndk_st_streaming_is_started()                   _ndk_st_is_started()
#define ndk_st_is_dual_streaming()                      _ndk_st_is_dualstream()

#ifdef __cplusplus
}
#endif

#endif

