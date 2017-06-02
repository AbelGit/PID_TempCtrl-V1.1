
#include <stdio.h>
#include <string.h>

#include "stm32f10x_it.h"
#include "stm32f10x.h"
#include "usart.h"
#include "lcd1602.h"
#include "key_exti.h"
#include "driver.h"
#include "ds18b20.h"
#include "led.h"
#include "timer.h"
#include "beep.h"
#include "relay.h"

_TEMP temp;
_PID pid;
_BUFF buffer;



void driver_init(void)
{
	Project_Nvic_Init();
	delay_init(72);
	usart_init(115200);
	lcd1602_init();
	exti_key_init();
	DS18B20_Init();
	led_gpio_init();
	tim2_init();
	tim3_init();
	beep_init();
	relay_init();
}

void data_init(float target)
{	
	memset(&temp, 0, sizeof(temp));
	memset(&pid, 0, sizeof(pid));
	memset(&buffer, 0, sizeof(buffer));
	
	temp.target = target;
	
	/* 如下计算周期始终大于最大控制时间长度，这样执行机构才能来的及动作，计算太快就会来不及动作。 */
	pid.t = 500;  //计算周期500ms
	
	/*位置式PID参数*/
	pid.p = 30;
	pid.i_t = 5000000; 
	pid.d_t = 1000;
	pid.static_pid_out = 1;
	pid.control_cycle_pid_out = 200; //最大控制时间200ms，因为是使用继电器控制，故此处的控制pid_out对应为控制加热的时间。
	
	/*增量式PID参数*/
//	pid.p = 5;
//	pid.i_t = 4000; 
//	pid.d_t = 2000;
//	pid.static_pid_out = 0;
//	pid.control_cycle_pid_out = 100; 
}





