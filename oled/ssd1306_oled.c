/*
 * ssd1306_oled.c
 *
 * Created: 8/20/2016 7:37:12 AM
 *  Author: Philip
 *
 */ 

#include <string.h>
#include <asf.h>
#include "oled_fonts.h"
#include "ssd1306_oled.h"
#include "ssd1306_config.h"
#include "ssd1306_i2c.h"

#define swap(a, b) { int16_t t = a; a = b; b = t; }

void oled_prints(const char * st, uint8_t x, uint8_t y){
	int stl, i;
	stl = strlen(st);

	for (i=0; i<stl; i++){
	    char c = *st++;				
		if(c == 0x20){		
			uint8_t indx = oled_getIndex(c);
		    uint8_t width = font.descriptors[indx][0];				
			x = width;		
		} else {			
			oled_printc(c, x + (font.last_font_width), y);
			x=1;
		}	
		
	}	
	font.last_font_width = 0;
}

void oled_printf(uint8_t x, uint8_t y, const char* fmt, ... )
{
	va_list arglist;
	char dest[MAXSTRINGSIZE];
	va_start( arglist, fmt );
	vsprintf(dest, fmt, arglist);
	va_end( arglist );
	char buff[MAXSTRINGSIZE];
	snprintf(buff, sizeof(buff), dest, arglist);
	oled_prints(buff,x,y);
	
}

void oled_printc(char c, uint8_t x, uint8_t y){
	
	uint8_t indx = oled_getIndex(c);
	font.width = font.descriptors[indx][0];
	font.height = font.descriptors[indx][1];		
	font.offSet = font.descriptors[indx][2];
		
	int nextIndx = 0;
	if (indx + 1 >= font.num_of_chars) { // array size 96
		
		nextIndx = font.offSet +  font.descriptors[1][2] -  font.descriptors[0][2];
		
	} else {
		nextIndx =  font.descriptors[indx + 1][2];
	}
	
	uint8_t bitmap[font.width * font.height];

	uint8_t ix = 0;
			
	for(int z = font.offSet; z < nextIndx; z++){ // copy the char data over
		bitmap[ix] = font.font[z];
		ix++;
	}		

	uint8_t current_byte = 0;
			
	uint16_t bytesPerRow = font.width/8;
	
	uint8_t remainder = (font.width % 8);
	if(remainder > 0){
		bytesPerRow++;
	}
	uint16_t ct = 0;
	for (uint16_t row = y; row < font.height + y; row++) {
		//printf("row%d\r", row);	
		for (uint8_t col = 0; col < bytesPerRow; col++) {			
			current_byte = bitmap [ct];
			//printf("col\r");
			for (int ii = 0; ii < font.width; ii++) {					
				if(current_byte & (1 << ii)){
					oled_drawPixel(x+ii+(col*8),row);
					font.last_font_width = (x+ii+(col*8)) + font.space; // keep track of last 1 bit written, this will calculate the space bet						
				  
				} else {
				   int indexX = x+ii+(col*8);
				   buffer[indexX +(row/8)*8] &= ~(1 << ii&7);
				  			
				}				
			}				
			ct++;
			
		}	
			
	}
	
}

void oled_clear_line(uint8_t line){
	for (uint8_t current_y = line; current_y < font.height + line; current_y++) {
		for(int i = 0; i < SSD1306_WIDTH; i++){
			buffer[i +(current_y/8)*SSD1306_WIDTH] = 0x00;
		}
	}
	oled_update();

}


unsigned char reverse(unsigned char b) {
	b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
	b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
	b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
	return b;
}

void oled_drawBitmap(const uint8_t *bitmap, uint8_t height, uint8_t width, uint8_t pos_x, uint8_t pos_y) {
	uint8_t current_byte;
	uint8_t byte_width = (width + 7)/8;

	for (uint8_t current_y = 0; current_y < height; current_y++) {
		for (uint8_t current_x = 0; current_x < width; current_x++) {
			current_byte = bitmap [current_y*byte_width + current_x/8];
			if (current_byte & (128 >> (current_x&7))) {
				oled_drawPixel(current_x+pos_x,current_y+pos_y);
			} 
		}
	}
}

void oled_drawCircle(int16_t x0, int16_t y0, int16_t r) {
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

	oled_drawPixel(x0  , y0+r);
	oled_drawPixel(x0  , y0-r);
	oled_drawPixel(x0+r, y0 );
	oled_drawPixel(x0-r, y0 );

	while (x<y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		oled_drawPixel(x0 + x, y0 + y);
		oled_drawPixel(x0 - x, y0 + y);
		oled_drawPixel(x0 + x, y0 - y);
		oled_drawPixel(x0 - x, y0 - y);
		oled_drawPixel(x0 + y, y0 + x);
		oled_drawPixel(x0 - y, y0 + x);
		oled_drawPixel(x0 + y, y0 - x);
		oled_drawPixel(x0 - y, y0 - x);
	}
}
void oled_drawCircleHelper( int16_t x0, int16_t y0, int16_t r, uint8_t cornername){
	int16_t f     = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x     = 0;
	int16_t y     = r;

	while (x<y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f     += ddF_y;
		}
		x++;
		ddF_x += 2;
		f     += ddF_x;
		if (cornername & 0x4) {
			oled_drawPixel(x0 + x, y0 + y);
			oled_drawPixel(x0 + y, y0 + x);
		}
		if (cornername & 0x2) {
			oled_drawPixel(x0 + x, y0 - y);
			oled_drawPixel(x0 + y, y0 - x);
		}
		if (cornername & 0x8) {
			oled_drawPixel(x0 - y, y0 + x);
			oled_drawPixel(x0 - x, y0 + y);
		}
		if (cornername & 0x1) {
			oled_drawPixel(x0 - y, y0 - x);
			oled_drawPixel(x0 - x, y0 - y);
		}
	}
	
}

// fill example found http://stackoverflow.com/a/14976268/893219
void oled_fillCircle(int16_t x0, int16_t y0, int16_t r) {
	
	
	int x = r;
	int y = 0;
	int xChange = 1 - (r << 1);
	int yChange = 0;
	int radiusError = 0;

	while (x >= y)
	{
		for (int i = x0 - x; i <= x0 + x; i++)
		{
			oled_drawPixel(i, y0 + y);
			oled_drawPixel(i, y0 - y);
		}
		for (int i = x0 - y; i <= x0 + y; i++)
		{
			oled_drawPixel(i, y0 + x);
			oled_drawPixel(i, y0 - x);
		}

		y++;
		radiusError += yChange;
		yChange += 2;
		if (((radiusError << 1) + xChange) > 0)
		{
			x--;
			radiusError += xChange;
			xChange += 2;
		}
	}
	
}
void init_oled(){
	
	write_command(SSD1306_DEACTIVATE_SCROLL);
	write_command(SSD1306_DISPLAYOFF);

	write_command(SSD1306_SETDISPLAYCLOCKDIV);
	write_command(0x80);

	write_command(SSD1306_SETMULTIPLEX);
	write_command(0x3F);
	
	write_command(SSD1306_SETDISPLAYOFFSET);
	write_command(0x00);
	
	write_command(SSD1306_SETSTARTLINE | 0x00);
	
	// We use internal charge pump
	write_command(SSD1306_CHARGEPUMP);
	write_command(0x14);
	
	// Horizontal memory mode
	write_command(SSD1306_MEMORYMODE);
	write_command(0x00);
	
	write_command(SSD1306_SEGREMAP | 0x1);

	write_command(SSD1306_COMSCANDEC);

	write_command(SSD1306_SETCOMPINS);
	write_command(0x12);

	// Max contrast
	write_command(SSD1306_SETCONTRAST);
	write_command(0xCF);

	write_command(SSD1306_SETPRECHARGE);
	write_command(0xF1);

	write_command(SSD1306_SETVCOMDETECT);
	write_command(0x40);

	write_command(SSD1306_DISPLAYALLON_RESUME);

	// Non-inverted display
	write_command(SSD1306_NORMALDISPLAY);

	// Turn display back on
	write_command(SSD1306_DISPLAYON);
	
	delay_ms(10);
	oled_clear();
}

void oled_drawPixel(uint8_t x, uint8_t y)
{   
	if (x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT) {
		return;
	}
	
	buffer[x+(y/8)*SSD1306_WIDTH] |= (1 << (y&7));
	
}
void oled_drawVLine(uint8_t x, uint8_t y, uint8_t length) {
	
	for (uint8_t i = 0; i < length; ++i) {
		oled_drawPixel(x,i+y);
	}
}
void oled_drawHLine(uint8_t x, uint8_t y, uint8_t length) {
	for (uint8_t i = 0; i < length; ++i) {
		oled_drawPixel(i+x,y);
	}
}
void oled_clear() {
	for (uint16_t buffer_location = 0; buffer_location < SSD1306_FBSIZE; buffer_location++) {
		buffer[buffer_location] = 0x00;
	}
	oled_update();
}
void oled_drawRectangle(uint8_t x, uint8_t y, uint8_t w, uint8_t h) {
			
	oled_drawHLine(x,y, w);
	oled_drawHLine(x, y+h-1, w);
	oled_drawVLine(x, y, h);	
	oled_drawVLine(x+w-1, y, h);
}


void oled_drawRectangle_fill(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t fill) {
	
	if (!fill) {

		oled_drawRectangle(x,y,w,h);

	} else {
		
		for (int x2 = x; x2 < w + x; ++x2) {
			for (int y2 = y; y2 <= h + y; ++y2) {
				oled_drawPixel(x2,y2);			
			}
		}
	}
}

void oled_draw_triangle(uint8_t x0, uint8_t y0,	uint8_t x1, uint8_t y1,	uint8_t x2, uint8_t y2){

	  oled_drawLine(x0, y0, x1, y1);
	  oled_drawLine(x1, y1, x2, y2);
	  oled_drawLine(x2, y2, x0, y0);
}

void oled_drawLine(uint8_t x0, uint8_t y0,uint8_t x1, uint8_t y1){

	int16_t steep = abs(y1 - y0) > abs(x1 - x0);
	if (steep) {
		swap(x0, y0);
		swap(x1, y1);
	}

	if (x0 > x1) {
		swap(x0, x1);
		swap(y0, y1);
	}

	int16_t dx, dy;
	dx = x1 - x0;
	dy = abs(y1 - y0);

	int16_t err = dx / 2;
	int16_t ystep;

	if (y0 < y1) {
		ystep = 1;
		} else {
		ystep = -1;
	}

	for (; x0<=x1; x0++) {
		if (steep) {
			oled_drawPixel(y0, x0);
		} else {
			oled_drawPixel(x0, y0);
		}
		err -= dy;
		if (err < 0) {
			y0 += ystep;
			err += dx;
		}
	}
}

// Activate a right handed scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.oled_hor_scroll(0x00, 0x0F)
void oled_hor_scroll(uint8_t start, uint8_t end, uint8_t direction){
	
	write_command(SSD1306_DEACTIVATE_SCROLL); // stop scrolling
	write_command(direction);
	write_command(0x00); // dummy data	A
	write_command(0x00); // row 0		B  start page
	write_command(0x00); //	Time		C  0 is fastest
	write_command(0x0F); // row 15		D  end page
	write_command(0x00); // dummy data	E
	write_command(0xFF); //	Finish		F
	write_command(SSD1306_ACTIVATE_SCROLL);
}

void oled_startscrolldiag(uint8_t start, uint8_t stop, uint8_t direction){
	write_command(SSD1306_DEACTIVATE_SCROLL); // stop scrolling
	write_command(SSD1306_SET_VERTICAL_SCROLL_AREA);
	write_command(0x00);
	write_command(SSD1306_HEIGHT);
	write_command(direction);
	write_command(0x00);
	write_command(start);
	write_command(0x00);
	write_command(stop);
	write_command(0x01);
	write_command(SSD1306_ACTIVATE_SCROLL);
}

void oled_update(){
	
	write_command(SSD1306_COLUMNADDR);
	write_command(0x00);
	write_command(0x7F);

	write_command(SSD1306_PAGEADDR);
	write_command(0x00);
	write_command(0x07);
	
	uint8_t buff[100];
	buff[0] = 0x40;

	#if BOARD == SAMD21_XPLAINED_PRO
		wr_packet.address  = ADDRESS;
		for (uint8_t packet = 0; packet < 64; packet++) {			
			for (uint8_t x = 0; x < 16; ++x) {				
				buff[x+1] = buffer[(packet*16) + x];				
			}
			wr_packet.data_length = 17;
			wr_packet.data        = buff;
			i2c_master_write_packet_wait(&i2c_master_instance, &wr_packet);
		}
	#elif  BOARD == SAM4S_XPLAINED_PRO	 
	 
	   packet.chip = (ADDRESS >> 1);	   
	   for (uint8_t packetx = 0; packetx < 64; packetx++) {
		   for (uint8_t x = 0; x < 16; ++x) {
			   buff[x+1] = buffer[(packetx*16) + x];
		   }
		     packet.buffer = &buff;
		     packet.length = 17;
		   twi_master_write(TWI_CHANNEL, &packet);
	   }  

	#endif
	
}


void oled_invert(uint8_t inverted) {
	if (inverted) {
		write_command(SSD1306_INVERTDISPLAY);
	} else {
		write_command(SSD1306_NORMALDISPLAY);
	}
}

void oled_setFont(const unsigned char* f, const unsigned int (*d)[3], uint8_t c, const unsigned char* cm, int8_t space){
	
	font.font = f;
	font.descriptors = d;
	font.num_of_chars = c;
	font.charmap = cm;
	font.space = space;
}

uint8_t oled_getIndex(char letter){
	
	uint8_t index = 0;

	for(uint8_t i = 0; i < font.num_of_chars; i++){
		
		if(letter == charMap[i]){
			
			return i;
		}
		
	}
	return index;
}

void oled_scroll(void){
	
	int scroll = 0;
	
	for(int x = 0; x<8;x++){
		
		for (int i = 0; i < 8; ++i) {
			delay_ms(30);
			write_command(SSD1306_SETSTARTLINE | scroll % 64);
			++scroll;
		}
		delay_ms(30);
	}
	
	//write_command(SSD1306_SETSTARTLINE); // reset
	
}





