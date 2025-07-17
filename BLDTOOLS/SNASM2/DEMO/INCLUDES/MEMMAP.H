
#define	CS0	0x00000000
#define	CS1	0x00020000
#define	CS2	0x04000000
#define	CS3	0x06000000

#define	CS0th	0x20000000
#define	CS1th	0x20020000
#define	CS2th	0x24000000
#define	CS3th	0x26000000

#define	FRAMEBUFFER	0x05C80000
#define	VDP1REGS		0x05D00000

#define	FBCR			(unsigned short *)(VDP1REGS+2)
