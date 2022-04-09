#include "ioCC2530.h"

  //#define LED1 P1_0     // LED1定义为1_0
#define LED2 P1_1     // LED2定义为1_1
//#define LED3 P1_4     // LED3定义为1_4


void Timer1_Init(void)
{
    T1CC0L = 0xC8;    //设置T1CC0,PWM输出时比较值上限 = 200
    T1CC0H = 0x00;
    //修改========
    T1CCTL2 = 0x74;   //设置T1的通道2，0x74产生中断 模式 - Set when=T1CC0,clear when=T1CC2
    T1CC2L = 0x0A;    //设置T1输出通道2比较输出值
    T1CC2H = 0x00;

    T1CCTL0=0x64;     //设置CC0的比较输出中断
    T1CTL = 0x0E;     //128分频 模模式

    T1IE = 1;
    EA = 1;           //开总中断
}


void LED_Init(void)
{
  CLKCONCMD &= 0x80;
  PERCFG |= 0x40;   //定时器1 的IO位置 2: T1 location=2
  P2SEL &= ~0x10; //优先定时器1比较输出功能
  //P2DIR |= 0xC0; //第1优先级：定时器1通道2-3
  P1SEL |= 0x01;    //设置P1.0为外设功能, P1.1依然是普通GPIO
  P1DIR |= 0x03;    //设置LED1的引脚P1.0, LED2的引脚P1.1为输出
  P1 &= ~0x13; //全灭
}

#pragma vector = T1_VECTOR
__interrupt void T1_ISR(void)
{
    if(T1STAT&0x04)
    {
      T1STAT &= ~(1 << 2);
      LED2 = 0;
    }
    else if(T1STAT&0x01)
    {
      T1STAT &= ~(1 << 0);
      LED2 = 1;
    }
}
    //修改========

void Delay(unsigned int xms)
{
 while(xms--);
}
/**************************************************************
函数名称：main
功能：程序主函数
入口参数：无
出口参数：无
返回值：无
**************************************************************/
void main(void)
{
  unsigned char rate[16]={0xAF,0x96,0x7D,0x64,0x4B,0x32,0x19,0x0A,0x0A,0x19,0x32,0x4B,0x64,0x7D,0x96,0xAF};
  LED_Init();
  unsigned char i;
  Timer1_Init();
  LED2 = 1;
  while(1){
        for(i=0;i<16;i++)
      {
        T1CC2L = rate[i];
        T1CC2H = 0x00;

        Delay(50000);
        Delay(50000);
        Delay(50000);
        Delay(50000);
        Delay(50000);
      }
  }
}
