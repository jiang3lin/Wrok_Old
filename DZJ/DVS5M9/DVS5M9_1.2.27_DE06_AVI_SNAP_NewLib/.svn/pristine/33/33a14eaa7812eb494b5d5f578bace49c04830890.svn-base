/*******************************************************************************
*
*         Copyright (c) 2002 by Sunplus Technology Co., Ltd.
*
*  This software is copyrighted by and is the property of Sunplus
*  Technology Co., Ltd. All rights are reserved by Sunplus Technology
*  Co., Ltd. This software may only be used in accordance with the
*  corresponding license agreement. Any unauthorized use, duplication,
*  distribution, or disclosure of this software is expressly forbidden.
*
*  This Copyright notice MUST not be removed or modified without prior
*  written consent of Sunplus Technology Co., Ltd.
*
*  Sunplus Technology Co., Ltd. reserves the right to modify this
*  software without notice.
*
*  Sunplus Technology Co., Ltd.
*  19, Innovation First Road, Science-Based Industrial Park,
*  Hsin-Chu, Taiwan, R.O.C.
*
*  Author:
*
*******************************************************************************/
#ifndef _SP5K_USB_API_H_
#define _SP5K_USB_API_H_

#include "common.h"
#include "middleware/usb_pima_code.h"

/*******************************************************************************
*                          D A T A    T Y P E S
*******************************************************************************/

/* Container */
typedef struct sp5kUsbSIDCContainer_s {
	UINT32 length;
	UINT16 type;
	UINT16 code;
	UINT32 transactionID;
	UINT32 payload;
} sp5kUsbSIDCContainer_t;

/* For Application mode defintion */
typedef enum {
	SP5K_USB_MODE_PCCAM   = 0x00000001,
	SP5K_USB_MODE_MSDC    = 0x00000002,
	SP5K_USB_MODE_SIDC    = 0x00000003,
	SP5K_USB_MODE_APITEST = 0x00000004,
	SP5K_USB_MODE_MULTI	  = 0x00000005,
	SP5K_USB_MODE_MAX_NUM = 0x00000006
} sp5k_Usb_Mode;


/* For vendor read/write callback function */
typedef enum{
	SP5K_FUNC_READ_REQ  = 0x00000000,
	SP5K_FUNC_WRITE_REQ = 0x00000001
} sp5k_Vnd_Req;


/* For DPS */
/* Get ConfigPrintService */
typedef enum {
	SP5K_prtVendorName             = 0x02010001,
	SP5K_prtVendorSpecificVersion  = 0x02010002,
	SP5K_prtProductName            = 0x02010003,
	SP5K_prtSerialNo               = 0x02010004
} sp5k_Dps_PrtInfoGet;

/* Get Capability */
typedef enum {
	SP5K_QUALITIY       = 0x02020001,
	SP5K_PAPERSIZES     = 0x02020002,
	SP5K_PAPERTYPES     = 0x02020003,
	SP5K_FILETYPES      = 0x02020004,
	SP5K_DATEPRINTS     = 0x02020005,
	SP5K_FILENAMEPRINTS = 0x02020006,
	SP5K_IMAGEOPTIMIZES = 0x02020007,
	SP5K_LAYOUTS        = 0x02020008,
	SP5K_FIXEDSIZES     = 0x02020009,
	SP5K_CROPPINGS      = 0x0202000A
} Get_Capability;

/* Set Capability */
typedef enum {
	SP5K_SET_QUALITY       = 0x0000,
	SP5K_SET_PAPERSIZE     = 0x0001,
	SP5K_SET_PAPERTYPE     = 0x0002,
	SP5K_SET_FILETYPE      = 0x0003,
	SP5K_SET_DATEPRINT     = 0x0004,
	SP5K_SET_FILENAMEPRINT = 0x0005,
	SP5K_SET_IMAGEOPTIMIZE = 0x0006,
	SP5K_SET_LAYOUT        = 0x0007,
	SP5K_SET_FIXEDSIZE     = 0x0008,
	SP5K_SET_CROPPING      = 0x0009
} CapType;

/* Capability value */
typedef enum {
	SP5K_Quality_Default            = 0x50000000,
	SP5K_Quality_Normal             = 0x50010000,
	SP5K_Quality_Draft              = 0x50020000,
	SP5K_Quality_Fine               = 0x50030000,
	SP5K_PaperSize_Default          = 0x51000000,
	SP5K_PaperSize_L                = 0x51010000,
	SP5K_PaperSize_2L               = 0x51020000,
	SP5K_PaperSize_HagakiPostcard   = 0x51030000,
	SP5K_PaperSize_CardSize         = 0x51040000,
	SP5K_PaperSize_100mmX150mm      = 0x51050000,
	SP5K_PaperSize_4X6in            = 0x51060000,
	SP5K_PaperSize_8X10in           = 0x51070000,
	SP5K_PaperSize_Letter           = 0x51080000,
	SP5K_PaperSize_11X17in          = 0x510A0000,
	SP5K_PaperSize_A0               = 0x51100000,
	SP5K_PaperSize_A1               = 0x51110000,
	SP5K_PaperSize_A2               = 0x51120000,
	SP5K_PaperSize_A3               = 0x51130000,
	SP5K_PaperSize_A4               = 0x51140000,
	SP5K_PaperSize_A5               = 0x51150000,
	SP5K_PaperSize_A6               = 0x51160000,
	SP5K_PaperSize_A7               = 0x51170000,
	SP5K_PaperSize_A8               = 0x51180000,
	SP5K_PaperSize_A9               = 0x51190000,
	SP5K_PaperSize_B0               = 0x51200000,
	SP5K_PaperSize_B1               = 0x51210000,
	SP5K_PaperSize_B2               = 0x51220000,
	SP5K_PaperSize_B3               = 0x51230000,
	SP5K_PaperSize_B4               = 0x51240000,
	SP5K_PaperSize_B5               = 0x51250000,
	SP5K_PaperSize_B6               = 0x51260000,
	SP5K_PaperSize_B7               = 0x51270000,
	SP5K_PaperSize_B8               = 0x51280000,
	SP5K_PaperSize_B9               = 0x51290000,
	SP5K_PaperSize_89mmRoll         = 0x51810000,
	SP5K_PaperSize_127mmRoll        = 0x51820000,
	SP5K_PaperSize_100mmRoll        = 0x51860000,
	SP5K_PaperSize_210mmRoll        = 0x51940000,
	SP5K_PaperType_Default          = 0x52000000,
	SP5K_PaperType_Plain            = 0x52010000,
	SP5K_PaperType_Photo            = 0x52020000,
	SP5K_PaperType_FastPhotp        = 0x52030000,
	SP5K_FileType_Default           = 0x53000000,
	SP5K_FileType_Exif_Jpeg         = 0x53010000,
	SP5K_FileType_Exif              = 0x53020000,
	SP5K_FileType_Jpeg              = 0x53030000,
	SP5K_FileType_TifEp             = 0x53040000,
	SP5K_FileType_FlashPix          = 0x53050000,
	SP5K_FileType_Bmp               = 0x53060000,
	SP5K_FileType_Ciff              = 0x53070000,
	SP5K_FileType_Gif               = 0x53080000,
	SP5K_FileType_Jfif              = 0x53090000,
	SP5K_FileType_Pcd               = 0x530A0000,
	SP5K_FileType_Pict              = 0x530B0000,
	SP5K_FileType_Png               = 0x530C0000,
	SP5K_FileType_Tiff              = 0x530D0000,
	SP5K_FileType_TifIt             = 0x530E0000,
	SP5K_FileType_Jp2               = 0x530F0000,
	SP5K_FileType_Jpx               = 0x53100000,
	SP5K_FileType_UndefineNonImg    = 0x53110000,
	SP5K_FileType_Assocation        = 0x53120000,
	SP5K_FileType_Script            = 0x53130000,
	SP5K_FileType_Executable        = 0x53140000,
	SP5K_FileType_Text              = 0x53150000,
	SP5K_FileType_Html              = 0x53160000,
	SP5K_FileType_XhtmlPrint        = 0x53170000,
	SP5K_FileType_Dpof              = 0x53180000,
	SP5K_FileType_Aiff              = 0x53190000,
	SP5K_FileType_Wav               = 0x531A0000,
	SP5K_FileType_Mp3               = 0x531B0000,
	SP5K_FileType_Avi               = 0x531C0000,
	SP5K_FileType_Mpeg              = 0x531D0000,
	SP5K_FileType_Asf               = 0x531E0000,
	SP5K_DatePrint_Default          = 0x54000000,
	SP5K_DatePrint_Off              = 0x54010000,
	SP5K_DatePrint_On               = 0x54020000,
	SP5K_FileNamePrint_Default      = 0x55000000,
	SP5K_FileNamePrint_Off          = 0x55010000,
	SP5K_FileNamePrint_On           = 0x55020000,
	SP5K_ImageOptimize_Default      = 0x56000000,
	SP5K_ImageOptimize_Off          = 0x56010000,
	SP5K_ImageOptimize_On           = 0x56020000,
	SP5K_Layout_Default             = 0x57000000,
	SP5K_Layout_1UpLayout           = 0x57010000,
	SP5K_Layout_2UpLayout           = 0x57020000,
	SP5K_Layout_250UpLayout         = 0x57FA0000,
	SP5K_Layout_IndexPrint          = 0x57FE0000,
	SP5K_Layout_1UpBorderLess       = 0x57FF0000,
	SP5K_FixedSizes_Default         = 0x58000000,
	SP5K_FixedSizes_2HX3Qin         = 0x58010000,
	SP5K_FixedSizes_3HX5in          = 0x58020000,
	SP5K_FixedSizes_4X6in           = 0x58030000,
	SP5K_FixedSizes_5X7in           = 0x58040000,
	SP5K_FixedSizes_8X10in          = 0x58050000,
	SP5K_FixedSizes_254X178mm       = 0x58060000,
	SP5K_FixedSizes_110X74mm        = 0x58070000,
	SP5K_FixedSizes_89X55mm         = 0x58080000,
	SP5K_FixedSizes_100X148mm       = 0x58090000,
	SP5K_FixedSizes_6X8cm           = 0x580A0000,
	SP5K_FixedSizes_7X10cm          = 0x580B0000,
	SP5K_FixedSizes_9X13cm          = 0x580C0000,
	SP5K_FixedSizes_10X15cm         = 0x580D0000,
	SP5K_FixedSizes_13X18cm         = 0x580E0000,
	SP5K_FixedSizes_15X21cm         = 0x580F0000,
	SP5K_FixedSizes_18X24cm         = 0x58100000,
	SP5K_FixedSizes_A4              = 0x58110000,
	SP5K_FixedSizes_Letter          = 0x58120000,
	SP5K_Cropping_Default           = 0x59000000,
	SP5K_Cropping_Off               = 0x59010000,
	SP5K_Cropping_On                = 0x59020000
} cap_value;

/* The fields in the structure has to be consistent with usbDPSJobCropArea_t */
typedef struct sp5kDpsCropArea_s {
	UINT16 wCropX;
	UINT16 wCropY;
	UINT16 wCropW;
	UINT16 wCropH;
}sp5kDpsCropArea_t;

typedef enum {
	SP5K_DPS_DATE_TYPE1 = 0x01030001, /*  2002/10/31   */
	SP5K_DPS_DATE_TYPE2 = 0x01030002, /*  31/10/2002   */
	SP5K_DPS_DATE_TYPE3 = 0x01030003, /*  10/31/2002   */
	SP5K_DPS_DATE_TYPE4 = 0x01030004, /*  31 Oct, 2002 */
	SP5K_DPS_DATE_TYPE5 = 0x01030005, /*  Oct 31, 2002 */
	SP5K_DPS_DATE_TYPE6 = 0x01030006  /*  2002, Oct 31 */
} sp5k_date_Type;

/* The fields in the structure has to be consistent with usbDPSJobDate_t */
typedef struct sp5kDpsDate_s {
	UINT32 dateType;
	UINT32 wYear;
	UINT32 bMonth;
	UINT32 bDay;
} sp5kDpsDate_t;

/* The fields in the structure has to be consistent with usbDPSDeviceStatusParams_t */
typedef struct sp5kDpsDeviceStatus_s {
	UINT32 dpsPrintServiceStatus;
	UINT32 jobEndReason;
	UINT32 errorStatus;
	UINT32 errorReason;
	UINT32 disconnectEnable;
	UINT32 capabilityChanged;
	UINT32 newJobOK;
} sp5kDpsDeviceStatus_t;

/* The fields in the structure has to be consistent with usbDPSJobStatusParams_t */
typedef struct sp5kDpsJobStatus_s {
	UINT8 prtPID[4];
	UINT8 filePath[256];
	UINT8 copyID[4];
	UINT8 progress[8];
	UINT8 imagesPrinted[4];
} sp5kDpsJobStatus_t;

/* Job action */
typedef enum {
	SP5K_ACTION_START               = 0x00000001,
	SP5K_ACTION_CONTINUE            = 0x00000002,
	SP5K_ACTION_ABORT               = 0x00000003,
	SP5K_ACTION_START_DPOF			= 0x00000004
} sp5k_Dps_Action;


/* For Error code */
typedef enum{
	SP5K_ERR_USB_SUCCESS           = 0x00000000,
	SP5K_ERR_USB_FAIL              = 0x00000001,
	SP5K_ERR_USB_PARAM_ERR         = 0x00000004,
	SP5K_ERR_DPS_PRT_NOT_AVAILABLE = 0x00000005,
	SP5K_ERR_DPS_ADDIMAGE          = 0x0D09060D,
	SP5K_ERR_USB_MAX_NUM           = 0x0D090603
} sp5k_usb_error_code;

typedef enum {
	/*!> max size is 31 byte */
	SP5K_USB_STR_INDEX_MANUFACTURER		= 1,
	SP5K_USB_STR_INDEX_PRODUCT			= 2,
	SP5K_USB_STR_INDEX_SERIAL_NUMBER	= 3,
	SP5K_USB_STR_INDEX_DPS_VERSION      = 4,
	SP5K_USB_STR_INDEX_DPS_NAME         = 5,
	SP5K_USB_STR_INDEX_DPS_SPEC_VERSION = 6,
	SP5K_USB_STR_INDEX_DPS_PRODUCT_NAME = 7,
	SP5K_USB_STR_INDEX_DPS_SERIAL_NUMBER= 8,
} sp5kUsbStrDesc_t;

typedef enum {
	SP5K_USB_DETECT_PIN = 0,
}sp5kUsbPinCfg_t;

/* USB Bus Max Power */
typedef enum {
	SP5K_USB_MAXPOWER_HIGH           = 0x00000001,
	SP5K_USB_MAXPOWER_NORMAL         = 0x00000002
} sp5kUsbMaxPower_t;

/* USB Host */
typedef enum {
	SP5K_USB_HOST_PC             = 0x00000001,
	SP5K_USB_HOST_PRINTER        = 0x00000002
} sp5kUsbHostMode_t;

/* USB bus event */
typedef enum {
	SP5K_USB_BUS_EVENT_NOINSTANCE,
	SP5K_USB_BUS_EVENT_SUSPEND,
	SP5K_USB_BUS_EVENT_RESUME,
	SP5K_USB_BUS_EVENT_RESET,
} sp5kUsbBusEvent_t;

/* USB charge selector */
typedef enum {
	SP5K_USB_CHARGE_MODE,
	SP5K_USB_CHARGE_START,
	SP5K_USB_CHARGE_STOP,
} sp5kUsbChargeSel_t;

/* USB charger port type */
typedef enum {
	SP5K_USB_DEDICATED_CHARGING_PORT_OPEN,
	SP5K_USB_DEDICATED_CHARGING_PORT_50K,
	SP5K_USB_DEDICATED_CHARGING_PORT_SHORT,
	SP5K_USB_DEDICATED_CHARGING_PORT_APPLE,
	SP5K_USB_DEDICATED_CHARGING_PORT_DPGND,
	SP5K_USB_STANDARD_DOWNSTREAM_PORT,
	SP5K_USB_CHARGING_DOWNSTREAM_PORT,
} sp5kUsbChargePortType_t;

/*paramId*/
typedef enum {
	SP5K_USB_DEV_DESCRIPTOR_CFG			= 0,/* param1: Config. customerize USB describtor */
	SP5K_USB_SIDC_ACCESS_MSG			= 1,/* param1: enable/disable USB SDIC access msg */
	SP5K_USB_IPHONE_CHARGER_HW_SUPPORT	= 2,/* SPCA5330 use only, SPCA6330 support embedded HW solution, param1: enable/disable iPhone charger external HW support. */
	SP5K_USB_CHARGER_IPHONE_HW_SUPPORT	= 2,
	SP5K_USB_UVC_RESOLUTION_CHANGE_CB	= 3,
	SP5K_USB_UVC_MJPG_BYTERATE_CFG		= 4,
	SP5K_USB_CHARGER_ENUM_TIMEOUT		= 5,
	SP5K_USB_MSDC_SCSI_CB				= 6,
	SP5K_USB_UVC_H264_BITRATE_SET       = 7,
	SP5K_USB_UVC_H264_MODE_SET          = 8,
	SP5K_USB_UVC_STILL_IMG_SAVE         = 9,
	SP5K_USB_UVC_IDR_FLAG_SET           = 10,
} sp5kUsbCfg_e;

#define SP5K_USB_UVC_H264_CABAC_IBBP    H264_ENC_MODE_CABAC_IBBP
#define SP5K_USB_UVC_H264_CABAC_IPPP    H264_ENC_MODE_CABAC_IPPP
#define SP5K_USB_UVC_H264_CABAC_IIII    H264_ENC_MODE_CABAC_IIII
#define SP5K_USB_UVC_H264_CAVLC_IBBP    H264_ENC_MODE_CAVLC_IBBP
#define SP5K_USB_UVC_H264_CAVLC_IPPP    H264_ENC_MODE_CAVLC_IPPP
#define SP5K_USB_UVC_H264_CAVLC_IIII    H264_ENC_MODE_CAVLC_IIII

typedef enum {
	SP5K_USB_DEV_DESCRIPTOR_CFG_BCD_DEVICE,
	SP5K_USB_DEV_DESCRIPTOR_CFG_MAX
} sp5kUsbDeviceDescriptorCfg_t;

typedef enum {
	SP5K_USB_PIMA_CAPTURE_TRIG_STILL = USB_PIMA_CAPTURE_TRIG_STILL,
	SP5K_USB_PIMA_CAPTURE_TRIG_VIDEO = USB_PIMA_CAPTURE_TRIG_VIDEO
} sp5kUsbTrigCaptureType_e;

typedef enum {
	SP5K_USB_PIMA_MTP_BK_PAUSE = USB_PIMA_MTP_BK_PAUSE,
	SP5K_USB_PIMA_MTP_BK_AUTO = USB_PIMA_MTP_BK_AUTO,
	SP5K_USB_PIMA_MTP_BK_MANUAL = USB_PIMA_MTP_BK_MANUAL
} sp5kUsbMtpBkUpdateType_e;

typedef enum {
	SP5K_PIMA_CONFIG_MTP_BK_UPDATE_TYPE,
	SP5K_PIMA_CONFIG_MTP_BK_BUFFER_SIZE,
	SP5K_PIMA_CONFIG_CB_GET_INFO,
	SP5K_PIMA_CONFIG_CB_APPLY_INFO,
	SP5K_PIMA_CONFIG_CB_TRIG_CAPTURE,
	SP5K_PIMA_CONFIG_CB_ABORT_CAPTURE,
	SP5K_PIMA_CONFIG_CB_CUSTOM_OP,
	SP5K_PIMA_CONFIG_CB_FILTER_FILES,
	SP5K_PIMA_CONFIG_DEVICE_PROPERTY_NUM,
	SP5K_PIMA_CONFIG_CB_GET_THUMB,
} sp5kPimaCfgSelector_t;

typedef UINT32 (*fpsp5kUsbCallback_t)(UINT16 idx, UINT16 val, UINT8 *pdata, UINT16 dataSize);
typedef UINT32 (*fpsp5kUsbPtpCallBack_t)(UINT8 *ppayload, UINT8 *pdata, UINT32);
typedef void (*fpsp5kUsb3ACallback_t)(void);
typedef UINT32 (*SP5K_PTP_IP_CB_OPEN)(UINT32 handle);
typedef void (*SP5K_PTP_IP_CB_CLOSE)(UINT32 handle);
typedef UINT32 (*SP5K_PTP_IP_CB_ISCONNECT)(UINT32 handle);
typedef UINT32 (*SP5K_PTP_IP_CB_REQUEST_RECV)(UINT32 handle, UINT8 *req);
typedef UINT32 (*SP5K_PTP_IP_CB_RESPONSE_SEND)(UINT32 handle, UINT32 trans_id, UINT8 *resp);
typedef UINT32 (*SP5K_PTP_IP_CB_EVENT_SEND)(UINT32 handle, UINT32 trans_id, UINT8 *evt);
typedef UINT32 (*SP5K_PTP_IP_CB_DATA_SEND)(UINT32 handle, UINT32 trans_id, UINT8 *data, UINT32 this_len, UINT32 total_len);
typedef UINT32 (*SP5K_PTP_IP_CB_DATA_LEN_GET)(UINT32 handle, UINT32 trans_id);
typedef UINT32 (*SP5K_PTP_IP_CB_DATA_RECV)(UINT32 handle, UINT32 trans_id, UINT8 *data, UINT32 len);
typedef void (*SP5K_USB_PIMA_CB_INFO_GET)(UINT32, UINT32 *);
typedef void (*SP5K_USB_PIMA_CB_INFO_APPLY)(UINT32, UINT32 *);
typedef UINT32 (*SP5K_USB_PIMA_CB_ICATCH_HOST_OP_HANDLER)(UINT32, UINT32, UINT32);
typedef void (*SP5K_USB_PIMA_CB_CAPTURE_IF)(sp5kUsbTrigCaptureType_e);
typedef UINT32 (*SP5K_USB_PIMA_CB_FILE_FILTER)(UINT32, const UINT8*);

/*******************************************************************************
*               F U N C T I O N    D E C L A R A T I O N S
*******************************************************************************/
/* general */
UINT32 sp5kUsbFuncCreate(void);
UINT32 sp5kUsbModeSet(UINT32 mode);
UINT32 sp5kUsbStrDescSet(sp5kUsbStrDesc_t strIdx, UINT8 *pstring, UINT16 strLen);
void sp5kUsbSwCon(UINT32 enable);
void sp5kUsbHwEventTrig(UINT8 bStatusType, UINT8 bOriginator, ...);
UINT32 sp5kUsbLineStateGet(void);
void sp5kUsbLineStableTimeCfg(UINT32 ms);
UINT32 sp5kUsbLineStateCfg(UINT32 *pstate);
UINT32 sp5kUsbLineStateTrig(void);/*must be after sp5kUsbLineStateCfg()*/
void sp5kUsbMaxPowerSet(UINT32 powerMode);

sp5kUsbHostMode_t sp5kUsbHostModeVerify(void);

UINT32 sp5kUsbBusEventGet(void);
#if SPCA5330
sp5kUsbChargePortType_t sp5kUsbChargePortTypeGet(void);
UINT32 sp5kUsbDetectStateGet(void);

#endif

UINT32 sp5kUsbCfgSet(UINT32 paramId,...);

/* msdc */
void sp5kUsbMSDCFuncReg(void);
UINT32 sp5kUsbLunSet(UINT32 drvId, UINT32 lunPos);

/* pccamera */
void sp5kUsbVNDRFuncReg(void);
void sp5kUsbVendreqCallbackSet(
	UINT8 direction,
	UINT8 requestLock,
	fpsp5kUsbCallback_t pfreadReqFunc,
	fpsp5kUsbCallback_t pfwriteReqFunc
);
void sp5kUsbVndrConEnhanceCfg(UINT32 enable);
void sp5kUsbVndr3ACallbackSet(fpsp5kUsb3ACallback_t pf3AReqFunc);



/* sidc and dps */
void sp5kUsbSIDCFuncReg(void);
void sp5kUsbPtpIpCbReg(
	SP5K_PTP_IP_CB_OPEN pfuncOpen,
	SP5K_PTP_IP_CB_CLOSE pfuncClose,
	SP5K_PTP_IP_CB_ISCONNECT pfuncIsConnect,
	SP5K_PTP_IP_CB_REQUEST_RECV pfuncReqRecv,
	SP5K_PTP_IP_CB_RESPONSE_SEND pfuncRespSend,
	SP5K_PTP_IP_CB_EVENT_SEND pfuncEvtSend,
	SP5K_PTP_IP_CB_DATA_SEND pfuncDataSend,
	SP5K_PTP_IP_CB_DATA_LEN_GET pfuncDataRecvSzGet,
	SP5K_PTP_IP_CB_DATA_RECV pfuncDataRecv
);
void sp5kUsbPtpIpOpen(void);
void sp5kUsbPtpIpStart(void);
void sp5kUsbPtpIpClose(void);


void sp5kUsbDPSFuncReg(void);
UINT32
sp5kDpsPrtInfoGet(
	UINT32 type,
	UINT8 *pbuf,
	UINT32 bufLen
);
UINT32
sp5kDpsPrtCapabilityGet(
	 UINT32 mainType,
     UINT32 subType,
     UINT32 *pdataArray,
     UINT32 *pdataArraySize
);
void
sp5kDpsPrtCapabilitySet(
	UINT32 capType,
	UINT32 capValue
);
UINT32
sp5kDpsPrtFileIdGet(
	UINT8 *ppath,
	UINT32 *pfileId
);

UINT32
sp5kDpsPrtDatePrintSet(
	UINT32 actType
);

UINT32
sp5kDpsPrtAddImage(
	UINT32 fileId,
	sp5kDpsCropArea_t *pcropArea,
	UINT8 *pfileName,
    sp5kDpsDate_t *pdate,
    UINT32 copies
);
UINT32
sp5kDpsPrtAddImageEx(
	UINT32 fileId,
	sp5kDpsCropArea_t *pcropArea,
	UINT8 *pfileName,
    sp5kDpsDate_t *pdate,
    UINT32 copies,
    UINT32 *jobEx,
    UINT32 jobExSize
);

UINT32
sp5kDpsPrtAction(
	UINT32 actType
);

/* PTP */
UINT32 sp5kUsbPtpOpFuncInstall(UINT32 opId,UINT32 ignore,fpsp5kUsbPtpCallBack_t pf, fpsp5kUsbPtpCallBack_t pfExit);
void sp5kUsbPtpOpFuncUninstall(UINT32 id);
UINT32 sp5kUsbPtpDataSend(UINT8 *pdata, UINT32 dataLength);
UINT32 sp5kUsbPtpDataGet(UINT8 *pdata, UINT32 *pDataLength);
UINT32 sp5kUsbPtpResponseSend(UINT32 codeId, UINT32 paramCnt, UINT32 param1, UINT32 param2, UINT32 param3);
UINT32 sp5kUsbPtpEventSend(UINT32 codeId,UINT32 param1, UINT32 param2, UINT32 param3);
UINT32 sp5kusbPimaDuplicateInstall(UINT32 uwOrigCode,UINT32 uwDupCode);
void sp5kusbPimaDuplicateUninstall(UINT32 id);
UINT32 sp5kUsbMaxPowerCheck(UINT32 powerMode);
UINT32 sp5kUsbPimaInfoUpdate(void);
void sp5kUsbPimaiCatchResp(UINT32 iCatchResp, UINT32 param1, UINT32 param2);
void sp5kUsbPimaEventSend(usbPIMAEventCode_e evt,UINT32 param1,UINT32 param2,UINT32 param3);
UINT32 sp5kUsbPimaCapDoneNotify(UINT32 more);
void sp5kUsbPimaCbRegister (SP5K_USB_PIMA_CB_INFO_GET cbPimaInfoGet,
							SP5K_USB_PIMA_CB_INFO_APPLY cbPimaInfoApply,
							SP5K_USB_PIMA_CB_ICATCH_HOST_OP_HANDLER cbPimaHostOpHandler,
							SP5K_USB_PIMA_CB_CAPTURE_IF cbCaptureTrigger,
							SP5K_USB_PIMA_CB_CAPTURE_IF cbCaptureAbort,
							SP5K_USB_PIMA_CB_FILE_FILTER cbFileFilter);
UINT32 sp5kUsbPimaSpecFileSend(UINT32 prot, UINT8 *fname);
UINT32 sp5kUsbPimaSendingFileCheck(void);
UINT32 sp5kUsbPimaInit(void);
void sp5kUsbPimaMtpBkCtl(sp5kUsbMtpBkUpdateType_e action);
UINT32 sp5kUsbPimaConfigSet(sp5kPimaCfgSelector_t id, UINT32 par1);

void sp5kUsbEyeChartConfig(UINT8 selector, UINT8 data);

#endif /* _SP5K_USB_API_H_ */

