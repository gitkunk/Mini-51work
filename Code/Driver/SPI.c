#include <SPI.h>

float Read_AD_Data(unsigned char cmd)
{
	unsigned char i;
	unsigned int AD_Value = 0;
	DCLK = 0;
	CS  = 0;
	
	for(i=0; i<8; i++)
	{
		DIN = cmd >> 7; 
		cmd <<= 1;
		DCLK = 0;	
		DCLK = 1;
	}
 
	for(i=6; i>0; i--); 
	DCLK = 1;	
	_nop_();
	_nop_();
	DCLK = 0;
	_nop_();
	_nop_();
 
	for(i=0; i<12; i++)
	{
		AD_Value <<= 1;
		DCLK = 1;
		DCLK = 0;
		AD_Value |= DOUT;
	}
	CS = 1;
	
	return (AD_Value / 4096.0) * 5;//读取到的值为12位精度，除以2^12次方得到比例，乘以总量程5V即可得到电压值
}
