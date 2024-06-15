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

unsigned char KeyNum;
unsigned char Seg_Disp_Page = 0;
unsigned char Key_Slow_Down,Seg_Slow_Down;						//按键及数码管减速参数
unsigned char Key_Val,Key_Down,Key_Up,Key_Old;				//按键参数(上升，下降沿)
unsigned char Seg_Buf[8] = {10,10,10,10,10,10,10,10};	//数码管段码显示
unsigned char Seg_Point[8] = {0,0,0,0,0,0,0,0};				//数码管小数点显示数组
unsigned char ucLed[8] = {0,0,0,0,0,0,0,0};						//Led显示数组
unsigned char Led_Wela[2][8] = {{0XFE,0XFD,0XFB,0XF7,0XEF,0XDF,0XBF,0X7F},//Led花式点亮段码
																{0X7E,0XBD,0XDB,0XE7,0XE7,0XDB,0XBD,0X7E}};//前四位0-7,1-6,2-5,3-4亮
unsigned char uc_Rtc[3] = {0X23,0X59,0X55};						//时钟初始值
unsigned char LED_OR_SEG_OR_BEEP;       							//LED/数码管/蜂鸣器 功能选择标志位
unsigned char Seg_Pos;					//数码管扫描位
unsigned char Seg_Disp_Page;		//设置界面
bit Led_Page_Choose;						//流转显示界面-->流转设置界面 切换按键
bit Led_Dat_Choose;							//LED流转参数操作选择按键
unsigned int Led_Ctro = 400;		//流转间隔控制参数
unsigned int Time400ms;					//闪烁计时
bit Shine_Flag;									//闪烁参数
unsigned char Led_Mode_Dat = 1;	//模式编号
bit Light_Disp;									//亮度等级参数显示标志位 0-灭 1-亮
unsigned char Light_Level = 2;	//亮度等级参数
unsigned char Led_Pos;					//Led的pwm选择控制位
unsigned char Led_cnt;					//Led的pwm计数参数
unsigned int Led_Time;					//Led流转时间计时参数
unsigned char k = 0;            //LED显示扫描循环参数
unsigned int h,i,hz;						//hz-频率(该参数对应音调的频率) h-音调(该参数对应音调数组song(do,re,mi,fa...)) i-计数参数(控制音乐的进行)
unsigned char jiep;							//节拍参数控制音符的持续时间,在定时器二中计时(一次50ms)
float Voltage;                 	//SPI读取AD转换值
unsigned char Ad_Disp_Choose;		//AD显示切换参数
float Temperature = 25;         //单总线读取温度值
bit Temp_Page_Choose;						//温度采集界面-->设置上下限界面 切换按键
bit Temp_Dat_Choose;						//温度阈值参数操作选择按键
unsigned char Temp_Max = 30;		//温度阈值上限
unsigned char Temp_Min = 20;		//温度阈值下限

sbit BEEP=P2^5;				//蜂鸣器接口
bit Music_Switch;			//音乐启停开关(按键控制)
bit Music_Stop;				//音乐暂停继续开关
bit flag;							//音乐启停开关(标志位)(0/1-->控制蜂鸣器响与不响)

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

void Sing_Init(){
	i = 0;
	TR1 = 0;
	TR2 = 0;
}

void Sing_Off(){
	TR1 = 0;
	TR2 = 0;
}

void Sing_On(){
	TR1 = 1;
	TR2 = 1;
}

void Key_Proc(){
	if(Key_Slow_Down) return;
	Key_Slow_Down = 1;
	
	Key_Val = Key_Read();
	Key_Down = Key_Val & (Key_Old ^ Key_Val);
	Key_Up = ~Key_Val & (Key_Old ^ Key_Val);
	Key_Old = Key_Val;
	
	switch(Key_Down){
		case 7://-按键
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
		case 6://设置按键
			if(LED_OR_SEG_OR_BEEP == 1){//仅限在数码管界面设置
				if(!Led_Page_Choose){
					if(++Seg_Disp_Page == 4) Seg_Disp_Page = 0;
					if(Seg_Disp_Page == 0){
						Ad_Disp_Choose = 0;
						Temp_Dat_Choose = 0;
						Led_Dat_Choose = 1;
					}
				}
				if(Seg_Disp_Page == 3){
					Led_Dat_Choose = ~Led_Dat_Choose;
				}
				if(Seg_Disp_Page == 3 && Temp_Page_Choose){
					Seg_Disp_Page = 2;
					Temp_Dat_Choose = ~Temp_Dat_Choose;
				}
			}
		break;
		case 5://+按键
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
		case 4://LED与数码管切换显示按键
			if(LED_OR_SEG_OR_BEEP == 1){
				Seg_Disp_Page = 0;
			}
			if(++LED_OR_SEG_OR_BEEP == 3) LED_OR_SEG_OR_BEEP = 0;
		break;
		case 1://亮度等级显示按键
			if(Seg_Disp_Page == 1){
     		if(++Ad_Disp_Choose == 4) Ad_Disp_Choose = 0;
      }
			if(Seg_Disp_Page == 2){
				Temp_Page_Choose = ~Temp_Page_Choose;
			}
			if(Seg_Disp_Page == 3){
				Led_Page_Choose = ~Led_Page_Choose;
			}
		break;
		case 2:
			if(LED_OR_SEG_OR_BEEP == 2){
				if(!Music_Switch) Sing_Init();
				Music_Switch = ~Music_Switch;
			}
		break;
		case 3:
			if(Music_Switch == 1){
				if(!Music_Stop) Sing_Off();
				else Sing_On();
				Music_Stop = ~Music_Stop;
			}
		break;
	}
	if(Key_Up == 7){
		if(Seg_Disp_Page == 1)
			Light_Disp = ~Light_Disp;
	}
}

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
			Seg_Point[5] = 0;
		break;
		case 1:
			if(Ad_Disp_Choose == 0){
				Voltage = Read_AD_Data(0x94);//电位器AD转换实际0~4.93V
				Light_Level = (unsigned char)(Voltage * 2);//等级细分0~9级		
				Seg_Buf[0] = 15;
				Seg_Buf[1] = Light_Disp ? 13 : 10;
				Seg_Buf[2] = Light_Disp ? Light_Level : 10;
				Seg_Buf[3] = Light_Disp ? 13 : 10;
				Seg_Buf[4] = 10;
				Seg_Buf[5] = (unsigned char)Voltage % 10;
				Seg_Buf[6] = (unsigned char)(Voltage * 10) % 10;
				Seg_Buf[7] = (unsigned char)(Voltage * 100) % 10;
				Seg_Point[5] = 1;
			} else if(Ad_Disp_Choose == 1){//光敏电阻AD转换实际0~4.93V 0XA4
				Voltage = Read_AD_Data(0xA4);//电位器AD转换实际0~4.93V
				Light_Level = (unsigned char)(Voltage * 2);//等级细分0~9级		
				Seg_Buf[0] = 17;
				Seg_Buf[1] = Light_Disp ? 13 : 10;
				Seg_Buf[2] = Light_Disp ? Light_Level : 10;
				Seg_Buf[3] = Light_Disp ? 13 : 10;
				Seg_Buf[4] = 10;
				Seg_Buf[5] = (unsigned char)Voltage % 10;
				Seg_Buf[6] = (unsigned char)(Voltage * 10) % 10;
				Seg_Buf[7] = (unsigned char)(Voltage * 100) % 10;
				Seg_Point[5] = 1;
			} else if(Ad_Disp_Choose == 2){//热敏电阻AD转换实际0~4.93V 0XD4
				Voltage = Read_AD_Data(0xD4);//电位器AD转换实际0~4.93V
				Light_Level = (unsigned char)(Voltage * 2);//等级细分0~9级		
				Seg_Buf[0] = 11;
				Seg_Buf[1] = Light_Disp ? 13 : 10;
				Seg_Buf[2] = Light_Disp ? Light_Level : 10;
				Seg_Buf[3] = Light_Disp ? 13 : 10;
				Seg_Buf[4] = 10;
				Seg_Buf[5] = (unsigned char)Voltage % 10;
				Seg_Buf[6] = (unsigned char)(Voltage * 10) % 10;
				Seg_Buf[7] = (unsigned char)(Voltage * 100) % 10;
				Seg_Point[5] = 1;
			} else{//AIN3位上的AD转换实际0~4.93V 0XE4
				Voltage = Read_AD_Data(0xE4);//电位器AD转换实际0~4.93V
				Light_Level = (unsigned char)(Voltage * 2);//等级细分0~9级		
				Seg_Buf[0] = 15;
				Seg_Buf[1] = Light_Disp ? 13 : 10;
				Seg_Buf[2] = Light_Disp ? Light_Level : 10;
				Seg_Buf[3] = Light_Disp ? 13 : 10;
				Seg_Buf[4] = 10;
				Seg_Buf[5] = (unsigned char)Voltage % 10;
				Seg_Buf[6] = (unsigned char)(Voltage * 10) % 10;
				Seg_Buf[7] = (unsigned char)(Voltage * 100) % 10;
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
				Seg_Point[5] = 0;
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
				Seg_Point[5] = 0;
			}
		break;
	}
}

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
void Init(void){
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
}

void Delay750ms(){		//@12.000MHz
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


void main(){
	unsigned char i,py=0,pl=0;
	LED_Init();
	EEPROM_Read(&Led_Mode_Dat,0,1);	//数据读取必须放在程序最开始
	Delay750ms();										//稍微延时
	EEPROM_Read(&Temp_Max,1,1);			//温度阈值数据读取
	EEPROM_Read(&Temp_Min,2,1);			//温度阈值数据读取
	Set_Clk(uc_Rtc);								//时钟数据初始化
	Voltage = Read_AD_Data(0x94);		//上电先转换电压数据，防止第一次读数据错误
	DS18B20_ConvertT();							//上电先转换一次温度，防止第一次读数据错误
	Delay750ms();										//等待转换完成
	Init();
	while(1){
		Key_Proc();
		if(LED_OR_SEG_OR_BEEP == 1){//数码管位选端与LED因为共用P2端口，P2端口冲突，这里数码管与LED显示分隔，通过按键K1操控。
			Seg_Proc();
			if(Temperature < Temp_Min || Temperature > Temp_Max){
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
			Light_Level = (unsigned char)(Voltage * 2);//等级细分0~9级
			Voltage = Read_AD_Data(0x94);//AD转换实际0~4.93V
			Led_Proc();
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
				if(Led_cnt < Light_Level){
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