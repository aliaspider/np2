#include	"compiler.h"
#include	"strres.h"
#include	"taskmng.h"
#include	"i286.h"
#include	"pccore.h"
#include	"iocore.h"
// #include	"hostdrv.h"
// #include	"hostdir.h"


#define		NP2SYSP_VER			"B"
// #define	NP2SYSP_CREDIT		""					// 要るなら・・・

#define		OPEN_HOSTDRV		0
#define		INTR_HOSTDRV		1
#define		CLOSE_HOSTDRV		2

// NP2依存ポート
// port:07edh	reserved
// port:07efh	np2 string comm

// 基本的に STRINGでやり取りする
// ポート 7efh に 'NP2' と出力で "NP2"が返ってきたら NP2である

// verA
//		out->str: 'ver'				in->str:	ver番号 A〜
//		out->str: 'poweroff'		NP2を終了

// verB
//		out->str: 'cpu'				in->str:	CPU型番
//		out->str: 'clock'			in->str:	動作クロック数


// ----

typedef struct {
const char	*key;
	void	(*func)(const void *arg1, const void *arg2);
const void	*arg1;
const void	*arg2;
} SYSPCMD;

static const char str_80286[] = "80286";
static const char str_v30[] = "V30";
static const char str_pentium[] = "PENTIUM";
static const char str_mhz[] = "%uMHz";


static void np2sysp_outstr(const void *arg1, const void *arg2) {

	milstr_ncpy(np2sysp.outstr, (char *)arg1, sizeof(np2sysp.outstr));
	np2sysp.outpos = 0;
	(void)arg2;
}

static void np2sysp_pwroff(const void *arg1, const void *arg2) {

	taskmng_exit();
	(void)arg1;
	(void)arg2;
}

static void np2sysp_cpu(const void *arg1, const void *arg2) {

	// CPUを返す
#if 1											// 80286 or V30
	if (!(CPUTYPE & CPUTYPE_V30)) {
		np2sysp_outstr(str_80286, NULL);
	}
	else {
		np2sysp_outstr(str_v30, NULL);
	}
#else
	// 386機以降の場合 V30モードはエミュレーションだから固定(?)
	np2sysp_outstr(str_pentium, NULL);
#endif
	(void)arg1;
	(void)arg2;
}

static void np2sysp_clock(const void *arg1, const void *arg2) {

	SPRINTF(np2sysp.outstr, str_mhz, (pc.realclock + 500000) / 1000000);
	np2sysp.outpos = 0;
	(void)arg1;
	(void)arg2;
}

static void np2sysp_mul(const void *arg1, const void *arg2) {

	SPRINTF(np2sysp.outstr, str_u, pc.multiple);
	np2sysp.outpos = 0;
	(void)arg1;
	(void)arg2;
}

#if 0
static void np2sysp_hostdrv(const void *arg1, const void *arg2) {

	switch((DWORD)arg1) {
		case OPEN_HOSTDRV:
			if(open_hostdrv())
				np2sysp_outstr("ok", NULL);
			else
				np2sysp_outstr("ng", NULL);
			break;

		case INTR_HOSTDRV:
			intr_hostdrv();
			break;

		case CLOSE_HOSTDRV:
			close_hostdrv();
			break;
	}
}
#endif


static const char str_np2[] = "NP2";
static const char str_ver[] = "ver";
static const char str_poweroff[] = "poweroff";
static const char str_credit[] = "credit";
static const char str_cpu[] = "credit";
static const char str_clock[] = "clock";
static const char str_multiple[] = "multiple";

#if defined(NP2SYSP_VER)
static const char str_syspver[] = NP2SYSP_VER;
#else
#define	str_syspver		str_null
#endif

#if defined(NP2SYSP_CREDIT)
static const char str_syspcredit[] = NP2SYSP_CREDIT;
#else
#define	str_syspcredit	str_null
#endif


static const SYSPCMD np2spcmd[] = {
			{str_np2,		np2sysp_outstr,	str_np2,		NULL},
			{str_ver,		np2sysp_outstr,	str_syspver,	NULL},

// version:A
			{str_poweroff,	np2sysp_pwroff,	NULL,			NULL},

// version:B
			{str_credit,	np2sysp_outstr,	str_syspcredit,	NULL},
			{str_cpu,		np2sysp_cpu,	NULL,			NULL},
			{str_clock,		np2sysp_clock,	NULL,			NULL},
			{str_multiple,	np2sysp_mul,	NULL,			NULL},

#if 0
// hostdrv
			{"check_hostdrv",	np2sysp_outstr,		"supported",
																NULL},
			{"open_hostdrv",	np2sysp_hostdrv,	(void *)OPEN_HOSTDRV,
																NULL},
			{"intr_hostdrv",	np2sysp_hostdrv,	(void *)INTR_HOSTDRV,
																NULL},
			{"close_hostdrv",	np2sysp_hostdrv,	(void *)CLOSE_HOSTDRV,
																NULL},

			{hostdir_check,		np2sysp_outstr,	hostdir_check,	NULL},
			{"hostdir_reset",	hostdir_reset,	NULL,			NULL},
			{"hostdir_band",	hostdir_band,	NULL,			NULL},
			{"hostdir_int",		hostdir_int,	NULL,			NULL},
#endif
};


static BOOL np2syspcmp(const char *p) {

	int		len;
	int		pos;

	len = strlen(p);
	if (!len) {
		return(TRUE);
	}
	pos = np2sysp.strpos;
	while(len--) {
		if (p[len] != np2sysp.substr[pos]) {
			return(TRUE);
		}
		pos--;
		pos &= NP2SYSP_MASK;
	}
	return(FALSE);
}

static void IOOUTCALL np2sysp_o7ef(UINT port, BYTE dat) {

	UINT	i;

	np2sysp.substr[np2sysp.strpos] = dat;
	for (i=0; i<(sizeof(np2spcmd)/sizeof(SYSPCMD)); i++) {
		if (!np2syspcmp(np2spcmd[i].key)) {
			np2spcmd[i].func(np2spcmd[i].arg1, np2spcmd[i].arg2);
			break;
		}
	}
	np2sysp.strpos++;
	np2sysp.strpos &= NP2SYSP_MASK;
	(void)port;
}

static BYTE IOINPCALL np2sysp_i7ef(UINT port) {

	BYTE	ret;

	ret = np2sysp.outstr[np2sysp.outpos];
	if (ret) {
		np2sysp.outpos++;
		np2sysp.outpos &= NP2SYSP_MASK;
	}
	(void)port;
	return(ret);
}


// ---- I/F

void np2sysp_reset(void) {

	ZeroMemory(&np2sysp, sizeof(np2sysp));
}

void np2sysp_bind(void) {

	iocore_attachout(0x07ef, np2sysp_o7ef);
	iocore_attachinp(0x07ef, np2sysp_i7ef);
}

