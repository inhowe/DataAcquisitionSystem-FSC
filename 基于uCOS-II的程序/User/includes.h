/*
*********************************************************************************************************
*
*	模块名称 : 头文件汇总
*	文件名称 : includes.h
*	版    本 : V1.0
*	说    明 : 当前使用头文件汇总
*
*	修改记录 :
*		版本号    日期        作者     说明
*		V1.0    2015-08-02  Eric2013   首次发布
*
*	Copyright (C), 2015-2020, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#ifndef  __INCLUDES_H__
#define  __INCLUDES_H__
/*
*********************************************************************************************************
*                                         标准库
*********************************************************************************************************
*/

#include  <stdarg.h>
#include  <stdio.h>
#include  <stdlib.h>
#include  <math.h>


/*
*********************************************************************************************************
*                                         其它库
*********************************************************************************************************
*/

#include  <cpu.h>
#include  <lib_def.h>
#include  <lib_ascii.h>
#include  <lib_math.h>
#include  <lib_mem.h>
#include  <lib_str.h>
#include  <app_cfg.h>


/*
*********************************************************************************************************
*                                           OS
*********************************************************************************************************
*/

#include  <ucos_ii.h>


/*
*********************************************************************************************************
*                                           宏定义
*********************************************************************************************************
*/




/*
*********************************************************************************************************
*                                        APP / BSP
*********************************************************************************************************
*/

#include  <bsp.h>

/*
*********************************************************************************************************
*                                         全局变量
*********************************************************************************************************
*/
#define RC_UART 						

//许可证声明
struct Limit
{
	u8 STOP;  
	u8 TimeLimit;
	u8 LocationLimit; 
	u16 RunTime;
};
extern struct Limit Licence;

//GPS声明
extern float GPS_Speed,GPS_lat,GPS_lon,GPS_direction,GPS_elvetion;
extern u8 GPS_sta,GPS_num;
extern nmeaTIME GPSTime;

//CAN总线声明
extern uint8_t CanFlag;
extern u8 MotecCache[176];
extern u8 MotecCnt,MotecFlag;

//姿态传感器声明
#define q30  1073741824.0f
extern long quat[4];//四元数存放数组
extern float Yaw,Roll,Pitch;//欧拉角
extern float q0,q1,q2,q3;//计算姿态过程用到的变量
extern short gyro[3], accel[3], sensors;//陀螺仪存放数组，加速度存放数组，返回状态量
extern unsigned char more;
extern unsigned long sensor_timestamp;

//CAPTURE
extern u8	TIM4CH1_CAPTURE_STA,TIM4CH2_CAPTURE_STA,  //输入捕获状态   
					TIM4CH3_CAPTURE_STA,TIM4CH4_CAPTURE_STA;
					 
extern u16	TIM4CH1_CAPTURE_VAL_A,TIM4CH1_CAPTURE_VAL_B, //输入捕获值
						TIM4CH2_CAPTURE_VAL_A,TIM4CH2_CAPTURE_VAL_B,
						TIM4CH3_CAPTURE_VAL_A,TIM4CH3_CAPTURE_VAL_B,	
						TIM4CH4_CAPTURE_VAL_A,TIM4CH4_CAPTURE_VAL_B;	
//无线开关
extern u8 RC_NRF;

//FILE相关
extern u8 OpenFileSave;
extern	const char *DirStringE;

//ADC缓存
extern u16 volatile ADCBuff[10];

//各模块初始化函数
void Init(void);
extern int nmea_decode_test(void);
#endif

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
