#include	"compiler.h"
#include	"cpucore.h"


	I286CORE	i286acore;

const UINT8 iflags[512] = {					// Z_FLAG, S_FLAG, P_FLAG
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
			0x81, 0x85, 0x85, 0x81, 0x85, 0x81, 0x81, 0x85};


// ----

void i286a_reset(void) {

	ZeroMemory(&CPU_STATSAVE, sizeof(CPU_STATSAVE));
	CPU_CS = 0xf000;
	CS_BASE = 0xf0000;
	CPU_IP = 0xfff0;
	CPU_ADRSMASK = 0xfffff;
}

void i286a_shut(void) {

	CPU_MSW = 0;

	CPU_ES = 0;
	CPU_CS = 0xf000;
	CPU_SS = 0;
	CPU_DS = 0;

	ES_BASE = 0;
	CS_BASE = 0xf0000;
	SS_BASE = 0;
	DS_BASE = 0;
	SS_FIX = 0;
	DS_FIX = 0;

	CPU_IP = 0xfff0;
	CPU_ADRSMASK = 0xfffff;
}


#if 0	// ---- test
void ea_assert(UINT32 x) {

	TCHAR	buf[32];

	wsprintf(buf, _T("addr = %x [%.2x]"), x, i286_memoryread(x - 2));
	MessageBox(NULL, buf, _T("!"), MB_OK);
	exit(1);
}
#endif

