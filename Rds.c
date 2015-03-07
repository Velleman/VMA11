#include "string.h"
#include <DEFINE.H>
#include <Serial.h>
#include <Lcd.H>
#include <Timer.h>
#include <RDS.H>

#if RDS
//==============================================================================
//
//	5B800 RDS function
//
//==============================================================================
static BOOL xdata rdsEnable = FALSE;
static U8 xdata strRdsData0A[8];
static U8 xdata strRdsData0B[8];
static U8 xdata strRdsData2A[64];
static U8 xdata strRdsData2B[32];
static U8 xdata rdsDataLength0A;
static U8 xdata rdsDataLength0B;
static U8 xdata rdsDataLength2A;
static U8 xdata rdsDataLength2B;
static U8 xdata blockNumber;
static U8 xdata segmentAddress0A, segmentAddress0B;
static U8 xdata segmentAddress2A, segmentAddress2B;
static U8 xdata rds_id = 0;
static BOOL xdata rdsReceived;

static U16 xdata rdsPIc;
static double xdata rdsMJD;
static S8 xdata rdsHour;
static S8 xdata rdsLocalTimeOfset;
static U8 xdata strDateTime[16];

static U8 xdata rdsPTY;
static U8 code strRdsTPY[16][8] = {
	"None    ",
	"News    ",
	"Affairs ",
	"Info    ",
	"Sport   ",
	"Educate ",
	"Drama   ",
	"Culture ",
	"Science ",
	"Varied  ",
	"Pop M   ",
	"Rock M  ",
	"Easy M  ",
	"Light M ",
	"Classics",
	"Other M "
};

static U8 code strMonth[12][3] = {
	"Jan",
	"Feb",
	"Mar",
	"Apr",
	"May",
	"Jun",
	"Jul",
	"Aug",
	"Sep",
	"Oct",
	"Nov",
	"Dec"
};

static void dec2str(U16 d, U8 *s)
{
	U8 buf;

	buf = d / 1000;
	d -= buf * 1000;
	s[0] = buf + '0';

	buf = d / 100;
	d -= buf * 100;
	s[1] = buf + '0';

	buf = d / 10;
	d -= buf * 10;
	s[2] = buf + '0';

	s[3] = d + '0';
}

//*********************************************************************
/// Start RDS
//*********************************************************************
static void initializeRds(void)
{
	if(rds_id == 0)
	{
		rds_id = getTimerID();
	}

	rdsReceived = FALSE;
	rdsDataLength0A = 0;
	rdsDataLength0B = 0;
	rdsDataLength2A = 0;
	rdsDataLength2B = 0;
	memset(&strRdsData0A[0], ' ', sizeof(strRdsData0A));
	memset(&strRdsData0B[0], ' ', sizeof(strRdsData0B));
	memset(&strRdsData2A[0], ' ', sizeof(strRdsData2A));
	memset(&strRdsData2B[0], ' ', sizeof(strRdsData2B));
	strcpy(&strDateTime[0], "--- -- '-- --:--");
	blockNumber = 0;
	rdsPIc = 0;
	rdsPTY = 0xFF;
}

//*********************************************************************
/// Start RDS
//*********************************************************************
void startRDS(void)
{
	initializeRds();
	regWrite(0x13, 0x13);
	rdsEnable = TRUE;
}

//*********************************************************************
/// Stop RDS
//*********************************************************************
void stopRds(void)
{
	regWrite(0x13, 0x12);
	rdsEnable = FALSE;
}

//*********************************************************************
/// Check RDS receive
//*********************************************************************
void checkRdsReceive(void)
{
	U16 rdsDateTime_y;
	U8 rdsDateTime_m;
	U8 rdsDateTime_d;
	U8 rdsDateTime_hour;
	U8 rdsDateTime_minute;

	U8 reg07, reg08, reg09;
	U8 buff[4];
	U16 regBuf16;

	U8 blockId, addr, j;// k;
	typGroupType grTyp;

	if(!rdsEnable) return;		/* RDS disable */

	buff[0] = reg07 = regRead(0x07);
	buff[1] = reg08 = regRead(0x08);
	buff[2] = reg09 = regRead(0x09);

	if(reg07 & 0x04)
	{	/* not receive */
		if(rdsReceived == TRUE)
		{
			if(time1msCheckBusy(rds_id) == FALSE)
			{
				initializeRds();
			}
		}
		return;
	}
	else
	{	/* receive */
		rdsReceived = TRUE;
		setTime1ms(rds_id, 200);
	}
/*
	RDSEN = 1;
	EA = 0;
	for(j=0; j<200; j++);
	for(k=0; k<3; k++)
	{
		RXD = 0;	// start bit
		for(j=0; j<48; j++);

		RXD = (buff[k] & 0x01) ? 1:0;
        RDSEN = 1;
		for(j=0; j<48; j++);

		RXD = (buff[k] & 0x02) ? 1:0;
		for(j=0; j<48; j++);

		RXD = (buff[k] & 0x04) ? 1:0;
        RDSEN = 1;
		for(j=0; j<48; j++);

		RXD = (buff[k] & 0x08) ? 1:0;
		for(j=0; j<48; j++);

		RXD = (buff[k] & 0x10) ? 1:0;
        RDSEN = 1;
		for(j=0; j<48; j++);

		RXD = (buff[k] & 0x20) ? 1:0;
		for(j=0; j<48; j++);

		RXD = (buff[k] & 0x40) ? 1:0;
        RDSEN = 1;
		for(j=0; j<48; j++);

		RXD = (buff[k] & 0x80) ? 1:0;
        RDSEN = 1;
		for(j=0; j<51; j++);

		RXD = 1;	// stop bit
		for(j=0; j<200; j++);
	}
	EA = 1;
	RDSEN = 0;
*/
	if((reg07 & 0x01) != 0x01)	/* sync error */
	{
		initializeRds();
		return;
	}

	if((reg07 & 0xC0) != 0x80)	/* receive error */
	{
		blockNumber = 0;
		return;
	}

	/* analysis data */
	blockId = (reg07 & 0x38) >> 3;

	switch(blockNumber)
	{
		case 0:

			if(blockId == 0)
			{
				rdsPIc = (reg09 << 8) + reg08;
				blockNumber = 1;
			}
			break;

		case 1:
			if(blockId == 1)
			{
				grTyp = reg09 & 0xF8;
				rdsPTY = ((reg09 & 0x03) << 3) + ((reg08 & 0xE0) >> 5);
				switch(grTyp)
				{
					case RDS_0A:
						segmentAddress0A = reg08 & 0x03;
						break;

					case RDS_0B:
						segmentAddress0B = reg08 & 0x03;
						break;

					case RDS_2A:
						segmentAddress2A = reg08 & 0x0F;
						break;

					case RDS_2B:
						segmentAddress2B = reg08 & 0x0F;
						break;

					case RDS_4A:
						rdsMJD = (double)(reg08&0x03) * 32768.0; // MJD[16:15] * 0x8000
					    break;

					default:
//						initializeRds();
blockNumber = 0;
						return;
				}
				blockNumber = 2;
			}
			else
			{
				blockNumber = 0;
			}
			break;

		case 2:
			if((blockId == 2) || (blockId == 5))
			{
				switch(grTyp)
				{
					case RDS_0A:
						break;

					case RDS_0B:
						break;

					case RDS_2A:
						addr = segmentAddress2A << 2;
						strRdsData2A[addr] = reg09;
						strRdsData2A[addr + 1] = reg08;
						if(rdsDataLength2A < (addr + 2))
						{
							rdsDataLength2A = addr + 2;
						}
						break;

					case RDS_2B:
						break;

					case RDS_4A:
						regBuf16 = (((U16)reg09) << 7) + (((U16)reg08) >> 1);
						rdsMJD += (double)regBuf16; // MJD[14:0]
						rdsHour = (S8)((reg08&0x01) << 4);
						break;

					default:
						break;
				}
				blockNumber = 3;
			}
			else
			{
				blockNumber = 0;
			}
			break;

		case 3:
			if(blockId == 3)
			{
				switch(grTyp)
				{
					case RDS_0A:
						addr = segmentAddress0A << 1;
						strRdsData0A[addr] = reg09;
						strRdsData0A[addr + 1] = reg08;
						if(rdsDataLength0A < (addr + 2))
						{
							rdsDataLength0A = addr + 2;
						}
						break;

					case RDS_0B:
						addr = segmentAddress0B << 1;
						strRdsData0B[addr] = reg09;
						strRdsData0B[addr + 1] = reg08;
						if(rdsDataLength0B < (addr + 2))
						{
							rdsDataLength0B = addr + 2;
						}
						break;

					case RDS_2A:
						addr = (segmentAddress2A << 2) + 2;
						strRdsData2A[addr] = reg09;
						strRdsData2A[addr + 1] = reg08;
						if(rdsDataLength2A < (addr + 2))
						{
							rdsDataLength2A = addr + 2;
						}
						break;

					case RDS_2B:
						addr = segmentAddress2B << 1;
						strRdsData2B[addr] = reg09;
						strRdsData2B[addr + 1] = reg08;
						if(rdsDataLength2B < (addr + 2))
						{
							rdsDataLength2B = addr + 2;
						}
						break;

					case RDS_4A:
						rdsDateTime_y = 0;
						rdsDateTime_m = 0;
						rdsDateTime_d = 0;
						rdsDateTime_hour = 0;
						rdsDateTime_minute = 0;

						rdsHour += (S8)((reg09&0xF0) >> 4);
						rdsDateTime_minute = (reg09&0x0F) << 2;
						rdsDateTime_minute += (reg08&0xC0) >> 6;
						rdsLocalTimeOfset = (S8)(reg08&0x1F);
						if(reg08 & 0x20)
						{
							rdsLocalTimeOfset = 0 - rdsLocalTimeOfset;
						}
						rdsHour += rdsLocalTimeOfset;
						if(rdsHour < 0)
						{
							rdsHour += 24;
							rdsMJD--;
						}
						else if(rdsHour > 23)
						{
							rdsHour -= 24;
							rdsMJD++;
						}
						rdsDateTime_hour = (U8)rdsHour;

						rdsDateTime_y = (U16)((rdsMJD - 15078.2) / 365.25);
						rdsDateTime_m = (U8)((rdsMJD - 14956.1 - (U16)(rdsDateTime_y * 365.25)) / 30.6001);
						rdsDateTime_d = (U8)(rdsMJD - 14956 - (U16)(rdsDateTime_y * 365.25) - (U16)(rdsDateTime_m * 30.6001));

						rdsDateTime_y += 1900;
						rdsDateTime_m -= 1;
						if(rdsDateTime_m > 12)
						{
							rdsDateTime_y += 1;
							rdsDateTime_m -= 12;
						}

						// Set Month
						for(j=0;j<3;j++)
						{
							strDateTime[j] = strMonth[rdsDateTime_m - 1][j];
						}

						// Set Day
						dec2str((U16)rdsDateTime_d, (U8 *)&buff[0]);
						strDateTime[4] = buff[2];
						strDateTime[5] = buff[3];

						// Set Year
						dec2str(rdsDateTime_y + 1900, (U8 *)&buff[0]);
						strDateTime[8] = buff[2];
						strDateTime[9] = buff[3];

						// Set Hour
						dec2str(rdsDateTime_hour, (U8 *)&buff[0]);
						strDateTime[11] = buff[2];
						strDateTime[12] = buff[3];

						// Set Minute
						dec2str(rdsDateTime_minute, (U8 *)&buff[0]);
						strDateTime[14] = buff[2];
						strDateTime[15] = buff[3];
						break;

					default:
						break;
				}
			}

		default :
			blockNumber = 0;
			break;
	}
}

//*********************************************************************
/// Get RDS Data
//*********************************************************************
U8 getRdsData0A(U8 addr)
{
	return strRdsData0A[addr];
}
U8 getRdsData0B(U8 addr)
{
	return strRdsData0B[addr];
}
U8 getRdsData2A(U8 addr)
{
	return strRdsData2A[addr];
}
U8 getRdsData2B(U8 addr)
{
	return strRdsData2B[addr];
}

U8 getRdsDateTime(U8 addr)
{
	return strDateTime[addr];
}

U8 getRdsPTY(U8 addr)
{
	return strRdsTPY[rdsPTY][addr];
}

BOOL checkRdsReceived(void)
{
	return rdsReceived;
}

//*********************************************************************
/// Get RDS Data length
//*********************************************************************
U8 getRdsDataLength0A(void)
{
	return rdsDataLength0A;
}
U8 getRdsDataLength0B(void)
{
	return rdsDataLength0B;
}
U8 getRdsDataLength2A(void)
{
	return rdsDataLength2A;
}
U8 getRdsDataLength2B(void)
{
	return rdsDataLength2B;
}

#endif // RDS
