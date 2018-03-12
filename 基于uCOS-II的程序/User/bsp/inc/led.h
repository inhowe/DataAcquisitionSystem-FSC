#ifndef __LED_H
#define __LED_H

#include "includes.h"

#define LED1 1
#define LED2 2

#define OFF  				0
#define ON	 				1

#define NORMAL			0
#define BREATH 			2
#define FASTFLICKER 3//©Лиа 10hz
#define MEDIFLICKER	4//жпиа	5hz
#define SLOWFLICKER	5//бЩиа	2hz

void LED_TIM3_Init(void);
void LED_STATE(u8 num,u8 state);
void LED_STATE_Fun(void);

#endif
