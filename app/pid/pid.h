#ifndef __PID_H
#define __PID_H

typedef enum
{
	BIT_CONTROL = 0,     			//二位式控制
	HYS_BIT_CONTROL,				//回差二位式控制
	POSITION_PID_CONTROL,			//位置式PID控制
	INCREASE_PID_CONTROL,			//增量式PID控制 
	
}_ALGORITHM;

void data_update(void);
void pid_control(int type);
void pid_out(void);

#endif 

