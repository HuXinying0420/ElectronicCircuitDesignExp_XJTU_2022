#include "stdio.h"
#include "stdlib.h"
#include "c8051f020.h"
#include "intrins.h"
#include "lcd.h"
#include "led.h"
#include "xjtu_image.h"
#include "image2.h"
#include "gif.h"

#define SYSCLK    		11059200
#define SAMPLERATE0    	50000
#define RI1  SCON1&		0x01

#define	AMX0SL_AIN   	8				  
#define INT_DEC      	256   
#define SAMPLE_RATE_DAC 100000L        // DAC sampling rate in Hz
#define PHASE_PRECISION 65536          // range of phase accumulator
#define FREQUENCY       1000           // Frequency of output waveform in Hz

#define TIMER_PRESCALER 12  			// Based on Timer CKCON settings
#define LED_TOGGLE_RATE 100 			// LED toggle rate in milliseconds    1ms
                                       // if LED_TOGGLE_RATE = 1, the LED will
                                       // be on for 1 millisecond and off for
                                       // 1 millisecond

// There are SYSCLK/TIMER_PRESCALER timer ticks per second, so
// SYSCLK/TIMER_PRESCALER/1000 timer ticks per millisecond.
#define TIMER_TICKS_PER_MS  SYSCLK/TIMER_PRESCALER/1000

// Note: LED_TOGGLE_RATE*TIMER_TICKS_PER_MS should not exceed 65535 (0xFFFF)
// for the 16-bit timer

#define AUX1     TIMER_TICKS_PER_MS*LED_TOGGLE_RATE
#define AUX2     -AUX1
#define AUX3     AUX2&0x00FF
#define AUX4     ((AUX2&0xFF00)>>8)

#define TIMER0_RELOAD_HIGH       AUX4  // Reload value for Timer0 high byte
#define TIMER0_RELOAD_LOW        AUX3  // Reload value for Timer0 low byte

#define uchar unsigned char
#define uint unsigned int

sfr16 TMR3     = 0x94;
sfr16 ADC0     = 0xbe;            
extern void          _nop_     (void);
sbit INT1 = P0^2; 			//0的时候按键没用

unsigned int PHASE_ADD = FREQUENCY * PHASE_PRECISION / SAMPLE_RATE_DAC;
long ADC0_result[2];		//AD转换缓冲
int xdata v[10];
int xdata u[10];
unsigned char count=0,tcount=0;		//按键计时
int w1,x1,y1,z1;			//led第一行显示数字
int w2,x2,y2,z2;			//led第二行显示数字
int w3,x3,y3,z3;			//led第三行显示数字
int setting=2200;			//设定控制电压值！！！！！！！！！！！！！！！！！
int result;					//画波形图时的中转值（16进制）
long xdata result2;			//画波形图时的中转值（10进制）

xdata float kp=1;			//PID参数
xdata float ki=0.02;
xdata float kd=0.004;

int error,derror;//误差中间量
int preerror=0;
int lasterror=0;
long sumerror=0;//误差积分
int output;//输出
unsigned int channel = 1;
int SIN_temp,Square_temp,Triangle_temp; 
unsigned char idata key_delay;
unsigned int idata lcd_delay;
volatile int xdata wavevalue[128]={0};
unsigned char idata wavepos=0;
unsigned int idata filtertype=0;
unsigned int idata filtertype2=0;
unsigned int idata paratype=0;
int idata init=10;
volatile unsigned char idata samplepos=0;
volatile long idata slidesum=0;
unsigned int ledcount=0;//延时显示波形用
unsigned char idata ckp[6]="1.000";
unsigned char idata cki[6]="0.020";
unsigned char idata ckd[6]="0.004";

unsigned int Time_num; 		//全局计时用
unsigned int Time_num2; 	//全局计时用
unsigned int Time_num3; 	//全局计时用
unsigned int botflag=4;		//按键标志 0，1，2分别表示左、中、右，按完返回4
unsigned int TRflag=0;		//TR值的标志，用于计时器启停
unsigned int mainflag=0;	//页面标志
unsigned int showflag1=0;	//实验一显示标志
unsigned int showflag2=0;	//实验二显示标志
unsigned int imgflag=0;		//实验一图片标志
unsigned int GFlag=0;		//实验一gif变量

//-----------------------------------------------------------------------------
// 16-bit SFR Definitions for 'F02x
//-----------------------------------------------------------------------------

sfr16 RCAP4    = 0xE4;                 // Timer4 capture/reload
sfr16 TMR4     = 0xF4;                 // Timer4
sfr16 DAC0     = 0xd2;                 // DAC0 data

void ADC0_Init (void)//ADC0初始化
{
	  ADC0CN = 0xC4;       // 关闭ADC0; 低能追踪模式
    REF0CN = 0x03;        // 打开温度传感器, on-chip VREF
    AMX0SL=0x01;     // ADC0使用"0"通道
    ADC0CF=31<<3;         // ADC转换时钟= 2.5MHz
    ADC0CF&=0xf8;          // PGA gain = 1
    EIE2|=0x02;        // 打开ADC0中断
}

unsigned int ADC_Value (unsigned char Flag)//计算ADC数值
{
    unsigned int data temp,temp0,temp1;
    if(Flag)                    //ADC1
        temp0=ADC1;                            
    else                        //ADC0                    
    {
        temp0=ADC0L;
        temp1=ADC0H;
    }
    temp=temp1*256+temp0;        
    return temp;                //取回ADC转换数据
}
void DAC0_Init(void)//DAC0初始化
{
	REF0CN = 0x03;                   // 使用内部VREF发生器
	DAC0CN = 0x98;                   //定时器2、000模式
	DAC0L=0x00;
	DAC0H=0x00;                   // 打开DAC0，右对齐模式
}
void SYSCLK_Init (void)//系统时钟初始化
{
   int i;                      // 延时计数器
   OSCXCN = 0x67;              // 使用外部22.1184MHz晶振
   for (i=0; i<256; i++);      // 等待1ms
   while (!(OSCXCN & 0x80));   // 等待外部振荡器稳定
   OSCICN = 0x88;              // 切换到外部晶振
	
}
void PORT_Init (void)//端口初始化
{
    XBR0     = 0X00;         //设置XBR0
    XBR1     = 0X14;         //设置XBR1
    XBR2     = 0X44;         //打开交叉开关和弱上拉
	  P74OUT  &= 0xcc;
	  P2MDOUT |= 0x7F;
		P0MDOUT|= 0x03;
		P1MDOUT=0xFF;			 //端口1设置为推挽输出
		P2MDOUT=0xFF;			 //端口2设置为推挽输出
		P3MDOUT=0xFF;			 //端口3设置为推挽输出
}
void Port_IO_Init()//IO口初始化
{
	//set P4[1] to push_pull model
    P74OUT |= 0x01;
	
	//enable crossbar
    XBR2 |= 0x40;

    //close the alam P4.1
    P4 &= 0xfd;
	
	//uart1,int1
	XBR1 |= 0x10;
	XBR2 |= 0x44;
}

void Oscillator_Init()//晶振初始化
{
    int i = 0;
    OSCXCN    = 0x67;
    for (i = 0; i < 3000; i++);  // Wait 1ms for initialization
    while ((OSCXCN & 0x80) == 0);
    OSCICN    = 0x08;
}

void Timer0_Init(void)//定时器0初始化
{
	TH0 = TIMER0_RELOAD_HIGH;           // Reinit Timer0 High register
	TL0 = TIMER0_RELOAD_LOW;
	ET0 = 1;                            // Timer0 interrupt enabled
	TMOD = 0x01;                        // 16-bit Mode Timer0
	//TCON |= 0x10;                        // Timer0 ON
}
void Timer1_Init(void)//定时器1初始化
{
		CKCON&=0x68;
		TMOD=0x10;
		TH1=0xFc;
		TL1=0x18;
		ET1=1;
		TR1=1;	 
}
void Timer3_Init (int counts)//定时器3初始化
{
   TMR3CN = 0x02;                      // 停止Timer3; 清除TF3;   
   TMR3RLL=-counts;                   // 初始化装载值
   TMR3=0xffff;                 	// 设置立即装载
   EIE2&=0xFE;                     	// 禁止定时器Timer3中断
   TMR3CN|= 0x04;                     // 开启Timer3
}
void Timer2_Init (void)//定时器2初始化 为DA0转换提供
{
    RCAP2H=0x05;
    RCAP2L=0x00;
    TH2=0x05;
    TL2=0x00;
    T2CON=0x04;
    IE|= 0x20;
}
void TIMER4_Init (int counts)//定时器4初始化
{   
   T4CON = 0x00;                       // Stop Timer4; Clear overflow flag (TF4);
                                       // Set to Auto-Reload Mode

   
   CKCON |= 0x40;                      // Timer4 counts SYSCLKs

   RCAP4 = -counts;                    // Set reload value
   TMR4 = RCAP4;                       // Initialzie Timer4 to reload value

   EIE2 |= 0x04;                       // Enable Timer4 interrupts
   T4CON |= 0x04;                      // Start Timer4

}
void Interrupt_Init()//中断初始化
{
	//INT1
	//low level triggle
	IT1 = 0;
	//enable INT1
	EX1 = 1;
	//enable all interrupt
	EA = 1;
}

void Init_Device(void)//各种初始化函数
{
  //关闭看门狗
  EA = 0;
  WDTCN = 0xde;
  WDTCN = 0xad;
  EA = 1;
	
  Oscillator_Init();
  Port_IO_Init();
	Timer0_Init();
	Interrupt_Init();
	SYSCLK_Init ();//
	PORT_Init();//
	Timer3_Init(SYSCLK/SAMPLERATE0);//
	//TIMER4_Init(SYSCLK/SAMPLE_RATE_DAC);
	ADC0_Init ();//
	Timer2_Init ();//
	DAC0_Init();//
	INT1 = 1;
	TR0=1;
	EX1=1;
	AD0EN=1;
	EA=1;
	
  LcdInit();
	LedInit();
}
void Set_DACs_forsine(void)//生成正弦函数对DAC0值的更改
{
   static unsigned phase_acc = 0;      // Holds phase accumulator
      
              // Temporary 16-bit variables 
   unsigned char index;                // Index into SINE table

   phase_acc += PHASE_ADD;             // Increment phase accumulator
   index = phase_acc >> 8;

   SIN_temp = SINE_TABLE[index];
// Read the table value
   Square_temp = Square_TABLE[index];
   Triangle_temp = Triangle_TABLE[index];
	 index += 64;//！！！这个DA例程里面有，但是综合设计例程里没有，先加上到时候连示波器看看作用
      switch(filtertype)
   {
   		case 0:
   			DAC0 = (SIN_temp ^ 0x8000)*0.92;
			break;
		case 1:      // Read the table value
   			DAC0 = (Triangle_temp ^ 0x8000)*0.92;
			break;
		case 2:
		    // Read the table value
   			DAC0 =(Square_temp ^ 0x8000)*0.92;
			break;
		default:
   			DAC0 =( SIN_temp ^ 0x8000)*0.92;
   };
}


void page2show(void)
{
	if (showflag1==0)//实验一子菜单
	{
		WriteStr(0, 0, "  实验一   ");
	    WriteStr(1, 0, "左：历程");
	    WriteStr(2, 0, "中：动画   ");
		WriteStr(3, 0, "右：返回");
		LedDispNum();
		if(botflag==0) 
		{showflag1=4;LcdClear();botflag=4;}
		if(botflag==1) 
      	{
          showflag1=3;LcdClear();botflag=4;
          WriteStr(0, 0, "");
		  WriteStr(1, 0, "左: 开始");
		  WriteStr(2, 0, "右: 返回");
		  WriteStr(3, 0, "");
      	}
		if(botflag==2) 
		{
			mainflag=0;
			LcdClear();
			botflag=4;
		}
	}
	if (showflag1==1)//计时子菜单
	{
		WriteStr(0, 0, "左：置零");
	    WriteStr(1, 0, "中：开始  暂停");
	    WriteStr(2, 0, "右：返回");
	    WriteStr(3, 0, "学号:2171411574");
	    if (botflag==0) {Time_num = 0;botflag=4;}//计时清零	
	    if (botflag==1)
      {
				  TRflag=TR0;
				  if (TRflag==0)
					  TR0=1;
				  else if(TRflag==1)
					  TR0=0;
          botflag=4;
      }//开始、停止读秒
			if (botflag==2) 
			{showflag1=0;LcdClear();botflag=4;}//退出界面
		    LedDispNum();   //显示三行数据
  	}
	if (showflag1==2)//选图子菜单
	{
		LedDispNum();
		WriteStr(0, 0, "左: 上一幅");
	    WriteStr(1, 0, "中: 下一幅  ");
	    WriteStr(2, 0, "右: 退出  ");
		if (botflag==0) 
      	{
          botflag=4;
		  LcdInit();
		  imgflag--; 
		  if(imgflag>5) imgflag=4;
		  imgflag=imgflag%5;
		  //chooselabel();
      	}//上一张图片
		if (botflag==1) 
		{
          botflag=4;LcdInit();imgflag=(imgflag+1)%5;
          chooselabel();
      	}//下一张图片
		if (botflag==2) 
		{showflag1=0;botflag=4;LcdInit();}//退出界面
	}
	if (showflag1==3)//动画子菜单
	{
	  	LedDispNum();
		  if (botflag==0)
		  {
	    		LcdInit();
				while (GFlag<6)
				{
				ImageShow2(GF[GFlag]);
				GFlag++;
				Delay_ms(160);
				}
				GFlag=0;botflag=4;LcdInit();
				WriteStr(1, 0, "结束，请按右键");
			}
			if (botflag==1) {TR0=1;LcdInit();botflag=4;}
			if (Time_num==500) {TR0=0;botflag=0;Time_num=0;}
			if (botflag==2) {showflag1=0;botflag=4;LcdInit();}//退出界面
	}
	if (showflag1==4)//计时选图中转菜单
	{
			LedDispNum();
			WriteStr(0, 0, "");
			WriteStr(1, 0, "左：计数");
	   		WriteStr(2, 0, "中：图片");
	    	WriteStr(3, 0, "右：返回");
			if (botflag==0) {showflag1=1;LcdClear();botflag=4;}
			if (botflag==1)
			{
					showflag1=2;LcdClear();botflag=4;
					ImageShow(xjtu_image[imgflag]);
					WriteStr(3, 0, "White");
			}
			if (botflag==2) {showflag1=0;botflag=4;LcdInit();}//退出界面
	}
}

void page3show(void)
{
	int i;
	if (showflag2==0)//实验二子菜单
	{
	  WriteStr(0, 0, "实验二");
	  WriteStr(1, 0, "左：示波器");
	  WriteStr(2, 0, "中：飞机控制");
	  WriteStr(3, 0, "右：返回");
	  LedDispNum();
	  if (botflag==2) {mainflag=0;LcdClear();botflag=4;}
		if (botflag==1) {showflag2=2;LcdInit();botflag=4;}
    if (botflag==0) {showflag2=1;LcdInit();botflag=4;WriteStr(1, 0, "按左键开始");}
	}
	if (showflag2==1)//显示波形菜单
	{
		LedDispNum();
		WriteStr(2, 0, "左: 上  中: 下");
		WriteStr(3, 0, "右: 停止&&返回");
		
		v[samplepos++]=(int)(ADC0_result[1]-270)*1.101;
		samplepos%=10;
		if(init>0)
		{
			init--;
		}
		else if(init==0)//为滑动均值计算参数
		{
			for(i=1;i<10;i++)
			  {slidesum+=v[i];}
		  init=-1;
	  }
		ledcount=30; //越大，显示波形速度越慢
		switch(filtertype2)
		{
			case 0:
				Average_filter();//算法平均值滤波
				break;
			case 1:
				Median_average_filter();//中位值平均滤波
				break;
			case 2:
				Sliding_filter();//滑动均值滤波
				break;
		}
		wavevalue[wavepos]=(result-1930)*5.12;//全屏显示
		LcdInit();
		if(wavepos==0) {ImageShow(gImage_white);}
		LcdShowPoint(wavepos++);
		wavepos%=128;
		w2 =  setting/1000;//显示设定值
	  	x2 = (setting - w2*1000)/100;
	  	y2 = (setting - w2*1000-x2*100)/10;
	  	z2 =  setting - w2*1000-x2*100-y2*10;
		w3 =  output/1000;//显示控制量
	  	x3 = (output - w2*1000)/100;
	  	y3 = (output - w2*1000-x2*100)/10;
	  	z3 =  output - w2*1000-x2*100-y2*10;
		while(ledcount)
	  {
			show_f_row();
			show_s_row();
			show_t_row();
			ledcount--;
		}
		//控制算法
		error=setting-result2;
		sumerror+=error;
		derror=lasterror-preerror;
		preerror=lasterror;
		lasterror=error;
		output=result2+kp*error+ki*sumerror+kd*derror;
		
		if (botflag==0) {setting=setting+50;botflag=4;}
		if (botflag==1) {setting=setting-50;botflag=4;}
		if (botflag==2) {showflag2=0;LcdInit();botflag=4;}		
	}
	
	if (showflag2==2)//控制平衡器选择滤波方式
	{
		WriteStr(0, 0, "滤波器");
	  WriteStr(2, 0, "左:next 中: 控制");
		WriteStr(3, 0, "右：返回");
		LedDispNum();
		switch(filtertype2)
		{
			case 0:
				WriteStr(1, 0, "算法平均值滤波  ");
			  break;
			case 1:
				WriteStr(1, 0, "中位值平均滤波  ");
			  break;
			case 2:
				WriteStr(1, 0, "滑动平均值滤波  ");
			  break;
		}
		if (botflag==0) {filtertype2=(filtertype2+1)%3;botflag=4;}
		if (botflag==1) {showflag2=3;LcdInit();ImageShow(gImage_white);botflag=4;}
		if (botflag==2) {showflag2=0;LcdInit();botflag=4;}
	}
	if (showflag2==3)//控制改变PID参数
	{

		WriteStr(0, 0, "Kp:     ");
	  WriteStr(1, 0, "Ki:     ");
		WriteStr(2, 0, "Kd:     ");
		WriteStr(3, 0, "左:Next 中：确定");		
		WriteStr(0, 4, ckp);
	  WriteStr(1, 4, cki);
		WriteStr(2, 4, ckd);	
		LedDispNum();
		if (botflag==0) {paratype=(paratype+1)%3;botflag=4;}
		if (botflag==1) {showflag2=paratype+30;LcdInit();botflag=4;} 
		if (botflag==2) {showflag2=0;LcdInit();botflag=4;}

	}
	if (showflag2==30)//控制改变PID参数
	{


		WriteStr(0, 0, "Kp:     ");
	  WriteStr(1, 0, "左：加         ");
		WriteStr(2, 0, "中：减         ");
		WriteStr(3, 0, "右：返回       ");		
		WriteStr(0, 4, ckp);
		LedDispNum();
		if (botflag==0) {kp=kp+0.1;ckp[0]=ckp[0]+1;WriteStr(0, 4, ckp);botflag=4;}
		if (botflag==1) {kp=kp-0.1;ckp[0]=ckp[0]-1;WriteStr(0, 4, ckp);botflag=4;} 
		if (botflag==2) {showflag2=3;LcdInit();botflag=4;}
	}
	
	if (showflag2==31)//控制改变PID参数
	{


		WriteStr(0, 0, "Ki:     ");
	  WriteStr(1, 0, "左：加         ");
		WriteStr(2, 0, "中：减         ");
		WriteStr(3, 0, "右：返回       ");		
		WriteStr(0, 4, cki);
		LedDispNum();
		if (botflag==0) {ki=ki+0.01;cki[3]=cki[3]+1;WriteStr(0, 4, cki);botflag=4;}
		if (botflag==1) {ki=ki-0.01;cki[3]=cki[3]-1;WriteStr(0, 4, cki);botflag=4;} 
		if (botflag==2) {showflag2=3;LcdInit();botflag=4;}
	}
	if (showflag2==32)//控制改变PID参数
	{


		WriteStr(0, 0, "Kd:     ");
	  WriteStr(1, 0, "左：加     ");
		WriteStr(2, 0, "中：减     ");
		WriteStr(3, 0, "右：返回   ");		
		WriteStr(0, 4, ckd);
		LedDispNum();
		if (botflag==0) {kd=kd+0.001;ckd[4]=ckd[4]+1;WriteStr(0, 4, ckd);botflag=4;}
		if (botflag==1) {kd=kd-0.001;ckd[4]=ckd[4]-1;WriteStr(0, 4, ckd);botflag=4;} 
		if (botflag==2) {showflag2=3;LcdInit();botflag=4;}
	}
}
//-----------------------------------------------------------------------------
// 主程序
//-----------------------------------------------------------------------------

void main()
{	
  Init_Device();//各种初始化
	INT1 = 1;//开启中断
	TR0=0;//计时器停止
    while(1)
    {
			if(mainflag==0)//主界面
			{
				WriteStr(0, 0, "电子线路设计实验");
	      WriteStr(1, 0, "自动化94  胡欣盈");
	      WriteStr(2, 0, "2194323176     ");
		    WriteStr(3, 0, "按任意键开始实验");
				LedDispNum();
			  if (botflag==0) {mainflag=3;LcdClear();botflag=4;}
				if (botflag==1) {mainflag=3;LcdClear();botflag=4;}
				if (botflag==2) {mainflag=3;LcdClear();botflag=4;}
			}
			if(mainflag==1){page3show();}
			if(mainflag==2){page2show();}
			if(mainflag==3){page3show();}//进入实验二界面
    }
}

void INT1_ISR(void) interrupt 2
{
	Delay_ms(1);
	switch(P5)
	{
		case 0xfb:   //左边那个键
			botflag=0;
		  	Delay_ms(50);
			break;
		
		case 0xfd:    //中间那个键
			botflag=1;
		  	Delay_ms(50);
			break;		
		
		case 0xfe:   //右边那个键
			botflag=2;
		  	Delay_ms(50);
			break;
	}
	
}

void Timer0_ISR (void) interrupt 1
{
	TH0 = TIMER0_RELOAD_HIGH;           // Reinit Timer0 High register
	TL0 = TIMER0_RELOAD_LOW;
	
	Time_num++;
	
	if(Time_num >= 6000) Time_num = 0;
	if(key_delay>0)
		key_delay++;
	if(key_delay==20) key_delay=0;
	
	if(lcd_delay>0)
		lcd_delay++;
	if(lcd_delay>100) lcd_delay=0;
}
void Timer1_ISR(void) interrupt 3//定时器1中断
{
    TH1|=0xfc;
		TL1|=0x18;
		tcount++;
		if(tcount>=20)
		{
			tcount=0;
//			Timeout=1;
		}
		count++;		
}
void Time2_ISR(void) interrupt 5 // 定时器2中断服务
{
	TF2=0;
	//Set_DACs_forsine();//生成函数点
	  DAC0=output;
}

void ADC0_ISR (void) interrupt 15 //ADC0中断服务
{
    // ADC mux ?? (0-1)
   AD0INT=0;                  // ??ADC??????
   ADC0_result[channel]=ADC_Value(0);      // ?ADC?
   channel++;                           // ????
   if (channel>=2)
   channel = 0;
   //AMX0SL = channel;              // ??mux?????
}
