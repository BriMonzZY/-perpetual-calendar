#include "config.h"
#include "app.h"
#include "Key_Buzzer.h"

uint8 code KeyCodeMap[4] = { //4位独立按键到标注按键的映射表
	0x0d,//回车键
	0x26,//上键
	0x28,//下键
	0x1b //ESC键
};

uint8 pdata KeySta[4] = {	//4位独立按键当前状态
  1, 1, 1, 1
};

void KeyDriver()
{
	uint8 i;
	static uint8 pdata backup[4] = {	//4位独立按键备份值
  		1, 1, 1, 1
	};
	for (i=0; i<4; i++)//循环检测4个独立按键
	{
		if (backup[i] != KeySta[i])//检测按键
		{
			if(backup[i] != 0) //如果按键按下
			{
				KeyAction(KeyCodeMap[i]); //调用按键动作函数
			}
			backup[i] =	KeySta[i];//刷新备份值
		}
	}	
}

/* 按键扫描函数，需在定时中断中调用，间隔4ms*/
void KeyScan()
{
	uint8 i;
	static uint8 flag = 0; //消抖计数
	static uint8 keybuf[4] = { //4位独立按键扫描缓冲区
		0xff, 0xff, 0xff, 0xff
	};

	//将4个独立按键值移入缓冲区
    keybuf[0] = (keybuf[0] << 1) | KEY_S2;
    keybuf[1] = (keybuf[1] << 1) | KEY_S3;
    keybuf[2] = (keybuf[2] << 1) | KEY_S4;
    keybuf[3] = (keybuf[3] << 1) | KEY_S5;
	
	flag++;	//间隔5ms扫描一次
	if(flag == 4)//4次就是20ms 完成消抖
	{
		flag = 0;//扫描次数清零
		for (i=0; i<4; i++)  //读取4个独立的值
		{
			if ((keybuf[i] & 0x0f) == 0x00)
				{
					KeySta[i] = 0;//如果4次扫描的值都为0，即按下状态
				}
				else if	((keybuf[i] & 0x0f) == 0x0f)
				{
					KeySta[i] = 1;//如果4次扫描的值都为1，即弹起状态	
				}
		}	
	}
			
}

/*蜂鸣器控制*/
void staBuzzer(bit e)
{
	if (e  == 1)
		BUZZER = 0;
	else
		BUZZER = 1;	
}