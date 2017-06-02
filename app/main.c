/**
  ******************************************************************************
  * @file    main.c
  * @author  ��w 
  * @version V1.0
  * @date    2017/3/26
  * @attention    http://www.chuxue123.com   http://www.openedv.com  
  * 
  @brief  
	//λ��ʽpid�㷨 
	1. ��������500ms��������ʱ��200ms��ÿ��1msִ�л�������һ�Σ�
	2. TIM2ˢ�½���50msˢһ�Σ�TIM3ÿ��1ms�ж�һ�Σ�����һ�Ρ�
	3. 
  *****************************************************************************/ 
#include <stdio.h>
#include <stdlib.h>
#include "stm32f10x.h"
#include "stm32f10x_tim.h"
#include "driver.h"
#include "delay.h"
#include "lcd1602.h"
#include "ds18b20.h"
#include "pid.h"
#include "relay.h"

/*
1.����ʽpid��λ��ʽpid���л���Ҫ������
	1.main.c   2.driver.c �е����ò���  3.stm32f10x_it.c �е�process_case2()����
*/

int main(void)
{
	driver_init();
	data_init(26.0);   //�˴��趨Ŀ���¶�ֵ
	
	while(1)
	{	
//		pid_control(BIT_CONTROL);
//		pid_control(HYS_BIT_CONTROL);
		pid_control(POSITION_PID_CONTROL);
//		pid_control(INCREASE_PID_CONTROL);
	}
}
























