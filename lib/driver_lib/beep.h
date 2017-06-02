#ifndef __BEEF_H
#define __BEEF_H

#include "stm32f10x.h"
#include "driver.h"


#define BEEP(a) 	if(a)  \
										GPIO_SetBits (BEEP_PORT,  BEEP_PIN);\
									else		\
										GPIO_ResetBits (BEEP_PORT,  BEEP_PIN);
									
																
void beep_init(void);

void beep_normal_sound(void);
void beep_high_sound(void);
void beep_low_sound(void);							

#endif

