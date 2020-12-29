#include "config.h"
#include "DS18B20.h" 

/*us��ʱ������ִ��һ��US--����6.5us����һ�κ�����Ҫ11.95us*/
void Delay_us(uint8 us)
{
	while(us--);	
}

/*��ʼ��DS18B20,����ȡ��������*/
bit GetDS18B20Ack()
{
	bit ack;
	EA = 0;   //��ֹ���ж�
	DS18B20_IO = 1;
	_nop_();
	DS18B20_IO = 0;
	Delay_us(75); //��������499.45us �ҽ��������ϵ�18B20����ȫ������λ
	DS18B20_IO = 1; //�ͷ�����
	Delay_us(11); //��ʱ83.45us �ȴ�18B20���ش����ź�
	ack = DS18B20_IO;
	while(!DS18B20_IO); //�ȴ������������
	_nop_();
	EA = 1;   //ʹ�����ж�

	return (ack);
}

/*������дһ���ֽ�*/
void write_byte(uint8 dat)
{
	uint8 i;
	EA = 0;   //��ֹ���ж�
	for(i=0;i<8;i++)
	{
		DS18B20_IO = 0;
		_nop_();//����Щʱ��
		DS18B20_IO = dat & 0x01;
		Delay_us(10);//76.95us
		DS18B20_IO = 1; //�ͷ�����׼����һ������д��
		_nop_();
		dat >>= 1;
	}
	EA = 1;   //ʹ�����ж�
}
/*�����߶�һ���ֽ�*/
uint8 read_byte()
{
	uint8 i, j, dat;
	EA = 0;   //��ֹ���ж�
	for(i=0;i<8;i++)
	{
		DS18B20_IO = 0;
		_nop_();//������ʱ��
		DS18B20_IO = 1;
		_nop_();//�ͷ�����
		j = DS18B20_IO;
		Delay_us(10);//76.95us
		DS18B20_IO = 1;
		_nop_();
		dat = (j<<7)|(dat>>1);	
	}
	EA = 1;   //ʹ�����ж�
	return (dat);
}

/* ����һ��18B20�¶�ת��������ֵ-��ʾ�Ƿ������ɹ� */
bit Start18B20()
{
    bit ack;
    
    ack = GetDS18B20Ack();   //ִ�����߸�λ������ȡ18B20Ӧ��
    if (ack == 0)          //��18B20��ȷӦ��������һ��ת��
    {
        write_byte(0xCC);  //����ROM����
        write_byte(0x44);  //����һ���¶�ת��
    }
    return ~ack;   //ack==0��ʾ�����ɹ������Է���ֵ����ȡ��
}

/* ��ȡDS18B20ת�����¶�ֵ������ֵ-��ʾ�Ƿ��ȡ�ɹ� */
bit Get18B20Temp(int16 *temp)
{
    bit ack;
    uint8 LSB, MSB; //16bit�¶�ֵ�ĵ��ֽں͸��ֽ�
    
    ack = GetDS18B20Ack();    //ִ�����߸�λ������ȡ18B20Ӧ��
    if (ack == 0)           //��18B20��ȷӦ�����ȡ�¶�ֵ
    {
        write_byte(0xCC);   //����ROM����
        write_byte(0xBE);   //���Ͷ�����
        LSB = read_byte();  //���¶�ֵ�ĵ��ֽ�
        MSB = read_byte();  //���¶�ֵ�ĸ��ֽ�
        *temp = ((int16)MSB << 8) + LSB;  //�ϳ�Ϊ16bit������
    }
	return ~ack;  //ack==0��ʾ����Ӧ�����Է���ֵΪ��ȡ��ֵ
}