/****************************************************************************
* �� �� ��: main.c
*     ���ڵ������ָ�CC2530���ַ���ʱ��������᷵�ؽ��յ����ַ���
ע�⣬����������'��'Ϊ������,һ��������50���ַ�
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
char RxData[SIZE];        //�洢�����ַ���
//�޸�
uchar txNext;
/****************************************************************************
* ��    ��: DelayMS()
* ��    ��: �Ժ���Ϊ��λ��ʱ
* ��ڲ���: msec ��ʱ������ֵԽ����ʱԽ��
* ���ڲ���: ��
****************************************************************************/
void DelayMS(uint msec)
{
    uint i,j;

    for (i=0; i<msec; i++)
        for (j=0; j<1070; j++);
}

/****************************************************************************
* ��    ��: InitUart()
* ��    ��: ���ڳ�ʼ������
* ��ڲ���: ��
* ���ڲ���: ��
****************************************************************************/
void InitUart(void)
{
    PERCFG = 0x00;           //������ƼĴ��� USART 0��IOλ��:0ΪP0��λ��1
    P0SEL = 0x0c;            //P0_2,P0_3�������ڣ����蹦�ܣ�
    P2DIR &= ~0xC0;          //P0������ΪUART0

    U0CSR |= 0x80;           //����ΪUART��ʽ
    U0GCR |= 11;	     // baud_e
    U0BAUD |= 216;           //��������Ϊ115200
    UTX0IF = 0;              //UART0 TX�жϱ�־��ʼ��λ0
    U0CSR |= 0x40;           //�������
    //�޸� 
    //ʹ�ܴ��ڷ���
    IEN2 |= 0x04; //��USART0����(TX)�ж�
    IEN0 |= 0x84; //�����ж��������(RX)�ж�
    EA=1;
}

//�޸� �жϷ��ͺ���
#pragma vector = UTX0_VECTOR 
__interrupt void UART0TX_ISR(void) { 
  txNext=1;
  UTX0IF = 0; // ���жϱ�־
}
//�޸� �жϽ��պ���
#pragma vector = URX0_VECTOR 
__interrupt void UART0RX_ISR(void) { 
  RxBuf = U0DBUF; //ȡ�����յ��ֽ�
  URX0IF = 0; // ���жϱ�־
}

/****************************************************************************
* ��    ��: UartSendString()
* ��    ��: ���ڷ��ͺ���
* ��ڲ���: Data:���ͻ�����   len:���ͳ���
* ���ڲ���: ��
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
//�޸ĵķ����ַ�������
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
* ������ں���
****************************************************************************/
void main(void)
{
    CLKCONCMD &= ~0x40;                        //����ϵͳʱ��ԴΪ32MHZ����
    while(CLKCONSTA & 0x40);                   //�ȴ������ȶ�Ϊ32M
    CLKCONCMD &= ~0x47;                        //����ϵͳ��ʱ��Ƶ��Ϊ32MHZ

    InitUart();                                //���ô��ڳ�ʼ������
    UartState = UART0_RX;                      //����0Ĭ�ϴ��ڽ���ģʽ
    memset(RxData, 0, SIZE);

    while(1)
    {
        if(UartState == UART0_RX)              //����״̬
        {
          //�޸ģ�ɾ����ѯ����
          if(RxBuf != 0)
          {
                if((RxBuf != '#')&&(count < 50))//��'��'Ϊ������,һ��������50���ַ�
                    RxData[count++] = RxBuf;
                else
                {
                    if(count >= 50)             //�ж����ݺϷ��ԣ���ֹ���
                    {
                        count = 0;              //������0
                        memset(RxData, 0, SIZE);//��ս��ջ�����
                    }
                    else
                        UartState = UART0_TX;  //���뷢��״̬
                }
                RxBuf  = 0;
          }
        }

        if(UartState == UART0_TX)              //����״̬
        {
            U0CSR &= ~0x40;                    //��ֹ����
            //�޸�
            UartTX_Send_String(RxData, count);   //�����Ѽ�¼���ַ�����
            U0CSR |= 0x40;                     //�������
            UartState = UART0_RX;              //�ָ�������״̬
            count = 0;                         //������0
            memset(RxData, 0, SIZE);           //��ս��ջ�����
        }
    }
}