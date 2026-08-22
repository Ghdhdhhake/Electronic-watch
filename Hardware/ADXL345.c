#include "stm32f10x.h"                  // Device header
#include "MyI2C.h"

#define ADXL345_ADDRESS		0xA6


void ADXL345_WriteReg(uint8_t RegAddress, uint8_t Data)
{
	MyI2C_Start();
	MyI2C_SendByte(ADXL345_ADDRESS);
	MyI2C_ReceiveAck();
	MyI2C_SendByte(RegAddress);
	MyI2C_ReceiveAck();
	MyI2C_SendByte(Data);
	MyI2C_ReceiveAck();
	MyI2C_Stop();
}

uint8_t ADXL345_ReadReg(uint8_t RegAddress)
{
	uint8_t Data;
	
	MyI2C_Start();
	MyI2C_SendByte(ADXL345_ADDRESS);
	MyI2C_ReceiveAck();
	MyI2C_SendByte(RegAddress);
	MyI2C_ReceiveAck();
	
	MyI2C_Start();
	MyI2C_SendByte(ADXL345_ADDRESS | 0x01);
	MyI2C_ReceiveAck();
	Data = MyI2C_ReceiveByte();
	MyI2C_SendAck(1);
	MyI2C_Stop();
	
	return Data;
}

#define 	DATA_FORMAT		0x31	//数据格式控制
#define 	BW_RATE			0x2C	//数据速率及功率模式控制
#define 	POWER_CTL		0x2D	//省电特性控制
#define 	INT_ENABLE		0x2E	//中断使能控制
#define 	INT_MAP			0x2F	//中断映射控制

#define 	THRESH_ACT		0x24	//活动阈值;		睡眠时唤醒阈值
#define 	THRESH_INACT	0x25	//不活动阈值;	小于此阈值满足睡眠条件
#define 	TIME_INACT		0x26	//不活动时间阈值;小于此时间且满足睡眠条件进入睡眠
#define 	ACT_INACT_CTL	0x27	//轴使活动和非活动检测的控制
#define 	INT_SOURCE		0x30	//中断源

#define 	OFSX			0x1E	//偏移
#define 	OFSY			0x1F	//偏移
#define 	OFSZ			0x20	//偏移


uint8_t ADXL345_GetINT(void)
{
	uint8_t ret=0;
	ret = ADXL345_ReadReg(INT_SOURCE);
	return ret;
}


void ADXL345_Init(uint8_t Wake_gravity)	//参数：唤醒加速度G
{
	MyI2C_Init();
	
	ADXL345_WriteReg(INT_ENABLE,0x00);		//先关闭中断
	
	ADXL345_WriteReg(DATA_FORMAT,0x0B);  	//高电平中断输出,13位全分辨率,输出数据右对齐,16g量程
	ADXL345_WriteReg(BW_RATE,0x0A);   		//采样率100HZ
	ADXL345_WriteReg(POWER_CTL,0x38);   	//自动休眠模式，休眠时以8HZ的频率采样
		
	//设置轴偏移
	ADXL345_WriteReg(OFSX,0);
	ADXL345_WriteReg(OFSY,0);
	ADXL345_WriteReg(OFSZ,0);
	
	//比例因子为62.5 mg/LSB。如果使能静止中断，值为0时，可能导致工作异常。
	//0x10*62.5=1000mg = 1G加速度 
	ADXL345_WriteReg(THRESH_ACT,Wake_gravity);   	//大于xG唤醒;						睡眠激活activity阈值，当大于这个值的时候唤醒，其中10代表1g  
	ADXL345_WriteReg(THRESH_INACT,0x20);	//小于2G睡眠;						睡眠开始inactivity阈值，当小于这个值的时候睡眠，其中02代表0.2g 
	ADXL345_WriteReg(TIME_INACT,0x01);		//小于2G且时间超过2秒进入睡眠;		当小于inactivity值时间超过这个值的时候进入睡眠，其中02代表2秒
	ADXL345_WriteReg(ACT_INACT_CTL,0xFF);	//直流交流触发配置，XYZ使能触发配置，此处选XYZ交流
	ADXL345_WriteReg(INT_MAP,0xEF);			//中断引脚选择，此处我们将activity映射到INT1,其他中断映射到INT2
	
	ADXL345_WriteReg(INT_ENABLE,0x18);  	//使能DATA_READY中断,开启唤醒中断和睡眠中断
}


void ADXL345_GetData(int16_t *AccX, int16_t *AccY, int16_t *AccZ)
{
	uint8_t DataH, DataL;
	
	DataH = ADXL345_ReadReg(0x33);
	DataL = ADXL345_ReadReg(0x32);
	*AccX = (DataH << 8) | DataL;
	
	DataH = ADXL345_ReadReg(0x35);
	DataL = ADXL345_ReadReg(0x34);
	*AccY = (DataH << 8) | DataL;
	
	DataH = ADXL345_ReadReg(0x37);
	DataL = ADXL345_ReadReg(0x36);
	*AccZ = (DataH << 8) | DataL;
}



