#ifndef __FF_USER_H
#define __FF_USER_H

#include "includes.h"
#include "stm32f10x.h"
#include "bsp_sdio_sd.h"  	/* SD卡驱动模块 */
#include "ff.h"				/* FatFS文件系统模块*/	

/* 仅允许本文件内调用的函数声明 */
 void ViewRootDir(void);
 void CreateNewFile(void);
 void ReadFileData(void);
 void CreateDir(void);
 void WriteFileTest(void);

void SearchDir(void);
void CreateOneDir(const char *str);
void CreateAFile(const char *filedir, char *filename);
void WriteSomething(const char *filedir, char *filename,unsigned char* data,u8 length);

void SD_UserInit(void);
#endif
