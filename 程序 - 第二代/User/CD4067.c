#include "CD4067.h"

//初始化
void CD4067Init(void)
{
	 GPIO_InitTypeDef  GPIO_InitStructure;
 	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
	
	//DCBA
  GPIO_InitStructure.GPIO_Pin  = (GPIO_Pin_3|GPIO_Pin_2|GPIO_Pin_1|GPIO_Pin_0);				 					 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
  GPIO_Init(GPIOE, &GPIO_InitStructure);
	//INH
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_4;				 					 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
  GPIO_Init(GPIOE, &GPIO_InitStructure);			
	
}

//选择切换通道
//@parameter ch:0-15
void CD4067_SelectChannel(u8 ch)
{
	switch (ch)
	{
		case 0 :GPIO_Write(GPIOE,0x00);break;
		case 1 :GPIO_Write(GPIOE,0x01);break;
		case 2 :GPIO_Write(GPIOE,0x02);break;
		case 3 :GPIO_Write(GPIOE,0x03);break;
		case 4 :GPIO_Write(GPIOE,0x04);break;
		case 5 :GPIO_Write(GPIOE,0x05);break;
		case 6 :GPIO_Write(GPIOE,0x06);break;
		case 7 :GPIO_Write(GPIOE,0x07);break;
		case 8 :GPIO_Write(GPIOE,0x08);break;
		case 9 :GPIO_Write(GPIOE,0x09);break;
		case 10:GPIO_Write(GPIOE,0x0a);break;
		case 11:GPIO_Write(GPIOE,0x0b);break;
		case 12:GPIO_Write(GPIOE,0x0c);break;
		case 13:GPIO_Write(GPIOE,0x0d);break;
		case 14:GPIO_Write(GPIOE,0x0e);break;
		case 15:GPIO_Write(GPIOE,0x0f);break;
		default:GPIO_Write(GPIOE,0x10);break;
	}
}

//自动切换通道，执行一次切换一次
void CD4067_AutoRotate(void)
{
	static u8 cnt=0;
	CD4067_SelectChannel(cnt++);
	if(cnt==16)cnt=0;
}

//该函数是用户函数，不属于CD4067底层驱动
extern u16 volatile ADCSample[1];
void CD4067_ADCToBuff(void)
{
	static u8 cnt=0;
	ADCBuff[cnt++]=ADCSample[0];
//	if(cnt==sizeof(ADCBuff))cnt=0;
	if(cnt==10)cnt=0;
	CD4067_AutoRotate();
}
