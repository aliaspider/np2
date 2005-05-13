
#define	GDCCMD_MAX	32

typedef struct {
	BYTE	para[256];
	UINT16	fifo[GDCCMD_MAX];
	UINT16	cnt;
	UINT8	ptr;
	UINT8	rcv;
	UINT8	snd;
	UINT8	cmd;
	UINT8	paracb;
	UINT8	reserved;
} _GDCDATA, *GDCDATA;

typedef struct {
	_GDCDATA	m;
	_GDCDATA	s;
	UINT8		mode1;
	UINT8		mode2;
	UINT8		vsync;
	UINT8		vsyncint;
	int			analog;
	int			palnum;
	UINT8		degpal[4];
	RGB32		anapal[16];
	UINT8		clock;
	UINT8		display;
	UINT8		bitac;
	UINT8		m_drawing;
	UINT8		s_drawing;
	UINT8		reserved[3];
} _GDC, *GDC;

typedef struct {
	UINT8	access;
	UINT8	disp;
	UINT8	textdisp;
	UINT8	msw_accessable;
	UINT8	grphdisp;
	UINT8	palchange;
	UINT8	mode2;
} _GDCS, *GDCS;

enum {
	GDC_DEGBBIT			= 0x01,
	GDC_DEGRBIT			= 0x02,
	GDC_DEGGBIT			= 0x04,

	GDCSCRN_ENABLE		= 0x80,
	GDCSCRN_EXT			= 0x40,
	GDCSCRN_ATR			= 0x10,
	GDCSCRN_ALLDRAW		= 0x04,
	GDCSCRN_REDRAW		= 0x01,
	GDCSCRN_ALLDRAW2	= 0x0c,
	GDCSCRN_REDRAW2		= 0x03,
	GDCSCRN_MAKE		= (GDCSCRN_ALLDRAW | GDCSCRN_REDRAW),

	GDCWORK_MASTER		= 0,
	GDCWORK_SLAVE		= 1
};


#ifdef __cplusplus
extern "C" {
#endif

void gdc_reset(void);
void gdc_bind(void);

void gdc_work(int id);
void gdc_forceready(GDCDATA item);
void gdc_paletteinit(void);

void gdc_setdegitalpal(int color, REG8 value);
void gdc_setanalogpal(int color, int rgb, REG8 value);
void gdc_setdegpalpack(int color, REG8 value);

void gdc_restorekacmode(void);

#ifdef __cplusplus
}
#endif
