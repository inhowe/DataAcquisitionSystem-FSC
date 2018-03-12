#ifndef _KEY_H 
#define _KEY_H
#include "includes.h"

extern int num;
extern s8 key_check;
extern s8 key_check_run;
extern s16 Key_Flg;
extern s8 keyget;


u8   Key_Scan(void);
void KeyInit(void);
void key_delay_ms(vu32 m);
void Key_Deal(void);

#endif

