#ifndef H_XTYPES_H
#define H_XTYPES_H

// FOR AVR only (use other definitions for PC)
#ifdef __CODEVISIONAVR__
// ============ AVR C definitions ===================

// do not promote char to int
#pragma promotechar-

// optimize for speed
#pragma optsize-

typedef unsigned char UCHAR;
typedef signed char SCHAR;

typedef unsigned int XUINT16;
typedef signed int XSINT16;

typedef unsigned long XUINT32;
typedef signed long XSINT32;

//const UINT MAX_UINT = 0xFFFF;
//const ULONG MAX_ULONG = 0xFFFFFFFF;
//const UINT MAX_SINT = 32767;
//const UCHAR CHAR_LENGTH = sizeof(char); // 1 byte
//const UCHAR UINT_LENGTH = sizeof(UINT); // 2 bytes

#else
// ============ Visual C++ or GCC/MinGW ===================
typedef unsigned char UCHAR;
typedef signed char SCHAR;

typedef unsigned short XUINT16;
typedef signed short XSINT16;

typedef unsigned int XUINT32;
typedef signed int XSINT32;

typedef XUINT32 XTIME;

#endif


// Timer type
#if (XTIME_USE_FAST_TIMER)
typedef XUINT16 XTIME;
const XTIME MAX_XTIME = 0xFFFF;
#else
typedef XUINT32 XTIME;
const XTIME MAX_XTIME = 0xFFFFFFFF;
#endif




#endif