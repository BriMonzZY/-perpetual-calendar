#ifndef _MAIN_H
#define _MAIN_H

enum eStaSystem {  //ϵͳ����״̬ö��
    E_NORMAL, E_SET_TIME, E_SET_ALARM, Dis_Alarm
};

#ifndef _MAIN_C
extern enum eStaSystem staSystem;
#endif

void RefreshTemp(uint8 ops);//ˢ���¶�
void Init_Timer0();	//��ʼ����ʱ��0

#endif