/*
 * SamILI9325.h
 *
 * Created: 10/28/2016 5:41:31 AM
 *  Author: pvallone
 */ 


#ifndef SAM_TFT_H_
#define SAM_TFT_H_
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#define				MAXSTRINGSIZE 1024
#define				PORTAMASK 0xC30 //PA04, PA05, PA10 & PA11
#define				PORTBMASK_8 0x0000FF // PB00-PB07
#define				PORTBMASK_16 0x00FFFF // PB00-PB15

#define RS          PORT_PA10
#define WR          PORT_PA11
#define CS          PORT_PA04
#define RS_T        PORT_PA05


#define				LEFT 0
#define				RIGHT 9999
#define				CENTER 9998
#define				PORTRAIT 0
#define				LANDSCAPE 1

#define swap(type, i, j) {type t = i; i = j; j = t;}

struct currentFont
{
	uint8_t* font;
	uint8_t x_size;
	uint8_t y_size;
	uint8_t offset;
	uint8_t numchars;
} cfont;

struct charInfo {

	int width;
	int offset;

} ci;

struct fontSet {

	const unsigned char* font; // your font array
	const unsigned int (*descriptors)[3]; // your font info
	uint8_t num_of_chars; // number of characters in your font set
	uint8_t rows;

} fi;

enum tft_type {
	ILI9325D_8 = 0,
	ILI9325D_16 = 1,
	SSD1963_480 = 2,
	SSD1289 = 3
};

struct tft_chip{	
	enum tft_type tft_model;
	int width;
	int height;
	uint8_t pages;
	uint8_t orient;
	int page_1; // start address of page
	int page_2;
	int page_3;
};
struct tft_chip tft_conf;

int current_x;
int current_y;

uint8_t fcolorr, fcolorg, fcolorb;
uint8_t bcolorr, bcolorg, bcolorb;

const unsigned char* cMap;            

void sam_setPinIO(void);
void sam_initTft(void);
void sam_setTFTProperties(void);

/* tft commands*/
void sam_clearRST(void);
void sam_setRST(void);
void sam_clearRs(void); 
void sam_setRs(void);
void sam_clearCs(void);
void sam_setCs(void);
void sam_writeComData(uint8_t com, uint16_t data);
void sam_write16ComData(uint8_t com, uint16_t data);
void sam_writeToTFT(uint8_t vl);
void sam_write16ToTFT(uint8_t vh, uint8_t vl);
void sam_pulseBitLow(void);
void sam_writeBus(uint8_t hv, uint8_t lv);
void sam_writeCom(uint8_t com);
void sam_writeData(uint8_t vh, uint8_t vl);
void sam_writeByteData(uint8_t data);
void sam_setXY(int x1, int y1, int x2, int y2);
void sam_clrXY(void);
void sam_clrScr(void);
void sam_scroll(int y);
void sam_setScrollArea(uint16_t top, uint16_t scr, uint16_t bottom);

/* paging functions*/
void sam_fillPage(uint8_t r, uint8_t g, uint8_t b, uint8_t page);
void sam_setPage(int pageNumber);
/* fonts */
void sam_setCharMap(const unsigned char* cm);
uint8_t sam_getIndex(char c);
void sam_setColor(uint8_t r, uint8_t g, uint8_t b);
void sam_setBackColor(uint8_t r, uint8_t g, uint8_t b);
void sam_setFont(const unsigned char* f, const unsigned int (*d)[3], uint8_t c);
/* BMPs*/
void sam_load_raw_image_mem(int x1, int y1, int imgX, int imgY, const uint16_t *img, int size);
void sam_test(int x, int y, int sx, int sy, const uint16_t *data);
/* draw functions*/
void sam_setPixel(uint8_t r, uint8_t g, uint8_t b);
void sam_fillScr(uint8_t r, uint8_t g, uint8_t b);
void sam_drawRect(int x1, int y1, int x2, int y2);
void sam_drawPixel(int x, int y);
void sam_drawCircle(int x, int y, int radius);
void sam_drawRoundRect(int x1, int y1, int x2, int y2);
void sam_fillRect(int x1, int y1, int x2, int y2);
void sam_fillRoundRect(int x1, int y1, int x2, int y2);
void sam_fillCircle(int x, int y, int radius);
void sam_drawLine(int x1, int y1, int x2, int y2);
void sam_drawHLine(int x, int y, int l);
void sam_drawVLine(int x, int y, int l);
void sam_print(const char *st, int x, int y);
void sam_printChar(char ch, int x, int y);
void sam_printf(uint8_t x, uint8_t y, const char* fmt, ... );

#endif /* SAM_TFT_H_ */