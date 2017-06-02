/**
  ******************************************************************************
  * @file    main.c
  * @author  阿w 
  * @version V1.0
  * @date    2017/3/26
  * @attention    http://www.chuxue123.com   http://www.openedv.com  
  * 
  @brief  
	//位置式pid算法 
	1. 计算周期500ms，最大控制时间200ms，每隔1ms执行机构控制一次，
	2. TIM2刷新界面50ms刷一次，TIM3每隔1ms中断一次，控制一次。
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
1.增量式pid和位置式pid的切换需要改三处
	1.main.c   2.driver.c 中的配置参数  3.stm32f10x_it.c 中的process_case2()函数
*/

int main(void)
{
	driver_init();
	data_init(26.0);   //此处设定目标温度值
	
	while(1)
	{	
//		pid_control(BIT_CONTROL);
//		pid_control(HYS_BIT_CONTROL);
		pid_control(POSITION_PID_CONTROL);
//		pid_control(INCREASE_PID_CONTROL);
	}
}
























