
enum {
	NORMAL2608	= 0,
	EXTEND2608	= 1
};


#ifdef __cplusplus
extern "C" {
#endif

#if !defined(SUPPORT_S98)		// コールすら面倒だ！

#define	S98_init()
#define	S98_trash()
#define	S98_open(f)			(FAILURE)
#define	S98_close()
#define	S98_put(m, a, d)
#define S98_sync()

#else

void S98_init(void);
void S98_trash(void);
BOOL S98_open(const char *filename);
void S98_close(void);
void S98_put(REG8 module, REG8 addr, REG8 data);
void S98_sync(void);

#endif

#ifdef __cplusplus
}
#endif

