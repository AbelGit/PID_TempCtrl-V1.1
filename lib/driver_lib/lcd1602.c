
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "LCD1602.h"
#include "driver.h"



unsigned char display1[COUNT];
unsigned char display2[COUNT];


void LCD1602_WriteCommand(u8 cmd)
{
	GPIO_ResetBits(LCD1602_PORT1,LCD1602_RW_PIN); //RW 
	GPIO_ResetBits(LCD1602_PORT1,LCD1602_RS_PIN); //RS
	delay_ms(5); 
	GPIO_SetBits(LCD1602_PORT1,LCD1602_E_PIN); //EN
	delay_ms(5); 
	LCD1602_PORT1->BSRR = cmd<<8 & 0xff00;  //将数据送到P0口 
	LCD1602_PORT1->BRR = ((~cmd)<<8) & 0xff00;
	delay_ms(5);  
	GPIO_ResetBits(LCD1602_PORT1,LCD1602_E_PIN); //pin7
}

void LCD1602_WriteData(u8 data)
{
	GPIO_ResetBits(LCD1602_PORT1,LCD1602_RW_PIN); //RW 
	GPIO_SetBits(LCD1602_PORT1,LCD1602_RS_PIN); //RS 
	delay_ms(5); 

	GPIO_SetBits(LCD1602_PORT1,LCD1602_E_PIN); //EN
	delay_ms(5); 
	LCD1602_PORT1->BSRR = data<<8 & 0xff00;    ////P0 = dat
	LCD1602_PORT1->BRR = ((~data)<<8) & 0xff00; 
	delay_ms(5);  
	GPIO_ResetBits(LCD1602_PORT1,LCD1602_E_PIN);
}


void lcd1602_init (void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(LCD1602_RCC_PORT1, ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_InitStructure.GPIO_Pin = LCD1602_RS_PIN| LCD1602_RW_PIN | LCD1602_E_PIN| LCD1602_DB0_PIN| 
								  LCD1602_DB1_PIN| LCD1602_DB2_PIN| LCD1602_DB3_PIN| LCD1602_DB4_PIN|
								  LCD1602_DB5_PIN| LCD1602_DB6_PIN| LCD1602_DB7_PIN;
	GPIO_Init(LCD1602_PORT1, &GPIO_InitStructure);	

	LCD1602_WriteCommand(0x38);		// Working mode setting
	LCD1602_WriteCommand(0x0C);		// Show switch setting, picture show or twinkle
	LCD1602_WriteCommand(0x06);		// Input method , if rolling
	LCD1602_WriteCommand(0x01);		// Clear screen		
}
	

void lcd1602_show_string(int line, char* str)
{
	if(line == 1)
	{
		LCD1602_WriteCommand(0x80);
	}else if(line == 2)
	{
		LCD1602_WriteCommand(0x80 + 0x40);
	}
	
	while(*str != '\0')
	{
		LCD1602_WriteData(*str++);	
	}
}

void show_data(void)
{
	char pid_flag = '0';
	
	if(pid.flag == 0)
	{
		pid_flag = '+';
	}else
	{
		pid_flag = '-';
	}
	
	//sprintf(temp_buf, "T:%1.1f |%d,%d| %d", temp.curret_value, (int)temp.max_value, (int)temp.min_value, pid_data_change);
	sprintf(buffer.temp_buf, "T:%1.2f |%1.1f| %c", temp.curret_value, temp.target, pid_flag);
	sprintf(buffer.pid_buf, "P:%1.1f|%1.2f|%1.2f", pid.p, pid.i, pid.d);
	lcd1602_show_string(1, buffer.temp_buf);
	lcd1602_show_string(2, buffer.pid_buf);
}

