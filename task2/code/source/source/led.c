#include "led.h"

/*select�Լ�display���������е�һ����ѡ������ܣ����ϵ��´�����1-12���ڶ�������ʾ����*/

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

void Delay_us(unsigned int times)//��ʱus
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
void display(int a)  // ��ʾ����
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

void select(int b)  // ѡ�������
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

void LedDispNum()  //�������ʾ����
{
	uchar temp[4];
	
	temp[0] = Time_num%10;// ��ĸ�λ
	temp[1] = Time_num%100/10;
	temp[2] = Time_num%1000/100;  
	temp[3] = Time_num/1000;

	// ���ϱ�һ����ʾ
	select(4);display(temp[0]); Delay(500); P7 = 0xff;
	select(3);display(temp[1]); Delay(500); P7 = 0xff;
	select(2);display(temp[2]); P7 = P7 & ~0x80; Delay(500); P7 = 0xff;            //��С����
	select(1);display(temp[3]); if(temp[3] == 0) P7 = 0xff; Delay(500); P7 = 0xff; //����

	// �м�һ��
	select(8);display(1); Delay(500); P7 = 0xff;
	select(7);display(4); Delay(500); P7 = 0xff;
	select(6);display(1); Delay(500); P7 = 0xff;
	select(5);display(7); Delay(500); P7 = 0xff;  

	// �±�һ��
	select(12);display(8); Delay(500); P7 = 0xff;
	select(11);display(9); Delay(500); P7 = 0xff;
	select(10);display(4); Delay(500); P7 = 0xff;
	select(9) ;display(1); Delay(500); P7 = 0xff;  
}

/*.����ƽ���˲�
����������
����ȡN������ֵ����ƽ�����㣬Nֵ�ϴ�ʱ���ź�ƽ���Ƚϸߣ��������Ƚϵ�
Nֵ��Сʱ���ź�ƽ���Ƚϵͣ��������Ƚϸߡ�Nֵ��ѡȡ��һ��12���ҡ�
�ŵ㣺
��Ӧ�ڶ�һ�����������ŵ��źŽ����˲��������źŵ��ص�����һ��ƽ��ֵ���ź���ĳһ��ֵ��Χ�������²���
ȱ�㣺
���ڲ����ٶȽ�����Ҫ�����ݼ����ٶȽϿ��ʵʱ���Ʋ������ã��Ƚ��˷�RAM*/

void Average_filter(void)//����ƽ���˲�
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
/*.��λֵƽ���˲��������������ƽ���˲�����
����������
�൱����λֵ�˲�+����ƽ���˲�����������N�����ݣ�ȥ��һ�����ֵ��һ����Сֵ��Ȼ�����N-2�����ݵ�����ƽ��ֵ��
Nֵ��ѡȡ��3-14
�ŵ㣺�ں��������˲������ŵ�
����żȻ���ֵ������Ը��ţ������������������������Ĳ���ֵƫ�
ȱ�㣺
�����ٶȽ��������㷨ƽ���˲�һ�����˷�RAM��*/
void Median_average_filter(void)//��ֵƽ���˲�
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
/*������ƽ���˲�����
����������
������ȡN������ֵ����һ�����У����еĳ��ȹ̶�ΪN��ÿ�β�����һ�������ݷ����β�����ӵ�ԭ�����׵�һ�����ݣ��Ƚ��ȳ�����
�Ѷ����е�N�����ݽ�������ƽ�����㣬�Ϳɻ���µ��˲������Nֵ��ѡȡ��һ��12.
�ŵ㣺
�������Ը��������õ��������ã�ƽ���ȸߣ���Ӧ�ڸ�Ƶ�񵴵�ϵͳ
ȱ�㣺
�����ȵͣ���żȻ���ֵ������Ը��ŵ��������ýϲ��������������������������Ĳ���ֵƫ���������������űȽ����صĳ���
�˷�RAM*/
  int i = 0;
void Sliding_filter(void)//����ƽ���˲�
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

/*�� 2 �ַ��� ��λֵ�˲���
A ����
�������� N �Σ�N ȡ�������� N �β���ֵ����С����ȡ�м�ֵΪ������Чֵ
B �ŵ�
����Ч�˷���żȻ��������Ĳ������Ŷ��¶ȡ�Һλ�ı仯�����ı�����������õ��˲�Ч��
C ȱ��
���������ٶȵȿ��ٱ仯�Ĳ�������
D ʵ������
N ֵ�ɸ���ʵ���������
�������ð�ݷ�*/
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

/* �޷��˲������ֳƳ����ж��˲�����
A ����
���ݾ����жϣ�ȷ�����β�����������ƫ��ֵ����Ϊ A��ÿ�μ�⵽��ֵʱ�жϣ��������ֵ���ϴ�ֵ
֮��<=A,�򱾴�ֵ��Ч�������ֵ���ϴ�ֵ֮��>A,�򱾴�ֵ��Ч,��������ֵ,���ϴ�ֵ���汾��ֵ
B �ŵ�
����Ч�˷���żȻ����������������
C ȱ��
�޷��������������Եĸ���ƽ���Ȳ�
D ʵ������
/* A ֵ�ɸ���ʵ���������
value Ϊ��Чֵ��new_value Ϊ��ǰ����ֵ
�˲����򷵻���Ч��ʵ��ֵ */

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