#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <stdarg.h>					//用于多个串口都可以使用printf函数
#include "main.h"


void Serial_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	USART_InitTypeDef USART_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin = PIN_LANYA_TX;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Pin = PIN_LANYA_RX;
	GPIO_Init(GPIOA,&GPIO_InitStruct);	
	
	USART_InitStruct.USART_BaudRate = 9600;
	USART_InitStruct.USART_HardwareFlowControl = DISABLE;//不用硬件流控制
	USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStruct.USART_Parity = USART_Parity_No;//奇数偶数校验,不校验
	USART_InitStruct.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;//传输8bit
	USART_Init(USART1,&USART_InitStruct);
	
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);//接收寄存器非空中断

	NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&NVIC_InitStruct);
	
	USART_Cmd(USART1,ENABLE);
}



//中断接收函数

/*
定义数据格式:
包头 0x55,0x55
数据长度 0x10
数据
*/
uint8_t Serial_F=0;		//状态
uint8_t Serial_len=0;	//数据长度
uint8_t Serial_msg[16];	//数据容器
uint8_t Serial_index=0;	//游标

void USART1_IRQHandler(void)
{
	uint8_t msg;
	
	if(USART_GetITStatus(USART1,USART_IT_RXNE)==SET)//接收寄存器非空中断
		{		
			msg = USART_ReceiveData(USART1);
			
			if(Serial_F==0)
			{
				if(msg==0x55)  Serial_F=1;			//获取包头1
				else 		   Serial_F=0;  		//进入空闲模式
			}
			else if(Serial_F==1)
			{
				if(msg==0x55)  Serial_F=2;			//获取包头2
				else 		   Serial_F=0;  		//进入空闲模式
			}
			else if(Serial_F==2)
			{
				Serial_len=msg; Serial_F=3;			//获取数据长度
			}
			else if(Serial_F==3)
			{
				if(Serial_index<Serial_len) 
				{
					Serial_msg[Serial_index]=msg;
					Serial_index++; 
					if(Serial_index==Serial_len)	//待处理数据标志位,此时数据接收完成,数组游标归零
					{
						Serial_index=0;
						Serial_F=4;
					}
				}
			}
			
			USART_ClearITPendingBit(USART1,USART_IT_RXNE);
		}
}


//取标志位状态
uint8_t Serial_GetFlag(void)
{
	if(Serial_F==4)
	{
		Serial_F=0;	//进入空闲模式
		return 4;
	}
	return 0;
}


//发送一个字节
void Serial_SendByte(uint8_t BYTE)
{
	//执行发送函数,即清除USART_FLAG_TXE标志位
	USART_SendData(USART1,BYTE);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==0);//数据是否已经被移位标志位
}


//发送数组
void Serial_SendArray(uint8_t* Array,uint32_t Len)
{
	uint32_t i;
	for(i=0;i<Len;i++)
	{
		Serial_SendByte(Array[i]);
	}
}

//发送字符串
void Serial_SendString(char* Str)
{
	uint32_t i=0;//初始值必须给0,否则while不能发送数据
	while(Str[i]!=0)
	{
		Serial_SendByte(Str[i]);
		i++;	
	}
}

//求X的Y次方
uint32_t GetPow(uint32_t x,uint32_t y)
{
	uint32_t t=1;//给个初始值
	while(y--)
	{
		t=t*x;
	}
	return t;
}

//发送十进制(拆分数字),数字/x次方%10
void Serial_SendNum(uint32_t Num,uint8_t Len)
{
	uint8_t i;
	for(i=0;i<Len;i++)
	{
		//字符形式结尾要加0x30或'0'
		Serial_SendByte(Num / GetPow(10,Len-i-1)%10 +'0');
	}
}



/*----------------------------------------------------------------*/

//printf重定向打印串口上,fputc是printf的底层
//printf重定向串口1了,串口2是无法使用printf的,详情见38分处教程 https://www.bilibili.com/video/BV1th411z7sn?p=24&vd_source=abd6d7e98b58c8bc43743d856605a6de
//如果只用一个串口的话调用此函数即可
int fputc(int ch,FILE* f)
{
	Serial_SendByte(ch);
	return ch;
}

//用于多个串口都可以使用printf函数
void Serial_Printf(char* format,...)
{
	char String[100];
	va_list arg;
	va_start(arg,format);
	vsprintf(String,format,arg);
	va_end(arg);
	Serial_SendString(String);
}


