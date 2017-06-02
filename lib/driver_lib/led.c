
#include "stm32f10x.h"
#include "driver.h"
#include "led.h"


void led_gpio_init(void)
{
	GPIO_InitTypeDef pig; 
	
	RCC_APB2PeriphClockCmd(LED_RCC_PORT1, ENABLE); 

	pig.GPIO_Pin = LED_HIGH_PIN | LED_LOW_PIN;  
	pig.GPIO_Mode = GPIO_Mode_Out_PP; 
	pig.GPIO_Speed = GPIO_Speed_50MHz;  

	GPIO_Init(LED_PORT, &pig); 
	
	LED_LOW(OFF);
	LED_HIGH(OFF);	
	LED_NORMAL(ON);
}
