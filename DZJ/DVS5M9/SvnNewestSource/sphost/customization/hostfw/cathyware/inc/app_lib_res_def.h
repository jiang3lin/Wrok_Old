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
#ifndef APP_LIB_RES_DEF_H
#define APP_LIB_RES_DEF_H

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
UINT8 * appRootGet(UINT8 *input);
#define RootGet(x) appRootGet(x)

/*---------------------A: B:----------------------------------------*/
#define RO_RES_PATH             "A:\\RO_RES\\"
#define USER_RES_PATH           "B:\\USER_RES\\"

/*---------------------A:\\RO_RES\\------------------------------*/
/*AWB*/
#define RES_AWB_CTT             (RO_RES_PATH"AWBCTT.BIN")
#define RES_AWB_CFG             (RO_RES_PATH"AWBCFG.BIN")

/*AE*/
#define RES_AE_AEALG            RootGet(RO_RES_PATH"AEALG.BIN")

/*DIQ*/
#define RES_DIQ_WDRFT           RootGet(RO_RES_PATH"WDRFT_DQ.BIN")
 
/*---------------------------A-IQ-----------------------------------*/
#define IQ_DIR                  RO_RES_PATH"IQ\\"

/*IQ*/
#define FIF_APPIQ_DATA          RootGet(IQ_DIR"APPIQ.BIN")
#define FIF_LENSIQ_DATA         RootGet(IQ_DIR"LENSIQ.BIN")
#define RES_IQ_HUECORR          RootGet(IQ_DIR"HUECORR.BIN")
#define RES_IQ_SKETCH           RootGet(IQ_DIR"ESKETCH.BIN")
#define RES_IQ_YGAMMA           RootGet(IQ_DIR"YGAMMAS.BIN")
#define RES_IQ_LSC              RootGet(IQ_DIR"LSC.BIN")
#define RES_IQ_GGGAMMAN         RootGet(IQ_DIR"GGGAMMAN.BIN")
#define RES_IQ_RBGAMMA          RootGet(IQ_DIR"RBGAMMAN.BIN")
#define RES_IQ_WTBL             RootGet(IQ_DIR"WTBL.BIN")
#define RES_IQ_DIVTBL           RootGet(IQ_DIR"DIVTBL.BIN")

/*CIQ*/
#define RES_IQ_CIQ              RootGet(IQ_DIR"CIQ.BIN")

/*DIQ*/
#define RES_IQ_GAMMAD           RootGet(IQ_DIR"YGAMMA.BIN")
#define RES_IQ_GAMMAL           RootGet(IQ_DIR"YGAMMAD.BIN")
#define RES_IQ_GAMMAN           RootGet(IQ_DIR"YGAMMAN.BIN")

#define RES_IQ_HSC              RootGet(IQ_DIR"HSC")
#define RES_IQ_YUVWDR           RootGet(IQ_DIR"YUVWDR.BIN")


#endif /*APP_LIB_RES_DEF_H*/

