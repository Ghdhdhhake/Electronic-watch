#include "stm32f10x.h"                  // Device header
#include "main.h"
#include "delay.h"


//WS2812,写好了,没应用上
//		WS2812_Set(0xff0000);
//		WS2812_arr2pwm();
//		delay_ms(500);
//		WS2812_Set(0x00ff00);
//		WS2812_arr2pwm();
//		delay_ms(500);
//		WS2812_Set(0x0000ff);
//		WS2812_arr2pwm();
//		delay_ms(500);
		

#define LED_NUM 2	//灯珠数量
uint16_t WS2812_PWM[LED_NUM*24+1];	//此函数存储的是PWM比较值30或60,30=低电平,60=高电平
uint32_t WS2812_Data[LED_NUM];		//24位颜色数组,红 绿 蓝
uint8_t  DMA_F=0;					//DMA传输完成中断标志位

//设置颜色到数组
void WS2812_Set(uint32_t x)
{	
	uint8_t i;
	for(i=0;i<LED_NUM;i++)
	{
		WS2812_Data[i] = x;
	}
}

//将颜色数组转换为pwm
void WS2812_arr2pwm(void)
{
	uint8_t i,j;
	for(i=0;i<LED_NUM;i++)
	{
		for(j=0;j<24;j++)
		{
			if(WS2812_Data[i]&(0x800000>>j)) 
			{
				WS2812_PWM[i*24+j+1] = 60;	
			}else
			{
				WS2812_PWM[i*24+j+1] = 30;	
			}
			 
		}
	}
	
	//开启DMA
	DMA_Cmd(DMA1_Channel5,DISABLE);
	DMA_SetCurrDataCounter(DMA1_Channel5,24*LED_NUM+1);	//指定传输次数
	DMA_Cmd(DMA1_Channel5,ENABLE);
	//开启定时器
	TIM_Cmd(TIM1,ENABLE);
	//等待DMA传输完毕
	while(DMA_F==0);
	DMA_F=0;//传输完成标志位,和DMA中断函数配合使用
	//delay_us(70);
	
}


void WS2812_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);	
	
	GPIO_InitTypeDef  GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin = PIN_WS2812;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	//初始化定时器;系统主频72MHZ
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period = 90-1;		
	TIM_TimeBaseInitStruct.TIM_Prescaler = 1-1;		//频率800KHZ
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM1,&TIM_TimeBaseInitStruct);
	//初始化PWM,空闲时为低电平
	TIM_OCInitTypeDef TIM_OCInitStruct;
	TIM_OCStructInit(&TIM_OCInitStruct);
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStruct.TIM_Pulse = 0;
	TIM_OC1Init(TIM1,&TIM_OCInitStruct);
	
	//初始化定时器MDA
	TIM_DMAConfig(TIM1, TIM_DMABase_CCR1, TIM_DMABurstLength_1Transfer);
	TIM_DMACmd(TIM1,TIM_DMA_Update,ENABLE);
	TIM_CtrlPWMOutputs(TIM1,ENABLE);	//使用外部时钟
	//失能定时器
	TIM_Cmd(TIM1,DISABLE);
}


void DMA1_Init(void)
{
	DMA_InitTypeDef DMA_InitStruct;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);	
	
	DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)&WS2812_PWM;
	DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&(TIM1->CCR1);
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStruct.DMA_BufferSize = 0;				//传输次数
	DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralDST;	//内存往外设传输
	DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;
	DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;		//不需要循环模式
	DMA_InitStruct.DMA_Priority = DMA_Priority_Medium;	
	DMA_Init(DMA1_Channel5, &DMA_InitStruct);		//DMA1_通道5
	
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = DMA1_Channel5_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 3;
	NVIC_Init(&NVIC_InitStruct);
	
	DMA_ITConfig(DMA1_Channel5,DMA_IT_TC,ENABLE);	//启动DMA中断
	DMA_Cmd(DMA1_Channel5,ENABLE);					//DMA1_通道5
}

void DMA1_Channel5_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1_IT_TC5)==1)//DMA1通道5的传输完成标志位
	{
		
		TIM_SetCompare1(TIM1,0);//清零比较值
		TIM_Cmd(TIM1,DISABLE);	//失能定时器
		DMA_F=1;
		
		DMA_ClearITPendingBit(DMA1_IT_TC5);
	}
}

