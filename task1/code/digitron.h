#ifndef DIGITRON_H
#define DIGITRON_H
#include "c8051f020.h"
#define SYSCLK  11059200
#define SAMPLERATE0    50000

void          _nop_     (void);
void SYSCLK_Init (void);//ϵͳʱ�ӳ�ʼ��
void Delay_us(unsigned int times);//��ʱus

#endif