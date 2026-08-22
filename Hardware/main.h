#include "stm32f10x.h"                  // Device header


#ifndef __main_H
#define __main_H

#define		PIN_ADXL345_INT0	GPIO_Pin_0	//PA0 加速度计中断引脚0
#define		PIN_ADXL345_INT1	GPIO_Pin_1	//PA1 加速度计中断引脚1
#define		PIN_ADXL345_SCL		GPIO_Pin_6	//PB6 加速度计SCL
#define		PIN_ADXL345_SDA		GPIO_Pin_7	//PB7 加速度计SDA

#define		PIN_TEST_V			GPIO_Pin_2	//PA2 电池电量检测
#define		PIN_TEST_V_SW		GPIO_Pin_9	//PB9 ADC检测电压开关


#define		PIN_SMG_G			GPIO_Pin_5	//PA5     数码管段1~8
#define		PIN_SMG_F			GPIO_Pin_6	//PA6  
#define		PIN_SMG_A			GPIO_Pin_7	//PA7  
#define		PIN_SMG_B			GPIO_Pin_0	//PB0  
#define		PIN_SMG_DP			GPIO_Pin_1	//PB1  
#define		PIN_SMG_C			GPIO_Pin_2	//PB2  
#define		PIN_SMG_D			GPIO_Pin_10	//PB10 
#define		PIN_SMG_E			GPIO_Pin_11	//PB11 
#define		PIN_SMG1_CS			GPIO_Pin_15	//PA15 数码管1片选
#define		PIN_SMG2_CS			GPIO_Pin_3	//PB3  数码管2片选
#define		PIN_SMG3_CS			GPIO_Pin_14	//PB14 数码管3片选
#define		PIN_SMG4_CS			GPIO_Pin_13	//PB13 数码管4片选
#define		PIN_SMG_LED1		GPIO_Pin_11	//PA11 数码管冒号1
#define		PIN_SMG_LED2		GPIO_Pin_12	//PA12 数码管冒号2


#define		PIN_WS2812			GPIO_Pin_8	//PA8   WS2812B,共用数码管总开关

#define		PIN_LANYA_TX		GPIO_Pin_9	//PA9 	串口TX
#define		PIN_LANYA_RX		GPIO_Pin_10	//PA10 	串口RX
#define		PIN_LANYA_CS		GPIO_Pin_15	//PB15  蓝牙开关



extern uint8_t Wake_time;      //唤醒时间
extern uint8_t Wake_gravity;   //唤醒重力
extern uint8_t Wake_gravity2;  //蓝牙唤醒重力
extern uint8_t light_1,light_2;//管亮度和秒针亮度

//PA13 14烧录接口
//8M外部晶振
//32.768KHZ晶振提供RTC时钟

#endif
