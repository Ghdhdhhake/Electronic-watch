#include "stm32f10x.h"                  // Device header
#include "main.h"
#include "SMG.h"

void TIM2_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period = 10-1;
	TIM_TimeBaseInitStruct.TIM_Prescaler = 72-1;
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStruct);
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
	
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&NVIC_InitStruct);
	
	TIM_Cmd(TIM2,ENABLE);
}




uint32_t Count_us=0;		//10us自增
uint32_t TIM2_Num[3];		//每1ms计数1次
uint32_t TIM2_Pwm[3];		//秒针pwm;保险丝发光管pwm;范围0~30
uint32_t TIM2_Comp[3];		//秒针比较值;发光管比较值

//10us中断一次
void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2,TIM_IT_Update)==1)
	{
		//模拟pwm 数码管秒针
		TIM2_Comp[0]++;
		TIM2_Comp[0]%=255;
		if(TIM2_Pwm[0]>TIM2_Comp[0]) 	GPIO_WriteBit(GPIOA,PIN_SMG_LED1,(BitAction)0),GPIO_WriteBit(GPIOA,PIN_SMG_LED2,(BitAction)0);	//亮
		else							GPIO_WriteBit(GPIOA,PIN_SMG_LED1,(BitAction)1),GPIO_WriteBit(GPIOA,PIN_SMG_LED2,(BitAction)1);	//灭	
		//模拟pwm 保险丝发光管
		TIM2_Comp[1]++;
		TIM2_Comp[1]%=255;
		if(TIM2_Pwm[1]>TIM2_Comp[1])	GPIO_SetBits(GPIOB,PIN_TEST_V_SW);		//亮
		else							GPIO_ResetBits(GPIOB,PIN_TEST_V_SW);	//灭
		
		//毫秒计数
		Count_us++;
		if(Count_us>=100)
		{
			Count_us=0;
			TIM2_Num[0]++;
			TIM2_Num[1]++;
			TIM2_Num[2]++;
		}	
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
	}
}



