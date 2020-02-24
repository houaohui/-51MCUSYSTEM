#include <stc12c5a60s2.h>
void KeyDriver()
{
	unsigned char i, j;
	static unsigned char backup[4][4] = {
	{1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}
	};
	 
	for (i=0; i<4; i++)
	{
		for (j=0; j<4; j++)
		{
			if (backup[i][j] != keysta[i][j])
			{
				if (backup[i][j] != 0)
				{
					keyaction(keycodemap[i][j]);
				}
				backup[i][j] = keysta[i][j];
			}
		}
	} 
}
void keyscan()
{
	unsigned char i;
	static unsigned char keyout=0;
	static unsigned char keybuf[4][4]={
	{0xff,0xff,0xff,0xff},{0xff,0xff,0xff,0xff},
	{0xff,0xff,0xff,0xff},{0xff,0xff,0xff,0xff}
	};
	keybuf[keyout][0]=(keybuf[keyout][0]<<1)|P20;
	keybuf[keyout][1]=(keybuf[keyout][1]<<1)|P21;
	keybuf[keyout][2]=(keybuf[keyout][2]<<1)|P22;
	keybuf[keyout][3]=(keybuf[keyout][3]<<1)|P23;
	for(i=0;i<4;i++)
	{
		if((keybuf[keyout][i]&0x0f)==0x00)
		{
			keysta[keyout][i]=0;
		}
		else if((keybuf[keyout][i]&0x0f)==0x0f)
		{
			keysta[keyout][i]=1;
		}
	}
	keyout++;
	keyout=keyout&0x03;
	switch(keyout)
	{
		case 0: P20=1;P23=0;break;
		case 1: P23=1;P22=0;break;
		case 2: P22=1;P21=0;break;
		case 3: P21=1;P20=0;break;
		default:break;
	}
}