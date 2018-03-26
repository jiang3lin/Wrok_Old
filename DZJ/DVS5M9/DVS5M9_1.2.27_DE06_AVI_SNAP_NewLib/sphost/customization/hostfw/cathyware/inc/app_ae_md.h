
#ifndef APP_AE_MD_H
#define APP_AE_MD_H

#include "app_3aalg.h" 


/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
typedef enum{
	APP_AE_MD_EN,
	APP_AE_MD_AVG_LV_THR,
	APP_AE_MD_BLOCK_LV_THR,
	APP_AE_MD_SENSITIVITY,
	APP_AE_MD_START_LEARN,
	APP_AE_MD_RESET,
}aeMDParaSel_e;

 typedef enum{
	 APP_AE_MD_SENS_LOW,
	 APP_AE_MD_SENS_MIDDLE,
	 APP_AE_MD_SENS_HIGH,
 }aeMDSens_e;


/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/
//UINT8 appAeMDAeInfoUpdate(aaaProcInfo_t *pinfo,UINT8 newInfo);
UINT8 appAeMDAeInfoUpdate(void);
UINT8 appAeMDParaCfg(UINT32 selector , UINT32 param);
void appAeMDInit(void);
void appAeMDSvc(void);
UINT8 appAeMDAeInfoAvailableGet(void);


#endif  /* APP_AE_MD_H */

