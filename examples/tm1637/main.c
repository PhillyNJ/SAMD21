#include <asf.h>
#include "sercom_usart_config.h"
#include "TM1637.h"
uint8_t ti = 0;
uint8_t timeDisp[] = {0x04,0x01,0x02,0x03};
int main (void)
{
	system_init();
	delay_init();
	configure_console(9600);
	printf("SAMD21 T11637 Display\n\r");
	tm1637_init(PORT_PA02, PORT_PA03);
	_PointFlag = true;
	tm1637_display(timeDisp);
	while (1) {
		
		timeDisp[0] = ti;
			/* Yes, so turn LED on. */
		
		
		/* Is button pressed? */
		if (port_pin_get_input_level(BUTTON_0_PIN) == BUTTON_0_ACTIVE) {
			timeDisp[0] = ti;
			/* Yes, so turn LED on. */
			port_pin_set_output_level(LED_0_PIN, LED_0_ACTIVE);
			tm1637_display(timeDisp);
		} else {
			/* No, so turn LED off. */
			port_pin_set_output_level(LED_0_PIN, !LED_0_ACTIVE);
		}
		ti++;
		if(ti > 9){
			ti = 0;
		}
	}
}
