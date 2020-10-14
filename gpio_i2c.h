#include <reg52.h>
#ifndef _GPIO_I2C_H
#define _GPIO_I2C_H

/*
    SCL --> P2.5
    SDA --> P2.6
*/

sbit I2C_SCL = P2^5;
sbit I2C_SDA = P2^6;

void I2C_START();
void I2C_STOP();
void I2C_ACK();
void I2CSendByte(unsigned char val);

#endif