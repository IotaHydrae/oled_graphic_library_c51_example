#include <reg52.h>
#ifndef _GPIO_SPI_H
#define _GPIO_SPI_H
/*
    SCK-->P2.0
    SDA-->P2.1
    RES-->P2.2
     DC-->  P2.3
     CS--> P2.4
*/
sbit SCK = P2^0;
sbit SDA = P2^1;
sbit RES = P2^2;
sbit DC = P2^3;
sbit CS = P2^4;

void SPIInit(void);
void SPISendByte(unsigned char val);

#endif