#include "adc.h"

/*				计算公式：
*	 		m1=Average(0)*3.3/4096.0 ;
*   		m2=Average(1)*3.3/4096.0;
*/
u16 volatile ADCSample[1];
u16 volatile ADCBuff[10];

void ADC1_Configuration(void)
{
	{
		GPIO_InitTypeDef GPIO_InitStructure;
		RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO , ENABLE);

		GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_7|GPIO_Pin_6|GPIO_Pin_5|GPIO_Pin_4;
		GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_AIN;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		
		GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_5;
		GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_Out_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		GPIO_SetBits(GPIOA,GPIO_Pin_5);
	}

	{	
		ADC_InitTypeDef ADC_InitStructure; 
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 , ENABLE);//开启ADC时钟
		RCC_ADCCLKConfig(RCC_PCLK2_Div6); //设置ADC时钟 PCLK2的6分频
		ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;//ADC1工作在独立模式
		ADC_InitStructure.ADC_ScanConvMode = ENABLE;//开启多通道扫描模式
		ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;//开启连续工作模式
		ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//转换由软件而不是外部触发启动
		ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//ADC数据右对齐
		ADC_InitStructure.ADC_NbrOfChannel = 1;//规定了顺序进行规则转换的ADC通道的数目
		ADC_Init(ADC1, &ADC_InitStructure);
	 
	   /* ADC1 规则转换顺序 */ 
	  ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 1, ADC_SampleTime_55Cycles5);
//		ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 2, ADC_SampleTime_55Cycles5);
//		ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 3, ADC_SampleTime_55Cycles5);
//		ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 4, ADC_SampleTime_55Cycles5);
		
	   ADC_DMACmd(ADC1, ENABLE);//开启DMA传输
	   ADC_Cmd(ADC1, ENABLE);//开启ADC1
	   ADC_ResetCalibration(ADC1);//校准ADC1
	   while(ADC_GetResetCalibrationStatus(ADC1));//等待校准完毕
	 
	   /* Start ADC1 calibration */
	   ADC_StartCalibration(ADC1);
	   /* Check the end of ADC1 calibration */
	   while(ADC_GetCalibrationStatus(ADC1));
	      
	   /* Start ADC1 Software Conversion */ 
	   ADC_SoftwareStartConvCmd(ADC1, ENABLE);//使能或者失能指定的ADC的软件转换启动功能
   }
}
void DMA1_Configuration(void)
{
   DMA_InitTypeDef DMA_InitStructure;
   RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);//使能或者失能AHB外设时钟

   DMA_DeInit(DMA1_Channel1);//将DMA的通道x寄存器重设为缺省值
   DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;//定义DMA外设基地址
   DMA_InitStructure.DMA_MemoryBaseAddr = (u32)ADCSample;//定义DMA内存基地址 放到数组里
   DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;//规定了外设是作为数据传输的目的地还是来源，如果内存作为来源，则内存变量
   DMA_InitStructure.DMA_BufferSize = 1*2;	//通道数*采样量*字节
   DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设地址寄存器不变
   DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//内存地址寄存器递增
   DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord ;//外设数据宽度为16位
   DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord ;//内存数据宽度为16位
   DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;//循环缓存
   DMA_InitStructure.DMA_Priority = DMA_Priority_High;//DMA通道x拥有高优先级
   DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;//DMA通道x没有设置为内存到内存传输 
   DMA_Init(DMA1_Channel1, &DMA_InitStructure);//根据DMA_InitStruct中指定的参数初始化DMA的通道x寄存器
   
   /* Enable DMA1 channel1 */
   DMA_Cmd(DMA1_Channel1, ENABLE);
   DMA_ClearFlag(DMA1_FLAG_TC1);
   DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);//使能或者失能指定的通道x中断
}

void DMA1_ADC1_Init(void)
{
	DMA1_Configuration();
	ADC1_Configuration();
}

void DMA1_Channel1_IRQHandler(void)
{
	DMA_ClearITPendingBit(DMA1_IT_TC1);
}

