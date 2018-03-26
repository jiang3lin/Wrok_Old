#ifndef _APP_GPIO_HW_PRJ_6330__H
#define _APP_GPIO_HW_PRJ_6330__H
/* SPCA5330B */

/*  IO DEFINE  */

/* LCD */
#define IN_PWR_BACKLIGHT		1
#define GRP_PWR_BACKLIGHT              	SP5K_GPIO_GRP_NO_USE
#define OUT_PWR_BACKLIGHT               (const UINT32)(dummyio)
#define SET_PWR_BACKLIGHT(en)          
#define POL_PWR_BACKLIGHT              	SP5K_GPIO_ACTIVE_HIGH
#define CFG_PWR_BACKLIGHT               PIN_0

#define GRP_PWR_LCD                    	SP5K_GPIO_GRP_NO_USE
#define OUT_PWR_LCD                     (const UINT32)(dummyio)
#define SET_PWR_LCD(en)                
#define POL_PWR_LCD                    	SP5K_GPIO_ACTIVE_HIGH
#define CFG_PWR_LCD                     PIN_0

#define GRP_PWR_BL_CTL                 	SP5K_GPIO_GRP_NO_USE
#define OUT_PWR_BL_CTL                  (const UINT32)(dummyio)
#define SET_PWR_BL_CTL(en)             
#define POL_PWR_BL_CTL                 	SP5K_GPIO_ACTIVE_HIGH
#define CFG_PWR_BL_CTL                  PIN_0

#define GRP_LCD_RESET                  	SP5K_GPIO_GRP_NO_USE
#define OUT_LCD_RESET                   (const UINT32)(dummyio)
#define SET_LCD_RESET(en)              
#define POL_LCD_RESET                  	SP5K_GPIO_ACTIVE_HIGH
#define CFG_LCD_RESET                   PIN_0


/* TV */
#define GRP_TV_IN                      	SP5K_GPIO_GRP_FMH
#define IN_TV_IN                       	(VALI_FmGPIO(bit_48))
#define POL_TV_IN                      	SP5K_GPIO_ACTIVE_HIGH
#define CFG_TV_IN                       PIN_48


/* FLASH & STROBE */
#define IN_FLASH_READY  		1 /* always ready */
#define GRP_FLASH_TRIG                 	SP5K_GPIO_GRP_NO_USE
#define OUT_FLASH_TRIG                  (const UINT32)(dummyio)
#define SET_FLASH_TRIG(en)             
#define POL_FLASH_TRIG                 	SP5K_GPIO_ACTIVE_HIGH
#define CFG_FLASH_TRIG                  PIN_0

#define OUT_FLASH_CHARGE		dummyioFlashCharge
#define GRP_PWR_FLASH                  	SP5K_GPIO_GRP_NO_USE
#define OUT_PWR_FLASH                   (const UINT32)(dummyio)
#define SET_PWR_FLASH(en)              
#define POL_PWR_FLASH                  	SP5K_GPIO_ACTIVE_HIGH
#define CFG_PWR_FLASH                   PIN_0


/* LENS */

#define OUT_PWR_ZOOM 			dummyio
#define OUT_PWR_FOCUS 			dummyio
#define OUT_SHUTTER_N 			dummyio
#define OUT_SHUTTER_P 			dummyio	

#define GRP_ZOOM_HP                    	SP5K_GPIO_GRP_NO_USE
#define IN_ZOOM_HP                     	0
#define POL_ZOOM_HP                    	SP5K_GPIO_ACTIVE_HIGH
#define CFG_ZOOM_HP                     PIN_0

#define GRP_ZOOM_CLK                   	SP5K_GPIO_GRP_NO_USE
#define IN_ZOOM_CLK                    	0
#define POL_ZOOM_CLK                   	SP5K_GPIO_ACTIVE_HIGH
#define CFG_ZOOM_CLK                    PIN_0

#define GRP_FOCUS_HP                   	SP5K_GPIO_GRP_NO_USE
#define IN_FOCUS_HP                    	0
#define POL_FOCUS_HP                   	SP5K_GPIO_ACTIVE_HIGH
#define CFG_FOCUS_HP                    PIN_0



#define GRP_MOT_SCLK                   	SP5K_GPIO_GRP_NO_USE
#define OUT_MOT_SCLK                    (const UINT32)(dummyio)
#define SET_MOT_SCLK(en)               
#define POL_MOT_SCLK                   	SP5K_GPIO_ACTIVE_HIGH
#define CFG_MOT_SCLK                    PIN_0

#define GRP_MOT_SDAT                   	SP5K_GPIO_GRP_NO_USE
#define OUT_MOT_SDAT                    (const UINT32)(dummyio)
#define SET_MOT_SDAT(en)               
#define POL_MOT_SDAT                   	SP5K_GPIO_ACTIVE_HIGH
#define CFG_MOT_SDAT                    PIN_0

#define GRP_MOT_SLD                    	SP5K_GPIO_GRP_NO_USE
#define OUT_MOT_SLD                     (const UINT32)(dummyio)
#define SET_MOT_SLD(en)                
#define POL_MOT_SLD                    	SP5K_GPIO_ACTIVE_HIGH
#define CFG_MOT_SLD                     PIN_0

#define GRP_IN1                        	SP5K_GPIO_GRP_NO_USE
#define OUT_IN1                         (const UINT32)(dummyio)
#define SET_IN1(en)                    
#define POL_IN1                        	SP5K_GPIO_ACTIVE_HIGH
#define CFG_IN1                         PIN_0

#define GRP_IN2                        	SP5K_GPIO_GRP_NO_USE
#define OUT_IN2                         (const UINT32)(dummyio)
#define SET_IN2(en)                    
#define POL_IN2                        	SP5K_GPIO_ACTIVE_HIGH
#define CFG_IN2                         PIN_0

#define GRP_IN3                        	SP5K_GPIO_GRP_NO_USE
#define OUT_IN3                         (const UINT32)(dummyio)
#define SET_IN3(en)                    
#define POL_IN3                        	SP5K_GPIO_ACTIVE_HIGH
#define CFG_IN3                         PIN_0

#define GRP_IN4                        	SP5K_GPIO_GRP_NO_USE
#define OUT_IN4                         (const UINT32)(dummyio)
#define SET_IN4(en)                    
#define POL_IN4                        	SP5K_GPIO_ACTIVE_HIGH
#define CFG_IN4                         PIN_0

#define GRP_IN5                        	SP5K_GPIO_GRP_NO_USE
#define OUT_IN5                         (const UINT32)(dummyio)
#define SET_IN5(en)                    
#define POL_IN5                        	SP5K_GPIO_ACTIVE_HIGH
#define CFG_IN5                         PIN_0

#define GRP_IN6                        	SP5K_GPIO_GRP_NO_USE
#define OUT_IN6                         (const UINT32)(dummyio)
#define SET_IN6(en)                    
#define POL_IN6                        	SP5K_GPIO_ACTIVE_HIGH
#define CFG_IN6                         PIN_0

#define GRP_IN7                        	SP5K_GPIO_GRP_NO_USE
#define OUT_IN7                         (const UINT32)(dummyio)
#define SET_IN7(en)                    
#define POL_IN7                        	SP5K_GPIO_ACTIVE_HIGH
#define CFG_IN7                         PIN_0

#define GRP_IN8                        	SP5K_GPIO_GRP_NO_USE
#define OUT_IN8                         (const UINT32)(dummyio)
#define SET_IN8(en)                    
#define POL_IN8                        	SP5K_GPIO_ACTIVE_HIGH
#define CFG_IN8                         PIN_0

#define GRP_IN12                       	SP5K_GPIO_GRP_NO_USE
#define OUT_IN12                        (const UINT32)(dummyio)
#define SET_IN12(en)                   
#define POL_IN12                       	SP5K_GPIO_ACTIVE_HIGH
#define CFG_IN12                        PIN_0

#define GRP_IN13                       	SP5K_GPIO_GRP_NO_USE
#define OUT_IN13                        (const UINT32)(dummyio)
#define SET_IN13(en)                   
#define POL_IN13                       	SP5K_GPIO_ACTIVE_HIGH
#define CFG_IN13                        PIN_0

#define GRP_IN14                       	SP5K_GPIO_GRP_NO_USE
#define OUT_IN14                        (const UINT32)(dummyio)
#define SET_IN14(en)                   
#define POL_IN14                       	SP5K_GPIO_ACTIVE_HIGH
#define CFG_IN14                        PIN_0

#define GRP_IN15                       	SP5K_GPIO_GRP_NO_USE
#define OUT_IN15                        (const UINT32)(dummyio)
#define SET_IN15(en)                   
#define POL_IN15                       	SP5K_GPIO_ACTIVE_HIGH
#define CFG_IN15                        PIN_0

#define GRP_IRIS_AP                    	SP5K_GPIO_GRP_NO_USE
#define OUT_IRIS_AP                     (const UINT32)(dummyio)
#define SET_IRIS_AP(en)                
#define POL_IRIS_AP                    	SP5K_GPIO_ACTIVE_HIGH
#define CFG_IRIS_AP                     PIN_0

#define GRP_IRIS_AN                    	SP5K_GPIO_GRP_NO_USE
#define OUT_IRIS_AN                     (const UINT32)(dummyio)
#define SET_IRIS_AN(en)                
#define POL_IRIS_AN                    	SP5K_GPIO_ACTIVE_HIGH
#define CFG_IRIS_AN                     PIN_0


#define GRP_MOT_RESET                  	SP5K_GPIO_GRP_NO_USE
#define OUT_MOT_RESET                   (const UINT32)(dummyio)
#define SET_MOT_RESET(en)              
#define POL_MOT_RESET                  	SP5K_GPIO_ACTIVE_HIGH
#define CFG_MOT_RESET                   PIN_0


/* LED */
#define GRP_LED_POWER                  	SP5K_GPIO_GRP_NO_USE
#define OUT_LED_POWER                   (const UINT32)(dummyio)
#define SET_LED_POWER(en)              
#define POL_LED_POWER                  	SP5K_GPIO_ACTIVE_HIGH
#define CFG_LED_POWER                   PIN_0

#define GRP_LED_FLASH                  	SP5K_GPIO_GRP_NO_USE
#define OUT_LED_FLASH                   (const UINT32)(dummyio)
#define SET_LED_FLASH(en)              
#define POL_LED_FLASH                  	SP5K_GPIO_ACTIVE_HIGH
#define CFG_LED_FLASH                   PIN_0

#define GRP_LED_SELFTIMER              	SP5K_GPIO_GRP_NO_USE
#define OUT_LED_SELFTIMER               (const UINT32)(dummyio)
#define SET_LED_SELFTIMER(en)          
#define POL_LED_SELFTIMER              	SP5K_GPIO_ACTIVE_HIGH
#define CFG_LED_SELFTIMER               PIN_0

#define GRP_LED_AF_LAMP                	SP5K_GPIO_GRP_NO_USE
#define OUT_LED_AF_LAMP                 (const UINT32)(dummyio)
#define SET_LED_AF_LAMP(en)            
#define POL_LED_AF_LAMP                	SP5K_GPIO_ACTIVE_HIGH
#define CFG_LED_AF_LAMP                 PIN_0


/* SD Card */
#define IN_SD_CARD_WP			0
#define GRP_SD_CARD_DETECT             	SP5K_GPIO_GRP_FMH
#define IN_SD_CARD_DETECT              	(!VALI_FmGPIO(bit_46))
#define POL_SD_CARD_DETECT             	SP5K_GPIO_ACTIVE_LOW
#define CFG_SD_CARD_DETECT              PIN_46



/* BUTTON KEY */
#define GRP_BUTTON_POWER               	SP5K_GPIO_GRP_POCU
#define IN_BUTTON_POWER                	(!VALI_PocuGPIO(bit_0))
#define POL_BUTTON_POWER               	SP5K_GPIO_ACTIVE_LOW
#define CFG_BUTTON_POWER                PIN_0

#define GRP_BUTTON_TELE                	SP5K_GPIO_GRP_GEN
#define IN_BUTTON_TELE                 	(!VALI_GenGPIO(bit_11))
#define POL_BUTTON_TELE                	SP5K_GPIO_ACTIVE_LOW
#define CFG_BUTTON_TELE                 PIN_11

#define GRP_BUTTON_WIDE                	SP5K_GPIO_GRP_GEN
#define IN_BUTTON_WIDE                 	(!VALI_GenGPIO(bit_12))
#define POL_BUTTON_WIDE                	SP5K_GPIO_ACTIVE_LOW
#define CFG_BUTTON_WIDE                 PIN_12

#define GRP_BUTTON_SNAP1               	SP5K_GPIO_GRP_GEN
#define IN_BUTTON_SNAP1                	(!VALI_GenGPIO(bit_10))
#define POL_BUTTON_SNAP1               	SP5K_GPIO_ACTIVE_LOW
#define CFG_BUTTON_SNAP1                PIN_10

#define GRP_BUTTON_SNAP2               	SP5K_GPIO_GRP_GEN
#define IN_BUTTON_SNAP2                	(!VALI_GenGPIO(bit_8))
#define POL_BUTTON_SNAP2               	SP5K_GPIO_ACTIVE_LOW
#define CFG_BUTTON_SNAP2                PIN_8

#define GRP_BUTTON_PLAY                	SP5K_GPIO_GRP_GEN
#define IN_BUTTON_PLAY                 	(VALI_GenGPIO(bit_3))
#define POL_BUTTON_PLAY                	SP5K_GPIO_ACTIVE_HIGH
#define CFG_BUTTON_PLAY                 PIN_3

/* SPEAKER */

/* USB */
#define GRP_USB_DETECT                 	SP5K_GPIO_GRP_DISP
#define IN_USB_DETECT                  	(VALI_DispGPIO(bit_17))
#define POL_USB_DETECT                 	SP5K_GPIO_ACTIVE_HIGH
#define CFG_USB_DETECT                  PIN_17


/* Battery */
#define GRP_BATTERY_DETECT_EN          	SP5K_GPIO_GRP_NO_USE
#define OUT_BATTERY_DETECT_EN           (const UINT32)(dummyio)
#define SET_BATTERY_DETECT_EN(en)      
#define POL_BATTERY_DETECT_EN          	SP5K_GPIO_ACTIVE_HIGH
#define CFG_BATTERY_DETECT_EN           PIN_0


/* Sensor */
#define GRP_TG_RESET                   	SP5K_GPIO_GRP_NO_USE
#define OUT_TG_RESET                    (const UINT32)(dummyio)
#define SET_TG_RESET(en)               
#define POL_TG_RESET                   	SP5K_GPIO_ACTIVE_HIGH
#define CFG_TG_RESET                    PIN_0

#define GRP_TG_CLK                     	SP5K_GPIO_GRP_NO_USE
#define OUT_TG_CLK                      (const UINT32)(dummyio)
#define SET_TG_CLK(en)                 
#define POL_TG_CLK                     	SP5K_GPIO_ACTIVE_HIGH
#define CFG_TG_CLK                      PIN_0

#define GRP_TG_CS                      	SP5K_GPIO_GRP_NO_USE
#define OUT_TG_CS                       (const UINT32)(dummyio)
#define SET_TG_CS(en)                  
#define POL_TG_CS                      	SP5K_GPIO_ACTIVE_HIGH
#define CFG_TG_CS                       PIN_0


/* GPIO cfg */
#define SPCA_GEN_GPIO_CFGIO (\
			GPIO_CFG_IN(CFG_BUTTON_TELE)| \
			GPIO_CFG_IN(CFG_BUTTON_WIDE)| \
			GPIO_CFG_IN(CFG_BUTTON_SNAP1)| \
			GPIO_CFG_IN(CFG_BUTTON_SNAP2)| \
			GPIO_CFG_IN(CFG_BUTTON_PLAY)| \
			(MASK_NOUSE_GEN) )

#define SPCA_GEN_GPIO_MASK (\
			GPIO_MASK(CFG_BUTTON_TELE)| \
			GPIO_MASK(CFG_BUTTON_WIDE)| \
			GPIO_MASK(CFG_BUTTON_SNAP1)| \
			GPIO_MASK(CFG_BUTTON_SNAP2)| \
			GPIO_MASK(CFG_BUTTON_PLAY)| \
			(MASK_NOUSE_GEN) )

#define SPCA_GEN_GPIO_INIT_VAL (\
			0 )

#define SPCA_GEN_GPIO_INIT_MASK (\
			(MASK_NOUSE_GEN) )

#define SPCA_TGL_GPIO_CFGIO (\
			(MASK_NOUSE_TGL) )

#define SPCA_TGL_GPIO_MASK (\
			(MASK_NOUSE_TGL) )

#define SPCA_TGL_GPIO_INIT_VAL (\
			0 )

#define SPCA_TGL_GPIO_INIT_MASK (\
			(MASK_NOUSE_TGL) )

#define SPCA_DISP_GPIO_CFGIO (\
			GPIO_CFG_IN(CFG_USB_DETECT)| \
			(MASK_NOUSE_DISP) )

#define SPCA_DISP_GPIO_MASK (\
			GPIO_MASK(CFG_USB_DETECT)| \
			(MASK_NOUSE_DISP) )

#define SPCA_DISP_GPIO_INIT_VAL (\
			0 )

#define SPCA_DISP_GPIO_INIT_MASK (\
			(MASK_NOUSE_DISP) )

#define SPCA_LMI_GPIO_CFGIO (\
			(MASK_NOUSE_LMI) )

#define SPCA_LMI_GPIO_MASK (\
			(MASK_NOUSE_LMI) )

#define SPCA_LMI_GPIO_INIT_VAL (\
			0 )

#define SPCA_LMI_GPIO_INIT_MASK (\
			(MASK_NOUSE_LMI) )

#define SPCA_FML_GPIO_CFGIO (\
			(MASK_NOUSE_FML) )

#define SPCA_FML_GPIO_MASK (\
			(MASK_NOUSE_FML) )

#define SPCA_FML_GPIO_INIT_VAL (\
			0 )

#define SPCA_FML_GPIO_INIT_MASK (\
			(MASK_NOUSE_FML) )

#define SPCA_FMH_GPIO_CFGIO (\
			GPIO_CFG_IN(CFG_TV_IN)| \
			GPIO_CFG_IN(CFG_SD_CARD_DETECT)| \
			(MASK_NOUSE_FMH) )

#define SPCA_FMH_GPIO_MASK (\
			GPIO_MASK(CFG_TV_IN)| \
			GPIO_MASK(CFG_SD_CARD_DETECT)| \
			(MASK_NOUSE_FMH) )

#define SPCA_FMH_GPIO_INIT_VAL (\
			0 )

#define SPCA_FMH_GPIO_INIT_MASK (\
			(MASK_NOUSE_FMH) )

#define SPCA_POCU_GPIO_CFGIO (\
			GPIO_CFG_IN(CFG_BUTTON_POWER)| \
			(MASK_NOUSE_POCU) )

#define SPCA_POCU_GPIO_MASK (\
			GPIO_MASK(CFG_BUTTON_POWER)| \
			(MASK_NOUSE_POCU) )

#define SPCA_POCU_GPIO_INIT_VAL (\
			0 )

#define SPCA_POCU_GPIO_INIT_MASK (\
			(MASK_NOUSE_POCU) )

#define SPCA_SAR_GPIO_CFGIO (\
			(MASK_NOUSE_SAR) )

#define SPCA_SAR_GPIO_MASK (\
			(MASK_NOUSE_SAR) )

#define SPCA_SAR_GPIO_INIT_VAL (\
			0 )

#define SPCA_SAR_GPIO_INIT_MASK (\
			(MASK_NOUSE_SAR) )

#define SPCA_OIS_GPIO_CFGIO (\
			(MASK_NOUSE_OIS) )

#define SPCA_OIS_GPIO_MASK (\
			(MASK_NOUSE_OIS) )

#define SPCA_OIS_GPIO_INIT_VAL (\
			0 )

#define SPCA_OIS_GPIO_INIT_MASK (\
			(MASK_NOUSE_OIS) )


/* GPIO pull */
#define SP5K_GPIO_GEN_PULL_MIN               0x00031F38
#define SP5K_GPIO_GEN_PULL_MASK_MIN          0x00000000
#define SP5K_GPIO_GEN_PULL                   0x00031F38
#define SP5K_GPIO_GEN_PULL_MASK              0x00000000
#define SP5K_GPIO_TGL_PULL_MIN               0x0000FB60
#define SP5K_GPIO_TGL_PULL_MASK_MIN          0x00000000
#define SP5K_GPIO_TGL_PULL                   0x0000FB60
#define SP5K_GPIO_TGL_PULL_MASK              0x00000000
#define SP5K_GPIO_DISP_PULL_MIN              0xFFFFFFFF
#define SP5K_GPIO_DISP_PULL_MASK_MIN         0x00000000
#define SP5K_GPIO_DISP_PULL                  0xFFFFFFFF
#define SP5K_GPIO_DISP_PULL_MASK             0x00000000
#define SP5K_GPIO_LMI_PULL_MIN               0x0000FFFF
#define SP5K_GPIO_LMI_PULL_MASK_MIN          0x00000000
#define SP5K_GPIO_LMI_PULL                   0x0000FFFF
#define SP5K_GPIO_LMI_PULL_MASK              0x00000000
#define SP5K_GPIO_FML_PULL_MIN               0x0000781F
#define SP5K_GPIO_FML_PULL_MASK_MIN          0x00000000
#define SP5K_GPIO_FML_PULL                   0x0000781F
#define SP5K_GPIO_FML_PULL_MASK              0x00000000
#define SP5K_GPIO_FMH_PULL_MIN               0x00017FFF
#define SP5K_GPIO_FMH_PULL_MASK_MIN          0x00000000
#define SP5K_GPIO_FMH_PULL                   0x00017FFF
#define SP5K_GPIO_FMH_PULL_MASK              0x00000000
#define SP5K_GPIO_SAR_PULL_MIN               0x00000000
#define SP5K_GPIO_SAR_PULL_MASK_MIN          0x00000000
#define SP5K_GPIO_SAR_PULL                   0x00000000
#define SP5K_GPIO_SAR_PULL_MASK              0x00000000
#define SP5K_GPIO_OIS_PULL_MIN               0x00000000
#define SP5K_GPIO_OIS_PULL_MASK_MIN          0x00000000
#define SP5K_GPIO_OIS_PULL                   0x00000000
#define SP5K_GPIO_OIS_PULL_MASK              0x00000000



/* GPIO driving strength */
#define SP5K_GPIO_GEN_DS0_MIN                0x00000000
#define SP5K_GPIO_GEN_DS1_MIN                0x00000000
#define SP5K_GPIO_GEN_DS0                    0x00000000
#define SP5K_GPIO_GEN_DS1                    0x00000000
#define SP5K_GPIO_TGL_DS0_MIN                0x00000000
#define SP5K_GPIO_TGL_DS1_MIN                0x00000000
#define SP5K_GPIO_TGL_DS0                    0x00000000
#define SP5K_GPIO_TGL_DS1                    0x00000000
#define SP5K_GPIO_DISP_DS0_MIN               0x00000000
#define SP5K_GPIO_DISP_DS1_MIN               0x00000000
#define SP5K_GPIO_DISP_DS0                   0x00000000
#define SP5K_GPIO_DISP_DS1                   0x00000000
#define SP5K_GPIO_LMI_DS0_MIN                0x00000000
#define SP5K_GPIO_LMI_DS1_MIN                0x00000000
#define SP5K_GPIO_LMI_DS0                    0x00000000
#define SP5K_GPIO_LMI_DS1                    0x00000000
#define SP5K_GPIO_FML_DS0_MIN                0x00000000
#define SP5K_GPIO_FML_DS1_MIN                0x00000000
#define SP5K_GPIO_FML_DS0                    0x00000000
#define SP5K_GPIO_FML_DS1                    0x00000000
#define SP5K_GPIO_FMH_DS0_MIN                0x00000000
#define SP5K_GPIO_FMH_DS1_MIN                0x00000000
#define SP5K_GPIO_FMH_DS0                    0x00000000
#define SP5K_GPIO_FMH_DS1                    0x00000000


#endif /* _APP_GPIO_HW_PRJ_6330__H */
