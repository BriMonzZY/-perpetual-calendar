#ifndef _LUNAR_H
#define _LUNAR_H

struct SPDATE {  //�����ṹ��
    uint8 year;  //��
    uint8 mon;   //��
    uint8 day;   //��
};

#ifndef _LUNAR_C
extern struct SPDATE SpDate;//����
#endif
/*
��������:����BCD��������,���BCD��������(ֻ����1901-2099��)
���ú���ʾ��:Conversion(c_sun,year_sun,month_sun,day_sun)
��:����2004��10��16��Conversion(0,0x4,0x10,0x16);
c_sun,year_sun,month_sun,day_sun��ΪBCD����,c_sunΪ���ͱ�־λ,c_sun=0Ϊ21��
��,c_sun=1Ϊ19����
���ú�����,ԭ�����ݲ���,��c_moon,year_moon,month_moon,day_moon�ó�����BCD����
*/
void Conversion(bit c,uchar year,uchar month,uchar day);
#endif