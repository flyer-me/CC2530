#include "hal.h"
#include "hal_types.h"
#include <string.h>
#define HAL_DMA_U0DBUF    0x70C1

char revbuf[20]; 
INT8 count = 0;

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
    
    IEN0 |= 0x84; //开总中断允许接收(RX)中断
    EA=1;
}

//中断接收函数
#pragma vector = URX0_VECTOR 
__interrupt void UART0RX_ISR(void) 
{ 
  revbuf[count++] = U0DBUF;//数据入缓冲区
  URX0IF = 0;              // 清中断标志
}

//初始化DMA传输应用程序示例
void initDma(void)
{
   SET_MAIN_CLOCK_SOURCE(CRYSTAL);
   
   InitUart();  //调用串口初始化函数   
}

//main
void main(void){
   DMA_DESC dmaChannel;

   initDma();
   //memset(revbuf, 0, sizeof(revbuf) );
     // Setting up the DMA channel.
   SET_WORD(dmaChannel.SRCADDRH, dmaChannel.SRCADDRL,   &revbuf); // 被传送data首地址
   SET_WORD(dmaChannel.DESTADDRH, dmaChannel.DESTADDRL, HAL_DMA_U0DBUF);   //以HAL_DMA_U0DBUF为目的地
   SET_WORD(dmaChannel.LENH, dmaChannel.LENL, sizeof(revbuf));    // Setting the number of bytes to transfer.
   dmaChannel.VLEN      = VLEN_USE_LEN;  // Using the length field to determine how many bytes to transfer.
   dmaChannel.PRIORITY  = PRI_HIGH;      // High priority.
   dmaChannel.M8        = M8_USE_8_BITS; // Irrelevant since length is determined by the LENH and LENL.
   dmaChannel.IRQMASK   = FALSE;         // The DMA shall not issue an IRQ upon completion.
   dmaChannel.DESTINC   = DESTINC_0;     // 每次传送后，不改变目的地址
   dmaChannel.SRCINC    = SRCINC_1;      // 每次传送后，源地址+1
   dmaChannel.TRIG      = DMATRIG_URX0;  // UART接收完成后启动
   dmaChannel.TMODE     = TMODE_SINGLE;   // 每次一个字节
   dmaChannel.WORDSIZE  = WORDSIZE_BYTE; // One byte is transferred each time.

   DMA_SET_ADDR_DESC0(&dmaChannel);//DMA寄存器配置 (DMA0CFGH、DMA0CFGL)
   DMA_ABORT_CHANNEL(0);// 停止通道0
   DMA_ARM_CHANNEL(0);  //DMA通道0进入工作状态    
   halWait(255); 

   while(1)
   {
       if(count>=20)    //当DMA源地址到达缓冲区末尾，重置DMA
       {
         count=0;
         //memset(revbuf, 0, sizeof(revbuf) ); //清空缓冲区 非必要
         DMA_SET_ADDR_DESC0(&dmaChannel);   //DMA寄存器配置 (DMA0CFGH、DMA0CFGL)
         DMA_ABORT_CHANNEL(0);              // 停止通道0
         DMA_ARM_CHANNEL(0);                //DMA通道0进入工作状态 DMAARM0 = 1
         halWait(255); 
       }

    while(!(DMAIRQ & DMA_CHANNEL_0));   //等待DMA结束

   }
}