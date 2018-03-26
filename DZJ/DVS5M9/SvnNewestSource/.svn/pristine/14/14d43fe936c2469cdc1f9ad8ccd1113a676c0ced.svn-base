/**************************************************************************
 *                                                                        *
 *         Copyright (c) 2012 by iCatch Technology Co., Ltd.             *
 *                                                                        *
 *  This software is copyrighted by and is the property of Sunplus        *
 *  Technology Co., Ltd. All rights are reserved by Sunplus Technology    *
 *  Co., Ltd. This software may only be used in accordance with the       *
 *  corresponding license agreement. Any unauthorized use, duplication,   *
 *  distribution, or disclosure of this software is expressly forbidden.  *
 *                                                                        *
 *  This Copyright notice MUST not be removed or modified without prior   *
 *  written consent of Sunplus Technology Co., Ltd.                       *
 *                                                                        *
 *  Sunplus Technology Co., Ltd. reserves the right to modify this        *
 *  software without notice.                                              *
 *                                                                        *
 *  Sunplus Technology Co., Ltd.                                          *
 *  19, Innovation First Road, Science-Based Industrial Park,             *
 *  Hsin-Chu, Taiwan, R.O.C.                                              *
 *                                                                        *
 *  Author: Andy.Li                                                       *
 *                                                                        *
 **************************************************************************/
#ifndef __NDK_HTTPD_H__
#define __NDK_HTTPD_H__

#ifdef __cpulsplus
extern "C" {
#endif

/* CGI Environment Variables
 */
enum {
	/* 0 */
	NDK_CGI_ENV_GATEWAY_INTERFACE,
	NDK_CGI_ENV_SERVER_PROTOCOL,
	NDK_CGI_ENV_SERVER_PORT,	/*long*/
	NDK_CGI_ENV_SERVER_NAME,
	NDK_CGI_ENV_SERVER_ROOT,
	NDK_CGI_ENV_DOCUMENT_ROOT,
	NDK_CGI_ENV_REQUEST_METHOD,
	NDK_CGI_ENV_LOCAL_ADDR,
	NDK_CGI_ENV_REMOTE_ADDR,
	NDK_CGI_ENV_REMOTE_PORT,	/*long*/
	NDK_CGI_ENV_REQUEST_URI,
	/* 10 */
	NDK_CGI_ENV_SCRIPT_NAME,
	NDK_CGI_ENV_SCRIPT_FILENAME,
	NDK_CGI_ENV_PATH_TRANSLATED,
	NDK_CGI_ENV_CONTENT_TYPE,
	NDK_CGI_ENV_CONTENT_LENGTH,	/*unsigned long*/
	NDK_CGI_ENV_QUERY_STRING,
	NDK_CGI_ENV_PATH_INFO,
	NDK_CGI_ENV_REMOTE_USER,
	NDK_CGI_ENV_AUTH_TYPE,

	NDK_CGI_ENV_VARS
};

/* HTTPd options */
enum {
	NDK_HTTPD_OPT_ROOT = 0, /* root directory. default D: */
	NDK_HTTPD_OPT_PORT,     /* listening port. default 80. */

	NDK_HTTPD_OPT_NUM       /* Number of options. */
};

/* \brief CGI entry function. The CGI function is executed in another thread.
 * \param vars environment variables.
 * \sock CGI function should use this socket to receive and/or send data to the
 *   http client. Don't close it when CGI function exit for the http server will close
 *   it automatically.
 */
typedef struct _CgiEntry  NDKCgiEntry;
typedef void (*NDKCgiFunc)(NDKCgiEntry *entry, char* vars[NDK_CGI_ENV_VARS], int sock);

struct _CgiEntry {
	NDKCgiEntry  *next;
	const char   *name;
	NDKCgiFunc   func;
};

typedef struct {
	int          id;
	const char   *value;
} NDKHttpdOpt;

/* \brief Start HTTP server.
 * \param num number of options.
 * \param opts Option array.
 * \return 0: succeed. < 0: error code.
 */
int  _ndk_httpd_start(int num, NDKHttpdOpt *opts);
void _ndk_httpd_stop();

/*
 * \param entry entry cannot be freed until calling after ndk_httpd_unregister_cgi.
 */
int _ndk_httpd_register_cgi(NDKCgiEntry *entry);

/*
 * \param cgi_uri: if cgi_uri == NULL, return the first NDKCgiEntry. It will make it easier to
 *   unregister all cgi_entries.
 */
NDKCgiEntry *_ndk_httpd_unregister_cgi(const char* cgi_uri);

/***********************************************************************************/
/*  NDK HTTP Server API Definitions. Don't call above functions directly           */
/***********************************************************************************/
#define ndk_httpd_start(a, b)       _ndk_httpd_start(a, b)
#define ndk_httpd_stop()            _ndk_httpd_stop()

#define ndk_httpd_register_cgi(a)   _ndk_httpd_register_cgi(a)
#define ndk_httpd_unregister_cgi(a) _ndk_httpd_unregister_cgi(a)

#ifdef __cpulsplus
}
#endif

#endif

