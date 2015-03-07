#include <Wire.h>
#include "define.h"
#include "5B700.h"
#if (ARDUINO >= 100)
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define AGC		TRUE
#define AMAGC	TRUE
#define ADC6	TRUE
#define AMTUNE	TRUE
//-----------------------
// Select X'tal frequency
//#define CLK7M2	FALSE
#define CLK12M	TRUE
//#define CLK24M	FALSE
//#define CLK32K	FALSE
//-----------------------

#define SD_GAIN	FALSE
#define ADCFULL	FALSE
static U8 reg18h = 0x00;
//*********************************************************************
/// Mute Setting
/// sw : TRUE:MUTE On   FALSE:MUTE Off
//*********************************************************************
void muteEnable(unsigned char sw)
{
	if(sw)
	{
		regWrite(0xD0, 0x00);				// Mute Wait CNT
		delay(20);
		regBitWrite(0x00, 0x02, 0x02);		// 00h[1] MUTE/on
		delay(20);
		//regBitWrite(0x30, 0x02, 0x00);	// 30h[1] PEDA off
	}
	else
	{
		//regBitWrite(0x30, 0x02, 0x02);	// 30h[1] PEDA on
		regBitWrite(0x00, 0x02, 0x00);		// 00h[1] MUTE/off
		delay(20);
		regWrite(0xD0, 0x60);				// Mute Wait CNT
	}
}

//*********************************************************************
/// Starting sequence
///		band : BAND_AM / BAND_FM
///		freq : 520 - 1720(BAND_AM) 7600 - 10810(BAND_FM)
///		aout : AUDIO_DAC : Analog audio
///			   AUDIO_IIS_32fs : IIS 32fs
///			   AUDIO_IIS_64fs : IIS 64fs
//*********************************************************************
void dspAlignment(void);
void bandAndFrequencySetting(typFREQ);

void StartingSequence(typFREQ freq)
{
	powerOnSequence();
	dspAlignment();
	typAUDIO_OUT audio;
	audio = AUDIO_DAC;
	audioOutputSetting(audio);
	bandAndFrequencySetting(freq);
}

//*********************************************************************
/// Power on sequence
//*********************************************************************
static void powerOnSequence(void)
{
	regWrite(0xFE, 0xAA);	// FEh : SRST/all reset
	regWrite(0x00, 0x02);	// 00h : MUTE[1]/on

	/* Module Power Enable Setting */
	regWrite(0x90, 0x13);	// 0x90 : PEIM1[5]/off PEAMMIX[3]/off PEAMLNA[2]/off
	regWrite(0x93, 0xEE);	// 0x96 : VCO Current, PADEN
	regWrite(0x96, 0xAD);	// 0x96 : FMEN[5]/on
#if A5B700
	regWrite(0x9A, 0x03);	// 0x9A : Stnthe Voltage
#endif // A5B700

	/* X'tal Setting */
	regWrite(0x18, reg18h);
	if(reg18h == 0)				// 32KHz
	{
		regWrite(0xE8, 0x08);	// 0xE8 : PELPFVGA[3]/on IQ_DIFF_G[2:0]/I=Q
		regWrite(0x39, 0x80);	// 0x39 : UL_WIN
#if A5B700
		regWrite(0x27, 0xFF);	// 0x27 : vco setting
		regWrite(0x25, 0x33);	// 0x25
		regWrite(0x28, 0xC0);	// 0x28
#endif // A5B700
		regWrite(0xFD, 0x18);	// 0xFD : CLKGEN PAD
		regBitWrite(0x2E, 0x30, 0x10);	// 0x2E : LOC_T2[5:4]
		regWrite(0x26, 0x38);	// 0x26 : loop filter
		regWrite(0x10, 0x48);	// 0x26 : loop filter
	}
	else						// 12MHz
	{
		regWrite(0xE8, 0x88);	// 0xE8 : PELPFVGA[3]/on IQ_DIFF_G[2:0]/I=Q
		regWrite(0x39, 0x04);	// 0x39 : UL_WIN
		regWrite(0x27, 0x77);	// 0x27 : vco setting
		regWrite(0xFD, 0x00);	// 0xFD : CLKGEN PAD
		regBitWrite(0x2E, 0x30, 0x00);	// 0x2E : LOC_T2[5:4]
		regWrite(0x26, 0x3B);	// 0x26 : loop filter
		regWrite(0x10, 0x78);	// 0x26 : loop filter
	}

	regWrite(0xF9, 0x00);	// 0xF9 : PSC Bias Current[4]/Low Power Comsumption

	/* Synthe Setting */

	regWrite(0x94, 0x21);	// 0x94 : IFSEL[1:0]/300KHz
	regWrite(0x9D, 0x04);	// 0x9D : ADCMOD[3:0]/Exchange IQ
	regWrite(0xa0, 0x01);	// 0xA0 : IQCROSS[2],DEM[1]/Disable
	regWrite(0x02, 0x53);	// 0x02 : PSY_L[7:0]
	regWrite(0x03, 0x03);	// 0x03 : PSY_H[7:0] : freq/85.1MHz

#if !RDS & A5B800
	regWrite(0x0E, 0x27);	/* 0Eh */
#else // A5B700
	regWrite(0x0E, 0x20);	/* 0Eh */
#endif // A5B700

	/* AGC Setting */
	regWrite(0x3E, 0xA9);	// 0x3E : FMIN[7:4]/24dB PGM[3:0]/18dB
	regWrite(0x3F, 0x00);	// 0x3F : AGC_AUTO[7]/off

#if A5B800
	regWrite(0xD2, 0xC8);	// 0xD2 : VIFO INIT
	regWrite(0xD3, 0xD7);	// change V120b
#endif // A5B800
#if A5B700
	regWrite(0xD2, 0xDA);	// 0xD2 : VIFO INIT
	regWrite(0xD3, 0xD7);
#endif // A5B700

	//--------------------------------------
#if A5B800
	regWrite(0x40, 0x15);	// 0x40 : VIFO Upper[4:0]
	regWrite(0x41, 0x92);	// 0x41 : VIFO Lower[4:0]
	regWrite(0x42, 0x00);	// 0x42 : VUD  Upper[4:0]
	regWrite(0x43, 0x8A);	// 0x43 : VUD  Lower[4:0]
#endif // A5B800
#if A5B700
	regWrite(0x40, 0x1A);	// 0x40 : VIFO Upper[4:0]
	regWrite(0x41, 0x36);	// 0x41 : VIFO Lower[4:0]
	regWrite(0x42, 0x04);	// 0x42 : VUD  Upper[4:0]
	regWrite(0x43, 0x8A);	// 0x43 : VUD  Lower[4:0]
#endif // A5B700

#if A5B700
	regWrite(0xDA, 0x0C);
#endif // A5B700

	regWrite(0x30, 0xE7);	// 0x30 : PECLK,PESYN/off other/on

#if A5B800
	regWrite(0x46, 0x1c);	// VIFO Upper(4:0)
	regWrite(0x47, 0x18);	// VIFO Lower(4:0)
	regWrite(0x48, 0x18);	// VUD  Upper(4:0)
	regWrite(0x49, 0x10);	// VUD  Lower(4:0)
	regWrite(0x4A, 0x0f);	// PIF  Scale(4:0)
	regWrite(0xD5, 0xdc);	// AM VIFO Init Upper/Lower(7:0) Default:DCh
	regWrite(0xD6, 0xA9);	// AM VUD  Init Upper/Lower(7:0) Default:EDh
	regWrite(0xD7, 0x1f);	// AM PIF  Init(7:0) Default:1Dh
#endif // A5B800


	/* DSP Initial Setting */
#if A5B800
	regWrite(0x54, 0xB0);
//	regWrite(0x55, 0x33);	// FM_RSSI_CONST 2dB up
	regWrite(0x55, 0x35);	// FM_RSSI_CONST 2dB up
//	regWrite(0x5C, 0xD8);	// -40dB
#endif // A5B800
#if A5B700
//	regWrite(0x55, 0x30);	// FM_RSSI_CONST
	regWrite(0x55, 0x32);	// FM_RSSI_CONST
//	regWrite(0x5C, 0xD8);	// -40dB
#endif // A5B700

//*#if A5B800
	regWrite(0x11, 0x00);	// SMT
	regWrite(0x12, 0x04);	// HCC SNC
//*#endif // A5B800
//*#if A5B700
//*	regWrite(0x11, 0x01);	// SMT
//*	regWrite(0x12, 0x44);	// HCC SNC
//*#endif // A5B700

#if A5B800
	regWrite(0xEB, 0x02);	// VBFSEL(2:1)=01, DSP=1.1V
	regWrite(0xFA, 0x00);	// DET_OFFSET Setting
#endif // A5B800
#if A5B700
	regWrite(0xEB, 0x01);	// VBFSEL(2:0)=001, DSP=1.1V
	regWrite(0xFA, 0x00);	// DET_OFFSET Setting
#endif // A5B700

	regWrite(0x84, 0x40);	// HPLVOL=40h
	regWrite(0x85, 0x40);	// HPRVOL=40h

	/* Main Power On */
	regBitWrite(0x00, 0x01, 0x01);	// 00h[0] : PE/on
	regWrite(0x30, 0xff);

	if(reg18h == 0)
	{
		delay(600);
#if A5B800
		regWrite(0xFD, 0x1E);	 	// CLKGEN PAD EN
#endif // A5B800
#if A5B700
		regWrite(0xFD, 0x1C);	 	// CLKGEN PAD EN
		delay(50);
		delay(50);
		regWrite(0xFD, 0x1E);	 	// CLKGEN PAD EN
#endif // A5B700
	}
	else
	{
		delay(30);
	}

	regWrite(0x37, 0x20);

	// RDSI OUTPUT Setting
	regWrite(0xF7, 0x55);
	regWrite(0xF8, 0xAA);
}

//*********************************************************************
/// Power off sequence
//*********************************************************************
void powerOffSequence(void)
{
	/* Mute ON */
	muteEnable(TRUE);

	/* Block Power */
	regWrite(0x30, 0xEF);

	/* Power OFF */
	regBitWrite(0x00, 0x01, 0x00);	// 00h[0] PE/off
}

//*********************************************************************
/// DSP Alignment
//*********************************************************************
typedef enum {
	IQCORECT	= 0x01,
	IMLADJUST	= 0x02,
	GOCTUNE		= 0x04
}typCTUNE;

static void ctuneRegE0Start(typCTUNE ctune)
{
	uint8_t timeout = 0;
	uint8_t num;

	if(ctune == IQCORECT) num = 150;
	else if(ctune == GOCTUNE) num = 50;
	else num = 5;

	regBitWrite(0xE0, ctune, ctune);		/* E0h[1] IMLADJUST */

	while(regRead(0xE0) & ctune)
	{
		if(timeout++ > num)
		{
			break;
		}
		delay(100);
	}
}

#define ALIGN_FREQ		6
#define ALIGN_FM_FREQ	3
#define ALIGN_AGCG		5

typedef union {
	uint16_t		word;
	struct {
		uint8_t	h;
		uint8_t	l;
	} byte;
} typFinePG;

static struct {
	typFinePG	fine_p;
	typFinePG	fine_g;
} fine_pg[6][5];

static unsigned char dsp_alignment_done;

#if A5B700
static typFREQ  align_freq_table[3] = { 7680, 8777, 10240 };	// X0.01MHz
static typFREQ  align_fm_f_table[4] = { 7960, 8500,  9140, 9880 };	// X0.01MHz
static uint8_t  align_imf_tabgle[3] = { 0x40, 0x50, 0x60 };
#endif // A5B700

static U8 align_agcg_table[ALIGN_AGCG] = {0x00, 0x03, 0x06, 0x09, 0x0C};	/* 0dB 6dB 12dB 18dB 24dB */
static unsigned char  am_tune_align_done;

void dspAlignBody(uint8_t);
void bestIml(uint8_t);
void findPg(uint8_t, uint8_t);

/// DSP Alignment
uint8_t det_ofst;
#if !RDS & A5B800
uint8_t det_ofst_am;
#endif // !RDS & A5B800
static void dspAlignment(void)
{
	uint8_t num;

	uint8_t best_det;

	dsp_alignment_done = FALSE;
	am_tune_align_done = FALSE;

	/* Send Registers */
	regBitWrite(0x0E, 0x60, 0x00);	/* 0Eh[6:5] BW */
	regBitWrite(0x01, 0x08, 0x08);	/* 01h[7] STATION_EN */
	regWrite(0x15, 0x00);

	regBitWrite(0xE8, 0x10, 0x10);		/* FC_SW=1(LPF 500kHz) */

	regBitWrite(0xE9, 0x1F, 0x10);	/* E9h[4:0] IMFMSW IMFMA */
#if !RDS & A5B800
	regBitWrite(0xEA, 0x1F, 0x10);	/* EAh[4:0] IMAMSW IMAMA */
#endif // !RDS & A5B800

	regBitWrite(0xE0, 0xC0, 0x40);	/* E0h[7:6] AVECOUNT */


	regWrite(0x3E, 0x00);		// FMI=0h
	regWrite(0xFA, 0x30);		// DET_AFM=FM

	// Best Det
	det_ofst = 0x08;
	while(TRUE)
	{
		regBitWrite(0xFA, 0x0F, det_ofst);

		best_det = 0;
		for(num = 0; num < 4; num++)
		{
			best_det += regRead(0x69) & 0x3F;
		}
		if(best_det > 16) break;
		if(det_ofst == 7) break;

		if(++det_ofst >= 0x10) det_ofst = 0;
	}
#if !RDS & A5B800
	det_ofst_am = (det_ofst + 3) & 0x0F;
	if(det_ofst_am > 7) det_ofst_am = 7;
#endif // !RDS & A5B800

#if A5B700
//	regWrite(0xD6, 0x80);
//	wait_1us(T200us);
	regWrite(0xD6, 0x10);
#endif // A5B700

	// DSP Align Body
#if A5B800
	for(num = 0; num < 4; num++)
#endif // A5B800
#if A5B700
	for(num = 0; num < 3; num++)
#endif // A5B700
	{
		dspAlignBody(num);
	}

	for (num = 0; num < 5; num++)
	{
		fine_pg[1][num].fine_g.word = (fine_pg[0][num].fine_g.word + fine_pg[2][num].fine_g.word) >> 1;
		fine_pg[1][num].fine_p.word = (fine_pg[0][num].fine_p.word + fine_pg[2][num].fine_p.word) >> 1;
		fine_pg[3][num].fine_g.word = (fine_pg[2][num].fine_g.word + fine_pg[4][num].fine_g.word) >> 1;
		fine_pg[3][num].fine_p.word = (fine_pg[2][num].fine_p.word + fine_pg[4][num].fine_p.word) >> 1;
	}

	regBitWrite(0xE9, 0x1F, 0x00);	/* E9h[4] IMFMSW */

	/* Send Registers */
	regBitWrite(0x0E, 0x60, 0x40);	/* 0Eh[6:5] BW */
	regWrite(0x15, 0xC0);

	regBitWrite(0xE8, 0x10, 0x00);		/* FC_SW=0 */

#if A5B700
	regWrite(0x98, 0x00);		// IMAGE off
#endif // A5B700

	dsp_alignment_done = TRUE;
}

/// DSP align body
static void dspAlignBody(uint8_t freq_num)
{
	uint8_t agcg_num;
	uint8_t freq_num2;
	typFREQ freq_buf;

	/* Send Registers */
	freq_num2 = freq_num << 1;

		freq_buf.word = align_freq_table[freq_num].word - 60;
		bandAndFrequencySetting(freq_buf);	// change LO position
		regBitWrite(0x0E, 0xE0, 0x20);

		regWrite(0x3E, 0x5C);				// FMIM=-6dB (ES2)
		regWrite(0x3F, 0x00);

		regWrite(0x90, 0x33);


#if A5B700
		regWrite(0x98, align_imf_tabgle[freq_num]);	// 98h[6:4] IMF
#endif // A5B700
		regWrite(0x96, 0xAD);
		regWrite(0xE5, 0x10);
		regBitWrite(0xE9, 0x10, 0x10);		// IMFMSW=1
#if !RDS & A5B800
		regBitWrite(0xEA, 0x10, 0x00);		// IMAMSW=0
#endif // !RDS & A5B800

		regWrite(0x32, 0x40);
		regWrite(0x59, 0x00);
		regWrite(0x5A, 0x32);

#if A5B700
		regBitWrite(0x04, 0x10, 0x10);
#endif // A5B700
		regBitWrite(0x5a, 0x80, 0x80);

	for(agcg_num = 0; agcg_num < 2; agcg_num++)
	{
		regBitWrite(0x3E, 0x0F, align_agcg_table[agcg_num]);	// 3Eh[3:0] PGM

		if (agcg_num == 0) delay(1);

		if (align_agcg_table[agcg_num] > 0x06)
			regBitWrite(0xE5, 0x10, 0x10);

		bestIml(agcg_num);

		regBitWrite(0xE0, 0xC0, 0x00);        // E0h[7:6] AVECOUNT
		findPg(freq_num2, agcg_num);
		regBitWrite(0xE0, 0xC0, 0x40);        // E0h[7:6] AVECOUNT

		if (align_agcg_table[agcg_num] > 0x06)
			regBitWrite(0xE5, 0x10, 0x00);
	}
}

/// best iml
static void bestIml(uint8_t agcg_num)
{
	typWORD best_iml;
	uint8_t iml = 0;
	uint16_t best_iml_th;

	if (agcg_num == 0) best_iml_th = 0x4c60;
	else best_iml_th = 0x6D1D;

	regBitWrite(0xE9, 0x0F, 0x00);		// E9h[3:0] IMFMA
#if !RDS & A5B800
	regBitWrite(0xEA, 0x0F, 0x00);		// E9h[3:0] IMAMA
#endif // !RDS & A5B800

	while(1)
	{
		ctuneRegE0Start(IMLADJUST);

		best_iml.byte.l = regRead(0xE1);
		best_iml.byte.h = regRead(0xE2);

		if (best_iml.word > best_iml_th) break;
        if (iml++ >= 15) break;

		regBitWrite(0xE9, 0x0F, iml);		// E9h[3:0] IMFMA
#if !RDS & A5B800
		regBitWrite(0xEA, 0x0F, iml);		// E9h[3:0] IMAMA
#endif // !RDS & A5B800
	}
}

/// find pg
static void findPg(uint8_t freq_num, uint8_t agcg_num)
{
	uint8_t addr;
	uint8_t pgm;

	uint16_t s;
	uint16_t fp[3];
	uint16_t fg[3];
	uint16_t i2;

	ctuneRegE0Start(IQCORECT);
	pgm = regRead(0x3E) & 0x0F;	// 3Eh[3:0] PGM

#if A5B800
	if(pgm <= 0x01)
	{
		addr = 0xA8;
	}
	else if(pgm <= 0x03)
	{
		addr = 0xAC;
	}
	else if(pgm <= 0x06)
	{
		addr = 0xB0;
	}
	else if(pgm <= 0x09)
	{
		addr = 0xB4;
	}
	else
	{
		addr = 0xB8;
	}

	if (addr != 0xA8)
#endif // A5B800
#if A5B700
	addr = 0xA8;

	if (pgm >= 0x02)
#endif // A5B700
	{
		fine_pg[freq_num][agcg_num].fine_g.byte.l = regRead(addr++);
		fine_pg[freq_num][agcg_num].fine_g.byte.h = regRead(addr++);
		fine_pg[freq_num][agcg_num].fine_p.byte.l = regRead(addr++);
		fine_pg[freq_num][agcg_num].fine_p.byte.h = regRead(addr);

		fine_pg[freq_num][2].fine_g.word = fine_pg[freq_num][agcg_num].fine_g.word;
		fine_pg[freq_num][2].fine_p.word = fine_pg[freq_num][agcg_num].fine_p.word;
		fine_pg[freq_num][3].fine_g.word = fine_pg[freq_num][agcg_num].fine_g.word;
		fine_pg[freq_num][3].fine_p.word = fine_pg[freq_num][agcg_num].fine_p.word;
		fine_pg[freq_num][4].fine_g.word = fine_pg[freq_num][agcg_num].fine_g.word;
		fine_pg[freq_num][4].fine_p.word = fine_pg[freq_num][agcg_num].fine_p.word;
	}
	else
	{
		fg[0] = regRead(addr++);
		fg[0] += regRead(addr++) << 8;
		fp[0] = regRead(addr++);
		fp[0] += regRead(addr) << 8;

		s = 8;
		for (i2 = 1; i2 < s; i2++)
		{

			ctuneRegE0Start(IQCORECT);
			addr = 0xA8;

			fg[0] += regRead(addr++);
			fg[0] += regRead(addr++) << 8;
			fp[0] += regRead(addr++);
			fp[0] += regRead(addr) << 8;
		}

		fine_pg[freq_num][agcg_num].fine_g.word = fg[0] >> 3;
		fine_pg[freq_num][agcg_num].fine_p.word = fp[0] >> 3;
	}
}

// PG Table Setting
static void pgTableSetting(typFREQ freq)
{
	typWORD pg_buf[2];
	uint8_t freq_table_count;
	uint8_t agcg_table_count;
	uint8_t adr, i;

#if A5B800
	if (freq.word > 4000)
	{
#endif // A5B800
		for (freq_table_count = 0; freq_table_count < 4; freq_table_count++)
		{
			if (freq.word < align_fm_f_table[freq_table_count].word)
			{
				break;
			}
		}
#if A5B800
	}
	else
	{
		freq_table_count = 5;
	}
#endif // A5B800

	for (agcg_table_count = 0; agcg_table_count < ALIGN_AGCG; agcg_table_count++)
	{
		if (align_agcg_table[agcg_table_count] < 0x02)
		{
			adr = 0xBC;
		}
		else if (align_agcg_table[agcg_table_count] < 0x04)
		{
			adr = 0xC0;
		}
		else if (align_agcg_table[agcg_table_count] < 0x07)
		{
			adr = 0xC4;
		}
		else if (align_agcg_table[agcg_table_count] < 0x0A)
		{
			adr = 0xC8;
		}
		else
		{
			adr = 0xCC;
		}

		pg_buf[0].word = fine_pg[freq_table_count][agcg_table_count].fine_g.word;
		pg_buf[1].word = fine_pg[freq_table_count][agcg_table_count].fine_p.word;

		for(i=0; i<2; i++)
		{
			if((pg_buf[i].word >= 0x400) && (pg_buf[i].word <= 0xc00))
			{
				regWrite(adr++, pg_buf[i].byte.l);
				regWrite(adr++, pg_buf[i].byte.h);
			}
			else
			{
				// default
				regWrite(adr++, 0x00);
				regWrite(adr++, 0x08);
			}
		}
	}
}

//*********************************************************************
/// Audio Output Setting
///		aout : AUDIO_DAC : Analog audio
///			   AUDIO_IIS_32fs : IIS 32fs
///			   AUDIO_IIS_64fs : IIS 64fs
//*********************************************************************
void audioOutputSetting(typAUDIO_OUT aout)
{
	uint8_t reg0D;
#if A5B800
	uint8_t reg58 = 0;
#endif // A5B800
#if A5B700
	uint8_t reg97 = 0;
#endif // A5B700

	if(aout == AUDIO_IIS_32fs)
	{
		reg0D = (0x08 | 0x00 | 0x01);	// 0Dh[4:3] IIS_SEL  0Dh[2] IIS_32_64  0Dh[1:0] IIS_EN
	}
	else if(aout == AUDIO_IIS_64fs)
	{
		reg0D = (0x08 | 0x04 | 0x01);	// 0Dh[4:3] IIS_SEL  0Dh[2] IIS_32_64  0Dh[1:0] IIS_EN
	}
	else
	{
		reg0D = (0x08 | 0x00 | 0x00);	// 0Dh[4:3] IIS_SEL  0Dh[2] IIS_32_64  0Dh[1:0] IIS_EN
#if A5B800
		reg58 = 0x28;	 				// Dither
#endif // A5B800
#if A5B700
		reg97 = 0x28;	 				// Dither
#endif // A5B700
	}

	regBitWrite(0x0D, 0x1F, reg0D);
#if A5B800
	regWrite(0x58, reg58);
#endif // A5B800
#if A5B700
	regWrite(0x97, reg97);
#endif // A5B700
}

//*********************************************************************
/// Audio output volume
//*********************************************************************
static  uint8_t fm_aov_cf = 0x40;
static  uint8_t fm_aov_om = 0x80;
void audioOutputVolume(uint8_t vol)
{
	regWrite(0x84, vol);	// Lch audio
	regWrite(0x85, vol);	// Rch audio
}
void setFmAudioOutputVolume(uint8_t cf, uint8_t om)
{
	fm_aov_cf = cf;
	fm_aov_om = om;
}

//*********************************************************************
/// Reset RFAGC
//*********************************************************************
static void resetRfagc(void)
{
		regBitWrite(0x01, 0x40, 0x40);	// 01h[6] : RST_RFAGC/enable
		regBitWrite(0x01, 0x40, 0x00);	// 01h[6] : RST_RFAGC/disable
}

//*********************************************************************
/// Set seek flag
//*********************************************************************
static unsigned char   on_seek;
void setSeekEnable(unsigned char seek)
{
	on_seek = seek;
}

//*********************************************************************
/// Band and Frequency Setting
///		band :  BAND_FM / BAND_AM
///		freq : (BAND_FM) 7600 - 10810 (76.00MHz - 108.10MHz)
///		freq : (BAND_AM)  520 -  1720 (  520KHz -   1720KHz)
//*********************************************************************
#define FM_F400KHZ_NUM		6
static U16 FM_f400KhzIf_table[FM_F400KHZ_NUM] = {7650, 8150, 8155, 10210, 10215, 10800};


static typFMBW  fmBW = FM_CF105K;

void bandAndFrequencySetting(typFREQ freq)
{
	uint8_t i;
	uint16_t buf;
	typWORD sens_word;

		regBitWrite(0x3F, 0x80, 0x80);	// 0x3F : AGC_AUTO[7]/on
		regWrite(0x90, 0x13);			// Block_PE(LO=1, fmMIX=1, fmLNA=1)
		regBitWrite(0xEC, 0x20, 0x00);  // ECh[5] AMRFSW off
		regBitWrite(0x00, 0x10, 0x00);	// 00h[4] BAND

		regBitWrite(0x32, 0x40, 0x00);	// DCO_OFF=0
		regWrite(0x94, 0x21);			// IFSEL=300KHz
		regWrite(0x59, 0x00);
		regWrite(0x5A, 0x32);			// DMixer Freq=300kHz
		sens_word.word = 0x3200;
		//=====================================================

		if(on_seek == FALSE)
		{
			for(i=0; i<FM_F400KHZ_NUM; i++)
			{
				buf = FM_f400KhzIf_table[i];
				if(freq.word == buf)
				{
					regWrite(0x94, 0x22);   // IFSEL=400KHz
					regWrite(0x59, 0xab);
					regWrite(0x5a, 0x42);	// DMixer Freq=400kHz
					sens_word.word = 0x42ab;
					break;
				}
			}
		}

		//=====================================================

		regWrite(0x96, 0x38);

		regWrite(0xE5, 0x11);	// 0xE5 : FMLNAS[4] Differential & 2.5mA mode

//		regBitWrite(0xE8, 0x10, 0x00);		// FC_SW=0(LPF 1000kHz)
		regBitWrite(0xFA, 0x3F, 0x30 + det_ofst);		// 0xFA : DET_AFM=FM ATTEN[5] DET_AFM[4]

		regBitWrite(0x0E, 0xE0, fmBW);
		regWrite(0xA2, 0x00);
//		regWrite(0xE4, 0x83);
		regWrite(0xE6, 0x23);

		fmFrequencySetting(freq);
		regBitWrite(0x01, 0x40, 0x40);	// 01h[6] : RST_RFAGC/enable
		delay(15);
		regBitWrite(0x01, 0x40, 0x00);	// 01h[6] : RST_RFAGC/disable
}

//*********************************************************************
/// Constrained Mono
///		mono : TRUE / FALSE
//*********************************************************************
void constrainMono(unsigned char mono)
{
	if (mono)
	{
		regBitWrite(0x00, 0x04, 0x04);	// 00h[2] MONO
	}
	else
	{
		regBitWrite(0x00, 0x04, 0x00);	// 00h[2] MONO
	}
}



//*********************************************************************
/// FM Frequency Setting
///		freq : 7600 - 10810 (76.00MHz - 108.10MHz)
//*********************************************************************
static  uint8_t reg12h;
#if !CAR
static typDEMP  emp_menu = DEMP_50us;
#endif // !CAR
void fmFrequencySetting(typFREQ freq)
{
	typFREQ psy;
	uint8_t psy_f;
	uint8_t reg01h;		// LO
	uint8_t regE8h;		// FC_SW
	uint8_t regE4h;		// ADC
	uint8_t _reg12h;		// HCC SNC
	uint8_t reg5Ah;		// DOWNCONV
	uint8_t regD1h;		// DCCAL_INV
#if !CAR
	typDEMP _emp_menu;
#endif // !CAR

	// calculate frequency setting value
	psy.word = freq.word / 10;
	psy_f = freq.word - psy.word * 10;

	if(psy_f)
	{
		if(psy_f <= 5)
		{
#if A5B800
			psy_f += 8;
#endif // A5B800
#if A5B700
			psy_f += 16;
#endif // A5B700
		}
		else
		{
			psy_f = 10 - psy_f;
			psy.word++;
		}
	}

	// CE Freq.
	switch(freq.word)
	{
#if !CAR
		case 9800 :		// Change parameter at 98.00MHz
			_reg12h = 0x65;		// HCC:22dBu SNC:41dBu
			reg01h = 0x10;		// LO +300
			reg5Ah = 0x00;		// DOWNCONV = 0
			regD1h = 0x00;		// DCCAL_INV = 0
			regE8h = 0x00;		// FC_SW=0 (LPF=1MHz)
			regE4h = 0x81;		// ADC 1bit
			_emp_menu = emp_menu;
			break;

		case 8760 :		// Change parameters at 87.60MHz
			_reg12h = 0x67;		// HCC:22dBu SNC:47dBu
			reg01h = 0x10;		// LO +300
			reg5Ah = 0x00;		// DOWNCONV = 0
			regD1h = 0x00;		// DCCAL_INV = 0
			regE8h = 0x10;		// FC_SW=1 (LPF=500KHz)
			regE4h = 0x81;		// ADC 1bit
			_emp_menu = DEMP_75us;
			break;

		case 10790 :	// Change parameters at 107.90MHz
			_reg12h = 0x67;		// HCC:22dBu SNC:47dBu
			reg01h = 0x00;		// LO -300
			reg5Ah = 0x80;		// DOWNCONV = 1
			regD1h = 0x01;		// DCCAL_INV = 1
			regE8h = 0x10;		// FC_SW=1 (LPF=500KHz)
			regE4h = 0x81;		// ADC 1bit
			_emp_menu = DEMP_75us;
			break;

#endif // CAR

		default :		// default parameters at other frequency
			_reg12h = reg12h;	// HCC SNC menu
			reg01h = 0x10;		// LO +300
			reg5Ah = 0x00;		// DOWNCONV = 0
			regD1h = 0x00;		// DCCAL_INV = 0
			regE8h = 0x00;		// FC_SW=0 (LPF=1MHz)
			regE4h = 0x83;		// ADC 3bit
#if !CAR
			_emp_menu = emp_menu;
#endif // !CAR
			break;
	}
	regWrite(0x12, _reg12h);
	regBitWrite(0x01, 0x10, reg01h);
	regBitWrite(0x5A, 0x80, reg5Ah);
	regBitWrite(0xD1, 0x01, regD1h);
	regBitWrite(0xE8, 0x10, regE8h);
	regWrite(0xE4, regE4h);
#if !CAR
	regBitWrite(0x0D, 0x40, _emp_menu);		// 0Dh[6] DeEmphasis
	regBitWrite(0x35, 0x01, _emp_menu);		// 35h[0]
#endif // !CAR

	// Set register frequency value
	regWrite(0x02, psy.byte.l);
	regWrite(0x03, psy.byte.h);
#if A5B800
	regBitWrite(0x04, 0x0F, psy_f);	// 04h[3:0] PSY_F
#endif // A5B800
#if A5B700
	regBitWrite(0x04, 0x1F, psy_f);	// 04h[4:0] PSY_F
#endif // A5B700

	if (dsp_alignment_done == TRUE)
	{
		// Gain/Phase for FM Table Write
		pgTableSetting(freq);
	}
}

//*********************************************************************
/// FM Seek operation
//*********************************************************************
//*********************************************************************
/// FM Check Reception Sensitivity
///		sens : 0-255 Reception Sensitivity
//*********************************************************************
unsigned char fmCheckReceptionSensitivity(uint8_t sens)
{
	uint8_t  ret = TRUE;
	uint8_t  lSens = sens - 2;
	uint8_t  i;
	unsigned char IF400 = FALSE;

	if(lSens > 0x7F) lSens = 0;

	//	regWrite(0xFE, 0x0A);

	if(on_seek)
	{
		regBitWrite(0x15, 0x07, 0x04);
		resetRfagc();
		delay(20);
	}

	while(TRUE)
	{
		for(i = 0; i < 6; i++)
		{
#if !RDS
			delay(10);
#else // RDS
			timer0Start(T10ms);
			while(timer0CheckBusy())
			{
				checkRdsReceive();
			}
#endif // RDS
			if(regRead(0x05) & 0x04)
			{
				ret = FALSE;
				break;
			}
//			if(regRead(0x6C) < sens) return FALSE;
//			if(regRead(0x06) < sens)


			if(regRead(0x05) & 0x01)
			{
				if((regRead(0xFC) < lSens) || (regRead(0xFC) >= 128))
				{
					ret = FALSE;
					break;
				}
			}
			else
			{
				if((regRead(0xFC) < lSens + 5) || (regRead(0xFC) >= 128))
				{
					ret = FALSE;
					break;
				}
			}
		}

		if(!ret) break;
		if(!on_seek) break;
		if(IF400) break;

		IF400 = TRUE;
		regWrite(0x94, 0x22);   // IFSEL=400KHz
		regWrite(0x59, 0xab);
//		regWrite(0x5a, 0x42);	// DMixer Freq=400kHz
		regBitWrite(0x5a, 0x7F, 0x42);	// DMixer Freq=400kHz
#if !RDS
			delay(20);
#else // RDS
			checkRdsReceive();
			delay(20ms);
			checkRdsReceive();
#endif // RDS
	}

	regBitWrite(0x15, 0x07, 0x00);
	return ret;

#if 0
	uint8_t  ret = TRUE;
	uint8_t  i;
	unsigned char IF400 = FALSE;
	uint8_t  reg15h = regRead(0x15);

	if(sens > 0x7F) sens = 0;

	//	regWrite(0xFE, 0x0A);

	if(on_seek)
	{
		regBitWrite(0x15, 0xC7, 0x03);
		regBitWrite(0xe8, 0x10, 0x10);
		resetRfagc();
		wait_1us(T20ms);
	}

	while(TRUE)
	{
		for(i = 0; i < 3; i++)
		{
#if !RDS
			if(on_seek)
			{
				delay(10);
			}
			else
			{
				delay(10);
			}
#else // RDS
			if(on_seek)
			{
				timer0Start(T10ms);
			}
			else
			{
				timer0Start(T10ms);
			}
			while(timer0CheckBusy())
			{
				checkRdsReceive();
			}
#endif // RDS
			if(regRead(0x05) & 0x04)
			{
				ret = FALSE;
				break;
			}
#if 0
			if(!(regRead(0x05) & 0x01))
			{
				if(regRead(0x7F) >= 20)
				{
					ret = FALSE;
					break;
				}
			}
			if((regRead(0x06) & 0x7F) < sens)
			{
				ret = FALSE;
				break;
			}
#else
			if(regRead(0x05) & 0x01)
			{
				if((regRead(0x06) & 0x7F) < sens)
				{
					ret = FALSE;
					break;
				}
			}
			else
			{
				if((regRead(0x06) & 0x7F) < (sens + 5))
				{
					ret = FALSE;
					break;
				}
			}
#endif
		}

		if(!ret) break;
		if(!on_seek) break;
		if(IF400) break;

		IF400 = TRUE;
		regWrite(0x94, 0x22);   // IFSEL=400KHz
		regWrite(0x59, 0xab);
		regBitWrite(0x5a, 0x7F, 0x42);	// DMixer Freq=400kHz
#if !RDS
			wait_1us(T45ms);
#else // RDS
			timer0Start(T45ms);
			while(timer0CheckBusy())
			{
				checkRdsReceive();
			}
#endif // RDS
	}

	regWrite(0x15, reg15h);
	return ret;
#endif
}


//*********************************************************************
/// FM Check Stereo
///		RETURN : TRUE:STEREO  FALSE:MONO
//*********************************************************************
unsigned char fmCheckStereo(void)
{
	return (regRead(0x05) & 0x01) ? TRUE : FALSE;	// 05h[0] STEREO
}

//*********************************************************************
// Set De-Emphasis
//	emp : DEMP_50us/DEMP_60us/DEMP_75us
//*********************************************************************
void fmSetDeEmphasis(typDEMP emp)
{
	regBitWrite(0x0D, 0x40, emp);		// 0Dh[6] DeEmphasis
	regBitWrite(0x35, 0x01, emp);		// 35h[0]
#if !CAR
	emp_menu = emp;
#endif // !CAR
}

//*********************************************************************
// Get Signal Strength
//  only FM
//*********************************************************************
#if !CEPATCH
uint8_t getSignalStrength(void)
{
	return regRead(0x06) & 0x7F;
}
#else
uint8_t getSignalStrength(void)
{
	uint8_t	reg05, reg06, reg0E, reg3F;
#if A5B800
	uint8_t	reg6c;
#endif // A5B800

	reg06 = regRead(0x06) & 0x7F;	// RSSI
#if A5B800
	reg6c = regRead(0x6C) & 0x7F;	// RSSI	  // tok
#endif // A5B800

	//==== add for CE Patch by I.M =====================
	reg05 = regRead(0x05) & 0x01;  	// stereo
	reg0E = regRead(0x0E) & 0xE0;	// BW
	reg3F = regRead(0x3F) & 0x80;	// AGC

	if (reg05 == 0x00 )	// MONO
	{
		if (reg3F == 0x00) regBitWrite(0x3F, 0x80, 0x80);		// AGC=ON
		if (reg0E != 0x80) regBitWrite(0x0E, 0xE0, 0x80);		// OM75K
	}
	else					// Stereo
	{
		if (reg3F == 0x80)
		{
#if A5B800
			regBitWrite(0x55, 0x80, 0x80);
			if ((reg06 <= 0x46) && (reg6c >= 0x32) && (reg6c <= 0x41))	 	// set CE Patch
#endif // A5B800
#if A5B700
			if (reg06 >= 0x41 && reg06 <= 0x49)	 	// set CE Patch
#endif // A5B700
			{
		  		regWrite(0x3E, 0x59);				// Manual Gain
		  		regBitWrite(0x3F, 0x80, 0x00);		// AGC=OFF
			}
		}
		else
		{
#if A5B800
			if (reg06 >= 0x50)	 	// set CE Patch
			{
				regWrite(0x3E, 0x70);
#endif // A5B800
#if A5B700
			if (reg06 <= 0x0B || reg06 >= 0x49)		// clr CE Patch
			{
#endif // A5B700
				regBitWrite(0x3F, 0x80, 0x80);		// AGC=ON
				regBitWrite(0x0E, 0xE0, 0xA0);		// OM105K
			}
#if A5B700
			else if (reg06 <= 0x19)					// UDS 150kHz
			{
				if (reg0E != 0x80) regBitWrite(0x0E, 0xE0, 0x80);		// OM75K
			}
#endif // A5B700
		}
	}

	if (reg3F == 0x80)
	{
		return (reg06);
	}
	else
	{
		return (reg06 + 0x2A);	   // tekitou
	}
	//==================================================
}
#endif // CEPATCH

//*********************************************************************
/// 5Bx00 BW (0Eh[7:5]) setting
//*********************************************************************
void fmFMOV_BwSetting(typFMBW dat)
{
	fmBW = (typFMBW)(dat & 0xF0);	// 0Eh[7:4] FM_OV, BW
	regBitWrite(0x0E, 0xF0, fmBW);

	if(fmBW & 0x80)
	{ // OM
		regWrite(0x84, fm_aov_om);	 		// HPLVOL=40h
		regWrite(0x85, fm_aov_om);	 		// HPRVOL=40h
		regWrite(0x22, 0x05);	 		// SDTH
		regWrite(0x23, 0x05);	 		// SDTH
	}
	else
	{ // CF
		regWrite(0x84, fm_aov_cf);	 		// HPLVOL=40h
		regWrite(0x85, fm_aov_cf);	 		// HPRVOL=40h
		regWrite(0x22, 0x0A);	 		// SDTH
		regWrite(0x23, 0x0A);	 		// SDTH
	}
}

//*********************************************************************
/// 0Fh setting
//*********************************************************************
void reg0FhSetting(uint8_t bmt_bw, uint8_t bmt_lvl)
{
	regBitWrite(0x0F, 0x7F, ((bmt_bw<<4)&0x70) + (bmt_lvl&0x0F) );
}
//*********************************************************************
/// 11h setting
//*********************************************************************
void reg11hSetting(uint8_t smt)
{
	regBitWrite(0x11, 0x0F, smt);
}
//*********************************************************************
/// 12h setting
//*********************************************************************
void reg12hSetting(uint8_t hcc, uint8_t snc)
{
	reg12h = (hcc << 4) + (snc & 0x0F);

	// Set register at fmFrequencySetting() function.
}
//*********************************************************************
/// 15h setting
//*********************************************************************
void reg15hSetting(uint8_t fo_sat_sel)
{
	regBitWrite(0x15, 0xC0, fo_sat_sel);
}
//*********************************************************************
/// 18h setting
//*********************************************************************
void reg18hSetting(uint8_t fxtal)
{
	reg18h = fxtal & 0x07;
	if(reg18h > 5) reg18h = 0;

	// Set register at powerOnSequence() function.
}

//*********************************************************************
/// Tune Output setting
//*********************************************************************
void tuneOutputSetting(unsigned char sw)
{
	uint8_t reg32h;

	if(sw == TRUE)
	{
		reg32h = 0x04;
	}
	else
	{
		reg32h = 0x00;
	}
	regBitWrite(0x32, 0x0F, reg32h);
}

#if !RDS & A5B800
//*********************************************************************
/// Tune Threshold setting
//*********************************************************************
void tuneThresholdSetting(uint8_t th)
{
	regWrite(0x57, th);
}

//*********************************************************************
/// AM Detect Threshold setting
//*********************************************************************
void amDetThresholdSetting(uint8_t th)
{
	regWrite(0x5C, th);
}

//*********************************************************************
///  LPFS HPFS setting
//*********************************************************************
void lpfsHpfsSetting(uint8_t lpfs, uint8_t hpfs)
{
	uint8_t reg0Eh;

	reg0Eh = (lpfs << 2) + (hpfs & 0x03);

	regBitWrite(0x0E, 0x0F, reg0Eh);
}
#endif // !RDS & A5B800

//*********************************************************************
/// check Hung-up
//*********************************************************************
unsigned char checkHungUp(void)
{
	if(regRead(0x00) & 0x01)
	{
		return FALSE;
	}
	return TRUE;
}

//==============================================================================
//
//	Terget device serial I/F function
//
//==============================================================================
void send_data(U8);
U8 receive_data(void);

static void openWire(void)
{
	Wire.begin();
}

static void closeWire(BOOL close)
{
	Wire.endTransmission(close);
}


void regWrite(U8 addr, U8 dat)
{
	openWire();

	/* send chip address & write flag */
	send_data((CHIP_ADDRESS & 0xFE)^SIG_XOR);

	/* send register address */
	send_data(addr^SIG_XOR);

	/* send register datas */
	send_data(dat^SIG_XOR);

	closeWire(true);
}

U8 regRead(U8 addr)
{
	U8 dat;

	openWire();

	/* send chip address & write flag */
	send_data((CHIP_ADDRESS & 0xFE)^SIG_XOR);

	/* send register address */
	send_data(addr^SIG_XOR);

	closeWire(false);

	/* send chip address & read flag */
	send_data((CHIP_ADDRESS | 0x01)^SIG_XOR);

	/* receive register datas */
	dat = receive_data()^SIG_XOR;

	closeWire(true);

	return dat;
}



/// Read modify Write Single Bytes (3-wire control bus)
/// addr : register address
/// mask : register bit mask
/// dat  : register data
void regBitWrite(U8 addr, U8 mask, U8 dat)
{
	U8 buf;

	dat &= mask;

	openWire();

	/* send chip address & write flag */
	send_data((CHIP_ADDRESS & 0xFE)^SIG_XOR);

	/* send register address */
	send_data(addr^SIG_XOR);

	closeWire(false);

	/* send chip address & read flag */
	send_data((CHIP_ADDRESS | 0x01)^SIG_XOR);

	receive_data();

	closeWire(true);

	/*write register data*/
	openWire();

	/* send chip address & write flag */
	send_data((CHIP_ADDRESS & 0xFE)^SIG_XOR);

	/* send register address */
	send_data(addr^SIG_XOR);

	/* send register datas */
	send_data(buf^SIG_XOR);

	closeWire(true);
}


/*----------------------------------------------------------------------------*/

/* Send 8bit data */
static void send_data(U8 dat)
{
	Wire.write(dat);
}

/* Receive 8bit data */
static U8 receive_data(void)
{
	U8 dat;
	return Wire.read();
	
}

