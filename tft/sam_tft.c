/*
 * SamILI9325.c
 *
 * Created: 10/28/2016 5:41:41 AM
 *  Author: pvallone
 */
 #include "sam_tft.h"
 #include <asf.h>
 #include <string.h>

 void sam_setPinIO(){
	 
	 REG_PORT_DIRSET0 = PORTAMASK; // set direction to output
	 REG_PORT_DIRSET1 = PORTBMASK_8;
	 
	 switch(tft_conf.tft_model){
		 
		 case ILI9325D_8:
		 REG_PORT_DIRSET1 = PORTBMASK_8;
		 break;
		 case ILI9325D_16:
		 case SSD1963_480:
		 case SSD1963_800:
		 case SSD1289:
		 REG_PORT_DIRSET1 = PORTBMASK_16;
		 break;
		 default:
		 printf("Device not recognized\n\r");
		 break;
	 }
	 
 }
 void sam_initTft(){
     switch(tft_conf.tft_model){
		case SSD1289:
			tft_width = 239;
			tft_height = 319;
			tft_conf.pages = 1;
			tft_conf.page_1 = 0;			
			break;
		case SSD1963_480:
			tft_width = 272;
			tft_height = 480;
			tft_conf.pages = 3;
			tft_conf.page_1 = 0;
			tft_conf.page_2 = 272;
			tft_conf.page_3 = 544;
			break;
		case SSD1963_800:
			// 1 max "SSD1963 is a display controller of 1215K byte frame buffer to support up to 864 x 480 x 24bit graphics content"
			// The 800X480 only has one page :(
			tft_width = 479;
			tft_height = 799;
			tft_conf.pages = 1;
			tft_conf.page_1 = 0;		
			break;
		default:
			printf("TFT Width and Height not defined in sam_initTft()\n");
			break;
	 }
	
	 sam_setRST(); 
	 sam_clearRST();
	 sam_setRST();	
	 sam_clearCs();
	 sam_setTFTProperties();
 }
 void sam_clearRST() {   
	
	REG_PORT_OUTCLR0 = RS_T;
 }

 void sam_setRST() {
   
	 REG_PORT_OUTSET0 = RS_T;	 
 }

void sam_clearRs() {
	
	REG_PORT_OUTCLR0 = RS;
}
void sam_setRs() {

	 REG_PORT_OUTSET0 = RS;
}

 void sam_clearCs() {

	 REG_PORT_OUTCLR0 = CS;
 }
 void sam_setCs() {

	REG_PORT_OUTSET0 = CS;
 }

 void sam_writeComData(uint8_t com, uint16_t data) {

	 sam_writeCom(com);
	 sam_writeData(data >> 8, data );

 }


 void sam_writeToTFT(uint8_t vl) {
	
	
	switch(tft_conf.tft_model){
		
		case ILI9325D_8:
		REG_PORT_OUTCLR1 = PORTBMASK_8;
		break;
		case ILI9325D_16:
		case SSD1963_480:
		case SSD1963_800:
		case SSD1289:
		REG_PORT_OUTCLR1 = PORTBMASK_16;
		break;
		default:
		printf("Device not recognized\n\r");
		break;
	}		
	
	REG_PORT_OUTSET1 = (((vl) & 0xFF));
	
 } 
void sam_write16ToTFT(uint8_t vh, uint8_t vl){
	
	REG_PORT_OUTCLR1 = PORTBMASK_16;
	REG_PORT_OUTSET1 = ( ((vl | (vh) << 8)) & 0xFFFF);
}

void sam_pulseBitLow() {
  
	 REG_PORT_OUT0 &= ~(WR);
	
	 REG_PORT_OUT0 |= WR; // set
}

void sam_writeBus(uint8_t hv, uint8_t lv) {

   switch(tft_conf.tft_model){
	   
	   case ILI9325D_8:
		  sam_writeToTFT(hv);
		  sam_pulseBitLow();
		  sam_writeToTFT(lv);
		  sam_pulseBitLow();
	   break;
	   case ILI9325D_16:
	   case SSD1963_480:
	   case SSD1963_800:
	   case SSD1289:
		   sam_write16ToTFT(hv,lv);
		   sam_pulseBitLow();
	   break;
	   default:
	   printf("Device not recognized\n\r");
	   break;
   }

	
}

void sam_writeCom(uint8_t com) {

	sam_clearRs();// clearBit(RS);
	sam_writeBus(0x00, com);

}

void sam_writeData(uint8_t vh, uint8_t vl) {

	sam_setRs();
	switch(tft_conf.tft_model){
		
		case ILI9325D_8:
		sam_writeBus(vh, vl);
		break;
		case ILI9325D_16:
		case SSD1963_480:
		case SSD1963_800:
		case SSD1289:
		sam_write16ToTFT(vh,vl);
		sam_pulseBitLow();
		break;
		default:
		printf("Device not recognized\n\r");
		break;
	}
}

void sam_writeByteData(uint8_t data){
	
	sam_setRs();
	sam_write16ToTFT(0x00,data);
	sam_pulseBitLow();

}


void sam_setPixel(uint8_t r, uint8_t g, uint8_t b)
{
	sam_writeData(((r & 248) | g >> 5), ((g & 28) << 3 | b >> 3));
}

void sam_clrScr()
{

	int i;

	sam_clearCs();

	sam_clrXY();

	sam_setRs();
	int pixels = (tft_width + 1) * (tft_height+1);
			
	for (i = 0; i < pixels; i++)
	{
		sam_writeBus(0, 0);
	}
	sam_setCs();
	
}

void sam_clrXY()
{
	if (tft_conf.orient == PORTRAIT){
		
		sam_setXY(0, 0, tft_width, tft_height);
	}else{
		sam_setXY(0, 0, tft_height, tft_width);

	}
	
}
void sam_setXY(int x1, int y1, int x2, int y2)
{
	if (tft_conf.orient == LANDSCAPE)
	{
		swap(int, x1, y1);
		swap(int, x2, y2)
		y1 = tft_height - y1;
		y2 = tft_height - y2;
		swap(int, y1, y2)
	}

    if(tft_conf.tft_model == ILI9325D_8 || tft_conf.tft_model == ILI9325D_16 ){
		
		sam_writeComData(0x20, x1);
		sam_writeComData(0x21, y1);
		sam_writeComData(0x50, x1);
		sam_writeComData(0x52, y1);
		sam_writeComData(0x51, x2);
		sam_writeComData(0x53, y2);
		sam_writeCom(0x22);
		
	} else if(tft_conf.tft_model == SSD1963_480){
	
		swap(int, x1, y1);
		swap(int, x2, y2);
		sam_writeCom(0x2a);
		sam_writeByteData(x1>>8);
		sam_writeByteData(x1);
		sam_writeByteData(x2>>8);
		sam_writeByteData(x2);
		sam_writeCom(0x2b);
		sam_writeByteData(y1>>8);
		sam_writeByteData(y1);
		sam_writeByteData(y2>>8);
		sam_writeByteData(y2);
		sam_writeCom(0x2c);

	} else if(tft_conf.tft_model == SSD1963_800){
		swap(int, x1, y1);
		swap(int, x2, y2);
		sam_writeCom(0x2a);
		sam_writeByteData(x1>>8);
		sam_writeByteData(x1);
		sam_writeByteData(x2>>8);
		sam_writeByteData(x2);
		sam_writeCom(0x2b);
		sam_writeByteData(y1>>8);
		sam_writeByteData(y1);
		sam_writeByteData(y2>>8);
		sam_writeByteData(y2);
		sam_writeCom(0x2c);
	
	}	else if( tft_conf.tft_model == SSD1289){
	
		sam_writeComData(0x44,(x2<<8)+x1);
		sam_writeComData(0x45,y1);
		sam_writeComData(0x46,y2);
		sam_writeComData(0x4e,x1);
		sam_writeComData(0x4f,y1);
		sam_writeCom(0x22);

	}	

}



void sam_setColor(uint8_t r, uint8_t g, uint8_t b)
{
	fcolorr = r;
	fcolorg = g;
	fcolorb = b;
}

void sam_setBackColor(uint8_t r, uint8_t g, uint8_t b)
{
	bcolorr = r;
	bcolorg = g;
	bcolorb = b;
}

void sam_setFont(const unsigned char* f, const unsigned int (*d)[3], uint8_t c) {

	fi.font = f;
	fi.descriptors = d;
	fi.num_of_chars = c;
	fi.rows = (int)fi.descriptors[0][1]; // get height for characters 	
}

/* draw functions*/

void sam_drawRect(int x1, int y1, int x2, int y2)
{

	if (x1 > x2)
	{
		swap(int, x1, x2);
	}
	if (y1 > y2)
	{
		swap(int, y1, y2);
	}

	sam_drawHLine(x1, y1, x2 - x1);
	sam_drawHLine(x1, y2, x2 - x1);
	sam_drawVLine(x1, y1, y2 - y1);
	sam_drawVLine(x2, y1, y2 - y1);
}

void sam_fillScr(uint8_t r, uint8_t g, uint8_t b)
{	
	int i;
	char ch, cl;

	ch = ((r & 248) | g >> 5);
	cl = ((g & 28) << 3 | b >> 3);
	sam_clearCs();	
	sam_clrXY();

	int pixels = (tft_width + 1) * (tft_height + 1);
	
	sam_setRs();
	
	for (i = 0; i < (pixels); i++)
	{	  
		sam_writeBus(ch, cl);
	
	
	}
	sam_setCs();
	
	
}

void sam_fillPage(uint8_t r, uint8_t g, uint8_t b, uint8_t page){
	
	int i;
	char ch, cl;

	ch = ((r & 248) | g >> 5);
	cl = ((g & 28) << 3 | b >> 3);
	sam_clearCs();
		
	if(tft_conf.orient == LANDSCAPE){
		
		sam_setXY(0, tft_width * (page), tft_height, tft_width * page); // paging - LANDSCAPE -works
			
	} else {
		
		printf("Portrait Paging not supported yet\n\r");
		
	}
	
	int pixels = ((tft_width + 1) * (tft_height + 1) * page);
	
	sam_setRs();
	for (i = 0; i < (pixels); i++)
	{		
		sam_writeBus(ch, cl);
	}
	sam_setCs();// setBit(CS);	
}

void sam_drawPixel(int x, int y)
{
	sam_clearCs();//clearBit(CS);
	sam_setXY(x, y, x, y);
	sam_setPixel(fcolorr, fcolorg, fcolorb);
	sam_setCs();//setBit(CS);
	sam_clrXY();
}

void sam_drawCircle(int x, int y, int radius)
{
	int f = 1 - radius;
	int ddF_x = 1;
	int ddF_y = -2 * radius;
	int x1 = 0;
	int y1 = radius;
	char ch, cl;

	ch = ((fcolorr & 248) | fcolorg >> 5);
	cl = ((fcolorg & 28) << 3 | fcolorb >> 3);

	sam_clearCs();//clearBit(CS);
	sam_setXY(x, y + radius, x, y + radius);
	sam_writeData(ch, cl);
	sam_setXY(x, y - radius, x, y - radius);
	sam_writeData(ch, cl);
	sam_setXY(x + radius, y, x + radius, y);
	sam_writeData(ch, cl);
	sam_setXY(x - radius, y, x - radius, y);
	sam_writeData(ch, cl);

	while (x1 < y1)
	{
		if (f >= 0)
		{
			y1--;
			ddF_y += 2;
			f += ddF_y;
		}
		x1++;
		ddF_x += 2;
		f += ddF_x;
		sam_setXY(x + x1, y + y1, x + x1, y + y1);
		sam_writeData(ch, cl);
		sam_setXY(x - x1, y + y1, x - x1, y + y1);
		sam_writeData(ch, cl);
		sam_setXY(x + x1, y - y1, x + x1, y - y1);
		sam_writeData(ch, cl);
		sam_setXY(x - x1, y - y1, x - x1, y - y1);
		sam_writeData(ch, cl);
		sam_setXY(x + y1, y + x1, x + y1, y + x1);
		sam_writeData(ch, cl);
		sam_setXY(x - y1, y + x1, x - y1, y + x1);
		sam_writeData(ch, cl);
		sam_setXY(x + y1, y - x1, x + y1, y - x1);
		sam_writeData(ch, cl);
		sam_setXY(x - y1, y - x1, x - y1, y - x1);
		sam_writeData(ch, cl);
	}
	sam_setCs();//setBit(CS);
	sam_clrXY();
}

void sam_drawRoundRect(int x1, int y1, int x2, int y2)
{

	if (x1 > x2)
	{
		swap(int, x1, x2);
	}
	if (y1 > y2)
	{
		swap(int, y1, y2);
	}
	if ((x2 - x1) > 4 && (y2 - y1) > 4)
	{
		sam_drawPixel(x1 + 1, y1 + 1);
		sam_drawPixel(x2 - 1, y1 + 1);
		sam_drawPixel(x1 + 1, y2 - 1);
		sam_drawPixel(x2 - 1, y2 - 1);
		sam_drawHLine(x1 + 2, y1, x2 - x1 - 4);
		sam_drawHLine(x1 + 2, y2, x2 - x1 - 4);
		sam_drawVLine(x1, y1 + 2, y2 - y1 - 4);
		sam_drawVLine(x2, y1 + 2, y2 - y1 - 4);
	}
}

void sam_fillRect(int x1, int y1, int x2, int y2)
{

	if (x1 > x2)
	{
		swap(int, x1, x2);
	}
	if (y1 > y2)
	{
		swap(int, y1, y2);
	}

	if (tft_conf.orient == PORTRAIT)
	{
		for (int i = 0; i < ((y2 - y1) / 2) + 1; i++)
		{
			sam_drawHLine(x1, y1 + i, x2 - x1);
			sam_drawHLine(x1, y2 - i, x2 - x1);
		}
	}
	else
	{
		for (int i = 0; i < ((x2 - x1) / 2) + 1; i++)
		{
			sam_drawVLine(x1 + i, y1, y2 - y1);
			sam_drawVLine(x2 - i, y1, y2 - y1);
		}
	}
}

void sam_fillRoundRect(int x1, int y1, int x2, int y2)
{
	if (x1 > x2)
	{
		swap(int, x1, x2);
	}
	if (y1 > y2)
	{
		swap(int, y1, y2);
	}

	if ((x2 - x1) > 4 && (y2 - y1) > 4)
	{
		for (int i = 0; i < ((y2 - y1) / 2) + 1; i++)
		{
			switch (i)
			{
				case 0:
				sam_drawHLine(x1 + 2, y1 + i, x2 - x1 - 4);
				sam_drawHLine(x1 + 2, y2 - i, x2 - x1 - 4);
				break;
				case 1:
				sam_drawHLine(x1 + 1, y1 + i, x2 - x1 - 2);
				sam_drawHLine(x1 + 1, y2 - i, x2 - x1 - 2);
				break;
				default:
				sam_drawHLine(x1, y1 + i, x2 - x1);
				sam_drawHLine(x1, y2 - i, x2 - x1);
			}
		}
	}
}

void sam_fillCircle(int x, int y, int radius)
{
	sam_clearCs();//clearBit(CS);
	for (int y1 = -radius; y1 <= radius; y1++)
		for (int x1 = -radius; x1 <= radius; x1++)
			if (x1 * x1 + y1 * y1 <= radius * radius)
			{
				sam_setXY(x + x1, y + y1, x + x1, y + y1);
				sam_writeData(((fcolorr & 248) | fcolorg >> 5), ((fcolorg & 28) << 3 | fcolorb >> 3));
			}
	sam_setCs();// setBit(CS);
	sam_clrXY();
}

void sam_drawLine(int x1, int y1, int x2, int y2)
{
	double delta, tx, ty;

	char ch, cl;

	ch = ((fcolorr & 248) | fcolorg >> 5);
	cl = ((fcolorg & 28) << 3 | fcolorb >> 3);

	if (((x2 - x1) < 0))
	{
		swap(int, x1, x2);
		swap(int, y1, y2);
	}
	if (((y2 - y1) < 0))
	{
		swap(int, x1, x2);
		swap(int, y1, y2);
	}

	if (y1 == y2)
	{
		if (x1 > x2)
		{
			swap(int, x1, x2);
		}
		sam_drawHLine(x1, y1, x2 - x1);
	}
	else if (x1 == x2)
	{
		if (y1 > y2)
		{
			swap(int, y1, y2);
		}
		sam_drawVLine(x1, y1, y2 - y1);
	}
	else if (abs(x2 - x1) > abs(y2 - y1))
	{
		sam_clearCs();//clearBit(CS);
		delta = ( (double)(y2 - y1) / (double)(x2 - x1));
		ty = (double)y1;
		if (x1 > x2)
		{
			for (int i = x1; i >= x2; i--)
			{
				sam_setXY(i, (int)(ty + 0.5), i, (int)(ty + 0.5));
				sam_writeData(ch, cl);
				ty = ty - delta;
			}
		}
		else
		{
			for (int i = x1; i <= x2; i++)
			{
				sam_setXY(i, (int)(ty + 0.5), i, (int)(ty + 0.5));
				sam_writeData(ch, cl);
				ty = ty + delta;
			}
		}
		sam_setCs();//setBit(CS);
	}
	else
	{
		sam_clearCs();//clearBit(CS);
		delta = ((float)(x2 - x1) / (float)(y2 - y1));
		tx = (float)x1;
		if (y1 > y2)
		{
			for (int i = y2 + 1; i > y1; i--)
			{
				sam_setXY((int)(tx + 0.5), i, (int)(tx + 0.5), i);
				sam_writeData(ch, cl);
				tx = tx + delta;
			}
		}
		else
		{
			for (int i = y1; i < y2 + 1; i++)
			{
				sam_setXY((int)(tx + 0.5), i, (int)(tx + 0.5), i);
				sam_writeData(ch, cl);
				tx = tx + delta;
			}
		}
		sam_setCs();// setBit(CS);
	}
	sam_clrXY();
}

void sam_drawHLine(int x, int y, int l)
{
	char ch, cl;

	ch = ((fcolorr & 248) | fcolorg >> 5);
	cl = ((fcolorg & 28) << 3 | fcolorb >> 3);

	sam_clearCs();//clearBit(CS);
	sam_setXY(x, y, x + l, y);
	for (int i = 0; i < l + 1; i++)
	{
		sam_writeData(ch, cl);
	}
	sam_setCs();//setBit(CS);
	sam_clrXY();
}

void sam_drawVLine(int x, int y, int l)
{
	char ch, cl;

	ch = ((fcolorr & 248) | fcolorg >> 5);
	cl = ((fcolorg & 28) << 3 | fcolorb >> 3);

	sam_clearCs();//clearBit(CS);
	sam_setXY(x, y, x, y + l);
	for (int i = 0; i < l; i++)
	{
		sam_writeData(ch, cl);
	}
	sam_setCs();//setBit(CS);
	sam_clrXY();
}

void sam_setCharMap(const unsigned char* cm) {
	cMap = cm;
}

uint8_t sam_getIndex(char c) {

	for (int i = 0; i < 96; i++) {

		if (c == (int)cMap[i]) {

			return i;
		}

	}
	return 0;
}

void sam_print(const char *st, int x, int y) {
	int stl, i;
	stl = strlen(st);
	current_x = x;
	current_y = y;

	for (i = 0; i < stl; i++) {

		sam_printChar(*st++, x, y);
		current_x = current_x + ci.width;
		current_y = y;
	}

}

void sam_printChar(char ch, int x, int y) {
	
	//printf("Char: %c\n\r", ch);
	
	sam_clearCs();
	uint8_t bitCt = 8;// default
	uint8_t indx;

	indx = sam_getIndex(ch); // get the index from the char map
	
	ci.width = (int)(fi.descriptors[indx][0]);
	ci.offset = (int)(fi.descriptors[indx][2]);
	
	if (ci.width < 9) {
		bitCt = 8;
	} else if (ci.width < 17) {
		bitCt = 16;
	} else if (ci.width < 25) {
		bitCt = 24;
	} else if (ci.width < 33) {
		bitCt = 32;
	} else if (ci.width < 41) {
		bitCt = 40;
	} else if (ci.width < 49) {
		bitCt = 48;
	} else if (ci.width < 57) {
		bitCt = 56;

	} else if (ci.width < 65) {
		bitCt = 64;
	}

	int nextIndx = 0;

	if (indx + 1 >= fi.num_of_chars) { // array size 95

		nextIndx = ci.offset + (int)fi.descriptors[1][2] - (int)fi.descriptors[0][2];

	} else {

		nextIndx = (int)(fi.descriptors[indx + 1][2]);

	}	

	for (int i = ci.offset; i < nextIndx; i = i + bitCt / 8) {

		if (bitCt  == 16) {

			if (tft_conf.orient == PORTRAIT) {

				sam_setXY(current_x, y, current_x + ci.width, y);

				uint8_t byte1 = (uint8_t)fi.font[i];
				uint8_t byte2 = (uint8_t)fi.font[i + 1];

				uint32_t chr = (byte2 << 8) + byte1 ;

				for (int ii = 0; ii < ci.width ; ii++) {

					if ((chr >> ii) % 2 != 0) {

						sam_setPixel(fcolorr, fcolorg, fcolorb);

						} else {
						sam_setPixel(bcolorr, bcolorg, bcolorb);
					}					
				}		
				

			} else { // LANDSCAPE
				
				sam_setXY(current_x, y, current_x + ci.width, y);
				
				uint8_t byte1 = (uint8_t)fi.font[i];
				uint8_t byte2 = (uint8_t)fi.font[i + 1];
				
				uint32_t chr = (byte2 << 8) + byte1 ;
				
				for (int ii = ci.width; ii > -1; ii--) {

					if ((chr >> ii) % 2 != 0) {
						//  printf("X");
						sam_setPixel(fcolorr, fcolorg, fcolorb);

					} else {

						//printf("0");
						sam_setPixel(bcolorr, bcolorg, bcolorb);
						
						
					}

				}
				
			}



		} else {

			if (tft_conf.orient == PORTRAIT) {

				sam_setXY(current_x, y, current_x + ci.width, y);

				for (int r = 0; r < bitCt / 8; r++) {

					uint32_t chr = (uint8_t)fi.font[i + r];
					
					for (int ii = 7; ii > -1; ii--) {

						if ((chr & (1 << (7 - ii))) != 0) {

							sam_setPixel(fcolorr, fcolorg, fcolorb);
							

						} else {

							sam_setPixel(bcolorr, bcolorg, bcolorb);
							
						}

					}
					
				}
				
				} else {
				// printf("2-L\n\r");
				sam_setXY(current_x, y, current_x + ci.width, y);
				
				for (int r = bitCt / 8 - 1; r > -1; r--) {

					uint32_t chr = (uint8_t)fi.font[i + r];

					for (int ii = 0; ii < 8; ii++) {

						if ((chr & (1 << (7 - ii))) != 0) {

							sam_setPixel(fcolorr, fcolorg, fcolorb);


							} else {

							sam_setPixel(bcolorr, bcolorg, bcolorb);

						}

					}

				}

			}



		}

		y++;
	}

	sam_setCs();//digitalWrite(CS, HIGH);
	sam_clrXY();

}


void sam_setTFTProperties() {

	switch(tft_conf.tft_model){
		
		case ILI9325D_8:
		case ILI9325D_16:
			sam_writeComData(0xE5, 0x78F0);
			sam_writeComData(0x01, 0x0100);
			sam_writeComData(0x02, 0x0200); // set 1 line inversion
			sam_writeComData(0x03, 0x1030); // set GRAM write direction and BGR=1.
			sam_writeComData(0x04, 0x0000); // Resize register
			sam_writeComData(0x08, 0x0207); // set the back porch and front porch
			sam_writeComData(0x09, 0x0000); // set non-display area refresh cycle ISC[3:0]
			sam_writeComData(0x0A, 0x0000); // FMARK function
			sam_writeComData(0x0C, 0x0000); // RGB interface setting
			sam_writeComData(0x0D, 0x0000); // Frame marker Position
			sam_writeComData(0x0F, 0x0000); // RGB interface polarity
			//*************Power On sequence ****************//
			sam_writeComData(0x10, 0x0000); // SAP, BT[3:0], AP, DSTB, SLP, STB
			sam_writeComData(0x11, 0x0007); // DC1[2:0], DC0[2:0], VC[2:0]
			sam_writeComData(0x12, 0x0000); // VREG1OUT voltage
			sam_writeComData(0x13, 0x0000); // VDV[4:0] for VCOM amplitude
			sam_writeComData(0x07, 0x0001);
			// delay(200); // Dis-charge capacitor power voltage
			sam_writeComData(0x10, 0x1690); // SAP, BT[3:0], AP, DSTB, SLP, STB
			sam_writeComData(0x11, 0x0227); // Set DC1[2:0], DC0[2:0], VC[2:0]
			// delay(50); // Delay 50ms
			sam_writeComData(0x12, 0x000D); // 0012
			//  delay(50); // Delay 50ms
			sam_writeComData(0x13, 0x1200); // VDV[4:0] for VCOM amplitude
			sam_writeComData(0x29, 0x000A); // 04  VCM[5:0] for VCOMH
			sam_writeComData(0x2B, 0x000D); // Set Frame Rate
			// delay(50); // Delay 50ms
			sam_writeComData(0x20, 0x0000); // GRAM horizontal Address
			sam_writeComData(0x21, 0x0000); // GRAM Vertical Address
			// ----------- Adjust the Gamma Curve ----------//
			sam_writeComData(0x30, 0x0000);
			sam_writeComData(0x31, 0x0404);
			sam_writeComData(0x32, 0x0003);
			sam_writeComData(0x35, 0x0405);
			sam_writeComData(0x36, 0x0808);
			sam_writeComData(0x37, 0x0407);
			sam_writeComData(0x38, 0x0303);
			sam_writeComData(0x39, 0x0707);
			sam_writeComData(0x3C, 0x0504);
			sam_writeComData(0x3D, 0x0808);
			//------------------ Set GRAM area ---------------//
			sam_writeComData(0x50, 0x0000); // Horizontal GRAM Start Address
			sam_writeComData(0x51, 0x00EF); // Horizontal GRAM End Address
			sam_writeComData(0x52, 0x0000); // Vertical GRAM Start Address
			sam_writeComData(0x53, 0x013F); // Vertical GRAM Start Address
			sam_writeComData(0x60, 0xA700); // Gate Scan Line
			sam_writeComData(0x61, 0x0001); // NDL,VLE, REV
			sam_writeComData(0x6A, 0x0000); // set scrolling line
			//-------------- Partial Display Control ---------//
			sam_writeComData(0x80, 0x0000);
			sam_writeComData(0x81, 0x0000);
			sam_writeComData(0x82, 0x0000);
			sam_writeComData(0x83, 0x0000);
			sam_writeComData(0x84, 0x0000);
			sam_writeComData(0x85, 0x0000);
			//-------------- Panel Control -------------------//
			sam_writeComData(0x90, 0x0010);
			sam_writeComData(0x92, 0x0000);
			sam_writeComData(0x07, 0x0133); // 262K color and display ON

		break;
		case SSD1963_480:						
			
			sam_writeCom(0xE2);				//PLL multiplier, set PLL clock to 120M
			sam_writeByteData(0x23);	    //N=0x36 for 6.5M, 0x23 for 10M crystal
			sam_writeByteData(0x02);
			sam_writeByteData(0x54);
			sam_writeCom(0xE0);				// PLL enable
			sam_writeByteData(0x01);
			delay_ms(10);
			sam_writeCom(0xE0);
			sam_writeByteData(0x03);
			delay_ms(10);
			sam_writeCom(0x01);				// software reset
			delay_ms(100);
			sam_writeCom(0xE6);				//PLL setting for PCLK, depends on resolution
			sam_writeByteData(0x01);
			sam_writeByteData(0x1F);
			sam_writeByteData(0xFF);

			sam_writeCom(0xB0);				//LCD SPECIFICATION
			sam_writeByteData(0x20);
			sam_writeByteData(0x00);
			sam_writeByteData(0x01);		//Set HDP	479
			sam_writeByteData(0xDF);
			sam_writeByteData(0x01);		//Set VDP	271
			sam_writeByteData(0x0F);
			sam_writeByteData(0x00);

			sam_writeCom(0xB4);				//HSYNC
			sam_writeByteData(0x02);		//Set HT	531
			sam_writeByteData(0x13);
			sam_writeByteData(0x00);		//Set HPS	8
			sam_writeByteData(0x08);
			sam_writeByteData(0x2B);		//Set HPW	43
			sam_writeByteData(0x00);		//Set LPS	2
			sam_writeByteData(0x02);
			sam_writeByteData(0x00);

			sam_writeCom(0xB6);				//VSYNC
			sam_writeByteData(0x01);		//Set VT	288
			sam_writeByteData(0x20);
			sam_writeByteData(0x00);		//Set VPS	4
			sam_writeByteData(0x04);
			sam_writeByteData(0x0c);		//Set VPW	12
			sam_writeByteData(0x00);		//Set FPS	2
			sam_writeByteData(0x02);

			sam_writeCom(0xBA);
			sam_writeByteData(0x0F);		//GPIO[3:0] out 1

			sam_writeCom(0xB8);
			sam_writeByteData(0x07);	    //GPIO3=input, GPIO[2:0]=output
			sam_writeByteData(0x01);		//GPIO0 normal

			sam_writeCom(0x36);				//rotation
			sam_writeByteData(0x22);

			sam_writeCom(0xF0);				//pixel data interface
			sam_writeByteData(0x03);

			sam_writeCom(0x3A);
			sam_writeByteData(0x05);		// Set Pixel Format 

			delay_ms(1);

			sam_setXY(0, 0, 479, 271);
			sam_writeCom(0x29);		//display on

			sam_writeCom(0xBE);		//set PWM for B/L
			sam_writeByteData(0x06);
			sam_writeByteData(0xf0);
			sam_writeByteData(0x01);
			sam_writeByteData(0xf0);
			sam_writeByteData(0x00);
			sam_writeByteData(0x00);

			sam_writeCom(0xd0);
			sam_writeByteData(0x0d);

			sam_writeCom(0x2C);
		
		break;
		case SSD1963_800:
		sam_writeCom(0xE2);				//PLL multiplier, set PLL clock to 120M
		sam_writeByteData(0x23);	    //N=0x36 for 6.5M, 0x23 for 10M crystal
		sam_writeByteData(0x02);
		sam_writeByteData(0x04);
		sam_writeCom(0xE0);				// PLL enable
		sam_writeByteData(0x01);
		delay_ms(10);
		sam_writeCom(0xE0);
		sam_writeByteData(0x03);
		delay_ms(10);
		sam_writeCom(0x01);				// software reset
		delay_ms(100);
		sam_writeCom(0xE6);				//PLL setting for PCLK, depends on resolution
		sam_writeByteData(0x04);
		sam_writeByteData(0x93);
		sam_writeByteData(0xE0);

		sam_writeCom(0xB0);				//LCD SPECIFICATION
		sam_writeByteData(0x19);
		sam_writeByteData(0x20);
		sam_writeByteData(0x03);		//Set HDP	799
		sam_writeByteData(0x21);
		sam_writeByteData(0x01);		//Set VDP	479
		sam_writeByteData(0xE1);
		sam_writeByteData(0x00);

		sam_writeCom(0xB4);				//HSYNC
		sam_writeByteData(0x03);		//Set HT	928
		sam_writeByteData(0xA0);
		sam_writeByteData(0x00);		//Set HPS	46
		sam_writeByteData(0x2E);
		sam_writeByteData(0x30);		//Set HPW	48
		sam_writeByteData(0x00);		//Set LPS	15
		sam_writeByteData(0x0F);
		sam_writeByteData(0x00);

		sam_writeCom(0xB6);				//VSYNC
		sam_writeByteData(0x02);		//Set VT	525
		sam_writeByteData(0x0D);
		sam_writeByteData(0x00);		//Set VPS	16
		sam_writeByteData(0x10);
		sam_writeByteData(0x10);		//Set VPW	16
		sam_writeByteData(0x00);		//Set FPS	8
		sam_writeByteData(0x08);

		sam_writeCom(0xBA);
		sam_writeByteData(0x05);		//GPIO[3:0] out 1

		sam_writeCom(0xB8);
		sam_writeByteData(0x07);	    //GPIO3=input, GPIO[2:0]=output
		sam_writeByteData(0x01);		//GPIO0 normal

		sam_writeCom(0x36);				//rotation
		sam_writeByteData(0x21);		// use 0x21 or 0x22

		sam_writeCom(0xF0);				//pixel data interface
		sam_writeByteData(0x03); 
		
		delay_ms(1);

		sam_setXY(0, 0, 799, 479);
		sam_writeCom(0x29);		//display on

		sam_writeCom(0xBE);		//set PWM for B/L
		sam_writeByteData(0x06);
		sam_writeByteData(0xf0);
		sam_writeByteData(0x01);
		sam_writeByteData(0xf0);
		sam_writeByteData(0x00);
		sam_writeByteData(0x00);

		sam_writeCom(0xd0);
		sam_writeByteData(0x0d);

		sam_writeCom(0x2C);
		break;
		case SSD1289:
			sam_writeComData(0x00,0x0001); //Oscillation Start
			sam_writeComData(0x03,0xA8A4); //Power control (1)
			sam_writeComData(0x0C,0x0000); //Power control (2)
			sam_writeComData(0x0D,0x080C); //Power control (3)
			sam_writeComData(0x0E,0x2B00); //Power control (4)
			sam_writeComData(0x1E,0x00B7); //Power control (5)
			sam_writeComData(0x01,0x2B3F); //Driver output control
			sam_writeComData(0x02,0x0600); //LCD drive AC control
			sam_writeComData(0x10,0x0000); //Sleep mode
			sam_writeComData(0x11,0x6070); //Entry mode
			sam_writeComData(0x05,0x0000); //Compare register(1)
			sam_writeComData(0x06,0x0000); //Compare register(2)
			sam_writeComData(0x16,0xEF1C); //Horizontal Porch
			sam_writeComData(0x17,0x0003); //Vertical Porch
			sam_writeComData(0x07,0x0633); //Display control was 0x233
			sam_writeComData(0x0B,0x0000); //Frame cycle control
			sam_writeComData(0x0F,0x0000); //Gate scan start position
			sam_writeComData(0x41,0x0000); //Vertical scroll control (1)
			sam_writeComData(0x42,0x0000); //Vertical scroll control (2)
			sam_writeComData(0x48,0x0000); //First window start
			sam_writeComData(0x49,0x013F); //First window end 
			sam_writeComData(0x4A,0x0000); //Second window start
			sam_writeComData(0x4B,0x0000); //Second window end
			sam_writeComData(0x44,0xEF00); // Horizontal RAM address position
			sam_writeComData(0x45,0x0000); // Vertical RAM address start position
			sam_writeComData(0x46,0x013F); // Vertical RAM address end position
			sam_writeComData(0x30,0x0707); // ? control (1)
			sam_writeComData(0x31,0x0204); // ? control (2)
			sam_writeComData(0x32,0x0204);
			sam_writeComData(0x33,0x0502);
			sam_writeComData(0x34,0x0507);
			sam_writeComData(0x35,0x0204);
			sam_writeComData(0x36,0x0204);
			sam_writeComData(0x37,0x0502);
			sam_writeComData(0x3A,0x0302);
			sam_writeComData(0x3B,0x0302); //? control (10)
			sam_writeComData(0x23,0x0000); // RAM write data mask (1)
			sam_writeComData(0x24,0x0000); // RAM write data mask (2)
			//sam_writeComData(0x25,0x8000); //?
			sam_writeComData(0x4f,0x0000); // Set GDDRAM Y address counter
			sam_writeComData(0x4e,0x0000); // Set GDDRAM X address counter
			sam_writeCom(0x22);
		break;
		default:
		printf("TFT not Supported");
		break;
		
	}
	
	sam_setCs();//setBit(CS);

	sam_setColor(255, 255, 255);
	sam_setBackColor(0, 0, 0);
	//cfont.font = 0;

}

/* 
Page 1 = Lines 0-271
Page 2 = Lines 272 - 543
Page 3 = Lines 544 - 815
*/
void sam_scroll(int y){
	
	int16_t pix = y;	
	sam_clearCs();//	cbi(P_CS, B_CS);
		
	if(tft_conf.tft_model == ILI9325D_8 || tft_conf.tft_model == ILI9325D_16){
	
		sam_writeComData(0x61,0x0003);
		sam_writeComData(0x6A, pix);
	
	}else if(tft_conf.tft_model == SSD1963_480 || tft_conf.tft_model == SSD1963_800){
			 
		sam_writeCom(0x37);
		sam_writeByteData(pix>>8);
		sam_writeByteData(pix);
		 

	} else if (tft_conf.tft_model == SSD1289){
			
		sam_writeComData(0x41, y); // screen 1 scrolls		
		sam_writeComData(0x42, y);// screen 2 scroll
		
	}	else {
		
		printf("Scroll not supported \n\r");
		
	}
	
	sam_setCs();// sbi(P_CS, B_CS);
	
}

void sam_setScrollArea(uint16_t top, uint16_t scr, uint16_t bottom){
	
	sam_clearCs();
	
	if(tft_conf.tft_model == SSD1963_480 || tft_conf.tft_model == SSD1963_800){

		sam_writeCom(0x33);
		sam_writeByteData(top>>8);
		sam_writeByteData(top);
		sam_writeByteData(scr>>8);
		sam_writeByteData(scr);
		sam_writeByteData(bottom>>8);
		sam_writeByteData(bottom);

	} else if (tft_conf.tft_model == SSD1289){
	   
	    if(bottom < 0){
			bottom = 0;
		}
		if(top > 320){
			
			top = 320;
		}

		sam_writeComData(0x48,bottom); //First window start
		sam_writeComData(0x49,top); //First window end

		sam_writeComData(0x4A,bottom); //Second window start
		sam_writeComData(0x4B,top); //Second window end

		sam_writeComData(0x45,(uint16_t)bottom); // Vertical RAM address start position
		sam_writeComData(0x46,(uint16_t)top); // Vertical RAM address end position

	} else {

		printf("TFT Scroll not support on this model\n");
	}
	
	
	
	sam_setCs();
}

void sam_setPage(int pageNumber){
	
	sam_scroll(pageNumber);	
	
}

// not working for ssd1289 - load from sd or usb instead
void sam_load_raw_image_mem(int x1, int y1, int imgX, int imgY, const uint16_t *img, int size)  {
	
	sam_clearCs();	
	sam_setXY(x1, y1,(imgX + x1) -1, (imgY + y1)-1);
	
	for (int y = 0; y < size; y++) {
	  	 
		 sam_writeData((img[y] >> 8), img[y]);	
	}

	sam_setCs();
	sam_clrXY();

}

void sam_test(int pos_x, int pos_y, int dim_x, int dim_y, const uint16_t *data){
	
	int x, y;
	
	sam_clearCs();
	//uint16_t bt;
	
	sam_setXY(pos_x, pos_y, pos_x + dim_x - 1, pos_y + dim_y - 1);

	for(y = pos_y; y < (pos_y + dim_y); y++ ) {

		for(x = pos_x; x < (pos_x + dim_x); x++ ) {	
			
			//bt = *data++;
			
			sam_writeByteData(*data++ );
			
		}		

	}
	
	sam_setCs();
	sam_clrXY();
}

void sam_printf(uint8_t x, uint8_t y, const char* fmt, ... ){

	va_list arglist;
	char dest[MAXSTRINGSIZE];
	va_start( arglist, fmt );
	vsprintf(dest, fmt, arglist);
	va_end( arglist );
	char buff[MAXSTRINGSIZE];
	snprintf(buff, sizeof(buff), dest, arglist);
	sam_print(buff,x,y);
}


