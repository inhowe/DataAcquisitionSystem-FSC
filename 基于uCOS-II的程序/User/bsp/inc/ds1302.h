#ifndef DS1302_H
#define DS1302_H
#include "includes.h"

extern void InitClock(void);
extern void ReadDS1302Clock(u8 *p);
extern void WriteDS1302Clock(u8 *p);
extern void ReadDSRam(u8 *p,u8 add,u8 cnt);
extern void WriteDSRam(u8 *p,u8 add,u8 cnt);
extern void TestDS1302(void);
void DS1302_Configuration(void);
#endif

