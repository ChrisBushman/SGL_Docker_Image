/* this is a small test program */

#include "includes/memmap.h"

void lockup(void);

main()
{
	unsigned short *fb = (unsigned short *)FRAMEBUFFER;

	int 	i,j;

	i = 0;

	j = (512 * 240);
	
	do
	{
		*fb++ = i | 0x8000;
		i++;
	}
	while(j--);

	*FBCR = 0x0003;

	lockup();
}

void lockup(void)
{
	for(;;);
}
