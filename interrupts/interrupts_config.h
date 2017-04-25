/*
 * interrupts_config.h
 *
 * Created: 4/23/2017 3:52:48 PM
 *  Author: pvallone
 */ 


#ifndef INTERRUPTS_CONFIG_H_
#define INTERRUPTS_CONFIG_H_
#include "asf.h"


typedef struct {	
	uint32_t gpio_pin;
	/** MUX position the GPIO pin should be configured to */
	uint32_t gpio_pin_mux;
	/** Internal pull to enable on the input pin */
	enum extint_pull gpio_pin_pull;
	/** Wake up the device if the channel interrupt fires during sleep mode */
	bool wake_if_sleeping;

	/** Filter the raw input signal to prevent noise from triggering an
	 *  interrupt accidentally, using a three sample majority filter */
	bool filter_input_signal;
	/** Edge detection mode to use */
	enum extint_detect detection_criteria;
	uint8_t eic_line;

} interrupt_conf_t;


void configure_extint_channel(interrupt_conf_t const *config);
void configure_extint_callbacks(uint8_t eic_line, const extint_callback_t callback);




#endif /* INTERRUPTS_CONFIG_H_ */