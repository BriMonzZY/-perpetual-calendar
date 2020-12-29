#ifndef _MAIN_H
#define _MAIN_H

enum eStaSystem {  //系统运行状态枚举
    E_NORMAL, E_SET_TIME, E_SET_ALARM, Dis_Alarm
};

#ifndef _MAIN_C
extern enum eStaSystem staSystem;
#endif

void RefreshTemp(uint8 ops);//刷新温度
void Init_Timer0();	//初始化定时器0

#endif