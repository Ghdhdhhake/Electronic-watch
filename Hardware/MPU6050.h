#ifndef __MPU6050_H
#define __MPU6050_H

#include "stm32f10x.h"

uint8_t MPU6050_Init(uint8_t *who_am_i);
uint8_t MPU6050_ReadAccel(int16_t accel[3]);

#endif
