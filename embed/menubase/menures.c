#include	"compiler.h"
#include	"vramhdl.h"
#include	"menubase.h"


#if !defined(RESOURCE_US) && (!defined(CHARSET_OEM) || defined(OSLANG_SJIS))
const char mstr_fontcheck[] = " ";
const char mstr_ok[] = "OK";
			// キャンセル
const char mstr_cancel[] = "\267\254\335\276\331";
			// 中止
const char mstr_abort[] = "\222\206\216\176";
			// 再試行
const char mstr_retry[] = "\215\304\216\216\215\163";
			// 無視
const char mstr_ignore[] = "\226\263\216\213";
			// はい
const char mstr_yes[] = "\202\315\202\242";
			// いいえ
const char mstr_no[] = "\202\242\202\242\202\246";
#elif defined(OSLANG_EUC)
const char mstr_fontcheck[] = " ";
const char mstr_ok[] = "OK";
			// キャンセル
const char mstr_cancel[] = "\216\267\216\254\216\335\216\276\216\331";
			// 中止
const char mstr_abort[] = "\303\346\273\337";
			// 再試行
const char mstr_retry[] = "\272\306\273\356\271\324";
			// 無視
const char mstr_ignore[] = "\314\265\273\353";
			// はい
const char mstr_yes[] = "\244\317\244\244";
			// いいえ
const char mstr_no[] = "\244\244\244\244\244\250";
#elif defined(OSLANG_UTF8) && !defined(RESOURCE_US)
const char mstr_fontcheck[] = " ";
const char mstr_ok[] = "OK";
			// キャンセル
const char mstr_cancel[] = "\357\275\267\357\275\254\357\276\235" \
							"\357\275\276\357\276\231";
			// 中止
const char mstr_abort[] = "\344\270\255\346\255\242";
			// 再試行
const char mstr_retry[] = "\345\206\215\350\251\246\350\241\214";
			// 無視
const char mstr_ignore[] = "\347\204\241\350\246\226";
			// はい
const char mstr_yes[] = "\343\201\257\343\201\204";
			// いいえ
const char mstr_no[] = "\343\201\204\343\201\204\343\201\210";
#else
const char mstr_fontcheck[] = " ";
const char mstr_ok[] = "OK";
const char mstr_cancel[] = "Cancel";
const char mstr_abort[] = "Abort";
const char mstr_retry[] = "Retry";
const char mstr_ignore[] = "Ignore";
const char mstr_yes[] = "Yes";
const char mstr_no[] = "No";
#endif


#ifndef SIZE_QVGA

// ---- minimize, close

static const BYTE minimize1[4] = {		// 12x10
		0x6d,0x66,0x96,0x00};

const MENURES2 menures_minimize = {12, 10, minimize1};


static const BYTE close1[14] = {		// 12x10
		0xe8,0x42,0x52,0x22,0x72,0x84,0x29,0x98,0x74,0x22,0x52,0x42,0x92,
		0x0a};

const MENURES2 menures_close = {12, 10, close1};


// ---- sys

static const BYTE scheck[7] = {			// 8x16
		0xda,0x41,0x11,0x42,0x63,0xb1,0x0c};

static const BYTE snext[9] = {			// 8x16
		0xc9,0x71,0x62,0x53,0x44,0x53,0x62,0xb1,0x03};

const MENURES2 menures_sys[2] = {{8, 16, scheck}, {8, 16, snext}};


// ---- radio

static const BYTE radio[43] = {			// 12x12
		0x08,0x48,0x0a,0x44,0x58,0x44,0x07,0x54,0x85,0x51,0x25,0x00,0x54,
		0x1a,0x23,0x40,0xc5,0x31,0x42,0xc5,0x31,0x42,0xc5,0x31,0x42,0xc5,
		0x31,0x02,0x54,0x1a,0x23,0x00,0x34,0x83,0x31,0x23,0x07,0x22,0x38,
		0x22,0x0a,0x28,0x08};

static const BYTE radiog[43] = {		// 12x12
		0x08,0x48,0x0a,0x44,0x58,0x44,0x07,0x54,0x85,0x56,0x25,0x00,0x54,
		0x6a,0x23,0x40,0xc5,0x36,0x42,0xc5,0x36,0x42,0xc5,0x36,0x42,0xc5,
		0x36,0x02,0x54,0x6a,0x23,0x00,0x34,0x83,0x36,0x23,0x07,0x22,0x38,
		0x22,0x0a,0x28,0x08};

static const BYTE radiohit[7] = {		// 12x12
		0x5b,0x82,0x49,0x88,0x84,0x29,0x5b};

const MENURES2 menures_radio[3] = {{12, 12, radio}, {12, 12, radiog},
							 		{12, 12, radiohit}};


// ---- check

static const BYTE checkhit[11] = {		// 9x9
		0x09,0x71,0x22,0x31,0x23,0x12,0x33,0x55,0x73,0x81,0x0e};

const MENURES2 menures_check = {9, 9, checkhit};


// ---- btn

static const BYTE btnup[7] = {			// 12x12
		0x5b,0x81,0x3a,0x88,0x65,0xb7,0x03};

static const BYTE btndn[7] = {			// 12x12
		0x2b,0x67,0x85,0x38,0xa8,0xb1,0x06};

const MENURES2 menures_scrbtn[2] = {{12, 12, btnup}, {12, 12, btndn}};

#else

// ---- minimize, close

static const BYTE minimize1[3] = {		// 8x8
		0x1b,0x85,0x0a};

const MENURES2 menures_minimize = {8, 8, minimize1};


static const BYTE close1[11] = {		// 8x8
		0x98,0x31,0x41,0x11,0x61,0x61,0x11,0x41,0x31,0x91,0x02};

const MENURES2 menures_close = {8, 8, close1};


// ---- sys

static const BYTE scheck[7] = {			// 8x12
		0x5a,0x41,0x11,0x42,0x63,0xa1,0x04};

static const BYTE snext[7] = {			// 8x12
		0xb9,0x71,0x62,0x53,0x62,0xa1,0x04};

const MENURES2 menures_sys[2] = {{8, 12, scheck}, {8, 12, snext}};


// ---- radio

static const BYTE radio[40] = {			// 11x11
		0x08,0x47,0x0a,0x44,0x57,0x44,0x07,0x54,0x75,0x51,0x25,0x00,0x54,
		0x19,0x23,0x40,0xb5,0x31,0x42,0xb5,0x31,0x42,0xb5,0x31,0x02,0x54,
		0x19,0x23,0x00,0x34,0x73,0x31,0x23,0x07,0x22,0x37,0x22,0x0a,0x27,
		0x08};

static const BYTE radiog[40] = {		// 11x11
		0x08,0x47,0x0a,0x44,0x57,0x44,0x07,0x54,0x75,0x56,0x25,0x00,0x54,
		0x69,0x23,0x40,0xb5,0x36,0x42,0xb5,0x36,0x42,0xb5,0x36,0x02,0x54,
		0x69,0x23,0x00,0x34,0x73,0x36,0x23,0x07,0x22,0x37,0x22,0x0a,0x27,
		0x08};

static const BYTE radiohit[6] = {		// 11x11
		0x1b,0x81,0x39,0x98,0xb1,0x01};

const MENURES2 menures_radio[3] = {{11, 11, radio}, {11, 11, radiog},
									 {11, 11, radiohit}};


// ---- check

static const BYTE checkhit[8] = {		// 7x7
		0xc8,0x51,0x22,0x11,0x32,0x53,0x81,0x0b};

const MENURES2 menures_check = {7, 7, checkhit};


// ---- btn

static const BYTE btnup[5] = {			// 6x6
		0x88,0x41,0x23,0x85,0x0d};


static const BYTE btndn[4] = {			// 6x6
		0x56,0x32,0x14,0xf8};

const MENURES2 menures_scrbtn[2] = {{6, 6, btnup}, {6, 6, btndn}};

#endif


// ---- slider

const BYTE menures_slddat[] = {
		0x28,0x25,0x33,0x54,0x32,0x43,0x25,0x33,0x54,0x32,0x43,0x25,0x33,
		0x54,0x32,0x43,0x05,0x42,0x75,0x50,0x00,

		0x28,0x25,0x33,0x54,0x32,0x46,0x25,0x63,0x54,0x32,0x46,0x25,0x63,
		0x54,0x32,0x46,0x05,0x42,0x75,0x50,0x00,

		0x28,0x25,0x33,0x54,0x32,0x43,0x25,0x33,0x54,0x32,0x43,0x25,0x33,
		0x54,0x32,0x43,0x25,0x47,0x5a,

		0x28,0x25,0x33,0x54,0x32,0x46,0x25,0x63,0x54,0x32,0x46,0x25,0x63,
		0x54,0x32,0x46,0x25,0x47,0x5a,

		0x00,0x75,0x20,0x54,0x20,0x33,0x54,0x32,0x43,0x25,0x33,0x54,0x32,
		0x43,0x25,0x33,0x54,0x72,0xa4,0x05,

		0x00,0x75,0x20,0x54,0x20,0x63,0x54,0x32,0x46,0x25,0x63,0x54,0x32,
		0x46,0x25,0x63,0x54,0x72,0xa4,0x05,

		0x2b,0x00,0xa2,0x23,0x20,0x3a,0x54,0xa2,0x54,0xb0,0x05,0x00,

		0x2b,0x00,0xa2,0x23,0x20,0x93,0x46,0x25,0x4a,0x05,0x5b,0x00,

		0x2c,0x25,0x3a,0x54,0xa2,0x43,0x25,0x4b,0x5e,

		0x2c,0x25,0x3a,0x54,0x32,0x69,0x54,0xb2,0xe4,0x05,

		0x00,0x2a,0x05,0x92,0x43,0x55,0x94,0x43,0x05,0xa5,0x54,0x00,0x5b,

		0x00,0x2a,0x05,0x92,0x43,0x55,0x94,0x46,0x05,0xa5,0x54,0x00,0x5b,

		0x2a,0x25,0x38,0x54,0x82,0x43,0x25,0x38,0x54,0x82,0x43,0x25,0x38,
		0x54,0x82,0x43,0x25,0x38,0x54,0x82,0x43,0x25,0x38,0x54,0x20,0x33,
		0x54,0x07,0x42,0x95,0x50,0x07,

		0x2a,0x25,0x38,0x54,0x32,0x67,0x54,0x32,0x67,0x54,0x32,0x67,0x54,
		0x32,0x67,0x54,0x32,0x67,0x54,0x32,0x67,0x54,0x32,0x67,0x54,0x32,
		0x67,0x54,0x20,0x63,0x54,0x07,0x42,0x95,0x50,0x07,

		0x2a,0x25,0x38,0x54,0x82,0x43,0x25,0x38,0x54,0x82,0x43,0x25,0x38,
		0x54,0x82,0x43,0x25,0x38,0x54,0x82,0x43,0x25,0x38,0x54,0x82,0x43,
		0x25,0x49,0x5c,

		0x2a,0x25,0x38,0x54,0x32,0x67,0x54,0x32,0x67,0x54,0x32,0x67,0x54,
		0x32,0x67,0x54,0x32,0x67,0x54,0x32,0x67,0x54,0x32,0x67,0x54,0x32,
		0x67,0x54,0x32,0x67,0x54,0x92,0xc4,0x05,

		0x07,0x95,0x20,0x54,0x07,0x32,0x43,0x05,0x82,0x43,0x25,0x38,0x54,
		0x82,0x43,0x25,0x38,0x54,0x82,0x43,0x25,0x38,0x54,0x82,0x43,0x25,
		0x38,0x54,0x92,0xc4,0x05,

		0x07,0x95,0x20,0x54,0x07,0x32,0x46,0x05,0x32,0x67,0x54,0x32,0x67,
		0x54,0x32,0x67,0x54,0x32,0x67,0x54,0x32,0x67,0x54,0x32,0x67,0x54,
		0x32,0x67,0x54,0x32,0x67,0x54,0x92,0xc4,0x05,

		0x2e,0x07,0xd2,0x23,0x00,0xe2,0x23,0x20,0x3e,0x54,0xd2,0x43,0x05,
		0xd2,0x54,0x00,0x5e,0x07,

		0x2e,0x07,0xd2,0x23,0x00,0x32,0x6c,0x23,0x20,0xd3,0x46,0x25,0xc3,
		0x46,0x05,0xd2,0x54,0x00,0x5e,0x07,

		0x2f,0x52,0xe2,0x43,0x25,0x3e,0x54,0xe2,0x43,0x25,0x3e,0x54,0xf2,
		0xf4,0x75,0x05,

		0x2f,0x52,0xe2,0x43,0x25,0xd3,0x46,0x25,0xd3,0x46,0x25,0xd3,0x46,
		0x25,0x4f,0x5f,0x57,

		0x07,0x2d,0x05,0x20,0x3c,0x54,0x20,0x3d,0x54,0x45,0x3d,0x54,0x50,
		0xc4,0x43,0x05,0x50,0x4d,0x75,0xe0,0x05,

		0x07,0x2d,0x05,0x20,0x3c,0x54,0x20,0xc3,0x46,0x55,0xd4,0x46,0x05,
		0x45,0x6c,0x54,0x00,0xd5,0x54,0x07,0x5e,

		0x2e,0x25,0x3c,0x54,0xc2,0x43,0x25,0x3c,0x54,0xc2,0x43,0x25,0x3c,
		0x54,0xc2,0x43,0x25,0x3c,0x54,0xc2,0x43,0x25,0x3c,0x54,0xc2,0x43,
		0x25,0x3c,0x54,0xc2,0x43,0x25,0x3c,0x54,0xc2,0x43,0x25,0x3c,0x54,
		0x20,0x3a,0x54,0x07,0x82,0x43,0x95,0x20,0x33,0x54,0x0b,0x42,0xd5,
		0x50,0x09,

		0x2e,0x25,0x3c,0x54,0x32,0x6b,0x54,0x32,0x6b,0x54,0x32,0x6b,0x54,
		0x32,0x6b,0x54,0x32,0x6b,0x54,0x32,0x6b,0x54,0x32,0x6b,0x54,0x32,
		0x6b,0x54,0x32,0x6b,0x54,0x32,0x6b,0x54,0x32,0x6b,0x54,0x32,0x6b,
		0x54,0x32,0x6b,0x54,0x32,0x6b,0x54,0x20,0x93,0x46,0x75,0x20,0x73,
		0x46,0x95,0x20,0x63,0x54,0x0b,0x42,0xd5,0x50,0x09,

		0x2e,0x25,0x3c,0x54,0xc2,0x43,0x25,0x3c,0x54,0xc2,0x43,0x25,0x3c,
		0x54,0xc2,0x43,0x25,0x3c,0x54,0xc2,0x43,0x25,0x3c,0x54,0xc2,0x43,
		0x25,0x3c,0x54,0xc2,0x43,0x25,0x3c,0x54,0xc2,0x43,0x25,0x3c,0x54,
		0xc2,0x43,0x25,0x3c,0x54,0xc2,0x43,0x25,0x4d,0x5f,0x05,

		0x2e,0x25,0x3c,0x54,0x32,0x6b,0x54,0x32,0x6b,0x54,0x32,0x6b,0x54,
		0x32,0x6b,0x54,0x32,0x6b,0x54,0x32,0x6b,0x54,0x32,0x6b,0x54,0x32,
		0x6b,0x54,0x32,0x6b,0x54,0x32,0x6b,0x54,0x32,0x6b,0x54,0x32,0x6b,
		0x54,0x32,0x6b,0x54,0x32,0x6b,0x54,0x32,0x6b,0x54,0x32,0x6b,0x54,
		0x32,0x6b,0x54,0xd2,0xf4,0x55,

		0x09,0xd5,0x20,0x54,0x0b,0x32,0x43,0x95,0x20,0x38,0x54,0x07,0xa2,
		0x43,0x05,0xc2,0x43,0x25,0x3c,0x54,0xc2,0x43,0x25,0x3c,0x54,0xc2,
		0x43,0x25,0x3c,0x54,0xc2,0x43,0x25,0x3c,0x54,0xc2,0x43,0x25,0x3c,
		0x54,0xc2,0x43,0x25,0x3c,0x54,0xc2,0x43,0x25,0x3c,0x54,0xd2,0xf4,
		0x55,

		0x09,0xd5,0x20,0x54,0x0b,0x32,0x46,0x95,0x20,0x73,0x46,0x75,0x20,
		0x93,0x46,0x05,0x32,0x6b,0x54,0x32,0x6b,0x54,0x32,0x6b,0x54,0x32,
		0x6b,0x54,0x32,0x6b,0x54,0x32,0x6b,0x54,0x32,0x6b,0x54,0x32,0x6b,
		0x54,0x32,0x6b,0x54,0x32,0x6b,0x54,0x32,0x6b,0x54,0x32,0x6b,0x54,
		0x32,0x6b,0x54,0x32,0x6b,0x54,0xd2,0xf4,0x55,

		0x2f,0x29,0x09,0xf2,0x83,0x23,0x08,0xf2,0x93,0x23,0x07,0xf2,0xa3,
		0x23,0x00,0xf2,0xb3,0x23,0x20,0x3f,0x3b,0x54,0xf2,0xa3,0x43,0x05,
		0xf2,0x93,0x43,0x05,0x20,0x3f,0x38,0x54,0x07,0xf2,0x84,0x54,0x08,
		0x5f,0x59,0x09,

		0x2f,0x29,0x09,0xf2,0x83,0x23,0x08,0x32,0x6f,0x67,0x23,0x07,0x32,
		0x6f,0x68,0x23,0x00,0x32,0x6f,0x69,0x23,0x20,0xf3,0xa6,0x46,0x25,
		0xf3,0x96,0x46,0x05,0x32,0x6f,0x68,0x54,0x00,0x32,0x6f,0x67,0x54,
		0x07,0xf2,0x84,0x54,0x08,0x5f,0x59,0x09,

		0x2f,0x2d,0x25,0x3f,0x3b,0x54,0xf2,0xb3,0x43,0x25,0x3f,0x3b,0x54,
		0xf2,0xb3,0x43,0x25,0x3f,0x3b,0x54,0xf2,0xb3,0x43,0x25,0x3f,0x3b,
		0x54,0xf2,0xb3,0x43,0x25,0x4f,0x4c,0x5f,0x5f,

		0x2f,0x2d,0x25,0x3f,0x3b,0x54,0x32,0x6f,0x6a,0x54,0x32,0x6f,0x6a,
		0x54,0x32,0x6f,0x6a,0x54,0x32,0x6f,0x6a,0x54,0x32,0x6f,0x6a,0x54,
		0x32,0x6f,0x6a,0x54,0x32,0x6f,0x6a,0x54,0xf2,0xc4,0xf4,0xf5,0x05,

		0x09,0x2f,0x28,0x85,0x20,0x3f,0x37,0x54,0x07,0xf2,0x83,0x43,0x05,
		0x20,0x3f,0x39,0x54,0x20,0x3f,0x3a,0x54,0x45,0x3f,0x3a,0x54,0x50,
		0xf4,0x93,0x43,0x05,0x50,0xf4,0x83,0x43,0x75,0x50,0xf4,0x73,0x43,
		0x85,0x50,0x4f,0x48,0x95,0xf0,0x95,0x05,

		0x09,0x2f,0x28,0x85,0x20,0x3f,0x37,0x54,0x07,0x32,0x6f,0x67,0x54,
		0x00,0x32,0x6f,0x68,0x54,0x20,0xf3,0x96,0x46,0x55,0xf4,0xa6,0x46,
		0x05,0x45,0x6f,0x69,0x54,0x00,0x45,0x6f,0x68,0x54,0x07,0x45,0x6f,
		0x67,0x54,0x08,0xf5,0x84,0x54,0x09,0x5f,0x59};

const UINT16 menures_sldpos[3][12] = {
		{  0,  21,  42,  61,  80, 100, 120, 132, 144, 153, 163, 176},
		{189, 221, 257, 286, 320, 351, 386, 404, 424, 440, 457, 478},
		{499, 553, 615, 665, 723, 776, 837, 879, 926, 961,1000,1047}};

