/*
 * max7219_lib.c
 *
 * Created: 9/9/2020 6:49:15 PM
 *  Author: pvallone
 */ 
#include "max7219_lib.h"

uint8_t max7219_font[91] = {
	0x00, // ' '
	0x01, // '!'
	0x01, // '"'
	0x01, // '#'
	0x01, // '$'
	0x01, // '%'
	0x01, // '&'
	0x01, // '''
	0x4E, // '('
	0x78, // ')'
	0x01, // '*'
	0x01, // '+'
	0x01, // ','
	0x01, // '-'	13
	0x80, // '.' 14
	0x01, // '/'	16
	0x7E, // '0'	17
	0x30, // '1'	18
	0x6D, // '2'
	0x79, // '3'
	0x33, // '4' 20
	0x5B, // '5'
	0x5F, // '6'
	0x70, // '7'
	0x7F, // '8'
	0x7B, // '9'
	0x01, // ':'
	0x01, // ';'
	0x01, // '<'
	0x01, // '='
	0x01, // '>'
	0x01, // '?'
	0x01, // '@'
	0x77, // 'A'
	0x1F, // 'B'
	0x4E, // 'C'
	0x3D, // 'D'
	0X4F, // 'E'
	0x47, // 'F'
	0x5E, // 'G'
	0x37, // 'H'
	0x30, // 'I'
	0x3C, // 'J'
	0x01, // 'K'
	0x0E, // 'L'
	0x01, // 'M'
	0x15, // 'N'
	0x7E, // 'O'
	0x67, // 'P'
	0x01, // 'Q'
	0x05, // 'R'
	0x5B, // 'S'
	0x07, // 'T'
	0x3E, // 'U'
	0x01, // 'V'
	0x01, // 'W'
	0x01, // 'X'
	0x27, // 'Y'
	0x01, // 'Z'
	0x4E, // '['
	0x01, // backslash
	0x78, // ']'
	0x01, // '^'
	0x08, // '_'
	0x01, // '`'
	0x77, // 'a'
	0x1F, // 'b'
	0x0D, // 'c'
	0x3D, // 'd'
	0x4F, // 'e'
	0x47, // 'f'
	0x5E, // 'g'
	0x17, // 'h'
	0x10, // 'i'
	0x3C, // 'j'
	0x01, // 'k'
	0x0E, // 'l'
	0x01, // 'm'
	0x15, // 'n'
	0x7E, // 'o'
	0x67, // 'p'
	0x01, // 'q'
	0x05, // 'r'
	0x5B, // 's'
	0x07, // 't'
	0x1C, // 'u'
	0x01, // 'v'
	0x01, // 'w'
	0x01, // 'x'
	0x27, // 'y'
	0x01, // 'z'
};
/*
	Init Registers to be used
*/
void max7219_init( uint64_t cs, uint64_t clk, uint64_t din, uint8_t numberOfChips){	
		
	REG_PORT_DIRSET1 =  cs | clk | din;
	REG_PORT_OUTSET1 =  cs | clk | din;
	REG_PORT_OUTCLR1 =  cs | clk | din;
	
	max_cs = cs;
	max_din = din;
	max_clk = clk;	
	chips = numberOfChips;
}

void max7219_chip_select(uint8_t direction){	
	if(direction == 0){
		REG_PORT_OUT1 &= ~(max_cs);			// set low
	} else {
		REG_PORT_OUT1 |= max_cs;			// set	high
	}
}

void max7219_reset_chip(void){
	for(int i = 1; i < chips + 1;i++){
		max7219_set_register(REG_SHUTDOWN, 0, i);
	}	
}
void max7219_set_register(uint8_t reg, uint8_t data, uint8_t pos){
	
	uint16_t dt = (max7219_reverse(data) << 8) | max7219_reverse(reg) ;
	uint8_t nopCount = pos - 1;
	
	max7219_chip_select(LOW); // sending data
	delay_us(1);
	DIN_LOW;
	
	for(uint8_t i = 0; i < 16; i++){
		CLKLOW;
		if(((dt >> i) & 1) == 0){
			REG_PORT_OUT1 &= ~(max_din);
			} else {
			REG_PORT_OUT1 |= max_din; // set
		}
		CLKHI;
	}
	
	// end with enough NOPs so later chips don't update
	for (int i = 0; i < chips - nopCount - 1; i++){
		max7219_send_byte (REG_NOOP, REG_NOOP);
	}
	DIN_HIGH;
	max7219_chip_select(HI); // done sending data
	
}

void max7219_print_str(const char*s){
	
	uint8_t pos = 0;
	while (*s != '\0') {

		if ((int)s[1] == 46) {		// .
			uint8_t value  =  (max7219_font[(int)*s - 32] | (1 << 7));
			max7219_update_digit (pos, value);
			s++;
		}
		else {
			max7219_update_digit (pos, max7219_font[ (int)*s - 32]);
		}
		s++;
		pos++;
	}
}
void max7219_update_digit(uint8_t pos, uint8_t value){
	
	uint8_t segment = abs(8 - (pos % 8));
	uint8_t nopCount = pos / 8;
	
	max7219_chip_select(LOW); // sending data
	// for each daisy-chained display we need an extra NOP
	for (uint8_t i = 0; i < nopCount; i++){
		max7219_send_byte (REG_NOOP, REG_NOOP);
	}
	// send the segment number and data
	max7219_send_byte (segment, value);
	
	// end with enough NOPs so later chips don't update
	for (int i = 0; i < chips - nopCount - 1; i++){
		max7219_send_byte (REG_NOOP, REG_NOOP);
	}
	max7219_chip_select(HIGH); // done sending data
}



void max7219_send_byte(uint8_t reg, uint8_t data){
	
	uint16_t dt = (max7219_reverse(data) << 8) | max7219_reverse(reg) ;
	for(uint8_t i = 0; i < 16; i++){
		CLKLOW;
		if(((dt >> i) & 1) == 0){
			REG_PORT_OUT1 &= ~(max_din);
		} else {
			REG_PORT_OUT1 |= max_din; // set
		}
		CLKHI;
	}
}
void max7219_set_mode(uint8_t bcd){
	
	max7219_chip_select(HI);		// set to high
	DIN_HIGH;
	CLKLOW;
	uint8_t bcdVal = bcd == 1 ? 0xFF : 0x00;
	for(uint8_t i = 1; i < chips + 1;i++){
		max7219_set_register(REG_SCANLIMIT, 7, i);
		max7219_set_register(REG_DECODE_MODE, bcdVal, i);
		max7219_set_register(REG_DISPLAY_TEST, 0, i);
		max7219_set_register(REG_INTENSITY, 15, i);
		max7219_set_register(REG_SHUTDOWN, 1, i);
	}
	
}
void max7219_clear_displays(uint8_t value){
	for(uint8_t x = 0; x < chips * 8; x++){
		max7219_update_digit(x, value);
	}
}

uint8_t max7219_reverse(uint8_t b) {
	b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
	b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
	b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
	return b;
}

void max7219_run_text_test(void){
	
	max7219_update_digit(24, max7219_font[33]);
	max7219_update_digit(25, max7219_font[34]);
	max7219_update_digit(26, max7219_font[35]);
	max7219_update_digit(27, max7219_font[36]);
	max7219_update_digit(28, max7219_font[37]);
	max7219_update_digit(29, max7219_font[38]);
	max7219_update_digit(30, max7219_font[39]);
	max7219_update_digit(31, max7219_font[40]);
	
	max7219_update_digit(16, DASH);
	max7219_update_digit(17, DASH);
	max7219_update_digit(18, LETP);
	max7219_update_digit(19, LETH);
	max7219_update_digit(20, LETI);
	max7219_update_digit(21, LETL);
	max7219_update_digit(22, DASH);
	max7219_update_digit(23, DASH);
	
	max7219_update_digit(15, BLANK);
	max7219_update_digit(14, NUM0);
	max7219_update_digit(13, NUM2);
	max7219_update_digit(12, NUM0);
	max7219_update_digit(11, NUM2);
	max7219_update_digit(10, LETS);
	max7219_update_digit(9, LETF);
	max7219_update_digit(8, BLANK);
	
	max7219_update_digit(7, DASH);
	max7219_update_digit(6, DASH);
	max7219_update_digit(5, DASH);
	max7219_update_digit(4, DASH);
	max7219_update_digit(3, DASH);
	max7219_update_digit(2, DASH);
	max7219_update_digit(1, DASH);
	max7219_update_digit(0, DASH);	
	
}

void max7219_run_bcd_test(void){
	
	int numberOfDigits = chips * 8; // total segments
	
	delay_ms(2000);
	
	for(int y = 1; y < 10; y++){
		for(int pos = 0; pos < numberOfDigits;pos++){
			
			uint8_t segment = abs(8 - (pos % 8));
			// for each daisy-chained display we need an extra NOP
			uint8_t nopCount = pos / 8;
			
			max7219_chip_select(LOW); // sending data
			for (uint8_t i = 0; i < nopCount; i++){
				max7219_send_byte (REG_NOOP, REG_NOOP);
			}
			// send the segment number and data
			max7219_send_byte (segment, y);
			
			// end with enough NOPs so later chips don't update
			for (int i = 0; i < chips - nopCount - 1; i++){
			 	max7219_send_byte (REG_NOOP, REG_NOOP);
			}
			max7219_chip_select(HIGH); // done sending data
			delay_ms(10);
		}
	}
	//
	max7219_clear_displays(BCD_BLANK);
	float ft = 1239.1000;
	uint8_t displayOn = 0; // which display to show this on? 0-N
	uint8_t diVal =0;
	uint8_t offset = (displayOn * 8);
	
	while(ft > 0){
		int d1 = ft;  // Get the integer part.
		float f2 = ft - d1;     // Get fractional part
		int d2 = f2 * 10000;   // Turn into integer
		uint8_t dVal = 1;
		for (uint8_t digit = 8; digit > 4; digit--) {
			dVal = digit + offset - 1;
			uint8_t d2Val = d2 % 10;
			max7219_update_digit(dVal, d2Val);
			d2 /= 10;
		}
		for (uint8_t digit = 4; digit > 0; digit--) {
			dVal = digit + offset - 1;
			
			if(digit == 4){
				uint8_t value  =  ((d1 % 10) | (1 << 7));
				dVal = digit + offset - 1;
				max7219_update_digit(dVal, value);
				
			} else {
				diVal = d1 % 10;
				max7219_update_digit(dVal, diVal);				
			}
			d1 /= 10;
		}
		ft = ft - .0001;
		delay_ms(10);
	}
	
}

