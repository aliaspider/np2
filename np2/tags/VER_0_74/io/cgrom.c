#include	"compiler.h"
#include	"cpucore.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"font.h"


static void cgwindowset(CGROM cr) {

	UINT	low;
	UINT	high;
	int		code;

	cgwindow.writable &= ~1;
	low = 0x7fff0;
	if (grcg.chip >= 2) {
		if (!(cr->code & 0xff00)) {
			high = 0x80000 + (cr->code << 4);
			if (!(gdc.mode1 & 8)) {
				high += 0x2000;
			}
		}
		else {
			code = cr->code & 0x007f;
			high = (cr->code & 0x7f7f) << 4;
			if ((code >= 0x56) && (code < 0x58)) {
				cgwindow.writable |= 1;
				high += cr->lr;
			}
			else if (((code >= 0x0c) && (code < 0x10)) ||
				((code >= 0x58) && (code < 0x60))) {
				high += cr->lr;
			}
			else if ((code < 0x08) || (code >= 0x10)) {
				low = high;
				high += 0x800;
			}
		}
	}
	else {
		high = low;
	}
#if !defined(CGWND_FONTPTR)
	cgwindow.low = low;
	cgwindow.high = high;
#else
	cgwindow.fontlow = fontrom + low;
	cgwindow.fonthigh = fontrom + high;
#endif
}


// ---- I/O

// write charactor code low
static void IOOUTCALL cgrom_oa1(UINT port, REG8 dat) {

	CGROM	cr;

	cr = &cgrom;
	cr->code = (dat << 8) | (cr->code & 0xff);
	cgwindowset(cr);
	(void)port;
}

// write charactor code high
static void IOOUTCALL cgrom_oa3(UINT port, REG8 dat) {

	CGROM	cr;

	cr = &cgrom;
	cr->code = (cr->code & 0xff00) | dat;
	cgwindowset(cr);
	(void)port;
}

// write charactor line
static void IOOUTCALL cgrom_oa5(UINT port, REG8 dat) {

	CGROM	cr;

	cr = &cgrom;
	cr->line = dat & 0x0f;
	cr->lr = ((~dat) & 0x20) << 6;
	cgwindowset(cr);
	(void)port;
}

// CG write pattern
static void IOOUTCALL cgrom_oa9(UINT port, REG8 dat) {

	CGROM	cr;

	cr = &cgrom;
	if ((cr->code & 0x007e) == 0x0056) {
		fontrom[((cr->code & 0x7f7f) << 4) +
							cr->lr + cr->line] = (UINT8)dat;
		cgwindow.writable |= 0x80;
	}
	(void)port;
}

static REG8 IOINPCALL cgrom_ia9(UINT port) {

	CGROM	cr;
const BYTE	*ptr;

	cr = &cgrom;
	ptr = fontrom;
	if (cr->code & 0xff00) {
		ptr += (cr->code & 0x7f7f) << 4;
		ptr += cr->lr;
	}
	else {
		ptr += 0x80000;
		ptr += cr->code << 4;
	}
	(void)port;
	return(ptr[cr->line]);
}


// ---- I/F

static const IOOUT cgromoa1[8] = {
					cgrom_oa1,	cgrom_oa3,	cgrom_oa5,	NULL,
					cgrom_oa9,	NULL,		NULL,		NULL};

static const IOINP cgromia1[8] = {
					NULL,		NULL,		NULL,		NULL,
					cgrom_ia9,	NULL,		NULL,		NULL};

void cgrom_reset(void) {

	CGWINDOW	cgw;

	cgw = &cgwindow;
	ZeroMemory(cgw, sizeof(cgrom));
#if !defined(CGWND_FONTPTR)
	cgw->low = 0x7fff0;
	cgw->high = 0x7fff0;
#else
	cgw->fontlow = fontrom + 0x7fff0;
	cgw->fonthigh = fontrom + 0x7fff0;
#endif
	cgw->writable = 0;
}

void cgrom_bind(void) {

	iocore_attachsysoutex(0x00a1, 0x0cf1, cgromoa1, 8);
	iocore_attachsysinpex(0x00a1, 0x0cf1, cgromia1, 8);
}

