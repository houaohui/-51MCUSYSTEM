#include <stc12c5a60s2.h>
//#include <intrins.h>

sfr PLASF = 0x9d;

#define ADC_POWER 0x80
#define ADC_FLAG 0x10
#define ADC_START 0x08
#define ADC_SPEEDLL 0x00

void ADC_init()
{
	PLASF = 0x01;
	ADC_RES = 0;
	ADC_CONTR = ADC_POWER|ADC_SPEEDLL|ADC_START;
}


//void adc() interrupt 5 using 1
//{
//	unsigned dat;
//	ADC_CONTR&=!ADC_FLAG;
//	dat = ADC_RES;
//	
//	z=ledchar[dat%10];
//	y=ledchar[dat/10%10];
//	x=ledchar[dat/100%10];
//	
//	ADC_CONTR = ADC_POWER|ADC_SPEEDLL|ADC_START;
//}