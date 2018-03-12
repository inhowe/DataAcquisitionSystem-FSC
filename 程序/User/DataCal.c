#include "DataCal.h"

u32 CAPTURE_Buff[4]={0,0,0,0};
extern u8  TIM4CH1_CAPTURE_STA,TIM4CH2_CAPTURE_STA,  //输入捕获状态   
					 TIM4CH3_CAPTURE_STA,TIM4CH4_CAPTURE_STA;
					 
extern u16 TIM4CH1_CAPTURE_VAL_A,TIM4CH1_CAPTURE_VAL_B, //输入捕获值
					 TIM4CH2_CAPTURE_VAL_A,TIM4CH2_CAPTURE_VAL_B,
					 TIM4CH3_CAPTURE_VAL_A,TIM4CH3_CAPTURE_VAL_B,	
					 TIM4CH4_CAPTURE_VAL_A,TIM4CH4_CAPTURE_VAL_B;		
//捕获时间计算
void CAPTURE_Cal(void)
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
			TIM4CH3_CAPTURE_STA=0;      //开启下一次捕获
		}
}

