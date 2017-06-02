#ifndef __LCD1602_H
#define __LCD1602_H

#include "stm32f10x.h"
#include "delay.h"
#include "stdio.h"

#define COUNT  20

extern unsigned char display1[COUNT];
extern unsigned char display2[COUNT];

void lcd1602_init(void);
void LCD1602_WriteCommand(u8 command);
void LCD1602_WriteData(u8 data);
void lcd1602_show_string(int line, char* str);
void show_data(void);

#endif




