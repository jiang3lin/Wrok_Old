/**************************************************************************
 *                                                                        *
 *         Copyright (c) 2006 by Sunplus Technology Co., Ltd.             *
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
 *  Author: Stephen Hung                                                  *
 *                                                                        *
 **************************************************************************/
#ifndef _COMMON_H_
#define _COMMON_H_

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define SUCCESS  0
#define FAIL     1

#define TRUE   1
#define FALSE  0

#ifdef __ASSEMBLER__

#ifndef NULL
#define NULL  0
#endif

#else  /*#ifdef __ASSEMBLER__*/

#ifdef __STRICT_ANSI__
/** \note Paul 20101231
 * our stdio.h will disable some standard declaration when -ansi is
 * specified as compiler option.  So #ifdef is added to remove warning
 */
#include <stdarg.h>
#include <stddef.h>
#endif /*#ifdef __STRICT_ANSI__*/

#ifndef NULL
#define NULL  ((void *)0)
#endif

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define COMMON_ALIGNED      __attribute__ ((__aligned__))
#define UNUSED              __attribute__ ((__unused__))

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef unsigned long long UINT64;
typedef unsigned int       UINT32;
typedef unsigned short     UINT16;
typedef unsigned char      UINT8;

typedef signed long long   SINT64;
typedef signed int         SINT32;
typedef signed short       SINT16;
typedef signed char        SINT8;

typedef unsigned char      BOOL;

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
/*workaround for <stdio.h> in which putchar is a macro*/
/*"common.h" is advised to include after <stdio.h>*/
#undef putchar
int putchar(int ch);

/*legacy symbols*/
#undef  sio_printf
#define sio_printf   printf
#undef  sio_vprintf
#define sio_vprintf  vsprintf
void usb_printf(const char *format,...);
#ifdef __STRICT_ANSI__
/**
 * \note Edward 20101215
 * Comment snprintf here due to it is already in stdio.h.
 * If some .c file includes stdio.h and common.h and calls snprintf(), 
 * then conflicting prototype error happened.
 * \note Paul 20101231
 * our stdio.h will disable some standard declaration when -ansi is
 * specified as compiler option.  So #ifdef is added to remove warning
 */
int snprintf(char *dest, size_t size, const char *format, ...)
			__attribute__ ((format (printf, 3, 4)));
int vsnprintf(char *dest, size_t size, const char *format, va_list ap);
char *strupr(char *string);
int strcasecmp(const char *s1, const char *s2);
int strncasecmp(const char *s1, const char *s2, size_t len);
#endif
void sioRedirectOff(void);
void * sioPutsRedirect(void *pf);
void * sioPutcRedirect(void *pf);
void * sioPutsCallbackSet(void *pf);
void * sioPutcCallbackSet(void *pf);
void ltoa(UINT32 val, UINT8 *pbuf, UINT32 radix, UINT32 digitno);
void tmrUsWait(UINT32 us);

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

#endif /*#ifdef __ASSEMBLER__*/

#ifdef __cplusplus
}
#endif

#endif  /* _COMMON_H_ */

