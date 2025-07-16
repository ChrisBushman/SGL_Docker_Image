/*   8button : 1101 0001 1111 1111 111L RXYZ SACB RLDU */
#define	NOPAD			0xFFFF

#define	PADUP			0x0001
#define	PADDOWN		0x0002
#define	PADLEFT		0x0004
#define	PADRIGHT		0x0008

#define	PADABUT		0x0040
#define	PADBBUT		0x0010
#define	PADCBUT		0x0020
#define	PADSTART		0x0080

#define	PADZBUT		0x0100
#define	PADYBUT		0x0200
#define	PADXBUT		0x0400
#define	PADRBUT		0x0800
#define	PADLBUT		0x1000


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
