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
 * Bt_mmi.h
 *
 * Project:
 * --------
 *   BT Project
 *
 * Description:
 * ------------
 *   This file is used on default platform as the common include header for both 
 *   internal and external BT SW. The header file should contain common system
 *   definition which is necessary known by both internal and external layers.
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
#ifndef __BT_MMI_H__
#define __BT_MMI_H__

#include "bt_types.h"

typedef enum {
   MOD_BT = 0,
   MOD_MMI,
   MOD_EXTERNAL = MOD_MMI,
   MOD_SPP_DEFAULT,
   MOD_DT = MOD_SPP_DEFAULT,
   MOD_ATCI = MOD_SPP_DEFAULT,
   MOD_ATCI_2 = MOD_SPP_DEFAULT,
   END_OF_MOD_ID
} module_type;

/* ThreadX limit msg size to 1,2,4,8,16 32-bit words.
   So the ILM SHALL be allocated by sender and freed by receiver */
typedef struct {
    unsigned long msg_id;
    void *local_para_ptr;
    module_type src_mod_id;
    module_type dest_mod_id;
    unsigned char sap_id;
    unsigned char used;
    void *peer_buff_ptr;
} ilm_struct;

typedef struct {
    ilm_struct *ilm;
} mtkMsgStruct;

#define get_ctrl_buffer sp5kMalloc
#define free_ctrl_buffer sp5kFree 

#define free_local_para sp5kFree
#define Assert(exp)  (((exp) != 0) ? (void)0 : (void)printf("Assert: file %s, line %d\n", __FILE__, __LINE__))

void free_ilm(ilm_struct *ptr);
void *construct_local_para(unsigned short size, unsigned char type);

#define LOCAL_PARA_HDR \
   kal_uint8	ref_count; \
   kal_uint16	msg_len;

/* So that we can use catcher for logging */
typedef struct local_para_struct {
   LOCAL_PARA_HDR
} local_para_struct;

#define peer_buff_struct void

typedef enum {
   TD_UL = 0x01 << 0,   /* Uplink Direction */
   TD_DL = 0x01 << 1,   /* Downlink Direction */
   TD_CTRL = 0x01 << 2, /* Control Plane. Both directions */
   TD_RESET = 0x01 << 3 /* Reset buffer content to 0 */
} transfer_direction;

typedef enum {
    io_low=0,
    io_high
} IO_level;

#endif  /* __BT_MMI_H__ */

