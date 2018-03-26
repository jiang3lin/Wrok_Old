;/* SPCA5330B */

;/*  IO DEFINE  */

;/* LCD */
;#define IN_PWR_BACKLIGHT		1
PWR_BACKLIGHT		out dummy
PWR_LCD         out dummy 
PWR_BL_CTL      out dummy 
LCD_RESET       out dummy

;/* TV */
TV_IN		in fm 48

;/* FLASH & STROBE */
;#define IN_FLASH_READY  		1 /* always ready */
FLASH_TRIG		out	dummy
;#define OUT_FLASH_CHARGE		dummyioFlashCharge
PWR_FLASH		out	dummy

;/* LENS */
;
;#define OUT_PWR_ZOOM 			dummyio
;#define OUT_PWR_FOCUS 			dummyio
;#define OUT_SHUTTER_N 			dummyio
;#define OUT_SHUTTER_P 			dummyio	
;
ZOOM_HP         in dummy
ZOOM_CLK        in dummy
FOCUS_HP        in dummy


MOT_SCLK	out	dummy
MOT_SDAT	out	dummy
MOT_SLD		out	dummy
IN1			out	dummy
IN2			out	dummy
IN3			out	dummy
IN4			out	dummy
IN5         out dummy ;/* for zoom micro step*/
IN6         out dummy
IN7         out dummy
IN8         out dummy
IN12        out dummy ;/* for shutter micro step*/
IN13        out dummy 
IN14        out dummy
IN15        out dummy
IRIS_AP		out	dummy
IRIS_AN		out	dummy

MOT_RESET   out	dummy

;/* LED */
LED_POWER		out  			dummy
LED_FLASH		out    		dummy
LED_SELFTIMER		out      	dummy
LED_AF_LAMP		out      	dummy

;/* SD Card */
;#define IN_SD_CARD_WP			0
SD_CARD_DETECT		!in	fm 46 1


;/* BUTTON KEY */
BUTTON_POWER	!in	pocu	0
BUTTON_TELE		!in	gen	11
BUTTON_WIDE		!in	gen	12
BUTTON_SNAP1		!in	gen	10
BUTTON_SNAP2		!in	gen	8
BUTTON_PLAY		in	gen	3
;/* SPEAKER */

;/* USB */
USB_DETECT		in	disp	17

;/* Battery */
BATTERY_DETECT_EN	out dummy

;/* Sensor */
TG_RESET        out dummy
TG_CLK          out dummy
TG_CS           out dummy
