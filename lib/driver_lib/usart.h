/**
  ******************************************************************************
  * @file    usart.h
  * @author  abel li 
  * @version V1.0
  * @date    2017/1/11
  * @attention    http://www.chuxue123.com   http://www.openedv.com  
  * @brief  
	1. 
	2. 
	3. 
  *****************************************************************************/ 

#ifndef __USART_H
#define __USART_H


#include <stdio.h>

#include "stm32f10x.h"
#include "sys.h"


#define USART_REC_LEN  			200  	//�����������ֽ��� 200
#define EN_USART1_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����
	  	
extern u8  USART_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART_RX_STA;         		//����״̬���	

//����봮���жϽ��գ��벻Ҫע�����º궨��
void usart_init(u32 baud);             	

#endif


