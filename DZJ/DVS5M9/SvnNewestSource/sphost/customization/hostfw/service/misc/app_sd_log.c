/*******************************************************************************
 *
 *       Copyright (c) 2002-2010 by iCatch Technology, Inc.
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
 *  File Description:
 *
 *  Author: Chris Wang
 *
 ******************************************************************************/
/** 
 * @file app_sd_log.c 
 * @brief Logging error message to storage
 * @author chris
 */ 

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "common.h"
#include "sp5k_msg_def.h"
#include "sp5k_disk_api.h"
#include "sp5k_os_api.h"
#include "sp5k_fs_api.h"
#include "sp5k_dbg_api.h"
#include "sp5k_global_api.h"
#include "sp5k_utility_api.h"
#include "app_timer.h"
#include "cmd.h"
#include "debug_mod/dbg_api.h"
#include "middleware/proflog.h"

#if SPCA6330

/* app watchdog unit test */
#define APWDG_UT 0 
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define LOG_SIGNATURE_FILE_NAME_NAND "D:\\NANDMARK.LOG"
#define LOG_SIGNATURE_FILE_NAME_SD "D:\\SDMARK.LOG"
#define LOG_WATCHDOG_TIMEOUT	(4000000) /* us */
#define LOG_WATCHDOG_ASSERT 	(LOG_WATCHDOG_TIMEOUT/10240) /* 10ms */
#define LOG_BUFFER_SIZE (1024*16)
#define DETECT_POWER_KEY_PRESSED_INTERVAL   (1000*500) /* us (500ms) */
#define POWER_KEY_LONG_PRESSED_POWEROFF_COUNT      (10) /* 5s */
#define POWER_KEY_LONG_PRESSED_REBOOT_COUNT        (20) /* 10s */
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#if 0
    #define PROF_LOG_ADD(...)
    #define PROF_LOG_PRINTF(...)
    #define LOGDBG_PRINT(fmt, args...)
#else
    /* #define PROF_LOG_ADD		profLogAdd */
    #define PROF_LOG_PRINTF		profLogPrintf
    #define LOGDBG_PRINT(fmt, args...) printf(fmt, ##args)
#endif
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
static UINT8 logfname[]="C:\\ICATCH00.LOG"; /* log file name */
static UINT32 isEnableLog = 0 ; /* SP5K_MSG_DISK_REMOVAL -> disable logging
                                   SP5K_MSG_DISK_INSERT  -> Enable logging 
                                   1 to store log, 0 no store */

static UINT32 logDevId = 0xFFFFFFFF ;
static TX_THREAD *thdAppWatchdog = 0;
static UINT32 osTimerTick = 0 ; /* unit 10 ms */
static UINT32 osTimerTick_PowerKeyPress = 0 ; /* unit 10 ms */
static UINT32 isKeyDetectRunning = -1 ; /* -1: Disable Power Key Detect */
static UINT32 logBufferSize = LOG_BUFFER_SIZE;
static UINT32 bASSERTPowerOff = 1 ; /* 0:Reboot , 1:PowerOff */

#if APWDG_UT

/* use background task to do unit test */
//#define WDG_BACKGRUND_TEST 1
#undef WDG_BACKGRUND_TEST

static TX_MUTEX muxWDGTest;
static UINT32 testHWWatchdog = 0 ;
static UINT32 testCase = 0 ;
static UINT32 divzero = 1 ;
#ifdef WDG_BACKGRUND_TEST
static void _watchdog_cmd_isr_test(int mode);
static void _watchdog_cmd_crash_test(int mode);
static void _watchdog_background_test(void);
#endif

#endif
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern void osDeadHostCbSet(void (*fp_osDeadCb)(void));
extern UINT32 dbgPrintLogInit(UINT32 size);
extern void dbgPrintLogFnameSet(UINT8 *fname);
extern void dbgPrintLogConfirmCallbackSet(void *fp);
extern void ros_thread_sleep(UINT32 ms);
extern void sioCtrl(UINT32 opt);
/* int uartWriteByte(UINT32 port,int ch); */
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

#define MAX_SECTION 50
#define MAX_NAME 50
#define INI_ALLOW_BOM 0
#define INI_ALLOW_MULTILINE 0

#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0

/** 
 * @brief Strip whitespace chars off end of given string, in place. Return s. 
 *  
 * @param[in] s 
 * @return s
 */ 
static char* rstrip(char* s)
{
    char* p = s + strlen(s);
    while (p > s && isspace((UINT8)(*--p)))
        *p = '\0';
    return s;
}

/** 
 * @brief Return pointer to first non-whitespace char in given 
 *        string.
 *  
 * @param[in] s 
 * @return s
 */ 
static char* lskip(const char* s)
{
    while (*s && isspace((UINT8)(*s)))
        s++;
    return (char*)s;
}

/** 
 * @brief Return pointer to first char c or ';' comment in given string, or pointer to
   null at end of string if neither found. ';' must be prefixed by a whitespace
   character to register as a comment.
 *  
 * @param[in] s 
 * @return s
 */ 
static char* find_char_or_comment(const char* s, char c)
{
    int was_whitespace = 0;
    while (*s && *s != c && !(was_whitespace && *s == ';')) {
        was_whitespace = isspace((UINT8)(*s));
        s++;
    }
    return (char*)s;
}

/** 
 * @brief Version of strncpy that ensures dest (size bytes) is null-terminated.
 *  
 * @param[in] dest 
 * @param[in] src 
 * @param[in] size 
 * @return s
 */ 
static char* strncpy0(char* dest, const char* src, size_t size)
{
    strncpy(dest, src, size);
    dest[size - 1] = '\0';
    return dest;
}



/** 
 * @brief get line of string from file
 *  
 * @param[in] lineBuff 
 * @param[in] lineSize 
 * @param[in] fd 
 * @return SUCCESS / FAIL
 */ 
UINT32
myfgets(
   UINT8* lineBuff,
   UINT32 lineSize,
   UINT32* fd)
{   
    static UINT32 fileSize = 0 ;
    static UINT8* buff = 0 ;
    static UINT32 buffSize = 0 ;
    static UINT32 length = 0 ;
    static UINT32 buffPos = 0 ;

    UINT32 done = 0 ;
    UINT32 i = 0 ;
    
    if ( !buffSize ) {
        length = 0 ;
        buffPos = 0 ;
        fileSize = sp5kFsFileSizeGet(*fd);
        if ( fileSize <= 0 ) {
            goto ERROR_RETURN;
        }

        if ( fileSize > 512 ) {
            buffSize = 512 ;
        } else {
            buffSize = fileSize ;
        }
        buff = sp5kMalloc(buffSize);
        if ( !buff ) {
            goto ERROR_RETURN;
        }
    }

    if ( ( fileSize == 0 ) && ( length == 0 ) ) {
        goto ERROR_RETURN;
    }

    for (i = 0 ;  !done && i < lineSize - 1; i++ , buffPos++ , length-- ) {
        if ( length == 0 ) {
            if ( fileSize == 0 ) {
                done = 1;
                break;
            } else {
                buffSize = sp5kFsFileRead(*fd, buff, buffSize);
                if ( buffSize <= 0 ) {
                    goto ERROR_RETURN;
                }
                buffPos = 0 ;
                length = buffSize ;
                fileSize = fileSize - buffSize ;
                if (( fileSize != 0 ) && ( fileSize < buffSize )) {
                    buffSize = fileSize ;
                }
            }
        }
        lineBuff[i] = buff[buffPos];
        if ( lineBuff[i] == '\n' ) {
            done = 1;
        }
    }
    lineBuff[i] = '\0' ;
    return SUCCESS;

ERROR_RETURN:
    if (buff) 
        sp5kFree(buff);
    buffSize = 0 ;
    fileSize = 0 ;
    return FAIL;
}

/** 
 * @brief parse INI file
 *  
 * @param[in] fd 
 * @param[in] handler 
 * @param[in] user 
 * @return SUCCESS / FAIL
 */ 
UINT32 
iniParseFile(
   UINT32* fd,
   UINT32 (*handler)(void*, const char*, const char*,const char*),
   void* user)
{

    /* Uses a fair bit of stack (use heap instead if you need to) */

    UINT8* line = 0 ;
    UINT32 ret = 0;
    UINT32 error = 0;


    UINT8 section[MAX_SECTION] = "";
    UINT8 prev_name[MAX_NAME] = "";

    UINT8* start;
    UINT8* end;
    UINT8* name;
    UINT8* value;
    UINT32 lineno = 0;


    line = sp5kMalloc(256);
    if (!line) {
        return FAIL;
    }
    ret = sp5kFsFileSeek(*fd, 0, SP5K_FS_SEEK_SET);
    if ( ret == FAIL ) {
        goto INIPARSE_EXIT;
    }
    
    /* Scan through file line by line */
    while (1) {
        ret = myfgets(line, 256, fd);
        if ( ret == FAIL ) {
            goto INIPARSE_EXIT;
        }
        lineno++;

        start = line;
#if INI_ALLOW_BOM
        if (lineno == 1 && (UINT8)start[0] == 0xEF &&
                           (UINT8)start[1] == 0xBB &&
                           (UINT8)start[2] == 0xBF) {
            start += 3;
        }
#endif
        start = lskip(rstrip(start));

        if (*start == ';' || *start == '#') {
            /* allow '#' comments at start of line */
        }
#if INI_ALLOW_MULTILINE
        else if (*prev_name && *start && start > line) {
            if (!handler(user, section, prev_name, start) && !error)
                error = lineno;
        }
#endif
        else if (*start == '[') {
            /* A "[section]" line */
            end = find_char_or_comment(start + 1, ']');
            if (*end == ']') {
                *end = '\0';
                strncpy0(section, start + 1, sizeof(section));
                *prev_name = '\0';
            }
            else if (!error) {
                /* No ']' found on section line */
                error = lineno;
            }
        }
        else if (*start && *start != ';') {
            /* Not a comment, must be a name[=:]value pair */
            end = find_char_or_comment(start, '=');
            if (*end != '=') {
                end = find_char_or_comment(start, ':');
            }
            if (*end == '=' || *end == ':') {
                *end = '\0';
                name = rstrip(start);
                value = lskip(end + 1);
                end = find_char_or_comment(value, '\0');
                if (*end == ';')
                    *end = '\0';
                rstrip(value);

                /* Valid name[=:]value pair found, call handler */
                strncpy0(prev_name, name, sizeof(prev_name));
                if (!handler(user, section, name, value) && !error) {
                    error = lineno;
                }
            }
            else if (!error) {
                /* No '=' or ':' found on name[=:]value line */
                error = lineno;
            }
        }
    }

INIPARSE_EXIT:
    if (line) 
        sp5kFree(line);
    if ( error ) {
        printf("configure file ERROR line %d\n",error);
    }
    return SUCCESS;
}


/** 
 * @brief file handle to check INI file
 *  
 * @param[in] fd 
 * @param[in] handler 
 * @param[in] user 
 * @return SUCCESS / FAIL
 */ 
static UINT32 configure(void* user, const char* section, const char* name,
                  const char* value)
{
/* 
INI file content 
[LOGCONFIG]
BUFFER_SIZE=1048576     ;1024KB 
LOG_DISK=1              ;1->SD Card  or 0->NAND 
POWEROFF=1              ;0->REBOOT or 1->POWEROFF 
POWERKEY_DETECT = 1     ;0: Disable Power Key Detect , 1: Enable Power Key Detect
LOGFILE = TEST.LOG      ;8.3 format file name  
*/
#if 1
    static char prev_section[50] = "";

    if (strcmp(section, prev_section)) {
        strncpy(prev_section, section, sizeof(prev_section));
        prev_section[sizeof(prev_section) - 1] = '\0';
    }
    PROF_LOG_PRINTF(0,"[%s]%d %s = %s\n",__FUNCTION__,__LINE__, name, value);
    LOGDBG_PRINT("[%s]%d %s = %s\n",__FUNCTION__,__LINE__, name, value);
#endif
    if (MATCH("LOGCONFIG", "BUFFER_SIZE")) {
        logBufferSize = atoi(value);
    } else if (MATCH("LOGCONFIG", "LOG_DISK")) {
        /* Add in the future */
    } else if (MATCH("LOGCONFIG", "POWEROFF")) {
        if ( atoi(value) == 1 ) 
            bASSERTPowerOff = 1 ;
        else
            bASSERTPowerOff = 0 ;
    } else if (MATCH("LOGCONFIG", "POWERKEY_DETECT")) {
        if ( atoi(value) == 1 ) 
            isKeyDetectRunning = 0 ;
        else
            isKeyDetectRunning = -1 ;
    } else if (MATCH("LOGCONFIG", "LOGFILE")) {
        /* Add in the future */
    #if APWDG_UT
    } else if (MATCH("LOGCONFIG", "TESTCASE")) {
        /* Add in the future */
        testCase = atoi(value) ;
    #endif
    } else {
        return 0;  /* unknown section/name, error */
    }
    return 1;

}



/** 
 * @brief callback to decide store or not which is called before
 *  	  log store start return 1 to store log, return 0 no
 *  	  store
 * @param[in] mode the dump mode, 0=file create, 1=file append, 
 *  	 2=file close
 * @return 1 or 0
 * @author chris
 */ 
static UINT32 
appLogMsgToStorageCallback(
	UINT32 mode
)
{
    tmx_t rtc;

    printf("%s 0x00C0(0x%x)\n",__FUNCTION__,*((volatile UINT8 *)0xB00000C0));
    printf("%s 0x00C1(0x%x)\n",__FUNCTION__,*((volatile UINT8 *)0xB00000C1));

    sp5kRtcDateTimeGet( SP5K_DATE_TIME_OPTION | SP5K_DATE_TIME_BLOCK_WAIT , &rtc);
    #if MP_RELEASE
	sioCtrl(3);
	isEnableLog = 0;
	#endif
    /* log crash time to log file */
    printf("%s Enable Option(%d) ASSERT Time - %d/%d/%d %d:%d:%d\n",__FUNCTION__,isEnableLog,
           rtc.tmx_year+1900,rtc.tmx_mon,rtc.tmx_mday,rtc.tmx_hour,rtc.tmx_min,rtc.tmx_sec);
    return isEnableLog ;
}


#if RTC_WAKEUP_SUPPORT
static void 
appRTCWakeUp(
   void
)
{
 	tmx_t rtc;
 	UINT32 r,reliableCode;	
 	r = sp5kRtcReliableGet(&reliableCode);
     sp5kRtcDateTimeGet(SP5K_DATE_TIME_OPTION | SP5K_DATE_TIME_BLOCK_WAIT, &rtc);
    rtc.tmx_sec += 5;
 	if (rtc.tmx_sec >= 60) {
    		rtc.tmx_sec -= 60;
 		rtc.tmx_min++;
 		if (rtc.tmx_min >= 59) {
    			rtc.tmx_min -= 59;
 			rtc.tmx_hour++;
    		}
 	}
    sp5kRtcDateTimeSet(SP5K_WAKEUP_OPTION | SP5K_DATE_TIME_BLOCK_WAIT, &rtc);
}
#endif

/** 
 * @brief callback for Os dead happend.
 * @author chris
 */ 
static void
appHostOsDeadCallback( 
   void
)
{
    if ( !bASSERTPowerOff ) {
        /* use RTC wakeup to reboot */
        #if RTC_WAKEUP_SUPPORT
            appRTCWakeUp();
        #endif
    }
    /* Powser off */
    sp5kPowerCtrl(SP5K_POWER_OFF, (UINT32)-1);

}


/** 
 * @brief callback to check power key long pressed. 
 * @author chris
 */ 
static void
appPowerKeyPressCallback(
   void
)
{
    UINT32 isPowerKeyPress = 0;
	
	sioCtrl(0); 		
    sp5kGpioRead(SP5K_GPIO_GRP_POCU,SP5K_PWR_SRC_0,&isPowerKeyPress);
    if ( isPowerKeyPress ) {
        printf("Long Pressed Power Key %d , %d\n",sp5kOsTimeGet(),osTimerTick_PowerKeyPress);
        isKeyDetectRunning = isKeyDetectRunning + 1 ;
        /* if ( isKeyDetectRunning > POWER_KEY_LONG_PRESSED_REBOOT_COUNT ) { */
        if ( ( sp5kOsTimeGet() - osTimerTick_PowerKeyPress ) > 1000 ) {
            bASSERTPowerOff = 0 ;
            PROF_LOG_PRINTF(0,"Long Pressed Power Key and Power OFF / Reboot\n");
            printf("Long Pressed Power Key and Power OFF / Reboot\n");
            PROF_LOG_PRINTF(0,"Wait for power key release.......\n");
            printf("Wait for power key release.......\n");
            while ( !isPowerKeyPress) {
                sp5kGpioRead(SP5K_GPIO_GRP_POCU,SP5K_PWR_SRC_0,&isPowerKeyPress);
            }
            ASSERT(0);
        }
        sp5kTimerUsOneShotReg(0, DETECT_POWER_KEY_PRESSED_INTERVAL , appPowerKeyPressCallback);
    } else {
        printf("Long Pressed Power Key %d , %d\n",sp5kOsTimeGet(),osTimerTick_PowerKeyPress);
        /* if ( isKeyDetectRunning > POWER_KEY_LONG_PRESSED_POWEROFF_COUNT ) { */
        if ( ( sp5kOsTimeGet() - osTimerTick_PowerKeyPress ) > 500 ) {
            bASSERTPowerOff = 1 ;
            PROF_LOG_PRINTF(0,"Long Pressed Power Key and Power OFF\n");
            printf("Long Pressed Power Key and Power OFF\n");
            ASSERT(0);
        }

        isKeyDetectRunning = 0 ;
    }
}


/** 
 * @brief callback to check host watchdog working or not. 
 * @author chris
 */ 
static void
appLogMsgWatchDogTimerCallback(
   void
)
{
    UINT32 isPowerKeyPress = 0;
    if ( isKeyDetectRunning == 0 ) {
        sp5kGpioRead(SP5K_GPIO_GRP_POCU,SP5K_PWR_SRC_0,&isPowerKeyPress);
        if ( isPowerKeyPress ) {
            sp5kTimerUsOneShotReg(0, DETECT_POWER_KEY_PRESSED_INTERVAL , appPowerKeyPressCallback);
            isKeyDetectRunning = 1 ;
            osTimerTick_PowerKeyPress = sp5kOsTimeGet();
        }
    }
	if (( sp5kOsTimeGet() - osTimerTick ) > LOG_WATCHDOG_ASSERT ) {
		
        printf("!!!!!!!!!!!! app watchdog timeout !!!!!!!!!!!!\n");

		#if APWDG_UT
		if ( testHWWatchdog != 0) return;
		#endif

		//ASSERT(0);
		/* Never executed here below */
	}
    /* use one shot callback */
	sp5kTimerUsOneShotReg(0, LOG_WATCHDOG_TIMEOUT, appLogMsgWatchDogTimerCallback);
}


/** 
 * @brief host app watchdog thread. 
 * @param[in] timeval thread sleep time interval
 * @author chris
 */ 

static void 
appLogMsgWatchDogThread(
   UINT32 timeval
)
{
	UINT32 t = timeval / 2048 ;
	while (1) {

		#if APWDG_UT
            /* unit test */
		    sp5kOsMutexGet(&muxWDGTest, TX_WAIT_FOREVER);
		#endif

        osTimerTick = sp5kOsTimeGet();

		#if APWDG_UT
            /* unit test */
		    sp5kOsMutexPut(&muxWDGTest);
		#endif

        ros_thread_sleep(t);
	} 
}

/** 
 * @brief check SD card must have a file named "SD.LOG" or 
 *        "NAND.LOG" in root directory.
 * @return SUCCESS or FAIL
 * @author chris
 */ 

UINT32
appInitLogMsgToStorage(
	void
)
{
    UINT32 fd;
	UINT32 devId;
    UINT32 ret = FAIL ;
    
    
	if ( logDevId != 0xFFFFFFFF ) {
		/* Already logging */
		return SUCCESS;
	}

	fd = sp5kFsFileOpen(LOG_SIGNATURE_FILE_NAME_SD, SP5K_FS_OPEN_RDONLY); 

	LOGDBG_PRINT("[%s] line %d: open file %s ", __FUNCTION__, 
		__LINE__, LOG_SIGNATURE_FILE_NAME_SD);
	devId = SP5K_DRIVE_SD ;
#if !CAM_TYPE_CVR
	if ( fd == 0 ) {
		LOGDBG_PRINT("FAIL\n");
		fd = sp5kFsFileOpen(LOG_SIGNATURE_FILE_NAME_NAND, SP5K_FS_OPEN_RDONLY); 
		LOGDBG_PRINT("[%s] line %d: open file %s ", __FUNCTION__, 
			__LINE__, LOG_SIGNATURE_FILE_NAME_NAND);
		#if !CAM_TYPE_CVR
		if ( fd == 0 ) {
			LOGDBG_PRINT("FAIL\n");
            fd = -1 ;
            #if 0
                #ifndef _EVB_HW_
                    goto ALWAYS_ENABLE;
                #endif
            #endif
			return FAIL;
		}
		devId = SP5K_DRIVE_NAND ;
		#endif
	}
#endif
	LOGDBG_PRINT("SUCCESS\n");
	if(fd){
		ret = iniParseFile(&fd, configure, NULL);

		sp5kFsFileClose(fd);
		fd = 0 ;
		logfname[0] = ( devId == SP5K_DRIVE_NAND ? 'C' : 'D' );
		logDevId = devId ;
	}
	osDeadHostCbSet(appHostOsDeadCallback);
#if CAM_TYPE_CVR
	/* CVR project always enable reboot after program dead */
	bASSERTPowerOff = 0;
	#if HW_PRJ_6330
	isKeyDetectRunning = 0;
	#endif
#endif
#if MP_RELEASE
	dbgPrintLogInit(logBufferSize);
	dbgPrintLogConfirmCallbackSet(appLogMsgToStorageCallback);
	dbgPrintLogFnameSet(logfname);
	LOGDBG_PRINT("%s Logging File Name (%s)\n",__FUNCTION__,logfname);
#endif

/*ALWAYS_ENABLE:*/

	/* Disable base fw watchdog */
	sp5kSystemCfgSet(SP5K_SYS_HALT_OPERAT_CFG, SP5K_SYS_HALT_DEBUG,0,0);
    
	
    /* Enable Hardward watchdog , longer than software watchdog */
    if ( bASSERTPowerOff == 1 )
        sp5kSystemCfgSet(SP5K_SYS_HALT_OPERAT_CFG, SP5K_SYS_HALT_IMMEDIATE_POWEROFF,LOG_WATCHDOG_TIMEOUT*6,0);
    else 
        sp5kSystemCfgSet(SP5K_SYS_HALT_OPERAT_CFG, SP5K_SYS_HALT_DELAY_REBOOT,LOG_WATCHDOG_TIMEOUT*6,0);
	
	sp5kSystemCfgSet(SP5K_SYS_HALT_OPERAT_CFG, SP5K_SYS_HALT_DEBUG,0,0);

    /* Create timer thread for software watchwod */
    if (thdAppWatchdog == 0 ) {
    	#if 1
            /* use one shot callback */
        	ret = sp5kTimerUsOneShotReg(0, LOG_WATCHDOG_TIMEOUT, appLogMsgWatchDogTimerCallback);
        	if ( ret == FAIL ) {
        		LOGDBG_PRINT("sp5kTimerUsOneShotReg FAIL\n");
        		return FAIL;
        	}
        	LOGDBG_PRINT("sp5kTimerUsOneShotReg SUCCESS\n");
    	#endif

    	#if 0
        	ret = sp5kTimerIsrReg(0, LOG_WATCHDOG_TIMEOUT / 1024 , appLogMsgWatchDogTimerCallback); 
        	if ( ret == FAIL ) {
        		LOGDBG_PRINT("sp5kTimerIsrReg FAIL\n");
        		return FAIL;
        	}
        	LOGDBG_PRINT("sp5kTimerIsrReg SUCCESS\n");
    	#endif

    	#if 0
        	ret = appTimerIsrReg(appLogMsgWatchDogTimerCallback,LOG_WATCHDOG_TIMEOUT / 1024);

        	if ( ret == TIMER_NULL ) {
        		LOGDBG_PRINT("appTimerIsrReg FAIL\n");
        		return FAIL;
        	}
        	LOGDBG_PRINT("appTimerIsrReg SUCCESS\n");
    	#endif

        thdAppWatchdog = sp5kOsThreadCreate("appWatchDogThread", appLogMsgWatchDogThread,
                                                 LOG_WATCHDOG_TIMEOUT , TX_PRIORITY_14, TX_PRIORITY_14,
                                                 0, TX_AUTO_START );           
        DBG_ASSERT(thdAppWatchdog);
        if ( !thdAppWatchdog ) {
            LOGDBG_PRINT("Thread create FAIL\n");
        }
    }

    if ( logDevId == SP5K_DRIVE_NAND ) {
        isEnableLog = 1 ; /* Logging */
        sioCtrl(3);
    } else if (!fd) {
        isEnableLog = 1 ; /* Logging */
        sioCtrl(3);
    }

    printf("isKeyDetectRunning 0x%x\n",isKeyDetectRunning);
    printf("bASSERTPowerOff 0x%x\n",bASSERTPowerOff);
    
#if APWDG_UT
    sp5kOsMutexCreate((TX_MUTEX *)&muxWDGTest, (char *)"wdgTest", TX_INHERIT);
    #ifdef WDG_BACKGRUND_TEST
    _watchdog_background_test();
    #endif
#endif

	return SUCCESS;
}



/**
 * @brief initization log debug message to SD card and check the
 *        sd card plug in/out message.
 * @param[in] msg receive message
 * @param[in] param receive parameter
 * @return SUCCESS or FAIL
 */

UINT32
appLogMsgToStorage(
	UINT32 msg,
	UINT32 param
)
{
	UINT32 ret = FAIL ;
	switch( msg )
	{
	case SP5K_MSG_DISK_INSERT:
		LOGDBG_PRINT("%s[%s] SP5K_MSG_DISK_INSERT param %d\n",__FILE__,__FUNCTION__,param);
		break;
	case SP5K_MSG_DISK_REMOVAL:
		LOGDBG_PRINT("%s[%s] SP5K_MSG_DISK_REMOVAL param %d\n",__FILE__,__FUNCTION__,param);
		if ( logDevId == param ) {
			/* Need to check for SD card plug-out */
			isEnableLog = 0 ;
		}
		break;
	case SP5K_MSG_DISK_MOUNT_COMPLETE:
		LOGDBG_PRINT("%s[%s] SP5K_MSG_DISK_MOUNT_COMPLETE param %d\n",__FILE__,__FUNCTION__,param);
		if ( ( param == SP5K_DRIVE_NAND ) || 
		     ( param == SP5K_DRIVE_SD   ) ) {
			ret = appInitLogMsgToStorage();
			if (ret == SUCCESS) {
				if ( logDevId == param ) {
					/* Need to check for SD card plug-in */
					isEnableLog = 1 ;
				}
			}
		}
		break;
	default:
		break;
	}

	return SUCCESS;
}


#if APWDG_UT
#ifdef WDG_BACKGRUND_TEST
static void _watchdog_background_test_thread(
   UINT32 param
)
{
    UINT32 i = 0 ;


    for (i=0; i < 20 ; i++ )
        ros_thread_sleep(1000);
    printf("_watchdog_background_test_thread testCase %d\n",param );
    PROF_LOG_PRINTF(0,"_watchdog_cmd_crash_test testCase %d\n",param);
    switch ( param ) {
    case 0:
        _watchdog_cmd_isr_test(2);
        ros_thread_sleep(1000);
        _watchdog_cmd_crash_test(0);
        break;
    case 1:
        _watchdog_cmd_isr_test(3);
        ros_thread_sleep(1000);
        _watchdog_cmd_crash_test(0);
        break;
    case 2:
        _watchdog_cmd_isr_test(4);
        ros_thread_sleep(1000);
        _watchdog_cmd_crash_test(0);
        break;

    case 3:
        _watchdog_cmd_isr_test(2);
        ros_thread_sleep(1000);
        _watchdog_cmd_crash_test(1);
        break;
    case 4:
        _watchdog_cmd_isr_test(3);
        ros_thread_sleep(1000);
        _watchdog_cmd_crash_test(1);
        break;
    case 5:
        _watchdog_cmd_isr_test(4);
        ros_thread_sleep(1000);
        _watchdog_cmd_crash_test(1);
        break;

    case 6:
        _watchdog_cmd_isr_test(2);
        ros_thread_sleep(1000);
        _watchdog_cmd_crash_test(2);
        break;
    case 7:
        _watchdog_cmd_isr_test(3);
        ros_thread_sleep(1000);
        _watchdog_cmd_crash_test(2);
        break;
    case 8:
        _watchdog_cmd_isr_test(4);
        ros_thread_sleep(1000);
        _watchdog_cmd_crash_test(2);
        break;

    case 9:
        _watchdog_cmd_isr_test(2);
        ros_thread_sleep(1000);
        _watchdog_cmd_crash_test(3);
        break;
    case 10:
        _watchdog_cmd_isr_test(3);
        ros_thread_sleep(1000);
        _watchdog_cmd_crash_test(3);
        break;
    case 11:
        _watchdog_cmd_isr_test(4);
        ros_thread_sleep(1000);
        _watchdog_cmd_crash_test(3);
        break;

    case 12:
        _watchdog_cmd_isr_test(2);
        ros_thread_sleep(1000);
        _watchdog_cmd_crash_test(4);
        break;
    case 13:
        _watchdog_cmd_isr_test(3);
        ros_thread_sleep(1000);
        _watchdog_cmd_crash_test(4);
        break;
    case 14:
        _watchdog_cmd_isr_test(4);
        ros_thread_sleep(1000);
        _watchdog_cmd_crash_test(4);
        break;

    case 15:
        _watchdog_cmd_isr_test(2);
        ros_thread_sleep(1000);
        _watchdog_cmd_crash_test(5);
        break;
    case 16:
        _watchdog_cmd_isr_test(3);
        ros_thread_sleep(1000);
        _watchdog_cmd_crash_test(5);
        break;
    case 17:
        _watchdog_cmd_isr_test(4);
        ros_thread_sleep(1000);
        _watchdog_cmd_crash_test(5);
        break;

    case 18:
        _watchdog_cmd_isr_test(3);
        ros_thread_sleep(1000);
        _watchdog_cmd_isr_test(8);
        ros_thread_sleep(1000);
        _watchdog_cmd_crash_test(5);
        break;
    case 19:
        _watchdog_cmd_isr_test(4);
        ros_thread_sleep(1000);
        _watchdog_cmd_isr_test(8);
        ros_thread_sleep(1000);
        _watchdog_cmd_crash_test(5);
        break;

    default:
        break;
    }


    while (1) {
        ros_thread_sleep(1000);
    }
}

static void 
_watchdog_background_test(
	void
)
{

    static TX_THREAD *thdBGTest = 0;
    printf("_watchdog_background_test testCase %d\n",testCase);
    PROF_LOG_PRINTF(0,"_watchdog_background_test testCase %d\n",testCase);
    thdBGTest = sp5kOsThreadCreate("appThreadBackGroundTest", _watchdog_background_test_thread,
										 testCase , TX_PRIORITY_14, TX_PRIORITY_14,
										 1, TX_AUTO_START );

}
#endif
/**
 * @brief command for unit test. Need to add cmdWatchDogInit(); 
 *        in customization\command.c to initialize command.
 */

static void _watchdog_thread_test(
   UINT32 param
)
{
	LOGDBG_PRINT("[%s] osTimerTick(%d)\n", __FUNCTION__,sp5kOsTimeGet());
	while (1) ;
}


#ifndef WDG_BACKGRUND_TEST
static void _watchdog_cmd_crash_test(
   int argc,
   char *argv[]
)
#else
static void _watchdog_cmd_crash_test(
   int mode
)
#endif
{

    UINT8 *ptr = sp5kMalloc(16);
	UINT32 *pinsn;
	void (*fp)(void);
    static TX_THREAD *thdTest = 0;

#ifndef WDG_BACKGRUND_TEST
	UINT32 mode = 0xFF ;
	if ( argc > 0 ) {
		mode = strtoul(argv[0], NULL, 0);
	}
#endif

    PROF_LOG_PRINTF(0,"_watchdog_cmd_crash_test mode %d\n",mode);
    printf("_watchdog_cmd_crash_test mode %d\n",mode);
	switch(mode) {
	case 0:
        printf("test mode %d\n",mode);
        mode = mode/divzero;
        mode = mode/0;
        mode = mode/0;
        mode = mode/0;
        break;
	case 1:
        printf("ptr 0x%x\n",ptr);
        *((UINT16 *)(ptr + 1)) = 0;
		break;
    case 2:
        printf("ptr 0x%x\n",ptr);
		mode = *((UINT32 *)(ptr + 1)) + mode;
        mode = *((UINT32 *)(ptr + 3)) + mode;
        mode = *((UINT16 *)(ptr + 1)) + mode;
        break;
	case 3:
		pinsn = (UINT32 *)ptr;
		*pinsn++ = 0x12345678;
		*pinsn++ = 0x90abcedf;
		*pinsn++ = 0xdeadbeef;
		*pinsn   = 0xbeefdead;
		fp = (void (*)(void))ptr;
		fp();
		break;
	case 4:
		*(UINT8 *)0 = 1;
		break;
	case 5:
		thdTest = sp5kOsThreadCreate("appThreadTest", _watchdog_thread_test,
										 0 , TX_PRIORITY_13, TX_PRIORITY_13,
										 1, TX_AUTO_START );
        DBG_ASSERT(thdTest);
		if ( !thdTest ) {
			LOGDBG_PRINT("Thread create FAIL\n");
		}
		ros_thread_sleep(100);
		break;
	case 6:
		sp5kOsThreadTerminate(thdTest);
		sp5kOsThreadDelete(thdTest); 
		break;
	default:
		printf("crash #(0-6)\n");
		break;
	}
	sp5kFree(ptr);
}


#ifndef WDG_BACKGRUND_TEST
static void _watchdog_cmd_isr_test(
   int argc,
   char *argv[]
)
#else
static void _watchdog_cmd_isr_test(
   int mode
)
#endif
{

    UINT32 isPlayKeyPowerOn = 0;

#ifndef WDG_BACKGRUND_TEST
    UINT32 mode = 0xFF ;
	if ( argc > 0 ) {
		mode = strtoul(argv[0], NULL, 0);
	}
#endif
    divzero = 0 ;
    PROF_LOG_PRINTF(0,"_watchdog_cmd_isr_test mode %d\n",mode);
    printf("_watchdog_cmd_isr_test mode %d\n",mode);
	switch(mode) {
    case 0:
        sp5kSystemCfgSet(SP5K_SYS_HALT_OPERAT_CFG, SP5K_SYS_HALT_IMMEDIATE_REBOOT,LOG_WATCHDOG_TIMEOUT*4,0);
        bASSERTPowerOff = 0 ;
        break;
    case 1:
        sp5kSystemCfgSet(SP5K_SYS_HALT_OPERAT_CFG, SP5K_SYS_HALT_GOTO_CMDLINE,LOG_WATCHDOG_TIMEOUT*4,0);
        bASSERTPowerOff = 0 ;
        break;
	case 2:
		sp5kSystemCfgSet(SP5K_SYS_HALT_OPERAT_CFG, SP5K_SYS_HALT_DEBUG,0,0);
        bASSERTPowerOff = 1 ;
		break;
    case 3:
        sp5kSystemCfgSet(SP5K_SYS_HALT_OPERAT_CFG, SP5K_SYS_HALT_DELAY_POWEROFF,LOG_WATCHDOG_TIMEOUT*4,0);
        bASSERTPowerOff = 1 ;
        break;
    case 4:
        sp5kSystemCfgSet(SP5K_SYS_HALT_OPERAT_CFG, SP5K_SYS_HALT_DELAY_REBOOT,LOG_WATCHDOG_TIMEOUT*4,0);
        bASSERTPowerOff = 0 ;
        break;
	case 5:
		sp5kOsMutexGet(&muxWDGTest, TX_WAIT_FOREVER);
		break;
	case 6:
		sp5kOsMutexPut(&muxWDGTest);
		break;
    case 7:
        while ( !isPlayKeyPowerOn) {
            sp5kGpioRead(SP5K_GPIO_GRP_POCU,SP5K_PWR_SRC_0,&isPlayKeyPowerOn);
            if ( isPlayKeyPowerOn == 1 ) {
                printf("!!!!!!!!!!!! Press Power On Key Press !!!!!!!!!!!!\n");
            }
        }
        while ( isPlayKeyPowerOn) {
            sp5kGpioRead(SP5K_GPIO_GRP_POCU,SP5K_PWR_SRC_0,&isPlayKeyPowerOn);
            if ( isPlayKeyPowerOn == 1 ) {
                printf("!!!!!!!!!!!! Press Power On Key Up !!!!!!!!!!!!\n");
            }
        }
        break;
    case 8:
        testHWWatchdog = 1;
        break;
    case 9:
        sp5kSystemCfgSet(SP5K_SYS_HALT_OPERAT_CFG, SP5K_SYS_HALT_IMMEDIATE_POWEROFF,LOG_WATCHDOG_TIMEOUT*4,0);
        bASSERTPowerOff = 1 ;
        break;
	default:
		printf("isr #(0)\n");
		break;
	}
	
}

static cmd_t subwatchdogcmds[] = {
    #ifndef WDG_BACKGRUND_TEST
	{"crash", _watchdog_cmd_crash_test, "watchdog crash test."},
	{"isr", _watchdog_cmd_isr_test, "watchdog isr test"},
    #endif
	{NULL,  NULL, NULL, NULL}
};

static void _watchdog_cmd_not_found(
   int argc,
   char *argv[]
)
{
	printf("available commands:\n");

	cmd_t *pcmd;
	const char *spaces = " : ";

	for (pcmd = subwatchdogcmds; pcmd->cmd; ++pcmd) {
		printf(" %s %s %s\n", pcmd->cmd, spaces , pcmd->phelp);
	}
	printf("\n");
}


static void _watchdog_cmd_main(
   int argc,
   char *argv[]
)
{
	static cmd_t *psubcmds = NULL;
    PROF_LOG_PRINTF(0,"_watchdog_cmd_isr_test\n");
	if (!psubcmds) {
		psubcmds = cmdSubBatchRegister(0, &subwatchdogcmds[0]);
	}

    cmdSubProcess(argc, argv, psubcmds, _watchdog_cmd_not_found);
}


static cmd_t watchdogcmds[] = {
	{ "watchdog", _watchdog_cmd_main, "Custom watchsog test command", NULL, },
	{ NULL      , NULL              , NULL                          , NULL, }, /* null terminator */
};


void
cmdWatchDogInit(
   void
)
{
	cmdBatchRegister(watchdogcmds);
	LOGDBG_PRINT("%s[%s] WatchDog Test CMD init done.\n",__FILE__,__FUNCTION__);
}
#endif

#else
/**
 * @brief initization log debug message to SD card and check the
 *        sd card plug in/out message.
 * @param[in] msg receive message
 * @param[in] param receive parameter
 * @return SUCCESS or FAIL
 */

UINT32
appLogMsgToStorage(
	UINT32 msg,
	UINT32 param
)
{
    /* compatibility for SPCA5330 basefw */
	return SUCCESS;
}

#endif

