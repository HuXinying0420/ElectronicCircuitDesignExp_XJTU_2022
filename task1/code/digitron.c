#include "digitron.h"                 
#define uchar unsigned char
#define uint unsigned int

uchar code tab[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90};//0，1，2，3，4，5，6,7，8，9

void SYSCLK_Init_digi (void)
{
   int i;                      // ?????
   OSCXCN = 0x67;              // ????22.1184MHz??
   for (i=0; i<256; i++);      // ??1ms
   while (!(OSCXCN & 0x80));   // ?????????
   OSCICN = 0x88;              // ???????
}

void Delay_us_digi(unsigned int times)
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

//void main (void) 
//{

//	WDTCN = 0xde;
//	WDTCN = 0xad;   //????????
//	SYSCLK_Init ();
//	while(1)
//	{
//		
//        /*上一右一显示6*/
//				P5 = 0XF7;
//				P6 = 0XFF;
//				P7 = 0x82;	
//		    Delay_us(500);
//        /*上一右二显示0.*/	
//				P5 = 0XFB;
//				P6 = 0XFF;
//				P7 = 0x40;	      		
//				Delay_us(500);
//		    /*上一右三显示0*/
//				P5 = 0XFD;
//				P6 = 0XFF;
//				P7 = 0xC0;
//				Delay_us(500);
//		    /*上一右四显示1*/
//				P5 = 0XFE;
//				P6 = 0XFF;
//				P7 = 0xF9;
//				Delay_us(500);
//				/*中右一显示 1*/
//				P5 = 0X7F;
//				P6 = 0XFF;
//				P7 = 0xC0;
//				Delay_us(500);
//				/*中右二显示2*/
//				P5 = 0XBF;
//				P6 = 0XFF;
//				P7 = 0xA4;
//				Delay_us(500);
//				/*中右3显示5*/
//				P5 = 0XDF;
//				P6 = 0XFF;
//				P7 = 0x92;
//				Delay_us(500);
//				/*下左一显示1*/
//				P5 = 0XFF;
//				P6 = 0XFE;
//				P7 = 0xF9;
//				Delay_us(500);
//        /*下左二显示3*/				
//				P5 = 0XFF;
//				P6 = 0XFD;
//				P7 = 0xB0;
//				Delay_us(500);
//				/*下左三显示1*/
//				P5 = 0XFF;
//				P6 = 0XFB;
//				P7 = 0xF9;
//				Delay_us(500);
//				/*下左四显示4*/
//				P5 = 0XFF;
//				P6 = 0XF7;
//				P7 = 0x99;
//				Delay_us(500);
//		}
//	}

