#include <stc12c5a60s2.h>
#define ADC_POWER 0x80
#define ADC_FLAG 0x10
#define ADC_START 0x08
#define ADC_SPEEDLL 0x00
sbit ENLED=P1^1;
unsigned char code ledchar[]={
	0xC0,0xF9,0xA4,0xB0,
	0x99,0x92,0x82,0xF8,
	0x80,0x90,0x88,0x83,
	0xC6,0xA1,0x86,0x8E
};
unsigned char code keycodemap[4][4] = {
	{ 1, 2, 3, 'u' },
	{ 4, 5, 6, 'l' },
	{ 7, 8, 9, 'd' },
	{ 0, 'e', 'n', 'r' }
};

unsigned char code str0[]="Wellcome";
unsigned char code str5[]="SwitchMode";
unsigned char code str1[]="1 timer";
unsigned char code str2[]="2 Infrared";
unsigned char code str3[]="3 Temperature";
unsigned char code str4[]="4 UART Disply";
unsigned char code str6[]="STOP  ";
unsigned char code str7[]="RUNING";
unsigned char code str8[]="5 ADC TRANSFORM";
unsigned char code str9[]="6 EEPROM";
unsigned char code str10[]="READ ADDR";
unsigned char code str11[]="WRITEADDR";
unsigned char code str12[]="OK";

unsigned char xdata ledbuff[7]={
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
unsigned char keysta[4][4] = {
 {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}
},i,sec,x=1,y=2,z=0,s;
unsigned char xdata ircode[4],addr=10,m=10;
unsigned int index=0;
unsigned char xdata dat,str[5];//adc

//void keyaction(unsigned char keycode);
void ConfigTimer0();
void keydriver();
void InitLcd1602();
void LcdWriteDat(unsigned char dat);
void LcdShowStr(unsigned char x, unsigned char y, unsigned char *str);
void LcdAreaClean(unsigned char x, unsigned char y, unsigned char len);

//void LcdFullClean();
void ds18b20();
void InitInfrared();
void configuart();
unsigned int GetHighTime();
unsigned int GetLowTime();
void IRreceive();
void timer();
void ADC_init();
void eeprom();
void adcstr();
unsigned char E2ReadByte(unsigned char addr);
void E2WriteByte(unsigned char addr, unsigned char dat);
bit Start18B20();
bit Get18B20Temp(int *temp);
bit en1602,en16022,enui,entimer,ds18flags,irflag,t=0,e2,rw;

//void Delay1ms()		//@33.1776MHz
//{
//	unsigned char i, j;
//	i = 33;
//	j = 66;
//	do
//	{
//		while (--j);
//	} while (--i);
//}

void main()
{
	ENLED=0;
	InitLcd1602();
	ConfigTimer0();
	LcdShowStr(0,1,str0);
	LcdShowStr(0,0,str5);
	EA=1;
	while(1)
	{
		
		switch(i)//用于主界面显示，加入变量en1602和en16022是为了是程序只初始化一次，运行界面只显示一次
		{
			case 1:if(en1602) {LcdAreaClean(0,1,16); LcdShowStr(0,1,str1); en1602=0;}break;
			case 2:if(en1602) {LcdAreaClean(0,1,16); LcdShowStr(0,1,str2); en1602=0;}break;
			case 3:if(en1602) {LcdAreaClean(0,1,16); LcdShowStr(0,1,str3); en1602=0;}break;
			case 4:if(en1602) {LcdAreaClean(0,1,16); LcdShowStr(0,1,str4); en1602=0;}break;
			case 5:if(en1602) {LcdAreaClean(0,1,16); LcdShowStr(0,1,str8); en1602=0;}break;
			case 6:if(en1602) {LcdAreaClean(0,1,16); LcdShowStr(0,1,str9); en1602=0;}break;
		}
		if(enui)
		switch(i)//用于启动不同模块
		{
			case 1:if(en16022) {LcdAreaClean(0,0,16); LcdShowStr(0,0,str1);t=~t;
				LcdAreaClean(0,1,16);if(t)LcdShowStr(0,1,str7);else LcdShowStr(0,1,str6); en16022=0; entimer=1;}
			timer();break;
			case 2:if(en16022) {LcdAreaClean(0,0,16); LcdShowStr(0,0,str2); en16022=0;InitInfrared();}  IRreceive();break;
			case 3:if(en16022) {LcdAreaClean(0,0,16); LcdShowStr(0,0,str3); en16022=0;Start18B20();}    ds18b20();break;
			case 4:if(en16022) {LcdAreaClean(0,0,16); LcdShowStr(0,0,str4); en16022=0;configuart();}  break;
			case 5:if(en16022) {LcdAreaClean(0,0,16); LcdShowStr(0,0,str8); en16022=0;ADC_init();EADC=1;} adcstr();break;
			case 6:if(en16022) {LcdAreaClean(0,0,16); LcdShowStr(0,0,str9); en16022=0;rw=~rw;
			LcdAreaClean(0,1,16);if(rw)LcdShowStr(0,1,str11);else LcdShowStr(0,1,str10); en16022=0;e2=1;addr=10;} eeprom();break;
		}
		keydriver();
	}
	
}
void deinit()//用于清除其它硬件的配置信息，退出时只执行一次
{
	unsigned char k;
	entimer=0;
	
	TR1 = 0;ET1 = 0;EX0 = 0;
	ES=0;TR0=1;
	for(k=0;k<6;k++)
	{
		ledbuff[k]=0xff;
	}
	EADC=0;
	t=0;//定时器暂停
	e2=0;
}

void adcstr()
{
	if(ds18flags)
	{
		index=0;
		ds18flags=0;
		str[1]='.';
		str[4]='\0';
		dat=(dat*50)/255;
		str[0]=dat/10+'0';
		str[2]=dat%10+'0';
		str[3]='v';
		LcdAreaClean(0,1,16);
		LcdShowStr(0,1,str);
	}
}
void eeprom()
{
	unsigned char c,str[2];
	bit n;
	str[1]='\0';
	if(rw==0)//自在读
	{
		if(addr!=10)
		{
			if(ds18flags)
			{
			str[0]=addr=addr+'0';
			LcdShowStr(10,1,str);
			c=E2ReadByte(addr);
			str[0]=c=c+'0';
			LcdShowStr(15,1,str);
			addr=10;
			}
		}
	}
	else
	{
		if(addr!=10)
		{
			if(ds18flags)//每次操作一次键盘延时一秒，延时显示和写入，应为键盘会影响lcd1602
			{
				str[0]=addr=addr+'0';
				if(n==0)//让lcd1602只显示一次
				{
					LcdAreaClean(14,1,2);//重复写入，以消除“ok”写入标志信号
					LcdShowStr(10,1,str);
					n=1;
				}
				if(m!=10)
				{
					addr=addr-'0';
					E2WriteByte(addr,m);//cant work
					LcdShowStr(14,1,str12);
					m=10;
					addr=10;
					n=0;
				}
			}
		}
	}
}
void stime(unsigned char k)
{
	if(s==1)
	{
		z=k;
	}
	if(s==2)
	{
		y=k;
	}
	if(s==3)
	{
		x=k;
	}
}

void keyaction(unsigned char keycode)
{
	switch(keycode)
	{
		case 'n':enui=1;en16022=1;break;
		case 'e':enui=0;en16022=1;i=1;en1602=1;
		LcdAreaClean(0,0,16);LcdShowStr(0,0,str5);
		deinit();break;
	}
	if(enui==0)//程序运行时，不能直接切换程序
	{
		switch(keycode)
		{
			case 'd':i=i-1;en1602=1;break;
			case 'u':i=i+1;en1602=1;break;
		}
	}
	if(i==7)i=0;if(i==0)i=6;
	
	if(t==0)//定时器专用键盘
	{
			switch(keycode)
		{
			case 'l':s++;break;
			case 'r':s--;break;
			case 1:stime(1);s--;break;
			case 2:stime(2);s--;break;
			case 3:stime(3);s--;break;
			case 4:stime(4);s--;break;
			case 5:stime(5);s--;break;
			case 6:stime(6);s--;break;
			case 7:stime(7);s--;break;
			case 8:stime(8);s--;break;
			case 9:stime(9);s--;break;
			case 0:stime(0);s--;break;
		}
	}
		if(e2==1)//EEPROM专用键盘
	{
		if(addr==10)//之操作一尺
		{
				switch(keycode)
			{
				case 1:addr=1;break;
				case 2:addr=2;break;
				case 3:addr=3;break;
				case 4:addr=4;break;
				case 5:addr=5;break;
				case 6:addr=6;break;
				case 7:addr=7;break;
				case 8:addr=8;break;
				case 9:addr=9;break;
				case 0:addr=0;break;
			}
			index=0;ds18flags=0;
		}
		if((ds18flags==1)&&(rw==1)&&(m==10))//写入m和写入addr延时一秒，否则会同时写入造成地址和数据值永远一样
		{
				switch(keycode)
			{
				case 1:m=1;break;
				case 2:m=2;break;
				case 3:m=3;break;
				case 4:m=4;break;
				case 5:m=5;break;
				case 6:m=6;break;
				case 7:m=7;break;
				case 8:m=8;break;
				case 9:m=9;break;
				case 0:m=0;break;
			}
			index=0;
			ds18flags=0;
		}
	}
}

void timer()
{
	if(t==0)//定时器暂停时数码管黑屏指示状态
	{
		if(s==1)
		{
			ledbuff[2]=0xff;
		}
		else ledbuff[2]=ledchar[z];
		if(s==2)
		{
			ledbuff[1]=0xff;
		}
		else ledbuff[1]=ledchar[y];
		if(s==3)
		{
			ledbuff[0]=0xff;
		}
		else ledbuff[0]=ledchar[x];
    sec=x*100+y*10+z;
	}
	else 
	{
		s=0;
	}
}
void IRreceive()
{
	if(irflag)
		irflag=0;	
		ledbuff[0]=ledchar[ircode[0]>>4];
		ledbuff[1]=ledchar[ircode[0]&0x0F];
		ledbuff[4]=ledchar[ircode[2]>>4];
		ledbuff[5]=ledchar[ircode[2]&0x0F];
}
unsigned char IntToString(unsigned char *str,int dat)
{
	signed char i=0;
	unsigned char len=0;
	unsigned char buf[6];

	if(dat<0)
	{
		dat=-dat;
		*str++='-';
		len++;
	}
	do{
		buf[i++]=dat%10;
		dat/=10;
	}while(dat>0);
	len+=i;
	while(i-->0)
	{
		*str++=buf[i]+'0';
	}
	*str='\0';
	return len;
}

void ds18b20()
{
	int xdata temp;
	int xdata intT,decT;
	unsigned char len;
	unsigned char str[12];
	if(ds18flags)
		{
			ds18flags=0;

			if(Get18B20Temp(&temp))
			{
				intT=temp>>4;
				decT=temp&0xF;
				len=IntToString(str,intT);
				str[len++]='.';
				decT=(decT*10)/16;
				str[len++]=decT+'0';
				while(len<6)
				{
					str[len++]=' ';
				}
				str[len]='\0';
				LcdAreaClean(0,1,16);
				LcdShowStr(0,1,str);
			}
			Start18B20();
		}
}
void ConfigTimer0()
{
	TMOD &= 0xF0;
	TMOD |= 0x01;
	TH0 = 0xf6;
	TL0 = 0x00;
	ET0 = 1;
	TR0 = 1;
}
void ShowNumber(unsigned char num)	 //对数字进行扫描处理
{
	signed char i;
	unsigned char buf[3];
	for(i=2;i>=0;i--)
		{
			buf[i]=num%10;
			num/=10;
		}
	for(i=2;i>=0;i--)
	{
		ledbuff[i]=ledchar[buf[i]];	 //选择数字显示
	}	
}
void ledscan()
{
	static unsigned char i = 0;
	 
	P0 = 0xFF;
	P2 = (P2 & 0xF8) | i;
	P0 = ledbuff[i];
	if (i < 6)
	i++;
	else
	i = 0;
}

void keydriver()
{
	unsigned char i, j;
	static unsigned char backup[4][4] = {
	{1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}
	};
	for (i=0; i<4; i++)
	{
		for (j=0; j<4; j++)
		{
			if (backup[i][j] !=keysta[i][j])
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
	keyout=keyout&0x03;
	switch(keyout)
	{
		case 0: P2=0x7f;break;
		case 1: P2=0xbf;break;
		case 2: P2=0xdf;break;
		case 3: P2=0xef;break;
		default:break;
	}
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
}
void InterruptTimer0() interrupt 1
{
	TH0 = 0xf6;
	TL0 = 0x00;
	keyscan();
	if(t==1)
	{
		if(entimer)
		{
			ShowNumber(sec);
		}
	}
	ledscan();
	index++;
	if(t==1)
	{
		if(index>=1000)
		{
			sec--;
			index=0;
			if(sec==0)
		{
			sec=60;
			P13=0;
		}
		else P13=1;
		}
	}
	if(index>=1000)
	{
		index=0;
		ds18flags=1;
	}
	x=sec/100%10;
	y=sec/10%10;
	z=sec%10;
	
}

void EXINT1_ISR() interrupt 0
{
	unsigned char i, j;
	unsigned int time;
	unsigned char byt;

	time = GetLowTime();
	if((time <23500) || (time > 26265))	   
	{
		IE0 = 0;
		return;	
	}

	time = GetHighTime();
	if((time<11059) || (time > 13824))
	{
		IE0 = 0;
		return;
	}
	for(i=0; i<4; i++)
	{
		for(j=0; j<8; j++)
		{
			time = GetLowTime();
			if((time<940) ||(time >2157))
			{
				IE0 = 0;
				return;
			}
			time = GetHighTime();
			if((time>940) && (time <2157))
			{
				byt >>= 1;	
			}
			else if((time>4037) && (time<5253))
			{
				byt >>= 1;
				byt |= 0x80;
			}
			else
			{
				IE0 = 0;
				return;
			}
		}
		ircode[i] = byt;
	}
	IE0 = 0;
	irflag=1;
}
void configuart()
{
	SCON=0x50;
	TMOD&=0x0f;
	TMOD|=0x20;
	TH1=0xf7;
	TL1=0xf7;
	ET1=0;
	ES=1;
	TR1=1;
}
void interruptuart() interrupt 4
{
	if(RI)
	{
		RI=0;
		SBUF=SBUF+1;
	}
	if(TI)
	{
		TI=0;
	}
}

void adc() interrupt 5 using 1
{
	ADC_CONTR&=!ADC_FLAG;
	dat = ADC_RES;
	ledbuff[2]=ledchar[dat%10];
	ledbuff[1]=ledchar[dat/10%10];
	ledbuff[0]=ledchar[dat/100%10];
	
	ADC_CONTR = ADC_POWER|ADC_SPEEDLL|ADC_START;
}