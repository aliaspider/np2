#include	"compiler.h"
#include	<ddraw.h>
#include	<winnls32.h>
#include	"resource.h"
#include	"np2.h"
#include	"mousemng.h"
#include	"scrnmng.h"
#include	"sysmng.h"
#include	"pccore.h"
#include	"scrndraw.h"
#include	"palettes.h"
#include	"dclock.h"
#include	"menu.h"


typedef struct {
	LPDIRECTDRAW		ddraw1;
	LPDIRECTDRAW2		ddraw2;
	LPDIRECTDRAWSURFACE	primsurf;
	LPDIRECTDRAWSURFACE	backsurf;
	LPDIRECTDRAWSURFACE	clocksurf;
	LPDIRECTDRAWCLIPPER	clipper;
	LPDIRECTDRAWPALETTE	palette;
	BYTE				scrnmode;
	int					width;
	int					height;
	int					extend;
	int					cliping;
	RGB32				pal16mask;
	BYTE				r16b;
	BYTE				l16r;
	BYTE				l16g;
	RECT				scrn;
	RECT				rect;
	PALETTEENTRY		pal[256];
} DDRAW;

typedef struct {
	int		width;
	int		height;
	int		extend;
	int		multiple;
} SCRNSTAT;

static	DDRAW		ddraw;
		SCRNMNG		scrnmng;
static	SCRNSTAT	scrnstat;
static	SCRNSURF	scrnsurf;


static void setwindowsize(int width, int height) {

	RECT	rectwindow;
	RECT	rectclient;
	RECT	workrc;
	int		scx, scy;
	UINT	update;

	GetWindowRect(hWndMain, &rectwindow);
	GetClientRect(hWndMain, &rectclient);
	width += np2oscfg.paddingx * 2;
	width += rectwindow.right - rectwindow.left;
	width -= rectclient.right - rectclient.left;
	height += np2oscfg.paddingy * 2;
	height += rectwindow.bottom - rectwindow.top;
	height -= rectclient.bottom - rectclient.top;

	SystemParametersInfo(SPI_GETWORKAREA, 0, &workrc, 0);
	scx = GetSystemMetrics(SM_CXSCREEN);
	scy = GetSystemMetrics(SM_CYSCREEN);

	update = 0;
	if (scx < width) {
		np2oscfg.winx = (scx - width) / 2;
		update |= SYS_UPDATEOSCFG;
	}
	else {
		if ((np2oscfg.winx + width) > workrc.right) {
			np2oscfg.winx = workrc.right - width;
			update |= SYS_UPDATEOSCFG;
		}
		if (np2oscfg.winx < workrc.left) {
			np2oscfg.winx = workrc.left;
			update |= SYS_UPDATEOSCFG;
		}
	}
	if (scy < height) {
		np2oscfg.winy = (scy - height) / 2;
		update |= SYS_UPDATEOSCFG;
	}
	else {
		if ((np2oscfg.winy + height) > workrc.bottom) {
			np2oscfg.winy = workrc.bottom - height;
			update |= SYS_UPDATEOSCFG;
		}
		if (np2oscfg.winy < workrc.top) {
			np2oscfg.winy = workrc.top;
			update |= SYS_UPDATEOSCFG;
		}
	}
	sysmng_update(update);
	MoveWindow(hWndMain, np2oscfg.winx, np2oscfg.winy, width, height, TRUE);
}

static void renewalclientsize(void) {

	int		width;
	int		height;
	int		extend;
	int		multiple;
	int		scrnwidth;
	int		scrnheight;

	width = min(scrnstat.width, ddraw.width);
	height = min(scrnstat.height, ddraw.height);
	extend = 0;

	// �`��͈́`
	if (ddraw.scrnmode & SCRNMODE_FULLSCREEN) {
		ddraw.rect.right = width;
		ddraw.rect.bottom = height;
		ddraw.scrn.left = (ddraw.width - width) / 2;
		ddraw.scrn.top = (ddraw.height - height) / 2;
		ddraw.scrn.right = ddraw.scrn.left + width;
		ddraw.scrn.bottom = ddraw.scrn.top + height;
	}
	else {
		multiple = scrnstat.multiple;
		if (!(ddraw.scrnmode & SCRNMODE_ROTATE)) {
			if ((np2oscfg.paddingx) && (multiple == 8)) {
				extend = min(scrnstat.extend, ddraw.extend);
			}
			scrnwidth = (width * multiple) >> 3;
			scrnheight = (height * multiple) >> 3;
			ddraw.rect.right = width + extend;
			ddraw.rect.bottom = height;
			ddraw.scrn.left = np2oscfg.paddingx - extend;
			ddraw.scrn.top = np2oscfg.paddingy;
		}
		else {
			if ((np2oscfg.paddingy) && (multiple == 8)) {
				extend = min(scrnstat.extend, ddraw.extend);
			}
			scrnwidth = (height * multiple) >> 3;
			scrnheight = (width * multiple) >> 3;
			ddraw.rect.right = height;
			ddraw.rect.bottom = width + extend;
			ddraw.scrn.left = np2oscfg.paddingx;
			ddraw.scrn.top = np2oscfg.paddingy - extend;
		}
		ddraw.scrn.right = np2oscfg.paddingx + scrnwidth;
		ddraw.scrn.bottom = np2oscfg.paddingy + scrnheight;
		setwindowsize(scrnwidth, scrnheight);
		setwindowsize(scrnwidth, scrnheight);
	}
	scrnsurf.width = width;
	scrnsurf.height = height;
	scrnsurf.extend = extend;
}

static void clearoutofrect(const RECT *target, const RECT *base) {

	LPDIRECTDRAWSURFACE	primsurf;
	DDBLTFX				ddbf;
	RECT				rect;

	primsurf = ddraw.primsurf;
	if (primsurf == NULL) {
		return;
	}
	ZeroMemory(&ddbf, sizeof(ddbf));
	ddbf.dwSize = sizeof(ddbf);
	ddbf.dwFillColor = 0;

	rect.left = base->left;
	rect.right = base->right;
	rect.top = base->top;
	rect.bottom = target->top;
	if (rect.top < rect.bottom) {
		primsurf->Blt(&rect, NULL, NULL, DDBLT_COLORFILL, &ddbf);
	}
	rect.top = target->bottom;
	rect.bottom = base->bottom;
	if (rect.top < rect.bottom) {
		primsurf->Blt(&rect, NULL, NULL, DDBLT_COLORFILL, &ddbf);
	}

	rect.top = max(base->top, target->top);
	rect.bottom = min(base->bottom, target->bottom);
	if (rect.top < rect.bottom) {
		rect.left = base->left;
		rect.right = target->left;
		if (rect.left < rect.right) {
			primsurf->Blt(&rect, NULL, NULL, DDBLT_COLORFILL, &ddbf);
		}
		rect.left = target->right;
		rect.right = base->right;
		if (rect.left < rect.right) {
			primsurf->Blt(&rect, NULL, NULL, DDBLT_COLORFILL, &ddbf);
		}
	}
}

static void clearoutscreen(void) {

	RECT	base;
	RECT	target;

	GetClientRect(hWndMain, &base);
	target.left = base.left + ddraw.scrn.left;
	target.top = base.top + ddraw.scrn.top;
	target.right = base.left + ddraw.scrn.right;
	target.bottom = base.top + ddraw.scrn.bottom;
	clearoutofrect(&target, &base);
}

static void clearoutfullscreen(void) {

	clearoutofrect(&ddraw.scrn, &ddraw.rect);
	dclock_redraw();
}

static void paletteinit(void) {

	HDC 	hdc;
	UINT	i;

	hdc = GetDC(hWndMain);
	GetSystemPaletteEntries(hdc, 0, 256, ddraw.pal);
	ReleaseDC(hWndMain, hdc);
	for (i=0; i<4; i++) {
		ddraw.pal[i+START_PALORG].peBlue = dclock_pal[i].p.b;
		ddraw.pal[i+START_PALORG].peRed = dclock_pal[i].p.r;
		ddraw.pal[i+START_PALORG].peGreen = dclock_pal[i].p.g;
		ddraw.pal[i+START_PALORG].peFlags = PC_RESERVED | PC_NOCOLLAPSE;
	}
	for (i=0; i<NP2PAL_TOTAL; i++) {
		ddraw.pal[i+START_PAL].peFlags = PC_RESERVED | PC_NOCOLLAPSE;
	}
	ddraw.ddraw2->CreatePalette(DDPCAPS_8BIT, ddraw.pal, &ddraw.palette, 0);
	ddraw.primsurf->SetPalette(ddraw.palette);
}

static void paletteset(void) {

	UINT	i;

	if (ddraw.palette != NULL) {
		for (i=0; i<NP2PAL_TOTAL; i++) {
			ddraw.pal[i+START_PAL].peRed = np2_pal32[i].p.r;
			ddraw.pal[i+START_PAL].peBlue = np2_pal32[i].p.b;
			ddraw.pal[i+START_PAL].peGreen = np2_pal32[i].p.g;
		}
		ddraw.palette->SetEntries(0, START_PAL, NP2PAL_TOTAL,
													&ddraw.pal[START_PAL]);
	}
}

static void make16mask(DWORD bmask, DWORD rmask, DWORD gmask) {

	BYTE	sft;

	sft = 0;
	while((!(bmask & 0x80)) && (sft < 32)) {
		bmask <<= 1;
		sft++;
	}
	ddraw.pal16mask.p.b = (BYTE)bmask;
	ddraw.r16b = sft;

	sft = 0;
	while((rmask & 0xffffff00) && (sft < 32)) {
		rmask >>= 1;
		sft++;
	}
	ddraw.pal16mask.p.r = (BYTE)rmask;
	ddraw.l16r = sft;

	sft = 0;
	while((gmask & 0xffffff00) && (sft < 32)) {
		gmask >>= 1;
		sft++;
	}
	ddraw.pal16mask.p.g = (BYTE)gmask;
	ddraw.l16g = sft;
}


// ----

void scrnmng_initialize(void) {

	scrnstat.width = 640;
	scrnstat.height = 400;
	scrnstat.extend = 1;
	scrnstat.multiple = 8;
	setwindowsize(640, 400);
}

BOOL scrnmng_create(BYTE scrnmode) {

	DWORD			winstyle;
	DWORD			winstyleex;
	HMENU			hmenu;
	LPDIRECTDRAW2	ddraw2;
	DDSURFACEDESC	ddsd;
	DDPIXELFORMAT	ddpf;
	UINT			bitcolor;
	int				height;

	ZeroMemory(&scrnmng, sizeof(scrnmng));
	winstyle = GetWindowLong(hWndMain, GWL_STYLE);
	winstyleex = GetWindowLong(hWndMain, GWL_EXSTYLE);
	hmenu = GetMenu(hWndMain);
	if (scrnmode & SCRNMODE_FULLSCREEN) {
		scrnmode &= ~SCRNMODE_ROTATEMASK;
		scrnmng.flag = SCRNFLAG_FULLSCREEN;
		winstyle &= ~(WS_CAPTION | WS_OVERLAPPED | WS_SYSMENU);
		winstyle |= WS_POPUP;
		winstyleex |= WS_EX_TOPMOST;
		CheckMenuItem(hmenu, IDM_WINDOW, MF_UNCHECKED);
		CheckMenuItem(hmenu, IDM_FULLSCREEN, MF_CHECKED);
	}
	else {
		scrnmng.flag = SCRNFLAG_HAVEEXTEND;
		winstyle |= WS_CAPTION | WS_OVERLAPPED | WS_SYSMENU;
		winstyle &= ~WS_POPUP;
		winstyleex &= ~WS_EX_TOPMOST;
		CheckMenuItem(hmenu, IDM_WINDOW, MF_CHECKED);
		CheckMenuItem(hmenu, IDM_FULLSCREEN, MF_UNCHECKED);
	}
	SetWindowLong(hWndMain, GWL_STYLE, winstyle);
	SetWindowLong(hWndMain, GWL_EXSTYLE, winstyleex);

	if (DirectDrawCreate(NULL, &ddraw.ddraw1, NULL) != DD_OK) {
		goto scre_err;
	}
	ddraw.ddraw1->QueryInterface(IID_IDirectDraw2, (void **)&ddraw2);
	ddraw.ddraw2 = ddraw2;

	if (scrnmode & SCRNMODE_FULLSCREEN) {
		dclock_init();
		ddraw2->SetCooperativeLevel(hWndMain,
					DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN | DDSCL_ALLOWREBOOT);
		height = (np2oscfg.force400)?400:480;
		bitcolor = (scrnmode & SCRNMODE_HIGHCOLOR)?16:8;
		if (ddraw2->SetDisplayMode(640, height, bitcolor, 0, 0) != DD_OK) {
			goto scre_err;
		}
		ddraw2->CreateClipper(0, &ddraw.clipper, NULL);
		ddraw.clipper->SetHWnd(0, hWndMain);

		ZeroMemory(&ddsd, sizeof(ddsd));
		ddsd.dwSize = sizeof(ddsd);
		ddsd.dwFlags = DDSD_CAPS;
		ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
		if (ddraw2->CreateSurface(&ddsd, &ddraw.primsurf, NULL) != DD_OK) {
			goto scre_err;
		}
//		fullscrn_clearblank();

		ZeroMemory(&ddpf, sizeof(ddpf));
		ddpf.dwSize = sizeof(DDPIXELFORMAT);
		if (ddraw.primsurf->GetPixelFormat(&ddpf) != DD_OK) {
			goto scre_err;
		}

		ZeroMemory(&ddsd, sizeof(ddsd));
		ddsd.dwSize = sizeof(ddsd);
		ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
		ddsd.dwWidth = 640;
		ddsd.dwHeight = height;
		if (ddraw2->CreateSurface(&ddsd, &ddraw.backsurf, NULL) != DD_OK) {
			goto scre_err;
		}
		if (bitcolor == 8) {
			paletteinit();
			dclock_init8();
		}
		else {
			make16mask(ddpf.dwBBitMask, ddpf.dwRBitMask,
														ddpf.dwGBitMask);
			dclock_init16();
		}

		ZeroMemory(&ddsd, sizeof(ddsd));
		ddsd.dwSize = sizeof(ddsd);
		ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
		ddsd.dwWidth = DCLOCK_X;
		ddsd.dwHeight = DCLOCK_Y;
		ddraw2->CreateSurface(&ddsd, &ddraw.clocksurf, NULL);
		dclock_reset();
	}
	else {
		ddraw2->SetCooperativeLevel(hWndMain, DDSCL_NORMAL);

		ZeroMemory(&ddsd, sizeof(ddsd));
		ddsd.dwSize = sizeof(ddsd);
		ddsd.dwFlags = DDSD_CAPS;
		ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
		if (ddraw2->CreateSurface(&ddsd, &ddraw.primsurf, NULL) != DD_OK) {
			goto scre_err;
		}

		ddraw2->CreateClipper(0, &ddraw.clipper, NULL);
		ddraw.clipper->SetHWnd(0, hWndMain);
		ddraw.primsurf->SetClipper(ddraw.clipper);

		ZeroMemory(&ddpf, sizeof(ddpf));
		ddpf.dwSize = sizeof(DDPIXELFORMAT);
		if (ddraw.primsurf->GetPixelFormat(&ddpf) != DD_OK) {
			goto scre_err;
		}

		ZeroMemory(&ddsd, sizeof(ddsd));
		ddsd.dwSize = sizeof(ddsd);
		ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
		if (!(scrnmode & SCRNMODE_ROTATE)) {
			ddsd.dwWidth = 641;
			ddsd.dwHeight = 480;
		}
		else {
			ddsd.dwWidth = 480;
			ddsd.dwHeight = 641;
		}
		height = 480;

		if (ddraw2->CreateSurface(&ddsd, &ddraw.backsurf, NULL) != DD_OK) {
			goto scre_err;
		}
		bitcolor = ddpf.dwRGBBitCount;
		if (bitcolor == 8) {
			paletteinit();
		}
		else if (bitcolor == 16) {
			make16mask(ddpf.dwBBitMask, ddpf.dwRBitMask, ddpf.dwGBitMask);
		}
		else if (bitcolor == 24) {
		}
		else if (bitcolor == 32) {
		}
		else {
			goto scre_err;
		}
		ddraw.extend = 1;
	}
	scrnmng.bpp = (BYTE)bitcolor;
	scrnsurf.bpp = bitcolor;
	ddraw.scrnmode = scrnmode;
	ddraw.width = 640;
	ddraw.height = height;
	ddraw.cliping = 0;
	renewalclientsize();
	return(SUCCESS);

scre_err:
	scrnmng_destroy();
	return(FAILURE);
}

void scrnmng_destroy(void) {

	if (ddraw.clocksurf) {
		ddraw.clocksurf->Release();
	}
	if (ddraw.backsurf) {
		ddraw.backsurf->Release();
	}
	if (ddraw.palette) {
		ddraw.palette->Release();
	}
	if (ddraw.clipper) {
		ddraw.clipper->Release();
	}
	if (ddraw.primsurf) {
		ddraw.primsurf->Release();
	}
	if (ddraw.ddraw2) {
		if (ddraw.scrnmode & SCRNMODE_FULLSCREEN) {
			ddraw.ddraw2->SetCooperativeLevel(hWndMain, DDSCL_NORMAL);
		}
		ddraw.ddraw2->Release();
	}
	if (ddraw.ddraw1) {
		ddraw.ddraw1->Release();
	}
	ZeroMemory(&ddraw, sizeof(ddraw));
}

void scrnmng_querypalette(void) {

	if (ddraw.palette) {
		ddraw.primsurf->SetPalette(ddraw.palette);
	}
}

UINT16 scrnmng_makepal16(RGB32 pal32) {

	RGB32	pal;

	pal.d = pal32.d & ddraw.pal16mask.d;
	return((UINT16)((pal.p.g << ddraw.l16g) +
						(pal.p.r << ddraw.l16r) + (pal.p.b >> ddraw.r16b)));
}

void scrnmng_topwinui(void) {

	mouse_running(MOUSE_STOP);
	if (!ddraw.cliping++) {											// ver0.28
		if (scrnmng.flag & SCRNFLAG_FULLSCREEN) {
			ddraw.primsurf->SetClipper(ddraw.clipper);
		}
		WINNLSEnableIME(hWndMain, TRUE);
	}
}

void scrnmng_clearwinui(void) {

	if ((ddraw.cliping > 0) && (!(--ddraw.cliping))) {
		WINNLSEnableIME(hWndMain, FALSE);
		if (scrnmng.flag & SCRNFLAG_FULLSCREEN) {
			ddraw.primsurf->SetClipper(0);
		}
	}
	if (scrnmng.flag & SCRNFLAG_FULLSCREEN) {
		clearoutfullscreen();
	}
	mouse_running(MOUSE_CONT);
}

void scrnmng_setwidth(int posx, int width) {

	scrnstat.width = width;
	renewalclientsize();
}

void scrnmng_setextend(int extend) {

	scrnstat.extend = extend;
	renewalclientsize();
}

void scrnmng_setheight(int posy, int height) {

	scrnstat.height = height;
	renewalclientsize();
}

const SCRNSURF *scrnmng_surflock(void) {

	DDSURFACEDESC	destscrn;

	ZeroMemory(&destscrn, sizeof(destscrn));
	destscrn.dwSize = sizeof(destscrn);
	if (ddraw.backsurf->Lock(NULL, &destscrn, DDLOCK_WAIT, NULL) != DD_OK) {
		return(NULL);
	}
	if (!(ddraw.scrnmode & SCRNMODE_ROTATE)) {
		scrnsurf.ptr = (BYTE *)destscrn.lpSurface;
		scrnsurf.xalign = scrnsurf.bpp >> 3;
		scrnsurf.yalign = destscrn.lPitch;
	}
	else if (!(ddraw.scrnmode & SCRNMODE_ROTATEDIR)) {
		scrnsurf.ptr = (BYTE *)destscrn.lpSurface;
		scrnsurf.ptr += (scrnsurf.width - 1) * destscrn.lPitch;
		scrnsurf.xalign = 0 - destscrn.lPitch;
		scrnsurf.yalign = scrnsurf.bpp >> 3;
	}
	else {
		scrnsurf.ptr = (BYTE *)destscrn.lpSurface;
		scrnsurf.ptr += (scrnsurf.height - 1) * (scrnsurf.bpp >> 3);
		scrnsurf.xalign = destscrn.lPitch;
		scrnsurf.yalign = 0 - (scrnsurf.bpp >> 3);
	}
	return(&scrnsurf);
}

void scrnmng_surfunlock(const SCRNSURF *surf) {

	ddraw.backsurf->Unlock(NULL);
	scrnmng_update();
}

void scrnmng_update(void) {

	POINT	clip;
	RECT	dst;

	if (scrnmng.palchanged) {
		scrnmng.palchanged = FALSE;
		paletteset();
	}
	if (ddraw.backsurf != NULL) {
		if (ddraw.scrnmode & SCRNMODE_FULLSCREEN) {
			if (scrnmng.allflash) {
				scrnmng.allflash = 0;
				clearoutfullscreen();
			}
			if (ddraw.primsurf->Blt(&ddraw.scrn, ddraw.backsurf, &ddraw.rect,
									DDBLT_WAIT, NULL) == DDERR_SURFACELOST) {
				ddraw.primsurf->Restore();
				ddraw.backsurf->Restore();
			}
		}
		else {
			if (scrnmng.allflash) {
				scrnmng.allflash = 0;
				clearoutscreen();
			}
			clip.x = 0;
			clip.y = 0;
			ClientToScreen(hWndMain, &clip);
			dst.left = clip.x + ddraw.scrn.left;
			dst.top = clip.y + ddraw.scrn.top;
			dst.right = clip.x + ddraw.scrn.right;
			dst.bottom = clip.y + ddraw.scrn.bottom;
			if (ddraw.primsurf->Blt(&dst, ddraw.backsurf, &ddraw.rect,
									DDBLT_WAIT, NULL) == DDERR_SURFACELOST) {
				ddraw.primsurf->Restore();
				ddraw.backsurf->Restore();
			}
		}
	}
}


// ----

void scrnmng_setmultiple(int multiple) {

	if (scrnstat.multiple != multiple) {
		scrnstat.multiple = multiple;
		renewalclientsize();
	}
}

static const RECT rectclk = {0, 0, DCLOCK_X, DCLOCK_Y};

void scrnmng_dispclock(void) {

	DDSURFACEDESC	dest;
																// ver0.26
	if ((ddraw.clocksurf) &&
		(ddraw.scrn.top >= DCLOCK_Y) && (dclock_disp())) {
		dclock_make();
		ZeroMemory(&dest, sizeof(dest));
		dest.dwSize = sizeof(dest);
		if (ddraw.clocksurf->Lock(NULL, &dest, DDLOCK_WAIT, NULL) == DD_OK) {
			if (scrnmng.bpp == 8) {
				dclock_out8(dest.lpSurface, dest.lPitch);
			}
			else {
				dclock_out16(dest.lpSurface, dest.lPitch);
			}
			ddraw.clocksurf->Unlock(NULL);
		}
		if (ddraw.primsurf->BltFast(640 - DCLOCK_X - 4,
									ddraw.height - DCLOCK_Y, 
									ddraw.clocksurf, (RECT *)&rectclk,
									DDBLTFAST_WAIT) == DDERR_SURFACELOST) {
			ddraw.primsurf->Restore();
			ddraw.clocksurf->Restore();
		}
		dclock_cntdown(np2oscfg.DRAW_SKIP);
	}
}
