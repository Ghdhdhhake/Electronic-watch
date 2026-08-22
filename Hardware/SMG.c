#include "stm32f10x.h"                  // Device header
#include "delay.h"
#include "main.h"

void SMG_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB,ENABLE);
	
	//初始化数码管段
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Pin = PIN_SMG_G | PIN_SMG_F | PIN_SMG_A | PIN_SMG1_CS | PIN_SMG_LED1 | PIN_SMG_LED2;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Pin = PIN_SMG_B | PIN_SMG_DP | PIN_SMG_C | PIN_SMG_D | PIN_SMG_E | PIN_SMG2_CS | PIN_SMG3_CS | PIN_SMG4_CS;
	GPIO_Init(GPIOB,&GPIO_InitStruct);

	GPIO_SetBits(GPIOA,PIN_SMG_LED1 | PIN_SMG_LED2);					//关闭数码管冒号
	GPIO_ResetBits(GPIOA,PIN_SMG1_CS);									//失能数码管片选1
	GPIO_ResetBits(GPIOB,PIN_SMG2_CS | PIN_SMG3_CS | PIN_SMG4_CS);		//失能数码管片选2 3 4
}

//参数:x第几个数码管亮;y显示数字几
uint8_t m_SMG_CE=0;
void SMG_Set(uint8_t x,uint8_t y)
{
	m_SMG_CE=x;
//数码管片选,用pwm调光注释下面代码
	switch(x)
	{
		case 1: GPIO_ResetBits(GPIOA,PIN_SMG1_CS);   GPIO_SetBits(GPIOB,PIN_SMG2_CS);
				GPIO_SetBits(GPIOB,PIN_SMG3_CS); GPIO_SetBits(GPIOB,PIN_SMG4_CS);	break;
		
		case 2: GPIO_SetBits(GPIOA,PIN_SMG1_CS); GPIO_ResetBits(GPIOB,PIN_SMG2_CS);
				GPIO_SetBits(GPIOB,PIN_SMG3_CS); GPIO_SetBits(GPIOB,PIN_SMG4_CS);	break;
		
		case 4:	GPIO_SetBits(GPIOA,PIN_SMG1_CS); GPIO_SetBits(GPIOB,PIN_SMG2_CS);
				GPIO_ResetBits(GPIOB,PIN_SMG3_CS);   GPIO_SetBits(GPIOB,PIN_SMG4_CS);	break;
		
		case 3: GPIO_SetBits(GPIOA,PIN_SMG1_CS); GPIO_SetBits(GPIOB,PIN_SMG2_CS);
				GPIO_SetBits(GPIOB,PIN_SMG3_CS); GPIO_ResetBits(GPIOB,PIN_SMG4_CS);		break;
		
		case 5: GPIO_SetBits(GPIOA,PIN_SMG1_CS); GPIO_SetBits(GPIOB,PIN_SMG2_CS);
				GPIO_SetBits(GPIOB,PIN_SMG3_CS); GPIO_SetBits(GPIOB,PIN_SMG4_CS);		break;
		
	}
	
	//显示数字代码	
	switch(y)
	{
		
		case 0: 	GPIO_SetBits(GPIOA,PIN_SMG_A); 			GPIO_SetBits(GPIOB,PIN_SMG_B); 			GPIO_SetBits(GPIOB,PIN_SMG_C); 			GPIO_SetBits(GPIOB,PIN_SMG_D);
					GPIO_SetBits(GPIOB,PIN_SMG_E); 			GPIO_SetBits(GPIOA,PIN_SMG_F); 			GPIO_ResetBits(GPIOA,PIN_SMG_G); 		GPIO_ResetBits(GPIOB,PIN_SMG_DP); break;
		
		case 1: 	GPIO_ResetBits(GPIOA,PIN_SMG_A); 		GPIO_SetBits(GPIOB,PIN_SMG_B); 			GPIO_SetBits(GPIOB,PIN_SMG_C); 			GPIO_ResetBits(GPIOB,PIN_SMG_D);
					GPIO_ResetBits(GPIOB,PIN_SMG_E); 		GPIO_ResetBits(GPIOA,PIN_SMG_F); 		GPIO_ResetBits(GPIOA,PIN_SMG_G); 		GPIO_ResetBits(GPIOB,PIN_SMG_DP); break;
		
		case 2: 	GPIO_SetBits(GPIOA,PIN_SMG_A); 			GPIO_SetBits(GPIOB,PIN_SMG_B); 			GPIO_ResetBits(GPIOB,PIN_SMG_C); 		GPIO_SetBits(GPIOB,PIN_SMG_D);
					GPIO_SetBits(GPIOB,PIN_SMG_E); 			GPIO_ResetBits(GPIOA,PIN_SMG_F); 		GPIO_SetBits(GPIOA,PIN_SMG_G); 			GPIO_ResetBits(GPIOB,PIN_SMG_DP); break;
		
		case 3: 	GPIO_SetBits(GPIOA,PIN_SMG_A); 			GPIO_SetBits(GPIOB,PIN_SMG_B); 			GPIO_SetBits(GPIOB,PIN_SMG_C); 			GPIO_SetBits(GPIOB,PIN_SMG_D);
					GPIO_ResetBits(GPIOB,PIN_SMG_E); 		GPIO_ResetBits(GPIOA,PIN_SMG_F); 		GPIO_SetBits(GPIOA,PIN_SMG_G); 			GPIO_ResetBits(GPIOB,PIN_SMG_DP); break;
		
		case 4: 	GPIO_ResetBits(GPIOA,PIN_SMG_A); 		GPIO_SetBits(GPIOB,PIN_SMG_B); 			GPIO_SetBits(GPIOB,PIN_SMG_C); 			GPIO_ResetBits(GPIOB,PIN_SMG_D);
					GPIO_ResetBits(GPIOB,PIN_SMG_E); 		GPIO_SetBits(GPIOA,PIN_SMG_F); 			GPIO_SetBits(GPIOA,PIN_SMG_G); 			GPIO_ResetBits(GPIOB,PIN_SMG_DP); break;
		
		case 5: 	GPIO_SetBits(GPIOA,PIN_SMG_A); 			GPIO_ResetBits(GPIOB,PIN_SMG_B); 		GPIO_SetBits(GPIOB,PIN_SMG_C); 			GPIO_SetBits(GPIOB,PIN_SMG_D);
					GPIO_ResetBits(GPIOB,PIN_SMG_E); 		GPIO_SetBits(GPIOA,PIN_SMG_F); 			GPIO_SetBits(GPIOA,PIN_SMG_G); 			GPIO_ResetBits(GPIOB,PIN_SMG_DP); break;
		
		case 6: 	GPIO_SetBits(GPIOA,PIN_SMG_A); 			GPIO_ResetBits(GPIOB,PIN_SMG_B); 		GPIO_SetBits(GPIOB,PIN_SMG_C); 			GPIO_SetBits(GPIOB,PIN_SMG_D);
					GPIO_SetBits(GPIOB,PIN_SMG_E); 			GPIO_SetBits(GPIOA,PIN_SMG_F); 			GPIO_SetBits(GPIOA,PIN_SMG_G); 			GPIO_ResetBits(GPIOB,PIN_SMG_DP); break;
		
		case 7: 	GPIO_SetBits(GPIOA,PIN_SMG_A); 			GPIO_SetBits(GPIOB,PIN_SMG_B); 			GPIO_SetBits(GPIOB,PIN_SMG_C); 			GPIO_ResetBits(GPIOB,PIN_SMG_D);
					GPIO_ResetBits(GPIOB,PIN_SMG_E); 		GPIO_ResetBits(GPIOA,PIN_SMG_F); 		GPIO_ResetBits(GPIOA,PIN_SMG_G); 		GPIO_ResetBits(GPIOB,PIN_SMG_DP); break;
		
		case 8: 	GPIO_SetBits(GPIOA,PIN_SMG_A); 			GPIO_SetBits(GPIOB,PIN_SMG_B); 			GPIO_SetBits(GPIOB,PIN_SMG_C); 			GPIO_SetBits(GPIOB,PIN_SMG_D);
					GPIO_SetBits(GPIOB,PIN_SMG_E); 			GPIO_SetBits(GPIOA,PIN_SMG_F); 			GPIO_SetBits(GPIOA,PIN_SMG_G); 			GPIO_ResetBits(GPIOB,PIN_SMG_DP); break;
					
		case 9: 	GPIO_SetBits(GPIOA,PIN_SMG_A); 			GPIO_SetBits(GPIOB,PIN_SMG_B); 			GPIO_SetBits(GPIOB,PIN_SMG_C); 			GPIO_SetBits(GPIOB,PIN_SMG_D);
					GPIO_ResetBits(GPIOB,PIN_SMG_E); 		GPIO_SetBits(GPIOA,PIN_SMG_F); 			GPIO_SetBits(GPIOA,PIN_SMG_G); 			GPIO_ResetBits(GPIOB,PIN_SMG_DP); break;
		
		case 10: 	GPIO_ResetBits(GPIOA,PIN_SMG_A); 		GPIO_ResetBits(GPIOB,PIN_SMG_B); 		GPIO_ResetBits(GPIOB,PIN_SMG_C); 		GPIO_ResetBits(GPIOB,PIN_SMG_D);
					GPIO_ResetBits(GPIOB,PIN_SMG_E); 		GPIO_ResetBits(GPIOA,PIN_SMG_F); 		GPIO_ResetBits(GPIOA,PIN_SMG_G); 		GPIO_SetBits(GPIOB,PIN_SMG_DP);   break;

	}	

	delay_ms(1);
	
}

void SMG_miao(uint8_t x)
{
	GPIO_WriteBit(GPIOA,PIN_SMG_LED1,(BitAction)x);
	GPIO_WriteBit(GPIOA,PIN_SMG_LED2,(BitAction)x);
}
