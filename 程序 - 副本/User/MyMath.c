#include "MyMath.h"


//加权平均滤波
char coe[50] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,13,14,15,16,17,18,19,20,
								21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,
								41,42,43,44,45,46,47,48,49,50};    // 权值

#define SAMPLINGTIME 1 
int sum_coe = (1+SAMPLINGTIME)*SAMPLINGTIME/2;      //权值之和,20表示采样20次,coe数组只用前20个

//参数:(数组首地址,列)
//求相应二维数组的列的平均值
u16 AveWeightedRecurFilt(u16 *array,u8	list) 
{
  u8 i;
  int filter_sum = 0;
  int temp;
  for(i = 0; i < SAMPLINGTIME*9; i+=9) 
  {
//		temp=(*(&array[i]+list));
		temp=*(array+list+i);
		filter_sum +=temp	/**coe[i]*/;
  }
  filter_sum /= SAMPLINGTIME;
  return filter_sum;
}

///*求平均值	n是通道次序*/
//u16 Average (u16 *array,u8	list)
//{
//	u32 i,a=0;
//	for(i=1;i<50;i++)
//	{
//		 a= a+(array[i]+list);
//	}
//	a=a/50;
//	return a;
//}
