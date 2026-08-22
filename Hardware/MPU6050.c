#include "stm32f10x.h"
#include "Delay.h"
#include "MPU6050.h"

#define MPU6050_SCL GPIO_Pin_10
#define MPU6050_SDA GPIO_Pin_11
#define MPU6050_ADDR_W 0xD0
#define MPU6050_ADDR_R 0xD1

static void MPU_W_SCL(uint8_t value) { GPIO_WriteBit(GPIOB, MPU6050_SCL, (BitAction)value); delay_us(5); }
static void MPU_W_SDA(uint8_t value) { GPIO_WriteBit(GPIOB, MPU6050_SDA, (BitAction)value); delay_us(5); }
static uint8_t MPU_R_SDA(void) { uint8_t value = GPIO_ReadInputDataBit(GPIOB, MPU6050_SDA); delay_us(5); return value; }
static void MPU_Start(void) { MPU_W_SDA(1); MPU_W_SCL(1); MPU_W_SDA(0); MPU_W_SCL(0); }
static void MPU_Stop(void) { MPU_W_SDA(0); MPU_W_SCL(1); MPU_W_SDA(1); }

static uint8_t MPU_SendByte(uint8_t value)
{
	uint8_t i, ack;
	for (i = 0; i < 8; i++)
	{
		MPU_W_SDA((value & (0x80 >> i)) != 0);
		MPU_W_SCL(1); MPU_W_SCL(0);
	}
	MPU_W_SDA(1); MPU_W_SCL(1); ack = (uint8_t)!MPU_R_SDA(); MPU_W_SCL(0);
	return ack;
}

static uint8_t MPU_ReadByte(uint8_t ack)
{
	uint8_t i, value = 0;
	MPU_W_SDA(1);
	for (i = 0; i < 8; i++)
	{
		MPU_W_SCL(1);
		if (MPU_R_SDA()) value |= (uint8_t)(0x80 >> i);
		MPU_W_SCL(0);
	}
	MPU_W_SDA(ack ? 0 : 1); MPU_W_SCL(1); MPU_W_SCL(0);
	return value;
}

static uint8_t MPU_WriteReg(uint8_t reg, uint8_t value)
{
	uint8_t ack = 0;
	MPU_Start();
	if (MPU_SendByte(MPU6050_ADDR_W) && MPU_SendByte(reg) && MPU_SendByte(value)) ack = 1;
	MPU_Stop();
	return ack;
}

static uint8_t MPU_ReadRegs(uint8_t reg, uint8_t *data, uint8_t length)
{
	uint8_t i, ack;
	MPU_Start();
	if (!MPU_SendByte(MPU6050_ADDR_W) || !MPU_SendByte(reg)) { MPU_Stop(); return 0; }
	MPU_Start();
	ack = MPU_SendByte(MPU6050_ADDR_R);
	if (!ack) { MPU_Stop(); return 0; }
	for (i = 0; i < length; i++) data[i] = MPU_ReadByte(i + 1 < length);
	MPU_Stop(); return 1;
}

uint8_t MPU6050_Init(uint8_t *who_am_i)
{
	GPIO_InitTypeDef gpio;
	uint8_t id;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	gpio.GPIO_Pin = MPU6050_SCL | MPU6050_SDA;
	gpio.GPIO_Mode = GPIO_Mode_Out_OD;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &gpio);
	MPU_W_SCL(1); MPU_W_SDA(1);
	if (!MPU_WriteReg(0x6B, 0x00)) return 0;
	delay_ms(100);
	if (!MPU_ReadRegs(0x75, &id, 1)) return 0;
	*who_am_i = id;
	return (id == 0x68 || id == 0x69);
}

uint8_t MPU6050_ReadAccel(int16_t accel[3])
{
	uint8_t data[6];
	if (!MPU_ReadRegs(0x3B, data, 6)) return 0;
	accel[0] = (int16_t)((data[0] << 8) | data[1]);
	accel[1] = (int16_t)((data[2] << 8) | data[3]);
	accel[2] = (int16_t)((data[4] << 8) | data[5]);
	return 1;
}
