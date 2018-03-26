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
 
/**
 * @file      app_menu_disp.h
 * @brief     Brief
 * @author    Phil Lin
 * @since     2012-10-12
 * @date      2012-10-12
*/

#ifndef __APP_MENU_DISP_H__
#define __APP_MENU_DISP_H__

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "app_menu.h" 
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
 
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
 
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
 
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
 
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
void menuProcDisp (void);
void menuProcDisp2Layer (void);
void menuProcDispYesNo(UINT16 idx);
void menuEVDisp(void);
void menuRTCDisp(void);
#if CAM_TYPE_CVR
void menuWIFIPasswordDisp(void);
#endif
void menuProcDispPalette(UINT16 msg);
#if HDMI_MENU_BY_JPG
UINT32 menuHDMIPIPInit(void);
void menuDlgShow(UINT32 num);
void menuDlgBarShow(UINT32 dlgNum,UINT32 item);
void menuHDMIClean(void);
#endif
#endif  /* __APP_MENU_DISP_H__ */

