#include "stm32f10x.h"                  // Device header
#include <time.h>

uint16_t g_Time[] = {2026,5,21,16,39,0};

void MyRTC_SetTime(void);
void MyRTC_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP,ENABLE);
	PWR_BackupAccessCmd(ENABLE);	
	
	//如果MPU或VBAT没掉电,则不需要重复初始化,通过BKP配置项来判断
	if(BKP_ReadBackupRegister(BKP_DR1)!=0x1234)
	{
		BKP_WriteBackupRegister(BKP_DR1,0x1234);
		
		RCC_LSEConfig(RCC_LSE_ON);	//配置外部低速振荡器
		while(RCC_GetFlagStatus(RCC_FLAG_LSERDY)!=1);	//等待LSE启动完成
		//选择RTC时钟源
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
		RCC_RTCCLKCmd(ENABLE);	//使能时钟
		
		RTC_WaitForSynchro();	//等待同步
		RTC_WaitForLastTask();	//等待上一次写操作完成
		
		RTC_SetPrescaler(32768-1);	//写配置分频器,分频为1HZ	
		RTC_WaitForLastTask();		//等待上一次写操作完成

		MyRTC_SetTime();//设定时间
	}
	else
	{
		RTC_WaitForSynchro();	//等待同步
		RTC_WaitForLastTask();	//等待上一次写操作完成
	}
}

//设定日期
void MyRTC_SetTime(void)
{
	time_t time_cnt;
	struct tm time_date;
	
	time_date.tm_year = g_Time[0]-1900;
	time_date.tm_mon = g_Time[1]-1;
	time_date.tm_mday = g_Time[2];
	time_date.tm_hour = g_Time[3];
	time_date.tm_min = g_Time[4];
	time_date.tm_sec = g_Time[5];
	time_cnt = mktime(&time_date)-8*60*60; //校准北京时间;不加偏移也可以,只是时间戳和北京时间对应不上
	
	RTC_SetCounter(time_cnt);	//设定时间
	RTC_WaitForLastTask();		//等待上一次写操作完成
	
}
//读取日期
void MyRTC_ReadTime(void)
{
	time_t time_cnt;
	struct tm time_date;
	
	time_cnt = RTC_GetCounter()+8*60*60;	//校准北京时间
	time_date = *localtime(&time_cnt);		//时间戳转日期	
	g_Time[0] = time_date.tm_year+1900;
	g_Time[1] = time_date.tm_mon+1;
	g_Time[2] = time_date.tm_mday;
	g_Time[3] = time_date.tm_hour;
	g_Time[4] = time_date.tm_min;
	g_Time[5] = time_date.tm_sec;
	
}
