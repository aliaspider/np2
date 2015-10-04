/**
 * @file	dd2.cpp
 * @brief	DirectDraw 2 surface
 *
 * @author	$Author: yui $
 * @date	$Date: 2011/03/07 09:54:11 $
 */

#include "compiler.h"
#include <ddraw.h>
#include "dd2.h"

#if !defined(__GNUC__)
#pragma comment(lib, "ddraw.lib")
#pragma comment(lib, "dxguid.lib")
#endif	// !defined(__GNUC__)

/**
 * コンストラクタ
 */
DD2Surface::DD2Surface()
{
	ZeroMemory(this, sizeof(*this));
}

/**
 * デストラクタ
 */
DD2Surface::~DD2Surface()
{
	Release();
}

/**
 * 作成
 */
bool DD2Surface::Create(HWND hwnd, int width, int height)
{
	DD2Surface* dd2 = this;
	ZeroMemory(dd2, sizeof(*dd2));
	dd2->hwnd = hwnd;
	if (DirectDrawCreate(NULL, &dd2->ddraw1, NULL) != DD_OK)
	{
		goto dd2cre_err2;
	}
	dd2->ddraw1->QueryInterface(IID_IDirectDraw2, (void **)&dd2->ddraw);
	dd2->ddraw->SetCooperativeLevel(hwnd, DDSCL_NORMAL);

	DDSURFACEDESC ddsd;
	ZeroMemory(&ddsd, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_CAPS;
	ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
	if (dd2->ddraw->CreateSurface(&ddsd, &dd2->primsurf, NULL) != DD_OK)
	{
		goto dd2cre_err2;
	}
	dd2->ddraw->CreateClipper(0, &dd2->clipper, NULL);
	dd2->clipper->SetHWnd(0, hwnd);
	dd2->primsurf->SetClipper(dd2->clipper);
	ZeroMemory(&dd2->ddpf, sizeof(DDPIXELFORMAT));
	dd2->ddpf.dwSize = sizeof(DDPIXELFORMAT);
	if (dd2->primsurf->GetPixelFormat(&dd2->ddpf) != DD_OK)
	{
		goto dd2cre_err2;
	}
	ZeroMemory(&ddsd, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
	ddsd.dwWidth = width;
	ddsd.dwHeight = height;
	if (dd2->ddraw->CreateSurface(&ddsd, &dd2->backsurf, NULL) != DD_OK)
	{
		goto dd2cre_err2;
	}
	if (dd2->ddpf.dwRGBBitCount == 8)
	{
		HDC hdc = ::GetDC(hwnd);
		::GetSystemPaletteEntries(hdc, 0, 256, dd2->pal);
		::ReleaseDC(hwnd, hdc);
		dd2->ddraw->CreatePalette(DDPCAPS_8BIT, dd2->pal, &dd2->palette, 0);
		dd2->primsurf->SetPalette(dd2->palette);
	}
	else if (dd2->ddpf.dwRGBBitCount == 16)
	{
		WORD	bit;
		UINT8	cnt;

		dd2->pal16.d = 0;
		for (bit = 1; (bit) && (!(dd2->ddpf.dwBBitMask & bit)); bit<<=1)
		{
		}
		for (dd2->r16b=8; (dd2->r16b) && (dd2->ddpf.dwBBitMask & bit);
													dd2->r16b--, bit<<=1) {
			dd2->pal16.p.b >>= 1;
			dd2->pal16.p.b |= 0x80;
		}
		for (dd2->l16r=0, bit=1; (bit) && (!(dd2->ddpf.dwRBitMask & bit));
													dd2->l16r++, bit<<=1);
		for (cnt=0x80; (cnt) && (dd2->ddpf.dwRBitMask & bit);
													cnt>>=1, bit<<=1) {
			dd2->pal16.p.r |= cnt;
		}
		for (; cnt; cnt>>=1) dd2->l16r--;
		for (dd2->l16g=0, bit=1; (bit) && (!(dd2->ddpf.dwGBitMask & bit));
													dd2->l16g++, bit<<=1);
		for (cnt=0x80; (cnt) && (dd2->ddpf.dwGBitMask & bit);
													cnt>>=1, bit<<=1) {
			dd2->pal16.p.g |= cnt;
		}
		for (; cnt; cnt >>= 1)
		{
			dd2->l16g--;
		}
	}
	else if (dd2->ddpf.dwRGBBitCount == 24)
	{
	}
	else if (dd2->ddpf.dwRGBBitCount == 32)
	{
	}
	else
	{
		goto dd2cre_err2;
	}
	dd2->cliping = 0;
	dd2->vram.width = width;
	dd2->vram.height = height;
	dd2->vram.xalign = dd2->ddpf.dwRGBBitCount / 8;
	dd2->vram.bpp = dd2->ddpf.dwRGBBitCount;
	return true;

dd2cre_err2:
	Release();
	return false;
}

/**
 * 解放
 */
void DD2Surface::Release()
{
	DD2Surface* dd2 = this;
	RELEASE(dd2->palette);
	RELEASE(dd2->clipper);
	RELEASE(dd2->backsurf);
	RELEASE(dd2->primsurf);
	RELEASE(dd2->ddraw);
	RELEASE(dd2->ddraw1);
}

/**
 * 
 */
CMNVRAM* DD2Surface::Lock()
{
	DD2Surface* dd2 = this;
	if (dd2->backsurf == NULL)
	{
		return NULL;
	}
	DDSURFACEDESC surface;
	ZeroMemory(&surface, sizeof(DDSURFACEDESC));
	surface.dwSize = sizeof(DDSURFACEDESC);
	HRESULT r = dd2->backsurf->Lock(NULL, &surface, DDLOCK_WAIT, NULL);
	if (r == DDERR_SURFACELOST)
	{
		dd2->backsurf->Restore();
		r = dd2->backsurf->Lock(NULL, &surface, DDLOCK_WAIT, NULL);
	}
	if (r != DD_OK)
	{
		return(NULL);
	}
	dd2->vram.ptr = (UINT8 *)surface.lpSurface;
	dd2->vram.yalign = surface.lPitch;
	return &dd2->vram;
}

void DD2Surface::Unlock()
{
	DD2Surface* dd2 = this;
	if (dd2->backsurf)
	{
		dd2->backsurf->Unlock(NULL);
	}
}

void DD2Surface::Blt(const POINT *pt, const RECT *rect)
{
	DD2Surface* dd2 = this;
	if (dd2->backsurf)
	{
		POINT	clipt;
		if (pt)
		{
			clipt = *pt;
		}
		else
		{
			clipt.x = 0;
			clipt.y = 0;
		}
		ClientToScreen(dd2->hwnd, &clipt);
		RECT scrn;
		scrn.left = clipt.x;
		scrn.top = clipt.y;
		scrn.right = clipt.x + rect->right - rect->left;
		scrn.bottom = clipt.y + rect->bottom - rect->top;
		if (dd2->primsurf->Blt(&scrn, dd2->backsurf, (RECT *)rect, DDBLT_WAIT, NULL) == DDERR_SURFACELOST)
		{
			dd2->backsurf->Restore();
			dd2->primsurf->Restore();
		}
	}
}

UINT16 DD2Surface::GetPalette16(RGB32 pal) const
{
	const DD2Surface* dd2 = this;

	pal.d &= dd2->pal16.d;
	return (((UINT16)pal.p.g) << dd2->l16g) | (((UINT16)pal.p.r) << dd2->l16r) | (pal.p.b >> dd2->r16b);
}



// ----

DD2HDL dd2_create(HWND hwnd, int width, int height)
{
	DD2HDL ret = new DD2Surface;
	ret->Create(hwnd, width, height);
	return ret;
}

void dd2_release(DD2HDL dd2hdl)
{
	if (dd2hdl)
	{
		delete dd2hdl;
	}
}

CMNVRAM *dd2_bsurflock(DD2HDL dd2hdl)
{
	if (dd2hdl)
	{
		return dd2hdl->Lock();
	}
	else
	{
		return NULL;
	}
}

void dd2_bsurfunlock(DD2HDL dd2hdl)
{
	if (dd2hdl)
	{
		dd2hdl->Unlock();
	}
}

void dd2_blt(DD2HDL dd2hdl, const POINT *pt, const RECT *rect)
{
	if (dd2hdl)
	{
		dd2hdl->Blt(pt, rect);
	}
}

UINT16 dd2_get16pal(DD2HDL dd2hdl, RGB32 pal)
{
	if (dd2hdl)
	{
		return dd2hdl->GetPalette16(pal);
	}
	else
	{
		return 0;
	}
}
