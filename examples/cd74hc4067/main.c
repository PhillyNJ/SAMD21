
#include <asf.h>
#include "sercom_usart_config.h"
#include "cd74hc4067_multiplexer.h"

/*
Example for the cd74hc4067 multiplexer
This example uses PORTB. To change, modify cd74hc4067_multiplexer.h
uint64_t en, uint8_t sig_pin, uint64_t s0, uint64_t s1, uint64_t s2, uint64_t s3, uint64_t sig_port

Pin Out

cd74hc4067		SAMD21	Mode
--------------------------------
EN				PB09

SIG 			PB07	Input / Read
SIG				5v		Output

S0				PB02
S1				PB03
S2				PB04
S3				PB05

*/

int main (void)
{
	system_init();
	delay_init();
	configure_console(9600);
	printf("SAMD21 cd74hc4067 16 channel Multiplexer\n\r");
	

	cd74hc4067_init(PORT_PB09, PIN_PB07, PORT_PB02,PORT_PB03,PORT_PB04,PORT_PB05, PORT_PB07);
	
	
	while (1) {
		/* Is button pressed? */
		if (port_pin_get_input_level(BUTTON_0_PIN) == BUTTON_0_ACTIVE) {
			/* Yes, so turn LED on. */
			port_pin_set_output_level(LED_0_PIN, LED_0_ACTIVE);
			
			for(uint8_t i = 0; i < 100; i++){
				
				cd74hc4067_set_channel(15);
				delay_ms(100);
				cd74hc4067_set_channel(0);
				delay_ms(100);
			}
			
			
			} else {
			/* No, so turn LED off. */
			port_pin_set_output_level(LED_0_PIN, !LED_0_ACTIVE);
			
		}
		//Read Example
		//for(uint8_t i = 0; i < 16; i++){
		//uint8_t val = cd74hc4067_read_channel(i);
		//if(val == 1){
		//printf("Channel: %d is HIGH\n\r", i);
		//}
		//}
		
		// output example
		//for(uint8_t i = 0; i < 16; i++){
		//cd74hc4067_set_channel(i);
		//delay_ms(10);
		//}
		
	}
}
