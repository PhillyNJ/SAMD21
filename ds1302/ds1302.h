/* This code was originally writen from DS1392 Library found at   
 *	 http://www.RinkyDinkElectronics.com/
 *	It was adopted to work on a samd21
 *  ds1302.h
 *
 * Created: 7/7/2017 9:43:46 AM
 *  Author: pvallone
 */ 


#ifndef DS1302_H_
#define DS1302_H_

#include <asf.h>
#include <string.h>

typedef struct {

	uint8_t	cell[31];

} ds1302_ram_t;

typedef struct {
	
	uint8_t		hour;
	uint8_t		min;
	uint8_t		sec;
	uint8_t		date;
	uint8_t		mon;
	uint16_t	year;
	uint8_t		dow;

}ds_time_t;

typedef enum {
 
	INPUT,
	OUTPUT
	
} direction_t; 
 
 #define LSBFIRST 0
 #define MSBFIRST 1

 #define CE		PORT_PB00
 #define DATA	PORT_PB01
 #define SCLK	PORT_PB06


#define REG_SEC		0
#define REG_MIN		1
#define REG_HOUR	2
#define REG_DATE	3
#define REG_MON		4
#define REG_DOW		5
#define REG_YEAR	6
#define REG_WP		7
#define REG_TCR		8

#define FORMAT_SHORT	1
#define FORMAT_LONG	2

#define FORMAT_LITTLEENDIAN	1
#define FORMAT_BIGENDIAN	2
#define FORMAT_MIDDLEENDIAN	3

#define MONDAY		1
#define TUESDAY		2
#define WEDNESDAY	3
#define THURSDAY	4
#define FRIDAY		5
#define SATURDAY	6
#define SUNDAY		7

#define TCR_D1R2K	165
#define TCR_D1R4K	166
#define TCR_D1R8K	167
#define TCR_D2R2K	169
#define TCR_D2R4K	170
#define TCR_D2R8K	171
#define TCR_OFF		92

uint8_t _burstArray[8];
bool additionalDelay;

void	ds1302_init(bool addDelay);

void	ds1302_getTime(ds_time_t *t);
void	ds1302_setTime(uint8_t hour, uint8_t min, uint8_t sec);
void	ds1302_setDate(uint8_t date, uint8_t mon, uint16_t year);
void	ds1302_setDOW(uint8_t dow);

void	ds1302_getTimeStr(uint8_t format, char *output);
void	ds1302_getDateStr(uint8_t slformat, uint8_t eformat, char divider, char *output);
void	ds1302_getDOWStr(uint8_t format, char *output);
void	ds1302_getMonthStr(uint8_t format, char *output);

void	ds1302_halt(bool value);
void	ds1302_writeProtect(bool enable);
void	ds1302_setTCR(uint8_t value);

void	ds1302_writeBuffer(ds1302_ram_t *r);
void	ds1302_readBuffer(ds1302_ram_t *r);
void	ds1302_poke(uint8_t addr, uint8_t value);
uint8_t	ds1302_peek(uint8_t addr);

uint8_t	ds1302_readByte(void);
void	ds1302_writeByte(uint8_t value);
uint8_t	ds1302_readRegister(uint8_t reg);
void 	ds1302_writeRegister(uint8_t reg, uint8_t value);
void	ds1302_burstRead(void);
uint8_t	ds1302_decode(uint8_t value);
uint8_t	ds1302_decodeH(uint8_t value);
uint8_t	ds1302_decodeY(uint8_t value);
uint8_t	ds1302_encode(uint8_t vaule);
void	ds1302_shiftOut(uint8_t bitOrder, uint8_t val);
void	 ds1302_set_data_port_direction(direction_t dir);
#endif /* DS1302_H_ */