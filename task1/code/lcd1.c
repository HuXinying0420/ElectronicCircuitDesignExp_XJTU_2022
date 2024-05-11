#include"c8051f020.h"
#include <intrins.h>
#include <stdio.h>

#define uchar unsigned char          
#define uint  unsigned int 

#define SYSCLK    11059200


//-----------------------------------------------------------------------------
// Һ���ӿڶ���
//-----------------------------------------------------------------------------
sbit SCLK=P1^3;
sbit SID=P1^4;
sbit CS=P1^5; 

//-----------------------------------------------------------------------------
// ��������
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
0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,      //��һ�к���λ��
0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,      //�ڶ��к���λ��
0x88,0x89,0x8a,0x8b,0x8c,0x8d,0x8e,0x8f,      //�����к���λ��
0x98,0x99,0x9a,0x9b,0x9c,0x9d,0x9e,0x9f,      //�����к���λ��
};

//--------------------------------------------------
uchar code str0[]="�༶���Զ���91";
uchar code str1[]="��������֪��";
uchar code str2[]="ѧ�ţ�2191421852";


//-----------------------------------------------
//ϵͳʱ�ӳ�ʼ��
//---------------------------------------------
void SYSCLK_Init (void)
{
   int i;                      // ��ʱ������
   OSCXCN = 0x67;              // ʹ���ⲿ����
   for (i=0; i<256; i++);      // �ȴ�1ms
   while (!(OSCXCN & 0x80));   // �ȴ��ⲿ�����ȶ�
   OSCICN = 0x88;              // �л����ⲿ����
}

//---------------------------------------------
//�˿ڳ�ʼ��
//---------------------------------------------
void PORT_Init (void)
{
    XBR0     = 0X04;         //����XBR0
    XBR1     = 0X1E;         //����XBR1
    XBR2     = 0X44;         //�򿪽��濪��

	P0MDOUT|= 0x03;
	P1MDOUT=0xFF;			 //�˿�1����Ϊ�������
	P2MDOUT=0xFF;			 //�˿�2����Ϊ�������
	P3MDOUT=0xFF;			 //�˿�3����Ϊ�������

}

//---------------------------------------------
//��ʱms
//---------------------------------------------
void Delay_ms(unsigned int times)
{
    unsigned int i,j;

    for (i=0; i<times; i++) 
		for (j=0; j<1300; j++);                       
 
}

//-----------------------------------------------------------------------------
// ��ʱ
//-----------------------------------------------------------------------------
void Delay(unsigned int k)
{ 
 do 
	{   
    
    }while( --k ); 
}

//-----------------------------------------------
//Һ����ʼ��
//-----------------------------------------------
void LcmInit( void )
{

     WriteCommand(0x30);      //8BitMCU
     WriteCommand(0x03);      //AC?0,���ı�DDRAMֵ
     WriteCommand(0x0C);      //��ʾON,�α�OFF,�α�λ����OFF
     WriteCommand(0x01);      //����,AC��0
     WriteCommand(0x06);      //д��ʱ,�α����ƶ�
}

//-------------------------------------------------
void WriteCommand(unsigned char Cbyte )
{
     CS = 1;
     CheckBusy();
     SendByte(0xf8);            //11111,RW(0),RS(0),0
     SendByte(0xf0&Cbyte);      //����λ
     SendByte(0xf0&Cbyte<<4);//����λ
     CS = 0;
}

//-------------------------------------------------
void SendByte(uchar Dbyte)
{
     uchar i;
     for(i=0;i<8;i++)
     {
           SCLK = 0;
           Dbyte=Dbyte<<1;      //����һλ
           SID = CY;            //�Ƴ���λ��SID
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
     WriteCommand(AC_TABLE[8*row+col]);      //��ʼλ��
     while(*puts !='\0')      //�ж��ַ����Ƿ���ʾ���
     {
           if(col==8)            //�жϻ���
           {            //�����ж�,���Զ��ӵ�һ�е�������
                 col=0;
                 row++;
           }
           if(row==4) row=0;      //һ����ʾ��,�ص������Ͻ�
           WriteCommand(AC_TABLE[8*row+col]);
           WriteData(*puts);      //һ������Ҫд����
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
     	WriteCommand(AC_TABLE[8*row+col]);      //��ʼλ��
        WriteData(ch);
}*/

//-------------------------------------------------------------
void WriteData(uchar Dbyte )
{
     CS = 1;
     CheckBusy();
     SendByte(0xfa);            //11111,RW(0),RS(1),0
     SendByte(0xf0&Dbyte);      //����λ
     SendByte(0xf0&Dbyte<<4);//����λ(��ִ��<<)
     CS = 0;
}

//-----------------------------------------------------------
void LcmClear( void )
{
     uchar i;
     WriteCommand(0x30);      //8BitMCU
     WriteCommand(0x80);      //AC����ʼλ
	 for(i=0;i<4;i++){
	 	Clear(i);
	 }
}

//--------------------------------------------------
void Clear(int l)
{

     uchar i;
     WriteCommand(0x30);      //8BitMCU
		 WriteCommand(AC_TABLE[8*l]);      //��ʼλ��

	   for(i=0;i<16;i++){
     WriteData(0x20);//�ո�
	}	
}

//-----------------------------------------------------------------------------
// ������
//-----------------------------------------------------------------------------

//void main()
//{ 

//  WDTCN = 0xde;
//  WDTCN = 0xad;                //�رտ��Ź� 
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