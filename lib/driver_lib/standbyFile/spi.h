#ifndef __SPI_H
#define __SPI_H

#include "include.h"


#define SPI_CE_H()   GPIO_SetBits(NRF_CE_PORT, NRF_CE_PIN) 
#define SPI_CE_L()   GPIO_ResetBits(NRF_CE_PORT, NRF_CE_PIN)

#define SPI_CSN_H()  GPIO_SetBits(NRF_CSN_PORT, NRF_CSN_PIN)
#define SPI_CSN_L()  GPIO_ResetBits(NRF_CSN_PORT, NRF_CSN_PIN)

void SPI_Config(void);
void SPI_SetSpeed(u8 SpeedSet); //…Ë÷√SPIÀŸ∂»   
u8 SPI_ReadWriteByte(u8 TxData);
	 
#endif


