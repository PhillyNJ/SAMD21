#include <asf.h>
#include <string.h>
#include "sercom_usart_config.h"
#include "max7219_lib.h"
/*
 Example Project for the MAX7219 7-Segment Display
 Support daisy-chaining displays. 

  Pin Out
  MAX7219	SAMD21
  ----------------
  CS		PB00
  CLK		PB01
  DIN		PB06
  Chips		4	Number of 7 segment displays
*/
int main (void)
{
	system_init();
	delay_init();
	configure_console(9600);
	max7219_init(PORT_PB00,PORT_PB01,PORT_PB06, 4); // Port PB
	max7219_set_mode(0); // set off BCD
	max7219_clear_displays(BLANK);
	delay_ms(1500);
	max7219_run_text_test();
	delay_ms(5500);
	max7219_clear_displays(BLANK);
	max7219_print_str("1.2345.5");
	delay_ms(5500);
	max7219_clear_displays(BLANK);
	max7219_reset_chip();
	delay_ms(1500);
	max7219_set_mode(1); // turn on BCD
	max7219_clear_displays(BCD_BLANK);
 	max7219_run_bcd_test();
	
	while (1) {
		/* Is button pressed? */
		if (port_pin_get_input_level(BUTTON_0_PIN) == BUTTON_0_ACTIVE) {
			/* Yes, so turn LED on. */
			port_pin_set_output_level(LED_0_PIN, LED_0_ACTIVE);
		} else {
			/* No, so turn LED off. */
			port_pin_set_output_level(LED_0_PIN, !LED_0_ACTIVE);
		}
	}
}
