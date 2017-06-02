#ifndef __PID_H
#define __PID_H

typedef enum
{
	BIT_CONTROL = 0,     			//��λʽ����
	HYS_BIT_CONTROL,				//�ز��λʽ����
	POSITION_PID_CONTROL,			//λ��ʽPID����
	INCREASE_PID_CONTROL,			//����ʽPID���� 
	
}_ALGORITHM;

void data_update(void);
void pid_control(int type);
void pid_out(void);

#endif 

