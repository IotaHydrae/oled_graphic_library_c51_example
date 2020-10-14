#include <reg52.h>
#include <intrins.h>
#include "gpio_i2c.h"

/* 使用GPIO模拟I2C */

void I2C_START()
{
    I2C_SDA = 0;

    I2C_SCL = 1;

    I2C_SCL = 0;

    I2CSendByte(0x78);
    I2C_ACK();
}
void I2C_STOP()
{
    I2C_SCL = 1;

    I2C_SDA = 0;

    I2C_SDA = 1;
}
void I2C_ACK()
{
    I2C_SDA = 1;

    I2C_SCL = 1;

    I2C_SCL = 0;
}

void I2CSendByte(unsigned char val)
{
    unsigned char length = 8;
    while (length--)
    {
        if (val & 0x80)
        {
            I2C_SDA = 1;
        }
        else
        {
            I2C_SDA = 0;
        }
        I2C_SCL = 1;
        I2C_SCL = 0;
        val = val << 1;
    }
}