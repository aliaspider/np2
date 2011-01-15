#include	"compiler.h"
#include	"cpucore.h"

/* Z_FLAG, S_FLAG, P_FLAG, C_FLAG */
const UINT8 iflags[512] = {
	0x44, 0x00, 0x00, 0x04, 0x00, 0x04, 0x04, 0x00,
	0x00, 0x04, 0x04, 0x00, 0x04, 0x00, 0x00, 0x04,
	0x00, 0x04, 0x04, 0x00, 0x04, 0x00, 0x00, 0x04,
	0x04, 0x00, 0x00, 0x04, 0x00, 0x04, 0x04, 0x00,
	0x00, 0x04, 0x04, 0x00, 0x04, 0x00, 0x00, 0x04,
	0x04, 0x00, 0x00, 0x04, 0x00, 0x04, 0x04, 0x00,
	0x04, 0x00, 0x00, 0x04, 0x00, 0x04, 0x04, 0x00,
	0x00, 0x04, 0x04, 0x00, 0x04, 0x00, 0x00, 0x04,
	0x00, 0x04, 0x04, 0x00, 0x04, 0x00, 0x00, 0x04,
	0x04, 0x00, 0x00, 0x04, 0x00, 0x04, 0x04, 0x00,
	0x04, 0x00, 0x00, 0x04, 0x00, 0x04, 0x04, 0x00,
	0x00, 0x04, 0x04, 0x00, 0x04, 0x00, 0x00, 0x04,
	0x04, 0x00, 0x00, 0x04, 0x00, 0x04, 0x04, 0x00,
	0x00, 0x04, 0x04, 0x00, 0x04, 0x00, 0x00, 0x04,
	0x00, 0x04, 0x04, 0x00, 0x04, 0x00, 0x00, 0x04,
	0x04, 0x00, 0x00, 0x04, 0x00, 0x04, 0x04, 0x00,
	0x80, 0x84, 0x84, 0x80, 0x84, 0x80, 0x80, 0x84,
	0x84, 0x80, 0x80, 0x84, 0x80, 0x84, 0x84, 0x80,
	0x84, 0x80, 0x80, 0x84, 0x80, 0x84, 0x84, 0x80,
	0x80, 0x84, 0x84, 0x80, 0x84, 0x80, 0x80, 0x84,
	0x84, 0x80, 0x80, 0x84, 0x80, 0x84, 0x84, 0x80,
	0x80, 0x84, 0x84, 0x80, 0x84, 0x80, 0x80, 0x84,
	0x80, 0x84, 0x84, 0x80, 0x84, 0x80, 0x80, 0x84,
	0x84, 0x80, 0x80, 0x84, 0x80, 0x84, 0x84, 0x80,
	0x84, 0x80, 0x80, 0x84, 0x80, 0x84, 0x84, 0x80,
	0x80, 0x84, 0x84, 0x80, 0x84, 0x80, 0x80, 0x84,
	0x80, 0x84, 0x84, 0x80, 0x84, 0x80, 0x80, 0x84,
	0x84, 0x80, 0x80, 0x84, 0x80, 0x84, 0x84, 0x80,
	0x80, 0x84, 0x84, 0x80, 0x84, 0x80, 0x80, 0x84,
	0x84, 0x80, 0x80, 0x84, 0x80, 0x84, 0x84, 0x80,
	0x84, 0x80, 0x80, 0x84, 0x80, 0x84, 0x84, 0x80,
	0x80, 0x84, 0x84, 0x80, 0x84, 0x80, 0x80, 0x84,
	0x45, 0x01, 0x01, 0x05, 0x01, 0x05, 0x05, 0x01,
	0x01, 0x05, 0x05, 0x01, 0x05, 0x01, 0x01, 0x05,
	0x01, 0x05, 0x05, 0x01, 0x05, 0x01, 0x01, 0x05,
	0x05, 0x01, 0x01, 0x05, 0x01, 0x05, 0x05, 0x01,
	0x01, 0x05, 0x05, 0x01, 0x05, 0x01, 0x01, 0x05,
	0x05, 0x01, 0x01, 0x05, 0x01, 0x05, 0x05, 0x01,
	0x05, 0x01, 0x01, 0x05, 0x01, 0x05, 0x05, 0x01,
	0x01, 0x05, 0x05, 0x01, 0x05, 0x01, 0x01, 0x05,
	0x01, 0x05, 0x05, 0x01, 0x05, 0x01, 0x01, 0x05,
	0x05, 0x01, 0x01, 0x05, 0x01, 0x05, 0x05, 0x01,
	0x05, 0x01, 0x01, 0x05, 0x01, 0x05, 0x05, 0x01,
	0x01, 0x05, 0x05, 0x01, 0x05, 0x01, 0x01, 0x05,
	0x05, 0x01, 0x01, 0x05, 0x01, 0x05, 0x05, 0x01,
	0x01, 0x05, 0x05, 0x01, 0x05, 0x01, 0x01, 0x05,
	0x01, 0x05, 0x05, 0x01, 0x05, 0x01, 0x01, 0x05,
	0x05, 0x01, 0x01, 0x05, 0x01, 0x05, 0x05, 0x01,
	0x81, 0x85, 0x85, 0x81, 0x85, 0x81, 0x81, 0x85,
	0x85, 0x81, 0x81, 0x85, 0x81, 0x85, 0x85, 0x81,
	0x85, 0x81, 0x81, 0x85, 0x81, 0x85, 0x85, 0x81,
	0x81, 0x85, 0x85, 0x81, 0x85, 0x81, 0x81, 0x85,
	0x85, 0x81, 0x81, 0x85, 0x81, 0x85, 0x85, 0x81,
	0x81, 0x85, 0x85, 0x81, 0x85, 0x81, 0x81, 0x85,
	0x81, 0x85, 0x85, 0x81, 0x85, 0x81, 0x81, 0x85,
	0x85, 0x81, 0x81, 0x85, 0x81, 0x85, 0x85, 0x81,
	0x85, 0x81, 0x81, 0x85, 0x81, 0x85, 0x85, 0x81,
	0x81, 0x85, 0x85, 0x81, 0x85, 0x81, 0x81, 0x85,
	0x81, 0x85, 0x85, 0x81, 0x85, 0x81, 0x81, 0x85,
	0x85, 0x81, 0x81, 0x85, 0x81, 0x85, 0x85, 0x81,
	0x81, 0x85, 0x85, 0x81, 0x85, 0x81, 0x81, 0x85,
	0x85, 0x81, 0x81, 0x85, 0x81, 0x85, 0x85, 0x81,
	0x85, 0x81, 0x81, 0x85, 0x81, 0x85, 0x85, 0x81,
	0x81, 0x85, 0x85, 0x81, 0x85, 0x81, 0x81, 0x85
};

UINT8 szpflag_w[0x10000];


void i386c_initialize(void) {

	UINT32	bit;
	UINT	i;
	UINT8	f;

	for (i=0; i<0x10000; i++) {
		f = P_FLAG;
		for (bit=0x80; bit; bit>>=1) {
			if (i & bit) {
				f ^= P_FLAG;
			}
		}
		if (!i) {
			f |= Z_FLAG;
		}
		if (i & 0x8000) {
			f |= S_FLAG;
		}
		szpflag_w[i] = f;
	}

	ia32_init();
}
