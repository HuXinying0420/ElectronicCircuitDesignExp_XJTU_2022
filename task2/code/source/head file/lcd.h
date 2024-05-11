#ifndef LCD_H
#define LCD_H

#include "c8051f020.h"
#include <intrins.h>
#include <stdio.h>

#define uchar unsigned char          
#define uint  unsigned int 

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
void ImageShow2(uchar *imagePtr);
void ImageShow3(uchar *imagePtr);
void LcdShowPoint(unsigned char x);

extern volatile int xdata wavevalue[128];
extern const unsigned char code gImage_white[1024];
#endif
