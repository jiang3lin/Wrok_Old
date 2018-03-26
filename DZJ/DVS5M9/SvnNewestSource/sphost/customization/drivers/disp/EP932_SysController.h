#ifndef EP932_API_H
#define EP932_API_H

#ifndef EP932CONTROLLER_H
#define EP932CONTROLLER_H

/****************************************************************************/
/* for EP Driver                                  */     
/*                                                 */
#include <stdio.h>
#include "common.h"
/*#include "../sensor/flow/hal_front.h"
#if 0
#include "../sensor/flow/hal_front_private.h"
#endif
#include "../sensor/flow/reg_front.h" */


#define DBG_printf(x)   printf  x
typedef unsigned char  BYTE;

/*#define DBG_printf(x)  */
typedef enum {
	TXS_Search_EDID,
	TXS_Wait_Upstream,
	TXS_Stream,
	TXS_HDCP,
} TX_STATE;


typedef enum {
	/* Master  */
	SMBUS_STATUS_Success = 0x00,
	SMBUS_STATUS_Pending,/*	SMBUS_STATUS_Abort,  */
	SMBUS_STATUS_NoAct = 0x02,
	SMBUS_STATUS_TimeOut,
	SMBUS_STATUS_ArbitrationLoss = 0x04
} SMBUS_STATUS;


typedef enum {
	AUD_Mute = 0,
	AUD_SF_32000Hz = 1,
	AUD_SF_44100Hz =2,
	AUD_SF_48000Hz =3,
	AUD_SF_88200Hz,
	AUD_SF_96000Hz,
	AUD_SF_176400Hz,
	AUD_SF_192000Hz
}HDMI_AudFreq;

typedef enum {
	Video_OUT_Auto	= 0,
	Video_OUT_YUV_444,
	Video_OUT_YUV_422,
	Video_OUT_RGB_444,
}HDMI_Out_format;


/**************************************************************************************/
/*
typedef enum{
	AudioMute_Disable = 0,
	AudioMute_Enable = 1
}CSTVOUT_Audio_MODE;
*/	

typedef enum {
	AUD_I2S = 0,
	AUD_SPDIF	
}HDMI_AudFmt_t;

#define DSEL_Dual_Edge		0x08
#define BSEL_24bit			0x04
#define EDGE_rising		0x02
#define FMT_12				0x01

/*  Controller  */
#define VERSION_MAJOR             0  /* Beta  */
#define VERSION_MINOR             38 /*      36  */

#define EP932C_TIMER_PERIOD       10		/*     The EP932Controller.c must be re-compiled if user want to change this value.  */
typedef enum {
	EP932C_TASK_Idle = 0,
	EP932C_TASK_Error,
	EP932C_TASK_Pending
} EP932C_TASK_STATUS;
typedef void (*EP932C_CALLBACK)(void);

#define EP932_ADDR				0x70
#define EP932_ADDR_2			0x72
#define HEY_ADDR				0xA8


typedef enum {
	COLORSPACE_601 = 1,
	COLORSPACE_709
} COLORSPACE;

typedef enum {
	SYNCMODE_HVDE = 0,
	SYNCMODE_HV,
	SYNCMODE_Embeded
} SYNCMODE;

typedef enum {
	COLORFORMAT_RGB = 0,
	COLORFORMAT_YCC444,
	COLORFORMAT_YCC422
} COLORFORMAT;

typedef enum {
	AFAR_VideoCode = 0,
	AFAR_4_3,
	AFAR_16_9,
	AFAR_14_9
} AFAR;

typedef enum {
	SCAN_NoData = 0,
	SCAN_OverScan,
	SCAN_UnderScan,
	SCAN_Future
} SCAN_MODE;



#define HDCP_ERROR_BKSV									0x80
#define HDCP_ERROR_AKSV									0x40
#define HDCP_ERROR_R0									0x20
#define HDCP_ERROR_Ri									0x10
#define HDCP_ERROR_RepeaterRdy							0x08
#define HDCP_ERROR_RepeaterSHA							0x04
#define HDCP_ERROR_RSEN									0x02
#define HDCP_ERROR_RepeaterMax							0x01

/* EDID status error code  */
typedef enum {
	/* Master  */
	EDID_STATUS_Success = 0x00,
	EDID_STATUS_Pending,/*	SMBUS_STATUS_Abort, */
	EDID_STATUS_NoAct = 0x02,
	EDID_STATUS_TimeOut,
	EDID_STATUS_ArbitrationLoss = 0x04,
	EDID_STATUS_ExtensionOverflow,
	EDID_STATUS_ChecksumError
} EDID_STATUS;

/* Video Output Congif Params  */
typedef struct _VDO_PARAMS {
/* Which cause Timing Change Reset  */
	unsigned char 	Interface;				/* DK[3:1], DKEN, DSEL, BSEL, EDGE, FMT12 */
	unsigned char 	VideoSettingIndex;	/* VIC  */
	unsigned char 	HVPol;					/* x, x, x, x, VSO_POL, HSO_POL, x, x  */
	SYNCMODE 		SyncMode;				/* 0 = HVDE, 1 = HV(DE Gen), 2 = Embedded Sync */
	COLORFORMAT 	FormatIn;				/* 0 = RGB, 1 = YCC444, 2 = YCC422  */
	COLORFORMAT 	FormatOut;				/* 0 = RGB, 1 = YCC444, 2 = YCC422  */
/* Which don't cause Timing Chage Reset  */
	COLORSPACE 		ColorSpace;			/* 0 = Auto, 1 = 601, 2 = 709  */
	AFAR 			AFARate;				/* 0 = Auto, 1 = 4:3, 2 = 16:9, 3 = 14:9 */
	SCAN_MODE   	SCAN;					/* 0 = no data, 1 = overscan, 2 = underscan, 3 = future  */
} VDO_PARAMS, *PVDO_PARAMS;

typedef enum {
	ADSFREQ_32000Hz = 0x03,
	ADSFREQ_44100Hz = 0x00,
	ADSFREQ_48000Hz = 0x02,
	ADSFREQ_88200Hz = 0x08,
	ADSFREQ_96000Hz = 0x0A,
	ADSFREQ_176400Hz = 0x0C,
	ADSFREQ_192000Hz = 0x0E
} ADSFREQ;

/* Audio Output Congif Params */
typedef struct _ADO_PARAMS {
	unsigned char 		Interface;				/* x, x, x, x, IIS, WS_M, WS_POL, SCK_POL */
	unsigned char 		VideoSettingIndex;		/* VIC */
	unsigned char 		ChannelNumber;			/* 1 = 2 ch, 2 = 3 ch, ... , 5 = 5.1 ch, 7 = 7.1 ch  */
	unsigned char 		ADSRate;				/* 1 = SF/2, 2 = SF/3, 3 = SF/4 (Down Sample) */
	ADSFREQ		InputFrequency;			/* ADSFREQ */
	unsigned char       	VFS; 					/* 0 = 59.94Hz, 1 = 60Hz (Vertical Frequency Shift of Video) */
	unsigned char 		NoCopyRight;
} ADO_PARAMS, *PADO_PARAMS;


#define HDCP_TIMER_PERIOD 					10		/*    */

/* HDCP Transmiter Link State */
typedef enum {
	A0_Wait_for_Active_Rx,
	A1_Exchange_KSVs,
	A2_Computations,
	A3_Validate_Receiver,
	A4_Authenticated,
	A5_Link_Integrity_Check,
	A6_Test_for_Repeater,
	A8_Wait_for_READY,
	A9_Read_KSV_List
} HDCP_STATE;

typedef struct _EP932C_REGISTER_MAP {

	/* Read */
	unsigned short		VendorID;			/* 0x00 */
	unsigned short		DeviceID;
	unsigned char		Version_Major;
	unsigned char		Version_Minor;
	unsigned char		Configuration;

	unsigned char		Interrupt_Flags;		/* 0x01 */

	unsigned char		System_Status;			/* 0x02 */

	unsigned char		HDCP_Status;			/* 0x03 */
	unsigned char		HDCP_State;
	unsigned char		HDCP_AKSV[5];
	unsigned char		HDCP_BKSV[5];
	unsigned char		HDCP_BCAPS3[3];
	unsigned char		HDCP_KSV_FIFO[5*16];
	unsigned char		HDCP_SHA[20];
	unsigned char		HDCP_M0[8];

	unsigned char		EDID_Status;			/* 0x04 */
	unsigned char		EDID_VideoDataAddr;
	unsigned char		EDID_AudioDataAddr;
	unsigned char		EDID_SpeakerDataAddr;
	unsigned char		EDID_VendorDataAddr;
	unsigned char		EDID_ASFreq;
	unsigned char		EDID_AChannel;
						
	/*unsigned short		VS_Period;	*/			/* 0x05 (Video Status) */
	/*unsigned short		H_Res; */
	/*unsigned short		V_Res; */
	/*unsigned short		Ratio_24; */
	VDO_PARAMS 			Video_Params_Backup;

	/*unsigned short		AS_Freq;			*/	/* 0x06 (Audio Status) */
	/*unsigned short		AS_Period;				*/ 
	ADO_PARAMS 			Audio_Params_Backup;

	unsigned char		Readed_EDID[256];		/* 0x07 */

	/* Read / Write */
	unsigned char		Analog_Test_Control;	/* 0X1C */

	unsigned char		Power_Control;			/* 0x20 */
	unsigned char		System_Configuration;

	unsigned char		Interrupt_Enable;		/* 0x21 */

	unsigned char		Video_Interface[2];		/* 0x22 */

	unsigned char		Audio_Interface;		/* 0x23 */

	unsigned char		Video_Input_Format[2];	/* 0x24 */

	unsigned char 		Video_Output_Format;

	unsigned char		Audio_Input_Format;		/* 0x25 */

	unsigned char		End;

} EP932C_REGISTER_MAP, *PEP932C_REGISTER_MAP;

/* ----------------------------------------------------------------------------- */
/* ----------------------------------------------------------------------------- */



void EP932Controller_Initial(PEP932C_REGISTER_MAP pEP932C_RegMap, EP932C_CALLBACK IntCall);

unsigned char EP932Controller_Task(void);

void EP932Controller_Timer(void);
void  EP_HDMI_DumpMessage(void);

/* ----------------------------------------------------------------------------- */

void EP_EP932M_Reset(void);
void  EP_HDMI_Set_Audio_Interface(HDMI_AudFmt_t  Audfmt, HDMI_AudFreq  Audfreq);
void  EP_HDMI_Set_Video_Timing(int Timing);
void EP_HDMI_Set_Video_Output(int HDMI_output);

unsigned char EP_HDMI_Init(void); 

#endif
#endif 

#ifndef HDCP_H
#define HDCP_H

/*#define EXT_RICMP_TRIGGER */


extern HDCP_STATE HDCP_Authentication_Task(unsigned char ReceiverRdy);
extern void HDCP_Stop(void);
extern unsigned char HDCP_Get_Status(void);
extern void HDCP_Timer(void);
extern void HDCP_Ext_Ri_Trigger(void);

/* Special Functions */
extern void HDCP_Assign_RKSV_List(unsigned char *pRevocationList, unsigned char ListNumber);
extern void HDCP_Fake(unsigned char Enable);
extern void HDCP_Extract_BKSV_BCAPS3(unsigned char *pBKSV_BCaps3);
extern void HDCP_Extract_FIFO(unsigned char *pFIFO, unsigned char ListNumber);
extern void HDCP_Extract_SHA_M0(unsigned char *pSha_M0);

#endif /* HDCP_H */

#ifndef EDID_H
#define EDID_H

#define EDID_BLOCK_SIZE  128

/*#define max(a,b) (((a)>(b))? (a):(b)) */


/*                        */
/* Structure Definitions  */
/*                        */

extern unsigned char EDID_GetHDMICap(unsigned char *pTarget);
extern unsigned char EDID_GetPCMFreqCap(unsigned char *pTarget);
extern unsigned char EDID_GetPCMChannelCap(unsigned char *pTarget);
extern unsigned char EDID_GetDataBlockAddr(unsigned char *pTarget, unsigned char Tag);

#endif /* EDID_H  */

#ifndef DDC_IF_H
#define DDC_IF_H

/*#include "type.h"  */

/*#include "EPcsapi.h" */


/*------------------------------------------------------------------------------------------------*/

#define HDCP_RX_ADDR          0x74     /* HDCP RX Address */
#define EDID_ADDR       		0xA0     /* EDID Address  */
#define EDID_SEGMENT_PTR		0x60

#define HDCP_RX_BKSV_ADDR       0x00     /* HDCP RX, BKSV Register Address */
#define HDCP_RX_RI_ADDR         0x08     /* HDCP RX, RI Register Address  */
#define HDCP_RX_AKSV_ADDR       0x10     /* HDCP RX, AKSV Register Address */
#define HDCP_RX_AINFO_ADDR      0x15     /* HDCP RX, AINFO Register Address  */
#define HDCP_RX_AN_ADDR         0x18     /* HDCP RX, AN Register Address  */
#define HDCP_RX_SHA1_HASH_ADDR  0x20     /* HDCP RX, SHA-1 Hash Value Start Address */
#define HDCP_RX_BCAPS_ADDR      0x40     /* HDCP RX, BCAPS Register Address  */
#define HDCP_RX_BSTATUS_ADDR    0x41     /* HDCP RX, BSTATUS Register Address  */
#define HDCP_RX_KSV_FIFO_ADDR   0x43     /* HDCP RX, KSV FIFO Start Address  */

/*-------------------------------------------------------------------------------------------------*/
#define READ8(a) (*((volatile UINT8*)(a)))
#define WRITE8(a,b)	*(volatile UINT8*)(a) = (b)


/*=================================================================================================*/
/*                                    */
/* Protected Data Member               */
/*                                    */
extern SMBUS_STATUS status; 
extern unsigned char DDC_Data[128]; /* The DDC Buffer  */




/*================================================================================================*/

/*  Public Functions  */


/*-------------------------------------------------------------------------------------------------*/

/* General  */


/* All Interface Inital  */
/*extern void DDC_If_Initial(CSI2C_HANDLE E_handle, CSI2C_HANDLE H_handle);  */


/*-------------------------------------------------------------------------------------------------*/
/*                                                                        */
/* Downstream HDCP Control Interface                                      */
/*                                                                        */

extern unsigned char Downstream_Rx_read_BKSV(unsigned char *pBKSV);
extern unsigned char Downstream_Rx_BCAPS(void);
extern void Downstream_Rx_write_AINFO(char ainfo);
extern void Downstream_Rx_write_AN(unsigned char *pAN);
extern void Downstream_Rx_write_AKSV(unsigned char *pAKSV);
extern unsigned char Downstream_Rx_read_RI(unsigned char *pRI);
extern void Downstream_Rx_read_BSTATUS(unsigned char *pBSTATUS);
extern void Downstream_Rx_read_SHA1_HASH(unsigned char *pSHA);
extern unsigned char Downstream_Rx_read_KSV_FIFO(unsigned char *pBKSV, unsigned char Index, unsigned char DevCount);


/*--------------------------------------------------------------------------------------------------*/
/*                               */
/* Downstream EDID Control Interface       */
/*                                                                                                  */

extern unsigned char Downstream_Rx_poll_EDID(void);
extern EDID_STATUS Downstream_Rx_read_EDID(unsigned char *pEDID);
void HDMI_i2c_read(UINT8 regaddr,UINT8  *pregdata,UINT8  cnt,UINT8  mode,UINT8  sync	);

void HDMI_i2c_write(UINT8 *pregaddr,UINT8 *pregdata,UINT8 cnt,UINT8 mode,UINT8 sync);



#endif /* DDC_IF_H  */

/*  EP932_IF  */

/*=================================================================================================*/
            
/* Public Functions  */


/*--------------------------------------------------------------------------------------------------*/

/* General   */
/*                                          */

/* All Interface Inital  */
/*extern void EP932_If_Initial(CSI2C_HANDLE E_handle, CSI2C_HANDLE K_handle);  */
extern void EP932_If_Initial();
extern void EP932_If_Reset(void);


/*-------------------------------------------------------------------------------------------------*/
/* HDMI Transmiter Interface                       */
/* Common                                           */                                                

extern void HDMI_Tx_Power_Down(void);
extern void HDMI_Tx_Power_Up(void);
extern void HDMI_Tx_HDMI(void);
extern void HDMI_Tx_DVI(void);
extern unsigned char HDMI_Tx_HTPLG(void);
extern unsigned char HDMI_Tx_RSEN(void);

/* HDCP  */
extern void HDMI_Tx_Mute_Enable(void);
extern void HDMI_Tx_Mute_Disable(void);
extern void HDMI_Tx_HDCP_Enable(void);
extern void HDMI_Tx_HDCP_Disable(void);
extern void HDMI_Tx_RPTR_Set(void);
extern void HDMI_Tx_RPTR_Clear(void);
extern unsigned char HDMI_Tx_RI_RDY(void);
extern void HDMI_Tx_write_AN(unsigned char *pAN);
extern unsigned char HDMI_Tx_AKSV_RDY(void);
extern unsigned char HDMI_Tx_read_AKSV(unsigned char *pAKSV);
extern void HDMI_Tx_write_BKSV(unsigned char *pBKSV);
extern unsigned char HDMI_Tx_read_RI(unsigned char *pRI);
extern void HDMI_Tx_read_M0(unsigned char *pM0);
extern SMBUS_STATUS HDMI_Tx_Get_Key(unsigned char *Key);

/* Special for EP932E    */
extern void HDMI_Tx_AMute_Enable(void);
extern void HDMI_Tx_AMute_Disable(void);
extern void HDMI_Tx_VMute_Enable(void);
extern void HDMI_Tx_VMute_Disable(void);
extern void HDMI_Tx_Video_Config(PVDO_PARAMS Params);
extern void HDMI_Tx_Audio_Config(PADO_PARAMS Params);


/*------------------------------------------------------------------------------------------------*/

/* Hardware Interface  */


/* EP932                                                                                 */
extern SMBUS_STATUS Key_Read(unsigned char ByteAddr, void *Data, unsigned int Size);

extern SMBUS_STATUS Key_Write(unsigned char ByteAddr, void *Data, unsigned int Size);

extern SMBUS_STATUS EP932_Reg_Read(unsigned char ByteAddr, unsigned char *Data, unsigned int Size);
extern SMBUS_STATUS EP932_Reg_Write(unsigned char ByteAddr, unsigned char *Data, unsigned int Size);
extern SMBUS_STATUS EP932_Reg_Set_Bit(unsigned char ByteAddr, unsigned char BitMask);
extern SMBUS_STATUS EP932_Reg_Clear_Bit(unsigned char ByteAddr, unsigned char BitMask);
void HDMI2_i2c_read(UINT8 regaddr,UINT8	*pregdata,UINT8  cnt,UINT8	mode,UINT8	sync   );

void HDMI2_i2c_write(UINT8 *pregaddr,UINT8 *pregdata,UINT8 cnt,UINT8 mode,UINT8 sync);








