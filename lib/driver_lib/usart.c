/**
  ******************************************************************************
  * @file    usart.c
  * @author  abel li 
  * @version V1.0
  * @date    2017/1/11
  * @attention    http://www.chuxue123.com   http://www.openedv.com  
  * @brief  
	1. ���ڲ����ʿ�����   
	2. ֧��printf, ��Ӵ��ڽ��������, ����printf��һ���ַ���ʧ��bug
	3. �޸���USART_RX_STA, ʹ�ô����������ֽ���Ϊ2��14�η�
	4. ������USART_REC_LEN, ���ڶ��崮�����������յ��ֽ���(������2��14�η�)
	5. �޸���EN_USART1_RX��ʹ�ܷ�ʽ
	6. ֧�� UCOSII
	7. ֧��scanf������usart���ض���
  *****************************************************************************/ 
  
#include "usart.h"
#include "sys.h"
#include "driver.h"

/************************************ 1-1 *************************************/
//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos ʹ��	  
#endif
/************************************ 1-1 end *********************************/


/************************************ 1-2 *************************************/
/**
  * @brief  �������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
  **/  
#if 0
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc����  �Ĵ�����
int fputc(int ch, FILE *f)
{ 	
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
	USART1->DR = (u8) ch;      
	return ch;
}

#endif
/************************************ 1-2 end**********************************/

/*********************************1-2-1****************************************/
/*ʹ��microLib�ķ���*/
//�ض���c�⺯��printf��USART1  �⺯����
int fputc(int ch, FILE *f)
{
	/* ����һ���ֽ����ݵ�USART1 */
	USART_SendData(USART1, (uint8_t) ch);
	/* �ȴ�������� */
	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);		
	return (ch);
}

//�ض���c�⺯��scanf��USART1   �⺯����
int fgetc(FILE *f)
{
	/* �ȴ�����1�������� */
	while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);
	return (int)USART_ReceiveData(USART1);
}
/**********************************1-2-1 end **********************************/

/************************************ 1-3 *************************************/
#if EN_USART1_RX   //���ʹ���˽���

//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART_RX_STA = 0;       //����״̬���	


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
	
	USART_ITConfig(USART_NAME, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
	USART_Cmd(USART_NAME, ENABLE);
	
}


void usart_init(u32 baud)
{
	usart_gpio_init();
	usart_mode_init(baud);
}

/**
  * @brief  ����1���жϷ�����
  * @param  no
  * @retval no
  */
void USART1_IRQHandler(void)                	
{
	u8 Res;

	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		Res = USART_ReceiveData(USART1);//(USART1->DR);	//��ȡ���յ�������
		
		if((USART_RX_STA&0x8000)==0)//����δ���
		{
			if(USART_RX_STA&0x4000)//���յ���0x0d
			{
				if(Res!=0x0a)USART_RX_STA=0;//���մ���,���¿�ʼ
				else USART_RX_STA|=0x8000;	//��������� 
			}
			else //��û�յ�0X0D
			{	
				if(Res==0x0d)USART_RX_STA|=0x4000;
				else
				{
					USART_RX_BUF[USART_RX_STA&0X3FFF]=Res ;
					USART_RX_STA++;
					if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;  //�������ݴ���,���¿�ʼ����	  
				}		 
			}
		}   		 
  } 
} 
#endif	







