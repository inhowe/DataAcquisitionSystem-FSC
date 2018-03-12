#include"exti.h"

void  EXTI_Configuration(void)
{


	/*GPIO Configuration*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);
{
	/* 定义GPIO初始化结构体 GPIO_InitStructure */
  	GPIO_InitTypeDef GPIO_InitStructure;
    
  	/* 设置PA.0为上拉输入（EXTI Line0）*/
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  	GPIO_Init(GPIOA , &GPIO_InitStructure);

	/* 定义PA.0,PA.1,PA.2,PA.3为外部中断0输入通道（EXIT0,EXIT1,EXIT2,EXIT3） */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA , GPIO_PinSource0);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA , GPIO_PinSource1);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA , GPIO_PinSource2);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA , GPIO_PinSource3);

}

/**********************************************************************/
	/*EXTI_InitStructure*/
{
	/* 定义EXIT初始化结构体 EXTI_InitStructure */
	EXTI_InitTypeDef EXTI_InitStructure;

	/* 设置外部中断0通道（EXIT Line0）在下降沿时触发中断 */  
  	EXTI_InitStructure.EXTI_Line = EXTI_Line0|EXTI_Line1|EXTI_Line2|EXTI_Line3 ;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);
}

/********************************************************************/
{
	/* 定义NVIC初始化结构体 NVIC_InitStructure */
	NVIC_InitTypeDef NVIC_InitStructure;	
  	/* 选择NVIC优先级分组2 */
  	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  
  	/* 使能EXIT 0通道 ，2级先占优先级 ，0级次占优先级 */
  	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQChannel;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);

	/* 使能EXIT 1通道 ，2级先占优先级 ，0级次占优先级 */
  	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQChannel;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);

	/* 使能EXIT 2通道 ，2级先占优先级 ，0级次占优先级 */
  	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQChannel;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);

	/* 使能EXIT 3通道 ，2级先占优先级 ，0级次占优先级 */
  	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQChannel;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);


}

	EXTI_ClearFlag(EXTI_Line0);	 //防止初始化就产生中断
	EXTI_ClearFlag(EXTI_Line1);	 //防止初始化就产生中断
	EXTI_ClearFlag(EXTI_Line2);	 //防止初始化就产生中断
	EXTI_ClearFlag(EXTI_Line3);	 //防止初始化就产生中断


	
}

//void EXTI0_IRQHandler(void)
//{	
////	EXTI_GenerateSWInterrupt(EXTI_Line1);
//	extern u8 state,begin;
//	if(state==0)
//	{TIM_Cmd(TIM3, ENABLE);state=1;begin=1;} 		             //TIM1总开关：开启
//	else
//	{TIM_Cmd(TIM3, DISABLE);state=0;} 		             //TIM1总开关：开启
//	
//	printf("\r\nEXIT0 IRQHandler return.\r\n");
//	
//	EXTI_ClearFlag(EXTI_Line0);
//}




