
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

//单个继电器适应
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
具有回差的二位式控制算法，取设定值左右5%作为回差调节上下限，高于上限才开始输出L，
低于下限才开始输出H，避免在临界点附近的部件频繁动作，仍然不能准确达到目标值。

实际系统的控制，温度降低只能靠自然冷却，故此处只有防止过冲的下限值，

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

/*位置式PID算法
达到一定的温度值，才开启pid算法，这个达到的温度值设定为target温度值的15%附近时
开启P算法，随后开启I算法消减误差，最后开启D算法精确调控 
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
  * @brief  位置式PID算法
1.此处的计算采用将模拟量离散化后的数字量计算而成，采用周期足够小的情况下
  可以近似认为逼近实际值。
2.由理论计算可以知道，对于温控系统而言pid.i, pid.d 这两个参数的大小直接被pid.p和Ti,Td的大小所决定
  所以后期按键调节PID的设置只是一种大概的模拟，当固定Ti，Td的大小时，pid.i and pid.d的大小直接被pid.p决定。
3.所以说与其对pid.i,pid.d参数的调整，不如代码中对应到对Ti,Td的直接调整，本系统中为了有直观的变化，我取了前者。

**/ 
static void position_pid_cal(void)
{
	//TIM2 50ms中断一次，count计数变量加1.
	if(pid.count < (pid.t/50))
	{
		return;
	}
	temp.currrent_deviation = temp.target - temp.curret_value;
	pid.p_out = temp.currrent_deviation * pid.p;    //比例输出
	
	temp.history_deviation += temp.currrent_deviation;  
	pid.i = pid.p * (pid.t/pid.i_t) * pid.p;   //pid.i = pid.p * (pid.t/pid.i_t) * pid.p; 更正 
	pid.i_out = temp.history_deviation * pid.i;   //积分输出
	
	temp.differential_deviation = temp.currrent_deviation - temp.pre_currrent_deviation; 
	pid.d = pid.p * (pid.d_t/pid.t);
	pid.d_out = temp.differential_deviation * pid.d;  //微分输出
	
	pid.pid_out = pid.p_out + pid.i_out + pid.d_out;   //总的输出 
	
	/*输出限幅*/
	pid_out_limit();
	
	temp.pre_currrent_deviation = temp.currrent_deviation;  //更新前一次微分偏差 
	
	pid.count = 0;  //清空，重新计数500ms 
	
	/* 有了pid的输出控制pid.pid_out(本系统对应为加热控制时间)，随后执行机构每隔1ms，输出控制一次 */
}
static void position_pid_control(void)
{
	//目标温度15%附近即开启P算法
	if(temp.curret_value >= ((1-0.15) * temp.target))
	{
		position_pid_cal();
	}else
	{
		WARM(ON);
	}
}

/**
  * @brief   增量式PID算法
1.上述位置式pid算法，会将I累加到很大的值，一旦计算机故障就会导致输出大幅度变化，可能造成严重的事故，
  故实际工厂可采用增量式pid算法来避免。

  **/ 
static void increment_pid_cal(void)
{
	float dk1;
	float dk2;
	float t1,t2,t3;
	
	//TIM2 50ms中断一次，count计数变量加1.
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

	pid.increase_out = t1 + t2 + t3;  //增量输出时间长度
	pid.pid_out += pid.increase_out;  //500ms计算一次，将pid_out每过500ms累加一次。
	
	pid_out_limit();
	
	temp.pre2_current_deviation = temp.pre_currrent_deviation;
	temp.pre_currrent_deviation = temp.currrent_deviation;
	
	pid.count = 0;
}
static void increment_pid_control(void)
{
	//目标温度15%附近即开启P算法
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




