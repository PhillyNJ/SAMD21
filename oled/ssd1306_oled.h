/*
 * ssd1306_oled.h
 *
 * Created: 8/20/2016 7:36:58 AM
 *  Author: Philip
 */ 


#ifndef SSD1306_OLED_H_
#define SSD1306_OLED_H_
#define MAXSTRINGSIZE 1024
#include <stdarg.h>
#include "ssd1306_config.h"
uint8_t buffer[SSD1306_FBSIZE];

void init_oled(void);
void oled_drawPixel(uint8_t x, uint8_t y);
void oled_update(void);
void oled_invert(uint8_t inverted);
void oled_drawVLine(uint8_t x, uint8_t y, uint8_t length);
void oled_drawHLine(uint8_t x, uint8_t y, uint8_t length);
void oled_drawRectangle(uint8_t x, uint8_t y, uint8_t w, uint8_t h);
void oled_drawRectangle_fill(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t fill);
void oled_draw_triangle(uint8_t x0, uint8_t y0,	uint8_t x1, uint8_t y1,	uint8_t x2, uint8_t y2);

void oled_drawLine(uint8_t x0, uint8_t y0,uint8_t x1, uint8_t y1);
void oled_drawCircleHelper( int16_t x0, int16_t y0, int16_t r, uint8_t cornername);
void oled_fillCircle(int16_t x0, int16_t y0, int16_t r);
void oled_clear(void);
void oled_printc(char c, uint8_t x, uint8_t y);
void oled_clear_line(uint8_t line);
unsigned char reverse(unsigned char b);
void oled_setFont(const unsigned char* f, const unsigned int (*d)[3], uint8_t c, const unsigned char* cm, int8_t space);
void oled_prints(const char * st, uint8_t x, uint8_t y);
void oled_printf(uint8_t x, uint8_t y, const char* fmt, ... );
void oled_drawBitmap(const uint8_t *bitmap, uint8_t height, uint8_t width, uint8_t pos_x, uint8_t pos_y);
uint8_t oled_getIndex(char letter);
void oled_drawCircle(int16_t x0, int16_t y0, int16_t r);
void oled_scroll(void);
void oled_hor_scroll(uint8_t start, uint8_t end, uint8_t direction);
void oled_startscrolldiag(uint8_t start, uint8_t stop, uint8_t direction);




#endif /* SSD1306_OLED_H_ */