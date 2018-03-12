#ifndef __CD4067_H
#define __CD4067_H

#include "includes.h"

void CD4067Init(void);
void CD4067_SelectChannel(u8 ch);
void CD4067_AutoRotate(void);
void CD4067_ADCToBuff(void);
#endif 
