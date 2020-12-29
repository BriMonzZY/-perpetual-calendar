#ifndef _APP_H
#define _APP_H

void RefreshTime();//刷新时间数据并显示
void RefreshDate(uint8 ops); //刷新日期数据并显示
void KeyAction(uint8 keycode);//按键驱动
void AlarmMonitor(); //监控闹钟
void RefreshLunar(); //刷新阴历
#endif