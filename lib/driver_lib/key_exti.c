
//PD 8,9,10,11
/* 按键的外部中断配置 */
#include <stdio.h>

#include "stm32f10x.h"
#include "key_exti.h"
#include "driver.h"

static void gpio_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = KEY_EXTI_PIN0 | KEY_EXTI_PIN1 | KEY_EXTI_PIN2 | KEY_EXTI_PIN3; 	  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;   //选择下拉和上拉回直接影响中断结果
	GPIO_Init(KEY_EXTI_PORT, &GPIO_InitStructure);
	
	GPIO_EXTILineConfig(KEY_EXTI_LINE_PORT1, KEY_EXTI_LINE_PIN0 | KEY_EXTI_LINE_PIN1 | KEY_EXTI_LINE_PIN2 | KEY_EXTI_LINE_PIN3);  
}

static void exti_init(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
	
	EXTI_InitStructure.EXTI_Line = KEY_EXTI_LINE0 | KEY_EXTI_LINE1 | KEY_EXTI_LINE2 | KEY_EXTI_LINE3;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;     
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; 
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;              
	
	EXTI_Init(&EXTI_InitStructure); 
}

void exti_key_init(void)
{	
	RCC_APB2PeriphClockCmd(KEY_RCC_PORT1 | RCC_APB2Periph_AFIO, ENABLE);

	gpio_init();
	exti_init();
}

