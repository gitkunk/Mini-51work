#include <Seg.h>

code unsigned char Seg_Dula[] = {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0X00,0X39,0x73,0X40,0X86,0X3E,0X88,0X38,0X89,0X8E,0X08,0X01};//C,P,-,E,U,A,L,H,F,_,^
code unsigned char Seg_Wela[] = {0X1C,0X18,0X14,0X10,0X0C,0X08,0X04,0X00};

void Seg_Disp(unsigned char wela,dula,point){
	P2 = (P2 & 0XE3) | Seg_Wela[wela];//1110 0011 = 0XE3

	P0 = 0X00;
	
	P0 = Seg_Dula[dula];

	if(point)
		P0 = Seg_Dula[dula] | 0X80;
	else
		P0 = Seg_Dula[dula];
}