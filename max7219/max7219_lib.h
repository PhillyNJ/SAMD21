/*
 * MAX7219_config.h
 *
 * Created: 9/6/2020 9:06:18 AM
 *  Author: pvallone
 */ 


#ifndef MAX7219_CONFIG_H_
#define MAX7219_CONFIG_H_
#include <asf.h>

#define REG_NOOP 0x00
#define REG_DIG0 0x01
#define REG_DIG1 0x02
#define REG_DIG2 0x03
#define REG_DIG3 0x04
#define REG_DIG4 0x05
#define REG_DIG5 0x06
#define REG_DIG6 0x07
#define REG_DIG7 0x08
#define REG_DECODE_MODE 0x09
#define REG_INTENSITY 0x0A
#define REG_SCANLIMIT 0x0B
#define REG_SHUTDOWN 0x0C
#define REG_DISPLAY_TEST 0x0F

// BCD Coding 
#define BCD_DASH		0x0A
#define BCD_E			0x0B
#define BCD_H			0x0C
#define BCD_L			0x0D
#define BCD_P			0x0E
#define BCD_BLANK		0x0F

#define BLANK 0x00
#define NUM0 0x7E
#define NUM2 0x6D
#define DASH 0x01

#define LETF 0x47
#define LETH 0x37
#define LETI 0x06
#define LETL 0x0E
#define LETP 0x67
#define LETS 0x5B

#define HI 1
#define LOW 0
uint64_t max_din, max_cs, max_clk;
uint8_t chips;
#define CLKLOW REG_PORT_OUT1 &= ~(max_clk);
#define CLKHI REG_PORT_OUT1 |= max_clk; // set
#define DIN_HIGH REG_PORT_OUT1 |= max_din; // set
#define DIN_LOW REG_PORT_OUT1 &= ~(max_din);
//#define CHIPS 4

void max7219_init(uint64_t cs, uint64_t clk, uint64_t din, uint8_t numberOfChips);
void max7219_chip_select(uint8_t direction);
void max7219_reset_chip(void);
void max7219_set_register(uint8_t reg, uint8_t data, uint8_t pos);
void max7219_print_str(const char*s);
uint8_t max7219_reverse(uint8_t b);
void max7219_update_digit(uint8_t pos, uint8_t value);
void max7219_send_byte(uint8_t reg, uint8_t data);
void max7219_set_mode(uint8_t bcd);
void max7219_run_text_test(void);
void max7219_clear_displays(uint8_t value);
void max7219_run_bcd_test(void);
#endif /* MAX7219_CONFIG_H_ */