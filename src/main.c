#include <INTRINS.H>
#include <REG52.H>
#include <STDIO.H>
#include <STRING.H>
#include <STDLIB.H>

#include "i2c.h"
#include "util.h"
#include "uart.h"

#define BUFFER_SIZE 10

sbit BUZZER = P2 ^ 0; // ����������P2.0

char received_string[BUFFER_SIZE]; // ���ڽ��մ����ַ���
uchar receive_index        = 0;    // �����ַ���������
float received_float       = 0.0;  // ���ڱ���ת����ĸ�����
float temperatureThreshold = 20;   // Ĭ����ֵΪ20���϶�

uchar DataH, DataL, Pecreg;

// ���Ʒ�����
void Buzzer_Control(uchar state)
{
    if (state) {
        BUZZER = 0; // ��������
    } else {
        BUZZER = 1; // ������ͣ
    }
}

// ���ڽ����жϷ������
void UART_ISR(void) interrupt 4
{
    uchar byte;

    // ����Ƿ�Ϊ�����ж�
    if (RI) {
        RI   = 0;    // ��������жϱ�־
        byte = SBUF; // ��ȡ���յ��������ֽ�

        // ������յ����з���'\n') ��س�����'\r')����Ϊ���ݽ������
        if (byte == '\n' || byte == '\r' || receive_index >= BUFFER_SIZE - 1) {
            received_string[receive_index] = '\0'; // �ַ���������
            receive_index                  = 0;    // ���ý�������
            // ת�����յ����ַ���Ϊ������
            received_float = atof(received_string);
            // ���͸������ش���
            if (received_float >= -40.0 && received_float <= 125.0) {
                uchar buff_[10];
                temperatureThreshold = received_float;
                sprintf(buff_, "%.2f", received_float);
                UART_SendBytes("New threshold set: ", 20);
                UART_SendBytes(buff_, strlen(buff_));
                UART_SendBytes((uchar *)" C\r\n", 4);
            } else {
                // ������Χ�����ʹ�����Ϣ
                UART_SendBytes((uchar *)"Error: Temperature out of range.\r\n", 34);
            }
        } else {
            // ����������ַ������뻺����
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
