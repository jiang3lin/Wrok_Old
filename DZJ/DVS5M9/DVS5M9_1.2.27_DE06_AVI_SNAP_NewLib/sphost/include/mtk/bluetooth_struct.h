/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2005
*
*  BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
*  THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
*  RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON
*  AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
*  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
*  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
*  NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
*  SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
*  SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH
*  THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
*  NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S
*  SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
*
*  BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
*  LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
*  AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
*  OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO
*  MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE. 
*
*  THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
*  WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF
*  LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND
*  RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER
*  THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).
*
*****************************************************************************/

/*****************************************************************************
 *
 * Filename:
 * ---------
 *   bluetooth_struct.h
 *
 * Project:
 * --------
 *   Maui_Software
 *
 * Description:
 * ------------
 *   This file is defines SAP for MTK Bluetooth.
 *
 * Author:
 * -------
 *   Max Chen
 *
 *============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Revision: #2 $
 * $Modtime$
 * $Log$
 *
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/
#ifndef __BLUETOOTH_STRUCT_H_
#define __BLUETOOTH_STRUCT_H_


#define HFG_SEQUENCE_NUM_LENGHT 16
#define BT_MAX_VERSION_STR_SIZE 20

/* Common */
typedef struct
{
    kal_uint32 lap;
    kal_uint8 uap;
    kal_uint16 nap;
} bt_addr_struct;


/* MSG_ID_BT_SPP_CONNECT_IND */
typedef struct
{
    kal_uint8 ref_count;
    kal_uint16 msg_len;
    UART_PORT port; /* virtual port number */
	kal_uint8    seq1[HFG_SEQUENCE_NUM_LENGHT];
	kal_uint8    seq2[HFG_SEQUENCE_NUM_LENGHT];
	
} bt_hf_connect_ind_struct;

/* MSG_ID_BT_HFG_SEND_DATA_REQ */
typedef struct
{
    kal_uint8 ref_count;
    kal_uint16 msg_len;
    UART_PORT port; /* virtual port number */
} bt_hf_send_data_req_struct;

/* MSG_ID_BT_POWERON_CNF */
typedef struct
{
    kal_uint8 ref_count;
    kal_uint16 msg_len;
    kal_bool result;    /* TRUE: successful, FALSE: failed */
} bt_poweron_cnf_struct;

typedef struct
{
    kal_uint8 ref_count;
    kal_uint16 msg_len;
    kal_bool result;    /* TRUE: successful, FALSE: failed */
} bt_engineer_mode_poweron_cnf_struct;

/* MSG_ID_BT_POWEROFF_CNF */
typedef struct
{
    kal_uint8 ref_count;
    kal_uint16 msg_len;
    kal_bool result;    /* TRUE: successful, FALSE: failed */
} bt_poweroff_cnf_struct;

typedef struct
{
    LOCAL_PARA_HDR 
    kal_bool afh;    /* True: send AFH command */
    kal_uint32 freq;
#if defined (WIFI_BB_MT5921) && (defined (__BTMODULE_MT6611__) || defined (__BTMODULE_MT6612__))
    kal_uint8 hb;
#endif    
    kal_bool pta;    /* True: send PTA command */
    kal_uint8 pta_action;
} bt_wifi_set_chnl_req_struct;

/* MSG_ID_BT_MMI_RESET_CNF */
typedef struct
{
    kal_uint8 ref_count;
    kal_uint16 msg_len;
    kal_bool result;    /* TRUE: successful, FALSE: failed */
} bt_mmi_reset_cnf_struct;

/* MSG_ID_BT_TEST_MODE_REQ */
typedef struct
{
    kal_uint8 ref_count;
    kal_uint16 msg_len;
    kal_uint8 mode;                     /* 1: enter test mode, 2: leave test mode */
#ifdef __GEMINI__
    kal_uint8 dual_sim_uart_setting;    /* 0: L4C_UART_TO_SIM1, 1: L4C_UART_UART_TO_SIM2 */
#endif 
} bt_test_mode_req_struct;

/* MSG_ID_BT_SSP_DEBUG_MODE_REQ */
typedef struct
{
    kal_uint8 ref_count;
    kal_uint16 msg_len;
    kal_uint8 on;                     /* 0: debug mode off, 1: debug mode on */
} bt_ssp_debug_mode_req_struct;

typedef struct
{
    kal_uint8 ref_count;
    kal_uint16 msg_len;
    kal_bool result;    /* TRUE: successful, FALSE: failed */
} bt_ssp_debug_mode_cnf_struct;

typedef struct
{
    kal_uint8 ref_count;
    kal_uint16 msg_len;
    kal_uint8 gpio;                /* ex. GPIO0: 0, GPIO1: 1 */
    kal_uint8 default_level;       /* low: 0, high: 1 */
    kal_uint8 mode;                /* fixed mode: 0, periodic mode: 1*/
    kal_uint8 periodic_mode_enable;/* disable: 0, enable: 1 */
    kal_uint16 high_period;        /* unit: 0.5 s */
    kal_uint16 low_period;         /* unit: 0.5 s */
} bt_gpio_set_req_struct;

typedef struct
{
    kal_uint8 ref_count;
    kal_uint16 msg_len;
    kal_uint32 trace;
    kal_uint8 primitive;
    kal_uint8 prompt;
} bt_engineer_mode_log_mask_req_struct;

typedef struct
{
    kal_uint8 ref_count;
    kal_uint16 msg_len;
    kal_uint8 name[32];
    kal_uint8 bd_addr[6];
    kal_uint8 dcs;
} bt_set_bd_param_req_struct;

typedef struct
{
    kal_uint8 ref_count;
    kal_uint16 msg_len;
    kal_uint8 index;
    kal_uint8 external_pa;
    kal_uint8 internal_pa;
    kal_uint16 output_power;
} bt_set_power_table_req_struct;

typedef struct
{
    kal_uint8 ref_count;
    kal_uint16 msg_len;
    kal_uint16 trim_value;
} bt_set_ana_trim_req_struct;

typedef struct
{
    kal_uint8 ref_count;
    kal_uint16 msg_len;
    kal_bool result;
} bt_pcm_lb_rsp_struct;

typedef struct
{
    kal_uint8 ref_count;
    kal_uint16 msg_len;
    kal_bool result;    /* TRUE: successful, FALSE: failed */
    kal_int8 chip_version[BT_MAX_VERSION_STR_SIZE];  /* ASCII */
    kal_int8 lmp_version[BT_MAX_VERSION_STR_SIZE];   /* ASCII */
    kal_int8 patch_version[BT_MAX_VERSION_STR_SIZE]; /* ASCII */
} bt_get_bt_version_cnf_struct;

typedef struct
{
    kal_uint8 ref_count;
    kal_uint16 msg_len;
    kal_bool result;
} bt_l4c_at_general_cnf_struct;

typedef struct
{
    kal_uint8 ref_count;
    kal_uint16 msg_len;
    kal_bool result;
} bt_engineer_mode_cnf_struct;

typedef enum
{
    BT_EM_TX_PATTERN_0000 = 1,
    BT_EM_TX_PATTERN_1111 = 2,
    BT_EM_TX_PATTERN_1010 = 3,
    BT_EM_TX_PATTERN_RANDOM = 4,
    BT_EM_TX_PATTERN_11110000 = 9,
    BT_EM_TX_PATTERN_SINGLE_TONE = 10,
} bt_em_tx_pattern_enum;

typedef enum
{
    BT_EM_RX_PATTERN_0000 = 1,
    BT_EM_RX_PATTERN_1111 = 2,
    BT_EM_RX_PATTERN_1010 = 3,
    BT_EM_RX_PATTERN_RANDOM = 4,
    BT_EM_RX_PATTERN_11110000 = 9,
} bt_em_rx_pattern_enum;

typedef enum
{
    BT_EM_TX_HOPPING_SINGLE,  /* Single frequency */
    BT_EM_TX_HOPPING_ALL,     /* All frequency 0~79 */
} bt_em_tx_hopping_enum;

typedef enum
{
    BT_EM_PACKET_NULL = 0x0,
    BT_EM_PACKET_POLL = 0x1,
    BT_EM_PACKET_FHS = 0x2,
    BT_EM_PACKET_DM1 = 0x3,
    BT_EM_PACKET_DH1 = 0x4,
    BT_EM_PACKET_HV1 = 0x5,
    BT_EM_PACKET_HV2 = 0x6,
    BT_EM_PACKET_HV3 = 0x7,
    BT_EM_PACKET_DV = 0x8,
    BT_EM_PACKET_AUX = 0x9,
    BT_EM_PACKET_DM3 = 0xA,
    BT_EM_PACKET_DH3 = 0xB,
    BT_EM_PACKET_DM5 = 0xE,
    BT_EM_PACKET_DH5 = 0xF,
    BT_EM_PACKET_EV3 = 0x17,
    BT_EM_PACKET_EV4 = 0x1C,
    BT_EM_PACKET_EV5 = 0x1D,
    BT_EM_PACKET_2DH1 = 0x24,
    BT_EM_PACKET_3DH1 = 0x28,
    BT_EM_PACKET_2DH3 = 0x2A,
    BT_EM_PACKET_3DH3 = 0x2B,
    BT_EM_PACKET_2DH5 = 0x2E,
    BT_EM_PACKET_3DH5 = 0x2F,
    BT_EM_PACKET_2EV3 = 0x36,
    BT_EM_PACKET_3EV3 = 0x37,
    BT_EM_PACKET_2EV5 = 0x3C,
    BT_EM_PACKET_3EV5 = 0x3D
} bt_em_packet_enum;

typedef enum
{
    BT_EM_RX_MODE = 0xB,
    BT_EM_RX_MODE_END = 0xFF,
} bt_em_rx_mode_enum;

typedef struct
{
    kal_uint8 ref_count;
    kal_uint16 msg_len;
    bt_em_tx_pattern_enum pattern;
    bt_em_tx_hopping_enum channel_hopping;
    kal_uint8 tx_freq;  /* range: 0~78 */
    kal_uint8 rx_freq;  /* range: 0~78 */
    kal_uint8 poll_period;
    bt_em_packet_enum packet_type;
    kal_uint16 packet_length;
} bt_engineer_mode_tx_test_req_struct;

typedef struct
{
    kal_uint8 ref_count;
    kal_uint16 msg_len;
    kal_bool result;
} bt_engineer_mode_tx_test_cnf_struct;

typedef struct
{
    kal_uint8 ref_count;
    kal_uint16 msg_len;
    bt_em_rx_pattern_enum pattern;
    bt_em_rx_mode_enum mode;
    kal_uint8 rx_freq;  /* range: 0~78 */
    bt_em_packet_enum packet_type;
    kal_uint8 bd_addr[6];
} bt_engineer_mode_rx_test_req_struct;

typedef struct
{
    kal_uint8 ref_count;
    kal_uint16 msg_len;
    kal_bool result;
} bt_engineer_mode_rx_test_cnf_struct;

typedef struct
{
    kal_uint8 ref_count;
    kal_uint16 msg_len;
    kal_uint32 received_packets;
    kal_uint8 packet_error_rate;  /* in percentage */
    kal_uint32 received_bytes;
    kal_uint8 bit_error_rate;     /* in percentage */
} bt_engineer_mode_rx_test_end_cnf_struct;

typedef struct
{
    kal_uint8 ref_count;
    kal_uint16 msg_len;
    kal_uint8 level;
} bt_engineer_mode_power_test_req_struct;

typedef struct
{
    kal_uint8 ref_count;
    kal_uint16 msg_len;
    kal_bool result;
} bt_engineer_mode_txrx_power_cnf_struct;

typedef struct
{
    kal_uint8 ref_count;
    kal_uint16 msg_len;
    kal_uint8 addr[12];
} bt_engineer_mode_device_test_req_struct;

#ifdef __GEMINI__
/* MSG_ID_BT_ENGINEER_MODE_POWERON_REQ */
typedef struct
{
    kal_uint8 ref_count;
    kal_uint16 msg_len;
    kal_uint8 dual_sim_uart_setting;
} bt_engineer_mode_poweron_req_struct;
#endif /* __GEMINI__ */ 

typedef struct
{
    kal_uint8 ref_count;
    kal_uint16 msg_len;
#ifdef __GEMINI__
    kal_uint8 dual_sim_uart_setting;    /* 0: L4C_UART_TO_SIM1, 1: L4C_UART_UART_TO_SIM2 */
#endif 
} bt_read_bd_addr_req_struct;

typedef struct
{
    kal_uint8 ref_count;
    kal_uint16 msg_len;
    kal_bool result;
    kal_uint8 bd_addr[6];
} bt_read_bd_addr_cnf_struct;

/* MSG_ID_BT_TEST_CMD_REQ */
typedef struct
{
    kal_uint8 ref_count;
    kal_uint16 msg_len;
    kal_uint8 test_cmd_content[256];
} bt_test_cmd_req_struct;

/* MSG_ID_BT_L4C_TEST_CMD_CNF */
typedef struct
{
    kal_uint8 ref_count;
    kal_uint16 msg_len;
    kal_uint8 test_cmd_rsp_buf[256];
    kal_uint16 length;
} bt_test_cmd_rsp_struct;

/* MSG_ID_BT_READ_RSSI_REQ */
typedef struct
{
    kal_uint8 ref_count;
    kal_uint16 msg_len;
    kal_uint32 lap;
    kal_uint8  uap;
    kal_uint16 nap;
}bt_read_rssi_req_struct;

/* MSG_ID_BT_READ_RSSI_CNF */
typedef struct
{
    kal_uint8 ref_count;
    kal_uint16 msg_len;
    kal_uint8 result;
    kal_int8 rssi_value;
    kal_uint32 lap;
    kal_uint8  uap;
    kal_uint16 nap;
}bt_read_rssi_cnf_struct;

#define OBRC_SCO_REJECT 0xFF    /* SCO REJECT */

/* BIP definitions */
#define BT_BIP_MAX_PATH_LEN     260
#define BT_BIP_MAX_IMG_FORMATS  10
#define BT_BIP_MAX_IMG_NAME_LEN  GOEP_MAX_OBJ_NAME      /* 256 */
#define BT_BIP_MAX_TIME_LEN     18  /* (YYYYMMDDTHHMMSS)(Z) */
#define BT_BIP_IMG_HANDLE_LEN   8
#define BT_BIP_MAX_DEV_NAME_LEN 80
#define BT_BIP_MAX_ATTR_NAME_LEN 40
#define BT_BIP_MAX_IMG_LIST_ITEMS 32
#define BT_BIP_ENCODING_LENGTH 2

/* GOEP definitions */
#define GOEP_MAX_UUID_SIZE 16
#define GOEP_MAX_PASSWD_SIZE 16
#define GOEP_MAX_REALM_SIZE 20
#define GOEP_MAX_USERID_SIZE 20
#define GOEP_MAX_OBJ_NAME (256 * 2)
#define GOEP_MAX_FOLDER_NAME (256 * 2)
#define GOEP_MAX_DEV_NAME 80
#define GOEP_MAX_DESCRIPTION 80
#define GOEP_MAX_MIME_TYPE 80
#define GOEP_MAX_SERVER_NUM 16
#define GOEP_MAX_CLIENT_NUM 16
#define GOEP_CONNECTION_NUM (GOEP_MAX_SERVER_NUM + GOEP_MAX_CLIENT_NUM)

/* The function call was successful. */
#define GOEP_STATUS_SUCCESS          0
/* The operation has failed to start. */
#define GOEP_STATUS_FAILED           1
/* The operation is pending to wait for authentication information */
#define GOEP_STATUS_PENDING          2
/* The transport layer link is disconnected. */
#define GOEP_STATUS_DISCONNECT       3
/* No connection exists, when one is required */
#define GOEP_STATUS_NO_CONNECT       4
/* A request is pending or resources are in use. */
#define GOEP_STATUS_BUSY             11
/* The necessary resources are not available. */
#define GOEP_STATUS_NO_RESOURCES     12
/* An invalid function parameter was provided. */
#define GOEP_STATUS_INVALID_PARM     18
/*
 * Bluetooth Only: The request failed because the specified device
 * is in the process of being disconnected.
 */
#define GOEP_STATUS_IN_PROGRESS      19
/* Feature not supported */
#define GOEP_STATUS_NOT_SUPPORTED    23
/*
 * Bluetooth Only : Feature is restricted, due to a 
 * Bluetooth security failure 
 */
#define GOEP_STATUS_RESTRICTED       20
/* The OBEX packet is too small to support the request. */
#define GOEP_STATUS_PACKET_TOO_SMALL 31
/* No active session exists, when one is required. */
#define GOEP_STATUS_NO_SESSION       32

/* SCO_REJECT */
#define GOEP_STATUS_SCO_REJECT       25

#define GOEP_CREATED              0x21  /* Created */
#define GOEP_ACCEPTED             0x22  /* Accepted */
#define GOEP_NON_AUTHOR_INFO      0x23  /* Non-Authoritative Information */
#define GOEP_NO_CONTENT           0x24  /* No Content */
#define GOEP_RESET_CONTENT        0x25  /* Reset Content */
#define GOEP_PARTIAL_CONTENT      0x26  /* Partial Content */

#define GOEP_MULTIPLE_CHOICES     0x30  /* Multiple Choices */
#define GOEP_MOVED_PERMANENT      0x31  /* Moved Permanently */
#define GOEP_MOVED_TEMPORARY      0x32  /* Moved Temporarily */
#define GOEP_SEE_OTHER            0x33  /* See Other */
#define GOEP_NOT_MODIFIED         0x34  /* Not Modified */
#define GOEP_USE_PROXY            0x35  /* Use Proxy */

#define GOEP_BAD_REQUEST          0x40  /* Bad Request */
#define GOEP_UNAUTHORIZED         0x41  /* Unauthorized */
#define GOEP_PAYMENT_REQUIRED     0x42  /* Payment Required */
#define GOEP_FORBIDDEN            0x43  /* Forbidden - operation is understood but refused */                                           
#define GOEP_NOT_FOUND            0x44  /* Not Found */
#define GOEP_METHOD_NOT_ALLOWED   0x45  /* Method Not Allowed */
#define GOEP_NOT_ACCEPTABLE       0x46  /* Not Acceptable */
#define GOEP_PROXY_AUTHEN_REQ     0x47  /* Proxy Authentication Required */
#define GOEP_REQUEST_TIME_OUT     0x48  /* Request Timed Out */
#define GOEP_CONFLICT             0x49  /* Conflict */

#define GOEP_GONE                 0x4a  /* Gone */
#define GOEP_LENGTH_REQUIRED      0x4b  /* Length Required */
#define GOEP_PRECONDITION_FAILED  0x4c  /* Precondition Failed */
#define GOEP_REQ_ENTITY_TOO_LARGE 0x4d  /* Requested entity is too large */
#define GOEP_REQ_URL_TOO_LARGE    0x4e  /* Requested URL is too large */
#define GOEP_UNSUPPORT_MEDIA_TYPE 0x4f  /* Unsupported Media Type */

#define GOEP_INTERNAL_SERVER_ERR  0x50  /* Internal Server Error */
#define GOEP_NOT_IMPLEMENTED      0x51  /* Not Implemented */
#define GOEP_BAD_GATEWAY          0x52  /* Bad Gateway */
#define GOEP_SERVICE_UNAVAILABLE  0x53  /* Service Unavailable */
#define GOEP_GATEWAY_TIMEOUT      0x54  /* Gateway Timeout */
#define GOEP_HTTP_VER_NO_SUPPORT  0x55  /* HTTP version not supported */

#define GOEP_DATABASE_FULL        0x60  /* Database Full */
#define GOEP_DATABASE_LOCKED      0x61  /* Database Locked */


#define GOEP_USER_CANCEL          0x70 /* User cancel the pushing/pulling/connect */
#define GOEP_USER_UNKNOW          0x71 /* User cancel the pushing/pulling/connect */
/* End of GOEPStatus */

/* GOEP ENUM */
typedef enum
{
    GOEP_TP_BT = 0,
    GOEP_TP_IRDA
} goep_tp_type_enum;

typedef enum
{
    GOEP_FIRST_PKT = 0,
    GOEP_NORMAL_PKT,
    GOEP_FINAL_PKT,
    GOEP_SINGLE_PKT
} goep_pkt_type_enum;

typedef enum
{
    GOEP_FORWARD_FOLDER = 0,
    GOEP_BACK_FOLDER,
    GOEP_ROOT_FOLDER,
    GOEP_CREATE_FOLDER
} goep_set_folder_type_enum;

typedef enum
{
    GOEP_PUT_NORMAL = 0,
    GOEP_PUT_DELETE,
    GOEP_CREATEEMPTY
} goep_put_type_enum;

typedef enum
{
    GOEP_SERVER_ROLE = 0,
    GOEP_CLIENT_ROLE
} goep_role_enum;


/**** Start of BIP Enum ****/
typedef enum
{
    BT_BIP_IMAGE_PUSH = 0x01,
    BT_BIP_IMAGE_PULL = 0x02,
    BT_BIP_ADVANCED_PRINTING = 0x04,
    BT_BIP_REMOTE_CAMERA = 0x08,
    BT_BIP_AUTO_ARCHIVE = 0x10,
    BT_BIP_REMOTE_DISPLAY = 0x20
} bt_bip_service_enum;

typedef enum
{
    BT_BIP_CNF_SUCCESS = 0x70,
    BT_BIP_CNF_FAILED,
    BT_BIP_PARTIAL_CONTENT,
    BT_BIP_XML_GEN_FAIL,
    BT_BIP_FILE_OPEN_FAIL,
    BT_BIP_FILE_READ_FAIL,
    BT_BIP_FILE_WRITE_FAIL,
    BT_BIP_FILE_MOVE_FAIL,
    BT_BIP_INVALID_PARM,
    BT_BIP_DISK_FULL,
    BT_BIP_ROOT_DIR_FULL,
    BT_BIP_FS_MEDIA_CHANGED,
    BT_BIP_SCO_REJECT,
    BT_BIP_BTCHIP_REJECT,
    BT_BIP_XML_PARSE_FAIL,
    BT_BIP_FS_QUOTA_FULL,
    BT_BIP_DISCONNECTED,
    BT_BIP_FS_WRITE_PROTECTION,
    BT_BIP_DRM_NO_RIGHTS,
    BT_BIP_CNF_CHALLENGE_OK,
    BT_BIP_CNF_CHALLENGE_FAILED,
    BT_BIP_AUTHENTICATE_FAILED,

    BT_BIP_LAST_CNF_CODE
} bt_bip_cnf_enum;

typedef enum
{
    BT_BIP_IMG_TYPE_UNSUPPORT = 0x00000000,
    BT_BIP_IMG_TYPE_JPEG = 0x00000001,
    BT_BIP_IMG_TYPE_BMP = 0x00000002,
    BT_BIP_IMG_TYPE_GIF = 0x00000004,
    BT_BIP_IMG_TYPE_WBMP = 0x00000008,
    BT_BIP_IMG_TYPE_PNG = 0x00000010
} bt_bip_img_format_enum;

/**** End of BIP Enum ****/

/* GOEP structure */

typedef bt_addr_struct goep_bd_addr_struct;

typedef struct
{
    kal_uint8 ref_count;
    kal_uint16 msg_len;
    kal_uint8 uuid[GOEP_MAX_UUID_SIZE];
    kal_uint8 uuid_len;
    kal_uint8 req_id;
    goep_tp_type_enum tp_type;
    kal_bool need_auth;
    kal_uint8 *buf_ptr;
    kal_uint16 buf_size;
    kal_uint8 rawheader;
} goep_register_server_req_struct;

typedef struct
{
    kal_uint8 ref_count;
    kal_uint16 msg_len;
    kal_uint8 uuid[GOEP_MAX_UUID_SIZE];
    kal_uint8 uuid_len;
    kal_uint8 req_id;
    kal_uint8 rsp_code;
    kal_uint8 goep_conn_id;
} goep_register_server_cnf_struct;

typedef struct
{
    kal_uint8 ref_count;
    kal_uint16 msg_len;
    kal_uint8 goep_conn_id;
} goep_deregister_server_req_struct;

typedef struct
{
    kal_uint8 ref_count;
    kal_uint16 msg_len;
    kal_uint8 uuid[GOEP_MAX_UUID_SIZE];
    kal_uint8 uuid_len;
    kal_uint8 goep_conn_id;
    goep_bd_addr_struct bd_addr;
    kal_uint8 dev_name[GOEP_MAX_DEV_NAME];
} goep_authorize_ind_struct;

typedef struct
{
    kal_uint8 ref_count;
    kal_uint16 msg_len;
    kal_uint8 uuid[GOEP_MAX_UUID_SIZE];
    kal_uint8 uuid_len;
    kal_uint8 goep_conn_id;
    goep_bd_addr_struct bd_addr;
    kal_uint8 dev_name[GOEP_MAX_DEV_NAME];
    kal_uint16 peer_mru;
    kal_uint32 cm_conn_id;
    kal_uint8 client_auth;
    kal_uint8 server_auth;
    kal_uint32 headerlen;
} goep_connect_ind_struct;

typedef struct
{
    kal_uint8 ref_count;
    kal_uint16 msg_len;
    kal_uint8 goep_conn_id;
    kal_uint8 rsp_code;
    kal_uint8 client_pwd[GOEP_MAX_PASSWD_SIZE];
    kal_uint8 client_pwd_len;
    kal_uint8 client_userid[GOEP_MAX_USERID_SIZE];
    kal_uint8 client_userid_len;
    kal_uint8 server_pwd[GOEP_MAX_PASSWD_SIZE];
    kal_uint8 server_pwd_len;
    kal_uint8 server_realm[GOEP_MAX_REALM_SIZE];
    kal_uint8 server_realm_len;
} goep_connect_rsp_struct;

typedef struct
{
    kal_uint8 ref_count;
    kal_uint16 msg_len;
    kal_uint8 uuid[GOEP_MAX_UUID_SIZE];
    kal_uint8 uuid_len;
    kal_uint8 goep_conn_id;
    goep_pkt_type_enum pkt_type;
    goep_put_type_enum put_type;
    kal_uint32 total_obj_len;
    kal_uint8 obj_name[GOEP_MAX_OBJ_NAME];
    kal_uint8 obj_mime_type[GOEP_MAX_MIME_TYPE];
    kal_uint8 *frag_ptr;
    kal_uint16 frag_len;
    kal_uint32 headerlen;
} goep_push_ind_struct;

typedef struct
{
    kal_uint8 ref_count;
    kal_uint16 msg_len;
    kal_uint8 uuid[GOEP_MAX_UUID_SIZE];
    kal_uint8 uuid_len;
    kal_uint8 goep_conn_id;
    kal_uint8 obj_name[GOEP_MAX_OBJ_NAME];
    kal_uint8 obj_mime_type[GOEP_MAX_MIME_TYPE]; 
    goep_pkt_type_enum pkt_type;
    kal_uint32 headerlen;
} goep_pull_ind_struct;

typedef struct
{
    kal_uint8 ref_count;
    kal_uint16 msg_len;
    kal_uint8 goep_conn_id;
    goep_pkt_type_enum pkt_type;
    kal_uint8 rsp_code;
    kal_uint8 obj_mime_type[GOEP_MAX_MIME_TYPE];
    kal_uint32 total_obj_len;
    kal_uint8 *frag_ptr;
    kal_uint16 frag_len;
} goep_pull_rsp_struct;

typedef struct
{
    kal_uint8 ref_count;
    kal_uint16 msg_len;
    kal_uint8 uuid[GOEP_MAX_UUID_SIZE];
    kal_uint8 uuid_len;
    kal_uint8 goep_conn_id;
    kal_uint8 folder_name[GOEP_MAX_FOLDER_NAME];
    goep_set_folder_type_enum setpath_flag;
    kal_uint32 headerlen;
} goep_set_folder_ind_struct;

typedef struct
{
    kal_uint8 ref_count;
    kal_uint16 msg_len;
    kal_uint8 uuid[GOEP_MAX_UUID_SIZE];
    kal_uint8 uuid_len;
    kal_uint8 goep_conn_id;
    kal_uint32 headerlen;
} goep_abort_ind_struct;


typedef struct 
{
    kal_uint8 ref_count;
    kal_uint16 msg_len;
    kal_uint8 uuid[GOEP_MAX_UUID_SIZE];
    kal_uint8 uuid_len;
    kal_uint8 goep_conn_id;
    kal_uint8 server_supported_formats;
} goep_opp_supported_formats_ind_struct;


typedef struct
{
    kal_uint8 ref_count;
    kal_uint16 msg_len;
    kal_uint8 uuid[GOEP_MAX_UUID_SIZE];
    kal_uint8 uuid_len;
    kal_uint8 req_id;
    kal_uint8 goep_conn_id;
    kal_uint8 passwd[GOEP_MAX_PASSWD_SIZE];
    kal_uint8 passwd_len;
    kal_uint8 realm_str[GOEP_MAX_REALM_SIZE];
    kal_uint8 realm_len;
    goep_role_enum goep_role;
} goep_auth_req_struct;

typedef struct
{
    kal_uint8 ref_count;
    kal_uint16 msg_len;
    kal_uint8 uuid[GOEP_MAX_UUID_SIZE];
    kal_uint8 uuid_len;
    kal_uint8 req_id;
    goep_bd_addr_struct bd_addr;
    goep_tp_type_enum tp_type;
    kal_uint8 *buf_ptr;
    kal_uint16 buf_size;
    kal_bool auth_use;
    kal_uint8 client_pwd[GOEP_MAX_PASSWD_SIZE];
    kal_uint8 client_pwd_len;
    kal_uint8 client_realm[GOEP_MAX_REALM_SIZE];
    kal_uint8 client_realm_len;
} goep_connect_req_struct;

typedef struct
{
    kal_uint8 ref_count;
    kal_uint16 msg_len;
    kal_uint8 uuid[GOEP_MAX_UUID_SIZE];
    kal_uint8 uuid_len;
    kal_uint8 req_id;
    kal_uint8 goep_conn_id;
    kal_uint8 rsp_code;
    kal_uint16 peer_mru;
    kal_uint32 cm_conn_id;
} goep_connect_cnf_struct;

typedef struct
{
    kal_uint8 ref_count;
    kal_uint16 msg_len;
    kal_uint8 uuid[GOEP_MAX_UUID_SIZE];
    kal_uint8 uuid_len;    
    kal_uint8 goep_conn_id;
    goep_pkt_type_enum pkt_type;
    goep_put_type_enum put_type;
    kal_uint8 obj_mime_type[GOEP_MAX_MIME_TYPE];
    kal_uint32 total_obj_len;
    kal_uint8 obj_name[GOEP_MAX_OBJ_NAME];
    kal_uint8 *frag_ptr;
    kal_uint16 frag_len;
} goep_push_req_struct;

typedef struct
{
    kal_uint8 ref_count;
    kal_uint16 msg_len;
    kal_uint8 uuid[GOEP_MAX_UUID_SIZE];
    kal_uint8 uuid_len;
    kal_uint8 goep_conn_id;
    kal_uint8 rsp_code;
    kal_uint8 reason;   /* failed reason. added for application */
} goep_push_cnf_struct;

typedef struct
{
    kal_uint8 ref_count;
    kal_uint16 msg_len;
    kal_uint8 uuid[GOEP_MAX_UUID_SIZE];
    kal_uint8 uuid_len;    
    kal_uint8 goep_conn_id;
    goep_pkt_type_enum pkt_type;
    kal_uint8 obj_mime_type[GOEP_MAX_MIME_TYPE];
    kal_uint8 obj_name[GOEP_MAX_OBJ_NAME];
} goep_pull_req_struct;

typedef struct
{
    kal_uint8 ref_count;
    kal_uint16 msg_len;
    kal_uint8 uuid[GOEP_MAX_UUID_SIZE];
    kal_uint8 uuid_len;
    kal_uint8 goep_conn_id;
    goep_pkt_type_enum pkt_type;
    kal_uint8 rsp_code;
    kal_uint32 total_obj_len;
    kal_uint8 *frag_ptr;
    kal_uint16 frag_len;
    kal_bool to_self;   /* added for continue parsing large folder content */
    kal_uint8 reason;   /* failed reason. added for application */
} goep_pull_cnf_struct;

typedef struct
{
    kal_uint8 ref_count;
    kal_uint16 msg_len;
    kal_uint8 uuid[GOEP_MAX_UUID_SIZE];
    kal_uint8 uuid_len;    
    kal_uint8 goep_conn_id;
    goep_set_folder_type_enum setpath_flag;
    kal_uint8 folder_name[GOEP_MAX_FOLDER_NAME];
} goep_set_folder_req_struct;

typedef struct
{
    kal_uint8 ref_count;
    kal_uint16 msg_len;
    kal_uint8 uuid[GOEP_MAX_UUID_SIZE];
    kal_uint8 uuid_len;
    kal_uint8 goep_conn_id;
    kal_uint8 rsp_code;
    kal_uint8 reason;   /* failed reason. added for application */
} goep_set_folder_cnf_struct;

typedef struct
{
    kal_uint8 ref_count;
    kal_uint16 msg_len;
    kal_uint8 uuid[GOEP_MAX_UUID_SIZE];
    kal_uint8 uuid_len;
    kal_uint8 goep_conn_id;
} goep_abort_req_struct;

typedef struct
{
    kal_uint8 ref_count;
    kal_uint16 msg_len;
    kal_uint8 uuid[GOEP_MAX_UUID_SIZE]; 
    kal_uint8 uuid_len;  
    kal_uint8 req_id;
    kal_uint8 goep_conn_id;
} goep_disconnect_req_struct;

typedef struct
{
    kal_uint8 ref_count;
    kal_uint16 msg_len;
    kal_uint8 uuid[GOEP_MAX_UUID_SIZE];
    kal_uint8 uuid_len;
    kal_uint8 goep_conn_id;
    kal_uint8 rsp_code;
} goep_disconnect_cnf_struct;

typedef struct
{
    kal_uint8 ref_count;
    kal_uint16 msg_len;
    kal_uint8 uuid[GOEP_MAX_UUID_SIZE];
    kal_uint8 uuid_len;
    kal_uint8 req_id;
    kal_uint8 goep_conn_id;
    kal_uint8 reason; 
} goep_disconnect_ind_struct;

typedef struct
{
    kal_uint8 ref_count;
    kal_uint16 msg_len;
    kal_uint8 uuid[GOEP_MAX_UUID_SIZE];
    kal_uint8 uuid_len;
    kal_uint8 goep_conn_id;
    kal_uint8 req_id;
    kal_uint8 realm[GOEP_MAX_REALM_SIZE];
    kal_uint8 realm_len;
    kal_uint8 dev_name[GOEP_MAX_DEV_NAME];
    goep_bd_addr_struct bd_addr;
    kal_uint8 options;
} goep_auth_ind_struct;

typedef struct
{
    kal_uint8 ref_count;
    kal_uint16 msg_len;
    kal_uint8 goep_conn_id;
    kal_uint8 uuid[GOEP_MAX_UUID_SIZE];
    kal_uint8 uuid_len;    
    kal_uint8 passwd[GOEP_MAX_PASSWD_SIZE];
    kal_uint8 passwd_len;
    kal_uint8 userid[GOEP_MAX_USERID_SIZE];
    kal_uint8 userid_len;
} goep_auth_rsp_struct;

typedef struct
{
    kal_uint8 ref_count;
    kal_uint16 msg_len;
    kal_uint8 uuid[GOEP_MAX_UUID_SIZE];
    kal_uint8 uuid_len;
    kal_uint8 req_id;
    kal_uint8 goep_conn_id;
    kal_uint32 timeticker;
} goep_register_client_req_struct;

typedef struct
{
    kal_uint8 ref_count;
    kal_uint16 msg_len;
    kal_uint8 uuid[GOEP_MAX_UUID_SIZE]; 
    kal_uint8 uuid_len;  
    kal_uint8 req_id;
    kal_uint8 goep_conn_id;
    kal_uint32 timeticker;
} goep_register_client_cnf_struct;

typedef struct
{
    kal_uint8 ref_count;
    kal_uint16 msg_len;
    kal_uint8 uuid[GOEP_MAX_UUID_SIZE];
    kal_uint8 uuid_len;
    kal_uint8 goep_conn_id;
    kal_uint8 rsp_code;
} goep_cnf_struct;

typedef goep_cnf_struct goep_deregister_server_cnf_struct;
typedef goep_cnf_struct goep_auth_cnf_struct;
typedef goep_cnf_struct goep_abort_cnf_struct;

typedef struct
{
    kal_uint8 ref_count;
    kal_uint16 msg_len;
    kal_uint8 goep_conn_id;
    kal_uint8 rsp_code;
} goep_rsp_struct;

typedef goep_rsp_struct goep_authorize_rsp_struct;
typedef goep_rsp_struct goep_push_rsp_struct;
typedef goep_rsp_struct goep_set_folder_rsp_struct;
typedef goep_rsp_struct goep_abort_rsp_struct;

/****** GOEP PROFILE****/
#define OBEX_EXTRA_SERVER_NUM 2
/****** END of GOEP PROFILE****/

/* GOEP for catcher */
/* goeps for catcher*/
typedef  goep_register_server_req_struct  bt_goeps_register_server_req_struct;
typedef  goep_register_server_cnf_struct  bt_goeps_register_server_cnf_struct;
typedef  goep_deregister_server_req_struct  bt_goeps_deregister_server_req_struct;
typedef  goep_deregister_server_cnf_struct  bt_goeps_deregister_server_cnf_struct;
typedef  goep_connect_ind_struct  bt_goeps_connect_ind_struct;
typedef  goep_connect_rsp_struct  bt_goeps_connect_rsp_struct;
typedef  goep_push_ind_struct  bt_goeps_push_ind_struct;
typedef  goep_push_rsp_struct  bt_goeps_push_rsp_struct;
typedef  goep_pull_ind_struct  bt_goeps_pull_ind_struct;
typedef  goep_pull_rsp_struct  bt_goeps_pull_rsp_struct;
typedef  goep_set_folder_ind_struct  bt_goeps_set_folder_ind_struct;
typedef  goep_set_folder_rsp_struct  goeps_set_folder_rsp_struct;
typedef  goep_abort_ind_struct  bt_goeps_abort_ind_struct;
typedef  goep_abort_rsp_struct  bt_goeps_abort_rsp_struct;
typedef  goep_auth_req_struct  bt_goeps_auth_req_struct;
typedef  goep_auth_cnf_struct  bt_goeps_auth_cnf_struct;
typedef  goep_disconnect_req_struct  bt_goeps_disconnect_req_struct;
typedef  goep_disconnect_ind_struct  bt_goeps_disconnect_ind_struct;
typedef  goep_auth_ind_struct  bt_goeps_auth_ind_struct;
typedef  goep_auth_rsp_struct  bt_goeps_auth_rsp_struct;
typedef  goep_authorize_ind_struct  bt_goeps_authorize_ind_struct;
typedef  goep_authorize_rsp_struct  bt_goeps_authorize_rsp_struct;
typedef  goep_disconnect_ind_struct  bt_goeps_tpdisconnect_ind_struct;
/* goepc for catcher*/
typedef  goep_register_client_req_struct  bt_goepc_register_client_req_struct;
typedef  goep_register_client_cnf_struct  bt_goepc_register_client_cnf_struct;
typedef  goep_connect_req_struct  bt_goepc_connect_req_struct;
typedef  goep_connect_cnf_struct  bt_goepc_connect_cnf_struct;
typedef  goep_push_req_struct  bt_goepc_push_req_struct;
typedef  goep_push_cnf_struct  bt_goepc_push_cnf_struct;
typedef  goep_pull_req_struct  bt_goepc_pull_req_struct;
typedef  goep_pull_cnf_struct  bt_goepc_pull_cnf_struct;
typedef  goep_set_folder_req_struct  bt_goepc_set_folder_req_struct;
typedef  goep_set_folder_cnf_struct  bt_goepc_set_folder_cnf_struct;
typedef  goep_abort_req_struct  bt_goepc_abort_req_struct;
typedef  goep_abort_cnf_struct  bt_goepc_abort_cnf_struct;
typedef  goep_disconnect_req_struct  bt_goepc_disconnect_req_struct;
typedef  goep_disconnect_cnf_struct  bt_goepc_disconnect_cnf_struct;
typedef  goep_disconnect_ind_struct  bt_goepc_disconnect_ind_struct;
typedef  goep_auth_ind_struct  bt_goepc_auth_ind_struct;
typedef  goep_auth_rsp_struct  bt_goepc_auth_rsp_struct;
typedef  goep_opp_supported_formats_ind_struct  bt_goepc_opp_supported_formats_ind_struct;
typedef  goep_disconnect_ind_struct  bt_goepc_tpdisconnect_ind_struct;
/****** opps for catcher ******/
typedef  goep_register_server_req_struct  bt_opps_register_server_req_struct;
typedef  goep_register_server_cnf_struct  bt_opps_register_server_cnf_struct;
typedef  goep_deregister_server_req_struct  bt_opps_deregister_server_req_struct;
typedef  goep_deregister_server_cnf_struct  bt_opps_deregister_server_cnf_struct;
typedef  goep_connect_ind_struct  bt_opps_connect_ind_struct;
typedef  goep_connect_rsp_struct  bt_opps_connect_rsp_struct;
typedef  goep_push_ind_struct  bt_opps_push_ind_struct;
typedef  goep_push_rsp_struct  bt_opps_push_rsp_struct;
typedef  goep_pull_ind_struct  bt_opps_pull_ind_struct;
typedef  goep_pull_rsp_struct  bt_opps_pull_rsp_struct;
typedef  goep_set_folder_ind_struct  bt_opps_set_folder_ind_struct;
typedef  goep_set_folder_rsp_struct  opps_set_folder_rsp_struct;
typedef  goep_abort_ind_struct  bt_opps_abort_ind_struct;
typedef  goep_abort_rsp_struct  bt_opps_abort_rsp_struct;
typedef  goep_auth_req_struct  bt_opps_auth_req_struct;
typedef  goep_auth_cnf_struct  bt_opps_auth_cnf_struct;
typedef  goep_disconnect_req_struct  bt_opps_disconnect_req_struct;
typedef  goep_disconnect_ind_struct  bt_opps_disconnect_ind_struct;
typedef  goep_auth_ind_struct  bt_opps_auth_ind_struct;
typedef  goep_auth_rsp_struct  bt_opps_auth_rsp_struct;
typedef  goep_authorize_ind_struct  bt_opps_authorize_ind_struct;
typedef  goep_authorize_rsp_struct  bt_opps_authorize_rsp_struct;
typedef  goep_disconnect_ind_struct  bt_opps_tpdisconnect_ind_struct;
/* oppc for catcher*/
typedef  goep_register_client_req_struct  bt_oppc_register_client_req_struct;
typedef  goep_register_client_cnf_struct  bt_oppc_register_client_cnf_struct;
typedef  goep_connect_req_struct  bt_oppc_connect_req_struct;
typedef  goep_connect_cnf_struct  bt_oppc_connect_cnf_struct;
typedef  goep_push_req_struct  bt_oppc_push_req_struct;
typedef  goep_push_cnf_struct  bt_oppc_push_cnf_struct;
typedef  goep_pull_req_struct  bt_oppc_pull_req_struct;
typedef  goep_pull_cnf_struct  bt_oppc_pull_cnf_struct;
typedef  goep_set_folder_req_struct  bt_oppc_set_folder_req_struct;
typedef  goep_set_folder_cnf_struct  bt_oppc_set_folder_cnf_struct;
typedef  goep_abort_req_struct  bt_oppc_abort_req_struct;
typedef  goep_abort_cnf_struct  bt_oppc_abort_cnf_struct;
typedef  goep_disconnect_req_struct  bt_oppc_disconnect_req_struct;
typedef  goep_disconnect_cnf_struct  bt_oppc_disconnect_cnf_struct;
typedef  goep_disconnect_ind_struct  bt_oppc_disconnect_ind_struct;
typedef  goep_auth_ind_struct  bt_oppc_auth_ind_struct;
typedef  goep_auth_rsp_struct  bt_oppc_auth_rsp_struct;
typedef  goep_opp_supported_formats_ind_struct  bt_oppc_opp_supported_formats_ind_struct;
typedef  goep_disconnect_ind_struct  bt_oppc_tpdisconnect_ind_struct;

/* ftps for catcher*/
typedef  goep_register_server_req_struct  bt_ftps_register_server_req_struct;
typedef  goep_register_server_cnf_struct  bt_ftps_register_server_cnf_struct;
typedef  goep_deregister_server_req_struct  bt_ftps_deregister_server_req_struct;
typedef  goep_deregister_server_cnf_struct  bt_ftps_deregister_server_cnf_struct;
typedef  goep_connect_ind_struct  bt_ftps_connect_ind_struct;
typedef  goep_connect_rsp_struct  bt_ftps_connect_rsp_struct;
typedef  goep_push_ind_struct  bt_ftps_push_ind_struct;
typedef  goep_push_rsp_struct  bt_ftps_push_rsp_struct;
typedef  goep_pull_ind_struct  bt_ftps_pull_ind_struct;
typedef  goep_pull_rsp_struct  bt_ftps_pull_rsp_struct;
typedef  goep_set_folder_ind_struct  bt_ftps_set_folder_ind_struct;
typedef  goep_set_folder_rsp_struct  ftps_set_folder_rsp_struct;
typedef  goep_abort_ind_struct  bt_ftps_abort_ind_struct;
typedef  goep_abort_rsp_struct  bt_ftps_abort_rsp_struct;
typedef  goep_auth_req_struct  bt_ftps_auth_req_struct;
typedef  goep_auth_cnf_struct  bt_ftps_auth_cnf_struct;
typedef  goep_disconnect_req_struct  bt_ftps_disconnect_req_struct;
typedef  goep_disconnect_ind_struct  bt_ftps_disconnect_ind_struct;
typedef  goep_auth_ind_struct  bt_ftps_auth_ind_struct;
typedef  goep_auth_rsp_struct  bt_ftps_auth_rsp_struct;
typedef  goep_authorize_ind_struct  bt_ftps_authorize_ind_struct;
typedef  goep_authorize_rsp_struct  bt_ftps_authorize_rsp_struct;
typedef  goep_disconnect_ind_struct  bt_ftps_tpdisconnect_ind_struct;
/* ftpc for catcher*/
typedef  goep_register_client_req_struct  bt_ftpc_register_client_req_struct;
typedef  goep_register_client_cnf_struct  bt_ftpc_register_client_cnf_struct;
typedef  goep_connect_req_struct  bt_ftpc_connect_req_struct;
typedef  goep_connect_cnf_struct  bt_ftpc_connect_cnf_struct;
typedef  goep_push_req_struct  bt_ftpc_push_req_struct;
typedef  goep_push_cnf_struct  bt_ftpc_push_cnf_struct;
typedef  goep_pull_req_struct  bt_ftpc_pull_req_struct;
typedef  goep_pull_cnf_struct  bt_ftpc_pull_cnf_struct;
typedef  goep_set_folder_req_struct  bt_ftpc_set_folder_req_struct;
typedef  goep_set_folder_cnf_struct  bt_ftpc_set_folder_cnf_struct;
typedef  goep_abort_req_struct  bt_ftpc_abort_req_struct;
typedef  goep_abort_cnf_struct  bt_ftpc_abort_cnf_struct;
typedef  goep_disconnect_req_struct  bt_ftpc_disconnect_req_struct;
typedef  goep_disconnect_cnf_struct  bt_ftpc_disconnect_cnf_struct;
typedef  goep_disconnect_ind_struct  bt_ftpc_disconnect_ind_struct;
typedef  goep_auth_ind_struct  bt_ftpc_auth_ind_struct;
typedef  goep_auth_rsp_struct  bt_ftpc_auth_rsp_struct;
typedef  goep_opp_supported_formats_ind_struct  bt_ftpc_opp_supported_formats_ind_struct;
typedef  goep_disconnect_ind_struct  bt_ftpc_tpdisconnect_ind_struct;
/***** for catcher ******/


typedef enum
{
    BT_OPP_READY_FEATURE ,     /* Not a auto fature. User could compose it*/
    BT_OPP_PULL_FEATURE = 1,   /* pull a vcard */
    BT_OPP_PUSH_FEATURE = 2,   /* push a object */
    BT_OPP_EXCHANGE_FEATURE    /* Exchange object */
} BT_OPC_FEATURE;

/****** OPP PROFILE****/
#define OPP_INVALID_CONN_ID  0xff
#define OPP_DEFAULT_CONN_ID  0xff
#define OPP_INVALID_CM_ID    0xff
#define OPP_DEFAULT_CM_ID    0xff
#define OPP_INVALID_REQ_ID   0xff
#define OPP_DEFAULT_REQ_ID   0xff

#define OPP_MAX_FLDR_LENGTH     41
#define OPP_MAX_PATH_LENGTH     260
#define OPP_MAX_OBJ_NAME_LENGTH (128 * 2)
#define OPP_MAX_OBJ_MIME_LENGTH 80
#define OPPC_MAX_OBEX_PACKET_LENGTH 14336
#define OPPS_MAX_OBEX_PACKET_LENGTH 14336
/****** END OF OPP PROFILE****/


/****** FTP PROFILE****/
#define FS_FOLDER_DELIMITOR             L"\\"
#define BTMTK_FTPS_MAX_FILEPATH            512

/****** END OF FTP PROFILE****/





/**** Start of BIP Structure ****/

typedef enum
{
    BT_BIP_NULL,
    BT_BIP_INITIATOR_ROLE,
    BT_BIP_RESPONDER_ROLE
} bt_bip_session_role_enum;

typedef enum
{
    BT_BIP_TRANS_NONE,
    BT_BIP_TRANS_STRECH,
    BT_BIP_TRANS_CROP,
    BT_BIP_TRANS_FILL,
    BT_BIP_TRANS_MAX
} bt_bip_img_trans_enum;

typedef struct
{
    bt_bip_img_format_enum encoding;
    kal_bool define_pixel_with_range;
    kal_uint16 max_pixel_width;
    kal_uint16 min_pixel_width;
    kal_uint16 max_pixel_height;
    kal_uint16 min_pixel_height;
    kal_uint16 specified_pixel_width;
    kal_uint16 specified_pixel_height;
    kal_uint8 img_handle[8];
    kal_uint8 friendly_name[BT_BIP_MAX_IMG_NAME_LEN];
    kal_uint32 size;
    kal_uint8 created[BT_BIP_MAX_TIME_LEN];     /* (YYYYMMDDTHHMMSS)(Z) */
    kal_uint8 modified[BT_BIP_MAX_TIME_LEN];    /* (YYYYMMDDTHHMMSS)(Z) */
} bt_bip_img_info_struct;

typedef struct
{
	U16 width;
	U16 height;
	U16 width2;
	U16 height2;
} bt_bip_pixel_struct;

typedef struct
{
	U32 encoding;	/* REQUIRED, 0 means get native format image in GetImage */
	bt_bip_pixel_struct pixel;	/* REQUIRED, only fixed sizes for PutImage */
	U32 size;
	U32 transformation; /* bt_bip_img_trans_enum */
} bt_bip_img_desc_struct;

typedef bt_addr_struct bt_bip_bd_addr_struct;

/*   MSG_ID_BT_BIP_OBEX_AUTH_IND */
typedef struct
{
    kal_uint8 ref_count;
    kal_uint16 msg_len;
    kal_uint32 cm_conn_id;
    kal_uint8 result;
    kal_uint8 option;
    kal_uint8 realm_len;
    kal_uint8 realm[GOEP_MAX_REALM_SIZE];
} bt_bip_obex_auth_ind_struct;

typedef struct
{
    kal_uint8 uid[GOEP_MAX_USERID_SIZE];
    kal_int8 uid_len;
    kal_uint8 pwd[GOEP_MAX_PASSWD_SIZE];
    kal_uint8 pwd_len;
} bt_bip_dig_response_struct;

/*  MSG_ID_BT_BIP_OBEX_AUTH_RSP */
typedef struct
{
    kal_uint8 ref_count;
    kal_uint16 msg_len;
    kal_uint32 cm_conn_id;
    kal_uint8 result;
    /* bt_bip_session_role_enum session_role; */
    bt_bip_dig_response_struct dig_rsp;    
} bt_bip_obex_auth_rsp_struct;

typedef struct
{
    kal_uint8 uid[GOEP_MAX_USERID_SIZE];
    kal_int8 uid_len;
    kal_uint8 pwd[GOEP_MAX_PASSWD_SIZE];
    kal_uint8 pwd_len;
    kal_uint8 realm[GOEP_MAX_REALM_SIZE];
    kal_uint8 realm_len;
} bt_bip_dig_challenge_struct;

/*  MSG_ID_BT_BIP_OBEX_AUTH_REQ */
typedef struct
{
    kal_uint8 ref_count;
    kal_uint16 msg_len;
    kal_uint32 cm_conn_id;
    /* bt_bip_session_role_enum session_role; */
    bt_bip_dig_challenge_struct dig_chal;
} bt_bip_obex_auth_req_struct;

/*  MSG_ID_BT_BIP_OBEX_AUTH_CNF */
typedef struct
{
    kal_uint8 ref_count;
    kal_uint16 msg_len;
    kal_uint32 cm_conn_id;
    /* bt_bip_session_role_enum session_role; */
    kal_uint8 cnf_code;
} bt_bip_obex_auth_cnf_struct;

/**** End of BIP Structure ****/

/* for OBEX authentication */
typedef struct
{
    kal_uint8 uuid[GOEP_MAX_UUID_SIZE];
    kal_uint8 uuid_len;
    kal_uint8 passwd[GOEP_MAX_PASSWD_SIZE];
    kal_uint8 passwd_len;
    kal_uint8 realm_str[GOEP_MAX_REALM_SIZE];
    kal_uint8 realm_len;
} auth_chal_struct;

typedef kal_uint8(*AUTH_IND_HANDLER) (goep_auth_ind_struct *auth_ind);

struct auth_ind_handler_struct
{
    kal_uint8 uuid[GOEP_MAX_UUID_SIZE];
    kal_uint8 uuid_len;

    AUTH_IND_HANDLER auth_ind_handler;

    struct auth_ind_handler_struct *next;

};

typedef struct auth_ind_handler_struct auth_ind_handler_struct;

/* END for OBEX authentication */

/* csj */
/* struct used by pbap */
/*#if defined(__BT_PBAP_NEW_PROFILE__) || defined(__MMI_PBAP_NEW_SUPPORT__) */
#ifndef __BT_PBAP_PROFILE__

#define PBAP_FILTER_SIZE                8
#define BT_PBAP_MAX_DEV_NAME_LEN        80

typedef struct _PbapVcardFilter
{
    /* Array of 8 bytes for this 64-bit filter value */
    kal_uint8 byte[PBAP_FILTER_SIZE];
} PbapVcardFilter;

typedef kal_uint8 PbapVcardFormat;

#define VCARD_FORMAT_21        0x00     /* Version 2.1 format */
#define VCARD_FORMAT_30        0x01     /* Version 3.0 format */

/*SearchAttributes*/
#define PBAP_SEARCH_ATTRIB_NAME        0x00    /* Search by Name */
#define PBAP_SEARCH_ATTRIB_NUMBER      0x01    /* Search by Number */
#define PBAP_SEARCH_ATTRIB_SOUND       0x02    /* Search by Sound */

/*vcard sort order in listing*/
#define PBAP_SORT_ORDER_INDEXED        0x00    /* Indexed sorting */
#define PBAP_SORT_ORDER_ALPHA          0x01    /* Alphabetical sorting */
#define PBAP_SORT_ORDER_PHONETICAL     0x02    /* Phonetical sorting */

/*---------------------------------------------------------------------------
 * PB_MAX_NAME_LEN constant
 *
 *     Maximum number of characters allowed for pathnames + 1 (null-
 *     terminating character).
 */
#define PBAP_MAX_NAME_LEN         128
#define MAX_PBAP_SEARCH_VALUE_LENGTH PBAP_MAX_NAME_LEN

/*---------------------------------------------------------------------------
 * PbStatus type
 *
 *     This type is returned from most phonebook APIs to indicate the success
 *     or failure of the operation. In many cases, BT_STATUS_PENDING
 *     is returned, meaning that a future callback will indicate the
 *     result of the operation.
 */

typedef kal_uint8 PbStatus;

#define PB_STATUS_SUCCESS           0   /* XA_STATUS_SUCCESS */
#define PB_STATUS_FAILED            1   /* XA_STATUS_FAILED */
#define PB_STATUS_NO_RESOURCES      12  /* XA_STATUS_NO_RESOURCES */
#define PB_STATUS_NOT_FOUND         13  /* XA_STATUS_NOT_FOUND */
#define PB_STATUS_UNKNOWN_REMOTE    14  /* XA_STATUS_DEVICE_NOT_FOUND */
#define PB_STATUS_INUSE             5   /* XA_STATUS_IN_USE */
#define PB_STATUS_NOT_SUPPORTED     23  /* XA_STATUS_NOT_SUPPORTED */
#define PB_STATUS_PENDING           2   /* XA_STATUS_PENDING */
/* End of PbStatus */
typedef kal_uint8 PbapRespCode;

/* Group: Successful response codes */

#define PBRC_CONTINUE              0x10 /* Continue */
#define PBRC_STATUS_SUCCESS        0x20 /* Success */

/* Group: Failure response codes */

#define PBRC_BAD_REQUEST           0x40 /* Bad Request */
#define PBRC_UNAUTHORIZED          0x41 /* Unauthorized */
#define PBRC_FORBIDDEN             0x43 /* Forbidden - operation is understood */
#define PBRC_NOT_FOUND             0x44 /* Not Found */
#define PBRC_NOT_ACCEPTABLE        0x46 /* Not Acceptable */
#define PBRC_PRECONDITION_FAILED   0x4c /* Precondition Failed */
#define PBRC_NOT_IMPLEMENTED       0x51 /* Not Implemented */
#define PBRC_SERVICE_UNAVAILABLE   0x53 /* Service Unavailable */
#define PBRC_LINK_DISCONNECT       0x80 /* Transport connection has been disconnected. */
#define PBRC_INTERNAL_SERVER_ERR   0x50 /* OBRC_INTERNAL_SERVER_ERR */

/* End of PbapRespCode */

#define PBAP_ADP_WORK_FOLDER     (L"Z:\\@pbap\\")
#define PBAP_ADP_FOLDER_FILE        (L"Z:\\@pbap\\folder.tmp")
#define PBAP_ADP_LIST_FILE   (L"Z:\\@pbap\\list.tmp")
#define PBAP_ADP_ENTRY_FILE   (L"Z:\\@pbap\\entry.tmp")

/*---------------------------------------------------------------------------
 * PbapAppParmsTag type
 *
 *     Describes the tag values used in the Application Parameters OBEX header
 *     that are used on both the Phonebook Access client and server side.
 */
typedef kal_uint8 PbapAppParmsTag;

#define PBAP_TAG_ORDER              0x01        /* 1-byte, 0x00 (indexed), 0x01 (alpha), or 0x02 (phonetic) */
#define PBAP_TAG_SEARCH_VALUE       0x02        /* Variable length text string */
#define PBAP_TAG_SEARCH_ATTRIB      0x03        /* 1-byte, 0x00 (Name), 0x01 (Number), or 0x02 (Sound) */
#define PBAP_TAG_MAX_LIST_COUNT     0x04        /* 2-bytes, 0x0000 to 0xFFFF */
#define PBAP_TAG_LIST_OFFSET        0x05        /* 2-bytes, 0x0000 to 0xFFFF */
#define PBAP_TAG_FILTER             0x06        /* 8-bytes, 64 bit mask */
#define PBAP_TAG_FORMAT             0x07        /* 1-byte, 0x00 = 2.1, 0x01 = 3.0 */
#define PBAP_TAG_PHONEBOOK_SIZE     0x08        /* 2-bytes, 0x0000 to 0xFFFF */
#define PBAP_TAG_MISSED_CALLS       0x09        /* 1-byte, 0x00 to 0xFF */

/* End of PbapAppParmsTag */

/*---------------------------------------------------------------------------
 * pbap_set_folder_type_enum type
 *
 *     Flags used in the SetFolder operation.  PBAP specification requires
 *     that the PBAP_SETPB_DONT_CREATE flag is always set.
 */
typedef enum
{
    PBAP_FORWARD_FOLDER = 0,
    PBAP_BACK_FOLDER,
    PBAP_ROOT_FOLDER
} pbap_set_folder_type_enum;


typedef enum
{
    PBAP_CNF_SUCCESS,
    PBAP_CNF_FAILED,
    PBAP_CNF_TOTAL
} bt_pbap_cnf_enum;

typedef struct
{
    kal_uint8 ref_count;
    kal_uint16 msg_len;
    kal_uint8 security_level;
} bt_pbap_register_req_struct;

typedef struct
{
    kal_uint8 ref_count;
    kal_uint16 msg_len;
    kal_uint8 register_result;
} bt_pbap_register_rsp_struct;

typedef struct
{
    kal_uint8 ref_count;
    kal_uint16 msg_len;
} bt_pbap_deregister_req_struct;

typedef struct
{
    kal_uint8 ref_count;
    kal_uint16 msg_len;
    kal_uint8 register_result;
} bt_pbap_deregister_rsp_struct;

typedef struct
{
    kal_uint8 ref_count;
    kal_uint16 msg_len;
} bt_pbap_abort_ind_struct;

typedef struct
{
    kal_uint8 ref_count;
    kal_uint16 msg_len;
} bt_pbap_disconnect_ind_struct;

typedef bt_addr_struct bt_pbap_bd_addr_struct;

typedef struct
{
    kal_uint8               ref_count;
    kal_uint16              msg_len;
    bt_pbap_bd_addr_struct   bd_addr;
    kal_uint8               dev_name[BT_PBAP_MAX_DEV_NAME_LEN];
} bt_pbap_authorize_ind_struct;

typedef struct
{
    kal_uint8               ref_count;
    kal_uint16              msg_len;
    kal_uint8               cnf_code;
} bt_pbap_authorize_rsp_struct;

typedef struct
{
    kal_uint8 ref_count;
    kal_uint16 msg_len;
    kal_uint32 cm_conn_id;
    bt_pbap_bd_addr_struct bd_addr;
    kal_uint8 dev_name[BT_PBAP_MAX_DEV_NAME_LEN];
} bt_pbap_connect_ind_struct;

typedef struct
{
    kal_uint8 ref_count;
    kal_uint16 msg_len;
    kal_uint32 cm_conn_id;
    kal_bool disconnect_tp_directly;
} bt_pbap_disconnect_req_struct;

typedef struct
{
    kal_uint8 ref_count;
    kal_uint16 msg_len;
} bt_pbap_disconnect_rsp_struct;

typedef struct
{
    kal_uint8 ref_count;
    kal_uint16 msg_len;
    kal_uint32 cm_conn_id;
    kal_uint8 cnf_code;
} bt_pbap_connect_rsp_struct;

typedef struct
{
    kal_uint8 ref_count;
    kal_uint16 msg_len;
    kal_uint8 dev_name[BT_PBAP_MAX_DEV_NAME_LEN];
} bt_pbap_auth_ind_struct;

typedef struct
{
    kal_uint8 ref_count;
    kal_uint16 msg_len;
    kal_uint8 cancel;
    /* OBEX Authentication password */
    kal_uint8 password[20];
    /* OBEX Authentication userId */
    kal_uint8 userId[20];
    /* OBEX Authentication realm */
    kal_uint8 realm[20];
} bt_pbap_auth_cnf_struct;

typedef struct
{
    kal_uint8 ref_count;
    kal_uint16 msg_len;
    pbap_set_folder_type_enum type;
    kal_uint16 name[PBAP_MAX_NAME_LEN/2 + 1];
} bt_pbap_set_path_ind_struct;

typedef struct
{
    kal_uint8 ref_count;
    kal_uint16 msg_len;
    kal_uint8 result;
} bt_pbap_set_path_rsp_struct;

typedef struct
{
    kal_uint8 ref_count;
    kal_uint16 msg_len;
    kal_uint16 objectName[PBAP_MAX_NAME_LEN/2 + 1];  /* (*.vcf) */
    PbapVcardFilter filter;
    PbapVcardFormat format;
} bt_pbap_read_entry_ind_struct;

typedef struct
{
    kal_uint8 ref_count;
    kal_uint16 msg_len;
    kal_uint8 result;   /* PBSTATUS */
} bt_pbap_read_entry_rsp_struct;

typedef struct
{
    kal_uint8 ref_count;
    kal_uint16 msg_len;
    kal_uint16 pbName[PBAP_MAX_NAME_LEN/2 + 1];
    PbapVcardFilter filter;
    PbapVcardFormat format;
    kal_uint16 maxListCount;
    kal_uint16 listStartOffset;
    kal_uint8 to_self;
} bt_pbap_read_folder_ind_struct;

typedef struct
{
    kal_uint8 ref_count;
    kal_uint16 msg_len;
    kal_uint16 folderName[PBAP_MAX_NAME_LEN/2 + 1];
    kal_uint8 searchAttribute;
    kal_uint8 searchValue[MAX_PBAP_SEARCH_VALUE_LENGTH + 1];
    kal_uint8 searchValueLength;
    kal_uint8 order;
    kal_uint16 maxListCount;
    kal_uint16 listStartOffset;
    kal_uint8 to_self;
} bt_pbap_read_list_ind_struct;

typedef struct
{
    kal_uint8 ref_count;
    kal_uint16 msg_len;
    kal_uint8 result;
    kal_uint16 phoneBookSize; /*0xFFFF indicates have only data*/
    kal_uint16 newMissedCalls; /*0xFFFF indicates the type is not MCH*/
} bt_pbap_read_rsp_struct;

typedef bt_pbap_read_rsp_struct bt_pbap_read_folder_rsp_struct;
typedef bt_pbap_read_rsp_struct bt_pbap_read_list_rsp_struct;

#endif /* __BT_PBAP_PROFILE__ */ 

#endif /* __BLUETOOTH_STRUCT_H_ */ 

