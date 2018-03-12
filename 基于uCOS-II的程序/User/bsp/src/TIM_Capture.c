#include "TIM_Capture.h"
#include "stdio.h"



//arr：自动重装值	
//psc：时钟预分频数
/*推荐配置TIM4_Cap_Init(0xFFFF,71)以1Mhz频率计数,65536us一个中断
*/
void TIM4_Cap_Init(u16 arr,u16 psc)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
//   NVIC_InitTypeDef NVIC_InitStructure;
	TIM_ICInitTypeDef  TIM4_ICInitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4 , ENABLE);	//使能TIM4时钟
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  //使能GPIOB时钟
	
	//GPIO配置好才会稳定不易被干扰
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB,GPIO_Pin_6);						
	GPIO_SetBits(GPIOB,GPIO_Pin_7);
	GPIO_SetBits(GPIOB,GPIO_Pin_8);						
	GPIO_SetBits(GPIOB,GPIO_Pin_9);
	
	//初始化定时器4 TIM4	 
	TIM_TimeBaseStructure.TIM_Period = arr; //设定计数器自动重装值 
	TIM_TimeBaseStructure.TIM_Prescaler =psc; 	//预分频器   
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
  
	//初始化TIM4输入捕获参数
	TIM4_ICInitStructure.TIM_Channel = TIM_Channel_1; //CC1S=01 
  TIM4_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling;	//下降沿捕获
  TIM4_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //映射到TI1上
  TIM4_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //配置输入分频,不分频 
  TIM4_ICInitStructure.TIM_ICFilter = 0x08;//过滤8个周期以下的
  TIM_ICInit(TIM4, &TIM4_ICInitStructure);
	
	TIM4_ICInitStructure.TIM_Channel = TIM_Channel_2; 
  TIM4_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling;
  TIM4_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; 
  TIM4_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 
  TIM4_ICInitStructure.TIM_ICFilter = 0x08;
  TIM_ICInit(TIM4, &TIM4_ICInitStructure);
	
	TIM4_ICInitStructure.TIM_Channel = TIM_Channel_3; 
  TIM4_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling;
  TIM4_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; 
  TIM4_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 
  TIM4_ICInitStructure.TIM_ICFilter = 0x08;
  TIM_ICInit(TIM4, &TIM4_ICInitStructure);
	
	TIM4_ICInitStructure.TIM_Channel = TIM_Channel_4; 
  TIM4_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling;	
  TIM4_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; 
  TIM4_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 
  TIM4_ICInitStructure.TIM_ICFilter = 0x08;
  TIM_ICInit(TIM4, &TIM4_ICInitStructure);
	
	//移至nvic.c里
//	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //先占优先级2级
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  //从优先级0级
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
//	NVIC_Init(&NVIC_InitStructure);  
	
	TIM_ITConfig(TIM4,TIM_IT_Update|TIM_IT_CC1|TIM_IT_CC2|TIM_IT_CC3|TIM_IT_CC4,ENABLE);//允许溢出更新中断和捕获中断	
																						/*注意这里不要漏掉TIM_IT_CC2*/
   TIM_Cmd(TIM4,ENABLE );//使能定时器4
}
/////////////////////////
//TIMxCHx_CAPTURE_STA解释
//[7]		:0,没有成功的捕获;1,成功捕获到一次.
//[6]		:0,还没捕获到高电平;1,已经捕获到高电平了.
//[5:0]	:捕获高电平后溢出的次数
u8  TIM4CH1_CAPTURE_STA=0,TIM4CH2_CAPTURE_STA=0,TIM4CH3_CAPTURE_STA=0,TIM4CH4_CAPTURE_STA=0; //输入捕获状态  
u16 TIM4CH1_CAPTURE_VAL_A,TIM4CH2_CAPTURE_VAL_A,TIM4CH3_CAPTURE_VAL_A,TIM4CH4_CAPTURE_VAL_A; //输入捕获值
u16 TIM4CH1_CAPTURE_VAL_B,TIM4CH2_CAPTURE_VAL_B,TIM4CH3_CAPTURE_VAL_B,TIM4CH4_CAPTURE_VAL_B;

//TIM4中断服务函数
void TIM4_IRQHandler(void)
{     
	u16 tsr;
	OS_CPU_SR  cpu_sr;

	OS_ENTER_CRITICAL();                         
	OSIntEnter();
	OS_EXIT_CRITICAL();
	
	tsr=TIM4->SR;
	//通道1捕获事件
/*1*/if((TIM4CH1_CAPTURE_STA&0X80)==0)//还未成功捕获
	{
/*3(b)*/if(tsr&0X01)//溢出事件
		{
			if(TIM4CH1_CAPTURE_STA&0X40)//已经捕获到高电平了
			{
				if((TIM4CH1_CAPTURE_STA&0X3F)==0X3F)//高电平太长了
				{
					TIM4CH1_CAPTURE_STA|=0X80;//标记成功捕获了一次
					TIM4CH1_CAPTURE_VAL_B=0XFFFF;
				} else TIM4CH1_CAPTURE_STA++;
			}	
		}
/*3(a)*/if(tsr&0x02)//捕获事件
		{
			if(TIM4CH1_CAPTURE_STA&0X40)    //捕获到下一个下降沿
			{ 
				TIM4CH1_CAPTURE_STA|=0X80;  //标记成功捕获到一次高电平脉宽
				TIM4CH1_CAPTURE_VAL_B=TIM4->CCR1;//获取当前的捕获值.
				TIM4->CCER&=~(1<<1);      //CC1P=0  设置为上升沿捕获		1<<1 是第一通道，若其他通道要改变，改成什么看参考手册即可
			}
/*2*/		else                   //还未开始,第一次捕获下降沿
			{
				TIM4CH1_CAPTURE_STA=0;    //清空
				TIM4CH1_CAPTURE_VAL_B=0;
				TIM4CH1_CAPTURE_STA|=0X40;  //标记捕获到了下降沿
				//TIM4->CNT=0;          //计数器清空,不清空可以实现多通道捕获
				TIM4CH1_CAPTURE_VAL_A=TIM4->CCR1;
				TIM4->CCER|=1<<1;          //CC1P=1  设置为下降沿捕获
			} 
		}                   
	}
	//通道2捕获事件
	if((TIM4CH2_CAPTURE_STA&0X80)==0)
	{
		if(tsr&0X01)//溢出	  
		{
			if(TIM4CH2_CAPTURE_STA&0X40)
			{
				if((TIM4CH2_CAPTURE_STA&0X3F)==0X3F)
				{
					TIM4CH2_CAPTURE_STA|=0X80;
					TIM4CH2_CAPTURE_VAL_B=0XFFFF;
				} else TIM4CH2_CAPTURE_STA++;
			}	
		}
		if(tsr&0x04)//捕获				 //====不同之处:0x02->0x04
		{
			if(TIM4CH2_CAPTURE_STA&0X40)   
			{ 
				TIM4CH2_CAPTURE_STA|=0X80; 
				TIM4CH2_CAPTURE_VAL_B=TIM4->CCR2;//====不同之处:CCR1->CCR2
				TIM4->CCER&=~(1<<5);      
			}
		else                   
			{
				TIM4CH2_CAPTURE_STA=0;    
				TIM4CH2_CAPTURE_VAL_B=0;
				TIM4CH2_CAPTURE_STA|=0X40;  
				TIM4CH2_CAPTURE_VAL_A=TIM4->CCR2;	//====不同之处:CCR1->CCR2
				TIM4->CCER|=1<<5;          
			} 
		}                   
	}
  //通道3捕获事件
	if((TIM4CH3_CAPTURE_STA&0X80)==0)
	{
		if(tsr&0X01)//溢出	  
		{
			if(TIM4CH3_CAPTURE_STA&0X40)
			{
				if((TIM4CH3_CAPTURE_STA&0X3F)==0X3F)
				{
					TIM4CH3_CAPTURE_STA|=0X80;
					TIM4CH3_CAPTURE_VAL_B=0XFFFF;
				} else TIM4CH3_CAPTURE_STA++;
			}	
		}
		if(tsr&0x08)//捕获				 //====不同之处:0x02->0x04
		{
			if(TIM4CH3_CAPTURE_STA&0X40)   
			{ 
				TIM4CH3_CAPTURE_STA|=0X80; 
				TIM4CH3_CAPTURE_VAL_B=TIM4->CCR3;//====不同之处:CCR1->CCR2
				TIM4->CCER&=~(1<<9);      
			}
		else                   
			{
				TIM4CH3_CAPTURE_STA=0;    
				TIM4CH3_CAPTURE_VAL_B=0;
				TIM4CH3_CAPTURE_STA|=0X40;  
				TIM4CH3_CAPTURE_VAL_A=TIM4->CCR3;	//====不同之处:CCR1->CCR2
				TIM4->CCER|=1<<9;          
			} 
		}                   
	}
	//通道4捕获事件
	if((TIM4CH4_CAPTURE_STA&0X80)==0)
	{
		if(tsr&0X01)//溢出	  
		{
			if(TIM4CH4_CAPTURE_STA&0X40)
			{
				if((TIM4CH4_CAPTURE_STA&0X3F)==0X3F)
				{
					TIM4CH4_CAPTURE_STA|=0X80;
					TIM4CH4_CAPTURE_VAL_B=0XFFFF;
				} else TIM4CH4_CAPTURE_STA++;
			}	
		}
		if(tsr&0x10)//捕获				 //====不同之处:0x02->0x04
		{
			if(TIM4CH4_CAPTURE_STA&0X40)   
			{ 
				TIM4CH4_CAPTURE_STA|=0X80; 
				TIM4CH4_CAPTURE_VAL_B=TIM4->CCR4;//====不同之处:CCR1->CCR2
				TIM4->CCER&=~(1<<13);      
			}
		else                   
			{
				TIM4CH4_CAPTURE_STA=0;    
				TIM4CH4_CAPTURE_VAL_B=0;
				TIM4CH4_CAPTURE_STA|=0X40;  
				TIM4CH4_CAPTURE_VAL_A=TIM4->CCR4;	//====不同之处:CCR1->CCR2
				TIM4->CCER|=1<<13;          
			} 
		}                   
	}
	TIM4->SR=0;//清除中断标志位     
	OSIntExit();  
}
//end of file
