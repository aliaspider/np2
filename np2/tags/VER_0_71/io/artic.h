
typedef struct {
	SINT32	lastclk2;
	SINT32	counter;
} _ARTIC, *ARTIC;


#ifdef __cplusplus
extern "C" {
#endif

void artic_callback(void);

void artic_reset(void);
void artic_bind(void);
UINT16 IOINPCALL artic_r16(UINT port);

#ifdef __cplusplus
}
#endif

