/****************************************************************************
* �� �� ��: main.c
* ��    ��: ���°���KEY1����LED1.LED2.LED3ʵ��������Ч��
****************************************************************************/
#include <ioCC2530.h>

typedef unsigned char uchar;
typedef unsigned int  uint;


#define LED1 P1_0       //����P1.0��ΪLED1���ƶ�
#define LED2 P1_1       //����P1.1��ΪLED2���ƶ�
#define LED3 P1_4       //����P1.4��ΪLED3���ƶ�

#define KEY1 P0_1       // P0.1�ڿ��ư���KEY1
//===�޸�
#define KEY2 P2_0       // P2.0�ڿ��ư���KEY2
#define KEY3 P0_7       // P0.7�ڿ��ư���KEY3
//===�޸�
#define ON      1
#define OFF     0


/****************************************************************************
* ��    ��: DelayMS()
* ��    ��: �Ժ���Ϊ��λ��ʱ��ϵͳʱ�Ӳ�����ʱĬ��Ϊ16M(��ʾ���������൱��ȷ)
* ��ڲ���: msec ��ʱ������ֵԽ����ʱԽ��
* ���ڲ���: ��
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
* ��    ��: LedOnOrOff()
* ��    ��: ������Ϩ������LED��
* ��ڲ���: modeΪ0ʱLED����  modeΪ1ʱLED����
* ���ڲ���: ��
****************************************************************************/
void LedOnOrOff(uchar mode)
{
    LED1 = mode;
    LED2 = mode;
    LED3 = mode;
}

/****************************************************************************
* ��    ��: InitLed()
* ��    ��: ����LED��Ӧ��IO��
* ��ڲ���: ��
* ���ڲ���: ��
****************************************************************************/
void InitLed(void)
{

  P1DIR |= 0x01;               //P1.0����Ϊ�����
  P1DIR |= 0x02;               //P1.1����Ϊ�����
  P1DIR |= 0x10;               //P1.4����Ϊ�����
  asm("NOP");

  LedOnOrOff(0);  // ʹ����LED��Ĭ��ΪϨ��״̬
}

/****************************************************************************
* ��    ��: InitKey()
* ��    ��: ���ð�����Ӧ��IO��
* ��ڲ���: ��
* ���ڲ���: ��
****************************************************************************/
void InitKey(void)
{

    P0SEL &= ~0x02;     //����P0.1Ϊ��ͨIO��
    P0DIR &= ~0x02;     //��������P0.1���ϣ���P0.1Ϊ����ģʽ
    P0INP &= ~0x02;     //��P0.1��������

    //�޸�  ��ע���˴�������Ҳ���������У��Ʋ��������Զ�����
    P0SEL &= ~0x80;     //����P0.7Ϊ��ͨIO��
    P0DIR &= ~0x80;     //��������P0.7���ϣ���P0.7Ϊ����ģʽ
    P0INP &= ~0x80;     //��P0.7��������

    P2SEL &= ~0x01;     //����P2.0Ϊ��ͨIO��
    P2DIR &= ~0x01;     //��������P0.1���ϣ���P2.0Ϊ����ģʽ
    P2INP &= ~0x01;     //��P2.0��������
    //�޸�
}

/****************************************************************************
* ��    ��: KeyScan()
* ��    ��: ��ȡ����״̬
* ��ڲ���: ��
* ���ڲ���: 0Ϊ̧��   1Ϊ��������
****************************************************************************/
uchar KeyScan(void)
{
    if (KEY1 == 0)
    {
        DelayMS(10);      //��ʱ10MSȥ��
        if (KEY1 == 0)
        {
            while(!KEY1); //���ּ��
            return 1;     //�а�������
        }
    }
 //===�޸�
    if (KEY2 == 0)
    {
        DelayMS(10);      //��ʱ10MSȥ��
        if (KEY2 == 0)
        {
            while(!KEY2); //���ּ��
            return 2;     //�а�������
        }
    }

    if (KEY3 == 0)
    {
        DelayMS(10);      //��ʱ10MSȥ��
        if (KEY3 == 0)
        {
            while(!KEY3); //���ּ��
            return 3;     //�а�������
        }
    }
 //===�޸�
    return 0;             //�ް�������
}

/****************************************************************************
* ������ں���
****************************************************************************/
void main(void)
{
    InitLed();		      //����LED����Ӧ��IO��
    InitKey();                //���ð���KEY1��Ӧ��IO��
    //===�޸�
    int flow1=0;
    int flow2=0;
    int flow3=0;

    while(1)
    {

        DelayMS(2);

       	//���Ĳ��֣� �������ֱ��flow1/2/3��ֵ������������ֵ����ѭ��Ч��ʵ�ַǳ���Ҫ��
        switch(KeyScan())
        {
        case 1:flow1=!flow1;flow2=flow3=0;break;
        case 2:flow2=!flow2;flow1=flow3=0;break;
        case 3:flow3=!flow3;flow1=flow2=0;break;
        }

        //ɨ�谴����ǰ״̬,����̧��һ�θı�LED��ʾЧ������������Ч����LED1��˸Ч�����л�
        if (flow1==0&&flow2==0&&flow3==0)
        { // LED1��˸Ч��
            LED1 = ON;    //����LED1
            DelayMS(500);
            LED1 = OFF;   //Ϩ��LED1
            DelayMS(500);
        }
        else if (flow1==1)
        { //������Ч��
            LED1 = ON;    //����LED1
            DelayMS(500);
            LED1 = OFF;   //Ϩ��LED1
            LED2 = ON;
            DelayMS(500);
            LED2 = OFF;
            LED3 = ON;
            DelayMS(500);
            LED3 = OFF;
        }
        else if(flow2==1)
        { //����������
            LED3 = ON;
            DelayMS(500);
            LED3 = OFF;
            LED2 = ON;
            DelayMS(500);
            LED2 = OFF;
            LED1 = ON;    //����LED1
            DelayMS(500);
            LED1 = OFF;   //Ϩ��LED1
        }
        else if(flow3==1)
        { //LED2�Ƶ���˸
            LED2 = ON;
            DelayMS(500);
            LED2 = OFF;
            DelayMS(250);
        }
    //===�޸�
    }
}
