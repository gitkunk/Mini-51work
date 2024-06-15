#include <Kunkunmv.h>

void Delay(unsigned int xms)
{

		while(xms--)
		{
					unsigned char i, j;

			i = 2;
			j = 199;
			do
			{
				while (--j);
			} while (--i);
	
		}
}
void _74HC595_WriteByte(unsigned char byte)
{
	unsigned char i;
	for(i=0;i<8;i++)
	{
		SER=byte&(0x80>>i);
		SCK=1;
		SCK=0;
	}
		RCK=1;
		RCK=0;
}
//led????
void LED(unsigned char Y,DATA)
{
	_74HC595_WriteByte(DATA);
	P0=~(0X80>>Y);
	Delay(1);
	P0=0XFF;
}
//led???
void LED_Init()
{
		SCK=0;
		RCK=0;
}