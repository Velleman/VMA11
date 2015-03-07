//******************************************************************************
//	DEFINE.H
//
//	Header file to define alias.
//******************************************************************************
#ifndef __DEFINE_H__
#define __DEFINE_H__

#include <REGFX2LP.H>

typedef unsigned char	U8;
typedef unsigned short	U16;
typedef signed char		S8;
typedef signed short	S16;

typedef unsigned char	BOOL;
#define TRUE		1
#define FALSE		0

/****************************************************************************************/
#define A5B800			FALSE
#define A5B700			!A5B800
#define RDS				FALSE
#define CAR				FALSE
#define HOST_I2C		TRUE				/* TRUE	: HOST I2C Mode		*/
/****************************************************************************************/

/*--------------------------------------------------------------------------------------*/
#if A5B800
  #define _TARGET_NAME		"5B800"
  #define _VERSION			"3.26 "
#endif //A5B800

#if A5B700
  #define _TARGET_NAME		"5B700"
  #define _VERSION			"3.16 "
#endif //A5B700

#define CHIP_ADDRESS	0xC8				/* Terget device chip address */
#define CLOCK			24000000.0			/* CPU clock 24000KHz type:double */
#define DATE_STAMP		"Dec 08 2010"

#define CODE_RANGE		0x0000 - 0x3FC7		/* code		: 16328 Byte		*/
#define XDATA_RANGE		0xE000 - 0xE1FF		/* xdata	:   512 Byte		*/
#define VALUE1_ADDRESS	0x3FC8				/* value1	:    56 Byte		*/

#define SIG_INV			TRUE				/* Switch to reverse serial I/O signals. */
#define USB64K			FALSE
#define CEPATCH			FALSE
/*--------------------------------------------------------------------------------------*/

#if HOST_I2C
#define _HOST_IF	"i"
#else
#define _HOST_IF	"s"
#endif

#if !RDS
#if A5B800
#define TARGET_NAME		_TARGET_NAME _HOST_IF"ES3.1 Test"
#endif // A5B800
#if A5B700
#define TARGET_NAME		_TARGET_NAME _HOST_IF"ES1.0 Test"
#endif // A5B700

#else
#define TARGET_NAME		_TARGET_NAME _HOST_IF"RDS Demo  "
#endif //RDS

#if !CEPATCH
	#if !CAR
		#define VERSION			" "_VERSION" Home"
	#else
		#define VERSION			" "_VERSION" Car "
	#endif
#else
	#if !CAR
		#define VERSION			"P"_VERSION" Home"
	#else
		#define VERSION			"P"_VERSION" Car "
	#endif
#endif

#if SIG_INV
	#define SIG_LOW		1
	#define SIG_HIGH	0
	#define SIG_XOR		0xFF
#else
	#define SIG_LOW		0
	#define SIG_HIGH	1
	#define SIG_XOR		0x00
#endif

#if !USB64K
#define DEV_LA		PA0
#define DEV_CK		PA1
#define DEV_DAO		PA2
#define DEV_DAI		PA3
#define LCD_DB4		PA4
#define LCD_DB5		PA5
#define LCD_DB6		PA6
#define LCD_DB7		PA7
#define LCD_DB		PA
#define LCD_OEDB	OEA
#else
#define DEV_LA		PC0
#define DEV_CK		PC1
#define DEV_DAO		PC2
#define DEV_DAI		PC3
#define LCD_DB4		PC4
#define LCD_DB5		PC5
#define LCD_DB6		PC6
#define LCD_DB7		PC7
#define LCD_DB		PC
#define LCD_OEDB	OEC
#endif

#define LCD_RS		PB5
#define LCD_RW		PB6
#define LCD_E		PB7


#define DEV_IIC		PD3
#define DEV_RDINT	PD2
#define AM_TUNE		PD2

#define IOP_SW1R	PD4
#define IOP_SW2R	PD5
#define IOP_SW3R	PD6
#define IOP_SW1C	PB0
#define IOP_SW2C	PB1
#define IOP_SW3C	PB2

#define LED_STEREO	PB3
#define LED_TUNE	PB4

#if !RDS
#define UNUSED2		PD0
#define UNUSED3		PD1
#else // RDS
#define RXD			PD0
#define RDSEN		PD1
#endif //RDS

#define UNUSED4		PD7

#endif
