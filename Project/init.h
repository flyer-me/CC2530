#ifndef __INIT_
#define __INIT_	

#include <ioCC2530.h>
#include <string.h>
#define uint unsigned int
#define uchar unsigned char
#define Uint16 unsigned int

extern unsigned char flag_KEY;  // 程序运行标志，1：KEY1/2：KEY2/3：KEY3
extern unsigned char GetData;	//GetData命令指示
extern unsigned short t1_count; //Timer1溢出次数计数
extern unsigned char adcdata[];          //ADC测量
extern unsigned char adcComplete;       //ADC测量完成指示
extern unsigned char LED;               //LED闪烁标识
//LED***************
#define LED1 P1_0       //P1_0口为LED1控制端
#define LED2 P1_1       //P1_1口为LED2控制端
#define LED3 P1_4       //P1_4口为LED3控制端
//KEY***************
#define KEY1 P0_1       // P0_1口控制按键KEY1
#define KEY2 P2_0       // P2_0口控制按键KEY2
#define KEY3 P0_7       // P0_7口控制按键KEY3
//UART***************
//P0_2,P0_3用作串口（外设功能）
//ADC***************
#define AIN4 P0_4       // P0_4口为ADC测量端（AIN4）
//I2C***************
#define SDA   P0_5//定义通讯数据端口
#define SCL    P0_6//定义通讯时钟端口
#define noACK 0       //继续传输数据，用于判断是否结束通讯
#define ACK   1       //结束数据传输；
                            //地址  命令
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