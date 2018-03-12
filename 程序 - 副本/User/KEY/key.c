#include "key.h"

void KeyInit(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
 	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD, ENABLE);
	
	//换挡按键
  GPIO_InitStructure.GPIO_Pin  = (GPIO_Pin_6|GPIO_Pin_7);				 					 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
  GPIO_Init(GPIOC, &GPIO_InitStructure);
	//DRS开关(D6)+预留按键
	GPIO_InitStructure.GPIO_Pin  = (GPIO_Pin_6|GPIO_Pin_7);				 					 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
  GPIO_Init(GPIOD, &GPIO_InitStructure);			
	
}
	//
	//KeyValue值解析
	//0x01=0000 0001 升档命令
	//0x02=0000 0010 降档命令
	//0x04=0000 0100 DRS开
	//0x08=0000 1000 预留按键
	//0xFF=1111 1111 按下标志位
	u8 KeyValue=0;
u8 Key_Scan(void)          //该函数不适用于中断函数
{	
//	if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_6)==0)//升档按键
//	{
//		key_delay_ms(5);
//		if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_6)==0)
//		{
//			while(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_6)==0);
//			Shifting(UP);
//			//KeyValue=KeyValue|0x01;
//		}
//	}
//	if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_7)==0)//降档按键 
//	{
//		key_delay_ms(5);
//		if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_7)==0)
//		{
//			while(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_7)==0);
//			Shifting(DOWN);
//			//KeyValue=KeyValue|0x02;
//		}
//	}
//	//if(KeyValue==0xC0);
//	if(GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_6)==0)//DRS开关
//	{
//		key_delay_ms(5);
//		if(GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_6)==0)
//		{
//			while(GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_6)==0);
//			//KeyValue=KeyValue|0x04;
//		}
//	}
//	if(GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_7)==0)//预留按键
//	{
//		key_delay_ms(5);
//		if(GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_7)==0)
//		{
//			while(GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_7)==0);
//			//KeyValue=KeyValue|0x08;
//		}
//	}
//	return(KeyValue);             //返回键值

	if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_6)==0)//升档按键
	{
		if(KeyValue==0)			//如果两次进入这里
		{	KeyValue=KeyValue|0xFF;	return KeyValue;}
		else if(KeyValue==0xFF)
		{	KeyValue=0x01;return KeyValue;}
	}
	else if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_7)==0)//降档按键
	{
		if(KeyValue==0)
		{	KeyValue=KeyValue|0xFF;	return KeyValue;}
		else if(KeyValue==0xFF)
		{	KeyValue=0x02;return KeyValue;}
	}
	//if(KeyValue==0xC0);
	else if(GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_6)==0)//DRS
	{
		if(KeyValue==0)
		{	KeyValue=KeyValue|0xFF;	return KeyValue;}
		else if(KeyValue==0xFF)
		{	KeyValue=0x04;return KeyValue;}
	}
	else if(GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_7)==0)//预留按键
	{
		if(KeyValue==0)
		{	KeyValue=KeyValue|0xFF;	return KeyValue;}
		else if(KeyValue==0xFF)
		{	KeyValue=0x08;return KeyValue;}
	}
	else KeyValue=0;
	
	return(KeyValue);             //返回键值
}

void Key_Deal(void)
{
	if(KeyValue==0x01)
	{KeyValue=0;}
	if(KeyValue==0x02)
	{KeyValue=0;}
	if(KeyValue==0x04)
	{KeyValue=0;}
	if(KeyValue==0x08)
	{KeyValue=0;}
}
void key_delay_ms(vu32 m)
{	 		  	  
	u32 i; 
	for(; m != 0; m--)	
       for (i=0; i<72000; i++);
} 

