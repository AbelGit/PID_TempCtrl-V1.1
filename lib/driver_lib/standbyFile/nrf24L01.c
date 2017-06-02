/******************** (C) COPYRIGHT 2014 Air Nano Team ***************************
 * �ļ���  ��nrf24l01.c
 * ����    ��nrf24l01����         
 * ʵ��ƽ̨��Air Nano���������
 * ��汾  ��ST3.5.0
 * ����    ��Air Nano Team 
 * �Ա�    ��http://byd2.taobao.com
**********************************************************************************/
#include <stdio.h>

#include "stm32f10x.h"
#include "nrf24L01.h"
#include "spi.h"
#include "driver.h"
#include "delay.h"
  
u8  TX_ADDRESS[TX_ADR_WIDTH]= {0xE1,0xE2,0xE3,0xE4,0xE5};	//���ص�ַ
u8  RX_ADDRESS[RX_ADR_WIDTH]= {0xE1,0xE2,0xE3,0xE4,0xE5};	//���յ�ַ

/*====================================================================================================*/
/*====================================================================================================*
**���� : NRF24L01_Init
**���� : ��ʼ��24L01��IO��
**���� : None
**ݔ�� : None
**��ע : None
**====================================================================================================*/
/*====================================================================================================*/
void NRF24L01_Init(void)
{ 	
	NRF24L01_CE_L();
	NRF24L01_CSN_H();
	SPI_Config(); 		 	 
}
/*====================================================================================================*/
/*====================================================================================================*
**���� : NRF24L01_Check
**���� : ���24L01�Ƿ����
**���� : None
**ݔ�� : 0���ɹ�;1��ʧ��	
**��ע : None
**====================================================================================================*/
/*====================================================================================================*/
u8 NRF24L01_Check(void)
{
	u8 buf[5]={0XA5,0XA5,0XA5,0XA5,0XA5};
	u8 i;  	 
	NRF24L01_Write_Buf(WRITE_REG_NRF+TX_ADDR,buf,5);//д��5���ֽڵĵ�ַ.	
	NRF24L01_Read_Buf(TX_ADDR,buf,5); //����д��ĵ�ַ  
	for(i=0;i<5;i++)if(buf[i]!=0XA5)break;	 							   
	if(i!=5)return 0;//���24L01����	
	return 1;		 //��⵽24L01
}	 	 
/*====================================================================================================*/
/*====================================================================================================*
**���� : NRF24L01_Write_Reg
**���� : SPIд�Ĵ���
**���� : reg:ָ���Ĵ�����ַ  value:д���ֵ
**ݔ�� : ����״ֵ̬
**��ע : None
**====================================================================================================*/
/*====================================================================================================*/
u8 NRF24L01_Write_Reg(uint8_t reg,uint8_t value)
{
	  u8 status;	
   	NRF24L01_CSN_L();                 //ʹ��SPI����
  	status =SPI_ReadWriteByte(reg);//���ͼĴ����� 
  	SPI_ReadWriteByte(value);      //д��Ĵ�����ֵ
  	NRF24L01_CSN_H();                 //��ֹSPI����	   
  	return(status);       			//����״ֵ̬
}
/*====================================================================================================*/
/*====================================================================================================*
**���� : NRF24L01_Read_Reg
**���� : ��ȡSPI�Ĵ���ֵ
**���� : reg:Ҫ���ļĴ���
**ݔ�� : None
**��ע : None
**====================================================================================================*/
/*====================================================================================================*/
u8 NRF24L01_Read_Reg(u8 reg)
{
	u8 reg_val;	    
	NRF24L01_CSN_L();;          //ʹ��SPI����		
	SPI_ReadWriteByte(reg);   //���ͼĴ�����
	reg_val=SPI_ReadWriteByte(0XFF);//��ȡ�Ĵ�������
	NRF24L01_CSN_H();;          //��ֹSPI����		    
	return(reg_val);           //����״ֵ̬
}	
/*====================================================================================================*/
/*====================================================================================================*
**���� : NRF24L01_Read_Buf
**���� : ��ָ��λ�ö���ָ�����ȵ�����
**���� : reg:�Ĵ���(λ��)  *pBuf:����ָ��  len:���ݳ���
**ݔ�� : None
**��ע : None
**====================================================================================================*/
/*====================================================================================================*/
u8 NRF24L01_Read_Buf(u8 reg,u8 *pBuf,u8 len)
{
	  u8 status,u8_ctr;	       
  	NRF24L01_CSN_L();           //ʹ��SPI����
  	status=SPI_ReadWriteByte(reg);//���ͼĴ���ֵ(λ��),����ȡ״ֵ̬   	   
 	  for(u8_ctr=0;u8_ctr<len;u8_ctr++)pBuf[u8_ctr]=SPI_ReadWriteByte(0XFF);//��������
  	NRF24L01_CSN_H();       //�ر�SPI����
  	return status;        //���ض�����״ֵ̬
}
/*====================================================================================================*/
/*====================================================================================================*
**���� : NRF24L01_Write_Buf
**���� : ��ָ��λ��дָ�����ȵ�����
**���� : reg:�Ĵ���(λ��)  *pBuf:����ָ��   len:���ݳ���
**ݔ�� : �˴ζ�����״̬�Ĵ���ֵ
**��ע : None
**====================================================================================================*/
/*====================================================================================================*/
u8 NRF24L01_Write_Buf(u8 reg, u8 *pBuf, u8 len)
{
	  u8 status,u8_ctr;	    
 	  NRF24L01_CSN_L();          //ʹ��SPI����
  	status = SPI_ReadWriteByte(reg);//���ͼĴ���ֵ(λ��),����ȡ״ֵ̬
  	for(u8_ctr=0; u8_ctr<len; u8_ctr++)SPI_ReadWriteByte(*pBuf++); //д������	 
  	NRF24L01_CSN_H();       //�ر�SPI����
  	return status;          //���ض�����״ֵ̬
}				   
/*====================================================================================================*/
/*====================================================================================================*
**���� : NRF24L01_TxPacket
**���� : ����NRF24L01����һ������
**���� : txbuf:�����������׵�ַ
**ݔ�� : �������״��
**��ע : None
**====================================================================================================*/
/*====================================================================================================*/
u8 NRF24L01_TxPacket(u8 *txbuf)
{
	u8 sta;
	SPI_SetSpeed(SPI_BaudRatePrescaler_16);//spi�ٶ�Ϊ9Mhz��24L01�����SPIʱ��Ϊ10Mhz��   
	NRF24L01_CE_L();
	NRF24L01_Write_Buf(WR_TX_PLOAD,txbuf,TX_PLOAD_WIDTH);//д���ݵ�TX BUF  32���ֽ�  
	NRF24L01_CE_H();	

	delay_us(2);  //��Ϊ��ʱ��������һֱ�ȴ�

	//while(NRF24L01_IRQ!=0);//�ȴ��������
	sta=NRF24L01_Read_Reg(STATUS);  //��ȡ״̬�Ĵ�����ֵ	   
	NRF24L01_Write_Reg(WRITE_REG_NRF+STATUS,sta); //���TX_DS��MAX_RT�жϱ�־
	if(sta&MAX_TX)//�ﵽ����ط�����
	{
		NRF24L01_Write_Reg(FLUSH_TX,0xff);//���TX FIFO�Ĵ��� 
		return MAX_TX; 
	}
	if(sta&TX_OK)//�������
	{
		return TX_OK;
	}
	return 0xff;//����ԭ����ʧ��
}
/*====================================================================================================*/
/*====================================================================================================*
**���� : NRF24L01_RxPacket
**���� : ����NRF24L01����һ������
**���� : txbuf:�����������׵�ַ
**ݔ�� : 0��������ɣ��������������
**��ע : None
**====================================================================================================*/
/*====================================================================================================*/
u8 NRF24L01_RxPacket(u8 *rxbuf)
{
	u8 sta;		    							   
	SPI_SetSpeed(SPI_BaudRatePrescaler_32); //spi�ٶ�Ϊ9Mhz��24L01�����SPIʱ��Ϊ10Mhz��   
	sta=NRF24L01_Read_Reg(STATUS);  //��ȡ״̬�Ĵ�����ֵ    	 
	NRF24L01_Write_Reg(WRITE_REG_NRF+STATUS,sta); //���TX_DS��MAX_RT�жϱ�־
	if(sta&RX_OK)//���յ�����
	{
		NRF24L01_Read_Buf(RD_RX_PLOAD,rxbuf,RX_PLOAD_WIDTH);//��ȡ����
		NRF24L01_Write_Reg(FLUSH_RX,0xff);//���RX FIFO�Ĵ��� 
		return 0; 
	}	   
	return 1;//û�յ��κ�����
}					    
/*====================================================================================================*/
/*====================================================================================================*
**���� : NRF24L01_Mode
**���� : ����2401����ģʽ
**���� : None
**ݔ�� : None
**��ע : None
**====================================================================================================*/
/*====================================================================================================*/
void NRF24L01_Mode(u8 model)
{
   NRF24L01_CE_L();     
   NRF24L01_Write_Buf(WRITE_REG_NRF+TX_ADDR,(u8*)TX_ADDRESS,TX_ADR_WIDTH);//дTX�ڵ��ַ 
   NRF24L01_Write_Buf(WRITE_REG_NRF+RX_ADDR_P0,(u8*)RX_ADDRESS,RX_ADR_WIDTH); //����TX�ڵ��ַ,��ҪΪ��ʹ��ACK	  

   NRF24L01_Write_Reg(WRITE_REG_NRF+EN_AA,0x01);     //ʹ��ͨ��0���Զ�Ӧ��    
   NRF24L01_Write_Reg(WRITE_REG_NRF+EN_RXADDR,0x01); //ʹ��ͨ��0�Ľ��յ�ַ  
   NRF24L01_Write_Reg(WRITE_REG_NRF+SETUP_RETR,0x1a);//�����Զ��ط����ʱ��:500us + 86us;����Զ��ط�����:10��
   NRF24L01_Write_Reg(WRITE_REG_NRF+RF_CH,40);       //����RFͨ��Ϊ40
   NRF24L01_Write_Reg(WRITE_REG_NRF+RF_SETUP,0x0f);  //����TX�������,0db����,2Mbps,���������濪��  
/////////////////////////////////////////////////////////
	if(model==1)				//RX
	{
		NRF24L01_Write_Reg(WRITE_REG_NRF+RX_PW_P0,RX_PLOAD_WIDTH);//ѡ��ͨ��0����Ч���ݿ��� 
		NRF24L01_Write_Reg(WRITE_REG_NRF + CONFIG, 0x0f);   		 // IRQ�շ�����жϿ���,16λCRC,������
	}
	else if(model==2)		//TX
	{
		NRF24L01_Write_Reg(WRITE_REG_NRF+RX_PW_P0,RX_PLOAD_WIDTH);//ѡ��ͨ��0����Ч���ݿ��� 
		NRF24L01_Write_Reg(WRITE_REG_NRF + CONFIG, 0x0e);   		 // IRQ�շ�����жϿ���,16λCRC,������
	}
	else if(model==3)		//RX2
	{
		NRF24L01_Write_Reg(FLUSH_TX,0xff);
		NRF24L01_Write_Reg(FLUSH_RX,0xff);
		NRF24L01_Write_Reg(WRITE_REG_NRF + CONFIG, 0x0f);   		 // IRQ�շ�����жϿ���,16λCRC,������
		
		SPI_ReadWriteByte(0x50);
		SPI_ReadWriteByte(0x73);
		NRF24L01_Write_Reg(WRITE_REG_NRF+0x1c,0x01);
		NRF24L01_Write_Reg(WRITE_REG_NRF+0x1d,0x07);
	}
	else								//TX2
	{
		NRF24L01_Write_Reg(WRITE_REG_NRF + CONFIG, 0x0e);   		 // IRQ�շ�����жϿ���,16λCRC,������
		NRF24L01_Write_Reg(FLUSH_TX,0xff);
		NRF24L01_Write_Reg(FLUSH_RX,0xff);
		
		SPI_ReadWriteByte(0x50);
		SPI_ReadWriteByte(0x73);
		NRF24L01_Write_Reg(WRITE_REG_NRF+0x1c,0x01);
		NRF24L01_Write_Reg(WRITE_REG_NRF+0x1d,0x07);
	}
	NRF24L01_CE_H(); 
}

