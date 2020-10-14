#include <reg52.h>
#include <intrins.h>
#include <math.h>
#include "oledfont.h"

//#define SPI_MODE 1
//#define MCU_12C5A60S2 1

void Delayms(int n);
void DelayS(int n);

/* oled位置结构体 */
struct Position
{
    /* 
        page：页
        col: 列
        rem：页余数
     */
    int page;
    int col;
    int rem;
};

#ifdef SPI_MODE
#include "gpio_spi.h"
/* SPI模式 */
static void OLED_Set_DC(char val)
{
    if (val)
        DC = 1;
    else
        DC = 0;
}

static void OLED_Set_CS(char val)
{
    if (val)
        CS = 1;
    else
        CS = 0;
}

static void OLEDWriteCmd(unsigned char cmd)
{
    OLED_Set_DC(0); /*command*/
    OLED_Set_CS(0); /* selected */

    SPISendByte(cmd);

    OLED_Set_DC(1);
    OLED_Set_CS(1);
}

static void OLEDWriteDat(unsigned char dat)
{
    OLED_Set_DC(1); /*dat*/
    OLED_Set_CS(0); /* selected */

    SPISendByte(dat);

    OLED_Set_DC(1);
    OLED_Set_CS(1);
}
#else
/* I2C模式 */
#include "gpio_i2c.h"
static void OLEDWriteCmd(unsigned char cmd)
{
    I2C_START();
    I2CSendByte(0x00);
    I2C_ACK();
    I2CSendByte(cmd);
    I2C_ACK();
    I2C_STOP();
}

static void OLEDWriteDat(unsigned char dat)
{
    I2C_START();
    I2CSendByte(0x40);
    I2C_ACK();
    I2CSendByte(dat);
    I2C_ACK();
    I2C_STOP();
}

#endif

static void OLEDSetPos(int page, int col)
{
    /* 设置页地址 */
    OLEDWriteCmd(0xB0 + page);

    /* 设置列地址 */
    OLEDWriteCmd(0x00 | (col & 0x0f)); /* 低地址 */
    OLEDWriteCmd(0x10 | (col >> 4));   /* 高地址 */
}

void OLEDClear()
{
    int page, col;
    for (page = 0; page < 8; page++)
    {
        OLEDSetPos(page, 0);
        for (col = 0; col < 128; col++)
            OLEDWriteDat(0);
    }
}

/*
    A[1:0] = 00b, Horizontal Addressing Mode
    A[1:0] = 01b, Vertical Addressing Mode
    A[1:0] = 10b, Page Addressing Mode (RESET)
    A[1:0] = 11b, Invalid

*/
static void OLEDSetMemoryAddressingMode(int mode)
{
    OLEDWriteCmd(0x20);
    switch (mode)
    {
    case 0:
        OLEDWriteCmd(0x00);
        break; // Horizontal Addressing Mode
    case 1:
        OLEDWriteCmd(0x01);
        break; // Vertical Addressing Mode
    case 2:
        OLEDWriteCmd(0x02);
        break; // Page Addressing Mode
    case 3:
        OLEDWriteCmd(0x03);
        break; // Invalid
    default:
        OLEDWriteCmd(0x02);
        break; // Page Addressing Mode
    }
}
void OLEDInit(void)
{
    OLEDWriteCmd(0xae); //--turn off oled panel

    OLEDWriteCmd(0x00); //--set low column address
    OLEDWriteCmd(0x10); //--set high column address

    OLEDWriteCmd(0x40); //--set start line address

    OLEDWriteCmd(0xb0); //--set page address

    OLEDWriteCmd(0x81); //--set contrast control register
    OLEDWriteCmd(0xff);

    OLEDWriteCmd(0xa1); //--set segment re-map 127 to 0   a0:0 to seg127
    OLEDWriteCmd(0xa6); //--set normal display

    OLEDWriteCmd(0xc9); //--set com(N-1)to com0  c0:com0 to com(N-1)

    OLEDWriteCmd(0xa8); //--set multiples ratio(1to64)
    OLEDWriteCmd(0x3f); //--1/64 duty

    OLEDWriteCmd(0xd3); //--set display offset
    OLEDWriteCmd(0x00); //--not offset

    OLEDWriteCmd(0xd5); //--set display clock divide ratio/oscillator frequency
    OLEDWriteCmd(0x80); //--set divide ratio

    OLEDWriteCmd(0xd9); //--set pre-charge period
    OLEDWriteCmd(0xf1);

    OLEDWriteCmd(0xda); //--set com pins hardware configuration
    OLEDWriteCmd(0x12);

    OLEDWriteCmd(0xdb); //--set vcomh
    OLEDWriteCmd(0x40);

    OLEDWriteCmd(0x8d); //--set chare pump enable/disable
    OLEDWriteCmd(0x14); //--set(0x10) disable
                        // OLEDWriteCmd(0x10);	//--set(0x10) disable
    OLEDWriteCmd(0xaf); //--turn on oled panel

    OLEDClear();
    OLEDSetMemoryAddressingMode(2);
}

void OLEDPutChar(int page, int col, char c)
{
    int i = 0;
    /* 获取字模 */
    const unsigned char *dots = oled_asc2_8x16[c - ' '];

    /* 发送到OLED */
    OLEDSetPos(page, col);
    /* 发出8字节数据 */
    for (i = 0; i < 8; i++)
        OLEDWriteDat(dots[i]);

    /* 再次发送8字节数据 */
    OLEDSetPos(page + 1, col);
    for (i = 0; i < 8; i++)
        OLEDWriteDat(dots[i + 8]);
}

/*
    page: 0-7
    col : 0-127
*/
void OLEDPrint(int page, int col, char *str)
{
    /* res = 128*64 */
    int i = 0;
    while (str[i])
    {
        OLEDPutChar(page, col, str[i]);
        col += 8;
        if (col > 127)
        {
            col = 0;
            page += 2;
        }
        i++;
    }
}

/*
 *********************************************
 * OLED DRAW API
 * 以下部分为OLED绘图库api函数
 *********************************************
 */

/* 余数转十六进制函数 */
unsigned char SelectBitValue(int val)
{
    unsigned char temp = 0x00;
    unsigned char op = 0x01;
    unsigned char result;
    val = 7 - val;
    result = temp | (op << val);
    return result;
}

/* xy坐标点转换为page和Colum */
struct Position XY2PageCol(int x, int y)
{
    struct Position pos;

    pos.page = 7 - y / 8;
    pos.col = x;
    pos.rem = y % 8;
    return pos;
}

/* 描点函数draw range x:0~127 y:0~63 */
void OLEDPutPixel(int x, int y)
{
    /* 将接收到的x,y坐标转换为page和col */
    unsigned char dat;
    struct Position pos;

    pos = XY2PageCol(x, y); //坐标转换

    OLEDSetPos(pos.page, pos.col); //设置画笔位置
    dat = SelectBitValue(pos.rem); //获取页余数
    OLEDWriteDat(dat);             //向oled中写数据
}

/* OLED画线函数，可以指定起点、终点 */

void swap_int(int *a, int *b)
{
    *a ^= *b;
    *b ^= *a;
    *a ^= *b;
}
void OLEDBresenhamLine(int x1, int y1, int x2, int y2)
{
    // 参数 c 为颜色值
    int dx, dy, yy;
    int ix, iy, cx, cy, n2dy, n2dydx, d;
    dx = abs(x2 - x1);
    dy = abs(y2 - y1);
    yy = 0;

    if (dx < dy)
    {
        yy = 1;
        swap_int(&x1, &y1);
        swap_int(&x2, &y2);
        swap_int(&dx, &dy);
    }

    ix = (x2 - x1) > 0 ? 1 : -1;
    iy = (y2 - y1) > 0 ? 1 : -1;
    cx = x1;
    cy = y1;
    n2dy = dy * 2;
    n2dydx = (dy - dx) * 2;
    d = dy * 2 - dx;

    if (yy)
    { // 如果直线与 x 轴的夹角大于 45 度
        while (cx != x2)
        {
            if (d < 0)
            {
                d += n2dy;
            }
            else
            {
                cy += iy;
                d += n2dydx;
            }
            OLEDPutPixel(cy, cx);
            cx += ix;
        }
    }
    else
    { // 如果直线与 x 轴的夹角小于 45 度
        while (cx != x2)
        {
            if (d < 0)
            {
                d += n2dy;
            }
            else
            {
                cy += iy;
                d += n2dydx;
            }
            OLEDPutPixel(cx, cy);
            cx += ix;
        }
    }
}

/* 1/8圆弧对称性画整圆 */
void CirclepPoint(int x, int y, int offset_x, int offset_y)
{
    OLEDPutPixel(x + offset_x, y + offset_y);
    OLEDPutPixel(y + offset_x, x + offset_y);
    OLEDPutPixel(x + offset_x, -y + offset_y);
    OLEDPutPixel(-y + offset_x, x + offset_y);
    OLEDPutPixel(-x + offset_x, y + offset_y);
    OLEDPutPixel(y + offset_x, -x + offset_y);
    OLEDPutPixel(-x + offset_x, -y + offset_y);
    OLEDPutPixel(-y + offset_x, -x + offset_y);
}

/* 中点画圆算法 */
void OLEDMidPointCircle(int x1, int y1, int r)
{
    int x, y, e;
    x = 0;
    y = r;
    e = 1 - r;
    CirclepPoint(x, y, x1, y1);
    while (x <= y)
    {
        if (e < 0)
            e += 2 * x + 3;
        else
        {
            e += 2 * (x - y) + 5;
            y--;
        }
        x++;
        CirclepPoint(x, y, x1, y1);
        Delayms(10);
    }
}

/* Bresenham画圆算法 */
void OLEDBresenhamCircle(int x1, int y1, int r)
{
    int x, y, p;
    x = 0;
    y = r;
    p = 3 - 2 * r;
    for (; x <= y; x++)
    {
        CirclepPoint(x, y, x1, y1);
        if (p >= 0)
        {
            p += 4 * (x - y) + 10;
            y--;
        }
        else
        {
            p += 4 * x + 6;
        }
        Delayms(10);
    }
}

#ifdef MCU_12C5A60S2
void Delayms(int n) //@11.0592MHz
{
    unsigned char i, j;
    int m;

    for (m = 0; m < n; m++)
    {
        _nop_();
        i = 11;
        j = 190;
        do
        {
            while (--j)
                ;
        } while (--i);
    }
}

void DelayS(int n) //@11.0592MHz
{
    unsigned char i, j, k;
    int m;

    for (m = 0; m < n; m++)
    {
        i = 43;
        j = 6;
        k = 203;
        do
        {
            do
            {
                while (--k)
                    ;
            } while (--j);
        } while (--i);
    }
}
#else
void Delayms(int n) //@11.0592MHz
{
    unsigned char i, j;
    int m;

    for (m = 0; m < n; m++)
    {
        _nop_();
        i = 2;
        j = 199;
        do
        {
            while (--j)
                ;
        } while (--i);
    }
}

void DelayS(int n) //@11.0592MHz
{
    unsigned char i, j, k;
    int m;
    for (m = 0; m < n; m++)
    {
        _nop_();
        i = 8;
        j = 1;
        k = 243;
        do
        {
            do
            {
                while (--k)
                    ;
            } while (--j);
        } while (--i);
    }
}

#endif
