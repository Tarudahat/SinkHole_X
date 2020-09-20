
#include <nds.h>
#include <stdio.h>

#include <nf_lib.h>


int main(void)
{
	consoleDemoInit();

	touchPosition touchXY;

	while (1)
	{
	
		swiWaitForVBlank(); //wait until something can be done
		scanKeys();			//get  button input
		touchRead(&touchXY);

		iprintf("\x1b[1;1H %04i , %04i", touchXY.px, touchXY.py);

		int keys = keysDown();

		if (keys & KEY_A)
		{
			//a
		}
	}

	return 0;
}
