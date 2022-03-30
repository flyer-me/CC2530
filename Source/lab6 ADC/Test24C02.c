·/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
* 文 件 名：Test24C02.c
× 功能：向I2C器件AT24C02中某地址(addr)写入一个数据(num0)，读出该数据(num1)，串口输出
*        验证读出的数据就是写入的数据
*
*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include"i2c.h"
// GPIO模拟I2C，引脚为P0.5和P0.6，定义见i2c.h
//#define SCL          P0_6     //I2C时钟
//#define SDA          P0_5     //I2C数据线

//按键 IO
#define K1 P0_1
#define K2 P2_0
#define K3 P0_7

#define ACK 1
#define noACK 0


void initIO(void);

char writedata[]="Write: 000";
char readdata[] ="Read: 000";
char numdata[]  ="Num: 000";

/*******************************************************************************
* 函数名         : void At24c02Write(unsigned char addr,unsigned char dat)
* 函数功能		   : 往24c02的一个地址写入一个数据
* 输入           : 无
* 输出         	 : 无
*******************************************************************************/
void At24c02Write(unsigned char addr,unsigned char dat)
{
	I2cStart();
	I2cSendByte(0xa2);//发送写器件地址
	I2cSendByte(addr);//发送要写入内存地址
	I2cSendByte(dat);	//发送数据
	I2cStop();
}
/*
*E2Write()多个单字节的连续写入
* E2写入函数，buf-源数据指针，addr-E2中的起始地址，len-写入长度*/
void E2Write(unsigned char *buf, unsigned char addr, unsigned char len)
{
     while (len--)
     {
     do {                           //用寻址操作查询当前是否可进行读写操作
        I2cStart();
        if (I2cSendByte(0x51<<1)) break; //应答则跳出循环，非应答则进行下一次查询
        I2cStop();
     } while(1);

    I2cSendByte(addr++);  //写入起始地址
    I2cSendByte(*buf++);  //写入一个字节数据
    I2cStop();          //结束写操作，以等待写入完成
     }
}

/*
*E2WriteP() EEPROM页写入
* E2写入函数，buf-源数据指针，addr-E2中的起始地址，len-写入长度*/
void E2WriteP(unsigned char *buf, unsigned char addr, unsigned char len)
{
    while (len > 0)
    {
    do {    //等待上次写入操作完成，用寻址操作查询当前是否可进行读写操作
        I2cStart();
        if (I2cSendByte(0x51<<1)) break;   //应答则跳出循环，非应答则进行下一次查询
        I2cStop();
     } while(1);

    //按页写模式连续写入字节
    I2cSendByte(addr);            //写入起始地址
    while (len > 0)
    {
       I2cSendByte(*buf++);      //写入一个字节数据
       len--;                   //待写入长度计数递减
       addr++;                  //E2地址递增
       if ((addr&0x07) == 0) break; //是否到达页边界，24C02每页8字节，只检查低3位是否为零  到达页边界时结束写操作
    }
    I2cStop();
    }
}


/* E2读取函数，buf-数据接收指针，addr-E2中的起始地址，len-读取长度*/
void E2Read(unsigned char *buf, unsigned char addr, unsigned char len) {
  do {                           //用寻址操作查询当前是否可进行读写操作
     I2cStart();
     if (I2cSendByte(0x51<<1)) {   //应答则跳出循环，非应答则进行下一次查询
         break;   }
      I2cStop();
    } while(1);
   I2cSendByte(addr);              //写入起始地址
   I2cStart();                   //发送重复启动信号
   I2cSendByte((0x51<<1)|0x01);  //寻址器件，后续为读操作
   while (len > 1) {   //连续读取len-1个字节
      *buf++ = I2cReadByte(ACK); //最后字节之前为读取操作+应答
       len--;  }
      *buf = I2cReadByte(noACK);       //最后一个字节为读取操作+非应答
   I2cStop();  }

/* 将一段内存数据转换为十六进制格式的字符串，
str-字符串指针，src-源数据地址，len-数据长度*/
void MemToStr(unsigned char *str, unsigned char *src, unsigned char len) {
  unsigned char tmp;
  while (len--)
  {
    tmp = *src >> 4; //先取高4位
    if (tmp <= 9)
      *str++ = tmp + '0'; //转换为0-9或A-F
    else *str++ = tmp -10 + 'A';

    tmp = *src & 0x0F; //再取低4位
    if (tmp <= 9)
      *str++ = tmp + '0'; //转换为0-9或A-F
    else
      *str++ = tmp -10 + 'A';
    *str++ = ' '; //转换完一个字节添加一个空格
    src++;
   }
}
/*******************************************************************************
* 函数名         : unsigned char At24c02Read(unsigned char addr)
* 函数功能		   : 读取24c02的一个地址的一个数据
* 输入           : 无
* 输出         	 : 返回读到的字节
*******************************************************************************/
unsigned char At24c02Read(unsigned char addr)
{
	unsigned char num;
	I2cStart();
	I2cSendByte(0xa2); //发送写器件地址
	I2cSendByte(addr); //发送要读取的地址
	I2cStart();
	I2cSendByte(0xa3); //发送读器件地址
	num = I2cReadByte(noACK); //读取数据，不继续
	I2cStop();
	return num;
}

/****************************************************************************
* 名    称: DelayMS()
* 功    能: 以毫秒为单位延时，系统时钟不配置时默认为16M(用示波器测量相当精确)
* 入口参数: msec 延时参数，值越大，延时越久
* 出口参数: 无
****************************************************************************/
void DelayMS(unsigned int msec)
{
    unsigned int i,j;

    for (i=0; i<msec; i++)
        for (j=0; j<535; j++);
}

unsigned char KeyScan(void)
{
    if (K1 == 0)
    {
        DelayMS(10);      //延时10MS去抖
        if (K1 == 0)
        {
            while(!K1); //松手检测
            return 1;     //有按键按下
        }
    }

    if (K2 == 0)
    {
        DelayMS(10);      //延时10MS去抖
        if (K2 == 0)
        {
            while(!K2); //松手检测
            return 2;     //有按键按下
        }
    }

    if (K3 == 0)
    {
        DelayMS(10);      //延时10MS去抖
        if (K3 == 0)
        {
            while(!K3); //松手检测
            return 3;     //有按键按下
        }
    }

    return 0;             //无按键按下
}

/**************************************************************************************************
 * 函数名称：initIO
 * 功能描述：IO初始化：按键P0.1,P2.0,P0.7;
 * 参    数：无
 * 返 回 值：无
 **************************************************************************************************/
void InitIO(void)
{

    P0SEL &= ~0x82;     //设置P0.1,P0.7为普通IO口
    P0DIR &= ~0x82;     //按键接在P0.1,P0.7口上，设置P0.1,P0.7为输入方向
    P0INP &= ~0x82;     //打开P0.1,P0.7上拉电阻

    P2SEL &= ~0x01;     //设置P2.0为普通IO口
    P2DIR &= ~0x01;     //按键接在P2.0口上，设置P2.0为输入方向
    P2INP &= ~0x01;     //打开P2.0上拉电阻

}

/*****************************************
 串口初始化函数：初始化串口 UART0
*****************************************/
void InitUART0(void)
{
  	P0SEL = 0x0c;				 //P0.2 P0.3用作串口
	PERCFG = 0x00;				 //选择USART0位置1
	P2DIR &= ~0XC0;                          //P0优先作为UART0
        U0CSR |= 0x80;   	                 //串口设置为UART方式
        U0GCR |= 11;
        U0BAUD |= 216;		                  //波特率设为115200
	UTX0IF = 0;                               //UART0 TX中断标志初始置位1
        U0CSR |= 0X40;				  //允许接收
        //IEN0 |= 0x84;				  //开总中断，接收中断
}

/****************************************************************
   串口发送字符串函数
****************************************************************/
void UartTX_Send_String(char *Data,int len)
{
  int j;
  for(j=0;j<len;j++)
  {
    U0DBUF = *Data++;
    while(UTX0IF == 0);
    UTX0IF = 0;
  }
  U0DBUF = 0x0A;        // 换行
  while(UTX0IF == 0);
  UTX0IF = 0;
}

/**************************
系统时钟 不分频
计数时钟 32分频
**************************/
void InitClock(void)
{
    CLKCONCMD &= ~0x40; // 设置系统时钟源为 32MHZ晶振
    while(CLKCONSTA & 0x40);    // 等待晶振稳定
    CLKCONCMD &= ~0x47;          // 设置系统主时钟频率为 32MHZ
}


/**************************************************************************************************
 * 函数名称：main
 * 功能描述：检测按键KEY1,KEY2,KEY3，根据按键执行相应的动作：
 *        KEY1：写数据
 *        KEY2：读数据
 *        KEY3：数据+1
 * 参    数：无
 * 返 回 值：无
 **************************************************************************************************/
void main()
{

  unsigned char keynum;
  unsigned char num0=15,num1=0,addr=9;
  //修改========
  unsigned char testdata[]  ={4,5,6,7,8,9,10,11,12,13};
  unsigned char getdata[]  ={0,0,0,0,0,0,0,0,0,0};
  unsigned char data[3*sizeof(testdata)];
  //修改========

    InitClock();  // 设置系统时钟源为32MHz晶体振荡器
    InitIO();
    InitUART0();  //初始化串口

    DelayMS(10);
    UartTX_Send_String("Start:" ,6);
    //串口打印当前待写入的数据，用三位十进制表示
    numdata[5]= (num0)/100+48;  //百位
    numdata[6]= (num0%100/10)+48; //十位
    numdata[7]= (num0%100%10) +48; //个位
    UartTX_Send_String(numdata ,8);
    DelayMS(10);

    while(1)
  {
         DelayMS(2);
         keynum = KeyScan();

         if(keynum==1) // Key1 press detected
         {
                // UartTX_Send_String("write loop:" ,11);
                //DelayMS(200);
		At24c02Write(addr,num0);
                writedata[7]= (char)(num0)/100+48;
                writedata[8]= (char)(num0%100/10)+48;
                writedata[9]= (char)(num0%100%10) +48;
                //DelayMS(50);
                UartTX_Send_String(writedata ,10);
	}

	if(keynum==2) // Key2 press detected
	{
                // UartTX_Send_String("read loop:" ,11);
                //DelayMS(200);
		num1=At24c02Read(addr);
                readdata[6]= (char)(num1)/100+48;
                readdata[7]= (char)(num1%100/10)+48;
                readdata[8]= (char)(num1%10) +48;
                //DelayMS(50);
                UartTX_Send_String(readdata ,9);
	}

	if(keynum==3) // Key3 press detected
	{
               //UartTX_Send_String("increase loop:" ,15);
               //DelayMS(10);
        //修改========
        E2WriteP(testdata ,11 ,10);
        E2Read(getdata ,11 ,10);
        MemToStr(data,getdata,10);
        UartTX_Send_String((char*)data ,sizeof(data));
        //修改========
	}
  }

}
