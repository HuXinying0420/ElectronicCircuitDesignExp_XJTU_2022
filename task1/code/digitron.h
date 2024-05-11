#ifndef DIGITRON_H
#define DIGITRON_H
#include "c8051f020.h"
#define SYSCLK  11059200
#define SAMPLERATE0    50000

void          _nop_     (void);
void SYSCLK_Init (void);//系统时钟初始化
void Delay_us(unsigned int times);//延时us

#endif