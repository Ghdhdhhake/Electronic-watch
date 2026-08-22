#include "stm32f10x.h"                  // Device header
#include "main.h"

void ON_OFF_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO,ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);	//关闭JTD调试引脚
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Pin = PIN_TEST_V_SW | PIN_LANYA_CS;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	GPIO_ResetBits(GPIOB,PIN_TEST_V_SW);						//关闭ADC检测总开关
	GPIO_SetBits(GPIOB,PIN_LANYA_CS);							//关闭蓝牙总开关
}


