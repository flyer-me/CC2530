/****************************************************************************
* 文 件 名: main.c
* 描    述: 按下按键KEY1控制LED1.LED2.LED3实现跑马灯效果
****************************************************************************/
#include <ioCC2530.h>

typedef unsigned char uchar;
typedef unsigned int  uint;


#define LED1 P1_0       //定义P1.0口为LED1控制端
#define LED2 P1_1       //定义P1.1口为LED2控制端
#define LED3 P1_4       //定义P1.4口为LED3控制端

#define KEY1 P0_1       // P0.1口控制按键KEY1
//===修改
#define KEY2 P2_0       // P2.0口控制按键KEY2
#define KEY3 P0_7       // P0.7口控制按键KEY3
//===修改
#define ON      1
#define OFF     0


/****************************************************************************
* 名    称: DelayMS()
* 功    能: 以毫秒为单位延时，系统时钟不配置时默认为16M(用示波器测量相当精确)
* 入口参数: msec 延时参数，值越大，延时越久
* 出口参数: 无
****************************************************************************/
void DelayMS(uint msec)
{ 
    uint i,j;
    static int DelayCallCount=0;
    
    for (i=0; i<msec; i++)
        for (j=0; j<535; j++);
    DelayCallCount++;
}

/****************************************************************************
* 名    称: LedOnOrOff()
* 功    能: 点亮或熄灭所有LED灯    
* 入口参数: mode为0时LED灯亮  mode为1时LED灯灭
* 出口参数: 无
****************************************************************************/
void LedOnOrOff(uchar mode)
{
    LED1 = mode;
    LED2 = mode;
    LED3 = mode; 
}

/****************************************************************************
* 名    称: InitLed()
* 功    能: 设置LED相应的IO口
* 入口参数: 无
* 出口参数: 无
****************************************************************************/
void InitLed(void)
{

  P1DIR |= 0x01;               //P1.0定义为输出口  
  P1DIR |= 0x02;               //P1.1定义为输出口 
  P1DIR |= 0x10;               //P1.4定义为输出口 
  asm("NOP");
  
  LedOnOrOff(0);  // 使所有LED灯默认为熄灭状态  
}

/****************************************************************************
* 名    称: InitKey()
* 功    能: 设置按键相应的IO口
* 入口参数: 无
* 出口参数: 无
****************************************************************************/
void InitKey(void)
{
  
    P0SEL &= ~0x02;     //设置P0.1为普通IO口  
    P0DIR &= ~0x02;     //按键接在P0.1口上，设P0.1为输入模式 
    P0INP &= ~0x02;     //打开P0.1上拉电阻
  
    //修改
    P0SEL &= ~0x80;     //设置P0.7为普通IO口  
    P0DIR &= ~0x80;     //按键接在P0.7口上，设P0.7为输入模式 
    P0INP &= ~0x80;     //打开P0.7上拉电阻
    
    P2SEL &= ~0x01;     //设置P2.0为普通IO口  
    P2DIR &= ~0x01;     //按键接在P0.1口上，设P2.0为输入模式 
    P2INP &= ~0x01;     //打开P2.0上拉电阻
    //修改
}

/****************************************************************************
* 名    称: KeyScan()
* 功    能: 读取按键状态
* 入口参数: 无
* 出口参数: 0为抬起   1为按键按下
****************************************************************************/
uchar KeyScan(void)
{
    if (KEY1 == 0)
    {
        DelayMS(10);      //延时10MS去抖
        if (KEY1 == 0)
        {
            while(!KEY1); //松手检测
            return 1;     //有按键按下
        }
    }
 //===修改   
    if (KEY2 == 0)
    {
        DelayMS(10);      //延时10MS去抖
        if (KEY2 == 0)
        {
            while(!KEY2); //松手检测
            return 2;     //有按键按下
        }
    }
    
    if (KEY3 == 0)
    {
        DelayMS(10);      //延时10MS去抖
        if (KEY3 == 0)
        {
            while(!KEY3); //松手检测
            return 3;     //有按键按下
        }
    }
 //===修改  
    return 0;             //无按键按下
}

/****************************************************************************
* 程序入口函数
****************************************************************************/
void main(void)
{
    InitLed();		      //设置LED灯相应的IO口
    InitKey();                //设置按键KEY1相应的IO口
    //===修改
    int flow1=0;
    int flow2=0;
    int flow3=0;
    
    while(1)
    {
       
        DelayMS(2);
        
       	//核心部分： 按键将分别给flow1/2/3赋值，并清零其他值（对循环效果实现非常重要） 
        switch(KeyScan())
        {
        case 1:flow1=!flow1;flow2=flow3=0;break;
        case 2:flow2=!flow2;flow1=flow3=0;break;
        case 3:flow3=!flow3;flow1=flow2=0;break;
        }
 
        //扫描按键当前状态,按下抬起一次改变LED显示效果，在跑马灯效果与LED1闪烁效果间切换
        if (flow1==0&&flow2==0&&flow3==0)    
        { // LED1闪烁效果
            LED1 = ON;    //点亮LED1      
            DelayMS(500); 
            LED1 = OFF;   //熄灭LED1 
            DelayMS(500); 
        }
        else if (flow1==1)
        { //跑马灯效果
            LED1 = ON;    //点亮LED1      
            DelayMS(500); 
            LED1 = OFF;   //熄灭LED1 
            LED2 = ON;                
            DelayMS(500); 
            LED2 = OFF;            
            LED3 = ON;           
            DelayMS(500); 
            LED3 = OFF;
        }
        else if(flow2==1)
        { //逆向跑马灯
            LED3 = ON;           
            DelayMS(500); 
            LED3 = OFF;
            LED2 = ON;                
            DelayMS(500); 
            LED2 = OFF;
            LED1 = ON;    //点亮LED1      
            DelayMS(500); 
            LED1 = OFF;   //熄灭LED1 
        }
        else if(flow3==1)
        { //LED2灯的闪烁
            LED2 = ON;                
            DelayMS(500); 
            LED2 = OFF;
            DelayMS(250);
        }
    //===修改
    }
}
