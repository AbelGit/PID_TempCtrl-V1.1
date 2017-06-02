/******************** (C) COPYRIGHT 2014 Air Nano Team ***************************
 * �ļ���  ��spi.c
 * ����    ��spi����         
 * ʵ��ƽ̨��Air Nano���������
 * ��汾  ��ST3.5.0
 * ����    ��Air Nano Team 
 * �Ա�    ��http://byd2.taobao.com
**********************************************************************************/
#include "spi.h"
#include "driver.h"

void SPI_Config(void)
{ 
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;

	/*ʹ��SPI1ʱ��*/
	RCC_APB2PeriphClockCmd(SPI_RCC_PORT, ENABLE);
	
	/*������ӦIO�˿ڵ�ʱ��*/
	RCC_APB2PeriphClockCmd(NRF_RCC_PORT1 | NRF_RCC_PORT2 | NRF_RCC_PORT3, ENABLE);

	//  PA5--CLK  PA7--MOSI  
	GPIO_InitStructure.GPIO_Pin   =  NRF_SCK_PIN | NRF_MOSI_PIN; 
	GPIO_InitStructure.GPIO_Speed =  GPIO_Speed_50MHz; 
	GPIO_InitStructure.GPIO_Mode  =  GPIO_Mode_AF_PP; 
	GPIO_Init(NRF_SCK_PORT, &GPIO_InitStructure);

	//PA6--MISO 
	GPIO_InitStructure.GPIO_Pin  = NRF_MISO_PIN; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
	GPIO_Init(NRF_MISO_PORT, &GPIO_InitStructure);

	//PG15--NSS   CSN 
	GPIO_InitStructure.GPIO_Pin   = NRF_CSN_PIN; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP; 
	GPIO_Init(NRF_CSN_PORT, &GPIO_InitStructure);

	//PG8--CE
	GPIO_InitStructure.GPIO_Pin   = NRF_CE_PIN; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP; 
	GPIO_Init(NRF_CE_PORT, &GPIO_InitStructure);

	//PC4--IRQ  
	GPIO_InitStructure.GPIO_Pin   = NRF_IRQ_PIN;   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU; //��������
	GPIO_Init(NRF_IRQ_PORT, &GPIO_InitStructure);

					 
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; //ȫ˫��
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;                      //��ģʽ
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;                  //һ��ת��8λ
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;                        //���е�ƽ�͵�ƽ
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;                      //��һ�������ز���
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;                         // NSSΪ���ģʽ
	SPI_InitStructure.SPI_BaudRatePrescaler =SPI_BaudRatePrescaler_8; //
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;        					//���ݴ����λ��ǰ
	SPI_InitStructure.SPI_CRCPolynomial = 7;                           //CRCУ��ģʽ
	SPI_Init(SPI_PORT, &SPI_InitStructure);                                //��ʼ��

	SPI_Cmd(SPI_PORT, ENABLE); //SPI1
	SPI_ReadWriteByte(0xff);//��������	
}
//SPI �ٶ����ú���
//SpeedSet:
//SPI_BaudRatePrescaler_2   2��Ƶ   
//SPI_BaudRatePrescaler_8   8��Ƶ   
//SPI_BaudRatePrescaler_16  16��Ƶ  
//SPI_BaudRatePrescaler_256 256��Ƶ 
  
void SPI_SetSpeed(u8 SPI_BaudRatePrescaler)
{
	assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));
	SPI_PORT->CR1 &= 0XFFC7;
	SPI_PORT->CR1 |= SPI_BaudRatePrescaler;	//����SPI�ٶ� 
	SPI_Cmd(SPI_PORT, ENABLE); 
} 


//SPIx ��дһ���ֽ�
//TxData:Ҫд����ֽ�
//����ֵ:��ȡ�����ֽ�
u8 SPI_ReadWriteByte(u8 TxData)
{		
	u8 retry=0;				 	
	while (SPI_I2S_GetFlagStatus(SPI_PORT, SPI_I2S_FLAG_TXE) == RESET) //���ָ����SPI��־λ�������:���ͻ���ձ�־λ
	{
		retry++;
		if(retry>200)return 0;
	}			  
	SPI_I2S_SendData(SPI_PORT, TxData); //ͨ������SPIx����һ������
	retry=0;

	while (SPI_I2S_GetFlagStatus(SPI_PORT, SPI_I2S_FLAG_RXNE) == RESET)//���ָ����SPI��־λ�������:���ܻ���ǿձ�־λ
	{
		retry++;
		if(retry>200)return 0;
	}	  						    
	return SPI_I2S_ReceiveData(SPI_PORT); //����ͨ��SPIx������յ�����					    
}


