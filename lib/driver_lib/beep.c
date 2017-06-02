#include "stm32f10x.h"
#include "beep.h"
#include "delay.h"

void beep_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(BEEP_RCC, ENABLE);	 

	GPIO_InitStructure.GPIO_Pin = BEEP_PIN;				 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(BEEP_PORT, &GPIO_InitStructure);	 		
	
	BEEP(OFF); 
}

void beep_normal_sound(void)
{
	BEEP(OFF); 
}

void beep_high_sound(void)
{
	BEEP(ON);
	delay_ms(1000);
	BEEP(OFF);
	delay_ms(1000);
	BEEP(ON);
}
void beep_low_sound(void)
{
	BEEP(ON);
	delay_ms(1000);
	BEEP(OFF);
	delay_ms(1000);
	BEEP(ON);
}	




