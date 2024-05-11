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
void Lcd_Port_Init(void);
void LcdInit(void);

void Delay_ms(uint t);
void Delay(uint k);

void SendByte(uchar Dbyte);
uchar ReceiveByte(void);
void CheckBusy(void);

void WriteData(uchar WDLCM);
void WriteCommand (uchar Cbyte);

void LineClear(uchar l);
void LcdClear( void );

void WriteChar(uchar row,uchar col,uchar ch);
void WriteStr(uchar row,uchar col,uchar *puts);

void ImageShow(uchar *imagePtr);

const unsigned char code xjtu_image1[1024] = { /* 0X00,0X01,0X80,0X00,0X40,0X00, */
0X60,0X00,0XD6,0XFD,0X67,0X80,0X08,0X00,0X00,0X00,0X00,0X10,0X00,0X94,0XBC,0X00,
0XBF,0X00,0X87,0XDF,0X8D,0X00,0X5C,0X00,0X00,0X00,0X00,0X06,0X01,0XE6,0X98,0X00,
0XEC,0X01,0XFF,0X17,0X09,0XA2,0X7F,0X80,0X00,0X00,0X00,0XDE,0X07,0XC1,0XB0,0X00,
0XC5,0X03,0XFF,0XEB,0XC8,0X7F,0X7B,0X84,0X00,0X00,0X25,0X8B,0XF8,0X01,0XA0,0X00,
0X80,0X6F,0XFF,0X79,0X5F,0XB7,0X47,0X9C,0X00,0X00,0X04,0X0D,0XE0,0X61,0X80,0X00,
0XC3,0XD7,0XFF,0XFB,0XBF,0X03,0X03,0X27,0XC8,0X00,0X28,0XFB,0XF0,0X40,0X1C,0X00,
0XD7,0XFF,0XFF,0X7F,0X38,0X1F,0XE1,0XBE,0X08,0X00,0X35,0XC0,0XC0,0X00,0X1C,0X00,
0XFF,0XFF,0XE1,0XFE,0XDC,0X00,0X11,0X71,0X08,0X00,0X42,0X0A,0X00,0X41,0XDC,0X40,
0XFF,0XFF,0XFF,0X7D,0XDC,0X10,0X61,0XED,0X58,0X00,0X02,0X00,0X20,0X83,0XD9,0XC0,
0XFF,0XFF,0XFF,0XFE,0XDF,0X68,0X13,0XFD,0X04,0X00,0X04,0X00,0X40,0X1D,0XC5,0XCF,
0XFF,0XFF,0XFF,0XFF,0XFA,0X76,0X77,0X6E,0XB7,0X80,0X00,0X00,0X00,0XF5,0X1C,0XC0,
0XFF,0XFF,0XFF,0XFF,0XFB,0XFF,0XA1,0X13,0XF3,0X80,0X00,0X00,0X01,0XDC,0X6C,0X81,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XE3,0X63,0XA3,0XC0,0X00,0X00,0X0E,0XC3,0X6C,0X07,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XF1,0XE1,0X40,0X40,0X00,0X00,0X7E,0XD7,0X60,0X1F,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFD,0X8F,0X80,0X00,0X00,0X03,0X6C,0X3B,0X80,0X7F,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XEF,0X83,0X00,0X00,0X0F,0XF3,0XBB,0X03,0XFF,
0XFF,0XF8,0XFF,0XF9,0XFF,0XFF,0X7F,0XE6,0X3C,0X00,0X00,0X0D,0X0F,0XB8,0X0F,0XFF,
0XFF,0X78,0XFF,0XFB,0XBF,0XFE,0XFB,0XF9,0X7F,0XF0,0X00,0X08,0XFB,0X80,0X7F,0XFF,
0XFF,0XE9,0XEC,0XFF,0XFF,0X1E,0XC5,0XF2,0XFE,0X00,0X00,0X07,0X58,0X03,0XFF,0XFF,
0XFF,0XF3,0XE7,0XFF,0XFF,0X9F,0XC4,0XEF,0X80,0X00,0X00,0X57,0X40,0X1F,0XFF,0XFF,
0XFF,0XE3,0XEF,0XFF,0XFF,0X07,0XFA,0XE0,0X00,0X00,0X00,0X75,0X00,0XFF,0XFF,0XFF,
0X82,0X73,0XF7,0XFF,0XFF,0X00,0X00,0X00,0X00,0X00,0X00,0X30,0X0F,0XFF,0XFF,0XFF,
0X00,0X00,0X00,0X00,0X00,0X00,0X10,0X00,0X00,0X00,0X00,0X00,0XFF,0XFF,0XFF,0XF0,
0X00,0X00,0X01,0X00,0X20,0X30,0X78,0X00,0X00,0X00,0X00,0X1F,0XFF,0XFF,0XFF,0XC0,
0X00,0X00,0X03,0XC0,0XE0,0X38,0X7C,0X00,0X00,0X00,0X03,0XDF,0XF7,0XFF,0XFF,0XC0,
0X00,0X00,0X07,0X80,0X60,0X60,0XF8,0X00,0X00,0X00,0X7F,0X8F,0XFF,0XFC,0XFF,0X80,
0X00,0X00,0X03,0X01,0XE0,0X78,0XF8,0X00,0X00,0X0F,0XFF,0X8F,0XF8,0X51,0XFF,0XCE,
0X00,0X00,0X07,0X01,0XF0,0X90,0X70,0X00,0X0F,0XFF,0XFF,0X9E,0X00,0X03,0XFF,0XFC,
0X00,0X00,0X19,0X80,0X31,0X00,0X30,0X3F,0XFF,0XFF,0XFF,0XE0,0X00,0X03,0XFF,0XDA,
0X00,0X00,0X00,0X00,0X00,0X1F,0XFF,0XFF,0XFF,0XFF,0XFE,0X00,0X00,0X0F,0XFF,0XD0,
0XFF,0XFF,0XFF,0XFF,0XFE,0X7F,0XFF,0XFF,0XFF,0XDF,0XFC,0X00,0X00,0X1F,0XFF,0XC1,
0XFF,0XFF,0XFF,0XFF,0XFE,0X7F,0XFF,0XF8,0X02,0X0F,0XFF,0XC0,0X01,0XFF,0XFF,0XFE,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFC,0X80,0X00,0X00,0X0F,0XFF,0X20,0X00,0X7F,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0X9E,0X00,0X80,0X00,0X1E,0X27,0XDF,0X40,0X01,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0X9F,0X00,0X80,0X03,0X7F,0XEB,0X07,0X60,0X03,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XBE,0XC1,0X00,0X19,0XFF,0XF8,0X07,0XD4,0X01,0XFF,0XFF,0XFF,
0XFF,0XFF,0XCF,0XFF,0XBF,0X02,0X00,0X77,0XFF,0XFC,0X07,0XF7,0X8E,0XFF,0XFF,0XFF,
0XFF,0XFF,0XC1,0X78,0XBF,0X26,0X00,0XFF,0XFD,0XFC,0X07,0XFF,0XCB,0XFF,0XFF,0XFF,
0XFF,0XFF,0XC0,0X00,0XBF,0XAC,0X00,0XFF,0XFF,0XFC,0X07,0XFF,0XE7,0XFF,0XFF,0XFF,
0XFF,0XFF,0XC0,0X00,0XBF,0X3C,0X03,0XFF,0XFF,0XFC,0X07,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0X80,0X00,0X3F,0XF7,0XDF,0XFF,0XFF,0XFC,0X07,0XF7,0XFF,0XFF,0XFF,0XEF,
0XFF,0XF5,0XFF,0XF8,0X3F,0XEF,0XCF,0XFF,0XFF,0XFE,0X0F,0XFF,0XBF,0XF0,0X00,0X03,
0XFF,0XEF,0XFF,0XFC,0X3F,0XFF,0XFF,0XFF,0XFF,0XFE,0X08,0X00,0X82,0X9F,0XFF,0XFF,
0XFF,0XEF,0XFF,0XFC,0X3F,0XFF,0XFF,0XFF,0XF8,0X26,0X0F,0XF8,0X37,0XDF,0XFF,0XFF,
0XFF,0XEF,0XFF,0XFC,0X3F,0XFF,0XFF,0XFF,0XDE,0X2E,0X0D,0XF8,0X3F,0X5F,0XFF,0XFF,
0XFF,0XEF,0XFF,0XFC,0XBF,0XFF,0XFF,0XFF,0X50,0X20,0X0B,0XFC,0X0F,0XCF,0XFF,0XFF,
0XFF,0XEF,0XFF,0XFC,0X3D,0XBF,0XFE,0XFF,0X90,0X24,0X01,0XC0,0X5F,0XC0,0X00,0X1E,
0XFF,0XEF,0XFF,0XFC,0X36,0X1F,0XFF,0XFF,0XD0,0X2C,0X0C,0X00,0X40,0X00,0X00,0X0A,
0XFF,0XCC,0X00,0X00,0X3E,0XBF,0XEF,0X8B,0XDE,0X20,0X08,0X00,0X62,0X00,0X01,0X07,
0XFF,0XC0,0X00,0X00,0XFF,0XD1,0X0F,0XE0,0X0F,0XFF,0XF8,0XC3,0XE0,0X00,0X00,0X17,
0XFF,0XF8,0X00,0X07,0XFF,0XFC,0X00,0X18,0X01,0XFE,0X00,0X00,0X00,0X00,0X00,0X00,
0XFF,0XF8,0X0F,0XFF,0XF0,0X00,0X30,0X18,0X7C,0X7C,0X00,0X00,0X00,0X00,0X00,0X00,
0XFF,0XFF,0XFF,0XF2,0XFF,0XFF,0XFF,0XFF,0XFC,0X3F,0XFF,0XFF,0XF8,0X00,0X00,0X00,
0XFF,0XFF,0XFF,0XE3,0XFF,0XFD,0XF7,0XFF,0XFE,0X1F,0XFF,0XF4,0X00,0XAF,0XFF,0XFF,
0XFF,0XC7,0XFF,0XFF,0XEF,0XFF,0XFF,0XFF,0XFF,0X0F,0XC0,0XFF,0X60,0X31,0XFF,0XFF,
0XFF,0X8F,0XFF,0X7F,0XFF,0XFF,0XFF,0XFF,0XFC,0X03,0XFF,0XFF,0XC0,0X4B,0XFF,0XFF,
0XFF,0XFF,0X83,0XFF,0XFF,0XFF,0XFE,0X00,0X3F,0X83,0XFF,0XFF,0XE6,0X03,0XFF,0XFF,
0XFF,0XF8,0X00,0XFF,0XFF,0XFC,0X00,0X7F,0XFF,0X81,0XFF,0XFF,0XF8,0X1F,0XFF,0XFF,
0XC0,0X00,0X7F,0XFF,0XF8,0X01,0XFF,0XFF,0XFF,0XC0,0XFF,0XFF,0XE0,0X0F,0XFF,0XFF,
0XFF,0XFF,0XFF,0XF0,0X0F,0XFF,0XE0,0X7F,0XFF,0XE0,0X7F,0XFF,0XCF,0X03,0XFF,0XFF,
0XFF,0XFF,0X00,0X1F,0XFF,0XFF,0XC3,0XFF,0XFF,0XE0,0X3F,0XFF,0XF3,0XFF,0XFF,0XFF,
0XFC,0X00,0X7F,0XFF,0XFF,0XFD,0XFC,0XFF,0XFF,0XE0,0X1F,0XFF,0X9E,0X0F,0XFF,0XFF,
0X07,0XFF,0XFF,0XFF,0XFF,0XFF,0XC3,0XFF,0XFF,0XF0,0X0F,0XFF,0XF0,0X1F,0XFF,0XFF,
0X00,0X00,0X0F,0XFF,0XFF,0XFF,0XC7,0XFF,0XFF,0XF8,0X07,0XFF,0XEC,0X1F,0XFF,0XFF,
};

//constant definition
uchar code AC_TABLE[]=
{
    0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,      //第一行汉字位置
    0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,      //第二行汉字位置
    0x88,0x89,0x8a,0x8b,0x8c,0x8d,0x8e,0x8f,      //第三行汉字位置
    0x98,0x99,0x9a,0x9b,0x9c,0x9d,0x9e,0x9f,      //第四行汉字位置
};



void Lcd_Port_Init(void)
{
	//set P1[5:3] to push_pull model
    P1MDOUT |= 0x38;
	
	//enable crossbar
    XBR2 |= 0x40;
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

void Oscillator_Init()
{
    int i = 0;
    OSCXCN    = 0x67;
    for (i = 0; i < 3000; i++);  // Wait 1ms for initialization
    while ((OSCXCN & 0x80) == 0);
    OSCICN    = 0x08;
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
	
    LcdInit();
}

void LcdInit( void )
{
    Lcd_Port_Init();

    WriteCommand(0x30);      //8BitMCU
    WriteCommand(0x03);      //AC?0,不改变DDRAM值
    WriteCommand(0x0C);      //显示ON,游标OFF,游标位反白OFF
    WriteCommand(0x01);      //清屏,AC归0
    WriteCommand(0x06);      //写入时,游标右移动

    LcdClear();
}


void Delay_ms(uint times)
{
    uint i,j;
    for (i=0; i<times; i++)
        for (j=0; j<1300; j++);
}

void Delay(uint k)
{
    do
    {
    }
    while( --k );
}


void SendByte(uchar Dbyte)
{
    uchar i;
    for(i=0; i<8; i++)
    {
        SCLK = 0;
        Dbyte=Dbyte<<1;      //左移一位
        SID = CY;            //移出的位给SID
        SCLK = 1;
        SCLK = 0;
    }
}

uchar ReceiveByte(void)
{
    uchar i, temp1, temp2, temp3;
    temp1 = 0;
    temp2 = 0;
    for(i=0; i<8; i++)
    {
        temp1=temp1<<1;
        SCLK = 0;
        SCLK = 1;
        SCLK = 0;
        if(SID) temp1++;
    }
    for(i=0; i<8; i++)
    {
        temp2=temp2<<1;
        SCLK = 0;
        SCLK = 1;
        SCLK = 0;
        if(SID) temp2++;
    }
	temp3 = (0xf0&temp1)+(0x0f&temp2);
    return (temp3);
}

void CheckBusy( void )
{
    do   SendByte(0xfc);      //11111,RW(1),RS(0),0
    while(0x80&ReceiveByte());      //BF(.7)=1 Busy
}

void WriteCommand(unsigned char Cbyte )
{
    CS = 1;
    CheckBusy();
    SendByte(0xf8);            //11111,RW(0),RS(0),0
    SendByte(0xf0&Cbyte);      //高四位
    SendByte(0xf0&Cbyte<<4);//低四位
    CS = 0;
}

void WriteData(uchar Dbyte )
{
    CS = 1;
    CheckBusy();
    SendByte(0xfa);            //11111,RW(0),RS(1),0
    SendByte(0xf0&Dbyte);      //高四位
    SendByte(0xf0&Dbyte<<4);//低四位(先执行<<)
    CS = 0;
}


void LineClear(uchar line)
{

    uchar i;
    WriteCommand(0x30);      //8BitMCU
    WriteCommand(AC_TABLE[8*line]);      //起始位置

    for(i=0; i<16; i++)
    {
        WriteData(0x20);//空格
    }
}


void LcdClear( void )
{
    uchar i;
    for(i=0; i<4; i++)
    {
        LineClear(i);
    }
}

void WriteChar(uchar row,uchar col,uchar ch)
{
	WriteCommand(0x30);      //8BitMCU
	WriteCommand(AC_TABLE[8*row+col]);      //起始位置
	WriteData(ch);
}

void WriteStr(uchar row,uchar col,uchar *puts)
{
    WriteCommand(0x30);      //8BitMCU
    WriteCommand(AC_TABLE[8*row+col]);      //起始位置
    while(*puts !='\0')      //判断字符串是否显示完毕
    {
        if(col==8)            //判断换行
        {
            //若不判断,则自动从第一行到第三行
            col=0;
            row++;
        }
        if(row==4) row=0;      //一屏显示完,回到屏左上角
        WriteCommand(AC_TABLE[8*row+col]);
        WriteData(*puts);      //一个汉字要写两次
        puts++;
        if(*puts == '\0')
        {
            return;
        }
        WriteData(*puts);
        puts++;
        col++;
    }
}


//image init and show
void ImageShow(uchar *imagePtr)
{
	uchar i, j, k, columnPosition;
	//init

	for(i = 0; i < 2; i++)
	{
		columnPosition = 0x80 + 0x08 * i;
		for(j = 0; j < 32; j++)
		{
			
/*	for(k = 0; k < 4; k++)   //左半屏显示
			{
				//RE = 1
				WriteCommand(0x34);
				//set start position
				WriteCommand(0x80 + j);
				WriteCommand(columnPosition + k);
				
				//RE = 0
				WriteCommand(0x30);
				//write two bytes continuously
				WriteData(0x00);
				WriteData(0x00);
			}*/
			
			for(k = 0; k < 8; k++)    //全屏显示
			{
				//RE = 1
				WriteCommand(0x34);
				//set start position
				WriteCommand(0x80 + j);
				WriteCommand(columnPosition + k);
				
				//RE = 1
				WriteCommand(0x30);
				//write two bytes continuously
				WriteData(*imagePtr++);
				WriteData(*imagePtr++);
			}
		}
	}
	WriteCommand(0x32);
	WriteCommand(0x36);
}


//-----------------------------------------------------------------------------
// 主程序
//-----------------------------------------------------------------------------

void main()
{ 
Init_Device();
ImageShow(xjtu_image1);
  while(1);

}  