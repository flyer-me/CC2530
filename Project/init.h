#ifndef __INIT_
#define __INIT_	

#include <ioCC2530.h>
#include <string.h>
#define uint unsigned int
#define uchar unsigned char
#define Uint16 unsigned int

extern unsigned char flag_KEY;  // �������б�־��1��KEY1/2��KEY2/3��KEY3
extern unsigned char GetData;	//GetData����ָʾ
extern unsigned short t1_count; //Timer1�����������
extern unsigned char adcdata[];          //ADC����
extern unsigned char adcComplete;       //ADC�������ָʾ
extern unsigned char LED;               //LED��˸��ʶ
//LED***************
#define LED1 P1_0       //P1_0��ΪLED1���ƶ�
#define LED2 P1_1       //P1_1��ΪLED2���ƶ�
#define LED3 P1_4       //P1_4��ΪLED3���ƶ�
//KEY***************
#define KEY1 P0_1       // P0_1�ڿ��ư���KEY1
#define KEY2 P2_0       // P2_0�ڿ��ư���KEY2
#define KEY3 P0_7       // P0_7�ڿ��ư���KEY3
//UART***************
//P0_2,P0_3�������ڣ����蹦�ܣ�
//ADC***************
#define AIN4 P0_4       // P0_4��ΪADC�����ˣ�AIN4��
//I2C***************
#define SDA   P0_5//����ͨѶ���ݶ˿�
#define SCL    P0_6//����ͨѶʱ�Ӷ˿�
#define noACK 0       //�����������ݣ������ж��Ƿ����ͨѶ
#define ACK   1       //�������ݴ��䣻
                            //��ַ  ����
#define MEASURE_TEMP 0x03   //000   00011
#define MEASURE_HUMI 0x05   //000   00101

#define ON      1
#define OFF     0
void Initial_IO(void);
void DelayMS(uint msec);
void LedOnOrOff(uchar mode);
void InitLed();
void InitKey();
void InitT1();
void InitialADC();
void InitUart(void);

void UartTX_Send_String(char *Data,int len);
void delay1Us(Uint16 microSecs);
void init_uart(void);
void s_connectionreset(void);
void s_transstart(void);
char s_measure(unsigned char *p_value, unsigned char *p_checksum, unsigned char mode);
char s_write_byte(unsigned char value);
char s_read_byte(unsigned char ack);
void calc_sth11(float *p_humidity ,float *p_temperature);
float calc_dewpoint(float h,float t);
void delay(unsigned int time);

#endif  