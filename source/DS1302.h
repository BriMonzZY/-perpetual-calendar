#ifndef _DS1302_H
#define _DS1302_H

struct sTime {  //����ʱ��ṹ
    uint16 year; //��
    uint8 mon;   //��
    uint8 day;   //��
    uint8 hour;  //ʱ
    uint8 min;   //��
    uint8 sec;   //��
    uint8 week;  //����
};

void Init_DS1302(); //��ʼ��DS1302��������Ĭ��ʱ��
void GetRealTime(struct sTime *time);  //��ȡDS1302ʱ����������
void SetRealTime(struct sTime *time);  //����DS1302ʱ����������
#endif