#include	<windows.h>
#include	<stdio.h>
#include	<stddef.h>
#include	<setjmp.h>

#define	BYTESEX_LITTLE
#define	OSLANG_SJIS
#define	OSLINEBREAK_CRLF


#ifndef __GNUC__
typedef signed char			SINT8;
typedef unsigned char		UINT8;
typedef	signed short		SINT16;
typedef	unsigned short		UINT16;
typedef	signed int			SINT32;
typedef	unsigned int		UINT32;
typedef signed __int64		SINT64;
typedef unsigned __int64	UINT64;
#define	INLINE				__inline
#else
#include	<stdlib.h>
typedef signed char			SINT8;
typedef unsigned char		UINT8;
typedef	short				SINT16;
typedef	unsigned short		UINT16;
typedef	int					SINT32;
typedef signed __int64		SINT64;
#define	INLINE				inline
#endif

// for RISC test
#define	REG8		UINT
#define REG16		UINT


#include	"common.h"
#include	"milstr.h"
#include	"_memory.h"
#include	"rect.h"
#include	"lstarray.h"
#include	"trace.h"

#define	GETTICK()	GetTickCount()
#define	SPRINTF		wsprintf
#define	__ASSERT(s)

#define	LABEL		__declspec(naked)
#define RELEASE(x) 	if (x) {(x)->Release(); (x) = NULL;}

#define	VERMOUTH_LIB
#define	PARTSCALL	__fastcall
#define	CPUCALL		__fastcall
#define	MEMCALL		__fastcall
#define DMACCALL	__fastcall
#define	IOOUTCALL	__fastcall
#define	IOINPCALL	__fastcall
#define SOUNDCALL	__fastcall
#define	VRAMCALL	__fastcall
#define	SCRNCALL	__fastcall

#define	SUPPORT_SJIS

// #define	SUPPORT_8BPP
// #define	SUPPORT_16BPP
// #define	SUPPORT_24BPP
#define	SUPPORT_32BPP
#define SUPPORT_NORMALDISP
#define	MEMOPTIMIZE		1

#define	SOUNDRESERVE	20

#define	SUPPORT_WAVEMIX

#define	FASTCALL	__fastcall
