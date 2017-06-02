/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "stm32f10x_it.h"
#include "driver.h"
#include "lcd1602.h"
#include "pid.h"

/*
	NVIC ��������16���ж����������ȼ�����NVIC��һ��4λ������������������4λ���ַ����
	��ռ���ȼ�����Ӧ���ȼ����֣���5����䷽ʽ��
			��ռ    ��Ӧ 
	0�飺   4		0
	1�飺 	1		3
	2�飺   2		2
	3�飺	3		1
	4�飺   0		4 

	NVIC_PriorityGroup_0-4

*/
void Project_Nvic_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	//2λ��ռ�� 2λ��Ӧ 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 	
	
	//usart1 
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			
	NVIC_Init(&NVIC_InitStructure);
	
	//tim2 
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //50ms
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	//tim3 
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;    //1ms,������Ӧpid_out���ƻ��� 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	
	// 0-4 line interrupt  ����ط�һ��Ҫ�ֿ�д��д��һ��Ͳ��ܶ��������ˡ�
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;            
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;            
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;            
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;            
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

static void process_case0(void)
{
	if(pid.flag == 0)
	{
		pid.flag = 1;
	}else
	{
		pid.flag = 0;
	}
}
static void process_case1(void)
{
	if(pid.flag == 0)
	{
		pid.p += 1.0f;
	}else
	{
		pid.p -= 1.0f;
	}
}
static void process_case2(void)
{
	if(pid.flag == 0)
	{
		//pid.i += 0.2f;
		pid.i_t += 200000;  //λ��ʽ
		//pid.i_t += 50;  //����ʽ
	}else
	{
		//pid.i -= 0.2f;
		pid.i_t -= 200000;  //λ��ʽ
		//pid.i_t -= 50;  //����ʽ
	}
}
static void process_case3(void)
{
	if(pid.flag == 0)
	{
		//pid.d += 0.1f;
		pid.d_t += 50;
	}else
	{
		//pid.d -= 0.1f;
		pid.d_t -= 50;
	}
}

static void process_exti(int select)
{
	switch(select)
	{
		case 0: process_case0(); break;
		case 1: process_case1(); break;
		case 2: process_case2(); break;
		case 3: process_case3(); break;
	}
}

void EXTI0_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line0) != RESET) 
	{
		process_exti(0);
		EXTI_ClearITPendingBit(EXTI_Line0);     
	}
}

void EXTI1_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line1) != RESET) 
	{
		process_exti(1);
		EXTI_ClearITPendingBit(EXTI_Line1);     
	}
}

void EXTI2_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line2) != RESET) 
	{
		process_exti(2);
		EXTI_ClearITPendingBit(EXTI_Line2);     
	}
}

void EXTI3_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line3) != RESET) 
	{
		process_exti(3);
		EXTI_ClearITPendingBit(EXTI_Line3);     
	}
}

void TIM2_IRQHandler(void)		    //50ms�ж�һ��
{	
	if(TIM2->SR & TIM_IT_Update)	
	{    
		show_data();
		pid.count++;
		TIM2->SR = ~TIM_FLAG_Update;  //����жϱ�־
	}
}
void TIM3_IRQHandler(void)		    //1ms�ж�һ��
{	
	if(TIM3->SR & TIM_IT_Update)	
	{    
		pid_out();  //ÿ��1ms���ƻ�������һ��
		TIM3->SR = ~TIM_FLAG_Update;  //����жϱ�־
	}
}


/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 



/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
