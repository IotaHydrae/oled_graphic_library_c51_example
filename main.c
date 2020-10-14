#include <reg52.h>
#include <intrins.h>
#include "oled.h"
#include "gpio_spi.h"
#include "gpio_i2c.h"

#define SPI_MODE 1

void main()
{
	int x1, y1, x2, y2;
	#ifdef SPI_MODE
	SPIInit();
	#endif
	OLEDInit();

	while (1)
	{
		OLEDPrint(0, 0, "Learn and teach through making");
		OLEDPrint(4, 0, "Welcome to raspberrypi world!");
		Delayms(1000);
		OLEDClear();

		#ifndef SPI_MODE 
		OLEDPrint(0, 0, "I2C MODE");
		OLEDPrint(2, 0, "I2C MODE");
		OLEDPrint(4, 0, "I2C MODE");
		OLEDPrint(6, 0, "I2C MODE");
		Delayms(1000);
		OLEDClear();
		#else
		OLEDPrint(0, 0, "SPI MODE");
		OLEDPrint(2, 0, "SPI MODE");
		OLEDPrint(4, 0, "SPI MODE");
		OLEDPrint(6, 0, "SPI MODE");
		Delayms(1000);
		OLEDClear();
		#endif


		/* 9圆 */
		OLEDBresenhamCircle(31, 31, 30);
		OLEDBresenhamCircle(31, 31, 20);
		OLEDBresenhamCircle(31, 31, 10);

		OLEDBresenhamCircle(63, 31, 30);
		OLEDBresenhamCircle(63, 31, 20);
		OLEDBresenhamCircle(63, 31, 10);

		OLEDBresenhamCircle(94, 31, 30);
		OLEDBresenhamCircle(94, 31, 20);
		OLEDBresenhamCircle(94, 31, 10);
		OLEDClear();

		/* 心率折线 */
		OLEDBresenhamLine(0, 31, 10, 31);
		OLEDBresenhamLine(10, 31, 20, 55);
		OLEDBresenhamLine(20, 55, 30, 10);

		OLEDBresenhamLine(30, 10, 40, 31);
		OLEDBresenhamLine(40, 31, 50, 31);
		OLEDBresenhamLine(50, 31, 80, 31);

		OLEDBresenhamLine(80, 31, 90, 55);
		OLEDBresenhamLine(90, 55, 100, 10);
		OLEDBresenhamLine(100, 10, 110, 31);
		OLEDBresenhamLine(110, 31, 127, 31);

		OLEDClear();

		/* 梯形 */
		for (x1 = 0, x2 = 127, y1 = 0, y2 = 63; x1 <= 127, y1 <= 63; x1++, y1++, x2--, y2--)
		{
			OLEDBresenhamLine(x1, y1, x2, y2);
		}
		Delayms(200);
		OLEDClear();

		/* 星空 */
		for (x1 = 0, y1 = 0, x2 = 0, y2 = 63; x2 < 127; x1 += 4, x2 += 4)
		{
			OLEDBresenhamLine(x1, y1, x2, y2);
		}
		Delayms(200);
		OLEDClear();

		/* 满屏横线 */
		for (x1 = 0, y1 = 63, x2 = 127, y2 = 63; y1 > 0; y1 -= 4, y2 -= 4)
		{
			OLEDBresenhamLine(x1, y1, x2, y2);
		}
		Delayms(200);
		OLEDClear();

		//斜线
		for (x1 = 0, y1 = 0, x2 = 0, y2 = 0; x2 < 63; y1++, x2 += 2)
		{
			OLEDBresenhamLine(x1, y1, x2, y2);
		}
		Delayms(200);
		OLEDClear();

		OLEDPrint(0, 0, "Temp:30C");
		OLEDPrint(2, 0, "Saturday");
		OLEDPrint(4, 0, "Weather:Clear");
		OLEDPrint(6, 0, "     10:30");
		Delayms(2000);
		OLEDClear();
	}
}
