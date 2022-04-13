//������
#include "stdio.h"
#include "delay.h"
#include "lcd.h"
#include "init.h"
#include "24C02.h"

union
{ unsigned int i;
  float f;
}humi_val,temp_val; 	//����������ͬ�壬һ������ʪ�ȣ�һ�������¶�
float temp_value = 0;
float humi_value = 0;

void main(void)
{	
	unsigned char error,checksum;
	unsigned char HUMI,TEMP;
	unsigned char addr = 0;//��ַ
	unsigned char data[5];//����EEPROM����
	char humi_result[2] = {0};
	unsigned char valueCount = 0;//��������
	HUMI=0X01;
	TEMP=0X02;

    Initial_IO();	//IO��ʼ��
	InitLed();		//LED��ʼ��	 
    InitKey();      //������ʼ��   
	Lcd_Init();		//LCD��ʼ��
	InitUart();     //���ڳ�ʼ������
	InitT1();		//��ʱ��T1��ʼ��
	InitialADC();	//ADC��ʼ��
	s_connectionreset();//SHT11��ʼ��
	LCD_Clear(WHITE);//����
	BACK_COLOR=WHITE;

	LCD_ShowChinese32x32(20,5,0,16,RED);   //��ʾ���֣�������
	LCD_ShowChinese32x32(38,5,1,16,RED);   
	LCD_ShowChinese32x32(54,5,2,16,RED);   
	//��ʾѧ�ţ�41924045
	LCD_ShowNum(90,5,41,2,RED);
	LCD_ShowNum(106,5,92,2,RED);
	LCD_ShowNum(122,5,40,2,RED);
	LCD_ShowNum(138,5,45,2,RED);
    LCD_ShowPicture(0,25,179,264);//��ʾͼƬ���Ŵ��ֱ���Ϊ240*180
	
    LCD_ShowString(58,280,"ADC:",RED);        
    LCD_ShowString(10,300,"Tempeture:",RED);        
	uchar clear[]="      ";//LCD����ַ���
                
        while(1)
		{
        //KEY1
			if(flag_KEY == 1)
			{
				while(flag_KEY == 1)
				{

					LED2 = 0;
					LED3 = 0;
					if(adcComplete)
    				{
						LCD_ShowString(90,300,clear,RED);//����¶���ʾ
						LCD_ShowString(90,280,adcdata,RED);//ADC��ʾ
      					adcComplete = 0;
      					InitialADC(); //������һ��ת��
    				}
				}
				
			}
		//KEY2
			if(flag_KEY == 2)
			{
				while(flag_KEY == 2)
				{
					while(t1_count%10);
					t1_count -= 10;
					//��ʱ��0.1s�ж�һ�� 10��Ϊ1s��ʱ
					LED3 = 0;
					error = 0;
					error+=s_measure((unsigned char*) &temp_val.i,&checksum,TEMP);  //�¶Ȳ���
					temp_value = temp_val.i * 0.01 - 39.6;
					LCD_ShowString(90,280,clear,RED);		//���ADC��ʾ
					LCD_ShowNum1(90,300,temp_value,4,RED);	//�¶���ʾ
					
					if(valueCount<100)
					{
						LCD_ShowNum(200,300,valueCount,2,RED);	//�������� ����/��֤
						LED2 = 1;
						error+=s_measure((unsigned char*) &humi_val.i,&checksum,HUMI);  //ʪ�Ȳ���
						humi_value = humi_val.i * 0.0367 - 2.0468;
						
						sprintf(humi_result,"%.f",humi_value*100);//����������2λС��
						E2WriteP((unsigned char*)humi_result, addr, 2);		//���ݴ���EEPROM
						valueCount++;
						addr += 2;
					}
					else
					{
						LED2 = 0;//Ϩ��LED2 ֹͣʪ�Ȳ���
						addr = 0;//��������ָ��������ʼ�� ׼������
					}
					if(error!=0)
					{
						s_connectionreset() ; //�����������ϵͳ��λ
						LED1 = !LED1;
					}	
					else
					{ 
						humi_val.f=(float)humi_val.i;                   //ת��Ϊ������
						temp_val.f=(float)temp_val.i;                   //ת��Ϊ������
						calc_sth11(&humi_val.f,&temp_val.f);            //�������ʪ�ȼ��¶�
					}
					
				}	
			}
		//KEY3
			if(flag_KEY == 3)
			{
				while(flag_KEY == 3)
				{
					LCD_ShowString(90,300,clear,RED);	//����¶���ʾ
					LCD_ShowString(90,280,clear,RED);	//���ADC��ʾ
					LED2 = 0;

					LED3 = LED || GetData;	//δ���������˸�������������
					//����GetData# �����ж�
					while(addr<200 & GetData)	//���100��ʪ��ֵ ��ʽ��ÿ�ζ�ȡ���� һ������ һ��С�� UART������ʽ���
					{
						E2Read(data, addr,1);
						UartTX_Send_String((char*)data, 2);	//�������
						UartTX_Send_String(".", 1);		//��ʾС����
						E2Read(data, addr + 1,1);
						UartTX_Send_String((char*)data, 2);	//С�����
						UartTX_Send_String("\n", 1);	//����
						addr+=2;
						DelayMS(5);
					}
				}
				
			}
        }

    //}//  while loop
}
void uart_Get()
{

}
