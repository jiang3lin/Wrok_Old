#ifndef _APP_GPIO_HW_DVS5M2_PVT_H
#define _APP_GPIO_HW_DVS5M2_PVT_H
/* SPCA5330B */

/*  IO DEFINE  */

#define bit_0 0
#define bit_1 1
#define bit_2 2
#define bit_3 3
#define bit_4 4
#define bit_5 5
#define bit_6 6
#define bit_7 7
#define bit_8 8
#define bit_9 9
#define bit_10 10
#define bit_11 11
#define bit_12 12
#define bit_13 13
#define bit_14 14
#define bit_15 15
#define bit_16 16
#define bit_17 17
#define bit_18 18
#define bit_19 19
#define bit_20 20
#define bit_21 21
#define bit_22 22
#define bit_23 23
#define bit_24 24
#define bit_25 25
#define bit_26 26
#define bit_27 27
#define bit_28 28
#define bit_29 29
#define bit_30 30
#define bit_31 31

#define VALI_TglGPIO(msk) 			gpioPinLevelGet(SP5K_GPIO_GRP_TGL, GPIO_MASK(msk))		
#define VALI_LmiGPIO(msk) 			gpioPinLevelGet(SP5K_GPIO_GRP_LMI, GPIO_MASK(msk))		
#define VALI_FmlGPIO(msk) 			gpioPinLevelGet(SP5K_GPIO_GRP_FML, GPIO_MASK(msk))		
#define VALI_FmhGPIO(msk) 			gpioPinLevelGet(SP5K_GPIO_GRP_FMH, GPIO_MASK(msk))		
#define VALI_GenGPIO(msk) 			gpioPinLevelGet(SP5K_GPIO_GRP_GEN, GPIO_MASK(msk))		
#define VALI_DisplGPIO(msk) 			gpioPinLevelGet(SP5K_GPIO_GRP_DISPL, GPIO_MASK(msk))		
#define VALI_DisphGPIO(msk) 			gpioPinLevelGet(SP5K_GPIO_GRP_DISPH, GPIO_MASK(msk))		
#define VALI_PocuGPIO(msk) 			gpioPinLevelGet(SP5K_GPIO_GRP_POCU, GPIO_MASK(msk))		
#define VALI_UsbGPIO(msk) 			gpioPinLevelGet(SP5K_GPIO_GRP_USB, GPIO_MASK(msk))		
#define VALI_CecGPIO(msk) 			gpioPinLevelGet(SP5K_GPIO_GRP_CEC, GPIO_MASK(msk))		

#define MASK_NOUSE_TGL 0
#define MASK_NOUSE_LMI 0
#define MASK_NOUSE_FML 0
#define MASK_NOUSE_FMH 0
#define MASK_NOUSE_GEN 0
#define MASK_NOUSE_DISPL 0
#define MASK_NOUSE_DISPH 0
#define MASK_NOUSE_POCU 0

/* LCD */
#define GRP_LCD_RESET                  	SP5K_GPIO_GRP_FMH
#define OUT_LCD_RESET                   (const UINT32)(!VALO_FmGPIO(bit_43))
#define SET_LCD_RESET(en)               sp5kGpioWrite(GRP_LCD_RESET, 1<<CFG_LCD_RESET, (!en)<<CFG_LCD_RESET)
#define POL_LCD_RESET                  	SP5K_GPIO_ACTIVE_LOW
#define CFG_LCD_RESET                   PIN_43

#define GRP_LCD_BACK_LIGHT             	SP5K_GPIO_GRP_FMH
#define OUT_LCD_BACK_LIGHT              (const UINT32)(VALO_FmGPIO(bit_45))
#define SET_LCD_BACK_LIGHT(en)          sp5kGpioWrite(GRP_LCD_BACK_LIGHT, 1<<CFG_LCD_BACK_LIGHT, (en)<<CFG_LCD_BACK_LIGHT)
#define POL_LCD_BACK_LIGHT             	SP5K_GPIO_ACTIVE_HIGH
#define CFG_LCD_BACK_LIGHT              PIN_45

#define GRP_LCD_BACK_LIGHT_PWM         	SP5K_GPIO_GRP_GEN
#define OUT_LCD_BACK_LIGHT_PWM          (const UINT32)(VALO_GenGPIO(bit_5))
#define SET_LCD_BACK_LIGHT_PWM(en)      sp5kGpioWrite(GRP_LCD_BACK_LIGHT_PWM, 1<<CFG_LCD_BACK_LIGHT_PWM, (en)<<CFG_LCD_BACK_LIGHT_PWM)
#define POL_LCD_BACK_LIGHT_PWM         	SP5K_GPIO_ACTIVE_HIGH
#define CFG_LCD_BACK_LIGHT_PWM          PIN_5


/* Sensor */
#define GRP_SENSOR_STB                 	SP5K_GPIO_GRP_TGL
#define OUT_SENSOR_STB                  (const UINT32)(VALO_TgGPIO(bit_10))
#define SET_SENSOR_STB(en)              sp5kGpioWrite(GRP_SENSOR_STB, 1<<CFG_SENSOR_STB, (en)<<CFG_SENSOR_STB)
#define POL_SENSOR_STB                 	SP5K_GPIO_ACTIVE_HIGH
#define CFG_SENSOR_STB                  PIN_10


/* BUTTON KEY */
#define GRP_BUTTON_POWER               	SP5K_GPIO_GRP_POCU
#define IN_BUTTON_POWER                	(VALI_PocuGPIO(bit_0))
#define POL_BUTTON_POWER               	SP5K_GPIO_ACTIVE_HIGH
#define CFG_BUTTON_POWER                PIN_0

#define GRP_BUTTON_OK                  	SP5K_GPIO_GRP_GEN
#define IN_BUTTON_OK                   	(!VALI_GenGPIO(bit_10))
#define POL_BUTTON_OK                  	SP5K_GPIO_ACTIVE_LOW
#define CFG_BUTTON_OK                   PIN_10


/* BUZZER */
#define GRP_BUZZER_PWM0                	SP5K_GPIO_GRP_GEN
#define OUT_BUZZER_PWM0                 (const UINT32)(VALO_GenGPIO(bit_0))
#define SET_BUZZER_PWM0(en)             sp5kGpioWrite(GRP_BUZZER_PWM0, 1<<CFG_BUZZER_PWM0, (en)<<CFG_BUZZER_PWM0)
#define POL_BUZZER_PWM0                	SP5K_GPIO_ACTIVE_HIGH
#define CFG_BUZZER_PWM0                 PIN_0


/* LED */
#define GRP_LED_POWER                  	SP5K_GPIO_GRP_LMI
#define OUT_LED_POWER                   (const UINT32)(VALO_LmiGPIO(bit_1))
#define SET_LED_POWER(en)               sp5kGpioWrite(GRP_LED_POWER, 1<<CFG_LED_POWER, (en)<<CFG_LED_POWER)
#define POL_LED_POWER                  	SP5K_GPIO_ACTIVE_HIGH
#define CFG_LED_POWER                   PIN_1

#define GRP_LED_BUSY_FRONT             	SP5K_GPIO_GRP_LMI
#define OUT_LED_BUSY_FRONT              (const UINT32)(VALO_LmiGPIO(bit_2))
#define SET_LED_BUSY_FRONT(en)          sp5kGpioWrite(GRP_LED_BUSY_FRONT, 1<<CFG_LED_BUSY_FRONT, (en)<<CFG_LED_BUSY_FRONT)
#define POL_LED_BUSY_FRONT             	SP5K_GPIO_ACTIVE_HIGH
#define CFG_LED_BUSY_FRONT              PIN_2

#define GRP_LED_BUSY_REAR              	SP5K_GPIO_GRP_GEN
#define OUT_LED_BUSY_REAR               (const UINT32)(!VALO_GenGPIO(bit_6))
#define SET_LED_BUSY_REAR(en)           sp5kGpioWrite(GRP_LED_BUSY_REAR, 1<<CFG_LED_BUSY_REAR, (!en)<<CFG_LED_BUSY_REAR)
#define POL_LED_BUSY_REAR              	SP5K_GPIO_ACTIVE_LOW
#define CFG_LED_BUSY_REAR               PIN_6

#define GRP_LED_CHARGE                 	SP5K_GPIO_GRP_GEN
#define OUT_LED_CHARGE                  (const UINT32)(!VALO_GenGPIO(bit_2))
#define SET_LED_CHARGE(en)              sp5kGpioWrite(GRP_LED_CHARGE, 1<<CFG_LED_CHARGE, (!en)<<CFG_LED_CHARGE)
#define POL_LED_CHARGE                 	SP5K_GPIO_ACTIVE_LOW
#define CFG_LED_CHARGE                  PIN_2


/* RF Module */
#define GRP_RF_CE                      	SP5K_GPIO_GRP_GEN
#define OUT_RF_CE                       (const UINT32)(VALO_GenGPIO(bit_4))
#define SET_RF_CE(en)                   sp5kGpioWrite(GRP_RF_CE, 1<<CFG_RF_CE, (en)<<CFG_RF_CE)
#define POL_RF_CE                      	SP5K_GPIO_ACTIVE_HIGH
#define CFG_RF_CE                       PIN_4

#define GRP_RF_IRQ                     	SP5K_GPIO_GRP_GEN
#define IN_RF_IRQ                      	(VALI_GenGPIO(bit_13))
#define POL_RF_IRQ                     	SP5K_GPIO_ACTIVE_HIGH
#define CFG_RF_IRQ                      PIN_13

#define GRP_RF_SPI_CS                  	SP5K_GPIO_GRP_LMI
#define OUT_RF_SPI_CS                   (const UINT32)(VALO_LmiGPIO(bit_0))
#define SET_RF_SPI_CS(en)               sp5kGpioWrite(GRP_RF_SPI_CS, 1<<CFG_RF_SPI_CS, (en)<<CFG_RF_SPI_CS)
#define POL_RF_SPI_CS                  	SP5K_GPIO_ACTIVE_HIGH
#define CFG_RF_SPI_CS                   PIN_0

#define GRP_RF_SPI_MISO                	SP5K_GPIO_GRP_FMH
#define IN_RF_SPI_MISO                 	(VALI_FmGPIO(bit_47))
#define POL_RF_SPI_MISO                	SP5K_GPIO_ACTIVE_HIGH
#define CFG_RF_SPI_MISO                 PIN_47

#define GRP_RF_SPI_SCK                 	SP5K_GPIO_GRP_FMH
#define OUT_RF_SPI_SCK                  (const UINT32)(VALO_FmGPIO(bit_48))
#define SET_RF_SPI_SCK(en)              sp5kGpioWrite(GRP_RF_SPI_SCK, 1<<CFG_RF_SPI_SCK, (en)<<CFG_RF_SPI_SCK)
#define POL_RF_SPI_SCK                 	SP5K_GPIO_ACTIVE_HIGH
#define CFG_RF_SPI_SCK                  PIN_48

#define GRP_RF_SPI_MOSI                	SP5K_GPIO_GRP_FMH
#define OUT_RF_SPI_MOSI                 (const UINT32)(VALO_FmGPIO(bit_49))
#define SET_RF_SPI_MOSI(en)             sp5kGpioWrite(GRP_RF_SPI_MOSI, 1<<CFG_RF_SPI_MOSI, (en)<<CFG_RF_SPI_MOSI)
#define POL_RF_SPI_MOSI                	SP5K_GPIO_ACTIVE_HIGH
#define CFG_RF_SPI_MOSI                 PIN_49


/* GSensor */
#define GRP_GSENSOR_I2C_CLK            	SP5K_GPIO_GRP_LMI
#define OUT_GSENSOR_I2C_CLK             (const UINT32)(VALO_LmiGPIO(bit_8))
#define SET_GSENSOR_I2C_CLK(en)         sp5kGpioWrite(GRP_GSENSOR_I2C_CLK, 1<<CFG_GSENSOR_I2C_CLK, (en)<<CFG_GSENSOR_I2C_CLK)
#define POL_GSENSOR_I2C_CLK            	SP5K_GPIO_ACTIVE_HIGH
#define CFG_GSENSOR_I2C_CLK             PIN_8

#define GRP_GSENSOR_I2C_SDA            	SP5K_GPIO_GRP_LMI
#define OUT_GSENSOR_I2C_SDA             (const UINT32)(VALO_LmiGPIO(bit_9))
#define SET_GSENSOR_I2C_SDA(en)         sp5kGpioWrite(GRP_GSENSOR_I2C_SDA, 1<<CFG_GSENSOR_I2C_SDA, (en)<<CFG_GSENSOR_I2C_SDA)
#define POL_GSENSOR_I2C_SDA            	SP5K_GPIO_ACTIVE_HIGH
#define CFG_GSENSOR_I2C_SDA             PIN_9

#define GRP_GSENSOR_DETECT             	SP5K_GPIO_GRP_FML
#define IN_GSENSOR_DETECT              	(VALI_FmGPIO(bit_18))
#define POL_GSENSOR_DETECT             	SP5K_GPIO_ACTIVE_HIGH
#define CFG_GSENSOR_DETECT              PIN_18


/* WIFI */
#define GRP_WIFI_STB                   	SP5K_GPIO_GRP_LMI
#define OUT_WIFI_STB                    (const UINT32)(!VALO_LmiGPIO(bit_11))
#define SET_WIFI_STB(en)                sp5kGpioWrite(GRP_WIFI_STB, 1<<CFG_WIFI_STB, (!en)<<CFG_WIFI_STB)
#define POL_WIFI_STB                   	SP5K_GPIO_ACTIVE_LOW
#define CFG_WIFI_STB                    PIN_11


/* TV */
#define GRP_TV_IN                      	SP5K_GPIO_GRP_FMH
#define IN_TV_IN                       	(VALI_FmGPIO(bit_48))
#define POL_TV_IN                      	SP5K_GPIO_ACTIVE_HIGH
#define CFG_TV_IN                       PIN_48


/* SD Card */
#define GRP_SD_CARD_DETECT             	SP5K_GPIO_GRP_GEN
#define IN_SD_CARD_DETECT              	(!VALI_GenGPIO(bit_15))
#define POL_SD_CARD_DETECT             	SP5K_GPIO_ACTIVE_LOW
#define CFG_SD_CARD_DETECT              PIN_15

#define GRP_SD_CARD_POWER_ON           	SP5K_GPIO_GRP_FMH
#define OUT_SD_CARD_POWER_ON            (const UINT32)(!VALO_FmGPIO(bit_37))
#define SET_SD_CARD_POWER_ON(en)        sp5kGpioWrite(GRP_SD_CARD_POWER_ON, 1<<CFG_SD_CARD_POWER_ON, (!en)<<CFG_SD_CARD_POWER_ON)
#define POL_SD_CARD_POWER_ON           	SP5K_GPIO_ACTIVE_LOW
#define CFG_SD_CARD_POWER_ON            PIN_37


/* USB */

/* HDMI Detect */


/* Battery */
#define GRP_BATTERY_CHARGE_EN          	SP5K_GPIO_GRP_LMI
#define OUT_BATTERY_CHARGE_EN           (const UINT32)(VALO_LmiGPIO(bit_4))
#define SET_BATTERY_CHARGE_EN(en)       sp5kGpioWrite(GRP_BATTERY_CHARGE_EN, 1<<CFG_BATTERY_CHARGE_EN, (en)<<CFG_BATTERY_CHARGE_EN)
#define POL_BATTERY_CHARGE_EN          	SP5K_GPIO_ACTIVE_HIGH
#define CFG_BATTERY_CHARGE_EN           PIN_4

#define GRP_BATTERY_CHARGE_DETECT      	SP5K_GPIO_GRP_LMI
#define IN_BATTERY_CHARGE_DETECT       	(VALI_LmiGPIO(bit_3))
#define POL_BATTERY_CHARGE_DETECT      	SP5K_GPIO_ACTIVE_HIGH
#define CFG_BATTERY_CHARGE_DETECT       PIN_3


/* Power */
#define GRP_POWER_5V                   	SP5K_GPIO_GRP_GEN
#define OUT_POWER_5V                    (const UINT32)(VALO_GenGPIO(bit_3))
#define SET_POWER_5V(en)                sp5kGpioWrite(GRP_POWER_5V, 1<<CFG_POWER_5V, (en)<<CFG_POWER_5V)
#define POL_POWER_5V                   	SP5K_GPIO_ACTIVE_HIGH
#define CFG_POWER_5V                    PIN_3




/* GPIO cfg */
#define SPCA_GEN_GPIO_CFGIO (\
			GPIO_CFG_OUT(CFG_LCD_BACK_LIGHT_PWM)| \
			GPIO_CFG_IN(CFG_BUTTON_OK)| \
			GPIO_CFG_OUT(CFG_BUZZER_PWM0)| \
			GPIO_CFG_OUT(CFG_LED_BUSY_REAR)| \
			GPIO_CFG_OUT(CFG_LED_CHARGE)| \
			GPIO_CFG_OUT(CFG_RF_CE)| \
			GPIO_CFG_IN(CFG_RF_IRQ)| \
			GPIO_CFG_IN(CFG_SD_CARD_DETECT)| \
			GPIO_CFG_OUT(CFG_POWER_5V)| \
			(MASK_NOUSE_GEN) )

#define SPCA_GEN_GPIO_MASK (\
			GPIO_MASK(CFG_LCD_BACK_LIGHT_PWM)| \
			GPIO_MASK(CFG_BUTTON_OK)| \
			GPIO_MASK(CFG_BUZZER_PWM0)| \
			GPIO_MASK(CFG_LED_BUSY_REAR)| \
			GPIO_MASK(CFG_LED_CHARGE)| \
			GPIO_MASK(CFG_RF_CE)| \
			GPIO_MASK(CFG_RF_IRQ)| \
			GPIO_MASK(CFG_SD_CARD_DETECT)| \
			GPIO_MASK(CFG_POWER_5V)| \
			(MASK_NOUSE_GEN) )

#define SPCA_GEN_GPIO_INIT_VAL (\
			GPIO_PIN_VAL_LO(CFG_LCD_BACK_LIGHT_PWM)| \
			GPIO_PIN_VAL_LO(CFG_BUZZER_PWM0)| \
			GPIO_PIN_VAL_LO(CFG_LED_BUSY_REAR)| \
			GPIO_PIN_VAL_LO(CFG_LED_CHARGE)| \
			GPIO_PIN_VAL_LO(CFG_RF_CE)| \
			GPIO_PIN_VAL_LO(CFG_POWER_5V) )

#define SPCA_GEN_GPIO_INIT_MASK (\
			GPIO_MASK(CFG_LCD_BACK_LIGHT_PWM)| \
			GPIO_MASK(CFG_BUZZER_PWM0)| \
			GPIO_MASK(CFG_LED_BUSY_REAR)| \
			GPIO_MASK(CFG_LED_CHARGE)| \
			GPIO_MASK(CFG_RF_CE)| \
			GPIO_MASK(CFG_POWER_5V)| \
			(MASK_NOUSE_GEN) )

#define SPCA_TGL_GPIO_CFGIO (\
			GPIO_CFG_OUT(CFG_SENSOR_STB)| \
			(MASK_NOUSE_TGL) )

#define SPCA_TGL_GPIO_MASK (\
			GPIO_MASK(CFG_SENSOR_STB)| \
			(MASK_NOUSE_TGL) )

#define SPCA_TGL_GPIO_INIT_VAL (\
			GPIO_PIN_VAL_LO(CFG_SENSOR_STB) )

#define SPCA_TGL_GPIO_INIT_MASK (\
			GPIO_MASK(CFG_SENSOR_STB)| \
			(MASK_NOUSE_TGL) )

#define SPCA_DISP_GPIO_CFGIO (\
			(MASK_NOUSE_DISP) )

#define SPCA_DISP_GPIO_MASK (\
			(MASK_NOUSE_DISP) )

#define SPCA_DISP_GPIO_INIT_VAL (\
			0 )

#define SPCA_DISP_GPIO_INIT_MASK (\
			(MASK_NOUSE_DISP) )

#define SPCA_LMI_GPIO_CFGIO (\
			GPIO_CFG_OUT(CFG_LED_POWER)| \
			GPIO_CFG_OUT(CFG_LED_BUSY_FRONT)| \
			GPIO_CFG_OUT(CFG_RF_SPI_CS)| \
			GPIO_CFG_OUT(CFG_GSENSOR_I2C_CLK)| \
			GPIO_CFG_OUT(CFG_GSENSOR_I2C_SDA)| \
			GPIO_CFG_OUT(CFG_WIFI_STB)| \
			GPIO_CFG_OUT(CFG_BATTERY_CHARGE_EN)| \
			GPIO_CFG_IN(CFG_BATTERY_CHARGE_DETECT)| \
			(MASK_NOUSE_LMI) )

#define SPCA_LMI_GPIO_MASK (\
			GPIO_MASK(CFG_LED_POWER)| \
			GPIO_MASK(CFG_LED_BUSY_FRONT)| \
			GPIO_MASK(CFG_RF_SPI_CS)| \
			GPIO_MASK(CFG_GSENSOR_I2C_CLK)| \
			GPIO_MASK(CFG_GSENSOR_I2C_SDA)| \
			GPIO_MASK(CFG_WIFI_STB)| \
			GPIO_MASK(CFG_BATTERY_CHARGE_EN)| \
			GPIO_MASK(CFG_BATTERY_CHARGE_DETECT)| \
			(MASK_NOUSE_LMI) )

#define SPCA_LMI_GPIO_INIT_VAL (\
			GPIO_PIN_VAL_LO(CFG_LED_POWER)| \
			GPIO_PIN_VAL_LO(CFG_LED_BUSY_FRONT)| \
			GPIO_PIN_VAL_LO(CFG_RF_SPI_CS)| \
			GPIO_PIN_VAL_LO(CFG_GSENSOR_I2C_CLK)| \
			GPIO_PIN_VAL_LO(CFG_GSENSOR_I2C_SDA)| \
			GPIO_PIN_VAL_LO(CFG_WIFI_STB)| \
			GPIO_PIN_VAL_LO(CFG_BATTERY_CHARGE_EN) )

#define SPCA_LMI_GPIO_INIT_MASK (\
			GPIO_MASK(CFG_LED_POWER)| \
			GPIO_MASK(CFG_LED_BUSY_FRONT)| \
			GPIO_MASK(CFG_RF_SPI_CS)| \
			GPIO_MASK(CFG_GSENSOR_I2C_CLK)| \
			GPIO_MASK(CFG_GSENSOR_I2C_SDA)| \
			GPIO_MASK(CFG_WIFI_STB)| \
			GPIO_MASK(CFG_BATTERY_CHARGE_EN)| \
			(MASK_NOUSE_LMI) )

#define SPCA_FML_GPIO_CFGIO (\
			GPIO_CFG_IN(CFG_GSENSOR_DETECT)| \
			(MASK_NOUSE_FML) )

#define SPCA_FML_GPIO_MASK (\
			GPIO_MASK(CFG_GSENSOR_DETECT)| \
			(MASK_NOUSE_FML) )

#define SPCA_FML_GPIO_INIT_VAL (\
			0 )

#define SPCA_FML_GPIO_INIT_MASK (\
			(MASK_NOUSE_FML) )

#define SPCA_FMH_GPIO_CFGIO (\
			GPIO_CFG_OUT(CFG_LCD_RESET)| \
			GPIO_CFG_OUT(CFG_LCD_BACK_LIGHT)| \
			GPIO_CFG_IN(CFG_RF_SPI_MISO)| \
			GPIO_CFG_OUT(CFG_RF_SPI_SCK)| \
			GPIO_CFG_OUT(CFG_RF_SPI_MOSI)| \
			GPIO_CFG_IN(CFG_TV_IN)| \
			GPIO_CFG_OUT(CFG_SD_CARD_POWER_ON)| \
			(MASK_NOUSE_FMH) )

#define SPCA_FMH_GPIO_MASK (\
			GPIO_MASK(CFG_LCD_RESET)| \
			GPIO_MASK(CFG_LCD_BACK_LIGHT)| \
			GPIO_MASK(CFG_RF_SPI_MISO)| \
			GPIO_MASK(CFG_RF_SPI_SCK)| \
			GPIO_MASK(CFG_RF_SPI_MOSI)| \
			GPIO_MASK(CFG_TV_IN)| \
			GPIO_MASK(CFG_SD_CARD_POWER_ON)| \
			(MASK_NOUSE_FMH) )

#define SPCA_FMH_GPIO_INIT_VAL (\
			GPIO_PIN_VAL_LO(CFG_LCD_RESET)| \
			GPIO_PIN_VAL_LO(CFG_LCD_BACK_LIGHT)| \
			GPIO_PIN_VAL_LO(CFG_RF_SPI_SCK)| \
			GPIO_PIN_VAL_LO(CFG_RF_SPI_MOSI)| \
			GPIO_PIN_VAL_LO(CFG_SD_CARD_POWER_ON) )

#define SPCA_FMH_GPIO_INIT_MASK (\
			GPIO_MASK(CFG_LCD_RESET)| \
			GPIO_MASK(CFG_LCD_BACK_LIGHT)| \
			GPIO_MASK(CFG_RF_SPI_SCK)| \
			GPIO_MASK(CFG_RF_SPI_MOSI)| \
			GPIO_MASK(CFG_SD_CARD_POWER_ON)| \
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


#endif /* _APP_GPIO_HW_DVS5M2_PVT_H */
