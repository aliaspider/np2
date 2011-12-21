
typedef struct {
	char	titles[256];
	
	int		winx;
	int		winy;
	BYTE	NOWAIT;
	BYTE	DRAW_SKIP;
	BYTE	DISPCLK;
	BYTE	F11KEY;
	BYTE	F12KEY;
    
	BYTE	MOUSE_SW;
	BYTE	JOYPAD1;
    
	BYTE	comfirm;
	BYTE	resume;
    
	BYTE	toolwin;
	BYTE	jastsnd;
	BYTE	I286SAVE;
} NP2OSCFG;


enum {
	SCREEN_WBASE		= 80,
	SCREEN_HBASE		= 50,
	SCREEN_DEFMUL		= 8,
	FULLSCREEN_WIDTH	= 640,
	FULLSCREEN_HEIGHT	= 480
};


extern	WindowPtr	hWndMain;
extern	BOOL		np2running;
extern	NP2OSCFG	np2oscfg;
extern	BYTE		scrnmode;

void recieveCommand(long param);