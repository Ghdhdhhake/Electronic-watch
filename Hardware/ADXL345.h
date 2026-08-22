#ifndef __ADXL345_H
#define __ADXL345_H

void ADXL345_Init(uint8_t Wake_gravity);	//参数：唤醒加速度G
void ADXL345_WriteReg(uint8_t RegAddress, uint8_t Data);
uint8_t ADXL345_ReadReg(uint8_t RegAddress);
void ADXL345_GetData(int16_t *AccX, int16_t *AccY, int16_t *AccZ);
uint8_t ADXL345_GetINT(void);
#endif
