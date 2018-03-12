#ifndef DS1302_H
#define DS1302_H
#include "includes.h"

//extern void InitClock(void);
//extern void ReadDS1302Clock(u8 *p);
//extern void WriteDS1302Clock(u8 *p);
//extern void ReadDSRam(u8 *p,u8 add,u8 cnt);
//extern void WriteDSRam(u8 *p,u8 add,u8 cnt);
//extern void TestDS1302(void);
//void DS1302_Configuration(void);

//*****************DS1302????*******************
#define   RdSec  						0x81
#define   RdMin  						0x83
#define   RdHour  						0x85
#define   RdDate  						0x87
#define   RdMonth  						0x89
#define   RdWeek  						0x8b
#define   RdYear  						0x8d
#define   RdControl          			0x8f
#define   RdTrickleCharge 				0x91
#define   RdClockBurst  				0xbf
#define   WrSec  						0x80
#define   WrMin  						0x82
#define   WrHour  						0x84
#define   WrDate  						0x86
#define   WrMonth  						0x88
#define   WrWeek  						0x8a
#define   WrYear  						0x8c
#define   WrControl         			0x8e
#define   WrTrickleCharge 				0x90
#define   WrClockBurst  				0xbe
//?????
#define   RdRamBurst 					0xbf


//????IO???

#define DS1302_PORT GPIOE

#define DS1302_SCK_PIN GPIO_Pin_2		//???IO?
#define DS1302_IO_PIN GPIO_Pin_3
#define DS1302_CE_PIN GPIO_Pin_4



#define DS1302_CLRSCK() (GPIO_ResetBits(GPIOE, GPIO_Pin_2))		//???IO?????
#define DS1302_SETSCK() (GPIO_SetBits(GPIOE, GPIO_Pin_2))

#define DS1302_CLRIO() (GPIO_ResetBits(GPIOE, GPIO_Pin_3) )
#define DS1302_SETIO() (GPIO_SetBits(GPIOE, GPIO_Pin_3)  )

#define DS1302_CLRCE() (GPIO_ResetBits(GPIOE, GPIO_Pin_4))
#define DS1302_SETCE() (GPIO_SetBits(GPIOE, GPIO_Pin_4))


void DS1302_IO_OUT(void );
void DS1302_IO_IN( void);


//#define DS1302_IO_IN()  DS1302_IO_IN()  //????????
//#define DS1302_IO_OUT() DS1302_IO_OUT()

#define DS1302_READ_SDA()    (GPIO_ReadInputDataBit(DS1302_PORT, DS1302_IO_PIN))



//???????
//typedef struct
//{
//	unsigned char  year;
//	unsigned char month;
//	unsigned char date;
//	unsigned char week;
//	unsigned char hour;
//	unsigned char min;
//	unsigned char sec;
//}TIME_TypeDef;

typedef struct
{
	unsigned char  sec;
	unsigned char min;
	unsigned char hour;
	unsigned char week;
	unsigned char date;
	unsigned char month;
	unsigned char year;
}TIME_TypeDef;
	

//????
void DS1302_Write8bit(unsigned char code);
unsigned char DS1302_Read8bit(void);
//????
extern void ds1302_init (void);
extern unsigned char DS1302_ReadByte(unsigned char con);
extern void DS1302_WriteByte(unsigned char con,unsigned char code);

extern void DS1302_WriteTime(TIME_TypeDef* time);
extern void DS1302_ReadTime(TIME_TypeDef* time);


void time_convert(TIME_TypeDef *time_get);
void readTimeTest(void);

#endif

