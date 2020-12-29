#include "config.h"
#include "Lcd1602.h"
#include "DS1302.h"
#include "Key_Buzzer.h"
#include "DS18B20.h"
#include "lunar.h"
#include "main.h"
#include "app.h"

struct sTime CurTime;   //当前日期时间
uint8 SetIndex = 0;  	//设置位索引
bit AlarmOn = 0;  		//闹钟开关志位
uint8 AlarmHour = 0x06; //闹钟时间的小时数，闹钟默认值6:30
uint8 AlarmMin  = 0x30; //闹钟时间的分钟数
uint8 pdata SetDat;		//设置时间日期数据缓存

uint8 code *week[8] = {//星期显示字符表
		"    ", "Mon ", "Tues", "Wed ", "Thur", "Fri ", "Sat ", "Sun "
	};
uint8 code *AlarmSta[2] = {//闹钟开关显示字符表
		"ON ", "OFF"	
	};

/*BCD码转十进制*/
uint8 BCD_DEC(uint8 BCD)
{
	uint8 DEC;
	DEC= BCD / 16;
	BCD %= 16;
	BCD += DEC * 10;

	return BCD;
}
/*十进制转BCD*/
uint8 DEC_BCD(uint8 DEC)
{
	uint8 BCD;
	BCD = DEC / 10;
	DEC %= 10;
	DEC += BCD * 16;

	return DEC;
}

/* 获取当前时间，并刷新显示 */
void RefreshTime()
{
	uint8 pdata time[10];
    GetRealTime(&CurTime); //获取当前日期/时间
	
		time[0] = (CurTime.hour >> 4) + '0';   //小时十位
        time[1] = (CurTime.hour & 0x0f) + '0'; //小时个位
		time[2] = ':';  
		time[3] = (CurTime.min >> 4) + '0';    //分钟十位
        time[4] = (CurTime.min & 0x0f) + '0';  //分钟个位
		time[5] = ':';
		time[6] = (CurTime.sec >> 4) + '0';    //秒钟十位
        time[7] = (CurTime.sec & 0x0f) + '0';  //秒钟个位
		time[8] = '\0';         			   //字符串结束符
		LCD1602_Dis_Str(0, 1, time);  		   //显示到液晶上

} 

/* 日期刷新函数，ops-刷新选项：为0时只当日期变化才刷新，非0则立即刷新 */
void RefreshDate(uint8 ops)
{
    uint8 pdata date[12];
    static uint8 backup = 0;

    if ((backup!=CurTime.day) || (ops!=0))//如果日期变化或刷新标志位置1则立即刷新
    {
		date[0] = ((CurTime.year>>12) & 0x0f) + '0';  //年份千位
        date[1] = ((CurTime.year>>8) & 0x0f) + '0';	  //年份百位
        date[2] = ((CurTime.year>>4) & 0x0f) + '0';	  //年份十位
        date[3] = (CurTime.year & 0x0f) + '0';		  //年份个位
        date[4] = '-';                        		  //分隔符
        date[5] = (CurTime.mon >> 4) + '0';   		  //月份十位
        date[6] = (CurTime.mon & 0x0f) + '0'; 		  //月份个位
        date[7] = '-';                        		  //分隔符
        date[8] = (CurTime.day >> 4) + '0';   		  //日期十位
        date[9] = (CurTime.day & 0x0f) + '0'; 		  //日期个位
        date[10] = '\0';         					  //字符串结束符
        LCD1602_Dis_Str(0, 0, date);  				  //显示到液晶上
		LCD1602_Dis_Str(12, 0, week[CurTime.week]);   //显示星期
        backup = CurTime.day;  						  //刷新上次日期值
    }
}
/*刷新阴历*/
void RefreshLunar()
{
	uint8 pdata date[12];
	GetRealTime(&CurTime); //获取当前日期/时间
	Conversion(0, CurTime.year, CurTime.mon, CurTime.day);//根据公历算出阴历
	date[0] = 0x02 + '0';  //年份千位
	date[1] = 0x00 + '0';	  //年份百位
	date[2] = ((SpDate.year>>4) & 0x0f) + '0';	  //年份十位
	date[3] = (SpDate.year & 0x0f) + '0';		  //年份个位
	date[4] = '-';                        		  //分隔符
	date[5] = (SpDate.mon >> 4) + '0';   		  //月份十位
	date[6] = (SpDate.mon & 0x0f) + '0'; 		  //月份个位
	date[7] = '-';                        		  //分隔符
	date[8] = (SpDate.day >> 4) + '0';   		  //日期十位
	date[9] = (SpDate.day & 0x0f) + '0'; 		  //日期个位
	date[10] = '\0';         					  //字符串结束符
	LCD1602_Dis_Str(0, 0, "Lunar");  			  //阴历字符    
	LCD1602_Dis_Str(6, 0, date);  				  //显示到液晶上 
}

/* 刷新闹钟的显示 */
void RefreshAlarm()
{
    uint8 pdata str[6];
    
    str[0] = (AlarmHour >> 4) + '0';   			//闹钟小时数
    str[1] = (AlarmHour & 0x0f) + '0';
    str[2] = ':';                      			//分隔符
    str[3] = (AlarmMin >> 4) + '0';    			//闹钟分钟数
    str[4] = (AlarmMin & 0x0f) + '0';
    str[5] = '\0';                     			//字符串结束符
	LCD1602_Dis_Str(0, 1, "Alarm ");    		//显示提示标题
	LCD1602_Dis_Str(13, 1, AlarmSta[AlarmOn]);	//显示闹钟状态 
    LCD1602_Dis_Str(6, 1, str);             	//显示到液晶上
}

/* 闹钟监控函数，抵达设定的闹钟时间时执行闹铃 */
void AlarmMonitor()
{
    if ((CurTime.hour == AlarmHour) && (CurTime.min == AlarmMin)) //检查时间匹配
    {
        if (!AlarmOn)      //检查闹钟是否打开
            staBuzzer(1);  //蜂鸣器鸣叫
        else
            staBuzzer(0);  //蜂鸣器停止鸣叫
    }
    else
    {
        staBuzzer(0);  	   //时间不匹配时，蜂鸣器停止鸣叫
    } 
}

/* S2按键功能函数，时间或闹钟设置时，光标移动，到头后折回 */
void SetShift()
{
    if (staSystem == E_SET_TIME)  //设置时间光标
    {
		switch (SetIndex)
        {
            case 0: SetIndex=1;  LCD1602_Set_Cursor(7, 1); break;//光标位于秒钟个位处
            case 1: SetIndex=2;  LCD1602_Set_Cursor(4, 1); break;//光标位于分钟处
            case 2: SetIndex=3;  LCD1602_Set_Cursor(1, 1); break;//光标位于小时处
            case 3: SetIndex=4;  LCD1602_Set_Cursor(12, 0); break;//光标位于星期处
            case 4: SetIndex=5;  LCD1602_Set_Cursor(9, 0); break;//光标位于日处
            case 5: SetIndex=6;  LCD1602_Set_Cursor(6, 0); break;//光标位于月处
            default: SetIndex=0; LCD1602_Set_Cursor(3, 0); break;//光标位于年处
        }
    }
    else if (staSystem == E_SET_ALARM) //设置闹钟光标
    {
        switch (SetIndex)
        {
            case 0: SetIndex=1;  LCD1602_Set_Cursor(13, 1); break;//光标位于闹钟开关处
			case 1: SetIndex=2;  LCD1602_Set_Cursor(10, 1); break;//光标位于闹钟分钟处
            default: SetIndex=0; LCD1602_Set_Cursor(7, 1); break; //光标位于闹钟小时处
        }
    }
	else if (staSystem == E_NORMAL)//正常运行状态下，切换为显示阴历/闹钟
	{
		staSystem = Dis_Alarm;	   //把系统状态更新为显示阴历/闹钟
		LCD1602_Clear_Screen();	   //LCD1602清屏
		RefreshLunar();			   //显示阴历
		RefreshAlarm();			   //显示闹钟
	}
	else if (staSystem == Dis_Alarm)//显示阴历/闹钟状态下，按S2可以切回显示公历和时间
	{
		staSystem = E_NORMAL;	    //把系统状态更新为正常显示
		LCD1602_Clear_Screen();		//LCD1602清屏
		RefreshTime();				//刷新时间
		RefreshDate(1);				//立即刷新日期
		RefreshTemp(1);				//立即刷新温度
	}
}

/* S5按键功能函数，切换系统运行状态 */
void SwitchSystemSta()
{
    if (staSystem == E_NORMAL)  //正常运行切换到时间设置
    {
        staSystem = E_SET_TIME;	//系统状态更新为设置时间
        SetIndex = 0;    		//光标索引设置到秒钟上
        SetShift();		 		//显示光标位置
        LCD1602_Open_Cursor();	//LCD1602打开光标显示
    }
    else if (staSystem == Dis_Alarm)//显示阴历/闹钟时，切换到设置闹钟
    {
        staSystem = E_SET_ALARM;	//系统状态更新为设置闹钟
        SetIndex = 0;    			//光标索引设置到秒钟上
        SetShift();		 			//显示光标位置
		LCD1602_Open_Cursor();		//LCD1602打开光标显示
    }
	else if	(staSystem == E_SET_TIME)//设置公历/时钟的状态下
	{
		staSystem = E_NORMAL;		 //系统状态更新为正常运行，显示公历/时钟、温度。
		SetRealTime(&CurTime);       //把设定时间写入实时时钟
		LCD1602_Close_Cursor();		 //LCD1602关闭光标显示
        LCD1602_Clear_Screen();		 //LCD1602清屏
		RefreshTime();				 //刷新时间
		RefreshDate(1);				 //立即刷新日期
		RefreshTemp(1);				 //立即刷新温度	
	}
    else  							 //设置闹钟的状态下
    {
        staSystem = E_NORMAL;		 //系统状态更新为正常运行，显示公历/时钟、温度。
	    RefreshAlarm();  			 //写入闹钟设定值
        LCD1602_Close_Cursor();		 //LCD1602关闭光标显示
        LCD1602_Clear_Screen();		 //LCD1602清屏
		RefreshTime();			  	 //刷新时间
		RefreshDate(1);				 //立即刷新日期
		RefreshTemp(1);				 //立即刷新温度
    }
}
/*S3按键功能函数，数据加*/
void SetDtate_Alarm_S3()
{
	if (staSystem == E_SET_TIME) //设置公历/时间
    {
		switch (SetIndex)//检查光标位置
        {				
			case 1:	SetDat = BCD_DEC(CurTime.sec);//把BCD码转为十进制
					if (SetDat < 59) SetDat++; else SetDat = 0;//限制设置秒钟不超过59秒，超过则回到0
					CurTime.sec = DEC_BCD(SetDat);//把十进制转为BCD码
					LCD1602_Dis_OneChar(6, 1, (CurTime.sec >> 4) + '0');
					LCD1602_Dis_OneChar(7, 1, (CurTime.sec & 0x0f) + '0');
					LCD1602_Set_Cursor(7, 1);//光标保持原位
					break;

			case 2:	SetDat = BCD_DEC(CurTime.min);//把BCD码转为十进制
					if (SetDat < 59) SetDat++; else SetDat = 0;//限制设置分钟不超过59分，超过则回到0
					CurTime.min = DEC_BCD(SetDat);//把十进制转为BCD码
					LCD1602_Dis_OneChar(3, 1, (CurTime.min >> 4) + '0');
					LCD1602_Dis_OneChar(4, 1, (CurTime.min & 0x0f) + '0');
					LCD1602_Set_Cursor(4, 1);//光标保持原位
					break;

			case 3:	SetDat = BCD_DEC(CurTime.hour);//把BCD码转为十进制
					if (SetDat < 23) SetDat++; else SetDat = 0;//限制设置小时不超过23时，超过则回到0
					CurTime.hour = DEC_BCD(SetDat);//把十进制转为BCD码
					LCD1602_Dis_OneChar(0, 1, (CurTime.hour >> 4) + '0');
					LCD1602_Dis_OneChar(1, 1, (CurTime.hour & 0x0f) + '0');
					LCD1602_Set_Cursor(1, 1);//光标保持原位
					break;

			case 4: //限制不超过星期日，如果超过则回到周一
					if (CurTime.week < 7) CurTime.week++; else CurTime.week = 1;
					LCD1602_Dis_Str(12, 0, week[CurTime.week]);  //显示到液晶上;
					LCD1602_Set_Cursor(12, 0);//光标保持原位
					break;

			case 5:	SetDat = BCD_DEC(CurTime.day);//把BCD码转为十进制
					if (SetDat < 31) SetDat++; else SetDat = 1;//限制设置日期不超过31日，超过则回到1
					CurTime.day = DEC_BCD(SetDat);//把十进制转为BCD码
					LCD1602_Dis_OneChar(8, 0, (CurTime.day >> 4) + '0');
					LCD1602_Dis_OneChar(9, 0, (CurTime.day & 0x0f) + '0');
					LCD1602_Set_Cursor(9, 0);//光标保持原位
					break;

			case 6:	SetDat = BCD_DEC(CurTime.mon);//把BCD码转为十进制
					if (SetDat < 12) SetDat++; else SetDat = 1;//限制设置月份不超过12月，超过则回到1
					CurTime.mon = DEC_BCD(SetDat);//把十进制转为BCD码
					LCD1602_Dis_OneChar(5, 0, (CurTime.mon >> 4) + '0');
					LCD1602_Dis_OneChar(6, 0, (CurTime.mon & 0x0f) + '0');
					LCD1602_Set_Cursor(6, 0);//光标保持原位
					break;

			case 0:	SetDat = BCD_DEC(CurTime.year);//把BCD码转为十进制
					if (SetDat < 99) SetDat++; else SetDat = 0;//限制设置年不超过99年，超过则回到0
					CurTime.year = DEC_BCD(SetDat);//把十进制转为BCD码
					LCD1602_Dis_OneChar(2, 0, (CurTime.year >> 4) + '0');
					LCD1602_Dis_OneChar(3, 0, (CurTime.year & 0x0f) + '0');
					LCD1602_Set_Cursor(3, 0);//光标保持原位
					break;
		}	
	}
	else if (staSystem == E_SET_ALARM)//设置闹钟
	{
		switch (SetIndex)//检查光标位置
        {
			case 1:	AlarmOn = ~AlarmOn;
					LCD1602_Dis_Str(13, 1, AlarmSta[AlarmOn]);//显示闹钟状态 
					LCD1602_Set_Cursor(13, 1);//光标保持原位
					break;
								
			case 2:	SetDat = BCD_DEC(AlarmMin);//把BCD码转为十进制
					if (SetDat < 59) SetDat++; else SetDat = 0;//限制设置分钟不超过59分，超过则回到0
					AlarmMin = DEC_BCD(SetDat);//把十进制转为BCD码
					LCD1602_Dis_OneChar(9, 1, (AlarmMin >> 4) + '0');
					LCD1602_Dis_OneChar(10, 1, (AlarmMin & 0x0f) + '0');
					LCD1602_Set_Cursor(10, 1);//光标保持原位
					break;

			case 0: SetDat = BCD_DEC(AlarmHour);//把BCD码转为十进制
					if (SetDat < 23) SetDat++; else SetDat = 0;//限制设置小时不超过23时，超过则回到0
					AlarmHour = DEC_BCD(SetDat);//把十进制转为BCD码 
					LCD1602_Dis_OneChar(6, 1, (AlarmHour >> 4) + '0');
					LCD1602_Dis_OneChar(7, 1, (AlarmHour & 0x0f) + '0');
					LCD1602_Set_Cursor(7, 1);//光标保持原位
					break;
		}		
	}	
}
/*S4按键功能函数，数据减*/
void SetDtate_Alarm_S4()
{
	if (staSystem == E_SET_TIME) //设置公历/时间
    {
		switch (SetIndex) //检查光标位置
        {				
			case 1:	SetDat = BCD_DEC(CurTime.sec);//把BCD码转为十进制
					if (SetDat != 0) SetDat--; else SetDat = 59;//限制设置秒钟不为0时减1，为0时回到59
					CurTime.sec = DEC_BCD(SetDat);//把十进制转为BCD码
					LCD1602_Dis_OneChar(6, 1, (CurTime.sec >> 4) + '0');
					LCD1602_Dis_OneChar(7, 1, (CurTime.sec & 0x0f) + '0');
					LCD1602_Set_Cursor(7, 1);//光标保持原位
					break;

			case 2:	SetDat = BCD_DEC(CurTime.min);//把BCD码转为十进制
					if (SetDat != 0) SetDat--; else SetDat = 59;//限制设置分钟不为0时减1，为0时回到59
					CurTime.min = DEC_BCD(SetDat);//把十进制转为BCD码
					LCD1602_Dis_OneChar(3, 1, (CurTime.min >> 4) + '0');
					LCD1602_Dis_OneChar(4, 1, (CurTime.min & 0x0f) + '0');
					LCD1602_Set_Cursor(4, 1);//光标保持原位
					break;

			case 3:	SetDat = BCD_DEC(CurTime.hour);//把BCD码转为十进制
					if (SetDat != 0) SetDat--; else SetDat = 23;//限制设置小时不为0时减1，为0时回到23
					CurTime.hour = DEC_BCD(SetDat);//把十进制转为BCD码
					LCD1602_Dis_OneChar(0, 1, (CurTime.hour >> 4) + '0');
					LCD1602_Dis_OneChar(1, 1, (CurTime.hour & 0x0f) + '0');
					LCD1602_Set_Cursor(1, 1);//光标保持原位
					break;

			case 4: //如果为当前为星期一时，回到星期天
					if (CurTime.week != 1) CurTime.week--; else CurTime.week = 7;
					LCD1602_Dis_Str(12, 0, week[CurTime.week]);  //显示到液晶上;
					LCD1602_Set_Cursor(12, 0);//光标保持原位
					break;

			case 5:	SetDat = BCD_DEC(CurTime.day);//把BCD码转为十进制
					if (SetDat != 1) SetDat--; else SetDat = 31;//限制设置日期不为1时减1，为0时回到31
					CurTime.day = DEC_BCD(SetDat);//把十进制转为BCD码
					LCD1602_Dis_OneChar(8, 0, (CurTime.day >> 4) + '0');
					LCD1602_Dis_OneChar(9, 0, (CurTime.day & 0x0f) + '0');
					LCD1602_Set_Cursor(9, 0);//光标保持原位
					break;

			case 6:	SetDat = BCD_DEC(CurTime.mon);//把BCD码转为十进制
					if (SetDat != 1) SetDat--; else SetDat = 12;//限制设置月份不为1时减1，为0时回到12
					CurTime.mon = DEC_BCD(SetDat);//把十进制转为BCD码
					LCD1602_Dis_OneChar(5, 0, (CurTime.mon >> 4) + '0');
					LCD1602_Dis_OneChar(6, 0, (CurTime.mon & 0x0f) + '0');
					LCD1602_Set_Cursor(6, 0);//光标保持原位
					break;

			case 0:	SetDat = BCD_DEC(CurTime.year);//把BCD码转为十进制
					if (SetDat != 0) SetDat--; else SetDat = 99;//限制设置年不为0时减1，为0时回到99
					CurTime.year = DEC_BCD(SetDat);//把十进制转为BCD码
					LCD1602_Dis_OneChar(2, 0, (CurTime.year >> 4) + '0');
					LCD1602_Dis_OneChar(3, 0, (CurTime.year & 0x0f) + '0');
					LCD1602_Set_Cursor(3, 0);//光标保持原位
					break;
		}		
	}
	else if (staSystem == E_SET_ALARM) //设置闹钟
	{
		switch (SetIndex) //检查光标位置
        {
			case 1:	AlarmOn = ~AlarmOn;
					LCD1602_Dis_Str(13, 1, AlarmSta[AlarmOn]);//显示闹钟状态 
					LCD1602_Set_Cursor(13, 1);//光标保持原位
					break;

			case 2:	SetDat = BCD_DEC(AlarmMin);//把BCD码转为十进制
					if (SetDat != 0) SetDat--; else SetDat = 59;//限制设置分钟不为0时减1，为0时回到59
					AlarmMin = DEC_BCD(SetDat);//把十进制转为BCD码 
					LCD1602_Dis_OneChar(9, 1, (AlarmMin >> 4) + '0');
					LCD1602_Dis_OneChar(10, 1, (AlarmMin & 0x0f) + '0');
					LCD1602_Set_Cursor(10, 1);//光标保持原位
					break;

			case 0: SetDat = BCD_DEC(AlarmHour);//把BCD码转为十进制
					if (SetDat != 0) SetDat--; else SetDat = 23;//限制设置小时不为0时减1，为0时回到59
					AlarmHour = DEC_BCD(SetDat);//把十进制转为BCD码
					LCD1602_Dis_OneChar(6, 1, (AlarmHour >> 4) + '0');
					LCD1602_Dis_OneChar(7, 1, (AlarmHour & 0x0f) + '0');
					LCD1602_Set_Cursor(7, 1);//光标保持原位
					break;
		}		
	}	
}


/*按键驱动*/
void KeyAction(uint8 keycode)
{
	switch (keycode)
	{
		case 0x0d: SetShift();          break;	//按下S2,显示切换键，在设置状态下为设置左移键
        case 0x26: SetDtate_Alarm_S3(); break;	//按下S3,数据加
        case 0x28: SetDtate_Alarm_S4(); break;	//按下S4,数据减
        case 0x1b: SwitchSystemSta();   break;	//按下S5,设置键
		default: break;
	}
}