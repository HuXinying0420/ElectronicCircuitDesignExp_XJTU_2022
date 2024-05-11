#include "digitron.h"

main()
{
  unsigned int i=0;
 //WDTCN = 0xde;
 // WDTCN = 0xad;
  WDTCN|= 0x07;
  WDTCN = 0xa5;
  WDTCN = 0xff;
	

  SYSCLK_Init ();
  PORT_Init();
  Timer3_Init(SYSCLK/SAMPLERATE0);
  ADC0_Init ();
  Timer2_Init ();
  DAC0_Init();
  Uart1_Init (BaudRate_9600);
//  IT1=1;    //=1为跳沿触发
  EX1=1;
  AD0EN=1;
  EA=1;
  b485Send=0;
  P3=0x1f;
  Addr=~(P3&0x1f);
  Addr=Addr&0x1f;	  
  p1=Flash_READ(1)*0.001;//
  p2=Flash_READ(2)*0.001;
  p3=Flash_READ(3)*0.001;
  p4=Flash_READ(4)*0.001;
  p5=Flash_READ5();
  while(1)
  {
    TR0=0;	
	TAMSwitch();
	timeProc();
	checkuart1();	
	//if (P2_3&&(!P2_2))   //位置型算法
	//{
	if(flagkey==1)
	  {flagkey=0;
	    Timer1_Init();
		}
    key();
    	if(AMflag)      //自动控制
        {
            P2_6=1;
			if(flag)//2次采集之间有一段时间用来开关切换
			{
			flag=~flag;
            AD0EN=0;
            fbvoltage=p1*ADC0_result[1];
            AD0EN=1;
			fbvoltage=fbfilter();
            }
			else
			{
			flag=~flag;
			AD0EN=0;
            voltage=p4*ADC0_result[0];
            AD0EN=1;
			voltage=filter(voltage);
          if((voltage*p2+p5)<819.2)    DAC0_result[0]=p2*819.2+p5;
        else if((voltage*p2+p5)<4096) DAC0_result[0]=p2*voltage+p5;
           else  DAC0_result[0]=4095;			
           //DAC0_result[0]=p2*voltage;
			}
			i++;
    	if(i>=50)
			{
				i=0;
				sv1=1.25*(voltage-819.2)/0.4096;//sv=1.25*(voltage-819.2)/0.4096;
                //out1=1.25*(voltage-819.2)/0.4095;//sv=1.25*(voltage-819.2)/0.4096;
		        mv1=1.25*(fbvoltage-819.2)/0.4096;//mv=1.25*(fbvoltage-819.2)/0.4096;
                out1=sv1;
         	}
			dis_mv(mv1);
  			dis_sv(sv1);
           	dis_out(sv1);
           
        }
        else            //手动控制
          {
            P2_6=0;
          if(flag)//2次采集之间有一段时间用来开关切换
			{
			flag=~flag;
            AD0EN=0;
            fbvoltage=p1*ADC0_result[1];
            AD0EN=1;
			fbvoltage=fbfilter();
            }
		  else
			{
			flag=~flag;
			AD0EN=0;
            voltage1=p4*ADC0_result[0];
            AD0EN=1;
			voltage1=filter(voltage1);
	   if((voltage*p2+p5)<819.2)  DAC0_result[0]=p2*819.2+p5;   
        else if((voltage*p2+p5)<4096) DAC0_result[0]=p2*voltage+p5;
           else  DAC0_result[0]=4095;
		//	DAC0_result[0]=p2*voltage;
			}
			i++;
    	if(i>=50)
			{
			 i=0;
			 sv1=1.25*(voltage1-819.2)/0.4096;	//sv=1.25*(voltage-819.2)/0.4096;
             mv1=1.25*(fbvoltage-819.2)/0.4096;//mv=1.25*(fbvoltage-819.2)/0.4096;
         	}
			out1=1.25*(voltage-819.2)/0.4095;//sv=1.25*(voltage-819.2)/0.4096;
            dis_mv(mv1);
  			dis_sv(sv1);
           	dis_out(out1);
           }
        if(P2_2&&(!P2_3))  //增量型算法
           {
            /*if(a<8)  
              { 
                a++;
			    P2_5=0;
                P2_4=0;
			   }       
            else if(a>=8)*/
		     	{
			   // if((sv1-mv1)>sv1*p3)
                 if((out1-mv1)>10000*p3)//绝对百分比
           	    	{
               		P2_5=1;
                	P2_4=0;
                 	}
			     else if((mv1-out1)>10000*p3)//绝对百分比  //if((mv1-sv1)>sv1*p3)
                 	{
                	P2_5=0;
                	P2_4=1;
                	}
			 	 else
			    	{
                	P2_5=0;
                	P2_4=0;
                    }
                  }}
		     WDTCN = 0xa5;
	}
		
	}