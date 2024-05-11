#include"c8051f020.h"
#include <intrins.h>
#include <stdio.h>

#define uchar unsigned char          
#define uint  unsigned int 

#define SYSCLK    11059200


//-----------------------------------------------------------------------------
// 液晶接口定义
//-----------------------------------------------------------------------------
sbit SCLK=P1^3;
sbit SID=P1^4;
sbit CS=P1^5; 

//-----------------------------------------------------------------------------
// 函数定义
//-----------------------------------------------------------------------------
void SYSCLK_Init (void);
void PORT_Init (void);
void LCMInit(void);
void Delay_ms(uchar t);
void Delay(uint k);

void WriteData(unsigned char WDLCM);
void WriteCommand (uchar Cbyte);
void LcmClear( void );
void Clear(int l);
//unsigned char ReadData(void);
void CheckBusy(void);
void SendByte(uchar Dbyte);
uchar ReceiveByte(void);
void PutChar(uchar row,uchar col,uchar ch);
void PutStr(uchar row,uchar col,uchar *puts);

uchar code AC_TABLE[]={
0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,      //第一行汉字位置
0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,      //第二行汉字位置
0x88,0x89,0x8a,0x8b,0x8c,0x8d,0x8e,0x8f,      //第三行汉字位置
0x98,0x99,0x9a,0x9b,0x9c,0x9d,0x9e,0x9f,      //第四行汉字位置
};

//--------------------------------------------------
uchar code str0[]="班级：自动化91";
uchar code str1[]="姓名：弓知鑫";
uchar code str2[]="学号：2191421852";


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

//---------------------------------------------
//端口初始化
//---------------------------------------------
void PORT_Init (void)
{
    XBR0     = 0X04;         //设置XBR0
    XBR1     = 0X1E;         //设置XBR1
    XBR2     = 0X44;         //打开交叉开关

	P0MDOUT|= 0x03;
	P1MDOUT=0xFF;			 //端口1设置为推挽输出
	P2MDOUT=0xFF;			 //端口2设置为推挽输出
	P3MDOUT=0xFF;			 //端口3设置为推挽输出

}

//---------------------------------------------
//延时ms
//---------------------------------------------
void Delay_ms(unsigned int times)
{
    unsigned int i,j;

    for (i=0; i<times; i++) 
		for (j=0; j<1300; j++);                       
 
}

//-----------------------------------------------------------------------------
// 延时
//-----------------------------------------------------------------------------
void Delay(unsigned int k)
{ 
 do 
	{   
    
    }while( --k ); 
}

//-----------------------------------------------
//液晶初始化
//-----------------------------------------------
void LcmInit( void )
{

     WriteCommand(0x30);      //8BitMCU
     WriteCommand(0x03);      //AC?0,不改变DDRAM值
     WriteCommand(0x0C);      //显示ON,游标OFF,游标位反白OFF
     WriteCommand(0x01);      //清屏,AC归0
     WriteCommand(0x06);      //写入时,游标右移动
}

//-------------------------------------------------
void WriteCommand(unsigned char Cbyte )
{
     CS = 1;
     CheckBusy();
     SendByte(0xf8);            //11111,RW(0),RS(0),0
     SendByte(0xf0&Cbyte);      //高四位
     SendByte(0xf0&Cbyte<<4);//低四位
     CS = 0;
}

//-------------------------------------------------
void SendByte(uchar Dbyte)
{
     uchar i;
     for(i=0;i<8;i++)
     {
           SCLK = 0;
           Dbyte=Dbyte<<1;      //左移一位
           SID = CY;            //移出的位给SID
           SCLK = 1;
           SCLK = 0;
     }
}

//-------------------------------------------------
void CheckBusy( void )
{
     do   SendByte(0xfc);      //11111,RW(1),RS(0),0
     while(0x80&ReceiveByte());      //BF(.7)=1 Busy
}

//-------------------------------------------------
uchar ReceiveByte(void)
{
     uchar i,temp1,temp2;
     temp1 = 0;
     temp2 = 0;
     for(i=0;i<8;i++)
     {
           temp1=temp1<<1;
           SCLK = 0;
           SCLK = 1;            
           SCLK = 0;
           if(SID) temp1++;
     }
     for(i=0;i<8;i++)
     {
           temp2=temp2<<1;
           SCLK = 0;
           SCLK = 1;
           SCLK = 0;
           if(SID) temp2++;
     }
     return ((0xf0&temp1)+(0x0f&temp2));
}

//--------------------------------------------------------------
void PutStr(uchar row,uchar col,uchar *puts)
{
     WriteCommand(0x30);      //8BitMCU
     WriteCommand(AC_TABLE[8*row+col]);      //起始位置
     while(*puts !='\0')      //判断字符串是否显示完毕
     {
           if(col==8)            //判断换行
           {            //若不判断,则自动从第一行到第三行
                 col=0;
                 row++;
           }
           if(row==4) row=0;      //一屏显示完,回到屏左上角
           WriteCommand(AC_TABLE[8*row+col]);
           WriteData(*puts);      //一个汉字要写两次
           puts++;
           WriteData(*puts);
           puts++;
           col++;
     }
}

//-------------------------------------------------------------
/*void PutChar(uchar row,uchar col,uchar ch)
{
     	WriteCommand(0x30);      //8BitMCU
     	WriteCommand(AC_TABLE[8*row+col]);      //起始位置
        WriteData(ch);
}*/

//-------------------------------------------------------------
void WriteData(uchar Dbyte )
{
     CS = 1;
     CheckBusy();
     SendByte(0xfa);            //11111,RW(0),RS(1),0
     SendByte(0xf0&Dbyte);      //高四位
     SendByte(0xf0&Dbyte<<4);//低四位(先执行<<)
     CS = 0;
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

//--------------------------------------------------
void Clear(int l)
{

     uchar i;
     WriteCommand(0x30);      //8BitMCU
		 WriteCommand(AC_TABLE[8*l]);      //起始位置

	   for(i=0;i<16;i++){
     WriteData(0x20);//空格
	}	
}

//-----------------------------------------------------------------------------
// 主程序
//-----------------------------------------------------------------------------

//void main()
//{ 

//  WDTCN = 0xde;
//  WDTCN = 0xad;                //关闭看门狗 
//	
//	WDTCN = 0xA5;

//  SYSCLK_Init();	
//  PORT_Init();  	
//  LCMInit(); 
//	LcmClear();
//  PutStr(0, 0, str0);
//  PutStr(1, 0, str1);
//  PutStr(2, 0, str2);

//  while(1);

//}  