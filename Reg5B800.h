//******************************************************************************
//	Reg5B800.H
//
//	Header file to contorol 5B800 Registers.
//******************************************************************************
#ifndef __REG5B800_H__
#define __REG5B800_H__

/* address 00h */
#define adrBAND					0x00
#define bmBAND					0x10
typedef enum {
	BAND_FM				= 0x00,
	BAND_AM				= 0x10
} typBAND;

#define adrMONO					0x00
#define bmMONO					0x04

#define adrMUTE					0x00
#define bmMUTE					0x02
#define MUTE_On					0x02
#define MUTE_Off				0x00

#define adrPE					0x00
#define bmPE					0x01
#define PE_On					0x01
#define PE_Off					0x00

/* address 01h */
#define adrRST_RFAGC			0x01
#define bmRST_RFAGC				0x40

#define adrLOSCL				0x01
#define bmLOSCL					0x10

#define adrSTATION_EN			0x01
#define bmSTATION_EN			0x08

#define adrXID					0x01
#define bmXID					0x04

#define adrXBIAS				0x01
#define bmXBIAS					0x02

#define adrXSEL					0x01
#define bmXSEL					0x01

/* address 02h */
#define adrPSY_L				0x02

/* address 03h */
#define adrPSY_H				0x03

/* address 04h */
#define adrPSY_F				0x04
#define bmPSY_F					0x0F

/* address 05h */
#define adrFO_SAT_POL			0x05
#define bmFO_SAT_POL			0x80

#define adrSTATION_RDY			0x05
#define bmSTATION_RDY			0x40

#define adrALIGN_DONE			0x05
#define bmALIGN_DONE			0x20

#define adrBMT_ON				0x05
#define bmBMT_ON				0x10

#define adrIM_ALIGN_RUN			0x05
#define bmIM_ALIGN_RUN			0x08

#define adrFO_SAT				0x05
#define bmFO_SAT				0x04

#define adrSTATION				0x05
#define bmSTATION				0x02

#define adrSTEREO				0x05
#define bmSTEREO				0x01

/* address 06h */
#define adrLOCK57K				0x06
#define bmLOCK57K				0x80

#define adrRSSI_RD				0x06
#define bmRSSI_RD				0x7F




/* address 0Ch */
#define adrAAUTO				0x0C
#define bmAAUTO					0x80
typedef enum {
	AAUTO_Manual		= 0x00,
	AAUTO_Auto			= 0x80
} typAAUTO;

/* address 0Dh */
#define adrDSP_RST				0x0D
#define bmDSP_RST				0x80
typedef enum {
	DSP_RST_Enable		= 0x80,
	DSP_RST_Disable		= 0x00
} typDSP_RST;

#define adrDEMP					0x0D
#define bmDEMP					0x40
#define adrDEMP2				0x35
#define bmDEMP2					0x01
typedef enum {
	DEMP_75us			= 0x00,
	DEMP_50us			= 0x40,
	DEMP_60us			= 0x41
} typDEMP;

#define adrIIS_LRP				0x0D
#define bmIIS_LRP				0x20
typedef enum {
	IIS_Normal			= 0x00,
	IIS_Reverse			= 0x20
} typIIS_LRP;

#define adrIIS_SEL				0x0D
#define bmIIS_SEL				0x18
typedef enum {
	IIS_Output_Off		= 0x00,
	IIS_Iis_Format		= 0x08,
	IIS_Left_Adjust		= 0x10,
	IIS_Right_Adjust	= 0x18
} typIIS_SEL;

#define adrIIS_32_64			0x0D
#define bmIIS_32_64				0x04
typedef enum {
	IIS_32fs			= 0x00,
	IIS_64fs			= 0x04
} typIIS_32_64;

#define adrIIS_EN				0x0D
#define bmIIS_EN				0x03
typedef enum {
	IIS_Dac_Out			= 0x00,
	IIS_Iis_Out			= 0x01,
	IIS_Spdif_In		= 0x02,
	IIS_Audio_Off		= 0x03
} typIIS_EN;

/* address 0Eh */
#define adrFM_OV				0x0E
#define bmFM_OV					0x80
typedef enum {
	FM_OV_Normal		= 0x00,
	FM_OV_Over			= 0x80
} typFM_OV;

#define adrBW					0x0E
#define bmBW					0x60
typedef enum {
	BW_75KHz			= 0x00,
	BW_105KHz			= 0x20,
	BW_130KHz			= 0x40,
	BW_160KHz			= 0x60
} typBW;

#define adrLPFS_SEL				0x0E
#define bmLPFS_SEL				0x0C
//typedef enum {
//	LPFS_2_0KHz			= 0x00,
//	LPFS_1_5KHz			= 0x04,
//	LPFS_1_0KHz			= 0x08,
//	LPFS_Through		= 0x0C
//} typLPFS_SEL;

#define adrHPFS_SEL				0x0E
#define bmHPFS_SEL				0x03
//typedef enum {
//	HPFS_50Hz			= 0x00,
//	HPFS_100Hz			= 0x01,
//	HPFS_150Hz			= 0x02
//} typHPFS_SEL;

/* address 15h */
#define adrFO_SAT_SEL			0x15
#define bmFO_SAT_SEL			0xC0
typedef enum {
	FO_SAT_10k		= 0x00,
	FO_SAT_15k		= 0x40,
	FO_SAT_20k		= 0x80,
	FO_SAT_25k		= 0xC0
} typFO_SAT_SEL;

#define adrIM_ALIGN_ADDR		0x15
#define bmIM_ALIGN_ADDR			0x30
typedef enum {
	IM_ALIGN_ADDR_Start	= 0x00,
	IM_ALIGN_ADDR_Pg9	= 0x00,
	IM_ALIGN_ADDR_Pg6	= 0x10,
	IM_ALIGN_ADDR_Pg3	= 0x20,
	IM_ALIGN_ADDR_Pg0	= 0x30,
	IM_ALIGN_ADDR_End	= 0x30,
	IM_ALIGN_ADDR_Step	= 0x10
} typIM_ALIGN_ADDR;

#define adrIM_ALIGN_MODE		0x15
#define bmIM_ALIGN_MODE			0x0C
typedef enum {
	IM_ALIGN_MODE_00	= 0x00,
	IM_ALIGN_MODE_01	= 0x04,
	IM_ALIGN_MODE_10	= 0x08,
	IM_ALIGN_MODE_11	= 0x0C
} typIM_ALIGN_MODE;

#define adrIM_TABLE_WRITE		0x15
#define bmIM_TABLE_WRITE		0x02
typedef enum {
	IM_TBL_WRITE_Auto	= 0x00,
	IM_TBL_WRITE_Manual	= 0x02
} typ;

#define adrIM_ALIGN_START		0x15
#define bmIM_ALIGN_START		0x01
typedef enum {
	IM_ALIGN_START_0	= 0x00,
	IM_ALIGN_START_1	= 0x01
} typIM_ALIGN_START;


/* address 16h */


/* address 17h */
#define adrDIS_CLK_SEL			0x17
#define bmDIS_CLK_SEL			0x80
typedef enum {
	DIS_CLK_SEL_Auto	= 0x00,
	DIS_CLK_SEL_Manual	= 0x80
} typDIS_CLK_SEL;

#define adrIML_AUTO				0x17
#define bmIML_AUTO				0x20
typedef enum {
	IML_AUTO_SEL_Manual	= 0x00,
	IML_AUTO_SEL_Auto	= 0x20
} typIML_AUTO;


/* address 30h */
#define adrBLOCK_PE				0x30
#define PEAD6_On				0x80
#define PEAN_On					0x40
#define PEDSP_On				0x20
#define PESY_On					0x10
#define PECLK_On				0x08
#define PEAD_On					0x04
#define PEDA_On					0x02
#define PERF_On					0x01
#define PEAD6_Off				0x00
#define PEAN_Of					0x00
#define PEDSP_Off				0x00
#define PESY_Off				0x00
#define PECLK_Off				0x00
#define PEAD_Off				0x00
#define PEDA_Off				0x00
#define PERF_Off				0x00

/* address 65h */
#define adrPC_GAIN				0x65

/* address 66h */
#define adrPC_PHASE				0x66


/* address FEh */
#define adrSRST					0xFE
typedef enum {
	RESET_Dsp			= 0x0A,
	RESET_Register		= 0xA0,
	RESET_All			= 0xAA
} typSRST;


#endif
