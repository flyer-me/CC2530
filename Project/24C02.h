#ifndef __24C02_
#define __24C02_	
#include "I2C.h"
// GPIO模拟I2C，引脚为P0.5和P0.6
#define ACK 1
#define noACK 0

/*******************************************************************************
* 函数名         : void At24c02Write(unsigned char addr,unsigned char dat)
* 函数功能		   : 往24c02的一个地址写入一个数据
*******************************************************************************/
void At24c02Write(unsigned char addr,unsigned char dat)
{
	I2cStart();
	I2cSendByte(0xa2);//发送写器件地址
	I2cSendByte(addr);//发送要写入内存地址
	I2cSendByte(dat);	//发送数据
	I2cStop();
}
/*
*E2Write()多个单字节的连续写入
* E2写入函数，buf-源数据指针，addr-E2中的起始地址，len-写入长度*/
void E2Write(unsigned char *buf, unsigned char addr, unsigned char len)
{
     while (len--)
     {
     do {                           //用寻址操作查询当前是否可进行读写操作
        I2cStart();
        if (I2cSendByte(0x51<<1)) break; //应答则跳出循环，非应答则进行下一次查询
        I2cStop();
     } while(1);

    I2cSendByte(addr++);  //写入起始地址
    I2cSendByte(*buf++);  //写入一个字节数据
    I2cStop();          //结束写操作，以等待写入完成
     }
}

/*
*E2WriteP() EEPROM页写入
* E2写入函数，buf-源数据指针，addr-E2中的起始地址，len-写入长度*/
void E2WriteP(unsigned char *buf, unsigned char addr, unsigned char len)
{
    while (len > 0)
    {
    do {    //等待上次写入操作完成，用寻址操作查询当前是否可进行读写操作
        I2cStart();
        if (I2cSendByte(0x51<<1)) break;   //应答则跳出循环，非应答则进行下一次查询
        I2cStop();
     } while(1);

    //按页写模式连续写入字节
    I2cSendByte(addr);            //写入起始地址
    while (len > 0)
    {
       I2cSendByte(*buf++);      //写入一个字节数据
       len--;                   //待写入长度计数递减
       addr++;                  //E2地址递增
       if ((addr&0x07) == 0) break; //是否到达页边界，24C02每页8字节，只检查低3位是否为零  到达页边界时结束写操作
    }
    I2cStop();
    }
}


/* E2读取函数，buf-数据接收指针，addr-E2中的起始地址，len-读取长度*/
void E2Read(unsigned char *buf, unsigned char addr, unsigned char len) 
{
  do {                           //用寻址操作查询当前是否可进行读写操作
     I2cStart();
     if (I2cSendByte(0x51<<1)) 
     {   //应答则跳出循环，非应答则进行下一次查询
        break;   
     }
      I2cStop();
    } while(1);

   I2cSendByte(addr);              //写入起始地址
   I2cStart();                   //发送重复启动信号
   I2cSendByte((0x51<<1)|0x01);  //寻址器件，后续为读操作
   while (len > 1) 
    {   //连续读取len-1个字节
      *buf++ = I2cReadByte(ACK); //最后字节之前为读取操作+应答
       len--;  
    }
      *buf = I2cReadByte(noACK);       //最后一个字节为读取操作+非应答
   I2cStop();  
}

/* 将一段内存数据转换为十六进制格式的字符串，
str-字符串指针，src-源数据地址，len-数据长度*/
void MemToStr(unsigned char *str, unsigned char *src, unsigned char len) 
{
  unsigned char tmp;
  while (len--)
  {
    tmp = *src >> 4; //先取高4位
    if (tmp <= 9)
      *str++ = tmp + '0'; //转换为0-9或A-F
    else *str++ = tmp -10 + 'A';

    tmp = *src & 0x0F; //再取低4位
    if (tmp <= 9)
      *str++ = tmp + '0'; //转换为0-9或A-F
    else
      *str++ = tmp -10 + 'A';
    *str++ = ' '; //转换完一个字节添加一个空格
    src++;
   }
}
/*******************************************************************************
* 函数名         : unsigned char At24c02Read(unsigned char addr)
* 函数功能		   : 读取24c02的一个地址的一个数据
* 输出         	 : 返回读到的字节
*******************************************************************************/
unsigned char At24c02Read(unsigned char addr)
{
	unsigned char num;
	I2cStart();
	I2cSendByte(0xa2); //发送写器件地址
	I2cSendByte(addr); //发送要读取的地址
	I2cStart();
	I2cSendByte(0xa3); //发送读器件地址
	num = I2cReadByte(noACK); //读取数据，不继续
	I2cStop();
	return num;
}

#endif