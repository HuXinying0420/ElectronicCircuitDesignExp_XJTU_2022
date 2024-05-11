#ifndef LCD_H
#define LCD_H

#include "c8051f020.h"
#include <intrins.h>
#include <stdio.h>


extern char yidong;
extern char lie;
extern unsigned int ren_posi;
extern unsigned int zongshu;
extern unsigned int defen;
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

void ImageShow(void);
// void dot_display(uchar x,uchar y);
void ImageShow_ren( void );
#endif
