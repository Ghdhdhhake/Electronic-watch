#ifndef __TIM2_H
#define __TIM2_H

void TIM2_Init(void);
extern uint32_t TIM2_Num[3];		//每1ms计数1次
extern uint32_t TIM2_Pwm[3];		//秒针pwm;保险丝发光管pwm;范围0~30
extern uint32_t TIM2_Comp[3];		//秒针比较值;发光管比较值
#endif
