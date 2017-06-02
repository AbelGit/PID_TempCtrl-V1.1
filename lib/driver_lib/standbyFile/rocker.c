
// PC14, 15 是摇杆上的按键的EXTI端口， 在key_exti.c文件中有外部中断的中断服务函数
/*
left rocker key_press  add throttle
left rocker to up： change > 2.0   plane_de_armed 
left rocker to down: change > 2.0   plane_armed
left rocker to left: yaw change
left rocker to right: yaw change 

right rocker key_press  decline throttle
right rocker to up： change > 1.0   pitch change 
right rocker to down: change > 1.0   pitch change
right rocker to left: change > 1.0  roll change
right rocker to right: change > 1.0  roll change 
*/

#include <stdio.h>
#include "stm32f10x.h"
#include "rocker.h"
#include "driver.h"

#define CH_NUM  4           //要使用的ADC通道数
#define COUNT 30
#define N 30   //滤波采样次数

static u16 ad_value[COUNT][CH_NUM]={0};  //AD 采样值
static u16 after_filter[4] = {0};

//-----------------------------------按键--------------------------------------- 

static void Rocker_EXTI_Key_GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = ROCKER_KEY_EXTI_PIN1 | ROCKER_KEY_EXTI_PIN2;       
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	  //key已经上拉 区别于IPD
	GPIO_Init(ROCKER_KEY_EXTI_PORT1, &GPIO_InitStructure);

	GPIO_EXTILineConfig(ROCKER_KEY_EXTI_LINE_PORT1, ROCKER_KEY_EXTI_LINE_PIN1);  
	GPIO_EXTILineConfig(ROCKER_KEY_EXTI_LINE_PORT1, ROCKER_KEY_EXTI_LINE_PIN2);  
}

void Rocker_EXTI_Key_init(void)
{	
	EXTI_InitTypeDef EXTI_InitStructure;												

	RCC_APB2PeriphClockCmd(ROCKER_KEY_EXTI_RCC_PORT | RCC_APB2Periph_AFIO, ENABLE);	
	
  	Rocker_EXTI_Key_GPIO_Configuration();

	EXTI_InitStructure.EXTI_Line = ROCKER_KEY_EXTI_LINE1 | ROCKER_KEY_EXTI_LINE2;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;     
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; 
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;               
	
	EXTI_Init(&EXTI_InitStructure); 
}

// ----------------------------------摇杆 --------------------------------------

//同时获取4个通道的数据
//PC 0,1,2,3  ADC1_IN10 / ADC1_IN11 / ADC1_IN12 / ADC1_IN13

/** @date			2013/8/28
    @author			Mei Jilin
	@brief		    ADC1的配置完成3通道的数据采集
    @attention
       1 ----------- adc_config();                     //配置adc
       2 ----------- adc_start();                      //启动adc,开始转换
       3 ----------- get_ChannelValue(ADC_Channel_x);  //读取相应通道的adc值
**/

/** @para none
  * @ret  none
  * @brief adc对应的gpio初始化
  */
static void rocker_adc_gpio_init(void)
{
    GPIO_InitTypeDef gpio;
    RCC_APB2PeriphClockCmd(ROCKER_ADC_RCC_PORT1, ENABLE);
    gpio.GPIO_Mode = GPIO_Mode_AIN;                         //配置为模拟输入
    gpio.GPIO_Pin = ROCKER_ADC_PIN1 | ROCKER_ADC_PIN2 | ROCKER_ADC_PIN3 | ROCKER_ADC_PIN4;   //ADC1 --channel0,11,12,13
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(ROCKER_ADC_PORT, &gpio); 
}



/** @para none
  * @ret  none
  * @brief 配置dma
  */
static void rocker_adc_dma_init()
{
    DMA_InitTypeDef dma;
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    
    DMA_DeInit(DMA1_Channel1);
    dma.DMA_PeripheralBaseAddr  = (u32)&(ADC1->DR);     //DMA对应的外设基地址
    dma.DMA_MemoryBaseAddr      = (u32)&ad_value;       //内存存储基地址
    dma.DMA_DIR                 = DMA_DIR_PeripheralSRC;//DMA的转换模式为SRC模式，由外设搬移到内存
    dma.DMA_M2M                 = DMA_M2M_Disable;      //M2M模式禁用
    dma.DMA_PeripheralDataSize  = DMA_PeripheralDataSize_HalfWord;//定义外设数据宽度为16位
    dma.DMA_MemoryDataSize      = DMA_MemoryDataSize_HalfWord;  //DMA搬移数据尺寸，HalfWord就是为16位    
    dma.DMA_BufferSize          = 120;                       //DMA缓存大小CH_NUM个
    dma.DMA_MemoryInc           = DMA_MemoryInc_Enable;         //接收一次数据后，目标内存地址后移
    dma.DMA_PeripheralInc       = DMA_PeripheralInc_Disable;    //接收一次数据后，设备地址禁止后移
    dma.DMA_Mode                = DMA_Mode_Circular;            //转换模式，循环缓存模式。
    dma.DMA_Priority            = DMA_Priority_High;            //DMA优先级高
    DMA_Init(DMA1_Channel1, &dma); 
	DMA_Cmd(DMA1_Channel1,ENABLE);

	DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE); //DMA传输完成中断
}


/** @para none
  * @ret  none
  * @brief adc配置
  */
static void rocker_adc_config(void)
{
    ADC_InitTypeDef adc;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    RCC_ADCCLKConfig(RCC_PCLK2_Div8);                       //设置ad时钟为72/6=12M,ad最大时钟频率为14M
    
    ADC_DeInit(ADC1);
    adc.ADC_Mode                = ADC_Mode_Independent;     //工作在独立模式
    adc.ADC_ContinuousConvMode  = ENABLE;                   //连续转换模式
    adc.ADC_DataAlign           = ADC_DataAlign_Right;      //数据右对齐
    adc.ADC_ExternalTrigConv    = ADC_ExternalTrigConv_None;//不使用外部触发
    adc.ADC_NbrOfChannel        = CH_NUM ;                       //转换4个通道
    adc.ADC_ScanConvMode        = ENABLE;                   //连续转换模式
    ADC_Init(ADC1,&adc);
    //设定指定的ADC通道组,设置转化顺序和采样时间
    ADC_RegularChannelConfig(ADC1, ROCKER_ADC_CHANNEL1, 1, ADC_SampleTime_239Cycles5);
    ADC_RegularChannelConfig(ADC1, ROCKER_ADC_CHANNEL2, 2, ADC_SampleTime_239Cycles5);
    ADC_RegularChannelConfig(ADC1, ROCKER_ADC_CHANNEL3, 3, ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1, ROCKER_ADC_CHANNEL4, 4, ADC_SampleTime_239Cycles5);
    
	//开启adc的DMA
    ADC_DMACmd(ADC1,ENABLE);
    //使能ADC1
    ADC_Cmd(ADC1, ENABLE);
    /* Enable ADC1 reset calibaration register */   
    ADC_ResetCalibration(ADC1);	  //重新校准
    /* Check the end of ADC1 reset calibration register */
    while(ADC_GetResetCalibrationStatus(ADC1));  //等待重新校准完成
    /* Start ADC1 calibaration */
    ADC_StartCalibration(ADC1);		//开始校准
    /* Check the end of ADC1 calibration */
    while(ADC_GetCalibrationStatus(ADC1));	   //等待校准完成

    ADC_SoftwareStartConvCmd(ADC1, ENABLE);	//连续转换开始，ADC通过DMA方式不断的更新RAM区。 
}


/** @para none
  * @ret  none
  * @brief 完成adc配置
  */
void rocker_adc_init(void)
{
    rocker_adc_gpio_init();
    rocker_adc_dma_init();
    rocker_adc_config();
}

//---------------------------DMA中断中段中对数据滤波 ---------------------------
void filter(void)  //均值滤波 
{
	int sum = 0;
	u8 count, i;
	for(i = 0; i < 4; i++)
	{
		for (count = 0; count < N; count++)
		{
			sum += ad_value[count][i];
		}
		after_filter[i] = sum/N;
		sum = 0;
	}
}

void DMA1_Channel1_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1_IT_TC1) != RESET)
	{
		filter();
		DMA_ClearITPendingBit(DMA1_IT_TC1);
	}
}	


//--------------------------------滤波之后的数据--------------------------------
/** @para ADC_Channel --- (ADC_Channel0 ~ ADC_Channel17)
  * @ret  ADC_Channel_x 的ADC值   rocker 获取的flaot数据 %1.1f才可以使用 
  */
float rocker_get_adc_val(u8 channel)
{
	float ret = 0.0f;

    switch(channel)
    {
        case ROCKER_ADC_CHANNEL1: ret = (float)3.3 * after_filter[0]/4096;  break;  //max: 4095   min: 0   armed  
        case ROCKER_ADC_CHANNEL2: ret = (float)3.3 * after_filter[1]/4096;  break;  //max: 4095   min: 0   yaw
        case ROCKER_ADC_CHANNEL3: ret = (float)3.3 * after_filter[2]/4096;  break;  //max: 4095   min: 0   pitch
		case ROCKER_ADC_CHANNEL4: ret = (float)3.3 * after_filter[3]/4096;  break;  //max: 4095   min: 0   roll
    }
    return ret;
}




























