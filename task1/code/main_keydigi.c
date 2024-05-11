 #include <stdio.h>

#include "c8051f020.h"
#include "stdlib.h"
#include "lcd.h"
#include "led.h"
#include "xjtu_image.h"
#include "1.h"
#include "2.h"
#include "3.h"
//#include "flower.h"
#include <intrins.h>
#define uchar unsigned char          
#define uint  unsigned int 
#define SYSCLK    11059200

#define TIMER_PRESCALER            12  // Based on Timer CKCON settings

#define LED_TOGGLE_RATE            100 // LED toggle rate in milliseconds    1ms
                                       // if LED_TOGGLE_RATE = 1, the LED will
                                       // be on for 1 millisecond and off for
                                       // 1 millisecond
sbit SCLK=P1^3;
sbit SID=P1^4;
sbit CS=P1^5; 
uchar code AC_TABLE1[]={
0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,      //第一行汉字位置
0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,      //第二行汉字位置
0x88,0x89,0x8a,0x8b,0x8c,0x8d,0x8e,0x8f,      //第三行汉字位置
0x98,0x99,0x9a,0x9b,0x9c,0x9d,0x9e,0x9f,      //第四行汉字位置
};
// There are SYSCLK/TIMER_PRESCALER timer ticks per second, so
// SYSCLK/TIMER_PRESCALER/1000 timer ticks per millisecond.
#define TIMER_TICKS_PER_MS  SYSCLK/TIMER_PRESCALER/1000

// Note: LED_TOGGLE_RATE*TIMER_TICKS_PER_MS should not exceed 65535 (0xFFFF)
// for the 16-bit timer

#define AUX1     TIMER_TICKS_PER_MS*LED_TOGGLE_RATE
#define AUX2     -AUX1
#define AUX3     AUX2&0x00FF
#define AUX4     ((AUX2&0xFF00)>>8)

#define TIMER0_RELOAD_HIGH       AUX4  // Reload value for Timer0 high byte
#define TIMER0_RELOAD_LOW        AUX3  // Reload value for Timer0 low byte


sbit INT1 = P0^3; 

unsigned int Time_num; //全局计时用
unsigned int ren_posi;//小人位置
unsigned int zongshu;
unsigned int defen;



//-----------------------------------------------
//系统时钟初始化
//---------------------------------------------
void SYSCLK_Init (void)
{
   int i;                      // 延时计数器
   OSCXCN = 0x67;              // 使用外部晶振
   for (i=0; i<256; i++);      // 等待1ms
   while (!(OSCXCN & 0x80));   // 等待外部晶振稳定
   OSCICN = 0x88;              // 切换到外部晶振
}

void Port_IO_Init()
{
	//set P4[1] to push_pull model
    P74OUT |= 0x01;
	
	//enable crossbar
    XBR2 |= 0x40;

    //close the alam P4.1
    P4 &= 0xfd;
	
	//uart1,int1
	XBR1 |= 0x14;
	XBR2 |= 0x44;
}

//--------------------------------------------------
void Clear(int l)
{

     uchar i;
     WriteCommand(0x30);      //8BitMCU
		 WriteCommand(AC_TABLE1[8*l]);      //起始位置

	   for(i=0;i<16;i++){
     WriteData(0x20);//空格
	}	
}
//-----------------------------------------------------------
void LcmClear( void )
{
     uchar i;
     WriteCommand(0x30);      //8BitMCU
     WriteCommand(0x80);      //AC归起始位
	 for(i=0;i<4;i++){
	 	Clear(i);
	 }
}

void Oscillator_Init()
{
    int i = 0;
    OSCXCN    = 0x67;
    for (i = 0; i < 3000; i++);  // Wait 1ms for initialization
    while ((OSCXCN & 0x80) == 0);
    OSCICN    = 0x08;
}

void Timer0_Init(void)
{
	TH0 = TIMER0_RELOAD_HIGH;           // Reinit Timer0 High register
	TL0 = TIMER0_RELOAD_LOW;
	ET0 = 1;                            // Timer0 interrupt enabled
	TMOD = 0x01;                        // 16-bit Mode Timer0
	//TCON |= 0x10;                        // Timer0 ON
}

void Interrupt_Init()
{
	//INT1
	//low level triggle
	IT1 = 0;
	//enable INT1
	EX1 = 1;
	//enable all interrupt
	EA = 1;
}

void Init_Device(void)
{
    //disable watchdog
    EA = 0;
    WDTCN = 0xde;
    WDTCN = 0xad;
    EA = 1;
	
    Oscillator_Init();
    Port_IO_Init();
	Timer0_Init();
	
	Interrupt_Init();
	
    LcdInit();
	LedInit();
}


uchar code str0[]="                   ";
uchar code str1[]="  接樱花!!!  ";
uchar code str2[]="  中间键开始！    左右键移动  ";
void PutStr(uchar row,uchar col,uchar *puts)
{
     WriteCommand(0x30);      //8BitMCU
     WriteCommand(AC_TABLE1[8*row+col]);      //起始位置
     while(*puts !='\0')      //判断字符串是否显示完毕
     {
           if(col==8)            //判断换行
           {            //若不判断,则自动从第一行到第三行
                 col=0;
                 row++;
           }
           if(row==4) row=0;      //一屏显示完,回到屏左上角
           WriteCommand(AC_TABLE1[8*row+col]);
           WriteData(*puts);      //一个汉字要写两次
           puts++;
           WriteData(*puts);
           puts++;
           col++;
     }
}

void Delay_us(unsigned int times)
{
	unsigned int i,j;
	
	for (i = 0; i< times; i++)
	{
		for (j = 0; j < 11; j++)
		{
			_nop_();
    }
  }
}

//void shanshuo(void)
//{      
//	while(1)
//	{
//		if(begin==1)
//			{	
//					ImageShow();
//////					xuehua_posi_lie[m+1]=xuehua_posi_lie[m]++;
////					m++;
////				
//  				Delay_ms(500);
//			}
//			else LcmClear();
//		}
//}
//-----------------------------------------------------------------------------
// 主程序
//-----------------------------------------------------------------------------

void main()
{	
  Init_Device();
	
	INT1 = 1;
	PutStr(0, 0, str0);
    PutStr(1, 0, str1);
    PutStr(2, 0, str2);
	zongshu=0;
	defen=0;
	ren_posi=4;
	
    while(1)
    {
			LedDispNum();   //显示三行数据
//			if(Time_num%50==0)
//			{
//					ImageShow();
//			}
			P5 = 0xFB;Delay_us(5);
			P5 = 0xFD;Delay_us(5);
			P5 = 0xFE;Delay_us(5);
    }
}

void INT1_ISR(void) interrupt 2
{
	Delay_ms(100);

	switch(P5)
	{
		//sub
		case 0xfb:   //左键
		//Time_num = 0; //计时清零	
		//LcmClear();
		ren_posi=ren_posi-1;
		ren_posi=(ren_posi)%8;
		ImageShow_ren();
		break;	
		
		case 0xfd:   //中间键
		TR0 = 1;  // 开始计时
		Time_num = 1000;
		LcmClear();
		ren_posi=4;
		defen=0;
		zongshu=0;
		ImageShow_ren();
		ImageShow();
		//shanshuo();
		break;
		
		case 0xfe:    //右键
        //TR0 = 0;  // 关闭计时
		//LcmClear();
		//TR0=1;
		ren_posi=ren_posi+1;
		ren_posi=(ren_posi)%8;
		ImageShow_ren();	
		break;
	}
}

void Timer0_ISR (void) interrupt 1
{
	TH0 = TIMER0_RELOAD_HIGH;           // Reinit Timer0 High register
	TL0 = TIMER0_RELOAD_LOW;
	Time_num--;
	if(Time_num%30==0)
	{
		ImageShow();
	}
	if(Time_num == 0) 
	{
		PutStr(0, 0, str0);
		PutStr(1, 0, str1);
		PutStr(2, 0, str2);
		TR0 = 0;  // 关闭计时
	}
}



