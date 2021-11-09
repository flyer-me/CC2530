/****************************************************************************
* 文 件 名: main.c
*     串口调试助手给CC2530发字符串时，开发板会返回接收到的字符串
注意，发送数据以'＃'为结束符,一次最多接收50个字符
****************************************************************************/
#include <ioCC2530.h>
#include <string.h>

typedef unsigned char uchar;
typedef unsigned int  uint;

#define UART0_RX    1
#define UART0_TX    2
#define SIZE       51

char RxBuf;
char UartState;
uchar count;
char RxData[SIZE];        //存储发送字符串
//修改
uchar txNext;
/****************************************************************************
* 名    称: DelayMS()
* 功    能: 以毫秒为单位延时
* 入口参数: msec 延时参数，值越大，延时越久
* 出口参数: 无
****************************************************************************/
void DelayMS(uint msec)
{
    uint i,j;

    for (i=0; i<msec; i++)
        for (j=0; j<1070; j++);
}

/****************************************************************************
* 名    称: InitUart()
* 功    能: 串口初始化函数
* 入口参数: 无
* 出口参数: 无
****************************************************************************/
void InitUart(void)
{
    PERCFG = 0x00;           //外设控制寄存器 USART 0的IO位置:0为P0口位置1
    P0SEL = 0x0c;            //P0_2,P0_3用作串口（外设功能）
    P2DIR &= ~0xC0;          //P0优先作为UART0

    U0CSR |= 0x80;           //设置为UART方式
    U0GCR |= 11;	     // baud_e
    U0BAUD |= 216;           //波特率设为115200
    UTX0IF = 0;              //UART0 TX中断标志初始置位0
    U0CSR |= 0x40;           //允许接收
    //修改 
    //使能串口发送
    IEN2 |= 0x04; //开USART0发送(TX)中断
    IEN0 |= 0x84; //开总中断允许接收(RX)中断
    EA=1;
}

//修改 中断发送函数
#pragma vector = UTX0_VECTOR 
__interrupt void UART0TX_ISR(void) { 
  txNext=1;
  UTX0IF = 0; // 清中断标志
}
//修改 中断接收函数
#pragma vector = URX0_VECTOR 
__interrupt void UART0RX_ISR(void) { 
  RxBuf = U0DBUF; //取出接收到字节
  URX0IF = 0; // 清中断标志
}

/****************************************************************************
* 名    称: UartSendString()
* 功    能: 串口发送函数
* 入口参数: Data:发送缓冲区   len:发送长度
* 出口参数: 无
****************************************************************************/
void UartSendString(char *Data, int len)
{
    uint i;
    for(i=0; i<len; i++)
    {
        U0DBUF = *Data++;
        while(UTX0IF == 0);
        UTX0IF = 0;
    }
}
//修改的发送字符串函数
void UartTX_Send_String(char *Data,int len) {
  int i;
  for(i=0;i<len;i++) {
    txNext=0;
    U0DBUF=*Data++;
    while(txNext==0); 
   }
  U0DBUF='\n';
  while(txNext==0);
}

/****************************************************************************
* 程序入口函数
****************************************************************************/
void main(void)
{
    CLKCONCMD &= ~0x40;                        //设置系统时钟源为32MHZ晶振
    while(CLKCONSTA & 0x40);                   //等待晶振稳定为32M
    CLKCONCMD &= ~0x47;                        //设置系统主时钟频率为32MHZ

    InitUart();                                //调用串口初始化函数
    UartState = UART0_RX;                      //串口0默认处于接收模式
    memset(RxData, 0, SIZE);

    while(1)
    {
        if(UartState == UART0_RX)              //接收状态
        {
          //修改：删除轮询代码
          if(RxBuf != 0)
          {
                if((RxBuf != '#')&&(count < 50))//以'＃'为结束符,一次最多接收50个字符
                    RxData[count++] = RxBuf;
                else
                {
                    if(count >= 50)             //判断数据合法性，防止溢出
                    {
                        count = 0;              //计数清0
                        memset(RxData, 0, SIZE);//清空接收缓冲区
                    }
                    else
                        UartState = UART0_TX;  //进入发送状态
                }
                RxBuf  = 0;
          }
        }

        if(UartState == UART0_TX)              //发送状态
        {
            U0CSR &= ~0x40;                    //禁止接收
            //修改
            UartTX_Send_String(RxData, count);   //发送已记录的字符串。
            U0CSR |= 0x40;                     //允许接收
            UartState = UART0_RX;              //恢复到接收状态
            count = 0;                         //计数清0
            memset(RxData, 0, SIZE);           //清空接收缓冲区
        }
    }
}
