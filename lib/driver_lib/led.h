#ifndef __LED_H
#define __LED_H

#define LED_HIGH(a)  if(a)  \
							GPIO_ResetBits (LED_PORT,  LED_HIGH_PIN);\
					 else	\
							GPIO_SetBits (LED_PORT,  LED_HIGH_PIN);
#define LED_LOW(a)	 if(a)  \
							GPIO_ResetBits(LED_PORT, LED_LOW_PIN);\
					 else	\
							GPIO_SetBits(LED_PORT, LED_LOW_PIN);
#define LED_NORMAL(a)	if(a) \
							GPIO_ResetBits (LED_PORT,  LED_NORMAL_PIN);\
					 else   \
							GPIO_SetBits (LED_PORT,  LED_NORMAL_PIN);
					 
					 
void led_gpio_init(void);


#endif 


