#include <REGX52.H>
#include <INTRINS.H>

sbit DIN = P3^4;
sbit DOUT = P3^7;
sbit CS = P3^5;
sbit DCLK = P3^6;

float Read_AD_Data(unsigned char cmd);