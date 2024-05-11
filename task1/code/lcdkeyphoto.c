
#include "lcd.h"
#include "stdlib.h"
#include <intrins.h>
#define uchar unsigned char          
#define uint  unsigned int 

//constant definition
const unsigned char code flower[1024] =   { /* 0X00,0X01,0X10,0X00,0X10,0X00, */
0X00,0X00,0X00,0X00,0X00,0X80,0X02,0X40,0X02,0X00,0X18,0X18,0X20,0X04,0X00,0X04,
0X10,0X00,0X00,0X10,0X00,0X00,0X08,0X08,0X00,0X00,0X0A,0X70,0X00,0X00,0X00,0X00,
};
const unsigned char code ren[1024]=  { /* 0X00,0X01,0X10,0X00,0X10,0X00, */
0X00,0X00,0X06,0X00,0X06,0X00,0X32,0X00,0X18,0X60,0X07,0XC0,0X03,0X00,0X03,0X80,
0X01,0X80,0X00,0XC0,0X00,0XF0,0X00,0XA0,0X00,0X80,0X01,0X80,0X01,0X80,0X00,0X00,
};

uchar *imagePtr=flower;
uchar *imagePtr1=ren;
sbit SCLK=P1^3;
sbit SID=P1^4;
sbit CS=P1^5;
uchar code AC_TABLE[]={
0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,      //第一行汉字位置
0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,      //第二行汉字位置
0x88,0x89,0x8a,0x8b,0x8c,0x8d,0x8e,0x8f,      //第三行汉字位置
0x98,0x99,0x9a,0x9b,0x9c,0x9d,0x9e,0x9f,      //第四行汉字位置
};


extern char yidong;
yidong=0;
extern char lie;
lie=0;




void Lcd_Port_Init(void)
{
	//set P1[5:3] to push_pull model
    P1MDOUT |= 0x38;
	
	//enable crossbar
    XBR2 |= 0x40;
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
void ImageShow( void )
{
	uchar i, j, k, columnPosition;
	//init
	
	i = 0;	
	columnPosition = 0x80 + 0x08 * i;
	for(j = 0; j < 32; j++)
	{
	for(k = 0; k < 8; k++)    //上半屏幕清空
		{
			//RE = 1
			WriteCommand(0x34);
			//set start position
			WriteCommand(0x80 + j);
			WriteCommand(columnPosition + k);
			
			//RE = 1
			WriteCommand(0x30);
			//write two bytes continuously
			WriteData(0x00);
			WriteData(0x00);
		}
	}
	i = 1;
	columnPosition = 0x80 + 0x08 * i;
	for(j = 0; j < 16; j++)
	{
	for(k = 0; k < 8; k++)    //留下人
		{
			//RE = 1
			WriteCommand(0x34);
			//set start position
			WriteCommand(0x80 + j);
			WriteCommand(columnPosition + k);
			
			//RE = 1
			WriteCommand(0x30);
			//write two bytes continuously
			WriteData(0x00);
			WriteData(0x00);
		}
	}
	
	
	for(i = 0; i <1; i++)
	{
		uchar *imagePtr=flower;
		columnPosition = 0x80 + 0x08 * i;
		for(j = yidong*8; j < yidong*8+16; j++)
		{
		for(k = lie ;k < lie+1; k++)    //显示花
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
	//yidong=rand()%7;
	yidong++;
	if(yidong>=4) 
	{
		lie=(lie+3)%8;
		zongshu++;
		yidong=0;
	}


		
	
	WriteCommand(0x32);
	WriteCommand(0x36);
}

void ImageShow_ren( void )
{
	//ImageShow();
	uchar j1, k1, columnPosition1;
	//init
		uchar *imagePtr1=ren;
	columnPosition1 = 0x80 + 0x08 * 1;
	for(j1 = 16; j1 < 32; j1++)
	{
	for(k1 = 0; k1 < 8; k1++)    //清除上一时刻人
		{
			//RE = 1
			WriteCommand(0x34);
			//set start position
			WriteCommand(0x80 + j1);
			WriteCommand(columnPosition1 + k1);
			
			//RE = 1
			WriteCommand(0x30);
			//write two bytes continuously
			WriteData(0x00);
			WriteData(0x00);
		}
	}
		for(j1 = 16; j1 < 32; j1++)
		{
		for(k1 =ren_posi;k1 <ren_posi+1; k1++)    //显示人
			{
				//RE = 1
				WriteCommand(0x34);
				//set start position
				WriteCommand(0x80 + j1);
				WriteCommand(columnPosition1 + k1);
				
				//RE = 1
				WriteCommand(0x30);
				//write two bytes continuously
				WriteData(*imagePtr1++);
				WriteData(*imagePtr1++);
			}
		}
	
					if((ren_posi==lie)&&(yidong))
			{
				defen++;
			}
	WriteCommand(0x32);
	WriteCommand(0x36);
}