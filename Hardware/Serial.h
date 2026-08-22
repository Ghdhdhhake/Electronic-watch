#ifndef __Serial_H
#define __Serial_H

#include <stdio.h>//不加这个使用printf会警告

extern uint8_t Serial_F;		//状态
extern uint8_t Serial_msg[16];	//数据容器


void Serial_Init(void);
void Serial_SendByte(uint8_t BYTE);	//发送一个字节
void Serial_SendArray(uint8_t* Array,uint32_t Len);//发送数组
void Serial_SendString(char* Str);//发送字符串
void Serial_SendNum(uint32_t Num,uint8_t Len);//发送十进制(拆分数字)
void Serial_Printf(char* format,...);//用于多个串口都可以使用printf函数

uint8_t Serial_GetFlag(void);

#endif
