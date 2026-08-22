#include "stm32f10x.h"                  // Device header
#include "main.h"

uint16_t AD_Value[1];

void AD_Init(void)
{
	//开启ADC时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	//dma是AHB总线设备!!
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
	//设置ADC分频
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);//ADCCLK=72MHz/6=12MHz
	//设置GPIOA
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;//模拟输入
	GPIO_InitStruct.GPIO_Pin = PIN_TEST_V;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	

	//选择规则组通道,通道采样周期55.5,转换周期固定12.5,共68周期. 1/12MHZ*68=5.6us
	ADC_RegularChannelConfig(ADC1,ADC_Channel_2,1,ADC_SampleTime_55Cycles5);	

	//ADC结构体初始化
	ADC_InitTypeDef ADC_InitStruct;
	ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;//独立模式
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;//数据右对齐
	ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//触发源:软件触发或硬件触发
	ADC_InitStruct.ADC_ContinuousConvMode = ENABLE;//单次扫描或连续扫描
	ADC_InitStruct.ADC_ScanConvMode = ENABLE;//多通道模式
	ADC_InitStruct.ADC_NbrOfChannel = 1;//用了几个通道
	ADC_Init(ADC1,&ADC_InitStruct);
	
	//定义DMA
	DMA_InitTypeDef DMA_InitStruct;
	DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;	//外设站点的基地址
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;	//存储宽度,16位
	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//地址是否自增
	DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)AD_Value;	//存储器站点的基地址
	DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;	//存储宽度
	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;	//地址是否自增,存储器的地址是自增的
	DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;	//外设站点数据向存储器站点转移数据
	DMA_InitStruct.DMA_BufferSize = 1;	//传输次数
	DMA_InitStruct.DMA_Mode = DMA_Mode_Circular;	//是否自动重装,警告:自动重装和软件触发不能同时使用!!否则就会连续触发停不下来.
	DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;	//开启硬件触发模式
	DMA_InitStruct.DMA_Priority = DMA_Priority_Medium;	//优先级	
	DMA_Init(DMA1_Channel1,&DMA_InitStruct);//ADC1的硬件触发是接在了DMA1的通道1上.所以不能随便选择ADC
	
	DMA_Cmd(DMA1_Channel1,ENABLE);//使能DMA
	ADC_DMACmd(ADC1,ENABLE);//开启ADC_DMA触发信号
	ADC_Cmd(ADC1,ENABLE);//开启ADC
	//ADC校准
	ADC_ResetCalibration(ADC1);//复位ADC
	//返回复位校准的状态,返回0复位成功
	while(ADC_GetResetCalibrationStatus(ADC1)==1);
	//启动校准,内部电路自动校准
	ADC_StartCalibration(ADC1);
	//获取校准状态,返回0校准成功
	while(ADC_GetCalibrationStatus(ADC1)==1);
	ADC_SoftwareStartConvCmd(ADC1,ENABLE);//adc开始触发转换
}

void AD_GetValue(void)
{	
	DMA_Cmd(DMA1_Channel1,DISABLE);//失能DMA
	DMA_SetCurrDataCounter(DMA1_Channel1,1);//定义传输次数
	DMA_Cmd(DMA1_Channel1,ENABLE);//失能DMA
	
	ADC_SoftwareStartConvCmd(ADC1,ENABLE);//adc开始触发
	
	while(DMA_GetFlagStatus(DMA1_FLAG_TC1)==RESET);
	DMA_ClearFlag(DMA1_FLAG_TC1);
}

