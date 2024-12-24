#ifndef I2C_H
#define I2C_H

#include <REG52.H>
#include <INTRINS.H>

#include "util.h"

#define uchar        unsigned char
#define Nack_counter 10

sbit SCL = P1 ^ 7; // 时钟线
sbit SDA = P1 ^ 6; // 数据线

bdata uchar flag; // 可位寻址数据
sbit bit_out = flag ^ 7;
sbit bit_in  = flag ^ 0;

void start_bit(void)
{
    SDA = 1;
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    SCL = 1;
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    SDA = 0;
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    SCL = 0;
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
}

void stop_bit(void)
{

    SCL = 0;
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    SDA = 0;
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    SCL = 1;
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    SDA = 1;
}

void send_bit(void)
{
    if (bit_out == 0)

        SDA = 0;
    else
        SDA = 1;
    _nop_();
    SCL = 1;
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    SCL = 0;
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
}

void receive_bit(void)
{
    SDA    = 1;
    bit_in = 1;
    SCL    = 1;
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    bit_in = SDA;
    _nop_();
    SCL = 0;
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
}

void tx_byte(uchar dat_byte)
{
    char i, n, dat;
    n = Nack_counter;
TX_again:
    dat = dat_byte;
    for (i = 0; i < 8; i++) {
        if (dat & 0x80)
            bit_out = 1;
        else
            bit_out = 0;
        send_bit();
        dat = dat << 1;
    }

    receive_bit();
    if (bit_in == 1) {
        stop_bit();
        if (n != 0) {
            n--;
            goto Repeat;
        } else
            goto exit;
    } else
        goto exit;
Repeat:
    start_bit();
    goto TX_again;
exit:;
}

uchar rx_byte(void)
{
    uchar i, dat;
    dat = 0;
    for (i = 0; i < 8; i++) {
        dat = dat << 1;
        receive_bit();
        if (bit_in == 1)
            dat = dat + 1;
    }
    send_bit();
    return dat;
}

void i2c_init(void)
{
    SCL = 1;
    SDA = 1;
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    SCL = 0;
    delay(1000);
    SCL = 1;
}

#endif