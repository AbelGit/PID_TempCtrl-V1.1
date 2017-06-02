#include <stdio.h>
#include "stm32f10x.h"
#include "timer.h"
#include "driver.h"


//50ms�ж�һ��
void tim2_init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	//�������ã�ʱ���ͱȽ�������ã���������ֻ�趨ʱ�����Բ���OC�Ƚ����
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	
	TIM_DeInit(TIM2);

	TIM_TimeBaseStructure.TIM_Period = 50000-1;//װ��ֵ
	//prescaler is 1200,that is 72000000/72/500=2000Hz;
	TIM_TimeBaseStructure.TIM_Prescaler = 71;//��Ƶϵ��
	//set clock division 
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //or TIM_CKD_DIV2 or TIM_CKD_DIV4
	//count up
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	//clear the TIM5 overflow interrupt flag
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);

	//TIM2 overflow interrupt enable
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	TIM_Cmd(TIM2, ENABLE);
}

void tim3_init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	
	TIM_DeInit(TIM3);

	TIM_TimeBaseStructure.TIM_Period = 1000-1;//װ��ֵ
	TIM_TimeBaseStructure.TIM_Prescaler = 72-1;//��Ƶϵ��
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	TIM_ClearFlag(TIM3, TIM_FLAG_Update);
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	TIM_Cmd(TIM3, ENABLE);
}






















