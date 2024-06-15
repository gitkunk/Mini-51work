/*
简易的时钟彩灯音乐盒
日期：2024-06-6
作者：1632242111031
参考：STC89C52硬件电路原理图

Simple real-time music box system with color lights
Date: 2024-06-6
Author: I roasted sweet potatoes in Fuzhou
Reference: STC89C52 hardware circuit schematic
Description: Please don't COPY.
anti-counterfeiting will be prosecuted
*/

/*⭐⭐⭐⭐⭐⭐⭐⭐Welcome to the works of sweet potato⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐*/

#include <REGX52.H>//51特殊寄存器定义相关头文件
#include <Kunkunmv.h>//点阵屏头文件
#include <INTRINS.H>//nop延时函数所在头文件
#include <DS18B20.h>//温度传感器头文件
#include <OneWire.h>//单总线头文件
#include <ds1302.h>//ds1302头文件
#include <SPI.h>//AD转换-->SPI通讯头文件
#include <Key.h>//按键底层判断头文件
#include <Led.h>//led闪烁头文件
#include <iic.h>//iic头文件
#include <Seg.h>//数码管底层头文件

/*系统启动开关*/
bit System_Begin;
/*上电流水灯参数*/
bit Led_Light;
/*按键,数码管显示,LED流水灯,时钟参数*/
unsigned char Seg_Disp_Page = 0;
unsigned char Key_Slow_Down,Seg_Slow_Down;						//按键及数码管减速参数
unsigned char Key_Val,Key_Down,Key_Up,Key_Old;				//按键参数(上升，下降沿)
unsigned char Seg_Buf[8] = {10,10,10,10,10,10,10,10};	//数码管段码显示
unsigned char Seg_Point[8] = {0,0,0,0,0,0,0,0};				//数码管小数点显示数组
unsigned char ucLed[8] = {0,0,0,0,0,0,0,0};						//Led显示数组
unsigned char Led_Wela[2][8] = {{0XFE,0XFD,0XFB,0XF7,0XEF,0XDF,0XBF,0X7F},//Led花式点亮段码
																{0X7E,0XBD,0XDB,0XE7,0XE7,0XDB,0XBD,0X7E}};//前四位0-7,1-6,2-5,3-4亮
unsigned char uc_Rtc[3] = {0X23,0X59,0X55};						//时钟初始值
unsigned char LED_OR_SEG_OR_BEEP;       							//LED/数码管/蜂鸣器 三大模式选择标志位 0- LED流转模式 1-数码管显示模式 2-音乐播放模式
unsigned char Seg_Disp_Page;		//数码管子界面切换参数 0-时钟显示界面,1-风扇等级显示界面,2-温度显示界面,3-LED流转参数设置界面
unsigned char Seg_Pos;					//数码管扫描位
/*LED花式点亮显示相关参数*/
bit Led_Page_Choose;						//流转显示界面-->流转设置界面 切换按键
bit Led_Dat_Choose;							//LED流转参数操作选择按键 0-亮度等级设置,1-流转时间设置
bit Light_Disp;									//等级(亮度/电压/光敏/热敏/外部电压)参数显示标志位 0-灭 1-亮 
unsigned int Led_Ctro = 400;		//流转时间控制参数
unsigned char Led_Mode_Dat = 1;	//模式编号
unsigned char Level = 2;	//等级参数(亮度/电压/光敏/热敏/外部电压)
unsigned char Led_Pos;					//Led的pwm选择控制位
unsigned char Led_cnt;					//Led的pwm计数参数
unsigned int Led_Time;					//Led流转时间计时参数
unsigned char k = 0;            //LED显示扫描循环参数
/*闪烁计时参数(在LED流转与温度的参数设置界面共用)*/
unsigned int Time400ms;					//闪烁计时
bit Shine_Flag;									//闪烁参数
/*风扇启停/四通道AD转换相关参数(光敏,热敏,变压器,外部通道)*/
float Voltage;                 	//SPI读取AD转换电压值,该值关联风扇PWM占空比,进一步控制风扇速度
unsigned char Ad_Disp_Choose;		//AD显示子界面切换参数 0-风扇等级/电压值显示子界面 1-亮度值显示子界面 2-电阻温度值显示子界面 3-外部电压值显示子界面
bit Fan_Ctro;										//风扇启停控制参数
/*温度控制相关参数*/
float Temperature = 25;         //单总线读取温度值
bit Temp_Page_Choose;						//温度采集界面-->设置上下限界面 切换按键
bit Temp_Dat_Choose;						//温度阈值参数操作选择按键
bit Beep_Ring;									//温度告警参数
unsigned char Temp_Max = 30;		//温度阈值上限
unsigned char Temp_Min = 20;		//温度阈值下限
/*蜂鸣器相关参数*/
sbit BEEP=P2^5;									//蜂鸣器接口
bit Music_Switch;								//音乐启停开关(按键控制)
bit Music_Stop;									//音乐暂停继续开关
bit flag;												//音乐启停开关(标志位)(0/1-->控制蜂鸣器响与不响)
unsigned int h,i,hz;						//hz-频率(该参数对应音调的频率) h-音调(该参数对应音调数组song(do,re,mi,fa...)) i-计数参数(控制音乐的进行)
unsigned char jiep;							//节拍参数控制音符的持续时间,在定时器二中计时(一次50ms)
unsigned int code G_fre[]={63928,64103,64260,			//#2,4,5 低音
64331,64463,64580,64633,64732,64820,64898,64934	//#5,#6,1,#1,#2,4,5,#5 中音
,0,64777};//#G调音阶，最后一个为中音3
unsigned char code song[]={
1,6,5,3,6,3,5,3,6,3,5,6,1,6,11,12,																//我应该拿你怎样
1,6,0,3,1,3,1,3,0,3,1,6,1,6,11,12,																//所有人都在看着我
1,3,1,3,6,6,5,3,6,3,5,6,1,6,11,12,																//我的心总是不安
1,6,0,3,1,3,1,3,0,3,1,6,1,6,3,12,																	//我现在已病入膏肓
6,12,5,12,1,6,1,6,11,6,0,6,1,6,11,6,6,3,6,3,5,3,6,3,5,6,1,6,11,12,//e~o，真的，难道，因你而疯狂吗
1,6,0,3,1,3,1,3,0,3,1,6,1,6,1,12,11,6,														//我本来不是这种人
1,6,0,3,1,3,1,3,0,3,1,6,1,6,3,12,																	//因你变成奇怪的人
10,12,9,12,7,12,5,6,5,6,10,12,9,12,7,6,5,12,7,12,11,6,						//第一次呀变成这样的我
5,3,5,6,5,6,12,6,12,6,12,6,8,6,9,6,																//不管我怎么去否认
1,6,5,6,5,6,8,12,11,12,11,12,11,6,1,3,1,6,11,12,									//鸡你太美，baby
1,6,5,6,5,6,8,12,11,12,11,12,11,6,1,3,1,6,11,12,11,12,						//鸡你太美，baby
1,6,5,6,5,6,8,12,11,12,11,12,11,6,1,3,1,6,11,12,									//鸡你太美，baby
1,6,5,6,5,6,8,12,11,12,11,12,11,6,1,3,1,6,11,12,11,12,						//鸡你太美，baby
5,3,5,3,8,3,5,3,5,3,5,3,5,3,5,3,5,3,4,3,5,6,5,6,									//迎面走来的你让我如此蠢蠢欲动
5,3,5,3,8,3,5,3,5,3,5,3,5,3,5,3,5,3,4,3,5,6,5,6,									//这种感觉我从未拥有，cause i got a crush on you
}; //歌谱 2个一组(0,0)(音调,节拍(一拍对应值为12))

/*蜂鸣器播放音乐相关函数*/
void sing(){
	if(Music_Switch){
		if(flag==1){
			h=song[i];
			jiep=song[i+1];
			i=i+2;
			if(i>254)
				i=0;
			hz=G_fre[h];
			TH1=(hz)/256;
			TL1=(hz)%256;
			flag=0;
			TR2=1;
			TR1=1;
		}
	}
}

void Sing_Init(){//开关音乐初始化函数
	i = 0;
	TR1 = 0;
	TR2 = 0;
}

void Sing_Off(){//启停函数-->关定时器-->暂停
	TR1 = 0;
	TR2 = 0;
}

void Sing_On(){//启停函数-->开定时器-->播放
	TR1 = 1;
	TR2 = 1;
}
/*系统初始化函数*/
void Sys_Init(void){
	TMOD=0x11;								//定时器0工作在模式1 定时1ms
	T2MOD=0;									//定时器1模式1 用作音调发生器
	T2CON=0;									//定时器2定时模式 定时50ms 用作延音
	TL0=(65536 - 1000) % 256;	//设置定时低八位初始值
	TH0=(65536 - 1000) / 256;	//设置定时高八位初始值
	TH2=0;
	TL2=0;
	TF0=0;
	TR0=1;
	ET0=1;
	ET1=1;
	ET2=1;
	EA=1;
	i=0;
	flag=1;
	P1_0 = 0;
}
/*系统关闭函数*/
void Sys_Off(void){
	TF0=0;
	TR0=0;
	ET0=0;
	ET1=0;
	ET2=0;
	EA=0;
	i=0;
	flag=0;
	P1_0 = 0;
	P0 = 0;
	P2 = 0XFF;
}
/*按键处理函数*/
void Key_Proc(){
	if(System_Begin){
		if(Key_Slow_Down) return;
		Key_Slow_Down = 1;
	}
	Key_Val = Key_Read();
	Key_Down = Key_Val & (Key_Old ^ Key_Val);
	Key_Up = ~Key_Val & (Key_Old ^ Key_Val);
	Key_Old = Key_Val;
	
	switch(Key_Down){
		case 9:
			if(!System_Begin) Led_Light = ~Led_Light;
			if(System_Begin) Sys_Off();
			System_Begin = ~System_Begin;
			
		break;
		case 8://-按键(对一些参数的减法操作) 兼容亮度等级显示 
			if(!Led_Dat_Choose && Seg_Disp_Page == 3){
				if(--Led_Mode_Dat == 0)
					Led_Mode_Dat = 4;
					EEPROM_Write(&Led_Mode_Dat,0,1);
				}
			if(Led_Dat_Choose && Seg_Disp_Page == 3){
				Led_Ctro -= 100;
				if(Led_Ctro < 400) Led_Ctro = 1200;
			}
			if(Seg_Disp_Page == 1){
				Light_Disp = ~Light_Disp;
			}
			if(Seg_Disp_Page == 2 && Temp_Page_Choose && !Temp_Dat_Choose){
				Temp_Min -= 1;
				if(Temp_Min < 20) Temp_Min = 20;
				EEPROM_Write(&Temp_Min,2,1);
			}
			if(Seg_Disp_Page == 2 && Temp_Page_Choose && Temp_Dat_Choose){
				Temp_Max -= 1;
				if(Temp_Max < 20) Temp_Max = 20;
				EEPROM_Write(&Temp_Max,1,1);
			}
		break;
		case 7://子界面切换按键兼容参数设置(对上下限以及流转参数的设置)
			if(LED_OR_SEG_OR_BEEP == 1){//仅限在数码管界面设置
				if(!Led_Page_Choose){			//若在流转参数设置界面，则子界面不切换
					if(++Seg_Disp_Page == 4) Seg_Disp_Page = 0;//子界面切换
					if(Seg_Disp_Page == 0){	//返回时钟首界面，相关参数置零
						Ad_Disp_Choose = 0;		//AD显示切换参数置为风扇等级显示界面
						Temp_Dat_Choose = 0;	//温度采集上下限设置界面置温度上限闪烁
						Led_Dat_Choose = 1;		//LED流转参数设置界面置亮度等级闪烁
					}
					if(Fan_Ctro){		//若风扇转动时切换子界面,风扇停止
						P1_0 = 0;			//引脚置低电平
						Fan_Ctro = 0;	//风扇启停控制参数清零
					}
				}
				if(Seg_Disp_Page == 3){//流转参数设置界面
					Led_Dat_Choose = ~Led_Dat_Choose;//对参数上下限进行选择
				}
				if(Seg_Disp_Page == 3 && Temp_Page_Choose){
					Seg_Disp_Page = 2;
					Temp_Dat_Choose = ~Temp_Dat_Choose;
				}
			}
		break;
		case 6://+按键(对一些参数的加操作)
			if(!Led_Dat_Choose && Seg_Disp_Page == 3){
				if(++Led_Mode_Dat >= 5)
					Led_Mode_Dat = 1;
				EEPROM_Write(&Led_Mode_Dat,0,1);
			}
			if(Led_Dat_Choose && Seg_Disp_Page == 3){
				Led_Ctro += 100;
				if(Led_Ctro > 1200) Led_Ctro = 400;
			}
			if(Seg_Disp_Page == 2 && Temp_Page_Choose && !Temp_Dat_Choose){
				Temp_Min += 1;
				if(Temp_Min > 35) Temp_Min = 35;
				EEPROM_Write(&Temp_Min,2,1);
			}
			if(Seg_Disp_Page == 2 && Temp_Page_Choose && Temp_Dat_Choose){
				Temp_Max += 1;
				if(Temp_Max > 35) Temp_Max = 35;
				EEPROM_Write(&Temp_Max,1,1);
			}
		break;
		case 5://LED与数码管切换显示按键
			if(LED_OR_SEG_OR_BEEP == 1){//若用户在参数设置界面切换模式 对相关闪烁参数以及界面选择参数操作
				Seg_Disp_Page = 0;		//若模式切换 置数码管初始界面为时钟首界面
				Ad_Disp_Choose = 0;		//AD显示切换参数置为风扇等级显示界面
				Temp_Page_Choose = 0;	//温度界面置温度检测界面
				Temp_Dat_Choose = 0;	//温度采集上下限设置界面置温度上限闪烁,温度下限不闪烁
				Led_Dat_Choose = 1;		//LED流转参数设置界面置亮度等级闪烁,流转参数不闪烁
				Light_Disp = 0;				//等级显示参数清零 默认不显示等级
				Led_Page_Choose = 0;	//LED参数界面置LED正常显示界面
			}
			if(++LED_OR_SEG_OR_BEEP == 3) LED_OR_SEG_OR_BEEP = 0;//模式切换
			if(Fan_Ctro){//若风扇转动时切换子界面,风扇停止
				P1_0 = 0;//引脚置低电平
				Fan_Ctro = 0;//风扇启停控制参数清零
			}
		break;
		case 1://数码管显示模式数码管显示子界面切换按键
			if(Seg_Disp_Page == 1){//AD显示子界面切换
     		if(++Ad_Disp_Choose == 4) Ad_Disp_Choose = 0;
      }
			if(Seg_Disp_Page == 2){//温度检测与设置界面切换
				Temp_Page_Choose = ~Temp_Page_Choose;
			}
			if(Seg_Disp_Page == 3){//LED正常显示与设置界面切换
				Led_Page_Choose = ~Led_Page_Choose;
			}
			if(Fan_Ctro){		//若风扇转动时切换子界面,风扇停止
				P1_0 = 0;			//引脚置低电平
				Fan_Ctro = 0;	//风扇启停控制参数清零
			}
		break;
		case 2://音乐播放
			if(LED_OR_SEG_OR_BEEP == 2){
				if(!Music_Switch) Sing_Init();
				Music_Switch = ~Music_Switch;
			}
		break;
		case 3://音乐暂停
			if(Music_Switch == 1){
				if(!Music_Stop) Sing_Off();
				else Sing_On();
				Music_Stop = ~Music_Stop;
			}
		break;
		case 4://风扇启动按键
			if(LED_OR_SEG_OR_BEEP == 1 && Seg_Disp_Page == 1){
				if(Fan_Ctro) P1_0 = 0;
				Fan_Ctro = ~Fan_Ctro;
			}
		break;
	}
}
/*数码管显示函数*/
void Seg_Proc(){
	if(Seg_Slow_Down) return;
	Seg_Slow_Down = 1;
	
	switch(Seg_Disp_Page){
		case 0:
			Read_Clk(uc_Rtc);
			Seg_Buf[0] = uc_Rtc[0] / 16 % 16;
			Seg_Buf[1] = uc_Rtc[0] % 16;
			Seg_Buf[2] = 13;
			Seg_Buf[3] = uc_Rtc[1] / 16 % 16;
			Seg_Buf[4] = uc_Rtc[1] % 16;
			Seg_Buf[5] = 13;
			Seg_Buf[6] = uc_Rtc[2] / 16 % 16;
			Seg_Buf[7] = uc_Rtc[2] % 16;
			Seg_Point[2] = 0;
			Seg_Point[5] = 0;
		break;
		case 1:
			if(Ad_Disp_Choose == 0){
				Voltage = Read_AD_Data(0x94);//电位器AD转换实际0~4.93V
				Level = (unsigned char)(Voltage * 2);//等级细分0~9级		
				Seg_Buf[0] = 15;
				Seg_Buf[1] = Light_Disp ? Level <= 3 ? 20 : Level > 6 ? 21 : 13 : 10;        
				Seg_Buf[2] = Light_Disp ? (unsigned char)Voltage % 10 : 10;        
				Seg_Buf[3] = Light_Disp ? (unsigned char)(Voltage * 10) % 10 : 10; 
				Seg_Buf[4] = Light_Disp ? (unsigned char)(Voltage * 100) % 10 : 10;
				Seg_Buf[5] = 13;
				Seg_Buf[6] = Level;
				Seg_Buf[7] = 13;
				Seg_Point[2] = Light_Disp ? 1 : 0;
				Seg_Point[5] = 0;
			} else if(Ad_Disp_Choose == 1){//光敏电阻AD转换实际0~4.93V 0XA4
				Voltage = Read_AD_Data(0xA4);//电位器AD转换实际0~4.93V
				Level = (unsigned char)(Voltage * 2);//等级细分0~9级		
				Seg_Buf[0] = 17;
				Seg_Buf[1] = Light_Disp ? 13 : 10;
				Seg_Buf[2] = Light_Disp ? Level : 10;
				Seg_Buf[3] = Light_Disp ? 13 : 10;
				Seg_Buf[4] = 10;
				Seg_Buf[5] = (unsigned char)Voltage % 10;
				Seg_Buf[6] = (unsigned char)(Voltage * 10) % 10;
				Seg_Buf[7] = (unsigned char)(Voltage * 100) % 10;
				Seg_Point[2] = 0;
				Seg_Point[5] = 1;
			} else if(Ad_Disp_Choose == 2){//热敏电阻AD转换实际0~4.93V 0XD4
				Voltage = Read_AD_Data(0xD4);//电位器AD转换实际0~4.93V
				Level = (unsigned char)(Voltage * 2);//等级细分0~9级		
				Seg_Buf[0] = 11;
				Seg_Buf[1] = Light_Disp ? 13 : 10;
				Seg_Buf[2] = Light_Disp ? Level : 10;
				Seg_Buf[3] = Light_Disp ? 13 : 10;
				Seg_Buf[4] = 10;
				Seg_Buf[5] = (unsigned char)Voltage % 10;
				Seg_Buf[6] = (unsigned char)(Voltage * 10) % 10;
				Seg_Buf[7] = (unsigned char)(Voltage * 100) % 10;
				Seg_Point[2] = 0;
				Seg_Point[5] = 1;
			} else if(Ad_Disp_Choose == 3){//AIN3位上的AD转换实际0~4.93V 0XE4
				Voltage = Read_AD_Data(0xE4);//电位器AD转换实际0~4.93V
				Level = (unsigned char)(Voltage * 2);//等级细分0~9级		
				Seg_Buf[0] = 15;
				Seg_Buf[1] = 0;
				Seg_Buf[2] = Light_Disp ? 13 : 10;
				Seg_Buf[3] = Light_Disp ? Level : 10;
				Seg_Buf[4] = Light_Disp ? 13 : 10;
				Seg_Buf[5] = (unsigned char)Voltage % 10;
				Seg_Buf[6] = (unsigned char)(Voltage * 10) % 10;
				Seg_Buf[7] = (unsigned char)(Voltage * 100) % 10;
				Seg_Point[2] = 0;
				Seg_Point[5] = 1;
			}
		break;
		case 2:
			if(!Temp_Page_Choose){
				DS18B20_ConvertT();
				Temperature = DS18B20_ReadT();
				Seg_Buf[0] = 11;
				Seg_Buf[1] = 10;
				Seg_Buf[2] = 10;
				Seg_Buf[3] = 10;
				Seg_Buf[4] = (unsigned char)(Temperature / 10) % 10;
				Seg_Buf[5] = (unsigned char)(Temperature) % 10;
				Seg_Buf[6] = (unsigned int)(Temperature * 10) % 10;
				Seg_Buf[7] = (unsigned int)(Temperature * 100) % 10;
				Seg_Point[2] = 0;
				Seg_Point[5] = 1;
			} else{
				Seg_Buf[0] = 12;
				Seg_Buf[1] = 13;
				Seg_Buf[2] = !Temp_Dat_Choose && Shine_Flag ? 10 : Temp_Min / 10 % 10;
				Seg_Buf[3] = !Temp_Dat_Choose && Shine_Flag ? 10 : Temp_Min % 10;
				Seg_Buf[4] = 13;
				Seg_Buf[5] = Temp_Dat_Choose && Shine_Flag ? 10 : Temp_Max / 10 % 10;
				Seg_Buf[6] = Temp_Dat_Choose && Shine_Flag ? 10 : Temp_Max % 10;
				Seg_Buf[7] = 13;
				Seg_Point[2] = 0;
				Seg_Point[5] = 0;
			}
			if(Temperature > Temp_Max){
				Beep_Ring = 1;
				Fan_Ctro = 1;
			} else if(Temperature < Temp_Min){
				Beep_Ring = 1;
				Fan_Ctro = 0;
			} else{
				Beep_Ring = 0;
				Fan_Ctro = 0;
			}
		break;
		case 3:
			if(Led_Page_Choose){
				Seg_Buf[0] = 13;
				Seg_Buf[1] = !Led_Dat_Choose && Shine_Flag ? 10 : Led_Mode_Dat;
				Seg_Buf[2] = 13;
				Seg_Buf[3] = 10;
				Seg_Buf[4] = Led_Dat_Choose && Shine_Flag ? 10 : Led_Ctro >= 1000 ? Led_Ctro / 1000 % 10 : 10;
				Seg_Buf[5] = Led_Dat_Choose && Shine_Flag ? 10 : Led_Ctro / 100 % 10;
				Seg_Buf[6] = Led_Dat_Choose && Shine_Flag ? 10 : Led_Ctro / 10 % 10;
				Seg_Buf[7] = Led_Dat_Choose && Shine_Flag ? 10 : Led_Ctro % 10;
				Seg_Point[2] = 0;
				Seg_Point[5] = 0;
			} else {//正常显示
				Seg_Buf[0] = 13;
				Seg_Buf[1] = Led_Mode_Dat;
				Seg_Buf[2] = 13;
				Seg_Buf[3] = 10;
				Seg_Buf[4] = Led_Ctro >= 1000 ? Led_Ctro / 1000 % 10 : 10;
				Seg_Buf[5] = Led_Ctro / 100 % 10;
				Seg_Buf[6] = Led_Ctro / 10 % 10;
				Seg_Buf[7] = Led_Ctro % 10;
				Seg_Point[2] = 0;
				Seg_Point[5] = 0;
			}
		break;
	}
}
/*LED显示函数*/
void Led_Proc(){
	static char k = 0;
	switch(Led_Mode_Dat){
		case 1:
			for(k = 0;k < 8;k++){
				ucLed[k] = Led_Pos == k ? 1 : 0;
			}
		break;
		case 2:
			for(k = 0;k < 8;k++){
				ucLed[k] = Led_Pos == 7 - k ? 1 : 0;
			}
		break;
		case 3:
			for(k = 0;k < 4;k++){
				ucLed[k] = ucLed[7 - k] = Led_Pos == 4 + k ? 1 : 0;
			}
		break;
		case 4:
			for(k = 4;k >= 0;k--){
				ucLed[k] = ucLed[7 - k] = Led_Pos == k ? 1 : 0;
			}
		break;
	}
}
/*延时函数*/
void Delay750ms(){								//@12.000MHz
	unsigned char i, j, k;

	_nop_();
	i = 6;
	j = 180;
	k = 26;
	do{
		do{
			while (--k);
		}while (--j);
	}while (--i);
}

void Delayxms(unsigned char n){		//@12.000MHz
	unsigned char i, j;
	
	while(n--){
	i = 2;
	j = 239;
		do
		{
			while (--j);
		} while (--i);
	}
}

/*启动显示LED花式点亮函数*/
void LED_Fancy_Light(){
	for(i = 0;i < 8;i++){
		P2 = Led_Wela[0][i];
		Delayxms(30);
	}
	for(i = 0;i < 8;i++){
		P2 = Led_Wela[0][7-i];
		Delayxms(50);
	}
	for(i = 0;i < 4;i++){
		P2 = Led_Wela[1][i];
		Delayxms(80);
	}
	for(i = 0;i < 4;i++){
		P2 = Led_Wela[1][i+4];
		Delayxms(100);
	}
}
/*主函数*/
void main(){
	unsigned char i,py=0,pl=0;
	P1_0 = 0;
	P0 = 0;
	LED_Init();											//SPI RCLK(P3^5) SRCL(P3^6)初始化
	EEPROM_Read(&Led_Mode_Dat,0,1);	//数据读取必须放在程序最开始
	Delay750ms();										//稍微延时
	EEPROM_Read(&Temp_Max,1,1);			//温度阈值数据读取
	EEPROM_Read(&Temp_Min,2,1);			//温度阈值数据读取
	Set_Clk(uc_Rtc);								//时钟数据初始化
	Voltage = Read_AD_Data(0x94);		//上电先转换电压数据，防止第一次读数据错误
	DS18B20_ConvertT();							//上电先转换一次温度，防止第一次读数据错误
	Delay750ms();										//等待转换完成

	while(1){
		Key_Proc();
		if(System_Begin){
			if(Led_Light){
				LED_Fancy_Light();							//程序启动显示 LED花式点亮
				Sys_Init();											//寄存器以及相关参数初始化	
				Led_Light = 0;
			}
			if(LED_OR_SEG_OR_BEEP == 1){//数码管位选端与LED因为共用P2端口，P2端口冲突，这里数码管与LED显示分隔，通过按键K1操控。
				Seg_Proc();//数码管显示函数 200ms进一次
				if(Beep_Ring){//温度报警与否
					BEEP = ~BEEP;
				}
			} else if(LED_OR_SEG_OR_BEEP == 2){
				/*初始化数码管，灭的状态*/
				Seg_Buf[0] = 10;
				Seg_Buf[1] = 10;
				Seg_Buf[2] = 10;
				Seg_Buf[3] = 10;
				Seg_Buf[4] = 10;
				Seg_Buf[5] = 10;
				Seg_Buf[6] = 10;
				Seg_Buf[7] = 10;
				Seg_Point[5] = 0;
				sing();//蜂鸣器播放音乐
				/*LED点阵屏显示kunkun*/
				for(i=0;i<8;i++){
					LED(i,DH[i+py]);
				}
				pl++;
				if(pl>25){
					pl=0;
					py+=8;
					if(py>10000) py=0;
				}
			} else {
				/*初始化数码管，灭的状态*/
				Seg_Buf[0] = 10;
				Seg_Buf[1] = 10;
				Seg_Buf[2] = 10;
				Seg_Buf[3] = 10;
				Seg_Buf[4] = 10;
				Seg_Buf[5] = 10;
				Seg_Buf[6] = 10;
				Seg_Buf[7] = 10;
				Seg_Point[5] = 0;
				/*LED轮流显示界面同时进行AD转换，进行PWM调亮*/
				Level = (unsigned char)(Voltage * 2);//等级细分0~9级
				Voltage = Read_AD_Data(0x94);//AD转换实际0~4.93V
				Led_Proc();
			}
		}
	}
}
/*定时器0服务函数*/
void Timer0_Server() interrupt 1{
	TL0 = 0x18;		//设置定时初值
	TH0 = 0xFC;		//设置定时初值
	if(++Key_Slow_Down == 10) Key_Slow_Down = 0;
	if(++Seg_Slow_Down == 200) Seg_Slow_Down = 0;
	if(LED_OR_SEG_OR_BEEP == 1){
		if(++Seg_Pos == 8) Seg_Pos = 0;
		Seg_Disp(Seg_Pos,Seg_Buf[Seg_Pos],Seg_Point[Seg_Pos]);
			if(Seg_Disp_Page >= 1){
				if(++Time400ms == 400){
					Time400ms = 0;
					Shine_Flag = ~Shine_Flag;
				}
			}
		if(Fan_Ctro){
			Led_cnt++;
			if(Seg_Disp_Page == 1 && Ad_Disp_Choose == 0){
				if(Led_cnt < Level){
					P1_0 = 1;
				} else if(Led_cnt < 10){
					P1_0 = 0;
				} else {
					Led_cnt = 0;
				}
			} else if(Seg_Disp_Page == 2){
				if(Led_cnt < Temperature - Temp_Max){
					P1_0 = 1;
				} else if(Led_cnt < 10){
					P1_0 = 0;
				} else {
					Led_cnt = 0;
				}
			}
		}
	} else if(LED_OR_SEG_OR_BEEP == 0){
		Led_Disp(Seg_Pos,ucLed[Seg_Pos]);//LED扫描底层函数只点亮一次，需要手动切换亮灭调整占空比。
		if(++Led_Time >= Led_Ctro){
			if(++Led_Pos == 8){
				Led_Pos = 0;
				if(++Led_Mode_Dat == 5){
					Led_Mode_Dat = 1;
				}
				if(Led_Mode_Dat == 3 || Led_Mode_Dat == 4)Led_Pos = 4;
			}
			Led_Time = 0;
		}
		for(k=0;k<8;k++){
			if(Led_Pos == k){
				Led_cnt++;
				if(Led_cnt < Level){
					if(Led_Mode_Dat == 1){
						P2 = Led_Wela[0][Led_Pos];
					} else if(Led_Mode_Dat == 2){
						P2 = Led_Wela[0][7-Led_Pos];
					} else if(Led_Mode_Dat == 3){
						P2 = Led_Wela[1][Led_Pos - 4];
					} else {
						P2 = Led_Wela[1][Led_Pos];
					}
				} else if(Led_cnt < 10){
					P2 = 0XFF;
				} else {
					Led_cnt = 0;
				}
			}
		}
	}
}
/*定时器1服务函数*/
void time1() interrupt 3{
	TH1=(hz)/256;
	TL1=(hz)%256;
	if(!flag)
		BEEP=!BEEP;
	else
		TR1=0;
}

/*定时器2服务函数*/
void time2() interrupt 5 {
	RCAP2H=(15536)/256;
	RCAP2L=(15536)%256;
	--jiep;
	if(jiep==0)
		flag=1;
	TF2=0;
}