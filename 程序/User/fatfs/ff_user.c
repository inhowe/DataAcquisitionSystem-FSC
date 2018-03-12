#include "ff_user.h"

/* 定义例程名和例程发布日期 */
#define EXAMPLE_NAME	"SDIO+FatFs文件系统例程"
#define EXAMPLE_DATE	"2011-10-16"
#define DEMO_VER		"2.0"

/* 用于测试读写速度 */
#define TEST_FILE_LEN			(2*1024*1024)	/* 用于测试的文件长度 */
#define BUF_SIZE				512				/* 每次读写SD卡的最大数据长度 */
uint8_t g_TestBuf[BUF_SIZE];		


/*
*********************************************************************************************************
*	函 数 名: ViewRootDir
*	功能说明: 显示SD卡根目录下的文件名
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/


 void ViewRootDir(void)
{
	/* 本函数使用的局部变量占用较多，请修改启动文件，保证堆栈空间够用 */
	FRESULT result;
	FATFS fs;
	DIR DirInf;  
	FILINFO FileInf;
	
	uint8_t tmpStr[20];
	uint32_t cnt = 0;
	
 	/* 挂载文件系统 */
	result = f_mount(0, &fs);			/* Mount a logical drive */
	if (result != FR_OK)
	{
		printf("挂载文件系统失败 (%d)\r\n", result);
	}

	/* 打开根文件夹 */
	result = f_opendir(&DirInf, "/"); /* 如果不带参数，则从当前目录开始 */
	if (result != FR_OK) 
	{
		printf("打开根目录失败 (%d)\r\n", result);
		return;
	}

	/* 读取当前文件夹下的文件和目录 */
	printf("Name\t\tTyepe\t\tSize\r\n");
	for (cnt = 0; ;cnt++) 
	{
		result = f_readdir(&DirInf,&FileInf); 		/* 读取目录项，索引会自动下移 */
		if (result != FR_OK || FileInf.fname[0] == 0)
		{
			break;
		}
		
		if (FileInf.fname[0] == '.')
		{
			continue;
		}
		
		printf("%s", FileInf.fname);

		if (strlen(FileInf.fname) < 8)	/* 对齐 */
		{
			printf("\t\t");
		}
		else
		{
			printf("\t");
		}
		if (FileInf.fattrib == AM_DIR)
		{
			printf("目录\t\t");
		} 
		else 
		{
			printf("文件\t\t");
		}
		printf("%d\r\n", FileInf.fsize);
		sprintf((char *)tmpStr, "%d", FileInf.fsize);
	}

	/* 卸载文件系统 */
	f_mount(0, NULL);	
}

/*
*********************************************************************************************************
*	函 数 名: CreateNewFile
*	功能说明: 在SD卡创建一个新文件，文件内容填写“www.armfly.com”
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
 void CreateNewFile(void)
{
	/* 本函数使用的局部变量占用较多，请修改启动文件，保证堆栈空间够用 */
	FRESULT result;
	FATFS fs;
	FIL file;
	DIR DirInf;  
	uint32_t bw;
	
 	/* 挂载文件系统 */
	result = f_mount(0, &fs);			/* Mount a logical drive */
	if (result != FR_OK)
	{
		printf("挂载文件系统失败 (%d)\r\n", result);
	}

	/* 打开根文件夹 */
	result = f_opendir(&DirInf, "/"); /* 如果不带参数，则从当前目录开始 */
	if (result != FR_OK) 
	{
		printf("打开根目录失败 (%d)\r\n", result);
		return;
	}

	/* 打开文件 */
//	result = f_open(&file, "armfly.txt",  FA_OPEN_ALWAYS|FA_WRITE);
	result = f_open(&file, "armfly.txt",  FA_WRITE);
	if(result==FR_NO_FILE) //暂时不好用  试试列出目录获得文件名
	{
		f_close(&file);
		printf("result:%d\r\n",result);
		result =f_open(&file, "armfly.txt",  FA_OPEN_ALWAYS);
	}
	printf("result:%d\r\n",result);
	result = f_lseek(&file,file.fsize); //移动指针至末尾
	/* 写一串数据 */
	result = f_write(&file, "FatFS Write Demo \r\n www.armfly.com \r\n", 37, &bw);	
	
	if (result == FR_OK)
	{
		printf("armfly.txt 文件写入成功\r\n");
	}
	else
	{
		printf("armfly.txt 文件写入失败\r\n");
		printf("result:%d\r\n",result);
	}
	
	/* 关闭文件*/
	f_close(&file);
 	
	/* 卸载文件系统 */
	f_mount(0, NULL);	
}

/*
*********************************************************************************************************
*	函 数 名: ReadFileData
*	功能说明: 读取文件armfly.txt前128个字符，并打印到串口
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
 void ReadFileData(void)
{
	/* 本函数使用的局部变量占用较多，请修改启动文件，保证堆栈空间够用 */
	FRESULT result;
	FATFS fs;
	FIL file;
	DIR DirInf;  
	uint32_t bw;
	char buf[128];
	
 	/* 挂载文件系统 */
	result = f_mount(0, &fs);			/* Mount a logical drive */
	if (result != FR_OK)
	{
		printf("挂载文件系统失败(%d)\r\n", result);
	}

	/* 打开根文件夹 */
	result = f_opendir(&DirInf, "/"); /* 如果不带参数，则从当前目录开始 */
	if (result != FR_OK) 
	{
		printf("打开根目录失败(%d)\r\n", result);
		return;
	}

	/* 打开文件 */
	result = f_open(&file, "armfly.txt", FA_OPEN_EXISTING | FA_READ);
	if (result !=  FR_OK)
	{
		printf("Don't Find File : armfly.txt\r\n");
		return;		
	}

	/* 读取文件 */
	result = f_read(&file, &buf, sizeof(buf) - 1, &bw);
	if (bw > 0)
	{
		buf[bw] = 0;
		printf("\r\narmfly.txt 文件内容 : \r\n%s\r\n", buf);
	}
	else
	{
		printf("\r\narmfly.txt 文件内容 : \r\n");		
	}
	
	/* 关闭文件*/
	f_close(&file);
 	
	/* 卸载文件系统 */
	f_mount(0, NULL);
}

/*
*********************************************************************************************************
*	函 数 名: CreateDir
*	功能说明: 在SD卡根目录创建Dir1和Dir2目录，在Dir1目录下创建子目录Dir1_1
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
 void CreateDir(void)
{
	/* 本函数使用的局部变量占用较多，请修改启动文件，保证堆栈空间够用 */
	FRESULT result;
	FATFS fs;
	
 	/* 挂载文件系统 */
	result = f_mount(0, &fs);			/* Mount a logical drive */
	if (result != FR_OK)
	{
		printf("挂载文件系统失败 (%d)\r\n", result);
	}

	/* 创建目录/Dir1 */
	result = f_mkdir("/Dir1"); 
	if (result == FR_OK) 
	{
		printf("f_mkdir Dir1 Ok\r\n");
	}
	else if (result == FR_EXIST)
	{
		printf("Dir1 目录已经存在(%d)\r\n", result);
	}
	else
	{
		printf("f_mkdir Dir1 失败 (%d)\r\n", result);
		return;
	}

	/* 创建目录/Dir2 */
	result = f_mkdir("/Dir2"); 
	if (result == FR_OK) 
	{
		printf("f_mkdir Dir2 Ok\r\n");
	}
	else if (result == FR_EXIST)
	{
		printf("Dir2 目录已经存在(%d)\r\n", result);
	}
	else
	{
		printf("f_mkdir Dir2 失败 (%d)\r\n", result);
		return;
	}			

	/* 创建子目录 /Dir1/Dir1_1	   注意：创建子目录Dir1_1时，必须先创建好Dir1 */
	result = f_mkdir("/Dir1/Dir1_1"); /* */
	if (result == FR_OK) 
	{
		printf("f_mkdir Dir1_1 成功\r\n");
	}
	else if (result == FR_EXIST)
	{
		printf("Dir1_1 目录已经存在 (%d)\r\n", result);
	}
	else
	{
		printf("f_mkdir Dir1_1 失败 (%d)\r\n", result);
		return;
	}
 	
	/* 卸载文件系统 */
	f_mount(0, NULL);	
}

/*
*********************************************************************************************************
*	函 数 名: WriteFileTest
*	功能说明: 测试文件读写速度
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/

uint8_t g_TestBuf[BUF_SIZE];	
 void WriteFileTest(void)
{
	/* 本函数使用的局部变量占用较多，请修改启动文件，保证堆栈空间够用 */
	FRESULT result;
	FATFS fs;
	FIL file;
	DIR DirInf;  
	uint32_t bw;
	uint32_t i;
	uint32_t runtime1,runtime2,timelen;
	uint8_t err = 0;
	
	for (i = 0; i < sizeof(g_TestBuf); i++)
	{
		g_TestBuf[i] = (i % 10) + '0';
	}

  	/* 挂载文件系统 */
	result = f_mount(0, &fs);			/* Mount a logical drive */
	if (result != FR_OK)
	{
		printf("挂载文件系统失败 (%d)\r\n", result);
	}

	/* 打开根文件夹 */
	result = f_opendir(&DirInf, "/"); /* 如果不带参数，则从当前目录开始 */
	if (result != FR_OK) 
	{
		printf("打开根目录失败 (%d)\r\n", result);
		return;
	}

	/* 打开文件 */
	result = f_open(&file, "Speed1.txt", FA_CREATE_ALWAYS | FA_WRITE);
	
	/* 写一串数据 */
	printf("开始写文件 %dKB ...\r\n", TEST_FILE_LEN / 1024);
	runtime1 = SysCnt;	/* 读取系统运行时间 */
	for (i = 0; i < TEST_FILE_LEN / BUF_SIZE; i++)
	{
		result = f_write(&file, g_TestBuf, sizeof(g_TestBuf), &bw);	
		if (result == FR_OK)
		{
			if ((i % 100) == 0)
			{
				//bsp_LedToggle(1);	/* 闪烁LED */
			}
		}
		else
		{
			err = 1;
			printf("Speed1.txt 文件写失败\r\n");
			break;
		}
	}
	runtime2 = SysCnt;	/* 读取系统运行时间 */
	
	if (err == 0)
	{
		timelen = (runtime2 - runtime1);
		printf("  写耗长 : %dms   平均写速度 : %dB/S (%dKB/S)\r\n", timelen, 
			(TEST_FILE_LEN * 1000) / timelen, ((TEST_FILE_LEN / 1024) * 1000) / timelen);
	}
	
	f_close(&file);		/* 关闭文件*/

	/* 开始读文件测试 */
	result = f_open(&file, "Speed1.txt", FA_OPEN_EXISTING | FA_READ);
	if (result !=  FR_OK)
	{
		printf("没有找到文件: Speed1.txtt\r\n");
		return;		
	}

	printf("开始读文件 %dKB ...\r\n", TEST_FILE_LEN / 1024);
	runtime1 = SysCnt;	/* 读取系统运行时间 */
	for (i = 0; i < TEST_FILE_LEN / BUF_SIZE; i++)
	{
		result = f_read(&file, g_TestBuf, sizeof(g_TestBuf), &bw);	
		if (result == FR_OK)
		{
			if ((i % 100) == 0)
			{
				//bsp_LedToggle(1);	/* 闪烁LED */
			}
		}
		else
		{
			err = 1;
			printf("Speed1.txt 文件读失败\r\n");
			break;
		}
	}
	runtime2 = SysCnt;	/* 读取系统运行时间 */
	
	if (err == 0)
	{
		timelen = (runtime2 - runtime1);
		printf("  读耗时 : %dms   平均读速度 : %dB/S (%dKB/S)\r\n", timelen, 
			(TEST_FILE_LEN * 1000) / timelen, ((TEST_FILE_LEN / 1024) * 1000) / timelen);
	}

	/* 关闭文件*/
	f_close(&file);
 	
	/* 卸载文件系统 */
	f_mount(0, NULL);	
}

char filepath[20];
extern unsigned char MotecSend[255];
u8 OpenFileSave=0;
void CreateAFile(const char *filedir, char *filename)
{
	/* 本函数使用的局部变量占用较多，请修改启动文件，保证堆栈空间够用 */
	FRESULT result;
	FATFS fs;
	FIL file;
	DIR DirInf;  
	uint32_t bw;
	
	sprintf(filepath,"%s/%s",filedir,filename);

	
 	/* 挂载文件系统 */
	result = f_mount(0, &fs);			/* Mount a logical drive */
	if (result != FR_OK)
	{
		printf("挂载文件系统失败 (%d)\r\n", result);
	}
	/* 打开根文件夹 */
	result = f_opendir(&DirInf, "/"); /* 如果不带参数，则从当前目录开始 */
	if (result != FR_OK) 
	{
		printf("打开根目录失败 (%d)\r\n", result);
		return;
	}

	/* 打开文件 */
	result = f_open(&file, filepath,  FA_OPEN_ALWAYS|FA_WRITE);

	result = f_lseek(&file,file.fsize); //移动指针至末尾
		/* 写一串数据 */
	result = f_write(&file," QTU DATA ACQUISITION SYSTEM \r\n\r\n", 33, &bw);	

	if (result == FR_OK)
	{
		printf("armfly.txt 文件写入成功\r\n");
		OpenFileSave=1;
	}
	else
	{
		printf("文件写入失败,代码：(%d) \r\n",result);
	}
	
	/* 关闭文件*/
	f_close(&file);
 	
	/* 卸载文件系统 */
	f_mount(0, NULL);	
	
}


char *LastfName=NULL; 

char DirString[20];
const char *DirStringE;
u16 DirNum;
u8 ReadmeFlag=1;
void SearchDir(void)
{
		/* 本函数使用的局部变量占用较多，请修改启动文件，保证堆栈空间够用 */
	FRESULT result;
	FATFS fs;
	DIR DirInf;  
	FILINFO FileInf;
	
	uint8_t tmpStr[20];
	uint32_t cnt = 0;
	
 	/* 挂载文件系统 */
	result = f_mount(0, &fs);			/* Mount a logical drive */
	if (result != FR_OK)
	{
		printf("挂载文件系统失败 (%d)\r\n", result);
	}

	/* 打开根文件夹 */
	result = f_opendir(&DirInf, "/"); /* 如果不带参数，则从当前目录开始 */
	if (result != FR_OK) 
	{
		printf("打开根目录失败 (%d)\r\n", result);
		return ;
	}

	/* 读取当前文件夹下的文件和目录 */

	for (cnt = 0; ;cnt++) 
	{
		result = f_readdir(&DirInf,&FileInf); 		/* 读取目录项，索引会自动下移 */
		if (result != FR_OK || FileInf.fname[0] == 0)
		{
			DirNum++;
			if(DirNum==500)DirNum=1;
			printf("DirNum：%d\r\n",DirNum);
			sprintf(DirString, "/%d", DirNum);
			DirStringE=DirString;
			printf("目录字符串%s\r\n",DirStringE);
			break;
		}
		
		if (FileInf.fname[0] == '.')
		{
			continue;
		}
		
		if (FileInf.fattrib == AM_DIR)
		{
			DirNum = atoi(FileInf.fname);
			//printf("%d\r\n", DirNum);
		}

		if(FileInf.fname=="Readme")
			ReadmeFlag=0;
		
		sprintf((char *)tmpStr, "%d", FileInf.fsize);
	}

	/* 卸载文件系统 */
	f_mount(0, NULL);	
	
}

void CreateOneDir(const char *str)
{
		/* 本函数使用的局部变量占用较多，请修改启动文件，保证堆栈空间够用 */
	FRESULT result;
	FATFS fs;

 	/* 挂载文件系统 */
	result = f_mount(0, &fs);			/* Mount a logical drive */

	if (result != FR_OK)
	{
		printf("挂载文件系统失败 (%d)\r\n", result);
	}

	/* 创建目录/Dir1 */
	result = f_mkdir(str); 

	if (result == FR_OK) 
	{
		//printf("Ok\r\n");
	}
	else if (result == FR_EXIST)
	{
		//printf("Dir1 目录已经存在(%d)\r\n", result);
	}
	else
	{
		printf("f_mkdir Dir1 失败 (%d)\r\n", result);
		return;
	}
 	
	/* 卸载文件系统 */
	f_mount(0, NULL);	
}

void WriteSomething(const char *filedir, char *filename,unsigned char* data,u8 length)
{
		FRESULT result;
	FATFS fs;
	FIL file;
	DIR DirInf;  
	uint32_t bw;
	
	sprintf(filepath,"%s/%s",filedir,filename);

	
 	/* 挂载文件系统 */
	result = f_mount(0, &fs);			/* Mount a logical drive */
	if (result != FR_OK)
	{
		printf("挂载文件系统失败 (%d)\r\n", result);
	}
	/* 打开根文件夹 */
	result = f_opendir(&DirInf, "/"); /* 如果不带参数，则从当前目录开始 */
	if (result != FR_OK) 
	{
		printf("打开根目录失败 (%d)\r\n", result);
		return;
	}

	/* 打开文件 */
	result = f_open(&file, filepath,  FA_OPEN_ALWAYS|FA_WRITE);

	result = f_lseek(&file,file.fsize); //移动指针至末尾
		/* 写一串数据 */
	result = f_write(&file, data , length, &bw);	

	if (result == FR_OK)
	{
		//printf("Successr\n");
	}
	else
	{
		printf("Failure,代码：(%d) \r\n",result);
	}
	
	/* 关闭文件*/
	f_close(&file);
 	
	/* 卸载文件系统 */
	f_mount(0, NULL);	
}

//用户初始化函数
void SD_UserInit(void)
{
	SearchDir();//搜寻目录
	CreateOneDir(DirStringE);//建立新目录
	CreateAFile(DirStringE,"Motec.txt");//在新目录下建立新文件
	CreateAFile(DirStringE,"ADC.txt");
	CreateAFile(DirStringE,"MPU6050.txt");
}
