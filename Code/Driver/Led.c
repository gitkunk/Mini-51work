/*⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐
⭐LED扫描底层函数只点亮一次，若要实现调亮需要在已点亮的基础上再切换亮灭调整占空比。
⭐基于LED点亮的流转时间Led_Ctro = 400ms，在定时器0中循环扫描LED灯，是否点亮
⭐若点亮，则进行PWM调亮，若不亮，则不对其进行操作
⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐*/

#include <Led.h>

static unsigned char temp = 0X00;
static unsigned char temp_Old = 0XFF;

void Led_Disp(unsigned char addr,enable){
	static unsigned char temp = 0X00;
	static unsigned char temp_Old = 0XFF;
	
	if(enable)
		temp |= 0X01 << addr;
	else
		temp &= ~(0X01 << addr);
	
	if(temp != temp_Old){//只点亮/关闭一次
		P2 = ~temp;
		temp_Old = temp;
	}
}