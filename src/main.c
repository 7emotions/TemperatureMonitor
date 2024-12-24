#include <INTRINS.H>
#include <REG52.H>
#include <STDIO.H>
#include <STRING.H>
#include <STDLIB.H>

#include "i2c.h"
#include "util.h"
#include "uart.h"

#define BUFFER_SIZE 10

sbit BUZZER = P2 ^ 0; // 蜂鸣器连接P2.0

char received_string[BUFFER_SIZE]; // 用于接收串口字符串
uchar receive_index        = 0;    // 接收字符串的索引
float received_float       = 0.0;  // 用于保存转换后的浮动数
float temperatureThreshold = 20;   // 默认阈值为20摄氏度

uchar DataH, DataL, Pecreg;

// 控制蜂鸣器
void Buzzer_Control(uchar state)
{
    if (state) {
        BUZZER = 0; // 蜂鸣器响
    } else {
        BUZZER = 1; // 蜂鸣器停
    }
}

// 串口接收中断服务程序
void UART_ISR(void) interrupt 4
{
    uchar byte;

    // 检查是否为接收中断
    if (RI) {
        RI   = 0;    // 清除接收中断标志
        byte = SBUF; // 获取接收到的数据字节

        // 如果接收到换行符（'\n') 或回车符（'\r')，认为数据接收完成
        if (byte == '\n' || byte == '\r' || receive_index >= BUFFER_SIZE - 1) {
            received_string[receive_index] = '\0'; // 字符串结束符
            receive_index                  = 0;    // 重置接收索引
            // 转换接收到的字符串为浮动数
            received_float = atof(received_string);
            // 发送浮动数回串口
            if (received_float >= -40.0 && received_float <= 125.0) {
                uchar buff_[10];
                temperatureThreshold = received_float;
                sprintf(buff_, "%.2f", received_float);
                UART_SendBytes("New threshold set: ", 20);
                UART_SendBytes(buff_, strlen(buff_));
                UART_SendBytes((uchar *)" C\r\n", 4);
            } else {
                // 超出范围，发送错误信息
                UART_SendBytes((uchar *)"Error: Temperature out of range.\r\n", 34);
            }
        } else {
            // 如果是正常字符，存入缓冲区
            received_string[receive_index++] = byte;
        }
    }
}

uint memread(void)
{
    start_bit();
    tx_byte(0x00);
    tx_byte(0x07);
    start_bit();
    tx_byte(0x01);
    bit_out = 0;
    DataL   = rx_byte();
    bit_out = 0;
    DataH   = rx_byte();
    bit_out = 1;
    Pecreg  = rx_byte();
    stop_bit();
    return (DataH << 8 | DataL);
}

void main()
{
    uint TemRaw;
    float temp;

    i2c_init();
    UartInit();

    while (1) {
        uchar buff[10];
        TemRaw = memread();
        temp   = (float)TemRaw * 0.02 - 273.15;
        sprintf(buff, "%.2f", temp);
        UART_SendBytes("Temperature: ", 14);
        UART_SendBytes(buff, strlen(buff));
        UART_SendBytes(" C\n", 4);

        sprintf(buff, "%.2f", temperatureThreshold);
        UART_SendBytes("Threshold: ", 11);
        UART_SendBytes(buff, strlen(buff));
        UART_SendBytes(" C\n", 4);

        if (temp > temperatureThreshold) {
            Buzzer_Control(1);
            UART_SendBytes("Alert\n", 7);

        } else {
            Buzzer_Control(0);
            UART_SendBytes("Normal\n", 8);
        }

        UART_SendByte('\n');

        Delay_ms(1000);
    }
}
