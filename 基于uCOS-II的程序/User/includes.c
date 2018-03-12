#include "includes.h"

//JTAG设置模式
//mode:jtag,swd  0：全使能，1：使能SWD，2：全关闭 	  
void JTAG_Set(u8 mode)
{
	u32 temp;

	temp=mode;
	temp<<=25;
	RCC->APB2ENR|=1<<0;     //开启辅助时钟	   
	AFIO->MAPR&=0XF8FFFFFF; //清除MAPR的[26:24]
	AFIO->MAPR|=temp;       //设置jtag模式
}

//自检程序
u8 RC_NRF=0;
void SelfTest(void)
{
	u8 status;
	//串口测试
	printf("\r\n串口测试\r\n");
	//无线模块自检
	status = NRF_Check(); 
 if(status == SUCCESS)	   
 {
	 NRF_TX_Mode();
	 printf("\r\nNRF与MCU连接成功！\r\n\r\n");
	 RC_NRF = 1;
 }
 else
 {		 
		RC_NRF = 0;
		printf("\r\nNRF连接失败！！\r\n");
 }
	printf("\r\n");
}

extern u8 RTCDate[6];
void Init(void)//各模块初始化函数
{
	JTAG_Set(1);

	USART1_Config();//ok
	CAN_Config();//work
	
	DS1302_Configuration();//work
	ReadDS1302Clock(RTCDate);
	
	STMFLASH_Read (FLASH_SAVE_ADDR,&Licence.RunTime,sizeof(Licence.RunTime));//ok
	ANBT_I2C_Configuration();//work
	AnBT_DMP_MPU6050_Init();
	SPI_NRF_Init();//work
	DMA1_ADC1_Init();//ok
	PWM_TIM2_Config();//ok
	LED_TIM3_Init();//ok
	LED_STATE(LED1,BREATH);//LED1工作于呼吸模式
	LED_STATE(LED2,FASTFLICKER);//LED2工作于快闪模式
	TIM4_Cap_Init(0xFFFF,71);//ok
	//WWDG_Init(0X7F,0X5F,WWDG_Prescaler_8);//计数器值为7f,窗口寄存器为5f,分频数为8	   

	SelfTest();//自检
	
	//非大容量芯片调用下面函数会导致堆栈溢出
#if 0
	SD_UserInit();
	GPS_Config();
#endif
}

//end of file
