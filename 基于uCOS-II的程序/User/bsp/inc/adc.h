#ifndef __ADC_H 
#define __ADC_H

#include "includes.h"

#define ADC1_DR_Address    ((uint32_t)0x4001244C)
void ADC1_Configuration(void);
void DMA1_Configuration(void);
void DMA1_ADC1_Init(void);
#endif
