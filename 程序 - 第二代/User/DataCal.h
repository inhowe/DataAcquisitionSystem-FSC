#ifndef __DATACAL_H
#define __DATACAL_H
#include "includes.h"

float SpeedCal(u8 choose);
void CAPTURE_Cal(void);
void accel_Filter(void);
void Show(void);

#endif
