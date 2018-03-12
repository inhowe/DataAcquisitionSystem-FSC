#include "nvic.h"

/***										***/
/************该文件用于管理中断向量************/
/***	  请将所有中断向量配置移至此处		***/

void NVIC_Config(void)
{	
	NVIC_InitTypeDef NVIC_InitStructure; 	
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
//====================================配置USART1接收中断======================================//
	/* Enable the USARTy Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;	 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
//====================================配置DMA接收中断======================================//
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel6_IRQn;	 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
//====================================配置CAN接收中断======================================//
  /* Enable CAN1 RX0 interrupt IRQ channel */
  NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
//====================================配置 看门狗 中断======================================//	
	NVIC_InitStructure.NVIC_IRQChannel = WWDG_IRQn;    //WWDG中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	
	NVIC_Init(&NVIC_InitStructure);
//====================================配置 PWM 中断======================================//	
		NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //先占优先级2级
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  //从优先级0级
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
		NVIC_Init(&NVIC_InitStructure); 
//====================================配置 CAPTURE 中断======================================//	
		NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级2级
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  //从优先级0级
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
		NVIC_Init(&NVIC_InitStructure);
//====================================配置  DMA1 中断======================================//	
		NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;  
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //先占优先级2级
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级0级
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
		NVIC_Init(&NVIC_InitStructure);
#ifdef STM32F10X_HD
//====================================配置  DMA2 中断======================================//
		NVIC_InitStructure.NVIC_IRQChannel = DMA2_Channel4_5_IRQn;  
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //先占优先级2级
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  //从优先级0级
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
		NVIC_Init(&NVIC_InitStructure);
//====================================配置  SDIO 中断======================================//	
		NVIC_InitStructure.NVIC_IRQChannel = SDIO_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
#endif
}
