#include <asf.h>
#include "sercom_usart_config.h"
#include "cd74hc4067_multiplexer_controller.h"
struct cd74hc4067_config config;

/*
Example on how to have multiple cd74hc4067 read

This example uses PORTB. To change, modify cd74hc4067_multiplexer_controller.h
Examples use 3 cd74hc4067 multiplexers for 48 inputs

Pin Out

cd74hc4067		SAMD21	Mode
--------------------------------
EN				PB09

SIG[n] 			PBXX	Input / Read

S0				PB02
S1				PB03
S2				PB04
S3				PB05

*/

uint8_t bus[16] = "/0";
int main (void)
{
	system_init();
	delay_init();
	configure_console(9600);
	
	printf("SAMD21 cd74hc4067 read controller example\n\r");
	
	config.s0 =			PORT_PB02;
	config.s1 =			PORT_PB03;
	config.s2 =			PORT_PB04;
	config.s3 =			PORT_PB05;	
	config.sig_pin =	PIN_PB07;	
	
	// enable pins - examples used 3
	config.en_devices[0] = PORT_PB09;
	config.en_devices[1] = PORT_PB10;
	config.en_devices[2] = PORT_PB15;
	
	cd74hc4067_init(&config);
	
	while (1) {
		/* Is button pressed? */
		if (port_pin_get_input_level(BUTTON_0_PIN) == BUTTON_0_ACTIVE) {
			/* Yes, so turn LED on. */
			port_pin_set_output_level(LED_0_PIN, LED_0_ACTIVE);
			cd74hc4067_read_bus_channel(0, bus);			
			for(uint8_t i = 0; i < 16; i++){				
				printf("Bus 1: Index: %d Val: %d\n\r", i, bus[i]);
			}
			cd74hc4067_read_bus_channel(1, bus);
			for(uint8_t i = 0; i < 16; i++){
				printf("Bus 2: Index: %d Val: %d\n\r", i, bus[i]);
			}
			cd74hc4067_read_bus_channel(2, bus);
			for(uint8_t i = 0; i < 16; i++){
				printf("Bus 3 Index: %d Val: %d\n\r", i, bus[i]);
			}
			delay_ms(10);
		} else {
			/* No, so turn LED off. */
			port_pin_set_output_level(LED_0_PIN, !LED_0_ACTIVE);
		}
	}
}
