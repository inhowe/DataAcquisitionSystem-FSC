#ifndef __SYSTICK_H
#define __SYSTICK_H

#include "includes.h"

void SysTick_Init(void);
void Delay_us(__IO u32 nTime);
#define Delay_ms(x) Delay_us(100*x)	 //��λms

#endif /* __SYSTICK_H */
