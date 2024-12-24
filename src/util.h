#ifndef UTIL_H
#define UTIL_H

#include <INTRINS.H>

#define uint unsigned int

void Delay_ms(uint N)
{
    uint i, j;
    for (i = 0; i < N; i++) {
        for (j = 0; j < 120; j++) {
            ;
        }
    }
}

void delay(uint N)
{
    uint i;
    for (i = 0; i < N; i++)
        _nop_();
}

#endif