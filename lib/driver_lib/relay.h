#ifndef __RELAY_H
#define __RELAY_H

#define RELAY(a) 	if(a)  \
										GPIO_ResetBits (RELAY_PORT,  RELAY_WARM_PIN);\
									else		\
										GPIO_SetBits (RELAY_PORT,  RELAY_WARM_PIN);

#define WARM(a) RELAY(a)									
						
									
void relay_init(void);

#endif 

