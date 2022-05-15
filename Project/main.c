//主程序
#include "stdio.h"
#include "delay.h"
#include "lcd.h"
#include "init.h"
#include "24C02.h"

union
{ unsigned int i;
  float f;
}humi_val,temp_val; 	//定义两个共同体，一个用于湿度，一个用于温度
float temp_value = 0;
float humi_value = 0;

void main(void)
{	
	unsigned char error,checksum;
	unsigned char HUMI,TEMP;
	unsigned char addr = 0;//地址
	unsigned char data[5];//送入EEPROM数据
	char humi_result[2] = {0};
	unsigned char valueCount = 0;//测量计数
	HUMI=0X01;
	TEMP=0X02;

    Initial_IO();	//IO初始化
	InitLed();		//LED初始化	 
    InitKey();      //按键初始化   
	Lcd_Init();		//LCD初始化
	InitUart();     //串口初始化函数
	InitT1();		//定时器T1初始化
	InitialADC();	//ADC初始化
	s_connectionreset();//SHT11初始化
	LCD_Clear(WHITE);//清屏
	BACK_COLOR=WHITE;

	LCD_ShowChinese32x32(20,5,0,16,RED);   //显示名字
	LCD_ShowChinese32x32(38,5,1,16,RED);   
	LCD_ShowChinese32x32(54,5,2,16,RED);   
	//显示学号
	LCD_ShowNum(90,5,12,2,RED);
	LCD_ShowNum(106,5,34,2,RED);
	LCD_ShowNum(122,5,56,2,RED);
	LCD_ShowNum(138,5,78,2,RED);
    LCD_ShowPicture(0,25,179,264);//显示图片：放大后分辨率为240*180
	
    LCD_ShowString(58,280,"ADC:",RED);        
    LCD_ShowString(10,300,"Tempeture:",RED);        
	uchar clear[]="      ";//LCD清除字符串
                
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
						LCD_ShowString(90,300,clear,RED);//清除温度显示
						LCD_ShowString(90,280,adcdata,RED);//ADC显示
      					adcComplete = 0;
      					InitialADC(); //启动下一次转换
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
					//定时器0.1s中断一次 10次为1s延时
					LED3 = 0;
					error = 0;
					error+=s_measure((unsigned char*) &temp_val.i,&checksum,TEMP);  //温度测量
					temp_value = temp_val.i * 0.01 - 39.6;
					LCD_ShowString(90,280,clear,RED);		//清除ADC显示
					LCD_ShowNum1(90,300,temp_value,4,RED);	//温度显示
					
					if(valueCount<100)
					{
						LCD_ShowNum(200,300,valueCount,2,RED);	//测量计数 调试/验证
						LED2 = 1;
						error+=s_measure((unsigned char*) &humi_val.i,&checksum,HUMI);  //湿度测量
						humi_value = humi_val.i * 0.0367 - 2.0468;
						
						sprintf(humi_result,"%.f",humi_value*100);//保存整数和2位小数
						E2WriteP((unsigned char*)humi_result, addr, 2);		//数据存入EEPROM
						valueCount++;
						addr += 2;
					}
					else
					{
						LED2 = 0;//熄灭LED2 停止湿度测量
						addr = 0;//测量结束指向数据起始点 准备读出
					}
					if(error!=0)
					{
						s_connectionreset() ; //如果发生错误，系统复位
						LED1 = !LED1;
					}	
					else
					{ 
						humi_val.f=(float)humi_val.i;                   //转换为浮点数
						temp_val.f=(float)temp_val.i;                   //转换为浮点数
						calc_sth11(&humi_val.f,&temp_val.f);            //修正相对湿度及温度
					}
					
				}	
			}
		//KEY3
			if(flag_KEY == 3)
			{
				while(flag_KEY == 3)
				{
					LCD_ShowString(90,300,clear,RED);	//清除温度显示
					LCD_ShowString(90,280,clear,RED);	//清除ADC显示
					LED2 = 0;

					LED3 = LED || GetData;	//未接受命令将闪烁，接收命令将常亮
					//经过GetData# 命令判断
					while(addr<200 & GetData)	//输出100个湿度值 方式：每次读取两次 一次整数 一次小数 UART调整格式输出
					{
						E2Read(data, addr,1);
						UartTX_Send_String((char*)data, 2);	//整数输出
						UartTX_Send_String(".", 1);		//显示小数点
						E2Read(data, addr + 1,1);
						UartTX_Send_String((char*)data, 2);	//小数输出
						UartTX_Send_String("\n", 1);	//换行
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
