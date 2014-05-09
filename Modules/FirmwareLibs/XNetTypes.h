#ifndef H_NET_ATM64_TYPES_H
#define H_NET_ATM64_TYPES_H

#include "XDataTypes.h"

//#include "XUSART64.h"
//#include "string.h"



// ========================== NETWORK PACKET ============================

#ifndef __CODEVISIONAVR__
#pragma pack(push,1)
#endif

const UCHAR NET_SIGNATURE1 = 0xFF;
const UCHAR NET_SIGNATURE2 = 0xFE;

// MCU answers by setting highest bit to the answer cmd
const UCHAR NET_ANS_COMMAND_MASK = 0b10000000; // 0x80



// header follows any response or request (see code section)
typedef struct tagNET_PACKETHEADER
{
	UCHAR signature1;	// 0xFF
	UCHAR signature2;	// 0xFE
	UCHAR length;		// length of the buffer following the header
	UCHAR cmd;
} NET_PACKETHEADER;



// _________________________________ EXTRA PACKET BYTES __________________________________

// =======================================================================================
// ======================== GENERIC PACKETS ==============================================
// =======================================================================================

/*
	Generic requests samples. This works on any device.

	REQ_ECHO		= $FF$FE$00$01
	REQ_DEVICECLASS	= $FF$FE$00$02
	REQ_DEVICENAME	= $FF$FE$00$03
 */

// All command codes must be less than 127
// === General commands ===
const UCHAR NET_CMD_REQ_ECHO = 0x01;
const UCHAR NET_CMD_REQ_DEVICECLASS = 0x02; // this command requests device class
const UCHAR NET_CMD_REQ_DEVICENAME = 0x03; // this command requests version

// === Device specific commands ===
const UCHAR NET_CMD_REQ_DEVICE_SPECIFIC = 0x20; // this is the minimum device-specific code





// === ITE HARDWARE MODULE DEVICE CLASS
const UCHAR DEVICE_CLASS_CDC = 1;
const UCHAR DEVICE_CLASS_PMC_MOTION = 2;
const UCHAR DEVICE_CLASS_PMC_ROTATION = 3;
const UCHAR DEVICE_CLASS_PPC = 4;
const UCHAR DEVICE_CLASS_NAVI = 5;

typedef struct tagNET_PACKET_ANS_DEVICECLASS
{
	UCHAR device_class;
} NET_PACKET_ANS_DEVICECLASS;


// === DEVICE NAME
typedef struct tagNET_PACKET_ANS_DEVICENAME
{
	UCHAR devicename[20];
} NET_PACKET_ANS_DEVICENAME;



// Request and Answer packet Extra sizes
const UCHAR NET_CMD_SIZE_REQ_ECHO = 0;
const UCHAR NET_CMD_SIZE_ANS_ECHO = 0;

const UCHAR NET_CMD_SIZE_REQ_DEVICECLASS = 0;
const UCHAR NET_CMD_SIZE_ANS_DEVICECLASS = sizeof(NET_PACKET_ANS_DEVICECLASS);

const UCHAR NET_CMD_SIZE_REQ_DEVICENAME = 0;
const UCHAR NET_CMD_SIZE_ANS_DEVICENAME = sizeof(NET_PACKET_ANS_DEVICENAME);





#ifndef __CODEVISIONAVR__
#pragma pack(pop)
#endif





#endif // H_NET_ATM64_TYPES_H
