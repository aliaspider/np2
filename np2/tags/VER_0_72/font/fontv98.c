#include	"compiler.h"
#include	"dosio.h"
#include	"memory.h"
#include	"font.h"
#include	"fontdata.h"


#define	V98FILESIZE		0x46800

static void v98knjcpy(BYTE *dst, const BYTE *src, int from, int to) {

	int		i, j, k;
const BYTE	*p;
	BYTE	*q;

	for (i=from; i<to; i++) {
		p = src + 0x1800 + (0x60 * 32 * (i - 1));
		q = dst + 0x20000 + (i << 4);
		for (j=0x20; j<0x80; j++) {
			for (k=0; k<16; k++) {
				*(q + 0x800) = *(p+16);
				*q++ = *p++;
			}
			p += 16;
			q += 0x1000 - 16;
		}
	}
}

BYTE fontv98_read(const char *filename, BYTE loading) {

	FILEH	fh;
	BYTE	*v98fnt;

	if (!(loading & FONTLOAD_ALL)) {
		goto frv_err1;
	}

	// �t�@�C�����I�[�v��
	fh = file_open_rb(filename);
	if (fh == FILEH_INVALID) {
		goto frv_err1;
	}

	v98fnt = (BYTE *)_MALLOC(V98FILESIZE, "v98font");
	if (v98fnt == NULL) {
		goto frv_err2;
	}

	// FONT.ROM �̓ǂݍ���
	if (file_read(fh, v98fnt, V98FILESIZE) != V98FILESIZE) {
		goto frv_err3;
	}

	// 8x8 �t�H���g��ǂޕK�v������H
	if (loading & FONT_ANK8) {
		loading &= ~FONT_ANK8;
		CopyMemory(fontrom + 0x82000, v98fnt, 8*256);
	}
	// 8x16 �t�H���g(�`0x7f)��ǂޕK�v������H
	if (loading & FONT_ANK16a) {
		loading &= ~FONT_ANK16a;
		CopyMemory(fontrom + 0x80000, v98fnt + 0x0800, 16*128);
	}
	// 8x16 �t�H���g(0x80�`)��ǂޕK�v������H
	if (loading & FONT_ANK16b) {
		loading &= ~FONT_ANK16b;
		CopyMemory(fontrom + 0x80800, v98fnt + 0x1000, 16*128);
	}

	// ��ꐅ��������ǂޕK�v������H
	if (loading & FONT_KNJ1) {
		loading &= ~FONT_KNJ1;
		v98knjcpy(fontrom, v98fnt, 0x01, 0x30);
	}
	// ��񐅏�������ǂޕK�v������H
	if (loading & FONT_KNJ2) {
		loading &= ~FONT_KNJ2;
		v98knjcpy(fontrom, v98fnt, 0x30, 0x56);
	}
	// �g��������ǂޕK�v������H
	if (loading & FONT_KNJ3) {
		loading &= ~FONT_KNJ3;
		v98knjcpy(fontrom, v98fnt, 0x58, 0x5d);
	}

frv_err3:
	_MFREE(v98fnt);

frv_err2:
	file_close(fh);							// ��n��

frv_err1:
	return(loading);
}
