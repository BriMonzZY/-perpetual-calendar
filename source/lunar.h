#ifndef _LUNAR_H
#define _LUNAR_H

struct SPDATE {  //阴历结构体
    uint8 year;  //年
    uint8 mon;   //月
    uint8 day;   //日
};

#ifndef _LUNAR_C
extern struct SPDATE SpDate;//阴历
#endif
/*
函数功能:输入BCD阳历数据,输出BCD阴历数据(只允许1901-2099年)
调用函数示例:Conversion(c_sun,year_sun,month_sun,day_sun)
如:计算2004年10月16日Conversion(0,0x4,0x10,0x16);
c_sun,year_sun,month_sun,day_sun均为BCD数据,c_sun为世纪标志位,c_sun=0为21世
纪,c_sun=1为19世纪
调用函数后,原有数据不变,读c_moon,year_moon,month_moon,day_moon得出阴历BCD数据
*/
void Conversion(bit c,uchar year,uchar month,uchar day);
#endif