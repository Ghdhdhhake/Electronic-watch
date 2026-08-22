#include "stm32f10x.h"                  // Device header
#include "delay.h"
#include "main.h"

//1024*页数(c8t6页范围0~63)即是页的起始地址
//每页大小1k=1024个字节
#define STARTADDR 	0x8007800	//金角大王：为了兼容C6T6我就从第30页起始地址

uint16_t m_Data[100];

void WriteFlashData(uint32_t addr,uint16_t* data,uint32_t num)
{
	uint16_t a = 0;	//标志位
	uint32_t j;
	
	FLASH_Unlock();		//解锁
	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);	//清除所有标志位
	a = FLASH_ErasePage(STARTADDR);	//擦除整页
	j=0;
	while(a!=FLASH_COMPLETE)
	{
		delay_us(1);j++;
		if(j>1000)break;			//等待1毫秒
	}
	if(a == FLASH_COMPLETE)			//Flash操作完成
	{
		for(uint32_t i=0;i<num;i++)
		{
			FLASH_ProgramHalfWord(STARTADDR + addr + (i*2),data[i]);	//写2个字节后地址加2
			//FLASH_ProgramOptionByteData(STARTADDR + addr + (i*2),data[i]);
		}
	}
	FLASH_Lock();		//锁
}	


uint8_t ReadFlashData(uint32_t addr)
{
	return  *(uint8_t*)(STARTADDR + addr);
}


void Data_Save(void)     //保存数据
{
	m_Data[0] = (uint16_t)0x03; 		 	//版本
	m_Data[1] = (uint16_t)Wake_time;    	//唤醒时间
	m_Data[2] = (uint16_t)Wake_gravity;  	//唤醒重力
	m_Data[3] = (uint16_t)Wake_gravity2;  	//蓝牙唤醒重力
	m_Data[4] = (uint16_t)light_1;    		//保险丝管亮度
	m_Data[5] = (uint16_t)light_2;  		//秒针led亮度
	WriteFlashData(0,m_Data,100);
}


void Data_Read(void)     //读取数据
{
	uint8_t Ver;
	uint8_t hh,ll;
	uint16_t j=0;	//游标
	
	//版本
	ll=ReadFlashData(j);j++;
	hh=ReadFlashData(j);j++;
    Ver = ll;        
	
	//唤醒时间
	ll=ReadFlashData(j);j++;
	hh=ReadFlashData(j);j++;
	Wake_time = ll;       
	if(Wake_time<3)Wake_time=5;
	
	//唤醒重力
	ll=ReadFlashData(j);j++;
	hh=ReadFlashData(j);j++;
	Wake_gravity = ll;    
	if(Wake_gravity<1 || Wake_gravity>9)Wake_gravity=4;	
	
	//蓝牙唤醒重力
	ll=ReadFlashData(j);j++;
	hh=ReadFlashData(j);j++;
	Wake_gravity2 = ll;    
	if(Wake_gravity2<1 || Wake_gravity2>9)Wake_gravity2=4;
	
	//保险丝管亮度
	ll=ReadFlashData(j);j++;
	hh=ReadFlashData(j);j++;
	light_1 = ll; 
	
	//秒针led亮度
	ll=ReadFlashData(j);j++;
	hh=ReadFlashData(j);j++;
	light_2 = ll+(hh*0); 
	
	//如果版本不一致初始化配置
	if(Ver!=0x03)	
	{
		Wake_time=5;	//默认唤醒5秒
		Wake_gravity=4;	//默认加度4G唤醒
		Wake_gravity2=4;//蓝牙默认加度4G唤醒
		light_1=30;
		light_2=30;
		Data_Save();
	}

}





/*
1.解锁Flash写保护
2.清除Flash所有标志
3.擦除Flash数据
4.写入Flash数据
5.重新锁定Flash写保护

	FLASH_Unlock();					//锁
	FLASH_Lock();					//解锁
	FLASH_ClearFlag();				//清除标志位
	FLASH_ErasePage();				//擦除页
	//FLASH_EraseAllBank1Pages();	//擦除所有Bank1闪存页面。
	FLASH_EraseAllPages();			//擦除全部Flash页面
	
	FLASH_ProgramWord();			//写一个字 32位
	FLASH_ProgramHalfWord();		//写半个字 16位
	FLASH_ProgramOptionByteData();	//写1个字节 8位

	注：这里需要说明，32 位字节写入实际上是写入的两次 16 位数据，
	写完第一次后地址+2，这与我们前面讲解的 STM32 闪存的编程每次必须写入 16 位并不矛盾。
	写入 8位实际也是占用的两个地址了，跟写入 16 位基本上没啥区别。

	
	FLASH_GetStatus();				//获取Flash状态

	typedef enum {
    FLASH_BUSY = 1,       //忙
    FLASH_ERROR_PG,       //编程错误
    FLASH_ERROR_WRP,      //写保护错误
    FLASH_COMPLETE,       //操作完成
    FLASH_TIMEOUT         //操作超时	
	
	}FLASH_Status;


	FLASH_WaitForLastOperation();	//等待上次操作完成,或者出现FLASH_WaitForLastOperation 操作超时
	在写操作时,不能进行读操作,所以要等待上次执行完成.

*/

