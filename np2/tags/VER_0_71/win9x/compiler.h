
#define	_WIN32_IE	0x0200

#include	<windows.h>
#include	<stdio.h>
#include	<stddef.h>

#define	BYTESEX_LITTLE

#ifndef __GNUC__

typedef	short			SINT16;
typedef	unsigned short	UINT16;
typedef	int				SINT32;
typedef	unsigned int	UINT32;

#else

#include	<stdlib.h>
typedef	short			SINT16;
typedef	unsigned short	UINT16;
typedef	int				SINT32;

#endif


#define	LOADINTELDWORD(a)		(*((UINT32 *)(a)))
#define	LOADINTELWORD(a)		(*((UINT16 *)(a)))
#define	STOREINTELDWORD(a, b)	*(UINT32 *)(a) = (b)
#define	STOREINTELWORD(a, b)	*(UINT16 *)(a) = (b)


#include	"common.h"
#include	"milstr.h"
#include	"_memory.h"
#include	"rect.h"
#include	"lstarray.h"
#include	"trace.h"

#define	GETTICK()			GetTickCount()
#define	GETRAND()			rand()
#define	SPRINTF				wsprintf

#define	LABEL		__declspec(naked)
#define RELEASE(x) 	if (x) {(x)->Release(); (x) = NULL;}

#define	OPNGENX86
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

#define	SUPPORT_8BPP
#define	SUPPORT_16BPP
#define	SUPPORT_24BPP
#define	SUPPORT_32BPP
#define SUPPORT_NORMALDISP

#define	SOUNDRESERVE	20
