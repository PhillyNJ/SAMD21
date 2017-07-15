/**
 * Example DS1302 with SAMD21 xplained pro
 *	Pin Out - To change pinout to different pins, see ds1302.h
 *	CE ->	PORT_PB00
 *  DATA -> PORT_PB01
 *  SCLK->	PORT_PB06
 *  If you change the pins to differnt port/group a, you need to update any register calls 
 *  Notes
 *  SAMD21 by defaults rund at 8mhz
 */
#include <asf.h>
#include "conf_usart.h"
#include "ds1302.h"


char buffer[20];
ds_time_t myTime;
int main (void)
{
	system_init();	
	
	configure_console(115200);	
	delay_init();	
	
	printf("SAMD21 with DS1302 RTC Example!\n");	
	printf(("Compile time %s \n"), __TIME__);
	printf(("Compile Date %s \n"), __DATE__);
	ds1302_init(true); // if the ds1302 doesnt work, set to true
	
	ds1302_halt(false);	
	ds1302_write_protect(false);
	ds1302_set_compile_time(SATURDAY); // always set to compile time :)

	while (1) {			
			
			ds1302_get_complete_time(&myTime);
			printf("It's %s!\n", myTime.monl);
			printf("Today is %s %d.%d.%d\n", myTime.dowl, myTime.mon, myTime.day, myTime.year);
			printf("Time is %02d:%02d:%02d\n", myTime.hour, myTime.min, myTime.sec);		
			delay_s(1);
	}
}
