#include <stc12c5a60s2.h>
sbit DS = P3^5; //1_wire

void delay(unsigned int us)
{
	while(us--);
}

//��ʼ������
bit ds_init()
{
	bit i;
	DS=1;
	delay(1);
	DS=0;
	delay(998);//��ʱ480us���ϣ�ds18b20������λ
	DS=1;
	delay(59);//15~60us,�ȴ�
	i=DS;
	delay(300);//����֮��ȴ�60~240us
	DS=1;
	delay(1);
	return (i);
}

//дһ���ֽ�
void write_byte(unsigned char dat)
{
	unsigned char i;
	for(i=0;i<8;i++)
	{
		DS=0;
		delay(1);
		DS=dat&0x01;
		delay(150);//��ʱ60us����
		DS=1; //�ͷ�����׼���´�����д��
		delay(1);//1us
		dat >>= 1;
	}
}
//��һ���ֽ�
unsigned char read_byte()
{
	unsigned char i,j,dat;
	for(i=0;i<8;i++)
	{
		DS=0;//������ʱ��
		delay(1);//1us
		DS=1;//�ͷ����߲���
		delay(1);//1us
		j=DS;
		delay(150);//60us����
		DS=1;
		delay(1);//1us
		dat=(j<<7)|(dat>>1);
	}
	return(dat);
}
bit Start18B20()
{
	bit ack;
	EA=0;
	ack=ds_init();
	if(ack==0)
	{
		write_byte(0xcc);
		write_byte(0x44);
	}
	EA=1;
	return(~ack);
}


bit Get18B20Temp(int *temp)
{
	bit ack;
	unsigned char LSB,MSB;
	EA=0;
	ack=ds_init();
	if(ack==0)
	{
		write_byte(0xcc);
		write_byte(0xbe);
		LSB=read_byte();
		MSB=read_byte();
		*temp=((int)MSB<<8)+LSB;
	}
	EA=1;
	return ~ack;
}