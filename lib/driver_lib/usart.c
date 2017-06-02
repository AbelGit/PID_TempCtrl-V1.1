/**
  ******************************************************************************
  * @file    usart.c
  * @author  abel li 
  * @version V1.0
  * @date    2017/1/11
  * @attention    http://www.chuxue123.com   http://www.openedv.com  
  * @brief  
	1. 串口波特率可配置   
	2. 支持printf, 添加串口接收命令功能, 修正printf第一个字符丢失的bug
	3. 修改了USART_RX_STA, 使得串口最大接收字节数为2的14次方
	4. 增加了USART_REC_LEN, 用于定义串口最大允许接收的字节数(不大于2的14次方)
	5. 修改了EN_USART1_RX的使能方式
	6. 支持 UCOSII
	7. 支持scanf函数到usart的重定向
  *****************************************************************************/ 
  
#include "usart.h"
#include "sys.h"
#include "driver.h"

/************************************ 1-1 *************************************/
//如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos 使用	  
#endif
/************************************ 1-1 end *********************************/


/************************************ 1-2 *************************************/
/**
  * @brief  加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
  **/  
#if 0
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数  寄存器版
int fputc(int ch, FILE *f)
{ 	
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
	USART1->DR = (u8) ch;      
	return ch;
}

#endif
/************************************ 1-2 end**********************************/

/*********************************1-2-1****************************************/
/*使用microLib的方法*/
//重定向c库函数printf到USART1  库函数版
int fputc(int ch, FILE *f)
{
	/* 发送一个字节数据到USART1 */
	USART_SendData(USART1, (uint8_t) ch);
	/* 等待发送完毕 */
	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);		
	return (ch);
}

//重定向c库函数scanf到USART1   库函数版
int fgetc(FILE *f)
{
	/* 等待串口1输入数据 */
	while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);
	return (int)USART_ReceiveData(USART1);
}
/**********************************1-2-1 end **********************************/

/************************************ 1-3 *************************************/
#if EN_USART1_RX   //如果使能了接收

//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART_RX_STA = 0;       //接收状态标记	


static void usart_gpio_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(USART_RCC_PORT, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = USART_Tx_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(USART_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = USART_Rx_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(USART_PORT, &GPIO_InitStructure);
}


static void usart_mode_init(u32 baud)
{
	USART_InitTypeDef USART_InitStructure;

	RCC_APB2PeriphClockCmd(USART_RCC_MODE_PORT, ENABLE);
	
	/* USART1 mode config */
	USART_InitStructure.USART_BaudRate = baud;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART_NAME, &USART_InitStructure); 
	
	USART_ITConfig(USART_NAME, USART_IT_RXNE, ENABLE);//开启串口接受中断
	USART_Cmd(USART_NAME, ENABLE);
	
}


void usart_init(u32 baud)
{
	usart_gpio_init();
	usart_mode_init(baud);
}

/**
  * @brief  串口1的中断服务函数
  * @param  no
  * @retval no
  */
void USART1_IRQHandler(void)                	
{
	u8 Res;

	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		Res = USART_ReceiveData(USART1);//(USART1->DR);	//读取接收到的数据
		
		if((USART_RX_STA&0x8000)==0)//接收未完成
		{
			if(USART_RX_STA&0x4000)//接收到了0x0d
			{
				if(Res!=0x0a)USART_RX_STA=0;//接收错误,重新开始
				else USART_RX_STA|=0x8000;	//接收完成了 
			}
			else //还没收到0X0D
			{	
				if(Res==0x0d)USART_RX_STA|=0x4000;
				else
				{
					USART_RX_BUF[USART_RX_STA&0X3FFF]=Res ;
					USART_RX_STA++;
					if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;  //接收数据错误,重新开始接收	  
				}		 
			}
		}   		 
  } 
} 
#endif	







