#include	"compiler.h"
#include	"dosio.h"
#include	"cpucore.h"
#include	"font.h"
#include	"fontdata.h"


static void x68kknjcpy(BYTE *dst, const BYTE *src, int from, int to) {

	int		i, j, k;
const BYTE	*p;
	BYTE	*q;

	for (i=from; i<to; i++) {
		q = dst + 0x21000 + (i << 4);
		for (j=0x21; j<0x7f; j++) {
			p = NULL;
			// �����̃|�C���^�����߂�
			if ((i >= 0x01) && (i < 0x08)) {			// 2121�`277e
				p = src + 0x00000
					+ ((((i - 0x01) * 0x5e) + (j - 0x21)) * 0x20);
			}
			else if ((i >= 0x10) && (i < 0x30)) {		// 3021�`5f7e
				p = src + 0x05e00
					+ ((((i - 0x10) * 0x5e) + (j - 0x21)) * 0x20);
			}
			else if ((i >= 0x30) && (i < 0x54)) {		// 5021�`737e
				p = src + 0x1d600
					+ ((((i - 0x30) * 0x5e) + (j - 0x21)) * 0x20);
			}
			else if ((i == 0x54) && (j < 0x25)) {		// 7421�`7424
				p = src + 0x1d600
					+ ((((0x54 - 0x30) * 0x5e) + (j - 0x21)) * 0x20);
			}
			if (p) {							// �K�i���R�[�h�Ȃ��
				// �R�s�[����
				for (k=0; k<16; k++) {
					*(q+k) = *p++;
					*(q+k+0x800) = *p++;
				}
			}
			q += 0x1000;
		}
	}
}

BYTE fontx68k_read(const char *filename, BYTE loading) {

	FILEH	fh;
	BYTE	*work;

	// �t�@�C�����I�[�v��
	fh = file_open_rb(filename);
	if (fh == FILEH_INVALID) {
		goto fr68_err1;
	}

	// �������A���P�[�g
	work = (BYTE *)_MALLOC(0x3b800, "x68kfont");
	if (work == NULL) {
		goto fr68_err2;
	}

	// CGROM.DAT �̓ǂݍ���
	if (file_read(fh, work, 0x3b800) != 0x3b800) {
		goto fr68_err3;
	}

	// 8dot ANK��ǂޕK�v�����邩
	if (loading & FONT_ANK8) {
		loading &= ~FONT_ANK8;
		CopyMemory(fontrom + 0x82100, work + 0x3a100, 0x60*8);
		CopyMemory(fontrom + 0x82500, work + 0x3a500, 0x40*8);
	}

	// 16dot ASCII��ǂޕK�v�����邩
	if (loading & FONT_ANK16a) {
		loading &= ~FONT_ANK16a;
		CopyMemory(fontrom + 0x80200, work + 0x3aa00, 0x60*16);
		fontdata_patch16a();
	}

	// 16dot ANK(0x80�`)��ǂޕK�v�����邩
	if (loading & FONT_ANK16b) {
		loading &= ~FONT_ANK16b;
		CopyMemory(fontrom + 0x80a00, work + 0x3b200, 0x40*16);
		fontdata_patch16b();
	}

	// ��ꐅ��������ǂݍ��ށH
	if (loading & FONT_KNJ1) {
		loading &= ~FONT_KNJ1;
		x68kknjcpy(fontrom, work, 0x01, 0x30);
		fontdata_patchjis();
	}

	// ��񐅏���ǂޕK�v�͂���H
	if (loading & FONT_KNJ2) {
		loading &= ~FONT_KNJ2;
		x68kknjcpy(fontrom, work, 0x30, 0x60);
	}

fr68_err3:
	_MFREE(work);

fr68_err2:
	file_close(fh);

fr68_err1:
	return(loading);
}
