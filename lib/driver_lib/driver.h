#ifndef __DRIVER_H
#define __DRIVER_H

#include "stm32f10x.h"

//usart
#define USART_RCC_PORT  RCC_APB2Periph_GPIOA
#define USART_RCC_MODE_PORT  RCC_APB2Periph_USART1
#define USART_Tx_Pin  GPIO_Pin_9
#define USART_Rx_Pin  GPIO_Pin_10
#define USART_PORT  GPIOA
#define USART_NAME  USART1

//LCD1602 
#define LCD1602_RS_PIN		GPIO_Pin_5
#define LCD1602_RW_PIN		GPIO_Pin_6
#define LCD1602_E_PIN		GPIO_Pin_7
#define LCD1602_DB0_PIN     GPIO_Pin_8
#define LCD1602_DB1_PIN		GPIO_Pin_9
#define LCD1602_DB2_PIN		GPIO_Pin_10
#define LCD1602_DB3_PIN		GPIO_Pin_11
#define LCD1602_DB4_PIN		GPIO_Pin_12
#define LCD1602_DB5_PIN		GPIO_Pin_13
#define LCD1602_DB6_PIN		GPIO_Pin_14
#define LCD1602_DB7_PIN		GPIO_Pin_15
#define LCD1602_PORT1 	GPIOB
#define LCD1602_RCC_PORT1 	RCC_APB2Periph_GPIOB

//key  
#define KEY_RCC_PORT1 	RCC_APB2Periph_GPIOA
#define KEY_EXTI_PIN0 	GPIO_Pin_0
#define KEY_EXTI_PIN1	GPIO_Pin_1
#define KEY_EXTI_PIN2	GPIO_Pin_2
#define KEY_EXTI_PIN3	GPIO_Pin_3
#define KEY_EXTI_PORT 	GPIOA
#define KEY_EXTI_LINE0 	EXTI_Line0
#define KEY_EXTI_LINE1	EXTI_Line1
#define KEY_EXTI_LINE2	EXTI_Line2
#define KEY_EXTI_LINE3	EXTI_Line3
#define KEY_EXTI_LINE_PORT1  GPIO_PortSourceGPIOA
#define KEY_EXTI_LINE_PIN0	GPIO_PinSource0
#define KEY_EXTI_LINE_PIN1	GPIO_PinSource1
#define KEY_EXTI_LINE_PIN2	GPIO_PinSource2
#define KEY_EXTI_LINE_PIN3	GPIO_PinSource3

//led
#define LED_RCC_PORT1 	RCC_APB2Periph_GPIOA
#define LED_HIGH_PIN 	GPIO_Pin_8
#define LED_NORMAL_PIN	GPIO_Pin_7
#define LED_LOW_PIN		GPIO_Pin_6
#define LED_PORT 	GPIOA

//switch
#define ON 1
#define OFF 0

//beep
#define BEEP_RCC	RCC_APB2Periph_GPIOA
#define BEEP_PIN	GPIO_Pin_5
#define BEEP_PORT	GPIOA

//relay
#define RELAY_RCC 			RCC_APB2Periph_GPIOA
#define RELAY_WARM_PIN		GPIO_Pin_7
#define RELAY_PORT			GPIOA

#define BUFF_SIZE 100

typedef struct
{
	float target;
	float curret_value;
	
	float min_value;
	float max_value;

	float currrent_deviation;
	float history_deviation;
	float pre_currrent_deviation;
	float pre2_current_deviation;
	float differential_deviation;
	
}_TEMP;

typedef struct
{
	int flag;   //pid参数是否加减的标志 
	
	float p;
	float i;
	float d;
	
	float t;  	//运算周期
	float i_t;  //积分时间
	float d_t;	//微分时间
	int count;  //50ms计数标志，即每过50ms count++ 
	
	float p_out;
	float i_out;
	float d_out;
	float increase_out;
	float pid_out;
	float static_pid_out;   //静态输出值
	float control_cycle_pid_out;  //控制周期
	
}_PID;

typedef struct
{
	char temp_buf[BUFF_SIZE];
	char pid_buf[BUFF_SIZE];
	
}_BUFF;

extern _TEMP temp;
extern _PID pid;
extern _BUFF buffer;

void driver_init(void);
void data_init(float target);
void data_update(void);
void show_data(void);

#endif 


