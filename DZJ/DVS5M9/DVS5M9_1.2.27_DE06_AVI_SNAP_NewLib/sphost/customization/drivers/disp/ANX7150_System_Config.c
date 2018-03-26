
#include "ANX7150_System_Config.h"
#include "ANX7150_Sys7150.h"
/*#include "Anx9011.h"*/

BYTE ANX7150_new_HW_interface_parameter;

void ANX7150_API_System_Config()
{
#if ANX8770_USED
    ANX7150_8770_Video_Config();
    ANX7150_8770_Audio_Config();
    ANX7150_8770_Packets_Config();
#else
    ANX7150_API_Video_Config(ANX7150_V1280x720p_50Hz,input_pixel_clk_1x_repeatition);
    /*ANX7150_API_Video_Config(ANX7150_V1920x1080i_60Hz,input_pixel_clk_1x_repeatition);*/
    ANX7150_API_Packets_Config(ANX7150_avi_sel | ANX7150_audio_sel);
    if(s_ANX7150_packet_config.packets_need_config & ANX7150_avi_sel)
        ANX7150_API_AVI_Config(YCbCr422,source_ratio,null,null,null,null,null,null,null,null,null,null,null);
    if(s_ANX7150_packet_config.packets_need_config & ANX7150_audio_sel)
        ANX7150_API_AUD_INFO_Config(null,null,null,null,null,null,null,null,null,null);
    ANX7150_API_AUD_CHStatus_Config(null,null,null,null,null,null,null,null,null,0/*ANX7150_AUD_CLK_SAMPLE_RATE*/);
#endif
}

void ANX7150_API_Video_Config(BYTE video_id,BYTE input_pixel_rpt_time)
{
    ANX7150_video_timing_id = video_id;
    ANX7150_in_pix_rpt = input_pixel_rpt_time;
}

void ANX7150_API_Packets_Config(BYTE pkt_sel)
{
    s_ANX7150_packet_config.packets_need_config = pkt_sel;
}

void ANX7150_API_AUD_CHStatus_Config(BYTE MODE,BYTE PCM_MODE,BYTE SW_CPRGT,BYTE NON_PCM,
    BYTE PROF_APP,BYTE CAT_CODE,BYTE CH_NUM,BYTE SOURCE_NUM,BYTE CLK_ACCUR,BYTE Fs)
{
#if 0
    //MODE: 0x00 = PCM Audio
    //PCM_MODE: 0x00 = 2 audio channels without pre-emphasis;
                                //0x01 = 2 audio channels with 50/15 usec pre-emphasis;
    //SW_CPRGT: 0x00 = copyright is asserted;
                                // 0x01 = copyright is not asserted;
    //NON_PCM: 0x00 = Represents linear PCM
                            //0x01 = For other purposes
    //PROF_APP: 0x00 = consumer applications;
                              // 0x01 = professional applications;

    //CAT_CODE: Category code
    //CH_NUM: 0x00 = Do not take into account
                           // 0x01 = left channel for stereo channel format
                           // 0x02 = right channel for stereo channel format
    //SOURCE_NUM: source number
                                   // 0x00 = Do not take into account
                                  // 0x01 = 1; 0x02 = 2; 0x03 = 3
    //CLK_ACCUR: 0x00 = level II
                                   // 0x01 = level I
                                   // 0x02 = level III
                                   // else reserved;
#endif
    s_ANX7150_audio_config.i2s_config.Channel_status1 = (MODE << 7) | (PCM_MODE << 5) |
        (SW_CPRGT << 2) | (NON_PCM << 1) | PROF_APP;
    s_ANX7150_audio_config.i2s_config.Channel_status2 = CAT_CODE;
    s_ANX7150_audio_config.i2s_config.Channel_status3 = (CH_NUM << 7) | SOURCE_NUM;
    s_ANX7150_audio_config.i2s_config.Channel_status4 = (CLK_ACCUR << 5) | Fs;
}

void ANX7150_API_AVI_Config(BYTE pb1,BYTE pb2,BYTE pb3,BYTE pb4,BYTE pb5,
    BYTE pb6,BYTE pb7,BYTE pb8,BYTE pb9,BYTE pb10,BYTE pb11,BYTE pb12,BYTE pb13)
{
    s_ANX7150_packet_config.avi_info.pb_byte[1] = pb1;
    s_ANX7150_packet_config.avi_info.pb_byte[2] = pb2;
    s_ANX7150_packet_config.avi_info.pb_byte[3] = pb3;
    s_ANX7150_packet_config.avi_info.pb_byte[4] = pb4;
    s_ANX7150_packet_config.avi_info.pb_byte[5] = pb5;
    s_ANX7150_packet_config.avi_info.pb_byte[6] = pb6;
    s_ANX7150_packet_config.avi_info.pb_byte[7] = pb7;
    s_ANX7150_packet_config.avi_info.pb_byte[8] = pb8;
    s_ANX7150_packet_config.avi_info.pb_byte[9] = pb9;
    s_ANX7150_packet_config.avi_info.pb_byte[10] = pb10;
    s_ANX7150_packet_config.avi_info.pb_byte[11] = pb11;
    s_ANX7150_packet_config.avi_info.pb_byte[12] = pb12;
    s_ANX7150_packet_config.avi_info.pb_byte[13] = pb13;
}

void ANX7150_API_AUD_INFO_Config(BYTE pb1,BYTE pb2,BYTE pb3,BYTE pb4,BYTE pb5,
    BYTE pb6,BYTE pb7,BYTE pb8,BYTE pb9,BYTE pb10)
{
    s_ANX7150_packet_config.audio_info.pb_byte[1] = pb1;
    s_ANX7150_packet_config.audio_info.pb_byte[2] = pb2;
    s_ANX7150_packet_config.audio_info.pb_byte[3] = pb3;
    s_ANX7150_packet_config.audio_info.pb_byte[4] = pb4;
    s_ANX7150_packet_config.audio_info.pb_byte[5] = pb5;
    s_ANX7150_packet_config.audio_info.pb_byte[6] = pb6;
    s_ANX7150_packet_config.audio_info.pb_byte[7] = pb7;
    s_ANX7150_packet_config.audio_info.pb_byte[8] = pb8;
    s_ANX7150_packet_config.audio_info.pb_byte[9] = pb9;
    s_ANX7150_packet_config.audio_info.pb_byte[10] = pb10;
}
void ANX7150_8770_Video_Config()
{
    BYTE video_id;
    i2c_read_p1_reg(0xa7, &video_id); 
    ANX7150_API_Video_Config(video_id,input_pixel_clk_1x_repeatition);
}

void ANX7150_8770_Packets_Config()
{
    BYTE c[13],c1[10],c2,c3,c4,i;
    /* AVI & Audio infoframe*/
    c2 = ANX7150_avi_sel;
    c3 = ANX7150_audio_sel;
    c4 = c2 | c3;
    ANX7150_API_Packets_Config(c4);

    for(i = 0; i < 13; i++)
        i2c_read_p1_reg(0xa4 + i, &c[i]);
    /* audio infoframe*/
    for(i=0; i<10; i++)
        i2c_read_p1_reg((0xc4+i), &c1[i]);
    ANX7150_API_AVI_Config(c[0],c[1],c[2],c[3],c[4],c[5],c[6],c[7],c[8],c[9],c[10],c[11],c[12]);
    ANX7150_API_AUD_INFO_Config(c1[0],c1[1],c1[2],c1[3],c1[4],c1[5],c1[6],c1[7],c1[8],c1[9]);
}

void ANX7150_8770_Audio_Config()
{
    BYTE c,ch1,ch2,ch3,ch4,ch5;

    BYTE mode; /*0x00 = PCM Audio*/
    BYTE pcm_mode;  /*0x00 = 2 audio channels without pre-emphasis;*/
                                /*0x01 = 2 audio channels with 50/15 usec pre-emphasis;*/
    BYTE sw_cprt; /* 0x00 = copyright is asserted;*/
                                /* 0x01 = copyright is not asserted;*/
    BYTE non_pcm;/*0x00 = Represents linear PCM*/
                            /*0x01 = For other purposes*/
    BYTE prof_app; /*0x00 = consumer applications;*/
                              /* 0x01 = professional applications;*/

    BYTE cat_code;/* Category code*/
    BYTE ch_num;/* 0x00 = Do not take into account*/
                           /* 0x01 = left channel for stereo channel format*/
                           /* 0x02 = right channel for stereo channel format*/
    BYTE source_num;/* source number*/
                                   /* 0x00 = Do not take into account*/
                                  /* 0x01 = 1; 0x02 = 2; 0x03 = 3*/
    BYTE clk_accur;   /* 0x00 = level II*/
                                   /* 0x01 = level I*/
                                   /* 0x02 = level III
*/
                                   /* else reserved;*/
    BYTE Fs;
    /* read channel status from anx9011*/
    i2c_read_p0_reg(0xc7, &ch1);
    i2c_read_p0_reg(0xc8, &ch2);
    i2c_read_p0_reg(0xc9, &ch3);
    i2c_read_p0_reg(0xca, &ch4);
    i2c_read_p0_reg(0xcb, &ch5);
    i2c_read_p0_reg(0x15, &c);
	if (c & 0x08)
	{
			s_ANX7150_audio_config.audio_layout = 0x80;
	}
	else
	{
			s_ANX7150_audio_config.audio_layout = 0x00;
	}
    /*debug_printf("@@@@@@ch4= %.2x\n", (unsigned int) ch4 );*/

    /* audio channel status config*/
    /* system should give value to below varibles according its need.*/
    mode = (ch1>>6) & 0x03;
    pcm_mode =  (ch1>>3) & 0x07;
    sw_cprt = (ch1>>2) & 0x01;
    non_pcm = (ch1>>1) & 0x01;
    prof_app = ch1 & 0x01;

    cat_code = ch2;

    ch_num = (ch3>>4) & 0x0f;
    source_num = ch3 & 0x0f;

    clk_accur = (ch4>>4) & 0x03;

    Fs = ch4 & 0x0f;

	/*debug_printf("@@@@@@Fs= %.2x\n", (unsigned int) Fs );*/
    ANX7150_API_AUD_CHStatus_Config(mode,pcm_mode,sw_cprt,non_pcm,
        prof_app,cat_code,ch_num,source_num,clk_accur,Fs);

}

BIT ANX7150_API_DetectDevice(void)
{
    return ANX7150_Chip_Located();
}

void ANX7150_API_HoldSystemConfig(BIT bHold_ANX7150)
{
	ANX7150_app_hold_system_config = bHold_ANX7150;
}

void ANX7150_API_ShutDown(BIT bShutDown_ANX7150)
{
    ANX7150_shutdown = bShutDown_ANX7150;
}

void ANX7150_API_HDCP_ONorOFF(BIT HDCP_ONorOFF)
{
    ANX7150_HDCP_enable = HDCP_ONorOFF;/* 1: on;  0:off*/
}

void ANX7150_API_Input_Changed()
{
   /* BYTE c,c1,c2,c3,c4,c5;*/
#if ANX8770_USED
    if(ANX7150_new_csc)/*change input color space*/
    {
        ANX7150_new_csc = 0;
        /*first send SET-AVMUTE and delay some time*/
        ANX7150_API_Set_AVMute();
        delay_ms(50);
        ANX7150_API_Clean_HDCP();
        debug_puts("ANX7150_new_csc---------------------------------------------");

        //then change the input
        switch(ANX7150_RGBorYCbCr)
        {
            case ANX7150_RGB:
                i2c_write_p0_reg(HDMI_RX_VID_AOF_REG, 0x00);
                break;
            case ANX7150_YCbCr422:
                 i2c_write_p0_reg(HDMI_RX_VID_AOF_REG, 0x03);
                break;
            case ANX7150_YCbCr444:
                 i2c_write_p0_reg(HDMI_RX_VID_AOF_REG, 0x01);
                break;
            default:
                break;
        }
    }

    if(ANX7150_new_vid_id)/*change input video resolution*/
    {
        ANX7150_new_vid_id = 0;
        //first send SET-AVMUTE and delay some time
        ANX7150_API_Set_AVMute();
        delay_ms(50);
        ANX7150_API_Clean_HDCP();
        debug_puts("ANX7150_new_vid_id---------------------------------------------");

        /*then change the input*/
        /*null*/
    }

    /*the function below is just for HW interface demo, the real system should never change HW interface*/
    if(ANX7150_new_HW_interface)
    {
        ANX7150_new_HW_interface = 0;
        ANX7150_API_Set_AVMute();
        delay_ms(50);
        ANX7150_API_Clean_HDCP();
        debug_puts("ANX7150_new_HW_interface---------------------------------------------");
        delay_ms(50);
        switch (ANX7150_new_HW_interface_parameter)
        {
            case 0x00:/*SepSYNC*/
                c = 0x03;	/*ycbcr422*/
                c2 = 0x04;
                c3 = 0x01;
                break;
            case 0x01: /*656*/
                c = 0x03;
                c2 = 0x04;
                c3 = 0x01;
                break;
            case 0x02:/*YCMUX*/
                c = 0x07;
                c2 = 0x04;
                c3 = 0x01;
                break;
            case 0x03:/*656+YCMUX*/
                c = 0x0f;
                c2 = 0x04;
                c3 = 0x01;
                break;
            case 0x04:/*DDR*/
                c = 0x03;
                c2 = 0;
                c3 = 0x01;
                break;
            case 0x05:/*DE GEN*/
                c = 0x03;
                c2 = 0x04;
                c3 = 0x00;
                i2c_write_p0_reg(0x19, 0x01);
                delay_ms(3);
                i2c_write_p0_reg(0x19, 0x00);
                break;
            default: break;
        }
        i2c_write_p0_reg(HDMI_RX_VID_AOF_REG, c);
        i2c_read_p0_reg(0x17, &c1);
        i2c_write_p0_reg(0x17, (c1 & 0xf3) |c2);/*DDR*/

#if 1
		i2c_read_p0_reg(0x40, &c1);
        i2c_write_p0_reg(0x40, (c1 & 0xfe) |c3);/*DE*/

#else
		i2c_read_p0_reg(0x19, &c1);
        i2c_write_p0_reg(0x19, (c1 & 0xfd) |c3);//DE
#endif
        i2c_read_p0_reg(HDMI_RX_VID_AOF_REG, &c);
        if(c == 0x0)
            c4 = ANX7150_RGB;
        else if((c == 0x81) ||(c == 0x01))
            c4 = ANX7150_YCbCr444;
        else
            c4 = ANX7150_YCbCr422;
        if(c == 0x0b)
            c5 = ANX7150_YCrCb422_EmbSync;
        else if(c == 0x07)
            c5 = ANX7150_YCMux422_SepSync_Mode1;
        else if(c == 0x0f)
            c5 = ANX7150_YCMux422_EmbSync_Mode1;
        else
        {
            i2c_read_p0_reg(0x17, &c);
            if(c & 0x04)
            {
                i2c_read_p0_reg(0x79, &c);/*just for DE enable*/
                if(c & 0x01)
                    c5 = ANX7150_RGB_YCrCb444_SepSync;
                else
                    c5 = ANX7150_RGB_YCrCb444_SepSync_No_DE;
            }
            else
                c5 = ANX7150_RGB_YCrCb444_DDR_SepSync;
        }
        ANX7150_video_format_config = c5;
        ANX7150_RGBorYCbCr = c4;
    }
#else
    /*ANX7150_User_Change_Input ();*/
#endif
}

/*			
void ANX7150_User_Change_Input ()
{
    if(0)
    {
        //first send SET-AVMUTE and delay some time
        ANX7150_API_Set_AVMute();
        delay_ms(50);
        ANX7150_API_Clean_HDCP();

        //then change the input
        // TODO:User changes , video resolution, audio Fs, ect. here
        ANX7150_system_config_done = 0;
    }
}
*/
void ANX7150_API_Set_AVMute()
{
    ANX7150_Set_AVMute();
}

void ANX7150_API_Clean_HDCP()
{
    ANX7150_Clean_HDCP();
}

