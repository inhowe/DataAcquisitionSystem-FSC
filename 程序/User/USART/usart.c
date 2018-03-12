#include "usart.h"

void USART1_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	/* 配置USART时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);
	//GPIO_PinRemapConfig(GPIO_Remap_USART1,ENABLE);
	/* USART1 GPIO口 设置 */
	/* Configure USART1 Tx (PB.06) as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);    
	/* Configure USART1 Rx (PA.10) as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/* USART1 mode config */
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);

	/* 使能串口1接收中断 */
	//USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	
	USART_Cmd(USART1, ENABLE);
}


void USART1_IRQHandler(void)
{
	 
}

// 重定向c库函数printf到USART1
int fputc(int ch, FILE *f)
{
		/* 发送一个字节数据到USART1 */
		USART_SendData(USART1, (uint8_t) ch);
		
		/* 等待发送完毕 */
		while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);		
	
		return (ch);
}

/// 重定向c库函数scanf到USART1
int fgetc(FILE *f)
{
		/* 等待串口1输入数据 */
		while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);

		return (int)USART_ReceiveData(USART1);
}

//USART"1"发送数组
//格式:(首地址,长度)
void SendArray(unsigned char *loc,unsigned char num)
{
	int i=0;
	for(i=0;i<num;i++)
	{
		USART_SendData(USART1,loc[i]);
	 while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	}
}
//



///*
// * 函数名：itoa
// * 描述  ：将整形数据转换成字符串
// * 输入  ：-radix =10 表示10进制，其他结果为0
// *         -value 要转换的整形数
// *         -buf 转换后的字符串
// *         -radix = 10
// * 输出  ：无
// * 返回  ：无
// * 调用  ：被USART2_printf()调用
// */
//static char *itoa(int value, char *string, int radix)
//{
//    int     i, d;
//    int     flag = 0;
//    char    *ptr = string;

//    /* This implementation only works for decimal numbers. */
//    if (radix != 10)
//    {
//        *ptr = 0;
//        return string;
//    }

//    if (!value)
//    {
//        *ptr++ = 0x30;
//        *ptr = 0;
//        return string;
//    }

//    /* if this is a negative value insert the minus sign. */
//    if (value < 0)
//    {
//        *ptr++ = '-';

//        /* Make the value positive. */
//        value *= -1;
//    }

//    for (i = 10000; i > 0; i /= 10)
//    {
//        d = value / i;

//        if (d || flag)
//        {
//            *ptr++ = (char)(d + 0x30);
//            value -= (d * i);
//            flag = 1;
//        }
//    }

//    /* Null terminate the string. */
//    *ptr = 0;

//    return string;

//} /* NCL_Itoa */


//中断缓存串口数据
#define UART_BUFF_SIZE      255
volatile    uint8_t uart_p = 0;
uint8_t     uart_buff[UART_BUFF_SIZE];

void bsp_USART2_IRQHandler(void)
{
    if(uart_p<UART_BUFF_SIZE)
    {
        if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
        {
            uart_buff[uart_p] = USART_ReceiveData(USART2);
            uart_p++;
        }
    }
}

//获取接收到的数据和长度
char *get_rebuff(uint8_t *len)
{
    *len = uart_p;
    return (char *)&uart_buff;
}

void clean_rebuff(void)
{
    uart_p = 0;
}


///////////////////////////////////
/*********************************************END OF FILE**********************/
