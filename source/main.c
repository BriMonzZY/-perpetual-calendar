/*****************************************
* �������ܡ��� ����������,LCD1602Һ����ʾ����ʾʱ���롢���ڡ����������������ӡ������¶ȡ�����Уʱ���������ӡ�			   			            			    
* ��ʹ��˵������ ʹ��3���Ű��߰�P10�ӵ�DS1302��TSCLK�ϣ�P11�ӵ�TIO��P12�ӵ�TRST�ϡ�
				 ����LCD1602Һ����
* ����������˵��������������ʾ״̬��S2����Ϊ�л�������ʱ��/������������ʾ��������״̬��S2Ϊ�������ơ�
					S5Ϊ���ü�������ʾ����ʱ��״̬�£�����S5Ϊ���ù�����ʱ�ӣ��ٴΰ���S5Ϊд�����ò������˳�
					����ʾ����������״̬�£�����S5Ϊ�������������ӣ��ٴΰ���S5Ϊд�����ò������˳�
					������״̬�£�S3Ϊ���ݼ�
					������״̬�£�S4Ϊ���ݼ�
******************************************/
#define _MAIN_C
#include "config.h"
#include "LCD1602.h"
#include "DS1302.h"
#include "DS18B20.h"
#include "app.h"
#include "Key_Buzzer.h"
#include "main.h"

bit flags = 0;	//1s��ʱ��־λ
bit flag200ms = 0;
enum eStaSystem staSystem = E_NORMAL;	//ϵͳ����״̬

void main()
{
	EA = 1;
	Start18B20();
	Init_Timer0();
	Init_LCD1602();
	Init_DS1302();
	while(!flags);	//�ϵ��ӳ�1s
	flags = 0;
	RefreshTime();
	RefreshDate(1);	//����ˢ������
	RefreshTemp(1);
	
	while(1) {
		KeyDriver();	//�������
		if(flag200ms) {
			flag200ms = 0;
			AlarmMonitor();
			if(staSystem == E_NORMAL) {	//���ϵͳ��������
				RefreshTime();
				RefreshDate(0);	//ˢ������
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

/* �¶�ˢ�º�������ȡ��ǰ�¶Ȳ�������Ҫˢ��Һ����ʾ��
** ops-ˢ��ѡ�Ϊ0ʱֻ���¶ȱ仯��ˢ�£���0������ˢ�� */
void RefreshTemp(uint8 ops) {
	int16 temp;
	uint8 pdata str[8];
	static int16 backup = 0;
	
	Get18B20Temp(&temp);
	Start18B20();
	temp >>= 4;	//����4bitС��λ
	if((backup != temp) || (ops != 0)) {
		str[0] = (temp / 10) + '0';
		str[1]= (temp % 10) + '0';
		str[2] = '\'';             //��'C�����
		str[3] = 'C';
		str[4] = '\0';             //�ַ���������
		LCD1602_Dis_Str(12, 1, str);    //��ʾ��Һ����
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