#include "config.h"
#include "DS18B20.h" 

/*us延时函数，执行一次US--所需6.5us进入一次函数需要11.95us*/
void Delay_us(uint8 us)
{
	while(us--);	
}

/*初始化DS18B20,并获取存在脉冲*/
bit GetDS18B20Ack()
{
	bit ack;
	EA = 0;   //禁止总中断
	DS18B20_IO = 1;
	_nop_();
	DS18B20_IO = 0;
	Delay_us(75); //拉低总线499.45us 挂接在总线上的18B20将会全部被复位
	DS18B20_IO = 1; //释放总线
	Delay_us(11); //延时83.45us 等待18B20发回存在信号
	ack = DS18B20_IO;
	while(!DS18B20_IO); //等待存在脉冲结束
	_nop_();
	EA = 1;   //使能总中断

	return (ack);
}

/*单总线写一个字节*/
void write_byte(uint8 dat)
{
	uint8 i;
	EA = 0;   //禁止总中断
	for(i=0;i<8;i++)
	{
		DS18B20_IO = 0;
		_nop_();//产生些时序
		DS18B20_IO = dat & 0x01;
		Delay_us(10);//76.95us
		DS18B20_IO = 1; //释放总线准备下一次数据写入
		_nop_();
		dat >>= 1;
	}
	EA = 1;   //使能总中断
}
/*单总线读一个字节*/
uint8 read_byte()
{
	uint8 i, j, dat;
	EA = 0;   //禁止总中断
	for(i=0;i<8;i++)
	{
		DS18B20_IO = 0;
		_nop_();//产生读时序
		DS18B20_IO = 1;
		_nop_();//释放总线
		j = DS18B20_IO;
		Delay_us(10);//76.95us
		DS18B20_IO = 1;
		_nop_();
		dat = (j<<7)|(dat>>1);	
	}
	EA = 1;   //使能总中断
	return (dat);
}

/* 启动一次18B20温度转换，返回值-表示是否启动成功 */
bit Start18B20()
{
    bit ack;
    
    ack = GetDS18B20Ack();   //执行总线复位，并获取18B20应答
    if (ack == 0)          //如18B20正确应答，则启动一次转换
    {
        write_byte(0xCC);  //跳过ROM操作
        write_byte(0x44);  //启动一次温度转换
    }
    return ~ack;   //ack==0表示操作成功，所以返回值对其取反
}

/* 读取DS18B20转换的温度值，返回值-表示是否读取成功 */
bit Get18B20Temp(int16 *temp)
{
    bit ack;
    uint8 LSB, MSB; //16bit温度值的低字节和高字节
    
    ack = GetDS18B20Ack();    //执行总线复位，并获取18B20应答
    if (ack == 0)           //如18B20正确应答，则读取温度值
    {
        write_byte(0xCC);   //跳过ROM操作
        write_byte(0xBE);   //发送读命令
        LSB = read_byte();  //读温度值的低字节
        MSB = read_byte();  //读温度值的高字节
        *temp = ((int16)MSB << 8) + LSB;  //合成为16bit整型数
    }
	return ~ack;  //ack==0表示操作应答，所以返回值为其取反值
}