#ifndef __WS2812D_H
#define __WS2812D_H

void WS2812_Set(uint32_t x);
void WS2812_arr2pwm(void);

void WS2812_Init(void);
void DMA1_Init(void);
#endif
