/*
*********************************************************************************************************
*	                                  
*	模块名称 : uCOS-II
*	文件名称 : app.c
*	版    本 : V1.0
*	Copyright (C), 2015-2020, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/
#include "includes.h"

/*
*********************************************************************************************************
*                                            TASK STACK SIZES
*                             Size of the task stacks (# of OS_STK entries)
*********************************************************************************************************
*/
//SD卡和GPS出问题估计是在以下几个宏定义内存溢出
//OS_TASK_TMR_STK_SIZE 
//OS_CPU_EXCEPT_STK_SIZE
//APP_TASK_GPS_DEAL_STK_SIZE

#define  APP_TASK_START_STK_SIZE               256
#define  APP_TASK_CAP_DEAL_STK_SIZE						 256
#define  APP_TASK_CAN_DEAL_STK_SIZE						 256
#define  APP_TASK_ATT_DEAL_STK_SIZE						 256
#define  APP_TASK_GPS_DEAL_STK_SIZE						 512
//#define  OS_CPU_EXCEPT_STK_SIZE              512     

/*
********************************************************************************************************
*                              任务堆栈
********************************************************************************************************
*/

/* 定义每个任务的堆栈空间，app_cfg.h文件中宏定义栈大小 */
static OS_STK AppTaskStartStk[APP_TASK_START_STK_SIZE];
static OS_STK AppTaskCapDealStk[APP_TASK_CAP_DEAL_STK_SIZE];
static OS_STK AppTaskCanDealStk[APP_TASK_CAN_DEAL_STK_SIZE];
static OS_STK AppTaskAttDealStk[APP_TASK_ATT_DEAL_STK_SIZE];
static OS_STK AppTaskGPSDealStk[APP_TASK_GPS_DEAL_STK_SIZE];
/*
*******************************************************************************************************
*                              函数声明
*******************************************************************************************************
*/
static void DispTaskInfo(void);
static void AppTaskCreate(void);
static void AppTaskStart(void *p_arg);
static void App_Printf (CPU_CHAR *format, ...);
static void AppTaskCapDeal(void* pdata);
static void AppTaskCanDeal(void* pdata);
static void AppTaskAttDeal(void* pdata);
static void AppTaskGPSDeal(void* pdata);
static void LEDCallback(void *ptmr, void *callback_arg);
static void SENDCallback(void *ptmr, void *callback_arg);
static void FILECallback(void *ptmr, void *callback_arg);
static void TEST(void);
u8 DectoBCD(u8 temp);
u8  BCDtoDec(u8 temp);  
void Protection(void);
void FileDataPreDeal(u8 * str,u8 type);

/*
*******************************************************************************************************
*                               变量
*******************************************************************************************************
*/

/* 定义一个邮箱， 这只是一个邮箱指针， OSMboxCreate函数会创建邮箱必需的资源 */
static OS_EVENT *AppUserIFMbox;

/*创建一个信号量*/
static OS_EVENT *AppPrintfSemp;

u8 RTCDate[6]={0,0,0,0,0,0};
struct Limit Licence={0,1,1,0};
u32 CAPTURE_Buff[4]={0,0,0,0};
u16 RPM,EngineTemperature,BattVoltage,Gear,ThrottlePosition,SpeedA,SpeedB,SpeedC,SpeedD,Lambda1;
u8 String[55];
/*
*******************************************************************************************************
*	函 数 名: main
*	功能说明: 标准c程序入口。
*	形    参: 无
*	返 回 值: 无
*******************************************************************************************************
*/
int main(void)
{
	INT8U  err;

	/* 初始化"uC/OS-II"内核 */
	OSInit();
  
	/* 创建一个启动任务（也就是主任务）。启动任务会创建所有的应用程序任务 */
	OSTaskCreateExt(AppTaskStart,	/* 启动任务函数指针 */
                    (void *)0,		/* 传递给任务的参数 */
                    (OS_STK *)&AppTaskStartStk[APP_TASK_START_STK_SIZE-1], /* 指向任务栈栈顶的指针 */
                    APP_TASK_START_PRIO,	/* 任务的优先级，必须唯一，数字越低优先级越高 */
                    APP_TASK_START_PRIO,	/* 任务ID，一般和任务优先级相同 */
                    (OS_STK *)&AppTaskStartStk[0],/* 指向任务栈栈底的指针。OS_STK_GROWTH 决定堆栈增长方向 */
                    APP_TASK_START_STK_SIZE, /* 任务栈大小 */
                    (void *)0,	/* 一块用户内存区的指针，用于任务控制块TCB的扩展功能
                       （如任务切换时保存CPU浮点寄存器的数据）。一般不用，填0即可 */
                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR); /* 任务选项字 */
					
					/*  定义如下：
						OS_TASK_OPT_STK_CHK      使能检测任务栈，统计任务栈已用的和未用的
						OS_TASK_OPT_STK_CLR      在创建任务时，清零任务栈
						OS_TASK_OPT_SAVE_FP      如果CPU有浮点寄存器，则在任务切换时保存浮点寄存器的内容
					*/                  

	/* 指定任务的名称，用于调试。这个函数是可选的 */
	OSTaskNameSet(APP_TASK_START_PRIO, APP_TASK_START_NAME, &err);
	
	/*ucosII的节拍计数器清0    节拍计数器是0-4294967295*/ 
	OSTimeSet(0);	
	
	/* 启动多任务系统，控制权交给uC/OS-II */
	OSStart();
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskStart
*	功能说明: 这是一个启动任务，在多任务系统启动后，必须初始化滴答计数器(在BSP_Init中实现)
*	形    参: p_arg 是在创建该任务时传递的形参
*	返 回 值: 无
	优 先 级: 0
*********************************************************************************************************
*/
static void AppTaskStart(void *p_arg)
{
    /* 仅用于避免编译器告警，编译器不会产生任何目标代码 */	
    (void)p_arg;  
	
	 
	/* BSP 初始化。 BSP = Board Support Package 板级支持包，可以理解为底层驱动。*/
	bsp_Init();
    CPU_Init();          
	BSP_Tick_Init();   
	

	/* 检测CPU能力，统计模块初始化。该函数将检测最低CPU占有率 */
	#if (OS_TASK_STAT_EN > 0)
		OSStatInit();
	#endif	
		
	/* 创建应用程序的任务 */
	AppTaskCreate();
	
	/* 任务主体，必须是一个死循环 */
	while (1)     
	{ 
		OSTimeDlyHMSM(0, 0, 0, 500);										
	}      
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskCreate
*	功能说明: 创建应用任务
*	形    参: p_arg 是在创建该任务时传递的形参
*	返 回 值: 无
*********************************************************************************************************
*/
static void AppTaskCreate (void)
{
	INT8U      err;
	static OS_TMR  *Tmr;

	/* 创建CapDeal任务 */
	OSTaskCreateExt(AppTaskCapDeal,
                    (void *)0,
                    (OS_STK *)&AppTaskCapDealStk[APP_TASK_CAP_DEAL_STK_SIZE - 1],
                    APP_TASK_CAP_DEAL_PRIO,
                    APP_TASK_CAP_DEAL_PRIO,
                    (OS_STK *)&AppTaskCapDealStk[0],
                    APP_TASK_CAP_DEAL_STK_SIZE,
                    (void *)0,
                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

	OSTaskNameSet(APP_TASK_CAP_DEAL_PRIO, APP_TASK_CAP_DEAL_NAME, &err);

/* 创建CanDeal任务 */
	OSTaskCreateExt(AppTaskCanDeal,
                    (void *)0,
                    (OS_STK *)&AppTaskCanDealStk[APP_TASK_CAN_DEAL_STK_SIZE - 1],
                    APP_TASK_CAN_DEAL_PRIO,
                    APP_TASK_CAN_DEAL_PRIO,
                    (OS_STK *)&AppTaskCanDealStk[0],
                    APP_TASK_CAN_DEAL_STK_SIZE,
                    (void *)0,
                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

	OSTaskNameSet(APP_TASK_CAN_DEAL_PRIO, APP_TASK_CAN_DEAL_NAME, &err);
	
	/* 创建AttDeal任务 */
	OSTaskCreateExt(AppTaskAttDeal,
                    (void *)0,
                    (OS_STK *)&AppTaskAttDealStk[APP_TASK_ATT_DEAL_STK_SIZE - 1],
                    APP_TASK_ATT_DEAL_PRIO,
                    APP_TASK_ATT_DEAL_PRIO,
                    (OS_STK *)&AppTaskAttDealStk[0],
                    APP_TASK_ATT_DEAL_STK_SIZE,
                    (void *)0,
                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

	OSTaskNameSet(APP_TASK_ATT_DEAL_PRIO, APP_TASK_ATT_DEAL_NAME, &err);
	
	/* 创建GPSDeal任务 */
	OSTaskCreateExt(AppTaskGPSDeal,
                    (void *)0,
                    (OS_STK *)&AppTaskGPSDealStk[APP_TASK_GPS_DEAL_STK_SIZE - 1],
                    APP_TASK_GPS_DEAL_PRIO,
                    APP_TASK_GPS_DEAL_PRIO,
                    (OS_STK *)&AppTaskGPSDealStk[0],
                    APP_TASK_GPS_DEAL_STK_SIZE,
                    (void *)0,
                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

	OSTaskNameSet(APP_TASK_GPS_DEAL_PRIO, APP_TASK_GPS_DEAL_NAME, &err);
	
	/* 创建一个NRF定时器任务*/
	Tmr = OSTmrCreate(1, //起始延迟时间 1*(1000/OS_TMR_CFG_TICKS_PER_SEC)ms执行一次
										1, //周期		
										OS_TMR_OPT_PERIODIC, 
										SENDCallback,
										(void *)0,
										"nrftmr",
										&err);
	
	if (err == OS_ERR_NONE) 
	{
		OSTmrStart(Tmr, &err);
	}
	
	/* 创建一个FILE定时器任务*/
	Tmr = OSTmrCreate(1, //起始延迟时间 1*(1000/OS_TMR_CFG_TICKS_PER_SEC)ms执行一次
										1, //周期		
										OS_TMR_OPT_PERIODIC, 
										FILECallback,
										(void *)0,
										"filetmr",
										&err);
	
	if (err == OS_ERR_NONE) 
	{
		OSTmrStart(Tmr, &err);
	}

	/* 创建一个LED定时器任务*/
	Tmr = OSTmrCreate(1, //起始延迟时间 2*(1000/OS_TMR_CFG_TICKS_PER_SEC)ms执行一次
										2, //周期		
										OS_TMR_OPT_PERIODIC, 
										LEDCallback,
										(void *)0,
										"ledtmr",
										&err);
	
	if (err == OS_ERR_NONE) 
	{
		OSTmrStart(Tmr, &err);
	}
	
}

void AppTaskCapDeal(void* pdata)
{
	
	while(1)
	{
		u32 temp=0;
		//TIM4
		if(TIM4CH1_CAPTURE_STA&0X80)   
		{
			temp=TIM4CH1_CAPTURE_STA&0X3F;					
			temp*=65536;             
			temp+=TIM4CH1_CAPTURE_VAL_B;    
			temp-=TIM4CH1_CAPTURE_VAL_A;
			CAPTURE_Buff[0]=temp;
			TIM4CH1_CAPTURE_STA=0;
		}
		if(TIM4CH2_CAPTURE_STA&0X80) 
		{
			temp=TIM4CH2_CAPTURE_STA&0X3F;					
			temp*=65536;             
			temp+=TIM4CH2_CAPTURE_VAL_B;    
			temp-=TIM4CH2_CAPTURE_VAL_A;
			CAPTURE_Buff[1]=temp;
			TIM4CH2_CAPTURE_STA=0;
		}
		if(TIM4CH3_CAPTURE_STA&0X80)   
		{
			temp=TIM4CH3_CAPTURE_STA&0X3F;	
			temp*=65536;            
			temp+=TIM4CH3_CAPTURE_VAL_B;    
			temp-=TIM4CH3_CAPTURE_VAL_A;
			CAPTURE_Buff[2]=temp;
			TIM4CH3_CAPTURE_STA=0;   
		}
		if(TIM4CH4_CAPTURE_STA&0X80)      //成功捕平获到了一个周期
		{
			temp=TIM4CH4_CAPTURE_STA&0X3F;					 //这个获得了溢出次数
			temp*=65536;              //溢出时间总和		 ，一次溢出代表一个十六位的计数器计满
			temp+=TIM4CH4_CAPTURE_VAL_B;    //得到总的高电平时间
			temp-=TIM4CH4_CAPTURE_VAL_A;
			CAPTURE_Buff[3]=temp;
			TIM4CH4_CAPTURE_STA=0;      //开启下一次捕获
		}
		/* 延迟5ms。必须释放CPU，否则低优先级任务会阻塞 */  
		OSTimeDlyHMSM(0, 0, 0, 5);
	}
	
}

static void AppTaskCanDeal(void* pdata)
{
	while(1)
	{
		if( CanFlag == ENABLE ) //收到一帧正确的数据，并打印到串口
	  {
      CanFlag = DISABLE;
      if(MotecCache[0]==0X82)//再验证
				if(MotecCache[1]==0X81)
					if(MotecCache[2]==0X80)
					{
						CAN_ITConfig(CAN1,CAN_IT_FMP0, DISABLE);
						
						RPM=(MotecCache[4]<<8)|MotecCache[5];
						Lambda1=(MotecCache[14]<<8)|MotecCache[15];
						ThrottlePosition=(MotecCache[6]<<8)|MotecCache[7];
						EngineTemperature=(MotecCache[12]<<8)|MotecCache[13];
						BattVoltage=(MotecCache[48]<<8)|MotecCache[49];
						//Gear=(MotecCache[116]<<8)|MotecCache[117];
						SpeedA=(MotecCache[52]<<8)|MotecCache[53];
						SpeedB=(MotecCache[54]<<8)|MotecCache[55];
						SpeedC=(MotecCache[56]<<8)|MotecCache[57];
						SpeedD=(MotecCache[58]<<8)|MotecCache[59];
						
						MotecCnt=0;
						MotecFlag=0;
						CAN_ITConfig(CAN1,CAN_IT_FMP0, ENABLE);
					}
	  }
		/*延迟5ms。必须释放CPU，否则低优先级任务会阻塞 */
		OSTimeDlyHMSM(0, 0, 0, 5);
	}
}

static void AppTaskAttDeal(void* pdata)
{
	while(1)
	{
		dmp_read_fifo(gyro, accel, quat, &sensor_timestamp, &sensors,&more);	//读取车身姿态并计算
		if(sensors&INV_WXYZ_QUAT)//耗时挺长，所以放在这里
		{
			q0=quat[0] / q30;
			q1=quat[1] / q30;
			q2=quat[2] / q30;
			q3=quat[3] / q30;
			Pitch = asin(-2 * q1 * q3 + 2 * q0* q2)* 57.3; // 
			Roll = atan2(2 * q2 * q3 + 2 * q0 * q1, -2 * q1 * q1 - 2 * q2* q2 + 1)* 57.3;
			Yaw = 	atan2(2*(q1*q2 + q0*q3),q0*q0+q1*q1-q2*q2-q3*q3) * 57.3;		//感觉没有价值，注掉
		}
		/*延迟5ms。必须释放CPU，否则低优先级任务会阻塞 */
		OSTimeDlyHMSM(0, 0, 0, 5);
	}
}
static void AppTaskGPSDeal(void* pdata)
{
	while(1)
	{
		//GPS屏蔽了
//		nmea_decode_test();
		/*延迟5ms。必须释放CPU，否则低优先级任务会阻塞 */
		OSTimeDlyHMSM(0, 0, 0, 5);
	}
}
static void  FILECallback(void *ptmr, void *callback_arg)
{
	if(OpenFileSave)
	 {
		 static u8 FileCnt=0;
		 FileCnt++;
		 FileDataPreDeal(String,FileCnt);
		 switch(FileCnt)
		 {
			case 1: WriteSomething(DirStringE,"Motec.txt"  ,(u8 *)String,sizeof(String)); break;
			case 2: WriteSomething(DirStringE,"ADC.txt"    ,(u8 *)String,sizeof(String)); break;
			case 3: WriteSomething(DirStringE,"MPU6050.txt",(u8 *)String,sizeof(String)); break;
			default: FileCnt=0;break;
		 }
	 }
}
static void  SENDCallback(void *ptmr, void *callback_arg)
{
	//SendDATA();
}
static void  LEDCallback(void *ptmr, void *callback_arg)
{
	Protection();
	LED_STATE_Fun();
	TEST();
}

/*
*********************************************************************************************************
*	函 数 名: App_Printf
*	功能说明: 线程安全的printf方式		  			  
*	形    参: 同printf的参数。
*             在C中，当无法列出传递函数的所有实参的类型和数目时,可以用省略号指定参数表
*	返 回 值: 无
*********************************************************************************************************
*/
static void  App_Printf (CPU_CHAR *format, ...)
{
    CPU_CHAR  buf_str[80 + 1];
    va_list   v_args;
    CPU_INT08U  os_err;


    va_start(v_args, format);
   (void)vsnprintf((char       *)&buf_str[0],
                   (size_t      ) sizeof(buf_str),
                   (char const *) format,
                                  v_args);
    va_end(v_args);

    OSSemPend((OS_EVENT  *)AppPrintfSemp,
              (INT32U     )0,
              (INT8U     *)&os_err);

    printf("%s", buf_str);

    os_err = OSSemPost((OS_EVENT *)AppPrintfSemp);
}

//LED2呼吸表示系统正常工作
//LED2慢闪表示系统限制工作
//LED2快闪表示系统停止工作
void Protection(void)
{
	static u16 cnt=0;
	cnt++;
	if(cnt%100==0)//每秒
	{
		//2016.10.8号前具有所有权限
		if(  (GPSTime.year==2016&&GPSTime.mon<=10  &&GPSTime.day<8)
			 ||(RTCDate[0]	==0x16&&RTCDate[1] <=0x10&&RTCDate[2]<0x08)	)
		{	Licence.TimeLimit=0;Licence.RunTime=0;Licence.STOP=0;}
		//襄阳以外地区具有所有权限，需要至少4颗卫星
		if((GPS_num>4)&&(GPS_lon<110||GPS_lon>114||GPS_lat<30||GPS_lat>33))
		{	Licence.LocationLimit=0;Licence.RunTime=0;Licence.STOP=0;}
		
		//具有限制性权限，传感器数据不显示
		if(Licence.STOP==0&&(Licence.LocationLimit||Licence.TimeLimit))
		{
			//运行时间计算
			Licence.RunTime++;
			LED_STATE(LED2,SLOWFLICKER);
		}
		else
			LED_STATE(LED2,BREATH);
		
		//每10s存储一次RunTime和RTC
		if(cnt%1000==0)
		{
			STMFLASH_Write(FLASH_SAVE_ADDR,&Licence.RunTime,sizeof(Licence.RunTime));
			//如果GPS正常存在则把时间存进RTC
			if(Licence.LocationLimit==0)
			{
				RTCDate[0]=DectoBCD(GPSTime.year);
				RTCDate[1]=DectoBCD(GPSTime.mon);
				RTCDate[2]=DectoBCD(GPSTime.day);
				RTCDate[3]=DectoBCD(GPSTime.hour);
				RTCDate[4]=DectoBCD(GPSTime.min);
				RTCDate[5]=DectoBCD(GPSTime.sec);
				WriteDS1302Clock(RTCDate);
			}
			//计数器清零
			cnt=0;
		}
		
		//1800s后进入禁止权限，传感器数据不显示，档位维持在1档，屏幕提示未授权
		if(Licence.RunTime>1800&&Licence.RunTime<65535)
		{	Licence.STOP =1;LED_STATE(LED2,FASTFLICKER); }
	}
}

//文件数据预备处理
void FileDataPreDeal(u8 * str,u8 type)
{
	memset(str,'\0',55);
	switch(type)
	{
	case 1:sprintf((char*)str,  "%4d %4d %4d %4d %4d %5.1f %5.1f\r\n",RPM,SpeedA,SpeedB,SpeedC,SpeedD,(float)ThrottlePosition/10,(float)BattVoltage/100);
		break;
  case 2:sprintf((char*)str, "%4d %4d %4d %4d\r\n",ADCBuff[0],ADCBuff[1],ADCBuff[2],ADCBuff[3]);
		break;
	case 3:sprintf((char*)str,"%5d %5d %5d | %5d %5d %5d | %4.1f %4.1f\r\n",accel[0],accel[1],accel[2],gyro[0],gyro[1],gyro[2],Pitch*100,Roll*100);
		break;
	default:break;
	}
}

u8  BCDtoDec(u8 temp)  
{  
	return ((temp/16)*10+temp%16);  
}  

u8 DectoBCD(u8 temp)  
{  
	return ((temp/10)*16+temp%10);  
}

/*
*********************************************************************************************************
*	函 数 名: DispTaskInfo
*	功能说明: 将uCOS-II任务信息打印到串口
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void DispTaskInfo(void)
{
	OS_TCB      *ptcb;	        /* 定义一个任务控制块指针, TCB = TASK CONTROL BLOCK */
	OS_STK_DATA stk;	        /* 用于读取任务栈使用情况 */

	ptcb    = &OSTCBTbl[0];		/* 指向第1个任务控制块(TCB) */

	/* 打印标题 */
	App_Printf("==================================================\r\n");
	App_Printf("  优先级   使用栈  剩余栈  百分比   任务名\r\n");
	App_Printf("  Prio     Used    Free    Per      Taskname\r\n");

	OSTimeDly(10);
	
	/* 遍历任务控制块列表(TCB list)，打印所有的任务的优先级和名称 */
	while (ptcb != NULL)
	{
		/* 
			ptcb->OSTCBPrio : 任务控制块中保存的任务的优先级
			ptcb->OSTCBTaskName : 任务控制块中保存的任务名称，需要在创建任务的时候
			调用OSTaskNameSet()指定任务名称，比如：
			OSTaskNameSet(APP_TASK_USER_IF_PRIO, "User I/F", &err);
		*/
		OSTaskStkChk(ptcb->OSTCBPrio, &stk);	/* 获得任务栈已用空间 */
		App_Printf("   %2d    %5d    %5d    %02d%%     %s\r\n", ptcb->OSTCBPrio, 
			stk.OSUsed, stk.OSFree, (stk.OSUsed * 100) / (stk.OSUsed + stk.OSFree),
			ptcb->OSTCBTaskName);		
		ptcb = ptcb->OSTCBPrev;		            /* 指向上一个任务控制块 */
	}
    printf("Tick Rate : %d  \r\n",OS_TICKS_PER_SEC);//节拍频率
    printf("CPU  Usage: %d% \r\n",OSCPUUsage);//CPU使用率
}

static void TEST(void)
{
	static u16 cnt=0;
	cnt++;
	if(cnt%10==0)
	{
//	printf("ADC:\r\n");
//	printf("A4:%4d A5:%4d A6:%4d A7:%4d\r\n",ADCBuff[0],ADCBuff[1],ADCBuff[2],ADCBuff[3]);
//	printf("C0:%4d C1:%4d C2:%4d C3:%4d C4:%4d C5:%4d\r\n",ADCBuff[4],ADCBuff[5],ADCBuff[6],ADCBuff[7],ADCBuff[8],ADCBuff[9]);
//	printf("\r\n");

	printf("CAPTURE\r\n");
	printf("B6:%d B7:%d B8:%d B9:%d\r\n",CAPTURE_Buff[0],CAPTURE_Buff[1],CAPTURE_Buff[2],CAPTURE_Buff[3]);
	printf("\r\n");
	}
	if(cnt%100==0)
	{
		DispTaskInfo();	
		if(Licence.RunTime!=0)
		{	printf("运行时间：%d\r\n",Licence.RunTime);
			if(Licence.LocationLimit)	printf("地点信息缺失\r\n");
			if(Licence.TimeLimit)		printf("时间信息缺失\r\n");
		}
	  else if(Licence.STOP)			printf("系统停止工作\r\n");
	}
}
/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/

