#include	"compiler.h"
#include	"resource.h"
#include	"np2.h"
#include	"np2class.h"
#include	"np2info.h"
#include	"dialog.h"
#include	"dialogs.h"
#include	"pccore.h"


static	SIZE	aboutsize;

static const char str_np2title[] = "Neko Project II  ";
static const char np2infostr[] = 									\
						"CPU: !CPU !CLOCK\r\n"						\
						"MEM: !MEM1\r\n"							\
						"GDC: !GDC\r\n"								\
						"TEXT: !TEXT\r\n"							\
						"GRPH: !GRPH\r\n"							\
						"SOUND: !EXSND\r\n"							\
						"\r\n"										\
						"BIOS: !BIOS\r\n"							\
						"RHYTHM: !RHYTHM\r\n"						\
						"\r\n"										\
						"SCREEN: !DISP";


static void about_init(HWND hWnd) {

	char	work[128];
	RECT	rect;
	POINT	pt;
	RECT	parent;

	milstr_ncpy(work, str_np2title, sizeof(work));
	milstr_ncat(work, np2version, sizeof(work));
	SetDlgItemText(hWnd, IDC_NP2VER, work);
	GetWindowRect(hWnd, &rect);
	aboutsize.cx = rect.right - rect.left;
	aboutsize.cy = rect.bottom - rect.top;
	pt.x = 0;
	pt.y = 0;
	ClientToScreen(GetParent(hWnd), &pt);
	GetClientRect(GetParent(hWnd), &parent);
	np2class_move(hWnd,
					pt.x + ((parent.right - parent.left - aboutsize.cx) / 2),
					pt.y + ((parent.bottom - parent.top - aboutsize.cy) / 2),
					aboutsize.cx, 84);
	SetFocus(GetDlgItem(hWnd, IDOK));
}

static void about_more(HWND hWnd) {

	char	infostr[1024];
	RECT	rect;

	np2info(infostr, np2infostr, sizeof(infostr));
	SetDlgItemText(hWnd, IDC_NP2INFO, infostr);
	EnableWindow(GetDlgItem(hWnd, IDC_MORE), FALSE);
	GetWindowRect(hWnd, &rect);
	np2class_move(hWnd, rect.left, rect.top, aboutsize.cx, aboutsize.cy);
	SetFocus(GetDlgItem(hWnd, IDOK));
}

LRESULT CALLBACK AboutDialogProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {

	switch (msg) {
		case WM_INITDIALOG:
			about_init(hWnd);
			return(FALSE);

		case WM_COMMAND:
			switch (LOWORD(wp)) {
				case IDOK:
					EndDialog(hWnd, IDOK);
					break;

				case IDC_MORE:
					about_more(hWnd);
					break;

				default:
					return(FALSE);
			}
			break;

		case WM_CLOSE:
			PostMessage(hWnd, WM_COMMAND, IDOK, 0);
			break;

		default:
			return(FALSE);
	}
	return(TRUE);
}
