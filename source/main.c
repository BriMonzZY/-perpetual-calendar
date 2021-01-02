/*****************************************
* 【程序功能】： 电子万年历,LCD1602液晶显示，显示时分秒、星期、公历、阴历、闹钟、环境温度、可以校时和设置闹钟。			   			            			    
* 【使用说明】： 使用3条杜邦线把P10接到DS1302的TSCLK上，P11接到TIO，P12接到TRST上。
				 接上LCD1602液晶。
* 【按键功能说明】：在正常显示状态下S2按键为切换公历、时钟/阴历、闹钟显示，在设置状态下S2为设置左移。
					S5为设置键，在显示公历时钟状态下，按下S5为设置公历、时钟，再次按下S5为写入设置参数并退出
					在显示阴历、闹钟状态下，按下S5为设置阴历、闹钟，再次按下S5为写入设置参数并退出
					在设置状态下，S3为数据加
					在设置状态下，S4为数据减
******************************************/
#define _MAIN_C
#include "config.h"
#include "LCD1602.h"
#include "DS1302.h"
#include "DS18B20.h"
#include "app.h"
#include "Key_Buzzer.h"
#include "main.h"

bit flags = 0;	//1s定时标志位
bit flag200ms = 0;
enum eStaSystem staSystem = E_NORMAL;	//系统运行状态

void main()
{
	EA = 1;
	Start18B20();
	Init_Timer0();
	Init_LCD1602();
	Init_DS1302();
	while(!flags);	//上电延迟1s
	flags = 0;
	RefreshTime();
	RefreshDate(1);	//立即刷新日期
	RefreshTemp(1);
	
	while(1) {
		KeyDriver();	//按键检测
		if(flag200ms) {
			flag200ms = 0;
			AlarmMonitor();
			if(staSystem == E_NORMAL) {	//如果系统正常运行
				RefreshTime();
				RefreshDate(0);	//刷新日期
			}
			if(staSystem == Dis_Alarm) {
				RefreshLunar();
			}
		}
		if(flags && (staSystem == E_NORMAL)) {
			flags = 0;
			RefreshTemp(0);
		}
	}
}

/* 温度刷新函数，读取当前温度并根据需要刷新液晶显示，
** ops-刷新选项：为0时只当温度变化才刷新，非0则立即刷新 */
void RefreshTemp(uint8 ops) {
	int16 temp;
	uint8 pdata str[8];
	static int16 backup = 0;
	
	Get18B20Temp(&temp);
	Start18B20();
	temp >>= 4;	//舍弃4bit小数位
	if((backup != temp) || (ops != 0)) {
		str[0] = (temp / 10) + '0';
		str[1]= (temp % 10) + '0';
		str[2] = '\'';             //用'C代替℃
		str[3] = 'C';
		str[4] = '\0';             //字符串结束符
		LCD1602_Dis_Str(12, 1, str);    //显示到液晶上
		backup = temp; 
	}
}

void Init_Timer0() {
	TMOD = 0x01;
	TH0 = 0xee;
	TL0 = 0x00;	//5ms
	ET0 = 1;
	TR0 = 1;
}

void Timer0() interrupt 1
{
	static uint8 tmrls = 0;
	static uint8 tmr200ms = 0;
	
	TH0 = 0xee;
	TL0 = 0x00;
	tmr200ms++;
	if(tmr200ms >= 40) {
		tmr200ms = 0;
		flag200ms = 1;
		tmrls ++;
		if(tmrls >= 5) {
			tmrls = 0;
			flags = 1;
		}
	}
	KeyScan();
}