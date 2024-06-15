#include <Key.h>

unsigned char Key_Read(){
	unsigned char temp = 0;
	if(P3_1 == 0) temp = 5;
	if(P3_0 == 0) temp = 6;
	if(P3_2 == 0) temp = 7;
	if(P3_3 == 0) temp = 8;
	P1_4 = P1_5 = P1_6 = 1;P1_7 = 0;
	if(P1_3 == 0) temp = 1;
	if(P1_2 == 0) temp = 2;
	if(P1_1 == 0) temp = 3;
	P1_4 = P1_5 = P1_7 = 1;P1_6 = 0;
	if(P1_3 == 0) temp = 4;
	if(P1_2 == 0) temp = 9;
	return temp;
}