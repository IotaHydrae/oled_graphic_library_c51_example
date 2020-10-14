#ifndef _OLED_H
#define _OLED_H

void OLEDInit(void);
void OLEDClear();
void OLEDPutChar(int page, int col, char c);
void OLEDPrint(int page, int col, char *str);
void OLEDPutPixel(int x, int y);
void OLEDBresenhamLine(int x1, int y1, int x2, int y2);
void OLEDDrawLine(int x1, int y1, int x2, int y2, int width);
void OLEDMidPointCircle(int x1, int y1, int r);
void OLEDBresenhamCircle(int x1, int y1, int r);
void Delayms(int n);    //@11.0592MHz
void DelayS(int n);		//@11.0592MHz

#endif