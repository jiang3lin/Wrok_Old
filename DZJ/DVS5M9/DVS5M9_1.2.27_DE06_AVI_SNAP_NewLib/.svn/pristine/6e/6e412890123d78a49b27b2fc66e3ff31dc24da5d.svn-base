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
    *   bluetooth_sap.h
    *
    * Project:
    * --------
    *   Maui_Software
    *
    * Description:
    * ------------
    *   This file is defines SAP for BT.
    *
    * Author:
    * -------
    *   Brad Chang
    *
    *============================================================================
    *             HISTORY
    * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
    *------------------------------------------------------------------------------
    * $Revision: #2 $
    * $Modtime$
    * $Log$
    *
    /* After add/delete message group, pls modify btmtk_adp_check_msg_id_range() accordingly */

    MSG_ID_BT_NOTIFY_EVM_IND = MSG_ID_BT_START,

    /* Common for most platform start */
    MSG_ID_BT_RESTART_REQ,
    MSG_ID_BT_ENGINEER_MODE_POWERON_REQ,
    MSG_ID_BT_ENGINEER_MODE_POWERON_CNF,    
    MSG_ID_BT_MMI_RESET_REQ,
    MSG_ID_BT_MMI_RESET_CNF,
    MSG_ID_BT_RESET_REQ_IND,
    MSG_ID_BT_SET_BD_ADDR_REQ,
    MSG_ID_BT_GET_BD_ADDR_REQ,
    MSG_ID_BT_GET_BD_ADDR_CNF,
    MSG_ID_BT_GET_BT_VERSION_REQ,
    MSG_ID_BT_GET_BT_VERSION_CNF,   
    MSG_ID_BT_TEST_MODE_REQ,
    MSG_ID_BT_PCM_LB_REQ,
    MSG_ID_BT_PCM_LB_CNF,
    MSG_ID_BT_SSP_DEBUG_MODE_REQ,
    MSG_ID_BT_SSP_DEBUG_MODE_CNF,
    MSG_ID_BT_HOST_WAKE_UP_IND,
    MSG_ID_BT_GPIO_SET_REQ,
    MSG_ID_BT_GPIO_SET_CNF,
    MSG_ID_BT_ENGINEER_MODE_LOG_MASK_REQ,
    MSG_ID_BT_ENGINEER_MODE_LOG_MASK_CNF,
    MSG_ID_BT_ENGINEER_MODE_TX_TEST_REQ,
    MSG_ID_BT_ENGINEER_MODE_TX_TEST_CNF,
    MSG_ID_BT_ENGINEER_MODE_TX_TEST_END_REQ,
    MSG_ID_BT_ENGINEER_MODE_TX_TEST_END_CNF,
    MSG_ID_BT_ENGINEER_MODE_RX_TEST_REQ,
    MSG_ID_BT_ENGINEER_MODE_RX_TEST_CNF,
    MSG_ID_BT_ENGINEER_MODE_RX_TEST_END_REQ,
    MSG_ID_BT_ENGINEER_MODE_RX_TEST_END_CNF,
    MSG_ID_UART_PLUGOUT_IND,
    MSG_ID_UART_READY_TO_READ_IND,
    MSG_ID_UART_READY_TO_WRITE_IND,
    MSG_ID_BT_DATA_TO_READ_IND,
    MSG_ID_TIMER_EXPIRY,
    MSG_ID_BT_END = MSG_ID_TIMER_EXPIRY,

    /* GAP */
    #include "bluetooth_gap_message.h"

#ifdef __BT_SDP_PROFILE__
    /* SDP */
    #include "bluetooth_sdp_message.h"
#endif

#ifdef __BT_A2DP_PROFILE__
    /* A2DP */
    #include "bluetooth_a2dp_message.h"
#endif

#ifdef __BT_AVRCP_PROFILE__
    /* AVRCP */
    #include "bluetooth_avrcp_message.h"
#endif

#ifdef __BT_BIP_PROFILE__
    /* BIP */
    #include "bluetooth_bipi_message.h"
    #include "bluetooth_bipr_message.h"
#endif

#ifdef __BT_BPP_PROFILE__
    /* BPP */
    #include "bluetooth_bpp_message.h"
#endif
    /* DUN */
    /* FAX */

#ifdef __BT_FTC_PROFILE__
    /* FTP */
    #include "bluetooth_ftpc_message.h"
#endif

#ifdef __BT_FTS_PROFILE__
    #include "bluetooth_ftps_message.h"    
#endif

#ifdef __BT_GOEP_PROFILE__
    /* GOEP */
    #include "bluetooth_goepc_message.h"
    #include "bluetooth_goeps_message.h"
#endif

#ifdef __BT_HFG_PROFILE__
    /* HFG */
    #include "bluetooth_hfg_message.h"
    /* CHN */
    #include "bluetooth_chn_message.h"
#endif

#if defined(__BT_HIDD_PROFILE__) || defined(__BT_HIDH_PROFILE__)
    /* HID */
    #include "bluetooth_hid_message.h"	
#endif

    /* HSG */
#ifdef __BT_OPP_PROFILE__
    /* OPP */
    #include "bluetooth_oppc_message.h"
    #include "bluetooth_opps_message.h"
#endif

#ifdef __BT_PBAP_PROFILE__
    /* PBAP */
    #include "bluetooth_pbap_message.h"
#endif

#ifdef __BT_SIM_PROFILE__
    /* SIMAP */
    #include "bluetooth_simap_message.h"
#endif

#ifdef __BT_SPP_PROFILE__
    /* SPP */
    #include "bluetooth_spp_message.h"
#endif

#ifdef __BT_JSR82__
    /* JSR82 */
    #include "bluetooth_jsr82_message.h"
#endif

#ifdef __BT_MAPS_PROFILE__
    /* MAP */
    #include "bluetooth_map_message.h"
#endif
    
#ifdef __BT_VDP_PROFILE__
    /* VDP */
    #include "bluetooth_vdp_message.h"
#endif

    MSG_ID_END = 0xFFFFFFFF