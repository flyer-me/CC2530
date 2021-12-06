
#include "lcd.h"
#include <ioCC2530.h>

#define uint unsigned int
#define uchar unsigned char

void Initial_IO(void)
{
    P0DIR |= 0xff;     //P0����Ϊ���
    P1DIR |= 0xff;     //P1����Ϊ���
}

//�޸�========
//������SPI��ʼ������**************************
void Init_SPI_Master()//Master SPI��ʼ��
{
	//Master Mode
	PERCFG =0x02 ;	//PERCFG.U1CFG = 1

	P1SEL|=0xE0; 	//����P1_5��P1_6��P1_7Ϊ����IO
	P1SEL&=~0x10;	//P1_4Ϊ SSN
	P1DIR|=0x10;	//SSN ���� output

	U1CSR &= ~0xA0; //SPI MAster ģʽ
	U1BAUD = 0x3b; 	//����BAUD_MΪ59
	U1GCR |= 0x10; 	//����BAUD_EΪ10 ��F = 32MHz ������ = 38400
	U1GCR &= ~0xC0;	// CPOL = CPHA = 0
	U1GCR |= 0x20;	//ORDER = 1 MSB��λ�ȴ���
}

//�޸�========
//******************************************************
int main(void)
 {
	 u8 i,m;
	 float t=0;

         Initial_IO();
         Init_SPI_Master();
	 Lcd_Init();			//��ʼ��OLED
	 LCD_Clear(WHITE);
	 BACK_COLOR=WHITE;

	 while(1)
	 {
		LCD_ShowChinese32x32(10,0,0,32,GBLUE);   //��
		LCD_ShowChinese32x32(45,0,1,32,GBLUE);   //��
		LCD_ShowChinese32x32(80,0,2,32,GBLUE);   //԰
		LCD_ShowChinese32x32(115,0,3,32,GBLUE);  //��
		LCD_ShowChinese32x32(150,0,4,32,GBLUE);  //��
		LCD_ShowChinese32x32(10,75,0,16,RED);   //��
		LCD_ShowChinese32x32(45,75,1,16,RED);   //��
		LCD_ShowChinese32x32(80,75,2,16,RED);   //԰
		LCD_ShowChinese32x32(115,75,3,16,RED);  //��
		LCD_ShowChinese32x32(150,75,4,16,RED);  //��
		LCD_ShowString(10,35,"2.8 TFT SPI 240*320",RED);
		LCD_ShowString(10,55,"LCD_W:",RED);	LCD_ShowNum(70,55,LCD_W,3,RED);
		LCD_ShowString(110,55,"LCD_H:",RED);LCD_ShowNum(160,55,LCD_H,3,RED);

        	for(i=0;i<2;i++)
			{
				for(m=0;m<6;m++)
				{
		        	LCD_ShowPicture(0+m*40,120+i*40,39+m*40,159+i*40);
				}
			}

        	while(1)
			{
            	LCD_ShowNum1(80,200,t,5,RED);
		    	t+=0.01;
        	}
    }//  while loop
}
