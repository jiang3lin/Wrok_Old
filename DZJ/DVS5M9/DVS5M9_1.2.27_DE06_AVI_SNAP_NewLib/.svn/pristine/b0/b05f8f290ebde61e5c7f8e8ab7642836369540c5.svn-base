;/* SPCA5330B */

;/*  IO DEFINE  */

;#define bit_0 0
;#define bit_1 1
;#define bit_2 2
;#define bit_3 3
;#define bit_4 4
;#define bit_5 5
;#define bit_6 6
;#define bit_7 7
;#define bit_8 8
;#define bit_9 9
;#define bit_10 10
;#define bit_11 11
;#define bit_12 12
;#define bit_13 13
;#define bit_14 14
;#define bit_15 15
;#define bit_16 16
;#define bit_17 17
;#define bit_18 18
;#define bit_19 19
;#define bit_20 20
;#define bit_21 21
;#define bit_22 22
;#define bit_23 23
;#define bit_24 24
;#define bit_25 25
;#define bit_26 26
;#define bit_27 27
;#define bit_28 28
;#define bit_29 29
;#define bit_30 30
;#define bit_31 31

;#define VALI_TglGPIO(msk) 			gpioPinLevelGet(SP5K_GPIO_GRP_TGL, GPIO_MASK(msk))		
;#define VALI_LmiGPIO(msk) 			gpioPinLevelGet(SP5K_GPIO_GRP_LMI, GPIO_MASK(msk))		
;#define VALI_FmlGPIO(msk) 			gpioPinLevelGet(SP5K_GPIO_GRP_FML, GPIO_MASK(msk))		
;#define VALI_FmhGPIO(msk) 			gpioPinLevelGet(SP5K_GPIO_GRP_FMH, GPIO_MASK(msk))		
;#define VALI_GenGPIO(msk) 			gpioPinLevelGet(SP5K_GPIO_GRP_GEN, GPIO_MASK(msk))		
;#define VALI_DisplGPIO(msk) 			gpioPinLevelGet(SP5K_GPIO_GRP_DISPL, GPIO_MASK(msk))		
;#define VALI_DisphGPIO(msk) 			gpioPinLevelGet(SP5K_GPIO_GRP_DISPH, GPIO_MASK(msk))		
;#define VALI_PocuGPIO(msk) 			gpioPinLevelGet(SP5K_GPIO_GRP_POCU, GPIO_MASK(msk))		
;#define VALI_UsbGPIO(msk) 			gpioPinLevelGet(SP5K_GPIO_GRP_USB, GPIO_MASK(msk))		
;#define VALI_CecGPIO(msk) 			gpioPinLevelGet(SP5K_GPIO_GRP_CEC, GPIO_MASK(msk))		

;#define MASK_NOUSE_TGL 0
;#define MASK_NOUSE_LMI 0
;#define MASK_NOUSE_FML 0
;#define MASK_NOUSE_FMH 0
;#define MASK_NOUSE_GEN 0
;#define MASK_NOUSE_DISPL 0
;#define MASK_NOUSE_DISPH 0
;#define MASK_NOUSE_POCU 0

;/* LCD */
LCD_RESET						!out fm 43	
LCD_BACK_LIGHT					out fm 45

;/* Sensor */
SENSOR_STB						out tg 10

;/* BUTTON KEY */
BUTTON_POWER					in pocu 0
BUTTON_OK						!in gen 10

;/* BUZZER */
BUZZER_PWM0					out gen 0

;/* LED */
LED_POWER						out lmi 1
LED_BUSY_FRONT					!out gen 1
LED_BUSY_REAR					!out gen 6
LED_CHARGE						!out gen 2

;/* RF Module */
RF_CE							out gen 4
RF_IRQ							in gen 13
RF_SPI_CS						out  lmi 0
RF_SPI_MISO						in fm 47
RF_SPI_SCK						out fm 48
RF_SPI_MOSI						out fm 49

;/* GSensor */
GSENSOR_I2C_CLK				out lmi 8
GSENSOR_I2C_SDA				out lmi 9
GSENSOR_DETECT					in fm 18

;/* WIFI */
WIFI_STB						!out lmi 11

;/* TV */
TV_IN							in fm 48

;/* SD Card */
SD_CARD_DETECT					!in gen 15
SD_CARD_POWER_ON				!out fm 37

;/* USB */

;/* HDMI Detect */


;/* Battery */
BATTERY_CHARGE_EN				out lmi 4
BATTERY_CHARGE_DETECT			in lmi 2

;/* Power */
POWER_5V						out gen 3


