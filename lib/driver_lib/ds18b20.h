#ifndef __DS18B20_H
#define __DS18B20_H

#include "sys.h"   

//IO��������
#define DS18B20_IO_IN()  {GPIOA->CRH&=0XFFFF0FFF;GPIOA->CRH|=8<<4;}
#define DS18B20_IO_OUT() {GPIOA->CRH&=0XFFFF0FFF;GPIOA->CRH|=3<<4;}
////IO��������											   
#define	DS18B20_DQ_OUT PAout(4) //���ݶ˿�	PA0 
#define	DS18B20_DQ_IN  PAin(4)  //���ݶ˿�	PA0 
   	
u8 DS18B20_Init(void);//��ʼ��DS18B20

char* DS18B20_Get_Temp_string(void);//��ȡ�¶�
double DS18B20_Get_Temp(void);

void DS18B20_Start(void);//��ʼ�¶�ת��
void DS18B20_Write_Byte(u8 dat);//д��һ���ֽ�
u8 DS18B20_Read_Byte(void);//����һ���ֽ�
u8 DS18B20_Read_Bit(void);//����һ��λ
u8 DS18B20_Check(void);//����Ƿ����DS18B20
void DS18B20_Rst(void);//��λDS18B20 



#endif 
