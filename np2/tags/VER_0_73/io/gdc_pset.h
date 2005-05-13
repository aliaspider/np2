
struct _gdcps;
typedef struct _gdcps	_GDCPSET;
typedef struct _gdcps	*GDCPSET;

typedef void (MEMCALL * GDCPSFN)(GDCPSET pset, UINT addr, UINT bit);

struct _gdcps {
	GDCPSFN	func;
	BYTE	*base;
	UINT16	pattern;
	BYTE	update;
	UINT16	x;
	UINT16	y;
	UINT	dots;
};


#ifdef __cplusplus
extern "C" {
#endif

void MEMCALL gdcpset_prepare(GDCPSET pset, UINT32 csrw, UINT16 pat, BYTE op);
void MEMCALL gdcpset(GDCPSET pset, UINT16 x, UINT16 y);

#ifdef __cplusplus
}
#endif
