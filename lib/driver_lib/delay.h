/**
  ******************************************************************************
  * @file    delay.h
  * @author  abel li 
  * @version V1.0
  * @date    2017/1/11
  * @attention    http://www.chuxue123.com   http://www.openedv.com  
  * @brief  
	1. 
	2. 
	3. 
  *****************************************************************************/ 
#ifndef __DELAY_H
#define __DELAY_H 		
  
#include "stm32f10x.h"

void delay_init(u8 SYSCLK);
void delay_us(u32 nus);
void delay_xms(u16 nms);
void delay_ms(u16 nms);  // 函数中的540,是考虑到某些客户可能超频使用, 
						 //比如超频到248M的时候,delay_xms最大只能延时541ms左右了

#endif

























