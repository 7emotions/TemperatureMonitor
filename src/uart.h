#ifndef UART_H
#define UART_H
#include <REG52.H>

#define uchar unsigned char

// ���ڳ�ʼ������
void UartInit() // 4800bps@11.0592MHz
{
    PCON &= 0x7F; // �����ʲ�����
    SCON = 0x50;  // 8λ����,�ɱ䲨����
    TMOD &= 0x0F; // �����ʱ��1ģʽλ
    TMOD |= 0x20; // �趨��ʱ��1Ϊ8λ�Զ���װ��ʽ
    TL1 = 0xFA;   // �趨��ʱ��ֵ
    TH1 = 0xFA;   // �趨��ʱ����װֵ
    ET1 = 0;      // ��ֹ��ʱ��1�ж�
    TR1 = 1;      // ������ʱ��1
    ES  = 1;      // ʹ�ܴ����ж�
    EA  = 1;      // ʹ��ȫ���ж�
}

// ���ڷ���һ���ֽ�����
void UART_SendByte(uchar Byte)
{
    SBUF = Byte;
    // ����Ƿ����
    while (TI == 0);
    TI = 0; // TI��λ
}

void UART_SendBytes(uchar *Bytes, uchar Length)
{
    while (Length--) {
        UART_SendByte(*Bytes++);
    }
}

// ���ڽ���һ���ֽ�
uchar UART_ReceiveByte()
{
    while (!RI); // �ȴ��������
    RI = 0;      // ��������жϱ�־
    return SBUF; // ���ؽ��յ����ֽ�
}
#endif