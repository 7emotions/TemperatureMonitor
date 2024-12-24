#ifndef UART_H
#define UART_H
#include <REG52.H>

#define uchar unsigned char

// 串口初始化函数
void UartInit() // 4800bps@11.0592MHz
{
    PCON &= 0x7F; // 波特率不倍速
    SCON = 0x50;  // 8位数据,可变波特率
    TMOD &= 0x0F; // 清除定时器1模式位
    TMOD |= 0x20; // 设定定时器1为8位自动重装方式
    TL1 = 0xFA;   // 设定定时初值
    TH1 = 0xFA;   // 设定定时器重装值
    ET1 = 0;      // 禁止定时器1中断
    TR1 = 1;      // 启动定时器1
    ES  = 1;      // 使能串口中断
    EA  = 1;      // 使能全局中断
}

// 串口发送一个字节数据
void UART_SendByte(uchar Byte)
{
    SBUF = Byte;
    // 检测是否完成
    while (TI == 0);
    TI = 0; // TI复位
}

void UART_SendBytes(uchar *Bytes, uchar Length)
{
    while (Length--) {
        UART_SendByte(*Bytes++);
    }
}

// 串口接收一个字节
uchar UART_ReceiveByte()
{
    while (!RI); // 等待接收完成
    RI = 0;      // 清除接收中断标志
    return SBUF; // 返回接收到的字节
}
#endif