/*
 * ds3231.c
 *
 * Created: 7/15/2017 2:20:41 PM
 *  Author: pvallone
 */ 
#include <asf.h>
#include "ds3231.h"
#include "samd21_i2c.h"
const uint8_t daysArray [] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
const uint8_t dowArray[]  = { 0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4 };
const unsigned short days[4][12] =
{
	{   0,  31,  60,  91, 121, 152, 182, 213, 244, 274, 305, 335},
	{ 366, 397, 425, 456, 486, 517, 547, 578, 609, 639, 670, 700},
	{ 731, 762, 790, 821, 851, 882, 912, 943, 974,1004,1035,1065},
	{1096,1127,1155,1186,1216,1247,1277,1308,1339,1369,1400,1430},
};

const char janurary[] = "January";
const char february[] = "February";
const char march[] = "March";
const char april[] = "April";
const char may[] = "May";
const char june[] = "June";
const char july[] = "July";
const char august[] = "August";
const char september[] = "September";
const char october[] = "October";
const char november[] = "November";
const char december[] = "December";

const char monday[] = "Monday";
const char tuesday[]= "Tuesday";
const char wednesday[]= "Wednesday";
const char thursday[]= "Thursday";
const char friday[]= "Friday";
const char saturday[]= "Saturday";
const char sunday[] = "Sunday";

const char am[] = "am";
const char am_c[] = "AM";
const char pm_c[] = "PM";
const char pm[] = "pm";

 void ds3231_init(void){
	
	samd21_configure_i2c();
	samd21_configure_i2c_callbacks();
	
	ds3231_set_battery(true, false);
	 t.year = 2000;
	 t.month = 1;
	 t.day = 1;
	 t.hour = 0;
	 t.minute = 0;
	 t.second = 0;
	 t.dayOfWeek = 6;
	 t.unixtime = 946681200;

 }

 uint8_t ds3231_read_register(uint8_t reg){
  
    uint16_t timeout = 0;	
	// write first	
	write_buffer[0] = reg;

	write_packet.address     = DS3231_ADDRESS;
	write_packet.data_length = 1;
	write_packet.data        = write_buffer;

	while (i2c_master_write_packet_wait(&i2c_master_instance, &write_packet) != STATUS_OK){

		if (timeout++ == I2C_TIMEOUT) {
			printf("write timeout \n\r");
			break;
		}
	}
	timeout = 0;


	read_packet.address     = DS3231_ADDRESS;
	read_packet.data_length = 2;
	read_packet.data        = read_buffer;

	while (i2c_master_read_packet_wait(&i2c_master_instance, &read_packet) != STATUS_OK) {
		
		if (timeout++ == I2C_TIMEOUT) {
			printf("ds3231_read_temperature - read timeout \n\r");
			break;
		}
	}
	timeout=0; // reset	
	
	return read_buffer[0];
 }


 void ds3231_write_register(uint8_t reg, uint8_t data){
	  
	  uint16_t timeout = 0;	 
	 
	  write_buffer[0] = reg;
	  write_buffer[1] = data;

	  write_packet.address     = DS3231_ADDRESS;
	  write_packet.data_length = 2;
	  write_packet.data        = write_buffer;
	  
	  while (i2c_master_write_packet_wait(&i2c_master_instance, &write_packet) != STATUS_OK) {
		  
		  if (timeout++ == I2C_TIMEOUT) {
			  printf("Write Timeout!! \n\r");
			  break;
		  }
	  }

	  timeout = 0;
  } 


 void ds3231_set_battery(bool timeBattery, bool squareBattery)
 {	
	 uint8_t value = ds3231_read_register(DS3231_REG_CONTROL); 
	 
	 if (squareBattery)
	 {
		 value |= 0b01000000;
	 } else
	 {
		 value &= 0b10111111;
	 }

	 if (timeBattery)
	 {
		 value &= 0b01111011;
	 } else
	 {
		 value |= 0b10000000;
	 }

	 ds3231_write_register(DS3231_REG_CONTROL, value);
 }

 void ds3231_setDateTime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second){
 
	
	 uint16_t timeout = 0;
	 write_buffer[0] =	DS3231_REG_TIME;
	 write_buffer[1] = ds3231_dec2bcd(second);
	 write_buffer[2] = ds3231_dec2bcd(minute);
	 write_buffer[3] = ds3231_dec2bcd(hour);
	 write_buffer[4] = ds3231_dec2bcd(ds3231_dow(year, month, day));	 

	 write_buffer[5] = ds3231_dec2bcd(day);
	 write_buffer[6] = ds3231_dec2bcd(month);
	 write_buffer[7] = ds3231_dec2bcd(year-2000);
	 write_buffer[8] =	DS3231_REG_TIME;
	 write_packet.address     = DS3231_ADDRESS;
	 write_packet.data_length = 9;
	 write_packet.data        = write_buffer;
	 
	 while (i2c_master_write_packet_wait(&i2c_master_instance, &write_packet) != STATUS_OK) {
		 
		 if (timeout++ == I2C_TIMEOUT) {
			 printf("Write Timeout!! \n\r");
			 break;
		 }
	 }

	 timeout = 0;

 }
 
 void ds3231_set_date_time_from_compile_time(const char* date, const char* time)
 {
	 char s_month[5];
	 int month, day, year, hour, minute;
	 
	 static const char month_names[] = "JanFebMarAprMayJunJulAugSepOctNovDec";

	 sscanf(__DATE__, "%s %d %d", s_month, &day, &year);
	 sscanf(__TIME__, "%d:%d", &hour, &minute);

	 month = ((strstr(month_names, s_month)-month_names)/3) + 1;

	 year = ds3231_conv2d(date + 9);	

	 ds3231_setDateTime(year+2000, month , day, hour, minute, 0);
 }

void ds3231_getDateTime(RTCDateTime *ti)
 {
	// write first
     uint16_t timeout = 0;
	 write_buffer[0] = DS3231_REG_TIME;

	 write_packet.address     = DS3231_ADDRESS;
	 write_packet.data_length = 1;
	 write_packet.data        = write_buffer;

	 while (i2c_master_write_packet_wait(&i2c_master_instance, &write_packet) != STATUS_OK){

		 if (timeout++ == I2C_TIMEOUT) {
			 printf("Get Time - Write timeout \n\r");
			 break;
		 }
	 }
	 timeout = 0;

	 // read back
	 int values[7];

	 
	 read_packet.address     = DS3231_ADDRESS;
	 read_packet.data_length = 7;
	 read_packet.data        = read_buffer;

	 while (i2c_master_read_packet_wait(&i2c_master_instance, &read_packet) != STATUS_OK) {
		 
		 if (timeout++ == I2C_TIMEOUT) {
			 printf("Get Time - Read timeout \n\r");
			 break;
		 }
	 }
	 timeout=0; // reset

	 for (int i = 6; i >= 0; i--)
	 {		
		values[i] = ds3231_bcd2dec(read_buffer[i]);		
	 }	


	 ti->year = values[6] + 2000;
	 ti->month = values[5];
	 ti->day = values[4];
	 ti->dayOfWeek = values[3];
	 ti->hour = values[2];
	 ti->minute = values[1];
	 ti->second = values[0];	 

 }

 // taken from https://codereview.stackexchange.com/questions/38275/convert-between-date-time-and-time-stamp-without-using-standard-library-routines
 // untested
void ds3231_setDateTimeFromUnix(uint32_t ut){
		

	uint16_t year;
	uint8_t month;
	uint8_t day;
	uint8_t hour;
	uint8_t minute;
	uint8_t second;

	 second = ut%60; ut /= 60;
	 minute = ut%60; ut /= 60;
	 hour   = ut%24; ut /= 24;

	 uint32_t years = ut/(365*4+1)*4; ut %= 365*4+1;	 

	 for (year=3; year>0; year--)
	 {
		 if (ut >= days[year][0])
		 break;
	 }
	
	 for (month=11; month>0; month--)
	 {
		 if (ut >= days[year][month])
		 break;
	 }

	 year  = years+year;
	 month = month+1;
	 day   = ut-days[year][month]+1;

	ds3231_setDateTime(year+2000, month, day, hour, minute, second);

}


void ds3231_get_dow_str(uint8_t dayOfWeek, char *output)
{
	
	switch (dayOfWeek)
	{
		case MONDAY:
		strncpy(output, monday, sizeof(monday));
		break;
		case TUESDAY:
		strncpy(output, tuesday, sizeof(tuesday));
		break;
		case WEDNESDAY:
		strncpy(output, wednesday, sizeof(wednesday));
		break;
		case THURSDAY:
		strncpy(output,thursday, sizeof(thursday));
		break;
		case FRIDAY:
		strncpy(output, friday, sizeof(friday));
		break;
		case SATURDAY:
		strncpy(output, saturday, sizeof(saturday));
		break;
		case SUNDAY:
		strncpy(output, sunday, sizeof(sunday));
		break;
	}
	
}

void ds3231_get_month_str(uint8_t month, char *output)
{
	switch (month)
	{
		case 1:
		strncpy(output, janurary, sizeof(janurary));
		break;
		case 2:
		strncpy(output, february, sizeof(february));
		break;
		case 3:
		strncpy(output, march, sizeof(march));
		break;
		case 4:
		strncpy(output, april, sizeof(april));
		break;
		case 5:
		strncpy(output, may, sizeof(may));
		break;
		case 6:
		strncpy(output, june, sizeof(june));
		break;
		case 7:
		strncpy(output, july, sizeof(july));
		break;
		case 8:
		strncpy(output, august, sizeof(august));
		break;
		case 9:
		strncpy(output, september, sizeof(september));
		break;
		case 10:
		strncpy(output, october, sizeof(october));
		break;
		case 11:
		strncpy(output, november, sizeof(november));
		break;
		case 12:
		strncpy(output, december, sizeof(december));
		break;
		default:
		strncpy(output, "Unknown", 7);
		break;
	}
	
}

 uint8_t ds3231_bcd2dec(uint8_t bcd)
 {
	 return ((bcd / 16) * 10) + (bcd % 16);
 }

 uint8_t ds3231_dec2bcd(uint8_t dec)
 {
	 return ((dec / 10) * 16) + (dec % 10);
 }

 uint8_t ds3231_conv2d(const char* p)
 {
	 uint8_t v = 0;

	 if ('0' <= *p && *p <= '9')
	 {
		 v = *p - '0';
	 }

	 return 10 * v + *++p - '0';
 }

 /*
 https://stackoverflow.com/a/21235587/893219
 */
 uint8_t  ds3231_dow(uint16_t y, uint8_t m, uint8_t d)
 { 	 	 	
	 return  (d += m < 3 ? y-- : y - 2, 23*m/9 + d + 4 + y/4- y/100 + y/400)%7; ;
 }

 
 void ds3231_enable_output(bool enabled){ 

	  uint8_t value = ds3231_read_register(DS3231_REG_CONTROL);	 
	  value |= (enabled << 2) ;	 	
	  ds3231_write_register(DS3231_REG_CONTROL, value);
 
 }

 bool ds3231_is_output(void)
 {
	uint8_t value = ds3231_read_register(DS3231_REG_CONTROL);
	
	value &= 0b00000100;
	value >>= 2;

	return !value;
 }

 void ds3231_set_output(DS3231_sqw_t mode)
 {
	  uint8_t value = ds3231_read_register(DS3231_REG_CONTROL);
	
	 value &= 0b11100111;
	 value |= (mode << 3);
	 ds3231_write_register(DS3231_REG_CONTROL, value);
 }

 DS3231_sqw_t ds3231_get_output(void)
 {

	  uint8_t value = ds3231_read_register(DS3231_REG_CONTROL);
	 
	 value &= 0b00011000;
	 value >>= 3;

	 return (DS3231_sqw_t)value;
 }

 void ds3231_enable_32kHz(bool enabled)
 { 

	 uint8_t value = ds3231_read_register(DS3231_REG_STATUS);
	 
	 value &= 0b11110111;
	 value |= (enabled << 3);
	 ds3231_write_register(DS3231_REG_STATUS, value);

 }

 bool ds3231_is_32kHz(void)
 {
	 uint8_t value = ds3231_read_register(DS3231_REG_STATUS);
	
	 value &= 0b00001000;
	 value >>= 3;
	 return value;
 }

 void ds3231_force_conversion(void)
 {
	
	 uint8_t value = ds3231_read_register(DS3231_REG_CONTROL);	
	 value |= 0b00100000;
	 ds3231_write_register(DS3231_REG_CONTROL, value);
	 do {} while ((ds3231_read_register(DS3231_REG_CONTROL) & 0b00100000) != 0);
 }


 float ds3231_read_temperature(void)
 {
	 uint8_t msb, lsb;
	 // write first
	 uint16_t timeout = 0;
	 write_buffer[0] = DS3231_REG_TEMPERATURE;

	 write_packet.address     = DS3231_ADDRESS;
	 write_packet.data_length = 2;
	 write_packet.data        = write_buffer;

	 while (i2c_master_write_packet_wait(&i2c_master_instance, &write_packet) != STATUS_OK){

		 if (timeout++ == I2C_TIMEOUT) {
			 printf("write timeout \n\r");
			 break;
		 }
	 }
	 timeout = 0;


	 read_packet.address     = DS3231_ADDRESS;
	 read_packet.data_length = 2;
	 read_packet.data        = read_buffer;

	 while (i2c_master_read_packet_wait(&i2c_master_instance, &read_packet) != STATUS_OK) {
		  
		  if (timeout++ == I2C_TIMEOUT) {
			  printf("ds3231_read_temperature - read timeout \n\r");
			  break;
		  }
	  }
	  timeout=0; // reset

	  msb = read_buffer[0];
	  lsb = read_buffer[1];
	

	 return ((((short)msb << 8) | (short)lsb) >> 6) / 4.0f;
 }

 void ds3231_get_alarm_1(RTCAlarmTime *a)
 {
	 uint16_t timeout = 0;
	 uint8_t values[4];	

	 write_buffer[0] = DS3231_REG_ALARM_1;

	 write_packet.address     = DS3231_ADDRESS;
	 write_packet.data_length = 1;
	 write_packet.data        = write_buffer;

	 while (i2c_master_write_packet_wait(&i2c_master_instance, &write_packet) != STATUS_OK){

		 if (timeout++ == I2C_TIMEOUT) {
			 printf("write timeout \n\r");
			 break;
		 }
	 }
	 timeout = 0;

	 read_packet.address     = DS3231_ADDRESS;
	 read_packet.data_length = 4;
	 read_packet.data        = read_buffer;

	 while (i2c_master_read_packet_wait(&i2c_master_instance, &read_packet) != STATUS_OK) {
		  
		  if (timeout++ == I2C_TIMEOUT) {
			  printf("ds3231_get_alarm_1 - read timeout \n\r");
			  break;
		  }
	 }
	  timeout=0; // reset
	  
	 for (int i = 3; i >= 0; i--)	 {
		
		 values[i] = ds3231_bcd2dec(read_buffer[i] & 0b01111111);
		
	 }	

	 a->day = values[3];
	 a->hour = values[2];
	 a->minute = values[1];
	 a->second = values[0];
	
 }


 DS3231_alarm1_t ds3231_get_alarm_type_1(void)
 {	
	 uint8_t mode = 0;
	 uint16_t timeout = 0;

	 write_buffer[0] = DS3231_REG_ALARM_1;

	 write_packet.address     = DS3231_ADDRESS;
	 write_packet.data_length = 1;
	 write_packet.data        = write_buffer;

	 while (i2c_master_write_packet_wait(&i2c_master_instance, &write_packet) != STATUS_OK){

		 if (timeout++ == I2C_TIMEOUT) {
			 printf("write timeout \n\r");
			 break;
		 }
	 }
	 timeout = 0;

	  read_packet.address     = DS3231_ADDRESS;
	  read_packet.data_length = 4;
	  read_packet.data        = read_buffer;

	  while (i2c_master_read_packet_wait(&i2c_master_instance, &read_packet) != STATUS_OK) {
		  
		  if (timeout++ == I2C_TIMEOUT) {
			  printf("ds3231_get_alarm_type_1 -read timeout \n\r");
			  break;
		  }
	  }
	  timeout=0; // reset 
	
	 mode |= ((read_buffer[3] & 0b10000000) >> 4);
	 mode |= ((read_buffer[2] & 0b10000000) >> 5);
	 mode |= ((read_buffer[1] & 0b10000000) >> 6);
	 mode |= ((read_buffer[0] & 0b10000000) >> 7);
	 mode |= ((read_buffer[0] & 0b01000000) >> 2);
	 
	 return (DS3231_alarm1_t)mode;
 }
 
 void ds3231_set_alarm_1(uint8_t dydw, uint8_t hour, uint8_t minute, uint8_t second, DS3231_alarm1_t mode, bool armed)
 {
	 uint16_t timeout = 0;
	 second = ds3231_dec2bcd(second);
	 minute = ds3231_dec2bcd(minute);
	 hour = ds3231_dec2bcd(hour);
	 dydw = ds3231_dec2bcd(dydw);

	 switch(mode)
	 {
		 case DS3231_EVERY_SECOND:
		 second |= 0b10000000;
		 minute |= 0b10000000;
		 hour |= 0b10000000;
		 dydw |= 0b10000000;
		 break;

		 case DS3231_MATCH_S:
		 second &= 0b01111111;
		 minute |= 0b10000000;
		 hour |= 0b10000000;
		 dydw |= 0b10000000;
		 break;

		 case DS3231_MATCH_M_S:
		 second &= 0b01111111;
		 minute &= 0b01111111;
		 hour |= 0b10000000;
		 dydw |= 0b10000000;
		 break;

		 case DS3231_MATCH_H_M_S:
		 second &= 0b01111111;
		 minute &= 0b01111111;
		 hour &= 0b01111111;
		 dydw |= 0b10000000;
		 break;

		 case DS3231_MATCH_DT_H_M_S:
		 second &= 0b01111111;
		 minute &= 0b01111111;
		 hour &= 0b01111111;
		 dydw &= 0b01111111;
		 break;

		 case DS3231_MATCH_DY_H_M_S:
		 second &= 0b01111111;
		 minute &= 0b01111111;
		 hour &= 0b01111111;
		 dydw &= 0b01111111;
		 dydw |= 0b01000000;
		 break;
	 }

	 write_buffer[0] = DS3231_REG_ALARM_1;
	 write_buffer[1] = second;
	 write_buffer[2] = minute;
	 write_buffer[3] = hour;
	 write_buffer[4] = dydw;
	 write_packet.address     = DS3231_ADDRESS;
	 write_packet.data_length = 5;
	 write_packet.data        = write_buffer;

	 while (i2c_master_write_packet_wait(&i2c_master_instance, &write_packet) != STATUS_OK){

		 if (timeout++ == I2C_TIMEOUT) {
			 printf("write timeout \n\r");
			 break;
		 }
	 }
	 timeout = 0;

	 ds3231_arm_alarm_1(armed);
	 ds3231_clear_alarm_1();
 }
 
 void ds3231_diable_alarm_1(void){

	 uint8_t value;

	 value = ds3231_read_register(DS3231_REG_CONTROL);
	 
	 value &= 0b11111110;

	 ds3231_write_register(DS3231_REG_CONTROL, value);
 }

 void ds3231_arm_alarm_1(bool armed)
 {
	 uint8_t value;
	 value = ds3231_read_register(DS3231_REG_CONTROL);

	 if (armed)
	 {
		 value |= 0b00000001;
	 } else
	 {
		 value &= 0b11111110;
	 }

	 ds3231_write_register(DS3231_REG_CONTROL, value);	 

 }

 bool ds3231_is_armed_1(void)
 {
	 uint8_t value;
	 value = ds3231_read_register(DS3231_REG_CONTROL);
	 value &= 0b00000001;
	 return value;
 }


 bool ds3231_check_alarm_1_state(bool clear)
 {
	 uint8_t alarm;

	 alarm = ds3231_read_register(DS3231_REG_STATUS);
	
	 alarm &= 0b00000001;	
	 if (alarm && clear)
	 {
		 ds3231_clear_alarm_1();
	 }

	 return alarm;
 }
 // clears the alarm A1F bit in Status Register
 void ds3231_clear_alarm_1(void)
 {
	 uint8_t value;

	 value = ds3231_read_register(DS3231_REG_STATUS); 
	
	 value &= 0b11111110;	

	 ds3231_write_register(DS3231_REG_STATUS, value);
 }

 
 void ds3231_get_alarm_2(RTCAlarmTime *a)
 {
	 uint16_t timeout = 0;
	 uint8_t values[3];

	 write_buffer[0] = DS3231_REG_ALARM_2;

	 write_packet.address     = DS3231_ADDRESS;
	 write_packet.data_length = 1;
	 write_packet.data        = write_buffer;

	 while (i2c_master_write_packet_wait(&i2c_master_instance, &write_packet) != STATUS_OK){

		 if (timeout++ == I2C_TIMEOUT) {
			 printf("write timeout \n\r");
			 break;
		 }
	 }
	 timeout = 0;

	 read_packet.address     = DS3231_ADDRESS;
	 read_packet.data_length = 3;
	 read_packet.data        = read_buffer;

	 while (i2c_master_read_packet_wait(&i2c_master_instance, &read_packet) != STATUS_OK) {
		 
		 if (timeout++ == I2C_TIMEOUT) {
			 printf("ds3231_get_alarm_2 - read timeout \n\r");
			 break;
		 }
	 }
	 timeout=0; // reset
	 
	 for (int i = 2; i >= 0; i--)
	 {
		 
		 values[i] = ds3231_bcd2dec(read_buffer[i] & 0b01111111);
		 
	 }

	 a->day = values[2];
	 a->hour = values[1];
	 a->minute = values[0];
	 a->second = 0;
	
 }


  DS3231_alarm2_t ds3231_get_alarm_type_2(void)
  {
	 // uint8_t values[4];
	  uint8_t mode = 0;
	  uint16_t timeout = 0;

	  write_buffer[0] = DS3231_REG_ALARM_2;

	  write_packet.address     = DS3231_ADDRESS;
	  write_packet.data_length = 1;
	  write_packet.data        = write_buffer;

	  while (i2c_master_write_packet_wait(&i2c_master_instance, &write_packet) != STATUS_OK){

		  if (timeout++ == I2C_TIMEOUT) {
			  printf("write timeout \n\r");
			  break;
		  }
	  }
	  timeout = 0;

	  read_packet.address     = DS3231_ADDRESS;
	  read_packet.data_length = 3;
	  read_packet.data        = read_buffer;

	  while (i2c_master_read_packet_wait(&i2c_master_instance, &read_packet) != STATUS_OK) {
		  
		  if (timeout++ == I2C_TIMEOUT) {
			  printf("ds3231_get_alarm_type_2 - read timeout \n\r");
			  break;
		  }
	  }
	  timeout=0; // reset	

	  mode |= ((read_buffer[2] & 0b10000000) >> 4);
	  mode |= ((read_buffer[1] & 0b10000000) >> 5);
	  mode |= ((read_buffer[0] & 0b10000000) >> 6);
	  mode |= ((read_buffer[0] & 0b10000000) >> 7);
	 
	
	  //mode |= ((values[2] & 0b01000000) >> 5);
	  //mode |= ((values[1] & 0b01000000) >> 4);
	  //mode |= ((values[0] & 0b01000000) >> 3);
	  //mode |= ((values[0] & 0b00100000) >> 1);

	  return (DS3231_alarm2_t)mode;
  }

  void ds3231_set_alarm_2(uint8_t dydw, uint8_t hour, uint8_t minute, DS3231_alarm2_t mode, bool armed)
  {
	  uint16_t timeout = 0;
	 
	  minute = ds3231_dec2bcd(minute);
	  hour = ds3231_dec2bcd(hour);
	  dydw = ds3231_dec2bcd(dydw);

	  switch(mode)
	  {		  

		  case DS3231_MATCH_S:
		 
		  minute |= 0b10000000;
		  hour |= 0b10000000;
		  dydw |= 0b10000000;
		  break;

		  case DS3231_MATCH_M_S:
		  
		  minute &= 0b01111111;
		  hour |= 0b10000000;
		  dydw |= 0b10000000;
		  break;

		  case DS3231_MATCH_H_M_S:
		  
		  minute &= 0b01111111;
		  hour &= 0b01111111;
		  dydw |= 0b10000000;
		  break;

		  case DS3231_MATCH_DT_H_M_S:
		 
		  minute &= 0b01111111;
		  hour &= 0b01111111;
		  dydw &= 0b01111111;
		  break;

		  case DS3231_MATCH_DY_H_M_S:
		  
		  minute &= 0b01111111;
		  hour &= 0b01111111;
		  dydw &= 0b01111111;
		  dydw |= 0b01000000;
		  break;
	  }

	  write_buffer[0] = DS3231_REG_ALARM_2;
	  write_buffer[1] = minute;
	  write_buffer[2] = hour;
	  write_buffer[3] = dydw;	 
	  write_packet.address     = DS3231_ADDRESS;
	  write_packet.data_length = 4;
	  write_packet.data        = write_buffer;

	  while (i2c_master_write_packet_wait(&i2c_master_instance, &write_packet) != STATUS_OK){

		  if (timeout++ == I2C_TIMEOUT) {
			  printf("write timeout \n\r");
			  break;
		  }
	  }
	  timeout = 0;

	  ds3231_arm_alarm_2(armed);
	  ds3231_clear_alarm_2();
  }
  void ds3231_arm_alarm_2(bool armed)
  {
	  uint8_t value;
	  value = ds3231_read_register(DS3231_REG_CONTROL);

	  if (armed)
	  {
		  value |= 0b00000010;
	  } else
	  {
		  value &= 0b11111101;
	  }

	 ds3231_write_register(DS3231_REG_CONTROL, value);
  }

  void ds3231_clear_alarm_2(void)
  {
	  uint8_t value;

	  value = ds3231_read_register(DS3231_REG_STATUS);
	  value &= 0b11111101;

	  ds3231_write_register(DS3231_REG_STATUS, value);
  }

  bool ds3231_is_armed_2(void)
  {
	  uint8_t value;
	  value = ds3231_read_register(DS3231_REG_CONTROL);
	  value &= 0b00000010;
	  value >>= 1;
	  return value;
  }
  
  bool ds3231_check_alarm_2_state(bool clear)
  {
	  uint8_t alarm;

	  alarm = ds3231_read_register(DS3231_REG_STATUS);
	  alarm &= 0b00000010;

	  if (alarm && clear)
	  {
		  ds3231_clear_alarm_2();
	  }

	  return alarm;
  }


void ds3231_str_am_pm(uint8_t hour, bool uppercase, char *output)
  {
	  if (hour < 12)
	  {
		  if (uppercase)
		  {
		      strncpy(output, am_c, sizeof(am_c));
			 
		  } else
		  {
			  strncpy(output, am, sizeof(am));
		  }
	  } else
	  {
		  if (uppercase)
		  {
			  strncpy(output, pm_c, sizeof(pm_c));
		  } else
		  {
			  strncpy(output, pm, sizeof(pm));
		  }
	  }
  }