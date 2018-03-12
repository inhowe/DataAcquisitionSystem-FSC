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

extern 	TIME_TypeDef RTCTime;
void Init(void)//各模块初始化函数
{
	JTAG_Set(1);
	//这个看门狗到底是不能用还是调试时不好用?
	SysTick_Init();//只是初始化，还没打开,1ms
	USART1_Config();
	CAN_Config();
	ds1302_init();
	DS1302_ReadTime(&RTCTime);
	STMFLASH_Read (FLASH_SAVE_ADDR,&Licence.RunTime,sizeof(Licence.RunTime));
	ANBT_I2C_Configuration();
	AnBT_DMP_MPU6050_Init();
	SPI_NRF_Init();
  DMA1_ADC1_Init();
  PWM_TIM2_Config();
	PWM_LED_Init();
	PWM_LED_STATE(LED1,BREATH);//LED1用于GPS工作指示
	PWM_LED_STATE(LED2,BREATH);//LED2用于RTC工作指示
	TIM4_Cap_Init(0xFFFF,71);
	CD4067Init();
	//WWDG_Init(0X7F,0X5F,WWDG_Prescaler_8);//计数器值为7f,窗口寄存器为5f,分频数为8	   
	SysTick->CTRL |=  SysTick_CTRL_ENABLE_Msk;//开启SysTick
	
	SelfTest();//自检
	
	//非大容量芯片调用下面函数会导致堆栈溢出

		SD_UserInit();
	  GPS_Config();

}

//自检程序
volatile u8 RC_NRF=0;
//u8 TxPack[32]={'n','r','f',' ','d','a','t','a',' ','s','e','n','d',' ','t','e','s','t'};
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
		printf("\r\n  NRF连接失败！！\r\n");
 }
	printf("\r\n");
}

//end of file
