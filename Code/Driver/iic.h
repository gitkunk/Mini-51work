//头文件声明
#include <REGX52.H>
#include <intrins.h>

//必要延时
#define DELAY_TIME	5

//定义总线

sbit scl = P2^1;
sbit sda = P2^0;

void I2CStart(void);
static void I2C_Delay(unsigned char n);
void I2CStop(void);
void I2CSendByte(unsigned char byt);
unsigned char I2CReceiveByte(void);
unsigned char I2CWaitAck(void);
void I2CSendAck(unsigned char ackbit);

unsigned char Ad_Write(unsigned char addr);
void Da_Write(unsigned char dat);

void EEPROM_Write(unsigned char* String,unsigned char addr,unsigned char num);
void EEPROM_Read(unsigned char* String,unsigned char addr,unsigned char num);