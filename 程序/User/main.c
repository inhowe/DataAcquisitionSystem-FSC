/**
  ******************************************************************************
  * @file    main.c
  * @author  DRINOW
  * @version V1.0
  * @date    2014-xx-xx
  * @brief   用3.5.0版本库建的工程模板
  ******************************************************************************
  * @attention
  *	NOTE：
  * 
	GPS  占用大量的Heap_Size  在启动文件里修改，原先是0x00000400改为0x00001000
	FatFS占用大量的Stack_Size 在启动文件里修改，原先是0x00000200改为0x00001000
	*
  ******************************************************************************
  */
  
#include "includes.h"

#define q30  1073741824.0f

extern long quat[4];//四元数存放数组
extern float Yaw,Roll,Pitch;//欧拉角
extern float q0,q1,q2,q3;//计算姿态过程用到的变量
extern short gyro[3], accel[3], sensors;//陀螺仪存放数组，加速度存放数组，返回状态量
extern unsigned char more;
extern unsigned long sensor_timestamp;

//extern u16 volatile ADCBuff[10];
//extern u32 CAPTURE_Buff[4];	

extern u8 OpenFileSave;
extern	const char *DirStringE;


u8 String[55];
volatile u16 RPM,EngineTemperature,BattVoltage,Gear,ThrottlePosition,SpeedA,SpeedB,SpeedC,SpeedD,Lambda1;

TIME_TypeDef RTCTime;

volatile u8 MotecCache[176]={0};//我猜测，变量在内存中是随机存放的，所以放在子文件里超长变量很可能没有足够长的空间，所以会被改变
volatile u8 MotecCnt=0,MotecFlag=0;
volatile uint8_t CanFlag;//中间变量

volatile float GPS_Speed,GPS_lat,GPS_lon,GPS_direction,GPS_elvetion;
volatile nmeaTIME GPSTime;
volatile u8 GPS_sta,GPS_num;

void DealGPS(void);
void DealFile(void);
void DealMPU6050(void);
void DealCan(void);
void DealADC(void);
void FileDataPreDeal(u8 * string,u8 type);
extern int nmea_decode_test(void);

void TEST(void)
{
	//static u8 cnt[3]={'0','\r','\n'};
	if(SysCnt%500==0)
	{
	//	printf("ADC:\r\n");
	//	printf("A4:%4d A5:%4d A6:%4d A7:%4d\r\n",ADCBuff[0],ADCBuff[1],ADCBuff[2],ADCBuff[3]);
	//	printf("C0:%4d C1:%4d C2:%4d C3:%4d C4:%4d C5:%4d\r\n",ADCBuff[4],ADCBuff[5],ADCBuff[6],ADCBuff[7],ADCBuff[8],ADCBuff[9]);
	//	printf("\r\n");

	//	printf("CAPTURE\r\n");
	//	printf("B6:%d B7:%d B8:%d B9:%d\r\n",CAPTURE_Buff[0],CAPTURE_Buff[1],CAPTURE_Buff[2],CAPTURE_Buff[3]);
	//	printf("\r\n");

		if(Licence.RunTime!=0)		{printf("运行时间：%d\r\n",Licence.RunTime);}
		if(Licence.LocationLimit)	{printf("地点信息缺失\r\n");}
		if(Licence.TimeLimit)			{printf("时间信息缺失\r\n");}
		if(Licence.STOP)					{printf("系统停止工作\r\n");}
		
		printf("\r\n--------------------------\r\n");
		
//		cnt[0]++;
//		if(cnt[0]>='9')cnt[0]='0';
//		NRF_Tx_Dat(cnt);
	}
}


/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
struct  Limit Licence={0,1,1,0};//每次上电时自动置一，根据条件清零
int main(void)
{
	
  NVIC_Config();//该函数用于管理所有中断向量
	Init();
	printf("Init OK! \r\n");
	while(1)
	 {
			 DealGPS();
//			 DealFile();
			 CAPTURE_Cal();
			 DealMPU6050();
			 DealCan();
			 DealADC(); 
			 TEST();
			 SendDATA(); //发送数据
	 }
}

//LED2呼吸表示系统正常工作
//LED2慢闪表示系统限制工作
//LED2快闪表示系统停止工作
void Protection(void)
{
	//时间
	if(  (GPSTime.year==2016&&GPSTime.mon<=10  &&GPSTime.day<8)
		 ||(RTCTime.year	<=16&&RTCTime.month <=10)	)
	{	Licence.TimeLimit=0;Licence.RunTime=0;Licence.STOP=0;}
	//坐标
	if((GPS_sta>1)&&(GPS_lon<11000||GPS_lon>11400||GPS_lat<3000||GPS_lat>3300))
	{	Licence.LocationLimit=0;Licence.RunTime=0;Licence.STOP=0;}
	else {Licence.LocationLimit=1;}
	//具有限制性权限
	if(Licence.STOP==0&&(Licence.LocationLimit||Licence.TimeLimit))
	{
		Licence.RunTime++;//运行时间计算
	}
	
	if(Licence.TimeLimit)//RTC，GPS指示
		PWM_LED_STATE(LED2,SLOWFLICKER);
	else
		PWM_LED_STATE(LED2,BREATH);
	if(Licence.LocationLimit)
		PWM_LED_STATE(LED1,SLOWFLICKER);
	else
		PWM_LED_STATE(LED1,BREATH);
//	PWM_LED_STATE(LED1,ON);
//	PWM_LED_STATE(LED2,ON);
		
	if(SysCnt%5000==0)//每5s存储一次RunTime和RTC,并读一次RTC
	{
		STMFLASH_Write(FLASH_SAVE_ADDR,&Licence.RunTime,sizeof(Licence.RunTime));
		DS1302_ReadTime(&RTCTime);
		if(Licence.LocationLimit==0)//如果GPS正常存在则把时间存进RTC
		//if(1)
		{
				RTCTime.sec=DectoBCD(GPSTime.sec);
				RTCTime.min=DectoBCD(GPSTime.min);
				RTCTime.hour=DectoBCD(GPSTime.hour);
				RTCTime.date=DectoBCD(GPSTime.day);
				RTCTime.month=DectoBCD(GPSTime.mon);
				RTCTime.year=DectoBCD(GPSTime.year);
				RTCTime.week=0x00;
//				RTCTime.sec=0x00;
//				RTCTime.min=0x50;
//				RTCTime.hour=0x21;
//				RTCTime.date=0x13;
//				RTCTime.month=0x08;
//				RTCTime.year=0x16;
//				RTCTime.week=0x00;
				DS1302_WriteTime(&RTCTime);
		}
	}
	//2000s后进入禁止权限
	if(Licence.RunTime>4000&&Licence.RunTime<65535)
	{	Licence.STOP =1;PWM_LED_STATE(LED2,FASTFLICKER);PWM_LED_STATE(LED1,FASTFLICKER); }
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

//CAN总线数据处理
void DealCan(void)
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
}

//陀螺仪数据处理
void DealMPU6050(void)
{
	if(SysCnt%5==0)
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
	}
}


//SD卡写入处理
void DealFile(void)
{
	if(SysCnt%20==0&&OpenFileSave)
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

//GPS数据处理
void DealGPS(void)
{
	 nmea_decode_test();
}

//ADC数据处理
void DealADC(void)
{
	if(SysCnt%10==0)
	{
	 //已经在DMA里采集
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



/*********************************************END OF FILE**********************/

