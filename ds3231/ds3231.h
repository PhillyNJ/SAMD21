/*
 * ds3231.h
 *
 * Created: 7/15/2017 2:20:32 PM
 *  Author: pvallone
 * Modified form -> https://github.com/jarzebski/Arduino-DS3231
 */ 


#ifndef DS3231_H_
#define DS3231_H_
#include <string.h>

#define MONDAY		1
#define TUESDAY		2
#define WEDNESDAY	3
#define THURSDAY	4
#define FRIDAY		5
#define SATURDAY	6
#define SUNDAY		0


#define DS3231_ADDRESS              0x68
#define DS3231_REG_TIME             0x00
#define DS3231_REG_ALARM_1          0x07
#define DS3231_REG_ALARM_2          0x0B
#define DS3231_REG_CONTROL          0x0E
#define DS3231_REG_STATUS           0x0F
#define DS3231_REG_TEMPERATURE      0x11


typedef struct 
{
	uint16_t year;
	uint8_t month;
	uint8_t day;
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
	uint8_t dayOfWeek;
	uint32_t unixtime;
} RTCDateTime;

typedef struct  
{
	uint8_t day;
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
} RTCAlarmTime;

typedef enum
{
	DS3231_1HZ          = 0x00,
	DS3231_4096HZ       = 0x01,
	DS3231_8192HZ       = 0x02,
	DS3231_32768HZ      = 0x03
} DS3231_sqw_t;

typedef enum
{
	DS3231_EVERY_SECOND   = 0b00001111,
	DS3231_MATCH_S        = 0b00001110,
	DS3231_MATCH_M_S      = 0b00001100,
	DS3231_MATCH_H_M_S    = 0b00001000,
	DS3231_MATCH_DT_H_M_S = 0b00000000,
	DS3231_MATCH_DY_H_M_S = 0b00010000
} DS3231_alarm1_t;

typedef enum
{
	DS3231_EVERY_MINUTE   = 0b00001110,
	DS3231_MATCH_M        = 0b00001100,
	DS3231_MATCH_H_M      = 0b00001000,
	DS3231_MATCH_DT_H_M   = 0b00000000,
	DS3231_MATCH_DY_H_M   = 0b00010000
} DS3231_alarm2_t;


RTCDateTime t;

void ds3231_init(void);
uint8_t ds3231_read_register(uint8_t reg);
void ds3231_write_register(uint8_t reg, uint8_t data);
void ds3231_set_battery(bool timeBattery, bool squareBattery);

uint8_t ds3231_bcd2dec(uint8_t bcd);
uint8_t ds3231_dec2bcd(uint8_t dec);
uint8_t ds3231_conv2d(const char* p);
uint8_t ds3231_dow(uint16_t y, uint8_t m, uint8_t d);
void ds3231_setDateTime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);
void ds3231_set_date_time_from_compile_time(const char* date, const char* time);
void ds3231_getDateTime(RTCDateTime *ti);
void ds3231_setDateTimeFromUnix(uint32_t ut);
void ds3231_get_dow_str(uint8_t dayOfWeek, char *output);
void ds3231_get_month_str(uint8_t month, char *output);
void ds3231_enable_output(bool enabled);
bool ds3231_is_output(void);
void ds3231_set_output(DS3231_sqw_t mode);
DS3231_sqw_t ds3231_get_output(void);
void ds3231_enable_32kHz(bool enabled);
bool ds3231_is_32kHz(void);
void ds3231_force_conversion(void);
float ds3231_read_temperature(void);
void ds3231_get_alarm_1(RTCAlarmTime *a);
bool ds3231_is_armed_1(void);
DS3231_alarm1_t ds3231_get_alarm_type_1(void);
void ds3231_arm_alarm_1(bool armed);
bool ds3231_check_alarm_1_state(bool clear);
void ds3231_clear_alarm_1(void);
void ds3231_set_alarm_1(uint8_t dydw, uint8_t hour, uint8_t minute, uint8_t second, DS3231_alarm1_t mode, bool armed);
void ds3231_get_alarm_2(RTCAlarmTime *a);
DS3231_alarm2_t ds3231_get_alarm_type_2(void);
void ds3231_set_alarm_2(uint8_t dydw, uint8_t hour, uint8_t minute, DS3231_alarm2_t mode, bool armed);
void ds3231_arm_alarm_2(bool armed);
void ds3231_clear_alarm_2(void);
bool ds3231_is_armed_2(void);
bool  ds3231_check_alarm_2_state(bool clear);
void ds3231_str_am_pm(uint8_t hour, bool uppercase, char *output);
void ds3231_diable_alarm_1(void);
#endif /* DS3231_H_ */