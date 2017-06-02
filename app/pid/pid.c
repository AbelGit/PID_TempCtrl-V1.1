
#include "pid.h"
#include "stm32f10x.h"
#include "driver.h"
#include "relay.h"
#include "delay.h"
#include "beep.h"
#include "led.h"
#include "ds18b20.h"

static void process_high_data(void)
{
	LED_HIGH(ON);
	LED_LOW(OFF);

	beep_high_sound();	
}

static void process_low_data(void)
{
	LED_LOW(ON);
	LED_HIGH(OFF);
	
	beep_low_sound();	
}

static void process_normal_data(void)
{
	beep_normal_sound();
}

void data_update(void)
{
	temp.curret_value = DS18B20_Get_Temp();
	
	if(temp.curret_value > temp.target)
	{
		process_high_data();
	}
	else if(temp.curret_value < temp.target)
	{
		process_low_data();
	}else
	{
		process_normal_data();
	}
}

//�����̵�����Ӧ
static void bit_control(void)
{
	if(temp.target >= temp.curret_value)
	{
		WARM(ON);
	}
	else
	{
		WARM(OFF);
	}
}
/*
���лز�Ķ�λʽ�����㷨��ȡ�趨ֵ����5%��Ϊ�ز���������ޣ��������޲ſ�ʼ���L��
�������޲ſ�ʼ���H���������ٽ�㸽���Ĳ���Ƶ����������Ȼ����׼ȷ�ﵽĿ��ֵ��

ʵ��ϵͳ�Ŀ��ƣ��¶Ƚ���ֻ�ܿ���Ȼ��ȴ���ʴ˴�ֻ�з�ֹ���������ֵ��

*/
static void hys_bit_control(void)
{
	float low_target = 0;
	
	low_target = (1-0.05) * temp.target;
	
	if(temp.curret_value >= low_target)
	{
		WARM(OFF);
	}else
	{
		WARM(ON);
	}
}

/*λ��ʽPID�㷨
�ﵽһ�����¶�ֵ���ſ���pid�㷨������ﵽ���¶�ֵ�趨Ϊtarget�¶�ֵ��15%����ʱ
����P�㷨�������I�㷨�����������D�㷨��ȷ���� 
*/
void pid_out(void)
{
	static u16 control_time = 0;
	
	control_time++;
	if(control_time >= pid.control_cycle_pid_out)
	{
		control_time = 0;
	}
	if(control_time < pid.pid_out)
	{
		WARM(ON);
	}else
	{
		WARM(OFF);
	}
}
static void pid_out_limit(void)
{
	if(pid.pid_out < 0)
	{
		pid.pid_out = pid.static_pid_out;
	}else if(pid.pid_out > pid.control_cycle_pid_out)
	{
		pid.pid_out = pid.control_cycle_pid_out;
	}
}
/**
  * @brief  λ��ʽPID�㷨
1.�˴��ļ�����ý�ģ������ɢ�����������������ɣ����������㹻С�������
  ���Խ�����Ϊ�ƽ�ʵ��ֵ��
2.�����ۼ������֪���������¿�ϵͳ����pid.i, pid.d �����������Ĵ�Сֱ�ӱ�pid.p��Ti,Td�Ĵ�С������
  ���Ժ��ڰ�������PID������ֻ��һ�ִ�ŵ�ģ�⣬���̶�Ti��Td�Ĵ�Сʱ��pid.i and pid.d�Ĵ�Сֱ�ӱ�pid.p������
3.����˵�����pid.i,pid.d�����ĵ�������������ж�Ӧ����Ti,Td��ֱ�ӵ�������ϵͳ��Ϊ����ֱ�۵ı仯����ȡ��ǰ�ߡ�

**/ 
static void position_pid_cal(void)
{
	//TIM2 50ms�ж�һ�Σ�count����������1.
	if(pid.count < (pid.t/50))
	{
		return;
	}
	temp.currrent_deviation = temp.target - temp.curret_value;
	pid.p_out = temp.currrent_deviation * pid.p;    //�������
	
	temp.history_deviation += temp.currrent_deviation;  
	pid.i = pid.p * (pid.t/pid.i_t) * pid.p;   //pid.i = pid.p * (pid.t/pid.i_t) * pid.p; ���� 
	pid.i_out = temp.history_deviation * pid.i;   //�������
	
	temp.differential_deviation = temp.currrent_deviation - temp.pre_currrent_deviation; 
	pid.d = pid.p * (pid.d_t/pid.t);
	pid.d_out = temp.differential_deviation * pid.d;  //΢�����
	
	pid.pid_out = pid.p_out + pid.i_out + pid.d_out;   //�ܵ���� 
	
	/*����޷�*/
	pid_out_limit();
	
	temp.pre_currrent_deviation = temp.currrent_deviation;  //����ǰһ��΢��ƫ�� 
	
	pid.count = 0;  //��գ����¼���500ms 
	
	/* ����pid���������pid.pid_out(��ϵͳ��ӦΪ���ȿ���ʱ��)�����ִ�л���ÿ��1ms���������һ�� */
}
static void position_pid_control(void)
{
	//Ŀ���¶�15%����������P�㷨
	if(temp.curret_value >= ((1-0.15) * temp.target))
	{
		position_pid_cal();
	}else
	{
		WARM(ON);
	}
}

/**
  * @brief   ����ʽPID�㷨
1.����λ��ʽpid�㷨���ὫI�ۼӵ��ܴ��ֵ��һ����������Ͼͻᵼ���������ȱ仯������������ص��¹ʣ�
  ��ʵ�ʹ����ɲ�������ʽpid�㷨�����⡣

  **/ 
static void increment_pid_cal(void)
{
	float dk1;
	float dk2;
	float t1,t2,t3;
	
	//TIM2 50ms�ж�һ�Σ�count����������1.
	if(pid.count < (pid.t/50))
	{
		return;
	}	
	temp.currrent_deviation = temp.target - temp.curret_value;
	dk1 = temp.currrent_deviation - temp.pre_currrent_deviation;
	dk2 = temp.currrent_deviation - 2*temp.pre_currrent_deviation + temp.pre2_current_deviation;
	
	t1 = pid.p * dk1;
	
	pid.i = pid.p * (pid.t/pid.i_t);
	t2 = pid.i * temp.currrent_deviation;
	
	pid.d = pid.p * (pid.d_t/pid.t);
	t3 = pid.d * dk2;

	pid.increase_out = t1 + t2 + t3;  //�������ʱ�䳤��
	pid.pid_out += pid.increase_out;  //500ms����һ�Σ���pid_outÿ��500ms�ۼ�һ�Ρ�
	
	pid_out_limit();
	
	temp.pre2_current_deviation = temp.pre_currrent_deviation;
	temp.pre_currrent_deviation = temp.currrent_deviation;
	
	pid.count = 0;
}
static void increment_pid_control(void)
{
	//Ŀ���¶�15%����������P�㷨
	if(temp.curret_value >= ((1-0.15) * temp.target))
	{
		increment_pid_cal();
	}else
	{
		WARM(ON);
	}
}
static void empty(void)
{}

void pid_control(int type)
{
	data_update();
	
	switch(type)
	{
		case BIT_CONTROL: bit_control(); break;
		case HYS_BIT_CONTROL: hys_bit_control(); break;
		case POSITION_PID_CONTROL: position_pid_control(); break;
		case INCREASE_PID_CONTROL: increment_pid_control(); break;
		default: empty(); break;
	}
}




