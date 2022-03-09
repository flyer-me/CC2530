#include "ioCC2530.h"
#define uint unsigned int
#define uchar unsigned char

//������ƵƵĶ˿�
#define LED1 P1_0       //����P1.0��ΪLED1���ƶ�
#define LED2 P1_1       //����P1.1��ΪLED2���ƶ�

void delay(uint);
void initUART(void);
void InitialAD(void);
void UartTX_Send_String(char *Data,int len);

char adcdata[]=" 0.0V\n ";

//�޸�========
uchar adcComplete = 0;
char OUT[] = "0";  //ADCL + ADCH �洢
float num;unsigned char temp[2] = "00"; uint adc = 0;   //��Ҫ���˶���д���жϺ����лᵼ�²ɼ��������

#pragma vector = ADC_VECTOR
__interrupt void ADC_ISR(void)
{
  LED1 = 0;	  	//ת�����ָʾ
  LED2= 1;           //�����ݴ���ָʾ��
  
  temp[1] = ADCL;
  temp[0] = ADCH;

  adc |= (uint)temp[1];
  adc |= ( (uint) temp[0] )<<8;
  adc>>=2; // ADCL[1:0]û��ʹ��
  num = adc*1.25/8192;//�ο���ѹΪ1.25V,12λ��Чλ
  adcdata[1] = (char)(num)%10+48;
  adcdata[3] = (char)(num*10)%10+48;
  adcComplete = 1; //����ת����ɵ�ȫ�ֱ���
 
  OUT[0] = temp[1] + temp[0];

  uint debug = 0;             //����
  debug |= temp[0] + temp[1]; //����:����temp������� ����ADCL+ADCH
  adc = debug;                //���ԣ�����debug�������
  
  adc = 0;    //��Ҫ�� ����һ��ת��ǰ����
}

void MemToStr(unsigned char *str, unsigned char *src, unsigned char len) {
  unsigned char tmp;
  while (len--)
  {
    tmp = *src >> 4; //��ȡ��4λ
    if (tmp <= 9)
      *str++ = tmp + '0'; //ת��Ϊ0-9��A-F
    else *str++ = tmp -10 + 'A';

    tmp = *src & 0x0F; //��ȡ��4λ
    if (tmp <= 9)
      *str++ = tmp + '0'; //ת��Ϊ0-9��A-F
    else
      *str++ = tmp -10 + 'A';
    *str++ = ' '; //ת����һ���ֽ����һ���ո�
    src++;
   }
}
//�޸�========

/****************************************************************
*�������� ��������						*
*��ڲ��� ����							*
*�� �� ֵ ����							*
*˵    �� ����							*
****************************************************************/
void main(void)
{
  P1DIR |= 0x03;

  initUART();   //��ʼ������
  InitialAD();       //��ʼ��ADC

  while(1)
  {
    
    if(adcComplete)
    {
      UartTX_Send_String((char*)OUT[0],sizeof(OUT));	//��������
      //���������������� ʵ���н�д��������ʹ��
      LED2= 0;      //������ݴ���
      delay(3000);
      adcComplete = 0;
      InitialAD(); //������һ��ת��
    }
    
    /*****��ѯ��ʽ
   if(ADCCON1>=0x80)    //ADCCON1.EOC,ת������ж�
    {
     LED1 = 0;	  	//ת�����ָʾ
     LED2= 1;           //�����ݴ���ָʾ��

     temp[1] = ADCL;
     temp[0] = ADCH;
     
      adc |= (uint)temp[1];
      adc |= ( (uint) temp[0] )<<8;

      adc>>=2; // ADCL[1:0]û��ʹ��
      num = adc*3.3/8192;//���ο���ѹΪ3.3V��14λ�ֱ���
      adcdata[1] = (char)(num)%10+48;
      adcdata[3] = (char)(num*10)%10+48;
      UartTX_Send_String(adcdata,6);	//��������
      
      LED2 = 0;                       //������ݴ���
      adc = 0;      //����adc �Ա��´�ת��
      //delay(30000);
      delay(3000);
      InitialAD(); //������һ��ת��
    }
    */
  }
}


/****************************************************************
*�������� ����ʱ						*
*��ڲ��� ��������ʱ						*
*�� �� ֵ ����							*
*˵    �� ��							*
****************************************************************/
void delay(uint time)
{ uint i;
  uchar j;
  for(i = 0; i < time; i++)
  {  for(j = 0; j < 240; j++)
      {   asm("NOP");    // asm����Ƕ��࣬nop�ǿղ���,ִ��һ��ָ������
          asm("NOP");
          asm("NOP");
       }
   }
}


/****************************************************************
*�������� ����ʼ������1				*
*��ڲ��� ����							      *
*�� �� ֵ ����							      *
*˵    �� ��57600-8-n-1						*
****************************************************************/
void initUART(void)
{
 CLKCONCMD &= 0x80;           //����32MHz

  PERCFG = 0x00;			//λ��1 P0��
  P0SEL = 0x0C;				//P0��������

  U0CSR |= 0x80;			//UART��ʽ
  U0GCR |= 10;				//baud_e = 10;
  U0BAUD |= 216;			//��������Ϊ57600
  UTX0IF = 1;

  U0CSR |= 0X40;			//�������
  IEN0 |= 0x84;				//�����жϣ������ж�
}


/****************************************************************
*�������� ����ʼ��ADC			  *
*��ڲ��� ����					        	*
*�� �� ֵ ����							      *
*˵    �� ���ο���ѹAVDD��ת��������1/3AVDD			*
****************************************************************/
void InitialAD(void)
{

  P0SEL  |= (1 << (0));	  //����P0.0Ϊ����IO��
  P0DIR  &= ~ (1 << (0)); //����P0.0Ϊ����I/O
  //�޸�========
  IEN0 |= 0x02;  //�ж�ʹ�� IEN0.ADCIE=1
  EA = 1;
  //�޸�========
  LED1=1; //����ADCת���� ָʾ��

  ADCCON1 &= ~0x80;		//��EOC��־
  APCFG  |=1;             //����P0.0Ϊģ��I/O
  ADCCON3=0xb0;		//����ת��,�ο���ѹΪ��Դ��ѹ  //14λ�ֱ���
  ADCCON1 = 0X30;	//ֹͣA/D
  ADCCON1 |= 0X40;	//ADCCON1.ST=1������A/D
}
/****************************************************************
*�������� �����ڷ����ַ�������		   *
*��ڲ��� : data:����						    *
*	         len :���ݳ���				    *
*�� �� ֵ ����						          *
*˵    �� ��							          *
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
}
