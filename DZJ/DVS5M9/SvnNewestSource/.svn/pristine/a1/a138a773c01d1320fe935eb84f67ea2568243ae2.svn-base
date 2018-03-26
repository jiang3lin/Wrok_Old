/*******************************************************************************
*
*       Copyright (c) 2006-2008 by Sunplus mMedia Inc., Ltd.
*
*  This software is copyrighted by and is the property of Sunplus mMedia
*  Inc., Ltd. All rights are reserved by Sunplus mMedia Inc., Ltd.
*  This software may only be used in accordance with the corresponding
*  license agreement. Any unauthorized use, duplication, distribution,
*  or disclosure of this software is expressly forbidden.
*
*  This Copyright notice MUST not be removed or modified without prior
*  written consent of Sunplus mMedia Inc., Ltd.
*
*  Sunplus mMedia Inc., Ltd. reserves the right to modify this software
*  without notice.
*
*  Sunplus mMedia Inc., Ltd.
*  19-1, Innovation First Road, Science-Based Industrial Park,
*  Hsin-Chu, Taiwan, R.O.C.
*
*
*******************************************************************************/
/* standard Lib  */
#include "common.h"
#include <stdlib.h>
#include <string.h>
#include "EP932_SysController.h"   /* HDMI Transmiter  */
#include "EP932_SystemConfig.h"
#include "api/sp5k_global_api.h"
#include "customization/dev_init.h"


EP932C_REGISTER_MAP EP932C_Registers;
#define AV_STABLE_TIME            1000

#define CmpInTolarence(Value, Compared, Tolarence) ((Value >= (Compared - Tolarence)) && (Value <= (Compared + Tolarence)))

#ifndef min
#define min(a,b) (a<b)? a:b
#endif


/* HDCP Key   */
unsigned char HDCP_Key[64][8];

/*                             */
/* Global State and Flags      */
/*



 System flags
bdata unsigned char EP932C_Flags[2];

sbit Event_HDMI_Int = EP932C_Flags[0]^0;
...

sbit is_Cap_HDMI = EP932C_Flags[0]^3;
sbit is_Cap_YCC444 = EP932C_Flags[0]^4;
sbit is_Cap_YCC422 = EP932C_Flags[0]^5;
sbit is_Connected = EP932C_Flags[0]^6;
sbit is_ReceiverSense = EP932C_Flags[0]^7;

sbit is_TimingChanging = EP932C_Flags[1]^0;
sbit is_VideoChanging = EP932C_Flags[1]^1;
sbit is_AudioChanging = EP932C_Flags[1]^2;
sbit is_HDCP_Info_BKSV_Rdy = EP932C_Flags[1]^3;
sbit is_Hot_Plug = EP932C_Flags[1]^4;
*/

unsigned char is_Cap_HDMI;
unsigned char is_Cap_YCC444;
unsigned char is_Cap_YCC422;
unsigned char is_Connected;
unsigned char is_ReceiverSense;

unsigned char is_TimingChanging;
unsigned char is_VideoChanging;
unsigned char is_AudioChanging;
unsigned char is_HDCP_Info_BKSV_Rdy;
unsigned char is_Hot_Plug;

/*
Global Data    */


/* Temp Data  */
unsigned char ChkSum, VC_Temp, ConnectionState;
/*int i;  */

unsigned char HTPLG_NOW = 0, HTPLG_LAST = 0;


/* System Data */
TX_STATE TX_State;
unsigned int HTP_TimeCount, VideoChg_TimeCount, AudioChg_TimeCount, ReadEDID_TimeCount;
unsigned char Process_Dispatch_ID;

unsigned char HP_ChangeCount, RSEN_ChangeCount, Backup_Analog_Test_Control;

VDO_PARAMS Video_Params;
ADO_PARAMS Audio_Params;
unsigned char Gamut_Packet_Header_Backup[3];

/* Register */
PEP932C_REGISTER_MAP pEP932C_Registers;
extern EP932C_REGISTER_MAP EP932C_Registers;

/*                        */
/* Private Functions      */
/*                       */

void EP932Controller_Reset(void);

void TXS_RollBack_Wait_Upstream(void);
void TXS_RollBack_Stream(void);
void TXS_RollBack_HDCP(void);

/* Hardware */
void ReadInterruptFlags(void);
/*void On_HDMI_Int();*/

EP932C_CALLBACK EP932C_GenerateInt;

/*                   */
/* System State       */
/*                  */

unsigned int HDCP_TimeCount = 0;
HDCP_STATE HDCP_State = 0;
unsigned char HDCP_Status = 0;
unsigned char RI_Check = 0, Fake_HDCP = 0;
unsigned char *pKSVList = 0, *pBKSV_BCAPS3 = 0, *pSHA_M0 = 0;
unsigned char KSVListNumber = 0;


/*                     */
/* Global Data         */
/*                     */
unsigned long Temp_SHA_H[5];
unsigned char Temp_HDCP[10];

/*int i, j;  */

/*         */
/* Private Functions */
/*            */

unsigned char HDCP_validate_RI(void);

/* Repeater      */
unsigned char HDCP_compute_SHA_message_digest(unsigned char hdcp_dev_count, unsigned char hdcp_depth);

/* SHA  */
void SHA_Initial(void);
void SHA_Push_Data(unsigned char *pData, unsigned char Size);
unsigned long *SHA_Get_SHA_Digest(void);
void SHA_Calculation(unsigned long pSHA_H[5], unsigned long pSHA_W1[16]);


/* EDID    */
static int edid_i, edid_j;

/* DDC_If  */
SMBUS_STATUS status;

unsigned char DDC_Data[128];
unsigned char TempBit;

/* Private Functions  */
SMBUS_STATUS DDC_Write(unsigned char IICAddr, unsigned char ByteAddr, void *Data, unsigned int Size);
SMBUS_STATUS DDC_Read(unsigned char IICAddr, unsigned char ByteAddr, void *Data, unsigned int Size);

/* EP932_IF  */

/*--------------------------------------------------------------------------------------------------*/

/* Private Data  */
unsigned char IIC_EP932_Addr,IIC_Key_Addr;

/*int i, j; */
/*SMBUS_STATUS status; */
/*USHORT TempUSHORT; */
unsigned short TempUSHORT;

unsigned char Temp_Data[15];
unsigned char W_Data[2];

/* Global date for HDMI Transmiter  */
unsigned char is_HDCP_AVMute;
unsigned char is_AMute;
unsigned char is_VMute;
unsigned char is_HDMI;
unsigned char is_RSEN;
unsigned char Cache_EP932_DE_Control;

/* Private Functions  */
SMBUS_STATUS IIC_Write(unsigned char IICAddr, unsigned char ByteAddr, unsigned char *Data, unsigned int Size);
SMBUS_STATUS IIC_Read(unsigned char IICAddr, unsigned char ByteAddr, unsigned char *Data, unsigned int Size);


/*==================================================================================================*/

/* Public Function Implementation

*/
/*--------------------------------------------------------------------------------------------------*/
/* Hardware Interface*/

void EP932_If_Initial()	/*customer setting */
{
	IIC_EP932_Addr = 0x70;
	IIC_Key_Addr = 0xA8;
}

void EP932_If_Reset(void)
{
	int i;

	/* Global date for HDMI Transmiter */
	is_HDCP_AVMute = 0;
	is_AMute = 1;
	is_VMute = 1;
	is_HDMI = 0;
	is_RSEN = 0;
	Cache_EP932_DE_Control = 0x03;

	/* Initial Settings */
	EP932_Reg_Set_Bit(EP932_General_Control_1, EP932_General_Control_1__VTX);
	EP932_Reg_Set_Bit(EP932_General_Control_1, EP932_General_Control_1__INT_OD);

	/* Default Audio Mute */
	EP932_Reg_Set_Bit(EP932_Color_Space_Control, EP932_Color_Space_Control__AMUTE);
	EP932_Reg_Set_Bit(EP932_Pixel_Repetition_Control, EP932_Pixel_Repetition_Control__CTS_M);
	/* Default Video Mute */
	EP932_Reg_Set_Bit(EP932_Color_Space_Control, EP932_Color_Space_Control__VMUTE);

	/*                             */
	/* Set Default AVI Info Frame  */
	/*                             */
	memset(Temp_Data, 0x00, 14);

	/* Set AVI Info Frame to RGB  */
	Temp_Data[1] &= 0x60;
	Temp_Data[1] |= 0x00; /* RGB  */

	/* Set AVI Info Frame to 601  */
	Temp_Data[2] &= 0xC0;
	Temp_Data[2] |= 0x40;

	/* Write AVI Info Frame  */
	Temp_Data[0] = 0;
	for(i=1; i<14; ++i) {
		Temp_Data[0] += Temp_Data[i];
	}
	Temp_Data[0] = ~(Temp_Data[0] - 1);
	EP932_Reg_Write(EP932_AVI_Packet, Temp_Data, 14);


	/*                             */
	/* Set Default ADO Info Frame  */
	/*                             */
	memset(Temp_Data, 0x00, 6);

	/* Write ADO Info Frame   */
	Temp_Data[0] = 0;
	for(i=1; i<6; ++i) {
		Temp_Data[0] += Temp_Data[i];
	}
	Temp_Data[0] = ~(Temp_Data[0] - 1);
	EP932_Reg_Write(EP932_ADO_Packet, Temp_Data, 6);
}

/*--------------------------------------------------------------------------------------------------*/
/*  */
/* HDMI Transmiter (EP932-Tx Implementation)  */
/*                                                */
void HDMI_Tx_Power_Down(void)
{
	/* Software power down  */
	EP932_Reg_Clear_Bit(EP932_General_Control_1, EP932_General_Control_1__PU);
}

void HDMI_Tx_Power_Up(void)
{
	/* Software power up  */
	EP932_Reg_Set_Bit(EP932_General_Control_1, EP932_General_Control_1__PU);
}

unsigned char HDMI_Tx_HTPLG(void)
{
	/* Software HotPlug Detect  */
	EP932_Reg_Read(EP932_General_Control_2, Temp_Data, 1);
	is_RSEN = (Temp_Data[0] & EP932_General_Control_2__RSEN)? 1:0;
	if(Temp_Data[0] & EP932_General_Control_2__HTPLG)
	{
		return 1;
	}
	else
	{
		/*DBG_printf(("HDMI_Tx_HTPLG disconnect\r\n"));  */
		return 0;
	}
	/* This is for old DVI monitor compatibility. For HDMI TV, there is no need to poll the EDID. */
	/*return Downstream_Rx_poll_EDID();          */
}

unsigned char HDMI_Tx_RSEN(void)
{
	return is_RSEN;
}

void HDMI_Tx_HDMI(void)
{
	if(!is_HDMI) {
		is_HDMI = 1;
		EP932_Reg_Set_Bit(EP932_General_Control_4, EP932_General_Control_4__HDMI);
		printf("Set to HDMI mode\r\n");
	}
}

void HDMI_Tx_DVI(void)
{
	if(is_HDMI) {
		is_HDMI = 0;
		EP932_Reg_Clear_Bit(EP932_General_Control_4, EP932_General_Control_4__HDMI);
		printf("Set to DVI mode\r\n");
	}
}

/*------------------------------------  */
/* HDCP                                 */

void HDMI_Tx_Mute_Enable(void)
{
	is_HDCP_AVMute = 1;
	EP932_Reg_Set_Bit(EP932_Color_Space_Control, EP932_Color_Space_Control__AMUTE | EP932_Color_Space_Control__VMUTE);
	EP932_Reg_Set_Bit(EP932_Pixel_Repetition_Control, EP932_Pixel_Repetition_Control__CTS_M);
}

void HDMI_Tx_Mute_Disable(void)
{
	is_HDCP_AVMute = 0;

	if(!is_AMute) {
		EP932_Reg_Clear_Bit(EP932_Pixel_Repetition_Control, EP932_Pixel_Repetition_Control__CTS_M);
		EP932_Reg_Clear_Bit(EP932_Color_Space_Control, EP932_Color_Space_Control__AMUTE);
	}
	if(!is_VMute) {
		EP932_Reg_Clear_Bit(EP932_Color_Space_Control, EP932_Color_Space_Control__VMUTE);
	}
}

void HDMI_Tx_HDCP_Enable(void)
{
	EP932_Reg_Set_Bit(EP932_General_Control_5, EP932_General_Control_5__ENC_EN);
}

void HDMI_Tx_HDCP_Disable(void)
{

	EP932_Reg_Clear_Bit(EP932_General_Control_5, EP932_General_Control_5__ENC_EN);
}

void HDMI_Tx_RPTR_Set(void)
{
	EP932_Reg_Set_Bit(EP932_General_Control_5, EP932_General_Control_5__RPTR);
}

void HDMI_Tx_RPTR_Clear(void)
{
	EP932_Reg_Clear_Bit(EP932_General_Control_5, EP932_General_Control_5__RPTR);
}

void HDMI_Tx_write_AN(unsigned char *pAN)
{
	EP932_Reg_Write(EP932_AN, pAN, 8);
}

unsigned char HDMI_Tx_AKSV_RDY(void)
{
	status = EP932_Reg_Read(EP932_General_Control_5, Temp_Data, 1);
	if(status != SMBUS_STATUS_Success) {
		printf("ERROR: AKSV RDY - MCU IIC %d\r\n", (int)status);
		return 0;
	}
	return (Temp_Data[0] & EP932_General_Control_5__AKSV_RDY)? 1:0;
}

unsigned char HDMI_Tx_read_AKSV(unsigned char *pAKSV)
{
	int i, j;

	status = EP932_Reg_Read(EP932_AKSV, pAKSV, 5);
	if(status != SMBUS_STATUS_Success) {
		printf("ERROR: AKSV read - MCU IIC %d\r\n", (int)status);
		return 0;
	}

	i = 0;
	j = 0;
	while (i < 5) {
		Temp_Data[0] = 1;
		while (Temp_Data[0]) {
			if (pAKSV[i] & Temp_Data[0]) j++;
			Temp_Data[0] <<= 1;
		}
		i++;
	}
	if(j != 20) {
		printf("ERROR: AKSV read - Key Wrong\r\n");
		return 0;
	}
	return 1;
}

void HDMI_Tx_write_BKSV(unsigned char *pBKSV)
{
	EP932_Reg_Write(EP932_BKSV, pBKSV, 5);
}

unsigned char HDMI_Tx_RI_RDY(void)
{
	EP932_Reg_Read(EP932_General_Control_5, Temp_Data, 1);
	return (Temp_Data[0] & EP932_General_Control_5__RI_RDY)? 1:0;
}

unsigned char HDMI_Tx_read_RI(unsigned char *pRI)
{
	status = EP932_Reg_Read(EP932_RI, pRI, 2);
	if(status != SMBUS_STATUS_Success) {
		printf("ERROR: Tx Ri read - MCU IIC %d\r\n", (int)status);
		return 0;
	}
	return 1;
}

void HDMI_Tx_read_M0(unsigned char *pM0)
{
	status = EP932_Reg_Read(EP932_M0, pM0, 8);
}

SMBUS_STATUS HDMI_Tx_Get_Key(unsigned char *Key)
{
	return IIC_Read(HEY_ADDR, 0, Key, 512);
}

/*------------------------------------  */
/* Special for config                    */

void HDMI_Tx_AMute_Enable(void)
{
	unsigned char Temp_byte[2];
	if(!is_AMute) {
		is_AMute = 1;
		EP932_Reg_Set_Bit(EP932_Color_Space_Control, EP932_Color_Space_Control__AMUTE);
		EP932_Reg_Set_Bit(EP932_Pixel_Repetition_Control, EP932_Pixel_Repetition_Control__CTS_M);

		EP932_Reg_Clear_Bit(EP932_IIS_Control, EP932_IIS_Control__ADO_EN);	/* add by eric.lu  */
		EP932_Reg_Clear_Bit(EP932_IIS_Control, EP932_IIS_Control__AUDIO_EN);	/* add by eric.lu  */

		printf("<<< AMute_enable >>>\r\n");

		/*read for verify  */
		EP932_Reg_Read(EP932_Color_Space_Control, Temp_byte, 1);
		printf("EP932_Color_Space_Control = 0x%02X\r\n",(int)Temp_byte[0]);
		EP932_Reg_Read(EP932_Pixel_Repetition_Control, Temp_byte, 1);
		printf("EP932_Pixel_Repetition_Control = 0x%02X\r\n",(int)Temp_byte[0]);
		/* add end  */
	}
}

void HDMI_Tx_AMute_Disable(void)
{
	unsigned char Temp_byte[2];
	if(is_AMute) {
		is_AMute = 0;
		if(!is_HDCP_AVMute) {
			EP932_Reg_Clear_Bit(EP932_Pixel_Repetition_Control, EP932_Pixel_Repetition_Control__CTS_M);
			EP932_Reg_Clear_Bit(EP932_Color_Space_Control, EP932_Color_Space_Control__AMUTE);

			EP932_Reg_Set_Bit(EP932_IIS_Control, EP932_IIS_Control__ADO_EN);		/* add by eric.lu */
			EP932_Reg_Set_Bit(EP932_IIS_Control, EP932_IIS_Control__AUDIO_EN);	/* add by eric.lu  */

			DBG_printf(("<<< AMute_disable >>>\r\n"));

			/*read for verify  */
			EP932_Reg_Read(EP932_Color_Space_Control, Temp_byte, 1);
			DBG_printf(("EP932_Color_Space_Control = 0x%02X\r\n",(int)Temp_byte[0]));
			EP932_Reg_Read(EP932_Pixel_Repetition_Control, Temp_byte, 1);
			DBG_printf(("EP932_Pixel_Repetition_Control = 0x%02X\r\n",(int)Temp_byte[0]));
			/* add end  */
		}
	}
}

void HDMI_Tx_VMute_Enable(void)
{
	if(!is_VMute) {
		is_VMute = 1;
		EP932_Reg_Set_Bit(EP932_Color_Space_Control, EP932_Color_Space_Control__VMUTE);

		printf("<<< VMute_enable >>>\r\n");
	}
}

void HDMI_Tx_VMute_Disable(void)
{
	if(is_VMute) {
		is_VMute = 0;
		if(!is_HDCP_AVMute) {
			EP932_Reg_Clear_Bit(EP932_Color_Space_Control, EP932_Color_Space_Control__VMUTE);

			printf("<<< VMute_disable >>>\r\n");
		}
	}
}

void HDMI_Tx_Video_Config(PVDO_PARAMS Params)
{
	int i;
	printf("\r\nStart Tx Video Config\r\n");

	/* */
	/* Disable Video        */
	/*                           */
	EP932_Reg_Clear_Bit(EP932_IIS_Control, EP932_IIS_Control__AVI_EN);

	/*                      */
	/* Video Settings        */
	/*                  */
	/* Interface  */
	EP932_Reg_Read(EP932_General_Control_3, Temp_Data, 1);
	Temp_Data[0] &= ~0xF0;
	Temp_Data[0] |= Params->Interface & 0xF0;
	EP932_Reg_Write(EP932_General_Control_3, Temp_Data, 1);

	EP932_Reg_Read(EP932_General_Control_1, Temp_Data, 1);
	Temp_Data[0] &= ~0x0E;
	Temp_Data[0] |= Params->Interface & 0x0E;
	EP932_Reg_Write(EP932_General_Control_1, Temp_Data, 1);

	if(Params->Interface & 0x01) {
		EP932_Reg_Set_Bit(EP932_General_Control_4, EP932_General_Control_4__FMT12);
	}
	else {
		EP932_Reg_Clear_Bit(EP932_General_Control_4, EP932_General_Control_4__FMT12);
	}

	/* Sync Mode  */
	switch(Params->SyncMode) {
		default:
	 	case SYNCMODE_HVDE:
			/* Disable E_SYNC */
			EP932_Reg_Clear_Bit(EP932_General_Control_4, EP932_General_Control_4__E_SYNC);
			/* Disable DE_GEN  */
			Cache_EP932_DE_Control &= ~EP932_DE_Control__DE_GEN;
			/*EP932_Reg_Write(EP932_DE_Control, &Cache_EP932_DE_Control, 1);  */

			/* Regular VSO_POL, HSO_POL  */
			if((Params->HVPol & VNegHPos) != (EP932_VDO_Settings[Params->VideoSettingIndex].HVRes_Type.HVPol & VNegHPos)) { /* V  */
				Cache_EP932_DE_Control |= EP932_DE_Control__VSO_POL; /* Invert  */
			}
			else {
				Cache_EP932_DE_Control &= ~EP932_DE_Control__VSO_POL;
			}
			if((Params->HVPol & VPosHNeg) != (EP932_VDO_Settings[Params->VideoSettingIndex].HVRes_Type.HVPol & VPosHNeg)) { /* H  */
				Cache_EP932_DE_Control |= EP932_DE_Control__HSO_POL; /* Invert  */
			}
			else {
				Cache_EP932_DE_Control &= ~EP932_DE_Control__HSO_POL;
			}
			printf("Set Sync mode to DE mode\r\n");
			break;

		case SYNCMODE_HV:
			/* Disable E_SYNC  */
			EP932_Reg_Clear_Bit(EP932_General_Control_4, EP932_General_Control_4__E_SYNC);
			/* Enable DE_GEN  */
			Cache_EP932_DE_Control |= EP932_DE_Control__DE_GEN;
			/*EP932_Reg_Write(EP932_DE_Control, &Cache_EP932_DE_Control, 1);  */

			/* Regular VSO_POL, HSO_POL  */
			if((Params->HVPol & VNegHPos) != (EP932_VDO_Settings[Params->VideoSettingIndex].HVRes_Type.HVPol & VNegHPos)) { /* V */
				Cache_EP932_DE_Control |= EP932_DE_Control__VSO_POL; /* Invert  */
			}
			else {
				Cache_EP932_DE_Control &= ~EP932_DE_Control__VSO_POL;
			}
			if((Params->HVPol & VPosHNeg) != (EP932_VDO_Settings[Params->VideoSettingIndex].HVRes_Type.HVPol & VPosHNeg)) { /* H */
				Cache_EP932_DE_Control |= EP932_DE_Control__HSO_POL; /* Invert */
			}
			else {
				Cache_EP932_DE_Control &= ~EP932_DE_Control__HSO_POL;
			}

			/* Set DE generation params  */
			if(Params->VideoSettingIndex < EP932_VDO_Settings_Max) {
				Cache_EP932_DE_Control &= ~0x03;

			#ifdef Little_Endian
				Cache_EP932_DE_Control |= ((unsigned char *)&EP932_VDO_Settings[Params->VideoSettingIndex].DE_Gen.DE_DLY)[1];
				Temp_Data[0] = ((unsigned char *)&EP932_VDO_Settings[Params->VideoSettingIndex].DE_Gen.DE_DLY)[0];
				EP932_Reg_Write(EP932_DE_DLY, Temp_Data, 1);
			#else	/* Big Endian  */
				Cache_EP932_DE_Control |= ((unsigned char *)&EP932_VDO_Settings[Params->VideoSettingIndex].DE_Gen.DE_DLY)[0];
				Temp_Data[0] = ((unsigned char *)&EP932_VDO_Settings[Params->VideoSettingIndex].DE_Gen.DE_DLY)[1];
				EP932_Reg_Write(EP932_DE_DLY, Temp_Data, 1);
			#endif

				Temp_Data[0] = ((unsigned char *)&EP932_VDO_Settings[Params->VideoSettingIndex].DE_Gen.DE_TOP)[0];
				EP932_Reg_Write(EP932_DE_TOP, Temp_Data, 1);

			#ifdef Little_Endian
				Temp_Data[0] = ((unsigned char *)&EP932_VDO_Settings[Params->VideoSettingIndex].DE_Gen.DE_CNT)[0];
				Temp_Data[1] = ((unsigned char *)&EP932_VDO_Settings[Params->VideoSettingIndex].DE_Gen.DE_CNT)[1];
				EP932_Reg_Write(EP932_DE_CNT, Temp_Data, 2);
			#else	/* Big Endian  */
				Temp_Data[0] = ((unsigned char *)&EP932_VDO_Settings[Params->VideoSettingIndex].DE_Gen.DE_CNT)[1];
				Temp_Data[1] = ((unsigned char *)&EP932_VDO_Settings[Params->VideoSettingIndex].DE_Gen.DE_CNT)[0];
				EP932_Reg_Write(EP932_DE_CNT, Temp_Data, 2);
			#endif


			#ifdef Little_Endian
				Temp_Data[0] = ((unsigned char *)&EP932_VDO_Settings[Params->VideoSettingIndex].DE_Gen.DE_LIN)[1];
				Temp_Data[1] = ((unsigned char *)&EP932_VDO_Settings[Params->VideoSettingIndex].DE_Gen.DE_LIN)[0];
				EP932_Reg_Write(EP932_DE_LIN, Temp_Data, 2);
			#else	/* Big Endian  */
				Temp_Data[0] = ((unsigned char *)&EP932_VDO_Settings[Params->VideoSettingIndex].DE_Gen.DE_LIN)[1];
				Temp_Data[1] = ((unsigned char *)&EP932_VDO_Settings[Params->VideoSettingIndex].DE_Gen.DE_LIN)[0];
				EP932_Reg_Write(EP932_DE_LIN, Temp_Data, 2);
			#endif

				printf("Update DE_GEN params %u", (unsigned short)EP932_VDO_Settings[Params->VideoSettingIndex].DE_Gen.DE_DLY);
				printf(", %u", (unsigned short)EP932_VDO_Settings[Params->VideoSettingIndex].DE_Gen.DE_CNT);
				printf(", %u", (unsigned short)EP932_VDO_Settings[Params->VideoSettingIndex].DE_Gen.DE_TOP);
				printf(", %u", (unsigned short)EP932_VDO_Settings[Params->VideoSettingIndex].DE_Gen.DE_LIN);
				printf("\r\n");
			}
			else {
				printf("ERROR: VideoCode overflow DE_GEN table\r\n");
			}
			break;

		case SYNCMODE_Embeded:
			/* Disable DE_GEN  */
			Cache_EP932_DE_Control &= ~EP932_DE_Control__DE_GEN;
			/* Enable E_SYNC  */
			EP932_Reg_Set_Bit(EP932_General_Control_4, EP932_General_Control_4__E_SYNC);

			/* Set E_SYNC params  */
			if(Params->VideoSettingIndex < EP932_VDO_Settings_Max) {

				Temp_Data[0] = EP932_VDO_Settings[Params->VideoSettingIndex].E_Sync.CTL;
				EP932_Reg_Write(EP932_Embedded_Sync, Temp_Data, 1);

				TempUSHORT = EP932_VDO_Settings[Params->VideoSettingIndex].E_Sync.H_DLY;
				if(!(Params->Interface & 0x04)) { /* Mux Mode  */
					TempUSHORT += 2;
				}

			#ifdef Little_Endian
				Temp_Data[0] = ((BYTE *)&TempUSHORT)[0];
				Temp_Data[1] = ((BYTE *)&TempUSHORT)[1];
				EP932_Reg_Write(EP932_H_Delay, Temp_Data, 2);
			#else	/* Big Endian  */
				Temp_Data[0] = ((BYTE *)&TempUSHORT)[1];
				Temp_Data[1] = ((BYTE *)&TempUSHORT)[0];
				EP932_Reg_Write(EP932_H_Delay, Temp_Data, 2);
			#endif


			#ifdef Little_Endian
				Temp_Data[0] = ((BYTE *)&EP932_VDO_Settings[Params->VideoSettingIndex].E_Sync.H_WIDTH)[0];
				Temp_Data[1] = ((BYTE *)&EP932_VDO_Settings[Params->VideoSettingIndex].E_Sync.H_WIDTH)[1];
				EP932_Reg_Write(EP932_H_Width, Temp_Data, 2);
			#else	/* Big Endian  */
				Temp_Data[0] = ((BYTE *)&EP932_VDO_Settings[Params->VideoSettingIndex].E_Sync.H_WIDTH)[1];
				Temp_Data[1] = ((BYTE *)&EP932_VDO_Settings[Params->VideoSettingIndex].E_Sync.H_WIDTH)[0];
				EP932_Reg_Write(EP932_H_Width, Temp_Data, 2);
			#endif

				Temp_Data[0] = EP932_VDO_Settings[Params->VideoSettingIndex].E_Sync.V_DLY;
				EP932_Reg_Write(EP932_V_Delay, Temp_Data, 1);
				/*DBG_printf(("[0x85]= 0x%02X\r\n",(int)Temp_Data[0]));  */

				Temp_Data[0] = EP932_VDO_Settings[Params->VideoSettingIndex].E_Sync.V_WIDTH;
				EP932_Reg_Write(EP932_V_Width, Temp_Data, 1);
				/*DBG_printf(("[0x86]= 0x%02X\r\n",(int)Temp_Data[0]));  */

			#ifdef Little_Endian
				Temp_Data[0] = ((BYTE *)&EP932_VDO_Settings[Params->VideoSettingIndex].E_Sync.V_OFST)[0];
				Temp_Data[1] = ((BYTE *)&EP932_VDO_Settings[Params->VideoSettingIndex].E_Sync.V_OFST)[1];
				EP932_Reg_Write(EP932_V_Off_Set, Temp_Data, 2);
			#else	/* Big Endian  */
				Temp_Data[0] = ((BYTE *)&EP932_VDO_Settings[Params->VideoSettingIndex].E_Sync.V_OFST)[1];
				Temp_Data[1] = ((BYTE *)&EP932_VDO_Settings[Params->VideoSettingIndex].E_Sync.V_OFST)[0];
				EP932_Reg_Write(EP932_V_Off_Set, Temp_Data, 2);
			#endif

				printf("Update E_SYNC params 0x%02X", (unsigned short)EP932_VDO_Settings[Params->VideoSettingIndex].E_Sync.CTL);
				printf(", %u", (unsigned short)EP932_VDO_Settings[Params->VideoSettingIndex].E_Sync.H_DLY);
				printf(", %u", (unsigned short)EP932_VDO_Settings[Params->VideoSettingIndex].E_Sync.H_WIDTH);
				printf(", %u", (unsigned short)EP932_VDO_Settings[Params->VideoSettingIndex].E_Sync.V_DLY);
				printf(", %u", (unsigned short)EP932_VDO_Settings[Params->VideoSettingIndex].E_Sync.V_WIDTH);
				printf(", %u", (unsigned short)EP932_VDO_Settings[Params->VideoSettingIndex].E_Sync.V_OFST);
				printf("\r\n");


				for(i=0x80; i<=0x88; i++)
				{
					EP932_Reg_Read(i, Temp_Data, 1);
					printf("EP932_reg[0x%02X]=0x%02X\r\n",(int)i,(int)Temp_Data[0]);
				}


				/* Regular VSO_POL, HSO_POL */
				if(EP932_VDO_Settings[Params->VideoSettingIndex].HVRes_Type.HVPol & VNegHPos) { /* VNeg?  */
					Cache_EP932_DE_Control |= EP932_DE_Control__VSO_POL;
				}
				else {
					Cache_EP932_DE_Control &= ~EP932_DE_Control__VSO_POL;
				}
				if(EP932_VDO_Settings[Params->VideoSettingIndex].HVRes_Type.HVPol & VPosHNeg) { /* HNeg?  */
					Cache_EP932_DE_Control |= EP932_DE_Control__HSO_POL;
				}
				else {
					Cache_EP932_DE_Control &= ~EP932_DE_Control__HSO_POL;
				}
			}
			else {
				printf("ERROR: VideoCode overflow E_SYNC table\r\n");
			}
			break;
	}
	EP932_Reg_Write(EP932_DE_Control, &Cache_EP932_DE_Control, 1);

	/* Pixel Repetition */
	EP932_Reg_Read(EP932_Pixel_Repetition_Control, Temp_Data, 1);
	Temp_Data[0] &= ~EP932_Pixel_Repetition_Control__PR;
	if(Params->VideoSettingIndex < EP932_VDO_Settings_Max) {
		Temp_Data[0] |= EP932_VDO_Settings[Params->VideoSettingIndex].AR_PR & 0x03;
	}
	EP932_Reg_Write(EP932_Pixel_Repetition_Control, Temp_Data, 1);



	/* Color Space  */
	switch(Params->FormatIn) {
		default:
	 	case COLORFORMAT_RGB:
			EP932_Reg_Clear_Bit(EP932_General_Control_4, EP932_General_Control_4__YCC_IN | EP932_General_Control_4__422_IN);
			printf("Set to RGB In\r\n");
			break;

	 	case COLORFORMAT_YCC444:
			EP932_Reg_Set_Bit(EP932_General_Control_4, EP932_General_Control_4__YCC_IN);
			EP932_Reg_Clear_Bit(EP932_General_Control_4, EP932_General_Control_4__422_IN);
			printf("Set to YCC444 In\r\n");
			break;
	 	case COLORFORMAT_YCC422:
			EP932_Reg_Set_Bit(EP932_General_Control_4, EP932_General_Control_4__YCC_IN | EP932_General_Control_4__422_IN);
			printf("Set to YCC422 In\r\n");
			break;
	}

	switch(Params->FormatOut) {
		default:
	 	case COLORFORMAT_RGB:
			/* Set to RGB  */
			if(Params->VideoSettingIndex < EP932_VDO_Settings_IT_Start) { /* CE Timing  */
				EP932_Reg_Clear_Bit(EP932_Color_Space_Control, EP932_Color_Space_Control__YCC_OUT | EP932_Color_Space_Control__422_OUT);
				EP932_Reg_Set_Bit(EP932_Color_Space_Control, EP932_Color_Space_Control__YCC_Range); /* Output limit range RGB  */
			}
			else { /* IT Timing  */
				EP932_Reg_Clear_Bit(EP932_Color_Space_Control, EP932_Color_Space_Control__YCC_OUT | EP932_Color_Space_Control__422_OUT | EP932_Color_Space_Control__YCC_Range);
			}
			printf("Set to RGB Out\r\n");
			break;

	 	case COLORFORMAT_YCC444:
			/* Set to YCC444  */
			EP932_Reg_Set_Bit(EP932_Color_Space_Control, EP932_Color_Space_Control__YCC_OUT);
			EP932_Reg_Clear_Bit(EP932_Color_Space_Control, EP932_Color_Space_Control__422_OUT);
			printf("Set to YCC444 Out\r\n");
			break;

	 	case COLORFORMAT_YCC422:
			/* Set to YCC422  */
			EP932_Reg_Set_Bit(EP932_Color_Space_Control, EP932_Color_Space_Control__YCC_OUT | EP932_Color_Space_Control__422_OUT);
			printf("Set to YCC422 Out\r\n");
			break;
	}

	/* Color Space  */
	switch(Params->ColorSpace) {
		default:
	 	case COLORSPACE_601:
			/* Set to 601  */
			EP932_Reg_Clear_Bit(EP932_Color_Space_Control, EP932_Color_Space_Control__COLOR);
			printf("Set to 601 color definition\r\n");
			break;

	 	case COLORSPACE_709:
			/* Set to 709  */
			EP932_Reg_Set_Bit(EP932_Color_Space_Control, EP932_Color_Space_Control__COLOR);
			printf("Set to 709 color definition\r\n");
			break;
	}

	/*                        */
	/* Update AVI Info Frame   */
	/*                    */
	/* Read AVI Info Frame   */
	memset(Temp_Data, 0x00, 14);
	/*Temp_Data[1] &= 0x60;  */
	switch(Params->FormatOut) {
		default:
	 	case COLORFORMAT_RGB:
			/* Set AVI Info Frame to RGB  */
			Temp_Data[1] |= 0x00; /* RGB   */
			break;

	 	case COLORFORMAT_YCC444:
			/* Set AVI Info Frame to RGB  */
			Temp_Data[1] |= 0x40; /* YCC 444 */
			break;

	 	case COLORFORMAT_YCC422:
			/* Set AVI Info Frame to RGB  */
			Temp_Data[1] |= 0x20; /* YCC 422  */
			break;
	}
	Temp_Data[1] |= 0x10; /* Active Format Information  */
	/*Temp_Data[2] &= 0xC0;  */

/* add by eric.lu  */
	switch(Params->SCAN) {
		default:
		case 3:						/* future 	 */
		case 0:						/* NoData    */
			Temp_Data[1] &= ~0x03;
			break;
		case 1:						/* overscan  */
			Temp_Data[1] |= 0x01;
			break;
		case 2:						/* underscan  */
			Temp_Data[1] |= 0x02;
			break;
	}
/* add of end  */

	switch(Params->ColorSpace) {
		default:
	 	case COLORSPACE_601:
			/* Set AVI Info Frame to 601  */
			Temp_Data[2] |= 0x40;
			break;

	 	case COLORSPACE_709:
			/* Set AVI Info Frame to 709  */
			Temp_Data[2] |= 0x80;
			break;
	}
	if(Params->VideoSettingIndex < EP932_VDO_Settings_Max) {
		Temp_Data[2] |= EP932_VDO_Settings[Params->VideoSettingIndex].AR_PR & 0x30;/* AVI Info Frame : Picture Aspect Ratio */
	}
	Temp_Data[2] |= Params->AFARate & 0x0F;/* AVI Info Frame : Active format Aspect Ratio */
	if(Params->VideoSettingIndex < EP932_VDO_Settings_IT_Start) {
		Temp_Data[4] |= EP932_VDO_Settings[Params->VideoSettingIndex].VideoCode;/* AVI Info Frame : Vedio Identification code */
	}
	if(Params->VideoSettingIndex < EP932_VDO_Settings_Max) {
		Temp_Data[5] |= (EP932_VDO_Settings[Params->VideoSettingIndex].AR_PR & 0x0C) >> 2;/* AVI Info Frame : Pixel Repetition  */
	}

	/* Write AVI Info Frame  */
	Temp_Data[0] = 0x91;
	for(i=1; i<6; ++i) {
		Temp_Data[0] += Temp_Data[i];
	}
	Temp_Data[0] = ~(Temp_Data[0] - 1);	/* checksum  */
	EP932_Reg_Write(EP932_AVI_Packet, Temp_Data, 14);

	printf("AVI Info: ");
	for(i=0; i<6; ++i) {
	/*	printf("0x%0.2X, ", (int)Temp_Data[i] );  */
	printf("0x%2x, ", (int)Temp_Data[i] );
	}
	printf("\r\n");

	/*               */
	/* Enable Video  */
	/*               */
	EP932_Reg_Set_Bit(EP932_IIS_Control, EP932_IIS_Control__AVI_EN);
}

void HDMI_Tx_Audio_Config(PADO_PARAMS Params)
{
	int i;
	unsigned char N_CTS_Index;
	unsigned long N_Value, CTS_Value;
	ADSFREQ FinalFrequency;
	unsigned char FinalADSRate;

	DBG_printf(("\r\nStart Tx Audio Config\r\n"));

	/*  */
	/* Audio Settings*/
	/*  */
	/* Update WS_M, WS_POL, SCK_POL  */
	EP932_Reg_Read(EP932_IIS_Control, Temp_Data, 1);
	Temp_Data[0] &= ~0x07;
	Temp_Data[0] |= Params->Interface & 0x07;
	EP932_Reg_Write(EP932_IIS_Control, Temp_Data, 1);

	/* Update Channel Status  */
	if(Params->Interface & 0x08) { /* IIS  */

		Temp_Data[0] = 0;
		/* Update Flat | IIS  */
		Temp_Data[0] |= EP932_ADO_Settings[Params->ChannelNumber].Flat;
		/* Update Channel Number  */
		if(Params->ChannelNumber > 1) {	/* 3 - 8 channel  */
			Temp_Data[0] |= EP932_Packet_Control__LAYOUT;
		}
		EP932_Reg_Write(EP932_Packet_Control, Temp_Data, 1); /* Clear IIS  */
		Temp_Data[0] |= EP932_Packet_Control__IIS;
		EP932_Reg_Write(EP932_Packet_Control, Temp_Data, 1); /* Set   IIS  */

		/* Downsample Convert  */
		FinalADSRate = Params->ADSRate;
		switch(Params->ADSRate) {
			default:
			case 0: /* Bypass  */
				/*DBG_printf(("Audio ADS = 0\r\n"));  */
				FinalADSRate = 0;
				FinalFrequency = Params->InputFrequency;
				break;
			case 1: /* 1/2  */
				/*DBG_printf(("Audio ADS = 1_2\r\n"));  */
				switch(Params->InputFrequency) {
					default: /* Bypass  */
						/*DBG_printf(("Audio ADS = 0\r\n"));  */
						FinalADSRate = 0;
						FinalFrequency = Params->InputFrequency;
						break;
					case ADSFREQ_88200Hz:
						FinalFrequency = ADSFREQ_44100Hz;
						break;
					case ADSFREQ_96000Hz:
						FinalFrequency = ADSFREQ_48000Hz;
						break;
					case ADSFREQ_176400Hz:
						FinalFrequency = ADSFREQ_88200Hz;
						break;
					case ADSFREQ_192000Hz:
						FinalFrequency = ADSFREQ_96000Hz;
						break;
				}
				break;
			case 2: /* 1/3  */
				/*DBG_printf(("Audio ADS = 1_3\r\n"));  */
				switch(Params->InputFrequency) {
					default: /* Bypass  */
						/*DBG_printf(("Audio ADS = 0\r\n"));  */
						FinalADSRate = 0;
						FinalFrequency = Params->InputFrequency;
						break;
					case ADSFREQ_96000Hz:
						FinalFrequency = ADSFREQ_32000Hz;
						break;
				}
				break;
			case 3: /* 1/4  */
				/*DBG_printf(("Audio ADS = 1_4\r\n"));  */
				switch(Params->InputFrequency) {
					default: /* Bypass  */
						/*DBG_printf(("Audio ADS = 0\r\n")); */
						FinalADSRate = 0;
						FinalFrequency = Params->InputFrequency;
						break;
					case ADSFREQ_176400Hz:
						FinalFrequency = ADSFREQ_44100Hz;
						break;
					case ADSFREQ_192000Hz:
						FinalFrequency = ADSFREQ_48000Hz;
						break;
				}
				break;
		}

		/* Update Down Sample ADSRate */
		EP932_Reg_Read(EP932_Pixel_Repetition_Control, Temp_Data, 1);
		Temp_Data[0] &= ~0x30;
		Temp_Data[0] |= (FinalADSRate << 4) & 0x30;
		EP932_Reg_Write(EP932_Pixel_Repetition_Control, Temp_Data, 1);


		/* Set Channel Status  */
		memset(Temp_Data, 0x00, 5);
		Temp_Data[0] = (Params->NoCopyRight)? 0x04:0x00;
		Temp_Data[1] = 0x00; 			/* Category code ??  */
		Temp_Data[2] = 0x00; 			/* Channel number ?? | Source number ??  */
		Temp_Data[3] = FinalFrequency; 	/* Clock accuracy ?? | Sampling frequency  */
		Temp_Data[4] = 0x01; 			/* Original sampling frequency ?? | Word length ??  */
		EP932_Reg_Write(EP932_Channel_Status, Temp_Data, 5);

		DBG_printf(("CS Info: "));
		for(i=0; i<5; ++i) {
			DBG_printf(("0x%02X, ", (int)Temp_Data[i] ));
		}
		DBG_printf(("\r\n"));

		EP932_Reg_Set_Bit(EP932_Pixel_Repetition_Control, EP932_Pixel_Repetition_Control__CS_M);
	}
	else { /* SPIDIF  */

		EP932_Reg_Set_Bit(EP932_Packet_Control, EP932_Packet_Control__IIS);
		EP932_Reg_Clear_Bit(EP932_Packet_Control, EP932_Packet_Control__FLAT3 | EP932_Packet_Control__FLAT2 | EP932_Packet_Control__FLAT1 | EP932_Packet_Control__FLAT0 |
			EP932_Packet_Control__IIS | EP932_Packet_Control__LAYOUT);

		/* No Downsample  */
		FinalADSRate = 0;
		FinalFrequency = Params->InputFrequency;

		/* Disable Down Sample and Bypass Channel Status  */
		EP932_Reg_Clear_Bit(EP932_Pixel_Repetition_Control, EP932_Pixel_Repetition_Control__ADSR | EP932_Pixel_Repetition_Control__CS_M);

		Params->ChannelNumber = 0;
	}

	/* Set CTS/N  */
	if(Params->VideoSettingIndex < EP932_VDO_Settings_Max) {
		N_CTS_Index = EP932_VDO_Settings[Params->VideoSettingIndex].Pix_Freq_Type;
		if(EP932_VDO_Settings[Params->VideoSettingIndex].HVRes_Type.Vprd % 500) { /* 59.94/60 Hz  */
			N_CTS_Index += Params->VFS;
			DBG_printf(("N_CTS_Index Shift %d\r\n", (int)Params->VFS));
		}
	}
	else {
		DBG_printf(("Use default N_CTS_Index\r\n"));
		N_CTS_Index = PIX_FREQ_25200KHz;
	}
	switch(FinalFrequency) {

		default:
		case ADSFREQ_32000Hz:
			DBG_printf(("Set to 32KHz"));
			N_Value = N_CTS_32K[N_CTS_Index].N;
			CTS_Value = N_CTS_32K[N_CTS_Index].CTS;
			break;
		case ADSFREQ_44100Hz:
			DBG_printf(("Set to 44.1KHz"));
			N_Value = N_CTS_44K1[N_CTS_Index].N;
			CTS_Value = N_CTS_44K1[N_CTS_Index].CTS;
			break;
		case ADSFREQ_48000Hz:
			DBG_printf(("Set to 48KHz"));
			N_Value = N_CTS_48K[N_CTS_Index].N;
			CTS_Value = N_CTS_48K[N_CTS_Index].CTS;
			break;
		case ADSFREQ_88200Hz:
			DBG_printf(("Set to 88.2KHz"));
			N_Value = N_CTS_44K1[N_CTS_Index].N * 2;
			CTS_Value = N_CTS_44K1[N_CTS_Index].CTS * 2;
			break;
		case ADSFREQ_96000Hz:
			DBG_printf(("Set to 96KHz"));
			N_Value = N_CTS_48K[N_CTS_Index].N * 2;
			CTS_Value = N_CTS_48K[N_CTS_Index].CTS * 2;
			break;
		case ADSFREQ_176400Hz:
			DBG_printf(("Set to 176.4KHz"));
			N_Value = N_CTS_44K1[N_CTS_Index].N * 4;
			CTS_Value = N_CTS_44K1[N_CTS_Index].CTS * 4;
			break;
		case ADSFREQ_192000Hz:
			DBG_printf(("Set to 192KHz"));
			N_Value = N_CTS_48K[N_CTS_Index].N * 4;
			CTS_Value = N_CTS_48K[N_CTS_Index].CTS * 4;
			break;
	}

	DBG_printf((", N[%i]=%lu(0x%lx)", (int)N_CTS_Index, N_Value, N_Value));
	DBG_printf((", CTS=%lu(0x%lx) \r\n", CTS_Value, CTS_Value));

	Temp_Data[0] = CTS_Value>>16;
	EP932_Reg_Write(EP932_CTS, Temp_Data, 1);
	Temp_Data[0] = CTS_Value>>8;
	EP932_Reg_Write(0x61, Temp_Data, 1);
	Temp_Data[0] = CTS_Value;
	EP932_Reg_Write(0x62, Temp_Data, 1);

	Temp_Data[0] = N_Value>>16;
	EP932_Reg_Write(EP932_N, Temp_Data, 1);
	Temp_Data[0] = N_Value>>8;
	EP932_Reg_Write(0x64, Temp_Data, 1);
	Temp_Data[0] = N_Value;
	EP932_Reg_Write(0x65, Temp_Data, 1);

/* read for verify  */

	EP932_Reg_Read(EP932_CTS, Temp_Data, 1);
	DBG_printf(("EP932_CTS_0(Reg addr 0x60) = 0x%02X\r\n",(int)Temp_Data[0]));
	EP932_Reg_Read(0x61, Temp_Data, 1);
	DBG_printf(("EP932_CTS_1(Reg addr 0x61) = 0x%02X\r\n",(int)Temp_Data[0]));
	EP932_Reg_Read(0x62, Temp_Data, 1);
	DBG_printf(("EP932_CTS_2(Reg addr 0x62) = 0x%02X\r\n",(int)Temp_Data[0]));

	EP932_Reg_Read(EP932_N, Temp_Data, 1);
	DBG_printf(("EP932_N_0(Reg addr 0x63) = 0x%02X\r\n",(int)Temp_Data[0]));
	EP932_Reg_Read(0x64, Temp_Data, 1);
	DBG_printf(("EP932_N_1(Reg addr 0x64) = 0x%02X\r\n",(int)Temp_Data[0]));
	EP932_Reg_Read(0x65, Temp_Data, 1);
	DBG_printf(("EP932_N_2(Reg addr 0x65) = 0x%02X\r\n",(int)Temp_Data[0]));

	/*   */
	/* Update ADO Info Frame  */
	/*  */
	/* Set Default ADO Info Frame  */
	memset(Temp_Data, 0x00, 6);

	/* Overwrite ADO Info Frame  */
	Temp_Data[1] = Params->ChannelNumber;
	Temp_Data[4] = EP932_ADO_Settings[Params->ChannelNumber].SpeakerMapping;

	/* Write ADO Info Frame back  */
	Temp_Data[0] = 0x8F;
	for(i=1; i<6; ++i) {
		Temp_Data[0] += Temp_Data[i];
	}
	Temp_Data[0] = ~(Temp_Data[0] - 1);
	EP932_Reg_Write(EP932_ADO_Packet, Temp_Data, 6);

	DBG_printf(("ADO Info: "));
	for(i=0; i<6; ++i) {
		DBG_printf(("0x%2x, ", (int)Temp_Data[i] ));
	}
	DBG_printf(("\r\n"));

	EP932_Reg_Set_Bit(EP932_IIS_Control, EP932_IIS_Control__ACR_EN | EP932_IIS_Control__ADO_EN | EP932_IIS_Control__AUDIO_EN);
}

/*--------------------------------------------------------------------------------------------------*/
/*           */
/* Hardware Interface  */
/*               */
SMBUS_STATUS Key_Read(unsigned char ByteAddr, void *Data, unsigned int Size)
{
	return IIC_Read(IIC_Key_Addr, ByteAddr, Data, Size);
}

SMBUS_STATUS Key_Write(unsigned char ByteAddr, void *Data, unsigned int Size)
{
	return IIC_Write(IIC_Key_Addr, ByteAddr, Data, Size);
}

SMBUS_STATUS EP932_Reg_Read(unsigned char ByteAddr, unsigned char *Data, unsigned int Size)
{
/*printf("EP932_Reg_Read size =%d\n",Size);  */
	return IIC_Read(IIC_EP932_Addr, ByteAddr, Data, Size);
}

SMBUS_STATUS EP932_Reg_Write(unsigned char ByteAddr, unsigned char *Data, unsigned int Size)
{
	/*DBG_printf(("EP932_Reg_Write 0x%02X, 0x%02X\r\n",(int)ByteAddr,(int)Data[0]));  */
	return IIC_Write(IIC_EP932_Addr, ByteAddr, Data, Size);
}

SMBUS_STATUS EP932_Reg_Set_Bit(unsigned char ByteAddr, unsigned char BitMask)
{

	int result = 1;
	result = IIC_Read(IIC_EP932_Addr, ByteAddr, Temp_Data, 1);
	if(result == 0)
	{
		/* Write back to Reg Reg_Addr  */
		Temp_Data[0] |= BitMask;

		return IIC_Write(IIC_EP932_Addr, ByteAddr, Temp_Data, 1);
	}
	else
	{
		return result;
	}
}

SMBUS_STATUS EP932_Reg_Clear_Bit(unsigned char ByteAddr, unsigned char BitMask)
{
	int result = 1;
	result = IIC_Read(IIC_EP932_Addr, ByteAddr, Temp_Data, 1);
	if(result == 0)
	{
		/* Write back to Reg Reg_Addr  */
		Temp_Data[0] &= ~BitMask;

		return IIC_Write(IIC_EP932_Addr, ByteAddr, Temp_Data, 1);
	}
	else
	{
		return result;
	}
}


/*==================================================================================================*/
/*                                    */
/* Private Functions               */
/*                         */

SMBUS_STATUS IIC_Write(unsigned char IICAddr, unsigned char ByteAddr, unsigned char *Data, unsigned int Size)
{
	/*                */
	/* How to implement this with Customer's I2C ????????????????????????????? */
	/* return 0; for success                           */
	/* return 2; for No_ACK                            */
	/* return 4; for Arbitration                                       */
	/*                                                                  */


	/*DBG_printf(("IIC_Write 0x%02X, 0x%02X\r\n",(int)ByteAddr,(int)Data[0]));  */

	/* need to use customer's I2C  function  */
	/*result = TLGI2C_WriteReg_EP932M(IICAddr, ByteAddr, Data, Size);  */
	int result = 1;
	UINT8 i;
	/* need to use customer's I2C  function  */
	/*result = I2C_WriteReg_EP932M(IICAddr, ByteAddr, Data, Size);  */

	UINT8 regaddr[1];
	UINT8 regdata[128];
	UINT8 tmp;
/*	printf ("IIC write  size =%x\n",Size); */
	regaddr[0]= ByteAddr;
	for(i=0;i<Size;i++)
	{regdata[i]= *(Data+i);}

	/*printf(" IICAddr f= %x, ByteAddr= %x, Data= %x , Size=%x \n",IICAddr,ByteAddr,Data[0],Size);  */
	WRITE8(0xb0009035, (READ8(0xb0009035)& 0xCF));/* i2c select function pins*/
	WRITE8(0xb0009148,0x1);/* clear i2c status*/
    WRITE8(0xb0009140, IICAddr);

	HDMI2_i2c_write(regaddr, regdata,(Size-1), 0, 0);

	if(Size <=8)
	{
    	HDMI2_i2c_write(regaddr, regdata,Size-1, 0, 0);
	}
	else
	{
		for(i=0;i<=Size ; i=i+8)
			{

			HDMI2_i2c_write(regaddr+i, regdata+i,8-1, 0, 0);
			}

			i=i-8;
			if (i<Size)
				{
				HDMI2_i2c_write(regaddr+i , regdata+i ,(Size-i)-1, 0, 0);
				/*printf(" IIC2-read******* size= %x ,i =%x ,result = %x \n",Size,i,(Size-i));   */
				}
	}



	tmp =READ8(0xb0009148);
	if ( tmp==0x2)
		{result =2;
		WRITE8(0xb0009148,0);
		}

	else
		{result =0x0;		}


/*	printf(" write result =  %d, regaddr=%x\n",result ,regaddr[1]);  */
	return result;


}

SMBUS_STATUS IIC_Read(unsigned char IICAddr, unsigned char ByteAddr, unsigned char *Data, unsigned int Size)
{
	/*                        */
	/* How to implement this with Customer's I2C ?????????????????????????????  */
	/* return 0; for success   */
	/* return 2; for No_ACK  */
	/* return 4; for Arbitration  */
	/*                             */



	/* need to use customer's I2C  function  */
	/*result = TLGI2C_ReadReg_EP932M(IICAddr, ByteAddr, Data, Size); */

/*
	if(result != 0)
	{
		DBG_printf(("EP932M IIC_Read error : 0x%02X, 0x%02X, 0x%02X, %d\r\n",(int)IICAddr,(int)ByteAddr,(int)Data[0],Size));
	}
*/

	int result = 1;
	UINT8 tmp;
    UINT8 i;
	/*printf ("IIC read  size =%x\n",Size); */
	/* need to use customer's I2C  function  */
	/*result = I2C_ReadReg_EP932M(IICAddr, ByteAddr, Data, Size);  */
	WRITE8(0xb0009035, (READ8(0xb0009035)& 0xCF));/* i2c select function pins*/
	WRITE8(0xb0009148,0x1);/* clear i2c status*/
    WRITE8(0xb0009140, IICAddr);
 /*  printf("iic 1 \n"); */
	if(Size <=8)
	{
    	HDMI2_i2c_read(ByteAddr, Data,Size-1 , 4, 0);
	}
	else
	{
		for(i=0;i<=Size ; i=i+8)
			{

			HDMI2_i2c_read(ByteAddr+i, Data+i,8-1 , 4, 0);

			}
			i=i-8;
			if ((i)<Size)
				{

				HDMI2_i2c_read(ByteAddr+i, Data+i,(Size-i)-1 , 4, 0);
/*printf(" IIC2-read******************** size , i \n",i, Size); */
				}
	}


   /*printf("iic 2 \n");	*/
	tmp =READ8(0xb0009148);
	if ( tmp==0x2)
		{result =2;
		WRITE8(0xb0009148,0);
		}
	else
		{result =0x0;		}
/*	printf(" read result =  %d, regaddr=%x\n",READ8(0xb0009148) , ByteAddr); */

	return result;

}
void
HDMI2_i2c_write(
	UINT8 *pregaddr,
	UINT8 *pregdata,
	UINT8 cnt,
	UINT8 mode,
	UINT8 sync
)
{
	UINT32 i;

	WRITE8(0xb0009100, mode); /* Select Sequential/Burst mode */
	WRITE8(0xb0009108, sync); /* Synchronize with Vd or Vvalid not */
	WRITE8(0xb000913C, cnt);


	if (mode) {
		for (i = 0; i < (UINT32)(cnt+1); i++) {
			if ((READ8(0xb0009138) & 0x30) == 0x30) {
				WRITE8(0xb0009110+i*4, pregaddr[i*2]);
				WRITE8(0xb0009110+i*4+2, pregaddr[i*2+1]);
			}
			else {
				WRITE8(0xb0009110+i*2, pregaddr[i]);
			}
			WRITE8(0xb0009110+i*2+1, pregdata[i]);
		}
	}
	else {

		WRITE8(0xb0009110, pregaddr[0]);
		WRITE8(0xb0009112, pregaddr[1]);
		for (i = 0; i < (UINT32)(cnt+1); i++) {
			WRITE8(0xb0009110+i*2+1, pregdata[i]);

		}
	}
    WRITE8(0xb0009104, 0x01);

	do {
		i = READ8(0xb0009104);

	} while (i == 0x01);

}

/**************************************************************************************
*                                                                                     *
*  Function Name : halFrontSsc2Read                                                     *
*                                                                                     *
*  Purposes      :                                                                    *
*    To Read data through SSC interface                                               *
*                                                                                     *
*  Desprictions  :                                                                    *
*    Read data from sensor through SSC interface                                      *
*                                                                                     *
*  Arguments     :                                                                    *
*    regaddr  : in, register address of sensor                                        *
*    pregdata : out, point of register data of sensor                                 *
*    cnt      : in, the number of data to Read                                        *
*    mode     : 2: normal read, 3: combine read, 4: combine read with restart         *
*    sync     : in, 0: immediate, 1: sync vdorg, 2: sync vvalid                       *
*                                                                                     *
*  Returns       :                                                                    *
*    None                                                                             *
*                                                                                     *
*  See also      :                                                                    *
*                                                                                     *
***************************************************************************************/

void
HDMI2_i2c_read(

		UINT8 regaddr,
		UINT8  *pregdata,
		UINT8  cnt,
		UINT8  mode,
		UINT8  sync
	)
	{
	UINT32 tmp0, i;

		/*UINT32 status=0;*/
	/*	printf(" cnt= %d, mode = %d, sync=%d\n",cnt,mode,sync); */
	/*	printf("I2C read function start1\n");	*/

		do tmp0 = (*((volatile UINT8*)(0xb0009104)));
		while (tmp0 == 0x01);						  /* Wait SSC Busy */

/*		printf("I2C read function start2\n");  */
		*(volatile UINT8*)(0xb0009100) = mode;
		*(volatile UINT8*)(0xb0009108) = sync;
		*(volatile UINT8*)(0xb000913c) = cnt;
		*(volatile UINT8*)(0xb0009110) = regaddr & 0xFF;
		*(volatile UINT8*)(0xb0009112) = regaddr >> 8;
   		*(volatile UINT8*)(0xb0009104) = 0x01;
	#if 1
		do tmp0 = (*((volatile UINT8*)(0xb0009104)));
		while (tmp0 == 0x01);						  /* Wait SSC Busy */
	#endif
/*		printf("I2C read function start3\n");	 */
		for (i = 0; i < (UINT32)(cnt+1); i++)
				pregdata[i] = (*((volatile UINT8*)(0xb0009110+i*2+1)));
}


/*DDC_IF    */

unsigned char Downstream_Rx_read_BKSV(unsigned char *pBKSV)
{
	int i, j;
	status = DDC_Read(HDCP_RX_ADDR, HDCP_RX_BKSV_ADDR, pBKSV, 5);
	if(status != SMBUS_STATUS_Success) {
		DBG_printf(("ERROR: BKSV read - DN DDC %d\r\n", (int)status));
		return 0;
	}

	i = 0;
	j = 0;
	while (i < 5) {
		TempBit = 1;
		while (TempBit) {
			if (pBKSV[i] & TempBit) j++;
			TempBit <<= 1;
		}
		i++;
	}
	if(j != 20) {
		DBG_printf(("ERROR: BKSV read - Key Wrong\r\n"));
		DBG_printf(("ERROR: BKSV=0x%02X,0x%02X,0x%02X,0x%02X,0x%02X\r\n", (unsigned int)pBKSV[0], (unsigned int)pBKSV[1], (unsigned int)pBKSV[2], (unsigned int)pBKSV[3], (unsigned int)pBKSV[4]));
		return 0;
	}
	return 1;
}

unsigned char Downstream_Rx_BCAPS(void)
{
	DDC_Read(HDCP_RX_ADDR, HDCP_RX_BCAPS_ADDR, DDC_Data, 1);
	return DDC_Data[0];
}

void Downstream_Rx_write_AINFO(char ainfo)
{
	DDC_Write(HDCP_RX_ADDR, HDCP_RX_AINFO_ADDR, &ainfo, 1);
}

void Downstream_Rx_write_AN(unsigned char *pAN)
{
	DDC_Write(HDCP_RX_ADDR, HDCP_RX_AN_ADDR, pAN, 8);
}

void Downstream_Rx_write_AKSV(unsigned char *pAKSV)
{
	DDC_Write(HDCP_RX_ADDR, HDCP_RX_AKSV_ADDR, pAKSV, 5);
}

unsigned char Downstream_Rx_read_RI(unsigned char *pRI)
{
	/* Short Read  */
	status = DDC_Read(HDCP_RX_ADDR, HDCP_RX_RI_ADDR, pRI, 2);
	if(status != SMBUS_STATUS_Success) {
		DBG_printf(("ERROR: Rx Ri read - MCU IIC %d\r\n", (int)status));
		return 0;
	}
	return 1;
}

void Downstream_Rx_read_BSTATUS(unsigned char *pBSTATUS)
{
	DDC_Read(HDCP_RX_ADDR, HDCP_RX_BSTATUS_ADDR, pBSTATUS, 2);
}

void Downstream_Rx_read_SHA1_HASH(unsigned char *pSHA)
{
#if 0/* 5310 can't support burst 128 stanley*/

	DDC_Read(HDCP_RX_ADDR, HDCP_RX_SHA1_HASH_ADDR, pSHA, 20);
#else
DDC_Read(HDCP_RX_ADDR, HDCP_RX_SHA1_HASH_ADDR, pSHA, 8);
DDC_Read(HDCP_RX_ADDR, HDCP_RX_SHA1_HASH_ADDR+8, pSHA, 8);
DDC_Read(HDCP_RX_ADDR, HDCP_RX_SHA1_HASH_ADDR+8, pSHA, 4);



#endif

}

/* Retrive a 5 byte KSV at "Index" from FIFO  */
unsigned char Downstream_Rx_read_KSV_FIFO(unsigned char *pBKSV, unsigned char Index, unsigned char DevCount)
{
	int i, j;

	/* Try not to re-read the previous KSV  */
	if(Index == 0) { /* Start  */
		/* Support a max 25 device count because of DDC_Data[] size is 128 byte  */
		status = DDC_Read(HDCP_RX_ADDR, HDCP_RX_KSV_FIFO_ADDR, DDC_Data, min(DevCount, 25));
	}
	memcpy(pBKSV, DDC_Data+(Index*5), 5);

	if(status != SMBUS_STATUS_Success) {
		DBG_printf(("ERROR: KSV FIFO read - DN DDC %d\r\n", (int)status));
		return 0;
	}

	i = 0;
	j = 0;
	while (i < 5) {
		TempBit = 1;
		while (TempBit) {
			if (pBKSV[i] & TempBit) j++;
			TempBit <<= 1;
		}
		i++;
	}
	if(j != 20) {
		DBG_printf(("ERROR: KSV FIFO read - Key Wrong\r\n"));
		return 0;
	}
	return 1;
}


/*--------------------------------------------------------------------------------------------------*/
/* */
/* Downstream EDID Control */
/*  */

unsigned char Downstream_Rx_poll_EDID(void)
{
	/* Read the EDID test */

	/*                                           */
	/* How to implement this with CSI2C ?????????????????????????????  */
	/* Without the Segment address implementation, it works normally. */
	/* But, this must be implemented for ATC test. */
	/*DDC_Data[0] = 0;	 Segment Pointer Address */
	/*SMBUS_master_rw_synchronous(DDC_Bus, EDID_SEGMENT_PTR, DDC_Data, 1, SMBUS_SkipStop);*/
	/*   */

	/* Base Address and Read 1*/
	status = DDC_Read(EDID_ADDR, 0, DDC_Data, 1);

	if(status != SMBUS_STATUS_Success) /* can't read EDID */
	{
		return 2;
	}
	if(DDC_Data[0] != 0x00)				/* EDID header fail  */
	{
		return 2;
	}
	return 0;							/* Read EDID success  */

}

EDID_STATUS Downstream_Rx_read_EDID(unsigned char *pEDID)
{
	int i;
	unsigned char seg_ptr, BlockCount, Block1Found, ChkSum/*, sys_ram[2]*/;

	/* =========================================================*/
	/* I. Read the block 0 */

	/*=========================================================*/
	/* How to implement this with CSI2C ?????????????????????????????  */
	/* Without the Segment address implementation, it works normally.  */
	/* But, this must be implemented for ATC test. */
	/*DDC_Data[0] = 0;	 Segment Pointer Address  */
	/*SMBUS_master_rw_synchronous(DDC_Bus, EDID_SEGMENT_PTR, DDC_Data, 1, SMBUS_SkipStop); */
	/*=========================================================*/

	/* Base Address and Read 128  */
	/*sys_ram[0] = 0;  */
	/*DDC_NoStop(EDID_SEGMENT_PTR, 0, sys_ram, 1);*/	/* skip stop */
#if 1 /* stanley 5310 can 't support burst read over 16 byte*/
	status = DDC_Read(EDID_ADDR, 0, pEDID, 128);
#else
status = DDC_Read(EDID_ADDR, 0    , pEDID    , 8);
status = DDC_Read(EDID_ADDR, 0+8  , pEDID+8  , 8);
status = DDC_Read(EDID_ADDR, 0+16 , pEDID+16 , 8);
status = DDC_Read(EDID_ADDR, 0+24 , pEDID+24 , 8);
status = DDC_Read(EDID_ADDR, 0+32 , pEDID+32 , 8);
status = DDC_Read(EDID_ADDR, 0+40 , pEDID+40 , 8);
status = DDC_Read(EDID_ADDR, 0+48 , pEDID+48 , 8);
status = DDC_Read(EDID_ADDR, 0+56 , pEDID+56 , 8);
status = DDC_Read(EDID_ADDR, 0+64 , pEDID+64 , 8);
status = DDC_Read(EDID_ADDR, 0+72 , pEDID+72 , 8);
status = DDC_Read(EDID_ADDR, 0+80 , pEDID+80 , 8);
status = DDC_Read(EDID_ADDR, 0+88 , pEDID+88 , 8);
status = DDC_Read(EDID_ADDR, 0+96 , pEDID+96 , 8);
status = DDC_Read(EDID_ADDR, 0+104, pEDID+104, 8);
status = DDC_Read(EDID_ADDR, 0+112, pEDID+112, 8);
status = DDC_Read(EDID_ADDR, 0+120, pEDID+120, 8);





#endif



	if(status != SMBUS_STATUS_Success) {
		DBG_printf(("ERROR: EDID b0 read - DN DDC %d\r\n", (int)status));
		return status;
	}
	DBG_printf(("EDID b0 read:"));
	for(i=0; i<128; ++i) {
		if(i%16 == 0) DBG_printf(("\r\n"));
		if(i%8 == 0) DBG_printf((" "));
		DBG_printf(("0x%02X, ", (int)pEDID[i] ));
	}
	DBG_printf(("\r\n"));

	if( (pEDID[0] != 0x00) ||
	    (pEDID[1] != 0xFF) ||
	    (pEDID[2] != 0xFF) ||
	    (pEDID[3] != 0xFF) ||
	    (pEDID[4] != 0xFF) ||
	    (pEDID[5] != 0xFF) ||
	    (pEDID[5] != 0xFF) ||
	    (pEDID[7] != 0x00))
	{
		/*return EDID_STATUS_NoAct;  */
	}

	/* Check EDID */
	if(pEDID[126] > 8) {
		DBG_printf(("ERROR: EDID Check failed, pEDID[126]=0x%02X > 8\n\r", (int)pEDID[126] ));
		return EDID_STATUS_ExtensionOverflow;
	}

	/* =========================================================*/
	/* II. Read other blocks and find Timing Extension Block  */

	BlockCount = pEDID[126];
	Block1Found = 0;
	for (seg_ptr = 1; seg_ptr <= BlockCount; ++seg_ptr) {

		/*================================================================================================*/
		/* How to implement this with Customer's I2C ????????????????????????????? */
		/* Without the Segment address implementation, it works normally. */
		/* But, this must be implemented for ATC test.  */
		/*DDC_Data[0] = seg_ptr >> 1;   */	/* Segment Pointer Address */
		/*SMBUS_master_rw_synchronous(DDC_Bus, EDID_SEGMENT_PTR, DDC_Data, 1, SMBUS_SkipStop); */
		/*================================================================================================*/

		/* Base Address and Read 128 */
		/*sys_ram[0] = seg_ptr >> 1; */	/* Segment Pointer Address */
		/*DDC_NoStop(EDID_SEGMENT_PTR, 0, sys_ram, 1);	*/  /* skip stop  */
#if 1 /* 5310 hw can't support burst 128*/
		status = DDC_Read(EDID_ADDR, (seg_ptr & 0x01) << 7, DDC_Data, 128);
#else

status = DDC_Read(EDID_ADDR, ((seg_ptr & 0x01) << 7)	, DDC_Data	  , 128);
status = DDC_Read(EDID_ADDR, ((seg_ptr & 0x01) << 7)+8	, DDC_Data+8  , 128);
status = DDC_Read(EDID_ADDR, ((seg_ptr & 0x01) << 7)+16 , DDC_Data+16 , 128);
status = DDC_Read(EDID_ADDR, ((seg_ptr & 0x01) << 7)+24 , DDC_Data+24 , 128);
status = DDC_Read(EDID_ADDR, ((seg_ptr & 0x01) << 7)+32 , DDC_Data+32 , 128);
status = DDC_Read(EDID_ADDR, ((seg_ptr & 0x01) << 7)+40 , DDC_Data+40 , 128);
status = DDC_Read(EDID_ADDR, ((seg_ptr & 0x01) << 7)+48 , DDC_Data+48 , 128);
status = DDC_Read(EDID_ADDR, ((seg_ptr & 0x01) << 7)+56 , DDC_Data+56 , 128);
status = DDC_Read(EDID_ADDR, ((seg_ptr & 0x01) << 7)+64 , DDC_Data+64 , 128);
status = DDC_Read(EDID_ADDR, ((seg_ptr & 0x01) << 7)+72 , DDC_Data+72 , 128);
status = DDC_Read(EDID_ADDR, ((seg_ptr & 0x01) << 7)+80 , DDC_Data+80 , 128);
status = DDC_Read(EDID_ADDR, ((seg_ptr & 0x01) << 7)+88 , DDC_Data+88 , 128);
status = DDC_Read(EDID_ADDR, ((seg_ptr & 0x01) << 7)+96 , DDC_Data+96 , 128);
status = DDC_Read(EDID_ADDR, ((seg_ptr & 0x01) << 7)+104, DDC_Data+104, 128);
status = DDC_Read(EDID_ADDR, ((seg_ptr & 0x01) << 7)+112, DDC_Data+112, 128);
status = DDC_Read(EDID_ADDR, ((seg_ptr & 0x01) << 7)+120, DDC_Data+120, 128);



#endif



		if(status != SMBUS_STATUS_Success) {
			DBG_printf(("ERROR: EDID bi read - DN DDC %d\r\n", (int)status));
			return status;
		}

		if(DDC_Data[0] == 0x02 && Block1Found == 0) {
			Block1Found = 1;
			memcpy(&pEDID[128], DDC_Data, 128);
		}

		DBG_printf(("EDID b%d read:", (int)seg_ptr));
		for(i=0; i<128; ++i) {
			if(i%16 == 0) DBG_printf(("\r\n"));
			if(i%8 == 0) DBG_printf((" "));
			DBG_printf(("0x%02X, ", (int)DDC_Data[i] ));
		}
		DBG_printf(("\r\n"));
	}

	/* Check CheckSum */
	ChkSum = 0;
	for(i=0; i<((Block1Found)?256:128); ++i) {
		ChkSum += pEDID[i];
	}
	if(ChkSum != 0) {
		return EDID_STATUS_ChecksumError;
	}
	if(Block1Found) {
		pEDID[126] = 1;
	}
	else {
		pEDID[126] = 0;
	}
	return EDID_STATUS_Success;
}

/*==================================================================================================*/
/*    */
/* Private Functions  */
/*==================================================================================================*/

SMBUS_STATUS DDC_Write(unsigned char IICAddr, unsigned char ByteAddr, void *Data, unsigned int Size)
{
	/*                                                                              */
	/* How to implement this with Customer's I2C ?????????????????????????????      */
	/* return 0; for success              */
	/* return 2; for No_ACK               */
	/* return 4; for Arbitration          */

/* printf ("DDC write  size =%x\n",Size);*/
	int result = 1;
	UINT8 i;

	/* need to use customer's I2C  function  */
	/*result = I2C_WriteReg_EP932M(IICAddr, ByteAddr, Data, Size);  */

	UINT8 regaddr[1];
	/*UINT8 regdata[16];*/
	UINT8 tmp;

	regaddr[0]= ByteAddr;
	/*regdata = Data;  */
	printf(" IICAddr f= %x, ByteAddr= %x, Data= %p , Size=%x \n",IICAddr,ByteAddr,Data,Size);
	WRITE8(0xb0009035, (READ8(0xb0009035)& 0xCF));/* i2c select function pins*/
	WRITE8(0xb0009148,0x1);/* clear i2c status*/
    WRITE8(0xb0009140, IICAddr);


	if(Size <=8)
	{
    	HDMI_i2c_write(regaddr, Data,Size-1, 0, 0);
	}
	else
	{
		for(i=0;i<=Size ; i=i+8)
			{

			HDMI_i2c_write(regaddr+i, Data+i,8-1, 0, 0);
			}
		i=i-8;
			if (i<Size)
				{
			HDMI_i2c_write(regaddr+i, Data+i,(Size-i)-1, 0, 0);
				printf(" ******************** i=%d size=%d \n",i, Size);
				}
	}


	tmp =READ8(0xb0009148);
	if ( tmp==0x2)
		{result =2;
		WRITE8(0xb0009148,0);
		}
	else
		{result =0x0;		}



	return result;
}

SMBUS_STATUS DDC_Read(unsigned char IICAddr, unsigned char ByteAddr, void *Data, unsigned int Size)
{
	/*===============================================================================================*/
	/* How to implement this with CSI2C ?????????????????????????????  */
	/* return 0; for success   */
	/* return 2; for No_ACK     */
	/* return 4; for Arbitration  */
	/*=================================================================================================*/
	int result = 1;
	UINT8 i;
	UINT8 tmp;
/*printf( "ddc-addr= %x\n",ByteAddr); */
/*	printf ("DDC read	size =%x\n",Size); */

	/* need to use customer's I2C  function  */
	/*result = I2C_ReadReg_EP932M(IICAddr, ByteAddr, Data, Size);  */
	WRITE8(0xb0009035, (READ8(0xb0009035)& 0xCF));/* i2c select function pins*/
	WRITE8(0xb0009148,0x1);/* clear i2c status*/
    WRITE8(0xb0009140, IICAddr);

	if(Size <=8)
	{
    	HDMI_i2c_read(ByteAddr, Data,Size-1 , 4, 0);
	}
	else
	{
		for(i=0;i<=Size ; i=i+8)
			{

			HDMI_i2c_read(ByteAddr+i, Data+i,8-1 , 4, 0);

			}
		i=i-8;
			if (i<Size)
			{HDMI_i2c_read(ByteAddr+i, Data+i,(Size-i)-1 , 4, 0);

			printf(" HDMI_i2c_read******************** i=%d  size=%d \n",i, Size);
				}
	}

	tmp =READ8(0xb0009148);
	if ( tmp==0x2)
		{result =2;
		WRITE8(0xb0009148,0);
		}
	else
		{result =0x0;		}


	return result;
}

#if 0
SMBUS_STATUS DDC_NoStop(unsigned char IICAddr, unsigned char ByteAddr, void *Data, unsigned int Size)
{
	/*                                                  */
	/* How to implement this with CSI2C ????????????????????????????? */
	/* return 0; for success  */
	/* return 2; for No_ACK  */
	/* return 4; for Arbitration  */
	/*                                                  */
	int result = 1;

	/* need to use customer's I2C  function  */
	/*result = I2C_ReadReg_EP932M(IICAddr, ByteAddr, Data, Size);  */

	return result;
}
#endif

void
HDMI_i2c_write(
	UINT8 *pregaddr,
	UINT8 *pregdata,
	UINT8 cnt,
	UINT8 mode,
	UINT8 sync
)
{
	UINT32 i;

	WRITE8(0xb0009100, mode); /* Select Sequential/Burst mode */
	WRITE8(0xb0009108, sync); /* Synchronize with Vd or Vvalid not */
	WRITE8(0xb000913C, cnt);


	if (mode) {
		for (i = 0; i < (UINT32)(cnt+1); i++) {
			if ((READ8(0xb0009138) & 0x30) == 0x30) {
				WRITE8(0xb0009110+i*4, pregaddr[i*2]);
				WRITE8(0xb0009110+i*4+2, pregaddr[i*2+1]);
			}
			else {
				WRITE8(0xb0009110+i*2, pregaddr[i]);
			}
			WRITE8(0xb0009110+i*2+1, pregdata[i]);
		}
	}
	else {

		WRITE8(0xb0009110, pregaddr[0]);
		WRITE8(0xb0009112, pregaddr[1]);
		for (i = 0; i < (UINT32)(cnt+1); i++) {
			WRITE8(0xb0009110+i*2+1, pregdata[i]);

		}
	}
    WRITE8(0xb0009104, 0x01);

	do {
		i = READ8(0xb0009104);

	} while (i == 0x01);

}

/**************************************************************************************
*                                                                                     *
*  Function Name : halFrontSsc2Read                                                     *
*                                                                                     *
*  Purposes      :                                                                    *
*    To Read data through SSC interface                                               *
*                                                                                     *
*  Desprictions  :                                                                    *
*    Read data from sensor through SSC interface                                      *
*                                                                                     *
*  Arguments     :                                                                    *
*    regaddr  : in, register address of sensor                                        *
*    pregdata : out, point of register data of sensor                                 *
*    cnt      : in, the number of data to Read                                        *
*    mode     : 2: normal read, 3: combine read, 4: combine read with restart         *
*    sync     : in, 0: immediate, 1: sync vdorg, 2: sync vvalid                       *
*                                                                                     *
*  Returns       :                                                                    *
*    None                                                                             *
*                                                                                     *
*  See also      :                                                                    *
*                                                                                     *
***************************************************************************************/

void
HDMI_i2c_read(

		UINT8 regaddr,
		UINT8  *pregdata,
		UINT8  cnt,
		UINT8  mode,
		UINT8  sync
	)
{
	UINT32 tmp0, i;

		/*UINT32 status=0;*/

		do tmp0 = (*((volatile UINT8*)(0xb0009104)));
		while (tmp0 == 0x01);						  /* Wait SSC Busy */

	/*	printf("I2C read function start\n");*/
		*(volatile UINT8*)(0xb0009100) = mode;
		*(volatile UINT8*)(0xb0009108) = sync;
		*(volatile UINT8*)(0xb000913c) = cnt;
		*(volatile UINT8*)(0xb0009110) = regaddr & 0xFF;
		*(volatile UINT8*)(0xb0009112) = regaddr >> 8;
		*(volatile UINT8*)(0xb0009104) = 0x01;

		do tmp0 = (*((volatile UINT8*)(0xb0009104)));
		while (tmp0 == 0x01);						  /* Wait SSC Busy */

		for (i = 0; i < (UINT32)(cnt+1); i++)
				pregdata[i] = (*((volatile UINT8*)(0xb0009110+i*2+1)));




}

/*/* EDID----------------------------------------------------------------------------------------------*/

unsigned char EDID_GetHDMICap(unsigned char *pTarget)
{
	if(pTarget[126] == 0x01) {
		for(edid_i=4; edid_i<pTarget[EDID_BLOCK_SIZE+2]; ++edid_i) {
			if((pTarget[EDID_BLOCK_SIZE+edid_i] & 0xE0) == 0x60) { /* find tag code - Vendor Specific Block  */
				if( (pTarget[EDID_BLOCK_SIZE+1+edid_i] == 0x03) && (pTarget[EDID_BLOCK_SIZE+2+edid_i] == 0x0C) && (pTarget[EDID_BLOCK_SIZE+3+edid_i] == 0x00) ) {
					return 1;
				}
			}
			else {
				edid_i += (pTarget[EDID_BLOCK_SIZE+edid_i] & 0x1F);
			}
		}
		if(edid_i>=pTarget[EDID_BLOCK_SIZE+2]) { /* Error, can not find the Vendor Specific Block  */
			return 0;
		}
	}
	return 0;
}

unsigned char EDID_GetPCMFreqCap(unsigned char *pTarget)
{
	if(pTarget[126] >= 0x01) {
		for(edid_i=4; edid_i<pTarget[EDID_BLOCK_SIZE+2]; ++edid_i) {
			if((pTarget[EDID_BLOCK_SIZE+edid_i] & 0xE0) == 0x20) { /* find tag code - Audio Data Block  */
				for(edid_j=1; edid_j<(pTarget[EDID_BLOCK_SIZE+edid_i] & 0x1F); edid_j+=3) {
					if((pTarget[EDID_BLOCK_SIZE+edid_i+edid_j] >> 3) == 1) {
						return pTarget[EDID_BLOCK_SIZE+edid_i+edid_j+1];
					}
				}
			}
			else {
				edid_i += (pTarget[EDID_BLOCK_SIZE+edid_i] & 0x1F);
			}
		}
		if(edid_i>=pTarget[EDID_BLOCK_SIZE+2]) { /* Error, can not find the Audio Data Block  */
			return 0x07;
		}
	}
	return 0x00;
}

unsigned char EDID_GetPCMChannelCap(unsigned char *pTarget)
{
	unsigned char MaxPCMChannel = 1;
	if(pTarget[126] >= 0x01) {
		for(edid_i=4; edid_i<pTarget[EDID_BLOCK_SIZE+2]; ++edid_i) {
			if((pTarget[EDID_BLOCK_SIZE+edid_i] & 0xE0) == 0x20) { /* find tag code - Audio Data Block  */
				for(edid_j=1; edid_j<(pTarget[EDID_BLOCK_SIZE+edid_i] & 0x1F); edid_j+=3) {
					if((pTarget[EDID_BLOCK_SIZE+edid_i+edid_j] >> 3) == 1) {
						/*return pTarget[EDID_BLOCK_SIZE+i+edid_j] & 0x07;  */
						MaxPCMChannel = max(MaxPCMChannel, pTarget[EDID_BLOCK_SIZE+edid_i+edid_j] & 0x07);
					}
				}
			}
			else {
				edid_i += (pTarget[EDID_BLOCK_SIZE+edid_i] & 0x1F);
			}
		}
		return MaxPCMChannel;
		/*if(i>=pTarget[EDID_BLOCK_SIZE+2]) { *//* Error, can not find the Audio Data Block  */
		/*	return 1;  */
		/* }  */
	}
	return 0;
}

unsigned char EDID_GetDataBlockAddr(unsigned char *pTarget, unsigned char Tag)
{
	if(pTarget[126] >= 0x01) {
		for(edid_i=4; edid_i<pTarget[EDID_BLOCK_SIZE+2]; ++edid_i) {
			if((pTarget[EDID_BLOCK_SIZE+edid_i] & 0xE0) == Tag) { /* find tag code  */
				return edid_i+128;
			}
			else {
				edid_i += (pTarget[EDID_BLOCK_SIZE+edid_i] & 0x1F);
			}
		}
		if(edid_i>=pTarget[EDID_BLOCK_SIZE+2]) { /* Error, can not find */
			return 0;
		}
	}
	return 0;
}

/*-------------------------------------------------------------------------------------------------------------------------*/

void HDCP_Extract_BKSV_BCAPS3(unsigned char *pBKSV_BCaps3)
{
	pBKSV_BCAPS3 = pBKSV_BCaps3;
}

void HDCP_Extract_FIFO(unsigned char *pFIFOList, unsigned char ListNumber)
{
	pKSVList = pFIFOList;
	KSVListNumber = ListNumber;
}

void HDCP_Extract_SHA_M0(unsigned char *pSha_M0)
{
	pSHA_M0 = pSha_M0;
}

void HDCP_Stop()
{
	HDCP_TimeCount = 1000/HDCP_TIMER_PERIOD; /* No delay for next startup */
	HDCP_Status = 0;
	HDCP_State = 0;
	RI_Check = 0;

	/* Disable the HDCP Engine */
	HDMI_Tx_HDCP_Disable();

	/* Disable mute for transmiter video and audio */
	HDMI_Tx_Mute_Disable();
}

void HDCP_Fake(unsigned char Enable)
{
	Fake_HDCP = Enable;
}

unsigned char HDCP_Get_Status(void)
{
	return HDCP_Status;
}

void HDCP_Timer(void)
{
	++HDCP_TimeCount;
}

void HDCP_Ext_Ri_Trigger(void)
{
	RI_Check = 1;
}

HDCP_STATE HDCP_Authentication_Task(unsigned char ReceiverRdy)
{
	if( (HDCP_State > A0_Wait_for_Active_Rx) && !(ReceiverRdy) ) {
		DBG_printf(("WARNING: No RSEN or Hot-Plug in Authentication\r\n"));

		/* Enable mute for transmiter video and audio */
		HDMI_Tx_Mute_Enable();

		/* Confirm, Disable the HDCP Engine (actived from Downstream)  */
		HDMI_Tx_HDCP_Disable();

		/* Restart HDCP authentication  */
		HDCP_TimeCount = 0;
		HDCP_State = A0_Wait_for_Active_Rx;
		HDCP_Status |= HDCP_ERROR_RSEN;
		return HDCP_State;
	}

	switch(HDCP_State) {
	/* HDCP authentication  */

		/* A0 state -- Wait for Active RX  */
		/*          -- read and validate BKSV for every 1 second.  */
		case A0_Wait_for_Active_Rx:

 	    	if(HDCP_TimeCount > 1000/HDCP_TIMER_PERIOD) {
				HDCP_TimeCount = 0;

				if( Downstream_Rx_read_BKSV(Temp_HDCP) ) {
					DBG_printf(("Authentication start...\r\n"));
		   			HDCP_State = A1_Exchange_KSVs;
				}
				else {	/* HDCP not support  */
					DBG_printf(("HDCP might not be supported\r\n"));
					Downstream_Rx_write_AINFO(0x00); /* Make TE sense the retry to pass ATC HDCP test  */
					HDCP_Status |= HDCP_ERROR_BKSV;
				}
			}
			break;

		/* A1 state -- Exchange KSVs  */
		/*          -- retrieve the random number  */
		/*          -- write AN to RX and TX  */
		/*          -- read and write AKSV, BKSV  */
		case A1_Exchange_KSVs:

			/* Write AINFO  */
			Downstream_Rx_write_AINFO(0x00);

			/* Check Repeater Bit  */
			Temp_HDCP[0] = Downstream_Rx_BCAPS();
			if(Temp_HDCP[0] & 0x40) {		/* REPEATER  */
				HDMI_Tx_RPTR_Set();
			}
			else {						/* NON-REPEATER  */
				HDMI_Tx_RPTR_Clear();
			}

			/* Exange AN  */
			for(Temp_HDCP[8]=0; Temp_HDCP[8]<8; ++Temp_HDCP[8]) {
				Temp_HDCP[Temp_HDCP[8]] = rand()%256;
			}
		    HDMI_Tx_write_AN(Temp_HDCP);
		    Downstream_Rx_write_AN(Temp_HDCP);

			/* Exange AKSV  */
		    if(!HDMI_Tx_read_AKSV(Temp_HDCP)) {

				if(!Fake_HDCP) {
					HDCP_State = A0_Wait_for_Active_Rx;
					HDCP_Status |= HDCP_ERROR_AKSV;
					break;
				}
				else {
					memset(Temp_HDCP, 0x5A, 5);
				}
			}
		    Downstream_Rx_write_AKSV(Temp_HDCP);

			/* Exange BKSV  */
		    if(!Downstream_Rx_read_BKSV(Temp_HDCP)) {
				HDCP_State = A0_Wait_for_Active_Rx;
				HDCP_Status |= HDCP_ERROR_BKSV;
				break;
			}
		   	HDMI_Tx_write_BKSV(Temp_HDCP);
			if(pBKSV_BCAPS3) memcpy(&pBKSV_BCAPS3[0], Temp_HDCP, 5);

		    HDCP_State = A2_Computations;
			break;

		/* A2 state -- Computations  */
		/*          -- Wait 150ms for R0 update (min 100ms)  */
		case A2_Computations:

		 	if(HDCP_TimeCount > 150/HDCP_TIMER_PERIOD) {
				if(HDMI_Tx_RI_RDY()) {
					HDCP_TimeCount = 0;
					HDCP_State = A3_Validate_Receiver;
				}
			}
			break;

		/* A3 state -- Validate Receiver  */
		/*          -- read and compare R0 from TX and RX  */
  		/*          -- allow IIC traffic or R0 compare error in 200ms  */
		case A3_Validate_Receiver:

			if(!HDCP_validate_RI()) {
	 	    	if(HDCP_TimeCount > 200/HDCP_TIMER_PERIOD) {
					HDCP_TimeCount = 0;

					DBG_printf(("ERROR: R0 check failed\r\n"));

					HDCP_State = A0_Wait_for_Active_Rx;
					HDCP_Status |= HDCP_ERROR_R0;
				}
			}
			else {
				HDCP_TimeCount = 0;
				HDCP_State = A6_Test_for_Repeater;
			}
			break;

		/* A4 state -- Authenticated  */
		/*          -- Disable mute   */
		case A4_Authenticated:

			/* Start the HDCP Engine  */
			if(!Fake_HDCP) HDMI_Tx_HDCP_Enable();

			/* Disable mute for transmiter video  */
			HDMI_Tx_Mute_Disable();

			DBG_printf(("Authenticated\r\n"));

			HDCP_State = A5_Link_Integrity_Check;
			break;

		/* A5 state -- Link Integrity Check every second */
		/*          -- HDCP Engine must be started  */
  		/*          -- read and compare RI from RX and TX  */
		case A5_Link_Integrity_Check:

			if(RI_Check) {
				RI_Check = 0;

				if(!HDCP_validate_RI()) {
					if(!HDCP_validate_RI()) {

						/* Enable mute for transmiter video and audio */
						HDMI_Tx_Mute_Enable();

						/* Disable the HDCP Engine */
						HDMI_Tx_HDCP_Disable();

						DBG_printf(("ERROR: Ri check failed\r\n"));

						HDCP_State = A0_Wait_for_Active_Rx;
						HDCP_Status |= HDCP_ERROR_Ri;
					}
				}
			}
#if 0
 	    	if(HDCP_TimeCount > 2000/HDCP_TIMER_PERIOD) {	/* Wait for 2 second */
				HDCP_TimeCount = 0;

				if(!HDCP_validate_RI()) {

					if(RI_Check) { /* RI_Failed_Two */

						/* Enable mute for transmiter video and audio */
						HDMI_Tx_Mute_Enable();

						/* Disable the HDCP Engine */
						HDMI_Tx_HDCP_Disable();

						DBG_printf(("ERROR: Ri check failed\r\n"));

						HDCP_State = A0_Wait_for_Active_Rx;
						HDCP_Status |= HDCP_ERROR_Ri;
					}
					else {
						DBG_printf(("WARNING: Ri check failed\r\n"));

						RI_Check = 1;
						HDCP_TimeCount = 1500/HDCP_TIMER_PERIOD;
					}
				}
				else {
					RI_Check = 0;
				}
			}
#endif
			break;

		/* A6 state -- Test For Repeater  */
		/*          -- REPEATER     : Enter the WAIT_RX_RDY state; */
		/*          -- NON-REPEATER : Enter the AUTHENTICATED state  */
		case A6_Test_for_Repeater:

			Temp_HDCP[0] = Downstream_Rx_BCAPS();
			if(pBKSV_BCAPS3) pBKSV_BCAPS3[5] = Temp_HDCP[0];

			if (Temp_HDCP[0] & 0x40) {	/* REPEATER	 */
				HDCP_State = A8_Wait_for_READY;
			}
			else {						/* NON-REPEATER */
				HDCP_State = A4_Authenticated;
			}
			break;

		/* A8 state -- Wait for READY  */
		/*          -- read BCAPS and check READY bit continuously */
		/*          -- time out while 5-second period exceeds */
		case A8_Wait_for_READY:

			Temp_HDCP[0] = Downstream_Rx_BCAPS();
			if(pBKSV_BCAPS3) pBKSV_BCAPS3[5] = Temp_HDCP[0];

			if (Temp_HDCP[0] & 0x20) {
				HDCP_TimeCount = 0;
				HDCP_State = A9_Read_KSV_List;
			}
			else {
				if(HDCP_TimeCount > 5000/HDCP_TIMER_PERIOD) {
					HDCP_TimeCount = 0;

					DBG_printf(("ERROR: Repeater check READY bit time-out\r\n"));

					HDCP_State = A0_Wait_for_Active_Rx;
					HDCP_Status |= HDCP_ERROR_RepeaterRdy;
				}
			}
			break;

		/* A9 state -- Read KSV List */
		/*          -- compute and validate SHA-1 values */
		case A9_Read_KSV_List:

			Downstream_Rx_read_BSTATUS(Temp_HDCP);
			if(pBKSV_BCAPS3) memcpy(&pBKSV_BCAPS3[6], Temp_HDCP, 2);

			if(!(Temp_HDCP[0] & 0x80) && !(Temp_HDCP[1] & 0x08)) {
				if(HDCP_compute_SHA_message_digest(Temp_HDCP[0], Temp_HDCP[1])) {
					HDCP_State = A4_Authenticated;
					break;
				}
				else {
					HDCP_Status |= HDCP_ERROR_RepeaterSHA;
				}
			}
			else {
				HDCP_Status |= HDCP_ERROR_RepeaterMax;
			}

			DBG_printf(("ERROR: Repeater HDCP SHA check failed\r\n"));

			HDCP_State = A0_Wait_for_Active_Rx;
			break;
	}

	return HDCP_State;
}

/*-------------------------------------------------------------------------------------------------------------------*/

unsigned char HDCP_validate_RI(void)
{
	unsigned short Temp_RI_Tx, Temp_RI_Rx;
	if(!HDMI_Tx_read_RI((unsigned char *)&Temp_RI_Tx)) return 0;		/* Read form Tx is fast, do it first  */
	if(!Downstream_Rx_read_RI((unsigned char *)&Temp_RI_Rx)) return 0;	/* Read form Rx is slow, do it second  */
/*	if(Temp_RI_Tx != Temp_RI_Rx) DBG_printf(("RI_Tx=0x%0.4X, RI_Rx=0x%0.4X\r\n", (int)Temp_RI_Tx, (int)Temp_RI_Rx)); */
	if(Fake_HDCP) return 1;
	return (Temp_RI_Tx == Temp_RI_Rx);
}

/*--------------------------------------------------------------------------------------------------*/

/*  */
/* NOTE : The following SHA calculation subroutine has not been verified in the   */
/*        real environment. It has been evaluated by some debugging procedure.    */
/*        The auther is not responsible to ensure the functionality.              */
/*                                                                                */
unsigned char HDCP_compute_SHA_message_digest(unsigned char hdcp_dev_count, unsigned char hdcp_depth)
{
	int i;
	unsigned long *SHA_H;

	/*===============================================================================================*/
	/* Calculate SHA Value */
	/*                     */

	SHA_Initial();

	/*                       */
	/* Step 1                   */
	/* Push all KSV FIFO to SHA caculation  */
	/*                            */

	/* Read KSV (5 byte) one by one and check the revocation list  */
	for(i=0; i<hdcp_dev_count; ++i) {

		/* Get KSV from FIFO  */
		if(!Downstream_Rx_read_KSV_FIFO(Temp_HDCP, i, hdcp_dev_count)) {
			return 0;
		}

		/* Save FIFO  */
		if(pKSVList && KSVListNumber) {
			if(i < KSVListNumber) memcpy(pKSVList+(i*5), Temp_HDCP, 5);
		}

		/* Push KSV to the SHA block buffer (Total 5 bytes)  */
		SHA_Push_Data(Temp_HDCP, 5);
	}
	if(hdcp_dev_count == 0) {
		Downstream_Rx_read_KSV_FIFO(Temp_HDCP, 0, 1);
	}

	/*                             */
	/* Step 2                       */
	/* Push BSTATUS, M0, and EOF to SHA caculation  */
	/*                                */

	/* Get the BSTATUS, M0, and EOF  */
	Temp_HDCP[0] = hdcp_dev_count;	/* Temp_HDCP[0]   = BStatus, LSB  */
	Temp_HDCP[1] = hdcp_depth;		/* Temp_HDCP[1]   = BStatus, MSB  */
	HDMI_Tx_read_M0(Temp_HDCP+2);	/* Temp_HDCP[2:9] = Read M0 from TX  */
	if(pSHA_M0) memcpy(pSHA_M0+20, (unsigned char*)Temp_HDCP+2, 8);

	/* Push the BSTATUS, and M0 to the SHA block buffer (Total 10 bytes)  */
	SHA_Push_Data(Temp_HDCP, 10);

	/*      */
	/* Step 3  */
	/* Push the final block with length to SHA caculation  */
	/*                          */

	SHA_H = SHA_Get_SHA_Digest();

	/*  */
	/* SHA complete  */
	/*====================================================================================*/


	/*=======================================================================================*/
	/* Compare the SHA value */
	/*                       */

	/* read RX SHA value     */
	Downstream_Rx_read_SHA1_HASH((unsigned char*)Temp_SHA_H);
	if(pSHA_M0) memcpy(pSHA_M0, (unsigned char*)Temp_SHA_H, 20);
	DBG_printf(("Rx_SHA_H: "));
#ifdef DBG
	for(i=0; i<20; i+=4) {
		DBG_printf(("0x%0.2X%0.2X%0.2X%0.2X ", (int)(((PBYTE)Temp_SHA_H)[i+3]), (int)(((PBYTE)Temp_SHA_H)[i+2]), (int)(((PBYTE)Temp_SHA_H)[i+1]), (int)(((PBYTE)Temp_SHA_H)[i+0])));
	}
	DBG_printf(("\r\n"));
#endif
	/* compare the TX/RX SHA value  */
	if( (hdcp_dev_count & 0x80) || (hdcp_depth & 0x08) ) {
		DBG_printf(("Max Cascade or Max Devs exceeded\r\n"));
		return 0;
	}
	else if( (SHA_H[0] != Temp_SHA_H[0]) || (SHA_H[1] != Temp_SHA_H[1]) || (SHA_H[2] != Temp_SHA_H[2]) || (SHA_H[3] != Temp_SHA_H[3]) || (SHA_H[4] != Temp_SHA_H[4]) ) {
		DBG_printf(("SHA Digit Unmatch\r\n"));
  		return 0;
	}
	else {
		DBG_printf(("SHA Digit Match\r\n"));
		return 1;
	}

	/*            */
	/* Return the compared result  */
	/*===============================================================*/

}

/*------------------------------------------------------------------------------------------------*/
/* SHA Implementation                        */
/*------------------------------------------------------------------------------------------------*/

unsigned long SHA_H[5];
unsigned char SHA_Block[64];	 	/* 16*4 */
unsigned char SHA_Block_Index;
unsigned char CopySize;
unsigned int SHA_Length;

void SHA_Initial(void)
{
	/*=================================================================================================*/
	/* Calculate SHA Value  */
	/*  */

	/* initial the SHA variables  */
	SHA_H[0] = 0x67452301;
	SHA_H[1] = 0xEFCDAB89;
	SHA_H[2] = 0x98BADCFE;
	SHA_H[3] = 0x10325476;
	SHA_H[4] = 0xC3D2E1F0;

	/* Clean the SHA Block buffer  */
	memset(SHA_Block, 0, 64);
	SHA_Block_Index = 0;

	SHA_Length = 0;
}

void SHA_Push_Data(unsigned char *pData, unsigned char Size)
{
	/*int i;*/
	SHA_Length += Size;

	while(Size) {
		/* Push Data to the SHA block buffer  */
		CopySize = min((64-SHA_Block_Index), Size);
		memcpy(SHA_Block+SHA_Block_Index, pData, CopySize);
		SHA_Block_Index += CopySize;
		pData += CopySize;
		Size -= CopySize;

		if(SHA_Block_Index >= 64) { /* The SHA block buffer Full  */

        /* add by Eric_Lu  */
		/*
		#ifdef Little_Endian   */
			/* Swap the sequence of SHA Block (The little-endian to big-endian) */
		/*	unsigned char Swap_Temp;
			for(i=0; i<64; i+=4) {

				Swap_Temp = SHA_Block[i+0];
				SHA_Block[i+0] = SHA_Block[i+3];
				SHA_Block[i+3] = Swap_Temp;

				Swap_Temp = SHA_Block[i+1];
				SHA_Block[i+1] = SHA_Block[i+2];
				SHA_Block[i+2] = Swap_Temp;
			}
		#endif
		 */
        /* add end */
			/* Do SHA caculation for this SHA block buffer */
			SHA_Calculation(SHA_H, (unsigned long*)SHA_Block);
			memset(SHA_Block, 0, 64);

			SHA_Block_Index = 0; /* Reset the Index  */
		}
	}
}

unsigned long *SHA_Get_SHA_Digest(void)
{
	int i;
	SHA_Block[SHA_Block_Index++] = 0x80;	/* Set EOF */

	if((64 - SHA_Block_Index) < 2) {
		memset(SHA_Block, 0, 64);
	}
	SHA_Length *= 8;
	SHA_Block[62] = (SHA_Length >> 8) & 0xFF; 			/* Pad with Length MSB  */
	SHA_Block[63] = SHA_Length & 0xFF;  				/* Pad with Length LSB   */

    /* add by Eric_Lu  */
    /*                 */

        /* add end  */

	/* Do SHA caculation for final SHA block  */
	SHA_Calculation(SHA_H, (unsigned long*)SHA_Block);

	/* Swap the sequence of SHA_H (The big-endian to little-endian) */
	DBG_printf(("SHA_H:    "));
	for(i=0; i<20; i+=4) {

		Temp_HDCP[0] = ((unsigned char*)SHA_H)[i+0];
		((unsigned char*)SHA_H)[i+0] = ((unsigned char*)SHA_H)[i+3];
		((unsigned char*)SHA_H)[i+3] = Temp_HDCP[0];

		Temp_HDCP[0] = ((unsigned char*)SHA_H)[i+1];
		((unsigned char*)SHA_H)[i+1] = ((unsigned char*)SHA_H)[i+2];
		((unsigned char*)SHA_H)[i+2] = Temp_HDCP[0];

		DBG_printf(("0x%02X%02X%02X%02X ", (int)(((unsigned char*)SHA_H)[i+3]), (int)(((unsigned char*)SHA_H)[i+2]), (int)(((unsigned char*)SHA_H)[i+1]), (int)(((unsigned char*)SHA_H)[i+0])));
	}
	DBG_printf(("\r\n"));

	return SHA_H;
}

void SHA_Calculation(unsigned long pSHA_H[5], unsigned long pSHA_W1[16])
{
	unsigned char i;
	unsigned long TEMP;

	/* =========================================================*/
	/*               */
	/* STEP (c) : Let A = H0, B = H1, C = H2, D = H3, E = H4 */
	/*           */
	Temp_SHA_H[0] = pSHA_H[0];
	Temp_SHA_H[1] = pSHA_H[1];
	Temp_SHA_H[2] = pSHA_H[2];
	Temp_SHA_H[3] = pSHA_H[3];
	Temp_SHA_H[4] = pSHA_H[4];

	/* =========================================================  */

	/* =========================================================   */
	/*                                                   */
	/* STEP (d) : FOR t = 0 to 79 DO                       */
	/*              1. TEMP = S5(A) + Ft(B,C,D) + E + Wt + Kt   */
	/*              2. E = D; D = C; C = S30(B); B = A; A = TEMP;  */
	/*                                                  */
	for (i = 0; i <= 79; i++) {
		/* Update the Message Word while loop time >= 16  */
		if (i >= 16) {
			/* tword = pSHA_W1[tm03] ^ pSHA_W1[tm08] ^ pSHA_W1[tm14] ^ pSHA_W1[tm16];    */
			TEMP = pSHA_W1[(i + 13) % 16] ^ pSHA_W1[(i + 8) % 16] ^ pSHA_W1[(i + 2) % 16] ^ pSHA_W1[i % 16];
			pSHA_W1[i % 16] = (TEMP << 1) | (TEMP >> 31);
		}

		/* Calculate first equation  */
		TEMP = pSHA_W1[i % 16];

	    TEMP += ((Temp_SHA_H[0] << 5) | (Temp_SHA_H[0] >> 27));

	    if (i <= 19)      TEMP += ((Temp_SHA_H[1] & Temp_SHA_H[2]) | (~Temp_SHA_H[1] & Temp_SHA_H[3])) + 0x5A827999;
	    else if (i <= 39) TEMP += (Temp_SHA_H[1] ^ Temp_SHA_H[2] ^ Temp_SHA_H[3]) + 0x6ED9EBA1;
	    else if (i <= 59) TEMP += ((Temp_SHA_H[1] & Temp_SHA_H[2]) | (Temp_SHA_H[1] & Temp_SHA_H[3]) | (Temp_SHA_H[2] & Temp_SHA_H[3])) + 0x8F1BBCDC;
	    else              TEMP += (Temp_SHA_H[1] ^ Temp_SHA_H[2] ^ Temp_SHA_H[3]) + 0xCA62C1D6;

	    TEMP += Temp_SHA_H[4];

    	/* Update the Value A/B/C/D/E  */
    	Temp_SHA_H[4] = Temp_SHA_H[3];
    	Temp_SHA_H[3] = Temp_SHA_H[2];
    	Temp_SHA_H[2] = ((Temp_SHA_H[1] << 30) | (Temp_SHA_H[1] >> 2));
    	Temp_SHA_H[1] = Temp_SHA_H[0];
    	Temp_SHA_H[0] = TEMP;
	}
	/*                                                    */
	/* =========================================================*/

	/* =========================================================*/
	/*                                                         */
	/* STEP (e) : H0 = H0 + A; H1 = H1 + B; H2 = H2 + C; H3 = H3 + D; H4 = H4 + E; */
	/*    */
	pSHA_H[0] += Temp_SHA_H[0];
	pSHA_H[1] += Temp_SHA_H[1];
	pSHA_H[2] += Temp_SHA_H[2];
	pSHA_H[3] += Temp_SHA_H[3];
	pSHA_H[4] += Temp_SHA_H[4];
	/* */
	/* ========================================================= */
}



/*
============================================================================*
	need use customer's function

	1. EP932 reset - use customer's GPIO function

		EP_EP932M_Reset()

		### EP932 must reset after video format timing change ###

	2. EP932 IIC/DDC - use customer's IIC function

		DDC_If.c  	=> 	DDC_Write(......) , DDC_Read(.......)
		EP932_If.c	=> 	IIC_Write(......) , IIC_Read(.......)

		### customer' IIC function must can check IIC error (no ack, write error, read error) ###

	3. Timer

		need [10ms timer] or [thread] to run EP932Controller_Task(); and EP932Controller_Timer();


   ============================================================================
	initial EP932

	1. initial EP932 variable and customer's GPIO + I2C (if need).

		EP_HDMI_Init();

	2. set video interface and timing, timing need modify to fit with customer's require

		EP_HDMI_Set_Video_Timing( 3 );    need change parameter

		EP_HDMI_Set_Video_Output(Video_OUT_Auto);

	3. set audio interface

		EP_HDMI_SetAudFmt(AUD_I2S, AUD_SF_48000Hz);  need change Audio sample rate

	4. need to run EP932Controller_Task and EP932Controller_Timer every 10ms

		while(1)
		{
			EP932Controller_Task();
			EP932Controller_Timer();
		}

============================================================================
*/


/*==============================================================================*/



void EP_EP932M_Reset(void)
{
	/*==============================================================================*/
	/* need use customer's GPIO to reset EP932 after video timing change            */
	/*                                                                              */
	/* 1. GPIO set to low level                                                      */
	/* 2. delay 200ms                                                               */
	/* 3. GPIO set to high level                                                    */
	/*                                                                               */
	/*==============================================================================*/
  /* gpio 5 */

   sp5kGpioWrite(SP5K_GPIO_GRP_GEN, 1<<5, 0);
	sp5kTimeDelay(3,200);  /*1ms*/
   sp5kGpioWrite(SP5K_GPIO_GRP_GEN, 1<<5, 1<<5);


}

void  EP_HDMI_SetAudFmt(HDMI_AudFmt_t  Audfmt, HDMI_AudFreq  Audfreq)
{
	if(Audfmt == AUD_I2S)
	{
		EP932C_Registers.Audio_Interface = 0x18;		/* 2 channel IIS  */
		printf(("Audio interface is IIS - 2.0 CH, "));
	}
	else
	{
		EP932C_Registers.Audio_Interface = 0x10;		/* SPDIF  */
		printf(("Audio interface is SPDIF, "));
	}

	if(Audfreq == AUD_Mute)
	{
		EP932C_Registers.System_Configuration = 0x22;	/* Audio mute enable	*/
	}
	else
	{
		EP932C_Registers.System_Configuration = 0x20;	/* Audio mute disable	*/
	}

	EP932C_Registers.Audio_Input_Format = Audfreq;	/* set Audio frequency */
	printf(("Audio sample rate = "));
	switch(Audfreq)
	{
		case AUD_Mute:
			printf(("Audio Mute\r\n"));
			break;

		case AUD_SF_32000Hz:
			printf(("32K Hz\r\n"));
			break;

		case AUD_SF_44100Hz:
			printf(("44.1K Hz\r\n"));
			break;

		case AUD_SF_48000Hz:
			printf(("48K Hz\r\n"));
			break;

		case AUD_SF_88200Hz:
			printf(("88.2K Hz\r\n"));
			break;

		case AUD_SF_96000Hz:
			printf(("96K Hz\r\n"));
			break;

		case AUD_SF_176400Hz:
			printf(("176.4K Hz\r\n"));
			break;

		case AUD_SF_192000Hz:
			printf(("192K Hz\r\n"));
			break;

		default:
/*		DBG_printf(("Unknown %d \r\n",Audfreq));
		  printf(" unknow %d \n",Audfreq);  */
			break;

	}
}



void  EP_HDMI_Set_Video_Timing(int Timing)
{
		printf(("\r\n\r\n"));
		printf(("##############################################\r\n"));

		/* no skew, Dual edge - falling edge first, 12 bit, FMT12 = 0,  */
		EP932C_Registers.Video_Interface[0] = 0x06;/* | DSEL_Dual_Edge | BSEL_24bit| EDGE_rising | FMT_12; */
		DBG_printf(("Video_Interface_0 = 0x%02X \r\n",(int)EP932C_Registers.Video_Interface[0] ));

		/* mode: DE + Hsync + Vsync , input: YUV422  */
		EP932C_Registers.Video_Interface[1] = 0x00; 	/* DE,HS,VS, YUV422 */
		DBG_printf(("Video_Interface_1 = 0x%02X \r\n",(int)EP932C_Registers.Video_Interface[1] ));

		switch (Timing)
		{
			case 0:
				DBG_printf(("Video_Input_720P_60Hz\r\n"));
				EP932C_Registers.Video_Input_Format[0] = 4;
				break;

			case 1:
				DBG_printf(("Video_Input_720P_50Hz\r\n"));
				EP932C_Registers.Video_Input_Format[0] = 19;
				break;

		   	case 2:
				DBG_printf(("Video_Input_1080I_60Hz\r\n"));
				EP932C_Registers.Video_Input_Format[0] = 5;
				break;

			case 3:
				DBG_printf(("Video_Input_1080P_60Hz\r\n"));
				EP932C_Registers.Video_Input_Format[0] = 16;
				break;

			default:
				DBG_printf(("Video_Input_Unknown : %d\r\n",Timing));
				EP932C_Registers.Video_Input_Format[0] = 0x00;
				break;
		}

		/*
			 0:  No video input
			 1:  CEA-861D 640 x 480 		(60Hz or 59.94Hz)	Progressive 4:3
			 2:  CEA-861D 720 x 480 		(60Hz or 59.94Hz)	Progressive 4:3
			 3:  CEA-861D 720 x 480 		(60Hz or 59.94Hz)	Progressive 16:9
			 4:  CEA-861D 1280 x 720		(60Hz or 59.94Hz)	Progressive 16:9
			 5:  CEA-861D 1920 x 1080	(60Hz or 59.94Hz)	Interlaced 16:9
			 6:  CEA-861D 720(1440) x 480	(60Hz or 59.94Hz)	Interlaced 4:3
			 7:  CEA-861D 720(1440) x 480	(60Hz or 59.94Hz)	Interlaced 16:9
			 8:  CEA-861D 720(1440) x 240	(60Hz or 59.94Hz)	Progressive 4:3
			 9:  CEA-861D 720(1440) x 240	(60Hz or 59.94Hz)	Progressive 16:9
			10: CEA-861D 2880 x 480 		(60Hz or 59.94Hz)	Interlaced 4:3
			11: CEA-861D 2880 x 480 		(60Hz or 59.94Hz)	Interlaced 16:9
			12: CEA-861D 2880 x 240 		(60Hz or 59.94Hz)	Progressive 4:3
			13: CEA-861D 2880 x 240 		(60Hz or 59.94Hz)	Progressive 16:9
			14: CEA-861D 1440 x 480 		(60Hz or 59.94Hz)	Progressive 4:3
			15: CEA-861D 1440 x 480 		(60Hz or 59.94Hz)	Progressive 16:9
			16: CEA-861D 1920 x 1080		(60Hz or 59.94Hz)	Progressive 16:9
			17: CEA-861D 720 x 576		(50Hz)			Progressive 4:3
			18: CEA-861D 720 x 576		(50Hz)			Progressive 16:9
			19: CEA-861D 1280 x 720 		(50Hz)			Progressive 16:9
			20: CEA-861D 1920 x 1080		(50Hz)			Interlaced 16:9
			21: CEA-861D 720(1440) x 576	(50Hz)			Interlaced 4:3
			22: CEA-861D 720(1440) x 576	(50Hz)			Interlaced 16:9
			23: CEA-861D 720(1440) x 288	(50Hz)			Progressive 4:3
			24: CEA-861D 720(1440) x 288	(50Hz)			Progressive 16:9
			25: CEA-861D 2880 x 576 		(50Hz)			Interlaced 4:3
			26: CEA-861D 2880 x 576 		(50Hz)			Interlaced 16:9
			27: CEA-861D 2880 x 288 		(50Hz)			Progressive 4:3
			28: CEA-861D 2880 x 288 		(50Hz)			Progressive 16:9
			29: CEA-861D 1440 x 576 		(50Hz)			Progressive 4:3
			30: CEA-861D 1440 x 576 		(50Hz)			Progressive 16:9
			31: CEA-861D 1920 x 1080		(50Hz)			Progressive 16:9
			32: CEA-861D 1920 x 1080		(24Hz)			Progressive 16:9
			33: CEA-861D 1920 x 1080		(25Hz)			Progressive 16:9
			34: CEA-861D 1920 x 1080		(30Hz)			Progressive 16:9
		*/

		/* power on  */
		EP932C_Registers.Power_Control = 0x00;

	/*===================================================================*/

		DBG_printf(("##############################################\r\n"));


}

void EP_HDMI_Set_Video_Output(int HDMI_output)
{
	switch(HDMI_output)
	{
		default:
		case Video_OUT_Auto:
			EP932C_Registers.Video_Output_Format = 0x00;		/* Auto select output		*/
			break;

		case Video_OUT_YUV_444:
			EP932C_Registers.Video_Output_Format = 0x01;		/* YUV 444 output			*/
			break;

		case Video_OUT_YUV_422:
			EP932C_Registers.Video_Output_Format = 0x02;		/* YUV 422 output			*/
			break;

		case Video_OUT_RGB_444:
			EP932C_Registers.Video_Output_Format = 0x03;		/* RGB 444 output		*/
			break;

	}
}

unsigned char EP_HDMI_Init(void)
{

	EP932Controller_Initial(&EP932C_Registers, NULL);

	return 0; /*HDMI_SUCCESS;  */
}

void hdmi2_start (void)
{
return ;
}


/*
		Controller.c start
*/

void EP932Controller_Initial(PEP932C_REGISTER_MAP pEP932C_RegMap, EP932C_CALLBACK IntCall)
{
	/* Save the Logical Hardware Assignment  */
	pEP932C_Registers = pEP932C_RegMap;
	EP932C_GenerateInt = IntCall;

	/* EP932 Reset Control  */
	EP_EP932M_Reset();

	/* Initial IIC	    */
	EP932_If_Initial();

	/* Reset Variables  */
	/* bit  */
	is_Cap_HDMI = 0;
	is_Cap_YCC444 = is_Cap_YCC422 = 0;
	is_Connected = 0;
	is_VideoChanging = 0;
	is_AudioChanging = 0;
	/* data  */
	TX_State = TXS_Search_EDID;
	HTP_TimeCount = 0;
	Process_Dispatch_ID = 0;
	VideoChg_TimeCount = 0;
	AudioChg_TimeCount = 0;
	ReadEDID_TimeCount = 0;
	HP_ChangeCount = 0;
	RSEN_ChangeCount = 0;
	memset(Gamut_Packet_Header_Backup, 0, 3);

	/* Reset all EP932C registers */
	memset(pEP932C_Registers, 0, sizeof(EP932C_REGISTER_MAP));
	pEP932C_Registers->Video_Interface[0] = 0x80;
	pEP932C_Registers->Power_Control = EP932E_Power_Control__PD_HDMI;
	pEP932C_Registers->Audio_Interface = 0x10; /* 2 Channel audio  */

	/* Update Version Registers  */
	pEP932C_Registers->VendorID = 0x177A;
	pEP932C_Registers->DeviceID = 0x0932;
	pEP932C_Registers->Version_Major = VERSION_MAJOR;
	pEP932C_Registers->Version_Minor = VERSION_MINOR;
	DBG_printf(("Version %d.%d\r\n", (int)VERSION_MAJOR, (int)VERSION_MINOR ));
	/* Initial HDCP Info  */

/*stanley	memset(pEP932C_Registers->HDCP_AKSV, 0x00, sizeof(pEP932C_Registers->HDCP_AKSV));*/
   memset(pEP932C_Registers->HDCP_BKSV, 0x00, sizeof(pEP932C_Registers->HDCP_BKSV));
	printf("after memset\n");
	/* Update Configuration Registers */
	EP932_Reg_Read(EP932_Configuration, DDC_Data, 1);
		printf("EP932_Reg_Read\n");
	pEP932C_Registers->Configuration = DDC_Data[0];
	printf("pEP932C_Registers\n");

	/* Set Revocation List address */

	HDCP_Extract_BKSV_BCAPS3(pEP932C_Registers->HDCP_BKSV);
	printf("HDCP_Extract_BKSV_BCAPS3\n");
	HDCP_Extract_FIFO((unsigned char*)pEP932C_Registers->HDCP_KSV_FIFO, sizeof(pEP932C_Registers->HDCP_KSV_FIFO));
	printf("HDCP_Extract_FIFO\n");
	HDCP_Stop();
	printf("HDCP_Stop\n");

	/* Reset EP932 Control Program */
	EP932Controller_Reset();
}

void EP932Controller_Reset(void)
{
#if Enable_HDCP
	int i;
	SMBUS_STATUS status = SMBUS_STATUS_Success;
#endif

	/* Reset Hardware  */
	DBG_printf(("Reset EP932\r\n"));

	EP_EP932M_Reset();

	/* Initial Variables  */
	EP932_Reg_Set_Bit(EP932_Pixel_Repetition_Control, EP932_Pixel_Repetition_Control__OSCSEL);

#if Enable_HDCP

	/* Read HDCP Key for EEPROM  */
	status = HDMI_Tx_Get_Key((unsigned char *)HDCP_Key);
	/*DBG_printf(("Read HDCP Key = 0x%02X\r\n",(int)status));  */
	HDCP_Fake(0);
	pEP932C_Registers->System_Status &= ~EP932E_System_Status__KEY_FAIL;

	/* Check HDCP key and up load the key  */
	if(status) {
		/* Do not upload the default Key!  */
		pEP932C_Registers->System_Configuration |= EP932E_System_Configuration__HDCP_DIS;
		pEP932C_Registers->System_Status |= EP932E_System_Status__KEY_FAIL;
		DBG_printf(("No HDCP Key\r\n"));
	}
	else {
		/* Check HDCP key and up load the key  */
		ChkSum = 0;
		for(i=0; i<328; ++i) {
			ChkSum += *((unsigned char *)HDCP_Key+i);
		}
		DBG_printf(("HDCP Key Check Sum 0x%02X\r\n", (int)ChkSum ));
		if(HDCP_Key[3][7] != 0x50 || HDCP_Key[12][7] != 0x01 || ChkSum != 0x00) {/* || HDCP_Key[40][0] != 0xA5) {  */
			HDCP_Fake(1);
			pEP932C_Registers->System_Status |= EP932E_System_Status__KEY_FAIL;
			DBG_printf(("Check Key failed!\r\n"));
			pEP932C_Registers->System_Configuration |= EP932E_System_Configuration__HDCP_DIS;
			/*DBG_printf(("Disable HDCP \r\n"));  */
		}
		else {
			/* Upload the key 0-39  */
			for(i=0; i<40; ++i) {
				DDC_Data[0] = (unsigned char)i;
				status |= EP932_Reg_Write(EP932_Key_Add, DDC_Data, 1);
				memcpy(DDC_Data,&HDCP_Key[i][0],7);
				status |= EP932_Reg_Write(EP932_Key_Data, DDC_Data, 7);
			}
			/* Read and check	 */
			for(i=0; i<40; ++i) {
				DDC_Data[0] = (unsigned char)i;
				status |= EP932_Reg_Write(EP932_Key_Add, DDC_Data, 1);
				status |= EP932_Reg_Read(EP932_Key_Data, DDC_Data, 7);
				if((memcmp(DDC_Data,&HDCP_Key[i][0],7) != 0) || status) {
					/* Test failed  */
					HDCP_Fake(1);
					pEP932C_Registers->System_Status |= EP932E_System_Status__KEY_FAIL;
					DBG_printf(("Check Key failed!\r\n"));
					pEP932C_Registers->System_Configuration |= EP932E_System_Configuration__HDCP_DIS;
					/*DBG_printf(("Disable HDCP \r\n"));  */
					break;
				}
			}
			/* Upload final KSV 40  */
			DDC_Data[0] = 40;
			status |= EP932_Reg_Write(EP932_Key_Add, DDC_Data, 1);
			memcpy(DDC_Data,&HDCP_Key[40][0],7);
			status |= EP932_Reg_Write(EP932_Key_Data, DDC_Data, 7);
			/* Read back and check  */
	    	if(!HDMI_Tx_read_AKSV(pEP932C_Registers->HDCP_AKSV)) {
				/* Test failed  */
				HDCP_Fake(1);
				pEP932C_Registers->System_Status |= EP932E_System_Status__KEY_FAIL;
				DBG_printf(("Check KSV failed!\r\n"));
				pEP932C_Registers->System_Configuration |= EP932E_System_Configuration__HDCP_DIS;
				/*DBG_printf(("Disable HDCP \r\n"));  */
			}
		}
	}

#else

	pEP932C_Registers->System_Status |= EP932E_System_Status__KEY_FAIL;
	pEP932C_Registers->System_Configuration |= EP932E_System_Configuration__HDCP_DIS;

#endif

	/* EP932 Interface Reset  */
	EP932_If_Reset();

	/* Internal Variable Reset  */
	/* bit  */
	/*Event_HDMI_Int = 0;  */
	is_ReceiverSense = 0;

	/* data  */
	Backup_Analog_Test_Control = 0;
	if(TX_State > TXS_Search_EDID) {
		DBG_printf(("\r\nState Transist: Reset -> [TXS_Wait_Upstream]\r\n"));
		TX_State = TXS_Wait_Upstream;
	}

	DBG_printf(("EP932Controller_Reset finish\r\n"));
}

void EP932Controller_Timer(void)
{
	sp5kTimeDelay(3,2);  /*10ms*/


	++HTP_TimeCount;
	if(is_VideoChanging) ++VideoChg_TimeCount;
	if(is_AudioChanging) ++AudioChg_TimeCount;
	if(TX_State == TXS_HDCP) HDCP_Timer();
	++ReadEDID_TimeCount;
}

unsigned char EP932Controller_Task(void)
{

	/* Read Interrupt Flag and updat the internal information  */
	ReadInterruptFlags();

	/* Polling Hot-Plug every 80ms  */
	if(HTP_TimeCount > 80/EP932C_TIMER_PERIOD) {
		HTP_TimeCount = 0;

		ConnectionState = HDMI_Tx_HTPLG();
          printf(" ConnectionState =%d \n", ConnectionState);
		HTPLG_NOW = ConnectionState;
		if(HTPLG_LAST != HTPLG_NOW)
		{
			HTPLG_LAST = HTPLG_NOW;
			if(HTPLG_NOW == 0)
			{
				DBG_printf(("Without HotPlug\r\n"));
				EP_HDMI_DumpMessage();
			}
			else
			{
				DBG_printf(("Detect HotPlug \r\n"));
			}
		}

		is_Hot_Plug = (ConnectionState == 1)? 1:0;
		if(is_Connected != ((ConnectionState)?1:0) ) {
			if(HP_ChangeCount++ >= 1) { /* Accept continuous 1 error = 1*80 ms = 80 ms (Skip when low period < 80 ms)  */
				HP_ChangeCount = 0;

				is_Connected = ((ConnectionState)?1:0);
			}
		}
		else {
			HP_ChangeCount = 0;
		}
		if(is_Hot_Plug) {
			pEP932C_Registers->System_Status |= EP932E_System_Status__HTPLG;
		}
		else {
			pEP932C_Registers->System_Status &= ~EP932E_System_Status__HTPLG;
		}

		is_ReceiverSense = HDMI_Tx_RSEN(); /* Only valid when TX is powered on */

		if(TX_State > TXS_Wait_Upstream) { /* Powered Up and have Input  */

			/* Update RSEN  */
			if(is_ReceiverSense) {
				pEP932C_Registers->System_Status |= EP932E_System_Status__RSEN;
			}
			else {
				pEP932C_Registers->System_Status &= ~EP932E_System_Status__RSEN;
			}
			RSEN_ChangeCount = 0;

			/* Read HSO VSO POL information  */
			EP932_Reg_Read(EP932_General_Control_4, DDC_Data, 1);
			Video_Params.HVPol = DDC_Data[0] & (EP932_DE_Control__VSO_POL | EP932_DE_Control__HSO_POL);
		}
		else {
			if(RSEN_ChangeCount++ >= 8) { /* Accept continuous 8 error = 8*80 ms = 640 ms (Skip when low period < 640 ms) */
				RSEN_ChangeCount = 0;

				pEP932C_Registers->System_Status &= ~EP932E_System_Status__RSEN;
			}
		}
	}

	/*                                                         */
	/* Update EP932 Registers according to the System Process  */
	/*                                                        */
	switch(TX_State) {
		case TXS_Search_EDID:
			if(is_Connected) {
				if(ReadEDID_TimeCount > 200/EP932C_TIMER_PERIOD) {
					unsigned char EDID_DDC_Status;

					/* Confirm Hot-Plug (time-out after 1s)  */
					if(!is_Hot_Plug) {
						if(ReadEDID_TimeCount <= 1000/EP932C_TIMER_PERIOD) break;
						DBG_printf(("WARNING: EDID detected without Hot-Plug for 1s\r\n"));
					}

					/* Read EDID  */
					DBG_printf(("\r\nState Transist: Read EDID -> [TXS_Wait_Upstream]\r\n"));
					memset(pEP932C_Registers->Readed_EDID, 0xFF, 256);
					EDID_DDC_Status = Downstream_Rx_read_EDID(pEP932C_Registers->Readed_EDID);

					if(EDID_DDC_Status) {
						/*if(EDID_DDC_Status == EDID_STATUS_NoAct) {  */
						if(EDID_DDC_Status != EDID_STATUS_ChecksumError) {
							DBG_printf(("WARNING: EDID read failed 0x%02X\r\n", (int)EDID_DDC_Status));
							if(ReadEDID_TimeCount <= 500/EP932C_TIMER_PERIOD) break;
						}
					}
					ReadEDID_TimeCount = 0;

					/* Set Output */
					if(pEP932C_Registers->System_Configuration & EP932E_System_Configuration__FORCE_HDMI_CAP) {
						is_Cap_HDMI = 1;
					}
					else {
						is_Cap_HDMI = 1 ;/*stanley EDID_GetHDMICap(pEP932C_Registers->Readed_EDID);*/
					}
					if(is_Cap_HDMI) {
						DBG_printf(("Support HDMI\n"));

						/* Default Capability */
						is_Cap_YCC444 =	is_Cap_YCC422 = 0;
						pEP932C_Registers->EDID_ASFreq = 0x07;
						pEP932C_Registers->EDID_AChannel = 1;

						pEP932C_Registers->EDID_VideoDataAddr = 0x00;
						pEP932C_Registers->EDID_AudioDataAddr = 0x00;
						pEP932C_Registers->EDID_SpeakerDataAddr = 0x00;
						pEP932C_Registers->EDID_VendorDataAddr = 0x00;

						if(!EDID_DDC_Status) {

							if(pEP932C_Registers->Readed_EDID[131] & 0x20) {	/* Support YCC444 */
								is_Cap_YCC444 = 1;
								DBG_printf((" YCC444"));
							}
							if(pEP932C_Registers->Readed_EDID[131] & 0x10) {	/* Support YCC422  */
								is_Cap_YCC422 = 1;
								DBG_printf((" YCC422"));
							}
							DBG_printf(("\r\n"));
							pEP932C_Registers->EDID_ASFreq = EDID_GetPCMFreqCap(pEP932C_Registers->Readed_EDID);
							DBG_printf(("EDID ASFreq = 0x%02X\r\n",(int)pEP932C_Registers->EDID_ASFreq));

							pEP932C_Registers->EDID_AChannel = EDID_GetPCMChannelCap(pEP932C_Registers->Readed_EDID);
							DBG_printf(("EDID AChannel = 0x%02X\r\n",(int)pEP932C_Registers->EDID_AChannel));

							pEP932C_Registers->EDID_VideoDataAddr = EDID_GetDataBlockAddr(pEP932C_Registers->Readed_EDID, 0x40);
							pEP932C_Registers->EDID_AudioDataAddr = EDID_GetDataBlockAddr(pEP932C_Registers->Readed_EDID, 0x20);
							pEP932C_Registers->EDID_SpeakerDataAddr = EDID_GetDataBlockAddr(pEP932C_Registers->Readed_EDID, 0x80);
							pEP932C_Registers->EDID_VendorDataAddr = EDID_GetDataBlockAddr(pEP932C_Registers->Readed_EDID, 0x60);
						}
					}
					else {
						DBG_printf(("Support DVI RGB only\r\n"));
						is_Cap_YCC444 =	is_Cap_YCC422 = 0;
						pEP932C_Registers->EDID_ASFreq = pEP932C_Registers->EDID_AChannel = 0;
					}

					if(is_Cap_HDMI)
						pEP932C_Registers->EDID_Status = EDID_DDC_Status | EP932E_EDID_Status__HDMI;
					else
						pEP932C_Registers->EDID_Status = EDID_DDC_Status;
					DBG_printf(("Support Max Audio Channel %d\r\n", (int)pEP932C_Registers->EDID_AChannel+1));
					DBG_printf(("Support Audio Freq 0x%02X\r\n", (int)pEP932C_Registers->EDID_ASFreq));

					/* Report EDID Change */
					pEP932C_Registers->Interrupt_Flags |= EP932E_Interrupt_Flags__EDID_CHG;
					if(EP932C_GenerateInt && (pEP932C_Registers->Interrupt_Enable & EP932E_Interrupt_Enable__EDID_CHG) ) EP932C_GenerateInt();

					TX_State = TXS_Wait_Upstream;
				}
			}
			else {
				pEP932C_Registers->EDID_Status = EDID_STATUS_NoAct;
				ReadEDID_TimeCount = 0;
			}
			break;

		case TXS_Wait_Upstream:

			if(!is_Connected) {

				TXS_RollBack_Wait_Upstream();
				TX_State = TXS_Search_EDID;
			}
			else if(!(pEP932C_Registers->Power_Control & (EP932E_Power_Control__PD_HDMI | EP932E_Power_Control__PD_TOT)) ) {
				DBG_printf(("\r\nState Transist: Power Up -> [TXS_Stream]\r\n"));

				/* Power Up */
				HDMI_Tx_Power_Up();

				TX_State = TXS_Stream;
			}
			else {
				/* Check Force HDMI bit */
				if(!is_Cap_HDMI) {
					if(pEP932C_Registers->System_Configuration & EP932E_System_Configuration__FORCE_HDMI_CAP) {
						TXS_RollBack_Wait_Upstream();
						TX_State = TXS_Search_EDID;
					}
				}
			}
			break;

		case TXS_Stream:

#if Enable_HDCP
			if(!is_HDCP_Info_BKSV_Rdy && is_ReceiverSense && is_Hot_Plug) {
				/* Get HDCP Info  */
		    	if(!Downstream_Rx_read_BKSV(pEP932C_Registers->HDCP_BKSV)) {
					pEP932C_Registers->HDCP_Status = EP932E_HDCP_Status__BKSV;
				}
				pEP932C_Registers->HDCP_BCAPS3[0] = Downstream_Rx_BCAPS();
				is_HDCP_Info_BKSV_Rdy = 1;
			}
#endif

			if(!is_Connected) {

				TXS_RollBack_Stream();
				TXS_RollBack_Wait_Upstream();
				TX_State = TXS_Search_EDID;
			}
			else if(pEP932C_Registers->Power_Control & (EP932E_Power_Control__PD_HDMI | EP932E_Power_Control__PD_TOT) ) {
				pEP932C_Registers->Power_Control |= EP932E_Power_Control__PD_HDMI;

				TXS_RollBack_Stream();
				TX_State = TXS_Wait_Upstream;
			}

#if Enable_HDCP
			else if(!((pEP932C_Registers->System_Configuration & EP932E_System_Configuration__HDCP_DIS) || is_VideoChanging) && is_ReceiverSense) {
				/* Enable mute for transmiter video and audio  */
				HDMI_Tx_Mute_Enable();

				DBG_printf(("\r\nState Transist: Start HDCP -> [TXS_HDCP]\r\n"));
				TX_State = TXS_HDCP;
			}
#endif

			break;
		case TXS_HDCP:
#if Enable_HDCP
			if(!is_Connected || !is_Hot_Plug) {

				TXS_RollBack_HDCP();
				TXS_RollBack_Stream();
				TXS_RollBack_Wait_Upstream();
				TX_State = TXS_Search_EDID;
			}
			else if(pEP932C_Registers->Power_Control & (EP932E_Power_Control__PD_HDMI | EP932E_Power_Control__PD_TOT) ) {
				pEP932C_Registers->Power_Control |= EP932E_Power_Control__PD_HDMI;

				TXS_RollBack_HDCP();
				TXS_RollBack_Stream();
				TX_State = TXS_Wait_Upstream;
			}
			else if((pEP932C_Registers->System_Configuration & EP932E_System_Configuration__HDCP_DIS) || is_VideoChanging) {

				TXS_RollBack_HDCP();
				TX_State = TXS_Stream;
			}
			else {
				pEP932C_Registers->HDCP_State = HDCP_Authentication_Task(is_ReceiverSense && is_Hot_Plug);
				pEP932C_Registers->HDCP_Status = HDCP_Get_Status();
			}
#endif
			break;

	}

	/*                                        */
	/* Update EP932 Registers for any time    */
	/*                                        */
#if 1

	/* Mute Control */
	if( (pEP932C_Registers->System_Configuration & EP932E_System_Configuration__AUDIO_DIS) || (TX_State < TXS_Stream) || is_VideoChanging  || is_AudioChanging ) {
		HDMI_Tx_AMute_Enable();
	}
	else {
		HDMI_Tx_AMute_Disable();
	}

	if( (pEP932C_Registers->System_Configuration & EP932E_System_Configuration__VIDEO_DIS) || (TX_State < TXS_Stream) || is_VideoChanging ) {
		HDMI_Tx_VMute_Enable();
	}
	else {
		HDMI_Tx_VMute_Disable();
	}

	/* HDMI Mode  */

	if(!is_Cap_HDMI || (pEP932C_Registers->System_Configuration & EP932E_System_Configuration__HDMI_DIS) ) {
		HDMI_Tx_DVI();		/* Set to DVI mode (The Info Frame and Audio Packets would not be send)  */
	}
	else {
		HDMI_Tx_HDMI(); /* Set to HDMI mode  */

	}
#else
/* stanley*/

	HDMI_Tx_AMute_Enable();
	HDMI_Tx_VMute_Enable();

	HDMI_Tx_HDMI(); /* Set to HDMI mode  */
/* end stanley*/
#endif

	++Process_Dispatch_ID;
	if(Process_Dispatch_ID > 2) Process_Dispatch_ID = 0;

	switch(Process_Dispatch_ID) {

		case 0:
			/*                         */
			/* Update Video Params     */
			/*                         */

			/* Video Interface  */
			Video_Params.Interface = pEP932C_Registers->Video_Interface[0];

			/* Video Timing  */
			if(pEP932C_Registers->Video_Input_Format[0]) {
				/* Manul set the Video Timing  */
				if(pEP932C_Registers->Video_Input_Format[0] < 128) {
					Video_Params.VideoSettingIndex = pEP932C_Registers->Video_Input_Format[0];
				}
				else {
					Video_Params.VideoSettingIndex = pEP932C_Registers->Video_Input_Format[0] - (128 - EP932_VDO_Settings_IT_Start);
				}
			}

			/* Select Sync Mode  */
			Video_Params.SyncMode = (pEP932C_Registers->Video_Interface[1] & EP932E_Video_Interface_Setting_1__SYNC) >> 2;

			/* Select Color Space  */
			switch(pEP932C_Registers->Video_Interface[1] & EP932E_Video_Interface_Setting_1__COLOR) {
				default:
				case EP932E_Video_Interface_Setting_1__COLOR__Auto:
					switch(Video_Params.VideoSettingIndex) {
						case  4: case  5: case 16: case 19: case 20: case 31: case 32:
						case 33: case 34: case 39: case 40: case 41: case 46: case 47:		/* HD Timing  */
							Video_Params.ColorSpace = COLORSPACE_709;
							break;

						default:
							if(Video_Params.VideoSettingIndex && Video_Params.VideoSettingIndex < EP932_VDO_Settings_IT_Start) { /* SD Timing */
								Video_Params.ColorSpace = COLORSPACE_601;
							}
							else {															/* IT Timing */
								Video_Params.ColorSpace = COLORSPACE_709;
							}
					}
					break;
				case EP932E_Video_Interface_Setting_1__COLOR__601:
					Video_Params.ColorSpace = COLORSPACE_601;
					break;
				case EP932E_Video_Interface_Setting_1__COLOR__709:
					Video_Params.ColorSpace = COLORSPACE_709;
					break;
			}

			/* Set Input Format  */
			switch(pEP932C_Registers->Video_Interface[1] & EP932E_Video_Interface_Setting_1__VIN_FMT) {
				default:
				case EP932E_Video_Interface_Setting_1__VIN_FMT__RGB:
					Video_Params.FormatIn = COLORFORMAT_RGB;
					Video_Params.FormatOut = COLORFORMAT_RGB;
					break;
				case EP932E_Video_Interface_Setting_1__VIN_FMT__YCC444:
					Video_Params.FormatIn = COLORFORMAT_YCC444;
					if(is_Cap_YCC444) {
						Video_Params.FormatOut = COLORFORMAT_YCC444;
					}
					else if(is_Cap_YCC422) {
						Video_Params.FormatOut = COLORFORMAT_YCC422;
					}
					else {
						Video_Params.FormatOut = COLORFORMAT_RGB;
					}
					break;
				case EP932E_Video_Interface_Setting_1__VIN_FMT__YCC422:
					Video_Params.FormatIn = COLORFORMAT_YCC422;
					if(is_Cap_YCC444) {
						Video_Params.FormatOut = COLORFORMAT_YCC444;
					}
					else if(is_Cap_YCC422) {
						Video_Params.FormatOut = COLORFORMAT_YCC422;
					}
					else {
						Video_Params.FormatOut = COLORFORMAT_RGB;
					}
					break;
			}

	/*add by eric.lu  */

			/* Set Output Format  */
			switch(pEP932C_Registers->Video_Output_Format) {
				default:
				case 0:		/* Auto, don't need change setting.  */
					break;

				case 1:		/* Force to YUV444 output format  */
					Video_Params.FormatOut = COLORFORMAT_YCC444;
					break;

				case 2:		/* Force to YUV422 output format  */
					Video_Params.FormatOut = COLORFORMAT_YCC422;
					break;

				case 3:		/* Force to RGB444 output format  */
					Video_Params.FormatOut = COLORFORMAT_RGB;
					break;
			}

	/* end of add  */

			/* DVI mode settings overwrite  */
			if(!is_Cap_HDMI || (pEP932C_Registers->System_Configuration & EP932E_System_Configuration__HDMI_DIS) ) {
				Video_Params.FormatOut = COLORFORMAT_RGB;
			}

			/* AFAR  */
			Video_Params.AFARate = ((pEP932C_Registers->Video_Input_Format[1] & EP932E_Video_Input_Format_1__AFAR) >> 4) | 0x08;

		/* add by eric.lu  */
			/* SCAN			*/
			Video_Params.SCAN = (pEP932C_Registers->Video_Input_Format[1] & EP932E_Video_Input_Format_1__SCAN);
		/* end of add  */

			/* Video Change */
			if(memcmp(&Video_Params, &pEP932C_Registers->Video_Params_Backup, sizeof(VDO_PARAMS)) != 0) {
				if(memcmp(&Video_Params, &pEP932C_Registers->Video_Params_Backup, 6) != 0) {
					is_TimingChanging = 1;
				}
/*				DBG_printf(("Video_Params new: interface 0x%02X, Vindex 0x%02X, HV 0x%02X, mode 0x%02X, Fin 0x%02X, Fout 0x%02X, color 0x%02X, AFAR 0x%02X\r\n",(int)Video_Params.Interface, (int)Video_Params.VideoSettingIndex, (int)Video_Params.HVPol ,(int)Video_Params.SyncMode, (int)Video_Params.FormatIn, (int)Video_Params.FormatOut, (int)Video_Params.ColorSpace, (int)Video_Params.AFARate));  */
/*				DBG_printf(("Video_Params old: interface 0x%02X, Vindex 0x%02X, HV 0x%02X, mode 0x%02X, Fin 0x%02X, Fout 0x%02X, color 0x%02X, AFAR 0x%02X\r\n",(int)pEP932C_Registers->Video_Params_Backup.Interface, (int)pEP932C_Registers->Video_Params_Backup.VideoSettingIndex, (int)pEP932C_Registers->Video_Params_Backup.HVPol ,(int)pEP932C_Registers->Video_Params_Backup.SyncMode, (int)pEP932C_Registers->Video_Params_Backup.FormatIn, (int)pEP932C_Registers->Video_Params_Backup.FormatOut, (int)pEP932C_Registers->Video_Params_Backup.ColorSpace, (int)pEP932C_Registers->Video_Params_Backup.AFARate)); */

				pEP932C_Registers->Video_Params_Backup = Video_Params;

				VideoChg_TimeCount = 0;
				is_VideoChanging = 1;
			}

			/* Video Change Debouncing */
			if(is_VideoChanging) {
				if(VideoChg_TimeCount > AV_STABLE_TIME/EP932C_TIMER_PERIOD) {

					DBG_printf(("### VideoChanging \r\n"));

					if(is_TimingChanging)
						EP932Controller_Reset();

					HDMI_Tx_Video_Config(&Video_Params);

					if(is_TimingChanging) {
						if(!is_AudioChanging)
							HDMI_Tx_Audio_Config(&Audio_Params);
					}

					is_TimingChanging = 0;
					is_VideoChanging = 0;
					VideoChg_TimeCount = 0;

					/* Report Video Change */
					pEP932C_Registers->Interrupt_Flags |= EP932E_Interrupt_Flags__VIDEO_CHG;
					if(EP932C_GenerateInt && (pEP932C_Registers->Interrupt_Enable & EP932E_Interrupt_Enable__VIDEO_CHG) ) EP932C_GenerateInt();
				}
			}
			break;

		case 1:
			/*                        */
			/* Update Audio Params     */
			/*                        */
			Audio_Params.Interface = pEP932C_Registers->Audio_Interface & 0x0F; /* IIS, WS_M, WS_POL, SCK_POL  */
			Audio_Params.VideoSettingIndex = Video_Params.VideoSettingIndex;

			/* Update Audio Channel Number  */
			if(EP932_VDO_Settings[Video_Params.VideoSettingIndex].Pix_Freq_Type <= PIX_FREQ_27027KHz) {
				Audio_Params.ChannelNumber = 1;
			}
			else {
				Audio_Params.ChannelNumber = min(((pEP932C_Registers->Audio_Interface & 0x70) >> 4), pEP932C_Registers->EDID_AChannel);
			}

			/* Update VFS  */
			if(Audio_Params.VideoSettingIndex < EP932_VDO_Settings_IT_Start) {
				/* Pixel Clock Type shift (59.94/60)  */
				Audio_Params.VFS = (pEP932C_Registers->Video_Input_Format[1] & EP932E_Video_Input_Format_1__VIF)? 1:0;
			}
			else {
				Audio_Params.VFS = 0;
			}
			Audio_Params.NoCopyRight = (pEP932C_Registers->Audio_Input_Format & EP932E_Audio_Input_Format__NoCopyRight)?1:0;

			/* Write Frequency info (Use ADO_FREQ or Auto)  */
			switch( pEP932C_Registers->Audio_Input_Format & EP932E_Audio_Input_Format__ADO_FREQ ) {

				case EP932E_Audio_Input_Format__ADO_FREQ__32000Hz:
					Audio_Params.InputFrequency = ADSFREQ_32000Hz;
					/* Disable Down Sample  */
					Audio_Params.ADSRate = 0;
					break;

				default:
				case EP932E_Audio_Input_Format__ADO_FREQ__44100Hz:
					Audio_Params.InputFrequency = ADSFREQ_44100Hz;
					/* Disable Down Sample  */
					Audio_Params.ADSRate = 0;
					break;

				case EP932E_Audio_Input_Format__ADO_FREQ__48000Hz:
					Audio_Params.InputFrequency = ADSFREQ_48000Hz;
					/* Disable Down Sample  */
					Audio_Params.ADSRate = 0;
					break;

				case EP932E_Audio_Input_Format__ADO_FREQ__88200Hz:
					Audio_Params.InputFrequency = ADSFREQ_88200Hz;
					if(pEP932C_Registers->EDID_ASFreq & 0x08) { /* 88.2kHz */
						/* Disable Down Sample  */
						Audio_Params.ADSRate = 0;
					}
					else {
						/* Enable Down Sample 1/2  */
						Audio_Params.ADSRate = 1;
					}
					break;

				case EP932E_Audio_Input_Format__ADO_FREQ__96000Hz:
					Audio_Params.InputFrequency = ADSFREQ_96000Hz;
					if(pEP932C_Registers->EDID_ASFreq & 0x10) { /* 96kHz */
						/* Disable Down Sample  */
						Audio_Params.ADSRate = 0;
					}
					else {
						if(pEP932C_Registers->EDID_ASFreq & 0x04) { /* 48kHz  */
							/* Enable Down Sample 1/2  */
							Audio_Params.ADSRate = 1;
						}
						else {
							/* Enable Down Sample 1/3  */
							Audio_Params.ADSRate = 2;
						}
					}
					break;

				case EP932E_Audio_Input_Format__ADO_FREQ__176400Hz:
					Audio_Params.InputFrequency = ADSFREQ_176400Hz;
					if(pEP932C_Registers->EDID_ASFreq & 0x20) { /* 176kHz  */
						/* Disable Down Sample  */
						Audio_Params.ADSRate = 0;
					}
					else {
						if(pEP932C_Registers->EDID_ASFreq & 0x08) { /* 88.2kHz  */
							/* Enable Down Sample 1/2  */
							Audio_Params.ADSRate = 1;
						}
						else {
							/* Enable Down Sample 1/4 */
							Audio_Params.ADSRate = 3;
						}
					}
					break;

				case EP932E_Audio_Input_Format__ADO_FREQ__192000Hz:
					Audio_Params.InputFrequency = ADSFREQ_192000Hz;
					if(pEP932C_Registers->EDID_ASFreq & 0x40) { /* 192kHz  */
						/* Disable Down Sample */
						Audio_Params.ADSRate = 0;
					}
					else {
						if(pEP932C_Registers->EDID_ASFreq & 0x10) { /* 96kHz  */
							/* Enable Down Sample 1/2  */
							Audio_Params.ADSRate = 1;
						}
						else {
							/* Enable Down Sample 1/4  */
							Audio_Params.ADSRate = 3;
						}
					}
					break;
			}

			/* Audio Change */
			if(memcmp(&Audio_Params, &pEP932C_Registers->Audio_Params_Backup, sizeof(ADO_PARAMS)) != 0) {
				pEP932C_Registers->Audio_Params_Backup = Audio_Params;

				AudioChg_TimeCount = 0;
				is_AudioChanging = 1;
			}

			/* Audio Change Debouncing  */
			if(is_AudioChanging) {
				if(AudioChg_TimeCount > AV_STABLE_TIME/EP932C_TIMER_PERIOD) {
					HDMI_Tx_Audio_Config(&Audio_Params);
					is_AudioChanging = 0;
					AudioChg_TimeCount = 0;

					/* Report Audio Change  */
					pEP932C_Registers->Interrupt_Flags |= EP932E_Interrupt_Flags__AUDIO_CHG;
					if(EP932C_GenerateInt && (pEP932C_Registers->Interrupt_Enable & EP932E_Interrupt_Enable__AUDIO_CHG) ) EP932C_GenerateInt();
				}
			}
			break;

		case 2:

			/* Update TREG */
			if(pEP932C_Registers->Analog_Test_Control != Backup_Analog_Test_Control) {
				Backup_Analog_Test_Control = pEP932C_Registers->Analog_Test_Control;

				if(pEP932C_Registers->Analog_Test_Control & 0x01) {
					EP932_Reg_Set_Bit(EP932_Color_Space_Control, 0x01);
				}
				else {
					EP932_Reg_Clear_Bit(EP932_Color_Space_Control, 0x01);
				}
				if(pEP932C_Registers->Analog_Test_Control & 0x02) {
					EP932_Reg_Set_Bit(EP932_Color_Space_Control, 0x02);
				}
				else {
					EP932_Reg_Clear_Bit(EP932_Color_Space_Control, 0x02);
				}
			}
			break;
	}

	/* Return the status */
	if(pEP932C_Registers->Power_Control & (EP932E_Power_Control__PD_HDMI | EP932E_Power_Control__PD_TOT)) {
		return EP932C_TASK_Idle;
	}
	else {
		return EP932C_TASK_Pending;
	}
}

void TXS_RollBack_Wait_Upstream(void)
{
	DBG_printf(("\r\nState Rollback: Reset EDID -> [TXS_Search_EDID]\r\n"));

	/* Reset EDID */
	memset(pEP932C_Registers->Readed_EDID, 0xFF, 256);

	/* Report EDID Change */
	pEP932C_Registers->Interrupt_Flags |= EP932E_Interrupt_Flags__EDID_CHG;
	if(EP932C_GenerateInt && (pEP932C_Registers->Interrupt_Enable & EP932E_Interrupt_Enable__EDID_CHG) ) EP932C_GenerateInt();
	ReadEDID_TimeCount = 0;
}

void TXS_RollBack_Stream(void)
{
	DBG_printf(("\r\nState Rollback: Power Down -> [TXS_Wait_Upstream]\r\n"));

	/* Power Down */
	HDMI_Tx_Power_Down();

	/* Reset HDCP Info */
	memset(pEP932C_Registers->HDCP_BKSV, 0x00, sizeof(pEP932C_Registers->HDCP_BKSV));
	is_HDCP_Info_BKSV_Rdy = 0;
}

void TXS_RollBack_HDCP(void)
{
	DBG_printf(("\r\nState Rollback: Stop HDCP -> [TXS_Stream]\r\n"));

	HDCP_Stop();
	pEP932C_Registers->HDCP_Status = 0;
	pEP932C_Registers->HDCP_State = 0;
}

/*----------------------------------------------------------------------------------------------------------------------*/

void ReadInterruptFlags(void)
{
	/*DBG_printf(("EP932 ReadInterruptFlags \r\n"));  */
	EP932_Reg_Read(EP932_General_Control_2, DDC_Data, 1);

	if(DDC_Data[0] & EP932_General_Control_2__RIF) {
		HDCP_Ext_Ri_Trigger();
		/* Clear the interrupt flag  */
		DDC_Data[0] = EP932_General_Control_2__RIF;
		EP932_Reg_Write(EP932_General_Control_2, DDC_Data, 1);
	}
	/*                                  */
	/* Clear the interrupt flag
	DDC_Data[0] = EP932_General_Control_2__RIF;
	EP932_Reg_Write(EP932_General_Control_2, DDC_Data, 1);
	*/
}

/*----------------------------------------------------------------------------------------------------------------------*/

void  EP_HDMI_DumpMessage(void)
{
	unsigned short Temp_USHORT;
	unsigned char temp_R[2];
	unsigned char reg_addr;
#if 1
	/* System Status  */
	DBG_printf(("\r\n\r\n======= Dump EP932E information =======\r\n"));

	DBG_printf(("\r\n[EDID Data]"));
	for(Temp_USHORT = 0; Temp_USHORT < 256; ++Temp_USHORT) {
		if(Temp_USHORT%16 == 0) DBG_printf(("\r\n"));
		if(Temp_USHORT%8 == 0) DBG_printf((" "));
		DBG_printf(("0x%02X,", (int)EP932C_Registers.Readed_EDID[Temp_USHORT] ));
	}
	DBG_printf(("\r\n"));

	DBG_printf(("\r\n[Revision & Configuration]\r\n"));
	DBG_printf(("VendorID=0x%04X, ", EP932C_Registers.VendorID ));
	DBG_printf(("DeviceID=0x%04X, ", EP932C_Registers.DeviceID ));
	DBG_printf(("Version=%d.%d, CFG=0x%02X\r\n", (int)EP932C_Registers.Version_Major, (int)EP932C_Registers.Version_Minor, (int)EP932C_Registers.Configuration ));

	DBG_printf(("\r\n[Interrupt Flags]\r\n"));
	DBG_printf(("EDID_CHG=%d, ", (int)((EP932C_Registers.Interrupt_Flags & EP932E_Interrupt_Flags__EDID_CHG)?1:0) ));
	DBG_printf(("VIDEO_CHG=%d, ", (int)((EP932C_Registers.Interrupt_Flags & EP932E_Interrupt_Flags__VIDEO_CHG)?1:0) ));
	DBG_printf(("AUDIO_CHG=%d\r\n", (int)((EP932C_Registers.Interrupt_Flags & EP932E_Interrupt_Flags__AUDIO_CHG)?1:0) ));

	DBG_printf(("\r\n[System Status]\r\n"));
	DBG_printf(("RSEN=%d, ", (int)((EP932C_Registers.System_Status & EP932E_System_Status__RSEN)?1:0) ));
	DBG_printf(("HTPLG=%d, ", (int)((EP932C_Registers.System_Status & EP932E_System_Status__HTPLG)?1:0) ));
	DBG_printf(("KEY_FAIL=%d, ", (int)((EP932C_Registers.System_Status & EP932E_System_Status__KEY_FAIL)?1:0) ));
	DBG_printf(("DEF_KEY=%d\r\n", (int)((EP932C_Registers.System_Status & EP932E_System_Status__DEF_KEY)?1:0) ));

	DBG_printf(("\r\n[EDID Status]\r\n"));
	DBG_printf(("EDID_HDMI=%d, ", (int)((EP932C_Registers.EDID_Status & EP932E_EDID_Status__HDMI)?1:0) ));
	DBG_printf(("DDC_STATUS=%d\r\n", (int)(EP932C_Registers.EDID_Status & 0x0F) ));
	DBG_printf(("VIDEO_DATA_ADDR=0x%02X, ", (int)EP932C_Registers.EDID_VideoDataAddr ));
	DBG_printf(("AUDIO_DATA_ADDR=0x%02X, ", (int)EP932C_Registers.EDID_AudioDataAddr ));
	DBG_printf(("SPEAKER_DATA_ADDR=0x%02X, ", (int)EP932C_Registers.EDID_SpeakerDataAddr ));
	DBG_printf(("VENDOR_DATA_ADDR=0x%02X\r\n", (int)EP932C_Registers.EDID_VendorDataAddr ));
	DBG_printf(("ASFREQ=0x%02X, ", (int)EP932C_Registers.EDID_ASFreq ));
	DBG_printf(("ACHANNEL=%d\r\n", (int)EP932C_Registers.EDID_AChannel ));

	DBG_printf(("\r\n[Video Status]\r\n"));
	DBG_printf(("Interface=0x%02X, ", (int)EP932C_Registers.Video_Params_Backup.Interface ));
	DBG_printf(("VideoSettingIndex=%d, ", (int)EP932C_Registers.Video_Params_Backup.VideoSettingIndex ));
	DBG_printf(("HVPol=%d, ", (int)EP932C_Registers.Video_Params_Backup.HVPol ));
	DBG_printf(("SyncMode=%d, ", (int)EP932C_Registers.Video_Params_Backup.SyncMode ));
	DBG_printf(("FormatIn=%d, ", (int)EP932C_Registers.Video_Params_Backup.FormatIn ));
	DBG_printf(("FormatOut=%d, ", (int)EP932C_Registers.Video_Params_Backup.FormatOut ));
	DBG_printf(("ColorSpace=%d, ", (int)EP932C_Registers.Video_Params_Backup.ColorSpace ));
	DBG_printf(("AFARate=%d\r\n", (int)EP932C_Registers.Video_Params_Backup.AFARate ));

	DBG_printf(("\r\n[Audio Status]\r\n"));
	DBG_printf(("Interface=0x%02X, ", (int)EP932C_Registers.Audio_Params_Backup.Interface ));
	DBG_printf(("VideoSettingIndex=%d, ", (int)EP932C_Registers.Audio_Params_Backup.VideoSettingIndex ));
	DBG_printf(("ChannelNumber=%d, ", (int)EP932C_Registers.Audio_Params_Backup.ChannelNumber ));
	DBG_printf(("ADSRate=%d, ", (int)EP932C_Registers.Audio_Params_Backup.ADSRate ));
	DBG_printf(("InputFrequency=%d, ", (int)EP932C_Registers.Audio_Params_Backup.InputFrequency ));
	DBG_printf(("VFS=%d, ", (int)EP932C_Registers.Audio_Params_Backup.VFS ));
	DBG_printf(("NoCopyRight=%d\r\n", (int)EP932C_Registers.Audio_Params_Backup.NoCopyRight ));

	DBG_printf(("\r\n[Power Control]\r\n"));
	DBG_printf(("PD_HDMI=%d, ", (int)((EP932C_Registers.Power_Control & EP932E_Power_Control__PD_HDMI)?1:0) ));
	DBG_printf(("PD_TOT=%d\r\n", (int)((EP932C_Registers.Power_Control & EP932E_Power_Control__PD_TOT)?1:0) ));

	DBG_printf(("\r\n[System Configuration]\r\n"));
	DBG_printf(("HDCP_DIS=%d, ", (int)((EP932C_Registers.System_Configuration & EP932E_System_Configuration__HDCP_DIS)?1:0) ));
	DBG_printf(("HDMI_DIS=%d, ", (int)((EP932C_Registers.System_Configuration & EP932E_System_Configuration__HDMI_DIS)?1:0) ));
	DBG_printf(("AUDIO_DIS=%d, ", (int)((EP932C_Registers.System_Configuration & EP932E_System_Configuration__AUDIO_DIS)?1:0) ));
	DBG_printf(("VIDEO_DIS=%d\r\n", (int)((EP932C_Registers.System_Configuration & EP932E_System_Configuration__VIDEO_DIS)?1:0) ));

	DBG_printf(("\r\n[Interrupt Enable]\r\n"));
	DBG_printf(("EDID_CHG=%d, ", (int)((EP932C_Registers.Interrupt_Enable & EP932E_Interrupt_Enable__EDID_CHG)?1:0) ));
	DBG_printf(("VS_PERIOD_CHG=%d, ", (int)((EP932C_Registers.Interrupt_Enable & EP932E_Interrupt_Enable__VIDEO_CHG)?1:0) ));
	DBG_printf(("AS_FREQ_CHG=%d\r\n", (int)((EP932C_Registers.Interrupt_Enable & EP932E_Interrupt_Enable__AUDIO_CHG)?1:0) ));

	DBG_printf(("\r\n[Video Interface 0]\r\n"));
	DBG_printf(("DK=%d, ", (int)((EP932C_Registers.Video_Interface[0] & EP932E_Video_Interface_Setting_0__DK)?1:0) ));
	DBG_printf(("DKEN=%d, ", (int)((EP932C_Registers.Video_Interface[0] & EP932E_Video_Interface_Setting_0__DKEN)?1:0) ));
	DBG_printf(("DSEL=%d, ", (int)((EP932C_Registers.Video_Interface[0] & EP932E_Video_Interface_Setting_0__DSEL)?1:0) ));
	DBG_printf(("BSEL=%d, ", (int)((EP932C_Registers.Video_Interface[0] & EP932E_Video_Interface_Setting_0__BSEL)?1:0) ));
	DBG_printf(("EDGE=%d, ", (int)((EP932C_Registers.Video_Interface[0] & EP932E_Video_Interface_Setting_0__EDGE)?1:0) ));
	DBG_printf(("FMT12=%d\r\n", (int)((EP932C_Registers.Video_Interface[0] & EP932E_Video_Interface_Setting_0__FMT12)?1:0) ));

	DBG_printf(("\r\n[Video Interface 1]\r\n"));
	DBG_printf(("COLOR=%d, ", (int)((EP932C_Registers.Video_Interface[1] & EP932E_Video_Interface_Setting_1__COLOR)>>4) ));
	DBG_printf(("SYNC=%d, ", (int)((EP932C_Registers.Video_Interface[1] & EP932E_Video_Interface_Setting_1__SYNC)>>2) ));
	DBG_printf(("VIN_FMT=%d\r\n", (int)((EP932C_Registers.Video_Interface[1] & EP932E_Video_Interface_Setting_1__VIN_FMT)>>0) ));

	DBG_printf(("\r\n[Audio Interface]\r\n"));
	DBG_printf(("CHANNEL=%d, ", (int) (EP932C_Registers.Audio_Interface & EP932E_Audio_Interface_Setting__CHANNEL)>>4 ));
	DBG_printf(("IIS=%d, ", (int)((EP932C_Registers.Audio_Interface & EP932E_Audio_Interface_Setting__IIS)?1:0) ));
	DBG_printf(("WS_M=%d, ", (int)((EP932C_Registers.Audio_Interface & EP932E_Audio_Interface_Setting__WS_M)?1:0) ));
	DBG_printf(("WS_POL=%d, ", (int)((EP932C_Registers.Audio_Interface & EP932E_Audio_Interface_Setting__WS_POL)?1:0) ));
	DBG_printf(("SCK_POL=%d\r\n", (int)((EP932C_Registers.Audio_Interface & EP932E_Audio_Interface_Setting__SCK_POL)?1:0) ));

	DBG_printf(("\r\n[Video Input Format 0]\r\n"));
	DBG_printf(("VIC=%d\r\n", (int)EP932C_Registers.Video_Input_Format[0] ));

	DBG_printf(("\r\n[Video Input Format 1]\r\n"));
	DBG_printf(("AFAR_VIF=0x%02X\r\n", (int)EP932C_Registers.Video_Input_Format[1] ));


	DBG_printf(("\r\n[EP932 Register value]"));
#endif
#if 1
	for(reg_addr = 0; reg_addr<= 0x88; reg_addr++)
	{
		EP932_Reg_Read(reg_addr, temp_R, 1);
		if(reg_addr%8 == 0)DBG_printf(("\r\n"));
		DBG_printf(("[%02X]%02X, ",(int)reg_addr,(int)temp_R[0]));
	}



#endif

	DBG_printf(("\r\n"));
}
