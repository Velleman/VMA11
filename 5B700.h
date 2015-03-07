//******************************************************************************
//	5B800.H
//
//	Header file to contorol 5B800.
//******************************************************************************
#ifndef __5B800_H__
#define __5B800_H__
#include "stdint.h"
#include <Reg5B800.h>
#include "define.h"

typedef enum {
	X32_768KHz = 0,
	X7_2MHz,
	X12_0MHz,
	X16_0MHz,
	X24_0MHz
} typXTAL;

typedef union {
	uint32_t		word;
	struct {
		uint8_t	h;
		uint8_t	l;
	} byte;
} typWORD;

typedef union {
	uint16_t		word;
	struct {
		uint8_t	h;
		uint8_t	l;
	} byte;
} typFREQ;

typedef enum {
	AUDIO_DAC		= 0,
	AUDIO_IIS_32fs,
	AUDIO_IIS_64fs
} typAUDIO_OUT;


//*********************************************************************
/// Mute Setting
/// sw : MUTE_On MUTE_Off
//*********************************************************************
extern void muteEnable(unsigned char sw);

//*********************************************************************
/// Starting sequence
///		band : BAND_AM / BAND_FM
///		freq : 520 - 1720(BAND_AM) 7600 - 10810(BAND_FM)
///		aout : AUDIO_DAC : Analog audio
///			   AUDIO_IIS_32fs : IIS 32fs
///			   AUDIO_IIS_64fs : IIS 64fs
//*********************************************************************
extern void StartingSequence(typFREQ freq);


//*********************************************************************
/// Power on sequence
//*********************************************************************
static void powerOnSequence(void);

//*********************************************************************
/// Power off sequence
//*********************************************************************
extern void powerOffSequence(void);

//*********************************************************************
/// Audio Output Setting
///		aout : AUDIO_DAC : Analog audio
///			   AUDIO_IIS_32fs : IIS 32fs
///			   AUDIO_IIS_64fs : IIS 64fs
//*********************************************************************
extern void audioOutputSetting(typAUDIO_OUT aout);

//*********************************************************************
/// Seek Enable Setting
///		seek :  FALSE/TRUE
//*********************************************************************
extern void setSeekEnable(unsigned char seek);

//*********************************************************************
/// Set DC offset flag
///		dc :  FALSE/TRUE
//*********************************************************************
//extern void setDcOffsetEnable(BOOL dc);

//*********************************************************************
/// Band and Frequency Setting
///		band :  BAND_FM / BAND_AM
///		freq : (BAND_FM) 7600 - 10810 (76.00MHz - 108.10MHz)
///		freq : (BAND_AM)  520 -  1720 (  520KHz -   1720KHz)
//*********************************************************************
extern void bandAndFrequencySetting(typFREQ freq);

//*********************************************************************
/// FM Frequency Setting
///		freq : 7600 - 10810 (76.00MHz - 108.10MHz)
//*********************************************************************
extern void fmFrequencySetting(typFREQ freq);

//*********************************************************************
/// AM Frequency Setting
///		freq : 520 - 1720 (520KHz - 1720KHz)
//*********************************************************************
extern void amFrequencySetting(typFREQ freq);

//*********************************************************************
/// Constrained Mono
///		mono : TRUE / FALSE
//*********************************************************************
extern void constrainMono(unsigned char mono);

//*********************************************************************
/// FM Check Reception Sensitivity
///		sens : 0-255 Reception Sensitivity
//*********************************************************************
extern unsigned char fmCheckReceptionSensitivity(uint8_t sens);

//*********************************************************************
/// AM Check Reception Sensitivity
///		sens : 0-127 Reception Sensitivity
//*********************************************************************
extern unsigned char amCheckReceptionSensitivity(uint8_t sens);

//*********************************************************************
/// FM Check Stereo
///		RETURN : TRUE:STEREO  FALSE:MONO
//*********************************************************************
extern unsigned char fmCheckStereo(void);

//*********************************************************************
// Set De-Emphasis
//	emp : DEMP_50us/DEMP_75us
//*********************************************************************
extern void fmSetDeEmphasis(typDEMP emp);

//*********************************************************************
// Get Signal Strength
//*********************************************************************
extern uint8_t getSignalStrength(void);

//*********************************************************************
/// 5B800 FM_OV(0Eh:[7]) BW (0Eh[6:5]) setting
//*********************************************************************
typedef enum {
	FM_CF75K	= 0x00,
	FM_OM75K	= 0x80,
	FM_CF105K	= 0x20,
	FM_OM105K	= 0xA0,
	FM_CF130K	= 0x40,
	FM_CF160K	= 0x60,
	FM_OM130K	= 0xC0,
	FM_OM160K	= 0xE0,
	FM_AUTO 	= 0x30,
	FM_WAUTO 	= 0xD0
} typFMBW;

extern void fmFMOV_BwSetting(typFMBW dat);
static void dspAlignment(void);
static void dspAlignBody(uint8_t freq_num);
static void findPg(uint8_t freq_num, uint8_t agcg_num);
static void bestIml(uint8_t agcg_num);

//*********************************************************************
/// 5B800 DSP & Register Reset
//*********************************************************************
//extern void deviceReset(void);

//*********************************************************************
/// 0Fh setting
//*********************************************************************
extern void reg0FhSetting(uint8_t bmt_bw, uint8_t bmt_lvl);
//*********************************************************************
/// 11h setting
//*********************************************************************
extern void reg11hSetting(uint8_t smt);
//*********************************************************************
/// 12h setting
//*********************************************************************
extern void reg12hSetting(uint8_t hcc, uint8_t snc);
//*********************************************************************
/// 15h setting
//*********************************************************************
extern void reg15hSetting(uint8_t fo_sat_sel);
//*********************************************************************
/// 18h setting
//*********************************************************************
extern void reg18hSetting(uint8_t fxtal);
//*********************************************************************
/// Tune Output setting
//*********************************************************************
extern void tuneOutputSetting(unsigned char sw);
//*********************************************************************
/// Tune Threshold setting
//*********************************************************************
extern void tuneThresholdSetting(uint8_t th);
//*********************************************************************
/// AM Detect Threshold setting
//*********************************************************************
extern void amDetThresholdSetting(uint8_t th);
//*********************************************************************
///  LPFS HPFS setting
//*********************************************************************
extern void lpfsHpfsSetting(uint8_t lpfs, uint8_t hpfs);
//*********************************************************************
/// CE Outside Patch
//*********************************************************************
#if !CAR
//extern void ceOutsidePatch(typFREQ freq);
#else // CAR
extern void ceOutsidePatch(typFREQ freq, U8 smt);
#endif // CAR
//*********************************************************************
/// Audio output volume
//*********************************************************************
extern void audioOutputVolume(uint8_t vol);
extern void setFmAudioOutputVolume(uint8_t cf, uint8_t om);
//*********************************************************************
/// check Hung-up
//*********************************************************************
extern unsigned char checkHungUp(void);

/// Write Multiple Bytes (3-wire control bus)
extern void regWrite2(U8, U8 *, U8);

/// Write Single Bytes (3-wire control bus)
extern void regWrite(U8 addr, U8 dat);

/// Read Multiple Bytes (3-wire control bus)
extern void regRead2(U8, U8 *, U8);

/// Read Single Bytes (3-wire control bus)
extern U8 regRead(U8);

/// Read modify Write Single Bytes (3-wire control bus)
extern void regBitWrite(U8, U8, U8);

static void send_data(U8 dat);
static U8 receive_data(void);

#endif
