/*
 * interrupts_config.c
 *
 * Created: 4/23/2017 3:53:03 PM
 *  Author: pvallone
 */ 
 #include "interrupts_config.h"


 void configure_extint_channel(interrupt_conf_t const *config){
	
	struct extint_chan_conf config_extint_chan;
	extint_chan_get_config_defaults(&config_extint_chan);
	config_extint_chan.gpio_pin           = config->gpio_pin;
	config_extint_chan.gpio_pin_mux       = config->gpio_pin_mux;
	config_extint_chan.gpio_pin_pull      = config->gpio_pin_pull;
	config_extint_chan.detection_criteria = config->detection_criteria;
	extint_chan_set_config(config->eic_line, &config_extint_chan);
 }

 
 void configure_extint_callbacks(uint8_t eic_line, const extint_callback_t callback)
 {
	 extint_register_callback(callback,	eic_line ,EXTINT_CALLBACK_TYPE_DETECT);
	 extint_chan_enable_callback(eic_line,EXTINT_CALLBACK_TYPE_DETECT);
 }

