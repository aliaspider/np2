#include	"compiler.h"
#include	"cpucore.h"
#include	"pccore.h"
#include	"iocore.h"


// 本I/Oは 80286のみ有効です。
#if !defined(CPUCORE_IA32)

// ---- I/O

static void IOOUTCALL emsio_o08e1(UINT port, REG8 dat) {

	UINT	pos;
	UINT32	addr;

	pos = (port >> 1) & 3;
	if (!emsio.target) {
		addr = 0xc0000 + (pos << 14);
		emsio.addr[pos] = addr;
		CPU_SETEMM(pos, addr);
	}
	else if (emsio.target <= emsio.maxmem) {
		dat &= 0xfc;
		addr = (emsio.target << 20) + (dat << 12);
		emsio.addr[pos] = addr;
		CPU_SETEMM(pos, addr);
	}
}

static void IOOUTCALL emsio_o08e9(UINT port, REG8 dat) {

	emsio.target = (UINT8)(dat & 0x0f);
	(void)port;
}

static REG8 IOINPCALL emsio_i08e9(UINT port) {

	if ((emsio.target) && (emsio.target <= emsio.maxmem)) {
		return(0);
	}
	else {
		return(0xff);
	}
	(void)port;
}

// ---- I/F

void emsio_reset(void) {

	ZeroMemory(&emsio, sizeof(emsio));
	emsio.maxmem = pccore.extmem;
	emsio.addr[0] = 0xc0000;
	emsio.addr[1] = 0xc4000;
	emsio.addr[2] = 0xc8000;
	emsio.addr[3] = 0xcc000;
}

void emsio_bind(void) {

	UINT	i;

	for (i=0; i<4; i++) {
		CPU_SETEMM(i, emsio.addr[i]);
	}
	if (emsio.maxmem) {
		iocore_attachout(0x08e1, emsio_o08e1);
		iocore_attachout(0x08e3, emsio_o08e1);
		iocore_attachout(0x08e5, emsio_o08e1);
		iocore_attachout(0x08e7, emsio_o08e1);
		iocore_attachout(0x08e9, emsio_o08e9);
		iocore_attachinp(0x08e9, emsio_i08e9);
	}
}
#endif

