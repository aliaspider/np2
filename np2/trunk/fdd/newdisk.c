#include	"compiler.h"
#include	"dosio.h"
#include	"newdisk.h"
#include	"fddfile.h"
#include	"sxsi.h"
#include	"hddboot.res"


// ---- fdd

void newdisk_fdd(const char *fname, BYTE type, const char *label) {

	_D88HEAD	d88head;
	FILEH		fh;

	ZeroMemory(&d88head, sizeof(d88head));
	STOREINTELDWORD(d88head.fd_size, sizeof(d88head));
	milstr_ncpy((char *)d88head.fd_name, label, sizeof(d88head.fd_name));
	d88head.fd_type = type;
	fh = file_create(fname);
	if (fh != FILEH_INVALID) {
		file_write(fh, &d88head, sizeof(d88head));
		file_close(fh);
	}
}


// ---- hdd

static const char vhd_ver[] = "VHD1.00";

static BOOL writezero(FILEH fh, UINT size) {

	BYTE	work[256];
	UINT	wsize;

	ZeroMemory(work, sizeof(work));
	while(size) {
		wsize = min(size, sizeof(work));
		if (file_write(fh, work, wsize) != wsize) {
			return(FAILURE);
		}
		size -= wsize;
	}
	return(SUCCESS);
}

static void writehddipl(FILEH fh, UINT ssize) {

	BYTE	work[1024];

	ZeroMemory(work, sizeof(work));
	CopyMemory(work, hdddiskboot, sizeof(hdddiskboot));
	if (ssize < 1024) {
		work[ssize - 2] = 0x55;
		work[ssize - 1] = 0xaa;
	}
	file_write(fh, work, sizeof(work));
}

void newdisk_thd(const char *fname, UINT hddsize) {

	FILEH	fh;
	BYTE	work[256];
	UINT	size;

	if ((fname == NULL) || (hddsize < 5) || (hddsize > 256)) {
		return;
	}
	fh = file_create(fname);
	if (fh != FILEH_INVALID) {
		ZeroMemory(work, 256);
		size = hddsize * 15;
		STOREINTELWORD(work, size);
		file_write(fh, work, 256);
		writehddipl(fh, 256);
		file_close(fh);
	}
}

// hddtype = 0:5MB / 1:10MB / 2:15MB / 3:20MB / 5:30MB / 6:40MB
void newdisk_hdi(const char *fname, UINT hddtype) {

const SASIHDD	*sasi;
	FILEH		fh;
	HDIHDR		hdi;
	UINT32		tmp;

	hddtype &= 7;
	if ((fname == NULL) || (hddtype == 7)) {
		return;
	}
	sasi = sasihdd + hddtype;
	fh = file_create(fname);
	if (fh != FILEH_INVALID) {
		ZeroMemory(&hdi, sizeof(hdi));
		tmp = 256 * sasi->sectors * sasi->surfaces * sasi->cylinders;
//		STOREINTELDWORD(hdi.hddtype, 0);				// ???
		STOREINTELDWORD(hdi.headersize, 4096);
		STOREINTELDWORD(hdi.hddsize, tmp);
		STOREINTELDWORD(hdi.sectorsize, 256);
		STOREINTELDWORD(hdi.sectors, sasi->sectors);
		STOREINTELDWORD(hdi.surfaces, sasi->surfaces);
		STOREINTELDWORD(hdi.cylinders, sasi->cylinders);
		file_write(fh, &hdi, sizeof(hdi));
		writezero(fh, 4096 - sizeof(hdi));
		writehddipl(fh, 256);
		file_close(fh);
	}
}

void newdisk_vhd(const char *fname, UINT hddsize) {

	FILEH	fh;
	VHDHDR	vhd;
	UINT	tmp;

	if ((fname == NULL) || (hddsize < 2) || (hddsize > 512)) {
		return;
	}
	fh = file_create(fname);
	if (fh != FILEH_INVALID) {
		ZeroMemory(&vhd, sizeof(vhd));
		CopyMemory(&vhd.sig, vhd_ver, 7);
		STOREINTELWORD(vhd.mbsize, (UINT16)hddsize);
		STOREINTELWORD(vhd.sectorsize, 256);
		vhd.sectors = 32;
		vhd.surfaces = 8;
		tmp = hddsize *	16;		// = * 1024 * 1024 / (8 * 32 * 256);
		STOREINTELWORD(vhd.cylinders, (UINT16)tmp);
		tmp *= 8 * 32;
		STOREINTELDWORD(vhd.totals, tmp);
		file_write(fh, &vhd, sizeof(vhd));
		writehddipl(fh, 256);
		file_close(fh);
	}
}

