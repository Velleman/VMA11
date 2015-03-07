//******************************************************************************
//	Rds.H
//
//	Header file to RDS.
//******************************************************************************
#ifndef __RDS_H__
#define __RDS_H__

typedef enum {
	RDS_0A		= 0x00,
	RDS_0B		= 0x08,
	RDS_1A		= 0x10,
	RDS_1B		= 0x18,
	RDS_2A		= 0x20,
	RDS_2B		= 0x28,
	RDS_4A		= 0x40,
	RDS_other	= 0xFF
} typGroupType;

//*********************************************************************
/// Start RDS
//*********************************************************************
extern void startRds(void);

//*********************************************************************
/// Stop RDS
//*********************************************************************
extern void stopRds(void);

//*********************************************************************
/// Check RDS receive
//*********************************************************************
extern void checkRdsReceive(void);

//*********************************************************************
/// Get RDS Data
//*********************************************************************
extern U8 getRdsData0A(U8 addr);
extern U8 getRdsData0B(U8 addr);
extern U8 getRdsData2A(U8 addr);
extern U8 getRdsData2B(U8 addr);
extern U8 getRdsDateTime(U8 addr);
extern U8 getRdsPTY(U8 addr);
extern BOOL checkRdsReceived(void);

//*********************************************************************
/// Get RDS Data length
//*********************************************************************
extern U8 getRdsDataLength0A(void);
extern U8 getRdsDataLength0B(void);
extern U8 getRdsDataLength2A(void);
extern U8 getRdsDataLength2B(void);

#endif
