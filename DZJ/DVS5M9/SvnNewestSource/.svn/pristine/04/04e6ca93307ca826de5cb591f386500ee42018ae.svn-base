
#ifndef EP932SETTINGDATA_H
#define EP932SETTINGDATA_H

/* ----------------------------------------------------------------------------- */

#define EP932_VDO_Settings_IT_Start 76

/* Definition of H/V Polarity  */
#define  VPosHPos               0x00
#define  VPosHNeg               0x04
#define  VNegHPos               0x08
#define  VNegHNeg               0x0C

/* Pixel Freq Type */
typedef enum {
	PIX_FREQ_25175KHz = 0,
	PIX_FREQ_25200KHz,

	PIX_FREQ_27000KHz,
	PIX_FREQ_27027KHz,

	PIX_FREQ_54000KHz,
	PIX_FREQ_54054KHz,

	PIX_FREQ_72000KHz,

	PIX_FREQ_74176KHz,
	PIX_FREQ_74250KHz,

	PIX_FREQ_108000KHz,
	PIX_FREQ_108108KHz,

	PIX_FREQ_148352KHz,
	PIX_FREQ_148500KHz,

	PIX_FREQ_PC

} PIX_FREQ_TYPE;

/*
 Index = [Video Code]
*/
typedef struct _HVRES_TYPE {
   unsigned char  HVPol;
   unsigned short Hres;
   unsigned short Vres;
   unsigned short Vprd;
} HVRES_TYPE, *PHVRES_TYPE;

/* DE Generation  */
typedef struct _DE_GEN_SETTINGS { /* VideoCode to */
	unsigned short DE_DLY;
	unsigned short DE_CNT;
	unsigned char  DE_TOP;
	unsigned short DE_LIN;
} DE_GEN_SETTINGS, *PDE_GEN_SETTINGS;

/* Embeded Sybc */
typedef struct _E_SYNC_SETTINGS { /* VideoCode to */
	unsigned char  CTL;
	unsigned short H_DLY;
	unsigned short H_WIDTH;
	unsigned char  V_DLY;
	unsigned char  V_WIDTH;
	unsigned short V_OFST;
} E_SYNC_SETTINGS, *PE_SYNC_SETTINGS;

/* AVI Settings  */
typedef struct _VDO_SETTINGS { 
	unsigned char	VideoCode;
	HVRES_TYPE      HVRes_Type;
	DE_GEN_SETTINGS DE_Gen;
	E_SYNC_SETTINGS E_Sync; /* (HV_Gen)  */
	unsigned char   AR_PR;
	PIX_FREQ_TYPE   Pix_Freq_Type;
} VDO_SETTINGS, *PVDO_SETTINGS;

extern VDO_SETTINGS EP932_VDO_Settings[];
extern unsigned char EP932_VDO_Settings_Max; /* = (sizeof(EP932_VDO_Settings)/sizeof(EP932_VDO_Settings[0]))  */

/* ----------------------------------------------------------------------------- */


/* Index = [Channel Number]  */

/* Audio Channel and Allocation  */
typedef struct _ADO_SETTINGS { /* IIS ChannelNumber to  */
	unsigned char SpeakerMapping;
	unsigned char Flat;
} ADO_SETTINGS, *PADO_SETTINGS;

extern ADO_SETTINGS EP932_ADO_Settings[];

/* -----------------------------------------------------------------------------*/


/* Index = [Pixel Freq Type]  */

/* N and CTS  */
typedef struct _N_CTS_SETTINGS { /* IIS ChannelNumber to  */
	unsigned long N;
	unsigned long CTS; /* Use hardware to calculate the CTS  */
} N_CTS_SETTINGS, *PN_CTS_SETTINGS;

extern N_CTS_SETTINGS N_CTS_32K[];
extern N_CTS_SETTINGS N_CTS_44K1[];
extern N_CTS_SETTINGS N_CTS_48K[];


#endif


#ifndef EP932REGDEF_H
#define EP932REGDEF_H

/* Registers								Word	BitMask  */
#define EP932_SMPRD							0x06			/* 2 Byte */

#define EP932_General_Control_1				0x08
#define EP932_General_Control_1__TSEL_HTP			0x80
#define EP932_General_Control_1__INT_OD				0x40
#define EP932_General_Control_1__INT_POL			0x20
#define EP932_General_Control_1__VTX				0x10
#define EP932_General_Control_1__DSEL				0x08
#define EP932_General_Control_1__BSEL				0x04
#define EP932_General_Control_1__EDGE				0x02
#define EP932_General_Control_1__PU					0x01

#define EP932_General_Control_2				0x09
#define EP932_General_Control_2__RSEN				0x80
#define EP932_General_Control_2__HTPLG				0x40
#define EP932_General_Control_2__RIE				0x20
#define EP932_General_Control_2__VIE				0x10
#define EP932_General_Control_2__MIE				0x08
#define EP932_General_Control_2__RIF				0x04
#define EP932_General_Control_2__VIF				0x02
#define EP932_General_Control_2__MIF				0x01

#define EP932_General_Control_3				0x0A	

#define EP932_Configuration					0x0B

#define EP932_Color_Space_Control			0x0C
#define EP932_Color_Space_Control__422_OUT			0x80
#define EP932_Color_Space_Control__YCC_OUT			0x40
#define EP932_Color_Space_Control__COLOR			0x20
#define EP932_Color_Space_Control__YCC_Range		0x10
#define EP932_Color_Space_Control__VMUTE			0x08
#define EP932_Color_Space_Control__AMUTE			0x04
#define EP932_Color_Space_Control__TREG				0x03
		
#define EP932_Pixel_Repetition_Control		0x0D	
#define EP932_Pixel_Repetition_Control__CS_M		0x80
#define EP932_Pixel_Repetition_Control__CTS_M		0x40
#define EP932_Pixel_Repetition_Control__ADSR		0x30
#define EP932_Pixel_Repetition_Control__OSCSEL		0x08
#define EP932_Pixel_Repetition_Control__VSYNC		0x04	
#define EP932_Pixel_Repetition_Control__PR			0x03

#define EP932_General_Control_4				0x0E
#define EP932_General_Control_4__FMT12				0x80
#define EP932_General_Control_4__422_IN				0x40
#define EP932_General_Control_4__YCC_IN				0x20
#define EP932_General_Control_4__E_SYNC				0x10
#define EP932_General_Control_4__VPOL_DET			0x08
#define EP932_General_Control_4__HPOL_DET			0x04
#define EP932_General_Control_4__EESS				0x02
#define EP932_General_Control_4__HDMI				0x01

#define EP932_General_Control_5				0x0F
#define EP932_General_Control_5__AKSV_RDY			0x80
#define EP932_General_Control_5__RPTR				0x10
#define EP932_General_Control_5__RI_RDY				0x02
#define EP932_General_Control_5__ENC_EN				0x01

#define EP932_BKSV							0x10			/* BKSV1-BKSV5 0x10-0x14 */

#define EP932_AN							0x15			/* AN1-AN8 0x15-0x1C  */

#define EP932_AKSV							0x1D			/* AKSV1-AKSV5 0x1D-0x21  */

#define EP932_RI							0x22			/* RI1-RI2 0x22-0x23  */

#define EP932_M0							0x25			/* 0x25-0x32  */

#define EP932_DE_DLY						0x32			/* 10 bit  */

#define EP932_DE_Control					0x33			/* 10 bit  */
#define EP932_DE_Control__DE_GEN					0x40
#define EP932_DE_Control__VSO_POL					0x08
#define EP932_DE_Control__HSO_POL					0x04

#define EP932_DE_TOP						0x34			/* 6 bit  */

#define EP932_DE_CNT						0x36			/* 10 bit  */

#define EP932_DE_LIN						0x38			/* 10 bit  */

#define EP932_H_RES							0x3A			/* 11 bit  */

#define EP932_V_RES							0x3C			/* 11 bit  */

#define EP932_Audio_Subpacket_Allocation	0x3E			/* Default 0xE4  */

#define EP932_IIS_Control					0x3F			/* Default 0x00  */
#define EP932_IIS_Control__ACR_EN					0x80
#define EP932_IIS_Control__AVI_EN					0x40
#define EP932_IIS_Control__ADO_EN					0x20
#define EP932_IIS_Control__AUDIO_EN					0x10
#define EP932_IIS_Control__WS_M						0x04
#define EP932_IIS_Control__WS_POL					0x02
#define EP932_IIS_Control__SCK_POL					0x01

#define EP932_Packet_Control				0x40			/* Default 0x00  */
#define EP932_Packet_Control__FLAT3					0x80
#define EP932_Packet_Control__FLAT2					0x40
#define EP932_Packet_Control__FLAT1					0x20
#define EP932_Packet_Control__FLAT0					0x10
#define EP932_Packet_Control__LAYOUT				0x08
#define EP932_Packet_Control__IIS					0x04
#define EP932_Packet_Control__PKT_RDY				0x01

#define EP932_Data_Packet_Header 			0x41			/* HB0-HB2 0x41-0x43  */

#define EP932_Data_Packet 					0x44			/* PB0-PB27 0x44-0x5F  */

#define EP932_CTS		 					0x60			/* 20bit (3 Byte)  */

#define EP932_N			 					0x63			/* 20bit (3 Byte)  */

#define EP932_AVI_Packet 					0x66			/* 14 Byte 0x66-0x73  */

#define EP932_ADO_Packet 					0x74			/* 6 Byte 0x74-0x79  */

#define EP932_SPDIF_Sampling_Frequency 		0x7A			/* 1 Byte  */

#define EP932_Channel_Status		 		0x7B			/* 5 Byte 0x7B-0x7F  */

#define EP932_Embedded_Sync			 		0x80			/* Default 0x00  */

#define EP932_H_Delay			 			0x81			/* 10 bit (2 Byte)  */

#define EP932_H_Width			 			0x83			/* 10 bit (2 Byte)  */

#define EP932_V_Delay			 			0x85			/* 6 bit  */

#define EP932_V_Width			 			0x86			/* 6 bit  */

#define EP932_V_Off_Set			 			0x87			/* 12 bit (2 Byte)  */

#define EP932_Key_Add			 			0xF0			/* 1 Byte  */

#define EP932_Key_Data			 			0xF1			/* 7 Byte  */

#endif


#ifndef _TYPE_H
#define _TYPE_H


/* =============================================================================*/
/* =============================================================================*/
/*
typedef unsigned char BYTE;
typedef BYTE *PBYTE;
typedef unsigned int WORD;
typedef unsigned int *PWORD;
typedef unsigned long DWORD;
typedef DWORD *PDWORD;

typedef unsigned long ULONG;
typedef unsigned long *PULONG;
typedef unsigned short USHORT;
typedef USHORT *PUSHORT;
typedef unsigned char UCHAR;
typedef UCHAR *PUCHAR;

*/

/* =============================================================================*/
/* ============================================================================= */ 

/*typedef void (*FVN)(void); */

/* =============================================================================*/
/* =============================================================================*/

/*#define FALSE               (0)  
#define TRUE                (!FALSE)
#define OFF                 (0)
#define ON                  (!OFF)
#define ARRAYSIZE(ary)      (sizeof(ary)/sizeof(ary[0]))  */

/* =============================================================================*/
/* =============================================================================*/

/*for big endian */
/*#define LOBYTE(x)           (BYTE)((x)>>8) */
/*#define HIBYTE(x)           (BYTE)((x) & 0x00FF) */
/*#define MKWORD(hi,lo)       (WORD)(((lo)<<8) | hi)              */       

/* =============================================================================  */
/* =============================================================================  */

#define setb0(x)                (x |= 0x01)
#define setb1(x)                (x |= 0x02)
#define setb2(x)                (x |= 0x04)
#define setb3(x)                (x |= 0x08)
#define setb4(x)                (x |= 0x10)
#define setb5(x)                (x |= 0x20)
#define setb6(x)                (x |= 0x40)
#define setb7(x)                (x |= 0x80)

#define clrb0(x)                (x &= 0xFE)
#define clrb1(x)                (x &= 0xFD)
#define clrb2(x)                (x &= 0xFB)
#define clrb3(x)                (x &= 0xF7)
#define clrb4(x)                (x &= 0xEF)
#define clrb5(x)                (x &= 0xDF)
#define clrb6(x)                (x &= 0xBF)
#define clrb7(x)                (x &= 0x7F)

#define getb0(x)                (x & 0x01)
#define getb1(x)                ((x & 0x02)>>1)
#define getb2(x)                ((x & 0x04)>>2)
#define getb3(x)                ((x & 0x08)>>3)
#define getb4(x)                ((x & 0x10)>>4)
#define getb5(x)                ((x & 0x20)>>5)
#define getb6(x)                ((x & 0x40)>>6)
#define getb7(x)                ((x & 0x80)>>7)


/* =============================================================================*/
/* =============================================================================*/

/* math */
#ifndef min
#define min(a,b) (((a)<(b))? (a):(b))
#endif

#ifndef max
#define max(a,b) (((a)>(b))? (a):(b))
#endif

#endif

#ifndef EP932EREGDEF_H
#define Ep932Eregdef_H

/* Registers	 							Address		BitMask		Value     math */

#define EP932E_Vendor_ID						0x0000
#define EP932E_Device_ID						0x0002
#define EP932E_Firmware_Revision__Major			0x0004
#define EP932E_Firmware_Revision__Minor			0x0005

#define EP932E_Interrupt_Flags					0x0100
#define EP932E_Interrupt_Flags__EDID_CHG					0x80
#define EP932E_Interrupt_Flags__VIDEO_CHG					0x40
#define EP932E_Interrupt_Flags__AUDIO_CHG					0x20
#define EP932E_Interrupt_Flags__VS_ALIGN_FAILED				0x10
#define EP932E_Interrupt_Flags__VS_ALIGN_DONE				0x08

#define EP932E_System_Status					0x0200
#define EP932E_System_Status__RSEN							0x80
#define EP932E_System_Status__HTPLG							0x40
#define EP932E_System_Status__KEY_FAIL						0x02
#define EP932E_System_Status__DEF_KEY						0x01

#define EP932E_HDCP_Status						0x0300
#define EP932E_HDCP_Status__BKSV							0x80
#define EP932E_HDCP_Status__AKSV							0x40
#define EP932E_HDCP_Status__R0								0x20
#define EP932E_HDCP_Status__Ri								0x10
#define EP932E_HDCP_Status__RepeaterRdy						0x08
#define EP932E_HDCP_Status__RepeaterSHA						0x04
#define EP932E_HDCP_Status__RSEN							0x02
#define EP932E_HDCP_Status__REVOKE							0x01

#define EP932E_HDCP_State						0x0301

#define EP932E_HDCP_AKSV						0x0302
 
#define EP932E_HDCP_BKSV						0x0307 

#define EP932E_HDCP_BCAPS						0x030C

#define EP932E_HDCP_BSTATUS						0x030D

#define EP932E_HDCP_KSV_FIFO					0x030F

#define EP932E_EDID_Status						0x0400
#define EP932E_EDID_Status__HDMI							0x10
#define EP932E_EDID_Status__DDC_Status						0x0F
typedef enum {
	EDID_DDC_Success = 0x00,
	EDID_DDC_Pending,
	EDID_DDC_NoAct = 0x02,
	EDID_DDC_TimeOut,
	EDID_DDC_ArbitrationLoss = 0x04,
	EDID_DDC_BlockNumber
} EDID_DDC_STATUS;
#define EP932E_EDID_Status_ASFreq				0x0401
#define EP932E_EDID_Status_AChannel				0x0402

#define EP932E_Video_Status_VS_Period			0x0500					/* 2 Byte  */

#define EP932E_Video_Status_H_RES				0x0502					/* 2 Byte  */

#define EP932E_Video_Status_V_RES				0x0504					/* 2 Byte  */

#define EP932E_Video_Status_RATIO_24			0x0506					/* 2 Byte */
#define EP932E_Video_Status_Params				0x0508					/* 8 Byte */

#define EP932E_Audio_Status_AS_Freq				0x0600					/* 2 Byte */
#define EP932E_Audio_Status_AS_Period			0x0602					/* 2 Byte */
#define EP932E_Audio_Status_Params				0x0604					/* 7 Byte */

#define EP932E_EDID_Data						0x0700					/* 256 Byte */

#define EP932E_Analog_Test_Control				0x1C00
#define EP932E_Analog_Test_Control__PREEMPHASIS				0x03

#define EP932E_SIP_Test_Control					0x1D00
#define EP932E_SIP_Test_Control__VS_ALIGN					0x08
#define EP932E_SIP_Test_Control__BIST						0x04
#define EP932E_SIP_Test_Control__ANA_TEST					0x02
#define EP932E_SIP_Test_Control__IIC_STOP 					0x01

#define EP932E_Power_Control					0x2000
#define EP932E_Power_Control__PD_HDMI						0x02
#define EP932E_Power_Control__PD_TOT						0x01

#define EP932E_System_Configuration				0x2001
#define EP932E_System_Configuration__PACKET_RDY				0x80
#define EP932E_System_Configuration__HDCP_DIS				0x20
#define EP932E_System_Configuration__HDMI_DIS				0x10
#define EP932E_System_Configuration__FORCE_HDMI_CAP			0x08
#define EP932E_System_Configuration__AUDIO_DIS				0x02
#define EP932E_System_Configuration__VIDEO_DIS				0x01

#define EP932E_Interrupt_Enable					0x2100
#define EP932E_Interrupt_Enable__EDID_CHG					0x80
#define EP932E_Interrupt_Enable__VIDEO_CHG					0x40
#define EP932E_Interrupt_Enable__AUDIO_CHG					0x20
#define EP932E_Interrupt_Enable__VS_ALIGN_DONE				0x08

#define EP932E_Video_Interface_Setting_0		0x2200
#define EP932E_Video_Interface_Setting_0__DK				0xE0
#define EP932E_Video_Interface_Setting_0__DKEN				0x10
#define EP932E_Video_Interface_Setting_0__DSEL				0x08
#define EP932E_Video_Interface_Setting_0__BSEL				0x04
#define EP932E_Video_Interface_Setting_0__EDGE				0x02
#define EP932E_Video_Interface_Setting_0__FMT12				0x01

#define EP932E_Video_Interface_Setting_1		0x2201
#define EP932E_Video_Interface_Setting_1__COLOR				0x30
#define EP932E_Video_Interface_Setting_1__COLOR__Auto					0x00
#define EP932E_Video_Interface_Setting_1__COLOR__601					0x10
#define EP932E_Video_Interface_Setting_1__COLOR__709					0x20
#define EP932E_Video_Interface_Setting_1__xvYCC_EN			0x80
#define EP932E_Video_Interface_Setting_1__SYNC				0x0C
#define EP932E_Video_Interface_Setting_1__SYNC__HVDE					0x00
#define EP932E_Video_Interface_Setting_1__SYNC__HV						0x04
#define EP932E_Video_Interface_Setting_1__SYNC__Embeded					0x08
#define EP932E_Video_Interface_Setting_1__VIN_FMT			0x03
#define EP932E_Video_Interface_Setting_1__VIN_FMT__RGB					0x00
#define EP932E_Video_Interface_Setting_1__VIN_FMT__YCC444				0x01
#define EP932E_Video_Interface_Setting_1__VIN_FMT__YCC422				0x02

#define EP932E_Audio_Interface_Setting			0x2300
#define EP932E_Audio_Interface_Setting__CHANNEL				0xF0
#define EP932E_Audio_Interface_Setting__IIS					0x08
#define EP932E_Audio_Interface_Setting__WS_M				0x04
#define EP932E_Audio_Interface_Setting__WS_POL				0x02
#define EP932E_Audio_Interface_Setting__SCK_POL				0x01

#define EP932E_Video_Input_Format_VIC			0x2400
#define EP932E_Video_Input_Format_1				0x2401
#define EP932E_Video_Input_Format_1__AFAR					0x30
#define EP932E_Video_Input_Format_1__AFAR__Auto							0x00
#define EP932E_Video_Input_Format_1__AFAR__4_3							0x10
#define EP932E_Video_Input_Format_1__AFAR__16_9							0x20
#define EP932E_Video_Input_Format_1__AFAR__14_9							0x30
#define EP932E_Video_Input_Format_1__VIF								0x08	 /*0x01 */  /*modify by eric.lu  */
#define EP932E_Video_Input_Format_1__SCAN								0x03	 		/*add by eric.lu  */

#define EP932E_Audio_Input_Format				0x2500
#define EP932E_Audio_Input_Format__NoCopyRight				0x10
#define EP932E_Audio_Input_Format__ADO_FREQ					0x07
#define EP932E_Audio_Input_Format__ADO_FREQ__Auto						0x00
#define EP932E_Audio_Input_Format__ADO_FREQ__32000Hz					0x01
#define EP932E_Audio_Input_Format__ADO_FREQ__44100Hz					0x02
#define EP932E_Audio_Input_Format__ADO_FREQ__48000Hz					0x03
#define EP932E_Audio_Input_Format__ADO_FREQ__88200Hz					0x04
#define EP932E_Audio_Input_Format__ADO_FREQ__96000Hz					0x05
#define EP932E_Audio_Input_Format__ADO_FREQ__176400Hz					0x06
#define EP932E_Audio_Input_Format__ADO_FREQ__192000Hz					0x07

#define EP932E_KSV_Revocation_List				0x2600

#define EP932E_Gamut_Packet_Header				0x2700
#define EP932E_Gamut_Packet_Data				0x2703

#define EP932E_Select_Packet_Header				0x2800
#define EP932E_Select_Packet_Data				0x2803

#endif
