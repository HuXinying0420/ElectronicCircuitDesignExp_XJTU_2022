#include "led.h"

/*select以及display函数，其中第一个是选择数码管，从上到下从左到右1-12，第二个是显示数字*/

extern int xdata v[10];
extern int xdata u[10];
extern long ADC0_result[2];
extern int w1;
extern int x1;
extern int y1;
extern int z1;
extern int w2;
extern int x2;
extern int y2;
extern int z2;
extern int result;
extern long xdata result2;
extern int w3;
extern int x3;
extern int y3;
extern int z3;
extern volatile unsigned char idata samplepos;
extern volatile long idata slidesum;

void Delay_us(unsigned int times)//延时us
{
    unsigned int i;

    for (i=0; i<times; i++)
    {
        _nop_();
        _nop_();
        _nop_();
		   _nop_();
    }
}
void display(int a)  // 显示数字
{
	switch(a)
	{
		case 0:P7=0xC0;break;
		case 1:P7=0xF9;break;
		case 2:P7=0xA4;break;
		case 3:P7=0xB0;break;
		case 4:P7=0x99;break;
		case 5:P7=0x92;break;
		case 6:P7=0x82;break;
		case 7:P7=0xF8;break;
		case 8:P7=0x80;break;
		case 9:P7=0x90;break;
		case 10:P7=0x88;break;//A
		case 11:P7=0x83;break;
		case 12:P7=0xC6;break;
		case 13:P7=0xA1;break;
		case 14:P7=0x86;break;
		case 15:P7=0x8e;break;
		default:P7=0xc0;break;
	}
}

void select(int b)  // 选择数码管
{
	switch(b)
	{			
		case 1: P5 = 0XFE;P6 = 0XFF;break;
		case 2: P5 = 0XFD;P6 = 0XFF;break;
		case 3: P5 = 0XFB;P6 = 0XFF;break;
		case 4: P5 = 0XF7;P6 = 0XFF;break;
		case 5: P5 = 0XEF;P6 = 0XFF;break;
		case 6: P5 = 0XDF;P6 = 0XFF;break;
		case 7: P5 = 0XBF;P6 = 0XFF;break;
		case 8: P5 = 0X77;P6 = 0XFF;break;
		case 9: P6 = 0XFE;P5 = 0XFF;break;
		case 10:P6 = 0XFD;P5 = 0XFF;break;
		case 11:P6 = 0XFB;P5 = 0XFF;break;
		case 12:P6 = 0XF7;P5 = 0XFF;break;
	}
}

void Led_Port_Init()
{
    //set to push_pull model
    P74OUT &= 0xff;

    //enable crossbar
    XBR2 |= 0x40;
}

void LedInit()
{
	Led_Port_Init();
	P5 |= 0xff;
	P6 |= 0x0f;
	P7 = 0xff;
}

void LedDispNum()  //数码管显示数字
{
	uchar temp[4];
	
	temp[0] = Time_num%10;// 秒的个位
	temp[1] = Time_num%100/10;
	temp[2] = Time_num%1000/100;  
	temp[3] = Time_num/1000;

	// 最上边一行显示
	select(4);display(temp[0]); Delay(500); P7 = 0xff;
	select(3);display(temp[1]); Delay(500); P7 = 0xff;
	select(2);display(temp[2]); P7 = P7 & ~0x80; Delay(500); P7 = 0xff;            //加小数点
	select(1);display(temp[3]); if(temp[3] == 0) P7 = 0xff; Delay(500); P7 = 0xff; //除零

	// 中间一行
	select(8);display(1); Delay(500); P7 = 0xff;
	select(7);display(4); Delay(500); P7 = 0xff;
	select(6);display(1); Delay(500); P7 = 0xff;
	select(5);display(7); Delay(500); P7 = 0xff;  

	// 下边一行
	select(12);display(8); Delay(500); P7 = 0xff;
	select(11);display(9); Delay(500); P7 = 0xff;
	select(10);display(4); Delay(500); P7 = 0xff;
	select(9) ;display(1); Delay(500); P7 = 0xff;  
}

/*.算术平均滤波
方法解析：
连续取N个采样值进行平均运算，N值较大时：信号平滑度较高，但灵敏度较低
N值较小时：信号平滑度较低，但灵敏度较高。N值的选取：一般12左右。
优点：
适应于对一般具有随机干扰的信号进行滤波，这样信号的特点是有一个平均值，信号在某一数值范围附近上下波动
缺点：
对于测量速度较慢或要求数据计算速度较快的实时控制并不适用，比较浪费RAM*/

void Average_filter(void)//算术平均滤波
{
	long i = 0,t = 0;
	long sum = 0;
	for ( i = 0 ; i < 10 ; i++ )
  {
     //sum = sum + (int)(ADC0_result[1]-270)*1.101;
     sum = sum + v[i]; 		
  }
	t = sum /10; 
	w1 =  t*5/4096;
	x1 = (t*50)/4096-w1*10;
	y1 = (t*500)/4096-w1*100-x1*10;
	z1 = (t*5000)/4096-w1*1000-x1*100-y1*10;
	/*w1 =  t/1000;
	x1 = (t - w1*1000)/100;
	y1 = (t - w1*1000-x1*100)/10;
	z1 =  t - w1*1000-x1*100-y1*10; */
	result=(int)(t);
	result2=t*5000/4096;
	//return t;
}
/*.中位值平均滤波法（防脉冲干扰平均滤波法）
方法解析：
相当于中位值滤波+算术平均滤波，连续采样N个数据，去掉一个最大值和一个最小值，然后计算N-2个数据的算术平均值。
N值的选取：3-14
优点：融合了两种滤波法的优点
对于偶然出现的脉冲性干扰，可消除由于脉冲干扰所引起的采样值偏差。
缺点：
测量速度较慢，和算法平均滤波一样，浪费RAM。*/
void Median_average_filter(void)//中值平均滤波
{
	long i,sum, average,max,min;
	average = 0;
	max = v[0];
	min = max;
	sum = 0;
	for ( i = 0; i<10;i++)
   {
      sum = sum + v[i];
		  if(v[i] > max)
			{
        max = v[i];
      }
		  if(v[i] < min)
			{
        min = v[i];
      }	
   }
	sum = sum - max -min;
	average = sum /8 ;
	w1 =  average*5/4096;
	x1 = (average*50)/4096-w1*10;
	y1 = (average*500)/4096-w1*100-x1*10;
	z1 = (average*5000)/4096-w1*1000-x1*100-y1*10;
	/*w1 =  average/1000;
	x1 = (average-w1*1000)/100;
	y1 = (average - w1*1000-x1*100)/10;
	z1 =  average - w1*1000-x1*100-y1*10; */
	result=(int)(average);
	 result2=average*5000/4096;
	//return average;
}
/*（滑动平均滤波法）
方法解析：
把连续取N个采样值看成一个队列，队列的长度固定为N，每次采样到一个新数据放入队尾，并扔掉原来队首的一次数据（先进先出）。
把队列中的N个数据进行算术平均运算，就可获得新的滤波结果。N值的选取：一般12.
优点：
对周期性干扰有良好的抑制作用，平滑度高，适应于高频振荡的系统
缺点：
灵敏度低，对偶然出现的脉冲性干扰的抑制作用较差。不易消除由于脉冲干扰所引起打的采样值偏差，不适用于脉冲干扰比较严重的场合
浪费RAM*/
  int i = 0;
void Sliding_filter(void)//滑动平均滤波
{
	long s;
//	int count;
//	int sum=0;
//	i++;
//	v[i] = (int)(ADC0_result[1]-270)*1.101;
//	if ( i == 12 ) i = 0;
//	for ( count = 1;count < 13;count++) sum = sum + v[count];

	slidesum-=v[samplepos];
	slidesum+=v[(samplepos+9)%10];
	s = slidesum /9;
	//if(s<0) s=0;
	if(s>4095) s=4095;
	//s = s/4096*5000;
  w1 =  s*5/4096;
	x1 = (s*50)/4096-w1*10;
	y1 = (s*500)/4096-w1*100-x1*10;
	z1 = (s*5000)/4096-w1*1000-x1*100-y1*10;
	/*w1 =  s/1000;
	x1 = (s-w1*1000)/100;
	y1 = (s - w1*1000-x1*100)/10;
	z1 =  s - w1*1000-x1*100-y1*10;*/
	result=(int)(s);
	result2=s*5000/4096;
	//return s;
}

/*第 2 种方法 中位值滤波法
A 方法
连续采样 N 次（N 取奇数）把 N 次采样值按大小排列取中间值为本次有效值
B 优点
能有效克服因偶然因素引起的波动干扰对温度、液位的变化缓慢的被测参数有良好的滤波效果
C 缺点
对流量、速度等快速变化的参数不宜
D 实例程序
N 值可根据实际情况调整
排序采用冒泡法*/
/*void Median_filter()
{
  int i,j,temp,mid;
	for (j=0;j<10-1;j++)
	{
		for (i=0;i<10-j;i++)
		{
			if ( u[i]>u[i+1] )
			{
			temp = u[i];
			u[i] = u[i+1];
			u[i+1] = temp;
			}
		}
	}
  mid = (int)(u[4]+u[5])/2 +1;

	w3 =  mid/1000;
	x3 = (mid-w3*1000)/100;
	y3 = (mid - w3*1000-x3*100)/10;
	z3 =  mid - w3*1000-x3*100-y3*10; 
}		 */

/* 限幅滤波法（又称程序判断滤波法）
A 方法
根据经验判断，确定两次采样允许的最大偏差值（设为 A）每次检测到新值时判断：如果本次值与上次值
之差<=A,则本次值有效如果本次值与上次值之差>A,则本次值无效,放弃本次值,用上次值代替本次值
B 优点
能有效克服因偶然因素引起的脉冲干扰
C 缺点
无法抑制那种周期性的干扰平滑度差
D 实例程序
/* A 值可根据实际情况调整
value 为有效值，new_value 为当前采样值
滤波程序返回有效的实际值 */

//void Limiting_filter()
//{
//int new_value;
//new_value = (int)(ADC0_result[1]-270)*1.101;
//if ( ( new_value - value > A ) || ( value - new_value > A )
//return value;
//return new_value;
//}


void show_f_row(void)
{
	select(1);display(w1);P7 = P7 & ~0x80;Delay(500); P7 = 0xff;
	select(2);display(x1);Delay(500); P7 = 0xff;
	select(3);display(y1);Delay(500); P7 = 0xff;
	select(4);display(z1);Delay(500); P7 = 0xff;
}
void show_s_row(void)
{
	select(5);display(w2);P7 = P7 & ~0x80;Delay(500); P7 = 0xff;
	select(6);display(x2);Delay(500); P7 = 0xff;
	select(7);display(y2);Delay(500); P7 = 0xff;
	select(8);display(z2);Delay(500); P7 = 0xff;
}
void show_t_row(void)
{
	select(9);display(w3);P7 = P7 & ~0x80;Delay(500); P7 = 0xff;
	select(10);display(x3);Delay(500); P7 = 0xff;
	select(11);display(y3);Delay(500); P7 = 0xff;
	select(12);display(z3);Delay(500); P7 = 0xff;
}