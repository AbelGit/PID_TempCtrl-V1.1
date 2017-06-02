#ifndef __ROCKER_H
#define __ROCKER_H

#include "stm32f10x.h"

//°´¼ü
void Rocker_EXTI_Key_init(void);
void Rocker_ADC_Init(void);

//rocker
void rocker_adc_init(void);  
void filter(void);
float rocker_get_adc_val(u8 channel);

#endif



