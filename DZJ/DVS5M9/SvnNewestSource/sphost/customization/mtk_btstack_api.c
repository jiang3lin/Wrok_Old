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
/*******************************************************************************
 *
 * Filename:
 * ---------
 * bt_sample_code.c
 *
 * Project:
 * --------
 *   BT Project
 *
 * Description:
 * ------------
 *   This file is used to
 *
 * Author:
 * -------
 * Autumn Li
 *
 *==============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Revision:
 * $Modtime:
 * $Log:
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *==============================================================================
 *******************************************************************************/
#include <stdio.h>
#include <string.h>
#include "mtk/bt_mmi.h"

#include "os/_tx_port.h"
#include "os/_tx_api.h"

#include "debug_mod/dbg_api.h" /*PhilipDebug*/

#include "api/sp5k_os_api.h"
#include "api/sp5k_utility_api.h"
#include "api/sp5k_fs_api.h"
#include "api/sp5k_global_api.h"

#include "mtk/bt_message.h"
#include "mtk/bluetooth_gap_struct.h"
#include "mtk/bluetooth_spp_struct.h"

#include "api/sp5k_gfx_api.h"

#include "middleware/common_types.h"

#if 1 /*PhilipTest@20120528*/
static UINT32 mBtSendStartTime, mBtSendConfirmTime, mBtSendDataLen;

UINT32 gSemBtSendBuf;
#endif

#define DBG_BTSTACK_PROFLOG_EN (1)

void mtk_test_sendbuf(U8 *buf, U32 size);

#define SPP_SEND_MAX_BUFF_SIZE (1024*5)
#define SPP_RECV_MAX_BUFF_SIZE (1024)

extern TX_QUEUE *g_bt_queue_ptr;
TX_QUEUE *g_mtktest_queue_ptr = NULL;
U8 g_spp_port = 0;
bt_addr_struct g_spp_remote_addr = {0, 0, 0};

static U32 g_spp_send_size = SPP_SEND_MAX_BUFF_SIZE;
static U8 g_spp_send_buf[SPP_SEND_MAX_BUFF_SIZE];
static U8 *g_spp_send_ptr = NULL;

static U32 g_spp_send_fp;
static U32 g_spp_file_size;

static U8 g_spp_rec_buf[SPP_RECV_MAX_BUFF_SIZE];
static U32 g_spp_rec_size = SPP_RECV_MAX_BUFF_SIZE;
/*static U8 *g_spp_data_rec_ptr;*/

/*****************************************************************************
 * FUNCTION
 *  mtk_test_send_msg
 * DESCRIPTION
 *  Send MTK message to BT profile and stack
 * PARAMETERS
 *  msg_id    [IN]    MTK message ID
 *  data      [IN]    Message-specific data
 *  size      [IN]    Message-specific data size
 * RETURNS
 *  void
 *****************************************************************************/
void mtk_test_send_msg(unsigned int msg_id, void *data, unsigned int size)
{
    mtkMsgStruct msg;
    msg.ilm = sp5kMalloc(sizeof(ilm_struct));
    msg.ilm->msg_id = msg_id;
    msg.ilm->src_mod_id = MOD_MMI;
    msg.ilm->dest_mod_id = MOD_BT;
    msg.ilm->local_para_ptr = data;
    if (data)
    {
        local_para_struct *header = (local_para_struct *)data;
        /* msg_len: local_para_ptr length. Pls fill it even not mentioned in SAP document */
        header->msg_len = size;
    }
    if (sp5kOsQueueSend(g_bt_queue_ptr, &msg, TX_NO_WAIT) != 0)
    {
        Assert(0);
    }
}

/*****************************************************************************
 * FUNCTION
 *  mtk_test_poweron
 * DESCRIPTION
 *  To power on BT chip and stack
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void mtk_test_poweron(void)
{
    mtk_test_send_msg(MSG_ID_BT_POWERON_REQ, NULL, 0);
}

/*****************************************************************************
 * FUNCTION
 *  mtk_test_poweroff
 * DESCRIPTION
 *  To power off BT chip and stack.
 *    Step 1. Deinit all profiles
 *    Step 2. Power off procedure
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void mtk_test_poweroff(void)
{
    bt_spp_deactivate_req_struct *sppDeactiveReq;
    sppDeactiveReq = sp5kMalloc(sizeof(bt_spp_deactivate_req_struct));
    sppDeactiveReq->port = g_spp_port;
    mtk_test_send_msg(MSG_ID_BT_SPP_DEACTIVATE_REQ, sppDeactiveReq, sizeof(bt_spp_deactivate_req_struct));
    /* Send MSG_ID_BT_POWEROFF_REQ after receiving MSG_ID_BT_SPP_DEACTIVATE_CNF*/
}

/*****************************************************************************
 * FUNCTION
 *  mtk_test_search
 * DESCRIPTION
 *  To search surrounding Bluetooth devices
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void mtk_test_search(void)
{
    bt_bm_discovery_req_struct *req;
    req = sp5kMalloc(sizeof(bt_bm_discovery_req_struct));
    req->cod = 0xFFFFFFFF;
    req->access_mode = BTBM_GENERAL_INQUIRY;
    req->discovery_mode = BTBM_DISCOVERY_WITHOUT_NAME;
    req->inquiry_length = 30;
    req->inquiry_number = 20;
    mtk_test_send_msg(MSG_ID_BT_BM_DISCOVERY_REQ, req, sizeof(bt_bm_discovery_req_struct));
}

/*****************************************************************************
 * FUNCTION
 *  mtk_test_inquiry_scan
 * DESCRIPTION
 *  To enter inquiry scan mode
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void mtk_test_inquiry_scan(void)
{
    bt_bm_write_scanenable_mode_req_struct *modeReq;
    modeReq = sp5kMalloc(sizeof(bt_bm_write_scanenable_mode_req_struct));
    modeReq->mode = BTBM_ADP_P_OFF_I_ON;
    mtk_test_send_msg(MSG_ID_BT_BM_WRITE_SCANENABLE_MODE_REQ, modeReq, sizeof(bt_bm_write_scanenable_mode_req_struct));
    printf("mtk_test_inquiry_scan!\n\n");
}

/*****************************************************************************
 * FUNCTION
 *  mtk_test_page_scan
 * DESCRIPTION
 *  To enter page scan mode
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void mtk_test_page_scan(void)
{
    bt_bm_write_scanenable_mode_req_struct *modeReq;
    modeReq = sp5kMalloc(sizeof(bt_bm_write_scanenable_mode_req_struct));
    modeReq->mode = BTBM_ADP_P_ON_I_OFF;
    mtk_test_send_msg(MSG_ID_BT_BM_WRITE_SCANENABLE_MODE_REQ, modeReq, sizeof(bt_bm_write_scanenable_mode_req_struct));
    printf("mtk_test_page_scan!\n\n");
}

void mtk_test_no_scan(void)
{
    bt_bm_write_scanenable_mode_req_struct *modeReq;
    modeReq = sp5kMalloc(sizeof(bt_bm_write_scanenable_mode_req_struct));
    modeReq->mode = BTBM_ADP_P_OFF_I_OFF;
    mtk_test_send_msg(MSG_ID_BT_BM_WRITE_SCANENABLE_MODE_REQ, modeReq, sizeof(bt_bm_write_scanenable_mode_req_struct));
    printf("mtk_test_no_scan!\n\n");
}

/*****************************************************************************
 * FUNCTION
 *  mtk_test_sendfile
 * DESCRIPTION
 *  To send file via SPP profile
 * PARAMETERS
 *  path    [IN]    file path
 * RETURNS
 *  void
 *****************************************************************************/
BOOL mtk_test_sendfile(char *path)
{
    g_spp_send_fp = sp5kFsFileOpen(path, SP5K_FS_OPEN_RDONLY);
    if (g_spp_send_fp == 0)
    {
        printf("mtk_test_sendfile: file not exist");
        return FALSE;
    }
    else
    {
        U32 byte_read = 0;
        bt_spp_uart_put_bytes_req_struct *req = sp5kMalloc(sizeof(bt_spp_uart_put_bytes_req_struct));

#if 0
        g_spp_send_buf = sp5kMalloc(SPP_SEND_MAX_BUFF_SIZE);
        if (g_spp_send_buf == NULL)
        {
            printf("mtk_test_sendfile allocate buffer fail\n");
            return FALSE;
        }
#endif
        byte_read = sp5kFsFileRead(g_spp_send_fp, g_spp_send_buf, SPP_SEND_MAX_BUFF_SIZE);
        if (byte_read > 0x7FFFFFFF)
        {
            printf("[FS] mtk_test_sendfile read failed: err=0x%x", byte_read);
            /*sp5kFree(g_spp_send_buf);*/
            return FALSE;
        }
        else
        {
            printf("mtk_test_sendfile: %s, time:0x%x\n", path, sp5kOsTimeGet());
            g_spp_file_size = sp5kFsFileSizeGet(g_spp_send_fp);
            g_spp_send_size = byte_read;
            g_spp_send_ptr = &g_spp_send_buf[0];
            req->port = g_spp_port;
            req->buffer = g_spp_send_buf;
            ASSERT(byte_read <= 0x7FFF);
            req->length = byte_read;
            mtk_test_send_msg(MSG_ID_BT_SPP_UART_PUT_BYTES_REQ, req, sizeof(bt_spp_uart_put_bytes_req_struct));
            return TRUE;
        }
    }
}

/*****************************************************************************
 * FUNCTION
 *  mtk_test_sendfile_handler
 * DESCRIPTION
 *  SPP message handler during send file operation
 * PARAMETERS
 *  msg    [IN]    received MTK BT message
 * RETURNS
 *  void
 *****************************************************************************/
static BOOL mtk_test_sendfile_handler(mtkMsgStruct *msg)
{
    if (g_spp_send_fp == 0)
    {
        return FALSE;
    }

    switch (msg->ilm->msg_id)
    {
        case MSG_ID_BT_SPP_UART_PUT_BYTES_CNF:
        {
            bt_spp_uart_put_bytes_cnf_struct *cnf = (bt_spp_uart_put_bytes_cnf_struct *)msg->ilm->local_para_ptr;
            U32 sent_byte = cnf->length;
            g_spp_file_size -= sent_byte;
            g_spp_send_size -= sent_byte;
            if (g_spp_send_size > 0)
            {
                /* buffer data still remaining for sent, wait MSG_ID_BT_SPP_UART_READY_TO_WRITE_IND for next send of remaining data */
                g_spp_send_ptr += sent_byte;
            }
            else
            {
                if (g_spp_file_size > 0)
                {
                    bt_spp_uart_put_bytes_req_struct *req = sp5kMalloc(sizeof(bt_spp_uart_put_bytes_req_struct));
                    U32 byte_read = sp5kFsFileRead(g_spp_send_fp, g_spp_send_buf, SPP_SEND_MAX_BUFF_SIZE);
                    if (byte_read > 0x7FFFFFFF)
                    {
                        printf("[FS] mtk_test_sendfile next read failed: err=0x%x", byte_read);
                        return FALSE;
                    }
                    else
                    {
                        g_spp_send_size = byte_read;
                        g_spp_send_ptr = &g_spp_send_buf[0];
                        req->port = g_spp_port;
                        req->buffer = g_spp_send_buf;
                        ASSERT(byte_read <= 0x7FFF);
                        req->length = byte_read;
                        mtk_test_send_msg(MSG_ID_BT_SPP_UART_PUT_BYTES_REQ, req, sizeof(bt_spp_uart_put_bytes_req_struct));
                    }
                }
                else
                {
                    printf("mtk_test_sendfile done time:0x%x\n", sp5kOsTimeGet());
                    sp5kFsFileClose(g_spp_send_fp);
                    /*sp5kFree(g_spp_send_buf);
                    g_spp_send_buf = NULL;*/
                    g_spp_send_fp = 0;
                    g_spp_file_size = 0;
                }
            }
        }
        return TRUE;
        case MSG_ID_BT_SPP_UART_READY_TO_WRITE_IND:  /* Stack buffer is available, send our data */
        {
            bt_spp_uart_ready_to_write_ind_struct *ind = (bt_spp_uart_ready_to_write_ind_struct *)msg->ilm->local_para_ptr;
            if (g_spp_send_size > 0)
            {
                bt_spp_uart_put_bytes_req_struct *req;
                req = sp5kMalloc(sizeof(bt_spp_uart_put_bytes_req_struct));
                req->port = g_spp_port;
                req->buffer = g_spp_send_ptr;
                ASSERT(g_spp_send_size <= 0x7FFF);
                req->length = g_spp_send_size;
                mtk_test_send_msg(MSG_ID_BT_SPP_UART_PUT_BYTES_REQ, req, sizeof(bt_spp_uart_put_bytes_req_struct));
            }
        }
        return TRUE;
    }
    return FALSE;
}

/*****************************************************************************
 * FUNCTION
 *  mtk_test_sendbuf
 * DESCRIPTION
 *  To send buffer data via SPP
 * PARAMETERS
 *  buf    [IN]    Data buffer to be sent. Must not be released until all data is sent (receiving the last MSG_ID_BT_SPP_UART_PUT_BYTES_CNF)
 *  size   [IN]    Data size to be sent
 * RETURNS
 *  void
 *****************************************************************************/
void mtk_test_sendbuf(U8 *buf, U32 size)
{
    bt_spp_uart_put_bytes_req_struct *req;
	UINT32 retVal, retFlags;

	#if DBG_BTSTACK_PROFLOG_EN
	profLogAdd(0, "gSemBtSendBuf Get++"); /*PhilipDebug*/
	profLogAdd(1, "gSemBtSendBuf Get++"); /*PhilipDebug*/
	#endif

	extern UINT32 ros_semaphore_get(UINT32 semaphore_id, UINT32 timeout);
	retVal = ros_semaphore_get(gSemBtSendBuf, TX_WAIT_FOREVER);

	#if DBG_BTSTACK_PROFLOG_EN
	profLogAdd(0, "gSemBtSendBuf Get--"); /*PhilipDebug*/
	profLogAdd(1, "gSemBtSendBuf Get--"); /*PhilipDebug*/
	#endif

	if (SUCCESS != retVal) {
		profLogPrintf(0, "[%s %d] gSemBtSendBuf get failed. (err=0x%x) (flags=0x%x)", __FILE__, __LINE__, retVal, retFlags);
		ASSERT(0);
	}

	mBtSendDataLen = size;
	mBtSendStartTime = sp5kOsTimeGet() * 10;

	#if 0 /*original*/
	printf("mtk_test_sendbuf s %u (sz=%u)\n\n", mBtSendStartTime, size);

	#else /*PhilipTest@20120528*/
	profLogPrintf(1, "sendbuf REQ TS=%u (sz=%u)", mBtSendStartTime, mBtSendDataLen); /*PhilipDebug*/
	/*profLogCallStack(1);*//*PhilipDebug*/
	#endif

	#if 1 /*PhilipDebug*/
	if ( (size>>16) & 0xFFFF ) {
		ASSERT_MSG(0, "sendbuf size (%u) > (2^16) bytes\n", size);
	}
	#endif

    req = sp5kMalloc(sizeof(bt_spp_uart_put_bytes_req_struct));
    req->port = g_spp_port;
    if (buf == NULL)
    {
        /* Use test data */
        strcpy(g_spp_send_buf, "Hello MT6622 and V33!");
        req->buffer = g_spp_send_buf;
        req->length = 22;/*g_spp_send_size;*/
        g_spp_send_ptr = &g_spp_send_buf[0];
    }
    else
    {
        req->buffer = buf;
        ASSERT(size <= 0x7FFF);
        req->length = size;
        g_spp_send_size = size;
        g_spp_send_ptr = &buf[0];
    }
    mtk_test_send_msg(MSG_ID_BT_SPP_UART_PUT_BYTES_REQ, req, sizeof(bt_spp_uart_put_bytes_req_struct));
}

/*****************************************************************************
 * FUNCTION
 *  mtk_test_sendbuf_handler
 * DESCRIPTION
 *  SPP message handler during send buffer operation
 * PARAMETERS
 *  msg    [IN]    received MTK BT message
 * RETURNS
 *  void
 *****************************************************************************/
static BOOL mtk_test_sendbuf_handler(mtkMsgStruct *msg)
{
    switch (msg->ilm->msg_id)
    {
        case MSG_ID_BT_SPP_UART_PUT_BYTES_CNF:
        {
            bt_spp_uart_put_bytes_cnf_struct *cnf = (bt_spp_uart_put_bytes_cnf_struct *)msg->ilm->local_para_ptr;
            U32 sent_byte = cnf->length;
            g_spp_send_size -= sent_byte;

            if (g_spp_send_size > 0)
            {
				profLogPrintf(1, "sendbuf CNF (left=%u)", g_spp_send_size); /*PhilipDebug*/

                /* buffer data still remaining for sent, wait MSG_ID_BT_SPP_UART_READY_TO_WRITE_IND for next send of remaining data */
                g_spp_send_ptr += sent_byte;
            }
            else
            {
				mBtSendConfirmTime = sp5kOsTimeGet() * 10;

				#if 0 /*original*/
				printf("mtk_test_sendbuf e %u (diff=%u)\n\n", mBtSendConfirmTime, (mBtSendConfirmTime-mBtSendStartTime));
				/*printf("Send buffer complete!\n"); */

				#else /*PhilipTest@20120528*/

				if (mBtSendConfirmTime == mBtSendStartTime) {
					profLogPrintf(1, "sendbuf CNF TS1=%u. (data-rate=%u Bps).", mBtSendConfirmTime, (mBtSendDataLen*1000)/(1));
				}
				else {
					profLogPrintf(1, "sendbuf CNF TS2=%u. (data-rate=%u Bps).", mBtSendConfirmTime, (mBtSendDataLen*1000)/(mBtSendConfirmTime-mBtSendStartTime+1));
				}
				#endif

                /* MMI Todo: free buffer */
				UINT32 retVal;

				#if DBG_BTSTACK_PROFLOG_EN
				profLogAdd(0, "gSemBtSendBuf Set++"); /*PhilipDebug*/
				profLogAdd(1, "gSemBtSendBuf Set++"); /*PhilipDebug*/
				#endif

				extern UINT32 ros_semaphore_put(UINT32 semaphore_id);
				retVal = ros_semaphore_put(gSemBtSendBuf);

				#if DBG_BTSTACK_PROFLOG_EN
				profLogAdd(0, "gSemBtSendBuf Set--"); /*PhilipDebug*/
				profLogAdd(1, "gSemBtSendBuf Set--"); /*PhilipDebug*/
				#endif
				if (SUCCESS != retVal) {
					profLogPrintf(0, "[%s %d] gSemBtSendBuf set failed. (err=0x%x)", __FILE__, __LINE__, retVal);
					ASSERT(0);
				}
            }
        }
        return TRUE;
        case MSG_ID_BT_SPP_UART_READY_TO_WRITE_IND:  /* Stack buffer is available, send our data */
        {
            bt_spp_uart_ready_to_write_ind_struct *ind = (bt_spp_uart_ready_to_write_ind_struct *)msg->ilm->local_para_ptr;
            if (g_spp_send_size > 0)
            {
                bt_spp_uart_put_bytes_req_struct *req;
                req = sp5kMalloc(sizeof(bt_spp_uart_put_bytes_req_struct));
                req->port = g_spp_port;
                req->buffer = g_spp_send_ptr;
                ASSERT(g_spp_send_size <= 0x7FFF);
                req->length = g_spp_send_size;
                mtk_test_send_msg(MSG_ID_BT_SPP_UART_PUT_BYTES_REQ, req, sizeof(bt_spp_uart_put_bytes_req_struct));
            }
        }
        return TRUE;
    }
    return FALSE;
}

#if 0
void mtk_test_recdata(U32 size)
{
    bt_spp_uart_get_bytes_req_struct *req;
    req = sp5kMalloc(sizeof(bt_spp_uart_get_bytes_req_struct));
    req->port = g_spp_port;
    req->length = size;
    mtk_test_send_msg(MSG_ID_BT_SPP_UART_GET_BYTES_REQ, req, sizeof(bt_spp_uart_get_bytes_req_struct));
}
#endif

/*****************************************************************************
 * FUNCTION
 *  mtk_test_disconnect
 * DESCRIPTION
 *  To disconnect the current SPP connection
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void mtk_test_disconnect(void)
{
    bt_spp_disconnect_req_struct *req;
    req = sp5kMalloc(sizeof(bt_spp_disconnect_req_struct));
    req->port = g_spp_port;
    mtk_test_send_msg(MSG_ID_BT_SPP_DISCONNECT_REQ, req, sizeof(bt_spp_disconnect_req_struct));
}

/*****************************************************************************
 * FUNCTION
 *  mtk_test_sniff
 * DESCRIPTION
 *  To put an active link in sniff mode
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void mtk_test_sniff(void)
{
    bt_bm_set_link_state_req_struct *req;
    req = sp5kMalloc(sizeof(bt_bm_set_link_state_req_struct));
    req->sniff = TRUE;
    memcpy(&req->bd_addr, &g_spp_remote_addr, sizeof(bt_addr_struct));
    mtk_test_send_msg(MSG_ID_BT_BM_SET_LINK_STATE_REQ, req, sizeof(bt_bm_set_link_state_req_struct));
}

/*****************************************************************************
 * FUNCTION
 *  mtk_test_unsniff
 * DESCRIPTION
 *  To restore a sniff link to active mode
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void mtk_test_unsniff(void)
{
    bt_bm_set_link_state_req_struct *req;
    req = sp5kMalloc(sizeof(bt_bm_set_link_state_req_struct));
    req->sniff = FALSE;
    memcpy(&req->bd_addr, &g_spp_remote_addr, sizeof(bt_addr_struct));
    mtk_test_send_msg(MSG_ID_BT_BM_SET_LINK_STATE_REQ, req, sizeof(bt_bm_set_link_state_req_struct));
}

typedef enum
{
    BT_TX_POWER_TEST,
    BT_TX_ICFT_TEST,
    BT_TX_DEVM_TEST,
    BT_TX_TEST_TOTAL
} mtk_bt_tx_test_enum;

bt_engineer_mode_tx_test_req_struct bt_tx_tests[BT_TX_TEST_TOTAL] = {
    {.ref_count = 0, .msg_len = 0,
    .pattern = BT_EM_TX_PATTERN_RANDOM,
    .channel_hopping = BT_EM_TX_HOPPING_SINGLE,
    .tx_freq = 39,
    .rx_freq = 0,
    .poll_period = 1,
    .packet_type = BT_EM_PACKET_DH5,
    .packet_length = 0x012C},
    {.ref_count = 0, .msg_len = 0,
    .pattern = BT_EM_TX_PATTERN_1010,
    .channel_hopping = BT_EM_TX_HOPPING_SINGLE,
    .tx_freq = 39,
    .rx_freq = 0,
    .poll_period = 1,
    .packet_type = BT_EM_PACKET_DH5,
    .packet_length = 0x012C},
    {.ref_count = 0, .msg_len = 0,
    .pattern = BT_EM_TX_PATTERN_RANDOM,
    .channel_hopping = BT_EM_TX_HOPPING_SINGLE,
    .tx_freq = 0,
    .rx_freq = 0,
    .poll_period = 1,
    .packet_type = BT_EM_PACKET_3DH5,
    .packet_length = 0x03E8}};

/*****************************************************************************
 * FUNCTION
 *  mtk_btrf_tx_test
 * DESCRIPTION
 *  BT RF test: TX
 * PARAMETERS
 *  type    [IN]    TX test type
 * RETURNS
 *  void
 *****************************************************************************/
void mtk_btrf_tx_test(mtk_bt_tx_test_enum type)
{
    bt_engineer_mode_tx_test_req_struct *req;
    req = sp5kMalloc(sizeof(bt_engineer_mode_tx_test_req_struct));
    if (type < BT_TX_TEST_TOTAL)
    {
        memcpy(req, &bt_tx_tests[type], sizeof(bt_engineer_mode_tx_test_req_struct));
        mtk_test_send_msg(MSG_ID_BT_ENGINEER_MODE_TX_TEST_REQ, req, sizeof(bt_engineer_mode_tx_test_req_struct));
    }
}

typedef enum
{
    BT_RX_SENSITIVITY_TEST,
    BT_RX_TEST_TOTAL
} mtk_bt_rx_test_enum;

bt_engineer_mode_rx_test_req_struct bt_rx_tests[BT_RX_TEST_TOTAL] = {
    {.ref_count = 0, .msg_len = 0,
    .pattern = BT_EM_RX_PATTERN_RANDOM,
    .mode = BT_EM_RX_MODE,
    .rx_freq = 0,
    .packet_type = BT_EM_PACKET_3DH5,
    .bd_addr = {0, 0, 0, 0, 0, 0}}};

static mtk_bt_rx_test_enum g_bt_in_nsr_test = BT_RX_TEST_TOTAL;
extern U8 gorm_use_nsr_patch;
/*****************************************************************************
 * FUNCTION
 *  mtk_btrf_rx_test
 * DESCRIPTION
 *  BT RF test: RX
 * PARAMETERS
 *  type    [IN]    RX test type
 * RETURNS
 *  void
 *****************************************************************************/
void mtk_btrf_rx_test(mtk_bt_rx_test_enum type)
{
    g_bt_in_nsr_test = type;
    mtk_test_send_msg(MSG_ID_BT_POWEROFF_REQ, NULL, 0);
}

static BOOL mtk_btrf_rx_test_handler(mtkMsgStruct *rmsg)
{
    if (g_bt_in_nsr_test == BT_RX_TEST_TOTAL)
    {
        return FALSE;
    }

    switch(rmsg->ilm->msg_id)
    {
        case MSG_ID_BT_POWEROFF_CNF:
        {
            gorm_use_nsr_patch = 1;
            mtk_test_send_msg(MSG_ID_BT_POWERON_REQ, NULL, 0);
        }
        return TRUE;
        case MSG_ID_BT_POWERON_CNF:
        {
            bt_poweron_cnf_struct *cnf = (bt_poweron_cnf_struct *)rmsg->ilm->local_para_ptr;
            gorm_use_nsr_patch = 0;
            if (cnf->result == TRUE)
            {
                bt_engineer_mode_rx_test_req_struct *req = sp5kMalloc(sizeof(bt_engineer_mode_rx_test_req_struct));
                if (g_bt_in_nsr_test < BT_RX_TEST_TOTAL)
                {
                    memcpy(req, &bt_rx_tests[g_bt_in_nsr_test], sizeof(bt_engineer_mode_rx_test_req_struct));
                    mtk_test_send_msg(MSG_ID_BT_ENGINEER_MODE_RX_TEST_REQ, req, sizeof(bt_engineer_mode_rx_test_req_struct));
                }
            }
            else
            {
                printf("Error: NSR power on failed!");
            }
        }
        return TRUE;
    }
    return FALSE;
}

/*****************************************************************************
 * FUNCTION
 *  mtk_btrf_tx_test_end
 * DESCRIPTION
 *  To stop the preceding TX test
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void mtk_btrf_tx_test_end(void)
{
    mtk_test_send_msg(MSG_ID_BT_ENGINEER_MODE_TX_TEST_END_REQ, NULL, 0);
}

/*****************************************************************************
 * FUNCTION
 *  mtk_btrf_tx_test_end
 * DESCRIPTION
 *  To stop the preceding RX test and get RX test report
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void mtk_btrf_rx_test_end(void)
{
    mtk_test_send_msg(MSG_ID_BT_ENGINEER_MODE_RX_TEST_END_REQ, NULL, 0);
}

/*****************************************************************************
 * FUNCTION
 *  mtk_test_ssp_debug
 * DESCRIPTION
 *  Debug use. For sniffer log recording.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void mtk_test_ssp_debug(void)
{
    bt_ssp_debug_mode_req_struct *req = sp5kMalloc(sizeof(bt_ssp_debug_mode_req_struct));
    req->on = 1;
    mtk_test_send_msg(MSG_ID_BT_SSP_DEBUG_MODE_REQ, req, sizeof(bt_ssp_debug_mode_req_struct));
}

/*****************************************************************************
 * FUNCTION
 *  mtk_test_set_gpio
 * DESCRIPTION
 *  To control MT6622 GPIO state
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void mtk_test_set_gps(void)
{
    bt_gpio_set_req_struct *req = sp5kMalloc(sizeof(bt_gpio_set_req_struct));
    req->gpio = 6;
    req->default_level = 1;
    req->mode = 0;
    req->periodic_mode_enable = 0;
    req->high_period = 0xFF;  /* 4s */
    req->low_period = 0xFF;   /* 2s */
    mtk_test_send_msg(MSG_ID_BT_GPIO_SET_REQ, req, sizeof(bt_gpio_set_req_struct));
}

void mtk_test_set_gpio_always_high(UINT32 uInput, UINT32 uValue)
{
	printf("mtk_test_set_gpio_always_high %u, %u\n\n", uInput, uValue);
    bt_gpio_set_req_struct *req = sp5kMalloc(sizeof(bt_gpio_set_req_struct));
    /*req->gpio = 4; */
	req->gpio = uInput;
    req->default_level = uValue;
    req->mode = 0x0;
    req->periodic_mode_enable = 0x0;
    req->high_period = 0x0;  /* 4s */
    req->low_period = 0x0;   /* 2s */
    mtk_test_send_msg(MSG_ID_BT_GPIO_SET_REQ, req, sizeof(bt_gpio_set_req_struct));
}

void mtk_test_set_gpio(UINT32 uInput, UINT32 uValue)
{
	printf("sc dd %u, %u\n\n", uInput, uValue);
    bt_gpio_set_req_struct *req = sp5kMalloc(sizeof(bt_gpio_set_req_struct));
    /*req->gpio = 4; */
	req->gpio = uInput;
    req->default_level = uValue;
    req->mode = 1;
    req->periodic_mode_enable = 1;
    req->high_period = 0x8;  /* 4s */
    req->low_period = 0x4;   /* 2s */
    mtk_test_send_msg(MSG_ID_BT_GPIO_SET_REQ, req, sizeof(bt_gpio_set_req_struct));
}

void mtk_test_reset_led(UINT32 uInput)
{
	printf("mtk_test_reset_led %u\n\n", uInput);
    bt_gpio_set_req_struct *req = sp5kMalloc(sizeof(bt_gpio_set_req_struct));
    /*req->gpio = 4; */
	req->gpio = uInput;
	if(uInput == 1)
	{
    		req->default_level = 1;
	}
	else
	{
		req->default_level = 0;
	}
	printf("check aa %u\n",req->default_level);
    req->mode = 1;
    req->periodic_mode_enable = 0;
    req->high_period = 0x0;  /* 4s */
    req->low_period = 0x0;   /* 2s */
    mtk_test_send_msg(MSG_ID_BT_GPIO_SET_REQ, req, sizeof(bt_gpio_set_req_struct));
}

/*****************************************************************************
 * FUNCTION
 *  mtk_test_log
 * DESCRIPTION
 *  To enable BT logging
 * PARAMETERS
 *  level    [IN]    0: all off, 1: all on, 2: only primitive
 * RETURNS
 *  void
 *****************************************************************************/
void mtk_test_log(U8 level)
{
    bt_engineer_mode_log_mask_req_struct *req = sp5kMalloc(sizeof(bt_engineer_mode_log_mask_req_struct));
    ASSERT(req);
    switch(level)
    {
    case 0: /*disable debug message*/
        req->prompt = 1;
        req->primitive = 1;
        break;
    case 1: /*debug message to printf*/
        req->prompt = 0;
        req->primitive = 0;
        break;
    case 2:
        req->prompt = 1;
        req->primitive = 0;
        break;
    case 3: /*debug message to profLog*/
        req->prompt = 2;
        req->primitive = 2;
        break;
    default:
        return;
    }

    printf("%s::%d. req=0x%x (size=%d)\n", __func__, __LINE__, req, sizeof(bt_engineer_mode_log_mask_req_struct));

    mtk_test_send_msg(MSG_ID_BT_ENGINEER_MODE_LOG_MASK_REQ, req, sizeof(bt_engineer_mode_log_mask_req_struct));
}


/*****************************************************************************
 * FUNCTION
 *  mtk_test_handler
 * DESCRIPTION
 *  To handle MTK messages from BT profile and stack
 * PARAMETERS
 *  parm    [IN]    Thread init parameter
 * RETURNS
 *  void
 *****************************************************************************/
void mtk_test_handler(UINT32 parm)
{
    extern TX_QUEUE *g_bt_queue_ptr;
    mtkMsgStruct rmsg;

    g_mtktest_queue_ptr = sp5kMalloc(sizeof(TX_QUEUE));
    if (sp5kOsQueueCreate(g_mtktest_queue_ptr, "MTKTEST_Q", sizeof(mtkMsgStruct)/sizeof(UINT32), NULL, 20 * sizeof(mtkMsgStruct)) != 0)
    {
        Assert(0);
    }
    printf("[MTK TEST] Create queue end\n");

    while(1)
    {
        sp5kOsQueueReceive(g_mtktest_queue_ptr, &rmsg, TX_WAIT_FOREVER);
        if (rmsg.ilm)
        {

            #if 1 /* Philip@20120424: add for debug */
            profLogPrintf(0, "%s()::%d, rmsg.ilm->msg_id=0x%x", __func__, __LINE__, rmsg.ilm->msg_id);
            #endif

            switch(rmsg.ilm->msg_id)
            {
            case MSG_ID_BT_POWERON_CNF:
            {
                if (mtk_btrf_rx_test_handler(&rmsg))
                {
                    /* NSR test power on procedure */
                }
                else
                {
                    /* Normal power on procedure */
                    bt_poweron_cnf_struct *cnf = (bt_poweron_cnf_struct *)rmsg.ilm->local_para_ptr;
                    if (cnf->result == TRUE)
                    {
                        bt_bm_write_local_name_req_struct *nameReq;
                        bt_bm_write_scanenable_mode_req_struct *modeReq;
                        bt_spp_activate_req_struct *sppActReq;

                        nameReq = sp5kMalloc(sizeof(bt_bm_write_local_name_req_struct));
                        #if defined(HW_KX1371)

						#if 1 /*original*/
                        strcpy(nameReq->name, "CASIO CAMERA");        /* Modify to your configuration */
                        nameReq->name_len = strlen("CASIO CAMERA");   /* Modify to your configuration */
                        nameReq->name[strlen("CASIO CAMERA")] = '\0'; /* Modify to your configuration */

						#else /*PhilipTest*/

                        strcpy(nameReq->name, "CASIO CAMERA-PhilipLin");        /* Modify to your configuration */
                        nameReq->name_len = strlen("CASIO CAMERA-PhilipLin");   /* Modify to your configuration */
                        nameReq->name[strlen("CASIO CAMERA-PhilipLin")] = '\0'; /* Modify to your configuration */
						#endif

						#else
                        strcpy(nameReq->name, "MT6622 BT CAMERA");        /* Modify to your configuration */
                        nameReq->name_len = strlen("MT6622 BT CAMERA");   /* Modify to your configuration */
                        nameReq->name[strlen("MT6622 BT CAMERA")] = '\0'; /* Modify to your configuration */
                        #endif
                        mtk_test_send_msg(MSG_ID_BT_BM_WRITE_LOCAL_NAME_REQ, nameReq, sizeof(bt_bm_write_local_name_req_struct));

                        modeReq = sp5kMalloc(sizeof(bt_bm_write_scanenable_mode_req_struct));
                        modeReq->mode = BTBM_ADP_P_ON_I_ON;
                        mtk_test_send_msg(MSG_ID_BT_BM_WRITE_SCANENABLE_MODE_REQ, modeReq, sizeof(bt_bm_write_scanenable_mode_req_struct));

                        sppActReq = sp5kMalloc(sizeof(bt_spp_activate_req_struct));
                        strcpy(sppActReq->svcName, "Video_Over_SPP");        /* Modify to your configuration */
                        sppActReq->svcName[strlen("Video_Over_SPP")] = '\0'; /* Modify to your configuration */
                        sppActReq->svcUUID = 0x1101; /* 0x1101: SC_SERIAL_PORT; */
                        mtk_test_send_msg(MSG_ID_BT_SPP_ACTIVATE_REQ, sppActReq, sizeof(bt_spp_activate_req_struct));
                        printf("BT power on successful\n");
                    }
                    else
                    {
                        printf("BT power on failed\n");
                    }
                }
            }
            break;
            case MSG_ID_BT_SPP_ACTIVATE_CNF:              /* SPP initialization result */
            {
                bt_spp_activate_cnf_struct *cnf = (bt_spp_activate_cnf_struct *)rmsg.ilm->local_para_ptr;
                if (cnf->result)
                {
                    bt_spp_uart_assign_buffer_req_struct *req = sp5kMalloc(sizeof(bt_spp_uart_assign_buffer_req_struct));
                    g_spp_port = cnf->port;
                    req->port = g_spp_port;
                    req->getBufPtr = g_spp_rec_buf;
                    req->getBufSize = SPP_RECV_MAX_BUFF_SIZE;
                    mtk_test_send_msg(MSG_ID_BT_UART_ASSIGN_BUFFER_REQ, req, sizeof(bt_spp_uart_assign_buffer_req_struct));
                }
            }
            break;
            case MSG_ID_BT_UART_ASSIGN_BUFFER_CNF:
            {
                /* bt_spp_uart_assign_buffer_cnf_struct *cnf = (bt_spp_uart_assign_buffer_cnf_struct *)rmsg.ilm->local_para_ptr;*/
            }
            break;
            case MSG_ID_BT_SPP_DEACTIVATE_CNF:
            {
                mtk_test_send_msg(MSG_ID_BT_POWEROFF_REQ, NULL, 0);
            }
            break;
            case MSG_ID_BT_POWEROFF_CNF:
            {
                if (mtk_btrf_rx_test_handler(&rmsg))
                {
                    /* NSR test power off procedure */
                }
                else
                {
                    /* Normal power off procedure */
                }
            }
            break;
            case MSG_ID_BT_BM_PIN_CODE_IND:  /* Request PIN code for BT2.0 pair */
            {
                bt_bm_pin_code_ind_struct *ind = (bt_bm_pin_code_ind_struct *)rmsg.ilm->local_para_ptr;
                bt_bm_pin_code_rsp_struct *rsp = sp5kMalloc(sizeof(bt_bm_pin_code_rsp_struct));
                memcpy(&rsp->bd_addr, &ind->bd_addr, sizeof(btbm_bd_addr_t));

                /* only allow to pair with limited devices */
/*                if (memcmp(ind->name, "REMOCONN_NAME", 13) ==0 )*/ /* Modify to your configuration */
                {
                    rsp->pin_len = 4;                  /* Modify to your configuration */
                    memcpy(rsp->pin_code, "0000", 4);  /* Modify to your configuration */
                }
#if 0
                else
                {
                    rsp->pin_len = 0;
                }
#endif
                mtk_test_send_msg(MSG_ID_BT_BM_PIN_CODE_RSP, rsp, sizeof(bt_bm_pin_code_rsp_struct));
            }
            break;
            case MSG_ID_BT_BM_SECURITY_USER_CONFIRM_IND:  /* Request agreement for BT2.1 pair */
            {
                bt_bm_security_user_confirm_ind_struct *ind = (bt_bm_security_user_confirm_ind_struct *)rmsg.ilm->local_para_ptr;
                bt_bm_security_user_confirm_rsp_struct *rsp = sp5kMalloc(sizeof(bt_bm_security_user_confirm_rsp_struct));
                memcpy(&rsp->bd_addr, &ind->bd_addr, sizeof(btbm_bd_addr_t));
                /* only allow to pair with limited devices */
                printf("display_numeric: %d, name=%s",  ind->display_numeric, ind->name);
                if (ind->display_numeric == FALSE /*&& strcmp(ind->name, "REMOCONN_NAME") == 0*/)  /* Modify to your configuration */
                {
                    rsp->accept = TRUE;
                }
                else
                {
					#if 0
                    rsp->accept = FALSE; /*Original*/
					#else
					rsp->accept = TRUE; /*Autumn@0504*/
					#endif
                }
                mtk_test_send_msg(MSG_ID_BT_BM_SECURITY_USER_CONFIRM_RSP, rsp, sizeof(bt_bm_security_user_confirm_rsp_struct));
            }
            break;
            case MSG_ID_BT_BM_BONDING_RESULT_IND:  /* pair result indication */
            {
                bt_bm_bonding_result_ind_struct *ind = (bt_bm_bonding_result_ind_struct *)rmsg.ilm->local_para_ptr;

				printf("\n\n\n\n\n");
				printf("MSG_ID_BT_BM_BONDING_RESULT_IND check result %u, %u\n\n",ind->result, BTBM_ADP_SUCCESS);
                if (ind->result == BTBM_ADP_SUCCESS)
                {
                    /* pairing succeed! */
					printf("David check ok\n\n");
                }
            }
            break;
            case MSG_ID_BT_SPP_CONNECT_IND_REQ:  /* Remote request SPP connection */
            {
                bt_spp_connect_ind_req_struct *ind = (bt_spp_connect_ind_req_struct *)rmsg.ilm->local_para_ptr;
                bt_spp_connect_ind_rsp_struct *rsp = sp5kMalloc(sizeof(bt_spp_connect_ind_rsp_struct));
                rsp->port = ind->port;
                rsp->lap = ind->lap;
                rsp->uap = ind->uap;
                rsp->nap = ind->nap;
                rsp->result = TRUE;  /* Modify to your configuration */
                mtk_test_send_msg(MSG_ID_BT_SPP_CONNECT_IND_RSP, rsp, sizeof(bt_spp_connect_ind_rsp_struct));
            }
            break;
            case MSG_ID_BT_SPP_CONNECT_IND:  /* SPP connection established */
            {
                bt_spp_connect_ind_struct *ind = (bt_spp_connect_ind_struct *)rmsg.ilm->local_para_ptr;
                g_spp_remote_addr.lap = ind->lap;
                g_spp_remote_addr.uap = ind->uap;
                g_spp_remote_addr.nap = ind->nap;
                printf("MSG_ID_BT_SPP_CONNECT_IND\n");
            }
            break;
            case MSG_ID_BT_SPP_UART_PUT_BYTES_CNF:  /* Some/All data is sent, check if all data is sent*/
            {
                bt_spp_uart_put_bytes_cnf_struct *cnf = (bt_spp_uart_put_bytes_cnf_struct *)rmsg.ilm->local_para_ptr;
                if (cnf->port == g_spp_port)
                {
                    if (mtk_test_sendfile_handler(&rmsg))
                    {
                    }
                    else if (mtk_test_sendbuf_handler(&rmsg))
                    {
                    }
                }
            }
            break;
            case MSG_ID_BT_SPP_UART_READY_TO_WRITE_IND:  /* Stack buffer is available, send our data */
            {
                bt_spp_uart_ready_to_write_ind_struct *ind = (bt_spp_uart_ready_to_write_ind_struct *)rmsg.ilm->local_para_ptr;
                if (ind->port == g_spp_port)
                {
                    if (mtk_test_sendfile_handler(&rmsg))
                    {
                    }
                    else if (mtk_test_sendbuf_handler(&rmsg))
                    {
                    }
                }
            }
            break;
            case MSG_ID_BT_SPP_UART_DATA_AVAILABLE_IND:  /* Receive data from remote, read it */
            {
                bt_spp_uart_data_available_ind_struct *ind = (bt_spp_uart_data_available_ind_struct *)rmsg.ilm->local_para_ptr;
                bt_spp_uart_data_available_ind_rsp_struct *rsp = sp5kMalloc(sizeof(bt_spp_uart_data_available_ind_rsp_struct));;
                if (ind->port == g_spp_port && ind->length > 0)
                {
                    rsp->length = ind->length;
                    rsp->port = g_spp_port;
                    DisplayHex("SPP recv: ", ind->buffer, ind->length);
			/*printf("David print here gogo\n\n");*/
			/*VQBT_process_remocon_msg(ind->buffer, ind->length);*/
			/*mtk_test_sendbuf(ind->buffer, ind->length);		*/
                    mtk_test_send_msg(MSG_ID_BT_SPP_UART_DATA_AVAILABLE_IND_RSP, rsp, sizeof(bt_spp_uart_data_available_ind_rsp_struct));
                }
            }
            break;
            case MSG_ID_BT_SPP_UART_PLUGOUT_IND:
            {
                bt_spp_uart_plugout_cnf_struct *cnf = sp5kMalloc(sizeof(bt_spp_uart_plugout_cnf_struct));
                cnf->port = g_spp_port;
                mtk_test_send_msg(MSG_ID_BT_SPP_UART_PLUGOUT_CNF, cnf, sizeof(bt_spp_uart_plugout_cnf_struct));
            }
            case MSG_ID_BT_SPP_DISCONNECT_IND:  /* SPP connection removed */
            {
                bt_spp_disconnect_ind_struct *ind = (bt_spp_disconnect_ind_struct *)rmsg.ilm->local_para_ptr;
                g_spp_remote_addr.lap = 0;
                g_spp_remote_addr.uap = 0;
                g_spp_remote_addr.nap = 0;
                printf("MSG_ID_BT_SPP_DISCONNECT_IND\n");
            }
            break;
            case MSG_ID_BT_BM_SET_LINK_STATE_CNF:  /* sniff/unsniff result */
            {
                bt_bm_set_link_state_cnf_struct *cnf = (bt_bm_set_link_state_cnf_struct *)rmsg.ilm->local_para_ptr;

                printf("MSG_ID_BT_BM_SET_LINK_STATE_CNF\n");
                if (cnf->result == BTBM_ADP_SUCCESS)
                {
                    if (cnf->is_sniff)
                    {
                        printf("Sniff succeeded\n");
                    }
                    else
                    {
                        printf("Unsniff succeeded\n");
                    }
                }
                else
                {
                    printf("Sniff/Unsniff failed\n");
                }
            }
            break;
            case MSG_ID_BT_BM_WRITE_LOCAL_NAME_CNF:       /* Write local BT device name to chip result */
            case MSG_ID_BT_BM_WRITE_SCANENABLE_MODE_CNF:  /* Write local scanable mode to chip result */
            case MSG_ID_BT_ENGINEER_MODE_TX_TEST_END_CNF:
            case MSG_ID_BT_ENGINEER_MODE_TX_TEST_CNF:
            case MSG_ID_BT_ENGINEER_MODE_RX_TEST_CNF:
            {
            }
            break;
            case MSG_ID_BT_ENGINEER_MODE_RX_TEST_END_CNF:
            {
                bt_engineer_mode_rx_test_end_cnf_struct *cnf = (bt_engineer_mode_rx_test_end_cnf_struct *)rmsg.ilm->local_para_ptr;
                printf("RX test: packet#=%d, PER=%d%%, byte#=%d, BER=%d%%\n", cnf->received_packets, cnf->packet_error_rate, cnf->received_bytes, cnf->bit_error_rate);
                g_bt_in_nsr_test = BT_RX_TEST_TOTAL;
            }
            break;
            default:
                break;
            }
            free_ilm(rmsg.ilm);
            rmsg.ilm = 0;
        }
    }
}

/* ------------------------- iCatch driver ------------------------- */
U8 BtRadio_Get_Deafult_Flowcontrol_Setting(void)
{

    #if 0
    profLogPrintf(0, "%s() return 0", __func__); /*PhilipDebug*/
	/*printf("%s() return 0\n", __func__); /*PhilipDebug*/
    /* V33 UART-1 (default) no flow control */
    return 0;
    #else
    profLogPrintf(0, "%s() return 1", __func__); /*PhilipDebug*/
	/*printf("%s() return 1\n", __func__); /*PhilipDebug*/
    /* V33 UART-1 (default) enable flow control */
    return 1;
    #endif
}

#if 1 /*++Autumn@0521 new lib need this API*/
UINT8
V33_BT_DisplayPairScreen(
	void
)
{
    /*1: display pair screen -> to avoid Remocon issue
      3: do not display pair screen */
    return 1;
}
#endif /*--Autumn@0521 new lib need this API*/

#if 1 /*++Philip@20120525*/
static UINT32 mV33UartBaudrate; /*=0*/
static UINT32 mV33PwmLowDuty; /*=0*/
static UINT32 mV33PwmHighDuty; /*=0*/

void
V33_UART_FlowControl_RxFreeRun(
	void
)
{
	profLogPrintf(0, "%s()", __func__);
	sp5kPwmCfgSetExt(1, 100/*lowTicks*/, 0/*highTicks*/);
}

void
V33_UART_FlowControl_RxPause(
	void
)
{
	profLogPrintf(0, "%s()", __func__);
	sp5kPwmCfgSetExt(1, 0/*lowTicks*/, 100/*highTicks*/);
}

void
V33_UART_FlowControl_RxResume(
	void
)
{
	profLogPrintf(0, "%s() (L:H)=(%u,%u)", __func__, mV33PwmLowDuty, mV33PwmHighDuty);
	sp5kPwmCfgSetExt(1, mV33PwmLowDuty/*281*//*lowTicks*/, mV33PwmHighDuty/*2047*//*highTicks*/);
}
#endif /*--Philip@20120525*/

void V33_UART_SetFlowControl(U8 enable)
{
	UINT32 sr, uartBaudrate;
	uartBaudrate = mV33UartBaudrate;
    profLogPrintf(0, "%s(%d)", __func__, enable); /*PhilipDebug*/
	/*printf("%s(%d)\n", __func__, enable); /*PhilipDebug*/

	/**
	 Due to V33 UART Rx FIFO too small
	 V33 MAY ask MT6622 stop via set V33's UART CTS# as HIGH.
	 When V33 UART Rx available, V33 set V33's UART CTS# as LOW.
	*/
    if (enable) {
		/**
		 For 115200 bps, 1 byte takes about 95.48 usec.
		 For 1.0Mbps, 1 byte takes about 11~9.54 usec.
		 For 1.5Mbps, 1 byte takes about 7.33~6.34 usec.
		 For 2.0Mbps, 1 byte takes about 5.50~4.77 usec.
		 For 2.5Mbps, 1 byte takes about 4.40~3.84 usec.
		 */
		/**
		 For K33/V33, 1 pwmTick is about 20.83 ns (=1/48MHz).
		 With sp5kPwmCfgSetExt, you can control PWM frequence (Hz) ONLY by (lowTicks+highTicks).
		 Min=0.
		 Max=4095 for chnlId==0, Max=2047 for the others.
		 (Output 2047 PWM ticks as HIGH takes 42.645 usec)

		 MT6622 suggest CTS# set LOW for 1.3 bytes for safety.
		 For 1.0Mbps, 1.3 byte takes about 633~549 pwmTick.
		 For 1.5Mbps, 1.3 byte takes about 422~365 pwmTick.
		 For 2.0Mbps, 1.3 byte takes about 316~274 pwmTick.
		 For 2.5Mbps, 1.3 byte takes about 253~219 pwmTick.
		 */
		/**
		 Philip@20120709,
		 Refine CTS# LOW period as 10 bits to prevent UART Rx FIFO Overflow.
		 For 1.0Mbps, 1 byte takes about 480~457 pwmTick.
		 For 1.5Mbps, 1 byte takes about 320~305 pwmTick.
		 For 2.0Mbps, 1 byte takes about 240~228 pwmTick.
		 For 2.5Mbps, 1 byte takes about 192~183 pwmTick.
		*/
    	if (uartBaudrate < 1000000) {
			/**
			 For 115200 bps, 1 byte takes about 95.48 us
			 Force CTS# always LOW (no flow control)
			*/
			mV33PwmLowDuty = 100; /* full clock cycle = 133 us */
			mV33PwmHighDuty = 0; /* ALWAYS LOW (allow MT6622 Tx to V33) */
		}
		else if (uartBaudrate == 1152000) {
			/**
			 1 PWM cycle about 51.32 usec.
			 For MT6622 Tx 8 bytes, V33 CPU has about 410 usec to handle other ISR.
			*/
			mV33PwmLowDuty = 416;
			mV33PwmHighDuty = 2047;
		}
		else if ((uartBaudrate >= 1000000)&&(uartBaudrate <= 1048576 /*(1024*1024)*/)) {
			/**
			 1 PWM cycle about 52.64 usec.
			 For MT6622 Tx 8 bytes, V33 CPU has about 421 usec to handle other ISR.
			*/
			mV33PwmLowDuty = 480;
			mV33PwmHighDuty = 2047;
		}
		else if (uartBaudrate == 1728000) {
			/**
			 1 PWM cycle about 48.43 usec.
			 For MT6622 Tx 8 bytes, V33 CPU has about 387 usec to handle other ISR.
			*/
			mV33PwmLowDuty = 277;
			mV33PwmHighDuty = 2047;
		}
		else if ((uartBaudrate >= 1500000)&&(uartBaudrate <= 1572864)) {
			/**
			 Full cycle about 49.31 usec.
			 For MT6622 Tx 8 bytes, V33 CPU has about 394 usec to handle other ISR.
			*/
			mV33PwmLowDuty = 320;
			mV33PwmHighDuty = 2047;
		}
		else if (uartBaudrate==2096000) {
			/**
			 Full cycle about 47.41 usec.
			 For MT6622 Tx 8 bytes, V33 CPU has about 379 usec to handle other ISR.
			*/
			mV33PwmLowDuty = 229;
			mV33PwmHighDuty = 2047;
		}
		else if (uartBaudrate==2304000) {
			/**
			 Full cycle about 46.98 usec.
			 For MT6622 Tx 8 bytes, V33 CPU has about 375 usec to handle other ISR.
			*/
			mV33PwmLowDuty = 208;
			mV33PwmHighDuty = 2047;
		}
		else if ((uartBaudrate >= 2000000)&&(uartBaudrate <= 2097152)) {
			/**
			 Full cycle about 47.64 usec.
			 For MT6622 Tx 8 bytes, V33 CPU has about 381 usec to handle other ISR.
			*/
			mV33PwmLowDuty = 316;
			mV33PwmHighDuty = 2047;
		}
		else if ((uartBaudrate==2621440)||(uartBaudrate==2621400)) {
			/**
			 Full cycle about 46.46 usec.
			 For MT6622 Tx 8 bytes, V33 CPU has about 371 usec to handle other ISR.
			*/
			mV33PwmLowDuty = 183;
			mV33PwmHighDuty = 2047;
		}
		else if (uartBaudrate==3200000) {
			/**
			 Full cycle about 45.77 usec.
			 For MT6622 Tx 8 bytes, V33 CPU has about 366 usec to handle other ISR.
			*/
			mV33PwmLowDuty = 150;
			mV33PwmHighDuty = 2047;
		}
		else {
			/*NG uartBaudrate=1500000, 2500000, 2880000, 3000000*/
			ASSERT_MSG(0, "Not Support (%u)!! Please fine-tune PWM high/low period at the first\n", uartBaudrate);
		}

		sp5kPwmCfgSetExt(1, mV33PwmLowDuty/*lowTicks*/, mV33PwmHighDuty/*highTicks*/);
        sp5kPwmTrig( 1 << 1, 0 ); /* Set V33's GPIO-1 output enable, and start PWM */
    }
    else {
        sp5kPwmTrig( 0, 1 << 1 ); /* Set V33's GPIO-1 output disable, and stop PWM */
    }
}

UINT32
V33_UART_Init(
	UINT32 baud
)
{
	sp5kUartInit(1, baud);

	if (baud!=115200) {
		sp5kUartModeSet(1, SP5K_UART_MODE_PIO);
	}
	return SUCCESS;
}

void
V33_UART_SetSpeed(
	UINT32 speed
)
{
    profLogPrintf(0, "%s(%d)", __func__, speed); /*PhilipDebug*/
	/*printf("%s(%d)\n", __func__, speed); /*PhilipDebug*/

	mV33UartBaudrate = speed;

	V33_UART_Init(speed);

	#if 1 /*Philip@20120604*/
	if (speed > 921600) { /* 115200*8 */
		V33_UART_SetFlowControl(1);
	}
	#endif
}

UINT32
V33_UART_Read(
	UINT32 *ch
)
{
	return sp5kUartRead(1, ch);
}

UINT32
V33_UART_Write(
	UINT8 *buf,
	UINT32 len
)
{
	V33_UART_FlowControl_RxResume();
	return sp5kUartWrite(1, buf, len);
}

void V33_PowerOnBT_low(void)
{
    profLogPrintf(0, "%s()", __func__); /*PhilipDebug*/
	/*printf("%s()\n", __func__); /*PhilipDebug*/
	#if defined(HW_KX1371)
	/* LMI-4 (connect to RESET): output, HIGH (follow with inverter to LOW) */
	sp5kGpioWrite(SP5K_GPIO_GRP_LMI, (1<<4), (1<<4) );
	sp5kGpioCfgSet(SP5K_GPIO_GRP_LMI, (1<<4), (1<<4) );
	profLogAdd(0, "V33 LMI-4 output HIGH (follow with inverter to LOW)");
	/*printf("V33 LMI-4 output HIGH (follow with inverter to LOW)\n");/*PhilipDebug*/

	#else /* EVB */

	#if 0 /*original interconnection*/
    /* LCDIF15 (connect to LDO28EN): output, high */
    /* LCDIF12 (connect to RESET): output, low */
    sp5kGpioWrite(SP5K_GPIO_GRP_DISP, (1<<15)|(1<<12), (1<<15)|(0<<12) );
    sp5kGpioCfgSet(SP5K_GPIO_GRP_DISP, (1<<15)|(1<<12), (1<<15)|(1<<12) );

    #else /*Philip@20120705: using LMI-3 instead of LCD-15, and LMI-4 instead of LCD-12 */
    /* LMI-3 (connect to LDO28EN): output, high */
    /* LMI-4 (connect to RESET): output, low */
    sp5kGpioWrite(SP5K_GPIO_GRP_LMI, (1<<3)|(1<<4), (1<<3)|(0<<4) );
    sp5kGpioCfgSet(SP5K_GPIO_GRP_LMI, (1<<3)|(1<<4), (1<<3)|(1<<4) );

    #endif /*Philip@20120705: using LMI-3 instead of LCD-15, and LMI-4 instead of LCD-12 */

	#endif
}

void V33_PowerOnBT_high(void)
{
    profLogPrintf(0, "%s()", __func__); /*PhilipDebug*/
	/*printf("%s()\n", __func__); /*PhilipDebug*/
	#if defined(HW_KX1371)
	/* LMI-4 (connect to RESET): output, LOW (follow with inverter to HIGH) */
	sp5kGpioWrite(SP5K_GPIO_GRP_LMI, (1<<4), (0<<4) );
	sp5kGpioCfgSet(SP5K_GPIO_GRP_LMI, (1<<4), (1<<4) );
	profLogAdd(0, "V33 LMI-4 output LOW (follow with inverter to HIGH)");
	/*printf("V33 LMI-4 output LOW (follow with inverter to HIGH)\n"); /*PhilipDebug*/

	#else /* EVB */

	#if 0 /*original interconnection*/
    /* LCDIF15 (connect to LDO28EN): output, high */
    /* LCDIF12 (connect to RESET): output, high */
    sp5kGpioWrite(SP5K_GPIO_GRP_DISP, (1<<15)|(1<<12), (1<<15)|(1<<12) );
    sp5kGpioCfgSet(SP5K_GPIO_GRP_DISP, (1<<15)|(1<<12), (1<<15)|(1<<12) );

    #else /*Philip@20120705: using LMI-3 instead of LCD-15, and LMI-4 instead of LCD-12 */
    /* LMI-3 (connect to LDO28EN): output, high */
    /* LMI-4 (connect to RESET): output, high */
    sp5kGpioWrite(SP5K_GPIO_GRP_LMI, (1<<3)|(1<<4), (1<<3)|(1<<4) );
    sp5kGpioCfgSet(SP5K_GPIO_GRP_LMI, (1<<3)|(1<<4), (1<<3)|(1<<4) );

    #endif /*Philip@20120705: using LMI-3 instead of LCD-15, and LMI-4 instead of LCD-12 */

	#endif
}

void V33_PowerOffBT(void)
{
    profLogPrintf(0, "%s()", __func__); /*PhilipDebug*/
	/*printf("%s()\n", __func__); /*PhilipDebug*/

	#if defined(HW_KX1371)
	/* LMI-4 (connect to RESET): output, HIGH (follow with inverter to LOW) */
	sp5kGpioWrite(SP5K_GPIO_GRP_LMI, (1<<4), (1<<4) );
	sp5kGpioCfgSet(SP5K_GPIO_GRP_LMI, (1<<4), (1<<4) );
	profLogAdd(0, "V33 LMI-4 output HIGH (follow with inverter to LOW)");
	/*printf("V33 LMI-4 output HIGH (follow with inverter to LOW)\n"); /*PhilipDebug*/

	#else /* EVB */

	#if 0 /*original interconnection*/
    /* LCDIF15 (connect to LDO28EN): output, low */
    /* LCDIF12 (connect to RESET): output, low */
    sp5kGpioWrite(SP5K_GPIO_GRP_DISP, (1<<15)|(1<<12), (0<<15)|(0<<12) );
    sp5kGpioCfgSet(SP5K_GPIO_GRP_DISP, (1<<15)|(1<<12), (1<<15)|(1<<12) );

    #else /*Philip@20120705: using LMI-3 instead of LCD-15, and LMI-4 instead of LCD-12 */
    /* LMI-3 (connect to LDO28EN): output, low */
    /* LMI-4 (connect to RESET): output, low */
    sp5kGpioWrite(SP5K_GPIO_GRP_LMI, (1<<3)|(1<<4), (0<<3)|(0<<4) );
    sp5kGpioCfgSet(SP5K_GPIO_GRP_LMI, (1<<3)|(1<<4), (1<<3)|(1<<4) );

    #endif /*Philip@20120705: using LMI-3 instead of LCD-15, and LMI-4 instead of LCD-12 */

	#endif
}


#if 1 /*PhilipHack*/
UINT8 mBtDataIni[23] = {
	0x22, 0x66, 0x00, 0x00, 0x35, 0x61, 0x40, 0x60,
	0x00, 0x23, 0x07, 0x80, 0x00, 0x06, 0x03, 0x07,
	0x03, 0x40, 0x1F, 0x40, 0x1F, 0x00, 0x04
};
#endif

BOOL V33_RetrieveBTData(U8 *buf, U32 size)
{
    UINT32 mFd;
    profLogPrintf(0, "%s(0x%x, %d)", __func__, buf, size); /*PhilipDebug*/
	/*printf("%s(0x%x, %d)\n", __func__, buf, size); /*PhilipDebug*/

	#if 1 /*PhilipHack*/
	memcpy(buf, mBtDataIni, size);

	#if 0
	printf("%s Ln %d. Force unmount NAND++\n", __FILE__, __LINE__); /*PhilipDebug*/
	sp5kDiskUnMount(SP5K_DRIVE_NAND);
	printf("%s Ln %d. Force unmount NAND--\n", __FILE__, __LINE__); /*PhilipDebug*/
	#endif

	#else /*original*/

    mFd = sp5kFsFileOpen("A:\\BTDATA.bin", SP5K_FS_OPEN_RDWR);
    if (!mFd)
    {
        printf("File Open fail for A:\\BTDATA.bin\n");
        return FALSE;
    }
    else
    {
        sp5kFsFileRead(mFd, buf, size);
        sp5kFsFileClose(mFd);
        return TRUE;
    }
	#endif
}

void
V33_AssertBT(
	void
)
{
    ASSERT(0); /*PhilipDebug*/
}

#define V33_BT_ROOT_DIR "A:\\"
char *V33_RetrieveBTRootDir(void)
{
	profLogPrintf(0, "%s() returns \"%s\"", __func__, V33_BT_ROOT_DIR); /*PhilipDebug*/
	/*printf("%s() returns \"%s\"\n", __func__, V33_BT_ROOT_DIR); /*PhilipDebug*/
    return V33_BT_ROOT_DIR;
}

#define MIN(a,b) ((a > b) ? b : a)

#define BT_UART_BUFFER_SIZE 2048
U8 g_bt_uart_buf[BT_UART_BUFFER_SIZE];
U8 *g_bt_uart_read_ptr;
U8 *g_bt_uart_read_ptr_last;
U8 *g_bt_uart_write_ptr;
TX_MUTEX g_mutex_bt_uart;

UINT32 mLastSts; /*PhilipDebug*/
UINT32 mErrCnt; /*PhilipDebug*/
#define BT_UART_READ_NONBLOCKING (0) /*PhilipDebug*/

void mtk_uart_init(U32 parm)
{
    extern BOOL BTCoreVerifySysInitState(void);
    printf("[MTKBT] mtk_uart_init\n");

	V33_UART_Init(115200);

    if(sp5kOsMutexCreate(&g_mutex_bt_uart, "MUTEX_BT", TX_INHERIT) != 0)
    {
        #if 1 /* Philip@20120424: add for debug */
        profLogAdd(0, "Assert:  BT UART mutex create failed");
        printf("Assert: BT UART mutex create failed\n");
        ASSERT(0); /*PhilipDebug*/
        #else
        printf("Assert: BT UART mutex create failed\n");
        while(1);
        #endif
    }

	extern UINT32 ros_semaphore_create(char *name, UINT32 init_val);
	gSemBtSendBuf = ros_semaphore_create("SEM_BT_SEND_BUF", 1);
    if(gSemBtSendBuf == 0)
    {
		#if 1 /* Philip@20120608: add for debug */
        profLogAdd(0, "Assert: SEM_BT_SEND_BUF create failed");
        printf("Assert: SEM_BT_SEND_BUF create failed\n");
        ASSERT(0); /*PhilipDebug*/
		#endif
    }

    g_bt_uart_write_ptr = &g_bt_uart_buf[1];
    g_bt_uart_read_ptr = &g_bt_uart_buf[0];
    g_bt_uart_read_ptr_last = NULL;
    while(1)
    {
        U32 ch = 0xFFFFFFFF;
        BOOL ret = BTCoreVerifySysInitState();

        #if 0 /* Philip@20120424: add for debug */
        profLogPrintf(0, "mtkUart: 0x%x", ret);
        #endif

        if (ret)
        {
        	#if (!BT_UART_READ_NONBLOCKING)
			/**
			 PhilipDebug: it seems UART not send initial 4 byte "0x01 01 10 00" when the 1st time ret==1.
			 Add dummy delay for testing
			*/
        	if (mLastSts==0) {
				#if 1
				profLogAdd(0, "mLastSts==0, sleep extra times for wait init UART string sent");
				ros_thread_sleep(2000);
				mLastSts = 1;
				continue;
				#endif
			}
			#endif

            V33_UART_Read(&ch);

			#if 0
            printf("UART read: 0x%x", ch);
			#endif
            if (ch == 0xFFFFFFFF)
            {
                #if BT_UART_READ_NONBLOCKING
				#if 0
				profLogAdd(0, "UART read return 0xFFFFFFFF. sleep 1 ms.");
                printf("\nUART read return 0xFFFFFFFF. sleep 1 ms.\n");
				#endif
                ros_thread_sleep(1);
				#else
				profLogAdd(0, "UART read return 0xFFFFFFFF");

				#if 1 /*++PhilipDebug blockMode NG*/
				mErrCnt++;
				ros_thread_sleep(1);

				if (mErrCnt==100) {
					ASSERT_MSG(0, "Please replace libdrv.hsuart.a with UART Rx block-mode instead of nonblock-mode.\n");
				}
				#endif /*--PhilipDebug blockMode NG*/

                #endif
                continue;
            }

            sp5kOsMutexGet(&g_mutex_bt_uart, TX_WAIT_FOREVER);
            if (g_bt_uart_write_ptr == g_bt_uart_read_ptr)
            {
                #if 1 /* Philip@20120424: add for debug */
                profLogAdd(0, "Assert:  BT UART overflow");
                printf("Assert: BT UART overflow\n");
                ASSERT(0); /*PhilipDebug*/
                #else
                while(1) {
                    printf("Assert: BT UART overflow\n");
                }
                #endif
            }
            if (g_bt_uart_read_ptr != g_bt_uart_read_ptr_last)
            {
                g_bt_uart_read_ptr_last = g_bt_uart_read_ptr;
                BT_SendMessage(MSG_ID_UART_READY_TO_READ_IND, MOD_BT, NULL, 0);
            }

            *g_bt_uart_write_ptr = (U8)ch;
            if (g_bt_uart_write_ptr == &g_bt_uart_buf[2047])
            {
                g_bt_uart_write_ptr = &g_bt_uart_buf[0];
            }
            else
            {
                g_bt_uart_write_ptr++;
            }
            sp5kOsMutexPut(&g_mutex_bt_uart);
        }
        else
        {
	        mLastSts = 0; /*PhilipDebug*/
            ros_thread_sleep(100);
        }
    }
}

U16 BTUartV33Read(U8 *buf, U16 bufLen)
{
    int len;

	profLogPrintf(0, "BTUartV33Read(0x%08x, %d)", buf, bufLen); /*PhilipDebug*/
	/*profLogCallStack(0); /*PhilipDebug*/

    sp5kOsMutexGet(&g_mutex_bt_uart, TX_WAIT_FOREVER);
    {
/*        printf("Uart read: 0x%x, w=0x%x, r=0x%x, bufLen=%d\n", g_bt_uart_buf, g_bt_uart_write_ptr, g_bt_uart_read_ptr, bufLen);*/
        if (g_bt_uart_write_ptr > g_bt_uart_read_ptr)
        {
            /*   |------|-----|------|
                 0      r     w      BT_UART_BUFFER_SIZE
             */
            len = g_bt_uart_write_ptr - g_bt_uart_read_ptr - 1;
            if (len)
            {
                len = MIN(len, bufLen);
                memcpy(buf, g_bt_uart_read_ptr + 1, len);
                g_bt_uart_read_ptr += len;
            }
        }
        else
        {
            /*   |------|-----|------|
                 0      w     r      BT_UART_BUFFER_SIZE
             */
            len = g_bt_uart_write_ptr + BT_UART_BUFFER_SIZE - g_bt_uart_read_ptr - 1;
            if (len)
            {
                int remain = &g_bt_uart_buf[0] + BT_UART_BUFFER_SIZE - g_bt_uart_read_ptr - 1;
                len = MIN(len, bufLen);
                if (remain > len)
                {
                    memcpy(buf, g_bt_uart_read_ptr + 1, len);
                    g_bt_uart_read_ptr += len;
                }
                else
                {
                    /* wrap around */
                    memcpy(buf, g_bt_uart_read_ptr + 1, remain);
                    memcpy(buf + remain, &g_bt_uart_buf[0], len - remain);
                    g_bt_uart_read_ptr += (len - BT_UART_BUFFER_SIZE);
                }
            }
        }
    }
    sp5kOsMutexPut(&g_mutex_bt_uart);
    return len;
}

void
mtk_eint_IsrCallback(
	void
)
{
	extern void BtRadio_EintIsr(void);
	printf("%s()++\n", __func__);/*PhilipDebug*/
	profLogPrintf(0, "%s()++\n", __func__);
	V33_UART_FlowControl_RxResume(); /*PhilipAdd@20120615*/
	BtRadio_EintIsr();
	profLogPrintf(0, "%s()--\n", __func__);
	printf("%s()--\n", __func__);/*PhilipDebug*/
}

#if defined(HW_KX1371)
void
mtk_eint_task(
	UINT32 parm
)
{
	UINT32 val;
	const UINT32 mtkEintPin = (1<<6);

	printf("[MTK] mtk_eint_task\n");

	/* Set LCDIF6 as input mode */
	sp5kGpioCfgSet(SP5K_GPIO_GRP_DISPL, mtkEintPin, (0<<6) );
	while(1) {
		sp5kGpioRead(SP5K_GPIO_GRP_DISPL, mtkEintPin, &val);
		if (val & mtkEintPin ) {
			/* If LCDIF6 is 1 (level HIGH), call IsrCallback */
			mtk_eint_IsrCallback();
			val &= ~mtkEintPin;
		}
		else {
			/* sleep, and next polling */
			#if 0 /*original*/
			ros_thread_sleep(param);
			#else /*PhilipHack*/
			ros_thread_sleep(100);
			#endif
		}
	}
}
#endif

void
V33_mtk_eint_enable(
	void
)
{
	profLogPrintf(0, "%s()", __func__);

#if defined(HW_KX1371)
	V33_UART_FlowControl_RxPause(); /*PhilipAdd@20120615*/
#else /* EVB */

	UINT32 ret;

	V33_UART_FlowControl_RxFreeRun(); /*PhilipAdd@20120615*/

	#if 1
    ret = sp5kGpioIntEnable(SP5K_GPIO_INT5 /*gpio15*/, /*ENABLE*/ 1);
	profLogPrintf(0, "V33_mtk_eint_enable return 0x%x", ret);
	printf("V33_mtk_eint_enable return 0x%x\n", ret);
	#endif
#endif
}

void V33_mtk_eint_disable(void)
{
	profLogPrintf(0, "%s()", __func__);

#if defined(HW_KX1371)

#else /* EVB */

	#if 1
	UINT32 ret;
    ret = sp5kGpioIntEnable(SP5K_GPIO_INT5 /*gpio15*/, /*DISABLE*/ 0);
	profLogPrintf(0, "V33_mtk_eint_disable return 0x%x", ret);
	printf("V33_mtk_eint_disable return 0x%x\n", ret);
	#endif
#endif
}

void
mtk_eint_init(U32 parm)
{
	UINT32 ret;
	printf("[MTK] mtk_eint_init\n");

	#if defined(HW_KX1371)
	/* No using GPIO INT pin, host MUST polling */
	#if 1 /*original*/
	sp5kOsThreadCreate("MTKEINT", mtk_eint_task, parm, 17, 17, 1, TX_AUTO_START);
	#else /*PhilipHack*/
	sp5kOsThreadCreate("MTKEINT", mtk_eint_task, parm, 7, 7, 1, TX_AUTO_START);
	#endif
	#else /* EVB */
	ret = sp5kGpioIsrReg(SP5K_GPIO_INT5/*GPIO15*/, /*ACTIVE_HIGH*/ 1, (void *)mtk_eint_IsrCallback);
    ret = sp5kGpioIntEnable(SP5K_GPIO_INT5/*GPIO15*/, /*ENABLE*/ 1);
	#endif

	return;
}
