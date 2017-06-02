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
void delay_ms(u16 nms);  // �����е�540,�ǿ��ǵ�ĳЩ�ͻ����ܳ�Ƶʹ��, 
						 //���糬Ƶ��248M��ʱ��,delay_xms���ֻ����ʱ541ms������

#endif

























