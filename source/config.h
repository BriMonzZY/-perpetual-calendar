#ifndef _CONFIG_H
#define _CONFIG_H
/****************************************************************
 通用头文件
****************************************************************/
#include <reg52.h>
#include <intrins.h>

#define MAIN_Fosc		11059200UL	//宏定义主时钟HZ

/****************************************************************
已有数据类型重新定义
****************************************************************/
typedef signed char int8;  //8位有符号型
typedef signed int  int16; //16位有符号型
typedef unsigned char uint8;  //8位无符号型
typedef unsigned char uchar;  //8位无符号型
typedef unsigned int  uint16; //16位无符号型
typedef unsigned int  uint; //16位无符号型
typedef unsigned long uint32; //32位无符号型

/****************************************************************
 硬件接口定义
****************************************************************/
/*DS1302IO接口 请用3P杜邦线与对应的P1口链接*/
sbit TSCLK = P1^0;   //DS1302时钟引脚
sbit TIO   = P1^1;   //DS1302数据引脚
sbit TRST  = P1^2;   //DS1302使能引脚
/*4位独立按键引脚*/
sbit KEY_S2 = P3^0;	//S2独立按键
sbit KEY_S3 = P3^1;	//S3独立按键
sbit KEY_S4 = P3^2;	//S4独立按键
sbit KEY_S5 = P3^3;	//S5独立按键
/*LCD1602液晶引脚*/
#define LCD1602_DB  P0      //data bus 数据总线
sbit LCD1602_RS = P3^5;		//LCD1602液晶命令/数据选择引脚
sbit LCD1602_RW = P3^6;		//LCD1602液晶读/写选择引脚
sbit LCD1602_EN = P3^4;	 	//LCD1602液晶使能引脚
/*DS18B20温度传感器引脚*/
sbit DS18B20_IO = P2^2;   //DS18B20通信引脚
/*5V有源蜂鸣器引脚*/
sbit BUZZER = P2^3;	//蜂鸣器引脚

#endif