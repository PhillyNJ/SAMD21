/**
 * \file
 *
 * \brief Empty user application template
 *
 */
#include <asf.h>
#include "conf_usart.h"
#include "ds1302.h"
int main (void)
{
	system_init();
	configure_console(115200);
	delay_init();
	printf("SAMD21 with DS1302 RTC Example!\n");
	ds1302_init(false); // if the ds1302 doesnt work, set to true
	ds1302_halt(false);
	
	ds1302_writeProtect(false);
	// The following lines can be commented out to use the values already stored in the DS1302
	//setDOW(MONDAY);        // Set Day-of-Week to FRIDAY
	//setTime(6, 30, 0);     // Set the time to 12:00:00 (24hr format)
	//setDate(7, 10, 2017);   // Set the date to August 6th, 2010

	char buffer[20];
	
	while (1) {
	
		ds1302_getMonthStr(FORMAT_LONG, buffer);
		printf("Its the month of %s \n", buffer);;

		ds1302_getDOWStr(FORMAT_LONG, buffer);
		printf("DOW: %s \n", buffer);
		ds1302_getDateStr(FORMAT_LONG, FORMAT_LITTLEENDIAN, '.', buffer);
		printf("DATE: %s \n", buffer);
		ds1302_getTimeStr(FORMAT_LONG, buffer);
		printf("TIME: %s\n\r", buffer);
		delay_s(1);
		
	}
}
