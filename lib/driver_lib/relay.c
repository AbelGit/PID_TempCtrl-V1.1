#include "stm32f10x.h"
#include "relay.h"
#include "driver.h"

void relay_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RELAY_RCC, ENABLE);	               

	GPIO_InitStructure.GPIO_Pin = RELAY_WARM_PIN;				 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 			  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 			
	GPIO_Init(RELAY_PORT, &GPIO_InitStructure);
	
	RELAY(OFF);
}





